
#include "boe.global.hpp"

#include "universe/universe.hpp"
#include "boe.newgraph.hpp"
#include "boe.graphics.hpp"
#include "boe.fileio.hpp"
#include "boe.actions.hpp"
#include "boe.dlgutil.hpp"
#include "boe.text.hpp"
#include "boe.graphutil.hpp"
#include "boe.items.hpp"
#include "boe.party.hpp"
#include "boe.main.hpp"
#include "sounds.hpp"
#include "fileio/fileio.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "tools/keymods.hpp"
#include "tools/winutil.hpp"
#include "boe.menus.hpp"
#include "mathutil.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "tools/prefs.hpp"
#include "tools/cursors.hpp"
#include "gfx/render_image.hpp"
#include "tools/enum_map.hpp"
#include "replay.hpp"

#include <vector>
using std::vector;

#include <sstream>

extern bool party_in_memory;
extern long register_flag;
extern sf::RenderWindow mainPtr;
extern cUniverse univ;
extern eGameMode overall_mode;
extern sf::View mainView;

enum_map(eStartButton, rectangle) startup_button;

void handle_startup_button_click(eStartButton btn) {
	if(recording){
		std::ostringstream sstr;
		sstr << btn;
		record_action("startup_button_click", sstr.str());
	}

	std::string scen_name;
	bool force_party = false;

	draw_start_button(btn,5);
	mainPtr.display(); // TODO: I suspect this won't work
	play_sound(37, time_in_ticks(5));
	draw_start_button(btn,0);
	switch(btn) {
		case STARTBTN_LOAD:
			do_load();
			break;
			
		case STARTBTN_NEW:
			draw_startup(0);
			start_new_game();
			set_cursor(sword_curs);
			draw_startup(0);
			break;
			
		case STARTBTN_ORDER:
			pick_preferences();
			break;
		
		case STARTBTN_CUSTOM: break; // Currently unused
			
		case STARTBTN_SCROLL: case MAX_eStartButton:
			// These aren't buttons
			break;
			
		case STARTBTN_JOIN:
			if(!party_in_memory) {
				if(kb.isAltPressed()) {
					force_party = true;
					start_new_game(true);
				} else {
					cChoiceDlog("need-party").show();
					break;
				}
			}
			
			auto scen = pick_a_scen();
			if(scen.file.empty()) {
				if(force_party)
					party_in_memory = false;
				break;
			}
			if(scen.prog_make_ver[0] > 2 || scen.prog_make_ver[1] > 0) {
				if(force_party)
					party_in_memory = false;
				cChoiceDlog("scen-version-mismatch").show();
				break;
			}
			scen_name = scen.file;
			put_party_in_scen(scen_name);
			if(force_party && scen_name != univ.party.scen_name)
				party_in_memory = false;
			break;
	}
}

// TODO: Always returns false, so make it void
bool handle_startup_press(location the_point) {
	the_point = mainPtr.mapPixelToCoords(the_point, mainView);
	
	for(auto btn : startup_button.keys()) {
		if(btn == eStartButton::STARTBTN_SCROLL) continue;
		if(the_point.in(startup_button[btn])) {
			handle_startup_button_click(btn);
		}
	}
	return false;
}

void handle_splash_events() {
	sf::Event event;
	while(mainPtr.pollEvent(event)) {
		if(event.type == sf::Event::GainedFocus || event.type == sf::Event::MouseMoved)
			set_cursor(sword_curs);
	}
}

rectangle view_rect() {
	sf::Vector2f size = mainPtr.getView().getSize();
	return rectangle(0, 0, size.y, size.x);
}

void show_logo() {
	rectangle whole_window = view_rect();
	
	if(get_int_pref("DisplayMode") != 5)
		hideMenuBar();
	
	double ui_scale = get_float_pref("UIScale", 1.0);
	if(ui_scale < 1) ui_scale = 1;
	rectangle logo_from = {0, 0, int(ui_scale *350), int(ui_scale * 350)};
	logo_from.offset((whole_window.right - logo_from.right) / 2,(whole_window.bottom - logo_from.bottom) / 2);
	sf::Texture& pict_to_draw = *ResMgr::graphics.get("spidlogo", true);
	
	play_sound(-95);
	while(sound_going(95)) {
		draw_splash(pict_to_draw, mainPtr, logo_from);
		handle_splash_events();
	}
	if(!get_int_pref("ShowStartupSplash", true)) {
		sf::Time delay = time_in_ticks(60);
		sf::Clock timer;
		while(timer.getElapsedTime() < delay)
			handle_splash_events();
	}
}

void plop_fancy_startup() {
	rectangle whole_window = view_rect();

	float ui_scale = get_float_pref("UIScale", 1.0);
	if (ui_scale<1) ui_scale=1;
	rectangle from_rect;
	rectangle intro_from = {0, 0, int(ui_scale * 480), int(ui_scale * 640)};
	sf::Time delay = time_in_ticks(220);
	intro_from.offset((whole_window.right - intro_from.right) / 2,(whole_window.bottom - intro_from.bottom) / 2);
	sf::Texture& pict_to_draw = *ResMgr::graphics.get("startsplash", true);
	
	play_sound(-22);
	sf::Clock timer;
	
	while(timer.getElapsedTime() < delay) {
		draw_splash(pict_to_draw, mainPtr, intro_from);
		handle_splash_events();
	}
}

/*
void start_game () {
	init_party(0);
	
	setup_outdoors(party.p_loc);
	
	load_area_graphics();
	
	draw_main_screen();
	
	in_startup_mode = false;
	
	adjust_monst_menu();
	adjust_spell_menus();
	
}*/
