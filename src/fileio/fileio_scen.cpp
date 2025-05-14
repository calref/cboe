//
//  fileio_scen.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#include "fileio.hpp"

#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

#include "dialogxml/dialogs/strdlog.hpp"

#include "scenario/scenario.hpp"
#include "scenario/town.hpp"
#include "map_parse.hpp"
#include "special_parse.hpp"
#include "gfx/gfxsheets.hpp"
#include "mathutil.hpp"
#include "gzstream.h"
#include "tarball.hpp"
#include "replay.hpp"

#include "porting.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "fileio/resmgr/res_sound.hpp"

// Because the full template definition needs to be visible in this file
// Also, for some reason, it's not found in the include paths, so use a relative path
#include "../scenario/town_import.tpp"

bool cur_scen_is_mac = true;
extern cCustomGraphics spec_scen_g;
extern fs::path tempDir, scenDir, progDir;
extern std::string last_load_file;

void load_spec_graphics_v1(fs::path scen_file);
void load_spec_graphics_v2(int num_sheets);
// Load old scenarios (town talk is handled by the town loading function)
static bool load_scenario_v1(fs::path file_to_load, cScenario& scenario, eLoadScenario load_type);
static bool load_outdoors_v1(fs::path scen_file, location which_out,cOutdoors& the_out, legacy::scenario_data_type& scenario);
static bool load_town_v1(fs::path scen_file,short which_town,cTown& the_town,legacy::scenario_data_type& scenario,std::vector<shop_info_t>& shops);
// Load new scenarios
static bool load_scenario_v2(fs::path file_to_load, cScenario& scenario, eLoadScenario load_type);
// Some of these are non-static so that the test cases can access them.
ticpp::Document xmlDocFromStream(std::istream& stream, std::string name);
void readScenarioFromXml(ticpp::Document&& data, cScenario& scenario);
void readEditorStateFromXml(ticpp::Document&& data, cScenario& scenario);
void readTerrainFromXml(ticpp::Document&& data, cScenario& scenario);
void readItemsFromXml(ticpp::Document&& data, cScenario& scenario);
void readMonstersFromXml(ticpp::Document&& data, cScenario& scenario);
void readOutdoorsFromXml(ticpp::Document&& data, cOutdoors& out);
void readTownFromXml(ticpp::Document&& data, cTown*& town, cScenario& scen);
void readDialogueFromXml(ticpp::Document&& data, cSpeech& talk, int town_num);
void loadOutMapData(map_data&& data, location which, cScenario& scen);
void loadTownMapData(map_data&& data, int which, cScenario& scen);

static std::string get_file_error() {
	std::ostringstream sout;
	sout << std_fmterr;
	return sout.str();
}

// Debug builds run from working directory "build/Blades of Exile"
// should helpfully let you enter replay test scenarios.

// Support for multiple scenario directories will also help with how I plan
// to handle scenario packaging/distribution for my fork's release.
//  - Nat
std::vector<fs::path> all_scen_dirs() {
	std::vector<fs::path> scen_dirs = { scenDir };

	for(fs::path scen_dir : extra_scen_dirs){
		scen_dirs.push_back(scen_dir);
	}

	#ifdef DEBUG
	fs::path replay_scenarios_dir = boost::filesystem::current_path();
	replay_scenarios_dir = replay_scenarios_dir/".."/".."/"test"/"replays"/"scenarios";
	if(fs::is_directory(replay_scenarios_dir)){
		scen_dirs.push_back(replay_scenarios_dir);
	}
	#endif

	return scen_dirs;
}

fs::path locate_scenario(std::string scen_name, bool allow_unpacked) {
	fs::create_directories(scenDir);
	std::transform(scen_name.begin(), scen_name.end(), scen_name.begin(), tolower);
	size_t dot = scen_name.find_first_of('.');
	std::string base_name = scen_name.substr(0,dot);
	if(base_name == "valleydy" || base_name == "stealth" || base_name == "zakhazi"/* || base_name == "busywork" */)
		return progDir/"Blades of Exile Scenarios"/scen_name;
	fs::path scenPath;

	for(fs::path scenDir : all_scen_dirs()){
		for(fs::recursive_directory_iterator iter(scenDir); iter != fs::recursive_directory_iterator(); iter++) {
			fs::file_status stat = iter->status();
			std::string fname = iter->path().filename().string().c_str();
			std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
			if(fname == "header.exs") {
				if(scen_name != "header.exs"){
					// We want to support a scenario whose main filename is header.exs, just in case.
					// However, any unpacked scenarios would have a header.exs.
					// So, skip them if they're in a .boes folder.
					fname = iter->path().parent_path().filename().string().c_str();
					std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
					size_t dot = fname.find_first_of('.');
					if(dot != std::string::npos && fname.substr(dot) == ".boes")
						continue;
				}else if(!allow_unpacked){
					continue;
				}
			}
			if(fname != scen_name) continue;
			size_t dot = fname.find_first_of('.');
			if(dot == std::string::npos) continue;
			if(fname.substr(dot) == ".exs" && stat.type() == fs::regular_file) {
				scenPath = iter->path();
				break;
			}
			if(fname.substr(dot) == ".boes" && (stat.type() == fs::regular_file || stat.type() == fs::directory_file)) {
				scenPath = iter->path();
				break;
			}
		}

		if(!scenPath.empty())
			break;
	}
	return scenPath;
}

bool cur_scen_pushed_paths = false;

bool load_scenario(fs::path file_to_load, cScenario& scenario, eLoadScenario load_type) {

	// Before loading a scenario, we may need to pop scenario resource paths.
	if(cur_scen_pushed_paths){
		fs::path graphics_path = ResMgr::graphics.popPath();
		for(auto p : graphics_path) {
			if(p.string() == "data") {
				ResMgr::graphics.pushPath(graphics_path);
				break;
			}
		}
		fs::path sounds_path = ResMgr::sounds.popPath();
		for(auto p : sounds_path) {
			if(p.string() == "data") {
				ResMgr::sounds.pushPath(sounds_path);
				break;
			}
		}
	}
	cur_scen_pushed_paths = false;
	scenario = cScenario();
	std::string fname = file_to_load.filename().string();
	std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
	size_t dot = fname.find_last_of('.');
	if(dot == std::string::npos) {
		showError("That is not a Blades of Exile scenario.");
		return false;
	}  else try {
		if(fname.substr(dot) == ".boes"){
			if(load_scenario_v2(file_to_load, scenario, load_type)){
				last_load_file = file_to_load.string();
				return true;
			}
			return false;
		}else if(fname.substr(dot) == ".exs"){
			if(load_scenario_v1(file_to_load, scenario, load_type)){
				last_load_file = file_to_load.string();
				return true;
			}
			return false;
		}
	} catch(std::exception& x) {
		showError("There was an error loading the scenario. The details of the error are given below; you may be able to decompress the scenario package, fix the error, and repack it.", x.what());
		return false;
	}
	showError("That is not a Blades of Exile scenario.");
	return false;
}

template<typename Container> static void port_shop_spec_node(cSpecial& spec, std::vector<shop_info_t>& shops, Container strs) {
	int which_shop;
	if(spec.ex1b < 4) {
		// Safeguard against invalid data
		if(spec.ex1a < 0)
			spec.ex1a = 1;
		std::string title;
		if(spec.m1 >= 0 && spec.m1 <= strs.size())
			title = strs[spec.m1];
		shops.push_back({eShopItemType(spec.ex1b + 1), spec.ex1a, spec.ex2a, title});
		which_shop = shops.size() + 5;
	} else if(spec.ex1b == 4)
		which_shop = 5;
	else which_shop = spec.ex1b - 5;
	
	spec.ex1a = which_shop;
	spec.ex1b = spec.ex2b;
	spec.ex2a = spec.ex2b = -1;
}

static const std::string err_prefix = "Error loading Blades of Exile Scenario: ";
bool load_scenario_v1(fs::path file_to_load, cScenario& scenario, eLoadScenario load_type){
	bool file_ok = false;
	long len;
	char temp_str[256];
	legacy::scenario_data_type temp_scenario;
	legacy::scen_item_data_type item_data;
	// TODO: Convert this (and all the others in this file) to use C++ streams
	FILE* file_id = fopen(file_to_load.string().c_str(),"rb");
	if(file_id == nullptr) {
		showError(err_prefix + "Could not open file.", get_file_error());
		return false;
	}
	
	len = (long) sizeof(scenario_header_flags);
	
	if(fread(&scenario.format, len, 1, file_id) < 1){
		showError(err_prefix + "Failed to read scenario header.", get_file_error());
		fclose(file_id);
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
	} else if(scenario.format.flag1 == 'O' && scenario.format.flag2 == 'B' && scenario.format.flag3 == 'O' && scenario.format.flag4 == 'E') {
		// This means we're looking at the scenario header file of an unpacked new-format scenario.
		return load_scenario_v2(file_to_load.parent_path(), scenario, load_type);
	}
	if(!file_ok) {
		fclose(file_id);
		showError("This is not a legitimate Blades of Exile scenario.");
		return false;
	}
	
	len = (long) sizeof(legacy::scenario_data_type);
	if(fread(&temp_scenario, len, 1, file_id) < 1) {
		showError(err_prefix + "Failed to read scenario data.", get_file_error());
		fclose(file_id);
		return false;
	}
	port_scenario(&temp_scenario);
	len = sizeof(legacy::scen_item_data_type); // item data
	if(fread(&item_data, len, 1, file_id) < 1) {
		showError(err_prefix + "Failed to read scenario items.", get_file_error());
		fclose(file_id);
		return false;
	}
	port_item_list(&item_data);
	scenario.import_legacy(temp_scenario);
	scenario.import_legacy(item_data);
	
	// TODO: Consider skipping the fread and assignment when len is 0
	scenario.special_items.resize(50);
	scenario.journal_strs.resize(50);
	scenario.spec_strs.resize(100);
	for(short i = 0; i < 270; i++) {
		len = (long) (temp_scenario.scen_str_len[i]);
		fread(temp_str, len, 1, file_id);
		temp_str[len] = 0;
		if(i == 0) scenario.scen_name = temp_str;
		else if(i == 1 || i == 2)
			scenario.teaser_text[i-1] = temp_str;
		else if(i == 3)
			scenario.contact_info[1] = temp_str;
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
	if(load_type == eLoadScenario::ONLY_HEADER) return true;
	load_spec_graphics_v1(scenario.scen_file);
	
	// Now load all the outdoor sectors
	scenario.outdoors.resize(temp_scenario.out_width, temp_scenario.out_height);
	for(int x = 0; x < temp_scenario.out_width; x++) {
		for(int y = 0; y < temp_scenario.out_height; y++) {
			scenario.outdoors[x][y] = new cOutdoors(scenario);
			load_outdoors_v1(scenario.scen_file, loc(x,y), *scenario.outdoors[x][y], temp_scenario);
		}
	}
	
	// Need to build a list of shops used in the scenario
	std::vector<shop_info_t> shops;
	
	// Then load all the towns
	scenario.towns.resize(scenario.format.num_towns);
	for(int i = 0; i < scenario.format.num_towns; i++) {
		switch(temp_scenario.town_size[i]) {
			case 0: scenario.towns[i] = new cTown(scenario, AREA_LARGE); break;
			case 1: scenario.towns[i] = new cTown(scenario, AREA_MEDIUM); break;
			case 2: scenario.towns[i] = new cTown(scenario, AREA_SMALL); break;
		}
		load_town_v1(scenario.scen_file, i, *scenario.towns[i], temp_scenario, shops);
	}
	// Enable character creation in starting town
	scenario.towns[scenario.which_town_start]->has_tavern = true;
	
	if(load_type == eLoadScenario::SAVE_PREVIEW)
		return true;
	
	// Check special nodes in case there were outdoor shops
	for(cSpecial& spec : scenario.scen_specials) {
		if(spec.type == eSpecType::ENTER_SHOP)
			port_shop_spec_node(spec, shops, scenario.spec_strs);
	}
	for(cOutdoors* out : scenario.outdoors) {
		for(cSpecial& spec : out->specials) {
			if(spec.type == eSpecType::ENTER_SHOP)
				port_shop_spec_node(spec, shops, out->spec_strs);
		}
	}
	// We'll check town nodes too, just in case.
	for(cTown* town : scenario.towns) {
		for(cSpecial& spec : town->specials) {
			if(spec.type == eSpecType::ENTER_SHOP)
				port_shop_spec_node(spec, shops, town->spec_strs);
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
		cShop shop(type, prompt, face, 0, info.name);
		if(info.type == eShopItemType::ITEM) {
			int end = info.first + info.count;
			end = min(end, scenario.scen_items.size());
			shop.addItems(info.first, scenario.scen_items.begin() + info.first, scenario.scen_items.begin() + end, cShop::INFINITE_AMOUNT);
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
	
	return true;
}

ticpp::Document xmlDocFromStream(std::istream& stream, std::string name) {
	if(!stream) throw std::string("Error loading scenario file: ") + name;
	std::string contents;
	stream.seekg(0, std::ios::end);
	contents.reserve(stream.tellg());
	stream.seekg(0, std::ios::beg);
	contents.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
	ticpp::Document doc(name);
	doc.Parse(contents);
	return doc;
}

static std::tuple<int,int,int> parse_version(std::string str) {
	int maj = 0, min = 0, rev = 0;
	int at_which_part = 0;
	for(char c : str) {
		if(isdigit(c)) {
			if(at_which_part == 0) {
				maj *= 10;
				maj += c - '0';
			} else if(at_which_part == 1) {
				min *= 10;
				min += c - '0';
			} else if(at_which_part == 2) {
				rev *= 10;
				rev += c - '0';
			}
		} else if(c == '.' && at_which_part == 0)
			at_which_part = 1;
		else if(c == '.' && at_which_part == 1)
			at_which_part = 2;
		else throw std::string("Invalid version string: " + str);
	}
	return std::make_tuple(maj, min, rev);
}

template<typename T = int>
static location readLocFromXml(ticpp::Element& data, std::string prefix = "", std::string extra = "", T* extra_val = nullptr) {
	using namespace ticpp;
	Iterator<Attribute> attr;
	std::string type, name, fname;
	data.GetDocument()->GetValue(&fname);
	data.GetValue(&type);
	location pos = {-1000, -1000};
	bool got_extra = extra_val == nullptr;
	for(attr = attr.begin(&data); attr != attr.end(); attr++) {
		attr->GetName(&name);
		if(name == prefix + "x")
			attr->GetValue(&pos.x);
		else if(name == prefix + "y")
			attr->GetValue(&pos.y);
		else if(name == extra && extra_val != nullptr) {
			attr->GetValue(extra_val);
			got_extra = true;
		} else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
	}
	if(pos.x == -1000)
		throw xMissingAttr(type, prefix + "x", data.Row(), data.Column(), fname);
	if(pos.y == -1000)
		throw xMissingAttr(type, prefix + "y", data.Row(), data.Column(), fname);
	if(!got_extra)
		throw xMissingAttr(type, extra, data.Row(), data.Column(), fname);
	return pos;
}

template<typename T = int>
static rectangle readRectFromXml(ticpp::Element& data, std::string prefix = "", std::string extra = "", T* extra_val=nullptr) {
	using namespace ticpp;
	Iterator<Attribute> attr;
	std::string type, name, fname;
	data.GetDocument()->GetValue(&fname);
	data.GetValue(&type);
	rectangle rect = {-1000, -1000, -1000, -1000};
	bool got_extra = extra_val == nullptr;
	for(attr = attr.begin(&data); attr != attr.end(); attr++) {
		attr->GetName(&name);
		if(name == prefix + "top")
			attr->GetValue(&rect.top);
		else if(name == prefix + "left")
			attr->GetValue(&rect.left);
		else if(name == prefix + "bottom")
			attr->GetValue(&rect.bottom);
		else if(name == prefix + "right")
			attr->GetValue(&rect.right);
		else if(name == extra && extra_val != nullptr) {
			attr->GetValue(extra_val);
			got_extra = true;
		} else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
	}
	if(rect.top == -1000)
		throw xMissingAttr(type, prefix + "top", data.Row(), data.Column(), fname);
	if(rect.left == -1000)
		throw xMissingAttr(type, prefix + "left", data.Row(), data.Column(), fname);
	if(rect.bottom == -1000)
		throw xMissingAttr(type, prefix + "bottom", data.Row(), data.Column(), fname);
	if(rect.right == -1000)
		throw xMissingAttr(type, prefix + "right", data.Row(), data.Column(), fname);
	if(!got_extra)
		throw xMissingAttr(type, extra, data.Row(), data.Column(), fname);
	return rect;
}

static void readSpecItemFromXml(ticpp::Element& data, cSpecItem& item) {
	using namespace ticpp;
	std::string type, name, val, fname;
	data.GetDocument()->GetValue(&fname);
	data.GetValue(&type);
	item.special = -1;
	item.flags = 0;
	Iterator<Attribute> attr;
	for(attr = attr.begin(&data); attr != attr.end(); attr++) {
		attr->GetName(&name);
		if(name == "special") {
			attr->GetValue(&item.special);
		} else if(name == "start-with") {
			attr->GetValue(&val);
			if(val == "true")
				item.flags += 10;
		} else if(name == "useable") {
			attr->GetValue(&val);
			if(val == "true")
				item.flags += 1;
		} else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
	}
	std::set<std::string> reqs = {"name", "description"};
	Iterator<Element> elem;
	for(elem = elem.begin(&data); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		reqs.erase(type);
		if(type == "name") {
			elem->GetText(&item.name, false);
		} else if(type == "description") {
			elem->GetText(&item.descr, false);
		} else throw xBadNode(type, elem->Row(), elem->Column(), fname);
	}
	if(!reqs.empty())
		throw xMissingElem("special-item", *reqs.begin(), data.Row(), data.Column(), fname);
}

static void readQuestFromXml(ticpp::Element& data, cQuest& quest) {
	using namespace ticpp;
	std::string type, name, val, fname;
	data.GetDocument()->GetValue(&fname);
	data.GetValue(&type);
	quest.deadline_is_relative = quest.auto_start = false;
	Iterator<Attribute> attr;
	for(attr = attr.begin(&data); attr != attr.end(); attr++) {
		attr->GetName(&name);
		if(name == "start-with") {
			attr->GetValue(&val);
			if(val == "true")
				quest.auto_start = true;
		} else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
	}
	std::set<std::string> reqs = {"name", "description"};
	Iterator<Element> elem;
	int banks_found = 0;
	for(elem = elem.begin(&data); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		reqs.erase(type);
		if(type == "deadline") {
			for(attr = attr.begin(elem.Get()); attr != attr.end(); attr++) {
				attr->GetName(&name);
				if(name == "relative") {
					attr->GetValue(&val);
					if(val == "true")
						quest.deadline_is_relative = true;
				} else if(name == "waive-if")
					attr->GetValue(&quest.event);
				else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
			}
			elem->GetText(&quest.deadline);
		} else if(type == "reward") {
			for(attr = attr.begin(elem.Get()); attr != attr.end(); attr++) {
				attr->GetName(&name);
				if(name == "xp")
					attr->GetValue(&quest.xp);
				else if(name == "gold")
					attr->GetValue(&quest.gold);
				else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
			}
		} else if(type == "bank") {
			if(banks_found == 0)
				elem->GetText(&quest.bank1);
			else if(banks_found == 1)
				elem->GetText(&quest.bank2);
			else throw xBadNode(type, elem->Row(), elem->Column(), fname);
			banks_found++;
		} else if(type == "name") {
			elem->GetText(&quest.name, false);
		} else if(type == "description") {
			elem->GetText(&quest.descr, false);
		} else throw xBadNode(type, elem->Row(), elem->Column(), fname);
	}
	if(!reqs.empty())
		throw xMissingElem("quest", *reqs.begin(), data.Row(), data.Column(), fname);
}

static void readShopFromXml(ticpp::Element& data, cShop& shop) {
	using namespace ticpp;
	std::string type, name, val, fname;
	data.GetDocument()->GetValue(&fname);
	data.GetValue(&type);
	std::set<std::string> reqs = {"name", "type", "prompt", "face", "entries"};
	Iterator<Element> elem;
	for(elem = elem.begin(&data); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		reqs.erase(type);
		if(type == "name") {
			elem->GetText(&val, false);
			shop.setName(val);
		} else if(type == "type") {
			elem->GetText(&val);
			shop.setType(boost::lexical_cast<eShopType>(val));
		} else if(type == "prompt") {
			elem->GetText(&val);
			shop.setPrompt(boost::lexical_cast<eShopPrompt>(val));
		} else if(type == "face") {
			pic_num_t face;
			elem->GetText(&face);
			shop.setFace(face);
		} else if(type == "entries") {
			cItem dummy_item;
			dummy_item.variety = eItemType::GOLD;
			Iterator<Element> entry;
			for(entry = entry.begin(elem.Get()); entry != entry.end(); entry++) {
				entry->GetValue(&type);
				if(type == "item") {
					int amount = -1, num, chance = 100;
					std::string title, descr;
					Iterator<Attribute> attr;
					for(attr = attr.begin(entry.Get()); attr != attr.end(); attr++) {
						attr->GetName(&name);
						if(name == "quantity") {
							attr->GetValue(&val);
							if(val == "infinite")
								amount = 0;
							else amount = boost::lexical_cast<int>(val);
						} else if(name == "chance") {
							attr->GetValue(&chance);
							if(amount == -1)
							   amount = 1;
						} else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
					}
					if(amount == -1)
						amount = 0;
					entry->GetText(&num);
					shop.addItem(num, dummy_item, amount, chance);
				} else if(type == "special") {
					int amount = 0, node = 0, cost = 0, icon = 0;
					std::string title, descr;
					std::set<std::string> reqs = {"quantity", "node", "icon", "name", "description"};
					Iterator<Element> attr;
					for(attr = attr.begin(entry.Get()); attr != attr.end(); attr++) {
						attr->GetValue(&name);
						reqs.erase(name);
						if(name == "quantity") {
							attr->GetText(&val);
							if(val == "infinite")
								amount = 0;
							else amount = boost::lexical_cast<int>(val);
						} else if(name == "cost") {
							attr->GetText(&cost);
						} else if(name == "node") {
							attr->GetText(&node);
						} else if(name == "icon") {
							attr->GetText(&icon);
						} else if(name == "name") {
							attr->GetText(&title, false);
						} else if(name == "description") {
							attr->GetText(&descr, false);
						} else throw xBadNode(name, attr->Row(), attr->Column(), fname);
					}
					if(!reqs.empty())
						throw xMissingElem("special", *reqs.begin(), entry->Row(), entry->Column(), fname);
					shop.addSpecial(title, descr, icon, node, cost, amount);
				} else {
					eShopItemType itype{};
					int n = 0;
					if(type == "mage-spell") {
						itype = eShopItemType::MAGE_SPELL;
						entry->GetText(&n);
					} else if(type == "priest-spell") {
						itype = eShopItemType::PRIEST_SPELL;
						entry->GetText(&n);
					} else if(type == "recipe") {
						itype = eShopItemType::ALCHEMY;
						entry->GetText(&n);
					} else if(type == "skill") {
						itype = eShopItemType::SKILL;
						entry->GetText(&n);
					} else if(type == "treasure") {
						itype = eShopItemType::TREASURE;
						entry->GetText(&n);
					} else if(type == "class") {
						itype = eShopItemType::CLASS;
						entry->GetText(&n);
					} else if(type == "heal") {
						entry->GetText(&n);
						itype = eShopItemType(n + int(eShopItemType::HEAL_WOUNDS));
						n = 0;
					} else throw xBadNode(type, entry->Row(), entry->Column(), fname);
					shop.addSpecial(itype, n);
				}
			}
		} else throw xBadNode(type, elem->Row(), elem->Column(), fname);
	}
	if(!reqs.empty())
		throw xMissingElem("shop", *reqs.begin(), data.Row(), data.Column(), fname);
}

static void readTimerFromXml(ticpp::Element& data, cTimer& timer) {
	using namespace ticpp;
	std::string type, name, fname;
	data.GetDocument()->GetValue(&fname);
	data.GetValue(&type);
	timer.time = -1000;
	Iterator<Attribute> attr;
	for(attr = attr.begin(&data); attr != attr.end(); attr++) {
		attr->GetName(&name);
		if(name == "freq")
			attr->GetValue(&timer.time);
		else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
	}
	if(timer.time == -1000)
		throw xMissingAttr(type, "freq", data.Row(), data.Column(), fname);
	data.GetText(&timer.node);
}

static void initialXmlRead(ticpp::Document& data, std::string root_tag, int& maj, int& min, int& rev, std::string& fname) {
	// This checks that the root tag is correct and reads the format version from the boes attribute.
	using namespace ticpp;
	maj = -1; min = -1; rev = -1; // These are currently unused, but eventually might be used if the format changes
	std::string type, name, val;
	data.GetValue(&fname);
	data.FirstChildElement()->GetValue(&type);
	if(type != root_tag) throw xBadNode(type,data.FirstChildElement()->Row(),data.FirstChildElement()->Column(),fname);
	Iterator<Attribute> attr;
	for(attr = attr.begin(data.FirstChildElement()); attr != attr.end(); attr++) {
		attr->GetName(&name);
		attr->GetValue(&val);
		if(name == "boes") {
			std::tie(maj, min, rev) = parse_version(val);
			if(maj < 2) {
				showError("This scenario specifies an invalid format version. Loading will be attempted as if it were version 2.0.0, but there is a possibility that there could be errors.");
				maj = 2;
				min = rev = 0;
			}
		} else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
	}
	if(maj < 0 || min < 0 || rev < 0)
		throw xMissingAttr(type, "boes", data.FirstChildElement()->Row(), data.FirstChildElement()->Column(), fname);
}

void readScenarioFromXml(ticpp::Document&& data, cScenario& scenario) {
	using namespace ticpp;
	int maj, min, rev;
	std::string fname, type, name, val;
	initialXmlRead(data, "scenario", maj, min, rev, fname);
	Iterator<Attribute> attr;
	Iterator<Element> elem;
	std::set<std::string> reqs = {
		"title", "icon", "id", "version",
		"language", "author", "text", "ratings",
		"flags", "creator", "game", "editor"
	};
	TiXmlBase::SetCondenseWhiteSpace(true);
	for(elem = elem.begin(data.FirstChildElement()); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		reqs.erase(type);
		if(type == "title") {
			elem->GetText(&scenario.scen_name, false);
		} else if(type == "icon") {
			// TODO: This element can have some attributes on it.
			elem->GetText(&scenario.intro_pic);
			scenario.intro_mess_pic = scenario.intro_pic;
		} else if(type == "id") {
			elem->GetText(&scenario.campaign_id, false);
		} else if(type == "version") {
			elem->GetText(&val);
			int maj, min, rev;
			std::tie(maj, min, rev) = parse_version(val);
			scenario.format.ver[0] = maj;
			scenario.format.ver[1] = min;
			scenario.format.ver[2] = rev;
		} else if(type == "language") {
			// This is currently unused; there's nowhere to store the value, and it's hardcoded to "en-US" on save
		} else if(type == "author") {
			elem->FirstChildElement("name")->GetText(&scenario.contact_info[0], false);
			elem->FirstChildElement("email")->GetText(&scenario.contact_info[1], false);
		} else if(type == "feature-flags") {
			// If the function fits, use it
			scenario.feature_flags = info_from_action(*elem);
		} else if(type == "text") {
			Iterator<Element> info;
			int found_teasers = 0, found_intro = 0;
			for(info = info.begin(elem.Get()); info != info.end(); info++) {
				info->GetValue(&type);
				if(type == "teaser") {
					if(found_teasers >= 2)
						throw xBadNode(type,info->Row(),info->Column(),fname);
					info->GetText(&scenario.teaser_text[found_teasers], false);
					found_teasers++;
				} else if(type == "icon") {
					info->GetText(&scenario.intro_mess_pic);
				} else if(type == "intro-msg") {
					if(found_intro >= scenario.intro_strs.size())
						throw xBadNode(type,info->Row(),info->Column(),fname);
					info->GetText(&scenario.intro_strs[found_intro], false);
					found_intro++;
				}
			}
		} else if(type == "ratings") {
			Element* content = elem->FirstChildElement("content");
			Element* difficulty = elem->FirstChildElement("difficulty");
			content->GetText(&scenario.rating);
			difficulty->GetText(&scenario.difficulty);
			if(scenario.difficulty < 1 || scenario.difficulty > 4)
				throw xBadVal("difficulty", xBadVal::CONTENT, std::to_string(scenario.difficulty), difficulty->Row(),difficulty->Column(), fname);
			scenario.difficulty--;
		} else if(type == "flags") {
			std::set<std::string> reqs = {"adjust-difficulty", "custom-graphics", "legacy"};
			Iterator<Element> flag;
			for(flag = flag.begin(elem.Get()); flag != flag.end(); flag++) {
				flag->GetValue(&type);
				reqs.erase(type);
				if(type == "adjust-difficulty") {
					flag->GetText(&val);
					scenario.adjust_diff = val == "true";
				} else if(type == "custom-graphics") {
					flag->GetText(&val);
					scenario.uses_custom_graphics = val == "true";
				} else if(type == "legacy") {
					flag->GetText(&val);
					scenario.is_legacy = val == "true";
				} else throw xBadNode(type,flag->Row(),flag->Column(),fname);
			}
			if(!reqs.empty())
				throw xMissingElem("flags", *reqs.begin(), elem->Row(), elem->Column(), fname);
		} else if(type == "creator") {
			elem->FirstChildElement("version")->GetText(&val);
			int maj, min, rev;
			std::tie(maj, min, rev) = parse_version(val);
			scenario.format.prog_make_ver[0] = maj;
			scenario.format.prog_make_ver[1] = min;
			scenario.format.prog_make_ver[2] = rev;
			std::string type;
			Element* tp_elem = elem->FirstChildElement("type");
			tp_elem->GetText(&type);
			std::transform(type.begin(), type.end(), type.begin(), tolower);
			if( type != "oboe")
				throw xBadVal("creator", "type", type, tp_elem->Row(), tp_elem->Column(), fname);
			// TODO: Check OS field?
		} else if(type == "game") {
			std::set<std::string> reqs = {
				"num-towns", "out-width", "out-height", "start-town", "town-start", "outdoor-start", "sector-start",
			};
			Iterator<Element> game;
			int town_mods = 0, spec_items = 0, quests = 0, shops = 0, timers = 0, strnum;
			for(game = game.begin(elem.Get()); game != game.end(); game++) {
				game->GetValue(&type);
				reqs.erase(type);
				if(type == "num-towns") {
					int num;
					game->GetText(&num);
					scenario.towns.resize(num);
				} else if(type == "out-width") {
					int w;
					game->GetText(&w);
					scenario.outdoors.resize(w, scenario.outdoors.height());
				} else if(type == "out-height") {
					int h;
					game->GetText(&h);
					scenario.outdoors.resize(scenario.outdoors.width(), h);
				} else if(type == "on-init") {
					game->GetText(&scenario.init_spec);
				} else if(type == "start-town") {
					game->GetText(&scenario.which_town_start);
				} else if(type == "town-start") {
					scenario.where_start = readLocFromXml(*game);
				} else if(type == "outdoor-start") {
					scenario.out_sec_start = readLocFromXml(*game);
				} else if(type == "sector-start") {
					scenario.out_start = readLocFromXml(*game);
				} else if(type == "store-items") {
					short town = 0;
					rectangle rect = readRectFromXml(*game, "", "town", &town);
					if(scenario.store_item_rects.find(town) != scenario.store_item_rects.end())
						throw xBadVal(type, "town", std::to_string(town), game->Row(), game->Column(), fname);
					scenario.store_item_rects[town] = rect;
				} else if(type == "town-flag") {
					if(town_mods >= 10)
						throw xBadNode(type,game->Row(),game->Column(),fname);
					auto town_p = &scenario.town_mods[town_mods].spec;
					location& loc = scenario.town_mods[town_mods];
					loc = readLocFromXml(*game, "add-", "town", town_p);
					town_mods++;
				} else if(type == "special-item") {
					scenario.special_items.emplace_back();
					readSpecItemFromXml(*game, scenario.special_items[spec_items]);
					spec_items++;
				} else if(type == "quest") {
					scenario.quests.emplace_back();
					readQuestFromXml(*game, scenario.quests[quests]);
					quests++;
				} else if(type == "shop") {
					scenario.shops.emplace_back();
					readShopFromXml(*game, scenario.shops[shops]);
					shops++;
				} else if(type == "timer") {
					if(timers >= 20)
						throw xBadNode(type,game->Row(),game->Column(),fname);
					readTimerFromXml(*game, scenario.scenario_timers[timers]);
					timers++;
				} else if(type == "string") {
					game->GetAttribute("id", &strnum);
					if(strnum >= scenario.spec_strs.size())
						scenario.spec_strs.resize(strnum + 1);
					game->GetText(&scenario.spec_strs[strnum], false);
				} else if(type == "journal") {
					game->GetAttribute("id", &strnum);
					if(strnum >= scenario.journal_strs.size())
						scenario.journal_strs.resize(strnum + 1);
					game->GetText(&scenario.journal_strs[strnum], false);
				} else if(type == "vehicle") {
					std::string vtype = game->GetAttribute("type");
					if(vtype != "boat" && vtype != "horse")
						throw xBadVal(type, "type", vtype, game->Row(), game->Column(), fname);
					auto& list = (vtype == "boat" ? scenario.boats : scenario.horses);
					game->GetAttribute("id", &strnum);
					list.resize(strnum);
					int i = strnum - 1;
					Iterator<Element> vehicle;
					for(vehicle = vehicle.begin(game.Get()); vehicle != vehicle.end(); vehicle++) {
						vehicle->GetValue(&type);
						if(type == "name") {
							vehicle->GetText(&list[i].name);
						} else if(type == "pic") {
							vehicle->GetText(&list[i].pic);
						} else throw xBadNode(type, vehicle->Row(), vehicle->Column(), fname);
					}
				} else throw xBadNode(type, game->Row(), game->Column(), fname);
			}
			if(!reqs.empty())
				throw xMissingElem("game", *reqs.begin(), elem->Row(), elem->Column(), fname);
		} else if(type == "editor") {
			std::set<std::string> reqs = {"default-ground"};
			Iterator<Element> edit;
			int num_storage = 0, num_pics = 0;
			for(edit = edit.begin(elem.Get()); edit != edit.end(); edit++) {
				edit->GetValue(&type);
				reqs.erase(type);
				if(type == "default-ground") {
					edit->GetText(&scenario.default_ground);
				}
				// Old scenario files may have last-out-section and last-town in scenario.xml
				else if(type == "last-out-section") {
					scenario.editor_state.last_out_edited = readLocFromXml(*edit);
				} else if(type == "last-town") {
					edit->GetText(&scenario.editor_state.last_town_edited);
				}
				else if(type == "sound") {
					int sndnum = 0;
					edit->GetAttribute("id", &sndnum);
					if(sndnum < 100)
						throw xBadVal(type, "id", std::to_string(sndnum), edit->Row(), edit->Column(), fname);
					sndnum -= 100;
					if(sndnum >= scenario.snd_names.size())
						scenario.snd_names.resize(sndnum + 1);
					edit->GetText(&scenario.snd_names[sndnum], false);
				} else if(type == "event") {
					int evtnum = 0;
					edit->GetAttribute("id", &evtnum);
					if(evtnum > scenario.evt_names.size())
						scenario.evt_names.resize(evtnum);
					edit->GetText(&scenario.evt_names[evtnum - 1], false);
				} else if(type == "item-class") {
					int icnum = 0;
					edit->GetAttribute("id", &icnum);
					if(icnum > scenario.ic_names.size())
						scenario.ic_names.resize(icnum);
					edit->GetText(&scenario.ic_names[icnum - 1], false);
				} else if(type == "item-typeflag") {
					int itfnum = 0;
					edit->GetAttribute("id", &itfnum);
					if(itfnum > scenario.itf_names.size())
						scenario.itf_names.resize(itfnum);
					edit->GetText(&scenario.itf_names[itfnum - 1], false);
				} else if(type == "job-board") {
					int qbnum = 0;
					edit->GetAttribute("id", &qbnum);
					if(qbnum > scenario.qb_names.size())
						scenario.qb_names.resize(qbnum);
					edit->GetText(&scenario.qb_names[qbnum - 1], false);
				} else if(type == "sdf") {
					int row, col;
					edit->GetAttribute("row", &row);
					if(row < 0 || row >= SDF_ROWS)
						throw xBadVal(type, "row", std::to_string(row), edit->Row(), edit->Column(), fname);
					edit->GetAttribute("col", &col);
					if(col < 0 || col >= SDF_COLUMNS)
						throw xBadVal(type, "col", std::to_string(col), edit->Row(), edit->Column(), fname);
					edit->GetText(&scenario.sdf_names[row][col]);
				} else if(type == "graphics") {
					static const std::set<ePicType> valid_pictypes = {
						PIC_TER, PIC_TER_ANIM, PIC_TER_MAP,
						PIC_MONST,  PIC_MONST_WIDE, PIC_MONST_TALL, PIC_MONST_LG,
						PIC_DLOG, PIC_DLOG_LG, PIC_ITEM,
						PIC_TALK, PIC_BOOM, PIC_MISSILE, PIC_NONE
					};
					if(num_pics > 0)
						throw xBadNode(type, edit->Row(), edit->Column(), fname);
					scenario.custom_graphics.clear();
					Iterator<Element> pic;
					for(pic = pic.begin(edit.Get()); pic != pic.end(); pic++) {
						pic->GetValue(&type);
						if(type != "pic")
							throw xBadNode(type, pic->Row(), pic->Column(), fname);
						int i = -1;
						ePicType pictype;
						pic->GetText(&pictype);
						if(pictype == PIC_FULL) pictype = PIC_NONE; // As a special case, treat FULL as equivalent to NONE
						if(pictype == PIC_NONE) continue;
						for(attr = attr.begin(pic.Get()); attr != attr.end(); attr++) {
							attr->GetName(&name);
							if(name != "index")
								throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
							attr->GetValue(&i);
							if(i >= scenario.custom_graphics.size())
								scenario.custom_graphics.resize(i + 1, PIC_FULL);
						}
						if(i < 0)
							throw xMissingAttr(type, "index", pic->Row(), pic->Column(), fname);
						if(!valid_pictypes.count(pictype))
							throw xBadVal(type, xBadVal::CONTENT, pic->GetText(), pic->Row(), pic->Column(), fname);
						scenario.custom_graphics[i] = pictype;
						num_pics++;
					}
				} else if(type == "storage") {
					if(num_storage >= 10)
						throw xBadNode(type, edit->Row(), edit->Column(), fname);
					int num_items = 0;
					std::set<std::string> reqs = {"on-terrain", "is-property"};
					Iterator<Element> store;
					for(store = store.begin(edit.Get()); store != store.end(); store++) {
						store->GetValue(&type);
						reqs.erase(type);
						if(type == "on-terrain") {
							store->GetText(&scenario.storage_shortcuts[num_storage].ter_type);
						} else if(type == "is-property") {
							store->GetText(&val);
							if(val == "true")
								scenario.storage_shortcuts[num_storage].property = true;
						} else if(type == "item") {
							if(num_items >= 10)
								throw xBadNode(type, store->Row(), store->Column(), fname);
							store->GetText(&scenario.storage_shortcuts[num_storage].item_num[num_items]);
							bool found_chance = false;
							for(attr = attr.begin(store.Get()); attr != attr.end(); attr++) {
								attr->GetName(&name);
								if(name == "chance") {
									found_chance = true;
									attr->GetValue(&scenario.storage_shortcuts[num_storage].item_odds[num_items]);
								} else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
							}
							if(!found_chance)
								throw xMissingAttr(type, "chance", store->Row(), store->Column(), fname);
							num_items++;
						} else throw xBadNode(type, store->Row(), store->Column(), fname);
					}
					if(!reqs.empty())
						throw xMissingElem("storage", *reqs.begin(), edit->Row(), edit->Column(), fname);
					num_storage++;
				} else throw xBadNode(type, edit->Row(), edit->Column(), fname);
			}
			if(!reqs.empty())
				throw xMissingElem("editor", *reqs.begin(), elem->Row(), elem->Column(), fname);
		} else throw xBadNode(type,elem->Row(),elem->Column(),fname);
	}
	if(!reqs.empty())
		throw xMissingElem("scenario", *reqs.begin(), data.FirstChildElement()->Row(), data.FirstChildElement()->Column(), fname);
}

void readEditorStateFromXml(ticpp::Document&& data, cScenario& scenario) {
	using namespace ticpp;
	int maj, min, rev;
	std::string fname, type, name, val;
	editor_state_t& editor_state = scenario.editor_state;
	initialXmlRead(data, "editor", maj, min, rev, fname);
	Iterator<Attribute> attr;
	Iterator<Element> elem;

	Iterator<Element> child;
	std::string child_type;
	short num;
	location section;
	location center;
	short viewing_mode;

	for(elem = elem.begin(data.FirstChildElement()); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		if(type == "last-out-section") {
			editor_state.last_out_edited = readLocFromXml(*elem);
		}else if(type == "last-town") {
			elem->GetText(&editor_state.last_town_edited);
		}else if(type == "drawing"){
			editor_state.drawing = str_to_bool(elem->GetText());
		}else if(type == "editing-town"){
			editor_state.editing_town = str_to_bool(elem->GetText());
		}else if(type == "town-view-state"){
			num = std::stoi(elem->GetAttribute("num"));
			for(child = child.begin(elem.Get()); child != child.end(); child++){
				child->GetValue(&child_type);
				if(child_type == "center"){
					center = readLocFromXml(*child);
				}else if(child_type == "viewing-mode"){
					child->GetText(&viewing_mode);
				}
			}
			editor_state.town_view_state[num] = {center, viewing_mode};
		}else if(type == "out-view-state"){
			section = readLocFromXml(*elem);
			for(child = child.begin(elem.Get()); child != child.end(); child++){
				child->GetValue(&child_type);
				if(child_type == "center"){
					center = readLocFromXml(*child);
				}else if(child_type == "viewing-mode"){
					child->GetText(&viewing_mode);
				}
			}
			editor_state.out_view_state[section] = {center, viewing_mode};
		}
	}
}

void readTerrainFromXml(ticpp::Document&& data, cScenario& scenario) {
	using namespace ticpp;
	int maj, min, rev;
	std::string fname, type, name, val;
	initialXmlRead(data, "terrains", maj, min, rev, fname);
	Iterator<Attribute> attr;
	Iterator<Element> elem;
	for(elem = elem.begin(data.FirstChildElement()); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		if(type != "terrain")
			throw xBadNode(type, elem->Row(), elem->Column(), fname);
		int which_ter;
		elem->GetAttribute("id", &which_ter);
		if(which_ter >= scenario.ter_types.size())
			scenario.ter_types.resize(which_ter + 1);
		cTerrain& the_ter = scenario.ter_types[which_ter];
		the_ter = cTerrain();
		std::set<std::string> reqs = {"name", "pic", "map", "blockage", "special", "trim", "arena"};
		Iterator<Element> ter;
		for(ter = ter.begin(elem.Get()); ter != ter.end(); ter++) {
			ter->GetValue(&type);
			reqs.erase(type);
			if(type == "name") {
				ter->GetText(&the_ter.name, false);
			} else if(type == "pic") {
				ter->GetText(&the_ter.picture);
			} else if(type == "map") {
				ter->GetText(&the_ter.map_pic);
			} else if(type == "blockage") {
				ter->GetText(&the_ter.blockage);
			} else if(type == "special") {
				int num_flags = 0;
				bool found_type = false;
				Iterator<Element> spec;
				for(spec = spec.begin(ter.Get()); spec != spec.end(); spec++) {
					spec->GetValue(&type);
					if(type == "type") {
						spec->GetText(&the_ter.special);
						found_type = true;
					} else if(type == "flag") {
						if(num_flags == 0)
							spec->GetText(&the_ter.flag1);
						else if(num_flags == 1)
							spec->GetText(&the_ter.flag2);
						else if(num_flags == 2)
							spec->GetText(&the_ter.flag3);
						else throw xBadNode(type, spec->Row(), spec->Column(), fname);
						num_flags++;
					} else throw xBadNode(type, spec->Row(), spec->Column(), fname);
				}
				if(!found_type)
					throw xMissingElem("special", "type", ter->Row(), ter->Column(), fname);
			} else if(type == "transform") {
				ter->GetText(&the_ter.trans_to_what);
			} else if(type == "fly") {
				ter->GetText(&val);
				if(val == "true")
					the_ter.fly_over = true;
			} else if(type == "boat") {
				ter->GetText(&val);
				if(val == "true")
					the_ter.boat_over = true;
			} else if(type == "ride") {
				ter->GetText(&val);
				if(val != "true")
					the_ter.block_horse = true;
			} else if(type == "archetype") {
				ter->GetText(&val);
				if(val == "true")
					the_ter.is_archetype = true;
			} else if(type == "light") {
				ter->GetText(&the_ter.light_radius);
			} else if(type == "step-sound") {
				ter->GetText(&the_ter.step_sound);
			} else if(type == "trim") {
				ter->GetText(&the_ter.trim_type);
			} else if(type == "trim-for") {
				ter->GetText(&the_ter.trim_ter);
			} else if(type == "ground") {
				ter->GetText(&the_ter.ground_type);
			} else if(type == "arena") {
				ter->GetText(&the_ter.combat_arena);
			} else if(type == "editor") {
				Iterator<Element> edit;
				for(edit = edit.begin(ter.Get()); edit != edit.end(); edit++) {
					edit->GetValue(&type);
					if(type == "shortcut") {
						val.clear();
						edit->GetText(&val, false);
						the_ter.shortcut_key = val.empty() ? 0 : val[0];
					} else if(type == "frill") {
						edit->GetText(&the_ter.frill_for);
						edit->GetAttributeOrDefault("chance", &the_ter.frill_chance, 10);
					} else if(type == "object") {
						std::set<std::string> reqs = {"num", "pos", "size"};
						Iterator<Element> obj;
						for(obj = obj.begin(edit.Get()); obj != obj.end(); obj++) {
							obj->GetValue(&type);
							reqs.erase(type);
							if(type == "num") {
								obj->GetText(&the_ter.obj_num);
							} else if(type == "pos") {
								the_ter.obj_pos = readLocFromXml(*obj);
							} else if(type == "size") {
								the_ter.obj_size = readLocFromXml(*obj);
							} else throw xBadNode(type, obj->Row(), obj->Column(), fname);
						}
						if(!reqs.empty())
							throw xMissingElem("object", *reqs.begin(), edit->Row(), edit->Column(), fname);
					} else throw xBadNode(type, edit->Row(), edit->Column(), fname);
				}
			} else throw xBadNode(type, ter->Row(), ter->Column(), fname);
		}
		if(!reqs.empty())
			throw xMissingElem("terrain", *reqs.begin(), elem->Row(), elem->Column(), fname);
	}
}

void readItemsFromXml(ticpp::Document&& data, cScenario& scenario) {
	using namespace ticpp;
	int maj, min, rev;
	std::string fname, type, name, val;
	initialXmlRead(data, "items", maj, min, rev, fname);
	Iterator<Attribute> attr;
	Iterator<Element> elem;
	for(elem = elem.begin(data.FirstChildElement()); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		if(type != "item")
			throw xBadNode(type, elem->Row(), elem->Column(), fname);
		int which_item;
		elem->GetAttribute("id", &which_item);
		if(which_item >= scenario.scen_items.size())
			scenario.scen_items.resize(which_item + 1);
		cItem& the_item = scenario.scen_items[which_item];
		the_item = cItem();
		std::set<std::string> reqs = {"variety", "level", "pic", "value", "weight", "name", "full-name"};
		Iterator<Element> item;
		for(item = item.begin(elem.Get()); item != item.end(); item++) {
			item->GetValue(&type);
			reqs.erase(type);
			if(type == "variety") {
				item->GetText(&the_item.variety);
			} else if(type == "level") {
				item->GetText(&the_item.item_level);
			} else if(type == "awkward") {
				item->GetText(&the_item.awkward);
			} else if(type == "bonus") {
				item->GetText(&the_item.bonus);
			} else if(type == "protection") {
				item->GetText(&the_item.protection);
			} else if(type == "charges") {
				item->GetText(&the_item.charges);
				the_item.max_charges = the_item.charges;
			} else if(type == "weapon-type") {
				item->GetText(&the_item.weap_type);
			} else if(type == "missile-type") {
				item->GetText(&the_item.missile);
			} else if(type == "pic") {
				item->GetText(&the_item.graphic_num);
			} else if(type == "flag") {
				item->GetText(&the_item.type_flag);
			} else if(type == "value") {
				item->GetText(&the_item.value);
			} else if(type == "weight") {
				item->GetText(&the_item.weight);
			} else if(type == "class") {
				item->GetText(&the_item.special_class);
			} else if(type == "name") {
				item->GetText(&the_item.name, false);
			} else if(type == "full-name") {
				item->GetText(&the_item.full_name, false);
			} else if(type == "treasure") {
				item->GetText(&the_item.treas_class);
			} else if(type == "ability") {
				std::set<std::string> reqs = {"type", "strength", "data"};
				Iterator<Element> abil;
				for(abil = abil.begin(item.Get()); abil != abil.end(); abil++) {
					abil->GetValue(&type);
					reqs.erase(type);
					if(type == "type") {
						abil->GetText(&the_item.ability);
					} else if(type == "strength") {
						abil->GetText(&the_item.abil_strength);
					} else if(type == "data") {
						// TODO: Consider reading it as the appropriate type, depending on ability type
						abil->GetText(&the_item.abil_data.value);
					} else if(type == "use-flag") {
						abil->GetText(&the_item.magic_use_type);
					} else throw xBadNode(type, abil->Row(), abil->Column(), fname);
				}
				if(!reqs.empty())
					throw xMissingElem("ability", *reqs.begin(), item->Row(), item->Column(), fname);
			} else if(type == "properties") {
				Iterator<Element> prop;
				for(prop = prop.begin(item.Get()); prop != prop.end(); prop++) {
					prop->GetValue(&type);
					auto state = [&prop,&val]() -> bool {
						prop->GetText(&val);
						return val == "true";
					};
					if(type == "identified") {
						the_item.ident = state();
					} else if(type == "magic") {
						the_item.magic = state();
					} else if(type == "cursed") {
						the_item.cursed = state();
					} else if(type == "concealed") {
						the_item.concealed = state();
					} else if(type == "enchanted") {
						the_item.enchanted = state();
					} else if(type == "rechargeable") {
						the_item.rechargeable = state();
					} else if(type == "unsellable") {
						the_item.unsellable = state();
					} else throw xBadNode(type, prop->Row(), prop->Column(), fname);
				}
			} else if(type == "description") {
				item->GetText(&the_item.desc, false);
			} else throw xBadNode(type, item->Row(), item->Column(), fname);
		}
		if(!reqs.empty())
			throw xMissingElem("item", *reqs.begin(), elem->Row(), elem->Column(), fname);
	}
	// Once we have the items, we have to go back and fill in the shops
	for(cShop& shop : scenario.shops)
		shop.refreshItems(scenario.scen_items);
}

static std::pair<int,int> parseDice(std::string str, std::string elem, std::string attr, std::string fname, int row, int col) {
	int count = 0, sides = 0;
	bool found_d = false, found_count = false;
	for(char c : str) {
		if(isdigit(c)) {
			if(found_d) {
				sides *= 10;
				sides += c - '0';
			} else {
				found_count = true;
				count *= 10;
				count += c - '0';
			}
		} else if(!found_d && c == 'd') {
			found_d = true;
			if(!found_count) count = 1;
		}
		else throw xBadVal(elem, attr, str, row, col, fname);
	}
	if(!found_d)
		throw xBadVal(elem, attr, str, row, col, fname);
	return {count, sides};
}

static void readMonstAbilFromXml(ticpp::Element& data, cMonster& monst) {
	using namespace ticpp;
	std::string fname, type, name, val;
	data.GetDocument()->GetValue(&fname);
	data.GetValue(&type);
	if(type == "invisible") monst.invisible = true;
	else if(type == "guard") monst.guard = true;
	else {
		eMonstAbil abil_type = eMonstAbil::NO_ABIL;
		Iterator<Attribute> attr;
		for(attr = attr.begin(&data); attr != attr.end(); attr++) {
			attr->GetName(&name);
			if(name != "type")
				throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
			attr->GetValue(&val);
			attr->GetValue(&abil_type);
			if(abil_type == eMonstAbil::NO_ABIL || monst.abil[abil_type].active)
				throw xBadVal(type, name, val, attr->Row(), attr->Column(), fname);
		}
		if(abil_type == eMonstAbil::NO_ABIL)
			throw xMissingAttr(type, "type", data.Row(), data.Column(), fname);
		uAbility& abil = monst.abil[abil_type];
		abil.active = true;
		Iterator<Element> elem;
		if(type == "general") {
			if(getMonstAbilCategory(abil_type) != eMonstAbilCat::GENERAL)
				throw xBadVal(type, "type", val, data.Row(), data.Column(), fname);
			std::set<std::string> reqs = {"type", "strength", "chance"};
			if(abil_type == eMonstAbil::DAMAGE || abil_type == eMonstAbil::DAMAGE2)
				reqs.insert("extra");
			else if(abil_type == eMonstAbil::FIELD)
				reqs.insert("extra");
			else if(abil_type == eMonstAbil::STATUS || abil_type == eMonstAbil::STATUS2 || abil_type == eMonstAbil::STUN)
				reqs.insert("extra");
			auto& general = abil.gen;
			for(elem = elem.begin(&data); elem != elem.end(); elem++) {
				elem->GetValue(&type);
				reqs.erase(type);
				if(type == "type") {
					elem->GetText(&general.type);
					if(general.type != eMonstGen::TOUCH) {
						reqs.insert("missile");
						reqs.insert("range");
					}
				} else if(type == "missile") {
					elem->GetText(&general.pic);
				} else if(type == "strength") {
					elem->GetText(&general.strength);
				} else if(type == "range") {
					elem->GetText(&general.range);
				} else if(type == "extra") {
					if(abil_type == eMonstAbil::DAMAGE || abil_type == eMonstAbil::DAMAGE2)
						elem->GetText(&general.dmg);
					else if(abil_type == eMonstAbil::FIELD)
						elem->GetText(&general.fld);
					else if(abil_type == eMonstAbil::STATUS || abil_type == eMonstAbil::STATUS2 || abil_type == eMonstAbil::STUN)
						elem->GetText(&general.stat);
					else throw xBadNode(type, elem->Row(), elem->Column(), fname);
				} else if(type == "chance") {
					long double percent;
					elem->GetText(&percent);
					general.odds = percent * 10;
				} else throw xBadNode(type, elem->Row(), elem->Column(), fname);
			}
			if(!reqs.empty())
				throw xMissingElem("general", *reqs.begin(), data.Row(), data.Column(), fname);
		} else if(type == "missile") {
			if(getMonstAbilCategory(abil_type) != eMonstAbilCat::MISSILE)
				throw xBadVal(type, "type", val, data.Row(), data.Column(), fname);
			std::set<std::string> reqs = {"type", "missile", "strength", "skill", "range", "chance"};
			auto& missile = abil.missile;
			for(elem = elem.begin(&data); elem != elem.end(); elem++) {
				elem->GetValue(&type);
				reqs.erase(type);
				if(type == "type") {
					elem->GetText(&missile.type);
				} else if(type == "missile") {
					elem->GetText(&missile.pic);
				} else if(type == "strength") {
					elem->GetText(&val);
					std::tie(missile.dice, missile.sides) = parseDice(val, type, xBadVal::CONTENT, fname, elem->Row(), elem->Column());
				} else if(type == "skill") {
					elem->GetText(&missile.skill);
				} else if(type == "range") {
					elem->GetText(&missile.range);
				} else if(type == "chance") {
					long double percent;
					elem->GetText(&percent);
					missile.odds = percent * 10;
				} else throw xBadNode(type, elem->Row(), elem->Column(), fname);
			}
			if(!reqs.empty())
				throw xMissingElem("missile", *reqs.begin(), data.Row(), data.Column(), fname);
		} else if(type == "summon") {
			if(getMonstAbilCategory(abil_type) != eMonstAbilCat::SUMMON)
				throw xBadVal(type, "type", val, data.Row(), data.Column(), fname);
			std::set<std::string> reqs = {"type+what", "min", "max", "duration", "chance"};
			auto& summon = abil.summon;
			for(elem = elem.begin(&data); elem != elem.end(); elem++) {
				elem->GetValue(&type);
				reqs.erase(type);
				if(type == "min") {
					elem->GetText(&summon.min);
				} else if(type == "max") {
					elem->GetText(&summon.max);
				} else if(type == "duration") {
					elem->GetText(&summon.len);
				} else if(type == "chance") {
					long double percent;
					elem->GetText(&percent);
					summon.chance = percent * 10;
				} else {
					if(type == "type" || type == "lvl") {
						elem->GetText(&summon.what);
					} else if(type == "race") {
						eRace race;
						elem->GetText(&race);
						summon.what = (mon_num_t) race;
					} else throw xBadNode(type, elem->Row(), elem->Column(), fname);
					reqs.erase("type+what");
					summon.type = boost::lexical_cast<eMonstSummon>(type);
				}
			}
			if(!reqs.empty())
				throw xMissingElem("summon", *reqs.begin(), data.Row(), data.Column(), fname);
		} else if(type == "radiate") {
			if(getMonstAbilCategory(abil_type) != eMonstAbilCat::RADIATE)
				throw xBadVal(type, "type", val, data.Row(), data.Column(), fname);
			std::set<std::string> reqs = {"type", "chance"};
			auto& radiate = abil.radiate;
			radiate.pat = PAT_SQ; // Default radiate pattern is 3x3 square
			for(elem = elem.begin(&data); elem != elem.end(); elem++) {
				elem->GetValue(&type);
				reqs.erase(type);
				if(type == "type") {
					elem->GetText(&radiate.type);
				} else if(type == "pattern") {
					elem->GetText(&radiate.pat);
				} else if(type == "chance") {
					elem->GetText(&radiate.chance);
				} else throw xBadNode(type, elem->Row(), elem->Column(), fname);
			}
			if(!reqs.empty())
				throw xMissingElem("radiate", *reqs.begin(), data.Row(), data.Column(), fname);
		} else if(type == "special") {
			if(getMonstAbilCategory(abil_type) != eMonstAbilCat::SPECIAL)
				throw xBadVal(type, "type", val, data.Row(), data.Column(), fname);
			auto& special = abil.special;
			int num_params = 0;
			for(elem = elem.begin(&data); elem != elem.end(); elem++) {
				elem->GetValue(&type);
				if(num_params >= 3 || type != "param")
					throw xBadNode(type, elem->Row(), elem->Column(), fname);
				if(num_params == 0)
					elem->GetText(&special.extra1);
				else if(num_params == 1)
					elem->GetText(&special.extra2);
				else if(num_params == 2)
					elem->GetText(&special.extra3);
				num_params++;
			}
		} else throw xBadNode(type, data.Row(), data.Column(), fname);
	}
}

void readMonstersFromXml(ticpp::Document&& data, cScenario& scenario) {
	using namespace ticpp;
	int maj, min, rev;
	std::string fname, type, name, val;
	initialXmlRead(data, "monsters", maj, min, rev, fname);
	Iterator<Attribute> attr;
	Iterator<Element> elem;
	for(elem = elem.begin(data.FirstChildElement()); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		if(type != "monster")
			throw xBadNode(type, elem->Row(), elem->Column(), fname);
		int which_monst;
		elem->GetAttribute("id", &which_monst);
		if(which_monst == 0)
			throw xBadVal(type, "id", "0", elem->Row(), elem->Column(), fname);
		if(which_monst >= scenario.scen_monsters.size())
			scenario.scen_monsters.resize(which_monst + 1);
		cMonster& the_mon = scenario.scen_monsters[which_monst];
		the_mon = cMonster();
		std::set<std::string> reqs = {
			"name", "level", "armor", "skill", "hp", "speed",
			"race", "attacks", "pic", "attitude", "immunity",
		};
		Iterator<Attribute> attr;
		Iterator<Element> monst;
		for(monst = monst.begin(elem.Get()); monst != monst.end(); monst++) {
			monst->GetValue(&type);
			reqs.erase(type);
			if(type == "name") {
				monst->GetText(&the_mon.m_name, false);
			} else if(type == "level") {
				monst->GetText(&the_mon.level);
			} else if(type == "armor") {
				monst->GetText(&the_mon.armor);
			} else if(type == "skill") {
				monst->GetText(&the_mon.skill);
			} else if(type == "hp") {
				monst->GetText(&the_mon.m_health);
			} else if(type == "speed") {
				monst->GetText(&the_mon.speed);
			} else if(type == "treasure") {
				monst->GetText(&the_mon.treasure);
			} else if(type == "mage") {
				monst->GetText(&the_mon.mu);
			} else if(type == "priest") {
				monst->GetText(&the_mon.cl);
			} else if(type == "race") {
				monst->GetText(&the_mon.m_type);
			} else if(type == "abilities") {
				Iterator<Element> abil;
				for(abil = abil.begin(monst.Get()); abil != abil.end(); abil++) {
					readMonstAbilFromXml(*abil, the_mon);
				}
			} else if(type == "attacks") {
				int num_attacks = 0;
				Iterator<Element> atk;
				for(atk = atk.begin(monst.Get()); atk != atk.end(); atk++) {
					atk->GetValue(&type);
					if(num_attacks >= the_mon.a.size() || type != "attack")
						throw xBadNode(type, atk->Row(), atk->Column(), fname);
					cMonster::cAttack& the_atk = the_mon.a[num_attacks];
					atk->GetText(&val);
					std::tie(the_atk.dice, the_atk.sides) = parseDice(val, type, xBadVal::CONTENT, fname, atk->Row(), atk->Column());
					bool found_type = false;
					for(attr = attr.begin(atk.Get()); attr != attr.end(); attr++) {
						attr->GetName(&name);
						if(name != "type")
							throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
						attr->GetValue(&the_atk.type);
						found_type = true;
					}
					if(!found_type)
						throw xMissingAttr("attack", "type", atk->Row(), atk->Column(), fname);
					num_attacks++;
				}
			} else if(type == "pic") {
				monst->GetText(&the_mon.picture_num);
				std::set<std::string> reqs = {"w", "h"};
				for(attr = attr.begin(monst.Get()); attr != attr.end(); attr++) {
					attr->GetName(&name);
					reqs.erase(name);
					if(name == "w")
						attr->GetValue(&the_mon.x_width);
					else if(name == "h")
						attr->GetValue(&the_mon.y_width);
					else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
				}
				if(!reqs.empty())
					throw xMissingAttr("pic", *reqs.begin(), monst->Row(), monst->Column(), fname);
			} else if(type == "default-face") {
				monst->GetText(&the_mon.default_facial_pic);
			} else if(type == "onsight") {
				monst->GetText(&the_mon.see_spec);
			} else if(type == "voice") {
				monst->GetText(&the_mon.ambient_sound);
			} else if(type == "summon") {
				monst->GetText(&the_mon.summon_type);
			} else if(type == "attitude") {
				monst->GetText(&the_mon.default_attitude);
			} else if(type == "immunity") {
				Iterator<Element> resist;
				for(resist = resist.begin(monst.Get()); resist != resist.end(); resist++) {
					resist->GetValue(&type);
					if(type == "all") {
						resist->GetText(&val);
						if(val == "true")
							the_mon.invuln = true;
					} else if(type == "fear") {
						resist->GetText(&val);
						if(val == "true")
							the_mon.mindless = true;
					} else if(type == "assassinate") {
						resist->GetText(&val);
						if(val == "true")
							the_mon.amorphous = true;
					} else try {
						eDamageType dmg = boost::lexical_cast<eDamageType>(type);
						resist->GetText(&the_mon.resist[dmg]);
					} catch(boost::bad_lexical_cast x) {
						throw xBadNode(type, resist->Row(), resist->Column(), fname);
					}
				}
			} else if(type == "loot") {
				std::set<std::string> reqs = {"type", "chance"};
				Iterator<Element> loot;
				for(loot = loot.begin(monst.Get()); loot != loot.end(); loot++) {
					loot->GetValue(&type);
					reqs.erase(type);
					if(type == "type") {
						loot->GetText(&the_mon.corpse_item);
					} else if(type == "chance") {
						loot->GetText(&the_mon.corpse_item_chance);
					} else throw xBadNode(type, loot->Row(), loot->Column(), fname);
				}
				if(!reqs.empty())
					throw xMissingElem("loot", *reqs.begin(), monst->Row(), monst->Column(), fname);
			} else throw xBadNode(type, monst->Row(), monst->Column(), fname);
		}
		if(!reqs.empty())
			throw xMissingElem("monster", *reqs.begin(), elem->Row(), elem->Column(), fname);
	}
}

void readOutdoorsFromXml(ticpp::Document&& data, cOutdoors& out) {
	using namespace ticpp;
	int maj, min, rev;
	std::string fname, type, name, val;
	initialXmlRead(data, "sector", maj, min, rev, fname);
	int num_rects = 0, num_encs = 0, num_wand = 0;
	bool found_name = false;
	Iterator<Element> elem;
	for(elem = elem.begin(data.FirstChildElement()); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		if(type == "name") {
			elem->GetText(&out.name, false);
			found_name = true;
		} else if(type == "comment") {
			elem->GetText(&out.comment, false);
		} else if(type == "sound") {
			elem->GetText(&val);
			if(val == "birds")
				out.ambient_sound = eAmbientSound::AMBIENT_BIRD;
			else if(val == "drip")
				out.ambient_sound = eAmbientSound::AMBIENT_DRIP;
			else {
				out.ambient_sound = eAmbientSound::AMBIENT_CUSTOM;
				out.out_sound = boost::lexical_cast<int>(val);
			}
		} else if(type == "encounter" || type == "wandering") {
			int& count = type == "encounter" ? num_encs : num_wand;
			if(count >= 4)
				throw xBadNode(type, elem->Row(), elem->Column(), fname);
			auto& enc_list = type == "encounter" ? out.special_enc : out.wandering;
			int num_hostile = 0, num_friendly = 0;
			Iterator<Attribute> attr;
			for(attr = attr.begin(elem.Get()); attr != attr.end(); attr++) {
				std::string name, strval;
				attr->GetName(&name);
				attr->GetValue(&strval);
				bool val = strval == "true";
				if(name == "can-flee")
					enc_list[count].cant_flee = !val;
				else if(name == "force")
					enc_list[count].forced = val;
				else throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
			}
			Iterator<Element> enc;
			for(enc = enc.begin(elem.Get()); enc != enc.end(); enc++) {
				std::string type;
				enc->GetValue(&type);
				if(type == "monster") {
					bool is_friendly = false;
					for(attr = attr.begin(enc.Get()); attr != attr.end(); attr++) {
						attr->GetName(&name);
						if(name != "friendly")
							throw xBadAttr(type, name, attr->Row(), attr->Column(), fname);
						attr->GetValue(&val);
						if(val == "true")
							is_friendly = true;
					}
					if((is_friendly && num_friendly >= 3) || (!is_friendly && num_hostile >= 7))
						throw xBadNode(name, enc->Row(), enc->Column(), fname);
					if(is_friendly) {
						enc->GetText(&enc_list[count].friendly[num_friendly]);
						num_friendly++;
					} else {
						enc->GetText(&enc_list[count].monst[num_hostile]);
						num_hostile++;
					}
				} else if(type == "onmeet") {
					enc->GetText(&enc_list[count].spec_on_meet);
				} else if(type == "onflee") {
					enc->GetText(&enc_list[count].spec_on_flee);
				} else if(type == "onwin") {
					enc->GetText(&enc_list[count].spec_on_win);
				} else if(type == "sdf") {
					location sdf = readLocFromXml(*enc);
					enc_list[count].end_spec1 = sdf.x;
					enc_list[count].end_spec2 = sdf.y;
				} else throw xBadNode(type, enc->Row(), enc->Column(), fname);
			}
			if(num_hostile + num_friendly == 0)
				throw xMissingElem(type, "monster", elem->Row(), elem->Column(), fname);
			count++;
		} else if(type == "sign") {
			int sign;
			elem->GetAttribute("id", &sign);
			if(sign >= out.sign_locs.size())
				out.sign_locs.resize(sign + 1);
			elem->GetText(&out.sign_locs[sign].text, false);
		} else if(type == "area") {
			if(num_rects >= out.area_desc.size())
				out.area_desc.resize(num_rects + 1);
			static_cast<rectangle&>(out.area_desc[num_rects]) = readRectFromXml(*elem);
			elem->GetText(&out.area_desc[num_rects].descr, false);
			num_rects++;
		} else if(type == "string") {
			int str;
			elem->GetAttribute("id", &str);
			if(str >= out.spec_strs.size())
				out.spec_strs.resize(str + 1);
			elem->GetText(&out.spec_strs[str], false);
		} else throw xBadNode(type, elem->Row(), elem->Column(), fname);
	}
	if(!found_name)
		throw xMissingElem("sector", "name", data.FirstChildElement()->Row(), data.FirstChildElement()->Column(), fname);
}

void readTownFromXml(ticpp::Document&& data, cTown*& town, cScenario& scen) {
	static const std::string dirs = "nwse";
	using namespace ticpp;
	int maj, min, rev;
	std::string fname, type, name, val;
	initialXmlRead(data, "town", maj, min, rev, fname);
	int num_cmt = 0, num_timers = 0, num_wand = 0, num_rects = 0;
	bool found_size = false;
	std::set<std::string> reqs = {"size", "name", "bounds", "difficulty", "lighting", "flags"};
	bool found_ondead = false, found_onalive = false;
	Iterator<Attribute> attr;
	Iterator<Element> elem;
	for(elem = elem.begin(data.FirstChildElement()); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		reqs.erase(type);
		if(type == "size") {
			size_t dim;
			elem->GetText(&dim);
			if(dim < 24)
				throw xBadVal(type, xBadVal::CONTENT, val, elem->Row(), elem->Column(), fname);
			else town = new cTown(scen, dim);
			found_size = true;
		} else if(!found_size) {
			throw xBadNode(type, elem->Row(), elem->Column(), fname);
		} else if(type == "name") {
			elem->GetText(&town->name, false);
		} else if(type == "comment") {
			if(num_cmt >= 3)
				throw xBadNode(type, elem->Row(), elem->Column(), fname);
			elem->GetText(&town->comment[num_cmt], false);
			num_cmt++;
		} else if(type == "bounds") {
			town->in_town_rect = readRectFromXml(*elem);
		} else if(type == "difficulty") {
			elem->GetText(&town->difficulty);
		} else if(type == "lighting") {
			elem->GetText(&town->lighting_type);
		} else if(type == "onenter") {
			elem->GetAttribute("condition", &val);
			if(!found_onalive && val == "alive") {
				elem->GetText(&town->spec_on_entry);
				found_onalive = true;
			} else if(!found_ondead && val == "dead") {
				elem->GetText(&town->spec_on_entry_if_dead);
				found_ondead = true;
			} else throw xBadVal(type, "condition", val, elem->Row(), elem->Column(), fname);
		} else if(type == "exit") {
			location loc = readLocFromXml(*elem, "", "dir", &val);
			size_t which = dirs.find_first_of(val);
			if(which == std::string::npos)
				throw xBadVal(type, "dir", val, elem->Row(), elem->Column(), fname);
			town->exits[which] = loc;
		} else if(type == "onexit") {
			elem->GetAttribute("dir", &val);
			size_t which = dirs.find_first_of(val);
			if(which == std::string::npos)
				throw xBadVal(type, "dir", val, elem->Row(), elem->Column(), fname);
			elem->GetText(&town->exits[which].spec);
		} else if(type == "onoffend") {
			elem->GetText(&town->spec_on_hostile);
		} else if(type == "timer") {
			if(num_timers >= 8)
				throw xBadNode(type, elem->Row(), elem->Column(), fname);
			readTimerFromXml(*elem, town->timers[num_timers]);
			town->timers[num_timers].node_type = eSpecCtxType::TOWN;
			num_timers++;
		} else if(type == "flags") {
			Iterator<Element> flag;
			for(flag = flag.begin(elem.Get()); flag != flag.end(); flag++) {
				flag->GetValue(&type);
				if(type == "chop") {
					if(flag->HasAttribute("day")){
						flag->GetAttribute("day", &town->town_chop_time);
					}
					if(flag->HasAttribute("event")){
						flag->GetAttribute("event", &town->town_chop_key);
					}
					flag->GetAttribute("kills", &town->max_num_monst);
				} else if(type == "hidden") {
					flag->GetText(&val);
					if(val == "true")
						town->is_hidden = true;
				} else if(type == "strong-barriers") {
					flag->GetText(&val);
					if(val == "true")
						town->strong_barriers = true;
				} else if(type == "defy-mapping") {
					flag->GetText(&val);
					if(val == "true")
						town->defy_mapping = true;
				} else if(type == "defy-scrying") {
					flag->GetText(&val);
					if(val == "true")
						town->defy_scrying = true;
				} else if(type == "tavern") {
					flag->GetText(&val);
					if(val == "true")
						town->has_tavern = true;
				} else throw xBadNode(type, flag->Row(), flag->Column(), fname);
			}
		} else if(type == "wandering") {
			int num_monst = 0;
			Iterator<Element> monst;
			for(monst = monst.begin(elem.Get()); monst != monst.end(); monst++) {
				monst->GetValue(&type);
				if(num_monst >= 4 || type != "monster")
					throw xBadNode(type, monst->Row(), monst->Column(), fname);
				monst->GetText(&town->wandering[num_wand].monst[num_monst]);
				num_monst++;
			}
			if(num_monst == 0)
				throw xMissingElem("wandering", "monster", elem->Row(), elem->Column(), fname);
			num_wand++;
		} else if(type == "sign") {
			int sign;
			elem->GetAttribute("id", &sign);
			if(sign >= town->sign_locs.size())
				town->sign_locs.resize(sign + 1);
			elem->GetText(&town->sign_locs[sign].text, false);
		} else if(type == "string") {
			int str;
			elem->GetAttribute("id", &str);
			if(str >= town->spec_strs.size())
				town->spec_strs.resize(str + 1);
			elem->GetText(&town->spec_strs[str], false);
		} else if(type == "item") {
			int which_item;
			elem->GetAttribute("id", &which_item);
			if(which_item >= town->preset_items.size())
				town->preset_items.resize(which_item + 1);
			cTown::cItem& item = town->preset_items[which_item];
			bool found_type = false;
			Iterator<Element> preset;
			for(preset = preset.begin(elem.Get()); preset != preset.end(); preset++) {
				preset->GetValue(&type);
				if(type == "type") {
					preset->GetText(&item.code);
					found_type = true;
				} else if(type == "mod") {
					preset->GetText(&item.ability);
				} else if(type == "charges") {
					preset->GetText(&item.charges);
				} else if(type == "always") {
					preset->GetText(&val);
					if(val == "true")
						item.always_there = true;
				} else if(type == "property") {
					preset->GetText(&val);
					if(val == "true")
						item.property = true;
				} else if(type == "contained") {
					preset->GetText(&val);
					if(val == "true")
						item.contained = true;
				} else throw xBadNode(type, preset->Row(), preset->Column(), fname);
			}
			if(!found_type)
				throw xMissingElem("item", "type", elem->Row(), elem->Column(), fname);
		} else if(type == "creature") {
			int who;
			elem->GetAttribute("id", &who);
			if(who >= town->creatures.size())
				town->creatures.resize(who + 1);
			cTownperson& npc = town->creatures[who];
			std::set<std::string> reqs = {"type", "attitude", "mobility"};
			Iterator<Element> monst;
			for(monst = monst.begin(elem.Get()); monst != monst.end(); monst++) {
				monst->GetValue(&type);
				reqs.erase(type);
				if(type == "type") {
					monst->GetText(&npc.number);
				} else if(type == "attitude") {
					monst->GetText(&npc.start_attitude);
				} else if(type == "mobility") {
					monst->GetText(&npc.mobility);
				} else if(type == "sdf") {
					location sdf = readLocFromXml(*monst);
					npc.spec1 = sdf.x;
					npc.spec2 = sdf.y;
				} else if(type == "encounter") {
					monst->GetText(&npc.spec_enc_code);
				} else if(type == "time") {
					monst->GetAttribute("type", &npc.time_flag);
					Element* param = monst->FirstChildElement("day", false);
					if(param) param->GetText(&npc.monster_time);
					param = monst->FirstChildElement("event", false);
					if(param) param->GetText(&npc.time_code);
				} else if(type == "face") {
					monst->GetText(&npc.facial_pic);
				} else if(type == "personality") {
					monst->GetText(&npc.personality);
				} else if(type == "onkill") {
					monst->GetText(&npc.special_on_kill);
				} else if(type == "ontalk") {
					monst->GetText(&npc.special_on_talk);
				} else throw xBadNode(type, monst->Row(), monst->Column(), fname);
			}
			if(!reqs.empty())
				throw xMissingElem("creature", *reqs.begin(), elem->Row(), elem->Column(), fname);
		} else if(type == "area") {
			if(num_rects >= town->area_desc.size())
				town->area_desc.resize(num_rects + 1);
			static_cast<rectangle&>(town->area_desc[num_rects]) = readRectFromXml(*elem);
			elem->GetText(&town->area_desc[num_rects].descr, false);
			num_rects++;
		} else throw xBadNode(type, elem->Row(), elem->Column(), fname);
	}
	if(!reqs.empty())
		throw xMissingElem("town", *reqs.begin(), data.FirstChildElement()->Row(), data.FirstChildElement()->Column(), fname);
}

void readDialogueFromXml(ticpp::Document&& data, cSpeech& talk, int town_num) {
	using namespace ticpp;
	int maj, min, rev;
	std::string fname, type, name, val;
	initialXmlRead(data, "dialogue", maj, min, rev, fname);
	int num_nodes = 0;
	Iterator<Attribute> attr;
	Iterator<Element> elem;
	for(elem = elem.begin(data.FirstChildElement()); elem != elem.end(); elem++) {
		elem->GetValue(&type);
		if(type == "personality") {
			int id;
			elem->GetAttribute("id", &id);
			if(id < town_num * 10 || id >= (town_num + 1) * 10)
				throw xBadVal(type, "id", std::to_string(id), elem->Row(), elem->Column(), fname);
			id %= 10;
			std::set<std::string> reqs = {"title", "look", "name", "job"};
			Iterator<Element> who;
			for(who = who.begin(elem.Get()); who != who.end(); who++) {
				who->GetValue(&type);
				reqs.erase(type);
				if(type == "title") {
					who->GetText(&talk.people[id].title, false);
				} else if(type == "look") {
					who->GetText(&talk.people[id].look, false);
				} else if(type == "name") {
					who->GetText(&talk.people[id].name, false);
				} else if(type == "job") {
					who->GetText(&talk.people[id].job, false);
				} else if(type == "unknown") {
					who->GetText(&talk.people[id].dunno, false);
				} else throw xBadNode(type, who->Row(), who->Column(), fname);
			}
			if(!reqs.empty())
				throw xMissingElem("personality", *reqs.begin(), elem->Row(), elem->Column(), fname);
		} else if(type == "node") {
			if(num_nodes >= talk.talk_nodes.size())
				talk.talk_nodes.resize(num_nodes + 1);
			elem->GetAttribute("for", &talk.talk_nodes[num_nodes].personality);
			int num_keys = 0, num_params = 0, num_strs = 0;
			bool got_type = false;
			Iterator<Element> node;
			for(node = node.begin(elem.Get()); node != node.end(); node++) {
				node->GetValue(&type);
				if(type == "keyword") {
					val.clear();
					node->GetText(&val, false);
					while(val.length() < 4) val += 'x';
					if(num_keys == 0)
						std::copy(val.begin(), val.begin() + 4, talk.talk_nodes[num_nodes].link1);
					else if(num_keys == 1)
						std::copy(val.begin(), val.begin() + 4, talk.talk_nodes[num_nodes].link2);
					else throw xBadNode(type, node->Row(), node->Column(), fname);
					num_keys++;
				} else if(type == "type") {
					node->GetText(&talk.talk_nodes[num_nodes].type);
					got_type = true;
				} else if(type == "param") {
					if(num_params >= 4)
						throw xBadNode(type, node->Row(), node->Column(), fname);
					node->GetText(&talk.talk_nodes[num_nodes].extras[num_params]);
					num_params++;
				} else if(type == "text") {
					if(num_strs == 0)
						node->GetText(&talk.talk_nodes[num_nodes].str1, false);
					else if(num_strs == 1)
						node->GetText(&talk.talk_nodes[num_nodes].str2, false);
					else throw xBadNode(type, node->Row(), node->Column(), fname);
					num_strs++;
				} else throw xBadNode(type, node->Row(), node->Column(), fname);
			}
			if(num_keys == 0)
				throw xMissingElem("node", "keyword", elem->Row(), elem->Column(), fname);
			if(!got_type)
				throw xMissingElem("node", "type", elem->Row(), elem->Column(), fname);
			if(num_strs == 0)
				throw xMissingElem("node", "text", elem->Row(), elem->Column(), fname);
			num_nodes++;
		} else throw xBadNode(type, elem->Row(), elem->Column(), fname);
	}
}

void loadOutMapData(map_data&& data, location which, cScenario& scen) {
	cOutdoors& out = *scen.outdoors[which.x][which.y];
	int num_towns = 0;
	for(int x = 0; x < 48; x++) {
		for(int y = 0; y < 48; y++) {
			out.terrain[x][y] = data.get(x,y);
			auto features = data.getFeatures(x,y);
			for(auto feat : features) {
				bool is_boat = false;
				cVehicle* what;
				switch(feat.first) {
						// Special values
					case eMapFeature::NONE:
						break;
						// Town-only features
					case eMapFeature::ENTRANCE_EAST: case eMapFeature::ENTRANCE_NORTH: case eMapFeature::ENTRANCE_SOUTH:
					case eMapFeature::ENTRANCE_WEST: case eMapFeature::ITEM: case eMapFeature::CREATURE:
						break;
					case eMapFeature::TOWN:
						out.city_locs.emplace_back();
						out.city_locs[num_towns].x = x;
						out.city_locs[num_towns].y = y;
						out.city_locs[num_towns].spec = feat.second;
						num_towns++;
						break;
					case eMapFeature::SPECIAL_NODE:
						out.special_locs.push_back({x, y, feat.second});
						break;
					case eMapFeature::BOAT:
						is_boat = true;
						BOOST_FALLTHROUGH;
					case eMapFeature::HORSE:
						(is_boat ? scen.boats : scen.horses).resize(abs(feat.second));
						what = &(is_boat ? scen.boats : scen.horses)[abs(feat.second) - 1];
						what->which_town = 200;
						what->sector = which;
						what->loc = loc(x,y);
						what->property = feat.second < 0;
						what->exists = true;
						break;
					case eMapFeature::FIELD:
						if(feat.second == SPECIAL_SPOT)
							out.special_spot[x][y] = true;
						else if(feat.second == SPECIAL_ROAD)
							out.roads[x][y] = true;
						else throw xMapParseError(map_out_bad_field, feat.second, y, x, data.file);
						break;
					case eMapFeature::SIGN:
						if(feat.second >= out.sign_locs.size())
							break;
						static_cast<location&>(out.sign_locs[feat.second]) = loc(x,y);
						break;
					case eMapFeature::WANDERING:
						if(feat.second < 0 || feat.second >= 4)
							break;
						out.wandering_locs[feat.second] = loc(x,y);
						break;
				}
			}
		}
	}
}

void loadTownMapData(map_data&& data, int which, cScenario& scen) {
	cTown& town = *scen.towns[which];
	for(int x = 0; x < town.max_dim; x++) {
		for(int y = 0; y < town.max_dim; y++) {
			town.terrain(x,y) = data.get(x,y);
			auto features = data.getFeatures(x,y);
			for(auto feat : features) {
				bool is_boat = false;
				cVehicle* what;
				switch(feat.first) {
					case eMapFeature::NONE: break; // Special value
					case eMapFeature::TOWN: break; // Outdoor-only feature
					case eMapFeature::SPECIAL_NODE:
						town.special_locs.push_back({x, y, feat.second});
						break;
					case eMapFeature::BOAT:
						is_boat = true;
						BOOST_FALLTHROUGH;
					case eMapFeature::HORSE:
						(is_boat ? scen.boats : scen.horses).resize(abs(feat.second));
						what = &(is_boat ? scen.boats : scen.horses)[abs(feat.second) - 1];
						what->which_town = which;
						what->loc = loc(x,y);
						what->property = feat.second < 0;
						what->exists = true;
						break;
					case eMapFeature::SIGN:
						if(feat.second >= town.sign_locs.size())
							break;
						static_cast<location&>(town.sign_locs[feat.second]) = loc(x,y);
						break;
					case eMapFeature::WANDERING:
						if(feat.second < 0 || feat.second >= 4)
							break;
						town.wandering_locs[feat.second] = loc(x,y);
						break;
					case eMapFeature::ENTRANCE_SOUTH:
						town.start_locs[0] = loc(x,y);
						break;
					case eMapFeature::ENTRANCE_WEST:
						town.start_locs[1] = loc(x,y);
						break;
					case eMapFeature::ENTRANCE_NORTH:
						town.start_locs[2] = loc(x,y);
						break;
					case eMapFeature::ENTRANCE_EAST:
						town.start_locs[3] = loc(x,y);
						break;
					case eMapFeature::FIELD:
						town.preset_fields.emplace_back();
						town.preset_fields.back().loc = loc(x,y);
						town.preset_fields.back().type = eFieldType(feat.second);
						break;
					case eMapFeature::ITEM:
						if(feat.second >= town.preset_items.size())
							break;
						town.preset_items[feat.second].loc = loc(x,y);
						break;
					case eMapFeature::CREATURE:
						if(feat.second >= town.creatures.size())
							break;
						town.creatures[feat.second].start_loc = loc(x,y);
						break;
				}
			}
		}
	}
	// Don't forget to set up lighting!
	town.set_up_lights();
}

static void readSpecialNodesFromStream(std::istream& stream, std::vector<cSpecial>& nodes, std::string name) {
	std::string contents;
	stream.seekg(0, std::ios::end);
	contents.reserve(stream.tellg());
	stream.seekg(0, std::ios::beg);
	contents.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
	auto loaded = SpecialParser().parse(contents, name);
	if(loaded.size() == 0) return; // If there were no nodes, we're already done here.
	nodes.resize(loaded.rbegin()->first + 1);
	for(auto p : loaded)
		nodes[p.first] = p.second;
}

extern std::string scenario_temp_dir_name;
bool load_scenario_v2(fs::path file_to_load, cScenario& scenario, eLoadScenario load_type) {
	// First determine whether we're dealing with a packed or unpacked scenario.
	bool is_packed = true;
	tarball pack;
	std::ifstream fin;
	if(!fs::exists(file_to_load)) {
		showError("The scenario could not be found.");
		return false;
	}
	if(fs::is_directory(file_to_load)) { // Unpacked
		is_packed = false;
	} else { // Packed
		igzstream gzin(file_to_load.string().c_str());
		pack.readFrom(gzin);
		if(gzin.bad()) {
			showError("There was an error loading the scenario.");
			return false;
		}
	}
	auto hasFile = [&](std::string relpath) -> bool {
		if(is_packed) return pack.hasFile("scenario/" + relpath);
		return fs::exists(file_to_load/relpath);
	};
	auto getFile = [&](std::string relpath) -> std::istream& {
		if(is_packed) return pack.getFile("scenario/" + relpath);
		if(fin.is_open()) fin.close();
		fin.clear();
		fin.open((file_to_load/relpath).string().c_str());
		// Yes, we're returning a reference to a local variable, but it's safe here,
		// because the local is in the enclosing scope and this lambda exists within the same scope.
		#ifdef __clang__
			#pragma clang diagnostic push
			#pragma clang diagnostic ignored "-Wreturn-stack-address"
		#endif
		return fin;
		#ifdef __clang__
			#pragma clang diagnostic pop
		#endif
	};
	scenario.scen_file = file_to_load;
	// From here on, we don't have to care about whether it's packed or unpacked.
	TiXmlBase::SetCondenseWhiteSpace(true); // Make sure this is enabled, because the dialog engine disables it
	{
		// First, load up the binary header data.
		std::istream& header = getFile("header.exs");
		header.read(reinterpret_cast<char*>(&scenario.format), sizeof(scenario_header_flags));
		
		// Then, the main scenario data.
		std::istream& scen_data = getFile("scenario.xml");
		readScenarioFromXml(xmlDocFromStream(scen_data, "scenario.xml"), scenario);
		
		if(load_type == eLoadScenario::ONLY_HEADER) return true;
		if(load_type != eLoadScenario::SAVE_PREVIEW){
			// Editor state, even though the game won't need it
			if(hasFile("editor.xml")){
				std::istream& editor = getFile("editor.xml");
				readEditorStateFromXml(xmlDocFromStream(editor, "editor.xml"), scenario);
			}

			// Next, terrain types...
			std::istream& terrain = getFile("terrain.xml");
			readTerrainFromXml(xmlDocFromStream(terrain, "terrain.xml"), scenario);
			
			// ...items...
			std::istream& items = getFile("items.xml");
			readItemsFromXml(xmlDocFromStream(items, "items.xml"), scenario);
			
			// ...and monsters.
			std::istream& monsters = getFile("monsters.xml");
			readMonstersFromXml(xmlDocFromStream(monsters, "monsters.xml"), scenario);
			
			// Finally, the special nodes.
			std::istream& nodes = getFile("scenario.spec");
			readSpecialNodesFromStream(nodes, scenario.scen_specials, "scenario.spec");
		}
	}
	
	// Next, read the outdoors. Note that the space has already been reserved for them.
	for(size_t x = 0; x < scenario.outdoors.width(); x++) {
		for(size_t y = 0; y < scenario.outdoors.height(); y++) {
			scenario.outdoors[x][y] = new cOutdoors(scenario);
			std::string file_basename = "out" + std::to_string(x) + '~' + std::to_string(y);
			// First the main data.
			std::istream& outdoors = getFile("out/" + file_basename + ".xml");
			readOutdoorsFromXml(xmlDocFromStream(outdoors, file_basename + ".xml"), *scenario.outdoors[x][y]);
			
			if(load_type != eLoadScenario::SAVE_PREVIEW){
				// Then the map.
				std::istream& out_map = getFile("out/" + file_basename + ".map");
				loadOutMapData(load_map(out_map, false, file_basename + ".map"), loc(x,y), scenario);
				
				// And the special nodes.
				std::istream& out_spec = getFile("out/" + file_basename + ".spec");
				readSpecialNodesFromStream(out_spec, scenario.outdoors[x][y]->specials, file_basename + ".spec");
			}
		}
	}
	
	// And finally, the towns. Again, the space has already been reserved - that's how we know how many there are.
	for(size_t i = 0; i < scenario.towns.size(); i++) {
		std::string file_basename = "town" + std::to_string(i);
		// First the main data.
		std::istream& town = getFile("towns/" + file_basename + ".xml");
		readTownFromXml(xmlDocFromStream(town, file_basename + ".xml"), scenario.towns[i], scenario);
		
		if(load_type != eLoadScenario::SAVE_PREVIEW){
			// Then the map.
			std::istream& town_map = getFile("towns/" + file_basename + ".map");
			loadTownMapData(load_map(town_map, true, file_basename + ".map"), i, scenario);
			
			// And the special nodes.
			std::istream& town_spec = getFile("towns/" + file_basename + ".spec");
			readSpecialNodesFromStream(town_spec, scenario.towns[i]->specials, file_basename + ".spec");
			
			// Don't forget the dialogue nodes.
			std::istream& town_talk = getFile("towns/talk" + std::to_string(i) + ".xml");
			readDialogueFromXml(xmlDocFromStream(town_talk, "talk.xml"), scenario.towns[i]->talking, i);
		}
	}
	
	if(load_type == eLoadScenario::SAVE_PREVIEW) return true;
	
	// One last thing - custom graphics and sounds.
	// First figure out where they are in the filesystem. The implementation of this depends on whether the scenario is packed.
	int num_graphic_sheets = 0;
	if(is_packed) {
		fs::remove_all(tempDir/scenario_temp_dir_name);
		std::bitset<65536> have_pic = {0};
		for(auto& file : pack) {
			std::string fname = file.filename;
			int dot = fname.find_last_of('.');
			if(dot == std::string::npos)
				continue; // No file extension? Can't be important.
			if(fname.substr(0,23) == "scenario/graphics/sheet") {
				if(fname.substr(dot,4) != ".png") continue;
				if(!std::all_of(fname.begin() + 23, fname.begin() + dot, isdigit)) continue;
				int i = boost::lexical_cast<int>(fname.substr(23, dot - 23));
				if(i >= 65536) continue;
				have_pic[i] = true;
			} else if(fname.substr(0,18) == "scenario/graphics/") {
				if(fname.substr(dot,4) != ".png") continue;
				// This would be an override sheet, one that replaces one of the preset sheets.
				// Or at least, we're going to assume it is. If it's not, there's no harm done
				// (except possibly storing a sheet that will never be used).
				// TODO: A way to edit these sheets in the scenario editor?
			} else if(fname.substr(0,19) == "scenario/sounds/SND") {
				if(fname.substr(dot,4) != ".wav") continue;
				if(!std::all_of(fname.begin() + 19, fname.begin() + dot, isdigit)) continue;
			} else continue;
			fname = fname.substr(9);
			fs::path path = tempDir/scenario_temp_dir_name/fname;
			fs::create_directories(path.parent_path());
			std::istream& f = file.contents;
			std::ofstream fout(path.string().c_str(), std::ios::binary);
			fout << f.rdbuf();
			fout.close();
		}
		// This is a bit of trickery to get it to only count the first consecutive range of sheets
		while(have_pic[num_graphic_sheets])
			num_graphic_sheets++;
		cur_scen_pushed_paths = true;
		ResMgr::graphics.pushPath(tempDir/scenario_temp_dir_name/"graphics");
		ResMgr::sounds.pushPath(tempDir/scenario_temp_dir_name/"sounds");
	} else {
		if(fs::is_directory(file_to_load/"graphics")){
			cur_scen_pushed_paths = true;
			ResMgr::graphics.pushPath(file_to_load/"graphics");
		}
		if(fs::is_directory(file_to_load/"sounds")){
			cur_scen_pushed_paths = true;
			ResMgr::sounds.pushPath(file_to_load/"sounds");
		}
		std::string fname;
		while(fname = "sheet" + std::to_string(num_graphic_sheets) + ".png", fs::exists(file_to_load/"graphics"/fname))
			num_graphic_sheets++;
	}
	load_spec_graphics_v2(num_graphic_sheets);
	return true;
}

static long get_town_offset(short which_town, legacy::scenario_data_type& scenario){
	long len_to_jump,store;
	
	len_to_jump = sizeof(scenario_header_flags);
	len_to_jump += sizeof(legacy::scenario_data_type);
	len_to_jump += sizeof(legacy::scen_item_data_type);
	for(short i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for(short i = 0; i < 100; i++)
		for(short j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	for(short i = 0; i < which_town; i++)
		for(short j = 0; j < 5; j++)
			store += (long) (scenario.town_data_size[i][j]);
	len_to_jump += store;
	
	return len_to_jump;
}

bool load_town_v1(fs::path scen_file, short which_town, cTown& the_town, legacy::scenario_data_type& scenario, std::vector<shop_info_t>& shops) {
	long len,len_to_jump = 0;
	char temp_str[256];
	legacy::town_record_type store_town;
	legacy::talking_record_type store_talk;
	legacy::big_tr_type t_d;
	legacy::ave_tr_type ave_t;
	legacy::tiny_tr_type tiny_t;
	
	FILE* file_id = fopen(scen_file.string().c_str(), "rb");
	if(file_id == nullptr) {
		showError(err_prefix + "Could not open file for reading town data.", get_file_error());
		return false;
	}
	
	len_to_jump = get_town_offset(which_town, scenario);
	if(fseek(file_id, len_to_jump, SEEK_SET) != 0) {
		showError(err_prefix + "Failure seeking to town record.", get_file_error());
		fclose(file_id);
		return false;
	}
	
	len = sizeof(legacy::town_record_type);
	if(fread(&store_town, len, 1, file_id) < 1) {
		showError(err_prefix + "Could not read town record.", get_file_error());
		fclose(file_id);
		return false;
	}
	port_town(&store_town);
	
	switch(scenario.town_size[which_town]) {
		case 0:
			len = sizeof(legacy::big_tr_type);
			fread(&t_d, len, 1, file_id);
			port_t_d(&t_d);
			the_town.import_legacy(store_town);
			the_town.import_legacy(t_d, which_town);
			break;
			
		case 1:
			len = sizeof(legacy::ave_tr_type);
			fread(&ave_t, len, 1, file_id);
			port_ave_t(&ave_t);
			the_town.import_legacy(store_town);
			the_town.import_legacy(ave_t, which_town);
			break;
			
		case 2:
			len = sizeof(legacy::tiny_tr_type);
			fread(&tiny_t, len, 1, file_id);
			port_tiny_t(&tiny_t);
			the_town.import_legacy(store_town);
			the_town.import_legacy(tiny_t, which_town);
			break;
	}
	
	the_town.is_hidden = scenario.town_hidden[which_town];
	the_town.spec_strs.resize(100);
	the_town.sign_locs.resize(20);
	the_town.area_desc.resize(16);
	for(short i = 0; i < 140; i++) {
		len = (long) (store_town.strlens[i]);
		fread(temp_str, len, 1, file_id);
		temp_str[len] = 0;
		if(i == 0) the_town.name = temp_str;
		else if(i >= 1 && i < 17)
			the_town.area_desc[i-1].descr = temp_str;
		else if(i >= 17 && i < 20)
			the_town.comment[i-17] = temp_str;
		else if(i >= 20 && i < 120)
			the_town.spec_strs[i-20] = temp_str;
		else if(i >= 120 && i < 140)
			the_town.sign_locs[i-120].text = temp_str;
	}
	
	len = sizeof(legacy::talking_record_type);
	if(fread(&store_talk, len, 1, file_id) < 1) {
		showError(err_prefix + "Could not read dialogue record.", get_file_error());
		fclose(file_id);
		return false;
	}
	port_talk_nodes(&store_talk);
	
	the_town.talking.talk_nodes.resize(60);
	for(short i = 0; i < 170; i++) {
		len = (long) (store_talk.strlens[i]);
		fread(temp_str, len, 1, file_id);
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
	
	// Do this after strings are loaded because porting shops requires access to strings
	the_town.talking.import_legacy(store_talk, shops);
	
	// And lastly, calculate lighting
	the_town.set_up_lights();
	
	if(fclose(file_id) != 0) {
		showError(err_prefix + "An error occurred while closing the file.", get_file_error());
	}
	
	return true;
}

static long get_outdoors_offset(location& which_out, legacy::scenario_data_type& scenario){
	int out_sec_num;
	long len_to_jump,store;
	out_sec_num = scenario.out_width * which_out.y + which_out.x;
	
	len_to_jump = sizeof(scenario_header_flags);
	len_to_jump += sizeof(legacy::scenario_data_type);
	len_to_jump += sizeof(legacy::scen_item_data_type);
	for(short i = 0; i < 300; i++)
		len_to_jump += (long) scenario.scen_str_len[i];
	store = 0;
	for(short i = 0; i < out_sec_num; i++)
		for(short j = 0; j < 2; j++)
			store += (long) (scenario.out_data_size[i][j]);
	len_to_jump += store;
	
	return len_to_jump;
}

//mode -> 0 - primary load  1 - add to top  2 - right  3 - bottom  4 - left
bool load_outdoors_v1(fs::path scen_file, location which_out,cOutdoors& the_out, legacy::scenario_data_type& scenario){
	long len,len_to_jump;
	char temp_str[256];
	legacy::outdoor_record_type store_out;
	
	FILE* file_id = fopen(scen_file.string().c_str(), "rb");
	if(file_id == nullptr) {
		showError(err_prefix + "Could not open file for reading outdoor data.", get_file_error());
		return false;
	}
	
	len_to_jump = get_outdoors_offset(which_out, scenario);
	if(fseek(file_id, len_to_jump, SEEK_SET) != 0) {
		showError(err_prefix + "Failure seeking to outdoor record.", get_file_error());
		fclose(file_id);
		return false;
	}
	
	len = sizeof(legacy::outdoor_record_type);
	if(fread(&store_out, len, 1, file_id) < 1) {
		showError(err_prefix + "Could not read outdoor record.", get_file_error());
		fclose(file_id);
		return false;
	}
	
	the_out.x = which_out.x;
	the_out.y = which_out.y;
	port_out(&store_out);
	the_out.import_legacy(store_out);
	the_out.spec_strs.resize(90);
	the_out.sign_locs.resize(8);
	the_out.area_desc.resize(8);
	for(short i = 0; i < 108; i++) {
		len = (long) (store_out.strlens[i]);
		fread(temp_str, len, 1, file_id);
		temp_str[len] = 0;
		if(i == 0) the_out.name = temp_str;
		else if(i == 9) the_out.comment = temp_str;
		else if(i < 9) the_out.area_desc[i-1].descr = temp_str;
		else if(i >= 10 && i < 100)
			the_out.spec_strs[i-10] = temp_str;
		else if(i >= 100 && i < 108)
			the_out.sign_locs[i-100].text = temp_str;
	}
	
	if(fclose(file_id) != 0) {
		showError(err_prefix + "Something went wrong when closing the file.", get_file_error());
	}
	return true;
}

#ifdef __APPLE__
bool tryLoadPictFromResourceFile(fs::path& gpath, sf::Image& graphics_store);
#endif

void load_spec_graphics_v1(fs::path scen_file) {
	static const char*const noGraphics = "The game will still work without the custom graphics, but some things will not look right.";
	fs::remove_all(tempDir/scenario_temp_dir_name/"graphics");
	fs::remove_all(tempDir/scenario_temp_dir_name/"sounds");
	fs::path path(scen_file);
	std::cout << "Loading scenario graphics... (" << path  << ")\n";
	// Tried path.replace_extension, but that only deleted the extension, so I have to do it manually
	std::string filename = path.stem().string();
	path = path.parent_path();
	if(spec_scen_g) spec_scen_g.clear();
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
				else showWarning("An old-style .bmp graphics file was found, but there was an error reading from the file.",noGraphics);
			}
		}
		if(foundGraphics) {
			// If we're here, we found old-style graphics.
			// This means they need an alpha channel
			graphics_store.createMaskFromColor(sf::Color::White);
			spec_scen_g.is_old = true;
			spec_scen_g.sheets.resize(1);
			spec_scen_g.numSheets = 1;
			sf::Texture sheet;
			if(sheet.loadFromImage(graphics_store)) {
				spec_scen_g.sheets[0].reset(new sf::Texture(sheet));
			} else {
				showWarning("An error occurred while converting old-style graphics into the new format.",noGraphics);
				spec_scen_g.is_old = false;
				spec_scen_g.numSheets = 0;
				spec_scen_g.sheets.clear();
			}
		}
	}
}

void load_spec_graphics_v2(int num_sheets) {
	spec_scen_g.clear();
	if(num_sheets > 0) {
		spec_scen_g.sheets.resize(num_sheets);
		spec_scen_g.numSheets = num_sheets;
	}
	while(num_sheets-- > 0) {
		std::string name = "sheet" + std::to_string(num_sheets);
		ResMgr::graphics.free(name);
		spec_scen_g.sheets[num_sheets] = &ResMgr::graphics.get(name);
	}
}
