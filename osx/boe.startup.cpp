
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
#include "dlgutil.h"

#include <vector>
using std::vector;

//extern party_record_type party;
//extern pc_record_type adven[6];
extern bool in_startup_mode,play_sounds,party_in_memory;
extern long register_flag;
extern WindowPtr	mainPtr;	
extern Point ul;
//extern piles_of_stuff_dumping_type *data_store;
extern cScenarioList scen_headers;;
extern bool unreg_party_in_scen_not_check;
//extern std::vector<std::string> scen_names;;
extern cUniverse univ;

//void start_game();

Rect startup_button[6];

bool handle_startup_press(Point the_point)
{
	std::string scen_name;
	short i,scen;

	the_point.h -= ul.h;
	the_point.v -= ul.v;
	
	for (i = 0; i < 5; i++) 
		if (PtInRect(the_point,&startup_button[i]) == true) {
			draw_start_button(i,5);
			if (play_sounds == true)
				play_sound(37);
			else FlushAndPause(5);
			draw_start_button(i,0);			
			switch (i) {
			case STARTBTN_LOAD:
				startup_load();
				break;
			
			case STARTBTN_NEW:
				draw_startup(0);
				start_new_game();
				update_pc_graphics();
				make_cursor_sword();
				draw_startup(0);
				break;
		
			case STARTBTN_ORDER:
				give_reg_info();
				draw_startup(0);
				break;

			case STARTBTN_JOIN: // regular scen
				if (party_in_memory == false) {
					FCD(867,0);
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
					FCD(867,0);
					break;
				}
				// if not reg, rub out
				
				scen = pick_a_scen();
				if(scen < 0) break;
				if (scen_headers.data(scen).prog_make_ver[0] >= 2) {
					FCD(912,0);
					break;
				}
				scen_name = scen_headers.strs(scen).file;
				put_party_in_scen(scen_name);
				break;
		
			case 5:
				FlushAndPause(50);
				return true;
				break;
		
			}
		}
	return false;
}

void startup_load()////
{
	try{
		FSSpec file_to_load = nav_get_party();
		if(load_party(file_to_load)){
			party_in_memory = true;
			update_pc_graphics();
			if(univ.party.scen_name.length() > 0)
				in_startup_mode = false;
			else in_startup_mode = true;
		}
	} catch(no_file_chosen){}
	if (!in_startup_mode) {
		//end_anim();
		end_startup();
		post_load();
	}
	else {
		menu_activate(0);
		draw_startup(0);
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
