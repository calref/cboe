#include <Carbon/Carbon.h>

#include "global.h"
#include "newgraph.h"
#include "blxgraphics.h"
#include "blxfileio.h"
#include "blxactions.h"
#include "dialogutils.h"
#include "text.h"
#include "blx.g.utils.h"
#include "items.h"
#include "party.h"
//#include "sound.h"
#include "startup.h"
#include "party.h"
#include "Exile.sound.h"

#include <vector>
using std::vector;

extern party_record_type party;
extern pc_record_type adven[6];
extern Boolean in_startup_mode,registered,play_sounds,party_in_memory;
extern long register_flag;
extern WindowPtr	mainPtr;	
extern Point ul;
extern piles_of_stuff_dumping_type *data_store;
extern vector<scen_header_type> scen_headers;
extern Boolean unreg_party_in_scen_not_check;

//void start_game();

Rect startup_button[6];

Boolean handle_startup_press(Point the_point)
{

	short i,scen;

	the_point.h -= ul.h;
	the_point.v -= ul.v;
	
	for (i = 0; i < 5; i++) 
		if (PtInRect(the_point,&startup_button[i]) == TRUE) {
			draw_start_button(i,5);
			if (play_sounds == TRUE)
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
				if (party_in_memory == FALSE) {
					FCD(867,0);
					break;
				}
				scen = pick_prefab_scen();
				if (scen < 0)
					break;
				if ((registered == FALSE) && (scen > 0)) {
					FCD(913,0);
					break;
				}
				
				switch (scen) {
				case 0: sprintf(party.scen_name,"valleydy.exs"); break;
					// if not reg, rub out
				case 1: sprintf(party.scen_name,"stealth.exs"); break;
				case 2: sprintf(party.scen_name,"zakhazi.exs"); break;
				}
				put_party_in_scen();
				break;
		
			case STARTBTN_CUSTOM: // custom
				if (party_in_memory == FALSE) {
					FCD(867,0);
					break;
				}
				if (registered == FALSE) {
					FCD(913,0);
					break;
				}
				// if not reg, rub out
				
				scen = pick_a_scen();
				if(scen < 0) break;
				if (scen_headers[scen].prog_make_ver[0] >= 2) {
					FCD(912,0);
					break;
				}
				if (registered == FALSE) 
					unreg_party_in_scen_not_check = TRUE;
				sprintf(party.scen_name,"%s",data_store->scen_names[scen].c_str());
				put_party_in_scen();
				break;
		
			case 5:
				FlushAndPause(50);
				return TRUE;
				break;
		
			}
		}
	return FALSE;
}

void startup_load()////
{
					load_file();
					update_pc_graphics();
					if (in_startup_mode == FALSE) {
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
	
	in_startup_mode = FALSE;
	
	adjust_monst_menu();
	adjust_spell_menus();
	
}*/