//
//  render_text.cpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#include "render_text.hpp"

#include <iostream>
#include "fileio/resmgr/res_font.hpp"
#include "gfx/render_shapes.hpp"
#include <utility>

void TextStyle::applyTo(sf::Text& text) {
	switch(font) {
		case FONT_PLAIN:
			text.setFont(*ResMgr::fonts.get("plain"));
			break;
		case FONT_BOLD:
			text.setFont(*ResMgr::fonts.get("bold"));
			break;
		case FONT_DUNGEON:
			text.setFont(*ResMgr::fonts.get("dungeon"));
			break;
		case FONT_MAIDWORD:
			text.setFont(*ResMgr::fonts.get("maidenword"));
			break;
	}
	text.setCharacterSize(pointSize);
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
	short total_width = str_to_draw.getLocalBounds().width;

	if(total_width < dest_rect.width()){
		// The text fits on one line, so break_info won't end up being used by win_draw_string anyway
		return break_info;
	}

	auto text_len = [&str_to_draw](size_t i) -> int {
		return str_to_draw.findCharacterPos(i).x;
	};

	short i;
	for(i = 0; text_len(i) != text_len(i + 1) && i < str_len; i++) {
		if(((text_len(i) - text_len(last_line_break) > (dest_rect.width() - 6))
			&& (last_word_break >= last_line_break)) || (str[i] == '|')) {
			if(str[i] == '|') {
				last_word_break = i + 1;
			} else if(last_line_break == last_word_break)
				last_word_break = i;
			break_info.push_back(std::make_pair(last_line_break, last_word_break));
			last_line_break = last_word_break;
		}
		if(str[i] == ' ')
			last_word_break = i + 1;
	}

	if(i - last_line_break > 0) {
		std::string snippet = str.substr(last_line_break);
		if(!snippet.empty())
			break_info.push_back(std::make_pair(last_line_break, str.length() + 1));
	}

	return break_info;
}

static void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,text_params_t& options) {
	if(str.empty()) return; // Nothing to do!
	short line_height = options.style.lineHeight;
	sf::Text str_to_draw;
	options.style.applyTo(str_to_draw);
	short adjust_x = 1, adjust_y = 10;
	
	str_to_draw.setString("fj"); // Something that has both an ascender and a descender
	// TODO: Why the heck are we drawing a whole line higher than requested!?
	adjust_y -= str_to_draw.getLocalBounds().height;
	
	str_to_draw.setString(str);
	short total_width = str_to_draw.getLocalBounds().width;
	
	eTextMode mode = options.mode;
	if(mode == eTextMode::WRAP && total_width < dest_rect.width())
		mode = eTextMode::LEFT_TOP;
	if(mode == eTextMode::LEFT_TOP && str.find('|') != std::string::npos)
		mode = eTextMode::WRAP;
	
	// Special stuff
	size_t iHilite = 0;
	
	location moveTo;
	line_height -= 2; // TODO: ...why are we arbitrarily reducing the line height from the requested value?

	if(!options.showBreaks){
		for(int i=0; i < str.length(); ++i){
			if(str[i] == '|') str[i] = ' ';
		}
	}

	if(mode == eTextMode::WRAP) {
		break_info_t break_info = options.break_info;

		// It is better to pre-calculate line-wrapping and pass it in the options,
		// but if you don't, this will handle line-wrapping every frame:
		if(break_info.empty()){
			break_info = calculate_line_wrapping(dest_rect, str, options.style);
		}

		moveTo = location(dest_rect.left + adjust_x, dest_rect.top + adjust_y);

		for(auto break_info_pair : break_info){
			push_snippets(break_info_pair.first, break_info_pair.second, options, iHilite, str, moveTo);
			moveTo.y += line_height;
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
		dest_window.draw(str_to_draw);
		if(options.style.font == FONT_BOLD) {
			str_to_draw.move(1, 0);
			dest_window.draw(str_to_draw);
		}
	}
}

void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,eTextMode mode,TextStyle style) {
	break_info_t break_info;
	win_draw_string(dest_window, dest_rect, str, mode, style, break_info);
}

void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,eTextMode mode,TextStyle style,break_info_t break_info) {
	text_params_t params;
	params.mode = mode;
	params.style = style;
	params.break_info = break_info;
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

size_t string_length(std::string str, TextStyle style, short* height){
	size_t total_width = 0;
	
	sf::Text text;
	style.applyTo(text);
	text.setString(str);
	total_width = text.getLocalBounds().width;
	if(height) *height = text.getLocalBounds().height;
	return total_width;
}
