/*
 *  regtown.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "dlogutil.h"
#include "classes.h"
#include "oldstructs.h"
#include "fileio.h"

extern cScenario scenario;

void cTinyTown::append(legacy::tiny_tr_type& old, int town_num){
	int i,j;
	cField the_field;
	the_field.type = 2;
	// Collect a list of unused special nodes, to be used for fixing specials that could be triggered in a boat.
	std::vector<int> unused_special_slots;
	for(i = 0; i < 100; i++) {
		if(specials[i].type == eSpecType::NONE && specials[i].jumpto == -1) {
			// Also make sure no specials jump to it
			bool is_free = true;
			for(j = 0; j < 100; j++) {
				if(specials[j].jumpto == i) is_free = false;
			}
			if(is_free) unused_special_slots.push_back(i);
		}
	}
	for (i = 0; i < 32; i++)
		for (j = 0; j < 32; j++) {
			_terrain[i][j] = old.terrain[i][j];
			_lighting[i / 8][j] = old.lighting[i / 8][j];
			if(scenario.ter_types[_terrain[i][j]].i == 3000) { // marker to indicate it used to be a special spot
				the_field.loc.x = i;
				the_field.loc.y = j;
				preset_fields.push_back(the_field);
			}
			if(scenario.ter_types[_terrain[i][j]].boat_over) {
				// Try to fix specials that could be triggered while in a boat
				// (Boats never triggered specials in the old BoE, so we probably don't want them to trigger.)
				int found_spec = -1;
				for(int k = 0; i < 50; k++) {
					if(i == special_locs[k].x && j == special_locs[k].y) {
						found_spec = k;
						break;
					}
				}
				if(found_spec >= 0) {
					if(!unused_special_slots.empty()) {
						int found_spec_id = spec_id[found_spec], use_slot = unused_special_slots.back();
						unused_special_slots.pop_back();
						cSpecial& node = specials[use_slot];
						node.type = eSpecType::IF_CONTEXT;
						node.ex1a = 101; // if in boat
						node.ex1c = -1; // do nothing
						node.jumpto = found_spec_id; // else jump here
						spec_id[found_spec] = use_slot;
					} else {
						std::stringstream sout;
						sout << "In town \"" << town_num << "\" at (" << i << ',' << j << "); special node ID " << spec_id[found_spec];
						giveError("Warning: A special node was found that could be triggered from in a boat, which is probably not what the designer intended. An attempt to fix this has failed because there were not enough unused special nodes.", sout.str());
					}
				}
			}
		}
	for (i = 0; i < 16; i++) {
		_room_rect[i].top = old.room_rect[i].top;
		_room_rect[i].left = old.room_rect[i].left;
		_room_rect[i].bottom = old.room_rect[i].bottom;
		_room_rect[i].right = old.room_rect[i].right;
	}
	for (i = 0; i < 30; i++) {
//		_creatures[i].number = old.creatures[i].number;
//		_creatures[i].start_attitude = old.creatures[i].start_attitude;
//		_creatures[i].start_loc.x = old.creatures[i].start_loc.x;
//		_creatures[i].start_loc.y = old.creatures[i].start_loc.y;
//		_creatures[i].mobile = old.creatures[i].mobile;
//		_creatures[i].time_flag = old.creatures[i].time_flag;
//		_creatures[i].extra1 = old.creatures[i].extra1;
//		_creatures[i].extra2 = old.creatures[i].extra2;
//		_creatures[i].spec1 = old.creatures[i].spec1;
//		_creatures[i].spec2 = old.creatures[i].spec2;
//		_creatures[i].spec_enc_code = old.creatures[i].spec_enc_code;
//		_creatures[i].time_code = old.creatures[i].time_code;
//		_creatures[i].monster_time = old.creatures[i].monster_time;
//		_creatures[i].personality = old.creatures[i].personality;
//		_creatures[i].special_on_kill = old.creatures[i].special_on_kill;
//		_creatures[i].facial_pic = old.creatures[i].facial_pic;
		_creatures[i] = old.creatures[i];
	}
}

void cMedTown::append(legacy::ave_tr_type& old, int town_num){
	int i,j;
	cField the_field;
	the_field.type = 2;
	// Collect a list of unused special nodes, to be used for fixing specials that could be triggered in a boat.
	std::vector<int> unused_special_slots;
	for(i = 0; i < 100; i++) {
		if(specials[i].type == eSpecType::NONE && specials[i].jumpto == -1) {
			// Also make sure no specials jump to it
			bool is_free = true;
			for(j = 0; j < 100; j++) {
				if(specials[j].jumpto == i) is_free = false;
			}
			if(is_free) unused_special_slots.push_back(i);
		}
	}
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			_terrain[i][j] = old.terrain[i][j];
			_lighting[i / 8][j] = old.lighting[i / 8][j];
			if(scenario.ter_types[_terrain[i][j]].i == 3000) { // marker to indicate it used to be a special spot
				the_field.loc.x = i;
				the_field.loc.y = j;
				preset_fields.push_back(the_field);
			}
			if(scenario.ter_types[_terrain[i][j]].boat_over) {
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
					if(!unused_special_slots.empty()) {
						int found_spec_id = spec_id[found_spec], use_slot = unused_special_slots.back();
						unused_special_slots.pop_back();
						cSpecial& node = specials[use_slot];
						node.type = eSpecType::IF_CONTEXT;
						node.ex1a = 101; // if in boat
						node.ex1c = -1; // do nothing
						node.jumpto = found_spec_id; // else jump here
						spec_id[found_spec] = use_slot;
					} else {
						std::stringstream sout;
						sout << "In town " << town_num << " at (" << i << ',' << j << "); special node ID " << spec_id[found_spec];
						giveError("Warning: A special node was found that could be triggered from in a boat, which is probably not what the designer intended. An attempt to fix this has failed because there were not enough unused special nodes.", sout.str());
					}
				}
			}
		}
	for (i = 0; i < 16; i++) {
		_room_rect[i].top = old.room_rect[i].top;
		_room_rect[i].left = old.room_rect[i].left;
		_room_rect[i].bottom = old.room_rect[i].bottom;
		_room_rect[i].right = old.room_rect[i].right;
	}
	for (i = 0; i < 40; i++) {
//		_creatures[i].number = old.creatures[i].number;
//		_creatures[i].start_attitude = old.creatures[i].start_attitude;
//		_creatures[i].start_loc.x = old.creatures[i].start_loc.x;
//		_creatures[i].start_loc.y = old.creatures[i].start_loc.y;
//		_creatures[i].mobile = old.creatures[i].mobile;
//		_creatures[i].time_flag = old.creatures[i].time_flag;
//		_creatures[i].extra1 = old.creatures[i].extra1;
//		_creatures[i].extra2 = old.creatures[i].extra2;
//		_creatures[i].spec1 = old.creatures[i].spec1;
//		_creatures[i].spec2 = old.creatures[i].spec2;
//		_creatures[i].spec_enc_code = old.creatures[i].spec_enc_code;
//		_creatures[i].time_code = old.creatures[i].time_code;
//		_creatures[i].monster_time = old.creatures[i].monster_time;
//		_creatures[i].personality = old.creatures[i].personality;
//		_creatures[i].special_on_kill = old.creatures[i].special_on_kill;
//		_creatures[i].facial_pic = old.creatures[i].facial_pic;
		_creatures[i] = old.creatures[i];
	}
}

void cBigTown::append(legacy::big_tr_type& old, int town_num){
	int i,j;
	cField the_field;
	the_field.type = 2;
	// Collect a list of unused special nodes, to be used for fixing specials that could be triggered in a boat.
	std::vector<int> unused_special_slots;
	for(i = 0; i < 100; i++) {
		if(specials[i].type == eSpecType::NONE && specials[i].jumpto == -1) {
			// Also make sure no specials jump to it
			bool is_free = true;
			for(j = 0; j < 100; j++) {
				if(specials[j].jumpto == i) is_free = false;
			}
			if(is_free) unused_special_slots.push_back(i);
		}
	}
	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++) {
			_terrain[i][j] = old.terrain[i][j];
			_lighting[i / 8][j] = old.lighting[i / 8][j];
			if(scenario.ter_types[_terrain[i][j]].i == 3000) { // marker to indicate it used to be a special spot
				the_field.loc.x = i;
				the_field.loc.y = j;
				preset_fields.push_back(the_field);
			}
			if(scenario.ter_types[_terrain[i][j]].boat_over) {
				// Try to fix specials that could be triggered while in a boat
				// (Boats never triggered specials in the old BoE, so we probably don't want them to trigger.)
				int found_spec = -1;
				for(int k = 0; i < 50; k++) {
					if(i == special_locs[k].x && j == special_locs[k].y) {
						found_spec = k;
						break;
					}
				}
				if(found_spec >= 0) {
					if(!unused_special_slots.empty()) {
						int found_spec_id = spec_id[found_spec], use_slot = unused_special_slots.back();
						unused_special_slots.pop_back();
						cSpecial& node = specials[use_slot];
						node.type = eSpecType::IF_CONTEXT;
						node.ex1a = 101; // if in boat
						node.ex1c = -1; // do nothing
						node.jumpto = found_spec_id; // else jump here
						spec_id[found_spec] = use_slot;
					} else {
						std::stringstream sout;
						sout << "In town " << town_num << " at (" << i << ',' << j << "); special node ID " << spec_id[found_spec];
						giveError("Warning: A special node was found that could be triggered from in a boat, which is probably not what the designer intended. An attempt to fix this has failed because there were not enough unused special nodes.", sout.str());
					}
				}
			}
		}
	for (i = 0; i < 16; i++) {
		_room_rect[i].top = old.room_rect[i].top;
		_room_rect[i].left = old.room_rect[i].left;
		_room_rect[i].bottom = old.room_rect[i].bottom;
		_room_rect[i].right = old.room_rect[i].right;
	}
	for (i = 0; i < 60; i++) {
//		_creatures[i].number = old.creatures[i].number;
//		_creatures[i].start_attitude = old.creatures[i].start_attitude;
//		_creatures[i].start_loc.x = old.creatures[i].start_loc.x;
//		_creatures[i].start_loc.y = old.creatures[i].start_loc.y;
//		_creatures[i].mobile = old.creatures[i].mobile;
//		_creatures[i].time_flag = old.creatures[i].time_flag;
//		_creatures[i].extra1 = old.creatures[i].extra1;
//		_creatures[i].extra2 = old.creatures[i].extra2;
//		_creatures[i].spec1 = old.creatures[i].spec1;
//		_creatures[i].spec2 = old.creatures[i].spec2;
//		_creatures[i].spec_enc_code = old.creatures[i].spec_enc_code;
//		_creatures[i].time_code = old.creatures[i].time_code;
//		_creatures[i].monster_time = old.creatures[i].monster_time;
//		_creatures[i].personality = old.creatures[i].personality;
//		_creatures[i].special_on_kill = old.creatures[i].special_on_kill;
//		_creatures[i].facial_pic = old.creatures[i].facial_pic;
		_creatures[i] = old.creatures[i];
	}
}

ter_num_t& cTinyTown::terrain(size_t x, size_t y){
	return _terrain[x][y];
}

void cTinyTown::writeTerrainTo(std::ostream& file) {
	writeArray(file, _terrain, 32, 32);
}

void cTinyTown::readTerrainFrom(std::istream& file) {
	readArray(file, _terrain, 32, 32);
}

rectangle& cTinyTown::room_rect(size_t i){
	return _room_rect[i];
}

cCreature& cTinyTown::creatures(size_t i){
	return _creatures[i];
}

unsigned char& cTinyTown::lighting(size_t i, size_t r){
	return _lighting[i][r];
}

ter_num_t& cMedTown::terrain(size_t x, size_t y){
	return _terrain[x][y];
}

void cMedTown::writeTerrainTo(std::ostream& file) {
	writeArray(file, _terrain, 48, 48);
}

void cMedTown::readTerrainFrom(std::istream& file) {
	readArray(file, _terrain, 48, 48);
}

rectangle& cMedTown::room_rect(size_t i){
	return _room_rect[i];
}

cCreature& cMedTown::creatures(size_t i){
	return _creatures[i];
}

unsigned char& cMedTown::lighting(size_t i, size_t r){
	return _lighting[i][r];
}

ter_num_t& cBigTown::terrain(size_t x, size_t y){
	return _terrain[x][y];
}

void cBigTown::writeTerrainTo(std::ostream& file) {
	writeArray(file, _terrain, 64, 64);
}

void cBigTown::readTerrainFrom(std::istream& file) {
	readArray(file, _terrain, 64, 64);
}

rectangle& cBigTown::room_rect(size_t i){
	return _room_rect[i];
}

cCreature& cBigTown::creatures(size_t i){
	return _creatures[i];
}

unsigned char& cBigTown::lighting(size_t i, size_t r){
	return _lighting[i][r];
}

cBigTown::cBigTown(){
	int i;
	cCreature dummy_creature;// = {0,0,loc(),0,0,0,0,0,0,0};
	RECT d_rect = {0,0,0,0};
	for (i = 0; i < 16; i++) {
		_room_rect[i].top = d_rect.top;
		_room_rect[i].left = d_rect.left;
		_room_rect[i].bottom = d_rect.bottom;
		_room_rect[i].right = d_rect.right;
	}
	for (i = 0; i < 60; i++) {
		_creatures[i] = dummy_creature;
	}
}

cMedTown::cMedTown(){
	int i;
	cCreature dummy_creature;// = {0,0,loc(),0,0,0,0,0,0,0};
	RECT d_rect = {0,0,0,0};
	for (i = 0; i < 16; i++) {
		_room_rect[i].top = d_rect.top;
		_room_rect[i].left = d_rect.left;
		_room_rect[i].bottom = d_rect.bottom;
		_room_rect[i].right = d_rect.right;
	}
	for (i = 0; i < 40; i++) {
		_creatures[i] = dummy_creature;
	}
}

cTinyTown::cTinyTown(){
	int i;
	cCreature dummy_creature;// = {0,0,loc(),0,0,0,0,0,0,0};
	RECT d_rect = {0,0,0,0};
	for (i = 0; i < 16; i++) {
		_room_rect[i].top = d_rect.top;
		_room_rect[i].left = d_rect.left;
		_room_rect[i].bottom = d_rect.bottom;
		_room_rect[i].right = d_rect.right;
	}
	for (i = 0; i < 30; i++) {
		_creatures[i] = dummy_creature;
	}
}

short cBigTown::max_dim(){
	return 64;
}

short cMedTown::max_dim(){
	return 48;
}

short cTinyTown::max_dim(){
	return 32;
}

short cBigTown::max_monst(){
	return 60;
}

short cMedTown::max_monst(){
	return 40;
}

short cTinyTown::max_monst(){
	return 30;
}

short cBigTown::max_items(){
	return 64;
}

short cMedTown::max_items(){
	return 64;
}

short cTinyTown::max_items(){
	return 64;
}
