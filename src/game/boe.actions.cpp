
#include <cmath>
#include <queue>
#include <boost/algorithm/string/replace.hpp>

#include "boe.global.hpp"
#include "tools/replay.hpp"

#include "universe/universe.hpp"
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
#include "fileio/fileio.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "dialogxml/dialogs/pictchoice.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/strchoice.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include "boe.menus.hpp"
#include "tools/keymods.hpp"
#include "tools/winutil.hpp"
#include "tools/cursors.hpp"
#include "spell.hpp"
#include "scenario/shop.hpp"
#include "tools/prefs.hpp"
#include "gfx/render_shapes.hpp"
#include "tools/enum_map.hpp"
#include <string>
#include <boost/lexical_cast.hpp>
#include <sstream>

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

extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern short shop_identify_cost, shop_recharge_amount;

extern bool record_verbose;
extern bool replay_verbose;

const char *dir_string[] = {"North", "NorthEast", "East", "SouthEast", "South", "SouthWest", "West", "NorthWest"};
char get_new_terrain();
cCreature save_monster_type;

short wand_loc_count = 0;
short monst_place_count = 0; // 1 - standard place	2 - place last

enum_map(eShopArea, rectangle) shopping_rects[8];
std::queue<pending_special_type> special_queue;
bool end_scenario = false;

// This is defined in pc.editors.cpp since the PC editor also uses it
extern void edit_stuff_done();

static void init_shopping_rects() {
	rectangle shop_base = {63,19,99,274};
	
	std::fill(shopping_rects[0].begin(), shopping_rects[0].end(), shop_base);

	shopping_rects[0][SHOPRECT_ACTIVE_AREA].right -= 35;
	shopping_rects[0][SHOPRECT_GRAPHIC].right = shopping_rects[0][SHOPRECT_GRAPHIC].left + 28;
	shopping_rects[0][SHOPRECT_KEY].right = shopping_rects[0][SHOPRECT_GRAPHIC].left;
	shopping_rects[0][SHOPRECT_KEY].left = shopping_rects[0][SHOPRECT_KEY].right - 6;
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

	for(short i = 1; i < 8; i++) {
		for(auto& j : shopping_rects[i].keys()) {
			shopping_rects[i][j] = shopping_rects[0][j];
			shopping_rects[i][j].offset(0,i * 36);
		}
	}
}

void init_screen_locs() {
	init_shopping_rects();
	
	rectangle startup_base = {281,1,329,302};
	
	for(auto btn : startup_button.keys()) {
		startup_button[btn] = startup_base;
		startup_button[btn].offset(301 * (btn / 3), 48 * (btn % 3));
	}
	startup_top.top = 7;
	startup_top.bottom = startup_button[STARTBTN_TUTORIAL].top;
	startup_top.left = startup_base.left;
	startup_top.right = startup_button[STARTBTN_NEW].right;
	
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
	item_buttons[0][ITEMBTN_USE].top++;
	item_buttons[0][ITEMBTN_USE].left = 196;
	item_buttons[0][ITEMBTN_USE].right = 210;
	item_buttons[0][ITEMBTN_GIVE] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_GIVE].top++;
	item_buttons[0][ITEMBTN_GIVE].left = 210;
	item_buttons[0][ITEMBTN_GIVE].right = 224;
	item_buttons[0][ITEMBTN_DROP] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_DROP].top++;
	item_buttons[0][ITEMBTN_DROP].left = 224;
	item_buttons[0][ITEMBTN_DROP].right = 238;
	item_buttons[0][ITEMBTN_INFO] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_INFO].top++;
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

// Some actions directly show a dialog. This handles that, and records it.
void show_dialog_action(std::string xml_file) {
	if(recording){
		record_action("show_dialog_action", xml_file);
	}
	
	cChoiceDlog(xml_file).show();
}

bool prime_time() {
	return overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT;
}

eSkill last_spellcast_type = eSkill::MAGE_SPELLS;

void handle_spellcast(eSkill which_type, bool& did_something, bool& need_redraw, bool& need_reprint, bool record) {
	if(record && recording){
		std::map<std::string,std::string> info;
		info["which_type"] = boost::lexical_cast<std::string>(which_type);
		info["spell_forced"] = bool_to_str(spell_forced);
		record_action("handle_spellcast", info);
	}
	last_spellcast_type = which_type;
	short store_sp[6];
	extern short spec_target_fail;
	extern eSpecCtxType spec_target_type;
	// Dual-caster recast hint toggle:
	// Change the recast hint to mage if last spell wasn't mage
	if(spell_forced && is_combat() && univ.current_pc().last_cast_type != which_type){
		spell_forced = false;
		univ.current_pc().last_cast_type = which_type;
		need_redraw = true;
		return;
	}
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

// Recenter the camera after look is finished or canceled:
static void end_look(bool right_button, bool& need_redraw) {
	if(overall_mode == MODE_LOOK_COMBAT) {
		overall_mode = MODE_COMBAT;
		if(!right_button){
			center = univ.current_pc().combat_pos;
			pause(5);
			need_redraw = true;
		}
	}
	else if(overall_mode == MODE_LOOK_TOWN) {
		overall_mode = MODE_TOWN;
		if(!right_button){
			center = univ.party.town_loc;
			need_redraw = true;
		}
	}
	else if(overall_mode == MODE_LOOK_OUTDOORS){
		overall_mode = MODE_OUTDOORS;
		need_redraw = true;
	}
}

void handle_begin_look(bool right_button, bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("handle_begin_look", bool_to_str(right_button));
	}
	switch(overall_mode){
		// Cancel look:
		case MODE_LOOK_OUTDOORS: BOOST_FALLTHROUGH;
		case MODE_LOOK_TOWN: BOOST_FALLTHROUGH;
		case MODE_LOOK_COMBAT:
			need_reprint = true;
			add_string_to_buf("  Cancelled.");
			// right_button will never be true here:
			end_look(false, need_redraw);
			return;
			break;

		// Begin look target mode:
		case MODE_OUTDOORS:
			overall_mode = MODE_LOOK_OUTDOORS;
			break;
		case MODE_TOWN:
			overall_mode = MODE_LOOK_TOWN;
			break;
		case MODE_COMBAT:
			overall_mode = MODE_LOOK_COMBAT;
			break;

		default:
			break;
	}
	if(!right_button) add_string_to_buf("Look: Select a space. You can also right click to look.", 2);
	need_redraw = true;
}

void handle_begin_talk(bool& need_reprint) {
	if(recording){
		record_action("handle_begin_talk", "");
	}
	if(overall_mode == MODE_TALK_TOWN){
		overall_mode = MODE_TOWN;
		add_string_to_buf("  Cancelled.");
	}else if(overall_mode == MODE_TOWN){
		overall_mode = MODE_TALK_TOWN;
		add_string_to_buf("Talk: Select someone.");
	}
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

void handle_parry(bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("handle_parry", "");
	}
	add_string_to_buf("Parry.");
	char_parry();
	did_something = true;
	need_reprint = true;
	need_redraw = true;
}

void handle_toggle_active(bool& need_reprint) {
	if(recording){
		record_action("handle_toggle_active", "");
	}

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

void handle_rest(bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("handle_rest", "");
	}
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
			increase_age(false);
			if(get_ran(1,1,2) == 2)
				do_monsters();
			if(get_ran(1,1,70) == 10)
				create_wand_monst();
			if(nearest_monster() <= 3) {
				i = 200;
				add_string_to_buf("  Monsters nearby.");
			}
			while(pollEvent(mainPtr(), dummy_evt));
			redraw_screen(REFRESH_NONE);
			i++;
		}
		put_pc_screen();
	}
	if(i == 50) {
		do_rest(1200, get_ran(5,1,10), 50);
		add_string_to_buf("  Rest successful.");
		try_auto_save("RestComplete");
		put_pc_screen();
		pause(25);
	}
	need_reprint = true;
	need_redraw = true;
}

void handle_pause(bool& did_something, bool& need_redraw) {
	if(recording){
		record_action("handle_pause", "");
	}
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

void handle_look(location destination, bool right_button, eKeyMod mods, bool& need_redraw, bool& need_reprint) {
	if(recording){
		std::map<std::string,std::string> info;
		info["destination"] = boost::lexical_cast<std::string>(destination);
		info["right_button"] = bool_to_str(right_button);
		info["mods"] = boost::lexical_cast<std::string>(mods);
		record_action("handle_look", info);
	}
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
				location lDest=global_to_local(destination);
				for(int k = 0; k < univ.out->sign_locs.size(); k++) {
					if(lDest == univ.out->sign_locs[k]) {
						need_reprint = true;
						if(adjacent(univ.out->sign_locs[k],univ.party.loc_in_sec))
							do_sign(200 + get_outdoor_num(),k,ter_looked_at);
						else add_string_to_buf("  Too far away to read sign.");
					}
				}
			}
		}
	}
	// If option/ctrl not pressed, looking done, so restore center
	// (Unless this was a quick look, which doesn't require re-centering)
	bool look_done = true;
	if(mod_contains(mods, mod_alt) || mod_contains(mods, mod_ctrl)) look_done = false;
	if(look_done) {
		end_look(right_button, need_redraw);
	}
}

void handle_move(location destination, bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(recording) {
		// record the action
		std::ostringstream out;
		out << destination;
		record_action("move", out.str());
	}

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

void handle_talk(location destination, bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("handle_talk", boost::lexical_cast<std::string>(destination));
	}
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
				} else if(univ.town.monst[i].is_alive()) {
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

void handle_target_space(location destination, bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(recording){
		std::map<std::string,std::string> info;
		info["destination"] = boost::lexical_cast<std::string>(destination);
		info["num_targets_left"] = boost::lexical_cast<std::string>(num_targets_left);
		record_action("handle_target_space", info);
	}
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

void handle_drop_item(location destination, bool& need_redraw) {
	if(recording){
		record_action("handle_drop_item_location", boost::lexical_cast<std::string>(destination));
	}

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

void handle_use_space_select(bool& need_reprint) {
	if(recording){
		record_action("handle_use_space_select", "");
	}
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

void handle_use_space(location destination, bool& did_something, bool& need_redraw) {
	if(recording){
		record_action("handle_use_space", boost::lexical_cast<std::string>(destination));
	}
	if(!adjacent(destination,univ.party.town_loc))
		add_string_to_buf("  Must be adjacent.");
	else did_something = use_space(destination);
	overall_mode = MODE_TOWN;
	need_redraw = true;
	put_pc_screen();
	put_item_screen(stat_window);
}

void handle_bash_pick_select(bool& need_reprint, bool isBash) {
	if(recording){
		if(isBash)
			record_action("handle_bash_select", "");
		else
			record_action("handle_pick_select", "");
	}
	if(overall_mode == MODE_BASH_TOWN || overall_mode == MODE_PICK_TOWN) {
		add_string_to_buf("  Cancelled.");
		overall_mode = MODE_TOWN;
	} else {
		overall_mode = isBash ? MODE_BASH_TOWN : MODE_PICK_TOWN;
		add_string_to_buf(isBash ? "Bash Door: Select a space." : "Pick Lock: Select a space.");
	}
	need_reprint = true;
}

void handle_bash_pick(location destination, bool& did_something, bool& need_redraw, bool isBash) {
	if(recording){
		std::map<std::string,std::string> info;
		std::string destination_str = boost::lexical_cast<std::string>(destination);

		if(isBash)
			record_action("handle_bash", destination_str);
		else
			record_action("handle_pick", destination_str);
	}
	if(!adjacent(destination,univ.party.town_loc))
		add_string_to_buf("  Must be adjacent.");
	else if(!is_unlockable(destination))
		add_string_to_buf("  Wrong terrain type.");
	else {
		short pc = select_pc(eSelectPC::ONLY_LIVING, isBash ? "Who will bash?" : "Who will pick the lock?", isBash ? eSkill::STRENGTH : eSkill::LOCKPICKING);
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

void handle_switch_pc(short which_pc, bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("handle_switch_pc", std::to_string(which_pc));
	}

	cPlayer& pc = univ.party[which_pc];
	if(!prime_time() && overall_mode != MODE_SHOPPING && overall_mode != MODE_TALKING && overall_mode != MODE_ITEM_TARGET)
		add_string_to_buf("Set active: " + FINISH_FIRST);
	else if(is_combat()) {
		if(univ.debug_mode && pc.ap <= 0){
			pc.ap = 4;
		}
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

		// The shop interface may have taken control of the active PC. If the player
		// manually sets active PC before shopping ends, don't restore the active PC from before
		// shopping switched it. Unless the player buys something again, forcing it to store again.
		extern short store_cur_pc;
		if(store_cur_pc != -1)
			store_cur_pc = -1;

		set_stat_window_for_pc(which_pc);
		add_string_to_buf("Now " + std::string(overall_mode == MODE_SHOPPING ? "shopping" : "active") + ": " + pc.name);
		adjust_spell_menus();
		need_redraw = true;
	}
	need_reprint = true;
}

void handle_switch_pc_items(short which_pc, bool& need_redraw) {
	if(recording){
		record_action("handle_switch_pc_items", std::to_string(which_pc));
	}

	cPlayer& pc = univ.party[which_pc];
	if(!prime_time() && overall_mode != MODE_TALKING && overall_mode != MODE_SHOPPING)
		add_string_to_buf("Set active: " + FINISH_FIRST);
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
			draw_shop_graphics(false,false,item_screen_button_rects[which_pc]); // rect is dummy
		}
	}
}

void handle_equip_item(short item_hit, bool& need_redraw) {
	if(recording){
		record_action("handle_equip_item", std::to_string(item_hit));
	}

	if(overall_mode == MODE_USE_TOWN) {
		// This handles someone pressing the "Use Space" button and then clicking on an item.
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
	} else add_string_to_buf("Equip: " + FINISH_FIRST);
}

void handle_use_item(short item_hit, bool& did_something, bool& need_redraw) {
	if(recording){
		record_action("handle_use_item", std::to_string(item_hit));
	}

	if(!prime_time()) {
		add_string_to_buf("Use item: " + FINISH_FIRST);
		return;
	}
	use_item(stat_window, item_hit);
	if(overall_mode != MODE_TOWN_TARGET && overall_mode != MODE_SPELL_TARGET)
		did_something = true;
	take_ap(3);
	need_redraw = true;
}

void handle_give_item(short item_hit, bool& did_something, bool& need_redraw) {
	if(recording){
		record_action("handle_give_item", std::to_string(item_hit));
	}

	if(!prime_time()) {
		add_string_to_buf("Give item: " + FINISH_FIRST);
		return;
	}
	give_thing(stat_window, item_hit);
	did_something = true;
	need_redraw = true;
	take_ap(1);
}

void handle_drop_item(short item_hit, bool& need_redraw) {
	if(recording){
		record_action("handle_drop_item_id", std::to_string(item_hit));
	}

	if(overall_mode == MODE_DROP_TOWN || overall_mode == MODE_DROP_COMBAT) {
		add_string_to_buf("Drop item: Cancelled");
		overall_mode = is_town() ? MODE_TOWN : MODE_COMBAT;
	} else if(!prime_time())
		add_string_to_buf("Drop item: " + FINISH_FIRST);
	else if(is_out())
		drop_item(stat_window,item_hit,univ.party.out_loc);
	else {
		add_string_to_buf("Drop item: Click where to drop item.");
		store_drop_item = item_hit;
		overall_mode = is_town() ? MODE_DROP_TOWN : MODE_DROP_COMBAT;
		need_redraw = true;
	}
}

void handle_item_shop_action(short item_hit) {
	if(recording){
		record_action("handle_item_shop_action", std::to_string(item_hit));
	}

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
				if(overall_mode == MODE_ITEM_TARGET) {
					if(univ.party.all_items_identified()){
						overall_mode = MODE_TOWN;
						stat_screen_mode = MODE_INVEN;
						ASB("Identify: All of your items are identified.");
					}
				}
			}
			break;
		case MODE_RECHARGE:
			if(!take_gold(shop_identify_cost,false))
				ASB("Recharge: You don't have the gold.");
			else {
				if(shop_identify_cost == 0) {
					// Chance of melting
					// This is tuned so that overcharging has an 80% melt chance,
					// and charging an empty wand has a 0% melt chance.
					static const short melt_chance[] = {0, 1, 1, 1, 3, 3, 5, 15, 30, 50, 80};
					static const short n_melt = std::distance(std::begin(melt_chance), std::end(melt_chance));
					int i = round(double(target.charges * (n_melt - 1))) / target.max_charges;
					i = minmax(0, n_melt - 1, i);
					short r1 = get_ran(1, 1, 100);
					if(r1 < melt_chance[i]) {
						play_sound(41);
						ASB("Your item melted!");
						shopper.take_item(item_hit);
						break;
					}
				}
				play_sound(68);
				ASB("Your item is recharged.");
				target.charges += shop_recharge_amount;
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
				target.enchant_weapon(ench);
			}
			break;
		case MODE_INVEN: case MODE_SHOP:
			// The button doesn't even exist, so just do nothing.
			break;
	}
}

void handle_alchemy(bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("handle_alchemy", "");
	}

	need_reprint = true;
	need_redraw = true;
	if(overall_mode == MODE_TOWN)
		do_alchemy();
	else if(!is_town()) add_string_to_buf("Alchemy: Only in town.");
	else add_string_to_buf("Alchemy: " + FINISH_FIRST);
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
		increase_age(false);
		do_monsters();
		do_monster_turn();
		int make_wand = get_ran(1,1,160 - univ.town->difficulty);
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
		while(pollEvent(mainPtr(), dummy_evt));
		redraw_screen(REFRESH_NONE);
	}
	put_pc_screen();
	if(!party_sees_a_monst()){
		try_auto_save("TownWaitComplete");
	}
}

void handle_wait(bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("handle_wait", "");
	}

	if(overall_mode == MODE_TOWN) {
		handle_town_wait(need_redraw, need_reprint);
	} else if(!is_town()){
		add_string_to_buf("Wait: In town only.");
		print_buf();
	}else if(overall_mode == MODE_COMBAT) {
		handle_stand_ready(need_redraw, need_reprint);
		advance_time(did_something, need_redraw, need_reprint);
	} else {
		add_string_to_buf("Wait: " + FINISH_FIRST);
		print_buf();
	}
}

void handle_combat_switch(bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("handle_combat_switch", "");
	}

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

void handle_missile(bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("handle_missile", "");
	}

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

void handle_get_items(bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("handle_get_items", "");
	}

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

void handle_victory(bool force, bool record) {
	if(record && recording){
		record_action("handle_victory", "");
	}
	end_scenario = false;
	// This is the point at which we need to transfer any exported graphics over to the party sheet.
	univ.exportGraphics();
	univ.exportSummons();
	univ.clear_stored_pcs();
	reload_startup();
	overall_mode = MODE_STARTUP;
	draw_startup(0);
	menu_activate();
	univ.party.scen_name = ""; // should be harmless...
	if(!force && cChoiceDlog("congrats-save",{"cancel","save"}).show() == "save"){
		do_save();
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

void screen_shift(int dx, int dy, bool& need_redraw) {
	(void) need_redraw; // Mark the variable unused
	if(abs(dx) + abs(dy) == 0)
		return;

	if(recording){
		std::map<std::string,std::string> info;
		info["dx"] = boost::lexical_cast<std::string>(dx);
		info["dy"] = boost::lexical_cast<std::string>(dy);
		record_action("screen_shift", info);
	}

	center.x += dx;
	center.y += dy;
}

// If configured to move the screen with arrow keys, do it and return true
bool handle_screen_shift(location delta, bool& need_redraw) {
	if(scrollableModes.count(overall_mode) && get_bool_pref("DirectionalKeyScrolling", false) != kb.isShiftPressed()){
		screen_shift(delta.x, delta.y, need_redraw);
		return true;
	}
	return false;
}

void handle_print_pc_hp(int which_pc, bool& need_reprint) {
	if(recording){
		record_action("handle_print_pc_hp", boost::lexical_cast<std::string>(which_pc));
	}
	std::ostringstream str;
	cPlayer& pc = univ.party[which_pc];
	str << pc.name << " has ";
	str << pc.cur_health << " health out of " << pc.max_health << '.';
	add_string_to_buf(str.str());
	need_reprint = true;
}

void handle_print_pc_sp(int which_pc, bool& need_reprint) {
	if(recording){
		record_action("handle_print_pc_sp", boost::lexical_cast<std::string>(which_pc));
	}
	std::ostringstream str;
	cPlayer& pc = univ.party[which_pc];
	str << pc.name << " has ";
	str << pc.cur_sp << " spell pts. out of " << pc.max_sp << '.';
	add_string_to_buf(str.str());
	need_reprint = true;
}

void handle_trade_places(int which_pc, bool& need_reprint) {
	if(recording){
		record_action("handle_trade_places", boost::lexical_cast<std::string>(which_pc));
	}
	if(!prime_time())
		add_string_to_buf("Trade places: " + FINISH_FIRST);
	else if(is_combat())
		add_string_to_buf("Trade places: Can't do this in combat.");
	else {
		switch_pc(which_pc);
	}
	need_reprint = true;
}

void show_item_info(short item_hit) {
	if(recording){
		record_action("show_item_info", boost::lexical_cast<std::string>(item_hit));
	}
	if(stat_window == ITEM_WIN_SPECIAL)
		put_spec_item_info(spec_item_array[item_hit]);
	else if(stat_window == ITEM_WIN_QUESTS)
		put_quest_info(spec_item_array[item_hit]);
	else display_pc_item(stat_window, item_hit,univ.party[stat_window].items[item_hit],0);
}

void update_item_stats_area(bool& need_reprint) {
	put_pc_screen();
	put_item_screen(stat_window);
	need_reprint = true;
}

bool handle_action(const sf::Event& event, cFramerateLimiter& fps_limiter) {
	long item_hit;
	bool are_done = false;
	bool need_redraw = false, did_something = false, need_reprint = false;
	location loc_in_sec,cur_direction;
	bool right_button = event.mouseButton.button == sf::Mouse::Right;
	rectangle world_screen = win_to_rects[WINRECT_TERVIEW];
	rectangle terrain_viewport = world_screen;
	world_screen.inset(13, 13);
	
	location point_in_area;
	
	location the_point(event.mouseButton.x, event.mouseButton.y);
	the_point = mainPtr().mapPixelToCoords(the_point, mainView);
	end_scenario = false;
	
	// MARK: First, figure out where party is
	if(overall_mode == MODE_STARTUP || overall_mode == MODE_RESTING) {
		// If we get here during these modes, something is probably not right, so bail out
		add_string_to_buf("Unexpected game state!");
		return are_done;
	}
	
	// Now split off the extra stuff, like talking and shopping.
	if(overall_mode == MODE_TALKING) {
		if(handle_talk_event(the_point, fps_limiter)){
			advance_time(did_something, need_redraw, need_reprint);
			are_done = All_Done;
			return are_done;
		}
	}
	if(overall_mode == MODE_SHOPPING) {
		if(handle_shop_event(the_point, fps_limiter)){
			advance_time(did_something, need_redraw, need_reprint);
			are_done = All_Done;
			return are_done;
		}
	}

	// Otherwise they're in a terrain view mode
	location cur_loc = is_out() ? univ.party.out_loc : center;
	auto button_hit = UI::toolbar.button_hit(mainPtr(), the_point, fps_limiter);

	// MARK: Then, handle a button being hit.
		switch(button_hit) {
			case TOOLBAR_NONE: break;
			case TOOLBAR_MAGE: case TOOLBAR_PRIEST:
				handle_spellcast(button_hit == TOOLBAR_MAGE ? eSkill::MAGE_SPELLS : eSkill::PRIEST_SPELLS, did_something, need_redraw, need_reprint);
				break;
				
			case TOOLBAR_LOOK:
				handle_begin_look(false, need_redraw, need_reprint);
				break;
				
			case TOOLBAR_SHIELD:
				if(overall_mode == MODE_COMBAT)
					handle_parry(did_something, need_redraw, need_reprint);
				break;
				
			case TOOLBAR_TALK:
				if(overall_mode == MODE_TOWN || overall_mode == MODE_TALK_TOWN)
					handle_begin_talk(need_reprint);
				break;
				
			case TOOLBAR_CAMP:
				if(overall_mode == MODE_OUTDOORS)
					handle_rest(need_redraw, need_reprint);
				break;
				
			case TOOLBAR_SCROLL: case TOOLBAR_MAP:
				display_map();
				// do not call advance_time
				return false;
				
			case TOOLBAR_BAG: case TOOLBAR_HAND:
				if(overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT)
					handle_get_items(did_something, need_redraw, need_reprint);
				break;
				
			case TOOLBAR_SAVE:
				if(overall_mode == MODE_OUTDOORS) {
					do_save();
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
		auto look_destination = [i, j, destination]() {
			location look_dest = destination;
			if (overall_mode == MODE_LOOK_OUTDOORS) look_dest = univ.party.out_loc;
			look_dest.x = look_dest.x + i - 4;
			look_dest.y = look_dest.y + j - 4;
			return look_dest;
		};

		// Check for quick look
		if(right_button) {
			handle_begin_look(true, need_redraw, need_reprint);
			handle_look(look_destination(), true, current_key_mod(), need_redraw, need_reprint);
		}
		
		// Moving/pausing
		else if(overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) {
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
			handle_look(look_destination(), false, current_key_mod(), need_redraw, need_reprint);
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
			screen_shift(0, -1, need_redraw);
		}
		if(the_point.x < world_screen.left && center.x > univ.town->in_town_rect.left && center.x > 4) {
			screen_shift(-1, 0, need_redraw);
		}
		if(the_point.y > world_screen.bottom && center.y < univ.town->in_town_rect.bottom && center.y < univ.town->max_dim - 5) {
			screen_shift(0, 1, need_redraw);
		}
		if(the_point.x > world_screen.right && center.x < univ.town->in_town_rect.right && center.x < univ.town->max_dim - 5) {
			screen_shift(1, 0, need_redraw);
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
			if(arrow_button_click(help_button, &fps_limiter))
				show_dialog_action("help-party");
		}
		for(int i = 0; i < 6; i++)
			for(auto j : pc_buttons[i].keys())
				if(pc_area_button_active[i][j] && point_in_area.in(pc_buttons[i][j])) {
					cPlayer& pc = univ.party[i];
					if((j == PCBTN_HP || j == PCBTN_SP) && !pc.is_alive())
						break;
					rectangle button_rect = pc_buttons[i][j];
					button_rect.offset(pc_win_ul);
					if(arrow_button_click(button_rect, &fps_limiter))
						switch(j) {
							case PCBTN_NAME:
								handle_switch_pc(i, need_redraw, need_reprint);
								break;
							case PCBTN_HP:
								handle_print_pc_hp(i, need_reprint);
								break;
							case PCBTN_SP:
								handle_print_pc_sp(i, need_reprint);
								break;
							case PCBTN_INFO:
								give_pc_info(i);
								// don't call advance_time
								return false;
							case PCBTN_TRADE:
								handle_trade_places(i, need_reprint);
								break;
							case MAX_ePlayerButton:
								break; // Not a button
						}
				}
		put_pc_screen();
		put_item_screen(stat_window);
		if(overall_mode == MODE_SHOPPING) {
			set_up_shop_array();
			draw_shop_graphics(false,false,pc_buttons[0][PCBTN_NAME]);
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
				if(arrow_button_click(button_rect, &fps_limiter))
					switch(i) {
						case 6: // special screen
							set_stat_window(ITEM_WIN_SPECIAL, true);
							break;
						case 7:
							set_stat_window(ITEM_WIN_QUESTS, true);
							break;
						case 8: // help
							show_dialog_action("help-inventory");
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
						if(arrow_button_click(button_rect, &fps_limiter)){
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
										use_spec_item(spec_item_array[item_hit], need_redraw);
									} else handle_drop_item(item_hit, need_redraw);
									break;
								case ITEMBTN_INFO:
									show_item_info(item_hit);
									break;
								case ITEMBTN_SPEC: // sell? That this code was reached indicates that the item was sellable
									// (Based on item_area_button_active)
									handle_item_shop_action(item_hit);
									break;
								case MAX_eItemButton:
									break; // Not a button
							}
						}
					}
		}
		update_item_stats_area(need_reprint);
	}
	
	advance_time(did_something, need_redraw, need_reprint);
	
	are_done = All_Done;
	return are_done;
}

void advance_time(bool did_something, bool need_redraw, bool need_reprint) {
	if(recording && record_verbose){
		std::map<std::string,std::string> info;
		info["did_something"] = bool_to_str(did_something);
		info["need_redraw"] = bool_to_str(need_redraw);
		info["need_reprint"] = bool_to_str(need_reprint);
		record_action("advance_time", info);
	}
	if(replaying && replay_verbose){
		if(has_next_action("advance_time")){
			std::string _last_action_type = last_action_type;
			Element& element = pop_next_action();
			std::map<std::string,std::string> info = info_from_action(element);
			std::ostringstream sstr;
			sstr << std::boolalpha;
			bool wrong_args = false;
			bool divergent = false;
			if(did_something != str_to_bool(info["did_something"])){
				wrong_args = true;
				divergent = true;
				sstr << "did_something: expected " << !did_something << ", was " << did_something << ". ";
			}
			if(need_redraw != str_to_bool(info["need_redraw"])){
				wrong_args = true;
				sstr << "need_redraw: expected " << !need_redraw << ", was " << need_redraw << ". ";
			}
			if(need_reprint != str_to_bool(info["need_reprint"])){
				wrong_args = true;
				sstr << "need_reprint: expected " << !need_reprint << ", was " << need_reprint << ". ";
			}
			sstr << "After " << _last_action_type;
			if(wrong_args){
				if(divergent){
					throw replay_error + "advance_time() was called with the wrong args, diverging behavior. " + sstr.str();
				}
				else{
					std::string message = "advance_time() was called with the wrong args, changing behavior cosmetically. " + sstr.str();
					if(replay_strict){
						throw replay_error + message;
					}else{
						std::cout << replay_warning << message << std::endl;
					}
				}
			}
		}else{
			if(did_something){
				throw replay_error + "advance_time() was called with divergent side effects following an action which does not advance time: " + last_action_type;
			}else{
				std::string message = "advance_time() was called with cosmetic side effects following an action which does not advance time: " + last_action_type;
				if(replay_strict){
					throw replay_error + message;
				}else{
					std::cout << replay_warning << message << std::endl;
				}
			}
		}
	}

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
						if(handle_wandering_specials(0)){
							// Render the frame where the monsters are next to the party!
							redraw_everything();

							initiate_outdoor_combat(i);
						}
						univ.party.out_c[i].exists = false;
						
						// Get rid of excess keyclicks
						flushingInput = true;
						
						need_reprint = false;
						i = 20;
					}
		}
		if(overall_mode == MODE_TOWN) {
			if(get_ran(1,1,160 - univ.town->difficulty + univ.party.less_wm * 200) == 2)
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
	if(recording){
		record_action("handle_menu_spell", boost::lexical_cast<std::string>((int) spell_picked));
	}

	eSkill spell_type = (*spell_picked).type;
	if(!prime_time()) {
		ASB("Cast: " + FINISH_FIRST);
		print_buf();
		return;
	}
	
	spell_forced = true;
	pc_casting = univ.cur_pc;
	univ.current_pc().last_cast[spell_type] = spell_picked;
	univ.current_pc().last_cast_type = spell_type;
	if(spell_type == eSkill::MAGE_SPELLS)
		store_mage = spell_picked;
	else store_priest = spell_picked;
	if(spell_type == eSkill::MAGE_SPELLS && (*spell_picked).need_select != SELECT_NO) {
		if((store_spell_target = select_pc((*spell_picked).need_select == SELECT_ANY ? eSelectPC::ANY : eSelectPC::ONLY_LIVING,"Cast spell on who?")) == 6)
			return;
	}
	else {
		if(spell_type == eSkill::PRIEST_SPELLS && (*spell_picked).need_select != SELECT_NO)
			if((store_spell_target = select_pc((*spell_picked).need_select == SELECT_ANY ? eSelectPC::ANY : eSelectPC::ONLY_LIVING,"Cast spell on who?")) == 6)
				return;
	}
	
	bool did_something = false, need_redraw = false, need_reprint = false;
	handle_spellcast(spell_type, did_something, need_redraw, need_reprint, false);
	advance_time(did_something, need_redraw, need_reprint);
}

static void set_up_combat() {
	location to_place;
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

void initiate_outdoor_combat(short i) {
	// Make sure the player can see the monster they stepped next to
	draw_terrain();
	
	// Is combat too easy?
	if((univ.party.get_level() > ((out_enc_lev_tot(i) * 5) / 3) ) && (out_enc_lev_tot(i) < 200)
		&& !univ.party.out_c[i].what_monst.cant_flee) {
		add_string_to_buf("Combat: Monsters fled!");
		univ.party.out_c[i].exists = false;
		return;
	}
	
	start_outdoor_combat(univ.party.out_c[i], univ.party.out_loc,count_walls(univ.party.out_loc));
	
	univ.party.out_c[i].exists = false;
	
	set_up_combat();
}

void show_inventory() {
	if(recording){
		record_action("show_inventory", "");
	}
	// Show active PC's inventory
	set_stat_window_for_pc(univ.cur_pc);
	put_item_screen(stat_window);
}

// Most give_help() calls don't need to be recorded, because
// they are triggered incidentally by other recorded actions.
void give_help_and_record(short help1, short help2, bool help_forced) {
	if(recording){
		std::map<std::string,std::string> info;
		info["help1"] = boost::lexical_cast<std::string>(help1);
		info["help2"] = boost::lexical_cast<std::string>(help2);
		info["help_forced"] = bool_to_str(help_forced);
		record_action("give_help", info);
	}
	give_help(help1, help2, help_forced);
}

void toggle_debug_mode() {
	if(recording){
		record_action("toggle_debug_mode", "");
	}
	if(univ.debug_mode) {
		univ.debug_mode = false;
		ASB("Debug mode OFF.");
	} else {
		univ.debug_mode = true;
		ASB("Debug mode ON.");
	}
	print_buf();
}

void debug_fight_encounter(bool wandering) {
	if(recording){
		record_action("debug_fight_encounter", bool_to_str(wandering));
	}
	if(!is_out()){
		ASB("Debug outdoor encounter: You have to be");
		ASB("outdoors!");
		print_buf();
		return;
	}

	std::string prompt = "Which ";
	if(wandering){
		prompt += "wandering encounter?";
	}else{
		prompt += "special encounter?";
	}

	int i = get_num_response(0, 3, prompt, {}, -1);
	if(i == -1) return;

	cOutdoors::cWandering encounter;
	if(wandering){
		encounter = univ.out->wandering[i];
	}else{
		encounter = univ.out->special_enc[i];
	}
	start_outdoor_combat(encounter, univ.party.out_loc, count_walls(univ.party.out_loc));

	set_up_combat();
}

void debug_give_item() {
	if(recording){
		record_action("debug_give_item", "");
	}
	std::vector<std::string> item_names;
	for(cItem& item : univ.scenario.scen_items){
		item_names.push_back(item.full_name);
	}
	int i = get_num_response(0, univ.scenario.scen_items.size()-1, "Which item?", item_names, -1);
	if(i == -1) return;
	bool was_ident = univ.scenario.scen_items[i].ident;
	univ.scenario.scen_items[i].ident = true;
	bool given = univ.current_pc().give_item(univ.scenario.scen_items[i], GIVE_DO_PRINT | GIVE_ALLOW_OVERLOAD) == eBuyStatus::OK;
	if(!given){
		ASB("Debug: can't give to " + univ.current_pc().name);
		given = univ.party.give_item(univ.scenario.scen_items[i], GIVE_DO_PRINT | GIVE_ALLOW_OVERLOAD);
	}
	if(!given)
		ASB("Debug: can't give anyone " + univ.scenario.scen_items[i].full_name);

	univ.scenario.scen_items[i].ident = was_ident;
	print_buf();
	put_item_screen(stat_window);
	put_pc_screen(); // In case the item was food or gold
}

void debug_overburden() {
	if(recording){
		record_action("debug_overburden", "");
	}

	cPlayer& pc = univ.current_pc();
	cItem item(ITEM_DEBUG_HEAVY);
	// Give the PC very heavy objects that do nothing:
	while(pc.give_item(item, GIVE_ALLOW_OVERLOAD) == eBuyStatus::OK){}
	if(pc.has_space()){
		// I don't know why this would ever happen, since the weight is 0, but just in case:
		ASB("Debug: failed to fill " + pc.name + "'s inventory.");
	}else{
		ASB("Debug: filled " + pc.name + "'s inventory.");
	}
	print_buf();
}

void debug_print_location() {
	if(recording){
		record_action("debug_print_location", "");
	}
	std::ostringstream sout;
	if(is_town()) {
		sout << "Debug:  You're at x " << (short) univ.party.town_loc.x << ", y " << (short) univ.party.town_loc.y
		<< " in town " << univ.party.town_num << '.';
	} else if(is_out()) {
		short x = univ.party.out_loc.x;
		short y = univ.party.out_loc.y;
		sout << "Debug:  You're outside in sec x " << univ.party.outdoor_corner.x << ", y " << univ.party.outdoor_corner.y << '\n';
		add_string_to_buf(sout.str());
		sout.str("");
		sout.seekp(0);
		sout << "   local x " << x << ", y " << y;
		x += 48 * univ.party.outdoor_corner.x;
		y += 48 * univ.party.outdoor_corner.y;
		sout << " / global x " << x << ", y " << y << '.';
	} else if(is_combat()) {
		location loc = univ.current_pc().combat_pos;
		sout << "Debug:  You're in combat at x " << loc.x << ", y " << loc.y << '.';
	}
	add_string_to_buf(sout.str());
	print_buf();
}

void debug_step_through() {
	if(recording){
		record_action("debug_step_through", "");
	}
	if(univ.node_step_through) {
		univ.node_step_through = false;
		add_string_to_buf("Debug: Step-through disabled");
	} else {
		univ.node_step_through = true;
		add_string_to_buf("Debug: Step-through enabled");
	}
	print_buf();
}

void debug_leave_town() {
	if(recording){
		record_action("debug_leave_town", "");
	}
	if(overall_mode == MODE_OUTDOORS){
		add_string_to_buf("Debug - Leave Town: You're not in town!");
		print_buf();
		return;
	}
	ASB("Debug: Reunite party and leave town.");
	print_buf();
	univ.party.end_split(0);
	end_town_mode(false, {0,0}, true);
}

void debug_kill() {
	if(recording){
		record_action("debug_kill", "");
	}
	for(short i = 0; i < univ.town.monst.size(); i++) {
		if(is_combat() && univ.town.monst[i].is_alive() && !univ.town.monst[i].is_friendly())
			univ.town.monst[i].active = eCreatureStatus::DEAD;
		
		if(univ.town.monst[i].is_alive() && !univ.town.monst[i].is_friendly()
			&& (dist(univ.town.monst[i].cur_loc,univ.party.town_loc) <= 10) )
			damage_monst(univ.town.monst[i], 7,1000,eDamageType::SPECIAL);
	}
	// kill_monst(&univ.town.monst[i],6);
	draw_terrain();
	add_string_to_buf("Debug: Kill things.");
	print_buf();
}

void debug_kill_party() {
	if(recording){
		record_action("debug_kill_party", "");
	}
	// New behavior to match debug_hurt_party and debug_give_status: allow choosing a PC,
	// and the type of death
	if(has_feature_flag("debug-kill-party", "V2")){
		size_t choice = cStringChoice({"Dead", "Dust", "Stone"}, "Kill how?").show(-1);
		if(choice == -1) return;
		eMainStatus death_type = static_cast<eMainStatus>(static_cast<size_t>(eMainStatus::DEAD) + choice);
		short pc = select_pc(eSelectPC::ONLY_LIVING, "Kill who?", eSkill::INVALID, true);
		if(pc == 6) return;
		for(int i = 0; i < 6; ++i){
			if(i == pc || (univ.party[i].is_alive() && pc == 7)) {
				// Kill PCs so you can test resurrection (or game over)
				kill_pc(univ.party[i], death_type);
			}
		}
		advance_time(false, true, true);
	}else{
		std::string confirm = cChoiceDlog("kill-party-confirm",{"yes","no"}).show();
		if(confirm == "yes"){
			for(short i = 0; i < 6; i++) {
				if(univ.party[i].is_alive())
					kill_pc(univ.party[i],eMainStatus::ABSENT);
			}
			draw_terrain();
			add_string_to_buf("Debug: Kill the party.");
			advance_time(false, true, true);
		}
	}
}

void debug_hurt_party() {
	if(recording){
		record_action("debug_hurt_party", "");
	}

	short pc = select_pc(eSelectPC::ONLY_LIVING, "Hurt who?", eSkill::INVALID, true);
	if(pc == 6) return;
	for(int i = 0; i < 6; ++i){
		if(i == pc || (univ.party[i].is_alive() && pc == 7)) {
			// Hurt PCs so you can test healing
			short wounded = univ.party[i].max_health / 2;
			univ.party[i].cur_health = min(univ.party[i].cur_health, wounded);
		}
	}
}

void debug_give_status() {
	if(recording){
		record_action("debug_give_status", "");
	}

	pic_num_t which = choose_status_effect(-1, false);
	if(which == -1) return;
	eStatus which_status = static_cast<eStatus>(which);
	std::string choice = cChoiceDlog("help-or-harm",{"harm","help"}).show();
	std::pair<int, int> bounds = status_bounds(which_status);
	int lo = bounds.first;
	int hi = bounds.second;
	status_info_t info = *which_status;
	int value = 0;
	if(choice == "harm"){
		if(info.isNegative){
			value = hi;
		}else{
			value = lo;
		}
	}else{
		if(info.isNegative){
			value = lo;
		}else{
			value = hi;
		}
	}

	short pc = select_pc(eSelectPC::ONLY_LIVING, "Give status to who?", eSkill::INVALID, true);
	if(pc == 6) return;
	for(int i = 0; i < 6; ++i){
		if(i == pc || (univ.party[i].is_alive() && pc == 7)) {
			// Give PCs a status effect. Bypass apply_status because it blocks 'wrapping' for
			// some opposite statuses
			univ.party[i].status[which_status] = value;
		}
	}
}

void debug_magic_map() {
	if(recording){
		record_action("debug_magic_map", "");
	}
	if(overall_mode == MODE_OUTDOORS) {
		for(short i = 0; i < univ.out.max_dim; i++)
			for(short j = 0; j < univ.out.max_dim; j++)
				make_explored(i,j);
	} else {
		for(short i = 0; i < univ.town->max_dim; i++)
			for(short j = 0; j < univ.town->max_dim; j++)
				make_explored(i,j);
	}
	clear_map();
	add_string_to_buf("Debug:  Magic Map.");
	print_buf();
}

void debug_enter_town() {
	if(recording){
		record_action("debug_enter_town", "");
	}

	std::vector<std::string> town_names;
	for(cTown* town : univ.scenario.towns){
		town_names.push_back(town->name);
	}
	int town = get_num_response(0, univ.scenario.towns.size() - 1, "Enter Town Number", town_names, -1);
	if(town == -1) return;

	if(has_feature_flag("debug-enter-town", "move-outdoors")){
		outd_move_to_first_town_entrance(town);
	}

	short find_direction_from;
	if(univ.party.direction == 0) find_direction_from = 2;
	else if(univ.party.direction == 4) find_direction_from = 0;
	else if(univ.party.direction < 4) find_direction_from = 3;
	else find_direction_from = 1;
	start_town_mode(town, find_direction_from, true);
}

void debug_refresh_stores() {
	if(recording){
		record_action("debug_refresh_stores", "");
	}
	univ.refresh_store_items();
	add_string_to_buf("Debug: Refreshed jobs/shops.");
	print_buf();
}

void debug_clean_up() {
	if(recording){
		record_action("debug_clean_up", "");
	}
	univ.party.clear_bad_status();
	add_string_to_buf("Debug: You get cleaned up!");
	print_buf();
	put_pc_screen();
}

void debug_stealth_detect_life_firewalk() {
	if(recording){
		record_action("debug_stealth_detect_life_firewalk", "");
	}
	univ.party.status[ePartyStatus::STEALTH] += 10;
	univ.party.status[ePartyStatus::DETECT_LIFE] += 10;
	univ.party.status[ePartyStatus::FIREWALK] += 10;
	add_string_to_buf("Debug: Stealth, Detect Life, Firewalk!");
	draw_text_bar();
	print_buf();
	put_pc_screen();
}

void debug_fly() {
	if(recording){
		record_action("debug_fly", "");
	}
	if(overall_mode != MODE_OUTDOORS){
		add_string_to_buf("Debug: Can only fly outdoors.");
	}else{
		univ.party.status[ePartyStatus::FLIGHT] += 10;
		add_string_to_buf("Debug: You start flying!");
	}
	print_buf();
	put_pc_screen();
}

void debug_ghost_mode() {
	if(recording){
		record_action("debug_ghost_mode", "");
	}
	if(univ.ghost_mode){
		univ.ghost_mode = false;
		ASB("Debug: Ghost mode OFF.");
	}else{
		univ.ghost_mode = true;
		ASB("Debug: Ghost mode ON.");
	}
	print_buf();
}

void debug_return_to_start() {
	if(recording){
		record_action("debug_return_to_start", "");
	}
	if(univ.party.in_boat >= 0) {
		add_string_to_buf("  Not while in boat.");
		return;
	}
	if(univ.party.in_horse >= 0) {
		add_string_to_buf("  Not while on horse.");
		return;
	}
	force_town_enter(univ.scenario.which_town_start,univ.scenario.where_start);
	start_town_mode(univ.scenario.which_town_start,9);
	position_party(univ.scenario.out_sec_start.x,univ.scenario.out_sec_start.y,
					univ.scenario.out_start.x,univ.scenario.out_start.y);
	center = univ.party.town_loc = univ.scenario.where_start;
	redraw_screen(REFRESH_ALL);
	add_string_to_buf("Debug:  You return to the start.");
	print_buf();
}

void debug_increase_age() {
	if(recording){
		record_action("debug_increase_age", "");
	}
	ASB("Debug: Increase age.");
	ASB("  It is now 1 day later.");
	print_buf();
	univ.party.age += 3700;
	put_pc_screen();
}

void debug_towns_forget() {
	if(recording){
		record_action("debug_towns_forget", "");
	}
	ASB("DEBUG: Towns have short memory.");
	ASB("Your deeds have been forgotten.");
	print_buf();
	for(auto& pop : univ.party.creature_save)
		pop.which_town = 200;
}

static void revive_all_dead(bool full_restore) {
	for(cPlayer& pc : univ.party) {
		if(isDead(pc.main_status))
			pc.main_status = eMainStatus::ALIVE;

		if(full_restore){
			pc.cur_health = pc.max_health;
			pc.cur_sp = 100;
		}
	}
	if(!full_restore){
		univ.party.heal(250);
		univ.party.restore_sp(100);
	}
}

void debug_heal() {
	if(recording){
		record_action("debug_heal", "");
	}
	univ.party.gold += 100;
	univ.party.food += 100;
	revive_all_dead(false);
	add_string_to_buf("Debug: Heal party.");
	print_buf();
	put_pc_screen();
}

void debug_heal_plus_extra() {
	if(recording){
		record_action("debug_heal_plus_extra", "");
	}
	univ.party.gold += 100;
	univ.party.food += 100;
	revive_all_dead(true);
	award_party_xp(25);
	for(cPlayer& who : univ.party) {
		who.priest_spells.set();
		who.mage_spells.set();
	}
	univ.refresh_store_items();
	add_string_to_buf("Debug: Add stuff and heal.");
	print_buf();
	put_pc_screen();
}

void close_map(bool record) {
	if(record && recording){
		record_action("close_map", "");
	}
	mini_map().setVisible(false);
	map_visible = false;
	mainPtr().setActive();
}

void cancel_item_target(bool& did_something, bool& need_redraw, bool& need_reprint) {
	if(recording){
		record_action("cancel_item_target", "");
	}
	if(stat_screen_mode == MODE_IDENTIFY)
		ASB("Identify: Finished");
	else if(stat_screen_mode == MODE_RECHARGE)
		ASB("Recharge: Finished");
	overall_mode = MODE_TOWN;
	stat_screen_mode = MODE_INVEN;
	// Time passes because a spell was cast. Redraw happens because item names
	// in the inventory can change. Reprint happens because of the buffer print
	did_something = need_redraw = need_reprint = true;
}

// I'm finally adding the easter egg to the replay system
// because it allows forcing the text buffer into a specific state
// which I'm debugging.
std::vector<std::string> easter_egg_messages = {
	"If Valorim ...",
	"You want to save ...",
	"Back up your save files ...",
	"Burma Shave."
};

void easter_egg(int idx) {
	if(recording){
		record_action("easter_egg", boost::lexical_cast<std::string>(idx));
	}
	add_string_to_buf(easter_egg_messages[idx]);
	print_buf();
}

std::map<char,key_action_t> debug_actions;
std::map<char,key_action_t> startup_debug_actions;
std::vector<std::vector<char>> debug_actions_help_order;

static void add_debug_action(std::vector<char> keys, std::string name, void (*action)(), bool startup_safe = false) {
	key_action_t shortcut = {keys, name, action};
	for(char ch: keys){
		debug_actions[ch] = shortcut;
		if(startup_safe)
			startup_debug_actions[ch] = shortcut;
	}
	debug_actions_help_order.push_back(keys);
}

void show_debug_help() {
	if(recording){
		record_action("show_debug_help", "");
	}
	cDialog debug_panel(*ResMgr::dialogs.get("help-debug"));
	int idx;
	for(idx = 0; idx < debug_actions_help_order.size(); ++idx){
		std::ostringstream btn_name;
		btn_name << "btn" << (idx+1);
		cControl& button = debug_panel[btn_name.str()];
		char key = debug_actions_help_order[idx][0];
		if(overall_mode == MODE_STARTUP && startup_debug_actions.find(key) == startup_debug_actions.end()){
			button.hide();
			continue;
		}
		key_action_t action = debug_actions[key];
		std::ostringstream btn_text;
		btn_text << action.keys[0];
		for(int key_idx = 1; key_idx < action.keys.size(); ++key_idx){
			btn_text << ", " << action.keys[key_idx];
		}
		btn_text << " - " << action.name;
		button.setText(btn_text.str());
		// TODO it's unfortunate that the button can only have one key if actions might have
		// more than one
		button.attachKey(charToKey(action.keys[0]));
		// Don't recursively open the panel
		if(action.action != &show_debug_help){
			button.attachClickHandler([action](cDialog& me, std::string, eKeyMod) -> bool {
				me.toast(false);
				// In a replay, the action will have been recorded next anyway, so the dialog doesn't need to trigger it.
				if(!replaying)
					action.action();
				return true;
			});
		}
	}
	for(; ; ++idx){
		std::ostringstream btn_name;
		btn_name << "btn" << (idx+1);
		if(!debug_panel.hasControl(btn_name.str()))
			break;
		cControl& button = debug_panel[btn_name.str()];
		button.hide();
	}

	debug_panel.attachClickHandlers([](cDialog& me, std::string, eKeyMod) -> bool {
		me.toast(false);
		return true;
	}, {"okay"});

	debug_panel.run();
}

void debug_launch_scen(std::string scen_name) {
	if(recording){
		record_action("debug_launch_scen", scen_name);
	}
	// Start by using the default party
	if(!party_in_memory){
		// TODO: Could add a debug party preference like the scenario editor has
		start_new_game(true);
	}
	// Take party out of scenario they're in
	if(univ.party.is_in_scenario()){
		if(cChoiceDlog("leave-scenario",{"okay","cancel"}).show() != "okay")
			return;
		handle_victory(true);
	}
	// Force party into the scenario, skipping the intro:
	put_party_in_scen(scen_name, true);
}

// Non-comprehensive list of unused keys:
// chjklnoqvy +[]{},.'"`|;:
// We want to keep lower-case for normal gameplay.
void init_debug_actions() {
	// optional `true` argument means you can use this action in the startup menu.
	add_debug_action({'B'}, "Leave town", debug_leave_town);
	add_debug_action({'C'}, "Get cleaned up (lose negative status effects)", debug_clean_up);
	add_debug_action({'D'}, "Toggle Debug mode", toggle_debug_mode, true);
	add_debug_action({'E'}, "Stealth, Detect Life, Firewalk", debug_stealth_detect_life_firewalk);
	add_debug_action({'F'}, "Flight", debug_fly);
	add_debug_action({'G'}, "Toggle Ghost mode (letting you pass through walls)", debug_ghost_mode);
	add_debug_action({'H'}, "Heal", debug_heal);
	// This one was missing from the old help dialog:
	add_debug_action({'I'}, "Give item", debug_give_item);
	add_debug_action({'Y'}, "Fill PC's inventory", debug_overburden);
	// TODO this is not recorded or replayed because the rsrc you pick might not even be packaged
	// in the build
	add_debug_action({'J'}, "Preview a dialog's layout", preview_dialog_xml, true);
	add_debug_action({'U'}, "Preview EVERY dialog's layout", preview_every_dialog_xml, true);
	add_debug_action({'K'}, "Kill everything", debug_kill);
	add_debug_action({'N'}, "End scenario", []() {handle_victory(true, true);});
	add_debug_action({'O'}, "Print your location", debug_print_location);
	add_debug_action({'Q'}, "Magic map", debug_magic_map);
	add_debug_action({'R'}, "Return to start", debug_return_to_start);
	add_debug_action({'S'}, "Set stuff done flags", []() {
		// edit_stuff_done() is used in the character editor which
		// doesn't have replays, so its replay action is recorded
		// external to the function definition unlike most actions.
		if(recording){
			record_action("edit_stuff_done", "");
		}
		edit_stuff_done();
	});
	add_debug_action({'T'}, "Enter town", debug_enter_town);
	add_debug_action({'W'}, "Refresh jobs/shops", debug_refresh_stores);
	add_debug_action({'X'}, "Kill party", debug_kill_party);
	add_debug_action({'-'}, "Hurt the party", debug_hurt_party);
	add_debug_action({'_'}, "Give a status effect", debug_give_status);
	add_debug_action({'~'}, "Clear captured souls", clear_trapped_monst);
	add_debug_action({'='}, "Heal, increase magic skills", debug_heal_plus_extra);
	add_debug_action({'<'}, "Make one day pass", debug_increase_age);
	add_debug_action({'>'}, "Reset towns (excludes the one you're in, if any)", debug_towns_forget);
	add_debug_action({'!'}, "Toggle Special Node Step-through Mode", debug_step_through);

	// Enter core scenarios, skipping intro:
	add_debug_action({'@'}, "Quick-launch The Valley of Dying Things", []() {debug_launch_scen("valleydy.boes");}, true);
	add_debug_action({'#'}, "Quick-launch A Small Rebellion", []() {debug_launch_scen("stealth.boes");}, true);
	add_debug_action({'$'}, "Quick-launch The Za-Khazi Run", []() {debug_launch_scen("zakhazi.boes");}, true);

	// std::bind won't work here for reasons
	add_debug_action({'%'}, "Fight wandering encounter from this section", []() {debug_fight_encounter(true);});
	add_debug_action({'^'}, "Fight special encounter from this section", []() {debug_fight_encounter(false);});
	add_debug_action({'/', '?'}, "Bring up this window", show_debug_help, true);
	add_debug_action({'Z'}, "Save the current action log for bug reporting", save_replay_log, true);
	add_debug_action({'\\'}, "Crash the game", debug_crash, true);
}

// Later we might want to know whether the key is used or not
static bool handle_debug_key(char key) {
	if(!univ.debug_mode && key != 'D')
		return false;
	if(overall_mode == MODE_STARTUP){
		if(startup_debug_actions.find(key) != startup_debug_actions.end()){
			startup_debug_actions[key].action();
			return true;
		}
	}else if(debug_actions.find(key) != debug_actions.end()){
		debug_actions[key].action();
		return true;
	}
	return false;
}

bool handle_keystroke(const sf::Event& event, cFramerateLimiter& fps_limiter){
	bool are_done = false;
	location pass_point; // TODO: This isn't needed
	using Key = sf::Keyboard::Key;
	
	Key keypad[10] = {
		Key::Numpad0,Key::Numpad1,Key::Numpad2,Key::Numpad3,
		Key::Numpad4,Key::Numpad5,Key::Numpad6,
		Key::Numpad7,Key::Numpad8,Key::Numpad9
	};
	// Terrain map coordinates to simulate a click for 8-directional movement/waiting
	// ordered to correspond with keypad keys
	location terrain_click[10] = {
		{150,185},{120,215},{150,215},{180,215},
		{120,185},{150,185},{180,185},
		{120,155},{150,155},{180,135}
	};
	// Screen shift deltas ordered to correspond with keypad keys
	location screen_shift_delta[10] = {
		{0,0},{-1,1},{0,1},{1,1},
		{-1,0},{0,0},{1,0},
		{-1,-1},{0,-1},{1,-1}
	};

	Key talk_chars[9] = {Key::L,Key::N,Key::J,Key::B,Key::S,Key::R,Key::D,Key::G,Key::A};
	Key shop_chars[8] = {Key::A,Key::B,Key::C,Key::D,Key::E,Key::F,Key::G,Key::H};

	bool did_something = false, need_redraw = false, need_reprint = false;
	
	if(event.key.code == Key::Escape){
		bool handled = true;
		switch(overall_mode){
			// These two cases are handled later:
			case MODE_TALKING: break;
			case MODE_SHOPPING: break;

			// Cancel use action
			case MODE_USE_TOWN:
				handle_use_space_select(need_reprint);
				break;
			// Cancel bash
			case MODE_BASH_TOWN:
				handle_bash_pick_select(need_reprint, true);
				break;
			// Cancel lockpick
			case MODE_PICK_TOWN:
				handle_bash_pick_select(need_reprint, false);
				break;
			// Cancel mage or priest spell
			case MODE_SPELL_TARGET: BOOST_FALLTHROUGH;
			case MODE_FANCY_TARGET: BOOST_FALLTHROUGH;
			case MODE_TOWN_TARGET:
				handle_spellcast(last_spellcast_type, did_something, need_redraw, need_reprint);
				break;
			// Cancel look
			case MODE_LOOK_TOWN: case MODE_LOOK_COMBAT: case MODE_LOOK_OUTDOORS:
				handle_begin_look(false, need_redraw, need_reprint);
				break;
			// Cancel talk
			case MODE_TALK_TOWN:
				handle_begin_talk(need_reprint);
				break;
			// Cancel missile
			case MODE_FIRING: BOOST_FALLTHROUGH;
			case MODE_THROWING:
				handle_missile(need_redraw, need_reprint);
				break;
			// Cancel item target
			case MODE_ITEM_TARGET:
				cancel_item_target(did_something, need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
				break;

			// Defer to closing the map:
			default:
				handled = false;
				break;
		}
		if(!handled && map_visible){
			close_map();
			return false;
		}
	}
	
	if(overall_mode != MODE_STARTUP)
		obscureCursor();
	
	// DEBUG
//	sprintf((char *) debug, "%d    ",(short) chr2);
//	add_string_to_buf((char *) debug);
//	print_buf();
	Key chr2 = event.key.code;
	
	if(chr2 == Key::LShift || chr2 == Key::LAlt || chr2 == Key::LControl || chr2 == Key::LSystem) return false;
	if(chr2 == Key::RShift || chr2 == Key::RAlt || chr2 == Key::RControl || chr2 == Key::RSystem) return false;
	
	if(chr2 == Key::Home) chr2 = Key::Numpad7;
	else if(chr2 == Key::End) chr2 = Key::Numpad1;
	else if(chr2 == Key::PageUp) chr2 = Key::Numpad9;
	else if(chr2 == Key::PageDown) chr2 = Key::Numpad3;
	
	// Startup menu can handle some debug actions:
	if(overall_mode == MODE_STARTUP){
		char chr = keyToChar(chr2, event.key.shift);
		return handle_debug_key(chr);
	}

	sf::Event pass_event = {sf::Event::MouseButtonPressed};
	extern rectangle talk_area_rect;
	if(overall_mode == MODE_TALKING) {
		if(chr2 == Key::Escape)
			chr2 = Key::D;
		if(chr2 == Key::Space)
			chr2 = Key::G;
		for(short i = 0; i < 9; i++)
			if(chr2 == talk_chars[i] && (!talk_end_forced || i == 6 || i == 5)) {
				// related to talk_area_rect, unsure why adding +9 is needed?
				pass_point = talk_words[i].rect.topLeft();
				pass_point.x += talk_area_rect.left+9;
				pass_point.y += talk_area_rect.top+9;
				pass_point = mainPtr().mapCoordsToPixel(pass_point, mainView);
				pass_event.mouseButton.x = pass_point.x;
				pass_event.mouseButton.y = pass_point.y;
				are_done = handle_action(pass_event, fps_limiter);
			}
	}
	else if(overall_mode == MODE_SHOPPING) { // shopping keystrokes
		if(chr2 == Key::Escape) {
			play_sound(37);
			end_shop_mode();
		}
		for(short i = 0; i < 8; i++)
			if(chr2 == shop_chars[i]) {
				pass_point = shopping_rects[i][SHOPRECT_ACTIVE_AREA].topLeft();
				// related to talk_area_rect
				pass_point.x += talk_area_rect.left+9;
				pass_point.y += talk_area_rect.top+9;
				pass_point = mainPtr().mapCoordsToPixel(pass_point, mainView);
				pass_event.mouseButton.x = pass_point.x;
				pass_event.mouseButton.y = pass_point.y;
				are_done = handle_action(pass_event, fps_limiter);
			}
	} else {
		for(short i = 0; i < 10; i++)
			if(chr2 == keypad[i]) {
				if(i == 0) {
					chr2 = Key::Z;
				}
				else {
					if(!handle_screen_shift(screen_shift_delta[i], need_redraw)){
						// Directional keys simulate directional click
						pass_point = mainPtr().mapCoordsToPixel(terrain_click[i], mainView);
						pass_event.mouseButton.x = pass_point.x;
						pass_event.mouseButton.y = pass_point.y;
						are_done = handle_action(pass_event, fps_limiter);
						return are_done;
					}
				}
			}
	}
	
	char chr = keyToChar(chr2, event.key.shift);
	// F1 should bring up help.
	// TODO: So should the help key, if it exists (but SFML doesn't support the help key)
	if(chr2 == Key::F1) chr = '?';
	
	switch(chr) {
			
		case '&':
			easter_egg(0);
			break;
		case '*':
			easter_egg(1);
			break;
		case '(':
			easter_egg(2);
			break;
		case ')':
			easter_egg(3);
			break;
			
		case '?':
			// In debug mode, override basic help menus with a debug mode cheat-sheet
			if(univ.debug_mode){
				show_debug_help();
				break;
			}
			if(overall_mode == MODE_SHOPPING) {
				give_help_and_record(226,27);
				break;
			}
			if(overall_mode == MODE_TALKING) {
				give_help_and_record(205,6);
				break;
			}
			if(is_out()) show_dialog_action("help-outdoor");
			if(is_town()) show_dialog_action("help-town");
			if(is_combat()) show_dialog_action("help-combat");
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
				advance_time(did_something, need_redraw, need_reprint);
			} else if(overall_mode == MODE_SPELL_TARGET)
				// Rotate a force wall
				spell_cast_hit_return();
			else if(overall_mode == MODE_ITEM_TARGET) {
				// Cancel choosing items
				cancel_item_target(did_something, need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			} else if(overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT || overall_mode == MODE_OUTDOORS) {
				// Pause (skip turn)
				handle_pause(did_something, need_redraw);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		// 'z', Really?
		case 'z':
		case 'i':
			show_inventory();
			break;
			
		case 'a': // Show automap
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
		case 'M': spell_forced = true; BOOST_FALLTHROUGH;
		case 'm':
			if(overall_mode == MODE_SPELL_TARGET || overall_mode == MODE_FANCY_TARGET || overall_mode == MODE_TOWN_TARGET || overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) {
				handle_spellcast(eSkill::MAGE_SPELLS, did_something, need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'P': spell_forced = true; BOOST_FALLTHROUGH;
		case 'p':
			if(overall_mode == MODE_SPELL_TARGET || overall_mode == MODE_FANCY_TARGET || overall_mode == MODE_TOWN_TARGET || overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) {
				handle_spellcast(eSkill::PRIEST_SPELLS, did_something, need_redraw, need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'l': // Look
			if((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_TOWN) || (overall_mode == MODE_COMBAT) || (overall_mode == MODE_LOOK_OUTDOORS) || (overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT)) {
				handle_begin_look(false, need_redraw, need_reprint);
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
			if(overall_mode == MODE_TOWN || overall_mode == MODE_TALK_TOWN) {
				handle_begin_talk(need_reprint);
				advance_time(did_something, need_redraw, need_reprint);
			}
			break;
			
		case 'w': // Wait / delay
			handle_wait(did_something, need_redraw, need_reprint);
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

		// Debug action keyboard shortcuts:
		default:
			handle_debug_key(chr);
			break;
	}
	spell_forced = false;
	return are_done;
}

bool handle_scroll(const sf::Event& event) {
	rectangle world_screen = win_to_rects[WINRECT_TERVIEW];
	world_screen.inset(13, 13);
	fill_rect(mainPtr(), world_screen, sf::Color::Magenta);
	
	// TODO: centralize this translation somewhere
	location pos(event.mouseWheel.x, event.mouseWheel.y);
	pos = mainPtr().mapPixelToCoords(pos, mainView);
	
	int amount = event.mouseWheel.delta;
	if(scrollableModes.count(overall_mode) && pos.in(world_screen)) {
		if(kb.isCtrlPressed())
			center.x = minmax(4, univ.town->max_dim - 5, center.x - amount);
		else center.y = minmax(4, univ.town->max_dim - 5, center.y - amount);
	}
	return true;
}

void do_load() {
	// TODO this needs to be changed/moved because a picker dialog opens now!!!
	// TODO wait, I thought I resolved that.^
	// Edge case: Replay can be cut off before a file is chosen,
	// or party selection can be canceled, and this will cause
	// a crash trying to decode a party
	if(replaying && !has_next_action("load_party")){
		return;
	}
	fs::path file_to_load = run_file_picker(false);
	if(file_to_load.empty()) return;
	set_cursor(watch_curs);
	if(!load_party(file_to_load, univ))
		return;
	finish_load_party();
	if(overall_mode != MODE_STARTUP)
		post_load();
	univ.file = file_to_load;
	menu_activate();
	restore_cursor();
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
	UI::toolbar.draw(mainPtr());
	draw_text_bar();
	
	print_buf();
	
	clear_map();
	adjust_spell_menus();
	adjust_monst_menu();
}

//mode; // 0 - normal  1 - save as
void do_save(bool save_as) {
	if(is_combat()){
		add_string_to_buf("Save: not in combat.", 2);
		print_buf();
		return;
	}
	if(overall_mode != MODE_TOWN && overall_mode != MODE_OUTDOORS && overall_mode != MODE_STARTUP) {
		add_string_to_buf("Save: Only while outdoors, or in town and not looking/casting.", 2);
		print_buf();
		return;
	}
	
	if(univ.party.is_in_scenario()) save_outdoor_maps();
	
	if(save_party(univ, save_as)){
		add_string_to_buf("Save: Game saved");
	}else{
		add_string_to_buf("Save: Save not completed");
	}

	// Cancel switching PC order
	current_switch = 6;

	pause(6);
	redraw_screen(REFRESH_TEXT);
}

void do_abort() {
	if(recording){
		record_action("do_abort", "");
	}
	if(party_in_memory) {
		std::string choice = cChoiceDlog("abort-game",{"okay","cancel"}).show();
		if (choice=="cancel") return;
		reload_startup();
		overall_mode = MODE_STARTUP;
	}
	party_in_memory = false;
	draw_startup(0);
	menu_activate();
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
				if(item.abil_data.value > 15) continue;
				if(!item.abil_group()) continue;
				if(get_ran(1,0,5) != 3) continue;
				int how_much = item.abil_strength;
				if(item.abil_harms()) how_much *= -1;
				if((*item.abil_data.status).isNegative)
					how_much *= -1;
				univ.party.apply_status(item.abil_data.status, how_much);
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
				int j = get_ran(1,0,item->abil_strength / 3);
				if(item->abil_strength / 3 == 0)
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

void increase_age(bool eating_trigger_autosave) {
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
				if(item.abil_data.value > 15) continue;
				if(!item.abil_group()) continue;
				if(get_ran(1,0,5) != 3) continue;
				int how_much = item.abil_strength;
				if(item.abil_harms()) how_much *= -1;
				eStatus status = item.abil_data.status;
				if((*status).isNegative)
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
			if(eating_trigger_autosave) try_auto_save("Eat");
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
					int j = get_ran(1,0,item->abil_strength / 3);
					if(item->abil_strength / 3 == 0)
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

void handle_drop_pc() {
	if(recording){
		record_action("handle_drop_pc", "");
	}
	if(!prime_time()) {
		ASB("Delete PC: " + FINISH_FIRST);
		print_buf();
	}else if(is_combat()){
		add_string_to_buf("Delete PC: Not in combat.");
	}else{
		int choice = select_pc(eSelectPC::ANY,"Delete who?");
		if(choice < 6) {
			std::string confirm = cChoiceDlog("delete-pc-confirm",{"yes","no"}).show();
			if(confirm == "no"){
				add_string_to_buf("Delete PC: Cancelled.");
			}else if(confirm == "yes"){
				add_string_to_buf("Delete PC: OK.");
				kill_pc(univ.party[choice],eMainStatus::ABSENT);
				draw_terrain();
			}
		}
	}
}

void handle_new_pc() {
	if(recording){
		record_action("handle_new_pc", "");
	}
	if(!(is_town())) {
		add_string_to_buf("Add PC: Town mode only.");
		print_buf();
		return;
	}
	if(univ.party.free_space() == 6) {
		ASB("Add PC: You already have 6 PCs.");
		print_buf();
		return;
	}
	if(univ.town->has_tavern) {
		give_help(56,0);
		create_pc(6,nullptr);
	} else {
		add_string_to_buf("Add PC: You cannot add new characters in this town. Try in the town you started in.", 2);
	}
	print_buf();
	put_pc_screen();
	put_item_screen(stat_window);
}

void new_party() {
	if(recording){
		record_action("new_party", "");
	}
	if(overall_mode != MODE_STARTUP) {
		std::string choice = cChoiceDlog("restart-game",{"okay","cancel"}).show();
		if(choice == "cancel")
			return;
		for(short i = 0; i < 6; i++)
			univ.party[i].main_status = eMainStatus::ABSENT;
		party_in_memory = false;
		reload_startup();
		overall_mode = MODE_STARTUP;
		draw_startup(0);
	}
	start_new_game();
	draw_startup(0);
	menu_activate();
}

void handle_death() {
	std::string choice;
	
	overall_mode = MODE_STARTUP;
	reload_startup();
	
	while(true) {
		// Use death (or leave Exile) dialog
		choice = cChoiceDlog("party-death",{"load","new","quit"}).show();
		
		if(choice == "quit") {
			All_Done = true;
			return;
		}
		else if(choice == "load") {
			fs::path file_to_load = run_file_picker(false);
			if(!file_to_load.empty()){
				if(load_party(file_to_load, univ)){
					finish_load_party();
					if(overall_mode != MODE_STARTUP)
						post_load();
					return;
				}
			}
		}
		else if(choice == "new") {
			// TODO: Windows version dumps to main screen without creating a party; which is better?
			start_new_game();
			return;
		}
	}
	
}

void start_new_game(bool force) {
	std::string choice;
	
	if(!force)
		choice = cChoiceDlog("new-party",{"okay","cancel"}).show();
	if(choice == "cancel")
		return;
	
	// Destroy the universe
	univ.~cUniverse();
	
	ePartyPreset party_type = PARTY_DEFAULT;
	
//	display_intro();
	// If system key held down, create debug party
	if(kb.isSystemPressed()) party_type = PARTY_DEBUG;
	
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
	do_save(true);
}

void start_tutorial() {
	// Start by using the default party
	start_new_game(true);
	// TODO start the tutorial scenario, which we need to design.
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
	std::bitset<8> to_dir;
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
	for(int i = DIR_N; i < DIR_HERE; i++)
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
			int lost = percent(univ.party.food, ter.flag2);
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
		univ.party.boats[univ.party.in_boat].loc = global_to_local(univ.party.out_loc);
		univ.party.boats[univ.party.in_boat].sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
		univ.party.boats[univ.party.in_boat].sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
	}
}

void outd_move_to_first_town_entrance(int town) {
	// Try to put the party in an outdoor section from which you can enter the town --
	// so when you leave, you'll hopefully be in the right place.
	auto town_entrances = univ.scenario.find_town_entrances(town);
	if(!town_entrances.empty()){
		// When there are multiple entrances, this part of the code shouldn't matter,
		// but also won't hurt.
		town_entrance_t first_entrance = town_entrances[0];
		position_party(first_entrance.out_sec.x, first_entrance.out_sec.y, first_entrance.loc.x, first_entrance.loc.y);
	}
}

bool outd_move_party(location destination,bool forced) {
	location real_dest, sector_p_in;
	bool keep_going = true,check_f;
	location store_corner,store_iwc;
	ter_num_t ter;
	
	if(!univ.out.is_on_map(destination.x, destination.y)) return false;
	
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
				univ.party.horses[univ.party.in_horse].loc = global_to_local(univ.party.out_loc);
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
	bool check_f = false;
	
	if(univ.town.is_force_cage(univ.party.town_loc.x, univ.party.town_loc.y)) {
		add_string_to_buf("Move: Can't escape.");
		return false;
	}
	
	if(!univ.town.is_on_map(destination.x, destination.y)) return false;
	
	if(univ.target_there(destination, TARG_MONST) == nullptr)
		keep_going = check_special_terrain(destination,eSpecCtx::TOWN_MOVE,univ.party[0],&check_f);
	if(check_f)
		forced = true;
	if(univ.debug_mode && univ.ghost_mode)
		forced = keep_going = true;
	
	if(keep_going) {
		ter_num_t ter = univ.town->terrain(destination.x,destination.y);
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

bool check_for_interrupt(std::string confirm_dialog){
	using kb = sf::Keyboard;
	bool interrupt = false;
	sf::Event evt;
	if(replaying && confirm_dialog == "confirm-interrupt-special" && has_next_action() && next_action_type() == "handle_interrupt"){
		pop_next_action();
		interrupt = true;
	}
	else if(pollEvent(mainPtr(), evt) && evt.type == sf::Event::KeyPressed) {
		// TODO: I wonder if there are other events we should handle here? Resize maybe?
#ifdef __APPLE__
		if(evt.key.code == kb::Period && evt.key.system)
			interrupt = true;
#endif
		if(evt.key.code == kb::C && evt.key.control)
			interrupt = true;
	}
	if(interrupt) {
		if(recording){
			record_action("handle_interrupt", "");
		}
		cChoiceDlog confirm(confirm_dialog, {"quit","cancel"});
		bool was_replaying = replaying;
		if(confirm_dialog == "confirm-interrupt-replay"){
			// There's a slight chance the next action could be snatched up by the replay system to respond
			// to the yes/no prompt, so suspend the replay loop
			replaying = false;
		}
		std::string result = confirm.show();
		replaying = was_replaying;
		if(result == "quit") return true;
	}
	return false;
}

void handle_new_pc_graphic() {
	if(recording){
		record_action("handle_new_pc_graphic", "");
	}
	short choice = select_pc(eSelectPC::ANY,"New graphic for who?");
	if(choice < 6)
		pick_pc_graphic(choice,1,nullptr);
	draw_terrain();
}

void handle_rename_pc() {
	if(recording){
		record_action("handle_rename_pc", "");
	}
	short choice = select_pc(eSelectPC::ANY,"Rename who?");
	if(choice < 6)
		pick_pc_name(choice,nullptr);
	put_pc_screen();
	put_item_screen(stat_window);
}

void preview_dialog_xml() {
	fs::path dialog_xml = nav_get_rsrc({"xml"});
	if(!dialog_xml.empty()) preview_dialog_xml(dialog_xml);
}

void preview_every_dialog_xml() {
	if(recording){
		record_action("preview_every_dialog_xml", "");
	}
	std::vector<fs::path> dialog_paths;
	fs::path dialogs_path = "data/dialogs";
	for(fs::directory_iterator it{dialogs_path}; it != fs::directory_iterator{}; ++it) {
		fs::path path = it->path();
		if(path.extension() != ".xml") continue;
		dialog_paths.push_back(path);
	}
	cChoiceDlog dlog("preview-dialogs-confirm",{"yes","no"});
	std::string text = dlog->getControl("msg").getText();
	boost::replace_first(text, "{{num}}", boost::lexical_cast<std::string>(dialog_paths.size()));
	dlog->getControl("msg").setText(text);
	std::string confirm = dlog.show();
	if(confirm == "yes"){
		std::for_each(dialog_paths.begin(), dialog_paths.end(), [](fs::path path) {
			ASB("Previewing dialog: " + path.stem().string());
			print_buf();
			preview_dialog_xml(path);
		});
	}
}

void save_replay_log(){
	// This doesn't need to be recorded or replayed.
	if(replaying) return;

	fs::path out_file = nav_put_rsrc({"xml"});

	start_log_file(out_file.string());
}

void debug_crash() {
	// If they don't confirm, the game can continue normally, and we'll need to replay
	// that the confirmation opened and closed.
	if(recording){
		record_action("debug_crash", "");
	}
	std::string confirm = cChoiceDlog("debug-crash-confirm",{"yes","no"}).show();
	if(confirm == "yes"){
		throw std::string { "Be careful what you wish for!" };
	}
}

void clear_trapped_monst() {
	if(recording){
		record_action("clear_trapped_monst", "");
	}
	univ.party.imprisoned_monst.fill(0);
	ASB("Debug: Captures souls cleared.");
	print_buf();
}
