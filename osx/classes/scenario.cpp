/*
 *  scenario.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>


#include "classes.h"
#include "oldstructs.h"

cScenario::cItemStorage::cItemStorage() : ter_type(-1), property(0) {
	for(int i = 0; i < 10; i++)
		item_num[i] = -1;
	for(int i = 0; i < 10; i++)
		item_odds[i] = 0;
}

//{-1,{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,0,0,0,0,0,0,0,0,0},0}

cScenario& cScenario::operator = (legacy::scenario_data_type& old){
	int i,j;
//	for(i = 0; i < 3; i++) format.ver[i] = old.ver[i];
//	format.min_run_ver = old.min_run_ver;
//	for(i = 0; i < 3; i++) format.prog_make_ver[i] = old.prog_make_ver[i];
	num_towns = format.num_towns;
	out_width = old.out_width;
	out_height = old.out_height;
	difficulty = old.difficulty;
	intro_pic = old.intro_pic;
	default_ground = old.default_ground;
	// TODO: One or both of these should probably be moved to cTown
	for(i = 0; i < 200; i++) town_size[i] = old.town_size[i];
	for(i = 0; i < 200; i++) town_hidden[i] = old.town_hidden[i];
	flag_a = old.flag_a;
	intro_mess_pic = old.intro_mess_pic;
	intro_mess_len = old.intro_mess_len;
	where_start.x = old.where_start.x;
	where_start.y = old.where_start.y;
	out_sec_start.x = old.out_sec_start.x;
	out_sec_start.y = old.out_sec_start.y;
	out_start.x = old.out_start.x;
	out_start.y = old.out_start.y;
	which_town_start = old.which_town_start;
	flag_b = old.flag_b;
	// TODO: Town data size doesn't even need to be stored at all; its only purpose is for locating the offset of a town within the old scenario format; same with out data size
	for(i = 0; i < 200; i++)
		for(j = 0; j < 5; j++)
			town_data_size[i][j] = old.town_data_size[i][j];
	for(i = 0; i < 10; i++) town_to_add_to[i] = old.town_to_add_to[i];
	for(i = 0; i < 10; i++)
		for(j = 0; j < 2; j++)
			flag_to_add_to_town[i][j] = old.flag_to_add_to_town[i][j];
	flag_c = old.flag_c;
	for(i = 0; i < 100; i++){
		out_data_size[i][0] = old.out_data_size[i][0];
		out_data_size[i][1] = old.out_data_size[i][1];
	}
	// TODO: Combine store_item_rects and store_item_towns into a structure
	for(i = 0; i < 3; i++) {
		store_item_rects[i].top = old.store_item_rects[i].top;
		store_item_rects[i].left = old.store_item_rects[i].left;
		store_item_rects[i].bottom = old.store_item_rects[i].bottom;
		store_item_rects[i].right = old.store_item_rects[i].right;
	}
	for(i = 0; i < 3; i++) store_item_towns[i] = old.store_item_towns[i];
	flag_e = old.flag_e;
	// TODO: Combine these two into a structure
	for(i = 0; i < 50; i++) special_items[i] = old.special_items[i];
	for(i = 0; i < 50; i++) special_item_special[i] = old.special_item_special[i];
	rating = old.rating;
	// TODO: Is this used anywhere?
	uses_custom_graphics = old.uses_custom_graphics;
	flag_f = old.flag_f;
	for(i = 0; i < 256; i++) scen_monsters[i] = old.scen_monsters[i];
	for(i = 0; i < 30; i++) boats[i] = old.scen_boats[i];
	for(i = 0; i < 30; i++) horses[i] = old.scen_horses[i];
	flag_g = old.flag_g;
	for(i = 0; i < 256; i++){
		ter_types[i].i = i;
		ter_types[i] = old.ter_types[i];
	}
	for(i = 0; i < 20; i++) scenario_timer_times[i] = old.scenario_timer_times[i];
	for(i = 0; i < 20; i++) scenario_timer_specs[i] = old.scenario_timer_specs[i];
	flag_h = old.flag_h;
	for(i = 0; i < 256; i++) scen_specials[i] = old.scen_specials[i];
	for(i = 0; i < 10; i++) storage_shortcuts[i] = old.storage_shortcuts[i];
	flag_d = old.flag_d;
	for(i = 0; i < 300; i++) scen_str_len[i] = old.scen_str_len[i];
	flag_i = old.flag_i;
	last_out_edited.x = old.last_out_edited.x;
	last_out_edited.y = old.last_out_edited.y;
	last_town_edited = old.last_town_edited;
	adjust_diff = true;
	return *this;
}

cScenario::cItemStorage& cScenario::cItemStorage::operator = (legacy::item_storage_shortcut_type& old){
	ter_type = old.ter_type;
	for(int i = 0; i < 10; i++) item_num[i] = old.item_num[i];
	for(int i = 0; i < 10; i++) item_odds[i] = old.item_odds[i];
	property = old.property;
	return *this;
}

void cScenario::append(legacy::scen_item_data_type& old){
	short i;
	for(i = 0; i < 400; i++)
		scen_items[i] = old.scen_items[i];
	for(i = 0; i < 256; i++)
		scen_monsters[i].m_name = old.monst_names[i];
	for(i = 0; i < 256; i++)
		ter_types[i].name = old.ter_names[i];
}

char(& cScenario::scen_strs(short i))[256]{
	if(i == 0) return scen_name;
	if(i == 1) return who_wrote[0];
	if(i == 2) return who_wrote[1];
	if(i == 3) return contact_info;
	if(i >= 4   && i < 10 ) return intro_strs[i - 4];
	if(i >= 10  && i < 60 ) return journal_strs[i - 10];
	if(i >= 60  && i < 160){
		if(i % 2 == 0) return spec_item_names[(i - 60) / 2];
		else return spec_item_strs[(i - 60) / 2];
	}
	if(i >= 160 && i < 260) return spec_strs[i - 160];
	return journal_strs[6]; // random unused slot
}
