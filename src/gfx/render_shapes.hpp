//
//  render_shapes.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#ifndef BoE_RENDER_SHAPES_HPP
#define BoE_RENDER_SHAPES_HPP

#include <vector>
#include <memory>
#include <map>
#include <SFML/Graphics/Shape.hpp>
#include "location.hpp"

class Region {
	std::vector<std::shared_ptr<sf::Shape>> shapes;
	void setStencil(sf::RenderWindow& where);
	friend void clip_region(sf::RenderWindow& where, Region& region);
public:
	void addEllipse(rectangle frame);
	void addRect(rectangle rect);
	void clear();
	void offset(int x, int y);
	void offset(location off);
	Region& operator-=(Region& other);
	rectangle getEnclosingRect();
};

void fill_rect(sf::RenderTarget& target, rectangle rect, sf::Color colour);
void fill_circle(sf::RenderTarget& target, rectangle rect, sf::Color colour);
void fill_roundrect(sf::RenderTarget& target, rectangle rect, int rad, sf::Color colour);
void fill_region(sf::RenderWindow& target, Region& region, sf::Color colour);

void frame_rect(sf::RenderTarget& target, rectangle rect, sf::Color colour);
void frame_circle(sf::RenderTarget& target, rectangle rect, sf::Color colour);
void frame_roundrect(sf::RenderTarget& target, rectangle rect, int rad, sf::Color colour);

void draw_line(sf::RenderTarget& target, location from, location to, int thickness, sf::Color colour, sf::BlendMode mode = sf::BlendNone);

void clip_rect(sf::RenderTarget& where, rectangle rect);
void clip_region(sf::RenderWindow& where, Region& region);
void undo_clip(sf::RenderTarget& where);

// reference colours for BoE's UI

namespace Colours {
	const sf::Color WHITE = sf::Color::White;
	const sf::Color BLACK = sf::Color::Black;
	const sf::Color GREY   { 0x80, 0x80, 0x80};
	const sf::Color RED    { 0xdd, 0x00, 0x00};
	const sf::Color GREEN  { 0x00, 0x88, 0x00};
	const sf::Color BLUE   { 0x00, 0x00, 0xdd};
	const sf::Color TEAL   { 0x59, 0x97, 0x8f};
	const sf::Color PINK   { 0xff, 0x00, 0x99};
	const sf::Color YELLOW { 0xff, 0xff, 0x31};
	const sf::Color ORANGE { 0xff, 0x80, 0x00};
	const sf::Color LIGHT_BLUE { 0xad, 0xd8, 0xe6 }; // Spell points on dark background
	// On Windows, TRANSPARENT is macro-defined as something...?
	const sf::Color EMPTY { 0x00, 0x00, 0x00, 0x00 };
	// Text colours for shopping / talking
	// TODO: The Windows version appears to use completely different colours?
	const sf::Color SHADOW      { 0x00, 0x00, 0x68}; // formerly c[3] QD colour = {0,0,26623} (shop/character name shadow, shop subtitle)
	const sf::Color TITLE_BLUE  { 0x00, 0x00, 0xe8}; // formerly c[4] QD colour = {0,0,59391} (shop/character name)
	const sf::Color NAVY        { 0x00, 0x00, 0x39}; // formerly c[2] QD colour = {0,0,14535} (talking text)
	const sf::Color DARK_BLUE   { 0x00, 0x00, 0x80}; // formerly c[1] QD colour = {0,0,32767} (talking text pressed)
	const sf::Color DARK_GREEN  { 0x00, 0x60, 0x00}; // formerly c[5] QD colour = {0,40959,0} (talking buttons)
	const sf::Color LIGHT_GREEN { 0x00, 0xa0, 0x00}; // formerly c[6] QD colour = {0,24575,0} (talking buttons pressed)
	const sf::Color DARK_RED    { 0xa0, 0x00, 0x14}; // formerly c[7] (clickable text, new in OBoE)
	// Extra colors
	const sf::Color LIME { 0x00, 0xFF, 0x00};
	const sf::Color AQUA { 0x00, 0xFF, 0xFF};
	const sf::Color FUCHSIA { 0xFF, 0x00, 0xFF};
	const sf::Color MAROON { 0x80, 0x00, 0x00};
	const sf::Color OLIVE { 0x80, 0x80, 0x00};
	const sf::Color PURPLE { 0x80, 0x00, 0x80};
	const sf::Color SILVER { 0xC0, 0xC0, 0xC0};
}

const sf::Color PRESET_WORD_ON = Colours::DARK_GREEN;
const sf::Color PRESET_WORD_OFF = Colours::LIGHT_GREEN;

const sf::Color CUSTOM_WORD_ON = Colours::DARK_BLUE;
const sf::Color CUSTOM_WORD_OFF = Colours::DARK_RED;


#endif
