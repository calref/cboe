
#include <cstring>
#include <cstdio>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"
#include "boe.graphics.h"
#include "boe.graphutil.h"
#include "boe.monster.h"
#include "boe.newgraph.h"
#include "boe.fileio.h"
#include "boe.itemdata.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "boe.text.h"
#include "soundtool.h"
#include "mathutil.h"
#include "graphtool.h"
#include "scrollbar.h"
#include <memory>
#include "location.h"

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
extern location ul;
extern RECT	windRect;
extern long anim_ticks;
extern RECT	bg[];
extern sf::RenderWindow mainPtr;
extern short town_type,which_combat_type;
extern eGameMode overall_mode;
extern bool play_sounds,boom_anim_active,in_startup_mode;
extern sf::Texture fields_gworld,boom_gworld,missiles_gworld,invenbtn_gworld;
extern sf::Texture tiny_obj_gworld, items_gworld, talkfaces_gworld;
extern sf::RenderTexture terrain_screen_gworld;
//extern party_record_type party;
extern sf::Texture bg_gworld;
extern RECT sbar_rect,item_sbar_rect,shop_sbar_rect;
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern location center;
extern short pc_marked_damage[6],pc_dir[6];
extern short monst_marked_damage[60];
extern location pc_pos[6];
//extern current_town_type univ.town;
//extern big_tr_type t_d;
//extern town_item_list	t_i;
extern ter_num_t combat_terrain[64][64];
//extern unsigned char misc_i[64][64],sfx[64][64];
extern location store_anim_ul;
extern char light_area[13][13];
extern short terrain_there[9][9];
extern char unexplored_area[13][13];
extern RECT bw_pats[6];
extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x
extern short store_talk_face_pic;
extern cScenario scenario;
extern cUniverse univ;
//extern talking_record_type talking;

// TODO: Arbitrary region support
std::shared_ptr<Region> oval_region,dark_mask_region,temp_rect_rgn;

// Talk vars
extern word_rect_type store_words[50];
extern eGameMode store_pre_talk_mode;
extern short store_personality,store_personality_graphic,current_pc;
extern sf::RenderTexture talk_gworld;
extern bool talk_end_forced;
extern std::string old_str1,old_str2,one_back1,one_back2;
extern word_rect_type preset_words[9];
extern RECT talk_area_rect, word_place_rect,talk_help_rect;
extern std::string title_string;
extern m_num_t store_monst_type;
//extern hold_responses store_resp[83];

// Shop vars
extern short store_shop_items[30],store_shop_costs[30];
extern short store_shop_type,store_shop_min,store_shop_max,store_cost_mult;
extern eGameMode store_pre_shop_mode;
extern char store_store_name[256];
extern RECT shopping_rects[8][7];
extern RECT bottom_help_rects[4];
extern RECT shop_name_str;
extern RECT shop_frame ;
extern RECT shop_done_rect;
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
RECT explode_place_rect[30];


// Animation vars
//extern town_record_type anim_town;
//extern tiny_tr_type anim_t_d;
//extern short anim_step;
//extern char anim_str[60];
//extern location anim_str_loc;
//
//// if < 6; target PC, if >= 100, targ monst, if 6, go to anim_pc_loc
//short anim_pc_targets[6];
//short anim_monst_targets[60];
//
//short anim_pcs[6];
//
//location anim_pc_locs[6],anim_monst_locs[60];
//location anim_string_loc;
//char anim_string[60];
char last_light_mask[13][13];

void apply_unseen_mask()
{
	// TODO: This is causing major lag and not even working properly
	return;
	RECT base_rect = {9,9,53,45},to_rect,big_to = {13,13,337,265};
	short i,j,k,l;
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
	
	for (i = 0; i < 11; i++)
		for (j = 0; j < 11; j++) 
			if (unexplored_area[i + 1][j + 1] == 1) {

					to_rect = base_rect;
				to_rect.offset(-28 + i * 28,-36 + 36 * j);
				to_rect |= big_to;
				tileImage(mainPtr, to_rect, bg_gworld, bw_pats[3], sf::BlendAlpha);
					//PaintRoundRect(&to_rect,4,4);
					for (k = i - 2; k < i + 1; k++)
						for (l = j - 2; l < j + 1; l++)
							if ((k >= 0) && (l >= 0) && (k < 9) && (l < 9) && ((k != i - 1) || (l != j - 1)))
								terrain_there[k][l] = -1;

				}
}
 
void apply_light_mask() 
{
	RECT temp = {0,0,108,84},paint_rect,base_rect = {0,0,36,28};
	RECT big_to = {13,13,337,265};
	short i,j;
	bool is_dark = false,same_mask = true;
	if (PSD[SDF_NO_FRILLS] > 0)
		return;
	if (is_out())
		return;
	if (univ.town->lighting_type == 0)
		return;
	
	if (oval_region == NULL) {
		temp_rect_rgn.reset(new Region);
		dark_mask_region.reset(new Region);
		oval_region.reset(new Region);
		oval_region->addEllipse(temp);
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
		fill_region(terrain_screen_gworld, *dark_mask_region, sf::Color::Black);
		mainPtr.setActive();
		return;
		}
	// TODO: Are these regions even used outside this function? If not, they should be local variables.
	dark_mask_region->clear();
	dark_mask_region->addRect(big_to);
	for (i = 0; i < 13; i++)
		for (j = 0; j < 13; j++) 
			last_light_mask[i][j] = light_area[i][j];
	for (i = 1; i < 12; i++)
		for (j = 1; j < 12; j++) {
			if (light_area[i][j] == 2) {
				int xOffset = 13 + 28 * (i - 3), yOffset = 13 + 36 * (j - 3);
				oval_region->offset(xOffset, yOffset);
				*dark_mask_region -= *oval_region;
				oval_region->offset(-xOffset, -yOffset);
				}
			if (light_area[i][j] == 3) {
				paint_rect = base_rect;
				paint_rect.offset(13 + 28 * (i - 2),13 + 36 * (j - 2));
				temp_rect_rgn->clear();
				temp_rect_rgn->addRect(paint_rect);
				*dark_mask_region -= *temp_rect_rgn;
				if (light_area[i + 1][j] == 3) light_area[i + 1][j] = 0;
				if (light_area[i + 1][j + 1] == 3) light_area[i + 1][j + 1] = 0;
				if (light_area[i][j + 1] == 3) light_area[i][j + 1] = 0;
				}
			}

	//rect_draw_some_item(light_mask_gworld,big_from,terrain_screen_gworld,big_to,0,0);
	fill_region(terrain_screen_gworld, *dark_mask_region, sf::Color::Black);

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
	// TODO: Get rid of temp_rect, at least
	RECT temp_rect,missile_origin_base = {1,1,17,17},active_area_rect,to_rect,from_rect;
	short i,store_missile_dir;
	location start_point,finish_point[30];
	location screen_ul;
	
	short x1[30],x2[30],y1[30],y2[30],t; // for path paramaterization
	RECT missile_place_rect[30],missile_origin_rect[30],store_erase_rect[30];
	location current_terrain_ul;
	sf::RenderTexture temp_gworld;
	
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
	if (in_startup_mode) {
		current_terrain_ul.x = 306;
		current_terrain_ul.y = 5;
	} else current_terrain_ul.x = current_terrain_ul.y = 5;
	
	// make terrain_template contain current terrain all nicely
	draw_terrain(1);
	to_rect = RECT(terrain_screen_gworld);
	RECT oldBounds = to_rect;
	to_rect.offset(current_terrain_ul);
	rect_draw_some_item(terrain_screen_gworld.getTexture(),oldBounds,to_rect,ul);
				
	// create and clip temporary anim template
	temp_rect = RECT(terrain_screen_gworld);
	active_area_rect = temp_rect;
	active_area_rect.inset(13,13);
	temp_gworld.create(temp_rect.width(), temp_rect.height());
	temp_gworld.setActive();
	clip_rect(temp_gworld, active_area_rect);
	mainPtr.setActive();
	
	
	// init missile paths
	for (i = 0; i < 30; i++) {
		store_erase_rect[i] = RECT();
		if ((store_missiles[i].missile_type >= 0) && (missile_origin == store_missiles[i].dest))
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
				missile_origin_rect[i].offset(18 * store_missile_dir,18 * store_missiles[i].missile_type);
				}
				else {
					missile_origin_rect[i] = missile_origin_base;
					missile_origin_rect[i].offset(0,18 * store_missiles[i].missile_type);
					}
			
			// x1 slope x2 start pt
			x1[i] = finish_point[i].x - start_point.x;
			x2[i] = start_point.x;
			y1[i] = finish_point[i].y - start_point.y;
			y2[i] = start_point.y;
			}
			else missile_place_rect[i] = RECT();
	
	play_sound(-1 * sound_num);
	
	
	// Now, at last, launch missile
	for (t = 0; t < num_steps; t++) {
		for (i = 0; i < 30; i++) 
			if (store_missiles[i].missile_type >= 0) {
				// Where place?
				temp_rect = missile_origin_base;
				temp_rect.offset(-8 + x2[i] + (x1[i] * t) / num_steps,
					-8 + y2[i] + (y1[i] * t) / num_steps);
				
				// now adjust for different paths
				if (store_missiles[i].path_type == 1)
				temp_rect.offset(0, -1 * (t * (num_steps - t)) / 100);
				
				missile_place_rect[i] = temp_rect | active_area_rect;
				
				// Now put terrain in temporary;
				rect_draw_some_item(terrain_screen_gworld.getTexture(),missile_place_rect[i],
					temp_gworld,missile_place_rect[i]);
				// Now put in missile
				from_rect = missile_origin_rect[i];
				if (store_missiles[i].missile_type >= 7) 
					from_rect.offset(18 * (t % 8),0);
				rect_draw_some_item(missiles_gworld,from_rect,
					temp_gworld,temp_rect,sf::BlendAlpha);
				}
		// Now draw all missiles to screen
		for (i = 0; i < 30; i++) 
			if (store_missiles[i].missile_type >= 0) {
				to_rect = store_erase_rect[i];
				to_rect.offset(current_terrain_ul);
				rect_draw_some_item(terrain_screen_gworld.getTexture(),store_erase_rect[i],to_rect,ul);
				
				to_rect = missile_place_rect[i];
				store_erase_rect[i] = to_rect;
				to_rect.offset(current_terrain_ul);
				rect_draw_some_item(temp_gworld.getTexture(),missile_place_rect[i],to_rect,ul);
				}
		mainPtr.display();
		if ((PSD[SDF_GAME_SPEED] == 3) || ((PSD[SDF_GAME_SPEED] == 1) && (t % 4 == 0)) ||
			((PSD[SDF_GAME_SPEED] == 2) && (t % 3 == 0)))
			sf::sleep(time_in_ticks(1));
		}
		
	// Exit gracefully, and clean up screen
	for (i = 0; i < 30; i++) 
		store_missiles[i].missile_type = -1;

	to_rect = RECT(terrain_screen_gworld);
	RECT oldRect = to_rect;
	to_rect.offset(current_terrain_ul);
	rect_draw_some_item(terrain_screen_gworld.getTexture(),oldRect,to_rect,ul);
}

short get_missile_direction(location origin_point,location the_point)
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

void do_explosion_anim(short sound_num,short special_draw)
// sound_num currently ignored
// special_draw - 0 normal 1 - first half 2 - second half
{
	RECT temp_rect,active_area_rect,to_rect,from_rect;
	RECT base_rect = {0,0,36,28},text_rect;
	char str[60];
	short i,temp_val,temp_val2;
	location screen_ul;
	
	short t,cur_boom_type = 0; 
	location current_terrain_ul; 
	sf::RenderTexture temp_gworld;
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
	if (in_startup_mode) {
		// TODO: I think this is a relic of the "demo" on the main screen of Exile III
		current_terrain_ul.x = 306;
		current_terrain_ul.y = 5;
	} else current_terrain_ul.x = current_terrain_ul.y = 5;
	
	// make terrain_template contain current terrain all nicely
	draw_terrain(1);
	if (special_draw != 2) {
		to_rect = RECT(terrain_screen_gworld);
		RECT oldRect = to_rect;
		to_rect.offset(current_terrain_ul);
		rect_draw_some_item(terrain_screen_gworld.getTexture(),oldRect,to_rect,ul);
		}
	
	// create and clip temporary anim template 
	temp_rect = RECT(terrain_screen_gworld);
	active_area_rect = temp_rect;
	active_area_rect.inset(13,13);
	// TODO: Deglobalize this
	TEXT.font = "Geneva";
	TEXT.style = sf::Text::Bold;
	TEXT.pointSize = 10;
	temp_gworld.create(temp_rect.width(), temp_rect.height());
	temp_gworld.setActive();
	clip_rect(temp_gworld, active_area_rect);
	mainPtr.setActive();
	
	// init missile paths
	screen_ul.x = center.x - 4; screen_ul.y = center.y - 4;
	for (i = 0; i < 30; i++) 
		if ((store_booms[i].boom_type >= 0)  && (special_draw < 2)) {
			cur_boom_type = store_booms[i].boom_type;
			explode_place_rect[i] = base_rect;
			explode_place_rect[i].offset(13 + 28 * (store_booms[i].dest.x - screen_ul.x) + store_booms[i].x_adj,
				13 + 36 * (store_booms[i].dest.y - screen_ul.y) + store_booms[i].y_adj);
				
			if ((store_booms[i].place_type == 1) && (special_draw < 2)) {
				temp_val = get_ran(1,0,50) - 25;
				temp_val2 = get_ran(1,0,50) - 25;
				explode_place_rect[i].offset(temp_val,temp_val2);
				}
			
			// eliminate stuff that's too gone. 
			RECT tempRect2;
			tempRect2 = RECT(terrain_screen_gworld);
			temp_rect = explode_place_rect[i] | tempRect2;
			if (temp_rect == explode_place_rect[i]) {
				store_booms[i].boom_type = -1;
				}
			
			}
			else if (special_draw < 2)
				explode_place_rect[i] = RECT();
	
	//play_sound(-1 * sound_num);
	if (special_draw < 2)
		play_sound(-1 * boom_type_sound[cur_boom_type]);
	
	// Now, at last, do explosion
	for (t = (special_draw == 2) ? 6 : 0; t < ((special_draw == 1) ? 6 : 11); t++) { // t goes up to 10 to make sure screen gets cleaned up
		// First, lay terrain in temporary graphic area;
		for (i = 0; i < 30; i++) 
			if (store_booms[i].boom_type >= 0) 
				rect_draw_some_item(terrain_screen_gworld.getTexture(),explode_place_rect[i],
					temp_gworld,explode_place_rect[i]);

		// Now put in explosions
		for (i = 0; i < 30; i++) 
			if (store_booms[i].boom_type >= 0) {
				if ((t + store_booms[i].offset >= 0) && (t + store_booms[i].offset <= 7)) {
						from_rect = base_rect;
						from_rect.offset(28 * (t + store_booms[i].offset),36 * (1 + store_booms[i].boom_type));
						rect_draw_some_item(boom_gworld,from_rect,
							temp_gworld,explode_place_rect[i],sf::BlendAlpha);
					
					if (store_booms[i].val_to_place > 0) {
						text_rect = explode_place_rect[i];
						text_rect.top += 4;
						text_rect.left -= 10;
						if (store_booms[i].val_to_place < 10)
							text_rect.left += 8;
						sprintf(str,"%d",store_booms[i].val_to_place);
						TEXT.colour = sf::Color::White;
						win_draw_string(temp_gworld,text_rect,str,1,12);
						TEXT.colour = sf::Color::Black;
						mainPtr.setActive();
						}
					}
				}
		// Now draw all missiles to screen
		for (i = 0; i < 30; i++) 
			if (store_booms[i].boom_type >= 0) {
				to_rect = explode_place_rect[i];
				to_rect.offset(current_terrain_ul);
				rect_draw_some_item(temp_gworld.getTexture(),explode_place_rect[i],to_rect,ul);
				}
		//if (((PSD[SDF_GAME_SPEED] == 1) && (t % 3 == 0)) || ((PSD[SDF_GAME_SPEED] == 2) && (t % 2 == 0)))
		mainPtr.display();
		sf::sleep(time_in_ticks(2 * (1 + PSD[SDF_GAME_SPEED])));
		}
		
	// Exit gracefully, and clean up screen
	for (i = 0; i < 30; i++) 
		if (special_draw != 1)
			store_booms[i].boom_type = -1;

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
void click_shop_rect(RECT area_rect)
{ 
	
	draw_shop_graphics(1,area_rect);
	mainPtr.display();
	if (play_sounds == true)
		play_sound(37);
	else sf::sleep(time_in_ticks(5));
	draw_shop_graphics(0,area_rect);

}

graf_pos calc_item_rect(int num,RECT& to_rect) {
	RECT from_rect = {0,0,18,18};
	sf::Texture *from_gw = &tiny_obj_gworld;
	if (num < 45) {
		from_gw = &items_gworld;
		from_rect = calc_rect(num % 5, num / 5);
	}else{
		to_rect.inset(5,9);
		from_rect.offset(18 * (num % 10), 18 * (num / 10));
	}
	return std::make_pair(from_gw, from_rect);
}

void draw_shop_graphics(bool pressed,RECT clip_area_rect)
// mode 1 - drawing dark for button press
{
	RECT area_rect,item_info_from = {11,42,24,56};
	
	RECT face_rect = {6,6,38,38};
	RECT title_rect = {15,48,42,260};
	RECT dest_rect,help_from = {85,36,101,54};
	short faces[13] = {1,1,1,42,43, 1,1,1,1,1, 44,44,44};
	
	short i,what_chosen;
	// In the 0..65535 range, these blue components were: 0, 32767, 14535, 26623, 59391
	// The green components on the second line were 40959 and 24575
	// TODO: The duplication of sf::Color here shouldn't be necessary...
	sf::Color c[7] = {sf::Color{0,0,0},sf::Color{0,0,128},sf::Color{0,0,57},sf::Color{0,0,104},sf::Color{0,0,232},
		sf::Color{0,160,0},sf::Color{0,96,0}};
	RECT shopper_name = {44,6,56,260};
	short current_pos;
	
	short cur_cost,what_magic_shop,what_magic_shop_item;
	char cur_name[60];
	char cur_info_str[60];
	static const char*const cost_strs[] = {"Extremely Cheap","Very Reasonable","Pretty Average","Somewhat Pricey",
	"Expensive","Exorbitant","Utterly Ridiculous"};
	cItemRec base_item;
	
	if (overall_mode != 21) {
		return;
		}
	
	talk_gworld.setActive();
	TEXT.font = "Dungeon";
	TEXT.pointSize = 18;
	TEXT.style = sf::Text::Regular;

	if (pressed) {
		talk_gworld.setActive();
		clip_rect(talk_gworld, clip_area_rect);
		}
	
	area_rect = RECT(talk_gworld);
	frame_rect(talk_gworld, area_rect, sf::Color::Black);
	area_rect.inset(1,1);
	tileImage(talk_gworld, area_rect,bg_gworld,bg[12]);

	frame_rect(talk_gworld, shop_frame, sf::Color::Black);
	
	// Place store icon
	if (!pressed) { 
		mainPtr.setActive();
		i = faces[store_shop_type];
		RECT from_rect = {0,0,32,32};
		from_rect.offset(32 * (i % 10),32 * (i / 10));
		rect_draw_some_item(talkfaces_gworld, from_rect, talk_gworld, face_rect);
		talk_gworld.setActive();
	}


	// Place name of store and shopper name
	TEXT.colour = c[3];
	dest_rect = title_rect;
	dest_rect.offset(1,1);
	win_draw_string(talk_gworld,dest_rect,store_store_name,2,18);
	dest_rect.offset(-1,-1);
	TEXT.colour = c[4];
	win_draw_string(talk_gworld,dest_rect,store_store_name,2,18);

	TEXT.font = "Geneva";
	TEXT.pointSize = 12;;
	TEXT.style = sf::Text::Bold;

	TEXT.colour = c[3];
	switch (store_shop_type) {
		case 3: sprintf(cur_name,"Healing for %s.",univ.party[current_pc].name.c_str()); break;
		case 10: sprintf(cur_name,"Mage Spells for %s.",univ.party[current_pc].name.c_str());break;
		case 11: sprintf(cur_name,"Priest Spells for %s.",univ.party[current_pc].name.c_str()); break;
		case 12: sprintf(cur_name,"Buying Alchemy.");break;
		case 4: sprintf(cur_name,"Buying Food.");break;
		default:sprintf(cur_name,"Shopping for %s.",univ.party[current_pc].name.c_str()); break;
		}
	win_draw_string(talk_gworld,shopper_name,cur_name,2,18);

	// Place help and done buttons
	TEXT.colour = sf::Color::Black;
	// TODO: Reimplement these with a cButton
#if 0
	help_from = RECT(dlg_buttons_gworld[3][0]); // help
	talk_help_rect.right = talk_help_rect.left + help_from.right - help_from.left;
	talk_help_rect.bottom = talk_help_rect.top + help_from.bottom - help_from.top;
	rect_draw_some_item(dlg_buttons_gworld[3][pressed],help_from,talk_gworld,talk_help_rect);
	help_from = RECT(dlg_buttons_gworld[11][0]); // done
	//talk_help_rect.right = talk_help_rect.left + help_from.right - help_from.left;
	//talk_help_rect.bottom = talk_help_rect.top + help_from.bottom - help_from.top;
	rect_draw_some_item(dlg_buttons_gworld[11][pressed],help_from,talk_gworld,shop_done_rect);
#endif
	
	if (pressed)
		TEXT.colour = c[4];
	else TEXT.colour = sf::Color::Black;
		
	// Place all the items
	for (i = 0; i < 8; i++) {
		current_pos = i + shop_sbar->getPosition();
		if (store_shop_items[current_pos] < 0)
			break; // theoretically, this shouldn't happen
		cur_cost = store_shop_costs[current_pos];
		what_chosen = store_shop_items[current_pos];
		RECT from_rect, to_rect = shopping_rects[i][2];
		sf::Texture* from_gw;
		switch (what_chosen / 100) {
			case 0: case 1: case 2: case 3: case 4: 
				base_item = get_stored_item(what_chosen);
				base_item.ident = true;
				graf_pos_ref(from_gw, from_rect) = calc_item_rect(base_item.graphic_num,to_rect);
				rect_draw_some_item(*from_gw, from_rect, talk_gworld, to_rect, sf::BlendAlpha);
				strcpy(cur_name,base_item.full_name.c_str());
				get_item_interesting_string(base_item,cur_info_str);
				break;
			case 5:
				base_item = store_alchemy(what_chosen - 500);
				graf_pos_ref(from_gw, from_rect) = calc_item_rect(53,to_rect);
				rect_draw_some_item(*from_gw, from_rect, talk_gworld, to_rect, sf::BlendAlpha);
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
				graf_pos_ref(from_gw, from_rect) = calc_item_rect(99,to_rect);
				rect_draw_some_item(*from_gw, from_rect, talk_gworld, to_rect, sf::BlendAlpha);
				strcpy(cur_name,heal_types[what_chosen]);
				strcpy(cur_info_str,"");
				break;
			case 8:
				base_item = store_mage_spells(what_chosen - 800 - 30);
				graf_pos_ref(from_gw, from_rect) = calc_item_rect(base_item.graphic_num,to_rect);
				rect_draw_some_item(*from_gw, from_rect, talk_gworld, to_rect, sf::BlendAlpha);

				strcpy(cur_name,base_item.full_name.c_str());
				strcpy(cur_info_str,"");		
				break;
			case 9:
				base_item = store_priest_spells(what_chosen - 900 - 30);
				graf_pos_ref(from_gw, from_rect) = calc_item_rect(base_item.graphic_num,to_rect);
				rect_draw_some_item(*from_gw, from_rect, talk_gworld, to_rect, sf::BlendAlpha);
				strcpy(cur_name,base_item.full_name.c_str());
				strcpy(cur_info_str,"");
				break;
			default:
				what_magic_shop = (what_chosen / 1000) - 1;
				what_magic_shop_item = what_chosen % 1000;
				base_item = univ.party.magic_store_items[what_magic_shop][what_magic_shop_item];
				base_item.ident = true;
				graf_pos_ref(from_gw, from_rect) = calc_item_rect(base_item.graphic_num,to_rect);
				rect_draw_some_item(*from_gw, from_rect, talk_gworld, to_rect, sf::BlendAlpha);
				strcpy(cur_name,base_item.full_name.c_str());
				get_item_interesting_string(base_item,cur_info_str);
				break;
			}

		// Now draw item shopping_rects[i][7]
		// 0 - whole area, 1 - active area 2 - graphic 3 - item name
		// 4 - item cost 5 - item extra str  6 - item help button
		TEXT.pointSize = 12;
		win_draw_string(talk_gworld,shopping_rects[i][3],cur_name,0,12);
		sprintf(cur_name,"Cost: %d",cur_cost);
		win_draw_string(talk_gworld,shopping_rects[i][4],cur_name,0,12);
		TEXT.pointSize = 10;
		win_draw_string(talk_gworld,shopping_rects[i][5],cur_info_str,0,12);
		if ((store_shop_type != 3) && (store_shop_type != 4))
			rect_draw_some_item(invenbtn_gworld,item_info_from,talk_gworld,shopping_rects[i][6],pressed ? sf::BlendNone : sf::BlendAlpha);

		}

	// Finally, cost info and help strs
	TEXT.pointSize = 12;
	sprintf(cur_name,"Prices here are %s.",cost_strs[store_cost_mult]);
	TEXT.pointSize = 10;
	win_draw_string(talk_gworld,bottom_help_rects[0],cur_name,0,12);
	win_draw_string(talk_gworld,bottom_help_rects[1],"Click on item name (or type 'a'-'h') to buy.",0,12);
	win_draw_string(talk_gworld,bottom_help_rects[2],"Hit done button (or Esc.) to quit.",0,12);
	if ((store_shop_type != 3) && (store_shop_type != 4))
		win_draw_string(talk_gworld,bottom_help_rects[3],"'I' button brings up description.",0,12);
	
	
	TEXT.colour = sf::Color::Black;
	undo_clip(talk_gworld);
	talk_gworld.display();

	refresh_shopping();
	shop_sbar->show();
	shop_sbar->draw();
}

void refresh_shopping()
{
	// TODO: The duplication of RECT here shouldn't be necessary...
	RECT from_rects[4] = {RECT{0,0,62,279},RECT{62,0,352,253},RECT{62,269,352,279},RECT{352,0,415,279}};
	RECT to_rect;
	short i;
	
	for (i = 0; i < 4; i++) {
		to_rect = from_rects[i];
		to_rect.offset(5,5);
		rect_draw_some_item(talk_gworld.getTexture(),from_rects[i],to_rect,ul);
		}
}

void click_talk_rect(std::string str_to_place,std::string str_to_place2,RECT c_rect)
{

	place_talk_str(str_to_place,str_to_place2,1,c_rect);
	mainPtr.display();
	if (play_sounds == true)
		play_sound(37);
	else sf::sleep(time_in_ticks(5));
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

	std::string str;
	
	if (which_s != minmax(0,31,which_s))
		which_s = 0;
	spell.item_level = which_s + 30;
	spell.value = cost[which_s];
	str = get_str("magic-names",which_s + 1);
	spell.full_name = str.c_str();
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
	std::string str;

	if (which_s != minmax(0,31,which_s))
		which_s = 0;
	spell.item_level = which_s + 30;
	spell.value = cost[which_s];
	str = get_str("magic-names",which_s + 50);
	spell.full_name = str.c_str();
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
	std::string str;
	
	if (which_s != minmax(0,19,which_s))
		which_s = 0;
	spell.item_level = which_s;
	spell.value = val[which_s];
	str = get_str("magic-names",which_s + 100);
	spell.full_name = str.c_str();
	return spell; 
}

void get_item_interesting_string(cItemRec item,char *message)
{
	if (item.property == true) {
		strcpy(message,"Not yours.");
		return;
		}
	if (item.ident == false) {
		strcpy(message,"");
		return;
		}
	if (item.cursed == true) {
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


void place_talk_str(std::string str_to_place,std::string str_to_place2,short color,RECT c_rect)
// color 0 - regular  1 - darker
{
	RECT area_rect;
	
	RECT face_rect = {6,6,38,38};
	RECT title_rect = {19,48,42,260};
	RECT dest_rect,help_from = {85,36,101,54};
	std::string str;
	sf::Text str_to_draw;
	static const char fn2[] = "Dungeon Bold";
	static const char fn3[] = "Palatino";

	short i,j,str_len,line_height = 17;
//	Str255 p_str,str,str_to_draw,str_to_draw2;
	short text_len[257],current_rect,store_last_word_break = 0,start_of_last_kept_word = -1;
	short last_line_break = 0,last_word_break = 0,on_what_line = 0,last_stored_word_break = 0;
	bool force_skip = false;
	short face_to_draw;
	
	// In the 0..65535 range, these blue components were: 0, 32767, 14535, 26623, 59391
	// The green components on the second line were 40959 and 24575
	// TODO: The duplication of sf::Color here shouldn't be necessary...
	sf::Color c[7] = {sf::Color{0,0,0},sf::Color{0,0,128},sf::Color{0,0,57},sf::Color{0,0,104},sf::Color{0,0,232},
		sf::Color{0,160,0},sf::Color{0,96,0}};
	
	talk_gworld.setActive();
		
	// TODO: All these TEXT.* things are used by win_draw_string, but this function is largely duplicating the functionality of that, so it won't work. Instead use setters on str_to_draw.
	TEXT.font = "Dungeon";
	TEXT.pointSize = 18;
	TEXT.style = sf::Text::Regular;

	if (c_rect.right > 0) {
		mainPtr.setActive();
		clip_rect(mainPtr, c_rect);
		}

	area_rect = RECT(talk_gworld);
	frame_rect(talk_gworld, area_rect, sf::Color::Black);
	area_rect.inset(1,1);
	tileImage(talk_gworld, area_rect,bg_gworld,bg[12]);

	// Put help button
	// TODO: Reimplement this with a cButton
#if 0
	help_from = RECT(dlg_buttons_gworld[3][0]); // help
	talk_help_rect.right = talk_help_rect.left + help_from.right - help_from.left;
	talk_help_rect.bottom = talk_help_rect.top + help_from.bottom - help_from.top;
	rect_draw_some_item(dlg_buttons_gworld[3][0],help_from,talk_gworld,talk_help_rect);
#endif
	
	// Place face of talkee
	if ((color == 0) && (c_rect.right == 0)) { 
		////
		mainPtr.setActive();
		face_to_draw = scenario.scen_monsters[store_monst_type].default_facial_pic;
		if (store_talk_face_pic >= 0)
			face_to_draw = store_talk_face_pic;
#if 0 // Note: The "false" parameter means "no frame"
		if (store_talk_face_pic >= 1000) {
			// TODO: Draw custom talk face graphic
			draw_dialog_graphic(  talk_gworld, face_rect, store_talk_face_pic, PIC_CUSTOM_TALK, false);
			}
			else {
				i = get_monst_picnum(store_monst_type);
				if (face_to_draw <= 0)
					// TODO: draw monster graphic for talk face
					draw_dialog_graphic(talk_gworld, face_rect, i, get_monst_pictype(store_monst_type), false);
					;
				// TODO: Draw preset talk face graphic
				else draw_dialog_graphic(talk_gworld, face_rect, face_to_draw, PIC_MONST, false);
			}
#endif
		talk_gworld.setActive();
		}
	// Place name oftalkee
	TEXT.colour = c[3];
	dest_rect = title_rect;
	dest_rect.offset(1,1);
	win_draw_string(talk_gworld,dest_rect,title_string.c_str(),2,18);
	dest_rect.offset(-1,-1);
	TEXT.colour = c[4];
	win_draw_string(talk_gworld,dest_rect,title_string.c_str(),2,18);
		
	// Place buttons at bottom.
	if (color == 0)
		TEXT.colour = c[5];
		else TEXT.colour = c[6];
	for (i = 0; i < 9; i++) 
		if ((talk_end_forced == false) || (i == 6) || (i == 5)) {
			preset_words[i].word_rect.offset(0,8);
			win_draw_string(talk_gworld,preset_words[i].word_rect,preset_words[i].word,2,18);
			preset_words[i].word_rect.offset(0,-8);
			}
	// Place bulk of what said. Save words.
	//TextSize(14);
	if (color == 0)
		for (i = 0; i < 50; i++)
			store_words[i].word_rect.left = store_words[i].word_rect.right = 0;
			
	str_len = str_to_place.length();
	if (str_len == 0) {
		str_to_place = ".";
		}
	str = str_to_place;
	for (i = 0; i < 257; i++) {
		text_len[i]= 0;
		char c = str[i];
		str[i] = 0;
		str_to_draw.setString(str);
		text_len[i] = str_to_draw.getLocalBounds().width;
		str[i] = c;
	}

	dest_rect = word_place_rect;

	current_rect = 0;
	
	if (color == 0)
		TEXT.colour = c[2];
	else TEXT.colour = c[1];
	// TODO: The clickable text doesn't work, and the entire text flashes when clicking anything
	win_draw_string(talk_gworld, dest_rect, str.c_str(), 0, line_height);
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
					
						// TODO: Debug line, remove
						frame_rect(talk_gworld, store_words[current_rect].word_rect, sf::Color::Red);
						}
				last_stored_word_break = i + 1;
				}
			}
		if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6)) 
		  && (last_word_break > last_line_break)) || (str[i] == '|') || (i == str_len - 1)) {
			if (str[i] == '|') {
				str[i] = ' ';
		 		last_word_break = i + 1;
	 			}
	 		store_last_word_break = last_word_break;
			last_line_break = last_word_break;
			if ((start_of_last_kept_word >= last_line_break) && (current_rect > 0)) {
				// TODO: Should we play an error sound here?
	 			store_words[current_rect - 1].word_rect.offset(5 - store_words[current_rect - 1].word_rect.left,line_height);
				}
		}
		if (str[i] == ' ') { // New word
			store_last_word_break = last_word_break = i + 1;
			}
		}
		
	// Now for string 2
	str_len = str_to_place2.length();
	start_of_last_kept_word = -1;
	
	if (str_len > 0) {
		
		str = str_to_place2;
		for (i = 0; i < 257; i++) {
			text_len[i]= 0;
			char c = str[i];
			str[i] = 0;
			str_to_draw.setString(str);
			text_len[i] = str_to_draw.getLocalBounds().width;
			str[i] = c;
		}
	
	last_line_break = store_last_word_break = last_word_break = last_stored_word_break = 0;
	win_draw_string(talk_gworld, dest_rect, str.c_str(), 0, line_height);
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
		 		last_word_break = i + 1;
	 			}
	 		store_last_word_break = last_word_break;
			last_line_break = last_word_break;
			if ((start_of_last_kept_word >= last_line_break) && (current_rect > 0)) {
	 			store_words[current_rect - 1].word_rect.offset(5 + -1 * store_words[current_rect - 1].word_rect.left,line_height);
				}
		}
		if (str[i] == ' ') { // New word
			store_last_word_break = last_word_break = i + 1;
			}
		}
	}
	
	TEXT.colour = sf::Color::Black;
	RECT oldRect(talk_gworld);
	undo_clip(talk_gworld);
	talk_gworld.display();
	
	// Finally place processed graphics
	mainPtr.setActive();
	rect_draw_some_item(talk_gworld.getTexture(),oldRect,talk_area_rect,ul);
	
	// Clean up strings
	for (i = 0; i < 50; i++)
		for (j = 0; j < 4; j++)
			if ((store_words[current_rect].word[j] >= 65) && (store_words[current_rect].word[j] <= 90))
				store_words[current_rect].word[j] += 32;

}

void refresh_talking()
{
	RECT tempRect(talk_gworld);
	rect_draw_some_item(talk_gworld.getTexture(),tempRect,talk_area_rect,ul);
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

