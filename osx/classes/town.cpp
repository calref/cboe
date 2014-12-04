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

void cTown::append(legacy::big_tr_type&){}
void cTown::append(legacy::ave_tr_type&){}
void cTown::append(legacy::tiny_tr_type&){}

cTown& cTown::operator = (legacy::town_record_type& old){
	int i;
	town_chop_time = old.town_chop_time;
	town_chop_key = old.town_chop_key;
	for(i = 0; i < 4; i++){
		start_locs[i].x = old.start_locs[i].x;
		start_locs[i].y = old.start_locs[i].y;
		exit_locs[i].x = old.exit_locs[i].x;
		exit_locs[i].y = old.exit_locs[i].y;
		exit_specs[i] = old.exit_specs[i];
		wandering[i] = old.wandering[i];
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
		temp = old.preset_fields[i];
		preset_fields.push_back(temp);
	}
	for(i = 0; i < 15; i++){
		sign_locs[i].x = old.sign_locs[i].x;
		sign_locs[i].y = old.sign_locs[i].y;
	}
	lighting_type = old.lighting;
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
		preset_items[i] = old.preset_items[i];
	}
	max_num_monst = old.max_num_monst;
	spec_on_entry = old.spec_on_entry;
	spec_on_entry_if_dead = old.spec_on_entry_if_dead;
	for(i = 0; i < 8; i++){
		timer_spec_times[i] = old.timer_spec_times[i];
		timer_specs[i] = old.timer_specs[i];
	}
	for(i = 0; i < 180; i++)
		strlens[i] = old.strlens[i];
	for(i = 0; i < 100; i++)
		specials[i] = old.specials[i];
	difficulty = old.difficulty;
	strong_barriers = defy_scrying = defy_mapping = false;
	return *this;
}

cTown::cTown(){}

short max_dim[3] = {64,48,32};
cTown::cTown(short){
	short i,s;
	location d_loc(100,0);
	cTown::cWandering d_wan = {0,0,0,0};
	cTown::cItem null_item = {loc(),-1,0,0,0,0,0};
	
	town_chop_time = -1;
	town_chop_key = -1;
	for (i = 0; i < 4; i++) {
		wandering[i] = d_wan;
		wandering_locs[i] = d_loc;
	}
	for (i = 0; i < 50; i++) {
		special_locs[i] = d_loc;
		spec_id[i] = 0;
	}
	lighting_type = 0;
	for (i = 0; i < 4; i++) {
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
	for (i = 0; i < 64; i++) 
		preset_items[i] = null_item;
	max_num_monst = 30000;
//	for (i = 0; i < 50; i++) 
//		preset_fields[i].type = 0;
	spec_on_entry = -1;
	spec_on_entry_if_dead = -1;
	for (i = 0; i < 15; i++) {
		sign_locs[i] = d_loc;
		sign_locs[i].x = 100;
	}	
	for (i = 0; i < 8; i++) {
		timer_spec_times[i] = 0;
		timer_specs[i] = -1;
	}	
	for (i = 0; i < 100; i++) {
		specials[i] = cSpecial();
	}
	difficulty = 0;
	strong_barriers = defy_scrying = defy_mapping = false;
}

char(& cTown::town_strs(short i))[256]{
	if(i == 0) return town_name;
	if(i >= 1   && i < 17 ) return rect_names[i - 1];
	if(i >= 17  && i < 20 ) return comment[i - 17];
	if(i >= 20  && i < 120) return spec_strs[i - 20];
	if(i >= 120 && i < 140) return sign_strs[i - 120];
	return comment[1]; // random unused string
}

cTown::cWandering& cTown::cWandering::operator = (legacy::wandering_type old){
	monst[0] = old.monst[0];
	monst[1] = old.monst[1];
	monst[2] = old.monst[2];
	monst[3] = old.monst[3];
	return *this;
}

cTown::cItem& cTown::cItem::operator = (legacy::preset_item_type old){
	loc.x = old.item_loc.x;
	loc.y = old.item_loc.y;
	code = old.item_code;
	ability = old.ability;
	charges = old.charges;
	always_there = old.always_there;
	property = old.property;
	contained = old.contained;
	return *this;
}

cTown::cField& cTown::cField::operator = (legacy::preset_field_type old){
	loc.x = old.field_loc.x;
	loc.y = old.field_loc.y;
	type = old.field_type;
	return *this;
}

bool cTown::cWandering::isNull(){
	for (short i = 0;i < 4;i++)
		if (monst[i] != 0)
			return false;
	return true;
}
