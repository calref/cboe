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

class cBigTown : public cTown { // formerly big_tr_type
protected:
	ter_num_t _terrain[64][64];
	rectangle _room_rect[16];
	cCreature _creatures[60];
	unsigned char _lighting[8][64];
public:
	void append(legacy::big_tr_type& old);
	ter_num_t& terrain(size_t x, size_t y);
	rectangle& room_rect(size_t i);
	cCreature& creatures(size_t i);
	unsigned char& lighting(size_t i, size_t r);
	short max_dim();
	short max_monst();
	short max_items();
	
	cBigTown();
	void writeTerrainTo(std::ostream& file);
	void readTerrainFrom(std::istream& file);
};

class cMedTown : public cTown { // formerly ave_tr_type
protected:
	ter_num_t _terrain[48][48];
	rectangle _room_rect[16];
	cCreature _creatures[40];
	unsigned char _lighting[6][48];
public:
	void append(legacy::ave_tr_type& old);
	ter_num_t& terrain(size_t x, size_t y);
	rectangle& room_rect(size_t i);
	cCreature& creatures(size_t i);
	unsigned char& lighting(size_t i, size_t r);
	short max_dim();
	short max_monst();
	short max_items();
	
	cMedTown();
	void writeTerrainTo(std::ostream& file);
	void readTerrainFrom(std::istream& file);
};

class cTinyTown : public cTown { // formerly tiny_tr_type
protected:
	ter_num_t _terrain[32][32];
	rectangle _room_rect[16];
	cCreature _creatures[30];
	unsigned char _lighting[4][32];
public:
	void append(legacy::tiny_tr_type& old);
	ter_num_t& terrain(size_t x, size_t y);
	rectangle& room_rect(size_t i);
	cCreature& creatures(size_t i);
	unsigned char& lighting(size_t i, size_t r);
	short max_dim();
	short max_monst();
	short max_items();
	
	cTinyTown();
	void writeTerrainTo(std::ostream& file);
	void readTerrainFrom(std::istream& file);
};

#endif