//
//  render_text.cpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#include "render_text.hpp"

#include <iostream>
#include <set>
#include <boost/algorithm/string/replace.hpp>
#include "fileio/resmgr/res_font.hpp"
#include "gfx/render_shapes.hpp"
#include <utility>
#include "winutil.hpp"

sf::Font& get_font_rsrc(eFont font) {
	switch(font) {
		case FONT_PLAIN:
			return *ResMgr::fonts.get("plain");
		case FONT_BOLD:
			return *ResMgr::fonts.get("bold");
		case FONT_DUNGEON:
			return *ResMgr::fonts.get("dungeon");
		case FONT_MAIDWORD:
			return *ResMgr::fonts.get("maidenword");
	}
}

void TextStyle::applyTo(sf::Text& text, double scale) const {
	// Guarantee the font texture for the needed text size won't need to be resized in the middle of rendering
	sf::Font& font_obj = get_font_rsrc(font);
	int size = pointSize * scale;
	sf::Texture& font_texture = const_cast<sf::Texture&>(font_obj.getTexture(size));
	int min_texture_size = 128 * scale;
	if(font_texture.getSize().x < min_texture_size){
		int texture_scale = 2;
		while(128 * texture_scale < min_texture_size){
			texture_scale << 1;
		}
		if(!font_texture.create(128 * texture_scale, 128 * texture_scale)){
			throw std::string { "Failed to create large enough font texture!" };
		}
	}

	text.setFont(font_obj);
	text.setCharacterSize(size);
	int style = sf::Text::Regular;
	if(italic) style |= sf::Text::Italic;
	if(underline) style |= sf::Text::Underlined;
	text.setStyle(style);
	text.setColor(colour);
}

struct text_params_t {
	TextStyle style;
	eTextMode mode;
	bool showBreaks = false;
	// Hilite ranges are, like the STL, of the form [first, last).
	std::vector<hilite_t> hilite_ranges;
	sf::Color hilite_fg, hilite_bg = sf::Color::Transparent;
	enum {RECTS, SNIPPETS} returnType;
	std::vector<rectangle> returnRects;
	std::vector<snippet_t> snippets;
	// Pre-calculated line wrapping:
	break_info_t break_info;
	bool right_align = false;
};

static void push_snippets(size_t start, size_t end, text_params_t& options, size_t& iHilite, const std::string& str, location loc) {
	std::vector<hilite_t>& hilites = options.hilite_ranges;
	std::vector<snippet_t>& snippets = options.snippets;
	// Check if we have any hilites on this line.
	// We assume the list of hilites is sorted, so we just need to
	// check the current entry.
	size_t upper_bound = end;
	do {
		bool hilited = false;
		// Skip any hilite rules that have start = end
		while(iHilite < hilites.size() && hilites[iHilite].first == hilites[iHilite].second)
			iHilite++;
		if(iHilite < hilites.size()) {
			// Possibilities: (vertical bars indicate line boundaries, parentheses for hilite boundaries, dots are data)
			// 1.  |...|...(...) --> no hilite on this line
			// 2a. |...(...|...) --> hilite starts on this line and continues past it
			// 2b. |...(...)...| --> hilite starts and ends on this line
			// 3a. (...|...)...| --> hilite starts (or continues) at the start of the line and ends on this line
			// 3b. (...|......)| --> entire line hilited
			// Case 1 needs no special handling; check case 3, then case 2.
			if(hilites[iHilite].first <= start) {
				hilited = true;
				if(hilites[iHilite].second < end) // 3a
					end = hilites[iHilite].second;
			} else if(hilites[iHilite].first < end)
				end = hilites[iHilite].first;
			// 2b will reduce to 3a after shifting start over
		}
		size_t amount = end - start;
		snippets.push_back({str.substr(start,amount), loc, hilited});
//		if(hilited) std::cout << "Hiliting passage : \"" << snippets.back().text << '"' << std::endl;
		loc.x += string_length(snippets.back().text, options.style);
		start = end;
		end = upper_bound;
		if(iHilite < hilites.size() && start >= hilites[iHilite].second)
			iHilite++;
	} while(start < upper_bound);
}

break_info_t calculate_line_wrapping(rectangle dest_rect, std::string str, TextStyle style) {
	break_info_t break_info;
	if(str.empty()) return break_info; // Nothing to do!

	sf::Text str_to_draw;
	style.applyTo(str_to_draw);
	short str_len = str.length();
	unsigned short last_line_break = 0,last_word_break = 0;

	str_to_draw.setString(str);

	// Even if the text is only one line, break_info is required for calculating word boundaries.
	// So we can't skip the rest of this.

	short len = 0;
	std::vector<short> lengths;
	sf::Font& font = get_font_rsrc(style.font);

	// getGlyph() requires Uint32 input, which I don't think char is guaranteed to be
	const sf::String& sf_str = str_to_draw.getString();
	// if i > 0, this lambda must be called with i-1 before calling it with i for the first time
	auto text_len = [&sf_str, &str_len, &font, &style, &len, &lengths](size_t i) -> int {
		if(i < lengths.size()){
			return lengths[i];
		}

		short ret = len;

		if(i < str_len){
			if(i > 0){
				len += font.getKerning(sf_str[i - 1], sf_str[i], style.pointSize);
			}
			len += font.getGlyph(sf_str[i], style.pointSize, false).advance;
		}

		// Memoize this function because it is destructive:
		lengths.push_back(ret);
		return ret;
	};

	short i;
	for(i = 0; text_len(i) != text_len(i + 1) && i < str_len; i++) {
		unsigned short line_width = text_len(i) - text_len(last_line_break);
		if(((line_width > (dest_rect.width() - 6))
			&& (last_word_break >= last_line_break)) || (str[i] == '|' && !style.showPipes)) {
			if(str[i] == '|') {
				last_word_break = i + 1;
			} else if(last_line_break == last_word_break)
				last_word_break = i;
			break_info.push_back(std::make_tuple(last_line_break, last_word_break, line_width));
			last_line_break = last_word_break;
		}
		if(str[i] == ' ')
			last_word_break = i + 1;
	}

	if(i - last_line_break > 0) {
		unsigned short line_width = text_len(i) - text_len(last_line_break);
		std::string snippet = str.substr(last_line_break);
		if(!snippet.empty())
			break_info.push_back(std::make_tuple(last_line_break, str.length() + 1, line_width));
	}

	return break_info;
}

// I don't know of a better way to do this than using pointers as keys.
extern std::map<sf::RenderTexture*,std::vector<ScaleAwareText>> store_scale_aware_text;
extern std::map<sf::RenderTexture*,rectangle> store_clip_rects;

void clear_scale_aware_text(sf::RenderTexture& texture) {
	store_scale_aware_text.erase(&texture);
}

// Offset the scale-aware text by the viewport top-left corner of the current
// window mode:
sf::Vector2f scaled_view_top_left(sf::RenderTarget& dest_window, sf::View& scaled_view) {
	sf::FloatRect viewport = scaled_view.getViewport();
	rectangle windRect { dest_window };
	sf::Vector2f top_left(viewport.left * windRect.width(), viewport.top * windRect.height());
	return top_left;
}

void draw_scale_aware_text(sf::RenderTarget& dest_window, sf::Text str_to_draw) {
	if(dynamic_cast<sf::RenderWindow*>(&dest_window) != nullptr){
		// Temporarily switch window to its unscaled view to draw scale-aware text
		sf::View scaled_view = dest_window.getView();
		dest_window.setView(dest_window.getDefaultView());

		sf::Vector2f text_position = str_to_draw.getPosition() * (float)get_ui_scale();
		text_position += scaled_view_top_left(dest_window, scaled_view);
		// Rounding to whole-number positions *might* avoid unpredictable graphics bugs:
		round_vec(text_position);
		str_to_draw.setPosition(text_position);

		// Draw the text immediately
		dest_window.draw(str_to_draw);

		// Restore the scaled view
		dest_window.setView(scaled_view);
	}else if(dynamic_cast<sf::RenderTexture*>(&dest_window) != nullptr){
		sf::RenderTexture* p = dynamic_cast<sf::RenderTexture*>(&dest_window);
		// Onto a RenderTexture is trickier because the texture is locked at the smaller size.
		// What we can do is save the sf::Text with its relative position and render
		// it onto the RenderWindow that eventually draws the RenderTexture.
		if(store_scale_aware_text.find(p) == store_scale_aware_text.end()){
			store_scale_aware_text[p] = std::vector<ScaleAwareText> {};
		}
		ScaleAwareText text = { str_to_draw, {}};
		if(store_clip_rects.find(p) != store_clip_rects.end()){
			text.clip_rect = store_clip_rects[p];
		}
		store_scale_aware_text[p].push_back(text);
	}
}

std::string truncate_with_ellipsis(std::string str, const TextStyle& style, int width) {
	size_t length = string_length(str, style);
	if(length > width){
		size_t ellipsis_length = string_length("...", style);

		size_t need_to_clip = length - width + ellipsis_length;
		int clip_idx = str.size() - 1;
		// clip_idx should never reach 0
		for(; clip_idx >= 0; --clip_idx){
			size_t clip_length = string_length(str.substr(clip_idx), style);
			if(clip_length >= need_to_clip) break;
		}
		str = str.substr(0, clip_idx) + "...";
	}
	return str;
}

std::map<std::string, std::string> substitutions = {
	{"–", "--"}
};

static void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,text_params_t& options) {
	if(str.empty()) return; // Nothing to do!
	short line_height = options.style.lineHeight;
	sf::Text str_to_draw;
	options.style.applyTo(str_to_draw);
	short adjust_x = 1, adjust_y = 10;
	
	str_to_draw.setString("fj"); // Something that has both an ascender and a descender
	// TODO: Why the heck are we drawing a whole line higher than requested!?
	adjust_y -= str_to_draw.getLocalBounds().height;
	
	for(auto it : substitutions){
		boost::replace_all(str, it.first, it.second);
	}
	str_to_draw.setString(str);
	short total_width = str_to_draw.getLocalBounds().width;

	options.style.applyTo(str_to_draw, get_ui_scale());
	
	eTextMode mode = options.mode;
	if(mode == eTextMode::WRAP && total_width < dest_rect.width() && !options.right_align)
		mode = eTextMode::LEFT_TOP;
	if(mode == eTextMode::LEFT_TOP && !options.style.showPipes && str.find('|') != std::string::npos)
		mode = eTextMode::WRAP;
	
	// Special stuff
	size_t iHilite = 0;
	
	location moveTo;
	line_height -= 2; // TODO: ...why are we arbitrarily reducing the line height from the requested value?

	// Pipes need to be left in the string that gets passed
	// to calculate_line_wrapping() or frame calculation is
	// broken.
	std::string str_with_pipes = str;
	if(!options.showBreaks && !options.style.showPipes){
		for(int i=0; i < str.length(); ++i){
			if(str[i] == '|') str[i] = ' ';
		}
	}

	if(mode == eTextMode::ELLIPSIS){
		str = truncate_with_ellipsis(str, options.style, dest_rect.width());
		mode = eTextMode::LEFT_TOP;
	}
	if(mode == eTextMode::WRAP){
		break_info_t break_info = options.break_info;

		// It is better to pre-calculate line-wrapping and pass it in the options,
		// but if you don't, this will handle line-wrapping every frame:
		if(break_info.empty()){
			break_info = calculate_line_wrapping(dest_rect, str_with_pipes, options.style);
		}

		moveTo = location(dest_rect.left + adjust_x, dest_rect.top + adjust_y);

		for(auto break_info_tuple : break_info){
			if(options.right_align){
				moveTo.x += (dest_rect.width() - std::get<2>(break_info_tuple));
			}
			push_snippets(std::get<0>(break_info_tuple), std::get<1>(break_info_tuple), options, iHilite, str, moveTo);
			moveTo.y += line_height;
			moveTo.x = dest_rect.left;
		}
	} else {
		switch(mode) {
			// TODO: CENTRE and LEFT_BOTTOM don't work with text rect calculation...
			// Low priority, since the API doesn't even offer a way to do this.
			case eTextMode::CENTRE:
				// TODO: Calculate adjust_x/y directly instead...
				moveTo = location((dest_rect.right + dest_rect.left) / 2 - (4 * total_width) / 9,
								  (dest_rect.bottom + dest_rect.top - line_height) / 2 + adjust_y - 1);
				adjust_x = moveTo.x - dest_rect.left;
				adjust_y = moveTo.y - dest_rect.top;
				break;
			case eTextMode::LEFT_TOP:
				moveTo = location(dest_rect.left + adjust_x, dest_rect.top + adjust_y);
				break;
			case eTextMode::LEFT_BOTTOM:
				moveTo = location(dest_rect.left + adjust_x, dest_rect.top + adjust_y + dest_rect.height() / 6);
				break;
			case eTextMode::WRAP:
				break; // Never happens, but put this here to silence warning
		}
		push_snippets(0, str.length() + 1, options, iHilite, str, moveTo);
	}
	
	for(auto& snippet : options.snippets) {
		str_to_draw.setString(snippet.text);
		str_to_draw.setPosition(snippet.at);
		if(snippet.hilited) {
			rectangle bounds = str_to_draw.getGlobalBounds();
			// Width and height are too large by ui_scale because scale-aware text exists in unscaled space:
			bounds.width() /= get_ui_scale();
			bounds.height() /= get_ui_scale();

			// Adjust so that drawing the same text to
			// the same rect is positioned exactly right
			bounds.move_to(snippet.at);
			bounds.offset(-adjust_x, -adjust_y);
			if(options.returnType == text_params_t::RECTS)
				options.returnRects.push_back(bounds);
			str_to_draw.setColor(options.hilite_fg);
			bounds.inset(0,-4);
			fill_rect(dest_window, bounds, options.hilite_bg);
		} else str_to_draw.setColor(options.style.colour);
		draw_scale_aware_text(dest_window, str_to_draw);
	}
}

void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,eTextMode mode,TextStyle style,bool right_align) {
	break_info_t break_info;
	win_draw_string(dest_window, dest_rect, str, mode, style, break_info, right_align);
}

void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,eTextMode mode,TextStyle style,break_info_t break_info,bool right_align) {
	text_params_t params;
	params.mode = mode;
	params.style = style;
	params.break_info = break_info;
	params.right_align = right_align;
	win_draw_string(dest_window, dest_rect, str, params);
}

std::vector<rectangle> draw_string_hilite(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,TextStyle style,std::vector<hilite_t> hilites,sf::Color hiliteClr) {
	text_params_t params;
	params.mode = eTextMode::WRAP;
	params.hilite_ranges = hilites;
	params.style = style;
	params.hilite_fg = hiliteClr;
	params.returnType = text_params_t::RECTS;
	win_draw_string(dest_window, dest_rect, str, params);
	return params.returnRects;
}

std::vector<snippet_t> draw_string_sel(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,TextStyle style,std::vector<hilite_t> hilites,sf::Color hiliteClr) {
	text_params_t params;
	params.mode = eTextMode::WRAP;
	params.showBreaks = true;
	params.hilite_ranges = hilites;
	params.style = style;
	params.hilite_fg = style.colour;
	params.hilite_bg = hiliteClr;
	params.returnType = text_params_t::RECTS;
	win_draw_string(dest_window, dest_rect, str, params);
	return params.snippets;
}

std::set<std::string> strings_to_cache = {" ", "..."};

size_t string_length(std::string str, const TextStyle& style, short* height){
	size_t total_width = 0;
	if(style.measurementCache.find(str) != style.measurementCache.end()){
		location measurement = style.measurementCache[str];
		if(height) *height = measurement.y;
		return measurement.x;
	}
	
	sf::Text text;
	style.applyTo(text);
	text.setString(str);
	total_width = text.getLocalBounds().width;
	if(strings_to_cache.count(str)){
		location measurement;
		measurement.x = total_width;
		measurement.y = text.getLocalBounds().height;
		style.measurementCache[str] = measurement;
	}
	if(height) *height = text.getLocalBounds().height;
	return total_width;
}
