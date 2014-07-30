
#include <windows.h>
#include <algorithm>

#define ND	15
#define	NI	500
#define	NL	100

#include "string.h"

#include "global.h"
#include "graphutl.h"
#include "stdio.h"
#include "graphics.h"
#include "edsound.h"
#include "dlogtool.h"
#include "keydlgs.h"


extern Boolean play_sounds,cursor_shown,dialog_not_toast,block_erase,mouse_button_held;
extern HBITMAP title_gworld,dlogpics_gworld,items_gworld,tiny_obj_gworld;
extern HBITMAP dlg_buttons_gworld,mixed_gworld,monst_gworld[10],talkfaces_gworld;
extern HFONT font,italic_font,underline_font,bold_font,tiny_font,small_bold_font;
extern HWND mainPtr;
extern HWND right_sbar;
extern HDC main_dc;
extern HINSTANCE store_hInstance;
long CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);


extern short terrain_pic[256];
extern unsigned char m_pic_index[200];
extern unsigned char m_pic_index_x[200];
extern unsigned char m_pic_index_y[200];
extern unsigned long anim_ticks;
extern terrain_type_type store_ter;
extern HBITMAP spec_scen_g;

extern HACCEL accel;

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
short custom_type[NI];//0 - not custom, 1 - 1x1, 2 - 2x1, 3 - 1x2, 4 - 2x2, 5 - animated custom

char text_long_str[10][256];
char text_short_str[140][40];
char labels[NL][25];
Boolean label_taken[NL];

HWND edit_box[80];
HWND store_edit_parent[80];
short store_edit_parent_num[80];
short store_edit_item[80]; // kludgy
FARPROC edit_proc,old_edit_proc[80];

HDC dlg_force_dc = NULL; // save HDCs when dealing with dlogs

short store_free_slot,store_dlog_num;
HWND store_parent;

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
short button_left_adj[140] = {0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0, // 50
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0, // 100
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,
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
							0,0,0,0,'o',0,0,0,0,0
							};
							/* specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
							 25-30  ctrl 1-6  31 - return\ */

short button_ul_x[15] = {0,46,0,126,0, 0,126,126,126,138, 166,0,0,126,172};
short button_ul_y[15] = {0,0,132,23,46, 69,46,69,36,36, 36,23,92,92,0};
short button_width[15] = {23,63,102,16,63, 63,63,63,6,14, 14,63,63,63,30};
short button_height[15] = {23,23,23,13,23, 23,23,23,6,10,10,23,40,40,30};

BOOL CALLBACK dummy_dialog_proc (HWND hDlg, UINT message, WPARAM wparam, LPARAM lparam);
long CALLBACK fresh_edit_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

FARPROC d_proc;

extern char szAppName[];
extern char szWinName[];

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
	for (i = 0; i < 80; i++) {
		edit_box[i] = NULL;
		store_edit_parent[i] = NULL;
		store_edit_parent_num[i] = -1;
		store_edit_item[i] = -1	;
		}
	d_proc = (FARPROC) dummy_dialog_proc;
	edit_proc = (FARPROC) fresh_edit_proc;
}

long CALLBACK fresh_edit_proc(HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
	short i,cur_box = -1,cur_item_num,item_for_focus = -1,first_edit_box = -1;


	for (i = 0; i < 80; i++)
		if (edit_box[i] == hwnd) {
			cur_box = i;
			cur_item_num = store_edit_item[i];
			}
		if (cur_box >= 0) {
		switch (message) {

			 case WM_CHAR:
			  if ((char) (wParam) == 9) {
					return TRUE;
					}
			break;
			case WM_KEYDOWN:
				if (wParam == VK_RETURN)
					SendMessage(store_edit_parent[cur_box],WM_KEYDOWN,wParam,lParam);
				if (wParam == VK_ESCAPE)
					SendMessage(store_edit_parent[cur_box],WM_KEYDOWN,wParam,lParam);

				if (wParam == VK_TAB) {
					//play_sound(1);
					for (i = 0; i < 80; i++) {
						if ((store_edit_parent[i] == store_edit_parent[cur_box]) &&
							(store_edit_item[i] == cur_item_num + 1))
								item_for_focus = i;
						if ((store_edit_parent[i] == store_edit_parent[cur_box]) &&
							(store_edit_item[i] == 2))
								first_edit_box = i;
						}

					if (item_for_focus >= 0)
						SetFocus(edit_box[item_for_focus]);
						else if (first_edit_box >= 0)
							SetFocus(edit_box[first_edit_box]);
					return TRUE;
					}
				break;
			}

			return CallWindowProc((WNDPROC) old_edit_proc[cur_box],hwnd,message,wParam,lParam);
		}
	return 0;
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

short cd_create_dialog(short dlog_num,HWND parent)
{
	short i,free_slot = -1;
	HWND dlg;

	if (parent != NULL) {
		if (IsWindowEnabled(parent) == 0)
      	return -1;
		}
   mouse_button_held = FALSE;
	store_dlog_num = dlog_num;
	store_parent = parent;

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
	dlg = CreateDialog(store_hInstance,MAKEINTRESOURCE(dlog_num),0,(DLGPROC)d_proc);

	if (dlgs[free_slot] == NULL) {
		play_sound(3);
		return -3;
		}
	center_window(dlgs[free_slot]);

	dlg_parent[free_slot] = parent;

	SetWindowText(dlgs[free_slot],"Blades of Exile Scenario Editor");

	ShowWindow(dlgs[free_slot],SW_SHOW);
	DestroyWindow(dlg); //Necessary? Dunno.

	if (dlg_parent[free_slot] != NULL) {
		EnableWindow(dlg_parent[free_slot],FALSE);
		}
	dialog_not_toast = TRUE;
   set_cursor(0);
	return 0;
}

BOOL CALLBACK dummy_dialog_proc	(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	short i,j,k,l,free_slot = -1,free_item = -1,type,flag;
	char item_str[256];
	Boolean str_stored = FALSE,focus_set = FALSE;
	RECT dlg_rect;
	short win_height = 0, win_width = 0, num_text_boxes = 0;
	short str_offset = 1;

	free_slot = store_free_slot;

	switch (message)
	{
		case WM_INITDIALOG:

	// now, make a window, matching dialog
	GetWindowRect(hDlg,&dlg_rect);
	dlgs[store_free_slot] = CreateWindow (szWinName,
			"BoE for Win32 Dialog",
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
			str_stored = FALSE;

			if (strlen((char *)item_str) == 0)
			{
				sprintf((char *) item_str, "+");
				type = 3;
				flag = 0;
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
				str_stored = TRUE;
				}
			else
			{
				//sscanf(item_str,"%d_%d",&type,&flag);
				type = atoi(item_str);
				flag = atoi(item_str+2);
			}


			free_item = -1;
			// find free item
			switch (type)
			{
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

			if (free_item >= 0)
			{
					item_dlg[free_item] = store_dlog_num;
					item_type[free_item] = type;
					item_number[free_item] = i;

					item_rect[free_item] = get_item_rect(hDlg,i);
					item_rect[free_item].top = item_rect[free_item].top / 2;
					item_rect[free_item].left = item_rect[free_item].left / 2;
					item_rect[free_item].bottom = item_rect[free_item].bottom / 2;
					item_rect[free_item].right = item_rect[free_item].right / 2;

					item_flag[free_item] = flag;
					item_active[free_item] = 1;
					item_label[free_item] = 0;
            	    item_label_loc[free_item] = -1;
               		item_key[free_item] = 0;

					switch (type) {
						case 0: case 1:
							item_rect[free_item].right = item_rect[free_item].left + button_width[button_type[flag]];
							item_rect[free_item].bottom = item_rect[free_item].top + button_height[button_type[flag]];

							if ((store_dlog_num >= 2000) || (store_dlog_num == 986))
								OffsetRect(&item_rect[free_item],20,0);

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
								if (free_item < 10) {
									sprintf(text_long_str[free_item],"%s",
									  (char *) (item_str + str_offset));
									for (k = 0; k < 256; k++) {
										if (text_long_str[free_item][k] == '|')
											 text_long_str[free_item][k] = 13;
										if (text_long_str[free_item][k] == '_')
											 text_long_str[free_item][k] = '"';
										}
									// give text a little extra room
									if ((store_dlog_num >= 2000) || (store_dlog_num == 986))
										item_rect[free_item].right += 20;
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
							for (l = 0; l < 80; l++)
								if (store_edit_parent[l] == NULL)
								{
									if (item_rect[free_item].bottom - item_rect[free_item].top < 24)
										item_rect[free_item].top -= 2;
									if (item_rect[free_item].bottom - item_rect[free_item].top <= 22)
									edit_box[l] = CreateWindow("edit",NULL,WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOVSCROLL,
										item_rect[free_item].left,item_rect[free_item].top,
										item_rect[free_item].right - item_rect[free_item].left,
										std::max<short>(22,item_rect[free_item].bottom - item_rect[free_item].top),
										dlgs[free_slot],(HMENU)150,store_hInstance,NULL);
									else edit_box[l] = CreateWindow("edit",NULL,WS_CHILD | WS_BORDER | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL,
										item_rect[free_item].left,item_rect[free_item].top,
										item_rect[free_item].right - item_rect[free_item].left,
										std::max<short>(22,item_rect[free_item].bottom - item_rect[free_item].top),
										dlgs[free_slot],(HMENU)150,store_hInstance,NULL);
									num_text_boxes++;
									store_edit_parent[l] =  dlgs[free_slot];
 									store_edit_parent_num[l] = store_dlog_num;
									store_edit_item[l] = i;
									old_edit_proc[l] = (FARPROC) GetWindowLong(edit_box[l],GWL_WNDPROC);
									SetWindowLong(edit_box[l],GWL_WNDPROC,(LONG) edit_proc);
									if (focus_set == FALSE)
									{
										SetFocus(edit_box[l]);
										focus_set = TRUE;
									}
									l = 80;
								}
							break;
						}
					win_height = std::max<short>(win_height, item_rect[free_item].bottom + 28);
					win_width = std::max<short>(win_width, item_rect[free_item].right + 11);

				}

			} /* end of for-if */
			MoveWindow(dlgs[free_slot],0,0,win_width,win_height,FALSE);
			EndDialog(hDlg, 0);
			return TRUE;

		} /* end of switch(message) */
		return TRUE;
	}

void cd_set_edit_focus(short which_win)
{
	short i;

	for (i = 0; i < 80; i++)
		if (store_edit_parent_num[i] == which_win) {
			if (edit_box[i] != NULL)
				SetFocus(edit_box[i]);
			return;
			}
}

short cd_kill_dialog(short dlog_num,short parent_message)
{
	short i,j,which_dlg = -1;

	for (i = 0; i < ND; i++)
		if ((dlg_keys[i] >= 0) && (dlg_types[i] == dlog_num))
			which_dlg = i;
	if (which_dlg < 0)
		return -1;

	for (i = 0; i < NI; i++)
		if (item_dlg[i] == dlog_num) {
			if (item_type[i] == 6)  {
				for (j = 0; j < 80; j++)
					if ((store_edit_parent_num[j] == dlog_num) && (store_edit_item[j] == item_number[i])) {
						DestroyWindow(edit_box[j]);
                  store_edit_parent[j] = NULL;
						edit_box[j] = NULL;
						}
				}
			if (item_label[i] > 0)
				label_taken[item_label_loc[i]] = FALSE;
			item_dlg[i] = -1;
			}

	if (dlg_parent[which_dlg] != NULL) {
		EnableWindow(dlg_parent[which_dlg],TRUE);

		SetFocus(dlg_parent[which_dlg]);
		SetWindowPos(dlg_parent[which_dlg],HWND_TOP,0,0,100,100,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
		cd_set_edit_focus((size_t)dlg_parent[which_dlg]);
        }

	if (parent_message > 0)
		SendMessage(dlg_parent[which_dlg],WM_COMMAND,parent_message,0);

DestroyWindow(dlgs[which_dlg]);
	dlg_keys[which_dlg] = -1;
	dialog_not_toast = TRUE;
   block_erase = TRUE;
	return 0;
}

short cd_process_click(HWND window,POINT the_point, WPARAM wparam, LPARAM lparam,short *item)
{
	short i,which_dlg,dlg_num,item_id;
	short dlog_key;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlog_key)) < 0)
		return -1;

	for (i = 0; i < dlg_highest_item[which_dlg] + 1; i++)
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
			if ((PtInRect(&item_rect[item_id],the_point)) && (item_active[item_id] > 0)
				&& ((item_type[item_id] < 3) || (item_type[item_id] == 8)
				|| (item_type[item_id] == 10)|| (item_type[item_id] == 11))) {
					*item = i;
					if (MK_CONTROL & wparam)
						*item += 100;
					if (item_type[item_id] != 8)
						cd_press_button(dlg_num,i);
					return dlg_num;
					}
			}
	return -1;
}

short cd_process_syskeystroke(HWND window,WPARAM wparam, LPARAM lparam,short *item)
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

short cd_process_keystroke(HWND window,WPARAM wparam, LPARAM lparam,short *item)
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

void cd_set_pict(short dlog_num, short item_num, short pict_num, short custom_pic_type)
{
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (item_type[item_index] != 5) {
		beep();
		return;
		}
	item_flag[item_index] = pict_num;
	custom_type[item_index] = custom_pic_type;
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
	short dlg_index,item_index,i;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_type[item_index] == 6) {
		sprintf(str,"");
		for (i = 0; i < 80; i++)
			if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] = item_num)
				&& (edit_box[i] != NULL))
				GetWindowText(edit_box[i],str,255);
		return;
		}
	if (item_index >= 150) {
		beep();
		return;
		}
	if (item_index < 10)
		sprintf(str,"%s",text_long_str[item_index]);
		else sprintf(str,"%s",text_short_str[item_index - 10]);
}


void cd_retrieve_text_edit_str(short dlog_num, short item_num, char *str)
{
	short i;

	for (i = 0; i < 80; i++)
		if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] == item_num)
			&& (edit_box[i] != NULL)) {
				GetWindowText(edit_box[i],str,255);
				return;
				}
}

short cd_retrieve_text_edit_num(short dlog_num, short item_num)
{
	short i;
	short num_given;
	char str[256];

	for (i = 0; i < 80; i++)
		if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] == item_num)
			&& (edit_box[i] != NULL)) {
				GetWindowText(edit_box[i],str,255);
				sscanf(str,"%d",&num_given);
				return (short) num_given;
				}
	return -1;


}


// NOTE!!! Expects a c string
void cd_set_text_edit_str(short dlog_num, short item_num, char *str)
{
	short i;
		for (i = 0; i < 80; i++)
			if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] == item_num)
				&& (edit_box[i] != NULL))
					SetWindowText(edit_box[i],str);

}

// NOTE!!! Expects a c string
//highlight is to select the dialog text
void cd_set_text_edit_num(short dlog_num, short item_num, short num, bool highlight)
{
	short i;
	char store_ptr[256];

		sprintf(store_ptr,"%d",num);
		for (i = 0; i < 80; i++)
			if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] == item_num)
				&& (edit_box[i] != NULL)){
					SetWindowText(edit_box[i],store_ptr);
					if(highlight)
					   SendMessage(edit_box[i],EM_SETSEL,0,-1);
                    }
}

void cdsin(short dlog_num, short item_num, short num)
{
	cd_set_item_num( dlog_num,  item_num,  num);
}

void csit(short dlog_num, short item_num, char *str)
{
cd_set_item_text( dlog_num,  item_num, str);
}
void csp(short dlog_num, short item_num, short pict_num, short custom_pic_type)
{
	cd_set_pict( dlog_num,  item_num,  pict_num, custom_pic_type);
}

void cd_set_item_text(short dlog_num, short item_num, char *str)
{
	short dlg_index,item_index,i;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_type[item_index] == 6) {
		for (i = 0; i < 80; i++)
			if ((store_edit_parent_num[i] == dlog_num) && (store_edit_item[i] = item_num)
				&& (edit_box[i] != NULL))
					SetWindowText(edit_box[i],str);
		return;
		}
	if (item_index >= 150) {
		beep();
		return;
		}
	if (item_index < 10)
		sprintf(text_long_str[item_index],"%s",str);
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

void cd_draw_item(short dlog_num,short item_num)
{
	short dlg_index,item_index,store_label;
	HDC win_dc;
	COLORREF colors[4] = {RGB(0,0,0),RGB(255,0,0),RGB(0,0,112),RGB(255,255,255)};
	RECT from_rect,to_rect;
	HFONT old_font;

	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (dlg_draw_ready[dlg_index] == FALSE)
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
					from_rect.top = button_ul_y[button_type[item_flag[item_index]]];
					from_rect.left = button_ul_x[button_type[item_flag[item_index]]];
					from_rect.bottom = from_rect.top + button_height[button_type[item_flag[item_index]]];
					from_rect.right = from_rect.left + button_width[button_type[item_flag[item_index]]];
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
					rect_draw_some_item( dlg_buttons_gworld,from_rect,(HBITMAP) win_dc,item_rect[item_index],0,2); break;
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
					else if(custom_type[item_index] > 0){
                        draw_custom_dialog_graphic(dlgs[dlg_index], item_rect[item_index],
						item_flag[item_index], TRUE,0, custom_type[item_index]);
                    }
                    else draw_dialog_graphic(dlgs[dlg_index], item_rect[item_index],
						item_flag[item_index],(item_flag[item_index] >= 2000) ? FALSE : TRUE,0);
					break;

				}
			}

			if ( item_label[item_index] != 0) {
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

void cd_redraw(HWND window)
{
	short which_dlg,dlg_num,dlg_key;
	short i;

	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return;
	dlg_draw_ready[which_dlg] = TRUE;
	cd_initial_draw(dlg_num);

	for (i = 0; i < 80; i++)
		if ((store_edit_parent[i] != NULL) &&
			(store_edit_parent_num[i] == dlg_num)) {

				RedrawWindow(edit_box[i],NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW);
				}
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
	Boolean just_label = FALSE;

	if (item_num >= 100) {
		item_num -= 100;
		just_label = TRUE;
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
		InflateRect(&to_fry,1,1);

		}
	if (dlg_draw_ready[dlg_index] == FALSE)
		return;
	win_dc = cd_get_dlog_dc(dlg_index);

	paint_pattern((HBITMAP)win_dc,2,to_fry,0);
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
	// no press action for redio buttons
	if (item_type[item_index] == 2) {
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

	if (play_sounds == TRUE) {
		play_sound(37);
		Delay(6,&dummy);
		}

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
	HDC win_dc;

	win_dc = GetDC(dlgs[which_slot]);
	SetBkMode(win_dc,TRANSPARENT);
	return win_dc;
	}

void cd_kill_dc(short which_slot,HDC hdc)
{
	fry_dc(dlgs[which_slot],hdc);
}

// External graphics tools (huh huh huh ... tool ... huh huh huh)

void center_window(HWND window)
{
	RECT main_rect,wind_rect;
	short width,height;

	cursor_shown = TRUE;
	showcursor(TRUE);

	GetWindowRect(GetDesktopWindow(),&main_rect);
	GetWindowRect(window,&wind_rect);
	width = wind_rect.right - wind_rect.left;
	height = wind_rect.bottom - wind_rect.top;
	MoveWindow(window,((main_rect.right - main_rect.left) - width) / 2,
		((main_rect.bottom - main_rect.top) - height) / 2,width,height,TRUE);

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
	Boolean keep_dc = FALSE;

	InflateRect(&rect,val,val);

	if (hDlg == mainPtr) {
		keep_dc = TRUE;
		hdc = main_dc;
		}
		else if (dlg_force_dc != NULL) {
		hdc = dlg_force_dc;
		keep_dc = TRUE;
		}
		else hdc = GetDC(hDlg);
	if (hdc == NULL) {
		beep();
		return;
		}
	dpen = CreatePen(PS_SOLID,1,y);
	if (hDlg != mainPtr)
		lpen = CreatePen(PS_SOLID,1,x);
	else lpen = CreatePen(PS_SOLID,1,y);
	old_pen = (HPEN) SelectObject(hdc,dpen);
	MoveToEx(hdc,rect.left,rect.top,NULL);
	LineTo(hdc,rect.right,rect.top);
	SelectObject(hdc,lpen);
	LineTo(hdc,rect.right,rect.bottom);
	LineTo(hdc,rect.left,rect.bottom);
	SelectObject(hdc,dpen);
	LineTo(hdc,rect.left,rect.top);
	SelectObject(hdc,old_pen);
	if (keep_dc == FALSE)
		fry_dc(hDlg,hdc);
	DeleteObject(dpen);
	DeleteObject(lpen);
}

void draw_custom_dialog_graphic(HWND hDlg, RECT rect, short which_g, Boolean do_frame,short win_or_gworld, short custom_pic){


	RECT from_rect = {0,0,28, 36};

	HBITMAP from_gworld;
	short draw_dest = 2,m_start_pic;
	HDC hdc;
	HBRUSH old_brush;
	RECT small_monst_rect = {0,0,14,18};

	if (win_or_gworld == 1)
		draw_dest = 0;

	if (which_g < 0)
		return;

	if (win_or_gworld == 0) {
		if (dlg_force_dc != NULL)
			hdc = dlg_force_dc;
			else hdc = GetDC(hDlg);
		}


    switch(custom_pic){
    case 1: // custom terrain, item or 1x1 monster
			from_gworld = spec_scen_g;
            from_rect = calc_rect(which_g % 10, which_g / 10);
			if (rect.right - rect.left != 28) {
				rect.left += 1;
				rect.right = rect.left + 28;
				}
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP) (hDlg): (HBITMAP)hdc)
			  ,rect,0,draw_dest);
        break;

	case 2: //custom 2x1 monster

				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0) {
					old_brush = (HBRUSH)SelectObject(hdc,GetStockObject(BLACK_BRUSH));
					Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
					SelectObject(hdc,old_brush);
					}
				from_gworld = spec_scen_g;
				m_start_pic = which_g;
				from_rect = calc_rect(m_start_pic % 10, m_start_pic / 10);
				OffsetRect(&small_monst_rect,rect.left,rect.top + 7);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

				m_start_pic = which_g + 1;
				from_gworld = spec_scen_g;
//				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(m_start_pic % 10, m_start_pic / 10);
				OffsetRect(&small_monst_rect,14,0);
				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

                break;
    case 3: //custom 1x2 monster

				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0) {
					old_brush = (HBRUSH) SelectObject(hdc,GetStockObject(BLACK_BRUSH));
					Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
					SelectObject(hdc,old_brush);
					}
				from_gworld = spec_scen_g;
				m_start_pic = which_g;
				from_rect = calc_rect(m_start_pic % 10, m_start_pic / 10);
				OffsetRect(&small_monst_rect,rect.left + 7,rect.top);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);
				m_start_pic = which_g + 1;
				from_gworld = spec_scen_g;
				from_rect = calc_rect(m_start_pic % 10, m_start_pic / 10);
				OffsetRect(&small_monst_rect,0,18);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

                break;

    case 4:	//custom 2x2 monster
                rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0) {
					old_brush = (HBRUSH)SelectObject(hdc,GetStockObject(BLACK_BRUSH));
					Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
					SelectObject(hdc,old_brush);
					}
				from_gworld = spec_scen_g;
				m_start_pic = which_g;
				from_rect = calc_rect(m_start_pic % 10, m_start_pic / 10);
				OffsetRect(&small_monst_rect,rect.left,rect.top);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

				m_start_pic = which_g + 1;
				from_gworld = spec_scen_g;
				from_rect = calc_rect(m_start_pic % 10, m_start_pic / 10);
				OffsetRect(&small_monst_rect,14,0);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

				m_start_pic = which_g + 2;
				from_gworld = spec_scen_g;
				from_rect = calc_rect(m_start_pic % 10, m_start_pic / 10);
				OffsetRect(&small_monst_rect,-14,18);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

				m_start_pic = which_g + 3;
				from_gworld = spec_scen_g;
				from_rect = calc_rect(m_start_pic % 10, m_start_pic / 10);
				OffsetRect(&small_monst_rect,14,0);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);
            break;

    case 5: // animated custom terrain

			from_gworld = spec_scen_g;
            from_rect = calc_rect(which_g % 10, which_g /10);
			if (rect.right - rect.left != 28) {
				rect.left += 1;
				rect.right = rect.left + 28;
				}
			from_rect.left += 28 * ((anim_ticks / 20) % 4);
			from_rect.right = from_rect.left + 28;
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,rect,0,draw_dest);
            break;
}
if ((win_or_gworld == 0) && (dlg_force_dc == NULL))
		fry_dc(hDlg, hdc);
	if ((win_or_gworld == 0) && (do_frame == TRUE)) {
      rect.right--; rect.bottom--;
		frame_dlog_rect(hDlg,rect,3);
		}
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
	RECT from2 = {0,0,36,36}, tiny_obj_rect = {0,0,18,18};
	RECT from_rect = {0,0,28, 36};
	RECT face_from = {0,0,32,32};
	RECT to_rect = {6,6,42,42};
	RECT pc_info_from = {0,127,106,157};
	RECT item_info_from = {174,0,312,112};

	HBITMAP from_gworld;
	short draw_dest = 2,m_start_pic;
	HDC hdc;
	HBRUSH old_brush;
	RECT small_monst_rect = {0,0,14,18};

	if (win_or_gworld == 1)
		draw_dest = 0;

	if (which_g < 0)
		return;

	if (which_g >= 2000)
		do_frame = FALSE;
	which_g = which_g % 2000;

	if (win_or_gworld == 0) {
		if (dlg_force_dc != NULL)
			hdc = dlg_force_dc;
			else hdc = GetDC(hDlg);
		}
	if (which_g == 950) { // Empty. Maybe clear space.
		if (win_or_gworld == 0) {
			rect.left -= 3;
			rect.top -= 3;
			rect.right += 3;
			rect.bottom += 4;
			paint_pattern((HBITMAP)hdc,2,rect,0);


			}
			//FillCRect(&rect,bg[5]);	// don't forget to nail dc!!!
		if (dlg_force_dc == NULL)
			fry_dc(hDlg, hdc);
		return;
		}

	switch (which_g / 100) {
		case 0: case 1: case 2: // terrain
			from_gworld = load_pict(800 + which_g / 50);
			which_g = which_g % 50;
			from_rect = calc_rect(which_g % 10, which_g / 10);
			if (rect.right - rect.left != 28) {
				rect.left += 1;
				rect.right = rect.left + 28;
				}
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP) (hDlg): (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			DeleteObject(from_gworld);
			break;
		case 3: // animated terrain
			which_g -= 300;
			if (rect.right - rect.left != 28) {
				rect.left += 1;
				rect.right = rect.left + 28;
				}
			from_gworld = load_pict(820);
//			from_rect = calc_rect(4 * (which_g / 5), which_g % 5);
			from_rect.left = 112 * (which_g / 5)+ 28 * ((anim_ticks / 20) % 4);
			from_rect.right = from_rect.left + 28;
			from_rect.top = 36 * (which_g % 5);
			from_rect.bottom = from_rect.top + 36;
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			DeleteObject(from_gworld);
			break;
		case 4: case 5: // monster
			// There are 4 different ways to draw, depending on size of monster
			which_g -= 400;
			m_start_pic = m_pic_index[which_g];

			if ((m_pic_index_x[which_g] == 1) && (m_pic_index_y[which_g] == 1)) {
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				rect.right = rect.left + 28; rect.bottom = rect.top + 36;
				if (win_or_gworld == 0) {
					old_brush = (HBRUSH)SelectObject(hdc,GetStockObject(BLACK_BRUSH));
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
					old_brush = (HBRUSH)SelectObject(hdc,GetStockObject(BLACK_BRUSH));
					Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
					SelectObject(hdc,old_brush);
					}
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,rect.left,rect.top + 7);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

				m_start_pic = m_pic_index[which_g] + 1;
				from_gworld = monst_gworld[m_start_pic / 20];
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
					old_brush = (HBRUSH) SelectObject(hdc,GetStockObject(BLACK_BRUSH));
					Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
					SelectObject(hdc,old_brush);
					}
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,rect.left + 7,rect.top);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);
				m_start_pic = m_pic_index[which_g] + 1;
				from_gworld = monst_gworld[m_start_pic / 20];
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
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,rect.left,rect.top);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

				m_start_pic = m_pic_index[which_g] + 1;
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,14,0);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);

				m_start_pic = m_pic_index[which_g] + 2;
				from_gworld = monst_gworld[m_start_pic / 20];
				m_start_pic = m_start_pic % 20;
				from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
				OffsetRect(&small_monst_rect,-14,18);

				if (win_or_gworld == 1)
					rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,small_monst_rect,0,0);
					else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,small_monst_rect,0,draw_dest);
				m_start_pic = m_pic_index[which_g] + 3;
				from_gworld = monst_gworld[m_start_pic / 20];
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
			if (rect.right - rect.left != 28) {
				rect.left += 1;
				rect.right = rect.left + 28;
				}
				if (win_or_gworld == 0) {
					old_brush = (HBRUSH) SelectObject(hdc,GetStockObject(WHITE_BRUSH));
					Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
					SelectObject(hdc,old_brush);
					}
			to_rect = rect;
			if (which_g < 45) {
				from_gworld = items_gworld;
				from_rect = calc_rect(which_g % 5, which_g / 5);
				}
				else {
					from_gworld = tiny_obj_gworld;
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

		case 10: // talk face
			which_g -= 1000;
			from_gworld = talkfaces_gworld;
			from_rect = face_from;
			OffsetRect(&from_rect,32 * ((which_g - 1) % 10),32 * ((which_g - 1) / 10));
			if (win_or_gworld == 1)
				 rect_draw_some_item(from_gworld,from_rect,(HBITMAP) hDlg,rect,0,0);
				else rect_draw_some_item(from_gworld,from_rect,(HBITMAP)hdc,rect,0,draw_dest);
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
		case 16:
			which_g -= 1600;
			from_gworld = load_pict(851);
			from_rect.right = 32;
			from_rect.bottom = 32;
			OffsetRect(&from_rect,32 * (which_g % 5),32 * (which_g / 5));
			rect.right = rect.left + 32;
			rect.bottom = rect.top + 32;
			rect_draw_some_item(from_gworld,from_rect,(HBITMAP) ((win_or_gworld == 1) ? (HBITMAP)hDlg: (HBITMAP)hdc)
			  ,rect,0,draw_dest);
			DeleteObject(from_gworld);
			break;
		}

	if ((win_or_gworld == 0) && (dlg_force_dc == NULL))
		fry_dc(hDlg, hdc);
	if ((win_or_gworld == 0) && (do_frame == TRUE)) {
      rect.right--; rect.bottom--;
		frame_dlog_rect(hDlg,rect,3);
		}
}

void showcursor(Boolean a)
{
	short i;
	i = ShowCursor(a);
	if (a == FALSE)
		while (i >= 0)
			i = ShowCursor(FALSE);
	if (a == TRUE)
		while (i < 0)
			i = ShowCursor(TRUE);
	}

void ModalDialog()
{
	MSG msg;

		while ((dialog_not_toast == TRUE) && (GetMessage(&msg,NULL,0,0))) {
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

void InsetRect(RECT *rect,short x, short y)
{
 	InflateRect(rect,-1 * x, -1 * y);
}
