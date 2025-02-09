

#ifndef BOE_GAME_GLOBAL_H
#define BOE_GAME_GLOBAL_H

#include <vector>
#include <string>
#include <sstream>
#include <map>
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

extern std::map<std::string, int> startup_button_indices;
extern std::map<int, std::string> startup_button_names;
extern std::map<int, std::string> startup_button_names_v1;

extern std::map<std::string, std::string> feature_flags;

inline bool has_feature_flag(std::string flag) {
	return feature_flags.find(flag) != feature_flags.end();
}
inline std::string get_feature_flag(std::string flag) {
	if(!has_feature_flag(flag)) return "";
	return feature_flags[flag];
}

#endif
