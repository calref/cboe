//#include <Quickdraw.h>
//#include <Dialogs.h>
//#include <Palettes.h>
//#include <QDOffscreen.h>

#include <Carbon/Carbon.h> /*jmr*/

#include "ed.graphics.h"
#include "ed.global.h"
#include "ed.editors.h"
#include "graphtool.h"
#include "dlgtool.h"

/* Adventure globals */
extern party_record_type party;
extern pc_record_type adven[6];
extern outdoor_record_type outdoors[2][2];
extern current_town_type c_town;
extern big_tr_type t_d;
extern town_item_list	t_i;
extern unsigned char out[96][96],out_e[96][96];
extern setup_save_type setup_save;
extern stored_items_list_type stored_items[3];

extern short dialog_answer;

extern short store_flags[3];

extern short current_active_pc;

extern WindowPtr mainPtr;
extern Rect d_rects[80];
extern short d_rect_index[80],current_cursor;

extern CursHandle sword_curs;
extern Boolean modeless_exists[12],diff_depth_ok,current_file_has_maps;
extern short modeless_key[12];
extern DialogPtr modeless_dialogs[12];

Boolean equippable[18] = {FALSE,TRUE,TRUE,FALSE,TRUE,TRUE,TRUE,FALSE,FALSE,TRUE,TRUE,TRUE,
							TRUE,TRUE,TRUE,FALSE,FALSE,TRUE};
short num_hands_to_use[18] = {0,1,2,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0};
short num_that_can_equip[18] = {0,2,1,0,1,1,1,0,0,2,1,1,1,2,1,0,0,1}; 
short selected,item_max = 0;
Boolean choice_active[6];



extern short store_trait_mode;
pc_record_type *store_pc;
GWorldPtr button_num_gworld;




void combine_things(short pc_num)
{
	short i,j,test;
	
	for (i = 0; i < 24; i++) {
		if ((adven[pc_num].items[i].variety > 0) &&
			(adven[pc_num].items[i].type_flag > 0) && (adven[pc_num].items[i].item_properties & 254 != 0)) {
			for (j = i + 1; j < 24; j++)
				if ((adven[pc_num].items[j].variety > 0) &&
				(adven[pc_num].items[j].type_flag == adven[pc_num].items[i].type_flag) 
				 && (adven[pc_num].items[j].item_properties & 254 != 0)) {
					add_string_to_buf("(items combined)");
					test = (short) (adven[pc_num].items[i].charges) + (short) (adven[pc_num].items[j].charges);
					if (test > 125) {
						adven[pc_num].items[i].charges = 125;
						ASB("Can have at most 125 of any item.");
						}
				 		else adven[pc_num].items[i].charges += adven[pc_num].items[j].charges;
				 	if (adven[pc_num].equip[j] == TRUE) {
				 		adven[pc_num].equip[i] = TRUE;
				 		adven[pc_num].equip[j] = FALSE;
				 		}
					take_item(pc_num,j);
				 	}
			}		
		if ((adven[pc_num].items[i].variety > 0) && (adven[pc_num].items[i].charges < 0))
			adven[pc_num].items[i].charges = 1;
		}			
}

Boolean give_to_pc(short pc_num,item_record_type  item, short print_result)
{
	short free_space;
	char announce_string[60];
	
	if (item.variety == 0)
		return TRUE;
	if (((free_space = pc_has_space(pc_num)) == 24 ) || (adven[pc_num].main_status != 1))
		return FALSE;
		else {
			adven[pc_num].items[free_space] = item;
			combine_things(pc_num);
			return TRUE;
			}
	return FALSE;
}

Boolean give_to_party(item_record_type item,short print_result)
{
	short free_space, i = 0;
	
	while (i < 6) {
		if (give_to_pc(i,item,print_result) == TRUE)
			return TRUE;
		i++;
	}
	return FALSE;
}

void give_gold(short amount,Boolean print_result)
{
	party.gold = party.gold + amount;
}

Boolean take_gold(short amount,Boolean print_result)
{
	if (party.gold < amount)
		return FALSE;
	party.gold = party.gold - amount;
	return TRUE;
}


short pc_has_space(short pc_num)
{
	short i = 0;
	
	while (i < 24) {
	if (adven[pc_num].items[i].variety == 0)
		return i;
	i++;
	}
	return 24;
}


void take_item(short pc_num,short which_item)
//short pc_num,which_item;  // if which_item > 20, don't update stat win, item is which_item - 20
{
	short i;
	Boolean do_print = TRUE;

	if ((adven[pc_num].weap_poisoned == which_item) && (adven[pc_num].status[0] > 0)) {
//			add_string_to_buf("  Poison lost.           ");
			adven[pc_num].status[0] = 0;
		}
	if ((adven[pc_num].weap_poisoned > which_item) && (adven[pc_num].status[0] > 0)) 
		adven[pc_num].weap_poisoned--;
		
	for (i = which_item; i < 23; i++) {
		adven[pc_num].items[i] = adven[pc_num].items[i + 1];
		adven[pc_num].equip[i] = adven[pc_num].equip[i + 1];
		}
	adven[pc_num].items[23].variety = 0;
	adven[pc_num].equip[23] = FALSE;

}


void fancy_choice_dialog_event_filter (short item_hit)
{
	toast_dialog();
	dialog_answer = item_hit;
}

short fancy_choice_dialog(short which_dlog,short parent)
// ignore parent in Mac version
{
	short item_hit,i,store_dialog_answer;
	Str255 temp_str;
	
	store_dialog_answer = dialog_answer;
	make_cursor_sword();
	
	cd_create_dialog_parent_num(which_dlog,parent);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(which_dlog,0);

	i = dialog_answer;
	dialog_answer = store_dialog_answer;
	
	return i;
}

void select_pc_event_filter (short item_hit)
{
	toast_dialog();
	if (item_hit == 16)
		dialog_answer = 6;
		else dialog_answer = item_hit - 3;
}

short char_select_pc(short active_only,short free_inv_only,char *title)
//active_only;  // 0 - no  1 - yes   2 - disarm trap   
{
	short item_hit,i;

	make_cursor_sword();
	
	cd_create_dialog(1018,mainPtr);
	
	if (active_only == 2)
		csit(1018,15,"Select PC to disarm trap:");
		else csit(	1018,15,title);
	
	for (i = 0; i < 6; i++) {
		if ((adven[i].main_status == 0) ||
			((active_only == TRUE) && (adven[i].main_status > 1)) ||
			((free_inv_only == 1) && (pc_has_space(i) == 24)) || (adven[i].main_status == 5)) {
				cd_activate_item(1018, 3 + i, 0);
				}
		if (adven[i].main_status != 0) {
				csit(1018,9 + i,adven[i].name);		
			}		
			else cd_activate_item(1018, 9 + i, 0);
	}
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(1018,0);

	return dialog_answer;
}

short select_pc(short active_only,short free_inv_only)
//active_only;  // 0 - no  1 - yes   2 - disarm trap   
{
	if (active_only == 2)
		return char_select_pc(active_only,free_inv_only,"Trap! Who will disarm?");
		else return char_select_pc(active_only,free_inv_only,"Select a character:");
}


// only used at beginning of program
short choice_dialog(short pic,short num)
{
	DialogPtr select_dialog = NULL;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	short item_hit,i;
	Str255 pc_name;
	GrafPtr old_port;
	PicHandle	pict_to_draw;


	select_dialog = GetNewDialog (num, 0, IN_FRONT);
	if (select_dialog == NULL) { 
		SysBeep(50);
		ExitToShell();
		}

	SetPort(GetDialogPort(select_dialog));	



	ShowWindow(GetDialogWindow(select_dialog));

	//if (pic > 0)
	//	put_dialog_graphic(pic,0,the_rect);
	ModalDialog(NIL, &item_hit);
		
	DisposeDialog(select_dialog);
	
//	SetPort(GetWindowPort(mainPtr));

//	BeginUpdate(mainPtr);
//	if (in_startup_mode == FALSE)
//		refresh_screen(0);
//		else draw_startup(0);
//	EndUpdate(mainPtr);

	
	return item_hit;

}








short party_total_level() 
{
	short i,j = 0;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			j += adven[i].level;
	return j;
}



short luck_total()
{
	short i = 0;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			i += adven[i].skills[18];
	return i;
}

void display_traits_graphics()
{
	short i,store;
	Str255 str;

	for (i = 0; i < 3; i++) {
		cd_set_led(1013,4 + i,(store_pc->race == i) ? 1 : 0);
		}
	for (i = 0; i < 10; i++) {
		cd_set_led(1013,7 + i,(store_pc->traits[i] > 0) ? 1 : 0);
		}
	for (i = 0; i < 5; i++) {
		cd_set_led(1013,36 + i,(store_pc->traits[10 + i] > 0) ? 1 : 0);
		}

	store = get_tnl(store_pc);
	cdsin(1013,18,store);
}

void pick_race_abil_event_filter(short item_hit)
{
	Str255 abil_str;
	pc_record_type *pc;

	pc = store_pc;
			switch (item_hit) {
				case 3:
					toast_dialog();
					break;
				case 4: case 5: case 6:
					if (store_trait_mode == 0)
						pc->race = item_hit - 4;
					display_traits_graphics();
					get_str(abil_str,5,12 + item_hit);
					csit(1013,19,(char *) abil_str);
					break;
				case 36: case 37: case 38: case 39: case 40:
					if (store_trait_mode != 1)
						pc->traits[item_hit - 36 + 10] = (pc->traits[item_hit - 36 + 10] == TRUE) ? FALSE : TRUE;
					display_traits_graphics();
					get_str(abil_str,5,item_hit - 36 + 11);				
					csit(1013,19,(char *) abil_str);
					break;
				default:
					if (item_hit >= 100)
						return;
					if (store_trait_mode != 1)
						pc->traits[item_hit - 7] = (pc->traits[item_hit - 7] == TRUE) ? FALSE : TRUE;
					display_traits_graphics();
					get_str(abil_str,5,item_hit - 6);				
					csit(1013,19,(char *) abil_str);
					break;
				}	

}

void pick_race_abil(pc_record_type *pc,short mode,short parent_num)
//mode; // 0 - edit  1 - just display  2 - can't change race
{
	char *start_str1 = "Click on advantage button for description.";
	char *start_str2 = "Click on advantage button to add/lose.";
	short item_hit;
	
	mode = 0;
	store_trait_mode = mode;
	store_pc = pc;
	make_cursor_sword();

	cd_create_dialog_parent_num(1013,parent_num);

	display_traits_graphics();
	if (mode == 1)
		csit(1013,19,start_str1);
	else csit(1013,19,start_str2);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(1013,0);
	untoast_dialog();
}


short get_tnl(pc_record_type *pc)
{
	short tnl = 100,i,store_per = 100;
	short rp[3] = {0,12,20};
	short ap[15] = {10,20,8,10,4, 6,10,7,12,15, -10,-8,-8,-20,-8};
	
	tnl = (tnl * (100 + rp[pc->race])) / 100;
	for (i = 0; i < 15; i++)
		if (pc->traits[i] == TRUE) 
			store_per = store_per + ap[i];

	tnl = (tnl * store_per) / 100;	
	
	return tnl;
}



//display_strings(20,7,0,0,"Editing party",57,707,916);

