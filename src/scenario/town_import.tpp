/*
 *  town_import.tpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 02/09/16.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "scenario.hpp"
#include "strdlog.hpp"
#include "oldstructs.hpp"
#include "fileio.hpp"

template<typename T>
struct import_helper {
	static const int dim = sizeof(T().terrain) / sizeof(T().terrain[0]);
	static const int num_monst = sizeof(T().creatures) / sizeof(T().creatures[0]);
};

template<typename T>
void cTown::import_legacy(T const & old, int){
	typedef import_helper<T> sizes;
	assert(max_dim == sizes::dim && "Tried to import legacy town into wrong-sized town");
	cField the_field, the_road;
	the_field.type = SPECIAL_SPOT;
	the_road.type = SPECIAL_ROAD;
	// Collect a list of unused special nodes, to be used for fixing specials that could be triggered in a boat.
	std::vector<int> unused_special_slots;
	for(short i = 0; i < 100; i++) {
		if(specials[i].type == eSpecType::NONE && specials[i].jumpto == -1) {
			// Also make sure no specials jump to it
			bool is_free = true;
			for(short j = 0; j < 100; j++) {
				if(specials[j].jumpto == i) is_free = false;
			}
			if(is_free) unused_special_slots.push_back(i);
		}
	}
	for(short i = 0; i < sizes::dim; i++)
		for(short j = 0; j < sizes::dim; j++) {
			terrain[i][j] = old.terrain[i][j];
			lighting[i][j] = old.lighting[i / 8][j] & (1 << (i % 8));
			if(scenario->get_terrain(terrain[i][j]).i == 3000) { // marker to indicate it used to be a special spot
				the_field.loc.x = i;
				the_field.loc.y = j;
				preset_fields.push_back(the_field);
			}
			if(scenario->get_terrain(terrain[i][j]).i == 3001) { // marker to indicate it used to be a road
				the_road.loc.x = i;
				the_road.loc.y = j;
				preset_fields.push_back(the_road);
			}
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
					if(connect == 80 || scenario->get_terrain(connect).trim_type == eTrimType::CITY)
						terrain[i][j] = 44;
				} else if(old.terrain[i-1][j] == 81) {
					ter_num_t connect = old.terrain[i+1][j];
					if(connect == 80 || scenario->get_terrain(connect).trim_type == eTrimType::CITY)
						terrain[i][j] = 40;
				} else if(old.terrain[i][j+1] == 81) {
					ter_num_t connect = old.terrain[i][j-1];
					if(connect == 80 || scenario->get_terrain(connect).trim_type == eTrimType::CITY)
						terrain[i][j] = 42;
				} else if(old.terrain[i][j-1] == 81) {
					ter_num_t connect = old.terrain[i][j+1];
					if(connect == 80 || scenario->get_terrain(connect).trim_type == eTrimType::CITY)
						terrain[i][j] = 38;
				}
			}
			if(scenario->get_terrain(terrain[i][j]).boat_over) {
				// Try to fix specials that could be triggered while in a boat
				// (Boats never triggered specials in the old BoE, so we probably don't want them to trigger.)
				int found_spec = -1;
				for(int k = 0; k < 50; k++) {
					if(i == special_locs[k].x && j == special_locs[k].y) {
						found_spec = k;
						break;
					}
				}
				if(found_spec >= 0) {
					int found_spec_id = special_locs[found_spec].spec;
					cSpecial* node;
					if(!unused_special_slots.empty()) {
						int use_slot = unused_special_slots.back();
						unused_special_slots.pop_back();
						node = &specials[use_slot];
						special_locs[found_spec].spec = use_slot;
					} else {
						special_locs[found_spec].spec = specials.size();
						specials.emplace_back();
						node = &specials.back();
					}
					node->type = eSpecType::IF_IN_BOAT;
					node->ex1b = -1; // any boat;
					node->ex1c = -1; // do nothing
					node->jumpto = found_spec_id; // else jump here
				}
			}
		}
	area_desc.resize(16);
	for(short i = 0; i < 16; i++) {
		area_desc[i].top = old.room_rect[i].top;
		area_desc[i].left = old.room_rect[i].left;
		area_desc[i].bottom = old.room_rect[i].bottom;
		area_desc[i].right = old.room_rect[i].right;
	}
	creatures.resize(sizes::num_monst);
	for(short i = 0; i < sizes::num_monst; i++) {
		creatures[i].import_legacy(old.creatures[i]);
	}
}
