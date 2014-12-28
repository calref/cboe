/*
 *  town.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "classes.h"
#include "oldstructs.h"

void cTown::append(legacy::big_tr_type&, int){}
void cTown::append(legacy::ave_tr_type&, int){}
void cTown::append(legacy::tiny_tr_type&, int){}

void cTown::append(legacy::town_record_type& old){
	int i;
	town_chop_time = old.town_chop_time;
	town_chop_key = old.town_chop_key;
	for(i = 0; i < 4; i++){
		start_locs[i].x = old.start_locs[i].x;
		start_locs[i].y = old.start_locs[i].y;
		exit_locs[i].x = old.exit_locs[i].x;
		exit_locs[i].y = old.exit_locs[i].y;
		exit_specs[i] = old.exit_specs[i];
		wandering[i].append(old.wandering[i]);
	}
	preset_fields.clear();
	preset_fields.reserve(50);
	for(i = 0; i < 50; i++){
		special_locs[i].x = old.special_locs[i].x;
		special_locs[i].y = old.special_locs[i].y;
		spec_id[i] = old.spec_id[i];
//		preset_fields[i].loc.x = old.preset_fields[i].field_loc.x;
//		preset_fields[i].loc.y = old.preset_fields[i].field_loc.y;
//		preset_fields[i].type = old.preset_fields[i].field_type;
		cField temp;
		temp.append(old.preset_fields[i]);
		preset_fields.push_back(temp);
	}
	for(i = 0; i < 15; i++){
		sign_locs[i].x = old.sign_locs[i].x;
		sign_locs[i].y = old.sign_locs[i].y;
	}
	lighting_type = (eLighting) old.lighting;
	in_town_rect.top = old.in_town_rect.top;
	in_town_rect.left = old.in_town_rect.left;
	in_town_rect.bottom = old.in_town_rect.bottom;
	in_town_rect.right = old.in_town_rect.right;
	for(i = 0; i < 64; i++){
//		preset_items[i].loc.x = old.preset_items[i].item_loc.x;
//		preset_items[i].loc.y = old.preset_items[i].item_loc.y;
//		preset_items[i].code = old.preset_items[i].item_code;
//		preset_items[i].ability = old.preset_items[i].ability;
//		preset_items[i].charges = old.preset_items[i].charges;
//		preset_items[i].always_there = old.preset_items[i].always_there;
//		preset_items[i].property = old.preset_items[i].property;
//		preset_items[i].contained = old.preset_items[i].contained;
		preset_items[i].append(old.preset_items[i]);
	}
	max_num_monst = old.max_num_monst;
	spec_on_entry = old.spec_on_entry;
	spec_on_entry_if_dead = old.spec_on_entry_if_dead;
	spec_on_hostile = -1;
	for(i = 0; i < 8; i++){
		timer_spec_times[i] = old.timer_spec_times[i];
		timer_specs[i] = old.timer_specs[i];
	}
	for(i = 0; i < 180; i++)
		strlens[i] = old.strlens[i];
	for(i = 0; i < 100; i++)
		specials[i].append(old.specials[i]);
	difficulty = old.difficulty;
	strong_barriers = defy_scrying = defy_mapping = false;
}

short max_dim[3] = {64,48,32};
cTown::cTown(cScenario& scenario, bool init_strings) : scenario(scenario) {
	short i,s;
	location d_loc(100,0);
	cTown::cWandering d_wan = {0,0,0,0};
	cTown::cItem null_item = {loc(),-1,0,0,0,0,0};
	
	town_chop_time = -1;
	town_chop_key = -1;
	for(i = 0; i < 4; i++) {
		wandering[i] = d_wan;
		wandering_locs[i] = d_loc;
	}
	for(i = 0; i < 50; i++) {
		special_locs[i] = d_loc;
		spec_id[i] = 0;
	}
	lighting_type = LIGHT_NORMAL;
	for(i = 0; i < 4; i++) {
		start_locs[i] = d_loc;
		exit_specs[i] = -1;
		exit_locs[i].x = -1;
		exit_locs[i].y = -1;
	}
//	s = town->max_dim();
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
	for(i = 0; i < 64; i++) 
		preset_items[i] = null_item;
	max_num_monst = 30000;
	spec_on_entry = -1;
	spec_on_entry_if_dead = -1;
	for(i = 0; i < 15; i++) {
		sign_locs[i] = d_loc;
		sign_locs[i].x = 100;
	}	
	for(i = 0; i < 8; i++) {
		timer_spec_times[i] = 0;
		timer_specs[i] = -1;
	}	
	for(i = 0; i < 100; i++) {
		specials[i] = cSpecial();
	}
	difficulty = 0;
	strong_barriers = defy_scrying = defy_mapping = false;
	for(i = 0; i < 60; i++) {
		talking.talk_nodes[i].personality = -1;
		talking.talk_nodes[i].type = 0;
		talking.talk_nodes[i].extras[0] = 0;
		talking.talk_nodes[i].extras[1] = 0;
		talking.talk_nodes[i].extras[2] = 0;
		talking.talk_nodes[i].extras[3] = -1;
		talking.talk_nodes[i].str1 = "";
		talking.talk_nodes[i].str2 = "";
		for(int j = 0; j < 4; j++) {
			talking.talk_nodes[i].link1[j] = 'x';
			talking.talk_nodes[i].link2[j] = 'x';
		}
	}
	if(!init_strings) return;
	std::string temp_str;
	for(i = 0; i < 180; i++) {
		temp_str = get_str("town-default",i + 1);
		if(i == 0) town_name = temp_str;
		else if(i >= 1 && i < 17)
			rect_names[i-1] = temp_str;
		else if(i >= 17 && i < 20)
			comment[i-17] = temp_str;
		else if(i >= 20 && i < 120)
			spec_strs[i-20] = temp_str;
		else if(i >= 120 && i < 140)
			sign_strs[i-120] = temp_str;
		strlens[i] = temp_str.length();
	}
	
	for(i = 0; i < 200; i++)
		talking.strlens[i] = 0;
	for(i = 0; i < 10; i++) {
		talking.people[i].title = "Unused";
		talking.people[i].look = "";
		talking.people[i].name = "";
		talking.people[i].job = "";
		talking.people[i].dunno = "";
	}
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
	ability = old.ability;
	charges = old.charges;
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
			rad = scenario.ter_types[this->terrain(i,j)].light_radius;
			if(rad > 0) {
				for(where.x = std::max(0,i - rad); where.x < std::min(this->max_dim(),short(i + rad + 1)); where.x++)
					for(where.y = std::max(0,j - rad); where.y < std::min(this->max_dim(),short(j + rad + 1)); where.y++)
						if((where_lit[where.x][where.y] == 0) && (dist(where,l) <= rad) && (can_see(l,where,get_obscurity) < 5))
							where_lit[where.x][where.y] = 1;
			}
		}
	for(short i = 0; i < 8; i++)
		for(short j = 0; j < 64; j++)
			this->lighting(i,j) = 0;
	for(where.x = 0; where.x < this->max_dim(); where.x++)
		for(where.y = 0; where.y < this->max_dim(); where.y++) {
			if(where_lit[where.x][where.y] > 0) {
				this->lighting(where.x / 8,where.y) = this->lighting(where.x / 8,where.y) | (1 << (where.x % 8));
			}
		}
}

short cTown::light_obscurity(short x,short y) {
	ter_num_t what_terrain;
	eTerObstruct store;
	
	what_terrain = this->terrain(x,y);
	
	store = scenario.ter_types[what_terrain].blockage;
	if(store == eTerObstruct::BLOCK_SIGHT || store == eTerObstruct::BLOCK_MOVE_AND_SIGHT)
		return 5;
	if(store == eTerObstruct::BLOCK_MOVE_AND_SHOOT)
		return 1;
	return 0;
}
