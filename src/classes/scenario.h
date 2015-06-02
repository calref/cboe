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
#include "terrain.h"
#include "monster.h"
#include "vehicle.h"
#include "item.h"
#include "special.h"
#include "outdoors.h"
#include "town.h"
#include "vector2d.hpp"
#include "shop.hpp"

namespace fs = boost::filesystem; // TODO: Centralize this namespace alias?

namespace legacy{
	struct scenario_data_type;
	struct item_storage_shortcut_type;
	struct scen_item_data_type;
};

struct scenario_header_flags {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
};

class cQuest {
public:
	short flags = 0; // 0 - absolute deadline, 1 - relative to when quest started, +10 - start quest when scenario starts
	short deadline = -1;
	short event = -1; // if this event occurs before the deadline, then the deadline is waived
	short xp = 0, gold = 0; // automatically award this much XP and gold to the party when the quest is marked complete
	short bank1 = -1, bank2 = -1; // which job bank(s) this quest is in; -1 for none
	std::string name;
	std::string descr;
};

class cScenario {
public:
	class cItemStorage {
	public:
		short ter_type;
		short item_num[10];
		short item_odds[10];
		short property;
		cItemStorage();
		cItemStorage& operator = (legacy::item_storage_shortcut_type& old);
	};
	cScenario& operator=(const cScenario& other) = default;
	void destroy_terrain();
public:
	unsigned short difficulty,intro_pic,default_ground;
	int bg_out, bg_fight, bg_town, bg_dungeon;
	short intro_mess_pic;
	location where_start,out_sec_start,out_start;
	size_t which_town_start;
	spec_num_t init_spec;
	std::array<spec_loc_t,10> town_mods;
	rectangle store_item_rects[3];
	short store_item_towns[3];
	std::array<cSpecItem,50> special_items;
	std::vector<cQuest> quests;
	std::vector<cShop> shops;
	short rating,uses_custom_graphics;
	std::vector<ePicType> custom_graphics;
	std::array<cMonster,256> scen_monsters;
	std::array<cVehicle,30> boats;
	std::array<cVehicle,30> horses;
	std::array<cTerrain,256> ter_types;
	std::array<cTimer,20> scenario_timers;
	std::vector<cSpecial> scen_specials;
	cItemStorage storage_shortcuts[10];
	// These two \/ are populated at load time and then never used again
	location last_out_edited;
	short last_town_edited;
	scenario_header_flags format;
	std::string campaign_id; // A hopefully unique identifier to specify the campaign this scenario is a part of.
	std::array<cItem,400> scen_items;
	std::string scen_name;
	std::string who_wrote[2];
	std::string contact_info;
	std::string intro_strs[6];
	// Using std::array here so we can have .size()
	// This'll make the transition smoother once it becomes a vector.
	std::array<std::string,50> journal_strs;
	std::array<std::string,101> spec_strs;
	bool adjust_diff;
	bool is_legacy;
	fs::path scen_file; // transient
	vector2d<cOutdoors*> outdoors;
	std::vector<cTown*> towns;
	template<typename Town> void addTown() {towns.push_back(new Town(*this, true));}
	std::string& get_buf() {return spec_strs[100];}
	
	void append(legacy::scenario_data_type& old);
	void append(legacy::scen_item_data_type& old);
	void writeTo(std::ostream& file) const;
	std::string format_scen_version();
	std::string format_ed_version();
	
	ter_num_t get_ground_from_ter(ter_num_t ter);
	ter_num_t get_ter_from_ground(unsigned short ground);
	ter_num_t get_trim_terrain(unsigned short ground, unsigned short trim_g, eTrimType trim);
	
	cScenario& operator=(cScenario&& other);
	cScenario(cScenario&) = delete;
	explicit cScenario(bool init_strings = false);
	~cScenario();
};

// OBoE Current Version
const unsigned long long OBOE_CURRENT_VERSION = 0x020000; // MMmmff; M - major, m - minor, f - bugfix

#endif
