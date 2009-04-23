/*
 *  town.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "location.h"
#include "special.h"
#include "town.h"
#include "oldstructs.h"

__attribute__((deprecated))
void cTown::append(legacy::big_tr_type& old){}
__attribute__((deprecated))
void cTown::append(legacy::ave_tr_type& old){}
__attribute__((deprecated))
void cTown::append(legacy::tiny_tr_type& old){}

__attribute__((deprecated))
void cTinyTown::append(legacy::tiny_tr_type& old){
	int i,j;
	for (i = 0; i < 32; i++)
		for (j = 0; j < 32; j++) {
			_terrain[i][j] = old.terrain[i][j];
			_lighting[i / 8][j] = old.lighting[i / 8][j];					
		}
	for (i = 0; i < 16; i++) {
		_room_rect[i] = old.room_rect[i];
	}
	for (i = 0; i < 30; i++) {
		_creatures[i].number = old.creatures[i].number;
		_creatures[i].start_attitude = old.creatures[i].start_attitude;
		_creatures[i].start_loc.x = old.creatures[i].start_loc.x;
		_creatures[i].start_loc.y = old.creatures[i].start_loc.y;
		_creatures[i].mobile = old.creatures[i].mobile;
		_creatures[i].time_flag = old.creatures[i].time_flag;
		_creatures[i].extra1 = old.creatures[i].extra1;
		_creatures[i].extra2 = old.creatures[i].extra2;
		_creatures[i].spec1 = old.creatures[i].spec1;
		_creatures[i].spec2 = old.creatures[i].spec2;
		_creatures[i].spec_enc_code = old.creatures[i].spec_enc_code;
		_creatures[i].time_code = old.creatures[i].time_code;
		_creatures[i].monster_time = old.creatures[i].monster_time;
		_creatures[i].personality = old.creatures[i].personality;
		_creatures[i].special_on_kill = old.creatures[i].special_on_kill;
		_creatures[i].facial_pic = old.creatures[i].facial_pic;
	}
}

__attribute__((deprecated))
void cMedTown::append(legacy::ave_tr_type& old){
	int i,j;
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			_terrain[i][j] = old.terrain[i][j];
			_lighting[i / 8][j] = old.lighting[i / 8][j];					
		}
	for (i = 0; i < 16; i++) {
		_room_rect[i] = old.room_rect[i];
	}
	for (i = 0; i < 40; i++) {
		_creatures[i].number = old.creatures[i].number;
		_creatures[i].start_attitude = old.creatures[i].start_attitude;
		_creatures[i].start_loc.x = old.creatures[i].start_loc.x;
		_creatures[i].start_loc.y = old.creatures[i].start_loc.y;
		_creatures[i].mobile = old.creatures[i].mobile;
		_creatures[i].time_flag = old.creatures[i].time_flag;
		_creatures[i].extra1 = old.creatures[i].extra1;
		_creatures[i].extra2 = old.creatures[i].extra2;
		_creatures[i].spec1 = old.creatures[i].spec1;
		_creatures[i].spec2 = old.creatures[i].spec2;
		_creatures[i].spec_enc_code = old.creatures[i].spec_enc_code;
		_creatures[i].time_code = old.creatures[i].time_code;
		_creatures[i].monster_time = old.creatures[i].monster_time;
		_creatures[i].personality = old.creatures[i].personality;
		_creatures[i].special_on_kill = old.creatures[i].special_on_kill;
		_creatures[i].facial_pic = old.creatures[i].facial_pic;
	}
}

__attribute__((deprecated))
void cBigTown::append(legacy::big_tr_type& old){
	int i,j;
	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++) {
			_terrain[i][j] = old.terrain[i][j];
			_lighting[i / 8][j] = old.lighting[i / 8][j];					
		}
	for (i = 0; i < 16; i++) {
		_room_rect[i] = old.room_rect[i];
	}
	for (i = 0; i < 60; i++) {
		_creatures[i].number = old.creatures[i].number;
		_creatures[i].start_attitude = old.creatures[i].start_attitude;
		_creatures[i].start_loc.x = old.creatures[i].start_loc.x;
		_creatures[i].start_loc.y = old.creatures[i].start_loc.y;
		_creatures[i].mobile = old.creatures[i].mobile;
		_creatures[i].time_flag = old.creatures[i].time_flag;
		_creatures[i].extra1 = old.creatures[i].extra1;
		_creatures[i].extra2 = old.creatures[i].extra2;
		_creatures[i].spec1 = old.creatures[i].spec1;
		_creatures[i].spec2 = old.creatures[i].spec2;
		_creatures[i].spec_enc_code = old.creatures[i].spec_enc_code;
		_creatures[i].time_code = old.creatures[i].time_code;
		_creatures[i].monster_time = old.creatures[i].monster_time;
		_creatures[i].personality = old.creatures[i].personality;
		_creatures[i].special_on_kill = old.creatures[i].special_on_kill;
		_creatures[i].facial_pic = old.creatures[i].facial_pic;
	}
}

unsigned char& cTinyTown::terrain(size_t x, size_t y){
	return _terrain[x][y];
}
Rect& cTinyTown::room_rect(size_t i){
	return _room_rect[i];
}
cTown::cCreature& cTinyTown::creatures(size_t i){
	return _creatures[i];
}
unsigned char& cTinyTown::lighting(size_t i, size_t r){
	return _lighting[i][r];
}

unsigned char& cMedTown::terrain(size_t x, size_t y){
	return _terrain[x][y];
}
Rect& cMedTown::room_rect(size_t i){
	return _room_rect[i];
}
cTown::cCreature& cMedTown::creatures(size_t i){
	return _creatures[i];
}
unsigned char& cMedTown::lighting(size_t i, size_t r){
	return _lighting[i][r];
}

unsigned char& cBigTown::terrain(size_t x, size_t y){
	return _terrain[x][y];
}
Rect& cBigTown::room_rect(size_t i){
	return _room_rect[i];
}
cTown::cCreature& cBigTown::creatures(size_t i){
	return _creatures[i];
}
unsigned char& cBigTown::lighting(size_t i, size_t r){
	return _lighting[i][r];
}

__attribute__((deprecated))
cTown& cTown::operator = (legacy::town_record_type& old){
	int i,j;
	town_chop_time = old.town_chop_time;
	town_chop_key = old.town_chop_key;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++)
			wandering[i].monst[i] = old.wandering[i].monst[i];
		wandering_locs[i].x = old.wandering_locs[i].x;
		wandering_locs[i].y = old.wandering_locs[i].y;
		start_locs[i].x = old.start_locs[i].x;
		start_locs[i].y = old.start_locs[i].y;
		exit_locs[i].x = old.exit_locs[i].x;
		exit_locs[i].y = old.exit_locs[i].y;
		exit_specs[i] = old.exit_specs[i];
	}
	for(i = 0; i < 50; i++){
		special_locs[i].x = old.special_locs[i].x;
		special_locs[i].y = old.special_locs[i].y;
		spec_id[i] = old.spec_id[i];
		preset_fields[i].loc.x = old.preset_fields[i].field_loc.x;
		preset_fields[i].loc.y = old.preset_fields[i].field_loc.y;
		preset_fields[i].type = old.preset_fields[i].field_type;
	}
	for(i = 0; i < 15; i++){
		sign_locs[i].x = old.sign_locs[i].x;
		sign_locs[i].y = old.sign_locs[i].y;
	}
	lighting_type = old.lighting;
	in_town_rect.top = old.in_town_rect.top;
	in_town_rect.left = old.in_town_rect.left;
	in_town_rect.bottom = old.in_town_rect.bottom;
	in_town_rect.right = old.in_town_rect.right;
	for(i = 0; i < 64; i++){
		preset_items[i].loc.x = old.preset_items[i].item_loc.x;
		preset_items[i].loc.y = old.preset_items[i].item_loc.y;
		preset_items[i].code = old.preset_items[i].item_code;
		preset_items[i].ability = old.preset_items[i].ability;
		preset_items[i].charges = old.preset_items[i].charges;
		preset_items[i].always_there = old.preset_items[i].always_there;
		preset_items[i].property = old.preset_items[i].property;
		preset_items[i].contained = old.preset_items[i].contained;
	}
	max_num_monst = old.max_num_monst;
	spec_on_entry = old.spec_on_entry;
	spec_on_entry_if_dead = old.spec_on_entry_if_dead;
	for(i = 0; i < 8; i++){
		timer_spec_times[i] = old.timer_spec_times[i];
		timer_specs[i] = old.timer_specs[i];
	}
	for(i = 0; i < 180; i++)
		strlens[i] = old.strlens[i];
	for(i = 0; i < 100; i++)
		specials[i] = old.specials[i];
	specials1 = old.specials1;
	specials2 = old.specials2;
	res1 = old.res1;
	res2 = old.res2;
	difficulty = old.difficulty;
}

cTown::cTown(){}

extern short max_dim[3];
cTown::cTown(short size){
	short i,j,s;
	location d_loc = {100,0};
	cTown::cWandering d_wan = {0,0,0,0};
	cTown::cItem null_item = {{0,0},-1,0,0,0,0,0};
	Str255 temp_str;
	
	town_chop_time = -1;
	town_chop_key = -1;
	for (i = 0; i < 4; i++) {
		wandering[i] = d_wan;
		wandering_locs[i] = d_loc;
	}
	for (i = 0; i < 50; i++) {
		special_locs[i] = d_loc;
		special_locs[i].x = 100;
		spec_id[i] = -1;
	}
	lighting_type = 0;
	for (i = 0; i < 4; i++) {
		start_locs[i] = d_loc;
		exit_specs[i] = -1;
		exit_locs[i].x = -1;
		exit_locs[i].y = -1;
	}
//	s = town->max_dim();
	start_locs[0].x = s / 2;
	start_locs[0].y = 4;
	start_locs[2].x = s / 2;
	start_locs[2].y = s - 5;
	start_locs[1].x = s - 5;
	start_locs[1].y = s / 2;
	start_locs[3].x = 4;
	start_locs[3].y = s / 2;
	in_town_rect.top = 3;
	in_town_rect.bottom = s - 4;
	in_town_rect.left = 3;
	in_town_rect.right = s - 4;
	for (i = 0; i < 64; i++) 
		preset_items[i] = null_item;
	max_num_monst = 30000;
	for (i = 0; i < 50; i++) 
		preset_fields[i].type = 0;
	spec_on_entry = -1;
	spec_on_entry_if_dead = -1;
	for (i = 0; i < 15; i++) {
		sign_locs[i] = d_loc;
		sign_locs[i].x = 100;
	}	
	for (i = 0; i < 8; i++) {
		timer_spec_times[i] = 0;
		timer_specs[i] = -1;
	}	
	for (i = 0; i < 100; i++) {
		specials[i] = cSpecial();
	}
	specials1 = 0;
	specials2 = 0;
	res1 = 0;
	res2 = 0;
	difficulty = 0;
}

cBigTown::cBigTown(){
	int i;
	cCreature dummy_creature = {0,0,{0,0},0,0,0,0,0,0,0};
	Rect d_rect = {0,0,0,0};
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
	cCreature dummy_creature = {0,0,{0,0},0,0,0,0,0,0,0};
	Rect d_rect = {0,0,0,0};
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
	cCreature dummy_creature = {0,0,{0,0},0,0,0,0,0,0,0};
	Rect d_rect = {0,0,0,0};
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

__attribute__((deprecated))
char(& cTown::town_strs(short i))[256]{
	if(i == 0) return town_name;
	if(i >= 1   && i < 17 ) return rect_names[i - 1];
	if(i >= 17  && i < 20 ) return comment[i - 17];
	if(i >= 20  && i < 120) return spec_strs[i - 20];
	if(i >= 120 && i < 140) return sign_strs[i - 120];
	return comment[1]; // random unused string
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
