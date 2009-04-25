/*
 *  tmpltown.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "classes.h"

unsigned char& cTemplTown::terrain(size_t x, size_t y){
	return  _lighting[0][0]; // will need to calculate the terrain somehow
}

rectangle& cTemplTown::room_rect(size_t i){
	return _room_rect[i];
}

cTown::cCreature& cTemplTown::creatures(size_t i){
	return _creatures[i];
}

unsigned char& cTemplTown::lighting(size_t i, size_t r){
	return _lighting[i][r];
}

short cTemplTown::max_dim(){
	return 0; // not sure what they are yet.
}

short cTemplTown::max_monst(){
	return 30;
}
