/*
 *  town.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "town.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "scenario.hpp"
#include "oldstructs.hpp"
#include "mathutil.hpp"

void cTown::append(legacy::big_tr_type&, int){}
void cTown::append(legacy::ave_tr_type&, int){}
void cTown::append(legacy::tiny_tr_type&, int){}

void cTown::append(legacy::town_record_type& old){
	town_chop_time = old.town_chop_time;
	town_chop_key = old.town_chop_key;
	for(short i = 0; i < 4; i++){
		start_locs[i].x = old.start_locs[i].x;
		start_locs[i].y = old.start_locs[i].y;
		exits[i].x = old.exit_locs[i].x;
		exits[i].y = old.exit_locs[i].y;
		exits[i].spec = old.exit_specs[i];
		wandering[i].append(old.wandering[i]);
	}
	preset_fields.clear();
	preset_fields.reserve(50);
	special_locs.resize(50);
	for(short i = 0; i < 50; i++){
		special_locs[i].x = old.special_locs[i].x;
		special_locs[i].y = old.special_locs[i].y;
		if(old.special_locs[i].x == 100)
			special_locs[i].spec = -1;
		else special_locs[i].spec = old.spec_id[i];
		cField temp;
		temp.append(old.preset_fields[i]);
		preset_fields.push_back(temp);
	}
	sign_locs.resize(15);
	for(short i = 0; i < 15; i++){
		sign_locs[i].x = old.sign_locs[i].x;
		sign_locs[i].y = old.sign_locs[i].y;
	}
	lighting_type = (eLighting) old.lighting;
	in_town_rect.top = old.in_town_rect.top;
	in_town_rect.left = old.in_town_rect.left;
	in_town_rect.bottom = old.in_town_rect.bottom;
	in_town_rect.right = old.in_town_rect.right;
	preset_items.resize(64);
	for(short i = 0; i < 64; i++){
		preset_items[i].append(old.preset_items[i]);
	}
	max_num_monst = old.max_num_monst;
	spec_on_entry = old.spec_on_entry;
	spec_on_entry_if_dead = old.spec_on_entry_if_dead;
	spec_on_hostile = -1;
	for(short i = 0; i < 8; i++){
		timers[i].time = old.timer_spec_times[i];
		timers[i].node = old.timer_specs[i];
	}
	specials.resize(100);
	for(short i = 0; i < 100; i++)
		specials[i].append(old.specials[i]);
	difficulty = old.difficulty;
	strong_barriers = defy_scrying = defy_mapping = false;
}

cTown::cTown(cScenario& scenario) : scenario(&scenario) {
	cTown::cWandering d_wan = {0,0,0,0};
	
	town_chop_time = -1;
	town_chop_key = -1;
	for(short i = 0; i < wandering.size(); i++) {
		wandering[i] = d_wan;
		wandering_locs[i].x = 100;
	}
	lighting_type = LIGHT_NORMAL;
	for(short i = 0; i < 4; i++) {
		start_locs[i].x = 100;
		exits[i].spec = -1;
		exits[i].x = -1;
		exits[i].y = -1;
	}
	max_num_monst = 30000;
	spec_on_entry = -1;
	spec_on_entry_if_dead = -1;
	spec_on_hostile = -1;
	for(short i = 0; i < 8; i++) {
		timers[i].time = 0;
		timers[i].node = -1;
	}
	difficulty = 0;
	bg_town = bg_fight = -1;
	strong_barriers = defy_scrying = defy_mapping = is_hidden = has_tavern = false;
	
	town_name = "Town name";
	comment[0] = "Comment 1";
	comment[1] = "Comment 2";
	comment[2] = "Comment 3";
	
	for(auto& who : talking.people) {
		who.title = "Unused";
	}
}

void cTown::init_start() {
	short s = this->max_dim();
	start_locs[0].x = s / 2;
	start_locs[0].y = 4;
	start_locs[2].x = s / 2;
	start_locs[2].y = s - 5;
	start_locs[1].x = s - 5;
	start_locs[1].y = s / 2;
	start_locs[3].x = 4;
	start_locs[3].y = s / 2;
	in_town_rect.top = 3;
	in_town_rect.bottom = s - 4;
	in_town_rect.left = 3;
	in_town_rect.right = s - 4;
}

void cTown::cWandering::append(legacy::wandering_type old){
	monst[0] = old.monst[0];
	monst[1] = old.monst[1];
	monst[2] = old.monst[2];
	monst[3] = old.monst[3];
}

void cTown::cItem::append(legacy::preset_item_type old){
	loc.x = old.item_loc.x;
	loc.y = old.item_loc.y;
	code = old.item_code;
	charges = old.ability;
	always_there = old.always_there;
	property = old.property;
	contained = old.contained;
}

void cTown::cField::append(legacy::preset_field_type old){
	loc.x = old.field_loc.x;
	loc.y = old.field_loc.y;
	switch(old.field_type) {
		case 3: type = FIELD_WEB; break;
		case 4: type = OBJECT_CRATE; break;
		case 5: type = OBJECT_BARREL; break;
		case 6: type = BARRIER_FIRE; break;
		case 7: type = BARRIER_FORCE; break;
		case 8: type = FIELD_QUICKFIRE; break;
		case 14: type = SFX_SMALL_BLOOD; break;
		case 15: type = SFX_MEDIUM_BLOOD; break;
		case 16: type = SFX_LARGE_BLOOD; break;
		case 17: type = SFX_SMALL_SLIME; break;
		case 18: type = SFX_LARGE_SLIME; break;
		case 19: type = SFX_ASH; break;
		case 20: type = SFX_BONES; break;
		case 21: type = SFX_RUBBLE; break;
	}
}

bool cTown::cWandering::isNull(){
	for(short i = 0;i < 4;i++)
		if(monst[i] != 0)
			return false;
	return true;
}

bool cTown::is_cleaned_out(long m_killed) {
	if(max_num_monst < 0) return false;
	return m_killed >= max_num_monst;
}

void cTown::set_up_lights() {
	using namespace std::placeholders;
	short rad;
	location where,l;
	bool where_lit[64][64] = {0};
	auto get_obscurity = std::bind(&cTown::light_obscurity, this, _1, _2);
	
	// Find bonfires, braziers, etc.
	for(short i = 0; i < this->max_dim(); i++)
		for(short j = 0; j < this->max_dim(); j++) {
			l.x = i;
			l.y = j;
			rad = scenario->ter_types[this->terrain(i,j)].light_radius;
			if(rad > 0) {
				for(where.x = std::max(0,i - rad); where.x < min(this->max_dim(),short(i + rad + 1)); where.x++)
					for(where.y = std::max(0,j - rad); where.y < min(this->max_dim(),short(j + rad + 1)); where.y++)
						if(!where_lit[where.x][where.y] && dist(where,l) <= rad && can_see(l,where,get_obscurity) < 5)
							where_lit[where.x][where.y] = true;
			}
		}
	for(short i = 0; i < this->max_dim() / 8; i++)
		for(short j = 0; j < this->max_dim(); j++)
			this->lighting(i,j) = 0;
	for(where.x = 0; where.x < this->max_dim(); where.x++)
		for(where.y = 0; where.y < this->max_dim(); where.y++) {
			if(where_lit[where.x][where.y]) {
				this->lighting(where.x / 8,where.y) = this->lighting(where.x / 8,where.y) | (1 << (where.x % 8));
			}
		}
}

short cTown::light_obscurity(short x,short y) {
	ter_num_t what_terrain;
	eTerObstruct store;
	
	what_terrain = this->terrain(x,y);
	
	store = scenario->ter_types[what_terrain].blockage;
	if(store == eTerObstruct::BLOCK_SIGHT || store == eTerObstruct::BLOCK_MOVE_AND_SIGHT)
		return 5;
	if(store == eTerObstruct::BLOCK_MOVE_AND_SHOOT)
		return 1;
	return 0;
}

cTown::cItem::cItem() {
	loc = {80,80};
	code = -1;
	ability = -1;
	charges = 0;
	always_there = false;
	property = false;
	contained = false;
}

cTown::cItem::cItem(location loc, short num, ::cItem& item) : cItem() {
	this->loc = loc;
	code = num;
	if(item.variety == eItemType::GOLD || item.variety == eItemType::FOOD)
		charges = get_ran(1,4,6);
}

void cTown::reattach(cScenario& scen) {
	scenario = &scen;
}
