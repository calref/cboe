#include <stdio.h>

#include "pc.graphics.h"
#include "pc.global.h"
#include "pc.editors.h"
#include "pc.fileio.h" 
#include "pc.action.h"
#include "graphtool.h"
#include "soundtool.h" 
#include "dlgtool.h"
#include "mathutil.h"
#include "dlgconsts.h"

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
extern stored_town_maps_type town_maps;
extern stored_outdoor_maps_type o_maps;

extern Boolean ed_reg;
extern long ed_flag,ed_key;

extern WindowPtr mainPtr;
extern Boolean file_in_mem;
extern short current_cursor,dialog_answer;
extern long register_flag;

extern GWorldPtr pc_gworld;
extern CursHandle sword_curs;
extern Boolean diff_depth_ok,registered,save_blocked;



short which_pc_displayed,store_pc_trait_mode,store_which_to_edit;
extern short current_active_pc;
Str255 empty_string = "\p                                                ";
extern Rect pc_area_buttons[6][4] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
extern Rect item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 - 
extern Rect pc_info_rect;
extern Rect name_rect;

extern Rect pc_race_rect;
extern Rect edit_rect[5][2]; 


short store_trait_mode,store_train_pc;
pc_record_type *store_xp_pc;

// Variables for spending xp
	Boolean talk_done = FALSE;
	long val_for_text;
	Boolean keep_change = FALSE;
	short store_skills[20],store_h,store_sp,i,which_skill,store_skp = 10000,store_g = 10000;


short skill_cost[20] = {3,3,3,2,2,2, 1,2,2,6,
						5, 1,2,4,2,1, 4,2,5,0};
short skill_max[20] = {20,20,20,20,20,20,20,20,20,7,
						7,20,20,10,20,20,20,20,20};
short skill_g_cost[20] = {50,50,50,40,40,40,30,50,40,250,
						250,25,100,200,30,20,100,80,0,0};
short skill_bonus[21] = {-3,-3,-2,-1,0,0,1,1,1,2,
							2,2,3,3,3,3,4,4,4,5,5};

//extern Rect pc_area_buttons[6][6] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
//extern Rect item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 - 
Boolean handle_action(EventRecord event,short mode)
//short mode; // ignore,
{
	short i,j,button_hit = 100,first_existing_pc = 0;
	
	char debug_line[60];	
	RGBColor s;
	Point the_point;
	short choice = 4,for_pc = 6;
	
	Boolean to_return = FALSE;

	the_point = event.where;
	GlobalToLocal(&the_point);	

	if (file_in_mem == FALSE) 
		return FALSE;
		
	for (i = 0; i < 6; i++)
		if ((PtInRect(the_point,&pc_area_buttons[i][0]) == TRUE) &&
			(adven[i].main_status > 0)) {
			do_button_action(0,i);
			current_active_pc = i;
			display_party(6,1);
			draw_items(1);
			}
	for (i = 0; i < 5; i++)
		if ((PtInRect(the_point,&edit_rect[i][0]) == TRUE) &&
			(adven[current_active_pc].main_status > 0)) {
			do_button_action(0,i + 10);
			if ((ed_reg == FALSE) && (save_blocked == FALSE))
				if ((choice = FCD(904,0)) == 1)
					return to_return;
					else save_blocked = TRUE;
			switch(i) {
				case 0:
					display_pc(current_active_pc,0,0);
					break;
				case 1:
			 		display_pc(current_active_pc,1,0);
					break;
				case 2: 
					pick_race_abil(&adven[current_active_pc],0,0);
					break;
				case 3: 
					spend_xp(current_active_pc,1,0);
					break;
				case 4: 
					edit_xp(&adven[current_active_pc]);
					
					break;
			}
		}
	for (i = 0; i < 24; i++)
		if ((PtInRect(the_point,&item_string_rects[i][1]) == TRUE) && // drop item
			(adven[current_active_pc].items[i].variety > 0)) { // variety = 0 no item in slot/ non 0 item exists
				flash_rect(item_string_rects[i][1]);
				take_item(current_active_pc,i);
				draw_items(1);
				}
	for (i = 0; i < 24; i++)
		if ((PtInRect(the_point,&item_string_rects[i][2]) == TRUE) && // identify item
			(adven[current_active_pc].items[i].variety > 0)) {
				flash_rect(item_string_rects[i][2]);
				adven[current_active_pc].items[i].item_properties = adven[current_active_pc].items[i].item_properties | 1;
				draw_items(1);
				}
	
	return to_return;
}

void flash_rect(Rect to_flash)
{
	RGBColor	test_color, store_color;
	int i;
	unsigned long dummy;
	
	InvertRect (&to_flash);
	play_sound(37);
	Delay(5,&dummy);
	InvertRect (&to_flash);
}

void edit_gold_or_food_event_filter (short item_hit)
{
	Str255 get_text;
	long dummy;
	
	cd_retrieve_text_edit_str((store_which_to_edit == 0) ? 1012 : 947,2,(char *) get_text);
	dialog_answer = 0;
#ifndef EXILE_BIG_GUNS
	sscanf((char *) get_text,"%d",&dialog_answer);
#endif		
#ifdef EXILE_BIG_GUNS
	sscanf((char *) get_text,"%d",&dummy);
	dialog_answer = dummy;
#endif		
	toast_dialog();
}

void edit_gold_or_food(short which_to_edit)
//0 - gold 1 - food
{

	short item_hit;
	Str255 sign_text;
	location view_loc;

	store_which_to_edit = which_to_edit;

	make_cursor_sword();
	cd_create_dialog((which_to_edit == 0) ? 1012 : 947, mainPtr);
	sprintf((char *) sign_text,"%d",(short) ((which_to_edit == 0) ? party.gold : party.food));
	cd_set_text_edit_str((which_to_edit == 0) ? 1012 : 947,2,(char *) sign_text);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog((which_to_edit == 0) ? 1012 : 947,0);
	dialog_answer = minmax(0,25000,dialog_answer);
	if (which_to_edit == 0)
		party.gold = dialog_answer;
	else
		party.food = dialog_answer;
}

void edit_day_event_filter (short item_hit)
{
	Str255 get_text;
	long dummy;
	
	cd_retrieve_text_edit_str(917,2,(char *) get_text);
	dialog_answer = 0;
#ifndef EXILE_BIG_GUNS
	sscanf((char *) get_text,"%d",&dialog_answer);
#endif		
#ifdef EXILE_BIG_GUNS
	sscanf((char *) get_text,"%d",&dummy);
	dialog_answer = dummy;
#endif		
	toast_dialog();
}

void edit_day()
{

	short item_hit;
	Str255 sign_text;
	location view_loc;


	make_cursor_sword();
	
	cd_create_dialog(917,mainPtr);
		
	sprintf((char *) sign_text,"%d",(short) ( ((party.age) / 3700) + 1));
	cd_set_text_edit_str(917,2,(char *) sign_text);
	
	item_hit = cd_run_dialog();
	
	cd_kill_dialog(917,0);
	
	dialog_answer = minmax(0,500,dialog_answer);
	
	party.age = (long) (3700) * (long) (dialog_answer);
}


void put_pc_graphics()
{
	short item_hit,what_talk_field,i;

	for (i = 3; i < 65; i++) {
		if (((store_trait_mode == 0) && (adven[which_pc_displayed].mage_spells[i - 3] == TRUE)) ||
		 ((store_trait_mode == 1) && (adven[which_pc_displayed].priest_spells[i - 3] == TRUE)))
			cd_set_led(991,i,1);
			else cd_set_led(991,i,0);
		}

	cd_set_item_text(991,69,adven[which_pc_displayed].name);
}
void display_pc_event_filter (short item_hit)
{
	short pc_num;

	pc_num = which_pc_displayed;
			switch (item_hit) {
				case 1: case 65:
					toast_dialog();
					break;

				case 66:
					do {
						pc_num = (pc_num == 0) ? 5 : pc_num - 1;
						} while (adven[pc_num].main_status == 0);
					which_pc_displayed = pc_num;
					put_pc_graphics();
					break;
				case 67:
					do {
						pc_num = (pc_num == 5) ? 0 : pc_num + 1;
						} while (adven[pc_num].main_status == 0);
					which_pc_displayed = pc_num;
					put_pc_graphics();	
					break;
					
				default:
					if (store_trait_mode == 0)
						adven[which_pc_displayed].mage_spells[item_hit - 3] = 
							1 - adven[which_pc_displayed].mage_spells[item_hit - 3];
						else
						adven[which_pc_displayed].priest_spells[item_hit - 3] = 
							1 - adven[which_pc_displayed].priest_spells[item_hit - 3];
					put_pc_graphics();							
					break;
				}
}

void display_pc(short pc_num,short mode,short parent)
{
	short i,item_hit;
	Str255 label_str;
	
	if (adven[pc_num].main_status == 0) {
		for (pc_num = 0; pc_num < 6; pc_num++)
			if (adven[pc_num].main_status == 1)
				break;
		}
	which_pc_displayed = pc_num;
	store_trait_mode = mode;

	make_cursor_sword();

	cd_create_dialog_parent_num(991,0);

	for (i = 3; i < 65; i++) {
		get_str(label_str,(mode == 0) ? 7 : 8,(i - 3) * 2 + 1);
		cd_add_label(991,i,(char *)label_str,46);
		}
	put_pc_graphics();

	cd_set_pict(991,2,14 + mode,PICT_DLG_TYPE);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(991,0);
}


void display_alchemy_event_filter (short item_hit)
{
	short i;

			switch (item_hit) {
				case 1: case 3:
					toast_dialog();
					break;
				default:
					party.alchemy[item_hit - 4] = 1 - party.alchemy[item_hit - 4];
					break;

				}
	for (i = 0; i < 20; i++) {
		if (party.alchemy[i] > 0)
			cd_set_led(996,i + 4,1);
			else cd_set_led(996,i + 4,0);
		}
}

void display_alchemy()
{
	short i,item_hit;
	char *alch_names[] = {"Weak Curing Potion (1)","Weak Healing Potion (1)","Weak Poison (1)",
	"Weak Speed Potion (3)","Medium Poison (3)",
		"Medium Heal Potion (4)","Strong Curing (5)","Medium Speed Potion (5)",
		"Graymold Salve (7)","Weak Power Potion (9)",
		"Potion of Clarity (9)","Strong Poison (10)","Strong Heal Potion (12)","Killer Poison (12)",
		"Resurrection Balm (9)","Medium Power Potion (14)","Knowledge Brew (19)",
		"Strong Strength (10)","Bliss (18)","Strong Power (20)"	
		};

	make_cursor_sword();

	cd_create_dialog_parent_num(996,0);


	for (i = 0; i < 20; i++) {
		cd_add_label(996,i + 4,alch_names[i],1083);
		if (party.alchemy[i] > 0)
			cd_set_led(996,i + 4,1);
			else cd_set_led(996,i + 4,0);
	}
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(996,0);
	untoast_dialog();

}

void do_xp_keep(short pc_num,short mode)
{
					for (i = 0; i < 20; i++)
						adven[pc_num].skills[i] = store_skills[i];
					adven[pc_num].cur_health += store_h - adven[pc_num].max_health;
					adven[pc_num].max_health = store_h;
					adven[pc_num].cur_sp += store_sp - adven[pc_num].max_sp;
					adven[pc_num].max_sp = store_sp;

}

void draw_xp_skills()
{
	short i;
	for (i = 0; i < 19; i++) {
		if ((store_skp >= skill_cost[i]) && (store_g >= skill_g_cost[i]))
			cd_text_frame(1010,54 + i,11);
			else cd_text_frame(1010,54 + i,1);
		cd_set_item_num(1010,54 + i,store_skills[i]);
		}

		if ((store_skp >= 1) && (store_g >= 10))
			cd_text_frame(1010,52,11);
			else cd_text_frame(1010,52,1);
	cd_set_item_num(1010,52,store_h);
		if ((store_skp >= 1) && (store_g >= 15))
			cd_text_frame(1010,53,11);
			else cd_text_frame(1010,53,1);
	cd_set_item_num(1010,53,store_sp);
}


void do_xp_draw()

{

	short item_hit,what_talk_field;
	char get_text[256];
	short pc_num;

	pc_num = store_train_pc;

			sprintf((char *) get_text, "%s",(char *) adven[pc_num].name);


	cd_set_item_text (1010, 51,get_text);

	for (i = 0; i < 20; i++)
		store_skills[i] = adven[pc_num].skills[i];
	store_h = adven[pc_num].max_health;
	store_sp = adven[pc_num].max_sp;
	store_g = 12000;
	store_skp = 10000;

	draw_xp_skills();

	update_gold_skills();
}

void spend_xp_event_filter (short item_hit)
{
	short what_talk_field,i,j,pc_num,mode = 1;
	char get_text[256];
	Boolean talk_done = FALSE;

	pc_num = store_train_pc;

		switch (item_hit) {
			case 73:
				dialog_answer = 0;
				talk_done = TRUE;
				break;
	


			case 3: case 4:
					if ((store_h >= 250) && (item_hit == 4)) 
							SysBeep(2);
						else {
							if (item_hit == 3) {
								store_g += 10;
								store_h -= 2;
								store_skp += 1;
								}
								else {
									if ((store_g < 10) || (store_skp < 1)) {

										SysBeep(2);
										}
										else {
											store_g -= 10;
											store_h += 2;
											store_skp -= 1;
											}
								}

							update_gold_skills();
							cd_set_item_num(1010,52,store_h);
                     	draw_xp_skills();

						}
				break;

			case 5: case 6:
					if ((store_sp >= 150) && (item_hit == 6))
							SysBeep(2);
						else {
							if (item_hit == 5) {
								store_g += 15;
								store_sp -= 1;
								store_skp += 1;
								}
								else {
									if ((store_g < 15) || (store_skp < 1)) {

										SysBeep(2);
										}
										else {
											store_sp += 1;
											store_g -= 15;
											store_skp -= 1;
											}
								}

							update_gold_skills();
							cd_set_item_num(1010,53,store_sp);
							draw_xp_skills();
						}
				break;

			case 48:
					do_xp_keep(pc_num,0);
					dialog_answer = 1;
					talk_done = TRUE;
				break;

			case 49:

						do_xp_keep(pc_num,0);
						do {
							pc_num = (pc_num == 0) ? 5 : pc_num - 1;
						} while (adven[pc_num].main_status != 1);
						store_train_pc = pc_num;
						do_xp_draw();
				break;

			case 50:

						do_xp_keep(pc_num,0);
						do {
							pc_num = (pc_num == 5) ? 0 : pc_num + 1;
						} while (adven[pc_num].main_status != 1);
						store_train_pc = pc_num;
						do_xp_draw();
				break;

			case 100:
				break;

			default:
				if (item_hit >= 100) {
					}
				else {
				which_skill = (item_hit - 7) / 2;
				
				if (((store_skills[which_skill] >= skill_max[which_skill]) && ((item_hit - 7) % 2 == 1)) ||
					((store_skills[which_skill] == 0) && ((item_hit - 7) % 2 == 0) && (which_skill > 2)) ||
					((store_skills[which_skill] == 1) && ((item_hit - 7) % 2 == 0) && (which_skill <= 2)))
						SysBeep(2);
					else {
						if ((item_hit - 7) % 2 == 0) {
							store_g += skill_g_cost[which_skill];
							store_skills[which_skill] -= 1;
							store_skp += skill_cost[which_skill];
							}
							else {
								if ((store_g < skill_g_cost[which_skill]) || (store_skp < skill_cost[which_skill])) {

									SysBeep(2);
									}
									else {
										store_skills[which_skill] += 1;
										store_g -= skill_g_cost[which_skill];
										store_skp -= skill_cost[which_skill];
										}
							}

							update_gold_skills();
							cd_set_item_num(1010,54 + which_skill,store_skills[which_skill]);
							draw_xp_skills();
						}
				}	
				break;
			}
			
	store_train_pc = pc_num;
	if (talk_done == TRUE) {
		toast_dialog();
		}
}
void update_gold_skills()
{
	csit(1010,47,"Lots!");
	csit(1010,46,"Lots!");
}
bool spend_xp(short pc_num, short mode, short parent)
//short mode; // 0 - create  1 - train
// returns 1 if cancelled
{
	Str255 get_text,text2;
	short item_hit;

	store_train_pc = pc_num;

	make_cursor_sword();

	cd_create_dialog_parent_num(1010,parent);
	sprintf((char *) get_text,"Health (%d/%d)",1,10);
	cd_add_label(1010,52,(char *) get_text,1075);
	sprintf((char *) get_text,"Spell Pts. (%d/%d)",1,15);
	//cd_add_label(1010,5,get_text,1040);
	cd_add_label(1010,53,(char *) get_text,1075);
	for (i = 54; i < 73; i++) {
		get_str(text2,9,1 + 2 * (i - 54));
		sprintf((char *) get_text,"%s (%d/%d)",text2,skill_cost[i - 54],skill_g_cost[i - 54]);
		cd_add_label(1010,i,(char *) get_text,(i < 63) ? 1075 : 1069);
		}
	do_xp_draw();
	
	dialog_answer = 0;
	
	item_hit = cd_run_dialog();	

	cd_kill_dialog(1010,0);

	return dialog_answer;
}

void give_reg_info_event_filter (short item_hit)
{
	short i;
	Str255 place_str;
	
			switch (item_hit) {
				case 1: 
					toast_dialog();
					break;
				}
}

void give_reg_info()
{
	long val_for_text;

	short i,item_hit;
	Str255 place_str;
	

	make_cursor_sword();

	cd_create_dialog_parent_num(1073,0);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(1073,0);

}

void edit_xp_event_filter (short item_hit)
{
	Str255 get_text;
	long dummy;
	
	cd_retrieve_text_edit_str(1024,2,(char *) get_text);
	dialog_answer = 0;
#ifndef EXILE_BIG_GUNS
	sscanf((char *) get_text,"%d",&dialog_answer);
#endif		
#ifdef EXILE_BIG_GUNS
	sscanf((char *) get_text,"%d",&dummy);
	dialog_answer = dummy;
#endif		
	toast_dialog();
}

void edit_xp(pc_record_type *pc)
{

	short item_hit;
	Str255 sign_text;
	location view_loc;

	store_xp_pc = pc;

	make_cursor_sword();
	
	cd_create_dialog(1024,mainPtr);
		
	sprintf((char *) sign_text,"%d",(short)pc->experience);
	cd_set_text_edit_str(1024,2,(char *) sign_text);
	item_hit = get_tnl(store_xp_pc);
	cdsin(1024,8,item_hit);
	
	item_hit = cd_run_dialog();
	
	cd_kill_dialog(1024,0);
	
	if (dialog_answer < 0)
		dialog_answer = dialog_answer * -1;
	dialog_answer = minmax(0,10000,dialog_answer);
	
	pc->experience = dialog_answer;
}


