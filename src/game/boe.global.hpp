

#ifndef BOE_GAME_GLOBAL_H
#define BOE_GAME_GLOBAL_H

#include <vector>
#include <string>
#include <sstream>
#include "boe.consts.hpp"

#define ASB	add_string_to_buf
#define	PSD	univ.party.stuff_done

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

// Input recording system
namespace ticpp { class Element; }
using ticpp::Element;

extern bool recording;
extern bool replaying;

extern bool init_action_log(std::string command, std::string file);
extern void record_action(std::string action_type, std::string inner_text);
extern Element* pop_next_action(std::string expected_action_type="");

#endif
