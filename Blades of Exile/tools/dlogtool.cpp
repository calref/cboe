#include <windows.h>
#include <cstring>
#include "../global.h"
#include "../boe.graphutil.h"
#include <cstdio>
#include "soundtool.h"
#include "dlogtool.h"
#include "../boe.text.h"
#include "../boe.locutils.h"
#include "../boe.dlgutil.h"
#include "../boe.infodlg.h"
#include "mathutil.h"

#include "../globvar.h"

/*
struct Dialog
{
	short key;
	short type;
	HWND handle;
	HWND parent;
	short highest_item;
	Boolean draw_ready;

	Dialog() : key(-1), type(0), handle(NULL), highest_item(0) { }
} dialogs[ND];
*/

short current_key = 0;
short dlg_keys[ND];
short dlg_types[ND];
HWND dlgs[ND];
HWND dlg_parent[ND];
short dlg_highest_item[ND];
Boolean dlg_draw_ready[ND];

short item_dlg[NI];
short item_number[NI];
char item_type[NI];
RECT item_rect[NI];
short item_flag[NI];
char item_active[NI];
char item_key[NI];
short item_label[NI];
short item_label_loc[NI];

inline void cd_kill_dc(short which_slot,HDC hdc) { fry_dc(dlgs[which_slot],hdc); }

LRESULT CALLBACK dummy_dialog_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK fresh_edit_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

void cd_init_dialogs()
{
	short i;

	for (i = 0; i < ND; i++)
	{
		dlg_keys[i] = -1;
		dlg_types[i] = 0;
		dlgs[i] = NULL;
		dlg_highest_item[i] = 0;
	}
	for (i = 0; i < NI; i++) item_dlg[i] = -1;
	for (i = 0; i < NL; i++) label_taken[i] = false;

	d_proc = dummy_dialog_proc;
	edit_proc = fresh_edit_proc;
}

LRESULT CALLBACK fresh_edit_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(hwnd == talk_edit_box){
    switch (message) {
        case WM_KEYDOWN:
			if (wParam == VK_RETURN){// talk edit box
                POINT p = {-1,-1};
           		GetWindowText(talk_edit_box,talk_edit_string,40);
                SetWindowText(talk_edit_box,"");
           		FlushEvents(0);
           		handle_talk_event(p);
           		SetFocus(talk_edit_box);
                }
			break;
        }
    }
    else{
	switch (message) {
		case WM_KEYDOWN:
			if (wParam == VK_RETURN)
				SendMessage(
					store_edit_parent,
					WM_COMMAND,
					9,
					0
					);
			break;
			if (wParam == VK_ESCAPE)
				SendMessage(store_edit_parent,
					WM_COMMAND,
					8,
					0);
			break;
		}
    }

	return CallWindowProc((WNDPROC) old_edit_proc,hwnd,message,wParam,lParam);
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

short cd_create_custom_pic_dialog(HWND parent, HBITMAP picture)
{

	short i,j,free_slot = -1,free_item = -1;

	short cur_item = 1, but_items;
	RECT pic_rect = {0,0,0,0}, c_rect;

	if (parent != NULL) {
		if (IsWindowEnabled(parent) == 0)
			return -1;
		}
    GetObject(picture, sizeof(bitmap_info), &bitmap_info);

    GetClientRect(mainPtr, &c_rect);

    if(bitmap_info.bmWidth > c_rect.right)
        bitmap_info.bmWidth = c_rect.right;

    if(bitmap_info.bmHeight > c_rect.bottom)
        bitmap_info.bmHeight = c_rect.bottom - 60;

    pic_rect.right = bitmap_info.bmWidth;
    pic_rect.bottom = bitmap_info.bmHeight;

	store_dlog_num = 905;

	for (i = 0; i < ND; i++) {
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == 905)) //custom_pic_dialog is number 905
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
	dlg_types[free_slot] = 905;
	dlg_highest_item[free_slot] = 1;
	dlg_draw_ready[free_slot] = false;
	dlgs[free_slot] = NULL;

	// first, create dummy dlog
	store_free_slot = free_slot;

	dlgs[free_slot] = CreateWindow (szWinName,
			"Blades of Exile",
			0,// was visible
			0,
			0,
			bitmap_info.bmWidth + 2,
			bitmap_info.bmHeight + 65, // additional space for the ok button
			NULL,
			NULL,
			store_hInstance,
			NULL);
	if (dlgs[free_slot] == NULL) {
		play_sound(3);
		return -3;
		}

	dlg_parent[free_slot] = parent;

	//process_new_window (dlgs[free_slot]);
	// instead of this, custom make items
	free_item = -1;

    	for (j = 150; j < NI; j++)
				if (item_dlg[j] < 0) {
					free_item = j;
					j = NI + 1;
					}
			item_dlg[free_item] = store_dlog_num;
			item_type[free_item] = 1;
			item_number[free_item] = cur_item;

			item_flag[free_item] = available_dlog_buttons[1];
			item_active[free_item] = 1;
			item_label[free_item] = 0;
				item_label_loc[free_item] = -1;
				item_key[free_item] = 31;
				but_items = free_item; // remember this to set item rect later
				cur_item++;

	item_rect[but_items].right = bitmap_info.bmWidth - 10;
	item_rect[but_items].left = item_rect[but_items].right - 63;
	item_rect[but_items].top = bitmap_info.bmHeight + 5;
	item_rect[but_items].bottom = item_rect[but_items].top + 23;

  	for (j = 150; j < NI; j++)
		if (item_dlg[j] < 0) {
			free_item = j;
			j = NI + 1;
			}
		item_dlg[free_item] = store_dlog_num;
		item_number[free_item] = cur_item;
		item_type[free_item] = 20; //draw loaded picture
		item_active[free_item] = 1;
        cur_item++;

	dlg_highest_item[free_slot] = cur_item - 1;

	center_window(dlgs[free_slot]);
	ShowWindow(dlgs[free_slot],SW_SHOW);

	if (dlg_parent[free_slot] != NULL) {
		EnableWindow(dlg_parent[free_slot],false);
		if (dlg_parent[free_slot] == mainPtr)
			for (i = 0; i < 18; i++)
				if (modeless_exists[i] == true)
					EnableWindow(modeless_dialogs[i],false);
		}
	dialog_not_toast = true;
	return 0;
}

short cd_create_custom_dialog(HWND parent,
	char *str_block,short pic_num,short buttons[3])
{

	short i,j,free_slot = -1,free_item = -1,str_width,cur_but_right = 0;
	short total_len = 0;

	char strs[6][256];

	short cur_item = 1;
	short but_items[3] = {-1,-1,-1};
	RECT measure_rect,pic_rect = {8,8,44,44},cur_text_rect = {50,2,0,0};
	short win_width = 100, win_height = 100;

	for (i = 0; i < 256; i++) {
		strs[0][i] = str_block[i];
		strs[1][i] = str_block[i + 256];
		strs[2][i] = str_block[i + 512];
		strs[3][i] = str_block[i + 768];
		strs[4][i] = str_block[i + 1024];
		strs[5][i] = str_block[i + 1280];
		}

	if (parent != NULL) {
		if (IsWindowEnabled(parent) == 0)
			return -1;
		}
	store_dlog_num = 900;

	for (i = 0; i < ND; i++) {
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == 900))
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

	// quick check, to make sure there's at least 1 button
	if ((buttons[0] < 0) && (buttons[1] < 0) && (buttons[2] < 0))
		buttons[0] = 1;
	current_key++;
	dlg_keys[free_slot] = current_key;
	dlg_types[free_slot] = 900;
	dlg_highest_item[free_slot] = 1;
	dlg_draw_ready[free_slot] = false;
	dlgs[free_slot] = NULL;

	// first, create dummy dlog
	store_free_slot = free_slot;

	dlgs[free_slot] = CreateWindow (szWinName,
			"Blades of Exile",
			0,// was visible
			0,
			0,
			100,
			100,
			NULL,
			NULL,
			store_hInstance,
			NULL);
	if (dlgs[free_slot] == NULL) {
		play_sound(3);
		return -3;
		}

	dlg_parent[free_slot] = parent;

	//process_new_window (dlgs[free_slot]);
	// instead of this, custom make items
	free_item = -1;

	// first, do 1-3 buttons
	for (i = 0; i < 3; i++)
		if (buttons[i] >= 0) {// buttons
		for (j = 150; j < NI; j++)
				if (item_dlg[j] < 0) {
					free_item = j;
					j = NI + 1;
					}
			item_dlg[free_item] = store_dlog_num;
			item_type[free_item] = (i == 0) ? 1 : 0;
			item_number[free_item] = cur_item;

			item_flag[free_item] = available_dlog_buttons[buttons[i]];
			item_active[free_item] = 1;
			item_label[free_item] = 0;
				item_label_loc[free_item] = -1;
				item_key[free_item] = button_def_key[available_dlog_buttons[buttons[i]]];
				if (i == 0)
					item_key[free_item] = 31;
				but_items[i] = free_item; // remember this to set item rect later
				cur_item++;
				}
	// next, the upper left picture (always there)
	for (j = 150; j < NI; j++)
		if (item_dlg[j] < 0) {
			free_item = j;
			j = NI + 1;
			}
	item_dlg[free_item] = store_dlog_num;
	item_type[free_item] = 5;
	item_number[free_item] = cur_item;
	item_rect[free_item] = pic_rect;

	if (pic_num < 0) {
		item_flag[free_item] = pic_num * -1;
		}
		else //if (pic_num < 1000)
			item_flag[free_item] = pic_num;
	item_active[free_item] = 1;
	item_label[free_item] = 0;
	 item_label_loc[free_item] = -1;
	 item_key[free_item] = 0;
	 cur_item++;

	// finally, 0-6 text, first do preprocessing to find out how much room we need
	for (i = 0; i < 6; i++)
		total_len += string_length((char *) strs[i],main_dc);
	total_len = total_len * 12;
	str_width = 100 + 20;
	if (str_width < 340)
		str_width = 340;
	cur_text_rect.right = cur_text_rect.left + str_width;
	// finally, 0-6 text, then create the items
	for (i = 0; i < 6; i++)
		if (strlen((char *) strs[i]) > 0) {// text
			for (j = 0; j < 10; j++)
				if (item_dlg[j] < 0) {
					free_item = j;
					j = NI + 1;
					}
			item_dlg[free_item] = store_dlog_num;
			item_type[free_item] = 9;
			item_number[free_item] = cur_item;
			item_rect[free_item] = cur_text_rect;
			measure_rect.top = 0; measure_rect.bottom = 0;
			measure_rect.left = 0; measure_rect.right = 340;
			DrawText(main_dc, (char *) strs[i],strlen((char *) strs[i]),
				&measure_rect,DT_CALCRECT | DT_WORDBREAK);
			item_rect[free_item].bottom = item_rect[free_item].top +
				(measure_rect.bottom - measure_rect.top) + 16;

			cur_text_rect.top = item_rect[free_item].bottom + 8;
			item_flag[free_item] = 0;
			item_active[free_item] = 1;
			item_label[free_item] = 0;
           	item_label_loc[free_item] = -1;
            item_key[free_item] = 0;
 			sprintf(text_long_str[free_item],"%s",
				(char *) strs[i]);
      		cur_item++;
       		}

	dlg_highest_item[free_slot] = cur_item - 1;

	// finally, do button rects
	cur_but_right = cur_text_rect.right + 30;
	//cur_text_rect.top += 8;
	for (i = 0; i < 3; i++)
		if (buttons[i] >= 0) {
			item_rect[but_items[i]].right = cur_but_right;
			item_rect[but_items[i]].top = cur_text_rect.top;
			item_rect[but_items[i]].bottom = item_rect[but_items[i]].top + 23;
			if (button_type[available_dlog_buttons[buttons[i]]] == 1)
				item_rect[but_items[i]].left = item_rect[but_items[i]].right - 63;
				else item_rect[but_items[i]].left = item_rect[but_items[i]].right - 110;
			cur_but_right = item_rect[but_items[i]].left - 4;
			if (i == 0) {
				win_width = item_rect[but_items[i]].right + 6;
				win_height = item_rect[but_items[i]].bottom + 6;
				}
			}

	win_height += 28;
	win_width += 3;

	MoveWindow(dlgs[free_slot],0,0,win_width,win_height,false);
	center_window(dlgs[free_slot]);
	ShowWindow(dlgs[free_slot],SW_SHOW);

	if (dlg_parent[free_slot] != NULL) {
		EnableWindow(dlg_parent[free_slot],false);
		if (dlg_parent[free_slot] == mainPtr)
			for (i = 0; i < 18; i++)
				if (modeless_exists[i] == true)
					EnableWindow(modeless_dialogs[i],false);
		}
	dialog_not_toast = true;
	return 0;
}

short cd_create_dialog(short dlog_num, HWND parent)
{
	short i, free_slot = -1;
	HWND dlg;

	if (parent != NULL) {
		if (IsWindowEnabled(parent) == 0) return -1;
		}

	store_dlog_num = dlog_num;
	store_parent = parent;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num)) return -1;

	for (i = 0; i < ND; i++) {
		if (dlg_keys[i] < 0) {
			free_slot = i;
			break;
			}
		}
	if (free_slot < 0) return -2;

	current_key++;

	dlg_keys[free_slot] = current_key;
	dlg_types[free_slot] = dlog_num;
	dlg_highest_item[free_slot] = 1;
	dlg_draw_ready[free_slot] = false;
	dlgs[free_slot] = NULL;

	// first, create dummy dlog
	store_free_slot = free_slot;

	dlg = CreateDialog(
			(HINSTANCE) store_hInstance,
			MAKEINTRESOURCE(dlog_num),
			0,
			(DLGPROC) d_proc
			);

	if (dlgs[free_slot] == NULL)
	{
		play_sound(3);
		DebugQuit("Couldn't create Dialog Box");
		return -3;
	}

	center_window(dlgs[free_slot]);

	dlg_parent[free_slot] = parent;

	switch (dlog_num) {
		case 958: SetWindowText(dlgs[free_slot],"Tip of the Day"); break;
		case 960: SetWindowText(dlgs[free_slot],"Talking Notes"); break;
		case 961: SetWindowText(dlgs[free_slot],"Adventure Notes"); break;
		case 962: SetWindowText(dlgs[free_slot],"Adventure Journal"); break;
		case 970: case 971: case 972: case 973: SetWindowText(dlgs[free_slot],"Blades of Exile"); break;
		case 987: SetWindowText(dlgs[free_slot],"Getting Items"); break;
		case 988: SetWindowText(dlgs[free_slot],"Casting Simulacrum"); break;
		case 989: SetWindowText(dlgs[free_slot],"Create a Party"); break;
		case 991: SetWindowText(dlgs[free_slot],"Character Statistics"); break;
		case 996: SetWindowText(dlgs[free_slot],"Known Alchemy"); break;
		case 997: SetWindowText(dlgs[free_slot],"Getting Help"); break;
		case 998: SetWindowText(dlgs[free_slot],"Item Information"); break;
		case 999: SetWindowText(dlgs[free_slot],"Monster Stats"); break;
		case 1012: SetWindowText(dlgs[free_slot],"How many?"); break;
		case 1013: SetWindowText(dlgs[free_slot],"Race/Advantages/Disadvantages"); break;
		case 1010: SetWindowText(dlgs[free_slot],"Training a PC"); break;
		case 1014: SetWindowText(dlgs[free_slot],"Reading a Sign"); break;
		case 1017: SetWindowText(dlgs[free_slot],"Blades of Exile"); break;
		case 1018: SetWindowText(dlgs[free_slot],"Select a PC"); break;
		case 1019: SetWindowText(dlgs[free_slot],"Character Statistics"); break;
		case 1020: SetWindowText(dlgs[free_slot],"In a Shop"); break;
		case 1021: SetWindowText(dlgs[free_slot],"Buying Food"); break;
		case 1047: SetWindowText(dlgs[free_slot],"Select Alchemy"); break;
		case 1048: SetWindowText(dlgs[free_slot],"Visiting the Healer"); break;
		case 1050: SetWindowText(dlgs[free_slot],"Pick PC Graphic"); break;
		case 1051: SetWindowText(dlgs[free_slot],"Pick PC Name"); break;
		case 1096: SetWindowText(dlgs[free_slot],"Spells Help"); break;
		case 1097: SetWindowText(dlgs[free_slot],"Skills Help"); break;
		case 1098: SetWindowText(dlgs[free_slot],"Cast a Spell"); break;
		case 1099: SetWindowText(dlgs[free_slot],"Preferences"); break;
		case 1100: SetWindowText(dlgs[free_slot],"Compatibility Options"); break;
		default: SetWindowText(dlgs[free_slot],"Blades of Exile"); break;
			}

	ShowWindow(dlgs[free_slot],SW_SHOW);
	DestroyWindow(dlg); //Necessary? Dunno.

	if (dlg_parent[free_slot] != NULL)
	{
		EnableWindow(dlg_parent[free_slot], false);

		if (dlg_parent[free_slot] == mainPtr)
			for (i = 0; i < 18; i++)
				if (modeless_exists[i] == true) EnableWindow(modeless_dialogs[i],false);
	}
	dialog_not_toast = true;
	return 0;
}

LRESULT CALLBACK dummy_dialog_proc (HWND hDlg, UINT message, WPARAM, LPARAM)
{
	short i,j,k,free_slot = -1,free_item = -1;
	int type, flag;
	char item_str[256];
	Boolean str_stored = false;
	RECT dlg_rect;
	short win_height = 0, win_width = 0;
	short str_offset = 1;

	free_slot = store_free_slot;

	switch (message)
	{
		case WM_INITDIALOG:
			// now, make a window, matching dialog
			GetWindowRect(hDlg,&dlg_rect);
			dlgs[store_free_slot] = CreateWindow(
				szWinName,
				"Blades of Exile Dialog",
				0,// was visible
				0,
				0,
				dlg_rect.right - dlg_rect.left,
				dlg_rect.bottom - dlg_rect.top,
				NULL,
				NULL,
				store_hInstance,
				NULL);

		// Now, give the window its items
		for (i = 0; i < 200; i++)
			if (GetDlgItem(hDlg,i) != NULL)
			{
				GetDlgItemText(hDlg,i,item_str,256);
	      		str_offset = 1;
				dlg_highest_item[free_slot] = i;
				str_stored = false;

				if (strlen((char *)item_str) == 0)
				{
					sprintf((char *) item_str, "+");
					type = 3;
					flag = 0;
		            str_stored = true;
				}
				else if (item_str[0] == '+') { // default is framed text
					type = 3;
					flag = 1;
	            	str_stored = true;
					}
				else if (item_str[0] == '*') {
					type = 3;
					flag = 0;
					str_stored = true;
					}
				else if (item_str[0] == '~') {
					type = 7;
					flag = 0;
					str_stored = true;
					}
				else if (item_str[0] == '!') {
					type = 4;
					flag = 0;
					str_stored = true;
					}
				else if (item_str[0] == '=') {
					type = 9;
					flag = 1;
					str_stored = true;
					}
				else if (((item_str[0] >= 65) && (item_str[0] <= 122)) || (item_str[0] == '"')) {
					type = 9;
					flag = 0;
					str_offset = 0;
					str_stored = true;
					}
				else if ((item_str[0] == '^') || (item_str[0] == '&')) {
					type = (item_str[0] == '^') ? 10 : 11;
					flag = 1;
					str_stored = true;
					}
				else sscanf(item_str,"%d_%d",&type,&flag);

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
						 ((str_stored == true) && (strlen((char *) item_str) > 35))) {
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
						item_number[free_item] = i;

						item_rect[free_item] = get_item_rect(hDlg,i);
						item_rect[free_item].top = item_rect[free_item].top / 2;
						item_rect[free_item].left = item_rect[free_item].left / 2;
						item_rect[free_item].bottom = item_rect[free_item].bottom / 2;
						item_rect[free_item].right = item_rect[free_item].right / 2;

						if ((type != 5) && ((store_dlog_num >= 2000) || (store_dlog_num == 986))) {
							item_rect[free_item].top =
								(item_rect[free_item].top * 11) / 10;
							item_rect[free_item].bottom =
								(item_rect[free_item].bottom * 11) / 10;
							}

						item_flag[free_item] = flag;
						item_active[free_item] = 1;
						item_label[free_item] = 0;
	            	    item_label_loc[free_item] = -1;
	               		item_key[free_item] = 0;

						switch (type) {
							case 0: case 1:
								if (item_flag[free_item] != 143) {
									item_rect[free_item].right = item_rect[free_item].left + button_width[button_type[flag]];
									item_rect[free_item].bottom = item_rect[free_item].top + button_height[button_type[flag]];

									item_key[free_item] = button_def_key[flag];
									if (type == 1)
										item_key[free_item] = 31;
							  		}
								break;
							case 2:
								item_rect[free_item].right = item_rect[free_item].left + 14;
								item_rect[free_item].bottom = item_rect[free_item].top + 10;
										item_key[free_item] = 255;
								break;
							case 3: case 4: case 7: case 8: case 9: case 10: case 11:
								sprintf(((free_item < 10) ? text_long_str[free_item] : text_short_str[free_item - 10]),"");
								if (str_stored == true) {
									if (free_item < 10) {
										sprintf(text_long_str[free_item],"%s",
										  (char *) (item_str + str_offset));
										for (k = 0; k < 256; k++) {
											if (text_long_str[free_item][k] == '|')
												 text_long_str[free_item][k] = 13;
											if (text_long_str[free_item][k] == '_')
												 text_long_str[free_item][k] = '"';
											}
										}
									else {
										sprintf(text_short_str[free_item - 10],"%-34s",
										  (char *) (item_str + str_offset));
										for (k = 0; k < 35; k++) {
											if (text_short_str[free_item][k] == '|')
												 text_short_str[free_item][k] = 13;
											if (text_short_str[free_item][k] == '_')
												 text_short_str[free_item][k] = '"';
											}
											}
									}
								item_key[free_item] = 255;
								break;
							case 6:
								edit_box = CreateWindow("edit",NULL,WS_CHILD | WS_BORDER | WS_VISIBLE,
									item_rect[free_item].left,item_rect[free_item].top,
									item_rect[free_item].right - item_rect[free_item].left,
									item_rect[free_item].bottom - item_rect[free_item].top,
									dlgs[free_slot],(HMENU) 150,(HINSTANCE) store_hInstance,NULL);
								store_edit_parent =  dlgs[free_slot];
								old_edit_proc = (WNDPROC) (GetWindowLongPtr(edit_box,GWLP_WNDPROC));
								SetWindowLongPtr(edit_box,GWLP_WNDPROC,(LONG_PTR) edit_proc);
								break;
							}
						win_height = max(win_height, item_rect[free_item].bottom + 28 + 6);
						win_width = max(win_width, item_rect[free_item].right + 11);
					}
				}
				MoveWindow(dlgs[free_slot],0,0,win_width,win_height,false);
				EndDialog(hDlg, 0);
				return true;
	} /* end of switch */

	return true;
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
			if (item_type[i] == 6)  {
				DestroyWindow(edit_box);
				edit_box = NULL;
				}
			if (item_label[i] > 0)
				label_taken[item_label_loc[i]] = false;
			item_dlg[i] = -1;
			}

	if (dlg_parent[which_dlg] != NULL) {
		EnableWindow(dlg_parent[which_dlg],true);
		if (dlg_parent[which_dlg] == mainPtr)
			for (i = 0; i < 18; i++)
				if (modeless_exists[i] == true)
					EnableWindow(modeless_dialogs[i],true);
		SetFocus(dlg_parent[which_dlg]);
		SetWindowPos(dlg_parent[which_dlg],HWND_TOP,0,0,100,100,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
		cd_set_edit_focus();
		}

	if (parent_message > 0)
		SendMessage(dlg_parent[which_dlg],WM_COMMAND,parent_message,0);

	DestroyWindow(dlgs[which_dlg]);
	dlg_keys[which_dlg] = -1;
	dialog_not_toast = true;
	block_erase = true;
	return 0;
}

short cd_process_click(HWND window, POINT the_point, WPARAM wparam, LPARAM,short *item)
{
	short i,which_dlg,dlg_num,item_id;
	short dlog_key;
//	char dummy[256];

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlog_key)) < 0)
		return -1;

	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
	{
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0)
		{
			if (PtInRect(&item_rect[item_id],the_point) && (item_active[item_id] > 0)
				&& ((item_type[item_id] < 3) || (item_type[item_id] == 8)
				|| (item_type[item_id] == 10)|| (item_type[item_id] == 11)))
			{
  				*item = i;
				if (MK_CONTROL & wparam) *item += 100;
				if (item_type[item_id] != 8) cd_press_button(dlg_num,i);
				return dlg_num;
			}
		}
	}

	return -1;
}

short cd_process_syskeystroke(HWND window, WPARAM wparam, LPARAM,short *item)
{
	short i,which_dlg,dlg_num,dlg_key,item_id;
	char char_hit;


	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return -1;
							// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
							// 25-30  ctrl 1-6

	switch (wparam) {
		case VK_ESCAPE:
			char_hit = 24;
			break;
		//case VK_SPACE:
		//	char_hit = ' ';
		//	break;
		case VK_LEFT:
			char_hit = 20;
			break;
		case VK_UP:
			char_hit = 22;
			break;
		case VK_RIGHT:
			char_hit = 21;
			break;
		case VK_DOWN:
			char_hit = 23;
			break;
		case VK_RETURN:
			char_hit = 31;
			break;
		default:
			return -1;
		}

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
	// kludgy. If you get an escape and is isn't processed, make it an enter
	if (wparam == VK_ESCAPE) {
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

short cd_process_keystroke(HWND window, WPARAM wparam, LPARAM, short *item)
{
	short i,which_dlg,dlg_num,dlg_key,item_id;
	char char_hit;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return -1;
							// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
							// 25-30  ctrl 1-6

	char_hit = (char) wparam;

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
	return -1;
}

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

void cd_set_pict(short dlog_num, short item_num, short pict_num)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (item_type[item_index] != 5) {
		beep();
		return;
		}
	item_flag[item_index] = pict_num;
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

void cd_get_text_edit_str(short, char *str)
{
	if (edit_box != NULL)
		GetWindowText(edit_box,str,255);
		else str[0] = 0;
}

void cd_set_item_text(short dlog_num, short item_num, char *str)
{
	short k,dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_type[item_index] == 6) {
		if (edit_box != NULL)
			SetWindowText(edit_box,str);
		return;
		}
	if (item_index >= 150) {
		beep();
		return;
		}
	if (item_index < 10) {
		sprintf(text_long_str[item_index],"%s",str);
		for (k = 0; k < 256; k++) {
			if (text_long_str[item_index][k] == '|')
				text_long_str[item_index][k] = 13;
			if (text_long_str[item_index][k] == '_')
				 text_long_str[item_index][k] = '"';
			}

		}
		else sprintf(text_short_str[item_index - 10],"%-34s",str);
	cd_draw_item( dlog_num,item_num);
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

void cd_set_flag(short dlog_num,short item_num,short flag)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;

	item_flag[item_index] = flag;
	cd_draw_item(dlog_num,item_num);
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
			if (label_taken[i] == false) {
				label_loc = i;
				label_taken[i] = true;
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

void cd_key_label(short dlog_num, short item_num,short loc)
{
	short dlg_index,item_index;
	char str[10];
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	sprintf((char *) str," ");
	str[0] = item_key[item_index];
	cd_add_label(dlog_num,item_num, str, 7 + loc * 100);
}

void cd_draw_item(short dlog_num,short item_num)
{
	short dlg_index,item_index,store_label;
	HDC win_dc, hdc;
	COLORREF colors[4] = {RGB(0,0,0),RGB(255,0,0),RGB(0,0,102),RGB(255,255,255)};
	RECT from_rect,to_rect;
	HFONT old_font;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (dlg_draw_ready[dlg_index] == false)
		return;

	win_dc = cd_get_dlog_dc(dlg_index);
    old_font = (HFONT) SelectObject(win_dc,small_bold_font);
	dlg_force_dc = win_dc;

	if (item_active[item_index] == 0) {
		cd_erase_item(dlog_num,item_num);
		cd_erase_item(dlog_num,item_num + 100);
		}
		else {
			switch (item_type[item_index]) {
				case 0: case 1: case 10: case 11:
					if (item_flag[item_index] == 143)
						break;
					from_rect.top = button_ul_y[button_type[item_flag[item_index]]];
					from_rect.left = button_ul_x[button_type[item_flag[item_index]]];
					from_rect.bottom = from_rect.top + button_height[button_type[item_flag[item_index]]];
					from_rect.right = from_rect.left + button_width[button_type[item_flag[item_index]]];
					rect_draw_some_item(dlg_buttons_gworld,from_rect,(HBITMAP) win_dc,item_rect[item_index],0,2);

					SelectObject(win_dc,bold_font);
					SetTextColor(win_dc,colors[2]);
					if (item_type[item_index] < 2)
						OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
					if (item_type[item_index] < 2) {
						char_win_draw_string(win_dc,item_rect[item_index],
						 (char *) (button_strs[item_flag[item_index]]),1,8);
						}
						else {
							char_win_draw_string(win_dc,item_rect[item_index],
							 (char *) ((item_index < 10) ? text_long_str[item_index] :
							  text_short_str[item_index - 10]),1,8);
							}
					if (item_type[item_index] < 2)
						OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);
					SetTextColor(win_dc,colors[0]);

					break;

				case 2:
					switch (item_flag[item_index]) {
						case 0: from_rect.left = 166; from_rect.top = 36; break;
						case 1: from_rect.left = 152; from_rect.top = 36; break;
						case 2: from_rect.left = 138; from_rect.top = 36; break;
						}
					from_rect.right = from_rect.left + 14;
					from_rect.bottom = from_rect.top + 10;
					rect_draw_some_item( dlg_buttons_gworld,from_rect, (HBITMAP) win_dc,item_rect[item_index],0,2); break;
					break;

				case 3: case 4: case 7: case 8: case 9:
					cd_erase_item(dlog_num,item_num);
						SetTextColor(win_dc,colors[3]);
					if ((item_type[item_index] == 3) || (item_type[item_index] == 9))
						SelectObject(win_dc,small_bold_font);
					if (item_type[item_index] == 4)
						SelectObject(win_dc,tiny_font);
					if (item_type[item_index] == 7)
						SelectObject(win_dc,bold_font);
					if (item_flag[item_index] % 10 == 1)
						cd_frame_item(dlog_num,item_num,2);
					if (item_flag[item_index] >= 10) {
						SetTextColor(win_dc,colors[1]);
						}

					if (item_rect[item_index].bottom - item_rect[item_index].top < 20) {
						item_rect[item_index].left += 3;
							DrawText(win_dc,(char *) ((item_index < 10) ? text_long_str[item_index] :
								text_short_str[item_index - 10]), -1, &item_rect[item_index],
								DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOCLIP);
						item_rect[item_index].left -= 3;
						}
						else {
							InflateRect(&item_rect[item_index],-4,-4);
							DrawText(win_dc,(char *) ((item_index < 10) ? text_long_str[item_index] :
								text_short_str[item_index - 10]), -1, &item_rect[item_index],
								DT_LEFT | DT_WORDBREAK | DT_NOCLIP);
							InflateRect(&item_rect[item_index],4,4);
							}
					SetTextColor(win_dc,colors[0]);
					break;
				case 5:
					if (item_flag[item_index] == -1)
						cd_erase_item(dlog_num,item_num);
						else draw_dialog_graphic(dlgs[dlg_index], item_rect[item_index],
							item_flag[item_index],(item_flag[item_index] >= 2000) ? false : true,0);
					break;
				case 20: // display picture special case

                from_rect.right = bitmap_info.bmWidth;
                from_rect.bottom = bitmap_info.bmHeight;

                GetObject(displayed_picture, sizeof(bitmap_info), &bitmap_info); //is picture greater ...

                hdc = CreateCompatibleDC(NULL); //let's draw the picture
            	SelectObject(hdc, displayed_picture);
	           	if((from_rect.right != bitmap_info.bmWidth) || (from_rect.bottom != bitmap_info.bmHeight)){ // ... than the screen ?
                   StretchBlt(win_dc,0, 0, from_rect.right, from_rect.bottom, hdc, 0 ,0 , bitmap_info.bmWidth, bitmap_info.bmHeight, SRCCOPY);
                    bitmap_info.bmWidth = from_rect.right; //in case of redraw ...
                    bitmap_info.bmHeight = from_rect.bottom; //... put back the screen max values
                }
                else
    	           	BitBlt(win_dc,0, 0, from_rect.right, from_rect.bottom, hdc, 0 ,0 , SRCCOPY);

	            DeleteDC(hdc); //clean after drawing
                //careful with the graphic files, if streched the quality can drop significantly.
				}
			}

			if (item_label[item_index] != 0) {
				store_label = item_label[item_index];
				if (store_label >= 1000) {
					store_label -= 1000;
					SelectObject(win_dc,bold_font);
					}
					else SelectObject(win_dc,tiny_font);
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

				if (item_active[item_index] != 0) {
					SetTextColor(win_dc,colors[3]);

					DrawText(win_dc, (char *) labels[item_label_loc[item_index]],
						-1, &to_rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
					SetTextColor(win_dc,colors[0]);

					}
				}

	// hook in special stuff. kludgy
	if ((dlog_num == 1098) && ((item_num >= 18) && (item_num <= 23))) {
		draw_pc_effects(10 + item_num - 18,win_dc);
		}

	SelectObject(win_dc,old_font);
	cd_kill_dc(dlg_index,win_dc);
	dlg_force_dc = NULL;
	}

void cd_initial_draw(short dlog_num)
{
	short i,which_dlg = -1;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	if (which_dlg < 0)
		return;
	dlg_draw_ready[which_dlg] = true;

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

void cd_redraw(HWND window)
{
	short which_dlg,dlg_num,dlg_key;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return;
	dlg_draw_ready[which_dlg] = true;
	cd_initial_draw(dlg_num);
}

void cd_frame_item(short dlog_num, short item_num, short width)
{
	short dlg_index,item_index;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	frame_dlog_rect(dlgs[dlg_index], item_rect[item_index], width);
}

void cd_erase_item(short dlog_num, short item_num)
// if item_num is 0, nail whole window
// item_num + 100  just erase label
{
	short i,dlg_index,item_index,store_label;
	RECT to_fry;
	HDC win_dc;
	Boolean just_label = false;

	if (item_num >= 100) {
		item_num -= 100;
		just_label = true;
		}

	if (item_num == 0) {
		for (i = 0; i < ND; i++)
				if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
					dlg_index = i;
		GetWindowRect(dlgs[dlg_index],&to_fry);
		OffsetRect(&to_fry,-1 * to_fry.left,-1 * to_fry.top);
		}
	else {
		if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
			return;
		to_fry = item_rect[item_index];
		if (just_label == true) {
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
		InflateRect(&to_fry,1,1);

		}
	if (dlg_draw_ready[dlg_index] == false)
		return;
	win_dc = cd_get_dlog_dc(dlg_index);
	paint_pattern((HBITMAP) win_dc,2,to_fry,0);
	cd_kill_dc(dlg_index,win_dc);
}

void cd_press_button(short dlog_num, short item_num)
{
	short dlg_index,item_index;
	long dummy;
	HDC win_dc;
	RECT from_rect;
	COLORREF colors[3] = {RGB(0,0,0),RGB(0,0,112),RGB(0,255,255)};

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	// no press action for redo buttons
	if ((item_type[item_index] == 2) || (item_flag[item_index] == 143)) {
		play_sound(34);
		return;
		}

	win_dc = cd_get_dlog_dc(dlg_index);

	from_rect.top = button_ul_y[button_type[item_flag[item_index]]];
	from_rect.left = button_ul_x[button_type[item_flag[item_index]]];
	from_rect.bottom = from_rect.top + button_height[button_type[item_flag[item_index]]];
	from_rect.right = from_rect.left + button_width[button_type[item_flag[item_index]]];
	OffsetRect(&from_rect,button_width[button_type[item_flag[item_index]]],0);

	rect_draw_some_item(dlg_buttons_gworld,from_rect,(HBITMAP)win_dc,item_rect[item_index],0,2);

	SelectObject(win_dc,bold_font);
	SetTextColor(win_dc,colors[2]);
	if (item_type[item_index] < 2)
		OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
	if (item_type[item_index] < 2) {
		char_win_draw_string(win_dc,item_rect[item_index],
		(char *) (button_strs[item_flag[item_index]]),1,8);
		}
		else {
			char_win_draw_string(win_dc,item_rect[item_index],
			 (char *) ((item_index < 10) ? text_long_str[item_index] :
			text_short_str[item_index - 10]),1,8);
			}
	if (item_type[item_index] < 2)
		OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);

	if (play_sounds == true) {
		play_sound(37);
		Delay(6,&dummy);
		}
		else Delay(10,&dummy);

	OffsetRect(&from_rect,-1 * button_width[button_type[item_flag[item_index]]],0);
	rect_draw_some_item(dlg_buttons_gworld,from_rect,(HBITMAP)win_dc,item_rect[item_index],0,2);

	SelectObject(win_dc,bold_font);
	SetTextColor(win_dc,colors[1]);
	if (item_type[item_index] < 2)
		OffsetRect(&item_rect[item_index],-1 * button_left_adj[item_flag[item_index]],0);
	if (item_type[item_index] < 2) {
		char_win_draw_string(win_dc,item_rect[item_index],
		(char *) (button_strs[item_flag[item_index]]),1,8);
		}
		else {
			char_win_draw_string(win_dc,item_rect[item_index],
			 (char *) ((item_index < 10) ? text_long_str[item_index] :
			text_short_str[item_index - 10]),1,8);
			}
	if (item_type[item_index] < 2)
		OffsetRect(&item_rect[item_index],button_left_adj[item_flag[item_index]],0);

	SelectObject(win_dc,font);
	SetTextColor(win_dc,colors[0]);

	cd_kill_dc(dlg_index,win_dc);
	}

// LOW LEVEL
short cd_get_indices(short dlg_num, short item_num, short *dlg_index, short *item_index)
{
	if ((*dlg_index = cd_get_dlg_index(dlg_num)) < 0) return -1;
	if ((*item_index = cd_get_item_id(dlg_num,item_num)) < 0) return -1;
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

short cd_find_dlog(HWND window, short *dlg_num, short *dlg_key)
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

HDC cd_get_dlog_dc(short which_slot)
{
	HDC win_dc = GetDC(dlgs[which_slot]);
	SetBkMode(win_dc,TRANSPARENT);
	return win_dc;
}

void center_window(HWND window)
{
	RECT main_rect,wind_rect;
	short width,height;

	cursor_shown = true;
	showcursor(true);

	GetWindowRect(GetDesktopWindow(),&main_rect);
	GetWindowRect(window,&wind_rect);
	width = wind_rect.right - wind_rect.left;
	height = wind_rect.bottom - wind_rect.top;
	MoveWindow(window,((main_rect.right - main_rect.left) - width) / 2,
		((main_rect.bottom - main_rect.top) - height) / 2,width,height,true);

}

RECT get_item_rect(HWND hDlg, short item_num)
{
	HWND item;
	RECT big_rect,small_rect;

	item = GetDlgItem(hDlg, item_num);
	GetWindowRect(hDlg,&big_rect);
	GetWindowRect(item,&small_rect);
	OffsetRect(&small_rect, -1 * big_rect.left - 7, -1 * big_rect.top - 7);
	small_rect.right += 2;
	small_rect.bottom += 2;
	return small_rect;
}


void frame_dlog_rect(HWND hDlg, RECT rect, short val)
{
	HDC hdc;
	HPEN dpen,lpen,old_pen;
	COLORREF x = RGB(0,204,255),y = RGB(0,204,255);//y = RGB(119,119,119);
	Boolean keep_dc = false;

	InflateRect(&rect,val,val);

	if (hDlg == mainPtr)
	{
		keep_dc = true;
		hdc = main_dc;
	}
	else if (dlg_force_dc != NULL)
	{
		hdc = dlg_force_dc;
		keep_dc = true;
	}
	else hdc = GetDC(hDlg);

	if (hdc == NULL)
	{
		beep();
		return;
	}

	lpen = CreatePen(PS_SOLID,1,x);
	dpen = CreatePen(PS_SOLID,1,y);
	old_pen = (HPEN) SelectObject(hdc,dpen);
	MoveToEx(hdc,rect.left,rect.top, NULL);
	LineTo(hdc,rect.right,rect.top);
	SelectObject(hdc,lpen);
	LineTo(hdc,rect.right,rect.bottom);
	LineTo(hdc,rect.left,rect.bottom);
	SelectObject(hdc,dpen);
	LineTo(hdc,rect.left,rect.top);
	SelectObject(hdc,old_pen);
	if (keep_dc == false)
		fry_dc(hDlg,hdc);
	DeleteObject(dpen);
	DeleteObject(lpen);
}

void draw_dialog_graphic(HWND hDlg, RECT rect, short which_g, Boolean do_frame,short win_or_gworld)
// win_or_gworld: 0 - window  1 - an HBITMAP
//    1 means hDlg is actually an HBITMAP variable!
// 0 - 300   number of terrain graphic
//   400 + x - monster graphic num
// 600 + x  item graphic
// 700 + x  dlog graphic
// 800 + x  pc graphic
// 900 + x  B&W graphic
// 950 null item
// 1000 + x  Talking face
// 1100 - item info help
// 1200 - pc screen help
// 1300 - combat ap
// 1400-1402 - button help
// 1500 - stat symbols help
// 1600 + x - B&W maps
// 1700 + x - anim graphic

{
	RECT from2 = {0,0,36,36},tiny_obj_rect = {0,0,18,18};
	RECT from_rect = {0,0,28, 36};
	RECT face_from = {0,0,32,32};
	RECT to_rect = {6,6,42,42};
//	RECT bw_from = {0,0,120,120};

	RECT pc_info_from = {0,127,106,157};
	RECT item_info_from = {174,0,312,112};
	RECT button_help_from = {0,0,320,100};
	RECT combat_ap_from = {0,0,275,100};
	RECT stat_symbols_from = {0,0,386,94}; /**/
	RECT small_monst_rect = {0,0,14,18};
	RECT large_scen_from = {0,0,64,64};

	HBITMAP from_gworld;
	short draw_dest = 2;
	HDC hdc;
	HBRUSH old_brush;
	short m_start_pic = 0,square_size = 32;

	if (win_or_gworld == 1)	draw_dest = 0;
	if (which_g < 0) return;
	if (which_g >= 3000) do_frame = false;
	which_g %= 3000;

	if (win_or_gworld == 0) {
		if (dlg_force_dc != NULL)
			hdc = dlg_force_dc;
			else hdc = GetDC(hDlg);
		}
	if (which_g == 950) { // Empty. Maybe clear space.
		if (win_or_gworld == 0) {
			paint_pattern((HBITMAP) hdc,2,rect,0);
			}
			//FillCRect(&rect,bg[5]);	// don't forget to nail dc!!!
		if (dlg_force_dc == NULL)
			fry_dc(hDlg, hdc);
		return;
		}

	switch (which_g / 100) {
		case 0: case 1: case 2: // terrain
			from_gworld = terrains_gworld[which_g / 50];
			which_g = which_g % 50;
			from_rect = calc_rect(which_g % 10, which_g / 10);
			if (rect.right - rect.left != 28) {
				rect.left += 1;
            rect.right = rect.left + 28;
				}
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)(hDlg): (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			break;
		case 3: // animated terrain
			which_g -= 300;
			from_gworld = ter_anim_gworld;
			from_rect = calc_rect(4 * (which_g / 5), which_g % 5);
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			break;
		case 4: case 5: // monster
			// There are 4 different ways to draw, depending on size of monster
			which_g -= 400;
			m_start_pic = m_pic_index[which_g];
			if ((m_pic_index_x[which_g] == 1) && (m_pic_index_y[which_g] == 1)) {
				from_gworld = monsters_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0) {
					old_brush = (HBRUSH) SelectObject(hdc,GetStockObject(BLACK_BRUSH));
					Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
					SelectObject(hdc,old_brush);
					}
				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,rect,0,draw_dest);
				}
			if ((m_pic_index_x[which_g] == 2) && (m_pic_index_y[which_g] == 1)) {
				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0) {
					old_brush = (HBRUSH) SelectObject(hdc,GetStockObject(BLACK_BRUSH));
					Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
					SelectObject(hdc,old_brush);
					}
				from_gworld = monsters_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,rect.left,rect.top + 7);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

				m_start_pic = m_pic_index[which_g] + 1;
				from_gworld = monsters_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,14,0);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);
				}
			if ((m_pic_index_x[which_g] == 1) && (m_pic_index_y[which_g] == 2)) {
				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0) {
					old_brush = (HBRUSH)SelectObject(hdc,GetStockObject(BLACK_BRUSH));
					Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
					SelectObject(hdc,old_brush);
					}

				from_gworld = monsters_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,rect.left + 7,rect.top);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);
				m_start_pic = m_pic_index[which_g] + 1;
				from_gworld = monsters_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,0,18);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);
				}

			if ((m_pic_index_x[which_g] == 2) && (m_pic_index_y[which_g] == 2)) {
				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0) {
					old_brush = (HBRUSH)SelectObject(hdc,GetStockObject(BLACK_BRUSH));
					Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
					SelectObject(hdc,old_brush);
					}
				from_gworld = monsters_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,rect.left,rect.top);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

				m_start_pic = m_pic_index[which_g] + 1;
				from_gworld = monsters_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,14,0);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

				m_start_pic = m_pic_index[which_g] + 2;
				from_gworld = monsters_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,-14,18);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);
				m_start_pic = m_pic_index[which_g] + 3;
				from_gworld = monsters_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,14,0);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);
				}
			break;
		case 18: case 19: // item
			which_g -= 1800;
			to_rect = rect;
			if (which_g < 45) {
				from_gworld = items_gworld;
				from_rect = calc_rect(which_g % 5, which_g / 5);
				}
				else {
					from_gworld = tiny_obj_gworld;
					//rect.top += 4; rect.bottom -= 4;
					//InflateRect(&to_rect,-5,-9);
					to_rect.top += 9;
					to_rect.left += 5;
					to_rect.right = to_rect.left + 18;
					to_rect.bottom = to_rect.top + 18;
					from_rect = tiny_obj_rect;
					OffsetRect(&from_rect,18 * (which_g % 10), 18 * (which_g / 10));
					}
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,to_rect,1,draw_dest);
			break;
		case 7: // dialog
				which_g -= 700;
			from_gworld = dlogpics_gworld;
			OffsetRect(&from2,36 * (which_g % 4),36 * (which_g / 4));
			rect_draw_some_item(from_gworld,from2,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			break;
		case 8: // PC
			which_g -= 800;
			from_rect = calc_rect(2 * (which_g / 8), which_g % 8);
			old_brush = (HBRUSH)SelectObject(hdc,GetStockObject(BLACK_BRUSH));
			Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
			SelectObject(hdc,old_brush);
			//PaintRect(&rect);
			rect_draw_some_item(pcs_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,rect,1,draw_dest);
			break;
/*		case 9: // B&W => Exile III maps
			which_g -= 900;
			from_gworld = load_pict(875,main_dc);
			from_rect = bw_from;
			OffsetRect(&from_rect,120 * ((which_g) % 3),120 * ((which_g) / 3));
			if (win_or_gworld == 1)
				 rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,rect,0,0);
				else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,rect,0,draw_dest);
			DeleteObject(from_gworld);
			break;*/
		case 10: // talk face
			which_g -= 1000;
			from_rect = face_from;
			OffsetRect(&from_rect,32 * ((which_g - 1) % 10),32 * ((which_g - 1) / 10));
			if (win_or_gworld == 1)
				 rect_draw_some_item(talking_portraits_gworld,from_rect,(HBITMAP) hDlg,rect,0,0);
				else rect_draw_some_item(talking_portraits_gworld,from_rect,(HBITMAP)hdc,rect,0,draw_dest);
			break;
		case 11: // item info help
			from_rect = item_info_from;
			rect.right = rect.left + from_rect.right - from_rect.left;
			rect.bottom = rect.top + from_rect.bottom - from_rect.top;
			rect_draw_some_item(mixed_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			break;
		case 12: // item info help
			from_rect = pc_info_from;
			rect.right = rect.left + pc_info_from.right - pc_info_from.left;
			rect.bottom = rect.top + pc_info_from.bottom - pc_info_from.top;
			rect_draw_some_item(mixed_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			break;
		case 14: // button help
			which_g -= 1400;
			if (which_g >= 10) {
				from_gworld = load_pict(900 + which_g,main_dc);
				from_rect = large_scen_from;
				OffsetRect(&from_rect,64 * (which_g % 10),0);
				rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
				  ,rect,0,draw_dest);
				DeleteObject(from_gworld);
				break;
				}
			from_gworld = load_pict(1401,main_dc);
			from_rect = button_help_from;
         rect.top += 10;
			rect.right = rect.left + from_rect.right;
			rect.bottom = rect.top + from_rect.bottom;
			OffsetRect(&from_rect,0,100 * which_g);
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			DeleteObject(from_gworld);
			break;
		case 13: // combat ap help
			from_gworld = load_pict(1402,main_dc);
			from_rect = combat_ap_from;
			rect.right = rect.left + from_rect.right;
			rect.bottom = rect.top + from_rect.bottom;
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP) (hDlg): (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			DeleteObject(from_gworld);
			break;
		case 15: // stat symbols help
			from_gworld = load_pict(1400,main_dc);
			from_rect = stat_symbols_from;
			rect.right = rect.left + from_rect.right;
			rect.bottom = rect.top + from_rect.bottom;
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP) (hDlg): (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			DeleteObject(from_gworld);
			break;
		case 16:
			which_g -= 1600;
			from_gworld = load_pict(851,main_dc);
			from_rect.right = 32;
			from_rect.bottom = 32;
			OffsetRect(&from_rect,32 * (which_g % 5),32 * (which_g / 5));
			rect.right = rect.left + 32;
			rect.bottom = rect.top + 32;
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP) (hDlg): (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			DeleteObject(from_gworld);
			break;
		case 17: // dialog
			which_g -= 1700;
			from_gworld = fields_gworld;
			from_rect = calc_rect(which_g % 8, which_g / 8);
			if (win_or_gworld == 0) {
				old_brush = (HBRUSH) SelectObject(hdc,GetStockObject(BLACK_BRUSH));
				Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
				SelectObject(hdc,old_brush);
				}
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			break;
		case 20: case 21: case 22: case 23: // dialog
			which_g -= 2000;
			rect.right = rect.left + 28;
			rect.bottom = rect.top + 36;
			from_gworld = spec_scen_g;
			from_rect = get_custom_rect(which_g);
			if ((win_or_gworld == 0) && (do_frame == true)) {
				old_brush = (HBRUSH)SelectObject(hdc,GetStockObject(BLACK_BRUSH));
				Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
				SelectObject(hdc,old_brush);
				}

			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP) (hDlg): (HBITMAP)hdc)
			  ,rect,(do_frame == false) ? 1 : 0,draw_dest);
			break;
		case 24: case 25: case 26: case 27: // dialog
			which_g -= 2400;
			if (rect.right - rect.left >= 36)
				square_size = 36;
			from_gworld = spec_scen_g;
			from_rect = get_custom_rect(which_g);
			to_rect = rect;
			to_rect.right = to_rect.left + square_size / 2;
			to_rect.bottom = to_rect.top + square_size;
			from_rect.right = from_rect.left + square_size / 2;
			from_rect.bottom = from_rect.top + square_size;
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP) (hDlg): (HBITMAP)hdc)
			  ,to_rect,1,draw_dest);
			from_rect = get_custom_rect(which_g + 1);
			OffsetRect(&to_rect,square_size / 2,0);
			from_rect.right = from_rect.left + square_size / 2;
			from_rect.bottom = from_rect.top + square_size;
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP) (hDlg): (HBITMAP)hdc)
			  ,to_rect,1,draw_dest);
			break;
		}

	if ((win_or_gworld == 0) && (dlg_force_dc == NULL))
		fry_dc(hDlg, hdc);
	if ((win_or_gworld == 0) && (do_frame == true)){
		rect.bottom--; rect.right--;
		frame_dlog_rect(hDlg,rect,3);
		}
}

void showcursor(Boolean a)
{
	short i;
	i = ShowCursor(a);
	if (a == false)
		while (i >= 0)
			i = ShowCursor(false);
	if (a == true)
		while (i < 0)
			i = ShowCursor(true);
	}

void ModalDialog()
{
	MSG msg;

	while ((dialog_not_toast == true) && (GetMessage(&msg,NULL,0,0)))
	{
		if (!TranslateAccelerator(mainPtr, accel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
	}
}

RECT calc_rect(short i, short j)
{
	RECT base_rect = {0,0,28,36};

	OffsetRect(&base_rect,i * 28, j * 36);
	return base_rect;
}
