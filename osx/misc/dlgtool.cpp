/*
 *  dlogtool.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 15/04/09.
 *
 */

#include <stdio.h>
#include <string.h>
#include <vector>
using std::vector;
//#include <algorithm>

#include "dlgconsts.h"
#include "dlgtool.h"
#include "dlglowlevel.h"

#include "soundtool.h"
#include "graphtool.h"
#include "mathutil.h"

extern void (*redraw_screen)();
extern bool play_sounds, cursor_shown;
extern WindowPtr mainPtr;
extern bool modeless_exists[18];
extern DialogPtr modeless_dialogs[18];

extern PixPatHandle bg[];
extern short geneva_font_num, sword_curs;

GWorldPtr dlg_buttons_gworld[NUM_BUTTONS][2];
map<char*,GWorldPtr*> dlg_gworlds;

short current_key = 0;
dlg_t dlgs[ND];
dlg_item_t items[NI];

char text_long_str[10][256];
char text_short_str[140][40];
dlg_label_t labels[NL];
bool dlg_not_toast;
RGBColor clr[] = {{65535,65535,65535},{65535,0,0},{0,0,8192},{0,0,4096}};
short dlg_bg = 5;

#ifdef EXILE_BIG_GUNS
main_dialog_UPP = NewModalFilterProc(cd_event_filter);
#endif
//vector<dlg_filters> event_filters;
map<short, dlg_filter_t> event_filters;
extern short available_dlog_buttons[NUM_DLOG_B];
extern btn_t buttons[];

void cd_init_dialogs(INIT_PARAMS){
	short i,j;
	
	for (i = 0; i < ND; i++) {
		dlgs[i].key = -1;
		dlgs[i].type = 0;
		dlgs[i].win = NULL;
		dlgs[i].highest_item = 0;
	}
	for (i = 0; i < NI; i++) {
		items[i].dlg = -1;
	}
	for (i = 0; i < NL; i++) {
		labels[i].taken = FALSE;
	}
	for (i = 0; i < NUM_BUTTONS; i++){
		for (j = 0; j < 2; j++)
			dlg_buttons_gworld[i][j] = load_pict(2000 + (2 * i) + j);
	}
	//dlg_gworlds.swap(gw);
	//copy(gw.begin(), gw.end(), dlg_gworlds.begin())
	//dlg_gworlds = gw;
	//for(map<char*,GWorldPtr*>::iterator it = gw.begin(); it != gw.end(); it++){
	//	printf("Copying gworld %s...\n",it->first);
	//	dlg_gworlds[it->first] = it->second;
	//}
	dlg_gworlds["anim"] = g1;
	dlg_gworlds["talkfaces"] = g2;
	dlg_gworlds["items"] = g3;
	dlg_gworlds["tiny-obj"] = g4;
	dlg_gworlds["pc"] = g5;
	dlg_gworlds["dlog"] = g6;
	dlg_gworlds["monst"] = g7;
	dlg_gworlds["ter"] = g8;
	dlg_gworlds["small-ter"] = g9;
	dlg_gworlds["fields"] = g10;
	dlg_gworlds["pc-stats"] = g11;
	dlg_gworlds["item-stats"] = g12;
//	dlg_gworlds["text-area"] = g13;
//	dlg_gworlds["storage"] = g14;
//	dlg_gworlds["ter-scr"] = g15;
//	dlg_gworlds["text_bar"] = g16;
//	dlg_gworlds["orig-text-bar"] = g17;
//	dlg_gworlds["buttons"] = g18;
//	dlg_gworlds["party-tmpl"] = g19;
	dlg_gworlds["mixed"] = g20;
	dlg_gworlds["custom"] = g21;
	//event_filters.push_back(dlg_filters(0,NULL)); // default event filter placeholder
}


short cd_create_dialog_parent_num(short dlog_num,short parent){
	short i;
	
	if ((parent == 0) || (parent == 1))
		return cd_create_dialog(dlog_num,mainPtr);
	i = cd_get_dlg_index(parent);
	if (i < 0)
		return -1;
	return cd_create_dialog(dlog_num,dlgs[i].win);
	
}

short cd_create_dialog(short dlog_num,WindowPtr parent){
	short i,j,free_slot = -1,free_item = -1;
	WindowPtr dlg;
	
	char item_str[256];
	short type,flag;
	
	for (i = 0; i < ND; i++) {
		if ((dlgs[i].key >= 0) && (dlgs[i].type == dlog_num))
			return -1;
	}
	for (i = 0; i < ND; i++) {
		if (dlgs[i].key < 0) {
			free_slot = i;
			break;
		}
	}
	if (free_slot < 0)
		return -2;
	current_key++;
	dlgs[free_slot].key = current_key;
	dlgs[free_slot].type = dlog_num;
	dlgs[free_slot].highest_item = 1;
	dlgs[free_slot].draw_ready = false;
	dlgs[free_slot].win = NULL;
	
	// first, create dummy dlog
	dlgs[free_slot].win = GetDialogWindow(GetNewDialog (dlog_num, NULL, IN_FRONT));
	if (dlgs[free_slot].win == NULL) {
		play_sound(57);
		return -3;
	}
	
	dlgs[free_slot].parent = parent;
	
	process_new_window (free_slot);
	ShowWindow(dlgs[free_slot].win);
	SetPortWindowPort(dlgs[free_slot].win);
	ForeColor(blackColor);
	BackColor(whiteColor);
	untoast_dialog();
	set_cursor(sword_curs);
	
	return 0;
}

short cd_kill_dialog(short dlog_num,short parent_message){
	short i,which_dlg = -1;
	
	for (i = 0; i < ND; i++)
		if ((dlgs[i].key >= 0) && (dlgs[i].type == dlog_num))
			which_dlg = i;
	if (which_dlg < 0)
		return -1;
	
	for (i = 0; i < NI; i++)
		if (items[i].dlg == dlog_num) {
			if (items[i].label > 0)
				labels[items[i].label_loc].taken = FALSE;
			items[i].dlg = -1;
		}
	
	DisposeDialog(GetDialogFromWindow(dlgs[which_dlg].win));
	dlgs[which_dlg].key = -1;
	if (dlgs[which_dlg].parent != NULL)
		SetPortWindowPort( dlgs[which_dlg].parent);
	if (redraw_screen != NULL && FrontWindow() == mainPtr) // Was formerly checkin != mainPtr for some reason...
		redraw_screen();
	untoast_dialog();
	return 0;
}

bool type_is_clickable(short t){
	if(t == DLG_BUTTON) return true;
	if(t == DLG_DEFAULT_BTN) return true;
	if(t == DLG_LED_BUTTON) return true;
	if(t == DLG_TEXT_CLICKABLE) return true;
	if(t == DLG_CUSTOM_BTN) return true;
	if(t == DLG_CUSTOM_DEF_BTN) return true;
	if(t == DLG_HIDDEN_BUTTON) return true;
	return false;
}

short cd_process_click(WindowPtr window,Point the_point, short mods,short *item){
	short i,which_dlg,dlg_num,item_id;
	short dlog_key;
	
	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlog_key)) < 0)
		return -1;
	
	for (i = 0; i <= dlgs[which_dlg].highest_item; i++)
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
			if ((PtInRect(the_point,&items[item_id].rect)) && (items[item_id].active > 0)
				&& (type_is_clickable(items[item_id].type))) {
				*item = i;
				if (mods & controlKey) 
					*item += 100;
				if (items[item_id].type != 8)
					cd_press_button(dlg_num,i);
				return dlg_num;
			}
		}
	return -1;
}

short cd_process_keystroke(WindowPtr window,char char_hit,short *item){
	short i,which_dlg,dlg_num,dlg_key,item_id;
	
	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return -1;
	
	for (i = 0; i < dlgs[which_dlg].highest_item + 1; i++){
		if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
			if ((items[item_id].key == char_hit) && (items[item_id].active > 0)
				&&  (type_is_clickable(items[item_id].type))) {
				*item = i;
				if (items[item_id].type != 8)
					cd_press_button(dlg_num,i);
				return dlg_num;
			}
		}
	}
	
	// kludgy. If you get an escape and it isn't processed, make it an enter
	if (char_hit == 24) {
		char_hit = 31;
		for (i = 0; i < dlgs[which_dlg].highest_item + 1; i++){
			if ((item_id = cd_get_item_id(dlg_num,i)) >= 0) {
				if ((items[item_id].key == char_hit) && (items[item_id].active > 0)
					&&  (type_is_clickable(items[item_id].type))) {
					*item = i;
					if (items[item_id].type != 8)
						cd_press_button(dlg_num,i);
					return dlg_num;
				}
			}
		}
	}
	return -1;
}

void cd_attach_key(short dlog_num,short item_num,char key){
	short dlg_index,item_index;
	
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	
	if (!type_is_clickable(items[item_index].type)) {
		SysBeep(20);
		return;
	}
	items[item_index].key = key;
}

void csp(short dlog_num, short item_num, short pict_num, short pict_type){
	cd_set_pict( dlog_num,  item_num,  pict_num,  pict_type);
}

void cd_set_pict(short dlog_num, short item_num, short pict_num, short pict_type){
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (items[item_index].type < DLG_NEW_PICTURE && items[item_index].type != DLG_OLD_PICTURE) {
		SysBeep(20);
		return;
	}
	items[item_index].flag = pict_num;
	items[item_index].type = DLG_NEW_PICTURE + pict_type;
	if (pict_num == -1)
		cd_erase_item(dlog_num,item_num);
	else cd_draw_item(dlog_num,item_num);
}

void cd_activate_item(short dlog_num, short item_num, short status){
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	
	items[item_index].active = status;
	cd_draw_item(dlog_num,item_num);
}

short cd_get_active(short dlog_num, short item_num){
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return -1;
	
	return items[item_index].active;
}

void cd_get_item_text(short dlog_num, short item_num, char *str){
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_index >= 150) {
		SysBeep(20);
		return;
	}
	if (item_index < 10)
		sprintf(str,"%s",text_long_str[item_index]);
	else sprintf(str,"%s",text_short_str[item_index - 10]);
}

void csit(short dlog_num, short item_num, char *str){
	cd_set_item_text( dlog_num,  item_num, str);
}

void cd_set_item_text(short dlog_num, short item_num, char *str){
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_index >= 150) {
		SysBeep(20);
		return;
	}
	if (item_index < 10)
		sprintf(text_long_str[item_index],"%s",str);
	else sprintf(text_short_str[item_index - 10],"%-39.39s",str);
	cd_draw_item( dlog_num,item_num);
}

void cd_retrieve_text_edit_str(short dlog_num, short item_num, char *str){
	short dlg_index,item_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	
	sprintf(str,"");
	dlg_index = cd_get_dlg_index(dlog_num);
	if (dlg_index < 0) {
		SysBeep(50); return ;
	}
	GetDialogItem(GetDialogFromWindow(dlgs[dlg_index].win), item_num, &the_type, &the_handle, &the_rect);
	GetDialogItemText(the_handle,store_ptr);
	p2cstr(store_ptr);
	strcpy(str,(char *) store_ptr);
}

short cd_retrieve_text_edit_num(short dlog_num, short item_num){
	short dlg_index,item_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	long num_given;
	
	dlg_index = cd_get_dlg_index(dlog_num);
	if (dlg_index < 0) {
		SysBeep(50); return -1;
	}
	GetDialogItem(GetDialogFromWindow( dlgs[dlg_index].win), item_num, &the_type, &the_handle, &the_rect);
	GetDialogItemText(the_handle,store_ptr);
	StringToNum (store_ptr,&num_given);
	return (short) num_given;
}

// NOTE!!! Expects a c string
void cd_set_text_edit_str(short dlog_num, short item_num, char *str){
	short dlg_index,item_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	
	dlg_index = cd_get_dlg_index(dlog_num);
	if (dlg_index < 0) {
		SysBeep(50); return ;
	}
	strcpy((char *) store_ptr,str);
	c2pstr((char*) store_ptr);
	GetDialogItem(GetDialogFromWindow( dlgs[dlg_index].win), item_num, &the_type, &the_handle, &the_rect );
	SetDialogItemText ( the_handle, store_ptr);	
}

void cd_set_text_edit_num(short dlog_num, short item_num, short num){
	short dlg_index,item_index;
	short the_type;
	Handle the_handle = NULL;
	Rect the_rect;
	Str255 store_ptr;
	
	dlg_index = cd_get_dlg_index(dlog_num);
	if (dlg_index < 0) {
		SysBeep(50);
		return;
	}
	
	sprintf((char *) store_ptr,"%d",num);
	c2pstr((char*) store_ptr);
	GetDialogItem( GetDialogFromWindow(dlgs[dlg_index].win), item_num, &the_type, &the_handle, &the_rect );
	SetDialogItemText ( the_handle, store_ptr);	
}

void cdsin(short dlog_num, short item_num, short num) {
	cd_set_item_num( dlog_num,  item_num,  num);
}


void cd_set_item_num(short dlog_num, short item_num, short num){
	short dlg_index,item_index;
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return ;
	if (item_index >= 150) {
		SysBeep(20);
		return;
	}
	if (item_index < 10)
		sprintf(text_long_str[item_index],"%d",num);
	else sprintf(text_short_str[item_index - 10],"%d",num);
	cd_draw_item( dlog_num,item_num);
}

void cd_set_led(short dlog_num,short item_num,short state){
	short dlg_index,item_index;
	
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	
	if (items[item_index].type != DLG_LED_BUTTON) {
		SysBeep(20);
		return;
	}
	items[item_index].flag = state;
	cd_draw_item(dlog_num,item_num);
}

// Use when checking for events ... if item hit matches item num, this led was
// hit, and flip it to other thing.
void cd_flip_led(short dlog_num,short item_num,short item_hit){
	if (item_hit != item_num)
		return;
	if (cd_get_led(dlog_num,item_num) > 0)
		cd_set_led(dlog_num,item_num,0);
	else cd_set_led(dlog_num,item_num,1);
}

void cd_set_led_range(short dlog_num,short first_led,short last_led,short which_to_set){
	short i;
	
	for (i = first_led; i <= last_led; i++) {
		if (i - first_led == which_to_set)
			cd_set_led(dlog_num,i,1);
		else cd_set_led(dlog_num,i,0);
	}
}

// Use when someone presses something in the range, to light up the right button
// Also has an error check ... if which_to_set is outside range, do nothing
void cd_hit_led_range(short dlog_num,short first_led,short last_led,short which_to_set){
	short i;
	
	if ((which_to_set < first_led) || (which_to_set > last_led))
		return;
	for (i = first_led; i <= last_led; i++) {
		if (i == which_to_set)
			cd_set_led(dlog_num,i,1);
		else cd_set_led(dlog_num,i,0);
	}
}

short cd_get_led_range(short dlog_num,short first_led,short last_led){
	short i;
	
	for (i = first_led; i <= last_led; i++) {
		if (cd_get_led(dlog_num,i) == 1)
			return i - first_led;
	}
	return 0;
}

void cd_set_flag(short dlog_num,short item_num,short flag){
	short dlg_index,item_index;
	
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	
	items[item_index].flag = flag;
	cd_draw_item(dlog_num,item_num);
}

short cd_get_led(short dlog_num,short item_num){
	short dlg_index,item_index;
	
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return 0;
	
	if (items[item_index].type != DLG_LED_BUTTON) {
		SysBeep(20);
		return 0;
	}
	return items[item_index].flag;
}

void cd_text_frame(short dlog_num,short item_num,short frame){
	short dlg_index,item_index;
	
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	
	if (item_index >= 150) {
		SysBeep(20);
		return;
	}
	items[item_index].flag = frame;
	cd_draw_item(dlog_num,item_num);
}

void cd_add_label(short dlog_num, short item_num, char *label, short label_flag){
	short dlg_index,item_index,label_loc = -1;
	short i;
	
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	
	if (items[item_index].label_loc < 0) {
		items[item_index].label = label_flag;
		for (i = 0; i < 100; i++)
			if (labels[i].taken == FALSE) {
				label_loc = i;
				labels[i].taken = TRUE;
				i = 100;
			}
		if (label_loc < 0) {
			SysBeep(20);
			return;
		}
		items[item_index].label_loc = label_loc;
	}
	else cd_erase_item(dlog_num,item_num + 100);
	label_loc = items[item_index].label_loc;
	sprintf((char *) labels[label_loc].str,"%-24s",label);
	if (items[item_index].active > 0)
		cd_draw_item(dlog_num,item_num);
}

void cd_take_label(short dlog_num, short item_num){
	short dlg_index,item_index;
	
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	items[item_index].label = 0;
	labels[items[item_index].label_loc].taken = FALSE;
}

void cd_key_label(short dlog_num, short item_num,short loc){
	short dlg_index,item_index;
	char str[10];
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	sprintf((char *) str,"  ");
	str[0] = items[item_index].key;
	cd_add_label(dlog_num,item_num, str, 7 + loc * 100);
}

void cd_draw_item(short dlog_num,short item_num){
	short dlg_index,item_index,store_label;
	Rect from_rect,to_rect;
	GrafPtr old_port;
	//printf("In cd_draw_item(%i,%i)\n",dlog_num,item_num);
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	if (dlgs[dlg_index].draw_ready == FALSE)
		return;
	//printf("Drawing item %i of type %i\n",item_num,items[item_index].type);
	GetPort(&old_port);
	SetPortWindowPort(dlgs[dlg_index].win);
	TextFont(geneva_font_num);
	TextFace(bold);
	TextSize(10);
	
	if (items[item_index].active == 0) {
		cd_erase_item(dlog_num,item_num);
		cd_erase_item(dlog_num,item_num + 100);
	}
	else {
		switch (items[item_index].type) {
		case DLG_HIDDEN_BUTTON:
			/// it's hidden, duh! Hence we don't do anything.
			break;
		case DLG_BUTTON: case DLG_DEFAULT_BTN:
			GetPortBounds(dlg_buttons_gworld[buttons[items[item_index].flag].type][0], &from_rect);
			rect_draw_some_item(dlg_buttons_gworld[buttons[items[item_index].flag].type][0],from_rect,
								(GWorldPtr)dlgs[dlg_index].win,//dlg_buttons_gworld[buttons[items[item_index].flag].type][0],
								items[item_index].rect,0,2);
			RGBForeColor(&clr[2]);
			TextSize(12);
			OffsetRect(&items[item_index].rect,-1 * buttons[items[item_index].flag].left_adj,0);
			char_win_draw_string(dlgs[dlg_index].win,items[item_index].rect,
								 (char *) (buttons[items[item_index].flag].str),1,8,false);
			OffsetRect(&items[item_index].rect,buttons[items[item_index].flag].left_adj,0);
			TextSize(10);
			ForeColor(blackColor);
			break;
		case DLG_CUSTOM_BTN: case DLG_CUSTOM_DEF_BTN:
			GetPortBounds(dlg_buttons_gworld[buttons[items[item_index].flag].type][0], &from_rect);
			rect_draw_some_item(dlg_buttons_gworld[buttons[items[item_index].flag].type][0],from_rect,
								(GWorldPtr)dlgs[dlg_index].win,//dlg_buttons_gworld[buttons[items[item_index].flag].type][0],
								items[item_index].rect,0,2);
			RGBForeColor(&clr[2]);
			TextSize(12);
			char_win_draw_string(dlgs[dlg_index].win,items[item_index].rect,
								 (char *) ((item_index < 10) ? text_long_str[item_index] : 
										   text_short_str[item_index - 10]),1,8,false);
			TextSize(10);
			ForeColor(blackColor);
			break;
		
		case DLG_LED_BUTTON:
			GetPortBounds(dlg_buttons_gworld[9][0], &from_rect);
			switch (items[item_index].flag) {
			case 0:
				rect_draw_some_item(dlg_buttons_gworld[10][0],
									from_rect, (GWorldPtr)dlgs[dlg_index].win,//dlg_buttons_gworld[10][0],
									items[item_index].rect,0,2);
				break;
			case 1:
				rect_draw_some_item(dlg_buttons_gworld[9][1],
									from_rect, (GWorldPtr)dlgs[dlg_index].win,//dlg_buttons_gworld[9][1],
									items[item_index].rect,0,2);
				break;
			case 2:
				rect_draw_some_item(dlg_buttons_gworld[9][0],
									from_rect, (GWorldPtr)dlgs[dlg_index].win,//dlg_buttons_gworld[9][0],
									items[item_index].rect,0,2);
				break;
			}
			break;
				
		case DLG_TEXT_BOLD: case DLG_TEXT_PLAIN: case DLG_TEXT_LARGE:
		case DLG_TEXT_CLICKABLE: case DLG_TEXT_DEFAULT:
			cd_erase_item(dlog_num,item_num);
			TextFont(geneva_font_num);
			TextFace(0);
			TextFace(bold);
			TextSize(10);
			if (items[item_index].type == DLG_TEXT_PLAIN)
				TextFace(0);
			if (items[item_index].type == DLG_TEXT_LARGE)
				TextSize(12); 
			ForeColor(blackColor);
			if (items[item_index].flag % 10 == 1)
				cd_frame_item(dlog_num,item_num,2);
			if (items[item_index].flag >= 10) {
				RGBForeColor(&clr[1]);
			}else RGBForeColor(&clr[0]);
			if (items[item_index].rect.bottom - items[item_index].rect.top < 20) {
				items[item_index].rect.left += 3;
				char_win_draw_string(dlgs[dlg_index].win,items[item_index].rect,
									 (char *) ((item_index < 10) ? text_long_str[item_index] : 
											   text_short_str[item_index - 10]),3,12,false);
				items[item_index].rect.left -= 3;
			}
			else {
				InsetRect(&items[item_index].rect,4,4);
				char_win_draw_string(dlgs[dlg_index].win,items[item_index].rect,
									 (char *) ((item_index < 10) ? text_long_str[item_index] : 
											   text_short_str[item_index - 10]),0,(items[item_index].type == DLG_TEXT_LARGE) ? 14 : 12,false);
				InsetRect(&items[item_index].rect,-4,-4);
			}
//			if ((items[item_index].type == DLG_TEXT_CLICKABLE) && (dlog_num == 989)) {
//				items[item_index].rect.bottom -= 12;
//				undo_clip();
//			} // Why the special case?
			break;
				
		case DLG_OLD_PICTURE:
			if (items[item_index].flag == -1)
				cd_erase_item(dlog_num,item_num);
			else draw_dialog_graphic(GetWindowPort(dlgs[dlg_index].win), items[item_index].rect, items[item_index].flag,
									 PICT_OLD, (items[item_index].flag >= 2000) ? FALSE : TRUE,0);
			break;
		default: // DLG_NEW_PICTURE
			printf("Type = %i\t",items[item_index].type);
			printf("Flag = %i\n",items[item_index].flag);
			if(items[item_index].type < DLG_NEW_PICTURE) break;
			if (items[item_index].flag == -1)
				cd_erase_item(dlog_num,item_num);
			else draw_dialog_graphic(GetWindowPort(dlgs[dlg_index].win), items[item_index].rect, items[item_index].flag,
									 items[item_index].type - DLG_NEW_PICTURE, (items[item_index].flag >= 2000) ? FALSE : TRUE,0);
			break;
		}
	}//printf("Item %i drawn.\n",item_num);
	
	if (items[item_index].label != 0 && items[item_index].type != DLG_HIDDEN_BUTTON) {
		store_label = items[item_index].label;
		if (store_label >= 1000) {
			store_label -= 1000;
		}
		else {
			TextFace(0);
		}
		to_rect = items[item_index].rect;
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
		if (items[item_index].active != 0) {
			char_win_draw_string(dlgs[dlg_index].win,to_rect,
								 labels[items[item_index].label_loc].str,2,12,false);
			
		}
		TextFace(bold);
	}
	
	TextFont(0);
	TextFace(0);
	TextSize(12);
	SetPort(old_port);
}

void cd_initial_draw(short dlog_num){
	short i,which_dlg = -1;
	
	for (i = 0; i < ND; i++)
		if ((dlgs[i].key >= 0) && (dlgs[i].type == dlog_num))
			which_dlg = i;
	if (which_dlg < 0) {
		return;
	}
	dlgs[which_dlg].draw_ready = TRUE;
	
	cd_erase_item(dlog_num, 0);
	cd_draw(dlog_num);
}

void cd_draw(short dlog_num){
	short i,which_dlg = -1;
	
	for (i = 0; i < ND; i++)
		if ((dlgs[i].key >= 0) && (dlgs[i].type == dlog_num))
			which_dlg = i;
	if (which_dlg < 0)
		return;
	for (i = 0; i < dlgs[which_dlg].highest_item + 1; i++) {
		cd_draw_item(dlog_num,i);
	}
}

void cd_redraw(WindowPtr window){
	short which_dlg,dlg_num,dlg_key;
	
	if ((which_dlg = cd_find_dlog(window,&dlg_num,&dlg_key)) < 0)
		return;
	dlgs[which_dlg].draw_ready = TRUE;
	cd_initial_draw(dlg_num);
}

void cd_frame_item(short dlog_num, short item_num, short width){
	short dlg_index,item_index;
	
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	frame_dlog_rect(GetWindowPort(dlgs[dlg_index].win), items[item_index].rect, width);
}

void cd_erase_item(short dlog_num, short item_num, Boolean just_label){
	// if item_num is 0, nail whole window
	// item_num + 100  just erase label
	short i,dlg_index,item_index,store_label;
	Rect to_fry;
	GrafPtr old_port;
	
	if (item_num >= 100) {
		item_num -= 100;
		just_label = TRUE;
	}
	
	if (item_num == 0) {
		for (i = 0; i < ND; i++)
			if ((dlgs[i].key >= 0) && (dlgs[i].type == dlog_num))
				dlg_index = i;
		//GetWindowRect(dlgs[dlg_index],&to_fry);
		GetWindowPortBounds(dlgs[dlg_index].win,&to_fry);
		//OffsetRect(&to_fry,-1 * to_fry.left,-1 * to_fry.top);
	}
	else {
		if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
			return;
		to_fry = items[item_index].rect;
		if (just_label == TRUE) {
			if (items[item_index].label != 0) {
				store_label = items[item_index].label;
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
	if (dlgs[dlg_index].draw_ready == FALSE) {
		return;
	}
	GetPort(&old_port);
	SetPortWindowPort(dlgs[dlg_index].win);
	FillCRect(&to_fry,bg[dlg_bg]);
	SetPort(old_port);
}

void cd_erase_rect(short dlog_num,Rect to_fry){
	short i,dlg_index,item_index,store_label;
	GrafPtr old_port;
	
	if ((dlg_index = cd_get_dlg_index(dlog_num)) < 0)
		return;
	if (dlgs[dlg_index].draw_ready == FALSE)
		return;
	
	GetPort(&old_port);
	SetPortWindowPort(dlgs[dlg_index].win);
	FillCRect(&to_fry,bg[dlg_bg]);
	SetPort(old_port);
}

void cd_press_button(short dlog_num, short item_num){
	short dlg_index,item_index;
	unsigned long dummy;
	Rect from_rect,to_rect,x = {0,0,55,55};
	GrafPtr old_port;
	//RGBColor clr[2] = {{0,0,4096},{0,0,8192}};
	
	if (cd_get_indices(dlog_num,item_num,&dlg_index,&item_index) < 0)
		return;
	// no press action for redio buttons
	if (items[item_index].type == DLG_LED_BUTTON) {
		play_sound(34);
		return;
	}	
	
	GetPort(&old_port);
	SetPortWindowPort(dlgs[dlg_index].win);
	TextFont(geneva_font_num);
	TextFace(bold);
	TextSize(10);
	
	ForeColor(blackColor);
	TextSize(12);
	GetPortBounds(dlg_buttons_gworld[buttons[items[item_index].flag].type][0], &from_rect);
	rect_draw_some_item(dlg_buttons_gworld[buttons[items[item_index].flag].type][1],from_rect,
						(GWorldPtr)dlgs[dlg_index].win,//dlg_buttons_gworld[buttons[items[item_index].flag].type][1],
						items[item_index].rect,0,2);
	TextFace(bold);
	RGBForeColor(&clr[3]);
	if (items[item_index].type < DLG_LED_BUTTON) {
		OffsetRect(&items[item_index].rect,-1 * buttons[items[item_index].flag].left_adj,0);
		char_win_draw_string(dlgs[dlg_index].win,items[item_index].rect,
							 (char *) (buttons[items[item_index].flag].str),1,8,false);
		OffsetRect(&items[item_index].rect,buttons[items[item_index].flag].left_adj,0);
	}
	else {
		char_win_draw_string(dlgs[dlg_index].win,items[item_index].rect,
							 (char *) ((item_index < 10) ? text_long_str[item_index] : 
									   text_short_str[item_index - 10]),1,8,false);
	}
	
	if (play_sounds == TRUE) {
		play_sound(37);
		Delay(6,&dummy);
	}
	else Delay(14,&dummy);
	
	rect_draw_some_item(dlg_buttons_gworld[buttons[items[item_index].flag].type][0],from_rect,
						(GWorldPtr)dlgs[dlg_index].win,//dlg_buttons_gworld[buttons[items[item_index].flag].type][0],
						items[item_index].rect,0,2);
	
	RGBForeColor(&clr[2]);
	if (items[item_index].type < DLG_LED_BUTTON) {
		OffsetRect(&items[item_index].rect,-1 * buttons[items[item_index].flag].left_adj,0);
		char_win_draw_string(dlgs[dlg_index].win,items[item_index].rect,
							 (char *) (buttons[items[item_index].flag].str),1,8,false);
		OffsetRect(&items[item_index].rect,buttons[items[item_index].flag].left_adj,0);
	}
	else {
		char_win_draw_string(dlgs[dlg_index].win,items[item_index].rect,
							 (char *) ((item_index < 10) ? text_long_str[item_index] : 
									   text_short_str[item_index - 10]),1,8,false);
	}
	
	TextFont(0);
	TextFace(0);
	TextSize(12);
	ForeColor(blackColor);
	Delay(8,&dummy);
	
	SetPort(old_port);
}

bool dialog_not_toast(){
	return dlg_not_toast;
}

void toast_dialog(){
	dlg_not_toast = false;
}

void untoast_dialog(){
	dlg_not_toast = true;
}

short cd_run_dialog(){
	short item_hit;
	untoast_dialog();
#ifndef EXILE_BIG_GUNS
	while (dialog_not_toast())
		ModalDialog((ModalFilterProcPtr) cd_event_filter, &item_hit);
#endif		
#ifdef EXILE_BIG_GUNS
	while (dialog_not_toast())
		ModalDialog(main_dialog_UPP, &item_hit);
#endif
	return item_hit;
}

void cd_register_event_filter(short dlg_id, dlg_filter_t filter){
	//event_filters.push_back(dlg_filters(dlg_id,filter));
	event_filters[dlg_id] = filter;
}

void cd_register_default_event_filter(dlg_filter_t filter){
	//event_filters[0] = dlg_filters(0,filter);
	event_filters[0] = filter;
}

pascal Boolean cd_event_filter (DialogPtr hDlg, EventRecord *event, short *dummy_item_hit){	
	char chr,chr2;
	short wind_hit,item_hit;
	Point the_point;
	WindowRef w;
	RgnHandle rgn;
	
	dummy_item_hit = 0;
	
	switch (event->what) {
		case updateEvt:
			w = GetDialogWindow(hDlg);
			rgn = NewRgn();
			GetWindowRegion(w,kWindowUpdateRgn,rgn);
			if (EmptyRgn(rgn) == TRUE) {
				DisposeRgn(rgn);
				return FALSE;
			}
			DisposeRgn(rgn);
			BeginUpdate(w);
			cd_redraw(w);
			EndUpdate(w);
			DrawDialog(hDlg);
			return TRUE;
			break;
			
		case keyDown:
			chr = event->message & charCodeMask;
			chr2 = (char) ((event->message & keyCodeMask) >> 8);
			switch (chr2) {
				case 126:
					chr = DLG_KEY_UP;
					break;
				case 124:
					chr = DLG_KEY_RIGHT;
					break;
				case 123:
					chr = DLG_KEY_LEFT;
					break;
				case 125:
					chr = DLG_KEY_DOWN;
					break;
				case 53:
					chr = DLG_KEY_ESC;
					break;
				case 36:
					chr = DLG_KEY_RETURN;
					break;
				case 76:
					chr = DLG_KEY_RETURN;
					break;
			}
			if(event->modifiers & controlKey) switch(chr){ // these aren't currently used
				case '1':
					chr = DLG_KEY_CTRL1;
					break;
				case '2':
					chr = DLG_KEY_CTRL2;
					break;
				case '3':
					chr = DLG_KEY_CTRL3;
					break;
				case '4':
					chr = DLG_KEY_CTRL4;
					break;
				case '5':
					chr = DLG_KEY_CTRL5;
					break;
				case '6':
					chr = DLG_KEY_CTRL6;
					break;
			}
			// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
			// 25-30  ctrl 1-6  31 - return
			
			wind_hit = cd_process_keystroke(GetDialogWindow(hDlg),chr,&item_hit);
			break;
			
		case mouseDown:
			the_point = event->where;
			GlobalToLocal(&the_point);	
			wind_hit = cd_process_click(GetDialogWindow(hDlg),the_point, event->modifiers,&item_hit);
			break;
			
		default:
			wind_hit = -1;
			break;
	}
//	bool handled = false;
//	for(int i = 1; i < event_filters.size(); i++){
//		if(event_filters[i].id == wind_hit){
//			event_filters[i].callback(item_hit);
//			handled = true;
//		}
//	}
//	if(!handled)
//		event_filters[0].callback(item_hit);
	if(wind_hit >= 0){
		if(event_filters.find(wind_hit) == event_filters.end())
			event_filters[0](item_hit);
		else
			event_filters[wind_hit](item_hit);
	}
//	switch (wind_hit) {
//		case -1: break;
//		case 823: give_password_filter(item_hit); break;
//		case 869: pick_prefab_scen_event_filter(item_hit); break;
//		case 947: pick_a_scen_event_filter(item_hit); break;
//		case 958: tip_of_day_event_filter(item_hit); break;
//		case 960: talk_notes_event_filter(item_hit); break;
//		case 961: adventure_notes_event_filter(item_hit); break;
//		case 962: journal_event_filter(item_hit); break;
//		case 970: case 971: case 972: case 973: display_strings_event_filter(item_hit); break;
//		case 987: display_item_event_filter(item_hit); break;
//		case 988: pick_trapped_monst_event_filter(item_hit); break;
//		case 989: edit_party_event_filter(item_hit); break;
//		case 991: display_pc_event_filter(item_hit); break;
//		case 996: display_alchemy_event_filter(item_hit); break;
//		case 997: display_help_event_filter(item_hit); break;
//		case 998: display_pc_item_event_filter(item_hit); break;
//		case 999: display_monst_event_filter(item_hit); break;
//		case 1010: spend_xp_event_filter (item_hit); break;
//		case 1012: get_num_of_items_event_filter (item_hit); break;
//		case 1013: pick_race_abil_event_filter (item_hit); break;
//		case 1014: sign_event_filter (item_hit); break;
//		case 1017: case 873: get_text_response_event_filter (item_hit); break;
//		case 1018: select_pc_event_filter (item_hit); break;
//		case 1019: give_pc_info_event_filter(item_hit); break;
//		case 1047: alch_choice_event_filter(item_hit); break;
//		case 1050: pc_graphic_event_filter(item_hit); break;
//		case 1051: pc_name_event_filter(item_hit); break;
//		case 1073: give_reg_info_event_filter (item_hit); break;
//		case 1075: do_registration_event_filter (item_hit); break;
//		case 1096: display_spells_event_filter (item_hit); break;
//		case 1097: display_skills_event_filter (item_hit); break;
//		case 1098: pick_spell_event_filter (item_hit); break;
//		case 1099: prefs_event_filter (item_hit); break;
//		default: fancy_choice_dialog_event_filter (item_hit); break;
//	}
	
	if (wind_hit < 0)
		return FALSE;
	else return TRUE;
}

void cd_set_bg_pat_num(short n){
	dlg_bg = n;
}

void cd_set_text_clr(RGBColor c){
	clr[0] = c;
}