

#ifndef BOE_GAME_GLOBAL_H
#define BOE_GAME_GLOBAL_H

#include <vector>
#include <string>
#include <sstream>

#include "pict.hpp"
#include "boe.consts.hpp"

#define ASB	add_string_to_buf

const int NUM_MONST_G = 173;
const int NUM_TER_G = 251;
const int NUM_ITEM_G = 120;
const int NUM_FACE_G = 80;
const int NUM_DLOG_G = 28;

struct scen_header_type{
	cPictNum intro_pic;
	int rating, difficulty, ver[3], prog_make_ver[3];
	std::string name, who1, who2, file;
};

struct effect_pat_type {
	unsigned short pattern[9][9];
};

#endif
