/*
 *  universe.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef BOE_DATA_UNIVERSE_H
#define BOE_DATA_UNIVERSE_H

#include <iosfwd>
#include <memory>
#include <set>
#include <array>
#include <boost/filesystem/path.hpp>
#include "party.hpp"
#include "population.hpp"
#include "scenario/item.hpp"
#include "scenario/town.hpp"
#include "scenario/talking.hpp"
#include "scenario/scenario.hpp"
#include "dialogxml/widgets/pictypes.hpp"

class cTagFile;

namespace legacy {
	struct out_info_type;
	struct current_town_type;
	struct town_item_list;
	struct stored_town_maps_type;
	struct stored_outdoor_maps_type;
	struct big_tr_type;
};

class cCurTown {
	short cur_talk_loaded = -1;
	bool free_for_sfx(short x, short y);
	cUniverse& univ;
	cTown* arena;
	cTown*const record() const;
	vector2d<unsigned long> fields;
public:
	bool quickfire_present = false, belt_present = false;
	// formerly current_town_type
	cPopulation monst;
	
	std::vector<cItem> items; // formerly town_item_list type
	
	void import_legacy(legacy::current_town_type const &old);
	void import_legacy(legacy::town_item_list const &old);
	void import_reset_fields_legacy();
	void import_legacy(legacy::big_tr_type const &old);
	
	cTown* operator -> ();
	cTown& operator * ();
	const cTown* operator -> () const;
	const cTown& operator * () const;
	explicit cCurTown(cUniverse& univ);
	short countMonsters() const;
	// Get the currently loaded speech
	cSpeech& cur_talk();
	const cSpeech& cur_talk() const;
	bool prep_talk(short which); // Prepare for loading specified speech, returning true if already loaded
	void prep_arena(); // Set up for a combat arena
	void place_preset_fields();
	void update_fields(const vector2d<unsigned short>& setup);
	void save_setup(vector2d<unsigned short>& setup) const;
	bool is_summon_safe(short x, short y) const;
	
	bool is_explored(short x, short y) const;
	bool is_force_wall(short x, short y) const;
	bool is_fire_wall(short x, short y) const;
	bool is_antimagic(short x, short y) const;
	bool is_scloud(short x, short y) const; // stinking cloud
	bool is_ice_wall(short x, short y) const;
	bool is_blade_wall(short x, short y) const;
	bool is_sleep_cloud(short x, short y) const;
	bool is_block(short x, short y) const; // pushable block
	bool is_spot(short x, short y) const;
	bool is_special(short x, short y) const;
	bool is_web(short x, short y) const;
	bool is_crate(short x, short y) const;
	bool is_barrel(short x, short y) const;
	bool is_fire_barr(short x, short y) const;
	bool is_force_barr(short x, short y) const;
	bool is_quickfire(short x, short y) const;
	bool is_sm_blood(short x, short y) const;
	bool is_med_blood(short x, short y) const;
	bool is_lg_blood(short x, short y) const;
	bool is_sm_slime(short x, short y) const;
	bool is_lg_slime(short x, short y) const;
	bool is_ash(short x, short y) const;
	bool is_bones(short x, short y) const;
	bool is_rubble(short x, short y) const;
	bool is_force_cage(short x, short y) const;
	bool is_road(short x, short y) const;
	bool set_explored(short x, short y, bool b);
	bool set_force_wall(short x, short y, bool b);
	bool set_fire_wall(short x, short y, bool b);
	bool set_antimagic(short x, short y, bool b);
	bool set_scloud(short x, short y, bool b); // stinking cloud
	bool set_ice_wall(short x, short y, bool b);
	bool set_blade_wall(short x, short y, bool b);
	bool set_sleep_cloud(short x, short y, bool b);
	bool set_block(short x, short y, bool b); // pushable block
	bool set_spot(short x, short y, bool b);
	bool set_web(short x, short y, bool b);
	bool set_crate(short x, short y, bool b);
	bool set_barrel(short x, short y, bool b);
	bool set_fire_barr(short x, short y, bool b);
	bool set_force_barr(short x, short y, bool b);
	bool set_quickfire(short x, short y, bool b);
	bool set_sm_blood(short x, short y, bool b);
	bool set_med_blood(short x, short y, bool b);
	bool set_lg_blood(short x, short y, bool b);
	bool set_sm_slime(short x, short y, bool b);
	bool set_lg_slime(short x, short y, bool b);
	bool set_ash(short x, short y, bool b);
	bool set_bones(short x, short y, bool b);
	bool set_rubble(short x, short y, bool b);
	bool set_force_cage(short x, short y, bool b);
	bool set_road(short x, short y, bool b);
	bool is_impassable(short x, short y) const;
	bool is_on_map(short x, short y) const;
	void writeTo(cTagFile& file) const;
	void readFrom(const cTagFile& file);
	
	~cCurTown();
	// It's not directly copyable due to the cUniverse reference, which must always point to the cUniverse that contains it.
	// The cUniverse copy constructor is thus responsible for performing the copy.
	cCurTown(const cCurTown&) = delete;
	cCurTown& operator=(const cCurTown&) = delete;
	// Not movable for similar reasons
	cCurTown(const cCurTown&& other) = delete;
	cCurTown& operator=(const cCurTown&& other) = delete;
	// This implements the actual copy/move.
	void copy(const cCurTown& other);
	friend void swap(cCurTown& lhs, cCurTown& rhs);
};

class cCurOut {
	cUniverse& univ;
public:
	static const int max_dim = 96;
	static const int half_dim = max_dim / 2;
	array2d<ter_num_t, max_dim, max_dim> out;
	array2d<unsigned char, max_dim, max_dim> out_e;
	
	// These take global coords (ie 0..95)
	bool is_spot(int x, int y) const;
	bool is_road(int x, int y) const;
	bool is_on_map(int x, int y) const;
	
	void import_legacy(legacy::out_info_type const &old);
	
	using arr_96 = decltype(out)::value_type;
	arr_96& operator [] (size_t i);
	const arr_96& operator [] (size_t i) const;
	ter_num_t& operator [] (location loc);
	const ter_num_t& operator [] (location loc) const;
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
	cOutdoors* operator->();
	const cOutdoors* operator->() const;
	explicit cCurOut(cUniverse& univ);
	// It's not directly copyable due to the cUniverse reference, which must always point to the cUniverse that contains it.
	// The cUniverse copy constructor is thus responsible for performing the copy.
	cCurOut(const cCurOut&) = delete;
	cCurOut& operator=(const cCurOut&) = delete;
	// Not movable for similar reasons
	cCurOut(const cCurOut&& other) = delete;
	cCurOut& operator=(const cCurOut&& other) = delete;
	// This implements the actual copy/move.
	void copy(const cCurOut& other);
	friend void swap(cCurOut& lhs, cCurOut& rhs);
};

enum eTargetType {TARG_ANY, TARG_PC, TARG_MONST};

class cUniverse{
	// The string buffer currently isn't saved
	std::string strbuf;
	std::map<int,std::string> extrabufs;
	cItem get_random_store_item(int loot_type, bool allow_junk_treasure);
public:
	void exportSummons();
	void exportGraphics();
	
	iLiving& get_target(size_t which);
	iLiving* target_there(location pos, eTargetType type = TARG_ANY);
	size_t get_target_i(iLiving& who);
	
	std::string& get_buf() {return strbuf;}
	void swap_buf(int newbuf) {std::swap(strbuf, extrabufs[newbuf]);}
	
	unsigned char& cpn_flag(unsigned int x, unsigned int y, std::string id = "");
	
	short cur_pc = 0;
	cPlayer& current_pc();
	
	cScenario scenario;
	cParty party;
	std::map<long, std::unique_ptr<cPlayer>> stored_pcs;
	cCurTown town;
	cCurOut out;
	fs::path file;
	bool debug_mode { false };
	bool ghost_mode { false };
	bool node_step_through { false };
	
	void clear_stored_pcs();
	void import_legacy(legacy::stored_town_maps_type const &old);
	void import_legacy(legacy::stored_outdoor_maps_type const &old);
	void enter_scenario(const std::string& name);
	void refresh_store_items();
	void generate_job_bank(int which, job_bank_t& bank);
	short difficulty_adjust() const;
	explicit cUniverse(ePartyPreset party_type = PARTY_DEFAULT);

	cItem const &get_item(item_num_t item) const { return scenario.get_item(item); }
	cItem &get_item(item_num_t item) { return scenario.get_item(item); }
	cQuest const &get_quest(int quest) const { return scenario.get_quest(quest); }
	cQuest &get_quest(int quest) { return scenario.get_quest(quest); }
	cShop const &get_shop(int shop) const { return scenario.get_shop(shop); }
	cShop &get_shop(int shop) { return scenario.get_shop(shop); }
	cSpecItem const &get_special_item(item_num_t item) const { return scenario.get_special_item(item); }
	cSpecItem &get_special_item(item_num_t item) { return scenario.get_special_item(item); }
	cTerrain const &get_terrain(ter_num_t ter) const { return scenario.get_terrain(ter); }
	cTerrain &get_terrain(ter_num_t ter) { return scenario.get_terrain(ter); }

	// Copy-and-swap
	friend void swap(cUniverse& lhs, cUniverse& rhs);
	cUniverse(const cUniverse& other);
	cUniverse(cUniverse&& other);
	cUniverse& operator=(cUniverse other);
	static void(* print_result)(std::string);
};

#endif
