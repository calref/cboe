
#include <cmath>
#include <queue>

#include "boe.global.hpp"

#include "universe.hpp"
#include "boe.actions.hpp"
#include "boe.graphutil.hpp"
#include "boe.graphics.hpp"
#include "boe.townspec.hpp"
#include "boe.fileio.hpp"
#include "boe.dlgutil.hpp"
#include "boe.locutils.hpp"
#include "boe.town.hpp"
#include "boe.text.hpp"
#include "boe.party.hpp"
#include "boe.monster.hpp"
#include "boe.specials.hpp"
#include "boe.newgraph.hpp"
#include "boe.combat.hpp"
#include "boe.items.hpp"
#include "sounds.hpp"
#include "boe.infodlg.hpp"
#include "boe.main.hpp"
#include "boe.ui.hpp"
#include "mathutil.hpp"
#include "fileio.hpp"
#include "choicedlog.hpp"
#include "dialog.hpp"
#include "scrollbar.hpp"
#include "boe.menus.hpp"
#include "winutil.hpp"
#include "cursors.hpp"
#include "spell.hpp"
#include "shop.hpp"
#include "prefs.hpp"
#include "render_shapes.hpp"
#include "enum_map.hpp"

rectangle item_screen_button_rects[9] = {
	{125,10,141,28},{125,40,141,58},{125,68,141,86},{125,98,141,116},{125,126,141,144},{125,156,141,174},
	{126,176,141,211},
	{126,213,141,248},
	{127,251,140,267}
};
rectangle medium_buttons[4] = {
	{383,190,401,225}, {402, 190, 420, 225},
	{383, 227, 401, 263}, {402, 227,420, 263}
};

enum_map(eItemButton, rectangle) item_buttons[8];
enum_map(ePlayerButton, rectangle) pc_buttons[6];
extern enum_map(eStartButton, rectangle) startup_button;
extern enum_map(eGuiArea, rectangle) win_to_rects;
extern bool flushingInput;
extern bool fog_lifted;
extern bool cartoon_happening;
rectangle startup_top;

enum_map(eItemButton, bool) item_area_button_active[8];
enum_map(ePlayerButton, bool) pc_area_button_active[6];
short item_bottom_button_active[9] = {0,0,0,0,0, 0,1,1,1};

rectangle pc_help_button;

short current_terrain_type = 0,num_out_moves = 0;
short store_drop_item;
short current_switch = 6;
cOutdoors::cWandering store_wandering_special;

short store_selling_values[8] = {0,0,0,0,0,0,0,0};
extern cShop active_shop;

extern short cen_x, cen_y;//,pc_moves[6];
extern eGameMode overall_mode;
extern eItemWinMode stat_window;
extern location	to_create;
extern bool All_Done,spell_forced,monsters_going;
extern bool party_in_memory;
extern sf::View mainView;

// game info globals
extern sf::RenderWindow mainPtr;
extern short which_item_page[6];
extern short store_spell_target,pc_casting;
extern eSpell store_mage, store_priest;
extern std::vector<int> spec_item_array;
extern cUniverse univ;
extern std::vector<word_rect_t> talk_words;
extern bool talk_end_forced;

// combat globals


extern short which_combat_type,num_targets_left;
extern location center;
extern short combat_active_pc;
extern eStatMode stat_screen_mode;

extern bool map_visible;
extern sf::RenderWindow mini_map;

extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern short shop_identify_cost;


const char *dir_string[] = {"North", "NorthEast", "East", "SouthEast", "South", "SouthWest", "West", "NorthWest"};
char get_new_terrain();
cCreature save_monster_type;

short wand_loc_count = 0;
short monst_place_count = 0; // 1 - standard place	2 - place last

enum_map(eShopArea, rectangle) shopping_rects[8];
std::queue<pending_special_type> special_queue;
bool end_scenario = false;

static void advance_time(bool did_something, bool need_redraw, bool need_reprint);

// This is defined in pc.editors.cpp since the PC editor also uses it
extern void edit_stuff_done();

void init_screen_locs() {
	rectangle startup_base = {281,1,329,302};
	rectangle shop_base = {63,12,99,267};
	
	std::fill(shopping_rects[0].begin(), shopping_rects[0].end(), shop_base);
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
	for(short i = 1; i < 8; i++)
		for(auto& j : shopping_rects[i].keys()) {
			shopping_rects[i][j] = shopping_rects[0][j];
			shopping_rects[i][j].offset(0,i * 36);
		}
	
	
	for(auto btn : startup_button.keys()) {
		startup_button[btn] = startup_base;
		startup_button[btn].offset(301 * (btn / 3), 48 * (btn % 3));
	}
	startup_top.top = 7;
	startup_top.bottom = startup_button[STARTBTN_LOAD].top;
	startup_top.left = startup_base.left;
	startup_top.right = startup_button[STARTBTN_JOIN].right;
	
	// icon, name, use, give, drip, info, sell/id   each one 13 down
	item_buttons[0][ITEMBTN_ICON].top = 15;
	item_buttons[0][ITEMBTN_ICON].bottom = item_buttons[0][ITEMBTN_ICON].top + 18;
	item_buttons[0][ITEMBTN_ICON].left = 20;
	item_buttons[0][ITEMBTN_ICON].right = item_buttons[0][ITEMBTN_ICON].left + 18;
	item_buttons[0][ITEMBTN_NAME].top = 17;
	item_buttons[0][ITEMBTN_NAME].bottom = item_buttons[0][ITEMBTN_NAME].top + 12;
	item_buttons[0][ITEMBTN_NAME].left = 3;
	item_buttons[0][ITEMBTN_NAME].right = item_buttons[0][ITEMBTN_NAME].left + 185;
	item_buttons[0][ITEMBTN_USE] = item_buttons[0][ITEMBTN_NAME];
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
	item_buttons[0][ITEMBTN_NAME].top += 3;
	for(short i = 1; i < 8; i++)
		for(auto j : item_buttons[i].keys()) {
			item_buttons[i][j] = item_buttons[0][j];
			item_buttons[i][j].offset(0,13 * i);
		}
	
/*	for(short i = 0; i < 8; i++) {
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
	pc_buttons[0][PCBTN_NAME].top += 3;
	for(short i = 1; i < 6; i++)
		for(auto j : pc_buttons[i].keys()) {
			pc_buttons[i][j] = pc_buttons[0][j];
			pc_buttons[i][j].offset(0,13 * i);
		}
	pc_help_button.top = 101;
	pc_help_button.bottom = 114;
	pc_help_button.left = 251;
	pc_help_button.right = 267;
}

bool prime_time() {
	return overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT;
}

static void handle_spellcast(eSkill which_type, bool& did_something, bool& need_redraw, bool& need_reprint) {
	short store_sp[6];
	extern short spec_target_fail;
	extern eSpecCtxType spec_target_type;
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
		extern eSpell town_spell;
		if(town_spell == eSpell::NONE)
			queue_special(eSpecCtx::TARGET, spec_target_type, spec_target_fail, univ.party.town_loc);
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
		center = univ.current_pc().combat_pos;
		pause(10);
		need_redraw = true;
		extern eSpell spell_being_cast;
		if(spell_being_cast == eSpell::NONE)
			queue_special(eSpecCtx::TARGET, spec_target_type, spec_target_fail, univ.current_pc().combat_pos);
	}
	put_pc_screen();
	put_item_screen(stat_window);
}

static void handle_begin_look(bool& need_redraw) {
	if(overall_mode == MODE_OUTDOORS) overall_mode = MODE_LOOK_OUTDOORS;
	if(overall_mode == MODE_TOWN) overall_mode = MODE_LOOK_TOWN;
	if(overall_mode == MODE_COMBAT) overall_mode = MODE_LOOK_COMBAT;
	add_string_to_buf("Look: Select a space. You can also right click to look.", 2);
	need_redraw = true;
}

static void handle_begin_talk(bool& need_reprint) {
	overall_mode = MODE_TALK_TOWN;
	add_string_to_buf("Talk: Select someone.");
	need_reprint = true;
}

static void handle_stand_ready(bool& need_redraw, bool& need_reprint) {
	need_reprint = true;
	need_redraw = true;
	//draw_terrain();
	//pause(2);
	univ.cur_pc++;
	combat_next_step();
	set_stat_window_for_pc(univ.cur_pc);
	put_pc_screen();
}

static void handle_parry(bool& did_something, bool& need_redraw, bool& need_reprint) {
	add_string_to_buf("Parry.");
	char_parry();
	did_something = true;
	need_reprint = true;
	need_redraw = true;
}

static void handle_toggle_active(bool& need_reprint) {
	if(combat_active_pc == 6) {
		add_string_to_buf("This PC now active.");
		combat_active_pc = univ.cur_pc;
	} else {
		add_string_to_buf("All PC's now active.");
		univ.cur_pc = combat_active_pc;
		combat_active_pc = 6;
	}
	need_reprint = true;
}

static void handle_rest(bool& need_redraw, bool& need_reprint) {
	sf::Event dummy_evt;
	int i = 0;
	ter_num_t ter = univ.out[univ.party.out_loc.x][univ.party.out_loc.y];
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
		if(univ.current_pc().status[eStatus::WEBS] > 0) {
			add_string_to_buf("You clean webs.");
			move_to_zero(univ.current_pc().status[eStatus::WEBS]);
			move_to_zero(univ.current_pc().status[eStatus::WEBS]);
			put_pc_screen();
		}
		check_fields(univ.current_pc().combat_pos,eSpecCtx::COMBAT_MOVE,univ.current_pc());
	} else {
		add_string_to_buf("Pause.");
		for(cPlayer& pc : univ.party)
			if(pc.main_status == eMainStatus::ALIVE && pc.status[eStatus::WEBS] > 0) {
				add_string_to_buf(pc.name + " cleans webs.");
				move_to_zero(pc.status[eStatus::WEBS]);
				move_to_zero(pc.status[eStatus::WEBS]);
			}
		if(univ.party.in_horse >= 0) {
			cVehicle& horse = univ.party.horses[univ.party.in_horse];
			if(overall_mode == MODE_OUTDOORS) {
				horse.which_town = 200;
				horse.loc = global_to_local(univ.party.out_loc);
				horse.sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
				horse.sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
				univ.party.in_horse = -1;
			} else if(overall_mode == MODE_TOWN){
				horse.loc = univ.party.town_loc;
				horse.which_town = univ.party.town_num;
				univ.party.in_horse = -1;
			}
		}
		if(univ.party.in_boat >= 0) {
			// If you pause on a bridge or other passable terrain, leave boat.
			cVehicle& boat = univ.party.boats[univ.party.in_boat];
			if(overall_mode == MODE_OUTDOORS && !impassable(univ.out[univ.party.out_loc.x][univ.party.out_loc.y])) {
				boat.which_town = 200;
				boat.loc = global_to_local(univ.party.out_loc);
				boat.sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
				boat.sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
				univ.party.in_boat = -1;
			} else if(overall_mode == MODE_TOWN && !impassable(univ.town->terrain(univ.party.town_loc.x,univ.party.town_loc.y))) {
				boat.loc = univ.party.town_loc;
				boat.which_town = univ.party.town_num;
				univ.party.in_boat = -1;
			}
		} else {
			// The above could leave you stranded in a single-tile passable area, so pausing again should re-enter the boat.
			cVehicle* boat = nullptr;
			if(overall_mode == MODE_OUTDOORS && (boat = out_boat_there(univ.party.out_loc)))
				univ.party.in_boat = boat - &univ.party.boats[0];
			else if(overall_mode == MODE_TOWN && (boat = town_boat_there(univ.party.town_loc)))
				univ.party.in_boat = boat - &univ.party.boats[0];
			if(boat)
				ASB("You board the boat.");
		}
		put_pc_screen();
		check_fields(univ.party.town_loc,eSpecCtx::TOWN_MOVE,univ.party[0]);
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
	else if(overall_mode == MODE_LOOK_COMBAT && can_see_light(univ.current_pc().combat_pos,destination,sight_obscurity) >= 4)
		add_string_to_buf("  Can't see space.");
	else {
		add_string_to_buf("You see...");
		ter_num_t ter_looked_at = do_look(destination);
		if(overall_mode == MODE_LOOK_TOWN || overall_mode == MODE_LOOK_COMBAT)
			if(adjacent(univ.party.town_loc,destination))
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
						if(adjacent(univ.town->sign_locs[k],univ.party.town_loc))
							do_sign(univ.party.town_num,k,ter_looked_at);
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
			center = univ.current_pc().combat_pos;
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
				center = univ.party.town_loc;
				update_explored(destination);
				if(loc_off_act_area(univ.party.town_loc)) {
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
			update_explored(univ.party.out_loc);
			menu_activate();
		} else need_redraw = true;
		
		ter_num_t storage = univ.out[univ.party.out_loc.x][univ.party.out_loc.y];
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
							univ.party.boats[univ.party.in_boat].which_town = univ.party.town_num;
						if(univ.party.in_horse >= 0)
							univ.party.horses[univ.party.in_horse].which_town = univ.party.town_num;
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
					short s1;
					run_special(eSpecCtx::HAIL, eSpecCtxType::TOWN, univ.town.monst[i].special_on_talk, univ.town.monst[i].cur_loc, &s1, nullptr, &need_redraw);
					if(s1 > 0)
						break;
				}
				if(!univ.town.monst[i].is_friendly()) {
					add_string_to_buf("  Creature is hostile.");
				} else if(univ.town.monst[i].summon_time > 0 || univ.town.monst[i].personality < 0) {
					short small_talk = 1;
					if(univ.town.monst[i].summon_time == 0)
						small_talk = -univ.town.monst[i].personality;
					std::string str = "No response.";
					if(small_talk > 1000 && small_talk < 1000 + univ.scenario.spec_strs.size())
						str = univ.scenario.spec_strs[small_talk - 1000];
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
			center = univ.party.town_loc;
		else center = univ.current_pc().combat_pos;
	}
	if(overall_mode != MODE_TOWN_TARGET) pause(6);
	need_redraw = true;
	if(overall_mode == MODE_TOWN_TARGET) overall_mode = MODE_TOWN;
	else if(is_combat() && overall_mode != MODE_FANCY_TARGET)
		overall_mode = MODE_COMBAT;
	put_pc_screen();
	put_item_screen(stat_window);
}

static void handle_drop_item(location destination, bool& need_redraw) {
	if(overall_mode == MODE_DROP_COMBAT) {
		if(!adjacent(univ.current_pc().combat_pos,destination))
			add_string_to_buf("Drop: must be adjacent.");
		else {
			drop_item(univ.cur_pc,store_drop_item,destination);
			take_ap(1);
		}
		pause(6);
		overall_mode = MODE_COMBAT;
	} else if(overall_mode == MODE_DROP_TOWN) {
		if(!adjacent(univ.party.town_loc,destination))
			add_string_to_buf("Drop: must be adjacent.");
		else if(sight_obscurity(destination.x,destination.y) == 5)
			ASB("Drop: Space is blocked.");
		else drop_item(univ.cur_pc,store_drop_item,destination);
		overall_mode = MODE_TOWN;
	}
	need_redraw = true;
	put_pc_screen();
	put_item_screen(stat_window);
}

static void handle_use_space_select(bool& need_reprint) {
	if(overall_mode == MODE_TOWN) {
		add_string_to_buf("Use: Select a space or item.");
		add_string_to_buf("  (Hit button again to cancel.)");
		need_reprint = true;
		overall_mode = MODE_USE_TOWN;
	} else if(overall_mode == MODE_USE_TOWN) {
		overall_mode = MODE_TOWN;
		need_reprint = true;
		add_string_to_buf("  Cancelled.");
	}
}

static void handle_use_space(location destination, bool& did_something, bool& need_redraw) {
	if(!adjacent(destination,univ.party.town_loc))
		add_string_to_buf("  Must be adjacent.");
	else did_something = use_space(destination);
	overall_mode = MODE_TOWN;
	need_redraw = true;
	put_pc_screen();
	put_item_screen(stat_window);
}

static void handle_bash_pick_select(bool& need_reprint, bool isBash) {
	if(overall_mode == MODE_BASH_TOWN || overall_mode == MODE_PICK_TOWN) {
		add_string_to_buf("  Cancelled.");
		overall_mode = MODE_TOWN;
	} else {
		overall_mode = isBash ? MODE_BASH_TOWN : MODE_PICK_TOWN;
		add_string_to_buf(isBash ? "Bash Door: Select a space." : "Pick Lock: Select a space.");
	}
	need_reprint = true;
}

static void handle_bash_pick(location destination, bool& did_something, bool& need_redraw, bool isBash) {
	if(!adjacent(destination,univ.party.town_loc))
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
	put_item_screen(stat_window);
}

static void handle_switch_pc(short which_pc, bool& need_redraw, bool& need_reprint) {
	cPlayer& pc = univ.party[which_pc];
	if(!prime_time() && overall_mode != MODE_SHOPPING && overall_mode != MODE_TALKING)
		add_string_to_buf("Set active: Finish what you're doing first.");
	else if(is_combat()) {
		if(pc.ap > 0) {
			draw_terrain();
			univ.cur_pc = which_pc;
			combat_next_step();
			set_stat_window_for_pc(univ.cur_pc);
			put_pc_screen();
		} else add_string_to_buf("Set active: PC has no APs.");
	} else if(pc.main_status != eMainStatus::ALIVE && (overall_mode != MODE_SHOPPING || active_shop.getType() != eShopType::ALLOW_DEAD))
		add_string_to_buf("Set active: PC must be here & active.");
	else {
		univ.cur_pc = which_pc;
		set_stat_window_for_pc(which_pc);
		add_string_to_buf("Now " + std::string(overall_mode == MODE_SHOPPING ? "shopping" : "active") + ": " + pc.name);
		adjust_spell_menus();
		need_redraw = true;
	}
	need_reprint = true;
}

static void handle_switch_pc_items(short which_pc, bool& need_redraw) {
	cPlayer& pc = univ.party[which_pc];
	if(!prime_time() && overall_mode != MODE_TALKING && overall_mode != MODE_SHOPPING)
		add_string_to_buf("Set active: Finish what you're doing first.");
	else {
		if(!is_combat()) {
			if(pc.main_status != eMainStatus::ALIVE && (overall_mode != MODE_SHOPPING || active_shop.getType() != eShopType::ALLOW_DEAD))
				add_string_to_buf("Set active: PC must be here & active.");
			else {
				univ.cur_pc = which_pc;
				add_string_to_buf("Now active: " + pc.name);
				adjust_spell_menus();
				need_redraw = true;
			}
		}
		set_stat_window_for_pc(which_pc);
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
	if(overall_mode == MODE_DROP_TOWN || overall_mode == MODE_DROP_COMBAT) {
		add_string_to_buf("Drop item: Cancelled");
		overall_mode = is_town() ? MODE_TOWN : MODE_COMBAT;
	} else if(!prime_time())
		add_string_to_buf("Drop item: Finish what you're doing first.");
	else if(is_out())
		drop_item(stat_window,item_hit,univ.party.out_loc);
	else {
		add_string_to_buf("Drop item: Click where to drop item.");
		store_drop_item = item_hit;
		overall_mode = is_town() ? MODE_DROP_TOWN : MODE_DROP_COMBAT;
		need_redraw = true;
	}
}

static void handle_item_shop_action(short item_hit) {
	long i = item_hit - item_sbar->getPosition();
	cPlayer& shopper = univ.party[stat_window];
	cItem& target = shopper.items[item_hit];
	switch(stat_screen_mode) {
		case MODE_IDENTIFY:
			if(!take_gold(shop_identify_cost,false))
				ASB("Identify: You don't have the gold.");
			else {
				play_sound(68);
				ASB("Your item is identified.");
				target.ident = true;
				shopper.combine_things();
			}
			break;
		case MODE_SELL_WEAP: case MODE_SELL_ARMOR: case MODE_SELL_ANY:
			play_sound(-39);
			univ.party.gold += store_selling_values[i];
			ASB("You sell your item.");
			shopper.take_item(item_hit);
			put_item_screen(stat_window);
			break;
		case MODE_ENCHANT:
			if(!take_gold(store_selling_values[i],false))
				ASB("Enchant: You don't have the gold.");
			else {
				play_sound(51);
				ASB("Your item is now enchanted.");
				eEnchant ench = eEnchant(shop_identify_cost);
				target.enchant_weapon(ench,store_selling_values[i]);
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
	std::vector<short> store_hp;
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
		for(cPlayer& pc : univ.party) {
			store_hp.push_back(pc.cur_health);
			pc.status[eStatus::WEBS] = 0;
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
			// TODO: It's not good for current_pc to be 6
			univ.cur_pc = 6;
			did_something = true;
			put_pc_screen();
		}
	} else if(overall_mode == MODE_COMBAT) {
		if(which_combat_type == 0) {
			if(hit_end_c_button()) {
				end_town_mode(0,univ.party.town_loc);
				play_sound(93);
				add_string_to_buf("End combat.");
				handle_wandering_specials(1);
				menu_activate();
				put_pc_screen();
				set_stat_window_for_pc(univ.cur_pc);
			} else add_string_to_buf("Can't end combat yet.");
		} else {
			eDirection dir = end_town_combat();
			if(dir == DIR_HERE) {
				add_string_to_buf("Failed to end combat.");
				need_reprint = true;
				return;
			}
			univ.party.direction = dir;
			center = univ.party.town_loc;
			set_stat_window_for_pc(univ.cur_pc);
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
		center = univ.current_pc().combat_pos;
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
		j = get_item(univ.party.town_loc,6,false);
	else {
		j = get_item(univ.current_pc().combat_pos,univ.cur_pc,false);
		take_ap(4);
	}
	if(j > 0) {
		put_item_screen(stat_window);
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
	for(cPlayer& pc : univ.party)
		if(pc.main_status == eMainStatus::FLED)
			pc.main_status = eMainStatus::ALIVE;
	if(is_combat() && univ.party.is_alive()) {
		// TODO: Should this only happen in outdoor combat? Or should we allow fleeing town during combat?
		end_town_mode(0,univ.party.town_loc);
		add_string_to_buf("End combat.");
		handle_wandering_specials(2);
	}
	if(!univ.party.is_alive() && univ.party.is_split()) {
		univ.party.end_split(0);
		if(univ.party.left_in == size_t(-1) || univ.party.town_num == univ.party.left_in) {
			univ.party.town_loc = univ.party.left_at;
			update_explored(univ.party.town_loc);
			center = univ.party.town_loc;
		} else change_level(univ.party.left_in, univ.party.left_at.x, univ.party.left_at.y);
		update_explored(univ.party.town_loc);
		center = univ.party.town_loc;
		if(is_combat()) overall_mode = MODE_TOWN;
		center = univ.party.town_loc;
	}
	menu_activate();
	draw_terrain();
	put_pc_screen();
	put_item_screen(stat_window);
	if(!univ.party.is_alive()) {
		play_sound(13);
		handle_death();
	}
}

bool handle_action(const sf::Event& event) {
	long item_hit;
	bool are_done = false;
	bool need_redraw = false, did_something = false, need_reprint = false;
	location loc_in_sec,cur_direction;
	bool right_button = event.mouseButton.button == sf::Mouse::Right;
	eGameMode previous_mode;
	rectangle world_screen = win_to_rects[WINRECT_TERVIEW];
	rectangle terrain_viewport = world_screen;
	world_screen.inset(13, 13);
	
	std::ostringstream str;
	location point_in_area;
	
	location the_point(event.mouseButton.x, event.mouseButton.y);
	the_point = mainPtr.mapPixelToCoords(the_point, mainView);
	end_scenario = false;
	
	// MARK: First, figure out where party is
	if(overall_mode == MODE_STARTUP || overall_mode == MODE_RESTING) {
		// If we get here during these modes, something is probably not right, so bail out
		add_string_to_buf("Unexpected game state!");
		return are_done;
	}
	
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

	// Otherwise they're in a terrain view mode
	location cur_loc = is_out() ? univ.party.out_loc : center;
	auto button_hit = UI::toolbar.button_hit(mainPtr, the_point);

	// MARK: Then, handle a button being hit.
		switch(button_hit) {
			case TOOLBAR_NONE: break;
			case TOOLBAR_MAGE: case TOOLBAR_PRIEST:
				handle_spellcast(button_hit == TOOLBAR_MAGE ? eSkill::MAGE_SPELLS : eSkill::PRIEST_SPELLS, did_something, need_redraw, need_reprint);
				break;
				
			case TOOLBAR_LOOK:
				handle_begin_look(need_redraw);
				break;
				
			case TOOLBAR_SHIELD:
				if(overall_mode == MODE_COMBAT)
					handle_parry(did_something, need_redraw, need_reprint);
				break;
				
			case TOOLBAR_TALK:
				if(overall_mode == MODE_TOWN)
					handle_begin_talk(need_reprint);
				break;
				
			case TOOLBAR_CAMP:
				if(overall_mode == MODE_OUTDOORS)
					handle_rest(need_redraw, need_reprint);
				break;
				
			case TOOLBAR_SCROLL: case TOOLBAR_MAP:
				if(overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN)
					display_map();
				break;
				
			case TOOLBAR_BAG: case TOOLBAR_HAND:
				if(overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT)
					handle_get_items(did_something, need_redraw, need_reprint);
				break;
				
			case TOOLBAR_SAVE:
				if(overall_mode == MODE_OUTDOORS) {
					save_party(univ.file, univ);
					need_redraw = true;
					current_switch = 6;
					break;
				}
				break;
				
			case TOOLBAR_USE:
				if(overall_mode == MODE_TOWN || overall_mode == MODE_USE_TOWN)
					handle_use_space_select(need_reprint);
				break;
				
			case TOOLBAR_WAIT:
				if(overall_mode == MODE_COMBAT) {
					handle_stand_ready(need_redraw, need_reprint);
				}
				break;
				
			case TOOLBAR_LOAD:
				if(overall_mode == MODE_OUTDOORS)
					do_load();
				break;
				
			case TOOLBAR_SHOOT:
				if(overall_mode == MODE_COMBAT || overall_mode == MODE_FIRING || overall_mode == MODE_THROWING)
					handle_missile(need_redraw, need_reprint);
				break;
				
			case TOOLBAR_SWORD: case TOOLBAR_END:
				handle_combat_switch(did_something, need_redraw, need_reprint);
				break;
				
			case TOOLBAR_ACT:
				if(overall_mode == MODE_COMBAT) {
					handle_toggle_active(need_reprint);
				}
				break;
		}
	
	// MARK: Begin: click in terrain
	if(the_point.in(world_screen) && (is_out() || is_town() || is_combat())){
		int i = (the_point.x - 32) / 28;
		int j = (the_point.y - 20) / 36;
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
					center = univ.current_pc().combat_pos;
					pause(5);
					need_redraw = true;
				}
				else if(overall_mode == MODE_LOOK_TOWN) {
					overall_mode = MODE_TOWN;
					center = univ.party.town_loc;
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
		else if(overall_mode == MODE_BASH_TOWN || overall_mode == MODE_PICK_TOWN) {
			destination.x += i - 4;
			destination.y += j - 4;
			handle_bash_pick(destination, did_something, need_redraw, overall_mode == MODE_BASH_TOWN);
		}
	}
	// MARK: End: click in terrain
	
	// MARK: Begin: Screen shift
	if(scrollableModes.count(overall_mode) && the_point.in(terrain_viewport) && !the_point.in(world_screen)) {
		if(the_point.y < world_screen.top && center.y > univ.town->in_town_rect.top && center.y > 4) {
			center.y--;
			need_redraw = true;
		}
		if(the_point.x < world_screen.left && center.x > univ.town->in_town_rect.left && center.x > 4) {
			center.x--;
			need_redraw = true;
		}
		if(the_point.y > world_screen.bottom && center.y < univ.town->in_town_rect.bottom && center.y < univ.town->max_dim - 5) {
			center.y++;
			need_redraw = true;
		}
		if(the_point.x > world_screen.right && center.x < univ.town->in_town_rect.right && center.x < univ.town->max_dim - 5) {
			center.x++;
			need_redraw = true;
		}
	}
	// MARK: End: Screen shift
	
	// MARK: Process clicks in PC stats area
	if(the_point.in(win_to_rects[WINRECT_PCSTATS])) {
		location pc_win_ul = win_to_rects[WINRECT_PCSTATS].topLeft();
		point_in_area = the_point;
		point_in_area.x -= pc_win_ul.x;
		point_in_area.y -= pc_win_ul.y;
		if(point_in_area.in(pc_help_button)) {
			rectangle help_button = pc_help_button;
			help_button.offset(pc_win_ul);
			arrow_button_click(help_button);
			cChoiceDlog("help-party").show();
		}
		for(int i = 0; i < 6; i++)
			for(auto j : pc_buttons[i].keys())
				if(pc_area_button_active[i][j] && point_in_area.in(pc_buttons[i][j])) {
					cPlayer& pc = univ.party[i];
					if((j == PCBTN_HP || j == PCBTN_SP) && !pc.is_alive())
						break;
					rectangle button_rect = pc_buttons[i][j];
					button_rect.offset(pc_win_ul);
					arrow_button_click(button_rect);
					switch(j) {
						case PCBTN_NAME:
							handle_switch_pc(i, need_redraw, need_reprint);
							break;
						case PCBTN_HP:
							str.str("");
							str << pc.name << " has ";
							str << pc.cur_health << " health out of " << pc.max_health << '.';
							add_string_to_buf(str.str());
							break;
						case PCBTN_SP:
							str.str("");
							str << pc.name << " has ";
							str << pc.cur_health << " spell pts. out of " << pc.max_health << '.';
							add_string_to_buf(str.str());
							break;
						case PCBTN_INFO:
							give_pc_info(i);
							break;
						case PCBTN_TRADE:
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
		put_item_screen(stat_window);
		if(overall_mode == MODE_SHOPPING) {
			set_up_shop_array();
			draw_shop_graphics(0,pc_buttons[0][PCBTN_NAME]);
		}
	}
	
	// Process clicks in item stats area
	if(the_point.in(win_to_rects[WINRECT_INVEN])) {
		location item_win_ul = win_to_rects[WINRECT_INVEN].topLeft();
		point_in_area = the_point;
		point_in_area.x -= item_win_ul.x;
		point_in_area.y -= item_win_ul.y;
		
		for(int i = 0; i < 9; i++)
			if(item_bottom_button_active[i] > 0 && point_in_area.in(item_screen_button_rects[i])) {
				rectangle button_rect = item_screen_button_rects[i];
				button_rect.offset(item_win_ul);
				arrow_button_click(button_rect);
				switch(i) {
					case 6: // special screen
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
				for(auto j : item_buttons[i].keys())
					if(item_area_button_active[i][j] && point_in_area.in(item_buttons[i][j])) {
						rectangle button_rect = item_buttons[i][j];
						button_rect.offset(item_win_ul);
						arrow_button_click(button_rect);
						
						item_hit = item_sbar->getPosition() + i;
						switch(j) {
							case ITEMBTN_NAME: case ITEMBTN_ICON: // equip
								handle_equip_item(item_hit, need_redraw);
								break;
							case ITEMBTN_USE:
								handle_use_item(item_hit, did_something, need_redraw);
								break;
							case ITEMBTN_GIVE:
								handle_give_item(item_hit, did_something, need_redraw);
								break;
							case ITEMBTN_DROP:
								if(stat_window == ITEM_WIN_SPECIAL) {
									use_spec_item(spec_item_array[item_hit]);
									need_redraw = true;
								} else handle_drop_item(item_hit, need_redraw);
								break;
							case ITEMBTN_INFO:
								if(stat_window == ITEM_WIN_SPECIAL)
									put_spec_item_info(spec_item_array[item_hit]);
								else if(stat_window == ITEM_WIN_QUESTS)
									put_quest_info(spec_item_array[item_hit]);
								else display_pc_item(stat_window, item_hit,univ.party[stat_window].items[item_hit],0);
								break;
							case ITEMBTN_SPEC: // sell? That this code was reached indicates that the item was sellable
								// (Based on item_area_button_active)
								handle_item_shop_action(item_hit);
								break;
						}
					}
		}
		put_pc_screen();
		put_item_screen(stat_window);
		need_reprint = true;
	}
	
	advance_time(did_something, need_redraw, need_reprint);
	
	are_done = All_Done;
	return are_done;
}

static void advance_time(bool did_something, bool need_redraw, bool need_reprint) {
 	// MARK: At this point, see if any specials have been queued up, and deal with them
	// Note: We just check once here instead of looping because run_special also pulls from the queue.
	if(!special_queue.empty()) {
		pending_special_type pending = special_queue.front();
		special_queue.pop();
		run_special(pending, nullptr, nullptr, &need_redraw);
	}
	
	// MARK: Handle non-PC stuff (like monsters) if the party actually did something
	if(did_something) handle_monster_actions(need_redraw, need_reprint);
	if(fog_lifted) need_redraw = true;
	if(cartoon_happening) {
		need_redraw = true;
		if(!is_combat()) {
			for(cPlayer& pc : univ.party)
				pc.combat_pos.x = pc.combat_pos.y = -1;
		}
	}
	fog_lifted = false;
	cartoon_happening = false;
	if(need_redraw) draw_terrain();
	if(need_reprint || need_redraw) print_buf();
	
	if(!univ.party.is_alive())
		handle_party_death();
	else if(end_scenario)
		handle_victory();
}

void handle_monster_actions(bool& need_redraw, bool& need_reprint) {
	draw_map(true);
	play_ambient_sound();
	
	if(is_combat() && overall_mode != MODE_LOOK_COMBAT) {
		if(!univ.party.is_alive()) {
			end_combat();
			if(which_combat_type == 0) {
				end_town_mode(0,univ.party.out_loc);
				add_string_to_buf("Fled the combat.");
				handle_wandering_specials(2);
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
			if(get_ran(1,1,70 + univ.party.less_wm * 200) == 10)
				create_wand_monst();
			for(int i = 0; i < 10; i++)
				if(univ.party.out_c[i].exists)
					if((adjacent(univ.party.out_loc,univ.party.out_c[i].m_loc) || univ.party.out_c[i].what_monst.forced)
					   && univ.party.in_boat < 0 && !flying()) {
						store_wandering_special = univ.party.out_c[i].what_monst;
						if(handle_wandering_specials(0))
							initiate_outdoor_combat(i);
						univ.party.out_c[i].exists = false;
						
						// Get rid of excess keyclicks
						flushingInput = true;
						
						need_reprint = false;
						i = 20;
					}
		}
		if(overall_mode == MODE_TOWN) {
			if(get_ran(1,1,160 - univ.town.difficulty + univ.party.less_wm * 200) == 2)
				create_wand_monst();
		}
	}
}

bool someone_awake() {
	for(cPlayer& pc : univ.party)
		if(pc.main_status == eMainStatus::ALIVE &&
		   pc.status[eStatus::ASLEEP] <= 0 && pc.status[eStatus::PARALYZED] <= 0)
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
	
	spell_forced = true;
	pc_casting = univ.cur_pc;
	univ.current_pc().last_cast[spell_type] = spell_picked;
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
	
	bool did_something = false, need_redraw = false, need_reprint = false;
	handle_spellcast(spell_type, did_something, need_redraw, need_reprint);
	advance_time(did_something, need_redraw, need_reprint);
}

void initiate_outdoor_combat(short i) {
	location to_place;
	
	draw_terrain();
	
	// Is combat too easy?
	if((univ.party.get_level() > ((out_enc_lev_tot(i) * 5) / 3) ) && (out_enc_lev_tot(i) < 200)
		&& !univ.party.out_c[i].what_monst.cant_flee) {
		add_string_to_buf("Combat: Monsters fled!");
		univ.party.out_c[i].exists = false;
		return;
	}
	
//	Delay((long) 100,&dummy);
	
	start_outdoor_combat(univ.party.out_c[i], univ.party.out_loc,count_walls(univ.party.out_loc));
	
	univ.party.out_c[i].exists = false;
	
	for(cPlayer& pc : univ.party)
		if(pc.main_status == eMainStatus::ALIVE)
			to_place = pc.combat_pos;
		else for(cItem& item : pc.items)
			if(item.variety != eItemType::NO_ITEM) {
				place_item(item,to_place);
				item.variety = eItemType::NO_ITEM;
			}
	
	overall_mode = MODE_COMBAT;
	center = univ.current_pc().combat_pos;
	draw_terrain();
}

bool handle_keystroke(const sf::Event& event){
	bool are_done = false;
	location pass_point; // TODO: This isn't needed
	std::ostringstream sout;
	using kb = sf::Keyboard;
	using Key = sf::Keyboard::Key;
	
	Key keypad[10] = {
		kb::Numpad0,kb::Numpad1,kb::Numpad2,kb::Numpad3,
		kb::Numpad4,kb::Numpad5,kb::Numpad6,
		kb::Numpad7,kb::Numpad8,kb::Numpad9
	};
	location terrain_click[10] = {
		{150,185},{120,215},{150,215},{180,215},
		{120,185},{150,185},{180,185},
		{120,155},{150,155},{180,135}
	};
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
	} else if(chr2 == kb::Home) chr2 = kb::Numpad7;
	else if(chr2 == kb::End) chr2 = kb::Numpad1;
	else if(chr2 == kb::PageUp) chr2 = kb::Numpad9;
	else if(chr2 == kb::PageDown) chr2 = kb::Numpad3;
	
	sf::Event pass_event = {sf::Event::MouseButtonPressed};
	if(overall_mode == MODE_TALKING) {
		if(chr2 == kb::Escape)
			chr2 = kb::D;
		if(chr2 == kb::Space)
			chr2 = kb::G;
		for(short i = 0; i < 9; i++)
			if(chr2 == talk_chars[i] && (!talk_end_forced || i == 6 || i == 5)) {
				int j = talk_end_forced ? i - 5 : i;
				pass_point = talk_words[j].rect.topLeft();
				pass_point.x += 9; pass_point.y += 9;
				pass_point = mainPtr.mapCoordsToPixel(pass_point, mainView);
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
		for(short i = 0; i < 8; i++)
			if(chr2 == shop_chars[i]) {
				pass_point = shopping_rects[i][SHOPRECT_ACTIVE_AREA].topLeft();
				pass_point.x += 9; pass_point.y += 9;
				pass_point = mainPtr.mapCoordsToPixel(pass_point, mainView);
				pass_event.mouseButton.x = pass_point.x;
				pass_event.mouseButton.y = pass_point.y;
				are_done = handle_action(pass_event);
			}
	} else {
		for(short i = 0; i < 10; i++)
			if(chr2 == keypad[i]) {
				if(i == 0) {
					chr2 = kb::Z;
				}
				else {
					pass_point = mainPtr.mapCoordsToPixel(terrain_click[i], mainView);
					pass_event.mouseButton.x = pass_point.x;
					pass_event.mouseButton.y = pass_point.y;
					are_done = handle_action(pass_event);
					return are_done;
				}
			}
	}
	
	bool did_something = false, need_redraw = false, need_reprint = false;
	
	char chr = keyToChar(chr2, event.key.shift);
	// F1 should bring up help.
	// TODO: So should the help key, if it exists (but SFML doesn't support the help key)
	if(chr2 == kb::F1) chr = '?';
	
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
				give_help(226,27);
				break;
			}
			if(overall_mode == MODE_TALKING) {
				give_help(205,6);
				break;
			}
			if(is_out()) cChoiceDlog("help-outdoor").show();
			if(is_town()) cChoiceDlog("help-town").show();
			if(is_combat()) cChoiceDlog("help-combat").show();
			break;
			
		case '1': case '2': case '3': case '4': case '5': case '6':
			handle_switch_pc(((short) chr) - 49, need_redraw, need_reprint);
			break;
			
		case '9': // Special items
			set_stat_window(ITEM_WIN_SPECIAL);
			break;
			
		case '0': // Jobs/quests
			set_stat_window(ITEM_WIN_QUESTS);
			break;
			
		case ' ':
			if(overall_mode == MODE_FANCY_TARGET) {
				// cast multi-target spell, set # targets to 0 so that space clicked doesn't matter
				num_targets_left = 0;
				handle_target_space(center, did_something, need_redraw, need_reprint);
			} else if(overall_mode == MODE_SPELL_TARGET)
				// Rotate a force wall
				spell_cast_hit_return();
			else if(overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT || overall_mode == MODE_OUTDOORS) {
				// Pause (skip turn)
				handle_pause(did_something, need_redraw);
			}
			break;
			
			
		case 'D':
			if(univ.debug_mode) {
				univ.debug_mode = false;
				ASB("Debug mode OFF.");
			} else {
				univ.debug_mode = true;
				ASB("Debug mode ON.");
			}
			print_buf();
			break;
		case 'z': // Show active PC's inventory
			if(is_combat()) {
				set_stat_window_for_pc(univ.cur_pc);
				put_item_screen(stat_window);
			} else {
				// ... or first PC's inventory... why?
				set_stat_window(ITEM_WIN_PC1);
				put_item_screen(stat_window);
			}
			break;
			
		case '=':
			if(!univ.debug_mode) break;
			univ.party.gold += 100;
			univ.party.food += 100;
			for(cPlayer& pc : univ.party) {
				pc.main_status = eMainStatus::ALIVE;
				pc.cur_health = pc.max_health;
				pc.cur_sp = 100;
			}
			award_party_xp(25);
			for(cPlayer& who : univ.party) {
				who.priest_spells.set();
				who.mage_spells.set();
			}
			univ.refresh_store_items();
			add_string_to_buf("Debug: Add stuff and heal.");
			print_buf();
			put_pc_screen();
			break;
			
		case 'B':
			if(!univ.debug_mode) break;
			if(overall_mode == MODE_OUTDOORS){
				add_string_to_buf("Debug - Leave Town: You're not in town!");
				print_buf();
				break;
			}
			univ.party.end_split(0);
			overall_mode = MODE_OUTDOORS;
			position_party(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y,univ.party.out_loc.x,univ.party.out_loc.y);
			clear_map();
			add_string_to_buf("Debug: Reunite party and leave town.");
			print_buf();
			redraw_screen(REFRESH_ALL);
			break;
			
		case 'C':
			if(!univ.debug_mode) break;
			univ.party.clear_bad_status();
			add_string_to_buf("Debug: You get cleaned up!");
			print_buf();
			put_pc_screen();
			break;
			
		case 'E':
			if(!univ.debug_mode) break;
			univ.party.status[ePartyStatus::STEALTH] += 10;
			univ.party.status[ePartyStatus::DETECT_LIFE] += 10;
			univ.party.status[ePartyStatus::FIREWALK] += 10;
			add_string_to_buf("Debug: Stealth, Detect Life, Firewalk!");
			draw_text_bar();
			print_buf();
			put_pc_screen();
			break;
			
		case 'F':
			if(!univ.debug_mode) break;
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
			if(!univ.debug_mode) break;
			if(univ.ghost_mode){
				univ.ghost_mode = false;
				ASB("Debug: Ghost mode OFF.");
			}else{
				univ.ghost_mode = true;
				ASB("Debug:Ghost mode ON.");
			}
			print_buf();
			break;
			
		case 'H':
			if(!univ.debug_mode) break;
			univ.party.gold += 100;
			univ.party.food += 100;
			for(cPlayer& pc : univ.party) {
				if(isDead(pc.main_status))
					pc.main_status = eMainStatus::ALIVE;
			}
			univ.party.heal(250);
			univ.party.restore_sp(100);
			add_string_to_buf("Debug: Heal party.");
			print_buf();
			put_pc_screen();
			break;
			
		case 'K':
			if(!univ.debug_mode) break;
			for(short i = 0; i < univ.town.monst.size(); i++) {
				if(is_combat() && univ.town.monst[i].active > 0 && !univ.town.monst[i].is_friendly())
					univ.town.monst[i].active = 0;
				
				if(univ.town.monst[i].active > 0 && !univ.town.monst[i].is_friendly()
					&& (dist(univ.town.monst[i].cur_loc,univ.party.town_loc) <= 10) )
					damage_monst(univ.town.monst[i], 7,1000,eDamageType::SPECIAL,0);
			}
			// kill_monst(&univ.town.monst[i],6);
			draw_terrain();
			add_string_to_buf("Debug: Kill things.");
			print_buf();
			break;
			
		case 'N':
			if(!univ.debug_mode) break;
			handle_victory();
			break;
			
		case 'O':
			if(!univ.debug_mode) break;
			if(is_town()) {
				sout << "Debug:  You're at x " << (short) univ.party.town_loc.x << ", y " << (short) univ.party.town_loc.y
				<< " in town " << univ.party.town_num << '.';
			} else if(is_out()) {
				short x = univ.party.out_loc.x;
				short y = univ.party.out_loc.y;
				x += 48 * univ.party.outdoor_corner.x;
				y += 48 * univ.party.outdoor_corner.y;
				sout << "Debug:  You're outside at x " << x << ", y " << y << '.';
			} else if(is_combat()) {
				location loc = univ.current_pc().combat_pos;
				sout << "Debug:  You're in combat at x " << loc.x << ", y " << loc.y << '.';
			}
			add_string_to_buf(sout.str());
			print_buf();
			break;
			
		case 'I':
			if(univ.debug_mode) {
				int i = get_num_response(0, univ.scenario.scen_items.size()-1, "Which item?");
				int j = univ.scenario.scen_items[i].ident;
				univ.scenario.scen_items[i].ident = true;
				univ.party.give_item(univ.scenario.scen_items[i], true);
				univ.scenario.scen_items[i].ident = j;
				print_buf();
				put_item_screen(stat_window);
				put_pc_screen(); // In case the item was food or gold
			}
			break;
			
		case 'Q':
			if(!univ.debug_mode) break;
			if(overall_mode == MODE_OUTDOORS) {
				for(short i = 0; i < 96; i++)
					for(short j = 0; j < 96; j++)
						make_explored(i,j);
			} else {
				for(short i = 0; i < 64; i++)
					for(short j = 0; j < 64; j++)
						make_explored(i,j);
			}
			clear_map();
			add_string_to_buf("Debug:  Magic Map.");
			print_buf();
			break;
			
		case 'R':
			if(!univ.debug_mode) break;
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
			center = univ.party.town_loc = univ.scenario.where_start;
			redraw_screen(REFRESH_ALL);
			add_string_to_buf("Debug:  You return to the start.");
			print_buf();
			break;
			
		case 'S':
			if(!univ.debug_mode) break;
			edit_stuff_done();
			break;
			
		case 'T':
			if(!univ.debug_mode) break;
			short find_direction_from;
			if(univ.party.direction == 0) find_direction_from = 2;
			else if(univ.party.direction == 4) find_direction_from = 0;
			else if(univ.party.direction < 4) find_direction_from = 3;
			else find_direction_from = 1;
			start_town_mode(get_num_response(0, univ.scenario.towns.size() - 1, "Enter Town Number"), find_direction_from);
			break;
			
		case 'W':
			if(!univ.debug_mode) break;
			univ.refresh_store_items();
			add_string_to_buf("Debug: Refreshed jobs/shops.");
			print_buf();
			break;
			
		case '<':
			if(!univ.debug_mode) break;
			ASB("Debug: Increase age.");
			ASB("  It is now 1 day later.");
			print_buf();
			univ.party.age += 3700;
			put_pc_screen();
			break;
		case '>':
			if(!univ.debug_mode) break;
			ASB("DEBUG: Towns have short memory.");
			ASB("Your deeds have been forgotten.");
			print_buf();
			for(auto& pop : univ.party.creature_save)
				pop.which_town = 200;
			break;
		case '!':
			if(!univ.debug_mode) break;
			if(univ.node_step_through) {
				univ.node_step_through = false;
				add_string_to_buf("Debug: Step-through disabled");
			} else {
				univ.node_step_through = true;
				add_string_to_buf("Debug: Step-through enabled");
			}
			print_buf();
			break;
		case '/':
			if(!univ.debug_mode) break;
			cChoiceDlog("help-debug").show();
			break;
		case 'a': // Show automap
			if(overall_mode == MODE_TOWN || overall_mode == MODE_OUTDOORS)
				display_map();
			break;
			
		case 'u': // Use space
			if(overall_mode == MODE_TOWN || overall_mode == MODE_USE_TOWN)
				handle_use_space_select(need_reprint);
			break;
			
		case 'b': // Bash door
			if(overall_mode == MODE_TOWN || overall_mode == MODE_BASH_TOWN)
				handle_bash_pick_select(need_reprint, true);
			break;
			
		case 'L': // Pick lock
			if(overall_mode == MODE_TOWN || overall_mode == MODE_PICK_TOWN)
				handle_bash_pick_select(need_reprint, false);
			break;
			
		case 'A': // Alchemy
			handle_alchemy(need_redraw, need_reprint);
			break;
			
			// Spells (cast/cancel)
		case 'M': spell_forced = true;
		case 'm':
			if(overall_mode == MODE_SPELL_TARGET || overall_mode == MODE_FANCY_TARGET || overall_mode == MODE_TOWN_TARGET || overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) {
				handle_spellcast(eSkill::MAGE_SPELLS, did_something, need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'P': spell_forced = true;
		case 'p':
			if(overall_mode == MODE_SPELL_TARGET || overall_mode == MODE_FANCY_TARGET || overall_mode == MODE_TOWN_TARGET || overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) {
				handle_spellcast(eSkill::PRIEST_SPELLS, did_something, need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'l': // Look
			if((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_TOWN) || (overall_mode == MODE_COMBAT)) {
				handle_begin_look(need_redraw);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'r': // Rest (outdoors)
			if(overall_mode == MODE_OUTDOORS) {
				handle_rest(need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 't': // Talk
			if(overall_mode == MODE_TOWN) {
				handle_begin_talk(need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'w': // Wait / delay
			if(overall_mode == MODE_TOWN) {
				handle_town_wait(need_redraw, need_reprint);
			} else if(overall_mode == MODE_COMBAT) {
				handle_stand_ready(need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			} else if(overall_mode == MODE_OUTDOORS) {
				add_string_to_buf("Wait: In town only.");
				print_buf();
				return false;
			}
			break;
			
		case 'd': // Parry
			if(overall_mode == MODE_COMBAT) {
				handle_parry(did_something, need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'g': // Get items
			if(overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) {
				handle_get_items(did_something, need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 's': // Shoot
			if(overall_mode == MODE_COMBAT || overall_mode == MODE_FIRING || overall_mode == MODE_THROWING) {
				handle_missile(need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'x': // Toggle active
			if(overall_mode == MODE_COMBAT) {
				handle_toggle_active(need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'e': // End combat
			if(overall_mode == MODE_COMBAT) {
				handle_combat_switch(did_something, need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'f': // Toggle combat
			if(overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) {
				handle_combat_switch(did_something, need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
	}
	spell_forced = false;
	return are_done;
}

bool handle_scroll(const sf::Event& event) {
	rectangle world_screen = win_to_rects[WINRECT_TERVIEW];
	world_screen.inset(13, 13);
	fill_rect(mainPtr, world_screen, sf::Color::Magenta);
	
	// TODO: centralize this translation somewhere
	location pos(event.mouseWheel.x, event.mouseWheel.y);
	pos = mainPtr.mapPixelToCoords(pos, mainView);
	
	int amount = event.mouseWheel.delta;
	if(scrollableModes.count(overall_mode) && pos.in(world_screen)) {
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
			center.x = minmax(4, univ.town->max_dim - 5, center.x - amount);
		else center.y = minmax(4, univ.town->max_dim - 5, center.y - amount);
	}
	return true;
}

void do_load() {
	fs::path file_to_load = nav_get_party();
	if(file_to_load.empty()) return;
	if(!load_party(file_to_load, univ))
		return;
	finish_load_party();
	if(overall_mode != MODE_STARTUP)
		post_load();
	univ.file = file_to_load;
	menu_activate();
}

void post_load() {
	current_switch = 6;
	
	if(!is_out())
		reset_item_max();
	
	if(overall_mode == MODE_OUTDOORS)
		update_explored(univ.party.out_loc);
//	if(overall_mode == MODE_TOWN) {
//		make_town_trim(0);
//		}
//	make_out_trim();
	
	text_sbar->show();
	item_sbar->show();
	shop_sbar->hide();
	set_stat_window(ITEM_WIN_PC1);
	put_pc_screen();
	draw_terrain();
	UI::toolbar.draw(mainPtr);
	draw_text_bar();
	
	print_buf();
	
	clear_map();
	adjust_spell_menus();
	adjust_monst_menu();
}

//mode; // 0 - normal  1 - save as
void do_save(short mode) {
	if(overall_mode != MODE_TOWN && overall_mode != MODE_OUTDOORS && overall_mode != MODE_STARTUP) {
		add_string_to_buf("Save: Only while outdoors, or in town and not looking/casting.", 2);
		print_buf();
		return;
	}
	fs::path file = univ.file;
	if(mode == 1 || file.empty())
		file = nav_put_party(file);
	bool saved = false;
	if(!file.empty()) {
		univ.file = file;
		saved = save_party(univ.file, univ);
	}
	
	if(saved)
		add_string_to_buf("Save: Game saved");
	
	pause(6);
	redraw_screen(REFRESH_TEXT);
}

void do_rest(long length, int hp_restore, int mp_restore) {
	unsigned long age_before = univ.party.age;
	univ.party.age += length;
	if(univ.scenario.is_legacy) {
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
	for(cPlayer& pc : univ.party)
		pc.status.clear();
	// Specials countdowns
	if((length > 500 || age_before / 500 < univ.party.age / 500) && univ.party.has_abil(eItemAbil::OCCASIONAL_STATUS)) {
		// TODO: There used to be a "display strings" here; should we hook in a special node call?
		for(cPlayer& pc : univ.party)
			for(const cItem& item : pc.items) {
				if(item.ability != eItemAbil::OCCASIONAL_STATUS) continue;
				if(item.abil_data[1] > 15) continue;
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
		univ.refresh_store_items();
	// Heal party
	univ.party.heal(hp_restore);
	univ.party.restore_sp(mp_restore);
	// Recuperation and chronic disease disads
	for(cPlayer& pc : univ.party)
		if(pc.main_status == eMainStatus::ALIVE) {
			if(pc.traits[eTrait::RECUPERATION] && pc.cur_health < pc.max_health) {
				pc.heal(hp_restore / 5);
			}
			if(pc.traits[eTrait::CHRONIC_DISEASE] && get_ran(1,0,110) == 1) {
				pc.disease(6);
			}
			cInvenSlot item = pc.has_abil_equip(eItemAbil::REGENERATE);
			if(item && pc.cur_health < pc.max_health && (overall_mode != MODE_OUTDOORS || get_ran(1,0,10) == 5)){
				int j = get_ran(1,0,item->abil_data[0] / 3);
				if(item->abil_data[0] / 3 == 0)
					j = get_ran(1,0,1);
				if(is_out()) j = j * 4;
				pc.heal(j);
			}
			// Bonus SP and HP wear off
			if(pc.cur_sp > pc.max_sp)
				pc.cur_sp = pc.max_sp;
			if(pc.cur_health > pc.max_health)
				pc.cur_health = pc.max_health;
		}
	special_increase_age(length, true);
	put_pc_screen();
	adjust_spell_menus();
}

void increase_age() {
	short how_many_short = 0,r1;
	
	
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
	move_to_zero(univ.party.hostiles_present);
	
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
		if(univ.scenario.ter_types[univ.out[univ.party.out_loc.x][univ.party.out_loc.y]].blocksMove()) {
			add_string_to_buf("  You plummet to your deaths.");
			slay_party(eMainStatus::DEAD);
			print_buf();
			pause(150);
		}
		else add_string_to_buf("  You land safely.");
	}
	
	move_to_zero(univ.party.status[ePartyStatus::FLIGHT]);
	
	if(overall_mode != MODE_OUTDOORS && univ.town->lighting_type >= LIGHT_DRAINS) {
		increase_light(-9);
		if(univ.town->lighting_type == LIGHT_NONE) {
			if(univ.party.light_level > 0)
				ASB("Your light is drained.");
			univ.party.light_level = 0;
		}
	}
	
	if(is_town() && univ.town->lighting_type != LIGHT_NORMAL) {
		int radiance = 0;
		for(cPlayer& pc : univ.party)
			radiance += pc.get_prot_level(eItemAbil::RADIANT);
		if(radiance > 0 && univ.party.light_level < radiance && get_ran(1,1,10) < radiance) {
			ASB("One of your items is glowing softly!");
			univ.party.light_level += radiance * 3;
		}
	}
	
	// Specials countdowns
	if(univ.party.age % 500 == 0 && univ.party.has_abil(eItemAbil::OCCASIONAL_STATUS)) {
		// TODO: There used to be a "display strings" here; should we hook in a special node call?
		for(cPlayer& pc : univ.party)
			for(const cItem& item : pc.items) {
				if(item.ability != eItemAbil::OCCASIONAL_STATUS) continue;
				if(item.abil_data[1] > 15) continue;
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
		univ.refresh_store_items();
	}
	
	// Protection, etc.
	for(cPlayer& pc : univ.party) { // Process some status things, and check if stats updated
		
		if(pc.status[eStatus::INVULNERABLE] == 1 || abs(pc.status[eStatus::MAGIC_RESISTANCE]) == 1
		   || pc.status[eStatus::INVISIBLE] == 1 || pc.status[eStatus::MARTYRS_SHIELD] == 1
		   || abs(pc.status[eStatus::ASLEEP]) == 1 || pc.status[eStatus::PARALYZED] == 1)
		move_to_zero(pc.status[eStatus::INVULNERABLE]);
		move_to_zero(pc.status[eStatus::MAGIC_RESISTANCE]);
		move_to_zero(pc.status[eStatus::INVISIBLE]);
		move_to_zero(pc.status[eStatus::MARTYRS_SHIELD]);
		move_to_zero(pc.status[eStatus::ASLEEP]);
		move_to_zero(pc.status[eStatus::PARALYZED]);
		if(univ.party.age % 40 == 0 && pc.status[eStatus::POISONED_WEAPON] > 0) {
			move_to_zero(pc.status[eStatus::POISONED_WEAPON]);
		}
		
	}
	
	// Food
	if(univ.party.age % 1000 == 0 && !is_combat()) {
		for(cPlayer& pc : univ.party)
			if(pc.main_status == eMainStatus::ALIVE)
				how_many_short++;
		how_many_short = take_food (how_many_short,false);
		if(how_many_short > 0) {
			add_string_to_buf("Starving!");
			play_sound(66);
			r1 = get_ran(3,1,6);
			hit_party(r1,eDamageType::SPECIAL);
			// Might seem redudant but maybe hit_party could change the mode if TPK?
			if(!is_combat())
				boom_space(univ.party.out_loc,overall_mode,0,r1,0);
		}
		else {
			play_sound(6);
			add_string_to_buf("You eat.");
		}
	}
	
	// Poison, acid, disease damage
	for(cPlayer& pc : univ.party) // Poison
		if(pc.status[eStatus::POISON] > 0) {
			if(((overall_mode == MODE_OUTDOORS) && (univ.party.age % 50 == 0)) || ((overall_mode == MODE_TOWN) && (univ.party.age % 20 == 0))) {
				do_poison();
			}
			break;
		}
	for(cPlayer& pc : univ.party) // Disease
		if(pc.status[eStatus::DISEASE] > 0) {
			if(((overall_mode == MODE_OUTDOORS) && (univ.party.age % 100 == 0)) || ((overall_mode == MODE_TOWN) && (univ.party.age % 25 == 0))) {
				handle_disease();
			}
			break;
		}
	for(cPlayer& pc : univ.party) // Acid
		if(pc.status[eStatus::ACID] > 0) {
			handle_acid();
			break;
		}
	
	// Healing and restoration of spell pts.
	if(is_out()) {
		if(univ.party.age % 100 == 0) {
			univ.party.heal(2);
		}
	}
	else {
		if(univ.party.age % 50 == 0) {
			for(cPlayer& pc : univ.party)
				if(pc.main_status == eMainStatus::ALIVE && pc.cur_health > pc.max_health)
					pc.cur_health--; // Bonus HP wears off
			univ.party.heal(1);
		}
	}
	if(is_out()) {
		if(univ.party.age % 80 == 0) {
			univ.party.restore_sp(2);
			for(cPlayer& pc : univ.party)
				if(pc.status[eStatus::DUMB] < 0)
					pc.status[eStatus::DUMB]++;
		}
	}
	else {
		if(univ.party.age % 40 == 0) {
			for(cPlayer& pc : univ.party) {
				if(pc.main_status == eMainStatus::ALIVE && pc.cur_sp > pc.max_sp)
					pc.cur_sp--; // Bonus SP wears off
				if(pc.status[eStatus::DUMB] < 0)
					pc.status[eStatus::DUMB]++;
			}
			univ.party.restore_sp(1);
		}
	}
	
	// Recuperation and chronic disease disads
	for(cPlayer& pc : univ.party)
		if(pc.main_status == eMainStatus::ALIVE) {
			if(pc.traits[eTrait::RECUPERATION] && get_ran(1,0,10) == 1 && pc.cur_health < pc.max_health) {
				pc.heal(2);
			}
			if(pc.traits[eTrait::CHRONIC_DISEASE] && get_ran(1,0,110) == 1) {
				pc.disease(4);
			}
			
		}
	
	
	// Blessing, slowed,etc.
	if(univ.party.age % 4 == 0)
		for(cPlayer& pc : univ.party) {
			move_to_zero(pc.status[eStatus::BLESS_CURSE]);
			move_to_zero(pc.status[eStatus::HASTE_SLOW]);
			if(cInvenSlot item = pc.has_abil_equip(eItemAbil::REGENERATE)) {
				if(pc.cur_health < pc.max_health && (overall_mode != MODE_OUTDOORS || get_ran(1,0,10) == 5)) {
					int j = get_ran(1,0,item->abil_data[0] / 3);
					if(item->abil_data[0] / 3 == 0)
						j = get_ran(1,0,1);
					if(is_out()) j = j * 4;
					pc.heal(j);
				}
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
	if(univ.out.is_road(univ.party.out_loc.x, univ.party.out_loc.y))
		return;
	ter_num_t ter = univ.out[univ.party.out_loc.x][univ.party.out_loc.y];
	if(!wilderness_lore_present(ter))
		return;
	eTrait trait = eTrait::PACIFIST;
	if(univ.scenario.ter_types[ter].special == eTerSpec::WILDERNESS_CAVE)
		trait = eTrait::CAVE_LORE;
	else if(univ.scenario.ter_types[ter].special == eTerSpec::WILDERNESS_SURFACE)
		trait = eTrait::WOODSMAN;
	if(trait == eTrait::PACIFIST)
		return;
	
	for(cPlayer& pc : univ.party)
		if(pc.is_alive() && pc.traits[trait] && get_ran(1,0,12) == 5) {
			univ.party.food += get_ran(univ.scenario.ter_types[ter].flag1,1,6);
			add_string_to_buf(pc.name + "hunts.");
			put_pc_screen();
		}
}

void switch_pc(short which) {
	if(current_switch < 6) {
		if(current_switch != which) {
			add_string_to_buf("Switch: OK.");
			univ.party.swap_pcs(which, current_switch);
			if(univ.cur_pc == current_switch)
				univ.cur_pc = which;
			else if(univ.cur_pc == which)
				univ.cur_pc = current_switch;
			set_stat_window_for_pc(univ.cur_pc);
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
	set_stat_window(ITEM_WIN_PC1);
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

void start_new_game(bool force) {
	std::string choice;
	using kb = sf::Keyboard;
	
	if(!force)
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
	
	// The original code called build_outdoors here, but they're not even in a scenario, so I removed it.
	// It was probably a relic of Exile III.
	// (It also refreshed stores... with uninitialized items.)
	
	if(!force)
		edit_party();
	
	// if no PCs left, forget it
	if(!univ.party.is_alive()) {
		party_in_memory = false;
		return;
	}
	
	// everyone gets a weapon
	for(cPlayer& pc : univ.party) {
		if(pc.main_status == eMainStatus::ALIVE) {
			pc.finish_create();
		}
	}
	party_in_memory = true;
	if(force) return;
	fs::path file = nav_put_party();
	if(!file.empty()) save_party(file, univ);
	univ.file = file;
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
		x = univ.party.town_loc.x;
		y = univ.party.town_loc.y;
	}else{
		x = univ.party.out_loc.x;
		y = univ.party.out_loc.y;
	}
	eDirection dir;
	while((dir = find_waterfall(x,y,mode)) != DIR_HERE){
		add_string_to_buf("  Waterfall!");
		if(mode == 0){
			x += 2 * dir_x_dif[dir];
			y += 2 * dir_y_dif[dir];
			univ.party.town_loc.x += 2 * dir_x_dif[dir];
			univ.party.town_loc.y += 2 * dir_y_dif[dir];
			update_explored(univ.party.town_loc);
		}else{
			x += 2 * dir_x_dif[dir];
			y += 2 * dir_y_dif[dir];
			univ.party.out_loc.x += 2 * dir_x_dif[dir];
			univ.party.loc_in_sec.x += 2 * dir_x_dif[dir];
			univ.party.out_loc.y += 2 * dir_y_dif[dir];
			univ.party.loc_in_sec.y += 2 * dir_y_dif[dir];
			update_explored(univ.party.out_loc);
		}
		draw_terrain();
		print_buf();
		if(wilderness_lore_present(coord_to_ter(x - dir_x_dif[dir], y - dir_y_dif[dir]) > 0) && get_ran(1,0,1) == 0)
			add_string_to_buf("  (No supplies lost.)");
		else {
			cTerrain& ter = univ.scenario.ter_types[coord_to_ter(x, y)];
			int lost = univ.party.food * ter.flag2 / 100;
			if(lost >= ter.flag3) {
				lost = ter.flag3;
				add_string_to_buf("  (Many supplies lost.)");
			} else add_string_to_buf("  (" + std::to_string(lost) + " supplies lost.)");
			univ.party.food -= lost;
		}
		put_pc_screen();
		play_sound(28);
		pause(8);
	}
	if(mode == 0){
		univ.party.boats[univ.party.in_boat].loc = univ.party.town_loc;
		univ.party.boats[univ.party.in_boat].which_town = univ.party.town_num;
	}else{
		univ.party.boats[univ.party.in_boat].which_town = 200;
		univ.party.boats[univ.party.in_boat].loc = univ.party.out_loc;
		univ.party.boats[univ.party.in_boat].sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
		univ.party.boats[univ.party.in_boat].sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
	}
}

bool outd_move_party(location destination,bool forced) {
	location real_dest, sector_p_in;
	bool keep_going = true,check_f;
	location store_corner,store_iwc;
	ter_num_t ter;
	
	keep_going = check_special_terrain(destination,eSpecCtx::OUT_MOVE,univ.party[0],&check_f);
	if(check_f)
		forced = true;
	if(univ.debug_mode && univ.ghost_mode)
		forced = keep_going = true;
	
	// If not blocked and not put in town by a special, process move
	if(keep_going && overall_mode == MODE_OUTDOORS) {
		
		real_dest.x = destination.x - univ.party.out_loc.x;
		real_dest.y = destination.y - univ.party.out_loc.y;
		
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
		real_dest.x = univ.party.out_loc.x + real_dest.x;
		real_dest.y = univ.party.out_loc.y + real_dest.y;
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
//			for(short i = 0; i < 10; i++)
//				if(same_point(destination,party.out_c[i].m_loc))
//					party.out_c[i].exists = false;
		
		ter = univ.out[real_dest.x][real_dest.y];
		if(univ.party.in_boat >= 0) {
			if(!outd_is_blocked(real_dest) //&& !outd_is_special(real_dest)
				// not in towns
				&& (!univ.scenario.ter_types[ter].boat_over
					|| ((real_dest.x != univ.party.out_loc.x) && (real_dest.y != univ.party.out_loc.y)))
				&& univ.scenario.ter_types[ter].special != eTerSpec::TOWN_ENTRANCE) {
				add_string_to_buf("You leave the boat.");
				univ.party.in_boat = -1;
			}
			else if(((real_dest.x != univ.party.out_loc.x) && (real_dest.y != univ.party.out_loc.y))
					 || (!forced && out_boat_there(destination)))
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
		
		univ.party.direction = set_direction(univ.party.out_loc, destination);
		std::string dir_str = dir_string[univ.party.direction];
		cVehicle* enter;
		if((enter = out_boat_there(real_dest)) && univ.party.in_boat < 0 && univ.party.in_horse < 0) {
			if(flying()) {
				add_string_to_buf("You land first.");
				univ.party.status[ePartyStatus::FLIGHT] = 0;
			}
			give_help(61,0);
			add_string_to_buf("Move: You board the boat.");
			univ.party.in_boat = enter - &univ.party.boats[0];
			
			univ.party.out_loc = real_dest;
			univ.party.i_w_c.x = (univ.party.out_loc.x > 48) ? 1 : 0;
			univ.party.i_w_c.y = (univ.party.out_loc.y > 48) ? 1 : 0;
			univ.party.loc_in_sec = global_to_local(univ.party.out_loc);
			
			if((store_corner.x != univ.party.outdoor_corner.x) || (store_corner.y != univ.party.outdoor_corner.y) ||
				(store_iwc.x != univ.party.i_w_c.x) || (store_iwc.y != univ.party.i_w_c.y))
				clear_map();
			
			return true;
		}
		else if((enter = out_horse_there(real_dest)) && univ.party.in_boat < 0 && univ.party.in_horse < 0) {
			if(flying()) {
				add_string_to_buf("Land before mounting horses.");
				return false;
			}
			
			give_help(60,0);
			add_string_to_buf("Move: You mount the horses.");
			play_sound(84);
			univ.party.in_horse = enter - &univ.party.horses[0];
			
			univ.party.out_loc = real_dest;
			univ.party.i_w_c.x = (univ.party.out_loc.x > 48) ? 1 : 0;
			univ.party.i_w_c.y = (univ.party.out_loc.y > 48) ? 1 : 0;
			univ.party.loc_in_sec = global_to_local(univ.party.out_loc);
			
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
			
			
			univ.party.out_loc = real_dest;
			univ.party.i_w_c.x = (univ.party.out_loc.x > 47) ? 1 : 0;
			univ.party.i_w_c.y = (univ.party.out_loc.y > 47) ? 1 : 0;
			univ.party.loc_in_sec = global_to_local(univ.party.out_loc);
			add_string_to_buf("Moved: " + dir_str);
			move_sound(univ.out[real_dest.x][real_dest.y],num_out_moves);
			num_out_moves++;
			
			if(univ.party.in_boat >= 0) { // Waterfall!!!
				run_waterfalls(1);
			}
			if(univ.party.in_horse >= 0) {
				univ.party.horses[univ.party.in_horse].which_town = 200;
				univ.party.horses[univ.party.in_horse].loc = univ.party.out_loc;
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
	ter_num_t ter;
	bool check_f = false;
	
	if(univ.town.is_force_cage(univ.party.town_loc.x, univ.party.town_loc.y)) {
		add_string_to_buf("Move: Can't escape.");
		return false;
	}
	
	if(univ.target_there(destination, TARG_MONST) == nullptr)
		keep_going = check_special_terrain(destination,eSpecCtx::TOWN_MOVE,univ.party[0],&check_f);
	if(check_f)
		forced = true;
	if(univ.debug_mode && univ.ghost_mode)
		forced = keep_going = true;
	
	ter = univ.town->terrain(destination.x,destination.y);
	
	if(keep_going) {
		if(univ.party.in_boat >= 0) {
			if((!is_blocked(destination)) && (!is_special(destination))
				// If to bridge, exit if heading diagonal, keep going if heading horiz or vert
				&& (!univ.scenario.ter_types[ter].boat_over
				|| ((destination.x != univ.party.town_loc.x) && (destination.y != univ.party.town_loc.y)))) {
				add_string_to_buf("You leave the boat.");
				univ.party.in_boat = -1;
			}
			else if((destination.x != univ.party.town_loc.x) && (destination.y != univ.party.town_loc.y))
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
			else if(town_boat_there(destination)) {
				add_string_to_buf("  Boat there already.");
				return false;
			}
			// water or lava
			else if(univ.scenario.ter_types[ter].boat_over)
				forced = true;
		}
		
		univ.party.direction = set_direction(univ.party.town_loc, destination);
		std::string dir_str = dir_string[univ.party.direction];
		cVehicle* there;
		if((there = town_boat_there(destination)) && univ.party.in_boat < 0 && univ.party.in_horse < 0) {
			if(there->property) {
				add_string_to_buf("  Not your boat.");
				return false;
			}
			give_help(61,0);
			add_string_to_buf("Move: You board the boat.");
			univ.party.in_boat = there - &univ.party.boats[0];
			
			univ.party.town_loc = destination;
			center = univ.party.town_loc;
			
			return true;
		}
		else if((there = town_horse_there(destination)) && univ.party.in_boat < 0 && univ.party.in_horse < 0) {
			if(there->property) {
				add_string_to_buf("  Not your horses.");
				return false;
			}
			give_help(60,0);
			add_string_to_buf("Move: You mount the horses.");
			play_sound(84);
			univ.party.in_horse = there - &univ.party.horses[0];
			
			univ.party.town_loc = destination;
			center = univ.party.town_loc;
			
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
			univ.party.town_loc = destination;
			add_string_to_buf("Moved: " + dir_str);
//			place_treasure(destination,5,3);
			
			move_sound(univ.town->terrain(destination.x,destination.y),(short) univ.party.age);
			
			if(univ.party.in_boat >= 0) {
				// Waterfall!!!
				run_waterfalls(0);
			}
			if(univ.party.in_horse >= 0) {
				univ.party.horses[univ.party.in_horse].loc = univ.party.town_loc;
				univ.party.horses[univ.party.in_horse].which_town = univ.party.town_num;
			}
			center = univ.party.town_loc;
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
	for(cPlayer& pc : univ.party)
		if(pc.main_status == eMainStatus::ALIVE && (pc.status[eStatus::POISON] > 0))
			return true;
	return false;
}

short nearest_monster() {
	short i = 100;
	
	for(short j = 0; j < 10; j++)
		if(univ.party.out_c[j].exists) {
			short s = dist(univ.party.out_loc,univ.party.out_c[j].m_loc);
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
	
	for(short k = 0; k < 31 ; k++) {
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
		cChoiceDlog confirm("confirm-interrupt", {"quit","cancel"});
		if(confirm.show() == "quit") return true;
	}
	return false;
}
