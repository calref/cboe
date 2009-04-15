#include <Carbon/Carbon.h>

#define ND	15
#define	NI	500
#define	NL	100

#include "stdio.h"
#include "string.h"
#include "dlogtool.h"
#include "sound.h"
#include "global.h"
#include "tGraphics.h"
#include "keydlgs.h"

extern Boolean play_sounds,cursor_shown,dialog_not_toast;
extern WindowPtr mainPtr;
extern Boolean modeless_exists[18];
extern DialogPtr modeless_dialogs[18];
extern unsigned char m_pic_index[200];
extern unsigned char m_pic_index_x[200];
extern unsigned char m_pic_index_y[200];

extern GWorldPtr dlg_buttons_gworld[NUM_BUTTONS][2],anim_gworld,talkfaces_gworld,items_gworld,tiny_obj_gworld;
extern GWorldPtr pc_gworld,dlogpics_gworld,monst_gworld[10],terrain_gworld[7];
extern PixPatHandle	bg[14];
extern short geneva_font_num;

short current_key = 0;
short dlg_keys[ND];
short dlg_types[ND];
WindowPtr dlgs[ND];
WindowPtr dlg_parent[ND];
short dlg_highest_item[ND];
Boolean dlg_draw_ready[ND];

short item_dlg[NI];
short item_number[NI];
char item_type[NI];
Rect item_rect[NI];
short item_flag[NI];
short item_flag2[NI];
char item_active[NI];
char item_key[NI];
short item_label[NI];
short item_label_loc[NI];

char text_long_str[10][256];
char text_short_str[140][40];
char labels[NL][25];
Boolean label_taken[NL];


short store_free_slot,store_dlog_num;

short available_dlog_buttons[NUM_DLOG_B] = {0,63,64,65,1,4,5,8, 
								128,
								9,10,11,12,13,
								14,15,16,17,29, 51,60,61,62,
								66,69,70, 71,72,73,74,79,
								80,83,86,87,88, 91,92,93,99,100,
								101,102,104, 129,130,131,132,133,
								134,135,136,137};
short button_type[140] = {1,1,4,5,1,1,0,0,1,1,
						 1,1,1,1,1,1,1,1,8,8,
						 9,9,9,1,1,2,1,6,7,1,
						 1,12,1,1,2,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,
						 0,1,1,1,2,1,1,1,2,2, // 50
						 1,1,1,1,1,1,2,3,1,1,
						 1,1,1,1,2,2,2,2,2,1,
						 1,1,1,1,2,2,1,1,1,2,
						 0,1,1,1,14,13,12,12,12,1,
						 1,1,1,2,1,2,2,2,2,1, // 100
						 2,2,2,2,2,2,2,2,2,2,
						 2,2,2,2,2,2,2,2,0,0,
						 1,1,1,1,1,1,0,0,0,0};
char *button_strs[140] = {"Done ","Ask"," "," ","Keep", "Cancel","+","-","Buy","Leave",
						"Get","1","2","3","4","5","6","Cast"," "," ",
						" "," "," ","Buy","Sell","Other Spells","Buy x10"," "," ","Save",
						"Race","Train","Items","Spells","Heal Party","1","2","3","4","5",
						"6","7","8","9","10","11","12","13","14","15",
				/*50*/  "16","Take","Create","Delete","Race/Special","Skill","Name","Graphic","Bash Door","Pick Lock",
						"Leave","Steal","Attack","OK","Yes","No","Step In"," ","Record","Climb",
						"Flee","Onward","Answer","Drink","Approach","Mage Spells","Priest Spells","Advantages","New Game","Land",
						"Under","Restore","Restart","Quit","Save First","Just Quit","Rest","Read","Pull","Alchemy",
						"17","Push","Pray","Wait","","","Delete","Graphic","Create","Give",
				/*100*/		"Destroy","Pay","Free","Next Tip","Touch", "Select Icon","Create/Edit","Clear Special","Edit Abilities","Choose",
						"Go Back","Create New","General","One Shots","Affect PCs","If-Thens","Town Specs","Out Specs","Advanced","Weapon Abil",
						"General Abil.","NonSpell Use","Spell Usable","Reagents","Missiles","Abilities","Pick Picture","Animated","Enter","Burn",
						"Insert","Remove","Accept","Refuse","Open","Close","Sit","Stand","",""};
short button_left_adj[140] = {2,0,0,0,6,0,0,0,0,5,
							0,0,0,0,0,0,0,4,0,0,
							0,0,0,0,0,5,0,0,0,6,
							6,6,0,0,0,0,0,0,0,0,
							0,0,0,0,6,6,6,6,6,6,
							6,6,0,2,0,0,0,2,3,3, // 50
							6,6,0,7,5,5,0,0,2,6,
							4,2,0,0,0,4,10,4,0,6,
							6,2,1,6,4,3,0,4,6,4,
							6,6,6,6,0,0,0,2,0,0,
							2,6,6,3,6,7,3,0,0,0, // 100
							0,5,3,3,3,3,3,4,4,4,
							6,4,4,2,2,3,6,5,0,0,
							4,4,4,4,6,4,0,0,0,0
							};
char button_def_key[140] = {0,0,20,21,'k', 24,0,0,0,0,
							'g','1','2','3','4', '5','6',0,0,0,
							0,0,0,0,0,' ',0,22,23,0,
							0,0,0,0,0,'1','2','3','4','5',
							'6','7','8','9','a', 'b','c','d','e','f',  
							'g',0,0,0,0,0,0,0,0,0,
							0,0,0,0,'y','n',0,'?','r',0,
							0,0,0,0,0,0,0,0,0, 0,
							0,0,0,0,0,0,0,0,0,0,
							'g',0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0
							};
							// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
							// 25-30  ctrl 1-6  31 - return\


void beep() 
{
	SysBeep(20);
}

void cd_init_dialogs()
{
	short i;

	for (i = 0; i < ND; i++) {
		dlg_keys[i] = -1;
		dlg_types[i] = 0;
		dlgs[i] = NULL;
		dlg_highest_item[i] = 0;
		}
	for (i = 0; i < NI; i++) {
		item_dlg[i] = -1;
		}
	for (i = 0; i < NL; i++) {
		label_taken[i] = FALSE;
		}
}


short cd_create_dialog_parent_num(short dlog_num,short parent)
{
	short i;

	if ((parent == 0) || (parent == 1))
		return cd_create_dialog(dlog_num,mainPtr);
	i = cd_get_dlg_index(parent);
	if (i < 0)
		return -1;
	return cd_create_dialog(dlog_num,dlgs[i]);

}

short cd_create_dialog(short dlog_num,WindowPtr parent)
{
	short i,j,free_slot = -1,free_item = -1;
	WindowPtr dlg;

	char item_str[256];
	short type,flag;

	store_dlog_num = dlog_num;
	//store_parent = parent;
	for (i = 0; i < ND; i++) {
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			return -1;
		}
	for (i = 0; i < ND; i++) {
		if (dlg_keys[i] < 0) {
			free_slot = i;
			i = 500;
			}
		}
	if (free_slot < 0)
		return -2;
	current_key++;
	dlg_keys[free_slot] = current_key;
	dlg_types[free_slot] = dlog_num;
	dlg_highest_item[free_slot] = 1;
	dlg_draw_ready[free_slot] = FALSE;
	dlgs[free_slot] = NULL;

	// first, create dummy dlog
	store_free_slot = free_slot;
	//dlg = CreateDialog(store_hInstance,MAKEINTRESOURCE(dlog_num),0,d_proc);
	dlg = GetDialogWindow(GetNewDialog (dlog_num, NIL, IN_FRONT));
	dlgs[free_slot] = dlg;
	if (dlgs[free_slot] == NULL) {
		play_sound(3);
		return -3;
		}
//	center_window(dlgs[free_slot]);

	dlg_parent[free_slot] = parent;
	
	process_new_window (dlgs[free_slot]);


	switch (dlog_num) {
/*		case 1010: SetWTitle(dlgs[free_slot],"Training a PC"); break;
		case 1014: SetWTitle(dlgs[free_slot],"Reading a Sign"); break;
		case 1015: SetWTitle(dlgs[free_slot],"Talking to Someone"); break;
		case 1018: SetWTitle(dlgs[free_slot],"Select a PC"); break;
		case 1019: SetWTitle(dlgs[free_slot],"In a Shop"); break;
		case 1020: SetWTitle(dlgs[free_slot],"In a Shop"); break;
		case 1021: SetWTitle(dlgs[free_slot],"Buying Food"); break;
		case 1048: SetWTitle(dlgs[free_slot],"Visiting the Healer"); break;
		case 1050: SetWTitle(dlgs[free_slot],"Pick PC Graphic"); break;
		case 998: SetWTitle(dlgs[free_slot],"Item Info"); break;
		case 992: SetWTitle(dlgs[free_slot],"PC Spells"); break;
		case 1097: SetWTitle(dlgs[free_slot],"Skills Library"); break;
		case 1096: SetWTitle(dlgs[free_slot],"Spells Library"); break;
		case 999: SetWTitle(dlgs[free_slot],"Monster Library"); break;
		case 997: SetWTitle(dlgs[free_slot],"Help Library"); break;
		case 995: SetWTitle(dlgs[free_slot],"Introduction"); break;
		case 996: SetWTitle(dlgs[free_slot],"Alchemy"); break;
		case 1013: SetWTitle(dlgs[free_slot],"Races/Advantages"); break;
		case 1099: SetWTitle(dlgs[free_slot],"Preferences"); break;
		case 1098: SetWTitle(dlgs[free_slot],"Casting a Spell"); break;
		case 987: SetWTitle(dlgs[free_slot],"Getting Items"); break;
		case 989: SetWTitle(dlgs[free_slot],"Create a Party"); break;
		case 1047: SetWTitle(dlgs[free_slot],"Do Alchemy"); break;*/
		}

	ShowWindow(dlgs[free_slot]);
	SetPortWindowPort(dlgs[free_slot]);
	//TextFont(geneva_font_num);
	//TextFace(bold);
	//TextSize(10);
	ForeColor(blackColor);
	BackColor(whiteColor);
	dialog_not_toast = TRUE;
	//DestroyWindow(dlg); //Necesary? Dunno.

/*	if (dlg_parent[free_slot] != NULL) {
		EnableWindow(dlg_parent[free_slot],FALSE);
		if (dlg_parent[free_slot] == mainPtr)
			for (i = 0; i < 18; i++)
				if (modeless_exists[i] == TRUE)
					EnableWindow(modeless_dialogs[i],FALSE);
		} */
	set_cursor(0);
	
	return 0;
}

void process_new_window (WindowPtr hDlg) {
	short i = -1,j,free_slot = -1,free_item = -1,type,flag;
	char but_str[30];
	Str255 item_str;
	Boolean str_stored = FALSE;
	Rect dlg_rect,store_rect;
	short win_height = 0, win_width = 0;
	short the_type;
	Handle the_handle = NULL;
	Rect small_rect;
	short item_hit,what_talk_field,num_items;
	short str_offset = 1;
	long typel,flagl;

	free_slot = store_free_slot;
	num_items = CountDITL(GetDialogFromWindow(hDlg));

	dlg_highest_item[free_slot] = 0;
	
	// Now, give the window its items
	for (i = 0; i < num_items; i++) {
		str_offset = 1;
		GetDialogItem( GetDialogFromWindow(hDlg), i + 1, &the_type, &the_handle, &small_rect);
		if (the_type % 128 == 8) {
			GetDialogItemText(the_handle,item_str);
			p2c(item_str);
			dlg_highest_item[free_slot]++;
			str_stored = FALSE;
			if (strlen((char *)item_str) == 0) {
				sprintf((char *) item_str, "+");
				type = 3;
				flag = 1;
	            str_stored = TRUE;
			}
			else if (item_str[0] == '+') { // default is framed text
				type = 3;
				flag = 1;
	            str_stored = TRUE;
				}
			else if (item_str[0] == '*') {
				type = 3;
				flag = 0;
				str_stored = TRUE;
				}
			else if (item_str[0] == '~') {
				type = 7;
				flag = 0;
				str_stored = TRUE;
				}
			else if (item_str[0] == '!') {
				type = 4;
				flag = 0;
				str_stored = TRUE;
				}
			else if (item_str[0] == '=') {
				type = 9;
				flag = 1;
				str_stored = TRUE;
				}
			else if (((item_str[0] >= 65) && (item_str[0] <= 122)) || (item_str[0] == '"')) {
				type = 9;
				flag = 0;
				str_offset = 0;
				str_stored = TRUE;
				}
			else if ((item_str[0] == '^') || (item_str[0] == '&')) {
				type = (item_str[0] == '^') ? 10 : 11;
				flag = 1;
				if (string_length((char *) item_str) > 55)
					flag = 2;
				str_stored = TRUE;
				}
			else {
#ifndef EXILE_BIG_GUNS
				sscanf((char *) item_str,"%hd_%hd",&type,&flag);
#endif		
#ifdef EXILE_BIG_GUNS
				sscanf((char *) item_str,"%hd_%hd",&type,&flag);
				//type = typel; flag = flagl;
#endif		
				}

			free_item = -1;
			// find free item
			switch (type) {
				case 0: case 1: case 2: case 5: case 6:
					for (j = 150; j < NI; j++)
						if (item_dlg[j] < 0) {
							free_item = j;
							j = NI + 1;
							}
					break;
				default:
					if ((type == 9) || 
					 ((str_stored == TRUE) && (strlen((char *) item_str) > 35))) {
						for (j = 0; j < 10; j++)
							if (item_dlg[j] < 0) {
								free_item = j;
								j = NI + 1;
								}
						}
						else {
							for (j = 10; j < 140; j++)
								if (item_dlg[j] < 0) {
									free_item = j;
									j = NI + 1;
									}
							}
					break;
				}

			if (free_item >= 0) {
					item_dlg[free_item] = store_dlog_num;
					item_type[free_item] = type;
					item_number[free_item] = i + 1;

					item_rect[free_item] = get_item_rect(hDlg,i + 1);

					item_flag[free_item] = flag;
					item_active[free_item] = 1;
					item_label[free_item] = 0;
               		item_label_loc[free_item] = -1;
               		item_key[free_item] = 0;
					switch (type) {
						case 0: case 1:
							GetPortBounds(dlg_buttons_gworld[button_type[flag]][0], &store_rect);
							item_rect[free_item].right = item_rect[free_item].left + store_rect.right;
							item_rect[free_item].bottom = item_rect[free_item].top + store_rect.bottom;
							item_key[free_item] = button_def_key[flag];
							if (type == 1)
								item_key[free_item] = 31;
							break;
						case 2:
							item_rect[free_item].right = item_rect[free_item].left + 14;
							item_rect[free_item].bottom = item_rect[free_item].top + 10;
                    		item_key[free_item] = 255;
							break;
						case 3: case 4: case 7: case 8: case 9: case 10: case 11: 
							sprintf(((free_item < 10) ? text_long_str[free_item] : text_short_str[free_item - 10]),"");
							if (str_stored == TRUE) {
								if (free_item < 10)
								sprintf(text_long_str[free_item],"%s",
								  (char *) (item_str + str_offset));
								else
								sprintf(text_short_str[free_item - 10],"%-39.39s",
								  (char *) (item_str + str_offset));
								}
							item_key[free_item] = 255; 
							if (type >= 10) {
								GetPortBounds(dlg_buttons_gworld[1][0], &store_rect);
								item_rect[free_item].right = item_rect[free_item].left + store_rect.right;
								item_rect[free_item].bottom = item_rect[free_item].top + store_rect.bottom;
								if (type == 11)
									item_key[free_item] = 31;
								}
							break;
						}
					win_height = max(win_height, item_rect[free_item].bottom + 5);
					win_width = max(win_width, item_rect[free_item].right + 6);

				}

			}
		}
	ShortenDITL(GetDialogFromWindow(hDlg),dlg_highest_item[free_slot]);
	SizeWindow(hDlg,win_width,win_height,FALSE);
	dlg_highest_item[free_slot] = num_items;
	}


short cd_kill_dialog(short dlog_num,short parent_message)
{
	short i,which_dlg = -1;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	if (which_dlg < 0)
		return -1;

	for (i = 0; i < NI; i++)
		if (item_dlg[i] == dlog_num) {
			if (item_label[i] > 0)
				label_taken[item_label_loc[i]] = FALSE;
			item_dlg[i] = -1;
			}

/*	if (dlg_parent[which_dlg] != NULL) {
		EnableWindow(dlg_parent[which_dlg],TRUE);
		if (dlg_parent[which_dlg] == mainPtr)
			for (i = 0; i < 18; i++)
				if (modeless_exists[i] == TRUE)
					EnableWindow(modeless_dialogs[i],TRUE);
		SetFocus(dlg_parent[which_dlg]);
		SetWindowPos(dlg_parent[which_dlg],WindowPtr_TOP,0,0,100,100,
			SWP_NOMOVE | SWP_NOSIZE);
		cd_set_edit_focus();
		} */
//	if (parent_message > 0)
//		SendMessage(dlg_parent[which_dlg],WM_COMMAND,parent_message,0);

	DisposeDialog(GetDialogFromWindow(dlgs[which_dlg]));
	dlg_keys[which_dlg] = -1;
	if (dlg_parent[which_dlg] != NULL)
		SetPortWindowPort( dlg_parent[which_dlg]);
	if (FrontWindow() != mainPtr)
		redraw_screen();
	dialog_not_toast = TRUE;
	return 0;
}

short cd_process_click(WindowPtr window,Point the_point, short mods,short *item)
{
	short i,which_dlg,dlg_num,item_id;
	short dlog_key;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlog_key)) < 0)
		return -1;

	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
			if ((PtInRect(the_point,&item_rect[item_id])) && (item_active[item_id] > 0)
				&& ((item_type[item_id] < 3) || (item_type[item_id] == 8)
				|| (item_type[item_id] == 10)|| (item_type[item_id] == 11))) {
					*item = i;
					if ((mods & 2048) != 0) 
						*item += 100;
					if (item_type[item_id] != 8)
						cd_press_button(dlg_num,i);
					return dlg_num;
					}
			}
	return -1;
}



short cd_process_keystroke(WindowPtr window,char char_hit,short *item)
{
	short i,which_dlg,dlg_num,dlg_key,item_id;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return -1;
							// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
							// 25-30  ctrl 1-6


	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
			if ((item_key[item_id] == char_hit) && (item_active[item_id] > 0)
				&&  ((item_type[item_id] < 3) || (item_type[item_id] == 8)
				 || (item_type[item_id] == 10) || (item_type[item_id] == 11))) {
					*item = i;
					if (item_type[item_id] != 8)
						cd_press_button(dlg_num,i);
					return dlg_num;
			}
			}
			
	// kludgy. If you get an escape and is isn't processed, make it an enter
	if (char_hit == 24) {
   	char_hit = 31;
		for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
			if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
				if ((item_key[item_id] == char_hit) && (item_active[item_id] > 0)
					&&  ((item_type[item_id] < 3) || (item_type[item_id] == 8))) {
						*item = i;
						if (item_type[item_id] != 8)
							cd_press_button(dlg_num,i);
						return dlg_num;
				}
				}
		}
	return -1;
}

/*
void cd_init_button(short dlog_num,short item_num, short button_num, short status)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (item_type[item_index] > 1) {
		beep();
		return;
		}
	item_flag[item_index] = button_num;
	item_active[item_index] = status;
	item_rect[item_index].right = item_rect[item_index].left + button_width[button_num];
	item_rect[item_index].bottom = item_rect[item_index].top + button_width[button_num];
	item_key[item_index] = button_def_key[button_num];
	cd_draw_item(dlog_num,item_num);
}
*/

void cd_attach_key(short dlog_num,short item_num,char key)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if ((item_type[item_index] > 2) && (item_type[item_index] != 8)) {
		beep();
		return;
		}
	item_key[item_index] = key;
}

void csp(short dlog_num, short item_num, short pict_num, short pict_type)
{
	cd_set_pict( dlog_num,  item_num,  pict_num,  pict_type);
}

void cd_set_pict(short dlog_num, short item_num, short pict_num, short pict_type)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (item_type[item_index] != 5) {
		beep();
		return;
	}
	item_flag[item_index] = pict_num;
	item_flag2[item_index] = pict_type;
	if (pict_num == -1)
		cd_erase_item(dlog_num,item_num);
	else cd_draw_item(dlog_num,item_num);
}

void cd_activate_item(short dlog_num, short item_num, short status)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	item_active[item_index] = status;
	cd_draw_item(dlog_num,item_num);
}

short cd_get_active(short dlog_num, short item_num)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return -1;

	return item_active[item_index];
}


void cd_get_item_text(short dlog_num, short item_num, char *str)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_index >= 150) {
		beep();
		return;
		}
	if (item_index < 10)
		sprintf(str,"%s",text_long_str[item_index]);
		else sprintf(str,"%s",text_short_str[item_index - 10]);
}

void csit(short dlog_num, short item_num, char *str)
{
cd_set_item_text( dlog_num,  item_num, str);
}

void cd_retrieve_text_edit_str(short dlog_num, short item_num, char *str)
{
	short dlg_index,item_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	
	sprintf(str,"");
	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0) {
		SysBeep(50); return ;
		}
	GetDialogItem(GetDialogFromWindow(dlgs[dlg_index]), item_num, &the_type, &the_handle, &the_rect);
	GetDialogItemText(the_handle,store_ptr);
	p2c(store_ptr);
	strcpy(str,(char *) store_ptr);
}
	
short cd_retrieve_text_edit_num(short dlog_num, short item_num)
{
	short dlg_index,item_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	long num_given;
	
	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0) {
		SysBeep(50); return -1;
		}
	GetDialogItem(GetDialogFromWindow( dlgs[dlg_index]), item_num, &the_type, &the_handle, &the_rect);
	GetDialogItemText(the_handle,store_ptr);
	StringToNum (store_ptr,&num_given);
	return (short) num_given;
}	


// NOTE!!! Expects a c string
void cd_set_text_edit_str(short dlog_num, short item_num, char *str)
{
	short dlg_index,item_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	
	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0) {
		SysBeep(50); return ;
		}
	strcpy((char *) store_ptr,str);
	c2p(store_ptr);
	GetDialogItem(GetDialogFromWindow( dlgs[dlg_index]), item_num, &the_type, &the_handle, &the_rect );
	SetDialogItemText ( the_handle, store_ptr);	
}
// NOTE!!! Expects a c string
void cd_set_text_edit_num(short dlog_num, short item_num, short num)
{
	short dlg_index,item_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	
	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0) {
		SysBeep(50); return ;
		}
		
	sprintf((char *) store_ptr,"%d",num);
	c2p(store_ptr);
	GetDialogItem( GetDialogFromWindow(dlgs[dlg_index]), item_num, &the_type, &the_handle, &the_rect );
	SetDialogItemText ( the_handle, store_ptr);	
}

void cd_set_item_text(short dlog_num, short item_num, char *str)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_index >= 150) {
		beep();
		return;
		}
	if (item_index < 10)
		sprintf(text_long_str[item_index],"%s",str);
		else sprintf(text_short_str[item_index - 10],"%-39.39s",str);
	cd_draw_item( dlog_num,item_num);
}

void cdsin(short dlog_num, short item_num, short num) 
{
	cd_set_item_num( dlog_num,  item_num,  num);
}


void cd_set_item_num(short dlog_num, short item_num, short num)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_index >= 150) {
		beep();
		return;
		}
	if (item_index < 10)
		sprintf(text_long_str[item_index],"%d",num);
		else sprintf(text_short_str[item_index - 10],"%d",num);
	cd_draw_item( dlog_num,item_num);
}

void cd_set_led(short dlog_num,short item_num,short state)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if (item_type[item_index] != 2) {
		beep();
		return;
		}
	item_flag[item_index] = state;
	cd_draw_item(dlog_num,item_num);
}

// Use when checking for events ... if item hit matches item num, this led was
// hit, and flip it to other thing.
void cd_flip_led(short dlog_num,short item_num,short item_hit)
{
	if (item_hit != item_num)
		return;
	if (cd_get_led(dlog_num,item_num) > 0)
		cd_set_led(dlog_num,item_num,0);
		else cd_set_led(dlog_num,item_num,1);
}

void cd_set_led_range(short dlog_num,short first_led,short last_led,short which_to_set)
{
	short i;
	
	for (i = first_led; i <= last_led; i++) {
		if (i - first_led == which_to_set)
			cd_set_led(dlog_num,i,1);
			else cd_set_led(dlog_num,i,0);
		}
}

// Use when someone presses something in the range, to light up the right button
// Also has an error check ... if which_to_set is outside range, do nothing
void cd_hit_led_range(short dlog_num,short first_led,short last_led,short which_to_set)
{
	short i;
	
	if ((which_to_set < first_led) || (which_to_set > last_led))
		return;
	for (i = first_led; i <= last_led; i++) {
		if (i == which_to_set)
			cd_set_led(dlog_num,i,1);
			else cd_set_led(dlog_num,i,0);
		}
}
short cd_get_led_range(short dlog_num,short first_led,short last_led)
{
	short i;
	
	for (i = first_led; i <= last_led; i++) {
		if (cd_get_led(dlog_num,i) == 1)
			return i - first_led;
		}
	return 0;
}

void cd_set_flag(short dlog_num,short item_num,short flag)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	item_flag[item_index] = flag;
	cd_draw_item(dlog_num,item_num);
}


short cd_get_led(short dlog_num,short item_num)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return 0;

	if (item_type[item_index] != 2) {
		beep();
		return 0;
		}
	return item_flag[item_index];
}


void cd_text_frame(short dlog_num,short item_num,short frame)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if (item_index >= 150) {
		beep();
		return;
		}
	item_flag[item_index] = frame;
	cd_draw_item(dlog_num,item_num);
}

void cd_add_label(short dlog_num, short item_num, char *label, short label_flag)
{
	short dlg_index,item_index,label_loc = -1;
	short i;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	if (item_label_loc[item_index] < 0) {
		item_label[item_index] = label_flag;
		for (i = 0; i < 100; i++)
			if (label_taken[i] == FALSE) {
				label_loc = i;
				label_taken[i] = TRUE;
				i = 100;
				}
		if (label_loc < 0) {
			beep();
			return;
			}
		item_label_loc[item_index] = label_loc;
		}
      else cd_erase_item(dlog_num,item_num + 100);
	label_loc = item_label_loc[item_index];
	sprintf((char *) labels[label_loc],"%-24s",label);
	if (item_active[item_index] > 0)
		cd_draw_item(dlog_num,item_num);
}

void cd_take_label(short dlog_num, short item_num)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	item_label[item_index] = 0;
	label_taken[item_label_loc[item_index]] = FALSE;
}

void cd_key_label(short dlog_num, short item_num,short loc)
{
	short dlg_index,item_index;
	char str[10];
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	sprintf((char *) str,"  ");
	str[0] = item_key[item_index];
	cd_add_label(dlog_num,item_num, str, 7 + loc * 100);
}

void cd_draw_item(short dlog_num,short item_num)
{
	short dlg_index,item_index,store_label;
	RGBColor c[3] = {{0,0,0},{65535,0,0},{0,0,8192}};
	Rect from_rect,to_rect;
	GrafPtr old_port;



	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (dlg_draw_ready[dlg_index] == FALSE)
		return;
	
	GetPort(&old_port);
	SetPortWindowPort( dlgs[dlg_index]);
	TextFont(geneva_font_num);
	TextFace(bold);
	TextSize(10);
	
	if (item_active[item_index] == 0) {
		cd_erase_item(dlog_num,item_num);
		cd_erase_item(dlog_num,item_num + 100);
		}
		else {
			switch (item_type[item_index]) {
				case 0: case 1: case 10: case 11:
					GetPortBounds(dlg_buttons_gworld[button_type[item_flag[item_index]]][0], &from_rect);
					rect_draw_some_item(dlg_buttons_gworld[button_type[item_flag[item_index]]][0],from_rect,
					 dlg_buttons_gworld[button_type[item_flag[item_index]]][0],item_rect[item_index],0,2);
					RGBForeColor(&c[2]);
					TextSize(12);
					if (item_type[item_index] < 2)
						OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
					if (item_type[item_index] < 2) {
						char_win_draw_string(dlgs[dlg_index],item_rect[item_index],
						 (char *) (button_strs[item_flag[item_index]]),1,8);
						}
						else {
							char_win_draw_string(dlgs[dlg_index],item_rect[item_index],
							 (char *) ((item_index < 10) ? text_long_str[item_index] : 
							  text_short_str[item_index - 10]),1,8);
							}
					if (item_type[item_index] < 2)
						OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);
					TextSize(10);
					ForeColor(blackColor);
					break;

				case 2:
					GetPortBounds(dlg_buttons_gworld[9][0], &from_rect);
					switch (item_flag[item_index]) {
						case 0: rect_draw_some_item( dlg_buttons_gworld[10][0],from_rect, dlg_buttons_gworld[10][0],item_rect[item_index],0,2); break;
						case 1: rect_draw_some_item( dlg_buttons_gworld[9][1],from_rect, dlg_buttons_gworld[9][1],item_rect[item_index],0,2); break;
						case 2: rect_draw_some_item( dlg_buttons_gworld[9][0],from_rect, dlg_buttons_gworld[9][0],item_rect[item_index],0,2); break;
						}
					break;

				case 3: case 4: case 7: case 8: case 9: 
					cd_erase_item(dlog_num,item_num);
					if (item_type[item_index] == 4)
						TextFace(0);
					if (item_type[item_index] == 7)
						TextSize(12); 
					ForeColor(blackColor);
					if (item_flag[item_index] % 10 == 1)
						cd_frame_item(dlog_num,item_num,2);

					if (item_flag[item_index] >= 10) {
						RGBForeColor(&c[1]);
						}
					if (item_rect[item_index].bottom - item_rect[item_index].top < 20) {
						item_rect[item_index].left += 3;
							char_win_draw_string(dlgs[dlg_index],item_rect[item_index],
							 (char *) ((item_index < 10) ? text_long_str[item_index] : 
							  text_short_str[item_index - 10]),3,12);
						item_rect[item_index].left -= 3;
						}
						else {
							InsetRect(&item_rect[item_index],4,4);
							char_win_draw_string(dlgs[dlg_index],item_rect[item_index],
							 (char *) ((item_index < 10) ? text_long_str[item_index] : 
							  text_short_str[item_index - 10]),0,(item_type[item_index] == 7) ? 14 : 12);
							InsetRect(&item_rect[item_index],-4,-4);
							}
					if ((item_type[item_index] == 8) && (dlog_num == 989)) {
						item_rect[item_index].bottom -= 12;
						undo_clip();
						}
					TextFont(geneva_font_num);
					TextFace(0);
					TextFace(bold);
					TextSize(10); 
					ForeColor(blackColor);
					break;

				case 5:
					if (item_flag[item_index] == -1)
						cd_erase_item(dlog_num,item_num);
						else draw_dialog_graphic(GetWindowPort(dlgs[dlg_index]), item_rect[item_index], item_flag[item_index],
						  item_flag2[item_index], (item_flag[item_index] >= 2000) ? FALSE : TRUE,0);
					break;
				}
			}

			if (item_label[item_index] != 0) {
				store_label = item_label[item_index];
				if (store_label >= 1000) {
					store_label -= 1000;
					}
					else {
						TextFace(0);
						}
				to_rect = item_rect[item_index];
				switch (store_label / 100) {
					case 0:
						to_rect.right = to_rect.left;
						to_rect.left -= 2 * (store_label % 100);
						break;
					case 1:
						to_rect.bottom = to_rect.top;
						to_rect.top -= 2 * (store_label % 100);
						break;
					case 2:
						to_rect.left = to_rect.right;
						to_rect.right += 2 * (store_label % 100);
						break;
					case 3:
						to_rect.top = to_rect.bottom;
						to_rect.bottom += 2 * (store_label % 100);
						break;
					}

				if (to_rect.bottom - to_rect.top < 14) {
					to_rect.bottom += (14 - (to_rect.bottom - to_rect.top)) / 2 + 1;
					to_rect.top -= (14 - (to_rect.bottom - to_rect.top)) / 2 + 1;
					}
					else OffsetRect(&to_rect, 0,(to_rect.bottom - to_rect.top) / 6);
				//cd_erase_rect(dlog_num,to_rect);
				if (item_active[item_index] != 0) {

					char_win_draw_string(dlgs[dlg_index],to_rect,
						labels[item_label_loc[item_index]],2,12);

					}
				TextFace(bold);
				}

	
	TextFont(0);
	TextFace(0);
	TextSize(12);
	SetPort(old_port);
	}

void cd_initial_draw(short dlog_num)
{
	short i,which_dlg = -1;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	if (which_dlg < 0) {
		return;
		}
	dlg_draw_ready[which_dlg] = TRUE;

	cd_erase_item(dlog_num, 0);
	cd_draw(dlog_num);
}

void cd_draw(short dlog_num)
{
	short i,which_dlg = -1;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	if (which_dlg < 0)
		return;
	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++) {
		cd_draw_item(dlog_num,i);
		}
}

void cd_redraw(WindowPtr window)
{
	short which_dlg,dlg_num,dlg_key;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return;
	dlg_draw_ready[which_dlg] = TRUE;
	cd_initial_draw(dlg_num);
}

void cd_frame_item(short dlog_num, short item_num, short width)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	frame_dlog_rect(GetWindowPort(dlgs[dlg_index]), item_rect[item_index], width);
}

void cd_erase_item(short dlog_num, short item_num)
// if item_num is 0, nail whole window
// item_num + 100  just erase label
{
	short i,dlg_index,item_index,store_label;
	Rect to_fry;
	Boolean just_label = FALSE;
	GrafPtr old_port;

	

	if (item_num >= 100) {
		item_num -= 100;
		just_label = TRUE;
		}

	if (item_num == 0) {
		for (i = 0; i < ND; i++)
				if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
					dlg_index = i;
		//GetWindowRect(dlgs[dlg_index],&to_fry);
		GetWindowPortBounds(dlgs[dlg_index],&to_fry);
		//OffsetRect(&to_fry,-1 * to_fry.left,-1 * to_fry.top);
		}
	else {
		if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
			return;
		to_fry = item_rect[item_index];
		if (just_label == TRUE) {
			if (item_label[item_index] != 0) {
				store_label = item_label[item_index];
				if (store_label >= 1000)
					store_label -= 1000;

				switch (store_label / 100) {
					case 0:
						to_fry.right = to_fry.left;
						to_fry.left -= 2 * (store_label % 100);
						break;
					case 1:
						to_fry.bottom = to_fry.top;
						to_fry.top -= 2 * (store_label % 100);
						break;
					case 2:
						to_fry.left = to_fry.right;
						to_fry.right += 2 * (store_label % 100);
						break;
					case 3:
						to_fry.top = to_fry.bottom;
						to_fry.bottom += 2 * (store_label % 100);
						break;
					}
				if ((i = 12 - (to_fry.bottom - to_fry.top)) > 0) {
					// adjust rect ... but doesn't work for bold letters
					to_fry.bottom += i / 2;
               to_fry.bottom++; // extra pixel to get dangly letters
					to_fry.top -= i / 2;
					}
				}
			}
		InsetRect(&to_fry,-1,-1);

		}
	if (dlg_draw_ready[dlg_index] == FALSE) {
		return;
		}
	GetPort(&old_port);
	SetPortWindowPort(dlgs[dlg_index]);
	FillCRect(&to_fry,bg[5]);
	SetPort(old_port);
}

void cd_erase_rect(short dlog_num,Rect to_fry)
{
	short i,dlg_index,item_index,store_label;
	GrafPtr old_port;

	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0)
		return;
	if (dlg_draw_ready[dlg_index] == FALSE)
		return;
	
	GetPort(&old_port);
	SetPortWindowPort(dlgs[dlg_index]);
	FillCRect(&to_fry,bg[5]);
	SetPort(old_port);
}

void cd_press_button(short dlog_num, short item_num)
{
	short dlg_index,item_index;
	unsigned long dummy;
	Rect from_rect,to_rect,x = {0,0,55,55};
	GrafPtr old_port;
	RGBColor c[2] = {{0,0,4096},{0,0,8192}};

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	// no press action for redio buttons
	if (item_type[item_index] == 2) {
		play_sound(34);
		return;
		}	
		
	GetPort(&old_port);
	SetPortWindowPort( dlgs[dlg_index]);
	TextFont(geneva_font_num);
	TextFace(bold);
	TextSize(10);


	ForeColor(blackColor);
	TextSize(12);
	GetPortBounds(dlg_buttons_gworld[button_type[item_flag[item_index]]][0], &from_rect);
	rect_draw_some_item(dlg_buttons_gworld[button_type[item_flag[item_index]]][1],from_rect,
	 dlg_buttons_gworld[button_type[item_flag[item_index]]][1],item_rect[item_index],0,2);
	TextFace(bold);
	RGBForeColor(&c[0]);
	if (item_type[item_index] < 2) {
		OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
		char_win_draw_string(dlgs[dlg_index],item_rect[item_index],
		 (char *) (button_strs[item_flag[item_index]]),1,8);
		OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);
		}
		else {
			char_win_draw_string(dlgs[dlg_index],item_rect[item_index],
			 (char *) ((item_index < 10) ? text_long_str[item_index] : 
			  text_short_str[item_index - 10]),1,8);
			}

	if (play_sounds == TRUE) {
		play_sound(37);
		Delay(6,&dummy);
		}
		else Delay(14,&dummy);

	rect_draw_some_item(dlg_buttons_gworld[button_type[item_flag[item_index]]][0],from_rect,
	 dlg_buttons_gworld[button_type[item_flag[item_index]]][0],item_rect[item_index],0,2);

	RGBForeColor(&c[1]);
	if (item_type[item_index] < 2) {
		OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
		char_win_draw_string(dlgs[dlg_index],item_rect[item_index],
		 (char *) (button_strs[item_flag[item_index]]),1,8);
		OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);
		}
		else {
			char_win_draw_string(dlgs[dlg_index],item_rect[item_index],
			 (char *) ((item_index < 10) ? text_long_str[item_index] : 
			  text_short_str[item_index - 10]),1,8);
			}

	TextFont(0);
	TextFace(0);
	TextSize(12);
	ForeColor(blackColor);
	Delay(8,&dummy);
	
	SetPort(old_port);
	}

// LOW LEVEL

short cd_get_indices(short dlg_num, short item_num, short *dlg_index, short *item_index)
{
	if ((*dlg_index = cd_get_dlg_index(dlg_num)) < 0) {
		return -1;
		}
	if ((*item_index = cd_get_item_id(dlg_num,item_num)) < 0) {
		return -1;
		}
	return 0;
		}

short cd_get_dlg_index(short dlog_num)
{
	short i;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			return i;
	return -1;
}

short cd_find_dlog(WindowPtr window, short *dlg_num, short *dlg_key)
{
	short i;
	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlgs[i] == window)) {
			*dlg_num = dlg_types[i];
			*dlg_key = dlg_keys[i];
			return i;
			}
	return -1;
}

short cd_get_item_id(short dlg_num, short item_num)
{
	short i;

	for (i = 0; i < NI; i++)
		if ((item_dlg[i] == dlg_num) && (item_number[i] == item_num))
			return i;
	return -1;
}

// External graphics tools (huh huh huh ... tool ... huh huh huh)

void center_window(WindowPtr window)
{
// if this is needed, see adjust_window_mode
}

Rect get_item_rect(WindowPtr hDlg, short item_num)
{
	short the_type;
	Handle the_handle = NULL;
	Rect small_rect;
	short item_hit,what_talk_field;

	GetDialogItem( GetDialogFromWindow(hDlg), item_num, &the_type, &the_handle, &small_rect);
	
	return small_rect;
}


void frame_dlog_rect(GrafPtr hDlg, Rect rect, short val)
{
	//COLORREF x = RGB(220,220,200),y = RGB(128,128,128);
	RGBColor lt_gray = {57344,57344,57344},dk_gray = {12287,12287,12287},med_gray = {24574,24574,24574};
	GrafPtr old_port;

	
	GetPort(&old_port);
	SetPort(hDlg);
	
	InsetRect(&rect,-1 * val,-1 * val);

	RGBForeColor(&dk_gray);
	MoveTo(rect.left,rect.top);
	LineTo(rect.right,rect.top);
	if (hDlg == GetWindowPort(mainPtr))
		RGBForeColor(&med_gray);
		else RGBForeColor(&lt_gray);
	LineTo(rect.right,rect.bottom);
	LineTo(rect.left,rect.bottom);
	RGBForeColor(&dk_gray);
	LineTo(rect.left,rect.top);
	ForeColor(blackColor);
	SetPort(old_port);
}

void draw_dialog_graphic(GrafPtr hDlg, Rect rect, short which_g, short type_g, Boolean do_frame,short win_or_gworld)
// win_or_gworld: 0 - window  1 - gworld
// 0 - 300   number of terrain graphic -> DLG_TER_TYPE (1), DLG_TER_ANIM_TYPE (2)
// 400 + x - monster graphic num (uses index from scenario) -> DLG_MONST_TYPE
//   400 is 1st monster from monster index, 401 is 2nd from m. index, and so on
// 700 + x  dlog graphic -> DLG_DLG_TYPE
// 800 + x  pc graphic -> DLG_PC_TYPE
// 900 + x  B&W graphic -> DLG_BW_TYPE
// 950 null item -> 0, DLG_BLANK_TYPE (0)
// 1000 + x  Talking face -> DLG_TALK_TYPE
// 1100 - item info help -> DLG_INFO_TYPE
// 1200 - pc screen help -> DLG_PC_HELP_TYPE
// 1300 - combat ap -> DLG_COMBAT_AP_TYPE
// 1400-1402 - button help -> DLG_HELP_TYPE
// 1500 - stat symbols help -> DLG_STAT_TYPE
// 1600 + x - scen graphics -> DLG_SCEN_TYPE
// 1700 + x - anim graphic -> DLG_ANIM_TYPE
// 1800 + x  item graphic -> DLG_ITEM_TYPE
{
	short picnum;
	Rect from1 = {0,0,36,28},from2 = {0,0,36,36},from3 = {0,0,72,72},tiny_obj_rect = {0,0,18,18};
	Rect from_rect = {0,0,36,28};
	Rect face_from = {0,0,32,32};
	Rect death_to = {6,6,78,78};
	Rect to_rect = {6,6,42,42};
	Rect m_to_rect = {6,10,42,38};
	Rect bw_from = {0,0,120,120};
	Rect map_from = {0,0,240,240};
	
	Rect pc_info_from = {130,0,157,106};
	Rect item_info_from = {0,174,113,312};
	Rect button_help_from = {0,0,125,320};
	Rect combat_ap_from = {0,0,100,275};
	Rect stat_symbols_from = {0,0,94,386};
	Rect small_monst_rect = {0,0,18,14};
	
	RGBColor store_color;
	GWorldPtr from_gworld;
	short draw_dest = 2,m_start_pic;
	
	if (win_or_gworld == 1)
		draw_dest = 0;
		
	if (which_g < 0)
		return;

	if (which_g >= 2000)
		do_frame = FALSE;
	which_g = which_g % 2000;
	
	if (type_g == DLG_BLANK_TYPE) { // Empty. Maybe clear space.
		if (win_or_gworld == 0) {
			rect.top -= 3;
			rect.left -= 3;
			rect.bottom += 3;
			rect.right += 3;
			FillCRect(&rect,bg[5]);
			}	
		return;
		}
	GetBackColor(&store_color);
	
	BackColor(whiteColor);	

	switch (type_g) {
		case DLG_TER_TYPE: // terrain
			from_gworld = terrain_gworld[which_g / 50];
			which_g = which_g % 50;
			from_rect = calc_rect(which_g % 10, which_g / 10);
			if (rect.right - rect.left > 28) 
				InsetRect(&rect,4,0);
			rect_draw_some_item(from_gworld,from_rect,(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr) (hDlg): from_gworld)
			  ,rect,0,draw_dest);
			//DisposeGWorld(from_gworld);
			break;
		case DLG_TER_ANIM_TYPE: // animated terrain
			//which_g -= 300;
			//from_gworld = load_pict(820);
			from_rect = calc_rect(4 * (which_g / 5), which_g % 5);
			if (rect.right - rect.left > 28) {
				InsetRect(&rect,4,0);
				rect.right = rect.left + 28;
				}
			rect_draw_some_item(anim_gworld,from_rect,(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr)hDlg: anim_gworld)
			  ,rect,0,draw_dest);
			//DisposeGWorld(from_gworld);
			break;
		case DLG_MONST_TYPE: // monster ... needs use index   small_monst_rect
			// There are 4 different ways to draw, depending on size of monster
			//which_g -= 400;
			m_start_pic = m_pic_index[which_g];
			
			if ((m_pic_index_x[which_g] == 1) && (m_pic_index_y[which_g] == 1)) {
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0)
					PaintRect(&rect);
				rect_draw_some_item(from_gworld,from_rect,
				(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr) (hDlg) : from_gworld)
				  ,rect,1,draw_dest);
			  	}
			if ((m_pic_index_x[which_g] == 2) && (m_pic_index_y[which_g] == 1)) {
				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0)
					PaintRect(&rect);
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,rect.left,rect.top + 7);
				if (win_or_gworld == 0)
					PaintRect(&small_monst_rect);
				rect_draw_some_item(from_gworld,from_rect,
				(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr) (hDlg) : from_gworld)
				  ,small_monst_rect,1,draw_dest);
				  
				m_start_pic = m_pic_index[which_g] + 1;
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,14,0);
				if (win_or_gworld == 0)
					PaintRect(&small_monst_rect);
				rect_draw_some_item(from_gworld,from_rect,
				(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr) (hDlg) : from_gworld)
				  ,small_monst_rect,1,draw_dest);
			  	}
			if ((m_pic_index_x[which_g] == 1) && (m_pic_index_y[which_g] == 2)) {
				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0)
					PaintRect(&rect);
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,rect.left + 7,rect.top);
				if (win_or_gworld == 0)
					PaintRect(&small_monst_rect);
				rect_draw_some_item(from_gworld,from_rect,
				(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr) (hDlg) : from_gworld)
				  ,small_monst_rect,1,draw_dest);
				  
				m_start_pic = m_pic_index[which_g] + 1;
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,0,18);
				if (win_or_gworld == 0)
					PaintRect(&small_monst_rect);
				rect_draw_some_item(from_gworld,from_rect,
				(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr) (hDlg) : from_gworld)
				  ,small_monst_rect,1,draw_dest);
			  	}
			if ((m_pic_index_x[which_g] == 2) && (m_pic_index_y[which_g] == 2)) {
				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0)
					PaintRect(&rect);
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,rect.left,rect.top);
				if (win_or_gworld == 0)
					PaintRect(&small_monst_rect);
				rect_draw_some_item(from_gworld,from_rect,
				(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr) (hDlg) : from_gworld)
				  ,small_monst_rect,1,draw_dest);
				  
				m_start_pic = m_pic_index[which_g] + 1;
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,14,0);
				if (win_or_gworld == 0)
					PaintRect(&small_monst_rect);
				rect_draw_some_item(from_gworld,from_rect,
				(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr) (hDlg) : from_gworld)
				  ,small_monst_rect,1,draw_dest);
				  
				m_start_pic = m_pic_index[which_g] + 2;
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,-14,18);
				if (win_or_gworld == 0)
					PaintRect(&small_monst_rect);
				rect_draw_some_item(from_gworld,from_rect,
				(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr) (hDlg) : from_gworld)
				  ,small_monst_rect,1,draw_dest);
				m_start_pic = m_pic_index[which_g] + 3;
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,14,0);
				if (win_or_gworld == 0)
					PaintRect(&small_monst_rect);
				rect_draw_some_item(from_gworld,from_rect,
				(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr) (hDlg) : from_gworld)
				  ,small_monst_rect,1,draw_dest);
			  	}
			  
			
			//DisposeGWorld(from_gworld);
			break;

		case DLG_ITEM_TYPE: // item
			//which_g -= 1800;
			rect.right = rect.left + 28; rect.bottom = rect.top + 36;
			to_rect = rect;
			if (win_or_gworld == 0)
				PaintRect(&rect);
			if (which_g < 45) {
				from_gworld = items_gworld;
				from_rect = calc_rect(which_g % 5, which_g / 5);
				}
				else {
					from_gworld = tiny_obj_gworld;
					//rect.top += 4; rect.bottom -= 4;
					InsetRect(&to_rect,5,9);
					from_rect = tiny_obj_rect;
					OffsetRect(&from_rect,18 * (which_g % 10), 18 * (which_g / 10));
					}
			rect_draw_some_item(from_gworld,from_rect,(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr)hDlg: from_gworld)
			  ,to_rect,1,draw_dest);
			break;
		case DLG_DLG_TYPE: // dialog
			//which_g -= 700;
			from_gworld = dlogpics_gworld;
			OffsetRect(&from2,36 * (which_g % 4),36 * (which_g / 4));
			rect_draw_some_item(from_gworld,from2,(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr)hDlg: from_gworld)
			  ,rect,0,draw_dest);
			break;

		case DLG_TALK_TYPE: // talk face
			rect.right = rect.left + 32; rect.bottom = rect.top + 32;
			//which_g -= 1000;
			from_gworld = talkfaces_gworld;
			from_rect = face_from;
			OffsetRect(&from_rect,32 * ((which_g - 1) % 10),32 * ((which_g - 1) / 10));
			if (win_or_gworld == 1)
				 rect_draw_some_item(from_gworld,from_rect,(GWorldPtr) hDlg,rect,0,0);
				else rect_draw_some_item(from_gworld,from_rect,from_gworld,rect,0,draw_dest);
			break;
		case DLG_SCEN_TYPE:
			//which_g -= 1600;
			from_gworld = load_pict(851);
			from_rect.right = 32;
			from_rect.bottom = 32;
			OffsetRect(&from_rect,32 * (which_g % 5),32 * (which_g / 5));
			rect.right = rect.left + 32;
			rect.bottom = rect.top + 32;
			rect_draw_some_item(from_gworld,from_rect,(GWorldPtr) ((win_or_gworld == 1) ? (GWorldPtr)hDlg: from_gworld)
			  ,rect,0,draw_dest);
			DisposeGWorld(from_gworld);
			break;
	
		}

	if (do_frame == TRUE) {
		rect.bottom--; rect.right--;
		frame_dlog_rect(hDlg,rect,3);
		}
	RGBBackColor(&store_color);
}



Rect calc_rect(short i, short j)
{
	Rect base_rect = {0,0,36,28};
	
	OffsetRect(&base_rect,i * 28, j * 36);
	return base_rect;
}

