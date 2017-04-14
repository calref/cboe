//
//  render_text.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#ifndef BoE_RENDER_TEXT_HPP
#define BoE_RENDER_TEXT_HPP

#include <utility>
#include <vector>
#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "location.hpp"

using hilite_t = std::pair<size_t,size_t>;

enum eFont {
	FONT_PLAIN,
	FONT_BOLD,
	FONT_DUNGEON,
	FONT_MAIDWORD
};

struct TextStyle {
	bool italic = false, underline = false;
	eFont font = FONT_BOLD;
	int pointSize = 10, lineHeight;
	sf::Color colour;
	TextStyle() : colour(sf::Color::Black) {}
	void applyTo(sf::Text& text);
};

struct snippet_t {
	std::string text;
	location at;
	bool hilited;
};

enum class eTextMode {
	WRAP,
	CENTRE,
	LEFT_TOP,
	LEFT_BOTTOM,
};

std::vector<rectangle> draw_string_hilite(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,TextStyle style,std::vector<hilite_t> hilites,sf::Color hiliteClr);
std::vector<snippet_t> draw_string_sel(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,TextStyle style,std::vector<hilite_t> hilites,sf::Color hiliteClr);
void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,eTextMode mode,TextStyle style, location offset = {0,0});
size_t string_length(std::string str, TextStyle style, short* height = nullptr);

#endif
