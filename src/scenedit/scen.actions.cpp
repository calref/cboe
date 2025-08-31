
#include <cmath>
#include <cstdio>
#include <cstring>
#include <array>
#include <string>
#include <stack>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>
#include "scen.global.hpp"
#include "scenario/scenario.hpp"
#include "gfx/render_shapes.hpp"
#include "scen.graphics.hpp"
#include "scen.actions.hpp"
#include "sounds.hpp"
#include "scen.core.hpp"
#include "scen.fileio.hpp"
#include "scen.keydlgs.hpp"
#include "scen.townout.hpp"
#include "scen.menus.hpp"
#include "scen.undo.hpp"
#include "mathutil.hpp"
#include "fileio/fileio.hpp"
#include "tools/keymods.hpp"
#include "tools/winutil.hpp"
#include "tools/cursors.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/strchoice.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#ifndef MSBUILD_GITREV
#include "tools/gitrev.hpp"
#endif

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
cUndoList undo_list;

short flood_count = 0;

rectangle terrain_rects[16*TYPE_ROWS_EDITING],terrain_rect_base = {0,0,16,16},command_rects[21];
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
bool small_any_drawn = false;
extern bool change_made;

extern void save_prefs();

rectangle left_buttons[NLS][2]; // 0 - whole, 1 - blue button
std::array<lb_t,NLS> left_button_status;
std::vector<rb_t> right_button_status;
rectangle right_buttons[NRSONPAGE];
rectangle palette_buttons[10][6];
short current_rs_top = 0;
extern short right_button_hovered;

ePalBtn out_buttons[6][10] = {
	{PAL_PENCIL, PAL_BRUSH_LG, PAL_BRUSH_SM, PAL_SPRAY_LG, PAL_SPRAY_SM, PAL_ERASER, PAL_RECT_HOLLOW, PAL_RECT_FILLED, PAL_BUCKET, PAL_DROPPER},
	{PAL_EDIT_TOWN, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_EDIT_SIGN, PAL_TEXT_AREA, PAL_BLANK, PAL_START, PAL_ZOOM},
	{PAL_SPEC, PAL_COPY_SPEC, PAL_ERASE_SPEC, PAL_EDIT_SPEC, PAL_SPEC_SPOT, PAL_BOAT, PAL_HORSE, PAL_COPY_TER, PAL_CHANGE, PAL_PASTE},
	{PAL_ROAD, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
	{PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
	{PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK, PAL_BLANK},
};

ePalBtn town_buttons[6][10] = {
	{PAL_PENCIL, PAL_BRUSH_LG, PAL_BRUSH_SM, PAL_SPRAY_LG, PAL_SPRAY_SM, PAL_ERASER, PAL_RECT_HOLLOW, PAL_RECT_FILLED, PAL_BUCKET, PAL_DROPPER},
	{PAL_ENTER_N, PAL_ENTER_W, PAL_ENTER_S, PAL_ENTER_E, PAL_TOWN_BORDER, PAL_EDIT_SIGN, PAL_TEXT_AREA, PAL_BLANK, PAL_START, PAL_ZOOM},
	{PAL_SPEC, PAL_COPY_SPEC, PAL_ERASE_SPEC, PAL_EDIT_SPEC, PAL_SPEC_SPOT, PAL_BOAT, PAL_HORSE, PAL_COPY_TER, PAL_CHANGE, PAL_TERRAIN},
	{PAL_ROAD, PAL_WEB, PAL_CRATE, PAL_BARREL, PAL_BLOCK, PAL_EDIT_STORAGE, PAL_EDIT_ITEM, PAL_COPY_ITEM, PAL_ERASE_ITEM, PAL_ITEM},
	{PAL_FIRE_BARR, PAL_FORCE_BARR, PAL_QUICKFIRE, PAL_FORCECAGE, PAL_BLANK, PAL_PASTE, PAL_EDIT_MONST, PAL_COPY_MONST, PAL_ERASE_MONST, PAL_MONST},
	{PAL_SFX_SB, PAL_SFX_MB, PAL_SFX_LB, PAL_SFX_SS, PAL_SFX_LS, PAL_SFX_ASH, PAL_SFX_BONE, PAL_SFX_ROCK, PAL_ERASE_FIELD, PAL_BLANK},
};

rectangle working_rect;
location last_space_hit;
bool erasing_mode;
ter_num_t current_ground = 0;
location last_placement{-1,-1};

boost::variant<
	boost::none_t,
	std::pair<long /* special */, bool /* town */>,
	cTownperson /* monst */,
	cTown::cItem /* item */,
	vector2d<ter_num_t> /* terrain */
> clipboard = boost::none;

bool monst_on_space(location loc,short m_num);
static bool terrain_matches(unsigned char x, unsigned char y, ter_num_t ter);

bool check_for_interrupt(std::string); // to suppress "missing prototype" warning
bool check_for_interrupt(std::string) { return false; }

void init_screen_locs() {
	for(short i = 0; i < 4; i++)
		border_rect[i] = terrain_rect;
	border_rect[0].bottom = border_rect[0].top + 8;
	border_rect[1].right = border_rect[1].left + 8;
	border_rect[2].top = border_rect[2].bottom - 8;
	border_rect[3].left = border_rect[3].right - 8;
	
	for(short i = 0; i < 16 * TYPE_ROWS_EDITING; i++) {
		terrain_rects[i] = terrain_rect_base;
		terrain_rects[i].offset(3 + (i % 16) * (terrain_rect_base.right + 1),
			3 + (i / 16) * (terrain_rect_base.bottom + 1));
	}
}

static cursor_type get_edit_cursor() {
	switch(overall_mode) {
		case MODE_INTRO_SCREEN: case MODE_MAIN_SCREEN: case MODE_EDIT_TYPES:
		case MODE_EDIT_SPECIALS: case MODE_EDIT_SPECIAL_ITEMS:
		case MODE_EDIT_QUESTS: case MODE_EDIT_SHOPS: case MODE_EDIT_STRINGS:
		case MODE_EDIT_DIALOGUE:
			
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
		case MODE_STORAGE_RECT: case MODE_COPY_TERRAIN:
			return mode_count == 2 ? topleft_curs : bottomright_curs;
			
		case MODE_ERASE_CREATURE: case MODE_ERASE_ITEM:
		case MODE_ERASE_SPECIAL:
			
		case MODE_ERASER: case MODE_CLEAR_FIELDS:
			return eraser_curs;
			
		case MODE_EDIT_CREATURE: case MODE_EDIT_ITEM:
		case MODE_EDIT_SPECIAL: case MODE_EDIT_TOWN_ENTRANCE:
		case MODE_EDIT_SIGN:
			
		case MODE_COPY_SPECIAL: case MODE_COPY_ITEM:
		case MODE_COPY_CREATURE: case MODE_PASTE:
			
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
	if(overall_mode >= MODE_MAIN_SCREEN){
		set_cursor(sword_curs);

		// Mouse over right-side buttons: highlight which is selected, because accidental misclicks are common
		right_button_hovered = -1;
		for(int i = 0; i < NRSONPAGE; i++){
			if(the_point.in(right_buttons[i])){
				right_button_hovered = i;
			}
		}
	}
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

static bool handle_lb_action(int i){
	fs::path file_to_load;
	int j;
	draw_lb_slot(i,1);
	play_sound(37);
	mainPtr().display();
	// TODO: Proper button handling
	sf::sleep(time_in_ticks(10));
	draw_lb_slot(i,0);
	mainPtr().display();
	if(overall_mode >= MODE_MAIN_SCREEN) {
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
			case LB_LOAD_LAST:
				file_to_load = get_string_pref("LastScenario");
				// Skip first line of the fallthrough
				if(false)
			case LB_LOAD_SCEN:
				file_to_load = nav_get_scenario();
				if(!file_to_load.empty() && load_scenario(file_to_load, scenario)) {
					set_pref("LastScenario", file_to_load.string());
					save_prefs();
					restore_editor_state();
					undo_list.clear();
					update_edit_menu();
				} else if(!file_to_load.empty())
					// If we tried to load but failed, the scenario record is messed up, so boot to start screen.
					set_up_start_screen();
				break;
			case LB_EDIT_TER:
				start_type_editing(DRAW_TERRAIN);
				break;
			case LB_EDIT_MONST:
				start_type_editing(DRAW_MONST);
				break;
			case LB_EDIT_ITEM:
				start_type_editing(DRAW_ITEM);
				break;
			case LB_NEW_TOWN:
				if(scenario.towns.size() >= 200) {
					showError("You have reached the limit of 200 towns you can have in one scenario.");
					return true;
				}
				if(new_town())
					handle_close_terrain_view(MODE_MAIN_SCREEN);
				break;
			case LB_EDIT_TEXT:
				right_sbar->setPosition(0);
				start_string_editing(STRS_SCEN);
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
				spot_hit = pick_out(cur_out, scenario, "Edit");
				if(spot_hit != cur_out) {
					set_current_out(spot_hit, false);
					if(overall_mode == MODE_EDIT_SPECIALS){
						start_special_editing(1);
					}
				}
				break;
			case LB_EDIT_OUT:
				start_out_edit();
				mouse_button_held = false;
				break;
			case LB_LOAD_TOWN:
				j = pick_town_num("select-town-edit",cur_town,scenario);
				if(j >= 0){
					cur_town = j;
					town = scenario.towns[cur_town];
					set_up_main_screen();
					if(overall_mode == MODE_EDIT_SPECIALS){
						start_special_editing(2);
					}
				}
				break;
			case LB_EDIT_TOWN:
				start_town_edit();
				mouse_button_held = false;
				break;
			case LB_EDIT_TALK:
				start_dialogue_editing();
				break;
		}
	}
	if((overall_mode < MODE_MAIN_SCREEN) && left_button_status[i].action == LB_RETURN) {
		handle_close_terrain_view(MODE_MAIN_SCREEN);
	}
	mouse_button_held = false;
	update_edit_menu();
	return true;
}

static bool handle_lb_click(location the_point) {
	for(int i = 0; i < NLS; i++)
		if(!mouse_button_held && the_point.in(left_buttons[i][0])
		   && (left_button_status[i].action != LB_NO_ACTION))  {
			return handle_lb_action(i);
		}
	return false;
}

static void clear_string(eStrMode mode, size_t which) {
	std::string& value = fetch_str(mode, which);
	undo_list.add(action_ptr(new aEditClearString(edit_string_action_name("Clear", mode), mode, which, value, "*")));
	update_edit_menu();
	value = "*";
}

// Only for strings not tied to a location -- for creation and deletion
std::vector<std::string>& fetch_str_list(eStrMode str_mode){
	switch(str_mode){
		case 0: return scenario.spec_strs;
		case 1: return current_terrain->spec_strs;
		case 2: return town->spec_strs;
		case 3: return scenario.journal_strs;
		default: throw "Invalid string mode " + std::to_string(str_mode) + " (valid are 0-3)";
	}
}

static void delete_last_string(eStrMode mode) {
	auto& str_list = fetch_str_list(mode);
	std::string str = str_list.back();
	undo_list.add(action_ptr(new aDeleteString(edit_string_action_name("Delete", mode), mode, str)));
	update_edit_menu();
	str_list.pop_back();
}

static void create_many_strings(eStrMode mode) {
	auto& str_list = fetch_str_list(mode);
	std::vector<std::string> new_strings;
	for(int i = 0; i < 8; ++i){
		str_list.push_back("*");
		new_strings.push_back("*");
	}
	undo_list.add(action_ptr(new aCreateStrings(mode, new_strings)));
	update_edit_menu();
}

static void try_edit_string(eStrMode mode, size_t which, bool is_new) {
	auto& str_list = fetch_str_list(mode);
	if(is_new)
		str_list.emplace_back("*");
	if(!edit_text_str(which,mode,is_new) && is_new)
		str_list.pop_back();
}

static void edit_loc_string(eStrMode mode, size_t which) {
	bool is_new = false;
	edit_text_str(which,mode,is_new);
}

static bool handle_rb_action(location the_point, bool option_hit) {
	long right_top = right_sbar->getPosition();
	for(int i = 0; i < NRSONPAGE && i + right_top < NRS; i++)
		if(!mouse_button_held && (the_point.in(right_buttons[i]) )
		   && (right_button_status[i + right_top].action != RB_CLEAR))  {
			
			int j = right_button_status[i + right_top].i;
			//flash_rect(left_buttons[i][0]);
			draw_rb_slot(i + right_top,1);
			mainPtr().display();
			// TODO: Proper button handling
			play_sound(37);
			sf::sleep(time_in_ticks(10));
			draw_rb_slot(i + right_top,0);
			mainPtr().display();
			change_made = true;
			size_t size_before;
			size_t pos_before = right_sbar->getPosition();
			switch(right_button_status[i + right_top].action) {
				case RB_CLEAR:
					break;
				case RB_SCEN_SPEC:
					size_before = scenario.scen_specials.size();
					if(option_hit) {
						if(j == size_before - 1){
							undo_list.add(action_ptr(new aCreateDeleteSpecial(false, 0, scenario.scen_specials.back())));
							update_edit_menu();
							scenario.scen_specials.pop_back();
						}
						else if(j == size_before)
							break;
						else{
							undo_list.add(action_ptr(new aEditSpecial(0, j, scenario.scen_specials[j])));
							update_edit_menu();
							scenario.scen_specials[j] = cSpecial();
						}
					} else {
						bool is_new = false;
						if(j == size_before){
							scenario.scen_specials.emplace_back();
							is_new = true;
						}
						edit_spec_enc(j,0,nullptr,is_new);
					}
					start_special_editing(0);
					if(size_before > scenario.scen_specials.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_OUT_SPEC:
					size_before = current_terrain->specials.size();
					if(option_hit) {
						if(j == size_before - 1){
							undo_list.add(action_ptr(new aCreateDeleteSpecial(false, 1, current_terrain->specials.back())));
							update_edit_menu();
							current_terrain->specials.pop_back();
						}
						else if(j == size_before)
							break;
						else{
							undo_list.add(action_ptr(new aEditSpecial(1, j, current_terrain->specials[j])));
							update_edit_menu();
							current_terrain->specials[j] = cSpecial();
						}
					} else {
						bool is_new = false;
						if(j == size_before){
							current_terrain->specials.emplace_back();
							is_new = true;
						}
						edit_spec_enc(j,1,nullptr,is_new);
					}
					start_special_editing(1);
					if(size_before > current_terrain->specials.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_TOWN_SPEC:
					size_before = town->specials.size();
					if(option_hit) {
						if(j == size_before - 1){
							undo_list.add(action_ptr(new aCreateDeleteSpecial(false, 2, town->specials.back())));
							update_edit_menu();
							town->specials.pop_back();
						}
						else if(j == size_before)
							break;
						else{
							undo_list.add(action_ptr(new aEditSpecial(2, j, town->specials[j])));
							update_edit_menu();
							town->specials[j] = cSpecial();
						}
					} else {
						bool is_new = false;
						if(j == size_before){
							town->specials.emplace_back();
							is_new = true;
						}
						edit_spec_enc(j,2,nullptr,is_new);
					}
					start_special_editing(2);
					if(size_before > town->specials.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_SCEN_STR:
					size_before = scenario.spec_strs.size();
					if(option_hit) {
						if(j == size_before - 1)
							delete_last_string(STRS_SCEN);
						else if(j == size_before)
							create_many_strings(STRS_SCEN);
						else clear_string(STRS_SCEN, j);
					} else {
						try_edit_string(STRS_SCEN, j, j == size_before);
					}
					start_string_editing(STRS_SCEN);
					if(size_before > scenario.spec_strs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_OUT_STR:
					size_before = current_terrain->spec_strs.size();
					if(option_hit) {
						if(j == size_before - 1)
							delete_last_string(STRS_OUT);
						else if(j == size_before)
							create_many_strings(STRS_OUT);
						else clear_string(STRS_OUT, j);
					} else {
						try_edit_string(STRS_OUT, j, j == size_before);
					}
					start_string_editing(STRS_OUT);
					if(size_before > current_terrain->spec_strs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_TOWN_STR:
					size_before = town->spec_strs.size();
					if(option_hit) {
						if(j == size_before - 1)
							delete_last_string(STRS_TOWN);
						else if(j == size_before)
							create_many_strings(STRS_TOWN);
						else clear_string(STRS_TOWN, j);
					} else {
						try_edit_string(STRS_TOWN, j, j == size_before);
					}
					start_string_editing(STRS_TOWN);
					if(size_before > town->spec_strs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_SPEC_ITEM:
					size_before = scenario.special_items.size();
					if(option_hit) {
						// Delete last special item
						if(j == size_before - 1){
							undo_list.add(action_ptr(new aCreateDeleteSpecialItem(false, scenario.special_items.back())));
							update_edit_menu();
							scenario.special_items.pop_back();
						}
						else if(j == size_before)
							break;
						// Clear special item (it can't be deleted fully)
						else {
							cSpecItem cleared;
							cleared.name = "Unused Special Item";
							undo_list.add(action_ptr(new aEditClearSpecialItem("Clear Special Item", j, scenario.special_items[j], cleared)));
							update_edit_menu();
							scenario.special_items[j] = cleared;
						}
					} else {
						bool is_new = (j == size_before);
						if(edit_spec_item(j)){
							// Special item create/edit undo action is added in save_spec_item() because special item editor
							// has left/right buttons and may someday be launched with create/edit buttons elsewhere.
						}
						// Create new canceled
						else if(is_new){
							scenario.special_items.pop_back();
						}
					}
					if(size_before > scenario.special_items.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_JOURNAL:
					size_before = scenario.journal_strs.size();
					if(option_hit) {
						if(j == size_before - 1)
							delete_last_string(STRS_JOURNAL);
						else if(j == size_before)
							create_many_strings(STRS_JOURNAL);
						else clear_string(STRS_JOURNAL, j);
					} else {
						try_edit_string(STRS_JOURNAL, j, j == size_before);
					}
					start_string_editing(STRS_JOURNAL);
					if(size_before > scenario.journal_strs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_DIALOGUE:
					size_before = town->talking.talk_nodes.size();
					if(option_hit) {
						if(j == size_before)
							break;
						undo_list.add(action_ptr(new aCreateDeleteTalkNode(false, cur_town, j, town->talking.talk_nodes[j])));
						update_edit_menu();
						town->talking.talk_nodes.erase(town->talking.talk_nodes.begin() + j);
					} else {
						bool is_new = false;
						if(j == size_before){
							is_new = true;
							town->talking.talk_nodes.emplace_back();
						}
						cSpeech::cNode old_node = town->talking.talk_nodes[j];
						int canceled;
						if((canceled = edit_talk_node(j, is_new)) >= 0){
							// Cancel the last create new
							if(is_new)
								town->talking.talk_nodes.erase(town->talking.talk_nodes.begin() + canceled);
						}
						update_edit_menu();
					}
					start_dialogue_editing();
					if(size_before > town->talking.talk_nodes.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_PERSONALITY:
					edit_basic_dlog(j);
					start_dialogue_editing();
					break;
				case RB_OUT_SIGN:
					size_before = current_terrain->sign_locs.size();
					if(option_hit) {
						if(j == size_before - 1){
							undo_list.add(action_ptr(new aDeleteLocString(current_terrain, current_terrain->sign_locs.back())));
							update_edit_menu();
							current_terrain->sign_locs.pop_back();
						}
						else{
							undo_list.add(action_ptr(new aClearLocString(current_terrain, j, current_terrain->sign_locs[j], {-1, -1, "*"})));
							update_edit_menu();
							current_terrain->sign_locs[j] = {-1, -1, "*"};
						}
					} else {
						edit_loc_string(STRS_OUT_SIGN,j);
					}
					start_string_editing(STRS_OUT_SIGN);
					if(size_before > current_terrain->sign_locs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_TOWN_SIGN:
					size_before = town->sign_locs.size();
					if(option_hit) {
						if(j == size_before - 1){
							undo_list.add(action_ptr(new aDeleteLocString(town, town->sign_locs.back())));
							update_edit_menu();
							town->sign_locs.pop_back();
						}
						else{
							undo_list.add(action_ptr(new aClearLocString(town, j, town->sign_locs[j], {-1, -1, "*"})));
							update_edit_menu();
							town->sign_locs[j] = {-1, -1, "*"};
						}
					} else {
						edit_loc_string(STRS_TOWN_SIGN,j);
					}
					start_string_editing(STRS_TOWN_SIGN);
					if(size_before > town->sign_locs.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_OUT_RECT:
					size_before = current_terrain->area_desc.size();
					if(option_hit) {
						if(j == size_before - 1){
							undo_list.add(action_ptr(new aDeleteLocString(current_terrain, current_terrain->area_desc.back())));
							update_edit_menu();
							current_terrain->area_desc.pop_back();
						}
						else{
							undo_list.add(action_ptr(new aClearLocString(current_terrain, j, current_terrain->area_desc[j], {0, 0, 0, 0, "*"})));
							update_edit_menu();
							current_terrain->area_desc[j] = {0, 0, 0, 0, "*"};
						}
					} else {
						edit_loc_string(STRS_OUT_RECT,j);
					}
					start_string_editing(STRS_OUT_RECT);
					if(size_before > current_terrain->area_desc.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_TOWN_RECT:
					size_before = town->area_desc.size();
					if(option_hit) {
						if(j == size_before - 1){
							undo_list.add(action_ptr(new aDeleteLocString(town, town->area_desc.back())));
							update_edit_menu();
							town->area_desc.pop_back();
						}
						else{
							undo_list.add(action_ptr(new aClearLocString(town, j, town->area_desc[j], {0, 0, 0, 0, "*"})));
							update_edit_menu();
							town->area_desc[j] = {0, 0, 0, 0, "*"};
						}
					} else {
						edit_loc_string(STRS_TOWN_RECT,j);
					}
					start_string_editing(STRS_TOWN_RECT);
					if(size_before > town->area_desc.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_QUEST:
					size_before = scenario.quests.size();
					if(option_hit) {
						// Delete last quest
						if(j == scenario.quests.size() - 1){
							undo_list.add(action_ptr(new aCreateDeleteQuest(false, scenario.quests.back())));
							update_edit_menu();
							scenario.quests.pop_back();
						}
						// Clear quest (it can't be deleted fully)
						else {
							cQuest cleared;
							cleared.name = "Unused Quest";
							undo_list.add(action_ptr(new aEditClearQuest("Clear Quest", j, scenario.quests[j], cleared)));
							update_edit_menu();
							scenario.quests[j] = cleared;
						}
					} else {
						bool is_new = (j == scenario.quests.size());
						if(edit_quest(j)){
							// Quest create/edit undo action is added in save_quest_from_dlog() because quest editor
							// has left/right buttons and can be launched with create/edit buttons elsewhere.
						}
						// Create new canceled
						else if(is_new){
							scenario.quests.pop_back();
						}
					}
					if(size_before > scenario.quests.size())
						pos_before--;
					right_sbar->setPosition(pos_before);
					break;
				case RB_SHOP:
					size_before = scenario.shops.size();
					if(option_hit) {
						// Delete last shop
						if(j == scenario.shops.size() - 1){
							undo_list.add(action_ptr(new aCreateDeleteShop(false, scenario.shops.back())));
							update_edit_menu();
							scenario.shops.pop_back();
						}
						// Clear shop (it can't be fully deleted)
						else{
							cShop cleared("Unused Shop");
							undo_list.add(action_ptr(new aEditClearShop("Clear Shop", j, scenario.shops[j], cleared)));
							update_edit_menu();
							scenario.shops[j] = cleared;
						}
					} else {
						bool is_new = (j == scenario.shops.size());
						if(edit_shop(j)){
							// Shop create/edit undo action is added in save_shop_from_dlog() because shop editor
							// has left/right buttons and can be launched with create/edit buttons elsewhere.
						}
						// Create new canceled
						else if(is_new){
							scenario.shops.pop_back();
						}
					}
					start_shops_editing();
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

stroke_ter_changes_t current_stroke_changes;
std::string current_stroke_type;

item_changes_t current_items_placed;
creature_changes_t current_creatures_placed;

clear_field_stroke_t current_fields_cleared;
field_stroke_t current_fields_placed;
field_stroke_t current_fields_toggled;
eFieldType current_field_type;

void commit_stroke() {
	if(!current_stroke_changes.empty()){
		undo_list.add(action_ptr(new aDrawTerrain("Draw Terrain (" + current_stroke_type + ")", current_stroke_changes)));
		update_edit_menu();
		current_stroke_changes.clear();
		current_stroke_type = "";
	}else if(!current_items_placed.empty()){
		undo_list.add(action_ptr(new aPlaceEraseItem(current_items_placed.size() > 1 ? "Place Items" : "Place Item", true, current_items_placed)));
		update_edit_menu();
		current_items_placed.clear();
	}else if(!current_creatures_placed.empty()){
		undo_list.add(action_ptr(new aPlaceEraseCreature(current_creatures_placed.size() > 1 ? "Place Creatures" : "Place Creature", true, current_creatures_placed)));
		update_edit_menu();
		current_items_placed.clear();
	}else if(!current_fields_cleared.empty()){
		undo_list.add(action_ptr(new aClearFields(current_fields_cleared)));
		update_edit_menu();
		current_fields_cleared.clear();
	}else if(!current_fields_placed.empty()){
		undo_list.add(action_ptr(new aPlaceFields(current_field_type, current_fields_placed)));
		update_edit_menu();
		current_fields_placed.clear();
	}else if(!current_fields_toggled.empty()){
		undo_list.add(action_ptr(new aToggleOutFields(current_field_type == SPECIAL_ROAD, mode_count, current_fields_toggled)));
		update_edit_menu();
		current_fields_toggled.clear();
	}
}

const std::vector<std::string> entrance_names = {"North", "East", "South", "West"};

static bool handle_terrain_action(location the_point, bool ctrl_hit) {
	cArea* cur_area = get_current_area();
	std::shared_ptr<cAction> undo_action = nullptr;
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
		
		if(!cur_area->is_on_map(spot_hit)) ;
		else switch(overall_mode) {
			case MODE_DRAWING:
				if(!mouse_button_held) {
					erasing_mode = terrain_matches(spot_hit.x, spot_hit.y, current_terrain_type);
					mouse_button_held = true;
				}
				if(erasing_mode){
					stroke_ter_changes_t changes;
					set_terrain(spot_hit,current_ground,changes);
					undo_action.reset(new aDrawTerrain("Erase Terrain", changes));
				}
				else{
					// This could be an ongoing stroke
					current_stroke_type = "Pencil";
					set_terrain(spot_hit,current_terrain_type,current_stroke_changes);
				}
				break;
				
			case MODE_ROOM_RECT: case MODE_SET_TOWN_RECT: case MODE_HOLLOW_RECT: case MODE_FILLED_RECT:
			case MODE_STORAGE_RECT: case MODE_COPY_TERRAIN:
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
					undo_list.add(action_ptr(new aEditTownBounds(false, town->in_town_rect, working_rect)));
					update_edit_menu();
					town->in_town_rect = working_rect;
					change_made = true;
				}
				else if(overall_mode == MODE_ROOM_RECT) {
					auto& area_descs = cur_area->area_desc;
					auto iter = std::find_if(area_descs.begin(), area_descs.end(), [](const info_rect_t& r) {
						return r.right == 0;
					});
					if(iter != area_descs.end()) {
						static_cast<rectangle&>(*iter) = working_rect;
						iter->descr = "";
						if(!edit_area_rect_str(*iter))
							iter->right = 0;
						else{
							undo_list.add(action_ptr(new aCreateAreaRect(iter - area_descs.begin(), *iter)));
							update_edit_menu();
						}
					} else {
						area_descs.emplace_back(working_rect);
						if(!edit_area_rect_str(area_descs.back()))
							area_descs.pop_back();
						else{
							undo_list.add(action_ptr(new aCreateAreaRect(area_descs.size() - 1, area_descs.back())));
							update_edit_menu();
						}
					}
					change_made = true;
				}
				else if(overall_mode == MODE_STORAGE_RECT) {
					undo_list.add(action_ptr(new aEditTownBounds(true, scenario.store_item_rects[cur_town], working_rect)));
					update_edit_menu();
					scenario.store_item_rects[cur_town] = working_rect;
					change_made = true;
				}
				else if(overall_mode == MODE_COPY_TERRAIN) {
					vector2d<ter_num_t> copied;
					copied.resize(working_rect.width() + 1, working_rect.height() + 1);
					for(int i = 0; i <= working_rect.width(); i++) {
						for(int j = 0; j <= working_rect.height(); j++) {
							if(editing_town) copied[i][j] = town->terrain(i + working_rect.left, j + working_rect.top);
							else copied[i][j] = current_terrain->terrain(i + working_rect.left, j + working_rect.top);
						}
					}
					clipboard = copied;
				}
				overall_mode = MODE_DRAWING;
				break;
			// Deprecated:
			case MODE_SET_WANDER_POINTS:
				if(mouse_button_held)
					break;
				if(editing_town) {
					town->wandering_locs[mode_count - 1].x = spot_hit.x;
					town->wandering_locs[mode_count - 1].y = spot_hit.y;
				} else {
					current_terrain->wandering_locs[mode_count - 1].x = spot_hit.x;
					current_terrain->wandering_locs[mode_count - 1].y = spot_hit.y;
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
				current_stroke_type = "Lg. Paintbrush";
				change_circle_terrain(spot_hit,4,current_terrain_type,20);
				break;
			case MODE_SMALL_PAINTBRUSH:
				mouse_button_held = true;
				current_stroke_type = "Sm. Paintbrush";
				change_circle_terrain(spot_hit,1,current_terrain_type,20);
				break;
			case MODE_LARGE_SPRAYCAN:
				mouse_button_held = true;
				current_stroke_type = "Lg. Spraycan";
				change_circle_terrain(spot_hit,4,current_terrain_type,1);
				break;
			case MODE_SMALL_SPRAYCAN:
				mouse_button_held = true;
				current_stroke_type = "Sm. Spraycan";
				change_circle_terrain(spot_hit,2,current_terrain_type,1);
				break;
			case MODE_ERASER: // erase
				change_circle_terrain(spot_hit,2,current_ground,20);
				current_stroke_type = "Eraser";
				mouse_button_held = true;
				break;
			case MODE_FLOOD_FILL:
				if(mouse_button_held) break;
				flood_fill_terrain(spot_hit, current_terrain_type);
				mouse_button_held = true;
				break;
			case MODE_PLACE_ITEM:
				// If we just placed this item there, forget it
				if(!mouse_button_held || last_placement != spot_hit) {
					mouse_button_held = true;
					place_item(spot_hit, mode_count, false, false, 100, current_items_placed);
					last_placement = spot_hit;
				}
				break;
			case MODE_EDIT_ITEM:
				for(short x = 0; x < town->preset_items.size(); x++){
					cTown::cItem old_item = town->preset_items[x];
					if((spot_hit.x == old_item.loc.x) &&
					   (spot_hit.y == old_item.loc.y) && (old_item.code >= 0)) {
						edit_placed_item(x);
						if(town->preset_items[x] != old_item){
							undo_list.add(action_ptr(new aEditPlacedItem(x, old_item, town->preset_items[x])));
							update_edit_menu();
						}
					}
				}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PASTE:
				if(auto spec = boost::get<std::pair<long,bool>>(&clipboard)) {
					if(!editing_town && (spot_hit.x == 0 || spot_hit.x == 47 || spot_hit.y == 0 || spot_hit.y == 47)) {
						cChoiceDlog("not-at-edge").show();
						break;
					} else {
						auto& specials = cur_area->special_locs;
						for(short x = 0; x <= specials.size(); x++) {
							if(x == specials.size())
								specials.emplace_back(-1,-1,-1);
							if(specials[x].spec < 0) {
								specials[x] = spot_hit;
								specials[x].spec = spec->first;
								undo_list.add(action_ptr(new aPlaceEraseSpecial("Paste Special Encounter", true, specials[x])));
								update_edit_menu();
								break;
							}
						}
					}
				} else if(auto monst = boost::get<cTownperson>(&clipboard)) {
					if(!editing_town) {
						set_string("Paste monster","Not while outdoors.");
						break;
					}
					place_creature(spot_hit, static_cast<mon_num_t>(mode_count), current_creatures_placed);
					undo_list.add(action_ptr(new aPlaceEraseCreature("Paste Creature", true, current_creatures_placed)));
					update_edit_menu();
					current_creatures_placed.clear();
				} else if(auto item = boost::get<cTown::cItem>(&clipboard)) {
					if(!editing_town) {
						set_string("Paste item","Not while outdoors.");
						break;
					}
					place_item(spot_hit, mode_count, false, false, 100, current_items_placed);
					undo_list.add(action_ptr(new aPlaceEraseItem("Paste Item", true, current_items_placed)));
					update_edit_menu();
					current_items_placed.clear();
				} else if(auto patch = boost::get<vector2d<ter_num_t>>(&clipboard)) {
					for(int x = 0; x < patch->width(); x++)
						for(int y = 0; y < patch->height(); y++)
							set_terrain(loc(spot_hit.x + x, spot_hit.y + y), (*patch)[x][y], current_stroke_changes, false);
					if(!current_stroke_changes.empty()){
						undo_list.add(action_ptr(new aDrawTerrain("Paste Terrain", current_stroke_changes)));
						update_edit_menu();
						current_stroke_changes.clear();
					}
				} else {
					showError("Nothing to paste. Try copying something first.");
				}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_CREATURE:
				// If we just placed this same creature here, forget it
				if(!mouse_button_held || last_placement != spot_hit) {
					mouse_button_held = true;
					place_creature(spot_hit, static_cast<mon_num_t>(mode_count), current_creatures_placed);
					last_placement = spot_hit;
				}
				break;
				
			case MODE_PLACE_NORTH_ENTRANCE: case MODE_PLACE_EAST_ENTRANCE:
			case MODE_PLACE_SOUTH_ENTRANCE: case MODE_PLACE_WEST_ENTRANCE:
				undo_list.add(action_ptr(new aPlaceTownEntrance(
					std::string {"Place "} + entrance_names[overall_mode - 10] + " Entrance",
					overall_mode - 10, town->start_locs[overall_mode - 10], spot_hit)));
				update_edit_menu();
				town->start_locs[overall_mode - 10].x = spot_hit.x;
				town->start_locs[overall_mode - 10].y = spot_hit.y;
				overall_mode = MODE_DRAWING;
				break;
			case MODE_PLACE_WEB:
				current_field_type = FIELD_WEB;
				make_field_type(spot_hit.x,spot_hit.y,FIELD_WEB,current_fields_placed);
				mouse_button_held = true;
				break;
			case MODE_PLACE_CRATE:
				current_field_type = OBJECT_CRATE;
				make_field_type(spot_hit.x,spot_hit.y,OBJECT_CRATE,current_fields_placed);
				mouse_button_held = true;
				break;
			case MODE_PLACE_BARREL:
				current_field_type = OBJECT_BARREL;
				make_field_type(spot_hit.x,spot_hit.y,OBJECT_BARREL,current_fields_placed);
				mouse_button_held = true;
				break;
			case MODE_PLACE_FIRE_BARRIER:
				current_field_type = BARRIER_FIRE;
				make_field_type(spot_hit.x,spot_hit.y,BARRIER_FIRE,current_fields_placed);
				mouse_button_held = true;
				break;
			case MODE_PLACE_FORCE_BARRIER:
				current_field_type = BARRIER_FORCE;
				make_field_type(spot_hit.x,spot_hit.y,BARRIER_FORCE,current_fields_placed);
				mouse_button_held = true;
				break;
			case MODE_PLACE_QUICKFIRE:
				current_field_type = FIELD_QUICKFIRE;
				make_field_type(spot_hit.x,spot_hit.y,FIELD_QUICKFIRE,current_fields_placed);
				mouse_button_held = true;
				break;
			case MODE_PLACE_STONE_BLOCK:
				current_field_type = OBJECT_BLOCK;
				make_field_type(spot_hit.x,spot_hit.y,OBJECT_BLOCK,current_fields_placed);
				mouse_button_held = true;
				break;
			case MODE_PLACE_FORCECAGE:
				current_field_type = BARRIER_CAGE;
				make_field_type(spot_hit.x,spot_hit.y,BARRIER_CAGE,current_fields_placed);
				mouse_button_held = true;
				break;
			case MODE_TOGGLE_SPECIAL_DOT:
				current_field_type = SPECIAL_SPOT;
				if(editing_town){
					make_field_type(spot_hit.x, spot_hit.y, SPECIAL_SPOT,current_fields_placed);
					mouse_button_held = true;
				} else {
					if(!mouse_button_held)
						mode_count = !current_terrain->special_spot[spot_hit.x][spot_hit.y];

					current_fields_toggled.insert(spot_hit);

					current_terrain->special_spot[spot_hit.x][spot_hit.y] = mode_count;
					mouse_button_held = true;
				}
				break;
			case MODE_TOGGLE_ROAD:
				current_field_type = SPECIAL_ROAD;
				if(editing_town){
					make_field_type(spot_hit.x, spot_hit.y, SPECIAL_ROAD, current_fields_placed);
					mouse_button_held = true;
				} else {
					if(!mouse_button_held)
						mode_count = !current_terrain->roads[spot_hit.x][spot_hit.y];

					current_fields_toggled.insert(spot_hit);

					current_terrain->roads[spot_hit.x][spot_hit.y] = mode_count;
					mouse_button_held = true;
				}
				break;
			case MODE_CLEAR_FIELDS:
				for(int i = 8; i <= SPECIAL_ROAD; i++)
					take_field_type(spot_hit.x,spot_hit.y, eFieldType(i), current_fields_cleared);
				mouse_button_held = true;
				break;
			case MODE_PLACE_SFX:
				current_field_type = eFieldType(SFX_SMALL_BLOOD + mode_count);
				make_field_type(spot_hit.x,spot_hit.y, current_field_type, current_fields_placed);
				mouse_button_held = true;
				break;
			case MODE_EYEDROPPER:
				set_new_terrain(cur_area->terrain(spot_hit.x,spot_hit.y));
				overall_mode = MODE_DRAWING;
				break;
			case MODE_EDIT_SIGN: //edit sign
			{
				auto& signs = cur_area->sign_locs;
				auto iter = std::find(signs.begin(), signs.end(), spot_hit);
				const cTerrain& terrain = scenario.ter_types[cur_area->terrain(spot_hit.x,spot_hit.y)];
				if(terrain.special != eTerSpec::IS_A_SIGN){
					showWarning("This is not a sign.");
					return true;
				}
				short picture = terrain.picture;
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
				for(short x = 0; x < town->creatures.size(); x++){
					if(monst_on_space(spot_hit,x)) {
						cTownperson old_creature = town->creatures[x];
						edit_placed_monst(x);
						if(town->creatures[x] != old_creature){
							undo_list.add(action_ptr(new aEditPlacedCreature(x, old_creature, town->creatures[x])));
							update_edit_menu();
						}
					}
				}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_EDIT_SPECIAL: //make special
				place_edit_special(spot_hit);
				overall_mode = MODE_DRAWING;
				break;
			case MODE_COPY_SPECIAL: //copy special
			{
				auto& specials = cur_area->special_locs;
				auto iter = std::find_if(town->special_locs.begin(), town->special_locs.end(), [](const spec_loc_t& loc) {
					return loc == spot_hit && loc.spec >= 0;
				});
				if(iter != specials.end())
					clipboard = std::pair<long,bool>{iter->spec, editing_town};
				else showError("There wasn't a special on that spot.");
				overall_mode = MODE_DRAWING;
				break;
			}
			case MODE_ERASE_SPECIAL: //erase special
			{
				auto& specials = cur_area->special_locs;
				for(short x = 0; x < specials.size(); x++)
					if(specials[x] == spot_hit && specials[x].spec >= 0) {
						spec_loc_t for_redo = specials[x];
						specials[x] = {-1,-1};
						specials[x].spec = -1;
						if(x == specials.size() - 1) {
							// Delete not only the last entry but any other empty entries at the end of the list
							do {
								specials.pop_back();
							} while(!specials.empty() && specials.back().spec < 0);
						}
						undo_action.reset(new aPlaceEraseSpecial("Erase Special", false, for_redo));
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
			case MODE_SET_OUT_START:{
				if((spot_hit.x != minmax(4,43,spot_hit.x)) || (spot_hit.y != minmax(4,43,spot_hit.y))) {
					showError("You can't put the starting location this close to the edge of an outdoor section. It has to be at least 4 spaces away.");
					break;
				}
				area_ref_t old_start = { false, 0, scenario.out_sec_start, scenario.out_start };
				scenario.out_sec_start.x = cur_out.x;
				scenario.out_sec_start.y = cur_out.y;
				scenario.out_start = spot_hit;
				area_ref_t new_start = { false, 0, scenario.out_sec_start, scenario.out_start };
				undo_list.add(action_ptr(new aPlaceStartLocation(old_start, new_start)));
				update_edit_menu();
				overall_mode = MODE_DRAWING;
				change_made = true;
			}break;
			case MODE_ERASE_CREATURE: //delete monst
				for(short x = 0; x < town->creatures.size(); x++)
					if(monst_on_space(spot_hit,x)) {
						undo_list.add(action_ptr(new aPlaceEraseCreature("Erase Creature", false, x, town->creatures[x])));
						update_edit_menu();
						town->creatures[x].number = 0;
						break;
					}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_ERASE_ITEM: // delete item
				for(short x = 0; x < town->preset_items.size(); x++)
					if((spot_hit.x == town->preset_items[x].loc.x) &&
					   (spot_hit.y == town->preset_items[x].loc.y) && (town->preset_items[x].code >= 0)) {

						undo_list.add(action_ptr(new aPlaceEraseItem("Erase Item", false, x, town->preset_items[x])));
						update_edit_menu();
						town->preset_items[x].code = -1;
						break;
					}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_SET_TOWN_START:{
				if(!town->in_town_rect.contains(spot_hit)) {
					showError("You can't put the starting location outside the town boundaries.");
					break;
				}
				area_ref_t old_start = { true, scenario.which_town_start, {}, scenario.where_start };
				scenario.which_town_start = cur_town;
				scenario.where_start = spot_hit;
				area_ref_t new_start = { true, scenario.which_town_start, {}, scenario.where_start };
				undo_list.add(action_ptr(new aPlaceStartLocation(old_start, new_start)));
				update_edit_menu();
				overall_mode = MODE_DRAWING;
				change_made = true;
			}break;
			case MODE_PLACE_BOAT: case MODE_PLACE_HORSE: {
				bool is_new = false;
				cVehicle old_vehicle;
				auto& all = overall_mode == MODE_PLACE_BOAT ? scenario.boats : scenario.horses;
				auto iter = std::find_if(all.begin(), all.end(), [](const cVehicle& what) {
					if(editing_town && cur_town != what.which_town) return false;
					else if(!editing_town && what.sector != cur_out) return false;
					return what.loc == spot_hit;
				});
				// Edit existing
				if(iter != all.end()){
					old_vehicle = *iter;
				}
				// Create new
				else{
					iter = std::find_if(all.begin(), all.end(), [](const cVehicle& what) {
						return what.which_town < 0;
					});
					if(iter == all.end()) {
						all.emplace_back();
						iter = all.end() - 1;
					}
					is_new = true;
					iter->loc = spot_hit;
					iter->which_town = editing_town ? cur_town : 200;
					iter->property = false;
					iter->exists = false;
					if(!editing_town) iter->sector = cur_out;
				}
				if(edit_vehicle(*iter, iter - all.begin(), overall_mode == MODE_PLACE_BOAT)){
					// Created new
					if(is_new){
						undo_list.add(action_ptr(new aPlaceEraseVehicle(true, overall_mode == MODE_PLACE_BOAT, iter - all.begin(), *iter)));
						update_edit_menu();
					}
					// Edited
					else if(old_vehicle != *iter){
						undo_list.add(action_ptr(new aEditVehicle(overall_mode == MODE_PLACE_BOAT, iter - all.begin(), old_vehicle, *iter)));
						update_edit_menu();
					}
				}
				else{
					// Edit existing--delete chosen
					if(!is_new){
						undo_list.add(action_ptr(new aPlaceEraseVehicle(false, overall_mode == MODE_PLACE_BOAT, iter - all.begin(), *iter)));
						update_edit_menu();
					}
					// Create new canceled or delete chosen
					else{
						// Nothing needs to be recorded
					}
					// Vehicle can be deleted completely without offseting other vehicle numbers
					if(iter == (all.end() - 1)){
						all.erase(iter);
					}
					// Vehicle can't be fully deleted
					else{
						*iter = cVehicle();
					}
				}
				overall_mode = MODE_DRAWING;
				break;
			}
			case MODE_INTRO_SCREEN:
			case MODE_EDIT_TYPES:
			case MODE_MAIN_SCREEN:
			case MODE_EDIT_SPECIALS:
			case MODE_EDIT_SPECIAL_ITEMS:
			case MODE_EDIT_QUESTS:
			case MODE_EDIT_SHOPS:
			case MODE_EDIT_STRINGS:
			case MODE_EDIT_DIALOGUE:
				break; // Nothing to do here, of course.
			case MODE_COPY_CREATURE:
				for(short x = 0; x < town->creatures.size(); x++)
					if(monst_on_space(spot_hit,x)) {
						clipboard = town->creatures[x];
						break;
					}
				overall_mode = MODE_DRAWING;
				break;
			case MODE_COPY_ITEM:
				for(short x = 0; x < town->preset_items.size(); x++)
					if((spot_hit.x == town->preset_items[x].loc.x) && (spot_hit.y == town->preset_items[x].loc.y) && (town->preset_items[x].code >= 0)) {
						clipboard = town->preset_items[x];
						break;
					}
				overall_mode = MODE_DRAWING;
				break;
		}
		if((overall_mode == MODE_DRAWING) && (old_mode != MODE_DRAWING))
			set_string("Drawing mode",scenario.ter_types[current_terrain_type].name);
		draw_terrain();

		if(undo_action != nullptr){
			undo_list.add(undo_action);
			update_edit_menu();
		}
		return true;
	}
	bool need_redraw = false;
	if((the_point.in(border_rect[0]))) {
		if(ctrl_hit)
			cen_y = ((editing_town) ? 4 : 3);
		else
			handle_editor_screen_shift(0, -1);
		need_redraw = true;
		mouse_button_held = true;
	}
	if((the_point.in(border_rect[1]))) {
		if(ctrl_hit)
			cen_x = ((editing_town) ? 4 : 3);
		else
			handle_editor_screen_shift(-1, 0);
		need_redraw = true;
		mouse_button_held = true;
	}
	auto max_dim = cur_area->max_dim - 5;
	// This allows you to see a strip of terrain from the adjacent sector when editing outdoors
	if(!editing_town) max_dim++;
	if((the_point.in(border_rect[2]))) {
		if(ctrl_hit)
			cen_y = max_dim;
		else
			handle_editor_screen_shift(0, 1);
		need_redraw = true;
		mouse_button_held = true;
	}
	if((the_point.in(border_rect[3]))) {
		if(ctrl_hit)
			cen_x = max_dim;
		else
			handle_editor_screen_shift(1, 0);
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
	int rows = TYPE_ROWS_DRAWING;
	if(overall_mode == MODE_EDIT_TYPES) rows = TYPE_ROWS_EDITING;
	for(int i = 0; i < 16 * rows; i++)
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
			// MODE_EDIT_TYPES:
			else {
				short pos_before = pos_before = pal_sbar->getPosition();
				i += pos_before * 16;
				short size_before = scenario.ter_types.size();
				if(draw_mode == DRAW_MONST) size_before = scenario.scen_monsters.size(), i++;
				else if(draw_mode == DRAW_ITEM) size_before = scenario.scen_items.size();
				if(i > size_before) return true;

				if(draw_mode == DRAW_TERRAIN && i == 90){
					showWarning("The pit/barrier terrain cannot be changed.");
					return true;
				}
				if(option_hit) {
					// option-click the plus button: create a new row of types
					if(i == size_before){
						switch(draw_mode){
							case DRAW_TERRAIN:{
								terrain_type_changes_t terrains;
								scenario.ter_types.resize(size_before + 16);
								for(; i < scenario.ter_types.size(); i++){
									scenario.ter_types[i].name = "New Terrain";
									terrains.push_back(scenario.ter_types[i]);
								}

								undo_list.add(action_ptr(new aCreateDeleteTerrain(terrains)));
								update_edit_menu();
							}break;
							case DRAW_MONST:{
								monst_type_changes_t monsts;
								scenario.scen_monsters.resize(size_before + 16);
								for(; i < scenario.scen_monsters.size(); i++){
									scenario.scen_monsters[i].m_name = "New Monster";
									monsts.push_back(scenario.scen_monsters[i]);
								}

								undo_list.add(action_ptr(new aCreateDeleteMonster(monsts)));
								update_edit_menu();
							}break;
							case DRAW_ITEM:{
								item_type_changes_t items;
								scenario.scen_items.resize(size_before + 16);
								for(; i < scenario.scen_items.size(); i++){
									scenario.scen_items[i].full_name = "New Item";
									items.push_back(scenario.scen_items[i]);
								}

								undo_list.add(action_ptr(new aCreateDeleteItem(items)));
								update_edit_menu();
							}break;
						}
					}
					else{
						switch(draw_mode){
							case DRAW_TERRAIN:
								// option-click the last one and it's safe to delete: delete it
								if(i == size_before - 1 && !scenario.is_ter_used(i)){
									undo_list.add(action_ptr(new aCreateDeleteTerrain(false, scenario.ter_types.back())));
									update_edit_menu();
									scenario.ter_types.pop_back();
								}
								// option-click type that can't be deleted (because it would break other types' numbers, or is in use somewhere):
								// reset type info
								else{
									cTerrain before = scenario.ter_types[i];
									cTerrain after;
									after.name = "Unused Terrain";
									scenario.ter_types[i] = after;
									undo_list.add(action_ptr(new aEditClearTerrain("Clear Terrain Type", i, before, after)));
									update_edit_menu();
								}
								break;
							case DRAW_MONST:
								// option-click the last one and it's safe to delete: delete it
								if(i == size_before - 1 && !scenario.is_monst_used(i)){
									undo_list.add(action_ptr(new aCreateDeleteMonster(false, scenario.scen_monsters.back())));
									update_edit_menu();
									scenario.scen_monsters.pop_back();
								}
								// option-click type that can't be deleted (because it would break other types' numbers, or is in use somewhere):
								// reset type info
								else{
									cMonster before = scenario.scen_monsters[i];
									cMonster after;
									after.m_name = "Unused Monster";
									scenario.scen_monsters[i] = after;
									undo_list.add(action_ptr(new aEditClearMonster("Clear Monster Type", i, before, after)));
									update_edit_menu();
								}
								break;
							case DRAW_ITEM:
								// option-click the last one and it's safe to delete: delete it
								if(i == size_before - 1 && !scenario.is_item_used(i)){
									undo_list.add(action_ptr(new aCreateDeleteItem(false, scenario.scen_items.back())));
									update_edit_menu();
									scenario.scen_items.pop_back();
								}
								// option-click type that can't be deleted (because it would break other types' numbers, or is in use somewhere):
								// reset type info
								else{
									cItem before = scenario.scen_items[i];
									cItem after;
									after.full_name = "Unused Item";
									scenario.scen_items[i] = after;
									undo_list.add(action_ptr(new aEditClearItem("Clear Item Type", i, before, after)));
									update_edit_menu();
								}
								break;
						}
					}
				} else {
					bool is_new = false;
					// Click the plus button: create a new type and edit it immediately
					if(i == size_before) {
						is_new = true;
						switch(draw_mode){
							case DRAW_TERRAIN:
								scenario.ter_types.emplace_back();
								scenario.ter_types.back().name = "New Terrain";
								break;
							case DRAW_MONST:
								scenario.scen_monsters.emplace_back();
								scenario.scen_monsters.back().m_name = "New Monster";
								break;
							case DRAW_ITEM:
								scenario.scen_items.emplace_back();
								scenario.scen_items.back().full_name = "New Item";
								break;
						}
					}
					switch(draw_mode){
						case DRAW_TERRAIN:
							if(!edit_ter_type(i, is_new)){
								// Canceled creating new:
								if(is_new){
									scenario.ter_types.pop_back();
								}
							}
							// Editing/creating terrain records its own undo actions because the
							// left/right buttons make it complicated.
							break;
						case DRAW_MONST:
							if(!edit_monst_type(i, is_new)){
								// Canceled creating new:
								if(is_new){
									scenario.scen_monsters.pop_back();
								}
							}
							// Editing/creating monster records its own undo actions because the
							// left/right buttons make it complicated.
							break;
						case DRAW_ITEM:
							if(!edit_item_type(i, is_new)){
								// Canceled creating new:
								if(is_new){
									scenario.scen_items.pop_back();
								}
							}
							// Editing/creating item records its own undo actions because the
							// left/right buttons make it complicated.
							break;
					}
				}
				short size_after = scenario.ter_types.size();
				if(draw_mode == DRAW_MONST) size_after = scenario.scen_monsters.size(), i++;
				else if(draw_mode == DRAW_ITEM) size_after = scenario.scen_items.size();
				pos_before += (size_after / 16 - size_before / 16);
				pal_sbar->setMaximum(max(pos_before, pal_sbar->getMaximum()));
				pal_sbar->setPosition(pos_before);
				set_up_type_buttons(false);
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
			// cur_palette_buttons uses [j][i] and palette_buttons uses [i][j] -- this is not a mistake,
			// just unfortunate.
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
						set_string("Fill rectangle (hollow)","Select upper left corner");
						if(false) // Skip next statement
					case PAL_RECT_FILLED:
							overall_mode = MODE_FILLED_RECT,
							set_string("Fill rectangle (solid)","Select upper left corner");
						mode_count = 2;
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
					case PAL_EDIT_STORAGE:
						overall_mode = MODE_STORAGE_RECT;
						mode_count = 2;
						set_string("Create saved item rect","Select upper left corner");
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
					case PAL_EDIT_ITEM:
						if(!editing_town) {
							set_string("Edit placed item","Not while outdoors.");
							break;
						}
						set_string("Edit placed item","Select item to edit");
						overall_mode = MODE_EDIT_ITEM;
						break;
					case PAL_COPY_ITEM:
						if(!editing_town) {
							set_string("Copy item","Not while outdoors.");
							break;
						}
						set_string("Copy item","Select item");
						overall_mode = MODE_COPY_ITEM;
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
					case PAL_PASTE:
						if(auto spec = boost::get<std::pair<long,bool>>(&clipboard))
							if(editing_town == spec->second) set_string("Paste special","Select location to paste");
							else {
								if(editing_town) set_string("Paste special","Not while in town");
								else set_string("Paste special","Not while outdoors");
								break;
							}
						else if(boost::get<cTownperson>(&clipboard))
							if(editing_town) set_string("Paste monster","Select location to paste");
							else {
								set_string("Paste monster","Not while outdoors.");
								break;
							}
						else if(boost::get<cTown::cItem>(&clipboard))
							if(editing_town) set_string("Paste item","Select location to paste");
							else {
								set_string("Paste item","Not while outdoors.");
								break;
							}
						else if(boost::get<vector2d<ter_num_t>>(&clipboard))
							set_string("Paste terrain","Select location to paste");
						else set_string("Can't paste","Nothing to paste");
						overall_mode = MODE_PASTE;
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
					case PAL_COPY_MONST:
						set_string("Copy creature","Select creature");
						overall_mode = MODE_COPY_CREATURE;
						break;
					case PAL_ERASE_MONST:
						set_string("Delete a creature","Select creature");
						overall_mode = MODE_ERASE_CREATURE;
						break;
					case PAL_ENTER_N:
						set_string("Place north entrance","Select entrance location");
						overall_mode = MODE_PLACE_NORTH_ENTRANCE;
						break;
					case PAL_ENTER_W:
						set_string("Place west entrance","Select entrance location");
						overall_mode = MODE_PLACE_WEST_ENTRANCE;
						break;
					case PAL_ENTER_S:
						set_string("Place south entrance","Select entrance location");
						overall_mode = MODE_PLACE_SOUTH_ENTRANCE;
						break;
					case PAL_ENTER_E:
						set_string("Place east entrance","Select entrance location");
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
						set_up_type_buttons(true);
						break;
					case PAL_ITEM: // Item palette
						draw_mode = DRAW_ITEM;
						set_up_type_buttons(true);
						break;
					case PAL_MONST: // Monster palette
						draw_mode = DRAW_MONST;
						set_up_type_buttons(true);
						break;
					case PAL_BOAT:
						set_string("Place/edit boat","Select boat location");
						overall_mode = MODE_PLACE_BOAT;
						break;
					case PAL_HORSE:
						set_string("Place/edit horse","Select horse location");
						overall_mode = MODE_PLACE_HORSE;
						break;
					case PAL_TOWN_BORDER:
						if(!editing_town) {
							set_string("Place boundaries","Not while outdoors.");
							break;
						}
						overall_mode = MODE_SET_TOWN_RECT;
						mode_count = 2;
						set_string("Set town boundary","Select upper left corner");
						break;
					case PAL_START:
						if(editing_town) {
							overall_mode = MODE_SET_TOWN_START;
							set_string("Select party starting location.","");
						} else {
							overall_mode = MODE_SET_OUT_START;
							set_string("Select party starting location.","");
						}
						break;
					case PAL_COPY_TER:
						set_string("Copy terrain", "Select upper left corner");
						overall_mode = MODE_COPY_TERRAIN;
						mode_count = 2;
						break;
				}
				return true;
			}
		}
	return false;
}

void handle_action(location the_point,sf::Event /*event*/) {
	std::string s2;
	
	bool option_hit = false,ctrl_hit = false;
	location spot_hit;
	location cur_point,cur_point2;
	rectangle temp_rect;
	if(kb.isAltPressed())
		option_hit = true;
	if(kb.isCtrlPressed())
		ctrl_hit = true;
	
	if(handle_lb_click(the_point))
		return;
	
	if(overall_mode >= MODE_MAIN_SCREEN && overall_mode != MODE_EDIT_TYPES && handle_rb_action(the_point, option_hit))
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
	
	// Prompt the designer for which terrain to change and how likely:
	if(!change_ter(a,b,c)) return;

	// Make the changes:
	cArea* cur_area = get_current_area();
	stroke_ter_changes_t changes;
	
	for(short i = 0; i < cur_area->max_dim; i++)
		for(short j = 0; j < cur_area->max_dim; j++) {
			ter = cur_area->terrain(i,j);
			if((ter == a) && (get_ran(1,1,100) <= c)) {
				set_terrain(loc(i,j), b, changes);
			}
		}
	

	undo_list.add(action_ptr(new aDrawTerrain("Change Terrain Randomly", changes)));
	update_edit_menu();
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
	
	// Shortcuts while terrain is visible:

	for(short i = 0; i < 10; i++)
		if(chr2 == keypad[i] || (i % 2 == 0 && i > 0 && chr2 == arrows[i / 2 - 1])) {
			if(i == 0) {
				chr = 'z';
				(void) chr; // TODO: This seems like it was intended to make Numpad0 and Z equivalent.
			}
			else {
				pass_point = terrain_click[i];
				handle_action(pass_point,event);
				draw_terrain();
				mouse_button_held = false;
				return;
			}
		}

	if(event.key.code == Key::Escape){
		if(overall_mode == MODE_DRAWING){
			// Not doing anything special. back to menu
			handle_lb_action(NLS - 2);
		}else{
			// Using a tool. Turn it off
			set_string("Drawing mode",scenario.ter_types[current_terrain_type].name);
			overall_mode = MODE_DRAWING;
		}
		return;
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
		case ',':
			set_string("Edit creature","Select creature to edit");
			overall_mode = MODE_EDIT_CREATURE;
			break;
			
		case '+': case '=': // accept + with or without shift held for symmetry with -
			cur_viewing_mode = std::max(cur_viewing_mode - 1, 0);
			// Skip first line of fallthrough
			if(false)
		case '-':
			cur_viewing_mode = std::min(cur_viewing_mode + 1, 3);
			draw_main_screen();
			draw_terrain();
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

// Don't repeatedly prompt when the designer scrolls out of bounds and says no to shifting.
// Unless it's in a new direction.
bool did_out_of_bounds_prompt = false;
int last_dx = 0, last_dy = 0;

static bool handle_outdoor_sec_shift(int dx, int dy){
	if(did_out_of_bounds_prompt) return false;
	if(editing_town) return false;
	int new_x = cur_out.x + dx;
	int new_y = cur_out.y + dy;
	if(new_x < 0) return true;
	if(new_x >= scenario.outdoors.width()) return true;
	if(new_y < 0) return true;
	if(new_y >= scenario.outdoors.height()) return true;

	did_out_of_bounds_prompt = true;
	cChoiceDlog shift_prompt("shift-outdoor-section", {"yes", "no"});
	location new_out_sec = { new_x, new_y };
	shift_prompt->getControl("out-sec").setText(boost::lexical_cast<std::string>(new_out_sec));

	if(shift_prompt.show() == "yes"){
		did_out_of_bounds_prompt = false;
		int last_cen_x = cen_x;
		int last_cen_y = cen_y;
		set_current_out(new_out_sec, true);
		// match the terrain view to where we were
		start_out_edit();
		if(dx < 0) {
			cen_x = get_current_area()->max_dim - 4;
		}else if(dx > 0){
			cen_x = 3;
		}else{
			cen_x = last_cen_x;
		}
		if(dy < 0){
			cen_y = get_current_area()->max_dim - 4;
		}else if(dy > 0){
			cen_y = 3;
		}else{
			cen_y = last_cen_y;
		}
		redraw_screen();
	}
	return true;
}

void handle_editor_screen_shift(int dx, int dy) {
	// Outdoors, you can see 1 tile across the border with neighboring sections:
	int min = (editing_town ? 0 : -1);
	int max = get_current_area()->max_dim - 1;
	if(!editing_town) max++;
	// When zoomed out, you can move your actual center beyond the zoomed-out camera limit,
	// then zoom in and be centered on that place.
	// The visible bounds, not the technical center, are what we care about
	// when prompting whether to shift areas:
	rectangle shift_bounds = visible_bounds();
	bool out_of_bounds = false;
	if(dx != last_dx || dy != last_dy){
		did_out_of_bounds_prompt = false;
	}
	last_dx = dx;
	last_dy = dy;
	if(dx < 0 && shift_bounds.left + dx < min){
		// In outdoors, prompt whether to swap to the next section west
		if(handle_outdoor_sec_shift(-1, 0)) return;
		out_of_bounds = true;
	}else if(dx > 0 && shift_bounds.right + dx > max){
		// In outdoors, prompt whether to swap to the next section east
		if(handle_outdoor_sec_shift(1, 0)) return;
		out_of_bounds = true;
	}else if(dy < 0 && shift_bounds.top + dy < min){
		// In outdoors, prompt whether to swap to the next section north
		if(handle_outdoor_sec_shift(0, -1)) return;
		out_of_bounds = true;
	}else if(dy > 0 && shift_bounds.bottom + dy > max){
		// In outdoors, prompt whether to swap to the next section south
		if(handle_outdoor_sec_shift(0, 1)) return;
		out_of_bounds = true;
	}

	if(out_of_bounds && !did_out_of_bounds_prompt){
		did_out_of_bounds_prompt = true;
		// In town, prompt whether to go back to outdoor entrance location
		std::vector<town_entrance_t> town_entrances = scenario.find_town_entrances(cur_town);
		if(town_entrances.size() == 1){
			town_entrance_t only_entrance = town_entrances[0];
			cChoiceDlog shift_prompt("shift-town-entrance", {"yes", "no"});
			shift_prompt->getControl("out-sec").setText(boost::lexical_cast<std::string>(only_entrance.out_sec));

			if(shift_prompt.show() == "yes"){
				set_current_out(only_entrance.out_sec, true);
				did_out_of_bounds_prompt = false;
				start_out_edit();
				cen_x = only_entrance.loc.x;
				cen_y = only_entrance.loc.y;
				redraw_screen();
				return;
			}
		}else if(town_entrances.size() > 1){
			std::vector<std::string> entrance_strings;
			for(town_entrance_t entrance : town_entrances){
				std::ostringstream sstr;
				sstr << "Entrance in section " << entrance.out_sec << " at " <<  entrance.loc;
				entrance_strings.push_back(sstr.str());

			}
			cStringChoice dlog(entrance_strings, "Shift to one of this town's entrances in the outdoors?");
			size_t choice = dlog.show(-1);
			if(choice >= 0 && choice < town_entrances.size()){
				did_out_of_bounds_prompt = false;
				town_entrance_t entrance = town_entrances[choice];
				set_current_out(entrance.out_sec, true);
				start_out_edit();
				cen_x = entrance.loc.x;
				cen_y = entrance.loc.y;
				redraw_screen();
				return;
			}
		}
	}

	if(!out_of_bounds)
		did_out_of_bounds_prompt = false;

	cen_x = minmax(min+4, max-4, cen_x + dx);
	cen_y = minmax(min+4, max-4, cen_y + dy);
}

void handle_scroll(const sf::Event& event) {
	location pos { translate_mouse_coordinates({event.mouseMove.x,event.mouseMove.y}) };
	int amount = event.mouseWheel.delta;
	if(overall_mode < MODE_MAIN_SCREEN && pos.in(terrain_rect)) {
		if(kb.isCtrlPressed())
			handle_editor_screen_shift(-amount, 0);
		else handle_editor_screen_shift(0, -amount);

		draw_terrain();
		place_location();
	}
}

// This is used by various paintbrushes/the spraycans
void change_circle_terrain(location center,short radius,ter_num_t terrain_type,short probability) {
	// prob is 0 - 20, 0 no, 20 always
	location l;
	cArea* cur_area = get_current_area();
	
	for(short i = 0; i < cur_area->max_dim; i++)
		for(short j = 0; j < cur_area->max_dim; j++) {
			l.x = i;
			l.y = j;
			if((dist(center,l) <= radius) && (get_ran(1,1,20) <= probability))
				set_terrain(l,terrain_type,current_stroke_changes);
		}
}

void change_rect_terrain(rectangle r,ter_num_t terrain_type,short probability,bool hollow) {
	// prob is 0 - 20, 0 no, 20 always
	location l;
	cArea* cur_area = get_current_area();
	stroke_ter_changes_t changes;
	
	for(short i = 0; i < cur_area->max_dim; i++)
		for(short j = 0; j < cur_area->max_dim; j++) {
			l.x = i;
			l.y = j;
			if((i >= r.left) && (i <= r.right) && (j >= r.top) && (j <= r.bottom)
				&& (!hollow || (i == r.left) || (i == r.right) || (j == r.top) || (j == r.bottom))
				&& ((hollow) || (get_ran(1,1,20) <= probability)))
				set_terrain(l,terrain_type,changes);
		}

	undo_list.add(action_ptr(new aDrawTerrain("Change Rectangle Terrain", changes)));
	update_edit_menu();
}

void flood_fill_terrain(location start, ter_num_t terrain_type) {
	static const int dx[4] = {0, 1, 0, -1};
	static const int dy[4] = {-1, 0, 1, 0};
	cArea* cur_area = get_current_area();
	ter_num_t to_replace = cur_area->terrain(start.x, start.y);
	std::stack<location> to_visit;
	std::set<location, loc_compare> visited;
	to_visit.push(start);
	stroke_ter_changes_t changes;
	
	while(!to_visit.empty()) {
		location this_loc = to_visit.top();
		to_visit.pop();
		visited.insert(this_loc);
		for(int i = 0; i < 4; i++) {
			location adj_loc = this_loc;
			adj_loc.x += dx[i];
			adj_loc.y += dy[i];
			if(!cur_area->is_on_map(adj_loc))
				continue;
			ter_num_t check = cur_area->terrain(adj_loc.x, adj_loc.y);
			if(check == to_replace && !visited.count(adj_loc))
				to_visit.push(adj_loc);
		}
		set_terrain(this_loc, terrain_type, changes);
	}

	undo_list.add(action_ptr(new aDrawTerrain("Fill Terrain", changes)));
	update_edit_menu();
}

void frill_up_terrain() {
	ter_num_t terrain_type;
	cArea* cur_area = get_current_area();
	stroke_ter_changes_t changes;
	
	for(short i = 0; i < cur_area->max_dim; i++)
		for(short j = 0; j < cur_area->max_dim; j++) {
			terrain_type = cur_area->terrain(i,j);
			
			for(size_t k = 0; k < scenario.ter_types.size(); k++) {
				if(terrain_type == k) continue;
				cTerrain& ter = scenario.ter_types[k];
				if(terrain_type == ter.frill_for && get_ran(1,1,100) < ter.frill_chance)
					terrain_type = k;
			}
			
			set_terrain(loc(i, j), terrain_type, changes);
		}
	undo_list.add(action_ptr(new aDrawTerrain("Frill Up Terrain", changes)));
	update_edit_menu();
	draw_terrain();
}

void unfrill_terrain() {
	ter_num_t terrain_type;
	cArea* cur_area = get_current_area();
	stroke_ter_changes_t changes;
	
	for(short i = 0; i < cur_area->max_dim; i++)
		for(short j = 0; j < cur_area->max_dim; j++) {
			terrain_type = cur_area->terrain(i,j);
			cTerrain& ter = scenario.ter_types[terrain_type];
			
			if(ter.frill_for >= 0)
				terrain_type = ter.frill_for;
			
			set_terrain(loc(i, j), terrain_type, changes);
		}
	undo_list.add(action_ptr(new aDrawTerrain("Remove Terrain Frills", changes)));
	update_edit_menu();
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
	ter2 = get_current_area()->terrain(x,y);
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

long get_town_entrance(location l) {
	auto& city_locs = current_terrain->city_locs;
	auto iter = std::find(city_locs.begin(), city_locs.end(), l);
	if(iter != city_locs.end()) return iter->spec;
	return 0;
}

// Set, create, or delete the current outdoor section's city_loc for a given location
// Pass town_num -1 to remove the entrance from city_locs
void set_town_entrance(location l, long town_num) {
	auto& city_locs = current_terrain->city_locs;
	auto iter = std::find(city_locs.begin(), city_locs.end(), l);
	if(iter != city_locs.end()){
		// Erase town entrance:
		if(town_num < 0){
			city_locs.erase(iter);
		}
		// Overwrite town entrance:
		else{
			iter->spec = town_num;
		}
	}else{
		// Add new town entrance:
		if(town_num >= 0){
			spec_loc_t loc = l;
			loc.spec = town_num;
			city_locs.push_back(loc);
		}
	}
}

void set_terrain(location l,ter_num_t terrain_type,stroke_ter_changes_t& stroke_changes, bool handle_special) {
	cArea* cur_area = get_current_area();
	
	if(!cur_area->is_on_map(l)) return;
	
	ter_num_t old = cur_area->terrain(l.x,l.y);
	if(stroke_changes.find(l) != stroke_changes.end()){
		// Don't successively overwrite the original terrain value of a tile in one stroke
		old = stroke_changes[l].old_num;
	}
	stroke_changes[l] = {old, terrain_type};
	auto revert = [&cur_area, &stroke_changes, l, old]() {
		cur_area->terrain(l.x,l.y) = old;
		stroke_changes.erase(l);
	};

	const cTerrain& new_ter = scenario.ter_types[terrain_type];
	const cTerrain& old_ter = scenario.ter_types[old];
	if(!editing_town) {
		// Implicitly erase town entrances when a space is set from a town terrain to a non-town terrain
		if(old_ter.special == eTerSpec::TOWN_ENTRANCE && new_ter.special != eTerSpec::TOWN_ENTRANCE){
			long old_town_num = get_town_entrance(l);
			undo_list.add(action_ptr(new aEditTownEntrance(spot_hit, old_town_num, -1)));
			update_edit_menu();
			set_town_entrance(l, -1);
		}
		// Don't implicitly erase signs, because the designer may have entered text in them
	}

	cur_area->terrain(l.x,l.y) = terrain_type;
	location l2 = l;
	
	if(handle_special){
		// Large objects (eg rubble)
		if(scenario.ter_types[terrain_type].obj_num > 0){
			int q = scenario.ter_types[terrain_type].obj_num;
			location obj_loc = scenario.ter_types[terrain_type].obj_pos;
			location obj_dim = scenario.ter_types[terrain_type].obj_size;
			while(obj_loc.x > 0) l2.x-- , obj_loc.x--;
			while(obj_loc.y > 0) l2.y-- , obj_loc.y--;
			for(short i = 0; i < obj_dim.x; i++)
				for(short j = 0; j < obj_dim.y; j++){
					location temp = loc(l2.x + i, l2.y + j);
					if(!cur_area->is_on_map(temp)) continue;

					ter_num_t object_part = find_object_part(q,i,j,terrain_type);
					ter_num_t part_old = cur_area->terrain(temp.x, temp.y);
					if(stroke_changes.find(temp) != stroke_changes.end()){
						part_old = stroke_changes[temp].old_num;
					}
					stroke_changes[temp] = {part_old, object_part};
					cur_area->terrain(temp.x,temp.y) = object_part;
				}
		}
	
		// First make sure surrounding spaces have the correct ground types.
		// This should handle the case of placing hills around mountains.
		unsigned int main_ground = scenario.ter_types[terrain_type].ground_type;
		long trim_ground = scenario.ter_types[terrain_type].trim_ter;
		for(int x = -1; x <= 1; x++) {
			for(int y = -1; y <= 1; y++) {
				location l3(l.x+x,l.y+y);
				if(!cur_area->is_on_map(l3)) continue;
				ter_num_t ter_there = cur_area->terrain(l3.x,l3.y);
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
					stroke_changes[l3] = {ter_there, new_ter};
					cur_area->terrain(l3.x,l3.y) = new_ter;
				}
			}
		}
	
		// Adjusting terrains with trim
		for(location d : trim_diffs) {
			location l3(l.x+d.x, l.y+d.y);
			if(!cur_area->is_on_map(l3)) continue;
			adjust_space(l3, stroke_changes);
		}
	}

	cTerrain& ter = scenario.ter_types[terrain_type];
	// Handle placing special terrains:
	// Signs:
	if(ter.special == eTerSpec::IS_A_SIGN) {
		// Can't place signs at the edge of an outdoor section:
		if(!editing_town && (l.x == 0 || l.x == 47 || l.y == 0 || l.y == 47)) {
			cChoiceDlog("not-at-edge").show();
			revert();
			mouse_button_held = false;
			return;
		}
		if(!handle_special) return;
		auto& signs = cur_area->sign_locs;
		auto iter = std::find(signs.begin(), signs.end(), l);
		if(iter == signs.end()) {
			iter = std::find_if(signs.begin(), signs.end(), [cur_area](const sign_loc_t& sign) {
				if(sign.x == LOC_UNUSED) return true;
				ter_num_t ter = cur_area->terrain(sign.x,sign.y);
				return scenario.ter_types[ter].special != eTerSpec::IS_A_SIGN;
			});
			if(iter == signs.end()) {
				signs.emplace_back();
				iter = signs.end() - 1;
			}
		}
		static_cast<location&>(*iter) = l;
		// Let the designer know the terrain was placed:
		draw_terrain();
		redraw_screen();

		edit_sign(*iter, iter - signs.begin(), ter.picture);
		mouse_button_held = false;
	}
	// Town entrances in the outdoors:
	else if(ter.special == eTerSpec::TOWN_ENTRANCE && !editing_town){
		if(!handle_special) return;
		// Let the designer know the terrain was placed:
		draw_terrain();
		redraw_screen();

		town_entry(l);
		mouse_button_held = false;
	}
}

void adjust_space(location l, stroke_ter_changes_t& stroke_changes) {
	bool needed_change = false;
	location l2;
	cArea* cur_area = get_current_area();
	
	if(!cur_area->is_on_map(l)) return;
	size_t size = cur_area->max_dim;
	ter_num_t off_map = -1;
	
	ter_num_t store_ter[3][3];
	long store_ter2[3][3];
	unsigned int store_ground[3][3];
	eTrimType store_trim[3][3];
	for(int dx = -1; dx <= 1; dx++) {
		for(int dy = -1; dy <= 1; dy++) {
			int x = l.x + dx, y = l.y + dy;
			if(x < 0 || x >= size || y < 0 || y >= size) {
				store_ter[dx+1][dy+1] = off_map;
				continue;
			}
			store_ter[dx+1][dy+1] = cur_area->terrain(x,y);
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
			ter_change_t change = {cur_area->terrain(l.x, l.y), replace};
			if(stroke_changes.find(l) != stroke_changes.end()){
				change.old_num = stroke_changes[l].old_num;
			}
			stroke_changes[l] = change;
			cur_area->terrain(l.x,l.y) = replace;
		}
	}
	
	if(needed_change) {
		for(location d : trim_diffs) {
			if(d.x == 0 && d.y == 0) continue;
			location l2(l.x+d.x, l.y+d.y);
			adjust_space(l2, stroke_changes);
		}
	}
}

bool place_item(location spot_hit,short which_item,bool property,bool always,short odds,std::map<size_t,cTown::cItem>& items_placed) {
	// odds 0 - 100, with 100 always
	if((which_item < 0) || (which_item >= scenario.scen_items.size()))
		return true;
	if(scenario.scen_items[which_item].variety == eItemType::NO_ITEM)
		return true;
	if(get_ran(1,1,100) > odds)
		return false;
	for(short x = 0; x <= town->preset_items.size(); x++){
		if(x == town->preset_items.size()){
			town->preset_items.resize(x+1);
			town->preset_items[x].code = -1;
		}
		if(town->preset_items[x].code < 0) {
			town->preset_items[x] = {spot_hit, which_item, scenario.scen_items[which_item]};
			town->preset_items[x].contained = container_there(spot_hit);
			town->preset_items[x].property = property;
			town->preset_items[x].always_there = always;
			items_placed[x] = town->preset_items[x];

			return true;
		}
	}
	// Shouldn't be reached:
	return false;
}

void place_items_in_town() {
	location l;
	std::map<size_t, cTown::cItem> items_placed;
	for(short i = 0; i < town->max_dim; i++)
		for(short j = 0; j < town->max_dim; j++) {
			l.x = i;
			l.y = j;
			
			for(short k = 0; k < 10; k++)
				if(town->terrain(i,j) == scenario.storage_shortcuts[k].ter_type) {
					for(short x = 0; x < 10; x++)
						place_item(l,scenario.storage_shortcuts[k].item_num[x],
								   scenario.storage_shortcuts[k].property,false,
								   scenario.storage_shortcuts[k].item_odds[x],items_placed);
				}
		}

	if(!items_placed.empty()){
		undo_list.add(action_ptr(new aPlaceEraseItem("Add Random Items", true, items_placed)));
		update_edit_menu();
	}else{
		cChoiceDlog("no-items-added").show();
	}
	draw_terrain();
}

bool place_creature(location spot_hit, mon_num_t which, creature_changes_t& creatures_placed) {
	for(short x = 0; x <= town->creatures.size(); x++){
		if(x == town->creatures.size()){
			town->creatures.resize(x+1);
			town->creatures[x].number = 0;
		}
		if(town->creatures[x].number == 0) {
			town->creatures[x] = {spot_hit, which, scenario.scen_monsters[which]};
			creatures_placed[x] = town->creatures[x];
			return true;
		}
	}
	// Shouldn't be reached:
	return false;
}

void place_edit_special(location loc) {
	if(!editing_town && (loc.x == 0 || loc.x == 47 || loc.y == 0 || loc.y == 47)) {
		cChoiceDlog("not-at-edge").show();
		return;
	}
	auto& specials = get_current_area()->special_locs;
	for(short i = 0; i < specials.size(); i++)
		if(specials[i] == loc && specials[i].spec >= 0) {
			edit_spec_enc(specials[i].spec, editing_town ? 2 : 1, nullptr, false);
			return;
		}
	// new special
	for(short i = 0; i <= specials.size(); i++) {
		bool is_new = false;
		if(i == specials.size()){
			specials.emplace_back(-1,-1,-1);
			is_new = true;
		}
		if(specials[i].spec < 0) {
			if(edit_spec_enc(i, editing_town ? 2: 1, nullptr, is_new)) {
				specials[i] = loc;
				specials[i].spec = i;
				undo_list.add(action_ptr(new aPlaceEraseSpecial("Place Special Encounter", true, specials[i])));
				update_edit_menu();
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
	auto& specials = get_current_area()->special_locs;
	for(short x = 0; x < specials.size(); x++)
		// Edit the node of the encounter already on the space
		if(specials[x] == spot_hit && specials[x].spec >= 0) {
			int spec = edit_special_num(editing_town ? 2 : 1,specials[x].spec);
			if(spec >= 0 && spec != specials[x].spec){
				undo_list.add(action_ptr(new aSetSpecial(spot_hit, specials[x].spec, spec)));
				update_edit_menu();
				specials[x].spec = spec;
			}
			return;
		}
	for(short x = 0; x <= specials.size(); x++) {
		// Find/create an empty encounter spot
		if(x == specials.size())
			specials.emplace_back(-1,-1,-1);
		if(specials[x].spec < 0) {
			int spec = edit_special_num(editing_town ? 2 : 1, 0);
			if(spec >= 0) {
				specials[x] = spot_hit;
				specials[x].spec = spec;
				undo_list.add(action_ptr(new aPlaceEraseSpecial("Place Special Encounter", true, specials[x])));
				update_edit_menu();
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
	long old_town_num = get_town_entrance(spot_hit);
	long town_num = pick_town_num("select-town-enter",old_town_num,scenario);

	if(town_num >= 0 && town_num != old_town_num){
		undo_list.add(action_ptr(new aEditTownEntrance(spot_hit, old_town_num, town_num)));
		update_edit_menu();
		set_town_entrance(spot_hit, town_num);
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
	fs::path last_scenario = get_string_pref("LastScenario");
	if(!last_scenario.empty() && fs::exists(last_scenario)){
		set_lb(5,LB_TEXT,LB_LOAD_LAST,"Load Last: " + last_scenario.filename().string());
	}
	set_lb(4,LB_TEXT,LB_LOAD_SCEN,"Load Scenario");
	set_lb(7,LB_TEXT,LB_NO_ACTION,"To find out how to use the");
	set_lb(8,LB_TEXT,LB_NO_ACTION,"editor, select Getting Started ");
	set_lb(9,LB_TEXT,LB_NO_ACTION,"from the Help menu.");
	set_lb(NLS - 2,LB_TEXT,LB_NO_ACTION,"Created 1997, Free Open Source");
	set_lb(NLS - 1,LB_TEXT,LB_NO_ACTION,version());
	change_made = false;
	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
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
	clear_sstr(strb);
	strb << "  Town " << cur_town << ": " << town->name;
	set_lb(-1,LB_TEXT,LB_NO_ACTION, strb.str());
	set_lb(-1,LB_TEXT,LB_LOAD_TOWN,"Load Another Town");
	set_lb(-1,LB_TEXT,LB_EDIT_TOWN,"Edit Town Terrain");
	set_lb(-1,LB_TEXT,LB_EDIT_TALK,"Edit Town Dialogue");
	set_lb(NLS - 2,LB_TEXT,LB_NO_ACTION,"Created 1997, Free Open Source");
	set_lb(NLS - 1,LB_TEXT,LB_NO_ACTION,version());
	if(overall_mode < MODE_MAIN_SCREEN)
		overall_mode = MODE_MAIN_SCREEN;
	right_sbar->show();
	pal_sbar->hide();
	shut_down_menus(4);
	shut_down_menus(3);
	redraw_screen();
	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
}

static void restore_current_town_state() {
	if(scenario.editor_state.town_view_state.find(cur_town) != scenario.editor_state.town_view_state.end()){
		location cen = scenario.editor_state.town_view_state[cur_town].center;
		cen_x = cen.x;
		cen_y = cen.y;
		cur_viewing_mode = scenario.editor_state.town_view_state[cur_town].cur_viewing_mode;
	}else{
		cur_viewing_mode = 0;
	}
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
	scenario.editor_state.editing_town = editing_town = true;
	scenario.editor_state.drawing = true;
	restore_current_town_state();
	set_up_type_buttons(true);
	shut_down_menus(4);
	shut_down_menus(2);
	right_sbar->hide();
	pal_sbar->show();
	rectangle pal_rect = pal_sbar->getWheelEventRect();
	pal_rect.height() = 1 + TYPE_ROWS_DRAWING * 17;
	pal_sbar->setWheelEventRect(pal_rect);
	pal_rect = pal_sbar->getBounds();
	pal_rect.height() = 1 + TYPE_ROWS_DRAWING * 17;
	pal_sbar->setBounds(pal_rect);
	set_string("Drawing mode",scenario.ter_types[current_terrain_type].name);
	place_location();
	// TODO this is hardcoding cave floor and grass as the only ground terrains
	for(short i = 0; i < town->max_dim; i++)
		for(short j = 0; j < town->max_dim; j++)
			if(town->terrain(i,j) == 0)
				current_ground = 0;
			else if(town->terrain(i,j) == 2)
				current_ground = 2;
	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
}

bool last_shift_continuous = false;

static void restore_current_out_state() {
	// Restore the last view state of the new outdoor section UNLESS we shifted to it continuously
	if(!last_shift_continuous && scenario.editor_state.out_view_state.find(cur_out) != scenario.editor_state.out_view_state.end()){
		location cen = scenario.editor_state.out_view_state[cur_out].center;
		cen_x = cen.x;
		cen_y = cen.y;
		cur_viewing_mode = scenario.editor_state.out_view_state[cur_out].cur_viewing_mode;
	}else{
		cur_viewing_mode = 0;
	}
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
	scenario.editor_state.editing_town = editing_town = false;
	scenario.editor_state.drawing = true;
	restore_current_out_state();
	set_up_type_buttons(true);
	right_sbar->hide();
	pal_sbar->show();
	rectangle pal_rect = pal_sbar->getWheelEventRect();
	pal_rect.height() = 1 + TYPE_ROWS_DRAWING * 17;
	pal_sbar->setWheelEventRect(pal_rect);
	pal_rect = pal_sbar->getBounds();
	pal_rect.height() = 1 + TYPE_ROWS_DRAWING * 17;
	pal_sbar->setBounds(pal_rect);
	shut_down_menus(4);
	shut_down_menus(1);
	set_string("Drawing mode",scenario.ter_types[current_terrain_type].name);
	place_location();
	// TODO this is hardcoding cave floor and grass as the only ground terrains
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 48; j++)
			if(current_terrain->terrain[i][j] == 0)
				current_ground = 0;
			else if(current_terrain->terrain[i][j] == 2)
				current_ground = 2;
	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
	redraw_screen();
}

void start_type_editing(eDrawMode mode) {
	right_sbar->hide();
	pal_sbar->show();
	rectangle pal_rect = pal_sbar->getWheelEventRect();
	pal_rect.height() = terrain_buttons_rect.height();
	pal_sbar->setWheelEventRect(pal_rect);
	pal_rect = pal_sbar->getBounds();
	pal_rect.height() = terrain_buttons_rect.height();
	pal_sbar->setBounds(pal_rect);
	overall_mode = MODE_EDIT_TYPES;
	// Remember non-drawing modes
	scenario.editor_state.overall_mode = MODE_EDIT_TYPES;
	scenario.editor_state.type_editing_mode = mode;
	draw_mode = mode;
	set_up_type_buttons(true);
	place_location();
	
	set_lb(NLS - 3,LB_TEXT,LB_NO_ACTION,"Alt-click to delete/clear",true);
	set_lb(NLS - 2,LB_TEXT,LB_NO_ACTION,"Alt-click '+' to add many",true);
	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
}

void start_special_item_editing() {
	int num_options = scenario.special_items.size() + 1;
	
	handle_close_terrain_view(MODE_EDIT_SPECIAL_ITEMS);
	right_sbar->show();
	pal_sbar->hide();
	right_sbar->setPosition(0);
	reset_rb();
	right_sbar->setMaximum(num_options - NRSONPAGE);

	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
	redraw_screen();
}

void start_quest_editing() {
	int num_options = scenario.quests.size() + 1;

	handle_close_terrain_view(MODE_EDIT_QUESTS);
	right_sbar->show();
	pal_sbar->hide();
	right_sbar->setPosition(0);
	reset_rb();
	right_sbar->setMaximum(num_options - NRSONPAGE);

	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
	redraw_screen();
}

void start_shops_editing() {
	int num_options = scenario.shops.size() + 1;
	handle_close_terrain_view(MODE_EDIT_SHOPS);
	right_sbar->show();
	pal_sbar->hide();
	right_sbar->setPosition(0);
	reset_rb();
	right_sbar->setMaximum(num_options - NRSONPAGE);

	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
	redraw_screen();
}

extern size_t num_strs(short mode); // defined in scen.keydlgs.cpp

void start_string_editing(eStrMode mode) {
	handle_close_terrain_view(MODE_EDIT_STRINGS);
	scenario.editor_state.string_editing_mode = mode;

	right_sbar->show();
	pal_sbar->hide();
	
	reset_rb();
	right_sbar->setMaximum(num_strs(mode) + 1 - NRSONPAGE);

	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
	redraw_screen();
}

// mode 0 - scen 1 - out 2 - town
void start_special_editing(short mode) {
	size_t num_specs;
	switch(mode) {
		case 0: num_specs = scenario.scen_specials.size(); break;
		case 1: num_specs = current_terrain->specials.size(); break;
		case 2: num_specs = town->specials.size(); break;
	}
	
	handle_close_terrain_view(MODE_EDIT_SPECIALS);
	scenario.editor_state.special_editing_mode = mode;
	right_sbar->show();
	pal_sbar->hide();
	
	reset_rb();
	right_sbar->setMaximum(num_specs + 1 - NRSONPAGE);

	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
	redraw_screen();
}

void start_dialogue_editing() {
	handle_close_terrain_view(MODE_EDIT_DIALOGUE);
	right_sbar->show();
	pal_sbar->hide();
	
	right_sbar->setPosition(0);
	reset_rb();

	update_mouse_spot(translate_mouse_coordinates(sf::Mouse::getPosition(mainPtr())));
	redraw_screen();
}

bool save_check(std::string which_dlog, bool allow_no) {
	std::string choice;
	
	if(!change_made)
		return true;
	cChoiceDlog dlog(which_dlog, {"save","revert","cancel"});
	if(!allow_no){
		dlog->getControl("revert").hide();
	}
	choice = dlog.show();
	if(choice == "revert")
		return true;
	else if(choice == "cancel")
		return false;
	change_made = false;
	town->set_up_lights();
	store_current_terrain_state();
	save_scenario();
	return true;
}

ter_num_t coord_to_ter(short x,short y) {
	if(!get_current_area()->is_on_map(loc(x,y))) return 0;
	return get_current_area()->terrain(x,y);
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

// Restore the state of the editor when the designer last saved this scenario.
// This state is serialized in scen.fileio.cpp: writeEditorStateToXml()
// and parsed in fileio_scen.cpp: readEditorStateFromXml().
void restore_editor_state() {
	set_current_town(scenario.editor_state.last_town_edited, true);
	set_current_out(scenario.editor_state.last_out_edited, false, true);
	if(scenario.editor_state.drawing){
		if(scenario.editor_state.editing_town)
			start_town_edit();
		else
			start_out_edit();
	}else{
		switch(scenario.editor_state.overall_mode){
			case MODE_EDIT_TYPES:
				start_type_editing(static_cast<eDrawMode>(scenario.editor_state.type_editing_mode));
				break;
			case MODE_EDIT_SPECIAL_ITEMS:
				start_special_item_editing();
				break;
			default: break;
		}
	}
}

void handle_close_terrain_view(eScenMode new_mode) {
	// When closing a terrain view, store its view state
	store_current_terrain_state();
	scenario.editor_state.drawing = false;
	// Remember non-drawing modes
	scenario.editor_state.overall_mode = new_mode;

	// set up the main screen if needed
	if(new_mode >= MODE_MAIN_SCREEN && overall_mode <= MODE_MAIN_SCREEN)
		set_up_main_screen();

	overall_mode = new_mode;
}
