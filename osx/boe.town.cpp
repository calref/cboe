
#include <cstdio>
#include <queue>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"

#include "boe.graphutil.h"
#include "boe.graphics.h"
#include "boe.newgraph.h"
#include "boe.fileio.h"
#include "boe.items.h"
#include "boe.itemdata.h"
#include "boe.monster.h"
#include "boe.town.h"
#include "boe.combat.h"
#include "boe.party.h"
#include "boe.text.h"
#include "soundtool.h"
#include "boe.locutils.h"
#include "boe.specials.h"
#include "boe.infodlg.h"
#include "mathutil.h"
#include "boe.main.h"
#include "graphtool.h"
#include "dlogutil.h"
#include "fileio.h"
#include "winutil.h"

//extern current_town_type univ.town;
//extern party_record_type	party;
//extern town_item_list univ.town;
extern short stat_window,store_spell_target,which_combat_type,current_pc,combat_active_pc;
extern eGameMode overall_mode;
extern location center;
extern sf::RenderWindow mainPtr;
//extern short monst_target[60]; // 0-5 target that pc   6 - no target  100 + x - target monster x
//extern cOutdoors univ.out.outdoors[2][2];
//extern unsigned char univ.out.misc_i[64][64];
extern short store_current_pc,current_ground;
//extern pascal bool cd_event_filter();
extern eGameMode store_pre_shop_mode,store_pre_talk_mode;
//extern location monster_targs[60];
extern std::queue<pending_special_type> special_queue;

extern bool map_visible,diff_depth_ok;
extern sf::RenderWindow mini_map;
//extern unsigned char univ.out[96][96],univ.out.out_e[96][96],univ.out.sfx[64][64];
//extern stored_items_list_type stored_items[3];
//extern stored_town_maps_type maps;
//extern stored_outdoor_maps_type o_maps;
//extern big_tr_type t_d;
//extern short town_size[3];
//extern setup_save_type setup_save;
//extern stored_town_maps_type town_maps;

extern location hor_vert_place[14];
extern location diag_place[14];
extern location golem_m_locs[16];
extern cScenario scenario;
extern cUniverse univ;
//extern piles_of_stuff_dumping_type *data_store;
extern sf::Texture bg_gworld;
extern cCustomGraphics spec_scen_g;
bool need_map_full_refresh = true,forcing_map_button_redraw = false;
extern sf::RenderTexture map_gworld;
extern sf::Texture small_ter_gworld;
// In the 0..65535 range, this colour was {65535,65535,52428}
sf::Color parchment = {255,255,205};
extern tessel_ref_t map_pat[];

// TODO: Add blue cave graphics to these arrays?
unsigned char map_pats[256] = {
	1,1,2,2,2,7,7,7,7,7, ////
	7,7,7,7,7,7,7,7,3,3,
	3,3,3,3,3,3,3,3,3,3,
	3,3,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,4,4,4,4,
	4,4,4,4,4,4,4,4,4,0, // 50
	0,0,0,0,0,0,0,24,24,24,
	16,16,25,25,0,0,0,0,18,8,
	8,9,2,15,15,10,10,10,6,0,
	0,0,0,0,0,0,0,0,0,0,
	19,0,0,0,0,0,0,0,0,0, // 100
	23,0,0,0,0,0,0,0,0,0,
	0,0,0,11,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,12,0,0, // 150
	0,0,0,13,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,18,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, // 200
	0,0,0,0,0,17,17,0,17,17,
	17,17,17,17,17,17,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0};// 250
unsigned char anim_map_pats[18] = {14,0,0,0,22, 0,0,21,20,21, 20,0,0,0,0, 0,0,0};

long pause_dummy;

location town_map_adj;
short town_force = 200,store_which_shop,store_min,store_max,store_shop,store_selling_pc;
short sell_offset = 0;
location town_force_loc;
bool shop_button_active[12];
RECT map_title_rect = {3,50,15,300};
RECT map_bar_rect = {15,50,27,300};
unsigned char map_graphic_placed[8][64]; // keeps track of what's been filled on map

void force_town_enter(short which_town,location where_start) {
	town_force = which_town;
	town_force_loc = where_start;
}

////
//short entry_dir; // if 9, go to forced
void start_town_mode(short which_town, short entry_dir) {
	short i,m,n;
	char message[60];
	short j,k,town_number;
	short at_which_save_slot,former_town;
	bool monsters_loaded = false,town_toast = false;
	location loc;
	unsigned short temp;
	//bool play_town_sound = false;
	
	if(town_force < 200)
		which_town = town_force;
//	else if(PSD[304][9] == 0) {
//		play_town_sound = true;
//	}
	
	former_town = town_number = which_town;
	
	if((town_number < 0) || (town_number >= scenario.num_towns)) {
		giveError("The scenario tried to put you into a town that doesn't exist.",
			"Requested town: " + std::to_string(town_number) + "|Max town: " + std::to_string(scenario.num_towns));
		return;
	}
	
	// Now adjust town number as necessary.
	for(i = 0; i < 10; i++)
		if((scenario.town_to_add_to[i] >= 0) && (scenario.town_to_add_to[i] < 200) &&
			(town_number == scenario.town_to_add_to[i]) &&
			(sd_legit(scenario.flag_to_add_to_town[i][0],scenario.flag_to_add_to_town[i][1]))) {
			former_town = town_number;
			town_number += PSD[scenario.flag_to_add_to_town[i][0]][scenario.flag_to_add_to_town[i][1]];
			// Now update horses & boats
			for(i = 0; i < 30; i++)
				if((univ.party.horses[i].exists) && (univ.party.horses[i].which_town == former_town))
					univ.party.horses[i].which_town = town_number;
			for(i = 0; i < 30; i++)
				if((univ.party.boats[i].exists) && (univ.party.boats[i].which_town == former_town))
					univ.party.boats[i].which_town = town_number;
		}
	
	
	
	if((town_number < 0) || (town_number >= scenario.num_towns)) {
		giveError("The scenario tried to put you into a town that doesn't exist.",
			"Requested town: " + std::to_string(former_town) + "|Adjusted town: " + std::to_string(town_number) + "|Max town: " + std::to_string(scenario.num_towns));
		return;
	}
	
	overall_mode = MODE_TOWN;
	
	
	load_town(town_number,univ.town.record);
	
	univ.town.num = town_number;
	load_town_talk(town_number);
	init_town();
	
//	if(play_town_sound) {
	if(univ.town->lighting_type > 0)
		play_sound(95);
	else play_sound(16);
	
//	}
	
	
	
	univ.town.belt_present = false;
	// Set up map, using stored map
	for(i = 0; i < univ.town->max_dim(); i++)
		for(j = 0; j < univ.town->max_dim(); j++) {
			//univ.town.fields[i][j] = 0;
			//univ.out.sfx[i][j] = 0;
			if(univ.town_maps[univ.town.num][i / 8][j] & (char)(s_pow(2,i % 8)))
				make_explored(i,j);
			
			if(univ.town->terrain(i,j) == 0)
				current_ground = 0;
			else if(univ.town->terrain(i,j) == 2)
				current_ground = 2;
			if(scenario.ter_types[univ.town->terrain(i,j)].special == eTerSpec::CONVEYOR)
				univ.town.belt_present = true;
		}
	
	univ.town.hostile = 0;
	univ.town.monst.which_town = town_number;
	univ.town.monst.friendly = 0;
	
	at_which_save_slot = univ.party.at_which_save_slot;
	
	for(i = 0; i < 4; i++)
		if(town_number == univ.party.creature_save[i].which_town) {
			univ.town.monst = univ.party.creature_save[i];
			monsters_loaded = true;
			
			for(j = 0; j < univ.town->max_monst(); j++) {
				if(loc_off_act_area(univ.town.monst[j].cur_loc))
					univ.town.monst[j].active = 0;
				if(univ.town.monst[j].active == 2)
					univ.town.monst[j].active = 1;
				univ.town.monst[j].cur_loc = univ.town->creatures(j).start_loc;
				univ.town.monst[j].health = univ.town.monst[j].m_health;
				univ.town.monst[j].mp = univ.town.monst[j].max_mp;
				univ.town.monst[j].morale = univ.town.monst[j].m_morale;
				univ.town.monst[j].status.clear();
				if(univ.town.monst[j].summoned > 0)
					univ.town.monst[j].active = 0;
				univ.town.monst[j].target = 6;
			}
			
			// Now, travelling NPCs might have arrived. Go through and put them in.
			// These should have protected status (i.e. spec1 >= 200, spec1 <= 204)
			for(j = 0; j < univ.town->max_monst(); j++) {
				switch(univ.town.monst[j].time_flag){
					case 4: case 5 : //case 6:
						if((((short) (univ.party.age / 1000) % 3) + 4) != univ.town.monst[j].time_flag)
							univ.town.monst[j].active = 0;
						else {
							univ.town.monst[j].active = 1;
							univ.town.monst[j].spec_enc_code = 0;
							// Now remove time flag so it doesn't get reappearing
							univ.town.monst[j].time_flag = 0;
							univ.town.monst[j].cur_loc = univ.town->creatures(j).start_loc;
							univ.town.monst[j].health = univ.town.monst[j].m_health;
						}
						break ;
						
						// Now, appearing/disappearing monsters might have arrived/disappeared.
					case 1:
						if(day_reached(univ.town.monst[j].monster_time, univ.town.monst[j].time_code)) {
							univ.town.monst[j].active = 1;
							univ.town.monst[j].time_flag=0; // Now remove time flag so it doesn't get reappearing
						}
						break;
					case 2:
						if(day_reached(univ.town.monst[j].monster_time, univ.town.monst[j].time_code)) {
							univ.town.monst[j].active = 0;
							univ.town.monst[j].time_flag=0; // Now remove time flag so it doesn't get disappearing again
						}
						break;
					case 7:
						if(calc_day() >= univ.party.key_times[univ.town.monst[j].time_code]){ //calc_day is used because of the "definition" of univ.party.key_times
							univ.town.monst[j].active = 1;
							univ.town.monst[j].time_flag=0; // Now remove time flag so it doesn't get reappearing
						}
						break;
						
					case 8:
						if(calc_day() >= univ.party.key_times[univ.town.monst[j].time_code]){
							univ.town.monst[j].active = 0;
							univ.town.monst[j].time_flag=0; // Now remove time flag so it doesn't get disappearing again
						}
						break;
				}
			}
			
			for(j = 0; j < univ.town->max_dim(); j++)
				for(k = 0; k < univ.town->max_dim(); k++) { // now load in saved setup,
					// except that pushable things restore to orig locs
					temp = univ.party.setup[i][j][k] << 8;
					temp &= OBJECT_CRATE | OBJECT_BARREL | OBJECT_BLOCK;
					univ.town.fields[j][k] &= ~(OBJECT_CRATE | OBJECT_BARREL | OBJECT_BLOCK);
					univ.town.fields[j][k] |= temp;
				}
		}
	
	if(!monsters_loaded) {
		for(i = 0; i < univ.town->max_monst(); i++){
			if(univ.town->creatures(i).number == 0) {
				univ.town.monst[i].active = 0;
				univ.town.monst[i].number = 0;
				univ.town.monst[i].time_flag = 0;
				univ.town.monst[i].cur_loc.x = 80;
			}
			else {
				// First set up the values.
				univ.town.monst[i] = univ.town->creatures(i);
				univ.town.monst[i].target = 6;
				univ.town.monst[i].active = 1; // TODO: Can those two \/ be commented out?
				//univ.town.monst[i].attitude = univ.town->creatures(i).start_attitude;
				//univ.town.monst[i].cur_loc = univ.town->creatures(i).start_loc;
				//univ.town.monst[i].mobility = univ.town->creatures(i).mobility;
				univ.town.monst[i].summoned = 0;
				
				if(univ.town.monst[i].spec_enc_code > 0)
					univ.town.monst[i].active = 0;
				
				// Now, if necessary, fry the monster.
				switch(univ.town.monst[i].time_flag) {
					case 1:
						if(!day_reached(univ.town.monst[i].monster_time,
										 univ.town.monst[i].time_code))
							univ.town.monst[i].active = 0;
						break;
					case 2:
						if(day_reached(univ.town.monst[i].monster_time,
										univ.town.monst[i].time_code))
							univ.town.monst[i].active = 0;
						break;
					case 3:
						// unused
						break;
					case 4: case 5: case 6:
						if((((short) (univ.party.age / 1000) % 3) + 4) != univ.town.monst[i].time_flag) {
							univ.town.monst[i].active = 0;
							univ.town.monst[i].spec_enc_code = 50;
						}
						else {
							univ.town.monst[i].active = 1;
							// Now remove time flag so it doesn't keep reappearing
							univ.town.monst[i].time_flag = 0;
						}
						break;
					case 7:
						if(calc_day() < univ.party.key_times[univ.town.monst[i].time_code])
							univ.town.monst[i].active = 0;
						break;
						
					case 8:
						if(calc_day() >= univ.party.key_times[univ.town.monst[i].time_code])
							univ.town.monst[i].active = 0;
						break;
					case 9:
						if(univ.town->town_chop_time > 0)
							if(day_reached(univ.town->town_chop_time,univ.town->town_chop_key)) {
								univ.town.monst[i].active += 10;
								break;
							}
						univ.town.monst[i].active = 0;
						break;
					case 0:
						break;
					default:
						ASB("ERROR! Odd character data.");
						print_nums(0,i,univ.town.monst[i].time_flag);
						break;
				}
			}
		}
	}
	
	// Now munch all large monsters that are misplaced
	// only large monsters, as some smaller monsters are intentionally placed
	// where they cannot be
	for(i = 0; i < univ.town->max_monst(); i++) {
		if(univ.town.monst[i].active > 0)
			if(((univ.town.monst[i].x_width > 1) || (univ.town.monst[i].y_width > 1)) &&
				!monst_can_be_there(univ.town.monst[i].cur_loc,i))
				univ.town.monst[i].active = 0;
	}
	
	
	// Thrash town?
	if(univ.party.m_killed[univ.town.num] > univ.town->max_num_monst) {
		town_toast = true;
		add_string_to_buf("Area has been cleaned out.");
	}
	if(univ.town->town_chop_time > 0) {
		if(day_reached(univ.town->town_chop_time,univ.town->town_chop_key)) {
			add_string_to_buf("Area has been abandoned.");
			for(i = 0; i < univ.town->max_monst(); i++)
				if((univ.town.monst[i].active > 0) && (univ.town.monst[i].active < 10) &&
					(univ.town.monst[i].attitude % 2 == 1))
					univ.town.monst[i].active += 10;
			town_toast = true;
		}
	}
	if(town_toast) {
		for(i = 0; i < univ.town->max_monst(); i++)
			if(univ.town.monst[i].active >= 10)
				univ.town.monst[i].active -= 10;
			else univ.town.monst[i].active = 0;
	}
	handle_town_specials(town_number, (short) town_toast,(entry_dir < 9) ? univ.town->start_locs[entry_dir] : town_force_loc);
	// TODO: Flush the special node queue on scenario start so that the special actually gets called.
	
	// Flush excess doomguards and viscous goos
	for(i = 0; i < univ.town->max_monst(); i++)
		if((univ.town.monst[i].spec_skill == 12) &&
			(univ.town.monst[i].number != univ.town->creatures(i).number))
			univ.town.monst[i].active = 0;
	
	// Set up field booleans, correct for doors
	for(j = 0; j < univ.town->max_dim(); j++)
		for(k = 0; k < univ.town->max_dim(); k++) {
			loc.x = j; loc.y = k;
			if(is_door(loc)) {
				univ.town.set_web(j,k,false);
				univ.town.set_crate(j,k,false);
				univ.town.set_barrel(j,k,false);
				univ.town.set_fire_barr(j,k,false);
				univ.town.set_force_barr(j,k,false);
				univ.town.set_quickfire(j,k,false);
				//univ.out.misc_i[j][k] = univ.out.misc_i[j][k] & 3;
			}
			if(univ.town.is_quickfire(j,k))
				univ.town.quickfire_present = true;
		}
	
	// Set up items, maybe place items already there
	for(i = 0; i < NUM_TOWN_ITEMS; i++)
		univ.town.items[i] = cItemRec();
	
	for(j = 0; j < 3; j++)
		if(scenario.store_item_towns[j] == town_number) {
			for(i = 0; i < NUM_TOWN_ITEMS; i++)
				univ.town.items[i] = univ.party.stored_items[j][i];
		}
	
	for(i = 0; i < 64; i++)
		if((univ.town->preset_items[i].code >= 0)
			&& (((univ.party.item_taken[univ.town.num][i / 8] & s_pow(2,i % 8)) == 0) ||
				(univ.town->preset_items[i].always_there))) {
				for(j = 0; j < NUM_TOWN_ITEMS; j++)
					
					// place the preset item, if party hasn't gotten it already
					if(univ.town.items[j].variety == eItemType::NO_ITEM) {
						univ.town.items[j] = get_stored_item(univ.town->preset_items[i].code);
						univ.town.items[j].item_loc = univ.town->preset_items[i].loc;
						
						// Not use the items data flags, starting with forcing an ability
						if(univ.town->preset_items[i].ability >= 0) {
							switch(univ.town.items[j].variety) {
								case eItemType::GOLD:
								case eItemType::FOOD: // If gold or food, this value is amount
									if(univ.town->preset_items[i].ability > 0)
										univ.town.items[j].item_level = univ.town->preset_items[i].ability;
									break;
								default: //leave other type alone
									// TODO: Could resupport this "forcing an ability" thing...
									break;
		 					}
		 				}
		 				
						if(town_toast)
							univ.town.items[j].property = false;
						else
							univ.town.items[j].property = univ.town->preset_items[i].property;
						univ.town.items[j].contained = univ.town->preset_items[i].contained;
						univ.town.items[j].is_special = i + 1;
						
						j = NUM_TOWN_ITEMS;
					}
		 	}
	
	
	for(i = 0; i < univ.town->max_monst(); i++)
		if(loc_off_act_area(univ.town.monst[i].cur_loc))
			univ.town.monst[i].active = 0;
	for(i = 0; i < NUM_TOWN_ITEMS; i++)
		if(loc_off_act_area(univ.town.items[i].item_loc))
			univ.town.items[i].variety = eItemType::NO_ITEM;
	
	// Clean out unwanted monsters
	for(i = 0; i < univ.town->max_monst(); i++)
		if(sd_legit(univ.town.monst[i].spec1,univ.town.monst[i].spec2)) {
			if(PSD[univ.town.monst[i].spec1][univ.town.monst[i].spec2] > 0)
				univ.town.monst[i].active = 0;
		}
	
	erase_specials();
//	make_town_trim(0);
	
	
	univ.town.p_loc = (entry_dir < 9) ? univ.town->start_locs[entry_dir] : town_force_loc;
	center = univ.town.p_loc;
	if(univ.party.in_boat >= 0) {
		univ.party.boats[univ.party.in_boat].which_town = which_town;
		univ.party.boats[univ.party.in_boat].loc = univ.town.p_loc;
	}
	if(univ.party.in_horse >= 0) {
		univ.party.horses[univ.party.in_horse].which_town = which_town;
		univ.party.horses[univ.party.in_horse].loc = univ.town.p_loc;
	}
	
	
	// End flying
	if(PSD[SDF_PARTY_FLIGHT] > 0) {
		PSD[SDF_PARTY_FLIGHT] = 0;
		add_string_to_buf("You land, and enter.             ");
	}
	
	// No hostile monsters present.
	PSD[SDF_HOSTILES_PRESENT] = 0;
	
	add_string_to_buf("Now entering:");
	sprintf(message, "   %-30.30s ",univ.town->town_name.c_str());
	add_string_to_buf(message);
	
	
	// clear entry space, and check exploration
	univ.town.set_web(univ.town.p_loc.x,univ.town.p_loc.y,false);
	univ.town.set_crate(univ.town.p_loc.x,univ.town.p_loc.y,false);
	univ.town.set_barrel(univ.town.p_loc.x,univ.town.p_loc.y,false);
	univ.town.set_fire_barr(univ.town.p_loc.x,univ.town.p_loc.y,false);
	univ.town.set_force_barr(univ.town.p_loc.x,univ.town.p_loc.y,false);
	univ.town.set_quickfire(univ.town.p_loc.x,univ.town.p_loc.y,false);
	//univ.out.misc_i[univ.town.p_loc.x][univ.town.p_loc.y] = univ.out.misc_i[univ.town.p_loc.x][univ.town.p_loc.y] & 3;
	update_explored(univ.town.p_loc);
	
	// If a PC dead, drop his items
	for(m = 0; m < 6; m++)
		for(n = 0; n < 24; n++)
			if(univ.party[m].main_status != eMainStatus::ALIVE && univ.party[m].items[n].variety != eItemType::NO_ITEM) {
				place_item(univ.party[m].items[n],univ.town.p_loc,true);
				univ.party[m].items[n].variety = eItemType::NO_ITEM;
			}
	
	for(i = 0; i < univ.town->max_monst(); i++) {
		univ.town.monst[i].targ_loc.x = 0;
		univ.town.monst[i].targ_loc.y = 0;
	}
	
	//// check horses
	for(i = 0; i < 30; i++) {
		if((scenario.boats[i].which_town >= 0) && (scenario.boats[i].loc.x >= 0)) {
			if(!univ.party.boats[i].exists) {
				univ.party.boats[i] = scenario.boats[i];
				univ.party.boats[i].exists = true;
			}
		}
		if((scenario.horses[i].which_town >= 0) && (scenario.horses[i].loc.x >= 0)) {
			if(!univ.party.horses[i].exists) {
				univ.party.horses[i] = scenario.horses[i];
				univ.party.horses[i].exists = true;
			}
		}
	}
	
	clear_map();
	reset_item_max();
	town_force = 200;
	// TODO: One problem with this - it paints the terrain after the town entry dialog is dismissed
	// ... except it actually doesn't, because the town enter special is only queued, not run immediately.
	draw_terrain(1);
}


location end_town_mode(short switching_level,location destination) { // returns new party location
	location to_return;
	bool data_saved = false,combat_end = false;
	short i,j,k;
	
	if(is_combat())
		combat_end = true;
	
	if(overall_mode == MODE_TOWN) {
		for(i = 0; i < 4; i++)
			if(univ.party.creature_save[i].which_town == univ.town.num) {
				univ.party.creature_save[i] = univ.town.monst;
				for(j = 0; j < univ.town->max_dim(); j++)
					for(k = 0; k < univ.town->max_dim(); k++)
						univ.party.setup[i][j][k] = (univ.town.fields[j][k] & 0xff00) >> 8;
				data_saved = true;
			}
		if(!data_saved) {
			univ.party.creature_save[univ.party.at_which_save_slot] = univ.town.monst;
			for(j = 0; j < univ.town->max_dim(); j++)
				for(k = 0; k < univ.town->max_dim(); k++)
					univ.party.setup[univ.party.at_which_save_slot][j][k] = (univ.town.fields[j][k] & 0xff00) >> 8;
			univ.party.at_which_save_slot = (univ.party.at_which_save_slot == 3) ? 0 : univ.party.at_which_save_slot + 1;
		}
		
		// Store items, if necessary
		for(j = 0; j < 3; j++)
			if(scenario.store_item_towns[j] == univ.town.num) {
				for(i = 0; i < NUM_TOWN_ITEMS; i++)
					if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].is_special == 0 &&
							((univ.town.items[i].item_loc.x >= scenario.store_item_rects[j].left) &&
							(univ.town.items[i].item_loc.x <= scenario.store_item_rects[j].right) &&
							(univ.town.items[i].item_loc.y >= scenario.store_item_rects[j].top) &&
							(univ.town.items[i].item_loc.y <= scenario.store_item_rects[j].bottom)) ) {
						univ.party.stored_items[j][i] = univ.town.items[i];
					}
				 	else univ.party.stored_items[j][i].variety = eItemType::NO_ITEM;
			}
		
		
		// Clean up special data, just in case
		for(i = 0; i < univ.town->max_monst(); i++) {
			univ.town.monst[i].spec1 = 0;
			univ.town.monst[i].spec2 = 0;
		}
		
		// Now store map
		//if(univ.town.town_num < 120) {
		for(i = 0; i < univ.town->max_dim(); i++)
			for(j = 0; j < univ.town->max_dim(); j++)
				if(is_explored(i,j) > 0) {
					univ.town_maps[univ.town.num][i / 8][j] = univ.town_maps[univ.town.num][i / 8][j] |
					(char) (s_pow(2,i % 8));
				}
		//}
		
		to_return = univ.party.p_loc;
		
		for(i = univ.party.party_event_timers.size() - 1; i >= 0; i++)
	 		if(univ.party.party_event_timers[i].global_or_town == 1) {
				cParty::timerIter iter = univ.party.party_event_timers.begin();
				iter += i;
				univ.party.party_event_timers.erase(iter);
			}
		
	}
	
	
	// Check for exit specials, if leaving town
	if(switching_level == 0) {
		to_return = univ.party.p_loc;
		
		if(is_town()) {
			if(destination.x <= univ.town->in_town_rect.left) {
				if(univ.town->exit_locs[1].x > 0)
					to_return = local_to_global(univ.town->exit_locs[1]);
				else to_return.x--;
				univ.party.p_loc = to_return; univ.party.p_loc.x++;
				handle_leave_town_specials(univ.town.num, univ.town->exit_specs[1],destination) ;
			}
			else if(destination.x >= univ.town->in_town_rect.right) {
				if(univ.town->exit_locs[3].x > 0)
					to_return = local_to_global(univ.town->exit_locs[3]);
				else to_return.x++;
				univ.party.p_loc = to_return; univ.party.p_loc.x--;
				handle_leave_town_specials(univ.town.num, univ.town->exit_specs[3],destination) ;
			}
			else if(destination.y <= univ.town->in_town_rect.top) {
				if(univ.town->exit_locs[0].x > 0)
					to_return = local_to_global(univ.town->exit_locs[0]);
				else to_return.y--;
				univ.party.p_loc = to_return; univ.party.p_loc.y++;
				handle_leave_town_specials(univ.town.num, univ.town->exit_specs[0],destination) ;
			}
			else if(destination.y >= univ.town->in_town_rect.bottom) {
				if(univ.town->exit_locs[2].x > 0)
					to_return = local_to_global(univ.town->exit_locs[2]);
				else to_return.y++;
				univ.party.p_loc = to_return; univ.party.p_loc.y--;
				handle_leave_town_specials(univ.town.num, univ.town->exit_specs[2],destination) ;
			}
			
		}
	}
	
	if(switching_level == 0) {
		fix_boats();
		overall_mode = MODE_OUTDOORS;
		
		erase_out_specials();
		
		PSD[SDF_PARTY_STEALTHY] = 0;
		//PSD[SDF_PARTY_DETECT_LIFE] = 0; // TODO: Yes? No? Maybe?
		for(i = 0; i < 6; i++)
			erase_if(univ.party[i].status, [](std::pair<const eStatus, short> kv) -> bool {
				// TODO: These were the only statuses kept in the original code, but what about acid? Should it be kept too?
				if(kv.first == eStatus::POISON) return false;
				if(kv.first == eStatus::DISEASE) return false;
				if(kv.first == eStatus::DUMB) return false;
				return true;
			});
		
		
		update_explored(to_return);
		redraw_screen(REFRESH_TERRAIN | REFRESH_TEXT);
		
	}
	
	if(!combat_end)
		clear_map();
	
	univ.town.num = 200; // should be harmless...
	
	return to_return;
}

void handle_town_specials(short /*town_number*/, bool town_dead,location /*start_loc*/) {
	queue_special(eSpecCtx::ENTER_TOWN, 2, town_dead ? univ.town->spec_on_entry_if_dead : univ.town->spec_on_entry, univ.town.p_loc);
}

void handle_leave_town_specials(short /*town_number*/, short which_spec,location /*start_loc*/) {
	queue_special(eSpecCtx::LEAVE_TOWN, 2, which_spec, univ.party.p_loc);
}

bool abil_exists(eItemAbil abil) { // use when outdoors
	short i,j;
	
	for(i = 0; i < 6; i++)
		for(j = 0; j < 24; j++)
			if(univ.party[i].items[j].variety != eItemType::NO_ITEM && univ.party[i].items[j].ability == abil)
				return true;
	for(i = 0; i < 3; i++)
		for(j = 0; j < NUM_TOWN_ITEMS; j++)
			if(univ.party.stored_items[i][j].variety != eItemType::NO_ITEM && univ.party.stored_items[i][j].ability == abil)
				return true;
	
	return false;
}




void start_town_combat(short direction) {
	short i;
	
	place_party(direction);
	if(current_pc == 6)
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE) {
				current_pc = i;
				break;
			}
	center = univ.party[current_pc].combat_pos;
	
	which_combat_type = 1;
	overall_mode = MODE_COMBAT;
	
	combat_active_pc = 6;
	for(i = 0; i < univ.town->max_monst(); i++)
		univ.town.monst[i].target = 6;
	
	for(i = 0; i < 6; i++) {
		univ.party[i].last_attacked = univ.town->max_monst() + 10;
		univ.party[i].parry = 0;
		univ.party[i].dir = direction;
		univ.party[current_pc].direction = direction;
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			update_explored(univ.party[i].combat_pos);
	}
	
	store_current_pc = current_pc;
	current_pc = 0;
	set_pc_moves();
	pick_next_pc();
	center = univ.party[current_pc].combat_pos;
	draw_buttons(0);
	put_pc_screen();
	set_stat_window(current_pc);
	give_help(48,49);
	
}

short end_town_combat() {
	short num_tries = 0,r1,i;
	
	r1 = get_ran(1,0,5);
	while(univ.party[r1].main_status != eMainStatus::ALIVE && num_tries++ < 1000)
		r1 = get_ran(1,0,5);
	univ.town.p_loc = univ.party[r1].combat_pos;
	overall_mode = MODE_TOWN;
	current_pc = store_current_pc;
	if(univ.party[current_pc].main_status != eMainStatus::ALIVE)
		current_pc = first_active_pc();
	for(i = 0; i < 6; i++) {
		univ.party[i].parry = 0;
	}
	return univ.party[r1].dir;
}

void place_party(short direction) {
	bool spot_ok[14] = {true,true,true,true,true,true,true,
		true,true,true,true,true,true,true};
	location pos_locs[14];
	location check_loc;
	short x_adj,y_adj,how_many_ok = 1,where_in_a = 0,i;
	
	for(i = 0; i < 14; i++) {
		check_loc = univ.town.p_loc;
		if(direction % 4 < 2)
			x_adj = ((direction % 2 == 0) ? hor_vert_place[i].x : diag_place[i].x);
		else x_adj = ((direction % 2 == 0) ? hor_vert_place[i].y : diag_place[i].y);
		if(direction % 2 == 0)
			x_adj = (direction < 4) ? x_adj : -1 * x_adj;
		else x_adj = ((direction == 1) || (direction == 7)) ? -1 * x_adj : x_adj;
		check_loc.x -= x_adj;
		if(direction % 4 < 2)
			y_adj = ((direction % 2 == 0) ? hor_vert_place[i].y : diag_place[i].y);
		else y_adj = ((direction % 2 == 0) ? hor_vert_place[i].x : diag_place[i].x);
		if(direction % 2 == 0)
			y_adj = ((direction > 1) && (direction < 6)) ? y_adj : -1 * y_adj;
		else y_adj = ((direction == 3) || (direction == 1)) ? -1 * y_adj : y_adj;
		
		check_loc.y -= y_adj;
		pos_locs[i] = check_loc;
		if(!is_blocked(check_loc) && !is_special(check_loc) && sight_obscurity(check_loc.x,check_loc.y) == 0
		   && can_see_light(univ.town.p_loc,check_loc,combat_obscurity) < 1 && !loc_off_act_area(check_loc)) {
			spot_ok[i] = true;
			how_many_ok += (i > 1) ? 1 : 0;
		}
		else spot_ok[i] = false;
		
		if(i == 0)
			spot_ok[i] = true;
	}
	i = 0;
	while(i < 6) {
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
			if(how_many_ok == 1)
				univ.party[i].combat_pos = pos_locs[where_in_a];
			else {
				univ.party[i].combat_pos = pos_locs[where_in_a];
				if(how_many_ok > 1)
					where_in_a++;
				how_many_ok--;
//				if(how_many_ok > 1) {
				while(!spot_ok[where_in_a])
					where_in_a++;
//				}
			}
		}
		i++;
	}
}

// spec_code is encounter's spec_code
void create_out_combat_terrain(short type,short num_walls,short /*spec_code*/) {
	short i,j,k,r1,ter_type;
	static const short ter_base[16] = {
		2,0,36,50,71,0,0,0,
		0,2,2, 2, 2, 0,0,36
	};
	static const short ground_type[16] = {
		2,0,36,50,71,0,0,0,
		0,2,2, 2, 2, 0,0,36
	};
	static const location special_ter_locs[15] = {
		loc(11,10),loc(11,14),loc(10,20),loc(11,26),loc(9,30),
		loc(15,19),loc(23,19),loc(19,29),loc(20,11),loc(28,16),
		loc(28,24),loc(27,19),loc(27,29),loc(15,28),loc(19,19)
	};
	static const ter_num_t cave_pillar[4][4] = {
		{0 ,14,11,1 },
		{14,19,20,11},
		{17,18,21,8 },
		{1 ,17,8 ,0 }
	};
	static const ter_num_t mntn_pillar[4][4] = {
		{37,29,27,36},
		{29,33,34,27},
		{31,32,35,25},
		{36,31,25,37}
	};
	static const ter_num_t surf_lake[4][4] = {
		{56,55,54,3 },
		{57,50,61,54},
		{58,51,59,53},
		{3 ,4 ,58,52}
	};
	static const ter_num_t cave_lake[4][4] = {
		{93,96,71,71},
		{96,71,71,71},
		{71,71,71,96},
		{71,71,71,96}
	};
	static const ter_num_t surf_fume[4][4] = {
		{75,75,75,36},
		{75,75,75,75},
		{75,75,75,75},
		{36,37,75,75}
	};
	static const ter_num_t cave_fume[4][4] = {
		{98,0 ,75,75},
		{0 ,75,75,75},
		{75,75,75,0 },
		{75,75,75,0 }
	};
	static const short terrain_odds[16][10] = {
		{3,80,4,40,115,20,114,10,112,1},
		{1,50,93,25,94,5,98,10,95,1},
		{37,20,0,0,0,0,0,0,0,0},
		{64,3,63,1,0,0,0,0,0,0},
		{74,1,0,0,0,0,0,0,0,0},
		{84,700,97,30,98,20,92,4,95,1},
		{93,280,91,300,92,270,95,7,98,10},
		{1,800,93,600,94,10,92,10,95,4},
		{1,700,96,200,95,100,92,10,112,5},
		{3,600,87,90,110,20,114,6,113,2},
		{3,200,4,400,111,250,0,0,0,0},
		{3,200,4,300,112,50,113,60,114,100},
		{3,100,4,250,115,120,114,30,112,2},
		{1,25,76,15,98,300,97,280,75,5},
		{37,20,76,20,75,5,0,0,0,0}
	}; // ter then odds then ter then odds ...
	location stuff_ul;
	
//	ter_type = scenario.ter_types[type].picture;
//	if(ter_type > 260)
//		ter_type = 1;
//	else ter_type = general_types[ter_type];
	ter_type = scenario.ter_types[type].combat_arena;
	if(ter_type >= 1000) ter_type = 1; // TODO: load town ter_type - 1000 as the combat arena
	// TODO: Also implement the roads and crops arenas
	
	for(i = 0; i < 48; i++)
		for(j = 0; j < 48; j++) {
			univ.town.fields[i][j] = 0;
			//univ.out.misc_i[i][j] = 0;
			//univ.out.sfx[i][j] = 0;
			if((j <= 8) || (j >= 35) || (i <= 8) || (i >= 35))
				univ.town->terrain(i,j) = 90;
			else univ.town->terrain(i,j) = ter_base[ter_type];
		}
	for(i = 0; i < 48; i++)
		for(j = 0; j < 48; j++)
			for(k = 0; k < 5; k++)
				if((univ.town->terrain(i,j) != 90) && (get_ran(1,1,1000) < terrain_odds[ter_type][k * 2 + 1]))
					univ.town->terrain(i,j) = terrain_odds[ter_type][k * 2];
	
	univ.town->terrain(0,0) = ter_base[ter_type];
	
	if((ter_type == 3) || (ter_type == 4) ) {
		univ.town->terrain(0,0) = 83;
		for(i = 15; i < 26; i++)
			for(j = 9; j < 35; j++)
				univ.town->terrain(i,j) = 83;
	}
	if((type >= 79) && (type <= 81)) {
		univ.town->terrain(0,0) = 82;
		for(i = 19; i < 23; i++)
			for(j = 9; j < 35; j++)
				univ.town->terrain(i,j) = 82;
	}
	
	
	// Now place special lakes, etc.
	if(ter_type == 2)
		for(i = 0; i < 15; i++)
			if(get_ran(1,0,5) == 1) {
				stuff_ul = special_ter_locs[i];
				for(j = 0; j < 4; j++)
					for(k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = mntn_pillar[k][j];
			}
	if(univ.town->terrain(0,0) == 0)
		for(i = 0; i < 15; i++)
			if(get_ran(1,0,25) == 1) {
				stuff_ul = special_ter_locs[i];
				for(j = 0; j < 4; j++)
					for(k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = cave_pillar[k][j];
			}
	if(univ.town->terrain(0,0) == 0)
		for(i = 0; i < 15; i++)
			if(get_ran(1,0,40) == 1) {
				stuff_ul = special_ter_locs[i];
				for(j = 0; j < 4; j++)
					for(k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = cave_lake[k][j];
			}
	if(univ.town->terrain(0,0) == 2)
		for(i = 0; i < 15; i++)
			if(get_ran(1,0,40) == 1) {
				stuff_ul = special_ter_locs[i];
				for(j = 0; j < 4; j++)
					for(k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = surf_lake[k][j];
			}
	if(ter_type == 14)
		for(i = 0; i < 15; i++)
			if(get_ran(1,0,5) == 1) {
				stuff_ul = special_ter_locs[i];
				for(j = 0; j < 4; j++)
					for(k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = cave_fume[k][j];
			}
	if(ter_type == 15)
		for(i = 0; i < 15; i++)
			if(get_ran(1,0,5) == 1) {
				stuff_ul = special_ter_locs[i];
				for(j = 0; j < 4; j++)
					for(k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = surf_fume[k][j];
			}
	
	
	if(ground_type[ter_type] == 0) {
		for(i = 0; i < num_walls; i++) {
			r1 = get_ran(1,0,3);
			for(j = 9; j < 35; j++)
				switch(r1) {
					case 0:
						univ.town->terrain(j,8) = 6;
						break;
					case 1:
						univ.town->terrain(8,j) = 9;
						break;
					case 2:
						univ.town->terrain(j,35) = 12;
						break;
					case 3:
						univ.town->terrain(32,j) = 15;
						break;
				}
		}
		if((univ.town->terrain(17,8) == 6) && (univ.town->terrain(8,20) == 9))
			univ.town->terrain(8,8) = 21;
		if((univ.town->terrain(32,20) == 15) && (univ.town->terrain(17,35) == 12))
			univ.town->terrain(32,35) = 19;
		if((univ.town->terrain(17,8) == 6) && (univ.town->terrain(32,20) == 15))
			univ.town->terrain(32,8) = 32;
		if((univ.town->terrain(8,20) == 9) && (univ.town->terrain(17,35) == 12))
			univ.town->terrain(8,35) = 20;
	}
	if(ground_type[ter_type] == 36) {
		for(i = 0; i < num_walls; i++) {
			r1 = get_ran(1,0,3);
			for(j = 9; j < 35; j++)
				switch(r1) {
					case 0:
						univ.town->terrain(j,8) = 24;
						break;
					case 1:
						univ.town->terrain(8,j) = 26;
						break;
					case 2:
						univ.town->terrain(j,35) = 28;
						break;
					case 3:
						univ.town->terrain(32,j) = 30;
						break;
				}
		}
		if((univ.town->terrain(17,8) == 6) && (univ.town->terrain(8,20) == 9))
			univ.town->terrain(8,8) = 35;
		if((univ.town->terrain(32,20) == 15) && (univ.town->terrain(17,35) == 12))
			univ.town->terrain(32,35) = 33;
		if((univ.town->terrain(17,8) == 6) && (univ.town->terrain(32,20) == 15))
			univ.town->terrain(32,8) = 32;
		if((univ.town->terrain(8,20) == 9) && (univ.town->terrain(17,35) == 12))
			univ.town->terrain(8,35) = 34;
	}
	
	// TODO: Looks like I haven't yet implemented the generalized arenas
	
//	make_town_trim(1);
}


void elim_monst(unsigned short which,short spec_a,short spec_b) {
	short i;
	
	if(!sd_legit(spec_a,spec_b))
		return;
	if(PSD[spec_a][spec_b] > 0) {
		for(i = 0; i < univ.town->max_monst(); i++)
			if(univ.town.monst[i].number == which) {
				univ.town.monst[i].active = 0;
			}
	}
	
}



//short print_mes; // 0 - no 1 - yes
void dump_gold(short print_mes) {
	// Mildly kludgy gold check
	if(univ.party.gold > 30000) {
		univ.party.gold = 30000;
		if(print_mes == 1) {
			put_pc_screen();
			add_string_to_buf("Excess gold dropped.            ");
			print_buf();
		}
	}
	if(univ.party.food > 25000) {
		univ.party.food = 25000;
		if(print_mes == 1) {
			put_pc_screen();
			add_string_to_buf("Excess food dropped.            ");
			print_buf();
		}
	}
}



void pick_lock(location where,short pc_num) {
	ter_num_t terrain;
	short r1,which_item;
	bool will_break = false;
	short unlock_adjust;
	
	terrain = univ.town->terrain(where.x,where.y);
	which_item = pc_has_abil_equip(pc_num,eItemAbil::LOCKPICKS);
	if(which_item == 24) {
		add_string_to_buf("  Need lockpick equipped.        ");
		return;
	}
	
	r1 = get_ran(1,1,100) + univ.party[pc_num].items[which_item].ability_strength * 7;
	
	if(r1 < 75)
		will_break = true;
	
	r1 = get_ran(1,1,100) - 5 * stat_adj(pc_num,eSkill::DEXTERITY) + univ.town.difficulty * 7
		- 5 * univ.party[pc_num].skills[eSkill::LOCKPICKING] - univ.party[pc_num].items[which_item].ability_strength * 7;
	
	// Nimble?
	if(univ.party[pc_num].traits[eTrait::NIMBLE])
		r1 -= 8;
	
	if(pc_has_abil_equip(pc_num,eItemAbil::THIEVING) < 24)
		r1 = r1 - 12;
	
	if(scenario.ter_types[terrain].special != eTerSpec::UNLOCKABLE) {
		add_string_to_buf("  Wrong terrain type.           ");
		return;
	}
	unlock_adjust = scenario.ter_types[terrain].flag2.u;
	if((unlock_adjust >= 5) || (r1 > (unlock_adjust * 15 + 30))) {
		add_string_to_buf("  Didn't work.                ");
		if(will_break) {
			add_string_to_buf("  Pick breaks.                ");
			remove_charge(pc_num,which_item);
		}
		play_sound(41);
	}
	else {
		add_string_to_buf("  Door unlocked.                ");
		play_sound(9);
		univ.town->terrain(where.x,where.y) = scenario.ter_types[terrain].flag1.u;
	}
}

void bash_door(location where,short pc_num) {
	ter_num_t terrain;
	short r1,unlock_adjust;
	
	terrain = univ.town->terrain(where.x,where.y);
	r1 = get_ran(1,1,100) - 15 * stat_adj(pc_num,eSkill::STRENGTH) + univ.town.difficulty * 4;
	
	if(scenario.ter_types[terrain].special != eTerSpec::UNLOCKABLE) {
		add_string_to_buf("  Wrong terrain type.           ");
		return;
	}
	
	unlock_adjust = scenario.ter_types[terrain].flag2.u;
	if((unlock_adjust >= 5) || (r1 > (unlock_adjust * 15 + 40)) || (scenario.ter_types[terrain].flag3.u != 1))  {
		add_string_to_buf("  Didn't work.                ");
		damage_pc(pc_num,get_ran(1,1,4),DAMAGE_UNBLOCKABLE,eRace::UNKNOWN,0);
	}
	else {
		add_string_to_buf("  Lock breaks.                ");
		play_sound(9);
		univ.town->terrain(where.x,where.y) = scenario.ter_types[terrain].flag1.u;
	}
}


void erase_specials() {
	location where;
	short k,sd1,sd2;
	cSpecial sn;
	
	if((is_combat()) && (which_combat_type == 0))
		return;
	if(!is_town() && !is_combat())
		return;
	for(k = 0; k < 50; k++) {
		//if(univ.town->spec_id[k] >= 0) {
		sn = univ.town->specials[univ.town->spec_id[k]];
		sd1 = sn.sd1; sd2 = sn.sd2;
		if((sd_legit(sd1,sd2)) && (PSD[sd1][sd2] == 250)) {
			where = univ.town->special_locs[k];
			if((where.x != 100) && ((where.x > univ.town->max_dim()) || (where.y > univ.town->max_dim())
									 || (where.x < 0) || (where.y < 0))) {
				beep();
				add_string_to_buf("Town corrupt. Problem fixed.");
				print_nums(where.x,where.y,k);
				univ.town->special_locs[k].x = 0;
			}
			
			if(where.x != 100) {
//				switch(scenario.ter_types[univ.town->terrain(where.x,where.y)].picture) {
//					case 207: univ.town->terrain(where.x,where.y) = 0; break;
//					case 208: univ.town->terrain(where.x,where.y) = 170; break;
//					case 209: univ.town->terrain(where.x,where.y) = 210; break;
//					case 210: univ.town->terrain(where.x,where.y) = 217; break;
//					case 211: univ.town->terrain(where.x,where.y) = 2; break;
//					case 212: univ.town->terrain(where.x,where.y) = 36; break;
//				}
				univ.town.set_spot(where.x,where.y,false);
			}
		}
		
		
		//}
	}
}

void erase_out_specials() {
	
	short out_num;
	cSpecial sn;
	short sd1,sd2;
	location where;
	
	for(short i = 0; i < 2; i++)
		for(short j = 0; j < 2; j++)
			if(quadrant_legal(i,j)) {
				for(short k = 0; k < 18; k++) {
					if(i < 8 && univ.out.outdoors[i][j].exit_dests[k] >= 0 &&
					   (univ.out.outdoors[i][j].exit_locs[k].x == minmax(0,47,univ.out.outdoors[i][j].exit_locs[k].x)) &&
					   (univ.out.outdoors[i][j].exit_locs[k].y == minmax(0,47,univ.out.outdoors[i][j].exit_locs[k].y))) {
						if(univ.party.can_find_town[univ.out.outdoors[i][j].exit_dests[k]] == 0) {
							univ.out[48 * i + univ.out.outdoors[i][j].exit_locs[k].x][48 * j + univ.out.outdoors[i][j].exit_locs[k].y] =
							scenario.ter_types[univ.out.outdoors[i][j].terrain[univ.out.outdoors[i][j].exit_locs[k].x][univ.out.outdoors[i][j].exit_locs[k].y]].flag1.u;
						}
						else if(univ.party.can_find_town[univ.out.outdoors[i][j].exit_dests[k]] > 0) {
							univ.out[48 * i + univ.out.outdoors[i][j].exit_locs[k].x][48 * j + univ.out.outdoors[i][j].exit_locs[k].y] =
							univ.out.outdoors[i][j].terrain[univ.out.outdoors[i][j].exit_locs[k].x][univ.out.outdoors[i][j].exit_locs[k].y];
							
						}
					}
					if(univ.out.outdoors[i][j].special_id[k] < 0) continue; // TODO: Is this needed? Seems to be important, so why was it commented out?
					out_num = scenario.out_width * (univ.party.outdoor_corner.y + j) + univ.party.outdoor_corner.x + i;
					
					sn = univ.out.outdoors[i][j].specials[univ.out.outdoors[i][j].special_id[k]];
					sd1 = sn.sd1; sd2 = sn.sd2;
					if((sd_legit(sd1,sd2)) && (PSD[sd1][sd2] == 250)) {
						where = univ.out.outdoors[i][j].special_locs[k];
						if((where.x > 48) || (where.y > 48)
							|| (where.x < 0) || (where.y < 0)) {
							beep();
							add_string_to_buf("Outdoor section corrupt. Problem fixed.");
							univ.out.outdoors[i][j].special_id[k] = -1; // TODO: Again, was there a reason for commenting this out?
						}
						
						univ.out.outdoors[i][j].special_spot[where.x][where.y] = false;
					}
				}
			}
}

// returns id # of special at where, or 50 if there is none.
short get_town_spec_id(location where) {
	short i = 0;
	
	while((univ.town->special_locs[i] != where)	&& (i < 50))
		i++;
	return i;
}

// TODO: I don't think we need this
void clear_map() {
	RECT map_world_rect(map_gworld);
	
//	if(!map_visible) {
//		return;
//	}
//	draw_map(mini_map,11);
	
	fill_rect(map_gworld, map_world_rect, sf::Color::Black);
	
}

void draw_map(bool need_refresh) {
	if(!map_visible) return;
	short i,j,pic,pic2;
	RECT the_rect,map_world_rect = {0,0,384,384};
	location where;
	location kludge;
	RECT draw_rect,orig_draw_rect = {0,0,6,6},ter_temp_from;
	RECT	dlogpicrect = {6,6,42,42};
	bool draw_pcs = true,out_mode;
	RECT view_rect= {0,0,48,48},tiny_rect = {0,0,32,32},
	redraw_rect = {0,0,48,48},big_rect = {0,0,64,64}; // Rectangle visible in view screen
	
	RECT area_to_draw_from,area_to_draw_on = {29,47,269,287};
	short small_adj = 0;
	ter_num_t what_ter,what_ter2;
	bool draw_surroundings = false,expl,expl2;
	short total_size = 48; // if full redraw, use this to figure out everything
	RECT area_to_put_on_map_rect;
	RECT custom_from;
	short town_type = scenario.town_size[univ.town.num];
	
	draw_surroundings = true;
	
	if(need_refresh) {
		for(i = 0; i < 8; i++)
			for(j = 0; j < 64; j++)
				map_graphic_placed[i][j] = 0;
		// TODO: I suspect we don't need to save this info
	}
	
	town_map_adj.x = 0;
	town_map_adj.y = 0;
	
	// view rect is rect that is visible, redraw rect is area to redraw now
	// area_to_draw_from is final draw from rect
	// area_to_draw_on is final draw to rect
	// extern short store_pre_shop_mode,store_pre_talk_mode;
	if((is_out()) || ((is_combat()) && (which_combat_type == 0)) ||
		((overall_mode == MODE_TALKING) && (store_pre_talk_mode == 0)) ||
		((overall_mode == MODE_SHOPPING) && (store_pre_shop_mode == 0))) {
		view_rect.left = minmax(0,8,univ.party.loc_in_sec.x - 20);
		view_rect.right = view_rect.left + 40;
		view_rect.top = minmax(0,8,univ.party.loc_in_sec.y - 20);
		view_rect.bottom = view_rect.top + 40;
		redraw_rect = view_rect;
	}
	else {
		switch(town_type) {
			case 0:
				view_rect.left = minmax(0,24,univ.town.p_loc.x - 20);
				view_rect.right = view_rect.left + 40;
				view_rect.top = minmax(0,24,univ.town.p_loc.y - 20);
				view_rect.bottom = view_rect.top + 40;
				redraw_rect = big_rect;
				total_size = 64;
				break;
			case 1:
				view_rect.left = minmax(0,8,univ.town.p_loc.x - 20);
				view_rect.right = view_rect.left + 40;
				view_rect.top = minmax(0,8,univ.town.p_loc.y - 20);
				view_rect.bottom = view_rect.top + 40;
				redraw_rect = view_rect;
				break;
			case 2:
				view_rect = tiny_rect;
				redraw_rect = view_rect;
				//InsetRect(&area_to_draw_to,48,48);
				total_size = 32;
				break;
		}
	}
	if((is_out()) || ((is_combat()) && (which_combat_type == 0)) ||
		((overall_mode == MODE_TALKING) && (store_pre_talk_mode == 0)) ||
		((overall_mode == MODE_SHOPPING) && (store_pre_shop_mode == 0)) ||
		(((is_town()) || (is_combat())) && (town_type != 2))) {
		area_to_draw_from = view_rect;
		area_to_draw_from.left *= 6;
		area_to_draw_from.right *= 6;
		area_to_draw_from.top *= 6;
		area_to_draw_from.bottom *= 6;
	}
	else {
		area_to_draw_from = area_to_draw_on;
		area_to_draw_from.offset(-area_to_draw_from.left,-area_to_draw_from.top);
		small_adj = 0;
	}
	
	if(is_combat())
		draw_pcs = false;
	
	const char* title_string = "Your map:";
	bool canMap = true;
	
	if((is_combat()) && (which_combat_type == 0)) {
		title_string = "No map in combat.";
		canMap = false;
	}
	else if((is_town()) && ((univ.town.num == -1) || (univ.town.num == -1))) {
		title_string = "No map here.";
		canMap = false;
	}else if((is_town() && univ.town->defy_mapping)) {
		title_string = "This place defies mapping.";
		canMap = false;
	}
	else if(need_refresh) {
		map_gworld.setActive();
		
		fill_rect(map_gworld, map_world_rect, sf::Color::Black);
		
		// Now, if shopping or talking, just don't touch anything.
		if((overall_mode == MODE_SHOPPING) || (overall_mode == MODE_TALKING))
			redraw_rect.right = -1;
		
		if((is_out()) ||
			((is_combat()) && (which_combat_type == 0)) ||
			((overall_mode == MODE_TALKING) && (store_pre_talk_mode == 0)) ||
			((overall_mode == MODE_SHOPPING) && (store_pre_shop_mode == 0)))
			out_mode = true;
		else out_mode = false;
		
		area_to_put_on_map_rect = redraw_rect;
		area_to_put_on_map_rect.top = area_to_put_on_map_rect.left = 0;
		area_to_put_on_map_rect.right = area_to_put_on_map_rect.bottom = total_size;
		
		for(where.x= area_to_put_on_map_rect.left; where.x < area_to_put_on_map_rect.right; where.x++)
			for(where.y= area_to_put_on_map_rect.top; where.y < area_to_put_on_map_rect.bottom; where.y++)
				if((map_graphic_placed[where.x / 8][where.y] & (unsigned char)(s_pow(2,where.x % 8))) == 0) {
					draw_rect = orig_draw_rect;
					draw_rect.offset(6 * where.x + small_adj, 6 * where.y + small_adj);
					
					if(out_mode)
						what_ter = univ.out[where.x + 48 * univ.party.i_w_c.x][where.y + 48 * univ.party.i_w_c.y];
					else what_ter = univ.town->terrain(where.x,where.y);
					
					ter_temp_from = orig_draw_rect;
					
					if(out_mode)
						expl = univ.out.out_e[where.x + 48 * univ.party.i_w_c.x][where.y + 48 * univ.party.i_w_c.y];
					else expl = is_explored(where.x,where.y);
					
					if(expl != 0) {
						map_graphic_placed[where.x / 8][where.y] =
						map_graphic_placed[where.x / 8][where.y] | (unsigned char)(s_pow(2,where.x % 8));
						pic = scenario.ter_types[what_ter].picture;
						if(pic >= 1000) {
							if(spec_scen_g) {
								//print_nums(0,99,pic);
								pic = pic % 1000;
								sf::Texture* src_gw;
								graf_pos_ref(src_gw, custom_from) = spec_scen_g.find_graphic(pic);
								custom_from.offset(-13,-13);
								rect_draw_some_item(*src_gw,custom_from,map_gworld,draw_rect);
							}
						}
						else switch((pic >= 400) ? anim_map_pats[pic - 400] : map_pats[pic]) {
							case 0: case 10: case 11:
								if(scenario.ter_types[what_ter].picture < 400)
									ter_temp_from.offset(6 * (scenario.ter_types[what_ter].picture % 10),
														 6 * (scenario.ter_types[what_ter].picture / 10));
								else ter_temp_from.offset(24 * ((scenario.ter_types[what_ter].picture - 400) / 5),
														  6 * ((scenario.ter_types[what_ter].picture - 400) % 5) + 156);
								rect_draw_some_item(small_ter_gworld,ter_temp_from,map_gworld,draw_rect);
								break;
								
							default:
								if(((pic >= 400) ? anim_map_pats[pic - 400] : map_pats[pic]) < 30) {
									// Try a little optimization
									if((pic < 400) && (where.x < area_to_put_on_map_rect.right - 1)) {
										if(out_mode)
											what_ter2 = univ.out[(where.x + 1)+ 48 * univ.party.i_w_c.x][where.y + 48 * univ.party.i_w_c.y];
										else what_ter2 = univ.town->terrain(where.x + 1,where.y);
										if(out_mode)
											expl2 = univ.out.out_e[(where.x + 1) + 48 * univ.party.i_w_c.x][where.y + 48 * univ.party.i_w_c.y];
										else expl2 = is_explored(where.x + 1,where.y);
										pic2 = scenario.ter_types[what_ter2].picture;
										if((map_pats[pic2] == map_pats[pic]) && (expl2 != 0)) {
											draw_rect.right += 6;
											kludge = where; kludge.x++; // I don't know why I need to
											// do this, but it seems I do
											map_graphic_placed[kludge.x / 8][kludge.y] =
											map_graphic_placed[kludge.x / 8][kludge.y] | (unsigned char)(s_pow(2,kludge.x % 8));
										}
									}
									tileImage(mini_map, draw_rect,map_pat[((pic >= 400) ? anim_map_pats[pic - 400] : map_pats[pic]) - 1]);
									break;
								}
								//OffsetRect(&ter_temp_from,
								//	24 * ((map_pats[pic] - 30) / 5),
								//	138 + 6 * ((map_pats[pic] - 30) % 5));
								//rect_draw_some_item(small_ter_gworld,ter_temp_from,
								//	map_gworld,draw_rect,0,0);
								break;
						}
					}
				}
		
		map_gworld.display();
	}
	
	mini_map.setActive();
	
	// Now place terrain map gworld
	TextStyle style;
	style.font = FONT_BOLD;
	style.pointSize = 10;;
	
	the_rect = RECT(mini_map);
	tileImage(mini_map, the_rect,bg[4]);
	cPict theGraphic(mini_map);
	theGraphic.setBounds(dlogpicrect);
	theGraphic.setPict(21, PIC_DLOG);
	theGraphic.setFormat(TXT_FRAME, false);
	theGraphic.draw();
	style.colour = sf::Color::White;
	style.lineHeight = 12;
	win_draw_string(mini_map, map_title_rect,title_string,eTextMode::WRAP,style);
	win_draw_string(mini_map, map_bar_rect,"(Hit Escape to close.)",eTextMode::WRAP,style);
	
	/*SetPort( mini_map);
	 GetDialogItem(mini_map, 1, &the_type, &the_handle, &the_rect);
	 
	 PenSize(3,3);
	 InsetRect(&the_rect, -4, -4);
	 FrameRoundRect(&the_rect, 16, 16);
	 PenSize(1,1); */
	if(canMap) {
		rect_draw_some_item(map_gworld.getTexture(),area_to_draw_from,mini_map,area_to_draw_on);
		
		// Now place PCs and monsters
		if(draw_pcs) {
			if((is_town()) && (PSD[SDF_PARTY_DETECT_LIFE] > 0))
				for(i = 0; i < univ.town->max_monst(); i++)
					if(univ.town.monst[i].active > 0) {
						where = univ.town.monst[i].cur_loc;
						if((is_explored(where.x,where.y)) &&
							((where.x >= view_rect.left) && (where.x < view_rect.right)
							 && (where.y >= view_rect.top) && (where.x < view_rect.bottom))){
								
								draw_rect.left = area_to_draw_on.left + 6 * (where.x - view_rect.left);
								draw_rect.top = area_to_draw_on.top + 6 * (where.y - view_rect.top);
								//if((!is_out()) && (town_type == 2)) {
								//	draw_rect.left += 48;
								//	draw_rect.top += 48;
								//}
								draw_rect.right = draw_rect.left + 6;
								draw_rect.bottom = draw_rect.top + 6;
								
								map_graphic_placed[where.x / 8][where.y] =
								map_graphic_placed[where.x / 8][where.y] & ~((unsigned char)(s_pow(2,where.x % 8)));
								fill_rect(mini_map, draw_rect, sf::Color::Green);
								frame_circle(mini_map, draw_rect, sf::Color::Blue);
							}
					}
			if((overall_mode != MODE_SHOPPING) && (overall_mode != MODE_TALKING)) {
				where = (is_town()) ? univ.town.p_loc : global_to_local(univ.party.p_loc);
				
				draw_rect.left = area_to_draw_on.left + 6 * (where.x - view_rect.left);
				draw_rect.top = area_to_draw_on.top + 6 * (where.y - view_rect.top);
				//if((!is_out()) && (town_type == 2)) {
				//	draw_rect.left += 48;
				//	draw_rect.top += 48;
				//}
				draw_rect.right = draw_rect.left + 6;
				draw_rect.bottom = draw_rect.top + 6;
				map_graphic_placed[where.x / 8][where.y] =
				map_graphic_placed[where.x / 8][where.y] & ~((unsigned char)(s_pow(2,where.x % 8)));
				fill_rect(mini_map, draw_rect, sf::Color::Red);
				frame_circle(mini_map, draw_rect, sf::Color::Black);
				
			}
		}
	}
	
	mini_map.display();
	
	// Now exit gracefully
	mainPtr.setActive();
	
}



bool is_door(location destination) {
	
	if(scenario.ter_types[univ.town->terrain(destination.x,destination.y)].special == eTerSpec::UNLOCKABLE ||
	   scenario.ter_types[univ.town->terrain(destination.x,destination.y)].special == eTerSpec::CHANGE_WHEN_STEP_ON)
		return true;
	return false;
}


void display_map() {
	// Show the automap if it's not already visible
	if(map_visible) return;
	RECT the_rect;
	RECT	dlogpicrect = {6,6,42,42};
	
	mini_map.setVisible(true);
	map_visible = true;
	draw_map(true);
	makeFrontWindow(mainPtr);
}

void check_done() {
}

bool quadrant_legal(short i, short j) {
	if(univ.party.outdoor_corner.x + i >= scenario.out_width)
		return false;
	if(univ.party.outdoor_corner.y + j >= scenario.out_height)
		return false;
	if(univ.party.outdoor_corner.x + i < 0)
		return false;
	if(univ.party.outdoor_corner.y + j < 0)
		return false;
	return true;
}
