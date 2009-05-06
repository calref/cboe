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
	i = abs((short) (p1.x - p2.x)); j = abs((short) (p1.y - p2.y));
	return max(i,j);
}

location::location() : x(0), y(0) {}
location::location(char a,char b) : x(a), y(b) {}

location loc(char a, char b){
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

rectangle::rectangle() : top(0), left(0), right(0), bottom(0) {}
rectangle::rectangle(location tl, location br) : top(tl.y), left(tl.x), right(br.x), bottom(br.y) {}
rectangle::rectangle(char t, char l, char b, char r) : top(t), left(l), right(r), bottom(b) {}

bool rectangle::contains(location p){
	if(p.y >= top && p.y <= bottom && p.x >= left && p.x <= right)
		return true;
	return false;
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
