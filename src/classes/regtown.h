/*
 *  regtown.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef BOE_DATA_REGTOWN_H
#define BOE_DATA_REGTOWN_H

#include <iosfwd>

#include "town.h"
#include "simpletypes.h"
#include "location.h"
#include "monster.h"

namespace legacy {
	struct big_tr_type;
	struct ave_tr_type;
	struct tiny_tr_type;
};

class cBigTown : public virtual cTown { // formerly big_tr_type
	ter_num_t ter[64][64];
	unsigned char light[8][64];
public:
	void append(legacy::big_tr_type& old, int town_num);
	ter_num_t& terrain(size_t x, size_t y);
	unsigned char& lighting(size_t i, size_t r);
	size_t max_dim() const;
	
	explicit cBigTown(cScenario& scenario, bool init_strings = false);
	void writeTerrainTo(std::ostream& file);
	void readTerrainFrom(std::istream& file);
};

class cMedTown : public virtual cTown { // formerly ave_tr_type
	ter_num_t ter[48][48];
	unsigned char light[6][48];
public:
	void append(legacy::ave_tr_type& old, int town_num);
	ter_num_t& terrain(size_t x, size_t y);
	unsigned char& lighting(size_t i, size_t r);
	size_t max_dim() const;
	
	explicit cMedTown(cScenario& scenario, bool init_strings = false);
	void writeTerrainTo(std::ostream& file);
	void readTerrainFrom(std::istream& file);
};

class cTinyTown : public virtual cTown { // formerly tiny_tr_type
	ter_num_t ter[32][32];
	unsigned char light[4][32];
public:
	void append(legacy::tiny_tr_type& old, int town_num);
	ter_num_t& terrain(size_t x, size_t y);
	unsigned char& lighting(size_t i, size_t r);
	size_t max_dim() const;
	
	explicit cTinyTown(cScenario& scenario, bool init_strings = false);
	void writeTerrainTo(std::ostream& file);
	void readTerrainFrom(std::istream& file);
};

#endif
