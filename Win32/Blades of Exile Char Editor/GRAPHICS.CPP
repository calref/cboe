#include <Windows.h>

#include "stdio.h"
#include "string.h"
#include "global.h"
#include "graphics.h"
#include "edsound.h"
#include "dlogtool.h"
#include "graphutl.h"


extern short store_size;

/* Adventure globals */
extern party_record_type far party;
extern pc_record_type far adven[6];
extern outdoor_record_type far outdoors[2][2];
extern current_town_type far c_town;
extern big_tr_type far t_d;
extern town_item_list far	t_i;
extern unsigned char far out[96][96];
extern unsigned char far out_e[96][96];
extern setup_save_type far setup_save;
extern stored_items_list_type far stored_items[3];
extern stored_town_maps_type far maps;
extern stored_outdoor_maps_type far o_maps;

extern HWND	mainPtr;
extern Boolean play_sounds,file_in_mem;

extern short store_flags[3];
extern HBITMAP button_num_gworld;
extern short current_active_pc,ulx,uly;
extern Boolean dialog_not_toast,party_in_scen;

extern HFONT font,italic_font,underline_font,bold_font,tiny_font,small_bold_font;
extern HCURSOR arrow_curs[3][3], sword_curs, boot_curs, key_curs, target_curs,talk_curs,look_curs;
//extern HPALETTE hpal;
extern HDC main_dc,main_dc2,main_dc3;
HBITMAP title_gworld,pc_gworld,dlogpics_gworld,buttons_gworld;
HBITMAP dlg_buttons_gworld,mixed_gworld,dialog_pattern_gworld,pattern_gworld;
RECT whole_win_rect;
	RECT title_from = {41,0,380,70};
extern RECT pc_area_buttons[6][4] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
extern RECT item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 - 
extern RECT pc_info_rect; // Frame that holds a pc's basic info and items
extern RECT name_rect;
extern RECT pc_race_rect;
extern RECT info_area_rect;
extern RECT hp_sp_rect;
extern RECT skill_rect;
extern RECT pc_skills_rect[19];
extern RECT status_rect; 
extern RECT pc_status_rect[10]; 
extern RECT traits_rect;
extern RECT pc_traits_rect[16]; 
extern RECT edit_rect[5][2]; 
extern HINSTANCE store_hInstance;

short store_str1a;
short store_str1b;
short store_str2a;
short store_str2b;
short store_which_string_dlog;
short store_page_on,store_num_i;
	RECT frame_rect = {0,0,605,440};
RECT ed_buttons_from[2] = {{0,0,57,57},{57,0,114,57}};
short current_pressed_button = -1;

void init_main_buttons()
{

	short i;

	short indent = 0, indent2 = 0;
	RECT base_rect;
	RECT active_area_rect = {0,0,590,440};


	GetClientRect(mainPtr,&whole_win_rect);
	ulx = (whole_win_rect.right - 600) / 2;
	uly = (whole_win_rect.bottom - 440) / 2;

	pc_info_rect= active_area_rect;
	InflateRect(&pc_info_rect,-100,-100);
	pc_info_rect.bottom+=52;
	pc_info_rect.top-=25;
	pc_info_rect.right+=5;
	name_rect.left = pc_info_rect.left;
	name_rect.right = pc_info_rect.left + 100;
	name_rect.bottom = pc_info_rect.top  + 15;
	name_rect.top = pc_info_rect.top;

	//Initialize pc_area_buttons
	pc_area_buttons[0][0].top=pc_info_rect.top;
	pc_area_buttons[0][0].bottom=pc_area_buttons[0][0].top + 57 ;

	for(i = 0; i < 6; i++)
	{
		pc_area_buttons[i][0].left = 20;
		pc_area_buttons[i][0].right = pc_area_buttons[0][0].left + 57;
		pc_area_buttons[i][2].left = 20;
		pc_area_buttons[i][2].right = pc_area_buttons[i][2].left + 56;
		pc_area_buttons[i][3].left = 20;
		pc_area_buttons[i][3].right = pc_area_buttons[i][3].left + 56;
		pc_area_buttons[i][1].left = 34;
		pc_area_buttons[i][1].right = pc_area_buttons[i][1].left + 28;

		pc_area_buttons[i][0].top = pc_area_buttons[0][0].top + 60*i;
		pc_area_buttons[i][0].bottom = pc_area_buttons[0][0].bottom + 60*i;

		pc_area_buttons[i][1].top = pc_area_buttons[i][0].top+3;
		pc_area_buttons[i][1].bottom = pc_area_buttons[i][2].top = pc_area_buttons[i][0].bottom - 18;
		pc_area_buttons[i][2].bottom = pc_area_buttons[i][3].top = pc_area_buttons[i][0].bottom - 9;
		pc_area_buttons[i][3].bottom = pc_area_buttons[i][0].bottom;
		OffsetRect(&pc_area_buttons[i][2],0,4);
		OffsetRect(&pc_area_buttons[i][3],0,-2);
	}

	//Initialize the edit_rect buttons
	edit_rect[0][0].top = pc_info_rect.top;
	for(i = 0; i < 5; i++) {
		if(i >= 2)
			indent = 7;
		else
			indent = 0;
		if( i == 4)
			indent2 = 1;
		edit_rect[i][0].top = edit_rect[0][0].top + 66*i;
		edit_rect[i][0].bottom = edit_rect[i][0].top + 53;
		edit_rect[i][0].left = 510;
		edit_rect[i][0].right = edit_rect[i][0].left + 53;
		edit_rect[i][1].top = edit_rect[i][0].top + 7 + indent;
		edit_rect[i][1].bottom = edit_rect[i][0].bottom - 11 - indent;
		edit_rect[i][1].right = edit_rect[i][0].right - 8 +indent2;
		edit_rect[i][1].left = edit_rect[i][0].left + 10 + indent2;
	}

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

	// Initialize hp_sp_rect
	hp_sp_rect.top = name_rect.bottom + 1;
	hp_sp_rect.left = pc_info_rect.left + 1;
	hp_sp_rect.right = pc_race_rect.right - 30;
	hp_sp_rect.bottom = hp_sp_rect.top + 12;
	for (i = 0; i < 6; i++) {
		pc_area_buttons[i][3].left =  hp_sp_rect.right - 32;
		pc_area_buttons[i][3].right = pc_area_buttons[i][3].left + 77;
		pc_area_buttons[i][3].top = hp_sp_rect.top + 1;
		pc_area_buttons[i][3].bottom = hp_sp_rect.bottom + 1;
		}

	// Initialize skill_rect
	skill_rect.top = hp_sp_rect.bottom + 2;
	skill_rect.left = pc_info_rect.left + 1;
	skill_rect.right = pc_race_rect.right;
	skill_rect.bottom = skill_rect.top + 12;

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
		OffsetRect(&pc_traits_rect[i], (i / 7) * ((name_rect.right)-(name_rect.left)), (i % 7) * 10);
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
	for (i = 0; i < 24; i++) {
			item_string_rects[i][0].left += 2;
			item_string_rects[i][0].bottom++;
			}

	name_rect.left -= 2;
	pc_info_rect.left -= 2;
	info_area_rect.left -= 2;

}

void Set_up_win ()
{


	main_dc = GetDC(mainPtr);

	dlg_buttons_gworld = load_pict(2000,main_dc);
	title_gworld = load_pict(5000,main_dc);
		//SelectPalette(main_dc,hpal,0);
		SelectObject(main_dc,font);
		SetBkMode(main_dc,TRANSPARENT);
		main_dc2 = CreateCompatibleDC(main_dc);
		SetMapMode(main_dc2,GetMapMode((HDC) mainPtr));
		//SelectPalette(main_dc2,hpal,0);
		main_dc3 = CreateCompatibleDC(main_dc);
		SetMapMode(main_dc3,GetMapMode((HDC) mainPtr));
		//SelectPalette(main_dc3,hpal,0);

		mixed_gworld = load_pict(903,main_dc);

	pc_gworld = load_pict(902,main_dc);
	dlogpics_gworld = load_pict(850,main_dc);

	buttons_gworld = load_pict(5001,main_dc);
	pattern_gworld = CreateCompatibleBitmap(main_dc,192,256);
		dialog_pattern_gworld = CreateCompatibleBitmap(main_dc,192,256);
}

void lose_graphics()
{
	DeleteObject(title_gworld);
	DeleteObject(pc_gworld);
	DeleteObject(mixed_gworld);
	DeleteObject(dlogpics_gworld);
	DeleteObject(dlg_buttons_gworld);
	DeleteObject(pattern_gworld);
	DeleteObject(dialog_pattern_gworld);
	//DeleteObject(hpal);
	DeleteDC(main_dc);
	DeleteDC(main_dc2);
	DeleteDC(main_dc3);
	DeleteObject(font);
	DeleteObject(underline_font);
	DeleteObject(italic_font);
	DeleteObject(bold_font);

	DeleteObject(sword_curs);
}

void redraw_screen()
{
	draw_main_screen();
	display_party(6,1);
	draw_items(1);
}

void draw_main_screen()
{
	RECT	source_rect, dest_rec,dest_rect;
	RECT reg_rect;
	char temp_str[256];
	RECT top_pic_from = {3,16,41,54};


	paint_pattern(NULL,3,whole_win_rect,3);

	dest_rec = source_rect = title_from;
	OffsetRect(&dest_rec,20,6);

	rect_draw_some_item (title_gworld,source_rect, title_gworld,dest_rec,  1,1);
	dest_rec = top_pic_from;
	OffsetRect(&dest_rec,20,6);
	rect_draw_some_item (title_gworld,top_pic_from, title_gworld,dest_rec,  0,1);

	dest_rect = dest_rec;
	dest_rect.top = dest_rect.bottom;
	dest_rect.bottom = dest_rect.top + 50;
	dest_rect.right += 40;
	dest_rect.left -=5;
	SelectObject(main_dc,bold_font);
	char_win_draw_string(main_dc,dest_rect,"Your party:",0,10);
	
	dest_rec = whole_win_rect;
	InflateRect(&dest_rec,-10,-10);

	if ((ulx >= 5) && (uly >= 5)  &&
		(whole_win_rect.right - 30 > frame_rect.right))
			frame_dlog_rect(mainPtr,frame_rect,0);

	frame_dlog_rect(mainPtr,pc_info_rect,0); // draw the frame

	dest_rect = pc_area_buttons[5][0];
	dest_rect.right = whole_win_rect.right - 30; //What is this for? Commenting it out has no effect.
	dest_rect.left += 60;
	OffsetRect(&dest_rect,0,21);
	if (file_in_mem == TRUE)
		char_win_draw_string(main_dc,dest_rect,"Click on character to edit it.",0,10);
		else char_win_draw_string(main_dc,dest_rect,"Select Open from File menu.",0,10);
	OffsetRect(&dest_rect,0,12);
	if (file_in_mem == TRUE)
		char_win_draw_string(main_dc,dest_rect,"Press 'I' button to identify item, and 'D' button to drop item.",0,10);
	OffsetRect(&dest_rect,0,16);
	if (file_in_mem == TRUE)
		char_win_draw_string(main_dc,dest_rect,"Back up save file before editing it!",0,10);
	OffsetRect(&dest_rect,275,0);
	char_win_draw_string(main_dc,dest_rect,"Released under the GNU GPL, Version 2",0,10);

	reg_rect.right = pc_info_rect.right + 100;
	reg_rect.left = reg_rect.right - 170;
	reg_rect.top = pc_info_rect.top - 70;
	reg_rect.bottom = pc_info_rect.top;
	
}

void do_button_action(short,short which_button)
{
	long dummy;

	current_pressed_button = which_button;
	display_party(6,0);
	play_sound(34);
	Delay(10,&dummy);
	current_pressed_button = -1;
	display_party(6,0);
}


//extern Rect pc_area_buttons[6][6] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
//extern Rect item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 - 
void draw_items(short clear_first)
//short clear_first; // 0 - redraw over, 1 - don't redraw over
{
	short i;
	char to_draw[256];
	RECT d_from = {28,12,42,24},i_from = {42,12,56,24},dest_rect;

	if (file_in_mem == FALSE)
		return;

	dest_rect = item_string_rects[0][0];
	dest_rect.bottom += 3;
	OffsetRect(&dest_rect,0,-14);

	if (clear_first == 1) {
		for (i = 0; i < 24; i++)
			paint_pattern(NULL,1,item_string_rects[i][0],3);
		paint_pattern(NULL,1,dest_rect,3);
		}
	
	// frame_dlog_rect(mainPtr,frame_rect,0);
	if (adven[current_active_pc].main_status != 1){
		frame_dlog_rect(mainPtr,pc_info_rect,0); // re draw entire frame 
		frame_dlog_rect(mainPtr,info_area_rect,0); // draw the frame
		frame_dlog_rect(mainPtr,pc_race_rect,0); // draw the frame
		return; // If PC is dead, it has no items
	}
	for (i = 0; i < 24; i++) // Loop through items and draw each
		if (adven[current_active_pc].items[i].variety > 0) { // i.e. does item exist
			sprintf((char *) to_draw, "");
			if (adven[current_active_pc].items[i].item_properties % 2 == 0)
				sprintf((char *) to_draw, "%d. %s  ",i + 1,adven[current_active_pc].items[i].name);
				else if (adven[current_active_pc].items[i].charges > 0)
					sprintf((char *) to_draw, "%d. %s (%d)",i + 1,adven[current_active_pc].items[i].full_name,
					adven[current_active_pc].items[i].charges);
				else sprintf((char *) to_draw, "%d. %s ",i + 1,adven[current_active_pc].items[i].full_name);			

			char_win_draw_string(main_dc,item_string_rects[i][0],(char *) to_draw,0,10);

			//Draw id/drop buttons
			rect_draw_some_item(mixed_gworld,d_from,mixed_gworld,item_string_rects[i][1],1,1);
			rect_draw_some_item(mixed_gworld,i_from,mixed_gworld,item_string_rects[i][2],1,1);
			}
	frame_dlog_rect(mainPtr,pc_info_rect,0); // re draw entire frame
	frame_dlog_rect(mainPtr,name_rect,0); // draw the frame
	frame_dlog_rect(mainPtr,pc_race_rect,0); // draw the frame
	frame_dlog_rect(mainPtr,info_area_rect,0); // draw the frame

}

void display_party(short,short clear_first)
//short mode; // 0 - 5 this pc, 6 - all
//short clear_first; // 0 - redraw over, 1 - don't redraw over
{
	short i;
	char to_draw[256],skill_value[256];

	RECT from_base = {0,0,28,36},from_rect;
	COLORREF colors[4] = {RGB(0,0,0),RGB(255,0,0),RGB(0,0,102),RGB(255,255,255)};
	//UINT c[4];


	short k,string_num, cur_rect=0;
	RECT no_party_rect,temp_rect;

	/*c[0] = GetNearestPaletteIndex(hpal,colors[0]);
	c[1] = GetNearestPaletteIndex(hpal,colors[1]);
	c[2] = GetNearestPaletteIndex(hpal,colors[2]);
	c[3] = GetNearestPaletteIndex(hpal,colors[3]);*/
	if (clear_first == 1) { // first erase what's already there
		for (i = 0; i < 6; i++)
			paint_pattern(NULL,1,pc_area_buttons[i][0],3);
		paint_pattern(NULL,1,name_rect,3);
		paint_pattern(NULL,1,pc_race_rect,3);
		paint_pattern(NULL,1,info_area_rect,3);
		frame_dlog_rect(mainPtr,pc_info_rect,0); // re-draw the frame
	}


	SelectObject(main_dc,bold_font);
	if (file_in_mem == FALSE) {
		no_party_rect=pc_info_rect;
		no_party_rect.top+=5;
		no_party_rect.left+=5;
		char_win_draw_string(main_dc,no_party_rect,"No party loaded.",0,10);
		}
		else {

        from_rect = title_from; //draw gold, food and day variables
        from_rect.top = from_rect.bottom-10;
        from_rect.left += 50;
        char GFD[256];
        sprintf(GFD,"Food: %d  Gold: %d  Day: %d",party.food,party.gold,(party.age/3700)+1);
        char_win_draw_string(main_dc,from_rect,GFD,0,10);

		from_rect = pc_info_rect;
		from_rect.top = from_rect.bottom - 14;
		from_rect.left += 3;
		if (party_in_scen == FALSE)
			char_win_draw_string(main_dc,from_rect,"Party not in a scenario.",0,10);
			else {
                char buf[256];
                sprintf(buf,"Party is in : %s.",party.scen_name);
                char_win_draw_string(main_dc,from_rect,buf,0,10);
            }
			
			for (i = 0; i < 6; i++) {
				if (i == current_active_pc)
					SetTextColor(main_dc,colors[1]);
					else SetTextColor(main_dc,colors[0]);

				from_rect = (current_pressed_button == i) ? ed_buttons_from[1] : ed_buttons_from[0];

				if ((current_pressed_button < 0) || (current_pressed_button == i))
					rect_draw_some_item(buttons_gworld,from_rect,buttons_gworld,pc_area_buttons[i][0],0,1);
				SetTextColor(main_dc,colors[0]);
				
				// pc_record_type adven[6] is the records that contains characters
				// main_status determins 0 - not exist, 1 - alive, OK, 2 - dead, 3 - stoned, 4 - dust
				if (adven[i].main_status != 0) { // PC exists?
					from_rect = from_base;
					// draw PC graphic
					OffsetRect(&from_rect,56 * (adven[i].which_graphic / 8),36 * (adven[i].which_graphic % 8));
					rect_draw_some_item(pc_gworld,from_rect,pc_gworld,pc_area_buttons[i][1],0,1);
					frame_dlog_rect(mainPtr,pc_area_buttons[i][1],0); // re-draw the frame

					//frame_dlog_rect((GrafPtr) mainPtr,pc_area_buttons[i][1],0); 
					// draw name
					//TextSize(9);
					if( (strlen(adven[i].name)) >= 0) {
						//TextFace(0);
						SelectObject(main_dc,font);
						sprintf((char *) to_draw, "%-s", (char *) adven[i].name);
						//TextSize(6);
						}
					else {
						sprintf((char *) to_draw, "%-s", (char *) adven[i].name);
						}

					if (i == current_active_pc)
						SetTextColor(main_dc,colors[1]);
						else SetTextColor(main_dc,colors[3]);
					win_draw_string(main_dc,pc_area_buttons[i][2],to_draw,1,10);
					SelectObject(main_dc,bold_font);


					if (i == current_active_pc){
						sprintf((char *) to_draw, "%-.18s  ", (char *) adven[i].name);
						if( (strlen(adven[i].name)) > 12)
							SelectObject(main_dc,font);
						SetTextColor(main_dc,colors[0]);
						win_draw_string(main_dc,name_rect,to_draw,1,10);
						SelectObject(main_dc,bold_font);
					}
					if ((current_pressed_button < 0) || (current_pressed_button == i))
					switch (adven[i].main_status) {
						// draw statistics
						case 1:
							if (i == current_active_pc) {
								//Draw in race
								if (adven[i].race == 0)
									char_win_draw_string(main_dc,pc_race_rect,"Human   ",1,10);
								if (adven[i].race == 1)
									char_win_draw_string(main_dc,pc_race_rect,"Nephilim   ",1,10);
								if (adven[i].race == 2)
									char_win_draw_string(main_dc,pc_race_rect,"Slithzerikai  ",1,10);
								// Draw in skills	
								
								sprintf((char *) to_draw, "Skills:");
								win_draw_string(main_dc,skill_rect,to_draw,0,10);
								sprintf((char *) to_draw, "Hp: %d/%d  Sp: %d/%d",adven[i].cur_health,adven[i].max_health,adven[i].cur_sp,
									adven[i].max_sp);
								win_draw_string(main_dc,hp_sp_rect,to_draw,0,10);
								
								
								SelectObject(main_dc,font);
								string_num=1;
								for( k = 0; k < 19 ; ++k)
								{
									temp_rect = pc_skills_rect[k];
									temp_rect.left = pc_skills_rect[k].left + ((k < 10) ? 90 : 83);
									
									get_str(to_draw,9,string_num);
									win_draw_string(main_dc,pc_skills_rect[k],to_draw,0,9);
									
									sprintf((char *) skill_value,"%d",adven[i].skills[k]);
									OffsetRect(&temp_rect,-8,0);
                           temp_rect.right += 10;
									win_draw_string(main_dc,temp_rect,skill_value,0,9);	
									//frame_dlog_rect((GrafPtr) mainPtr,pc_skills_rect[k],0);
									string_num+=2;
								}
								//end skills
								
								//Write in pc Status
								SelectObject(main_dc,bold_font);
								sprintf((char *) to_draw, "Status:");
								win_draw_string(main_dc,status_rect,to_draw,0,10);
								
								SelectObject(main_dc,font);
								//for(k = 0 ; k < 10; k++)
									//frame_dlog_rect((GrafPtr) mainPtr,pc_status_rect[k],0);
								if (adven[i].status[0] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Poisoned Weap.",0,9);
											cur_rect++;
										}
								if (adven[i].status[1] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Blessed",0,9);
											cur_rect++;
										}
								else if(adven[i].status[1] < 0)
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Cursed",0,9);
											cur_rect++;
										}
								if (adven[i].status[2] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Poisoned",0,9);
											cur_rect++;
										}	
								if (adven[i].status[3] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Hasted",0,9);
											cur_rect++;
										}
								else if(adven[i].status[3] < 0)
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Slowed",0,9);
											cur_rect++;
										}
								if (adven[i].status[4] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Invulnerable",0,9);
											cur_rect++;
										}
								if (adven[i].status[5] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Magic Resistant",0,9);
											cur_rect++;
										}
								if (adven[i].status[6] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Webbed",0,9);
											cur_rect++;
										}
								if (adven[i].status[7] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Diseased",0,9);
											cur_rect++;
										}
								if (adven[i].status[8] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Sanctury",0,9);
											cur_rect++;
										}
								if (adven[i].status[9] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Dumbfounded",0,9);
											cur_rect++;
										}
								if (adven[i].status[10] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Martyr's Shield",0,9);
											cur_rect++;
										}
								if (adven[i].status[11] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Asleep",0,9);
											cur_rect++;
										}
								if (adven[i].status[12] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Paralyzed",0,9);
											cur_rect++;
										}
								if (adven[i].status[13] > 0) 
										if(cur_rect <= 9) {
											char_win_draw_string(main_dc,pc_status_rect[cur_rect],"Acid",0,9);
											cur_rect++;
										}
								//end pc status section
								
								//Write in Traits
								SelectObject(main_dc,bold_font);
								sprintf((char *) to_draw, "Traits:");
								win_draw_string(main_dc,traits_rect,to_draw,0,10);
								SelectObject(main_dc,font);
								cur_rect=0;
								if (adven[i].traits[0] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Toughness",0,9);
											cur_rect++;
										}
								if (adven[i].traits[1] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Magically Apt",0,9);
											cur_rect++;
										}		
								if (adven[i].traits[2] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Ambidextrous",0,9);
											cur_rect++;
										}
								if (adven[i].traits[3] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Nimble Fingers",0,9);
											cur_rect++;
										}
								if (adven[i].traits[4] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Cave Lore",0,9);
											cur_rect++;
										}
										
								if (adven[i].traits[5] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Woodsman",0,9);
											cur_rect++;
										}
								if (adven[i].traits[6] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Good Constitution",0,9);
											cur_rect++;
										}		
								if (adven[i].traits[7] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Highly Alert",0,9);
											cur_rect++;
										}
								if (adven[i].traits[8] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Exceptional Str.",0,9);
											cur_rect++;
										}
								if (adven[i].traits[9] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Recuperation",0,9);
											cur_rect++;
										}
								if (adven[i].traits[10] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Sluggish",0,9);
											cur_rect++;
										}
								if (adven[i].traits[11] == 1) 
										if(cur_rect <= 15) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Magically Inept",0,9);
											cur_rect++;
										}		
								if (adven[i].traits[12] == 1) 
										if(cur_rect <= 14) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Frail",0,9);
											cur_rect++;
										}
								if (adven[i].traits[13] == 1)
										if(cur_rect <= 14) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Chronic Disease",0,9);
											cur_rect++;
										}
								if (adven[i].traits[14] == 1)
										if(cur_rect <= 13) {
											char_win_draw_string(main_dc,pc_traits_rect[cur_rect],"Bad Back",0,9);
											cur_rect++;
										}
										
								//end traits
							
							
							SetTextColor(main_dc,colors[0]);
							SelectObject(main_dc,bold_font);
							char_win_draw_string(main_dc,pc_area_buttons[i][3],"Alive ",1,10);
							SelectObject(main_dc,bold_font);
                            }
							break;
						case 2:
           				if (i == current_active_pc){
							SetTextColor(main_dc,colors[0]);
							SelectObject(main_dc,bold_font);
							char_win_draw_string(main_dc,pc_area_buttons[i][3],"Dead ",1,10);
							SelectObject(main_dc,bold_font);
							break;
                            }
						case 3:
                            if (i == current_active_pc){
							SetTextColor(main_dc,colors[0]);
							SelectObject(main_dc,bold_font);
							char_win_draw_string(main_dc,pc_area_buttons[i][3],"Dust ",1,10);
							SelectObject(main_dc,bold_font);
							break;
                            }
						case 4:
                            if (i == current_active_pc){
							SetTextColor(main_dc,colors[0]);
							SelectObject(main_dc,bold_font);
							char_win_draw_string(main_dc,pc_area_buttons[i][3],"Stone ",1,10);
							SelectObject(main_dc,bold_font);
							break;
                            }
						case 5:
                            if (i == current_active_pc){
							SetTextColor(main_dc,colors[0]);
							SelectObject(main_dc,bold_font);
							char_win_draw_string(main_dc,pc_area_buttons[i][3],"Fled ",1,10);
							SelectObject(main_dc,bold_font);
							break;
                            }
						case 6:
                            if (i == current_active_pc){
							SetTextColor(main_dc,colors[0]);
							SelectObject(main_dc,bold_font);
							char_win_draw_string(main_dc,pc_area_buttons[i][3],"Surface ",1,10);
							SelectObject(main_dc,bold_font);
							break;
                            }
						default:
                            if (i == current_active_pc){
							SetTextColor(main_dc,colors[0]);
							SelectObject(main_dc,bold_font);
							char_win_draw_string(main_dc,pc_area_buttons[i][3],"Absent ",1,10);
							SelectObject(main_dc,bold_font);
							break;
                            }
						}
				//frame_dlog_rect((GrafPtr) mainPtr,pc_area_buttons[i][0],0); 
				

				}
				
			} // Closes the for i=6 loop
		SetTextColor(main_dc,colors[0]);
		
		for(i = 0; i < 5; i++) 
			if ((current_pressed_button < 0) || (current_pressed_button == i + 10)) {	
			if (clear_first == 1) { // first erase what's already there
				paint_pattern(NULL,1,edit_rect[i][0],3);
				}		

			from_rect = (current_pressed_button == i + 10) ? ed_buttons_from[1] : ed_buttons_from[0];
			rect_draw_some_item(buttons_gworld,from_rect,buttons_gworld,edit_rect[i][0],0,1);
			SetTextColor(main_dc,colors[3]);
			switch(i) {
				case 0:
					char_win_draw_string(main_dc,edit_rect[0][1]," Add|Mage|Spells ",0,10);
					break;
				case 1:
					char_win_draw_string(main_dc,edit_rect[1][1]," Add|Priest|Spells ",0,10);
					break;
				case 2:
					char_win_draw_string(main_dc,edit_rect[2][1]," Edit|Traits",0,10);
					break;
				case 3:
					char_win_draw_string(main_dc,edit_rect[3][1]," Edit|Skills",0,10);
					break;
				case 4:
					char_win_draw_string(main_dc,edit_rect[4][1]," Edit|  XP",0,10);
					break;
				default:
					break;
			}
			SetTextColor(main_dc,colors[0]);
			 
		}

		}
SetTextColor(main_dc,colors[0]);
}

void display_strings_event_filter (short item_hit)
{


	
	switch (item_hit) {
		case 1:
			dialog_not_toast = FALSE;
			break;
		}
}

void display_strings(short str1a,short str1b,short str2a,short str2b,
	char *title,short sound_num,short graphic_num,short parent_num)
{


	char sign_text[256];



	make_cursor_sword();
	
	store_str1a = str1a;
	store_str1b = str1b;
	store_str2a = str2a;
	store_str2b = str2b;
	
	if ((str1a <= 0) || (str1b <= 0))
		return;
	store_which_string_dlog = 970;
	if (strlen(title) > 0)
		store_which_string_dlog += 2;
	if ((str2a > 0) && (str2b > 0))
		store_which_string_dlog++;
	cd_create_dialog_parent_num(store_which_string_dlog,parent_num);
	
	cd_activate_item(store_which_string_dlog,2,0);
	
	csp(store_which_string_dlog,store_which_string_dlog,graphic_num);
	
	get_str(sign_text,str1a,str1b);
	csit(store_which_string_dlog,4,(char *) sign_text);
	if ((str2a > 0) && (str2b > 0)) {
		get_str(sign_text,str2a,str2b);
		csit(store_which_string_dlog,5,(char *) sign_text);
		}
	if (strlen(title) > 0)
		csit(store_which_string_dlog,6,title);
	csp(store_which_string_dlog,3,graphic_num);
	if (sound_num >= 0)
		play_sound(sound_num);
	while (dialog_not_toast)
		ModalDialog();	
	
	cd_kill_dialog(store_which_string_dlog,0);
}

void get_str(char *str,short i, short j)
{
	GetIndString(str, i, j);
}

void char_win_draw_string(HDC dest_window,RECT dest_rect,char *str,short mode,short line_height)
{
	char store_s[256];
	
	strcpy((char *) store_s,str);
	win_draw_string( dest_window, dest_rect,store_s, mode, line_height);

}

// mode: 0 - align up and left, 1 - center on one line
// str is a c string, 256 characters
// uses current font
void win_draw_string(HDC dest_hdc,RECT dest_rect,char *str,short mode,short)
{
	short i;

// this will need formatting for '|' line breaks
	for (i = 0; i < 256; i++)  {
		if (str[i] == '|')
			str[i] = 13;
		if (str[i] == '_')
      	str[i] = 34;
		}
	// if dest is main window, add ulx, uly
	if (dest_hdc == main_dc)
		OffsetRect(&dest_rect,ulx,uly);
	switch (mode) {
		case 0:
         dest_rect.bottom += 6;
			DrawText(dest_hdc,str,strlen((char *)str),&dest_rect,DT_LEFT | DT_WORDBREAK); break;
		case 1:
			dest_rect.bottom += 6; dest_rect.top -= 6;
			DrawText(dest_hdc,str,strlen((char *)str),&dest_rect,
			DT_CENTER | DT_VCENTER | DT_NOCLIP | DT_SINGLELINE); break;
		case 2: case 3:
			dest_rect.bottom += 6; dest_rect.top -= 6;
			DrawText(dest_hdc,str,strlen((char *)str),&dest_rect,
			DT_LEFT | DT_VCENTER | DT_NOCLIP | DT_SINGLELINE); break;
		}
	// not yet done adjusts for 1, 2, 3
	
}

short string_length(char *str,HDC hdc)
{
	short text_len[257];
	short total_width = 0,i,len;
	char p_str[256];
	
	for (i = 0; i < 257; i++)
		text_len[i]= 0;
	
	strcpy((char *) p_str,str);
	MeasureText(256,p_str,text_len,hdc);
	len = strlen((char *)str);

	for (i = 0; i < 257; i++)
		if ((text_len[i] > total_width) && (i <= len))
			total_width = text_len[i];
	return total_width;
}

// Note ... this expects a str len of at most 256 and
// len_array pointing to a 256 long array of shorts
void MeasureText(short str_len,char *str, short *len_array,HDC hdc)
{
	short text_len[257];
	short i;
	char p_str[257];
	SIZE val_returned;
	char *store_array;
	short *store2;

	store_array = (char *) len_array;
	for (i = 0; i < 256; i++)
		text_len[i] = 0;
	for (i = 1; i < str_len; i++) {
		strncpy(p_str,str,i);
		p_str[i] = 0;
		GetTextExtentPoint32(hdc,p_str,i, &val_returned);
		text_len[i] = val_returned.cx;
		}
	for (i = 0; i < 256; i++) {
		store2 = (short *) store_array;
		*store2 = text_len[i];
		store_array += 2;
		}
	}


void GetIndString(char *str,short i, short j) {
	UINT resnum = 0,len;
	short k;

	resnum = i * 300 + j;

	len = LoadString(store_hInstance,resnum,str,256);
	if (len == 0) {
		sprintf(str,"");
		return;
		}
	for (k = 0; k < 256; k++)  {
		if (str[k] == '|')
			str[k] = 13;
		if (str[k] == '_')
			str[k] = 34;
		}
}


void make_cursor_sword() 
{
	SetCursor(sword_curs);
}
