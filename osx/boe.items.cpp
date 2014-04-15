
#include <cstdio>
#include <cstring>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"

#include "boe.graphics.h"
#include "boe.text.h"
#include "boe.items.h"
#include "boe.party.h"
#include "boe.fields.h"
#include "boe.locutils.h"
#include "boe.newgraph.h"
#include "boe.itemdata.h"
#include "boe.infodlg.h"
#include "soundtool.h"
#include "boe.monster.h"
#include "boe.main.h"
#include "graphtool.h"
#include "mathutil.h"
#include "dlogutil.h"
#include "restypes.hpp"
#include "message.h"
#include <array>
#include <boost/lexical_cast.hpp>
#include "cursors.h"
#include "winutil.h"

extern short stat_window,which_combat_type,current_pc;
extern eGameMode overall_mode;
//extern party_record_type party;
//extern current_town_type univ.town;
//extern town_item_list	univ.town.items;
extern sf::RenderWindow mainPtr;
extern bool in_startup_mode,boom_anim_active;
extern RECT d_rects[80];
extern short d_rect_index[80];
//extern big_tr_type t_d;
extern location pc_pos[6];

//extern CursHandle sword_curs;
extern bool map_visible,diff_depth_ok;
extern sf::RenderWindow mini_map;
//extern short town_size[3];
extern short town_type;
extern sf::Texture pc_gworld;
extern cScenario scenario;
extern cUniverse univ;

////
bool equippable[26] = {false,true,true,false,true, true,true,false,false,false,
						true,false,true,true,true, true,true,true,true,true,
						false,false,true,true,true,true};
short num_hands_to_use[26] = {0,1,2,0,0, 0,0,0,0,0 ,0,0,1,0,0, 0,1,0,0,0, 0,0,0,0,0, 0};
short num_that_can_equip[26] = {0,2,1,0,1, 1,1,0,0,0, 1,0,1,1,1, 1,1,1,2,1, 0,0,1,1,1, 1}; 

// For following, if an item of type n is equipped, no other items of type n can be equipped, 
// if n > 0
short excluding_types[26] = {0,0,0,0,2, 1,1,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,2,1, 2};

short selected,item_max = 0;

short first_item_shown,store_get_mode,current_getting_pc,store_pcnum,total_items_gettable; // these 5 used for get items dialog
short item_array[130]; // NUM_TOWN_ITEMS + a bit

void sort_pc_items(short pc_num)
{
	cItemRec store_item;
	////
	short item_priority[26] = {20,8,8,20,9, 9,3,2,1,0, 7,20,10,10,10, 10,10,10,5,6, 4,11,12,9,9, 9};
	bool no_swaps = false,store_equip;
	short i;
	
	while (no_swaps == false) {
		no_swaps = true;
		for (i = 0; i < 23; i++)
			if (item_priority[univ.party[pc_num].items[i + 1].variety] < 
			  item_priority[univ.party[pc_num].items[i].variety]) {
			  	no_swaps = false;
			  	store_item = univ.party[pc_num].items[i + 1];
			  	univ.party[pc_num].items[i + 1] = univ.party[pc_num].items[i];
			  	univ.party[pc_num].items[i] = store_item;
			  	store_equip = univ.party[pc_num].equip[i + 1];
	 			univ.party[pc_num].equip[i + 1] = univ.party[pc_num].equip[i];
				univ.party[pc_num].equip[i] = store_equip;
				if (univ.party[pc_num].weap_poisoned == i + 1)
					univ.party[pc_num].weap_poisoned--;
					else if (univ.party[pc_num].weap_poisoned == i)
						univ.party[pc_num].weap_poisoned++;
			  	}
		}
}

bool give_to_party(cItemRec item, short print_result) {
	short i = 0;
	
	while (i < 6) {
		if (give_to_pc(i,item,print_result))
			return true;
		i++;
	}
	return false;
}

////  
bool give_to_pc(short pc_num,cItemRec  item,short  print_result)
{
	short free_space;
	char announce_string[60];
	
	if (item.variety == 0)
		return true;
	if (item.variety == 3) {
		univ.party.gold += item.item_level;
		ASB("You get some gold.");
		return true;
		}
	if (item.variety == 11) {
		univ.party.food += item.item_level;
		ASB("You get some food.");
		return true;
		}
	if (item.item_weight() > 
	  amount_pc_can_carry(pc_num) - pc_carry_weight(pc_num)) {
	  	if (print_result == true) {
			// TODO: Play an error sound here
			ASB("Item too heavy to carry.");
			}
		return false;
	  	}
	free_space = pc_has_space(pc_num);
	if ((free_space == 24) || (univ.party[pc_num].main_status != 1))
		return false;
		else {
			item.property = false;
			item.contained = false;
			univ.party[pc_num].items[free_space] = item;

			if (print_result == 1) {
				if (stat_window == pc_num)
					put_item_screen(stat_window,0);
			}
			if (in_startup_mode == false) {
				if (!item.ident)
					sprintf((char *) announce_string,"  %s gets %s.",univ.party[pc_num].name.c_str(),item.name.c_str());
					else sprintf((char *) announce_string,"  %s gets %s.",univ.party[pc_num].name.c_str(),item.full_name.c_str());
				if (print_result == true)
					add_string_to_buf((char *)announce_string);
				}

			combine_things(pc_num);
			sort_pc_items(pc_num);
			return true;
			}
	return false;
}

bool forced_give(short item_num,eItemAbil abil) ////
// if abil > 0, force abil, else ignore
{
	short i,j;
	cItemRec item;
	char announce_string[60];

	if ((item_num < 0) || (item_num > 399))
		return true;
	item = get_stored_item(item_num);
	if (abil > ITEM_NO_ABILITY)
		item.ability = abil;
	for (i = 0; i < 6; i++)
		for (j = 0; j < 24; j++)
			if ((univ.party[i].main_status == 1) && (univ.party[i].items[j].variety == 0)) {
				univ.party[i].items[j] = item;
			
				if (!item.ident)
					sprintf((char *) announce_string,"  %s gets %s.",univ.party[i].name.c_str(),item.name.c_str());
					else sprintf((char *) announce_string,"  %s gets %s.",univ.party[i].name.c_str(),item.full_name.c_str());
				add_string_to_buf((char *)announce_string);
				combine_things(i);
				sort_pc_items(i);
				return true;
				}	
	return false;
}

bool GTP(short item_num)
{
	cItemRec item;
	
	item = get_stored_item(item_num);
	return give_to_party(item,true);
}
bool silent_GTP(short item_num)
{
	cItemRec item;
	
	item = get_stored_item(item_num);
	return give_to_party(item,false);
}
void give_gold(short amount,bool print_result)////
{
	if (amount < 0) return;
	univ.party.gold = univ.party.gold + amount;
	if (print_result == true)
		put_pc_screen();
}

bool take_gold(short amount,bool print_result)
{
	if (univ.party.gold < amount)
		return false;
	univ.party.gold = univ.party.gold - amount;
	if (print_result == true)
		put_pc_screen();
	return true;
}

// returnes equipped protection level of specified abil, or -1 if no such abil is equipped
short get_prot_level(short pc_num,short abil) ////
{
	short i = 0;
	
	for (i = 0; i < 24; i++)
		if ((univ.party[pc_num].items[i].variety != 0) && (univ.party[pc_num].items[i].ability == abil)
			&& (univ.party[pc_num].equip[i] == true))
				return univ.party[pc_num].items[i].ability_strength;
	return -1;
				
}

short pc_has_abil_equip(short pc_num,short abil)
{
	short i = 0;
	
	while (((univ.party[pc_num].items[i].variety == 0) || (univ.party[pc_num].items[i].ability != abil)
			|| (univ.party[pc_num].equip[i] == false)) && (i < 24))
				i++;
	return i;
				
}

short pc_has_abil(short pc_num,short abil)
{
	short i = 0;
	
	while (((univ.party[pc_num].items[i].variety == 0) || (univ.party[pc_num].items[i].ability != abil)
			) && (i < 24))
				i++;
	return i;
}

bool party_has_abil(short abil)
{
	short i;
	
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1)
			if (pc_has_abil(i,abil) < 24)
				return true;
	return false;
}

bool party_take_abil(short abil)
{
	short i,item;
	
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1)
			if ((item = pc_has_abil(i,abil)) < 24) {
				if (univ.party[i].items[item].charges > 1)
					univ.party[i].items[item].charges--;
					else take_item(i,item);
				return true;
				}
	return false;
}

// returns true is party has item of given item class
// mode - 0 - take one of them, 1 - don't take
bool party_check_class(short item_class,short mode) ////
{
	short i,j;
	
	if (item_class == 0)
		return false;
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1)
			for (j = 23; j >= 0; j--)
				if ((univ.party[i].items[j].variety > 0) && (univ.party[i].items[j].special_class == item_class)) {
					if (mode == 0) {
						if (univ.party[i].items[j].charges > 1)
							univ.party[i].items[j].charges--;
							else take_item(i,j);
						}
					return true;
					}
	return false;
}
short amount_pc_can_carry(short pc_num)
{
	return 100 + (15 * min(univ.party[pc_num].skills[0],20)) + ((univ.party[pc_num].traits[8] == 0) ? 0 : 30)
		+ ((univ.party[pc_num].traits[14] == 0) ? 0 : -50);
}
short pc_carry_weight(short pc_num)
{
	short i,storage = 0;
	bool airy = false,heavy = false;
	
	for (i = 0; i < 24; i++) 
		if (univ.party[pc_num].items[i].variety > 0) {
		storage += univ.party[pc_num].items[i].item_weight();
		if (univ.party[pc_num].items[i].ability == 44)
			airy = true;
		if (univ.party[pc_num].items[i].ability == 45)
			heavy = true;
		}
	if (airy)
		storage -= 30;
	if (heavy)
		storage += 30;
	if (storage < 0)
		storage = 0;
	return storage;
}

void give_food(short amount,bool print_result)////
{
	if (amount < 0) return;
	univ.party.food = univ.party.food + amount;
	if (print_result == true)
		put_pc_screen();	
}

short take_food(short amount,bool print_result)
{
	short diff;

	diff = amount - univ.party.food;
	if (diff > 0) {
		univ.party.food = 0;
		if (print_result == true)
			put_pc_screen();
		return diff;
		}

	univ.party.food = univ.party.food - amount;
	if (print_result == true)
		put_pc_screen();	
	return 0;	
}

short pc_has_space(short pc_num)
{
	short i = 0;
	
	while (i < 24) {
	if (univ.party[pc_num].items[i].variety == 0)
		return i;
	i++;
	}
	return 24;
}

// returns 1 if OK, 2 if no room, 3 if not enough cash, 4 if too heavy, 5 if too many of item
// Made specials cases for if item is gold or food
short pc_ok_to_buy(short pc_num,short cost,cItemRec item) ////
{
	short i;
	
	if ((item.variety != 3) && (item.variety != 11)) {
		for (i = 0; i < 24; i++)
			if ((univ.party[pc_num].items[i].variety > 0) && (univ.party[pc_num].items[i].type_flag == item.type_flag)
				&& (univ.party[pc_num].items[i].charges > 123))
					return 5;

		if (pc_has_space(pc_num) == 24)
			return 2;
		if (item.item_weight() > 
		  amount_pc_can_carry(pc_num) - pc_carry_weight(pc_num)) {
	  		return 4;
	  		}	
	  	}
	if (take_gold(cost,false) == false)
		return 3;
	return 1;


}

void take_item(short pc_num,short which_item)
//short pc_num,which_item;  // if which_item > 30, don't update stat win, item is which_item - 30
{
	short i;
	bool do_print = true;
	
	if (which_item >= 30) {
		do_print = false;
		which_item -= 30;
		}
		
	if ((univ.party[pc_num].weap_poisoned == which_item) && (univ.party[pc_num].status[0] > 0)) {
			add_string_to_buf("  Poison lost.           ");
			univ.party[pc_num].status[0] = 0;
		}
	if ((univ.party[pc_num].weap_poisoned > which_item) && (univ.party[pc_num].status[0] > 0)) 
		univ.party[pc_num].weap_poisoned--;
		
	for (i = which_item; i < 23; i++) {
		univ.party[pc_num].items[i] = univ.party[pc_num].items[i + 1];
		univ.party[pc_num].equip[i] = univ.party[pc_num].equip[i + 1];
		}
	univ.party[pc_num].items[23] = cItemRec();
	univ.party[pc_num].equip[23] = false;
	
	if ((stat_window == pc_num) && (do_print == true))
		put_item_screen(stat_window,1);
}

void remove_charge(short pc_num,short which_item)
{
	if (univ.party[pc_num].items[which_item].charges > 0) {
			univ.party[pc_num].items[which_item].charges--;
			if (univ.party[pc_num].items[which_item].charges == 0) {
				take_item(pc_num,which_item);
				}
		}

	if (stat_window == pc_num)
		put_item_screen(stat_window,1);

}

void enchant_weapon(short pc_num,short item_hit,short enchant_type,short new_val)
{
	char store_name[60];

	////
	if (univ.party[pc_num].items[item_hit].magic ||
		(univ.party[pc_num].items[item_hit].ability != 0))
			return;
	univ.party[pc_num].items[item_hit].magic = true;
	univ.party[pc_num].items[item_hit].enchanted = true;
	switch (enchant_type) {
		case 0:
			sprintf((char *)store_name,"%s (+1)",univ.party[pc_num].items[item_hit].full_name.c_str());
			univ.party[pc_num].items[item_hit].bonus++;
			univ.party[pc_num].items[item_hit].value = new_val;
			break;
		case 1:
			sprintf((char *)store_name,"%s (+2)",univ.party[pc_num].items[item_hit].full_name.c_str());
			univ.party[pc_num].items[item_hit].bonus += 2;
			univ.party[pc_num].items[item_hit].value = new_val;
			break;
		case 2:
			sprintf((char *)store_name,"%s (+3)",univ.party[pc_num].items[item_hit].full_name.c_str());
			univ.party[pc_num].items[item_hit].bonus += 3;
			univ.party[pc_num].items[item_hit].value = new_val;
			break;
		case 3:
			sprintf((char *)store_name,"%s (F)",univ.party[pc_num].items[item_hit].full_name.c_str());
			univ.party[pc_num].items[item_hit].ability = ITEM_SPELL_FLAME;
			univ.party[pc_num].items[item_hit].ability_strength = 5;
			univ.party[pc_num].items[item_hit].charges = 8;
			break;
		case 4:
			sprintf((char *)store_name,"%s (F!)",univ.party[pc_num].items[item_hit].full_name.c_str());
			univ.party[pc_num].items[item_hit].value = new_val;
			univ.party[pc_num].items[item_hit].ability = ITEM_FLAMING_WEAPON;
			univ.party[pc_num].items[item_hit].ability_strength = 5;
			break;
		case 5:
			sprintf((char *)store_name,"%s (+5)",univ.party[pc_num].items[item_hit].full_name.c_str());
			univ.party[pc_num].items[item_hit].value = new_val;
			univ.party[pc_num].items[item_hit].bonus += 5;
			break;
		case 6:
			sprintf((char *)store_name,"%s (B)",univ.party[pc_num].items[item_hit].full_name.c_str());
			univ.party[pc_num].items[item_hit].bonus++;
			univ.party[pc_num].items[item_hit].ability = ITEM_BLESS_CURSE;
			univ.party[pc_num].items[item_hit].ability_strength = 5;
			univ.party[pc_num].items[item_hit].magic_use_type = 0;
			univ.party[pc_num].items[item_hit].charges = 8;
			break;
		default:
			strcpy(store_name,univ.party[pc_num].items[item_hit].full_name.c_str());
			break;
		}
	if (univ.party[pc_num].items[item_hit].value > 15000)
		univ.party[pc_num].items[item_hit].value = 15000;
	if (univ.party[pc_num].items[item_hit].value < 0)
		univ.party[pc_num].items[item_hit].value = 15000;
	univ.party[pc_num].items[item_hit].full_name = store_name;
}

void equip_item(short pc_num,short item_num)
{
	short num_equipped_of_this_type = 0;
	short num_hands_occupied = 0;
	short i;
	short equip_item_type = 0;
	
if ((overall_mode == MODE_COMBAT) && (univ.party[pc_num].items[item_num].variety == 11))
		add_string_to_buf("Equip: Not in combat");
	else {

		// unequip
	if (univ.party[pc_num].equip[item_num] == true) {
		if (univ.party[pc_num].equip[item_num] && univ.party[pc_num].items[item_num].cursed)
			add_string_to_buf("Equip: Item is cursed.           ");
			else {
				univ.party[pc_num].equip[item_num] = false;
				add_string_to_buf("Equip: Unequipped");
				if ((univ.party[pc_num].weap_poisoned == item_num) && (univ.party[pc_num].status[0] > 0)) {
						add_string_to_buf("  Poison lost.           ");
						univ.party[pc_num].status[0] = 0;
					}
			}
		}

	else {  // equip
		if (equippable[univ.party[pc_num].items[item_num].variety] == false)
			add_string_to_buf("Equip: Can't equip this item.");
				else {
					for (i = 0; i < 24; i++) 
						if (univ.party[pc_num].equip[i] == true) {
							if (univ.party[pc_num].items[i].variety == univ.party[pc_num].items[item_num].variety)
								num_equipped_of_this_type++;
							num_hands_occupied = num_hands_occupied + num_hands_to_use[univ.party[pc_num].items[i].variety];
						}
						
						
					equip_item_type = excluding_types[univ.party[pc_num].items[item_num].variety];
					// Now if missile is already equipped, no more missiles
					if (equip_item_type > 0) {
						for (i = 0; i < 24; i++) 
							if ((univ.party[pc_num].equip[i] == true) && (excluding_types[univ.party[pc_num].items[i].variety] == equip_item_type)) {
								add_string_to_buf("Equip: You have something of");
								add_string_to_buf("  this type equipped.");
								return;
								}
						}

					if ((is_combat()) && (univ.party[pc_num].items[item_num].variety == 13))
						add_string_to_buf("Equip: Not armor in combat");
						else if ((2 - num_hands_occupied) < num_hands_to_use[univ.party[pc_num].items[item_num].variety])
							add_string_to_buf("Equip: Not enough free hands");
							else if (num_that_can_equip[univ.party[pc_num].items[item_num].variety] <= num_equipped_of_this_type)
								add_string_to_buf("Equip: Can't equip another");
								else {
									univ.party[pc_num].equip[item_num] = true;
									add_string_to_buf("Equip: OK");
									}
					}
			
		}
	}
	if (stat_window == pc_num)
		put_item_screen(stat_window,1);
}


void drop_item(short pc_num,short item_num,location where_drop)
{
	std::string choice;
	short how_many = 0;
	cItemRec item_store;
	bool take_given_item = true;
	location loc;
	
	item_store = univ.party[pc_num].items[item_num];

	if (univ.party[pc_num].equip[item_num] && univ.party[pc_num].items[item_num].cursed) 
			add_string_to_buf("Drop: Item is cursed.           ");	
	else switch (overall_mode) {
		case MODE_OUTDOORS:
			choice = cChoiceDlog("drop-item-confirm.xml",{"okay","cancel"}).show();
			if (choice == "cancel")
				return;
			add_string_to_buf("Drop: OK");
			if ((item_store.type_flag > 0) && (item_store.charges > 1)) {
				how_many = get_num_of_items(item_store.charges);
				if (how_many == item_store.charges)
					take_item(pc_num,item_num);
					else univ.party[pc_num].items[item_num].charges -= how_many;
				}
				else take_item(pc_num,item_num);
			break;
		
		case MODE_DROP_TOWN: case MODE_DROP_COMBAT:
			loc = where_drop;
			if ((item_store.type_flag > 0) && (item_store.charges > 1)) {
				how_many = get_num_of_items(item_store.charges);
				if (how_many <= 0)
					return;
				if (how_many < item_store.charges)
					take_given_item = false;
				item_store.charges = how_many;
				}
			if (is_container(loc) == true)
				item_store.contained = true;
			if (!place_item(item_store,loc,false)) {
				add_string_to_buf("Drop: Too many items on ground");
				item_store.contained = false;
				}
				else {
					if (item_store.contained)
						add_string_to_buf("Drop: Item put away");
						else add_string_to_buf("Drop: OK");
					univ.party[pc_num].items[item_num].charges -= how_many;
					if (take_given_item)
						take_item(pc_num,item_num);
					}	
			break;
		default: //should never be reached
			break;
		}
}

bool place_item(cItemRec item,location where,bool forced)
{
	short i;
	
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (univ.town.items[i].variety == 0) {
			univ.town.items[i] = item;
			univ.town.items[i].item_loc = where;
			reset_item_max();
			return true;
			}
	if (forced == false)
		return false;
	destroy_an_item();
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (univ.town.items[i].variety == 0) {
			univ.town.items[i] = item;
			univ.town.items[i].item_loc = where;
			reset_item_max();
			return true;
			}
	
	return true;
}

void destroy_an_item()
{
	short i;
	ASB("Too many items. Some item destroyed.");
//	for (i = 0; i < NUM_TOWN_ITEMS; i++)
//		if (univ.town.items[i].type_flag == 15) {
//			univ.town.items[i].variety = ITEM_TYPE_NO_ITEM;
//			return;
//			}
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (univ.town.items[i].value < 3) {
			univ.town.items[i].variety = ITEM_TYPE_NO_ITEM;
			return;
			}
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (univ.town.items[i].value < 30) {
			univ.town.items[i].variety = ITEM_TYPE_NO_ITEM;
			return;
			}
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (!univ.town.items[i].magic) {
			univ.town.items[i].variety = ITEM_TYPE_NO_ITEM;
			return;
			}
	i = get_ran(1,0,NUM_TOWN_ITEMS);
	univ.town.items[i].variety = ITEM_TYPE_NO_ITEM;
	
}

void give_thing(short pc_num, short item_num)
{
	short who_to,how_many = 0;
	cItemRec item_store;
	bool take_given_item = true;
	
	if (univ.party[pc_num].equip[item_num] && univ.party[pc_num].items[item_num].cursed)
			add_string_to_buf("Give: Item is cursed.           ");
			else {
				item_store = univ.party[pc_num].items[item_num];
				who_to = char_select_pc(1,1,"Give item to who?");
				if ((overall_mode == MODE_COMBAT) && (adjacent(pc_pos[pc_num],pc_pos[who_to]) == false)) {
					add_string_to_buf("Give: Must be adjacent.");
					who_to = 6;
					}

				if ((who_to < 6) && (who_to != pc_num) 
					&& ((overall_mode != MODE_COMBAT) || (adjacent(pc_pos[pc_num],pc_pos[who_to]) == true))) {
					if ((item_store.type_flag > 0) && (item_store.charges > 1)) {
						how_many = get_num_of_items(item_store.charges);
						if (how_many == 0)
							return;
						if (how_many < item_store.charges)
							take_given_item = false;
						univ.party[pc_num].items[item_num].charges -= how_many;
						item_store.charges = how_many;
						}					
					if (give_to_pc(who_to,item_store,0) == true) {
						if (take_given_item)
							take_item(pc_num,item_num);	
						}
						else {
							if (pc_has_space(who_to) == 24)
								ASB("Can't give: PC has max. # of items.");
								else ASB("Can't give: PC carrying too much.");
							if (how_many > 0)
								univ.party[pc_num].items[item_num].charges += how_many;
							}				
				}
		}
}

void combine_things(short pc_num)
{
	short i,j,test;
	
	for (i = 0; i < 24; i++) {
		if ((univ.party[pc_num].items[i].variety > 0) &&
			(univ.party[pc_num].items[i].type_flag > 0) && (univ.party[pc_num].items[i].ident)) {
			for (j = i + 1; j < 24; j++)
				if ((univ.party[pc_num].items[j].variety > 0) &&
				(univ.party[pc_num].items[j].type_flag == univ.party[pc_num].items[i].type_flag) 
				 && (univ.party[pc_num].items[j].ident)) {
					add_string_to_buf("(items combined)");
					test = (short) (univ.party[pc_num].items[i].charges) + (short) (univ.party[pc_num].items[j].charges);
					if (test > 125) {
						univ.party[pc_num].items[i].charges = 125;
						ASB("(Can have at most 125 of any item.");
						}
				 		else univ.party[pc_num].items[i].charges += univ.party[pc_num].items[j].charges;
				 	if (univ.party[pc_num].equip[j] == true) {
				 		univ.party[pc_num].equip[i] = true;
				 		univ.party[pc_num].equip[j] = false;
				 		}
					take_item(pc_num,30 + j);
				 	}
			}		
		if ((univ.party[pc_num].items[i].variety > 0) && (univ.party[pc_num].items[i].charges < 0))
			univ.party[pc_num].items[i].charges = 1;
		}			
}

// Procedure only ready for town and outdoor
short dist_from_party(location where)
{
	short store = 1000, i;
	
	if ((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING)) {
		for (i = 0; i < 6; i++)
			if (univ.party[i].main_status == 1)
				store = min(store,dist(pc_pos[i],where));
		}
		else store = dist(univ.town.p_loc,where);
		
	return store;
}

void set_item_flag(cItemRec *item)
{
	if ((item->is_special > 0) && (item->is_special < 65)) {
		item->is_special--;
		univ.party.item_taken[univ.town.num][item->is_special / 8] =
			univ.party.item_taken[univ.town.num][item->is_special / 8] | s_pow(2,item->is_special % 8);
		item->is_special = 0;
		}
}

short get_item(location place,short pc_num,bool check_container)
//short pc_num; // if 6, any   
{
	short i,taken = 0;
	bool item_near = false;
	short mass_get = 1;
	
	for (i = 0; i < univ.town->max_monst(); i++)
		if ((univ.town.monst[i].active > 0) && (univ.town.monst[i].attitude == 1)
			&& (can_see(place,univ.town.monst[i].cur_loc,0) < 5))
				mass_get = 0;
		
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (univ.town.items[i].variety != 0)
			if (((adjacent(place,univ.town.items[i].item_loc) == true) || 
			 ((mass_get == 1) && (check_container == false) &&
			 ((dist(place,univ.town.items[i].item_loc) <= 4) || ((is_combat()) && (which_combat_type == 0)))
			  && (can_see(place,univ.town.items[i].item_loc,0) < 5))) 
			  && ((!univ.town.items[i].contained) || (check_container == true))) {
				taken = 1;

					if (univ.town.items[i].value < 2)
						univ.town.items[i].ident = true;
					item_near = true;
				}
	if (item_near == true)
		if (display_item(place,pc_num,mass_get,check_container) > 0) { // if true, there was a theft
			for (i = 0; i < univ.town->max_monst(); i++)
				if ((univ.town.monst[i].active > 0) && (univ.town.monst[i].attitude % 2 != 1)
					&& (can_see(place,univ.town.monst[i].cur_loc,0) < 5)) {
						make_town_hostile();
						i = univ.town->max_monst();
						add_string_to_buf("Your crime was seen!");
						}
			}
				
	if (pc_num != 10) {
		if (taken == 0)
			add_string_to_buf("Get: nothing here");
			else add_string_to_buf("Get: OK");
		}

	reset_item_max();

	return taken;
				
}

void make_town_hostile()
{
	short i,num;
	bool fry_party = false;
	
	if (which_combat_type == 0)
		return;
	give_help(53,0);
	univ.town.monst.friendly = 1;
	////
	for (i = 0; i < univ.town->max_monst(); i++) 
		if ((univ.town.monst[i].active > 0) && (univ.town.monst[i].summoned == 0)){
			univ.town.monst[i].attitude = 1;
			num = univ.town.monst[i].number;
			univ.town.monst[i].mobility = 1;
			if (scenario.scen_monsters[num].spec_skill == 37) {
				univ.town.monst[i].active = 2;
				
				// If a town, give pwoer boost
				univ.town.monst[i].health *= 3;
				univ.town.monst[i].status[3] = 8;
				univ.town.monst[i].status[1] = 8;
				}
			}
			
// In some towns, doin' this'll getcha' killed.
//// wedge in special

	if (fry_party == true) {
		for (i = 0; i < 6; i++)
			if (univ.party[i].main_status > MAIN_STATUS_ABSENT)
				univ.party[i].main_status = MAIN_STATUS_ABSENT;
		stat_window = 6;
		boom_anim_active = false;	
		}
}


void put_item_graphics(cDialog& me)
{
	short i,storage;
	cItemRec item;
	char message[256];

	// First make sure all arrays for who can get stuff are in order.
	if ((current_getting_pc < 6) && ((univ.party[current_getting_pc].main_status != 1) 
	 || (pc_has_space(current_getting_pc) == 24))) {
	 	current_getting_pc = 6;
	 	
	 	}
	 	
	for (i = 0; i < 6; i++) {
		std::ostringstream sout;
		sout << "pc" << i + 1;
		std::string id = sout.str();
		if ((univ.party[i].main_status == 1) && (pc_has_space(i) < 24)
		 && ((!is_combat()) || (current_pc == i))) {
			if (current_getting_pc == 6)
				current_getting_pc = i;
			me[id].show();
		} else {
			me[id].hide();
			sout << "-g";
			me[sout.str()].hide();
		}
		if(current_getting_pc == i)
			me.addLabelFor(id, "*   ", LABEL_LEFT, 7, true);
		else me.addLabelFor(id,"    ", LABEL_LEFT, 7, true);
	}
			
	// darken arrows, as appropriate
	if (first_item_shown == 0)
		me["up"].hide();
	else me["up"].show();
	if ((first_item_shown > total_items_gettable - 7) || 
		(total_items_gettable <= 8) )
		me["down"].hide();
	else me["down"].show();

	for (i = 0; i < 8; i++) {
		std::ostringstream sout;
		sout << "item" << i + 1;
		std::string pict = sout.str() + "-g", name = sout.str() + "-name";
		std::string detail = sout.str() + "-detail", weight = sout.str() + "-weight";

		if (item_array[i + first_item_shown] != 200) { // display an item in window
			me[pict].show();
			item = univ.town.items[item_array[i + first_item_shown]];
			me[name].setText(item.ident ? item.full_name : item.name);
			// TODO: Party sheet items
			cPict& pic = dynamic_cast<cPict&>(me[pict]);
			if(item.graphic_num >= 1000)
				pic.setPict(item.graphic_num - 1000, PIC_CUSTOM_ITEM);
			else pic.setPict(item.graphic_num, PIC_ITEM);
			// TODO: This code is currently kept here for reference to the changed numbers. It can be removed after verifying it works correctly.
#if 0
					if (item.graphic_num >= 1000) // was 150
						csp(987,20 + i * 4,/*3000 + 2000 + */item.graphic_num - 1000,PICT_CUSTOM + PICT_ITEM);
					else csp(987,20 + i * 4,/*4800 + */item.graphic_num,PICT_ITEM);
#endif
			get_item_interesting_string(item,(char *) message);
			me[detail].setText(message);
			storage = item.item_weight();
			sprintf ((char *) message, "Weight: %d",storage);
			me[weight].setText(message);
		} else { // erase the spot
			me[pict].hide();
			me[name].setText("");
			me[detail].setText("");
			me[weight].setText("");
		}
	}
	
	if (current_getting_pc < 6) {
		i = amount_pc_can_carry(current_getting_pc);
		storage = pc_carry_weight(current_getting_pc);
		sprintf ((char *) message, "%s is carrying %d out of %d.",univ.party[current_getting_pc].name.c_str(),storage,i);
		me["prompt"].setText(message);
	}
		
	for (i = 0; i < 6; i++) 
		if (univ.party[i].main_status == 1) {
			std::ostringstream sout;
			sout << "pc" << i + 1 << "-g";
			dynamic_cast<cPict&>(me[sout.str()]).setPict(univ.party[i].which_graphic);
		}
}


bool display_item_event_filter(cDialog& me, std::string id, eKeyMod mods)
{
	cItemRec item;
	short i;
	
	if(id == "done") {
		me.toast();
	} else if(id == "up") {
		if(first_item_shown > 0) {
			first_item_shown -= 8;
			put_item_graphics(me);
		}
	} else if(id ==  "down") {
		// TODO: This 116 is a magic number, and a limit to be removed
		if (first_item_shown < 116) {
			first_item_shown += 8;
			put_item_graphics(me);
		}
	} else if(id.substr(0,2) == "pc") {
		current_getting_pc = id[2] - '1';
		put_item_graphics(me);
	} else {
		if(current_getting_pc == 6) return true;
		short item_hit;
		item_hit = id[4] - '1';
		item_hit += first_item_shown;
		if(item_array[item_hit] >= NUM_TOWN_ITEMS)
			return true;
		item = univ.town.items[item_array[item_hit]];
		if(item.property) {
			if(me.getResult<bool>()) {
				std::string choice = cChoiceDlog("steal-item.xml",{"steal","leave"}).show();
				if(choice == "leave") return true;
				me.setResult(true);
			}
			item.property = false;
		}
		
		if(univ.town.items[item_array[item_hit]].variety == 3) {
			if(univ.town.items[item_array[item_hit]].item_level > 3000)
				univ.town.items[item_array[item_hit]].item_level = 3000;
			set_item_flag(&item);
			give_gold(univ.town.items[item_array[item_hit]].item_level,false);
			play_sound(39); // formerly force_play_sound
		} else if(univ.town.items[item_array[item_hit]].variety == 11) {
			give_food(univ.town.items[item_array[item_hit]].item_level,false);
			set_item_flag(&item);
			set_item_flag(&univ.town.items[item_array[item_hit]]);
			play_sound(62); // formerly force_play_sound
		} else {
			if(item.item_weight() > amount_pc_can_carry(current_getting_pc) - pc_carry_weight(current_getting_pc)) {
				// TODO: Play an error sound here
				me["prompt"].setText("It's too heavy to carry.");
				give_help(38,0,me);
				return true;
			}
			
			set_item_flag(&item);
			play_sound(0); // formerly force_play_sound
			give_to_pc(current_getting_pc, item, 0);////
		}
		univ.town.items[item_array[item_hit]] = cItemRec();
		for(short i = item_hit; i < 125; i++)
			item_array[i] = item_array[i + 1];
		total_items_gettable--;
		put_item_graphics(me);
	}
		
}

// TODO: Move this to a more appropriate place
bool pc_gworld_loaded = false;

// Returns true is a theft committed
bool display_item(location from_loc,short pc_num,short mode, bool check_container)
//pc_num;  // < 6 - this pc only  6 - any pc
//short mode; // 0 - adjacent  1 - all in sight
{
	short i,array_position = 0;
	
	make_cursor_sword();
	
	first_item_shown = 0;
	store_get_mode = mode;
	current_getting_pc = current_pc;
	store_pcnum = pc_num;
	
	for (i = 0; i < 130; i++)
		item_array[i] = 200;
	
	total_items_gettable = 0;
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (univ.town.items[i].variety != 0) {
			if (((adjacent(from_loc,univ.town.items[i].item_loc) == true) || 
				 ((mode == 1) && (check_container == false) &&
				 ((dist(from_loc,univ.town.items[i].item_loc) <= 4) || ((is_combat()) && (which_combat_type == 0)))
				  && (can_see(from_loc,univ.town.items[i].item_loc,0) < 5))) &&
				  (univ.town.items[i].contained == check_container) &&
				  ((!check_container) || (univ.town.items[i].item_loc == from_loc))) {
				  	item_array[array_position] = i;
			  		array_position++;
			  		total_items_gettable++;
			  		}
			}

	if (!pc_gworld_loaded)
		pc_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcs"));
	
	cDialog itemDialog("get-items.xml");
	itemDialog.attachClickHandlers(display_item_event_filter, {"done", "up", "down"});
	itemDialog.attachClickHandlers(display_item_event_filter, {"pc1", "pc2", "pc3", "pc4", "pc5", "pc6"});
	itemDialog.setResult(false);
	cTextMsg& title = dynamic_cast<cTextMsg&>(itemDialog["title"]);

	if(check_container == true)
		title.setText("Looking in container:");
	else if(mode == 0)
		title.setText("Getting all adjacent items:");
	else title.setText("Getting all nearby items:");
	
	for(i = 1; i <= 8; i++) {
		std::ostringstream sout;
		sout << "item" << i << "-key";
		itemDialog[sout.str()].attachKey({false, static_cast<unsigned char>('`' + i), mod_none});
		itemDialog[sout.str()].attachClickHandler(display_item_event_filter);
	}
	put_item_graphics(itemDialog);

	if (univ.party.help_received[36] == 0) {
		// TODO: Not sure if I need to an initial draw
//		cd_initial_draw(987);
		give_help(36,37,itemDialog);
	}
	
	itemDialog.run();
	
	put_item_screen(stat_window,0);
	put_pc_screen();
	
	return itemDialog.getResult<bool>();
			

}


//void fancy_choice_dialog_event_filter (short item_hit)
//{
//	toast_dialog();
//	dialog_answer = item_hit;
//}

short custom_choice_dialog(std::array<std::string, 6>& strs,short pic_num,ePicType pic_type,std::array<short, 3>& buttons) ////
{
	make_cursor_sword();
	
	std::vector<std::string> vec;
	std::copy(strs.begin(), strs.end(), std::inserter(vec, vec.begin()));
	cThreeChoice customDialog(vec, buttons, pic_num, pic_type);
	std::string item_hit = customDialog.show();
	
	redraw_screen();
	
	for(int i = 0; i < 3; i++) {
		cBasicButtonType& btn = basic_buttons[available_btns[i]];
		if(btn && item_hit == btn->label)
			return i;
	}
	}

//short fancy_choice_dialog(short which_dlog,short parent)
//// ignore parent in Mac version
//{
//	short item_hit,i,store_dialog_answer,err;
//	
//	store_dialog_answer = dialog_answer;
//	make_cursor_sword();
//	
//	err = cd_create_dialog_parent_num(which_dlog,parent);
//	if(err != 0)
//		printf("Error %i while creating dialog %i.\n",err,which_dlog);
//	
//	if (which_dlog == 1062) {
//		//i = get_ran(1,0,12);
//		//get_str(temp_str,11,10 + i);
//		//csit(1062,10,(char *) temp_str);
//	}
//	
//	item_hit = cd_run_dialog();
//	cd_kill_dialog(which_dlog,0);
//
//	if (parent < 2) {
//		SetPort(GetWindowPort(mainPtr));
//		BeginUpdate(mainPtr);
//		if (in_startup_mode == false)
//			refresh_screen(0);
//			else draw_startup(0);
//		EndUpdate(mainPtr);
//		}
//	i = dialog_answer;
//	dialog_answer = store_dialog_answer;
//	
//	return i;
//}

bool select_pc_event_filter (cDialog& me, std::string item_hit, eKeyMod mods)
{
	me.toast();
	if(item_hit != "cancel") {
		short which_pc = item_hit[item_hit.length() - 1] - '1';
		me.setResult<short>(which_pc);
	} else me.setResult<short>(6);
	return true;
}

short char_select_pc(short active_only,short free_inv_only,const char *title)
//active_only;  // 0 - no  1 - yes   2 - disarm trap   
{
	short item_hit,i;

	make_cursor_sword();
	
	cDialog selectPc("select-pc.xml");
	selectPc.attachClickHandlers(select_pc_event_filter, {"cancel", "pick1", "pick2", "pick3", "pick4", "pick5", "pick6"});
	
	selectPc["title"].setText(title);
	
	for (i = 0; i < 6; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if ((univ.party[i].main_status == 0) ||
			((active_only == true) && (univ.party[i].main_status > 1)) ||
			((free_inv_only == 1) && (pc_has_space(i) == 24)) || (univ.party[i].main_status == 5)) {
			selectPc["pick" + n].hide();
				}
		// TODO: Wouldn't this lead to blank name fields for non-active characters if those characters are allowed?
		if (univ.party[i].main_status != 0) {
			selectPc["pc" + n].setText(univ.party[i].name);
			}		
		else selectPc["pc" + n].hide();
	}
	
	selectPc.run();
	item_hit = selectPc.getResult<short>();

	if (in_startup_mode == false)
		//refresh_screen(0); 
		redraw_screen();
		else draw_startup(0);

	return item_hit;
}

short select_pc(short active_only,short free_inv_only)
//active_only;  // 0 - no  1 - yes   2 - disarm trap   
{
	if (active_only == 2)
		return char_select_pc(active_only,free_inv_only,"Trap! Who will disarm?");
		else return char_select_pc(active_only,free_inv_only,"Select a character:");
}

bool get_num_of_items_event_filter(cDialog& me, std::string item_hit, eKeyMod mods)
{
	me.setResult<int>(me["number"].getTextAsNum());
	return true;
}

short get_num_of_items(short max_num)
//town_num; // Will be 0 - 200 for town, 200 - 290 for outdoors
//short sign_type; // terrain type
{

	short item_hit;
	char sign_text[256];

	make_cursor_sword();
	
	cDialog numPanel("get-num.xml");
	numPanel.attachClickHandlers(get_num_of_items_event_filter, {"okay"});
		
	sprintf((char *) sign_text,"How many? (0-%d) ",max_num);
	numPanel["prompt"].setText(sign_text);
	numPanel["number"].setTextToNum(max_num);
	numPanel.run();
	
	return minmax(0,max_num,numPanel.getResult<int>());
}

void init_mini_map() {
	// TODO: I'm not sure if the bounds in the DLOG resource included the titlebar height; perhaps the actual height should be a little less
	mini_map.create(sf::VideoMode(296,277), "Map", sf::Style::Titlebar | sf::Style::Close);
	mini_map.setPosition(sf::Vector2i(52,62));
	mini_map.setVisible(false);
	setWindowFloating(mini_map, true);
	makeFrontWindow(mainPtr);
}

void make_cursor_watch() 
{
	// TODO: If this doesn't work, which I suspect it won't, add a "custom" watch cursor and use that. Or conclude this isn't even needed.
	SetNSCursorWatch();
}

////
void place_glands(location where,m_num_t m_type)
{
	cItemRec store_i;
	cMonster monst;
	
	monst = scenario.scen_monsters[m_type];
	
	if ((monst.corpse_item >= 0) && (monst.corpse_item < 400) && (get_ran(1,1,100) < monst.corpse_item_chance)) {
		store_i = get_stored_item(monst.corpse_item);
		place_item(store_i,where,false);
		}
}

short party_total_level() 
{
	short i,j = 0;
	
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1)
			j += univ.party[i].level;
	return j;
}

void reset_item_max()
{
	short i;

	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (univ.town.items[i].variety != 0)
			item_max = i + 1;
}

short item_val(cItemRec item)
{
	if (item.charges == 0)
		return item.value;
	return item.charges * item.value;
}

//// rewrite
void place_treasure(location where,short level,short loot,short mode)
//short mode;  // 0 - normal, 1 - force
{

	cItemRec new_item;
	short amt,r1,i,j;
	// Make these static const because they are never changed.
	// Saves them being initialized every time the function is called.
	static const short treas_chart[5][6] = {
		{0,-1,-1,-1,-1,-1},
		{1,-1,-1,-1,-1,-1},
		{2,1,1,-1,-1,-1},
		{3,2,1,1,-1,-1},
		{4,3,2,2,1,1}
	};
	static const short treas_odds[5][6] = {
		{10,0,0,0,0,0},
		{50,0,0,0,0,0},
		{60,50,40,0,0,0},
		{100,90,80,70,0,0},
		{100,80,80,75,75,75}
	};
	static const short id_odds[21] = {
		0,10,15,20,25,30,35,
		39,43,47,51,55,59,63,
		67,71,73,75,77,79,81
	};
	static const short max_mult[5][10] = {
		{0,0,0,0,0,0,0,0,0,1},
		{0,0,1,1,1,1,2,3,5,20},
		{0,0,1,1,2,2,4,6,10,25},
		{5,10,10,10,15,20,40,80,100,100},
		{25,25,50,50,50,100,100,100,100,100}
	};
	static const short min_chart[5][10] = {
		{0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,5,20},
		{0,0,0,0,1,1,5,10,15,40},
		{10,10,15,20,20,30,40,50,75,100},
		{50,100,100,100,100,200,200,200,200,200}
	};
	short max,min;
	
	if (loot == 1)
		amt = get_ran(2,1,7) + 1;
		else amt = loot * (get_ran(1,0,10 + (loot * 6) + (level * 2)) + 5);

	if (party_total_level() <= 12)
		amt += 1;	
	if ((party_total_level() <= 60)	&& (amt > 2))
		amt += 2;
		
	if (amt > 3) {	
			new_item = get_stored_item(0);
			new_item.item_level = amt;
			r1 = get_ran(1,1,9);
			if (((loot > 1) && (r1 < 7)) || ((loot == 1) && (r1 < 5)) || (mode == 1)
				|| ((r1 < 6) && (party_total_level() < 30)) || (loot > 2) )
				place_item(new_item,where,false);
		}
	for (j = 0; j < 5; j++) {
		r1 = get_ran(1,1,100);
		if ((treas_chart[loot][j] >= 0) && (r1 <= treas_odds[loot][j] + luck_total())) {
			r1 = get_ran(1,0,9);
			min = min_chart[treas_chart[loot][j]][r1];
			r1 = get_ran(1,0,9);
			max = (min + level + (2 * (loot - 1)) + (luck_total() / 3)) * max_mult[treas_chart[loot][j]][r1];
			if (get_ran(1,0,1000) == 500) {
				max = 10000;
				min = 100;
				}
				
			// reality check
			if ((loot == 1) && (max > 100) && (get_ran(1,0,8) < 7))
				max = 100;
			if ((loot == 2) && (max > 200) && (get_ran(1,0,8) < 6))
				max = 200;
				
				
				new_item = return_treasure(treas_chart[loot][j],level,mode);
				if ((item_val(new_item) < min) || (item_val(new_item) > max)) {
					new_item = return_treasure(treas_chart[loot][j],level,mode);
					if ((item_val(new_item) < min) || (item_val(new_item) > max)) {
						new_item = return_treasure(treas_chart[loot][j],level,mode);
						if (item_val(new_item) > max)
							new_item.variety = ITEM_TYPE_NO_ITEM;
						}
					}

			// not many magic items
			if (mode == 0) {
				if (new_item.magic && (level < 2) && (get_ran(1,0,5) < 3))
					new_item.variety = ITEM_TYPE_NO_ITEM;
				if (new_item.magic && (level == 2) && (get_ran(1,0,5) < 2))
					new_item.variety = ITEM_TYPE_NO_ITEM;
				if (new_item.cursed && (get_ran(1,0,5) < 3))
					new_item.variety = ITEM_TYPE_NO_ITEM;
				}
				
			// if forced, keep dipping until a treasure comes up
			if ((mode == 1)	&& (max >= 20)) {
				do new_item = return_treasure(treas_chart[loot][j],level,mode);
				while ((new_item.variety == 0) || (item_val(new_item) > max));
			}

			// Not many cursed items
			if (new_item.cursed && (get_ran(1,0,2) == 1))
				new_item.variety = ITEM_TYPE_NO_ITEM;
							
			if (new_item.variety != ITEM_TYPE_NO_ITEM) {
				for (i = 0; i < 6; i++)
					if ((univ.party[i].main_status == 1) 
						&& (get_ran(1,1,100) < id_odds[univ.party[i].skills[13]]))
							new_item.ident = true;
				place_item(new_item,where,false);
				}			
			}
		}
}

short luck_total()
{
	short i = 0;
	
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1)
			i += univ.party[i].skills[18];
	return i;
}

cItemRec return_treasure(short loot,short level,short mode)
//short mode; // 0 - normal  1 - force
{
	cItemRec treas;
	static const short which_treas_chart[48] = {
		1,1,1,1,1,2,2,2,2,2,
		3,3,3,3,3,2,2,2,4,4,
		4,4,5,5,5,6,6,6,7,7,
		7,8,8,9,9,10,11,12,12,13,
		13,14, 9,10,11,9,10,11
	};
	short r1;
	
	treas.variety = ITEM_TYPE_NO_ITEM;
	r1 = get_ran(1,0,41);
	if (loot >= 3)
		r1 += 3;
	if (loot == 4)
		r1 += 3;
	switch (which_treas_chart[r1]) {
		case 1: treas = get_food(); break;
		case 2: treas = get_weapon(loot,level);	break;
		case 3: treas = get_armor(loot,level); break;
		case 4: treas = get_shield(loot); break;
		case 5: treas = get_helm(loot); break;
		case 6: treas = get_missile(loot); break;
		case 7: treas = get_potion(loot); break;
		case 8: treas = get_scroll(loot); break;
		case 9: treas = get_wand(loot); break; 
		case 10: treas = get_ring(loot); break;
		case 11: treas = get_necklace(loot); break;
		case 12: treas = get_poison(loot,level); break;
		case 13: treas = get_gloves(loot); break;
		case 14: treas = get_boots(loot); break;
		} 
	if (treas.variety == 0)
		treas.value = 0;	
	return treas;

}

void refresh_store_items()
{
	short i,j;
	short loot_index[10] = {1,1,1,1,2,2,2,3,3,4};
	
	for (i = 0; i < 5; i++)
		for (j = 0; j < 10; j++) {
			univ.party.magic_store_items[i][j] = return_treasure(loot_index[j],7,1);
			if ((univ.party.magic_store_items[i][j].variety == 3) ||
				(univ.party.magic_store_items[i][j].variety == 11))
				univ.party.magic_store_items[i][j] = cItemRec();
			univ.party.magic_store_items[i][j].ident = true;
		}

}


bool get_text_response_event_filter(cDialog& me, std::string item_hit, eKeyMod mods)
{
	me.toast();
	me.setResult(me["response"].getText());
	return true;
}

std::string get_text_response(short dlg,short parent_num)
{

	short item_hit,i;

	make_cursor_sword();
	
	cDialog strPanel("get-response.xml");
	strPanel.attachClickHandlers(get_text_response_event_filter, {"okay"});
	// FIXME: A sort of hack to change the prompt and graphic if the other text response dialog was requested.
	if(dlg == 1017) {
		dynamic_cast<cPict&>(strPanel["pic"]).setPict(8);
		strPanel["prompt"].setText("Ask about what?");
	}
	
	strPanel.run();
	// Note: Originally it only changed the first 15 characters.
	std::string result = strPanel.getResult<std::string>();
	std::transform(result.begin(), result.end(), result.begin(), tolower);
	return result;
}


