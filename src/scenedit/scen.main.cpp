
#include <cstdio>
#include <string>
#include <memory>
#include <boost/filesystem/operations.hpp>

#include "scen.global.hpp"
#include "scenario/scenario.hpp"
#include "gfx/render_image.hpp"
#include "gfx/tiling.hpp"
#include "scen.graphics.hpp"
#include "scen.actions.hpp"
#include "scen.fileio.hpp"
#include "scen.btnmg.hpp"
#include "sounds.hpp"
#include "scen.townout.hpp"
#include "scen.core.hpp"
#include "scen.keydlgs.hpp"
#include "mathutil.hpp"
#include "fileio/fileio.hpp"
#include "dialogxml/widgets/button.hpp"
#include "dialogxml/widgets/led.hpp"
#include "dialogxml/widgets/ledgroup.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include "tools/winutil.hpp"
#include "tools/cursors.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "scen.menus.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "tools/prefs.hpp"
#include "tools/framerate_limiter.hpp"
#include "tools/event_listener.hpp"
#include "tools/drawable_manager.hpp"

#ifdef __APPLE__
short menuChoiceId=-1;
bool pending_quit=false;
#endif

fs::path pending_file_to_load=fs::path();

/* Globals */
bool  All_Done = false;
bool changed_display_mode = false;
sf::RenderWindow mainPtr;
sf::View mainView;
cTown* town = nullptr;
bool mouse_button_held = false,editing_town = false;
short cur_viewing_mode = 0;
short cen_x, cen_y;
eScenMode overall_mode = MODE_INTRO_SCREEN;
short mode_count = 0;
cOutdoors* current_terrain;

std::string scenario_temp_dir_name = "ed_scenario";
bool change_made = false, ae_loading = false;

// Numbers of current areas being edited
short cur_town;
location cur_out;

/* Prototypes */
static void init_scened(int, char*[]);
void handle_events();
void handle_one_event(const sf::Event&);
void redraw_everything();
void Mouse_Pressed(const sf::Event&);
void close_program();
void ding();
void adjust_windows(sf::RenderWindow&, sf::View&);
sf::FloatRect compute_viewport(const sf::RenderWindow&, float ui_scale);

void pick_preferences();
void save_prefs();

cScenario scenario;

extern void set_up_apple_events(int argc, char* argv[]);

// TODO: these should be members of some global entity instead of being here
cDrawableManager drawable_mgr;

//Changed to ISO C specified argument and return type.
int main(int argc, char* argv[]) {
	try {
		
		init_scened(argc, argv);
		
		if(ae_loading)
			set_up_main_screen();
		else {
			shut_down_menus(0);
			set_up_start_screen();
		}
		
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

sf::FloatRect compute_viewport(const sf::RenderWindow & mainPtr, float ui_scale) {

	// See compute_viewport() in boe.graphics.cpp
	const int os_specific_y_offset =
#if defined(SFML_SYSTEM_WINDOWS) || defined(SFML_SYSTEM_MAC)
		0;
#else
		getMenubarHeight();
#endif

	sf::FloatRect viewport;
	
	viewport.top    = float(os_specific_y_offset) / mainPtr.getSize().y;
	viewport.left   = 0;
	viewport.width  = ui_scale;
	viewport.height = ui_scale;
	
	return viewport;
}

void adjust_windows (sf::RenderWindow & mainPtr, sf::View & mainView) {

	// TODO: things might still be broken when upscaled.
	//   translate_mouse_coordinates has been applied in some places but more work might be needed.
	//   In particular, the white area on the right side of the main menu needs fixing.
	float ui_scale = get_float_pref("UIScale", 1.0);
	
	const int width  = ui_scale * 584;
	const int height = ui_scale * 420
#ifndef SFML_SYSTEM_WINDOWS
		+ getMenubarHeight()
#endif
	;
	
	mainPtr.create(sf::VideoMode(width, height), "Blades of Exile Scenario Editor", sf::Style::Titlebar | sf::Style::Close);
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	mainPtr.setPosition({static_cast<int>((desktop.width - width) / 2), static_cast<int>((desktop.height - height) / 2)});

	// Initialize the view
	mainView.setSize(width, height);
	mainView.setCenter(width / 2, height / 2);

	// Apply the viewport to the view
	sf::FloatRect mainPort = compute_viewport(mainPtr, ui_scale);
	mainView.setViewport(mainPort);

	// Apply view to the main window
	mainPtr.setView(mainView);

#ifndef __APPLE__ // This overrides Dock icon on OSX, which isn't what we want at all
	auto const & icon = *ResMgr::textures.get("icon", true);
	mainPtr.setIcon(icon->getSize().x, icon->getSize().y, icon->copyToImage().getPixelsPtr());
#endif
	init_menubar();
}

void init_scened(int argc, char* argv[]) {
	init_directories(argv[0]);
	sync_prefs();
	adjust_windows(mainPtr, mainView);
	//init_menubar();
	init_shaders();
	init_tiling();
	init_snd_tool();
#ifdef SFML_SYSTEM_MAC
	init_menubar(); // This is called twice because Windows and Mac have different ordering requirements
#endif
	mainPtr.clear(sf::Color::Black);
	mainPtr.display();
	
	set_cursor(watch_curs);
	check_for_intel();
	srand(time(nullptr));
		
	cen_x = 18;
	cen_y = 18;
	
	scen_controls.init();
	
	load_graphics();
	cDialog::init();
	if(get_bool_pref("ShowStartupLogo", true))
		run_startup_g();
	set_cursor(sword_curs);
	
	cDialog::defaultBackground = cDialog::BG_LIGHT;
	cDialog::doAnimations = true;
	set_up_apple_events(argc, argv);
	init_fileio();
}

void handle_events() {
	sf::Event currentEvent;
	cFramerateLimiter fps_limiter;

	while(!All_Done) {
		bool need_redraw=false;
		if(changed_display_mode) {
			need_redraw = true;
			changed_display_mode = false;
			adjust_windows(mainPtr, mainView);
		}

#ifdef __APPLE__
		if (pending_quit) {
			pending_quit=false;
			handle_menu_choice(eMenu::QUIT);
			if (All_Done)
				break;
		}
		if (!pending_file_to_load.empty() && save_check("save-before-load")) {
			// FIXME: do not dupplicate this code another time
			if(load_scenario(pending_file_to_load, scenario)) {
				cur_town = scenario.last_town_edited;
				town = scenario.towns[cur_town];
				cur_out = scenario.last_out_edited;
				current_terrain = scenario.outdoors[cur_out.x][cur_out.y];
				overall_mode = MODE_MAIN_SCREEN;
				change_made = false;
				set_up_main_screen();
			} else
				set_up_start_screen(); // Failed to load file, dump to start
			extern cUndoList undo_list;
			undo_list.clear();
			update_edit_menu();
			pending_file_to_load.clear();
		}
		if (menuChoiceId>=0) {
			need_redraw = true;
			handle_menu_choice(eMenu(menuChoiceId));
			menuChoiceId=-1;
		}
#endif
		while(mainPtr.pollEvent(currentEvent)) {
			need_redraw = true;
			handle_one_event(currentEvent);
		}

		// Why do we have to set this to false after handling every event?
		ae_loading = false;

		if (need_redraw)
			redraw_everything();

		// Prevent the loop from executing too fast.
		fps_limiter.frame_finished();
	}
}

void handle_one_event(const sf::Event& event) {
	if (scen_controls.handle_one_event(event))
		return;
	
	switch(event.type) {
		case sf::Event::KeyPressed:
			if(!(event.key.*systemKey))
				handle_keystroke(event);
			break;
			
		case sf::Event::MouseButtonPressed:
			Mouse_Pressed(event);
			break;
			
		case sf::Event::MouseMoved:
			if(mouse_button_held)
				handle_action(location { translate_mouse_coordinates({event.mouseMove.x,event.mouseMove.y})},event);
			update_mouse_spot(location { translate_mouse_coordinates({event.mouseMove.x,event.mouseMove.y})});
			break;
			
		case sf::Event::MouseWheelMoved:
			handle_scroll(event);
			break;
			
		case sf::Event::MouseButtonReleased:
			mouse_button_held = false;
			break;
			
		case sf::Event::Closed:
			if(!save_check("save-before-quit"))
				break;
			All_Done = true;
			break;
			
		default:
			break;
	}
}

void redraw_everything() {
	redraw_screen();
	restore_cursor();
}

extern fs::path progDir;
void handle_menu_choice(eMenu item_hit) {
	extern cUndoList undo_list;
	bool isEdit = false, isHelp = false;
	std::string helpDlog;
	fs::path file_to_load;
	cKey editKey = {true};
	auto &controls=scen_controls;
	switch(item_hit) {
		case eMenu::NONE: return;
		case eMenu::FILE_OPEN:
			if(change_made && !save_check("save-before-load"))
				break;
		if(false)
		case eMenu::FILE_REVERT:
			if(change_made && cChoiceDlog("save-before-revert", {"revert", "cancel"}).show() == "cancel")
				break;
			file_to_load = item_hit == eMenu::FILE_OPEN ? nav_get_scenario() : scenario.scen_file;
			if(!file_to_load.empty() && load_scenario(file_to_load, scenario)) {
				cur_town = scenario.last_town_edited;
				town = scenario.towns[cur_town];
				cur_out = scenario.last_out_edited;
				current_terrain = scenario.outdoors[cur_out.x][cur_out.y];
				overall_mode = MODE_MAIN_SCREEN;
				change_made = false;
				set_up_main_screen();
			} else if(!file_to_load.empty())
				set_up_start_screen(); // Failed to load file, dump to start
			undo_list.clear();
			update_edit_menu();
			break;
		case eMenu::FILE_SAVE:
			save_scenario();
			break;
		case eMenu::FILE_SAVE_AS:
			save_scenario(true);
			break;
		case eMenu::FILE_NEW:
			if(!save_check("save-before-close"))
				break;
			if(build_scenario()) {
				overall_mode = MODE_MAIN_SCREEN;
				set_up_main_screen();
			}
			undo_list.clear();
			update_edit_menu();
			break;
		case eMenu::FILE_CLOSE:
			if(!save_check("save-before-close"))
				break;
			overall_mode = MODE_INTRO_SCREEN;
			controls.show_right_bar(false);
			controls.show_palette_bar(false);
			shut_down_menus(0);
			set_up_start_screen();
			undo_list.clear();
			update_edit_menu();
			break;
		case eMenu::PREFS:
			pick_preferences();
			break;
		case eMenu::QUIT: // quit
			if(!save_check("save-before-quit"))
				break;
			All_Done = true;
			break;
		case eMenu::EDIT_UNDO:
			editKey.k = key_undo;
			isEdit = true;
			break;
		case eMenu::EDIT_REDO:
			editKey.k = key_redo;
			isEdit = true;
			break;
		case eMenu::EDIT_CUT:
			editKey.k = key_cut;
			isEdit = true;
			break;
		case eMenu::EDIT_COPY:
			editKey.k = key_copy;
			isEdit = true;
			break;
		case eMenu::EDIT_PASTE:
			editKey.k = key_paste;
			isEdit = true;
			break;
		case eMenu::EDIT_DELETE:
			editKey.k = key_del;
			isEdit = true;
			break;
		case eMenu::EDIT_SELECT_ALL:
			editKey.k = key_selectall;
			isEdit = true;
			break;
		case eMenu::TOWN_CREATE:
			if(scenario.towns.size() >= 200) {
				showError("You have reached the limit of 200 towns you can have in one scenario.");
				return;
			}
			if(new_town())
				set_up_main_screen();
			break;
		case eMenu::OUT_RESIZE:
			if(resize_outdoors())
				change_made = true;
			if(overall_mode == MODE_MAIN_SCREEN)
				set_up_main_screen();
			break;
		case eMenu::SCEN_DETAILS:
			edit_scen_details();
			change_made = true;
			break;
		case eMenu::SCEN_INTRO:
			edit_scen_intro();
			change_made = true;
			break;
		case eMenu::TOWN_START:
			overall_mode = MODE_SET_TOWN_START;
			set_string("Select party starting location.","");
			break;
		case eMenu::SCEN_SHEETS:
			edit_custom_sheets();
			change_made = true;
			break;
		case eMenu::SCEN_PICS:
			edit_custom_pics_types();
			change_made = true;
			break;
		case eMenu::SCEN_SNDS:
			edit_custom_sounds();
			change_made = true;
			break;
		case eMenu::SCEN_SPECIALS:
			controls.right_bar->setPosition(0);
			start_special_editing(0,0);
			break;
		case eMenu::SCEN_TEXT:
			controls.right_bar->setPosition(0);
			start_string_editing(STRS_SCEN,0);
			break;
		case eMenu::SCEN_JOURNALS:
			controls.right_bar->setPosition(0);
			start_string_editing(STRS_JOURNAL,0);
			break;
		case eMenu::TOWN_IMPORT:
			if(cTown* town = pick_import_town()) {
				town->reattach(scenario);
				delete scenario.towns[cur_town];
				scenario.towns[cur_town] = town;
				::town = town;
				change_made = true;
				redraw_screen();
			}
			break;
		case eMenu::OUT_IMPORT:
			if(cOutdoors* out = pick_import_out()) {
				out->reattach(scenario);
				delete scenario.outdoors[cur_out.x][cur_out.y];
				scenario.outdoors[cur_out.x][cur_out.y] = out;
				current_terrain = out;
				change_made = true;
				redraw_screen();
			}
			break;
		case eMenu::SCEN_SAVE_ITEM_RECTS:
			edit_save_rects();
			change_made = true;
			break;
		case eMenu::TOWN_VARYING:
			edit_add_town();
			change_made = true;
			break;
		case eMenu::SCEN_TIMERS:
			edit_scenario_events();
			change_made = true;
			break;
		case eMenu::SCEN_ITEM_SHORTCUTS:
			edit_item_placement();
			break;
		case eMenu::TOWN_DELETE:
			if(scenario.towns.size() == 1) {
				showError("You can't delete the last town in a scenario. All scenarios must have at least 1 town.");
				return;
			}
			if(scenario.towns.size() - 1 == cur_town) {
				showError("You can't delete the last town in a scenario while you're working on it. Load a different town, and try this again.");
				return;
			}
			if(scenario.towns.size() - 1 == scenario.which_town_start) {
				showError("You can't delete the last town in a scenario while it's the town the party starts the scenario in. Change the parties starting point and try this again.");
				return;
			}
			if(cChoiceDlog("delete-town-confirm", {"okay", "cancel"}).show() == "okay")
				delete_last_town();
			break;
		case eMenu::SCEN_DATA_DUMP:
			if(cChoiceDlog("data-dump-confirm", {"okay", "cancel"}).show() == "okay")
				start_data_dump();
			break;
		case eMenu::SCEN_TEXT_DUMP:
			if(cChoiceDlog("text-dump-confirm", {"okay", "cancel"}).show() == "okay")
				scen_text_dump();
			break;
		case eMenu::TOWN_DETAILS:
			edit_town_details();
			change_made = true;
			break;
		case eMenu::TOWN_WANDERING:
			edit_town_wand();
			change_made = true;
			break;
		case eMenu::TOWN_BOUNDARIES:
			overall_mode = MODE_SET_TOWN_RECT;
			mode_count = 2;
			set_string("Set town boundary","Select upper left corner");
			break;
		case eMenu::FRILL:
			frill_up_terrain();
			change_made = true;
			break;
		case eMenu::UNFRILL:
			unfrill_terrain();
			change_made = true;
			break;
		case eMenu::TOWN_AREAS:
			controls.right_bar->setPosition(0);
			start_string_editing(STRS_TOWN_RECT,0);
			break;
		case eMenu::TOWN_ITEMS_RANDOM:
			if(cChoiceDlog("add-random-items", {"okay", "cancel"}).show() == "cancel")
				break;
			place_items_in_town();
			change_made = true;
			break;
		case eMenu::TOWN_ITEMS_NOT_PROPERTY:
			for(int i = 0; i < town->preset_items.size(); i++)
				town->preset_items[i].property = 0;
			cChoiceDlog("set-not-owned").show();
			draw_terrain();
			change_made = true;
			break;
		case eMenu::TOWN_ITEMS_CLEAR:
			if(cChoiceDlog("clear-items-confirm", {"okay", "cancel"}).show() == "cancel")
				break;
			town->preset_items.clear();
			draw_terrain();
			change_made = true;
			break;
		case eMenu::TOWN_SPECIALS:
			controls.right_bar->setPosition(0);
			start_special_editing(2,0);
			break;
		case eMenu::TOWN_TEXT:
			controls.right_bar->setPosition(0);
			start_string_editing(STRS_TOWN,0);
			break;
		case eMenu::TOWN_SIGNS:
			controls.right_bar->setPosition(0);
			start_string_editing(STRS_TOWN_SIGN,0);
			break;
		case eMenu::TOWN_ADVANCED:
			edit_advanced_town();
			change_made = true;
			break;
		case eMenu::TOWN_TIMERS:
			edit_town_events();
			change_made = true;
			break;
		case eMenu::OUT_DETAILS:
			outdoor_details();
			change_made = true;
			break;
		case eMenu::OUT_WANDERING:
			edit_out_wand(0);
			change_made = true;
			break;
		case eMenu::OUT_ENCOUNTERS:
			edit_out_wand(1);
			change_made = true;
			break;
		case eMenu::OUT_AREAS:
			controls.right_bar->setPosition(0);
			start_string_editing(STRS_OUT_RECT,0);
			break;
		case eMenu::OUT_START:
			overall_mode = MODE_SET_OUT_START;
			set_string("Select party starting location.","");
			break;
		case eMenu::OUT_SPECIALS:
			controls.right_bar->setPosition(0);
			start_special_editing(1,0);
			break;
		case eMenu::OUT_TEXT:
			controls.right_bar->setPosition(0);
			start_string_editing(STRS_OUT,0);
			break;
		case eMenu::OUT_SIGNS:
			controls.right_bar->setPosition(0);
			start_string_editing(STRS_OUT_SIGN,0);
			break;
		case eMenu::ABOUT:
			helpDlog = "about-scened";
			isHelp = true;
			break;
		case eMenu::HELP_TOC:
			if(fs::is_directory(progDir/"doc"))
				launchURL("file://" + (progDir/"doc/editor/Contents.html").string());
			else launchURL("http://openboe.com/docs/editor/Contents.html");
			break;
		case eMenu::HELP_START:
			helpDlog = "help-editing";
			isHelp = true;
			break;
		case eMenu::HELP_TEST:
			helpDlog = "help-testing";
			isHelp = true;
			break;
		case eMenu::HELP_DIST:
			helpDlog = "help-distributing";
			isHelp = true;
			break;
		case eMenu::HELP_CONTEST:
			helpDlog = "help-contest";
			isHelp = true;
			break;
	}
	if(isEdit) {
		if(!cDialog::sendInput(editKey)) {
			// Handle non-dialog edit operations here.
			switch(editKey.k) {
				case key_undo: undo_list.undo(); break;
				case key_redo: undo_list.redo(); break;
				default: break;
			}
			update_edit_menu();
			if(overall_mode == MODE_MAIN_SCREEN)
				set_up_main_screen();
			redraw_screen();
		}
	}
	if(isHelp)
		cChoiceDlog(helpDlog).show();
	redraw_screen();
}

void save_prefs(){
	bool success = sync_prefs();
	if(!success){
		showWarning("There was a problem writing to the preferences file. When the Scenerio Editor is next run the preferences will revert to their previously set values.","Should you manage to resolve the problem without closing the program, simply open the preferences screen and click \"OK\" to try again.");
	}
}

static bool prefs_event_filter (cDialog& me, std::string id, eKeyMod) {
	bool did_cancel = false;
	
	if(id == "okay") {
		me.toast(true);
	} else if(id == "cancel") {
		me.toast(false);
		did_cancel = true;
	}
	
	if(!did_cancel) {
		std::string scale = dynamic_cast<cLedGroup&>(me["scaleui"]).getSelected();
		if(scale == "1")
			set_pref("UIScale", 1.0);
		else if(scale == "2")
			set_pref("UIScale", 2.0);
		else if(scale == "4")
			set_pref("UIScale", 4.0);
		set_pref("PlaySounds", dynamic_cast<cLed&>(me["nosound"]).getState() == led_off);
	}
	save_prefs();
	return true;
}

void pick_preferences() {
	set_cursor(sword_curs);
	
	cDialog prefsDlog("pref-scenario");
	prefsDlog.attachClickHandlers(&prefs_event_filter, {"okay", "cancel"});
	
	float ui_scale = get_float_pref("UIScale", 1.0);
	cLedGroup& uiScale = dynamic_cast<cLedGroup&>(prefsDlog["scaleui"]);
	if (ui_scale>0.95 && ui_scale<1.05) uiScale.setSelected("1");
	else if (ui_scale>1.95 && ui_scale<2.05) uiScale.setSelected("2");
	else if (ui_scale>3.95 && ui_scale<4.05) uiScale.setSelected("4");
	else uiScale.setSelected("other");

	dynamic_cast<cLed&>(prefsDlog["nosound"]).setState(get_bool_pref("PlaySounds", true) ? led_off : led_red);
	
	prefsDlog.run();
	
	if(get_float_pref("UIScale") != ui_scale)
		changed_display_mode = true;
}

void Mouse_Pressed(const sf::Event & event) {
	location mousePos { translate_mouse_coordinates({event.mouseButton.x, event.mouseButton.y}) }; 
	handle_action(mousePos,event);
}

void close_program() {
}

// TODO: Remove this function and replace it with beep() or play_sound() everywhere.
void ding() {
	beep();
}
