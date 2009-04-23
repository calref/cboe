/*
 *  location.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#pragma once

struct location {
	char x,y;
};

struct shortloc {
	short x,y;
};

bool operator == (location p1,location p2);
bool operator != (location p1,location p2);
short dist(location p1,location p2);