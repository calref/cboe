#include <Windows.h>

#include "stdio.h"
#include "string.h"
#include "global.h"
#include "graphics.h"
#include "math.h"
#include "dlogtool.h"
#include "edsound.h"
#include "graphutl.h"
#include "keydlgs.h"

extern char scen_strs[160][256];
extern char scen_strs2[110][256];
extern char talk_strs[170][256];
extern char town_strs[180][256];
extern HFONT font,italic_font,underline_font,bold_font,tiny_font,small_bold_font;
extern HDC main_dc,main_dc2,main_dc3;

extern HCURSOR cursors[8];
extern outdoor_record_type current_terrain;
extern HWND	mainPtr;
extern town_record_type current_town;
extern short cen_x, cen_y,current_terrain_type,cur_town;
extern town_record_type town;
extern big_tr_type t_d;
extern template_town_type town_template;
extern short town_type;  // 0 - big 1 - ave 2 - small
extern short cur_viewing_mode,overall_mode;
extern short available_dlog_buttons[NUM_DLOG_B];
extern Boolean editing_town;
extern short max_dim[3];
extern short dungeon_font_num,geneva_font_num;
extern RECT windRect;
extern piles_of_stuff_dumping_type *data_store;
extern scenario_data_type scenario;
extern RECT world_screen;
extern scen_item_data_type scen_item_list;

extern RECT left_button[NLS];
extern RECT right_buttons[NRSONPAGE];
extern RECT right_scrollbar_rect;
extern RECT right_area_rect;
extern unsigned char borders[4][50];
extern HWND right_sbar;
extern RECT right_sbar_rect;

extern Boolean left_buttons_active,right_buttons_active;
extern short left_button_status[NLS]; // 0 - clear, 1 - text, 2 - title text, +10 - button
extern short right_button_status[NRS];
extern unsigned char m_pic_index[200];
extern char *button_strs[140];
extern location cur_out;
extern short ulx,uly;
extern HINSTANCE store_hInstance;
extern HACCEL accel;
extern unsigned long anim_ticks;

short num_ir[3] = {12,10,4};

GWorldPtr monst_gworld[10],terrain_gworld[6],small_terrain_gworld,editor_mixed,terrain_buttons_gworld;
GWorldPtr anim_gworld,field_gworld,dialog_gworld,items_gworld,tiny_obj_gworld;
GWorldPtr dlg_buttons_gworld,ter_draw_gworld,dlogpics_gworld,talkfaces_gworld,mixed_gworld;
GWorldPtr dialog_pattern_gworld,pattern_gworld;
HBITMAP spec_scen_g = NULL;

	short small_icons[24] = {0,23,37,38,39,35,33,34,30,0,
							30,26,0,0,36,0,27,28,29,20,
							21,22,0,0};

HBRUSH map_brush[25];
HBITMAP map_bitmap[25];

// begin new stuff
RECT blue_button_from = {91,112,107,126};//
RECT start_button_from = {70,112,91,119};
RECT boat_button_from = {61,1,88,35};
RECT horse_button_from = {117,110,144,145};
RECT base_small_button_from = {0,112,7,119};
extern RECT palette_buttons[8][6];
RECT palette_button_base = {0,0,25,18};
RECT terrain_buttons_rect = {0,0,294,410};
extern RECT left_buttons[NLS][2]; // 0 - whole, 1 - blue button
RECT left_button_base = {5,5,200,21};
RECT right_button_base = {RIGHT_AREA_UL_Y,RIGHT_AREA_UL_X,17,RIGHT_AREA_UL_Y};
RECT terrain_rect = {0,0,272,340};
char current_string[256] = "";
char current_string2[256] = "";
extern RECT terrain_rects[256];

short map_pats[220] = {50,50,1,1,1,6,6,6,6,6,
				6,6,6,6,6,6,6,6,2,2,
				2,2,2,2,2,2,2,2,2,2,
				2,2,4,4,4,4,4,4,4,4,
				4,4,4,4,4,4,3,3,3,3,
				3,3,3,3,3,3,3,3,3,0, // 50
				0,0,0,0,0,0,0,23,23,23,
				15,15,50,50,0,0,0,0,0,7,
				7,8,8,14,14,9,9,9,5,5,
				0,0,0,0,0,0,0,0,0,0,
				18,18,0,0,0,0,0,0,0,0, // 100
				22,22,0,0,0,0,0,0,0,0,
				0,0,0,10,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,11,0,0, // 150
				0,0,0,12,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0,
				0,0,0,0,0,0,0,0,0,0, // 200
				0,0,0,0,0,0,0,0,0,0
				};


unsigned char small_what_drawn[64][64];
extern Boolean small_any_drawn;

void Set_up_win ()
{
	short i,j;
   RECT whole_win_rect;

	GetClientRect(mainPtr,&whole_win_rect);
	ulx = (whole_win_rect.right - 600) / 2 ;
	uly = (whole_win_rect.bottom - 440) / 2 + 8;


	for (i = 0; i < 8; i++)
		for (j = 0; j < 6; j++) {
		palette_buttons[i][j] = palette_button_base;
		OffsetRect(&palette_buttons[i][j],i * 25, j * 17);
		if (i == 7)
			palette_buttons[i][j].right++;
		}
	for (i = 0; i < 8; i++)
		for (j = 2; j < 6; j++)
			OffsetRect(&palette_buttons[i][j],0,3);
	for (i = 0; i < 8; i++)
		for (j = 3; j < 6; j++)
			OffsetRect(&palette_buttons[i][j],0,3);
	for (i = 0; i < 8; i++)
		for (j = 4; j < 6; j++)
			OffsetRect(&palette_buttons[i][j],0,3);

	for (i = 0; i < NLS; i++) {
		left_buttons[i][0] = left_button_base;
		OffsetRect(&left_buttons[i][0],0,i * 16);
		left_buttons[i][1] = left_buttons[i][0];
		left_buttons[i][1].top += 1;
		left_buttons[i][1].bottom -= 1;
		left_buttons[i][1].left += 0;
		left_buttons[i][1].right = left_buttons[i][1].left + 16;
		}
	right_button_base.left = RIGHT_AREA_UL_X + 1;
	right_button_base.top = RIGHT_AREA_UL_Y + 1;
	right_button_base.bottom = right_button_base.top + 12;
	right_button_base.right = right_button_base.left + RIGHT_AREA_WIDTH - 20;
	for (i = 0; i < NRSONPAGE; i++) {
		right_buttons[i] = right_button_base;
		OffsetRect(&right_buttons[i],0,i * 12);
		}
	load_main_screen();
}

void run_startup_g()
{
	RECT pict_rect = {0,0,640,480},to_rect;
	//PicHandle	pict_to_draw;
	HBITMAP start_g;
	RECT pat_rect;
	HBRUSH old_brush;

	pat_rect = windRect;
	InflateRect(&pat_rect,500,500);
//	paint_pattern(NULL,1,pat_rect,3);
	old_brush = (HBRUSH) SelectObject(main_dc,GetStockObject(BLACK_BRUSH));
	Rectangle(main_dc, pat_rect.left,pat_rect.top,
		pat_rect.right,pat_rect.bottom);
	SelectObject(main_dc,old_brush);

	start_g = load_pict(3002);
	to_rect = pict_rect;
	OffsetRect(&to_rect,-18,-30);
	rect_draw_some_item(start_g,pict_rect,start_g,to_rect,0,1);
	DeleteObject(start_g);
	play_sound(95);

}

void load_main_screen()
{

	short i;
 	RECT map_from_orig = {372,0,380,8},map_from,brush_to = {0,0,8,8};

	main_dc = GetDC(mainPtr);

	SelectObject(main_dc,font);
	SetBkMode(main_dc,TRANSPARENT);


	SetStretchBltMode(main_dc,STRETCH_DELETESCANS);

	terrain_buttons_gworld = CreateCompatibleBitmap(main_dc,terrain_buttons_rect.right,terrain_buttons_rect.bottom);

	ter_draw_gworld = CreateCompatibleBitmap(main_dc,terrain_rect.right,terrain_rect.bottom);

	dlg_buttons_gworld = load_pict(2000);
	mixed_gworld = load_pict(903);

	run_startup_g();


	world_screen = terrain_rect;
	OffsetRect(&world_screen,TER_RECT_UL_X,TER_RECT_UL_Y);

	for (i = 0; i < 10; i++)
		monst_gworld[i] = load_pict(1100 + i);
	for (i = 0; i < 6; i++)
		terrain_gworld[i] = load_pict(800 + i);
	small_terrain_gworld = load_pict(904);
	editor_mixed = load_pict(906);
	anim_gworld = load_pict(820);
	field_gworld = load_pict(821);
	talkfaces_gworld = load_pict(860);
	items_gworld = load_pict(901);
	tiny_obj_gworld = load_pict(900);
	dlogpics_gworld = load_pict(850);
	dialog_pattern_gworld = CreateCompatibleBitmap(main_dc,192,256);
	pattern_gworld = CreateCompatibleBitmap(main_dc,192,256);
	//draw_main_screen();

	for (i = 0; i < 25; i++) {
		map_from = map_from_orig;
		OffsetRect(&map_from,8 * (i / 10),8 * (i % 10));
		map_bitmap[i] = CreateCompatibleBitmap(main_dc,8,8);
		rect_draw_some_item(mixed_gworld,map_from,map_bitmap[i],brush_to,0,0);
		map_brush[i] = CreatePatternBrush(map_bitmap[i]);
		}
}

void redraw_screen()
{
	RECT pat_rect;

	pat_rect = windRect;
	InflateRect(&pat_rect,500,500);
	OffsetRect(&pat_rect,-ulx, -uly); //to counteract the shifting done in paint_pattern for ALL patterns. We're dealing with the background pattern here.
	paint_pattern(NULL,1,pat_rect,3);
	draw_main_screen();
	if (overall_mode < 60);
		draw_terrain();
}

void draw_main_screen()
{
	RECT draw_rect,big_rect = {-1,-1,582,414};
	HBRUSH new_brush;
	COLORREF y = RGB(128,128,128);//y = RGB(119,119,119);

	// draw left buttons (always active)
	draw_lb();

	// draw right buttons (only when not editing terrain)
	if (overall_mode >= 60) {
		draw_rect.left = RIGHT_AREA_UL_X;
		draw_rect.top = RIGHT_AREA_UL_Y;
		draw_rect.right = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 16;
		draw_rect.bottom = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT;

		//c = GetNearestPaletteIndex(hpal,y);
		new_brush = CreateSolidBrush(y);
		OffsetRect(&draw_rect,ulx,uly);
		FrameRect(main_dc,&draw_rect,new_brush);
		OffsetRect(&big_rect,ulx,uly);
		if (ulx > 40)
			FrameRect(main_dc,&big_rect,new_brush);
		DeleteObject(new_brush);

		InsetRect(&draw_rect,1,1);
		OffsetRect(&draw_rect,-1 * ulx,-1 * uly);
		paint_pattern(NULL,1,draw_rect,3);

		draw_rb();
		}

	// draw terrain palette
	if ((overall_mode < 60) || (overall_mode == 62)) {
		place_location();
		}


}

void draw_lb()
{
	RECT temp_rect;
	short i;

	temp_rect = windRect;
	temp_rect.right = RIGHT_AREA_UL_X - 2;
	paint_pattern(NULL,1,temp_rect,3);
	for (i = 0; i < NLS; i++)
		draw_lb_slot(i,0);
}

// mode 0 normal 1 click
void draw_lb_slot (short which,short mode)
{
	RECT text_rect,from_rect;
 	COLORREF colors = RGB(0,0,102),colors2 = RGB(0,0,0);

	paint_pattern(NULL,1,left_buttons[which][0],3);
	if (left_button_status[which] == 0)
		return;
	text_rect = left_buttons[which][0];
	if (left_button_status[which] >= 10) {
		text_rect.left += 18;
		from_rect = blue_button_from;
		if (mode > 0)
			OffsetRect(&from_rect,from_rect.right - from_rect.left,0);
		rect_draw_some_item(editor_mixed,from_rect,editor_mixed,left_buttons[which][1],0,1);
		}
	if (left_button_status[which] % 10 == 3)
		text_rect.left += 16;
	if (left_button_status[which] % 10 == 2) {
		SelectObject(main_dc,bold_font);
		}
		else OffsetRect(&text_rect,0,2);
	if (mode > 0)
		SetTextColor(main_dc,colors);

	char_win_draw_string(main_dc,text_rect,(char *)data_store->strings_ls[which],0,12);
	SetTextColor(main_dc,colors2);

	SelectObject(main_dc,font);

}

void draw_rb()
{
	short i,pos;

 	pos = GetScrollPos(right_sbar,SB_CTL);
	for (i = pos; i < pos + NRSONPAGE; i++)
		draw_rb_slot(i,0);
}

// mode 0 normal 1 pressed
void draw_rb_slot (short which,short mode)
{
	RECT text_rect;
 	short pos;
  	COLORREF colors = RGB(255,0,0),colors2 = RGB(0,0,0);

 	pos = GetScrollPos(right_sbar,SB_CTL);
	if ((which < pos) || (which >= pos + NRSONPAGE))
		return;

	text_rect = right_buttons[which - pos];
	text_rect.right += 2;
   text_rect.bottom += 2;
	paint_pattern(NULL,1,text_rect,1);
	text_rect.bottom -= 2;
	if (right_button_status[which] == 0)
		return;

	if (mode > 0)
		SetTextColor(main_dc,colors);
   OffsetRect(&text_rect,0,-1);
	char_win_draw_string(main_dc,text_rect,(char *)data_store->strings_rs[which],0,12);
	SetTextColor(main_dc,colors2);
}

void set_up_terrain_buttons()
{
	short i,pic,small_i;
	RECT ter_from,ter_from_base = {0,0,28,36};
	RECT tiny_from,tiny_to;
	HDC hdc;
	RECT palette_from = {0,0,0,0},palette_to;
	HBITMAP store_bmp;
	COLORREF y = RGB(128,128,128);//y = RGB(119,119,119);
	HBRUSH new_brush;

	paint_pattern(terrain_buttons_gworld,0,terrain_buttons_rect,1);

	hdc = CreateCompatibleDC(main_dc);

	SetBkMode(hdc,TRANSPARENT);
	SelectObject(hdc,small_bold_font);
	SetStretchBltMode(hdc,STRETCH_DELETESCANS);
	store_bmp = (HBITMAP)SelectObject(hdc,terrain_buttons_gworld);

	new_brush = CreateSolidBrush(y);

	store_bmp = (HBITMAP)SelectObject(hdc,terrain_buttons_gworld);
	FrameRect(hdc,&terrain_buttons_rect,new_brush);

 	// first make terrain buttons
	for (i = 0; i < 256; i++) {
		ter_from = ter_from_base;
		pic = scenario.ter_types[i].picture;
		}
	SelectObject(hdc,store_bmp);
	DeleteObject(hdc);
	DeleteObject(new_brush);
	SetStretchBltMode(main_dc,STRETCH_DELETESCANS);

	// first make terrain buttons
	for (i = 0; i < 256; i++) {
		ter_from = ter_from_base;
		pic = scenario.ter_types[i].picture;
		if (pic >= 1000) {
			ter_from = get_custom_rect(pic % 1000);
			rect_draw_some_item(spec_scen_g,ter_from,terrain_buttons_gworld,terrain_rects[i],0,0);
			}
		else if (pic < 400)	{
				pic = pic % 50;
				OffsetRect(&ter_from,28 * (pic % 10), 36 * (pic / 10));
				rect_draw_some_item(terrain_gworld[scenario.ter_types[i].picture/50],ter_from,terrain_buttons_gworld,terrain_rects[i],0,0);
				}
				else if (pic < 1000) {
					pic = pic % 50;
					ter_from.left = 112 * (pic / 5);
					ter_from.right = ter_from.left + 28;
					ter_from.top = 36 * (pic % 5);
					ter_from.bottom = ter_from.top + 36;

					rect_draw_some_item(anim_gworld,ter_from,terrain_buttons_gworld,terrain_rects[i],0,0);
					}
		small_i = small_icons[scenario.ter_types[i].special];
		if ((small_i == 30) && (scenario.ter_types[i].flag2 >= 5))
			small_i = 31;
		if ((small_i == 31) && (scenario.ter_types[i].flag2 == 10))
			small_i = 32;
		if (i == 82)
			small_i = 3;
		if (i == 83)
			small_i = 2;
		if ((i == 7) || (i == 10) || (i == 13) || (i == 16))
			small_i = 23;
		tiny_from = base_small_button_from;
		OffsetRect(&tiny_from,7 * (small_i % 10),7 * (small_i / 10));
		tiny_to = terrain_rects[i];
		tiny_to.top = tiny_to.bottom - 7;
		tiny_to.left = tiny_to.right - 7;
		if (small_i > 0) {
			rect_draw_some_item(editor_mixed,
				tiny_from,terrain_buttons_gworld,tiny_to,0,0);
			}
		}

	if (overall_mode < 60) {
		palette_to.left = 5;
		palette_to.top = terrain_rects[255].bottom + 5;
		if (editing_town == TRUE) {
			palette_from.bottom = palette_buttons[0][5].bottom;
			palette_from.right = palette_buttons[7][5].right;
			}
			else {
				palette_from.bottom = palette_buttons[0][2].bottom;
				palette_from.right = palette_buttons[7][2].right;
				}
		palette_to.right = palette_to.left + palette_from.right;
		palette_to.bottom = palette_to.top + palette_from.bottom;
		rect_draw_some_item(editor_mixed,
			palette_from,terrain_buttons_gworld,palette_to,1,0);
		}


}

void draw_terrain()
{
	short q,r,x,i,small_i;
	location which_pt,where_draw;
	RECT draw_rect,clipping_rect = {8,8,260,332};
	unsigned char t_to_draw;
	RECT tiny_to,tiny_to_base = {29,37,36,44},tiny_from,from_rect,to_rect;
	HBITMAP store_bmp;
	COLORREF gray = RGB(128,128,128);
	COLORREF red = RGB(255,0,0);
	COLORREF white = RGB(255,255,255);

	HBRUSH new_brush;
	HDC hdc;
	HPEN old_pen;

	if (overall_mode >= 60)
		return;

	if (cur_viewing_mode == 0) {
		paint_pattern(ter_draw_gworld,0,terrain_rect,1);
		}
	hdc = CreateCompatibleDC(main_dc);

	SetBkMode(hdc,TRANSPARENT);
	SelectObject(hdc,small_bold_font);
	store_bmp = (HBITMAP)SelectObject(hdc,ter_draw_gworld);


	new_brush = CreateSolidBrush(gray);
	FrameRect(hdc,&terrain_rect,new_brush);
	DeleteObject(new_brush);
	old_pen = (HPEN)SelectObject(hdc,GetStockObject(NULL_PEN));
	SelectObject(hdc,store_bmp);

	if (cur_viewing_mode == 0) {
	for (q = 0; q < 9; q++)
		for (r = 0; r < 9; r++)
			{
			where_draw.x = q; where_draw.y = r;
			if (editing_town == TRUE) {
				t_to_draw = t_d.terrain[cen_x + q - 4][cen_y + r - 4];
				}
				else {
					if (cen_x + q - 4 == -1)
						t_to_draw = borders[3][cen_y + r - 4];
					else if (cen_x + q - 4 == 48)
						t_to_draw = borders[1][cen_y + r - 4];
					else if (cen_y + r - 4 == -1)
						t_to_draw = borders[0][cen_x + q - 4];
					else if (cen_y + r - 4 == 48)
						t_to_draw = borders[2][cen_x + q - 4];
					else t_to_draw = current_terrain.terrain[cen_x + q - 4][cen_y + r - 4];
					}
			draw_one_terrain_spot(q,r,t_to_draw);
			which_pt.x = cen_x + q - 4;
			which_pt.y =cen_y + r - 4;

			tiny_to = tiny_to_base;
			OffsetRect(&tiny_to,28 * q, 36 * r);

			// draw start icon, if starting point
			if ((editing_town == TRUE) &&
				(cur_town == scenario.which_town_start) && (scenario.where_start.x == cen_x + q - 4)
				&& (scenario.where_start.y == cen_y + r - 4)) {
				from_rect = start_button_from;
				to_rect = tiny_to;
				to_rect.left -= 14;
				rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect,0,0);
				OffsetRect(&tiny_to,0,-7);
				}
			if ((editing_town == FALSE)
				&& (scenario.out_sec_start.x == cur_out.x)
				&& (scenario.out_sec_start.y == cur_out.y)
				&& (scenario.out_start.x == cen_x + q - 4)
				&& (scenario.out_start.y == cen_y + r - 4)) {
				from_rect = start_button_from;
				to_rect = tiny_to;
				to_rect.left -= 14;
				rect_draw_some_item(editor_mixed,from_rect,ter_draw_gworld,to_rect,0,0);
				OffsetRect(&tiny_to,0,-7);
				}
			small_i = small_icons[scenario.ter_types[t_to_draw].special];
			if ((small_i == 30) && (scenario.ter_types[t_to_draw].flag2 >= 5))
				small_i = 31;
			if ((small_i == 31) && (scenario.ter_types[t_to_draw].flag2 == 10))
				small_i = 32;
			tiny_from = base_small_button_from;
			OffsetRect(&tiny_from,7 * (small_i % 10),7 * (small_i / 10));
			if (small_i > 0) {
				rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
				OffsetRect(&tiny_to,0,-7);
				}

				if (is_special(cen_x + q - 4,cen_y + r - 4) == TRUE) {
					tiny_from = base_small_button_from;
					OffsetRect(&tiny_from,7 * (7),7 * (0));
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
					OffsetRect(&tiny_to,0,-7);
					}
				if ((t_to_draw == 7) || (t_to_draw == 10) || (t_to_draw == 13) || (t_to_draw == 16)) {
					tiny_from = base_small_button_from;
					OffsetRect(&tiny_from,7 * (3),7 * (2));
					rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
					OffsetRect(&tiny_to,0,-7);
					}
				if (editing_town == FALSE) {
					for (i = 0; i < 4; i++)
						if ((cen_x + q - 4 == current_terrain.wandering_locs[i].x) &&
							(cen_y + r - 4 == current_terrain.wandering_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (2),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
							i = 4;
							}

					}

				if (editing_town == TRUE) {
					for (i = 0; i < 4; i++)
						if ((cen_x + q - 4 == town.start_locs[i].x) &&
							(cen_y + r - 4 == town.start_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (6 + i),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
							}
					for (i = 0; i < 4; i++)
						if ((cen_x + q - 4 == town.wandering_locs[i].x) &&
							(cen_y + r - 4 == town.wandering_locs[i].y)) {
							tiny_from = base_small_button_from;
							OffsetRect(&tiny_from,7 * (2),7 * (1));
							rect_draw_some_item(editor_mixed,tiny_from,ter_draw_gworld,tiny_to,0,0);
							OffsetRect(&tiny_to,0,-7);
							i = 4;
							}
					if (is_web(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(5,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}
					if (is_crate(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(6,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}
					if (is_barrel(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(7,0);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}
					if (is_fire_barrier(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(0,2);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}
					if (is_quickfire(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(7,1);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}
					if (is_force_barrier(cen_x + q - 4,cen_y + r - 4) == TRUE) {
						from_rect = calc_rect(2,2);
						Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
						}
					for (i = 0; i < 8; i++)
						if (is_sfx(cen_x + q - 4,cen_y + r - 4,i)) {
							from_rect = calc_rect(i,3);
							Draw_Some_Item(field_gworld,from_rect,ter_draw_gworld,where_draw,1,0);
							}
					for (x = 0; x < 64; x++)
						if ((cen_x + q - 4 == town.preset_items[x].item_loc.x) &&
						 (cen_y + r - 4 == town.preset_items[x].item_loc.y) && (town.preset_items[x].item_code >= 0)) {
						 }
					for (x = 0; x < 60; x++)
						if ((cen_x + q - 4 == t_d.creatures[x].start_loc.x) &&
						 (cen_y + r - 4 == t_d.creatures[x].start_loc.y) && (t_d.creatures[x].number != 0)) {
						 }

					 }
				}
	if (editing_town == TRUE) {
		draw_monsts(hdc);
		draw_items(hdc);
		}

	store_bmp = (HBITMAP)SelectObject(hdc,ter_draw_gworld);
	ClipRect(hdc,&clipping_rect);

	if (editing_town == TRUE) {
		// draw info rects
		for (i = 0; i < 16; i++)
			if (t_d.room_rect[i].left > -1) {
				draw_rect.left = 22 + 28 * (t_d.room_rect[i].left - cen_x + 4);
				draw_rect.right = 22 + 28 * (t_d.room_rect[i].right - cen_x + 4);
				draw_rect.top = 24 + 36 * (t_d.room_rect[i].top - cen_y + 4);
				draw_rect.bottom = 24 + 36 * (t_d.room_rect[i].bottom - cen_y + 4);

				//c = GetNearestPaletteIndex(hpal,red);
				new_brush = CreateSolidBrush(red);
				FrameRect(hdc,&draw_rect,new_brush);
				DeleteObject(new_brush);
				}
		// draw border rect
			draw_rect.left = 21 + 28 * (town.in_town_rect.left - cen_x + 4);
			draw_rect.right = 21 + 28 * (town.in_town_rect.right - cen_x + 4);
			draw_rect.top = 25 + 36 * (town.in_town_rect.top - cen_y + 4);
			draw_rect.bottom = 25 + 36 * (town.in_town_rect.bottom - cen_y + 4);

//			c = GetNearestPaletteIndex(hpal,white);
			new_brush = CreateSolidBrush(white);
			FrameRect(hdc,&draw_rect,new_brush);
			DeleteObject(new_brush);
		}
	if (editing_town == FALSE) {
		// draw info rects
		for (i = 0; i < 8; i++)
			if (current_terrain.info_rect[i].left > -1) {
				draw_rect.left = 22 + 28 * (current_terrain.info_rect[i].left - cen_x + 4);
				draw_rect.right = 22 + 28 * (current_terrain.info_rect[i].right - cen_x + 4);
				draw_rect.top = 24 + 36 * (current_terrain.info_rect[i].top - cen_y + 4);
				draw_rect.bottom = 24 + 36 * (current_terrain.info_rect[i].bottom - cen_y + 4);
				//c = GetNearestPaletteIndex(hpal,red);
				new_brush = CreateSolidBrush(red);

				FrameRect(hdc,&draw_rect,new_brush);
				DeleteObject(new_brush);
				}
		}
	undo_clip(hdc);
	SelectObject(hdc,store_bmp);

	small_any_drawn = FALSE;
	}

	if (cur_viewing_mode == 1) {
		if (small_any_drawn == FALSE) {
			InsetRect(&terrain_rect,1,1);
			paint_pattern(ter_draw_gworld,0,terrain_rect,1);
			InsetRect(&terrain_rect,-1,-1);
			//FrameRect(&terrain_rect);
			}
		for (q = 0; q < ((editing_town == TRUE) ? max_dim[town_type] : 48); q++)
			for (r = 0; r < ((editing_town == TRUE) ? max_dim[town_type] : 48); r++) {
				t_to_draw = (editing_town == TRUE) ? t_d.terrain[q][r] :
					current_terrain.terrain[q][r];
				if ((small_what_drawn[q][r] != t_to_draw) || (small_any_drawn == FALSE)) {
					draw_one_tiny_terrain_spot(q,r,t_to_draw,hdc);
					small_what_drawn[q][r] = t_to_draw;
					}
			}
		small_any_drawn = TRUE;
		}

	SelectObject(hdc,old_pen);
	DeleteObject(hdc);
	//draw_cur_string();
	place_location();

	rect_draw_some_item(ter_draw_gworld,terrain_rect,ter_draw_gworld,world_screen,0,1);
}

void draw_monsts(HDC hdc)
{
	short i,k,width,height,m_start_pic;
	GWorldPtr from_gworld;
	RECT source_rect;
	location where_draw,store_loc;

		for (i = 0; i < 60; i++)
		if (t_d.creatures[i].number != 0) {
				where_draw.x = t_d.creatures[i].start_loc.x - cen_x + 4;
				where_draw.y = t_d.creatures[i].start_loc.y - cen_y + 4;
				width = scenario.scen_monsters[t_d.creatures[i].number].x_width;
				height = scenario.scen_monsters[t_d.creatures[i].number].y_width;

				for (k = 0; k < width * height; k++) {
					store_loc = where_draw;
					if ((where_draw.x == minmax(0,8,where_draw.x)) &&
					(where_draw.y == minmax(0,8,where_draw.y)) &&
						(scenario.scen_monsters[t_d.creatures[i].number].picture_num >= 1000)) {
						source_rect = get_custom_rect((scenario.scen_monsters[t_d.creatures[i].number].picture_num + k) % 1000);
						store_loc.x += k % width;
						store_loc.y += k / width;
						Draw_Some_Item(spec_scen_g, source_rect, ter_draw_gworld, store_loc, 1, 0);
						}
						else if (scenario.scen_monsters[t_d.creatures[i].number].picture_num < 1000) {
							m_start_pic = m_pic_index[scenario.scen_monsters[t_d.creatures[i].number].picture_num] + k;
							from_gworld = monst_gworld[m_start_pic / 20];
							m_start_pic = m_start_pic % 20;
							source_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
							store_loc.x += k % width;
							store_loc.y += k / width;
							Draw_Some_Item(from_gworld, source_rect, ter_draw_gworld, store_loc, 1, 0);
							}
					}
			}
}

// Returns rect for drawing an item, if num < 25, rect is in big item template,
// otherwise in small item template
RECT get_item_template_rect (short type_wanted)
{
	RECT store_rect;

	if (type_wanted < 45) {
		store_rect.top = (type_wanted / 5) * BITMAP_HEIGHT;
		store_rect.bottom = store_rect.top + BITMAP_HEIGHT;
		store_rect.left = (type_wanted % 5) * BITMAP_WIDTH;
		store_rect.right = store_rect.left + BITMAP_WIDTH;
		}
		else {
			store_rect.top = (type_wanted / 10) * 18;
			store_rect.bottom = store_rect.top + 18;
			store_rect.left = (type_wanted % 10) * 18;
			store_rect.right = store_rect.left + 18;
			}

	return store_rect;
}

void draw_items(HDC hdc)
{
	short i;
	RECT source_rect,draw_rect;
	location where_draw;
	short pic_num;

	for (i = 0; i < 64; i++) {
		if (town.preset_items[i].item_code >= 0) {
			where_draw.x = town.preset_items[i].item_loc.x - cen_x + 4;
			where_draw.y = town.preset_items[i].item_loc.y - cen_y + 4;
			pic_num = scen_item_list.scen_items[town.preset_items[i].item_code].graphic_num;
			if ((where_draw.x >= 0) && (where_draw.x <= 8) &&
				(where_draw.y >= 0) && (where_draw.y <= 8))  {

					if (pic_num >= 150) {
						source_rect = get_custom_rect(pic_num - 150);
						draw_rect = calc_rect(where_draw.x,where_draw.y);
						OffsetRect(&draw_rect,8,8);
						rect_draw_some_item(spec_scen_g,source_rect, ter_draw_gworld, draw_rect, 1, 0);
  						}
						else {
							source_rect = get_item_template_rect(pic_num);
							draw_rect = calc_rect(where_draw.x,where_draw.y);
							OffsetRect(&draw_rect,8,8);
							if (pic_num >= 45) {
								draw_rect.top += 9;
								draw_rect.bottom -= 9;
								draw_rect.left += 5;
								draw_rect.right -= 5;
								}
							rect_draw_some_item((pic_num < 45) ? items_gworld : tiny_obj_gworld,source_rect, ter_draw_gworld, draw_rect, 1, 0);
							}
					}
			}
		}
}


void redraw_selected_ter()
{

}

void draw_one_terrain_spot (short i,short j,unsigned char terrain_to_draw)
{
	location where_draw;
	RECT source_rect;
	short picture_wanted;
	GWorldPtr source_gworld;
	short k;

	picture_wanted = scenario.ter_types[terrain_to_draw].picture;

	where_draw.x = (char) i;
	where_draw.y = (char) j;

	if ((picture_wanted >= 2000) && (spec_scen_g != NULL)) {
        source_gworld = spec_scen_g;
        source_rect = get_custom_rect(picture_wanted % 2000);
		source_rect.left += 28 * ((anim_ticks / 20) % 4);
		source_rect.right = source_rect.left + 28;
    }
    else if ((picture_wanted >= 1000) && (spec_scen_g != NULL)) {
		source_gworld = spec_scen_g;
		source_rect = get_custom_rect(picture_wanted % 1000);
		}
	else if (picture_wanted >= 400)	{
			source_gworld = anim_gworld;
			picture_wanted -= 400;
			source_rect.left = 112 * (picture_wanted / 5)+ 28 * ((anim_ticks / 20) % 4);
			source_rect.right = source_rect.left + 28;
			source_rect.top = 36 * (picture_wanted % 5);
			source_rect.bottom = source_rect.top + 36;
		}
		else {
			source_rect = get_template_rect(terrain_to_draw);
			source_gworld = terrain_gworld[picture_wanted / 50];
		}

	Draw_Some_Item(source_gworld, source_rect, ter_draw_gworld, where_draw, 0, 0);

    if(editing_town)
        for(k=0;k<30;k++){
            if ((cur_town == scenario.scen_horses[k].which_town) &&
							 (scenario.scen_horses[k].horse_loc.x == cen_x + i - 4) &&
							 (scenario.scen_horses[k].horse_loc.y == cen_y + j - 4)) {
							 				source_rect = horse_button_from;
											Draw_Some_Item(mixed_gworld,source_rect,ter_draw_gworld,where_draw,1,0);
                                        }
            if ((scenario.scen_boats[k].which_town == cur_town) &&
							 (scenario.scen_boats[k].boat_loc.x == cen_x + i - 4) &&
							 (scenario.scen_boats[k].boat_loc.y == cen_y + j - 4)) {
							 				source_rect = boat_button_from;
                                            Draw_Some_Item(mixed_gworld,source_rect,ter_draw_gworld,where_draw,1,0);
                                    }
                    }

}

void draw_one_tiny_terrain_spot (short i,short j,unsigned char terrain_to_draw,HDC hdc)
{

	location where_draw;
	RECT dest_rect = {0,0,5,5},from_rect = {0,0,4,4},orig_draw_rect = {0,0,4,4};
	short picture_wanted;
	GWorldPtr source_gworld;
	HBITMAP store_bmp;
	HBRUSH old_brush;

	picture_wanted = scenario.ter_types[terrain_to_draw].picture;

	where_draw.x = (char) i;
	where_draw.y = (char) j;
	OffsetRect(&dest_rect,8 + 4 * i,8 + 4 * j);

	store_bmp = (HBITMAP)SelectObject(hdc,ter_draw_gworld);
	old_brush = (HBRUSH) SelectObject(hdc,map_brush[0]);

	switch (picture_wanted) {

	case 0: case 1: case 73: case 72:
		SelectObject(hdc,map_brush[0]);
		Rectangle(hdc,dest_rect.left, dest_rect.top,dest_rect.right,dest_rect.bottom);
		break;
	case 2: case 3: case 4:
		SelectObject(hdc,map_brush[1]);
		Rectangle(hdc,dest_rect.left, dest_rect.top,dest_rect.right,dest_rect.bottom);
		break;

	default:
		if ((picture_wanted < 170) && (map_pats[picture_wanted] > 0)) {
			SelectObject(hdc,map_brush[map_pats[picture_wanted]]);
			Rectangle(hdc,dest_rect.left, dest_rect.top,dest_rect.right,dest_rect.bottom);
			}
		else if (picture_wanted >= 1000)	{
			SelectObject(hdc,store_bmp);
			from_rect = get_custom_rect(picture_wanted % 1000);
			rect_draw_some_item(spec_scen_g, from_rect, ter_draw_gworld, dest_rect, 0, 0);
			store_bmp = (HBITMAP)SelectObject(hdc,ter_draw_gworld);
			}

		else if (picture_wanted >= 400)	{
				source_gworld = anim_gworld;
				picture_wanted -= 400;
				if (picture_wanted == 0) {
			SelectObject(hdc,map_brush[13]);
			Rectangle(hdc,dest_rect.left, dest_rect.top,dest_rect.right,dest_rect.bottom);
			}
				else if (picture_wanted == 4) {
			SelectObject(hdc,map_brush[21]);
			Rectangle(hdc,dest_rect.left, dest_rect.top,dest_rect.right,dest_rect.bottom);
			}
				else if (picture_wanted == 7) {
			SelectObject(hdc,map_brush[20]);
			Rectangle(hdc,dest_rect.left, dest_rect.top,dest_rect.right,dest_rect.bottom);
			}
				else if (picture_wanted == 8) {
			SelectObject(hdc,map_brush[19]);
			Rectangle(hdc,dest_rect.left, dest_rect.top,dest_rect.right,dest_rect.bottom);
			}
				else if (picture_wanted == 9) {
			SelectObject(hdc,map_brush[20]);
			Rectangle(hdc,dest_rect.left, dest_rect.top,dest_rect.right,dest_rect.bottom);
			}
				else if (picture_wanted == 10) {
			SelectObject(hdc,map_brush[19]);
			Rectangle(hdc,dest_rect.left, dest_rect.top,dest_rect.right,dest_rect.bottom);
			}
				else {
					from_rect = orig_draw_rect;
					OffsetRect(&from_rect,
									312 + 24 * (picture_wanted / 5) + 1,
									163 + 6 * (picture_wanted % 5) + 1);
		 			SelectObject(hdc,store_bmp);
					dest_rect.right--; dest_rect.bottom--;
					rect_draw_some_item(mixed_gworld, from_rect, ter_draw_gworld, dest_rect, 0, 0);
					store_bmp = (HBITMAP)SelectObject(hdc,ter_draw_gworld);
					}
			}
			else {

					from_rect = orig_draw_rect;
				OffsetRect(&from_rect,
					6 * (picture_wanted % 10) + 312 + 1,1 + 6 * (picture_wanted / 10));

				SelectObject(hdc,store_bmp);
				dest_rect.right--; dest_rect.bottom--;
				rect_draw_some_item(mixed_gworld, from_rect, ter_draw_gworld, dest_rect, 0, 0);
				store_bmp = (HBITMAP)SelectObject(hdc,ter_draw_gworld);
			}
		break;
		}
	SelectObject(hdc,store_bmp);
	SelectObject(hdc,old_brush);
}

/* Draw a bitmap in the world window. hor in 0 .. 8, vert in 0 .. 8,
	object is ptr. to bitmap to be drawn, and masking is for Copybits. */
void Draw_Some_Item ( GWorldPtr src_gworld,RECT src_rect,GWorldPtr targ_gworld,location target,
char masked,short main_win)
{
RECT	destrec;

	if ((target.x < 0) || (target.x > 8) || (target.y < 0) || (target.y > 8))
		return;
	destrec.left = 8 + BITMAP_WIDTH * target.x;
	destrec.right = destrec.left + BITMAP_WIDTH;
	destrec.top = 8 + BITMAP_HEIGHT * target.y;
	destrec.bottom = destrec.top + BITMAP_HEIGHT;

	destrec.left = destrec.right - (src_rect.right - src_rect.left);
	destrec.top = destrec.bottom - (src_rect.bottom - src_rect.top);

	rect_draw_some_item(src_gworld,src_rect,ter_draw_gworld,destrec,masked,0);

}


void discard_graphics()
{
	short i;

	DeleteObject(terrain_buttons_gworld);
	DeleteObject(ter_draw_gworld);
	DeleteObject(dlg_buttons_gworld);
	for (i = 0; i < 10; i++)
	DeleteObject(monst_gworld[i]);
	for (i = 0; i < 6; i++)
	DeleteObject(terrain_gworld[i]);
	DeleteObject(small_terrain_gworld);
	DeleteObject(editor_mixed);
	DeleteObject(anim_gworld);
	DeleteObject(talkfaces_gworld);
	DeleteObject(field_gworld);
	DeleteObject(tiny_obj_gworld);
	DeleteObject(items_gworld);
	DeleteObject(dlogpics_gworld);
	DeleteObject(mixed_gworld);
	DeleteObject(pattern_gworld);
	DeleteObject(dialog_pattern_gworld);
	if (spec_scen_g != NULL)
		DeleteObject(spec_scen_g);

	DeleteDC(main_dc);

	DeleteObject(font);
//	DeleteObject(small_bold_font);
	DeleteObject(underline_font);
	DeleteObject(italic_font);
	DeleteObject(bold_font);

	for (i = 0; i < 8; i++)
		if (cursors[i] != NULL)
			DeleteObject(cursors[i]);
	for (i = 0; i < 25; i++) {
		DeleteObject(map_bitmap[i]);
		DeleteObject(map_brush[i]);
		}
}

/* Input terrain currently trying to draw. Get back RECT in terrain template containing
desired pixmap, or RECT to darkness if desired map not present */
RECT get_template_rect (unsigned char type_wanted)
{
	RECT store_rect;
	short picture_wanted;

	picture_wanted = scenario.ter_types[type_wanted].picture;
	if (picture_wanted >= 1000)
		picture_wanted = 0;
	picture_wanted = picture_wanted % 50;
	store_rect.top = 0 + (picture_wanted / 10) * BITMAP_HEIGHT;
	store_rect.bottom = store_rect.top + BITMAP_HEIGHT;
	store_rect.left = 0 + (picture_wanted % 10) * BITMAP_WIDTH;
	store_rect.right = store_rect.left + BITMAP_WIDTH;

	return store_rect;
}


void place_location()
{
	char draw_str[256];
	RECT draw_rect,source_rect,erase_rect;
	short picture_wanted;
	HBITMAP store_bmp;
	HDC hdc;

	erase_rect.left = terrain_rects[255].left + 17;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].top + 12 - 10;
	erase_rect.bottom = erase_rect.top + 12;
	paint_pattern(terrain_buttons_gworld,0,erase_rect,1);
	erase_rect.left = 2;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].bottom + 117;
	erase_rect.bottom = RIGHT_AREA_HEIGHT + 6;
	paint_pattern(terrain_buttons_gworld,0,erase_rect,1);

	hdc = CreateCompatibleDC(main_dc);

	SetBkMode(hdc,TRANSPARENT);
	SelectObject(hdc,small_bold_font);
	store_bmp = (HBITMAP)SelectObject(hdc,terrain_buttons_gworld);

	draw_rect.left = terrain_rects[255].left + 20;
	draw_rect.top = terrain_rects[255].top;
	if (overall_mode < 60)
		sprintf( draw_str,"Center: x = %d, y = %d  ", cen_x, cen_y);
		else {
			draw_rect.left = 5;
			draw_rect.top = terrain_rects[255].top + 28;
			sprintf(draw_str,"Click terrain to edit. ");
			}

	draw_rect.bottom = draw_rect.top + 14;
	draw_rect.right = draw_rect.left + 200;
	win_draw_string(hdc,draw_rect,draw_str,0,12);
	SelectObject(hdc,store_bmp);


	store_bmp = (HBITMAP)SelectObject(hdc,terrain_buttons_gworld);
	if (overall_mode < 60) {
		draw_rect.left = 5;
		draw_rect.top = terrain_rects[255].bottom + 119;
		draw_rect.bottom = draw_rect.top + 14;
		draw_rect.right = draw_rect.left + 400;
		win_draw_string(hdc,draw_rect,current_string,0,12)     ;

		draw_rect.left = RIGHT_AREA_WIDTH / 2;
		draw_rect.top = terrain_rects[255].bottom + 119;
		draw_rect.bottom = draw_rect.top + 14;
		draw_rect.right = draw_rect.left + 200;
		win_draw_string(hdc,draw_rect,current_string2,0,12) ;
		}

	SelectObject(hdc,store_bmp);
	DeleteObject(hdc);

	draw_rect.top = palette_buttons[0][0].top + terrain_rects[255].bottom + 5;
	draw_rect.left = palette_buttons[7][0].right + 17;
	draw_rect.bottom = draw_rect.top + 36;
	draw_rect.right = draw_rect.left + 28;
	picture_wanted = scenario.ter_types[current_terrain_type].picture;

	if (overall_mode < 60) {
		if (picture_wanted >= 1000)	{
			source_rect = get_custom_rect(picture_wanted % 1000);
			rect_draw_some_item(spec_scen_g,source_rect,terrain_buttons_gworld,draw_rect,0,0);
 			}
		else if (picture_wanted >= 400)	{
			picture_wanted -= 400;
			source_rect.left = 112 * (picture_wanted / 5);
			source_rect.right = source_rect.left + 28;
			source_rect.top = 36 * (picture_wanted % 5);
			source_rect.bottom = source_rect.top + 36;
			rect_draw_some_item(anim_gworld,source_rect,terrain_buttons_gworld,draw_rect,0,0);
			}
		else {
			source_rect = get_template_rect(current_terrain_type);
			rect_draw_some_item(terrain_gworld[picture_wanted / 50],source_rect,
			terrain_buttons_gworld,draw_rect,0,0);
			}
		}

	draw_rect = terrain_buttons_rect;
	OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld,terrain_buttons_rect,
		terrain_buttons_gworld,draw_rect,0,1);
}

// klugde for speed ...exactly like place location above, but just writes location
void place_just_location()
{
	char draw_str[256];
	RECT from_rect,draw_rect,erase_rect;
	HBITMAP store_bmp;
	HDC hdc;

	erase_rect.left = terrain_rects[255].left + 17;
	erase_rect.right = RIGHT_AREA_WIDTH - 1;
	erase_rect.top = terrain_rects[255].top + 12 - 9;
	erase_rect.bottom = erase_rect.top + 12;
	paint_pattern(terrain_buttons_gworld,0,erase_rect,1);

	hdc = CreateCompatibleDC(main_dc);

	SetBkMode(hdc,TRANSPARENT);
	SelectObject(hdc,small_bold_font);
	store_bmp = (HBITMAP)SelectObject(hdc,terrain_buttons_gworld);

	draw_rect.left = terrain_rects[255].left + 20;
	draw_rect.top = terrain_rects[255].top;
	if (overall_mode < 60)
		sprintf(draw_str,"Center: x = %d, y = %d  ",cen_x,cen_y);
		else {
			draw_rect.left = 5;
			draw_rect.top = terrain_rects[255].top + 28;
			sprintf(draw_str,"Click terrain to edit. ");
			}

	draw_rect.bottom = draw_rect.top + 14;
	draw_rect.right = draw_rect.left + 200;
	win_draw_string(hdc,draw_rect,draw_str,0,12);
	SelectObject(hdc,store_bmp);


	from_rect = terrain_buttons_rect;
	from_rect.top = erase_rect.top;
	from_rect.bottom = erase_rect.bottom;
	draw_rect = from_rect;
	OffsetRect(&draw_rect,RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	rect_draw_some_item(terrain_buttons_gworld,from_rect,
		terrain_buttons_gworld,draw_rect,0,1);
	DeleteObject(hdc);
}

void set_string(char *string,char *string2)
{
	strcpy((char *)current_string,string);
	strcpy((char *)current_string2,string2);
	place_location();
}

Boolean is_special(short i,short j)
{
	short k;

	if (editing_town == TRUE)
		for (k = 0; k < 50; k++)
			if ((town.special_locs[k].x == i) && (town.special_locs[k].y == j))
				return TRUE;
	if (editing_town == FALSE)
		for (k = 0; k < 18; k++)
			if ((current_terrain.special_locs[k].x == i) && (current_terrain.special_locs[k].y == j))
				return TRUE;

	return FALSE;
}

Boolean is_field_type(short i,short j,short field_type)
{
	short k;

	for (k = 0; k < 50; k++)
		if ((town.preset_fields[k].field_type == field_type) &&
			(town.preset_fields[k].field_loc.x == i) &&
			(town.preset_fields[k].field_loc.y == j))
				return TRUE;
	return FALSE;
}

void make_field_type(short i,short j,short field_type)
{
	short k;

	if (is_field_type(i,j,field_type) == TRUE)
		return;
	for (k = 0; k < 50; k++)
		if (town.preset_fields[k].field_type == 0) {
			town.preset_fields[k].field_loc.x = i;
			town.preset_fields[k].field_loc.y = j;
			town.preset_fields[k].field_type = field_type;
			return;
			}
	give_error("Each town can have at most 50 fields and special effects (webs, barrels, blood stains, etc.). To place more, use the eraser first.","",0);
}


void take_field_type(short i,short j,short field_type)
{
	short k;

	for (k = 0; k < 50; k++)
		if ((town.preset_fields[k].field_type == field_type) &&
			(town.preset_fields[k].field_loc.x == i) &&
			(town.preset_fields[k].field_loc.y == j)) {
				town.preset_fields[k].field_type = 0;
				return;
				}
}

Boolean is_web(short i,short j)
{
	return is_field_type(i,j,3);

}
void make_web(short i,short j)
{
	make_field_type(i,j,3);
}
void take_web(short i,short j)
{
	take_field_type(i,j,3);
}

Boolean is_crate(short i,short j)
{
	return is_field_type(i,j,4);
}
void make_crate(short i,short j)
{
	make_field_type(i,j,4);

}
void take_crate(short i,short j)
{
	take_field_type(i,j,4);
}

Boolean is_barrel(short i,short j)
{
	return is_field_type(i,j,5);
}
void make_barrel(short i,short j)
{
	make_field_type(i,j,5);
}
void take_barrel(short i,short j)
{
	take_field_type(i,j,5);
}

Boolean is_fire_barrier(short i,short j)
{
	return is_field_type(i,j,6);
}
void make_fire_barrier(short i,short j)
{
	make_field_type(i,j,6);
}
void take_fire_barrier(short i,short j)
{
	take_field_type(i,j,6);
}

Boolean is_force_barrier(short i,short j)
{
	return is_field_type(i,j,7);
}
void make_force_barrier(short i,short j)
{
	make_field_type(i,j,7);
}
void take_force_barrier(short i,short j)
{
	take_field_type(i,j,7);

}

Boolean is_sfx(short i,short j,short type)
{
	return is_field_type(i,j,type + 14);
}
void make_sfx(short i,short j,short type)
{
	make_field_type(i,j,type + 14);
}
void take_sfx(short i,short j,short type)
{
	take_field_type(i,j,type + 14);
}


Boolean is_quickfire(short i,short j)
{
	return is_field_type(i,j,8);
}
void make_quickfire(short i,short j)
{
	make_field_type(i,j,8);
}
void take_quickfire(short i,short j)
{
	take_field_type(i,j,8);
}

Boolean container_there(location l)
{

	if (editing_town == FALSE)
		return FALSE;
	if (scenario.ter_types[t_d.terrain[l.x][l.y]].special == 14)
		return TRUE;
	if (is_barrel(l.x,l.y) == TRUE)
		return TRUE;
	if (is_crate(l.x,l.y) == TRUE)
		return TRUE;
	return 0;
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
void win_draw_string(HDC dest_hdc,RECT dest_rect,char *str,short mode,short line_height)
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
	//text_len = len_array;
	for (i = 0; i < 256; i++)
		text_len[i] = 0;
	for (i = 1; i < str_len; i++) {
		strncpy(p_str,str,i);
		p_str[i] = 0;
		GetTextExtentPoint32(hdc,p_str,i,&val_returned);
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

void get_str(char *str,short i, short j)
{
	if (i == -1) {
		strcpy((char *) str,scen_item_list.monst_names[j]);
		return;
		}
	if (i == -2) {
		strcpy((char *) str,scen_item_list.scen_items[j].full_name);
		return;
		}
	if (i == -3) {
		strcpy((char *) str,button_strs[available_dlog_buttons[j]]);
		return;
		}
	if (i == -4) {
		strcpy((char *) str,scen_item_list.ter_names[j]);
		return;
		}
	if (i == -5) {
		get_str(str,40,j * 7 + 1);
		return;
		}
	GetIndString(str, i, j);
}


void undo_clip(HDC hdc)
{
 //	RECT overall_rect = {0,0,530,435};
	HRGN rgn;

	rgn = CreateRectRgn(0,0,5000,5000);
	SelectClipRgn(hdc,rgn);
	DeleteObject(rgn);
}

void ClipRect(HDC hdc,RECT *rect)
{
	HRGN rgn;
	RECT rect2;

	rect2 = *rect;
//	OffsetRect(&rect2,ulx,uly);

	rgn = CreateRectRgn(rect2.left,rect2.top,rect2.right,rect2.bottom);
	SelectClipRgn(hdc,rgn);
	DeleteObject(rgn);
}

void FlushEvents(short mode)
// mode... 0 - keystrokes   1 - mouse presses  2 - both
{
	MSG msg;

	if ((mode == 0) || (mode == 2)) {
		while ((PeekMessage(&msg, mainPtr, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE)) != 0)
			;
		}
	if ((mode == 1) || (mode == 2)) {
		while ((PeekMessage(&msg, mainPtr, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE)) != 0)
			;
		}

}

RECT get_custom_rect (short which_rect) ////
{
	RECT store_rect = {0,0,28,36};

	OffsetRect(&store_rect,28 * (which_rect % 10),36 * (which_rect / 10));
	return store_rect;
}
