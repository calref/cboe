
#include <stdio.h>

//#include "item.h"

#include "boe.global.h"
#include "classes.h"
#include "boe.graphutil.h"
#include "boe.text.h"
#include "boe.fields.h"
#include "boe.locutils.h"
#include "boe.graphics.h"
#include "boe.infodlg.h"
#include "boe.monster.h"
#include "dlgtool.h"
//#include "soundtool.h"
#include "graphtool.h"
#include "mathutil.h"
#include "dlgutil.h"

extern WindowPtr	mainPtr;
extern Rect	windRect;
extern short stat_window,give_delays;
extern eGameMode overall_mode;
extern short current_spell_range,town_type;
extern bool in_startup_mode,anim_onscreen,play_sounds,frills_on,startup_loaded,cartoon_happening;
//extern short town_size[3];
//extern cParty party;
//extern pc_record_type adven[6];
//extern big_tr_type t_d;
//extern cOutdoors outdoors[2][2];
extern cUniverse univ;
//extern current_town_type c_town;
//extern town_item_list t_i;
//extern unsigned char out[96][96],out_e[96][96];
extern unsigned char combat_terrain[64][64];
extern effect_pat_type current_pat;
extern bool web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern bool sleep_field;
//extern unsigned char misc_i[64][64],sfx[64][64];
extern short on_monst_menu[256];
extern DialogPtr modeless_dialogs[18];
extern short monst_target[60]; // 0-5 target that pc   6 - no target  100 + x - target monster x
extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x

//extern piles_of_stuff_dumping_type *data_store;
extern GWorldPtr storage_gworld,terrain_screen_gworld,party_template_gworld,items_gworld,tiny_obj_gworld;
extern GWorldPtr fields_gworld,mixed_gworld;
extern short which_g_stored[STORED_GRAPHICS];
extern short wish_list[STORED_GRAPHICS];
extern short storage_status[STORED_GRAPHICS]; // 0 - empty 1 - in use 2 - there, not in use
extern short terrain_there[9][9];

extern Point ul;
extern location pc_pos[6],center;
extern short which_combat_type,pc_dir[6],current_pc;
extern bool monsters_going,anim_onscreen;

extern short pc_moves[6];
extern short num_targets_left;
extern location spell_targets[8];
extern short display_mode;

extern long anim_ticks;

extern short terrain_pic[256];
extern char spot_seen[9][9];
extern char out_trim[96][96],town_trim[64][64];
extern short monster_index[21];

extern bool supressing_some_spaces;
extern location ok_space[4];
extern bool can_draw_pcs;
extern cScenario scenario;
extern GWorldPtr spec_scen_g;

Rect boat_rects[4] = {{0,0,36,28}, {0,28,36,56},{0,56,36,84},{0,84,36,112}};
bool gave_no_g_error = false;

//unsigned char m_pic_index[200] = {////
//1,2,3,4,5,6,7,8,9,10,
//11,12,13,14,15,16,17,18,19,20,
//
//21,22,23,24,25, 26,27,28,29,30,
//31,32,33,34,35, 36,37,38,39,40,
//
//41,42,43,44,46,47,48,49,50,51,
//53,55,57,59,60,61,62,63,64,65,
//
//66,67,68,69,70, 71,72,73,74,75,
//76,77,78,79,81, 82,83,85,86,87,
//
//88,89,90,91,92, 93,94,95,96,97,
//98,99,100,101,102, 103,104,105,106,107,
//
//108,109,111,112,113,  114,116,117,118,119, //100
//120,122,123,125,127, 128,129,130,131,135,
//
//136,137,139,140,141,142,143,144,145,146,
//147,148,149,150,151,152,152,154,155,159,
//
//160,164,166,168,170,171,172,173,174,175,
//176,177,178,179,180,181,182,183,184,185,
//
//186,187,188,189,190,191,192,193,194,195,
//196,197,198,0,0,0,0,0,0,0,
//
//0,0,0,0,0,0,0,0,0,0,
//0,0,0,0,0,0,0,0,0,0};
// 
//unsigned char m_pic_index_x[200] = {
//
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,2,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//
//1,2,1,1,1,2,1,1,1,1, // 100
//2,1,1,1,1,1,1,1,2,1,
//1,2,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,2,1,
//
//2,2,2,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1};
//
//unsigned char m_pic_index_y[200] = {
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,2,
//2,2,2,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,2,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//
//1,1,1,1,1,1,1,1,1,1,
//1,1,2,2,1,1,1,1,2,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,2,1,
//2,1,1,2,1,1,1,1,1,1,
//
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1,
//1,1,1,1,1,1,1,1,1,1};

void draw_one_terrain_spot (short i,short j,short terrain_to_draw,short dest) ////
//short dest; // 0 - terrain gworld   1 - screen
// if terrain_to_draw is -1, do black
// if terrain_to_draw >= 1000, force to draw graphic which is terrain_to_draw - 1000
{
	Rect where_draw;
	Rect source_rect;
	GWorldPtr source_gworld;
	short anim_type = 0;
	GrafPtr old_port;
	location l;
	
	source_gworld = storage_gworld;
	
	l.x = i; l.y = j;
	if (supressing_some_spaces && (l != ok_space[0]) && (l != ok_space[1]) && (l != ok_space[2]) && (l != ok_space[3]))
		return;

	where_draw = calc_rect(i,j);
 	OffsetRect(&where_draw,13,13);
 	if (terrain_to_draw == -1) {
 		if ((cartoon_happening == false) && (terrain_there[i][j] == 300)) {
 			return;
 			}
 		terrain_there[i][j] = 300;
 		GetPort(&old_port);
 		SetPort(terrain_screen_gworld);
 		PaintRect(&where_draw);
  		SetPort(old_port);
		return;
 		}
 	////
	if (terrain_to_draw >= 10000) { // force using a specific graphic
 		if (terrain_there[i][j] == terrain_to_draw - 10000)
 			return;
 		terrain_there[i][j] = terrain_to_draw - 10000;
		source_rect = return_item_rect(terrain_to_draw - 10000);
		anim_type = -1;
		}
		else if (terrain_pic[terrain_to_draw] >= 2000) { // custom
			source_gworld = spec_scen_g;
			source_rect = get_custom_rect(terrain_pic[terrain_to_draw] - 2000 + (anim_ticks % 4));
			anim_type = 0;
			terrain_there[i][j] = -1;
			}
		else if (terrain_pic[terrain_to_draw] >= 1000) { // custom
			source_gworld = spec_scen_g;
			source_rect = get_custom_rect(terrain_pic[terrain_to_draw] - 1000);
			terrain_there[i][j] = -1;
			}
		else if (terrain_pic[terrain_to_draw] >= 400) { // animated
			source_rect = return_item_rect(terrain_pic[terrain_to_draw] + 600 + 100 * (anim_ticks % 4));
			terrain_there[i][j] = -1;
			anim_type = 0;
			}
			else {
				if (cartoon_happening == false) {
	 				if (terrain_there[i][j] == terrain_pic[terrain_to_draw]) {
		 				return;
 						}
 					terrain_there[i][j] = terrain_pic[terrain_to_draw];
 					}
				source_rect = return_item_rect(terrain_pic[terrain_to_draw]);
				anim_type = -1;
				}
	
		if (anim_type >= 0) 
		 {  
			if ((is_town()) || (is_out()))
				anim_onscreen = true;
			}
	
	if (dest == 0)
		rect_draw_some_item(source_gworld, source_rect, terrain_screen_gworld, where_draw, (unsigned char) 0, 0);	  
		else rect_draw_some_item(source_gworld, source_rect, terrain_screen_gworld, where_draw, (unsigned char) 0, 1);	  
}

void draw_monsters() ////
{
	short i,j = 0,k;
	short width,height;
	Rect source_rect,to_rect;
	location where_draw,store_loc;
	short picture_wanted;
	unsigned char ter;
	Rect monst_rects[4][4] = {{{0,0,36,28},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
		{{0,7,18,21},{18,7,36,21},{0,0,0,0},{0,0,0,0}},
		{{9,0,27,14},{9,14,27,28},{0,0,0,0},{0,0,0,0}},
		{{0,0,18,14},{0,14,18,28},{18,0,36,14},{18,14,36,28}}};

	if (is_out())
		for (i = 0; i < 10; i++) 
			if (univ.party.out_c[i].exists == true) {
				if ((point_onscreen(univ.party.p_loc, univ.party.out_c[i].m_loc) == true) &&
					(can_see(univ.party.p_loc, univ.party.out_c[i].m_loc,0) < 5)) {
					where_draw.x = univ.party.out_c[i].m_loc.x - univ.party.p_loc.x + 4;
					where_draw.y = univ.party.out_c[i].m_loc.y - univ.party.p_loc.y + 4;
					terrain_there[where_draw.x][where_draw.y] = -1;
					
					j = 0;
					while ((univ.party.out_c[i].what_monst.monst[j] == 0) && (j < 7)) {
						j++;
						}
					
					if (j == 7) univ.party.out_c[i].exists = false; // begin watch out
						else {
							picture_wanted = get_monst_picnum(univ.party.out_c[i].what_monst.monst[j]);
							} // end watch out
					
					if (univ.party.out_c[i].exists == true) {
						get_monst_dims(univ.party.out_c[i].what_monst.monst[j],&width,&height);
						if (picture_wanted >= 1000) {
							for (k = 0; k < width * height; k++) {
								source_rect = get_custom_rect(picture_wanted % 1000 +
								  ((univ.party.out_c[i].direction < 4) ? 0 : (width * height)) + k);
								to_rect = monst_rects[(width - 1) * 2 + height - 1][k];
								OffsetRect(&to_rect,13 + 28 * where_draw.x,13 + 36 * where_draw.y);
								rect_draw_some_item(spec_scen_g, source_rect, terrain_screen_gworld,to_rect, 1, 0); 			
								}					
							}
						if (picture_wanted < 1000) {
							for (k = 0; k < width * height; k++) {
								source_rect = get_monster_template_rect(univ.party.out_c[i].what_monst.monst[j],
								  (univ.party.out_c[i].direction < 4) ? 0 : 1,k);
								to_rect = monst_rects[(width - 1) * 2 + height - 1][k];
								OffsetRect(&to_rect,13 + 28 * where_draw.x,13 + 36 * where_draw.y);
								rect_draw_some_item(storage_gworld, source_rect, terrain_screen_gworld,to_rect, 1, 0); 			
								}
							}
					}
				}
			}
	if (is_town())
		for (i = 0; i < univ.town->max_monst(); i++)
		if ((univ.town.monst.dudes[i].active != 0) && (univ.town.monst.dudes[i].m_d.spec_skill != 11))
			if (party_can_see_monst(i)) {
				check_if_monst_seen(univ.town.monst.dudes[i].number);
				where_draw.x = univ.town.monst.dudes[i].m_loc.x - center.x + 4;
				where_draw.y = univ.town.monst.dudes[i].m_loc.y - center.y + 4;
				get_monst_dims(univ.town.monst.dudes[i].number,&width,&height);
				if (point_onscreen(center,univ.town.monst.dudes[i].m_loc) == true)
					play_see_monster_str(univ.town.monst.dudes[i].number);
				
				for (k = 0; k < width * height; k++) {
					store_loc = where_draw;
					store_loc.x += k % width;
					store_loc.y += k / width;
					// customize?
					if (univ.town.monst.dudes[i].m_d.picture_num >= 1000) {
						source_rect = get_custom_rect((univ.town.monst.dudes[i].m_d.picture_num % 1000) +
							 k + ((univ.town.monst.dudes[i].m_d.direction < 4) ? 0 : width * height)
							 + ((combat_posing_monster == i + 100) ? (2 * width * height) : 0));
						Draw_Some_Item(spec_scen_g, source_rect, terrain_screen_gworld, store_loc, 1, 0); 
						}
					if (univ.town.monst.dudes[i].m_d.picture_num < 1000) {
						source_rect = get_monster_template_rect(univ.town.monst.dudes[i].number,
						 ((univ.town.monst.dudes[i].m_d.direction < 4) ? 0 : 1) + ((combat_posing_monster == i + 100) ? 10 : 0),k);
						ter = univ.town->terrain(univ.town.monst.dudes[i].m_loc.x,univ.town.monst.dudes[i].m_loc.y);
						// in bed?
						if ((store_loc.x >= 0) && (store_loc.x < 9) && (store_loc.y >= 0) && (store_loc.y < 9) &&
							(scenario.ter_types[ter].picture == 143) && 
							((univ.town.monst.dudes[i].m_d.m_type < 7) 
							&& (univ.town.monst.dudes[i].m_d.m_type != 1) && (univ.town.monst.dudes[i].m_d.m_type != 2))
							&& ((univ.town.monst.dudes[i].active == 1) || (monst_target[i] == 6)) &&
							(width == 1) && (height == 1)) ////
							draw_one_terrain_spot((short) where_draw.x,(short) where_draw.y,10230,0);
							else Draw_Some_Item(storage_gworld, source_rect, terrain_screen_gworld, store_loc, 1, 0); 
							}
					}
				}
	if (is_combat()) {	
			for (i = 0; i < univ.town->max_monst(); i++)
				if ((univ.town.monst.dudes[i].active != 0) && (univ.town.monst.dudes[i].m_d.spec_skill != 11))
					if (((point_onscreen(center,univ.town.monst.dudes[i].m_loc) == true) && (cartoon_happening == true))
						|| (party_can_see_monst(i) == true)) {
						check_if_monst_seen(univ.town.monst.dudes[i].number);
						where_draw.x = univ.town.monst.dudes[i].m_loc.x - center.x + 4;
						where_draw.y = univ.town.monst.dudes[i].m_loc.y - center.y + 4;
						get_monst_dims(univ.town.monst.dudes[i].number,&width,&height);
						
						for (k = 0; k < width * height; k++) {
							store_loc = where_draw;
							store_loc.x += k % width;
							store_loc.y += k / width;
							// customize?
							if (univ.town.monst.dudes[i].m_d.picture_num >= 1000) {
								source_rect = get_custom_rect((univ.town.monst.dudes[i].m_d.picture_num % 1000) +
									 k + ((univ.town.monst.dudes[i].m_d.direction < 4) ? 0 : width * height)
									 	+ ((combat_posing_monster == i + 100) ? (2 * width * height) : 0));
								Draw_Some_Item(spec_scen_g, source_rect, terrain_screen_gworld, store_loc, 1, 0); 
								}
							if (univ.town.monst.dudes[i].m_d.picture_num < 1000) {
								source_rect = get_monster_template_rect(univ.town.monst.dudes[i].number,
								 ((univ.town.monst.dudes[i].m_d.direction < 4) ? 0 : 1) + ((combat_posing_monster == i + 100) ? 10 : 0)
								 ,k);
								ter = univ.town->terrain(univ.town.monst.dudes[i].m_loc.x,univ.town.monst.dudes[i].m_loc.y);
								if ((store_loc.x >= 0) && (store_loc.x < 9) && (store_loc.y >= 0) && (store_loc.y < 9) &&
									(scenario.ter_types[ter].picture == 143) && 
									((univ.town.monst.dudes[i].m_d.m_type < 7) 
										&& (univ.town.monst.dudes[i].m_d.m_type != 1) && (univ.town.monst.dudes[i].m_d.m_type != 2))
									&& ((univ.town.monst.dudes[i].active == 1) || (monst_target[i] == 6)) &&
									(width == 1) && (height == 1))
										draw_one_terrain_spot((short) where_draw.x,(short) where_draw.y,10230,0); ////
										else Draw_Some_Item(storage_gworld, source_rect, terrain_screen_gworld, store_loc, 1, 0); 
								}
							}
						}
	}
}

void play_see_monster_str(unsigned char m)
{}

void draw_pcs(location center,short mode)
//short mode; // 0 - put pcs in gworld  1 - only rectangle around active pc
{
	short i;
	Rect source_rect,active_pc_rect;
	location where_draw;
	
	if (party_toast() == true)
		return;
	if (can_draw_pcs == false)
		return;
	
	if ((mode == 1) && (cartoon_happening == true))
		return;
		
	for (i = 0; i < 6; i++)
		if (ADVEN[i].main_status == 1)
			if (((point_onscreen(center, pc_pos[i])) == true) && 
				((cartoon_happening == true) || (party_can_see(pc_pos[i]) < 6))){
				where_draw.x = pc_pos[i].x - center.x + 4;
				where_draw.y = pc_pos[i].y - center.y + 4;
				source_rect = get_party_template_rect(i,(pc_dir[i] < 4) ? 0 : 1);
				if (combat_posing_monster == i)
					OffsetRect(&source_rect,0,108);
					
				if (mode == 0) {
					Draw_Some_Item(party_template_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0); 			
					}
					
				if ((current_pc == i) && (mode == 1) && (monsters_going == false)) {
					active_pc_rect.top = 18 + where_draw.y * 36;
					active_pc_rect.left = 18 + where_draw.x * 28;
					active_pc_rect.bottom = 54 + where_draw.y * 36;
					active_pc_rect.right = 46 + where_draw.x * 28;
					OffsetRect(&active_pc_rect,ul.h,ul.v);
				
					ForeColor(magentaColor);
					FrameRoundRect(&active_pc_rect,8,8);
					ForeColor(blackColor);
					}
			}

	// Draw current pc on top
	if ( ((point_onscreen(center, pc_pos[current_pc])) == true) && (ADVEN[current_pc].main_status == 1)) {
		where_draw.x = pc_pos[current_pc].x - center.x + 4;
		where_draw.y = pc_pos[current_pc].y - center.y + 4;
		source_rect = get_party_template_rect(current_pc,(pc_dir[current_pc] < 4) ? 0 : 1);
				if (combat_posing_monster == current_pc)
					OffsetRect(&source_rect,0,108);
				
		if (mode == 0)
			Draw_Some_Item(party_template_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0); 			
		}
}


void draw_items()
{
	short i;
	Rect source_rect,dest_rect;
	location where_draw;
	
	
	for (i = 0; i < NUM_TOWN_ITEMS; i++) {
		if (univ.town.items[i].variety != 0) {
			where_draw.x = univ.town.items[i].item_loc.x - center.x + 4;
			where_draw.y = univ.town.items[i].item_loc.y - center.y + 4;

			if (supressing_some_spaces && (where_draw != ok_space[0]) && (where_draw != ok_space[1])
				&& (where_draw != ok_space[2]) && (where_draw != ok_space[3]));
			else if (point_onscreen(center, univ.town.items[i].item_loc) && !univ.town.items[i].is_contained() &&
			   (cartoon_happening || (party_can_see(univ.town.items[i].item_loc) < 6))) {
					// safety valve
					//if ((univ.town.items[i].graphic_num < 0) || 
					//	((univ.townt_i.items[i].graphic_num >= NUM_ITEM_G) && (univ.town.items[i].graphic_num < 1000))) {
					//	univ.town.items[i].variety = 0;
					//	univ.town.items[i].graphic_num = 0;
					//	}
					if (univ.town.items[i].graphic_num >= 150) {
						source_rect = get_custom_rect(univ.town.items[i].graphic_num - 150);
						dest_rect = coord_to_rect(where_draw.x,where_draw.y);
						terrain_there[where_draw.x][where_draw.y] = -1;

						rect_draw_some_item(spec_scen_g,
						 source_rect, terrain_screen_gworld, dest_rect, 1, 0); 
						}
						else {
							source_rect = get_item_template_rect(univ.town.items[i].graphic_num);
							dest_rect = coord_to_rect(where_draw.x,where_draw.y);
							terrain_there[where_draw.x][where_draw.y] = -1;
							if (univ.town.items[i].graphic_num >= 45) {
								dest_rect.top += 9;
								dest_rect.bottom -= 9;
								dest_rect.left += 5;
								dest_rect.right -= 5;
								}
							rect_draw_some_item((univ.town.items[i].graphic_num < 45) ? items_gworld : tiny_obj_gworld,
							 source_rect, terrain_screen_gworld, dest_rect, 1, 0); 
					 		}
					}
			}
		}
}

void draw_outd_boats(location center)
{
	location where_draw;
	Rect source_rect;
	short i;
	
	for (i = 0; i < 30; i++)
			if ((point_onscreen(center, univ.party.boats[i].loc) == true) && (univ.party.boats[i].exists == true) &&
				(univ.party.boats[i].which_town == 200) &&
				(can_see(center, univ.party.boats[i].loc,0) < 5) && (univ.party.in_boat != i)) {
				where_draw.x = univ.party.boats[i].loc.x - center.x + 4;
				where_draw.y = univ.party.boats[i].loc.y - center.y + 4;
				source_rect = boat_rects[0];
				OffsetRect(&source_rect,61,0);
				Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0); 
				}		
	for (i = 0; i < 30; i++)
			if ((point_onscreen(center, univ.party.horses[i].loc) == true) && (univ.party.horses[i].exists == true) &&
				(univ.party.horses[i].which_town == 200) &&
				(can_see(center, univ.party.horses[i].loc,0) < 5) && (univ.party.in_horse != i)) {
				where_draw.x = univ.party.horses[i].loc.x - center.x + 4;
				where_draw.y = univ.party.horses[i].loc.y - center.y + 4;
				source_rect = boat_rects[0];
				OffsetRect(&source_rect,61,0);
				OffsetRect(&source_rect,0,74);
				OffsetRect(&source_rect,56,36);
				Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0); 
				}		
}

void draw_town_boat(location center) 
{
	location where_draw;
	Rect source_rect;
	short i;
	
	for (i = 0; i < 30; i++)
		if ((univ.party.boats[i].which_town == univ.town.num) &&
		((point_onscreen(center, univ.party.boats[i].loc) == true) && 
		(can_see(center, univ.party.boats[i].loc,0) < 5) && (univ.party.in_boat != i)
		 && (pt_in_light(center,univ.party.boats[i].loc) == true))) {
		where_draw.x = univ.party.boats[i].loc.x - center.x + 4;
		where_draw.y = univ.party.boats[i].loc.y - center.y + 4;
		source_rect = boat_rects[0];
		OffsetRect(&source_rect,61,0);
		Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0); 
		}		
	for (i = 0; i < 30; i++)
		if ((univ.party.horses[i].which_town == univ.town.num) &&
		((point_onscreen(center, univ.party.horses[i].loc) == true) && 
		(can_see(center, univ.party.horses[i].loc,0) < 5) && (univ.party.in_horse != i)
		 && (pt_in_light(center,univ.party.horses[i].loc) == true))) {
		where_draw.x = univ.party.horses[i].loc.x - center.x + 4;
		where_draw.y = univ.party.horses[i].loc.y - center.y + 4;

		source_rect = boat_rects[0];
				OffsetRect(&source_rect,61,0);
		OffsetRect(&source_rect,0,74);
		OffsetRect(&source_rect,56,36);
		Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0); 
		}		
}

void draw_fields()
{
	if (crate == true) 
		draw_one_field(8,6,0);
	if (barrel == true)
		draw_one_field(16,7,0);
	if (web == true)
		draw_one_field(4,5,0);
	if (quickfire == true)
		draw_one_field(128,7,1);
	if (fire_barrier == true) 
		draw_one_field(32,anim_ticks % 4,2);
	if (force_barrier == true) 	
		draw_one_field(64,anim_ticks % 4,2);
}

void draw_spec_items()
{	
	if (force_wall == true)
		draw_one_spec_item(2,0,1);
	if (fire_wall == true)
		draw_one_spec_item(4,1,1);
	if (antimagic == true)
		draw_one_spec_item(8,2,1);
	if (scloud == true)
		draw_one_spec_item(16,3,1);	
	if (ice_wall == true)
		draw_one_spec_item(32,4,1);
	if (blade_wall == true)
		draw_one_spec_item(64,5,1);
	if (sleep_field == true)
		draw_one_spec_item(128,6,1);
}


void draw_sfx()
{
	short q,r,i,flag;
	location where_draw,loc;
	Rect orig_rect = {0,0,36,28},source_rect;
	
	if (PSD[SDF_NO_FRILLS] > 0)
		return;
	
		for (q = 0; q < 9; q++) 
			for (r = 0; r < 9; r++)
				{
				where_draw = center;
					where_draw.x += q - 4;
					where_draw.y += r - 4;
				
				if ((where_draw.x < 0) || (where_draw.x > univ.town->max_dim() - 1) 
						|| (where_draw.y < 0) || (where_draw.y > univ.town->max_dim() - 1)) 
							;
						else if (univ.town.sfx(where_draw.x,where_draw.y) != 0) {
							for (i = 0; i < 8; i++) {
								flag = s_pow(2,i);
								if (univ.town.sfx(where_draw.x,where_draw.y) & flag) 
									if (spot_seen[q][r] > 0) {
									loc.x = q;
										loc.y = r;
									source_rect = orig_rect;
									OffsetRect(&source_rect,28 * i,36 * 3);
									Draw_Some_Item(fields_gworld,source_rect,terrain_screen_gworld,loc,
									 1,0);
									}
								}
							}
				}
}


void draw_one_field(unsigned char flag,short source_x,short source_y)
{
	short q,r;
	location where_draw,loc;
	Rect orig_rect = {0,0,36,28},source_rect;
	
		for (q = 0; q < 9; q++) 
			for (r = 0; r < 9; r++)
				{
				where_draw = center;where_draw.x += q - 4;where_draw.y += r - 4;
				
				if ((where_draw.x < 0) || (where_draw.x > univ.town->max_dim() - 1) 
						|| (where_draw.y < 0) || (where_draw.y > univ.town->max_dim() - 1)) 
							;
						else {
							if (univ.town.misc_i(where_draw.x,where_draw.y) & flag) 
								if (spot_seen[q][r] > 0) {
								loc.x = q; loc.y = r;
								source_rect = orig_rect;
								OffsetRect(&source_rect,28 * source_x,36 * source_y);
								Draw_Some_Item(fields_gworld,source_rect,terrain_screen_gworld,loc,
								 1,0);
								if ((is_town()) && ((flag == 32) || (flag == 64)))
									anim_onscreen = true;		
								}
							}
				}
}

void draw_one_spec_item(unsigned char flag,short source_x,short source_y)
{
	short q,r;
	location where_draw,loc;
	Rect orig_rect = {0,0,36,28},source_rect;

		for (q = 0; q < 9; q++) 
			for (r = 0; r < 9; r++)
				{
				where_draw = center;
					where_draw.x += q - 4;
					where_draw.y += r - 4;
				
				if ((where_draw.x < 0) || (where_draw.x > univ.town->max_dim() - 1) 
						|| (where_draw.y < 0) || (where_draw.y > univ.town->max_dim() - 1)) 
							;
						else {
							if (univ.town.explored(where_draw.x,where_draw.y) & flag) 
								if (spot_seen[q][r] > 0) {
								loc.x = q; loc.y = r;
								source_rect = orig_rect;
								OffsetRect(&source_rect,28 * source_x,36 * source_y);
								Draw_Some_Item(fields_gworld,source_rect,terrain_screen_gworld,loc,
								 1,0);
								}
				}
			}
}


							
void draw_party_symbol(short mode,location center)
// mode currently unused
{
	Rect source_rect;
	location target(4,4);
	short i = 0;
	short dir_array[8] = {0,3,3,3,2,1,1,1};

	if (can_draw_pcs == false)
		return;
	if (party_toast() == true)
		return;
	if ((is_town()) && (univ.town.p_loc.x > 70))
		return;
	if (overall_mode == MODE_LOOK_TOWN) {
		target.x += univ.town.p_loc.x - center.x;
		target.y += univ.town.p_loc.y - center.y;
		}

	if ((univ.party.in_boat < 0) && (univ.party.in_horse < 0)) {////
			i = first_active_pc();
			source_rect = get_party_template_rect(i,(univ.party.direction < 4) ? 0 : 1);			

			// now wedge in bed graphic
			if ((is_town()) && (scenario.ter_types[univ.town->terrain(univ.town.p_loc.x,univ.town.p_loc.y)].picture == 143))
				draw_one_terrain_spot((short) target.x,(short) target.y,10230,0); ////
				else Draw_Some_Item(party_template_gworld, source_rect, terrain_screen_gworld, target, 1, 0);
		}
		else if (univ.party.in_boat >= 0) {
				source_rect = boat_rects[dir_array[univ.party.direction]];
				OffsetRect(&source_rect,61,0);
				Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, target, 1, 0); 
			}
			else {
				source_rect = boat_rects[(univ.party.direction < 4) ? 0 : 1];
				OffsetRect(&source_rect,61,0);
				OffsetRect(&source_rect,0,74);
				Draw_Some_Item(mixed_gworld, source_rect, terrain_screen_gworld, target, 1, 0); 			
				}
}



/* Input terrain currently trying to draw. Get back Rect in terrain template containing 
desired pixmap, or Rect to darkness if desired map not present */
Rect get_terrain_template_rect (unsigned char type_wanted) ////
{
	short picture_wanted;
	
	picture_wanted = terrain_pic[type_wanted];

	if (picture_wanted >= 400)
		return return_item_rect(picture_wanted + 700);
	return return_item_rect(picture_wanted);
}

Rect return_item_rect(short wanted)////
{
	Rect orig_rect = {0,0,36,28};
	short i;
	if (wanted < 50) {
		OffsetRect(&orig_rect,28 * (wanted % 10),36 * (wanted / 10));
		return orig_rect;
		}
	for (i = 50; i < STORED_GRAPHICS; i++)
		if (which_g_stored[i] == wanted) {
			OffsetRect(&orig_rect,28 * (i % 10),36 * (i / 10));
			return orig_rect;
			}
	// oops it's not here. better try to add it.
	add_one_graphic(wanted);
	for (i = 50; i < STORED_GRAPHICS; i++)
		if (which_g_stored[i] == wanted) {
			OffsetRect(&orig_rect,28 * (i % 10),36 * (i / 10));
			return orig_rect;
			}
	// oh well.
	if (gave_no_g_error == false) {
		give_error("Blades of Exile doesn't have enough memory to show all the graphics needed. Some things may look like cave floor. The game can still be played safely - restarting isn't necessary.","",0);
		gave_no_g_error = true;
		}
	return orig_rect;
}

// Give the position of the monster graphic in the picture resource
// Will store monsters the same in Exile's II and III
Rect get_monster_rect (unsigned char type_wanted,short mode) ////
//short mode; // 0 - left  1 - right  2 - both
{
	Rect store_rect;
	short i;
	
	i = (short) type_wanted;
	i = i % 20;
	store_rect.top = 0 + BITMAP_HEIGHT * (i % 10) ;
	store_rect.bottom = store_rect.top + BITMAP_HEIGHT;
	store_rect.left = 0 + (i / 10) * BITMAP_WIDTH * 2 + ((mode != 1) ? 0 : 28);
	store_rect.right = store_rect.left + BITMAP_WIDTH * ((mode == 2) ? 2 : 1);
	
	return store_rect;
}

// Give the position of the monster graphic in the template in memory
Rect get_monster_template_rect (unsigned char type_wanted,short mode,short which_part) ////
//mode; // 0 - left  1 - right  +10 - combat mode
{
	Rect store_rect = {0,0,36,28};
	short picture_wanted;
	short adj = 0;
	
	if (mode >= 10) {adj = 2000; mode -= 10;}
	picture_wanted = get_monst_picnum(type_wanted);
	if (picture_wanted >= 1000)
		return store_rect;
	picture_wanted = m_pic_index[picture_wanted].i + which_part;
	return return_item_rect(300 + picture_wanted + (300 * mode) + adj);
}




// Returns rect for drawing an item, if num < 25, rect is in big item template,
// otherwise in small item template
Rect get_item_template_rect (short type_wanted)////
{
	Rect store_rect;
	
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


unsigned char get_t_t(char x,char y)  // returns terrain type at where
{
	if (is_out())
		return univ.out.out[x][y];
		else if (is_town())
			return univ.town->terrain(x,y);
			else return combat_terrain[x][y];
}

// Is this is subterranean fluid that gets shore plopped down on it?
bool is_fluid(unsigned char ter_type)////
{

	if (((ter_type >= 71) && (ter_type <= 76)) || (ter_type == 90))
		return true;
	
	return false;
}

// Is this is subterranean beach that gets shore plopped down next to it?
bool is_shore(unsigned char ter_type)////
{
	if (is_fluid(ter_type) == true)
		return false;
	if (ter_type == 77)
		return false;
	if (ter_type == 90)
		return false;
/*	if (ter_type == 240)
		return false;
	if ((ter_type >= 117) && (ter_type <= 131))
		return false;
	if ((ter_type >= 193) && (ter_type <= 207))
		return false; */
	return true;
}

// These two functions used to determine wall round-cornering
bool is_wall(unsigned char ter_type)////
{
	short pic;

	pic = scenario.ter_types[ter_type].picture;
	
	if ((pic >= 88) && (pic <= 120))
		return true;
	
	return false;
}
bool is_ground(unsigned char ter_type)
{
	short pic;

	pic = scenario.ter_types[ter_type].picture;
	if ((pic >= 0) && (pic <= 87))
		return true;
	if ((pic >= 121) && (pic <= 122))
		return true;
	if ((pic >= 179) && (pic <= 208))
		return true;
	if ((pic >= 211) && (pic <= 212))
		return true;
	if ((pic >= 215) && (pic <= 246))
		return true;

	return false;
}

void make_town_trim(short mode)
//mode; // 0 - town 1 - outdoor combat
{
	location where;
	eGameMode store_mode;
	
	store_mode = overall_mode;
	overall_mode = (mode == 0) ? MODE_TOWN : MODE_COMBAT;
	for (where.x = 0; where.x < univ.town->max_dim(); where.x++)
		for (where.y = 0; where.y < univ.town->max_dim(); where.y++)
			town_trim[where.x][where.y] = add_trim_to_array(where,
			 (mode == 0) ? univ.town->terrain(where.x,where.y) : combat_terrain[where.x][where.y]);
	for (where.x = 0; where.x < univ.town->max_dim(); where.x++)
		for (where.y = 0; where.y < univ.town->max_dim(); where.y++) {
			if (town_trim[where.x][where.y] & 1)
				town_trim[where.x][where.y] &= 125;
			if (town_trim[where.x][where.y] & 4)
				town_trim[where.x][where.y] &= 245;
			if (town_trim[where.x][where.y] & 10)
				town_trim[where.x][where.y] &= 215;	
			if (town_trim[where.x][where.y] & 64)
				town_trim[where.x][where.y] &= 95;	
			}
	overall_mode = store_mode;			
}

void make_out_trim()
{
	location where;
	eGameMode store_mode;
	
	store_mode = overall_mode;
	overall_mode = MODE_OUTDOORS;

	for (where.x = 0; where.x < 96; where.x++)
		for (where.y = 0; where.y < 96; where.y++)
			out_trim[where.x][where.y] = add_trim_to_array(where,univ.out.out[where.x][where.y]);
	for (where.x = 0; where.x < 96; where.x++)
		for (where.y = 0; where.y < 96; where.y++) {
			if (out_trim[where.x][where.y] & 1)
				out_trim[where.x][where.y] &= 125;
			if (out_trim[where.x][where.y] & 4)
				out_trim[where.x][where.y] &= 245;
			if (out_trim[where.x][where.y] & 10)
				out_trim[where.x][where.y] &= 215;	
			if (out_trim[where.x][where.y] & 64)
				out_trim[where.x][where.y] &= 95;	
			}
	overall_mode = store_mode;
			
}

char add_trim_to_array(location where,unsigned char ter_type)
{
	bool at_top = false,at_bot = false,at_left = false,at_right = false;
	unsigned char store;
	char to_return = 0;
	
	if (where.x == 0)
		at_left = true;
	if (where.y == 0)
		at_top = true;
	if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS)) {
		if (where.x == 95)
			at_right = true;
		if (where.y == 95)
			at_bot = true;
		}
		else {
		if (where.x == univ.town->max_dim() - 1)
			at_right = true;
		if (where.y == univ.town->max_dim() - 1)
			at_bot = true;
		}
		
	// Set up trim for fluids
	if (is_fluid(ter_type) == true) {
		if (at_left == false) {
			store = get_t_t(where.x - 1,where.y);
			if (is_shore(store) == true)
				to_return |= 64;
			}
		if (at_right == false) {
			store = get_t_t(where.x + 1,where.y);
			if (is_shore(store) == true)
				to_return |= 4;
			}
		if (at_top == false) {
			store = get_t_t(where.x,where.y - 1);
			if (is_shore(store) == true)
				to_return |= 1;
			}	
		if (at_bot == false) {
			store = get_t_t(where.x,where.y + 1);
			if (is_shore(store) == true)
				to_return |= 16;
			}
		if ((at_left == false) && (at_top == false)) {
			store = get_t_t(where.x - 1,where.y - 1);
			if (is_shore(store) == true)
				to_return |= 128;
			}
		if ((at_right == false) && (at_top == false)) {
			store = get_t_t(where.x + 1,where.y + 1);
			if (is_shore(store) == true)
				to_return |= 8;
			}
		if ((at_right == false) && (at_bot == false)) {
			store = get_t_t(where.x + 1,where.y - 1);
			if (is_shore(store) == true)
				to_return |= 2;
			}
		if ((at_left == false) && (at_bot == false)) {
			store = get_t_t(where.x - 1,where.y + 1);
			if (is_shore(store) == true)
				to_return |= 32;
			}
		}

	return to_return;
}

// Sees if party has seen a monster of this sort, updates menu and gives
// special messages as necessary
void check_if_monst_seen(unsigned char m_num)
{
	// this rule has been changed
	return;
	if (univ.party.m_seen[m_num] == 0) {
		univ.party.m_seen[m_num] = 1;
		switch (m_num) {
		
			}
		adjust_monst_menu();
		}
}

void adjust_monst_menu()
{
	short i,monst_pos = 0;
	MenuHandle monst_menu;
	Str255 monst_name;

	if (in_startup_mode == true)
		return;
	monst_menu = GetMenuHandle(700);
	for (i = 0; i < 256; i++) {
		on_monst_menu[i] = -1; 
		}
	for (i = 1; i < 256; i++) 
		if ((i == 1) || (univ.party.m_seen[i] > 0)) {
			on_monst_menu[monst_pos] = i;
			monst_pos++;
			}

	for (i = 0; i < 256; i++) {
		DeleteMenuItem(monst_menu,1); 
		}
	for (i = 0; i < 256; i++) 	
		if (on_monst_menu[i] >= 0) {
			//GetIndString(monst_name, 2,on_monst_menu[i]);
			sprintf((char *) monst_name,"%s",scenario.scen_monsters[on_monst_menu[i]].m_name);
			c2pstr((char*)monst_name);
			AppendMenu(monst_menu,monst_name);
			}
}	
