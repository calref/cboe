/*
 *  universe.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>


#include "classes.h"
#include "oldstructs.h"
#include "mathutil.h"

void cCurOut::append(legacy::out_info_type& old){
	for(int i = 0; i < 96; i++)
		for(int j = 0; j < 96; j++)
			expl[i][j] = old.expl[i][j];
}

void cCurTown::append(legacy::current_town_type& old,short which_size){
	num = old.town_num;
	difficulty = old.difficulty;
	if(record != NULL) delete record;
	switch(which_size){
		case 0:
			record = new cBigTown;
			break;
		case 1:
			record = new cMedTown;
			break;
		case 2:
			record = new cTinyTown;
			break;
	}
	*record = old.town;
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++)
			fields[i][j] = old.explored[i][j];
	hostile = old.hostile;
	monst = old.monst;
	in_boat = old.in_boat;
	p_loc.x = old.p_loc.x;
	p_loc.y = old.p_loc.y;
	curTalk = &record->talking;
	talkNeedsDeleting = false;
	cur_talk_loaded = num;
}

void cCurTown::append(legacy::big_tr_type& old){
	int i,j;
	for(i = 0; i < record->max_dim(); i++)
		for(j = 0; j < record->max_dim(); j++)
			record->terrain(i,j) = old.terrain[i][j];
	for(i = 0; i < 16; i++){
		record->room_rect(i).top = old.room_rect[i].top;
		record->room_rect(i).left = old.room_rect[i].left;
		record->room_rect(i).bottom = old.room_rect[i].bottom;
		record->room_rect(i).right = old.room_rect[i].right;
	}
	for(i = 0; i < record->max_monst(); i++)
		record->creatures(i) = old.creatures[i];
	for(i = 0; i < record->max_dim() / 8; i++)
		for(j = 0; j < record->max_dim(); j++)
			record->lighting(i,j) = old.lighting[i][j];
}

void cCurTown::append(legacy::town_item_list& old){
	for(int i = 0; i < 115; i++)
		items[i] = old.items[i];
}

void cUniverse::append(legacy::stored_town_maps_type& old){
	for(int n = 0; n < 200; n++)
		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 64; j++)
				town_maps[n][i][j] = old.town_maps[n][i][j];
}

void cUniverse::append(legacy::stored_outdoor_maps_type& old){
	for(int n = 0; n < 100; n++)
		for(int i = 0; i < 6; i++)
			for(int j = 0; j < 48; j++)
				out_maps[n][i][j] = old.outdoor_maps[n][i][j];
}

void cCurTown::append(unsigned char(& old_sfx)[64][64], unsigned char(& old_misc_i)[64][64]){
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++){
			unsigned long tmp_sfx, tmp_misc_i;
			tmp_sfx = old_sfx[i][j];
			tmp_misc_i = old_misc_i[i][j];
			tmp_sfx <<= 16;
			tmp_misc_i <<= 8;
			fields[i][j] |= tmp_sfx;
			fields[i][j] |= tmp_misc_i;
		}
}

cTown* cCurTown::operator -> (){
	return record;
}

bool cCurTown::loaded() const{
	return record != NULL;
}

void cCurTown::unload(){
	delete record;
	record = NULL;
}


cSpeech& cCurTown::cur_talk() {
	// Make sure we actually have a valid speech stored
	if(curTalk == NULL) prep_talk(num);
	return *curTalk;
}

bool cCurTown::prep_talk(short which) {
	if(which == cur_talk_loaded) return true;
	if(talkNeedsDeleting && curTalk != NULL) delete curTalk;
	cur_talk_loaded = which;
	if(which == num) {
		curTalk = &record->talking;
		talkNeedsDeleting = false;
		return true;
	} else {
		curTalk = new cSpeech;
		talkNeedsDeleting = true;
		return false;
	}
}

cCurTown::~cCurTown() {
	if(talkNeedsDeleting && curTalk != NULL) delete curTalk;
}

bool cCurTown::is_explored(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 1L;
}

bool cCurTown::is_force_wall(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 2L;
}

bool cCurTown::is_fire_wall(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 4L;
}

bool cCurTown::is_antimagic(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 8L;
}

bool cCurTown::is_scloud(char x, char y) const{ // stinking cloud
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 16L;
}

bool cCurTown::is_ice_wall(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 32L;
}

bool cCurTown::is_blade_wall(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 64L;
}

bool cCurTown::is_sleep_cloud(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 128L;
}

bool cCurTown::is_block(char x, char y) const{ // currently unused
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 256L;
}

bool cCurTown::is_spot(char x, char y) const{
	return special_spot[x][y];
}

bool cCurTown::is_special(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 512L;
}

bool cCurTown::is_web(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 1024L;
}

bool cCurTown::is_crate(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 2048L;
}

bool cCurTown::is_barrel(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 4096L;
}

bool cCurTown::is_fire_barr(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 8192L;
}

bool cCurTown::is_force_barr(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 16384L;
}

bool cCurTown::is_quickfire(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 32768L;
}

bool cCurTown::is_sm_blood(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 65536L;
}

bool cCurTown::is_med_blood(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 131072L;
}

bool cCurTown::is_lg_blood(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 262144L;
}

bool cCurTown::is_sm_slime(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 524288L;
}

bool cCurTown::is_lg_slime(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 1048576L;
}

bool cCurTown::is_ash(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 2097152L;
}

bool cCurTown::is_bones(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 4194304L;
}

bool cCurTown::is_rubble(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 8388608L;
}

bool cCurTown::is_force_cage(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return false;
	return fields[x][y] & 16777216L;
}

//bool cCurTown::is_trim(char x, char y, char t){
//	unsigned char bit = 1 << t;
//	return trim[x][y] & bit;
//}

bool cCurTown::set_explored(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b) fields[x][y] |=  1L;
	else  fields[x][y] &= ~1L;
	return true;
}

bool cCurTown::set_force_wall(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on space, there's no room for field.
		if(is_antimagic(x,y) || is_blade_wall(x,y) || is_quickfire(x,y))
			return false;
		if(is_crate(x,y) || is_barrel(x,y) || is_fire_barr(x,y) || is_force_barr(x,y))
			return false;
		fields[x][y]  |=  2L;
		set_web(x,y,false);
		set_fire_wall(x,y,false);
	}
	else fields[x][y] &= ~2L;
	return true;
}

bool cCurTown::set_fire_wall(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on space, there's no room for field.
		if(is_antimagic(x,y) || is_blade_wall(x,y) || is_quickfire(x,y) || is_ice_wall(x,y))
			return false;
		if(is_crate(x,y) || is_barrel(x,y) || is_fire_barr(x,y) || is_force_barr(x,y))
			return false;
		if(is_web(x,y) || is_scloud(x,y) || is_sleep_cloud(x,y))
			return false;
		fields[x][y]  |=  4L;
		set_web(x,y,false);
		set_fire_wall(x,y,false);
	}
	else fields[x][y] &= ~4L;
	return true;
}

bool cCurTown::set_antimagic(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on space, there's no room for a field.
		if(is_quickfire(x,y) || is_force_wall(x,y) || is_fire_wall(x,y))
			return false;
		fields[x][y]  |=  8L;
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
		set_antimagic(x,y,false);
		set_scloud(x,y,false);
		set_ice_wall(x,y,false);
		set_blade_wall(x,y,false);
		set_sleep_cloud(x,y,false);
	}
	else fields[x][y] &= ~8L;
	return true;
}

bool cCurTown::set_scloud(char x, char y, bool b){ // stinking cloud
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on space, there's no room for cloud.
		if(is_force_wall(x,y) || is_fire_wall(x,y) || is_ice_wall(x,y) || is_blade_wall(x,y))
			return false;
		if(is_antimagic(x,y) || is_sleep_cloud(x,y) || is_quickfire(x,y))
			return false;
		if(is_fire_barr(x,y) || is_force_barr(x,y))
			return false;
		fields[x][y]  |=  16L;
	}
	else fields[x][y] &= ~16L;
	return true;
}

bool cCurTown::set_ice_wall(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on space, ther's no room for a field.
		if(is_force_wall(x,y) || is_blade_wall(x,y) || is_antimagic(x,y))
			return false;
		if(is_web(x,y) || is_crate(x,y) || is_barrel(x,y))
			return false;
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y))
			return false;
		fields[x][y]  |=  32L;
		set_fire_wall(x,y,false);
		set_scloud(x,y,false);
	}
	else fields[x][y] &= ~32L;
	return true;
}

bool cCurTown::set_blade_wall(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // if certain things are on space, there's no room for a field.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_antimagic(x,y))
			return false;
		fields[x][y]  |=  64L;
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
	}
	else fields[x][y] &= ~64L;
	return true;
}

bool cCurTown::set_sleep_cloud(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // if certain things are on space, there's no room for cloud.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_antimagic(x,y))
			return false;
		fields[x][y]  |=  128L;
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
	}
	else fields[x][y] &= ~128L;
	return true;
}

bool cCurTown::set_block(char x, char y, bool b){ // currently unused
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b) fields[x][y] |=  256L;
	else  fields[x][y] &= ~256L;
	return true;
}

bool cCurTown::set_spot(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	special_spot[x][y] = b;
	return true;
}

bool cCurTown::set_special(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b) fields[x][y] |=  512L;
	else  fields[x][y] &= ~512L;
	return true;
}

bool cCurTown::set_web(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on the space, there's no room for web.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y))
			return false;
		if(is_force_wall(x,y) || is_fire_wall(x,y) || is_antimagic(x, y))
			return false;
		if(is_ice_wall(x, y) || is_blade_wall(x,y) || is_sleep_cloud(x,y))
			return false;
		fields[x][y]  |=  1024L;
	}
	else fields[x][y] &= ~1024L;
	return true;
}

bool cCurTown::set_crate(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on the space, there's no room for a crate.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_barrel(x,y))
			return false;
		fields[x][y]  |=  2048L;
	}
	else fields[x][y] &= ~2048L;
	return true;
}

bool cCurTown::set_barrel(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on the space, there's no room for a crate.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_crate(x,y))
			return false;
		fields[x][y]  |=  4096L;
	}
	else fields[x][y] &= ~4096L;
	return true;
}

bool cCurTown::set_fire_barr(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on the space, there's no room for a barrier.
		if(is_barrel(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_crate(x,y))
			return false;
		if (is_antimagic(x,y) && get_ran(1,0,3) < 3)
			return false;
		fields[x][y]  |=  8192L;
		// Cancel out fields
		set_web(x,y,false);
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
		set_antimagic(x,y,false);
		set_scloud(x,y,false);
		set_ice_wall(x,y,false);
		set_blade_wall(x,y,false);
		set_sleep_cloud(x,y,false);
	}
	else fields[x][y] &= ~8192L;
	return true;
}

bool cCurTown::set_force_barr(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on the space, there's no room for a barrier.
		if(is_fire_barr(x,y) || is_barrel(x,y) || is_quickfire(x,y) || is_crate(x,y))
			return false;
		if (is_antimagic(x,y) && get_ran(1,0,2) < 2)
			return false;
		fields[x][y]  |=  16384L;
		// Cancel out fields
		set_web(x,y,false);
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
		set_antimagic(x,y,false);
		set_scloud(x,y,false);
		set_ice_wall(x,y,false);
		set_blade_wall(x,y,false);
		set_sleep_cloud(x,y,false);
	}
	else fields[x][y] &= ~16384L;
	return true;
}

bool cCurTown::set_quickfire(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){ // If certain things are on space, there's no room for quickfire.
		if (is_antimagic(x,y) && get_ran(1,0,1) == 0)
			return false;
		if (is_force_barr(x,y) || is_fire_barr(x,y))
			return false;
		fields[x][y]  |=  32768L;
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
		set_antimagic(x,y,false);
		set_scloud(x,y,false);
		set_ice_wall(x,y,false);
		set_blade_wall(x,y,false);
		set_sleep_cloud(x,y,false);
		set_web(x,y,false);
		set_crate(x,y,false);
		set_barrel(x,y,false);
		set_force_barr(x,y,false);
		set_fire_barr(x,y,false);
	}
	else fields[x][y] &= ~32768;
	return true;
}

bool cCurTown::set_sm_blood(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){
		if(is_med_blood(x,y) || is_lg_blood(x,y))
			return false;
		fields[x][y]  |=  65536L;
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
	}
	else fields[x][y] &= ~65536L;
	return true;
}

bool cCurTown::set_med_blood(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){
		if(is_lg_blood(x,y))
			return false;
		fields[x][y] |=  131072L;
		set_sm_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
	}
	else fields[x][y] &= ~131072L;
	return true;
}

bool cCurTown::set_lg_blood(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){
		fields[x][y] |=  262144L;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
	}
	else fields[x][y] &= ~262144L;
	return true;
}

bool cCurTown::set_sm_slime(char x, char y, bool b){
	if(b){
		if(is_lg_slime(x,y))
			return false;
		fields[x][y] |=  524288L;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_lg_blood(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
	}
	else fields[x][y] &= ~524288L;
	return true;
}

bool cCurTown::set_lg_slime(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){
		fields[x][y] |=  1048576L;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_lg_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
	}
	else fields[x][y] &= ~1048576L;
	return true;
}

bool cCurTown::set_ash(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){
		fields[x][y] |=  2097152L;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_lg_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
	}
	else fields[x][y] &= ~2097152L;
	return true;
}

bool cCurTown::set_bones(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){
		fields[x][y] |=  4194304L;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_lg_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_ash(x,y,false);
		set_rubble(x,y,false);
	}
	else fields[x][y] &= ~4194304L;
	return true;
}

bool cCurTown::set_rubble(char x, char y, bool b){
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b){
		fields[x][y] |=  8388608L;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_lg_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
	}
	else fields[x][y] &= ~8388608L;
	return true;
}

bool cCurTown::set_force_cage(char x, char y, bool b){
	// TODO: Consider whether placing a forcecage should erase anything already present, or fail due to something already present
	// TODO: Also consider checking for forcecage in some of the other placement functions.
	if(x > record->max_dim() || y > record->max_dim()) return false;
	if(b) fields[x][y] |=  16777216L;
	else  fields[x][y] &= ~16777216L;
	return true;
}

//bool cCurTown::set_trim(char x, char y, char t, bool b){
//	unsigned char bit = 1 << t;
//	if(b){
//		switch(t){
//			case 0:
//				set_trim(x,y,1,false);
//				break;
//			case 1:
//				if(!is_trim(x,y,3)) break;
//				set_trim(x,y,3,false);
//				set_trim(x,y,5,false);
//				break;
//			case 2:
//				set_trim(x,y,1,false);
//				set_trim(x,y,3,false);
//				break;
//			case 3:
//				if(!is_trim(x,y,1)) break;
//				set_trim(x,y,3,false);
//				set_trim(x,y,5,false);
//				break;
//			case 6:
//				set_trim(x,y,5,false);
//				break;
//		}
//		trim[x][y] |= bit;
//	}else trim[x][y] &= ~bit;
//}

unsigned char cCurTown::explored(char x,char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return 0;
	return fields[x][y] & 0x000000FF;
}

unsigned char cCurTown::misc_i(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return 0;
	return (fields[x][y] & 0x0000FF00) >> 8;
}

unsigned char cCurTown::sfx(char x, char y) const{
	if(x > record->max_dim() || y > record->max_dim()) return 0;
	return (fields[x][y] & 0x00FF0000) >> 16;
}

ter_num_t(& cCurOut::operator [] (size_t i))[96]{
	return out[i];
}

ter_num_t& cCurOut::operator [] (location loc) {
	return out[loc.x][loc.y];
}

void cCurOut::writeTo(std::ostream& file){
//	for(int i = 0; i < 96; i++){
//		file << expl[i][0];
//		for(int j = 1; j < 96; j++){
//			file << '\t' << expl[i][j];
//		}
//		file << std::endl;
//	}
//	file << '\f';
//	for(int i = 9; i < 96; i++){
//		file << out[i][0];
//		for(int j = 1; j < 96; j++){
//			file << '\t' << out[i][j];
//		}
//		file << std::endl;
//	}
//	file << '\f';
	for(int i = 0; i < 96; i++){
		file << out_e[i][0];
		for(int j = 1; j < 96; j++){
			file << '\t' << out_e[i][j];
		}
		file << std::endl;
	}
//	file << "OUTDOORS 0 0" << std::endl;
//	outdoors[0][0].writeTo(file);
//	file << "OUTDOORS 0 1" << std::endl;
//	outdoors[0][1].writeTo(file);
//	file << "OUTDOORS 1 0" << std::endl;
//	outdoors[1][0].writeTo(file);
//	file << "OUTDOORS 1 1" << std::endl;
//	outdoors[1][1].writeTo(file);
//	file << std::endl;
}

void cCurTown::writeTo(std::ostream& file){
	file << "TOWN " << num << std::endl;
	file << "DIFFICULTY " << difficulty << std::endl;
	if(hostile) file << "HOSTILE" << std::endl;
	file << "INBOAT " << in_boat << std::endl;
	file << "AT " << p_loc.x << ' ' << p_loc.y << std::endl;
	for(int i; i < 115; i++)
		if(items[i].variety > ITEM_TYPE_NO_ITEM){
			std::ostringstream sout;
			sout << "ITEM " << i << ' ';
			items[i].writeTo(file, sout.str());
		}
	file << '\f';
	for(int i = 0; i < 64; i++){
		file << fields[i][0];
		for(int j = 1; j < 64; j++)
			file << '\t' << fields[i][j];
		file << std::endl;
	}
	file << '\f' << record->max_dim() << std::endl;
	for(int i = 0; i < record->max_dim(); i++){
		file << record->terrain(i,0);
		for(int j = 1; j < record->max_dim(); j++)
			file << '\t' << record->terrain(i,j);
		file << std::endl;
	}
	file << '\f';
	// TODO: Write population
}

void cCurTown::readFrom(std::istream& file){
	std::istringstream bin, sin;
	std::string cur;
	getline(file, cur, '\f');
	bin.str(cur);
	while(bin){
		getline(bin, cur);
		sin.str(cur);
		sin >> cur;
		if(cur == "TOWN")
			sin >> num;
		else if(cur == "DIFFICULTY")
			sin >> difficulty;
		else if(cur == "HOSTILE")
			hostile = true;
		else if(cur == "INBOAT")
			sin >> in_boat;
		else if(cur == "AT")
			sin >> p_loc.x >> p_loc.y;
		else if(cur == "ITEM"){
			int i;
			sin >> i >> cur;
			items[i].readAttrFrom(cur, sin);
		}
	}
	getline(file, cur, '\f');
	bin.str(cur);
	for(int i = 0; i < 64; i++)
		for(int j = 1; j < 64; j++)
			bin >> fields[i][j];
	getline(file, cur, '\f');
	bin.str(cur);
	int dim;
	bin >> dim;
	if(record != NULL) delete record;
	switch(dim){
		case 32:
			record = new cTinyTown;
			break;
		case 48:
			record = new cMedTown;
			break;
		case 64:
			record = new cBigTown;
			break;
	}
	for(int i = 0; i < dim; i++)
		for(int j = 1; j < dim; j++)
			bin >> record->terrain(i,j);
	// TODO: Read population
}


void cCurOut::readFrom(std::istream& file){
	for(int i = 0; i < 96; i++)
		for(int j = 1; j < 96; j++)
			file >> out_e[i][j];
}

cCurTown::cCurTown(){
	record = NULL;
	num = 200;
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++)
			fields[i][j] = 0L;
}
extern cScenario scenario;
short cUniverse::difficulty_adjust() {
	short party_level = 0;
	short adj = 1;
	
	if(!scenario.adjust_diff) return 1;
	
	for (short i = 0; i < 6; i++)
		if (party[i].main_status == 1)
			party_level += party[i].level;
	
	if ((scenario.difficulty <= 0) && (party_level >= 60))
		adj++;
	if ((scenario.difficulty <= 1) && (party_level >= 130))
		adj++;
	if ((scenario.difficulty <= 2) && (party_level >= 210))
		adj++;
	
	return adj;
}

short cCurTown::countMonsters(){
	short to_ret = 0;
	for (short i = 0; i < record->max_monst(); i++)
		if (monst[i].active > 0)
			to_ret++;
	return to_ret;
}
