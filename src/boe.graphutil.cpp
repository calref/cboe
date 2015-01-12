
#include <SFML/Graphics.hpp>

#include <cstdio>
#include <queue>

#include "boe.global.h"

#include "classes.h"
#include "boe.graphutil.h"
#include "boe.text.h"
#include "boe.locutils.h"
#include "boe.graphics.h"
#include "boe.infodlg.h"
#include "boe.monster.h"
#include "boe.specials.h"
#include "soundtool.hpp"
#include "graphtool.hpp"
#include "mathutil.hpp"
#include "dlogutil.hpp"

extern sf::RenderWindow mainPtr;
extern rectangle	windRect;
extern short stat_window;
extern bool give_delays;
extern eGameMode overall_mode;
extern short current_spell_range;
extern bool anim_onscreen,play_sounds,frills_on,startup_loaded;
extern cUniverse univ;
extern effect_pat_type current_pat;
extern sf::RenderWindow mini_map;
extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x

extern sf::RenderTexture terrain_screen_gworld;
extern sf::Texture items_gworld,tiny_obj_gworld,pc_gworld,monst_gworld[NUM_MONST_SHEETS];
extern sf::Texture fields_gworld,anim_gworld,vehicle_gworld,terrain_gworld[NUM_TER_SHEETS];
extern short terrain_there[9][9];
extern std::queue<pending_special_type> special_queue;

extern location ul;
extern location center;
extern short which_combat_type,current_pc;
extern bool monsters_going,anim_onscreen;

extern short num_targets_left;
extern location spell_targets[8];
extern short display_mode;

extern long anim_ticks;

extern char spot_seen[9][9];
extern short monster_index[21];

extern bool supressing_some_spaces;
extern location ok_space[4];
extern bool can_draw_pcs;
extern cCustomGraphics spec_scen_g;

// TODO: The duplication of rectangle here shouldn't be necessary...
rectangle boat_rects[4] = {rectangle{0,0,36,28}, rectangle{0,28,36,56},rectangle{0,56,36,84},rectangle{0,84,36,112}};
bool gave_no_g_error = false;
eAmbientSound ambient_sound;

//short dest; // 0 - terrain gworld   1 - screen
// if terrain_to_draw is -1, do black
// if terrain_to_draw >= 10000, force to draw graphic which is terrain_to_draw - 10000
void draw_one_terrain_spot (short i,short j,short terrain_to_draw) {
	rectangle where_draw;
	rectangle source_rect;
	sf::Texture* source_gworld;
	short anim_type = 0;
	location l;
	
	l.x = i; l.y = j;
	if(supressing_some_spaces && (l != ok_space[0]) && (l != ok_space[1]) && (l != ok_space[2]) && (l != ok_space[3]))
		return;
	
	where_draw = calc_rect(i,j);
 	where_draw.offset(13,13);
 	if(terrain_to_draw == -1) {
 		if(terrain_there[i][j] == 300) {
 			return;
		}
 		terrain_there[i][j] = 300;
		fill_rect(terrain_screen_gworld, where_draw, sf::Color::Black);
		return;
	}
 	
	if(terrain_to_draw >= 10000) { // force using a specific graphic
		terrain_to_draw -= 10000;
 		if(terrain_there[i][j] == terrain_to_draw)
 			return;
		source_gworld = &terrain_gworld[terrain_to_draw / 50];
 		terrain_there[i][j] = terrain_to_draw;
		terrain_to_draw %= 50;
		source_rect = calc_rect(terrain_to_draw % 10, terrain_to_draw / 10);
		anim_type = -1;
	}
	else if(univ.scenario.ter_types[terrain_to_draw].picture >= 2000) { // custom
		graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(univ.scenario.ter_types[terrain_to_draw].picture - 2000 + (anim_ticks % 4));
		anim_type = 0;
		terrain_there[i][j] = -1;
	}
	else if(univ.scenario.ter_types[terrain_to_draw].picture >= 1000) { // custom
		graf_pos_ref(source_gworld, source_rect) = spec_scen_g.find_graphic(univ.scenario.ter_types[terrain_to_draw].picture - 1000);
		terrain_there[i][j] = -1;
	}
	else if(univ.scenario.ter_types[terrain_to_draw].picture >= 960) { // animated
		source_gworld = &anim_gworld;
		terrain_to_draw = univ.scenario.ter_types[terrain_to_draw].picture;
		source_rect = calc_rect(4 * ((terrain_to_draw - 960) / 5) + (anim_ticks % 4),(terrain_to_draw - 960) % 5);
		terrain_there[i][j] = -1;
		anim_type = 0;
	}
	else {
		if(terrain_there[i][j] == univ.scenario.ter_types[terrain_to_draw].picture) return;
		terrain_there[i][j] = univ.scenario.ter_types[terrain_to_draw].picture;
		terrain_to_draw = univ.scenario.ter_types[terrain_to_draw].picture;
		source_gworld = &terrain_gworld[terrain_to_draw / 50];
 		terrain_there[i][j] = terrain_to_draw;
		terrain_to_draw %= 50;
		source_rect = calc_rect(terrain_to_draw % 10, terrain_to_draw / 10);
		anim_type = -1;
	}
	
	if(anim_type >= 0) {
		if((is_town()) || (is_out()))
			anim_onscreen = true;
	}
	
	rect_draw_some_item(*source_gworld, source_rect, terrain_screen_gworld, where_draw);
}

void draw_monsters() {
	short i,j = 0,k;
	short width,height;
	rectangle source_rect,to_rect;
	location where_draw,store_loc;
	short picture_wanted;
	ter_num_t ter;
	// TODO: The duplication of rectangle here shouldn't be necessary...
	rectangle monst_rects[4][4] = {
		{rectangle{0,0,36,28}},
		{rectangle{0,7,18,21},rectangle{18,7,36,21}},
		{rectangle{9,0,27,14},rectangle{9,14,27,28}},
		{rectangle{0,0,18,14},rectangle{0,14,18,28},rectangle{18,0,36,14},rectangle{18,14,36,28}}
	};
	
	if(is_out())
		for(i = 0; i < 10; i++)
			if(univ.party.out_c[i].exists) {
				if((point_onscreen(univ.party.p_loc, univ.party.out_c[i].m_loc)) &&
					(can_see_light(univ.party.p_loc, univ.party.out_c[i].m_loc,sight_obscurity) < 5)) {
					where_draw.x = univ.party.out_c[i].m_loc.x - univ.party.p_loc.x + 4;
					where_draw.y = univ.party.out_c[i].m_loc.y - univ.party.p_loc.y + 4;
					terrain_there[where_draw.x][where_draw.y] = -1;
					
					for(j = 0; univ.party.out_c[i].what_monst.monst[j] == 0 && j < 7; j++);
					
					if(j == 7) univ.party.out_c[i].exists = false; // begin watch out
					else {
						picture_wanted = get_monst_picnum(univ.party.out_c[i].what_monst.monst[j]);
					} // end watch out
					
					if(univ.party.out_c[i].exists) {
						get_monst_dims(univ.party.out_c[i].what_monst.monst[j],&width,&height);
						if(picture_wanted >= 1000) {
							for(k = 0; k < width * height; k++) {
								sf::Texture* src_gw;
								graf_pos_ref(src_gw, source_rect) = spec_scen_g.find_graphic(picture_wanted % 1000 +
																							 ((univ.party.out_c[i].direction < 4) ? 0 : (width * height)) + k);
								to_rect = monst_rects[(width - 1) * 2 + height - 1][k];
								to_rect.offset(13 + 28 * where_draw.x,13 + 36 * where_draw.y);
								rect_draw_some_item(*src_gw, source_rect, terrain_screen_gworld,to_rect, sf::BlendAlpha);
							}
						}
						if(picture_wanted < 1000) {
							for(k = 0; k < width * height; k++) {
								// TODO: Windows special-cases the bear and drake, whose graphics are split between two columns/sheets. Is this necessary?
								// It really doesn't look necessary to me, since each quadrant of the graphic is fetched separately.
								// Technically what they do is always pass 0 as the final argument to get_monster_template_rect, instead of passing k; they also hardcode the sheet to look on (4 for drake, 5 for bear).
								source_rect = get_monster_template_rect(picture_wanted,(univ.party.out_c[i].direction < 4) ? 0 : 1,k);
								to_rect = monst_rects[(width - 1) * 2 + height - 1][k];
								to_rect.offset(13 + 28 * where_draw.x,13 + 36 * where_draw.y);
								rect_draw_some_item(monst_gworld[m_pic_index[picture_wanted].i/20], source_rect, terrain_screen_gworld,to_rect, sf::BlendAlpha);
							}
						}
					}
				}
			}
	if(is_town() || is_combat()) {
		for(i = 0; i < univ.town->max_monst(); i++)
			if((univ.town.monst[i].active != 0) && (univ.town.monst[i].spec_skill != 11))
				if(point_onscreen(center,univ.town.monst[i].cur_loc) && party_can_see_monst(i)) {
					check_if_monst_seen(univ.town.monst[i].number, univ.town.monst[i].cur_loc);
					where_draw.x = univ.town.monst[i].cur_loc.x - center.x + 4;
					where_draw.y = univ.town.monst[i].cur_loc.y - center.y + 4;
					get_monst_dims(univ.town.monst[i].number,&width,&height);
					
					for(k = 0; k < width * height; k++) {
						store_loc = where_draw;
						store_loc.x += k % width;
						store_loc.y += k / width;
						ter = univ.town->terrain(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y);
						// in bed?
						if(store_loc.x >= 0 && store_loc.x < 9 && store_loc.y >= 0 && store_loc.y < 9 &&
						   (univ.scenario.ter_types[ter].special == eTerSpec::BED) && isHumanoid(univ.town.monst[i].m_type)
						   && (univ.town.monst[i].active == 1 || univ.town.monst[i].target == 6) &&
						   width == 1 && height == 1)
							draw_one_terrain_spot((short) where_draw.x,(short) where_draw.y,10000 + univ.scenario.ter_types[ter].flag1.u);
						else if(univ.town.monst[i].picture_num >= 1000) {
							bool isParty = univ.town.monst[i].picture_num >= 10000;
							sf::Texture* src_gw;
							pic_num_t need_pic = (univ.town.monst[i].picture_num % 1000) + k;
							if(univ.town.monst[i].direction >= 4) need_pic += width * height;
							if(combat_posing_monster == i + 100) need_pic += (2 * width * height);
							graf_pos_ref(src_gw, source_rect) = spec_scen_g.find_graphic(need_pic, isParty);
							Draw_Some_Item(*src_gw, source_rect, terrain_screen_gworld, store_loc, 1, 0);
						} else {
							pic_num_t this_monst = univ.town.monst[i].picture_num;
							int pic_mode = (univ.town.monst[i].direction) < 4 ? 0 : 1;
							pic_mode += (combat_posing_monster == i + 100) ? 10 : 0;
							source_rect = get_monster_template_rect(this_monst, pic_mode, k);
							Draw_Some_Item(monst_gworld[m_pic_index[this_monst].i/20], source_rect, terrain_screen_gworld, store_loc, 1, 0);
						}
					}
				}
	}
}

void play_see_monster_str(unsigned short m, location monst_loc) {
	short str1, str2, pic, snd, spec;
	ePicType type;
	str1 = univ.scenario.scen_monsters[m].see_str1;
	str2 = univ.scenario.scen_monsters[m].see_str2;
	pic = univ.scenario.scen_monsters[m].picture_num;
	type =  get_monst_pictype(m);
	snd = univ.scenario.scen_monsters[m].see_sound;
	spec = univ.scenario.scen_monsters[m].see_spec;
	// First display strings, if any
	if((str1 >= 0 && str1 < 100) || (str2 >= 0 && str2 < 100)) {
		short where1 = is_out() ? univ.party.outdoor_corner.x + univ.party.i_w_c.x : univ.town.num;
		short where2 = is_out() ? univ.party.outdoor_corner.y + univ.party.i_w_c.y : univ.town.num;
		std::string placename = is_out() ? univ.out->out_name : univ.town->town_name;
		cStrDlog display_strings(str1 < 100 ? univ.scenario.monst_strs[str1] : "", str2 < 100 ? univ.scenario.monst_strs[str2] : "", "", pic, type, NULL);
		display_strings.setSound(snd);
		display_strings.setRecordHandler(cStringRecorder(NOTE_MONST).string1(str1).string2(str2).from(where1,where2).at(placename));
		display_strings.show();
	}
	// Then run the special, if any
	if(spec > -1){
		queue_special(eSpecCtx::SEE_MONST, 0, spec, monst_loc);
	}
}

//short mode; // 0 - put pcs in gworld  1 - only rectangle around active pc
void draw_pcs(location center,short mode) {
	short i;
	rectangle source_rect,active_pc_rect;
	location where_draw;
	
	if(party_toast())
		return;
	if(!can_draw_pcs)
		return;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			if(point_onscreen(center, univ.party[i].combat_pos) &&
			   (/*cartoon_happening ||*/ party_can_see(univ.party[i].combat_pos) < 6)){
				where_draw.x = univ.party[i].combat_pos.x - center.x + 4;
				where_draw.y = univ.party[i].combat_pos.y - center.y + 4;
				source_rect = calc_rect(2 * (univ.party[i].which_graphic / 8), univ.party[i].which_graphic % 8);
				if(univ.party[i].dir >= 4)
					source_rect.offset(28,0);
				if(combat_posing_monster == i)
					source_rect.offset(0,288);
				
				if(mode == 0) {
					Draw_Some_Item(pc_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0);
				}
				
				if((current_pc == i) && (mode == 1) && !monsters_going) {
					active_pc_rect.top = 18 + where_draw.y * 36;
					active_pc_rect.left = 18 + where_draw.x * 28;
					active_pc_rect.bottom = 54 + where_draw.y * 36;
					active_pc_rect.right = 46 + where_draw.x * 28;
					active_pc_rect.offset(ul);
					
					frame_roundrect(mainPtr, active_pc_rect, 8, sf::Color::Magenta);
				}
			}
	
	// Draw current pc on top
	if(point_onscreen(center, univ.party[current_pc].combat_pos) && univ.party[current_pc].main_status == eMainStatus::ALIVE) {
		where_draw.x = univ.party[current_pc].combat_pos.x - center.x + 4;
		where_draw.y = univ.party[current_pc].combat_pos.y - center.y + 4;
		source_rect = calc_rect(2 * (univ.party[current_pc].which_graphic / 8), univ.party[current_pc].which_graphic % 8);
		if(univ.party[current_pc].dir >= 4)
			source_rect.offset(28,0);
		if(combat_posing_monster == current_pc)
			source_rect.offset(0,288);
		
		if(mode == 0)
			Draw_Some_Item(pc_gworld, source_rect, terrain_screen_gworld, where_draw, 1, 0);
	}
}

void draw_items(location where){
	if(!point_onscreen(center,where)) return;
	location where_draw(4 + where.x - center.x, 4 + where.y - center.y);
	rectangle from_rect, to_rect;
	if(supressing_some_spaces && (where != ok_space[0]) && (where != ok_space[1]) && (where != ok_space[2]) && (where != ok_space[3]))
		return;
	for(int i = 0; i < NUM_TOWN_ITEMS; i++) {
		if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where) {
			if(univ.town.items[i].contained) continue;
			if(party_can_see(where) >= 6) continue;
			if(univ.town.items[i].graphic_num >= 10000){
				sf::Texture* src_gw;
				graf_pos_ref(src_gw, from_rect) = spec_scen_g.find_graphic(univ.town.items[i].graphic_num - 10000, true);
				to_rect = coord_to_rect(where_draw.x,where_draw.y);
				terrain_there[where_draw.x][where_draw.y] = -1;
				rect_draw_some_item(*src_gw,from_rect,terrain_screen_gworld,to_rect,sf::BlendAlpha);
			}else if(univ.town.items[i].graphic_num >= 1000){
				sf::Texture* src_gw;
				graf_pos_ref(src_gw, from_rect) = spec_scen_g.find_graphic(univ.town.items[i].graphic_num - 1000);
				to_rect = coord_to_rect(where_draw.x,where_draw.y);
				terrain_there[where_draw.x][where_draw.y] = -1;
				rect_draw_some_item(*src_gw,from_rect,terrain_screen_gworld,to_rect,sf::BlendAlpha);
			}else{
				from_rect = get_item_template_rect(univ.town.items[i].graphic_num);
				to_rect = coord_to_rect(where_draw.x,where_draw.y);
				terrain_there[where_draw.x][where_draw.y] = -1;
				if(univ.town.items[i].graphic_num >= 55) {
					to_rect.inset(5,9);
					rect_draw_some_item(tiny_obj_gworld, from_rect, terrain_screen_gworld, to_rect,sf::BlendAlpha);
				}else
					rect_draw_some_item(items_gworld, from_rect, terrain_screen_gworld, to_rect,sf::BlendAlpha);
			}
		}
	}
}

void draw_outd_boats(location center) {
	location where_draw;
	rectangle source_rect;
	short i;
	
	for(i = 0; i < 30; i++)
		if((point_onscreen(center, univ.party.boats[i].loc)) && (univ.party.boats[i].exists) &&
			(univ.party.boats[i].which_town == 200) &&
			(can_see_light(center, univ.party.boats[i].loc,sight_obscurity) < 5) && (univ.party.in_boat != i)) {
			where_draw.x = univ.party.boats[i].loc.x - center.x + 4;
			where_draw.y = univ.party.boats[i].loc.y - center.y + 4;
			Draw_Some_Item(vehicle_gworld, calc_rect(0,0), terrain_screen_gworld, where_draw, 1, 0);
		}
	for(i = 0; i < 30; i++)
		if((point_onscreen(center, univ.party.horses[i].loc)) && (univ.party.horses[i].exists) &&
			(univ.party.horses[i].which_town == 200) &&
			(can_see_light(center, univ.party.horses[i].loc,sight_obscurity) < 5) && (univ.party.in_horse != i)) {
			where_draw.x = univ.party.horses[i].loc.x - center.x + 4;
			where_draw.y = univ.party.horses[i].loc.y - center.y + 4;
			Draw_Some_Item(vehicle_gworld, calc_rect(0,1), terrain_screen_gworld, where_draw, 1, 0);
		}
}

void draw_town_boat(location center) {
	location where_draw;
	rectangle source_rect;
	short i;
	
	for(i = 0; i < 30; i++)
		if((univ.party.boats[i].which_town == univ.town.num) &&
			((point_onscreen(center, univ.party.boats[i].loc)) &&
			 (can_see_light(center, univ.party.boats[i].loc,sight_obscurity) < 5) && (univ.party.in_boat != i)
			 && (pt_in_light(center,univ.party.boats[i].loc)))) {
				where_draw.x = univ.party.boats[i].loc.x - center.x + 4;
				where_draw.y = univ.party.boats[i].loc.y - center.y + 4;
				Draw_Some_Item(vehicle_gworld, calc_rect(1,0), terrain_screen_gworld, where_draw, 1, 0);
			}
	for(i = 0; i < 30; i++)
		if((univ.party.horses[i].which_town == univ.town.num) &&
			((point_onscreen(center, univ.party.horses[i].loc)) &&
			 (can_see_light(center, univ.party.horses[i].loc,sight_obscurity) < 5) && (univ.party.in_horse != i)
			 && (pt_in_light(center,univ.party.horses[i].loc)))) {
				where_draw.x = univ.party.horses[i].loc.x - center.x + 4;
				where_draw.y = univ.party.horses[i].loc.y - center.y + 4;
				Draw_Some_Item(vehicle_gworld, calc_rect(1,1), terrain_screen_gworld, where_draw, 1, 0);
			}
}

extern std::vector<location> forcecage_locs;

void draw_fields(location where){
	if(!point_onscreen(center,where)) return;
	if(party_can_see(where) >= 6) return;
	location where_draw(4 + where.x - center.x, 4 + where.y - center.y);
	if(is_out()){
		where = global_to_local(where);
		if(univ.out->special_spot[where.x][where.y])
			Draw_Some_Item(fields_gworld,calc_rect(4,0),terrain_screen_gworld,where_draw,1,0);
		return;
	}
	if(univ.town.is_force_wall(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(0,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_fire_wall(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(1,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_antimagic(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(2,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_scloud(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(3,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_ice_wall(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(4,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_blade_wall(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(5,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_sleep_cloud(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(6,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_block(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(3,0),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_spot(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(4,0),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_web(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(5,0),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_crate(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(6,0),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_barrel(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(7,0),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_fire_barr(where.x,where.y) || univ.town.is_force_barr(where.x,where.y))
		Draw_Some_Item(anim_gworld,calc_rect(8 + (anim_ticks % 4),4),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_quickfire(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(7,1),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_sm_blood(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(0,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_med_blood(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(1,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_lg_blood(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(2,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_sm_slime(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(3,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_lg_slime(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(4,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_ash(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(5,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_bones(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(6,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_rubble(where.x,where.y))
		Draw_Some_Item(fields_gworld,calc_rect(7,3),terrain_screen_gworld,where_draw,1,0);
	if(univ.town.is_force_cage(where.x,where.y)) {
		Draw_Some_Item(fields_gworld,calc_rect(1,0),terrain_screen_gworld,where_draw,1,0);
		forcecage_locs.push_back(where_draw);
	}
}

void draw_party_symbol(location center) {
	rectangle source_rect;
	location target(4,4);
	short i = 0;
	
	if(!can_draw_pcs)
		return;
	if(party_toast())
		return;
	if((is_town()) && (univ.town.p_loc.x > 70))
		return;
	if(overall_mode == MODE_LOOK_TOWN) {
		target.x += univ.town.p_loc.x - center.x;
		target.y += univ.town.p_loc.y - center.y;
	}
	
	if((univ.party.in_boat < 0) && (univ.party.in_horse < 0)) {
		i = first_active_pc();
		source_rect = calc_rect(2 * (univ.party[current_pc].which_graphic / 8), univ.party[i].which_graphic % 8);
		if(univ.party[current_pc].dir >= 4)
			source_rect.offset(28,0);
		ter_num_t ter = is_out() ? univ.out[univ.party.p_loc.x][univ.party.p_loc.y] : univ.town->terrain(univ.town.p_loc.x,univ.town.p_loc.y);
		// now wedge in bed graphic
		if(is_town() && univ.scenario.ter_types[ter].special == eTerSpec::BED)
			draw_one_terrain_spot((short) target.x,(short) target.y,10000 + univ.scenario.ter_types[ter].flag1.u);
		else Draw_Some_Item(pc_gworld, source_rect, terrain_screen_gworld, target, 1, 0);
	}
	else if(univ.party.in_boat >= 0) {
		if(univ.party.direction == 0 || univ.party.direction > 4) i = 4;
		else i = 3;
		Draw_Some_Item(vehicle_gworld, calc_rect(i,0), terrain_screen_gworld, target, 1, 0);
	}else {
		Draw_Some_Item(vehicle_gworld, calc_rect( univ.party.direction > 3 , 1), terrain_screen_gworld, target, 1, 0);
	}
}

// Give the position of the monster graphic in the picture resource
// Will store monsters the same in Exile's II and III
//short mode; // 0 - left  1 - right  2 - both
rectangle get_monster_rect (unsigned short type_wanted,short mode) {
	rectangle store_rect;
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
//mode; // 0 - left  1 - right  +10 - combat mode
rectangle get_monster_template_rect (pic_num_t picture_wanted,short mode,short which_part) {
	rectangle store_rect = {0,0,36,28};
	short adj = 0;
	
	if(mode >= 10) {
		adj += 4;
		mode -= 10;
	}
	if(mode == 0) adj++;
	picture_wanted = (m_pic_index[picture_wanted].i + which_part) % 20;
	return calc_rect(2 * (picture_wanted / 10) + adj, picture_wanted % 10);
}

// Returns rect for drawing an item, if num < 25, rect is in big item template,
// otherwise in small item template
rectangle get_item_template_rect (short type_wanted) {
	rectangle store_rect;
	
	if(type_wanted < 45) {
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

// Is this a fluid that gets shore plopped down on it?
bool is_fluid(ter_num_t ter_type) {
//	if(((ter_type >= 71) && (ter_type <= 76)) || (ter_type == 90))
//		return true;
//	return false;
	return univ.scenario.ter_types[ter_type].trim_type == eTrimType::FRILLS;
}

// Is this a beach that gets shore plopped down next to it?
bool is_shore(ter_num_t ter_type) {
	if(is_fluid(ter_type))
		return false;
	if(univ.scenario.ter_types[ter_type].trim_type == eTrimType::WATERFALL)
		return false;
//	if(ter_type == 77)
//		return false;
//	if(ter_type == 90)
//		return false;
/*	if(ter_type == 240)
		return false;
	if((ter_type >= 117) && (ter_type <= 131))
		return false;
	if((ter_type >= 193) && (ter_type <= 207))
		return false; */
	return true;
}

// These two functions used to determine wall round-cornering
bool is_wall(ter_num_t ter_type) {
	return univ.scenario.ter_types[ter_type].trim_type == eTrimType::WALL;
//	short pic;
//
//	pic = scenario.ter_types[ter_type].picture;
//
//	if((pic >= 88) && (pic <= 120))
//		return true;
//
//	return false;
}
bool is_ground(ter_num_t ter_type) {
	if(univ.scenario.ter_types[ter_type].trim_type == eTrimType::WALL)
		return false;
	if(univ.scenario.ter_types[ter_type].block_horse)
		return false;
//	if(scenario.ter_types[ter_type].trim_type == eTrimType::WALKWAY)
//		return false;
	return true;
}

char get_fluid_trim(location where,ter_num_t ter_type) {
	bool at_top = false,at_bot = false,at_left = false,at_right = false;
	ter_num_t store;
	char to_return = 0;
	
	if(where.x == 0)
		at_left = true;
	if(where.y == 0)
		at_top = true;
	if((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS)) {
		if(where.x == 95)
			at_right = true;
		if(where.y == 95)
			at_bot = true;
	}
	else {
		if(where.x == univ.town->max_dim() - 1)
			at_right = true;
		if(where.y == univ.town->max_dim() - 1)
			at_bot = true;
	}
	
	// Set up trim for fluids
	if(is_fluid(ter_type)) {
		if(!at_left) {
			store = coord_to_ter(where.x - 1,where.y);
			if(is_shore(store))
				to_return |= 64;
		}
		if(!at_right) {
			store = coord_to_ter(where.x + 1,where.y);
			if(is_shore(store))
				to_return |= 4;
		}
		if(!at_top) {
			store = coord_to_ter(where.x,where.y - 1);
			if(is_shore(store))
				to_return |= 1;
		}
		if(!at_bot) {
			store = coord_to_ter(where.x,where.y + 1);
			if(is_shore(store))
				to_return |= 16;
		}
		if((!at_left) && (!at_top)) {
			store = coord_to_ter(where.x - 1,where.y - 1);
			if(is_shore(store))
				to_return |= 128;
		}
		if((!at_right) && (!at_bot)) {
			store = coord_to_ter(where.x + 1,where.y + 1);
			if(is_shore(store))
				to_return |= 8;
		}
		if((!at_right) && (!at_top)) {
			store = coord_to_ter(where.x + 1,where.y - 1);
			if(is_shore(store))
				to_return |= 2;
		}
		if((!at_left) && (!at_bot)) {
			store = coord_to_ter(where.x - 1,where.y + 1);
			if(is_shore(store))
				to_return |= 32;
		}
	}
	if(to_return & 1)
		to_return &= 125;
	if(to_return & 4)
		to_return &= 245;
	if(to_return & 10)
		to_return &= 215;
	if(to_return & 64)
		to_return &= 95;
	
	return to_return;
}

// Sees if party has seen a monster of this sort, gives special messages as necessary
void check_if_monst_seen(unsigned short m_num, location at) {\
	// Give special messages if necessary
	if(m_num < 10000 && !univ.party.m_seen[m_num]) {
		univ.party.m_seen[m_num] = true;
		play_see_monster_str(m_num, at);
	}
	// Make the monster vocalize if applicable
	snd_num_t sound;
	if(m_num >= 10000)
		sound = univ.party.summons[m_num - 10000].ambient_sound;
	else sound = univ.scenario.scen_monsters[m_num].ambient_sound;
	if(get_ran(1,1,100) < 10) play_sound(-sound);
}

void play_ambient_sound(){ // TODO: Maybe add a system for in-town ambient sounds
	static const short drip[2] = {78,79}, bird[3] = {76,77,91};
	if(overall_mode != MODE_OUTDOORS) return; // ambient sounds are outdoors only at the moment
	if(get_ran(1,1,100) > 10) return; // 10% chance per move of playing a sound
	short sound_to_play;
	switch(ambient_sound){
		case AMBIENT_DRIP:
			sound_to_play = get_ran(1,0,1);
			play_sound(-drip[sound_to_play]);
			break;
		case AMBIENT_BIRD:
			sound_to_play = get_ran(1,0,2);
			play_sound(-bird[sound_to_play]);
			break;
		case AMBIENT_CUSTOM:
			sound_to_play = univ.out_sound;
			play_sound(-sound_to_play);
			break;
		case AMBIENT_NONE:
			break; // obviously, do nothing
	}
}


