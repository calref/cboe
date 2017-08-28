//
//  render_shapes.cpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#include "render_shapes.hpp"

#include <cmath>
#include <iostream>
#include <boost/math/constants/constants.hpp>
#include <SFML/OpenGL.hpp>
#include "render_image.hpp"

using boost::math::constants::pi;

// TODO: Put these classes in a header?
class EllipseShape : public sf::Shape {
	float divSz;
	int points;
	float a, b;
public:
	explicit EllipseShape(sf::Vector2f size, std::size_t points = 30) : points(points) {
		a = size.x / 2.0f;
		b = size.y / 2.0f;
		divSz = 2 * pi<float>() / points;
		update();
	}
	
	std::size_t getPointCount() const override {
		return points;
	}
	
	sf::Vector2f getPoint(std::size_t i) const override {
		float t = i * divSz;
		return sf::Vector2f(a + a*sin(t), b + b*cos(t));
	}
	
	// TODO: Additional functions?
};

class RoundRectShape : public sf::Shape {
	float divSz;
	int points;
	float w,h,r;
public:
	RoundRectShape(sf::Vector2f size, float cornerRadius, std::size_t points = 32) : points(points / 4) {
		w = size.x;
		h = size.y;
		r = cornerRadius;
		divSz = 2 * pi<float>() / points;
		update();
	}
	
	std::size_t getPointCount() const override {
		return points * 4;
	}
	
	sf::Vector2f getPoint(std::size_t i) const override {
		const float pi = ::pi<float>();
		const float half_pi = 0.5 * pi;
		float t = i * divSz;
		switch(i / points) {
			case 0: // top left corner
				return {r + r*sinf(t + pi), r + r*cosf(t + pi)};
			case 1: // bottom left corner
				return {r + r*cosf(t + half_pi), h - r + r*sinf(t - half_pi)};
			case 2: // bottom right corner
				return {w - r + r*cosf(t + half_pi), h - r - r*sinf(t + half_pi)};
			case 3: // top right corner
				return {w - r - r*cosf(t - half_pi), r + r*sinf(t - half_pi)};
		}
		// Unreachable
		std::cerr << "Whoops, rounded rectangle had bad point!" << std::endl;
		return {0,0};
	}
	
	// TODO: Additional functions?
};

void draw_line(sf::RenderTarget& target, location from, location to, int thickness, sf::Color colour, sf::BlendMode mode) {
	sf::VertexArray line(sf::LinesStrip, 2);
	line[0].position = from;
	line[0].color = colour;
	line[1].position = to;
	line[1].color = colour;
	setActiveRenderTarget(target);
	float saveThickness;
	glGetFloatv(GL_LINE_WIDTH, &saveThickness);
	glLineWidth(thickness);
	target.draw(line, mode);
	glLineWidth(saveThickness);
}

static void fill_shape(sf::RenderTarget& target, sf::Shape& shape, int x, int y, sf::Color colour) {
	shape.setPosition(x, y);
	shape.setFillColor(colour);
	setActiveRenderTarget(target);
	target.draw(shape);
	
}

static void frame_shape(sf::RenderTarget& target, sf::Shape& shape, int x, int y, sf::Color colour) {
	shape.setPosition(x, y);
	shape.setOutlineColor(colour);
	shape.setFillColor(sf::Color::Transparent);
	// TODO: Determine the correct outline value; should be one pixel, not sure if it should be negative
	shape.setOutlineThickness(-1.0);
	target.draw(shape);
}

void fill_rect(sf::RenderTarget& target, rectangle rect, sf::Color colour) {
	sf::RectangleShape fill(sf::Vector2f(rect.width(), rect.height()));
	fill_shape(target, fill, rect.left, rect.top, colour);
}

void frame_rect(sf::RenderTarget& target, rectangle rect, sf::Color colour) {
	sf::RectangleShape frame(sf::Vector2f(rect.width(), rect.height()));
	frame_shape(target, frame, rect.left, rect.top, colour);
}

void fill_roundrect(sf::RenderTarget& target, rectangle rect, int rad, sf::Color colour) {
	RoundRectShape fill(sf::Vector2f(rect.width(), rect.height()), rad);
	fill_shape(target, fill, rect.left, rect.top, colour);
}

void frame_roundrect(sf::RenderTarget& target, rectangle rect, int rad, sf::Color colour) {
	RoundRectShape frame(sf::Vector2f(rect.width(), rect.height()), rad);
	frame_shape(target, frame, rect.left, rect.top, colour);
}

void fill_circle(sf::RenderTarget& target, rectangle rect, sf::Color colour) {
	EllipseShape fill(sf::Vector2f(rect.width(), rect.height()));
	fill_shape(target, fill, rect.left, rect.top, colour);
}

void frame_circle(sf::RenderTarget& target, rectangle rect, sf::Color colour) {
	EllipseShape frame(sf::Vector2f(rect.width(), rect.height()));
	frame_shape(target, frame, rect.left, rect.top, colour);
}

void fill_region(sf::RenderWindow& target, Region& region, sf::Color colour) {
	clip_region(target, region);
	fill_rect(target, rectangle(target), colour);
	undo_clip(target);
}

void Region::addEllipse(rectangle frame) {
	EllipseShape* ellipse = new EllipseShape(sf::Vector2f(frame.width(), frame.height()));
	ellipse->setFillColor(sf::Color::Black);
	shapes.push_back(std::shared_ptr<sf::Shape>(ellipse));
}

void Region::addRect(rectangle rect){
	sf::RectangleShape* frame = new sf::RectangleShape(sf::Vector2f(rect.width(), rect.height()));
	frame->setPosition(rect.left, rect.top);
	frame->setFillColor(sf::Color::Black);
	shapes.push_back(std::shared_ptr<sf::Shape>(frame));
}

void Region::clear() {
	shapes.clear();
}

void Region::offset(int x, int y) {
	for(auto shape : shapes) {
		shape->move(x,y);
	}
}

void Region::offset(location off) {
	offset(off.x, off.y);
}

rectangle Region::getEnclosingRect() {
	if(shapes.empty()) return rectangle();
	rectangle bounds = shapes[0]->getGlobalBounds();
	for(size_t i = 0; i < shapes.size(); i++) {
		rectangle shapeRect = shapes[i]->getGlobalBounds();
		if(shapeRect.top < bounds.top) bounds.top = shapeRect.top;
		if(shapeRect.left < bounds.left) bounds.top = shapeRect.top;
		if(shapeRect.bottom > bounds.bottom) bounds.top = shapeRect.top;
		if(shapeRect.right > bounds.right) bounds.top = shapeRect.top;
	}
	return bounds;
}

// We can only use stencil buffer in the main window
// Could request it in dialogs, but currently don't
// SFML does not appear to allow requesting it for render textures
void Region::setStencil(sf::RenderWindow& where) {
	where.setActive();
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	for(auto shape : shapes) {
		// Save the colour in case we need to reuse this region
		sf::Color colour = shape->getFillColor();
		if(colour == sf::Color::Black)
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		else glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
		// Make transparent so we don't overwrite important stuff
		shape->setFillColor(sf::Color::Transparent);
		where.draw(*shape);
		shape->setFillColor(colour);
	}
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void clip_rect(sf::RenderTarget& where, rectangle rect) {
	rect &= rectangle(where); // Make sure we don't draw out of bounds
	// TODO: Make sure this works for the scissor test...
	setActiveRenderTarget(where);
	glEnable(GL_SCISSOR_TEST);
	glScissor(rect.left, rectangle(where).height() - rect.bottom, rect.width(), rect.height());
}

void clip_region(sf::RenderWindow& where, Region& region) {
	region.setStencil(where);
}

void undo_clip(sf::RenderTarget& where) {
	setActiveRenderTarget(where);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);
}

Region& Region::operator-=(Region& other) {
	for(auto shape : other.shapes) {
		// TODO: Shouldn't this be done to a copy of the shape instead?
		if(shape->getFillColor() == sf::Color::Black)
			shape->setFillColor(sf::Color::White);
		else shape->setFillColor(sf::Color::Black);
		shapes.push_back(shape);
	}
	return *this;
}
