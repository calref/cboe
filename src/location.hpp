/*
 *  location.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef BOE_LOCATION_H
#define BOE_LOCATION_H

#include <SFML/Graphics.hpp>
#include <string>
#include <iosfwd>

enum eDirection {
	DIR_N = 0,
	DIR_NE = 1,
	DIR_E = 2,
	DIR_SE = 3,
	DIR_S = 4,
	DIR_SW = 5,
	DIR_W = 6,
	DIR_NW = 7,
	DIR_HERE = 8,
};

struct rectangle;

struct location {
	int x,y;
	
	location();
	location(int x, int y);
	template<typename T>
	location(sf::Vector2<T> other) : location(other.x, other.y) {}
	bool in(rectangle r) const;
	template<typename T>
	operator typename sf::template Vector2<T>() {
		return sf::Vector2<T>(x,y);
	}
};

struct loc_compare {
	bool operator()(location a, location b) const;
};

class rectangle_size_delegate {
	friend struct rectangle;
	rectangle& forRect;
	int rectangle::* b1;
	int rectangle::* b2;
	rectangle_size_delegate(rectangle& rect, int rectangle::* first, int rectangle::* last);
public:
	operator int() const;
	rectangle_size_delegate& operator=(int val);
	rectangle_size_delegate& operator+=(int val);
	rectangle_size_delegate& operator-=(int val);
	rectangle_size_delegate& operator*=(double val);
	rectangle_size_delegate& operator/=(double val);
	rectangle_size_delegate& operator=(const rectangle_size_delegate& val);
	rectangle_size_delegate& operator+=(const rectangle_size_delegate& val);
	rectangle_size_delegate& operator-=(const rectangle_size_delegate& val);
	rectangle_size_delegate& operator*=(const rectangle_size_delegate& val);
	rectangle_size_delegate& operator/=(const rectangle_size_delegate& val);
};

struct rectangle {
	int top, left, bottom, right;
	
	rectangle();
	rectangle(location tl, location br);
	rectangle(int t, int l, int b, int r);
	explicit rectangle(const sf::Texture& texture);
	explicit rectangle(const sf::RenderTarget& texture);
	template<typename T>
	rectangle(sf::Rect<T> other) : rectangle(other.top, other.left, other.top + other.height, other.left + other.width) {}
	template<typename T>
	rectangle(sf::Vector2<T> size) : rectangle(0, 0, size.y, size.x) {}
	rectangle_size_delegate width();
	rectangle_size_delegate height();
	const rectangle_size_delegate width() const;
	const rectangle_size_delegate height() const;
	location centre() const;
	location topLeft() const;
	location topRight() const;
	location bottomLeft() const;
	location bottomRight() const;
	bool contains(location p) const;
	bool contains(int x, int y) const;
	bool empty() const;
	void offset(int h, int v);
	void offset(location diff);
	void move_to(int x, int y);
	void move_to(location loc);
	template<typename T>
	void offset(sf::Vector2<T> diff) {offset(diff.x,diff.y);}
	void inset(int dh, int dv);
	rectangle& operator&=(rectangle other);
	template<typename T>
	operator typename sf::template Rect<T>() {
		return sf::Rect<T>(left, top, width(), height());
	}
};

struct info_rect_t : public rectangle {
	std::string descr;
	
	info_rect_t(int t, int l, int b, int r, const std::string& s) : rectangle(t,l,b,r), descr(s) {}
	info_rect_t(const rectangle& r) : rectangle(r) {}
	// Declaring one constructor suppresses all implicit constructors, so declare them explicitly
	info_rect_t() = default;
	info_rect_t(const info_rect_t& other) = default;
	// Ditto for assignment operator
	info_rect_t& operator=(const info_rect_t& other) = default;
};

struct sign_loc_t : public location {
	std::string text;
	
	sign_loc_t(int x, int y, const std::string& s) : location(x,y), text(s) {}
	sign_loc_t(const location& loc) : location(loc) {}
	// Declaring one constructor suppresses all implicit constructors, so declare them explicitly
	sign_loc_t() = default;
	sign_loc_t(const sign_loc_t& other) = default;
	// Ditto for assignment operator
	sign_loc_t& operator=(const sign_loc_t& other) = default;
};

struct spec_loc_t : public location {
	long spec;
	
	spec_loc_t() : location(), spec(-1) {}
	spec_loc_t(int x, int y, long sp) : location(x,y), spec(sp) {}
	spec_loc_t(const location& loc) : location(loc), spec(-1) {}
	spec_loc_t& operator=(const location& loc) {
		*this = spec_loc_t(loc);
		spec = -1;
		return *this;
	}
	// Declaring one constructor suppresses all implicit constructors, so declare them explicitly
	spec_loc_t(const spec_loc_t& other) = default;
	// Ditto for assignment operator
	spec_loc_t& operator=(const spec_loc_t& other) = default;
};

eDirection& operator++ (eDirection& me, int);

bool operator == (location p1,location p2);
bool operator != (location p1,location p2);
bool operator == (rectangle r1, rectangle r2);
bool operator != (rectangle r1, rectangle r2);
// TODO: This isn't a union, because it returns a rectangle.
rectangle operator&(rectangle one, rectangle two);
rectangle rectunion(rectangle one, rectangle two);
short dist(location p1,location p2);
float fdist(location p1,location p2);
short vdist(location p1,location p2);

location loc(int a, int b);
location loc();
rectangle rect();
rectangle rect(location tl, location br);
rectangle rect(int top, int left, int bottom, int right);

std::ostream& operator << (std::ostream& out, eDirection e);
std::istream& operator >> (std::istream& in, eDirection& e);
std::ostream& operator<< (std::ostream& out, location l);
std::istream& operator>> (std::istream& in, location& l);
std::ostream& operator<< (std::ostream& out, spec_loc_t l);
std::ostream& operator<< (std::ostream& out, sign_loc_t l);
std::ostream& operator<< (std::ostream& out, rectangle r);
std::istream& operator>> (std::istream& in, rectangle& r);
std::ostream& operator<< (std::ostream& out, info_rect_t r);

const int SCREEN_RADIUS=4;

// CAMERA UTILITY FUNCTIONS

// Whether a tile is within a radius of a given view center.
// 4 is the actual tile radius of the terrain screen.
bool is_on_screen(location loc, location view_center, int radius=SCREEN_RADIUS);

// Default view center: the actual current view center
extern location center;
inline bool is_on_screen(location loc, int radius=SCREEN_RADIUS) {
	return is_on_screen(loc, center, radius);
}

// Calculate the closest point to the center of two anchors that is
// guaranteed to contain the first anchor with the given amount of padding
location between_anchor_points(location anchor1, location anchor2, int padding=1);

// Get all view center points which contain the greatest possible number of the given points
// with the given amount of padding
std::vector<location> points_containing_most(std::vector<location> points, int padding=1);

// Get all view center points which contain the greatest possible number of the given points
// with the given amount of padding, as long as they still contain all of the given required points
std::vector<location> points_containing_most(std::vector<location> points, std::vector<location> required_points, int padding=1);

// Find which of the given points is closest to the given anchor point.
int closest_point_idx(std::vector<location> points, location anchor);
location closest_point(std::vector<location> points, location anchor);

#endif
