
#include <cmath>
#include <queue>

#include "boe.global.h"

#include "universe.h"
#include "boe.actions.h"
#include "boe.graphutil.h"
#include "boe.graphics.h"
#include "boe.townspec.h"
#include "boe.fileio.h"
#include "boe.dlgutil.h"
#include "boe.locutils.h"
#include "boe.town.h"
#include "boe.text.h"
#include "boe.party.h"
#include "boe.monster.h"
#include "boe.specials.h"
#include "boe.newgraph.h"
#include "boe.combat.h"
#include "boe.items.h"
#include "soundtool.hpp"
#include "boe.infodlg.h"
#include "boe.itemdata.h"
#include "boe.main.h"
#include "mathutil.hpp"
#include "fileio.hpp"
#include "dlogutil.hpp"
#include "dialog.hpp"
#include "scrollbar.hpp"
#include "boe.menus.h"
#include "winutil.hpp"
#include "cursors.hpp"
#include "spell.hpp"
#include "shop.hpp"

rectangle bottom_buttons[7];
rectangle town_buttons[10];
rectangle combat_buttons[9];
rectangle world_screen = {23, 23, 346, 274};
// TODO: The duplication of rectangle here shouldn't be necessary...
rectangle item_screen_button_rects[9] = {
	rectangle{125,10,141,28},rectangle{125,40,141,58},rectangle{125,68,141,86},rectangle{125,98,141,116},rectangle{125,126,141,144},rectangle{125,156,141,174},
	rectangle{126,176,141,211},
	rectangle{126,213,141,248},
	rectangle{127,251,140,267}};

rectangle border_rect[4] = {rectangle{5, 5, 15, 283}, rectangle{5, 5, 355, 15},
	rectangle{345, 5, 355, 283}, rectangle{5, 273, 355, 283}};
rectangle medium_buttons[4] = {rectangle{383,190,401,225}, rectangle{402, 190, 420, 225},
	rectangle{383, 227, 401, 263}, rectangle{402, 227,420, 263}}; ;

rectangle item_buttons[8][6];
// name, use, give, drip, info, sell/id
rectangle pc_buttons[6][5];
// name, hp, sp, info, trade
short num_chirps_played = 0;

extern rectangle startup_button[6];
extern bool flushingInput;
extern bool fog_lifted;
bool ghost_mode;
rectangle startup_top;

bool item_area_button_active[8][6];
bool pc_area_button_active[6][5];
short item_bottom_button_active[9] = {0,0,0,0,0, 0,1,1,1};

rectangle pc_help_button,pc_area_rect,item_area_rect;

short current_terrain_type = 0,num_out_moves = 0;
short door_pc,store_drop_item;
short current_switch = 6;
cOutdoors::cWandering store_wandering_special;
long dummy;

short store_selling_values[8] = {0,0,0,0,0,0,0,0};
extern cShop active_shop;

extern short cen_x, cen_y, stat_window;//,pc_moves[6];
extern bool give_delays;
extern eGameMode overall_mode;
extern location	to_create;
extern bool All_Done,play_sounds,frills_on,spell_forced,save_maps,monsters_going;
extern bool party_in_memory,in_scen_debug;

// game info globals
extern sf::RenderWindow mainPtr;
extern short which_item_page[6];
extern short store_spell_target,pc_casting;
extern eSpell store_mage, store_priest;
extern short spec_item_array[60];
extern cUniverse univ;
extern std::vector<word_rect_t> talk_words;
extern bool talk_end_forced;

// combat globals


extern short which_combat_type,num_targets_left;
extern location center;
extern short current_pc;
extern short combat_active_pc;
extern eStatMode stat_screen_mode;

extern bool map_visible,diff_depth_ok;
extern sf::RenderWindow mini_map;

extern location ul;
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern short shop_identify_cost;


const char *dir_string[] = {"North", "NorthEast", "East", "SouthEast", "South", "SouthWest", "West", "NorthWest"};
char get_new_terrain();
cCreature save_monster_type;

short wand_loc_count = 0;
short monst_place_count = 0; // 1 - standard place	2 - place last

rectangle shopping_rects[8][7];
std::queue<pending_special_type> special_queue;
bool end_scenario = false;
bool current_bash_is_bash = false;

// This is defined in pc.editors.cpp since the PC editor also uses it
extern void edit_stuff_done();

void init_screen_locs() {
	short i,j,k,l;
	rectangle startup_base = {279,5,327,306};
	rectangle shop_base = {63,12,99,267};
	
	for(i = 0; i < 7; i++)
		shopping_rects[0][i] = shop_base;
	shopping_rects[0][SHOPRECT_ACTIVE_AREA].right -= 35;
	shopping_rects[0][SHOPRECT_GRAPHIC].right = shopping_rects[0][SHOPRECT_GRAPHIC].left + 28;
	shopping_rects[0][SHOPRECT_ITEM_NAME].top += 4;
	shopping_rects[0][SHOPRECT_ITEM_NAME].left += 28;
	shopping_rects[0][SHOPRECT_ITEM_COST].top += 20;
	shopping_rects[0][SHOPRECT_ITEM_COST].left += 154;
	shopping_rects[0][SHOPRECT_ITEM_COST].right -= 20;
	shopping_rects[0][SHOPRECT_ITEM_EXTRA].top += 20;
	shopping_rects[0][SHOPRECT_ITEM_EXTRA].left += 34;
	shopping_rects[0][SHOPRECT_ITEM_HELP].top += 3;
	shopping_rects[0][SHOPRECT_ITEM_HELP].bottom -= 21;
	shopping_rects[0][SHOPRECT_ITEM_HELP].right -= 19;
	shopping_rects[0][SHOPRECT_ITEM_HELP].left = shopping_rects[0][SHOPRECT_ITEM_HELP].right - 14;
	for(i = 1; i < 8; i++)
		for(j = 0; j < 7; j++) {
			shopping_rects[i][j] = shopping_rects[0][j];
			shopping_rects[i][j].offset(0,i * 36);
		}
	
	
	for(i = 0; i < 6; i++) {
		startup_button[i] = startup_base;
		startup_button[i].offset(301 * (i / 3)  - 18,48 * (i % 3));
	}
	startup_top.top = 5;
	startup_top.bottom = startup_button[STARTBTN_LOAD].top;
	startup_top.left = 5;
	startup_top.right = startup_button[STARTBTN_JOIN].right;
	
	for(i = 0; i < 200; i++)
		for(j = 0; j < 8; j++)
			for(k = 0; k < 64; k++)////
				univ.town_maps[i][j][k] = 0;
	
	for(i = 0; i < 100; i++)
		for(k = 0; k < 6; k++)
			for(l = 0; l < 48; l++)
				univ.out_maps[i][k][l] = 0;
	
	for(i = 0; i < 7; i++) {
		bottom_buttons[i].top = 383;
		bottom_buttons[i].bottom = 420;
		bottom_buttons[i].left = 5 + (i * 37);
		bottom_buttons[i].right = bottom_buttons[i].left + 36;
		town_buttons[i] = bottom_buttons[i];
	}
	
	for(i = 0; i < 5; i++) {
		combat_buttons[i] = bottom_buttons[i];
	}
	town_buttons[7] = bottom_buttons[6];
	town_buttons[5] = medium_buttons[0];
	town_buttons[6] = medium_buttons[1];
	for(i = 5; i < 9; i++) {
		combat_buttons[i] = medium_buttons[i - 5];
	}
	
	// name, use, give, drip, info, sell/id   each one 13 down
	item_buttons[0][ITEMBTN_NAME].top = 17;
	item_buttons[0][ITEMBTN_NAME].bottom = item_buttons[0][ITEMBTN_NAME].top + 12;
	item_buttons[0][ITEMBTN_NAME].left = 3;
	item_buttons[0][ITEMBTN_NAME].right = item_buttons[0][ITEMBTN_NAME].left + 185;
	item_buttons[0][ITEMBTN_USE] = item_buttons[0][0];
	item_buttons[0][ITEMBTN_USE].left = 196;
	item_buttons[0][ITEMBTN_USE].right = 210;
	item_buttons[0][ITEMBTN_GIVE] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_GIVE].left = 210;
	item_buttons[0][ITEMBTN_GIVE].right = 224;
	item_buttons[0][ITEMBTN_DROP] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_DROP].left = 224;
	item_buttons[0][ITEMBTN_DROP].right = 238;
	item_buttons[0][ITEMBTN_INFO] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_INFO].left = 238;
	item_buttons[0][ITEMBTN_INFO].right = 252;
	item_buttons[0][ITEMBTN_SPEC] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_SPEC].left = 173;
	item_buttons[0][ITEMBTN_SPEC].right = 232;
	for(i = 1; i < 8; i++)
		for(j = 0; j < 6; j++) {
			item_buttons[i][j] = item_buttons[0][j];
			item_buttons[i][j].offset(0,13 * i);
		}
	
/*	for(i = 0; i < 8; i++) {
		item_screen_button_rects[i] = bottom_base;
		OffsetRect(&item_screen_button_rects[i],10 + i * 29,126);
	}
	item_screen_button_rects[6].left = 176;
	item_screen_button_rects[6].right = 211;
	item_screen_button_rects[7].left = 213;
	item_screen_button_rects[7].right = 248;
	item_screen_button_rects[8].top = 127;
	item_screen_button_rects[8].bottom = 140;
	item_screen_button_rects[8].left = 251;
	item_screen_button_rects[8].right = 267; */
	
	// name, hp, sp, info, trade
	pc_buttons[0][PCBTN_NAME].top = 18;
	pc_buttons[0][PCBTN_NAME].bottom = pc_buttons[0][PCBTN_NAME].top + 12;
	pc_buttons[0][PCBTN_NAME].left = 3;
	pc_buttons[0][PCBTN_NAME].right = pc_buttons[0][PCBTN_NAME].left + 177;
	pc_buttons[0][PCBTN_HP] = pc_buttons[0][PCBTN_NAME];
	pc_buttons[0][PCBTN_HP].left = 184;
	pc_buttons[0][PCBTN_HP].right = 214;
	pc_buttons[0][PCBTN_SP] = pc_buttons[0][PCBTN_NAME];
	pc_buttons[0][PCBTN_SP].left = 214;
	pc_buttons[0][PCBTN_SP].right = 237;
	pc_buttons[0][PCBTN_INFO] = pc_buttons[0][PCBTN_NAME];
	pc_buttons[0][PCBTN_INFO].left = 241;
	pc_buttons[0][PCBTN_INFO].right = 253;
	pc_buttons[0][PCBTN_TRADE] = pc_buttons[0][PCBTN_NAME];
	pc_buttons[0][PCBTN_TRADE].left = 253;
	pc_buttons[0][PCBTN_TRADE].right = 262;
	for(i = 1; i < 6; i++)
		for(j = 0; j < 5; j++) {
			pc_buttons[i][j] = pc_buttons[0][j];
			pc_buttons[i][j].offset(0,13 * i);
		}
	pc_help_button.top = 101;
	pc_help_button.bottom = 114;
	pc_help_button.left = 251;
	pc_help_button.right = 267;
	
	pc_area_rect.top = PC_WIN_UL_Y;
	pc_area_rect.left = PC_WIN_UL_X;
	pc_area_rect.bottom = PC_WIN_UL_Y + 116;
	pc_area_rect.right = PC_WIN_UL_X + 271;
	item_area_rect.top = ITEM_WIN_UL_Y;
	item_area_rect.left = ITEM_WIN_UL_X;
	item_area_rect.bottom = ITEM_WIN_UL_Y + 143;
	item_area_rect.right = ITEM_WIN_UL_X + 271;
}

bool prime_time() {
	if((overall_mode < MODE_TALK_TOWN) || (overall_mode == MODE_COMBAT))
		return true;
	return false;
}

static void handle_spellcast(eSkill which_type, bool& did_something, bool& need_redraw, bool& need_reprint) {
	short store_sp[6];
	if(!someone_awake()) {
		ASB("Everyone's asleep/paralyzed.");
		need_reprint = true;
		need_redraw = true;
	} else if(overall_mode == MODE_OUTDOORS) {
		cast_spell(which_type);
		spell_forced = false;
		need_reprint = true;
		need_redraw = true;
	} else if(overall_mode == MODE_TOWN) {
		for(int i = 0; i < 6; i++)
			store_sp[i] = univ.party[i].cur_sp;
		cast_spell(which_type);
		spell_forced = false;
		need_reprint = true;
		need_redraw = true;
		for(int i = 0; i < 6; i++)
			if(store_sp[i] != univ.party[i].cur_sp)
				did_something = true;
	} else if(overall_mode == MODE_TOWN_TARGET) {
		add_string_to_buf("  Cancelled.");
		overall_mode = MODE_TOWN;
		need_redraw = true;
		need_reprint = true;
	} else if(overall_mode == MODE_COMBAT) {
		if(which_type == eSkill::MAGE_SPELLS) {
			did_something = combat_cast_mage_spell();
			need_reprint = true;
		} else if(which_type == eSkill::PRIEST_SPELLS) {
			did_something = combat_cast_priest_spell();
			need_reprint = true;
		}
		need_redraw = true;
		spell_forced = false;
		redraw_terrain();
	} else if(overall_mode == MODE_SPELL_TARGET || overall_mode == MODE_FANCY_TARGET) {
		add_string_to_buf("  Cancelled.");
		overall_mode = MODE_COMBAT;
		center = univ.party[current_pc].combat_pos;
		pause(10);
		need_redraw = true;
	}
	put_pc_screen();
	put_item_screen(stat_window,0);
}

static void handle_rest(bool& need_redraw, bool& need_reprint) {
	sf::Event dummy_evt;
	int i = 0;
	ter_num_t ter = univ.out[univ.party.p_loc.x][univ.party.p_loc.y];
	if(univ.party.in_boat >= 0)
		add_string_to_buf("Rest:  Not in boat.");
	else if(someone_poisoned())
		add_string_to_buf("Rest: Someone poisoned.");
	else if(univ.party.food <= 12)
		add_string_to_buf("Rest: Not enough food.");
	else if(nearest_monster() <= 3)
		add_string_to_buf("Rest: Monster too close.");
	else if(univ.scenario.ter_types[ter].special == eTerSpec::DAMAGING || univ.scenario.ter_types[ter].special == eTerSpec::DANGEROUS)
		add_string_to_buf("Rest: It's dangerous here.");
	else if(flying())
		add_string_to_buf("Rest: Not while flying.");
	else {
		add_string_to_buf("Resting...");
		print_buf();
		play_sound(-20);
		draw_rest_screen();
		pause(25);
		univ.party.food -= 6;
		while(i < 50) {
			increase_age();
			if(get_ran(1,1,2) == 2)
				do_monsters();
			if(get_ran(1,1,70) == 10)
				create_wand_monst();
			if(nearest_monster() <= 3) {
				i = 200;
				add_string_to_buf("  Monsters nearby.");
			}
			while(mainPtr.pollEvent(dummy_evt));
			redraw_screen(REFRESH_NONE);
			i++;
		}
		put_pc_screen();
	}
	if(i == 50) {
		do_rest(1200, get_ran(5,1,10), 50);
		add_string_to_buf("  Rest successful.");
		put_pc_screen();
		pause(25);
	}
	need_reprint = true;
	need_redraw = true;
}

static void handle_pause(bool& did_something, bool& need_redraw) {
	if(overall_mode == MODE_COMBAT) {
		char_stand_ready();
		add_string_to_buf("Stand ready.");
		if(univ.party[current_pc].status[eStatus::WEBS] > 0) {
			add_string_to_buf("You clean webs.");
			move_to_zero(univ.party[current_pc].status[eStatus::WEBS]);
			move_to_zero(univ.party[current_pc].status[eStatus::WEBS]);
			put_pc_screen();
		}
		check_fields(univ.party[current_pc].combat_pos,eSpecCtx::COMBAT_MOVE,univ.party[current_pc]);
	} else {
		add_string_to_buf("Pause.");
		for(int k = 0; k < 6; k++)
			if(univ.party[k].main_status == eMainStatus::ALIVE && univ.party[k].status[eStatus::WEBS] > 0) {
				add_string_to_buf(univ.party[k].name + " cleans webs.");
				move_to_zero(univ.party[k].status[eStatus::WEBS]);
				move_to_zero(univ.party[k].status[eStatus::WEBS]);
			}
		if(univ.party.in_horse >= 0) {
			if(overall_mode == MODE_OUTDOORS) {
				univ.party.horses[univ.party.in_horse].which_town = 200;
				univ.party.horses[univ.party.in_horse].loc_in_sec = global_to_local(univ.party.p_loc);
				univ.party.horses[univ.party.in_horse].loc = univ.party.p_loc;
				univ.party.horses[univ.party.in_horse].sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
				univ.party.horses[univ.party.in_horse].sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
				univ.party.in_horse = -1;
			} else if(overall_mode == MODE_TOWN){
				univ.party.horses[univ.party.in_horse].loc = univ.town.p_loc;
				univ.party.horses[univ.party.in_horse].which_town = univ.town.num;
				univ.party.in_horse = -1;
			}
		}
		if(univ.party.in_boat >= 0) {
			// If you pause on a bridge or other passable terrain, leave boat.
			if(overall_mode == MODE_OUTDOORS && !impassable(univ.out[univ.party.p_loc.x][univ.party.p_loc.y])) {
				univ.party.boats[univ.party.in_boat].which_town = 200;
				univ.party.boats[univ.party.in_boat].loc_in_sec = global_to_local(univ.party.p_loc);
				univ.party.boats[univ.party.in_boat].loc = univ.party.p_loc;
				univ.party.boats[univ.party.in_boat].sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
				univ.party.boats[univ.party.in_boat].sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
				univ.party.in_boat = -1;
			} else if(overall_mode == MODE_TOWN && !impassable(univ.town->terrain(univ.town.p_loc.x,univ.town.p_loc.y))) {
				univ.party.boats[univ.party.in_boat].loc = univ.town.p_loc;
				univ.party.boats[univ.party.in_boat].which_town = univ.town.num;
				univ.party.in_boat = -1;
			}
		} else {
			// The above could leave you stranded in a single-tile passable area, so pausing again should re-enter the boat.
			int boat = univ.party.boats.size();
			if(overall_mode == MODE_OUTDOORS && (boat = out_boat_there(univ.party.p_loc)) < univ.party.boats.size())
				univ.party.in_boat = boat;
			else if(overall_mode == MODE_TOWN && (boat = town_boat_there(univ.town.p_loc)) < univ.party.boats.size())
				univ.party.in_boat = boat;
			if(boat < univ.party.boats.size())
				ASB("You board the boat.");
		}
		put_pc_screen();
		check_fields(univ.town.p_loc,eSpecCtx::TOWN_MOVE,univ.party[0]);
	}
	
	did_something = true;
	need_redraw = true;
}

static void handle_look(location destination, bool& need_redraw, bool& need_reprint) {
	need_reprint = true;
	// TODO: I'm not sure what this check was for or if it would be needed for anything anymore.
//	if(can_see(cur_loc,destination) >= 4 || (overall_mode != MODE_LOOK_OUTDOORS && loc_off_world(destination)))
	if(overall_mode != MODE_LOOK_COMBAT && party_can_see(destination) == 6)
		add_string_to_buf("  Can't see space.");
	else if(overall_mode == MODE_LOOK_COMBAT && can_see_light(univ.party[current_pc].combat_pos,destination,sight_obscurity) >= 4)
		add_string_to_buf("  Can't see space.");
	else {
		add_string_to_buf("You see...");
		ter_num_t ter_looked_at = do_look(destination);
		if(overall_mode == MODE_LOOK_TOWN || overall_mode == MODE_LOOK_COMBAT)
			if(adjacent(univ.town.p_loc,destination))
				if(adj_town_look(destination))
					need_redraw = true;
		// TODO: This would be the place to call OUT_LOOK special
		// TODO: Do we really need an is_sign function?
		if(is_sign(ter_looked_at)) {
			print_buf();
			need_reprint = false;
			if(overall_mode == MODE_LOOK_TOWN) {
				for(int k = 0; k < univ.town->sign_locs.size(); k++) {
					if(destination == univ.town->sign_locs[k]) {
						need_reprint = true;
						if(adjacent(univ.town->sign_locs[k],univ.town.p_loc))
							do_sign(univ.town.num,k,ter_looked_at);
						else add_string_to_buf("  Too far away to read sign.");
					}
				}
			} else if(overall_mode == MODE_LOOK_OUTDOORS) {
				for(int k = 0; k < univ.out->sign_locs.size(); k++) {
					if(destination == univ.out->sign_locs[k]) {
						need_reprint = true;
						if(adjacent(univ.out->sign_locs[k],univ.party.loc_in_sec))
							do_sign(200 + get_outdoor_num(),k,ter_looked_at);
						else add_string_to_buf("  Too far away to read sign.");
					}
				}
			}
		}
	}
}

static void handle_move(location destination, bool& did_something, bool& need_redraw, bool& need_reprint) {
	bool town_move_done = false;
	if(overall_mode == MODE_COMBAT) {
		if(pc_combat_move(destination)) {
			center = univ.party[current_pc].combat_pos;
			did_something = true;
			update_explored(destination);
		}
		need_redraw = true;
		menu_activate();
	} else if(overall_mode == MODE_TOWN) {
		if(!someone_awake()) {
			ASB("Everyone's asleep/paralyzed.");
			need_reprint = true;
			need_redraw = true;
		} else {
			need_redraw = true;
			if(town_move_party(destination,0)) {
				did_something = true;
				center = univ.town.p_loc;
				update_explored(destination);
				if(loc_off_act_area(univ.town.p_loc)) {
					destination = end_town_mode(0,destination);
					town_move_done = true;
					flushingInput = true;
				}
			} else need_reprint = true;
			menu_activate();
		}
	}
	// If leaving town, we now handle outdoors move
	if(overall_mode == MODE_OUTDOORS)  {
		if(outd_move_party(destination,town_move_done)) {
			center = destination;
			need_redraw = true;
			did_something = true;
			update_explored(univ.party.p_loc);
			menu_activate();
		} else need_redraw = true;
		
		ter_num_t storage = univ.out[univ.party.p_loc.x][univ.party.p_loc.y];
		if(univ.scenario.ter_types[storage].special == eTerSpec::TOWN_ENTRANCE) {
			short find_direction_from;
			if(univ.party.direction == 0) find_direction_from = 2;
			else if(univ.party.direction == 4) find_direction_from = 0;
			else if(univ.party.direction < 4) find_direction_from = 3;
			else find_direction_from = 1;
			
			for(int i = 0; i < univ.out->city_locs.size(); i++)
				if(univ.party.loc_in_sec == univ.out->city_locs[i]) {
					short which_t = univ.out->city_locs[i].spec;
					if(which_t >= 0)
						start_town_mode(univ.out->city_locs[i].spec, find_direction_from);
					if(is_town()) {
						need_redraw = false;
						i = 8;
						if(univ.party.in_boat >= 0)
							univ.party.boats[univ.party.in_boat].which_town = univ.town.num;
						if(univ.party.in_horse >= 0)
							univ.party.horses[univ.party.in_horse].which_town = univ.town.num;
					}
				}
		}
	}
}

static void handle_talk(location destination, bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(can_see_light(center,destination,sight_obscurity) >= 4 || loc_off_world(destination)) {
		add_string_to_buf("  Can't see space.");
		need_reprint = true;
	} else {
		for(int i = 0; i < univ.town.monst.size(); i++) {
			if(univ.town.monst[i].on_space(destination)) {
				did_something = true;
				need_redraw = true;
				if(univ.town.monst[i].special_on_talk >= 0) {
					short s1, s2, s3;
					run_special(eSpecCtx::HAIL, 2, univ.town.monst[i].special_on_talk, univ.town.monst[i].cur_loc, &s1, &s2, &s3);
					if(s3 > 0)
						need_redraw = true;
					if(s1 > 0)
						break;
				}
				if(univ.town.monst[i].attitude % 2 == 1) {
					add_string_to_buf("  Creature is hostile.");
				} else if(univ.town.monst[i].summon_time > 0 || univ.town.monst[i].personality < 0) {
					short small_talk = 1;
					if(univ.town.monst[i].summon_time == 0)
						small_talk = -univ.town.monst[i].personality;
					std::string str = "No response.";
					if(small_talk > 1000) str = univ.scenario.spec_strs[small_talk - 1000];
					// TODO: Come up with a set of pre-cooked responses.
					add_string_to_buf("Talk: " + str, 4);
				} else {
					start_talk_mode(i,univ.town.monst[i].personality,univ.town.monst[i].number,univ.town.monst[i].facial_pic);
					did_something = false;
					need_redraw = false;
					break;
				}
			}
		}
		if(overall_mode != MODE_TALKING) {
			overall_mode = MODE_TOWN;
			need_redraw = true;
			if(!did_something) {
				add_string_to_buf("  Nobody there");
				need_reprint = true;
			}
		}
	}
}

static void handle_target_space(location destination, bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(overall_mode == MODE_SPELL_TARGET)
		do_combat_cast(destination);
	else if(overall_mode == MODE_THROWING || overall_mode == MODE_FIRING)
		fire_missile(destination);
	else if(overall_mode == MODE_TOWN_TARGET)
		cast_town_spell(destination);
	else if(overall_mode == MODE_FANCY_TARGET) {
		place_target(destination);
		need_reprint = true;
	}
	if(overall_mode != MODE_FANCY_TARGET) {
		did_something = true;
		if(overall_mode == MODE_TOWN_TARGET)
			center = univ.town.p_loc;
		else center = univ.party[current_pc].combat_pos;
	}
	if(overall_mode != MODE_TOWN_TARGET) pause(6);
	need_redraw = true;
	if(overall_mode == MODE_TOWN_TARGET) overall_mode = MODE_TOWN;
	else if(is_combat() && overall_mode != MODE_FANCY_TARGET)
		overall_mode = MODE_COMBAT;
	put_pc_screen();
	put_item_screen(stat_window,0);
}

static void handle_drop_item(location destination, bool& need_redraw) {
	if(overall_mode == MODE_DROP_COMBAT) {
		if(!adjacent(univ.party[current_pc].combat_pos,destination))
			add_string_to_buf("Drop: must be adjacent.");
		else {
			drop_item(current_pc,store_drop_item,destination);
			take_ap(1);
		}
		pause(6);
		overall_mode = MODE_COMBAT;
	} else if(overall_mode == MODE_DROP_TOWN) {
		if(!adjacent(univ.town.p_loc,destination))
			add_string_to_buf("Drop: must be adjacent.");
		else if(sight_obscurity(destination.x,destination.y) == 5)
			ASB("Drop: Space is blocked.");
		else drop_item(current_pc,store_drop_item,destination);
		overall_mode = MODE_TOWN;
	}
	need_redraw = true;
	put_pc_screen();
	put_item_screen(stat_window,0);
}

static void handle_use_space(location destination, bool& did_something, bool& need_redraw) {
	if(!adjacent(destination,univ.town.p_loc))
		add_string_to_buf("  Must be adjacent.");
	else did_something = use_space(destination);
	overall_mode = MODE_TOWN;
	need_redraw = true;
	put_pc_screen();
	put_item_screen(stat_window,0);
}

static void handle_bash_pick(location destination, bool& did_something, bool& need_redraw, bool isBash) {
	if(!adjacent(destination,univ.town.p_loc))
		add_string_to_buf("  Must be adjacent.");
	else {
		short pc = char_select_pc(0, isBash ? "Who will bash?" : "Who will pick the lock?");
		if(pc == 6) {
			add_string_to_buf("  Cancelled.");
			overall_mode = MODE_TOWN;
			need_redraw = true;
			return;
		}
		if(isBash) bash_door(destination, pc);
		else pick_lock(destination, pc);
	}
	did_something = true;
	overall_mode = MODE_TOWN;
	need_redraw = true;
	put_pc_screen();
	put_item_screen(stat_window,0);
}

static void handle_switch_pc(short which_pc, bool& need_redraw) {
	if(!prime_time() && overall_mode != MODE_SHOPPING && overall_mode != MODE_TALKING)
		add_string_to_buf("Set active: Finish what you're doing first.");
	else if(is_combat()) {
		if(univ.party[which_pc].ap > 0) {
			draw_terrain();
			current_pc = which_pc;
			combat_next_step();
			set_stat_window(current_pc);
			put_pc_screen();
		} else add_string_to_buf("Set active: PC has no APs.");
	} else if(univ.party[which_pc].main_status != eMainStatus::ALIVE && (overall_mode != MODE_SHOPPING || active_shop.getType() != eShopType::ALLOW_DEAD))
		add_string_to_buf("Set active: PC must be here & active.");
	else {
		current_pc = which_pc;
		set_stat_window(which_pc);
		add_string_to_buf("Now " + std::string(overall_mode == MODE_SHOPPING ? "shopping" : "active") + ": " + univ.party[which_pc].name);
		adjust_spell_menus();
		need_redraw = true;
	}
}

static void handle_switch_pc_items(short which_pc, bool& need_redraw) {
	if(!prime_time() && overall_mode != MODE_TALKING && overall_mode != MODE_SHOPPING)
		add_string_to_buf("Set active: Finish what you're doing first.");
	else {
		if(!is_combat()) {
			if(univ.party[which_pc].main_status != eMainStatus::ALIVE && (overall_mode != MODE_SHOPPING || active_shop.getType() != eShopType::ALLOW_DEAD))
				add_string_to_buf("Set active: PC must be here & active.");
			else {
				current_pc = which_pc;
				add_string_to_buf("Now active: " + univ.party[which_pc].name);
				adjust_spell_menus();
				need_redraw = true;
			}
		}
		set_stat_window(which_pc);
		if(overall_mode == MODE_SHOPPING) {
			set_up_shop_array();
			draw_shop_graphics(0,item_screen_button_rects[which_pc]); // rect is dummy
		}
	}
}

static void handle_equip_item(short item_hit, bool& need_redraw) {
	if(overall_mode == MODE_USE_TOWN) {
		// TODO: Uh, this looks wrong somehow.
		add_string_to_buf("Note: Clicking 'U' button by item uses the item.", 2);
		use_item(stat_window, item_hit);
		overall_mode = MODE_TOWN;
		take_ap(3);
	} else if(prime_time()) {
		equip_item(stat_window, item_hit);
		take_ap(1);
		need_redraw = true;
	} else if(stat_screen_mode > MODE_SHOP) {
		// TODO: For some reason, the game didn't do anything at all in this case.
		// I'm not sure why; maybe it intended to forward to the sell button?
	} else add_string_to_buf("Equip: Finish what you're doing first.");
}

static void handle_use_item(short item_hit, bool& did_something, bool& need_redraw) {
	if(!prime_time()) {
		add_string_to_buf("Use item: Finish what you're doing first.");
		return;
	}
	use_item(stat_window, item_hit);
	if(overall_mode != MODE_TOWN_TARGET && overall_mode != MODE_SPELL_TARGET)
		did_something = true;
	take_ap(3);
	need_redraw = true;
}

static void handle_give_item(short item_hit, bool& did_something, bool& need_redraw) {
	if(!prime_time()) {
		add_string_to_buf("Give item: Finish what you're doing first.");
		return;
	}
	give_thing(stat_window, item_hit);
	did_something = true;
	need_redraw = true;
	take_ap(1);
}

static void handle_drop_item(short item_hit, bool& need_redraw) {
	if(!prime_time())
		add_string_to_buf("Drop item: Finish what you're doing first.");
	else if(is_out())
		drop_item(stat_window,item_hit,univ.party.p_loc);
	else {
		add_string_to_buf("Drop item: Click where to drop item.");
		store_drop_item = item_hit;
		overall_mode = is_town() ? MODE_DROP_TOWN : MODE_DROP_COMBAT;
		need_redraw = true;
	}
}

static void handle_item_shop_action(short item_hit) {
	long i = item_hit - item_sbar->getPosition();
	switch(stat_screen_mode) {
		case MODE_IDENTIFY:
			if(!take_gold(shop_identify_cost,false))
				ASB("Identify: You don't have the gold.");
			else {
				play_sound(68);
				ASB("Your item is identified.");
				univ.party[stat_window].items[item_hit].ident = true;
				univ.party[stat_window].combine_things();
			}
			break;
		case MODE_SELL_WEAP: case MODE_SELL_ARMOR: case MODE_SELL_ANY:
			play_sound(-39);
			univ.party.gold += store_selling_values[i];
			ASB("You sell your item.");
			univ.party[stat_window].take_item(item_hit);
			put_item_screen(stat_window,1);
			break;
		case MODE_ENCHANT:
			if(!take_gold(store_selling_values[i],false))
				ASB("Enchant: You don't have the gold.");
			else {
				play_sound(51);
				ASB("Your item is now enchanted.");
				eEnchant ench = eEnchant(shop_identify_cost);
				univ.party[stat_window].items[item_hit].enchant_weapon(ench,store_selling_values[i]);
			}
			break;
		case MODE_INVEN: case MODE_SHOP:
			// The button doesn't even exist, so just do nothing.
			break;
	}
}

static void handle_alchemy(bool& need_redraw, bool& need_reprint) {
	need_reprint = true;
	need_redraw = true;
	if(overall_mode == MODE_TOWN)
		do_alchemy();
	else add_string_to_buf("Alchemy: Only in town.");
}

static void handle_town_wait(bool& need_redraw, bool& need_reprint) {
	short store_hp[6];
	sf::Event dummy_evt;
	need_reprint = true;
	need_redraw = true;
	
	if(party_sees_a_monst())
		add_string_to_buf("Long wait: Monster in sight.");
	else {
		add_string_to_buf("Long wait...");
		print_buf();
		play_sound(-20);
		draw_rest_screen();
		pause(10);
		for(int i = 0; i < 6; i++) {
			store_hp[i] = univ.party[i].cur_health;
			univ.party[i].status[eStatus::WEBS] = 0;
		}
	}
	
	for(int i = 0; i < 80 && !party_sees_a_monst(); i++){
		increase_age();
		do_monsters();
		do_monster_turn();
		int make_wand = get_ran(1,1,160 - univ.town.difficulty);
		if(make_wand == 10)
			create_wand_monst();
		for(int j = 0; j < 6; j++)
			if(univ.party[j].cur_health < store_hp[j]) {
				i = 200;
				j = 6;
				add_string_to_buf("  Waiting interrupted.");
			}
		if(party_sees_a_monst()) {
			i = 200;
			add_string_to_buf("  Monster sighted!");
		}
		while(mainPtr.pollEvent(dummy_evt));
		redraw_screen(REFRESH_NONE);
	}
	put_pc_screen();
}

static void handle_combat_switch(bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(overall_mode == MODE_TOWN) {
		if(univ.party.in_boat >= 0) {
			need_reprint = true;
			add_string_to_buf("Combat: Not while in boat.");
		} else if(univ.party.in_horse >= 0) {
			need_reprint = true;
			add_string_to_buf("Combat: Not while on horseback.");
		} else {
			add_string_to_buf("Combat!");
			play_sound(18);
			need_reprint = true;
			start_town_combat(univ.party.direction);
			need_redraw = true;
			current_pc = 6;
			did_something = true;
			put_pc_screen();
		}
	} else if(overall_mode == MODE_COMBAT) {
		if(which_combat_type == 0) {
			if(hit_end_c_button()) {
				end_town_mode(0,univ.town.p_loc);
				play_sound(93);
				add_string_to_buf("End combat.");
				handle_wandering_specials(0,1);
				menu_activate();
				put_pc_screen();
				set_stat_window(current_pc);
			} else add_string_to_buf("Can't end combat yet.");
		} else {
			univ.party.direction = end_town_combat();
			center = univ.town.p_loc;
			set_stat_window(current_pc);
			redraw_screen(REFRESH_TERRAIN | REFRESH_TEXT | REFRESH_STATS);
			play_sound(93);
			need_reprint = true;
			need_redraw = true;
			did_something = true;
			menu_activate();
		}
		need_redraw = true;
	}
}

static void handle_missile(bool& need_redraw, bool& need_reprint) {
	if(overall_mode == MODE_COMBAT) {
		load_missile();
		need_reprint = true;
		redraw_terrain();
	} else if(overall_mode == MODE_FIRING || overall_mode == MODE_THROWING) {
		add_string_to_buf("  Cancelled.");
		center = univ.party[current_pc].combat_pos;
		pause(10);
		need_redraw = true;
		overall_mode = MODE_COMBAT;
	}
}

static void handle_get_items(bool& did_something, bool& need_redraw, bool& need_reprint) {
	int j = 0;
	if(univ.party.in_boat >= 0)
		add_string_to_buf("Get: Not while in boat.");
	if(overall_mode == MODE_TOWN)
		j = get_item(univ.town.p_loc,6,false);
	else {
		j = get_item(univ.party[current_pc].combat_pos,current_pc,false);
		take_ap(4);
	}
	if(j > 0) {
		put_item_screen(stat_window, 0);
		put_pc_screen();
		need_redraw = true;
		did_something = true;
	}
	need_reprint = true;
}

static void handle_victory() {
	// This is the point at which we need to transfer any exported graphics over to the party sheet.
	univ.exportGraphics();
	univ.exportSummons();
	univ.clear_stored_pcs();
	reload_startup();
	overall_mode = MODE_STARTUP;
	in_scen_debug = false;
	ghost_mode = false;
	draw_startup(0);
	menu_activate();
	univ.party.scen_name = ""; // should be harmless...
	if(cChoiceDlog("congrats-save",{"cancel","save"}).show() == "save"){
		// TODO: Wait, this shouldn't be a "save as" action, should it? It should save without asking for a location.
		fs::path file = nav_put_party();
		if(!file.empty()) save_party(file, univ);
	}
}

static void handle_party_death() {
	for(int i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::FLED)
			univ.party[i].main_status = eMainStatus::ALIVE;
	if(is_combat()) {
		// TODO: Should this only happen in outdoor combat? Or should we allow fleeing town during combat?
		end_town_mode(0,univ.town.p_loc);
		add_string_to_buf("End combat.");
		handle_wandering_specials(0,2);
	}
	if(!univ.party.is_alive() && univ.party.is_split()) {
		univ.party.end_split(0);
		if(univ.party.left_in == size_t(-1) || univ.town.num == univ.party.left_in) {
			univ.town.p_loc = univ.party.left_at;
			update_explored(univ.town.p_loc);
			center = univ.town.p_loc;
		} else change_level(univ.party.left_in, univ.party.left_at.x, univ.party.left_at.y);
		update_explored(univ.town.p_loc);
		center = univ.town.p_loc;
		if(is_combat()) overall_mode = MODE_TOWN;
		center = univ.town.p_loc;
	}
	menu_activate();
	draw_terrain();
	put_pc_screen();
	put_item_screen(stat_window,0);
	if(!univ.party.is_alive()) {
		play_sound(13);
		handle_death();
	}
}

bool handle_action(sf::Event event) {
	short s1,s2,s3;
	long item_hit;
	bool are_done = false;
	bool need_redraw = false, did_something = false, need_reprint = false;
	bool pc_delayed = false;
	location cur_loc,loc_in_sec,cur_direction;
	short button_hit = 12;
	bool right_button = event.mouseButton.button == sf::Mouse::Right;
	eGameMode previous_mode;
	
	std::ostringstream str;
	location the_point,point_in_area;
	
	the_point = location(event.mouseButton.x, event.mouseButton.y);
	the_point.x -= ul.x;
	the_point.y -= ul.y;
	end_scenario = false;
	
	// Now split off the extra stuff, like talking and shopping.
	if(overall_mode == MODE_TALKING) {
		handle_talk_event(the_point);
		if(overall_mode != MODE_TALKING)
			return false;
	}
	if(overall_mode == MODE_SHOPPING) {
		handle_shop_event(the_point);
		if(overall_mode != MODE_SHOPPING)
			return false;
	}
	
	num_chirps_played = 0;
	
	// MARK: First, figure out where party is
	switch(overall_mode) {
		case MODE_OUTDOORS: case MODE_LOOK_OUTDOORS:
			cur_loc = univ.party.p_loc;
			for(int i = 0; i < 7; i++)
				if(the_point.in(bottom_buttons[i])) {
					button_hit = i;
					if(!spell_forced)
						main_button_click(bottom_buttons[i]);
				}
			break;
			
		case MODE_TOWN: case MODE_TALK_TOWN: case MODE_TOWN_TARGET: case MODE_USE_TOWN: case MODE_LOOK_TOWN:
		case MODE_DROP_TOWN: case MODE_BASH_TOWN:
			cur_loc = center;
			for(int i = 0; i < 8; i++)
				if(the_point.in(town_buttons[i])) {
					button_hit = i;
					if(!spell_forced)
						main_button_click(town_buttons[i]);
				}
			break;
			
		case MODE_TALKING: case MODE_SHOPPING: break;
			
		case MODE_COMBAT: case MODE_SPELL_TARGET: case MODE_FIRING: case MODE_THROWING:
		case MODE_FANCY_TARGET: case MODE_DROP_COMBAT: case MODE_LOOK_COMBAT:
			cur_loc = center;
			for(int i = 0; i < 9; i++)
				if(the_point.in(combat_buttons[i])) {
					button_hit = i;
					if(!spell_forced)
						main_button_click(combat_buttons[i]);
				}
			break;
		case MODE_STARTUP: case MODE_RESTING: case MODE_CUTSCENE:
			// If we get here during these modes, something is probably not right, so bail out
			add_string_to_buf("Unexpected game state!");
			return are_done;
		case MODE_ROOM_DESCR:
			// TODO: This mode is not yet implemented.
			// The idea is to put a long description in the transcript with something like "press any key to continue".
			break;
	}
	
	// MARK: Then, handle a button being hit.
	if(button_hit != 12)
		switch(button_hit) {
			case 0: case 1:
				handle_spellcast(button_hit == 0 ? eSkill::MAGE_SPELLS : eSkill::PRIEST_SPELLS, did_something, need_redraw, need_reprint);
				break;
				
			case 2:
				if(overall_mode == MODE_OUTDOORS) overall_mode = MODE_LOOK_OUTDOORS;
				if(overall_mode == MODE_TOWN) overall_mode = MODE_LOOK_TOWN;
				if(overall_mode == MODE_COMBAT) overall_mode = MODE_LOOK_COMBAT;
				add_string_to_buf("Look: Select a space. You can also right click to look.", 2);
				need_redraw = true;
				break;
				
			case 3:
				if(overall_mode == MODE_COMBAT) {
					add_string_to_buf("Parry.");
					char_parry();
					did_something = true;
					need_reprint = true;
					need_redraw = true;
				} else if(overall_mode == MODE_TOWN) {
					overall_mode = MODE_TALK_TOWN;
					add_string_to_buf("Talk: Select someone.");
					need_reprint = true;
				} else if(overall_mode == MODE_OUTDOORS)
					handle_rest(need_redraw, need_reprint);
				break;
				
			case 4:
				if(overall_mode == MODE_OUTDOORS) {
					give_help(62,0);
					display_map();
					make_cursor_sword();
				} else if(overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT)
					handle_get_items(did_something, need_redraw, need_reprint);
				break;
				
			case 5:
				if(overall_mode == MODE_OUTDOORS) {
					save_party(univ.file, univ);
					need_redraw = true;
					current_switch = 6;
					break;
				} else if(overall_mode == MODE_TOWN) {
					add_string_to_buf("Use: Select a space or item.");
					add_string_to_buf("  (Hit button again to cancel.)");
					need_reprint = true;
					overall_mode = MODE_USE_TOWN;
				} else if(overall_mode == MODE_USE_TOWN) {
					overall_mode = MODE_TOWN;
					need_reprint = true;
					add_string_to_buf("  Cancelled.");
				} else if(overall_mode == MODE_COMBAT) {
					need_reprint = true;
					need_redraw = true;
					pc_delayed = true;
				}
				
				break;
				
			case 6:
				if(overall_mode == MODE_OUTDOORS)
					do_load();
				else if(overall_mode == MODE_TOWN) {
					give_help(62,0);
					display_map();
					make_cursor_sword();
				} else handle_missile(need_redraw, need_reprint);
				break;
				
			case 7:
				handle_combat_switch(did_something, need_redraw, need_reprint);
				break;
				
			case 8:
				if(overall_mode == MODE_COMBAT) {
					if(combat_active_pc == 6) {
						add_string_to_buf("This PC now active.");
						combat_active_pc = current_pc;
					} else {
						add_string_to_buf("All PC's now active.");
						current_pc = combat_active_pc;
						combat_active_pc = 6;
					}
					need_reprint = true;
				}
				break;
		}
	
	// I think this was for a "click to continue" option in the transcript.
	// Or it might be a relic of Exile II room descriptions displayed in the transcript.
/*	if(overall_mode == 30) /// I don't know what this is for.
		if(PtInRect(the_point, &text_panel_rect)) {
			through_sending();
			restore_mode();
			print_buf();
		} */
	
	// MARK: Begin: click in terrain
	if(the_point.in(world_screen) && (is_out() || is_town() || is_combat())){
		int i = (the_point.x - 23) / 28;
		int j = (the_point.y - 23) / 36;
		location destination = cur_loc;
		
		// Check for quick look
		if(right_button) {
			previous_mode = overall_mode;
			if(is_combat()) overall_mode = MODE_LOOK_COMBAT;
			if(is_out()) overall_mode = MODE_LOOK_OUTDOORS;
			if(is_town()) overall_mode = MODE_LOOK_TOWN;
		}
		
		// Moving/pausing
		if(overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) {
			if((i == 4) & (j == 4)) handle_pause(did_something, need_redraw);
			else {
				cur_direction = get_cur_direction(the_point);
				destination.x += cur_direction.x;
				destination.y += cur_direction.y;
				handle_move(destination, did_something, need_redraw, need_reprint);
			}
		}
		
		// Looking at something
		else if(overall_mode == MODE_LOOK_OUTDOORS || overall_mode == MODE_LOOK_TOWN || overall_mode == MODE_LOOK_COMBAT) {
			if(overall_mode == MODE_LOOK_OUTDOORS) destination = univ.party.loc_in_sec;
			destination.x = destination.x + i - 4;
			destination.y = destination.y + j - 4;
			handle_look(destination, need_redraw, need_reprint);
			// If option/ctrl not pressed, looking done, so restore center
			bool look_done = true;
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) look_done = false;
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)) look_done = false;
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) look_done = false;
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) look_done = false;
			if(look_done) {
				if(right_button) overall_mode = previous_mode;
				else if(overall_mode == MODE_LOOK_COMBAT) {
					overall_mode = MODE_COMBAT;
					center = univ.party[current_pc].combat_pos;
					pause(5);
					need_redraw = true;
				}
				else if(overall_mode == MODE_LOOK_TOWN) {
					overall_mode = MODE_TOWN;
					center = univ.town.p_loc;
					need_redraw = true;
				}
				else if(overall_mode == MODE_LOOK_OUTDOORS)
					overall_mode = MODE_OUTDOORS;
				
			}
		}
		
		// Talking to someone
		else if(overall_mode == MODE_TALK_TOWN) {
			destination.x = destination.x + i - 4;
			destination.y = destination.y + j - 4;
			handle_talk(destination, did_something, need_redraw, need_reprint);
		}
		
		// Targeting a space
		else if(overall_mode == MODE_SPELL_TARGET || overall_mode == MODE_FIRING || overall_mode == MODE_THROWING ||
				overall_mode == MODE_FANCY_TARGET || overall_mode == MODE_TOWN_TARGET) {
			destination = center;
			destination.x += i - 4;
			destination.y += j - 4;
			handle_target_space(destination, did_something, need_redraw, need_reprint);
		}
		
		// Dropping an item
		else if(overall_mode == MODE_DROP_TOWN || overall_mode == MODE_DROP_COMBAT) {
			destination.x += i - 4;
			destination.y += j - 4;
			handle_drop_item(destination, need_redraw);
		}
		
		// Using a space
		else if(overall_mode == MODE_USE_TOWN) {
			destination.x += i - 4;
			destination.y += j - 4;
			handle_use_space(destination, did_something, need_redraw);
		}
		
		// Bashing/lockpicking
		else if(overall_mode == MODE_BASH_TOWN) {
			destination.x += i - 4;
			destination.y += j - 4;
			handle_bash_pick(destination, did_something, need_redraw, current_bash_is_bash);
		}
	}
	// MARK: End: click in terrain
	
	// MARK: Begin: Screen shift
	if(overall_mode == MODE_SPELL_TARGET ||  overall_mode == MODE_FIRING || overall_mode == MODE_THROWING
	   || overall_mode == MODE_FANCY_TARGET || overall_mode == MODE_LOOK_COMBAT || overall_mode == MODE_LOOK_TOWN) {
		if(the_point.in(border_rect[0]) && center.y > univ.town->in_town_rect.top && center.y > 4) {
			center.y--;
			need_redraw = true;
		}
		if(the_point.in(border_rect[1]) && center.x > univ.town->in_town_rect.left && center.x > 4) {
			center.x--;
			need_redraw = true;
		}
		if(the_point.in(border_rect[2]) && center.y < univ.town->in_town_rect.bottom && center.y < univ.town->max_dim() - 5) {
			center.y++;
			need_redraw = true;
		}
		if(the_point.in(border_rect[3]) && center.x < univ.town->in_town_rect.right && center.x < univ.town->max_dim() - 5) {
			center.x++;
			need_redraw = true;
		}
	}
	// MARK: End: Screen shift
	
	// MARK: Process clicks in PC stats area
	if(the_point.in(pc_area_rect)) {
		point_in_area = the_point;
		point_in_area.x -= PC_WIN_UL_X;
		point_in_area.y -= PC_WIN_UL_Y;
		if(point_in_area.in(pc_help_button)) {
			pc_help_button.offset(PC_WIN_UL_X,PC_WIN_UL_Y);
			arrow_button_click(pc_help_button);
			pc_help_button.offset(-PC_WIN_UL_X,-PC_WIN_UL_Y);
			cChoiceDlog("help-party").show();
		}
		for(int i = 0; i < 6; i++)
			for(int j = 0; j < 5; j++)
				if(pc_area_button_active[i][j] && point_in_area.in(pc_buttons[i][j])) {
					pc_buttons[i][j].offset(PC_WIN_UL_X,PC_WIN_UL_Y);
					arrow_button_click(pc_buttons[i][j]);
					pc_buttons[i][j].offset(-PC_WIN_UL_X,-PC_WIN_UL_Y);
					switch(j) {
						case 0:
							handle_switch_pc(i, need_redraw);
							need_reprint = true;
							break;
						case 1:
							str.str("");
							str << univ.party[i].name << " has ";
							str << univ.party[i].cur_health << " health out of " << univ.party[i].max_health << '.';
							add_string_to_buf(str.str());
							break;
						case 2:
							str.str("");
							str << univ.party[i].name << " has ";
							str << univ.party[i].cur_health << " spell pts. out of " << univ.party[i].max_health << '.';
							add_string_to_buf(str.str());
							break;
						case 3: // pc info
							give_pc_info(i);
							break;
						case 4: // trade places
							if(!prime_time())
								add_string_to_buf("Trade places: Finish what you're doing first.");
							else if(is_combat())
								add_string_to_buf("Trade places: Can't do this in combat.");
							else {
								switch_pc(i);
							}
							break;
					}
				}
		need_reprint = true;
		put_pc_screen();
		put_item_screen(stat_window,0);
		if(overall_mode == MODE_SHOPPING) {
			set_up_shop_array();
			draw_shop_graphics(0,pc_buttons[0][0]);
		}
	}
	
	// Process clicks in item stats area
	if(the_point.in(item_area_rect)) {
		point_in_area = the_point;
		point_in_area.x -= ITEM_WIN_UL_X;
		point_in_area.y -= ITEM_WIN_UL_Y;
		
		for(int i = 0; i < 9; i++)
			if(item_bottom_button_active[i] > 0 && point_in_area.in(item_screen_button_rects[i])) {
				item_screen_button_rects[i].offset(ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
				arrow_button_click(item_screen_button_rects[i]);
				item_screen_button_rects[i].offset(-ITEM_WIN_UL_X,-ITEM_WIN_UL_Y);
				switch(i) {
					case 6: // special screen
						give_help(50,0);
						set_stat_window(ITEM_WIN_SPECIAL);
						break;
					case 7:
						set_stat_window(ITEM_WIN_QUESTS);
						break;
					case 8: // help
						cChoiceDlog("help-inventory").show();
						break;
					default:
						handle_switch_pc_items(i, need_redraw);
						break;
				}
			}
		if(stat_window <= ITEM_WIN_QUESTS) {
			for(int i = 0; i < 8; i++)
				for(int j = 0; j < 6; j++)
					if(item_area_button_active[i][j] && point_in_area.in(item_buttons[i][j])) {
						item_buttons[i][j].offset(ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
						arrow_button_click(item_buttons[i][j]);
						item_buttons[i][j].offset(-ITEM_WIN_UL_X,-ITEM_WIN_UL_Y);
						
						item_hit = item_sbar->getPosition() + i;
						switch(j) {
							case 0: // equip
								handle_equip_item(item_hit, need_redraw);
								break;
							case 1: // use
								handle_use_item(item_hit, did_something, need_redraw);
								break;
							case 2: // give
								handle_give_item(item_hit, did_something, need_redraw);
								break;
							case 3: // drop
								if(stat_window == ITEM_WIN_SPECIAL) {
									use_spec_item(spec_item_array[item_hit]);
									need_redraw = true;
								} else handle_drop_item(item_hit, need_redraw);
								break;
							case 4: // info
								if(stat_window == ITEM_WIN_SPECIAL)
									put_spec_item_info(spec_item_array[item_hit]);
								else if(stat_window == ITEM_WIN_QUESTS)
									put_quest_info(spec_item_array[item_hit]);
								else display_pc_item(stat_window, item_hit,univ.party[stat_window].items[item_hit],0);
								break;
							case 5: // sell? That this code was reached indicates that the item was sellable
								// (Based on item_area_button_active)
								handle_item_shop_action(item_hit);
								break;
						}
					}
		}
		put_pc_screen();
		put_item_screen(stat_window,0);
		need_reprint = true;
	}
	
	
	// MARK: Fake clicks (alchemy, town rest)
	if(the_point.x == 1000) handle_alchemy(need_redraw, need_reprint);
	if(the_point.x == 1001) handle_town_wait(need_redraw, need_reprint);
	if(the_point.x == 1002 || the_point.x == 1003) {
		if(overall_mode == MODE_BASH_TOWN) {
			add_string_to_buf("  Cancelled.");
			overall_mode = MODE_TOWN;
		} else {
			overall_mode = MODE_BASH_TOWN;
			current_bash_is_bash = the_point.x == 1002;
			add_string_to_buf(the_point.x == 1002 ? "Bash Door: Select a space." : "Pick Lock: Select a space.");
		}
		need_reprint = true;
	}
	
 	// MARK: If in combat and pc delayed, jump forward a step
 	if(pc_delayed) {
 		draw_terrain();
		//pause(2);
		current_pc++;
		combat_next_step();
		set_stat_window(current_pc);
		put_pc_screen();
	}
 	
 	// MARK: At this point, see if any specials have been queued up, and deal with them
	// Note: We just check once here instead of looping because run_special also pulls from the queue.
	if(!special_queue.empty()) {
		s3 = 0;
		pending_special_type pending = special_queue.front();
		special_queue.pop();
		run_special(pending, &s1, &s2, &s3);
		if(s3 > 0) need_redraw = true;
	}
	
	// MARK: Handle non-PC stuff (like monsters) if the party actually did something
	if(did_something) handle_monster_actions(need_redraw, need_reprint);
	if(fog_lifted) need_redraw = true;
	fog_lifted = false;
	if(need_redraw) draw_terrain();
	if(need_reprint || need_redraw) print_buf();
	
	if(!univ.party.is_alive())
		handle_party_death();
	else if(end_scenario)
		handle_victory();
	
	are_done = All_Done;
	return are_done;
}

void handle_monster_actions(bool& need_redraw, bool& need_reprint) {
	draw_map(true);
	play_ambient_sound();
	
	if(is_combat() && overall_mode != MODE_LOOK_COMBAT) {
		if(no_pcs_left()) {
			end_combat();
			if(which_combat_type == 0) {
				end_town_mode(0,univ.party.p_loc);
				add_string_to_buf("Fled the combat.");
				handle_wandering_specials(0,2);
			}
		} else {
			if(need_redraw) {
				draw_terrain();
				if((combat_active_pc == 6) || (univ.party[combat_active_pc].ap > 0))
					need_redraw = false;
			}
			//pause(2);
			if(combat_next_step())
				need_redraw = true;
		}
	} else {
		increase_age();
		if(!is_out() || (is_out() && univ.party.age % 10 == 0)) // no monst move if party outdoors and on horse
			do_monsters();
		if(overall_mode != MODE_OUTDOORS)
			do_monster_turn();
		// Wand monsts
		if(overall_mode == MODE_OUTDOORS && univ.party.is_alive() && univ.party.age % 10 == 0) {
			if(get_ran(1,1,70 + PSD[SDF_LESS_WANDER_ENC] * 200) == 10)
				create_wand_monst();
			for(int i = 0; i < 10; i++)
				if(univ.party.out_c[i].exists)
					if((adjacent(univ.party.p_loc,univ.party.out_c[i].m_loc) || univ.party.out_c[i].what_monst.cant_flee >= 10)
					   && univ.party.in_boat < 0 && !flying()) {
						store_wandering_special = univ.party.out_c[i].what_monst;
						if(handle_wandering_specials(0,0))
							initiate_outdoor_combat(i);
						univ.party.out_c[i].exists = false;
						
						// Get rid of excess keyclicks
						flushingInput = true;
						
						need_reprint = false;
						i = 20;
					}
		}
		if(overall_mode == MODE_TOWN) {
			if(get_ran(1,1,160 - univ.town.difficulty + PSD[SDF_LESS_WANDER_ENC] * 200) == 2)
				create_wand_monst();
		}
	}
}

bool someone_awake() {
	short i;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE &&
		   univ.party[i].status[eStatus::ASLEEP] <= 0 && univ.party[i].status[eStatus::PARALYZED] <= 0)
			return true;
	return false;
}


void handle_menu_spell(eSpell spell_picked) {
	eSkill spell_type = (*spell_picked).type;
	if(!prime_time()) {
		ASB("Finish what you're doing first.");
		print_buf();
		return;
	}
	location pass_point;
	sf::Event event;
	
	spell_forced = true;
	pc_casting = current_pc;
	univ.party[current_pc].last_cast[spell_type] = spell_picked;
	if(spell_type == eSkill::MAGE_SPELLS)
		store_mage = spell_picked;
	else store_priest = spell_picked;
	if(spell_type == eSkill::MAGE_SPELLS && (*spell_picked).need_select != SELECT_NO) {
		if((store_spell_target = char_select_pc((*spell_picked).need_select == SELECT_ANY ? 1 : 0,"Cast spell on who?")) == 6)
			return;
	}
	else {
		if(spell_type == eSkill::PRIEST_SPELLS && (*spell_picked).need_select != SELECT_NO)
			if((store_spell_target = char_select_pc((*spell_picked).need_select == SELECT_ANY ? 1 : 0,"Cast spell on who?")) == 6)
				return;
	}
/*	if((is_combat()) && (((spell_type == 0) && (refer_mage[spell_picked] > 0)) ||
						  ((spell_type == 1) && (refer_priest[spell_picked] > 0)))){
		if((spell_type == 0) && (mage_need_select[spell_picked] > 0))
			store_spell_target = char_select_pc(2 - mage_need_select[spell_picked],0,"Cast spell on who?");
		else if((spell_type == 1) && (priest_need_select[spell_picked] > 0))
			store_spell_target = char_select_pc(2 - priest_need_select[spell_picked],0,"Cast spell on who?");
	}
	else {
	} */
	if(spell_type == eSkill::MAGE_SPELLS) {
		pass_point.x = bottom_buttons[0].left + 5;
		pass_point.y = bottom_buttons[0].top + 5;
	} else {
		pass_point.x = bottom_buttons[1].left + 5;
		pass_point.y = bottom_buttons[1].top + 5;
	}
	event.mouseButton.x = pass_point.x + ul.x;
	event.mouseButton.y = pass_point.y + ul.y;
	handle_action(event);
}

void initiate_outdoor_combat(short i) {
	short m,n;
	location to_place;
	
	draw_terrain();
	
	// Is combat too easy?
	if((univ.party.get_level() > ((out_enc_lev_tot(i) * 5) / 3) ) && (out_enc_lev_tot(i) < 200)
		&& (univ.party.out_c[i].what_monst.cant_flee % 10 != 1)) {
		add_string_to_buf("Combat: Monsters fled!");
		univ.party.out_c[i].exists = false;
		return;
	}
	
//	Delay((long) 100,&dummy);
	
	start_outdoor_combat(univ.party.out_c[i], univ.out[univ.party.p_loc.x][univ.party.p_loc.y],count_walls(univ.party.p_loc));
	
	univ.party.out_c[i].exists = false;
	
	for(m = 0; m < 6; m++)
		if(univ.party[m].main_status == eMainStatus::ALIVE)
			to_place = univ.party[m].combat_pos;
	for(m = 0; m < 6; m++)
		for(n = 0; n < 24; n++)
			if(univ.party[m].main_status != eMainStatus::ALIVE && univ.party[m].items[n].variety != eItemType::NO_ITEM) {
				place_item(univ.party[m].items[n],to_place,true);
				univ.party[m].items[n].variety = eItemType::NO_ITEM;
			}
	
	overall_mode = MODE_COMBAT;
	center = univ.party[current_pc].combat_pos;
	draw_terrain();
}

bool handle_keystroke(sf::Event& event){
	bool are_done = false;
	location pass_point; // TODO: This isn't needed
	short i,j;
	std::ostringstream sout;
	using kb = sf::Keyboard;
	using Key = sf::Keyboard::Key;
	
	Key keypad[10] = {kb::Numpad0,kb::Numpad1,kb::Numpad2,kb::Numpad3,kb::Numpad4,kb::Numpad5,kb::Numpad6,kb::Numpad7,kb::Numpad8,kb::Numpad9};
	// TODO: The duplication of location here shouldn't be necessary...
	location terrain_click[10] = {location{150,185},location{120,215},location{150,215},location{180,215},
		location{120,185},location{150,185},location{180,185},
		location{120,155},location{150,155},location{180,135}};
	Key talk_chars[9] = {kb::L,kb::N,kb::J,kb::B,kb::S,kb::R,kb::D,kb::G,kb::A};
	Key shop_chars[8] = {kb::A,kb::B,kb::C,kb::D,kb::E,kb::F,kb::G,kb::H};
	
	if(map_visible && event.key.code == kb::Escape
	   && (overall_mode != MODE_TALKING) && (overall_mode != MODE_SHOPPING)) {
		mini_map.setVisible(false);
		map_visible = false;
		mainPtr.setActive();
		return false;
	}
	
	if(overall_mode == MODE_STARTUP)
		return false;
	
	obscureCursor();
	
	// DEBUG
//	sprintf((char *) debug, "%d    ",(short) chr2);
//	add_string_to_buf((char *) debug);
//	print_buf();
	Key chr2 = event.key.code;
	
	if(chr2 == kb::LShift || chr2 == kb::LAlt || chr2 == kb::LControl || chr2 == kb::LSystem) return false;
	if(chr2 == kb::RShift || chr2 == kb::RAlt || chr2 == kb::RControl || chr2 == kb::RSystem) return false;
	
	if(chr2 == kb::Up && !kb::isKeyPressed(kb::Down)) {
		if(kb::isKeyPressed(kb::Left)) chr2 = kb::Numpad7;
		else if(kb::isKeyPressed(kb::Right)) chr2 = kb::Numpad9;
		else chr2 = kb::Numpad8;
	} else if(chr2 == kb::Down && !kb::isKeyPressed(kb::Up)) {
		if(kb::isKeyPressed(kb::Left)) chr2 = kb::Numpad1;
		else if(kb::isKeyPressed(kb::Right)) chr2 = kb::Numpad3;
		else chr2 = kb::Numpad2;
	} else if(chr2 == kb::Left && !kb::isKeyPressed(kb::Right)) {
		if(kb::isKeyPressed(kb::Up)) chr2 = kb::Numpad7;
		else if(kb::isKeyPressed(kb::Down)) chr2 = kb::Numpad1;
		else chr2 = kb::Numpad4;
	} else if(chr2 == kb::Right && !kb::isKeyPressed(kb::Left)) {
		if(kb::isKeyPressed(kb::Up)) chr2 = kb::Numpad9;
		else if(kb::isKeyPressed(kb::Down)) chr2 = kb::Numpad3;
		else chr2 = kb::Numpad6;
	}
	
	sf::Event pass_event = {sf::Event::MouseButtonPressed};
	if(overall_mode == MODE_TALKING) {
		if(chr2 == kb::Escape)
			chr2 = kb::D;
		if(chr2 == kb::Space)
			chr2 = kb::G;
		for(i = 0; i < 9; i++)
			if(chr2 == talk_chars[i] && (!talk_end_forced || i == 6 || i == 5)) {
				int j = talk_end_forced ? i - 5 : i;
				pass_point.x = talk_words[j].rect.left + 9 + ul.x;
				pass_point.y = talk_words[j].rect.top + 9 + ul.y;
				pass_event.mouseButton.x = pass_point.x;
				pass_event.mouseButton.y = pass_point.y;
				are_done = handle_action(pass_event);
			}
	}
	else if(overall_mode == MODE_SHOPPING) { // shopping keystrokes
		if(chr2 == kb::Escape) {
			play_sound(37);
			end_shop_mode();
		}
		for(i = 0; i < 8; i++)
			if(chr2 == shop_chars[i]) {
				pass_point.x = shopping_rects[i][SHOPRECT_ACTIVE_AREA].left + 9 + ul.x;
				pass_point.y = shopping_rects[i][SHOPRECT_ACTIVE_AREA].top + 9 + ul.y;
				pass_event.mouseButton.x = pass_point.x;
				pass_event.mouseButton.y = pass_point.y;
				are_done = handle_action(pass_event);
			}
	} else {
		for(i = 0; i < 10; i++)
			if(chr2 == keypad[i]) {
				if(i == 0) {
					chr2 = kb::Z;
				}
				else {
					pass_point.x = terrain_click[i].x + ul.x;
					pass_point.y = terrain_click[i].y + ul.y;
					pass_event.mouseButton.x = pass_point.x;
					pass_event.mouseButton.y = pass_point.y;
					are_done = handle_action(pass_event);
					return are_done;
				}
			}
	}
	
	char chr = keyToChar(chr2, event.key.shift);
	
	switch(chr) {
			
		case '&':
			add_string_to_buf("If Valorim ...");
			print_buf();
			break;
		case '*':
			add_string_to_buf("You want to save ...");
			print_buf();
			break;
		case '(':
			add_string_to_buf("Back up your save files ...");
			print_buf();
			break;
		case ')':
			add_string_to_buf("Burma Shave.");
			print_buf();
			break;
			
		case '?':
			if(overall_mode == MODE_SHOPPING) {
				univ.party.help_received[26] = 0;
				give_help(226,27);
				break;
			}
			if(overall_mode == MODE_TALKING) {
				univ.party.help_received[5] = 0;
				give_help(205,6);
				break;
			}
			if(is_out()) cChoiceDlog("help-outdoor").show();
			if(is_town()) cChoiceDlog("help-town").show();
			if(is_combat()) cChoiceDlog("help-combat").show();
			break;
			
		case '1': case '2': case '3': case '4': case '5': case '6':
			pass_point.x = pc_buttons[((short) chr) - 49][0].left + 1 + PC_WIN_UL_X + ul.x;
			pass_point.y = pc_buttons[((short) chr) - 49][0].top + PC_WIN_UL_Y + ul.y;
			pass_event.mouseButton.x = pass_point.x;
			pass_event.mouseButton.y = pass_point.y;
			are_done = handle_action(pass_event);
			break;
			
		case '9':
			pass_point.x = item_screen_button_rects[6].left + ITEM_WIN_UL_X + ul.x;
			pass_point.y = item_screen_button_rects[6].top + ITEM_WIN_UL_Y + ul.y;
			pass_event.mouseButton.x = pass_point.x;
			pass_event.mouseButton.y = pass_point.y;
			are_done = handle_action(pass_event);
			break;
			
		case ' ':
			if(overall_mode == MODE_FANCY_TARGET) { // cast multi-target spell, set # targets to 0 so that
				// space clicked doesn't matter
				num_targets_left = 0;
				pass_point = terrain_click[5];
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
			}
			if(overall_mode == MODE_SPELL_TARGET)
				spell_cast_hit_return();
			break;
			
			
		case 'D':
			if(in_scen_debug) {
				in_scen_debug = false;
				ASB("Debug mode OFF.");
			} else {
				in_scen_debug = true;
				ASB("Debug mode ON.");
			}
			print_buf();
			break;
		case 'z':
			if(((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING)) || (overall_mode == MODE_LOOK_COMBAT)) {
				set_stat_window(current_pc);
				put_item_screen(stat_window,0);
			} else {
				set_stat_window(0);
				put_item_screen(stat_window,0);
			}
			break;
			
		case '=':
			if(!in_scen_debug) break;
			univ.party.gold += 100;
			univ.party.food += 100;
			for(i = 0; i < 6; i++) {
				univ.party[i].main_status = eMainStatus::ALIVE;
				univ.party[i].cur_health = univ.party[i].max_health;
				univ.party[i].cur_sp = 100;
			}
			award_party_xp(25);
			for(i = 0; i < 6; i++)
				for(j = 0; j < 62; j++) {
					univ.party[i].priest_spells[j] = 1;
					univ.party[i].mage_spells[j] = 1;
				}
			refresh_store_items();
			add_string_to_buf("Debug: Add stuff and heal.");
			print_buf();
			put_pc_screen();
			break;
			
		case 'B':
			if(!in_scen_debug) break;
			if(overall_mode == MODE_OUTDOORS){
				add_string_to_buf("Debug - Leave Town: You're not in town!");
				print_buf();
				break;
			}
			univ.party.end_split(0);
			overall_mode = MODE_OUTDOORS;
			position_party(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y,univ.party.p_loc.x,univ.party.p_loc.y);
			clear_map();
			add_string_to_buf("Debug: Reunite party and leave town.");
			print_buf();
			redraw_screen(REFRESH_ALL);
			break;
			
		case 'C':
			if(!in_scen_debug) break;
			univ.party.clear_bad_status();
			add_string_to_buf("Debug: You get cleaned up!");
			print_buf();
			put_pc_screen();
			break;
			
		case 'E':
			if(!in_scen_debug) break;
			univ.party.status[ePartyStatus::STEALTH] += 10;
			univ.party.status[ePartyStatus::DETECT_LIFE] += 10;
			univ.party.status[ePartyStatus::FIREWALK] += 10;
			add_string_to_buf("Debug: Stealth, Detect Life, Firewalk!");
			draw_text_bar();
			print_buf();
			put_pc_screen();
			break;
			
		case 'F':
			if(!in_scen_debug) break;
			if(overall_mode != MODE_OUTDOORS){
				add_string_to_buf("Debug: Can only fly outdoors.");
			}else{
				univ.party.status[ePartyStatus::FLIGHT] += 10;
				add_string_to_buf("Debug: You start flying!");
			}
			print_buf();
			put_pc_screen();
			break;
			
		case 'G':
			if(!in_scen_debug) break;
			if(ghost_mode){
				ghost_mode = false;
				ASB("Debug: Ghost mode OFF.");
			}else{
				ghost_mode = true;
				ASB("Debug:Ghost mode ON.");
			}
			print_buf();
			break;
			
		case 'H':
			if(!in_scen_debug) break;
			univ.party.gold += 100;
			univ.party.food += 100;
			for(i = 0; i < 6; i++) {
				if(isDead(univ.party[i].main_status))
					univ.party[i].main_status = eMainStatus::ALIVE;
			}
			univ.party.heal(250);
			univ.party.restore_sp(100);
			add_string_to_buf("Debug: Heal party.");
			print_buf();
			put_pc_screen();
			break;
			
		case 'K':
			if(!in_scen_debug) break;
			for(i = 0; i < univ.town.monst.size(); i++) {
				if((is_combat()) && (univ.town.monst[i].active > 0) && (univ.town.monst[i].attitude % 2 == 1))
					univ.town.monst[i].active = 0;
				
				if((univ.town.monst[i].active > 0) && (univ.town.monst[i].attitude % 2 == 1)
					&& (dist(univ.town.monst[i].cur_loc,univ.town.p_loc) <= 10) )
					damage_monst(univ.town.monst[i], 7,1000,eDamageType::UNBLOCKABLE,0);
			}
			// kill_monst(&univ.town.monst[i],6);
			draw_terrain();
			add_string_to_buf("Debug: Kill things.");
			print_buf();
			break;
			
		case 'N':
			if(!in_scen_debug) break;
			handle_victory();
			break;
			
		case 'O':
			if(!in_scen_debug) break;
			if(is_town()) {
				sout << "Debug:  You're at x " << (short) univ.town.p_loc.x << ", y " << (short) univ.town.p_loc.y
				<< " in town " << univ.town.num << '.';
			} else if(is_out()) {
				short x = univ.party.p_loc.x;
				short y = univ.party.p_loc.y;
				x += 48 * univ.party.outdoor_corner.x;
				y += 48 * univ.party.outdoor_corner.y;
				sout << "Debug:  You're outside at x " << x << ", y " << y << '.';
			} else if(is_combat()) {
				location loc = univ.party[current_pc].combat_pos;
				sout << "Debug:  You're in combat at x " << loc.x << ", y " << loc.y << '.';
			}
			add_string_to_buf(sout.str());
			print_buf();
			break;
			
		case 'I': // TODO: Seems useless?
//			if(!in_scen_debug) break;
//			sout << "Debug: The party's age is " << univ.party.age;
//			add_string_to_buf(sout.str());
//			add_string_to_buf("Debug: Reset map."); // Surely this won't work?
//			print_buf();
			break;
			
		case 'Q':
			if(!in_scen_debug) break;
			if(overall_mode == MODE_OUTDOORS) {
				for(i = 0; i < 96; i++)
					for(j = 0; j < 96; j++)
						make_explored(i,j);
			} else {
				for(i = 0; i < 64; i++)
					for(j = 0; j < 64; j++)
						make_explored(i,j);
			}
			clear_map();
			add_string_to_buf("Debug:  Magic Map.");
			print_buf();
			break;
			
		case 'R':
			if(!in_scen_debug) break;
			if(univ.party.in_boat >= 0) {
				add_string_to_buf("  Not while in boat.");
				break;
			}
			if(univ.party.in_horse >= 0) {
				add_string_to_buf("  Not while on horse.");
				break;
			}
			force_town_enter(univ.scenario.which_town_start,univ.scenario.where_start);
			start_town_mode(univ.scenario.which_town_start,9);
			position_party(univ.scenario.out_sec_start.x,univ.scenario.out_sec_start.y,
						   univ.scenario.out_start.x,univ.scenario.out_start.y);
			center = univ.town.p_loc = univ.scenario.where_start;
			redraw_screen(REFRESH_ALL);
			add_string_to_buf("Debug:  You return to the start.");
			print_buf();
			break;
			
		case 'S':
			if(!in_scen_debug) break;
			edit_stuff_done();
			break;
			
		case 'T':
			if(!in_scen_debug) break;
			short find_direction_from;
            i = get_num_response(0, univ.scenario.towns.size() - 1, "Enter Town Number");
            if(i >= 0 && i < univ.scenario.towns.size()) {
            	if(univ.party.direction == 0) find_direction_from = 2;
				else if(univ.party.direction == 4) find_direction_from = 0;
				else if(univ.party.direction < 4) find_direction_from = 3;
				else find_direction_from = 1;
				start_town_mode(i, find_direction_from);
			}
			break;
			
		case 'W':
			if(!in_scen_debug) break;
			refresh_store_items();
			add_string_to_buf("Debug: Refreshed jobs/shops.");
			print_buf();
			break;
			
		case '<':
			if(!in_scen_debug) break;
			ASB("Debug: Increase age.");
			ASB("  It is now 1 day later.");
			print_buf();
			univ.party.age += 3700;
			put_pc_screen();
			break;
		case '>':
			if(!in_scen_debug) break;
			ASB("DEBUG: Towns have short memory.");
			ASB("Your deeds have been forgotten.");
			print_buf();
			for(i = 0; i < 4; i++)
				univ.party.creature_save[i].which_town = 200;
			break;
		case '/':
			if(!in_scen_debug) break;
			cChoiceDlog("help-debug").show();
			break;
		case 'a':
			if(overall_mode < MODE_TALK_TOWN) {
				pass_point.x = (overall_mode == MODE_OUTDOORS) ? 170 : 221;
				pass_point.y = 405;
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
			}
			break;
			
		case 'u':
			if(overall_mode == MODE_TOWN || overall_mode == MODE_USE_TOWN) {
				pass_point.x = 220;
				pass_point.y = 388;
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
			}
			break;
			
		case 'b': case 'L':
			if(overall_mode == MODE_TOWN || overall_mode == MODE_BASH_TOWN) {
				pass_point.x = chr == 'b' ? 1002 : 1003;
				pass_point.y = 0;
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
			}
			
		case 's': case 'x': case 'e':
			if((overall_mode == MODE_COMBAT) ||
				((overall_mode == MODE_FIRING)  && (chr == 's')) ||
				((overall_mode == MODE_THROWING)  && (chr == 's')) ) {
				pass_point.x = (chr == 's') ? 205 : 240;
				pass_point.y = (chr == 'e') ? 390 : 406;
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
			}
			break;
			
			
		case 'm': case 'p': case 'l': case 'r': case 'w': case 't': case 'd': case 'g': case 'f':
		case 'M': case 'P': case 'A':
			j = 50;
			if(overall_mode == MODE_SPELL_TARGET || overall_mode == MODE_FANCY_TARGET || overall_mode == MODE_TOWN_TARGET) { // cancel spell
				if(chr == 'm') j = 0;
				else if(chr == 'p') j = 1;
			}
			if(chr == 'f' && (overall_mode == MODE_FIRING || overall_mode == MODE_THROWING || chr == 't')) // cancel missile
				j = 6;
			if((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_TOWN) || (overall_mode == MODE_COMBAT)) {
				switch(chr) {
					case 'M': spell_forced = true; j = 0; break;
					case 'm': j = 0; break;
					case 'P': spell_forced = true; j = 1; break;
					case 'p': j = 1; break;
					case 'l': j = 2; break;
					case 'r':
						if(overall_mode != MODE_OUTDOORS) return false;
						j = 3;
						break;
					case 't':
						if(overall_mode == MODE_TOWN)
							j = 3;
					else return false;
						break;
					case 'A':
						if(overall_mode == MODE_TOWN) {
							pass_point.x = 1000 + ul.x;
							pass_event.mouseButton.x = pass_point.x;
							pass_event.mouseButton.y = pass_point.y;
							are_done = handle_action(pass_event);
						} else {
							add_string_to_buf("Alchemy: In town only.");
							print_buf();
							return false;
						}
						break;
					case 'w':
						if(overall_mode == MODE_COMBAT)
							j = 5;
						else if(overall_mode == MODE_TOWN) {
							pass_point.x = 1001 + ul.x;
							pass_event.mouseButton.x = pass_point.x;
							pass_event.mouseButton.y = pass_point.y;
							are_done = handle_action(pass_event);
						}
						else {
							add_string_to_buf("Wait: In town only.");
							print_buf();
							return false;
						}
						break;
					case 'd':
						if(overall_mode != MODE_COMBAT) return false;
						j = 3;
						break;
					case 'g':
						if(overall_mode == MODE_OUTDOORS) return false;
						j = 4;
						break;
					case 'f':
						if(overall_mode != MODE_TOWN) return false;
						j = 6;
						break;
				}
			}
			if(j < 50) {
				pass_point.x = bottom_buttons[j].left + 5;
				pass_point.y = bottom_buttons[j].top + 5;
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
			}
			break;
	}
	spell_forced = false;
	return are_done;
}

void do_load() {
	fs::path file_to_load = nav_get_party();
	if(file_to_load.empty()) return;
	if(!load_party(file_to_load, univ))
		return;
	finish_load_party();
	if(overall_mode != MODE_STARTUP)
		post_load();
	menu_activate();
}

void post_load() {
	current_switch = 6;
	
	reset_item_max();
	
	if(overall_mode == MODE_OUTDOORS)
		update_explored(univ.party.p_loc);
//	if(overall_mode == MODE_TOWN) {
//		make_town_trim(0);
//		}
//	make_out_trim();
	
	text_sbar->show();
	item_sbar->show();
	shop_sbar->hide();
	set_stat_window(0);
	put_pc_screen();
	draw_terrain();
	draw_buttons(0);
	draw_text_bar();
	
	print_buf();
	
	clear_map();
	adjust_spell_menus();
	adjust_monst_menu();
}

//mode; // 0 - normal  1 - save as
void do_save(short mode) {
	if(overall_mode > MODE_TOWN && overall_mode != MODE_STARTUP) {
		add_string_to_buf("Save: Only while outdoors, or in town and not looking/casting.", 2);
		print_buf();
		return;
	}
	fs::path file = univ.file;
	if(mode == 1) file = nav_put_party(file);
	if(!file.empty()) {
		univ.file = file;
		save_party(univ.file, univ);
	}
	
	
	pause(6);
	redraw_screen(REFRESH_TEXT);
}

void do_rest(long length, int hp_restore, int mp_restore) {
	unsigned long age_before = univ.party.age;
	univ.party.age += length;
	if(!PSD[SDF_TIMERS_DURING_REST]) {
		univ.party.heal(hp_restore);
		univ.party.restore_sp(mp_restore);
		put_pc_screen();
		return;
	}
	// If some players diseased, allow it to progress a bit
	handle_disease();
	handle_disease();
	handle_disease();
	// Clear party spell effects
	univ.party.status[ePartyStatus::STEALTH] = 0;
	univ.party.status[ePartyStatus::DETECT_LIFE] = 0;
	univ.party.status[ePartyStatus::FIREWALK] = 0;
	univ.party.status[ePartyStatus::FLIGHT] = 0; // This one shouldn't be nonzero anyway, since you can't rest while flying.
	for(int i = 0; i < 6; i++)
		univ.party[i].status.clear();
	// Specials countdowns
	if((length > 500 || age_before / 500 < univ.party.age / 500) && univ.party.has_abil(eItemAbil::OCCASIONAL_STATUS)) {
		// TODO: There used to be a "display strings" here; should we hook in a special node call?
		for(int i = 0; i < 6; i++)
			for(int j = 0; j < 24; j++) {
				cItem& item = univ.party[i].items[j];
				if(item.ability != eItemAbil::OCCASIONAL_STATUS) continue;
				if(item.abil_data[1] < 0 || item.abil_data[1] > 15) continue;
				if(!item.abil_group()) continue;
				if(get_ran(1,0,5) != 3) continue;
				int how_much = item.abil_data[0];
				if(item.abil_harms()) how_much *= -1;
				if(isStatusNegative(eStatus(item.abil_data[1])))
					how_much *= -1;
				univ.party.apply_status(eStatus(item.abil_data[1]), how_much);
			}
	}
	// Plants and magic shops
	if(length > 4000 || age_before / 4000 < univ.party.age / 4000)
		refresh_store_items();
	// Heal party
	univ.party.heal(hp_restore);
	univ.party.restore_sp(mp_restore);
	// Recuperation and chronic disease disads
	for(int i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
			if(univ.party[i].traits[eTrait::RECUPERATION] && univ.party[i].cur_health < univ.party[i].max_health) {
				univ.party[i].heal(hp_restore / 5);
			}
			if(univ.party[i].traits[eTrait::CHRONIC_DISEASE] && get_ran(1,0,110) == 1) {
				univ.party[i].disease(6);
			}
			short item = univ.party[i].has_abil_equip(eItemAbil::REGENERATE);
			if(item < 24 && univ.party[i].cur_health < univ.party[i].max_health && (overall_mode > MODE_OUTDOORS || get_ran(1,0,10) == 5)){
				int j = get_ran(1,0,univ.party[i].items[item].abil_data[0] / 3);
				if(univ.party[i].items[item].abil_data[0] / 3 == 0)
					j = get_ran(1,0,1);
				if(is_out()) j = j * 4;
				univ.party[i].heal(j);
			}
			// Bonus SP and HP wear off
			if(univ.party[i].cur_sp > univ.party[i].max_sp)
				univ.party[i].cur_sp = univ.party[i].max_sp;
			if(univ.party[i].cur_health > univ.party[i].max_health)
				univ.party[i].cur_health = univ.party[i].max_health;
		}
	special_increase_age(length, true);
	put_pc_screen();
	adjust_spell_menus();
}

void increase_age() {
	short i,j,item,how_many_short = 0,r1;
	
	
	// Increase age, adjust light level & stealth
	if(is_out()) {
		if(univ.party.in_horse < 0)
			univ.party.age -= univ.party.age % 10;
		else univ.party.age -= univ.party.age % 5;
		univ.party.age += 5;
		if(univ.party.in_horse < 0)
			univ.party.age += 5;
		
	}
	else univ.party.age++;
	
	move_to_zero(univ.party.light_level);
	
	// decrease monster present counter
	move_to_zero(PSD[SDF_HOSTILES_PRESENT]);
	
	// Party spell effects
	if(univ.party.status[ePartyStatus::STEALTH] == 1) {
		add_string_to_buf("Your footsteps grow louder.");
	}
	move_to_zero(univ.party.status[ePartyStatus::STEALTH]);
	if(univ.party.status[ePartyStatus::DETECT_LIFE] == 1) {
		add_string_to_buf("You stop detecting monsters.");
	}
	move_to_zero(univ.party.status[ePartyStatus::DETECT_LIFE]);
	if(univ.party.status[ePartyStatus::FIREWALK] == 1) {
		add_string_to_buf("Your feet stop glowing.");
	}
	move_to_zero(univ.party.status[ePartyStatus::FIREWALK]);
	
	if(univ.party.status[ePartyStatus::FLIGHT] == 2)
		add_string_to_buf("You are starting to descend.");
	if(univ.party.status[ePartyStatus::FLIGHT] == 1) {
		if(blocksMove(univ.scenario.ter_types[univ.out[univ.party.p_loc.x][univ.party.p_loc.y]].blockage)) {
			add_string_to_buf("  You plummet to your deaths.");
			slay_party(eMainStatus::DEAD);
			print_buf();
			pause(150);
		}
		else add_string_to_buf("  You land safely.");
	}
	
	move_to_zero(univ.party.status[ePartyStatus::FLIGHT]);
	
	if(overall_mode > MODE_OUTDOORS && univ.town->lighting_type >= LIGHT_DRAINS) {
		increase_light(-9);
		if(univ.town->lighting_type == LIGHT_NONE) {
			if(univ.party.light_level > 0)
				ASB("Your light is drained.");
			univ.party.light_level = 0;
		}
	}
	
	if(is_town() && univ.town->lighting_type != LIGHT_NORMAL) {
		int radiance = 0;
		for(int i = 0; i < 6; i++)
			radiance += univ.party[i].get_prot_level(eItemAbil::RADIANT);
		if(radiance > 0 && univ.party.light_level < radiance && get_ran(1,1,10) < radiance) {
			ASB("One of your items is glowing softly!");
			univ.party.light_level += radiance * 3;
		}
	}
	
	// Specials countdowns
	if(univ.party.age % 500 == 0 && univ.party.has_abil(eItemAbil::OCCASIONAL_STATUS)) {
		// TODO: There used to be a "display strings" here; should we hook in a special node call?
		for(int i = 0; i < 6; i++)
			for(int j = 0; j < 24; j++) {
				cItem& item = univ.party[i].items[j];
				if(item.ability != eItemAbil::OCCASIONAL_STATUS) continue;
				if(item.abil_data[1] < 0 || item.abil_data[1] > 15) continue;
				if(!item.abil_group()) continue;
				if(get_ran(1,0,5) != 3) continue;
				int how_much = item.abil_data[0];
				if(item.abil_harms()) how_much *= -1;
				eStatus status = eStatus(item.abil_data[1]);
				if(isStatusNegative(status))
					how_much *= -1;
				univ.party.apply_status(status, how_much);
			}
	}
	
	
	// Plants and magic shops
	if(univ.party.age % 4000 == 0) {
		refresh_store_items();
	}
	
	// Protection, etc.
	for(i = 0; i < 6; i++) { // Process some status things, and check if stats updated
		
		if(univ.party[i].status[eStatus::INVULNERABLE] == 1 || abs(univ.party[i].status[eStatus::MAGIC_RESISTANCE]) == 1
		   || univ.party[i].status[eStatus::INVISIBLE] == 1 || univ.party[i].status[eStatus::MARTYRS_SHIELD] == 1
		   || abs(univ.party[i].status[eStatus::ASLEEP]) == 1 || univ.party[i].status[eStatus::PARALYZED] == 1)
		move_to_zero(univ.party[i].status[eStatus::INVULNERABLE]);
		move_to_zero(univ.party[i].status[eStatus::MAGIC_RESISTANCE]);
		move_to_zero(univ.party[i].status[eStatus::INVISIBLE]);
		move_to_zero(univ.party[i].status[eStatus::MARTYRS_SHIELD]);
		move_to_zero(univ.party[i].status[eStatus::ASLEEP]);
		move_to_zero(univ.party[i].status[eStatus::PARALYZED]);
		if(univ.party.age % 40 == 0 && univ.party[i].status[eStatus::POISONED_WEAPON] > 0) {
			move_to_zero(univ.party[i].status[eStatus::POISONED_WEAPON]);
		}
		
	}
	
	// Food
	if((univ.party.age % 1000 == 0) && (overall_mode < MODE_COMBAT)) {
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				how_many_short++;
		how_many_short = take_food (how_many_short,false);
		if(how_many_short > 0) {
			add_string_to_buf("Starving!");
			play_sound(66);
			r1 = get_ran(3,1,6);
			hit_party(r1,eDamageType::UNBLOCKABLE);
			if(overall_mode < MODE_COMBAT)
				boom_space(univ.party.p_loc,overall_mode,0,r1,0);
		}
		else {
			play_sound(6);
			add_string_to_buf("You eat.");
		}
	}
	
	// Poison, acid, disease damage
	for(i = 0; i < 6; i++) // Poison
		if(univ.party[i].status[eStatus::POISON] > 0) {
			i = 6;
			if(((overall_mode == MODE_OUTDOORS) && (univ.party.age % 50 == 0)) || ((overall_mode == MODE_TOWN) && (univ.party.age % 20 == 0))) {
				do_poison();
			}
		}
	for(i = 0; i < 6; i++) // Disease
		if(univ.party[i].status[eStatus::DISEASE] > 0) {
			i = 6;
			if(((overall_mode == MODE_OUTDOORS) && (univ.party.age % 100 == 0)) || ((overall_mode == MODE_TOWN) && (univ.party.age % 25 == 0))) {
				handle_disease();
			}
		}
	for(i = 0; i < 6; i++) // Acid
		if(univ.party[i].status[eStatus::ACID] > 0) {
			i = 6;
			handle_acid();
		}
	
	// Healing and restoration of spell pts.
	if(is_out()) {
		if(univ.party.age % 100 == 0) {
			univ.party.heal(2);
		}
	}
	else {
		if(univ.party.age % 50 == 0) {
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE && univ.party[i].cur_health > univ.party[i].max_health)
					univ.party[i].cur_health--; // Bonus HP wears off
			univ.party.heal(1);
		}
	}
	if(is_out()) {
		if(univ.party.age % 80 == 0) {
			univ.party.restore_sp(2);
		}
	}
	else {
		if(univ.party.age % 40 == 0) {
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE && univ.party[i].cur_sp > univ.party[i].max_sp)
					univ.party[i].cur_sp--; // Bonus SP wears off
			univ.party.restore_sp(1);
		}
	}
	
	// Recuperation and chronic disease disads
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
			if(univ.party[i].traits[eTrait::RECUPERATION] && get_ran(1,0,10) == 1 && univ.party[i].cur_health < univ.party[i].max_health) {
				univ.party[i].heal(2);
			}
			if(univ.party[i].traits[eTrait::CHRONIC_DISEASE] && get_ran(1,0,110) == 1) {
				univ.party[i].disease(4);
			}
			
		}
	
	
	// Blessing, slowed,etc.
	if(univ.party.age % 4 == 0)
		for(i = 0; i < 6; i++) {
			move_to_zero(univ.party[i].status[eStatus::BLESS_CURSE]);
			move_to_zero(univ.party[i].status[eStatus::HASTE_SLOW]);
			if((item = univ.party[i].has_abil_equip(eItemAbil::REGENERATE)) < 24
			   && (univ.party[i].cur_health < univ.party[i].max_health)
			   && ((overall_mode > MODE_OUTDOORS) || (get_ran(1,0,10) == 5))){
				j = get_ran(1,0,univ.party[i].items[item].abil_data[0] / 3);
				if(univ.party[i].items[item].abil_data[0] / 3 == 0)
					j = get_ran(1,0,1);
				if(is_out()) j = j * 4;
				univ.party[i].heal(j);
			}
		}
	
	dump_gold(1);
	
	special_increase_age();
	push_things();
	
	if(is_town()) {
		process_fields();
	}
	
	// Cancel switching PC order
	current_switch = 6;
	
	// If a change, draw stat screen
	put_pc_screen();
	adjust_spell_menus();
}

void handle_hunting() {
	if(!is_out())
		return;
	if(flying())
		return;
	ter_num_t ter = univ.out[univ.party.p_loc.x][univ.party.p_loc.y];
	if(!wilderness_lore_present(ter))
		return;
	eTrait trait = eTrait::PACIFIST;
	if(univ.scenario.ter_types[ter].special == eTerSpec::WILDERNESS_CAVE)
		trait = eTrait::CAVE_LORE;
	else if(univ.scenario.ter_types[ter].special == eTerSpec::WILDERNESS_SURFACE)
		trait = eTrait::WOODSMAN;
	if(trait == eTrait::PACIFIST)
		return;
	
	for(int i = 0; i < 6; i++)
		if(univ.party[i].is_alive() && univ.party[i].traits[trait] && get_ran(1,0,12) == 5) {
			univ.party.food += get_ran(univ.scenario.ter_types[ter].flag1,1,6);
			add_string_to_buf(univ.party[i].name + "hunts.");
			put_pc_screen();
		}
}

void switch_pc(short which) {
	if(current_switch < 6) {
		if(current_switch != which) {
			add_string_to_buf("Switch: OK.");
			univ.party.swap_pcs(which, current_switch);
			if(current_pc == current_switch)
				current_pc = which;
			else if(current_pc == which)
				current_pc = current_switch;
			set_stat_window(current_pc);
		} else ASB("Switch: Not with self.");
		current_switch = 6;
	}
	else {
		add_string_to_buf("Switch: Switch with who?");
		current_switch = which;
	}
}

void drop_pc(short which) {
	std::string choice;
	
	choice = cChoiceDlog("delete-pc-confirm",{"yes","no"}).show();
	if(choice == "no") {
		add_string_to_buf("Delete PC: Cancelled.");
		return;
	}
	add_string_to_buf("Delete PC: OK.");
	kill_pc(univ.party[which],eMainStatus::ABSENT);
	for(short i = which; i < 5; i++)
		univ.party.swap_pcs(i, i + 1);
	univ.party[5].main_status = eMainStatus::ABSENT;
	set_stat_window(0);
	put_pc_screen();
}

void handle_death() {
	std::string choice;
	
	overall_mode = MODE_STARTUP;
	
	while(true) {
		// Use death (or leave Exile) dialog
		choice = cChoiceDlog("party-death",{"load","new","quit"}).show();
		
		if(choice == "quit") {
			All_Done = true;
			return;
		}
		else if(choice == "load") {
			fs::path file_to_load = nav_get_party();
			if(!file_to_load.empty()) load_party(file_to_load, univ);
			if(univ.party.is_alive()) {
				if(overall_mode != MODE_STARTUP)
					post_load(), finish_load_party();
            	return;
			}
		}
		else if(choice == "new") {
			// TODO: Windows version dumps to main screen without creating a party; which is better?
			start_new_game();
			reload_startup();
			return;
		}
	}
	
}

void start_new_game() {
	short i;
	std::string choice;
	using kb = sf::Keyboard;
	
	choice = cChoiceDlog("new-party",{"okay","cancel"}).show();
	if(choice == "cancel")
		return;
	
	// Destroy the universe
	univ.~cUniverse();
	
	long party_type = 'dflt';
	
//	display_intro();
	// If system key held down, create debug party
#ifdef __APPLE__
	if(kb::isKeyPressed(kb::LSystem) || kb::isKeyPressed(kb::RSystem))
#else
	if(kb::isKeyPressed(kb::LControl) || kb::isKeyPressed(kb::RControl))
#endif
		party_type = 'dbug';
	
	// And now, reconstruct the universe.
	new(&univ) cUniverse(party_type);
	
	// Destroy party graphics
	extern cCustomGraphics spec_scen_g;
	spec_scen_g.party_sheet.reset();
	
	// Default is save maps
	PSD[SDF_NO_MAPS] = 0;
	save_maps = true;
	
	// The original code called build_outdoors here, but they're not even in a scenario, so I removed it.
	// It was probably a relic of Exile III.
	// (It also refreshed stores... with uninitialized items.)
	
	edit_party();
	
	// if no PCs left, forget it
	for(i = 0 ; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			i = 100;
	if(i == 6)
		return;
	
	
	// everyone gets a weapon
	for(i = 0; i < 6; i++) {
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
			univ.party[i].finish_create();
		}
	}
	fs::path file = nav_put_party();
	if(!file.empty()) save_party(file, univ);
	party_in_memory = true;
}

location get_cur_direction(location the_point) {
	location store_dir;
	
	// This is a kludgy adjustment to adjust for the screen shifting between Exile I & II
	the_point.x += 5;
	the_point.y += 5;
	
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
	
	return store_dir;
}

static eDirection find_waterfall(short x, short y, short mode){
	// If more than one waterfall adjacent, randomly selects
	bool to_dir[8];
	for(eDirection i = DIR_N; i < DIR_HERE; i++){
		if(mode == 0){
			eTerSpec spec = univ.scenario.ter_types[univ.town->terrain(x + dir_x_dif[i],y + dir_y_dif[i])].special;
			to_dir[i] = spec == eTerSpec::WATERFALL_CAVE || spec == eTerSpec::WATERFALL_SURFACE;
			if(univ.scenario.ter_types[univ.town->terrain(x + dir_x_dif[i],y + dir_y_dif[i])].flag1 != i) to_dir[i] = false;
		}else{
			eTerSpec spec = univ.scenario.ter_types[univ.out[x + dir_x_dif[i]][y + dir_y_dif[i]]].special;
			to_dir[i] = spec == eTerSpec::WATERFALL_CAVE || spec == eTerSpec::WATERFALL_SURFACE;
			if(univ.scenario.ter_types[univ.out[x + dir_x_dif[i]][y + dir_y_dif[i]]].flag1 != i) to_dir[i] = false;
		}
	}
	short count = 0;
	for(int i = 0; i < 8; i++)
		count += to_dir[i];
	if(count > 0) count = get_ran(1,1,count);
	else return DIR_HERE;
	for(eDirection i = DIR_N; i < DIR_HERE; i++){
		if(to_dir[i]){
			count--;
			if(count == 0) return i;
		}
	}
	return DIR_HERE; // just in case something wonky happens
}

static void run_waterfalls(short mode){ // mode 0 - town, 1 - outdoors
	short x,y;
	if(mode == 0){
		x = univ.town.p_loc.x;
		y = univ.town.p_loc.y;
	}else{
		x = univ.party.p_loc.x;
		y = univ.party.p_loc.y;
	}
	eDirection dir;
	while((dir = find_waterfall(x,y,mode)) != DIR_HERE){
		add_string_to_buf("  Waterfall!");
		if(mode == 0){
			x += 2 * dir_x_dif[dir];
			y += 2 * dir_y_dif[dir];
			univ.town.p_loc.x += 2 * dir_x_dif[dir];
			univ.town.p_loc.y += 2 * dir_y_dif[dir];
			update_explored(univ.party.p_loc);
		}else{
			x += 2 * dir_x_dif[dir];
			y += 2 * dir_y_dif[dir];
			univ.party.p_loc.x += 2 * dir_x_dif[dir];
			univ.party.loc_in_sec.x += 2 * dir_x_dif[dir];
			univ.party.p_loc.y += 2 * dir_y_dif[dir];
			univ.party.loc_in_sec.y += 2 * dir_y_dif[dir];
			update_explored(univ.party.p_loc);
		}
		draw_terrain();
		print_buf();
		if(wilderness_lore_present(coord_to_ter(x - dir_x_dif[dir], y - dir_y_dif[dir]) > 0) && get_ran(1,0,1) == 0)
			add_string_to_buf("  (No supplies lost.)");
		else if(univ.party.food > 1800){
			// TODO: Shouldn't this check n instead of univ.party.food?
			add_string_to_buf("  (Many supplies lost.)");
			univ.party.food -= 50;
		}
		else {
			int n = univ.party.food;
			univ.party.food = (univ.party.food * 19) / 20;
			add_string_to_buf("  (" + std::to_string(n - univ.party.food) + " supplies lost.)");
		}
		put_pc_screen();
		play_sound(28);
		pause(8);
	}
	if(mode == 0){
		univ.party.boats[univ.party.in_boat].loc = univ.town.p_loc;
		univ.party.boats[univ.party.in_boat].which_town = univ.town.num;
	}else{
		univ.party.boats[univ.party.in_boat].which_town = 200;
		univ.party.boats[univ.party.in_boat].loc_in_sec = univ.party.loc_in_sec;
		univ.party.boats[univ.party.in_boat].loc = univ.party.p_loc;
		univ.party.boats[univ.party.in_boat].sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
		univ.party.boats[univ.party.in_boat].sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
	}
}

bool outd_move_party(location destination,bool forced) {
	short boat_num,horse_num,spec_num;
	location real_dest, sector_p_in;
	bool keep_going = true,check_f;
	location store_corner,store_iwc;
	ter_num_t ter;
	
	keep_going = check_special_terrain(destination,eSpecCtx::OUT_MOVE,univ.party[0],&spec_num,&check_f);
	if(check_f)
		forced = true;
	if(in_scen_debug && ghost_mode)
		forced = true;
	if(spec_num == 50)
		forced = true;
	
	// If not blocked and not put in town by a special, process move
	if(keep_going && overall_mode == MODE_OUTDOORS) {
		
		real_dest.x = destination.x - univ.party.p_loc.x;
		real_dest.y = destination.y - univ.party.p_loc.y;
		
		sector_p_in.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
		sector_p_in.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
		
		store_corner = univ.party.outdoor_corner;
		store_iwc = univ.party.i_w_c;
		
		// Check if party moves into new sector
		if((destination.x < 6) && (univ.party.outdoor_corner.x > 0))
			shift_universe_left();
		if(destination.x > 90 && univ.party.outdoor_corner.x < univ.scenario.outdoors.width() - 1)
			shift_universe_right();
		if((destination.y < 6)  && (univ.party.outdoor_corner.y > 0)) {
			shift_universe_up();
		}
		else if(destination.y > 90 && univ.party.outdoor_corner.y < univ.scenario.outdoors.height() - 1)
			shift_universe_down();
		// Now stop from going off the world's edge
		real_dest.x = univ.party.p_loc.x + real_dest.x;
		real_dest.y =univ. party.p_loc.y + real_dest.y;
		if((real_dest.x < 1 /*4*/) && (univ.party.outdoor_corner.x <= 0)) {
			ASB("You've reached the world's edge.");
			return false;
		}
		if((real_dest.x > 94 /*92*/ && univ.party.outdoor_corner.x >= univ.scenario.outdoors.width() - 2) ||
			(real_dest.x > 46 /*44*/ && univ.party.outdoor_corner.x >= univ.scenario.outdoors.width() - 1)) {
			ASB("You've reached the world's edge.");
			return false;
		}
		if((real_dest.y < 1 /*4*/)  && (univ.party.outdoor_corner.y <= 0)) {
			ASB("You've reached the world's edge.");
			return false;
		}
		else if((real_dest.y > 94 /*92*/ && univ.party.outdoor_corner.y >= univ.scenario.outdoors.height() - 2) ||
				 (real_dest.y > 46 /*44*/ && univ.party.outdoor_corner.y >= univ.scenario.outdoors.height() - 1)) {
			ASB("You've reached the world's edge.");
			return false;
		}
		
//		if((store_corner.x != party.outdoor_corner.x) || (store_corner.y != party.outdoor_corner.y) ||
//			(store_iwc.x != party.i_w_c.x) || (store_iwc.y != party.i_w_c.y))
//			clear_map();
		
		
		
//		if(forced)
//			for(i = 0; i < 10; i++)
//				if(same_point(destination,party.out_c[i].m_loc))
//					party.out_c[i].exists = false;
		
		ter = univ.out[real_dest.x][real_dest.y];
		if(univ.party.in_boat >= 0) {
			if(!outd_is_blocked(real_dest) //&& !outd_is_special(real_dest)
				// not in towns
				&& (!univ.scenario.ter_types[ter].boat_over
					|| ((real_dest.x != univ.party.p_loc.x) && (real_dest.y != univ.party.p_loc.y)))
				&& univ.scenario.ter_types[ter].special != eTerSpec::TOWN_ENTRANCE) {
				add_string_to_buf("You leave the boat.");
				univ.party.in_boat = -1;
			}
			else if(((real_dest.x != univ.party.p_loc.x) && (real_dest.y != univ.party.p_loc.y))
					 || (!forced && (out_boat_there(destination) < 30)))
				return false;
			else if(!outd_is_blocked(real_dest)
					 && univ.scenario.ter_types[ter].boat_over
					 && univ.scenario.ter_types[ter].special != eTerSpec::TOWN_ENTRANCE) {
				// TODO: It kinda looks like there should be a check for eTerSpec::BRIDGE here?
				// Note: Maybe not though, since this is where boating over lava was once hard-coded...?
				if(cChoiceDlog("boat-bridge",{"under","land"}).show() == "under")
					forced = true;
				else {
					add_string_to_buf("You leave the boat.");
					univ.party.in_boat = -1;
				}
			}
			else if(univ.scenario.ter_types[ter].boat_over)
				forced = true;
		}
		
		univ.party.direction = set_direction(univ.party.p_loc, destination);
		std::string dir_str = dir_string[univ.party.direction];
		if(((boat_num = out_boat_there(real_dest)) < 30) && (univ.party.in_boat < 0) && (univ.party.in_horse < 0)) {
			if(flying()) {
				add_string_to_buf("You land first.");
				univ.party.status[ePartyStatus::FLIGHT] = 0;
			}
			give_help(61,0);
			add_string_to_buf("Move: You board the boat.");
			univ.party.in_boat = boat_num;
			
			univ.party.p_loc = real_dest;
			univ.party.i_w_c.x = (univ.party.p_loc.x > 48) ? 1 : 0;
			univ.party.i_w_c.y = (univ.party.p_loc.y > 48) ? 1 : 0;
			univ.party.loc_in_sec = global_to_local(univ.party.p_loc);
			
			if((store_corner.x != univ.party.outdoor_corner.x) || (store_corner.y != univ.party.outdoor_corner.y) ||
				(store_iwc.x != univ.party.i_w_c.x) || (store_iwc.y != univ.party.i_w_c.y))
				clear_map();
			
			return true;
		}
		else if(((horse_num = out_horse_there(real_dest)) < 30) && (univ.party.in_boat < 0) && (univ.party.in_horse < 0)) {
			if(flying()) {
				add_string_to_buf("Land before mounting horses.");
				return false;
			}
			
			give_help(60,0);
			add_string_to_buf("Move: You mount the horses.");
			play_sound(84);
			univ.party.in_horse = horse_num;
			
			univ.party.p_loc = real_dest;
			univ.party.i_w_c.x = (univ.party.p_loc.x > 48) ? 1 : 0;
			univ.party.i_w_c.y = (univ.party.p_loc.y > 48) ? 1 : 0;
			univ.party.loc_in_sec = global_to_local(univ.party.p_loc);
			
			if((store_corner.x != univ.party.outdoor_corner.x) || (store_corner.y != univ.party.outdoor_corner.y) ||
				(store_iwc.x != univ.party.i_w_c.x) || (store_iwc.y != univ.party.i_w_c.y))
				clear_map();
			
			return true;
		}
		else if(!outd_is_blocked(real_dest) || forced
				 // Check if can fly over
				 || (flying() && univ.scenario.ter_types[ter].fly_over)) {
			if(univ.party.in_horse >= 0) {
				if(univ.scenario.ter_types[ter].special == eTerSpec::DAMAGING || univ.scenario.ter_types[ter].special == eTerSpec::DANGEROUS) {
					ASB("Your horses quite sensibly refuse.");
					return false;
				}
				if(univ.scenario.ter_types[ter].block_horse) {
					ASB("You can't take horses there!");
					return false;
				}
			}
			
			// TODO: But I though you automatically landed when entering?
			if(flying() && univ.scenario.ter_types[ter].special == eTerSpec::TOWN_ENTRANCE) {
				add_string_to_buf("Moved: You have to land first.");
				return false;
			}
			
			
			univ.party.p_loc = real_dest;
			univ.party.i_w_c.x = (univ.party.p_loc.x > 47) ? 1 : 0;
			univ.party.i_w_c.y = (univ.party.p_loc.y > 47) ? 1 : 0;
			univ.party.loc_in_sec = global_to_local(univ.party.p_loc);
			add_string_to_buf("Moved: " + dir_str);
			move_sound(univ.out[real_dest.x][real_dest.y],num_out_moves);
			num_out_moves++;
			
			if(univ.party.in_boat >= 0) { // Waterfall!!!
				run_waterfalls(1);
			}
			if(univ.party.in_horse >= 0) {
				univ.party.horses[univ.party.in_horse].which_town = 200;
				univ.party.horses[univ.party.in_horse].loc_in_sec = univ.party.loc_in_sec;
				univ.party.horses[univ.party.in_horse].loc = univ.party.p_loc;
				univ.party.horses[univ.party.in_horse].sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
				univ.party.horses[univ.party.in_horse].sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
				
			}
			
			if((store_corner.x != univ.party.outdoor_corner.x) || (store_corner.y != univ.party.outdoor_corner.y) ||
				(store_iwc.x != univ.party.i_w_c.x) || (store_iwc.y != univ.party.i_w_c.y))
				clear_map();
			
			return true;
		}
		else {
 			add_string_to_buf("Blocked: " + dir_str);
			return false;
		}
	}
	return false;
}

bool town_move_party(location destination,short forced) {
	bool keep_going = true;
	short boat_there,horse_there,spec_num;
	ter_num_t ter;
	bool check_f = false;
	
	if(univ.town.is_force_cage(univ.town.p_loc.x, univ.town.p_loc.y)) {
		add_string_to_buf("Move: Can't escape.");
		return false;
	}
	
	if(in_scen_debug && ghost_mode)
		forced = true;
	
	// remove if not registered
	/*
	if((scenario.out_width != 3) || (scenario.out_height != 3) ||
		(scenario.num_towns != 21) || (scenario.town_size[3] != 1) || (scenario.town_size[9] != 0)) {
		ASB("Blades of Exile must be registered");
		ASB("before you can play scenarios besides");
		ASB("the unmodified Valley of Dying things.");
		print_buf();
		return false;
	}
	*/
	
	if(univ.target_there(destination, TARG_MONST) == nullptr)
		keep_going = check_special_terrain(destination,eSpecCtx::TOWN_MOVE,univ.party[0],&spec_num,&check_f);
	if(check_f)
		forced = true;
	
	if(spec_num == 50)
		forced = true;
	ter = univ.town->terrain(destination.x,destination.y);
	
	if(keep_going) {
		if(univ.party.in_boat >= 0) {
			if((!is_blocked(destination)) && (!is_special(destination))
				// If to bridge, exit if heading diagonal, keep going if heading horiz or vert
				&& (!univ.scenario.ter_types[ter].boat_over
				|| ((destination.x != univ.town.p_loc.x) && (destination.y != univ.town.p_loc.y)))) {
				add_string_to_buf("You leave the boat.");
				univ.party.in_boat = -1;
			}
			else if((destination.x != univ.town.p_loc.x) && (destination.y != univ.town.p_loc.y))
				return false;
			// Crossing bridge: land or go through
			else if(!is_blocked(destination) && univ.scenario.ter_types[ter].boat_over && univ.scenario.ter_types[ter].special == eTerSpec::BRIDGE) {
				if(cChoiceDlog("boat-bridge",{"under","land"}).show() == "under")
					forced = true;
				else if(!is_blocked(destination)) {
					add_string_to_buf("You leave the boat.");
					univ.party.in_boat = -1;
				}
			}
			// boat in destination
			else if(town_boat_there(destination) < 30) {
				add_string_to_buf("  Boat there already.");
				return false;
			}
			// water or lava
			else if(univ.scenario.ter_types[ter].boat_over)
				forced = true;
		}
		
		univ.party.direction = set_direction(univ.town.p_loc, destination);
		std::string dir_str = dir_string[univ.party.direction];
		if(((boat_there = town_boat_there(destination)) < 30) && (univ.party.in_boat < 0)) {
			if(univ.party.boats[boat_there].property) {
				add_string_to_buf("  Not your boat.");
				return false;
			}
			give_help(61,0);
			add_string_to_buf("Move: You board the boat.");
			univ.party.in_boat = boat_there;
			
			univ.town.p_loc = destination;
			center = univ.town.p_loc;
			
			return true;
		}
		else if(((horse_there = town_horse_there(destination)) < 30) && (univ.party.in_horse < 0)) {
			if(univ.party.horses[horse_there].property) {
				add_string_to_buf("  Not your horses.");
				return false;
			}
			give_help(60,0);
			add_string_to_buf("Move: You mount the horses.");
			play_sound(84);
			univ.party.in_horse = horse_there;
			
			univ.town.p_loc = destination;
			center = univ.town.p_loc;
			
			return true;
		}
		else if(!is_blocked(destination) || forced) {
			if(univ.party.in_horse >= 0) {
				if(univ.scenario.ter_types[ter].special == eTerSpec::DAMAGING || univ.scenario.ter_types[ter].special == eTerSpec::DANGEROUS) {
					ASB("Your horses quite sensibly refuse.");
					return false;
				}
				if(univ.scenario.ter_types[ter].block_horse) {
					ASB("You can't take horses there!");
					return false;
				}
				if((univ.town->lighting_type > 0) && (get_ran(1,0,1) == 0)) {
					ASB("The darkness spooks your horses.");
					return false;
				}
				
			}
			univ.town.p_loc = destination;
			add_string_to_buf("Moved: " + dir_str);
//			place_treasure(destination,5,3);
			
			move_sound(univ.town->terrain(destination.x,destination.y),(short) univ.party.age);
			
			if(univ.party.in_boat >= 0) {
				// Waterfall!!!
				run_waterfalls(0);
			}
			if(univ.party.in_horse >= 0) {
				univ.party.horses[univ.party.in_horse].loc = univ.town.p_loc;
				univ.party.horses[univ.party.in_horse].which_town = univ.town.num;
			}
			center = univ.town.p_loc;
			return true;
		}
		else {
			add_string_to_buf((is_door(destination) ? "Door locked: " : "Blocked: ") + dir_str);
			return false;
		}
	}
	return false;
}



bool someone_poisoned() {
	short i;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE && (univ.party[i].status[eStatus::POISON] > 0))
			return true;
	return false;
}

short nearest_monster() {
	short i = 100,j,s;
	
	for(j = 0; j < 10; j++)
		if(univ.party.out_c[j].exists) {
			s = dist(univ.party.p_loc,univ.party.out_c[j].m_loc);
			i = min(i,s);
		}
	return i;
}

void setup_outdoors(location where) {
	update_explored(where);
}

short get_outdoor_num() {
	return (univ.scenario.outdoors.width() * (univ.party.outdoor_corner.y + univ.party.i_w_c.y) + univ.party.outdoor_corner.x + univ.party.i_w_c.x);
}

short count_walls(location loc) { // TODO: Generalize this function
	unsigned char walls[31] = {5,6,7,8,9, 10,11,12,13,14, 15,16,17,18,19, 20,21,22,23,24,
		25,26,27,28,29, 30,31,32,33,34, 35};
	short answer = 0;
	short k = 0;
	
	for(k = 0; k < 31 ; k++) {
		if(univ.out[loc.x + 1][loc.y] == walls[k])
			answer++;
		if(univ.out[loc.x - 1][loc.y] == walls[k])
			answer++;
		if(univ.out[loc.x][loc.y + 1] == walls[k])
			answer++;
		if(univ.out[loc.x][loc.y - 1] == walls[k])
			answer++;
	}
	return answer;
}

bool is_sign(ter_num_t ter) {
	
	if(univ.scenario.ter_types[ter].special == eTerSpec::IS_A_SIGN)
		return true;
	return false;
}

bool check_for_interrupt(){
	using kb = sf::Keyboard;
	bool interrupt = false;
#ifdef __APPLE__
	if((kb::isKeyPressed(kb::LSystem) || kb::isKeyPressed(kb::RSystem)) && kb::isKeyPressed(kb::Period))
		interrupt = true;
#endif
	if((kb::isKeyPressed(kb::LControl) || kb::isKeyPressed(kb::RControl)) && kb::isKeyPressed(kb::C))
		interrupt = true;
	if(interrupt) {
		// TODO: A customized dialog with a more appropriate message
		cChoiceDlog confirm("quit-confirm-nosave", {"quit","cancel"});
		if(confirm.show() == "quit") return true;
	}
	return false;
}
