
#include "boe.global.hpp"

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

#include <vector>
using std::vector;

extern bool party_in_memory;
extern long register_flag;
extern sf::RenderWindow mainPtr;
extern location ul;
extern std::vector<scen_header_type> scen_headers;
extern cUniverse univ;
extern eGameMode overall_mode;

rectangle startup_button[6];

// TODO: Always returns false, so make it void
bool handle_startup_press(location the_point) {
	using kb = sf::Keyboard;
	std::string scen_name;
	bool force_party = false;
	
	the_point.x -= ul.x;
	the_point.y -= ul.y;
	
	for(short i = 0; i < 5; i++)
		if(the_point.in(startup_button[i])) {
			draw_start_button(i,5);
			mainPtr.display(); // TODO: I suspect this won't work
			play_sound(37, time_in_ticks(5));
			draw_start_button(i,0);
			switch(i) {
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
					// TODO: Figure out something to put here.
//					give_reg_info();
//					draw_startup(0);
					break;
					
				case STARTBTN_JOIN: // regular scen
					if(!party_in_memory) {
						if(kb::isKeyPressed(kb::LAlt) || kb::isKeyPressed(kb::RAlt)) {
							force_party = true;
							start_new_game(true);
						} else {
							cChoiceDlog("need-party").show();
							break;
						}
					}
					
					switch(pick_prefab_scen()) {
						case 0: scen_name = "valleydy.boes"; break;
						case 1: scen_name = "stealth.boes"; break;
						case 2: scen_name = "zakhazi.boes"; break;
						case 3: scen_name = "busywork.boes"; break;
						default:
							if(force_party) party_in_memory = false;
							break;
					}
					put_party_in_scen(scen_name);
					if(force_party && scen_name != univ.party.scen_name)
						party_in_memory = false;
					break;
					
				case STARTBTN_CUSTOM: // custom
					if(!party_in_memory) {
						if(kb::isKeyPressed(kb::LAlt) || kb::isKeyPressed(kb::RAlt)) {
							force_party = true;
							start_new_game(true);
						} else {
							cChoiceDlog("need-party").show();
							break;
						}
					}
					
					int scen = pick_a_scen();
					if(scen < 0) {
						if(force_party)
							party_in_memory = false;
						break;
					}
					if(scen_headers[scen].prog_make_ver[0] > 2 || scen_headers[scen].prog_make_ver[1] > 0) {
						if(force_party)
							party_in_memory = false;
						cChoiceDlog("scen-version-mismatch").show();
						break;
					}
					scen_name = scen_headers[scen].file;
					put_party_in_scen(scen_name);
					if(force_party && scen_name != univ.party.scen_name)
						party_in_memory = false;
					break;
			}
		}
	return false;
}

void handle_splash_events() {
	extern sf::Event event;
	if(!mainPtr.pollEvent(event)) return;
	if(event.type == sf::Event::GainedFocus || event.type == sf::Event::MouseMoved)
		set_cursor(sword_curs);
}

void show_logo() {
	rectangle whole_window,from_rect;
	rectangle logo_from = {0,0,350,350};
	
	if(get_int_pref("DisplayMode") != 5)
		hideMenuBar();
	
	whole_window = rectangle(mainPtr);
	logo_from.offset((whole_window.right - logo_from.right) / 2,(whole_window.bottom - logo_from.bottom) / 2);
	sf::Texture& pict_to_draw = *ResMgr::get<ImageRsrc>("spidlogo");
	from_rect = rectangle(pict_to_draw);
	
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
	rectangle whole_window,from_rect;
	rectangle intro_from = {0,0,480,640};
	whole_window = rectangle(mainPtr);
	sf::Time delay = time_in_ticks(220);
	intro_from.offset((whole_window.right - intro_from.right) / 2,(whole_window.bottom - intro_from.bottom) / 2);
	sf::Texture& pict_to_draw = *ResMgr::get<ImageRsrc>("startsplash");
	
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
