
#include <cstring>
#include <cstdio>
#include <vector>
#include <iostream>

#include "boe.global.hpp"

#include "universe/universe.hpp"
#include "boe.graphics.hpp"
#include "boe.graphutil.hpp"
#include "boe.monster.hpp"
#include "boe.newgraph.hpp"
#include "boe.fileio.hpp"
#include "boe.locutils.hpp"
#include "boe.text.hpp"
#include "boe.consts.hpp"
#include "boe.ui.hpp"
#include "sounds.hpp"
#include "mathutil.hpp"
#include "gfx/render_image.hpp"
#include "gfx/render_shapes.hpp"
#include "gfx/render_text.hpp"
#include "gfx/tiling.hpp"
#include "utility.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include <memory>
#include "location.hpp"
#include "scenario/shop.hpp"
#include "spell.hpp"
#include "dialogxml/widgets/button.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "tools/prefs.hpp"
#include "tools/enum_map.hpp"
#include "replay.hpp"
#include <boost/lexical_cast.hpp>

short monsters_faces[190] = {
	0,1,2,3,4,5,6,7,8,9,
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
extern rectangle	windRect;
extern long anim_ticks;
extern tessel_ref_t bg[];
extern short which_combat_type;
extern eGameMode overall_mode;
extern bool boom_anim_active;
extern rectangle sbar_rect,item_sbar_rect,shop_sbar_rect;
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern std::shared_ptr<cButton> done_btn, help_btn;
extern location center;
extern location store_anim_ul;
extern char light_area[13][13];
extern char unexplored_area[13][13];
extern tessel_ref_t bw_pats[6];
extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x
extern short store_talk_face_pic;
extern cUniverse univ;
extern cCustomGraphics spec_scen_g;
extern bool fog_lifted;
extern enum_map(eGuiArea, rectangle) win_to_rects;
extern sf::View mainView;

// Talk vars
extern eGameMode store_pre_talk_mode;
extern short store_personality,store_personality_graphic;
extern bool talk_end_forced;
extern std::string old_str1,old_str2,one_back1,one_back2;
extern rectangle talk_area_rect, word_place_rect;
extern std::string title_string;
extern mon_num_t store_monst_type;
std::vector<word_rect_t> talk_words;

// Shop vars
extern cShop active_shop;
extern eGameMode store_pre_shop_mode;
extern enum_map(eShopArea, rectangle) shopping_rects[8];
extern rectangle bottom_help_rects[4];
extern rectangle shop_name_str;
extern rectangle shop_frame ;
extern rectangle shop_done_rect;
extern char *heal_types[];
extern short heal_costs[8];
extern std::vector<int> shop_array;

// Missile anim vars
struct store_missile_type {
	location dest;
	miss_num_t missile_type; // -1 no miss
	short path_type,x_adj,y_adj;
};
struct store_boom_type {
	location dest;
	short val_to_place,offset;
	short place_type; // 0 - on spot, 1 - random area near spot
	short boom_type;  // -1 no miss
	short x_adj,y_adj;
};
store_missile_type store_missiles[30];
store_boom_type store_booms[30];
bool have_missile,have_boom;
rectangle explode_place_rect[30];

char last_light_mask[13][13];

terrain_screen_rects_t terrain_screen_rects() {
	rectangle from = rectangle(terrain_screen_gworld());

	location current_terrain_ul = win_to_rects[WINRECT_TERVIEW].topLeft();
	rectangle to = from;
	to.offset(current_terrain_ul);

	rectangle in_frame = to;
	in_frame.top += 11;
	in_frame.left += 11;
	in_frame.bottom -= 11;
	in_frame.right -= 11;

	return {from, to, in_frame};
}

void apply_unseen_mask() {
	rectangle base_rect = {9,9,53,45},to_rect,big_to = {13,13,337,265};
	bool need_bother = false;
	
	if(fog_lifted)
		return;
	
	if((is_combat()) && (which_combat_type == 0))
		return;
	if(!(is_out()) && (univ.town->lighting_type > 0))
		return;
	
	for(short i = 0; i < 11; i++)
		for(short j = 0; j < 11; j++)
			if(unexplored_area[i + 1][j + 1] == 1)
				need_bother = true;
	if(!need_bother)
		return;
	
	for(short i = 0; i < 11; i++)
		for(short j = 0; j < 11; j++)
			if(unexplored_area[i + 1][j + 1] == 1) {
				
				to_rect = base_rect;
				to_rect.offset(-28 + i * 28,-36 + 36 * j);
				to_rect &= big_to;
				tileImage(terrain_screen_gworld(), to_rect, bw_pats[3], sf::BlendAlpha);
			}
}

void apply_light_mask(bool onWindow) {
	static Region dark_mask_region;
	rectangle base_ellipse = {0,0,106,84},paint_rect,base_rect = {0,0,36,28};
	// I correct the values to make the display ok
	// but I am not sure what are the correct values
	rectangle big_to = {13+2,13+14,337+3,265+15};
	//rectangle big_to = {13,13,337,265};
	bool same_mask = true;
	if(fog_lifted)
		return;
	if(is_out() || overall_mode == MODE_RESTING)
		return;
	if(univ.town->lighting_type == 0)
		return;
	
	if(onWindow) {
		mainPtr().setActive(false);
		fill_region(mainPtr(), dark_mask_region, sf::Color::Black);
		mainPtr().setActive();
		return;
	}
	
	// Process the light array
	for(short i = 1; i < 12; i++)
		for(short j = 1; j < 12; j++)
			if((light_area[i - 1][j - 1] >= 1) && (light_area[i + 1][j - 1] >= 1) &&
				(light_area[i - 1][j] >= 1) && (light_area[i + 1][j] >= 1) &&
				(light_area[i - 1][j + 1] >= 1) && (light_area[i + 1][j + 1] >= 1) &&
				(light_area[i][j - 1] >= 1) && (light_area[i][j + 1] >= 1)) {
				light_area[i][j] = 2;
			}
	for(short i = 1; i < 12; i++)
		for(short j = 1; j < 12; j++)
			if((light_area[i - 1][j - 1] >= 2) && (light_area[i + 1][j - 1] >= 2) &&
				(light_area[i - 1][j] >= 2) && (light_area[i + 1][j] >= 2) &&
				(light_area[i - 1][j + 1] >= 2) && (light_area[i + 1][j + 1] >= 2) &&
				(light_area[i][j - 1] >= 2) && (light_area[i][j + 1] >= 2)) {
				light_area[i][j] = 3;
			}
	
	for(short i = 0; i < 13; i++)
		for(short j = 0; j < 13; j++)
			if(last_light_mask[i][j] != light_area[i][j])
				same_mask = false;
	
	if(same_mask) {
		return;
	}
	
	#ifdef DEBUG_LIGHT_MASK
	std::cout << "Current light mask:\n";
	for(short i = 0; i < 13; i++) {
		for(short j = 0; j < 13; j++)
			std::cout << int(light_area[j][i]) << ' ';
		std::cout << '\n';
	}
	#endif
	
	dark_mask_region.clear();
	dark_mask_region.addRect(big_to);
	for(short i = 0; i < 13; i++)
		for(short j = 0; j < 13; j++)
			last_light_mask[i][j] = light_area[i][j];
	for(short i = 1; i < 12; i++)
		for(short j = 1; j < 12; j++) {
			if(light_area[i][j] == 2) {
				int xOffset = 28 + 28 * (i - 3), yOffset = 16 + 36 * (j - 3);
				Region oval_region;
				oval_region.addEllipse(base_ellipse);
				oval_region.offset(xOffset, yOffset);
				dark_mask_region -= oval_region;
			}
			if(light_area[i][j] == 3) {
				paint_rect = base_rect;
				int xOffset = 13 + 28 * (i - 2), yOffset = 13 + 36 * (j - 2);
				paint_rect.offset(xOffset, yOffset);
				paint_rect.right += 28;
				paint_rect.bottom += 36;
				Region temp_rect_rgn;
				temp_rect_rgn.addRect(paint_rect);
				dark_mask_region -= temp_rect_rgn;
				if(light_area[i + 1][j] == 3) light_area[i + 1][j] = 0;
				if(light_area[i + 1][j + 1] == 3) light_area[i + 1][j + 1] = 0;
				if(light_area[i][j + 1] == 3) light_area[i][j + 1] = 0;
			}
		}
	
	dark_mask_region.offset(5,5);
}

void start_missile_anim() {
	if(boom_anim_active)
		return;
	boom_anim_active = true;
	for(short i = 0; i < 30; i++) {
		store_missiles[i].missile_type = -1;
		store_booms[i].boom_type = -1;
	}
	for(short i = 0; i < 6; i++)
		univ.party[i].marked_damage = 0;
	for(short i = 0; i < univ.town.monst.size(); i++)
		univ.town.monst[i].marked_damage = 0;
	have_missile = false;
	have_boom = false;
}

void end_missile_anim() {
	boom_anim_active = false;
}

void add_missile(location dest,miss_num_t missile_type,short path_type,short x_adj,short y_adj) {
	if(!boom_anim_active)
		return;
	// lose redundant missiles
	for(short i = 0; i < 30; i++)
		if((store_missiles[i].missile_type >= 0) && (dest == store_missiles[i].dest))
			return;
	for(short i = 0; i < 30; i++)
		if(store_missiles[i].missile_type < 0) {
			have_missile = true;
			store_missiles[i].dest = dest;
			store_missiles[i].missile_type =missile_type;
			store_missiles[i].path_type =path_type;
			store_missiles[i].x_adj =x_adj;
			store_missiles[i].y_adj =y_adj;
			return;
		}
}

void run_a_missile(location from,location fire_to,miss_num_t miss_type,short path,short sound_num,short x_adj,short y_adj,short len) {
	start_missile_anim();
	add_missile(fire_to,miss_type,path, x_adj, y_adj);
	do_missile_anim(len,from, sound_num);
	end_missile_anim();
}

void run_a_boom(location boom_where,short type,short x_adj,short y_adj,short snd) {
	start_missile_anim();
	add_explosion(boom_where,-1,0,type, x_adj, y_adj);
	do_explosion_anim(5,0,snd);
	end_missile_anim();
}

void mondo_boom(location l,short type,short snd) {
	start_missile_anim();
	for(short i = 0; i < 12; i++)
		add_explosion(l,-1,1,type,0,0);
	do_explosion_anim(5,0,snd);
	
	end_missile_anim();
}

void add_explosion(location dest,short val_to_place,short place_type,short boom_type,short x_adj,short y_adj, bool use_unique_ran) {
	if(!get_bool_pref("DrawTerrainFrills", true))
		return;
	if(!boom_anim_active)
		return;
	// lose redundant explosions
	for(short i = 0; i < 30; i++)
		if((store_booms[i].boom_type >= 0) && (dest == store_booms[i].dest)
			&& (place_type == 0)) {
			if(val_to_place > 0)
				store_booms[i].val_to_place = val_to_place;
			return;
		}
	for(short i = 0; i < 30; i++)
		if(store_booms[i].boom_type < 0) {
			have_boom = true;
			store_booms[i].offset = (i == 0) ? 0 : -1 * get_ran(1,0,2,use_unique_ran);
			store_booms[i].dest = dest;
			store_booms[i].val_to_place = val_to_place;
			store_booms[i].place_type = place_type;
			store_booms[i].boom_type =  boom_type;
			store_booms[i].x_adj =x_adj;
			store_booms[i].y_adj =y_adj;
			return;
		}
}

void do_missile_anim(short num_steps,location missile_origin,short sound_num) {
	// TODO: Get rid of temp_rect, at least
	rectangle temp_rect, missile_origin_base = {1,1,17,17},to_rect,from_rect;
	short store_missile_dir;
	location start_point,finish_point[30];
	location screen_ul;
	
	short x1[30],x2[30],y1[30],y2[30]; // for path paramaterization
	rectangle missile_place_rect[30],missile_origin_rect[30];
	location current_terrain_ul = win_to_rects[WINRECT_TERVIEW].topLeft();
	
	if(!have_missile || !boom_anim_active) {
		boom_anim_active = false;
		return;
	}
	
	// Eliminate missiles traveling 0 distance
	for(short i = 0; i < 30; i++) {
		if((store_missiles[i].missile_type >= 0) && (missile_origin == store_missiles[i].dest))
			store_missiles[i].missile_type = -1;
	}

	std::vector<location> missile_targets;
	std::vector<int> missile_indices;
	int tracking_missile = -1;
	location camera_dest;
	for(short i = 0; i < 30; i++)
		if(store_missiles[i].missile_type >= 0){
			missile_indices.push_back(i);
			missile_targets.push_back(store_missiles[i].dest);
		}

	if(missile_targets.empty()) return;

	if(missile_targets.size() == 1){
		tracking_missile = missile_indices[0];
		camera_dest = between_anchor_points(missile_targets[0], missile_origin);
	}else{
		std::vector<location> dest_candidates = points_containing_most(missile_targets);
		camera_dest = closest_point(dest_candidates, missile_origin);
		tracking_missile = missile_indices[closest_point_idx(missile_targets, camera_dest)];
	}

	// Start the camera as close as possible to containing the origin and the camera destination
	// on the same screen
	center = between_anchor_points(missile_origin, camera_dest);

	// make terrain_template contain current terrain all nicely
	draw_terrain(1);
	auto ter_rects = terrain_screen_rects();
	rect_draw_some_item(terrain_screen_gworld().getTexture(),ter_rects.from,mainPtr(),ter_rects.to);
	
	mainPtr().setActive(false);
	
	// init missile paths
	screen_ul.x = center.x - 4; screen_ul.y = center.y - 4;
	start_point.x = 13 + 14 + 28 * (short) (missile_origin.x - screen_ul.x);
	start_point.y = 13 + 18 + 36 * (short) (missile_origin.y - screen_ul.y);
	for(short i = 0; i < 30; i++)
		if(store_missiles[i].missile_type >= 0) {
			finish_point[i].x = 1 + 13 + 14 + store_missiles[i].x_adj + 28 * (short) (store_missiles[i].dest.x - screen_ul.x);
			finish_point[i].y = 1 + 13 + 18 + store_missiles[i].y_adj + 36 * (short) (store_missiles[i].dest.y - screen_ul.y);
			// note ... +1 at beginning is put in to prevent infinite slope
			
			if(store_missiles[i].missile_type < 7) {
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
		else missile_place_rect[i] = rectangle();
	
	play_sound(-1 * sound_num);
	
	sf::Texture& missiles_gworld = *ResMgr::graphics.get("missiles");
	bool recentered = false;
	int offset_x = 0;
	int offset_y = 0;
	// Now, at last, launch missile
	for(short t = 0; t < num_steps; t++) {
		// Temporarily switch to the original view to fill in the background
		mainPtr().setView(mainPtr().getDefaultView());
		put_background();
		mainPtr().setView(mainView);
		draw_terrain();
		UI::toolbar.draw(mainPtr());
		for(short i = 0; i < 30; i++)
			if(store_missiles[i].missile_type >= 0) {
				// Where place?
				temp_rect = missile_origin_base;
				temp_rect.offset(-8 + x2[i] + (x1[i] * t) / num_steps,
								 -8 + y2[i] + (y1[i] * t) / num_steps);
				temp_rect.offset(current_terrain_ul);
				temp_rect.offset(offset_x, offset_y);
				
				// now adjust for different paths
				if(store_missiles[i].path_type == 1)
					temp_rect.offset(0, -1 * (t * (num_steps - t)) / 100);
				
				missile_place_rect[i] = temp_rect;
				
				// Halfway through the missile's arc, or when the missile we're tracking goes off-screen, re-position the camera
				if(((t == num_steps / 2) || (tracking_missile == i && (missile_place_rect[i] & ter_rects.to) != missile_place_rect[i])) && !recentered){
					location old_center = center;

					center = camera_dest;

					// Offset the missile trajectory for the new camera position
					int dx = center.x - old_center.x;
					int dy = center.y - old_center.y;
					offset_x = -dx * 28;
					offset_y = -dy * 36;
					draw_terrain();

					recentered = true;

					// Redo this frame
					i--;
					continue;
				}

				// Now put in missile
				if(store_missiles[i].missile_type < 1000) {
					from_rect = missile_origin_rect[i];
					if(store_missiles[i].missile_type >= 7)
						from_rect.offset(18 * (t % 8),0);
					rect_draw_some_item(missiles_gworld,from_rect, mainPtr(),temp_rect,ter_rects.in_frame,sf::BlendAlpha);
				} else {
					// Custom missile graphics
					// TODO: Test this!
					int step = missile_origin_rect[i].left / 18;
					miss_num_t base = store_missiles[i].missile_type;
					bool isParty = false;
					if(base >= 10000) {
						isParty = true;
						base -= 10000;
					} else base -= 1000;
					base += step % 4;
					std::shared_ptr<const sf::Texture> from_gw = nullptr;
					graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(base, isParty);
					if(from_gw == nullptr) continue;
					from_rect.width() = 18;
					from_rect.height() = 18;
					if(step >= 4)
						from_rect.offset(0,18);
					rect_draw_some_item(*from_gw,from_rect, mainPtr(),temp_rect,ter_rects.in_frame,sf::BlendAlpha);
				}
			}
		refresh_text_bar();
		refresh_stat_areas(0);
		mainPtr().setActive();
		mainPtr().display();
		sf::sleep(sf::milliseconds(2 + 5 * get_int_pref("GameSpeed")));
	}

	// Exit gracefully, and clean up screen
	for(short i = 0; i < 30; i++)
		store_missiles[i].missile_type = -1;
	
	rect_draw_some_item(terrain_screen_gworld().getTexture(),ter_rects.from,mainPtr(),ter_rects.to);
}

short get_missile_direction(location origin_point,location the_point) {
	location store_dir;
	short dir = 0;
	// To reuse legacy code, will renormalize the_point, which is missile destination,
	// so that origin_point is moved to (149,185) and the_point is moved in proportion
	the_point.x +=  149 - origin_point.x;
	the_point.y +=  185 - origin_point.y;
	
	if((the_point.x < 135) & (the_point.y >= ((the_point.x * 34) / 10) - 293)
		& (the_point.y <= (-1 * ((the_point.x * 34) / 10) + 663)))
		store_dir.x--;
	if((the_point.x > 163) & (the_point.y <= ((the_point.x * 34) / 10) - 350)
		& (the_point.y >= (-1 * ((the_point.x * 34) / 10) + 721)))
		store_dir.x++;
	
	if((the_point.y < 167) & (the_point.y <= (the_point.x / 2) + 102)
		& (the_point.y <= (-1 * (the_point.x / 2) + 249)))
		store_dir.y--;
	if((the_point.y > 203) & (the_point.y >= (the_point.x / 2) + 123)
		& (the_point.y >= (-1 * (the_point.x / 2) + 268)))
		store_dir.y++;
	
	switch(store_dir.y) {
		case 0:
			dir = 4 - 2 * (store_dir.x); break;
		case -1:
			dir = (store_dir.x == -1) ? 7 : store_dir.x; break;
		case 1:
			dir = 4 - store_dir.x; break;
	}
	return dir;
}

// sound_num currently ignored
// special_draw - 0 normal 1 - first half 2 - second half
void do_explosion_anim(short /*sound_num*/,short special_draw, short snd) {
	rectangle active_area_rect,to_rect,from_rect;
	rectangle base_rect = {0,0,36,28},text_rect;
	short temp_val,temp_val2;
	location screen_ul;
	
	short cur_boom_type = 0;
	location current_terrain_ul = win_to_rects[WINRECT_TERVIEW].topLeft();;
	short boom_type_sound[6] = {5,10,53,53,53,75};
	
	if(!have_boom || !boom_anim_active) {
		boom_anim_active = false;
		return;
	}
	
	if(std::all_of(store_booms, store_booms + 30, [](const store_boom_type& b) {
		return b.boom_type == 0;
	})){
		return;
	}
	
	// make terrain_template contain current terrain all nicely
	draw_terrain(1);
	if(special_draw != 2) {
		auto ter_rects = terrain_screen_rects();
		rect_draw_some_item(terrain_screen_gworld().getTexture(),ter_rects.from,mainPtr(),ter_rects.to);
	}
	
	TextStyle style;
	style.font = FONT_BOLD;
	style.pointSize = 10;
	style.lineHeight = 10;
	mainPtr().setActive(false);
	
	// init missile paths
	screen_ul.x = center.x - 4; screen_ul.y = center.y - 4;
	for(short i = 0; i < 30; i++)
		if((store_booms[i].boom_type >= 0)  && (special_draw < 2)) {
			cur_boom_type = store_booms[i].boom_type;
			explode_place_rect[i] = base_rect;
			explode_place_rect[i].offset(13 + 28 * (store_booms[i].dest.x - screen_ul.x) + store_booms[i].x_adj,
										 13 + 36 * (store_booms[i].dest.y - screen_ul.y) + store_booms[i].y_adj);
			explode_place_rect[i].offset(current_terrain_ul);
			
			if((store_booms[i].place_type == 1) && (special_draw < 2)) {
				temp_val = get_ran(1,0,50) - 25;
				temp_val2 = get_ran(1,0,50) - 25;
				explode_place_rect[i].offset(temp_val,temp_val2);
			}
		}
		else if(special_draw < 2)
			explode_place_rect[i] = rectangle();
	
	if(special_draw < 2) {
		snd_num_t snd_num = snd;
		if(snd == -1 && cur_boom_type < 6)
			snd_num = boom_type_sound[cur_boom_type];
		play_sound(-1 * snd_num);
	}
	
	sf::Texture& boom_gworld = *ResMgr::graphics.get("booms");
	auto ter_rects = terrain_screen_rects();
	// Now, at last, do explosion
	for(short t = (special_draw == 2) ? 6 : 0; t < ((special_draw == 1) ? 6 : 11); t++) { // t goes up to 10 to make sure screen gets cleaned up
		// Temporarily switch to the original view to fill in the background
		mainPtr().setView(mainPtr().getDefaultView());
		put_background();
		mainPtr().setView(mainView);
		draw_terrain();
		UI::toolbar.draw(mainPtr());
		
		// Now put in explosions
		for(short i = 0; i < 30; i++){
			if(store_booms[i].boom_type >= 0) {
				if((t + store_booms[i].offset >= 0) && (t + store_booms[i].offset <= 7)) {
					if(cur_boom_type >= 1000) {
						std::shared_ptr<const sf::Texture> src_gworld;
						graf_pos_ref(src_gworld, from_rect) = spec_scen_g.find_graphic(cur_boom_type - 1000 + t);
						rect_draw_some_item(*src_gworld, from_rect, mainPtr(), explode_place_rect[i], ter_rects.in_frame, sf::BlendAlpha);
					} else {
						from_rect = base_rect;
						from_rect.offset(28 * (t + store_booms[i].offset),36 * (1 + store_booms[i].boom_type));
						rect_draw_some_item(boom_gworld,from_rect,mainPtr(),explode_place_rect[i], ter_rects.in_frame, sf::BlendAlpha);
					}
					
					if(store_booms[i].val_to_place > 0 && (explode_place_rect[i] & ter_rects.in_frame) == explode_place_rect[i]) {
						text_rect = explode_place_rect[i];
						text_rect.top += 13;
						text_rect.height() = 10;
						std::string dam_str = std::to_string(store_booms[i].val_to_place);
						style.colour = Colours::WHITE; 
						text_rect.offset(-1,-1);
						win_draw_string(mainPtr(),text_rect,dam_str,eTextMode::CENTRE,style);
						text_rect.offset(2,2);
						win_draw_string(mainPtr(),text_rect,dam_str,eTextMode::CENTRE,style);
						style.colour = Colours::BLACK; 
						text_rect.offset(-1,-1);
						win_draw_string(mainPtr(),text_rect,dam_str,eTextMode::CENTRE,style);
						mainPtr().setActive();
					}
				}
			}
		}
		//if(((PSD[SDF_GAME_SPEED] == 1) && (t % 3 == 0)) || ((PSD[SDF_GAME_SPEED] == 2) && (t % 2 == 0)))
		refresh_stat_areas(0);
		refresh_text_bar();
		mainPtr().setActive();
		mainPtr().display();
		sf::sleep(time_in_ticks(2 * (1 + get_int_pref("GameSpeed"))));
	}
	
	// Exit gracefully, and clean up screen
	for(short i = 0; i < 30; i++)
		if(special_draw != 1)
			store_booms[i].boom_type = -1;
}

void click_shop_rect(rectangle area_rect, bool item_help) {
	if(recording){
		std::string action_name = item_help ? "click_shop_item_help" : "click_shop_item";
		record_action(action_name, boost::lexical_cast<std::string>(area_rect));
	}

	draw_shop_graphics(!item_help,item_help,area_rect);
	mainPtr().display();
	play_sound(37, time_in_ticks(5));
	draw_shop_graphics(false,false,area_rect);

}

graf_pos calc_item_rect(int num,rectangle& to_rect) {
	if(num >= 1000) return spec_scen_g.find_graphic(num - 1000);
	rectangle from_rect = {0,0,18,18};
	std::shared_ptr<const sf::Texture> from_gw;
	if(num < 55) {
		from_gw = &ResMgr::graphics.get("objects");
		from_rect = calc_rect(num % 5, num / 5);
	}else{
		from_gw = &ResMgr::graphics.get("tinyobj");
		to_rect.inset(5,9);
		from_rect.offset(18 * (num % 10), 18 * (num / 10));
	}
	return std::make_pair(from_gw, from_rect);
}

// mode 1 - drawing dark for button press
void draw_shop_graphics(bool item_pressed, bool item_help_pressed, rectangle clip_area_rect) {
	rectangle area_rect,item_info_from = {12,42,24,56};
	
	rectangle face_rect = {6,6,38,38};
	rectangle title_rect = {15,48,42,260};
	rectangle dest_rect,help_from = {85,36,101,54};
	rectangle shopper_name = {44,6,56,260};
	long current_pos;
	
	short cur_cost;
	static const char*const cost_strs[] = {
		"Extremely Cheap","Very Reasonable","Pretty Average","Somewhat Pricey",
		"Expensive","Exorbitant","Utterly Ridiculous"};
	cItem base_item;
	
	if(overall_mode != MODE_SHOPPING) {
		return;
	}
	
	TextStyle style;
	style.font = FONT_DUNGEON;
	style.pointSize = 18;
	
	area_rect = rectangle(talk_gworld());
	talk_gworld().setActive(false);

	// Only re-render on top of the item that is clicked:
	if(item_pressed || item_help_pressed) {
		clip_area_rect.offset(-area_rect.left, -area_rect.top);
		clip_rect(talk_gworld(), clip_area_rect);
	} else {
		clear_scale_aware_text(talk_gworld());
		frame_rect(talk_gworld(), area_rect, Colours::BLACK);
		area_rect.inset(1,1);
		tileImage(talk_gworld(), area_rect,bg[12]);

		frame_rect(talk_gworld(), shop_frame, Colours::BLACK);
	}
	
	// Place store icon:
	// We can skip this when rendering for a button press, but it won't matter anyway
	// because button press rendering is clipped anyway.
	if(!item_pressed || item_help_pressed) {
		rectangle from_rect = {0,0,32,32};
		std::shared_ptr<const sf::Texture> from_gw;
		int i = std::max<int>(0, active_shop.getFace());
		if(i >= 1000) {
			graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(i - 1000);
		} else {
			from_rect.offset(32 * (i % 10),32 * (i / 10));
			from_gw = &ResMgr::graphics.get("talkportraits");
		}
		rect_draw_some_item(*from_gw, from_rect, talk_gworld(), face_rect);
	}
	
	
	// Place name of store and shopper name
	style.colour = Colours::SHADOW; 
	style.lineHeight = 18;
	dest_rect = title_rect;
	dest_rect.offset(1,1);
	win_draw_string(talk_gworld(),dest_rect,active_shop.getName(),eTextMode::LEFT_TOP,style);
	dest_rect.offset(-1,-1);
	style.colour = Colours::TITLE_BLUE; 
	win_draw_string(talk_gworld(),dest_rect,active_shop.getName(),eTextMode::LEFT_TOP,style);
	
	style.font = FONT_BOLD;
	style.pointSize = 12;
	
	style.colour = Colours::SHADOW;
	std::ostringstream title;
	switch(active_shop.getPrompt()) {
		case eShopPrompt::HEALING:
			title << "Healing for " << univ.current_pc().name << '.';
			break;
		case eShopPrompt::MAGE:
			title << "Mage Spells for " << univ.current_pc().name << '.';
			break;
		case eShopPrompt::PRIEST:
			title << "Priest Spells for " << univ.current_pc().name << '.';
			break;
		case eShopPrompt::SPELLS:
			title << "Spells for " << univ.current_pc().name << '.';
			break;
		case eShopPrompt::TRAINING:
			title << "Training for " << univ.current_pc().name << '.';
			break;
		case eShopPrompt::ALCHEMY:
			title << "Buying Alchemy.";
			break;
		case eShopPrompt::SHOPPING:
			title << "Shopping for " << univ.current_pc().name << '.';
			break;
	}
	win_draw_string(talk_gworld(),shopper_name,title.str(),eTextMode::LEFT_TOP,style);
	
	if(item_pressed)
		style.colour = Colours::TITLE_BLUE; 
	else 
		style.colour = Colours::BLACK; 
	
	sf::Texture& invenbtn_gworld = *ResMgr::graphics.get("invenbtns");
	// Place all the items
	for(short i = 0; i < 8; i++) {
		current_pos = i + shop_sbar->getPosition();
		if(current_pos >= shop_array.size() || shop_array[current_pos] < 0)
			break; // theoretically, the second condition shouldn't happen
		cShopItem item = active_shop.getItem(shop_array[current_pos]);
		eSpell spell;
		cur_cost = item.getCost(active_shop.getCostAdjust());
		base_item = item.item;
		std::string cur_name = base_item.full_name, cur_info_str;
		rectangle from_rect, to_rect = shopping_rects[i][SHOPRECT_GRAPHIC];
		std::shared_ptr<const sf::Texture> from_gw;
		switch(item.type) {
			case eShopItemType::ITEM:
				base_item.ident = true;
				cur_info_str = base_item.interesting_string();
				break;
			case eShopItemType::ALCHEMY: {
				const cAlchemy& info = *(eAlchemy(base_item.item_level));
				cur_info_str = get_str("item-abilities", int(info.ingred1) + 1);
				if(info.ingred2 != eItemAbil::NONE) {
					cur_info_str += " & " + get_str("item-abilities", int(info.ingred2) + 1);
				}
			} break;
			case eShopItemType::MAGE_SPELL:
				spell = cSpell::fromNum(eSkill::MAGE_SPELLS, base_item.item_level);
				cur_info_str = "Level: " + std::to_string((*spell).level) + "    SP: " + std::to_string((*spell).cost);
				break;
			case eShopItemType::PRIEST_SPELL:
				spell = cSpell::fromNum(eSkill::PRIEST_SPELLS, base_item.item_level);
				cur_info_str = "Level: " + std::to_string((*spell).level) + "    SP: " + std::to_string((*spell).cost);
				break;
			case eShopItemType::SKILL:
				cur_info_str = "Increase skill by 1";
				break;
			default: // Healing
				cur_info_str = "";
				break;
		}

		// Now draw item
		graf_pos_ref(from_gw, from_rect) = calc_item_rect(base_item.graphic_num,to_rect);
		rect_draw_some_item(*from_gw, from_rect, talk_gworld(), to_rect, sf::BlendAlpha);
		
		// Draw item key
		style.pointSize = 10;
		style.lineHeight = 10;
		std::string key(1, (char)('a' + i));
		win_draw_string(talk_gworld(),shopping_rects[i][SHOPRECT_KEY],key, eTextMode::WRAP,style);

		// Now draw item text
		style.pointSize = 12;
		style.lineHeight = 12;
		win_draw_string(talk_gworld(),shopping_rects[i][SHOPRECT_ITEM_NAME],cur_name,eTextMode::WRAP,style);
		cur_name = std::to_string(cur_cost);
		rectangle cost_rect = shopping_rects[i][SHOPRECT_ITEM_COST];
		cost_rect.left = cost_rect.right - string_length(cur_name, style) - 10;
		win_draw_string(talk_gworld(),cost_rect,cur_name,eTextMode::WRAP,style);
		// Draw the coin
		cost_rect.left = cost_rect.right - 7;
		cost_rect.top += 3;
		cost_rect.height() = 7;
		rect_draw_some_item(invenbtn_gworld, {0, 29, 7, 36}, talk_gworld(), cost_rect, sf::BlendAlpha);
		style.pointSize = 10;
		// Alchemy ingredients often don't fit
		win_draw_string(talk_gworld(),shopping_rects[i][SHOPRECT_ITEM_EXTRA],cur_info_str,eTextMode::ELLIPSIS,style);
		rect_draw_some_item(invenbtn_gworld,item_info_from,talk_gworld(),shopping_rects[i][SHOPRECT_ITEM_HELP],item_help_pressed ? sf::BlendNone : sf::BlendAlpha);
	}
	
	// Finally, cost info and help strs
	clear_sstr(title);
	title << "Prices here are " << cost_strs[active_shop.getCostAdjust()] << '.';
	style.pointSize = 10;
	style.lineHeight = 12;
	win_draw_string(talk_gworld(),bottom_help_rects[0],title.str(),eTextMode::WRAP,style);
	win_draw_string(talk_gworld(),bottom_help_rects[1],"Click on item name (or type 'a'-'h') to buy.",eTextMode::WRAP,style);
	win_draw_string(talk_gworld(),bottom_help_rects[2],"Hit done button (or Esc.) to quit.",eTextMode::WRAP,style);
	win_draw_string(talk_gworld(),bottom_help_rects[3],"'I' button brings up description.",eTextMode::WRAP,style);
	
	undo_clip(talk_gworld());
	talk_gworld().setActive();
	talk_gworld().display();
	
	refresh_shopping();
	if(shop_sbar->getMaximum() > 0)
		shop_sbar->show();
	else shop_sbar->hide();
	done_btn->show();
	done_btn->draw();
	help_btn->show();
	help_btn->draw();
}

void refresh_shopping() {
	rectangle from_rect(talk_gworld());
	rectangle to_rect = from_rect;
	to_rect.offset(talk_gword_offset_x, talk_gword_offset_y);
	rect_draw_some_item(talk_gworld(),from_rect,mainPtr(),to_rect);
}

static void place_talk_face() {
	rectangle face_rect = {6,6,38,38};
	face_rect.offset(talk_area_rect.topLeft());
	mainPtr().setActive();
	short face_to_draw = univ.scenario.scen_monsters[store_monst_type].default_facial_pic;
	if(store_talk_face_pic >= -1)
		face_to_draw = store_talk_face_pic;
	if(store_talk_face_pic >= 1000) {
		cPict::drawAt(mainPtr(), face_rect, store_talk_face_pic, PIC_CUSTOM_TALK, false);
	}
	else {
		if(face_to_draw == NO_PIC) {
			short i = get_monst_picnum(store_monst_type);
			cPict::drawAt(mainPtr(), face_rect, i, get_monst_pictype(store_monst_type), false);
		} else cPict::drawAt(mainPtr(), face_rect, face_to_draw, PIC_TALK, false);
	}
}

void click_talk_rect(word_rect_t word) {
	if(recording){
		record_click_talk_rect(word, word.on == PRESET_WORD_ON);
	}

	rectangle talkRect(talk_gworld()), wordRect(word.rect);
	mainPtr().setActive();
	rect_draw_some_item(talk_gworld(),talkRect,mainPtr(),talk_area_rect);
	wordRect.offset(talk_area_rect.topLeft());
	wordRect.width() += 10; // Arbitrary extra space fixes #481 and shouldn't cause any problems
	TextStyle style;
	style.font = FONT_DUNGEON;
	style.pointSize = 18;
	style.lineHeight = 18;
	style.colour = word.on;
	win_draw_string(mainPtr(), wordRect, word.word, eTextMode::WRAP, style);
	place_talk_face();
	help_btn->draw();
	mainPtr().display();
	play_sound(37, time_in_ticks(5));
	rect_draw_some_item(talk_gworld(),talkRect,mainPtr(),talk_area_rect);
	place_talk_face();
}

extern std::vector<std::string> preset_words;

// color 0 - regular  1 - darker
void place_talk_str(std::string str_to_place,std::string str_to_place2,short color,rectangle c_rect) {
	rectangle area_rect;
	
	rectangle title_rect = {19,48,42,260};
	rectangle dest_rect,help_from = {46,60,59,76};
	
	talk_gworld().setActive(false);
	
	TextStyle style;
	style.font = FONT_DUNGEON;
	style.pointSize = TALK_WORD_SIZE;
	
	if(c_rect.right > 0) {
		mainPtr().setActive(false);
		c_rect.offset(talk_area_rect.topLeft());
		clip_rect(mainPtr(), c_rect);
	}
	
	area_rect = rectangle(talk_gworld());
	clear_scale_aware_text(talk_gworld());
	frame_rect(talk_gworld(), area_rect, sf::Color::Black);
	area_rect.inset(1,1);
	tileImage(talk_gworld(), area_rect,bg[12]);
	
	// Place name oftalkee
	style.colour = Colours::SHADOW; 
	style.lineHeight = 18;
	dest_rect = title_rect;
	dest_rect.offset(1,1);
	win_draw_string(talk_gworld(),dest_rect,title_string,eTextMode::LEFT_TOP,style);
	dest_rect.offset(-1,-1);
	style.colour = Colours::TITLE_BLUE; 
	win_draw_string(talk_gworld(),dest_rect,title_string,eTextMode::LEFT_TOP,style);
	
	// Place buttons at bottom.
	if(color == 0)
		style.colour = PRESET_WORD_OFF;
	else 
		style.colour = PRESET_WORD_ON;
	int i = 0;
	for(std::string preset : preset_words) {
		if(talk_words[i].word == preset){
			win_draw_string(talk_gworld(),talk_words[i].rect,talk_words[i].word,eTextMode::LEFT_TOP,style);
			i++;
		}
	}
	
	style.colour = Colours::NAVY; 
	// First determine the offsets of clickable words.
	// The added spaces ensure that end-of-word boundaries are found
	std::string str = str_to_place + " |" + str_to_place2 + " ";
	std::vector<hilite_t> hilites;
	std::vector<int> nodes;
	int wordStart = 0, wordEnd = 0;
	if(!talk_end_forced){
		for(size_t i = 0; i < str.length(); i++) {
			char c = str[i];
			if(isalpha(c) || c == '-' || c == '\'') {
				if(wordStart <= wordEnd) wordStart = i;
			} else if(wordEnd <= wordStart) {
				wordEnd = i;
				short node = scan_for_response(str.c_str() + wordStart);
				if(node >= 0) {
					nodes.push_back(node);
					hilites.push_back({wordStart, wordEnd});
				}
			}
		}
	}
	
	std::vector<rectangle> word_rects = draw_string_hilite(talk_gworld(), word_place_rect, str, style, hilites, color ? CUSTOM_WORD_ON : CUSTOM_WORD_OFF);
	
	if(!talk_end_forced) {
		// Now build the list of word rects
		for(size_t i = 0; i < hilites.size(); i++) {
			word_rect_t thisRect;
			thisRect.word = str.substr(hilites[i].first, hilites[i].second - hilites[i].first);
			thisRect.rect = word_rects[i];
			thisRect.node = nodes[i];
			thisRect.on = CUSTOM_WORD_ON;
			thisRect.off = CUSTOM_WORD_OFF; // Note: "off" is never used
			talk_words.push_back(thisRect);
		}
	}
	
	talk_gworld().setActive();
	rectangle oldRect(talk_gworld());
	undo_clip(talk_gworld());
	talk_gworld().display();
	
	// Finally place processed graphics
	mainPtr().setActive();
	rect_draw_some_item(talk_gworld(),oldRect,mainPtr(),talk_area_rect);
	// I have no idea what this check is for; I'm jsut preserving it in case it was important
	if(c_rect.right == 0) place_talk_face();
}

void refresh_talking() {
	rectangle tempRect(talk_gworld());
	rect_draw_some_item(talk_gworld(),tempRect,mainPtr(),talk_area_rect);
	place_talk_face();
}

short scan_for_response(const char *str) {
	if(strnicmp(str, "    ", 4) == 0) return -1;
	cSpeech talk = univ.town.cur_talk();
	for(short i = 0; i < talk.talk_nodes.size(); i++) {
		cSpeech::cNode node = talk.talk_nodes[i];
		short personality = node.personality;
		if(personality == -1) continue;
		if(personality != store_personality && personality != -2)
			continue;
		if(strnicmp(str, node.link1, 4) == 0) return i;
		if(strnicmp(str, node.link2, 4) == 0) return i;
	}
	return -1;
}

bool targeting_line_visible = false;

void handle_target_mode(eGameMode target_mode, int range, eSpell spell) {
	overall_mode = target_mode;
	targeting_line_visible = true;
	// Lock on to enemies in range: 
	if(has_feature_flag("target-lock", "V1") && get_bool_pref("TargetLock", true)){
		// Skip this for spells that don't target enemies
		if(spell != eSpell::NONE){
			cSpell spell_info = *spell;
			if(!spell_info.target_lock) return;
		}

		location loc = univ.current_pc().combat_pos;

		std::vector<location> enemy_locs_in_range;
		std::vector<location> enemy_locs_already_seen;
		for(short i = 0; i < univ.town.monst.size(); i++){
			auto& monst = univ.town.monst[i];
			if(monst.is_alive() && party_can_see_monst(i) && !monst.invisible) {
				eAttitude att = monst.attitude;
				if((att == eAttitude::HOSTILE_A || att == eAttitude::HOSTILE_B)
					&& dist(loc, monst.cur_loc) <= range){
					// Target lock V2: Don't move the screen if it hides an enemy the player can already see
					if(has_feature_flag("target-lock", "V2") && is_on_screen(monst.cur_loc))
						enemy_locs_already_seen.push_back(monst.cur_loc);

					enemy_locs_in_range.push_back(monst.cur_loc);
				}
			}
		}
		if(!enemy_locs_in_range.empty()){
			std::vector<location> dest_candidates = points_containing_most(enemy_locs_in_range, enemy_locs_already_seen);
			// Center can stay the same if all points with the most monsters exclude any required (already seen) monsters
			if(!dest_candidates.empty()){
				center = closest_point(dest_candidates, loc);
			}
			draw_terrain();
		}
	}
}