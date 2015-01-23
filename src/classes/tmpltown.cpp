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

#include "classes.h"

ter_num_t& cBigTemplTown::terrain(size_t /*x*/, size_t /*y*/){
	return  _terrain[0][0]; // TODO: will need to calculate the terrain somehow
}

void cBigTemplTown::writeTerrainTo(std::ostream& /*file*/) {
	// TODO: Write out the terrain somehow;
}

void cBigTemplTown::readTerrainFrom(std::istream& /*file*/) {
	// TODO: Read in the terrain somehow
}

cTownperson& cBigTemplTown::creatures(size_t i){
	return _creatures[i];
}

unsigned char& cBigTemplTown::lighting(size_t i, size_t r){
	return _lighting[i][r];
}

size_t cBigTemplTown::max_dim() const {
	return 0; // not sure what they are yet.
}

size_t cBigTemplTown::max_monst() const {
	return 30;
}

size_t cBigTemplTown::max_items() const {
	return 64;
}
