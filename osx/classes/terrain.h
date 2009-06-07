/*
 *  terrain.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>
#include <iosfwd>
#include "graphtool.h" // for pic_num_t

namespace legacy { struct terrain_type_type; };

// Depending on the special ability, the flags may need to be treated as either signed or unsigned
union ter_flag_t {signed short s; unsigned short u;};

class cTerrain {
public:
	std::string name;
	pic_num_t picture;
	unsigned char blockage;
	ter_flag_t flag1;
	ter_flag_t flag2;
	ter_flag_t flag3; // new additional flag for special properties
	eTerSpec special;
	ter_num_t trans_to_what;
	unsigned char fly_over;
	unsigned char boat_over;
	unsigned char block_horse;
	unsigned char light_radius;
	unsigned char step_sound;
	unsigned char shortcut_key; // for editor use only
	unsigned char obj_num; // ditto (formerly res1)
	unsigned char ground_type; // ditto (formerly res2)
	eTrimType trim_type; // ditto, mostly (formerly res3)
	unsigned short trim_ter; // ditto
	unsigned short combat_arena;
	location obj_pos; // editor use only
	location obj_size; // editor use only
	pic_num_t map_pic;
	unsigned short i; // for temporary use in porting
	
	cTerrain& operator = (legacy::terrain_type_type& old);
	void writeTo(std::ostream& file);
};

std::ostream& operator << (std::ostream& out, eTerSpec& e);
std::istream& operator >> (std::istream& in, eTerSpec& e);

#endif