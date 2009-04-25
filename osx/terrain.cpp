/*
 *  terrain.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "classes.h"
#include "oldstructs.h"

__attribute__((deprecated))
cTerrain& cTerrain::operator = (legacy::terrain_type_type& old){
	picture = old.picture;
	blockage = old.blockage;
	flag1 = old.flag1;
	flag2 = old.flag2;
	special = old.special;
	trans_to_what = old.trans_to_what;
	fly_over = old.fly_over;
	boat_over = old.boat_over;
	block_horse = old.block_horse;
	light_radius = old.light_radius;
	step_sound = old.step_sound;
	shortcut_key = old.shortcut_key;
	res1 = old.res1;
	res2 = old.res2;
	res3 = old.res3;
}