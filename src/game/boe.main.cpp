
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
#include <sstream>
#include <deque>
#include "boe.graphics.hpp"
#include "boe.newgraph.hpp"
#include "boe.fileio.hpp"
#include "boe.actions.hpp"
#include "boe.text.hpp"
#include "boe.party.hpp"
#include "boe.items.hpp"
#include "boe.locutils.hpp"
#include "boe.actions.hpp"
#include "boe.town.hpp"
#include "boe.dlgutil.hpp"
#include "boe.infodlg.hpp"
#include "boe.main.hpp"
#include "boe.consts.hpp"
#include "boe.ui.hpp"
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

static void process_args(int argc, char* argv[]) {
	preprocess_args(argc, argv);
	clara::Args args(argc, argv);
	clara::Parser cli;
	bool record_unique = false;
	boost::optional<std::string> record_to, replay, saved_game;
	boost::optional<double> replay_speed;
	cli |= clara::Opt(record_to, "record")["--record"]("Records a replay of your session to the specified XML file.");
	cli |= clara::Opt(record_unique)["--unique"]("When recording, automatically insert a timestamp into the filename to guarantee uniqueness.");
	cli |= clara::Opt(replay, "replay-file")["--replay"]("Replays a previously-recorded session from the specified XML file.");
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
	if(record_to && init_action_log(record_unique || record_to->empty() ? "record-unique" : "record", *record_to)) return;
	if(replay && init_action_log("replay", *replay)) {
		if(replay_speed) {
			extern boost::optional<cFramerateLimiter> replay_fps_limit;
			replay_fps_limit.emplace(*replay_speed);
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

	Element& next_action = pop_next_action();
	std::string t = next_action.Value();
	
	if(overall_mode == MODE_STARTUP && t == "startup_button_click"){
		auto info = info_from_action(next_action);
		eStartButton btn = static_cast<eStartButton>(std::stoi(info["btn"]));
		eKeyMod mods = static_cast<eKeyMod>(std::stoi(info["mods"]));
		handle_startup_button_click(btn, mods);
	}else if(t == "load_party"){
		decode_file(next_action.GetText(), tempDir / "temp.exg");
		load_party(tempDir / "temp.exg", univ);
		finish_load_party();
	}else if(t == "move"){
		location l = location_from_action(next_action);
		handle_move(l, did_something, need_redraw, need_reprint);
	}else if(t == "handle_switch_pc"){
		short which_pc = short_from_action(next_action);
		handle_switch_pc(which_pc, need_redraw, need_reprint);
	}else if(t == "handle_switch_pc_items"){
		short which_pc = short_from_action(next_action);
		handle_switch_pc_items(which_pc, need_redraw);
	}else if(t == "handle_equip_item"){
		short item_hit = short_from_action(next_action);
		handle_equip_item(item_hit, need_redraw);
	}else if(t == "handle_use_item"){
		short item_hit = short_from_action(next_action);
		handle_use_item(item_hit, did_something, need_redraw);
	}else if(t == "handle_item_shop_action"){
		short item_hit = short_from_action(next_action);
		handle_item_shop_action(item_hit);
	}else if(t == "handle_alchemy"){
		handle_alchemy(need_redraw, need_reprint);
	}else if(t == "handle_town_wait"){
		handle_town_wait(need_redraw, need_reprint);
	}else if(t == "handle_combat_switch"){
		handle_combat_switch(did_something, need_redraw, need_reprint);
	}else if(t == "handle_missile"){
		handle_missile(need_redraw, need_reprint);
	}else if(t == "handle_get_items"){
		handle_get_items(did_something, need_redraw, need_reprint);
	}else if(t == "handle_drop_item_id"){
		short item_hit = short_from_action(next_action);
		handle_drop_item(item_hit, need_redraw);
	}else if(t == "handle_drop_item_location"){
		location destination = location_from_action(next_action);
		handle_drop_item(destination, need_redraw);
	}else if(t == "handle_give_item"){
		short item_hit = short_from_action(next_action);
		handle_give_item(item_hit, did_something, need_redraw);
	}else if(t == "close_window"){
		handle_quit_event();
	}else if(t == "arrow_button_click"){
		rectangle button_rect;
		std::istringstream sstr(next_action.GetText());
		sstr >> button_rect;
		arrow_button_click(button_rect);
	}else if(t == "show_dialog_action"){
		show_dialog_action(next_action.GetText());
	}else if(t == "drop_pc"){
		drop_pc();
	}else if(t == "new_party"){
		new_party();
	}else if(t == "pick_preferences"){
		pick_preferences();
	}

	advance_time(did_something, need_redraw, need_reprint);
}

void init_boe(int argc, char* argv[]) {
	set_up_apple_events();
	init_directories(argv[0]);
	check_for_intel();
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
	cChoiceDlog welcome("welcome");
	welcome->attachClickHandlers([](cDialog& self, std::string clicked, eKeyMod) {
		launchURL(self[clicked].getText());
		return false;
	}, {"spidweb", "scen", "forum", "home", "src"});
	welcome.show();
}

void handle_events() {
	sf::Event currentEvent;
	cFramerateLimiter fps_limiter;

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
			while(!fake_event_queue.empty()){
				const sf::Event& next_event = fake_event_queue.front();
				fake_event_queue.pop_front();
				handle_one_event(next_event, fps_limiter);
			}
			while(mainPtr.pollEvent(currentEvent)) handle_one_event(currentEvent, fps_limiter);

			// It would be nice to have minimap inside the main game window (we have lots of screen space in fullscreen mode).
			// Alternatively, minimap could live on its own thread.
			// But for now we just handle events from both windows on this thread.
			while(map_visible && mini_map.pollEvent(currentEvent)) handle_one_minimap_event(currentEvent);
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
	
	// If it's just a modifier key, update the state
	if(kb.handleModifier(event)) return;

	// Check if any of the event listeners want this event.
	for(auto & listener : event_listeners) {
		if(listener.second->handle_event(event)) return;
	}

	switch(event.type) {
		case sf::Event::KeyPressed:
			if(flushingInput) return;
			if(!(event.key.*systemKey)) handle_keystroke(event, fps_limiter);
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
		mini_map.setVisible(false);
		map_visible = false;
	} else if(event.type == sf::Event::GainedFocus) {
		makeFrontWindow(mainPtr);
	} else if(event.type == sf::Event::KeyPressed) {
		switch(event.key.code) {
			case sf::Keyboard::Escape:
				mini_map.setVisible(false);
				map_visible = false;
				break;
			default: break;
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
	static const long twentyTicks = time_in_ticks(20).asMilliseconds();

	if(overall_mode != MODE_STARTUP) return;
	if(animTimer.getElapsedTime().asMilliseconds() < twentyTicks) return;

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
void handle_menu_choice(eMenu item_hit) {
	std::string dialogToShow;
	sf::Event dummyEvent = {sf::Event::KeyPressed};
	short choice;
	
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
			// TODO record and replay
			if(overall_mode != MODE_STARTUP) {
				std::string choice = cChoiceDlog("abort-game",{"okay","cancel"}).show();
				if (choice=="cancel") return;
				reload_startup();
				overall_mode = MODE_STARTUP;
			}
			party_in_memory = false;
			draw_startup(0);
			menu_activate();
			break;
		case eMenu::PREFS:
			pick_preferences();
			break;
		case eMenu::QUIT:
			handle_quit_event();
			break;
		case eMenu::OPTIONS_PC_GRAPHIC:
			// TODO record and replay
			choice = char_select_pc(1,"New graphic for who?");
			if(choice < 6)
				pick_pc_graphic(choice,1,nullptr);
			draw_terrain();
			break;
			
		case eMenu::OPTIONS_DELETE_PC:
			drop_pc();
			break;
			
			
		case eMenu::OPTIONS_RENAME_PC:
			// TODO record and replay
			choice = char_select_pc(1,"Rename who?");
			if(choice < 6)
				pick_pc_name(choice,nullptr);
			put_pc_screen();
			put_item_screen(stat_window);
			break;
			
			
		case eMenu::OPTIONS_NEW_PC:
			// TODO record and replay
			if(!(is_town())) {
				add_string_to_buf("Add PC: Town mode only.");
				print_buf();
				break;
			}
			if(univ.party.free_space() == 6) {
				ASB("Add PC: You already have 6 PCs.");
				print_buf();
				break;
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
			break;
			
		case eMenu::OPTIONS_JOURNAL:
			// TODO record and replay
			journal();
			break;
		case eMenu::OPTIONS_TALK_NOTES:
			// TODO record and replay
			if(overall_mode == MODE_TALKING) {
				ASB("Talking notes: Can't read while talking.");
				print_buf();
				return;
			}
			talk_notes();
			break;
		case eMenu::OPTIONS_ENCOUNTER_NOTES:
			// TODO record and replay
			adventure_notes();
			break;
		case eMenu::OPTIONS_STATS:
			// TODO record and replay
			if(overall_mode != MODE_STARTUP)
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
			// TODO record and replay
			display_spells(eSkill::MAGE_SPELLS,100,0);
			break;
		case eMenu::LIBRARY_PRIEST:
			// TODO record and replay
			display_spells(eSkill::PRIEST_SPELLS,100,0);
			break;
		case eMenu::LIBRARY_SKILLS:
			// TODO record and replay
			display_skills(eSkill::INVALID,0);
			break;
		case eMenu::LIBRARY_ALCHEMY:
			// TODO record and replay
			// TODO: Create a dedicated dialog for alchemy info
			display_alchemy();
			break;
		case eMenu::LIBRARY_TIPS:
			// TODO record and replay
			tip_of_day();
			break;
		case eMenu::LIBRARY_INTRO:
			// TODO record and replay
			showWelcome();
			break;
		case eMenu::ACTIONS_ALCHEMY:
			// TODO record and replay
			dummyEvent.key.code = sf::Keyboard::A;
			dummyEvent.key.shift = true;
			queue_fake_event(dummyEvent);
			break;
		case eMenu::ACTIONS_WAIT:
			// TODO record and replay
			dummyEvent.key.code = sf::Keyboard::W;
			queue_fake_event(dummyEvent);
			break;
		case eMenu::ACTIONS_AUTOMAP:
			// TODO record and replay
			if(!prime_time()) {
				ASB("Finish what you're doing first.");
				print_buf();
			} else display_map();
			set_cursor(sword_curs);
			break;
		case eMenu::HELP_TOC:
			// TODO record and replay
			if(fs::is_directory(progDir/"doc"))
				launchURL("file://" + (progDir/"doc/game/Contents.html").string());
			else launchURL("http://openboe.com/docs/game/Contents.html");
			break;
		// TODO record and replay
		case eMenu::ABOUT_MAGE:
		case eMenu::ABOUT_PRIEST:
			give_help(209,0);
			break;
		case eMenu::ABOUT_MONSTERS:
			// TODO record and replay
			give_help(212,0);
			break;
	}
	if(!dialogToShow.empty()) {
		show_dialog_action(dialogToShow);
	}
}

//  TODO: Let this function take a cMonster* instead of the item_hit
void handle_monster_info_menu(int item_hit) {
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
