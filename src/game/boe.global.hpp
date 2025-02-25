

#ifndef BOE_GAME_GLOBAL_H
#define BOE_GAME_GLOBAL_H

#include <vector>
#include <string>
#include <sstream>
#include <map>
#include "boe.consts.hpp"
#include "universe/universe.hpp"

#define ASB	add_string_to_buf
#define	PSD	univ.party.stuff_done

const int NUM_MONST_G = 173;
const int NUM_TER_G = 251;
const int NUM_ITEM_G = 120;
const int NUM_FACE_G = 80;
const int NUM_DLOG_G = 28;

struct scen_header_type{
	int intro_pic;
	eContentRating rating;
	int difficulty, ver[3], prog_make_ver[3];
	std::string name, teaser1, teaser2, file;
};

struct effect_pat_type {
	unsigned short pattern[9][9];
};

extern std::map<std::string, int> startup_button_indices;
extern std::map<int, std::string> startup_button_names;
extern std::map<int, std::string> startup_button_names_v1;

extern cUniverse univ;
extern std::map<std::string, std::vector<std::string>> feature_flags;

inline bool has_feature_flag(std::string flag, std::string version) {
	auto iter = feature_flags.find(flag);
	if(iter == feature_flags.end()) return false;
	std::vector<std::string> versions = iter->second;
	return std::find(versions.begin(), versions.end(), version) != versions.end();
}

// Return the version of a feature that SHOULD BE USED in the currently running game.
inline std::string get_feature_version(std::string flag) {
	// If a scenario is loaded and specifies the flag, use that version.
	if(!univ.party.scen_name.empty()){
		std::string scenario_flag = univ.scenario.get_feature_flag(flag);
		if(!scenario_flag.empty()) return scenario_flag;
	}
	// Otherwise, use the most recent version of the feature supported by this build,
	// or by the build that recorded the current replay.
	auto iter = feature_flags.find(flag);
	if(iter == feature_flags.end()) return "";
	return iter->second.back();
}

#endif
