/*
 *  outdoors.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "outdoors.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "strdlog.hpp"
#include "oldstructs.hpp"
#include "scenario.hpp"

void cOutdoors::append(legacy::outdoor_record_type& old){
	ambient_sound = AMBIENT_NONE;
	// Collect a list of unused special nodes, to be used for fixing specials that could be triggered in a boat.
	std::vector<int> unused_special_slots;
	for(short i = 0; i < 60; i++) {
		if(old.specials[i].type == 0 && old.specials[i].jumpto == -1) {
			// Also make sure no specials jump to it
			bool is_free = true;
			for(short j = 0; j < 60; j++) {
				if(old.specials[j].jumpto == i) is_free = false;
			}
			if(is_free) unused_special_slots.push_back(i);
		}
	}
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 48; j++){
			terrain[i][j] = old.terrain[i][j];
			if(scenario->ter_types[terrain[i][j]].i == 3000) // marker to indicate it used to be a special spot
				special_spot[i][j] = true;
			else special_spot[i][j] = false;
			if(scenario->ter_types[terrain[i][j]].i == 3001) // marker to indicate it used to be a road
				roads[i][j] = true;
			else roads[i][j] = false;
			// Convert roads that crossed grass/hill boundaries
			// It won't catch ones that sit exactly at the edge, though; in that case they'd need manual fixing
			// For simplicity we assume the non-hill space is either a city or a grass road
			// Terrain types used here:
			// 80 - grass road    81 - hill road
			// 38 - hill/grass    40 - hill|grass    42 - grass/hill    44 - grass|hill
			// where / means "over" and | means "beside"
			if(old.terrain[i][j] == 81 && i > 0 && i < 47 && j > 0 && j < 47) {
				if(old.terrain[i+1][j] == 81) {
					ter_num_t connect = old.terrain[i-1][j];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						terrain[i][j] = 44;
				} else if(old.terrain[i-1][j] == 81) {
					ter_num_t connect = old.terrain[i+1][j];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						terrain[i][j] = 40;
				} else if(old.terrain[i][j+1] == 81) {
					ter_num_t connect = old.terrain[i][j-1];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						terrain[i][j] = 42;
				} else if(old.terrain[i][j-1] == 81) {
					ter_num_t connect = old.terrain[i][j+1];
					if(connect == 80 || scenario->ter_types[connect].trim_type == eTrimType::CITY)
						terrain[i][j] = 38;
				}
			}
		}
	special_locs.resize(18);
	for(short i = 0; i < 18; i++){
		special_locs[i].x = old.special_locs[i].x;
		special_locs[i].y = old.special_locs[i].y;
		if(old.special_locs[i].x == 100)
			special_locs[i].spec = -1;
		else special_locs[i].spec = old.special_id[i];
	}
	city_locs.resize(8);
	sign_locs.resize(8);
	info_rect.resize(8);
	for(short i = 0; i < 8; i++){
		city_locs[i].x = old.exit_locs[i].x;
		city_locs[i].y = old.exit_locs[i].y;
		city_locs[i].spec = old.exit_dests[i];
		sign_locs[i].x = old.sign_locs[i].x;
		sign_locs[i].y = old.sign_locs[i].y;
		info_rect[i].top = old.info_rect[i].top;
		info_rect[i].left = old.info_rect[i].left;
		info_rect[i].bottom = old.info_rect[i].bottom;
		info_rect[i].right = old.info_rect[i].right;
	}
	for(short i = 0; i < 4; i++){
		wandering[i].append(old.wandering[i]);
		special_enc[i].append(old.special_enc[i]);
		wandering_locs[i].x = old.wandering_locs[i].x;
		wandering_locs[i].y = old.wandering_locs[i].y;
	}
	specials.resize(60);
	for(short i = 0; i < 60; i++)
		specials[i].append(old.specials[i]);
}

cOutdoors::cWandering::cWandering() {
	spec_on_meet = spec_on_win = spec_on_flee = -1;
	end_spec1 = end_spec2 = -1;
	std::fill(monst.begin(), monst.end(), 0);
	std::fill(friendly.begin(), friendly.end(), 0);
}

cOutdoors::cOutdoors(cScenario& scenario) : scenario(&scenario) {
	location locs[4] = {loc(8,8),loc(32,8),loc(8,32),loc(32,32)};
	bg_out = bg_fight = bg_town = bg_dungeon = -1;
	out_sound = 0;
	
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 48; j++) {
			terrain[i][j] = scenario.default_ground;
			special_spot[i][j] = false;
			roads[i][j] = false;
		}
	
	for(short i = 0; i < wandering_locs.size(); i++) {
		wandering_locs[i] = locs[i];
	}
	out_name = "Area name";
	comment = "Comment";
}

void cOutdoors::cWandering::append(legacy::out_wandering_type old){
	for(int i = 0; i < 7; i++)
		monst[i] = old.monst[i];
	for(int j = 0; j < 3; j++)
		friendly[j] = old.friendly[j];
	spec_on_meet = old.spec_on_meet;
	spec_on_win = old.spec_on_win;
	spec_on_flee = old.spec_on_flee;
	cant_flee = old.cant_flee % 10 == 1;
	forced = old.cant_flee >= 10;
	end_spec1 = old.end_spec1;
	end_spec2 = old.end_spec2;
}

void cOutdoors::cCreature::append(legacy::outdoor_creature_type old){
	exists = old.exists;
	direction = old.direction;
	what_monst.append(old.what_monst);
	which_sector.x = old.which_sector.x;
	which_sector.y = old.which_sector.y;
	m_loc.x = old.m_loc.x;
	m_loc.y = old.m_loc.y;
}

void cOutdoors::cWandering::writeTo(std::ostream& file, std::string prefix) const {
	for(int i = 0; i < 7; i++)
		file << prefix << "HOSTILE " << i << ' ' << monst[i] << '\n';
	for(int i = 0; i < 3; i++)
		file << prefix << "FRIEND " << i << ' ' << friendly[i] << '\n';
	file << prefix << "MEET " << spec_on_meet << '\n';
	file << prefix << "WIN " << spec_on_win << '\n';
	file << prefix << "FLEE " << spec_on_flee << '\n';
	file << prefix << "FLAGS " << cant_flee << ' ' << forced << '\n';
	file << prefix << "SDF " << end_spec1 << ' ' << end_spec2 << '\n';
}

void cOutdoors::cWandering::readFrom(std::istream& file){
	while(file) {
		std::string cur;
		getline(file, cur);
		std::istringstream sin(cur);
		sin >> cur;
		if(cur == "HOSTILE"){
			int i;
			sin >> i;
			sin >> monst[i];
		}else if(cur == "FRIEND"){
			int i;
			sin >> i;
			sin >> friendly[i];
		}else if(cur == "MEET")
			sin >> spec_on_meet;
		else if(cur == "WIN")
			sin >> spec_on_win;
		else if(cur == "FLEE")
			sin >> spec_on_flee;
		else if(cur == "FLAGS")
			sin >> cant_flee >> forced;
		else if(cur == "SDF")
			sin >> end_spec1 >> end_spec2;
	}
}

bool cOutdoors::cWandering::isNull(){
	for(short i = 0; i < 7; i++)
		if(monst[i] != 0)
			return false;
	return true;
}

void cOutdoors::reattach(cScenario& scen) {
	scenario = &scen;
}
