/*
 *  scenario.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "scenario.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "oldstructs.hpp"
#include "mathutil.hpp"
#include "fileio/tagfile.hpp"

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
	is_legacy = false;
	bg_out = 10;
	bg_fight = 4;
	bg_town = 13;
	bg_dungeon = 9;
	for(short i = 0; i < town_mods.size(); i++) {
		town_mods[i].spec = -1;
	}
	for(short i = 0; i < scenario_timers.size(); i++) {
		scenario_timers[i].node = -1;
	}
	scen_name = "Scen name";
	teaser_text[0] = "An exciting adventure!";
	teaser_text[1] = "";
	contact_info[0] = "Author";
	contact_info[1] = "";
}

cScenario::cScenario(const cScenario& other)
	: difficulty(other.difficulty)
	, intro_pic(other.intro_pic)
	, feature_flags(other.feature_flags)
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
	, editor_state(other.editor_state)
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
	teaser_text[0] = other.teaser_text[0];
	teaser_text[1] = other.teaser_text[1];
	contact_info[0] = other.contact_info[0];
	contact_info[1] = other.contact_info[1];
	// Copy towns and sectors
	for(size_t i = 0; i < towns.size(); i++){
		towns[i] = new cTown(*other.towns[i]);
		towns[i]->reattach(*this);
	}
	for(size_t i = 0; i < outdoors.width(); i++){
		for(size_t j = 0; j < outdoors.height(); j++){
			outdoors[i][j] = new cOutdoors(*other.outdoors[i][j]);
			outdoors[i][j]->reattach(*this);
		}
	}
}

cScenario::cScenario(cScenario&& other) {
	swap(*this, other);
}

cScenario& cScenario::operator=(cScenario other) {
	swap(*this, other);
	return *this;
}

void swap(cScenario& lhs, cScenario& rhs) {
	using std::swap;
	swap(lhs.difficulty, rhs.difficulty);
	swap(lhs.intro_pic, rhs.intro_pic);
	swap(lhs.feature_flags, rhs.feature_flags);
	swap(lhs.default_ground, rhs.default_ground);
	swap(lhs.bg_out, rhs.bg_out);
	swap(lhs.bg_fight, rhs.bg_fight);
	swap(lhs.bg_town, rhs.bg_town);
	swap(lhs.bg_dungeon, rhs.bg_dungeon);
	swap(lhs.intro_mess_pic, rhs.intro_mess_pic);
	swap(lhs.where_start, rhs.where_start);
	swap(lhs.out_sec_start, rhs.out_sec_start);
	swap(lhs.out_start, rhs.out_start);
	swap(lhs.which_town_start, rhs.which_town_start);
	swap(lhs.init_spec, rhs.init_spec);
	swap(lhs.town_mods, rhs.town_mods);
	swap(lhs.store_item_rects, rhs.store_item_rects);
	swap(lhs.special_items, rhs.special_items);
	swap(lhs.quests, rhs.quests);
	swap(lhs.shops, rhs.shops);
	swap(lhs.uses_custom_graphics, rhs.uses_custom_graphics);
	swap(lhs.rating, rhs.rating);
	swap(lhs.custom_graphics, rhs.custom_graphics);
	swap(lhs.scen_monsters, rhs.scen_monsters);
	swap(lhs.boats, rhs.boats);
	swap(lhs.horses, rhs.horses);
	swap(lhs.ter_types, rhs.ter_types);
	swap(lhs.scenario_timers, rhs.scenario_timers);
	swap(lhs.scen_specials, rhs.scen_specials);
	swap(lhs.storage_shortcuts, rhs.storage_shortcuts);
	swap(lhs.editor_state, rhs.editor_state);
	swap(lhs.format, rhs.format);
	swap(lhs.campaign_id, rhs.campaign_id);
	swap(lhs.scen_items, rhs.scen_items);
	swap(lhs.scen_name, rhs.scen_name);
	swap(lhs.teaser_text[0], rhs.teaser_text[0]);
	swap(lhs.teaser_text[1], rhs.teaser_text[1]);
	swap(lhs.contact_info[0], rhs.contact_info[0]);
	swap(lhs.contact_info[1], rhs.contact_info[1]);
	swap(lhs.intro_strs, rhs.intro_strs);
	swap(lhs.journal_strs, rhs.journal_strs);
	swap(lhs.spec_strs, rhs.spec_strs);
	swap(lhs.snd_names, rhs.snd_names);
	swap(lhs.adjust_diff, rhs.adjust_diff);
	swap(lhs.is_legacy, rhs.is_legacy);
	swap(lhs.scen_file, rhs.scen_file);
	swap(lhs.outdoors, rhs.outdoors);
	for(size_t i = 0; i < lhs.outdoors.width(); i++){
		for(size_t j = 0; j < lhs.outdoors.height(); j++){
			lhs.outdoors[i][j]->reattach(lhs);
		}
	}
	for(size_t i = 0; i < rhs.outdoors.width(); i++){
		for(size_t j = 0; j < rhs.outdoors.height(); j++){
			rhs.outdoors[i][j]->reattach(rhs);
		}
	}
	swap(lhs.towns, rhs.towns);
	for(size_t i = 0; i < lhs.towns.size(); ++i){
		lhs.towns[i]->reattach(lhs);
	}
	for(size_t i = 0; i < rhs.towns.size(); ++i){
		rhs.towns[i]->reattach(rhs);
	}
	swap(lhs.evt_names, rhs.evt_names);
	swap(lhs.ic_names, rhs.ic_names);
	swap(lhs.itf_names, rhs.itf_names);
	swap(lhs.sdf_names, rhs.sdf_names);
}

cScenario::cItemStorage::cItemStorage() : ter_type(-1), property(0) {
	for(int i = 0; i < 10; i++)
		item_num[i] = -1;
	for(int i = 0; i < 10; i++)
		item_odds[i] = 0;
}

void cScenario::import_legacy(legacy::scenario_data_type& old){
	is_legacy = true;
	// TODO eventually the absence of feature flags here will replace is_legacy altogether
	feature_flags = {};
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
	for(short i = 0; i < 3; i++) {
		auto& rect = store_item_rects[old.store_item_towns[i]];
		rect.top = old.store_item_rects[i].top;
		rect.left = old.store_item_rects[i].left;
		rect.bottom = old.store_item_rects[i].bottom;
		rect.right = old.store_item_rects[i].right;
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
		horses[i].import_legacy(old.scen_horses[i]);
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
	editor_state.last_out_edited.x = old.last_out_edited.x;
	editor_state.last_out_edited.y = old.last_out_edited.y;
	editor_state.last_town_edited = old.last_town_edited;
	adjust_diff = true;
}

cScenario::cItemStorage& cScenario::cItemStorage::operator = (legacy::item_storage_shortcut_type& old){
	ter_type = old.ter_type;
	for(int i = 0; i < 10; i++) item_num[i] = old.item_num[i];
	for(int i = 0; i < 10; i++) item_odds[i] = old.item_odds[i];
	property = old.property;
	return *this;
}

void cScenario::import_legacy(legacy::scen_item_data_type& old){
	scen_items.resize(400);
	for(short i = 0; i < 400; i++)
		scen_items[i].import_legacy(old.scen_items[i]);
	for(short i = 0; i < 256; i++) {
		scen_monsters[i].m_name = old.monst_names[i];
		if(scen_monsters[i].m_type == eRace::UNDEAD && scen_monsters[i].m_name.find("Skeleton") != std::string::npos)
			scen_monsters[i].m_type = eRace::SKELETAL;
		if(scen_monsters[i].m_type == eRace::HUMANOID && scen_monsters[i].m_name.find("Goblin") != std::string::npos)
			scen_monsters[i].m_type = eRace::GOBLIN;
	}
	for(short i = 0; i < 256; i++)
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

std::string cScenario::format_ed_version() const {
	return format_version(format.prog_make_ver);
}

std::string cScenario::format_scen_version() const {
	return format_version(format.ver);
}

ter_num_t cScenario::get_ground_from_ter(ter_num_t ter) const {
	return get_ter_from_ground(ter_types[ter].ground_type);
}

ter_num_t cScenario::get_ter_from_ground(unsigned short ground) const {
	ter_num_t archetype = -1;
	for(int i = 0; i < ter_types.size(); i++)
		if(ter_types[i].ground_type == ground) {
			if(ter_types[i].is_archetype)
				return i;
			else if(archetype < 0)
				archetype = i;
		}
	return std::max(archetype, ter_num_t());
}

ter_num_t cScenario::get_trim_terrain(unsigned short ground, unsigned short trim_g, eTrimType trim) const {
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

bool cScenario::is_ter_used(ter_num_t ter) const {
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

bool cScenario::is_monst_used(mon_num_t monst) const {
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

bool cScenario::is_item_used(item_num_t item) const {
	if(item >= scen_items.size()) return false;
	for(int i = 0; i < towns.size(); i++) {
		for(int j = 0; j < towns[i]->preset_items.size(); j++) {
			if(towns[i]->preset_items[j].code == item)
				return true;
		}
	}
	return false;
}

cItem cScenario::get_stored_item(int loot) const {
	if(loot >= 0 && loot < scen_items.size())
		return scen_items[loot];
	return cItem();
}

static const short loot_min[5] = {0,0,5,50,400};
static const short loot_max[5] = {3,8,40,800,4000};

cItem cScenario::pull_item_of_type(unsigned int loot_max,short min_val,short max_val, const std::vector<eItemType>& types,bool allow_junk_treasure) const {
	// occasionally get nice item
	if(get_ran(1,0,160) == 80) {
		loot_max += 2;
		max_val += 2000;
	}
	for(short i = 0; i < 80; i++) {
		int j = get_ran(1,0,scen_items.size() - 1);
		cItem temp_i = get_stored_item(j);
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

cItem cScenario::return_treasure(int loot, bool allow_junk) const {
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
				// preset food is bread and drumstick (small object 72)
				treas = cItem(ITEM_FOOD);
				// the next 2 graphics are also food
				treas.graphic_num += get_ran(1,0,2);
				treas.item_level = get_ran(1,5,10);

				// meat of some kind:
				if(get_ran(1,0,9) == 5)
					treas.graphic_num = 123;
				if(get_ran(1,0,9) == 5)
					treas.graphic_num = 124;
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

bool cScenario::is_town_entrance_valid(spec_loc_t loc) const {
	auto towns_in_scenario = towns.size();
	return loc.spec >= 0 && loc.spec < towns_in_scenario;
}

void cScenario::writeTo(cTagFile& file) const {
	auto& page = file.add();
	for(int i = 0; i < towns.size(); i++) {
		if(towns[i]->item_taken.any())
			page["ITEMTAKEN"] << i << towns[i]->item_taken;
		for(location door : towns[i]->door_unlocked){
			page["DOORUNLOCKED"] << i << door.x << door.y;
		}
		if(towns[i]->can_find)
			page["TOWNVISIBLE"] << i;
		else page["TOWNHIDDEN"] << i;
		if(towns[i]->m_killed > 0)
			page["TOWNSLAUGHTER"] << i << towns[i]->m_killed;
	}
}

void cScenario::readFrom(const cTagFile& file){
	std::map<int, boost::dynamic_bitset<>> taken;
	std::map<int, std::vector<location>> unlocked;
	std::vector<size_t> visible, hidden, slaughter;
	auto& page = file[0];
	page["ITEMTAKEN"].extractSparse(taken);
	if(page.contains("DOORUNLOCKED")){
		short town;
		location door;
		while(page["DOORUNLOCKED"] >> town >> door.x >> door.y){
			unlocked[town].push_back(door);
		}
	}
	page["TOWNVISIBLE"].extract(visible);
	page["TOWNHIDDEN"].extract(hidden);
	page["TOWNSLAUGHTER"].extractSparse(slaughter);
	std::sort(visible.begin(), visible.end());
	std::sort(hidden.begin(), hidden.end());
	for(size_t i = 0; i < towns.size(); i++) {
		if(taken.find(i) != taken.end()) towns[i]->item_taken = taken[i];
		else towns[i]->item_taken.clear();
		if(unlocked.find(i) != unlocked.end()) towns[i]->door_unlocked = unlocked[i];
		else towns[i]->door_unlocked.clear();
		if(i < slaughter.size()) towns[i]->m_killed = slaughter[i];
		else towns[i]->m_killed = 0;
		if(std::binary_search(visible.begin(), visible.end(), i)) {
			towns[i]->can_find = true;
		}
		if(std::binary_search(hidden.begin(), hidden.end(), i)) {
			towns[i]->can_find = false;
		}
	}
}

std::vector<town_entrance_t> cScenario::find_town_entrances(int town_num) {
	std::vector<town_entrance_t> matching_entrances;
	for(int x = 0; x < outdoors.width(); ++x){
		for(int y = 0; y < outdoors.height(); ++y){
			for(spec_loc_t& entrance : outdoors[x][y]->city_locs){
				if(town_num == -1 || entrance.spec == town_num){
					matching_entrances.push_back({{x, y}, {entrance.x, entrance.y}, static_cast<int>(entrance.spec)});
				}
			}
		}
	}
	return matching_entrances;
}

bool cScenario::has_feature_flag(std::string flag) {
	return this->feature_flags.find(flag) != this->feature_flags.end();
}

std::string cScenario::get_feature_flag(std::string flag) {
	std::map<std::string, std::string>::const_iterator iter = this->feature_flags.find(flag);
	if(iter == this->feature_flags.end()) return "";
	return iter->second;
}

std::string cScenario::get_sdf_name(int row, int col) {
	if(row < 0 || row >= SDF_ROWS || col < 0 || col >= SDF_COLUMNS){
		throw "Tried to access SDF name for out-of-bounds flag (" + std::to_string(row) + ", " + std::to_string(col) + ")";
	}
	if(sdf_names.find(row) == sdf_names.end())
		return "";
	if(sdf_names[row].find(col) == sdf_names[row].end())
		return "";
	return sdf_names[row][col];
}