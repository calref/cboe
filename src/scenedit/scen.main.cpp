
#include <cstdio>
#include <string>
#include <memory>
#include <boost/filesystem/operations.hpp>

#include "scen.global.hpp"
#include "scenario.hpp"
#include "render_image.hpp"
#include "tiling.hpp"
#include "scen.graphics.hpp"
#include "scen.actions.hpp"
#include "scen.fileio.hpp"
#include "scen.btnmg.hpp"
#include "sounds.hpp"
#include "scen.townout.hpp"
#include "scen.core.hpp"
#include "scen.keydlgs.hpp"
#include "mathutil.hpp"
#include "fileio.hpp"
#include "scrollbar.hpp"
#include "winutil.hpp"
#include "cursors.hpp"
#include "strdlog.hpp"
#include "choicedlog.hpp"
#include "scen.menus.hpp"
#include "res_image.hpp"
#include "prefs.hpp"
#include "framerate_limiter.hpp"
#include "event_listener.hpp"
#include "drawable_manager.hpp"

/* Globals */
bool  All_Done = false;
sf::RenderWindow mainPtr;
sf::View mainView;
cTown* town = nullptr;
bool mouse_button_held = false,editing_town = false;
short cur_viewing_mode = 0;
short cen_x, cen_y;
eScenMode overall_mode = MODE_INTRO_SCREEN;
std::shared_ptr<cScrollbar> right_sbar, pal_sbar;
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
void init_main_window(sf::RenderWindow&, sf::View&);
sf::FloatRect compute_viewport(const sf::RenderWindow&, float ui_scale);

cScenario scenario;
rectangle right_sbar_rect;
extern rectangle terrain_buttons_rect;

extern void set_up_apple_events(int argc, char* argv[]);

// TODO: these should be members of some global entity instead of being here
std::unordered_map <std::string, std::shared_ptr <iEventListener>> event_listeners;
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

static void init_sbar(std::shared_ptr<cScrollbar>& sbar, std::string const name, rectangle rect, rectangle events_rect, int pgSz) {
	sbar.reset(new cScrollbar(mainPtr));
	sbar->setBounds(rect);
	sbar->set_wheel_event_rect(events_rect);
	sbar->setPageSize(pgSz);
	sbar->hide();
	
	drawable_mgr.add_drawable(UI_LAYER_DEFAULT, name, sbar);
	event_listeners[name] = std::dynamic_pointer_cast <iEventListener> (sbar);
}

static void init_scrollbars () {
	right_sbar_rect.top = RIGHT_AREA_UL_Y - 1;
	right_sbar_rect.left = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 1 - 16;
	right_sbar_rect.bottom = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT + 1;
	right_sbar_rect.right = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 1;
	rectangle pal_sbar_rect = terrain_buttons_rect;
	pal_sbar_rect.offset(RIGHT_AREA_UL_X,RIGHT_AREA_UL_Y);
	pal_sbar_rect.left = pal_sbar_rect.right - 16;
	pal_sbar_rect.height() = 17 * 16;
	
	rectangle const right_sbar_event_rect { 5, 287, 405, 577 };
	rectangle const pal_sbar_event_rect   { 5, 287, 279, 581 };
		
	init_sbar(right_sbar, "right_sbar", right_sbar_rect, right_sbar_event_rect, NRSONPAGE - 1);
	init_sbar(pal_sbar, "pal_sbar", pal_sbar_rect, pal_sbar_event_rect, 16);
}

sf::FloatRect compute_viewport(sf::RenderWindow const & mainPtr, float ui_scale) {

	// See compute_viewport() in boe.graphics.cpp
	int const os_specific_y_offset =
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

void init_main_window (sf::RenderWindow & mainPtr, sf::View & mainView) {

	// TODO: things might still be broken when upscaled.
	//   translate_mouse_coordinates has been applied in some places but more work might be needed.
	//   In particular, the white area on the right side of the main menu needs fixing.
	float ui_scale = get_float_pref("UIScale", 1.0);
	
	int const width  = ui_scale * 584;
	int const height = ui_scale * 420
#ifndef SFML_SYSTEM_WINDOWS
		+ getMenubarHeight()
#endif
	;
	
	mainPtr.create(sf::VideoMode(width, height), "Blades of Exile Scenario Editor", sf::Style::Titlebar | sf::Style::Close);
	mainPtr.setPosition({0,0});

	// Initialize the view
	mainView.setSize(width, height);
	mainView.setCenter(width / 2, height / 2);

	// Apply the viewport to the view
	sf::FloatRect mainPort = compute_viewport(mainPtr, ui_scale);
	mainView.setViewport(mainPort);

	// Apply view to the main window
	mainPtr.setView(mainView);

#ifndef SFML_SYSTEM_MAC // This overrides Dock icon on OSX, which isn't what we want at all
	const ImageRsrc& icon = ResMgr::graphics.get("icon", true);
	mainPtr.setIcon(icon->getSize().x, icon->getSize().y, icon->copyToImage().getPixelsPtr());
#endif
}

void init_scened(int argc, char* argv[]) {
	init_directories(argv[0]);
	sync_prefs();
	init_main_window(mainPtr, mainView);
	init_menubar();
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
		
	init_scrollbars();
	init_lb();
	init_rb();
	
	Set_up_win();
	init_screen_locs();
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
		while(mainPtr.pollEvent(currentEvent)) handle_one_event(currentEvent);

		// Why do we have to set this to false after handling every event?
		ae_loading = false;

		redraw_everything();

		// Prevent the loop from executing too fast.
		fps_limiter.frame_finished();
	}
}

void handle_one_event(sf::Event const & event) {
	
	// Check if any of the event listeners want this event.
	for (auto & listener : event_listeners) {
		if(listener.second->handle_event(event)) return;
	}
	
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
			right_sbar->hide();
			pal_sbar->hide();
			shut_down_menus(0);
			set_up_start_screen();
			undo_list.clear();
			update_edit_menu();
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
			right_sbar->setPosition(0);
			start_special_editing(0,0);
			break;
		case eMenu::SCEN_TEXT:
			right_sbar->setPosition(0);
			start_string_editing(STRS_SCEN,0);
			break;
		case eMenu::SCEN_JOURNALS:
			right_sbar->setPosition(0);
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
			right_sbar->setPosition(0);
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
			right_sbar->setPosition(0);
			start_special_editing(2,0);
			break;
		case eMenu::TOWN_TEXT:
			right_sbar->setPosition(0);
			start_string_editing(STRS_TOWN,0);
			break;
		case eMenu::TOWN_SIGNS:
			right_sbar->setPosition(0);
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
			right_sbar->setPosition(0);
			start_string_editing(STRS_OUT_RECT,0);
			break;
		case eMenu::OUT_START:
			overall_mode = MODE_SET_OUT_START;
			set_string("Select party starting location.","");
			break;
		case eMenu::OUT_SPECIALS:
			right_sbar->setPosition(0);
			start_special_editing(1,0);
			break;
		case eMenu::OUT_TEXT:
			right_sbar->setPosition(0);
			start_string_editing(STRS_OUT,0);
			break;
		case eMenu::OUT_SIGNS:
			right_sbar->setPosition(0);
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

void Mouse_Pressed(sf::Event const & event) {
	location mousePos { translate_mouse_coordinates({event.mouseButton.x, event.mouseButton.y}) }; 
	handle_action(mousePos,event);
}

void close_program() {
}

// TODO: Remove this function and replace it with beep() or play_sound() everywhere.
void ding() {
	beep();
}
