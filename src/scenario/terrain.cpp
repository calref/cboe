/*
 *  terrain.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "terrain.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <set>

#include "oldstructs.hpp"
#include "gfx/gfxsheets.hpp" // for NO_PIC
#include "damage.hpp"

void cTerrain::import_legacy(legacy::terrain_type_type& old){
	static const std::set<int> archetypes = {
		// This lists graphics that represent the archetypal terrains for each ground type
		// One line per graphics sheet; last line is the animated sheet
		0, 2, 5, 18, 32, 46,
		74, 88,
		100, 110, 123,
		157, 163,
		215, 216,
		400, 404,
	};
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
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 14 - cave fumarole
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,/* 150 */	// 15 - surface fumarole
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 16 - cave camp
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,			// 17 - surface camp
		1, 0, 1, 1, 1, 1, 1, 1, 1, 0,			// 18 - cave crops (uses base terrain for crops)
		0, 0, 0, 0, 1, 0, 0, 0, 0, 0,			// 19 - surface crops (as above)
		0, 0, 1, 0, 2, 0, 0, 1, 1, 1,/* 200 */	// (note: fumaroles would have lava.)
		1, 0, 2, 1, 1, 1, 0, 1, 1, 1,
		1, 1, 0, 0, 0, 0, 1, 0, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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
		0, 1 ,0, 0, 0, 0, 0, 0, 0, 1,   1, 1, 3, 3, 3, 3, 1, 1, 1, 1,
		1, 1, 0, 1, 4, 1, 1, 0, 13,14,  15,1, 4, 13,13,16,17,0, 16,16,
		16,16,17,17,17,17,0, 1, 18,19,  13,20,0, 13,0, 0, 0, 0, 0, 0,
		2, 2, 2, 2, 0, 0, 15,15,15,15,  15,13,13,1, 1, 1, 1, 1, 1, 4,
		6, 6, 6, 6, 7, 6, 0, 0, 0, 0,   0, 0, 13,13
	};
	static const short trims[274] = {
		0, 0, 0, 0, 0, 0, 2, 3, 4, 5,   6, 7, 8, 9, 10,11,12,13,0, 0,
		2, 3, 4, 5, 6, 7, 8, 9, 10,11,  12,13,0, 0, 2, 3, 4, 5, 6, 7,
		8, 9, 10,11,12,13,0, 2, 3, 4,   5, 6, 7, 8, 9, 10,11,12,13,0,
		0,18, 0,18,18, 0,18, 0, 0, 0,   0, 0, 0, 0, 14,0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 18,18,18,  18,18,18,18,18,18,18,18,0, 0,
		18,18,0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 16,16,0, 16,16,
		16,16,16,16,16,16,0, 0, 0, 0,   0, 0, 18,0, 0, 0, 18,18,18,18,
		18,18,18,18,18,18,0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		14,14,14,14,14,17,18,0, 0, 0,   0, 0, 0, 0,
	};
	static const short trim_ters[274] = {
		99,99,99,99,99,0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4,   4, 4, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1, 1, 99,
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
	// This is a weird expression, but necessary, because all archetypes except lava lack a special ability
	is_archetype = archetypes.count(picture) && (old.special == 0) == (old.picture != 404);
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
		picture += 560;
	}else{
		combat_arena = 1;
		ground_type = 255;
		trim_type = eTrimType::NONE;
		trim_ter = 0;
	}
	if(trim_ter == 99) trim_ter = -1;
	flag1 = old.flag1;
	flag2 = old.flag2;
	switch(old.special){
		case 0:
			if(i == 7 || i == 10 || i == 13 || i == 16){
				special = eTerSpec::NONE;
				flag1 = 87;
				flag2 = flag3 = 0;
			}else if(picture == 215 || (picture >= 218 && picture <= 221)){
				picture = 215;
				special = eTerSpec::NONE;
				flag1 = 63;
				flag2 = flag3 = 0;
			}else if(picture == 216 || (picture >= 222 && picture <= 225)){
				picture = 215;
				special = eTerSpec::NONE;
				flag1 = 62;
				flag2 = flag3 = 0;
			}else if(picture == 143) {
				special = eTerSpec::BED;
				flag1 = 230;
				flag2 = flag3 = 0;
			}else if((picture >= 61 && picture <= 66) || picture == 961 || picture == 962){
				special = eTerSpec::BRIDGE;
				flag1 = flag2 = flag3 = 0;
				break;
			}else{
				special = eTerSpec::NONE;
				flag1 = -1;
				flag2 = flag3 = 0;
			}
			break;
		case 1:
			special = eTerSpec::CHANGE_WHEN_STEP_ON;
			if(flag2 == 200) flag2 = -1;
			flag3 = 0;
			break;
		case 2:
			special = eTerSpec::DAMAGING;
			flag3 = int(eDamageType::FIRE);
			break;
		case 3:
			special = eTerSpec::DAMAGING;
			flag3 = int(eDamageType::COLD);
			break;
		case 4:
			special = eTerSpec::DAMAGING;
			flag3 = int(eDamageType::MAGIC);
			break;
		case 5:
			special = eTerSpec::DANGEROUS;
			flag3 = (int)eStatus::POISON;
			break;
		case 6:
			special = eTerSpec::DANGEROUS;
			flag3 = (int)eStatus::DISEASE;
			break;
		case 7:
			special = eTerSpec::CRUMBLING;
			flag2 = 0; // destroyed by Move Mountains but not by quickfire; 1 = both, 2 = quickfire only
			break;
		case 8:
			special = eTerSpec::LOCKABLE;
			flag3 = 0;
			break;
		case 9:
			special = eTerSpec::UNLOCKABLE;
			flag3 = false; // can't bash
			break;
		case 10:
			special = eTerSpec::UNLOCKABLE;
			flag3 = true; // can bash
			break;
		case 11:
			special = eTerSpec::IS_A_SIGN;
			flag3 = 0;
			break;
		case 12:
			special = eTerSpec::CALL_SPECIAL;
			flag2 = 1; // local special
			flag3 = -1;
			break;
		case 13:
			special = eTerSpec::CALL_SPECIAL;
			flag2 = 0; // global special
			flag3 = -1;
			break;
		case 14:
			special = eTerSpec::IS_A_CONTAINER;
			flag3 = 0;
			break;
		case 15:
			special = eTerSpec::WATERFALL_CAVE;
			flag1 = DIR_S;
			flag2 = 5;
			flag3 = 90;
			break;
		case 16:
			special = eTerSpec::CONVEYOR;
			flag1 = DIR_N;
			flag3 = 0;
			break;
		case 17:
			special = eTerSpec::CONVEYOR;
			flag1 = DIR_E;
			flag3 = 0;
			break;
		case 18:
			special = eTerSpec::CONVEYOR;
			flag1 = DIR_S;
			flag3 = 0;
			break;
		case 19:
			special = eTerSpec::CONVEYOR;
			flag1 = DIR_W;
			flag3 = 0;
			break;
		case 20:
			special = eTerSpec::BLOCKED_TO_MONSTERS;
			flag3 = 0;
			break;
		case 21:
			special = eTerSpec::TOWN_ENTRANCE;
			flag3 = 0;
			break;
		case 22:
			special = eTerSpec::CHANGE_WHEN_USED;
			flag3 = 0;
			break;
		case 23:
			special = eTerSpec::CALL_SPECIAL_WHEN_USED;
			flag2 = 0; // global special
			flag3 = -1;
			break;
	}
	trans_to_what = old.trans_to_what;
	fly_over = old.fly_over;
	boat_over = old.boat_over;
	if(special == eTerSpec::DANGEROUS) block_horse = false; // because it's now redundant and unhelpful
	else block_horse = old.block_horse;
	light_radius = old.light_radius;
	step_sound = eStepSnd(old.step_sound);
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
			picture = 210;
			obj_num = 3;
			obj_pos.x = 0;
			obj_pos.y = 0;
			obj_size.x = 2;
			obj_size.y = 2;
			break;
		case 248:
			picture = 211;
			obj_num = 3;
			obj_pos.x = 1;
			obj_pos.y = 0;
			obj_size.x = 2;
			obj_size.y = 2;
			break;
		case 249:
			picture = 212;
			obj_num = 3;
			obj_pos.x = 0;
			obj_pos.y = 1;
			obj_size.x = 2;
			obj_size.y = 2;
			break;
		case 250:
			picture = 213;
			obj_num = 3;
			obj_pos.x = 1;
			obj_pos.y = 1;
			obj_size.x = 2;
			obj_size.y = 2;
			break;
		// Roads
		case 202:
			picture = 0;
			i = 3001;
			break;
		case 203:
			picture = 2;
			i = 3001;
			break;
		case 204:
			picture = 32;
			i = 3001;
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
		case 215:
		case 218: case 219: case 220: case 221:
		case 222: case 223: case 224: case 225:
			picture = 216;
			break;
		case 233:
			picture = 137;
			break;
		case 213:
			picture = 214;
			break;
		case 214:
			picture = 215;
			break;
		case 246:
			picture = 209;
			break;
		case 251:
			picture = 207;
			break;
		case 252:
			picture = 208;
			break;
	};
	if(picture < 1000) map_pic = picture;
	else map_pic = NO_PIC;
	if(i == 1)
		frill_for = 0, frill_chance = 10;
	else if(i == 3)
		frill_for = 2, frill_chance = 15;
	else if(i == 4)
		frill_for = 2, frill_chance = 10;
	else if(i == 37)
		frill_for = 36, frill_chance = 25;
}

bool cTerrain::blocksMove() const {
	int code = (int) blockage;
	return code > 2;
}
