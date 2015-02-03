//
//  fileio_scen.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#include "fileio.hpp"

#include <fstream>
#include <boost/filesystem/operations.hpp>

#include "dlogutil.hpp"

#include "scenario.h"
#include "regtown.h"
#include "map_parse.hpp"
#include "graphtool.hpp"
#include "mathutil.hpp"

#include "porting.hpp"
#include "restypes.hpp"

bool cur_scen_is_mac = true;
extern sf::Texture items_gworld,tiny_obj_gworld,fields_gworld,roads_gworld,boom_gworld,missiles_gworld;
extern sf::Texture dlogpics_gworld,monst_gworld[],terrain_gworld[],anim_gworld,talkfaces_gworld,pc_gworld;
extern sf::Texture status_gworld, vehicle_gworld, small_ter_gworld;
extern cCustomGraphics spec_scen_g;
//fs::path progDir, tempDir;

void load_spec_graphics(fs::path scen_file);
// Load old scenarios (town talk is handled by the town loading function)
static bool load_scenario_v1(fs::path file_to_load, cScenario& scenario);
static bool load_outdoors_v1(fs::path scen_file, location which_out,cOutdoors& the_out, legacy::scenario_data_type& scenario);
static bool load_town_v1(fs::path scen_file,short which_town,cTown& the_town,legacy::scenario_data_type& scenario,std::vector<shop_info_t>& shops);
// Load new scenarios
static bool load_outdoors(fs::path out_base, location which_out, cOutdoors& the_out);
static bool load_town(fs::path town_base, short which_town, cTown*& the_town);
static bool load_town_talk(fs::path town_base, short which_town, cSpeech& the_talk);

bool load_scenario(fs::path file_to_load, cScenario& scenario) {
	scenario = cScenario();
	// TODO: Implement checking to determine whether it's old or new
	return load_scenario_v1(file_to_load, scenario);
}

static void port_shop_spec_node(cSpecial& spec, std::vector<shop_info_t>& shops) {
	int which_shop;
	if(spec.ex1b < 4) {
		shops.push_back({eShopItemType(spec.ex1b + 1), spec.ex1a, spec.ex2a});
		which_shop = shops.size() + 5;
	} else if(spec.ex1b == 4)
		which_shop = 5;
	else which_shop = spec.ex1b - 5;
	
	spec.ex1a = which_shop;
	spec.ex1b = spec.ex2b;
	spec.ex2a = spec.ex2b = -1;
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
	scenario.outdoors.resize(temp_scenario->out_width, temp_scenario->out_height);
	for(int x = 0; x < temp_scenario->out_width; x++) {
		for(int y = 0; y < temp_scenario->out_height; y++) {
			scenario.outdoors[x][y] = new cOutdoors(scenario);
			load_outdoors_v1(scenario.scen_file, loc(x,y), *scenario.outdoors[x][y], *temp_scenario);
		}
	}
	
	// Need to build a list of shops used in the scenario
	std::vector<shop_info_t> shops;
	
	// Then load all the towns
	scenario.towns.resize(scenario.format.num_towns);
	for(int i = 0; i < scenario.format.num_towns; i++) {
		switch(temp_scenario->town_size[i]) {
			case 0: scenario.towns[i] = new cBigTown(scenario); break;
			case 1: scenario.towns[i] = new cMedTown(scenario); break;
			case 2: scenario.towns[i] = new cTinyTown(scenario); break;
		}
		load_town_v1(scenario.scen_file, i, *scenario.towns[i], *temp_scenario, shops);
	}
	// Enable character creation in starting town
	scenario.towns[scenario.which_town_start]->has_tavern = true;
	
	// Check special nodes in case there were outdoor shops
	for(cSpecial& spec : scenario.scen_specials) {
		if(spec.type == eSpecType::ENTER_SHOP)
			port_shop_spec_node(spec, shops);
	}
	for(cOutdoors* out : scenario.outdoors) {
		for(cSpecial& spec : out->specials) {
			if(spec.type == eSpecType::ENTER_SHOP)
				port_shop_spec_node(spec, shops);
		}
	}
	// We'll check town nodes too, just in case.
	for(cTown* town : scenario.towns) {
		for(cSpecial& spec : town->specials) {
			if(spec.type == eSpecType::ENTER_SHOP)
				port_shop_spec_node(spec, shops);
		}
	}
	
	// Now we have to build all those shops
	for(shop_info_t info : shops) {
		pic_num_t face = 0;
		if(info.type == eShopItemType::MAGE_SPELL || info.type == eShopItemType::PRIEST_SPELL || info.type == eShopItemType::ALCHEMY)
			face = 43;
		else if(info.type == eShopItemType::ITEM) {
			bool is_food_shop = true;
			for(int i = info.first; i < info.first + info.count && i < scenario.scen_items.size(); i++) {
				if(scenario.scen_items[i].variety != eItemType::FOOD)
					is_food_shop = false;
			}
			if(is_food_shop)
				face = 42;
		}
		eShopType type = eShopType::NORMAL;
		eShopPrompt prompt = eShopPrompt::SHOPPING;
		if(info.type == eShopItemType::MAGE_SPELL)
			prompt = eShopPrompt::MAGE;
		else if(info.type == eShopItemType::PRIEST_SPELL)
			prompt = eShopPrompt::PRIEST;
		else if(info.type == eShopItemType::ALCHEMY) {
			prompt = eShopPrompt::ALCHEMY;
			type = eShopType::ALLOW_DEAD;
		}
		cShop shop(type, prompt, face, 0, "");
		if(info.type == eShopItemType::ITEM) {
			int end = info.first + info.count;
			end = min(end, scenario.scen_items.size());
			shop.addItems(scenario.scen_items.begin() + info.first, scenario.scen_items.begin() + end, cShop::INFINITE);
		} else {
			int max = 62;
			if(info.type == eShopItemType::ALCHEMY)
				max = 20;
			int end = min(max, info.first + info.count);
			for(int i = info.first; i < end; i++)
				shop.addSpecial(info.type, i);
		}
		scenario.shops.push_back(shop);
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

bool load_town_v1(fs::path scen_file, short which_town, cTown& the_town, legacy::scenario_data_type& scenario, std::vector<shop_info_t>& shops) {
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
			the_town.room_rect[i-1].descr = temp_str;
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
	the_town.talking.append(store_talk, shops);
	
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
	map_data map = load_map(town_base/fname, true);
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

static long get_outdoors_offset(location& which_out, legacy::scenario_data_type& scenario){
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
bool load_outdoors_v1(fs::path scen_file, location which_out,cOutdoors& the_out, legacy::scenario_data_type& scenario){
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
	the_out.append(store_out);
	for(i = 0; i < 108; i++) {
		len = (long) (store_out.strlens[i]);
		n = fread(temp_str, len, 1, file_id);
		temp_str[len] = 0;
		if(i == 0) the_out.out_name = temp_str;
		else if(i == 9) the_out.comment = temp_str;
		else if(i < 9) the_out.info_rect[i-1].descr = temp_str;
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
	map_data map = load_map(out_base/fname, false);
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
	std::cout << "Loading scenario graphics... (" << path  << ")\n";
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
