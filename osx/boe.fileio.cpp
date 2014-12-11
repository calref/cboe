
#include <iostream>
#include <fstream>

//#include "item.h"

#include "boe.global.h"
#include "classes.h"
#include "boe.fileio.h"
#include "boe.text.h"
#include "boe.town.h"
#include "boe.items.h"
#include "boe.graphics.h"
#include "boe.locutils.h"
#include "boe.newgraph.h"
#include "boe.dlgutil.h"
#include "boe.infodlg.h"
#include "boe.graphutil.h"
#include "graphtool.h"
#include "soundtool.h"
#include "mathutil.h"
#include "dlogutil.h"
#include "fileio.h"
#include "porting.h" // only needed for one little flip_short call, though...
#include <boost/filesystem.hpp>
#include "restypes.hpp"

#define	DONE_BUTTON_ITEM	1

//extern party_record_type	party;
//extern pc_record_type adven[6];
//extern cOutdoors outdoors[2][2];
//extern unsigned char out[96][96],out_e[96][96];
extern short give_delays,stat_screen_mode;
extern eGameMode overall_mode;
extern bool play_sounds,sys_7_avail,save_maps,party_in_memory,in_scen_debug;
//extern current_town_type	c_town;
//extern town_item_list	t_i;
extern location center;
extern long register_flag;
extern sf::RenderWindow mainPtr;
//extern stored_items_list_type stored_items[3];
//extern stored_town_maps_type maps;
//extern stored_outdoor_maps_type o_maps;
//extern big_tr_type t_d;
//extern short town_size[3];
extern short town_type,current_pc;
//extern setup_save_type setup_save;
//extern unsigned char misc_i[64][64],sfx[64][64];
//extern unsigned char template_terrain[64][64];
//extern tiny_tr_type anim_t_d;
extern bool map_visible;
//extern location monster_targs[60];
extern sf::RenderWindow mini_map;
extern short which_combat_type;
extern short cur_town_talk_loaded;
extern cScenario scenario;
extern cUniverse univ;
//extern piles_of_stuff_dumping_type *data_store;
//std::vector<std::string> scen_names;
//stored_town_maps_type town_maps;
//extern talking_record_type talking;
//extern outdoor_strs_type outdoor_text[2][2];
cScenarioList scen_headers;
extern ter_num_t combat_terrain[64][64];
extern bool mac_is_intel;

bool loaded_yet = false, got_nagged = false,ae_loading = false;
char last_load_file[63] = "Blades of Exile Save";
fs::path file_to_load;
fs::path store_file_reply;
short jl;

extern bool cur_scen_is_mac;

void print_write_position ();
void save_outdoor_maps();
void add_outdoor_maps();

short specials_res_id,data_dump_file_id;
char start_name[256];
short start_volume,data_volume;
extern fs::path progDir;

//outdoor_record_type dummy_out;////
//town_record_type dummy_town;

// Trying this to fix bug. Hope it works.
//	tiny_tr_type tiny_t;
//	ave_tr_type ave_t;

//template_town_type town_template;
cCustomGraphics spec_scen_g;

extern bool pc_gworld_loaded;
extern sf::Texture pc_gworld;

std::ofstream flog("bladeslog.txt");

void finish_load_party(){
	bool town_restore = univ.town.num < 200;
	bool in_scen = univ.party.scen_name.length() > 0;
	
	party_in_memory = true;
	
	// now if not in scen, this is it.
	if (!in_scen) {
		if(overall_mode != MODE_STARTUP) {
			reload_startup();
			draw_startup(0);
		}
		if(!pc_gworld_loaded) {
			pc_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcs"));
			pc_gworld_loaded = true;
		}
		overall_mode = MODE_STARTUP;
		return;
	}
	
	fs::path path;
	path = progDir/"Blades of Exile Scenarios";
	path /= univ.party.scen_name;
	std::cout<<"Searching for scenario at:\n"<<path<<'\n';
	if (!load_scenario(path))
		return;
	
	// Saved creatures may not have had their monster attributes saved
	// Make sure that they know what they are!
	// Cast to cMonster base class and assign, to avoid clobbering other attributes
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 60; j++) {
			int number = univ.party.creature_save[i][j].number;
			cMonster& monst = univ.party.creature_save[i][j];
			monst = scenario.scen_monsters[number];
		}
	}
	for(int j = 0; j < 60; j++) {
		int number = univ.town.monst[j].number;
		cMonster& monst = univ.town.monst[j];
		monst = scenario.scen_monsters[number];
	}
	
	// if at this point, startup must be over, so make this call to make sure we're ready,
	// graphics wise
	end_startup();
	
	load_outdoors(loc(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y + 1),univ.out.outdoors[1][1]);
	load_outdoors(loc(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y + 1),univ.out.outdoors[0][1]);
	load_outdoors(loc(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y),univ.out.outdoors[1][0]);
	load_outdoors(loc(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y),univ.out.outdoors[0][0]);
	
	//end_anim();
	overall_mode = town_restore ? MODE_TOWN : MODE_OUTDOORS;
	stat_screen_mode = 0;
	build_outdoors();
	erase_out_specials();
	
	
	if (!town_restore) {
		center = univ.party.p_loc;
	}
	else {
		load_town(univ.town.num,univ.town.record);
		load_town_str(univ.town.num,univ.town.record);
		
		for (int i = 0; i < univ.town->max_monst(); i++){
			univ.town.monst[i].targ_loc.x = 0;
			univ.town.monst[i].targ_loc.y = 0;
		}
		
		town_type = scenario.town_size[univ.town.num];
		
		// Set up field booleans
		for (int j = 0; j < univ.town->max_dim(); j++)
			for (int k = 0; k < univ.town->max_dim(); k++) {
				if (univ.town.is_quickfire(j,k) == true)
					univ.town.quickfire_present = true;
				if(scenario.ter_types[univ.town->terrain(j,k)].special == eTerSpec::CONVEYOR)
					univ.town.belt_present = true;
			}
		center = univ.town.p_loc;
	}
	
	redraw_screen(REFRESH_ALL);
	current_pc = first_active_pc();
	loaded_yet = true;
	
	
	strcpy ((char *) last_load_file, file_to_load.filename().c_str());
	store_file_reply = file_to_load;
	
	add_string_to_buf("Load: Game loaded.            ");
	
	// Set sounds, map saving, and speed
	if (((play_sounds == true) && (PSD[SDF_NO_SOUNDS] == 1)) ||
		((play_sounds == false) && (PSD[SDF_NO_SOUNDS] == 0))) {
		flip_sound();
	}
	give_delays = PSD[SDF_NO_FRILLS];
	if (PSD[SDF_NO_MAPS] == 0)
		save_maps = true;
	else save_maps = false;
	
	in_scen_debug = false;
}

//void port_save_file(party_record_type* party){
//	if(!mac_is_intel) return;
//	short i,j;
//	flip_long(&univ.party->age);
//	flip_short(&univ.party->gold);
//	flip_short(&univ.party->food);
//	flip_short(&univ.party->light_level);
//	for(i = 0; i < 30; i++)
//		flip_short(&univ.party->boats[i].which_town);
//	for(i = 0; i < 30; i++)
//		flip_short(&univ.party->horses[i].which_town);
//	flip_short
//	creature_list_type creature_save[4];
//	short in_boat,in_horse;
//	outdoor_creature_type out_c[10];
//	cItemRec magic_store_items[5][10];
//	short imprisoned_monst[4];
//	char m_seen[256];
//	char journal_str[50];
//	short journal_day[50];
//	short special_notes_str[140][2];
//	talk_save_type talk_save[120];
//	short direction,at_which_save_slot;
//	char alchemy[20];
//	bool can_find_town[200];
//	short key_times[100];
//	short party_event_timers[30];
//	short global_or_town[30];
//	short node_to_call[30];
//	char spec_items[50],help_received[120];
//	short m_killed[200];
//	long total_m_killed,total_dam_done,total_xp_gained,total_dam_taken;
//	char scen_name[256];
//}


void swap_val(unsigned char *val,short a,short b)
{
	if (*val == a)
		*val = b;
	else if (*val == b)
		*val = a;
}
void change_val_4 (unsigned char *val,short a,short b,short c,short d)
{
	if (*val == a)
		*val = b;
	else if (*val == b)
		*val = c;
	else if (*val == c)
		*val = d;
	else if (*val == d)
		*val = a;
}
void change_val (unsigned char *val,short a,short b)
{
	if (*val == a)
		*val = b;
}



//void build_scen_file_name (Str255 file_n)
//{
//	sprintf((char *) file_n,"::::Blades of Exile Scenarios:%s",univ.party.scen_name);
//	c2pstr((char*)file_n);
//}

// mode 0 want town and talking, 1 talking only, 2 want a string only, and extra is string num
// Hey's let's be kludgy and overload these value again! If extra is -1, and mode 2, that
// means we want to load all the strings and only the strings
//void load_town(short town_num,short mode,short extra,char *str)
//{
//	short file_id;
//	short i,j;
//	long store = 0;
//	short which_town;
//	long len,len_to_jump = 0;
//	OSErr error;
//	char file_name[256];
//
//	if (town_num != minmax(0,scenario.num_towns - 1,town_num)) {
//		give_error("The scenario tried to place you into a non-existant town.","",0);
//		return;
//		}
//
//	which_town = town_num;
//
//	//HGetVol((StringPtr) start_name,&start_volume,&start_dir);
//	build_scen_file_name(file_name);
//
//	error = HOpen(start_volume,start_dir,file_name,3,&file_id);
//	if (error != 0) {
//		//FCD(949,0);
//		SysBeep(50);
//		return;
//		}
//	len_to_jump = sizeof(scenario_data_type);
//	len_to_jump += sizeof(scen_item_data_type);
//	for (i = 0; i < 300; i++)
//		len_to_jump += (long) scenario.scen_str_len[i];
//	store = 0;
//	for (i = 0; i < 100; i++)
//		for (j = 0; j < 2; j++)
//			store += (long) (scenario.out_data_size[i][j]);
//	for (i = 0; i < which_town; i++)
//		for (j = 0; j < 5; j++)
//			store += (long) (scenario.town_data_size[i][j]);
//	len_to_jump += store;
//
//	error = SetFPos (file_id, 1, len_to_jump);
//	if (error != 0) {FSClose(file_id);oops_error(35);}
//
//	len = sizeof(town_record_type);
//
//	if (mode == 0) {
//		error = FSRead(file_id, &len , (char *) &univ.town.town);
//		port_town();
//		}
//		else error = FSRead(file_id, &len , (char *) &dummy_town);
//	if (error != 0) {FSClose(file_id);oops_error(36);}
//
//	switch (scenario.town_size[which_town]) {
//		case 0:
//			len =  sizeof(big_tr_type);
//			if (mode == 0) {
//				FSRead(file_id, &len, (char *) &t_d);
//				port_t_d();
//				}
//				else error = SetFPos (file_id, 3, len);
//
//			break;
//
//		case 1:
//				len = sizeof(ave_tr_type);
//				if (mode == 0) {
//					FSRead(file_id, &len, (char *) &ave_t);
//					for (i = 0; i < 48; i++)
//						for (j = 0; j < 48; j++) {
//							t_d.terrain[i][j] = ave_t.terrain[i][j];
//							t_d.lighting[i / 8][j] = ave_t.lighting[i / 8][j];
//							}
//					//print_nums(5555,5555,5555);
//					//for (i = 0; i < 8; i++)
//					//	for (j = 0; j < 48; j++)
//					//		if (t_d.lighting[i][j] != 0) {
//					//			print_nums(i,j,t_d.lighting[i][j]);
//					//			}
//
//					for (i = 0; i < 16; i++) {
//						t_d.room_rect[i] = ave_t.room_rect[i];
//						}
//					for (i = 0; i < 40; i++) {
//						t_d.creatures[i] = ave_t.creatures[i];
//						}
//					for (i = 40; i < 60; i++) {
//						t_d.creatures[i].number = 0;
//						}
//					port_t_d();
//					}
//					else error = SetFPos (file_id, 3, len);
//
//			break;
//
//		case 2:
//			len = sizeof(tiny_tr_type);
//			if (mode == 0) {
//				FSRead(file_id,&len , (char *) &tiny_t);
//				for (i = 0; i < 32; i++)
//					for (j = 0; j < 32; j++) {
//						t_d.terrain[i][j] = tiny_t.terrain[i][j];
//						t_d.lighting[i / 8][j] = tiny_t.lighting[i / 8][j];
//						}
//				for (i = 0; i < 16; i++) {
//					t_d.room_rect[i] = tiny_t.room_rect[i];
//				}
//				for (i = 0; i < 30; i++) {
//					t_d.creatures[i] = tiny_t.creatures[i];
//					}
//				for (i = 30; i < 60; i++) {
//					t_d.creatures[i].number = 0;
//					}
//				port_t_d();
//				}
//				else error = SetFPos (file_id, 3, len);
//			break;
//		}
//
//	for (i = 0; i < 140; i++) {
//		len = (mode == 0) ? (long) (univ.town.town.strlens[i]) : (long) (dummy_town.strlens[i]);
//		switch (mode) {
//			case 0:
//				FSRead(file_id, &len, (char *) &(town_strs[i]));
//				data_store->town_strs[i][len] = 0;
//				break;
//			case 1:
//				SetFPos (file_id, 3, len);
//				break;
//			case 2:
//				if (extra < 0) {
//					FSRead(file_id, &len, (char *) &(data_store->town_strs[i]));
//					data_store->town_strs[i][len] = 0;
//					}
//				else if (i == extra) {
//					FSRead(file_id, &len, (char *) str);
//					str[len] = 0;
//					}
//					else SetFPos (file_id, 3, len);
//				break;
//			}
//		}
//
//	if (mode < 2) {
//		len = sizeof(talking_record_type);
//		error = FSRead(file_id, &len , (char *) &talking);
//		port_talk_nodes();
//		if (error != 0) {FSClose(file_id);oops_error(37);}
//
//		for (i = 0; i < 170; i++) {
//			len = (long) (talking.strlens[i]);
//			FSRead(file_id, &len, (char *) &(talk_strs[i]));
//			data_store->talk_strs[i][len] = 0;
//			}
//		cur_town_talk_loaded = town_num;
//		}
//	if (mode == 0)
//		town_type = scenario.town_size[which_town];
//	error = FSClose(file_id);
//	if (error != 0) {FSClose(file_id);oops_error(38);}
//
//	// Now more initialization is needed. First need to properly create the misc_i array.
//
void init_town(){ // formerly part of load_town
	// Initialize barriers, etc. Note non-sfx gets forgotten if this is a town recently visited.
//	if (mode == 0) {
	unsigned int i,j;
	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++) {
			univ.town.fields[i][j] = 0;
			//univ.out.sfx[i][j] = 0;
		}
	for (i = 0; i < 50; i++)
		if (univ.town->special_locs[i].x < 100){
			univ.town.set_special(univ.town->special_locs[i].x,univ.town->special_locs[i].y,true);
		}
	for (i = 0; i < univ.town->preset_fields.size(); i++) {
		switch(univ.town->preset_fields[i].type){
			case 1: // currently unused
				univ.town.set_block(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 2:
				univ.town.set_spot(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 3:
				univ.town.set_web(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 4:
				univ.town.set_crate(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 5:
				univ.town.set_barrel(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 6:
				univ.town.set_fire_barr(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 7:
				univ.town.set_force_barr(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 8:
				univ.town.set_quickfire(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 14:
				univ.town.set_sm_blood(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 15:
				univ.town.set_med_blood(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 16:
				univ.town.set_lg_blood(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 17:
				univ.town.set_sm_slime(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 18:
				univ.town.set_lg_slime(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 19:
				univ.town.set_ash(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 20:
				univ.town.set_bones(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
			case 21:
				univ.town.set_rubble(univ.town->preset_fields[i].loc.x,univ.town->preset_fields[i].loc.y,true);
				break;
		}
		
	}
//	}
}


void shift_universe_left()
{
	short i,j;
	
	make_cursor_watch();
	
	save_outdoor_maps();
	univ.party.outdoor_corner.x--;
	univ.party.i_w_c.x++;
	univ.party.p_loc.x += 48;
	univ.out.outdoors[1][0] = univ.out.outdoors[0][0];
	univ.out.outdoors[1][1] = univ.out.outdoors[0][1];
//	outdoor_text[1][0] = outdoor_text[0][0];
//	outdoor_text[1][1] = outdoor_text[0][1];
	
	for (i = 48; i < 96; i++)
		for (j = 0; j < 96; j++)
			univ.out.out_e[i][j] = univ.out.out_e[i - 48][j];
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 96; j++)
			univ.out.out_e[i][j] = 0;
	
	for (i = 0; i < 10; i++) {
		if (univ.party.out_c[i].m_loc.x > 48)
			univ.party.out_c[i].exists = false;
		if (univ.party.out_c[i].exists == true)
			univ.party.out_c[i].m_loc.x += 48;
	}
	
	load_outdoors(loc(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y),univ.out.outdoors[0][0]);
	load_outdoors(loc(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y + 1),univ.out.outdoors[0][1]);
	build_outdoors();
	restore_cursor();
	
}

void shift_universe_right()
{
	short i,j;
	
	make_cursor_watch();
	save_outdoor_maps();
	univ.party.outdoor_corner.x++;
	univ.party.i_w_c.x--;
	univ.party.p_loc.x -= 48;
	univ.out.outdoors[0][0] = univ.out.outdoors[1][0];
	univ.out.outdoors[0][1] = univ.out.outdoors[1][1];
//	outdoor_text[0][0] = outdoor_text[1][0];
//	outdoor_text[0][1] = outdoor_text[1][1];
	for (i = 0; i < 48; i++)
		for (j = 0; j < 96; j++)
			univ.out.out_e[i][j] = univ.out.out_e[i + 48][j];
	for (i = 48; i < 96; i++)
		for (j = 0; j < 96; j++)
			univ.out.out_e[i][j] = 0;
	
	
	for (i = 0; i < 10; i++) {
		if (univ.party.out_c[i].m_loc.x < 48)
			univ.party.out_c[i].exists = false;
		if (univ.party.out_c[i].exists == true)
			univ.party.out_c[i].m_loc.x -= 48;
	}
	load_outdoors(loc(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y),univ.out.outdoors[1][0]);
	load_outdoors(loc(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y + 1),univ.out.outdoors[1][1]);
	build_outdoors();
	restore_cursor();
	
}

void shift_universe_up()
{
	short i,j;
	
	make_cursor_watch();
	save_outdoor_maps();
	univ.party.outdoor_corner.y--;
	univ.party.i_w_c.y++;
	univ.party.p_loc.y += 48;
	univ.out.outdoors[0][1] = univ.out.outdoors[0][0];
	univ.out.outdoors[1][1] = univ.out.outdoors[1][0];
	
//	outdoor_text[0][1] = outdoor_text[0][0];
//	outdoor_text[1][1] = outdoor_text[1][0];
	for (i = 0; i < 96; i++)
		for (j = 48; j < 96; j++)
			univ.out.out_e[i][j] = univ.out.out_e[i][j - 48];
	for (i = 0; i < 96; i++)
		for (j = 0; j < 48; j++)
			univ.out.out_e[i][j] = 0;
	
	for (i = 0; i < 10; i++) {
		if (univ.party.out_c[i].m_loc.y > 48)
			univ.party.out_c[i].exists = false;
		if (univ.party.out_c[i].exists == true)
			univ.party.out_c[i].m_loc.y += 48;
	}
	load_outdoors(loc(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y),univ.out.outdoors[0][0]);
	load_outdoors(loc(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y),univ.out.outdoors[1][0]);
	
	build_outdoors();
	restore_cursor();
	
}

void shift_universe_down()
{
	short i,j;
	
	make_cursor_watch();
	
	save_outdoor_maps();
	univ.party.outdoor_corner.y++;
	univ.party.i_w_c.y--;
	univ.party.p_loc.y = univ.party.p_loc.y - 48;
	univ.out.outdoors[0][0] = univ.out.outdoors[0][1];
	univ.out.outdoors[1][0] = univ.out.outdoors[1][1];
	
//	outdoor_text[0][0] = outdoor_text[0][1];
//	outdoor_text[1][0] = outdoor_text[1][1];
	for (i = 0; i < 96; i++)
		for (j = 0; j < 48; j++)
			univ.out.out_e[i][j] = univ.out.out_e[i][j + 48];
	for (i = 0; i < 96; i++)
		for (j = 48; j < 96; j++)
			univ.out.out_e[i][j] = 0;
	
	for (i = 0; i < 10; i++) {
		if (univ.party.out_c[i].m_loc.y < 48)
			univ.party.out_c[i].exists = false;
		if (univ.party.out_c[i].exists == true)
			univ.party.out_c[i].m_loc.y = univ.party.out_c[i].m_loc.y - 48;
	}
	load_outdoors(loc(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y + 1),univ.out.outdoors[0][1]);
	load_outdoors(loc(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y + 1),univ.out.outdoors[1][1]);
	
	build_outdoors();
	restore_cursor();
	
}


void position_party(short out_x,short out_y,short pc_pos_x,short pc_pos_y) ////
{
	short i,j;
	
	if ((pc_pos_x != minmax(0,47,pc_pos_x)) || (pc_pos_y != minmax(0,47,pc_pos_y)) ||
		(out_x != minmax(0,scenario.out_width - 1,out_x)) || (out_y != minmax(0,scenario.out_height - 1,out_y))) {
		giveError("The scenario has tried to place you in an out of bounds outdoor location.");
		return;
	}
	
	save_outdoor_maps();
	univ.party.p_loc.x = pc_pos_x;
	univ.party.p_loc.y = pc_pos_y;
	univ.party.loc_in_sec = global_to_local(univ.party.p_loc);
	
	if ((univ.party.outdoor_corner.x != out_x) || (univ.party.outdoor_corner.y != out_y)) {
		univ.party.outdoor_corner.x = out_x;
		univ.party.outdoor_corner.y = out_y;
		load_outdoors(loc(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y + 1),univ.out.outdoors[1][1]);
		load_outdoors(loc(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y + 1),univ.out.outdoors[0][1]);
		load_outdoors(loc(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y),univ.out.outdoors[1][0]);
		load_outdoors(loc(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y),univ.out.outdoors[0][0]);
	}
	univ.party.i_w_c.x = (univ.party.p_loc.x > 47) ? 1 : 0;
	univ.party.i_w_c.y = (univ.party.p_loc.y > 47) ? 1 : 0;
	for (i = 0; i < 10; i++)
		univ.party.out_c[i].exists = false;
	for (i = 0; i < 96; i++)
		for (j = 0; j < 96; j++)
			univ.out.out_e[i][j] = 0;
	build_outdoors();
}


void build_outdoors()
{
	short i,j;
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			univ.out[i][j] = univ.out.outdoors[0][0].terrain[i][j];
			univ.out[48 + i][j] = univ.out.outdoors[1][0].terrain[i][j];
			univ.out[i][48 + j] = univ.out.outdoors[0][1].terrain[i][j];
			univ.out[48 + i][48 + j] = univ.out.outdoors[1][1].terrain[i][j];
		}
	
	fix_boats();
	add_outdoor_maps();
//	make_out_trim();
	// TODO: This might be another relic of the "demo" mode
	if(overall_mode != MODE_STARTUP)
		erase_out_specials();
	
	for (i = 0; i < 10; i++)
		if (univ.party.out_c[i].exists == true)
			if ((univ.party.out_c[i].m_loc.x < 0) || (univ.party.out_c[i].m_loc.y < 0) ||
				(univ.party.out_c[i].m_loc.x > 95) || (univ.party.out_c[i].m_loc.y > 95))
				univ.party.out_c[i].exists = false;
	
}

short onm(char x_sector,char y_sector)
{
	short i;
	
	i = y_sector * scenario.out_width + x_sector;
	return i;
}



// This adds the current outdoor map info to the saved outdoor map info
void save_outdoor_maps()
{
	short i,j;
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			if (univ.out.out_e[i][j] > 0)
				univ.out_maps[onm(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y)][i / 8][j] =
				univ.out_maps[onm(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y)][i / 8][j] |
				(char) (s_pow(2,i % 8));
			if (univ.party.outdoor_corner.x + 1 < scenario.out_width) {
				if (univ.out.out_e[i + 48][j] > 0)
					univ.out_maps[onm(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y)][i / 8][j] =
					univ.out_maps[onm(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y)][i / 8][j] |
					(char) (s_pow(2,i % 8));
			}
			if (univ.party.outdoor_corner.y + 1 < scenario.out_height) {
				if (univ.out.out_e[i][j + 48] > 0)
					univ.out_maps[onm(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y + 1)][i / 8][j] =
					univ.out_maps[onm(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y + 1)][i / 8][j] |
					(char) (s_pow(2,i % 8));
			}
			if ((univ.party.outdoor_corner.y + 1 < scenario.out_height) &&
				(univ.party.outdoor_corner.x + 1 < scenario.out_width)) {
				if (univ.out.out_e[i + 48][j + 48] > 0)
					univ.out_maps[onm(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y + 1)][i / 8][j] =
					univ.out_maps[onm(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y + 1)][i / 8][j] |
					(char) (s_pow(2,i % 8));
			}
		}
}

void add_outdoor_maps()  // This takes the existing outdoor map info and supplements it
// with the saved map info
{
	short i,j;
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			if ((univ.out.out_e[i][j] == 0) &&
				((univ.out_maps[onm(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y)][i / 8][j] &
				  (char) (s_pow(2,i % 8))) != 0))
			 	univ.out.out_e[i][j] = 1;
			if (univ.party.outdoor_corner.x + 1 < scenario.out_width) {
				if ((univ.out.out_e[i + 48][j] == 0) &&
					((univ.out_maps[onm(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y)][i / 8][j] &
					  (char) (s_pow(2,i % 8))) != 0))
				 	univ.out.out_e[i + 48][j] = 1;
			}
			if (univ.party.outdoor_corner.y + 1 < scenario.out_height) {
				if ((univ.out.out_e[i][j + 48] == 0) &&
					((univ.out_maps[onm(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y + 1)][i / 8][j] &
					  (char) (s_pow(2,i % 8))) != 0))
				 	univ.out.out_e[i][j + 48] = 1;
			}
			if ((univ.party.outdoor_corner.y + 1 < scenario.out_height) &&
				(univ.party.outdoor_corner.x + 1 < scenario.out_width)) {
				if ((univ.out.out_e[i + 48][j + 48] == 0) &&
					((univ.out_maps[onm(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y + 1)][i / 8][j] &
					  (char) (s_pow(2,i % 8))) != 0))
				 	univ.out.out_e[i + 48][j + 48] = 1;
			}
		}
}



void fix_boats()
{
	short i;
	
	for (i = 0; i < 30; i++)
		if ((univ.party.boats[i].exists == true) && (univ.party.boats[i].which_town == 200)) {
			if (univ.party.boats[i].sector.x == univ.party.outdoor_corner.x)
				univ.party.boats[i].loc.x = univ.party.boats[i].loc_in_sec.x;
			else if (univ.party.boats[i].sector.x == univ.party.outdoor_corner.x + 1)
				univ.party.boats[i].loc.x = univ.party.boats[i].loc_in_sec.x + 48;
			else univ.party.boats[i].loc.x = 500;
			if (univ.party.boats[i].sector.y == univ.party.outdoor_corner.y)
				univ.party.boats[i].loc.y = univ.party.boats[i].loc_in_sec.y;
			else if (univ.party.boats[i].sector.y == univ.party.outdoor_corner.y + 1)
				univ.party.boats[i].loc.y = univ.party.boats[i].loc_in_sec.y + 48;
			else univ.party.boats[i].loc.y = 500;
		}
	for (i = 0; i < 30; i++)
		if ((univ.party.horses[i].exists == true) && (univ.party.horses[i].which_town == 200)) {
			if (univ.party.horses[i].sector.x == univ.party.outdoor_corner.x)
				univ.party.horses[i].loc.x = univ.party.horses[i].loc_in_sec.x;
			else if (univ.party.horses[i].sector.x == univ.party.outdoor_corner.x + 1)
				univ.party.horses[i].loc.x = univ.party.horses[i].loc_in_sec.x + 48;
			else univ.party.horses[i].loc.x = 500;
			if (univ.party.horses[i].sector.y == univ.party.outdoor_corner.y)
				univ.party.horses[i].loc.y = univ.party.horses[i].loc_in_sec.y;
			else if (univ.party.horses[i].sector.y == univ.party.outdoor_corner.y + 1)
				univ.party.horses[i].loc.y = univ.party.horses[i].loc_in_sec.y + 48;
			else univ.party.horses[i].loc.y = 500;
		}
}


//void load_outdoors(short to_create_x, short to_create_y, short targ_x, short targ_y,
//	short mode,short extra,char *str)
////short	to_create_x, to_create_y; // actual sector being loaded
////short 	targ_x, targ_y; // 0 or 1
//// mode 0 - whole out, 1 - just string number extra
//{
//	short file_id;
//	short i,j;
//	long len;
//	char file_name[256];
//	short out_sec_num;
//	long len_to_jump = 0,store = 0;
//	OSErr error;
//
//	if ((to_create_x != minmax(0,scenario.out_width - 1,to_create_x)) ||
//		(to_create_y != minmax(0,scenario.out_height - 1,to_create_y))) { // not exist
//			for (i = 0; i < 48; i++)
//				for (j = 0; j < 48; j++)
//					univ.out.outdoors[targ_x][targ_y].terrain[i][j] = 5;
//			for (i = 0; i < 18; i++) {
//				univ.out.outdoors[targ_x][targ_y].special_id[i] = -1;
//				univ.out.outdoors[targ_x][targ_y].special_locs[i].x = 100;
//				}
//			return;
//			}
//
//	build_scen_file_name(file_name);
//
//	error = HOpen(start_volume,start_dir,file_name,3,&file_id);
//
//	out_sec_num = scenario.out_width * to_create_y + to_create_x;
//
//	len_to_jump = sizeof(scenario_data_type);
//	len_to_jump += sizeof(scen_item_data_type);
//	for (i = 0; i < 300; i++)
//		len_to_jump += (long) scenario.scen_str_len[i];
//	store = 0;
//	for (i = 0; i < out_sec_num; i++)
//		for (j = 0; j < 2; j++)
//			store += (long) (scenario.out_data_size[i][j]);
//	len_to_jump += store;
//
//	error = SetFPos (file_id, 1, len_to_jump);
//	if (error != 0) {FSClose(file_id);oops_error(32);}
//
//	len = sizeof(outdoor_record_type);
//	if (mode == 0) {
//		error = FSRead(file_id, &len, (char *) &univ.out.outdoors[targ_x][targ_y]);
//		port_out(&univ.out.outdoors[targ_x][targ_y]);
//		if (error != 0) {FSClose(file_id);oops_error(33);}
//		}
//		else error = FSRead(file_id, &len, (char *) &dummy_out);
//
//	if (mode == 0) {
//		for (i = 0; i < 9; i++) {
//			len = (long) (univ.out.outdoors[targ_x][targ_y].strlens[i]);
//			FSRead(file_id, &len, (char *) &(outdoor_text[targ_x][targ_y].out_strs[i]));
//			outdoor_text[targ_x][targ_y].out_strs[i][len] = 0;
//			}
//		}
//	if (mode == 1) {
//		for (i = 0; i < 120; i++) {
//			len = (long) (dummy_out.strlens[i]);
//			if (i == extra) {
//				FSRead(file_id, &len, (char *) str);
//				str[len] = 0;
//				}
//				else SetFPos (file_id, 3, len);
//			}
//
//		}
//
//	error = FSClose(file_id);
//	if (error != 0) {FSClose(file_id);oops_error(33);}
//}





void start_data_dump() {
	fs::path path = progDir/"Data Dump.txt";
	std::ofstream fout(path.c_str());
	
	fout << "Begin data dump:\n";
	fout << "  Overall mode  " << overall_mode << "\n";
	fout << "  Outdoor loc  " << univ.party.outdoor_corner.x << " " << univ.party.outdoor_corner.y;
	fout << "  Ploc " << univ.party.p_loc.x << " " << univ.party.p_loc.y << "\n";
	if ((is_town()) || (is_combat())) {
		fout << "  Town num " << univ.town.num << "  Town loc  " << univ.town.p_loc.x << " " << univ.town.p_loc.y << " \n";
		if (is_combat()) {
			fout << "  Combat type " << which_combat_type << " \n";
		}
		
		for (long i = 0; i < univ.town->max_monst(); i++) {
			fout << "  Monster " << i << "   Status " << univ.town.monst[i].active;
			fout << "  Loc " << univ.town.monst[i].cur_loc.x << " " << univ.town.monst[i].cur_loc.y;
			fout << "  Number " << univ.town.monst[i].number << "  Att " << univ.town.monst[i].attitude;
			fout << "  Tf " << univ.town.monst[i].time_flag << "\n";
		}
	}
}

// expecting party record to contain name of proper scenario to load
//bool load_scenario()
//{
//
//	short i,file_id;
//	bool file_ok = false;
//	OSErr error;
//	long len;
//	char file_name[256];
//
//	build_scen_file_name(file_name);
//
//	error = HOpen(start_volume,start_dir,file_name,3,&file_id);
//	if (error != 0) {
//		oops_error(10000 + error);
//		SysBeep(2);	return false;
//		}
//
//	len = (long) sizeof(scenario_data_type);
//	if ((error = FSRead(file_id, &len, (char *) &scenario)) != 0){
//		FSClose(file_id); oops_error(29); return false;
//		}
//	if ((scenario.flag1 == 10) && (scenario.flag2 == 20)
//	 && (scenario.flag3 == 30)
//	  && (scenario.flag4 == 40)) {
//	  	file_ok = true;
//	  	cur_scen_is_mac = true;
//	  	}
//	if ((scenario.flag1 == 20) && (scenario.flag2 == 40)
//	 && (scenario.flag3 == 60)
//	  && (scenario.flag4 == 80)) {
//	  	file_ok = true;
//	  	cur_scen_is_mac = false;
//	  	}
//	 if (file_ok == false) {
//		FSClose(file_id);
//		give_error("This is not a legitimate Blades of Exile scenario.","",0);
//		return false;
//	 	}
//	port_scenario();
//	len = sizeof(scen_item_data_type); // item data
//	if ((error = FSRead(file_id, &len, (char *) &(scen_item_list))) != 0){
//		FSClose(file_id); oops_error(30); return false;
//		}
//	port_item_list();
//	for (i = 0; i < 270; i++) {
//		len = (long) (scenario.scen_str_len[i]);
//		FSRead(file_id, &len, (char *) &(scen_strs[i]));
//		scen_strs[i][len] = 0;
//		}
//
//	FSClose(file_id);
//	load_spec_graphics();
//	set_up_ter_pics();
//	return true;
//}

//void set_up_ter_pics()
//{
//	short i;
//
//	set_terrain_blocked();
//	for (i = 0; i < 256; i++)
//		terrain_pic[i] = scenario.ter_types[i].picture;
//}
//void oops_error(short error)
//{
//	char error_str[256];
//
//		SysBeep(50);
//		SysBeep(50);
//		SysBeep(50);
//	sprintf((char *) error_str,"Giving the scenario editor more memory might also help. Be sure to back your scenario up often. Error number: %d.",error);
//	give_error("The program encountered an error while loading/saving/creating the scenario. To prevent future problems, the program will now terminate. Trying again may solve the problem.",(char *) error_str,0);
//	//ExitToShell();
//}

/*

typedef struct {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,default_ground;
} scen_header_type;
*/
void build_scen_headers()
{
	fs::path scenDir = progDir;
//	scenDir.erase(scenDir.find_last_of("/"));
	scenDir /= "Blades of Exile Scenarios";
	printf("%s\n%s\n",progDir.c_str(),scenDir.c_str());
	scen_headers.clear();
//	for (i = 0; i < 25; i++)
//		scen_headers[i].flag1 = 0;
//	FSOpenIterator(&folderRef,0,&files);
	fs::path folderRef(scenDir); // TODO: This variable is unnecessary
	fs::directory_iterator iter(folderRef);
	/**TESTING**/
//	UniChar x[] = {0x0024, 0x0041, 0x0020, 0x0075, 0x0073, 0x0065, 0x006c, 0x0065, 0x0073, 0x0073, 0x0020, 0x0066, 0x0069, 0x006c, 0x0065};
//	err = FSCreateFileUnicode (&ref, 15, x, NULL, NULL, NULL, NULL);
//	if(err != noErr)printf("Error creating file.");
	/**END TESTING**/
	// TODO: Double-check that kFSIterateFlat is identical to the behaviour of Boost's directory_iterator
#if 0
	err = FSOpenIterator(&folderRef, kFSIterateFlat, &iter);
	if(err != noErr){
		printf("Error opening iterator!\n");
		return;
	}
#endif
	//myCPB.dirInfo.ioCompletion = NULL;
	//myCPB.dirInfo.ioNamePtr = scen_name;
	//myCPB.dirInfo.ioVRefNum = start_volume;
	
//	HFSUniStr255 names[MAXSHEETS];
//	FSSpec locations[MAXSHEETS];
//	int numFound=0;
//	FSGetCatalogInfoBulk (files,(ItemCount)MAXSHEETS,(ItemCount*)&numFound,NULL,kFSCatInfoNone,NULL,NULL,locations, names)
	while(iter != fs::directory_iterator()) {
		fs::file_status stat = iter->status();
		if(stat.type() == fs::regular_file)
			load_scenario_header(iter->path());
		iter++;
	}
	if (scen_headers.size() == 0) { // no scens present
		// TODO: Should something be done here?
	}
}

// This is only called at startup, when bringing headers of active scenarios.
// This wipes out the scenario record, so be sure not to call it while in an active scenario.
bool load_scenario_header(fs::path file/*,short header_entry*/){
	bool file_ok = false;
	long len;
	bool mac_header = true;
	
	// TODO: Rewrite using ifstream, or maybe ifstream_buf
	FILE* file_id = fopen(file.c_str(), "rb");
	if (file_id == NULL) {
		return false;
	}
	scen_header_type curScen;
	len = (long) sizeof(scen_header_type);
	if (fread(&curScen, len, 1, file_id) < 1){
		fclose(file_id); return false;
	}
	if ((curScen.flag1 == 10) && (curScen.flag2 == 20)
		&& (curScen.flag3 == 30)
		&& (curScen.flag4 == 40)) {
	  	file_ok = true;
	  	mac_header = true;
	} else
		if ((curScen.flag1 == 20) && (curScen.flag2 == 40)
			&& (curScen.flag3 == 60)
			&& (curScen.flag4 == 80)) {
			file_ok = true;
			mac_header = false;
		}
	if (file_ok == false) {
		//scen_headers[header_entry].flag1 = 0;
		//scen_headers.pop_back();
		fclose(file_id);
		return false;
	}
	
	// So file is OK, so load in string data and close it.
//	SetFPos(file_id,1,0);
//	len = 41942;//(long) sizeof(scenario_data_type);
//	error = FSRead(file_id, &len, (char *) &scenario);
//	if (error != 0){
//		FSClose(file_id);
//		oops_error(29);
//		return false;
//	}
//	store = scenario.rating;
//	if (mac_header == false)
//		flip_short(&store);
//	curScen.default_ground = store;
//
//	len = 39200;//sizeof(scen_item_data_type);
//	if (SetFPos(file_id,3,len) != 0){
//		FSClose(file_id);
//		return false;
//	}
	
	fclose(file_id);
	load_scenario(file);
	
	scen_header_str_type scen_strs;
	scen_strs.name = scenario.scen_name;
	scen_strs.who1 = scenario.who_wrote[0];
	scen_strs.who2 = scenario.who_wrote[1];
	std::string curScenarioName(file.filename().string());
	scen_strs.file = curScenarioName;
	
	if(scen_strs.file == "valleydy.exs" ||
	   scen_strs.file == "stealth.exs" ||
	   scen_strs.file == "zakhazi.exs"/* ||
	   scen_strs.file == "busywork.exs" */)
		return false;
	
	scen_headers.push_back(curScen,scen_strs);
//	for (i = 0; i < 3; i++) {
//		store = (short) scenario.scen_str_len[i];
//		len = (long) (store);
//		FSRead(file_id, &len, (char *) load_str);
//		load_str[len] = 0;
//		if (i == 0)
//			load_str[29] = 0;
//		else load_str[59] = 0;
//		scen_strs[i] = (char*)load_str;
//	}
	
	//FSClose(file_id);
	
	return true;
}

//extern GWorldPtr spec_scen_g;
//void load_spec_graphics()
//{
//	short i,file_num;
//	char file_name[256];
//
//	if (spec_scen_g != NULL) {
//		DisposeGWorld(spec_scen_g);
//		spec_scen_g = NULL;
//		}
//	build_scen_file_name(file_name);
//	for (i = 0; i < 250; i++) {
//		if (file_name[i] == '.') {
//			file_name[i + 1] = 'm';
//			file_name[i + 2] = 'e';
//			file_name[i + 3] = 'g';
//			i = 250;
//			}
//		}
//	file_num = HOpenResFile(start_volume,start_dir,file_name,1);
//	if (file_num < 0)
//	{
//		for (i = 0; i < 250; i++) {
//			if (file_name[i] == '.') {
//				file_name[i + 1] = 'b';
//				file_name[i + 2] = 'm';
//				file_name[i + 3] = 'p';
//				i = 250;
//			}
//		}
//		spec_scen_g = load_bmp_from_file(file_name);
//		return;
//	}
//	spec_scen_g = load_pict(1);
//	CloseResFile(file_num);
//}


//short init_data(short flag)
//{
//	long k = 0;
//
//	k = (long) flag;
//	k = k * k;
//	jl = jl * jl + 84 + k;
//	k = k + 51;
//		jl = jl * 2 + 1234 + k;
//k = k % 3000;
//	jl = jl * 54;
//	jl = jl * 2 + 1234 + k;
//	k = k * 82;
//	k = k % 10000;
//	k = k + 10000;
//
//	return (short) k;
//}
//
//short town_s(short flag)
//{
//	long k = 0;
//
//	k = (long) flag;
//	k = k * k * k;
//	jl = jl * 54;
//	jl = jl * 2 + 1234 + k;
//	k = k + 51;
//	k = k % 3000;
//	jl = jl * 2 + 1234 + k;
//	k = k * scenario.num_towns;
//	k = k % 10000;
//	jl = jl * jl + 84 + k;
//	k = k + 10000;
//
//	return (short) k;
//}
//
//short out_s(short flag)
//{
//	long k = 0;
//
//	k = (long) flag;
//	k = k * k * k;
//	jl = jl * jl + 84 + k;
//	k = k + scenario.out_data_size[0][1];
//	k = k % 3000;
//	k = k * 4;
//	jl = jl * 2 + 1234 + k;
//	jl = jl * 54;
//	jl = jl * 2 + 1234 + k;
//	k = k % 10000;
//	k = k + 4;
//
//	return (short) k;
//}
//
//short str_size_1(short flag)
//{
//	long k = 0;
//
//	k = (long) flag;
//	k = k * k;
//	jl = jl * 2 + 1234 + k;
//	jl = jl * jl + 84 + k;
//	k = k + scenario.scen_str_len[0] + scenario.scen_str_len[1] + scenario.scen_str_len[2];
//	jl = jl * 2 + 1234 + k;
//	k = k % 3000;
//	jl = jl * 54;
//	jl = jl * jl + 84 + k;
//	k = k * 4;
//	k = k % 5000;
//	k = k - 9099;
//
//	return (short) k;
//}
//
//short str_size_2(short flag)
//{
//	long k = 0;
//
//	k = (long) flag;
//	jl = jl * jl + 84 + k;
//	k = k * k * k * k;
//	jl = jl * 54;
//	k = k + 80;
//	k = k % 3000;
//	jl = jl * 2 + 1234 + k;
//	k = k * scenario.out_width * scenario.out_height;
//	jl = jl * jl + 84 + k;
//	k = k % 3124;
//	k = k - 5426;
//
//	return (short) k;
//}
//
//short str_size_3(short flag)
//{
//	long k = 0;
//
//	k = (long) flag;
//	k = k * (scenario.town_data_size[0][0] +  scenario.town_data_size[0][1] +  scenario.town_data_size[0][2] +  scenario.town_data_size[0][3]);
//	k = k + 80;
//	jl = jl * jl + 84 + k;
//	k = k % 3000;
//	jl = jl * 2 + 1234 + k;
//	k = k * 45;
//	jl = jl * 54;
//	jl = jl * jl + 84 + k;
//	k = k % 887;
//	k = k + 9452;
//
//	return (short) k;
//}
//
//short get_buf_ptr(short flag)
//{
//	long k = 0;
//
//	k = (long) flag;
//	jl = jl * jl + 84 + k;
//	k = k * (scenario.out_width +  scenario.out_width +  scenario.out_height +  scenario.town_data_size[0][3]);
//	k = k + 80;
//	jl = jl * jl + 84 + k;
//	k = k % 2443;
//	jl = jl * 54;
//	k = k * 322;
//	jl = jl * 2 + 1234 + k;
//	k = k % 2542;
//	jl = jl * jl + 84 + k;
//	k = k + 234;
//
//	return (short) k;
//}
//
//bool check_p (short pword)
//{
//	if (scenario.flag_b != town_s(pword))
//		return false;
//	if (scenario.flag_c != out_s(pword))
//		return false;
//	if (scenario.flag_e != str_size_1(pword))
//		return false;
//	if (scenario.flag_f != str_size_2(pword))
//		return false;
//	if (scenario.flag_h != str_size_3(pword))
//		return false;
//	if (scenario.flag_d != init_data(pword))
//		return false;
//
//	return true;
//}

//void port_talk_nodes()
//{
//	short i;
//
//	if (cur_scen_is_mac != mac_is_intel)
//		return;
//	for (i = 0; i < 60; i++) {
//		flip_short(&talking.talk_nodes[i].personality);
//		flip_short(&talking.talk_nodes[i].type);
//		flip_short(&talking.talk_nodes[i].extras[0]);
//		flip_short(&talking.talk_nodes[i].extras[1]);
//		flip_short(&talking.talk_nodes[i].extras[2]);
//		flip_short(&talking.talk_nodes[i].extras[3]);
//		}
//}
//
//void port_town()
//{
//	short i;
//
//	if (cur_scen_is_mac != mac_is_intel)
//		return;
//	flip_short(&univ.town.town.town_chop_time);
//	flip_short(&univ.town.town.town_chop_key);
//	flip_short(&univ.town.town.lighting);
//	for (i =0 ; i < 4; i++)
//		flip_short(&univ.town.town.exit_specs[i]);
//	flip_rect(&univ.town.town.in_town_rect);
//	for (i =0 ; i < 64; i++) {
//		flip_short(&univ.town.town.preset_items[i].item_code);
//		flip_short(&univ.town.town.preset_items[i].ability);
//		}
//	for (i =0 ; i < 50; i++) {
//		flip_short(&univ.town.town.preset_fields[i].field_type);
//		}
//	flip_short(&univ.town.town.max_num_monst);
//	flip_short(&univ.town.town.spec_on_entry);
//	flip_short(&univ.town.town.spec_on_entry_if_dead);
//	for (i =0 ; i < 8; i++)
//		flip_short(&univ.town.town.timer_spec_times[i]);
//	for (i =0 ; i < 8; i++)
//		flip_short(&univ.town.town.timer_specs[i]);
//	flip_short(&univ.town.town.difficulty);
//	for (i =0 ; i < 100; i++)
//		flip_spec_node(&univ.town.town.specials[i]);
//
//}
//
//void port_t_d()
//{
//	short i;
//	if (cur_scen_is_mac != mac_is_intel)
//		return;
//
//	for (i =0 ; i < 16; i++)
//		flip_rect(&t_d.room_rect[i]);
//	for (i =0 ; i < 60; i++) {
//		flip_short(&t_d.creatures[i].spec1);
//		flip_short(&t_d.creatures[i].spec2);
//		flip_short(&t_d.creatures[i].monster_time);
//		flip_short(&t_d.creatures[i].personality);
//		flip_short(&t_d.creatures[i].special_on_kill);
//		flip_short(&t_d.creatures[i].facial_pic);
//
//		}
//}
//
//void port_scenario()
//{
//	short i,j;
//
//	if (cur_scen_is_mac != mac_is_intel)
//		return;
//	flip_short(&scenario.flag_a);
//	flip_short(&scenario.flag_b);
//	flip_short(&scenario.flag_c);
//	flip_short(&scenario.flag_d);
//	flip_short(&scenario.flag_e);
//	flip_short(&scenario.flag_f);
//	flip_short(&scenario.flag_g);
//	flip_short(&scenario.flag_h);
//	flip_short(&scenario.flag_i);
//	flip_short(&scenario.intro_mess_pic);
//	flip_short(&scenario.intro_mess_len);
//	flip_short(&scenario.which_town_start);
//	for (i = 0; i < 200; i++)
//		for (j = 0; j < 5; j++)
//			flip_short(&scenario.town_data_size[i][j]);
//	for (i = 0; i < 10; i++)
//		flip_short(&scenario.town_to_add_to[i]);
//	for (i = 0; i < 10; i++)
//		for (j = 0; j < 2; j++)
//			flip_short(&scenario.flag_to_add_to_town[i][j]);
//	for (i = 0; i < 100; i++)
//		for (j = 0; j < 2; j++)
//			flip_short(&scenario.out_data_size[i][j]);
//	for (i = 0; i < 3; i++)
//		flip_rect(&scenario.store_item_rects[i]);
//	for (i = 0; i < 3; i++)
//		flip_short(&scenario.store_item_towns[i]);
//	for (i = 0; i < 50; i++)
//		flip_short(&scenario.special_items[i]);
//	for (i = 0; i < 50; i++)
//		flip_short(&scenario.special_item_special[i]);
//	flip_short(&scenario.rating);
//	flip_short(&scenario.uses_custom_graphics);
//	for (i = 0; i < 256; i++) {
//		flip_short(&scenario.scen_monsters[i].health);
//		flip_short(&scenario.scen_monsters[i].m_health);
//		flip_short(&scenario.scen_monsters[i].max_mp);
//		flip_short(&scenario.scen_monsters[i].mp);
//		flip_short(&scenario.scen_monsters[i].a[1]);
//		flip_short(&scenario.scen_monsters[i].a[0]);
//		flip_short(&scenario.scen_monsters[i].a[2]);
//		flip_short(&scenario.scen_monsters[i].morale);
//		flip_short(&scenario.scen_monsters[i].m_morale);
//		flip_short(&scenario.scen_monsters[i].corpse_item);
//		flip_short(&scenario.scen_monsters[i].corpse_item_chance);
//		flip_short(&scenario.scen_monsters[i].picture_num);
//		}
//
//	for (i = 0; i < 256; i++) {
//		flip_short(&scenario.ter_types[i].picture);
//		}
//	for (i = 0; i < 30; i++) {
//		flip_short(&scenario.boats[i].which_town);
//		}
//	for (i = 0; i < 30; i++) {
//		flip_short(&scenario.horses[i].which_town);
//		}
//	for (i = 0; i < 20; i++)
//		flip_short(&scenario.scenario_timer_times[i]);
//	for (i = 0; i < 20; i++)
//		flip_short(&scenario.scenario_timer_specs[i]);
//	for (i = 0; i < 256; i++) {
//		flip_spec_node(&scenario.scen_specials[i]);
//		}
//	for (i = 0; i < 10; i++)  {
//		flip_short(&scenario.storage_shortcuts[i].ter_type);
//		flip_short(&scenario.storage_shortcuts[i].property);
//		for (j = 0; j < 10; j++)  {
//			flip_short(&scenario.storage_shortcuts[i].item_num[j]);
//			flip_short(&scenario.storage_shortcuts[i].item_odds[j]);
//			}
//		}
//	flip_short(&scenario.last_town_edited);
//}
//
//
//void port_item_list()
//{
//	short i;
//
//	if (cur_scen_is_mac != mac_is_intel)
//		return;
//
//	for (i = 0; i < 400; i++) {
//		flip_short(&(scenario.scen_items[i].variety));
//		flip_short(&(scenario.scen_items[i].item_level));
//		flip_short(&(scenario.scen_items[i].value));
//		}
//}
//
//void port_out(outdoor_record_type *out)
//{
//	short i;
//
//	if (cur_scen_is_mac != mac_is_intel)
//		return;
//
//	for (i = 0; i < 4; i++) {
//		flip_short(&(out->wandering[i].spec_on_meet));
//		flip_short(&(out->wandering[i].spec_on_win));
//		flip_short(&(out->wandering[i].spec_on_flee));
//		flip_short(&(out->wandering[i].cant_flee));
//		flip_short(&(out->wandering[i].end_spec1));
//		flip_short(&(out->wandering[i].end_spec2));
//		flip_short(&(out->special_enc[i].spec_on_meet));
//		flip_short(&(out->special_enc[i].spec_on_win));
//		flip_short(&(out->special_enc[i].spec_on_flee));
//		flip_short(&(out->special_enc[i].cant_flee));
//		flip_short(&(out->special_enc[i].end_spec1));
//		flip_short(&(out->special_enc[i].end_spec2));
//		}
//	for (i = 0; i < 8; i++)
//		flip_rect(&(out->info_rect[i]));
//	for (i = 0; i < 60; i++)
//		flip_spec_node(&(out->specials[i]));
//}
//
//void flip_spec_node(cSpecial *spec)
//{
//	flip_short(&(spec->type));
//	flip_short(&(spec->sd1));
//	flip_short(&(spec->sd2));
//	flip_short(&(spec->pic));
//	flip_short(&(spec->m1));
//	flip_short(&(spec->m2));
//	flip_short(&(spec->ex1a));
//	flip_short(&(spec->ex1b));
//	flip_short(&(spec->ex2a));
//	flip_short(&(spec->ex2b));
//	flip_short(&(spec->jumpto));
//}
//
//void flip_short(short *s)
//{
//	char store,*s1, *s2;
//
//	s1 = (char *) s;
//	s2 = s1 + 1;
//	store = *s1;
//	*s1 = *s2;
//	*s2 = store;
//
//}
//
//void flip_long(long *s)
//{
//	char store,*s1, *s2, *s3, *s4;
//
//	s1 = (char *) s;
//	s2 = s1 + 1;
//	s3 = s1 + 2;
//	s4 = s1 + 3;
//	store = *s1;
//	*s1 = *s4;
//	*s4 = store;
//	store = *s2;
//	*s2 = *s3;
//	*s3 = store;
//
//}
//void alter_rect(RECT *r)
//{
//	short a;
//
//	a = r->top;
//	r->top = r->left;
//	r->left = a;
//	a = r->bottom;
//	r->bottom = r->right;
//	r->right = a;
//}
//
//void flip_rect(RECT *s)
//{
//	flip_short(&(s->top));
//	flip_short(&(s->bottom));
//	flip_short(&(s->left));
//	flip_short(&(s->right));
//	alter_rect(s);
//}
