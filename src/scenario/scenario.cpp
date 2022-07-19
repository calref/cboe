/*
 *  scenario.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "scenario.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "mathutil.hpp"
#include "oldstructs.hpp"
#include "fileio/resmgr/res_strings.hpp"

void cScenario::reset_version() {
	format.prog_make_ver[0] = 2;
	format.prog_make_ver[1] = 0;
	format.prog_make_ver[2] = 0;
	format.flag1 = 'O'; format.flag2 = 'B';
	format.flag3 = 'O'; format.flag4 = 'E';
}

cScenario::~cScenario() {
	destroy_terrain();
}

void cScenario::destroy_terrain() {
	// Nuke towns
	if(!towns.empty()) {
		for(size_t i = 0; i < towns.size(); i++) {
			if(towns[i] != nullptr) delete towns[i];
			towns[i] = nullptr;
		}
	}
	if(!outdoors.empty()){
		for(size_t i = 0; i < outdoors.width(); i++) {
			for(size_t j = 0; j < outdoors.height(); j++) {
				if(outdoors[i][j] != nullptr) delete outdoors[i][j];
				outdoors[i][j] = nullptr;
			}
		}
	}
}

cOutdoors *cScenario::get_bad_outdoor()
{
	static std::shared_ptr<cOutdoors> badOutdoor;
	if (!badOutdoor)
		badOutdoor=std::make_shared<cOutdoors>(*this);
	badOutdoor->reattach(*this);
	badOutdoor->name="Bad Outdoor";
	return badOutdoor.get();
}

cScenario::cScenario() {
	std::string temp_str;
	
	format.ver[0] = 1;
	format.min_run_ver = 2;
	format.prog_make_ver[0] = 2;
	init_spec = -1;
	default_ground = 2;
	which_town_start = 0;
	where_start.x = 24;
	where_start.y = 24;
	out_start = where_start = loc(24,24);
	rating = eContentRating::G;
	difficulty = 0;
	intro_pic = intro_mess_pic = 0;
	adjust_diff = true;
	bg_out = 10;
	bg_fight = 4;
	bg_town = 13;
	bg_dungeon = 9;
	outdoors.set_get_default_function([this](){return get_bad_outdoor();});
	// ASAN used but unset
	is_legacy = false;
	for(short i = 0; i < town_mods.size(); i++) {
		town_mods[i].spec = -1;
	}
	for(short i = 0; i < 3; i++) {
		store_item_towns[i] = -1;
	}
	for(short i = 0; i < scenario_timers.size(); i++) {
		scenario_timers[i].node = -1;
	}
	scen_name = "Scen name";
	who_wrote[0] = "Who wrote 1";
	who_wrote[1] = "Who wrote 2";
	contact_info[0] = "Name not given";
	contact_info[1] = "Contact info";
}

cScenario::cScenario(const cScenario& other)
	: difficulty(other.difficulty)
	, intro_pic(other.intro_pic)
	, default_ground(other.default_ground)
	, bg_out(other.bg_out)
	, bg_fight(other.bg_fight)
	, bg_town(other.bg_town)
	, bg_dungeon(other.bg_dungeon)
	, intro_mess_pic(other.intro_mess_pic)
	, where_start(other.where_start)
	, out_sec_start(other.out_sec_start)
	, out_start(other.out_start)
	, which_town_start(other.which_town_start)
	, init_spec(other.init_spec)
	, town_mods(other.town_mods)
	, store_item_rects(other.store_item_rects)
	, store_item_towns(other.store_item_towns)
	, special_items(other.special_items)
	, quests(other.quests)
	, shops(other.shops)
	, uses_custom_graphics(other.uses_custom_graphics)
	, rating(other.rating)
	, custom_graphics(other.custom_graphics)
	, scen_monsters(other.scen_monsters)
	, boats(other.boats)
	, horses(other.horses)
	, ter_types(other.ter_types)
	, scenario_timers(other.scenario_timers)
	, scen_specials(other.scen_specials)
	, storage_shortcuts(other.storage_shortcuts)
	, last_out_edited(other.last_out_edited)
	, last_town_edited(other.last_town_edited)
	, format(other.format)
	, campaign_id(other.campaign_id)
	, scen_items(other.scen_items)
	, scen_name(other.scen_name)
	, intro_strs(other.intro_strs)
	, journal_strs(other.journal_strs)
	, spec_strs(other.spec_strs)
	, snd_names(other.snd_names)
	, adjust_diff(other.adjust_diff)
	, is_legacy(other.is_legacy)
	, scen_file(other.scen_file)
	, towns(other.towns.size())
	, outdoors(other.outdoors.width(), other.outdoors.height())
{
	// MSVC 12 doesn't like arrays in the initializer list. :(
	who_wrote[0] = other.who_wrote[0];
	who_wrote[1] = other.who_wrote[1];
	contact_info[0] = other.contact_info[0];
	contact_info[1] = other.contact_info[1];
	// Copy towns and sectors
	for(size_t i = 0; i < towns.size(); i++)
		towns[i] = new cTown(*other.towns[i]);
	for(size_t i = 0; i < outdoors.width(); i++)
		for(size_t j = 0; j < outdoors.height(); j++)
			outdoors[i][j] = new cOutdoors(*other.outdoors[i][j]);
	outdoors.set_get_default_function([this](){return get_bad_outdoor();});
}

cScenario::cScenario(cScenario&& other) {
	swap(other);
}

cScenario& cScenario::operator=(cScenario other) {
	swap(other);
	return *this;
}

void cScenario::swap(cScenario& other) {
	std::swap(difficulty, other.difficulty);
	std::swap(intro_pic, other.intro_pic);
	std::swap(default_ground, other.default_ground);
	std::swap(bg_out, other.bg_out);
	std::swap(bg_fight, other.bg_fight);
	std::swap(bg_town, other.bg_town);
	std::swap(bg_dungeon, other.bg_dungeon);
	std::swap(intro_mess_pic, other.intro_mess_pic);
	std::swap(where_start, other.where_start);
	std::swap(out_sec_start, other.out_sec_start);
	std::swap(out_start, other.out_start);
	std::swap(which_town_start, other.which_town_start);
	std::swap(init_spec, other.init_spec);
	std::swap(town_mods, other.town_mods);
	std::swap(store_item_rects, other.store_item_rects);
	std::swap(store_item_towns, other.store_item_towns);
	std::swap(special_items, other.special_items);
	std::swap(quests, other.quests);
	std::swap(shops, other.shops);
	std::swap(uses_custom_graphics, other.uses_custom_graphics);
	std::swap(rating, other.rating);
	std::swap(custom_graphics, other.custom_graphics);
	std::swap(scen_monsters, other.scen_monsters);
	std::swap(boats, other.boats);
	std::swap(horses, other.horses);
	std::swap(ter_types, other.ter_types);
	std::swap(scenario_timers, other.scenario_timers);
	std::swap(scen_specials, other.scen_specials);
	std::swap(storage_shortcuts, other.storage_shortcuts);
	std::swap(last_out_edited, other.last_out_edited);
	std::swap(last_town_edited, other.last_town_edited);
	std::swap(format, other.format);
	std::swap(campaign_id, other.campaign_id);
	std::swap(scen_items, other.scen_items);
	std::swap(scen_name, other.scen_name);
	std::swap(who_wrote[0], other.who_wrote[0]);
	std::swap(who_wrote[1], other.who_wrote[1]);
	std::swap(contact_info[0], other.contact_info[0]);
	std::swap(contact_info[1], other.contact_info[1]);
	std::swap(intro_strs, other.intro_strs);
	std::swap(journal_strs, other.journal_strs);
	std::swap(spec_strs, other.spec_strs);
	std::swap(snd_names, other.snd_names);
	std::swap(adjust_diff, other.adjust_diff);
	std::swap(is_legacy, other.is_legacy);
	std::swap(scen_file, other.scen_file);
	std::swap(outdoors, other.outdoors);
	std::swap(towns, other.towns);
}

cScenario::cItemStorage::cItemStorage() : ter_type(-1), property(0) {
	for(int i = 0; i < 10; i++)
		item_num[i] = -1;
	for(int i = 0; i < 10; i++)
		item_odds[i] = 0;
}

cItem const &cScenario::get_item(item_num_t item) const
{
	if (item>=0 && item<scen_items.size())
		return scen_items[item];
	static cItem badItem=cItem::bad();
	return badItem;
}

cItem &cScenario::get_item(item_num_t item)
{
	if (item>=0 && item<scen_items.size())
		return scen_items[item];
	static cItem badItem;
	badItem=cItem::bad();
	return badItem;
}

std::string &cScenario::get_journal_string(int id)
{
	   if (id>=0 && id<journal_strs.size())
			   return journal_strs[id];
	   if (id>=0 && id<200) {
		   journal_strs.resize(id+1);
			   return journal_strs[id];
	   }
	   static std::string badString;
	   badString="Bad Journal String";
	   return badString;
}

std::string const &cScenario::get_journal_string(int id) const
{
	   if (id>=0 && id<journal_strs.size())
			   return journal_strs[id];
	   static std::string badString="Bad Journal String";
	   return badString;
}

cQuest const &cScenario::get_quest(int quest) const
{
	if (quest>=0 && quest<quests.size())
		return quests[quest];
	static cQuest badQuest=cQuest::bad();
	return badQuest;
}

cQuest &cScenario::get_quest(int quest)
{
	if (quest>=0 && quest<quests.size())
		return quests[quest];
	static cQuest badQuest;
	badQuest=cQuest::bad();
	return badQuest;
}

cSpecItem const &cScenario::get_special_item(item_num_t item) const
{
	if (item>=0 && item<special_items.size())
		return special_items[item];
	static cSpecItem badItem=cSpecItem::bad();
	return badItem;
}

cSpecItem &cScenario::get_special_item(item_num_t item)
{
	if (item>=0 && item<special_items.size())
		return special_items[item];
	static cSpecItem badItem=cSpecItem::bad();
	return badItem;
}

cShop const &cScenario::get_shop(int shop) const
{
	if (shop>=0 && shop<shops.size())
		return shops[shop];
	static cShop badShop=cShop::bad();
	return badShop;

}
cShop &cScenario::get_shop(int shop)
{
	if (shop>=0 && shop<shops.size())
		return shops[shop];
	static cShop badShop;
	badShop=cShop::bad();
	return badShop;
}

std::string &cScenario::get_special_string(int id)
{
	   if (id>=0 && id<spec_strs.size())
			   return spec_strs[id];
	   if (id>=0 && id<200) {
			   spec_strs.resize(id+1);
			   return spec_strs[id];
	   }
	   static std::string badString;
	   badString="Bad Special String";
	   return badString;
}

std::string const &cScenario::get_special_string(int id) const
{
	   if (id>=0 && id<spec_strs.size())
			   return spec_strs[id];
	   static std::string badString="Bad Special String";
	   return badString;
}
 
cTerrain const &cScenario::get_terrain(ter_num_t ter) const
{
	if (ter<ter_types.size())
		return ter_types[ter];
	static cTerrain badTerrain=cTerrain::bad();
	return badTerrain;
}

cTerrain &cScenario::get_terrain(ter_num_t ter)
{
	if (ter<ter_types.size())
		return ter_types[ter];
	static cTerrain badTerrain;
	badTerrain=cTerrain::bad();
	return badTerrain;
}

void cScenario::import_legacy(legacy::scenario_data_type const &old){
	is_legacy = true;
	difficulty = old.difficulty;
	intro_pic = old.intro_pic;
	default_ground = old.default_ground * 2;
	intro_mess_pic = old.intro_pic;
	where_start.x = old.where_start.x;
	where_start.y = old.where_start.y;
	out_sec_start.x = old.out_sec_start.x;
	out_sec_start.y = old.out_sec_start.y;
	out_start.x = old.out_start.x;
	out_start.y = old.out_start.y;
	which_town_start = old.which_town_start;
	for(short i = 0; i < 10; i++) {
		town_mods[i].spec = old.town_to_add_to[i];
		town_mods[i].x = old.flag_to_add_to_town[i][0];
		town_mods[i].y = old.flag_to_add_to_town[i][1];
		storage_shortcuts[i] = old.storage_shortcuts[i];
	}
	// TODO: Combine store_item_rects and store_item_towns into a structure
	for(short i = 0; i < 3; i++) {
		store_item_rects[i].top = old.store_item_rects[i].top;
		store_item_rects[i].left = old.store_item_rects[i].left;
		store_item_rects[i].bottom = old.store_item_rects[i].bottom;
		store_item_rects[i].right = old.store_item_rects[i].right;
		store_item_towns[i] = old.store_item_towns[i];
	}
	special_items.resize(50);
	for(short i = 0; i < 50; i++) {
		special_items[i].flags = old.special_items[i];
		special_items[i].special = old.special_item_special[i];
	}
	rating = eContentRating(old.rating);
	// TODO: Is this used anywhere?
	uses_custom_graphics = old.uses_custom_graphics;
	boats.resize(30);
	horses.resize(30);
	for(short i = 0; i < 30; i++) {
		boats[i].import_legacy(old.scen_boats[i]);
		// in the .exs scenario, the boats in town seem to be defined with exists=false
		//    I suppose that this is also true for outdoor's boat
		if (boats[i].which_town>=0)
			boats[i].exists = true;
		horses[i].import_legacy(old.scen_horses[i]);
		// I suppose that this is similar for horses
		if (horses[i].which_town>=0)
			horses[i].exists = true;
	}
	ter_types.resize(256);
	scen_specials.resize(256);
	scen_monsters.resize(256);
	for(short i = 0; i < 256; i++){
		ter_types[i].i = i;
		ter_types[i].import_legacy(old.ter_types[i]);
		scen_monsters[i].import_legacy(old.scen_monsters[i]);
		scen_specials[i].import_legacy(old.scen_specials[i]);
	}
	for(short i = 0; i < 20; i++) {
		scenario_timers[i].time = old.scenario_timer_times[i];
		scenario_timers[i].node = old.scenario_timer_specs[i];
	}
	last_out_edited.x = old.last_out_edited.x;
	last_out_edited.y = old.last_out_edited.y;
	last_town_edited = old.last_town_edited;
	adjust_diff = true;
}

cScenario::cItemStorage& cScenario::cItemStorage::operator = (legacy::item_storage_shortcut_type const &old){
	ter_type = old.ter_type;
	for(int i = 0; i < 10; i++) item_num[i] = old.item_num[i];
	for(int i = 0; i < 10; i++) item_odds[i] = old.item_odds[i];
	property = old.property;
	return *this;
}

void cScenario::import_legacy(legacy::scen_item_data_type const &old){

	scen_items.resize(400);
	StringList strings = *ResMgr::strings.get("legacy-items-desc");
	std::map<std::string, std::string> fullNameToDesc;
	for (auto const &str : strings) {
		auto split = str.find('|');
		if (split == std::string::npos)
			continue;
		std::string name=str.substr(0,split);
		std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
		fullNameToDesc[name] = str.substr(split+1);
	}
	for(size_t i = 0; i < 400; i++) {
		scen_items[i].import_legacy(old.scen_items[i]);
		std::string name=scen_items[i].full_name+(scen_items[i].cursed ? "_cursed" : scen_items[i].magic ? "_magic" : "");
		std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c){ return std::tolower(c); });
		auto const &descIt=fullNameToDesc.find(name);
		if (descIt==fullNameToDesc.end())
			continue;
		if (!scen_items[i].desc.empty())
			scen_items[i].desc=descIt->second+" ("+scen_items[i].desc+")";
		else
			scen_items[i].desc=descIt->second;
	}
	for(short i = 0; i < std::min<short>(256,scen_monsters.size()); i++) {
		auto &monster=scen_monsters[i];
		monster.m_name = old.monst_names[i];
		if(monster.m_type == eRace::UNDEAD && monster.m_name.find("Skeleton") != std::string::npos)
			monster.m_type = eRace::SKELETAL;
		if(monster.m_type == eRace::HUMANOID && monster.m_name.find("Goblin") != std::string::npos)
			monster.m_type = eRace::GOBLIN;
	}
	for(short i = 0; i < std::min<short>(256, ter_types.size()); i++)
		ter_types[i].name = old.ter_names[i];
	// Some default shops - the five magic shops and the healing shop.
	cShop magic_shop(SHOP_JUNK);
	for(short i = 0; i < 5; i++)
		shops.push_back(magic_shop);
	shops.push_back(cShop(SHOP_HEALING));
}

static std::string format_version(const unsigned char(& ver)[3]) {
	std::ostringstream fmt;
	fmt << int(ver[0]) << '.' << int(ver[1]) << '.' << int(ver[2]);
	return fmt.str();
}

std::string cScenario::format_ed_version() {
	return format_version(format.prog_make_ver);
}

std::string cScenario::format_scen_version() {
	return format_version(format.ver);
}

ter_num_t cScenario::get_ground_from_ter(ter_num_t ter){
	return get_ter_from_ground(get_terrain(ter).ground_type);
}

ter_num_t cScenario::get_ter_from_ground(unsigned short ground){
	short archetype = -1;
	for(int i = 0; i < ter_types.size(); i++)
		if(ter_types[i].ground_type == ground) {
			if(ter_types[i].is_archetype)
				return i;
			else if(archetype < 0)
				archetype = i;
		}
	return archetype<0 ? ter_num_t() : archetype;
}

ter_num_t cScenario::get_trim_terrain(unsigned short ground, unsigned short trim_g, eTrimType trim) {
	for(int i = 0; i < ter_types.size(); i++) {
		if(ter_types[i].ground_type != ground)
			continue;
		if(ter_types[i].trim_ter != trim_g)
			continue;
		if(ter_types[i].trim_type != trim)
			continue;
		return i;
	}
	return 90;
}

bool cScenario::is_ter_used(ter_num_t ter) {
	if(ter >= ter_types.size()) return false;
	if(ter <= 90) return true;
	for(int sx = 0; sx < outdoors.width(); sx++) {
		for(int sy = 0; sy < outdoors.height(); sy++) {
			for(int x = 0; x < 48; x++) {
				for(int y = 0; y < 48; y++) {
					if(outdoors[sx][sy]->terrain[x][y] == ter)
						return true;
				}
			}
		}
	}
	for(int i = 0; i < towns.size(); i++) {
		for(int x = 0; x < towns[i]->max_dim; x++) {
			for(int y = 0; y < towns[i]->max_dim; y++) {
				if(towns[i]->terrain(x,y) == ter)
					return true;
			}
		}
	}
	return false;
}

bool cScenario::is_monst_used(mon_num_t monst) {
	if(monst >= scen_monsters.size()) return false;
	for(int sx = 0; sx < outdoors.width(); sx++) {
		for(int sy = 0; sy < outdoors.height(); sy++) {
			for(int i = 0; i < outdoors[sx][sy]->wandering.size(); i++) {
				for(int j = 0; j < outdoors[sx][sy]->wandering[i].monst.size(); j++) {
					if(outdoors[sx][sy]->wandering[i].monst[j] == monst)
						return true;
				}
				for(int j = 0; j < outdoors[sx][sy]->wandering[i].friendly.size(); j++) {
					if(outdoors[sx][sy]->wandering[i].friendly[j] == monst)
						return true;
				}
			}
			for(int i = 0; i < outdoors[sx][sy]->special_enc.size(); i++) {
				for(int j = 0; j < outdoors[sx][sy]->special_enc[i].monst.size(); j++) {
					if(outdoors[sx][sy]->special_enc[i].monst[j] == monst)
						return true;
				}
				for(int j = 0; j < outdoors[sx][sy]->special_enc[i].friendly.size(); j++) {
					if(outdoors[sx][sy]->special_enc[i].friendly[j] == monst)
						return true;
				}
			}
		}
	}
	for(int i = 0; i < towns.size(); i++) {
		for(int j = 0; j < towns[i]->creatures.size(); j++) {
			if(towns[i]->creatures[j].number == monst)
				return true;
		}
	}
	return false;
}

bool cScenario::is_item_used(item_num_t item) {
	if(item >= scen_items.size()) return false;
	for(int i = 0; i < towns.size(); i++) {
		for(int j = 0; j < towns[i]->preset_items.size(); j++) {
			if(towns[i]->preset_items[j].code == item)
				return true;
		}
	}
	return false;
}

static const short loot_min[5] = {0,0,5,50,400};
static const short loot_max[5] = {3,8,40,800,4000};

cItem cScenario::pull_item_of_type(unsigned int loot_max,short min_val,short max_val, const std::vector<eItemType>& types,bool allow_junk_treasure) {
	// occasionally get nice item
	if(get_ran(1,0,160) == 80) {
		loot_max += 2;
		max_val += 2000;
	}
	for(short i = 0; i < 80; i++) {
		int j = get_ran(1,0,scen_items.size() - 1);
		cItem const &temp_i = get_item(j);
		if(temp_i.variety == eItemType::NO_ITEM) continue;
		if(std::find(types.begin(), types.end(), temp_i.variety) != types.end()) {
			short val = (temp_i.charges > 0) ? temp_i.charges * temp_i.value : temp_i.value;
			if(val >= min_val && val <= max_val && (temp_i.treas_class != 0 || allow_junk_treasure) && temp_i.treas_class <= loot_max)
				return temp_i;
		}
	}
	return cItem();
}

enum eTreasureType {
	FOOD, WEAPON, ARMOR, SHIELD, HELM, MISSILE, POTION,
	SCROLL, WAND, RING, NECKLACE, POISON, GLOVES, BOOTS,
};

cItem cScenario::return_treasure(int loot, bool allow_junk) {
	static const std::vector<eItemType>
		weapon = {eItemType::ONE_HANDED,eItemType::TWO_HANDED},
		armor = {eItemType::ARMOR}, shield = {eItemType::SHIELD}, helm = {eItemType::HELM},
		missiles1 = {eItemType::ARROW,eItemType::THROWN_MISSILE,eItemType::BOW},
		missiles2 = {eItemType::CROSSBOW,eItemType::BOLTS,eItemType::MISSILE_NO_AMMO},
		scroll = {eItemType::SCROLL}, wand = {eItemType::WAND}, ring = {eItemType::RING}, necklace = {eItemType::NECKLACE},
		potion = {eItemType::POTION}, poison = {eItemType::WEAPON_POISON}, gloves = {eItemType::GLOVES}, boots = {eItemType::BOOTS};
	
	static const eTreasureType which_treas_chart[] = {
		FOOD,FOOD,FOOD,FOOD,FOOD, WEAPON,WEAPON,WEAPON,WEAPON,WEAPON,
		ARMOR,ARMOR,ARMOR,ARMOR,ARMOR, WEAPON,WEAPON,WEAPON,SHIELD,SHIELD,
		SHIELD,SHIELD,HELM,HELM,HELM, MISSILE,MISSILE,MISSILE,POTION,POTION,
		POTION,SCROLL,SCROLL,WAND,WAND, RING,NECKLACE,POISON,POISON,GLOVES,
		GLOVES,BOOTS,WAND,RING,NECKLACE, WAND,RING,NECKLACE,
	};
	
	cItem treas;
	short r1 = get_ran(1,0,41);
	if(loot >= 3)
		r1 += 3;
	if(loot == 4)
		r1 += 3;
	switch(which_treas_chart[r1]) {
		case FOOD:
			// food doesn't always appear
			if(get_ran(1,0,2) == 1) {
				treas = cItem(ITEM_FOOD);
				treas.graphic_num += get_ran(1,0,2);
				treas.item_level = get_ran(1,5,10);
				if(get_ran(1,0,9) == 5)
					treas.graphic_num = 113;
				if(get_ran(1,0,9) == 5)
					treas.graphic_num = 114;
			}
			break;
		case WEAPON:
			if(loot > 0)
				treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],weapon,allow_junk);
			break;
		case ARMOR:
			// For some reason the armor code calculated this but never used it
			//r1 = get_ran(1,(loot - 1) * 5 + 124,142);
			if(loot > 0)
				treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],armor,allow_junk);
			break;
		case SHIELD:
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],shield,allow_junk);
			break;
		case HELM:
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],helm,allow_junk);
			break;
		case MISSILE:
			if(get_ran(1,0,2) < 2)
				treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],missiles1,allow_junk);
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],missiles2,allow_junk);
			break;
		case POTION:
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot] / (get_ran(1,0,80) < 20 * (4 - loot) ? 2 : 1),potion,allow_junk);
			break;
		case SCROLL:
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],scroll,allow_junk);
			break;
		case WAND:
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],wand,allow_junk);
			break;
		case RING:
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],ring,allow_junk);
			break;
		case NECKLACE:
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],necklace,allow_junk);
			break;
		case POISON:
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],poison,allow_junk);
			break;
		case GLOVES:
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],gloves,allow_junk);
			break;
		case BOOTS:
			treas = pull_item_of_type(loot,loot_min[loot],loot_max[loot],boots,allow_junk);
			break;
	}
	if(treas.variety == eItemType::NO_ITEM)
		treas.value = 0;
	return treas;
}

cOutdoors& cScenario::get_sector(int x, int y) {
	return *outdoors[x][y];
}

cMonster &cScenario::get_monster(mon_num_t monst)
{
	if (monst<scen_monsters.size())
		return scen_monsters[monst];
	static cMonster badMonster;
	badMonster=cMonster::bad();
	return badMonster;
}

cMonster const &cScenario::get_monster(mon_num_t monst) const
{
	if (monst<scen_monsters.size())
		return scen_monsters[monst];
	static cMonster const badMonster=cMonster::bad();
	return badMonster;
}

bool cScenario::is_town_entrance_valid(spec_loc_t loc) const {
	return loc.spec >= 0 && loc.spec < towns.size();
}

void cScenario::writeTo(std::ostream& file) const {
	for(int i = 0; i < towns.size(); i++) {
		if(towns[i]->item_taken.any())
			file << "ITEMTAKEN " << i << ' ' << towns[i]->item_taken << '\n';
		if(towns[i]->can_find)
			file << "TOWNVISIBLE " << i << '\n';
		else file << "TOWNHIDDEN " << i << '\n';
		if(towns[i]->m_killed > 0)
			file << "TOWNSLAUGHTER " << i << ' ' << towns[i]->m_killed << '\n';
	}
}

void cScenario::readFrom(std::istream& file){
	// TODO: Error-check input
	// TODO: Don't call this sin, it's a trig function
	std::istringstream bin;
	std::string cur;
	getline(file, cur, '\f');
	bin.str(cur);
	while(bin) { // continue as long as no error, such as eof, occurs
		getline(bin, cur);
		std::istringstream sin(cur);
		sin >> cur;
		if(cur == "ITEMTAKEN"){
			int i;
			sin >> i;
			if(i >= 0 && i < towns.size())
				sin >> towns[i]->item_taken;
		} else if(cur == "TOWNVISIBLE") {
			int i;
			sin >> i;
			if(i >= 0 && i < towns.size())
				towns[i]->can_find = true;
		} else if(cur == "TOWNHIDDEN") {
			int i;
			sin >> i;
			if(i >= 0 && i < towns.size())
				towns[i]->can_find = false;
		} else if(cur == "TOWNSLAUGHTER"){
			int i;
			sin >> i;
			if(i >= 0 && i < towns.size())
				sin >> towns[i]->m_killed;
		}
	}
}
