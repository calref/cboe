/*
 *  outdoors.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "dlogutil.hpp"
#include "classes.h"
#include "oldstructs.h"

void cOutdoors::append(legacy::outdoor_record_type& old){
	int i,j;
	ambient_sound = AMBIENT_NONE;
	// Collect a list of unused special nodes, to be used for fixing specials that could be triggered in a boat.
	std::vector<int> unused_special_slots;
	for(i = 0; i < 60; i++) {
		if(specials[i].type == eSpecType::NONE && specials[i].jumpto == -1) {
			// Also make sure no specials jump to it
			bool is_free = true;
			for(j = 0; j < 60; j++) {
				if(specials[j].jumpto == i) is_free = false;
			}
			if(is_free) unused_special_slots.push_back(i);
		}
	}
	for(i = 0; i < 48; i++)
		for(j = 0; j < 48; j++){
			terrain[i][j] = old.terrain[i][j];
			if(scenario.ter_types[terrain[i][j]].i == 3000) // marker to indicate it used to be a special spot
				special_spot[i][j] = true;
			else special_spot[i][j] = false;
			// Convert roads that crossed grass/hill boundaries
			// It won't catch ones that sit exactly at the edge, though; in that case they'd need manual fixing
			// For simplicity we assume the non-hill space is either a city or a grass road
			// Terrain types used here:
			// 80 - grass road    81 - hill road
			// 38 - hill/grass    40 - hill|grass    42 - grass/hill    44 - grass|hill
			// where / means "over" and | means "beside"
			// Not going to do it for town since roads in town are uncommon. Maybe later.
			if(old.terrain[i][j] == 81 && i > 0 && i < 47 && j > 0 && j < 47) {
				if(old.terrain[i+1][j] == 81) {
					ter_num_t connect = old.terrain[i-1][j];
					if(connect == 80 || scenario.ter_types[connect].trim_type == eTrimType::CITY)
						terrain[i][j] = 44;
				} else if(old.terrain[i-1][j] == 81) {
					ter_num_t connect = old.terrain[i+1][j];
					if(connect == 80 || scenario.ter_types[connect].trim_type == eTrimType::CITY)
						terrain[i][j] = 40;
				} else if(old.terrain[i][j+1] == 81) {
					ter_num_t connect = old.terrain[i][j-1];
					if(connect == 80 || scenario.ter_types[connect].trim_type == eTrimType::CITY)
						terrain[i][j] = 42;
				} else if(old.terrain[i][j-1] == 81) {
					ter_num_t connect = old.terrain[i][j+1];
					if(connect == 80 || scenario.ter_types[connect].trim_type == eTrimType::CITY)
						terrain[i][j] = 38;
				}
			}
			if(scenario.ter_types[terrain[i][j]].boat_over) {
				// Try to fix specials that could be triggered while in a boat
				// (Boats never triggered specials in the old BoE, so we probably don't want them to trigger.)
				int found_spec = -1;
				for(int k = 0; k < 18; k++) {
					if(i == special_locs[k].x && j == special_locs[k].y) {
						found_spec = k;
						break;
					}
				}
				if(found_spec >= 0) {
					if(!unused_special_slots.empty()) {
						int found_spec_id = special_id[found_spec], use_slot = unused_special_slots.back();
						unused_special_slots.pop_back();
						cSpecial& node = specials[use_slot];
						node.type = eSpecType::IF_CONTEXT;
						node.ex1a = 101; // if in boat
						node.ex1c = -1; // do nothing
						node.jumpto = found_spec_id; // else jump here
						special_id[found_spec] = use_slot;
					} else {
						std::stringstream sout;
						sout << "In outdoor section (" << x << ',' << y << ") at (" << i << ',' << j << "); special node ID ";
						sout << special_id[found_spec];
						giveError("Warning: A special node was found that could be triggered from in a boat, which is probably not what the designer intended. An attempt to fix this has failed because there were not enough unused special nodes.", sout.str());
					}
				}
			}
		}
	for(i = 0; i < 18; i++){
		special_locs[i].x = old.special_locs[i].x;
		special_locs[i].y = old.special_locs[i].y;
		special_id[i] = old.special_id[i];
	}
	for(i = 0; i < 8; i++){
		exit_locs[i].x = old.exit_locs[i].x;
		exit_locs[i].y = old.exit_locs[i].y;
		exit_dests[i] = old.exit_dests[i];
		sign_locs[i].x = old.sign_locs[i].x;
		sign_locs[i].y = old.sign_locs[i].y;
		info_rect[i].top = old.info_rect[i].top;
		info_rect[i].left = old.info_rect[i].left;
		info_rect[i].bottom = old.info_rect[i].bottom;
		info_rect[i].right = old.info_rect[i].right;
	}
	for(i = 0; i < 4; i++){
//		for(j = 0; j < 7; j++){
//			wandering[i].monst[j] = old.wandering[i].monst[j];
//			special_enc[i].monst[j] = old.special_enc[i].monst[j];
//		}
//		for(j = 0; j < 3; j++){
//			wandering[i].fristd::endly[j] = old.wandering[i].fristd::endly[j];
//			special_enc[i].fristd::endly[j] = old.special_enc[i].fristd::endly[j];
//		}
//		wandering[i].spec_on_meet = old.wandering[i].spec_on_meet;
//		special_enc[i].spec_on_meet = old.special_enc[i].spec_on_meet;
//		wandering[i].spec_on_win = old.wandering[i].spec_on_win;
//		special_enc[i].spec_on_win = old.special_enc[i].spec_on_win;
//		wandering[i].spec_on_flee = old.wandering[i].spec_on_flee;
//		special_enc[i].spec_on_flee = old.special_enc[i].spec_on_flee;
//		wandering[i].cant_flee = old.wandering[i].cant_flee;
//		special_enc[i].cant_flee = old.special_enc[i].cant_flee;
//		wandering[i].end_spec1 = old.wandering[i].end_spec1;
//		special_enc[i].end_spec1 = old.special_enc[i].end_spec1;
//		wandering[i].end_spec2 = old.wandering[i].end_spec2;
//		special_enc[i].end_spec2 = old.special_enc[i].end_spec2;
		wandering[i].append(old.wandering[i]);
		special_enc[i].append(old.special_enc[i]);
		wandering_locs[i].x = old.wandering_locs[i].x;
		wandering_locs[i].y = old.wandering_locs[i].y;
	}
	for(i = 0; i < 60; i++)
		specials[i].append(old.specials[i]);
}

cOutdoors::cWandering::cWandering() {
	spec_on_meet = spec_on_win = spec_on_flee = -1;
	end_spec1 = end_spec2 = -1;
}

cOutdoors::cOutdoors(cScenario& scenario, bool init_strings) : scenario(scenario) {
	short i,j;
	location d_loc(100,0);;
	location locs[4] = {loc(8,8),loc(32,8),loc(8,32),loc(32,32)};
	
	for(i = 0; i < 48; i++)
		for(j = 0; j < 48; j++) {
			terrain[i][j] = 5; // formerly 0
		}
	
	for(i = 0; i < 18; i++) {
		special_locs[i] = d_loc;
	}
	for(i = 0; i < 8; i++) {
		exit_locs[i] = d_loc;
		sign_locs[i] = d_loc;
		sign_locs[i].x = 100;
	}
	for(i = 0; i < 4; i++) {
		wandering_locs[i] = locs[i];
	}
	if(!init_strings) return;
	std::string temp_str;
	for(i = 0; i < 120; i++) {
		temp_str = get_str("outdoor-default",i + 1);
		if(i == 0) out_name = temp_str;
		else if(i == 9) comment = temp_str;
		else if(i < 9) info_rect[i-1].descr = temp_str;
		else if(i >= 10 && i < 100)
			spec_strs[i-10] = temp_str;
		else if(i >= 100 && i < 108)
			sign_strs[i-100] = temp_str;
	}
}

void cOutdoors::cWandering::append(legacy::out_wandering_type old){
	for(int i = 0; i < 7; i++)
		monst[i] = old.monst[i];
	for(int j = 0; j < 3; j++)
		friendly[j] = old.friendly[j];
	spec_on_meet = old.spec_on_meet;
	spec_on_win = old.spec_on_win;
	spec_on_flee = old.spec_on_flee;
	cant_flee = old.cant_flee;
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
	file << prefix << "FLAGS " << cant_flee << '\n';
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
			sin >> cant_flee;
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
