/*
 *  location.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#pragma once

struct rectangle;

struct location {
	char x,y;
	
	location();
	location(char a, char b);
	bool in(rectangle r);
};

struct rectangle {
	char top, left, bottom, right;
	
	rectangle();
	rectangle(location tl, location br);
	rectangle(char t, char l, char b, char r);
	bool contains(location p);
};

struct shortloc {
	short x,y;
};

bool operator == (location p1,location p2);
bool operator != (location p1,location p2);
bool operator == (rectangle r1, rectangle r2);
bool operator != (rectangle r1, rectangle r2);
short dist(location p1,location p2);
short vdist(location p1,location p2);

location loc(char a, char b);
location loc();
rectangle rect();
rectangle rect(location tl, location br);
rectangle rect(char top, char left, char bottom, char right);
