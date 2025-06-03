
#include <cstdio>
#include <string>
#include <memory>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/process/child.hpp>
#include <boost/process/io.hpp>
#include <boost/process/search_path.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "cli.hpp"

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
#include "dialogxml/widgets/field.hpp"
#include "dialogxml/widgets/led.hpp"
#include "dialogxml/widgets/ledgroup.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include "tools/winutil.hpp"
#include "tools/cursors.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/strchoice.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "scen.menus.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "tools/prefs.hpp"
#include "tools/framerate_limiter.hpp"
#include "tools/event_listener.hpp"
#include "tools/drawable_manager.hpp"
#include "tools/keymods.hpp"
#include "universe/universe.hpp"

#ifdef __APPLE__
short menuChoiceId=-1;
#endif

/* Globals */
bool  All_Done = false;
bool changed_display_mode = false;
sf::View mainView;
cTown* town = nullptr;
bool mouse_button_held = false,editing_town = false;
short cur_viewing_mode = 0;
short cen_x, cen_y;
eScenMode overall_mode = MODE_INTRO_SCREEN;
std::shared_ptr<cScrollbar> right_sbar, pal_sbar;
std::shared_ptr<cTextField> palette_search_field;
short mode_count = 0;
short right_button_hovered = -1;

cOutdoors* current_terrain;

std::string scenario_temp_dir_name = "ed_scenario";
bool change_made = false, ae_loading = false;
std::vector<fs::path> extra_scen_dirs;

// Set this based on context before calling give_help():
std::string help_text_rsrc = "";

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
void adjust_windows(sf::RenderWindow&, sf::View&);
sf::FloatRect compute_viewport(const sf::RenderWindow&, float ui_scale);

void pick_preferences();
void save_prefs();

cScenario scenario;
rectangle right_sbar_rect;
extern rectangle terrain_buttons_rect;
rectangle search_field_text_rect;
rectangle search_field_rect;

extern void set_up_apple_events();

// TODO: these should be members of some global entity instead of being here
std::unordered_map<std::string, std::shared_ptr <iEventListener>> event_listeners;
cDrawableManager drawable_mgr;

fs::path scenedit_dir;
fs::path game_dir;
fs::path game_binary;
extern std::string last_load_file;

enum class eLaunchType {LOC,START,ENTRANCE};

static void launch_scenario(eLaunchType type) {
	// Make sure scenario is loaded and currently editing the terrain of a town or outdoor section
	if(type == eLaunchType::LOC){
		if(overall_mode >= MODE_MAIN_SCREEN){
			showError("Must be viewing the terrain of a town or outdoor section at the place where you want to put the debug party.");
			return;
		}
	}
	// Make sure scenario is loaded
	else if(overall_mode == MODE_INTRO_SCREEN){
		showError("Must have a scenario loaded.");
		return;
	}

	// Prompt to save first
	if(!save_check("save-before-launch", false)) return;

	fs::path app_folder = fs::current_path();
	fs::path game_binary;
#ifdef SFML_SYSTEM_MACOS
	game_binary = "Blades of Exile.app/Contents/MacOS/Blades of Exile";
	// Run by double-click on app bundle
	if(app_folder == "/"){
		extern fs::path bundlePath();
		app_folder = bundlePath().parent_path();
	}
#elif defined(SFML_SYSTEM_WINDOWS)
	game_binary = "Blades of Exile.exe";
#elif defined(SFML_SYSTEM_LINUX)
	game_binary = "Blades of Exile";
#endif

	game_binary = bp::search_path(game_binary, {app_folder});
	if(game_binary.empty()){
		showError("The current working directory does not contain Blades of Exile.");
		return;
	}

	std::vector<std::string> args;
	args.push_back("--scenario");
	args.push_back(last_load_file);
	if(type == eLaunchType::LOC){
		if(editing_town){
			args.push_back("--town");
			args.push_back(boost::lexical_cast<std::string>(cur_town));
		}else{
			location out_sec = {cur_out.x, cur_out.y};
			args.push_back("--out-sec");
			args.push_back(boost::lexical_cast<std::string>(out_sec));
		}
		location loc = {cen_x, cen_y};
		args.push_back("--loc");
		args.push_back(boost::lexical_cast<std::string>(loc));
	}else if(type == eLaunchType::ENTRANCE){
		args.push_back("--town");
		args.push_back(boost::lexical_cast<std::string>(cur_town));

		std::ostringstream prompt;
		prompt << "Launch in " << scenario.towns[cur_town]->name << " at which entrance?";
		std::vector<std::string> choices = {"North", "East", "South", "West"};
		cStringChoice dlog(choices, prompt.str());
		size_t choice = dlog.show(0);
		if(dlog->accepted()){
			args.push_back("--entrance");
			args.push_back(boost::lexical_cast<std::string>(choice));
		}else{
			// Cancel
			return;
		}
	}else if(type == eLaunchType::START){
		args.push_back("--start");
	}

	// allow specifying a debug party path as an editor preference
	std::string default_party = get_string_pref("DefaultPartyPath");
	if(!(get_bool_pref("ForceDefaultParty", false) || default_party.empty())){
		args.push_back(default_party);
	}

	bp::child ch(game_binary, args=args, bp::std_out > stdout);
	ch.detach();
}

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

static void init_sbar(std::shared_ptr<cScrollbar>& sbar, const std::string& name, rectangle rect, rectangle events_rect, int pgSz) {
	static cParentless mainWin(mainPtr());
	sbar.reset(new cScrollbar(mainWin));
	sbar->setBounds(rect);
	sbar->set_wheel_event_rect(events_rect);
	sbar->setPageSize(pgSz);
	sbar->hide();
	
	drawable_mgr.add_drawable(UI_LAYER_DEFAULT, name, sbar);
	event_listeners[name] = std::dynamic_pointer_cast<iEventListener>(sbar);
}

static void init_scrollbars() {
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

static void init_search_field() {
	search_field_text_rect.top = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT + 16;
	search_field_text_rect.bottom = search_field_text_rect.top + 12;
	search_field_text_rect.left = RIGHT_AREA_UL_X + 5;
	TextStyle style;
	search_field_text_rect.right = search_field_text_rect.left + string_length("Search: ", style);
	search_field_rect = search_field_text_rect;
	search_field_rect.offset({search_field_text_rect.width() + 5, -2});
	search_field_rect.width() = RIGHT_AREA_WIDTH / 2;
	
	static cParentless mainWin(mainPtr());
	palette_search_field.reset(new cTextField(mainWin));
	palette_search_field->setBounds(search_field_rect);
	palette_search_field->show();
	drawable_mgr.add_drawable(UI_LAYER_DEFAULT, "search_field", palette_search_field);
	event_listeners["search_field"] = std::dynamic_pointer_cast<iEventListener>(palette_search_field);
}

sf::FloatRect compute_viewport(const sf::RenderWindow& mainPtr, float ui_scale) {

	// See compute_viewport() in boe.graphics.cpp
	sf::FloatRect viewport;
	
	viewport.top    = float(os_specific_y_offset()) / mainPtr.getSize().y;
	viewport.left   = 0;
	viewport.width  = ui_scale;
	viewport.height = ui_scale;
	
	return viewport;
}

void adjust_windows (sf::RenderWindow& mainPtr, sf::View & mainView) {

	// TODO: things might still be broken when upscaled.
	//   translate_mouse_coordinates has been applied in some places but more work might be needed.
	//   In particular, the white area on the right side of the main menu needs fixing.
	double ui_scale = get_ui_scale();
	
	const int width  = ui_scale * scen_width;
	const int height = ui_scale * scen_height + os_specific_y_offset();
	
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

#ifndef SFML_SYSTEM_MACOS // This overrides Dock icon on OSX, which isn't what we want at all
	const ImageRsrc& icon = ResMgr::graphics.get("icon", true);
	mainPtr.setIcon(icon->getSize().x, icon->getSize().y, icon->copyToImage().getPixelsPtr());
#endif
	init_menubar();
	adjust_window_for_menubar(5, width, height);
}

static void process_args(int argc, char* argv[]) {
	preprocess_args(argc, argv);
	clara::Args args(argc, argv);
	clara::Parser cli;
	std::string file;
	cli |= clara::Arg(file, "file")("The scenario file to open");
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
	if(!file.empty()) {
		if(load_scenario(file, scenario)) {
			set_pref("LastScenario", file);
			restore_editor_state(true);
			change_made = false;
			ae_loading = true;
		} else {
			std::cout << "Failed to load scenario: " << file << std::endl;
		}
	}
}

void init_scened(int argc, char* argv[]) {
	init_directories(argv[0]);
	sync_prefs();
	adjust_windows(mainPtr(), mainView);
	//init_menubar();
	init_shaders();
	init_tiling();
	init_snd_tool();
#ifdef SFML_SYSTEM_MACOS
	init_menubar(); // This is called twice because Windows and Mac have different ordering requirements
#endif
	mainPtr().clear(sf::Color::Black);
	mainPtr().display();
	
	set_cursor(watch_curs);
	game_rand.seed(time(nullptr));
		
	cen_x = 18;
	cen_y = 18;
		
	init_scrollbars();
	init_search_field();
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
	cDialog::defaultDoAnimations = true;
	set_up_apple_events();
	process_args(argc, argv);
	init_fileio();
}

void handle_events() {
	sf::Event currentEvent;
	cFramerateLimiter fps_limiter;

	while(!All_Done) {
		if(changed_display_mode) {
			changed_display_mode = false;
			adjust_windows(mainPtr(), mainView);
		}

#ifdef __APPLE__
		if (menuChoiceId>=0) {
			handle_menu_choice(eMenu(menuChoiceId));
			menuChoiceId=-1;
		}
#endif
		while(pollEvent(mainPtr(), currentEvent)) handle_one_event(currentEvent);

		// Why do we have to set this to false after handling every event?
		ae_loading = false;

		redraw_everything();

		// Prevent the loop from executing too fast.
		fps_limiter.frame_finished();
	}
}

void handle_one_event(const sf::Event& event) {
	
	// Check if any of the event listeners want this event.
	for (auto& listener : event_listeners) {
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
			commit_stroke();
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

static void show_outdated_warning() {
	std::string outdated_help1 = "The information in the following help window is extremely outdated! The most up-to-date documentation for OpenBoE is here:";
	std::string outdated_help2 = "https://openboe.com/docs";

	showWarning(outdated_help1, outdated_help2);
}

void handle_menu_choice(eMenu item_hit) {
	extern cUndoList undo_list;
	bool isEdit = false, isHelp = false, isOutdated = false;
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
				set_pref("LastScenario", file_to_load.string());
				restore_editor_state(true);
				change_made = false;
			} else if(!file_to_load.empty())
				set_up_start_screen(); // Failed to load file, dump to start
			undo_list.clear();
			update_edit_menu();
			break;
		case eMenu::FILE_SAVE:
			store_current_terrain_state();
			save_scenario();
			break;
		case eMenu::FILE_SAVE_AS:
			store_current_terrain_state();
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
		case eMenu::LAUNCH_HERE:
			launch_scenario(eLaunchType::LOC);
			break;
		case eMenu::LAUNCH_START:
			launch_scenario(eLaunchType::START);
			break;
		case eMenu::LAUNCH_ENTRANCE:
			launch_scenario(eLaunchType::ENTRANCE);
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
		case eMenu::SCEN_ADV_DETAILS:
			edit_scen_adv_details();
			change_made = true;
			break;
		case eMenu::SCEN_INTRO:
			edit_scen_intro();
			change_made = true;
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
				showError("You can't delete the only town in a scenario. All scenarios must have at least 1 town.");
				return;
			}
			if(scenario.towns.size() - 1 == cur_town) {
				showError("You can't delete the last town in a scenario while you're working on it. Load a different town, and try this again.");
				return;
			}
			if(scenario.towns.size() - 1 == scenario.which_town_start) {
				showError("You can't delete the last town in a scenario while it's the town the party starts the scenario in. Change the party's starting point and try this again.");
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
			if(!town->any_items()){
				cChoiceDlog("no-items").show();
				break;
			}
			for(int i = 0; i < town->preset_items.size(); i++)
				town->preset_items[i].property = 0;
			cChoiceDlog("set-not-owned").show();
			draw_terrain();
			change_made = true;
			break;
		case eMenu::TOWN_ITEMS_CLEAR:{
			if(!town->any_items()){
				cChoiceDlog("no-items").show();
				break;
			}
			if(cChoiceDlog("clear-items-confirm", {"okay", "cancel"}).show() == "cancel")
				break;
			item_changes_t changes;
			auto& town_items = town->preset_items;
			for(size_t i = 0; i < town_items.size(); ++i){
				if(town_items[i].code < 0) continue;
				changes[i] = town_items[i];
				town_items[i].code = -1;
			}
			undo_list.add(action_ptr(new aPlaceEraseItem("Clear Items", false, changes)));
			update_edit_menu();

			draw_terrain();
			change_made = true;
		}break;
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
			launchDocs("editor/Contents.html");
			break;
		case eMenu::HELP_START:
			helpDlog = "help-editing";
			isHelp = true;
			isOutdated = true;
			break;
		case eMenu::HELP_TEST:
			helpDlog = "help-testing";
			isHelp = true;
			isOutdated = true;
			break;
		case eMenu::HELP_DIST:
			helpDlog = "help-distributing";
			isHelp = true;
			isOutdated = true;
			break;
		case eMenu::HELP_CONTEST:
			helpDlog = "help-contest";
			isHelp = true;
			isOutdated = true;
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
	if(isHelp){
		if(isOutdated){
			show_outdated_warning();
		}
		cChoiceDlog(helpDlog).show();
	}
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
		if(scale == "1") set_pref("UIScale", 1.0);
		else if(scale == "1_5") set_pref("UIScale", 1.5);
		else if(scale == "2") set_pref("UIScale", 2.0);
		else if(scale == "3") set_pref("UIScale", 3.0);
		else if(scale == "4") set_pref("UIScale", 4.0);
		set_pref("PlaySounds", dynamic_cast<cLed&>(me["nosound"]).getState() == led_off);
		set_pref("ForceDefaultParty", dynamic_cast<cLed&>(me["force-default-party"]).getState() == led_red);
		set_pref("DefaultPartyPath", dynamic_cast<cTextField&>(me["party-path"]).getText());
	}
	save_prefs();
	return true;
}

void pick_preferences() {
	set_cursor(sword_curs);
	
	cDialog prefsDlog(*ResMgr::dialogs.get("pref-scenario"));
	prefsDlog.attachClickHandlers(&prefs_event_filter, {"okay", "cancel"});
	
	cLedGroup& uiScale = dynamic_cast<cLedGroup&>(prefsDlog["scaleui"]);
	double ui_scale = get_ui_scale();
	if (ui_scale>0.95 && ui_scale<1.05) uiScale.setSelected("1");
	else if (ui_scale>1.45 && ui_scale<1.55) uiScale.setSelected("1_5");
	else if (ui_scale>1.95 && ui_scale<2.05) uiScale.setSelected("2");
	else if (ui_scale>2.95 && ui_scale<3.05) uiScale.setSelected("3");
	else if (ui_scale>3.95 && ui_scale<4.05) uiScale.setSelected("4");
	else uiScale.setSelected("other");
	
	if(uiScale.getSelected() == "other") {
		auto val = std::to_string(ui_scale);
		while(val.length() > 2 && val[val.length() - 1] == val[val.length() - 2]) {
			val.pop_back();
		}
		while(val.length() > 1 && (val.back() == '.' || val.back() == '0')) {
			val.pop_back();
		}
		uiScale["other"].setText("Custom: " + val);
	} else {
		uiScale["other"].hide();
	}

	// Disable buttons for scales that won't fit on scenario designer's screen:
	double max_main_window_scale = fallback_scale();
	if(max_main_window_scale < 4.0){
		uiScale["4"].hide();
	}
	if(max_main_window_scale < 3.0){
		uiScale["3"].hide();
	}
	if(max_main_window_scale < 2.0){
		uiScale["2"].hide();
	}
	if(max_main_window_scale < 1.5){
		uiScale["1_5"].hide();
	}
	
	dynamic_cast<cLed&>(prefsDlog["nosound"]).setState(get_bool_pref("PlaySounds", true) ? led_off : led_red);
	
	dynamic_cast<cLed&>(prefsDlog["force-default-party"]).setState(get_bool_pref("ForceDefaultParty", false) ? led_red : led_off);

	cTextField& default_party_field = dynamic_cast<cTextField&>(prefsDlog["party-path"]);
	default_party_field.setText(get_string_pref("DefaultPartyPath"));

	default_party_field.attachFocusHandler([](cDialog& me, std::string id, bool losing) -> bool {
		if(!losing) return true;
		std::string debug_party = me[id].getText();
		// Validate the debug party
		if(!debug_party.empty()){
			cUniverse univ;
			cCustomGraphics graphics;
			if(!load_party(debug_party, univ, graphics)){
				showError("Your chosen debug party could not be loaded.", "", &me);
				me[id].setText(get_string_pref("DefaultPartyPath"));
				return false;
			}
		}
		return true;
	});

	cButton& choose_button = dynamic_cast<cButton&>(prefsDlog["choose-party"]);
	choose_button.attachClickHandler([&default_party_field](cDialog&, std::string, eKeyMod) -> bool {
		fs::path new_path = nav_get_party();
		if(!new_path.empty()){
			default_party_field.setText(new_path.string());
		}
		return true;
	});

	prefsDlog.run();
	
	// Suppress the float comparison warning.
	// We know it's safe here - we're just comparing static values.
	#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wfloat-equal"
	#endif
	if(get_ui_scale() != ui_scale)
		changed_display_mode = true;
	#ifdef __GNUC__
	#pragma GCC diagnostic pop
	#endif
}

void Mouse_Pressed(const sf::Event & event) {
	location mousePos { translate_mouse_coordinates({event.mouseButton.x, event.mouseButton.y}) }; 
	handle_action(mousePos,event);
}

void close_program() {
}
