#include <Windows.h>

#include <cstring>
#include <cstdio>
#include "global.h"
#include "boe.graphics.h"
#include "boe.monster.h"
#include "tools/dlogtool.h"
#include "boe.newgraph.h"
#include "boe.fileio.h"
#include "boe.itemdata.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "boe.text.h"
#include "tools/soundtool.h"
#include "tools/mathutil.h"
#include "boe.graphutil.h"

#include "globvar.h"

// Missile anim vars
typedef struct {
	location dest;
	short missile_type; // -1 no miss
	short path_type,x_adj,y_adj;
	} store_missile_type;
	
typedef struct {
	location dest;
	short val_to_place,offset;
	short place_type; // 0 - on spot, 1 - random area near spot
	short boom_type;  // -1 no miss
	short x_adj,y_adj;
	} store_boom_type;
	
store_missile_type store_missiles[30];
store_boom_type store_booms[30];
Boolean have_missile,have_boom;
RECT explode_place_rect[30];

char last_light_mask[13][13];

void MoveTo(short x, short y)
{
	store_text_x = x;
	store_text_y = y - 16;
}

void apply_unseen_mask()
{
	RECT base_rect = {8,10,43,53}/*{9,9,43,53}*/,to_rect,big_to = {13,13,265,337}; /**/
	short i,j,k,l;
	Boolean need_bother = FALSE;
	HDC hdc;
	HBITMAP store_bmp;
	HBRUSH old_brush;
	HPEN old_pen;

	if (PSD[306][2] > 0)
		return;
	if ((is_combat()) && (which_combat_type == 0))
		return;
	if (!(is_out()) && (c_town.town.lighting > 0))
		return;
		
	for (i = 0; i < 11; i++)
		for (j = 0; j < 11; j++) 
			if (unexplored_area[i + 1][j + 1] == 1)
				need_bother = TRUE;
	if (need_bother == FALSE)
		return;
				
	hdc = CreateCompatibleDC(main_dc);
	SetBkMode(hdc,TRANSPARENT);
	old_brush = (HBRUSH) SelectObject(hdc,checker_brush);
	old_pen = (HPEN) SelectObject(hdc,GetStockObject(NULL_PEN));
   SetROP2(hdc,R2_MASKPEN);

	store_bmp = (HBITMAP) SelectObject(hdc,terrain_screen_gworld);

	for (i = 0; i < 11; i++)
		for (j = 0; j < 11; j++) 
			if (unexplored_area[i + 1][j + 1] == 1) {

					to_rect = base_rect;
					OffsetRect(&to_rect,-28 + i * 28 + 2,-36 + 36 * j);
					//SectRect(&to_rect,&big_to,&to_rect);
					IntersectRect(&to_rect,&to_rect,&big_to);
					Rectangle(hdc, to_rect.left,to_rect.top,to_rect.right,to_rect.bottom);
					//PaintRect(&to_rect);
					for (k = i - 2; k < i + 1; k++)
						for (l = j - 2; l < j + 1; l++)
							if ((k >= 0) && (l >= 0) && (k < 9) && (l < 9) && ((k != i - 1) || (l != j - 1)))
								terrain_there[k][l] = -1;
				}
	
	// Now put text on window.
	SelectObject(hdc,old_brush);
	SelectObject(hdc,old_pen);
	SelectObject(hdc,store_bmp);
	if (!DeleteDC(hdc)) DebugQuit("Cannot release DC 3");
}
 
void apply_light_mask() 
{
	RECT temp = {0,0,84,108},paint_rect,base_rect = {0,0,28,36};
	RECT big_to = {13,13,265,337}; /**/
	short i,j;
	Boolean is_dark = FALSE,same_mask = TRUE;
	
	HDC hdc;
	HBITMAP store_bmp;
	HBRUSH old_brush;
	
	if (PSD[306][2] > 0)
		return;
	if (is_out())
		return;
	if (c_town.town.lighting == 0)
		return;
	
	if (oval_region == NULL) {
		temp_rect_rgn = CreateRectRgn(0,0,0,0);
		dark_mask_region = CreateRectRgn(0,0,0,0);
		oval_region = CreateEllipticRgnIndirect(&temp);
		}
	
	// Process the light array
	for (i = 2; i < 11; i++)
		for (j = 2; j < 11; j++) 
			if (light_area[i][j] == 0) is_dark = TRUE;
	if (is_dark == FALSE) { 
		for (i = 2; i < 11; i++)
			for (j = 2; j < 11; j++) 
				last_light_mask[i][j] = 0;
		return;
		}
	for (i = 1; i < 12; i++)
		for (j = 1; j < 12; j++)
			if ((light_area[i - 1][j - 1] >= 1) && (light_area[i + 1][j - 1] >= 1) && 
				(light_area[i - 1][j] >= 1) && (light_area[i + 1][j] >= 1) && 
				(light_area[i - 1][j + 1] >= 1) && (light_area[i + 1][j + 1] >= 1) && 
				(light_area[i][j - 1] >= 1) && (light_area[i][j + 1] >= 1)) {
					light_area[i][j] = 2;
					}
	for (i = 1; i < 12; i++)
		for (j = 1; j < 12; j++)
			if ((light_area[i - 1][j - 1] >= 2) && (light_area[i + 1][j - 1] >= 2) && 
				(light_area[i - 1][j] >= 2) && (light_area[i + 1][j] >= 2) && 
				(light_area[i - 1][j + 1] >= 2) && (light_area[i + 1][j + 1] >= 2) && 
				(light_area[i][j - 1] >= 2) && (light_area[i][j + 1] >= 2)) {
					light_area[i][j] = 3;
					}
		
	hdc = CreateCompatibleDC(main_dc);
	SetBkMode(hdc,TRANSPARENT);
	store_bmp = (HBITMAP) SelectObject(hdc,terrain_screen_gworld);
	old_brush = (HBRUSH) SelectObject(hdc,GetStockObject(BLACK_BRUSH));

	for (i = 2; i < 11; i++)
		for (j = 2; j < 11; j++) {
			if (light_area[i][j] == 1)
				terrain_there[i - 2][j - 2] = -1;
			}
	for (i = 0; i < 13; i++)
		for (j = 0; j < 13; j++) 
			if (last_light_mask[i][j] != light_area[i][j])
				same_mask = FALSE;
	
	if (same_mask == TRUE) {
		PaintRgn(hdc,dark_mask_region);
		SelectObject(hdc,store_bmp);
		if (!DeleteDC(hdc)) DebugQuit("Cannot release DC 2");
		return;
		}
	SetRectRgn(dark_mask_region,big_to.left,big_to.top,big_to.right,big_to.bottom);
	for (i = 0; i < 13; i++)
		for (j = 0; j < 13; j++) 
			last_light_mask[i][j] = light_area[i][j];
	for (i = 1; i < 12; i++)
		for (j = 1; j < 12; j++) {
			if (light_area[i][j] == 2) {
				
				OffsetRgn(oval_region,13 + 28 * (i - 3), 13 + 36 * (j - 3));
				
				CombineRgn(dark_mask_region,dark_mask_region,oval_region,RGN_DIFF);
				
				OffsetRgn(oval_region,-13 + -1 * (28 * (i - 3)),-13 + -1 * (36 * (j - 3)));
				}
			if (light_area[i][j] == 3) {
				paint_rect = base_rect;
				OffsetRect(&paint_rect,13 + 28 * (i - 2),13 + 36 * (j - 2));
				SetRectRgn(temp_rect_rgn,paint_rect.left,paint_rect.top,
					paint_rect.right + 28,paint_rect.bottom + 36);
				CombineRgn(dark_mask_region,dark_mask_region,temp_rect_rgn,RGN_DIFF);
				if (light_area[i + 1][j] == 3) light_area[i + 1][j] = 0;
				if (light_area[i + 1][j + 1] == 3) light_area[i + 1][j + 1] = 0;
				if (light_area[i][j + 1] == 3) light_area[i][j + 1] = 0;
				}
			}

	PaintRgn(hdc,dark_mask_region);
	SelectObject(hdc,store_bmp);
	DeleteObject(hdc);
}



void start_missile_anim()
{
	short i;
	
	if (boom_anim_active == TRUE)
		return;
	boom_anim_active = TRUE;
	for (i = 0; i < 30; i++) {
		store_missiles[i].missile_type = -1;
		store_booms[i].boom_type = -1;
		}
	for (i = 0; i < 6; i++) 
		pc_marked_damage[i] = 0;
	for (i = 0; i < T_M; i++) 
		monst_marked_damage[i] = 0;
	have_missile = FALSE;
	have_boom = FALSE;
}

void end_missile_anim() 
{
	boom_anim_active = FALSE;
}
	
void add_missile(location dest,short missile_type,short path_type,short x_adj,short y_adj)
{
	short i;
	
	if (boom_anim_active == FALSE)
		return;
	if (PSD[306][2] > 0)
		return;
	// lose redundant missiles
	for (i = 0; i < 30; i++)
		if ((store_missiles[i].missile_type >= 0) && (same_point(dest,store_missiles[i].dest) == TRUE))
			return;
	for (i = 0; i < 30; i++)
		if (store_missiles[i].missile_type < 0) {
			have_missile = TRUE;
			store_missiles[i].dest = dest;
			store_missiles[i].missile_type =missile_type;
			store_missiles[i].path_type =path_type;
			store_missiles[i].x_adj =x_adj;
			store_missiles[i].y_adj =y_adj;			
			return;
			}
}

void run_a_missile(location from,location fire_to,short miss_type,short path,short sound_num,short x_adj,short y_adj,short len)
{
	start_missile_anim();
	add_missile(fire_to,miss_type,path, x_adj, y_adj);
	do_missile_anim(len,from, sound_num);
	end_missile_anim();
}

void run_a_boom(location boom_where,short type,short x_adj,short y_adj)
{
	if ((type < 0) || (type > 2))
		return;
	start_missile_anim();
	add_explosion(boom_where,-1,0,type, x_adj, y_adj);
	do_explosion_anim(5,0);
	end_missile_anim();
}
	
void mondo_boom(location l,short type)
{
	short i;
	
	start_missile_anim();
	for (i = 0; i < 12; i++)
		add_explosion(l,-1,1,type,0,0);
	do_explosion_anim(5,0);

	end_missile_anim();	
}
		
void add_explosion(location dest,short val_to_place,short place_type,short boom_type,short x_adj,short y_adj)
{
	short i;
	
	if (PSD[306][2] > 0)
		return;
	if (boom_anim_active == FALSE)
		return;
	// lose redundant explosions
	for (i = 0; i < 30; i++)
		if ((store_booms[i].boom_type >= 0) && (same_point(dest,store_booms[i].dest) == TRUE)
		 && (place_type == 0)) {
			if (val_to_place > 0)
				store_booms[i].val_to_place = val_to_place;
			return;
			}
	for (i = 0; i < 30; i++)
		if (store_booms[i].boom_type < 0) {
			have_boom = TRUE;
			store_booms[i].offset = (i == 0) ? 0 : -1 * get_ran(1,0,2);
			store_booms[i].dest = dest;
			store_booms[i].val_to_place = val_to_place;
			store_booms[i].place_type = place_type;
			store_booms[i].boom_type =  boom_type;
			store_booms[i].x_adj =x_adj;
			store_booms[i].y_adj =y_adj;			
			return;
			}
}		

void do_missile_anim(short num_steps,location missile_origin,short sound_num) 
{
	RECT temp_rect,missile_origin_base = {1,1,17,17},active_area_rect,to_rect,from_rect;
	short i,store_missile_dir;
	POINT start_point,finish_point[30];
	location screen_ul;
	
	short x1[30],x2[30],y1[30],y2[30],t; // for path paramaterization
	RECT missile_place_rect[30],missile_origin_rect[30],store_erase_rect[30];
	POINT current_terrain_ul; 
	HBITMAP temp_gworld;
	long delay_dummy;
	RECT ter_scrn_rect = {0,0,279,351};
	long t1,t2;
	long pause_len = 0;

	t2 = t1 = (long) GetCurrentTime();
	if (sound_num == 11) pause_len = 660;
	if (sound_num == 12) pause_len = 410;
	if (sound_num == 14) pause_len = 200;
	if (sound_num == 53) pause_len = 1000;
	if (sound_num == 64) pause_len = 500;

	if ((have_missile == FALSE) || (boom_anim_active == FALSE)) {
		boom_anim_active = FALSE;
		return;
		}
	
	for (i = 0; i < 30; i++)
		if (store_missiles[i].missile_type >= 0)
			i = 50;
	if (i == 30)
		return;

	// initialize general data

	current_terrain_ul.x = current_terrain_ul.y = 5;

	
	// make terrain_template contain current terrain all nicely
	draw_terrain(1);
	to_rect = ter_scrn_rect;
	OffsetRect(&to_rect,current_terrain_ul.x, current_terrain_ul.y);
	rect_draw_some_item(terrain_screen_gworld,ter_scrn_rect,
		terrain_screen_gworld,to_rect,0,1);
							
	// create and clip temporary anim template 
	temp_rect = ter_scrn_rect;
	temp_gworld = CreateCompatibleBitmap(main_dc,ter_scrn_rect.right,ter_scrn_rect.bottom);
	active_area_rect = temp_rect;
	InflateRect(&active_area_rect,-13,-13);

	// init missile paths
	for (i = 0; i < 30; i++) {
		SetRectEmpty(&store_erase_rect[i]);
		if ((store_missiles[i].missile_type >= 0) && (same_point(missile_origin,store_missiles[i].dest) == TRUE))
			store_missiles[i].missile_type = -1;
		}
	screen_ul.x = center.x - 4; screen_ul.y = center.y - 4;
	start_point.x = 13 + 14 + 28 * (short) (missile_origin.x - screen_ul.x);
	start_point.y = 13 + 18 + 36 * (short) (missile_origin.y - screen_ul.y);
	for (i = 0; i < 30; i++)
		if (store_missiles[i].missile_type >= 0) {
			finish_point[i].x = 1 + 13 + 14 + store_missiles[i].x_adj + 28 * (short) (store_missiles[i].dest.x - screen_ul.x);
			finish_point[i].y = 1 + 13 + 18 + store_missiles[i].y_adj + 36 * (short) (store_missiles[i].dest.y - screen_ul.y);
			// note ... +1 at beginning is put in to prevent infinite slope

			if (store_missiles[i].missile_type < 7) {
				store_missile_dir = get_missile_direction(start_point,finish_point[i]);
				missile_origin_rect[i] = missile_origin_base;
				OffsetRect(&missile_origin_rect[i],18 * store_missile_dir,18 * store_missiles[i].missile_type);
				}
				else {
					missile_origin_rect[i] = missile_origin_base;
					OffsetRect(&missile_origin_rect[i],0,18 * store_missiles[i].missile_type);
					}

			// x1 slope x2 start pt
			x1[i] = finish_point[i].x - start_point.x;
			x2[i] = start_point.x;
			y1[i] = finish_point[i].y - start_point.y;
			y2[i] = start_point.y;
			}
			else missile_place_rect[i].top =missile_place_rect[i].left =missile_place_rect[i].bottom =missile_place_rect[i].right = 0;

	play_sound(-1 * sound_num);


	// Now, at last, launch missile
	for (t = 0; t < num_steps; t++) {
		for (i = 0; i < 30; i++)
			if (store_missiles[i].missile_type >= 0) {
				// Where place?
				temp_rect = missile_origin_base;
				OffsetRect(&temp_rect,-8 + x2[i] + (x1[i] * t) / num_steps,
					-8 + y2[i] + (y1[i] * t) / num_steps);

				// now adjust for different paths
				if (store_missiles[i].path_type == 1)
				OffsetRect(&temp_rect,0,
					-1 * (t * (num_steps - t)) / 100);

				//SectRect(&temp_rect,&active_area_rect,&missile_place_rect[i]);
				IntersectRect(&missile_place_rect[i],&temp_rect,&active_area_rect);

				// Now put terrain in temporary;
				rect_draw_some_item(terrain_screen_gworld,missile_place_rect[i],
					temp_gworld,missile_place_rect[i],0,0);
				// Now put in missile
				from_rect = missile_origin_rect[i];
				if (store_missiles[i].missile_type >= 7)
					OffsetRect(&from_rect,18 * (t % 8),0);
				rect_draw_some_item(missiles_gworld,from_rect,
					temp_gworld,temp_rect,1,0);
				}

		// Now draw all missiles to screen
		for (i = 0; i < 30; i++)
			if (store_missiles[i].missile_type >= 0) {
				to_rect = store_erase_rect[i];
				OffsetRect(&to_rect,current_terrain_ul.x,current_terrain_ul.y);
				rect_draw_some_item(terrain_screen_gworld,store_erase_rect[i],
					terrain_screen_gworld,to_rect,0,1);

				to_rect = missile_place_rect[i];
				store_erase_rect[i] = to_rect;
				OffsetRect(&to_rect,current_terrain_ul.x,current_terrain_ul.y);
				rect_draw_some_item(temp_gworld,missile_place_rect[i],
					temp_gworld,to_rect,0,1);
				}
		if ((PSD[306][6] == 3) || ((PSD[306][6] == 1) && (t % 4 == 0)) || ((PSD[306][6] == 2) && (t % 3 == 0)))
			Delay(1,&delay_dummy);

		}

	// Exit gracefully, and clean up screen
	for (i = 0; i < 30; i++)
		store_missiles[i].missile_type = -1;
	DeleteObject(temp_gworld);

	to_rect = ter_scrn_rect;
	OffsetRect(&to_rect,current_terrain_ul.x,current_terrain_ul.y);
	rect_draw_some_item(terrain_screen_gworld,ter_scrn_rect,
		terrain_screen_gworld,to_rect,0,1);

		while (t2 - t1 < pause_len + 40) {
			t2 = (long)GetCurrentTime();
			}
		play_sound(99);
}

short get_missile_direction(POINT origin_point,POINT the_point)
{
	location store_dir;
	short dir = 0;
	// To reuse legacy code, will renormalize the_point, which is missile destination,
	// so that origin_point is moved to (149,185) and the_point is moved in proportion
	the_point.x +=  149 - origin_point.x;
	the_point.y +=  185 - origin_point.y;
	
				if ((the_point.x < 135) & (the_point.y >= ((the_point.x * 34) / 10) - 293)
					& (the_point.y <= (-1 * ((the_point.x * 34) / 10) + 663)))
					store_dir.x--;
				if ((the_point.x > 163) & (the_point.y <= ((the_point.x * 34) / 10) - 350)
					& (the_point.y >= (-1 * ((the_point.x * 34) / 10) + 721)))
					store_dir.x++;
					
				if ((the_point.y < 167) & (the_point.y <= (the_point.x / 2) + 102)
					& (the_point.y <= (-1 * (the_point.x / 2) + 249)))
					store_dir.y--;
				if ((the_point.y > 203) & (the_point.y >= (the_point.x / 2) + 123)
					& (the_point.y >= (-1 * (the_point.x / 2) + 268)))
					store_dir.y++;
			
	switch (store_dir.y) {
		case 0:
			dir = 4 - 2 * (store_dir.x); break;
		case -1:
			dir = (store_dir.x == -1) ? 7 : store_dir.x; break;
		case 1:
			dir = 4 - store_dir.x; break;
		}	
	return dir;
}

void do_explosion_anim(short, short special_draw)
// sound_num currently ignored
// special_draw - 0 normal 1 - first half 2 - second half
{
	RECT temp_rect,to_rect,from_rect;
	RECT base_rect = {0,0,28,36},text_rect;
	char str[256];
	short i,temp_val,temp_val2;

	location screen_ul;
	
	short t,cur_boom_type = 0; 
	POINT current_terrain_ul; 
	HBITMAP temp_gworld;
	long delay_dummy;
	short boom_type_sound[3] = {5,10,53};
	RECT ter_scrn_rect = {0,0,279,351};
	HDC hdc;
	COLORREF colors[5] = {RGB(0,0,0),RGB(255,0,0),RGB(128,0,0),RGB(0,160,0),RGB(255,255,255)};
	HBITMAP store_bmp;
	long t1,t2;
	long snd_len[3] = {1500,1410,1100};

	t2 = t1 = (long) GetCurrentTime();

	if ((have_boom == FALSE) || (boom_anim_active == FALSE)) {
		boom_anim_active = FALSE;
		return;
		}
	
	for (i = 0; i < 30; i++)
		if (store_booms[i].boom_type >= 0)
			i = 50;
	if (i == 30)
		return;

	// initialize general data
	current_terrain_ul.x = current_terrain_ul.y = 5;

	
	// make terrain_template contain current terrain all nicely
	draw_terrain(1);
	if (special_draw != 2) {
		to_rect = ter_scrn_rect;
		OffsetRect(&to_rect,current_terrain_ul.x, current_terrain_ul.y);
		rect_draw_some_item(terrain_screen_gworld,ter_scrn_rect,
			terrain_screen_gworld,to_rect,0,1);
		}
		
				
	// create and clip temporary anim template 
	temp_rect = ter_scrn_rect;
	temp_gworld = CreateCompatibleBitmap(main_dc,ter_scrn_rect.right,ter_scrn_rect.bottom);

	hdc = CreateCompatibleDC(main_dc);
	SetBkMode(hdc,TRANSPARENT);
	SelectObject(hdc,small_bold_font);
	SetTextColor(hdc,colors[4]);
		
	// init missile paths
	screen_ul.x = center.x - 4; screen_ul.y = center.y - 4;
	for (i = 0; i < 30; i++) 
		if ((store_booms[i].boom_type >= 0)  && (special_draw < 2)) {
			cur_boom_type = store_booms[i].boom_type;
			explode_place_rect[i] = base_rect;
			OffsetRect(&explode_place_rect[i],13 + 28 * (store_booms[i].dest.x - screen_ul.x) + store_booms[i].x_adj,
				13 + 36 * (store_booms[i].dest.y - screen_ul.y) + store_booms[i].y_adj);
				
			if ((store_booms[i].place_type == 1) && (special_draw < 2)) {
				temp_val = get_ran(1,0,50) - 25;
				temp_val2 = get_ran(1,0,50) - 25;
				OffsetRect(&explode_place_rect[i],temp_val,temp_val2);
				}
			
			// eliminate stuff that's too gone. 
			IntersectRect(&temp_rect,&explode_place_rect[i],&ter_scrn_rect);
			if (EqualRect(&temp_rect,&explode_place_rect[i]) == FALSE) {
				store_booms[i].boom_type = -1;
				}
			
			}
			else if (special_draw < 2)
				explode_place_rect[i].top =explode_place_rect[i].left =explode_place_rect[i].bottom =explode_place_rect[i].right = 0;
	
	if (special_draw < 2)
		play_sound(boom_type_sound[cur_boom_type]);
	
	// Now, at last, do explosion
	for (t = (special_draw == 2) ? 6 : 0; t < ((special_draw == 1) ? 6 : 11); t++) { // t goes up to 10 to make sure screen gets cleaned up
		// First, lay terrain in temporary graphic area;
		for (i = 0; i < 30; i++) 
			if (store_booms[i].boom_type >= 0) 
				rect_draw_some_item(terrain_screen_gworld,explode_place_rect[i],
					temp_gworld,explode_place_rect[i],0,0);

		// Now put in explosions
		for (i = 0; i < 30; i++) 
			if (store_booms[i].boom_type >= 0) {
				if ((t + store_booms[i].offset >= 0) && (t + store_booms[i].offset <= 7)) {
						from_rect = base_rect;
						OffsetRect(&from_rect,28 * (t + store_booms[i].offset),144 + 36 * (store_booms[i].boom_type));
						rect_draw_some_item(fields_gworld,from_rect,
							temp_gworld,explode_place_rect[i],1,0);
					
					if (store_booms[i].val_to_place > 0) {
						text_rect = explode_place_rect[i];
						text_rect.top += 2;
						text_rect.left -= 2;
						if (store_booms[i].val_to_place < 10)
							text_rect.left += 4;
						sprintf(str,"%d",store_booms[i].val_to_place);
						store_bmp = (HBITMAP) SelectObject(hdc,temp_gworld);
						char_win_draw_string(hdc,text_rect,str,1,12);
						SelectObject(hdc,store_bmp);
						}
					}
				}
		// Now draw all missiles to screen
		for (i = 0; i < 30; i++) 
			if (store_booms[i].boom_type >= 0) {
				to_rect = explode_place_rect[i];
				OffsetRect(&to_rect,current_terrain_ul.x,current_terrain_ul.y);
				rect_draw_some_item(temp_gworld,explode_place_rect[i],
					temp_gworld,to_rect,0,1);
				}

			Delay(2 * (1 + PSD[306][6]),&delay_dummy);
		}
		
	// Exit gracefully, and clean up screen
	for (i = 0; i < 30; i++) 
		if (special_draw != 1)
			store_booms[i].boom_type = -1;
	SelectObject(hdc,store_bmp);
	if (DeleteDC(hdc) == 0) DebugQuit("DeleteDC failed 1");
	DeleteObject(temp_gworld);

	if (special_draw != 1) {
		while (t2 - t1 < snd_len[cur_boom_type] + 100) {
			t2 = (long)GetCurrentTime();
			}
		play_sound(99);
		}
}

/*
shop_type:
0 - weapon shop
1 - armor shop
2 - misc shop
3 - healer
4 - food
5-9 - magic shop
10 - mage spells
11 - priest spells
12 alchemy
*/
void click_shop_rect(RECT area_rect)
{
	long dum;
	
	draw_shop_graphics(1,area_rect);
	if (play_sounds == TRUE) play_sound(37);
	else Delay(5,&dum);
	
	draw_shop_graphics(0,area_rect);

}
void draw_shop_graphics(short draw_mode,RECT clip_area_rect)
// mode 1 - drawing dark for button press
{
	RECT area_rect,from_rect,item_info_from = {42,11,56,24};
	
	RECT face_rect = {6,6,38,38};
	RECT title_rect = {48,15,260,42};
	RECT dest_rect,help_from = {126,23,142,36},done_from = {0,23,63,46}; /**/
	short faces[13] = {1,1,1,42,43, 1,1,1,1,1, 44,44,44};
	
	short i,what_chosen;
	RECT shopper_name = {6,44,260,56};
	short current_pos;
	
	short cur_cost,what_magic_shop,what_magic_shop_item;
	char cur_name[256];
	char cur_info_str[256];
	char *cost_strs[] = 
		{	"Extremely Cheap",
			"Very Reasonable",
			"Pretty Average",
			"Somewhat Pricey",
			"Expensive",
			"Exorbitant",
			"Utterly Ridiculous"
		};
	item_record_type base_item;
	HDC hdc;
	COLORREF colors[7] = {RGB(0,0,0),RGB(255,0,130),RGB(128,0,70),RGB(0,0,100),RGB(0,0,220),
		RGB(0,220,0),RGB(255,110,255)};
	HBITMAP store_bmp;
	HBRUSH old_brush;
	
	hdc = CreateCompatibleDC(main_dc);
	SetBkMode(hdc,TRANSPARENT);
	SelectObject(hdc,small_bold_font);
	store_bmp = (HBITMAP) SelectObject(hdc,talk_gworld);

	if (draw_mode > 0) {
		IntersectClipRect(hdc,clip_area_rect.left,clip_area_rect.top,clip_area_rect.right,clip_area_rect.bottom);
		}

	area_rect = talk_area_rect;
	OffsetRect(&area_rect,-5,-5);

	old_brush = (HBRUSH) SelectObject(hdc,GetStockObject(NULL_BRUSH));
	Rectangle(hdc,area_rect.left,area_rect.top,area_rect.right,area_rect.bottom);  
	SelectObject(hdc,old_brush);
	InflateRect(&area_rect,-1,-1);
	SelectObject(hdc,store_bmp);
	if (draw_mode > 0) {
		paint_pattern(talk_gworld,0,clip_area_rect,3);
		}
		else paint_pattern(talk_gworld,0,area_rect,3);

	SelectObject(hdc,talk_gworld);
	
	old_brush = (HBRUSH) SelectObject(hdc,GetStockObject(NULL_BRUSH));
	Rectangle(hdc,shop_frame.left,shop_frame.top,shop_frame.right,shop_frame.bottom);
	SelectObject(hdc,old_brush);

	SelectObject(hdc,store_bmp);
	// Place store icon
	if (draw_mode == 0) {
		i = faces[store_shop_type];
		draw_dialog_graphic((HWND) talk_gworld, face_rect, 1000 + i, FALSE,1);
		}
	SelectObject(hdc,talk_gworld);

	// Place name of store and shopper name
	SelectObject(hdc,fantasy_font);
	SetTextColor(hdc,colors[3]);
	dest_rect = title_rect;
	OffsetRect(&dest_rect,1,-5);
	char_win_draw_string(hdc,dest_rect,store_store_name,2,18);
	OffsetRect(&dest_rect,-1,-1);
	SetTextColor(hdc,colors[4]);
	char_win_draw_string(hdc,dest_rect,store_store_name,2,18);
	SelectObject(hdc,small_bold_font);

	SetTextColor(hdc,colors[3]);
	switch (store_shop_type) {
		case 3: sprintf(cur_name,"Healing for %s.",adven[current_pc].name); break;
		case 10: sprintf(cur_name,"Mage Spells for %s.",adven[current_pc].name);break;
		case 11: sprintf(cur_name,"Priest Spells for %s.",adven[current_pc].name); break;
		case 12: sprintf(cur_name,"Buying Alchemy.");break;
		case 4: sprintf(cur_name,"Buying Food.");break;
		default:sprintf(cur_name,"Shopping for %s.",adven[current_pc].name); break;
		}
	char_win_draw_string(hdc,shopper_name,cur_name,2,18);

	// Place help and done buttons
	SetTextColor(hdc,colors[0]);
	SelectObject(hdc,store_bmp);
	talk_help_rect.right = talk_help_rect.left + help_from.right - help_from.left;
	talk_help_rect.bottom = talk_help_rect.top + help_from.bottom - help_from.top;
	rect_draw_some_item(dlg_buttons_gworld,help_from,talk_gworld,talk_help_rect,0,0);
	rect_draw_some_item(dlg_buttons_gworld,done_from,talk_gworld,shop_done_rect,0,0);
	SelectObject(hdc,talk_gworld);
	if (draw_mode == 0)
		SetTextColor(hdc,colors[0]);
		else SetTextColor(hdc,colors[4]);


	SelectObject(hdc,small_bold_font);
	// Place all the items
	for (i = 0; i < 8; i++) {
		current_pos = i + GetScrollPos(shop_sbar,SB_CTL);
		if (store_shop_items[current_pos] < 0)
			break; // theoretically, this shouldn't happen
		cur_cost = store_shop_costs[current_pos];
		what_chosen = store_shop_items[current_pos];
		SelectObject(hdc,store_bmp);
		switch (what_chosen / 100) {
			case 0: case 1: case 2: case 3: case 4:
				base_item = get_stored_item(what_chosen);
				base_item.item_properties = base_item.item_properties | 1;
				draw_dialog_graphic((HWND) talk_gworld, shopping_rects[i][2],1800 + base_item.graphic_num, FALSE,1);
				strcpy(cur_name,base_item.full_name);
				get_item_interesting_string(base_item,cur_info_str);
				break;
			case 5:
				base_item = store_alchemy(what_chosen - 500);
				draw_dialog_graphic((HWND) talk_gworld, shopping_rects[i][2],1853, FALSE,1);//// all graphic nums
				strcpy(cur_name,base_item.full_name);
				sprintf(cur_info_str,"");
				break;
			case 6:
				break;
			case 7:
				what_chosen -= 700;
				draw_dialog_graphic((HWND) talk_gworld, shopping_rects[i][2],1879, FALSE,1);
				strcpy(cur_name,heal_types[what_chosen]);
				sprintf(cur_info_str,"");
				break;
			case 8:
				base_item = store_mage_spells(what_chosen - 800 - 30);
				draw_dialog_graphic((HWND)talk_gworld, shopping_rects[i][2],1800 + base_item.graphic_num, FALSE,1);

				strcpy(cur_name,base_item.full_name);
				sprintf(cur_info_str,"");
				break;
			case 9:
				base_item = store_priest_spells(what_chosen - 900 - 30);
				draw_dialog_graphic((HWND)talk_gworld, shopping_rects[i][2],1853, FALSE,1);
				strcpy(cur_name,base_item.full_name);
				sprintf(cur_info_str,"");
				break;
			default:
				what_magic_shop = (what_chosen / 1000) - 1;
				what_magic_shop_item = what_chosen % 1000;
				base_item = party.magic_store_items[what_magic_shop][what_magic_shop_item];
				base_item.item_properties = base_item.item_properties | 1;
				draw_dialog_graphic((HWND)talk_gworld, shopping_rects[i][2],1800 + base_item.graphic_num, FALSE,1);
				strcpy(cur_name,base_item.full_name);
				get_item_interesting_string(base_item,cur_info_str);
				break;
			}

		from_rect = item_info_from;
		OffsetRect(&from_rect,0,1);
		shopping_rects[i][6].bottom = shopping_rects[i][6].top +
			(from_rect.bottom - from_rect.top);
		if ((store_shop_type != 3) && (store_shop_type != 4))
			rect_draw_some_item(mixed_gworld,item_info_from,talk_gworld,shopping_rects[i][6],1,0);
		SelectObject(hdc,talk_gworld);
		// Now draw item shopping_rects[i][7]
		// 0 - whole area, 1 - active area 2 - graphic 3 - item name
		// 4 - item cost 5 - item extra str  6 - item help button
		char_win_draw_string(hdc,shopping_rects[i][3],cur_name,0,12);
		sprintf(cur_name,"Cost: %d",cur_cost);
		char_win_draw_string(hdc,shopping_rects[i][4],cur_name,0,12);
		char_win_draw_string(hdc,shopping_rects[i][5],cur_info_str,0,12);

		}

	// Finally, cost info and help strs
	sprintf(cur_name,"Prices here are %s.",cost_strs[store_cost_mult]);
	char_win_draw_string(hdc,bottom_help_rects[0],cur_name,0,12);
	char_win_draw_string(hdc,bottom_help_rects[1],"Click on item name (or type 'a'-'h') to buy.",0,12);
	char_win_draw_string(hdc,bottom_help_rects[2],"Hit done button (or Esc.) to quit.",0,12);
	if ((store_shop_type != 3) && (store_shop_type != 4))
		char_win_draw_string(hdc,bottom_help_rects[3],"'I' button brings up description.",0,12);
	
	SelectObject(hdc,store_bmp);
	if (!DeleteDC(hdc)) DebugQuit("Cannot release DC 8");

	refresh_shopping();
	ShowScrollBar(shop_sbar,SB_CTL,TRUE);
}

void refresh_shopping()
{
	RECT from_rects[4] = {{0,0,279,62},{0,62,253,352},{269,62,279,352},{0,352,279,415}},to_rect;
	/**/
	short i;
	
	for (i = 0; i < 4; i++) {
		to_rect = from_rects[i];
		OffsetRect(&to_rect,5,5);
		rect_draw_some_item(talk_gworld,from_rects[i],talk_gworld,to_rect,0,1);
		}
}

void click_talk_rect(char *str_to_place,char *str_to_place2,RECT c_rect)
{

	long dum;

	place_talk_str(str_to_place,str_to_place2,1,c_rect);
	if (play_sounds == TRUE)
		play_sound(37);
		else Delay(5,&dum);
	place_talk_str(str_to_place,str_to_place2,0,c_rect);
}

item_record_type store_mage_spells(short which_s) 
{
	item_record_type spell = {21,0, 0,0,0,0,0,0, 53,0,0,0,0,0, 0, 0,0, location(),"", "",0,0,0,0};

 short cost[32] = {150,200,150,1000,1200,400,300,200,
 200,250,500,1500,300,  250,125,150, 
  400,450, 800,600,700,600,7500, 500,
 5000,3000,3500,4000,4000,4500,7000,5000};

	char str[256];
	
	if (which_s != minmax(0,31,(int)which_s))
		which_s = 0;
	spell.item_level = which_s + 30;
	spell.value = cost[which_s];
	GetIndString(str,38,which_s + 1);
	strcpy((char *)spell.full_name,(char *)str);
	return spell;
}

// which_s = 0 means that it returns first 4th level spell
item_record_type store_priest_spells(short which_s)
{
	item_record_type spell = {21,0, 0,0,0,0,0,0, 53,0,0,0,0,0, 0, 0,0, location(),"", "",0,0,0,0};

	short cost[32] =
		{100,150,75,400,200, 100,80,250,
		400,400,1200,600,300, 600,350,250,
		500,500,600,800, 1000,900,400,600,
		2500,2000,4500,4500,3000,3000,2000,2000};
	char str[256];

	if (which_s != minmax(0,31,(int)which_s))
		which_s = 0;
	spell.item_level = which_s + 30;
	spell.value = cost[which_s];
	GetIndString(str,38,which_s + 50);
	strcpy((char *)spell.full_name,(char *)str);
	return spell;
}
item_record_type store_alchemy(short which_s)
{
	item_record_type spell = {21,0, 0,0,0,0,0,0, 53,0,0,0,0,0, 0, 0,0, location(),"", "",0,0,0,0};
short val[20] = {50,75,30,130,100,150, 200,200,300,250,300, 500,600,750,700,1000,10000,5000,7000,12000};
	char str[256];
	
	if (which_s != minmax(0,19,(int)which_s))
		which_s = 0;
	spell.item_level = which_s;
	spell.value = val[which_s];
	GetIndString(str,38,which_s + 100);
	strcpy((char *)spell.full_name,(char *)str);
	return spell; 
}

void get_item_interesting_string(item_record_type item,char *message)
{
	if (item.isProperty())
	{
		sprintf(message,"Not yours.");
		return;
	}
	if (item.isIdent() == false)
	{
		sprintf(message,"");
		return;
	}
	if (item.isCursed())
	{
		sprintf(message,"Cursed item.");
		return;
	}
	switch (item.variety) {
		case 1: case 2: case 5: case 6: case 24: case 25: ////
			if (item.bonus != 0)
				sprintf(message,"Damage: 1-%d + %d.",item.item_level,item.bonus);
				else sprintf(message,"Damage: 1-%d.",item.item_level);
			break;
		case 12: case 13: case 14: case 15: case 16: case 17:
			sprintf(message,"Blocks %d-%d damage.",item.item_level + (item.protection > 0) ? 1 : 0,
				item.item_level + item.protection);
			break;
		case 4: case 23:
			sprintf(message,"Bonus : +%d to hit.",item.bonus);
			break;
		case 3:
			sprintf(message,"%d gold pieces.",item.item_level);
			break;
		case 11:
			sprintf(message,"%d food.",item.item_level);
			break;
		case 20:
			sprintf(message,"Poison: Does %d-%d damage.",item.item_level,item.item_level * 6);
			break;
		default:
			sprintf(message,"");		
			if (item.charges > 0)
				sprintf(message,"Uses: %d",item.charges);		
			break;
		}	
	if (item.charges > 0)
		sprintf(message,"Uses: %d",item.charges);
		}


void place_talk_str(char *str_to_place,char *str_to_place2,short color,RECT c_rect)
// color 0 - regular  1 - darker
{
	RECT area_rect;
	
	RECT face_rect = {6,6,38,38};
	RECT title_rect = {48,19,260,42};
	RECT dest_rect,help_from = {126,23,142,36}; /**/

	short i,j,str_len,line_height = 18;
	char p_str[256],str[256],str_to_draw[256],str_to_draw2[256];
	short text_len[257],current_rect,store_last_word_break = 0,start_of_last_kept_word = -1;
	short last_line_break = 0,last_word_break = 0,on_what_line = 0,last_stored_word_break = 0;
	Boolean force_skip = FALSE;
	short face_to_draw;
	
	HDC hdc;
	COLORREF colors[7] = {RGB(0,0,0),RGB(0,0,204),RGB(0,0,102),RGB(0,0,100),RGB(0,0,220),
		RGB(51,153,51),RGB(0,204,0)};
	HBITMAP store_bmp;
	HBRUSH old_brush;
	HFONT store_font;

	hdc = CreateCompatibleDC(main_dc);
	SetBkMode(hdc,TRANSPARENT);
	store_font = (HFONT) SelectObject(hdc,fantasy_font);
	store_bmp = (HBITMAP)SelectObject(hdc,talk_gworld);

	if (c_rect.right > 0) {
		IntersectClipRect(hdc,c_rect.left,c_rect.top,c_rect.right,c_rect.bottom);
		}

	area_rect = talk_area_rect;
	OffsetRect(&area_rect,-5,-5);

	old_brush = (HBRUSH) SelectObject(hdc,GetStockObject(NULL_BRUSH));
	Rectangle(hdc,area_rect.left,area_rect.top,area_rect.right,area_rect.bottom);
	SelectObject(hdc,old_brush);
	InflateRect(&area_rect,-1,-1);

	SelectObject(hdc,store_bmp);
	if (c_rect.right > 0) {
		paint_pattern(talk_gworld,0,c_rect,3);
		}
		else paint_pattern(talk_gworld,0,area_rect,3);
		
	// Put help button
	talk_help_rect.right = talk_help_rect.left + help_from.right - help_from.left;
	talk_help_rect.bottom = talk_help_rect.top + help_from.bottom - help_from.top;
	rect_draw_some_item(dlg_buttons_gworld,help_from,talk_gworld,talk_help_rect,0,0);

	// Place face of talkee
	if ((color == 0) && (c_rect.right == 0)) {
		face_to_draw = scenario.scen_monsters[store_monst_type].default_facial_pic;
		if (store_talk_face_pic >= 0)
			face_to_draw = store_talk_face_pic;
		if (store_talk_face_pic >= 1000) {
			draw_dialog_graphic((HWND)  talk_gworld, face_rect, 2400 + store_talk_face_pic - 1000, FALSE,1);
			}
			else {
				i = get_monst_picnum(store_monst_type);

				if (face_to_draw <= 0)
					draw_dialog_graphic((HWND)  talk_gworld, face_rect, 400 + i, FALSE,1);
					else draw_dialog_graphic((HWND)  talk_gworld, face_rect, 1000 + face_to_draw, FALSE,1);
				}
		}
	SelectObject(hdc,talk_gworld);

	// Place name oftalkee
	SetTextColor(hdc,colors[3]);
	dest_rect = title_rect;
	OffsetRect(&dest_rect,1,-5);
	char_win_draw_string(hdc,dest_rect,title_string,2,18);
	OffsetRect(&dest_rect,-1,-1);
	SetTextColor(hdc,colors[4]);
	char_win_draw_string(hdc,dest_rect,title_string,2,18);

	// Place buttons at bottom.
	if (color == 0)
		SetTextColor(hdc,colors[5]);
		else SetTextColor(hdc,colors[6]);
    if((PSD[307][1] == 1) && (talk_end_forced))
    	ShowWindow(talk_edit_box, SW_HIDE);
	for (i = 0; i < 9; i++)
		if ((talk_end_forced == FALSE) || (i == 6) || (i == 5)) {
			OffsetRect(&preset_words[i].word_rect,0,-2);
			char_win_draw_string(hdc,preset_words[i].word_rect,preset_words[i].word,2,18);
			OffsetRect(&preset_words[i].word_rect,0,2);
			}
	// Place bulk of what said. Save words.
	if (color == 0)
		for (i = 0; i < 50; i++)
			store_words[i].word_rect.left = store_words[i].word_rect.right = 0;

	str_len = (short) strlen((char *)str_to_place);
	if (str_len == 0) {
		sprintf((char *) str_to_place,".");
		}
	strcpy((char *) str,str_to_place);
	strcpy((char *) p_str,str_to_place);

	for (i = 0; i < 257; i++)
		text_len[i]= 0;
	MeasureText(256,p_str,text_len,hdc);

	dest_rect = word_place_rect;

	current_rect = 0;

	if (color == 0)
		SetTextColor(hdc,colors[2]);
		else SetTextColor(hdc,colors[1]);
	MoveTo(dest_rect.left + 1 , dest_rect.top + 1 + line_height * on_what_line + 9);

	for (i = 0;i < str_len;i++) {
		if (((str[i] != 39) && ((str[i] < 65) || (str[i] > 122)) && ((str[i] < 48) || (str[i] > 57))) && (color == 0)) { // New word, so set up a rect
			if (((i - store_last_word_break >= 4) || (i >= str_len - 1)) 
			 && (i - last_stored_word_break >= 4) && (talk_end_forced == FALSE)) {
				store_words[current_rect].word_rect.left = dest_rect.left + (text_len[store_last_word_break] - text_len[last_line_break]) - 2;
				store_words[current_rect].word_rect.right = dest_rect.left + (text_len[i + 1] - text_len[last_line_break]) - 1;
				store_words[current_rect].word_rect.top = dest_rect.top + 1 + line_height * on_what_line - 5;
				store_words[current_rect].word_rect.bottom = dest_rect.top + 1 + line_height * on_what_line + 13;

				if ((str[store_last_word_break] < 48) || (str[store_last_word_break] == 96) 
					|| (str[store_last_word_break] > 122)
					|| ((str[store_last_word_break] >= 58) && (str[store_last_word_break] <= 64)))
						store_last_word_break++;
				
				store_words[current_rect].word[0] = str[store_last_word_break];
				store_words[current_rect].word[1] = str[store_last_word_break + 1];
				store_words[current_rect].word[2] = str[store_last_word_break + 2];
				store_words[current_rect].word[3] = str[store_last_word_break + 3];
				store_words[current_rect].word[4] = 0;
				for (j = 0; j < 4; j++)
					if ((store_words[current_rect].word[j] >= 65) && (store_words[current_rect].word[j] <= 90))
						store_words[current_rect].word[j] += 32;
				if (scan_for_response(store_words[current_rect].word) < 0) {
					store_words[current_rect].word_rect.left = store_words[current_rect].word_rect.right = 0;
					}
					else {
						start_of_last_kept_word = store_last_word_break;
						if (current_rect < 49)
							current_rect++;
						}
				last_stored_word_break = i + 1;
				}
			}
		if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6)) 
		  && (last_word_break > last_line_break)) || (str[i] == '|') || (i == str_len - 1)) {
			if (str[i] == '|') {
				str[i] = ' ';
		 		force_skip = TRUE;
				}
	 		store_last_word_break = last_word_break;
	 		if (i == str_len - 1)
	 			last_word_break = i + 2;
			sprintf((char *)str_to_draw,"                                                         ");
			strncpy ((char *) str_to_draw,(char *) str + last_line_break,(size_t) (last_word_break - last_line_break - 1));
			sprintf((char *)str_to_draw2," %s",str_to_draw);
			str_to_draw2[0] = (char) strlen((char *)str_to_draw);
			DrawString((str_to_draw2 + 1),store_text_x,store_text_y, hdc);
			on_what_line++;
			MoveTo(dest_rect.left + 1 , dest_rect.top + 1 + line_height * on_what_line + 9);
			last_line_break = last_word_break;
			if (force_skip == TRUE) {
				force_skip = FALSE;
				i++;
				last_line_break++;
				last_word_break++;
				}
			if ((start_of_last_kept_word >= last_line_break) && (current_rect > 0)) {
				OffsetRect(&store_words[current_rect - 1].word_rect,5 + -1 * store_words[current_rect - 1].word_rect.left,line_height);
				}
		}
		if (str[i] == ' ') { // New word
			store_last_word_break = last_word_break = i + 1;
			}
		if (on_what_line == 17)
			i = 10000;
		}

	// Now for string 2
	str_len = (short) strlen((char *)str_to_place2);
	start_of_last_kept_word = -1;

	if (str_len > 0) {

	strcpy((char *) str,str_to_place2);
	strcpy((char *) p_str,str_to_place2);

	for (i = 0; i < 257; i++)
		text_len[i]= 0;
	MeasureText(256,p_str,text_len,hdc);

	last_line_break = store_last_word_break = last_word_break = last_stored_word_break = 0;
	MoveTo(dest_rect.left + 1 , dest_rect.top + 1 + line_height * on_what_line + 9);
	for (i = 0;i < str_len;i++) {
		if (((str[i] != 39) && ((str[i] < 65) || (str[i] > 122)) && ((str[i] < 48) || (str[i] > 57))) && (color == 0))  { // New word, so set up a rect
			if (((i - store_last_word_break >= 4) || (i >= str_len - 1))
			 && (i - last_stored_word_break >= 4) && (talk_end_forced == FALSE)) {
				store_words[current_rect].word_rect.left = dest_rect.left + (text_len[store_last_word_break] - text_len[last_line_break]) - 2;
				store_words[current_rect].word_rect.right = dest_rect.left + (text_len[i + 1] - text_len[last_line_break]) - 1;
				store_words[current_rect].word_rect.top = dest_rect.top + 1 + line_height * on_what_line - 5;
				store_words[current_rect].word_rect.bottom = dest_rect.top + 1 + line_height * on_what_line + 13;

				if ((str[store_last_word_break] < 48) || (str[store_last_word_break] == 96)
					|| (str[store_last_word_break] > 122)
					|| ((str[store_last_word_break] >= 58) && (str[store_last_word_break] <= 64)))
						store_last_word_break++;

				store_words[current_rect].word[0] = str[store_last_word_break];
				store_words[current_rect].word[1] = str[store_last_word_break + 1];
				store_words[current_rect].word[2] = str[store_last_word_break + 2];
				store_words[current_rect].word[3] = str[store_last_word_break + 3];
				store_words[current_rect].word[4] = 0;
				for (j = 0; j < 4; j++)
					if ((store_words[current_rect].word[j] >= 65) && (store_words[current_rect].word[j] <= 90))
						store_words[current_rect].word[j] += 32;
				if (scan_for_response(store_words[current_rect].word) < 0)
					store_words[current_rect].word_rect.left = store_words[current_rect].word_rect.right = 0;
					else {
						start_of_last_kept_word = store_last_word_break;
						if (current_rect < 49)
							current_rect++;

						}
				last_stored_word_break = i + 1;
				}
			}
		if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6))
		  && (last_word_break > last_line_break)) || (str[i] == '|') || (i == str_len - 1)) {
			if (str[i] == '|') {
				str[i] = ' ';
				force_skip = TRUE;
				}
			store_last_word_break = last_word_break;
			if (i == str_len - 1)
				last_word_break = i + 2;
			sprintf((char *)str_to_draw,"                                                         ");
			strncpy ((char *) str_to_draw,(char *) str + last_line_break,(size_t) (last_word_break - last_line_break - 1));
			sprintf((char *)str_to_draw2," %s",str_to_draw);
			str_to_draw2[0] = (char) strlen((char *)str_to_draw);
			DrawString((str_to_draw2 + 1),store_text_x,store_text_y, hdc);
			on_what_line++;
			MoveTo(dest_rect.left + 1 , dest_rect.top + 1 + line_height * on_what_line + 9);
			last_line_break = last_word_break;
			if (force_skip == TRUE) {
				force_skip = FALSE;
				i++;
				last_line_break++;
				last_word_break++;
				}
			if ((start_of_last_kept_word >= last_line_break) && (current_rect > 0)) {
	 			OffsetRect(&store_words[current_rect - 1].word_rect,5 + -1 * store_words[current_rect - 1].word_rect.left,line_height);				
				}
		}
		if (str[i] == ' ') { // New word
			store_last_word_break = last_word_break = i + 1;
			}
		if (on_what_line == 17)
			i = 10000;
		}

	}
	
	SelectObject(hdc,store_bmp);
	SelectObject(hdc,store_font);
	if (!DeleteDC(hdc)) DebugQuit("Cannot release DC 7");

	refresh_talking();
	
	// Clean up strings
	for (i = 0; i < 50; i++)
		for (j = 0; j < 4; j++)
			if ((store_words[current_rect].word[j] >= 65) && (store_words[current_rect].word[j] <= 90))
				store_words[current_rect].word[j] += 32;



}

void refresh_talking()
{
	RECT from = {0,0,279,415};
	rect_draw_some_item(talk_gworld,from,talk_gworld,talk_area_rect,0,1);
}

short scan_for_response(char *str)
// returns -1 if no go
{
	short i;
	
	for (i = 0; i < 60; i++) { // 60 response in each bunch
		if ((talking.talk_nodes[i].personality != -1) &&
			((talking.talk_nodes[i].personality == store_personality)
		 || (talking.talk_nodes[i].personality == -2)) && 
			(((str[0] == talking.talk_nodes[i].link1[0]) && (str[1] == talking.talk_nodes[i].link1[1]) 
			&& (str[2] == talking.talk_nodes[i].link1[2]) && (str[3] == talking.talk_nodes[i].link1[3])) 
			|| ((str[0] == talking.talk_nodes[i].link2[0]) && (str[1] == talking.talk_nodes[i].link2[1]) 
			&& (str[2] == talking.talk_nodes[i].link2[2]) && (str[3] == talking.talk_nodes[i].link2[3]))))
				return i;
		}
	return -1;
}
