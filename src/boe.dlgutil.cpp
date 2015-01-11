
#include <cstring>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"

#include "boe.dlgutil.h"
#include "boe.text.h"
#include "boe.town.h"
#include "boe.itemdata.h"
#include "boe.locutils.h"
#include "boe.party.h"
#include "boe.specials.h"
#include "boe.fileio.h"
#include "boe.graphics.h"
#include "boe.townspec.h"
#include "boe.main.h"
#include "boe.items.h"
#include "soundtool.hpp"
#include <cstdio>
#include "boe.newgraph.h"
#include "boe.infodlg.h"
#include "graphtool.hpp"
#include "mathutil.hpp"
#include "dlogutil.hpp"
#include "winutil.hpp"
#include "fileio.hpp"
#include "restypes.hpp"
#include "scrollbar.hpp"
#include "button.hpp"
#include "pict.hpp"
#include <boost/lexical_cast.hpp>
#include "prefs.hpp"
#include "shop.hpp"
#define	NUM_HINTS	30

//extern big_tr_type t_d;
extern short stat_window;
extern eGameMode overall_mode;
//extern current_town_type univ.town;
//extern party_record_type party;
extern bool play_sounds,give_intro_hint,show_startup_splash,changed_display_mode;
extern sf::RenderWindow mainPtr;
extern rectangle d_rects[80];
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
eGameMode store_pre_talk_mode;
short store_personality,store_personality_graphic,shop_identify_cost;
std::string save_talk_str1, save_talk_str2;
sf::RenderTexture talk_gworld;
bool talk_end_forced;
rectangle talk_area_rect = {5,5,420,284}, word_place_rect = {44,7,372,257},talk_help_rect = {5,254,21,272};
std::string title_string;
m_num_t store_monst_type;
short store_m_num;
rectangle dummy_rect = {0,0,0,0};
//hold_responses store_resp[83];
short strnum1,strnum2,oldstrnum1,oldstrnum2;
short store_talk_face_pic;//,cur_town_talk_loaded = -1;
int current_talk_node;
extern std::vector<word_rect_t> talk_words;

// Shopping vars

// 1 - 499 ... regular items
// 500 alchemy
// 600-620 ... food not used!!!
// 700+ i - that healing
// 800 + - mage spells
// 900 + - priest spells
// n000 + i - magic store n item i
// talk_area_rect and talk_help_rect used for this too
eGameMode store_pre_shop_mode;
// 0 - whole area, 1 - active area 2 - graphic 3 - item name
// 4 - item cost 5 - item extra str  6 - item help button
extern rectangle shopping_rects[8][7];
// TODO: The duplication of rectangle here shouldn't be necessary...
rectangle bottom_help_rects[4] = {rectangle{356,6,368,250},rectangle{374,6,386,270},rectangle{386,6,398,250},rectangle{398,6,410,250}};
rectangle shop_name_str = {44,6,56,200};
rectangle shop_frame = {62,10,352,269};
rectangle shop_done_rect = {388,212,411,275};

short store_scen_page_on,store_num_scen;

cShop active_shop;

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
void start_shop_mode(eShopType shop_type,short shop_min,short shop_max,short cost_adj,std::string store_name) {
	rectangle area_rect;
	
	// This would be a place to hide the text box, if I add it.
	
	active_shop = cShop(shop_type, cost_adj, store_name);
	
	area_rect = talk_area_rect;
	talk_gworld.create(area_rect.width(), area_rect.height());
	
	store_pre_shop_mode = overall_mode;
	overall_mode = MODE_SHOPPING;
	stat_screen_mode = 1;
	
	set_up_shop_array(shop_type, shop_min, std::max(shop_min, shop_max));
	put_background();
	
	draw_shop_graphics(0,area_rect);
	
	put_item_screen(stat_window,0);
	give_help(26,27);
}

static void update_last_talk(int new_node) {
	// Store last node in the Go Back button
	for(word_rect_t& word : talk_words) {
		if(word.word != "Go Back") continue;
		word.node = current_talk_node;
		current_talk_node = new_node;
		break;
	}
}

void end_shop_mode() {
	rectangle dummy_rect = {0,0,0,0};
	
	// This would be a place to show the text box, if I add it (and if this is not an outdoor shop).
	
	shop_sbar->hide();
	if(store_pre_shop_mode == 20) {
		strnum1 = strnum2 = oldstrnum1 = oldstrnum2 = 0;
		place_talk_str("You conclude your business.", "", 0, dummy_rect);
		update_last_talk(TALK_BUSINESS);
	}
	
	overall_mode = store_pre_shop_mode;
	if(overall_mode == MODE_TALK_TOWN)
		overall_mode = MODE_TOWN;
	if(overall_mode == MODE_TOWN) {
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

void handle_shop_event(location p) {
	short i;
	unsigned long store_what_picked;
	
	p.x -= 5;
	p.y -= 5;
	
	if(p.in(talk_help_rect)) {
		click_shop_rect(talk_help_rect);
		univ.party.help_received[26] = 0;
		give_help(26,27);
		return;
	}
	if(p.in(shop_done_rect)) {
		click_shop_rect(shop_done_rect);
		end_shop_mode();
		return;
	}
	
	for(i = 0; i < 8; i++) {
		store_what_picked = i + shop_sbar->getPosition();
		if(store_what_picked >= active_shop.size()) break;
		if(p.in(shopping_rects[i][1])) {
			click_shop_rect(shopping_rects[i][1]);
			handle_sale(active_shop.getItem(store_what_picked), store_what_picked);
		} else if(p.in(shopping_rects[i][6]) && active_shop.getType() != eShopType::HEALING){
			click_shop_rect(shopping_rects[i][6]);
			handle_info_request(active_shop.getItem(store_what_picked));
		}
	}
}

void handle_sale(cShopItem item, int i) {
	cItemRec base_item = item.item;
	short cost = item.cost;
	rectangle dummy_rect = {0,0,0,0};
	
	switch(item.type) {
		case eShopItemType::ITEM:
			switch(pc_ok_to_buy(current_pc,cost,base_item)) {
				case 1:
					play_sound(-38);
					give_to_pc(current_pc,base_item,true);
					if(active_shop.getType() != eShopType::ITEMS) {
						// Magic shops have limited stock
						active_shop.clearItem(i);
						shop_sbar->setMaximum(shop_sbar->getMaximum() - 1);
					}
					break;
				case 2: ASB("Can't carry any more items."); break;
				case 3: ASB("Not enough cash."); break;
				case 4: ASB("Item is too heavy."); break;
				case 5: ASB("You own too many of this."); break;
			}
			break;
		case eShopItemType::ALCHEMY:
			if(univ.party.alchemy[base_item.item_level])
				ASB("You already know that recipe.");
			else if(!take_gold(cost,false))
				ASB("Not enough gold.");
			else {
				// TODO: This seems like the wrong sound
				play_sound(62);
				ASB("You buy an alchemical recipe.");
				univ.party.alchemy[base_item.item_level] = true;
			}
			break;
		case eShopItemType::FOOD:
			// TODO: This shop type is never actually used. Should it be deleted or restored?
			if(!take_gold(cost,false))
				ASB("Not enough gold.");
			else {
				play_sound(-38);
				ASB("You buy food.");
				univ.party.food += base_item.item_level;
			}
			break;
		default:
			if(!take_gold(cost,false))
				ASB("Not enough gold.");
			else {
				ASB("You pay the healer.");
				play_sound(68);
				switch(item.type) {
					case eShopItemType::HEAL_WOUNDS:
						univ.party[current_pc].cur_health = univ.party[current_pc].max_health;
						break;
					case eShopItemType::CURE_POISON:
						univ.party[current_pc].status[eStatus::POISON] = 0;
						break;
					case eShopItemType::CURE_DISEASE:
						univ.party[current_pc].status[eStatus::DISEASE] = 0;
						break;
					case eShopItemType::CURE_PARALYSIS:
						univ.party[current_pc].status[eStatus::PARALYZED] = 0;
						break;
					case eShopItemType::REMOVE_CURSE:
						for(int i = 0; i < 24; i++)
							if((univ.party[current_pc].equip[i]) &&
								(univ.party[current_pc].items[i].cursed))
								univ.party[current_pc].items[i].cursed = univ.party[current_pc].items[i].unsellable = false;
						break;
					case eShopItemType::DESTONE: case eShopItemType::RAISE_DEAD: case eShopItemType::RESURRECT:
						univ.party[current_pc].main_status = eMainStatus::ALIVE;
						break;
					case eShopItemType::CURE_DUMBFOUNDING:
						univ.party[current_pc].status[eStatus::DUMB] = 0;
						break;
				}
				// Once you've been healed, of course you're no longer eligible for that form of healing.
				active_shop.clearItem(i);
			}
			break;
		case eShopItemType::MAGE_SPELL:
			if((base_item.item_level < 0) || (base_item.item_level > 61)) {
				beep();
				giveError("Error: The scenario tried to sell you an invalid mage spell!");
				break;
			}
			if(univ.party[current_pc].mage_spells[base_item.item_level])
				ASB("You already have this spell.");
			else if(!take_gold(cost,false))
				ASB("Not enough gold.");
			else {
				// TODO: This seems like the wrong sound
				play_sound(62);
				ASB("You buy a spell.");
				univ.party[current_pc].mage_spells[base_item.item_level] = true;
				give_help(41,0);
			}
			break;
		case eShopItemType::PRIEST_SPELL:
			if((base_item.item_level < 0) || (base_item.item_level > 61)) {
				beep();
				giveError("Error: The scenario tried to sell you an invalid priest spell!");
				break;
			}
			if(univ.party[current_pc].priest_spells[base_item.item_level])
				ASB("You already have this spell.");
			else if(!take_gold(cost,false))
				ASB("Not enough gold.");
			else {
				// TODO: This seems like the wrong sound
				play_sound(62);
				ASB("You buy a spell.");
				univ.party[current_pc].priest_spells[base_item.item_level] = true;
				give_help(41,0);
			}
			break;
	}
	
	if(overall_mode != MODE_SHOPPING) {
		beep();
		ASB("Shop error 1. Report This!");
	}
	draw_shop_graphics(0,dummy_rect);
	print_buf();
	put_pc_screen();
	put_item_screen(stat_window,0);
}


void handle_info_request(cShopItem item) {
	cItemRec base_item = item.item;
	
	switch(item.type) {
		case eShopItemType::ITEM:
		case eShopItemType::FOOD:
			display_pc_item(6,0, base_item,0);
			break;
		case eShopItemType::ALCHEMY:
			// TODO: Create a dedicated dialog for alchemy info
			display_alchemy();
			break;
		case eShopItemType::MAGE_SPELL:
			display_spells(eSkill::MAGE_SPELLS,base_item.item_level - 800 - 30,0);
			break;
		case eShopItemType::PRIEST_SPELL:
			display_spells(eSkill::PRIEST_SPELLS,base_item.item_level - 900 - 30,0);
			break;
	}
}

void set_up_shop_array(eShopType store_shop_type, short store_shop_min, short store_shop_max) {
	bool cursed_item = false;
	active_shop.clear();
	
	switch(store_shop_type) {
		case eShopType::ITEMS:
			for(int i = store_shop_min; i <= store_shop_max; i++)
				active_shop.addItem(get_stored_item(i));
			break;
		case eShopType::HEALING:
			if(univ.party[current_pc].cur_health < univ.party[current_pc].max_health)
				active_shop.addSpecial(eShopItemType::HEAL_WOUNDS);
			if(univ.party[current_pc].status[eStatus::POISON] > 0)
				active_shop.addSpecial(eShopItemType::CURE_POISON);
			if(univ.party[current_pc].status[eStatus::DISEASE] > 0)
				active_shop.addSpecial(eShopItemType::CURE_DISEASE);
			if(univ.party[current_pc].status[eStatus::PARALYZED] > 0)
				active_shop.addSpecial(eShopItemType::CURE_PARALYSIS);
			if(univ.party[current_pc].status[eStatus::DUMB] > 0)
				active_shop.addSpecial(eShopItemType::CURE_DUMBFOUNDING);
			for(int i = 0; i < 24; i++)
				if((univ.party[current_pc].equip[i]) && (univ.party[current_pc].items[i].cursed))
					cursed_item = true;
			if(cursed_item) active_shop.addSpecial(eShopItemType::REMOVE_CURSE);
			if(univ.party[current_pc].main_status == eMainStatus::STONE)
				active_shop.addSpecial(eShopItemType::DESTONE);
			if(univ.party[current_pc].main_status == eMainStatus::DEAD)
				active_shop.addSpecial(eShopItemType::RAISE_DEAD);
			if(univ.party[current_pc].main_status == eMainStatus::DUST)
				active_shop.addSpecial(eShopItemType::RESURRECT);
			break;
		case eShopType::FOOD:
			// TODO: This shop type is never actually used. Should it be deleted or resurrected?
			for(int i = store_shop_min; i <= store_shop_max; i++)
				active_shop.addSpecial(eShopItemType::FOOD, i);
			break;
		case eShopType::MAGIC_JUNK:
			active_shop.addItems(univ.party.magic_store_items[0].begin(), univ.party.magic_store_items[0].end());
			break;
		case eShopType::MAGIC_LOUSY:
			active_shop.addItems(univ.party.magic_store_items[1].begin(), univ.party.magic_store_items[1].end());
			break;
		case eShopType::MAGIC_SO_SO:
			active_shop.addItems(univ.party.magic_store_items[2].begin(), univ.party.magic_store_items[2].end());
			break;
		case eShopType::MAGIC_GOOD:
			active_shop.addItems(univ.party.magic_store_items[3].begin(), univ.party.magic_store_items[3].end());
			break;
		case eShopType::MAGIC_GREAT:
			active_shop.addItems(univ.party.magic_store_items[4].begin(), univ.party.magic_store_items[4].end());
			break;
		case eShopType::MAGE:
			for(int i = store_shop_min; i <= store_shop_max && i < 62; i++)
				active_shop.addSpecial(eShopItemType::MAGE_SPELL, i);
			break;
		case eShopType::PRIEST:
			for(int i = store_shop_min; i <= store_shop_max && i < 62; i++)
				active_shop.addSpecial(eShopItemType::PRIEST_SPELL, i);
			break;
		case eShopType::ALCHEMY:
			for(int i = store_shop_min; i <= store_shop_max && i < 20; i++)
				active_shop.addSpecial(eShopItemType::ALCHEMY, i);
			break;
	}
	shop_sbar->setMaximum(active_shop.size() - 8);
}

void start_talk_mode(short m_num,short personality,m_num_t monst_type,short store_face_pic) {
	rectangle area_rect;
	std::string place_string1;
	
	store_personality = personality;
	
	store_monst_type = monst_type;
	store_m_num = m_num;
	store_talk_face_pic = store_face_pic; ////
	area_rect = talk_area_rect;
	talk_gworld.create(area_rect.width(), area_rect.height());
	
	// This would be the place to show the text box, if I add it.
	
	// Set the current town for talk strings
	univ.town.prep_talk(personality / 10);
	
	// Dredge up critter's name
	title_string = std::string(univ.town.cur_talk().people[personality % 10].title) + ":";
	
	store_pre_talk_mode = overall_mode;
	overall_mode = MODE_TALKING;
	talk_end_forced = false;
	stat_screen_mode = 1;
	current_talk_node = TALK_LOOK;
	
	// Bring up and place first strings.
	place_string1 = univ.town.cur_talk().people[personality % 10].look;
	strnum1 = personality % 10 + 10;
	strnum2 = 0;
	
	place_talk_str(place_string1, "", 0, dummy_rect);
	
	put_item_screen(stat_window,0);
	give_help(5,6);
	
}

void end_talk_mode() {
	// This would be where to hide the text box, if I add it.
	overall_mode = store_pre_talk_mode;
	if(overall_mode == MODE_TALK_TOWN)
		overall_mode = MODE_TOWN;
	if(overall_mode == MODE_TOWN) {
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

void handle_talk_event(location p) {
	short i,get_pc,s1 = -1,s2 = -1,s3 = -1;
	char asked[4];
	
	short a,b,c,d,ttype;
	
	p.x -= 5;
	p.y -= 5;
	
	if(p.in(talk_help_rect)) {
		univ.party.help_received[5] = 0;
		give_help(5,6);
		return;
	}
	
	int which_talk_entry = TALK_DUNNO;
	for(word_rect_t& word : talk_words) {
		if(word.node == -1) continue;
		if(!p.in(word.rect)) continue;
		click_talk_rect(word);
		which_talk_entry = word.node;
		break;
	}
	if(which_talk_entry == TALK_DUNNO)
		return;
	
	switch(which_talk_entry) {
		case TALK_DUNNO:
		SPECIAL_DUNNO:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].dunno;
			if(save_talk_str1.length() < 2) save_talk_str1 = "You get no response.";
			place_talk_str(save_talk_str1, "", 0, dummy_rect);
			update_last_talk(TALK_DUNNO);
			return;
		case TALK_BUSINESS: // This one only reachable via "go back".
			place_talk_str("You conclude your business.", "", 0, dummy_rect);
			update_last_talk(TALK_BUSINESS);
			return;
		case TALK_LOOK:
		SPECIAL_LOOK:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].look;
			place_talk_str(save_talk_str1, "", 0, dummy_rect);
			update_last_talk(TALK_LOOK);
			return;
		case TALK_NAME:
		SPECIAL_NAME:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].name;
			place_talk_str(save_talk_str1, "", 0, dummy_rect);
			update_last_talk(TALK_NAME);
			return;
		case TALK_JOB:
		SPECIAL_JOB:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].job;
			place_talk_str(save_talk_str1, "", 0, dummy_rect);
			update_last_talk(TALK_JOB);
			return;
		case TALK_BUY:
		SPECIAL_BUY:
			which_talk_entry = scan_for_response("purc");
			if(which_talk_entry < 0) which_talk_entry = scan_for_response("sale");
			if(which_talk_entry < 0) which_talk_entry = scan_for_response("heal");
			if(which_talk_entry < 0) which_talk_entry = scan_for_response("iden");
			if(which_talk_entry < 0) which_talk_entry = scan_for_response("trai");
			if(which_talk_entry == -1) goto SPECIAL_DUNNO;
			break;
		case TALK_SELL:
		SPECIAL_SELL:
			which_talk_entry = scan_for_response("sell");
			if(which_talk_entry == -1) goto SPECIAL_DUNNO;
			break;
		case TALK_RECORD:
			if(strnum1 <= 0) {
				beep();
				return;
			}
			if(univ.party.save_talk(univ.town->talking.people[store_personality].title, univ.town->town_name, save_talk_str1, save_talk_str2)) {
				give_help(57,0);
				play_sound(0);
				ASB("Noted in journal.");
			} else ASB("This is already saved.");
			print_buf();
			return;
		case TALK_DONE:
		SPECIAL_DONE:
			end_talk_mode();
			return;
		case TALK_BACK: // only if there's nothing to go back to
			return; // so, there's nothing to do here
		case TALK_ASK: // ask about
			save_talk_str1 = get_text_response("Ask about what?", 8);
			strncpy(asked, save_talk_str1.c_str(), 4);
			if(strnicmp(asked, "name", 4) == 0) goto SPECIAL_NAME;
			if(strnicmp(asked, "look", 4) == 0) goto SPECIAL_LOOK;
			if(strnicmp(asked, "job", 3) == 0)  goto SPECIAL_JOB;
			if(strnicmp(asked, "work", 4) == 0) goto SPECIAL_JOB;
			if(strnicmp(asked, "bye", 3) == 0)  goto SPECIAL_DONE;
			if(strnicmp(asked, "buy", 3) == 0)  goto SPECIAL_BUY;
			if(strnicmp(asked, "sell", 4) == 0) goto SPECIAL_SELL;
			which_talk_entry = scan_for_response(asked);
			if(which_talk_entry == -1) goto SPECIAL_DUNNO;
			break;
	}
	update_last_talk(which_talk_entry);
	
	ttype = univ.town.cur_talk().talk_nodes[which_talk_entry].type;
	a = univ.town.cur_talk().talk_nodes[which_talk_entry].extras[0];
	b = univ.town.cur_talk().talk_nodes[which_talk_entry].extras[1];
	c = univ.town.cur_talk().talk_nodes[which_talk_entry].extras[2];
	d = univ.town.cur_talk().talk_nodes[which_talk_entry].extras[3];
	
	save_talk_str1 = univ.town.cur_talk().talk_nodes[which_talk_entry].str1;
	save_talk_str2 = univ.town.cur_talk().talk_nodes[which_talk_entry].str2;
	
	oldstrnum1 = strnum1; oldstrnum2 = strnum2;
	strnum1 =  40 + which_talk_entry * 2; strnum2 = 40 + which_talk_entry * 2 + 1;
	
	switch(ttype) {
		case 0:
			break;
		case 1:
			if(PSD[a][b] > c) {
				strnum1 = strnum2;
				save_talk_str1 = save_talk_str2;
			}
			save_talk_str2 = "";
			strnum2 = 0;
			break;
		case 2:
			PSD[a][b] = 1;
			break;
		case 3:
			if(univ.party.gold < a) {
				strnum1 = strnum2;
				save_talk_str1 = save_talk_str2;
			}
			else {
				talk_end_forced = true;
				univ.party.gold -= a;
				put_pc_screen();
				do_rest(700, 30 & b, 25 * b);
				univ.town.p_loc.x = c;
				univ.town.p_loc.y = d;
				center = univ.town.p_loc;
			}
			strnum2 = 0;
			save_talk_str2 = "";
			break;
		case 4:
			if(day_reached((unsigned char) a,0)) {
				strnum1 = strnum2;
				save_talk_str1 = save_talk_str2;
			}
			save_talk_str2 = "";
			strnum2 = 0;
			break;
		case 5:
			if(day_reached((unsigned char) a,(unsigned char) b)) {
				strnum1 = strnum2;
				save_talk_str1 = save_talk_str2;
			}
			save_talk_str2 = "";
			strnum2 = 0;
			break;
		case 6:
			if(univ.town.num != a) {
				strnum1 = strnum2;
				save_talk_str1 = save_talk_str2;
			}
			save_talk_str2 = "";
			strnum2 = 0;
			break;
		case 7:
			c = minmax(1,30,c);
			start_shop_mode(eShopType::ITEMS,b,b + c - 1,a,save_talk_str1.c_str());
			strnum1 = -1;
			return;
		case 8:
			if((get_pc = char_select_pc(1,0,"Train who?")) < 6) {
				strnum1 = -1;
				spend_xp(get_pc,1, NULL);
			}
			save_talk_str1 = "You conclude your training.";
			return;
			
		case 9:
			c = minmax(1,61,c);
			start_shop_mode(eShopType::MAGE,b,b + c - 1,a,save_talk_str1.c_str());
			strnum1 = -1;
			return;
		case 10:
			c = minmax(1,61,c);
			start_shop_mode(eShopType::PRIEST,b,b + c - 1,a,save_talk_str1.c_str());
			strnum1 = -1;
			return;
		case 11:
			c = minmax(1,19,c);
			start_shop_mode(eShopType::ALCHEMY,b,b + c - 1,a,save_talk_str1.c_str());
			strnum1 = -1;
			return;
		case 12: //healer
			// TODO: extra1 and extra2 are actually never used! So remove them.
			start_shop_mode(eShopType::HEALING,univ.town.monst[store_m_num].extra1,
							univ.town.monst[store_m_num].extra2,a,save_talk_str1.c_str());
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
			if(univ.party.gold < a) {
				strnum1 = strnum2;
				save_talk_str1 = save_talk_str2;
			}
			else {
				univ.party.gold -= a;
				put_pc_screen();
				
			}
			save_talk_str2 = "";
			strnum2 = 0;
			break;
		case 19:
			if((sd_legit(b,c)) && (PSD[b][c] == d)) {
				save_talk_str1 = "You've already learned that.";
				strnum1 = -1;
			}
			else if(univ.party.gold < a) {
				strnum1 = strnum2;
				save_talk_str1 + save_talk_str2;
			}
			else {
				univ.party.gold -= a;
				put_pc_screen();
				if(sd_legit(b,c))
					PSD[b][c] = d;
				else giveError("Invalid Stuff Done flag called in conversation.");
			}
			strnum2 = 0;
			save_talk_str2 = "";
			break;
		case 20:
			if(univ.party.gold < a) {
				strnum1 = strnum2;
				strnum2 = 0;
				save_talk_str1 = save_talk_str2;
				save_talk_str2 = "";
				break;
			}
			else {
				for(i = b; i <= b + c; i++)
					if((i >= 0) && (i < 30) && (univ.party.boats[i].property)) {
						univ.party.gold -= a;
						put_pc_screen();
						univ.party.boats[i].property = false;
						save_talk_str2 = "";
						strnum2 = 0;
						i = 1000;
					}
				if(i >= 1000)
					break;
			}
			save_talk_str1 = "There are no boats left.";
			save_talk_str2 = "";
			strnum1 = -1;
			strnum2 = -1;
			break;
		case 21:
			if(univ.party.gold < a) {
				strnum1 = strnum2;
				strnum2 = 0;
				save_talk_str1 = save_talk_str2;
				save_talk_str2 = "";
				break;
			}
			else {
				for(i = b; i <= b + c; i++)
					if((i >= 0) && (i < 30) && (univ.party.horses[i].property)) {
						univ.party.gold -= a;
						put_pc_screen();
						univ.party.horses[i].property = false;
						save_talk_str2 = "";
						strnum2 = 0;
						i = 1000;
					}
				if(i >= 1000)
					break;
			}
			save_talk_str1 = "There are no horses left.";
			save_talk_str2 = "";
			strnum1 = -1;
			strnum2 = -1;
			break;
		case 22:
			if(univ.party.spec_items[a] > 0) {
				save_talk_str1 = "You already have it.";
				strnum1 = -1;
			}
			else if(univ.party.gold < b) {
				save_talk_str1 = save_talk_str2;
				strnum1 = strnum2;
			}
			else {
				univ.party.gold -= b;
				put_pc_screen();
				univ.party.spec_items[a] = 1;
			}
			strnum2 = 0;
			save_talk_str2 = "";
			break;
		case 23:
			start_shop_mode(eShopType(5 + b),0,
							9,a,save_talk_str1.c_str());
			strnum1 = -1;
			return;
		case 24:
			if(univ.party.can_find_town[b]) {
				// TODO: Uh, is something supposed to happen here?
			}
			else if(univ.party.gold < a) {
				strnum1 = strnum2;
				save_talk_str1 = save_talk_str2;
			}
			else {
				univ.party.gold -= a;
				put_pc_screen();
				univ.party.can_find_town[b] = 1;
			}
			strnum2 = 0;
			save_talk_str2 = "";
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
			// TODO: Any reason not to call something like kill_monst?
			univ.town.monst[store_m_num].active = 0;
			// Special killing effects
			if(sd_legit(univ.town.monst[store_m_num].spec1,univ.town.monst[store_m_num].spec2))
				PSD[univ.town.monst[store_m_num].spec1][univ.town.monst[store_m_num].spec2] = 1;
			talk_end_forced = true;
			break;
			// TODO: Strings resulting from this don't seem to be recordable; whyever not?
		case 29: // town special
			run_special(eSpecCtx::TALK,2,a,univ.town.p_loc,&s1,&s2,&s3);
			// check s1 & s2 to see if we got diff str, and, if so, munch old strs
			if((s1 >= 0) || (s2 >= 0)) {
				strnum1 = -1;
				strnum2 = -1;
				save_talk_str1 = "";
				save_talk_str2 = "";
			}
			get_strs(save_talk_str1,save_talk_str2,2,s1,s2);
			if(s1 >= 0) strnum1 = 2000 + s1;
			if(s2 >= 0) strnum2 = 2000 + s2;
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;
		case 30: // scen special
			run_special(eSpecCtx::TALK,0,a,univ.town.p_loc,&s1,&s2,&s3);
			// check s1 & s2 to see if we got diff str, and, if so, munch old strs
			if((s1 >= 0) || (s2 >= 0)) {
				strnum1 = -1;
				strnum2 = -1;
				save_talk_str1 = "";
				save_talk_str2 = "";
			}
			get_strs(save_talk_str1,save_talk_str2,0,s1,s2);
			if(s1 >= 0) strnum1 = 3000 + s1;
			if(s2 >= 0) strnum2 = 3000 + s2;
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;
	}
	
	place_talk_str(save_talk_str1,save_talk_str2,0,dummy_rect);
}

//town_num; // Will be 0 - 200 for town, 200 - 290 for outdoors
//short sign_type; // terrain type
void do_sign(short town_num, short which_sign, short sign_type) {
	std::string sign_text;
	location view_loc;
	
	// TODO: Why is this line here? The location isn't used anywhere.
	view_loc = (is_out()) ? univ.party.p_loc : univ.town.p_loc;
	make_cursor_sword();
	
	cChoiceDlog sign("view-sign");
	cPict& pict = dynamic_cast<cPict&>(sign->getControl("ter"));
	
	store_sign_mode = sign_type;
	pict.setPict(univ.scenario.ter_types[sign_type].picture);
	
	if(town_num >= 200) {
		town_num -= 200;
		sign_text = univ.out->sign_strs[which_sign];
	}
	else {
		sign_text = univ.town->sign_strs[which_sign];
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

static bool prefs_event_filter (cDialog& me, std::string id, eKeyMod) {
	// TODO: I should no longer need done_yet as this now only handles the okay and cancel buttons; the LEDs are now handled automatically by the cLed class (and the cLedGroup class, for LED groups).
	bool done_yet = false,did_cancel = false,reset_help = false;
	short i;
	
	if(id == "okay") {
		done_yet = true;
		me.toast(true);
	} else if(id == "cancel") {
		done_yet = true;
		me.toast(false);
		did_cancel = true;
	}
	
	if(done_yet) {
		if(!did_cancel) {
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
			/* TODO: Should I add these additional preferences from Windows?
			party.stuff_done[SDF_NO_TARGET_LINE] = cd_get_led(1099,50);
			party.stuff_done[SDF_LESS_SOUND] = cd_get_led(1099,52);
			party.stuff_done[SDF_FASTER_BOOM_SPACES] = cd_get_led(1099,56);
			party.stuff_done[SDF_ASK_ABOUT_TEXT_BOX] = cd_get_led(1099,60);
			*/
			// TODO: NO_SOUNDS and NO_MAPS preferences are stored in two different places
			if(speed == "fast")
				PSD[SDF_GAME_SPEED] = 0;
			else if(speed == "med")
				PSD[SDF_GAME_SPEED] = 1;
			else if(speed == "slow")
				PSD[SDF_GAME_SPEED] = 2;
			else if(speed == "snail")
				PSD[SDF_GAME_SPEED] = 3;
			if(dynamic_cast<cLed&>(me["resethelp"]).getState() == led_red) {
				for(i = 0; i < 120; i++)
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

void pick_preferences() {
	make_cursor_sword();
	
	cDialog prefsDlog("preferences");
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
	
	if(univ.party.help_received[55] == 0) {
		// TODO: Not sure if this bit is needed?
//		cd_initial_draw(1099);
		give_help(55,0,prefsDlog);
	}
	
	int store_display_mode = display_mode;
	prefsDlog.run();
	
	if(display_mode != store_display_mode)
		changed_display_mode = true;
}

static void put_party_stats(cDialog& me) {
	short i;
	
	
	for(i = 0; i < 6; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if(univ.party[i].main_status != eMainStatus::ABSENT) {
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

static bool edit_party_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	if(item_hit == "done") {
		me.toast(true);
	} else if(item_hit == "help") {
		univ.party.help_received[22] = 0;
		give_help(222,23,me);
	} else {
		short which_pc = item_hit[item_hit.length()-1] - '1';
		item_hit = item_hit.substr(0, item_hit.length() - 1);
		if(item_hit == "name") {
			if(univ.party[which_pc].main_status == eMainStatus::ABSENT) {
				// TODO: Clicking a blank field made a new PC? Something to consider reinstating!
				//give_help(56,0,989);
				//create_pc(which_pc,989);
				//cd_initial_draw(989); // extra redraw, just in case
			}
			else pick_pc_name(which_pc,&me);
			put_party_stats(me);
		} else if(item_hit == "trait") {
			pick_race_abil(&univ.party[which_pc],0);
			put_party_stats(me);
		} else if(item_hit == "train") {
			spend_xp(which_pc,0,&me);
			put_party_stats(me);
		} else if(item_hit == "pic") {
			if(univ.party[which_pc].main_status == eMainStatus::ABSENT)
				return true;
			// TODO: Uhh, why did he check this twice...
			if(univ.party[which_pc].main_status != eMainStatus::ABSENT)
				pick_pc_graphic(which_pc,1,&me);
			put_party_stats(me);
		} else if(item_hit == "delete") { // Note: This button is also used for "create new PC".
			if(univ.party[which_pc].main_status != eMainStatus::ABSENT) {
				if(cChoiceDlog("delete-pc-confirm",{"yes","no"}).show() == "yes")
					univ.party[which_pc].main_status = eMainStatus::ABSENT;
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
void edit_party() {
	bool munch_pc_graphic = false;
	
	make_cursor_sword();
	
	if(!pc_gworld_loaded) {
		munch_pc_graphic = true;
		pc_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcs"));
	}
	
	cDialog pcDialog("edit-party");
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
	if(univ.party.help_received[22] == 0) {
		// TODO: Not sure if this is needed
		//		cd_initial_draw(989);
		give_help(22,23,pcDialog);
	}
	
	pcDialog.run();
	
	if(univ.party[current_pc].main_status != eMainStatus::ALIVE)
		current_pc = first_active_pc();
	
	
}

static bool tip_of_day_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	std::string place_str;
	
	if(item_hit == "done") {
		me.toast(true);
	} else if(item_hit == "next") {
		store_tip_page_on++;
		if(store_tip_page_on == NUM_HINTS)
			store_tip_page_on = 0;
		place_str = get_str("tips",50 + store_tip_page_on);
		me["tip"].setText(place_str);
	}
	return true;
}

void tip_of_day() {
	std::string place_str;
	
	store_tip_page_on = get_ran(1,0,NUM_HINTS - 1);
	
	make_cursor_sword();
	
	cDialog tips("tip-of-day");
	tips.attachClickHandlers(&tip_of_day_event_filter,{"done","next"});
	
	place_str = get_str("tips",50 + store_tip_page_on);
	tips["tip"].setText(place_str);
	
	cLed& showAtStart = dynamic_cast<cLed&>(tips["onstart"]);
	showAtStart.setState(give_intro_hint ? led_red : led_off);
	
	tips.run();
	give_intro_hint = showAtStart.getState() != led_off;
	save_prefs();
	
}

static void put_scen_info(cDialog& me) {
	unsigned int i;
	std::ostringstream sout;
	const char *ratings[] = {"G","PG","R","NC-17"};
	const char *difficulty[] = {"Low","Medium","High","Very High"};
	
	for(i = 0; i < 3; i++) {
		sout.clear();
		sout.str("");
		sout << i + 1;
		std::string n = sout.str();
		if(scen_headers.size() > (store_scen_page_on * 3 + i) && scen_headers.data(store_scen_page_on * 3 + i).flag1 != 0) {
			me["pic" + n].show();
			dynamic_cast<cPict&>(me["pic" + n]).setPict(scen_headers.data(store_scen_page_on * 3 + i).intro_pic);
			sout.str("");
			sout << scen_headers.strs(store_scen_page_on * 3 + i).name;
			sout << " v" << int(scen_headers.data(store_scen_page_on * 3 + i).ver[0]);
			sout << '.' << int(scen_headers.data(store_scen_page_on * 3 + i).ver[1]);
			sout << '.' << int(scen_headers.data(store_scen_page_on * 3 + i).ver[2]);
			sout << " - |  Difficulty: " << difficulty[scen_headers.data(store_scen_page_on * 3 + i).difficulty];
			sout << ", Rating: " << ratings[scen_headers.data(store_scen_page_on * 3 + i).rating];
			sout << " |" << scen_headers.strs(store_scen_page_on * 3 + i).who1;
			sout << " |" << scen_headers.strs(store_scen_page_on * 3 + i).who2;
			me["desc" + n].setText(sout.str());
			me["start" + n].show();
		}
		else {
			me["pic" + n].hide();
			me["desc" + n].setText("");
			me["start" + n].hide();
		}
	}
}

static bool pick_a_scen_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	if(item_hit == "cancel") {
		me.setResult<short>(-1);
		me.toast(false);
	} else if(item_hit == "next") {
		if(store_scen_page_on == 0)
			store_scen_page_on = (store_num_scen - 1) / 3;
		else store_scen_page_on--;
		put_scen_info(me);
	} else if(item_hit == "prev") {
		if(store_scen_page_on == (store_num_scen - 1) / 3)
			store_scen_page_on = 0;
		else store_scen_page_on++;
		put_scen_info(me);
	} else if(item_hit.substr(0,item_hit.length()-1) == "start") {
		int scen_hit = item_hit[item_hit.length()-1] - '1';
		me.setResult<short>(scen_hit + store_scen_page_on * 3);
		me.toast(true);
	}
	return true;
}

short pick_a_scen() {
	
	build_scen_headers();
	
	store_num_scen = scen_headers.size();
//	for(i = 0; i < 25; i++)
//		if(scen_headers[i].flag1 != 0)
//			store_num_scen++;
	store_scen_page_on = 0;
	
	if(store_num_scen == 0) {
		cChoiceDlog("no-scenarios").show();
		return -1;
	}
	make_cursor_sword();
	
	cDialog pickScen("pick-scenario");
	pickScen.attachClickHandlers(pick_a_scen_event_filter, {"cancel", "next", "prev", "start1", "start2", "start3"});
	
	put_scen_info(pickScen);
	
	if(store_num_scen <= 3) {
		pickScen["next"].hide();
		pickScen["prev"].hide();
	}
	
	pickScen.run();
	return pickScen.getResult<short>();
}

short pick_prefab_scen() {
	
	make_cursor_sword();
	
	cChoiceDlog pickScenario("pick-prefab-scen", {"cancel", "scen1", "scen2", "scen3"});
	
	std::string item_hit = pickScenario.show();
	if(item_hit == "cancel") return -1;
	else return item_hit[4] - '1';
}

//void give_password_filter (short item_hit)
//{
//
//	switch(item_hit) {
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
//	for(i = 0; i < 6; i++) {
//		if(str[i] == 0)
//			i = 6;
//			else {
//				if((str[i] >= 65) && (str[i] <= 90))
//					pwd[i] = str[i] + 32;
//				else if((str[i] >= 48) && (str[i] <= 57))
//					pwd[i] = str[i] + 49;
//				else if((str[i] >= 97) && (str[i] <= 122))
//					pwd[i] = str[i];
//				}
//		}
//	for(i = 0; i < 6; i++)
//		val = val + pow[i] * (long) (pwd[i] - 97);
//	val = val % 30000;
//	return (short) val;
//}
