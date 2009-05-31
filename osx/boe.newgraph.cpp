
#include <string.h>
#include <stdio.h>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"
#include "boe.graphics.h"
#include "boe.graphutil.h"
#include "boe.monster.h"
#include "dlgtool.h"
#include "boe.newgraph.h"
#include "boe.fileio.h"
#include "boe.itemdata.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "boe.text.h"
#include "soundtool.h"
#include "dlglowlevel.h"
#include "dlgconsts.h"
#include "mathutil.h"
#include "graphtool.h"

short monsters_faces[190] = {0,1,2,3,4,5,6,7,8,9,
							10,0,12,11,11,12,13,13,2,11,
							11,14,15,14,59,59,59,14,17,16,
							18,27,20,30,31,32,19,19,25,25,
							45,45,45,45,45, 24,24,53,53,53,
							53,53,53,53,24, 24,24,24,22,22, // 50
							22,22,22,22,22,22,22,22,22,21,
							0,0,0,0,0, 0,0,0,0,0,
							0,0,0,23,0, 0,0,0,0,0,
							47,47,47,47,47, 49,49,49,49,0,
							0,0,0,0,0, 0,0,26,0,0, // 100
							0,0,0,0,0, 0,0,0,46,46,
							0,0,0,0,0, 0,0,0,0,0,
							0,0,0,0,0, 0,0,0,0,0,
							0,0,0,48,48,48,48,48,48,51,
							51,51,52,52,52,54,54,54,54,0, // 150
							0,0,0,0,26,26,0,0,0,50,
							23,0,0,0,0,0,0,0,23,23,
							0,0,0,55,23,36,31,0,0,0};
extern Point ul;
extern Rect	windRect;
extern long anim_ticks;
extern PixPatHandle	bg[];
extern WindowPtr mainPtr;
extern short dungeon_font_num,geneva_font_num,town_type,which_combat_type;
extern eGameMode overall_mode;
extern bool play_sounds,boom_anim_active,cartoon_happening,in_startup_mode;
extern GWorldPtr fields_gworld,mixed_gworld,dlg_buttons_gworld[NUM_BUTTONS][2],terrain_screen_gworld,missiles_gworld;
//extern party_record_type party;
extern Rect sbar_rect,item_sbar_rect,shop_sbar_rect;
extern ControlHandle text_sbar,item_sbar,shop_sbar;
extern location center;
extern short pc_marked_damage[6],pc_dir[6];
extern short monst_marked_damage[60];
extern location pc_pos[6];
//extern current_town_type univ.town;
//extern big_tr_type t_d;
//extern town_item_list	t_i;
extern ter_num_t combat_terrain[64][64];
//extern unsigned char misc_i[64][64],sfx[64][64];
extern Point store_anim_ul;
extern char light_area[13][13];
extern short terrain_there[9][9];
extern char unexplored_area[13][13];
extern PatHandle bw_pats[15];
extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x
extern short store_talk_face_pic;
extern cScenario scenario;
extern cUniverse univ;
//extern talking_record_type talking;

RgnHandle oval_region = NULL,dark_mask_region,temp_rect_rgn;

// Talk vars
extern word_rect_type store_words[50];
extern eGameMode store_pre_talk_mode;
extern short store_personality,store_personality_graphic,current_pc;
extern GWorldPtr talk_gworld;
extern bool talk_end_forced;
extern Str255 old_str1,old_str2,one_back1,one_back2;
extern word_rect_type preset_words[9];
extern Rect talk_area_rect, word_place_rect,talk_help_rect;
extern char title_string[50];
extern m_num_t store_monst_type;
//extern hold_responses store_resp[83];

// Shop vars
extern short store_shop_items[30],store_shop_costs[30];
extern short store_shop_type,store_shop_min,store_shop_max,store_cost_mult;
extern eGameMode store_pre_shop_mode;
extern char store_store_name[256];
extern Rect shopping_rects[8][7];
extern Rect bottom_help_rects[4];
extern Rect shop_name_str;
extern Rect shop_frame ;
extern Rect shop_done_rect;
//extern item_record_type food_types[15];
extern char *heal_types[];
extern short heal_costs[8];
extern short terrain_there[9][9];

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
bool have_missile,have_boom;
Rect explode_place_rect[30];


// Animation vars
//extern town_record_type anim_town;
//extern tiny_tr_type anim_t_d;
extern short anim_step;
short store_anim_type;
extern char anim_str[60];
extern location anim_str_loc;

// if < 6; target PC, if >= 100, targ monst, if 6, go to anim_pc_loc
short anim_pc_targets[6];
short anim_monst_targets[60];

short anim_pcs[6];

location anim_pc_locs[6],anim_monst_locs[60];
location anim_string_loc;
char anim_string[60];
char last_light_mask[13][13];

void apply_unseen_mask()
{
	Rect base_rect = {9,9,53,45},to_rect,big_to = {13,13,337,265};
	GrafPtr old_port;
	short i,j,k,l;
	ConstPatternParam c;
	bool need_bother = false;
	
	if (PSD[SDF_NO_FRILLS] > 0)
		return;

	if ((is_combat()) && (which_combat_type == 0))
		return;
	if (!(is_out()) && (univ.town->lighting_type > 0))
		return;
		
	for (i = 0; i < 11; i++)
		for (j = 0; j < 11; j++) 
			if (unexplored_area[i + 1][j + 1] == 1)
				need_bother = true;
	if (need_bother == false)
		return;
				
	GetPort(&old_port);
	SetPort(terrain_screen_gworld);
	//p = *bw_pats[3];
	//c = p;
	c = *bw_pats[3];
	PenPat(c);
	PenMode(notPatOr);
	
	for (i = 0; i < 11; i++)
		for (j = 0; j < 11; j++) 
			if (unexplored_area[i + 1][j + 1] == 1) {

					to_rect = base_rect;
					OffsetRect(&to_rect,-28 + i * 28,-36 + 36 * j);
					SectRect(&to_rect,&big_to,&to_rect);
					PaintRect(&to_rect);
					//PaintRoundRect(&to_rect,4,4);
					for (k = i - 2; k < i + 1; k++)
						for (l = j - 2; l < j + 1; l++)
							if ((k >= 0) && (l >= 0) && (k < 9) && (l < 9) && ((k != i - 1) || (l != j - 1)))
								terrain_there[k][l] = -1;

				}
	
	//p = *bw_pats[6];
	//c = p;
	c = *bw_pats[6];
	PenPat(c);
	PenMode(patCopy);
	SetPort(old_port);
}
 
void apply_light_mask() 
{
	GrafPtr old_port;
	Rect temp = {0,0,108,84},paint_rect,base_rect = {0,0,36,28};
	Rect big_to = {13,13,337,265};
	short i,j;
	bool is_dark = false,same_mask = true;
	if (PSD[SDF_NO_FRILLS] > 0)
		return;
	if (is_out())
		return;
	if (univ.town->lighting_type == 0)
		return;
	
	if (oval_region == NULL) {
		temp_rect_rgn = NewRgn();
		dark_mask_region = NewRgn();
		oval_region = NewRgn();
		OpenRgn();
		FrameOval(&temp);
		CloseRgn(oval_region);
		}
	
	// Process the light array
	for (i = 2; i < 11; i++)
		for (j = 2; j < 11; j++) 
			if (light_area[i][j] == 0) is_dark = true;
	if (is_dark == false) { 
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
		
	GetPort(&old_port);
	SetPort(terrain_screen_gworld);

	for (i = 2; i < 11; i++)
		for (j = 2; j < 11; j++) {
			if (light_area[i][j] == 1)
				terrain_there[i - 2][j - 2] = -1;
			}
	for (i = 0; i < 13; i++)
		for (j = 0; j < 13; j++) 
			if (last_light_mask[i][j] != light_area[i][j])
				same_mask = false;
	
	if (same_mask == true) {
		PaintRgn(dark_mask_region);
		SetPort(old_port);
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
				
				DiffRgn(dark_mask_region,oval_region,dark_mask_region);
				
				OffsetRgn(oval_region,-13 + -1 * (28 * (i - 3)),-13 + -1 * (36 * (j - 3)));
				//PaintRect(&paint_rect);
				}
			if (light_area[i][j] == 3) {
				paint_rect = base_rect;
				OffsetRect(&paint_rect,13 + 28 * (i - 2),13 + 36 * (j - 2));
				SetRectRgn(temp_rect_rgn,paint_rect.left,paint_rect.top,
					paint_rect.right + 28,paint_rect.bottom + 36);
				DiffRgn(dark_mask_region,temp_rect_rgn,dark_mask_region);
				if (light_area[i + 1][j] == 3) light_area[i + 1][j] = 0;
				if (light_area[i + 1][j + 1] == 3) light_area[i + 1][j + 1] = 0;
				if (light_area[i][j + 1] == 3) light_area[i][j + 1] = 0;
				}
			}

	//rect_draw_some_item(light_mask_gworld,big_from,terrain_screen_gworld,big_to,0,0);
	PaintRgn(dark_mask_region);
	SetPort(old_port);

}

void start_missile_anim()
{
	short i;
	
	if (boom_anim_active == true)
		return;
	boom_anim_active = true;
	for (i = 0; i < 30; i++) {
		store_missiles[i].missile_type = -1;
		store_booms[i].boom_type = -1;
		}
	for (i = 0; i < 6; i++) 
		pc_marked_damage[i] = 0;
	for (i = 0; i < univ.town->max_monst(); i++) 
		monst_marked_damage[i] = 0;
	have_missile = false;
	have_boom = false;
}

void end_missile_anim() 
{
	boom_anim_active = false;
	}
	
void add_missile(location dest,short missile_type,short path_type,short x_adj,short y_adj)
{
	short i;
	
	if (boom_anim_active == false)
		return;
	if (PSD[SDF_NO_FRILLS] > 0)
		return;
	// lose redundant missiles
	for (i = 0; i < 30; i++)
		if ((store_missiles[i].missile_type >= 0) && (dest == store_missiles[i].dest))
			return;
	for (i = 0; i < 30; i++)
		if (store_missiles[i].missile_type < 0) {
			have_missile = true;
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
//	if ((cartoon_happening == true) && (anim_step < 140))
//		return;
	start_missile_anim();
	add_missile(fire_to,miss_type,path, x_adj, y_adj);
	do_missile_anim(len,from, sound_num);
	end_missile_anim();
}

void run_a_boom(location boom_where,short type,short x_adj,short y_adj)////
{

//	if ((cartoon_happening == true) && (anim_step < 140))
//		return;
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
	
	if (PSD[SDF_NO_FRILLS] > 0)
		return;
	if (boom_anim_active == false)
		return;
	// lose redundant explosions
	for (i = 0; i < 30; i++)
		if ((store_booms[i].boom_type >= 0) && (dest == store_booms[i].dest)
		 && (place_type == 0)) {
			if (val_to_place > 0)
				store_booms[i].val_to_place = val_to_place;
			return;
			}
	for (i = 0; i < 30; i++)
		if (store_booms[i].boom_type < 0) {
			have_boom = true;
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
	Rect temp_rect,missile_origin_base = {1,1,17,17},active_area_rect,to_rect,from_rect;
	short i,store_missile_dir;
	Point start_point,finish_point[30];
	location screen_ul;
	
	short x1[30],x2[30],y1[30],y2[30],t; // for path paramaterization
	Rect missile_place_rect[30],missile_origin_rect[30],store_erase_rect[30];
	Point current_terrain_ul; 
	GWorldPtr temp_gworld;
	GrafPtr old_port;
	
	if ((have_missile == false) || (boom_anim_active == false)) {
		boom_anim_active = false;
		return;
		}
	
	for (i = 0; i < 30; i++)
		if (store_missiles[i].missile_type >= 0)
			i = 50;
	if (i == 30)
		return;

	// initialize general data
	if ((in_startup_mode == true) && (store_anim_type == 0)) {
		current_terrain_ul.h = 306;
		current_terrain_ul.v = 5;
		}
		else if ((cartoon_happening == true) && (store_anim_type > 0)) {
			current_terrain_ul = store_anim_ul;
			}
			else {
				current_terrain_ul.h = current_terrain_ul.v = 5;
				}
	
	// make terrain_template contain current terrain all nicely
	draw_terrain(1);
	GetPortBounds(terrain_screen_gworld,&to_rect);
	Rect oldBounds = to_rect;
	OffsetRect(&to_rect,current_terrain_ul.h, current_terrain_ul.v);
	rect_draw_some_item(terrain_screen_gworld,oldBounds,
		terrain_screen_gworld,to_rect,0,1);
			
	GetPort(&old_port);	
				
	// create and clip temporary anim template 
	GetPortBounds(terrain_screen_gworld,&temp_rect);
	NewGWorld(&temp_gworld,  0 /*8*/,&temp_rect, NULL, NULL, kNativeEndianPixMap);
	SetPort(temp_gworld);
	active_area_rect = temp_rect;
	InsetRect(&active_area_rect,13,13);
	ClipRect(&active_area_rect);
	SetPort(GetWindowPort(mainPtr));
	
	
	// init missile paths
	for (i = 0; i < 30; i++) {
		SetRect(&store_erase_rect[i],0,0,0,0);
		if ((store_missiles[i].missile_type >= 0) && (missile_origin == store_missiles[i].dest))
			store_missiles[i].missile_type = -1;
		}
	screen_ul.x = center.x - 4; screen_ul.y = center.y - 4;
	start_point.h = 13 + 14 + 28 * (short) (missile_origin.x - screen_ul.x);
	start_point.v = 13 + 18 + 36 * (short) (missile_origin.y - screen_ul.y);
	for (i = 0; i < 30; i++) 
		if (store_missiles[i].missile_type >= 0) {
			finish_point[i].h = 1 + 13 + 14 + store_missiles[i].x_adj + 28 * (short) (store_missiles[i].dest.x - screen_ul.x);
			finish_point[i].v = 1 + 13 + 18 + store_missiles[i].y_adj + 36 * (short) (store_missiles[i].dest.y - screen_ul.y);
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
			x1[i] = finish_point[i].h - start_point.h;
			x2[i] = start_point.h;
			y1[i] = finish_point[i].v - start_point.v;
			y2[i] = start_point.v;
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
				
				SectRect(&temp_rect,&active_area_rect,&missile_place_rect[i]);
				
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
				OffsetRect(&to_rect,current_terrain_ul.h,current_terrain_ul.v);
				rect_draw_some_item(terrain_screen_gworld,store_erase_rect[i],
					terrain_screen_gworld,to_rect,0,1);
				
				to_rect = missile_place_rect[i];
				store_erase_rect[i] = to_rect;
				OffsetRect(&to_rect,current_terrain_ul.h,current_terrain_ul.v);
				rect_draw_some_item(temp_gworld,missile_place_rect[i],
					temp_gworld,to_rect,0,1);
				}
		if ((PSD[SDF_GAME_SPEED] == 3) || ((PSD[SDF_GAME_SPEED] == 1) && (t % 4 == 0)) ||
			((PSD[SDF_GAME_SPEED] == 2) && (t % 3 == 0)))
			FlushAndPause(1);
		}
		
	// Exit gracefully, and clean up screen
	for (i = 0; i < 30; i++) 
		store_missiles[i].missile_type = -1;
	DisposeGWorld(temp_gworld);
	SetPort(old_port);

	GetPortBounds(terrain_screen_gworld,&to_rect);
	Rect oldRect = to_rect;
	OffsetRect(&to_rect,current_terrain_ul.h,current_terrain_ul.v);
	rect_draw_some_item(terrain_screen_gworld,oldRect,
		terrain_screen_gworld,to_rect,0,1);
}

short get_missile_direction(Point origin_point,Point the_point)
{
	location store_dir;
	short dir = 0;
	// To reuse legacy code, will renormalize the_point, which is missile destination,
	// so that origin_point is moved to (149,185) and the_point is moved in proportion
	the_point.h +=  149 - origin_point.h;
	the_point.v +=  185 - origin_point.v;
	
				if ((the_point.h < 135) & (the_point.v >= ((the_point.h * 34) / 10) - 293)
					& (the_point.v <= (-1 * ((the_point.h * 34) / 10) + 663)))
					store_dir.x--;
				if ((the_point.h > 163) & (the_point.v <= ((the_point.h * 34) / 10) - 350)
					& (the_point.v >= (-1 * ((the_point.h * 34) / 10) + 721)))
					store_dir.x++;
					
				if ((the_point.v < 167) & (the_point.v <= (the_point.h / 2) + 102)
					& (the_point.v <= (-1 * (the_point.h / 2) + 249)))
					store_dir.y--;
				if ((the_point.v > 203) & (the_point.v >= (the_point.h / 2) + 123)
					& (the_point.v >= (-1 * (the_point.h / 2) + 268)))
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

void do_explosion_anim(short sound_num,short special_draw)
// sound_num currently ignored
// special_draw - 0 normal 1 - first half 2 - second half
{
	Rect temp_rect,active_area_rect,to_rect,from_rect;
	Rect base_rect = {0,0,36,28},text_rect;
	char str[60];
	short i,temp_val,temp_val2;
	location screen_ul;
	
	short t,cur_boom_type = 0; 
	Point current_terrain_ul; 
	GWorldPtr temp_gworld;
	GrafPtr old_port;
	short boom_type_sound[3] = {5,10,53};
	
	if ((have_boom == false) || (boom_anim_active == false)) {
		boom_anim_active = false;
		return;
		}
	
	for (i = 0; i < 30; i++)
		if (store_booms[i].boom_type >= 0)
			i = 50;
	if (i == 30)
		return;

	// initialize general data
	if ((in_startup_mode == true) && (store_anim_type == 0)) {
		current_terrain_ul.h = 306;
		current_terrain_ul.v = 5;
		}
		else if ((cartoon_happening == true) && (store_anim_type > 0)) {
			current_terrain_ul = store_anim_ul;
			}
			else {
				current_terrain_ul.h = current_terrain_ul.v = 5;
				}

	
	// make terrain_template contain current terrain all nicely
	draw_terrain(1);
	if (special_draw != 2) {
		GetPortBounds(terrain_screen_gworld,&to_rect);
		Rect oldRect = to_rect;
		OffsetRect(&to_rect,current_terrain_ul.h, current_terrain_ul.v);
		rect_draw_some_item(terrain_screen_gworld,oldRect,
			terrain_screen_gworld,to_rect,0,1);
		}
		
	GetPort(&old_port);	
				
	// create and clip temporary anim template 
	GetPortBounds(terrain_screen_gworld,&temp_rect);
	NewGWorld(&temp_gworld,  0 /*8*/,&temp_rect, NULL, NULL, kNativeEndianPixMap);
	SetPort(temp_gworld);
	TextFont(geneva_font_num);
	TextFace(bold);
	TextSize(10);
	active_area_rect = temp_rect;
	InsetRect(&active_area_rect,13,13);
	ClipRect(&active_area_rect);
	SetPort(GetWindowPort(mainPtr));
	
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
			Rect tempRect2;
			GetPortBounds(terrain_screen_gworld,&tempRect2);
			SectRect(&explode_place_rect[i],&tempRect2,&temp_rect);
			if (EqualRect(&temp_rect,&explode_place_rect[i]) == false) {
				store_booms[i].boom_type = -1;
				}
			
			}
			else if (special_draw < 2)
				explode_place_rect[i].top =explode_place_rect[i].left =explode_place_rect[i].bottom =explode_place_rect[i].right = 0;
	
	//play_sound(-1 * sound_num);
	if (special_draw < 2)
		play_sound(-1 * boom_type_sound[cur_boom_type]);
	
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
						text_rect.top += 4;
						text_rect.left -= 10;
						if (store_booms[i].val_to_place < 10)
							text_rect.left += 8;
						sprintf(str,"%d",store_booms[i].val_to_place);
						SetPort(temp_gworld);
						ForeColor(whiteColor);
						char_port_draw_string(temp_gworld,text_rect,str,1,12,false);
						ForeColor(blackColor);
						SetPort(GetWindowPort(mainPtr));
						}
					}
				}
		// Now draw all missiles to screen
		for (i = 0; i < 30; i++) 
			if (store_booms[i].boom_type >= 0) {
				to_rect = explode_place_rect[i];
				OffsetRect(&to_rect,current_terrain_ul.h,current_terrain_ul.v);
				rect_draw_some_item(temp_gworld,explode_place_rect[i],
					temp_gworld,to_rect,0,1);
				}
		//if (((PSD[SDF_GAME_SPEED] == 1) && (t % 3 == 0)) || ((PSD[SDF_GAME_SPEED] == 2) && (t % 2 == 0)))
			FlushAndPause(2 * (1 + PSD[SDF_GAME_SPEED]));
		}
		
	// Exit gracefully, and clean up screen
	for (i = 0; i < 30; i++) 
		if (special_draw != 1)
			store_booms[i].boom_type = -1;
	DisposeGWorld(temp_gworld);
	SetPort(old_port);

	//to_rect = terrain_screen_gworld->portRect;
	//OffsetRect(&to_rect,current_terrain_ul.h,current_terrain_ul.v);
	//rect_draw_some_item(terrain_screen_gworld,terrain_screen_gworld->portRect,
	//	terrain_screen_gworld,to_rect,0,1);
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
void click_shop_rect(Rect area_rect)
{ 
	
	draw_shop_graphics(1,area_rect);
	if (play_sounds == true)
		play_sound(37);
		else FlushAndPause(5);
	draw_shop_graphics(0,area_rect);

}
void draw_shop_graphics(short draw_mode,Rect clip_area_rect)
// mode 1 - drawing dark for button press
{
	Rect area_rect,item_info_from = {11,42,24,56};
	
	Rect face_rect = {6,6,38,38};
	Rect title_rect = {15,48,42,260};
	Rect dest_rect,help_from = {85,36,101,54};
	short faces[13] = {1,1,1,42,43, 1,1,1,1,1, 44,44,44};
	
	short i,what_chosen;
	RGBColor c[7] = {{0,0,0},{0,0,32767},{0,0,14535},{0,0,26623},{0,0,59391},
	{0,40959,0},{0,24575,0}};
	Rect shopper_name = {44,6,56,260};
	short current_pos;
	
	short cur_cost,what_magic_shop,what_magic_shop_item;
	char cur_name[60];
	char cur_info_str[60];
char *cost_strs[] = {"Extremely Cheap","Very Reasonable","Pretty Average","Somewhat Pricey",
	"Expensive","Exorbitant","Utterly Ridiculous"};
	GrafPtr old_port;
	cItemRec base_item;
	
	if (overall_mode != 21) {
		return;
		}
	
	
	GetPort(&old_port);
	SetPort(talk_gworld);
	TextFont(dungeon_font_num);
	TextSize(18);
	TextFace(0);

	if (draw_mode > 0) {
		ClipRect(&clip_area_rect);	
		}

	GetPortBounds(talk_gworld,&area_rect);
	FrameRect(&area_rect);
	InsetRect(&area_rect,1,1);
	FillCRect(&area_rect,bg[12]);

	FrameRect(&shop_frame);
	
	// Place store icon
	if (draw_mode == 0) { 
		SetPort(GetWindowPort(mainPtr));
		i = faces[store_shop_type];
		draw_dialog_graphic( talk_gworld, face_rect, i, PICT_TALK, false,1);
		SetPort( talk_gworld);
	}


	// Place name of store and shopper name
	RGBForeColor(&c[3]);
	dest_rect = title_rect;
	OffsetRect(&dest_rect,1,1);
	char_port_draw_string(talk_gworld,dest_rect,store_store_name,2,18,false);
	OffsetRect(&dest_rect,-1,-1);
	RGBForeColor(&c[4]);
	char_port_draw_string( talk_gworld,dest_rect,store_store_name,2,18,false);	

	TextFont(geneva_font_num);
	TextSize(12);
	TextFace(bold);

	RGBForeColor(&c[3]);
	switch (store_shop_type) {
		case 3: sprintf(cur_name,"Healing for %s.",univ.party[current_pc].name.c_str()); break;
		case 10: sprintf(cur_name,"Mage Spells for %s.",univ.party[current_pc].name.c_str());break;
		case 11: sprintf(cur_name,"Priest Spells for %s.",univ.party[current_pc].name.c_str()); break;
		case 12: sprintf(cur_name,"Buying Alchemy.");break;
		case 4: sprintf(cur_name,"Buying Food.");break;
		default:sprintf(cur_name,"Shopping for %s.",univ.party[current_pc].name.c_str()); break;
		}
	char_port_draw_string( talk_gworld,shopper_name,cur_name,2,18,false);	

	// Place help and done buttons
	ForeColor(blackColor);
	GetPortBounds(dlg_buttons_gworld[3][0],&help_from);
	talk_help_rect.right = talk_help_rect.left + help_from.right - help_from.left;
	talk_help_rect.bottom = talk_help_rect.top + help_from.bottom - help_from.top;
	rect_draw_some_item(dlg_buttons_gworld[3][draw_mode],help_from,talk_gworld,talk_help_rect,0,0);
	GetPortBounds(dlg_buttons_gworld[11][0],&help_from);
	//talk_help_rect.right = talk_help_rect.left + help_from.right - help_from.left;
	//talk_help_rect.bottom = talk_help_rect.top + help_from.bottom - help_from.top;
	rect_draw_some_item(dlg_buttons_gworld[11][draw_mode],help_from,talk_gworld,shop_done_rect,0,0);
	
	if (draw_mode == 0)
		ForeColor(blackColor);
		else RGBForeColor(&c[4]);
		
	// Place all the items
	for (i = 0; i < 8; i++) {
		current_pos = i + GetControlValue(shop_sbar);
		if (store_shop_items[current_pos] < 0)
			break; // theoretically, this shouldn't happen
		cur_cost = store_shop_costs[current_pos];
		what_chosen = store_shop_items[current_pos];
		switch (what_chosen / 100) {
			case 0: case 1: case 2: case 3: case 4: 
				base_item = get_stored_item(what_chosen);
				base_item.item_properties = base_item.item_properties | 1;
				draw_dialog_graphic( talk_gworld, shopping_rects[i][2],base_item.graphic_num,PICT_ITEM, false,1);
				strcpy(cur_name,base_item.full_name.c_str());
				get_item_interesting_string(base_item,cur_info_str);
				break;
			case 5:
				base_item = store_alchemy(what_chosen - 500);
				draw_dialog_graphic( talk_gworld, shopping_rects[i][2],53,PICT_ITEM, false,1);//// all graphic nums
				strcpy(cur_name,base_item.full_name.c_str());
				strcpy(cur_info_str,"");
				break;
			case 6:
				//base_item = food_types[what_chosen - 600];
				//draw_dialog_graphic( talk_gworld, shopping_rects[i][2],633, false,1);
				//strcpy(cur_name,base_item.full_name);
				//get_item_interesting_string(base_item,cur_info_str);
				break;
			case 7:
				what_chosen -= 700;
				draw_dialog_graphic( talk_gworld, shopping_rects[i][2],99,PICT_ITEM, false,1);
				strcpy(cur_name,heal_types[what_chosen]);
				strcpy(cur_info_str,"");
				break;
			case 8:
				base_item = store_mage_spells(what_chosen - 800 - 30);
				draw_dialog_graphic( talk_gworld, shopping_rects[i][2],base_item.graphic_num,PICT_ITEM, false,1);

				strcpy(cur_name,base_item.full_name.c_str());
				strcpy(cur_info_str,"");		
				break;
			case 9:
				base_item = store_priest_spells(what_chosen - 900 - 30);
				draw_dialog_graphic( talk_gworld, shopping_rects[i][2],base_item.graphic_num,PICT_ITEM, false,1);
				strcpy(cur_name,base_item.full_name.c_str());
				strcpy(cur_info_str,"");
				break;
			default:
				what_magic_shop = (what_chosen / 1000) - 1;
				what_magic_shop_item = what_chosen % 1000;
				base_item = univ.party.magic_store_items[what_magic_shop][what_magic_shop_item];
				base_item.item_properties = base_item.item_properties | 1;
				draw_dialog_graphic( talk_gworld, shopping_rects[i][2],base_item.graphic_num,PICT_ITEM, false,1);
				strcpy(cur_name,base_item.full_name.c_str());
				get_item_interesting_string(base_item,cur_info_str);
				break;
			}

		// Now draw item shopping_rects[i][7]
		// 0 - whole area, 1 - active area 2 - graphic 3 - item name
		// 4 - item cost 5 - item extra str  6 - item help button
		TextSize(12);
		char_port_draw_string( talk_gworld,shopping_rects[i][3],cur_name,0,12,false);
		sprintf(cur_name,"Cost: %d",cur_cost);
		char_port_draw_string( talk_gworld,shopping_rects[i][4],cur_name,0,12,false);
		TextSize(10);
		char_port_draw_string( talk_gworld,shopping_rects[i][5],cur_info_str,0,12,false);
		if ((store_shop_type != 3) && (store_shop_type != 4))
			rect_draw_some_item(mixed_gworld,item_info_from,talk_gworld,shopping_rects[i][6],1 - draw_mode,0);

		}

	// Finally, cost info and help strs
	TextSize(12);
	sprintf(cur_name,"Prices here are %s.",cost_strs[store_cost_mult]);
	TextSize(10);
	char_port_draw_string( talk_gworld,bottom_help_rects[0],cur_name,0,12,false);
	char_port_draw_string( talk_gworld,bottom_help_rects[1],"Click on item name (or type 'a'-'h') to buy.",0,12,false);
	char_port_draw_string( talk_gworld,bottom_help_rects[2],"Hit done button (or Esc.) to quit.",0,12,false);
	if ((store_shop_type != 3) && (store_shop_type != 4))
		char_port_draw_string( talk_gworld,bottom_help_rects[3],"'I' button brings up description.",0,12,false);
	
	
	ForeColor(blackColor);
	GetPortBounds(talk_gworld,&area_rect);
	ClipRect(&area_rect);
	SetPort(old_port);

	refresh_shopping();
	ShowControl(shop_sbar);
	Draw1Control(shop_sbar);
}

void refresh_shopping()
{
	Rect from_rects[4] = {{0,0,62,279},{62,0,352,253},{62,269,352,279},{352,0,415,279}},to_rect;
	short i;
	
	for (i = 0; i < 4; i++) {
		to_rect = from_rects[i];
		OffsetRect(&to_rect,5,5);
		rect_draw_some_item(talk_gworld,from_rects[i],talk_gworld,to_rect,0,1);
		}
}

void click_talk_rect(char *str_to_place,char *str_to_place2,Rect c_rect)
{

	place_talk_str(str_to_place,str_to_place2,1,c_rect);
	if (play_sounds == true)
		play_sound(37);
		else FlushAndPause(5);
	place_talk_str(str_to_place,str_to_place2,0,c_rect);
}

////
// which_s = 0 means that it returns first 4th level spell
cItemRec store_mage_spells(short which_s) 
{
	cItemRec spell('spel');// = {21,0, 0,0,0,0,0,0, 53,0,0,0,0, 0, 0,0, {0,0},"", "",0,0,0,0};
	static const short cost[32] = {
		150,200,150,1000,1200,400,300,200,
		200,250,500,1500,300,  250,125,150, 
		400,450, 800,600,700,600,7500, 500,
		5000,3000,3500,4000,4000,4500,7000,5000
	};

	char str[255];
	
	if (which_s != minmax(0,31,which_s))
		which_s = 0;
	spell.item_level = which_s + 30;
	spell.value = cost[which_s];
	get_str((unsigned char*)str,38,which_s + 1);
	spell.full_name = str;
	return spell;
}

// which_s = 0 means that it returns first 4th level spell
cItemRec store_priest_spells(short which_s)
{
	cItemRec spell('spel');// = {21,0, 0,0,0,0,0,0, 53,0,0,0,0, 0, 0,0, {0,0},"", "",0,0,0,0};
	static const short cost[32] = {
		100,150,75,400,200, 100,80,250,
		400,400,1200,600,300, 600,350,250,
		500,500,600,800, 1000,900,400,600,
		2500,2000,4500,4500,3000,3000,2000,2000
	};
	char str[255];

	if (which_s != minmax(0,31,which_s))
		which_s = 0;
	spell.item_level = which_s + 30;
	spell.value = cost[which_s];
	get_str((unsigned char*)str,38,which_s + 50);
	spell.full_name = str;
	return spell;
}
cItemRec store_alchemy(short which_s)
{
	cItemRec spell('spel');// = {21,0, 0,0,0,0,0,0, 53,0,0,0,0, 0, 0,0, {0,0},"", "",0,0,0,0};
	static const short val[20] = {
		50,75,30,130,100,
		150, 200,200,300,250,
		300, 500,600,750,700,
		1000,10000,5000,7000,12000
	};
	char str[255];
	
	if (which_s != minmax(0,19,which_s))
		which_s = 0;
	spell.item_level = which_s;
	spell.value = val[which_s];
	get_str((unsigned char*)str,38,which_s + 100);
	spell.full_name = str;
	return spell; 
}

void get_item_interesting_string(cItemRec item,char *message)
{
	if (item.is_property() == true) {
		strcpy(message,"Not yours.");
		return;
		}
	if (item.is_ident() == false) {
		strcpy(message,"");
		return;
		}
	if (item.is_cursed() == true) {
		strcpy(message,"Cursed item.");
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
			strcpy(message,"");		
			if (item.charges > 0)
				sprintf(message,"Uses: %d",item.charges);		
			break;
		}	
	if (item.charges > 0)
		sprintf(message,"Uses: %d",item.charges);		
}


void place_talk_str(char *str_to_place,char *str_to_place2,short color,Rect c_rect)
// color 0 - regular  1 - darker
{
	Rect area_rect;
	
	Rect face_rect = {6,6,38,38};
	Rect title_rect = {19,48,42,260};
	Rect dest_rect,help_from = {85,36,101,54};
	Str255 fn2 = "\pDungeon Bold";
	Str255 fn3 = "\pPalatino";

	short i,j,str_len,line_height = 17;
	Str255 p_str,str,str_to_draw,str_to_draw2;
	short text_len[257],current_rect,store_last_word_break = 0,start_of_last_kept_word = -1;
	short last_line_break = 0,last_word_break = 0,on_what_line = 0,last_stored_word_break = 0;
	bool force_skip = false;
	short face_to_draw;
	
	RGBColor c[7] = {{0,0,0},{0,0,32767},{0,0,14535},{0,0,26623},{0,0,59391},
	{0,40959,0},{0,24575,0}};
	
	GrafPtr old_port;
	
	GetPort(&old_port);
	SetPort( talk_gworld);

	// This redundancy is to try to keep the font from disappearing
	GetFNum(fn2,&dungeon_font_num);
	if (dungeon_font_num == 0)
		GetFNum(fn3,&dungeon_font_num);
		
	TextFont(dungeon_font_num);
	//TextFont(geneva_font_num);
	TextSize(18);
	TextFace(0);

	if (c_rect.right > 0) {
		ClipRect(&c_rect);	
		}

	GetPortBounds(talk_gworld,&area_rect);
	FrameRect(&area_rect);
	InsetRect(&area_rect,1,1);
	FillCRect(&area_rect,bg[12]);

	// Put help button
	GetPortBounds(dlg_buttons_gworld[3][0], &help_from);
	talk_help_rect.right = talk_help_rect.left + help_from.right - help_from.left;
	talk_help_rect.bottom = talk_help_rect.top + help_from.bottom - help_from.top;
	rect_draw_some_item(dlg_buttons_gworld[3][0],help_from,talk_gworld,talk_help_rect,0,0);
	
	// Place face of talkee
	if ((color == 0) && (c_rect.right == 0)) { 
		////
		SetPort(GetWindowPort(mainPtr));
		face_to_draw = scenario.scen_monsters[store_monst_type].default_facial_pic;
		if (store_talk_face_pic >= 0)
			face_to_draw = store_talk_face_pic;
		if (store_talk_face_pic >= 1000) {
			draw_dialog_graphic(  talk_gworld, face_rect, store_talk_face_pic, PICT_CUSTOM + PICT_TALK, false,1);
			}
			else {
				i = get_monst_picnum(store_monst_type);
				if (face_to_draw <= 0)
					draw_dialog_graphic(talk_gworld, face_rect, i, get_monst_pictype(store_monst_type), false,1);
				else draw_dialog_graphic(talk_gworld, face_rect, face_to_draw, PICT_MONST, false,1);
			}
		SetPort( talk_gworld);
		}
	// Place name oftalkee
	RGBForeColor(&c[3]);
	dest_rect = title_rect;
	OffsetRect(&dest_rect,1,1);
	char_port_draw_string( talk_gworld,dest_rect,title_string,2,18,false);
	OffsetRect(&dest_rect,-1,-1);
	RGBForeColor(&c[4]);
	char_port_draw_string( talk_gworld,dest_rect,title_string,2,18,false);
		
	// Place buttons at bottom.
	if (color == 0)
		RGBForeColor(&c[5]);
		else RGBForeColor(&c[6]);
	for (i = 0; i < 9; i++) 
		if ((talk_end_forced == false) || (i == 6) || (i == 5)) {
			OffsetRect(&preset_words[i].word_rect,0,8);
			char_port_draw_string( talk_gworld,preset_words[i].word_rect,preset_words[i].word,2,18,false);
			OffsetRect(&preset_words[i].word_rect,0,-8);
			}
	// Place bulk of what said. Save words.
	//TextSize(14);
	if (color == 0)
		for (i = 0; i < 50; i++)
			store_words[i].word_rect.left = store_words[i].word_rect.right = 0;
			
	str_len = (short) strlen((char *)str_to_place);
	if (str_len == 0) {
		sprintf((char *) str_to_place,".");
		}	
	strcpy((char *) str,str_to_place);
	strcpy((char *) p_str,str_to_place);
	c2pstr((char*)p_str);	
	for (i = 0; i < 257; i++)
		text_len[i]= 0;
	MeasureText(256,p_str,text_len);

	dest_rect = word_place_rect;

	current_rect = 0;
	
	if (color == 0)
		RGBForeColor(&c[2]);
		else RGBForeColor(&c[1]);
	MoveTo(dest_rect.left + 1 , dest_rect.top + 1 + line_height * on_what_line + 9);
	//for (i = 0;text_len[i] != text_len[i + 1], i < str_len;i++) {
	for (i = 0;i < str_len;i++) {
		if (((str[i] != 39) && ((str[i] < 65) || (str[i] > 122)) && ((str[i] < 48) || (str[i] > 57))) && (color == 0)) { // New word, so set up a rect
			if (((i - store_last_word_break >= 4) || (i >= str_len - 1)) 
			 && (i - last_stored_word_break >= 4) && (talk_end_forced == false)) {
				store_words[current_rect].word_rect.left = dest_rect.left + (text_len[store_last_word_break] - text_len[last_line_break]) - 2;
				store_words[current_rect].word_rect.right = dest_rect.left + (text_len[i + 1] - text_len[last_line_break]) - 1;
				store_words[current_rect].word_rect.top = dest_rect.top + 1 + line_height * on_what_line - 5;
				store_words[current_rect].word_rect.bottom = dest_rect.top + 1 + line_height * on_what_line + 13;
				
				if ((str[store_last_word_break] < 48) || (str[store_last_word_break] == 96) 
					|| (str[store_last_word_break] > 122)
					|| ((str[store_last_word_break] >= 58) && (str[store_last_word_break] <= 64)))
						store_last_word_break++;

				// adjust for if this word will be scrolled down
				//if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6)) 
		  		//	&& (last_word_break > last_line_break)) || (str[i] == '|')) {
		  		//	OffsetRect(&store_words[current_rect].word_rect,5 + -1 * store_words[current_rect].word_rect.left,line_height);
		  		//	}
				
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
					
						//FrameRect(&store_words[current_rect].word_rect);
						}
				last_stored_word_break = i + 1;
				}
			}
		if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6)) 
		  && (last_word_break > last_line_break)) || (str[i] == '|') || (i == str_len - 1)) {
			if (str[i] == '|') {
				str[i] = ' ';
		 		force_skip = true;
	 			}
	 		store_last_word_break = last_word_break;
	 		if (i == str_len - 1)
	 			last_word_break = i + 2;
			sprintf((char *)str_to_draw,"                                                         ");
			strncpy ((char *) str_to_draw,(char *) str + last_line_break,(size_t) (last_word_break - last_line_break - 1));
			sprintf((char *)str_to_draw2," %s",str_to_draw);
			str_to_draw2[0] = (char) strlen((char *)str_to_draw);
			DrawString(str_to_draw2);
			on_what_line++;
			MoveTo(dest_rect.left + 1 , dest_rect.top + 1 + line_height * on_what_line + 9);
			last_line_break = last_word_break;
			if (force_skip == true) {
				force_skip = false;
				i++;
				last_line_break++;
				last_word_break++;
				}
			if ((start_of_last_kept_word >= last_line_break) && (current_rect > 0)) {
				//SysBeep(2);
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
	c2pstr((char*)p_str);	
	for (i = 0; i < 257; i++)
		text_len[i]= 0;
	MeasureText(256,p_str,text_len);
	
	last_line_break = store_last_word_break = last_word_break = last_stored_word_break = 0;
	MoveTo(dest_rect.left + 1 , dest_rect.top + 1 + line_height * on_what_line + 9);
	//for (i = 0;text_len[i] != text_len[i + 1], i < str_len;i++) 
	for (i = 0;i < str_len;i++) {
		if (((str[i] != 39) && ((str[i] < 65) || (str[i] > 122)) && ((str[i] < 48) || (str[i] > 57))) && (color == 0))  { // New word, so set up a rect
			if (((i - store_last_word_break >= 4) || (i >= str_len - 1)) 
			 && (i - last_stored_word_break >= 4) && (talk_end_forced == false)) {
				store_words[current_rect].word_rect.left = dest_rect.left + (text_len[store_last_word_break] - text_len[last_line_break]) - 2;
				store_words[current_rect].word_rect.right = dest_rect.left + (text_len[i + 1] - text_len[last_line_break]) - 1;
				store_words[current_rect].word_rect.top = dest_rect.top + 1 + line_height * on_what_line - 5;
				store_words[current_rect].word_rect.bottom = dest_rect.top + 1 + line_height * on_what_line + 13;
				
				if ((str[store_last_word_break] < 48) || (str[store_last_word_break] == 96) 
					|| (str[store_last_word_break] > 122)
					|| ((str[store_last_word_break] >= 58) && (str[store_last_word_break] <= 64)))
						store_last_word_break++;

				// adjust for if this word will be scrolled down
				//if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6)) 
		  		//	&& (last_word_break > last_line_break)) || (str[i] == '|')) {
		  		//	OffsetRect(&store_words[current_rect].word_rect,5 + -1 * store_words[current_rect].word_rect.left,line_height);
		  		//	}
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
					
						//FrameRect(&store_words[current_rect].word_rect);
						}
				last_stored_word_break = i + 1;
				}
			}
		if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6)) 
		  && (last_word_break > last_line_break)) || (str[i] == '|') || (i == str_len - 1)) {
			if (str[i] == '|') {
				str[i] = ' ';
		 		force_skip = true;
	 			}
	 		store_last_word_break = last_word_break;
	 		if (i == str_len - 1)
	 			last_word_break = i + 2;
			sprintf((char *)str_to_draw,"                                                         ");
			strncpy ((char *) str_to_draw,(char *) str + last_line_break,(size_t) (last_word_break - last_line_break - 1));
			sprintf((char *)str_to_draw2," %s",str_to_draw);
			str_to_draw2[0] = (char) strlen((char *)str_to_draw);
			DrawString(str_to_draw2);
			on_what_line++;
			MoveTo(dest_rect.left + 1 , dest_rect.top + 1 + line_height * on_what_line + 9);
			last_line_break = last_word_break;
			if (force_skip == true) {
				force_skip = false;
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
	
	ForeColor(blackColor);
	GetPortBounds(talk_gworld,&area_rect);
	Rect oldRect = area_rect;
	ClipRect(&area_rect);
	
	// Finally place processed graphics
	SetPort(GetWindowPort(mainPtr));
	rect_draw_some_item(talk_gworld,oldRect,talk_gworld,talk_area_rect,0,1);
	SetPort(old_port);
	
	// Clean up strings
	for (i = 0; i < 50; i++)
		for (j = 0; j < 4; j++)
			if ((store_words[current_rect].word[j] >= 65) && (store_words[current_rect].word[j] <= 90))
				store_words[current_rect].word[j] += 32;

}

void refresh_talking()
{
	Rect tempRect;
	GetPortBounds(talk_gworld,&tempRect);
	rect_draw_some_item(talk_gworld,tempRect,talk_gworld,talk_area_rect,0,1);
}

short scan_for_response(char *str)
// returns -1 if no go
{
	short i;
	
	for (i = 0; i < 60; i++) { // 60 response in each bunch
		if ((univ.town.cur_talk->talk_nodes[i].personality != -1) &&
			((univ.town.cur_talk->talk_nodes[i].personality == store_personality)
		 || (univ.town.cur_talk->talk_nodes[i].personality == -2)) && 
			(((str[0] == univ.town.cur_talk->talk_nodes[i].link1[0]) && (str[1] == univ.town.cur_talk->talk_nodes[i].link1[1]) 
			&& (str[2] == univ.town.cur_talk->talk_nodes[i].link1[2]) && (str[3] == univ.town.cur_talk->talk_nodes[i].link1[3])) 
			|| ((str[0] == univ.town.cur_talk->talk_nodes[i].link2[0]) && (str[1] == univ.town.cur_talk->talk_nodes[i].link2[1]) 
			&& (str[2] == univ.town.cur_talk->talk_nodes[i].link2[2]) && (str[3] == univ.town.cur_talk->talk_nodes[i].link2[3]))))
				return i;
		}
	return -1;
}

