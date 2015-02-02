/*
 *  terrain.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef BOE_DATA_TERRAIN_H
#define BOE_DATA_TERRAIN_H

#include <string>
#include <iosfwd>

#include "simpletypes.h"
#include "pictypes.hpp"
#include "location.h"
#include "soundtool.hpp" // for snd_num_t

namespace legacy { struct terrain_type_type; };

enum class eStepSnd {STEP, SQUISH, CRUNCH, NONE, SPLASH};

class cTerrain {
public:
	std::string name;
	pic_num_t picture;
	eTerObstruct blockage;
	int flag1, flag2, flag3;
	eTerSpec special;
	ter_num_t trans_to_what;
	bool fly_over;
	bool boat_over;
	bool block_horse;
	unsigned int light_radius;
	eStepSnd step_sound;
	unsigned char shortcut_key; // for editor use only
	unsigned int obj_num = 0; // ditto (formerly res1)
	unsigned int ground_type; // ditto (formerly res2)
	eTrimType trim_type; // ditto, mostly (formerly res3)
	long trim_ter; // ditto
	unsigned short combat_arena;
	location obj_pos; // editor use only
	location obj_size; // editor use only
	pic_num_t map_pic;
	unsigned short i; // for temporary use in porting
	
	void append(legacy::terrain_type_type& old);
	void writeTo(std::ostream& file) const;
};

std::ostream& operator << (std::ostream& out, eTerSpec e);
std::istream& operator >> (std::istream& in, eTerSpec& e);
std::ostream& operator << (std::ostream& out, eTrimType e);
std::istream& operator >> (std::istream& in, eTrimType& e);
std::ostream& operator << (std::ostream& out, eTerObstruct e);
std::istream& operator >> (std::istream& in, eTerObstruct& e);
std::ostream& operator << (std::ostream& out, eStepSnd e);
std::istream& operator >> (std::istream& in, eStepSnd& e);

#endif
