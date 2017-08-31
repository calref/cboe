
#include <iostream>
#include <fstream>

#include "boe.global.hpp"
#include "universe.hpp"
#include "boe.fileio.hpp"
#include "boe.text.hpp"
#include "boe.town.hpp"
#include "boe.items.hpp"
#include "boe.graphics.hpp"
#include "boe.locutils.hpp"
#include "boe.newgraph.hpp"
#include "boe.dlgutil.hpp"
#include "boe.infodlg.hpp"
#include "boe.graphutil.hpp"
#include "sounds.hpp"
#include "mathutil.hpp"
#include "strdlog.hpp"
#include "fileio.hpp"
#include "cursors.hpp"
#include <boost/filesystem.hpp>

#define	DONE_BUTTON_ITEM	1

extern eStatMode stat_screen_mode;
extern eGameMode overall_mode;
extern bool party_in_memory;
extern location center;
extern long register_flag;
extern sf::RenderWindow mainPtr;
extern bool map_visible;
extern sf::RenderWindow mini_map;
extern short which_combat_type;
extern short cur_town_talk_loaded;
extern cUniverse univ;
std::vector<scen_header_type> scen_headers;
extern bool mac_is_intel;

bool loaded_yet = false, got_nagged = false;
std::string last_load_file = "Blades of Exile Save";
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

cCustomGraphics spec_scen_g;

void finish_load_party(){
	bool town_restore = univ.party.town_num < 200;
	bool in_scen = univ.party.scen_name.length() > 0;
	
	party_in_memory = true;
	
	// now if not in scen, this is it.
	if(!in_scen) {
		if(overall_mode != MODE_STARTUP) {
			reload_startup();
			draw_startup(0);
		}
		overall_mode = MODE_STARTUP;
		return;
	}
	
	// Saved creatures may not have had their monster attributes saved
	// Make sure that they know what they are!
	// Cast to cMonster base class and assign, to avoid clobbering other attributes
	for(auto& pop : univ.party.creature_save) {
		for(size_t i = 0; i < pop.size(); i++) {
			int number = pop[i].number;
			cMonster& monst = pop[i];
			monst = univ.scenario.scen_monsters[number];
		}
	}
	for(size_t j = 0; j < univ.town.monst.size(); j++) {
		int number = univ.town.monst[j].number;
		cMonster& monst = univ.town.monst[j];
		monst = univ.scenario.scen_monsters[number];
	}
	
	// if at this point, startup must be over, so make this call to make sure we're ready,
	// graphics wise
	end_startup();
	
	overall_mode = town_restore ? MODE_TOWN : MODE_OUTDOORS;
	stat_screen_mode = MODE_INVEN;
	build_outdoors();
	erase_out_specials();
	
	if(!town_restore) {
		center = univ.party.out_loc;
	}
	else {
		for(int i = 0; i < univ.town.monst.size(); i++){
			univ.town.monst[i].targ_loc.x = 0;
			univ.town.monst[i].targ_loc.y = 0;
		}
		
		// Set up field booleans
		for(int j = 0; j < univ.town->max_dim; j++)
			for(int k = 0; k < univ.town->max_dim; k++) {
				if(univ.town.is_quickfire(j,k))
					univ.town.quickfire_present = true;
				if(univ.scenario.ter_types[univ.town->terrain(j,k)].special == eTerSpec::CONVEYOR)
					univ.town.belt_present = true;
			}
		center = univ.party.town_loc;
	}
	
	redraw_screen(REFRESH_ALL);
	univ.cur_pc = first_active_pc();
	loaded_yet = true;
	
	last_load_file = file_to_load.filename().string();
	store_file_reply = file_to_load;
	
	add_string_to_buf("Load: Game loaded.");
}

void shift_universe_left() {
	set_cursor(watch_curs);
	
	save_outdoor_maps();
	univ.party.outdoor_corner.x--;
	univ.party.i_w_c.x++;
	univ.party.out_loc.x += 48;
	
	for(short i = 48; i < 96; i++)
		for(short j = 0; j < 96; j++)
			univ.out.out_e[i][j] = univ.out.out_e[i - 48][j];
	
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 96; j++)
			univ.out.out_e[i][j] = 0;
	
	for(short i = 0; i < 10; i++) {
		if(univ.party.out_c[i].m_loc.x > 48)
			univ.party.out_c[i].exists = false;
		if(univ.party.out_c[i].exists)
			univ.party.out_c[i].m_loc.x += 48;
	}
	
	build_outdoors();
	restore_cursor();
	
}

void shift_universe_right() {
	set_cursor(watch_curs);
	save_outdoor_maps();
	univ.party.outdoor_corner.x++;
	univ.party.i_w_c.x--;
	univ.party.out_loc.x -= 48;
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 96; j++)
			univ.out.out_e[i][j] = univ.out.out_e[i + 48][j];
	for(short i = 48; i < 96; i++)
		for(short j = 0; j < 96; j++)
			univ.out.out_e[i][j] = 0;
	
	
	for(short i = 0; i < 10; i++) {
		if(univ.party.out_c[i].m_loc.x < 48)
			univ.party.out_c[i].exists = false;
		if(univ.party.out_c[i].exists)
			univ.party.out_c[i].m_loc.x -= 48;
	}
	build_outdoors();
	restore_cursor();
	
}

void shift_universe_up() {
	set_cursor(watch_curs);
	save_outdoor_maps();
	univ.party.outdoor_corner.y--;
	univ.party.i_w_c.y++;
	univ.party.out_loc.y += 48;
	
	for(short i = 0; i < 96; i++)
		for(short j = 48; j < 96; j++)
			univ.out.out_e[i][j] = univ.out.out_e[i][j - 48];
	for(short i = 0; i < 96; i++)
		for(short j = 0; j < 48; j++)
			univ.out.out_e[i][j] = 0;
	
	for(short i = 0; i < 10; i++) {
		if(univ.party.out_c[i].m_loc.y > 48)
			univ.party.out_c[i].exists = false;
		if(univ.party.out_c[i].exists)
			univ.party.out_c[i].m_loc.y += 48;
	}
	
	build_outdoors();
	restore_cursor();
	
}

void shift_universe_down() {
	set_cursor(watch_curs);
	
	save_outdoor_maps();
	univ.party.outdoor_corner.y++;
	univ.party.i_w_c.y--;
	univ.party.out_loc.y = univ.party.out_loc.y - 48;
	
	for(short i = 0; i < 96; i++)
		for(short j = 0; j < 48; j++)
			univ.out.out_e[i][j] = univ.out.out_e[i][j + 48];
	for(short i = 0; i < 96; i++)
		for(short j = 48; j < 96; j++)
			univ.out.out_e[i][j] = 0;
	
	for(short i = 0; i < 10; i++) {
		if(univ.party.out_c[i].m_loc.y < 48)
			univ.party.out_c[i].exists = false;
		if(univ.party.out_c[i].exists)
			univ.party.out_c[i].m_loc.y = univ.party.out_c[i].m_loc.y - 48;
	}
	
	build_outdoors();
	restore_cursor();
	
}


void position_party(short out_x,short out_y,short pc_pos_x,short pc_pos_y) {
	if((pc_pos_x != minmax(0,47,pc_pos_x)) || (pc_pos_y != minmax(0,47,pc_pos_y)) ||
		(out_x != minmax(0,univ.scenario.outdoors.width() - 1,out_x)) || (out_y != minmax(0,univ.scenario.outdoors.height() - 1,out_y))) {
		showError("The scenario has tried to place you in an out of bounds outdoor location.");
		return;
	}
	
	save_outdoor_maps();
	univ.party.out_loc.x = pc_pos_x;
	univ.party.out_loc.y = pc_pos_y;
	univ.party.loc_in_sec = global_to_local(univ.party.out_loc);
	
	if((univ.party.outdoor_corner.x != out_x) || (univ.party.outdoor_corner.y != out_y)) {
		univ.party.outdoor_corner.x = out_x;
		univ.party.outdoor_corner.y = out_y;
	}
	univ.party.i_w_c.x = (univ.party.out_loc.x > 47) ? 1 : 0;
	univ.party.i_w_c.y = (univ.party.out_loc.y > 47) ? 1 : 0;
	for(short i = 0; i < 10; i++)
		univ.party.out_c[i].exists = false;
	for(short i = 0; i < 96; i++)
		for(short j = 0; j < 96; j++)
			univ.out.out_e[i][j] = 0;
	build_outdoors();
}


void build_outdoors() {
	size_t x = univ.party.outdoor_corner.x, y = univ.party.outdoor_corner.y;
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 48; j++) {
			univ.out[i][j] = univ.scenario.outdoors[x][y]->terrain[i][j];
			if(x + 1 < univ.scenario.outdoors.width())
				univ.out[48 + i][j] = univ.scenario.outdoors[x+1][y]->terrain[i][j];
			if(y + 1 < univ.scenario.outdoors.height())
				univ.out[i][48 + j] = univ.scenario.outdoors[x][y+1]->terrain[i][j];
			if(x + 1 < univ.scenario.outdoors.width() && y + 1 < univ.scenario.outdoors.height())
				univ.out[48 + i][48 + j] = univ.scenario.outdoors[x+1][y+1]->terrain[i][j];
		}
	
	add_outdoor_maps();
//	make_out_trim();
	// TODO: This might be another relic of the "demo" mode
	if(overall_mode != MODE_STARTUP)
		erase_out_specials();
	
	for(short i = 0; i < 10; i++)
		if(univ.party.out_c[i].exists)
			if((univ.party.out_c[i].m_loc.x < 0) || (univ.party.out_c[i].m_loc.y < 0) ||
				(univ.party.out_c[i].m_loc.x > 95) || (univ.party.out_c[i].m_loc.y > 95))
				univ.party.out_c[i].exists = false;
	
}

// This adds the current outdoor map info to the saved outdoor map info
void save_outdoor_maps() {
	location corner = univ.party.outdoor_corner;
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 48; j++) {
			univ.scenario.outdoors[corner.x][corner.y]->maps[j][i] = univ.out.out_e[i][j];
			if(corner.x + 1 < univ.scenario.outdoors.width())
				univ.scenario.outdoors[corner.x + 1][corner.y]->maps[j][i] = univ.out.out_e[i + 48][j];
			if(corner.y + 1 < univ.scenario.outdoors.height())
				univ.scenario.outdoors[corner.x][corner.y + 1]->maps[j][i] = univ.out.out_e[i][j + 48];
			if(corner.y + 1 < univ.scenario.outdoors.height() && corner.x + 1 < univ.scenario.outdoors.width())
				univ.scenario.outdoors[corner.x + 1][corner.y + 1]->maps[j][i] = univ.out.out_e[i + 48][j + 48];
		}
}

// This takes the existing outdoor map info and supplements it with the saved map info
void add_outdoor_maps() {
	location corner = univ.party.outdoor_corner;
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 48; j++) {
			univ.out.out_e[i][j] = univ.scenario.outdoors[corner.x][corner.y]->maps[j][i];
			if(corner.x + 1 < univ.scenario.outdoors.width())
				univ.out.out_e[i + 48][j] = univ.scenario.outdoors[corner.x + 1][corner.y]->maps[j][i];
			if(corner.y + 1 < univ.scenario.outdoors.height())
				univ.out.out_e[i][j + 48] = univ.scenario.outdoors[corner.x][corner.y + 1]->maps[j][i];
			if(corner.y + 1 < univ.scenario.outdoors.height() && corner.x + 1 < univ.scenario.outdoors.width())
				univ.out.out_e[i + 48][j + 48] = univ.scenario.outdoors[corner.x + 1][corner.y + 1]->maps[j][i];
		}
}

void start_data_dump() {
	fs::path path = progDir/"Data Dump.txt";
	std::ofstream fout(path.string().c_str());
	
	fout << "Begin data dump:\n";
	fout << "  Overall mode  " << overall_mode << "\n";
	fout << "  Outdoor loc  " << univ.party.outdoor_corner.x << " " << univ.party.outdoor_corner.y;
	fout << "  Ploc " << univ.party.out_loc.x << " " << univ.party.out_loc.y << "\n";
	if((is_town()) || (is_combat())) {
		fout << "  Town num " << univ.party.town_num << "  Town loc  " << univ.party.town_loc.x << " " << univ.party.town_loc.y << " \n";
		if(is_combat()) {
			fout << "  Combat type " << which_combat_type << " \n";
		}
		
		for(long i = 0; i < univ.town.monst.size(); i++) {
			fout << "  Monster " << i << "   Status " << univ.town.monst[i].active;
			fout << "  Loc " << univ.town.monst[i].cur_loc.x << " " << univ.town.monst[i].cur_loc.y;
			fout << "  Number " << univ.town.monst[i].number << "  Att " << univ.town.monst[i].attitude;
			fout << "  Tf " << univ.town.monst[i].time_flag << "\n";
		}
	}
}

extern fs::path scenDir;
void build_scen_headers() {
	fs::create_directories(scenDir);
	std::cout << progDir << '\n' << scenDir << std::endl;
	scen_headers.clear();
	fs::recursive_directory_iterator iter(scenDir);
	set_cursor(watch_curs);
	
	while(iter != fs::recursive_directory_iterator()) {
		fs::file_status stat = iter->status();
		if(stat.type() == fs::regular_file)
			load_scenario_header(iter->path());
		iter++;
	}
	if(scen_headers.size() == 0) { // no scens present
		// TODO: Should something be done here?
	} else {
		std::sort(scen_headers.begin(), scen_headers.end(), [](scen_header_type hdr_a, scen_header_type hdr_b) -> bool {
			std::string a = hdr_a.name, b = hdr_b.name;
			std::transform(a.begin(), a.end(), a.begin(), tolower);
			std::transform(b.begin(), b.end(), b.begin(), tolower);
			if(a.substr(0,2) == "a ") a.erase(a.begin(), a.begin() + 2);
			else if(a.substr(0,4) == "the ") a.erase(a.begin(), a.begin() + 4);
			if(b.substr(0,2) == "a ") b.erase(b.begin(), b.begin() + 2);
			else if(b.substr(0,4) == "the ") b.erase(b.begin(), b.begin() + 4);
			return a < b;
		});
	}
}

// This is only called at startup, when bringing headers of active scenarios.
bool load_scenario_header(fs::path file/*,short header_entry*/){
	bool file_ok = false;
	
	std::string fname = file.filename().string();
	int dot = fname.find_first_of('.');
	if(dot == std::string::npos)
		return false; // If it has no file extension, it's not a valid scenario.
	std::string file_ext = fname.substr(dot);
	std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), tolower);
	if(file_ext == ".exs") {
		std::ifstream fin(file.string(), std::ios::binary);
		if(fin.fail()) return false;
		scenario_header_flags curScen;
		long len = (long) sizeof(scenario_header_flags);
		if(!fin.read((char*)&curScen, len)) return false;
		if(curScen.flag1 == 10 && curScen.flag2 == 20 && curScen.flag3 == 30 && curScen.flag4 == 40)
			file_ok = true; // Legacy Mac scenario
		else if(curScen.flag1 == 20 && curScen.flag2 == 40 && curScen.flag3 == 60 && curScen.flag4 == 80)
			file_ok = true; // Legacy Windows scenario
		else if(curScen.flag1 == 'O' && curScen.flag2 == 'B' && curScen.flag3 == 'O' && curScen.flag4 == 'E')
			file_ok = true; // Unpacked OBoE scenario
	} else if(file_ext == ".boes") {
		if(fs::is_directory(file)) {
			if(fs::exists(file/"header.exs"))
				return load_scenario_header(file/"header.exs");
		} else {
			unsigned char magic[2];
			std::ifstream fin(file.string(), std::ios::binary);
			if(fin.fail()) return false;
			if(!fin.read((char*)magic, 2)) return false;
			// Check for the gzip magic number
			if(magic[0] == 0x1f && magic[1] == 0x8b)
				file_ok = true;
		}
	}
	if(!file_ok) return false;
	
	// So file is (probably) OK, so load in string data and close it.
	cScenario temp_scenario;
	if(!load_scenario(file, temp_scenario, true))
		return false;
	
	scen_header_type scen_head;
	scen_head.name = temp_scenario.scen_name;
	scen_head.who1 = temp_scenario.who_wrote[0];
	scen_head.who2 = temp_scenario.who_wrote[1];
	scen_head.file = fname == "header.exs" ? file.parent_path().filename().string() : fname;
	scen_head.intro_pic = temp_scenario.intro_pic;
	scen_head.rating = temp_scenario.rating;
	scen_head.difficulty = temp_scenario.difficulty;
	std::copy(temp_scenario.format.ver, temp_scenario.format.ver + 3, scen_head.ver);
	std::copy(temp_scenario.format.prog_make_ver, temp_scenario.format.prog_make_ver + 3, scen_head.prog_make_ver);

	fname = fname.substr(0,dot);
	std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
	
	if(fname == "valleydy" || fname == "stealth" || fname == "zakhazi"/* || fname == "busywork" */)
		return false;
	
	scen_headers.push_back(scen_head);
	
	return true;
}
