#include <windows.h>

#include "global.h"

#include "graphics.h"
#include "text.h"
#include "dlogtool.h"
#include "items.h"
#include "party.h"
#include "fields.h"
#include "locutils.h"
#include "newgraph.h"
#include "dlogtool.h"
#include "itemdata.h"
#include "infodlgs.h"
#include "exlsound.h"
#include "stdio.h"
#include "string.h"
#include "graphutl.h"
#include "monster.h"

#include "globvar.h"

Boolean give_to_party(item_record_type item,short print_result)
{
	short i = 0;
	
	while (i < 6) {
		if (adven[i].giveToPC(item,print_result) == true)
			return TRUE;
		i++;
	}
	return FALSE;
}

Boolean forced_give(short item_num,short abil)
// if abil > 0, force abil, else ignore
{
	short i,j;
	item_record_type item;
	char announce_string[60];

	if ((item_num < 0) || (item_num > 399))
		return TRUE;
	item = get_stored_item(item_num);
	if (abil > 0)
		item.ability = abil;
	for (i = 0; i < 6; i++)
		for (j = 0; j < 24; j++)
			if ((adven[i].isAlive()) && (adven[i].items[j].variety == 0)) {
				adven[i].items[j] = item;

				if (item.isIdent() == false)
					sprintf((char *) announce_string,"  %s gets %s.",adven[i].name,item.name);
					else sprintf((char *) announce_string,"  %s gets %s.",adven[i].name,item.full_name);
				add_string_to_buf((char *)announce_string);
				adven[i].combineThings();
				adven[i].sortItems();
				return TRUE;
				}
	return FALSE;
}

void party_record_type::giveGold(short amount, bool print_result)
{
	if (amount < 0) return;
	
	gold += amount;
	if (print_result) put_pc_screen();
}

bool party_record_type::takeGold(short amount, bool print_result)
{
	if (gold < amount) return false;
	
	gold -= amount;
	
	if (print_result) put_pc_screen();
	
	return true;
}



bool pc_array::hasAbil(short abil)
{
	short i;
	
	for (i = 0; i < NUM_OF_PCS; i++)
		if (pc[i].isAlive())
			if (pc[i].hasAbil(abil) < 24)
				return true;
	return false;
}

short item_weight(item_record_type item)
{
	if (item.variety == 0)
		return 0;
	if ((item.variety == 5) || (item.variety == 6) || (item.variety == 24) || (item.variety == 7)
		|| ((item.variety == 21) && (item.charges > 0)))
		return (short) (item.charges) * (short) (item.weight);
	return (short) (item.weight);
}

void party_record_type::giveFood(short amount, bool print_result)
{
	if (amount < 0) return;
	
	food += amount;
	
	if (print_result) put_pc_screen();	
}

short party_record_type::takeFood(short amount, bool print_result)
{
	short diff;

	diff = amount - food;
	
	if (diff > 0)
	{
		food = 0;
		if (print_result) put_pc_screen();
		return diff;
	}

	food -= amount;
	if (print_result) put_pc_screen();	
	return 0;	
}



Boolean place_item(item_record_type item,location where,Boolean forced)
{
	short i;
	
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].variety == 0) {
			t_i.items[i] = item;
			t_i.items[i].item_loc = where;
			reset_item_max();
			return TRUE;
			}
	if (forced == FALSE)
		return FALSE;
	destroy_an_item();
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].variety == 0) {
			t_i.items[i] = item;
			t_i.items[i].item_loc = where;
			reset_item_max();
			return TRUE;
			}
	
	return TRUE;
}

void destroy_an_item()
{
////
	short i;
	ASB("Too many items. Some item destroyed.");
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].type_flag == 15) {
			t_i.items[i].variety = 0;
			return;
			}
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].value < 3) {
			t_i.items[i].variety = 0;
			return;
			}
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].value < 30) {
			t_i.items[i].variety = 0;
			return;
			}
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].isMagic() == false) {
			t_i.items[i].variety = 0;
			return;
			}
	i = get_ran(1,0,NUM_TOWN_ITEMS);
	t_i.items[i].variety = 0;
	
}

void set_item_flag(item_record_type *item)
{
	if ((item->is_special > 0) && (item->is_special < 65)) {
		item->is_special--;
		party.item_taken[c_town.town_num][item->is_special / 8] =
			party.item_taken[c_town.town_num][item->is_special / 8] | s_pow(2,item->is_special % 8);
		item->is_special = 0;
		}
}

short get_item(location place,short pc_num,Boolean check_container)
//short pc_num; // if 6, any   
{
	short i,taken = 0;

	Boolean item_near = FALSE;
	short mass_get = 1;
	
	for (i = 0; i < T_M; i++)
		if ((c_town.monst.dudes[i].active > 0) && (c_town.monst.dudes[i].attitude == 1)
			&& (can_see(place,c_town.monst.dudes[i].m_loc,0) < 5))
				mass_get = 0;
		
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].variety != 0)
			if (((adjacent(place,t_i.items[i].item_loc) == TRUE) || 
			 ((mass_get == 1) && (check_container == FALSE) &&
			 ((dist(place,t_i.items[i].item_loc) <= 4) || ((is_combat()) && (which_combat_type == 0)))
			  && (can_see(place,t_i.items[i].item_loc,0) < 5))) 
			  && ((t_i.items[i].isContained() == false) || (check_container == TRUE))) {
				taken = 1;

					if (t_i.items[i].value < 2)
						t_i.items[i].item_properties = t_i.items[i].item_properties | 1;
					item_near = TRUE;
				}
	if (item_near == TRUE)
		if (display_item(place,pc_num,mass_get,(bool) check_container) > 0) { // if true, there was a theft
			for (i = 0; i < T_M; i++)
				if ((c_town.monst.dudes[i].active > 0) && (c_town.monst.dudes[i].attitude % 2 != 1)
					&& (can_see(place,c_town.monst.dudes[i].m_loc,0) < 5)) {
						make_town_hostile();
						i = T_M;
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
	Boolean fry_party = FALSE;

	if (which_combat_type == 0)
		return;
	give_help(53,0,0);
	c_town.monst.friendly = 1;
	////
	for (i = 0; i < T_M; i++)
		if ((c_town.monst.dudes[i].active > 0) && (c_town.monst.dudes[i].summoned == 0)){
			c_town.monst.dudes[i].attitude = 1;
			num = c_town.monst.dudes[i].number;
			c_town.monst.dudes[i].mobile = TRUE;
			if (scenario.scen_monsters[num].spec_skill == 37) {
				c_town.monst.dudes[i].active = 2;
				
				// If a town, give power boost
				c_town.monst.dudes[i].m_d.health *= 3;
				c_town.monst.dudes[i].m_d.status[3] = 8;
				c_town.monst.dudes[i].m_d.status[1] = 8;
				}
			}
			
// In some towns, doin' this'll getcha' killed.
// wedge in special

	if (fry_party == TRUE) {
		for (i = 0; i < 6; i++)
			if (adven[i].main_status > MAIN_STATUS_ABSENT)
				adven[i].main_status = MAIN_STATUS_ABSENT;
		stat_window = 6;
		boom_anim_active = FALSE;	
		}
}


void put_item_graphics()
{
	short i,storage;

	item_record_type item;
	char message[256];

	// First make sure all arrays for who can get stuff are in order.
	if ((current_getting_pc < 6) && ((adven[current_getting_pc].isAlive() == false) 
	 || (adven[current_getting_pc].hasSpace() == 24)))
	 {
	 	current_getting_pc = 6;
		}
	for (i = 0; i < 6; i++)
		if ((adven[i].isAlive()) && (adven[i].hasSpace() < 24)
		 && ((!is_combat()) || (current_pc == i))) {
			if (current_getting_pc == 6)
				current_getting_pc = i;
			cd_activate_item(987,3 + i,1);
			}
			else {
				cd_activate_item(987,3 + i,0);
				cd_activate_item(987,11 + i,0);
				}
	for (i = 0; i < NUM_OF_PCS; i++)
		if (current_getting_pc == i)
			cd_add_label(987,3 + i,"*   ",1007);
			else cd_add_label(987,3 + i,"    ",1007); 
			
	// darken arrows, as appropriate
	if (first_item_shown == 0)
		cd_activate_item(987,9,0);
		else cd_activate_item(987,9,1);
	if ((first_item_shown > total_items_gettable - 7) || 
		(total_items_gettable <= 8) )
		cd_activate_item(987,10,0);
		else cd_activate_item(987,10,1); 

	for (i = 0; i < 8; i++) {
		// first, clear whatever item graphic is there
		csp(987,20 + i * 4,950);

		if (item_array[i + first_item_shown] != 200) { // display an item in window
			item = t_i.items[item_array[i + first_item_shown]]; 

					sprintf ((char *) message, "%s",
					 (item.isIdent()) ? (char *) item.full_name : (char *) item.name);
					csit(987,21 + i * 4,(char *) message);
					if (item.graphic_num >= 150)
						csp(987,20 + i * 4,3000 + 2000 + item.graphic_num - 150);
						else csp(987,20 + i * 4,4800 + item.graphic_num);////
					get_item_interesting_string(item,(char *) message);
					csit(987,22 + i * 4,(char *) message);
					storage = item_weight(item);
					sprintf ((char *) message, "Weight: %d",storage);
					csit(987,53 + i,(char *) message);

		  			}
			else { // erase the spot
				sprintf ((char *) message, "");
				csit(987,21 + i * 4,(char *) message);
				csit(987,22 + i * 4,(char *) message);
				csit(987,53 + i,(char *) message);
				}
		}
	
	if (current_getting_pc < 6) {
		i = adven[current_getting_pc].amountCanCarry();
		storage = adven[current_getting_pc].amountCarried();
		sprintf ((char *) message, "%s is carrying %d out of %d.", adven[current_getting_pc].name,storage,i);
		csit(987,52,(char *) message);
		}
		
	for (i = 0; i < 6; i++) 
		if (adven[i].isAlive()) {
			csp(987,11 + i,800 + adven[i].which_graphic);
			}
}


void display_item_event_filter (short item_hit)
{
	item_record_type item;
	short i;
	
		switch (item_hit) {
			case 1:
				dialog_not_toast = FALSE;
				break;
			case 9:
				if (first_item_shown > 0)
					first_item_shown -= 8;
				put_item_graphics();					
				break;
			case 10:
				if (first_item_shown < 116)
					first_item_shown += 8;
				put_item_graphics();					
				break;
			case 3: case 4: case 5: case 6:case 7: case 8: 
				current_getting_pc = item_hit - 3;
				put_item_graphics();					
				break;
			default:
				if (current_getting_pc == 6) {
					break;
					}
				item_hit = (item_hit - 19) / 4;
				item_hit += first_item_shown;
				if (item_array[item_hit] >= NUM_TOWN_ITEMS)
					break;
				item = t_i.items[item_array[item_hit]];
				if (item.isProperty())
				{
					i = (dialog_answer == 0) ? fancy_choice_dialog(1011,987) : 2;
					if (i == 1)
						break;
						else {
							dialog_answer = 1;
							item.item_properties = item.item_properties & 253;
  							}
				}


				if (t_i.items[item_array[item_hit]].variety == 3) {
					if (t_i.items[item_array[item_hit]].item_level > 3000)
						t_i.items[item_array[item_hit]].item_level = 3000;
				set_item_flag(&item);
					party.giveGold(t_i.items[item_array[item_hit]].item_level, false);
					force_play_sound(39);
					}
				else if (t_i.items[item_array[item_hit]].variety == 11) {
					party.giveFood(t_i.items[item_array[item_hit]].item_level, false);
 				set_item_flag(&item);
					set_item_flag(&t_i.items[item_array[item_hit]]);
					force_play_sound(62);
					}
  				else {
					if (item_weight(item) >
					  adven[current_getting_pc].amountCanCarry() - adven[current_getting_pc].amountCarried()) {
						MessageBeep(MB_OK);
						csit(987,52,"It's too heavy to carry.");
						give_help(38,0,987);
						break;
						}

 				set_item_flag(&item);
					force_play_sound(0);
					adven[current_getting_pc].giveToPC(item, 0);
					}
				t_i.items[item_array[item_hit]] = return_dummy_item();
				for (i = item_hit; i < 125; i++)
					item_array[i] = item_array[i + 1];
				total_items_gettable--;
				put_item_graphics();
				break;
				}

}


// Returns TRUE is a theft committed
short display_item(location from_loc,short pc_num, short mode, bool check_container)
//pc_num;  // < 6 - this pc only  6 - any pc
//short mode; // 0 - adjacent  1 - all in sight
{
	short i,array_position = 0;	
	SetCursor(sword_curs);
	
	first_item_shown = 0;
	store_get_mode = mode;
	current_getting_pc = current_pc;
	store_pcnum = pc_num;
	dialog_answer = 0;
	
	for (i = 0; i < 130; i++)
		item_array[i] = 200;
	
	total_items_gettable = 0;
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].variety != 0) {
			if (((adjacent(from_loc,t_i.items[i].item_loc) == TRUE) || 
				 ((mode == 1) && (check_container == false) &&
				 ((dist(from_loc,t_i.items[i].item_loc) <= 4) || ((is_combat()) && (which_combat_type == 0)))
				  && (can_see(from_loc,t_i.items[i].item_loc,0) < 5))) &&
				  (t_i.items[i].isContained() == check_container) &&
				  ((check_container == false) || (same_point(t_i.items[i].item_loc,from_loc) == TRUE))) {
				  	item_array[array_position] = i;
			  		array_position++;
			  		total_items_gettable++;
			  		}
			}

	cd_create_dialog(987,mainPtr);

	if (check_container == true)
		csit(987,17,"Looking in container:");
		else if (mode == 0)
		csit(987,17,"Getting all adjacent items:");
			else csit(987,17,"Getting all nearby items:");
	cd_set_flag(987,18,1);
	cd_set_flag(987,51,0);
	cd_set_flag(987,52,0);
	for (i = 0; i < 8; i++)
		cd_attach_key(987,19 + 4 * i,(char) (97 + i));
	put_item_graphics();

	if (party.help_received[36] == 0) {
		cd_initial_draw(987);
		give_help(36,37,987);
		}

	while (dialog_not_toast)
		ModalDialog();	
		
	cd_kill_dialog(987,0);

	put_item_screen(stat_window,0);
	put_pc_screen();
	
	return dialog_answer;
}

short custom_choice_dialog(char *strs,short pic_num,short buttons[3]) ////
{

	short i,store_dialog_answer;


	store_dialog_answer = dialog_answer;
	SetCursor(sword_curs);

	cd_create_custom_dialog(mainPtr,strs,pic_num, buttons);

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(900,0);

		if (in_startup_mode == FALSE)
			refresh_screen(0);
			else draw_startup(0);
	i = dialog_answer;
	dialog_answer = store_dialog_answer;

	return i;

	}

void fancy_choice_dialog_event_filter (short item_hit)
{
	dialog_not_toast = FALSE;
	dialog_answer = item_hit;
}

short fancy_choice_dialog(short which_dlog,short parent)
// ignore parent in Mac version
{
	short i,store_dialog_answer;
	char temp_str[256];
	
	store_dialog_answer = dialog_answer;
	SetCursor(sword_curs);
	
	cd_create_dialog_parent_num(which_dlog,parent);
	
	if (which_dlog == 1062) {
		i = get_ran(1,0,12);
		GetIndString(temp_str,11,10 + i);
		csit(1062,10,(char *) temp_str);
		}
	while (dialog_not_toast)
		ModalDialog();	
	
	cd_kill_dialog(which_dlog,0);

	if (parent < 2) {
		if (in_startup_mode == FALSE)
			refresh_screen(0);
			else draw_startup(0);
  		}
	i = dialog_answer;
	dialog_answer = store_dialog_answer;
	
	return i;
}

void select_pc_event_filter (short item_hit)
{
	dialog_not_toast = FALSE;
	if (item_hit == 16)
		dialog_answer = 6;
		else dialog_answer = item_hit - 3;
}

short char_select_pc(short active_only,short free_inv_only,char *title)
//active_only;  // 0 - no  1 - yes   2 - disarm trap   
{
	short i;

	SetCursor(sword_curs);
	
	cd_create_dialog(1018,mainPtr);
	
	if (active_only == 2)
		csit(1018,15,"Select PC to disarm trap:");
		else csit(	1018,15,title);
	
	for (i = 0; i < 6; i++) {
		if ((adven[i].main_status == MAIN_STATUS_ABSENT) ||
			((active_only == TRUE) && (adven[i].main_status > MAIN_STATUS_ALIVE)) ||
			((free_inv_only == 1) && (adven[i].hasSpace() == 24)) ||
			 (adven[i].main_status == MAIN_STATUS_FLED)) {
				cd_activate_item(1018, 3 + i, 0);
				}
		if (adven[i].main_status != MAIN_STATUS_ABSENT) {
				csit(1018,9 + i,adven[i].name);		
			}		
			else cd_activate_item(1018, 9 + i, 0);
		}
	
	while (dialog_not_toast)
		ModalDialog();	
	cd_kill_dialog(1018,0);

	if (in_startup_mode == FALSE)
		refresh_screen(0);
		else draw_startup(0);

	return dialog_answer;
}

short select_pc(short active_only,short free_inv_only)
//active_only;  // 0 - no  1 - yes   2 - disarm trap   
{
	if (active_only == 2)
		return char_select_pc(active_only,free_inv_only,"Trap! Who will disarm?");
		else return char_select_pc(active_only,free_inv_only,"Select a character:");
}

void get_num_of_items_event_filter (short)
{
	char get_text[256];

	cd_get_text_edit_str(1012,(char *) get_text);
	dialog_answer = 0;
	sscanf((char *) get_text,"%d",&dialog_answer);
	dialog_not_toast = FALSE;
}

short get_num_of_items(short max_num)
//town_num; // Will be 0 - 200 for town, 200 - 290 for outdoors
//short sign_type; // terrain type
{
	char sign_text[256];

	SetCursor(sword_curs);
	
	cd_create_dialog(1012,mainPtr);
		
	sprintf((char *) sign_text,"How many? (0-%d) ",max_num);
	csit(1012,4,(char *)sign_text);	
	sprintf((char *) sign_text,"%d",max_num);
	cd_set_text_edit_str(1012,(char *) sign_text);
	cd_set_edit_focus();
		
	while (dialog_not_toast)
		ModalDialog();	
	cd_kill_dialog(1012,0);
	
	dialog_answer = minmax(0,(int)max_num,dialog_answer);
	
	return dialog_answer;
}

void make_cursor_watch()
{
	SetCursor(LoadCursor(NULL,IDC_WAIT));
	ShowCursor(TRUE);
}

void place_glands(location where,unsigned char m_type)
{
	item_record_type store_i;
	monster_record_type monst;
	
	monst = return_monster_template(m_type);
	
	if ((monst.corpse_item >= 0) && (monst.corpse_item < 400) && (get_ran(1,1,100) <= monst.corpse_item_chance)) {
		store_i = get_stored_item(monst.corpse_item);
		place_item(store_i,where,FALSE);
		}
}

short pc_array::getTotalLevel() 
{
	short i, j = 0;
	
	for (i = 0; i < NUM_OF_PCS; i++)
		if (pc[i].isAlive())
			j += pc[i].level;
	return j;
}

void reset_item_max()
{
	short i;

	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].variety != 0)
			item_max = i + 1;
}

short item_val(item_record_type item)
{
	if (item.charges == 0)
		return item.value;
	return item.charges * item.value;
}

void place_treasure(location where,short level,short loot,short mode)
//short mode;  // 0 - normal, 1 - force
{

	item_record_type new_item;
	short amt,r1,i,j;
	short treas_chart[5][6] = {{0,-1,-1,-1,-1,-1},
								{1,-1,-1,-1,-1,-1},
								{2,1,1,-1,-1,-1},
								{3,2,1,1,-1,-1},
								{4,3,2,2,1,1}};
	short treas_odds[5][6] = {{10,0,0,0,0,0},
								{50,0,0,0,0,0},
								{60,50,40,0,0,0},
								{100,90,80,70,0,0},
								{100,80,80,75,75,75}};
	short id_odds[21] = {0,10,15,20,25,30,35,39,43,47,
							51,55,59,63,67,71,73,75,77,79,81};
	short max_mult[5][10] = {{0,0,0,0,0,0,0,0,0,1},
							{0,0,1,1,1,1,2,3,5,20},
							{0,0,1,1,2,2,4,6,10,25},
							{5,10,10,10,15,20,40,80,100,100},
							{25,25,50,50,50,100,100,100,100,100}};
	short min_chart[5][10] = {{0,0,0,0,0,0,0,0,0,1},
						{0,0,0,0,0,0,0,0,5,20},
						{0,0,0,0,1,1,5,10,15,40},
						{10,10,15,20,20,30,40,50,75,100},
						{50,100,100,100,100,200,200,200,200,200}};
	short max,min;
	
	if (loot == 1)
		amt = get_ran(2,1,7) + 1;
		else amt = loot * (get_ran(1,0,10 + (loot * 6) + (level * 2)) + 5);

	if (adven.getTotalLevel() <= 12)
		amt += 1;	
	if ((adven.getTotalLevel() <= 60) && (amt > 2))
		amt += 2;
		
	if (amt > 3) {	
			new_item = get_stored_item(0);
			new_item.item_level = amt;
			r1 = get_ran(1,1,9);
			if (((loot > 1) && (r1 < 7)) || ((loot == 1) && (r1 < 5)) || (mode == 1)
				|| ((r1 < 6) && (adven.getTotalLevel() < 30)) || (loot > 2) )
				place_item(new_item,where,FALSE);
		}
	for (j = 0; j < 5; j++) {
		r1 = get_ran(1,0,100);
		if ((treas_chart[loot][j] >= 0) && (r1 <= treas_odds[loot][j] + adven.getTotalLuck())) {
			r1 = get_ran(1,0,9);
			min = min_chart[treas_chart[loot][j]][r1];
			r1 = get_ran(1,0,9);
			max = (min + level + (2 * (loot - 1)) + (adven.getTotalLuck() / 3)) * max_mult[treas_chart[loot][j]][r1];
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
							new_item.variety = 0;
						}
					}

			// not many magic items
			if (mode == 0) {
				if (new_item.isMagic() && (level < 2) && (get_ran(1,0,5) < 3))
					new_item.variety = 0;
				if (new_item.isMagic() && (level == 2) && (get_ran(1,0,5) < 2))
					new_item.variety = 0;
				if (new_item.isCursed() && (get_ran(1,0,5) < 3))
					new_item.variety = 0;
				}
				
			// if forced, keep dipping until a treasure comes uo
			if ((mode == 1)	&& (max >= 20)) {
				do
					new_item = return_treasure(treas_chart[loot][j],level,mode);
					while ((new_item.variety == 0) || (item_val(new_item) > max));
				}

			// Not many cursed items
			if (new_item.isCursed() && (get_ran(1,0,2) == 1))
				new_item.variety = 0;
							
			if (new_item.variety != 0) {
				for (i = 0; i < 6; i++)
					if ((adven[i].isAlive()) 
						&& (get_ran(1,0,100) < id_odds[adven[i].skills[13]]))
							new_item.item_properties = new_item.item_properties | 1;
				place_item(new_item,where,FALSE);
				}			
			}
		}

}

short pc_array::getTotalLuck()
{
	short i = 0;
	
	for (i = 0; i < NUM_OF_PCS; i++)
		if (pc[i].isAlive())
			i += pc[i].skills[SKILL_LUCK];
			
	return i;
}

void get_text_response_event_filter (short)
{
	cd_get_text_edit_str(store_dnum,(char *) store_str);
	dialog_not_toast = FALSE;
}

item_record_type return_treasure(short loot,short,short)
{
	item_record_type treas;
	short which_treas_chart[48] = {1,1,1,1,1,2,2,2,2,2,
								3,3,3,3,3,2,2,2,4,4,
								4,4,5,5,5,6,6,6,7,7,
								7,8,8,9,9,10,11,12,12,13,
								13,14, 9,10,11,9,10,11};
	short r1;
	
	treas.variety = 0;
	r1 = get_ran(1,0,41);
	if (loot >= 3) r1 += 3;
	switch (which_treas_chart[r1]) {
		case 1: treas = get_food(); break;
		case 2: treas = get_weapon(loot);	break;
		case 3: treas = get_armor(loot); break;
		case 4: treas = get_shield(loot); break;
		case 5: treas = get_helm(loot); break;
		case 6: treas = get_missile(loot); break;
		case 7: treas = get_potion(loot); break;
		case 8: treas = get_scroll(loot); break;
		case 9: treas = get_wand(loot); break; 
		case 10: treas = get_ring(loot); break;
		case 11: treas = get_necklace(loot); break;
		case 12: treas = get_poison(loot); break;
		case 13: treas = get_gloves(loot); break;
		case 14: treas = get_boots(loot); break;
		} 
	if (treas.variety == 0)	treas.value = 0;	
	return treas;

}


void refresh_store_items()
{
	short i,j;
	short loot_index[10] = {1,1,1,1,2,2,2,3,3,4};
	
	for (i = 0; i < 5; i++)
		for (j = 0; j < 10; j++) {
			party.magic_store_items[i][j] = return_treasure(loot_index[j],7,1);
			if ((party.magic_store_items[i][j].variety == 3) ||
				(party.magic_store_items[i][j].variety == 11))
				party.magic_store_items[i][j] = return_dummy_item();
			party.magic_store_items[i][j].item_properties =
				party.magic_store_items[i][j].item_properties | 1;
			}
}

void get_text_response(short dlg,char *str,short parent_num)
{
	short i;

	SetCursor(sword_curs);
	
	store_str = (char *) str;
	store_dnum = dlg;
	
	cd_create_dialog_parent_num(dlg,parent_num);
	cd_set_edit_focus();
			
	while (dialog_not_toast)
		ModalDialog();	
	for (i = 0; i < 15; i++)
		if ((str[i] > 64) && (str[i] < 91))
			str[i] = str[i] + 32;

	final_process_dialog(dlg);
}

// returns true is party has item of given item class
// mode - 0 - take one of them, 1 - don't take
bool pc_array::checkClass(short item_class, short mode)
{
	short i,j;
	
	if (item_class == 0)
		return false;
	for (i = 0; i < NUM_OF_PCS; i++)
		if (pc[i].isAlive())
			for (j = 23; j >= 0; j--)
				if ((pc[i].items[j].variety > 0) && (pc[i].items[j].special_class == item_class)) {
					if (mode == 0) {
						if (pc[i].items[j].charges > 1)
							pc[i].items[j].charges--;
							else pc[i].takeItem(j);
						}
					return true;
					}
	return false;
}
