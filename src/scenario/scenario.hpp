/*
 *  scenario.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef BOE_DATA_SCENARIO_H
#define BOE_DATA_SCENARIO_H

#include <iosfwd>
#include <array>
#include <boost/filesystem/path.hpp>
#include "monster.hpp"
// For reasons unknown, putting terrain above monster breaks the tests build in MSVC 2013
#include "terrain.hpp"
#include "vehicle.hpp"
#include "item.hpp"
#include "special.hpp"
#include "outdoors.hpp"
#include "town.hpp"
#include "vector2d.hpp"
#include "shop.hpp"
#include "quest.hpp"

namespace legacy{
	struct scenario_data_type;
	struct item_storage_shortcut_type;
	struct scen_item_data_type;
};

struct scenario_header_flags {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
};

enum eContentRating {G, PG, R, NC17};

class cScenario {
public:
	class cItemStorage {
	public:
		short ter_type;
		short item_num[10];
		short item_odds[10];
		short property;
		cItemStorage();
		cItemStorage& operator = (legacy::item_storage_shortcut_type const &old);
	};
	void destroy_terrain();
	
	cItem const &get_item(item_num_t item) const;
	cItem &get_item(item_num_t item);
	cQuest const &get_quest(int quest) const;
	cQuest &get_quest(int quest);
	cSpecItem const &get_special_item(item_num_t item) const;
	cSpecItem &get_special_item(item_num_t item);
	cShop const &get_shop(int shop) const;
	cShop &get_shop(int shop);
	cTerrain const &get_terrain(ter_num_t ter) const;
	cTerrain &get_terrain(ter_num_t ter);

public:
	unsigned short difficulty,intro_pic,default_ground;
	int bg_out, bg_fight, bg_town, bg_dungeon;
	short intro_mess_pic;
	location where_start,out_sec_start,out_start;
	size_t which_town_start;
	spec_num_t init_spec;
	std::array<spec_loc_t,10> town_mods;
	std::array<rectangle,3> store_item_rects;
	std::array<short,3> store_item_towns;
	std::vector<cSpecItem> special_items;
	std::vector<cQuest> quests;
	std::vector<cShop> shops;
	short uses_custom_graphics;
	eContentRating rating;
	std::vector<ePicType> custom_graphics;
	std::vector<cMonster> scen_monsters;
	std::vector<cVehicle> boats;
	std::vector<cVehicle> horses;
	std::vector<cTerrain> ter_types;
	std::array<cTimer,20> scenario_timers;
	std::vector<cSpecial> scen_specials;
	std::array<cItemStorage,10> storage_shortcuts;
	// These two \/ are populated at load time and then never used again
	location last_out_edited;
	short last_town_edited;
	scenario_header_flags format;
	std::string campaign_id; // A hopefully unique identifier to specify the campaign this scenario is a part of.
	std::vector<cItem> scen_items;
	std::string scen_name;
	std::string who_wrote[2];
	std::string contact_info[2];
	std::array<std::string, 6> intro_strs;
	std::vector<std::string> journal_strs;
	std::vector<std::string> spec_strs;
	std::vector<std::string> snd_names;
	bool adjust_diff;
	bool is_legacy;
	fs::path scen_file; // transient
	vector2d<cOutdoors*> outdoors;
	std::vector<cTown*> towns;
	void addTown(size_t dim) {
		towns.push_back(new cTown(*this, dim));
	}
	
	void import_legacy(legacy::scenario_data_type const &old);
	void import_legacy(legacy::scen_item_data_type const &old);
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
	std::string format_scen_version();
	std::string format_ed_version();
	
	ter_num_t get_ground_from_ter(ter_num_t ter);
	ter_num_t get_ter_from_ground(unsigned short ground);
	ter_num_t get_trim_terrain(unsigned short ground, unsigned short trim_g, eTrimType trim);
	cOutdoors& get_sector(int x, int y);
	bool is_town_entrance_valid(spec_loc_t loc) const;
	
	bool is_ter_used(ter_num_t ter);
	bool is_monst_used(mon_num_t monst);
	bool is_item_used(item_num_t item);
	cItem return_treasure(int loot, bool allow_junk_treasure = false);
	cItem pull_item_of_type(unsigned int loot_max,short min_val,short max_val,const std::vector<eItemType>& types,bool allow_junk_treasure=false);
	
	void reset_version();
	explicit cScenario();
	~cScenario();
	// Copy-and-swap
	void swap(cScenario& other);
	cScenario(const cScenario& other);
	cScenario(cScenario&& other);
	cScenario& operator=(cScenario other);
};

std::istream& operator>> (std::istream& in, eContentRating& rating);
std::ostream& operator<< (std::ostream& out, eContentRating rating);

#endif
