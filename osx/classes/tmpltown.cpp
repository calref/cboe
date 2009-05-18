/*
 *  tmpltown.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;

#include "classes.h"

unsigned short& cBigTemplTown::terrain(size_t x, size_t y){
	return  _terrain[0][0]; // will need to calculate the terrain somehow
}

rectangle& cBigTemplTown::room_rect(size_t i){
	return _room_rect[i];
}

cCreature& cBigTemplTown::creatures(size_t i){
	return _creatures[i];
}

unsigned char& cBigTemplTown::lighting(size_t i, size_t r){
	return _lighting[i][r];
}

short cBigTemplTown::max_dim(){
	return 0; // not sure what they are yet.
}

short cBigTemplTown::max_monst(){
	return 30;
}
