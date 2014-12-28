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
#include "fileio.h"

void cCurOut::append(legacy::out_info_type& old){
	for(int i = 0; i < 96; i++)
		for(int j = 0; j < 96; j++)
			expl[i][j] = old.expl[i][j];
}

void cCurTown::append(legacy::current_town_type& old){
	num = old.town_num;
	difficulty = old.difficulty;
	record()->append(old.town);
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++)
			fields[i][j] = old.explored[i][j];
	hostile = old.hostile;
	monst.append(old.monst);
	in_boat = old.in_boat;
	p_loc.x = old.p_loc.x;
	p_loc.y = old.p_loc.y;
	cur_talk_loaded = num;
}

void cCurTown::append(legacy::big_tr_type& old){
	int i,j;
	for(i = 0; i < record()->max_dim(); i++)
		for(j = 0; j < record()->max_dim(); j++)
			record()->terrain(i,j) = old.terrain[i][j];
	for(i = 0; i < 16; i++){
		record()->room_rect(i).top = old.room_rect[i].top;
		record()->room_rect(i).left = old.room_rect[i].left;
		record()->room_rect(i).bottom = old.room_rect[i].bottom;
		record()->room_rect(i).right = old.room_rect[i].right;
	}
	for(i = 0; i < record()->max_monst(); i++)
		record()->creatures(i).append(old.creatures[i]);
	for(i = 0; i < record()->max_dim() / 8; i++)
		for(j = 0; j < record()->max_dim(); j++)
			record()->lighting(i,j) = old.lighting[i][j];
}

void cCurTown::append(legacy::town_item_list& old){
	for(int i = 0; i < 115; i++)
		items[i].append(old.items[i]);
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
			fields[i][j] = tmp_sfx;
			fields[i][j] = tmp_misc_i;
		}
}

cTown* cCurTown::operator -> (){
	return record();
}

void cCurTown::place_preset_fields() {
	// Initialize barriers, etc. Note non-sfx gets forgotten if this is a town recently visited.
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++) {
			fields[i][j] = 0;
		}
	for(size_t i = 0; i < record()->preset_fields.size(); i++) {
		switch(record()->preset_fields[i].type){
			case OBJECT_BLOCK:
				univ.town.set_block(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SPECIAL_SPOT:
				set_spot(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case FIELD_WEB:
				set_web(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case OBJECT_CRATE:
				set_crate(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case OBJECT_BARREL:
				set_barrel(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case BARRIER_FIRE:
				set_fire_barr(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case BARRIER_FORCE:
				set_force_barr(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case BARRIER_CAGE:
				set_force_cage(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case FIELD_QUICKFIRE:
				set_quickfire(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SFX_SMALL_BLOOD:
				set_sm_blood(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SFX_MEDIUM_BLOOD:
				set_med_blood(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SFX_LARGE_BLOOD:
				set_lg_blood(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SFX_SMALL_SLIME:
				set_sm_slime(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SFX_LARGE_SLIME:
				set_lg_slime(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SFX_ASH:
				set_ash(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SFX_BONES:
				set_bones(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SFX_RUBBLE:
				set_rubble(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
		}
	}
}

cSpeech& cCurTown::cur_talk() {
	// Make sure we actually have a valid speech stored
	return univ.scenario.towns[cur_talk_loaded]->talking;
}

bool cCurTown::prep_talk(short which) {
	if(which == cur_talk_loaded) return true;
	cur_talk_loaded = which;
	return true;
}

void cCurTown::prep_arena() {
	if(arena != nullptr) delete arena;
	arena = new cMedTown(univ.scenario, false);
}

cCurTown::~cCurTown() {
	if(arena != nullptr) delete arena;
}

cTown*const cCurTown::record() const {
	if(num == 200) return arena;
	return univ.scenario.towns[num];
}

bool cCurTown::is_explored(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & SPECIAL_EXPLORED;
}

bool cCurTown::is_force_wall(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & WALL_FORCE;
}

bool cCurTown::is_fire_wall(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & WALL_FIRE;
}

bool cCurTown::is_antimagic(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & FIELD_ANTIMAGIC;
}

bool cCurTown::is_scloud(char x, char y) const{ // stinking cloud
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & CLOUD_STINK;
}

bool cCurTown::is_ice_wall(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & WALL_ICE;
}

bool cCurTown::is_blade_wall(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & WALL_BLADES;
}

bool cCurTown::is_sleep_cloud(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & CLOUD_SLEEP;
}

bool cCurTown::is_block(char x, char y) const{ // currently unused
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & OBJECT_BLOCK;
}

bool cCurTown::is_spot(char x, char y) const{
	return fields[x][y] & SPECIAL_SPOT;
}

bool cCurTown::is_special(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	for(int i = 0; i < 50; i++)
		if(x == record()->special_locs[i].x && y == record()->special_locs[i].y)
			return true;
	return false;
}

bool cCurTown::is_web(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & FIELD_WEB;
}

bool cCurTown::is_crate(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & OBJECT_CRATE;
}

bool cCurTown::is_barrel(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & OBJECT_BARREL;
}

bool cCurTown::is_fire_barr(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & BARRIER_FIRE;
}

bool cCurTown::is_force_barr(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & BARRIER_FORCE;
}

bool cCurTown::is_quickfire(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & FIELD_QUICKFIRE;
}

bool cCurTown::is_sm_blood(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & SFX_SMALL_BLOOD;
}

bool cCurTown::is_med_blood(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & SFX_MEDIUM_BLOOD;
}

bool cCurTown::is_lg_blood(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & SFX_LARGE_BLOOD;
}

bool cCurTown::is_sm_slime(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & SFX_SMALL_SLIME;
}

bool cCurTown::is_lg_slime(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & SFX_LARGE_SLIME;
}

bool cCurTown::is_ash(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & SFX_ASH;
}

bool cCurTown::is_bones(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & SFX_BONES;
}

bool cCurTown::is_rubble(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & SFX_RUBBLE;
}

bool cCurTown::is_force_cage(char x, char y) const{
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	return fields[x][y] & BARRIER_CAGE;
}

bool cCurTown::set_explored(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b) fields[x][y] |=  SPECIAL_EXPLORED;
	else  fields[x][y] &= ~SPECIAL_EXPLORED;
	return true;
}

bool cCurTown::set_force_wall(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on space, there's no room for field.
		if(is_impassable(x,y))
			return false;
		if(is_antimagic(x,y) || is_blade_wall(x,y) || is_quickfire(x,y))
			return false;
		if(is_crate(x,y) || is_barrel(x,y) || is_fire_barr(x,y) || is_force_barr(x,y))
			return false;
		set_web(x,y,false);
		set_fire_wall(x,y,false);
		fields[x][y]  |=  WALL_FORCE;
	}
	else fields[x][y] &= ~WALL_FORCE;
	return true;
}

bool cCurTown::set_fire_wall(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on space, there's no room for field.
		if(is_impassable(x,y))
			return false;
		if(is_antimagic(x,y) || is_blade_wall(x,y) || is_quickfire(x,y) || is_ice_wall(x,y))
			return false;
		if(is_crate(x,y) || is_barrel(x,y) || is_fire_barr(x,y) || is_force_barr(x,y))
			return false;
		if(is_web(x,y) || is_scloud(x,y) || is_sleep_cloud(x,y))
			return false;
		set_web(x,y,false);
		set_fire_wall(x,y,false);
		fields[x][y]  |=  WALL_FIRE;
	}
	else fields[x][y] &= ~WALL_FIRE;
	return true;
}

bool cCurTown::set_antimagic(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on space, there's no room for a field.
		if(is_impassable(x,y))
			return false;
		if(is_quickfire(x,y) || is_force_wall(x,y) || is_fire_wall(x,y))
			return false;
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
		set_antimagic(x,y,false);
		set_scloud(x,y,false);
		set_ice_wall(x,y,false);
		set_blade_wall(x,y,false);
		set_sleep_cloud(x,y,false);
		fields[x][y]  |=  FIELD_ANTIMAGIC;
	}
	else fields[x][y] &= ~FIELD_ANTIMAGIC;
	return true;
}

bool cCurTown::set_scloud(char x, char y, bool b){ // stinking cloud
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on space, there's no room for cloud.
		if(is_impassable(x,y))
			return false;
		if(is_force_wall(x,y) || is_fire_wall(x,y) || is_ice_wall(x,y) || is_blade_wall(x,y))
			return false;
		if(is_antimagic(x,y) || is_sleep_cloud(x,y) || is_quickfire(x,y))
			return false;
		if(is_fire_barr(x,y) || is_force_barr(x,y))
			return false;
		fields[x][y]  |=  CLOUD_STINK;
	}
	else fields[x][y] &= ~CLOUD_STINK;
	return true;
}

bool cCurTown::set_ice_wall(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on space, ther's no room for a field.
		if(is_impassable(x,y))
			return false;
		if(is_force_wall(x,y) || is_blade_wall(x,y) || is_antimagic(x,y))
			return false;
		if(is_web(x,y) || is_crate(x,y) || is_barrel(x,y))
			return false;
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y))
			return false;
		set_fire_wall(x,y,false);
		set_scloud(x,y,false);
		fields[x][y]  |=  WALL_ICE;
	}
	else fields[x][y] &= ~WALL_ICE;
	return true;
}

bool cCurTown::set_blade_wall(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // if certain things are on space, there's no room for a field.
		if(is_impassable(x,y))
			return false;
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_antimagic(x,y))
			return false;
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
		fields[x][y]  |=  WALL_BLADES;
	}
	else fields[x][y] &= ~WALL_BLADES;
	return true;
}

bool cCurTown::set_sleep_cloud(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // if certain things are on space, there's no room for cloud.
		if(is_impassable(x,y))
			return false;
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_antimagic(x,y))
			return false;
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
		fields[x][y]  |=  CLOUD_SLEEP;
	}
	else fields[x][y] &= ~CLOUD_SLEEP;
	return true;
}

bool cCurTown::set_block(char x, char y, bool b){ // currently unused
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b) fields[x][y] |=  OBJECT_BLOCK;
	else  fields[x][y] &= ~OBJECT_BLOCK;
	return true;
}

bool cCurTown::set_spot(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b) fields[x][y] |=  SPECIAL_SPOT;
	else  fields[x][y] &= ~SPECIAL_SPOT;
	return true;
}

bool cCurTown::set_web(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on the space, there's no room for webs
		if(is_impassable(x,y))
			return false;
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y))
			return false;
		if(is_force_wall(x,y) || is_fire_wall(x,y) || is_antimagic(x, y))
			return false;
		if(is_ice_wall(x, y) || is_blade_wall(x,y) || is_sleep_cloud(x,y))
			return false;
		fields[x][y]  |=  FIELD_WEB;
	}
	else fields[x][y] &= ~FIELD_WEB;
	return true;
}

bool cCurTown::set_crate(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on the space, there's no room for a crate.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_barrel(x,y))
			return false;
		fields[x][y]  |=  OBJECT_CRATE;
	}
	else fields[x][y] &= ~OBJECT_CRATE;
	return true;
}

bool cCurTown::set_barrel(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on the space, there's no room for a crate.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_crate(x,y))
			return false;
		fields[x][y]  |=  OBJECT_BARREL;
	}
	else fields[x][y] &= ~OBJECT_BARREL;
	return true;
}

bool cCurTown::set_fire_barr(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on the space, there's no room for a barrier.
		if(is_barrel(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_crate(x,y))
			return false;
		if(is_antimagic(x,y) && get_ran(1,0,3) < 3)
			return false;
		// Cancel out fields
		set_web(x,y,false);
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
		set_antimagic(x,y,false);
		set_scloud(x,y,false);
		set_ice_wall(x,y,false);
		set_blade_wall(x,y,false);
		set_sleep_cloud(x,y,false);
		fields[x][y]  |=  BARRIER_FIRE;
	}
	else fields[x][y] &= ~BARRIER_FIRE;
	return true;
}

bool cCurTown::set_force_barr(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on the space, there's no room for a barrier.
		if(is_fire_barr(x,y) || is_barrel(x,y) || is_quickfire(x,y) || is_crate(x,y))
			return false;
		if(is_antimagic(x,y) && get_ran(1,0,2) < 2)
			return false;
		// Cancel out fields
		set_web(x,y,false);
		set_force_wall(x,y,false);
		set_fire_wall(x,y,false);
		set_antimagic(x,y,false);
		set_scloud(x,y,false);
		set_ice_wall(x,y,false);
		set_blade_wall(x,y,false);
		set_sleep_cloud(x,y,false);
		fields[x][y]  |=  BARRIER_FORCE;
	}
	else fields[x][y] &= ~BARRIER_FORCE;
	return true;
}

bool cCurTown::set_quickfire(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){ // If certain things are on space, there's no room for quickfire.
		ter_num_t ter = record()->terrain(x,y);
		if(univ.scenario.ter_types[ter].blockage == eTerObstruct::BLOCK_SIGHT)
			return false;
		// TODO: Isn't it a little odd that BLOCK_MOVE_AND_SHOOT isn't included here?
		if(univ.scenario.ter_types[ter].blockage == eTerObstruct::BLOCK_MOVE_AND_SIGHT)
			return false;
		if(is_antimagic(x,y) && get_ran(1,0,1) == 0)
			return false;
		if(is_force_barr(x,y) || is_fire_barr(x,y))
			return false;
		quickfire_present = true;
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
		fields[x][y]  |=  FIELD_QUICKFIRE;
	}
	else fields[x][y] &= ~FIELD_QUICKFIRE;
	return true;
}

bool cCurTown::free_for_sfx(short x, short y) {
	ter_num_t ter;
	ter = record()->terrain(x,y);
	if(univ.scenario.ter_types[ter].blockage != eTerObstruct::CLEAR)
		return false;
	return true;
}

bool cCurTown::set_sm_blood(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){
		if(!free_for_sfx(x,y)) return false;
		if(is_med_blood(x,y) || is_lg_blood(x,y))
			return false;
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
		fields[x][y]  |=  SFX_SMALL_BLOOD;
	}
	else fields[x][y] &= ~SFX_SMALL_BLOOD;
	return true;
}

bool cCurTown::set_med_blood(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){
		if(!free_for_sfx(x,y)) return false;
		if(is_lg_blood(x,y))
			return false;
		set_sm_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
		fields[x][y]  |=  SFX_MEDIUM_BLOOD;
	}
	else fields[x][y] &= ~SFX_MEDIUM_BLOOD;
	return true;
}

bool cCurTown::set_lg_blood(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){
		if(!free_for_sfx(x,y)) return false;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
		fields[x][y]  |=  SFX_LARGE_BLOOD;
	}
	else fields[x][y] &= ~SFX_LARGE_BLOOD;
	return true;
}

bool cCurTown::set_sm_slime(char x, char y, bool b){
	if(b){
		if(!free_for_sfx(x,y)) return false;
		if(is_lg_slime(x,y))
			return false;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_lg_blood(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
		fields[x][y]  |=  SFX_SMALL_SLIME;
	}
	else fields[x][y] &= ~SFX_SMALL_SLIME;
	return true;
}

bool cCurTown::set_lg_slime(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){
		if(!free_for_sfx(x,y)) return false;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_lg_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
		fields[x][y]  |=  SFX_LARGE_SLIME;
	}
	else fields[x][y] &= ~SFX_LARGE_SLIME;
	return true;
}

bool cCurTown::set_ash(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){
		if(!free_for_sfx(x,y)) return false;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_lg_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_bones(x,y,false);
		set_rubble(x,y,false);
		fields[x][y]  |=  SFX_ASH;
	}
	else fields[x][y] &= ~SFX_ASH;
	return true;
}

bool cCurTown::set_bones(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){
		if(!free_for_sfx(x,y)) return false;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_lg_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_ash(x,y,false);
		set_rubble(x,y,false);
		fields[x][y]  |=  SFX_BONES;
	}
	else fields[x][y] &= ~SFX_BONES;
	return true;
}

bool cCurTown::set_rubble(char x, char y, bool b){
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b){
		if(!free_for_sfx(x,y)) return false;
		set_sm_blood(x,y,false);
		set_med_blood(x,y,false);
		set_lg_blood(x,y,false);
		set_sm_slime(x,y,false);
		set_lg_slime(x,y,false);
		set_ash(x,y,false);
		set_bones(x,y,false);
		fields[x][y]  |=  SFX_RUBBLE;
	}
	else fields[x][y] &= ~SFX_RUBBLE;
	return true;
}

bool cCurTown::set_force_cage(char x, char y, bool b){
	// TODO: Consider whether placing a forcecage should erase anything already present, or fail due to something already present
	// TODO: Also consider checking for forcecage in some of the other placement functions.
	if(x > record()->max_dim() || y > record()->max_dim()) return false;
	if(b) fields[x][y] |=  BARRIER_CAGE;
	else  fields[x][y] &= ~BARRIER_CAGE;
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

// TODO: This seems to be wrong; impassable implies "blocks movement", which two other blockages also do
bool cCurTown::is_impassable(short i,short  j) {
	ter_num_t ter;
	
	ter = record()->terrain(i,j);
	if(univ.scenario.ter_types[ter].blockage == eTerObstruct::BLOCK_MOVE_AND_SIGHT)
		return true;
	else return false;
}

ter_num_t(& cCurOut::operator [] (size_t i))[96]{
	return out[i];
}

ter_num_t& cCurOut::operator [] (location loc) {
	return out[loc.x][loc.y];
}

void cCurOut::writeTo(std::ostream& file) const {
	writeArray(file, expl, 96, 96);
	writeArray(file, out, 96, 96);
	writeArray(file, out_e, 96, 96);
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

void cCurOut::readFrom(std::istream& file) {
	readArray(file, expl, 96, 96);
	readArray(file, out, 96, 96);
	readArray(file, out_e, 96, 96);
}

void cCurTown::writeTo(std::ostream& file) const {
	file << "TOWN " << num << '\n';
	file << "DIFFICULTY " << difficulty << '\n';
	if(hostile) file << "HOSTILE" << '\n';
	file << "INBOAT " << in_boat << '\n';
	file << "AT " << p_loc.x << ' ' << p_loc.y << '\n';
	file << '\f';
	for(int i = 0; i < 115; i++)
		if(items[i].variety != eItemType::NO_ITEM){
			file << "ITEM " << i << '\n';
			items[i].writeTo(file);
			file << '\f';
		}
	file << '\f';
	for(int i = 0; i < 60; i++) {
		if(monst[i].active > 0) {
			file << "CREATURE " << i << '\n';
			monst[i].writeTo(file);
			file << '\f';
		}
	}
	file << '\f';
	file << "FIELDS\n";
	writeArray(file, fields, 64, 46);
	file << "SIZE " << record()->max_dim() << "\n";
	record()->writeTerrainTo(file);
	// TODO: Do we need to save special_spot?
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
		sin.clear();
	}
	bin.clear();
	while(file) {
		getline(file, cur, '\f');
		bin.str(cur);
		bin >> cur;
		if(cur == "FIELDS")
			readArray(bin, fields, 64, 64);
		else if(cur == "ITEM") {
			int i;
			bin >> i;
			items[i].readFrom(bin);
		} else if(cur == "CREATURE") {
			int i;
			bin >> i;
			monst[i].active = true;
			monst[i].readFrom(bin);
		} else if(cur == "SIZE") {
			int dim;
			bin >> dim;
			univ.scenario.towns[num]->readTerrainFrom(bin);
		}
		bin.clear();
	}
}

cCurTown::cCurTown(cUniverse& univ) : univ(univ) {
	arena = nullptr;
	num = 200;
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++)
			fields[i][j] = 0L;
}

cCurOut::cCurOut(cUniverse& univ) : univ(univ) {}

cOutdoors* cCurOut::operator->() {
	short x = univ.party.outdoor_corner.x + univ.party.i_w_c.x, y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
	return univ.scenario.outdoors[x][y];
}

cUniverse::cUniverse(long party_type) : party(*this, party_type), out(*this), town(*this) {}

short cUniverse::difficulty_adjust() const {
	short party_level = 0;
	short adj = 1;
	
	if(!scenario.adjust_diff) return 1;
	
	for(short i = 0; i < 6; i++)
		if(party[i].main_status == eMainStatus::ALIVE)
			party_level += party[i].level;
	
	if((scenario.difficulty <= 0) && (party_level >= 60))
		adj++;
	if((scenario.difficulty <= 1) && (party_level >= 130))
		adj++;
	if((scenario.difficulty <= 2) && (party_level >= 210))
		adj++;
	
	return adj;
}

short cCurTown::countMonsters(){
	short to_ret = 0;
	for(short i = 0; i < record()->max_monst(); i++)
		if(monst[i].active > 0)
			to_ret++;
	return to_ret;
}
