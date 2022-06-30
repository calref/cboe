/*
 *  universe.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "universe.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stack>

#include "town.hpp"
#include "oldstructs.hpp"
#include "mathutil.hpp"
#include "fileio.hpp"
#include "gfxsheets.hpp"

void cCurOut::import_legacy(legacy::out_info_type& old){
	for(int i = 0; i < 96; i++)
		for(int j = 0; j < 96; j++)
			out_e[i][j] = old.expl[i][j];
}

void cCurTown::import_legacy(legacy::current_town_type& old){
	univ.party.town_num = old.town_num;
	difficulty = old.difficulty;
	record()->import_legacy(old.town);
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++)
			fields[i][j] = old.explored[i][j];
	monst.import_legacy(old.monst);
	univ.party.town_loc.x = old.p_loc.x;
	univ.party.town_loc.y = old.p_loc.y;
	cur_talk_loaded = old.town_num;
}

void cCurTown::import_legacy(legacy::big_tr_type& old){
	for(short i = 0; i < record()->max_dim; i++)
		for(short j = 0; j < record()->max_dim; j++)
			record()->terrain(i,j) = old.terrain[i][j];
	record()->area_desc.resize(16);
	for(short i = 0; i < 16; i++){
		record()->area_desc[i].top = old.room_rect[i].top;
		record()->area_desc[i].left = old.room_rect[i].left;
		record()->area_desc[i].bottom = old.room_rect[i].bottom;
		record()->area_desc[i].right = old.room_rect[i].right;
	}
	record()->creatures.resize(60);
	for(short i = 0; i < 60; i++)
		record()->creatures[i].import_legacy(old.creatures[i]);
	for(short i = 0; i < record()->max_dim; i++)
		for(short j = 0; j < record()->max_dim; j++) {
			record()->lighting[i][j] = old.lighting[i / 8][j] & (1 << (i % 8));
		}
}

void cCurTown::import_legacy(legacy::town_item_list& old){
	items.resize(115);
	for(int i = 0; i < 115; i++)
		items[i].import_legacy(old.items[i]);
}

void cUniverse::import_legacy(legacy::stored_town_maps_type& old){
	for(int n = 0; n < scenario.towns.size(); n++)
		for(size_t j = 0; j < scenario.towns[n]->maps.size(); j++)
			for(int i = 0; i < scenario.towns[n]->maps[j].size(); i++)
				scenario.towns[n]->maps[j][i] = old.town_maps[n][i / 8][j] & (1 << (i % 8));
}

static short onm(char x_sector,char y_sector, char w) {
	return y_sector * w + x_sector;
}

void cUniverse::import_legacy(legacy::stored_outdoor_maps_type& old){
	for(int x = 0; x < scenario.outdoors.width(); x++)
		for(int y = 0; y < scenario.outdoors.height(); y++)
			for(int i = 0; i < 48; i++)
				for(int j = 0; j < 48; j++)
					scenario.outdoors[x][y]->maps[i][j] = old.outdoor_maps[onm(x,y,scenario.outdoors.width())][i / 8][j] & (1 << i % 8);
}

void cCurTown::import_reset_fields_legacy(){
	// boe does not use the stored sfx and misc_i
	// but discard them and recompute their values
	for (auto const &f : record()->preset_fields) {
		if (f.loc.x<0 || f.loc.x>=64 || f.loc.y<0 || f.loc.y>=64) continue;
		// only 0<old_type<9 and 14<=old_type<=21 are retrieved here
		if ((f.type>8 && f.type<17) || (f.type>=30 & f.type<=37))
			fields[f.loc.x][f.loc.y]|=f.type;
	}
	auto const &terrain=record()->terrain;
	auto const &maxTerrain=univ.scenario.ter_types.max_size();
	for (auto const &spec : record()->special_locs) {
		if (spec.spec<0 || spec.x<0 || spec.x>=terrain.width() ||
			spec.y<0 || spec.y>=terrain.height()) continue;
		ter_num_t id=terrain[spec.x][spec.y];
		if (id>=0 && id<maxTerrain && univ.scenario.ter_types[id].i==3000)
			fields[spec.x][spec.y]|=SPECIAL_SPOT;
	}
}

cTown* cCurTown::operator -> (){
	return record();
}

cTown& cCurTown::operator * (){
	return *record();
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
				set_block(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SPECIAL_SPOT:
				set_spot(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
				break;
			case SPECIAL_ROAD:
				set_road(record()->preset_fields[i].loc.x,record()->preset_fields[i].loc.y,true);
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
				// The rest can't be preset, but enumerate them in case a new field is added.
			case FIELD_DISPEL: case FIELD_SMASH: case SPECIAL_EXPLORED: case CLOUD_SLEEP: case CLOUD_STINK:
			case FIELD_ANTIMAGIC: case WALL_BLADES: case WALL_FIRE: case WALL_FORCE: case WALL_ICE:
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
	arena = new cTown(univ.scenario, AREA_MEDIUM);
}

cCurTown::~cCurTown() {
	if(arena != nullptr) delete arena;
}

cTown*const cCurTown::record() const {
	if(univ.party.town_num == 200) return arena;
	return univ.scenario.towns[univ.party.town_num];
}

bool cCurTown::is_explored(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & SPECIAL_EXPLORED;
}

bool cCurTown::is_force_wall(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & WALL_FORCE;
}

bool cCurTown::is_fire_wall(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & WALL_FIRE;
}

bool cCurTown::is_antimagic(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & FIELD_ANTIMAGIC;
}

bool cCurTown::is_scloud(short x, short y) const{ // stinking cloud
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & CLOUD_STINK;
}

bool cCurTown::is_ice_wall(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & WALL_ICE;
}

bool cCurTown::is_blade_wall(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & WALL_BLADES;
}

bool cCurTown::is_sleep_cloud(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & CLOUD_SLEEP;
}

bool cCurTown::is_block(short x, short y) const{ // currently unused
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & OBJECT_BLOCK;
}

bool cCurTown::is_spot(short x, short y) const{
	return fields[x][y] & SPECIAL_SPOT;
}

bool cCurTown::is_road(short x, short y) const{
	return fields[x][y] & SPECIAL_ROAD;
}

bool cCurTown::is_special(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	location check(x,y);
	for(int i = 0; i < record()->special_locs.size(); i++)
		if(check == record()->special_locs[i] && record()->special_locs[i].spec >= 0)
			return true;
	return false;
}

bool cCurTown::is_web(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & FIELD_WEB;
}

bool cCurTown::is_crate(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & OBJECT_CRATE;
}

bool cCurTown::is_barrel(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & OBJECT_BARREL;
}

bool cCurTown::is_fire_barr(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & BARRIER_FIRE;
}

bool cCurTown::is_force_barr(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & BARRIER_FORCE;
}

bool cCurTown::is_quickfire(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & FIELD_QUICKFIRE;
}

bool cCurTown::is_sm_blood(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & SFX_SMALL_BLOOD;
}

bool cCurTown::is_med_blood(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & SFX_MEDIUM_BLOOD;
}

bool cCurTown::is_lg_blood(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & SFX_LARGE_BLOOD;
}

bool cCurTown::is_sm_slime(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & SFX_SMALL_SLIME;
}

bool cCurTown::is_lg_slime(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & SFX_LARGE_SLIME;
}

bool cCurTown::is_ash(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & SFX_ASH;
}

bool cCurTown::is_bones(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & SFX_BONES;
}

bool cCurTown::is_rubble(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & SFX_RUBBLE;
}

bool cCurTown::is_force_cage(short x, short y) const{
	if(x > record()->max_dim || y > record()->max_dim) return false;
	return fields[x][y] & BARRIER_CAGE;
}

bool cCurTown::set_explored(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
	if(b) fields[x][y] |=  SPECIAL_EXPLORED;
	else  fields[x][y] &= ~SPECIAL_EXPLORED;
	return true;
}

bool cCurTown::set_force_wall(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_fire_wall(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_antimagic(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_scloud(short x, short y, bool b){ // stinking cloud
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_ice_wall(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_blade_wall(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_sleep_cloud(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_block(short x, short y, bool b){ // currently unused
	if(x > record()->max_dim || y > record()->max_dim) return false;
	if(b) fields[x][y] |=  OBJECT_BLOCK;
	else  fields[x][y] &= ~OBJECT_BLOCK;
	return true;
}

bool cCurTown::set_spot(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
	if(b) fields[x][y] |=  SPECIAL_SPOT;
	else  fields[x][y] &= ~SPECIAL_SPOT;
	return true;
}

bool cCurTown::set_road(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
	if(b) fields[x][y] |=  SPECIAL_ROAD;
	else  fields[x][y] &= ~SPECIAL_ROAD;
	return true;
}

bool cCurTown::set_web(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_crate(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
	if(b){ // If certain things are on the space, there's no room for a crate.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_barrel(x,y))
			return false;
		fields[x][y]  |=  OBJECT_CRATE;
	}
	else fields[x][y] &= ~OBJECT_CRATE;
	return true;
}

bool cCurTown::set_barrel(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
	if(b){ // If certain things are on the space, there's no room for a crate.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_crate(x,y))
			return false;
		fields[x][y]  |=  OBJECT_BARREL;
	}
	else fields[x][y] &= ~OBJECT_BARREL;
	return true;
}

bool cCurTown::set_fire_barr(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_force_barr(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_quickfire(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_sm_blood(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_med_blood(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_lg_blood(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_sm_slime(short x, short y, bool b){
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

bool cCurTown::set_lg_slime(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_ash(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_bones(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_rubble(short x, short y, bool b){
	if(x > record()->max_dim || y > record()->max_dim) return false;
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

bool cCurTown::set_force_cage(short x, short y, bool b){
	// TODO: Consider whether placing a forcecage should erase anything already present, or fail due to something already present
	// TODO: Also consider checking for forcecage in some of the other placement functions.
	if(x > record()->max_dim || y > record()->max_dim) return false;
	if(b) fields[x][y] |=  BARRIER_CAGE;
	else  fields[x][y] &= ~BARRIER_CAGE;
	return true;
}

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
	readArray(file, out, 96, 96);
	readArray(file, out_e, 96, 96);
}

void cCurTown::writeTo(std::ostream& file) const {
	file << "TOWN " << univ.party.town_num << '\n';
	file << "DIFFICULTY " << difficulty << '\n';
	if(monst.hostile) file << "HOSTILE" << '\n';
	file << "AT " << univ.party.town_loc.x << ' ' << univ.party.town_loc.y << '\n';
	file << '\f';
	for(size_t i = 0; i < items.size(); i++)
		if(items[i].variety != eItemType::NO_ITEM){
			file << "ITEM " << i << '\n';
			items[i].writeTo(file);
			file << '\f';
		}
	file << '\f';
	for(int i = 0; i < monst.size(); i++) {
		if(monst[i].active > 0) {
			file << "CREATURE " << i << '\n';
			monst[i].writeTo(file);
			file << '\f';
		}
	}
	file << '\f';
	file << "FIELDS\n";
	file << std::hex;
	writeArray(file, fields, record()->max_dim, record()->max_dim);
	file << std::dec;
	file << "TERRAIN\n";
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
		if(cur == "TOWN") {
			sin >> univ.party.town_num;
			monst.which_town=univ.party.town_num; // OSNOLA, checkme: to fix the creature save
		}
		else if(cur == "DIFFICULTY")
			sin >> difficulty;
		else if(cur == "HOSTILE")
			monst.hostile = true;
		else if(cur == "AT")
			sin >> univ.party.town_loc.x >> univ.party.town_loc.y;
		sin.clear();
	}
	bin.clear();
	while(file) {
		getline(file, cur, '\f');
		bin.str(cur);
		bin >> cur;
		if(cur == "FIELDS") {
			int num = univ.party.town_num;
			bin >> std::hex;
			readArray(bin, fields, univ.scenario.towns[num]->max_dim, univ.scenario.towns[num]->max_dim);
			bin >> std::dec;
		} else if(cur == "ITEM") {
			int i;
			bin >> i;
			if(i >= items.size())
				items.resize(i + 1);
			items[i].readFrom(bin);
		} else if(cur == "CREATURE") {
			int i;
			bin >> i;
			monst.readFrom(bin, i);
			monst[i].active = true;
		} else if(cur == "TERRAIN")
			univ.scenario.towns[univ.party.town_num]->readTerrainFrom(bin);
		bin.clear();
	}
}

cCurTown::cCurTown(cUniverse& univ) : univ(univ) {
	arena = nullptr;
	univ.party.town_num = 200;
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++)
			fields[i][j] = 0L;
}

cCurOut::cCurOut(cUniverse& univ) : univ(univ) {}

cOutdoors* cCurOut::operator->() {
	short x = univ.party.outdoor_corner.x + univ.party.i_w_c.x, y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
	return univ.scenario.outdoors[x][y];
}

bool cCurOut::is_spot(int x, int y) {
	int sector_x = 0, sector_y = 0;
	if(x >= 48) sector_x++, x -= 48;
	if(y >= 48) sector_y++, y -= 48;
	sector_x += univ.party.outdoor_corner.x;
	sector_y += univ.party.outdoor_corner.y;
	return univ.scenario.outdoors[sector_x][sector_y]->special_spot[x][y];
}

bool cCurOut::is_road(int x, int y) {
	int sector_x = 0, sector_y = 0;
	if(x >= 48) sector_x++, x -= 48;
	if(y >= 48) sector_y++, y -= 48;
	sector_x += univ.party.outdoor_corner.x;
	sector_y += univ.party.outdoor_corner.y;
	// ASAN can happens if a hole allows to goes out the scenario...
	if (sector_x<0 || sector_x>=univ.scenario.outdoors.width() ||
		sector_y<0 || sector_y>=univ.scenario.outdoors.height())
		return false;
	return univ.scenario.outdoors[sector_x][sector_y]->roads[x][y];
}

cUniverse::cUniverse(ePartyPreset party_type) : party(party_type), out(*this), town(*this) {}

cUniverse::cUniverse(const cUniverse& other)
	: strbuf(other.strbuf)
	, extrabufs(other.extrabufs)
	, cur_pc(other.cur_pc)
	, scenario(other.scenario)
	, party(other.party)
	, stored_pcs(other.stored_pcs)
	, town(*this)
	, out(*this)
	, file(other.file)
	, debug_mode(other.debug_mode)
	, ghost_mode(other.ghost_mode)
	, node_step_through(other.node_step_through)
{
	for(auto& p : stored_pcs) {
		p.second = new cPlayer(*p.second);
	}
	town.copy(other.town);
	out.copy(other.out);
}

cUniverse::cUniverse(cUniverse&& other) : town(*this), out(*this) {
	swap(other);
}

cUniverse& cUniverse::operator=(cUniverse other) {
	swap(other);
	return *this;
}

void cUniverse::swap(cUniverse& other) {
	party.swap(other.party);
	town.swap(other.town);
	out.swap(other.out);
	scenario.swap(other.scenario);
	std::swap(stored_pcs, other.stored_pcs);
	std::swap(file, other.file);
	std::swap(debug_mode, other.debug_mode);
	std::swap(ghost_mode, other.ghost_mode);
	std::swap(node_step_through, other.node_step_through);
	std::swap(cur_pc, other.cur_pc);
	std::swap(strbuf, other.strbuf);
	std::swap(extrabufs, other.extrabufs);
}

void cCurOut::copy(const cCurOut& other) {
	memcpy(out, other.out, sizeof(out));
	memcpy(out_e, other.out_e, sizeof(out_e));
}

void cCurOut::swap(cCurOut& other) {
	cCurOut temp(univ);
	temp.copy(other);
	other.copy(*this);
	copy(temp);
}

void cCurTown::copy(const cCurTown& other) {
	cur_talk_loaded = other.cur_talk_loaded;
	quickfire_present = other.quickfire_present;
	belt_present = other.belt_present;
	difficulty = other.difficulty;
	monst = other.monst;
	items = other.items;
	memcpy(fields, other.fields, sizeof(fields));
}

void cCurTown::swap(cCurTown& other) {
	std::swap(cur_talk_loaded, other.cur_talk_loaded);
	std::swap(quickfire_present, other.quickfire_present);
	std::swap(belt_present, other.belt_present);
	std::swap(difficulty, other.difficulty);
	monst.swap(other.monst);
	std::swap(items, other.items);
	unsigned long temp[64][64];
	memcpy(temp, other.fields, sizeof(fields));
	memcpy(other.fields, fields, sizeof(fields));
	memcpy(fields, temp, sizeof(fields));
}

void cUniverse::check_monst(cMonster& monst) {
	if(monst.see_spec == -2) return; // Avoid infinite recursion
	monst.see_spec = -2;
	if(monst.picture_num >= 10000) {
		int pic = monst.picture_num - 10000;
		int sz = pic / 1000, base = pic % 1000;
		int numGraph = 4;
		if(sz > 1) numGraph *= 2;
		if(sz == 4) numGraph *= 2;
		for(int i = 0; i < numGraph; i++)
			used_graphics.insert(base + i);
	} else if(monst.picture_num >= 1000) {
		update_monsters[monst.picture_num - 1000].insert(&monst);
	}
	for(auto& abil : monst.abil) {
		switch(getMonstAbilCategory(abil.first)) {
			case eMonstAbilCat::MISSILE:
				if(abil.second.missile.pic >= 10000) {
					for(int i = 0; i < 4; i++)
						used_graphics.insert(abil.second.missile.pic - 10000 + i);
				} else if(abil.second.missile.pic >= 1000) {
					update_missiles[abil.second.missile.pic - 1000].insert(&abil.second.missile.pic);
				}
				break;
			case eMonstAbilCat::GENERAL:
				if(abil.second.gen.pic >= 10000) {
					for(int i = 0; i < 4; i++)
						used_graphics.insert(abil.second.gen.pic - 10000 + i);
				} else if(abil.second.gen.pic >= 1000) {
					update_missiles[abil.second.gen.pic - 1000].insert(&abil.second.gen.pic);
				}
				break;
			case eMonstAbilCat::SUMMON:
				if(abil.second.summon.type == eMonstSummon::TYPE)
					check_monst(scenario.scen_monsters[abil.second.summon.what]);
				break;
			case eMonstAbilCat::RADIATE:
			case eMonstAbilCat::SPECIAL:
			case eMonstAbilCat::INVALID:
				break;
		}
	}
}

void cUniverse::check_item(cItem& item) {
	if(item.variety == eItemType::NO_ITEM) return;
	if(item.graphic_num >= 10000)
		used_graphics.insert(item.graphic_num - 10000);
	else if(item.graphic_num >= 1000)
		update_items[item.graphic_num - 1000].insert(&item);
	if(item.ability == eItemAbil::SUMMONING || item.ability == eItemAbil::MASS_SUMMONING) {
		mon_num_t monst = item.abil_data[1];
		if(monst >= 10000)
			check_monst(party.summons[monst - 10000]);
		else check_monst(scenario.scen_monsters[monst]);
	}
	if(item.variety == eItemType::ARROW || item.variety == eItemType::BOLTS || item.variety == eItemType::MISSILE_NO_AMMO || item.variety == eItemType::THROWN_MISSILE) {
		if(item.missile >= 10000)
			for(int i = 0; i < 4; i++)
				used_graphics.insert(item.missile - 10000 + i);
		else if(item.missile >= 1000)
			update_missiles[item.missile - 1000].insert(&item.missile);
	}
}

// This attempts to find the index of a living entity in the party or town
// Assuming success, the two get_target() calls are a round-trip
// Returns maxint on failure (which could happen eg with a stored PC or a monster from a saved town)
size_t cUniverse::get_target_i(iLiving& who) {
	if(dynamic_cast<cParty*>(&who))
		return 6;
	else if(cPlayer* check = dynamic_cast<cPlayer*>(&who)) {
		for(int i = 0; i < 6; i++)
			if(check == &party[i])
				return i;
	} else if(cCreature* check = dynamic_cast<cCreature*>(&who)) {
		for(size_t i = 0; i < town.monst.size(); i++)
			if(check == &town.monst[i])
				return i + 100;
	}
	return -1;
}

// TODO: Both of these have an issue that they'll return garbage if called outdoors
// It's less of a problem with target_there since that should never actually be called outdoors,
// but get_target would be called by "affect PC" special nodes which could be run outdoors.
iLiving& cUniverse::get_target(size_t which) {
	size_t maxint = -1;
	if(which == maxint || which == 6)
		return party;
	else if(which < 6)
		return party[which];
	else if(which >= 100 && which < 100 + town.monst.size())
		return town.monst[which - 100];
	else throw std::string("Tried to get nonexistent target!");
}

iLiving* cUniverse::target_there(location where, eTargetType type) {
	if(type == TARG_ANY || type == TARG_PC) {
		for(int i = 0; i < 6; i++)
			if(party[i].is_alive() && where == party[i].get_loc())
				return &party[i];
	}
	if(type == TARG_ANY || type == TARG_MONST) {
		for(size_t i = 0; i < town.monst.size(); i++)
			if(town.monst[i].is_alive() && town.monst[i].on_space(where))
				return &town.monst[i];
	}
	return nullptr;
}

unsigned char& cUniverse::cpn_flag(unsigned int x, unsigned int y, std::string id) {
	if(id.empty()) id = scenario.campaign_id;
	if(id.empty()) id = scenario.scen_name;
	if(x >= 25 || y >= 25) throw std::range_error("Attempted to access a campaign flag out of range (0..25)");
	return party.campaign_flags[id].idx[x][y];
}

extern cCustomGraphics spec_scen_g;

pic_num_t cUniverse::addGraphic(pic_num_t pic, ePicType type) {
	// Now find the first unused slot with sufficient space for the graphic we're adding
	int needSlots = 0;
	switch(type - PIC_PARTY) {
		case PIC_MONST: needSlots = 4; break;
		case PIC_MONST_WIDE: needSlots = 8; break;
		case PIC_MONST_TALL: needSlots = 8; break;
		case PIC_MONST_LG: needSlots = 16; break;
		case PIC_ITEM: needSlots = 1; break;
		case PIC_PC: needSlots = 4; break;
		case PIC_MISSILE: needSlots = 4; break;
		default: break;
	}
	pic_num_t pos = -1;
	bool foundSpace = false;
	while(!foundSpace) {
		// First find an empty slot.
		while(used_graphics.count(++pos));
		// Then check if there's enough space.
		foundSpace = true;
		for(pic_num_t i = 1; i < needSlots; i++) {
			if(used_graphics.count(pos + i)) foundSpace = false;
		}
	}
	// And finally, actually transfer the graphic over
	spec_scen_g.copy_graphic(pos, pic, needSlots);
	// Also mark these slots used so we don't overwrite them with the next copy
	for(pic_num_t i = 1; i < needSlots; i++) {
		used_graphics.insert(pos + i);
	}
	return pos;
}

void cUniverse::exportGraphics() {
	// First determine which graphics are used, and which need to be copied.
	// The party sheet can contain the following types of graphics:
	// - Monster graphics for monsters summoned by custom items or captured in the party's soul crystal
	// - Item graphics for custom items that the party has in their possession or in their saved item rectangles
	// - Custom PC graphics
	// TODO: Missile graphics for custom monsters
	// So basically, almost all the graphics are linked to items.
	used_graphics.clear();
	for(int i = 0; i < 6; i++) {
		if(party[i].main_status == eMainStatus::ABSENT)
			continue;
		if(party[i].which_graphic >= 10000) {
			for(int j = 0; j < 4; j++)
				used_graphics.insert(party[i].which_graphic - 10000 + j);
		} else if(party[i].which_graphic >= 1000)
			update_pcs[party[i].which_graphic - 1000].insert(&party[i]);
		for(size_t j = 0; j < party[i].items.size(); j++) {
			check_item(party[i].items[j]);
		}
	}
	for(size_t i = 0; i < party.stored_items.size(); i++) {
		for(size_t j = 0; j < party.stored_items[i].size(); j++) {
			check_item(party.stored_items[i][j]);
		}
	}
	for(mon_num_t monst : party.imprisoned_monst) {
		if(monst > 0 && monst < scenario.scen_monsters.size())
			check_monst(scenario.scen_monsters[monst]);
		else if(monst >= 10000 && monst - 10000 < party.summons.size())
			check_monst(party.summons[monst - 10000]);
	}
	// And then, just add all the graphics, and update references to them
	for(auto pic : update_pcs) {
		pic_num_t pos = addGraphic(pic.first, PIC_PC);
		for(auto& pc : pic.second)
			pc->which_graphic = 10000 + pos;
	}
	update_pcs.clear();
	for(auto pic : update_items) {
		pic_num_t pos = addGraphic(pic.first, PIC_ITEM);
		for(auto& item : pic.second)
			item->graphic_num = 10000 + pos;
	}
	update_items.clear();
	for(auto pic : update_missiles) {
		pic_num_t pos = addGraphic(pic.first, PIC_MISSILE);
		for(auto& missile : pic.second)
			*missile = 10000 + pos;
	}
	update_missiles.clear();
	for(auto pic : update_monsters) {
		int sz = pic.first / 1000, base = pic.first % 1000;
		ePicType type;
		switch(sz) {
			case 1: type = PIC_MONST; break;
			case 2: type = PIC_MONST_WIDE; break;
			case 3: type = PIC_MONST_TALL; break;
			case 4: type = PIC_MONST_LG; break;
			default: continue;
		}
		pic_num_t pos = addGraphic(base, type);
		for(auto& monst : pic.second)
			monst->picture_num = 10000 + sz * 1000 + pos;
	}
	update_monsters.clear();
}

void cUniverse::exportSummons() {
	std::set<mon_num_t> used_monsters, need_monsters;
	std::map<mon_num_t, update_info<cItem>> update_items;
	for(int i = 0; i < 6; i++) {
		if(party[i].main_status == eMainStatus::ABSENT)
			continue;
		for(size_t j = 0; j < party[i].items.size(); j++) {
			if(party[i].items[j].variety == eItemType::NO_ITEM) continue;
			if(party[i].items[j].ability == eItemAbil::SUMMONING || party[i].items[j].ability == eItemAbil::MASS_SUMMONING) {
				mon_num_t monst = party[i].items[j].abil_data[1];
				if(monst >= 10000)
					used_monsters.insert(monst - 10000);
				else {
					need_monsters.insert(monst);
					update_items[monst].insert(&party[i].items[j]);
				}
			}
		}
	}
	for(size_t i = 0; i < party.stored_items.size(); i++) {
		for(size_t j = 0; j < party.stored_items[i].size(); j++) {
			if(party.stored_items[i][j].variety == eItemType::NO_ITEM) continue;
			if(party.stored_items[i][j].ability == eItemAbil::SUMMONING||party.stored_items[i][j].ability == eItemAbil::MASS_SUMMONING) {
				mon_num_t monst = party.stored_items[i][j].abil_data[1];
				if(monst >= 10000)
					used_monsters.insert(monst - 10000);
				else {
					need_monsters.insert(monst);
					update_items[monst].insert(&party.stored_items[i][j]);
				}
			}
		}
	}
	for(mon_num_t monst : party.imprisoned_monst) {
		if(monst == 0) continue;
		if(monst >= 10000)
			used_monsters.insert(monst - 10000);
		else need_monsters.insert(monst);
	}
	std::stack<mon_num_t> last_check;
	for(mon_num_t m : need_monsters) last_check.push(m);
	while(!last_check.empty()) {
		mon_num_t monst = last_check.top();
		last_check.pop();
		cMonster& what = scenario.scen_monsters[monst];
		if(what.abil[eMonstAbil::SUMMON].active && what.abil[eMonstAbil::SUMMON].summon.type == eMonstSummon::TYPE) {
			mon_num_t summon = what.abil[eMonstAbil::SUMMON].summon.what;
			if(summon >= 10000)
				used_monsters.insert(summon - 10000);
			else if(!need_monsters.count(summon)) {
				last_check.push(summon);
				need_monsters.insert(summon);
			}
		}
	}
	// Now that we know which exported summon slots are still in use and which new monsters need to be exported,
	// we can copy the monster records from the scenario record into the exported summon slots.
	if(used_monsters.empty()) party.summons.clear();
	else {
		auto max = std::max_element(used_monsters.begin(), used_monsters.end());
		party.summons.resize(*max + 1);
	}
	for(mon_num_t monst : need_monsters) {
		mon_num_t dest = -1;
		while(used_monsters.count(++dest));
		used_monsters.insert(dest);
		if(dest < party.summons.size())
			party.summons[dest] = scenario.scen_monsters[monst];
		else party.summons.push_back(scenario.scen_monsters[monst]);
		for(auto& item : update_items[monst])
			item->abil_data[1] = 10000 + dest;
		for(mon_num_t& sc : party.imprisoned_monst)
			if(sc == monst)
				sc = dest + 10000;
	}
}

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

cUniverse::~cUniverse() {
	clear_stored_pcs();
}

void cUniverse::clear_stored_pcs() {
	for(auto& p : stored_pcs)
		delete p.second;
	stored_pcs.clear();
}

short cCurTown::countMonsters(){
	short to_ret = 0;
	for(short i = 0; i < monst.size(); i++)
		if(monst[i].active > 0)
			to_ret++;
	return to_ret;
}

void cUniverse::enter_scenario(const std::string& name) {
	using namespace std::placeholders;
	
	party.age = 0;
	memset(party.stuff_done, 0, sizeof(party.stuff_done));
	party.light_level = 0;
	party.outdoor_corner = scenario.out_sec_start;
	party.i_w_c = {0, 0};
	party.loc_in_sec = scenario.out_start;
	party.out_loc = scenario.out_start;
	party.boats.clear();
	party.horses.clear();
	std::copy_if(scenario.boats.begin(), scenario.boats.end(), std::back_inserter(party.boats), std::bind(&cVehicle::exists, _1));
	std::copy_if(scenario.horses.begin(), scenario.horses.end(), std::back_inserter(party.horses), std::bind(&cVehicle::exists, _1));
	for(auto& pc : party) {
		pc.status.clear();
		if(isSplit(pc.main_status))
			pc.main_status -= eMainStatus::SPLIT;
		pc.cur_health = pc.max_health;
		pc.cur_sp = pc.max_sp;
	}
	party.in_boat = -1;
	party.in_horse = -1;
	for(auto& pop : party.creature_save)
		pop.which_town = 200;
	for(short i = 0; i < 10; i++)
		party.out_c[i].exists = false;
	party.store_limited_stock.clear();
	party.magic_store_items.clear();
	// TODO: Now uncertain if the journal should really persist
//	univ.party.journal.clear();
	party.special_notes.clear();
	party.talk_save.clear();
	// reset the scried monster
	party.m_noted.clear();
	
	party.direction = DIR_N;
	party.at_which_save_slot = 0;
	for(auto town : scenario.towns) {
		town->can_find = !town->is_hidden;
		town->m_killed = 0;
		town->clear_items_taken();
		for(auto& m : town->maps)
			m.reset();
	}
	party.key_times.clear();
	party.party_event_timers.clear();
	party.spec_items.clear();
	for(short i = 0; i < scenario.special_items.size(); i++) {
		if(scenario.special_items[i].flags >= 10)
			party.spec_items.insert(i);
	}
	for(short i = 0; i < scenario.quests.size(); i++) {
		if(scenario.quests[i].flags >= 10) {
			party.active_quests[i] = cJob(1);
		}
	}
	
	refresh_store_items();
	
	for(short i = 0; i < 96; i++)
		for(short j = 0; j < 96; j++)
			out.out_e[i][j] = 0;
	for(short i = 0; i < 3; i++)
		party.stored_items[i].clear();
	
	for(auto sector : scenario.outdoors)
		for(auto& m : sector->maps)
			m.reset();
	
	// time to init the town's difficulty
	town.difficulty = scenario.difficulty;
	party.scen_name = name;
}

void cUniverse::generate_job_bank(int which, job_bank_t& bank) {
	std::fill(bank.jobs.begin(), bank.jobs.end(), -1);
	bank.inited = true;
	size_t iSlot = 0;
	for(size_t i = 0; iSlot < 4 && i < scenario.quests.size(); i++) {
		if(scenario.quests[i].bank1 != which && scenario.quests[i].bank2 != which)
			continue;
		if(party.active_quests[i].status != eQuestStatus::AVAILABLE)
			continue;
		if(get_ran(1,1,100) <= 50 - bank.anger)
			bank.jobs[iSlot++] = i;
	}
}

cItem cUniverse::get_random_store_item(int loot_type, bool allow_junk_treasure) {
	cItem item = scenario.return_treasure(loot_type, allow_junk_treasure);
	if(item.variety == eItemType::GOLD || item.variety == eItemType::SPECIAL || item.variety == eItemType::FOOD || item.variety == eItemType::QUEST)
		item = cItem();
	item.ident = true;
	return item;
}

void cUniverse::refresh_store_items() {
	for(size_t i = 0; i < scenario.shops.size(); i++) {
		if(scenario.shops[i].getType() != eShopType::RANDOM)
			continue;
		for(int j = 0; j < scenario.shops[i].size(); j++) {
			cShopItem entry = scenario.shops[i].getItem(j);
			if(entry.type == eShopItemType::TREASURE) {
				party.magic_store_items[i][j] = get_random_store_item(entry.item.item_level, entry.item.item_level == 0);
				continue;
			} else if(entry.type == eShopItemType::CLASS) {
				std::set<int> choices;
				for(int k = 0; k < scenario.scen_items.size(); k++) {
					if(scenario.scen_items[k].special_class == entry.item.special_class)
						choices.insert(k);
				}
				int choice = get_ran(1,0,choices.size());
				if(choice < choices.size()) {
					auto iter = choices.begin();
					std::advance(iter, choice);
					party.magic_store_items[i][j] = scenario.scen_items[*iter];
					continue;
				}
			} else if(entry.type == eShopItemType::OPT_ITEM) {
				int roll = get_ran(1,1,100);
				if(roll <= entry.quantity / 1000) {
					party.magic_store_items[i][j] = entry.item;
					continue;
				}
			}
		}
	}
	
	for(int i = 0; i < party.job_banks.size(); i++) {
		generate_job_bank(i, party.job_banks[i]);
	}
}

cPlayer& cUniverse::current_pc() {
	return party[cur_pc];
}

void(* cUniverse::print_result)(std::string) = nullptr;
