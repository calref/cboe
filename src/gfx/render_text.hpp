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
#include <tuple>
#include <vector>
#include <string>
#include <cmath>

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
	bool showPipes = false;
	eFont font = FONT_BOLD;
	int pointSize = 10, lineHeight = 10;
	sf::Color colour;
	TextStyle() : colour(sf::Color::Black) {}
	void applyTo(sf::Text& text, double scale = 1.0) const;
	mutable std::map<std::string, location> measurementCache;
};

struct ScaleAwareText {
	sf::Text text;
	rectangle clip_rect;
};

// elements: std::make_tuple(last_line_break, last_word_break, line_width)
typedef std::vector<std::tuple<unsigned short, unsigned short, unsigned short>> break_info_t;

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
	ELLIPSIS,
};

std::vector<rectangle> draw_string_hilite(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,TextStyle style,std::vector<hilite_t> hilites,sf::Color hiliteClr);
std::vector<snippet_t> draw_string_sel(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,TextStyle style,std::vector<hilite_t> hilites,sf::Color hiliteClr);
sf::Vector2f scaled_view_top_left(sf::RenderTarget& dest_window, sf::View& scaled_view);
void draw_scale_aware_text(sf::RenderTarget& dest_window, sf::Text str_to_draw);
void clear_scale_aware_text(sf::RenderTexture& texture);
void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,eTextMode mode,TextStyle style,bool right_align = false);
void win_draw_string(sf::RenderTarget& dest_window,rectangle dest_rect,std::string str,eTextMode mode,TextStyle style, break_info_t break_info,bool right_align = false);
break_info_t calculate_line_wrapping(rectangle dest_rect, std::string str, TextStyle style);
std::string truncate_with_ellipsis(std::string str, const TextStyle& style, int width);
size_t string_length(std::string str, const TextStyle& style, short* height = nullptr);
inline void round_vec(sf::Vector2f& vec) {
	vec.x = std::round(vec.x);
	vec.y = std::round(vec.y);
}

#endif
