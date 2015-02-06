
#include <cmath>
#include <cstdio>
#include <cstring>
#include <array>
#include <string>
#include "scen.global.h"
#include "scenario.h"
#include "graphtool.hpp"
#include "scen.graphics.h"
#include "scen.actions.h"
#include "soundtool.hpp"
#include "scen.core.h"
#include "scen.fileio.h"
#include "scen.keydlgs.h"
#include "scen.townout.h"
#include "scen.menus.h"
#include "mathutil.hpp"
#include "fileio.hpp"
#include "winutil.hpp"
#include "cursors.hpp"
#include "scrollbar.hpp"
#include "dlogutil.hpp"

#include "scen.btnmg.h"

extern char current_string[256];
extern short mini_map_scales[3];
rectangle world_screen;
// border rects order: top, left, bottom, right //
rectangle border_rect[4];
short current_block_edited = 0;
short current_terrain_type = 0;
short safety = 0;
location spot_hit,last_spot_hit(-1,-1),mouse_spot(-1,-1);
bool sign_error_received = false;
short copied_spec = -1;

bool good_palette_buttons[2][6][10] = {
	{
		{1,1,1,1,1,1,1,1,0,0},
		{1,1,1,1,1,1,0,1,0,0},
		{0,0,1,1,1,1,1,1,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0}
	}, {
		{1,1,1,1,1,1,1,1,0,1},
		{1,1,1,1,1,1,0,0,0,1},
		{1,1,1,1,1,1,1,1,0,1},
		{1,1,1,0,1,1,1,1,0,1},
		{1,1,1,1,1,1,1,1,0,0},
		{1,1,1,1,1,1,1,1,0,1}
	}
};
cTown::cItem store_place_item;

short flood_count = 0;

rectangle terrain_rects[256],terrain_rect_base = {0,0,16,16},command_rects[21];


extern short cen_x, cen_y, cur_town;
extern eScenMode overall_mode;
extern bool mouse_button_held,editing_town;
extern short cur_viewing_mode;
extern cTown* town;
extern short mode_count,to_create;
extern ter_num_t template_terrain[64][64];
extern cScenario scenario;
extern std::shared_ptr<cScrollbar> right_sbar;
extern cOutdoors* current_terrain;
extern location cur_out;
extern sf::RenderWindow mainPtr;
bool small_any_drawn = false;
extern bool change_made;

rectangle left_buttons[NLS][2]; // 0 - whole, 1 - blue button
std::array<lb_t,NLS> left_button_status;
std::array<rb_t,NRS> right_button_status;
rectangle right_buttons[NRSONPAGE];
rectangle palette_buttons_from[71];
rectangle palette_buttons[10][6];
short current_rs_top = 0;

short out_buttons[6][10] = {
	{0, 1, 2, 3, 4, 5, 6, 7, -1,-1},
	{10,11,12,13,14,15,-1,17,-1,-1},
	{-1,-1,46,23,24,25,26,27,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
};
short town_buttons[6][10] = {
	{0, 1, 2, 3, 4, 5, 6, 7, -1,9 },
	{10,11,12,13,14,15,-1,-1,-1,29},
	{20,21,22,23,24,25,26,27,-1,39},
	{30,31,32,-1,34,35,36,37,-1,49},
	{40,41,42,43,44,45,46,47,-1,-1},
	{50,51,52,53,54,55,56,57,-1,69},
};

cTownperson last_placed_monst;

rectangle working_rect;
location last_space_hit;
bool erasing_mode;
ter_num_t current_ground = 0;

short special_to_paste = -1;

bool monst_on_space(location loc,short m_num);

void init_current_terrain() {
//	short i,j;
//	location d_loc(0,0);
//	rectangle d_rect = {0,0,0,0};
//	cTown::cWandering d_wan = {0,0,0,0};
//	cTown::cCreature dummy_creature = {0,0,loc(),0,0,0,0,0,0,0};
//	//city_ter_rect_type dummy_ter_rect = {{0,0,0,0},0,0};
}

void init_screen_locs() {
	int i;
	
	for(i = 0; i < 4; i++)
		border_rect[i] = world_screen;
	border_rect[0].bottom = border_rect[0].top + 8;
	border_rect[1].right = border_rect[1].left + 8;
	border_rect[2].top = border_rect[2].bottom - 8;
	border_rect[3].left = border_rect[3].right - 8;
	
	for(i = 0; i < 256; i++) {
		terrain_rects[i] = terrain_rect_base;
		terrain_rects[i].offset(3 + (i % 17) * (terrain_rect_base.right + 1),
			3 + (i / 17) * (terrain_rect_base.bottom + 1));
	}
}

void update_mouse_spot(location the_point) {
	rectangle terrain_rect = world_screen;
	terrain_rect.inset(8,8);
	if(terrain_rect.contains(the_point)) {
		if(cur_viewing_mode == 0) {
			mouse_spot.x = (the_point.x - TER_RECT_UL_X - 8) / 28;
			mouse_spot.y = (the_point.y - TER_RECT_UL_Y - 8) / 36;
		} else {
			short scale = mini_map_scales[cur_viewing_mode - 1];
			mouse_spot.x = (the_point.x - TER_RECT_UL_X - 8) / scale;
			mouse_spot.y = (the_point.y - TER_RECT_UL_Y - 8) / scale;
		}
	} else mouse_spot = {-1,-1};
}

bool handle_action(location the_point,sf::Event /*event*/) {
	using kb = sf::Keyboard;
	short i,j, x;
	bool are_done = false;
	std::string s2;
	fs::path file_to_load;
	
	bool need_redraw = false,option_hit = false,ctrl_hit = false;
	location spot_hit;
	location cur_point,cur_point2;
	long right_top;
	eScenMode old_mode;
	rectangle temp_rect;
	if(kb::isKeyPressed(kb::LAlt) || kb::isKeyPressed(kb::RAlt))
		option_hit = true;
	if(kb::isKeyPressed(kb::LControl) || kb::isKeyPressed(kb::RControl))
		ctrl_hit = true;
	
	for(i = 0; i < NLS; i++)
		if(!mouse_button_held && the_point.in(left_buttons[i][0])
		   && (left_button_status[i].action != LB_NO_ACTION))  {
			draw_lb_slot(i,1);
			play_sound(37);
			mainPtr.display();
			// TODO: Proper button handling
			sf::sleep(time_in_ticks(10));
			draw_lb_slot(i,0);
			mainPtr.display();
			if(overall_mode == MODE_INTRO_SCREEN || overall_mode == MODE_MAIN_SCREEN || overall_mode == MODE_EDIT_TYPES) {
				switch(left_button_status[i].action) {
					case LB_NO_ACTION:
						break;
					case LB_RETURN: // Handled separately, below
						break;
					case LB_NEW_SCEN:
						if(build_scenario()){
							overall_mode = MODE_MAIN_SCREEN;
							set_up_main_screen();
						}
						break;
						
					case LB_LOAD_SCEN:
						file_to_load = nav_get_scenario();
						if(!file_to_load.empty() && load_scenario(file_to_load, scenario)) {
							cur_town = scenario.last_town_edited;
							town = scenario.towns[cur_town];
							cur_out = scenario.last_out_edited;
							current_terrain = scenario.outdoors[cur_out.x][cur_out.y];
							overall_mode = MODE_MAIN_SCREEN;
							set_up_main_screen();
							update_item_menu();
						}
						break;
					case LB_EDIT_TER:
						start_terrain_editing();
						break;
					case LB_EDIT_MONST:
						start_monster_editing(0);
						break;
					case LB_EDIT_ITEM:
						start_item_editing(0);
						break;
					case LB_NEW_TOWN:
						if(change_made) {
							giveError("You need to save the changes made to your scenario before you can add a new town.");
							return are_done;
						}
						if(scenario.towns.size() >= 200) {
							giveError("You have reached the limit of 200 towns you can have in one scenario.");
							return are_done;
						}
						if(new_town(scenario.towns.size()))
							set_up_main_screen();
						break;
					case LB_EDIT_TEXT:
						right_sbar->setPosition(0);
						start_string_editing(0,0);
						break;
					case LB_EDIT_SPECITEM:
						start_special_item_editing();
						break;
					case LB_EDIT_QUEST:
						start_quest_editing();
						break;
					case LB_EDIT_SHOPS:
						start_shops_editing();
						break;
					case LB_LOAD_OUT:
						if(change_made) {
							if(!save_check("save-section-confirm"))
								break;
						}
						spot_hit = pick_out(cur_out);
						if(spot_hit != cur_out) {
							cur_out = spot_hit;
							current_terrain = scenario.outdoors[cur_out.x][cur_out.y];
							set_up_main_screen();
						}
						break;
					case LB_EDIT_OUT:
						start_out_edit();
						mouse_button_held = false;
						return false;
						break;
					case LB_LOAD_TOWN:
						if(change_made) {
							if(!save_check("save-section-confirm"))
								break;
						}
						x = pick_town_num("select-town-edit",cur_town,scenario);
						if(x >= 0){
							cur_town = x;
							town = scenario.towns[cur_town];
							set_up_main_screen();
						}
						break;
					case LB_EDIT_TOWN:
						start_town_edit();
						mouse_button_held = false;
						return false;
						break;
					case LB_EDIT_TALK:
						start_dialogue_editing(0);
						break;
						
				}
			}
			if((overall_mode < MODE_MAIN_SCREEN) && left_button_status[i].action == LB_RETURN) {
				set_cursor(wand_curs);
				set_up_main_screen();
			}
			mouse_button_held = false;
		}
	
	if(overall_mode == MODE_MAIN_SCREEN) {
		right_top = right_sbar->getPosition();
		for(i = 0; i < NRSONPAGE; i++)
			if(!mouse_button_held && (the_point.in(right_buttons[i]) )
			   && (right_button_status[i + right_top].action != RB_CLEAR))  {
				
				j = right_button_status[i + right_top].i;
				//flash_rect(left_buttons[i][0]);
				draw_rb_slot(i + right_top,1);
				mainPtr.display();
				// TODO: Proper button handling
				play_sound(37);
				sf::sleep(time_in_ticks(10));
				draw_rb_slot(i + right_top,0);
				mainPtr.display();
				change_made = true;
				size_t size_before;
				size_t pos_before = right_sbar->getPosition();
				switch(right_button_status[i + right_top].action) {
					case RB_CLEAR:
						break;
					case RB_TER:
						edit_ter_type(j);
						update_item_menu();
						break;
					case RB_MONST:
						edit_monst_type(j);
						update_item_menu();
						start_monster_editing(1);
						break;
					case RB_ITEM:
						edit_item_type(j);
						update_item_menu();
						start_item_editing(1);
						break;
					case RB_SCEN_SPEC:
						size_before = scenario.scen_specials.size();
						if(option_hit) {
							if(j == size_before - 1)
								scenario.scen_specials.pop_back();
							else if(j == size_before)
								break;
							else scenario.scen_specials[j] = cSpecial();
						} else {
							if(j == size_before)
								scenario.scen_specials.emplace_back();
							if(!edit_spec_enc(j,0,nullptr) && j == size_before)
								scenario.scen_specials.pop_back();
						}
						start_special_editing(0,size_before == scenario.scen_specials.size());
						if(size_before > scenario.scen_specials.size())
							pos_before--;
						right_sbar->setPosition(pos_before);
						break;
					case RB_OUT_SPEC:
						size_before = current_terrain->specials.size();
						if(option_hit) {
							if(j == size_before - 1)
								current_terrain->specials.pop_back();
							else if(j == size_before)
								break;
							else current_terrain->specials[j] = cSpecial();
						} else {
							if(j == size_before)
								current_terrain->specials.emplace_back();
							if(!edit_spec_enc(j,1,nullptr) && j == size_before)
								current_terrain->specials.pop_back();
						}
						start_special_editing(1,size_before == current_terrain->specials.size());
						if(size_before > current_terrain->specials.size())
							pos_before--;
						right_sbar->setPosition(pos_before);
						break;
					case RB_TOWN_SPEC:
						size_before = town->specials.size();
						if(option_hit) {
							if(j == size_before - 1)
								town->specials.pop_back();
							else if(j == size_before)
								break;
							else town->specials[j] = cSpecial();
						} else {
							if(j == size_before)
								town->specials.emplace_back();
							if(!edit_spec_enc(j,2,nullptr) && j == size_before)
								town->specials.pop_back();
						}
						start_special_editing(2,size_before == town->specials.size());
						if(size_before > town->specials.size())
							pos_before--;
						right_sbar->setPosition(pos_before);
						break;
					case RB_SCEN_STR:
						if(option_hit) {
							s2 = get_str("scen-default", j + 161);
							scenario.spec_strs[j] = s2;
						}
						else edit_text_str(j,0);
						start_string_editing(0,1);
						break;
						
					case RB_OUT_STR:
						if(option_hit) {
							s2 = get_str("outdoor-default", j + 11);
							current_terrain->spec_strs[j] = s2;
						}
						else edit_text_str(j,1);
						start_string_editing(1,1);
						break;
					case RB_TOWN_STR:
						if(option_hit) {
							s2 = get_str("town-default", j + 21);
							town->spec_strs[j] = s2;
						}
						else edit_text_str(j,2);
						start_string_editing(2,1);
						break;
					case RB_SPEC_ITEM:
						edit_spec_item(j);
						start_special_item_editing();
						break;
					case RB_JOURNAL:
						if(option_hit) {
							s2 = get_str("scen-default", j + 11);
							scenario.journal_strs[j] = s2;
						}
						else edit_text_str(j,3);
						start_string_editing(3,1);
						break;
					case RB_DIALOGUE:
						edit_talk_node(j);
						start_dialogue_editing(1);
						break;
					case RB_PERSONALITY:
						edit_basic_dlog(j);
						start_dialogue_editing(1);
						break;
					case RB_OUT_SIGN:
						if(option_hit) {
							s2 = get_str("outdoor-default", j + 101);
							current_terrain->spec_strs[j] = s2;
						}
						else edit_text_str(j,4);
						start_string_editing(4,1);
						break;
					case RB_TOWN_SIGN:
						if(option_hit) {
							s2 = get_str("town-default", j + 121);
							town->spec_strs[j] = s2;
						}
						else edit_text_str(j,5);
						start_string_editing(5,1);
						break;
					case RB_QUEST:
						if(option_hit) {
							if(j == scenario.quests.size() - 1)
								scenario.quests.pop_back();
							else {
								scenario.quests[j] = cQuest();
								scenario.quests[j].name = "Unused Quest";
							}
						} else edit_quest(j);
						start_quest_editing();
						break;
					case RB_SHOP:
						if(option_hit) {
							if(j == scenario.shops.size() - 1)
								scenario.shops.pop_back();
							else scenario.shops[j] = cShop("Unused Shop");
						} else edit_shop(j);
						start_shops_editing();
						break;
				}
				mouse_button_held = false;
			}
	}
	update_mouse_spot(the_point);
	if(overall_mode < MODE_MAIN_SCREEN) {;
		if(mouse_spot.x >= 0 && mouse_spot.y >= 0) {
		if(cur_viewing_mode == 0) {
			spot_hit.x = cen_x + mouse_spot.x - 4;
			spot_hit.y = cen_y + mouse_spot.y - 4;
			if((mouse_spot.x < 0) || (mouse_spot.x > 8) || (mouse_spot.y < 0) || (mouse_spot.y > 8))
				spot_hit.x = -1;
		}
		else {
			short scale = mini_map_scales[cur_viewing_mode - 1];
			if(scale > 4) {
				if(cen_x + 5 > 256 / scale)
					spot_hit.x = cen_x + 5 - 256/scale + mouse_spot.x;
				else spot_hit.x = mouse_spot.x;
				if(cen_y + 5 > 324 / scale)
					spot_hit.y = cen_y + 5 - 324/scale + mouse_spot.y;
				else spot_hit.y = mouse_spot.y;
			} else {
				spot_hit.x = mouse_spot.x;
				spot_hit.y = mouse_spot.y;
			}
		}
		
		if((mouse_button_held) && (spot_hit.x == last_spot_hit.x) &&
			(spot_hit.y == last_spot_hit.y))
			return are_done;
		else last_spot_hit = spot_hit;
		if(!mouse_button_held)
			last_spot_hit = spot_hit;
		
		old_mode = overall_mode;
		change_made = true;
		
		if((spot_hit.x < 0) || (spot_hit.x > ((editing_town) ? town->max_dim() - 1 : 47)) ||
			(spot_hit.y < 0) || (spot_hit.y > ((editing_town) ? town->max_dim() - 1 : 47))) ;
		else switch(overall_mode) {
			case MODE_DRAWING:
				if((!mouse_button_held && terrain_matches(spot_hit.x,spot_hit.y,current_terrain_type)) ||
					(mouse_button_held && erasing_mode)) {
					set_terrain(spot_hit,current_ground);
					set_cursor(wand_curs);
					erasing_mode = true;
					mouse_button_held = true;
				}
				else {
					mouse_button_held = true;
					set_cursor(wand_curs);
			 		set_terrain(spot_hit,current_terrain_type);
					erasing_mode = false;
				}
				break;
				
			case MODE_ROOM_RECT: case MODE_SET_TOWN_RECT: case MODE_HOLLOW_RECT: case MODE_FILLED_RECT:
				if(mouse_button_held)
					break;
				if(mode_count == 2) {
					working_rect.left = spot_hit.x;
				 	working_rect.top = spot_hit.y;
					mode_count = 1;
					set_cursor(bottomright_curs);
					set_string("Now select lower right corner","");
					break;
				}
				working_rect.right = spot_hit.x;
			 	working_rect.bottom = spot_hit.y;
				if((overall_mode == 1) || (overall_mode == MODE_FILLED_RECT)) {
					change_rect_terrain(working_rect,current_terrain_type,20,0);
					change_made = true;
				}
				else if(overall_mode == MODE_HOLLOW_RECT) {
					change_rect_terrain(working_rect,current_terrain_type,20,1);
					change_made = true;
				}
				else if(overall_mode == MODE_SET_TOWN_RECT) {
					town->in_town_rect = working_rect;
					change_made = true;
				}
				else { // MODE_ROOM_RECT
					if(editing_town) {
						for(x = 0; x < 16; x++)
							if(town->room_rect[x].right == 0) {
								static_cast<rectangle&>(town->room_rect[x]) = working_rect;
								town->room_rect[x].descr = "";
								if(!edit_area_rect_str(x,1))
									town->room_rect[x].right = 0;
								x = 500;
							}
					}
					else {
						for(x = 0; x < 8; x++)
							if(current_terrain->info_rect[x].right == 0) {
								static_cast<rectangle&>(current_terrain->info_rect[x]) = working_rect;
								current_terrain->info_rect[x].descr = "";
								if(!edit_area_rect_str(x,0))
									current_terrain->info_rect[x].right = 0;
								x = 500;
							}
						
					}
					if(x < 500)
						giveError("You have placed the maximum number of area rectangles (16 in town, 8 outdoors).");
					else change_made = true;
				}
				overall_mode = MODE_DRAWING;
				set_cursor(wand_curs);
				break;
			case MODE_SET_WANDER_POINTS:
				if(mouse_button_held)
					break;
				if(!editing_town) {
					current_terrain->wandering_locs[mode_count - 1].x = spot_hit.x;
					current_terrain->wandering_locs[mode_count - 1].y = spot_hit.y;
				}
				if(editing_town) {
					town->wandering_locs[mode_count - 1].x = spot_hit.x;
					town->wandering_locs[mode_count - 1].y = spot_hit.y;
				}
				mode_count--;
				switch(mode_count) {
					case 3:
						set_string("Place second wandering monster arrival point","");
						break;
					case 2:
						set_string("Place third wandering monster arrival point","");
						break;
					case 1:
						set_string("Place fourth wandering monster arrival point","");
						break;
					case 0:
						overall_mode = MODE_DRAWING;
						set_cursor(wand_curs);
						set_string("Drawing mode",(char*)scenario.ter_types[current_terrain_type].name.c_str());
						break;
				}
				break;
				
			case MODE_LARGE_PAINTBRUSH:
				mouse_button_held = true;
				change_circle_terrain(spot_hit,4,current_terrain_type,20);
				break;
			case MODE_SMALL_PAINTBRUSH:
				mouse_button_held = true;
				change_circle_terrain(spot_hit,1,current_terrain_type,20);
				break;
			case MODE_LARGE_SPRAYCAN:
				mouse_button_held = true;
				shy_change_circle_terrain(spot_hit,4,current_terrain_type,1);
				break;
			case MODE_SMALL_SPRAYCAN:
				mouse_button_held = true;
				shy_change_circle_terrain(spot_hit,2,current_terrain_type,1);
				break;
			case MODE_ERASER: // erase
				change_circle_terrain(spot_hit,2,current_ground,20);
				mouse_button_held = true;
				break;
			case MODE_PLACE_ITEM:
				for(x = 0; x < town->preset_items.size(); x++)
					if(town->preset_items[x].code < 0) {
						town->preset_items[x] = {spot_hit, mode_count, scenario.scen_items[mode_count]};
						if(container_there(spot_hit)) town->preset_items[x].contained = true;
						store_place_item = town->preset_items[x];
						break;
					}
				if(x == town->preset_items.size()) {
					town->preset_items.push_back({spot_hit, mode_count, scenario.scen_items[mode_count]});
					if(container_there(spot_hit)) town->preset_items.back().contained = true;
					store_place_item = town->preset_items.back();
				}
				
				overall_mode = MODE_DRAWING;
				set_cursor(wand_curs);
				set_string("Drawing mode",(char*)scenario.ter_types[current_terrain_type].name.c_str());
				break;
			case MODE_EDIT_ITEM:
				for(x = 0; x < town->preset_items.size(); x++)
					if((spot_hit.x == town->preset_items[x].loc.x) &&
						(spot_hit.y == town->preset_items[x].loc.y) && (town->preset_items[x].code >= 0)) {
						edit_placed_item(x);
						store_place_item = town->preset_items[x];
					}
				overall_mode = MODE_DRAWING;
				set_cursor(wand_curs);
				break;
			case MODE_PLACE_SAME_CREATURE:
				if(last_placed_monst.number == 0) {
					giveError("Either no monster has been placed, or the last time you tried to place a monster the operation failed.");
					break;
				}
				for(i = 0; i < town->creatures.size(); i++)
					if(town->creatures[i].number == 0) {
						town->creatures[i] = last_placed_monst;
						town->creatures[i].start_loc = spot_hit;
						break;
					}
				if(i == town->creatures.size()) { // Placement failed
					town->creatures.push_back(last_placed_monst);
					town->creatures.back().start_loc = spot_hit;
				}
				overall_mode = MODE_DRAWING;
				set_cursor(wand_curs);
				break;
			case MODE_PLACE_CREATURE:
				for(i = 0; i < town->creatures.size(); i++)
					if(town->creatures[i].number == 0) {
						town->creatures[i] = {spot_hit, mode_count, scenario.scen_monsters[mode_count]};
						last_placed_monst = town->creatures[i];
						break;
					}
				if(i == town->creatures.size()) { // Placement failed
					town->creatures.push_back({spot_hit, mode_count, scenario.scen_monsters[mode_count]});
					last_placed_monst = town->creatures.back();
				}
				overall_mode = MODE_DRAWING;
				set_cursor(wand_curs);
				break;
				
			case MODE_PLACE_NORTH_ENTRANCE: case MODE_PLACE_EAST_ENTRANCE:
			case MODE_PLACE_SOUTH_ENTRANCE: case MODE_PLACE_WEST_ENTRANCE:
				town->start_locs[overall_mode - 10].x = spot_hit.x;
				town->start_locs[overall_mode - 10].y = spot_hit.y;
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_WEB:
				make_field_type(spot_hit.x,spot_hit.y,FIELD_WEB);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_CRATE:
				make_field_type(spot_hit.x,spot_hit.y,OBJECT_CRATE);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_BARREL:
				make_field_type(spot_hit.x,spot_hit.y,OBJECT_BARREL);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_FIRE_BARRIER:
				make_field_type(spot_hit.x,spot_hit.y,BARRIER_FIRE);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_FORCE_BARRIER:
				make_field_type(spot_hit.x,spot_hit.y,BARRIER_FORCE);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_QUICKFIRE:
				make_field_type(spot_hit.x,spot_hit.y,FIELD_QUICKFIRE);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_STONE_BLOCK:
				make_field_type(spot_hit.x,spot_hit.y,OBJECT_BLOCK);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_FORCECAGE:
				make_field_type(spot_hit.x,spot_hit.y,BARRIER_CAGE);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_TOGGLE_SPECIAL_DOT:
				if(!editing_town){
					current_terrain->special_spot[spot_hit.x][spot_hit.y] = !current_terrain->special_spot[spot_hit.x][spot_hit.y];
					overall_mode = MODE_DRAWING;
					break;
				}
				make_field_type(spot_hit.x, spot_hit.y, SPECIAL_SPOT);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_CLEAR_FIELDS:
				for(int i = 8; i <= BARRIER_CAGE; i++)
					take_field_type(spot_hit.x,spot_hit.y, eFieldType(i));
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_SFX:
				make_field_type(spot_hit.x,spot_hit.y,eFieldType(SFX_SMALL_BLOOD + mode_count));
				overall_mode = MODE_DRAWING;
				break;
			case MODE_EYEDROPPER:
				if(editing_town)
					set_new_terrain(town->terrain(spot_hit.x,spot_hit.y));
				else set_new_terrain(current_terrain->terrain[spot_hit.x][spot_hit.y]);
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_SAME_ITEM:
				if(store_place_item.code < 0) {
					giveError("Either no item has been placed, or the last time you tried to place an item the operation failed.");
					break;
				}
				for(x = 0; x < town->preset_items.size(); x++)
					if(town->preset_items[x].code < 0) {
						town->preset_items[x] = store_place_item;
						town->preset_items[x].loc = spot_hit;
						town->preset_items[x].contained = container_there(spot_hit);
						break;
					}
				if(x == town->preset_items.size()) {
					town->preset_items.push_back(store_place_item);
					town->preset_items.back().loc = spot_hit;
					town->preset_items.back().contained = container_there(spot_hit);
				}
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_EDIT_SIGN: //edit sign
				if(editing_town) {
					for(x = 0; x < town->sign_locs.size(); x++)
						if((town->sign_locs[x].x == spot_hit.x) && (town->sign_locs[x].y == spot_hit.y)) {
							edit_sign(x,scenario.ter_types[town->terrain(spot_hit.x,spot_hit.y)].picture);
							x = 30;
						}
					if(x == 15) {
						giveError("Either this space is not a sign, or you have already placed too many signs on this level.");
					}
				}
				if(!editing_town) {
					for(x = 0; x < current_terrain->sign_locs.size(); x++)
						if((current_terrain->sign_locs[x].x == spot_hit.x) && (current_terrain->sign_locs[x].y == spot_hit.y)) {
							edit_sign(x,scenario.ter_types[current_terrain->terrain[spot_hit.x][spot_hit.y]].picture);
							x = 30;
						}
					if(x == 8) {
						giveError("Either this space is not a sign, or you have already placed too many signs on this level.");
					}
				}
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_EDIT_CREATURE: //edit monst
				for(x = 0; x < town->creatures.size(); x++)
					if(monst_on_space(spot_hit,x)) {
						edit_placed_monst(x);
						last_placed_monst = town->creatures[x];
					}
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_EDIT_SPECIAL: //make special
				place_edit_special(spot_hit);
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_COPY_SPECIAL: //copy special
				if(editing_town) {
					for(x = 0; x < town->special_locs.size(); x++)
						if(town->special_locs[x] == spot_hit && town->special_locs[x].spec >= 0) {
							copied_spec = town->special_locs[x].spec;
							x = -1;
							break;
						}
				}
				if(!editing_town) {
					for(x = 0; x < current_terrain->special_locs.size(); x++)
						if(current_terrain->special_locs[x] == spot_hit && current_terrain->special_locs[x].spec >= 0) {
							copied_spec = current_terrain->special_locs[x].spec;
							x = -1;
							break;
						}
				}
				if(x < 0)
					giveError("There wasn't a special on that spot.");
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PASTE_SPECIAL: //paste special
				if(copied_spec < 0) {
					giveError("You need to select a special to copy first.");
					break;
				}
				if(editing_town) {
					for(x = 0; x <= town->special_locs.size(); x++) {
						if(x == town->special_locs.size())
							town->special_locs.emplace_back(-1,-1,-1);
						if(town->special_locs[x].spec < 0) {
							town->special_locs[x] = spot_hit;
							town->special_locs[x].spec = copied_spec;
							break;
						}
					}
				}
				if(!editing_town) {
					if((spot_hit.x == 0) || (spot_hit.x == 47) || (spot_hit.y == 0) || (spot_hit.y == 47)) {
						cChoiceDlog("not-at-edge").show();
						break;
					}
					for(x = 0; x <= current_terrain->special_locs.size(); x++) {
						if(x == current_terrain->special_locs.size())
							current_terrain->special_locs.emplace_back(-1,-1,-1);
						if(current_terrain->special_locs[x].spec < 0) {
							current_terrain->special_locs[x] = spot_hit;
							current_terrain->special_locs[x].spec = copied_spec;
							break;
						}
					}
				}
				
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_ERASE_SPECIAL: //erase special
				if(editing_town) {
					for(x = 0; x < town->special_locs.size(); x++)
						if(town->special_locs[x] == spot_hit && town->special_locs[x].spec >= 0) {
							town->special_locs[x] = {-1,-1};
							town->special_locs[x].spec = -1;
							if(x == town->special_locs.size() - 1) {
								// Delete not only the last entry but any other empty entries at the end of the list
								do {
									town->special_locs.pop_back();
								} while(town->special_locs.back().spec < 0);
							}
							break;
						}
				}
				if(!editing_town) {
					for(x = 0; x < current_terrain->special_locs.size(); x++)
						if(current_terrain->special_locs[x] == spot_hit && current_terrain->special_locs[x].spec >= 0) {
							current_terrain->special_locs[x] = {-1,-1};
							current_terrain->special_locs[x].spec = -1;
							if(x == current_terrain->special_locs.size() - 1) {
								// Delete not only the last entry but any other empty entries at the end of the list
								do {
									current_terrain->special_locs.pop_back();
								} while(current_terrain->special_locs.back().spec < 0);
							}
							break;
						}
				}
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_SPECIAL: //edit special
				set_special(spot_hit);
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_EDIT_TOWN_ENTRANCE: //edit town entry
				town_entry(spot_hit);
				overall_mode = MODE_DRAWING;
				set_cursor(wand_curs);
				break;
			case MODE_SET_OUT_START: //edit out start loc
				if(cChoiceDlog("set-out-start-confirm", {"okay", "cancel"}).show() == "cancel")
					break;
				if((spot_hit.x != minmax(4,43,spot_hit.x)) ||
					(spot_hit.y != minmax(4,43,spot_hit.y))) {
					giveError("You can't put the starting location this close to the edge of an outdoor section. It has to be at least 4 spaces away.");
					break;
				}
				scenario.out_sec_start.x = cur_out.x;
				scenario.out_sec_start.y = cur_out.y;
				scenario.out_start = spot_hit;
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				change_made = true;
				break;
			case MODE_ERASE_CREATURE: //delete monst
				for(x = 0; x < 60; x++)
					if(monst_on_space(spot_hit,x)) {
						town->creatures[x].number = 0;
						break;
					}
				while(town->creatures.back().number == 0)
					town->creatures.pop_back();
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_ERASE_ITEM: // delete item
				for(x = 0; x < town->preset_items.size(); x++)
					if((spot_hit.x == town->preset_items[x].loc.x) &&
						(spot_hit.y == town->preset_items[x].loc.y) && (town->preset_items[x].code >= 0)) {
						town->preset_items[x].code = -1;
						break;
					}
				while(town->preset_items.back().code == -1)
					town->preset_items.pop_back();
				set_cursor(wand_curs);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_SET_TOWN_START: // TODO: Implement this
				break;
			case MODE_INTRO_SCREEN:
			case MODE_EDIT_TYPES:
			case MODE_MAIN_SCREEN:
				break; // Nothing to do here, of course.
		}
		if((overall_mode == MODE_DRAWING) && (old_mode != MODE_DRAWING))
			set_string("Drawing mode",(char*)scenario.ter_types[current_terrain_type].name.c_str());
		draw_terrain();
		
	}
		if((the_point.in(border_rect[0])) & (cen_y > (editing_town ? 4 : 3))) {
			cen_y--;
			if(ctrl_hit)
				cen_y = ((editing_town) ? 4 : 3);
			need_redraw = true;
			mouse_button_held = true;
		}
		if((the_point.in(border_rect[1])) & (cen_x > (editing_town ? 4 : 3))) {
			cen_x--;
			if(ctrl_hit)
				cen_x = ((editing_town) ? 4 : 3);
			need_redraw = true;
			mouse_button_held = true;
		}
		if((the_point.in(border_rect[2])) & (cen_y < (editing_town ? town->max_dim() - 5 : 44))) {
			cen_y++;
			if(ctrl_hit)
				cen_y = (editing_town) ? town->max_dim() - 5 : 44;
			need_redraw = true;
			mouse_button_held = true;
		}
		if((the_point.in(border_rect[3])) & (cen_x < (editing_town ? town->max_dim() - 5 : 44))) {
			cen_x++;
			if(ctrl_hit)
				cen_x = (editing_town) ? town->max_dim() - 5 : 44;
			need_redraw = true;
			mouse_button_held = true;
		}
		if(need_redraw) {
			draw_terrain();
			place_location();
			need_redraw = false;
		}
	}
	
	if(!mouse_button_held && ((overall_mode < MODE_MAIN_SCREEN) || (overall_mode == MODE_EDIT_TYPES))) {
		cur_point = the_point;
		cur_point.x -= RIGHT_AREA_UL_X;
		cur_point.y -= RIGHT_AREA_UL_Y;
		
		for(i = 0; i < 256; i++)
			if(cur_point.in(terrain_rects[i])) {
				temp_rect = terrain_rects[i];
				temp_rect.offset(RIGHT_AREA_UL_X, RIGHT_AREA_UL_Y );
				flash_rect(temp_rect);
				if(overall_mode < MODE_MAIN_SCREEN) {
					set_new_terrain(i);
				}
				else {
					edit_ter_type(i);
					set_up_terrain_buttons();
				}
				place_location();
			}
		cur_point2 = cur_point;
		cur_point2.x -= 5;
		cur_point2.y -= terrain_rects[255].bottom + 5;
		for(i = 0; i < 10; i++)
			for(j = 0; j < 6; j++) {
				if(good_palette_buttons[editing_town][j][i] && !mouse_button_held && cur_point2.in(palette_buttons[i][j])
					&& /*((j < 3) || (editing_town)) &&*/ (overall_mode < MODE_MAIN_SCREEN)) {
					temp_rect = palette_buttons[i][j];
					temp_rect.offset(RIGHT_AREA_UL_X + 5, RIGHT_AREA_UL_Y + terrain_rects[255].bottom + 5);
					flash_rect(temp_rect);
					switch(i + 100 * j) {
						case 0:
							set_string("Drawing mode",(char*)scenario.ter_types[current_terrain_type].name.c_str());
							overall_mode = MODE_DRAWING;
							set_cursor(wand_curs);
							break;
						case 1:
							set_string("Paintbrush (large)",(char*)scenario.ter_types[current_terrain_type].name.c_str());
							overall_mode = MODE_LARGE_PAINTBRUSH;
							set_cursor(brush_curs);
							break;
						case 2:
							set_string("Paintbrush (small)",(char*)scenario.ter_types[current_terrain_type].name.c_str());
							set_cursor(brush_curs);
							overall_mode = MODE_SMALL_PAINTBRUSH;
							break;
						case 3:
							set_string("Spraycan (large)",(char*)scenario.ter_types[current_terrain_type].name.c_str());
							set_cursor(spray_curs);
							overall_mode = MODE_LARGE_SPRAYCAN;
							break;
						case 4:
							set_string("Spraycan (small)",(char*)scenario.ter_types[current_terrain_type].name.c_str());
							set_cursor(spray_curs);
							overall_mode = MODE_SMALL_SPRAYCAN;
							break;
						case 5:
							set_string("Eyedropper","Select terrain to draw");
							set_cursor(eyedropper_curs);
							overall_mode = MODE_EYEDROPPER;
							break;
						case 6: case 7:
							overall_mode = (i == 6) ? MODE_HOLLOW_RECT : MODE_FILLED_RECT;
							mode_count = 2;
							set_cursor(topleft_curs);
							if(i == 6)
								set_string("Fill rectangle (hollow)","Select upper left corner");
							else set_string("Fill rectangle (solid)","Select upper left corner");
							break;
						case 100: // switch view
							cur_viewing_mode = (cur_viewing_mode + 1) % 4;
							need_redraw = true;
							break;
						case 101:
							set_string("Erase space","Select space to clear");
							overall_mode = MODE_ERASER;
							set_cursor(eraser_curs);
							break;
						case 102:
							set_string("Edit sign","Select sign to edit");
							set_cursor(hand_curs);
							overall_mode = MODE_EDIT_SIGN;
							break;
						case 103:
							overall_mode = MODE_ROOM_RECT;
							mode_count = 2;
							set_cursor(topleft_curs);
							set_string("Create room rectangle","Select upper left corner");
							break;
						case 104:
							overall_mode = MODE_SET_WANDER_POINTS;
							mode_count = 4;
							set_cursor(hand_curs);
							set_string("Place first wandering monster arrival point","");
							break;
						case 105: // replace terrain
							swap_terrain();
							need_redraw = true;
							mouse_button_held = false;
							break;
						case 107:
							if(editing_town) {
								set_string("Can only set town entrances outdoors","");
								break;
							}
							set_string("Set town entrance","Select town to edit");
							set_cursor(hand_curs);
							overall_mode = MODE_EDIT_TOWN_ENTRANCE;
							break;
						case 200:
							if(!editing_town) {
								set_string("Edit placed item","Not while outdoors.");
								break;
							}
							set_string("Edit placed item","Select item to edit");
							set_cursor(hand_curs);
							overall_mode = MODE_EDIT_ITEM;
							break;
						case 201:
							if(!editing_town) {
								set_string("Edit placed item","Not while outdoors.");
								break;
							}
							set_string("Place same item","Select location");
							set_cursor(hand_curs);
							overall_mode = MODE_PLACE_SAME_ITEM;
							break;
						case 202:
							if(!editing_town) {
								set_string("Toggle special spot","Select location");
								overall_mode = MODE_TOGGLE_SPECIAL_DOT;
								set_cursor(wand_curs);
								break;
							}
							set_string("Delete an item","Select item");
							set_cursor(hand_curs);
							overall_mode = MODE_ERASE_ITEM;
							break;
						case 203:
							set_string("Create/Edit special","Select special location");
							set_cursor(hand_curs);
							overall_mode = MODE_EDIT_SPECIAL;
							break;
						case 204:
							set_string("Copy special","Select special to copy");
							set_cursor(hand_curs);
							overall_mode = MODE_COPY_SPECIAL;
							break;
						case 205:
							if(special_to_paste < 0) {
								set_string("Can't paste special","No special to paste");
							}
							set_string("Paste special","Select location to paste");
							overall_mode = MODE_PASTE_SPECIAL;
							set_cursor(hand_curs);
							break;
						case 206:
							set_string("Erase special","Select special to erase");
							overall_mode = MODE_ERASE_SPECIAL;
							set_cursor(eraser_curs);
							break;
						case 207:
							set_string("Set/place special","Select special location");
							set_cursor(hand_curs);
							overall_mode = MODE_PLACE_SPECIAL;
							break;
						case 300:
							set_string("Edit creature","Select creature to edit");
							set_cursor(hand_curs);
							overall_mode = MODE_EDIT_CREATURE;
							break;
						case 301:
							set_string("Place same creature","Select creature location");
							set_cursor(hand_curs);
							overall_mode = MODE_PLACE_SAME_CREATURE;
							break;
						case 302:
							set_string("Delete a creature","Select creature");
							set_cursor(eraser_curs);
							overall_mode = MODE_ERASE_CREATURE;
							break;
						case 304:
							set_string("Place north entrace","Select entrance location");
							set_cursor(hand_curs);
							overall_mode = MODE_PLACE_NORTH_ENTRANCE;
							break;
						case 305:
							set_string("Place west entrace","Select entrance location");
							set_cursor(hand_curs);
							overall_mode = MODE_PLACE_WEST_ENTRANCE;
							break;
						case 306:
							set_string("Place south entrace","Select entrance location");
							set_cursor(hand_curs);
							overall_mode = MODE_PLACE_SOUTH_ENTRANCE;
							break;
						case 307:
							set_string("Place east entrace","Select entrance location");
							set_cursor(hand_curs);
							overall_mode = MODE_PLACE_EAST_ENTRANCE;
							break;
						case 400:
							set_string("Place web","Select location");
							overall_mode = MODE_PLACE_WEB;
							set_cursor(wand_curs);
							break;
						case 401:
							set_string("Place crate","Select location");
							overall_mode = MODE_PLACE_CRATE;
							set_cursor(wand_curs);
							break;
						case 402:
							set_string("Place barrel","Select location");
							overall_mode = MODE_PLACE_BARREL;
							set_cursor(wand_curs);
							break;
						case 403:
							set_string("Place fire barrier","Select location");
							overall_mode = MODE_PLACE_FIRE_BARRIER;
							set_cursor(wand_curs);
							break;
						case 404:
							set_string("Place force barrier","Select location");
							overall_mode = MODE_PLACE_FORCE_BARRIER;
							set_cursor(wand_curs);
							break;
						case 405:
							set_string("Place quickfire","Select location");
							overall_mode = MODE_PLACE_QUICKFIRE;
							set_cursor(wand_curs);
							break;
						case 406:
							set_string("Place special spot","Select location");
							overall_mode = MODE_TOGGLE_SPECIAL_DOT;
							set_cursor(wand_curs);
							break;
						case 407:
							set_string("Clear space","Select space to clear");
							overall_mode = MODE_CLEAR_FIELDS;
							set_cursor(eraser_curs);
							break;
						case 500:
							set_string("Place small blood stain","Select stain location");
							overall_mode = MODE_PLACE_SFX;
							mode_count = 0;
							set_cursor(wand_curs);
							break;
						case 501:
							set_string("Place ave. blood stain","Select stain location");
							overall_mode = MODE_PLACE_SFX;
							mode_count = 1;
							set_cursor(wand_curs);
							break;
						case 502:
							set_string("Place large blood stain","Select stain location");
							overall_mode = MODE_PLACE_SFX;
							mode_count = 2;
							set_cursor(wand_curs);
							break;
						case 503:
							set_string("Place small slime pool","Select slime location");
							overall_mode = MODE_PLACE_SFX;
							mode_count = 3;
							set_cursor(wand_curs);
							break;
						case 504:
							set_string("Place large slime pool","Select slime location");
							overall_mode = MODE_PLACE_SFX;
							mode_count = 4;
							set_cursor(wand_curs);
							break;
						case 505:
							set_string("Place ash","Select ash location");
							overall_mode = MODE_PLACE_SFX;
							mode_count = 5;
							set_cursor(wand_curs);
							break;
						case 506:
							set_string("Place bones","Select bones location");
							overall_mode = MODE_PLACE_SFX;
							mode_count = 6;
							set_cursor(wand_curs);
							break;
						case 507:
							set_string("Place rocks","Select rocks location");
							overall_mode = MODE_PLACE_SFX;
							mode_count = 7;
							set_cursor(wand_curs);
							break;
					}
				}
			}
	}
	if(need_redraw) {
		draw_main_screen();
		draw_terrain();
	}
	
	return are_done;
}


void flash_rect(rectangle /*to_flash*/) {
	// TODO: Determine a good way to do this
//	InvertRect (&to_flash);
	play_sound(37);
	sf::sleep(time_in_ticks(10));
	
}



void swap_terrain() {
	short a,b,c,i,j;
	ter_num_t ter;
	
	if(!change_ter(a,b,c)) return;
	
	for(i = 0; i < ((editing_town) ? town->max_dim() : 48); i++)
		for(j = 0; j < ((editing_town) ? town->max_dim() : 48); j++) {
			ter = editing_town ? town->terrain(i,j) : current_terrain->terrain[i][j];
			if((ter == a) && (get_ran(1,1,100) <= c)) {
				if(editing_town)
					town->terrain(i,j) = b;
				else current_terrain->terrain[i][j] = b;
			}
		}
	
}

void set_new_terrain(ter_num_t selected_terrain) {
	current_terrain_type = selected_terrain;
	redraw_selected_ter();
//	if(selected_terrain < 2)
//		current_ground = 0;
//	else if(selected_terrain < 5)
//		current_ground = 2;
	current_ground = get_ground_from_ter(selected_terrain);
	set_string((char*)current_string,(char*)scenario.ter_types[current_terrain_type].name.c_str());
}

void handle_keystroke(sf::Event event) {
	using kb = sf::Keyboard;
	using Key = sf::Keyboard::Key;
	
	Key keypad[10] = {kb::Numpad0,kb::Numpad1,kb::Numpad2,kb::Numpad3,kb::Numpad4,kb::Numpad5,kb::Numpad6,kb::Numpad7,kb::Numpad8,kb::Numpad9};
	// TODO: The repetition of location shouldn't be needed here!
	location terrain_click[10] = {location{0,0},		// 0
		location{6,356}, location{140,356}, location{270,356},
		location{6,190}, location{140,190}, location{270,190},
		location{6,24}, location{140,20}, location{270,24},
	};
	location pass_point;
	Key chr2 = event.key.code;
	char chr;
	short i,j,store_ter;
	bool are_done;
	
	obscureCursor();
	
	if(overall_mode >= MODE_MAIN_SCREEN)
		return;
	
	for(i = 0; i < 10; i++)
		if(chr2 == keypad[i]) {
			if(i == 0) {
				chr = 'z';
			}
			else {
				pass_point = terrain_click[i];
				are_done = handle_action(pass_point,event);
				draw_terrain();
				mouse_button_held = false;
				return;
			}
		}
	
	store_ter = current_terrain_type;
	chr = keyToChar(chr2, event.key.shift);
	
	switch(chr) {
			/*
		case 'g':
			set_new_terrain(current_ground);
			break;
		case 'm':
			
			
		case 's':
			set_string("Mark special.");
			overall_mode = 6;
			break;
		case 'u':
			modify_lists();
			break;
		case 'y':
			overall_mode = 2;
			mode_count = 4;
			set_string("Place wandering.");
			break;
		case 'z':
			overall_mode = 46;
			set_string("Place same item.");
			break;
		case 'F':
			overall_mode = 1;
			mode_count = 2;
			set_string("Fill rect.");
			break;
			
		case 'i':
			
			working_rect.right = spot_hit.x;
			working_rect.bottom = spot_hit.y;
			overall_mode = 3;
			mode_count = 2;
			set_string("Create info rect.");
			break;*/
		//case 'e':
		//	set_string("Pick item to edit.");
		//	overall_mode = 5;
		//	break;
		/*case 'q':
			if(overall_mode != 7) {
				set_string("Place monster.");
				overall_mode = 7;
			}
			else {
				set_string("Place same monster.");
				overall_mode = 8;
			}
			break;
		case '`':
			set_string("Place same monster.");
			overall_mode = 8;
			break;*/
		case 'D':
			pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[0][0].left;
			pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[0][0].top;
			handle_action(pass_point,event);
			break;
		case 'R':
			pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[7][0].left;
			pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[7][0].top;
			handle_action(pass_point,event);
			break;
		case '1': case '2': case '3': case '4': case '5': case '6':
			pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[chr - 49][4].left;
			pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[chr - 48][4].top;
			handle_action(pass_point,event);
			break;
		case '0':
			pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[7][4].left;
			pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[7][4].top;
			handle_action(pass_point,event);
			break;
		case 'I':
			for(i = 0; i < town->preset_items.size(); i++) {
				if((town->preset_items[i].loc.x < 0) ||
					(town->preset_items[i].loc.y < 0))
					town->preset_items[i].code = -1;
				if(town->preset_items[i].code >= 0) {
					edit_placed_item(i);
				}
			}
			break;
		case '.':
			set_string("Pick item to edit.","");
			overall_mode = MODE_EDIT_ITEM;
			break;
		case '`':
			set_string("Place same creature","Select creature location");
			overall_mode = MODE_PLACE_SAME_CREATURE;
			break;
		case ',':
			set_string("Edit creature","Select creature to edit");
			overall_mode = MODE_EDIT_CREATURE;
			break;
		case '/':
			set_string("Place same item","Select location");
			overall_mode = MODE_PLACE_SAME_ITEM;
			break;
			
		default:
			if((chr >= 97) && (chr <= 122)) {
				for(i = 0; i < 256; i++) {
					j = current_terrain_type + i + 1;
					j = j % 256;
					if(scenario.ter_types[j].shortcut_key == chr) {
						set_new_terrain(j);
						i = 256;
					}
				}
				
			}
			break;
			
	}
	if(store_ter != current_terrain_type)
		draw_terrain();
	mouse_button_held = false;
}

void shy_change_circle_terrain(location center,short radius,ter_num_t terrain_type,short probability) {
	// prob is 0 - 20, 0 no, 20 always
	location l;
	short i,j;
	ter_num_t ter;
	
	for(i = 0; i < ((editing_town) ? town->max_dim() : 48); i++)
		for(j = 0; j < ((editing_town) ? town->max_dim() : 48); j++) {
			l.x = i;
			l.y = j;
			ter = editing_town ? town->terrain(i,j) : current_terrain->terrain[i][j];
			if((dist(center,l) <= radius) && (get_ran(1,1,20) <= probability)
				&& (ter < 5))
				set_terrain(l,terrain_type);
		}
}

void change_circle_terrain(location center,short radius,ter_num_t terrain_type,short probability) {
	// prob is 0 - 20, 0 no, 20 always
	location l;
	short i,j;
	
	for(i = 0; i < ((editing_town) ? town->max_dim() : 48); i++)
		for(j = 0; j < ((editing_town) ? town->max_dim() : 48); j++) {
			l.x = i;
			l.y = j;
			if((dist(center,l) <= radius) && (get_ran(1,1,20) <= probability))
				set_terrain(l,terrain_type);
		}
}

void change_rect_terrain(rectangle r,ter_num_t terrain_type,short probability,bool hollow) {
	// prob is 0 - 20, 0 no, 20 always
	location l;
	short i,j;
	
	for(i = 0; i < ((editing_town) ? town->max_dim() : 48); i++)
		for(j = 0; j < ((editing_town) ? town->max_dim() : 48); j++) {
			l.x = i;
			l.y = j;
			if((i >= r.left) && (i <= r.right) && (j >= r.top) && (j <= r.bottom)
				&& (!hollow || (i == r.left) || (i == r.right) || (j == r.top) || (j == r.bottom))
				&& ((hollow) || (get_ran(1,1,20) <= probability)))
				set_terrain(l,terrain_type);
		}
}

void frill_up_terrain() {
	short i,j;
	ter_num_t terrain_type;
	
	for(i = 0; i < ((editing_town) ? town->max_dim() : 48); i++)
		for(j = 0; j < ((editing_town) ? town->max_dim() : 48); j++) {
			terrain_type = editing_town ? town->terrain(i,j) : current_terrain->terrain[i][j];
			
			if((terrain_type == 2) && (get_ran(1,1,20) < 3))
				terrain_type = 3;
			if((terrain_type == 2) && (get_ran(1,1,20) < 2))
				terrain_type = 4;
			if((terrain_type == 0) && (get_ran(1,1,20) < 2))
				terrain_type = 1;
			if((terrain_type == 36) && (get_ran(1,1,20) < 5))
				terrain_type = 37;
			if(editing_town)
				town->terrain(i,j) = terrain_type;
			else current_terrain->terrain[i][j] = terrain_type;
		}
	draw_terrain();
}

void unfrill_terrain() {
	short i,j;
	ter_num_t terrain_type;
	
	for(i = 0; i < ((editing_town) ? town->max_dim() : 48); i++)
		for(j = 0; j < ((editing_town) ? town->max_dim() : 48); j++) {
			terrain_type = editing_town ? town->terrain(i,j) : current_terrain->terrain[i][j];
			if(terrain_type == 3)
				terrain_type = 2;
			if(terrain_type == 4)
				terrain_type = 2;
			if(terrain_type == 1)
				terrain_type = 0;
			if(terrain_type == 37)
				terrain_type = 36;
			if(editing_town)
				town->terrain(i,j) = terrain_type;
			else current_terrain->terrain[i][j] = terrain_type;
		}
	draw_terrain();
}

static ter_num_t find_object_part(unsigned char num, short x, short y, ter_num_t fallback){
	for(int i = 0; i < 256; i++){
		if(scenario.ter_types[i].obj_num == num &&
		   scenario.ter_types[i].obj_pos.x == x &&
		   scenario.ter_types[i].obj_pos.y == y)
			return i;
	}
	return fallback;
}

ter_num_t get_ground_from_ter(ter_num_t ter){
	unsigned char ground = scenario.ter_types[ter].ground_type;
	for(int i = 0; i < 256; i++)
		if(scenario.ter_types[i].ground_type == ground)
			return i;
	return 0;
}

bool terrain_matches(unsigned char x, unsigned char y, ter_num_t ter){
	ter_num_t ter2;
	if(editing_town) ter2 = town->terrain(x,y); else ter2 = current_terrain->terrain[x][y];
	if(ter2 == ter) return true;
	if(scenario.ter_types[ter2].ground_type != scenario.ter_types[ter].ground_type)
		return false;
	if(scenario.ter_types[ter].trim_type == eTrimType::NONE &&
	   scenario.ter_types[ter2].trim_type >= eTrimType::S &&
	   scenario.ter_types[ter2].trim_type <= eTrimType::NW_INNER)
		return ter == get_ground_from_ter(ter);
	if(scenario.ter_types[ter2].trim_type == eTrimType::NONE &&
	   scenario.ter_types[ter].trim_type >= eTrimType::S &&
	   scenario.ter_types[ter].trim_type <= eTrimType::NW_INNER)
		return ter2 == get_ground_from_ter(ter2);
	if(scenario.ter_types[ter2].trim_type >= eTrimType::S &&
	   scenario.ter_types[ter2].trim_type <= eTrimType::NW_INNER &&
	   scenario.ter_types[ter].trim_type >= eTrimType::S &&
	   scenario.ter_types[ter].trim_type <= eTrimType::NW_INNER)
		return true;
	return false;
}

static const std::array<location,5> trim_diffs = {{
	loc(0,0), loc(-1,0), loc(1,0), loc(0,-1), loc(0,1)
}};

void set_terrain(location l,ter_num_t terrain_type) {
	short i,j,which_sign = -1;
	ter_num_t ter;
	location l2;
	
	i = l.x;
	j = l.y;
	//if((l.x < 0) || (l.x > town->max_dim() - 1) || (l.y < 0) || (l.y > town->max_dim() - 1))
	//	return;
	if((editing_town) && ((i < 0) || (i > town->max_dim() - 1) || (j < 0) || (j > town->max_dim() - 1)))
		return ;
	if(!editing_town && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return ;
	
	if(editing_town)
		town->terrain(i,j) = terrain_type;
	else current_terrain->terrain[i][j] = terrain_type;
	l2 = l;
	
	// Large objects (eg rubble)
	if(scenario.ter_types[terrain_type].obj_num > 0){
		int q = scenario.ter_types[terrain_type].obj_num;
		location obj_loc = scenario.ter_types[terrain_type].obj_pos;
		location obj_dim = scenario.ter_types[terrain_type].obj_size;
		while(obj_loc.x > 0) l2.x-- , obj_loc.x--;
		while(obj_loc.y > 0) l2.y-- , obj_loc.y--;
		for(i = 0; i < obj_dim.x; i++)
			for(j = 0; j < obj_dim.y; j++){
				if(editing_town)
					town->terrain(l2.x + i,l2.y + j) = find_object_part(q,i,j,terrain_type);
				else current_terrain->terrain[l2.x + i][l2.y + j] = find_object_part(q,i,j,terrain_type);
			}
	}
	
	// First make sure surrounding spaces have the correct ground types.
	// This should handle the case of placing hills around mountains.
	unsigned int main_ground = scenario.ter_types[terrain_type].ground_type;
	long trim_ground = scenario.ter_types[terrain_type].trim_ter;
	for(int x = -1; x <= 1; x++) {
		for(int y = -1; y <= 1; y++) {
			location l3(l.x+x,l.y+y);
			ter_num_t ter_there = editing_town ? town->terrain(l3.x,l3.y) : current_terrain->terrain[l3.x][l3.y];
			unsigned int ground_there = scenario.ter_types[ter_there].ground_type;
			if(ground_there != main_ground && ground_there != trim_ground) {
				ter_num_t new_ter = scenario.get_ter_from_ground(trim_ground);
				cTerrain& ter_type = scenario.ter_types[new_ter];
				// We need to be very cautious here.
				// Only make the change if the terrain already there is the archetype for the ground type
				// that is the trim terrain of the terrain we're trying to place.
				// Otherwise it might overwrite important things, like buildings or forests.
				if(ter_there != scenario.get_ter_from_ground(ter_type.trim_ter))
					continue;
				if(editing_town)
					town->terrain(l3.x,l3.y) = new_ter;
				else current_terrain->terrain[l3.x][l3.y] = new_ter;
			}
		}
	}
	
	// Adjusting terrains with trim
	for(location d : trim_diffs) {
		location l3(l.x+d.x, l.y+d.y);
		adjust_space(l3);
	}
	
	if(scenario.ter_types[terrain_type].special == eTerSpec::IS_A_SIGN && editing_town) {
		for(i = 0; i < town->sign_locs.size(); i++)
			if(which_sign < 0) {
				if((town->sign_locs[i].x == l.x) && (town->sign_locs[i].y == l.y))
					which_sign = i;
			}
		for(i = 0; i < town->sign_locs.size(); i++)
			if(which_sign < 0) {
				if(town->sign_locs[i].x == 100)
					which_sign = i;
				else {
					ter = town->terrain(town->sign_locs[i].x,town->sign_locs[i].y);
					if(scenario.ter_types[ter].special != eTerSpec::IS_A_SIGN)
						which_sign = i;
				}
			}
		if(which_sign >= 0) {
			static_cast<location&>(town->sign_locs[which_sign]) = l;
			edit_sign(which_sign,scenario.ter_types[terrain_type].picture);
			sign_error_received = false;
		}
		else {
			town->terrain(l.x,l.y) = current_ground;
			if(!sign_error_received) {
				giveError("Towns can have at most 15 signs. Outdoor sections can have at most 8. When the party looks at this sign, they will get no message.");
				sign_error_received = true;
			}
		}
		mouse_button_held = false;
	}
	if(scenario.ter_types[terrain_type].special == eTerSpec::IS_A_SIGN && !editing_town) {
		if((l.x == 0) || (l.x == 47) || (l.y == 0) || (l.y == 47)) {
			cChoiceDlog("not-at-edge").show();
			mouse_button_held = false;
			return;
		}
		for(i = 0; i < current_terrain->sign_locs.size(); i++)
			if(which_sign < 0) {
				if((current_terrain->sign_locs[i].x == l.x) && (current_terrain->sign_locs[i].y == l.y))
					which_sign = i;
			}
		for(i = 0; i < current_terrain->sign_locs.size(); i++)
			if(which_sign < 0) {
				if(current_terrain->sign_locs[i].x == 100)
					which_sign = i;
				else {
					ter = current_terrain->terrain[current_terrain->sign_locs[i].x][current_terrain->sign_locs[i].y];
					if(scenario.ter_types[ter].special != eTerSpec::IS_A_SIGN)
						which_sign = i;
				}
			}
		if(which_sign >= 0) {
			static_cast<location&>(current_terrain->sign_locs[which_sign]) = l;
			edit_sign(which_sign,scenario.ter_types[terrain_type].picture);
			sign_error_received = false;
		}
		else {
			current_terrain->terrain[l.x][l.y] = current_ground;
			if(!sign_error_received) {
				giveError("Towns can have at most 15 signs. Outdoor sections can have at most 8. When the party looks at this sign, they will get no message.");
				sign_error_received = true;
			}
		}
		mouse_button_held = false;
	}
}

void adjust_space(location l) {
	bool needed_change = false;
	location l2;
	short i,j;
	
	i = l.x;
	j = l.y;
	if((editing_town) && ((i < 0) || (i > town->max_dim() - 1) || (j < 0) || (j > town->max_dim() - 1)))
		return ;
	if(!editing_town && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return ;
	size_t size = editing_town ? town->max_dim() : 48;
	ter_num_t off_map = -1;
	
	ter_num_t store_ter[3][3];
	long store_ter2[3][3];
	unsigned int store_ground[3][3];
	eTrimType store_trim[3][3];
	for(int dx = -1; dx <= 1; dx++) {
		for(int dy = -1; dy <= 1; dy++) {
			int x = i + dx, y = j + dy;
			if(x < 0 || x >= size || y < 0 || y >= size) {
				store_ter[dx+1][dy+1] = off_map;
				continue;
			}
			store_ter[dx+1][dy+1] = editing_town ? town->terrain(x,y) : current_terrain->terrain[x][y];
			cTerrain& ter_type = scenario.ter_types[store_ter[dx+1][dy+1]];
			store_ter2[dx+1][dy+1] = ter_type.trim_ter;
			store_ground[dx+1][dy+1] = ter_type.ground_type;
			store_trim[dx+1][dy+1] = ter_type.trim_type;
		}
	}
	
	// Correctable spaces are recognizable by having a trim terrain...
	if(store_ter2[1][1] < 0) return;
	// ...as well as a particular subset of trim types.
	if(store_trim[1][1] >= eTrimType::FRILLS || store_trim[1][1] == eTrimType::WALL)
		return;
	
	bool have_wall = scenario.ter_types[store_ter[1][1]].blockage >= eTerObstruct::BLOCK_MOVE;
	
	unsigned int main_ground = store_ground[1][1];
	long trim_ground = store_ter2[1][1];
	
	auto is_same_ter = [&](int x,int y) -> bool {
		if(store_ground[x][y] == main_ground)
			return true;
		if(store_ter2[x][y] == main_ground)
			return true;
		if(store_ter[x][y] == off_map)
			return true;
		if(!have_wall)
			return false;
		if(store_trim[x][y] == eTrimType::WATERFALL)
			return true;
		if(store_trim[x][y] == eTrimType::WALL)
			return true;
		return false;
	};
	
	// Then go through and figure out what the trim type of the centre should be.
	eTrimType need_trim = eTrimType::NONE;
	if(!is_same_ter(0,1)) {
		if(!is_same_ter(1,0)) {
			need_trim = eTrimType::NW;
		} else if(!is_same_ter(1,2)) {
			need_trim = eTrimType::SW;
		} else need_trim = eTrimType::W;
	} else if(!is_same_ter(2,1)) {
		if(!is_same_ter(1,0)) {
			need_trim = eTrimType::NE;
		} else if(!is_same_ter(1,2)) {
			need_trim = eTrimType::SE;
		} else need_trim = eTrimType::E;
	} else if(!is_same_ter(1,0))
		need_trim = eTrimType::N;
	else if(!is_same_ter(1,2))
		need_trim = eTrimType::S;
	else if(!is_same_ter(0,0))
		need_trim = eTrimType::SE_INNER;
	else if(!is_same_ter(0,2))
		need_trim = eTrimType::NE_INNER;
	else if(!is_same_ter(2,0))
		need_trim = eTrimType::SW_INNER;
	else if(!is_same_ter(2,2))
		need_trim = eTrimType::NW_INNER;
	
	if(store_trim[1][1] != need_trim) {
		ter_num_t replace = scenario.get_trim_terrain(main_ground, trim_ground, need_trim);
		if(replace != 90) { // If we got 90 back, the required trim doesn't exist.
			needed_change = true;
			if(editing_town)
				town->terrain(i,j) = replace;
			else current_terrain->terrain[i][j] = replace;
		}
	}
	
	if(needed_change) {
		for(location d : trim_diffs) {
			if(d.x == 0 && d.y == 0) continue;
			location l2(l.x+d.x, l.y+d.y);
			adjust_space(l2);
		}
	}
	
}

bool place_item(location spot_hit,short which_item,bool property,bool always,short odds)  {
	// odds 0 - 100, with 100 always
	
	short x;
	
	if((which_item < 0) || (which_item > 399))
		return true;
	if(scenario.scen_items[which_item].variety == eItemType::NO_ITEM)
		return true;
	if(get_ran(1,1,100) > odds)
		return false;
	for(x = 0; x < town->preset_items.size(); x++)
		if(town->preset_items[x].code < 0) {
			town->preset_items[x] = {spot_hit, which_item, scenario.scen_items[which_item]};
			town->preset_items[x].contained = container_there(spot_hit);
			return true;
		}
	town->preset_items.push_back({spot_hit, which_item, scenario.scen_items[which_item]});
	town->preset_items.back().contained = container_there(spot_hit);
	return true;
}

void place_items_in_town() {
	location l;
	short i,j,k,x;
	bool place_failed = false;
	
	for(i = 0; i < town->max_dim(); i++)
		for(j = 0; j < town->max_dim(); j++) {
			l.x = i;
			l.y = j;
			
			for(k = 0; k < 10; k++)
				if(town->terrain(i,j) == scenario.storage_shortcuts[k].ter_type) {
					for(x = 0; x < 10; x++)
						if(place_item(l,scenario.storage_shortcuts[k].item_num[x],
									   scenario.storage_shortcuts[k].property,false,
									   scenario.storage_shortcuts[k].item_odds[x]) == false)
							place_failed = true;
				}
		}
	if(place_failed)
		giveError("Not all of the random items could be placed. The preset item per town limit of 64 was reached.");
	draw_terrain();
}

void place_edit_special(location loc) {
	short i,spec;
	
	if(editing_town) {
		for(i = 0; i < town->special_locs.size(); i++)
			if(town->special_locs[i] == loc && town->special_locs[i].spec >= 0) {
				edit_spec_enc(town->special_locs[i].spec,2,nullptr);
				break;
			}
		if(i == town->special_locs.size()) { // new special
			spec = get_fresh_spec(2);
			for(i = 0; i <= town->special_locs.size(); i++) {
				if(i == town->special_locs.size())
					town->special_locs.emplace_back(-1,-1,-1);
				if(town->special_locs[i].spec < 0) {
					if(edit_spec_enc(spec,2,nullptr)) {
						town->special_locs[i] = loc;
						town->special_locs[i].spec = spec;
					}
					break;
				}
			}
		}
	}
	
	if(!editing_town) {
		if((loc.x == 0) || (loc.x == 47) || (loc.y == 0) || (loc.y == 47)) {
			cChoiceDlog("not-at-edge").show();
			return;
		}
		for(i = 0; i < current_terrain->special_locs.size(); i++)
			if(current_terrain->special_locs[i] == loc && current_terrain->special_locs[i].spec >= 0) {
				edit_spec_enc(current_terrain->special_locs[i].spec,1,nullptr);
				break;
			}
		if(i == current_terrain->special_locs.size()) { // new special
			spec = get_fresh_spec(1);
			for(i = 0; i <= current_terrain->special_locs.size(); i++) {
				if(i == current_terrain->special_locs.size())
					current_terrain->special_locs.emplace_back(-1,-1,-1);
				if(current_terrain->special_locs[i].spec < 0) {
					if(edit_spec_enc(spec,1,nullptr)) {
						current_terrain->special_locs[i] = loc;
						current_terrain->special_locs[i].spec = spec;
					}
					break;
				}
			}
		}
	}
	
}

void set_special(location spot_hit) {
	short x,y;
	
	if(editing_town) {
		for(x = 0; x < town->special_locs.size(); x++)
			if(town->special_locs[x] == spot_hit && town->special_locs[x].spec >= 0) {
				y = edit_special_num(2,town->special_locs[x].spec);
				if(y >= 0) town->special_locs[x].spec = y;
				break;
			}
		if(x == town->special_locs.size()) {
			for(x = 0; x <= town->special_locs.size(); x++) {
				if(x == town->special_locs.size())
					town->special_locs.emplace_back(-1,-1,-1);
				if(town->special_locs[x].spec < 0) {
					y = edit_special_num(2,0);
					if(y >= 0) {
						town->special_locs[x] = spot_hit;
						town->special_locs[x].spec = y;
					}
					break;
				}
			}
		}
	}
	if(!editing_town) {
		if((spot_hit.x == 0) || (spot_hit.x == 47) || (spot_hit.y == 0) || (spot_hit.y == 47)) {
			cChoiceDlog("not-at-edge").show();
			return;
		}
		for(x = 0; x < current_terrain->special_locs.size(); x++)
			if(current_terrain->special_locs[x] == spot_hit && current_terrain->special_locs[x].spec >= 0) {
				y = edit_special_num(1,current_terrain->special_locs[x].spec);
				if(y >= 0) current_terrain->special_locs[x].spec = y;
				break;
			}
		if(x == current_terrain->special_locs.size()) {
			for(x = 0; x <= current_terrain->special_locs.size(); x++) {
				if(x == current_terrain->special_locs.size())
					current_terrain->special_locs.emplace_back(-1,-1,-1);
				if(current_terrain->special_locs[x].spec < 0) {
					y = edit_special_num(1,current_terrain->special_locs[x].spec);
					if(y >= 0) {
						current_terrain->special_locs[x] = spot_hit;
						current_terrain->special_locs[x].spec = y;
					}
					break;
				}
			}
		}
	}
	
}

void town_entry(location spot_hit) {
	short x,y;
	ter_num_t ter;
	
	ter = current_terrain->terrain[spot_hit.x][spot_hit.y];
	if(scenario.ter_types[ter].special != eTerSpec::TOWN_ENTRANCE) {
		giveError("This space isn't a town entrance. Town entrances are marked by a small brown castle icon.");
		return;
	}
	// clean up old town entries
	for(x = 0; x < current_terrain->city_locs.size(); x++)
		if(current_terrain->city_locs[x].spec >= 0) {
			ter = current_terrain->terrain[current_terrain->city_locs[x].x][current_terrain->city_locs[x].y];
			if(scenario.ter_types[ter].special != eTerSpec::TOWN_ENTRANCE)
				current_terrain->city_locs[x].spec = -1;
		}
	y = -2;
	for(x = 0; x < current_terrain->city_locs.size(); x++)
		if(current_terrain->city_locs[x] == spot_hit) {
			y = pick_town_num("select-town-enter",current_terrain->city_locs[x].spec,scenario);
			if(y >= 0) current_terrain->city_locs[x].spec = y;
		}
	if(y == -2) {
		for(x = 0; x < current_terrain->city_locs.size(); x++)
			if(current_terrain->city_locs[x].spec < 0) {
				y = pick_town_num("select-town-enter",0,scenario);
				if(y >= 0) {
					current_terrain->city_locs[x].spec = y;
					current_terrain->city_locs[x] = spot_hit;
				}
				x = 500;
			}
		if(y == -2)
			giveError("You can't set more than 8 town entrances in any outdoor section.");
	}
}

// is slot >= 0, force that slot
// if -1, use 1st free slot
void set_up_start_screen() {
	set_lb(0,LB_TITLE,LB_NO_ACTION,"Blades of Exile");
	set_lb(1,LB_TITLE,LB_NO_ACTION,"Scenario Editor");
	set_lb(3,LB_TEXT,LB_NEW_SCEN,"Make New Scenario");
	set_lb(4,LB_TEXT,LB_LOAD_SCEN,"Load Scenario");
	set_lb(7,LB_TEXT,LB_NO_ACTION,"To find out how to use the");
	set_lb(8,LB_TEXT,LB_NO_ACTION,"editor, select Getting Started ");
	set_lb(9,LB_TEXT,LB_NO_ACTION,"from the Help menu.");
	set_lb(NLS - 5,LB_TEXT,LB_NO_ACTION,"Be sure to read the file Blades");
	set_lb(NLS - 4,LB_TEXT,LB_NO_ACTION,"of Exile License. Using this");
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"program implies that you agree ");
	set_lb(NLS - 2,LB_TEXT,LB_NO_ACTION,"with the terms of the license.");
	set_lb(NLS - 1,LB_TEXT,LB_NO_ACTION,"Copyright 1997, All rights reserved.");
}

void set_up_main_screen() {
	std::ostringstream strb;
	
	reset_lb();
	reset_rb();
	set_lb(-1,LB_TITLE,LB_NO_ACTION,"Blades of Exile");
	set_lb(-1,LB_TEXT,LB_NO_ACTION,"Scenario Options");
	set_lb(-1,LB_TEXT,LB_EDIT_TER,"Edit Terrain Types");
	set_lb(-1,LB_TEXT,LB_EDIT_MONST,"Edit Monsters");
	set_lb(-1,LB_TEXT,LB_EDIT_ITEM,"Edit Items");
	set_lb(-1,LB_TEXT,LB_NEW_TOWN,"Create New Town");
	set_lb(-1,LB_TEXT,LB_EDIT_TEXT,"Edit Scenario Text");
	set_lb(-1,LB_TEXT,LB_EDIT_SPECITEM,"Edit Special Items");
	set_lb(-1,LB_TEXT,LB_EDIT_QUEST,"Edit Quests");
	set_lb(-1,LB_TEXT,LB_EDIT_SHOPS,"Edit Shops");
	set_lb(-1,LB_TEXT,LB_NO_ACTION,"");
	set_lb(-1,LB_TEXT,LB_NO_ACTION,"Outdoors Options");
	strb << "  Section x = " << cur_out.x << ", y = " << cur_out.y;
	set_lb(-1,LB_TEXT,LB_NO_ACTION, strb.str());
	set_lb(-1,LB_TEXT,LB_LOAD_OUT,"Load New Section");
	set_lb(-1,LB_TEXT,LB_EDIT_OUT,"Edit Outdoor Terrain");
	set_lb(-1,LB_TEXT,LB_NO_ACTION,"",0);
	set_lb(-1,LB_TEXT,LB_NO_ACTION,"Town/Dungeon Options");
	strb.str("");
	strb << "  Town " << cur_town << ": " << town->town_name;
	set_lb(-1,LB_TEXT,LB_NO_ACTION, strb.str());
	set_lb(-1,LB_TEXT,LB_LOAD_TOWN,"Load Another Town");
	set_lb(-1,LB_TEXT,LB_EDIT_TOWN,"Edit Town Terrain");
	set_lb(-1,LB_TEXT,LB_EDIT_TALK,"Edit Town Dialogue");
	set_lb(NLS - 1,LB_TEXT,LB_NO_ACTION,"Copyright 1997, All rights reserved.");
	overall_mode = MODE_MAIN_SCREEN;
	right_sbar->show();
	shut_down_menus(4);
	shut_down_menus(3);
	redraw_screen();
}

void start_town_edit() {
	short i,j;
	std::ostringstream strb;
	small_any_drawn = false;
	cen_x = town->max_dim() / 2;
	cen_y = town->max_dim() / 2;
	reset_lb();
	strb << "Editing Town " << cur_town;
	set_lb(0,LB_TITLE,LB_NO_ACTION,strb.str());
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,town->town_name);
	set_lb(NLS - 2,LB_TEXT,LB_NO_ACTION,"(Click border to scroll view.)");
	set_lb(NLS - 1,LB_TEXT,LB_RETURN,"Back to Main Menu");
	overall_mode = MODE_DRAWING;
	editing_town = true;
	set_up_terrain_buttons();
	shut_down_menus(4);
	shut_down_menus(2);
	right_sbar->hide();
	set_string("Drawing mode",(char*)scenario.ter_types[current_terrain_type].name.c_str());
	place_location();
	copied_spec = -1;
	for(i = 0; i < town->max_dim(); i++)
		for(j = 0; j < town->max_dim(); j++)
			if(town->terrain(i,j) == 0)
				current_ground = 0;
			else if(town->terrain(i,j) == 2)
				current_ground = 2;
}

void start_out_edit() {
	short i,j;
	std::ostringstream strb;
	small_any_drawn = false;
	cen_x = 24;
	cen_y = 24;
	reset_lb();
	strb << "Editing outdoors (" << cur_out.x << ',' << cur_out.y << ")";
	set_lb(0,LB_TITLE,LB_NO_ACTION,strb.str());
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,current_terrain->out_name);
	set_lb(NLS - 2,LB_TEXT,LB_NO_ACTION,"(Click border to scroll view.)");
	set_lb(NLS - 1,LB_TEXT,LB_RETURN,"Back to Main Menu");
	overall_mode = MODE_DRAWING;
	editing_town = false;
	set_up_terrain_buttons();
	right_sbar->hide();
	shut_down_menus(4);
	shut_down_menus(1);
	redraw_screen();
	set_string("Drawing mode",(char*)scenario.ter_types[current_terrain_type].name.c_str());
	place_location();
	copied_spec = -1;
	for(i = 0; i < 48; i++)
		for(j = 0; j < 48; j++)
			if(current_terrain->terrain[i][j] == 0)
				current_ground = 0;
			else if(current_terrain->terrain[i][j] == 2)
				current_ground = 2;
}

void start_terrain_editing() {
	right_sbar->hide();
	overall_mode = MODE_EDIT_TYPES;
	set_up_terrain_buttons();
	place_location();
	
	set_lb(NLS - 3,LB_CLEAR,LB_NO_ACTION,"",true);
}

void start_monster_editing(short just_redo_text) {
	short i;
	bool draw_full = false;
	
	if(overall_mode == MODE_EDIT_TYPES)
		draw_full = true;
	if(just_redo_text == 0) {
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		right_sbar->setPosition(0);
		
		reset_rb();
		right_sbar->setMaximum(255 - NRSONPAGE);
	}
	for(i = 1; i < 256; i++) {
		set_rb(i - 1,RB_MONST, i,std::to_string(i) + " - " + scenario.scen_monsters[i].m_name);
	}
	if(draw_full)
		redraw_screen();
	else for(i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i,0);
	set_lb(NLS - 3,LB_CLEAR,LB_NO_ACTION,"",true);
}

void start_item_editing(short just_redo_text) {
	short i;
	bool draw_full = false;
	
	if(just_redo_text == 0) {
		if(overall_mode < MODE_MAIN_SCREEN)
			set_up_main_screen();
		if(overall_mode == MODE_EDIT_TYPES)
			draw_full = true;
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		
		right_sbar->setPosition(0);
		reset_rb();
		right_sbar->setMaximum(400 - NRSONPAGE);
	}
	for(i = 0; i < 400; i++) {
		set_rb(i,RB_ITEM, i,std::to_string(i) + " - " + scenario.scen_items[i].full_name);
	}
	if(draw_full)
		redraw_screen();
	else for(i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i,0);
	set_lb(NLS - 3,LB_CLEAR,LB_NO_ACTION,"",true);
}

void start_special_item_editing() {
	short i;
	bool draw_full = false;
	
	if(overall_mode < MODE_MAIN_SCREEN)
		set_up_main_screen();
	if(overall_mode == MODE_EDIT_TYPES)
		draw_full = true;
	overall_mode = MODE_MAIN_SCREEN;
	right_sbar->show();
	
	right_sbar->setPosition(0);
	reset_rb();
	right_sbar->setMaximum(50 - NRSONPAGE);
	for(i = 0; i < 50; i++) {
		set_rb(i,RB_SPEC_ITEM, i,std::to_string(i) + " - " + scenario.special_items[i].name);
	}
	if(draw_full)
		redraw_screen();
	else for(i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i,0);
	set_lb(NLS - 3,LB_CLEAR,LB_NO_ACTION,"",true);
}

void start_quest_editing() {
	int num_options = scenario.quests.size() + 1;
	if(overall_mode < MODE_MAIN_SCREEN)
		set_up_main_screen();
	overall_mode = MODE_MAIN_SCREEN;
	right_sbar->show();
	right_sbar->setPosition(0);
	reset_rb();
	right_sbar->setMaximum(num_options - NRSONPAGE);
	for(int i = 0; i < num_options; i++) {
		std::string title;
		if(i == scenario.quests.size())
			title = "Create New Quest";
		else title = scenario.quests[i].name;
		title = std::to_string(i) + " - " + title;
		set_rb(i, RB_QUEST, i, title);
	}
	redraw_screen();
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Command-click or right-click to delete",true);
}

void start_shops_editing() {
	int num_options = scenario.shops.size() + 1;
	if(overall_mode < MODE_MAIN_SCREEN)
		set_up_main_screen();
	overall_mode = MODE_MAIN_SCREEN;
	right_sbar->show();
	right_sbar->setPosition(0);
	reset_rb();
	right_sbar->setMaximum(num_options - NRSONPAGE);
	for(int i = 0; i < num_options; i++) {
		std::string title;
		if(i == scenario.shops.size())
			title = "Create New Shop";
		else title = scenario.shops[i].getName();
		title = std::to_string(i) + " - " + title;
		set_rb(i, RB_SHOP, i, title);
	}
	redraw_screen();
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Command-click or right-click to delete",true);
}

extern size_t num_strs(short mode); // defined in scen.keydlgs.cpp

// mode 0 - scen 1 - out 2 - town 3 - journal
// if just_redo_text not 0, simply need to update text portions
void start_string_editing(short mode,short just_redo_text) {
	long pos;
	bool draw_full = false;
	
	if(just_redo_text == 0) {
		if(overall_mode < MODE_MAIN_SCREEN)
			set_up_main_screen();
		if(overall_mode == MODE_EDIT_TYPES)
			draw_full = true;
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		
		reset_rb();
		right_sbar->setMaximum(num_strs(mode) - NRSONPAGE);
	}
	for(size_t i = 0; i < num_strs(mode); i++) {
		std::ostringstream str;
		switch(mode) {
			case 0:
				str << i << " - " << scenario.spec_strs[i].substr(0,30);
				set_rb(i,RB_SCEN_STR, i,str.str());
				break;
			case 1:
				str << i << " - " << current_terrain->spec_strs[i].substr(0,30);
				set_rb(i,RB_OUT_STR, i,str.str());
				break;
			case 2:
				str << i << " - " << town->spec_strs[i].substr(0,30);
				set_rb(i,RB_TOWN_STR, i,str.str());
				break;
			case 3:
				str << i << " - " << scenario.journal_strs[i].substr(0,30);
				set_rb(i,RB_JOURNAL, i,str.str());
				break;
			case 4:
				str << i << " - " << current_terrain->sign_locs[i].text.substr(0,30);
				set_rb(i,RB_OUT_SIGN, i,str.str());
				break;
			case 5:
				str << i << " - " << town->sign_locs[i].text.substr(0,30);
				set_rb(i,RB_TOWN_SIGN, i,str.str());
				break;
		}
	}
	
	pos = right_sbar->getPosition();
	if(draw_full)
		redraw_screen();
	else for(int i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i + pos,0);
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Command-click to clear",true);
}

// mode 0 - scen 1 - out 2 - town
// if just_redo_text not 0, simply need to update text portions
void start_special_editing(short mode,short just_redo_text) {
	short i;
	char str[256];
	size_t num_specs;
	bool draw_full = false;
	switch(mode) {
		case 0: num_specs = scenario.scen_specials.size(); break;
		case 1: num_specs = current_terrain->specials.size(); break;
		case 2: num_specs = town->specials.size(); break;
	}
	
	if(just_redo_text == 0) {
		if(overall_mode < MODE_MAIN_SCREEN)
			set_up_main_screen();
		if(overall_mode == MODE_EDIT_TYPES)
			draw_full = true;
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		
		reset_rb();
		right_sbar->setMaximum(num_specs + 1 - NRSONPAGE);
	}
	
	for(size_t i = 0; i < num_specs; i++) {
		std::ostringstream strb;
		switch(mode) {
			case 0:
				strb << i << " - " << (*scenario.scen_specials[i].type).name();
				set_rb(i,RB_SCEN_SPEC, i, strb.str());
				break;
			case 1:
				strb << i << " - " << (*current_terrain->specials[i].type).name();
				set_rb(i,RB_OUT_SPEC, i, strb.str());
				break;
			case 2:
				strb << i << " - " << (*town->specials[i].type).name();
				set_rb(i,RB_TOWN_SPEC, i, strb.str());
				break;
		}
	}
	std::string make_new = std::to_string(num_specs) + " - Create New Special";
	switch(mode) {
		case 0: set_rb(num_specs, RB_SCEN_SPEC, num_specs, make_new); break;
		case 1: set_rb(num_specs, RB_OUT_SPEC, num_specs, make_new); break;
		case 2: set_rb(num_specs, RB_TOWN_SPEC, num_specs, make_new); break;
	}
	if(draw_full)
		redraw_screen();
	else for(i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i,0);
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Command-click to clear",true);
}

// if restoring is 1, this is just a redraw, so don't move scroll bar position
void start_dialogue_editing(short restoring) {
	short i,j;
	char s[15] = "    ,      ";
	bool draw_full = false;
	
	if(overall_mode < MODE_MAIN_SCREEN)
		set_up_main_screen();
	if(overall_mode == MODE_EDIT_TYPES)
		draw_full = true;
	overall_mode = MODE_MAIN_SCREEN;
	right_sbar->show();
	
	if(restoring == 0) {
		right_sbar->setPosition(0);
		reset_rb();
		right_sbar->setMaximum(70 - NRSONPAGE);
	}
	for(i = 0; i < 10; i++) {
		std::ostringstream strb;
		strb << "Personality " << (i + cur_town * 10) << " - " << town->talking.people[i].title;
		set_rb(i,RB_PERSONALITY, i, strb.str());
	}
	for(i = 0; i < 60; i++) {
		for(j = 0; j < 4; j++) {
			s[j] = town->talking.talk_nodes[i].link1[j];
			s[j + 6] = town->talking.talk_nodes[i].link2[j];
		}
		std::ostringstream strb;
		strb << "Node " << i << " - Per. " << town->talking.talk_nodes[i].personality << ", " << s;
		set_rb(10 + i,RB_DIALOGUE, i, strb.str());
	}
	if(draw_full)
		redraw_screen();
	else for(i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i,0);
	set_lb(NLS - 3,LB_CLEAR,LB_NO_ACTION,"",true);
}

bool save_check(std::string which_dlog) {
	std::string choice;
	
	if(!change_made)
		return true;
	change_made = false;
	choice = cChoiceDlog(which_dlog, {"save","revert","cancel"}).show();
	if(choice == "revert")
		return true;
	else if(choice == "cancel")
		return false;
	town->set_up_lights();
	save_scenario(scenario.scen_file);
	return true;
}

bool is_lava(short x,short y) {
	if((coord_to_ter(x,y) == 75) || (coord_to_ter(x,y) == 76))
		return true;
	else return false;
}

ter_num_t coord_to_ter(short x,short y) {
	ter_num_t what_terrain;
	
	if(editing_town)
		what_terrain = town->terrain(x,y);
	else what_terrain = current_terrain->terrain[x][y];
	
	return what_terrain;
}

bool monst_on_space(location loc,short m_num) {
	if(!editing_town)
		return false;
	if(m_num >= town->creatures.size())
		return false;
	if(town->creatures[m_num].number == 0)
		return false;
	if((loc.x - town->creatures[m_num].start_loc.x >= 0) &&
		(loc.x - town->creatures[m_num].start_loc.x <= scenario.scen_monsters[town->creatures[m_num].number].x_width - 1) &&
		(loc.y - town->creatures[m_num].start_loc.y >= 0) &&
		(loc.y - town->creatures[m_num].start_loc.y <= scenario.scen_monsters[town->creatures[m_num].number].y_width - 1))
		return true;
	return false;
	
}
