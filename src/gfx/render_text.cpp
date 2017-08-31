//
//  render_text.cpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#include "render_text.hpp"

#include "res_font.hpp"
#include "render_shapes.hpp"

void TextStyle::applyTo(sf::Text& text) {
	switch(font) {
		case FONT_PLAIN:
			text.setFont(*ResMgr::get<FontRsrc>("plain"));
			break;
		case FONT_BOLD:
			text.setFont(*ResMgr::get<FontRsrc>("bold"));
			break;
		case FONT_DUNGEON:
			text.setFont(*ResMgr::get<FontRsrc>("dungeon"));
			break;
		case FONT_MAIDWORD:
			text.setFont(*ResMgr::get<FontRsrc>("maidenword"));
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
	location offset = {0,0};
	// Hilite ranges are, like the STL, of the form [first, last).
	std::vector<hilite_t> hilite_ranges;
	sf::Color hilite_fg, hilite_bg = sf::Color::Transparent;
	enum {RECTS, SNIPPETS} returnType;
	std::vector<rectangle> returnRects;
	std::vector<snippet_t> snippets;
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
		loc.x += string_length(snippets[snippets.size()-1].text, options.style);
		start = end;
		end = upper_bound;
		if(iHilite < hilites.size() && start >= hilites[iHilite].second)
			iHilite++;
	} while(start < upper_bound);
}

static void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,text_params_t& options) {
	if(str.empty()) return; // Nothing to do!
	short line_height = options.style.lineHeight;
	sf::Text str_to_draw;
	options.style.applyTo(str_to_draw);
	short str_len;
	unsigned short last_line_break = 0,last_word_break = 0;
	short total_width = 0;
	short adjust_x = 0,adjust_y = 0;
	
	adjust_x = options.offset.x;
	adjust_y = options.offset.y;
	str_to_draw.setString("fj"); // Something that has both an ascender and a descender
	adjust_y -= str_to_draw.getLocalBounds().height;
	
	str_to_draw.setString(str);
	str_len = str.length();
	if(str_len == 0) {
		return;
	}
	
	eTextMode mode = options.mode;
	total_width = str_to_draw.getLocalBounds().width;
	if(mode == eTextMode::WRAP && total_width < dest_rect.width())
		mode = eTextMode::LEFT_TOP;
	if(mode == eTextMode::LEFT_TOP && str.find('|') != std::string::npos)
		mode = eTextMode::WRAP;
	
	auto text_len = [&str_to_draw](size_t i) -> int {
		return str_to_draw.findCharacterPos(i).x;
	};
	
	// Special stuff
	size_t iHilite = 0;
	
	location moveTo;
	line_height -= 2; // TODO: ...why are we arbitrarily reducing the line height from the requested value?
	
	if(mode == eTextMode::WRAP) {
		moveTo = location(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + adjust_y + 9);
		short i;
		for(i = 0; text_len(i) != text_len(i + 1) && i < str_len; i++) {
			if(((text_len(i) - text_len(last_line_break) > (dest_rect.width() - 6))
				&& (last_word_break >= last_line_break)) || (str[i] == '|')) {
				if(str[i] == '|') {
					if(!options.showBreaks) str[i] = ' ';
					last_word_break = i + 1;
				} else if(last_line_break == last_word_break)
					last_word_break = i;
				push_snippets(last_line_break, last_word_break, options, iHilite, str, moveTo);
				moveTo.y += line_height;
				last_line_break = last_word_break;
			}
			if(str[i] == ' ')
				last_word_break = i + 1;
		}
		
		if(i - last_line_break > 0) {
			std::string snippet = str.substr(last_line_break);
			if(!snippet.empty())
				push_snippets(last_line_break, str.length() + 1, options, iHilite, str, moveTo);
		}
	} else {
		switch(mode) {
			case eTextMode::CENTRE:
				moveTo = location((dest_rect.right + dest_rect.left) / 2 - (4 * total_width) / 9 + adjust_x,
								  (dest_rect.bottom + dest_rect.top - line_height) / 2 + 9 + adjust_y);
				break;
			case eTextMode::LEFT_TOP:
				moveTo = location(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + adjust_y + 9);
				break;
			case eTextMode::LEFT_BOTTOM:
				moveTo = location(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + adjust_y + 9 + dest_rect.height() / 6);
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
			bounds.left = snippet.at.x - 1;
			bounds.top = snippet.at.y + 5;
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

void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,eTextMode mode,TextStyle style, location offset) {
	text_params_t params;
	params.mode = mode;
	params.style = style;
	params.offset = offset;
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
