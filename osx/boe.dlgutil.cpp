
#include <string.h>

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
#include <stdio.h>
#include "dlgtool.h"
#include "dlgconsts.h"
#include "boe.newgraph.h"
#include "boe.infodlg.h"
#include "graphtool.h"
#include "mathutil.h"
#include "dlgutil.h"
#include "fileio.h"
#define	NUM_HINTS	30

//extern big_tr_type t_d;
extern short stat_window,dialog_answer;
extern eGameMode overall_mode;
//extern current_town_type univ.town;
//extern party_record_type party;
extern bool in_startup_mode,play_sounds,give_intro_hint,show_startup_splash;
extern WindowPtr mainPtr;
extern Rect d_rects[80];
extern short d_rect_index[80];
extern short display_mode,stat_screen_mode,current_pc;
extern long register_flag;
extern long ed_flag,ed_key;
extern bool save_maps,give_delays;
extern location center;
extern ControlHandle text_sbar,item_sbar,shop_sbar;
extern bool modeless_exists[18];
extern DialogPtr modeless_dialogs[18] ;
//extern town_item_list	t_i;
extern bool game_run_before,skip_boom_delay;
extern ModalFilterUPP main_dialog_UPP;
extern cScenario scenario;
extern cUniverse univ;
//extern piles_of_stuff_dumping_type *data_store;
//extern talking_record_type talking;


					
short terrain_pic[256]; 

extern cScenarioList scen_headers;
					
GWorldPtr pcs_gworld = NULL;

short sign_mode,person_graphic,store_person_graphic,store_sign_mode;
long num_talk_entries;
Str255 null_string = "";
short store_tip_page_on = 0;

// Talking vars
word_rect_type store_words[50];
eGameMode store_pre_talk_mode;
short store_personality,store_personality_graphic,shop_identify_cost;
GWorldPtr talk_gworld = NULL;
bool talk_end_forced;
Str255 old_str1,old_str2,one_back1,one_back2; 
extern word_rect_type preset_words[9];
Rect talk_area_rect = {5,5,420,284}, word_place_rect = {44,7,372,257},talk_help_rect = {5,254,21,272};
char title_string[50];
m_num_t store_monst_type;
short store_m_num;
Rect dummy_rect = {0,0,0,0};
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
extern Rect shopping_rects[8][7];
Rect bottom_help_rects[4] = {{356,6,368,250},{374,6,386,270},{386,6,398,250},{398,6,410,250}};
Rect shop_name_str = {44,6,56,200};
Rect shop_frame = {62,10,352,269};
Rect shop_done_rect = {388,212,411,275};

extern short store_shop_type;

char *heal_types[] = {"Heal Damage","Cure Poison","Cure Disease","Cure Paralysis",
		"Uncurse Items","Cure Stoned Character","Raise Dead","Resurrection","Cure Dumbfounding"};
short heal_costs[9] = {50,30,80,100,250,500,1000,3000,100};
long cost_mult[7] = {5,7,10,13,16,20,25}; 
short cur_display_mode;

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
void start_shop_mode(short shop_type,short shop_min,short shop_max,short cost_adj,char *store_name)
{
	Rect area_rect;
	
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

	OffsetRect(&area_rect, -1 * area_rect.left,-1 * area_rect.top);
	if (talk_gworld == NULL)
		NewGWorld(&talk_gworld,  0 /*8*/,&area_rect, NULL, NULL, kNativeEndianPixMap);
	
	store_pre_shop_mode = overall_mode;
	overall_mode = MODE_SHOPPING;
	stat_screen_mode = 1;
	create_clip_region();
	
	set_up_shop_array();
	put_background();
	
	draw_shop_graphics(0,area_rect);
	
	put_item_screen(stat_window,0);
	give_help(26,27,0);
}

void end_shop_mode()
{
	Rect dummy_rect = {0,0,0,0};
	
	HideControl(shop_sbar);
	if (store_pre_shop_mode == 20) {
		strcpy((char *)old_str1,"You conclude your business.");
		strcpy((char *)old_str2,"");
		strcpy((char *)one_back1,"You conclude your business.");
		strcpy((char *)one_back2,"");

		strnum1 = strnum2 = oldstrnum1 = oldstrnum2 = 0;
		place_talk_str((char *)old_str1,"",0,dummy_rect);
		}
		else {
			DisposeGWorld(talk_gworld);
			talk_gworld = NULL;
			}
			
	overall_mode = store_pre_shop_mode;
	create_clip_region();
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
	redraw_screen();
}

void handle_shop_event(Point p)
{
	short i,store_what_picked;

	p.h -= 5;
	p.v -= 5;

	if (PtInRect(p,&talk_help_rect)) {
		click_shop_rect(talk_help_rect);
		univ.party.help_received[26] = 0;
		give_help(26,27,0);
		return;
		}
	if (PtInRect(p,&shop_done_rect)) {
		click_shop_rect(shop_done_rect);
		end_shop_mode();
		return;
		}

	for (i = 0; i < 8; i++) {
		store_what_picked = i + GetControlValue(shop_sbar);
		if ((PtInRect(p,&shopping_rects[i][1])) && (store_shop_items[store_what_picked] >= 0)) {
			click_shop_rect(shopping_rects[i][1]);
			handle_sale(store_shop_items[store_what_picked],store_shop_costs[store_what_picked]);
			}
		if ((PtInRect(p,&shopping_rects[i][6])) && (store_shop_items[store_what_picked] >= 0)
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
	Rect dummy_rect = {0,0,0,0};

	switch (what_chosen / 100) {
		case 0: case 1: case 2: case 3: case 4: 
			base_item = get_stored_item(what_chosen);
			base_item.item_properties = base_item.item_properties | 1;
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
			//	else {
			//		play_sound(-38); ASB("You buy food.");
			//		univ.party.food += base_item.item_level;
			//		}
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
									(univ.party[current_pc].items[i].is_cursed()))
										univ.party[current_pc].items[i].item_properties =
											univ.party[current_pc].items[i].item_properties & 239;
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
				SysBeep(50); ASB("Error 102: Report this!"); break;}
			if (univ.party[current_pc].mage_spells[base_item.item_level] == true)
				ASB("You already have this spell.");
				else if (take_gold(cost,false) == false)
					ASB("Not enough gold.");
					else {
						play_sound(62);
						ASB("You buy a spell.");
						univ.party[current_pc].mage_spells[base_item.item_level] = true;
						give_help(41,0,0);
						}
			break;
		case 9:
			base_item = store_priest_spells(what_chosen - 900 - 30);
			if ((base_item.item_level < 0) || (base_item.item_level > 61)) {
				SysBeep(50); ASB("Error 101: Report this!"); break;}
			if (univ.party[current_pc].priest_spells[base_item.item_level] == true)
				ASB("You already have this spell.");
				else if (take_gold(cost,false) == false)
					ASB("Not enough gold.");
					else {
						play_sound(62);
						ASB("You buy a spell.");
						univ.party[current_pc].priest_spells[base_item.item_level] = true;
						give_help(41,0,0);
						}
			break;
		default:
			what_magic_shop = (what_chosen / 1000) - 1;
			what_magic_shop_item = what_chosen % 1000;
			base_item = univ.party.magic_store_items[what_magic_shop][what_magic_shop_item];
			base_item.item_properties = base_item.item_properties | 1;
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
		SysBeep(50);
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
			base_item.item_properties = base_item.item_properties | 1;
			display_pc_item(6,0, base_item,0);
			break;
		case 5:
			display_help(0,0);
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
			base_item.item_properties = base_item.item_properties | 1;
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
				if ((univ.party[current_pc].equip[i] == true) && (univ.party[current_pc].items[i].is_cursed() == true))
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
			//	}
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
	SetControlMaximum(shop_sbar,i);
}

void start_talk_mode(short m_num,short personality,m_num_t monst_type,short store_face_pic)////
{
	Rect area_rect;
	Str255 place_string1 = "";
	Str255 place_string2 = "";
		
	store_personality = personality;
		
	store_monst_type = monst_type;
	store_m_num = m_num;
	store_talk_face_pic = store_face_pic; ////
	area_rect = talk_area_rect;
	OffsetRect(&area_rect, -1 * area_rect.left,-1 * area_rect.top);
	NewGWorld(&talk_gworld,  0 /*8*/,&area_rect, NULL, NULL, kNativeEndianPixMap);
	
	// first make sure relevant talk strs are loaded in
	if (personality / 10 != univ.town.cur_talk_loaded){
		if(personality / 10 == univ.town.num) univ.town.cur_talk = &univ.town->talking;
		else load_town(personality / 10,*univ.town.cur_talk);
		univ.town.cur_talk_loaded = personality / 10;
	}

	// load all possible responses
	store_responses();

	// Dredge up critter's name
	sprintf((char *) title_string,"%s:",univ.town.cur_talk->talk_strs[personality % 10]);
	
	store_pre_talk_mode = overall_mode;
	overall_mode = MODE_TALKING;
	create_clip_region();
	talk_end_forced = false;
	stat_screen_mode = 1;
	
	// Bring up and place first strings.
	sprintf((char *) place_string1,"%s",univ.town.cur_talk->talk_strs[personality % 10 + 10]);
	strnum1 = personality % 10 + 10;
	strnum2 = 0;
	
	strcpy((char *) old_str1,(char *) place_string1);
	strcpy((char *) old_str2,(char *) place_string2);
	strcpy((char *) one_back1,(char *) place_string1);
	strcpy((char *) one_back2,(char *) place_string2);
	place_talk_str((char *) place_string1,(char *) place_string2,0,dummy_rect);
	
	put_item_screen(stat_window,0);
	give_help(5,6,0);

}

void end_talk_mode()
{
	DisposeGWorld(talk_gworld);
	talk_gworld = NULL;
	
	overall_mode = store_pre_talk_mode;
	create_clip_region();
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
	redraw_screen();
}

void handle_talk_event(Point p)
{
	short i,j,force_special = 0,get_pc,s1 = -1,s2 = -1,s3 = -1;
	char asked[4];
	Str255 place_string1 = "";
	Str255 place_string2 = "";

	short a,b,c,d,ttype,which_talk_entry = -1;

	p.h -= 5;
	p.v -= 5;

	if (PtInRect(p,&talk_help_rect)) {
		univ.party.help_received[5] = 0;
		give_help(5,6,0);
		return;
		}

	for (i = 0; i < 9; i++)
		if ((PtInRect(p,&preset_words[i].word_rect)) && ((talk_end_forced == false) || (i == 6) || (i == 5))) {
			click_talk_rect((char *) old_str1,(char *) old_str2,preset_words[i].word_rect);
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
						SysBeep(20);
						return;
						}
					if(univ.party.has_talk_save(store_personality, strnum1, strnum2)){
					   ASB("This is already saved.");
					   print_buf();
					   return;
					} else {
						give_help(57,0,0);
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
			if ((PtInRect(p,&store_words[i].word_rect)) && (talk_end_forced == false)) {
				click_talk_rect((char *) old_str1,(char *) old_str2,store_words[i].word_rect);
				for (j = 0; j < 4; j++)
					asked[j] = store_words[i].word[j];
			
				i = 100;
				}
		}
	if (i == 50) // no event
		return;
	if (force_special == 9) {
		get_text_response(1017,place_string1,0);
		asked[0] = place_string1[0];
		asked[1] = place_string1[1];
		asked[2] = place_string1[2];
		asked[3] = place_string1[3];
		}
	
	if ((asked[0] == 'n') && (asked[1] == 'a') &&(asked[2] == 'm') &&(asked[3] == 'a')) {
		force_special = 2;
		}
	if ((asked[0] == 'l') && (asked[1] == 'o') &&(asked[2] == 'o') &&(asked[3] == 'k')) {
		force_special = 1;
		}
	if (((asked[0] == 'j') && (asked[1] == 'o') &&(asked[2] == 'b')) ||
		((asked[0] == 'w') && (asked[1] == 'o') &&(asked[2] == 'r')&&(asked[3] == 'k')) ) {
		force_special = 3;	
		}

	if (force_special > 0) {
		switch (force_special) {
			case 1: case 2: case 3:
				get_str(place_string1,120 + ((store_personality - 1) / 10),
				 ((store_personality - 1) % 10) * 3 + 10 + force_special);
				sprintf((char *) place_string1,"%s",univ.town.cur_talk->talk_strs[store_personality % 10 + 10 * force_special]);
					
				oldstrnum1 = strnum1; oldstrnum2 = strnum2;
				strnum1 =  store_personality % 10 + 10 * force_special;
				strnum2 = 0;
				strcpy((char *) one_back1,(char *) old_str1);
				strcpy((char *) one_back2,(char *) old_str2);
				strcpy((char *) old_str1,(char *) place_string1);
				strcpy((char *) old_str2,(char *) place_string2);
				place_talk_str((char *) place_string1,(char *) place_string2,0,dummy_rect);
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
				strcpy((char *) place_string1,(char *) one_back1);
				strcpy((char *) place_string2,(char *) one_back2);
				strcpy((char *) one_back1,(char *) old_str1);
				strcpy((char *) one_back2,(char *) old_str2);
				strcpy((char *) old_str1,(char *) place_string1);
				strcpy((char *) old_str2,(char *) place_string2);
				place_talk_str((char *) place_string1,(char *) place_string2,0,dummy_rect);
				return;
				break;
			}
		}
	
	which_talk_entry = scan_for_response(asked);
	if ((which_talk_entry < 0) || (which_talk_entry > 59)) {
		strcpy((char *) one_back1,(char *) old_str1);
		strcpy((char *) one_back2,(char *) old_str2);
		strcpy((char *) old_str2,"");
		sprintf((char *) old_str1,"%s",univ.town.cur_talk->talk_strs[store_personality % 10 + 160]);
		if (strlen((char *) old_str1) < 2)
			strcpy((char *) old_str1,"You get no response.");
		place_talk_str((char *) old_str1,(char *) old_str2,0,dummy_rect);
		strnum1 = -1;
		return;	
		}
	
	ttype = univ.town.cur_talk->talk_nodes[which_talk_entry].type;
	a = univ.town.cur_talk->talk_nodes[which_talk_entry].extras[0];
	b = univ.town.cur_talk->talk_nodes[which_talk_entry].extras[1];
	c = univ.town.cur_talk->talk_nodes[which_talk_entry].extras[2];
	d = univ.town.cur_talk->talk_nodes[which_talk_entry].extras[3];

	sprintf((char *) place_string1,"%s",univ.town.cur_talk->talk_strs[40 + which_talk_entry * 2]);
	sprintf((char *) place_string2,"%s",univ.town.cur_talk->talk_strs[40 + which_talk_entry * 2 + 1]);
	
	oldstrnum1 = strnum1; oldstrnum2 = strnum2;
	strnum1 =  40 + which_talk_entry * 2; strnum2 = 40 + which_talk_entry * 2 + 1;

	switch(ttype) {
		case 0:
			break;
		case 1:
			if (PSD[a][b] > c) {
				strnum1 = strnum2;
				strcpy((char *) place_string1,(char *) place_string2);
				}
			strcpy((char *) place_string2,"");					
			strnum2 = 0;
			break;
		case 2:
			PSD[a][b] = 1;
			break;
		case 3:
			if (univ.party.gold < a) {
				strnum1 = strnum2;
				strcpy((char *) place_string1,(char *) place_string2);
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
			strcpy((char *) place_string2,"");					
			break;
		case 4:
			if (day_reached((unsigned char) a,0) == true) {
				strnum1 = strnum2;
				strcpy((char *) place_string1,(char *) place_string2);
				}
			strcpy((char *) place_string2,"");					
			strnum2 = 0;
			break;
		case 5:
			if (day_reached((unsigned char) a,(unsigned char) b) == true) {
				strnum1 = strnum2;
				strcpy((char *) place_string1,(char *) place_string2);
				}
			strcpy((char *) place_string2,"");					
			strnum2 = 0;
			break;
		case 6:
			if (univ.town.num != a) {
				strnum1 = strnum2;
				strcpy((char *) place_string1,(char *) place_string2);
				}
			strcpy((char *) place_string2,"");					
			strnum2 = 0;
			break;
		case 7: 
			c = minmax(1,30,c);
			start_shop_mode(2,b,
				b + c - 1,a,(char *)place_string1);
			strnum1 = -1;
			return;
		case 8: 
			if ((get_pc = char_select_pc(1,0,"Train who?")) < 6) {
				strnum1 = -1;
				spend_xp(get_pc,1, 0);
				}
			sprintf((char *) place_string1, "You conclude your training.");  
			return;
		
		case 9: case 10: case 11: 
			c = minmax(1,30,c);
			start_shop_mode(ttype + 1,b,
				b + c - 1,a,(char *)place_string1);
			strnum1 = -1;
			return;
		case 12: //healer
			start_shop_mode(3,univ.town.monst[store_m_num].extra1,
				univ.town.monst[store_m_num].extra2,a,(char *)place_string1);
			strnum1 = -1;
			return;
			break;
		case 13: // sell weap
			strnum1 = -1;
			stat_screen_mode = 3;
			put_item_screen(stat_window,1);
			give_help(42,43,0);
			break;
		case 14: // sell armor
			strnum1 = -1;
			stat_screen_mode = 4;
			put_item_screen(stat_window,1);
			give_help(42,43,0);
			break;
		case 15: // sell misc
			strnum1 = -1;
			stat_screen_mode = 5;
			put_item_screen(stat_window,1);
			give_help(42,43,0);
			break;
		case 16: case 17: // ident enchant
				strnum1 = -1;
			stat_screen_mode = (ttype == 16) ? 2 : 6;
			shop_identify_cost = a;
			put_item_screen(stat_window,1);
			give_help(ttype - 16 + 44,0,0);
			break;
		case 18:
			if (univ.party.gold < a) {
				strnum1 = strnum2;
				strcpy((char *) place_string1,(char *) place_string2);
				}
				else {
					univ.party.gold -= a;
					put_pc_screen();

					}
			strcpy((char *) place_string2,"");					
			strnum2 = 0;
			break;
		case 19:
			if ((sd_legit(b,c) == true) && (PSD[b][c] == d)) {
				sprintf((char *) place_string1, "You've already learned that.");  
				strnum1 = -1;
				}
			else if (univ.party.gold < a) {
				strnum1 = strnum2;
				strcpy((char *) place_string1,(char *) place_string2);
				}
				else {
					univ.party.gold -= a;
					put_pc_screen();
					if (sd_legit(b,c) == true)
						PSD[b][c] = d;
						else give_error("Invalid Stuff Done flag called in conversation.","",0);
					}
			strnum2 = 0;
			strcpy((char *) place_string2,"");					
			break;
		case 20:
			if (univ.party.gold < a) {
				strnum1 = strnum2;
				strnum2 = 0;
				strcpy((char *) place_string1,(char *) place_string2);
				strcpy((char *) place_string2,"");					
				break;
				}
				else {
					for (i = b; i <= b + c; i++)
						if ((i >= 0) && (i < 30) && (univ.party.boats[i].property == true)) {
							univ.party.gold -= a;
							put_pc_screen();
							univ.party.boats[i].property = false;
							strcpy((char *) place_string2,"");					
							strnum2 = 0;
							i = 1000;
							}
					if (i >= 1000)
						break;
					}
			strcpy((char *) place_string1, "There are no boats left.");  
			strcpy((char *) place_string2,"");					
			strnum1 = -1;
			strnum2 = -1;
			break;
		case 21:
			if (univ.party.gold < a) {
				strnum1 = strnum2;
				strnum2 = 0;
				strcpy((char *) place_string1,(char *) place_string2);
				strcpy((char *) place_string2,"");					
				break;
				}
				else {
					for (i = b; i <= b + c; i++)
						if ((i >= 0) && (i < 30) && (univ.party.horses[i].property == true)) {
							univ.party.gold -= a;
							put_pc_screen();
							univ.party.horses[i].property = false;
							strcpy((char *) place_string2,"");					
							strnum2 = 0;
							i = 1000;
							}
					if (i >= 1000)
						break;
					}
			strcpy((char *) place_string1, "There are no horses left.");  
			strcpy((char *) place_string2,"");					
			strnum1 = -1;
			strnum2 = -1;
			break;
		case 22:
			if (univ.party.spec_items[a] > 0) {
				sprintf((char *) place_string1, "You already have it.");  
				strnum1 = -1;
				}
			else if (univ.party.gold < b) {
				strcpy((char *) place_string1,(char *) place_string2);
				strnum1 = strnum2;
				}
				else {
					univ.party.gold -= b;
					put_pc_screen();
					univ.party.spec_items[a] = 1;
					}
			strnum2 = 0;
			strcpy((char *) place_string2,"");					
			break;
		case 23:
			start_shop_mode(5 + b,0,
				9,a,(char *)place_string1);
			strnum1 = -1;
			return;
		case 24:		
			if (univ.party.can_find_town[b] > 0) {
				}
				else if (univ.party.gold < a) {
					strnum1 = strnum2;
					strcpy((char *) place_string1,(char *) place_string2);
					}
					else {
						univ.party.gold -= a;
						put_pc_screen();
						univ.party.can_find_town[b] = 1;
						}
			strnum2 = 0;
			strcpy((char *) place_string2,"");					
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
				strcpy((char *) place_string1,"");
				strcpy((char *) place_string2,"");
				}
			 get_strs((char *) place_string1,(char *) place_string2,2,s1,s2); 
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
				strcpy((char *) place_string1,"");
				strcpy((char *) place_string2,"");
				}
			 get_strs((char *) place_string1,(char *) place_string2,0,s1,s2); 
			 //strnum1 = -1;
			 //strnum2 = -1;
			 if (s1 >= 0) strnum1 = 3000 + s1;
			 if (s2 >= 0) strnum2 = 3000 + s2;
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;	
		
			
			
		}

	strcpy((char *) one_back1,(char *) old_str1);
	strcpy((char *) one_back2,(char *) old_str2);
	strcpy((char *) old_str1,(char *) place_string1);
	strcpy((char *) old_str2,(char *) place_string2);
	place_talk_str((char *) old_str1,(char *) old_str2,0,dummy_rect);
	
}

void handle_talk_spec(short ttype,Str255 place_string1,Str255 place_string2) ////
{
	short s1 = 1, s2 = 0;
	
	//switch (ttype) {}
	
	strnum1 = 1000 + s1;
	get_str(place_string1,15,s1);
	if (s2 > 0) {
		strnum2 = 1000 + s2;
		get_str(place_string2,15,s2);
		}
	print_buf();
	put_pc_screen();
}



void store_responses()
{

}


void put_num_in_text(short num_to_put,DialogPtr the_dialog,short item_num)
{
	Str255		text_entry;
	short	the_type;
	Handle	the_handle = NULL;
	Rect	the_rect;
	
	NumToString ((long) num_to_put , text_entry);
	GetDialogItem( the_dialog, item_num, &the_type, &the_handle, &the_rect );
	SetDialogItemText ( the_handle, text_entry);	
}

long get_text_item_num(short item_num,DialogPtr the_dialog)
{
	long storage;
	long		number_given = 0;

	short	the_type;
	Handle	the_handle = NULL;
	Rect	the_rect;
	Str255 	the_string;	
	
	GetDialogItem( the_dialog, item_num, &the_type, &the_handle, &the_rect );
	GetDialogItemText ( the_handle, the_string);
					
	StringToNum( the_string, &number_given);	
	storage = number_given;

	return storage;
}



void sign_event_filter (short item_hit)
{
	toast_dialog();
}

void do_sign(short town_num, short which_sign, short sign_type,location sign_loc)
//town_num; // Will be 0 - 200 for town, 200 - 290 for outdoors
//short sign_type; // terrain type
{

	short item_hit;
	Str255 sign_text;
	location view_loc;

	view_loc = (is_out()) ? univ.party.p_loc : univ.town.p_loc;
	make_cursor_sword();
	
	cd_create_dialog(1014,mainPtr);
	
	store_sign_mode = sign_type;
	if (terrain_pic[sign_type] < 1000)
		csp(1014,3,terrain_pic[sign_type],PICT_TER);
	else csp(1014,3,94,PICT_TER);
	
	if (town_num >= 200) {
		town_num -= 200;
		//load_outdoor_str(loc(town_num % scenario.out_width, town_num / scenario.out_width),which_sign + 100,(char *) sign_text);
		strcpy((char*)sign_text,univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].sign_strs[which_sign]);
		}
		else {
			sprintf((char *) sign_text,"%s",univ.town->town_strs(120 + which_sign));
			}
	csit(1014,2,(char *) sign_text);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(1014);
}



void give_reg_info_event_filter (short item_hit)
{
			switch (item_hit) {
				case 1: 
					toast_dialog();
					break;
				}
}

void give_reg_info()
{

	short item_hit;
	

	make_cursor_sword();

	cd_create_dialog_parent_num(1073,0);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(1073);

}




void do_registration_event_filter (short item_hit)
{
	Str255 get_text;
	
	//cd_retrieve_text_edit_str(1075,(char *) get_text);
	dialog_answer = 0;
#ifndef EXILE_BIG_GUNS
	sscanf((char *) get_text,"%hd",&dialog_answer);
#endif		
#ifdef EXILE_BIG_GUNS
	sscanf((char *) get_text,"%d",&dummy);
	dialog_answer = dummy;
#endif		
	toast_dialog();
}

//void do_registration()
//{
//
//	short item_hit;
//
//	make_cursor_sword();
//	
//	cd_create_dialog(1075,mainPtr);
//		
//	cdsin(1075,7,(short) register_flag);	
//	
//	item_hit = cd_run_dialog();
//	
//	cd_kill_dialog(1075,0);
//	
//	if (dialog_answer == 1)
//		SysBeep(30);
//	
//	if (dialog_answer == (short) init_data(register_flag)) {
//		play_sound(40);
//		//build_data_file(2);
//		FCD(1078,0);
//		registered = true;
//		save_prefs();
//		check_pref_file();
//		}
//		else {
//			SysBeep(30);
//			FCD(1077,0);
//			}
//	
//	
//}

void load_prefs(){
	Boolean b, valid;
	CFIndex x;
	b = CFPreferencesGetAppBooleanValue(CFSTR("GiveIntroHint"), CFSTR("com.spidweb.bladesofexile"), &valid);
	if(valid) give_intro_hint = b;
	x = CFPreferencesGetAppIntegerValue(CFSTR("DisplayMode"), CFSTR("com.spidweb.bladesofexile"), &valid);
	if(valid) display_mode = x;
	b = CFPreferencesGetAppBooleanValue(CFSTR("PlaySounds"), CFSTR("com.spidweb.bladesofexile"), &valid);
	if(valid) play_sounds = b;
	b = CFPreferencesGetAppBooleanValue(CFSTR("ShowStartupSplash"), CFSTR("com.spidweb.bladesofexile"), &valid);
	if(valid) show_startup_splash = b;
	b = CFPreferencesGetAppBooleanValue(CFSTR("GameRunBefore"), CFSTR("com.spidweb.bladesofexile"), &valid);
	if(valid) game_run_before = b;
	b = CFPreferencesGetAppBooleanValue(CFSTR("SkipBoomDelay"), CFSTR("com.spidweb.bladesofexile"), &valid);
	if(valid) skip_boom_delay = b;
}

//void check_pref_file() {
//	short num_tries = 0;
//	
//	while ((num_tries < 300) && (open_pref_file() == -100))
//		num_tries++;
//		
//	if (num_tries == 300) {
//		Alert(983,NULL);		
//		ExitToShell();
//		}	
//	}
	
//long open_pref_file()
//{
//	Str255 pref_name;
//	short vol_ref = 0;
//	long dir_ID = 0,stored_key;
//	FSSpec pref;
//	short file_ref_num;
//	PrefHandle data_handle;
//	
//	GetIndString(pref_name,5,19);
//	FindFolder(kOnSystemDisk,kPreferencesFolderType,
//			kDontCreateFolder,&vol_ref,&dir_ID);
//	FSMakeFSSpec(vol_ref,dir_ID,pref_name,&pref);
//	file_ref_num = FSpOpenResFile(&pref,fsCurPerm);
//	if (file_ref_num == -1) {
//		game_run_before = false;
//		save_prefs();
//		//make_pref_file(pref);
//		return -1;
//		}
//		
//	UseResFile(file_ref_num);
//	data_handle = (PrefHandle) Get1Resource('PRFN',128);
//	
//	if ((**data_handle).l[1] != 0)
//		ask_to_change_color = true;
//		else ask_to_change_color = false;
//	if ((**data_handle).l[2] != 0)
//		give_intro_hint = true;
//		else give_intro_hint = false;
//	display_mode = (short) ((**data_handle).l[3]);
//	play_sounds = (short) ((**data_handle).l[4]);
//	register_flag = (long) (800000) - (**data_handle).l[5];
//	stored_key = (long) (700000) - (**data_handle).l[7];
//	ed_flag = (long) (800000) - (**data_handle).l[6];
//	ed_key = (long) (700000) - (**data_handle).l[8];
//	
//	if ((registered == true) && (stored_key != init_data(register_flag))) {
//		display_mode = 0;
//		CloseResFile(file_ref_num);
//		save_prefs();
//		return -100;	
//		}
//	if ((stored_key != init_data(register_flag)) && ((register_flag < 10000) || (register_flag > 30000)
//		|| (display_mode < 0) || (display_mode > 5) || (ed_flag < 0) || (ed_flag > 10000)) ) {
//		registered = false;
//		if  ((register_flag < 10000) || (register_flag > 30000))
//			register_flag = get_ran(1,10000,30000);
//		if ((ed_flag < 0) || (ed_flag > 10000))
//			ed_flag = 10000;
//		
//		stored_key = 0;
//		display_mode = 0;
//		CloseResFile(file_ref_num);
//		save_prefs();
//		return -100;
//		}
//
//
//	CloseResFile(file_ref_num);
//	
//	return stored_key;
//	
//}

//void make_pref_file(FSSpec pref)
//{
//	short file_ref_num;
//	Handle app_handle;
//	short res_ID;
//	ResType res_type;
//	Str255 res_name = "xxx";
//	short res_attributes;
//
//	
////	rf = CurResFile();
////	UseResFile(rf);
////	app_handle = Get1Resource('PRFN',128);
//	
////	for (i = 0; i < 10; i++)
////		(**(PrefHandle)app_handle).l[i] = (long) (get_ran(1,-20000,20000)) * (i + 2);
////	(**(PrefHandle)app_handle).l[2] = (long) (give_intro_hint);
////	(**(PrefHandle)app_handle).l[3] = (long) (display_mode);
////	(**(PrefHandle)app_handle).l[4] = (long) (play_sounds);
//
////	if (register_flag == 0) {
////		register_flag = (long) get_ran(1,10000,20000);
////		ed_flag = get_ran(1,5000,9999);
////		}
//
//	// Amputating this code, cause it's broken, while save prefs code works OK
//	
////	(**(PrefHandle)app_handle).l[5] = (long) (800000) - register_flag;
////	if (registered == true)
////		(**(PrefHandle)app_handle).l[7] = (long) (700000) - init_data(register_flag);
//
////	(**(PrefHandle)app_handle).l[6] = (long) (800000) - ed_flag;
////	if (ed_reg == true)
////		(**(PrefHandle)app_handle).l[8] = (long) (700000) - init_data(ed_flag);
//
////	GetResInfo((Handle) app_handle,&res_ID,&res_type,res_name);
////	res_attributes = GetResAttrs(app_handle);
////	DetachResource(app_handle);
//	FSpCreateResFile(&pref,'blx!','BLPR',smSystemScript);
////	save_prefs();
//	return;
//
////	FSpCreateResFile(&pref,'RSED','rsrc',smSystemScript);
//	file_ref_num = FSpOpenResFile(&pref,fsCurPerm);
//
//	UseResFile(file_ref_num);
//	AddResource(app_handle,res_type,res_ID,res_name);
//	SetResAttrs(app_handle,res_attributes);
//	ChangedResource(app_handle);		
//	WriteResource(app_handle);		
//	ReleaseResource(app_handle);		
//	CloseResFile(file_ref_num);
//	
//}

void save_prefs(){
	CFPreferencesSetAppValue(
		CFSTR("GiveIntroHint"),
		give_intro_hint ? kCFBooleanTrue : kCFBooleanFalse,
		CFSTR("com.spidweb.bladesofexile")
	);
	CFPreferencesSetAppValue(
		CFSTR("DisplayMode"),
		CFNumberCreate(NULL, kCFNumberShortType, &display_mode),
		CFSTR("com.spidweb.bladesofexile")
	);
	CFPreferencesSetAppValue(
		CFSTR("PlaySounds"),
		play_sounds ? kCFBooleanTrue : kCFBooleanFalse,
		CFSTR("com.spidweb.bladesofexile")
	);
	CFPreferencesSetAppValue(
		CFSTR("ShowStartupSplash"),
		show_startup_splash ? kCFBooleanTrue : kCFBooleanFalse,
		CFSTR("com.spidweb.bladesofexile")
	);
	CFPreferencesSetAppValue(
		CFSTR("GameRunBefore"),
		game_run_before ? kCFBooleanTrue : kCFBooleanFalse,
		CFSTR("com.spidweb.bladesofexile")
	);
	CFPreferencesSetAppValue(
		CFSTR("SkipBoomDelay"),
		skip_boom_delay ? kCFBooleanTrue : kCFBooleanFalse,
		CFSTR("com.spidweb.bladesofexile")
	);
	
	bool success = CFPreferencesAppSynchronize (CFSTR("com.spidweb.bladesofexile"));
	if(!success){
		give_error("There was a problem writing to the preferences file. When the game is next run the preferences will revert to their previously set values.","Should you manage to resolve the problem without closing the program, simply open the preferences screen and click \"OK\" to try again.",0);
	}
}

//void save_prefs()
//{
//	short file_ref_num;
//	Handle old_handle,data_handle;
//	short vol_ref,i;
//	short res_ID;
//	long dir_ID;
//	ResType res_type = 'PRFN';
//	Str255 res_name;
//	short res_attributes;
//	FSSpec pref;
//	short app_res_num;
//	Str255 pref_name;
//	
//	app_res_num = CurResFile();
//	
//	GetIndString(pref_name,5,19);
//	if (sys_7_avail == true)
//		FindFolder(kOnSystemDisk,kPreferencesFolderType,
//			kDontCreateFolder,&vol_ref,&dir_ID);
//	FSMakeFSSpec(vol_ref,dir_ID,pref_name,&pref);
//	file_ref_num = FSpOpenResFile(&pref,fsCurPerm);
//	if (file_ref_num == -1) {
//		make_pref_file(pref);
//		file_ref_num = FSpOpenResFile(&pref,fsCurPerm);
////		return;
//		}
//		
//	UseResFile(file_ref_num);
//
//	data_handle = NewHandleClear(sizeof(PrefRecord));
//	HLock(data_handle);
//
//	for (i = 0; i < 10; i++)
//		(**(PrefHandle)data_handle).l[i] = (long) (get_ran(1,-20000,20000)) * (i + 2);
//	(**(PrefHandle)data_handle).l[1] = (long) (ask_to_change_color);
//	(**(PrefHandle)data_handle).l[2] = (long) (give_intro_hint);
//	(**(PrefHandle)data_handle).l[3] = (long) (display_mode);
//	(**(PrefHandle)data_handle).l[4] = (long) (play_sounds);
//	
//
//	if (register_flag == 0) {
//		register_flag = (long) get_ran(1,10000,20000);
//		ed_flag = get_ran(1,5000,9999);
//		}
//	(**(PrefHandle)data_handle).l[5] = (long) (800000) - register_flag;
//	(**(PrefHandle)data_handle).l[6] = (long) (800000) - ed_flag;
//	if (registered == true)
//		(**(PrefHandle)data_handle).l[7] = (long) (700000) - init_data(register_flag);
//	if (ed_reg == true)
//		(**(PrefHandle)data_handle).l[8] = (long) (700000) - init_data(ed_flag);
//	
//	old_handle = Get1Resource('PRFN',128);
//	GetResInfo(old_handle,&res_ID,&res_type,res_name);
//	res_attributes = GetResAttrs(old_handle);
//	RemoveResource(old_handle);
//	
//	AddResource(data_handle,'PRFN',128,res_name);
//	WriteResource(data_handle);
//	HUnlock(data_handle);
//	ReleaseResource(data_handle);
//
//	CloseResFile(file_ref_num);
//	UseResFile(app_res_num);
//}

void prefs_event_filter (short item_hit)
{
	bool done_yet = false,did_cancel = false;
	short i;
	
		switch (item_hit) {
			case 1:
				done_yet = true;
				toast_dialog();
				break;

			case 2:
				done_yet = true;
				toast_dialog();
				did_cancel = true;
				break;

			case 18: case 20: case 22: case 24: case 27: case 38: case 40: case 43: case 45:
				cd_set_led(1099,item_hit,1 - cd_get_led(1099,item_hit));
				break;
			
			case 29:
				cd_set_led(1099,29,1);
				break;
			
			case 32: case 34: case 36: case 47:
				cd_set_led(1099,32,(item_hit == 32) ? 1 : 0);
				cd_set_led(1099,34,(item_hit == 34) ? 1 : 0);
				cd_set_led(1099,36,(item_hit == 36) ? 1 : 0);
				cd_set_led(1099,47,(item_hit == 47) ? 1 : 0);
				break;
				
			
			default:
				cd_set_led(1099,4 + cur_display_mode,0);
				cur_display_mode = item_hit - 4;
				cd_set_led(1099,4 + cur_display_mode,1);
				break;
			}
	if (done_yet== true) {
		if (did_cancel == false) {
			display_mode = cur_display_mode;
			PSD[SDF_NO_MAPS] = cd_get_led(1099,18);
			PSD[SDF_NO_SOUNDS] = cd_get_led(1099,20);
			play_sounds = 1 - PSD[SDF_NO_SOUNDS];
			PSD[SDF_NO_FRILLS] = cd_get_led(1099,22);
			PSD[SDF_ROOM_DESCS_AGAIN] = cd_get_led(1099,24);
			PSD[SDF_NO_INSTANT_HELP] = cd_get_led(1099,27);
			PSD[SDF_EASY_MODE] = cd_get_led(1099,38);
			PSD[SDF_LESS_WANDER_ENC] = cd_get_led(1099,40);
			PSD[SDF_NO_TER_ANIM] = cd_get_led(1099,43);
			PSD[SDF_NO_SHORE_FRILLS] = cd_get_led(1099,45);
			//PSD[SDF_SKIP_STARTUP] = cd_get_led(1099,46);
			if (cd_get_led(1099,32) == 1)
				PSD[SDF_GAME_SPEED] = 0;
			else if (cd_get_led(1099,34) == 1)
				PSD[SDF_GAME_SPEED] = 1;
			else if (cd_get_led(1099,36) == 1)
				PSD[SDF_GAME_SPEED] = 2;
			else if (cd_get_led(1099,47) == 1)
				PSD[SDF_GAME_SPEED] = 3;
			if (cd_get_led(1099,29) == 1)
				for (i = 0; i < 120; i++)
					univ.party.help_received[i] = 0;
			/*if (display_mode < 5)
				max_window(mainPtr);
				else {
					GetWindowRect(GetDesktopWindow(),&windRECT);
					MoveWindow(mainPtr,(windRECT.right - (563 + 10)) / 2,
					(windRECT.bottom - (425 + 40)) / 2 ,
					563 + 10,425 + 40,true);
					}*/
			}
		//play_sounds = 1 - PSD[SDF_NO_SOUNDS];
		save_maps = 1 - PSD[SDF_NO_MAPS];
		give_delays = PSD[SDF_NO_FRILLS];
		save_prefs();
		}
}

void pick_preferences()
{
	short item_hit;

	cur_display_mode = display_mode;

	make_cursor_sword();

	cd_create_dialog(1099,mainPtr);

	cd_set_led(1099,4 + cur_display_mode,1);

	cd_set_led(1099,18,(PSD[SDF_NO_MAPS] != 0) ? 1 : 0);
	cd_set_led(1099,20,(play_sounds == false) ? 1 : 0);
	cd_set_led(1099,22,(PSD[SDF_NO_FRILLS] != 0) ? 1 : 0);
	cd_set_led(1099,24,(PSD[SDF_ROOM_DESCS_AGAIN] != 0) ? 1 : 0);
	cd_set_led(1099,27,(PSD[SDF_NO_INSTANT_HELP] != 0) ? 1 : 0);
	cd_set_led(1099,38,(PSD[SDF_EASY_MODE] != 0) ? 1 : 0);
	cd_set_led(1099,40,(PSD[SDF_LESS_WANDER_ENC] != 0) ? 1 : 0);
	cd_set_led(1099,43,(PSD[SDF_NO_TER_ANIM] != 0) ? 1 : 0);
	cd_set_led(1099,45,(PSD[SDF_NO_SHORE_FRILLS] != 0) ? 1 : 0);
	//cd_set_led(1099,46,(PSD[SDF_SKIP_STARTUP] != 0) ? 1 : 0);
	if (PSD[SDF_GAME_SPEED] == 3) 
		cd_set_led(1099,47,1);
	else cd_set_led(1099,32 + PSD[SDF_GAME_SPEED] * 2,1);

	if (univ.party.help_received[55] == 0) {
		cd_initial_draw(1099);
		give_help(55,0,1099);
	}
	
	item_hit = cd_run_dialog();

	cd_kill_dialog(1099);
	adjust_window_mode();
}

void put_party_stats()
{
	short i;


	for (i = 0; i < 6; i++) {
		if (univ.party[i].main_status > 0) {
			cd_set_item_text(989,5 + 5 * i,univ.party[i].name.c_str());
			cd_activate_item(989,7 + 5 * i,1);
			cd_activate_item(989,8 + 5 * i,1);
			cd_set_flag(989,6 + 5 * i,96);
			cd_activate_item(989,35 + i,1);
			cd_activate_item(989,42 + i,1);
			csp(989,42 + i,univ.party[i].which_graphic,PICT_PC);
		}
		else {
			cd_set_item_text(989,5 + 5 * i,"Empty.");
			cd_activate_item(989,7 + 5 * i,0);
			cd_activate_item(989,8 + 5 * i,0);
			cd_set_flag(989,6 + 5 * i,98);
			cd_activate_item(989,35 + i,0);
			cd_activate_item(989,42 + i,0);
		}

	}
	draw_startup(0);
}

void edit_party_event_filter (short item_hit)
{
	short which_pc;

		switch (item_hit) {
			case 1:
				toast_dialog();
				break;

			case 41:
				univ.party.help_received[22] = 0;
				give_help(222,23,989);
				return;

			case 5: case 10: case 15: case 20: case 25: case 30:
				which_pc = (item_hit - 5) / 5;
				if (univ.party[which_pc].main_status == 0) {
					//give_help(56,0,989);
					//create_pc(which_pc,989);
					//cd_initial_draw(989); // extra redraw, just in case
					}
					else pick_pc_name(which_pc,989);
				put_party_stats();
				break;
			case 7: case 12: case 17: case 22: case 27: case 32:
				which_pc = (item_hit - 7) / 5;
				pick_race_abil(&univ.party[which_pc],0,989);
				put_party_stats();
				break;
			case 8: case 13: case 18: case 23: case 28: case 33:
				which_pc = (item_hit - 8) / 5;
				spend_xp(which_pc,0,989);
				put_party_stats();
				break;
			case 35: case 36: case 37: case 38: case 39: case 40:
				which_pc = item_hit - 35;

				if (univ.party[which_pc].main_status <= 0)
					break;
				if (univ.party[which_pc].main_status != 0)
					pick_pc_graphic(which_pc,1,989);
				put_party_stats();
				break;
			case 6: case 11: case 16: case 21: case 26: case 31:
				which_pc = (item_hit - 6) / 5;
				if (univ.party[which_pc].main_status != MAIN_STATUS_ABSENT) {
					if (FCD(1053,989) == 2)
						univ.party[which_pc].main_status = MAIN_STATUS_ABSENT;
					put_party_stats();
					}
					else {
						give_help(56,0,989);
						create_pc(which_pc,989);
						put_party_stats();
						cd_initial_draw(989); // extra redraw, just in case
						}
				break;
//			case 7: case 12: case 17: case 22: case 27: case 32:

			}

}

void edit_party(short can_create,short can_cancel)
{
	short item_hit;
	bool munch_pc_graphic = false;
	
	make_cursor_sword();

	if (pcs_gworld == NULL) {
		munch_pc_graphic = true;
		pcs_gworld = load_pict(902);
		}	

	cd_create_dialog(989,mainPtr);

	put_party_stats();
	if (univ.party.help_received[22] == 0) {
		cd_initial_draw(989);
		give_help(22,23,989);
	}
	
	item_hit = cd_run_dialog();

	cd_kill_dialog(989);

	if (munch_pc_graphic == true) {
		DisposeGWorld(pcs_gworld);
		pcs_gworld = NULL;
		}

	if (univ.party[current_pc].main_status != 1)
		current_pc = first_active_pc();


}

void tip_of_day_event_filter (short item_hit)
{
	Str255 place_str;
	
			switch (item_hit) {
				case 1: 
					toast_dialog();
					break;
				
				case 5:
					store_tip_page_on++;
					if (store_tip_page_on == NUM_HINTS)
						store_tip_page_on = 0;
					get_str(place_str,12,50 + store_tip_page_on);
					csit(958,3,(char *) place_str);
					break;

				case 7:
					give_intro_hint = 1 - give_intro_hint;
					cd_set_led(958,7,give_intro_hint);
					break;
			}

}

void tip_of_day()
{

	short item_hit;
	Str255 place_str;
	
	store_tip_page_on = get_ran(1,0,NUM_HINTS - 1);
	
	make_cursor_sword();

	cd_create_dialog_parent_num(958,0);

	get_str(place_str,12,50 + store_tip_page_on);
	csit(958,3,(char *) place_str);

	cd_set_led(958,7,give_intro_hint);
	
	item_hit = cd_run_dialog();	
	cd_kill_dialog(958);
	save_prefs();

}

void put_scen_info()
{
	unsigned int i;
	std::ostringstream sout;
	char *ratings[] = {"G","PG","R","NC-17"};
	char *difficulty[] = {"Low","Medium","High","Very High"};

	for (i = 0; i < 3; i++)
		if (scen_headers.size() > (store_scen_page_on * 3 + i) && scen_headers.data(store_scen_page_on * 3 + i).flag1 != 0) {
			cd_set_pict(947, 6 + i * 3,scen_headers.data(store_scen_page_on * 3 + i).intro_pic,PICT_SCEN);
			sout.str("");
			sout << scen_headers.strs(store_scen_page_on * 3 + i).name;
			sout << " v" << scen_headers.data(store_scen_page_on * 3 + i).ver[0];
			sout << '.' << scen_headers.data(store_scen_page_on * 3 + i).ver[1];
			sout << '.' << scen_headers.data(store_scen_page_on * 3 + i).ver[2];
			sout << " - |  Difficulty: " << difficulty[scen_headers.data(store_scen_page_on * 3 + i).difficulty];
			sout << ", Rating: " << ratings[scen_headers.data(store_scen_page_on * 3 + i).rating];
			sout << " |" << scen_headers.strs(store_scen_page_on * 3 + i).who1;
			sout << " |" << scen_headers.strs(store_scen_page_on * 3 + i).who2;
			csit(947,7 + i * 3,(char *) sout.str().c_str());
			cd_activate_item(947,8 + i * 3,1);			
			}
			else {
				cd_set_pict(947, 6 + i * 3,0,PICT_BLANK);
				csit(947,7 + i * 3,"");
				cd_activate_item(947,8 + i * 3,0);
				}
}

void pick_a_scen_event_filter (short item_hit)
{
	
			switch (item_hit) {
				case 1: 
					dialog_answer = -1;
					toast_dialog();
					break;
				
				case 3: case 4:
					if (item_hit == 3) {
						if (store_scen_page_on == 0)
							store_scen_page_on = (store_num_scen - 1) / 3;
							else store_scen_page_on--;
						}
						else {
							if (store_scen_page_on == (store_num_scen - 1) / 3)
								store_scen_page_on = 0;
								else store_scen_page_on++;
							}					
					put_scen_info();
					break;
				
				case 8: case 11: case 14:
					dialog_answer = ((item_hit - 8) / 3) + store_scen_page_on * 3;
					toast_dialog();
					break;
				}

}

short pick_a_scen()
{

	short item_hit;
	
	build_scen_headers();
	
	store_num_scen = scen_headers.size();
//	for (i = 0; i < 25; i++)
//		if (scen_headers[i].flag1 != 0)
//			store_num_scen++;
	store_scen_page_on = 0;
	
	if (store_num_scen == 0) {
		FCD(868,0);
		return -1;
	}
	make_cursor_sword();
	
	cd_create_dialog_parent_num(947,0);

	put_scen_info();
	
	if (store_num_scen <= 3) {
		cd_activate_item(947,3,0);
		cd_activate_item(947,4,0);
	}
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(947);
	return dialog_answer;
}


void pick_prefab_scen_event_filter (short item_hit)
{
	
			switch (item_hit) {
				case 1: 
					dialog_answer = -1;
					toast_dialog();
					break;
				
				case 6: case 9: case 12:
					dialog_answer = ((item_hit - 6) / 3);
					toast_dialog();
					break;
				}

}

short pick_prefab_scen()
{

	short item_hit;
	
	make_cursor_sword();

	cd_create_dialog_parent_num(869,0);
	cd_activate_item(869,2,0);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(869);
	return dialog_answer;
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
//	Str255 temp_str;
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
