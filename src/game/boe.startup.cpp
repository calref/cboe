
#include "boe.global.hpp"

#include "framerate_limiter.hpp"
#include "universe.hpp"
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
#include "fileio.hpp"
#include "choicedlog.hpp"
#include "winutil.hpp"
#include "boe.menus.hpp"
#include "mathutil.hpp"
#include "res_image.hpp"
#include "prefs.hpp"
#include "cursors.hpp"
#include "render_image.hpp"
#include "enum_map.hpp"

#include <vector>
using std::vector;

extern bool party_in_memory;
extern long register_flag;
extern sf::RenderWindow mainPtr;
extern cUniverse univ;
extern eGameMode overall_mode;
extern sf::View mainView;

enum_map(eStartButton, rectangle) startup_button;

// TODO: Always returns false, so make it void
bool handle_startup_press(location the_point) {
	using kb = sf::Keyboard;
	std::string scen_name;
	bool force_party = false;
	
	the_point = mainPtr.mapPixelToCoords(the_point, mainView);
	
	for(auto btn : startup_button.keys()) {
		if(btn == eStartButton::STARTBTN_SCROLL) continue;
		if(the_point.in(startup_button[btn])) {
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
					
				case STARTBTN_JOIN:
					if(!party_in_memory) {
						if(kb::isKeyPressed(kb::LAlt) || kb::isKeyPressed(kb::RAlt)) {
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
	}
	return false;
}

void handle_splash_events() {
	sf::Event event;
	if(!mainPtr.pollEvent(event)) return;
	if(event.type == sf::Event::GainedFocus || event.type == sf::Event::MouseMoved)
		set_cursor(sword_curs);
}

void show_logo() {
	rectangle whole_window;
	
	if(get_int_pref("DisplayMode") != 5)
		hideMenuBar();
	
	whole_window = rectangle(mainPtr);
	float ui_scale = get_float_pref("UIScale", 1.0);
	if (ui_scale<1) ui_scale=1;
	rectangle logo_from = {0,0,int(ui_scale*350),int(ui_scale*350)};
	logo_from.offset((whole_window.right - logo_from.right) / 2,(whole_window.bottom - logo_from.bottom) / 2);
	auto const &pict_to_draw = *ResMgr::textures.get("spidlogo", true);
	
	play_sound(-95);
	cFramerateLimiter fps_limiter;
	while(sound_going(95)) {
		draw_splash(pict_to_draw, mainPtr, logo_from);
		handle_splash_events();
		fps_limiter.frame_finished();
	}
	if(!get_int_pref("ShowStartupSplash", true)) {
		sf::Time delay = time_in_ticks(60);
		sf::Clock timer;
		while(timer.getElapsedTime() < delay) {
			handle_splash_events();
			fps_limiter.frame_finished();
		}
	}
}

void plop_fancy_startup() {
	float ui_scale = get_float_pref("UIScale", 1.0);
	if (ui_scale<1) ui_scale=1;
	rectangle whole_window,from_rect;
	rectangle intro_from = {0,0,int(ui_scale*480), int(ui_scale*640)};
	whole_window = rectangle(mainPtr);
	sf::Time delay = time_in_ticks(220);
	intro_from.offset((whole_window.right - intro_from.right) / 2,(whole_window.bottom - intro_from.bottom) / 2);
	auto const & pict_to_draw = *ResMgr::textures.get("startsplash", true);
	
	play_sound(-22);
	sf::Clock timer;
	
	cFramerateLimiter fps_limiter;
	while(timer.getElapsedTime() < delay) {
		draw_splash(pict_to_draw, mainPtr, intro_from);
		handle_splash_events();
		fps_limiter.frame_finished();
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
