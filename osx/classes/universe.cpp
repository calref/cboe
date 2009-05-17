/*
 *  universe.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "classes.h"
#include "oldstructs.h"
#include "mathutil.h"

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
	cur_talk = &record->talking;
	cur_talk_loaded = num;
}

__attribute__((deprecated))
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

__attribute__((deprecated))
void cCurTown::append(legacy::town_item_list& old){
	for(int i = 0; i < 115; i++)
		items[i] = old.items[i];
}

__attribute__((deprecated))
void cUniverse::append(legacy::stored_town_maps_type& old){
	for(int n = 0; n < 200; n++)
		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 64; j++)
				town_maps[n][i][j] = old.town_maps[n][i][j];
}

__attribute__((deprecated))
void cUniverse::append(legacy::stored_outdoor_maps_type& old){
	for(int n = 0; n < 100; n++)
		for(int i = 0; i < 6; i++)
			for(int j = 0; j < 48; j++)
				out_maps[n][i][j] = old.outdoor_maps[n][i][j];
}

__attribute__((deprecated))
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

unsigned short(& cCurOut::operator [] (size_t i))[96]{
	return out[i];
}

void cCurOut::writeTo(ostream& file){
//	for(int i = 0; i < 96; i++){
//		file << expl[i][0];
//		for(int j = 1; j < 96; j++){
//			file << '\t' << expl[i][j];
//		}
//		file << endl;
//	}
//	file << '\f';
//	for(int i = 9; i < 96; i++){
//		file << out[i][0];
//		for(int j = 1; j < 96; j++){
//			file << '\t' << out[i][j];
//		}
//		file << endl;
//	}
//	file << '\f';
	for(int i = 0; i < 96; i++){
		file << out_e[i][0];
		for(int j = 1; j < 96; j++){
			file << '\t' << out_e[i][j];
		}
		file << endl;
	}
//	file << "OUTDOORS 0 0" << endl;
//	outdoors[0][0].writeTo(file);
//	file << "OUTDOORS 0 1" << endl;
//	outdoors[0][1].writeTo(file);
//	file << "OUTDOORS 1 0" << endl;
//	outdoors[1][0].writeTo(file);
//	file << "OUTDOORS 1 1" << endl;
//	outdoors[1][1].writeTo(file);
//	file << endl;
}

void cCurTown::writeTo(ostream& file){
	file << "TOWN " << num << endl;
	file << "DIFFICULTY " << difficulty << endl;
	if(hostile) file << "HOSTILE" << endl;
	file << "INBOAT " << in_boat << endl;
	file << "AT " << p_loc.x << ' ' << p_loc.y << endl;
	for(int i; i < 115; i++)
		if(items[i].variety > ITEM_TYPE_NO_ITEM){
			ostringstream sout;
			sout << "ITEM " << i << ' ';
			items[i].writeTo(file, sout.str());
		}
	file << '\f';
	for(int i = 0; i < 64; i++){
		file << fields[i][0];
		for(int j = 1; j < 64; j++)
			file << '\t' << fields[i][j];
		file << endl;
	}
	file << '\f' << record->max_dim() << endl;
	for(int i = 0; i < record->max_dim(); i++){
		file << record->terrain(i,0);
		for(int j = 1; j < record->max_dim(); j++)
			file << '\t' << record->terrain(i,j);
		file << endl;
	}
	file << '\f';
	// TODO: Write population
}

void cCurTown::readFrom(istream& file){
	istringstream bin, sin;
	string cur;
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


void cCurOut::readFrom(istream& file){
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
