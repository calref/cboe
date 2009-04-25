/*
 *  universe.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "classes.h"
#include "oldstructs.h"

__attribute__((deprecated))
void cCurTown::append(legacy::setup_save_type& old){
	for(int n = 0; n < 4; n++)
		for(int i = 0; i < 64; i++)
			for(int j = 0; j < 64; j++)
				setup[n][i][j] = old.setup[n][i][j];
}

__attribute__((deprecated))
void cCurOut::append(legacy::out_info_type& old){
	for(int i = 0; i < 96; i++)
		for(int j = 0; j < 96; j++)
			expl[i][j] = old.expl[i][j];
}

__attribute__((deprecated))
void cCurTown::append(legacy::current_town_type& old,short which_size){
	num = old.town_num;
	difficulty = old.difficulty;
	if(town != NULL) delete town;
	switch(which_size){
		case 0:
			town = new cBigTown;
			break;
		case 1:
			town = new cMedTown;
			break;
		case 2:
			town = new cTinyTown;
			break;
	}
	*town = old.town;
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++)
			explored[i][j] = old.explored[i][j];
	hostile = old.hostile;
	monst = old.monst;
	in_boat = old.in_boat;
	p_loc.x = old.p_loc.x;
	p_loc.y = old.p_loc.y;
	cur_talk = &town->talking;
	cur_talk_loaded = num;
}

__attribute__((deprecated))
void cCurTown::append(legacy::big_tr_type& old){
	int i,j;
	for(i = 0; i < town->max_dim(); i++)
		for(j = 0; j < town->max_dim(); j++)
			town->terrain(i,j) = old.terrain[i][j];
	for(i = 0; i < 16; i++){
		town->room_rect(i).top = old.room_rect[i].top;
		town->room_rect(i).left = old.room_rect[i].left;
		town->room_rect(i).bottom = old.room_rect[i].bottom;
		town->room_rect(i).right = old.room_rect[i].right;
	}
	for(i = 0; i < town->max_monst(); i++)
		town->creatures(i) = old.creatures[i];
	for(i = 0; i < town->max_dim() / 8; i++)
		for(j = 0; j < town->max_dim(); j++)
			town->lighting(i,j) = old.lighting[i][j];
}

__attribute__((deprecated))
void cCurTown::append(legacy::town_item_list& old){
	for(int i = 0; i < 115; i++)
		items[i] = old.items[i];
}

__attribute__((deprecated))
void cCurTown::append(legacy::stored_town_maps_type& old){
	for(int n = 0; n < 200; n++)
		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 64; j++)
				maps[n][i][j] = old.town_maps[n][i][j];
}

__attribute__((deprecated))
void cCurOut::append(legacy::stored_outdoor_maps_type& old){
	for(int n = 0; n < 100; n++)
		for(int i = 0; i < 6; i++)
			for(int j = 0; j < 48; j++)
				maps[n][i][j] = old.outdoor_maps[n][i][j];
}

__attribute__((deprecated))
void cCurOut::append(unsigned char(& old_sfx)[64][64], unsigned char(& old_misc_i)[64][64]){
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++){
			sfx[i][j] = old_sfx[i][j];
			misc_i[i][j] = old_misc_i[i][j];
		}
}
