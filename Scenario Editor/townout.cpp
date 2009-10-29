#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "global.h"
#include "graphics.h"
#include "dlogtool.h"
#include "townout.h"
#include "keydlgs.h"
#include "tfileio.h"

extern char town_strs[180][256];
extern char talk_strs[170][256];
extern scen_item_data_type scen_item_list;

extern short cen_x, cen_y, overall_mode;
extern Boolean mouse_button_held,dialog_not_toast,editing_town;
extern short cur_viewing_mode;
extern town_record_type town;
extern big_tr_type t_d;
extern short town_type;  // 0 - big 1 - ave 2 - small
extern short max_dim[3],mode_count,to_create,cur_town;
extern unsigned char template_terrain[64][64];
extern short_item_record_type item_list[400];
extern scenario_data_type scenario;
extern special_node_type null_spec_node;
extern talking_node_type null_talk_node;
extern piles_of_stuff_dumping_type *data_store;
extern outdoor_record_type current_terrain;
extern unsigned char borders[4][50];
extern pascal Boolean cd_event_filter();
extern location cur_out;
extern talking_record_type talking;

creature_start_type store_placed_monst,store_placed_monst2;
short store_which_placed_monst;
preset_item_type store_placed_item;
short store_which_placed_item,store_which_sign;
extern short dialog_answer;
short store_whigh_dlog;
short str_do_delete[16];
short a,b,c;
short store_which_out_wand,store_out_wand_mode;
out_wandering_type store_out_wand;

char *day_str_1[] = {"Unused","Day creature appears","Day creature disappears",
		"Unused","Unused","Unused","Unused","Unused"};
char *day_str_2[] = {"Unused","Event code (0 - no event)","Event code (0 - no event)",
					"Unused","Unused","Unused",
					"Event code (0 - no event)","Event code (0 - no event)"};

short store_which_talk_node,last_talk_node[60];
talking_node_type store_talk_node;
location store_cur_loc;

void init_town(short size)
{
	short i,j,s;
	location d_loc = {100,0};
	RECT d_rect = {-1,-1,-1,-1};
	wandering_type d_wan = {0,0,0,0};
	creature_start_type dummy_creature = {0,0,{0,0},0,0,0,0,0,0,0,0,0,0,0,0};
	preset_item_type null_item = {{0,0},-1,0,0,0,0,0};
	char temp_str[256];
	
	town.town_chop_time = -1;
	town.town_chop_key = -1;
	for (i = 0; i < 4; i++) {
		town.wandering[i] = d_wan;
		town.wandering_locs[i] = d_loc;
		}
	for (i = 0; i < 50; i++) {
		town.special_locs[i] = d_loc;
		town.special_locs[i].x = 100;
		//town.spec_id[i] = -1;
		town.spec_id[i] = 0xFF;
		}
	town.lighting = 0;
	for (i = 0; i < 4; i++) {
		town.start_locs[i] = d_loc;
		town.exit_specs[i] = -1;
		town.exit_locs[i].x = -1;
		town.exit_locs[i].y = -1;
		}
	s = max_dim[size];
	town.start_locs[0].x = s / 2;
	town.start_locs[0].y = 4;
	town.start_locs[2].x = s / 2;
	town.start_locs[2].y = s - 5;
	town.start_locs[1].x = s - 5;
	town.start_locs[1].y = s / 2;
	town.start_locs[3].x = 4;
	town.start_locs[3].y = s / 2;
	town.in_town_rect.top = 3;
	town.in_town_rect.bottom = s - 4;
	town.in_town_rect.left = 3;
	town.in_town_rect.right = s - 4;
	for (i = 0; i < 64; i++) 
		town.preset_items[i] = null_item;
	town.max_num_monst = 30000;
	for (i = 0; i < 50; i++) 
		town.preset_fields[i].field_type = 0;
	town.spec_on_entry = -1;
	town.spec_on_entry_if_dead = -1;
	for (i = 0; i < 15; i++) {
		town.sign_locs[i] = d_loc;
		town.sign_locs[i].x = 100;
		}	
	for (i = 0; i < 8; i++) {
		town.timer_spec_times[i] = 0;
		town.timer_specs[i] = -1;
		}	
	for (i = 0; i < 100; i++) {
		town.specials[i] = null_spec_node;
		}
	town.defy_scrying = 0;
	town.hostile_spec_to_call = -1;
	town.res2 = 0;
//    town.strong_barriers = 0;
	town.defy_mapping = 0;
	town.difficulty = 0;
	for (i = 0; i < 180; i++) {
		get_str(temp_str,36,i + 1);
		sprintf((char *)town_strs[i], "%s", temp_str);
		town.strlens[i] = strlen(town_strs[i]);
		}
		
		
		
		
	for (i = 0; i < 16; i++) {
		t_d.room_rect[i] = d_rect;
		//sprintf((char *) t_d.room_string[i], "");	
		}	
	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++) {
			t_d.terrain[i][j] = scenario.default_ground * 2;
			t_d.lighting[i / 8][j] = 0;
			}

	for (i = 0; i < 60; i++) {
		t_d.creatures[i] = dummy_creature;
		}

	for(i = 0; i < 200; i++) 
		talking.strlens[i] = 0;
	for(i = 0; i < 170; i++) 
		if (i < 10)
			sprintf(talk_strs[i],"Unused");
			else sprintf(talk_strs[i],"");
	for(i = 0; i < 60; i++) {
		talking.talk_nodes[i].personality = -1;
		talking.talk_nodes[i].type = 0;
		talking.talk_nodes[i].extras[0] = 0;
		talking.talk_nodes[i].extras[1] = 0;
		talking.talk_nodes[i].extras[2] = 0;
		talking.talk_nodes[i].extras[3] = -1;
		for (j = 0; j < 4; j++) {
			talking.talk_nodes[i].link1[j] = 'x';
			talking.talk_nodes[i].link2[j] = 'x';		
			}
		}
		
}

void init_out()
{
	short i,j;
	location d_loc = {100,0};
	out_wandering_type d_monst = {{0,0,0,0,0,0,0},{0,0,0},-1,-1,-1,0,-1,-1};
	RECT d_rect = {-1,-1,-1,-1};
	location locs[4] = {{8,8},{32,8},{8,32},{32,32}};
	char temp_str[256];
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			current_terrain.terrain[i][j] = 0;
			}
	for (i = 0; i < 4; i++)
		for (j = 0; j < 50; j++)
			borders[i][j] = 90;
			
	for (i = 0; i < 18; i++) {
		current_terrain.special_locs[i] = d_loc;
		current_terrain.special_id[i] = 0;
		}
	for (i = 0; i < 8; i++) {
		current_terrain.exit_locs[i] = d_loc;
		current_terrain.exit_dests[i] = 0;
		current_terrain.sign_locs[i] = d_loc;
		current_terrain.sign_locs[i].x = 100;
		current_terrain.info_rect[i] = d_rect;
		}
	for (i = 0; i < 4; i++) {
		current_terrain.wandering[i] = d_monst;
		current_terrain.wandering_locs[i] = locs[i];
		current_terrain.special_enc[i] = d_monst;
		}
		
	for (i = 0; i < 120; i++) {
		get_str(temp_str,37,i + 1);
		sprintf((char *)data_store->out_strs[i], "%s", temp_str);
		current_terrain.strlens[i] = strlen((char *) data_store->out_strs[i]);
		}
	for (i = 0; i < 60; i++) {
		current_terrain.specials[i] = null_spec_node;
		}
	
}

void put_placed_monst_in_dlog()
{
	char str[256];
	short i;
	
	cdsin(837,24,store_which_placed_monst);
	csit(837,7,scen_item_list.monst_names[store_placed_monst.number]);
 	cd_set_led_range(837,14,17,store_placed_monst.start_attitude);
 	cd_set_led_range(837,18,19,store_placed_monst.mobile);
 	CDSN(837,2,store_placed_monst.personality);
 	CDSN(837,3,store_placed_monst.facial_pic);
 	if ((store_placed_monst.facial_pic < 1) || (store_placed_monst.facial_pic >= 1000))
	 	csp(837,11,950);
	 	else csp(837,11,store_placed_monst.facial_pic + 1000);
}
Boolean get_placed_monst_in_dlog()
{
	char str[256];
	short i;
	
	store_placed_monst.start_attitude = cd_get_led_range(837,14,17);
	store_placed_monst.mobile = cd_get_led_range(837,18,19);
	store_placed_monst.personality = CDGN(837,2);
	store_placed_monst.facial_pic = CDGN(837,3);
	// later
	t_d.creatures[store_which_placed_monst] = store_placed_monst;
	return TRUE;
}

void edit_placed_monst_event_filter (short item_hit)
{
	char str[256];
	short i;
	creature_start_type store_m;
	
	switch (item_hit) {
		case 20:
			if (get_placed_monst_in_dlog() == FALSE)
				break;
			dialog_not_toast = FALSE;
			break;
		case 21:
			dialog_not_toast = FALSE;
			break;
		case 6: // choose m type
			if (get_placed_monst_in_dlog() == FALSE)
				break;
			i = choose_text_res(-1,1,255,store_placed_monst.number,837,"Choose Which Monster:");
			if (i >= 0) {
				store_placed_monst.number = i;
				put_placed_monst_in_dlog();
				}			
			break;
		case 12: // facial pic
			if (get_placed_monst_in_dlog() == FALSE)
				break;
			i = choose_graphic(1001,1084,store_placed_monst.facial_pic + 1000,837);
			if (i >= 0) {
				store_placed_monst.facial_pic = i - 1000;
				}
				else break;
			put_placed_monst_in_dlog();
			break;
		case 25: //advanced
			store_m = edit_placed_monst_adv(store_placed_monst);
			if (store_m.number != 0)
				store_placed_monst = store_m;
			break;
		default:
			cd_hit_led_range(837,14,17,item_hit);
			cd_hit_led_range(837,18,19,item_hit);
			break;
		}
}

void edit_placed_monst(short which_m)
{
	
	short item_hit,i,store_dialog_answer;
	char temp_str[256];
	char *attitude[4] = {"Friendly, Docile","Hostile, Type A","Friendly, Will Fight","Hostile, Type B"};
	
	store_placed_monst = t_d.creatures[which_m];
	store_which_placed_monst = which_m;
	
	cd_create_dialog_parent_num(837,0);
	
	put_placed_monst_in_dlog();
	
	cd_add_label(837,18,"No",13);
	cd_add_label(837,19,"Yes",13);

	for (i = 0; i < 4; i++) 
		cd_add_label(837,14 + i,attitude[i],57);

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(837,0);

}

void put_placed_monst_adv_in_dlog()
{
	char str[256];
	short i;
	
	cdsin(838,20,store_which_placed_monst);
	csit(838,9,scen_item_list.monst_names[store_placed_monst2.number]);
	for (i = 22; i < 30; i++)
		cd_set_led(838,i,0);
	if (store_placed_monst2.time_flag < 3)
		cd_set_led(838,22 + store_placed_monst2.time_flag,1);
		else cd_set_led(838,21 + store_placed_monst2.time_flag,1);
	csit(838,11,day_str_1[(store_placed_monst2.time_flag < 3) ? store_placed_monst2.time_flag : store_placed_monst2.time_flag - 1]);
	csit(838,12,day_str_2[(store_placed_monst2.time_flag < 3) ? store_placed_monst2.time_flag : store_placed_monst2.time_flag - 1]);
	CDSN(838,2,store_placed_monst2.monster_time);
	CDSN(838,3,store_placed_monst2.time_code);
 	cd_set_led_range(838,30,40,(short) store_placed_monst2.spec_enc_code);
	CDSN(838,4,store_placed_monst2.special_on_kill);
	CDSN(838,5,store_placed_monst2.spec1);
	CDSN(838,6,store_placed_monst2.spec2);
}
Boolean get_placed_monst_adv_in_dlog()
{
	char str[256];
	short i;
	
	store_placed_monst2.time_flag = cd_get_led_range(838,22,29);
	if (store_placed_monst2.time_flag > 2)
		store_placed_monst2.time_flag++;
	store_placed_monst2.monster_time = CDGN(838,2);
  	if (cre(store_placed_monst2.monster_time,0,1000,"Given day must be from 0 to 1000.","",838) > 0) return FALSE;
	store_placed_monst2.time_code = CDGN(838,3);
  	if (cre(store_placed_monst2.time_code,0,10,"Event code must be 0 (for no event) or from 1 to 10.","",838) > 0) return FALSE;
	store_placed_monst2.special_on_kill = CDGN(838,4);
  	if (cre(store_placed_monst2.special_on_kill,-1,99,"Town special node number must be from 0 to 99 (or -1 for no special).","",838) > 0) return FALSE;
	store_placed_monst2.spec1 = CDGN(838,5);
  	if (cre(store_placed_monst2.spec1,-1,299,"First part of special flag must be -1 (if this is to be ignored) or from 0 to 299.","",838) > 0) return FALSE;
	store_placed_monst2.spec2 = CDGN(838,6);
  	if (cre(store_placed_monst2.spec2,-1,9,"Second part of special flag must be -1 (if this is to be ignored) or from 0 to 9.","",838) > 0) return FALSE;
		
	store_placed_monst2.spec_enc_code = cd_get_led_range(838,30,40);
	return TRUE;
}

void edit_placed_monst_adv_event_filter (short item_hit)
{
	char str[256];
	short i;
	
	switch (item_hit) {
		case 16:
			if (get_placed_monst_adv_in_dlog() == FALSE)
				break;
			dialog_not_toast = FALSE;
			break;
		case 17:
			store_placed_monst2.number = 0;
			dialog_not_toast = FALSE;
			break;

		default:
			cd_hit_led_range(838,22,29,item_hit);
			cd_hit_led_range(838,30,40,item_hit);
			if ((item_hit >= 22) && (item_hit <= 29)) {
				csit(838,11,day_str_1[item_hit - 22]);
				csit(838,12,day_str_2[item_hit - 22]);
				}
			break;
		}
}

creature_start_type edit_placed_monst_adv(creature_start_type monst_record)
{
	
	short item_hit,i,store_dialog_answer;
	char temp_str[256];
	char *time_labels[] = {"Always here","Appear on given day","Disappear on day",
						"Sometimes here A","Sometimes here B","Sometimes here C",
						"Appear when event","Disappear when event"};

	store_placed_monst2 = monst_record;
	
	cd_create_dialog_parent_num(838,837);
	
	put_placed_monst_adv_in_dlog();

	for (i = 0; i < 8; i++) 
		cd_add_label(838,22 + i,time_labels[i],57);
	cd_add_label(838,30,"None",18);
	for (i = 0; i < 10; i++) {
		sprintf((char *) temp_str,"%d",i + 1);
		cd_add_label(838,31 + i,(char *) temp_str,18);
		}
		
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(838,0);
	return store_placed_monst2;
}

void put_placed_item_in_dlog()
{
	char str[256];
	short i;
	
	cdsin(836,17,store_which_placed_item);
	sprintf((char *) str,"X = %d, Y = %d",store_placed_item.item_loc.x,store_placed_item.item_loc.y);
	csit(836,22,(char *) str);
	csit(836,15,scen_item_list.scen_items[store_placed_item.item_code].full_name);
	CDSN(836,2,store_placed_item.ability);
	if (store_placed_item.always_there == TRUE)
		cd_set_led(836,12,1);
		else cd_set_led(836,12,0);
	if (store_placed_item.property == TRUE)
		cd_set_led(836,13,1);
		else cd_set_led(836,13,0);
	if (store_placed_item.contained == TRUE)
		cd_set_led(836,14,1);
		else cd_set_led(836,14,0);
	
	i = scen_item_list.scen_items[store_placed_item.item_code].graphic_num;
	if (i >= 1000)
	 	csp(836,4,950);
	 	else csp(836,4,1800 + i);
}
Boolean get_placed_item_in_dlog()
{
	char str[256];
	short i;
	
	store_placed_item.ability = CDGN(836,2);
	if ((store_placed_item.ability < -1) || (store_placed_item.ability > 2500)) {
		give_error("Number of charges/amount of gold or food must be from 0 to 2500."
			,"If an item with charges (not gold or food) leave this at -1 for the item to have the default number of charges.",836);
		return FALSE;
		}
	i = scen_item_list.scen_items[store_placed_item.item_code].variety;
	if ((store_placed_item.ability == 0) && ((i == 3) || (i == 11))) {
		give_error("You must assign gold or food an amount of at least 1.","",836);
		return FALSE;
		}
	store_placed_item.always_there = cd_get_led(836,12);
	store_placed_item.property = cd_get_led(836,13);
	store_placed_item.contained = cd_get_led(836,14);

	town.preset_items[store_which_placed_item] = store_placed_item;
	return TRUE;
}

void edit_placed_item_event_filter (short item_hit)
{
	char str[256];
	short i;
	creature_start_type store_m;
	
	switch (item_hit) {
		case 3:
			if (get_placed_item_in_dlog() == FALSE)
				break;
			dialog_not_toast = FALSE;
			break;
		case 20:
			dialog_not_toast = FALSE;
			break;
		case 18:
			i = choose_text_res(-2,0,399,store_placed_item.item_code,836,"Place which item?");
			if (i >= 0) {
				store_placed_item.item_code = i;
				put_placed_item_in_dlog();		
				}
			break;

		default:
			for (i = 12; i <= 14; i++)
				cd_flip_led(836,i,item_hit);
			break;
		}
}

void edit_placed_item(short which_i)
{
	
	short item_hit,i,store_dialog_answer;
	char temp_str[256];
	
	store_placed_item = town.preset_items[which_i];
	store_which_placed_item = which_i;
	
	cd_create_dialog_parent_num(836,0);
	
	cd_activate_item(836,19,0);
	put_placed_item_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(836,0);

}

void edit_sign_event_filter (short item_hit)
{
	short i;
	Boolean had1 = FALSE, had2 = FALSE;
	
	switch (item_hit) {
		case 9: case 4:
			if (editing_town == FALSE)
				CDGT(831,2,data_store->out_strs[100 + store_which_sign]);
				else CDGT(831,2,town_strs[120 + store_which_sign]);
			if (item_hit == 9)
				dialog_not_toast = FALSE;
				else {
					if (item_hit == 3)
						store_which_sign--;
						else store_which_sign++;
					if (store_which_sign < 0)
						store_which_sign = (editing_town == TRUE) ? 14 : 7;
					if (store_which_sign > (editing_town == TRUE) ? 14 : 7)
						store_which_sign = 0;
					}
			break;
		case 3: 
			dialog_not_toast = FALSE;
			break;
		}
	cdsin(831,5,store_which_sign);
	if (editing_town == FALSE)
		CDST(831,2,data_store->out_strs[100 + store_which_sign]);
		else CDST(831,2,town_strs[120 + store_which_sign]);
}

void edit_sign(short which_sign,short picture)
{

	short item_hit;
	char sign_text[256];
	location view_loc;
	Boolean sound_done = FALSE;

	store_which_sign = which_sign;
	
	cd_create_dialog_parent_num(831,0);
	
	if (picture >= 1000)
		csp(831,6,950);
	else if (picture >= 400)
		csp(831,6,picture - 100);
	else csp(831,6,picture);

	cdsin(831,5,store_which_sign);
	if (editing_town == FALSE)
		CDST(831,2,data_store->out_strs[100 + store_which_sign]);
		else CDST(831,2,town_strs[120 + store_which_sign]);
	//cd_activate_item(831,3,0);
	//cd_activate_item(831,4,0);
	while (dialog_not_toast)
		ModalDialog();
	
	cd_kill_dialog(831,0);
}

Boolean save_out_strs()
{
	char str[256];
	short i;
	
	for (i = 0; i < 8; i++) {
		CDGT(850,2 + i,(char *) str);
		sprintf((char *)data_store->out_strs[i + 1],"%-29.29s",str);
		if (str_do_delete[i] > 0)
			current_terrain.info_rect[i].right = -1;
		}
	return TRUE;
}

void put_out_strs_in_dlog()
{
	char str[256];
	short i;

	for (i = 0; i < 8; i++) {
		if ((current_terrain.info_rect[i].left < 1) && (current_terrain.info_rect[i].top < 1) && (current_terrain.info_rect[i].right < 1) && (current_terrain.info_rect[i].bottom < 1))
		 {
			sprintf((char *) str,"Not yet placed.");
			cd_activate_item(850,25 + i,0);
			}
			else
		sprintf((char *) str,"L = %d, T = %d",current_terrain.info_rect[i].left,current_terrain.info_rect[i].top);
		csit(850,13 + i,(char *) str);
		sprintf((char *) str,"R = %d, B = %d",current_terrain.info_rect[i].right,current_terrain.info_rect[i].bottom);
		csit(850,33 + i,(char *) str);
		CDST(850,2 + i,data_store->out_strs[i + 1]);
		}
}

void edit_out_strs_event_filter (short item_hit)
{
	char str[256];
	short i;
	
	switch (item_hit) {
		case 10:
			if (save_out_strs() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 11:
				 dialog_not_toast = FALSE; 
			break;
		default:
			if ((item_hit >= 25) && (item_hit <= 32)) {
				//sprintf((char *)data_store->out_strs[item_hit - 25 + 1],"");
				CDST(850,2 + item_hit - 25,"");
				str_do_delete[item_hit - 25] = 1;
				put_out_strs_in_dlog();
				}
			break;
		}
}

void edit_out_strs()
// ignore parent in Mac version
{
	short out_strs_hit,i,store_dialog_answer;
	char temp_str[256];
	
	for (i = 0; i < 8; i++)
		str_do_delete[i] = 0;
		
	cd_create_dialog_parent_num(850,0);
	
	put_out_strs_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(850,0);
}


Boolean save_town_strs()
{
	char str[256];
	short i;
	
	for (i = 0; i < 16; i++) {
		CDGT(839,2 + i,(char *) str);
		sprintf((char *)town_strs[i + 1],"%-29.29s",str);
		if (str_do_delete[i] > 0)
			t_d.room_rect[i].right = -1;
		}
	return TRUE;
}

void put_town_strs_in_dlog()
{
	char str[256];
	short i;
	
	for (i = 0; i < 16; i++) {
//		if ((t_d.room_rect[i].left < 1) && (t_d.room_rect[i].top < 1) && (t_d.room_rect[i].right < 1) && (t_d.room_rect[i].bottom < 1)) {
//rectangle (0,0,0,0) is a valid rectangle (and in fact placed in legacy scenarios). With the new scenario format, the rectangle may have its own utility. Note that, the deleted test was omitted in the previous check !!!
    if ((t_d.room_rect[i].right == -1) || (str_do_delete[i] > 0)) {
			cd_activate_item(839,41 + i,0);
			sprintf(str,"Not yet placed.");
    		csit(839,21 + i,str);
			sprintf(str,"Not yet placed.");
        	csit(839,57 + i,str);
			}
		else{
		  sprintf( str,"L = %d, T = %d",t_d.room_rect[i].left,t_d.room_rect[i].top);
		  csit(839,21 + i,str);
		  sprintf(str,"R = %d, B = %d",t_d.room_rect[i].right,t_d.room_rect[i].bottom);
		  csit(839,57 + i,str);
          }
	  CDST(839,2 + i,town_strs[i + 1]);          
	 }
}

void edit_town_strs_event_filter (short item_hit)
{
	char str[256];
	short i;
	
	switch (item_hit) {
		case 18:
			if (save_town_strs() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 19:
				 dialog_not_toast = FALSE; 
			break;
		default:
			if ((item_hit >= 41) && (item_hit <= 56)) {
				//sprintf((char *)town_strs[item_hit - 41 + 1],"");
				CDST(839,2 + item_hit - 41,"");
				str_do_delete[item_hit - 41] = 1;
				put_town_strs_in_dlog();
				}
			break;
		}
}

void edit_town_strs()
// ignore parent in Mac version
{
	short town_strs_hit,i,store_dialog_answer;
	char temp_str[256];
	
	for (i = 0; i < 16; i++)
		str_do_delete[i] = 0;
		
	cd_create_dialog_parent_num(839,0);
	
	put_town_strs_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(839,0);
}



void pick_town_num_event_filter (short item_hit)
{
	char str[256];
	short i;
	
	switch (item_hit) {
		case 3:
			dialog_answer = CDGN(store_whigh_dlog,2);
			if ((dialog_answer < 0) || (dialog_answer >= scenario.num_towns)) {
				give_error("This number is out of the correct range. (0 to the number of towns minus 1)","",store_whigh_dlog);
				break;
				}
			 dialog_not_toast = FALSE; 
			break;
		case 4:
			dialog_answer = -1;
				 dialog_not_toast = FALSE; 
			break;

		}
}

short pick_town_num(short which_dlog,short def)
// ignore parent in Mac version
{
	short town_strs_hit,i,store_dialog_answer;
	char temp_str[256], str2[256];
	
	 store_whigh_dlog = which_dlog;
	
	cd_create_dialog_parent_num(store_whigh_dlog,0);
	
	CDSN(store_whigh_dlog,2,def);
	cd_get_item_text(which_dlog,7,(char *) temp_str);
	sprintf((char *) str2,"%s (0 - %d)",(char *) temp_str,scenario.num_towns - 1);
	csit(which_dlog,7,(char *) str2);
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(store_whigh_dlog,0);
	return dialog_answer;
}

void change_ter_event_filter (short item_hit)
{
	char str[256];
	short i;
	
	switch (item_hit) {
		case 5:
			a = CDGN(857,2);
			b = CDGN(857,3);
			c = CDGN(857,4);
			if (cre(a,0,255,"Both terrain types must be from 0 to 255.","",857) == TRUE) break;
			if (cre(b,0,255,"Both terrain types must be from 0 to 255.","",857) == TRUE) break;
			if (cre(c,0,100,"The Chance must be from 0 to 100.","",857) == TRUE) break;

			 dialog_not_toast = FALSE; 
			break;
		case 6:
			a = -1; b = -1; c = -1;
			dialog_not_toast = FALSE; 
			break;
		case 10: case 11:
			i = CDGN(857,item_hit - 8);
			i = choose_text_res(-4,0,255,i,857,"Which Terrain?");
			if (i >= 0)
				CDSN(857,item_hit - 8,i);
			break;
		}
}

void change_ter(short *change_from,short *change_to,short *chance)
// ignore parent in Mac version
{
	short town_strs_hit,i,store_dialog_answer;
	char temp_str[256];
	
	cd_create_dialog_parent_num(857,0);
	
	CDSN(857,2,0);
	CDSN(857,3,0);
	CDSN(857,4,100);
	
	while (dialog_not_toast)
		ModalDialog();

	*change_from = a;
	*change_to = b; 
	*chance = c;
	
	cd_kill_dialog(857,0);
}

void outdoor_details_event_filter (short item_hit)
{
	char str[256];
	short i;
	
	switch (item_hit) {
		case 3:
			CDGT(851,2,(char *) str);
			str[29] = 0;
			sprintf(data_store->out_strs[0],"%s",(char *) str);
			dialog_not_toast = FALSE; 
			break;

		}
}

void outdoor_details()
// ignore parent in Mac version
{
	short town_strs_hit,i,store_dialog_answer;
	char temp_str[256];
	
	
	cd_create_dialog_parent_num(851,0);
	
	CDST(851,2,data_store->out_strs[0]);
	sprintf((char *) temp_str,"X = %d, Y = %d",cur_out.x,cur_out.y);
	csit(851,8,(char *) temp_str);
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(851,0);
}

void put_out_wand_in_dlog()
{
	char str[256];
	short i;
	
	cdsin(852,45,store_which_out_wand);
	for (i = 0; i < 7; i++)
		if (store_out_wand.monst[i] == 0)
			csit(852, i + 7, "Empty");
			else {
				get_str(str,-1,store_out_wand.monst[i]);
				csit(852,i + 7,(char *) str);
				}
	for (i = 0; i < 3; i++)
		if (store_out_wand.friendly[i] == 0)
			csit(852, i + 7 + 7, "Empty");
			else {
				get_str(str,-1,store_out_wand.friendly[i]);
				csit(852,i + 7 + 7,(char *) str);
				}
	if (store_out_wand.cant_flee % 10 == 1)
		cd_set_led(852,51,1); 
		else cd_set_led(852,51,0);
	if (store_out_wand.cant_flee >= 10)
		cd_set_led(852,53,1); 
		else cd_set_led(852,53,0);
	CDSN(852,2,store_out_wand.spec_on_meet);
	CDSN(852,3,store_out_wand.spec_on_win);
	CDSN(852,4,store_out_wand.spec_on_flee);
	CDSN(852,5,store_out_wand.end_spec1);
	CDSN(852,6,store_out_wand.end_spec2);
}
Boolean get_out_wand_in_dlog()
{
	char str[256];
	short i;
	
	store_out_wand.spec_on_meet = CDGN(852,2);
	store_out_wand.spec_on_win = CDGN(852,3);
	store_out_wand.spec_on_flee = CDGN(852,4);
	store_out_wand.end_spec1 = CDGN(852,5);
	store_out_wand.end_spec2 = CDGN(852,6);
	if (cre(store_out_wand.end_spec1,
		-1,299,"First part of Stuff Done flag must be from 0 to 299 (or -1 if it's not used).","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.end_spec2,
		-1,9,"Second part of Stuff Done flag must be from 0 to 9 (or -1 if it's not used).","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.spec_on_meet,
		-1,59,"Outdoor Special Nodes run from 0 to 59 (or -1 if not used).","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.spec_on_win,
		-1,59,"Outdoor Special Nodes run from 0 to 59 (or -1 if not used).","",852) == TRUE) return FALSE;
	if (cre(store_out_wand.spec_on_flee,
		-1,59,"Outdoor Special Nodes run from 0 to 59 (or -1 if not used).","",852) == TRUE) return FALSE;
	
	store_out_wand.cant_flee = 0;
	if (cd_get_led(852,53) > 0)
		store_out_wand.cant_flee += 10;
	if (cd_get_led(852,51) > 0)
		store_out_wand.cant_flee += 1;
		
	if (store_out_wand_mode == 0)
		current_terrain.wandering[store_which_out_wand] = store_out_wand;
		else current_terrain.special_enc[store_which_out_wand] = store_out_wand;
	return TRUE;
}

void edit_out_wand_event_filter (short item_hit)
{
	char str[256];
	short i,spec;
	creature_start_type store_m;
	
	switch (item_hit) {
		case 17:
			if (get_out_wand_in_dlog() == FALSE)
				break;
			dialog_not_toast = FALSE;
			break;
		case 18:
			dialog_not_toast = FALSE;
			break;
		case 19:
			if (get_out_wand_in_dlog() == FALSE) break;
			store_which_out_wand--;
			if (store_which_out_wand < 0) store_which_out_wand = 3;
			store_out_wand = (store_out_wand_mode == 0) ? 
				current_terrain.wandering[store_which_out_wand] : current_terrain.special_enc[store_which_out_wand];
			put_out_wand_in_dlog();
			break;
		case 20:
			if (get_out_wand_in_dlog() == FALSE) break;
			store_which_out_wand++;
			if (store_which_out_wand > 3) store_which_out_wand = 0;
			store_out_wand = (store_out_wand_mode == 0) ? 
				current_terrain.wandering[store_which_out_wand] : current_terrain.special_enc[store_which_out_wand];
			put_out_wand_in_dlog();
			break;

		case 31: case 32: case 33:
			if (get_out_wand_in_dlog() == FALSE)
				 break; 
			spec = CDGN(852,item_hit - 29);
			if ((spec < 0) || (spec >= 60)) {
				spec = get_fresh_spec(1);
				if (spec < 0) {
					give_error("You can't create a new special encounter because there are no more free special nodes.",
						"To free a special node, set its type to No Special and set its Jump To special to -1.",852);
					break;
					}
				CDSN(852,item_hit - 29,spec);
				}	
			edit_spec_enc(spec,1,852);
			if ((spec >= 0) && (spec < 60) && (current_terrain.specials[spec].pic < 0))
				CDSN(852,item_hit - 29,-1);
			if (get_out_wand_in_dlog() == FALSE)
				 break; 
			break;
		default:
			if (get_out_wand_in_dlog() == FALSE)
				 break; 
			cd_flip_led(852,51,item_hit);
			cd_flip_led(852,53,item_hit);
			if ((item_hit >= 21) && (item_hit <= 27)) {
				i = choose_text_res(-1,0,255,store_out_wand.monst[item_hit - 21],852,"Choose Which Monster:"); 
				if (i >= 0) store_out_wand.monst[item_hit - 21] = i;
				put_out_wand_in_dlog();
				}
			if ((item_hit >= 28) && (item_hit <= 30)) {
				i = choose_text_res(-1,0,255,store_out_wand.friendly[item_hit - 28],852,"Choose Which Monster:"); 
				if (i >= 0) store_out_wand.friendly[item_hit - 28] = i;
				put_out_wand_in_dlog();
				}
			break;
		}
}

// mode 0 - wandering 1 - special
void edit_out_wand(short mode)
{
	
	short item_hit,i,store_dialog_answer;
	char temp_str[256];

	store_which_out_wand = 0;
	store_out_wand_mode = mode;
	store_out_wand = (store_out_wand_mode == 0) ? current_terrain.wandering[0] : current_terrain.special_enc[0];
	
	cd_create_dialog_parent_num(852,0);
	
	if (mode == 1)
		csit(852,47,"Outdoor Special Encounter:");

	put_out_wand_in_dlog();

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(852,0);

}

Boolean save_town_details()
{
	char str[256];
	short i;
	
	CDGT(832,2,(char *) str);
	sprintf(town_strs[0],"%-29.29s",str);
	town.town_chop_time = CDGN(832,3);
	if (cre(town.town_chop_time,-1,10000,"The day the town becomes abandoned must be from 0 to 10000 (or -1 if it doesn't)."
		,"",832) == TRUE) return FALSE;
	town.town_chop_key = CDGN(832,4);
	if (cre(town.town_chop_key,-1,10,"The event which prevents the town from becoming abandoned must be from 1 to 10 (-1 or 0 for none)."
		,"",832) == TRUE) return FALSE;
	town.lighting = cd_get_led_range(832,19,22);
	town.max_num_monst = CDGN(832,5);
	town.difficulty = CDGN(832,6);
	if (cre(town.difficulty,0,10,"The town difficulty must be between 0 (easiest) and 10 (hardest)."
		,"",832) == TRUE) return FALSE;
	
	return TRUE;
}

void put_town_details_in_dlog()
{
	char str[256];
	short i;
	
	CDST(832,2,town_strs[0]);
	CDSN(832,3,town.town_chop_time);
	CDSN(832,4,town.town_chop_key);
	cd_set_led_range(832,19,22,town.lighting);
	CDSN(832,5,town.max_num_monst);
	CDSN(832,6,town.difficulty);
	
}

void edit_town_details_event_filter (short item_hit)
{
	char str[256];
	short i;
	
	switch (item_hit) {
		case 7:
			if (save_town_details() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		default:
			cd_hit_led_range(832,19,22,item_hit);
			break;
		}
}

void edit_town_details()
// ignore parent in Mac version
{
	short town_details_hit,i,store_dialog_answer;
	char temp_str[256];
		
	cd_create_dialog_parent_num(832,0);
	
	put_town_details_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(832,0);
}

Boolean save_town_events()
{
	char str[256];
	short i;
	
	for (i = 0; i < 8; i++) {
		town.timer_spec_times[i] = CDGN(833,2 + i);
		town.timer_specs[i] = CDGN(833,10 + i);
		if (cre(town.timer_specs[i],-1,99,"The town special nodes must be between 0 at 99 (or -1 for no special)."
			,"",833) == TRUE) return FALSE;
		}
	return TRUE;
}

void put_town_events_in_dlog()
{
	char str[256];
	short i;
	
	for (i = 0; i < 8; i++) {
		CDSN(833,2 + i,town.timer_spec_times[i]);
		CDSN(833,10 + i,town.timer_specs[i]);
		}

}

void edit_town_events_event_filter (short item_hit)
{
	char str[256];
	short i,spec;
	
	switch (item_hit) {
		case 18:
			if (save_town_events() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		default:
			if ((item_hit >= 26) && (item_hit <= 33)) {
				if (save_town_events() == FALSE)
					 break; 
				spec = CDGN(833,item_hit - 26 + 10);
				if ((spec < 0) || (spec > 99)) {
					spec = get_fresh_spec(2);
					if (spec < 0) {
						give_error("You can't create a new special encounter because there are no more free special nodes.",
							"To free a special node, set its type to No Special and set its Jump To special to -1.",833);
						break;
						}
					CDSN(833,item_hit - 26 + 10,spec);
					}	
				edit_spec_enc(spec,2,833);
				if ((spec >= 0) && (spec < 100) && (town.specials[spec].pic < 0))
					CDSN(833,item_hit - 26 + 10,-1);
				}
			break;
		}
}

void edit_town_events()
// ignore parent in Mac version
{
	short advanced_town_hit,i,store_dialog_answer;
	char temp_str[256];
	
	
	cd_create_dialog_parent_num(833,0);
	
	put_town_events_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(833,0);
}

Boolean save_advanced_town()
{
	char str[256];
	short i;
	
	for (i = 0; i < 4; i++) {
		town.exit_specs[i] = CDGN(834,2 + i);
		if (cre(town.exit_specs[i],-1,99,"The town exit specials must be between 0 at 99 (or -1 for no special)."
			,"",834) == TRUE) return FALSE;
		town.exit_locs[i].x = CDGN(834,8 + i * 2);
		if (cre(town.exit_locs[i].x,-1,47,"The town exit coordinates must be from 0 to 47 (or -1 if you want them ignored)."
			,"",834) == TRUE) return FALSE;
		town.exit_locs[i].y = CDGN(834,9 + i * 2);
		if (cre(town.exit_locs[i].y,-1,47,"The town exit coordinates must be from 0 to 47 (or -1 if you want them ignored)."
			,"",834) == TRUE) return FALSE;
		}
	town.spec_on_entry = CDGN(834,6);
	town.spec_on_entry_if_dead = CDGN(834,7);
	town.hostile_spec_to_call = CDGN(834,43);
	if (cre(town.spec_on_entry,-1,99,"The town entry specials must be from 0 to 99 (or -1 for no special)."
		,"",834) == TRUE) return FALSE;
	if (cre(town.spec_on_entry_if_dead,-1,99,"The town entry specials must be from 0 to 99 (or -1 for no special)."
		,"",834) == TRUE) return FALSE;
	if (cre(town.hostile_spec_to_call,-1,99,"The town specials to call must be from 0 to 99 (or -1 for no special)."
		,"",834) == TRUE) return FALSE;	
	scenario.town_hidden[cur_town] = cd_get_led(834,37);
	town.defy_mapping = cd_get_led(834,39);
	town.defy_scrying = cd_get_led(834,41);	
	return TRUE;
}

void put_advanced_town_in_dlog()
{
	char str[256];
	short i;
	
	for (i = 0; i < 4; i++) {
		CDSN(834,2 + i,town.exit_specs[i]);
		CDSN(834,8 + i * 2,town.exit_locs[i].x);
		CDSN(834,9 + i * 2,town.exit_locs[i].y);
		CDSN(834,43,town.hostile_spec_to_call);
		}
	CDSN(834,6,town.spec_on_entry);
	CDSN(834,7,town.spec_on_entry_if_dead);
	cd_set_led(834,37,scenario.town_hidden[cur_town]);
	cd_set_led(834,39,town.defy_mapping);	
	cd_set_led(834,41,town.defy_scrying);		
}

void edit_advanced_town_event_filter (short item_hit)
{
	char str[256];
	short i,spec;
	
	switch (item_hit) {
		case 16:
			if (save_advanced_town() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 17:
				 dialog_not_toast = FALSE; 
			break;
		case 37:
			cd_flip_led(834,37,item_hit);
			break;
		case 39:
			cd_flip_led(834,39,item_hit);
			break;	   
		case 41:
    		cd_flip_led(834,41,item_hit);
			break;	   
		}
}

void edit_advanced_town()
// ignore parent in Mac version
{
	short advanced_town_hit,i,store_dialog_answer;
	char temp_str[256];
	
	
	cd_create_dialog_parent_num(834,0);
	
	put_advanced_town_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(834,0);
}

Boolean save_town_wand()
{
	char str[256];
	short i,j;
	
	for (i = 0; i < 4; i++) 
	for (j = 0; j < 4; j++) 
		{
		town.wandering[i].monst[j] = CDGN(835,2 + i * 4 + j);
		if (cre(town.wandering[i].monst[j],0,255,"Wandering monsters must all be from 0 to 255 (0 means no monster)."
			,"",835) == TRUE) return FALSE;
		}	
		
	return TRUE;
}

void put_town_wand_in_dlog()
{
	char str[256];
	short i,j;
	
	for (i = 0; i < 4; i++) 
	for (j = 0; j < 4; j++) 
		{
		CDSN(835,2 + i * 4 + j,town.wandering[i].monst[j]);
		}

}

void edit_town_wand_event_filter (short item_hit)
{
	char str[256];
	short i,j,spec;
	
	switch (item_hit) {
		case 18:
			if (save_town_wand() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 27: case 28: case 29: case 30:
			for (i = 0; i < 4; i++) {
				j = choose_text_res(-1,0,255,town.wandering[item_hit - 27].monst[i],835,"Choose Which Monster:"); 
				if (j < 0)
					i = 5;
					else {
						CDSN(835,2 + (item_hit - 27) * 4 + i,j);
						}
				}
			break;
		}
}

void edit_town_wand()
// ignore parent in Mac version
{
	short town_wand_hit,i,store_dialog_answer;
	char temp_str[256];
	
	
	cd_create_dialog_parent_num(835,0);
	
	put_town_wand_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(835,0);
}

Boolean save_basic_dlog()
{
	char str[256];
	short i,j;
	
	CDGT(821,2,talk_strs[store_which_talk_node]);
	talk_strs[store_which_talk_node][30] = 0;
	CDGT(821,3,talk_strs[160 + store_which_talk_node]);
	CDGT(821,4,talk_strs[10 + store_which_talk_node]);
	CDGT(821,5,talk_strs[20 + store_which_talk_node]);
	CDGT(821,6,talk_strs[30 + store_which_talk_node]);
		
	return TRUE;
}

void put_basic_dlog_in_dlog()
{
	char str[256];
	short i,j;
	
	cdsin(821,14,store_which_talk_node + cur_town * 10);
	CDST(821,2,talk_strs[store_which_talk_node]);
	CDST(821,3,talk_strs[160 + store_which_talk_node]);
	CDST(821,4,talk_strs[10 + store_which_talk_node]);
	CDST(821,5,talk_strs[20 + store_which_talk_node]);
	CDST(821,6,talk_strs[30 + store_which_talk_node]);

}

void edit_basic_dlog_event_filter (short item_hit)
{
	char str[256];
	short i,j,spec;
	
	switch (item_hit) {
		case 7:
			if (save_basic_dlog() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 8:
				 dialog_not_toast = FALSE; 
			break;
		case 9:
			if (save_basic_dlog() == FALSE) break;
			store_which_talk_node--;
			if (store_which_talk_node < 0) store_which_talk_node = 9;
			put_basic_dlog_in_dlog();
			break;
		case 10:
			if (save_basic_dlog() == FALSE) break;
			store_which_talk_node++;
			if (store_which_talk_node > 9) store_which_talk_node = 0;
			put_basic_dlog_in_dlog();
			break;
		}
}

void edit_basic_dlog(short which_node)
// ignore parent in Mac version
{
	short basic_dlog_hit,i,store_dialog_answer;
	char temp_str[256];
	
	store_which_talk_node = which_node;
	
	cd_create_dialog_parent_num(821,0);
	
	cd_attach_key(821,10,0);
	cd_attach_key(821,9,0);
	
	put_basic_dlog_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(821,0);
}

Boolean save_talk_node()
{
	char str[256];
	short i;
	

	store_talk_node.personality = CDGN(817,2);
	if ((store_talk_node.personality >= 0) &&
		((store_talk_node.personality < cur_town * 10) || (store_talk_node.personality >= (cur_town + 1) * 10))) {
			sprintf((char *) str,"The legal range for personalities in this town is from %d to %d.",
				cur_town * 10,cur_town * 10 + 9,817);
			give_error("Personalities in talk nodes must be -1 (for unused node), -2 (all personalities use) or in the legal range of personalities in this town.",
				(char *) str,817);
				return FALSE;
				} 
	CDGT(817,3,(char *) str);
	for (i = 0; i < 4; i++) {
		store_talk_node.link1[i] = str[i];
		if ((str[i] < 97) || (str[i] > 122)) {
			give_error("The words this node is the response to must be at least 4 characters long, and all characters must be lower case letters.",
				"",817);
			return FALSE;
			}
		}
	CDGT(817,4,(char *) str);
	for (i = 0; i < 4; i++) {
		store_talk_node.link2[i] = str[i];
		if ((str[i] < 97) || (str[i] > 122)) {
			give_error("The words this node is the response to must be at least 4 characters long, and all characters must be lower case letters.",
				"",817);
			return FALSE;
			}		
		}
	for (i = 0; i < 4; i++)
		store_talk_node.extras[i] = CDGN(817,5 + i);
	
	switch (store_talk_node.type) {
		case 1: case 2: 
			if (cre(store_talk_node.extras[0],0,299,"First part of Stuff Done flag must be from 0 to 299.","",817) > 0) return FALSE;
			if (cre(store_talk_node.extras[1],0,9,"Second part of Stuff Done flag must be from 0 to 9.","",817) > 0) return FALSE;
			break;	
		case 3: 
			if (cre(store_talk_node.extras[0],0,1000,"Inn cost must be from 0 to 1000.","",817) > 0) return FALSE;
			if (cre(store_talk_node.extras[1],0,3,"Inn quality must be from 0 to 3.","",817) > 0) return FALSE;
			break;	
		case 5: 
			if (cre(store_talk_node.extras[1],0,9,"Event must be from 0 to 9. (0 means no event)","",817) > 0) return FALSE;
			break;	
		case 6: 
			if (cre(store_talk_node.extras[0],0,199,"Town number must be from 0 to 199.","",817) > 0) return FALSE;
			break;	
		case 7: case 9: case 10: case 11: case 12:
			if (cre(store_talk_node.extras[0],0,6,"Cost adjustment must be from 0 (cheapest) to 6 (most expensive).","",817) > 0) return FALSE;
			break;	
		case 17:
			if (cre(store_talk_node.extras[0],0,6,"Enchantment type must be from 0 to 6. See the documentation for a list of possible abilities.","",817) > 0) return FALSE;
			break;	
		case 19: case 23: 
			if (cre(store_talk_node.extras[1],0,299,"First part of Stuff Done flag must be from 0 to 299.","",817) > 0) return FALSE;
			if (cre(store_talk_node.extras[2],0,9,"Second part of Stuff Done flag must be from 0 to 9.","",817) > 0) return FALSE;
			break;	
		case 20: case 21:
			if (cre(store_talk_node.extras[1],0,29,"The first boat/horse must be in the legal range (0 - 29).","",817) > 0) return FALSE;
			break;	
		case 22:
			if (cre(store_talk_node.extras[0],0,49,"The special item must be in the legal range (0 - 49).","",817) > 0) return FALSE;
			break;	
		case 29:
			if (cre(store_talk_node.extras[0],-1,99,"The town special node called must be in the legal range (0 - 99), or -1 for No Special.","",817) > 0) return FALSE;
			break;	
		case 30:
			if (cre(store_talk_node.extras[0],-1,255,"The scenario special node called must be in the legal range (0 - 255), or -1 for No Special.","",817) > 0) return FALSE;
			break;	
	}
	
	CDGT(817,9,talk_strs[40 + store_which_talk_node * 2]);
	CDGT(817,10,talk_strs[40 + store_which_talk_node * 2 + 1]);
	
	talking.talk_nodes[store_which_talk_node] = store_talk_node;

	return TRUE;
}

void put_talk_node_in_dlog()
{
	char str[256];
	short i;

	CDSN(817,2,store_talk_node.personality);
	str[4] = 0;
	for (i = 0; i < 4; i++)
		str[i] = store_talk_node.link1[i];
	CDST(817,3,(char *) str);
	for (i = 0; i < 4; i++)
		str[i] = store_talk_node.link2[i];
	CDST(817,4,(char *) str);
	get_str(str,40,store_talk_node.type * 7 + 1);
	csit(817,15,(char *) str);
	get_str(str,40,store_talk_node.type * 7 + 2);
	csit(817,16,(char *) str);
	get_str(str,40,store_talk_node.type * 7 + 3);
	csit(817,17,(char *) str);
	get_str(str,40,store_talk_node.type * 7 + 4);
	csit(817,18,(char *) str);
	get_str(str,40,store_talk_node.type * 7 + 5);
	csit(817,19,(char *) str);
	get_str(str,40,store_talk_node.type * 7 + 6);
	csit(817,20,(char *) str);
	get_str(str,40,store_talk_node.type * 7 + 7);
	csit(817,21,(char *) str);
	for (i = 0; i < 4; i++)
		CDSN(817,5 + i,store_talk_node.extras[i]);
	CDST(817,9,talk_strs[40 + store_which_talk_node * 2]);
	CDST(817,10,talk_strs[40 + store_which_talk_node * 2 + 1]);

	if ((store_talk_node.type == 7) || (store_talk_node.type == 9) ||
		(store_talk_node.type == 10) || (store_talk_node.type == 11))
		cd_activate_item(817,37,1);
		else cd_activate_item(817,37,0);
	if (last_talk_node[0] >= 0)
		cd_activate_item(817,13,1);
		else cd_activate_item(817,13,0);
	if ((store_talk_node.type != 29) && (store_talk_node.type != 30))
		cd_activate_item(817,38,0);
		else cd_activate_item(817,38,1);

}

void edit_talk_node_event_filter (short item_hit)
{
	char str[256];
	short i,j,node_to_change_to = -1,spec;
	
	switch (item_hit) {
		case 11:
			if (save_talk_node() == TRUE)
				 dialog_not_toast = FALSE; 
			break;
		case 13:  //go_back
			if (save_talk_node() == FALSE)
				 break; 
			for (i = 0; i < 60; i++) 
				if (last_talk_node[i] < 0) {
					node_to_change_to = last_talk_node[i - 1];
					if (i > 0)
						last_talk_node[i - 1] = -1;
					i = 256;
					}
			break;
		case 12: 
			
			dialog_not_toast = FALSE; break;
		case 14:
			if (save_talk_node() == FALSE)
				 break; 

				spec = -1;
				for (j = 0; j < 60; j++)
					if ((talking.talk_nodes[j].personality < 0) && (talking.talk_nodes[j].extras[3] < 0)) {
						spec = j;
						j = 60;
						}
				
				if (spec < 0) {
					give_error("You have used all 60 available talk nodes. To create fresh dialogue, go back and reuse and old one.",
						"",817);
					break;
					}

			for (i = 0; i < 60; i++) 
				if (last_talk_node[i] < 0) {
					last_talk_node[i] = store_which_talk_node;
					node_to_change_to = spec;
					i = 60;
					}					
			break;
			
		case 28:
			if (save_talk_node() == FALSE)
				 break; 
			i = choose_text_res(-5,0,30,store_talk_node.type,817,"What Talking Node type?");
			if (i >= 0) {
				store_talk_node.type = i;
				if ((store_talk_node.type == 29) || (store_talk_node.type == 30))
					store_talk_node.extras[0] = -1;
				put_talk_node_in_dlog();
				}
				
			break;
		case 37:
			i = CDGN(817,6);
			if (store_talk_node.type == 9) {
				i = choose_text_res(38,1,32,i + 1,817,"What is the first spell in the shop?");
				if (i >= 0)
					CDSN(817,6,i - 1);
				}
			else if (store_talk_node.type == 10) {
				i = choose_text_res(38,50,81,i + 50,817,"What is the first spell in the shop?");
				if (i >= 0)
					CDSN(817,6,i - 50);
				}
			else if (store_talk_node.type == 11) {
				i = choose_text_res(38,100,119,i + 100,817,"What is the first recipe in the shop?");
				if (i >= 0)
					CDSN(817,6,i - 100);
				}
			else {
				i = choose_text_res(-2,0,399,i,817,"What is the first item in the shop?");
				if (i >= 0)
					CDSN(817,6,i);
				}
			break;
		case 38:
			if (save_talk_node() == FALSE)
				 break; 
			spec = CDGN(817,5);
			if ((spec < 0) || (spec >= 100)) {
				if (store_talk_node.type == 29)
					spec = get_fresh_spec(2);
					else spec = get_fresh_spec(0);
				if (spec < 0) {
					give_error("You can't create a new special encounter because there are no more free special nodes.",
						"To free a special node, set its type to No Special and set its Jump To special to -1.",817);
					break;
					}
				CDSN(817,5,spec);
				}	
			if (store_talk_node.type == 29)
				edit_spec_enc(spec,2,817);
				else edit_spec_enc(spec,0,817);
			if (store_talk_node.type == 29) {
				if ((spec >= 0) && (spec < 100) && (town.specials[spec].pic < 0))
					CDSN(817,5,-1);
				}
				else {
					if ((spec >= 0) && (spec < 256) && (scenario.scen_specials[spec].pic < 0))
						CDSN(817,5,-1);
					}
			if (save_talk_node() == FALSE)
				 break; 		
			break;
		}
	if (node_to_change_to >= 0) {
		store_which_talk_node = node_to_change_to;
		store_talk_node = talking.talk_nodes[store_which_talk_node];
		if (store_talk_node.extras[3] < 0)
			store_talk_node.extras[3] = 0;
		put_talk_node_in_dlog();
		}	
}

void edit_talk_node(short which_node,short parent_num)
// ignore parent in Mac version
{
	short talk_node_hit,i,store_dialog_answer;
	char temp_str[256];
	
	store_which_talk_node = which_node;
	for (i = 0; i < 60; i++)
		last_talk_node[i] = -1;
	//last_talk_node[0] = store_which_mode * 1000 + store_which_talk_node;
	store_talk_node = talking.talk_nodes[store_which_talk_node];
	if (store_talk_node.extras[3] < 0)
		store_talk_node.extras[3] = 0;
		
	cd_create_dialog_parent_num(817,parent_num);
	
	cd_activate_item(817,13,0);
	cd_activate_item(817,37,0);
	
	if ((store_talk_node.type != 29) && (store_talk_node.type != 30))
		cd_activate_item(817,38,0);
		else cd_activate_item(817,38,1);
	put_talk_node_in_dlog();
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(817,0);
}

void pick_out_event_filter (short item_hit)
{
	char temp_str[256];
	short i,j,spec;
	
	switch (item_hit) {
		case 2:
			dialog_answer = store_cur_loc.x * 100 + store_cur_loc.y;
			 dialog_not_toast = FALSE; 
			break;
		case 3:
			dialog_answer = -1;
			 dialog_not_toast = FALSE; 
			break;
		case 12:
			if (store_cur_loc.x == 0) SysBeep(20);
				else store_cur_loc.x--;
			break;
		case 13:
			if (store_cur_loc.x >= scenario.out_width - 1) SysBeep(20);
				else store_cur_loc.x++;
			break;
		case 14:
			if (store_cur_loc.y == 0) SysBeep(20);
				else store_cur_loc.y--;
			break;
		case 15:
			if (store_cur_loc.y >= scenario.out_height - 1) SysBeep(20);
				else store_cur_loc.y++;
			break;
		}
	sprintf((char *) temp_str,"X = %d",store_cur_loc.x);
	csit(854,8,(char *) temp_str);
	sprintf((char *) temp_str,"Y = %d",store_cur_loc.y);
	csit(854,11,(char *) temp_str);
}

short pick_out(location default_loc)
// ignore parent in Mac version
{
	short basic_dlog_hit,i,store_dialog_answer;
	char temp_str[256];
	
	store_cur_loc = default_loc;
	
	cd_create_dialog_parent_num(854,0);
	
	cdsin(854,7,scenario.out_width);	
	cdsin(854,10,scenario.out_height);
	sprintf((char *) temp_str,"X = %d",store_cur_loc.x);
	csit(854,8,(char *) temp_str);
	sprintf((char *) temp_str,"Y = %d",store_cur_loc.y);
	csit(854,11,(char *) temp_str);

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(854,0);
	return dialog_answer;
}

void new_town_event_filter (short item_hit)
{
	char temp_str[256];
	short i,j,spec;
	
	switch (item_hit) {
		case 3:
			dialog_answer = 1;
			 dialog_not_toast = FALSE; 
			break;
		case 23:
			dialog_answer = -1;
			 dialog_not_toast = FALSE; 
			break;
		default:
			cd_hit_led_range(830,11,13,item_hit);
			cd_hit_led_range(830,18,20,item_hit);
			break;
		}

}

Boolean new_town(short which_town)
// ignore parent in Mac version
{
	short basic_dlog_hit,i,j,store_dialog_answer;
	char temp_str[256];
	short size = 0,preset = 0;

	 
	cd_create_dialog_parent_num(830,0);
	
	cdsin(830,22,which_town);
	cd_set_led(830,12,1);
	cd_set_led(830,18,1);
	sprintf((char *) temp_str,"Town name");
	CDST(830,2,(char *) temp_str);
	
	while (dialog_not_toast)
		ModalDialog();

	size = cd_get_led_range(830,11,13);
	preset = cd_get_led_range(830,18,20);
	CDGT(830,2,(char *) temp_str);
	temp_str[30] = 0;
	cd_kill_dialog(830,0);
	if (dialog_answer < 0)
		return FALSE;
	
	scenario.num_towns++;
	scenario.town_size[which_town] = size;
	scenario.town_hidden[which_town] = 0;
	cur_town = which_town;
	scenario.last_town_edited = cur_town;
	init_town(size);
	strcpy(town_strs[0],(char *) temp_str);
	town_type = size;
	
	for (i = 0; i < max_dim[size]; i++)
		for (j = 0; j < max_dim[size]; j++)
			switch (preset) {
				case 0:
					t_d.terrain[i][j] = 0;
					break;
				case 1:
					t_d.terrain[i][j] = 2;
					break;
				case 2:
					t_d.terrain[i][j] = 2;
					if (get_ran(1,0,8) == 0)
						t_d.terrain[i][j] = 3;
						else  if (get_ran(1,0,10) == 0)
						t_d.terrain[i][j] = 4;
					break;
				}
				
	return TRUE;
}

// before calling this, be sure to do all checks to make sure it's safe.
void delete_last_town()
{
	scenario.num_towns--;
	scenario.town_size[scenario.num_towns] = 0;
	scenario.town_hidden[scenario.num_towns] = 0;
	scenario.town_data_size[scenario.num_towns][0] = 0;
	scenario.town_data_size[scenario.num_towns][1] = 0;
	scenario.town_data_size[scenario.num_towns][2] = 0;
	scenario.town_data_size[scenario.num_towns][3] = 0;
	scenario.town_data_size[scenario.num_towns][4] = 0;
	save_scenario();
}

void pick_import_town_event_filter (short item_hit)
{
	char str[256];
	short i;
	
	switch (item_hit) {
		case 3:
			dialog_answer = CDGN(store_whigh_dlog,2);
			if (dialog_answer < 0) {
				give_error("This number is out of the correct range. It must be at least 0.","",store_whigh_dlog);
				break;
				}
			 dialog_not_toast = FALSE; 
			break;
		case 8:
			dialog_answer = -1;
				 dialog_not_toast = FALSE; 
			break;

		}
}

short pick_import_town(short which_dlog,short def)
// ignore parent in Mac version
{
	short town_strs_hit,i,store_dialog_answer;
	char temp_str[256], str2[256];
	
	 store_whigh_dlog = which_dlog;
	
	cd_create_dialog_parent_num(store_whigh_dlog,0);
	
	CDSN(store_whigh_dlog,2,0);
	
	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(store_whigh_dlog,0);
	return dialog_answer;
}


