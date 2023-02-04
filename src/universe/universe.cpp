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

#include "scenario/town.hpp"
#include "oldstructs.hpp"
#include "mathutil.hpp"
#include "fileio/fileio.hpp"
#include "fileio/tagfile.hpp"
#include "gfx/gfxsheets.hpp"

void cCurOut::import_legacy(legacy::out_info_type const &old){
	for(int i = 0; i < 96; i++)
		for(int j = 0; j < 96; j++)
			out_e[i][j] = old.expl[i][j];
}

void cCurTown::import_legacy(legacy::current_town_type const &old){
	univ.party.town_num = old.town_num;
	record()->import_legacy(old.town);
	// TODO: Is this right? Was current_town_type::difficulty just a mirror of town difficulty?
	record()->difficulty = old.difficulty;
	for(int i = 0; i < 64; i++)
		for(int j = 0; j < 64; j++)
			fields[i][j] = old.explored[i][j];
	monst.import_legacy(old.monst);
	univ.party.town_loc.x = old.p_loc.x;
	univ.party.town_loc.y = old.p_loc.y;
	cur_talk_loaded = old.town_num;
}

void cCurTown::import_legacy(legacy::big_tr_type const &old){
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

void cCurTown::import_legacy(legacy::town_item_list const &old){
	items.resize(115);
	for(int i = 0; i < 115; i++)
		items[i].import_legacy(old.items[i]);
}

void cUniverse::import_legacy(legacy::stored_town_maps_type const &old){
	for(int n = 0; n < scenario.towns.size(); n++)
		for(size_t j = 0; j < scenario.towns[n]->maps.size(); j++)
			for(int i = 0; i < scenario.towns[n]->maps[j].size(); i++)
				scenario.towns[n]->maps[j][i] = old.town_maps[n][i / 8][j] & (1 << (i % 8));
}

static short onm(char x_sector,char y_sector, char w) {
	return y_sector * w + x_sector;
}

void cUniverse::import_legacy(legacy::stored_outdoor_maps_type const &old){
	for(int x = 0; x < scenario.outdoors.width(); x++)
		for(int y = 0; y < scenario.outdoors.height(); y++)
			for(int i = 0; i < 48; i++)
				for(int j = 0; j < 48; j++)
					scenario.outdoors[x][y]->maps[i][j] = old.outdoor_maps[onm(x,y,scenario.outdoors.width())][i / 8][j] & (1 << i % 8);
}

void cCurTown::import_reset_fields_legacy(){
	// boe does not use the stored sfx and misc_i
	// but discard them and recompute their values
	auto const &terrain=record()->terrain;
	for (auto const &f : record()->preset_fields) {
		if (f.loc.x<0 || f.loc.x>=terrain.width() ||
			f.loc.y<0 || f.loc.y>=terrain.height()) continue;
		// only 0<old_type<9 and 14<=old_type<=21 are retrieved here
		if ((f.type>8 && f.type<17) || (f.type>=30 & f.type<=37))
			fields[f.loc.x][f.loc.y]|=f.type;
	}
	for (auto const &spec : record()->special_locs) {
		if (spec.spec<0 || spec.x<0 || spec.x>=terrain.width() ||
			spec.y<0 || spec.y>=terrain.height()) continue;
		if (univ.get_terrain(terrain[spec.x][spec.y]).i==3000)
			fields[spec.x][spec.y]|=SPECIAL_SPOT;
	}
}

cTown* cCurTown::operator -> (){
	return record();
}

cTown& cCurTown::operator * (){
	return *record();
}

const cTown* cCurTown::operator -> () const {
	return record();
}

const cTown& cCurTown::operator * () const {
	return *record();
}

void cCurTown::place_preset_fields() {
	// Initialize barriers, etc. Note non-sfx gets forgotten if this is a town recently visited.
	fields.resize(record()->max_dim, record()->max_dim);
	fields.fill(0);
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

void cCurTown::update_fields(const vector2d<unsigned short>& setup) {
	for(short i = 0; i < record()->max_dim && i < setup.width(); i++) {
		for(short j = 0; j < record()->max_dim && j < setup.height(); j++) {
			// except that pushable things restore to orig locs
			unsigned short temp = setup[i][j] << 8;
			temp &= ~(OBJECT_CRATE | OBJECT_BARREL | OBJECT_BLOCK);
			univ.town.fields[i][j] |= temp;
		}
	}
}

void cCurTown::save_setup(vector2d<unsigned short>& setup) const {
	setup.resize(record()->max_dim, record()->max_dim);
	for(short i = 0; i < record()->max_dim; i++) {
		for(short j = 0; j < record()->max_dim; j++) {
			setup[i][j] = fields[i][j] >> 8;
		}
	}
}

cSpeech& cCurTown::cur_talk() {
	// TODO: Make sure we actually have a valid speech stored
	return univ.scenario.towns[cur_talk_loaded]->talking;
}

const cSpeech& cCurTown::cur_talk() const {
	// TODO: Make sure we actually have a valid speech stored
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
	fields.resize(AREA_MEDIUM, AREA_MEDIUM);
	fields.fill(0);
}

cCurTown::~cCurTown() {
	if(arena != nullptr) delete arena;
}

cTown*const cCurTown::record() const {
	if(univ.party.town_num == 200) return arena;
	return univ.scenario.towns[univ.party.town_num];
}

bool cCurTown::is_summon_safe(short x, short y) const {
	if(!is_on_map(x, y)) return false;
	// Here 254 indicates the low byte of the town fields, minus explored spaces (which is lowest bit).
	static const unsigned long blocking_fields = SPECIAL_SPOT | OBJECT_CRATE | OBJECT_BARREL | OBJECT_BLOCK | FIELD_QUICKFIRE | 254;
	return fields[x][y] & blocking_fields;
}

unsigned long cCurTown::get_fields(short x, short y) const
{
	if(!is_on_map(x,y)) return 0;
	return fields[x][y];
}

bool cCurTown::is_special(short x, short y) const{
	if(!is_on_map(x,y)) return false;
	location check(x,y);
	for(int i = 0; i < record()->special_locs.size(); i++)
		if(check == record()->special_locs[i] && record()->special_locs[i].spec >= 0)
			return true;
	return false;
}

bool cCurTown::set_explored(short x, short y, bool b){
	if(!is_on_map(x, y)) return false;
	if(b) fields[x][y] |=  SPECIAL_EXPLORED;
	else  fields[x][y] &= ~SPECIAL_EXPLORED;
	return true;
}

bool cCurTown::set_force_wall(short x, short y, bool b){
	if(!is_on_map(x, y)) return false;
	if(b){ // If certain things are on space, there's no room for field.
		if(is_impassable(x,y))
			return false;
		// checkme: do we really want to change the comportement of quickfire in new game
		if(is_antimagic(x,y) || is_blade_wall(x,y) || (!univ.scenario.is_legacy && is_quickfire(x,y)))
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
	if(b) fields[x][y] |=  OBJECT_BLOCK;
	else  fields[x][y] &= ~OBJECT_BLOCK;
	return true;
}

bool cCurTown::set_spot(short x, short y, bool b){
	if(!is_on_map(x, y)) return false;
	if(b) fields[x][y] |=  SPECIAL_SPOT;
	else  fields[x][y] &= ~SPECIAL_SPOT;
	return true;
}

bool cCurTown::set_road(short x, short y, bool b){
	if(!is_on_map(x, y)) return false;
	if(b) fields[x][y] |=  SPECIAL_ROAD;
	else  fields[x][y] &= ~SPECIAL_ROAD;
	return true;
}

bool cCurTown::set_web(short x, short y, bool b){
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
	if(b){ // If certain things are on the space, there's no room for a crate.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_barrel(x,y))
			return false;
		fields[x][y]  |=  OBJECT_CRATE;
	}
	else fields[x][y] &= ~OBJECT_CRATE;
	return true;
}

bool cCurTown::set_barrel(short x, short y, bool b){
	if(!is_on_map(x, y)) return false;
	if(b){ // If certain things are on the space, there's no room for a crate.
		if(is_fire_barr(x,y) || is_force_barr(x,y) || is_quickfire(x,y) || is_crate(x,y))
			return false;
		fields[x][y]  |=  OBJECT_BARREL;
	}
	else fields[x][y] &= ~OBJECT_BARREL;
	return true;
}

bool cCurTown::set_fire_barr(short x, short y, bool b){
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
	if(b){ // If certain things are on space, there's no room for quickfire.
		cTerrain const &terrain=univ.get_terrain(record()->terrain(x,y));
		if(terrain.blockage == eTerObstruct::BLOCK_SIGHT)
			return false;
		// TODO: Isn't it a little odd that BLOCK_MOVE_AND_SHOOT isn't included here?
		if(terrain.blockage == eTerObstruct::BLOCK_MOVE_AND_SIGHT)
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
	if(!is_on_map(x, y)) return false;
	ter_num_t ter = record()->terrain(x,y);
	return univ.scenario.get_terrain(ter).blockage == eTerObstruct::CLEAR;
}

bool cCurTown::set_sm_blood(short x, short y, bool b){
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
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
	if(!is_on_map(x, y)) return false;
	if(b) fields[x][y] |=  BARRIER_CAGE;
	else  fields[x][y] &= ~BARRIER_CAGE;
	return true;
}

// TODO: This seems to be wrong; impassable implies "blocks movement", which two other blockages also do
bool cCurTown::is_impassable(short i,short  j) const {
	if(!is_on_map(i, j)) return false;
	ter_num_t ter = record()->terrain(i,j);
	if(univ.scenario.get_terrain(ter).blockage == eTerObstruct::BLOCK_MOVE_AND_SIGHT)
		return true;
	else return false;
}

bool cCurTown::is_on_map(short x, short y) const {
	if(x < 0 || y < 0) return false;
	if(x >= record()->max_dim) return false;
	if(y >= record()->max_dim) return false;
	return true;
}

auto cCurOut::operator [] (size_t i) -> arr_96& {
	return out[i];
}

auto cCurOut::operator [] (size_t i) const -> const arr_96& {
	return out[i];
}

ter_num_t& cCurOut::operator [] (location loc) {
	return out[loc.x][loc.y];
}

const ter_num_t& cCurOut::operator [] (location loc) const {
	return out[loc.x][loc.y];
}

void cCurOut::writeTo(std::ostream& file) const {
	writeArray(file, out, max_dim, max_dim);
	writeArray(file, out_e, max_dim, max_dim);
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
	readArray(file, out, max_dim, max_dim);
	readArray(file, out_e, max_dim, max_dim);
}

void cCurTown::writeTo(cTagFile& file) const {
	auto& page = file.add();
	page["TOWN"] << univ.party.town_num;
	page["DIFFICULTY"] << record()->difficulty;
	if(monst.hostile) page.add("HOSTILE");
	page["AT"] << univ.party.town_loc.x << univ.party.town_loc.y;
	for(size_t i = 0; i < items.size(); i++) {
		if(items[i].variety != eItemType::NO_ITEM) {
			auto& item_page = file.add();
			item_page["ITEM"] << i;
			items[i].writeTo(item_page);
		}
	}
	for(int i = 0; i < monst.size(); i++) {
		if(monst[i].active > 0) {
			auto& monst_page = file.add();
			monst_page["CREATURE"] << i;
			monst[i].writeTo(monst_page);
		}
	}
	auto& fields_page = file.add();
	fields_page["FIELDS"].encode(fields);
	fields_page["TERRAIN"].encode(record()->terrain);
	// TODO: Do we need to save special_spot?
}

void cCurTown::readFrom(const cTagFile& file){
	for(const auto& page : file) {
		if(page.index() == 0) {
			page["TOWN"] >> univ.party.town_num;
			page["DIFFICULTY"] >> record()->difficulty;
			monst.hostile = page.contains("HOSTILE");
			page["AT"] >> univ.party.town_loc.x >> univ.party.town_loc.y;
		} else if(page.getFirstKey() == "FIELDS" || page.getFirstKey() == "TERRAIN") {
			page["FIELDS"].extract(fields);
			page["TERRAIN"].extract(record()->terrain);
			fields.resize(record()->max_dim, record()->max_dim);
			for(size_t x = 0; x < record()->max_dim; x++) {
				for(size_t y = 0; y < record()->max_dim; y++) {
					if(is_quickfire(x, y)) {
						quickfire_present = true;
					}
				}
			}
			for(size_t x = 0; x < record()->max_dim; x++) {
				for(size_t y = 0; y < record()->max_dim; y++) {
					auto ter_num = record()->terrain(x,y);
					const auto ter_info = univ.scenario.get_terrain(ter_num);
					if(ter_info.special == eTerSpec::CONVEYOR) {
						belt_present = true;
					}
				}
			}
		} else if(page.getFirstKey() == "ITEM") {
			size_t i;
			page["ITEM"] >> i;
			if(i >= items.size()) items.resize(i + 1);
			items[i].readFrom(page);
		} else if(page.getFirstKey() == "CREATURE") {
			size_t i;
			page["CREATURE"] >> i;
			monst.init(i);
			monst[i].readFrom(page);
			monst[i].active = true;
		}
	}
}

cCurTown::cCurTown(cUniverse& univ) : univ(univ) {
	arena = nullptr;
	univ.party.town_num = 200;
}

cCurOut::cCurOut(cUniverse& univ) : univ(univ) {}

cOutdoors* cCurOut::operator->() {
	short x = univ.party.outdoor_corner.x + univ.party.i_w_c.x, y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
	return univ.scenario.outdoors[x][y];
}

const cOutdoors* cCurOut::operator->() const {
	return const_cast<cCurOut*>(this)->operator->();
}

bool cCurOut::is_spot(int x, int y) const {
	int sector_x = 0, sector_y = 0;
	if(x >= 48) sector_x++, x -= 48;
	if(y >= 48) sector_y++, y -= 48;
	sector_x += univ.party.outdoor_corner.x;
	sector_y += univ.party.outdoor_corner.y;
	// can happen if a hole allows to goes out the scenario
	if(sector_x < 0 || sector_x >= univ.scenario.outdoors.width() || sector_y < 0 || sector_y >= univ.scenario.outdoors.height())
		return false;
	return univ.scenario.outdoors[sector_x][sector_y]->is_special_spot(x,y);
}

bool cCurOut::is_road(int x, int y) const {
	int sector_x = 0, sector_y = 0;
	if(x >= 48) sector_x++, x -= 48;
	if(y >= 48) sector_y++, y -= 48;
	sector_x += univ.party.outdoor_corner.x;
	sector_y += univ.party.outdoor_corner.y;
	// can happen if a hole allows to goes out the scenario...
	if(sector_x < 0 || sector_x >= univ.scenario.outdoors.width() || sector_y < 0 || sector_y >= univ.scenario.outdoors.height())
		return false;
	return univ.scenario.outdoors[sector_x][sector_y]->is_road(x,y);
}

bool cCurOut::is_on_map(int x, int y) const {
	if(x < 0 || y < 0) return false;
	if(x >= max_dim) return false;
	if(y >= max_dim) return false;
	return true;
}

cUniverse::cUniverse(ePartyPreset party_type) : party(party_type), out(*this), town(*this) {}

cUniverse::cUniverse(const cUniverse& other)
	: strbuf(other.strbuf)
	, extrabufs(other.extrabufs)
	, cur_pc(other.cur_pc)
	, scenario(other.scenario)
	, party(other.party)
	, town(*this)
	, out(*this)
	, file(other.file)
	, debug_mode(other.debug_mode)
	, ghost_mode(other.ghost_mode)
	, node_step_through(other.node_step_through)
{
	town.copy(other.town);
	out.copy(other.out);
	for(const auto& pc : other.stored_pcs) {
		std::unique_ptr<cPlayer> clone(new cPlayer(no_party, *pc.second));
		stored_pcs.emplace(pc.first, std::move(clone));
	}
}

cUniverse::cUniverse(cUniverse&& other) : town(*this), out(*this) {
	swap(*this, other);
}

cUniverse& cUniverse::operator=(cUniverse other) {
	swap(*this, other);
	return *this;
}

void swap(cUniverse& lhs, cUniverse& rhs) {
	using std::swap;
	swap(lhs.party, rhs.party);
	swap(lhs.town, rhs.town);
	swap(lhs.out, rhs.out);
	swap(lhs.scenario, rhs.scenario);
	swap(lhs.stored_pcs, rhs.stored_pcs);
	swap(lhs.file, rhs.file);
	swap(lhs.debug_mode, rhs.debug_mode);
	swap(lhs.ghost_mode, rhs.ghost_mode);
	swap(lhs.node_step_through, rhs.node_step_through);
	swap(lhs.cur_pc, rhs.cur_pc);
	swap(lhs.strbuf, rhs.strbuf);
	swap(lhs.extrabufs, rhs.extrabufs);
}

void cCurOut::copy(const cCurOut& other) {
	out = other.out;
	out_e = other.out_e;
}

void swap(cCurOut& lhs, cCurOut& rhs) {
	using std::swap;
	swap(lhs.out, rhs.out);
	swap(lhs.out_e, rhs.out_e);
}

void cCurTown::copy(const cCurTown& other) {
	cur_talk_loaded = other.cur_talk_loaded;
	quickfire_present = other.quickfire_present;
	belt_present = other.belt_present;
	monst = other.monst;
	items = other.items;
	fields = other.fields;
}

void swap(cCurTown& lhs, cCurTown& rhs) {
	using std::swap;
	swap(lhs.cur_talk_loaded, rhs.cur_talk_loaded);
	swap(lhs.quickfire_present, rhs.quickfire_present);
	swap(lhs.belt_present, rhs.belt_present);
	swap(lhs.monst, rhs.monst);
	swap(lhs.items, rhs.items);
	swap(lhs.fields, rhs.fields);
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

struct cCustomUpdateState {
	template<typename T> using update_info = std::set<T *>;
	std::map<pic_num_t, update_info<cItem>> items;
	std::map<pic_num_t, update_info<cMonster>> monsters;
	std::map<pic_num_t, update_info<cPlayer>> pcs;
	std::map<pic_num_t, update_info<miss_num_t>> missiles;
	std::set<pic_num_t> graphics;
	
	std::set<cItem const *> seenItem;
	std::set<cMonster const *> seenMonster;
	void insert_missile_pict(miss_num_t &pict_id) {
		if(pict_id >= 10000) {
			for(int i = 0; i < 4; i++)
				graphics.insert(pict_id - 10000 + i);
		} else if(pict_id >= 1000)
			missiles[pict_id - 1000].insert(&pict_id);
	}
	pic_num_t add_graphic(pic_num_t pic, ePicType type);
	void check_monst(cUniverse &univers, cMonster & monst);
	void check_item(cUniverse &univers, cItem& item);
};

extern cCustomGraphics spec_scen_g;
pic_num_t cCustomUpdateState::add_graphic(pic_num_t pic, ePicType type) {
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
		while(graphics.count(++pos));
		// Then check if there's enough space.
		foundSpace = true;
		for(pic_num_t i = 1; i < needSlots; i++) {
			if(graphics.count(pos + i)) foundSpace = false;
		}
	}
	// And finally, actually transfer the graphic over
	spec_scen_g.copy_graphic(pos, pic, needSlots);
	// Also mark these slots used so we don't overwrite them with the next copy
	for(pic_num_t i = 0; i < needSlots; i++) {
		graphics.insert(pos + i);
	}
	return pos;
}

void cCustomUpdateState::check_monst(cUniverse &univers, cMonster & monst) {
	if (seenMonster.count(&monst)) return; // Avoid infinite recursion
	seenMonster.insert(&monst);
	if(monst.picture_num >= 10000) {
		int pic = monst.picture_num - 10000;
		int sz = pic / 1000, base = pic % 1000;
		int numGraph = 4;
		if(sz > 1) numGraph *= 2;
		if(sz == 4) numGraph *= 2;
		for(int i = 0; i < numGraph; i++)
			graphics.insert(base + i);
	} else if(monst.picture_num >= 1000) {
		monsters[monst.picture_num - 1000].insert(&monst);
	}
	for(auto& abil : monst.abil) {
		switch(getMonstAbilCategory(abil.first)) {
			case eMonstAbilCat::MISSILE:
				insert_missile_pict(abil.second.missile.pic);
				break;
			case eMonstAbilCat::GENERAL:
				insert_missile_pict(abil.second.gen.pic);
				break;
			case eMonstAbilCat::SUMMON:
				if(abil.second.summon.type == eMonstSummon::TYPE)
					check_monst(univers, univers.scenario.get_monster(abil.second.summon.what));
				break;
			case eMonstAbilCat::RADIATE:
			case eMonstAbilCat::SPECIAL:
			case eMonstAbilCat::INVALID:
				break;
		}
	}
}

void cCustomUpdateState::check_item(cUniverse &universe, cItem& item) {
	if (seenItem.count(&item)) return;
	seenItem.insert(&item);
	if(item.variety == eItemType::NO_ITEM) return;
	if(item.graphic_num >= 10000)
		graphics.insert(item.graphic_num - 10000);
	else if(item.graphic_num >= 1000)
		items[item.graphic_num - 1000].insert(&item);
	if(item.ability == eItemAbil::SUMMONING || item.ability == eItemAbil::MASS_SUMMONING) {
		mon_num_t monst = item.abil_data.value;
		if(monst >= 10000)
			check_monst(universe, universe.party.get_summon(monst - 10000));
		else check_monst(universe, universe.scenario.get_monster(monst));
	}
	if(item.variety == eItemType::ARROW || item.variety == eItemType::BOLTS || item.variety == eItemType::MISSILE_NO_AMMO || item.variety == eItemType::THROWN_MISSILE)
		insert_missile_pict(item.missile);
}

void cUniverse::exportGraphics() {
	// First determine which graphics are used, and which need to be copied.
	// The party sheet can contain the following types of graphics:
	// - Monster graphics for monsters summoned by custom items or captured in the party's soul crystal
	// - Item graphics for custom items that the party has in their possession or in their saved item rectangles
	// - Custom PC graphics
	// TODO: Missile graphics for custom monsters
	// So basically, almost all the graphics are linked to items.
	cCustomUpdateState state;
	for(int i = 0; i < 6; i++) {
		if(party[i].main_status == eMainStatus::ABSENT)
			continue;
		if(party[i].which_graphic >= 10000) {
			for(int j = 0; j < 4; j++)
				state.graphics.insert(party[i].which_graphic - 10000 + j);
		} else if(party[i].which_graphic >= 1000)
			state.pcs[party[i].which_graphic - 1000].insert(&party[i]);
		for (auto &item : party[i].items)
			state.check_item(*this, item);
	}
	for (auto &items : party.stored_items)
		for (auto &item : items)
			state.check_item(*this, item);
	for(mon_num_t monst : party.imprisoned_monst) {
		if(monst > 0 && monst < scenario.scen_monsters.size())
			state.check_monst(*this, scenario.scen_monsters[monst]);
		else if(monst >= 10000 && monst - 10000 < party.summons.size())
			state.check_monst(*this, party.get_summon(monst - 10000));
	}
	// And then, just add all the graphics, and update references to them
	for(auto const &pic : state.pcs) {
		pic_num_t pos = state.add_graphic(pic.first, PIC_PC);
		for(auto& pc : pic.second)
			pc->which_graphic = 10000 + pos;
	}
	for(auto const &pic : state.items) {
		pic_num_t pos = state.add_graphic(pic.first, PIC_ITEM);
		for(auto& item : pic.second)
			item->graphic_num = 10000 + pos;
	}
	for(auto const &pic : state.missiles) {
		pic_num_t pos = state.add_graphic(pic.first, PIC_MISSILE);
		for(auto& missile : pic.second)
			*missile = 10000 + pos;
	}
	for(auto const &pic : state.monsters) {
		int sz = pic.first / 1000, base = pic.first % 1000;
		ePicType type;
		switch(sz) {
			case 1: type = PIC_MONST; break;
			case 2: type = PIC_MONST_WIDE; break;
			case 3: type = PIC_MONST_TALL; break;
			case 4: type = PIC_MONST_LG; break;
			default: continue;
		}
		pic_num_t pos = state.add_graphic(base, type);
		for(auto& monst : pic.second)
			monst->picture_num = 10000 + sz * 1000 + pos;
	}
}

void cUniverse::exportSummons() {
	std::set<mon_num_t> used_monsters, need_monsters;
	std::map<mon_num_t, std::set<cItem *>> update_items;
	for(int i = 0; i < 6; i++) {
		if(party[i].main_status == eMainStatus::ABSENT)
			continue;
		for(auto &item : party[i].items) {
			if(item.variety == eItemType::NO_ITEM) continue;
			if(item.ability == eItemAbil::SUMMONING || item.ability == eItemAbil::MASS_SUMMONING) {
				mon_num_t monst = item.abil_data.value;
				if(monst >= 10000)
					used_monsters.insert(monst - 10000);
				else {
					need_monsters.insert(monst);
					update_items[monst].insert(&item);
				}
			}
		}
	}
	for(auto &items : party.stored_items) {
		for(auto &item : items) {
			if(item.variety == eItemType::NO_ITEM) continue;
			if(item.ability == eItemAbil::SUMMONING||item.ability == eItemAbil::MASS_SUMMONING) {
				mon_num_t monst = item.abil_data.value;
				if(monst >= 10000)
					used_monsters.insert(monst - 10000);
				else {
					need_monsters.insert(monst);
					update_items[monst].insert(&item);
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
		cMonster& what = scenario.get_monster(monst);
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
			party.summons[dest] = scenario.get_monster(monst);
		else party.summons.push_back(scenario.get_monster(monst));
		for(auto& item : update_items[monst])
			item->abil_data.value = 10000 + dest;
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

void cUniverse::clear_stored_pcs() {
	stored_pcs.clear();
}

short cCurTown::countMonsters() const {
	short to_ret = 0;
	for(short i = 0; i < monst.size(); i++)
		if(monst[i].active > 0)
			to_ret++;
	return to_ret;
}

void cUniverse::enter_scenario(const std::string& name) {
	using namespace std::placeholders;
	
	party.enter_scenario(scenario);
	party.scen_name = name;

	for(auto town : scenario.towns) {
		town->can_find = !town->is_hidden;
		town->m_killed = 0;
		town->clear_items_taken();
		for(auto& m : town->maps)
			m.reset();
	}
	refresh_store_items();
	
	for(short i = 0; i < 96; i++)
		for(short j = 0; j < 96; j++)
			out.out_e[i][j] = 0;
	
	for(auto sector : scenario.outdoors)
		for(auto& m : sector->maps)
			m.reset();
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
					party.magic_store_items[i][j] = scenario.get_item(*iter);
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
