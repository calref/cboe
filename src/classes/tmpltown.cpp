/*
 *  tmpltown.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "tmpltown.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>

ter_num_t& cBigTemplTown::terrain(size_t x, size_t y){
	location pos(x,y);
	return cBigTown::terrain(x,y);
}

void cBigTemplTown::writeTerrainTo(std::ostream& /*file*/) {
	// TODO: Write out the terrain somehow;
}

void cBigTemplTown::readTerrainFrom(std::istream& /*file*/) {
	// TODO: Read in the terrain somehow
}

