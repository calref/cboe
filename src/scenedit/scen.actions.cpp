
#include <cmath>
#include <cstdio>
#include <cstring>
#include <array>
#include <string>
#include <stack>
#include "scen.global.hpp"
#include "scenario.hpp"
#include "render_shapes.hpp"
#include "scen.graphics.hpp"
#include "scen.actions.hpp"
#include "sounds.hpp"
#include "scen.core.hpp"
#include "scen.fileio.hpp"
#include "scen.keydlgs.hpp"
#include "scen.townout.hpp"
#include "scen.menus.hpp"
#include "mathutil.hpp"
#include "fileio.hpp"
#include "winutil.hpp"
#include "cursors.hpp"
#include "scrollbar.hpp"
#include "strdlog.hpp"
#include "choicedlog.hpp"
#include "gitrev.hpp"

#include "scen.btnmg.hpp"

extern std::string current_string[2];
extern short mini_map_scales[3];
extern rectangle terrain_rect;
extern eDrawMode draw_mode;
// border rects order: top, left, bottom, right //
rectangle border_rect[4];
short current_block_edited = 0;
short current_terrain_type = 0;
short safety = 0;
location spot_hit,last_spot_hit(-1,-1),mouse_spot(-1,-1);
short copied_spec = -1;
cUndoList undo_list;

cTown::cItem store_place_item;

short flood_count = 0;

rectangle terrain_rects[256],terrain_rect_base = {0,0,16,16},command_rects[21];
extern rectangle terrain_buttons_rect;

extern short cen_x, cen_y, cur_town;
extern eScenMode overall_mode;
extern bool mouse_button_held,editing_town;
extern short cur_viewing_mode;
extern cTown* town;
extern short mode_count,to_create;
extern ter_num_t template_terrain[64][64];
extern cScenario scenario;
extern std::shared_ptr<cScrollbar> right_sbar, pal_sbar;
extern cOutdoors* current_terrain;
extern location cur_out;
extern sf::RenderWindow mainPtr;
bool small_any_drawn = false;
extern bool change_made;

rectangle left_buttons[NLS][2]; // 0 - whole, 1 - blue button
std::array<lb_t,NLS> left_button_status;
std::vector<rb_t> right_button_status;
rectangle right_buttons[NRSONPAGE];
rectangle palette_buttons[10][6];
short current_rs_top = 0;

ePalBtn out_buttons[6][10] = {
	{PAL_PENCIL, PAL_BRUSH_LG, PAL_BRUSH_SM, PAL_SPRAY_LG, PAL_SPRAY_SM, PAL_ERASER, PAL_DROPPER, PAL_RECT_HOLLOW, PAL_RECT_FILLED, PAL_BUCKET},
	{PAL_EDIT_TOWN, PAL_ERASE_TOWN, PAL_BLANK, PAL_BLANK, PAL_EDIT_SIGN, PAL_TEXT_AREA, PAL_WANDER, PAL_CHANGE, PAL_ZOOM, PAL_BLANK},
	{PAL_SPEC, PAL_COPY_SPEC, PAL_PASTE_SPEC, PAL_ERASE_SPEC, PAL_EDIT_SPEC, PAL_SPEC_SPOT, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
	{PAL_BOAT, PAL_HORSE, PAL_ROAD, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
	{PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
	{PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
};

ePalBtn town_buttons[6][10] = {
	{PAL_PENCIL, PAL_BRUSH_LG, PAL_BRUSH_SM, PAL_SPRAY_LG, PAL_SPRAY_SM, PAL_ERASER, PAL_DROPPER, PAL_RECT_HOLLOW, PAL_RECT_FILLED, PAL_BUCKET},
	{PAL_ENTER_N, PAL_ENTER_W, PAL_ENTER_S, PAL_ENTER_E, PAL_EDIT_SIGN, PAL_TEXT_AREA, PAL_WANDER, PAL_CHANGE, PAL_ZOOM, PAL_TERRAIN},
	{PAL_SPEC, PAL_COPY_SPEC, PAL_PASTE_SPEC, PAL_ERASE_SPEC, PAL_EDIT_SPEC, PAL_SPEC_SPOT, PAL_EDIT_ITEM, PAL_SAME_ITEM, PAL_ERASE_ITEM, PAL_ITEM},
	{PAL_BOAT, PAL_HORSE, PAL_ROAD, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_EDIT_MONST, PAL_SAME_MONST, PAL_ERASE_MONST, PAL_MONST},
	{PAL_WEB, PAL_CRATE, PAL_BARREL, PAL_BLOCK, PAL_FIRE_BARR, PAL_FORCE_BARR, PAL_QUICKFIRE, PAL_FORCECAGE, PAL_ERASE_FIELD, PAL_BLANK},
	{PAL_SFX_SB, PAL_SFX_MB, PAL_SFX_LB, PAL_SFX_SS, PAL_SFX_LS, PAL_SFX_ASH, PAL_SFX_BONE, PAL_SFX_ROCK, PAL_BLANK, PAL_BLANK},
};

cTownperson last_placed_monst;

rectangle working_rect;
location last_space_hit;
bool erasing_mode;
ter_num_t current_ground = 0;

short special_to_paste = -1;

bool monst_on_space(location loc,short m_num);
static bool terrain_matches(unsigned char x, unsigned char y, ter_num_t ter);

void init_screen_locs() {
	for(short i = 0; i < 4; i++)
		border_rect[i] = terrain_rect;
	border_rect[0].bottom = border_rect[0].top + 8;
	border_rect[1].right = border_rect[1].left + 8;
	border_rect[2].top = border_rect[2].bottom - 8;
	border_rect[3].left = border_rect[3].right - 8;
	
	for(short i = 0; i < 256; i++) {
		terrain_rects[i] = terrain_rect_base;
		terrain_rects[i].offset(3 + (i % 16) * (terrain_rect_base.right + 1),
			3 + (i / 16) * (terrain_rect_base.bottom + 1));
	}
}

static cursor_type get_edit_cursor() {
	switch(overall_mode) {
		case MODE_INTRO_SCREEN: case MODE_MAIN_SCREEN: case MODE_EDIT_TYPES:
			
		case MODE_PLACE_CREATURE: case MODE_PLACE_ITEM: case MODE_PLACE_SPECIAL:
			
		case MODE_PLACE_WEB: case MODE_PLACE_CRATE: case MODE_PLACE_BARREL:
		case MODE_PLACE_STONE_BLOCK: case MODE_PLACE_FIRE_BARRIER:
		case MODE_PLACE_FORCE_BARRIER: case MODE_PLACE_QUICKFIRE:
		case MODE_PLACE_FORCECAGE: case MODE_PLACE_SFX:
		case MODE_PLACE_HORSE: case MODE_PLACE_BOAT:
		case MODE_TOGGLE_SPECIAL_DOT: case MODE_TOGGLE_ROAD:
			
		case MODE_DRAWING:
			return wand_curs;
			
		case MODE_LARGE_PAINTBRUSH: case MODE_SMALL_PAINTBRUSH:
			return brush_curs;
			
		case MODE_LARGE_SPRAYCAN: case MODE_SMALL_SPRAYCAN:
			return spray_curs;
			
		case MODE_FLOOD_FILL:
			return bucket_curs;
			
		case MODE_EYEDROPPER:
			return eyedropper_curs;
			
		case MODE_ROOM_RECT: case MODE_SET_TOWN_RECT:
		case MODE_HOLLOW_RECT: case MODE_FILLED_RECT:
			return mode_count == 2 ? topleft_curs : bottomright_curs;
			
		case MODE_ERASE_CREATURE: case MODE_ERASE_ITEM:
		case MODE_ERASE_SPECIAL: case MODE_ERASE_TOWN_ENTRANCE:
			
		case MODE_ERASER: case MODE_CLEAR_FIELDS:
			return eraser_curs;
			
		case MODE_EDIT_CREATURE: case MODE_EDIT_ITEM:
		case MODE_EDIT_SPECIAL: case MODE_EDIT_TOWN_ENTRANCE:
		case MODE_EDIT_SIGN:
			
		case MODE_PLACE_SAME_CREATURE: case MODE_PLACE_SAME_ITEM:
		case MODE_COPY_SPECIAL: case MODE_PASTE_SPECIAL:
			
		case MODE_PLACE_EAST_ENTRANCE: case MODE_PLACE_NORTH_ENTRANCE:
		case MODE_PLACE_SOUTH_ENTRANCE: case MODE_PLACE_WEST_ENTRANCE:
			
		case MODE_SET_OUT_START: case MODE_SET_TOWN_START:
			
		case MODE_SET_WANDER_POINTS:
			return hand_curs;
	}
	return wand_curs;
}

void update_mouse_spot(location the_point) {
	rectangle terrain_rect = ::terrain_rect;
	terrain_rect.inset(8,8);
	terrain_rect.right -= 4;
	if(overall_mode >= MODE_MAIN_SCREEN)
		set_cursor(sword_curs);
	else if(terrain_rect.contains(the_point)) {
		set_cursor(get_edit_cursor());
		if(cur_viewing_mode == 0) {
			mouse_spot.x = (the_point.x - TER_RECT_UL_X - 8) / 28;
			mouse_spot.y = (the_point.y - TER_RECT_UL_Y - 8) / 36;
		} else {
			short scale = mini_map_scales[cur_viewing_mode - 1];
			mouse_spot.x = (the_point.x - TER_RECT_UL_X - 8) / scale;
			mouse_spot.y = (the_point.y - TER_RECT_UL_Y - 8) / scale;
		}
	} else {
		mouse_spot = {-1,-1};
		the_point.x -= RIGHT_AREA_UL_X;
		the_point.y -= RIGHT_AREA_UL_Y;
		rectangle terpal_rect = terrain_rects[0];
		terpal_rect.right = terrain_rects[255].right;
		terpal_rect.bottom = terrain_rects[255].bottom;
		if(terpal_rect.contains(the_point))
			set_cursor(eyedropper_curs);
		else set_cursor(wand_curs);
	}
	if(overall_mode < MODE_MAIN_SCREEN) place_location();
}

static bool handle_lb_action(location the_point) {
	fs::path file_to_load;
	int x;
	for(int i = 0; i < NLS; i++)
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
							set_current_town(scenario.last_town_edited);
							cur_out = scenario.last_out_edited;
							current_terrain = scenario.outdoors[cur_out.x][cur_out.y];
							overall_mode = MODE_MAIN_SCREEN;
							set_up_main_screen();
						} else if(!file_to_load.empty())
							// If we tried to load but failed, the scenario record is messed up, so boot to start screen.
							set_up_start_screen();
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
						if(scenario.towns.size() >= 200) {
							showError("You have reached the limit of 200 towns you can have in one scenario.");
							return true;
						}
						if(new_town())
							set_up_main_screen();
						break;
					case LB_EDIT_TEXT:
						right_sbar->setPosition(0);
						start_string_editing(STRS_SCEN,0);
						break;
					case LB_EDIT_SPECITEM:
						start_special_item_editing(false);
						break;
					case LB_EDIT_QUEST:
						start_quest_editing(false);
						break;
					case LB_EDIT_SHOPS:
						start_shops_editing(false);
						break;
					case LB_LOAD_OUT:
						spot_hit = pick_out(cur_out, scenario);
						if(spot_hit != cur_out) {
							cur_out = spot_hit;
							current_terrain = scenario.outdoors[cur_out.x][cur_out.y];
							set_up_main_screen();
						}
						break;
					case LB_EDIT_OUT:
						start_out_edit();
						mouse_button_held = false;
						break;
					case LB_LOAD_TOWN:
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
						break;
					case LB_EDIT_TALK:
						start_dialogue_editing(0);
						break;
						
				}
			}
			if((overall_mode < MODE_MAIN_SCREEN) && left_button_status[i].action == LB_RETURN) {
				set_up_main_screen();
			}
			mouse_button_held = false;
			update_edit_menu();
			return true;
		}
	return false;
}

static bool handle_rb_action(location the_point, bool option_hit) {
	long right_top = right_sbar->getPosition();
	for(int i = 0; i < NRSONPAGE && i + right_top < NRS; i++)
		if(!mouse_button_held && (the_point.in(right_buttons[i]) )
		   && (right_button_status[i + right_top].action != RB_CLEAR))  {
			
			int j = right_button_status[i + right_top].i;
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
				case RB_MONST:
					size_before = scenario.scen_monsters.size();
					if(option_hit) {
						if(j == size_before - 1 && !scenario.is_monst_used(j))
							scenario.scen_monsters.pop_back();
						else if(j == size_before) {
							scenario.scen_monsters.resize(size_before + 8);
							for(; j < scenario.scen_monsters.size(); j++)
								scenario.scen_monsters[j].m_name = "New Monster";
						} else {
							scenario.scen_monsters[j] = cMonster();
							scenario.scen_monsters[j].m_name = "Unused Monster";
						}
					} else {
						if(j == size_before) {
							scenario.scen_monsters.emplace_back();
							scenario.scen_monsters.back().m_name = "New Monster";
						}
						if(!edit_monst_type(j) && j == size_before && scenario.scen_monsters.back().m_name == "New Monster")
							scenario.scen_monsters.pop_back();
					}
					start_monster_editing(size_before == scenario.scen_monsters.size());
					if(size_before > scenario.scen_monsters.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_ITEM:
					size_before = scenario.scen_items.size();
					if(option_hit) {
						if(j == size_before - 1 && !scenario.is_item_used(j))
							scenario.scen_items.pop_back();
						else if(j == size_before) {
							scenario.scen_items.resize(size_before + 8);
							for(; j < scenario.scen_items.size(); j++) {
								scenario.scen_items[j].full_name = "New Item";
								scenario.scen_items[j].name = "Item";
							}
						} else {
							scenario.scen_items[j] = cItem();
							scenario.scen_items[j].full_name = "Unused Item";
							scenario.scen_items[j].name = "Item";
						}
					} else {
						if(j == size_before) {
							scenario.scen_items.emplace_back();
							scenario.scen_items.back().full_name = "New Item";
							scenario.scen_items.back().name = "Item";
						}
						if(!edit_item_type(j) && j == size_before && scenario.scen_items.back().name == "New Item")
							scenario.scen_items.pop_back();
					}
					start_item_editing(size_before == scenario.scen_items.size());
					if(size_before > scenario.scen_items.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
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
						edit_spec_enc(j,0,nullptr);
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
						edit_spec_enc(j,1,nullptr);
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
						edit_spec_enc(j,2,nullptr);
					}
					start_special_editing(2,size_before == town->specials.size());
					if(size_before > town->specials.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_SCEN_STR:
					size_before = scenario.spec_strs.size();
					if(option_hit) {
						if(j == size_before - 1)
							scenario.spec_strs.pop_back();
						else if(j == size_before)
							scenario.spec_strs.resize(size_before + 8, "*");
						else scenario.spec_strs[j] = "*";
					} else {
						if(j == size_before)
							scenario.spec_strs.emplace_back("*");
						if(!edit_text_str(j,STRS_SCEN) && j == size_before && scenario.spec_strs[j] == "*")
							scenario.spec_strs.pop_back();
					}
					start_string_editing(STRS_SCEN,size_before == scenario.spec_strs.size());
					if(size_before > scenario.spec_strs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_OUT_STR:
					size_before = current_terrain->spec_strs.size();
					if(option_hit) {
						if(j == size_before - 1)
							current_terrain->spec_strs.pop_back();
						else if(j == size_before)
							current_terrain->spec_strs.resize(size_before + 8, "*");
						else current_terrain->spec_strs[j] = "*";
					} else {
						if(j == size_before)
							current_terrain->spec_strs.emplace_back("*");
						if(!edit_text_str(j,STRS_OUT) && j == size_before && current_terrain->spec_strs[j] == "*")
							current_terrain->spec_strs.pop_back();
					}
					start_string_editing(STRS_OUT,size_before == current_terrain->spec_strs.size());
					if(size_before > current_terrain->spec_strs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_TOWN_STR:
					size_before = town->spec_strs.size();
					if(option_hit) {
						if(j == size_before - 1)
							town->spec_strs.pop_back();
						else if(j == size_before)
							town->spec_strs.resize(size_before + 8, "*");
						else town->spec_strs[j] = "*";
					} else {
						if(j == size_before)
							town->spec_strs.emplace_back("*");
						if(!edit_text_str(j,STRS_TOWN) && j == size_before && town->spec_strs[j] == "*")
							town->spec_strs.pop_back();
					}
					start_string_editing(STRS_TOWN,size_before == town->spec_strs.size());
					if(size_before > town->spec_strs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_SPEC_ITEM:
					size_before = scenario.special_items.size();
					if(option_hit) {
						if(j == size_before - 1)
							scenario.special_items.pop_back();
						else if(j == size_before)
							break;
						else {
							scenario.special_items[j] = cSpecItem();
							scenario.special_items[j].name = "Unused Special Item";
						}
					} else {
						if(j == size_before) {
							scenario.special_items.emplace_back();
							scenario.special_items.back().name = "New Special Item";
						}
						if(!edit_spec_item(j) && j == size_before)
							scenario.special_items.pop_back();
					}
					start_special_item_editing(size_before == scenario.special_items.size());
					if(size_before > scenario.special_items.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_JOURNAL:
					size_before = scenario.journal_strs.size();
					if(option_hit) {
						if(j == size_before - 1)
							scenario.journal_strs.pop_back();
						else if(j == size_before)
							scenario.journal_strs.resize(size_before + 8, "*");
						else scenario.journal_strs[j] = "*";
					} else {
						if(j == size_before)
							scenario.journal_strs.emplace_back("*");
						if(!edit_text_str(j,STRS_JOURNAL) && j == size_before && scenario.journal_strs[j] == "*")
							scenario.journal_strs.pop_back();
					}
					start_string_editing(STRS_JOURNAL,size_before == scenario.journal_strs.size());
					if(size_before > scenario.journal_strs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_DIALOGUE:
					size_before = town->talking.talk_nodes.size();
					if(option_hit) {
						if(j == size_before)
							break;
						town->talking.talk_nodes.erase(town->talking.talk_nodes.begin() + j);
					} else {
						if(j == size_before)
							town->talking.talk_nodes.emplace_back();
						if((j = edit_talk_node(j)) >= 0 && town->talking.talk_nodes[j].personality == -1)
							town->talking.talk_nodes.erase(town->talking.talk_nodes.begin() + j);
					}
					start_dialogue_editing(size_before == town->talking.talk_nodes.size());
					if(size_before > town->talking.talk_nodes.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_PERSONALITY:
					edit_basic_dlog(j);
					start_dialogue_editing(1);
					break;
				case RB_OUT_SIGN:
					size_before = current_terrain->sign_locs.size();
					if(option_hit) {
						if(j == size_before - 1)
							current_terrain->sign_locs.pop_back();
						else if(j == size_before)
							break;
						else current_terrain->sign_locs[j] = {-1, -1, "*"};
					} else {
						if(j == size_before)
							current_terrain->sign_locs.emplace_back(-1,-1,"*");
						if(!edit_text_str(j,STRS_OUT_SIGN) && j == size_before && current_terrain->sign_locs[j].text == "*")
							current_terrain->sign_locs.pop_back();
					}
					start_string_editing(STRS_OUT_SIGN,size_before == current_terrain->sign_locs.size());
					if(size_before > current_terrain->sign_locs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_TOWN_SIGN:
					size_before = town->sign_locs.size();
					if(option_hit) {
						if(j == size_before - 1)
							town->sign_locs.pop_back();
						else if(j == size_before)
							break;
						else town->sign_locs[j] = {-1, -1, "*"};
					} else {
						if(j == size_before)
							town->sign_locs.emplace_back(-1,-1,"*");
						if(!edit_text_str(j,STRS_TOWN_SIGN) && j == size_before && town->sign_locs[j].text == "*")
							town->sign_locs.pop_back();
					}
					start_string_editing(STRS_TOWN_SIGN,size_before == town->sign_locs.size());
					if(size_before > town->sign_locs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_OUT_RECT:
					size_before = current_terrain->area_desc.size();
					if(option_hit) {
						if(j == size_before - 1)
							current_terrain->area_desc.pop_back();
						else if(j == size_before)
							break;
						else current_terrain->area_desc[j] = {0, 0, 0, 0, "*"};
					} else {
						if(j == size_before)
							current_terrain->area_desc.emplace_back(0,0,0,0,"*");
						if(!edit_text_str(j,STRS_OUT_RECT) && j == size_before && current_terrain->area_desc[j].descr == "*")
							current_terrain->area_desc.pop_back();
					}
					start_string_editing(STRS_OUT_RECT,size_before == current_terrain->area_desc.size());
					if(size_before > current_terrain->area_desc.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_TOWN_RECT:
					size_before = town->area_desc.size();
					if(option_hit) {
						if(j == size_before - 1)
							town->area_desc.pop_back();
						else if(j == size_before)
							break;
						else town->area_desc[j] = {0, 0, 0, 0, "*"};
					} else {
						if(j == size_before)
							town->area_desc.emplace_back(0,0,0,0,"*");
						if(!edit_text_str(j,STRS_TOWN_RECT) && j == size_before && town->area_desc[j].descr == "*")
							town->area_desc.pop_back();
					}
					start_string_editing(STRS_TOWN_RECT,size_before == town->area_desc.size());
					if(size_before > town->area_desc.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_QUEST:
					size_before = scenario.quests.size();
					if(option_hit) {
						if(j == scenario.quests.size() - 1)
							scenario.quests.pop_back();
						else {
							scenario.quests[j] = cQuest();
							scenario.quests[j].name = "Unused Quest";
						}
					} else {
						if(!edit_quest(j) && j == size_before && scenario.quests[j].name == "New Quest")
							scenario.quests.pop_back();
					}
					start_quest_editing(size_before == scenario.quests.size());
					if(size_before > scenario.quests.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_SHOP:
					size_before = scenario.shops.size();
					if(option_hit) {
						if(j == scenario.shops.size() - 1)
							scenario.shops.pop_back();
						else scenario.shops[j] = cShop("Unused Shop");
					} else {
						if(!edit_shop(j) && j == size_before && scenario.shops[j].getName() == "New Shop")
							scenario.shops.pop_back();
					}
					start_shops_editing(size_before == scenario.shops.size());
					if(size_before > scenario.shops.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
			}
			change_made = true;
			mouse_button_held = false;
			return true;
		}
	return false;
}

static bool handle_terrain_action(location the_point, bool ctrl_hit) {
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
			return true;
		else last_spot_hit = spot_hit;
		if(!mouse_button_held)
			last_spot_hit = spot_hit;
		
		eScenMode old_mode = overall_mode;
		change_made = true;
		
		if((spot_hit.x < 0) || (spot_hit.x > ((editing_town) ? town->max_dim - 1 : 47)) ||
		   (spot_hit.y < 0) || (spot_hit.y > ((editing_town) ? town->max_dim - 1 : 47))) ;
		else switch(overall_mode) {
			case MODE_DRAWING:
				if((!mouse_button_held && terrain_matches(spot_hit.x,spot_hit.y,current_terrain_type)) ||
				   (mouse_button_held && erasing_mode)) {
					set_terrain(spot_hit,current_ground);
					erasing_mode = true;
					mouse_button_held = true;
				}
				else {
					mouse_button_held = true;
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
					auto& area_descs = editing_town ? town->area_desc : current_terrain->area_desc;
					auto iter = std::find_if(area_descs.begin(), area_descs.end(), [](const info_rect_t& r) {
						return r.right == 0;
					});
					if(iter != area_descs.end()) {
						static_cast<rectangle&>(*iter) = working_rect;
						iter->descr = "";
						if(!edit_area_rect_str(*iter))
							iter->right = 0;
					} else {
						area_descs.emplace_back(working_rect);
						if(!edit_area_rect_str(area_descs.back()))
							area_descs.pop_back();
					}
					change_made = true;
				}
				overall_mode = MODE_DRAWING;
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
						set_string("Drawing mode",scenario.ter_types[current_terrain_type].name);
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
				change_circle_terrain(spot_hit,4,current_terrain_type,1);
				break;
			case MODE_SMALL_SPRAYCAN:
				mouse_button_held = true;
				change_circle_terrain(spot_hit,2,current_terrain_type,1);
				break;
			case MODE_ERASER: // erase
				change_circle_terrain(spot_hit,2,current_ground,20);
				mouse_button_held = true;
				break;
			case MODE_FLOOD_FILL:
				if(mouse_button_held) break;
				flood_fill_terrain(spot_hit, current_terrain_type);
				mouse_button_held = true;
				break;
			case MODE_PLACE_ITEM:
				// If we just placed this item there, forget it
				if(!mouse_button_held || store_place_item.loc != spot_hit) {
					mouse_button_held = true;
					auto iter = std::find_if(town->preset_items.begin(), town->preset_items.end(), [](const cTown::cItem& item) {
						return item.code < 0;
					});
					if(iter != town->preset_items.end()) {
						*iter = {spot_hit, mode_count, scenario.scen_items[mode_count]};
						if(container_there(spot_hit)) iter->contained = true;
						store_place_item = *iter;
					} else {
						town->preset_items.push_back({spot_hit, mode_count, scenario.scen_items[mode_count]});
						if(container_there(spot_hit)) town->preset_items.back().contained = true;
						store_place_item = town->preset_items.back();
					}
				}
				break;
			case MODE_EDIT_ITEM:
				for(short x = 0; x < town->preset_items.size(); x++)
					if((spot_hit.x == town->preset_items[x].loc.x) &&
					   (spot_hit.y == town->preset_items[x].loc.y) && (town->preset_items[x].code >= 0)) {
						edit_placed_item(x);
						store_place_item = town->preset_items[x];
					}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_SAME_CREATURE:
				if(last_placed_monst.number == 0) {
					showError("Either no monster has been placed, or the last time you tried to place a monster the operation failed.");
				} else {
					auto iter = std::find_if(town->creatures.begin(), town->creatures.end(), [](const cTownperson& who) {
						return who.number == 0;
					});
					if(iter != town->creatures.end()) {
						*iter = last_placed_monst;
						iter->start_loc = spot_hit;
					} else { // Placement failed
						town->creatures.push_back(last_placed_monst);
						town->creatures.back().start_loc = spot_hit;
					}
				}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_CREATURE:
				// If we just placed this same creature here, forget it
				if(!mouse_button_held || last_placed_monst.start_loc != spot_hit) {
					mouse_button_held = true;
					auto iter = std::find_if(town->creatures.begin(), town->creatures.end(), [](const cTownperson& who) {
						return who.number == 0;
					});
					if(iter != town->creatures.end()) {
						*iter = {spot_hit, static_cast<mon_num_t>(mode_count), scenario.scen_monsters[mode_count]};
						last_placed_monst = *iter;
					} else { // Placement failed
						town->creatures.push_back({spot_hit, static_cast<mon_num_t>(mode_count), scenario.scen_monsters[mode_count]});
						last_placed_monst = town->creatures.back();
					}
				}
				break;
				
			case MODE_PLACE_NORTH_ENTRANCE: case MODE_PLACE_EAST_ENTRANCE:
			case MODE_PLACE_SOUTH_ENTRANCE: case MODE_PLACE_WEST_ENTRANCE:
				town->start_locs[overall_mode - 10].x = spot_hit.x;
				town->start_locs[overall_mode - 10].y = spot_hit.y;
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_WEB:
				make_field_type(spot_hit.x,spot_hit.y,FIELD_WEB);
				mouse_button_held = true;
				break;
			case MODE_PLACE_CRATE:
				make_field_type(spot_hit.x,spot_hit.y,OBJECT_CRATE);
				mouse_button_held = true;
				break;
			case MODE_PLACE_BARREL:
				make_field_type(spot_hit.x,spot_hit.y,OBJECT_BARREL);
				mouse_button_held = true;
				break;
			case MODE_PLACE_FIRE_BARRIER:
				make_field_type(spot_hit.x,spot_hit.y,BARRIER_FIRE);
				mouse_button_held = true;
				break;
			case MODE_PLACE_FORCE_BARRIER:
				make_field_type(spot_hit.x,spot_hit.y,BARRIER_FORCE);
				mouse_button_held = true;
				break;
			case MODE_PLACE_QUICKFIRE:
				make_field_type(spot_hit.x,spot_hit.y,FIELD_QUICKFIRE);
				mouse_button_held = true;
				break;
			case MODE_PLACE_STONE_BLOCK:
				make_field_type(spot_hit.x,spot_hit.y,OBJECT_BLOCK);
				mouse_button_held = true;
				break;
			case MODE_PLACE_FORCECAGE:
				make_field_type(spot_hit.x,spot_hit.y,BARRIER_CAGE);
				mouse_button_held = true;
				break;
			case MODE_TOGGLE_SPECIAL_DOT:
				if(!editing_town){
					if(!mouse_button_held)
						mode_count = !current_terrain->special_spot[spot_hit.x][spot_hit.y];
					current_terrain->special_spot[spot_hit.x][spot_hit.y] = mode_count;
					mouse_button_held = true;
					break;
				}
				make_field_type(spot_hit.x, spot_hit.y, SPECIAL_SPOT);
				mouse_button_held = true;
				break;
			case MODE_TOGGLE_ROAD:
				if(!editing_town){
					if(!mouse_button_held)
						mode_count = !current_terrain->roads[spot_hit.x][spot_hit.y];
					current_terrain->roads[spot_hit.x][spot_hit.y] = mode_count;
					mouse_button_held = true;
					break;
				}
				make_field_type(spot_hit.x, spot_hit.y, SPECIAL_ROAD);
				mouse_button_held = true;
				break;
			case MODE_CLEAR_FIELDS:
				for(int i = 8; i <= SPECIAL_ROAD; i++)
					take_field_type(spot_hit.x,spot_hit.y, eFieldType(i));
				mouse_button_held = true;
				break;
			case MODE_PLACE_SFX:
				make_field_type(spot_hit.x,spot_hit.y,eFieldType(SFX_SMALL_BLOOD + mode_count));
				mouse_button_held = true;
				break;
			case MODE_EYEDROPPER:
				if(editing_town)
					set_new_terrain(town->terrain(spot_hit.x,spot_hit.y));
				else set_new_terrain(current_terrain->terrain[spot_hit.x][spot_hit.y]);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_SAME_ITEM:
				if(store_place_item.code < 0) {
					showError("Either no item has been placed, or the last time you tried to place an item the operation failed.");
				} else {
					auto iter = std::find_if(town->preset_items.begin(), town->preset_items.end(), [](const cTown::cItem& item) {
						return item.code < 0;
					});
					if(iter != town->preset_items.end()) {
						*iter = store_place_item;
						iter->loc = spot_hit;
						iter->contained = container_there(spot_hit);
					} else {
						town->preset_items.push_back(store_place_item);
						town->preset_items.back().loc = spot_hit;
						town->preset_items.back().contained = container_there(spot_hit);
					}
				}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_EDIT_SIGN: //edit sign
			{
				auto& signs = editing_town ? town->sign_locs : current_terrain->sign_locs;
				auto iter = std::find(signs.begin(), signs.end(), spot_hit);
				short picture = scenario.ter_types[editing_town ? town->terrain(spot_hit.x,spot_hit.y) : current_terrain->terrain[spot_hit.x][spot_hit.y]].picture;
				if(iter != signs.end()) {
					edit_sign(*iter, iter - signs.begin(), picture);
				} else {
					signs.emplace_back(spot_hit);
					edit_sign(signs.back(), signs.size() - 1, picture);
				}
				overall_mode = MODE_DRAWING;
				break;
			}
			case MODE_EDIT_CREATURE: //edit monst
				for(short x = 0; x < town->creatures.size(); x++)
					if(monst_on_space(spot_hit,x)) {
						edit_placed_monst(x);
						last_placed_monst = town->creatures[x];
					}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_EDIT_SPECIAL: //make special
				place_edit_special(spot_hit);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_COPY_SPECIAL: //copy special
			{
				auto& specials = editing_town ? town->special_locs : current_terrain->special_locs;
				auto iter = std::find_if(town->special_locs.begin(), town->special_locs.end(), [](const spec_loc_t& loc) {
					return loc == spot_hit && loc.spec >= 0;
				});
				if(iter != specials.end())
					copied_spec = iter->spec;
				else showError("There wasn't a special on that spot.");
				overall_mode = MODE_DRAWING;
				break;
			}
			case MODE_PASTE_SPECIAL: //paste special
				if(copied_spec < 0) {
					showError("You need to select a special to copy first.");
				} else if(!editing_town && (spot_hit.x == 0 || spot_hit.x == 47 || spot_hit.y == 0 || spot_hit.y == 47)) {
					cChoiceDlog("not-at-edge").show();
					break;
				} else {
					auto& specials = editing_town ? town->special_locs : current_terrain->special_locs;
					for(short x = 0; x <= specials.size(); x++) {
						if(x == specials.size())
							specials.emplace_back(-1,-1,-1);
						if(specials[x].spec < 0) {
							specials[x] = spot_hit;
							specials[x].spec = copied_spec;
							break;
						}
					}
				}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_ERASE_SPECIAL: //erase special
			{
				auto& specials = editing_town ? town->special_locs : current_terrain->special_locs;
				for(short x = 0; x < specials.size(); x++)
					if(specials[x] == spot_hit && specials[x].spec >= 0) {
						specials[x] = {-1,-1};
						specials[x].spec = -1;
						if(x == specials.size() - 1) {
							// Delete not only the last entry but any other empty entries at the end of the list
							do {
								specials.pop_back();
							} while(!specials.empty() && specials.back().spec < 0);
						}
						break;
					}
				overall_mode = MODE_DRAWING;
				break;
			}
			case MODE_PLACE_SPECIAL: //edit special
				set_special(spot_hit);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_EDIT_TOWN_ENTRANCE: //edit town entry
				town_entry(spot_hit);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_ERASE_TOWN_ENTRANCE:
				for(short x = current_terrain->city_locs.size() - 1; x >= 0; x--) {
					if(current_terrain->city_locs[x] == spot_hit)
						current_terrain->city_locs.erase(current_terrain->city_locs.begin() + x);
				}
				overall_mode = MODE_DRAWING;
				change_made = true;
				break;
			case MODE_SET_OUT_START:
				if((spot_hit.x != minmax(4,43,spot_hit.x)) || (spot_hit.y != minmax(4,43,spot_hit.y))) {
					showError("You can't put the starting location this close to the edge of an outdoor section. It has to be at least 4 spaces away.");
					break;
				}
				scenario.out_sec_start.x = cur_out.x;
				scenario.out_sec_start.y = cur_out.y;
				scenario.out_start = spot_hit;
				overall_mode = MODE_DRAWING;
				change_made = true;
				break;
			case MODE_ERASE_CREATURE: //delete monst
				for(short x = 0; x < town->creatures.size(); x++)
					if(monst_on_space(spot_hit,x)) {
						town->creatures[x].number = 0;
						break;
					}
				while(!town->creatures.empty() && town->creatures.back().number == 0)
					town->creatures.pop_back();
				overall_mode = MODE_DRAWING;
				break;
			case MODE_ERASE_ITEM: // delete item
				for(short x = 0; x < town->preset_items.size(); x++)
					if((spot_hit.x == town->preset_items[x].loc.x) &&
					   (spot_hit.y == town->preset_items[x].loc.y) && (town->preset_items[x].code >= 0)) {
						town->preset_items[x].code = -1;
						break;
					}
				while(!town->preset_items.empty() && town->preset_items.back().code == -1)
					town->preset_items.pop_back();
				overall_mode = MODE_DRAWING;
				break;
			case MODE_SET_TOWN_START:
				if(!town->in_town_rect.contains(spot_hit)) {
					showError("You can't put the starting location outside the town boundaries.");
					break;
				}
				scenario.which_town_start = cur_town;
				scenario.where_start = spot_hit;
				overall_mode = MODE_DRAWING;
				change_made = true;
				break;
			case MODE_PLACE_BOAT: case MODE_PLACE_HORSE: {
				auto& all = overall_mode == MODE_PLACE_BOAT ? scenario.boats : scenario.horses;
				auto iter = std::find_if(all.begin(), all.end(), [](const cVehicle& what) {
					if(editing_town && cur_town != what.which_town) return false;
					else if(!editing_town && what.which_town != 200) return false;
					return what.loc == spot_hit;
				});
				if(iter == all.end()) {
					iter = std::find_if(all.begin(), all.end(), [](const cVehicle& what) {
						return what.which_town < 0;
					});
					if(iter == all.end()) {
						all.emplace_back();
						iter = all.end() - 1;
					}
					iter->loc = spot_hit;
					iter->which_town = editing_town ? cur_town : 200;
					iter->property = false;
					iter->exists = false;
					if(!editing_town) iter->sector = cur_out;
				}
				if(!edit_vehicle(*iter, iter - all.begin(), overall_mode == MODE_PLACE_BOAT))
					all.erase(iter);
				overall_mode = MODE_DRAWING;
				break;
			}
			case MODE_INTRO_SCREEN:
			case MODE_EDIT_TYPES:
			case MODE_MAIN_SCREEN:
				break; // Nothing to do here, of course.
		}
		if((overall_mode == MODE_DRAWING) && (old_mode != MODE_DRAWING))
			set_string("Drawing mode",scenario.ter_types[current_terrain_type].name);
		draw_terrain();
		return true;
	}
	bool need_redraw = false;
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
	if((the_point.in(border_rect[2])) & (cen_y < (editing_town ? town->max_dim - 5 : 44))) {
		cen_y++;
		if(ctrl_hit)
			cen_y = (editing_town) ? town->max_dim - 5 : 44;
		need_redraw = true;
		mouse_button_held = true;
	}
	if((the_point.in(border_rect[3])) & (cen_x < (editing_town ? town->max_dim - 5 : 44))) {
		cen_x++;
		if(ctrl_hit)
			cen_x = (editing_town) ? town->max_dim - 5 : 44;
		need_redraw = true;
		mouse_button_held = true;
	}
	if(need_redraw) {
		draw_terrain();
		place_location();
		return true;
	}
	return false;
}

static bool handle_terpal_action(location cur_point, bool option_hit) {
	for(int i = 0; i < 256; i++)
		if(cur_point.in(terrain_rects[i])) {
			int k = pal_sbar->getPosition() * 16 + i;
			rectangle temp_rect = terrain_rects[i];
			temp_rect.offset(RIGHT_AREA_UL_X, RIGHT_AREA_UL_Y );
			flash_rect(temp_rect);
			if(overall_mode < MODE_MAIN_SCREEN) {
				switch(draw_mode) {
					case DRAW_TERRAIN:
						set_new_terrain(k);
						break;
					case DRAW_ITEM:
						if(k >= scenario.scen_items.size())
							break;
						if(scenario.scen_items[k].variety == eItemType::NO_ITEM) {
							showError("This item has its Variety set to No Item. You can only place items with a Variety set to an actual item type.");
							break;
						}
						overall_mode = MODE_PLACE_ITEM;
						mode_count = k;
						set_string("Place the item:",scenario.scen_items[mode_count].full_name);
						break;
					case DRAW_MONST:
						if(k + 1 >= scenario.scen_monsters.size())
							break;
						overall_mode = MODE_PLACE_CREATURE;
						mode_count = k + 1;
						set_string("Place the monster:",scenario.scen_monsters[mode_count].m_name);
						break;
				}
			}
			else {
				short size_before = scenario.ter_types.size(), pos_before = pal_sbar->getPosition();
				i += pos_before * 16;
				if(i > size_before) return true;
				if(option_hit) {
					if(i == size_before - 1 && !scenario.is_ter_used(i))
						scenario.ter_types.pop_back();
					else if(i == size_before) {
						scenario.ter_types.resize(size_before + 16);
						for(; i < scenario.ter_types.size(); i++)
							scenario.ter_types[i].name = "New Terrain";
					} else {
						scenario.ter_types[i] = cTerrain();
						scenario.ter_types[i].name = "Unused Terrain";
					}
				} else {
					if(i == size_before) {
						scenario.ter_types.emplace_back();
						scenario.ter_types.back().name = "New Terrain";
					}
					if(!edit_ter_type(i) && i == size_before && scenario.ter_types.back().name == "New Terrain")
						scenario.ter_types.pop_back();
				}
				if(size_before / 16 > scenario.ter_types.size() / 16)
					pos_before--;
				pal_sbar->setPosition(pos_before);
				set_up_terrain_buttons(false);
				change_made = true;
			}
			place_location();
			return true;
		}
	return false;
}

static bool handle_toolpal_action(location cur_point2) {
	for(int i = 0; i < 10; i++)
		for(int j = 0; j < 6; j++) {
			auto cur_palette_buttons = editing_town ? town_buttons : out_buttons;
			if(cur_palette_buttons[j][i] != PAL_BLANK && !mouse_button_held && cur_point2.in(palette_buttons[i][j])
			   && /*((j < 3) || (editing_town)) &&*/ (overall_mode < MODE_MAIN_SCREEN)) {
				rectangle temp_rect = palette_buttons[i][j];
				temp_rect.offset(RIGHT_AREA_UL_X + 5, RIGHT_AREA_UL_Y + terrain_rects[255].bottom + 5);
				flash_rect(temp_rect);
				switch(cur_palette_buttons[j][i]) {
					case PAL_ARROW_UP: case PAL_ARROW_DOWN: // These two might never be used.
					case PAL_BLANK: break;
					case PAL_PENCIL:
						set_string("Drawing mode",scenario.ter_types[current_terrain_type].name);
						overall_mode = MODE_DRAWING;
						break;
					case PAL_BRUSH_LG:
						set_string("Paintbrush (large)",scenario.ter_types[current_terrain_type].name);
						overall_mode = MODE_LARGE_PAINTBRUSH;
						break;
					case PAL_BRUSH_SM:
						set_string("Paintbrush (small)",scenario.ter_types[current_terrain_type].name);
						overall_mode = MODE_SMALL_PAINTBRUSH;
						break;
					case PAL_SPRAY_LG:
						set_string("Spraycan (large)",scenario.ter_types[current_terrain_type].name);
						overall_mode = MODE_LARGE_SPRAYCAN;
						break;
					case PAL_SPRAY_SM:
						set_string("Spraycan (small)",scenario.ter_types[current_terrain_type].name);
						overall_mode = MODE_SMALL_SPRAYCAN;
						break;
					case PAL_DROPPER:
						set_string("Eyedropper","Select terrain to draw");
						overall_mode = MODE_EYEDROPPER;
						break;
					case PAL_RECT_HOLLOW:
						overall_mode = MODE_HOLLOW_RECT;
						if(false) // Skip next statement
					case PAL_RECT_FILLED:
							overall_mode = MODE_FILLED_RECT;
						mode_count = 2;
						if(i == 6)
							set_string("Fill rectangle (hollow)","Select upper left corner");
						else set_string("Fill rectangle (solid)","Select upper left corner");
						break;
					case PAL_BUCKET:
						overall_mode = MODE_FLOOD_FILL;
						set_string("Flood fill", scenario.ter_types[current_terrain_type].name);
						break;
					case PAL_ZOOM: // switch view
						cur_viewing_mode = (cur_viewing_mode + 1) % 4;
						draw_main_screen();
						draw_terrain();
						break;
					case PAL_ERASER:
						set_string("Erase space","Select space to clear");
						overall_mode = MODE_ERASER;
						break;
					case PAL_EDIT_SIGN:
						set_string("Edit sign","Select sign to edit");
						overall_mode = MODE_EDIT_SIGN;
						break;
					case PAL_TEXT_AREA:
						overall_mode = MODE_ROOM_RECT;
						mode_count = 2;
						set_string("Create room rectangle","Select upper left corner");
						break;
					case PAL_WANDER:
						overall_mode = MODE_SET_WANDER_POINTS;
						mode_count = 4;
						set_string("Place first wandering monster arrival point","");
						break;
					case PAL_CHANGE: // replace terrain
						swap_terrain();
						draw_main_screen();
						draw_terrain();
						mouse_button_held = false;
						break;
					case PAL_EDIT_TOWN:
						if(editing_town) {
							set_string("Can only set town entrances outdoors","");
							break;
						}
						set_string("Set town entrance","Select town to edit");
						overall_mode = MODE_EDIT_TOWN_ENTRANCE;
						break;
					case PAL_ERASE_TOWN:
						if(editing_town) {
							set_string("Can only erase town entrances outdoors","");
							break;
						}
						set_string("Erase town entrance","Select town to erase");
						overall_mode = MODE_ERASE_TOWN_ENTRANCE;
						break;
					case PAL_EDIT_ITEM:
						if(!editing_town) {
							set_string("Edit placed item","Not while outdoors.");
							break;
						}
						set_string("Edit placed item","Select item to edit");
						overall_mode = MODE_EDIT_ITEM;
						break;
					case PAL_SAME_ITEM:
						if(!editing_town) {
							set_string("Edit placed item","Not while outdoors.");
							break;
						}
						set_string("Place same item","Select location");
						overall_mode = MODE_PLACE_SAME_ITEM;
						break;
					case PAL_ERASE_ITEM:
						set_string("Delete an item","Select item");
						overall_mode = MODE_ERASE_ITEM;
						break;
					case PAL_SPEC:
						set_string("Create/Edit special","Select special location");
						overall_mode = MODE_EDIT_SPECIAL;
						break;
					case PAL_COPY_SPEC:
						set_string("Copy special","Select special to copy");
						overall_mode = MODE_COPY_SPECIAL;
						break;
					case PAL_PASTE_SPEC:
						if(special_to_paste < 0) {
							set_string("Can't paste special","No special to paste");
						}
						set_string("Paste special","Select location to paste");
						overall_mode = MODE_PASTE_SPECIAL;
						break;
					case PAL_ERASE_SPEC:
						set_string("Erase special","Select special to erase");
						overall_mode = MODE_ERASE_SPECIAL;
						break;
					case PAL_EDIT_SPEC:
						set_string("Set/place special","Select special location");
						overall_mode = MODE_PLACE_SPECIAL;
						break;
					case PAL_EDIT_MONST:
						set_string("Edit creature","Select creature to edit");
						overall_mode = MODE_EDIT_CREATURE;
						break;
					case PAL_SAME_MONST:
						set_string("Place same creature","Select creature location");
						overall_mode = MODE_PLACE_SAME_CREATURE;
						break;
					case PAL_ERASE_MONST:
						set_string("Delete a creature","Select creature");
						overall_mode = MODE_ERASE_CREATURE;
						break;
					case PAL_ENTER_N:
						set_string("Place north entrace","Select entrance location");
						overall_mode = MODE_PLACE_NORTH_ENTRANCE;
						break;
					case PAL_ENTER_W:
						set_string("Place west entrace","Select entrance location");
						overall_mode = MODE_PLACE_WEST_ENTRANCE;
						break;
					case PAL_ENTER_S:
						set_string("Place south entrace","Select entrance location");
						overall_mode = MODE_PLACE_SOUTH_ENTRANCE;
						break;
					case PAL_ENTER_E:
						set_string("Place east entrace","Select entrance location");
						overall_mode = MODE_PLACE_EAST_ENTRANCE;
						break;
					case PAL_WEB:
						set_string("Place web","Select location");
						overall_mode = MODE_PLACE_WEB;
						break;
					case PAL_CRATE:
						set_string("Place crate","Select location");
						overall_mode = MODE_PLACE_CRATE;
						break;
					case PAL_BARREL:
						set_string("Place barrel","Select location");
						overall_mode = MODE_PLACE_BARREL;
						break;
					case PAL_BLOCK:
						set_string("Place stone block","Select location");
						overall_mode = MODE_PLACE_STONE_BLOCK;
						break;
					case PAL_FIRE_BARR:
						set_string("Place fire barrier","Select location");
						overall_mode = MODE_PLACE_FIRE_BARRIER;
						break;
					case PAL_FORCE_BARR:
						set_string("Place force barrier","Select location");
						overall_mode = MODE_PLACE_FORCE_BARRIER;
						break;
					case PAL_QUICKFIRE:
						set_string("Place quickfire","Select location");
						overall_mode = MODE_PLACE_QUICKFIRE;
						break;
					case PAL_SPEC_SPOT:
						set_string(editing_town ? "Place special spot" : "Toggle special spot","Select location");
						overall_mode = MODE_TOGGLE_SPECIAL_DOT;
						break;
					case PAL_ROAD:
						set_string(editing_town ? "Place roads" : "Toggle roads", "Select location");
						overall_mode = MODE_TOGGLE_ROAD;
						break;
					case PAL_FORCECAGE:
						set_string("Place forcecage","Select location");
						overall_mode = MODE_PLACE_FORCECAGE;
						break;
					case PAL_ERASE_FIELD:
						set_string("Clear space","Select space to clear");
						overall_mode = MODE_CLEAR_FIELDS;
						break;
					case PAL_SFX_SB:
						set_string("Place small blood stain","Select stain location");
						overall_mode = MODE_PLACE_SFX;
						mode_count = 0;
						break;
					case PAL_SFX_MB:
						set_string("Place ave. blood stain","Select stain location");
						overall_mode = MODE_PLACE_SFX;
						mode_count = 1;
						break;
					case PAL_SFX_LB:
						set_string("Place large blood stain","Select stain location");
						overall_mode = MODE_PLACE_SFX;
						mode_count = 2;
						break;
					case PAL_SFX_SS:
						set_string("Place small slime pool","Select slime location");
						overall_mode = MODE_PLACE_SFX;
						mode_count = 3;
						break;
					case PAL_SFX_LS:
						set_string("Place large slime pool","Select slime location");
						overall_mode = MODE_PLACE_SFX;
						mode_count = 4;
						break;
					case PAL_SFX_ASH:
						set_string("Place ash","Select ash location");
						overall_mode = MODE_PLACE_SFX;
						mode_count = 5;
						break;
					case PAL_SFX_BONE:
						set_string("Place bones","Select bones location");
						overall_mode = MODE_PLACE_SFX;
						mode_count = 6;
						break;
					case PAL_SFX_ROCK:
						set_string("Place rocks","Select rocks location");
						overall_mode = MODE_PLACE_SFX;
						mode_count = 7;
						break;
					case PAL_TERRAIN: // Terrain palette
						draw_mode = DRAW_TERRAIN;
						set_up_terrain_buttons(true);
						break;
					case PAL_ITEM: // Item palette
						draw_mode = DRAW_ITEM;
						set_up_terrain_buttons(true);
						break;
					case PAL_MONST: // Monster palette
						draw_mode = DRAW_MONST;
						set_up_terrain_buttons(true);
						break;
					case PAL_BOAT:
						set_string("Place/edit boat","Select boat location");
						overall_mode = MODE_PLACE_BOAT;
						break;
					case PAL_HORSE:
						set_string("Place/edit horse","Select horse location");
						overall_mode = MODE_PLACE_HORSE;
						break;
				}
				return true;
			}
		}
	return false;
}

void handle_action(location the_point,sf::Event /*event*/) {
	using kb = sf::Keyboard;
	std::string s2;
	
	bool option_hit = false,ctrl_hit = false;
	location spot_hit;
	location cur_point,cur_point2;
	rectangle temp_rect;
	if(kb::isKeyPressed(kb::LAlt) || kb::isKeyPressed(kb::RAlt))
		option_hit = true;
	if(kb::isKeyPressed(kb::LControl) || kb::isKeyPressed(kb::RControl))
		ctrl_hit = true;
	
	if(handle_lb_action(the_point))
		return;
	
	if(overall_mode == MODE_MAIN_SCREEN && handle_rb_action(the_point, option_hit))
		return;
		
	update_mouse_spot(the_point);
	if(overall_mode < MODE_MAIN_SCREEN && handle_terrain_action(the_point, ctrl_hit))
		return;
	
	if(!mouse_button_held && ((overall_mode < MODE_MAIN_SCREEN) || (overall_mode == MODE_EDIT_TYPES))) {
		cur_point = the_point;
		cur_point.x -= RIGHT_AREA_UL_X;
		cur_point.y -= RIGHT_AREA_UL_Y;
		if(handle_terpal_action(cur_point, option_hit))
			return;

		cur_point2 = the_point;
		cur_point2.x -= 5;
		cur_point2.y -= terrain_rects[255].bottom + 5;
		if(handle_toolpal_action(cur_point2))
			return;
	}
	
	return;
}


void flash_rect(rectangle /*to_flash*/) {
	// TODO: Determine a good way to do this
//	InvertRect (&to_flash);
	play_sound(37);
	sf::sleep(time_in_ticks(10));
	
}



void swap_terrain() {
	short a,b,c;
	ter_num_t ter;
	
	if(!change_ter(a,b,c)) return;
	
	for(short i = 0; i < ((editing_town) ? town->max_dim : 48); i++)
		for(short j = 0; j < ((editing_town) ? town->max_dim : 48); j++) {
			ter = editing_town ? town->terrain(i,j) : current_terrain->terrain[i][j];
			if((ter == a) && (get_ran(1,1,100) <= c)) {
				if(editing_town)
					town->terrain(i,j) = b;
				else current_terrain->terrain[i][j] = b;
			}
		}
	
}

void set_new_terrain(ter_num_t selected_terrain) {
	if(selected_terrain >= scenario.ter_types.size()) return;
	current_terrain_type = selected_terrain;
	current_ground = scenario.get_ground_from_ter(selected_terrain);
	cTerrain& ter = scenario.ter_types[current_terrain_type];
	cTerrain& gter = scenario.ter_types[current_ground];
	if(gter.blockage >= eTerObstruct::BLOCK_MOVE || ter.trim_type == eTrimType::WALKWAY || /*current_ground == current_terrain_type ||*/
	   (ter.trim_type >= eTrimType::S && ter.trim_type <= eTrimType::NW_INNER)) {
		long trim = scenario.ter_types[current_ground].trim_ter;
		if(trim < 0) current_ground = 0;
		else current_ground = scenario.get_ter_from_ground(trim);
	}
	set_string(current_string[0],scenario.ter_types[current_terrain_type].name);
}

void handle_keystroke(sf::Event event) {
	using kb = sf::Keyboard;
	using Key = sf::Keyboard::Key;
	
	Key keypad[10] = {kb::Numpad0,kb::Numpad1,kb::Numpad2,kb::Numpad3,kb::Numpad4,kb::Numpad5,kb::Numpad6,kb::Numpad7,kb::Numpad8,kb::Numpad9};
	Key arrows[4] = {kb::Down, kb::Left, kb::Right, kb::Up};
	// TODO: The repetition of location shouldn't be needed here!
	location terrain_click[10] = {location{0,0},		// 0
		location{6,356}, location{140,356}, location{270,356},
		location{6,190}, location{140,190}, location{270,190},
		location{6,24}, location{140,20}, location{270,24},
	};
	location pass_point;
	Key chr2 = event.key.code;
	char chr;
	short store_ter;
	
	obscureCursor();
	
	if(overall_mode >= MODE_MAIN_SCREEN)
		return;
	
	for(short i = 0; i < 10; i++)
		if(chr2 == keypad[i] || (i % 2 == 0 && i > 0 && chr2 == arrows[i / 2 - 1])) {
			if(i == 0) {
				chr = 'z';
			}
			else {
				pass_point = terrain_click[i];
				handle_action(pass_point,event);
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
			for(short i = 0; i < town->preset_items.size(); i++) {
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
			if(chr >= 'a' && chr <= 'z') {
				for(short i = 0; i < scenario.ter_types.size(); i++) {
					int j = current_terrain_type + i + 1;
					j %= scenario.ter_types.size();
					if(scenario.ter_types[j].shortcut_key == chr) {
						set_new_terrain(j);
						place_location();
						break;
					}
				}
				
			}
			break;
			
	}
	if(store_ter != current_terrain_type)
		draw_terrain();
	mouse_button_held = false;
}

void handle_scroll(sf::Event& event) {
	rectangle pal_rect = terrain_buttons_rect, right_area_rect = {0,0,RIGHT_AREA_HEIGHT,RIGHT_AREA_WIDTH};
	right_area_rect.offset(RIGHT_AREA_UL_X, RIGHT_AREA_UL_Y);
	pal_rect.offset(RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	pal_rect.height() = 16 * 17 + 2;
	fill_rect(mainPtr, right_area_rect, sf::Color::Magenta);
	location pos(event.mouseWheel.x, event.mouseWheel.y);
	int amount = event.mouseWheel.delta;
	if(right_sbar->isVisible() && pos.in(right_area_rect)) {
		right_sbar->setPosition(right_sbar->getPosition() - amount);
		redraw_screen();
	} else if(pal_sbar->isVisible() && pos.in(pal_rect)) {
		pal_sbar->setPosition(pal_sbar->getPosition() - amount);
		set_up_terrain_buttons(false);
		redraw_screen();
	} else if(overall_mode < MODE_MAIN_SCREEN && pos.in(terrain_rect)) {
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
			cen_x = minmax(4, town->max_dim - 5, cen_x - amount);
		else cen_y = minmax(4, town->max_dim - 5, cen_y - amount);
		redraw_screen();
	}
}

void change_circle_terrain(location center,short radius,ter_num_t terrain_type,short probability) {
	// prob is 0 - 20, 0 no, 20 always
	location l;
	
	for(short i = 0; i < ((editing_town) ? town->max_dim : 48); i++)
		for(short j = 0; j < ((editing_town) ? town->max_dim : 48); j++) {
			l.x = i;
			l.y = j;
			if((dist(center,l) <= radius) && (get_ran(1,1,20) <= probability))
				set_terrain(l,terrain_type);
		}
}

void change_rect_terrain(rectangle r,ter_num_t terrain_type,short probability,bool hollow) {
	// prob is 0 - 20, 0 no, 20 always
	location l;
	
	for(short i = 0; i < ((editing_town) ? town->max_dim : 48); i++)
		for(short j = 0; j < ((editing_town) ? town->max_dim : 48); j++) {
			l.x = i;
			l.y = j;
			if((i >= r.left) && (i <= r.right) && (j >= r.top) && (j <= r.bottom)
				&& (!hollow || (i == r.left) || (i == r.right) || (j == r.top) || (j == r.bottom))
				&& ((hollow) || (get_ran(1,1,20) <= probability)))
				set_terrain(l,terrain_type);
		}
}

void flood_fill_terrain(location start, ter_num_t terrain_type) {
	static const int dx[4] = {0, 1, 0, -1};
	static const int dy[4] = {-1, 0, 1, 0};
	ter_num_t to_replace = editing_town ? town->terrain(start.x, start.y) : current_terrain->terrain[start.x][start.y];
	std::stack<location> to_visit;
	std::set<location, loc_compare> visited;
	to_visit.push(start);
	int max_dim = editing_town ? town->max_dim : current_terrain->max_dim;
	
	while(!to_visit.empty()) {
		location this_loc = to_visit.top();
		to_visit.pop();
		visited.insert(this_loc);
		for(int i = 0; i < 4; i++) {
			location adj_loc = this_loc;
			adj_loc.x += dx[i];
			adj_loc.y += dy[i];
			if(adj_loc.x < 0 || adj_loc.x >= max_dim || adj_loc.y < 0 || adj_loc.y >= max_dim)
				continue;
			ter_num_t check = editing_town ? town->terrain(adj_loc.x, adj_loc.y) : current_terrain->terrain[adj_loc.x][adj_loc.y];
			if(check == to_replace && !visited.count(adj_loc))
				to_visit.push(adj_loc);
		}
		if(editing_town)
			town->terrain(this_loc.x, this_loc.y) = terrain_type;
		else current_terrain->terrain[this_loc.x][this_loc.y] = terrain_type;
	}
}

void frill_up_terrain() {
	ter_num_t terrain_type;
	
	for(short i = 0; i < ((editing_town) ? town->max_dim : 48); i++)
		for(short j = 0; j < ((editing_town) ? town->max_dim : 48); j++) {
			terrain_type = editing_town ? town->terrain(i,j) : current_terrain->terrain[i][j];
			
			for(size_t k = 0; k < scenario.ter_types.size(); k++) {
				if(terrain_type == k) continue;
				cTerrain& ter = scenario.ter_types[k];
				if(terrain_type == ter.frill_for && get_ran(1,1,100) < ter.frill_chance)
					terrain_type = k;
			}
			
			if(editing_town)
				town->terrain(i,j) = terrain_type;
			else current_terrain->terrain[i][j] = terrain_type;
		}
	draw_terrain();
}

void unfrill_terrain() {
	ter_num_t terrain_type;
	
	for(short i = 0; i < ((editing_town) ? town->max_dim : 48); i++)
		for(short j = 0; j < ((editing_town) ? town->max_dim : 48); j++) {
			terrain_type = editing_town ? town->terrain(i,j) : current_terrain->terrain[i][j];
			cTerrain& ter = scenario.ter_types[terrain_type];
			
			if(ter.frill_for >= 0)
				terrain_type = ter.frill_for;
			
			if(editing_town)
				town->terrain(i,j) = terrain_type;
			else current_terrain->terrain[i][j] = terrain_type;
		}
	draw_terrain();
}

static ter_num_t find_object_part(unsigned char num, short x, short y, ter_num_t fallback){
	for(int i = 0; i < scenario.ter_types.size(); i++){
		if(scenario.ter_types[i].obj_num == num &&
		   scenario.ter_types[i].obj_pos.x == x &&
		   scenario.ter_types[i].obj_pos.y == y)
			return i;
	}
	return fallback;
}

bool terrain_matches(unsigned char x, unsigned char y, ter_num_t ter) {
	ter_num_t ter2;
	if(editing_town) ter2 = town->terrain(x,y); else ter2 = current_terrain->terrain[x][y];
	if(ter2 == ter) return true;
	if(scenario.ter_types[ter2].ground_type != scenario.ter_types[ter].ground_type)
		return false;
	if(scenario.ter_types[ter].trim_type == eTrimType::NONE &&
	   scenario.ter_types[ter2].trim_type >= eTrimType::S &&
	   scenario.ter_types[ter2].trim_type <= eTrimType::NW_INNER)
		return ter == scenario.get_ground_from_ter(ter);
	if(scenario.ter_types[ter2].trim_type == eTrimType::NONE &&
	   scenario.ter_types[ter].trim_type >= eTrimType::S &&
	   scenario.ter_types[ter].trim_type <= eTrimType::NW_INNER)
		return ter2 == scenario.get_ground_from_ter(ter2);
	if(scenario.ter_types[ter2].trim_type >= eTrimType::S &&
	   scenario.ter_types[ter2].trim_type <= eTrimType::NW_INNER &&
	   scenario.ter_types[ter].trim_type >= eTrimType::S &&
	   scenario.ter_types[ter].trim_type <= eTrimType::NW_INNER)
		return scenario.ter_types[ter].trim_type != scenario.ter_types[ter2].trim_type;
	return false;
}

static const std::array<location,5> trim_diffs = {{
	loc(0,0), loc(-1,0), loc(1,0), loc(0,-1), loc(0,1)
}};

void set_terrain(location l,ter_num_t terrain_type) {
	location l2;
	
	if((editing_town) && ((l.x < 0) || (l.x > town->max_dim - 1) || (l.y < 0) || (l.y > town->max_dim - 1)))
		return ;
	if(!editing_town && ((l.x < 0) || (l.x > 47) || (l.y < 0) || (l.y > 47)))
		return ;
	
	if(editing_town)
		town->terrain(l.x,l.y) = terrain_type;
	else current_terrain->terrain[l.x][l.y] = terrain_type;
	l2 = l;
	
	// Large objects (eg rubble)
	if(scenario.ter_types[terrain_type].obj_num > 0){
		int q = scenario.ter_types[terrain_type].obj_num;
		location obj_loc = scenario.ter_types[terrain_type].obj_pos;
		location obj_dim = scenario.ter_types[terrain_type].obj_size;
		while(obj_loc.x > 0) l2.x-- , obj_loc.x--;
		while(obj_loc.y > 0) l2.y-- , obj_loc.y--;
		for(short i = 0; i < obj_dim.x; i++)
			for(short j = 0; j < obj_dim.y; j++){
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
				if(new_ter > scenario.ter_types.size()) continue;
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
	
	if(scenario.ter_types[terrain_type].special == eTerSpec::IS_A_SIGN) {
		if(!editing_town && (l.x == 0 || l.x == 47 || l.y == 0 || l.y == 47)) {
			cChoiceDlog("not-at-edge").show();
			mouse_button_held = false;
			return;
		}
		auto& signs = editing_town ? town->sign_locs : current_terrain->sign_locs;
		auto iter = std::find(signs.begin(), signs.end(), l);
		if(iter == signs.end()) {
			iter = std::find_if(signs.begin(), signs.end(), [](const sign_loc_t& sign) {
				if(sign.x == 100) return true;
				ter_num_t ter = editing_town ? town->terrain(sign.x,sign.y) : current_terrain->terrain[sign.x][sign.y];
				return scenario.ter_types[ter].special != eTerSpec::IS_A_SIGN;
			});
			if(iter == signs.end()) {
				signs.emplace_back();
				iter = signs.end() - 1;
			}
		}
		static_cast<location&>(*iter) = l;
		ter_num_t terrain_type = editing_town ? town->terrain(iter->x,iter->y) : current_terrain->terrain[iter->x][iter->y];
		edit_sign(*iter, iter - signs.begin(), scenario.ter_types[terrain_type].picture);
		mouse_button_held = false;
	}
}

void adjust_space(location l) {
	bool needed_change = false;
	location l2;
	short i,j;
	
	i = l.x;
	j = l.y;
	if((editing_town) && ((i < 0) || (i > town->max_dim - 1) || (j < 0) || (j > town->max_dim - 1)))
		return ;
	if(!editing_town && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return ;
	size_t size = editing_town ? town->max_dim : 48;
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
	if((which_item < 0) || (which_item >= scenario.scen_items.size()))
		return true;
	if(scenario.scen_items[which_item].variety == eItemType::NO_ITEM)
		return true;
	if(get_ran(1,1,100) > odds)
		return false;
	for(short x = 0; x < town->preset_items.size(); x++)
		if(town->preset_items[x].code < 0) {
			town->preset_items[x] = {spot_hit, which_item, scenario.scen_items[which_item]};
			town->preset_items[x].contained = container_there(spot_hit);
			town->preset_items[x].property = property;
			town->preset_items[x].always_there = always;
			return true;
		}
	town->preset_items.push_back({spot_hit, which_item, scenario.scen_items[which_item]});
	town->preset_items.back().contained = container_there(spot_hit);
	town->preset_items.back().property = property;
	town->preset_items.back().always_there = always;
	return true;
}

void place_items_in_town() {
	location l;
	for(short i = 0; i < town->max_dim; i++)
		for(short j = 0; j < town->max_dim; j++) {
			l.x = i;
			l.y = j;
			
			for(short k = 0; k < 10; k++)
				if(town->terrain(i,j) == scenario.storage_shortcuts[k].ter_type) {
					for(short x = 0; x < 10; x++)
						place_item(l,scenario.storage_shortcuts[k].item_num[x],
								   scenario.storage_shortcuts[k].property,false,
								   scenario.storage_shortcuts[k].item_odds[x]);
				}
		}
	draw_terrain();
}

void place_edit_special(location loc) {
	if(!editing_town && (loc.x == 0 || loc.x == 47 || loc.y == 0 || loc.y == 47)) {
		cChoiceDlog("not-at-edge").show();
		return;
	}
	auto& specials = editing_town ? town->special_locs : current_terrain->special_locs;
	for(short i = 0; i < specials.size(); i++)
		if(specials[i] == loc && specials[i].spec >= 0) {
			edit_spec_enc(specials[i].spec, editing_town ? 2 : 1, nullptr);
			return;
		}
	// new special
	short spec = get_fresh_spec(editing_town ? 2 : 1);
	for(short i = 0; i <= specials.size(); i++) {
		if(i == specials.size())
			specials.emplace_back(-1,-1,-1);
		if(specials[i].spec < 0) {
			if(edit_spec_enc(spec, editing_town ? 2: 1, nullptr)) {
				specials[i] = loc;
				specials[i].spec = spec;
			}
			break;
		}
	}
}

void set_special(location spot_hit) {
	if(!editing_town && (spot_hit.x == 0 || spot_hit.x == 47 || spot_hit.y == 0 || spot_hit.y == 47)) {
		cChoiceDlog("not-at-edge").show();
		return;
	}
	auto& specials = editing_town ? town->special_locs : current_terrain->special_locs;
	for(short x = 0; x < specials.size(); x++)
		if(specials[x] == spot_hit && specials[x].spec >= 0) {
			int spec = edit_special_num(editing_town ? 2 : 1,specials[x].spec);
			if(spec >= 0) specials[x].spec = spec;
			return;
		}
	for(short x = 0; x <= specials.size(); x++) {
		if(x == specials.size())
			specials.emplace_back(-1,-1,-1);
		if(specials[x].spec < 0) {
			int spec = edit_special_num(editing_town ? 2 : 1, 0);
			if(spec >= 0) {
				specials[x] = spot_hit;
				specials[x].spec = spec;
			}
			break;
		}
	}
}

void town_entry(location spot_hit) {
	ter_num_t ter = current_terrain->terrain[spot_hit.x][spot_hit.y];
	if(scenario.ter_types[ter].special != eTerSpec::TOWN_ENTRANCE) {
		showError("This space isn't a town entrance. Town entrances are marked by a small brown castle icon.");
		return;
	}
	// clean up old town entries
	for(short x = 0; x < current_terrain->city_locs.size(); x++)
		if(current_terrain->city_locs[x].spec >= 0) {
			ter = current_terrain->terrain[current_terrain->city_locs[x].x][current_terrain->city_locs[x].y];
			if(scenario.ter_types[ter].special != eTerSpec::TOWN_ENTRANCE)
				current_terrain->city_locs[x].spec = -1;
		}
	auto iter = std::find(current_terrain->city_locs.begin(), current_terrain->city_locs.end(), spot_hit);
	if(iter != current_terrain->city_locs.end()) {
		int town = pick_town_num("select-town-enter",iter->spec,scenario);
		if(town >= 0) iter->spec = town;
	} else {
		iter = std::find_if(current_terrain->city_locs.begin(), current_terrain->city_locs.end(), [](const spec_loc_t& loc) {
			return loc.spec < 0;
		});
		if(iter != current_terrain->city_locs.end()) {
			int town = pick_town_num("select-town-enter",0,scenario);
			if(town >= 0) {
				*iter = spot_hit;
				iter->spec = town;
			}
		} else {
			int town = pick_town_num("select-town-enter",0,scenario);
			if(town >= 0) {
				current_terrain->city_locs.emplace_back(spot_hit);
				current_terrain->city_locs.back().spec = town;
			}
		}
	}
}

static std::string version() {
	static std::string version;
	if(version.empty()) {
		std::ostringstream sout;
		sout << "Version " << oboeVersionString();
#if defined(GIT_REVISION) && defined(GIT_TAG_REVISION)
		if(strcmp(GIT_REVISION, GIT_TAG_REVISION) != 0) {
			sout << " [" << GIT_REVISION << "]";
		}
#endif
		version = sout.str();
	}
	return version;
}

// is slot >= 0, force that slot
// if -1, use 1st free slot
void set_up_start_screen() {
	reset_lb();
	reset_rb();
	set_lb(0,LB_TITLE,LB_NO_ACTION,"Blades of Exile");
	set_lb(1,LB_TITLE,LB_NO_ACTION,"Scenario Editor");
	set_lb(3,LB_TEXT,LB_NEW_SCEN,"Make New Scenario");
	set_lb(4,LB_TEXT,LB_LOAD_SCEN,"Load Scenario");
	set_lb(7,LB_TEXT,LB_NO_ACTION,"To find out how to use the");
	set_lb(8,LB_TEXT,LB_NO_ACTION,"editor, select Getting Started ");
	set_lb(9,LB_TEXT,LB_NO_ACTION,"from the Help menu.");
	set_lb(NLS - 6,LB_TEXT,LB_NO_ACTION,"Be sure to read the file Blades");
	set_lb(NLS - 5,LB_TEXT,LB_NO_ACTION,"of Exile License. Using this");
	set_lb(NLS - 4,LB_TEXT,LB_NO_ACTION,"program implies that you agree ");
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"with the terms of the license.");
	set_lb(NLS - 2,LB_TEXT,LB_NO_ACTION,"Copyright 1997, All rights reserved.");
	set_lb(NLS - 1,LB_TEXT,LB_NO_ACTION,version());
	change_made = false;
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
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
	strb << "  Town " << cur_town << ": " << town->name;
	set_lb(-1,LB_TEXT,LB_NO_ACTION, strb.str());
	set_lb(-1,LB_TEXT,LB_LOAD_TOWN,"Load Another Town");
	set_lb(-1,LB_TEXT,LB_EDIT_TOWN,"Edit Town Terrain");
	set_lb(-1,LB_TEXT,LB_EDIT_TALK,"Edit Town Dialogue");
	set_lb(NLS - 2,LB_TEXT,LB_NO_ACTION,"Copyright 1997, All rights reserved.");
	set_lb(NLS - 1,LB_TEXT,LB_NO_ACTION,version());
	overall_mode = MODE_MAIN_SCREEN;
	right_sbar->show();
	pal_sbar->hide();
	shut_down_menus(4);
	shut_down_menus(3);
	redraw_screen();
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
}

void start_town_edit() {
	std::ostringstream strb;
	small_any_drawn = false;
	cen_x = town->max_dim / 2;
	cen_y = town->max_dim / 2;
	reset_lb();
	strb << "Editing Town " << cur_town;
	set_lb(0,LB_TITLE,LB_NO_ACTION,strb.str());
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,town->name);
	set_lb(NLS - 2,LB_TEXT,LB_RETURN,"Back to Main Menu");
	set_lb(NLS - 1,LB_TEXT,LB_NO_ACTION,"(Click border to scroll view.)");
	overall_mode = MODE_DRAWING;
	editing_town = true;
	set_up_terrain_buttons(true);
	shut_down_menus(4);
	shut_down_menus(2);
	right_sbar->hide();
	pal_sbar->show();
	set_string("Drawing mode",scenario.ter_types[current_terrain_type].name);
	place_location();
	copied_spec = -1;
	for(short i = 0; i < town->max_dim; i++)
		for(short j = 0; j < town->max_dim; j++)
			if(town->terrain(i,j) == 0)
				current_ground = 0;
			else if(town->terrain(i,j) == 2)
				current_ground = 2;
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
}

void start_out_edit() {
	std::ostringstream strb;
	small_any_drawn = false;
	cen_x = 24;
	cen_y = 24;
	reset_lb();
	strb << "Editing outdoors (" << cur_out.x << ',' << cur_out.y << ")";
	set_lb(0,LB_TITLE,LB_NO_ACTION,strb.str());
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,current_terrain->name);
	set_lb(NLS - 2,LB_TEXT,LB_RETURN,"Back to Main Menu");
	set_lb(NLS - 1,LB_TEXT,LB_NO_ACTION,"(Click border to scroll view.)");
	overall_mode = MODE_DRAWING;
	draw_mode = DRAW_TERRAIN;
	editing_town = false;
	set_up_terrain_buttons(true);
	right_sbar->hide();
	pal_sbar->show();
	shut_down_menus(4);
	shut_down_menus(1);
	set_string("Drawing mode",scenario.ter_types[current_terrain_type].name);
	place_location();
	copied_spec = -1;
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 48; j++)
			if(current_terrain->terrain[i][j] == 0)
				current_ground = 0;
			else if(current_terrain->terrain[i][j] == 2)
				current_ground = 2;
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
	redraw_screen();
}

void start_terrain_editing() {
	right_sbar->hide();
	pal_sbar->show();
	overall_mode = MODE_EDIT_TYPES;
	draw_mode = DRAW_TERRAIN;
	set_up_terrain_buttons(true);
	place_location();
	
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Alt-click to delete/clear",true);
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
}

void start_monster_editing(bool just_redo_text) {
	int num_options = scenario.scen_monsters.size() + 1;
	
	if(!just_redo_text) {
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		pal_sbar->hide();
		right_sbar->setPosition(0);
		
		reset_rb();
		right_sbar->setMaximum(num_options - 1 - NRSONPAGE);
	}
	for(short i = 1; i < num_options; i++) {
		std::string title;
		if(i == scenario.scen_monsters.size())
			title = "Create New Monster";
		else title = scenario.scen_monsters[i].m_name;
		title = std::to_string(i) + " - " + title;
		set_rb(i - 1,RB_MONST, i, title);
	}
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Alt-click to delete",true);
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
	redraw_screen();
}

void start_item_editing(bool just_redo_text) {
	int num_options = scenario.scen_items.size() + 1;
	bool draw_full = false;
	
	if(!just_redo_text) {
		if(overall_mode < MODE_MAIN_SCREEN)
			set_up_main_screen();
		if(overall_mode == MODE_EDIT_TYPES)
			draw_full = true;
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		pal_sbar->hide();
		
		right_sbar->setPosition(0);
		reset_rb();
		right_sbar->setMaximum(num_options - NRSONPAGE);
	}
	for(short i = 0; i < num_options; i++) {
		std::string title;
		if(i == scenario.scen_items.size())
			title = "Create New Item";
		else title = scenario.scen_items[i].full_name;
		title = std::to_string(i) + " - " + title;
		set_rb(i,RB_ITEM, i, title);
	}
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Alt-click to delete",true);
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
	redraw_screen();
}

void start_special_item_editing(bool just_redo_text) {
	int num_options = scenario.special_items.size() + 1;
	
	if(!just_redo_text) {
		if(overall_mode < MODE_MAIN_SCREEN)
			set_up_main_screen();
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		pal_sbar->hide();
		
		right_sbar->setPosition(0);
		reset_rb();
		right_sbar->setMaximum(num_options - NRSONPAGE);
	}
	for(short i = 0; i < num_options; i++) {
		std::string title;
		if(i == scenario.special_items.size())
			title = "Create New Special Item";
		else title = scenario.special_items[i].name;
		title = std::to_string(i) + " - " + title;
		set_rb(i,RB_SPEC_ITEM, i, title);
	}
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Alt-click to delete",true);
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
	redraw_screen();
}

void start_quest_editing(bool just_redo_text) {
	int num_options = scenario.quests.size() + 1;
	if(!just_redo_text) {
		if(overall_mode < MODE_MAIN_SCREEN)
			set_up_main_screen();
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		pal_sbar->hide();
		right_sbar->setPosition(0);
		reset_rb();
		right_sbar->setMaximum(num_options - NRSONPAGE);
	}
	for(int i = 0; i < num_options; i++) {
		std::string title;
		if(i == scenario.quests.size())
			title = "Create New Quest";
		else title = scenario.quests[i].name;
		title = std::to_string(i) + " - " + title;
		set_rb(i, RB_QUEST, i, title);
	}
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Alt-click to delete",true);
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
	redraw_screen();
}

void start_shops_editing(bool just_redo_text) {
	int num_options = scenario.shops.size() + 1;
	if(!just_redo_text) {
		if(overall_mode < MODE_MAIN_SCREEN)
			set_up_main_screen();
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		pal_sbar->hide();
		right_sbar->setPosition(0);
		reset_rb();
		right_sbar->setMaximum(num_options - NRSONPAGE);
	}
	for(int i = 0; i < num_options; i++) {
		std::string title;
		if(i == scenario.shops.size())
			title = "Create New Shop";
		else title = scenario.shops[i].getName();
		title = std::to_string(i) + " - " + title;
		set_rb(i, RB_SHOP, i, title);
	}
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Alt-click to delete",true);
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
	redraw_screen();
}

extern size_t num_strs(short mode); // defined in scen.keydlgs.cpp

// mode 0 - scen 1 - out 2 - town 3 - journal
// if just_redo_text not 0, simply need to update text portions
void start_string_editing(eStrMode mode,short just_redo_text) {
	long pos;
	
	if(just_redo_text == 0) {
		if(overall_mode < MODE_MAIN_SCREEN)
			set_up_main_screen();
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		pal_sbar->hide();
		
		reset_rb();
		right_sbar->setMaximum(num_strs(mode) + 1 - NRSONPAGE);
	}
	size_t num_strs = ::num_strs(mode);
	for(size_t i = 0; i < num_strs; i++) {
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
			case 6:
				str << i << " - " << current_terrain->area_desc[i].descr.substr(0,30);
				set_rb(i,RB_OUT_RECT, i,str.str());
				break;
			case 7:
				str << i << " - " << town->area_desc[i].descr.substr(0,30);
				set_rb(i,RB_TOWN_RECT, i,str.str());
				break;
		}
	}
	std::string make_new = std::to_string(num_strs) + " - Create New String";
	switch(mode) {
		case 0: set_rb(num_strs, RB_SCEN_STR, num_strs, make_new); break;
		case 1: set_rb(num_strs, RB_OUT_STR, num_strs, make_new); break;
		case 2: set_rb(num_strs, RB_TOWN_STR, num_strs, make_new); break;
		case 3: set_rb(num_strs, RB_JOURNAL, num_strs, make_new); break;
		case 4: set_rb(num_strs, RB_OUT_SIGN, num_strs, make_new); break;
		case 5: set_rb(num_strs, RB_TOWN_SIGN, num_strs, make_new); break;
		case 6: set_rb(num_strs, RB_OUT_RECT, num_strs, make_new); break;
		case 7: set_rb(num_strs, RB_TOWN_RECT, num_strs, make_new); break;
	}
	
	pos = right_sbar->getPosition();
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Alt-click to delete",true);
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
	redraw_screen();
}

// mode 0 - scen 1 - out 2 - town
// if just_redo_text not 0, simply need to update text portions
void start_special_editing(short mode,short just_redo_text) {
	size_t num_specs;
	switch(mode) {
		case 0: num_specs = scenario.scen_specials.size(); break;
		case 1: num_specs = current_terrain->specials.size(); break;
		case 2: num_specs = town->specials.size(); break;
	}
	
	if(just_redo_text == 0) {
		if(overall_mode < MODE_MAIN_SCREEN)
			set_up_main_screen();
		overall_mode = MODE_MAIN_SCREEN;
		right_sbar->show();
		pal_sbar->hide();
		
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
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Alt-click to delete",true);
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
	redraw_screen();
}

// if restoring is 1, this is just a redraw, so don't move scroll bar position
void start_dialogue_editing(short restoring) {
	char s[15] = "    ,      ";
	
	if(overall_mode < MODE_MAIN_SCREEN)
		set_up_main_screen();
	overall_mode = MODE_MAIN_SCREEN;
	right_sbar->show();
	pal_sbar->hide();
	
	if(restoring == 0) {
		right_sbar->setPosition(0);
		reset_rb();
		right_sbar->setMaximum(70 - NRSONPAGE);
	}
	for(short i = 0; i < 10; i++) {
		std::ostringstream strb;
		strb << "Personality " << (i + cur_town * 10) << " - " << town->talking.people[i].title;
		set_rb(i,RB_PERSONALITY, i, strb.str());
	}
	size_t n_nodes = town->talking.talk_nodes.size();
	for(short i = 0; i < n_nodes; i++) {
		for(short j = 0; j < 4; j++) {
			s[j] = town->talking.talk_nodes[i].link1[j];
			s[j + 6] = town->talking.talk_nodes[i].link2[j];
		}
		std::ostringstream strb;
		strb << "Node " << i << " - Per. " << town->talking.talk_nodes[i].personality << ", " << s;
		set_rb(10 + i,RB_DIALOGUE, i, strb.str());
	}
	set_rb(10 + n_nodes, RB_DIALOGUE, n_nodes, "Create New Node");
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Alt-click node to delete",true);
	update_mouse_spot(sf::Mouse::getPosition(mainPtr));
	redraw_screen();
}

bool save_check(std::string which_dlog) {
	std::string choice;
	
	if(!change_made)
		return true;
	choice = cChoiceDlog(which_dlog, {"save","revert","cancel"}).show();
	if(choice == "revert")
		return true;
	else if(choice == "cancel")
		return false;
	change_made = false;
	town->set_up_lights();
	save_scenario();
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
