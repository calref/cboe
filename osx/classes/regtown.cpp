/*
 *  regtown.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "classes.h"
#include "oldstructs.h"

__attribute__((deprecated))
void cTinyTown::append(legacy::tiny_tr_type& old){
	int i,j;
	for (i = 0; i < 32; i++)
		for (j = 0; j < 32; j++) {
			_terrain[i][j] = old.terrain[i][j];
			_lighting[i / 8][j] = old.lighting[i / 8][j];					
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

__attribute__((deprecated))
void cMedTown::append(legacy::ave_tr_type& old){
	int i,j;
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			_terrain[i][j] = old.terrain[i][j];
			_lighting[i / 8][j] = old.lighting[i / 8][j];					
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

__attribute__((deprecated))
void cBigTown::append(legacy::big_tr_type& old){
	int i,j;
	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++) {
			_terrain[i][j] = old.terrain[i][j];
			_lighting[i / 8][j] = old.lighting[i / 8][j];					
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

unsigned short& cTinyTown::terrain(size_t x, size_t y){
	return _terrain[x][y];
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

unsigned short& cMedTown::terrain(size_t x, size_t y){
	return _terrain[x][y];
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

unsigned short& cBigTown::terrain(size_t x, size_t y){
	return _terrain[x][y];
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
	cCreature dummy_creature;// = {0,0,loc(),0,0,0,0,0,0,0};
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
	cCreature dummy_creature;// = {0,0,loc(),0,0,0,0,0,0,0};
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
