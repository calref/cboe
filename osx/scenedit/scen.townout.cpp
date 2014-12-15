#include <cstdio>
#include <cstring>
#include "scen.global.h"
#include "classes.h"
#include "graphtool.h"
#include "scen.graphics.h"
#include "scen.townout.h"
#include "scen.keydlgs.h"
#include "scen.fileio.h"
#include "mathutil.h"
#include "button.h"

extern short cen_x, cen_y, overall_mode;//,user_given_password;
extern bool mouse_button_held,editing_town;
extern short cur_viewing_mode;
extern cTown* town;
//extern big_tr_type t_d;
extern short mode_count,to_create,cur_town;
extern ter_num_t template_terrain[64][64];
extern cItemRec item_list[400];
extern cScenario scenario;
extern cSpecial null_spec_node;
extern cSpeech::cNode null_talk_node;
//extern piles_of_stuff_dumping_type *data_store;
extern cOutdoors current_terrain;
extern ter_num_t borders[4][50];
extern location cur_out;
//extern cSpeech talking;

cCreature store_placed_monst,store_placed_monst2;
short store_which_placed_monst;
cTown::cItem store_placed_item;
short store_which_placed_item,store_which_sign;
short str_do_delete[16];
short a,b,c;
short store_which_out_wand,store_out_wand_mode;
cOutdoors::cWandering store_out_wand;

const char *day_str_1[] = {"Unused","Day creature appears","Day creature disappears","",
	"Unused","Unused","Unused","Unused","Unused"};
const char *day_str_2[] = {"Unused","Event code (0 - no event)","Event code (0 - no event)","",
	"Unused","Unused","Unused",
	"Event code (0 - no event)","Event code (0 - no event)"};

short store_which_talk_node,last_talk_node[60];
cSpeech::cNode store_talk_node;
location store_cur_loc;

void init_town(short size) {
	short i,j;
	std::string temp_str;
	if(size == 0) town = new cBigTown;
	else if(size == 1) town = new cMedTown;
	else if(size == 2) town = new cTinyTown;
	else return;
	for(i = 0; i < 180; i++) {
		temp_str = get_str("town-default",i + 1);
		sprintf(town->town_strs(i), "%s", temp_str.c_str());
		town->strlens[i] = strlen((char *) town->town_strs(i));
	}
	for(i = 0; i < town->max_dim(); i++)
		for(j = 0; j < town->max_dim(); j++) {
			town->terrain(i,j) = scenario.default_ground * 2;
			town->lighting(i / 8,j) = 0;
		}
	
	for(i = 0; i < 200; i++)
		town->talking.strlens[i] = 0;
	for(i = 0; i < 170; i++)
		if(i < 10)
			strcpy(town->talking.talk_strs[i],"Unused");
		else strcpy(town->talking.talk_strs[i],"");
	
	for(i = 0; i < 60; i++) {
		town->talking.talk_nodes[i].personality = -1;
		town->talking.talk_nodes[i].type = 0;
		town->talking.talk_nodes[i].extras[0] = 0;
		town->talking.talk_nodes[i].extras[1] = 0;
		town->talking.talk_nodes[i].extras[2] = 0;
		town->talking.talk_nodes[i].extras[3] = -1;
		for(j = 0; j < 4; j++) {
			town->talking.talk_nodes[i].link1[j] = 'x';
			town->talking.talk_nodes[i].link2[j] = 'x';
		}
	}
}

void init_out() {
	short i,j;
	std::string temp_str;
	
	for(i = 0; i < 4; i++)
		for(j = 0; j < 50; j++)
			borders[i][j] = 90;
	
	for(i = 0; i < 120; i++) {
		temp_str = get_str("outdoor-default",i + 1);
		sprintf(current_terrain.out_strs(i), "%s", temp_str.c_str());
		current_terrain.strlens[i] = strlen((char *) current_terrain.out_strs(i));
	}
}

static void put_placed_monst_in_dlog(cDialog& me) {
	me["num"].setTextToNum(store_which_placed_monst);
	me["type"].setText(scenario.scen_monsters[store_placed_monst.number].m_name);
	// TODO: Make attitude an enum
	dynamic_cast<cLedGroup&>(me["attitude"]).setSelected("att" + std::to_string(store_placed_monst.start_attitude + 1));
	dynamic_cast<cLedGroup&>(me["mobility"]).setSelected("mob" + std::to_string(store_placed_monst.mobility + 1));
	me["talk"].setTextToNum(store_placed_monst.personality);
	me["picnum"].setTextToNum(store_placed_monst.facial_pic);
	// TODO: Use -1 instead of 0 for "no pic", since 0 is a valid talking picture
 	if((store_placed_monst.facial_pic < 1))
		dynamic_cast<cPict&>(me["pic"]).setPict(-1);
	else if((store_placed_monst.facial_pic >= 1000))
		dynamic_cast<cPict&>(me["pic"]).setPict(store_placed_monst.facial_pic - 1000,PIC_CUSTOM_TALK);
	else dynamic_cast<cPict&>(me["pic"]).setPict(store_placed_monst.facial_pic,PIC_TALK);
}

static bool get_placed_monst_in_dlog(cDialog& me) {
	store_placed_monst.start_attitude = dynamic_cast<cLedGroup&>(me["attitude"]).getSelected()[3] - '1';
	store_placed_monst.mobility = dynamic_cast<cLedGroup&>(me["mobility"]).getSelected()[3] - '1';
	store_placed_monst.personality = me["talk"].getTextAsNum();
	store_placed_monst.facial_pic = me["picnum"].getTextAsNum();
	// later
	town->creatures(store_which_placed_monst) = store_placed_monst;
	return true;
}

static bool edit_placed_monst_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	short i;
	cCreature store_m;
	
	if(item_hit == "okay") {
		if(!get_placed_monst_in_dlog(me)) return true;
		me.toast(true);
	} else if(item_hit == "cancel") {
		me.toast(false);
	} else if(item_hit == "type-edit") {
		if(!get_placed_monst_in_dlog(me)) return true;
		i = choose_text(STRT_MONST,store_placed_monst.number,&me,"Choose Which Monster:");
		if(i >= 0) {
			store_placed_monst.number = i;
			put_placed_monst_in_dlog(me);
		}
	} else if(item_hit == "pict-edit") {
		if(!get_placed_monst_in_dlog(me)) return true;
		i = choose_graphic(store_placed_monst.facial_pic,PIC_TALK,&me);
		if(i >= 0) {
			// TODO: Use -1 instead of 0 for "no pic"
			store_placed_monst.facial_pic = i + 1;
		}
		else return true;
		put_placed_monst_in_dlog(me);
	} else if(item_hit == "more") { //advanced
		store_m = edit_placed_monst_adv(store_placed_monst, me);
		if(store_m.number != 0)
			store_placed_monst = store_m;
	}
	return true;
}

void edit_placed_monst(short which_m) {
	store_placed_monst = town->creatures(which_m);
	store_which_placed_monst = which_m;
	
	cDialog edit("edit-townperson.xml");
	edit.attachClickHandlers(edit_placed_monst_event_filter, {"type-edit", "pict-edit", "okay", "cancel", "more"});
	
	put_placed_monst_in_dlog(edit);
	
	edit.run();
}

static void put_placed_monst_adv_in_dlog(cDialog& me) {
	
	me["num"].setTextToNum(store_which_placed_monst);
	me["type"].setText(scenario.scen_monsters[store_placed_monst2.number].m_name);
	dynamic_cast<cLedGroup&>(me["time"]).setSelected("time" + std::to_string(store_placed_monst2.time_flag + 1));
	me["extra1-lbl"].setText(day_str_1[store_placed_monst2.time_flag]);
	me["extra2-lbl"].setText(day_str_2[store_placed_monst2.time_flag]);
	me["extra1"].setTextToNum(store_placed_monst2.monster_time);
	me["extra2"].setTextToNum(store_placed_monst2.time_code);
	// TODO: Why on earth is this an LED group? Just use a text field!
	dynamic_cast<cLedGroup&>(me["group"]).setSelected("group" + std::to_string(store_placed_monst2.spec_enc_code));
	me["death"].setTextToNum(store_placed_monst2.special_on_kill);
	me["sdfx"].setTextToNum(store_placed_monst2.spec1);
	me["sdfy"].setTextToNum(store_placed_monst2.spec2);
}

static bool get_placed_monst_adv_in_dlog(cDialog& me) {
	store_placed_monst2.time_flag = dynamic_cast<cLedGroup&>(me["time"]).getSelected()[4] - '1';
	store_placed_monst2.monster_time = me["extra1"].getTextAsNum();
  	if(cre(store_placed_monst2.monster_time,0,1000,"Given day must be from 0 to 1000.","",&me) > 0) return false;
	store_placed_monst2.time_code = me["extra2"].getTextAsNum();
  	if(cre(store_placed_monst2.time_code,0,10,"Event code must be 0 (for no event) or from 1 to 10.","",&me) > 0) return false;
	store_placed_monst2.special_on_kill = me["death"].getTextAsNum();
  	if(cre(store_placed_monst2.special_on_kill,-1,99,"Town special node number must be from 0 to 99 (or -1 for no special).","",&me) > 0) return false;
	store_placed_monst2.spec1 = me["sdfx"].getTextAsNum();
  	if(cre(store_placed_monst2.spec1,-1,299,"First part of special flag must be -1 (if this is to be ignored) or from 0 to 299.","",&me) > 0) return false;
	store_placed_monst2.spec2 = me["sdfy"].getTextAsNum();
  	if(cre(store_placed_monst2.spec2,-1,9,"Second part of special flag must be -1 (if this is to be ignored) or from 0 to 9.","",&me) > 0) return false;
	
	std::string group = dynamic_cast<cLedGroup&>(me["group"]).getSelected();
	if(group == "group10") store_placed_monst2.spec_enc_code = 10;
	else store_placed_monst2.spec_enc_code = group[5] - '0';
	return true;
}

static bool edit_placed_monst_adv_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	if(item_hit == "okay") {
		if(!get_placed_monst_adv_in_dlog(me)) return true;
		me.toast(true);
	} else if(item_hit == "cancel") {
		store_placed_monst2.number = 0;
		me.toast(false);
	}
	return true;
}

static bool edit_placed_monst_adv_time_flag(cDialog& me, std::string id, bool losingFocus) {
	if(losingFocus) return true;
	
	if(id.substr(0,4) == "time") {
		int item_hit = id[4] - '1';
		me["extra1-lbl"].setText(day_str_1[item_hit]);
		me["extra2-lbl"].setText(day_str_2[item_hit]);
	}
	return true;
}

cCreature edit_placed_monst_adv(cCreature monst_record, cDialog& parent) {
	store_placed_monst2 = monst_record;
	
	cDialog edit("edit-townperson-advanced.xml", &parent);
	edit["okay"].attachClickHandler(edit_placed_monst_adv_event_filter);
	edit["cancel"].attachClickHandler(edit_placed_monst_adv_event_filter);
	edit["time"].attachFocusHandler(edit_placed_monst_adv_time_flag);
	
	put_placed_monst_adv_in_dlog(edit);
	
	edit.run();
	
	return store_placed_monst2;
}

void put_placed_item_in_dlog() {
	char str[256];
	short i;
#if 0
	cdsin(836,17,store_which_placed_item);
	sprintf((char *) str,"X = %d, Y = %d",store_placed_item.loc.x,store_placed_item.loc.y);
	csit(836,22,(char *) str);
	csit(836,15,scenario.scen_items[store_placed_item.code].full_name.c_str());
	CDSN(836,2,store_placed_item.ability);
	if(store_placed_item.always_there)
		cd_set_led(836,12,1);
	else cd_set_led(836,12,0);
	if(store_placed_item.property)
		cd_set_led(836,13,1);
	else cd_set_led(836,13,0);
	if(store_placed_item.contained)
		cd_set_led(836,14,1);
	else cd_set_led(836,14,0);
	
	i = scenario.scen_items[store_placed_item.code].graphic_num;
	if(i >= 1000) // was 1000, then was 150
	 	csp(836,4,i % 1000,PICT_CUSTOM + PICT_ITEM);
	else csp(836,4,/*1800 + */i,PICT_ITEM);
#endif
}

bool get_placed_item_in_dlog() {
	short i;
#if 0
	store_placed_item.ability = CDGN(836,2);
	if((store_placed_item.ability < -1) || (store_placed_item.ability > 2500)) {
		give_error("Number of charges/amount of gold or food must be from 0 to 2500."
				   ,"If an item with charges (not gold or food) leave this at -1 for the item to have the default number of charges.",836);
		return false;
	}
	i = scenario.scen_items[store_placed_item.code].variety;
	if((store_placed_item.ability == 0) && ((i == 3) || (i == 11))) {
		give_error("You must assign gold or food an amount of at least 1.","",836);
		return false;
	}
	store_placed_item.always_there = cd_get_led(836,12);
	store_placed_item.property = cd_get_led(836,13);
	store_placed_item.contained = cd_get_led(836,14);
	
	town->preset_items[store_which_placed_item] = store_placed_item;
#endif
	return true;
}

void edit_placed_item_event_filter (short item_hit) {
	short i;
	cCreature store_m;
#if 0
	switch(item_hit) {
		case 3:
			if(!get_placed_item_in_dlog())
				break;
			toast_dialog();
			break;
		case 20:
			toast_dialog();
			break;
		case 18:
			i = choose_text_res(-2,0,399,store_placed_item.code,836,"Place which item?");
			if(i >= 0) {
				store_placed_item.code = i;
				put_placed_item_in_dlog();
			}
			break;
		default:
			for(i = 12; i <= 14; i++)
				cd_flip_led(836,i,item_hit);
			break;
	}
#endif
}

void edit_placed_item(short which_i) {
#if 0
	short item_hit;
	
	store_placed_item = town->preset_items[which_i];
	store_which_placed_item = which_i;
	
	cd_create_dialog_parent_num(836,0);
	
	cd_activate_item(836,19,0);
	put_placed_item_in_dlog();
	
	item_hit = cd_run_dialog();
	
	cd_kill_dialog(836);
#endif
}

void edit_sign_event_filter (short item_hit) {
#if 0
	switch(item_hit) {
		case 9: case 4:
			if(!editing_town)
				CDGT(831,2,current_terrain.out_strs(100 + store_which_sign));
			else CDGT(831,2,town->town_strs(120 + store_which_sign));
			if(item_hit == 9)
				toast_dialog();
			else {
				if(item_hit == 3)
					store_which_sign--;
				else store_which_sign++;
				if(store_which_sign < 0)
					store_which_sign = (editing_town) ? 14 : 7;
				if(store_which_sign > (editing_town) ? 14 : 7)
					store_which_sign = 0;
			}
			break;
		case 3:
			toast_dialog();
			break;
	}
	cdsin(831,5,store_which_sign);
	if(!editing_town)
		CDST(831,2,current_terrain.out_strs(100 + store_which_sign));
	else CDST(831,2,town->town_strs(120 + store_which_sign));
#endif
}

void edit_sign(short which_sign,short picture) {
#if 0
	short item_hit;
	location view_loc;
	
	store_which_sign = which_sign;
	
	cd_create_dialog_parent_num(831,0);
	
	if(picture >= 1000)
		csp(831,6,picture % 1000,PICT_CUSTOM + PICT_TER);
	else if(picture >= 400)
		csp(831,6,picture - 400,PICT_TER_ANIM);
	else csp(831,6,picture,PICT_TER);
	
	cdsin(831,5,store_which_sign);
	if(!editing_town)
		CDST(831,2,current_terrain.out_strs(100 + store_which_sign));
	else CDST(831,2,town->town_strs(120 + store_which_sign));
	//cd_activate_item(831,3,0);
	//cd_activate_item(831,4,0);
	
	item_hit = cd_run_dialog();
	
	cd_kill_dialog(831);
#endif
}

bool save_out_strs() {
#if 0
	char str[256];
	short i;
	
	for(i = 0; i < 8; i++) {
		CDGT(850,2 + i,(char *) str);
		sprintf((char *)current_terrain.out_strs(i + 1),"%-29.29s",str);
		if(str_do_delete[i] > 0)
			current_terrain.info_rect[i].right = 0;
	}
#endif
	return true;
}

void put_out_strs_in_dlog() {
	char str[256];
	short i;
#if 0
	for(i = 0; i < 8; i++) {
		if((current_terrain.info_rect[i].right == 0) || (str_do_delete[i] > 0)) {
			sprintf((char *) str,"Not yet placed.");
			cd_activate_item(850,25 + i,0);
		}
		else sprintf((char *) str,"X = %d, Y = %d",current_terrain.info_rect[i].left,
					 current_terrain.info_rect[i].top);
		csit(850,13 + i,(char *) str);
		CDST(850,2 + i,current_terrain.out_strs(i + 1));
	}
#endif
}

void edit_out_strs_event_filter (short item_hit) {
#if 0
	switch(item_hit) {
		case 10:
			if(save_out_strs())
				toast_dialog();
			break;
		case 11:
			toast_dialog();
			break;
		default:
			if((item_hit >= 25) && (item_hit <= 32)) {
				//sprintf((char *)data_store->out_strs[item_hit - 25 + 1],"");
				CDST(850,2 + item_hit - 25,"");
				str_do_delete[item_hit - 25] = 1;
				put_out_strs_in_dlog();
			}
			break;
	}
#endif
}

void edit_out_strs() {
#if 0
	// ignore parent in Mac version
	short out_strs_hit,i;
	
	for(i = 0; i < 8; i++)
		str_do_delete[i] = 0;
	
	cd_create_dialog_parent_num(850,0);
	
	put_out_strs_in_dlog();
	
	out_strs_hit = cd_run_dialog();
	
	cd_kill_dialog(850);
#endif
}


bool save_town_strs() {
	char str[256];
	short i;
#if 0
	for(i = 0; i < 16; i++) {
		CDGT(839,2 + i,(char *) str);
		sprintf((char *)town->town_strs(i + 1),"%-29.29s",str);
		if(str_do_delete[i] > 0)
			town->room_rect(i).right = 0;
	}
#endif
	return true;
}

void put_town_strs_in_dlog() {
	char str[256];
	short i;
#if 0
	for(i = 0; i < 16; i++) {
		if((town->room_rect(i).right == 0) || (str_do_delete[i] > 0)) {
			sprintf((char *) str,"Not yet placed.");
			cd_activate_item(839,41 + i,0);
		}
		else sprintf((char *) str,"X = %d, Y = %d",town->room_rect(i).left,
					 town->room_rect(i).top);
		csit(839,21 + i,(char *) str);
		CDST(839,2 + i,town->town_strs(i + 1));
	}
#endif
}

void edit_town_strs_event_filter (short item_hit) {
#if 0
	switch(item_hit) {
		case 18:
			if(save_town_strs())
				toast_dialog();
			break;
		case 19:
			toast_dialog();
			break;
		default:
			if((item_hit >= 41) && (item_hit <= 56)) {
				//sprintf((char *)data_store->town_strs[item_hit - 41 + 1],"");
				CDST(839,2 + item_hit - 41,"");
				str_do_delete[item_hit - 41] = 1;
				put_town_strs_in_dlog();
			}
			break;
	}
#endif
}

void edit_town_strs() {
	// ignore parent in Mac version
	short town_strs_hit,i;
#if 0
	for(i = 0; i < 16; i++)
		str_do_delete[i] = 0;
	
	cd_create_dialog_parent_num(839,0);
	
	put_town_strs_in_dlog();
	
	town_strs_hit = cd_run_dialog();
	
	cd_kill_dialog(839);
#endif
}


short store_which_town_dlg;
void pick_town_num_event_filter (short item_hit) {
#if 0
	switch(item_hit) {
		case 3:
			dialog_answer = CDGN(store_which_town_dlg,2);
			if((dialog_answer < 0) || (dialog_answer >= scenario.num_towns)) {
				give_error("This number is out of the correct range. (0 to the number of towns minus 1)","",store_which_town_dlg);
				break;
			}
			toast_dialog();
			break;
		case 4:
			dialog_answer = -1;
			toast_dialog();
			break;
			
	}
#endif
}

short pick_town_num(short which_dlog,short def) {
#if 0
	// ignore parent in Mac version
	short town_strs_hit;
	char temp_str[256],str2[256];
	
	store_which_town_dlg = which_dlog;
	
	cd_create_dialog_parent_num(store_which_town_dlg,0);
	
	CDSN(store_which_town_dlg,2,def);
	cd_get_item_text(store_which_town_dlg,7,(char *) temp_str);
	sprintf((char *) str2,"%s (0 - %d)",(char *) temp_str,scenario.num_towns - 1);
	csit(store_which_town_dlg,7,(char *) str2);
	
	town_strs_hit = cd_run_dialog();
	
	cd_kill_dialog(store_which_town_dlg);
#endif
	return 0;//dialog_answer;
}

void change_ter_event_filter (short item_hit) {
	short i;
#if 0
	switch(item_hit) {
		case 5:
			a = CDGN(857,2);
			b = CDGN(857,3);
			c = CDGN(857,4);
			if(cre(a,0,255,"Both terrain types must be from 0 to 255.","",857)) break;
			if(cre(b,0,255,"Both terrain types must be from 0 to 255.","",857)) break;
			if(cre(c,0,100,"The Chance must be from 0 to 100.","",857)) break;
			
			toast_dialog();
			break;
		case 6:
			a = -1;
			b = -1;
			c = -1;
			toast_dialog();
			break;
		case 10: case 11:
			i = CDGN(857,item_hit - 8);
			i = choose_text_res(-4,0,255,i,857,"Which Terrain?");
			if(i >= 0)
				CDSN(857,item_hit - 8,i);
			break;
	}
#endif
}

void change_ter(short *change_from,short *change_to,short *chance) {
	// ignore parent in Mac version
	short town_strs_hit;
#if 0
	cd_create_dialog_parent_num(857,0);
	
	CDSN(857,2,0);
	CDSN(857,3,0);
	CDSN(857,4,100);
	
	town_strs_hit = cd_run_dialog();
	
	*change_from = a;
	*change_to = b;
	*chance = c;
	
	cd_kill_dialog(857);
#endif
}

void outdoor_details_event_filter (short item_hit) {
#if 0
	char str[256];
	
	switch(item_hit) {
		case 3:
			CDGT(851,2,(char *) str);
			str[29] = 0;
			sprintf(current_terrain.out_strs(0),"%s",(char *) str);
			toast_dialog();
			break;
			
	}
#endif
}

void outdoor_details() {
#if 0
	// ignore parent in Mac version
	short town_strs_hit;
	char temp_str[256];
	
	
	cd_create_dialog_parent_num(851,0);
	
	CDST(851,2,current_terrain.out_strs(0));
	sprintf((char *) temp_str,"X = %d, Y = %d",cur_out.x,cur_out.y);
	csit(851,8,(char *) temp_str);
	
	town_strs_hit = cd_run_dialog();
	
	cd_kill_dialog(851);
#endif
}

void put_out_wand_in_dlog() {
#if 0
	char str[256];
	short i;
	
	cdsin(852,45,store_which_out_wand);
	for(i = 0; i < 7; i++)
		if(store_out_wand.monst[i] == 0)
			csit(852, i + 7, "Empty");
		else {
			strcpy((char*)str,(char*)scenario.scen_monsters[store_out_wand.monst[i]].m_name.c_str());
			csit(852,i + 7,(char *) str);
		}
	for(i = 0; i < 3; i++)
		if(store_out_wand.friendly[i] == 0)
			csit(852, i + 7 + 7, "Empty");
		else {
			strcpy((char*)str,(char*)scenario.scen_monsters[store_out_wand.friendly[i]].m_name.c_str());
			csit(852,i + 7 + 7,(char *) str);
		}
	if(store_out_wand.cant_flee % 10 == 1)
		cd_set_led(852,51,1);
	else cd_set_led(852,51,0);
	if(store_out_wand.cant_flee >= 10)
		cd_set_led(852,53,1);
	else cd_set_led(852,53,0);
	CDSN(852,2,store_out_wand.spec_on_meet);
	CDSN(852,3,store_out_wand.spec_on_win);
	CDSN(852,4,store_out_wand.spec_on_flee);
	CDSN(852,5,store_out_wand.end_spec1);
	CDSN(852,6,store_out_wand.end_spec2);
#endif
}

bool get_out_wand_in_dlog() {
#if 0
	store_out_wand.spec_on_meet = CDGN(852,2);
	store_out_wand.spec_on_win = CDGN(852,3);
	store_out_wand.spec_on_flee = CDGN(852,4);
	store_out_wand.end_spec1 = CDGN(852,5);
	store_out_wand.end_spec2 = CDGN(852,6);
	if(cre(store_out_wand.end_spec1,
			-1,299,"First part of Stuff Done flag must be from 0 to 299 (or -1 if it's not used).","",852)) return false;
	if(cre(store_out_wand.end_spec2,
			-1,9,"Second part of Stuff Done flag must be from 0 to 9 (or -1 if it's not used).","",852)) return false;
	if(cre(store_out_wand.spec_on_meet,
			-1,59,"Outdoor Special Nodes run from 0 to 59 (or -1 if not used).","",852)) return false;
	if(cre(store_out_wand.spec_on_win,
			-1,59,"Outdoor Special Nodes run from 0 to 59 (or -1 if not used).","",852)) return false;
	if(cre(store_out_wand.spec_on_flee,
			-1,59,"Outdoor Special Nodes run from 0 to 59 (or -1 if not used).","",852)) return false;
	
	store_out_wand.cant_flee = 0;
	if(cd_get_led(852,53) > 0)
		store_out_wand.cant_flee += 10;
	if(cd_get_led(852,51) > 0)
		store_out_wand.cant_flee += 1;
	
	if(store_out_wand_mode == 0)
		current_terrain.wandering[store_which_out_wand] = store_out_wand;
	else current_terrain.special_enc[store_which_out_wand] = store_out_wand;
#endif
	return true;
}

void edit_out_wand_event_filter (short item_hit) {
	short i,spec;
	cCreature store_m;
#if 0
	switch(item_hit) {
		case 17:
			if(!get_out_wand_in_dlog())
				break;
			toast_dialog();
			break;
		case 18:
			toast_dialog();
			break;
		case 19:
			if(!get_out_wand_in_dlog()) break;
			store_which_out_wand--;
			if(store_which_out_wand < 0) store_which_out_wand = 3;
			store_out_wand = (store_out_wand_mode == 0) ?
				current_terrain.wandering[store_which_out_wand] : current_terrain.special_enc[store_which_out_wand];
			put_out_wand_in_dlog();
			break;
		case 20:
			if(!get_out_wand_in_dlog()) break;
			store_which_out_wand++;
			if(store_which_out_wand > 3) store_which_out_wand = 0;
			store_out_wand = (store_out_wand_mode == 0) ?
				current_terrain.wandering[store_which_out_wand] : current_terrain.special_enc[store_which_out_wand];
			put_out_wand_in_dlog();
			break;
		case 31: case 32: case 33:
			if(!get_out_wand_in_dlog())
				break;
			spec = CDGN(852,item_hit - 29);
			if((spec < 0) || (spec >= 60)) {
				spec = get_fresh_spec(1);
				if(spec < 0) {
					give_error("You can't create a new special encounter because there are no more free special nodes.",
							   "To free a special node, set its type to No Special and set its Jump To special to -1.",852);
					break;
				}
				CDSN(852,item_hit - 29,spec);
			}
			edit_spec_enc(spec,1,852);
			if((spec >= 0) && (spec < 60) && (current_terrain.specials[spec].pic < 0))
				CDSN(852,item_hit - 29,-1);
			if(!get_out_wand_in_dlog())
				break;
			break;
		default:
			if(!get_out_wand_in_dlog())
				break;
			cd_flip_led(852,51,item_hit);
			cd_flip_led(852,53,item_hit);
			if((item_hit >= 21) && (item_hit <= 27)) {
				i = choose_text_res(-1,0,255,store_out_wand.monst[item_hit - 21],852,"Choose Which Monster:");
				if(i >= 0) store_out_wand.monst[item_hit - 21] = i;
				put_out_wand_in_dlog();
			}
			if((item_hit >= 28) && (item_hit <= 30)) {
				i = choose_text_res(-1,0,255,store_out_wand.friendly[item_hit - 28],852,"Choose Which Monster:");
				if(i >= 0) store_out_wand.friendly[item_hit - 28] = i;
				put_out_wand_in_dlog();
			}
			break;
	}
#endif
}

// mode 0 - wandering 1 - special
void edit_out_wand(short mode) {
#if 0
	short item_hit;
	
	store_which_out_wand = 0;
	store_out_wand_mode = mode;
	store_out_wand = (store_out_wand_mode == 0) ? current_terrain.wandering[0] : current_terrain.special_enc[0];
	
	cd_create_dialog_parent_num(852,0);
	
	if(mode == 1)
		csit(852,47,"Outdoor Special Encounter:");
	
	put_out_wand_in_dlog();
	
	item_hit = cd_run_dialog();
	
	cd_kill_dialog(852);
#endif
}

bool save_town_details() {
#if 0
	char str[256];
	
	CDGT(832,2,(char *) str);
	sprintf(town->town_strs(0),"%-29.29s",str);
	town->town_chop_time = CDGN(832,3);
	if(cre(town->town_chop_time,-1,10000,"The day the town becomes abandoned must be from 0 to 10000 (or -1 if it doesn't)."
			,"",832)) return false;
	town->town_chop_key = CDGN(832,4);
	if(cre(town->town_chop_key,-1,10,"The event which prevents the town from becoming abandoned must be from 1 to 10 (-1 or 0 for none)."
			,"",832)) return false;
	town->lighting_type = cd_get_led_range(832,19,22);
	town->max_num_monst = CDGN(832,5);
	town->difficulty = CDGN(832,6);
	if(cre(town->difficulty,0,10,"The town difficulty must be between 0 (easiest) and 10 (hardest)."
			,"",832)) return false;
#endif
	return true;
}

void put_town_details_in_dlog() {
#if 0
	CDST(832,2,town->town_strs(0));
	CDSN(832,3,town->town_chop_time);
	CDSN(832,4,town->town_chop_key);
	cd_set_led_range(832,19,22,town->lighting_type);
	CDSN(832,5,town->max_num_monst);
	CDSN(832,6,town->difficulty);
#endif
}

void edit_town_details_event_filter (short item_hit) {
#if 0
	switch(item_hit) {
		case 7:
			if(save_town_details())
				toast_dialog();
			break;
		default:
			cd_hit_led_range(832,19,22,item_hit);
			break;
	}
#endif
}

void edit_town_details() {
	// ignore parent in Mac version
	short town_details_hit;
#if 0
	cd_create_dialog_parent_num(832,0);
	
	put_town_details_in_dlog();
	
	town_details_hit = cd_run_dialog();
	cd_kill_dialog(832);
#endif
}

bool save_town_events() {
	short i;
#if 0
	for(i = 0; i < 8; i++) {
		town->timer_spec_times[i] = CDGN(833,2 + i);
		town->timer_specs[i] = CDGN(833,10 + i);
		if(cre(town->timer_specs[i],-1,99,"The town special nodes must be between 0 at 99 (or -1 for no special)."
				,"",833)) return false;
	}
#endif
	return true;
}

void put_town_events_in_dlog() {
	short i;
#if 0
	for(i = 0; i < 8; i++) {
		CDSN(833,2 + i,town->timer_spec_times[i]);
		CDSN(833,10 + i,town->timer_specs[i]);
	}
#endif
}

void edit_town_events_event_filter (short item_hit) {
	short spec;
#if 0
	switch(item_hit) {
		case 18:
			if(save_town_events())
				toast_dialog();
			break;
		default:
			if((item_hit >= 26) && (item_hit <= 33)) {
				if(!save_town_events())
					break;
				spec = CDGN(833,item_hit - 26 + 10);
				if((spec < 0) || (spec > 99)) {
					spec = get_fresh_spec(2);
					if(spec < 0) {
						give_error("You can't create a new special encounter because there are no more free special nodes.",
								   "To free a special node, set its type to No Special and set its Jump To special to -1.",833);
						break;
					}
					CDSN(833,item_hit - 26 + 10,spec);
				}
				edit_spec_enc(spec,2,833);
				if((spec >= 0) && (spec < 100) && (town->specials[spec].pic < 0))
					CDSN(833,item_hit - 26 + 10,-1);
			}
			break;
	}
#endif
}

void edit_town_events() {
	// ignore parent in Mac version
	short advanced_town_hit;
#if 0
	
	cd_create_dialog_parent_num(833,0);
	
	put_town_events_in_dlog();
	
	advanced_town_hit = cd_run_dialog();
	
	cd_kill_dialog(833);
#endif
}

bool save_advanced_town() {
	short i;
#if 0
	for(i = 0; i < 4; i++) {
		town->exit_specs[i] = CDGN(834,2 + i);
		if(cre(town->exit_specs[i],-1,99,"The town exit specials must be between 0 at 99 (or -1 for no special)."
				,"",834)) return false;
		town->exit_locs[i].x = CDGN(834,8 + i * 2);
		if(cre(town->exit_locs[i].x,-1,47,"The town exit coordinates must be from 0 to 47 (or -1 if you want them ignored)."
				,"",834)) return false;
		town->exit_locs[i].y = CDGN(834,9 + i * 2);
		if(cre(town->exit_locs[i].y,-1,47,"The town exit coordinates must be from 0 to 47 (or -1 if you want them ignored)."
				,"",834)) return false;
	}
	town->spec_on_entry = CDGN(834,6);
	town->spec_on_entry_if_dead = CDGN(834,7);
	if(cre(town->spec_on_entry,-1,99,"The town entry specials must be from 0 to 99 (or -1 for no special)."
			,"",834)) return false;
	if(cre(town->spec_on_entry_if_dead,-1,99,"The town entry specials must be from 0 to 99 (or -1 for no special)."
			,"",834)) return false;
	scenario.town_hidden[cur_town] = cd_get_led(834,37);
#endif
	return true;
}

void put_advanced_town_in_dlog() {
	short i;
#if 0
	for(i = 0; i < 4; i++) {
		CDSN(834,2 + i,town->exit_specs[i]);
		CDSN(834,8 + i * 2,town->exit_locs[i].x);
		CDSN(834,9 + i * 2,town->exit_locs[i].y);
	}
	CDSN(834,6,town->spec_on_entry);
	CDSN(834,7,town->spec_on_entry_if_dead);
	cd_set_led(834,37,scenario.town_hidden[cur_town]);
#endif
}

void edit_advanced_town_event_filter (short item_hit) {
#if 0
	switch(item_hit) {
		case 16:
			if(save_advanced_town())
				toast_dialog();
			break;
		case 17:
			toast_dialog();
			break;
		case 37:
			cd_flip_led(834,37,item_hit);
			break;
	}
#endif
}

void edit_advanced_town() {
	// ignore parent in Mac version
	short advanced_town_hit;
#if 0
	
	cd_create_dialog_parent_num(834,0);
	
	put_advanced_town_in_dlog();
	
	advanced_town_hit = cd_run_dialog();
	
	cd_kill_dialog(834);
#endif
}

bool save_town_wand() {
	short i,j;
#if 0
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
		{
			town->wandering[i].monst[j] = CDGN(835,2 + i * 4 + j);
			if(cre(town->wandering[i].monst[j],0,255,"Wandering monsters must all be from 0 to 255 (0 means no monster)."
					,"",835)) return false;
		}
#endif
	return true;
}

void put_town_wand_in_dlog() {
	short i,j;
#if 0
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
		{
			CDSN(835,2 + i * 4 + j,town->wandering[i].monst[j]);
		}
#endif
}

void edit_town_wand_event_filter (short item_hit) {
	short i,j;
#if 0
	switch(item_hit) {
		case 18:
			if(save_town_wand())
				toast_dialog();
			break;
		case 27: case 28: case 29: case 30:
			for(i = 0; i < 4; i++) {
				j = choose_text_res(-1,0,255,town->wandering[item_hit - 27].monst[i],835,"Choose Which Monster:");
				if(j < 0)
					i = 5;
				else {
					CDSN(835,2 + (item_hit - 27) * 4 + i,j);
				}
			}
			break;
	}
#endif
}

void edit_town_wand() {
	// ignore parent in Mac version
	short town_wand_hit;
#if 0
	
	cd_create_dialog_parent_num(835,0);
	
	put_town_wand_in_dlog();
	
	town_wand_hit = cd_run_dialog();
	
	cd_kill_dialog(835);
#endif
}

bool save_basic_dlog() {
#if 0
	CDGT(821,2,town->talking.talk_strs[store_which_talk_node]);
	town->talking.talk_strs[store_which_talk_node][30] = 0;
	CDGT(821,3,town->talking.talk_strs[160 + store_which_talk_node]);
	CDGT(821,4,town->talking.talk_strs[10 + store_which_talk_node]);
	CDGT(821,5,town->talking.talk_strs[20 + store_which_talk_node]);
	CDGT(821,6,town->talking.talk_strs[30 + store_which_talk_node]);
#endif
	return true;
}

void put_basic_dlog_in_dlog() {
#if 0
	cdsin(821,14,store_which_talk_node + cur_town * 10);
	CDST(821,2,town->talking.talk_strs[store_which_talk_node]);
	CDST(821,3,town->talking.talk_strs[160 + store_which_talk_node]);
	CDST(821,4,town->talking.talk_strs[10 + store_which_talk_node]);
	CDST(821,5,town->talking.talk_strs[20 + store_which_talk_node]);
	CDST(821,6,town->talking.talk_strs[30 + store_which_talk_node]);
#endif
}

void edit_basic_dlog_event_filter (short item_hit) {
#if 0
	switch(item_hit) {
		case 7:
			if(save_basic_dlog())
				toast_dialog();
			break;
		case 8:
			toast_dialog();
			break;
		case 9:
			if(!save_basic_dlog()) break;
			store_which_talk_node--;
			if(store_which_talk_node < 0) store_which_talk_node = 9;
			put_basic_dlog_in_dlog();
			break;
		case 10:
			if(!save_basic_dlog()) break;
			store_which_talk_node++;
			if(store_which_talk_node > 9) store_which_talk_node = 0;
			put_basic_dlog_in_dlog();
			break;
	}
#endif
}

void edit_basic_dlog(short which_node) {
	// ignore parent in Mac version
	short basic_dlog_hit;
#if 0
	store_which_talk_node = which_node;
	
	cd_create_dialog_parent_num(821,0);
	
	cd_attach_key(821,10,0);
	cd_attach_key(821,9,0);
	
	put_basic_dlog_in_dlog();
	
	basic_dlog_hit = cd_run_dialog();
	cd_kill_dialog(821);
#endif
}

bool save_talk_node() {
	char str[256];
	short i;
#if 0
	
	store_talk_node.personality = CDGN(817,2);
	if((store_talk_node.personality >= 0) &&
		((store_talk_node.personality < cur_town * 10) || (store_talk_node.personality >= (cur_town + 1) * 10))) {
		sprintf((char *) str,"The legal range for personalities in this town is from %d to %d.",
				cur_town * 10,cur_town * 10 + 9);
		give_error("Personalities in talk nodes must be -1 (for unused node), -2 (all personalities use) or in the legal range of personalities in this town.",
				   (char *) str,817);
		return false;
	}
	CDGT(817,3,(char *) str);
	for(i = 0; i < 4; i++) {
		store_talk_node.link1[i] = str[i];
		if((str[i] < 97) || (str[i] > 122)) {
			give_error("The words this node is the response to must be at least 4 characters long, and all characters must be lower case letters.",
					   "",817);
			return false;
		}
	}
	CDGT(817,4,(char *) str);
	for(i = 0; i < 4; i++) {
		store_talk_node.link2[i] = str[i];
		if((str[i] < 97) || (str[i] > 122)) {
			give_error("The words this node is the response to must be at least 4 characters long, and all characters must be lower case letters.",
					   "",817);
			return false;
		}
	}
	for(i = 0; i < 4; i++)
		store_talk_node.extras[i] = CDGN(817,5 + i);
	
	switch(store_talk_node.type) {
		case 1: case 2:
			if(cre(store_talk_node.extras[0],0,299,"First part of Stuff Done flag must be from 0 to 299.","",817) > 0) return false;
			if(cre(store_talk_node.extras[1],0,9,"Second part of Stuff Done flag must be from 0 to 9.","",817) > 0) return false;
			break;
		case 3:
			if(cre(store_talk_node.extras[0],0,1000,"Inn cost must be from 0 to 1000.","",817) > 0) return false;
			if(cre(store_talk_node.extras[1],0,3,"Inn quality must be from 0 to 3.","",817) > 0) return false;
			break;
		case 5:
			if(cre(store_talk_node.extras[1],0,9,"Event must be from 0 to 9. (0 means no event)","",817) > 0) return false;
			break;
		case 6:
			if(cre(store_talk_node.extras[0],0,199,"Town number must be from 0 to 199.","",817) > 0) return false;
			break;
		case 7: case 9: case 10: case 11: case 12:
			if(cre(store_talk_node.extras[0],0,6,"Cost adjustment must be from 0 (cheapest) to 6 (most expensive).","",817) > 0) return false;
			break;
		case 17:
			if(cre(store_talk_node.extras[0],0,6,"Enchantment type must be from 0 to 6. See the documentation for a list of possible abilities.","",817) > 0) return false;
			break;
		case 19: case 23:
			if(cre(store_talk_node.extras[1],0,299,"First part of Stuff Done flag must be from 0 to 299.","",817) > 0) return false;
			if(cre(store_talk_node.extras[2],0,9,"Second part of Stuff Done flag must be from 0 to 9.","",817) > 0) return false;
			break;
		case 20: case 21:
			if(cre(store_talk_node.extras[1],0,29,"The first boat/horse must be in the legal range (0 - 29).","",817) > 0) return false;
			break;
		case 22:
			if(cre(store_talk_node.extras[0],0,49,"The special item must be in the legal range (0 - 49).","",817) > 0) return false;
			break;
		case 29:
			if(cre(store_talk_node.extras[0],-1,99,"The town special node called must be in the legal range (0 - 99), or -1 for No Special.","",817) > 0) return false;
			break;
		case 30:
			if(cre(store_talk_node.extras[0],-1,255,"The scenario special node called must be in the legal range (0 - 255), or -1 for No Special.","",817) > 0) return false;
			break;
	}
	
	CDGT(817,9,town->talking.talk_strs[40 + store_which_talk_node * 2]);
	CDGT(817,10,town->talking.talk_strs[40 + store_which_talk_node * 2 + 1]);
	
	town->talking.talk_nodes[store_which_talk_node] = store_talk_node;
#endif
	return true;
}

void put_talk_node_in_dlog() {
	char str[256];
	short i;
#if 0
	CDSN(817,2,store_talk_node.personality);
	str[4] = 0;
	for(i = 0; i < 4; i++)
		str[i] = store_talk_node.link1[i];
	CDST(817,3,(char *) str);
	for(i = 0; i < 4; i++)
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
	for(i = 0; i < 4; i++)
		CDSN(817,5 + i,store_talk_node.extras[i]);
	CDST(817,9,town->talking.talk_strs[40 + store_which_talk_node * 2]);
	CDST(817,10,town->talking.talk_strs[40 + store_which_talk_node * 2 + 1]);
	
	if((store_talk_node.type == 7) || (store_talk_node.type == 9) ||
		(store_talk_node.type == 10) || (store_talk_node.type == 11))
		cd_activate_item(817,37,1);
	else cd_activate_item(817,37,0);
	if(last_talk_node[0] >= 0)
		cd_activate_item(817,13,1);
	else cd_activate_item(817,13,0);
	if((store_talk_node.type != 29) && (store_talk_node.type != 30))
		cd_activate_item(817,38,0);
	else cd_activate_item(817,38,1);
#endif
}

void edit_talk_node_event_filter (short item_hit) {
	short i,j,node_to_change_to = -1,spec;
#if 0
	switch(item_hit) {
		case 11:
			if(save_talk_node())
				toast_dialog();
			break;
		case 13:  //go_back
			if(!save_talk_node())
				break;
			for(i = 0; i < 60; i++)
				if(last_talk_node[i] < 0) {
					node_to_change_to = last_talk_node[i - 1];
					if(i > 0)
						last_talk_node[i - 1] = -1;
					i = 256;
				}
			break;
		case 12:
			toast_dialog();
			break;
		case 14:
			if(!save_talk_node())
				break;
			
			spec = -1;
			for(j = 0; j < 60; j++)
				if((town->talking.talk_nodes[j].personality < 0) && (town->talking.talk_nodes[j].extras[3] < 0)) {
					spec = j;
					j = 60;
				}
			
			if(spec < 0) {
				give_error("You have used all 60 available talk nodes. To create fresh dialogue, go back and reuse and old one.",
						   "",817);
				break;
			}
			
			for(i = 0; i < 60; i++)
				if(last_talk_node[i] < 0) {
					last_talk_node[i] = store_which_talk_node;
					node_to_change_to = spec;
					i = 60;
				}
			break;
			
		case 28:
			if(!save_talk_node())
				break;
			i = choose_text_res(-5,0,30,store_talk_node.type,817,"What Talking Node type?");
			if(i >= 0) {
				store_talk_node.type = i;
				if((store_talk_node.type == 29) || (store_talk_node.type == 30))
					store_talk_node.extras[0] = -1;
				put_talk_node_in_dlog();
			}
			
			break;
		case 37:
			i = CDGN(817,6);
			if(store_talk_node.type == 9) {
				i = choose_text_res(38,1,32,i + 1,817,"What is the first mage spell in the shop?");
				if(i >= 0)
					CDSN(817,6,i - 1);
			}
			else if(store_talk_node.type == 10) {
				i = choose_text_res(38,50,81,i + 50,817,"What is the first priest spell in the shop?");
				if(i >= 0)
					CDSN(817,6,i - 50);
			}
			else if(store_talk_node.type == 11) {
				i = choose_text_res(38,100,119,i + 100,817,"What is the first recipe in the shop?");
				if(i >= 0)
					CDSN(817,6,i - 100);
			}
			else {
				i = choose_text_res(-2,0,399,i,817,"What is the first item in the shop?");
				if(i >= 0)
					CDSN(817,6,i);
			}
			break;
		case 38:
			if(!save_talk_node())
				break;
			spec = CDGN(817,5);
			if((spec < 0) || (spec >= 100)) {
				if(store_talk_node.type == 29)
					spec = get_fresh_spec(2);
				else spec = get_fresh_spec(0);
				if(spec < 0) {
					give_error("You can't create a new special encounter because there are no more free special nodes.",
							   "To free a special node, set its type to No Special and set its Jump To special to -1.",817);
					break;
				}
				CDSN(817,5,spec);
			}
			if(store_talk_node.type == 29)
				edit_spec_enc(spec,2,817);
			else edit_spec_enc(spec,0,817);
			if(store_talk_node.type == 29) {
				if((spec >= 0) && (spec < 100) && (town->specials[spec].pic < 0))
					CDSN(817,5,-1);
			}
			else {
				if((spec >= 0) && (spec < 256) && (scenario.scen_specials[spec].pic < 0))
					CDSN(817,5,-1);
			}
			if(!save_talk_node())
				break;
			break;
	}
	if(node_to_change_to >= 0) {
		store_which_talk_node = node_to_change_to;
		store_talk_node = town->talking.talk_nodes[store_which_talk_node];
		if(store_talk_node.extras[3] < 0)
			store_talk_node.extras[3] = 0;
		put_talk_node_in_dlog();
	}
#endif
}

void edit_talk_node(short which_node,short parent_num) {
	// ignore parent in Mac version
	short talk_node_hit,i;
#if 0
	store_which_talk_node = which_node;
	for(i = 0; i < 60; i++)
		last_talk_node[i] = -1;
	//last_talk_node[0] = store_which_mode * 1000 + store_which_talk_node;
	store_talk_node = town->talking.talk_nodes[store_which_talk_node];
	if(store_talk_node.extras[3] < 0)
		store_talk_node.extras[3] = 0;
	
	cd_create_dialog_parent_num(817,parent_num);
	
	cd_activate_item(817,13,0);
	cd_activate_item(817,37,0);
	
	if((store_talk_node.type != 29) && (store_talk_node.type != 30))
		cd_activate_item(817,38,0);
	else cd_activate_item(817,38,1);
	put_talk_node_in_dlog();
	
	talk_node_hit = cd_run_dialog();
	
	cd_kill_dialog(817);
#endif
}

void pick_out_event_filter (short item_hit) {
	char temp_str[256];
#if 0
	switch(item_hit) {
		case 2:
			dialog_answer = store_cur_loc.x * 100 + store_cur_loc.y;
			toast_dialog();
			break;
		case 3:
			dialog_answer = -1;
			toast_dialog();
			break;
		case 12:
			if(store_cur_loc.x == 0) SysBeep(20);
			else store_cur_loc.x--;
			break;
		case 13:
			if(store_cur_loc.x >= scenario.out_width - 1) SysBeep(20);
			else store_cur_loc.x++;
			break;
		case 14:
			if(store_cur_loc.y == 0) SysBeep(20);
			else store_cur_loc.y--;
			break;
		case 15:
			if(store_cur_loc.y >= scenario.out_height - 1) SysBeep(20);
			else store_cur_loc.y++;
			break;
	}
	sprintf((char *) temp_str,"X = %d",store_cur_loc.x);
	csit(854,8,(char *) temp_str);
	sprintf((char *) temp_str,"Y = %d",store_cur_loc.y);
	csit(854,11,(char *) temp_str);
#endif
}

short pick_out(location default_loc) {
	// ignore parent in Mac version
	short basic_dlog_hit;
	char temp_str[256];
#if 0
	store_cur_loc = default_loc;
	
	cd_create_dialog_parent_num(854,0);
	
	cdsin(854,7,scenario.out_width);
	cdsin(854,10,scenario.out_height);
	sprintf((char *) temp_str,"X = %d",store_cur_loc.x);
	csit(854,8,(char *) temp_str);
	sprintf((char *) temp_str,"Y = %d",store_cur_loc.y);
	csit(854,11,(char *) temp_str);
	
	basic_dlog_hit = cd_run_dialog();
	
	cd_kill_dialog(854);
#endif
	return 0;//dialog_answer;
}

void new_town_event_filter (short item_hit) {
#if 0
	switch(item_hit) {
		case 3:
			dialog_answer = 1;
			toast_dialog();
			break;
		case 23:
			dialog_answer = -1;
			toast_dialog();
			break;
		default:
			cd_hit_led_range(830,11,13,item_hit);
			cd_hit_led_range(830,18,20,item_hit);
			break;
	}
#endif
}

bool new_town(short which_town) {
	// ignore parent in Mac version
	printf("Town creation currently disabled.\n");
//	short basic_dlog_hit,i,j,store_dialog_answer;
//	char temp_str[256];
//	short size = 0,preset = 0;
//
//
//	cd_create_dialog_parent_num(830,0);
//
//	cdsin(830,22,which_town);
//	cd_set_led(830,12,1);
//	cd_set_led(830,18,1);
//	sprintf((char *) temp_str,"Town name");
//	CDST(830,2,(char *) temp_str);
//
//	basic_dlog_hit = cd_run_dialog();
//
//	size = cd_get_led_range(830,11,13);
//	preset = cd_get_led_range(830,18,20);
//	CDGT(830,2,(char *) temp_str);
//	temp_str[30] = 0;
//	cd_kill_dialog(830,0);
//	if(dialog_answer < 0)
//		return false;
//
//	scenario.num_towns++;
//	scenario.town_size[which_town] = size;
//	scenario.town_hidden[which_town] = 0;
//	cur_town = which_town;
//	scenario.last_town_edited = cur_town;
//	init_town(size);
//	strcpy(town->town_strs(0),(char *) temp_str);
//
//	for(i = 0; i < max_dim[size]; i++)
//		for(j = 0; j < max_dim[size]; j++)
//			switch(preset) {
//				case 0:
//					town->terrain(i,j) = 0;
//					break;
//				case 1:
//					town->terrain(i,j) = 2;
//					break;
//				case 2:
//					town->terrain(i,j) = 2;
//					if(get_ran(1,0,8) == 0)
//						town->terrain(i,j) = 3;
//						else  if(get_ran(1,0,10) == 0)
//						town->terrain(i,j) = 4;
//					break;
//				}
//
//	reset_pwd();
//	return true;
	return false;
}

// before calling this, be sure to do all checks to make sure it's safe.
void delete_last_town() {
	scenario.num_towns--;
	scenario.town_size[scenario.num_towns] = 0;
	scenario.town_hidden[scenario.num_towns] = 0;
	scenario.town_data_size[scenario.num_towns][0] = 0;
	scenario.town_data_size[scenario.num_towns][1] = 0;
	scenario.town_data_size[scenario.num_towns][2] = 0;
	scenario.town_data_size[scenario.num_towns][3] = 0;
	scenario.town_data_size[scenario.num_towns][4] = 0;
	//scenario.flag_a = 41942 /*sizeof(scenario_data_type)*/ + get_ran(1,-1000,1000);
	//scenario.flag_b = town_s(user_given_password);
	//scenario.flag_c = out_s(user_given_password);
	//scenario.flag_e = str_size_1(user_given_password);
	//scenario.flag_f = str_size_2(user_given_password);
	//scenario.flag_h = str_size_3(user_given_password);
	//scenario.flag_g = 10000 + get_ran(1,0,5000);
	//scenario.flag_d = init_data(user_given_password);
	save_scenario();
}

void pick_import_town_event_filter (short item_hit) {
#if 0
	switch(item_hit) {
		case 3:
			dialog_answer = CDGN(841,2);
			if(dialog_answer < 0) {
				give_error("This number is out of the correct range. It must be at least 0.","",841);
				break;
			}
			toast_dialog();
			break;
		case 8:
			dialog_answer = -1;
			toast_dialog();
			break;
			
	}
#endif
}

short pick_import_town(short def, fs::path& temp_file_to_load) {
#if 0
	// ignore parent in Mac version
	short town_strs_hit;
	NavReplyRecord s_reply;
	
	NavChooseFile(NULL,&s_reply,NULL,NULL,NULL,NULL,NULL,NULL);
	
	if(!s_reply.validRecord)
		return -1;
	AEKeyword keyword;
	DescType descType;
	Size actualSize;
	
	AEGetNthPtr(&s_reply.selection,1,typeFSS,&keyword,&descType,&temp_file_to_load,sizeof(FSSpec),&actualSize);
	
	cd_create_dialog_parent_num(841,0);
	
	CDSN(841,2,0);
	
	town_strs_hit = cd_run_dialog();
	
	cd_kill_dialog(841);
#endif
	return 0;//dialog_answer;
}
