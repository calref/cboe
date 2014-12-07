/*
 *  terrain.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "classes.h"
#include "oldstructs.h"
#include "boe.consts.h" // TODO: Put these constants in a global file

cTerrain& cTerrain::operator = (legacy::terrain_type_type& old){
	static const short arenas[274] = {
		1, 1, 0, 0, 0, 1, 1, 1, 1, 1,			//  0 - grassy field
		1, 1, 1, 1, 1, 1, 1, 1, 2, 2,			//  1 - ordinary cave
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2,			//  2 - mountain
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2,			//  3 - surface bridge
		2, 2, 2, 2, 2, 2, 0, 0, 0, 0,			//  4 - cave bridge
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,/* 50  */	//  5 - rubble-strewn cave
		0, 3, 3, 3, 3, 3, 3, 5, 5, 5,			//  6 - cave tree forest
		6, 6, 7, 7, 1, 1, 8, 9, 10,11,			//  7 - cave mushrooms
		11,11,12,13,13,9, 9, 9, 1, 1,			//  8 - cave swamp
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			//  9 - surface rocks
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,/* 100 */	// 10 - surface swamp
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 11 - surface woods
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 12 - shrubbery
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 13 - stalagmites
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 14 - cave fumarole (proposed)
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,/* 150 */	// 15 - surface fumarole (proposed)
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 16 - cave road (proposed)
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 17 - surface road (proposed)
		1, 0, 1, 1, 1, 1, 1, 1, 1, 0,			// 18 - hills road (proposed)
		0, 0, 0, 0, 1, 0, 0, 0, 0, 0,			// 19 - crops (proposed)
		0, 0, 1, 0, 2, 0, 0, 1, 1, 1,/* 200 */	// 20 - cave camp (proposed)
		1, 0, 2, 1, 1, 1, 0, 1, 1, 1,			// 21 - surface camp (proposed)
		1, 1, 0, 0, 0, 0, 1, 0, 1, 1,			// (note: fumaroles would have lava.)
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// the numbers in this array are indices into the other arrays
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// (ter_base, ground_type, and terrain_odds first index)
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 250 */
		4, 4, 4, 4, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1
	};
	static const short ground[274] = {
		0, 0, 1, 1, 1, 2, 2, 2, 2, 2,   2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3,   3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 5, 5, 5, 5,   5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 0, 0, 0,   0, 0, 0, 0, 9, 0, 0, 1, 1, 1,
		1, 1, 1, 0, 0, 1, 1, 1, 10,10,  10,10,10,10,10,10,10,10,10,10,
		11,11,11,11,11,11,11,11,11,11,  12,12,12,12,12,12,12,12,12,12,
		12,0, 0, 13,13,13,13,13,13,13,  13,13,13,13,13,13,13,13,13,13,
		13,13,13,13,13,13,13,13,13,13,  13,13,13,13,13,13,13,14,14,14,
		14,14,14,15,15,15,15,15,15,15,  15,15,15,15,15,15,15,15,0, 0,
		0, 16,0, 0, 0, 0, 0, 0, 0, 1,   1, 1, 3, 3, 3, 3, 1, 1, 1, 1,
		1, 1, 0, 1, 4, 1, 1, 0, 13,14,  15,1, 4, 13,13,17,17,0, 17,17,
		17,17,17,17,17,17,0, 1, 18,19,  13,20,0, 13,0, 0, 0, 0, 0, 0,
		2, 2, 2, 2, 0, 0, 15,15,15,15,  15,13,13,1, 1, 1, 1, 1, 1, 4,
		6, 6, 6, 6, 7, 6, 0, 21,22,23,  24,0, 13,13
	};
	static const short trims[274] = {
		0, 0, 0, 0, 0, 1, 2, 3, 4, 5,   6, 7, 8, 9, 10,11,12,13,1, 1,
		2, 3, 4, 5, 6, 7, 8, 9, 10,11,  12,13,0, 0, 2, 3, 4, 5, 6, 7,
		8, 9, 10,11,12,13,0, 2, 3, 4,   5, 6, 7, 8, 9, 10,11,12,13,0,
		0, 8, 0, 4, 6, 0, 2, 0, 0, 0,   0, 0, 0, 0, 14,0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 18,18,18,  18,18,6, 4, 2, 8, 18,18,0, 0,
		18,18,15,15,15,0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 16,16,0, 16,16,
		16,16,16,16,16,16,0, 0, 0, 0,   0, 0, 18,0, 0, 0, 18,18,18,18,
		2, 4, 6, 8, 18,18,0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		14,14,14,14,14,17,18,0, 0, 0,   0, 0, 0, 0,
	};
	static const short trim_ters[274] = {
		99,99,99,99,99,0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 99,99,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4,   4, 4, 99,99,1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 99,1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1, 1, 99,
		99,1, 99,1, 1, 99,1, 99,99,99,  99,99,99,99,99,99,99,99,99,99,
		99,99,99,99,99,99,99,99,99,99,  99,99,99,99,99,99,99,99,99,99,
		99,99,99,99,99,99,99,99,99,99,  99,99,99,99,99,99,99,99,99,99,
		99,99,99,99,99,99,99,99,99,99,  99,99,99,99,99,99,99,99,99,99,
		99,99,99,99,99,99,99,99,99,99,  99,99,99,99,99,99,99,99,99,99,
		99,99,99,99,99,99,99,99,99,99,  99,99,99,99,99,99,99,99,99,99,
		99,99,99,99,99,99,99,99,99,99,  99,99,4, 4, 4, 4, 99,99,99,99,
		99,99,99,99,99,99,99,99,99,99,  99,99,99,99,99,0, 1, 99,0, 0,
		0, 0, 1, 1, 1, 1, 99,99,99,99,  99,99,99,99,99,99,99,99,99,99,
		0, 0, 0, 0, 99,99,99,99,99,99,  99,99,99,99,99,99,99,99,99,99,
		99,99,99,99,99,2, 99,99,99,99,  99,99,99,99,
	};
	picture = old.picture;
	blockage = (eTerObstruct) old.blockage;
	if(picture < 260){
		combat_arena = arenas[picture];
		ground_type = ground[picture];
		trim_type = (eTrimType) trims[picture];
		trim_ter = trim_ters[picture];
	}else if(picture >= 400 && picture < 1000){
		combat_arena = arenas[picture - 140];
		ground_type = ground[picture - 140];
		trim_type = (eTrimType) trims[picture - 140];
		trim_ter = trim_ters[picture - 140];
	}else{
		combat_arena = 1;
		ground_type = 255;
		trim_type = eTrimType::NONE;
		trim_ter = 0;
	}
	if(trim_ter == 99) trim_ter = 0;
	flag1.u = old.flag1;
	flag2.u = old.flag2;
	switch(old.special){
		case 0:
			if(i == 7 || i == 10 || i == 13 || i == 16){
				special = eTerSpec::NONE;
				flag1.s = 23;
				flag2.u = flag3.u = 0;
			}else if(picture == 215 || (picture >= 218 && picture <= 221)){
				picture = 215;
				special = eTerSpec::NONE;
				flag1.s = 3;
				flag2.u = flag3.u = 0;
			}else if(picture == 216 || (picture >= 222 && picture <= 225)){
				picture = 215;
				special = eTerSpec::NONE;
				flag1.s = 2;
				flag2.u = flag3.u = 0;
			}else if(picture == 143) {
				special = eTerSpec::BED;
				flag1.s = 230;
				flag2.u = flag3.u = 0;
			}else if((picture >= 61 && picture <= 66) || picture == 401 || picture == 402){
				special = eTerSpec::BRIDGE;
				flag1.u = flag2.u = flag3.u = 0;
				break;
			}else{
				special = eTerSpec::NONE;
				flag1.s = -1;
				flag2.u = flag3.u = 0;
			}
			break;
		case 1:
			special = eTerSpec::CHANGE_WHEN_STEP_ON;
			flag3.u = 0;
			break;
		case 2:
			special = eTerSpec::DAMAGING;
			flag3.u = DAMAGE_FIRE;
			break;
		case 3:
			special = eTerSpec::DAMAGING;
			flag3.u = DAMAGE_COLD;
			break;
		case 4:
			special = eTerSpec::DAMAGING;
			flag3.u = DAMAGE_MAGIC;
			break;
		case 5:
			special = eTerSpec::DANGEROUS;
			flag3.u = (int)eStatus::POISON;
			break;
		case 6:
			special = eTerSpec::DANGEROUS;
			flag3.u = (int)eStatus::DISEASE;
			break;
		case 7:
			special = eTerSpec::CRUMBLING;
			flag2.u = 0; // ???: may change this
			flag3.u = 1; // destroyed by Move Mountains but not by quickfire; 0 = both, 2 = quickfire only
			break;
		case 8:
			special = eTerSpec::LOCKABLE;
			flag3.u = 0;
			break;
		case 9:
			special = eTerSpec::UNLOCKABLE;
			flag3.u = false; // can't bash
			break;
		case 10:
			special = eTerSpec::UNLOCKABLE;
			flag3.u = true; // can bash
			break;
		case 11:
			special = eTerSpec::IS_A_SIGN;
			flag3.u = 0;
			break;
		case 12:
			special = eTerSpec::CALL_SPECIAL;
			flag2.u = 0; // local special, always (1 would be local if in town, global if outdoors)
			flag3.s = -1;
			break;
		case 13:
			special = eTerSpec::CALL_SPECIAL;
			flag2.u = 3; // global special, always (2 would be local if outdoors, global if in town)
			flag3.s = -1;
			break;
		case 14:
			special = eTerSpec::IS_A_CONTAINER;
			flag3.u = 0;
			break;
		case 15:
			special = eTerSpec::WATERFALL;
			flag1.u = DIR_S;
			flag3.u = 0;
			break;
		case 16:
			special = eTerSpec::CONVEYOR;
			flag1.u = DIR_N;
			flag3.u = 0;
			break;
		case 17:
			special = eTerSpec::CONVEYOR;
			flag1.u = DIR_E;
			flag3.u = 0;
			break;
		case 18:
			special = eTerSpec::CONVEYOR;
			flag1.u = DIR_S;
			flag3.u = 0;
			break;
		case 19:
			special = eTerSpec::CONVEYOR;
			flag1.u = DIR_W;
			flag3.u = 0;
			break;
		case 20:
			special = eTerSpec::BLOCKED_TO_MONSTERS;
			flag3.u = 0;
			break;
		case 21:
			special = eTerSpec::TOWN_ENTRANCE;
			flag3.u = 0;
			break;
		case 22:
			special = eTerSpec::CHANGE_WHEN_USED;
			flag2.u = 3;
			flag3.u = 0;
			break;
		case 23:
			special = eTerSpec::CALL_SPECIAL_WHEN_USED;
			flag2.u = 3; // global special, always (2 would be local if outdoors, global if in town)
			flag3.s = -1;
			break;
	}
	trans_to_what = old.trans_to_what;
	fly_over = old.fly_over;
	boat_over = old.boat_over;
	if(special == eTerSpec::DANGEROUS) block_horse = false; // because it's now redundant and unhelpful
	else block_horse = old.block_horse;
	light_radius = old.light_radius;
	step_sound = old.step_sound;
	if(step_sound > 99) step_sound = 99;
	shortcut_key = old.shortcut_key;
	switch(picture){
		// Rubbles, plus pentagram as a bonus
		case 68:
			obj_num = 1;
			obj_pos.x = 0;
			obj_pos.y = 0;
			obj_size.x = 2;
			obj_size.y = 1;
			break;
		case 69:
			obj_num = 1;
			obj_pos.x = 1;
			obj_pos.y = 0;
			obj_size.x = 2;
			obj_size.y = 1;
			break;
		case 86:
			obj_num = 2;
			obj_pos.x = 0;
			obj_pos.y = 0;
			obj_size.x = 2;
			obj_size.y = 1;
			break;
		case 87:
			obj_num = 2;
			obj_pos.x = 1;
			obj_pos.y = 0;
			obj_size.x = 2;
			obj_size.y = 1;
			break;
		case 247:
			obj_num = 3;
			obj_pos.x = 0;
			obj_pos.y = 0;
			obj_size.x = 2;
			obj_size.y = 2;
			break;
		case 248:
			obj_num = 3;
			obj_pos.x = 1;
			obj_pos.y = 0;
			obj_size.x = 2;
			obj_size.y = 2;
			break;
		case 249:
			obj_num = 3;
			obj_pos.x = 0;
			obj_pos.y = 1;
			obj_size.x = 2;
			obj_size.y = 2;
			break;
		case 250:
			obj_num = 3;
			obj_pos.x = 1;
			obj_pos.y = 1;
			obj_size.x = 2;
			obj_size.y = 2;
			break;
		// Special spaces
		case 207:
			picture = 0;
			i = 3000;
			break;
		case 208:
			picture = 123;
			i = 3000;
			break;
		case 209:
			picture = 157;
			i = 3000;
			break;
		case 210:
			picture = 163;
			i = 3000;
			break;
		case 211:
			picture = 2;
			i = 3000;
			break;
		case 212:
			picture = 32;
			i = 3000;
			break;
		// Misc
		case 216:
		case 218: case 219: case 220: case 221:
		case 222: case 223: case 224: case 225:
			picture = 215;
			break;
		case 233:
			picture = 137;
			break;
	};
	return *this;
}

std::ostream& operator << (std::ostream& out, eTerSpec& e){
	return out << (int) e;
}

std::istream& operator >> (std::istream& in, eTerSpec& e){
	int i;
	in >> i;
	if(i > 0 && i < 24)
		e = (eTerSpec) i;
	else e = eTerSpec::NONE;
	return in;
}

std::ostream& operator << (std::ostream& out, eTerObstruct& e){
	return out << (int) e;
}

std::istream& operator >> (std::istream& in, eTerObstruct& e){
	int i;
	in >> i;
	if(i > 0 && i < 6)
		e = (eTerObstruct) i;
	else e = eTerObstruct::CLEAR;
	return in;
}
