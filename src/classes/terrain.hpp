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

#include "simpletypes.hpp"
#include "pictypes.hpp"
#include "location.hpp"
#include "soundtool.hpp" // for snd_num_t

namespace legacy { struct terrain_type_type; };

enum class eStepSnd {STEP, SQUISH, CRUNCH, NONE, SPLASH};

class cTerrain {
public:
	std::string name;
	pic_num_t picture = 0;
	eTerObstruct blockage = eTerObstruct::CLEAR;
	int flag1 = 0, flag2 = 0, flag3 = 0;
	eTerSpec special = eTerSpec::NONE;
	ter_num_t trans_to_what = 0;
	bool fly_over = false;
	bool boat_over = false;
	bool block_horse = false;
	bool is_archetype = false;
	unsigned int light_radius = 0;
	eStepSnd step_sound = eStepSnd::STEP;
	unsigned char shortcut_key = 0; // for editor use only
	unsigned int obj_num = 0; // ditto
	unsigned int ground_type = 0; // ditto
	eTrimType trim_type = eTrimType::NONE; // ditto, mostly
	long trim_ter = 0; // ditto
	unsigned short combat_arena = 0;
	location obj_pos; // editor use only
	location obj_size; // editor use only
	pic_num_t map_pic = -1;
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
