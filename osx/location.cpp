/*
 *  location.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "location.h"

bool operator == (location p1,location p2){
	if ((p1.x == p2.x) & (p1.y == p2.y))
		return true;
	else return false;
}

bool operator != (location p1,location p2){
	return ! (p1 == p2);
}