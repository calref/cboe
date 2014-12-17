
#include <cstdio>

#include "pc.graphics.h"
#include "pc.global.h"
#include "classes.h"
#include "pc.editors.h"
#include "pc.fileio.h"
#include "pc.action.h"
#include "graphtool.h"
#include "soundtool.h"
#include "mathutil.h"
#include "dlogutil.h"
#include <boost/lexical_cast.hpp>

/* Adventure globals */
//extern party_record_type party;
//extern outdoor_record_type outdoors[2][2];
//extern current_town_type c_town;
//extern big_tr_type t_d;
//extern town_item_list	t_i;
//extern unsigned char out[96][96],out_e[96][96];
//extern setup_save_type setup_save;
//extern stored_items_list_type stored_items[3];
//extern stored_town_maps_type town_maps;
//extern stored_outdoor_maps_type o_maps;
extern cUniverse univ;

//extern bool ed_reg;
//extern long ed_flag,ed_key;

extern sf::RenderWindow mainPtr;
extern bool file_in_mem;
//extern long register_flag;

extern sf::Texture pc_gworld;
extern bool diff_depth_ok;



short which_pc_displayed,store_pc_trait_mode,store_which_to_edit;
extern short current_active_pc;
char empty_string[256] = "                                                ";
extern rectangle pc_area_buttons[6][4] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
extern rectangle item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 -
extern rectangle pc_info_rect;
extern rectangle name_rect;

extern rectangle pc_race_rect;
extern rectangle edit_rect[5][2];



//extern rectangle pc_area_buttons[6][6] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
//extern rectangle item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 -
//short mode; // ignore,
bool handle_action(sf::Event event) {
	short i;
	
	location the_point;
	
	bool to_return = false;
	
	the_point = {event.mouseButton.x, event.mouseButton.y};
	
	if(!file_in_mem)
		return false;
	
	for(i = 0; i < 6; i++)
		if((the_point.in(pc_area_buttons[i][0])) &&
		   (univ.party[i].main_status != eMainStatus::ABSENT)) {
			do_button_action(0,i);
			current_active_pc = i;
			redraw_screen();
		}
	for(i = 0; i < 5; i++)
		if((the_point.in(edit_rect[i][0])) &&
		   (univ.party[current_active_pc].main_status != eMainStatus::ABSENT)) {
			do_button_action(0,i + 10);
			switch(i) {
				case 0:
					display_pc(current_active_pc,0,NULL);
					break;
				case 1:
			 		display_pc(current_active_pc,1,NULL);
					break;
				case 2:
					pick_race_abil(&univ.party[current_active_pc],0);
					break;
				case 3:
					spend_xp(current_active_pc,1,NULL);
					break;
				case 4:
					edit_xp(&univ.party[current_active_pc]);
					
					break;
			}
		}
	for(i = 0; i < 24; i++)
		if((the_point.in(item_string_rects[i][1])) && // drop item
		   univ.party[current_active_pc].items[i].variety != eItemType::NO_ITEM) {
			flash_rect(item_string_rects[i][1]);
			take_item(current_active_pc,i);
		}
	for(i = 0; i < 24; i++)
		if((the_point.in(item_string_rects[i][2])) && // identify item
		   univ.party[current_active_pc].items[i].variety != eItemType::NO_ITEM) {
			flash_rect(item_string_rects[i][2]);
			univ.party[current_active_pc].items[i].ident = true;
		}
	
	return to_return;
}

void flash_rect(rectangle /*to_flash*/) {
	
	// TODO: Think of a good way to do this
	//InvertRect (&to_flash);
	play_sound(37);
	sf::sleep(time_in_ticks(5));
}

static bool get_num_event_filter(cDialog& me, std::string id, eKeyMod) {
	me.toast(id == "okay");
	me.setResult<long long>(me["number"].getTextAsNum());
	return true;
}

//0 - gold 1 - food
void edit_gold_or_food(short which_to_edit) {
	
	location view_loc;
	
	store_which_to_edit = which_to_edit;
	
	make_cursor_sword();
	cDialog dlog("get-num.xml");
	dlog["okay"].attachClickHandler(get_num_event_filter);
	if(which_to_edit == 0)
		dlog["prompt"].setText("How much gold do you want?");
	else dlog["prompt"].setText("How much food do you want?");
	dlog["number"].setTextToNum((which_to_edit == 0) ? univ.party.gold : univ.party.food);
	
	dlog.run();
	int dialog_answer = minmax(0,25000,dlog.getResult<long long>());
	if(which_to_edit == 0)
		univ.party.gold = dialog_answer;
	else
		univ.party.food = dialog_answer;
}

void edit_day() {
	
	location view_loc;
	
	
	make_cursor_sword();
	
	cDialog dlog("edit-day.xml");
	dlog["okay"].attachClickHandler(get_num_event_filter);
	
	dlog["number"].setTextToNum(((univ.party.age) / 3700) + 1);
	
	dlog.run();
	
	long long dialog_answer = minmax(0,500,dlog.getResult<long long>());
	
	univ.party.age = (long long) (3700) * (long long) (dialog_answer);
}

void combine_things(short pc_num) {
	short i,j,test;
	
	for(i = 0; i < 24; i++) {
		if(univ.party[pc_num].items[i].variety != eItemType::NO_ITEM &&
		   (univ.party[pc_num].items[i].type_flag > 0) && (univ.party[pc_num].items[i].ident)) {
			for(j = i + 1; j < 24; j++)
				if(univ.party[pc_num].items[j].variety != eItemType::NO_ITEM &&
				   (univ.party[pc_num].items[j].type_flag == univ.party[pc_num].items[i].type_flag)
				   && (univ.party[pc_num].items[j].ident)) {
					//					add_string_to_buf("(items combined)");
					test = (short) (univ.party[pc_num].items[i].charges) + (short) (univ.party[pc_num].items[j].charges);
					if(test > 125) {
						univ.party[pc_num].items[i].charges = 125;
						//						ASB("Can have at most 125 of any item.");
					}
					else univ.party[pc_num].items[i].charges += univ.party[pc_num].items[j].charges;
				 	if(univ.party[pc_num].equip[j]) {
				 		univ.party[pc_num].equip[i] = true;
				 		univ.party[pc_num].equip[j] = false;
					}
					take_item(pc_num,j);
				}
		}
		if(univ.party[pc_num].items[i].variety != eItemType::NO_ITEM && univ.party[pc_num].items[i].charges < 0)
			univ.party[pc_num].items[i].charges = 1;
	}
}

bool give_to_pc(short pc_num,cItemRec item, short /*print_result*/) {
	short free_space;
	
	if(item.variety == eItemType::NO_ITEM)
		return true;
	if((free_space = pc_has_space(pc_num)) == 24  || univ.party[pc_num].main_status != eMainStatus::ALIVE)
		return false;
	else {
		univ.party[pc_num].items[free_space] = item;
		combine_things(pc_num);
		return true;
	}
	return false;
}

bool give_to_party(cItemRec item,short print_result) {
	short i = 0;
	
	while(i < 6) {
		if(give_to_pc(i,item,print_result))
			return true;
		i++;
	}
	return false;
}

void give_gold(short amount,bool /*print_result*/) {
	univ.party.gold = univ.party.gold + amount;
}

bool take_gold(short amount,bool /*print_result*/) {
	if(univ.party.gold < amount)
		return false;
	univ.party.gold = univ.party.gold - amount;
	return true;
}

short pc_has_space(short pc_num) {
	short i = 0;
	
	while(i < 24) {
		if(univ.party[pc_num].items[i].variety == eItemType::NO_ITEM)
			return i;
		i++;
	}
	return 24;
}

//short pc_num,which_item;  // if which_item > 20, don't update stat win, item is which_item - 20
void take_item(short pc_num,short which_item) {
	short i;
	
	if(univ.party[pc_num].weap_poisoned == which_item && univ.party[pc_num].status[eStatus::POISONED_WEAPON] > 0) {
		//			add_string_to_buf("  Poison lost.           ");
		univ.party[pc_num].status[eStatus::POISONED_WEAPON] = 0;
	}
	if(univ.party[pc_num].weap_poisoned > which_item && univ.party[pc_num].status[eStatus::POISONED_WEAPON] > 0)
		univ.party[pc_num].weap_poisoned--;
	
	for(i = which_item; i < 23; i++) {
		univ.party[pc_num].items[i] = univ.party[pc_num].items[i + 1];
		univ.party[pc_num].equip[i] = univ.party[pc_num].equip[i + 1];
	}
	univ.party[pc_num].items[23].variety = eItemType::NO_ITEM;
	univ.party[pc_num].equip[23] = false;
	
}

void edit_xp(cPlayer *pc) {
	location view_loc;
	
	make_cursor_sword();
	
	cDialog dlog("edit-xp.xml");
	dlog["okay"].attachClickHandler(get_num_event_filter);
	
	dlog["number"].setTextToNum(pc->experience);
	dlog["perlevel"].setTextToNum(pc->get_tnl());
	
	dlog.run();
	
	int dialog_answer = minmax(0,10000,abs(dlog.getResult<long long>()));
	
	pc->experience = dialog_answer;
}


