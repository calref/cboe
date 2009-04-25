//#include <Memory.h>
#include <stdio.h>
#include <string.h>
#include "pc.global.h"
#include "classes.h"
#include "pc.graphics.h"
#include "pc.editors.h"
#include "pc.action.h"
#include "soundtool.h"
#include "graphtool.h"
#include "dlgtool.h"
#include "dlglowlevel.h"
#include "dlgutil.h"

/* Adventure globals */
//extern party_record_type party;
//extern pc_record_type ADVEN[6];
//extern outdoor_record_type outdoors[2][2];
//extern current_town_type c_town;
//extern big_tr_type t_d;
//extern town_item_list	t_i;
//extern unsigned char out[96][96],out_e[96][96];
//extern setup_save_type setup_save;
//extern stored_items_list_type stored_items[3];
//extern stored_town_maps_type maps;
//extern stored_outdoor_maps_type o_maps;
extern cUniverse univ;

extern WindowPtr	mainPtr;
extern Boolean play_sounds,file_in_mem,party_in_scen,scen_items_loaded;

extern short store_flags[3];
extern GWorldPtr button_num_gworld;
extern short current_active_pc;
//extern Boolean ed_reg;

//extern long register_flag,stored_key;
//extern long ed_flag,ed_key;

GWorldPtr title_gworld,pc_gworld,dlogpics_gworld;
GWorldPtr mixed_gworld,buttons_gworld;
//GWorldPtr race_dark,train_dark,items_dark,spells_dark;
//GWorldPtr race_light,train_light,items_light,spells_light;
Rect whole_win_rect = {0,0,440,590};
Rect title_from = {0,0,70,380};
extern Rect pc_area_buttons[6][4] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
extern Rect item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 - 
extern Rect pc_info_rect; // Frame that holds a pc's basic info and items
extern Rect name_rect;
extern Rect pc_race_rect;
extern Rect info_area_rect;
extern Rect hp_sp_rect;
extern Rect skill_rect;
extern Rect pc_skills_rect[19];
extern Rect status_rect; 
extern Rect pc_status_rect[10]; 
extern Rect traits_rect;
extern Rect pc_traits_rect[16]; 
extern Rect edit_rect[5][2]; 

short store_str1a;
short store_str1b;
short store_str2a;
short store_str2b;
short store_which_string_dlog;
short store_page_on,store_num_i;
Rect ed_buttons_from[2] = {{0,0,57,57},{0,57,57,114}};
short current_pressed_button = -1;
Boolean init_once = FALSE;
GWorldPtr spec_scen_g; // not actually needed; just here to silence compiler because it's reference in fileio.h

void init_dialogs(){
	cd_init_dialogs(NULL,NULL,NULL,NULL,NULL,&dlogpics_gworld,NULL,NULL,NULL,NULL,NULL,NULL/*,NULL,NULL,NULL,NULL,NULL,NULL,NULL*/,NULL,NULL);
	cd_register_event_filter(917,edit_day_event_filter);
	cd_register_event_filter(970,display_strings_event_filter);
	cd_register_event_filter(971,display_strings_event_filter);
	cd_register_event_filter(972,display_strings_event_filter);
	cd_register_event_filter(973,display_strings_event_filter);
	cd_register_event_filter(991,display_pc_event_filter);
	cd_register_event_filter(996,display_alchemy_event_filter);
	cd_register_event_filter(1010,spend_xp_event_filter);
	cd_register_event_filter(947,edit_gold_or_food_event_filter);
	cd_register_event_filter(1012,edit_gold_or_food_event_filter);
	cd_register_event_filter(1013,pick_race_abil_event_filter);
	cd_register_event_filter(1018,select_pc_event_filter);
	cd_register_event_filter(1024,edit_xp_event_filter);
	cd_register_event_filter(1073,give_reg_info_event_filter);
	cd_register_default_event_filter(fancy_choice_dialog_event_filter);
}

void init_main_buttons()
{

	short i,j;
	int	indent = 0, indent2 = 0;
	Rect start_rect = {0,0,36,28};
	GrafPtr old_port;
	
	Str255 fn1 = "\pGeneva";
	Str255 fn3 = "\pPalatino";
	Rect mask_rect = {0,0,396,308},r,base_rect;
	
	if (init_once == FALSE) {
		init_once = TRUE;
		GetFNum(fn1,&geneva_font_num);
		if (geneva_font_num == 0)
			GetFNum(fn3,&geneva_font_num);

		GetPort(&old_port);	
		SetPort(GetWindowPort(mainPtr));
		TextFont(geneva_font_num);
		TextFace(bold);
		TextSize(10);
		SetPort(old_port);
		}
	//whole_win_rect = mainPtr->portRect;
	//Initialize pc_info_rect in center
	pc_info_rect= whole_win_rect;
	InsetRect(&pc_info_rect,100,100);
	pc_info_rect.bottom+=52;
	pc_info_rect.top-=25;
	pc_info_rect.right+=5;
	name_rect.left = pc_info_rect.left;
	name_rect.right = pc_info_rect.left + 100;
	name_rect.bottom = pc_info_rect.top  + 15;
	name_rect.top = pc_info_rect.top;
	
	//Initialize pc_area_buttons
	pc_area_buttons[0][0].top=pc_info_rect.top;
	pc_area_buttons[0][0].bottom=pc_area_buttons[0][0].top + 56;
	
	for(i=0; i<6; i++)
	{ 
		pc_area_buttons[i][0].left = 20;
		pc_area_buttons[i][0].right = pc_area_buttons[0][0].left + 56;
		pc_area_buttons[i][2].left = 20;
		pc_area_buttons[i][2].right = pc_area_buttons[i][2].left + 56;
		pc_area_buttons[i][3].left = 20;
		pc_area_buttons[i][3].right = pc_area_buttons[i][3].left + 56;
		pc_area_buttons[i][1].left = 34;
		pc_area_buttons[i][1].right = pc_area_buttons[i][1].left + 28;
		
		pc_area_buttons[i][0].top = pc_area_buttons[0][0].top + 60*i;
		pc_area_buttons[i][0].bottom = pc_area_buttons[0][0].bottom + 60*i;
		
		pc_area_buttons[i][1].top = pc_area_buttons[i][0].top+2;
		pc_area_buttons[i][1].bottom = pc_area_buttons[i][2].top = pc_area_buttons[i][0].bottom - 18;
		pc_area_buttons[i][2].bottom = pc_area_buttons[i][3].top = pc_area_buttons[i][0].bottom - 9;
		pc_area_buttons[i][3].bottom = pc_area_buttons[i][0].bottom;
		OffsetRect(&pc_area_buttons[i][2],0,-1);
		OffsetRect(&pc_area_buttons[i][3],0,-2);
	}	
	//Initialize the edit_rect buttons
	edit_rect[0][0].top = pc_info_rect.top;
	for(i = 0; i < 5; i++) {
		if(i >= 2)
			indent = 5;
		else
			indent = 0;
		if( i == 4)
			indent2 = 1;
		edit_rect[i][0].top = edit_rect[0][0].top + 66*i;
		edit_rect[i][0].bottom = edit_rect[i][0].top + 53;
	 	edit_rect[i][0].left = 510;
		edit_rect[i][0].right = edit_rect[i][0].left + 53;
		edit_rect[i][1].top = edit_rect[i][0].top + 11 + indent; 
		edit_rect[i][1].bottom = edit_rect[i][0].bottom - 11 - indent;
		edit_rect[i][1].right = edit_rect[i][0].right - 8 +indent2;
		edit_rect[i][1].left = edit_rect[i][0].left + 8 + indent2;
	}
	


//	name_rect.left = pc_info_rect.left;
//	name_rect.right = pc_info_rect.left + (pc_info_rect.right - pc_info_rect.left)/4;
//	name_rect.bottom = pc_info_rect.top  + 15;
//	name_rect.top = pc_info_rect.top;

//	if (name_rect.right == name_rect.left) {
//		SysBeep(50); SysBeep(50);
//		}
	//Initialize pc_race_rect
	pc_race_rect.top = pc_info_rect.top;
	pc_race_rect.bottom = name_rect.bottom;
	pc_race_rect.left = name_rect.right;
	pc_race_rect.right = pc_info_rect.left + (pc_info_rect.right - pc_info_rect.left)/2;

	//initialize info_area_rect
	info_area_rect.top = pc_info_rect.top;
	info_area_rect.left = pc_info_rect.left;
	info_area_rect.right = pc_race_rect.right;
	info_area_rect.bottom = pc_info_rect.bottom;
/*	if (name_rect.right == name_rect.left) {
		SysBeep(50);
		} */

	// Initialize hp_sp_rect
	hp_sp_rect.top = name_rect.bottom + 1;
	hp_sp_rect.left = pc_info_rect.left + 1;
	hp_sp_rect.right = pc_race_rect.right;
	hp_sp_rect.bottom = hp_sp_rect.top + 12;
	// Initialize skill_rect
	skill_rect.top = hp_sp_rect.bottom + 2;
	skill_rect.left = pc_info_rect.left + 1;
	skill_rect.right = pc_race_rect.right;
	skill_rect.bottom = skill_rect.top + 12;
	
	//if (name_rect.right == name_rect.left) {
	//	SysBeep(50); SysBeep(50);
	//	}
	//Initialize skills_rect
	base_rect.top = skill_rect.bottom + 1;
	base_rect.left = skill_rect.left + 1;
	base_rect.right = name_rect.right - 1;
	base_rect.bottom = base_rect.top + (pc_info_rect.bottom - skill_rect.bottom)/30;
	
	for (i = 0; i < 19; i++) {
		pc_skills_rect[i] = base_rect;
		OffsetRect(&pc_skills_rect[i], (i / 10) * ((name_rect.right)-(name_rect.left)), (i % 10) * (pc_info_rect.bottom - name_rect.bottom)/30);
	}
	
	//Initialize status_rect
	status_rect.top = pc_skills_rect[9].bottom + 5;
	status_rect.left = pc_info_rect.left + 1;
	status_rect.right = pc_race_rect.right;
	status_rect.bottom = status_rect.top + 12;
	//Initialize pc_status_rect
	base_rect.top = status_rect.bottom + 1;
	base_rect.left = status_rect.left + 1;
	base_rect.right = name_rect.right - 1;
	base_rect.bottom = base_rect.top + (pc_info_rect.bottom - status_rect.bottom)/15;
	for (i = 0; i < 10; i++) {
		pc_status_rect[i] = base_rect;
		OffsetRect(&pc_status_rect[i], (i / 5) * ((name_rect.right)-(name_rect.left)), (i % 5) * (pc_info_rect.bottom - status_rect.bottom)/15);
	}
	//Initialize traits_rect
	traits_rect.top = pc_status_rect[4].bottom + 5;
	traits_rect.left = pc_info_rect.left + 1;
	traits_rect.right = pc_race_rect.right;
	traits_rect.bottom = traits_rect.top + 12;
	//Initialize pc_traits_rect
	base_rect.top = traits_rect.bottom - 1;
	base_rect.left = traits_rect.left + 1;
	base_rect.right = name_rect.right - 1;
	base_rect.bottom = base_rect.top + 10;
	for (i = 0; i < 16; i++) {
		pc_traits_rect[i] = base_rect;
		OffsetRect(&pc_traits_rect[i], (i / 8) * ((name_rect.right)-(name_rect.left)), (i % 8) * 9);
	}


	item_string_rects[0][0].top = pc_info_rect.top + 3;
	item_string_rects[0][0].left = pc_info_rect.left + (pc_info_rect.right - pc_info_rect.left)/2;
	item_string_rects[0][0].right = pc_info_rect.right;
	item_string_rects[0][0].bottom = item_string_rects[0][0].top + 12;
	for (i = 1; i < 24; i++) {
		item_string_rects[i][0] = item_string_rects[0][0];
		OffsetRect(&item_string_rects[i][0],0,13 * i);
	}
	for (i = 0; i < 24; i++) {
		item_string_rects[i][1] = item_string_rects[i][0];
		item_string_rects[i][1].right -= 14;
		item_string_rects[i][1].left = item_string_rects[i][1].right - 14;
		item_string_rects[i][2] = item_string_rects[i][0];
		item_string_rects[i][2].left = item_string_rects[i][2].right - 14;
	}		

}

void Set_up_win ()
{
	short i,j;
	title_gworld = load_pict(5000);	
	pc_gworld = load_pict(902);
	mixed_gworld = load_pict(903);
	dlogpics_gworld = load_pict(850);
	buttons_gworld = load_pict(5001);

	for (i = 0; i < 14; i++){
	    bg[i] = GetPixPat (128 + i);
	}
}


void redraw_screen()
{
	draw_main_screen();
	display_party(6,1);
	draw_items(1);
}

void draw_main_screen()
{
	Rect	source_rect, dest_rec,dest_rect;
	Rect reg_rect;
	Str255 temp_str;
	int i=0;

	SetPort(GetWindowPort(mainPtr));
	FillCRect(&whole_win_rect,bg[12]); // fill whole window with background texture
	dest_rec = source_rect = title_from; // initializes, to draw title 
		 // title_from is a Rect constant
	OffsetRect(&dest_rec,20,0);
	
	rect_draw_some_item (title_gworld,source_rect, title_gworld,dest_rec,  1,1);
		// The first title_gworld is the from, the gworld to draw from
		// and source_rect is the rectangle in that gworld to draw
		// The 1,1 at the end means ...
		// the 1st 1 means transparent, if the first 1 was a 0, not transparent
		// The second 1 means on the main window, not into another gworld
		// dest_rec is where it's drawn to
		// Finally, the second title_gworld. This would be the gworld you were drawing into if
		// you were drawing into a gworld. You aren't so this doesn't matter, so just put the earlier
		// gworld in as a place holder.
	
	dest_rect = dest_rec;
	dest_rect.top = dest_rect.bottom;
	dest_rect.bottom = dest_rect.top + 50;
		// initialize rectangle to draw text into
	TextSize(12);
	TextFace(bold + underline);
		// set the pen
	//char_win_draw_string(GetWindowPort(mainPtr),dest_rect,"Characters",0,10);
		// This draws a chunk of text on the screen
	TextSize(10); /// reset text size
	TextFace(0); // reset text pen
	TextFace(bold);
	
	frame_dlog_rect(GetWindowPort(mainPtr),pc_info_rect,1); // draw the frame
	//i = pc_info_rect.left-pc_info_rect.right;
	//sprintf((char *)temp_str,"Width of pc_info_rect %d ",
				//(short) i);
	//win_draw_string(mainPtr,pc_info_rect,temp_str,0,12);

	
	dest_rect = pc_area_buttons[5][0]; 
	dest_rect.right = whole_win_rect.right - 30; //What is this for? Commenting it out has no effect.
	dest_rect.left += 60;
	//Off0setRect(&dest_rect,0,45);
	OffsetRect(&dest_rect,0,21);
	if (file_in_mem == TRUE)
		char_win_draw_string(mainPtr,dest_rect,"Click on character to edit it.",0,10,true);
	else
		char_win_draw_string(mainPtr,dest_rect,"Select Open from File menu.",0,10,true);
	if(file_in_mem == TRUE && party_in_scen==TRUE && scen_items_loaded==FALSE){
		OffsetRect(&dest_rect,200,0);
		char_win_draw_string(mainPtr,dest_rect,"Warning: Scenario item data could not be loaded.",0,10,true);
		OffsetRect(&dest_rect,-200,0);
	}
	OffsetRect(&dest_rect,0,12);
	if (file_in_mem == TRUE)
		char_win_draw_string(mainPtr,dest_rect,"Press 'I' button to identify item, and 'D' button to drop item.",0,10,true);
	TextSize(12);
	OffsetRect(&dest_rect,0,16);
	if (file_in_mem == TRUE)
		char_win_draw_string(mainPtr,dest_rect,"Back up save file before editing it!",0,10,true);
	TextSize(10);
	TextFace(0);
	OffsetRect(&dest_rect,300,0);
	char_win_draw_string(mainPtr,dest_rect,"Created in 1997 by Spiderweb Software, Inc.",0,10,true);
	TextFace(bold);
	
	
	reg_rect = whole_win_rect;
	reg_rect.left = reg_rect.right - 170;
	reg_rect.top += 8;
	reg_rect.right -= 3;
	
}

void do_button_action(short which_pc,short which_button)
{
	unsigned long dummy;

	current_pressed_button = which_button;
	display_party(6,0);
	play_sound(34);
	Delay(10,&dummy);
	current_pressed_button = -1;
	display_party(6,0);
}

GWorldPtr load_pict(short picture_to_get)
{
	PicHandle	current_pic_handle;
	Rect	pic_rect;
	short	pic_wd,pic_hgt;
	GWorldPtr	myGWorld;
	CGrafPtr	origPort;
	GDHandle	origDev;
	QDErr		check_error;
	PixMapHandle	offPMHandle;
	char good;
	char debug[60];
			
    current_pic_handle = GetPicture (picture_to_get);
	if (current_pic_handle == NIL)  {
		SysBeep(2);SysBeep(50);SysBeep(50);
		ExitToShell();
	}
	QDGetPictureBounds(current_pic_handle, &pic_rect);
	pic_wd = pic_rect.right - pic_rect.left;
	pic_hgt = pic_rect.bottom - pic_rect.top;  
	GetGWorld (&origPort, &origDev);
	check_error = NewGWorld (&myGWorld, 0,
				&pic_rect,
				NULL, NULL, kNativeEndianPixMap);
	if (check_error != noErr)  {
		SysBeep(50); 
//		sprintf((char *) debug, "Stuck on %d          ",(short) picture_to_get);
//		add_string_to_buf((char *)debug);
//		print_buf(); 
//		ExitToShell();
		}
	
	SetGWorld(myGWorld, NULL);
	
	offPMHandle = GetGWorldPixMap (myGWorld);
	good = LockPixels (offPMHandle);
	if (good == FALSE)  {
		SysBeep(50); }
	SetRect (&pic_rect, 0, 0, pic_wd, pic_hgt);
	DrawPicture (current_pic_handle, &pic_rect);
	SetGWorld (origPort, origDev);
	UnlockPixels (offPMHandle);
	ReleaseResource ((Handle) current_pic_handle);

	return myGWorld;
}

//extern Rect pc_area_buttons[6][6] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
//extern Rect item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 - 
void draw_items(short clear_first)
//short clear_first; // 0 - redraw over, 1 - don't redraw over
{
	short i;
	Str255 to_draw;
	Rect d_from = {12,28,24,42},i_from = {12,42,24,56},dest_rect;

	if (file_in_mem == FALSE)  // save file loaded
		return;

	dest_rect = item_string_rects[0][0];
	dest_rect.bottom += 3;
	OffsetRect(&dest_rect,0,-14);

	// First erase crap there already by painting background texture over it
	if (clear_first == 1) {
		for (i = 0; i < 24; i++)
			FillCRect(&item_string_rects[i][0],bg[12]);	
		FillCRect(&dest_rect,bg[12]);	
		}
	
	// First, draw "Fred's Items:"
	//sprintf((char *)to_draw,"%s items:",ADVEN[current_active_pc].name);
	//TextSize(12);
	//ClipRect(&dest_rect);
	//win_draw_string(mainPtr,item_string_rects[0][0],to_draw,0,12);
	//undo_clip();
	//TextSize(10);

	if (ADVEN[current_active_pc].main_status != 1){
		frame_dlog_rect(GetWindowPort(mainPtr),pc_info_rect,1); // re draw entire frame 
		frame_dlog_rect(GetWindowPort(mainPtr),info_area_rect,1); // draw the frame
		frame_dlog_rect(GetWindowPort(mainPtr),pc_race_rect,1); // draw the frame	
		return; // If PC is dead, it has no items
	}
	for (i = 0; i < 24; i++) // Loop through items and draw each
		if (ADVEN[current_active_pc].items[i].variety > 0) { // i.e. does item exist
			sprintf((char *) to_draw, "");
			if (ADVEN[current_active_pc].items[i].item_properties & 1 == 0)
				sprintf((char *) to_draw, "%d. %s  ",i + 1,ADVEN[current_active_pc].items[i].name);
				else if (ADVEN[current_active_pc].items[i].charges > 0)
					sprintf((char *) to_draw, "%d. %s (%d)",i + 1,ADVEN[current_active_pc].items[i].full_name,
					ADVEN[current_active_pc].items[i].charges);
				else sprintf((char *) to_draw, "%d. %s ",i + 1,ADVEN[current_active_pc].items[i].full_name);			

			//if (i % 2 == 0)
			//	sprintf((char *) to_draw, "%d %d %d %d",
			//	pc_info_rect.left,pc_info_rect.right,pc_info_rect.top,pc_info_rect.bottom);
			//	else sprintf((char *) to_draw, "%d %d %d %d",
			//	name_rect.left,name_rect.right,name_rect.top,name_rect.bottom);

			char_win_draw_string(mainPtr,item_string_rects[i][0],(char *) to_draw,0,10,true);

			//Draw id/drop buttons
			rect_draw_some_item(mixed_gworld,d_from,mixed_gworld,item_string_rects[i][1],1,1);
			rect_draw_some_item(mixed_gworld,i_from,mixed_gworld,item_string_rects[i][2],1,1);
			}
	frame_dlog_rect(GetWindowPort(mainPtr),pc_info_rect,1); // re draw entire frame 
	frame_dlog_rect(GetWindowPort(mainPtr),name_rect,1); // draw the frame
	frame_dlog_rect(GetWindowPort(mainPtr),pc_race_rect,1); // draw the frame
	frame_dlog_rect(GetWindowPort(mainPtr),info_area_rect,1); // draw the frame

}

void display_party(short mode,short clear_first)
//short mode; // 0 - 5 this pc, 6 - all
//short clear_first; // 1 - redraw over what's already there, 0 - don't redraw over
{
	short i,j,k,string_num, cur_rect=0, flag;
	Str255 to_draw, skill_value;
	short start_v = 115, start_h = 80;	
	Rect to_rect,from_base = {0,0,36,28},from_rect,no_party_rect,temp_rect;
	
	// lots of stuff is global. Like ...
	// Boolean file_in_mem
	// short current_active_pc
	// pc_record_type ADVEN[6]
	if (clear_first == 1) { // first erase what's already there
		for (i = 0; i < 6; i++)
			FillCRect(&pc_area_buttons[i][0],bg[12]);
		FillCRect(&name_rect,bg[12]);
		FillCRect(&pc_race_rect,bg[12]);
		FillCRect(&info_area_rect,bg[12]);
		frame_dlog_rect(GetWindowPort(mainPtr),pc_info_rect,1); // re-draw the frame
	}
	
	if (file_in_mem == FALSE) { // what if no party loaded?
		no_party_rect=pc_info_rect;
		no_party_rect.top+=5;
		no_party_rect.left+=5;
		char_win_draw_string(mainPtr,no_party_rect,"No party loaded.",0,10,true);
	}
	else {
		from_rect = pc_info_rect;
		from_rect.top = from_rect.bottom - 14;
		if (party_in_scen == FALSE)
			char_win_draw_string(mainPtr,from_rect,"Party not in a scenario.",0,10,true);
		else
			char_win_draw_string(mainPtr,from_rect,"Party is in a scenario.",0,10,true);
		for (i = 0; i < 6; i++) {
			if (i == current_active_pc) // active pc is drawn in blue
				ForeColor(blueColor);
			else ForeColor(blackColor);
			
			from_rect = (current_pressed_button == i) ? ed_buttons_from[1] : ed_buttons_from[0];
			
			if ((current_pressed_button < 0) || (current_pressed_button == i))
				rect_draw_some_item(buttons_gworld,from_rect,buttons_gworld,pc_area_buttons[i][0],0,1);
			ForeColor(blackColor);
			
			// pc_record_type ADVEN[6] is the records that contains chaarcters
			// main_status determins 0 - not exist, 1 - alive, OK, 2 - dead, 3 - stoned, 4 - dust
			if (ADVEN[i].main_status != 0) { // PC exists?
				from_rect = from_base;
				// draw PC graphic
				OffsetRect(&from_rect,56 * (ADVEN[i].which_graphic / 8),36 * (ADVEN[i].which_graphic % 8));
				rect_draw_some_item(pc_gworld,from_rect,pc_gworld,pc_area_buttons[i][1],1,1);
				
				//frame_dlog_rect(GetWindowPort(mainPtr),pc_area_buttons[i][1],0); 
				// draw name
				TextSize(9);
				if( (strlen(ADVEN[i].name)) >= 10) {
					TextFace(0);
					sprintf((char *) to_draw, "%-s  ", (char *) ADVEN[i].name);	
					TextSize(6);
				}
				else {
					sprintf((char *) to_draw, "%-s ", (char *) ADVEN[i].name);	
				}
				
				ForeColor(whiteColor);
				win_draw_string(GetWindowPort(mainPtr),pc_area_buttons[i][2],to_draw,1,10,true);
				TextFace(bold);
				TextSize(10);
				
				if (i == current_active_pc){
					sprintf((char *) to_draw, "%-.18s  ", (char *) ADVEN[i].name);
					if( (strlen(ADVEN[i].name)) > 12)
						TextSize(8);
					ForeColor(blackColor);
					win_draw_string(GetWindowPort(mainPtr),name_rect,to_draw,1,10,true);
					TextSize(10);
				}
				if ((current_pressed_button < 0) || (current_pressed_button == i))
					switch (ADVEN[i].main_status) {
							// draw statistics
						case 1:
							if (i == current_active_pc) {
								//Draw in race
								if (ADVEN[i].race == 0)
									char_win_draw_string(mainPtr,pc_race_rect,"Human   ",1,10,true);
								if (ADVEN[i].race == 1)
									char_win_draw_string(mainPtr,pc_race_rect,"Nephilim   ",1,10,true);
								if (ADVEN[i].race == 2)
									char_win_draw_string(mainPtr,pc_race_rect,"Slithzerikai  ",1,10,true);
								// Draw in skills	
								
								sprintf((char *) to_draw, "Skills:");
								win_draw_string(GetWindowPort(mainPtr),skill_rect,to_draw,0,10,true);
								sprintf((char *) to_draw, "Hp: %d/%d  Sp: %d/%d",ADVEN[i].cur_health,ADVEN[i].max_health,ADVEN[i].cur_sp,
										ADVEN[i].max_sp);
								win_draw_string(GetWindowPort(mainPtr),hp_sp_rect,to_draw,0,10,true);
								
								
								TextSize(9);
								TextFace(0);
								string_num=1;
								for( k = 0; k < 19 ; ++k)
								{
									temp_rect = pc_skills_rect[k];
									temp_rect.left = pc_skills_rect[k].left + 80;
									
									get_str(to_draw,9,string_num);
									win_draw_string(GetWindowPort(mainPtr),pc_skills_rect[k],to_draw,0,9,true);
									
									sprintf((char *) skill_value,"%d",ADVEN[i].skills[k]);
									win_draw_string(GetWindowPort(mainPtr),temp_rect,skill_value,0,9,true);	
									//frame_dlog_rect(GetWindowPort(mainPtr),pc_skills_rect[k],0);
									string_num+=2;
								}
								//end skills
								
								//Write in pc Status
								TextSize(10);
								TextFace(bold);	
								sprintf((char *) to_draw, "Status:");
								win_draw_string(GetWindowPort(mainPtr),status_rect,to_draw,0,10,true);
								
								TextSize(9);
								TextFace(0);
								//for(k = 0 ; k < 10; k++)
								//frame_dlog_rect(GetWindowPort(mainPtr),pc_status_rect[k],0);
								if (ADVEN[i].status[0] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Poisoned Weap.",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].status[1] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Blessed",0,9,true);
										cur_rect++;
									}
									else if(ADVEN[i].status[1] < 0)
										if(cur_rect <= 9) {
											char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Cursed",0,9,true);
											cur_rect++;
										}
								if (ADVEN[i].status[2] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Poisoned",0,9,true);
										cur_rect++;
									}	
								if (ADVEN[i].status[3] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Hasted",0,9,true);
										cur_rect++;
									}
									else if(ADVEN[i].status[3] < 0)
										if(cur_rect <= 9) {
											char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Slowed",0,9,true);
											cur_rect++;
										}
								if (ADVEN[i].status[4] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Invulnerable",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].status[5] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Magic Resistant",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].status[6] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Webbed",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].status[7] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Diseased",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].status[8] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Sanctury",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].status[9] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Dumbfounded",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].status[10] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Martyr's Shield",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].status[11] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Asleep",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].status[12] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Paralyzed",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].status[13] > 0) 
									if(cur_rect <= 9) {
										char_win_draw_string(mainPtr,pc_status_rect[cur_rect],"Acid",0,9,true);
										cur_rect++;
									}
								//end pc status section
								
								//Write in Traits
								TextSize(10);
								TextFace(bold);	
								sprintf((char *) to_draw, "Traits:");
								win_draw_string(GetWindowPort(mainPtr),traits_rect,to_draw,0,10,true);
								//for(k = 0 ; k < 16; k++)
								//frame_dlog_rect(GetWindowPort(mainPtr),pc_traits_rect[k],0);
								TextSize(9);
								TextFace(0);	
								cur_rect=0;
								if (ADVEN[i].traits[0] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Toughness",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].traits[1] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Magically Apt",0,9,true);
										cur_rect++;
									}		
								if (ADVEN[i].traits[2] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Ambidextrous",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].traits[3] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Nimble Fingers",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].traits[4] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Cave Lore",0,9,true);
										cur_rect++;
									}
								
								if (ADVEN[i].traits[5] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Woodsman",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].traits[6] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Good Constitution",0,9,true);
										cur_rect++;
									}		
								if (ADVEN[i].traits[7] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Highly Alert",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].traits[8] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Exceptional Str.",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].traits[9] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Recuperation",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].traits[10] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Sluggish",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].traits[11] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Magically Inept",0,9,true);
										cur_rect++;
									}		
								if (ADVEN[i].traits[12] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Frail",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].traits[13] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Chronic Disease",0,9,true);
										cur_rect++;
									}
								if (ADVEN[i].traits[14] == 1) 
									if(cur_rect <= 15) {
										char_win_draw_string(mainPtr,pc_traits_rect[cur_rect],"Bad Back",0,9,true);
										cur_rect++;
									}
								
								//end traits
							}
							
							ForeColor(whiteColor);
							TextSize(9);
							TextFace(0);
							char_win_draw_string(mainPtr,pc_area_buttons[i][3],"Alive ",1,10,true);
							TextFace(bold);
							TextSize(10);
							break;
							case 2:
							ForeColor(whiteColor);
							TextSize(9);
							TextFace(0);
							char_win_draw_string(mainPtr,pc_area_buttons[i][3],"Dead ",1,10,true);
							TextFace(bold);
							TextSize(10);
							break;
							case 3:
							ForeColor(whiteColor);
							TextSize(9);
							TextFace(0);
							char_win_draw_string(mainPtr,pc_area_buttons[i][3],"Dust ",1,10,true);
							TextFace(bold);
							TextSize(10);
							break;
							case 4:
							ForeColor(whiteColor);
							TextSize(9);
							TextFace(0);
							char_win_draw_string(mainPtr,pc_area_buttons[i][3],"Stone ",1,10,true);
							TextFace(bold);
							TextSize(10);
							break;
							case 5:
							ForeColor(whiteColor);
							TextSize(9);
							TextFace(0);
							char_win_draw_string(mainPtr,pc_area_buttons[i][3],"Fled ",1,10,true);
							TextFace(bold);
							TextSize(10);
							break;
							case 6:
							ForeColor(whiteColor);
							TextSize(9);
							TextFace(0); 
							char_win_draw_string(mainPtr,pc_area_buttons[i][3],"Surface ",1,10,true);
							TextFace(bold);
							TextSize(10);
							break;
							default:
							ForeColor(whiteColor);
							TextFace(0);
							TextSize(9);
							char_win_draw_string(mainPtr,pc_area_buttons[i][3],"Absent ",1,10,true);
							TextFace(bold);
							TextSize(10);
							break;
					}
				//frame_dlog_rect(GetWindowPort(mainPtr),pc_area_buttons[i][0],0); 
				
				
			}
			
		} // Closes the for i=6 loop
		ForeColor(blackColor);			
		
		for(i = 0; i < 5; i++) 
			if ((current_pressed_button < 0) || (current_pressed_button == i + 10)) {	
				if (clear_first == 1) { // first erase what's already there
					FillCRect(&edit_rect[i][0],bg[12]);
				}		
				//frame_dlog_rect(GetWindowPort(mainPtr),edit_rect[i][0],0);
				//frame_dlog_rect(GetWindowPort(mainPtr),edit_rect[i][1],0);
				from_rect = (current_pressed_button == i + 10) ? ed_buttons_from[1] : ed_buttons_from[0];
				rect_draw_some_item(buttons_gworld,from_rect,buttons_gworld,edit_rect[i][0],0,1);
				ForeColor(whiteColor);			
				switch(i) {
					case 0:
						char_win_draw_string(mainPtr,edit_rect[0][1],"  Add  Mage Spells ",0,10,true);
						break;
					case 1:
						char_win_draw_string(mainPtr,edit_rect[1][1],"  Add Priest Spells ",0,10,true);
						break;
					case 2: 
						char_win_draw_string(mainPtr,edit_rect[2][1]," Edit  Traits",0,10,true);
						break;
					case 3:
						char_win_draw_string(mainPtr,edit_rect[3][1]," Edit  Skills",0,10,true);
						break;
					case 4:
						char_win_draw_string(mainPtr,edit_rect[4][1]," Edit   XP",0,10,true);
						break;
					default:
						break;	
				}		
				ForeColor(blackColor);			
				
			}
		//			MoveTo(start_h + 10, start_v + 127);	
		//			sprintf((char *) to_draw, " Gold: %d       Food: %d ",(short) party.gold, (short) party.food);
		//			DrawString(to_draw);
	}
	
	
	ForeColor(blackColor);
}

void undo_clip()
{
	ClipRect(&whole_win_rect);
}

void add_string_to_buf(char *str) {

	}
	
//void rect_draw_some_item (GWorldPtr	src_gworld, Rect	src_rect, GWorldPtr	targ_gworld,Rect targ_rect, 
//char masked,short main_win)
////char	 masked; // if 10 - make AddOver
////short   main_win; // if 2, drawing onto dialog
//{
//	Rect	destrec;
//	PixMapHandle	test1, test2;
//	const BitMap *store_dest;
//	GrafPtr cur_port;
//	RGBColor	store_color;
//
//	if (main_win == 2) {
//		GetBackColor(&store_color);
//		BackColor(whiteColor);
//		}
//		
//	GetPort(&cur_port);	
//	store_dest = GetPortBitMapForCopyBits(cur_port);
//	
//	test1 = GetPortPixMap(src_gworld);
//
//	LockPixels(test1);
//	if (main_win == 0) {
//		test2 = GetPortPixMap(targ_gworld);
//		LockPixels(test2);
//		if (masked == 1) 
//			CopyBits ( (BitMap *) *test1 ,
//					(BitMap *) *test2 ,
//					&src_rect, &targ_rect, 
//					 transparent , NULL);	
//			else CopyBits ( (BitMap *) *test1 ,
//					(BitMap *) *test2 ,
//					&src_rect, &targ_rect, 
//					  (masked == 10) ? addOver : 0, NULL);
//		UnlockPixels(test2);
//		}  
//		else {
//		if (masked == 1) 
//			CopyBits ( (BitMap *) *test1 ,
//					store_dest ,
//					&src_rect, &targ_rect, 
//					 transparent , NULL);
//			else CopyBits ( (BitMap *) *test1 ,
//					store_dest ,
//					&src_rect, &targ_rect, 
//					  (masked == 10) ? addOver : 0, NULL);   
//			}
//	UnlockPixels(test1);
//
//	if (main_win == 2) 
//		RGBBackColor(&store_color);
//}

//short string_length(char *str)
//{
//	short text_len[257];
//	short total_width = 0,i,len;
//	Str255 p_str;
//	
//	for (i = 0; i < 257; i++)
//		text_len[i]= 0;
//	
//	strcpy((char *) p_str,str);
//	c2pstr((char*) p_str);
//	MeasureText(256,p_str,text_len);
//	len = strlen((char *)str);
//	
//	for (i = 0; i < 257; i++)
//		if ((text_len[i] > total_width) && (i <= len))
//			total_width = text_len[i];
//	return total_width;
//}

//void char_win_draw_string(GrafPtr dest_window,Rect dest_rect,char *str,short mode,short line_height)
//{
//	Str255 store_s;
//	strcpy((char *) store_s,str);
//	win_draw_string( dest_window, dest_rect,store_s, mode, line_height);
//}
//
//// mode: 0 - align up and left, 1 - center on one line
//// str is a c string, 256 characters
//// uses current font
//void win_draw_string(GrafPtr dest_window,Rect dest_rect,Str255 str,short mode,short line_height)
//{
//	GrafPtr old_port;
//	Str255 p_str,str_to_draw,str_to_draw2,c_str;
//	Str255 null_s = "                                                                                                                                                                                                                                                              ";
//	short str_len,i;
//	short last_line_break = 0,last_word_break = 0,on_what_line = 0;
//	short text_len[257];
//	short total_width = 0;
//	Boolean end_loop,force_skip = FALSE;
//	KeyMap key_state;
//	long dummy3;
//	RgnHandle current_clip;
//	short adjust_x = 0,adjust_y = 0;
//	strcpy((char *) p_str,(char *) str);
//	strcpy((char *) c_str,(char *) str);
//	c2pstr((char*) p_str);
//	for (i = 0; i < 257; i++)
//		text_len[i]= 0;
//	MeasureText(256,p_str,text_len);
//	str_len = (short) strlen((char *)str);
//	if (str_len == 0) {
//		return;
//	}
//	GetPort(&old_port);	
//	SetPort(dest_window);
//	
//	//FrameRect(&dest_rect);
//	
//	current_clip = NewRgn();
//	GetClip(current_clip);
//	
//	dest_rect.bottom += 5;
//	//ClipRect(&dest_rect);
//	dest_rect.bottom -= 5;
//	
//	for (i = 0; i < 257; i++)
//		if ((text_len[i] > total_width) && (i <= str_len))
//			total_width = text_len[i];
//	if ((mode == 0) && (total_width < dest_rect.right - dest_rect.left))
//		mode = 2;
//	for (i = 0; i < 257; i++)
//		if ((i <= str_len) && (c_str[i] == '|') && (mode == 2))
//			mode = 0;
//	
//	switch (mode) {
//		case 0: 
//			MoveTo(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + line_height * on_what_line + adjust_y + 9);
//			for (i = 0;text_len[i] != text_len[i + 1], i < str_len;i++) {
//				if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6))  && (last_word_break > last_line_break)) || (c_str[i] == '|')) {
//				  	if (c_str[i] == '|') {
//				  		c_str[i] = ' ';
//				  		force_skip = TRUE;
//					}
//					sprintf((char *)str_to_draw,"%s",(char *)null_s);
//					strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (last_word_break - last_line_break - 1));
//					sprintf((char *)str_to_draw2," %s",str_to_draw);
//					str_to_draw2[0] = (char) strlen((char *)str_to_draw);
//					DrawString(str_to_draw2);
//					on_what_line++;
//					MoveTo(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + line_height * on_what_line + adjust_y + 9);
//					last_line_break = last_word_break;
//					if (force_skip == TRUE) {
//						force_skip = FALSE;
//						i++;
//						last_line_break++;
//						last_word_break++;
//					}
//				}
//				if (c_str[i] == ' ')
//					last_word_break = i + 1;
//				//if (on_what_line == LINES_IN_TEXT_WIN - 1)
//				//	i = 10000;
//			}
//			if (i - last_line_break > 1) {
//				strcpy((char *)str_to_draw,(char *)null_s);
//				strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (i - last_line_break));
//				sprintf((char *)str_to_draw2," %s",str_to_draw);
//				if (strlen((char *) str_to_draw2) > 3) {
//					str_to_draw2[0] = (char) strlen((char *)str_to_draw);
//					DrawString(str_to_draw2);
//				}
//			}
//			break;
//		case 1:
//			MoveTo((dest_rect.right + dest_rect.left) / 2 - (4 * total_width) / 9 + adjust_x, 
//			  (dest_rect.bottom + dest_rect.top - line_height) / 2 + 9 + adjust_y);	
//			DrawString(p_str);
//			break;
//		case 2:
//			MoveTo(dest_rect.left + 1 + adjust_x, 
//			  dest_rect.top + 1 + adjust_y + 9);
//			DrawString(p_str);					
//			break;
//		case 3:
//			MoveTo(dest_rect.left + 1 + adjust_x, 
//			  dest_rect.top + 1 + adjust_y + 9 + (dest_rect.bottom - dest_rect.top) / 6);
//			DrawString(p_str);					
//			break;
//		}
//	SetClip(current_clip);
//	DisposeRgn(current_clip);
//	SetPort(old_port);
//}

//void display_strings_event_filter (short item_hit)
//{
//	short i;
//	Boolean had1 = FALSE, had2 = FALSE;
//	
//	switch (item_hit) {
//		case 1:
//			toast_dialog();
//			break;
//		}
//}

void record_display_strings(){}

//void display_strings(short str1a,short str1b,short str2a,short str2b,
//	char *title,short sound_num,short graphic_num,short graphic_type,short parent_num)
//{
//	short item_hit;
//	Str255 text;
//	location view_loc;
//	Boolean sound_done = FALSE;
//
//	make_cursor_sword();
//	
//	store_str1a = str1a;
//	store_str1b = str1b;
//	store_str2a = str2a;
//	store_str2b = str2b;
//	
//	if ((str1a <= 0) || (str1b <= 0))
//		return;
//	store_which_string_dlog = 970;
//	if (strlen(title) > 0)
//		store_which_string_dlog += 2;
//	if ((str2a > 0) && (str2b > 0))
//		store_which_string_dlog++;
//	cd_create_dialog_parent_num(store_which_string_dlog,parent_num);
//	
//	cd_activate_item(store_which_string_dlog,2,0);
//	
//	csp(store_which_string_dlog,store_which_string_dlog,graphic_num,graphic_type);
//	
//	get_str(text,str1a,str1b);
//	csit(store_which_string_dlog,4,(char *) text);
//	if ((str2a > 0) && (str2b > 0)) {
//		get_str(text,str2a,str2b);
//		csit(store_which_string_dlog,5,(char *) text);
//		}
//	if (strlen(title) > 0)
//		csit(store_which_string_dlog,6,title);
//	csp(store_which_string_dlog,3,graphic_num,graphic_type);
//	if (sound_num >= 0)
//		play_sound(sound_num);
//	
//	item_hit = cd_run_dialog();
//	cd_kill_dialog(store_which_string_dlog,0);
//}

//void get_str(Str255 str,short i, short j)
//{
//	GetIndString(str, i, j);
//	p2cstr(str);
//}

void make_cursor_sword() 
{
	set_cursor(sword_curs);
}

