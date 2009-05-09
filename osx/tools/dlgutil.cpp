/*
 *  dlgutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "location.h"
#include "dlgconsts.h"
#include "dlgtool.h"
#include "dlgutil.h"
#include "soundtool.h"
#include "dlglowlevel.h"
#include "graphtool.h"
#include "mathutil.h"

extern char text_long_str[10][256];
extern char text_short_str[140][40];
extern dlg_t dlgs[ND];
extern dlg_item_t items[NI];
extern short available_dlog_buttons[NUM_DLOG_B];
extern btn_t buttons[];
extern short current_key;

short cd_create_custom_dialog(WindowPtr parent, Str255 strs[6],short pic_num,short btns[3]){
	
	short i,j,free_slot = -1,free_item = -1,str_width,cur_but_right = 0;
	WindowPtr dlg;
	short total_len = 0;
	
	
	short cur_item = 1;
	short but_items[3] = {-1,-1,-1};
	Rect pic_rect = {8,8,44,44},cur_text_rect = {2,50,0,0};
	short win_width = 100, win_height = 100;
	
	//store_parent = parent;
	for (i = 0; i < ND; i++) {
		if ((dlgs[i].key >= 0) && (dlgs[i].type == 900))
			return -1;
	}
	for (i = 0; i < ND; i++) {
		if (dlgs[i].key < 0) {
			free_slot = i;
			i = 500;
		}
	}
	if (free_slot < 0)
		return -2;
	
	// quick check, to make sure there's at least 1 button
	if ((btns[0] < 0) && (btns[1] < 0) && (btns[2] < 0))
		btns[0] = 1;
	current_key++;
	dlgs[free_slot].key = current_key;
	dlgs[free_slot].type = 900;
	dlgs[free_slot].highest_item = 1;
	dlgs[free_slot].draw_ready = false;
	dlgs[free_slot].win = NULL;
	
	// first, create dummy dlog
	dlg = GetDialogWindow(GetNewDialog (900, NULL, IN_FRONT));
	dlgs[free_slot].win = dlg;
	if (dlgs[free_slot].win == NULL) {
		play_sound(3);
		return -3;
	}
	
	dlgs[free_slot].parent = parent;
	
	//process_new_window (dlgs[free_slot]);
	// instead of this, custom make items
	free_item = -1;
	
	// first, do 1-3 buttons
	for (i = 0; i < 3; i++) 
		if (btns[i] >= 0) {// buttons
			for (j = 150; j < NI; j++)
				if (items[j].dlg < 0) {
					free_item = j;
					j = NI + 1;
				}
			items[free_item].dlg = 900;
			items[free_item].type = (i == 0) ? 1 : 0;
			items[free_item].number = cur_item;
			//items[free_item].rect = get_item_rect(hDlg,i + 1);
			
			items[free_item].flag = available_dlog_buttons[btns[i]];
			items[free_item].active = 1;
			items[free_item].label = 0;
           	items[free_item].label_loc = -1;
            items[free_item].key = buttons[available_dlog_buttons[btns[i]]].def_key;
            if (i == 0)
            	items[free_item].key = 31;
            but_items[i] = free_item; // remember this to set item rect later
       		cur_item++;
		}
	// next, the upper left picture (always there)
	for (j = 150; j < NI; j++)
		if (items[j].dlg < 0) {
			free_item = j;
			j = NI + 1;
		}
	items[free_item].dlg = 900;
	items[free_item].type = 5;
	items[free_item].number = cur_item;
	items[free_item].rect = pic_rect;
	
	if (pic_num < 0) {
		items[free_item].flag = pic_num * -1;
	}
	else //if (pic_num < 1000)
		items[free_item].flag = pic_num;
	//			else items[free_item].flag = (pic_num % 1000) + 2400;
	//	if (pic_num >= 2000)
	//		items[free_item].flag += 300;
	items[free_item].active = 1;
	items[free_item].label = 0;
    items[free_item].label_loc = -1;
    items[free_item].key = 0;
    cur_item++;
	
	// finally, 0-6 text, first do preprocessing to find out how much room we need
	for (i = 0; i < 6; i++) 
		total_len += string_length((char *) strs[i]);
	total_len = total_len * 12;
	str_width = s_sqrt(total_len) + 20;
	//print_nums(0,total_len,str_width);
	if (str_width < 340)
		str_width = 340;
	cur_text_rect.right = cur_text_rect.left + str_width;
	// finally, 0-6 text, then create the items
	for (i = 0; i < 6; i++) 
		if (strlen((char *) strs[i]) > 0) {// text
			for (j = 0; j < 10; j++)
				if (items[j].dlg < 0) {
					free_item = j;
					j = NI + 1; 
				}
			items[free_item].dlg = 900;
			items[free_item].type = 9;
			items[free_item].number = cur_item;
			items[free_item].rect = cur_text_rect;
			items[free_item].rect.bottom = items[free_item].rect.top + 
			((string_length((char *) strs[i]) + 60) / str_width) * 12 + 16;
			items[free_item].rect.right += 20;
			
			//print_nums(i,string_length((char *) strs[i]),str_width);
			cur_text_rect.top = items[free_item].rect.bottom + 8;
			items[free_item].flag = 0;
			items[free_item].active = 1;
			items[free_item].label = 0;
           	items[free_item].label_loc = -1;
            items[free_item].key = 0;
 			sprintf(text_long_str[free_item],"%s",
					(char *) strs[i]);
      		cur_item++;
		}
	
	dlgs[free_slot].highest_item = cur_item - 1;
	
	// finally, do button rects
	cur_but_right = cur_text_rect.right + 30;
	//cur_text_rect.top += 8;
	for (i = 0; i < 3; i++)
		if (btns[i] >= 0) {
			items[but_items[i]].rect.right = cur_but_right;
			items[but_items[i]].rect.top = cur_text_rect.top;
			items[but_items[i]].rect.bottom = items[but_items[i]].rect.top + 23;
			if (buttons[available_dlog_buttons[btns[i]]].type == 1)
				items[but_items[i]].rect.left = items[but_items[i]].rect.right - 63;
			else items[but_items[i]].rect.left = items[but_items[i]].rect.right - 110;
			cur_but_right = items[but_items[i]].rect.left - 4;
			if (i == 0) {
				win_width = items[but_items[i]].rect.right + 6;
				win_height = items[but_items[i]].rect.bottom + 6;
			}
		}	
	
	//MoveWindow(dlgs[free_slot].win,(windRect.right - win_width) / 2,(windRect.bottom - win_width) / 2 + 20,false);	
	SizeWindow(dlgs[free_slot].win,win_width,win_height,false);
	
	ShowWindow(dlgs[free_slot].win);
	SetPort(GetWindowPort(dlgs[free_slot].win));
	TextFont(geneva_font_num);
	TextFace(bold);
	TextSize(10);
	ForeColor(blackColor);
	BackColor(whiteColor);
	untoast_dialog();
	
	return 0;
}

void oops_error(short error, short code, short mode){ // mode is 0 for scened, 1 for game, 2 for pced
	Str255 error_str1, error_str2;
	static const char* progname[3] = {"the scenario editor", "Blades of Exile", "the PC editor"};
	static const char* filetname[3] = {"scenario", "game", "game"};
	
	SysBeep(50);
	SysBeep(50);
	SysBeep(50);
	sprintf((char*)error_str1,"The program encountered an error while loading/saving/creating the %s. To prevent future problems, the program will now terminate. Trying again may solve the problem.", filetname[mode]);
	sprintf((char *) error_str2,"Giving %s more memory might also help. Be sure to back your %s up often. Error number: %d.",progname[mode],filetname[mode],error);
	if(code != 0)
		sprintf((char*) error_str2,"%s Result code: %i.",error_str2,code);
	give_error((char*) error_str1,(char *) error_str2,0);
	ExitToShell();
}

void give_error(char *text1, char *text2,short parent_num){
	display_strings(text1,text2,"Error!",57,/*7*/16,PICT_DLG,parent_num);
}

short store_str_label_1, store_str_label_2, store_str_label_1b, store_str_label_2b;
// str_label_1 & str_label_2 uysed for saving button for journal
// 1000 + x scen 2000 + x out 3000 + x town
__attribute__((deprecated))
void display_strings(char *text1, char *text2,short str_label_1,short str_label_2,short str_label_1b,
					 short str_label_2b,
					 char *title,short sound_num,short graphic_num,short graphic_type,short parent_num){
	
	short item_hit;
	
	set_cursor(sword_curs);
	
	store_str_label_1 = str_label_1;
	store_str_label_2 = str_label_2;
	store_str_label_1b = str_label_1b;
	store_str_label_2b = str_label_2b;
	//	store_str1a = str1a;
	//	store_str1b = str1b;
	//	store_str2a = str2a;
	//	store_str2b = str2b;
	
	//	if ((str1a <= 0) || (str1b <= 0))
	//		return;
	short which_string_dlog = 970;
	if (strlen(title) > 0)
		which_string_dlog += 2;
	if ((text2 != NULL) && (text2[0] != 0))
		which_string_dlog++;
	cd_create_dialog_parent_num(which_string_dlog,parent_num);
	
	csp(which_string_dlog,which_string_dlog,graphic_num,graphic_type);
	
	csit(which_string_dlog,4,(char *) text1);
	if (text2 != NULL) {
		csit(which_string_dlog,5,(char *) text2);
	}
	if (strlen(title) > 0)
		csit(which_string_dlog,6,title);
	csp(which_string_dlog,3,graphic_num,graphic_type);
	if (sound_num >= 0)
		play_sound(sound_num);
	
	if ((str_label_1 < 0) && (str_label_2 < 0))
		cd_activate_item(which_string_dlog,2,0);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(which_string_dlog);
	//final_process_dialog(which_string_dlog);
}

void display_strings(short a1,short a2, short b1, short b2,
							   char *title,short sound_num,short graphic_num,short graphic_type,short parent_num){
	Str255 str1 = "", str2 = "";
	
	if ((a1 > 0) && (a2 > 0))
		get_str(str1,a1,a2);
	if ((b1 > 0) && (b2 > 0))
		get_str(str2,b1,b2);
	display_strings((char *) str1,(char *) str2,-1,-1,-1,-1,
					title, sound_num, graphic_num, graphic_type, parent_num);
}

extern void record_display_strings();
void display_strings_event_filter (short item_hit){
	switch (item_hit) {
		case 1:
			toast_dialog();
			break;
		case 2:
			record_display_strings();
			break;
	}
}

short dialog_answer;
void fancy_choice_dialog_event_filter (short item_hit)
{
	toast_dialog();
	dialog_answer = item_hit;
}

short fancy_choice_dialog(short which_dlog,short parent)
// ignore parent in Mac version
{
	short item_hit,i,store_dialog_answer;
	
	store_dialog_answer = dialog_answer;
	//make_cursor_sword();
	
	cd_create_dialog_parent_num(which_dlog,parent);
	
	item_hit = cd_run_dialog();
	
	cd_kill_dialog(which_dlog);
	
	
	i = dialog_answer;
	dialog_answer = store_dialog_answer;
	
	return i;
}

void display_strings(char *text1, char *text2,
					 char *title,short sound_num,short graphic_num,short graphic_type,short parent_num){
	
	short item_hit;
	location view_loc;
	bool sound_done = false;
	
	//make_cursor_sword();
	
	if ((text1 == NULL) && (text2 == NULL))
		return;
	short which_string_dlog = 970;
	if (strlen(title) > 0)
		which_string_dlog += 2;
	if ((text2 != NULL) && (text2[0] != 0))
		which_string_dlog++;
	cd_create_dialog_parent_num(which_string_dlog,parent_num);
	
	csp(which_string_dlog,which_string_dlog,graphic_num,graphic_type);
	
	csit(which_string_dlog,4,(char *) text1);
	if (text2 != NULL) {
		csit(which_string_dlog,5,(char *) text2);
	}
	if (strlen(title) > 0)
		csit(which_string_dlog,6,title);
	csp(which_string_dlog,3,graphic_num,graphic_type);
	//if (sound_num >= 0)
	//	play_sound(sound_num);
	
	item_hit = cd_run_dialog();
	
	cd_kill_dialog(which_string_dlog);
}

