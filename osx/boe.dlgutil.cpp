
#include <cstring>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"

#include "boe.dlgutil.h"
#include "boe.text.h"
#include "boe.town.h"
#include "boe.itemdata.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "boe.party.h"
#include "boe.specials.h"
#include "boe.fileio.h"
#include "boe.graphics.h"
#include "boe.townspec.h"
#include "boe.main.h"
#include "boe.items.h"
#include "soundtool.h"
#include <cstdio>
#include "boe.newgraph.h"
#include "boe.infodlg.h"
#include "graphtool.h"
#include "mathutil.h"
#include "dlogutil.h"
#include "fileio.h"
#include "restypes.hpp"
#include "scrollbar.h"
#include "button.h"
#include "pict.h"
#include <boost/lexical_cast.hpp>
#include "prefs.hpp"
#define	NUM_HINTS	30

//extern big_tr_type t_d;
extern short stat_window;
extern eGameMode overall_mode;
//extern current_town_type univ.town;
//extern party_record_type party;
extern bool play_sounds,give_intro_hint,show_startup_splash;
extern sf::RenderWindow mainPtr;
extern RECT d_rects[80];
extern short d_rect_index[80];
extern short display_mode,stat_screen_mode,current_pc;
extern long register_flag;
extern long ed_flag,ed_key;
extern bool save_maps,give_delays;
extern location center;
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern bool map_visible;
extern sf::RenderWindow mini_map;
//extern town_item_list	t_i;
extern bool game_run_before,skip_boom_delay;
extern cScenario scenario;
extern cUniverse univ;
//extern piles_of_stuff_dumping_type *data_store;
//extern talking_record_type talking;
extern sf::Texture pc_gworld;

extern cScenarioList scen_headers;

short sign_mode,person_graphic,store_person_graphic,store_sign_mode;
long num_talk_entries;
char null_string[256] = "";
short store_tip_page_on = 0;

// Talking vars
word_rect_type store_words[50];
eGameMode store_pre_talk_mode;
short store_personality,store_personality_graphic,shop_identify_cost;
sf::RenderTexture talk_gworld;
bool talk_end_forced;
std::string old_str1, old_str2, one_back1, one_back2;
extern word_rect_type preset_words[9];
RECT talk_area_rect = {5,5,420,284}, word_place_rect = {44,7,372,257},talk_help_rect = {5,254,21,272};
std::string title_string;
m_num_t store_monst_type;
short store_m_num;
RECT dummy_rect = {0,0,0,0};
//hold_responses store_resp[83];
short strnum1,strnum2,oldstrnum1,oldstrnum2;
short store_talk_face_pic;//,cur_town_talk_loaded = -1;

// Shopping vars

// 1 - 499 ... regular items
// 500 alchemy
// 600-620 ... food not used!!!
// 700+ i - that healing
// 800 + - mage spells
// 900 + - priest spells
// n000 + i - magic store n item i
short store_shop_items[30],store_shop_costs[30];
// talk_area_rect and talk_help_rect used for this too
short store_shop_min,store_shop_max,store_cost_mult;
eGameMode store_pre_shop_mode;
char store_store_name[256];
// 0 - whole area, 1 - active area 2 - graphic 3 - item name
// 4 - item cost 5 - item extra str  6 - item help button
extern RECT shopping_rects[8][7];
// TODO: The duplication of RECT here shouldn't be necessary...
RECT bottom_help_rects[4] = {RECT{356,6,368,250},RECT{374,6,386,270},RECT{386,6,398,250},RECT{398,6,410,250}};
RECT shop_name_str = {44,6,56,200};
RECT shop_frame = {62,10,352,269};
RECT shop_done_rect = {388,212,411,275};

extern short store_shop_type;

const char* heal_types[] = {
	"Heal Damage","Cure Poison","Cure Disease","Cure Paralysis",
	"Uncurse Items","Cure Stoned Character","Raise Dead","Resurrection","Cure Dumbfounding"};
short heal_costs[9] = {50,30,80,100,250,500,1000,3000,100};
long cost_mult[7] = {5,7,10,13,16,20,25};

short store_scen_page_on,store_num_scen;

/*
shop_type:
0 - weapon shop
1 - armor shop
2 - misc shop
3 - healer
4 - food  no longer used!!!
5-9 - magic shop
10 - mage spells
11 - priest spells
12 alchemy
*/
void start_shop_mode(short shop_type,short shop_min,short shop_max,short cost_adj,const char* store_name)
{
	RECT area_rect;
	
	if (shop_max < shop_min)
		shop_max = shop_min; ////
	store_cost_mult = cost_adj;
	if (store_cost_mult > 6)
		store_cost_mult = 6;
	store_shop_type = shop_type;
	store_shop_min = shop_min;
	store_shop_max = shop_max;
	
	area_rect = talk_area_rect;
	strcpy((char *) store_store_name,store_name);
	
	talk_gworld.create(area_rect.width(), area_rect.height());
	
	store_pre_shop_mode = overall_mode;
	overall_mode = MODE_SHOPPING;
	stat_screen_mode = 1;
	
	set_up_shop_array();
	put_background();
	
	draw_shop_graphics(0,area_rect);
	
	put_item_screen(stat_window,0);
	give_help(26,27);
}

void end_shop_mode()
{
	RECT dummy_rect = {0,0,0,0};
	
	shop_sbar->hide();
	if (store_pre_shop_mode == 20) {
		old_str1 = "You conclude your business.";
		old_str2 = "";
		one_back1 = "You conclude your business.";
		one_back2 = "";
		
		strnum1 = strnum2 = oldstrnum1 = oldstrnum2 = 0;
		place_talk_str(old_str1, "", 0, dummy_rect);
	}
	
	overall_mode = store_pre_shop_mode;
	if (overall_mode == MODE_TALK_TOWN)
		overall_mode = MODE_TOWN;
	if (overall_mode == MODE_TOWN) {
		center = univ.town.p_loc;
		update_explored(center);
	}
	stat_screen_mode = 0;
	put_item_screen(stat_window,0);
	put_pc_screen();
	//refresh_screen(0);
	// TODO: I suspect REFRESH_NONE will suffice here
	redraw_screen(REFRESH_TERRAIN | REFRESH_BAR);
}

void handle_shop_event(location p)
{
	short i,store_what_picked;
	
	p.x -= 5;
	p.y -= 5;
	
	if (p.in(talk_help_rect)) {
		click_shop_rect(talk_help_rect);
		univ.party.help_received[26] = 0;
		give_help(26,27);
		return;
	}
	if (p.in(shop_done_rect)) {
		click_shop_rect(shop_done_rect);
		end_shop_mode();
		return;
	}
	
	for (i = 0; i < 8; i++) {
		store_what_picked = i;
		if ((p.in(shopping_rects[i][1])) && (store_shop_items[store_what_picked] >= 0)) {
			click_shop_rect(shopping_rects[i][1]);
			handle_sale(store_shop_items[store_what_picked],store_shop_costs[store_what_picked]);
		}
		if ((p.in(shopping_rects[i][6])) && (store_shop_items[store_what_picked] >= 0)
			&& (store_shop_type != 3) && (store_shop_type != 4)){
			click_shop_rect(shopping_rects[i][6]);
			handle_info_request(store_shop_items[store_what_picked]);
		}
	}
}

void handle_sale(short what_chosen,short cost)
{
	cItemRec base_item;
	short what_magic_shop,what_magic_shop_item,i;
	RECT dummy_rect = {0,0,0,0};
	
	switch (what_chosen / 100) {
		case 0: case 1: case 2: case 3: case 4:
			base_item = get_stored_item(what_chosen);
			base_item.ident = true;
			//cost = (base_item.charges == 0) ? base_item.value : base_item.value * base_item.charges;
			switch (pc_ok_to_buy(current_pc,cost,base_item)) {
				case 1: play_sound(-38); give_to_pc(current_pc,base_item,true); break;
				case 2: ASB("Can't carry any more items."); break;
				case 3: ASB("Not enough cash."); break;
				case 4: ASB("Item is too heavy."); break;
				case 5: ASB("You own too many of this."); break;
			}
			break;
		case 5:
			base_item = store_alchemy(what_chosen - 500);
			if (univ.party.alchemy[base_item.item_level] == true)
				ASB("You already know that recipe.");
			else if (take_gold(cost,false) == false)
				ASB("Not enough gold.");
			else {
				play_sound(62);
				ASB("You buy an alchemical recipe.");
				univ.party.alchemy[base_item.item_level] = true;
			}
			break;
		case 6:
			//base_item = food_types[what_chosen - 600];
			//if (take_gold(cost,false) == false)
			//	ASB("Not enough gold.");
			//else {
			//	play_sound(-38); ASB("You buy food.");
			//	univ.party.food += base_item.item_level;
			//}
			break;
		case 7:
			what_chosen -= 700;
			if (take_gold(cost,false) == false)
				ASB("Not enough gold.");
			else {
				ASB("You pay the healer.");
				play_sound(68);
				switch (what_chosen) {
					case 0:
						univ.party[current_pc].cur_health = univ.party[current_pc].max_health;
						break;
					case 1:
						univ.party[current_pc].status[2] = 0;
						break;
					case 2:
						univ.party[current_pc].status[7] = 0; break;
					case 3:
						univ.party[current_pc].status[12] = 0; break;
					case 4:
						for (i = 0; i < 24; i++)
							if ((univ.party[current_pc].equip[i] == true) &&
								(univ.party[current_pc].items[i].cursed))
								univ.party[current_pc].items[i].cursed = univ.party[current_pc].items[i].unsellable = false;
						break;
					case 5: case 6: case 7:
						univ.party[current_pc].main_status = MAIN_STATUS_ALIVE; break;
					case 8:
						univ.party[current_pc].status[9] = 0; break;
				}
			}
			break;
		case 8:
			base_item = store_mage_spells(what_chosen - 800 - 30);
			if ((base_item.item_level < 0) || (base_item.item_level > 61)) {
				ASB("Error 102: Report this!"); break;} // TODO: Play an error sound here
			if (univ.party[current_pc].mage_spells[base_item.item_level] == true)
				ASB("You already have this spell.");
			else if (take_gold(cost,false) == false)
				ASB("Not enough gold.");
			else {
				play_sound(62);
				ASB("You buy a spell.");
				univ.party[current_pc].mage_spells[base_item.item_level] = true;
				give_help(41,0);
			}
			break;
		case 9:
			base_item = store_priest_spells(what_chosen - 900 - 30);
			if ((base_item.item_level < 0) || (base_item.item_level > 61)) {
				ASB("Error 101: Report this!"); break;} // TODO: Play an error sound here
			if (univ.party[current_pc].priest_spells[base_item.item_level] == true)
				ASB("You already have this spell.");
			else if (take_gold(cost,false) == false)
				ASB("Not enough gold.");
			else {
				play_sound(62);
				ASB("You buy a spell.");
				univ.party[current_pc].priest_spells[base_item.item_level] = true;
				give_help(41,0);
			}
			break;
		default:
			what_magic_shop = (what_chosen / 1000) - 1;
			what_magic_shop_item = what_chosen % 1000;
			base_item = univ.party.magic_store_items[what_magic_shop][what_magic_shop_item];
			base_item.ident = true;
			switch (pc_ok_to_buy(current_pc,cost,base_item)) {
				case 1: play_sound(-38); give_to_pc(current_pc,base_item,true);
					univ.party.magic_store_items[what_magic_shop][what_magic_shop_item].variety = ITEM_TYPE_NO_ITEM;
					break;
				case 2: ASB("Can't carry any more items."); break;
				case 3: ASB("Not enough cash."); break;
				case 4: ASB("Item is too heavy."); break;
			}
			break;
	}
	set_up_shop_array();
	
	if (overall_mode != MODE_SHOPPING) {
		// TODO: Play an error sound here
		ASB("Shop error 1. Report This!");
	}
	draw_shop_graphics(0,dummy_rect);
	print_buf();
	put_pc_screen();
	put_item_screen(stat_window,0);
}


void handle_info_request(short what_chosen)
{
	cItemRec base_item;
	short what_magic_shop,what_magic_shop_item;
	
	switch (what_chosen / 100) {
		case 0: case 1: case 2: case 3: case 4:
			base_item = get_stored_item(what_chosen);
			base_item.ident = true;
			display_pc_item(6,0, base_item,0);
			break;
		case 5:
			// TODO: Create a dedicated dialog for alchemy info
			display_alchemy(NULL);
			break;
		case 8:
			base_item = store_mage_spells(what_chosen - 800 - 30);
			display_spells(0,base_item.item_level,0);
			break;
		case 9:
			base_item = store_priest_spells(what_chosen - 900 - 30);
			display_spells(1,base_item.item_level,0);
			break;
		default:
			what_magic_shop = (what_chosen / 1000) - 1;
			what_magic_shop_item = what_chosen % 1000;
			base_item = univ.party.magic_store_items[what_magic_shop][what_magic_shop_item];
			base_item.ident = true;
			display_pc_item(6,0, base_item,0);
			break;
	}
}

void set_up_shop_array()
{
	short i,shop_pos = 0;
	bool cursed_item = false;
	cItemRec store_i;
	long store_l;
	
	for (i = 0; i < 30; i++)
		store_shop_items[i] = -1;
	switch (store_shop_type) {
		case 0: case 1: case 2:
			for (i = store_shop_min; i < store_shop_max + 1; i++) {
				store_shop_items[shop_pos] = i;
				store_i = get_stored_item(store_shop_items[shop_pos]);
				store_shop_costs[shop_pos] = (store_i.charges == 0) ?
				store_i.value : store_i.value * store_i.charges;
				shop_pos++;
			}
			break;
		case 3:
			if (univ.party[current_pc].cur_health < univ.party[current_pc].max_health) {
				store_shop_items[shop_pos] = 700;
				store_shop_costs[shop_pos] = heal_costs[0];
				shop_pos++;
			}
			if (univ.party[current_pc].status[2] > 0) {
				store_shop_items[shop_pos] = 701;
				store_shop_costs[shop_pos] = heal_costs[1];
				shop_pos++;
			}
			if (univ.party[current_pc].status[7] > 0) {
				store_shop_items[shop_pos] = 702;
				store_shop_costs[shop_pos] = heal_costs[2];
				shop_pos++;
			}
			if (univ.party[current_pc].status[12] > 0) {
				store_shop_items[shop_pos] = 703;
				store_shop_costs[shop_pos] = heal_costs[3];
				shop_pos++;
			}
			if (univ.party[current_pc].status[9] > 0) {
				store_shop_items[shop_pos] = 708;
				store_shop_costs[shop_pos] = heal_costs[8];
				shop_pos++;
			}
			for (i = 0; i < 24; i++)
				if ((univ.party[current_pc].equip[i]) && (univ.party[current_pc].items[i].cursed))
					cursed_item = true;
			if (cursed_item) {
				store_shop_items[shop_pos] = 704;
				store_shop_costs[shop_pos] = heal_costs[4];
				shop_pos++;
			}
			if (univ.party[current_pc].main_status == 4) {
				store_shop_items[shop_pos] = 705;
				store_shop_costs[shop_pos] = heal_costs[5];
				shop_pos++;
			}
			if (univ.party[current_pc].main_status == 2){
				store_shop_items[shop_pos] = 706;
				store_shop_costs[shop_pos] = heal_costs[6];
				shop_pos++;
			}
			if  (univ.party[current_pc].main_status == 3){
				store_shop_items[shop_pos] = 707;
				store_shop_costs[shop_pos] = heal_costs[7];
				shop_pos++;
			}
			break;
		case 4:
			//for (i = store_shop_min; i < store_shop_max + 1; i++) {
			//	store_shop_items[shop_pos] = 600 + i;
			//	store_shop_costs[shop_pos] = food_types[i].value;
			//	shop_pos++;
			//}
			break;
		case 5: case 6: case 7: case 8: case 9:
			for (i = 0; i < 10; i++)
				if (univ.party.magic_store_items[store_shop_type - 5][i].variety != 0) {
					store_shop_items[shop_pos] = (store_shop_type - 4) * 1000 + i;
					store_i = univ.party.magic_store_items[store_shop_type - 5][i];
					store_shop_costs[shop_pos] = (store_i.charges == 0) ?
					store_i.value : store_i.value * store_i.charges;
					shop_pos++;
				}
			break;
		case 10:
			for (i = store_shop_min; i < store_shop_max + 1; i++)
				if (i == minmax(0,31,i)) {
					store_i = store_mage_spells(i);
					store_shop_costs[shop_pos] = store_i.value;
					store_shop_items[shop_pos] = 800 + i + 30;
					shop_pos++;
				}
			break;
		case 11:
			for (i = store_shop_min; i < store_shop_max + 1; i++)
				if (i == minmax(0,31,i)) {
					store_i = store_priest_spells(i);
					store_shop_costs[shop_pos] = store_i.value;
					store_shop_items[shop_pos] = 900 + i + 30;
					shop_pos++;
				}
			break;
		case 12:
			for (i = store_shop_min; i < store_shop_max + 1; i++)
				if (i == minmax(0,19,i)) {
					store_i = store_alchemy(i);
					store_shop_costs[shop_pos] = store_i.value;
					store_shop_items[shop_pos] = 500 + i;
					shop_pos++;
				}
			break;
	}
	for (i = 0; i < 30; i++)
		if (store_shop_items[i] >= 0) {
			store_l = store_shop_costs[i];
			store_l = (store_l * cost_mult[store_cost_mult]) / 10;
			store_shop_costs[i] = (short) store_l;
		}
	i = max(0,shop_pos - 8);
	shop_sbar->setMaximum(i);
}

void start_talk_mode(short m_num,short personality,m_num_t monst_type,short store_face_pic)////
{
	RECT area_rect;
	std::string place_string1;
	
	store_personality = personality;
	
	store_monst_type = monst_type;
	store_m_num = m_num;
	store_talk_face_pic = store_face_pic; ////
	area_rect = talk_area_rect;
	talk_gworld.create(area_rect.width(), area_rect.height());
	
	// first make sure relevant talk strs are loaded in
	load_town(personality / 10,univ.town.cur_talk);
	
	// load all possible responses
	store_responses();
	
	// Dredge up critter's name
	title_string = std::string(univ.town.cur_talk->talk_strs[personality % 10]) + ":";
	
	store_pre_talk_mode = overall_mode;
	overall_mode = MODE_TALKING;
	talk_end_forced = false;
	stat_screen_mode = 1;
	
	// Bring up and place first strings.
	place_string1 = univ.town.cur_talk->talk_strs[personality % 10 + 10];
	strnum1 = personality % 10 + 10;
	strnum2 = 0;
	
	old_str1 = place_string1;
	old_str2 = "";
	one_back1 = place_string1;
	one_back2 = "";
	place_talk_str(place_string1, "", 0, dummy_rect);
	
	put_item_screen(stat_window,0);
	give_help(5,6);
	
}

void end_talk_mode()
{
	overall_mode = store_pre_talk_mode;
	if (overall_mode == MODE_TALK_TOWN)
		overall_mode = MODE_TOWN;
	if (overall_mode == MODE_TOWN) {
		center = univ.town.p_loc;
		update_explored(center);
	}
	stat_screen_mode = 0;
	put_item_screen(stat_window,0);
	put_pc_screen();
	//refresh_screen(0);
	// TODO: I suspect REFRESH_NONE will suffice here
	redraw_screen(REFRESH_TERRAIN | REFRESH_BAR);
}

void handle_talk_event(location p)
{
	short i,j,force_special = 0,get_pc,s1 = -1,s2 = -1,s3 = -1;
	char asked[4];
	std::string place_string1, place_string2;
	
	short a,b,c,d,ttype,which_talk_entry = -1;
	
	p.x -= 5;
	p.y -= 5;
	
	if (p.in(talk_help_rect)) {
		univ.party.help_received[5] = 0;
		give_help(5,6);
		return;
	}
	
	for (i = 0; i < 9; i++)
		if ((p.in(preset_words[i].word_rect)) && ((talk_end_forced == false) || (i == 6) || (i == 5))) {
			click_talk_rect(old_str1,old_str2,preset_words[i].word_rect);
			switch (i) {
				case 0: case 1: case 2: case 7: case 8:
					force_special = i + 1;
					break;
				case 3: case 4:
					force_special = i + 1;
					
					//asked[0] = 'p';asked[1] = 'u';asked[2] = 'r';asked[3] = 'c';
					break;
				case 5: // save
					if (strnum1 <= 0) {
						// TODO: Play an error sound here
						return;
					}
					if(univ.party.has_talk_save(store_personality, univ.town.num, strnum1, strnum2)){
						ASB("This is already saved.");
						print_buf();
						return;
					} else {
						give_help(57,0);
						play_sound(0);
						bool success = univ.party.save_talk(store_personality,univ.town.num,strnum1,strnum2);
						if(success){
							ASB("Noted in journal.");
						} else {
							ASB("No more room in talking journal.");
						}
					}
					print_buf();
					return;
					break;
				case 6: // quit
					end_talk_mode();
					return;
					break;
				default:
					for (j = 0; j < 4; j++)
						asked[j] = preset_words[i].word[j];
					break;
			}
			i = 100;
		}
	if (i < 100) {
		for (i = 0; i < 50; i++)
			if ((p.in(store_words[i].word_rect)) && (talk_end_forced == false)) {
				click_talk_rect(old_str1,old_str2,store_words[i].word_rect);
				for (j = 0; j < 4; j++)
					asked[j] = store_words[i].word[j];
				
				i = 100;
			}
	}
	if (i == 50) // no event
		return;
	if (force_special == 9) {
		place_string1 = get_text_response(1017,0);
		asked[0] = place_string1[0];
		asked[1] = place_string1[1];
		asked[2] = place_string1[2];
		asked[3] = place_string1[3];
	}
	
	if ((asked[0] == 'n') && (asked[1] == 'a') &&(asked[2] == 'm') &&(asked[3] == 'e')) {
		force_special = 2;
	}
	else if ((asked[0] == 'l') && (asked[1] == 'o') &&(asked[2] == 'o') &&(asked[3] == 'k')) {
		force_special = 1;
	}
	else if (((asked[0] == 'j') && (asked[1] == 'o') &&(asked[2] == 'b')) ||
			 ((asked[0] == 'w') && (asked[1] == 'o') &&(asked[2] == 'r')&&(asked[3] == 'k')) ) {
		force_special = 3;
	}
	else if (((asked[0] == 'b') && (asked[1] == 'u') &&(asked[2] == 'y'))) {
		force_special = 3;
	}
	else if (((asked[0] == 'b') && (asked[1] == 'y') &&(asked[2] == 'e'))) {
		end_talk_mode();
	}
	
	if (force_special > 0) {
		switch (force_special) {
			case 1: case 2: case 3:
				place_string1 = univ.town.cur_talk->talk_strs[store_personality % 10 + 10 * force_special];
				
				oldstrnum1 = strnum1; oldstrnum2 = strnum2;
				strnum1 =  store_personality % 10 + 10 * force_special;
				strnum2 = 0;
				one_back1 = old_str1;
				one_back2 = old_str2;
				old_str1 = place_string1;
				old_str2 = place_string2;
				place_talk_str(place_string1,place_string2,0,dummy_rect);
				return;
				break;
			case 4: // buy button
				asked[0] = 'p';asked[1] = 'u';asked[2] = 'r';asked[3] = 'c';
				if (scan_for_response(asked) >= 0)
					break;
				asked[0] = 's';asked[1] = 'a';asked[2] = 'l';asked[3] = 'e';
				if (scan_for_response(asked) >= 0)
					break;
				asked[0] = 'h';asked[1] = 'e';asked[2] = 'a';asked[3] = 'l';
				if (scan_for_response(asked) >= 0)
					break;
				asked[0] = 'i';asked[1] = 'd';asked[2] = 'e';asked[3] = 'n';
				if (scan_for_response(asked) >= 0)
					break;
				asked[0] = 't';asked[1] = 'r';asked[2] = 'a';asked[3] = 'i';
				if (scan_for_response(asked) >= 0)
					break;
				break;
			case 5: // sell button
				asked[0] = 's';asked[1] = 'e';asked[2] = 'l';asked[3] = 'l';
				if (scan_for_response(asked) >= 0)
					break;
				break;
			case 8: // back 1
				strnum1 = oldstrnum1; strnum2 = oldstrnum2;
				place_string1 = one_back1;
				place_string2 = one_back2;
				one_back1 = old_str1;
				one_back2 = old_str2;
				old_str1 = place_string1;
				old_str2 = place_string2;
				place_talk_str(place_string1,place_string2,0,dummy_rect);
				return;
				break;
		}
	}
	
	which_talk_entry = scan_for_response(asked);
	if ((which_talk_entry < 0) || (which_talk_entry > 59)) {
		one_back1 = old_str1;
		one_back2 = old_str2;
		old_str2 = "";
		old_str1 = univ.town.cur_talk->talk_strs[store_personality % 10 + 160];
		if(old_str1.length() < 2)
			old_str1 = "You get no response.";
		place_talk_str(old_str1,old_str2,0,dummy_rect);
		strnum1 = -1;
		return;
	}
	
	ttype = univ.town.cur_talk->talk_nodes[which_talk_entry].type;
	a = univ.town.cur_talk->talk_nodes[which_talk_entry].extras[0];
	b = univ.town.cur_talk->talk_nodes[which_talk_entry].extras[1];
	c = univ.town.cur_talk->talk_nodes[which_talk_entry].extras[2];
	d = univ.town.cur_talk->talk_nodes[which_talk_entry].extras[3];
	
	place_string1 = univ.town.cur_talk->talk_strs[40 + which_talk_entry * 2];
	place_string2 = univ.town.cur_talk->talk_strs[40 + which_talk_entry * 2 + 1];
	
	oldstrnum1 = strnum1; oldstrnum2 = strnum2;
	strnum1 =  40 + which_talk_entry * 2; strnum2 = 40 + which_talk_entry * 2 + 1;
	
	switch(ttype) {
		case 0:
			break;
		case 1:
			if (PSD[a][b] > c) {
				strnum1 = strnum2;
				place_string1 = place_string2;
			}
			place_string2 = "";
			strnum2 = 0;
			break;
		case 2:
			PSD[a][b] = 1;
			break;
		case 3:
			if (univ.party.gold < a) {
				strnum1 = strnum2;
				place_string1 = place_string2;
			}
			else {
				talk_end_forced = true;
				univ.party.gold -= a;
				put_pc_screen();
				heal_party(30 * b);
				restore_sp_party(25 * b);
				univ.party.age += 700;
				univ.town.p_loc.x = c;
				univ.town.p_loc.y = d;
				center = univ.town.p_loc;
			}
			strnum2 = 0;
			place_string2 = "";
			break;
		case 4:
			if (day_reached((unsigned char) a,0) == true) {
				strnum1 = strnum2;
				place_string1 = place_string2;
			}
			place_string2 = "";
			strnum2 = 0;
			break;
		case 5:
			if (day_reached((unsigned char) a,(unsigned char) b) == true) {
				strnum1 = strnum2;
				place_string1 = place_string2;
			}
			place_string2 = "";
			strnum2 = 0;
			break;
		case 6:
			if (univ.town.num != a) {
				strnum1 = strnum2;
				place_string1 = place_string2;
			}
			place_string2 = "";
			strnum2 = 0;
			break;
		case 7:
			c = minmax(1,30,c);
			start_shop_mode(2,b,
							b + c - 1,a,place_string1.c_str());
			strnum1 = -1;
			return;
		case 8:
			if ((get_pc = char_select_pc(1,0,"Train who?")) < 6) {
				strnum1 = -1;
				spend_xp(get_pc,1, NULL);
			}
			place_string1 = "You conclude your training.";
			return;
			
		case 9: case 10: case 11:
			c = minmax(1,30,c);
			start_shop_mode(ttype + 1,b,
							b + c - 1,a,place_string1.c_str());
			strnum1 = -1;
			return;
		case 12: //healer
			start_shop_mode(3,univ.town.monst[store_m_num].extra1,
							univ.town.monst[store_m_num].extra2,a,place_string1.c_str());
			strnum1 = -1;
			return;
			break;
		case 13: // sell weap
			strnum1 = -1;
			stat_screen_mode = 3;
			put_item_screen(stat_window,1);
			give_help(42,43);
			break;
		case 14: // sell armor
			strnum1 = -1;
			stat_screen_mode = 4;
			put_item_screen(stat_window,1);
			give_help(42,43);
			break;
		case 15: // sell misc
			strnum1 = -1;
			stat_screen_mode = 5;
			put_item_screen(stat_window,1);
			give_help(42,43);
			break;
		case 16: case 17: // ident enchant
			strnum1 = -1;
			stat_screen_mode = (ttype == 16) ? 2 : 6;
			shop_identify_cost = a;
			put_item_screen(stat_window,1);
			give_help(ttype - 16 + 44,0);
			break;
		case 18:
			if (univ.party.gold < a) {
				strnum1 = strnum2;
				place_string1 = place_string2;
			}
			else {
				univ.party.gold -= a;
				put_pc_screen();
				
			}
			place_string2 = "";
			strnum2 = 0;
			break;
		case 19:
			if ((sd_legit(b,c) == true) && (PSD[b][c] == d)) {
				place_string1 = "You've already learned that.";
				strnum1 = -1;
			}
			else if (univ.party.gold < a) {
				strnum1 = strnum2;
				place_string1 + place_string2;
			}
			else {
				univ.party.gold -= a;
				put_pc_screen();
				if (sd_legit(b,c) == true)
					PSD[b][c] = d;
				else giveError("Invalid Stuff Done flag called in conversation.");
			}
			strnum2 = 0;
			place_string2 = "";
			break;
		case 20:
			if (univ.party.gold < a) {
				strnum1 = strnum2;
				strnum2 = 0;
				place_string1 = place_string2;
				place_string2 = "";
				break;
			}
			else {
				for (i = b; i <= b + c; i++)
					if ((i >= 0) && (i < 30) && (univ.party.boats[i].property == true)) {
						univ.party.gold -= a;
						put_pc_screen();
						univ.party.boats[i].property = false;
						place_string2 = "";
						strnum2 = 0;
						i = 1000;
					}
				if (i >= 1000)
					break;
			}
			place_string1 = "There are no boats left.";
			place_string2 = "";
			strnum1 = -1;
			strnum2 = -1;
			break;
		case 21:
			if (univ.party.gold < a) {
				strnum1 = strnum2;
				strnum2 = 0;
				place_string1 = place_string2;
				place_string2 = "";
				break;
			}
			else {
				for (i = b; i <= b + c; i++)
					if ((i >= 0) && (i < 30) && (univ.party.horses[i].property == true)) {
						univ.party.gold -= a;
						put_pc_screen();
						univ.party.horses[i].property = false;
						place_string2 = "";
						strnum2 = 0;
						i = 1000;
					}
				if (i >= 1000)
					break;
			}
			place_string1 = "There are no horses left.";
			place_string2 = "";
			strnum1 = -1;
			strnum2 = -1;
			break;
		case 22:
			if (univ.party.spec_items[a] > 0) {
				place_string1 = "You already have it.";
				strnum1 = -1;
			}
			else if (univ.party.gold < b) {
				place_string1 = place_string2;
				strnum1 = strnum2;
			}
			else {
				univ.party.gold -= b;
				put_pc_screen();
				univ.party.spec_items[a] = 1;
			}
			strnum2 = 0;
			place_string2 = "";
			break;
		case 23:
			start_shop_mode(5 + b,0,
							9,a,place_string1.c_str());
			strnum1 = -1;
			return;
		case 24:
			if (univ.party.can_find_town[b] > 0) {
			}
			else if (univ.party.gold < a) {
				strnum1 = strnum2;
				place_string1 = place_string2;
			}
			else {
				univ.party.gold -= a;
				put_pc_screen();
				univ.party.can_find_town[b] = 1;
			}
			strnum2 = 0;
			place_string2 = "";
			break;
		case 25:
			talk_end_forced = true;
			break;
		case 26:
			univ.town.monst[store_m_num].attitude = 1;
			univ.town.monst[store_m_num].mobility = 1;
			talk_end_forced = true;
			break;
		case 27:
			make_town_hostile();
			talk_end_forced = true;
			break;
		case 28:
			univ.town.monst[store_m_num].active = 0;
			// Special killing effects
			if (sd_legit(univ.town.monst[store_m_num].spec1,univ.town.monst[store_m_num].spec2) == true)
				PSD[univ.town.monst[store_m_num].spec1][univ.town.monst[store_m_num].spec2] = 1;
			talk_end_forced = true;
			break;
		case 29: // town special
			run_special(7,2,a,univ.town.p_loc,&s1,&s2,&s3);
			// check s1 & s2 to see if we got diff str, and, if so, munch old strs
			if ((s1 >= 0) || (s2 >= 0)) {
				strnum1 = -1;
				strnum2 = -1;
				place_string1 = "";
				place_string2 = "";
			}
			get_strs(place_string1,place_string2,2,s1,s2);
			//strnum1 = -1;
			//strnum2 = -1;
			if (s1 >= 0) strnum1 = 2000 + s1;
			if (s2 >= 0) strnum2 = 2000 + s2;
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;
		case 30: // scen special
			run_special(7,0,a,univ.town.p_loc,&s1,&s2,&s3);
			// check s1 & s2 to see if we got diff str, and, if so, munch old strs
			if ((s1 >= 0) || (s2 >= 0)) {
				strnum1 = -1;
				strnum2 = -1;
				place_string1 = "";
				place_string2 = "";
			}
			get_strs(place_string1,place_string2,0,s1,s2);
			//strnum1 = -1;
			//strnum2 = -1;
			if (s1 >= 0) strnum1 = 3000 + s1;
			if (s2 >= 0) strnum2 = 3000 + s2;
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;
			
			
			
	}
	
	one_back1 = old_str1;
	one_back2 = old_str2;
	old_str1 = place_string1;
	old_str2 = place_string2;
	place_talk_str(old_str1,old_str2,0,dummy_rect);
	
}

void store_responses()
{
	
}

void do_sign(short town_num, short which_sign, short sign_type,location sign_loc)
//town_num; // Will be 0 - 200 for town, 200 - 290 for outdoors
//short sign_type; // terrain type
{
	char sign_text[256];
	location view_loc;
	
	view_loc = (is_out()) ? univ.party.p_loc : univ.town.p_loc;
	make_cursor_sword();
	
	cChoiceDlog sign("view-sign.xml");
	cPict& pict = dynamic_cast<cPict&>(sign->getControl("ter"));
	
	store_sign_mode = sign_type;
	// TODO: Uh, why is it always displaying pic 94 for signs with custom graphics?
	if (scenario.ter_types[sign_type].picture < 1000)
		pict.setPict(scenario.ter_types[sign_type].picture);
	else pict.setPict(94);
	
	if (town_num >= 200) {
		town_num -= 200;
		//load_outdoor_str(loc(town_num % scenario.out_width, town_num / scenario.out_width),which_sign + 100,(char *) sign_text);
		strcpy((char*)sign_text,univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].sign_strs[which_sign]);
	}
	else {
		sprintf((char *) sign_text,"%s",univ.town->sign_strs[which_sign]);
	}
	sign->getControl("sign").setText(sign_text);
	
	sign.show();
}

void load_prefs(){
	give_intro_hint = get_bool_pref("GiveIntroHint", true);
	display_mode = get_int_pref("DisplayMode");
	play_sounds = get_bool_pref("PlaySounds", true);
	show_startup_splash = get_bool_pref("ShowStartupSplash", true);
	game_run_before = get_bool_pref("GameRunBefore");
	skip_boom_delay = get_bool_pref("SkipBoomDelay");
	
	PSD[SDF_NO_MAPS] = !get_bool_pref("SaveAutoMap", true);
	PSD[SDF_NO_FRILLS] = !get_bool_pref("DrawTerrainFrills", true);
	PSD[SDF_NO_INSTANT_HELP] = !get_bool_pref("ShowInstantHelp", true);
	PSD[SDF_NO_TER_ANIM] = !get_bool_pref("DrawTerrainAnimation", true);
	PSD[SDF_NO_SHORE_FRILLS] = !get_bool_pref("DrawTerrainShoreFrills", true);
	PSD[SDF_ROOM_DESCS_AGAIN] = get_bool_pref("RepeatRoomDescriptions");
	PSD[SDF_EASY_MODE] = get_bool_pref("EasyMode");
	PSD[SDF_LESS_WANDER_ENC] = get_bool_pref("LessWanderingMonsters");
	PSD[SDF_GAME_SPEED] = get_int_pref("GameSpeed");
	
	std::vector<int> help = get_iarray_pref("ReceivedHelp");
	for(int i : help)
		if(i < 120)
			univ.party.help_received[i] = 1;
}

void save_prefs(bool resetHelp){
	set_pref("GiveIntroHint", give_intro_hint);
	set_pref("DisplayMode", display_mode);
	set_pref("PlaySounds", play_sounds);
	set_pref("ShowStartupSplash", show_startup_splash);
	set_pref("GameRunBefore", game_run_before);
	set_pref("SkipBoomDelay", skip_boom_delay);
	
	if(overall_mode == MODE_STARTUP) {
		set_pref("SaveAutoMap", !PSD[SDF_NO_MAPS]);
		set_pref("DrawTerrainFrills", !PSD[SDF_NO_FRILLS]);
		set_pref("ShowInstantHelp", !PSD[SDF_NO_INSTANT_HELP]);
		set_pref("DrawTerrainAnimation", !PSD[SDF_NO_TER_ANIM]);
		set_pref("DrawTerrainShoreFrills", !PSD[SDF_NO_SHORE_FRILLS]);
		set_pref("RepeatRoomDescriptions", bool(PSD[SDF_ROOM_DESCS_AGAIN]));
		set_pref("EasyMode", bool(PSD[SDF_EASY_MODE]));
		set_pref("LessWanderingMonsters", bool(PSD[SDF_LESS_WANDER_ENC]));
		set_pref("GameSpeed", PSD[SDF_GAME_SPEED]);
	}
	
	if(resetHelp) clear_pref("ReceivedHelp");
	
	bool success = sync_prefs();
	if(!success){
		giveError("There was a problem writing to the preferences file. When the game is next run the preferences will revert to their previously set values.","Should you manage to resolve the problem without closing the program, simply open the preferences screen and click \"OK\" to try again.");
	}
}

static bool prefs_event_filter (cDialog& me, std::string id, eKeyMod mods)
{
	// TODO: I should no longer need done_yet as this now only handles the okay and cancel buttons; the LEDs are now handled automatically by the cLed class (and the cLedGroup class, for LED groups).
	bool done_yet = false,did_cancel = false,reset_help = false;
	short i;
	
	if(id == "okay") {
		done_yet = true;
		me.toast();
	} else if(id == "cancel") {
		done_yet = true;
		me.toast();
		did_cancel = true;
	}
	
	if (done_yet== true) {
		if (did_cancel == false) {
			std::string cur_display_mode = dynamic_cast<cLedGroup&>(me["display"]).getSelected();
			// TODO: Make an enum for the display mode
			if(cur_display_mode == "mid") display_mode = 0;
			else if(cur_display_mode == "tl") display_mode = 1;
			else if(cur_display_mode == "tr") display_mode = 2;
			else if(cur_display_mode == "bl") display_mode = 3;
			else if(cur_display_mode == "br") display_mode = 4;
			else if(cur_display_mode == "win") display_mode = 5;
			PSD[SDF_NO_MAPS] = dynamic_cast<cLed&>(me["nomaps"]).getState() != led_off;
			PSD[SDF_NO_SOUNDS] = dynamic_cast<cLed&>(me["nosound"]).getState() != led_off;
			play_sounds = !PSD[SDF_NO_SOUNDS];
			PSD[SDF_NO_FRILLS] = dynamic_cast<cLed&>(me["nofrills"]).getState() != led_off;
			PSD[SDF_ROOM_DESCS_AGAIN] = dynamic_cast<cLed&>(me["repeatdesc"]).getState() != led_off;
			PSD[SDF_NO_INSTANT_HELP] = dynamic_cast<cLed&>(me["nohelp"]).getState() != led_off;
			PSD[SDF_EASY_MODE] = dynamic_cast<cLed&>(me["easier"]).getState() != led_off;
			PSD[SDF_LESS_WANDER_ENC] = dynamic_cast<cLed&>(me["lesswm"]).getState() != led_off;
			PSD[SDF_NO_TER_ANIM] = dynamic_cast<cLed&>(me["noanim"]).getState() != led_off;
			PSD[SDF_NO_SHORE_FRILLS] = dynamic_cast<cLed&>(me["noshore"]).getState() != led_off;
			show_startup_splash = dynamic_cast<cLed&>(me["skipsplash"]).getState() == led_off;
			std::string speed = dynamic_cast<cLedGroup&>(me["speed"]).getSelected();
			if(speed == "fast")
				PSD[SDF_GAME_SPEED] = 0;
			else if(speed == "med")
				PSD[SDF_GAME_SPEED] = 1;
			else if(speed == "slow")
				PSD[SDF_GAME_SPEED] = 2;
			else if(speed == "snail")
				PSD[SDF_GAME_SPEED] = 3;
			if(dynamic_cast<cLed&>(me["resethelp"]).getState() == led_red) {
				for (i = 0; i < 120; i++)
					univ.party.help_received[i] = 0;
				reset_help = true;
			}
		}
		save_maps = 1 - PSD[SDF_NO_MAPS];
		give_delays = PSD[SDF_NO_FRILLS];
		save_prefs(reset_help);
	}
	return true;
}

void pick_preferences()
{
	make_cursor_sword();
	
	cDialog prefsDlog("preferences.xml");
	prefsDlog.attachClickHandlers(&prefs_event_filter, {"okay", "cancel"});
	
	cLedGroup& displayMode = dynamic_cast<cLedGroup&>(prefsDlog["display"]);
	switch(display_mode) {
		case 0:
			displayMode.setSelected("mid");
			break;
		case 1:
			displayMode.setSelected("tl");
			break;
		case 2:
			displayMode.setSelected("tr");
			break;
		case 3:
			displayMode.setSelected("bl");
			break;
		case 4:
			displayMode.setSelected("br");
			break;
		case 5:
			displayMode.setSelected("win");
			break;
	}
	
	dynamic_cast<cLed&>(prefsDlog["nomaps"]).setState(PSD[SDF_NO_MAPS] ? led_red : led_off);
	dynamic_cast<cLed&>(prefsDlog["nosound"]).setState(!play_sounds ? led_red : led_off);
	dynamic_cast<cLed&>(prefsDlog["nofrills"]).setState(PSD[SDF_NO_FRILLS] ? led_red : led_off);
	dynamic_cast<cLed&>(prefsDlog["repeatdesc"]).setState(PSD[SDF_ROOM_DESCS_AGAIN] ? led_red : led_off);
	dynamic_cast<cLed&>(prefsDlog["nohelp"]).setState(PSD[SDF_NO_INSTANT_HELP] ? led_red : led_off);
	dynamic_cast<cLed&>(prefsDlog["easier"]).setState(PSD[SDF_EASY_MODE] ? led_red : led_off);
	dynamic_cast<cLed&>(prefsDlog["lesswm"]).setState(PSD[SDF_LESS_WANDER_ENC] ? led_red : led_off);
	dynamic_cast<cLed&>(prefsDlog["noanim"]).setState(PSD[SDF_NO_TER_ANIM] ? led_red : led_off);
	dynamic_cast<cLed&>(prefsDlog["noshore"]).setState(PSD[SDF_NO_SHORE_FRILLS] ? led_red : led_off);
	dynamic_cast<cLed&>(prefsDlog["skipsplash"]).setState(show_startup_splash ? led_off : led_red);
	cLedGroup& gameSpeed = dynamic_cast<cLedGroup&>(prefsDlog["speed"]);
	switch(PSD[SDF_GAME_SPEED]) {
		case 0:
			gameSpeed.setSelected("fast");
			break;
		case 1:
			gameSpeed.setSelected("med");
			break;
		case 2:
			gameSpeed.setSelected("slow");
			break;
		case 3:
			gameSpeed.setSelected("snail");
			break;
	}
	
	if (univ.party.help_received[55] == 0) {
		// TODO: Not sure if this bit is needed?
//		cd_initial_draw(1099);
		give_help(55,0,prefsDlog);
	}
	
	int store_display_mode = display_mode;
	prefsDlog.run();
	
	if(display_mode != store_display_mode)
		adjust_window_mode();
}

static void put_party_stats(cDialog& me)
{
	short i;
	
	
	for (i = 0; i < 6; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if (univ.party[i].main_status > 0) {
			me["name" + n].setText(univ.party[i].name);
			me["trait" + n].show();
			me["train" + n].show();
			me["delete" + n].setText("Delete");
			me["pic" + n].show();
			me["pc" + n].show();
			dynamic_cast<cPict&>(me["pc" + n]).setPict(univ.party[i].which_graphic);
		}
		else {
			me["name" + n].setText("Empty.");
			me["trait" + n].hide();
			me["train" + n].hide();
			me["delete" + n].setText("Create");
			me["pic" + n].hide();
			me["pc" + n].hide();
		}
		
	}
	draw_startup(0);
}

static bool edit_party_event_filter(cDialog& me, std::string item_hit, eKeyMod mods)
{
	if(item_hit == "done") {
		me.toast();
	} else if(item_hit == "help") {
		univ.party.help_received[22] = 0;
		give_help(222,23,me);
	} else {
		short which_pc = item_hit[item_hit.length()-1] - '1';
		item_hit = item_hit.substr(0, item_hit.length() - 1);
		if(item_hit == "name") {
			if (univ.party[which_pc].main_status == 0) {
				// TODO: Clicking a blank field made a new PC? Something to consider reinstating!
				//give_help(56,0,989);
				//create_pc(which_pc,989);
				//cd_initial_draw(989); // extra redraw, just in case
			}
			else pick_pc_name(which_pc,&me);
			put_party_stats(me);
		} else if(item_hit == "trait") {
			pick_race_abil(&univ.party[which_pc],0,&me);
			put_party_stats(me);
		} else if(item_hit == "train") {
			spend_xp(which_pc,0,&me);
			put_party_stats(me);
		} else if(item_hit == "pic") {
			if (univ.party[which_pc].main_status <= 0)
				return true;
			if (univ.party[which_pc].main_status != 0)
				pick_pc_graphic(which_pc,1,&me);
			put_party_stats(me);
		} else if(item_hit == "delete") { // Note: This button is also used for "create new PC".
			if (univ.party[which_pc].main_status != MAIN_STATUS_ABSENT) {
				if(cChoiceDlog("delete-pc-confirm.xml",{"yes","no"}).show() == "yes")
					univ.party[which_pc].main_status = MAIN_STATUS_ABSENT;
				put_party_stats(me);
			}
			else {
				give_help(56,0,me);
				create_pc(which_pc,&me);
				put_party_stats(me);
				// TODO: Not sure if this is needed
//				cd_initial_draw(989); // extra redraw, just in case
			}
		}
	}
	return true;
}

extern bool pc_gworld_loaded;
void edit_party(short can_create,short can_cancel)
{
	bool munch_pc_graphic = false;
	
	make_cursor_sword();
	
	if (!pc_gworld_loaded) {
		munch_pc_graphic = true;
		pc_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcs"));
	}
	
	cDialog pcDialog("edit-party.xml");
	std::vector<std::string> buttons = {"done", "help"};
	for(int i = 1; i <= 6; i++) {
		std::string n = boost::lexical_cast<std::string>(i);
		buttons.push_back("name" + n);
		buttons.push_back("delete" + n);
		buttons.push_back("trait" + n);
		buttons.push_back("train" + n);
		buttons.push_back("pic" + n);
	}
	
	pcDialog.attachClickHandlers(edit_party_event_filter, buttons);
	
	put_party_stats(pcDialog);
	if (univ.party.help_received[22] == 0) {
		// TODO: Not sure if this is needed
		//		cd_initial_draw(989);
		give_help(22,23,pcDialog);
	}
	
	pcDialog.run();
	
	if (univ.party[current_pc].main_status != 1)
		current_pc = first_active_pc();
	
	
}

static bool tip_of_day_event_filter(cDialog& me, std::string item_hit, eKeyMod mods)
{
	std::string place_str;
	
	if(item_hit == "done") {
		me.toast();
	} else if(item_hit == "next") {
		store_tip_page_on++;
		if (store_tip_page_on == NUM_HINTS)
			store_tip_page_on = 0;
		place_str = get_str("tips",50 + store_tip_page_on);
		me["tip"].setText(place_str);
	}
	return true;
}

void tip_of_day()
{
	std::string place_str;
	
	store_tip_page_on = get_ran(1,0,NUM_HINTS - 1);
	
	make_cursor_sword();
	
	cDialog tips("tip-of-day.xml");
	tips.attachClickHandlers(&tip_of_day_event_filter,{"done","next"});
	
	place_str = get_str("tips",50 + store_tip_page_on);
	tips["tip"].setText(place_str);
	
	cLed& showAtStart = dynamic_cast<cLed&>(tips["onstart"]);
	showAtStart.setState(give_intro_hint ? led_red : led_off);
	
	tips.run();
	give_intro_hint = showAtStart.getState() != led_off;
	save_prefs();
	
}

static void put_scen_info(cDialog& me)
{
	unsigned int i;
	std::ostringstream sout;
	const char *ratings[] = {"G","PG","R","NC-17"};
	const char *difficulty[] = {"Low","Medium","High","Very High"};
	
	for(i = 0; i < 3; i++) {
		sout << i + 1;
		std::string n = sout.str();
		if (scen_headers.size() > (store_scen_page_on * 3 + i) && scen_headers.data(store_scen_page_on * 3 + i).flag1 != 0) {
			me["pict" + n].show();
			dynamic_cast<cPict&>(me["pic" + n]).setPict(scen_headers.data(store_scen_page_on * 3 + i).intro_pic);
			sout.str("");
			sout << scen_headers.strs(store_scen_page_on * 3 + i).name;
			sout << " v" << scen_headers.data(store_scen_page_on * 3 + i).ver[0];
			sout << '.' << scen_headers.data(store_scen_page_on * 3 + i).ver[1];
			sout << '.' << scen_headers.data(store_scen_page_on * 3 + i).ver[2];
			sout << " - |  Difficulty: " << difficulty[scen_headers.data(store_scen_page_on * 3 + i).difficulty];
			sout << ", Rating: " << ratings[scen_headers.data(store_scen_page_on * 3 + i).rating];
			sout << " |" << scen_headers.strs(store_scen_page_on * 3 + i).who1;
			sout << " |" << scen_headers.strs(store_scen_page_on * 3 + i).who2;
			me["desc" + n].setText(sout.str());
			me["start" + n].show();
		}
		else {
			me["pict" + n].hide();
			me["desc" + n].setText("");
			me["start" + n].hide();
		}
	}
}

static static bool pick_a_scen_event_filter(cDialog& me, std::string item_hit, eKeyMod mods)
{
	if(item_hit == "cancel") {
		me.setResult<short>(-1);
		me.toast();
	} else if(item_hit == "next") {
		if (store_scen_page_on == 0)
			store_scen_page_on = (store_num_scen - 1) / 3;
		else store_scen_page_on--;
		put_scen_info(me);
	} else if(item_hit == "prev") {
		if (store_scen_page_on == (store_num_scen - 1) / 3)
			store_scen_page_on = 0;
		else store_scen_page_on++;
		put_scen_info(me);
	} else if(item_hit.substr(0,item_hit.length()-1) == "start") {
		int scen_hit = item_hit[item_hit.length()-1] - '1';
		me.setResult<short>(scen_hit + store_scen_page_on * 3);
		me.toast();
	}
	return true;
}

short pick_a_scen()
{
	
	build_scen_headers();
	
	store_num_scen = scen_headers.size();
//	for (i = 0; i < 25; i++)
//		if (scen_headers[i].flag1 != 0)
//			store_num_scen++;
	store_scen_page_on = 0;
	
	if (store_num_scen == 0) {
		cChoiceDlog("no-scenarios.xml").show();
		return -1;
	}
	make_cursor_sword();
	
	cDialog pickScen("pick-scenario.xml");
	pickScen.attachClickHandlers(pick_a_scen_event_filter, {"cancel", "next", "prev", "start1", "start2", "start3"});
	
	put_scen_info(pickScen);
	
	if (store_num_scen <= 3) {
		pickScen["next"].hide();
		pickScen["prev"].hide();
	}
	
	pickScen.run();
	return pickScen.getResult<short>();
}

short pick_prefab_scen()
{
	
	make_cursor_sword();
	
	cChoiceDlog pickScenario("pick-prefab-scen.xml", {"cancel", "scen1", "scen2", "scen3"});
	
	std::string item_hit = pickScenario.show();
	if(item_hit == "cancel") return -1;
	else return item_hit[4] - '1';
}

//void give_password_filter (short item_hit)
//{
//
//	switch (item_hit) {
//		default:
//
//			toast_dialog();
//			break;
//
//		}
//}
//
//bool enter_password()
//// ignore parent in Mac version
//{
//	short town_strs_hit,i;
//	char temp_str[256];
//
//	cd_create_dialog_parent_num(823,0);
//
//	CDSN(823,2,0);
//
//	town_strs_hit = cd_run_dialog();
//
//	CDGT(823,2,(char *) temp_str);
//	i = wd_to_pwd(temp_str);
//
//	cd_kill_dialog(823);
//
//	return check_p(i);
//}
//
//short wd_to_pwd(Str255 str)
//{
//	char pwd[8] = "aaaaaa";
//	short i;
//	long val = 0,pow[6] = {1,10,100,1000,9999,99999};
//
//	for (i = 0; i < 6; i++) {
//		if (str[i] == 0)
//			i = 6;
//			else {
//				if ((str[i] >= 65) && (str[i] <= 90))
//					pwd[i] = str[i] + 32;
//				else if ((str[i] >= 48) && (str[i] <= 57))
//					pwd[i] = str[i] + 49;
//				else if ((str[i] >= 97) && (str[i] <= 122))
//					pwd[i] = str[i];
//				}
//		}
//	for (i = 0; i < 6; i++)
//		val = val + pow[i] * (long) (pwd[i] - 97);
//	val = val % 30000;
//	return (short) val;
//}
