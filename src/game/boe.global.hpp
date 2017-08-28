

#ifndef BOE_GAME_GLOBAL_H
#define BOE_GAME_GLOBAL_H

#include <vector>
#include <string>
#include <sstream>
#include "boe.consts.hpp"

#define ASB	add_string_to_buf
#define	PSD	univ.party.stuff_done

const int PC_WIN_UL_X = 291;
const int PC_WIN_UL_Y = 5;
const int ITEM_WIN_UL_X = 291;
const int ITEM_WIN_UL_Y = 130;
const int TEXT_WIN_UL_X = 291;
const int TEXT_WIN_UL_Y = 283;

const int NUM_MONST_G = 173;
const int NUM_TER_G = 251;
const int NUM_ITEM_G = 120;
const int NUM_FACE_G = 80;
const int NUM_DLOG_G = 28;

struct scen_header_type{
	int intro_pic, rating, difficulty, ver[3], prog_make_ver[3];
	std::string name, who1, who2, file;
};

struct effect_pat_type {
	unsigned short pattern[9][9];
};

#endif
