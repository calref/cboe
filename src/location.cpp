/*
 *  location.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "location.hpp"
#include "mathutil.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <limits>

eDirection& operator++ (eDirection& me, int) {
	if(me == DIR_HERE) return me = DIR_N;
	else return me = (eDirection)(1 + (int)me);
}

bool operator == (location p1,location p2){
	if((p1.x == p2.x) & (p1.y == p2.y))
		return true;
	else return false;
}

bool operator != (location p1,location p2){
	return ! (p1 == p2);
}

short dist(location p1,location p2){
	return hypot(p1.x - p2.x, p1.y - p2.y);
}

float fdist(location p1,location p2){
	return hypot(p1.x - p2.x, p1.y - p2.y);
}

short vdist(location p1,location p2) {
	short i,j;
	i = abs((long double)p1.x - p2.x);
	j = abs((long double)p1.y - p2.y);
	return max(i,j);
}

location::location() : x(0), y(0) {}
location::location(int a,int b) : x(a), y(b) {}

location loc(int a, int b){
	return location(a,b);
}

location loc(){
	return location();
}

bool loc_compare::operator()(location a, location b) const {
	// This is just a lexicographical ordering.
	if(a.x != b.x) return a.x < b.x;
	if(a.y != b.y) return a.y < b.y;
	return false;
}

bool location::in(rectangle r) const {
	if(y >= r.top && y <= r.bottom && x >= r.left && x <= r.right)
		return true;
	return false;
}

bool operator == (rectangle r1, rectangle r2) {
	if(r1.top == r2.top && r1.left == r2.left && r1.bottom == r2.bottom && r1.right == r2.right)
		return true;
	return false;
}

bool operator != (rectangle r1, rectangle r2) {
	return !(r1 == r2);
}

rectangle operator&(rectangle one, rectangle two) {
	return one &= two;
}

rectangle rectunion(rectangle one, rectangle two) {
	int left = min(one.left, two.left);
	int right = max(one.right, two.right);
	int top = min(one.top, two.top);
	int bottom = max(one.bottom, two.bottom);
	return rectangle(top, left, bottom, right);
}

rectangle::rectangle() : top(0), left(0), right(0), bottom(0) {}
rectangle::rectangle(location tl, location br) : top(tl.y), left(tl.x), right(br.x), bottom(br.y) {}
rectangle::rectangle(int t, int l, int b, int r) : top(t), left(l), right(r), bottom(b) {}
rectangle::rectangle(const sf::Texture& texture) : top(0), left(0), right(texture.getSize().x), bottom(texture.getSize().y) {}
rectangle::rectangle(const sf::RenderTarget& texture) : top(0), left(0), right(texture.getSize().x), bottom(texture.getSize().y) {}

bool rectangle::contains(location p) const {
	if(p.y >= top && p.y <= bottom && p.x >= left && p.x <= right)
		return true;
	return false;
}

bool rectangle::contains(int x, int y) const {
	return contains(location(x,y));
}

bool rectangle::empty() const {
	if(right < left || bottom < top) return true;
	return height() == 0 && width() == 0;
}

rectangle_size_delegate::rectangle_size_delegate(rectangle& rect, int rectangle::* first, int rectangle::* last) :
	forRect(rect), b1(first), b2(last) {}

rectangle_size_delegate::operator int() const {
	return forRect.*b2 - forRect.*b1;
}

rectangle_size_delegate& rectangle_size_delegate::operator=(int val) {
	forRect.*b2 = forRect.*b1 + val;
	return *this;
}

rectangle_size_delegate& rectangle_size_delegate::operator+=(int val) {
	*this = *this + val;
	return *this;
}

rectangle_size_delegate& rectangle_size_delegate::operator-=(int val) {
	*this = *this - val;
	return *this;
}

rectangle_size_delegate& rectangle_size_delegate::operator*=(double val) {
	*this = *this * val;
	return *this;
}

rectangle_size_delegate& rectangle_size_delegate::operator/=(double val) {
	*this = *this / val;
	return *this;
}

rectangle_size_delegate& rectangle_size_delegate::operator=(const rectangle_size_delegate& val) {
	return operator=(int(val));
}

rectangle_size_delegate& rectangle_size_delegate::operator+=(const rectangle_size_delegate& val) {
	return operator+=(int(val));
}

rectangle_size_delegate& rectangle_size_delegate::operator-=(const rectangle_size_delegate& val) {
	return operator-=(int(val));
}

rectangle_size_delegate& rectangle_size_delegate::operator*=(const rectangle_size_delegate& val) {
	return operator*=(int(val));
}

rectangle_size_delegate& rectangle_size_delegate::operator/=(const rectangle_size_delegate& val) {
	return operator/=(int(val));
}

rectangle_size_delegate rectangle::width() {
	return rectangle_size_delegate(*this, &rectangle::left, &rectangle::right);
}

rectangle_size_delegate rectangle::height() {
	return rectangle_size_delegate(*this, &rectangle::top, &rectangle::bottom);
}

const rectangle_size_delegate rectangle::width() const {
	return rectangle_size_delegate(*const_cast<rectangle*>(this), &rectangle::left, &rectangle::right);
}

const rectangle_size_delegate rectangle::height() const {
	return rectangle_size_delegate(*const_cast<rectangle*>(this), &rectangle::top, &rectangle::bottom);
}

location rectangle::centre() const {
	return location((left + right) / 2, (top + bottom) / 2);
}

location rectangle::topLeft() const {
	return location(left, top);
}

location rectangle::topRight() const {
	return location(right, top);
}

location rectangle::bottomLeft() const {
	return location(left, bottom);
}

location rectangle::bottomRight() const {
	return location(right, bottom);
}

void rectangle::offset(int h, int v) {
	left += h; right += h;
	top += v; bottom += v;
}

void rectangle::offset(location diff) {
	offset(diff.x, diff.y);
}

void rectangle::move_to(int x, int y) {
	offset(x - left, y - top);
}

void rectangle::move_to(location loc) {
	move_to(loc.x, loc.y);
}

void rectangle::inset(int dh, int dv) {
	left += dh; right -= dh;
	top += dv; bottom -= dv;
}

rectangle& rectangle::operator&=(rectangle other) {
	left = std::max(left, other.left);
	top = std::max(top, other.top);
	right = std::min(right, other.right);
	bottom = std::min(bottom, other.bottom);
	return *this;
}

rectangle rect(){
	return rectangle();
}

rectangle rect(location tl, location br){
	return rectangle(tl,br);
}

rectangle rect(int top, int left, int bottom, int right) {
	return rectangle(top, left, bottom, right);
}

std::ostream& operator<< (std::ostream& out, location l) {
	out << '(' << l.x << ',' << l.y << ')';
	return out;
}

struct match {
	char c;
};

struct opt {
	match m;
};

static std::istream& operator>> (std::istream& in, opt o) {
	if(in.eof()){
		in.clear();
		return in;
	}
	char next = in.get();
	if(next != o.m.c) {
		in.putback(next);
	}
	return in;
}

static std::istream& operator>> (std::istream& in, match m) {
	if(in.get() != m.c) {
		in.setstate(std::ios_base::failbit);
	}
	return in;
}

std::istream& operator>> (std::istream& in, location& l) {
	in >> std::ws >> opt{match{'('}};
	in >> l.x;
	in >> match{','} >> std::ws;
	in >> l.y;
	in >> opt{match{')'}};
	return in;
}

std::ostream& operator<< (std::ostream& out, spec_loc_t l) {
	out << static_cast<location&>(l);
	out << ':' << l.spec;
	return out;
}

std::ostream& operator<< (std::ostream& out, sign_loc_t l) {
	out << static_cast<location&>(l);
	out << " -- \"" << l.text << '"';
	return out;
}

std::ostream& operator<< (std::ostream& out, rectangle r) {
	out << "{" << r.topLeft() << " - " << r.bottomRight() << '}';
	return out;
}

std::istream& operator>> (std::istream& in, rectangle& r) {
	location tl;
	location br;
	in >> std::ws >> match{'{'} >> std::ws >> tl;
	in >> std::ws >> match{'-'} >> std::ws >> br >> std::ws >> match{'}'};
	r = rectangle(tl, br);
	return in;
}

std::ostream& operator<< (std::ostream& out, info_rect_t r) {
	out << static_cast<rectangle&>(r);
	out << " -- \"" << r.descr << '"';
	return out;
}

bool is_on_screen(location loc, location view_center, int radius) {
	return loc.x >= view_center.x - radius
		&& loc.x <= view_center.x + radius
		&& loc.y >= view_center.y - radius
		&& loc.y <= view_center.y + radius;
}

location between_anchor_points(location anchor1, location anchor2, int padding){
	// First check the point directly between anchor1 and anchor2 (rounding towards anchor1)
	float center_x_f = (anchor1.x + anchor2.x) / 2.0;
	float center_y_f = (anchor1.y + anchor2.y) / 2.0;
	location point_between;
	if(anchor1.x < anchor2.x){
		point_between.x = floor(center_x_f);
	}else{
		point_between.x = ceil(center_x_f);
	}
	if(anchor1.y < anchor2.y){
		point_between.y = floor(center_y_f);
	}else{
		point_between.y = ceil(center_y_f);
	}

	// Then if necessary, move back towards anchor1 until it is visible with the desired padding
	int padded_radius = SCREEN_RADIUS - padding;
	while(!is_on_screen(anchor1, point_between, padded_radius)){
		if(anchor1.x < point_between.x - padded_radius){
			--point_between.x;
		}else if(anchor1.x > point_between.x + padded_radius){
			++point_between.x;
		}
		if(anchor1.y < point_between.y - padded_radius){
			--point_between.y;
		}else if(anchor1.y > point_between.y + padded_radius){
			++point_between.y;
		}
	}
	return point_between;
}

std::vector<location> points_containing_most(std::vector<location> points, int padding) {
	// Find the bounding box of the given points:
	int min_x = std::numeric_limits<int>::max();
	int min_y = min_x;
	int max_x = std::numeric_limits<int>::min();
	int max_y = max_x;

	for(location p : points){
		if(p.x < min_x) min_x = p.x;
		if(p.y < min_y) min_y = p.y;
		if(p.x > max_x) max_x = p.x;
		if(p.y > max_y) max_y = p.y;
	}

	// Expand the bounds by the view radius:
	int padded_radius = SCREEN_RADIUS - padding;
	min_x -= padded_radius; min_y -= padded_radius;
	max_x += padded_radius; max_y += padded_radius;
	// This function's output may include out-of-bounds points, but those will be eliminated
	// by calling closest_point() with an anchor point that is in-bounds.

	// Calculate how many of the points can be seen from each point in the bounding box:
	std::vector<std::pair<location, int>> points_seen_from;
	location checking_point;
	for(checking_point.x = min_x; checking_point.x <= max_x; ++checking_point.x){
		for(checking_point.y = min_y; checking_point.y <= max_y; ++checking_point.y){
			int can_see = 0;
			for(location p : points){
				if(is_on_screen(p, checking_point, padded_radius)) ++can_see;
			}
			points_seen_from.push_back(std::make_pair(checking_point, can_see));
		}
	}

	// Sort candidates by how many of the points they see
	std::sort(points_seen_from.begin(), points_seen_from.end(), [](std::pair<location,int> pair1, std::pair<location,int> pair2) -> bool {
		return pair1.second > pair2.second;
	});
	int max_seen = points_seen_from[0].second;

	// Return all of them that see the max number
	std::vector<location> return_points;
	for(auto pair : points_seen_from){
		if(pair.second == max_seen) return_points.push_back(pair.first);
	}

	return return_points;
}

int closest_point_idx(std::vector<location> points, location anchor) {
	float min_distance = std::numeric_limits<float>::max();
	int min_idx = -1;
	for(int idx = 0; idx < points.size(); ++idx){
		float distance = fdist(points[idx], anchor);
		if(distance < min_distance){
			min_distance = distance;
			min_idx = idx;
		}
	}
	return min_idx;
}

location closest_point(std::vector<location> points, location anchor) {
	return points[closest_point_idx(points, anchor)];
}