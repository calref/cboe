/*
 *  fileio.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "fileio.h"

#include <fstream>
#include <boost/filesystem/operations.hpp>

#include "dlogutil.hpp"
#include "gzstream.h"

#include "classes.h"
#include "map_parse.hpp"
#include "graphtool.h"
#include "mathutil.h"

#include "porting.h"
#include "restypes.hpp"
#include "tarball.hpp"
#include "cursors.h"

bool cur_scen_is_mac =  true, mac_is_intel;
extern sf::Texture items_gworld,tiny_obj_gworld,fields_gworld,roads_gworld,boom_gworld,missiles_gworld;
extern sf::Texture dlogpics_gworld,monst_gworld[],terrain_gworld[],anim_gworld,talkfaces_gworld,pc_gworld;
extern sf::Texture status_gworld, vehicle_gworld, small_ter_gworld;
extern cCustomGraphics spec_scen_g;
fs::path progDir, tempDir;

void load_spec_graphics(fs::path scen_file);
// Load old scenarios (town talk is handled by the town loading function)
static bool load_scenario_v1(fs::path file_to_load, cScenario& scenario);
static bool load_outdoors_v1(fs::path scen_file, location which_out,cOutdoors& the_out, cScenario& scenario);
static bool load_town_v1(fs::path scen_file, short which_town, cTown& the_town, legacy::scenario_data_type& scenario);
// Load new scenarios
static bool load_outdoors(fs::path out_base, location which_out, cOutdoors& the_out);
static bool load_town(fs::path town_base, short which_town, cTown*& the_town);
static bool load_town_talk(fs::path town_base, short which_town, cSpeech& the_talk);
// Load saved games
static bool load_party_v1(fs::path file_to_load, cUniverse& univ, bool town_restore, bool in_scen, bool maps_there, bool must_port);
static bool load_party_v2(fs::path file_to_load, cUniverse& univ, bool town_restore, bool in_scen, bool maps_there);

// Cursors included here so that they needn't be unnecessarily duplicated in platform-specific files
cursor_type current_cursor = sword_curs;
cursor_type arrow_curs[3][3] = {
	{NW_curs, N_curs, NE_curs},
	{W_curs, wait_curs, E_curs},
	{SW_curs, S_curs, SE_curs},
};

#include <stdexcept>

void init_directories(const char* exec_path) {
	progDir = fs::canonical(exec_path);
#ifdef __APPLE__
	// Need to back up out of the application package
	// We're pointing at .app/Contents/MacOS/exec_name, so back out three steps
	progDir = progDir.parent_path().parent_path().parent_path();
#endif
	progDir = progDir.parent_path();
	if(progDir.filename() == "Scenario Editor") progDir = progDir.parent_path();
	std::cout << progDir << std::endl;
	// Initialize the resource manager paths
	ResMgr::pushPath<ImageRsrc>(progDir/"Scenario Editor"/"graphics.exd"/"mac");
	ResMgr::pushPath<CursorRsrc>(progDir/"Scenario Editor"/"graphics.exd"/"mac"/"cursors");
	ResMgr::pushPath<FontRsrc>(progDir/"data"/"fonts");
	ResMgr::pushPath<StringRsrc>(progDir/"data"/"strings");
	ResMgr::pushPath<SoundRsrc>(progDir/"Scenario Editor"/"sounds.exa");
	
	// We need a location for temporary files, primarily for loading and saving operations
	// The scenario editor may also use this location as "scratch space"
#if defined(_WIN32) || defined(_WIN64)
	tempDir = getenv("APPDATA");
	tempDir /= "Blades of Exile";
#else
	tempDir = getenv("HOME");
#ifdef __APPLE__
	tempDir /= "Library/Application Support/Blades of Exile";
#else
	tempDir /= ".oboe/blades";
#endif // __APPLE__
#endif // _Win32||_Win64
	tempDir /= "Temporary Files";
}

void check_for_intel() {
	union {uint16_t x; uint8_t c;} endian;
	endian.x = 1;
	mac_is_intel = endian.c;
}

bool load_scenario(fs::path file_to_load, cScenario& scenario) {
	scenario = cScenario();
	// TODO: Implement checking to determine whether it's old or new
	return load_scenario_v1(file_to_load, scenario);
}

bool load_scenario_v1(fs::path file_to_load, cScenario& scenario){
	short i,n;
	bool file_ok = false;
	long len;
	char temp_str[256];
	legacy::scenario_data_type *temp_scenario = new legacy::scenario_data_type;
	legacy::scen_item_data_type *item_data = new legacy::scen_item_data_type;
	// TODO: Convert this (and all the others in this file) to use C++ streams
	FILE* file_id = fopen(file_to_load.string().c_str(),"rb");
	if(file_id == NULL) {
		// TODO: The third parameter to oopsError is supposed to specify whether we're in the scenario editor or the game, but I don't think this code knows that.
		// TODO: Alternatively, nuke oopsError and just use giveError. It's more informative, anyway.
		oopsError(10, 0, 0);
		return false;
	}
	
	len = (long) sizeof(scenario_header_flags);
	
	if(fread(&scenario.format, len, 1, file_id) < 1){
		fclose(file_id);
		oopsError(11, 0, 0);
		return false;
	}
	
	if((scenario.format.flag1 == 10) && (scenario.format.flag2 == 20) &&
		(scenario.format.flag3 == 30) && (scenario.format.flag4 == 40)) {
	  	cur_scen_is_mac = true;
	  	file_ok = true;
	}
	else if((scenario.format.flag1 == 20) && (scenario.format.flag2 == 40) &&
		(scenario.format.flag3 == 60) && (scenario.format.flag4 == 80)) {
	  	cur_scen_is_mac = false;
	  	file_ok = true;
	}
	if(!file_ok) {
		fclose(file_id);
		giveError("This is not a legitimate Blades of Exile scenario.");
		return false;
	}
	
	len = (long) sizeof(legacy::scenario_data_type);
	n = fread(temp_scenario, len, 1, file_id);
	if(n < 1){
		fclose(file_id);
		oopsError(12, 0, 0);
		return false;
	}
	port_scenario(temp_scenario);
	len = sizeof(legacy::scen_item_data_type); // item data
	n = fread(item_data, len, 1, file_id);
	if(n < 1){
		fclose(file_id);
		oopsError(13, 0, 0);
		return false;
	}
	port_item_list(item_data);
	scenario.append(*temp_scenario);
	scenario.append(*item_data);
	
		// TODO: Consider skipping the fread and assignment when len is 0
		for(i = 0; i < 270; i++) {
			len = (long) (temp_scenario->scen_str_len[i]);
			n = fread(temp_str, len, 1, file_id);
			temp_str[len] = 0;
			if(i == 0) scenario.scen_name = temp_str;
			else if(i == 1 || i == 2)
				scenario.who_wrote[i-1] = temp_str;
			else if(i == 3)
				scenario.contact_info = temp_str;
			else if(i >= 4 && i < 10)
				scenario.intro_strs[i-4] = temp_str;
			else if(i >= 10 && i < 60)
				scenario.journal_strs[i-10] = temp_str;
			else if(i >= 60 && i < 160) {
				if(i % 2 == 0) scenario.special_items[(i-60)/2].name = temp_str;
				else scenario.special_items[(i-60)/2].descr = temp_str;
			} else if(i >= 260) continue; // These were never ever used, for some reason.
			else scenario.spec_strs[i-160] = temp_str;
		}
	
	fclose(file_id);
	
	scenario.ter_types[23].fly_over = false;
	
	scenario.scen_file = file_to_load;
	load_spec_graphics(scenario.scen_file);
	
	// Now load all the outdoor sectors
	scenario.outdoors.resize(scenario.out_width, scenario.out_height);
	for(int x = 0; x < scenario.out_width; x++) {
		for(int y = 0; y < scenario.out_height; y++) {
			scenario.outdoors[x][y] = new cOutdoors(scenario);
			load_outdoors_v1(scenario.scen_file, loc(x,y), *scenario.outdoors[x][y], scenario);
		}
	}
	
	// Then load all the towns
	scenario.towns.resize(scenario.num_towns);
	for(int i = 0; i < scenario.num_towns; i++) {
		switch(scenario.town_size[i]) {
			case 0: scenario.towns[i] = new cBigTown(scenario); break;
			case 1: scenario.towns[i] = new cMedTown(scenario); break;
			case 2: scenario.towns[i] = new cTinyTown(scenario); break;
		}
		load_town_v1(scenario.scen_file, i, *scenario.towns[i], *temp_scenario);
	}
	
	delete temp_scenario;
	delete item_data;
	return true;
}

static long get_town_offset(short which_town, legacy::scenario_data_type& scenario){
	int i,j;
	long len_to_jump,store;
	
	len_to_jump = sizeof(scenario_header_flags);
	len_to_jump += sizeof(legacy::scenario_data_type);
	len_to_jump += sizeof(legacy::scen_item_data_type);
	for(i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for(i = 0; i < 100; i++)
		for(j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	for(i = 0; i < which_town; i++)
		for(j = 0; j < 5; j++)
			store += (long) (scenario.town_data_size[i][j]);
	len_to_jump += store;
	
	return len_to_jump;
}

bool load_town_v1(fs::path scen_file, short which_town, cTown& the_town, legacy::scenario_data_type& scenario) {
	short i,n;
	long len,len_to_jump = 0;
	char temp_str[256];
	legacy::town_record_type store_town;
	legacy::talking_record_type store_talk;
	legacy::big_tr_type t_d;
	legacy::ave_tr_type ave_t;
	legacy::tiny_tr_type tiny_t;
	
	FILE* file_id = fopen(scen_file.string().c_str(), "rb");
	if(file_id == NULL) {
		oopsError(14, 0, 0);
		return false;
	}
	
	len_to_jump = get_town_offset(which_town, scenario);
	n = fseek(file_id, len_to_jump, SEEK_SET);
	if(n != 0) {
		fclose(file_id);
		oopsError(15, 0, 0);
		return false;
	}
	
	len = sizeof(legacy::town_record_type);
	n = fread(&store_town, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		oopsError(16, 0, 0);
		return false;
	}
	port_town(&store_town);
	
	switch(scenario.town_size[which_town]) {
		case 0:
			len = sizeof(legacy::big_tr_type);
			n = fread(&t_d, len, 1, file_id);
			port_t_d(&t_d);
			the_town.append(store_town);
			the_town.append(t_d, which_town);
			break;
			
		case 1:
			len = sizeof(legacy::ave_tr_type);
			n = fread(&ave_t, len, 1, file_id);
			port_ave_t(&ave_t);
			the_town.append(store_town);
			the_town.append(ave_t, which_town);
			break;
			
		case 2:
			len = sizeof(legacy::tiny_tr_type);
			n = fread(&tiny_t, len, 1, file_id);
			port_tiny_t(&tiny_t);
			the_town.append(store_town);
			the_town.append(tiny_t, which_town);
			break;
	}
	
	for(i = 0; i < 140; i++) {
		len = (long) (store_town.strlens[i]);
		n = fread(temp_str, len, 1, file_id);
		temp_str[len] = 0;
		if(i == 0) the_town.town_name = temp_str;
		else if(i >= 1 && i < 17)
			the_town.rect_names[i-1] = temp_str;
		else if(i >= 17 && i < 20)
			the_town.comment[i-17] = temp_str;
		else if(i >= 20 && i < 120)
			the_town.spec_strs[i-20] = temp_str;
		else if(i >= 120 && i < 140)
			the_town.sign_strs[i-120] = temp_str;
	}
	
	len = sizeof(legacy::talking_record_type);
	n = fread(&store_talk, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		oopsError(17, 0, 0);
		return false;
	}
	port_talk_nodes(&store_talk);
	the_town.talking.append(store_talk);
	
	for(i = 0; i < 170; i++) {
		len = (long) (the_town.talking.strlens[i]);
		n = fread(temp_str, len, 1, file_id);
		temp_str[len] = 0;
		if(i >= 0 && i < 10)
			the_town.talking.people[i].title = temp_str;
		else if(i >= 10 && i < 20)
			the_town.talking.people[i-10].look = temp_str;
		else if(i >= 20 && i < 30)
			the_town.talking.people[i-20].name = temp_str;
		else if(i >= 30 && i < 40)
			the_town.talking.people[i-30].job = temp_str;
		else if(i >= 160)
			the_town.talking.people[i-160].dunno = temp_str;
		else {
			if(i % 2 == 0)
				the_town.talking.talk_nodes[(i-40)/2].str1 = temp_str;
			else the_town.talking.talk_nodes[(i-40)/2].str2 = temp_str;
		}
	}
	
	n = fclose(file_id);
	if(n != 0) {
		oopsError(18, 0, 0);
	}
	
	return true;
}

bool load_town(fs::path town_base, short which_town, cTown*& the_town) {
	// TODO: This stuff goes in load_scenario() now
//	fs::path town_base = scenario.scen_file/"towns";
	std::string base_fname = "t" + std::to_string(which_town), fname;
	// TODO: Implement all this.
	// First load the main town data.
	fname = base_fname + ".xml";
	// Next, load in the town map.
	fname = base_fname + ".map";
	map_data map = load_map(town_base/fname);
	// Then load the town's special nodes.
	fname = base_fname + ".spec";
	// Load the town's special encounter strings
	fname = base_fname + ".txt";
	// And finally, load the town's dialogue nodes.
	load_town_talk(town_base, which_town, the_town->talking);
	return false;
}

bool load_town_talk(fs::path town_base, short which_town, cSpeech& the_talk) {
	// TODO: Implement this.
	std::string fname = "t" + std::to_string(which_town) + "talk.xml";
	return false;
}

static long get_outdoors_offset(location& which_out, cScenario& scenario){
	int i,j,out_sec_num;
	long len_to_jump,store;
	out_sec_num = scenario.out_width * which_out.y + which_out.x;
	
	len_to_jump = sizeof(scenario_header_flags);
	len_to_jump += sizeof(legacy::scenario_data_type);
	len_to_jump += sizeof(legacy::scen_item_data_type);
	for(i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for(i = 0; i < out_sec_num; i++)
		for(j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	len_to_jump += store;
	
	return len_to_jump;
}

//mode -> 0 - primary load  1 - add to top  2 - right  3 - bottom  4 - left
bool load_outdoors_v1(fs::path scen_file, location which_out,cOutdoors& the_out, cScenario& scenario){
	short i,n;
	long len,len_to_jump;
	char temp_str[256];
	legacy::outdoor_record_type store_out;
	
	FILE* file_id = fopen(scen_file.string().c_str(), "rb");
	if(file_id == NULL) {
		oopsError(32, 0, 0);
		return false;
	}
	
	len_to_jump = get_outdoors_offset(which_out, scenario);
	n = fseek(file_id, len_to_jump, SEEK_SET);
	if(n != 0) {
		fclose(file_id);
		oopsError(33, 0, 0);
		return false;
	}
	
	len = sizeof(legacy::outdoor_record_type);
	n = fread(&store_out, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		oopsError(34, 0, 0);
		return false;
	}
	
	the_out.x = which_out.x;
	the_out.y = which_out.y;
	port_out(&store_out);
	the_out.append(store_out, scenario);
	for(i = 0; i < 108; i++) {
		len = (long) (store_out.strlens[i]);
		n = fread(temp_str, len, 1, file_id);
		temp_str[len] = 0;
		if(i == 0) the_out.out_name = temp_str;
		else if(i == 9) the_out.comment = temp_str;
		else if(i < 9) the_out.rect_names[i-1] = temp_str;
		else if(i >= 10 && i < 100)
			the_out.spec_strs[i-10] = temp_str;
		else if(i >= 100 && i < 108)
			the_out.sign_strs[i-100] = temp_str;
	}
	
	n = fclose(file_id);
	if(n != 0) {
		oopsError(35, 0, 0);
	}
	return true;
}

bool load_outdoors(fs::path out_base, location which_out,cOutdoors& the_out) {
	// TODO: This bit goes in load_scenario() now
//	fs::path town_base = scenario.scen_file/"outdoors";
	std::string base_fname = "tut" + std::to_string(which_out.x) + "~" + std::to_string(which_out.y), fname;
	// TODO: Implement all this.
	// First load the main sector data.
	fname = base_fname + ".xml";
	// Next, load in the sector map.
	fname = base_fname + ".map";
	map_data map = load_map(out_base/fname);
	// Then load the sector's special nodes.
	fname = base_fname + ".spec";
	// Load the sector's special encounter strings
	fname = base_fname + ".txt";
	return false;
}

#ifdef __APPLE__
bool tryLoadPictFromResourceFile(fs::path& gpath, sf::Image& graphics_store);
#endif

void load_spec_graphics(fs::path scen_file) {
	static const char*const noGraphics = "The game will still work without the custom graphics, but some things will not look right.";
	short i;
	fs::path path(scen_file);
	printf("Loading scenario graphics... (%s)\n",path.c_str());
	// Tried path.replace_extension, but that only deleted the extension, so I have to do it manually
	std::string filename = path.stem().string();
	path = path.parent_path();
	if(spec_scen_g) spec_scen_g.clear();
	// TODO: Load new-style sheets
	{
		static sf::Image graphics_store;
		bool foundGraphics = false;
#ifdef __APPLE__
		fs::path gpath = path/(filename + ".meg");
		if(fs::exists(gpath))
			foundGraphics = tryLoadPictFromResourceFile(gpath, graphics_store);
#endif
		if(!foundGraphics) {
			fs::path gpath = path/(filename + ".bmp");
			if(fs::exists(gpath)) {
				if(graphics_store.loadFromFile(gpath.string()))
					foundGraphics = true;
				else giveError("An old-style .bmp graphics file was found, but there was an error reading from the file.",noGraphics);
			}
		}
		if(foundGraphics) {
			// If we're here, we found old-style graphics.
			// This means they need an alpha channel
			graphics_store.createMaskFromColor(sf::Color::White);
			spec_scen_g.is_old = true;
			spec_scen_g.sheets = new sf::Texture[1];
			spec_scen_g.sheets[0].loadFromImage(graphics_store);
		} else {
			// Check for new-style graphics
		}
	}//else{}
	
	// TODO: This should really reload ALL textures...
	// Now load regular graphics
	items_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("objects"));
	tiny_obj_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("tinyobj"));
	fields_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("fields"));
	roads_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("trim"));
	boom_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("booms"));
	missiles_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("missiles"));
	dlogpics_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("dlogpics"));
	status_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("staticons"));
	
	for(i = 0; i < NUM_MONST_SHEETS; i++){
		std::ostringstream sout;
		sout << "monst" << i + 1;
		monst_gworld[i].loadFromImage(*ResMgr::get<ImageRsrc>(sout.str()));
	}
	for(i = 0; i < NUM_TER_SHEETS; i++){
		std::ostringstream sout;
		sout << "ter" << i + 1;
		terrain_gworld[i].loadFromImage(*ResMgr::get<ImageRsrc>(sout.str()));
	}
	anim_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("teranim"));
	talkfaces_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("talkportraits"));
	pc_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcs"));
	vehicle_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("vehicle"));
	small_ter_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("termap"));
	// TODO: Scenario icons ...
}

bool load_party(fs::path file_to_load, cUniverse& univ){
	bool town_restore = false;
	bool maps_there = false;
	bool in_scen = false;
	enum {old_mac, old_win, new_oboe, unknown} format;
	typedef unsigned short ushort;
	
	long len;
	short vers,n;
	struct {ushort a; ushort b; ushort c; ushort d; ushort e;} flags;
	
	// TODO: Putting these flags in hex would make some things a bit clearer
	static const unsigned short mac_flags[3][2] = {
		{5790,1342}, // slot 0 ... 5790 - out,         1342 - town
		{100,200},   // slot 1 ... 100  - in scenario, 200  - not in
		{3422,5567}  // slot 2 ... 3422 - no maps,     5567 - maps
	};
	static const unsigned short win_flags[3][2] = {
		{40470,15877}, // slot 0 ... 40470 - out,         15877 - town
		{25600,51200}, // slot 1 ... 25600 - in scenario, 51200 - not in
		{24077,48917}  // slot 2 ... 24077 - no maps,     48917 - maps
	};
	// but if the first flag is 0x0B0E, we have a new-format save
	// the three flags still follow that.
	FILE* file_id = fopen(file_to_load.string().c_str(), "rb");
	if(file_id == NULL) {
		cChoiceDlog("load-game-fail").show();
		return false;
	}
	
	len = sizeof(flags); // 10
	
	n = fread(&flags, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		cChoiceDlog("not-save-game").show();
		return false;
	}
	
	if(mac_is_intel && flags.a == 0x0B0E){ // new format
		format = new_oboe;
		if(flags.b == mac_flags[0][1]) town_restore = true;
		else if(flags.b != mac_flags[0][0]) format = unknown;
		if(flags.c == mac_flags[1][0]) in_scen = true;
		else if(flags.c != mac_flags[1][1]) format = unknown;
		if(flags.d == mac_flags[2][1]) maps_there = true;
		else if(flags.d != mac_flags[2][0]) format = unknown;
		vers = flags.e;
	}else if(!mac_is_intel && flags.a == 0x0E0B){ // new format
		format = new_oboe;
		flip_short((short*)&flags.b);
		flip_short((short*)&flags.c);
		flip_short((short*)&flags.d);
		flip_short((short*)&flags.e);
		if(flags.b == mac_flags[0][1]) town_restore = true;
		else if(flags.b != mac_flags[0][0]) format = unknown;
		if(flags.c == mac_flags[1][0]) in_scen = true;
		else if(flags.c != mac_flags[1][1]) format = unknown;
		if(flags.d == mac_flags[2][1]) maps_there = true;
		else if(flags.d != mac_flags[2][0]) format = unknown;
		vers = flags.e;
	}else if(flags.a == mac_flags[0][0] || flags.a == mac_flags[0][1]){ // old format
		if(mac_is_intel){ // it's actually a windows save
			flip_short((short*)&flags.a);
			flip_short((short*)&flags.b);
			flip_short((short*)&flags.c);
			format = old_win;
			if(flags.a == win_flags[0][1]) town_restore = true;
			else if(flags.a != win_flags[0][0]) format = unknown;
			if(flags.b == win_flags[1][0]) in_scen = true;
			else if(flags.b != win_flags[1][1]) format = unknown;
			if(flags.c == win_flags[2][1]) maps_there = true;
			else if(flags.c != win_flags[2][0]) format = unknown;
		}else{ // mac save
			format = old_mac;
			if(flags.a == mac_flags[0][1]) town_restore = true;
			else if(flags.a != mac_flags[0][0]) format = unknown;
			if(flags.b == mac_flags[1][0]) in_scen = true;
			else if(flags.b != mac_flags[1][1]) format = unknown;
			if(flags.c == mac_flags[2][1]) maps_there = true;
			else if(flags.c != mac_flags[2][0]) format = unknown;
		}
	}else if(flags.a == win_flags[0][0] || flags.a == win_flags[0][1]){ // old format
		if(mac_is_intel){ // it's actually a macintosh save
			flip_short((short*)&flags.a);
			flip_short((short*)&flags.b);
			flip_short((short*)&flags.c);
			format = old_mac;
			if(flags.a == mac_flags[0][1]) town_restore = true;
			else if(flags.a != mac_flags[0][0]) format = unknown;
			if(flags.b == mac_flags[1][0]) in_scen = true;
			else if(flags.b != mac_flags[1][1]) format = unknown;
			if(flags.c == mac_flags[2][1]) maps_there = true;
			else if(flags.c != mac_flags[2][0]) format = unknown;
		}else{ // win save
			format = old_win;
			if(flags.a == win_flags[0][1]) town_restore = true;
			else if(flags.a != win_flags[0][0]) format = unknown;
			if(flags.b == win_flags[1][0]) in_scen = true;
			else if(flags.b != win_flags[1][1]) format = unknown;
			if(flags.c == win_flags[2][1]) maps_there = true;
			else if(flags.c != win_flags[2][0]) format = unknown;
		}
	}else format = unknown;
	
	fclose(file_id);
	switch(format){
		case old_mac:
			return load_party_v1(file_to_load, univ, town_restore, in_scen, maps_there, mac_is_intel);
		case old_win:
			return load_party_v1(file_to_load, univ, town_restore, in_scen, maps_there, !mac_is_intel);
		case new_oboe:
			return load_party_v2(file_to_load, univ, town_restore, in_scen, maps_there);
		case unknown:
			cChoiceDlog("not-save-game").show();
			return false;
	}
	
	return true;
}

bool load_party_v1(fs::path file_to_load, cUniverse& univ, bool town_restore, bool in_scen, bool maps_there, bool must_port){
	std::ifstream fin(file_to_load.c_str(), std::ios_base::binary);
	fin.seekg(3*sizeof(short),std::ios_base::beg); // skip the header, which is 6 bytes in the old format
	
	legacy::party_record_type store_party;
	legacy::setup_save_type store_setup;
	legacy::pc_record_type store_pc[6];
	legacy::out_info_type store_out_info;
	legacy::current_town_type store_c_town;
	legacy::big_tr_type t_d;
	legacy::town_item_list t_i;
	legacy::stored_items_list_type stored_items[3];
	legacy::stored_town_maps_type town_maps;
	legacy::stored_outdoor_maps_type o_maps;
	unsigned char misc_i[64][64], sfx[64][64];
	char *party_ptr;
	char *pc_ptr;
	long len,store_len,count;
	
	// LOAD PARTY	
	len = (long) sizeof(legacy::party_record_type); // should be 46398
	store_len = len;
	fin.read((char*)&store_party, len);
	if(must_port) port_party(&store_party);
	party_ptr = (char*) &store_party;
	for(count = 0; count < store_len; count++)
		party_ptr[count] ^= 0x5C;	
	
	// LOAD SETUP
	len = (long) sizeof(legacy::setup_save_type);
	fin.read((char*)&store_setup, len);
	
	// LOAD PCS
	store_len = (long) sizeof(legacy::pc_record_type);
	for(int i = 0; i < 6; i++) {
		len = store_len;
		fin.read((char*)&store_pc[i], len);
		if(must_port) port_pc(&store_pc[i]);
		pc_ptr = (char*) &store_pc[i];
		for(count = 0; count < store_len; count++)
			pc_ptr[count] ^= 0x6B;	
	}
	
	if(in_scen) {
		
		// LOAD OUTDOOR MAP
		len = (long) sizeof(legacy::out_info_type);
		fin.read((char*)&store_out_info, len);
		
		// LOAD TOWN 
		if(town_restore) {
			len = (long) sizeof(legacy::current_town_type);
			fin.read((char*)&store_c_town, len);
			if(must_port) port_c_town(&store_c_town);
			
			len = (long) sizeof(legacy::big_tr_type);
			fin.read((char*)&t_d, len);
			if(must_port) port_t_d(&t_d);
			
			len = (long) sizeof(legacy::town_item_list);
			fin.read((char*)&t_i, len);			
		}else univ.town.num = 200;
		
		// LOAD STORED ITEMS
		for(int i = 0; i < 3; i++) {
			len = (long) sizeof(legacy::stored_items_list_type);
			fin.read((char*)&stored_items[i], len);
		}
		
		// LOAD SAVED MAPS
		if(maps_there) {
			len = (long) sizeof(legacy::stored_town_maps_type);
			fin.read((char*)&town_maps, len);
			
			len = (long) sizeof(legacy::stored_outdoor_maps_type);
			fin.read((char*)&o_maps, len);
		}
		
		// LOAD SFX & MISC_I
		len = (long) (64 * 64);
		fin.read((char*)sfx, len);
		
		fin.read((char*)misc_i, len);	
		
	} // end if_scen
	
	fin.close();
	
	if(in_scen){
		fs::path path;
		path = progDir/"Blades of Exile Scenarios"/univ.party.scen_name;
		
		if(!load_scenario(path, univ.scenario))
			return false;
		univ.file = path;
	}else{
		univ.party.scen_name = "";
	}
	
	univ.party.append(store_party);
	univ.party.append(store_setup);
	univ.party.void_pcs();
	for(int i = 0; i < 6; i++)
		univ.party.add_pc(store_pc[i]);
	if(in_scen){
		univ.out.append(store_out_info);
		if(town_restore){
			univ.town.append(store_c_town);
			univ.town.append(t_d);
			univ.town.append(t_i);
		}
		for(int i = 0; i < 3; i++)
			univ.party.append(stored_items[i],i);
		univ.append(town_maps);
		univ.append(o_maps);
		univ.town.append(sfx, misc_i);
	}
	
	// Compatibility flags
	// TODO: Pretty sure I did this elsewhere, so probably don't need it here
	if(in_scen && univ.scenario.format.prog_make_ver[0] < 2){
		univ.party.stuff_done[305][8] = 1;
	} else {
		univ.party.stuff_done[305][8] = 0;
	}
	
	return true;
}

bool load_party_v2(fs::path file_to_load, cUniverse& univ, bool town_restore, bool in_scen, bool maps_there){
	if(!fs::exists(tempDir)) fs::create_directories(tempDir);
	fs::path tempPath = tempDir/"loadtemp.exg";
	
	{ // First, strip off the header and save to a temporary location.
		std::ifstream fin(file_to_load.c_str(), std::ios_base::binary);
		std::ofstream fout(tempPath.c_str(), std::ios_base::binary);
		fin.seekg(10);
		fout << fin.rdbuf();
		fin.close();
		fout.close();
	}
	
	igzstream zin(tempPath.string().c_str());
	tarball partyIn;
	partyIn.readFrom(zin);
	zin.close();
	
	{ // Load main party data first
		std::istream& fin = partyIn.getFile("save/party.txt");
		if(!fin) {
			cChoiceDlog("load-game-fail").show();
			return false;
		}
		univ.party.readFrom(fin);
	}
	
	{ // Then the "setup" array
		std::istream& fin = partyIn.getFile("save/setup.dat");
		if(!fin) {
			cChoiceDlog("load-game-fail").show();
			return false;
		}
		uint16_t magic;
		fin.read((char*)&magic, 2);
		fin.read((char*)&univ.party.setup, sizeof(univ.party.setup));
		if(magic == 0x0E0B) // should be 0x0B0E!
			for(auto& i : univ.party.setup)
				for(auto& j : i)
					for(auto& k : j)
						flip_short(reinterpret_cast<int16_t*>(&k));
	}
	
	// Next load the PCs
	for(int i = 0; i < 6; i++) {
		static char fname[] = "save/pc1.txt";
		fname[7] = i + '1';
		std::istream& fin = partyIn.getFile(fname);
		if(!fin) {
			cChoiceDlog("load-game-fail").show();
			return false;
		}
		univ.party[i].readFrom(fin);
	}
	
	if(in_scen) {
		fs::path path;
		path = progDir/"Blades of Exile Scenarios"/univ.party.scen_name;
		
		if(!load_scenario(path, univ.scenario))
			return false;
		
		if(town_restore) {
			// Load town data
			std::istream& fin = partyIn.getFile("save/town.txt");
			if(!fin) {
				cChoiceDlog("load-game-fail").show();
				return false;
			}
			univ.town.readFrom(fin);
			
			if(maps_there) {
				// Read town maps
				std::istream& fin = partyIn.getFile("save/townmaps.dat");
				// TODO: Warn if maps missing
				for(int i = 0; i < 200; i++)
					for(int j = 0; j < 8; j++)
						for(int k = 0; k < 64; k++)
							univ.town_maps[i][j][k] = fin.get();
			}
		}
		
		// Load outdoors data
		std::istream& fin = partyIn.getFile("save/out.txt");
		if(!fin) {
			cChoiceDlog("load-game-fail").show();
			return false;
		}
		univ.out.readFrom(fin);
		
		if(maps_there) {
			// Read outdoor maps
			std::istream& fin = partyIn.getFile("save/outmaps.dat");
			// TODO: Warn if maps missing
			for(int i = 0; i < 100; i++)
				for(int j = 0; j < 6; j++)
					for(int k = 0; k < 48; k++)
						univ.out_maps[i][j][k] = fin.get();
		}
	}
	
	// TODO: Also get the party custom graphics sheet
	
	return true;
}

//mode;  // 0 - normal  1 - save as
bool save_party(fs::path dest_file, const cUniverse& univ) {
	if(!fs::exists(tempDir)) fs::create_directories(tempDir);
	
	bool in_scen = univ.party.scen_name != "";
	bool in_town = univ.town.num < 200;
	bool save_maps = !univ.party.stuff_done[306][0];
	
	tarball partyOut;
	
	// First, write the main party data
	univ.party.writeTo(partyOut.newFile("save/party.txt"));
	{
		std::ostream& fout = partyOut.newFile("save/setup.dat");
		static uint16_t magic = 0x0B0E;
		fout.write((char*)&magic, 2);
		fout.write((char*)&univ.party.setup, sizeof(univ.party.setup));
	}
	
	// Then write the data for each of the party members
	for(int i = 0; i < 6; i++) {
		static char fname[] = "save/pc1.txt";
		fname[7] = i + '1';
		univ.party[i].writeTo(partyOut.newFile(fname));
	}
	
	if(in_scen) {
		if(in_town) {
			// Write the current town data
			univ.town.writeTo(partyOut.newFile("save/town.txt"));
			
			if(save_maps) {
				// Write the town map data
				std::ostream& fout = partyOut.newFile("save/townmaps.dat");
				for(int i = 0; i < 200; i++)
					for(int j = 0; j < 8; j++)
						for(int k = 0; k < 64; k++)
							fout.put(univ.town_maps[i][j][k]);
			}
		}
		
		// Write the current outdoors data
		univ.out.writeTo(partyOut.newFile("save/out.txt"));
		
		if(save_maps) {
			// Write the outdoors map data
			std::ostream& fout = partyOut.newFile("save/outmaps.dat");
			for(int i = 0; i < 100; i++)
				for(int j = 0; j < 6; j++)
					for(int k = 0; k < 48; k++)
						fout.put(univ.out_maps[i][j][k]);
		}
	}
	
	// TODO: Add the party graphics sheet
	
	// Write out the compressed data
	fs::path tempPath = tempDir/"savetemp.exg";
	ogzstream zout(tempPath.string().c_str());
	partyOut.writeTo(zout);
	zout.close();
	
	// Now add the header. We use the temporary file because we want the header to be uncompressed.
	int16_t flags[] = {
		0x0B0E, // to indicate new format
		static_cast<short>(in_town ? 1342 : 5790), // is the party in town?
		static_cast<short>(in_scen ? 100 : 200), // is the party in a scenario?
		static_cast<short>(save_maps ? 5567 : 3422), // is the save maps feature enabled?
		OBOE_CURRENT_VERSION >> 8, // current version number, major and minor revisions only
		// Version 1 indicates a beta format that may not be supported in the final release
	};
	if(!mac_is_intel) // must flip all the flags to little-endian
		for(int i = 0; i < 5; i++)
			flip_short(&flags[i]);
	
	std::ifstream fin(tempPath.c_str(), std::ios_base::binary);
	std::ofstream fout(dest_file.c_str(), std::ios_base::binary);
	fout.write((char*) flags, 10);
	fout << fin.rdbuf();
	fin.close();
	fout.close();
	return true;
}

std::string read_maybe_quoted_string(std::istream& from) {
	std::string result;
	from >> std::skipws;
	if(from.peek() == '"' ||  from.peek() == '\'') {
		char delim = from.get();
		getline(from, result, delim);
		if(result.empty()) return result;
		while(result[result.length() - 1] == '\\') {
			result += delim;
			std::string nextPart;
			getline(from, nextPart, delim);
			result += nextPart;
		}
	} else from >> result;
	return result;
}

std::string maybe_quote_string(std::string which) {
	if(which.find_first_of(' ') != std::string::npos || which[0] == '"' || which[0] == '\'') {
		// The string contains spaces or starts with a quote, so quote it.
		// We may have to escape quotes or backslashes.
		int apos = 0, quot = 0, bslash = 0;
		std::for_each(which.begin(), which.end(), [&apos,&quot,&bslash](char c) {
			if(c == '\'') apos++;
			if(c == '"') quot++;
			if(c == '\\') bslash++;
		});
		char quote_c;
		// Surround it in whichever quote character appears fewer times.
		if(quot < apos) quote_c = '"';
		else quote_c = '\'';
		// Let's create this string to initially have the required size.
		std::string temp;
		size_t quoted_len = which.length() + std::min(quot,apos) + bslash + 2;
		temp.reserve(quoted_len);
		temp += quote_c;
		for(size_t i = 0; i < which.length(); i++) {
			if(which[i] == quote_c) {
				temp += '\\';
				temp += quote_c;
			} else if(which[i] == '\\')
				temp += R"(\\)";
			else temp += which[i];
		}
		temp += quote_c;
		which.swap(temp);
	}
	return which;
}

