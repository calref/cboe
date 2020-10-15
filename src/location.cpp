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

bool location::in(rectangle r){
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

bool rectangle::contains(location p){
	if(p.y >= top && p.y <= bottom && p.x >= left && p.x <= right)
		return true;
	return false;
}

bool rectangle::contains(int x, int y) {
	return contains(location(x,y));
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

rectangle_size_delegate& rectangle_size_delegate::operator*=(int val) {
	*this = *this * val;
	return *this;
}

rectangle_size_delegate& rectangle_size_delegate::operator/=(int val) {
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

location rectangle::centre() {
	return location((left + right) / 2, (top + bottom) / 2);
}

location rectangle::topLeft() {
	return location(left, top);
}

location rectangle::topRight() {
	return location(right, top);
}

location rectangle::bottomLeft() {
	return location(left, bottom);
}

location rectangle::bottomRight() {
	return location(right, bottom);
}

void rectangle::offset(int h, int v) {
	left += h; right += h;
	top += v; bottom += v;
}

void rectangle::offset(location diff) {
	offset(diff.x, diff.y);
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

std::ostream& operator<< (std::ostream& out, info_rect_t r) {
	out << static_cast<rectangle&>(r);
	out << " -- \"" << r.descr << '"';
	return out;
}
