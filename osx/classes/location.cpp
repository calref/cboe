/*
 *  location.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "location.h"
#include "mathutil.h"

bool operator == (location p1,location p2){
	if ((p1.x == p2.x) & (p1.y == p2.y))
		return true;
	else return false;
}

bool operator != (location p1,location p2){
	return ! (p1 == p2);
}

short dist(location p1,location p2){
	return s_sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
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

rectangle operator|(rectangle one, rectangle two) {
	return one |= two;
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
rectangle::rectangle(sf::Texture& texture) : top(0), left(0), right(texture.getSize().x), bottom(texture.getSize().y) {}
rectangle::rectangle(sf::RenderTarget& texture) : top(0), left(0), right(texture.getSize().x), bottom(texture.getSize().y) {}

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

rectangle_size_delegate::operator int() {
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

rectangle_size_delegate rectangle::width() {
	return rectangle_size_delegate(*this, &rectangle::left, &rectangle::right);
}

rectangle_size_delegate rectangle::height() {
	return rectangle_size_delegate(*this, &rectangle::top, &rectangle::bottom);
}

location rectangle::centre() {
	return location((left + right) / 2, (top + bottom) / 2);
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

rectangle& rectangle::operator|=(rectangle other) {
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

rectangle rect(char top, char left, char bottom, char right){
	return rectangle(top, left, bottom, right);
}

