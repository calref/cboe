
#include "boe.global.hpp"
#include "tools/replay.hpp"
#include "universe/universe.hpp"
#include "cli.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <ctime>
#include <deque>
#include "boe.graphics.hpp"
#include "boe.newgraph.hpp"
#include "boe.fileio.hpp"
#include "boe.actions.hpp"
#include "boe.text.hpp"
#include "boe.party.hpp"
#include "boe.items.hpp"
#include "boe.locutils.hpp"
#include "boe.town.hpp"
#include "boe.dlgutil.hpp"
#include "boe.infodlg.hpp"
#include "boe.main.hpp"
#include "boe.consts.hpp"
#include "boe.ui.hpp"
#include "boe.specials.hpp"
#include "tools/winutil.hpp"
#include "sounds.hpp"
#include "gfx/render_image.hpp"
#include "gfx/tiling.hpp"
#include "mathutil.hpp"
#include "fileio/fileio.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include "boe.menus.hpp"
#include "tools/cursors.hpp"
#include "tools/keymods.hpp"
#include "tools/prefs.hpp"
#include "dialogxml/widgets/button.hpp"
#include "tools/enum_map.hpp"
#include "tools/event_listener.hpp"
#include "tools/drawable_manager.hpp"
bool All_Done = false;
sf::RenderWindow mainPtr;
short had_text_freeze = 0,num_fonts;
bool first_startup_update = true;
bool first_sound_played = false,spell_forced = false;
bool party_in_memory = false;
std::shared_ptr<cScrollbar> text_sbar, item_sbar, shop_sbar;
std::shared_ptr<cButton> done_btn, help_btn;
// TODO: move these 3 to boe.ui.cpp ?
extern const rectangle sbar_rect      = {285,560,423,576};
extern const rectangle shop_sbar_rect = {69,272,359,288};
extern const rectangle item_sbar_rect = {148,560,255,576};
bool bgm_on = false,bgm_init = false;
location store_anim_ul;
cUniverse univ;

bool flushingInput = false, ae_loading = false;
long start_time;

std::deque<sf::Event> fake_event_queue;

short on_spell_menu[2][62];
short on_monst_menu[256];

extern bool map_visible;
extern sf::View mainView;

extern rectangle shop_frame;
extern enum_map(eGuiArea, rectangle) win_to_rects;

std::string scenario_temp_dir_name = "scenario";
extern fs::path tempDir;

/* Display globals */
short combat_posing_monster = -1, current_working_monster = -1; // 0-5 PC 100 + x - monster x
short fast_bang = false; // Note: This mostly behaves as a boolean variable, but values other than 1 are assigned to it, so I made it a short
std::vector<int> spec_item_array;
short current_spell_range;
eGameMode overall_mode = MODE_STARTUP;
bool changed_display_mode = false;
eItemWinMode stat_window = ITEM_WIN_PC1;
bool monsters_going = false,boom_anim_active = false;
bool finished_init = false;

sf::RenderWindow mini_map;
short which_item_page[6] = {0,0,0,0,0,0}; // Remembers which of the 2 item pages pc looked at
short current_ground = 0;
eStatMode stat_screen_mode;
short anim_step = -1;

// Spell casting globals
eSpell store_mage = eSpell::LIGHT, store_priest = eSpell::BLESS_MINOR;
short store_mage_lev = 0, store_priest_lev = 0;
short store_spell_target = 6,pc_casting;
short num_targets_left = 0;
location spell_targets[8];

extern void spell_cast_hit_return();

/* Combat globals */
short which_combat_type;
location center;
short combat_active_pc;
effect_pat_type current_pat;
short missile_firer,current_monst_tactic;
short store_current_pc = 0;

// TODO: these should be members of some global entity instead of being here
std::unordered_map <std::string, std::shared_ptr <iEventListener>> event_listeners;
cDrawableManager drawable_mgr;

sf::Clock animTimer;
extern long anim_ticks;

static void init_boe(int, char*[]);
static void showWelcome();

void handle_quit_event();
void handle_help_toc();
void menu_give_help(short help1);

extern void edit_stuff_done();

#ifdef __APPLE__
eMenuChoice menuChoice=eMenuChoice::MENU_CHOICE_NONE;
short menuChoiceId=-1;
#endif

int main(int argc, char* argv[]) {
#if 0
	void debug_oldstructs();
	debug_oldstructs();
#endif
	try{
		cDialog::redraw_everything = &redraw_everything;

		init_boe(argc, argv);
		
		if(!get_bool_pref("GameRunBefore"))
			showWelcome();
		else if(get_bool_pref("GiveIntroHint", true))
			tip_of_day();
		set_pref("GameRunBefore", true);
		finished_init = true;
		
		if(ae_loading) {
			finish_load_party();
			post_load();
		}
		
		menu_activate();
		restore_cursor();

		handle_events();

		close_program();
		return 0;
	} catch(std::exception& x) {
		showFatalError(x.what());
		throw;
	} catch(std::string& x) {
		showFatalError(x);
		throw;
	} catch(...) {
		showFatalError("An unknown error occurred!");
		throw;
	}
}

static void init_sbar(std::shared_ptr<cScrollbar>& sbar, const std::string& name, rectangle rect, rectangle events_rect, int max, int pgSz, int start = 0) {
	sbar.reset(new cScrollbar(mainPtr));
	sbar->setName(name);
	sbar->setBounds(rect);
	sbar->setMaximum(max);
	sbar->setPosition(start);
	sbar->setPageSize(pgSz);
	sbar->set_wheel_event_rect(events_rect);
	sbar->hide();
	
	drawable_mgr.add_drawable(UI_LAYER_DEFAULT, name, sbar);
	event_listeners[name] = std::dynamic_pointer_cast<iEventListener>(sbar);
}

static void init_scrollbars() {

	// Cover entire transcript + scrollbar
	const rectangle transcript_events_rect {
		win_to_rects[WINRECT_TRANSCRIPT].top,
		win_to_rects[WINRECT_TRANSCRIPT].left,
		sbar_rect.bottom,
		sbar_rect.right
	};
	
	// Cover entire inventory + scrollbar
	const rectangle inventory_events_rect {
		win_to_rects[WINRECT_INVEN].top,
		win_to_rects[WINRECT_INVEN].left,
		item_sbar_rect.bottom,
		item_sbar_rect.right
	};
	
	// MAGIC NUMBERS: max size, page size, initial position - all in abstract "step" units
	init_sbar(text_sbar, "transcript-scrollbar", sbar_rect, transcript_events_rect, 58, 11, 58);
	init_sbar(item_sbar, "inventory-scrollbar", item_sbar_rect, inventory_events_rect, 16, 8);
	init_sbar(shop_sbar, "shop-scrollbar", shop_sbar_rect, shop_frame, 16, 8);	
}

static void init_btn(std::shared_ptr<cButton>& btn, eBtnType type, location loc) {
	btn.reset(new cButton(mainPtr));
	btn->setBtnType(type);
	btn->relocate(loc);
	btn->hide();
}

static void init_buttons() {
	
	// MAGIC NUMBERS: move to boe.ui.cpp ?
	
	init_btn(done_btn, BTN_DONE, {231,395});
	init_btn(help_btn, BTN_HELP, {273,12});
}

// NOTE: this should possibly be moved to boe.ui.cpp at some point
static void init_ui() {
	cDialog::init();
	init_scrollbars();
	init_buttons();
}

extern bool record_verbose;
extern bool replay_verbose;
extern bool replay_strict;

static void process_args(int argc, char* argv[]) {
	preprocess_args(argc, argv);
	clara::Args args(argc, argv);
	clara::Parser cli;
	bool record_unique = false;
	boost::optional<std::string> record_to, replay, saved_game;
	boost::optional<double> replay_speed;
	cli |= clara::Opt(record_to, "record")["--record"]("Records a replay of your session to the specified XML file.");
	cli |= clara::Opt(record_unique)["--unique"]("When recording, automatically insert a timestamp into the filename to guarantee uniqueness.");
	cli |= clara::Opt(record_verbose)["--verbose"]("Record extra information for internal testing of the replay system.");
	cli |= clara::Opt(replay, "replay-file")["--replay"]("Replays a previously-recorded session from the specified XML file.");
	cli |= clara::Opt(replay_strict)["--strict"]("Enforces strictly identical replay behavior, even where this is only cosmetic");
	cli |= clara::Opt(replay_speed, "fps")["--replay-speed"]("Specifies how quickly actions are processed while replaying");
	cli |= clara::Arg(saved_game, "save-file")("Launch and load a saved game file.");
	bool show_help = false;
	cli |= clara::Help(show_help);
	if(auto result = cli.parse(args)); else {
		std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
		exit(1);
	}
	if(show_help) {
		cli.writeToStream(std::cout);
		exit(0);
	}
	if(replay){
		if(record_to){
			std::cout << "Warning: flag --record conflicts with --replay and will be ignored." << std::endl;
		}
		if(saved_game){
			std::cout << "Warning: save file argument conflicts with --replay and will be ignored." << std::endl;
		}
		if(!init_action_log("replay", *replay)) {
			std::cerr << "Failed to load replay: " << *replay << std::endl;
			exit(1);
		}
		if(replay_speed) {
			extern boost::optional<cFramerateLimiter> replay_fps_limit;
			replay_fps_limit.emplace(*replay_speed);
		}
		return;
	}
	if(record_to){
		if(!init_action_log(record_unique || record_to->empty() ? "record-unique" : "record", *record_to)){
			std::cerr << "Failed to start recording: " << *record_to << std::endl;
			exit(1);
		}
		return;
	}
	if(saved_game) {
		if(!load_party(*saved_game, univ)) {
			std::cout << "Failed to load save file: " << *saved_game << std::endl;
			return;
		}

		if(!finished_init) {
			ae_loading = true;
			overall_mode = MODE_STARTUP;
		} else finish_load_party();
		if(overall_mode != MODE_STARTUP)
			end_startup();
		if(overall_mode != MODE_STARTUP)
			post_load();
	}
}

static void replay_next_action() {
	bool did_something = false, need_redraw = false, need_reprint = false;

	std::string _last_action_type = last_action_type;
	Element& next_action = pop_next_action();
	std::string t = next_action.Value();
	int enum_v;
	
	// NOTE: Action replay blocks need to return early unless the action advances time
	if(overall_mode == MODE_STARTUP && t == "startup_button_click"){
		auto info = info_from_action(next_action);
		eStartButton btn = static_cast<eStartButton>(std::stoi(info["btn"]));
		eKeyMod mods = static_cast<eKeyMod>(std::stoi(info["mods"]));
		handle_startup_button_click(btn, mods);
		return;
	}else if(t == "load_party"){
		decode_file(next_action.GetText(), tempDir / "temp.exg");
		load_party(tempDir / "temp.exg", univ);

		finish_load_party();

		if(overall_mode != MODE_STARTUP)
			post_load();

		menu_activate();
		return;
	}else if(t == "move"){
		location l = location_from_action(next_action);
		handle_move(l, did_something, need_redraw, need_reprint);
	}else if(t == "handle_switch_pc"){
		short which_pc = short_from_action(next_action);
		handle_switch_pc(which_pc, need_redraw, need_reprint);
	}else if(t == "handle_switch_pc_items"){
		short which_pc = short_from_action(next_action);
		handle_switch_pc_items(which_pc, need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "handle_equip_item"){
		short item_hit = short_from_action(next_action);
		handle_equip_item(item_hit, need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "handle_use_item"){
		short item_hit = short_from_action(next_action);
		handle_use_item(item_hit, did_something, need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "handle_item_shop_action"){
		short item_hit = short_from_action(next_action);
		handle_item_shop_action(item_hit);
		update_item_stats_area(need_reprint);
	}else if(t == "handle_alchemy"){
		handle_alchemy(need_redraw, need_reprint);
		return;
	}else if(t == "handle_wait"){
		handle_wait(did_something, need_redraw, need_reprint);
		return;
	}else if(t == "handle_combat_switch"){
		handle_combat_switch(did_something, need_redraw, need_reprint);
	}else if(t == "handle_missile"){
		handle_missile(need_redraw, need_reprint);
	}else if(t == "handle_get_items"){
		handle_get_items(did_something, need_redraw, need_reprint);
	}else if(t == "handle_drop_item_id"){
		short item_hit = short_from_action(next_action);
		handle_drop_item(item_hit, need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "handle_drop_item_location"){
		location destination = location_from_action(next_action);
		handle_drop_item(destination, need_redraw);
	}else if(t == "handle_give_item"){
		short item_hit = short_from_action(next_action);
		handle_give_item(item_hit, did_something, need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "close_window"){
		handle_quit_event();
		return;
	}else if(t == "arrow_button_click"){
		rectangle button_rect = boost::lexical_cast<rectangle>(next_action.GetText());
		arrow_button_click(button_rect);
		return;
	}else if(t == "show_dialog_action"){
		show_dialog_action(next_action.GetText());
		return;
	}else if(t == "handle_drop_pc"){
		handle_drop_pc();
		return;
	}else if(t == "new_party"){
		new_party();
		return;
	}else if(t == "pick_preferences"){
		pick_preferences();
		return;
	}else if(t == "do_abort"){
		do_abort();
		return;
	}else if(t == "handle_new_pc_graphic"){
		handle_new_pc_graphic();
		return;
	}else if(t == "handle_rename_pc"){
		handle_rename_pc();
		return;
	}else if(t == "handle_new_pc"){
		handle_new_pc();
		return;
	}else if(t == "journal"){
		journal();
		return;
	}else if(t == "talk_notes"){
		talk_notes();
		return;
	}else if(t == "adventure_notes"){
		adventure_notes();
		return;
	}else if(t == "print_party_stats"){
		print_party_stats();
		return;
	}else if(t == "display_alchemy"){
		display_alchemy();
		return;
	}else if(t == "display_spells"){
		auto info = info_from_action(next_action);
		eSkill mode = boost::lexical_cast<eSkill>(info["mode"]);
		short force_spell = boost::lexical_cast<short>(info["force_spell"]);

		display_spells(mode, force_spell, nullptr);
		return;
	}else if(t == "display_skills"){
		eSkill force_skill = boost::lexical_cast<eSkill>(next_action.GetText());

		display_skills(force_skill, nullptr);
		return;
	}else if(t == "tip_of_day"){
		tip_of_day();
		return;
	}else if(t == "showWelcome"){
		showWelcome();
		return;
	}else if(t == "display_map"){
		display_map();
		return;
	}else if(t == "handle_help_toc"){
		handle_help_toc();
		return;
	}else if(t == "menu_give_help"){
		short help1 = short_from_action(next_action);
		menu_give_help(help1);
		return;
	}else if(t == "handle_begin_look"){
		bool right_button = str_to_bool(next_action.GetText());
		handle_begin_look(right_button, need_redraw, need_reprint);
		if (right_button){
			return;
		}
	}else if(t == "handle_look"){
		auto info = info_from_action(next_action);
		location destination = boost::lexical_cast<location>(info["destination"]);
		bool right_button = str_to_bool(info["right_button"]);
		eKeyMod mods = static_cast<eKeyMod>(std::stoi(info["mods"]));

		handle_look(destination, right_button, mods, need_redraw, need_reprint);
	}else if(t == "screen_shift"){
		auto info = info_from_action(next_action);
		int dx = std::stoi(info["dx"]);
		int dy = std::stoi(info["dy"]);

		screen_shift(dx, dy, need_redraw);
	}else if(t == "handle_rest"){
		handle_rest(need_redraw, need_reprint);
	}else if(t == "handle_menu_spell"){
		eSpell spell_picked = static_cast<eSpell>(std::stoi(next_action.GetText()));
		handle_menu_spell(spell_picked);
		return;
	}else if(t == "handle_spellcast"){
		auto info = info_from_action(next_action);
		eSkill which_type = boost::lexical_cast<eSkill>(info["which_type"]);
		spell_forced = str_to_bool(info["spell_forced"]);

		handle_spellcast(which_type, did_something, need_redraw, need_reprint);
	}else if(t == "handle_target_space"){
		auto info = info_from_action(next_action);
		location destination = boost::lexical_cast<location>(info["destination"]);
		num_targets_left = boost::lexical_cast<short>(info["num_targets_left"]);

		handle_target_space(destination, did_something, need_redraw, need_reprint);
	}else if(t == "spell_cast_hit_return"){
		spell_cast_hit_return();
		return;
	}else if(t == "handle_pause"){
		handle_pause(did_something, need_redraw);
	}else if(t == "handle_bash_select"){
		handle_bash_pick_select(need_reprint, true);
		return;
	}else if(t == "handle_pick_select"){
		handle_bash_pick_select(need_reprint, false);
		return;
	}else if(t == "handle_bash"){
		location destination = location_from_action(next_action);
		handle_bash_pick(destination, did_something, need_redraw, true);
	}else if(t == "handle_pick"){
		location destination = location_from_action(next_action);
		handle_bash_pick(destination, did_something, need_redraw, false);
	}else if(t == "handle_use_space_select"){
		handle_use_space_select(need_reprint);
	}else if(t == "handle_use_space"){
		location destination = location_from_action(next_action);
		handle_use_space(destination, did_something, need_redraw);
	}else if(t == "show_inventory"){
		show_inventory();
		return;
	}else if(t == "give_help"){
		auto info = info_from_action(next_action);
		short help1 = boost::lexical_cast<short>(info["help1"]);
		short help2 = boost::lexical_cast<short>(info["help2"]);
		give_help(help1, help2);
	}else if(t == "toggle_debug_mode"){
		toggle_debug_mode();
		return;
	}else if(t == "debug_give_item"){
		debug_give_item();
		return;
	}else if(t == "debug_print_location"){
		debug_print_location();
		return;
	}else if(t == "debug_step_through"){
		debug_step_through();
		return;
	}else if(t == "debug_leave_town"){
		debug_leave_town();
		return;
	}else if(t == "debug_kill"){
		debug_kill();
		return;
	}else if(t == "debug_magic_map"){
		debug_magic_map();
		return;
	}else if(t == "debug_enter_town"){
		debug_enter_town();
		return;
	}else if(t == "debug_refresh_stores"){
		debug_refresh_stores();
		return;
	}else if(t == "debug_clean_up"){
		debug_clean_up();
		return;
	}else if(t == "debug_stealth_detect_life_firewalk"){
		debug_stealth_detect_life_firewalk();
		return;
	}else if(t == "debug_fly"){
		debug_fly();
		return;
	}else if(t == "debug_ghost_mode"){
		debug_ghost_mode();
		return;
	}else if(t == "debug_return_to_start"){
		debug_return_to_start();
		return;
	}else if(t == "handle_victory"){
		handle_victory();
		return;
	}else if(t == "debug_increase_age"){
		debug_increase_age();
		return;
	}else if(t == "debug_towns_forget"){
		debug_towns_forget();
		return;
	}else if(t == "edit_stuff_done"){
		edit_stuff_done();
		return;
	}else if(t == "debug_heal"){
		debug_heal();
		return;
	}else if(t == "debug_heal_plus_extra"){
		debug_heal_plus_extra();
		return;
	}else if(t == "handle_print_pc_hp"){
		handle_print_pc_hp(boost::lexical_cast<int>(next_action.GetText()), need_reprint);
	}else if(t == "handle_print_pc_sp"){
		handle_print_pc_sp(boost::lexical_cast<int>(next_action.GetText()), need_reprint);
	}else if(t == "give_pc_info"){
		give_pc_info(boost::lexical_cast<short>(next_action.GetText()));
		return;
	}else if(t == "handle_trade_places"){
		handle_trade_places(boost::lexical_cast<short>(next_action.GetText()), need_reprint);
	}else if(t == "handle_begin_talk"){
		handle_begin_talk(need_reprint);
	}else if(t == "handle_talk"){
		handle_talk(location_from_action(next_action), did_something, need_redraw, need_reprint);
	}else if(t == "click_talk_rect"){
		word_rect_t word_rect = word_rect_from_action(next_action);
		click_talk_rect(word_rect);
		handle_talk_node(word_rect.node);
		return;
	}else if(t == "click_shop_rect"){
		rectangle rect = boost::lexical_cast<rectangle>(next_action.GetText());
		click_shop_rect(rect);
		return;
	}else if(t == "end_shop_mode"){
		end_shop_mode();
		return;
	}else if(t == "scrollbar_setPosition"){
		auto info = info_from_action(next_action);
		std::string name = info["name"];
		long newPos = boost::lexical_cast<long>(info["newPos"]);

		std::shared_ptr<cScrollbar> sbar = std::dynamic_pointer_cast<cScrollbar>(event_listeners[name]);
		sbar->setPosition(newPos);
		return;
	}else if(t == "use_spec_item"){
		use_spec_item(boost::lexical_cast<short>(next_action.GetText()), need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "show_item_info"){
		show_item_info(boost::lexical_cast<short>(next_action.GetText()));
		update_item_stats_area(need_reprint);
		return;
	}else if(t == "set_stat_window"){
		set_stat_window(static_cast<eItemWinMode>(boost::lexical_cast<int>(next_action.GetText())));
		update_item_stats_area(need_reprint);
	}else if(t == "handle_sale"){
		handle_sale(boost::lexical_cast<int>(next_action.GetText()));
	}else if(t == "handle_info_request"){
		handle_info_request(boost::lexical_cast<int>(next_action.GetText()));
	}else if(t == "close_map"){
		close_map(true);
		return;
	}else if(t == "handle_toggle_active"){
		handle_toggle_active(need_reprint);
	}else if(t == "handle_parry"){
		handle_parry(did_something, need_redraw, need_reprint);
	}else if(t == "handle_monster_info_menu"){
		handle_monster_info_menu(boost::lexical_cast<int>(next_action.GetText()));
		return;
	}else if(t == "cancel_item_target"){
		cancel_item_target(did_something, need_redraw, need_reprint);
	}else if(t == "advance_time"){
		// This is bad regardless of strictness, because visual changes may have occurred which won't get redrawn/reprinted
		throw std::string { "Replay system internal error! advance_time() was supposed to be called by the last action, but wasn't: " } + _last_action_type;
	}else{
		std::ostringstream sstr;
		sstr << "Couldn't replay action: " << next_action;
		throw sstr.str();
	}

	// NOTE: commands that do not advance time must return early
	advance_time(did_something, need_redraw, need_reprint);
}

void init_boe(int argc, char* argv[]) {
	set_up_apple_events();
	init_directories(argv[0]);
	process_args(argc, argv);
#ifdef __APPLE__
	init_menubar(); // Do this first of all because otherwise a default File and Window menu will be seen
#endif

	sync_prefs();
	init_shaders();
	init_tiling();
	init_snd_tool();
	
	adjust_window_mode();
	init_ui();
	// If we don't do this now it'll flash white to start with
	mainPtr.clear(sf::Color::Black);
	mainPtr.display();
	
	set_cursor(watch_curs);
	init_buf();

	// Seed the RNG
	if(replaying) {
		Element& srand_element = pop_next_action("srand");
		
		std::string ts(srand_element.GetText());
		game_rand.seed(std::stoi(ts));
	} else {
		auto t = time(nullptr);
		if (recording) {
			std::string ts = boost::lexical_cast<std::string>(t);
			record_action("srand", ts);
		}
		game_rand.seed(t);
	}
	std::cout << game_rand() << std::endl;
	init_screen_locs();	
	init_startup();
	flushingInput = true;
	cFramerateLimiter fps_limiter;
	// Hidden preference to hide the startup logo - should be kept hidden
	if(get_bool_pref("ShowStartupLogo", true))
		show_logo(fps_limiter);
	// The preference to hide the startup splash is exposed however.
	if(get_bool_pref("ShowStartupSplash", true))
		plop_fancy_startup(fps_limiter);
	
	cUniverse::print_result = iLiving::print_result = add_string_to_buf;
	cPlayer::give_help = give_help;
	init_fileio();
	init_spell_menus();
	init_mini_map();
	redraw_screen(REFRESH_NONE);
	showMenuBar();
}

void showWelcome() {
	show_dialog_action("welcome");
}

using Key = sf::Keyboard::Key;
std::map<Key,int> delayed_keys;
const int ARROW_SIMUL_FRAMES = 3;

// Terrain map coordinates to simulate a click for 8-directional movement
// ordered to correspond with eDirection
// TODO terrain_click is duplicated (with different ordering) in boe.actions.cpp
location terrain_click[8] = {
	{150,155}, // north
	{180,135}, // northeast
	{180,185}, // east
	{180,215}, // southeast
	{150,215}, // south
	{120,215}, // southwest
	{120,185}, // west
	{120,155}, // northwest
};

static void fire_delayed_key(Key code) {
	bool isUpPressed = delayed_keys[Key::Up] > 0;
	bool isDownPressed = delayed_keys[Key::Down] > 0;
	bool isLeftPressed = delayed_keys[Key::Left] > 0;
	bool isRightPressed = delayed_keys[Key::Right] > 0;
	delayed_keys[Key::Up] = 0;
	delayed_keys[Key::Down] = 0;
	delayed_keys[Key::Left] = 0;
	delayed_keys[Key::Right] = 0;

	bool diagonal = false;
	int dir = -1;

	if(code == Key::Up && !isDownPressed) {
		if(isLeftPressed){ dir = DIR_NW; diagonal = true; }
		else if(isRightPressed){ dir = DIR_NE; diagonal = true; }
		else dir = DIR_N;
	} else if(code == Key::Down && !isUpPressed) {
		if(isLeftPressed){ dir = DIR_SW; diagonal = true; }
		else if(isRightPressed){ dir = DIR_SE; diagonal = true; }
		else dir = DIR_S;
	} else if(code == Key::Left && !isRightPressed) {
		if(isUpPressed){ dir = DIR_NW; diagonal = true; }
		else if(isDownPressed){ dir = DIR_SW; diagonal = true; }
		else dir = DIR_W;
	} else if(code == Key::Right && !isLeftPressed) {
		if(isUpPressed){ dir = DIR_NE; diagonal = true; }
		else if(isDownPressed){ dir = DIR_SE; diagonal = true; }
		else dir = DIR_E;
	} else {
		return;
	}
	if(diagonal){
		mainPtr.setKeyRepeatEnabled(false);
	}else{
		mainPtr.setKeyRepeatEnabled(true);
	}

	if(dir != -1){
		sf::Event pass_event = {sf::Event::MouseButtonPressed};
		location pass_point = mainPtr.mapCoordsToPixel(terrain_click[dir], mainView);
		pass_event.mouseButton.x = pass_point.x;
		pass_event.mouseButton.y = pass_point.y;
		queue_fake_event(pass_event);
	}
}

static void handle_delayed_key(Key code) {
	// a keypress of this code is already delayed, so push it through:
	if(delayed_keys[code] > 0)
		fire_delayed_key(code);

	delayed_keys[code] = ARROW_SIMUL_FRAMES;
}

static void update_delayed_keys() {
	for(auto elem : delayed_keys){
		Key code = elem.first;
		int countdown = elem.second;

		if(countdown > 0){
			--countdown;
			delayed_keys[code] = countdown;
			if(countdown == 0){
				fire_delayed_key(code);
			}
		}
	}
}

void handle_events() {
	sf::Event currentEvent;
	cFramerateLimiter fps_limiter;

	delayed_keys[Key::Left] = 0;
	delayed_keys[Key::Right] = 0;
	delayed_keys[Key::Up] = 0;
	delayed_keys[Key::Down] = 0;

	while(!All_Done) {
		if(replaying && has_next_action()){
			replay_next_action();
		}else{
#ifdef __APPLE__
			if (menuChoiceId>=0) {
				eMenuChoice aMenuChoice=menuChoice;
				menuChoice=eMenuChoice::MENU_CHOICE_NONE;
				switch(aMenuChoice) {
					case eMenuChoice::MENU_CHOICE_GENERIC:
						handle_menu_choice(eMenu(menuChoiceId));
						break;
					case eMenuChoice::MENU_CHOICE_SPELL:
						handle_menu_spell(eSpell(menuChoiceId));
						break;
					case eMenuChoice::MENU_CHOICE_MONSTER_INFO:
						handle_monster_info_menu(menuChoiceId);
						break;
					case eMenuChoice::MENU_CHOICE_NONE:
						break;
				}
				menuChoiceId=-1;
			}
#endif
			update_delayed_keys();
			while(!fake_event_queue.empty()){
				const sf::Event& next_event = fake_event_queue.front();
				fake_event_queue.pop_front();
				handle_one_event(next_event, fps_limiter);
			}
			while(pollEvent(mainPtr, currentEvent)) handle_one_event(currentEvent, fps_limiter);

			// It would be nice to have minimap inside the main game window (we have lots of screen space in fullscreen mode).
			// Alternatively, minimap could live on its own thread.
			// But for now we just handle events from both windows on this thread.
			while(map_visible && pollEvent(mini_map, currentEvent)) handle_one_minimap_event(currentEvent);
		}

		if(changed_display_mode) {
			changed_display_mode = false;
			adjust_window_mode();
			init_mini_map();
		}

		// Still no idea what this does. It's possible that this does not work at all.
		flushingInput = false;

		// Ideally this call should update all of the things that are happening in the world current tick.
		// NOTE that update does not mean draw.
		update_everything();

		// Ideally, this should be the only draw call that is done in a cycle.
		redraw_everything();

		// Prevent the loop from executing too fast.
		fps_limiter.frame_finished();
	}
}

void handle_quit_event() {
	if(recording){
		record_action("close_window", "");
	}
	if(overall_mode == MODE_STARTUP) {
		if(party_in_memory) {
			std::string choice = cChoiceDlog("quit-confirm-save", {"save","quit","cancel"}).show();
			if(choice == "cancel") return;
			if(choice == "save") {
				fs::path file = nav_put_or_temp_party();
				if(file.empty()) return;
				save_party(file, univ);
			}
		}
		All_Done = true;
	}else if(overall_mode == MODE_TOWN || overall_mode == MODE_OUTDOORS){
		std::string choice = cChoiceDlog("quit-confirm-save", {"save", "quit", "cancel"}).show();
		if(choice == "cancel")
			return;
		if(choice == "save")
			save_party(univ.file, univ);
	} else {
		std::string choice = cChoiceDlog("quit-confirm-nosave", {"quit", "cancel"}).show();
		if(choice == "cancel")
			return;
	}
	All_Done = true;
}

void handle_one_event(const sf::Event& event, cFramerateLimiter& fps_limiter) {

	// What does this do and should it be here?
	through_sending();

	// What does this do and should it be here?
	clear_sound_memory();
	
	// Check if any of the event listeners want this event.
	for(auto & listener : event_listeners) {
		if(listener.second->handle_event(event)) return;
	}

	switch(event.type) {
		case sf::Event::KeyPressed:
			if(flushingInput) return;
			if (delayed_keys.find(event.key.code) != delayed_keys.end()){
				handle_delayed_key(event.key.code);
			} else if(!(event.key.*systemKey)) {
				mainPtr.setKeyRepeatEnabled(true);
				handle_keystroke(event, fps_limiter);
			}
			break;
			
		case sf::Event::MouseButtonPressed:
			if(flushingInput) return;
			Mouse_Pressed(event, fps_limiter);
			break;
			
		case sf::Event::MouseLeft:
			// Make sure we don't have an arrow cursor when it's outside the window
			set_cursor(sword_curs);
			break;
			
		case sf::Event::GainedFocus:
			makeFrontWindow(mainPtr);
			change_cursor({event.mouseMove.x, event.mouseMove.y});
			return;

		case sf::Event::MouseMoved:
			change_cursor({event.mouseMove.x, event.mouseMove.y});
			return;

		// TODO: EVENT TYPE DEPRECATED IN SFML 2.5.1
		case sf::Event::MouseWheelMoved:
			if(flushingInput) return;
			handle_scroll(event);
			break;
			
		case sf::Event::Closed:
			handle_quit_event();
			break;
		default:
			break; // There's several events we don't need to handle at all
	}
}

void queue_fake_event(const sf::Event& event) {
	fake_event_queue.push_back(event);
}

void handle_one_minimap_event(const sf::Event& event) {
	if(event.type == sf::Event::Closed) {
		close_map(true);
	} else if(event.type == sf::Event::GainedFocus) {
		makeFrontWindow(mainPtr);
	} else if(event.type == sf::Event::KeyPressed) {
		switch(event.key.code){
			case sf::Keyboard::Escape:
				close_map(true);
				return;
				break;
			default: break;
		}
		char chr = keyToChar(event.key.code, event.key.shift);
		switch(chr){
			case 'a':
				close_map(true);
				return;
				break;
			default:
				break;
		}
	}
}

void update_terrain_animation() {
	static const long fortyTicks = time_in_ticks(40).asMilliseconds();

	if(overall_mode == MODE_STARTUP) return;
	if(!get_bool_pref("DrawTerrainAnimation", true)) return;
	if(animTimer.getElapsedTime().asMilliseconds() < fortyTicks) return;

	anim_ticks++;
	animTimer.restart();
}

void update_startup_animation() {
	static const long interval = time_in_ticks(10).asMilliseconds();

	if(overall_mode != MODE_STARTUP) return;
	if(animTimer.getElapsedTime().asMilliseconds() < interval) return;

	draw_startup_anim(true);
	animTimer.restart();
}

void update_everything() {
	update_terrain_animation();
	update_startup_animation();
}

void redraw_everything() {
	redraw_screen(REFRESH_ALL);
	if(map_visible) draw_map(false);
}

void Mouse_Pressed(const sf::Event& event, cFramerateLimiter& fps_limiter) {

	// What is this stuff? Why is it here?
	if(had_text_freeze > 0) {
		had_text_freeze--;
		return;
	}
	
	if(overall_mode == MODE_STARTUP) {
		All_Done = handle_startup_press({event.mouseButton.x, event.mouseButton.y});
	} else {
		All_Done = handle_action(event, fps_limiter);
	}
	
	// Why does every mouse click activate a menu?
	menu_activate();
}

void close_program() {
	// TODO: Ultimately we would like to have cleanup happen automatically, negating the need for this function
	// On the Mac, prefs are synced automatically. However, doing it manually won't hurt.
	// On other platforms, we need to do it manually.
	sync_prefs();
}

extern fs::path progDir;

void handle_help_toc() {
	if(recording){
		record_action("handle_help_toc", "");
	}
	if(fs::is_directory(progDir/"doc"))
		launchURL("file://" + (progDir/"doc/game/Contents.html").string());
	else launchURL("http://openboe.com/docs/game/Contents.html");
}

void menu_give_help(short help1){
	if(recording){
		record_action("menu_give_help", boost::lexical_cast<std::string>(help1));
	}
	give_help(help1, 0);
}

void handle_menu_choice(eMenu item_hit) {
	std::string dialogToShow;
	sf::Event dummyEvent = {sf::Event::KeyPressed};
	
	switch(item_hit) {
		case eMenu::NONE: break;
		case eMenu::FILE_OPEN:
			do_load();
			break;
		case eMenu::FILE_SAVE:
			do_save(0);
			break;
		case eMenu::FILE_SAVE_AS:
			do_save(1);
			break;
		case eMenu::FILE_NEW:
			new_party();
			break;
		case eMenu::FILE_ABORT:
			do_abort();
			break;
		case eMenu::PREFS:
			pick_preferences();
			break;
		case eMenu::QUIT:
			handle_quit_event();
			break;
		case eMenu::OPTIONS_PC_GRAPHIC:
			handle_new_pc_graphic();
			break;
			
		case eMenu::OPTIONS_DELETE_PC:
			handle_drop_pc();
			break;
			
			
		case eMenu::OPTIONS_RENAME_PC:
			handle_rename_pc();
			break;
			
			
		case eMenu::OPTIONS_NEW_PC:
			handle_new_pc();
			break;
			
		case eMenu::OPTIONS_JOURNAL:
			journal();
			break;
		case eMenu::OPTIONS_TALK_NOTES:
			talk_notes();
			break;
		case eMenu::OPTIONS_ENCOUNTER_NOTES:
			adventure_notes();
			break;
		case eMenu::OPTIONS_STATS:
			print_party_stats();
			break;
		case eMenu::HELP_OUT:
			dialogToShow = "help-outdoor";
			break;
		case eMenu::HELP_TOWN:
			dialogToShow = "help-town";
			break;
		case eMenu::HELP_COMBAT:
			dialogToShow = "help-combat";
			break;
		case eMenu::HELP_BARRIER:
			dialogToShow = "help-fields";
			break;
		case eMenu::HELP_HINTS:
			dialogToShow = "help-hints";
			break;
		case eMenu::HELP_SPELLS:
			dialogToShow = "help-magic";
			break;
		case eMenu::ABOUT:
			dialogToShow = "about-boe";
			break;
		case eMenu::LIBRARY_MAGE:
			display_spells(eSkill::MAGE_SPELLS,100,nullptr);
			break;
		case eMenu::LIBRARY_PRIEST:
			display_spells(eSkill::PRIEST_SPELLS,100,nullptr);
			break;
		case eMenu::LIBRARY_SKILLS:
			display_skills(eSkill::INVALID,nullptr);
			break;
		case eMenu::LIBRARY_ALCHEMY:
			// TODO: Create a dedicated dialog for alchemy info
			display_alchemy();
			break;
		case eMenu::LIBRARY_TIPS:
			tip_of_day();
			break;
		case eMenu::LIBRARY_INTRO:
			showWelcome();
			break;
		case eMenu::ACTIONS_ALCHEMY:
			// This will be recorded when the fake event is processed
			dummyEvent.key.code = sf::Keyboard::A;
			dummyEvent.key.shift = true;
			queue_fake_event(dummyEvent);
			break;
		case eMenu::ACTIONS_WAIT:
			// This will be recorded when the fake event is processed
			dummyEvent.key.code = sf::Keyboard::W;
			queue_fake_event(dummyEvent);
			break;
		case eMenu::ACTIONS_AUTOMAP:
			display_map();
			break;
		case eMenu::HELP_TOC:
			handle_help_toc();
			break;
		case eMenu::ABOUT_MAGE:
		case eMenu::ABOUT_PRIEST:
			menu_give_help(209);
			break;
		case eMenu::ABOUT_MONSTERS:
			menu_give_help(212);
			break;
	}
	if(!dialogToShow.empty()) {
		show_dialog_action(dialogToShow);
	}
}

//  TODO: Let this function take a cMonster* instead of the item_hit
void handle_monster_info_menu(int item_hit) {
	if(recording){
		record_action("handle_monster_info_menu", boost::lexical_cast<std::string>(item_hit));
	}
	display_monst(item_hit - 1, nullptr,1);
}

static cursor_type get_mode_cursor(){
	switch(overall_mode){
		case MODE_TOWN_TARGET:
		case MODE_SPELL_TARGET:
		case MODE_FIRING:
		case MODE_THROWING:
		case MODE_FANCY_TARGET:
			return target_curs;
		case MODE_TALK_TOWN:
			return talk_curs;
		case MODE_USE_TOWN:
			return key_curs;
		case MODE_BASH_TOWN:
			return boot_curs;
		case MODE_DROP_TOWN:
		case MODE_DROP_COMBAT:
			return drop_curs;
		case MODE_LOOK_OUTDOORS:
		case MODE_LOOK_TOWN:
		case MODE_LOOK_COMBAT:
			return look_curs;
		case MODE_RESTING:
			return wait_curs;
		default:
			return sword_curs;
	}
	return sword_curs; // this should never be reached, though
}

void change_cursor(location where_curs) {
	cursor_type cursor_needed;
	location cursor_direction;
	extern enum_map(eGuiArea, rectangle) win_to_rects;
	rectangle world_screen = win_to_rects[WINRECT_TERVIEW];
	world_screen.inset(13, 13);
	
	where_curs = mainPtr.mapPixelToCoords(where_curs, mainView);
	
	if(!world_screen.contains(where_curs))
		cursor_needed = sword_curs;
	else cursor_needed = get_mode_cursor();
	
	if((overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) && world_screen.contains(where_curs)){
		cursor_direction = get_cur_direction(where_curs);
		cursor_needed = arrow_curs[cursor_direction.y + 1][cursor_direction.x + 1];
	}
	
	if(cursor_needed != Cursor::current)
		set_cursor(cursor_needed);
}

void move_sound(ter_num_t ter,short step){
	static bool on_swamp = false;
	eTerSpec spec = univ.scenario.ter_types[ter].special;
	eStepSnd snd = univ.scenario.ter_types[ter].step_sound;
	
	// if on swamp don't play squish sound : BoE legacy behavior, can be removed safely
	if(snd == eStepSnd::SPLASH && !flying() && univ.party.in_boat < 0){
		if(on_swamp && get_ran(1,1,100) >= 10) return;
		on_swamp = true;
	} else on_swamp = false;
	
	if(!monsters_going && !is_combat() && (univ.party.in_boat >= 0)) {
		if(spec == eTerSpec::TOWN_ENTRANCE)
			return;
		play_sound(48); //play boat sound
	} else if(!monsters_going && !is_combat() && (univ.party.in_horse >= 0)) {
		play_sound(85); //so play horse sound
	} else switch(univ.scenario.ter_types[ter].step_sound){
		case eStepSnd::SQUISH:
			play_sound(55);
			break;
		case eStepSnd::CRUNCH:
			play_sound(47);
			break;
		case eStepSnd::NONE:
			break;
		case eStepSnd::SPLASH:
			play_sound(17);
			break;
		case eStepSnd::STEP:
			if(step % 2 == 0) //footsteps alternate sound
				play_sound(49);
			else play_sound(50);
	}
}

void incidental_noises(bool on_surface){
	short sound_to_play;
	if(on_surface){
		if(get_ran(1,1,100) < 40){
			sound_to_play = get_ran(1,1,3);
			switch(sound_to_play){
				case 1:
					play_sound(76);
					break;
				case 2:
					play_sound(77);
					break;
				case 3:
					play_sound(91);
					break;
			}
		}
	}else{
		if(get_ran(1,1,100) < 40){
			sound_to_play = get_ran(1,1,2);
			switch(sound_to_play){
				case 1:
					play_sound(78);
					break;
				case 2:
					play_sound(79);
					break;
			}
		}
	}
	// Dog: 80
	// Cat: 81
	// Sheep: 82
	// Cow: 83
	// Chicken: 92
}

void pause(short length) {
	long len;
	
	len = (long) length;
	
	// Before pausing, make sure the screen is updated.
	redraw_screen(REFRESH_NONE);
	
	if(get_bool_pref("DrawTerrainFrills", true))
		sf::sleep(time_in_ticks(len));
}
