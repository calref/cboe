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

#endif
