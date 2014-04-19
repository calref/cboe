
//#include "item.h"

#include "boe.global.h"

#include "classes.h"
#include "boe.newgraph.h"
#include "boe.graphics.h"
#include "boe.fileio.h"
#include "boe.actions.h"
#include "boe.dlgutil.h"
#include "boe.text.h"
#include "boe.graphutil.h"
#include "boe.items.h"
#include "boe.party.h"
#include "boe.startup.h"
#include "boe.party.h"
#include "soundtool.h"
#include "fileio.h"
#include "dlogutil.h"
#include "winutil.h"
#include "boe.menus.h"
#include "mathutil.h"

#include <vector>
using std::vector;

//extern party_record_type party;
//extern pc_record_type adven[6];
extern bool play_sounds,party_in_memory;
extern long register_flag;
extern sf::RenderWindow mainPtr;
extern location ul;
//extern piles_of_stuff_dumping_type *data_store;
extern cScenarioList scen_headers;;
//extern std::vector<std::string> scen_names;;
extern cUniverse univ;
extern eGameMode overall_mode;

//void start_game();

RECT startup_button[6];

// TODO: Always returns false, so make it void
bool handle_startup_press(location the_point)
{
	std::string scen_name;
	short i,scen;
	
	the_point.x -= ul.x;
	the_point.y -= ul.y;
	
	for (i = 0; i < 5; i++)
		if (the_point.in(startup_button[i]) == true) {
			draw_start_button(i,5);
			mainPtr.display(); // TODO: I suspect this won't work
			if (play_sounds == true)
				play_sound(37);
			else sf::sleep(time_in_ticks(5));
			draw_start_button(i,0);
			switch (i) {
				case STARTBTN_LOAD:
					startup_load();
					break;
					
				case STARTBTN_NEW:
					draw_startup(0);
					start_new_game();
					make_cursor_sword();
					draw_startup(0);
					break;
					
				case STARTBTN_ORDER:
					// TODO: Figure out something to put here.
//					give_reg_info();
//					draw_startup(0);
					break;
					
				case STARTBTN_JOIN: // regular scen
					if (party_in_memory == false) {
						cChoiceDlog("need-party.xml").show();
						break;
					}
					scen = pick_prefab_scen();
					if (scen < 0)
						break;
					
					switch (scen) {
						case 0: scen_name = "valleydy.exs"; break;
							// if not reg, rub out
						case 1: scen_name = "stealth.exs"; break;
						case 2: scen_name = "zakhazi.exs"; break;
							//case 3: sprintf(univ.party.scen_name,"busywork.exs"); break;
					}
					put_party_in_scen(scen_name);
					break;
					
				case STARTBTN_CUSTOM: // custom
					if (party_in_memory == false) {
						cChoiceDlog("need-party.xml").show();
						break;
					}
					// if not reg, rub out
					
					scen = pick_a_scen();
					if(scen < 0) break;
					if (scen_headers.data(scen).prog_make_ver[0] >= 2) {
						cChoiceDlog("scen-version-mismatch.xml").show();
						break;
					}
					scen_name = scen_headers.strs(scen).file;
					put_party_in_scen(scen_name);
					break;
			}
		}
	return false;
}

void startup_load()////
{
	bool in_startup_mode = true;
	fs::path file_to_load = nav_get_party();
	if(!file_to_load.empty() && load_party(file_to_load)){
		party_in_memory = true;
		if(univ.party.scen_name.length() > 0)
			in_startup_mode = false;
		else in_startup_mode = true;
	}
	if (!in_startup_mode) {
		//end_anim();
		end_startup();
		post_load();
	}
	else {
		menu_activate();
		draw_startup(0);
		overall_mode = MODE_STARTUP;
	}
	
}
/*
void start_game ()
{
	init_party(0);
	
	setup_outdoors(party.p_loc);
	
	load_area_graphics();
	
	draw_main_screen();
	
	in_startup_mode = false;
	
	adjust_monst_menu();
	adjust_spell_menus();
	
}*/
