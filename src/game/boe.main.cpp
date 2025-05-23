
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
#include "fileio/resmgr/res_font.hpp"
#include "fileio/resmgr/res_dialog.hpp"
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
#include "gfx/gfxsheets.hpp"

using clara::ParserResult;
using clara::ParseResultType;

bool All_Done = false;
short num_fonts;
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
extern cCustomGraphics spec_scen_g;

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
std::vector<fs::path> extra_scen_dirs;
boost::optional<std::string> scen_arg_path;
bool scen_arg_start = false;
boost::optional<short> scen_arg_town, scen_arg_town_entrance;
boost::optional<location> scen_arg_out_sec, scen_arg_loc;
extern std::string last_load_file;
std::string help_text_rsrc = "help";

/*
// Example feature flags:
{
	// A build which supports both V2 and V3 of the updated graphics sheet:
	{"graphics-sheet", {"V2", "V3"}}
}
*/
std::map<std::string,std::vector<std::string>> feature_flags = {
	// Legacy scenario flags
	{"resurrection-balm", {"required"}}, // This means it CAN be supported, if the scenario has the flag.
	// Legacy behavior of the T debug action (used by some replays)
	// does not change the party's outdoors location
	{"debug-enter-town", {"move-outdoors"}},
	// Legacy behavior of the X debug action (used by the OneOfEverything replay)
	// kills the whole party with 'Absent' status
	{"debug-kill-party", {"V2"}},
	// Legacy behavior of pacifist spellcasting (used by some replays)
	// lets the player select combat spells and click 'Cast' which will fail.
	{"pacifist-spellcast-check", {"V2"}},
	// Target lock
	// V1: Shift screen to show the maximum number of enemies in range
	// V2: Like V1, but don't shift if it hides any enemies that are already visible
	{"target-lock", {"V1", "V2"}},
	// New in-game save file picker
	{"file-picker-dialog", {"V1"}},
	{"scenario-meta-format", {"V2"}},
	// Talk mode
	{"talk-go-back", {"StackV1"}},
	// Bugs required for several VoDT test replays to run faithfully
	{"empty-wandering-monster-bug", {"fixed"}},
	{"too-many-extra-wandering-monsters-bug", {"fixed"}},
	{"store-spell-target", {"fixed"}},
	{"store-spell-caster", {"fixed"}},
	// Game balance
	{"magic-resistance", {"fixed"}} // Resist Magic used to not help with magic damage!
};

struct cParseEntrance {
	boost::optional<short>& opt;
	cParseEntrance(boost::optional<short>& opt) : opt(opt) {}
	ParserResult operator()(std::string v) const {
		ParserResult error = ParserResult::logicError( "Invalid entrance: '" + v + "'. Try N, S, E, or W.");
		if(v.size() == 1){
			switch(v.at(0)){
				case 'N': case 'n': case '0':
					opt = 0;
					break;
				case 'E': case 'e': case '1':
					opt = 1;
					break;
				case 'S': case 's': case '2':
					opt = 2;
					break;
				case 'W': case 'w': case '3':
					opt = 3;
					break;
				default:
					return error;
			}
			return ParserResult::ok( ParseResultType::Matched );
		}
		return error;
	}
};

struct cParseLocation {
	boost::optional<location>& opt;
	cParseLocation(boost::optional<location>& opt) : opt(opt) {}
	ParserResult operator()(std::string v) const {
		try{
			opt = boost::lexical_cast<location>(v);
			return ParserResult::ok( ParseResultType::Matched );
		}catch(boost::bad_lexical_cast){
			return ParserResult::logicError( "Invalid location: '" + v + "'. Try 'x,y' format.");
		}
	}
};

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

short which_item_page[6] = {0,0,0,0,0,0}; // Remembers which of the 2 item pages pc looked at
short current_ground = 0;
eStatMode stat_screen_mode;
short anim_step = -1;

// Spell casting globals
eSpell store_mage = eSpell::NONE, store_priest = eSpell::NONE;
short store_mage_lev = 0, store_priest_lev = 0;
short store_mage_target = 6, store_priest_target = 6;
short store_mage_caster = 6, store_priest_caster = 6;
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
extern long ter_anim_ticks;

static void init_boe(int, char*[]);
static void handle_scenario_args();
static void showWelcome();

static void replay_next_action();

void handle_quit_event();
void handle_help_toc();
void menu_give_help(short help1);

extern void edit_stuff_done();

#ifdef __APPLE__
eMenuChoice menuChoice=eMenuChoice::MENU_CHOICE_NONE;
short menuChoiceId=-1;
#endif

static void handleFatalError(std::string what) {
	bool was_recording = recording;
	if(recording){
		record_action("error", what);
		recording = false; // Don't record click_control on the error message
	}
	replaying = false; // Don't try to run replay actions in the fatal error dialog
	showFatalError(what);
	if(was_recording){
		extern fs::path log_file;
		if(log_file.empty() && cChoiceDlog("ask-save-replay", {"yes", "no"}).show() == "yes") {
			save_replay_log();
		}
	}
}

void dialog_lost_focus(sf::RenderWindow& win) {
	setWindowFloating(mini_map(), false);
}

void dialog_gained_focus(sf::RenderWindow& win) {
	setWindowFloating(mini_map(), true);
	if(map_visible){
		makeFrontWindow(mini_map());
	}
	makeFrontWindow(mainPtr(), mini_map());
}

// Comment this line out for exact exception callstacks:
#define CATCH_ERRORS

int main(int argc, char* argv[]) {
#if 0
	void debug_oldstructs();
	debug_oldstructs();
#endif
#ifdef CATCH_ERRORS
	try{
#endif
		cDialog::redraw_everything = &redraw_everything;
		cDialog::onLostFocus = &dialog_lost_focus;
		cDialog::onGainedFocus = &dialog_gained_focus;

		init_boe(argc, argv);
		
		if(!get_bool_pref("GameRunBefore"))
			showWelcome();
		else if(get_bool_pref("GiveIntroHint", true))
			tip_of_day();
		set_pref("GameRunBefore", true);
		finished_init = true;
		
		if(ae_loading) {
			finish_load_party();
			if(overall_mode != MODE_STARTUP)
				post_load();
		}
		
		menu_activate();
		restore_cursor();

		handle_scenario_args();
		handle_events();

		close_program();
		return 0;
#ifdef CATCH_ERRORS
	} catch(std::exception& x) {
		handleFatalError(x.what());
		throw;
	} catch(std::string& x) {
		handleFatalError(x);
		throw;
	} catch(...) {
		handleFatalError("An unknown error occurred!");
		throw;
	}
#endif
}

static void init_sbar(std::shared_ptr<cScrollbar>& sbar, const std::string& name, rectangle rect, rectangle events_rect, int max, int pgSz, int start = 0) {
	static cParentless mainWin(mainPtr());
	sbar.reset(new cScrollbar(mainWin));
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
	static cParentless mainWin(mainPtr());
	btn.reset(new cButton(mainWin));

	btn->setBtnType(type);
	btn->relocate(loc);
	btn->hide();
}

static void init_buttons() {
	
	// MAGIC NUMBERS: move to boe.ui.cpp ?
	
	init_btn(done_btn, BTN_DONE, {231,395});
	init_btn(help_btn, BTN_HELP, {273,12});
}

// Spell dialog is slow to open on Windows, so keep it prepared and reuse it.
std::unique_ptr<cDialog> storeCastSpell;

// NOTE: this should possibly be moved to boe.ui.cpp at some point
static void init_ui() {
	cDialog::init();
	init_scrollbars();
	init_buttons();

	storeCastSpell.reset(new cDialog(*ResMgr::dialogs.get("cast-spell")));
}

extern bool record_verbose;
extern bool replay_verbose;
extern bool replay_strict;

bool record_in_memory = true;

static void process_args(int argc, char* argv[]) {
	preprocess_args(argc, argv);
	clara::Args args(argc, argv);
	clara::Parser cli;
	bool record_unique = false;
	boost::optional<std::string> record_to, replay, scen_file, saved_game;
	boost::optional<double> replay_speed;

	// Replay system options:
	cli |= clara::Opt(record_to, "record")["--record"]("Records a replay of your session to the specified XML file.");
	cli |= clara::Opt(record_unique)["--unique"]("When recording, automatically insert a timestamp into the filename to guarantee uniqueness.");
	cli |= clara::Opt(record_verbose)["--verbose"]("Record extra information for internal testing of the replay system.");
	cli |= clara::Opt(replay, "replay-file")["--replay"]("Replays a previously-recorded session from the specified XML file.");
	cli |= clara::Opt(replay_strict)["--strict"]("Enforces strictly identical replay behavior, even where this is only cosmetic");
	cli |= clara::Opt(replay_speed, "fps")["--replay-speed"]("Specifies how quickly actions are processed while replaying");

	// Scenario options:
	// Loading and entering a scenario has to happen after other initialization steps,
	// so we just save these options for later.
	cli |= clara::Opt(scen_arg_path, "scen-path")["--scenario"]("Launch a scenario, with the default party if no party is loaded.");
	cli |= clara::Opt(scen_arg_start)["--start"]("Put the party at the starting location of the scenario.");
	cli |= clara::Opt(scen_arg_town, "town")["--town"]("Put the party in a town.");
	cli |= clara::Opt(cParseEntrance(scen_arg_town_entrance), "entrance")["--entrance"]("Put the party at a town entrance point.");
	cli |= clara::Opt(cParseLocation(scen_arg_out_sec), "x,y")["--out-sec"]("Put the party in an outdoor section.");
	cli |= clara::Opt(cParseLocation(scen_arg_loc), "x,y")["--loc"]("Put the party at a location.");

	// Party option:
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
	// This obsolete preference should always be true unless running an old replay
	// (which will set it false after this line if it needs to)
	set_pref("DrawTerrainFrills", true);
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
		// Handle replays that loaded a savegame from the command line
		if(has_next_action("load_party")){
			replay_next_action();
		}
		return;
	}
	else if(record_to){
		if(!init_action_log(record_unique || record_to->empty() ? "record-unique" : "record", *record_to)){
			std::cerr << "Failed to start recording: " << *record_to << std::endl;
			exit(1);
		}
		// Don't return, because we want to support recording a run that starts with a party from the CLI.
	}else if(record_in_memory){
		if(!init_action_log("record", "")){
			std::cerr << "Failed to start recording in memory." << std::endl;
			exit(1);
		}
	}

	if(saved_game){
		if(!load_party(*saved_game, univ, spec_scen_g)) {
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

	if(scen_arg_town && scen_arg_out_sec){
		std::cout << "Warning: --out-sec conflicts with --town and will be ignored." << std::endl;
	}
	if(scen_arg_town_entrance && scen_arg_loc){
		std::cout << "Warning: --loc conflicts with --entrance and will be ignored." << std::endl;
	}
}

static void handle_scenario_args() {
	bool resetting = false;
	if(scen_arg_path){
		// When changing the command-line party's scenario status, clear last_load_file
		// so the player will be prompted to Save As.
		last_load_file = "Blades of Exile Save";

		fs::path path = *scen_arg_path;

		if(!path.parent_path().empty()){
			// Add the scenario's path to the search paths put_party_in_scen() uses
			extra_scen_dirs.push_back(path.parent_path());
		}else{
			path = locate_scenario(*scen_arg_path, true);
		}

		cScenario scenario;
		if(load_scenario(path, scenario)){
			if(!party_in_memory){
				start_new_game(true);
			}
			if(univ.party.is_in_scenario()){
				// This check is correct, because the scen_name field of cScenario is the actual title,
				// not header.exs or *.boes as univ.party.scen_name would be
				if(univ.scenario.scen_name == scenario.scen_name){
					// The party is already in the correct scenario.
					// Ask whether to clear SDFs or not
					if(cChoiceDlog("reset-story",{"reset","keep"}).show() == "reset"){
						resetting = true;
						handle_victory(true);
					}
				}else{
					// The party is in a different scenario.
					// TODO maybe the player should be warned before they're removed from it?
					handle_victory(true);
				}
			}else{
				resetting = true;
			}
			if(!univ.party.is_in_scenario()){
				put_party_in_scen(path.string(), scen_arg_town || scen_arg_out_sec, true);
			}
		}else{
			std::cerr << "Failed to load scenario: " << *scen_arg_path << std::endl;
			exit(1);
		}
	}

	if(scen_arg_town){
		if(!party_in_memory || !univ.party.is_in_scenario() || univ.scenario.towns.size() < (*scen_arg_town + 1)){
			std::cerr << "Expected a scenario with at least " << (*scen_arg_town + 1) << " towns" << std::endl;
			exit(1);
		}

		outd_move_to_first_town_entrance(*scen_arg_town);

		short town_entrance = 0;
		location town_location;
		if(scen_arg_town_entrance){
			town_entrance = *scen_arg_town_entrance;
		}else if(scen_arg_loc){
			town_entrance = 9;
			town_location = *scen_arg_loc;
		}
		force_town_enter(*scen_arg_town, town_location);
		start_town_mode(*scen_arg_town, town_entrance, true);
	}else if(scen_arg_out_sec){
		if(!party_in_memory || !univ.party.is_in_scenario() ||
				univ.scenario.outdoors.width() < (scen_arg_out_sec->x + 1) ||
				univ.scenario.outdoors.height() < (scen_arg_out_sec->y + 1)){
			std::cerr << "Expected a scenario with at least " << (scen_arg_out_sec->x + 1) << "x" << (scen_arg_out_sec->y + 1) << " outdoor sections" << std::endl;
			exit(1);
		}

		if(scen_arg_loc){
			overall_mode = MODE_OUTDOORS;
			position_party(scen_arg_out_sec->x,scen_arg_out_sec->y,scen_arg_loc->x,scen_arg_loc->y);
			clear_map();
			update_explored(univ.party.out_loc);
			redraw_screen(REFRESH_ALL);
		}else{
			std::cerr << "--x and --y are required when loading a scenario outdoors" << std::endl;
			exit(1);
		}
	}else if(scen_arg_start){
		// If restarting the scenario completely, this isn't needed:
		if(!resetting){
			debug_return_to_start();
		}
	}
}

std::map<std::string, int> startup_button_indices = {
	// Button layout since 11/30/24
	{"Tutorial", 0}, {"Make New Party", 3},
	{"Load Game", 1}, {"Start Scenario", 4},
	{"Preferences", 2},

	// Buttons that don't exist anymore
	{"Custom Scenario", -1},
};

std::map<int, std::string> startup_button_names = {
	{0, "Tutorial"}, {3, "Make New Party"},
	{1, "Load Game"}, {4, "Start Scenario"},
	{2, "Preferences"}, {5, ""},
};

// Map legacy int indices onto new string-mapped layout
std::map<int, std::string> startup_button_names_v1 = {
	{0, "Load Game"}, {3, "Start Scenario"},
	{1, "Make New Party"}, {4, "Custom Scenario"},
	{2, "Preferences"},
};

static void replay_action(Element& action) {
	bool did_something = false, need_redraw = false, need_reprint = false;

	std::string _last_action_type = last_action_type;
	std::string t = action.Value();
	
	// NOTE: Action replay blocks need to return early unless the action advances time
	if(overall_mode == MODE_STARTUP && t == "startup_button_click"){
		auto info = info_from_action(action);
		int btn_idx = -1;
		try{
			// Legacy replays use ints to encode startup buttons
			btn_idx = std::stoi(info["btn"]);
			btn_idx = startup_button_indices[startup_button_names_v1[btn_idx]];
		}catch(std::invalid_argument& err){
			// Newer replays use strings to encode startup buttons
			btn_idx = startup_button_indices[info["btn"]];
        }
		// No-op button
		if(btn_idx == -1){
			return;
		}
		eStartButton btn = static_cast<eStartButton>(btn_idx);
		eKeyMod mods = static_cast<eKeyMod>(std::stoi(info["mods"]));
		handle_startup_button_click(btn, mods);
		return;
	}else if(t == "change_fps"){
		extern boost::optional<cFramerateLimiter> replay_fps_limit;
		// default new fps: slow the replay down substantially
		int new_fps = 2;
		if(!action.GetText().empty()){
			new_fps = boost::lexical_cast<int>(action.GetText());
		}
		replay_fps_limit.emplace(new_fps);
	}else if(t == "fancy_file_picker"){
		bool saving = str_to_bool(action.GetText());
		fancy_file_picker(saving);
	}else if(t == "load_party"){
		decode_file(action.GetText(), tempDir / "temp.exg");
		load_party(tempDir / "temp.exg", univ, spec_scen_g);

		finish_load_party();

		if(overall_mode != MODE_STARTUP)
			post_load();

		menu_activate();
		return;
	}else if(t == "move"){
		location l = location_from_action(action);
		handle_move(l, did_something, need_redraw, need_reprint);
	}else if(t == "handle_switch_pc"){
		short which_pc = short_from_action(action);
		handle_switch_pc(which_pc, need_redraw, need_reprint);
	}else if(t == "handle_switch_pc_items"){
		short which_pc = short_from_action(action);
		handle_switch_pc_items(which_pc, need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "handle_equip_item"){
		short item_hit = short_from_action(action);
		handle_equip_item(item_hit, need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "handle_use_item"){
		short item_hit = short_from_action(action);
		handle_use_item(item_hit, did_something, need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "handle_item_shop_action"){
		short item_hit = short_from_action(action);
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
		short item_hit = short_from_action(action);
		handle_drop_item(item_hit, need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "handle_drop_item_location"){
		location destination = location_from_action(action);
		handle_drop_item(destination, need_redraw);
	}else if(t == "handle_give_item"){
		short item_hit = short_from_action(action);
		handle_give_item(item_hit, did_something, need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "close_window"){
		handle_quit_event();
		return;
	}else if(t == "arrow_button_click"){
		rectangle button_rect = boost::lexical_cast<rectangle>(action.GetText());
		arrow_button_click(button_rect);
		return;
	}else if(t == "show_dialog_action"){
		show_dialog_action(action.GetText());
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
		auto info = info_from_action(action);
		eSkill mode = boost::lexical_cast<eSkill>(info["mode"]);
		short force_spell = boost::lexical_cast<short>(info["force_spell"]);

		display_spells(mode, force_spell, nullptr);
		return;
	}else if(t == "display_skills"){
		eSkill force_skill = boost::lexical_cast<eSkill>(action.GetText());

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
		short help1 = short_from_action(action);
		menu_give_help(help1);
		return;
	}else if(t == "handle_begin_look"){
		bool right_button = str_to_bool(action.GetText());
		handle_begin_look(right_button, need_redraw, need_reprint);
		if (right_button){
			return;
		}
	}else if(t == "handle_look"){
		auto info = info_from_action(action);
		location destination = boost::lexical_cast<location>(info["destination"]);
		bool right_button = str_to_bool(info["right_button"]);
		eKeyMod mods = static_cast<eKeyMod>(std::stoi(info["mods"]));

		handle_look(destination, right_button, mods, need_redraw, need_reprint);
	}else if(t == "screen_shift"){
		auto info = info_from_action(action);
		int dx = std::stoi(info["dx"]);
		int dy = std::stoi(info["dy"]);

		screen_shift(dx, dy, need_redraw);
	}else if(t == "handle_rest"){
		handle_rest(need_redraw, need_reprint);
	}else if(t == "handle_menu_spell"){
		eSpell spell_picked = static_cast<eSpell>(std::stoi(action.GetText()));
		handle_menu_spell(spell_picked);
		return;
	}else if(t == "handle_spellcast"){
		auto info = info_from_action(action);
		eSkill which_type = boost::lexical_cast<eSkill>(info["which_type"]);
		spell_forced = str_to_bool(info["spell_forced"]);

		handle_spellcast(which_type, did_something, need_redraw, need_reprint);
	}else if(t == "handle_target_space"){
		auto info = info_from_action(action);
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
		location destination = location_from_action(action);
		handle_bash_pick(destination, did_something, need_redraw, true);
	}else if(t == "handle_pick"){
		location destination = location_from_action(action);
		handle_bash_pick(destination, did_something, need_redraw, false);
	}else if(t == "handle_use_space_select"){
		handle_use_space_select(need_reprint);
	}else if(t == "handle_use_space"){
		location destination = location_from_action(action);
		handle_use_space(destination, did_something, need_redraw);
	}else if(t == "show_inventory"){
		show_inventory();
		return;
	}else if(t == "give_help"){
		auto info = info_from_action(action);
		short help1 = boost::lexical_cast<short>(info["help1"]);
		short help2 = boost::lexical_cast<short>(info["help2"]);
		bool help_forced;
		// Legacy replays may have the text indices offset by 200 to indicate
		// help_forced true.
		if(info.find("help_forced") == info.end()){
			help_forced = help1 >= 200;
			help1 -= 200;
		}else{
			help_forced = str_to_bool(info["help_forced"]);
		}
		give_help(help1, help2, help_forced);
	}else if(t == "toggle_debug_mode"){
		toggle_debug_mode();
		return;
	}else if(t == "debug_launch_scen"){
		debug_launch_scen(action.GetText());
		return;
	}else if(t == "debug_give_item"){
		debug_give_item();
		return;
	}else if(t == "debug_overburden"){
		debug_overburden();
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
	}else if(t == "debug_kill_party"){
		debug_kill_party();
		return;
	}else if(t == "debug_hurt_party"){
		debug_hurt_party();
		return;
	}else if(t == "debug_give_status"){
		debug_give_status();
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
		handle_victory(true); // This is for the debug action which forces it.
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
		handle_print_pc_hp(boost::lexical_cast<int>(action.GetText()), need_reprint);
	}else if(t == "handle_print_pc_sp"){
		handle_print_pc_sp(boost::lexical_cast<int>(action.GetText()), need_reprint);
	}else if(t == "give_pc_info"){
		give_pc_info(boost::lexical_cast<short>(action.GetText()));
		return;
	}else if(t == "handle_trade_places"){
		handle_trade_places(boost::lexical_cast<short>(action.GetText()), need_reprint);
	}else if(t == "handle_begin_talk"){
		handle_begin_talk(need_reprint);
	}else if(t == "handle_talk"){
		handle_talk(location_from_action(action), did_something, need_redraw, need_reprint);
	}else if(t == "click_talk_rect"){
		word_rect_t word_rect = word_rect_from_action(action);
		click_talk_rect(word_rect);
		handle_talk_node(word_rect.node);
		return;
	}

	// Legacy action: clicking any shop button used to highlight both the info
	// button and the item text (which was a bug)
	else if(t == "click_shop_rect"){
		rectangle rect = boost::lexical_cast<rectangle>(action.GetText());
		click_shop_rect(rect);
		return;
	}
	// Fixed versions:
	else if(t == "click_shop_item"){
		rectangle rect = boost::lexical_cast<rectangle>(action.GetText());
		click_shop_rect(rect, false);
	}else if(t == "click_shop_item_help"){
		rectangle rect = boost::lexical_cast<rectangle>(action.GetText());
		click_shop_rect(rect, true);
	}

	else if(t == "end_shop_mode"){
		end_shop_mode();
		return;
	}else if(t == "scrollbar_setPosition"){
		auto info = info_from_action(action);
		std::string name = info["name"];
		long newPos = boost::lexical_cast<long>(info["newPos"]);

		std::shared_ptr<cScrollbar> sbar = std::dynamic_pointer_cast<cScrollbar>(event_listeners[name]);
		sbar->setPosition(newPos);
		return;
	}else if(t == "use_spec_item"){
		use_spec_item(boost::lexical_cast<short>(action.GetText()), need_redraw);
		update_item_stats_area(need_reprint);
	}else if(t == "show_item_info"){
		show_item_info(boost::lexical_cast<short>(action.GetText()));
		update_item_stats_area(need_reprint);
		return;
	}else if(t == "set_stat_window"){
		set_stat_window(static_cast<eItemWinMode>(boost::lexical_cast<int>(action.GetText())));
		update_item_stats_area(need_reprint);
	}else if(t == "handle_sale"){
		handle_sale(boost::lexical_cast<int>(action.GetText()));
	}else if(t == "handle_info_request"){
		handle_info_request(boost::lexical_cast<int>(action.GetText()));
	}else if(t == "close_map"){
		close_map(true);
		return;
	}else if(t == "handle_toggle_active"){
		handle_toggle_active(need_reprint);
	}else if(t == "handle_parry"){
		handle_parry(did_something, need_redraw, need_reprint);
	}else if(t == "handle_monster_info_menu"){
		handle_monster_info_menu(boost::lexical_cast<int>(action.GetText()));
		return;
	}else if(t == "cancel_item_target"){
		cancel_item_target(did_something, need_redraw, need_reprint);
	}else if(t == "easter_egg"){
		easter_egg(boost::lexical_cast<int>(action.GetText()));
	}else if(t == "show_debug_help"){
		show_debug_help();
		return;
	}else if(t == "debug_fight_encounter"){
		debug_fight_encounter(str_to_bool(action.GetText()));
	}else if(t == "preview_every_dialog_xml"){
		preview_every_dialog_xml();
	}else if(t == "clear_trapped_monst"){
		clear_trapped_monst();
	}else if(t == "error"){
		// The error is recorded for debugging only. It should be triggered by replaying the actions.
	}else if(t == "debug_crash"){
		debug_crash();
	}else if(t == "advance_time"){
		// This is bad regardless of strictness, because visual changes may have occurred which won't get redrawn/reprinted
		throw std::string { "Replay system internal error! advance_time() was supposed to be called by the last action, but wasn't: " } + _last_action_type;
	}else{
		std::ostringstream sstr;
		sstr << "Couldn't replay action: " << action << " on line " << action.Row();
		replaying = false;
		throw sstr.str();
	}

	// NOTE: commands that do not advance time must return early
	advance_time(did_something, need_redraw, need_reprint);
}

static void replay_next_action() {
	replay_action(pop_next_action());
}

static void record_feature_flags() {
	Element next_action("feature_flags");
	for(auto& p : feature_flags){
		Element next_flag(p.first);
		std::vector<std::string> supported_versions = p.second;
		for(std::string version : supported_versions){
			Element next_version("version");
			Text version_text(version);
			next_version.InsertEndChild(version_text);
			next_flag.InsertEndChild(next_version);
		}
		next_action.InsertEndChild(next_flag);
	}
	record_action(next_action);
}

static void replay_feature_flags() {
	std::map<std::string,std::vector<std::string>> recorded_flags = {};
	if(has_next_action("feature_flags")){
		Element action = pop_next_action();
		Element* next_flag = action.FirstChildElement(false);
		while(next_flag){
			std::string flag = next_flag->Value();
			std::vector<std::string> supported_versions;
			Element* next_version = next_flag->FirstChildElement(false);
			while(next_version){
				std::string version = next_version->GetText();
				// The game build needs to support the feature version that the replay had
				if(!has_feature_flag(flag, version)){
					std::string error = "This replay requires a feature that is not supported in your version of Blades of Exile: " + flag + " should support '" + version + "'";
					throw error;
				}
				supported_versions.push_back(version);
				next_version = next_version->NextSiblingElement(false);
			}
			recorded_flags[flag] = supported_versions;
			next_flag = next_flag->NextSiblingElement(false);
		}
	}

	feature_flags = recorded_flags;
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
	
	// see fallback_scale() in winutil.cpp for where the default UI scale is calculated based on screen size
	adjust_window_mode();
	init_ui();
	// If we don't do this now it'll flash white to start with
	mainPtr().clear(sf::Color::Black);
	mainPtr().display();
	
	set_cursor(watch_curs);
	init_buf();

	if(recording){
		record_feature_flags();
	}else if(replaying){
		replay_feature_flags();
	}

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
	cPlayer::give_help_enabled = true;
	init_fileio();
	init_debug_actions();
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

// Directional deltas ordered to correspond with eDirection
// TODO directional_delta is duplicated (with different ordering) in boe.actions.cpp
location directional_delta[8] = {
	{0,-1}, // north
	{1,-1}, // northeast
	{1,0}, // east
	{1,1}, // southeast
	{0,1}, // south
	{-1,1}, // southwest
	{-1,0}, // west
	{-1,-1}, // northwest
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
		mainPtr().setKeyRepeatEnabled(false);
	}else{
		mainPtr().setKeyRepeatEnabled(true);
	}

	if(dir != -1){
		bool did_something = false;
		bool need_redraw = false;
		bool need_reprint = false;
		location delta = directional_delta[dir];
		if(!handle_screen_shift(delta, need_redraw)){
			// TODO this was deferred to the next frame before. Is that still necessary when not spoofing an event?
			handle_terrain_screen_actions(delta, false, false, did_something, need_redraw, need_reprint);
		}
		advance_time(did_something, need_redraw, need_reprint);
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

bool game_has_focus = true;
bool game_had_focus = true;
bool main_window_lost_focus = false;
bool main_window_gained_focus = false;
bool map_window_lost_focus = false;
bool map_window_gained_focus = false;

void handle_events() {
	sf::Event currentEvent;
	cFramerateLimiter fps_limiter;

	delayed_keys[Key::Left] = 0;
	delayed_keys[Key::Right] = 0;
	delayed_keys[Key::Up] = 0;
	delayed_keys[Key::Down] = 0;

	while(!All_Done) {
		if(replaying && has_next_action()){
			if(check_for_interrupt("confirm-interrupt-replay")){
				replaying = false;
				continue;
			}
			replay_next_action();
		}else{
			replaying = false;
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

			while(pollEvent(mainPtr(), currentEvent)) handle_one_event(currentEvent, fps_limiter);

			// It would be nice to have minimap inside the main game window (we have lots of screen space in fullscreen mode).
			// Alternatively, minimap could live on its own thread.
			// But for now we just handle events from both windows on this thread.
			while(map_visible && pollEvent(mini_map(), currentEvent)) handle_one_minimap_event(currentEvent);
		}

		if(changed_display_mode) {
			changed_display_mode = false;
			// Force reload fonts for possible new UI scale
			ResMgr::fonts.drain();

			adjust_window_mode();
			storeCastSpell.reset(new cDialog(*ResMgr::dialogs.get("cast-spell")));
			init_mini_map();
		}

		// Still no idea what this does. It's possible that this does not work at all.
		flushingInput = false;

		// Ideally this call should update all of the things that are happening in the world current tick.
		// NOTE that update does not mean draw.
		update_everything();

		// Ideally, this should be the only draw call that is done in a cycle.
		redraw_everything();

		bool any_lost_focus = main_window_lost_focus || map_window_lost_focus;
		bool any_gained_focus = main_window_gained_focus || map_window_gained_focus;

		if(game_had_focus && any_lost_focus && !any_gained_focus){
			game_has_focus = false;
			// The game completely lost focus. The map floating is going to block
			// other applications and be annoying.
			setWindowFloating(mini_map(), false);
		}else if(!game_had_focus && any_gained_focus){
			game_has_focus = true;

			// The game regained focus
			setWindowFloating(mini_map(), true);
			if(map_visible){
				makeFrontWindow(mini_map());
			}
			makeFrontWindow(mainPtr(), mini_map());
		}

		main_window_lost_focus = main_window_gained_focus =
		map_window_lost_focus = map_window_gained_focus = false;

		game_had_focus = game_has_focus;

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
				do_save();
			}
		}
		All_Done = true;
	}else if(overall_mode == MODE_TOWN || overall_mode == MODE_OUTDOORS){
		std::string choice = cChoiceDlog("quit-confirm-save", {"save", "quit", "cancel"}).show();
		if(choice == "cancel")
			return;
		if(choice == "save")
			do_save();
	} else {
		std::string choice = cChoiceDlog("quit-confirm-nosave", {"quit", "cancel"}).show();
		if(choice == "cancel")
			return;
	}
	All_Done = true;
}

int last_window_x = 0;
int last_window_y = 0;

void handle_one_event(const sf::Event& event, cFramerateLimiter& fps_limiter) {
	// What does this do and should it be here?
	clear_sound_memory();
	
	// Check if any of the event listeners want this event.
	for(auto & listener : event_listeners) {
		if(listener.second->handle_event(event)) return;
	}

	switch(event.type) {
		case sf::Event::KeyPressed:
			if(flushingInput) return;
			if (!noDelayKeyModes.count(overall_mode) && delayed_keys.find(event.key.code) != delayed_keys.end()){
				handle_delayed_key(event.key.code);
			} else if(!(event.key.*systemKey)) {
				mainPtr().setKeyRepeatEnabled(true);
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
			check_window_moved(mainPtr(), last_window_x, last_window_y, "MainWindow");
			main_window_gained_focus = true;
			makeFrontWindow(mainPtr(), mini_map());
			change_cursor();
			return;

		case sf::Event::LostFocus:
			main_window_lost_focus = true;
			return;

		case sf::Event::MouseMoved:
			check_window_moved(mainPtr(), last_window_x, last_window_y, "MainWindow");
			change_cursor();
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

int last_map_x = 0;
int last_map_y = 0;

void handle_one_minimap_event(const sf::Event& event) {
	if(event.type == sf::Event::Closed) {
		close_map(true);
	}else if(event.type == sf::Event::GainedFocus){
		map_window_gained_focus = true;
		check_window_moved(mini_map(), last_map_x, last_map_y, "MapWindow");
		makeFrontWindow(mainPtr(), mini_map());
	}else if(event.type == sf::Event::LostFocus){
		map_window_lost_focus = true;
	}else if(event.type == sf::Event::MouseMoved){
		check_window_moved(mini_map(), last_map_x, last_map_y, "MapWindow");
	}else if(event.type == sf::Event::KeyPressed){
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
	if(animTimer.getElapsedTime().asMilliseconds() < fortyTicks) return;

	ter_anim_ticks++;
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

void handle_help_toc() {
	if(recording){
		record_action("handle_help_toc", "");
	}
	launchDocs("game/Contents.html");
}

void menu_give_help(short help1){
	if(recording){
		record_action("menu_give_help", boost::lexical_cast<std::string>(help1));
	}
	give_help(help1, 0, true);
}

void handle_menu_choice(eMenu item_hit) {
	std::string dialogToShow;
	bool did_something = false, need_redraw = false, need_reprint = false;
	
	switch(item_hit) {
		case eMenu::NONE: break;
		case eMenu::FILE_OPEN:
			do_load();
			break;
		case eMenu::FILE_SAVE:
			do_save();
			break;
		case eMenu::FILE_SAVE_AS:
			do_save(true);
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
			display_spells(eSkill::MAGE_SPELLS,100,nullptr,true);
			break;
		case eMenu::LIBRARY_PRIEST:
			display_spells(eSkill::PRIEST_SPELLS,100,nullptr,true);
			break;
		case eMenu::LIBRARY_SKILLS:
			display_skills(eSkill::INVALID,nullptr,true);
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
			handle_alchemy(need_redraw, need_reprint);
			break;
		case eMenu::ACTIONS_WAIT:
			handle_wait(did_something, need_redraw, need_reprint);
			break;
		case eMenu::ACTIONS_AUTOMAP:
			display_map();
			break;
		case eMenu::HELP_TOC:
			handle_help_toc();
			break;
		case eMenu::ABOUT_MAGE:
		case eMenu::ABOUT_PRIEST:
			menu_give_help(9);
			break;
		case eMenu::ABOUT_MONSTERS:
			menu_give_help(12);
			break;
	}
	if(!dialogToShow.empty()) {
		show_dialog_action(dialogToShow);
	}
	if(did_something || need_redraw || need_reprint){
		advance_time(did_something, need_redraw, need_reprint);
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

void change_cursor() {
	cursor_type cursor_needed;
	location where_curs = mouse_window_coords();
	location cursor_direction;
	extern enum_map(eGuiArea, rectangle) win_to_rects;
	rectangle world_screen = win_to_rects[WINRECT_TERVIEW];
	world_screen.inset(13, 13);
	
	if(!world_screen.contains(where_curs))
		cursor_needed = sword_curs;
	else cursor_needed = get_mode_cursor();
	
	if((overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) && world_screen.contains(where_curs)){
		location tile;
		mouse_to_terrain_coords(tile, true);
		if(tile.x != 4 || tile.y != 4){
			cursor_direction = get_cur_direction();
			cursor_needed = arrow_curs[cursor_direction.y + 1][cursor_direction.x + 1];
		}else{
			cursor_needed = wait_curs;
		}
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
	
	sf::sleep(time_in_ticks(len));
}
