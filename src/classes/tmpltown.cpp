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
	static ter_num_t dummy;
	return dummy; // TODO: will need to calculate the terrain somehow
}

void cBigTemplTown::writeTerrainTo(std::ostream& /*file*/) {
	// TODO: Write out the terrain somehow;
}

void cBigTemplTown::readTerrainFrom(std::istream& /*file*/) {
	// TODO: Read in the terrain somehow
}

unsigned char& cBigTemplTown::lighting(size_t i, size_t r){
	return _lighting[i][r];
}

size_t cBigTemplTown::max_dim() const {
	return 0; // not sure what they are yet.
}

