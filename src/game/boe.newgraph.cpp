
#include <cstring>
#include <cstdio>
#include <vector>

#include "boe.global.hpp"

#include "universe.hpp"
#include "boe.graphics.hpp"
#include "boe.graphutil.hpp"
#include "boe.monster.hpp"
#include "boe.newgraph.hpp"
#include "boe.fileio.hpp"
#include "boe.locutils.hpp"
#include "boe.text.hpp"
#include "sounds.hpp"
#include "mathutil.hpp"
#include "render_image.hpp"
#include "render_shapes.hpp"
#include "tiling.hpp"
#include "utility.hpp"
#include "scrollbar.hpp"
#include <memory>
#include "location.hpp"
#include "shop.hpp"
#include "spell.hpp"
#include "button.hpp"
#include "res_image.hpp"
#include "prefs.hpp"

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
extern location ul;
extern rectangle	windRect;
extern long anim_ticks;
extern tessel_ref_t bg[];
extern sf::RenderWindow mainPtr;
extern short which_combat_type;
extern eGameMode overall_mode;
extern bool boom_anim_active;
extern sf::RenderTexture terrain_screen_gworld;
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
extern const short alch_difficulty[20];
extern const eItemAbil alch_ingred1[20];
extern const eItemAbil alch_ingred2[20];

// Talk vars
extern eGameMode store_pre_talk_mode;
extern short store_personality,store_personality_graphic;
extern sf::RenderTexture talk_gworld;
extern bool talk_end_forced;
extern std::string old_str1,old_str2,one_back1,one_back2;
extern rectangle talk_area_rect, word_place_rect;
extern std::string title_string;
extern mon_num_t store_monst_type;
std::vector<word_rect_t> talk_words;

// Shop vars
extern cShop active_shop;
extern eGameMode store_pre_shop_mode;
extern rectangle shopping_rects[8][7];
extern rectangle bottom_help_rects[4];
extern rectangle shop_name_str;
extern rectangle shop_frame ;
extern rectangle shop_done_rect;
extern char *heal_types[];
extern short heal_costs[8];
extern short shop_array[30];

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

void apply_unseen_mask() {
	rectangle base_rect = {9,9,53,45},to_rect,big_to = {13,13,337,265};
	bool need_bother = false;
	
	if(!get_bool_pref("DrawTerrainFrills", true) || fog_lifted)
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
				tileImage(terrain_screen_gworld, to_rect, bw_pats[3], sf::BlendAlpha);
			}
}

void apply_light_mask(bool onWindow) {
	static Region dark_mask_region;
	rectangle temp = {0,0,108,84},paint_rect,base_rect = {0,0,36,28};
	rectangle big_to = {13,13,337,265};
	bool same_mask = true;
	if(!get_bool_pref("DrawTerrainFrills", true) > 0 || fog_lifted)
		return;
	if(is_out() || overall_mode == MODE_RESTING)
		return;
	if(univ.town->lighting_type == 0)
		return;
	
	if(onWindow) {
		mainPtr.setActive();
		fill_region(mainPtr, dark_mask_region, sf::Color::Black);
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
	
	std::cout << "Current light mask:\n";
	for(short i = 0; i < 13; i++) {
		for(short j = 0; j < 13; j++)
			std::cout << int(light_area[j][i]) << ' ';
		std::cout << '\n';
	}
	
	dark_mask_region.clear();
	dark_mask_region.addRect(big_to);
	for(short i = 0; i < 13; i++)
		for(short j = 0; j < 13; j++)
			last_light_mask[i][j] = light_area[i][j];
	for(short i = 1; i < 12; i++)
		for(short j = 1; j < 12; j++) {
			if(light_area[i][j] == 2) {
				int xOffset = 13 + 28 * (i - 3), yOffset = 13 + 36 * (j - 3);
				Region oval_region;
				oval_region.addEllipse(temp);
				oval_region.offset(xOffset, yOffset);
				dark_mask_region -= oval_region;
			}
			if(light_area[i][j] == 3) {
				paint_rect = base_rect;
				paint_rect.offset(13 + 28 * (i - 2),13 + 36 * (j - 2));
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
	dark_mask_region.offset(ul);
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
	if(!get_bool_pref("DrawTerrainFrills", true))
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
//	if((cartoon_happening) && (anim_step < 140))
//		return;
	start_missile_anim();
	add_missile(fire_to,miss_type,path, x_adj, y_adj);
	do_missile_anim(len,from, sound_num);
	end_missile_anim();
}

void run_a_boom(location boom_where,short type,short x_adj,short y_adj,short snd) {
	
//	if((cartoon_happening) && (anim_step < 140))
//		return;
	if((type < 0) || (type > 2))
		return;
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

void add_explosion(location dest,short val_to_place,short place_type,short boom_type,short x_adj,short y_adj) {
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

void do_missile_anim(short num_steps,location missile_origin,short sound_num) {
	// TODO: Get rid of temp_rect, at least
	rectangle temp_rect, missile_origin_base = {1,1,17,17},to_rect,from_rect;
	short store_missile_dir;
	location start_point,finish_point[30];
	location screen_ul;
	
	short x1[30],x2[30],y1[30],y2[30]; // for path paramaterization
	rectangle missile_place_rect[30],missile_origin_rect[30];
	location current_terrain_ul;
	
	if(!have_missile || !boom_anim_active) {
		boom_anim_active = false;
		return;
	}
	
	if(std::all_of(store_missiles, store_missiles + 30, [](const store_missile_type& m) {
		return m.missile_type == 0;
	})) return;
	
	// initialize general data
	// TODO: This is probably yet another relic of the Exile III demo
	if(overall_mode == MODE_STARTUP) {
		current_terrain_ul.x = 306;
		current_terrain_ul.y = 5;
	} else current_terrain_ul.x = current_terrain_ul.y = 5;
	
	// make terrain_template contain current terrain all nicely
	draw_terrain(1);
	to_rect = rectangle(terrain_screen_gworld);
	to_rect.bottom -= 10; // Adjust for pointing buttons
	rectangle oldBounds = to_rect;
	to_rect.offset(current_terrain_ul);
	rect_draw_some_item(terrain_screen_gworld.getTexture(),oldBounds,to_rect,ul);
	
	mainPtr.setActive();
	
	
	// init missile paths
	for(short i = 0; i < 30; i++) {
		if((store_missiles[i].missile_type >= 0) && (missile_origin == store_missiles[i].dest))
			store_missiles[i].missile_type = -1;
	}
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
	
	sf::Texture& missiles_gworld = *ResMgr::get<ImageRsrc>("missiles");
	// Now, at last, launch missile
	for(short t = 0; t < num_steps; t++) {
		draw_terrain();
		for(short i = 0; i < 30; i++)
			if(store_missiles[i].missile_type >= 0) {
				// Where place?
				temp_rect = missile_origin_base;
				temp_rect.offset(-8 + x2[i] + (x1[i] * t) / num_steps,
								 -8 + y2[i] + (y1[i] * t) / num_steps);
				temp_rect.offset(ul);
				temp_rect.offset(current_terrain_ul);
				
				// now adjust for different paths
				if(store_missiles[i].path_type == 1)
					temp_rect.offset(0, -1 * (t * (num_steps - t)) / 100);
				
				missile_place_rect[i] = temp_rect;
				
				// Now put in missile
				if(store_missiles[i].missile_type < 1000) {
					from_rect = missile_origin_rect[i];
					if(store_missiles[i].missile_type >= 7)
						from_rect.offset(18 * (t % 8),0);
					rect_draw_some_item(missiles_gworld,from_rect, mainPtr,temp_rect,sf::BlendAlpha);
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
					sf::Texture* from_gw = nullptr;
					graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(base, isParty);
					if(from_gw == nullptr) continue;
					from_rect.width() = 18;
					from_rect.height() = 18;
					if(step >= 4)
						from_rect.offset(0,18);
					rect_draw_some_item(*from_gw,from_rect, mainPtr,temp_rect,sf::BlendAlpha);
				}
			}
		mainPtr.display();
		int speed = get_int_pref("GameSpeed");
		if(speed == 3 || (speed == 1 && t % 4 == 0) || (speed == 2 && t % 3 == 0))
			sf::sleep(time_in_ticks(1));
	}
	
	// Exit gracefully, and clean up screen
	for(short i = 0; i < 30; i++)
		store_missiles[i].missile_type = -1;
	
	to_rect = rectangle(terrain_screen_gworld);
	to_rect.bottom -= 10; // Adjust for pointing buttons
	rectangle oldRect = to_rect;
	to_rect.offset(current_terrain_ul);
	rect_draw_some_item(terrain_screen_gworld.getTexture(),oldRect,to_rect,ul);
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
	location current_terrain_ul;
	short boom_type_sound[6] = {5,10,53,53,53,75};
	
	if(!have_boom || !boom_anim_active) {
		boom_anim_active = false;
		return;
	}
	
	if(std::all_of(store_booms, store_booms + 30, [](const store_boom_type& b) {
		return b.boom_type == 0;
	})) return;
	
	// initialize general data
	if(overall_mode == MODE_STARTUP) {
		// TODO: I think this is a relic of the "demo" on the main screen of Exile III
		current_terrain_ul.x = 306;
		current_terrain_ul.y = 5;
	} else current_terrain_ul.x = current_terrain_ul.y = 5;
	
	// make terrain_template contain current terrain all nicely
	draw_terrain(1);
	if(special_draw != 2) {
		to_rect = rectangle(terrain_screen_gworld);
		to_rect.bottom -= 10; // Adjust for pointing buttons
		rectangle oldRect = to_rect;
		to_rect.offset(current_terrain_ul);
		rect_draw_some_item(terrain_screen_gworld.getTexture(),oldRect,to_rect,ul);
	}
	
	TextStyle style;
	style.font = FONT_BOLD;
	style.pointSize = 10;
	style.lineHeight = 10;
	mainPtr.setActive();
	
	// init missile paths
	screen_ul.x = center.x - 4; screen_ul.y = center.y - 4;
	for(short i = 0; i < 30; i++)
		if((store_booms[i].boom_type >= 0)  && (special_draw < 2)) {
			cur_boom_type = store_booms[i].boom_type;
			explode_place_rect[i] = base_rect;
			explode_place_rect[i].offset(13 + 28 * (store_booms[i].dest.x - screen_ul.x) + store_booms[i].x_adj,
										 13 + 36 * (store_booms[i].dest.y - screen_ul.y) + store_booms[i].y_adj);
			explode_place_rect[i].offset(ul);
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
	
	sf::Texture& boom_gworld = *ResMgr::get<ImageRsrc>("booms");
	// Now, at last, do explosion
	for(short t = (special_draw == 2) ? 6 : 0; t < ((special_draw == 1) ? 6 : 11); t++) { // t goes up to 10 to make sure screen gets cleaned up
		draw_terrain();
		
		// Now put in explosions
		for(short i = 0; i < 30; i++)
			if(store_booms[i].boom_type >= 0) {
				if((t + store_booms[i].offset >= 0) && (t + store_booms[i].offset <= 7)) {
					if(cur_boom_type >= 1000) {
						sf::Texture* src_gworld;
						graf_pos_ref(src_gworld, from_rect) = spec_scen_g.find_graphic(cur_boom_type - 1000 + t);
						rect_draw_some_item(*src_gworld, from_rect, mainPtr, explode_place_rect[i], sf::BlendAlpha);
					} else {
						from_rect = base_rect;
						from_rect.offset(28 * (t + store_booms[i].offset),36 * (1 + store_booms[i].boom_type));
						rect_draw_some_item(boom_gworld,from_rect,mainPtr,explode_place_rect[i],sf::BlendAlpha);
					}
					
					if(store_booms[i].val_to_place > 0) {
						text_rect = explode_place_rect[i];
						text_rect.top += 13;
						text_rect.height() = 10;
						std::string dam_str = std::to_string(store_booms[i].val_to_place);
						style.colour = sf::Color::White;
						text_rect.offset(-1,-1);
						win_draw_string(mainPtr,text_rect,dam_str,eTextMode::CENTRE,style);
						text_rect.offset(2,2);
						win_draw_string(mainPtr,text_rect,dam_str,eTextMode::CENTRE,style);
						style.colour = sf::Color::Black;
						text_rect.offset(-1,-1);
						win_draw_string(mainPtr,text_rect,dam_str,eTextMode::CENTRE,style);
						mainPtr.setActive();
					}
				}
			}
		//if(((PSD[SDF_GAME_SPEED] == 1) && (t % 3 == 0)) || ((PSD[SDF_GAME_SPEED] == 2) && (t % 2 == 0)))
		mainPtr.display();
		sf::sleep(time_in_ticks(2 * (1 + get_int_pref("GameSpeed"))));
	}
	
	// Exit gracefully, and clean up screen
	for(short i = 0; i < 30; i++)
		if(special_draw != 1)
			store_booms[i].boom_type = -1;
}

void click_shop_rect(rectangle area_rect) {
	
	draw_shop_graphics(1,area_rect);
	mainPtr.display();
	play_sound(37, time_in_ticks(5));
	draw_shop_graphics(0,area_rect);
	
}

graf_pos calc_item_rect(int num,rectangle& to_rect) {
	if(num >= 1000) return spec_scen_g.find_graphic(num - 1000);
	rectangle from_rect = {0,0,18,18};
	sf::Texture *from_gw;
	if(num < 55) {
		from_gw = ResMgr::get<ImageRsrc>("objects").get();
		from_rect = calc_rect(num % 5, num / 5);
	}else{
		from_gw = ResMgr::get<ImageRsrc>("tinyobj").get();
		to_rect.inset(5,9);
		from_rect.offset(18 * (num % 10), 18 * (num / 10));
	}
	return std::make_pair(from_gw, from_rect);
}

// mode 1 - drawing dark for button press
void draw_shop_graphics(bool pressed,rectangle clip_area_rect) {
	rectangle area_rect,item_info_from = {11,42,24,56};
	
	rectangle face_rect = {6,6,38,38};
	rectangle title_rect = {15,48,42,260};
	rectangle dest_rect,help_from = {85,36,101,54};
	
	// In the 0..65535 range, these blue components were: 0, 32767, 14535, 26623, 59391
	// The green components on the second line were 40959 and 24575
	// TODO: The Windows version appears to use completely different colours?
	sf::Color c[7] = {
		{0,0,0},{0,0,128},{0,0,57},{0,0,104},{0,0,232},
		{0,160,0},{0,96,0}
	};
	rectangle shopper_name = {44,6,56,260};
	long current_pos;
	
	short cur_cost;
	static const char*const cost_strs[] = {
		"Extremely Cheap","Very Reasonable","Pretty Average","Somewhat Pricey",
		"Expensive","Exorbitant","Utterly Ridiculous"};
	cItem base_item;
	
	if(overall_mode != 21) {
		return;
	}
	
	talk_gworld.setActive();
	TextStyle style;
	style.font = FONT_DUNGEON;
	style.pointSize = 18;
	
	talk_gworld.setActive();
	if(pressed) {
		clip_rect(talk_gworld, clip_area_rect);
	}
	
	area_rect = rectangle(talk_gworld);
	frame_rect(talk_gworld, area_rect, sf::Color::Black);
	area_rect.inset(1,1);
	tileImage(talk_gworld, area_rect,bg[12]);
	
	frame_rect(talk_gworld, shop_frame, sf::Color::Black);
	
	// Place store icon
	if(!pressed) {
		rectangle from_rect = {0,0,32,32};
		sf::Texture* from_gw;
		int i = std::max<int>(0, active_shop.getFace());
		if(i >= 1000) {
			graf_pos_ref(from_gw, from_rect) = spec_scen_g.find_graphic(i - 1000);
		} else {
			from_rect.offset(32 * (i % 10),32 * (i / 10));
			from_gw = ResMgr::get<ImageRsrc>("talkportraits").get();
		}
		rect_draw_some_item(*from_gw, from_rect, talk_gworld, face_rect);
	}
	
	
	// Place name of store and shopper name
	style.colour = c[3];
	style.lineHeight = 18;
	dest_rect = title_rect;
	dest_rect.offset(1,1);
	win_draw_string(talk_gworld,dest_rect,active_shop.getName(),eTextMode::LEFT_TOP,style);
	dest_rect.offset(-1,-1);
	style.colour = c[4];
	win_draw_string(talk_gworld,dest_rect,active_shop.getName(),eTextMode::LEFT_TOP,style);
	
	style.font = FONT_BOLD;
	style.pointSize = 12;
	
	style.colour = c[3];
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
	win_draw_string(talk_gworld,shopper_name,title.str(),eTextMode::LEFT_TOP,style);
	
	if(pressed)
		style.colour = c[4];
	else style.colour = sf::Color::Black;
	
	sf::Texture& invenbtn_gworld = *ResMgr::get<ImageRsrc>("invenbtns");
	// Place all the items
	for(short i = 0; i < 8; i++) {
		current_pos = i + shop_sbar->getPosition();
		if(shop_array[current_pos] < 0)
			break; // theoretically, this shouldn't happen
		cShopItem item = active_shop.getItem(shop_array[current_pos]);
		eSpell spell;
		cur_cost = item.getCost(active_shop.getCostAdjust());
		base_item = item.item;
		std::string cur_name = base_item.full_name, cur_info_str;
		rectangle from_rect, to_rect = shopping_rects[i][SHOPRECT_GRAPHIC];
		sf::Texture* from_gw;
		switch(item.type) {
			case eShopItemType::ITEM:
				base_item.ident = true;
				cur_info_str = get_item_interesting_string(base_item);
				break;
			case eShopItemType::ALCHEMY:
				cur_info_str = get_str("item-abilities", int(alch_ingred1[base_item.item_level]) + 1);
				if(alch_ingred2[base_item.item_level] != eItemAbil::NONE) {
					cur_info_str += " and " + get_str("item-abilities", int(alch_ingred2[base_item.item_level]) + 1);
				}
				break;
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
		graf_pos_ref(from_gw, from_rect) = calc_item_rect(base_item.graphic_num,to_rect);
		rect_draw_some_item(*from_gw, from_rect, talk_gworld, to_rect, sf::BlendAlpha);
		
		// Now draw item
		style.pointSize = 12;
		style.lineHeight = 12;
		win_draw_string(talk_gworld,shopping_rects[i][SHOPRECT_ITEM_NAME],cur_name,eTextMode::WRAP,style);
		cur_name = std::to_string(cur_cost);
		rectangle cost_rect = shopping_rects[i][SHOPRECT_ITEM_COST];
		cost_rect.left = cost_rect.right - string_length(cur_name, style) - 10;
		win_draw_string(talk_gworld,cost_rect,cur_name,eTextMode::WRAP,style);
		cost_rect.left = cost_rect.right - 7;
		cost_rect.top += 3;
		cost_rect.height() = 7;
		rect_draw_some_item(invenbtn_gworld, {0, 29, 7, 36}, talk_gworld, cost_rect, sf::BlendAlpha);
		style.pointSize = 10;
		win_draw_string(talk_gworld,shopping_rects[i][SHOPRECT_ITEM_EXTRA],cur_info_str,eTextMode::WRAP,style);
		rect_draw_some_item(invenbtn_gworld,item_info_from,talk_gworld,shopping_rects[i][SHOPRECT_ITEM_HELP],pressed ? sf::BlendNone : sf::BlendAlpha);
		
	}
	
	// Finally, cost info and help strs
	title.str("");
	title << "Prices here are " << cost_strs[active_shop.getCostAdjust()] << '.';
	style.pointSize = 10;
	style.lineHeight = 12;
	win_draw_string(talk_gworld,bottom_help_rects[0],title.str(),eTextMode::WRAP,style);
	win_draw_string(talk_gworld,bottom_help_rects[1],"Click on item name (or type 'a'-'h') to buy.",eTextMode::WRAP,style);
	win_draw_string(talk_gworld,bottom_help_rects[2],"Hit done button (or Esc.) to quit.",eTextMode::WRAP,style);
	win_draw_string(talk_gworld,bottom_help_rects[3],"'I' button brings up description.",eTextMode::WRAP,style);
	
	undo_clip(talk_gworld);
	talk_gworld.display();
	
	refresh_shopping();
	if(shop_sbar->getMaximum() > 1)
		shop_sbar->show();
	else shop_sbar->hide();
	done_btn->show();
}

void refresh_shopping() {
	rectangle from_rect(talk_gworld);
	rectangle to_rect = from_rect;
	to_rect.offset(5,5);
	rect_draw_some_item(talk_gworld.getTexture(),from_rect,to_rect,ul);
	shop_sbar->draw();
}

static void place_talk_face() {
	rectangle face_rect = {6,6,38,38};
	face_rect.offset(talk_area_rect.topLeft());
	face_rect.offset(ul);
	mainPtr.setActive();
	short face_to_draw = univ.scenario.scen_monsters[store_monst_type].default_facial_pic;
	if(store_talk_face_pic >= 0)
		face_to_draw = store_talk_face_pic;
	if(store_talk_face_pic >= 1000) {
		cPict::drawAt(mainPtr, face_rect, store_talk_face_pic, PIC_CUSTOM_TALK, false);
	}
	else {
		if(face_to_draw == NO_PIC) {
			short i = get_monst_picnum(store_monst_type);
			cPict::drawAt(mainPtr, face_rect, i, get_monst_pictype(store_monst_type), false);
		} else cPict::drawAt(mainPtr, face_rect, face_to_draw, PIC_TALK, false);
	}
}

void click_talk_rect(word_rect_t word) {
	rectangle talkRect(talk_gworld), wordRect(word.rect);
	mainPtr.setActive();
	rect_draw_some_item(talk_gworld.getTexture(),talkRect,talk_area_rect,ul);
	wordRect.offset(talk_area_rect.topLeft());
	wordRect.offset(ul);
	TextStyle style;
	style.font = FONT_DUNGEON;
	style.pointSize = 18;
	style.lineHeight = 18;
	style.colour = word.on;
	win_draw_string(mainPtr, wordRect, word.word, eTextMode::LEFT_TOP, style);
	place_talk_face();
	mainPtr.display();
	play_sound(37, time_in_ticks(5));
	rect_draw_some_item(talk_gworld.getTexture(),talkRect,talk_area_rect,ul);
	place_talk_face();
}

std::string get_item_interesting_string(cItem item) {
	if(item.property) {
		return "Not yours.";
	}
	if(!item.ident) {
		return "Not identified.";
	}
	if(item.cursed) {
		return "Cursed item.";
	}
	bool got_string = true;
	std::ostringstream sout;
	switch(item.variety) {
		case eItemType::ONE_HANDED:
		case eItemType::TWO_HANDED:
		case eItemType::ARROW:
		case eItemType::THROWN_MISSILE:
		case eItemType::BOLTS:
		case eItemType::MISSILE_NO_AMMO:
			if(item.bonus != 0)
				sout << "Damage: 1-" << item.item_level << " + " << item.bonus;
			else sout << "Damage: 1-" << item.item_level;
			break;
		case eItemType::SHIELD:
		case eItemType::ARMOR:
		case eItemType::HELM:
		case eItemType::GLOVES:
		case eItemType::SHIELD_2:
		case eItemType::BOOTS: // TODO: Verify that this is displayed correctly
			sout << "Blocks " << item.item_level + ((item.protection > 0) ? 1 : 0) << '-' << item.item_level + item.protection << " damage";
			break;
		case eItemType::BOW:
		case eItemType::CROSSBOW:
			sout << "Bonus: +" << item.bonus << " to hit";
			break;
		case eItemType::GOLD:
			sout << item.item_level << " gold pieces";
			break;
		case eItemType::SPECIAL:
		case eItemType::QUEST:
			sout << "Special";
			break;
		case eItemType::FOOD:
			sout << item.item_level << " food";
			break;
		case eItemType::WEAPON_POISON:
			sout << "Poison: " << item.item_level << '-' << item.item_level * 6 << " damage";
			break;
		default:
			got_string = false;
			break;
	}
	if(item.charges > 0 && item.ability != eItemAbil::MESSAGE) {
		if(got_string) sout << "; ";
		sout << "Uses: " << item.charges;
	}
	sout << '.';
	return sout.str();
}

// color 0 - regular  1 - darker
void place_talk_str(std::string str_to_place,std::string str_to_place2,short color,rectangle c_rect) {
	rectangle area_rect;
	
	rectangle title_rect = {19,48,42,260};
	rectangle dest_rect,help_from = {46,60,59,76};
	sf::Text str_to_draw;
	
	// In the 0..65535 range, these blue components were: 0, 32767, 14535, 26623, 59391
	// The green components on the second line were 40959 and 24575
	sf::Color c[8] = {
		{0,0,0},{0,0,128},{0,0,57},{0,0,104},{0,0,232},
		{0,160,0},{0,96,0},{160,0,20}
	};
	
	talk_gworld.setActive();
	
	TextStyle style;
	style.font = FONT_DUNGEON;
	style.pointSize = 18;
	
	if(c_rect.right > 0) {
		mainPtr.setActive();
		c_rect.offset(talk_area_rect.topLeft());
		c_rect.offset(ul);
		clip_rect(mainPtr, c_rect);
	}
	
	area_rect = rectangle(talk_gworld);
	frame_rect(talk_gworld, area_rect, sf::Color::Black);
	area_rect.inset(1,1);
	tileImage(talk_gworld, area_rect,bg[12]);
	
	// Place name oftalkee
	style.colour = c[3];
	style.lineHeight = 18;
	dest_rect = title_rect;
	dest_rect.offset(1,1);
	win_draw_string(talk_gworld,dest_rect,title_string,eTextMode::LEFT_TOP,style);
	dest_rect.offset(-1,-1);
	style.colour = c[4];
	win_draw_string(talk_gworld,dest_rect,title_string,eTextMode::LEFT_TOP,style);
	
	talk_words.clear();
	static const rectangle preset_rects[9] = {
		rectangle{366,4,386,54}, rectangle{366,70,386,130}, rectangle{366,136,386,186},
		rectangle{389,4,409,54}, rectangle{389,70,409,120}, rectangle{389,121,409,186},
		rectangle{389,210,409,270}, rectangle{366,190,386,270},
		rectangle{343,4,363,134},
	};
	static const char*const preset_words[9] = {
		"Look", "Name", "Job",
		"Buy", "Sell", "Record",
		"Done", "Go Back",
		"Ask About...",
	};
	
	// Place buttons at bottom.
	if(color == 0)
		style.colour = c[5];
	else style.colour = c[6];
	for(short i = 0; i < 9; i++) {
		if(!talk_end_forced || i == 6 || i == 5) {
			word_rect_t preset_word(preset_words[i], preset_rects[i]);
			preset_word.on = c[5];
			preset_word.off = c[6];
			switch(i) {
				case 0: preset_word.node = TALK_LOOK; break;
				case 1: preset_word.node = TALK_NAME; break;
				case 2: preset_word.node = TALK_JOB; break;
				case 3: preset_word.node = TALK_BUY; break;
				case 4: preset_word.node = TALK_SELL; break;
				case 5: preset_word.node = TALK_RECORD; break;
				case 6: preset_word.node = TALK_DONE; break;
				case 7: preset_word.node = TALK_BACK; break;
				case 8: preset_word.node = TALK_ASK; break;
			}
			talk_words.push_back(preset_word);
			rectangle draw_rect = preset_word.rect;
			win_draw_string(talk_gworld,draw_rect,preset_word.word,eTextMode::LEFT_TOP,style);
		}
	}
	
	dest_rect = word_place_rect;
	style.colour = c[2];
	// First determine the offsets of clickable words.
	// The added spaces ensure that end-of-word boundaries are found
	std::string str = str_to_place + " |" + str_to_place2 + " ";
	std::vector<hilite_t> hilites;
	std::vector<int> nodes;
	int wordStart = 0, wordEnd = 0;
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
	
	std::vector<rectangle> word_rects = draw_string_hilite(talk_gworld, dest_rect, str, style, hilites, color ? c[1] : c[7]);
	
	if(!talk_end_forced) {
		// Now build the list of word rects
		for(size_t i = 0; i < hilites.size(); i++) {
			word_rect_t thisRect;
			thisRect.word = str.substr(hilites[i].first, hilites[i].second - hilites[i].first);
			thisRect.rect = word_rects[i];
			thisRect.node = nodes[i];
			thisRect.on = c[1];
			thisRect.off = c[2]; // Note: "off" is never used
			talk_words.push_back(thisRect);
		}
	}
	
	rectangle oldRect(talk_gworld);
	undo_clip(talk_gworld);
	talk_gworld.display();
	
	// Finally place processed graphics
	mainPtr.setActive();
	rect_draw_some_item(talk_gworld.getTexture(),oldRect,talk_area_rect,ul);
	// I have no idea what this check is for; I'm jsut preserving it in case it was important
	if(c_rect.right == 0) place_talk_face();
}

void refresh_talking() {
	rectangle tempRect(talk_gworld);
	rect_draw_some_item(talk_gworld.getTexture(),tempRect,talk_area_rect,ul);
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

