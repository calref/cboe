/*
 *  location.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef LOCATION_H
#define LOCATION_H

#include <SFML/Graphics.hpp>

struct rectangle;

struct location {
	int x,y;
	
	location();
	location(int x, int y);
	template<typename T>
	location(sf::Vector2<T> other) : location(other.x, other.y) {}
	bool in(rectangle r);
	template<typename T>
	operator typename sf::template Vector2<T>() {
		return sf::Vector2<T>(x,y);
	}
};

class rectangle_size_delegate {
	friend struct rectangle;
	rectangle& forRect;
	int rectangle::* b1;
	int rectangle::* b2;
	rectangle_size_delegate(rectangle& rect, int rectangle::* first, int rectangle::* last);
public:
	operator int();
	rectangle_size_delegate& operator=(int val);
	rectangle_size_delegate& operator+=(int val);
	rectangle_size_delegate& operator-=(int val);
	rectangle_size_delegate& operator*=(int val);
	rectangle_size_delegate& operator/=(int val);
};

struct rectangle {
	int top, left, bottom, right;
	
	rectangle();
	rectangle(location tl, location br);
	rectangle(int t, int l, int b, int r);
	explicit rectangle(sf::Texture& texture);
	explicit rectangle(sf::RenderTarget& texture);
	template<typename T>
	rectangle(sf::Rect<T> other) : rectangle(other.top, other.left, other.top + other.height, other.left + other.width) {}
	template<typename T>
	rectangle(sf::Vector2<T> size) : rectangle(0, 0, size.y, size.x) {}
	rectangle_size_delegate width();
	rectangle_size_delegate height();
	location centre();
	location topLeft();
	location topRight();
	location bottomLeft();
	location bottomRight();
	bool contains(location p);
	bool contains(int x, int y);
	void offset(int h, int v);
	void offset(location diff);
	template<typename T>
	void offset(sf::Vector2<T> diff) {offset(diff.x,diff.y);}
	void inset(int dh, int dv);
	rectangle& operator|=(rectangle other);
	template<typename T>
	operator typename sf::template Rect<T>() {
		return sf::Rect<T>(left, top, width(), height());
	}
};

using RECT = rectangle; // TODO: A temporary measure, eventually I'll actually replace RECT with rectangle everywhere

bool operator == (location p1,location p2);
bool operator != (location p1,location p2);
bool operator == (rectangle r1, rectangle r2);
bool operator != (rectangle r1, rectangle r2);
rectangle operator|(rectangle one, rectangle two);
rectangle rectunion(rectangle one, rectangle two);
short dist(location p1,location p2);
short vdist(location p1,location p2);

location loc(int a, int b);
location loc();
rectangle rect();
rectangle rect(location tl, location br);
rectangle rect(int top, int left, int bottom, int right);

#endif