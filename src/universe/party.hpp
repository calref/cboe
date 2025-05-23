/*
 *  party.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef BOE_DATA_PARTY_H
#define BOE_DATA_PARTY_H

#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>

#include <boost/iterator/indirect_iterator.hpp>

#include "scenario/vehicle.hpp"
#include "population.hpp"
#include "scenario/item.hpp"
#include "pc.hpp"
#include "scenario/outdoors.hpp"
#include "scenario/monster.hpp"
#include "living.hpp"
#include "scenario/quest.hpp"

namespace legacy {
	struct party_record_type;
	struct big_tr_type;
	struct stored_items_list_type;
	struct talk_save_type;
	struct creature_list_type;
	struct pc_record_type;
	struct setup_save_type;
};

template<typename T, size_t rows, size_t cols>
using array2d = std::array<std::array<T, cols>, rows>;

struct campaign_flag_type{
	array2d<unsigned char, 25, 25> idx{};
private:
	using idx_array = decltype(idx);
public:
	static const int x_max = std::tuple_size<idx_array>::value;
	static const int y_max = std::tuple_size<idx_array::value_type>::value;
};

struct job_bank_t {
	std::array<int,6> jobs;
	int anger = 0;
	bool inited = false;
};

enum eEncNoteType {
	NOTE_SCEN,
	NOTE_OUT,
	NOTE_TOWN,
};

class cUniverse;
class cItem;
class cTagFile;

class cParty : public iLiving {
public:
	class cConvers { // conversation; formerly talk_save_type
	public:
		std::string who_said, in_town, the_str1, the_str2, in_scen;
		
		void import_legacy(legacy::talk_save_type old, const cScenario& scenario);
	};
	class cJournal {
	public:
		unsigned short day;
		std::string the_str, in_scen;
	};
	class cEncNote {
	public:
		eEncNoteType type;
		std::string the_str, where, in_scen;
		
		void import_legacy(int16_t(& old)[2], const cScenario& scenario);
	};
	// formerly party_record_type
	// TODO: Should we make age a long long?
	long next_pc_id = 1000;
	unsigned long age;
	unsigned short gold;
	unsigned short food;
	array2d<unsigned char, SDF_ROWS, SDF_COLUMNS> stuff_done;
	// These used to be stored as magic SDFs
	unsigned char hostiles_present;
	bool easy_mode = false, less_wm = false;
	// End former magic SDFs
	std::array<unsigned char,90> magic_ptrs;
	short light_level;
	location outdoor_corner;
	location i_w_c;
	location out_loc, town_loc;
	location loc_in_sec;
	short town_num;
	std::vector<cVehicle> boats;
	std::vector<cVehicle> horses;
	std::array<cPopulation,4> creature_save;
	short in_boat;
	short in_horse;
	std::array<cOutdoors::cCreature,10> out_c;
	std::map<int,std::map<int,cItem>> magic_store_items;
	std::map<int,std::map<int,int>> store_limited_stock;
	std::vector<job_bank_t> job_banks;
	std::array<mon_num_t,4> imprisoned_monst; // Soul Crystal
	std::set<mon_num_t> m_noted; // has the monster been scried?
	std::set<mon_num_t> m_seen; // has the monster ever been seen? (this used to have the above meaning)
	std::vector<cJournal> journal;
	std::vector<cEncNote> special_notes;
	std::vector<cConvers> talk_save;
	std::map<ePartyStatus,short> status;
	std::map<int, cJob> active_quests;
	location left_at;
	size_t left_in;
	short at_which_save_slot;
	std::bitset<20> alchemy;
	std::map<int,int> key_times;
	std::vector<cTimer> party_event_timers;
	std::set<int> spec_items;
	long long total_m_killed, total_dam_done, total_xp_gained, total_dam_taken;
	std::string scen_name;
	bool load_failed = false;
private:
	std::array<std::unique_ptr<cPlayer>,6> adven;
public:
	std::array<vector2d<unsigned short>, 4> setup; // formerly setup_save_type
	std::map<short, std::vector<cItem>> stored_items; // formerly stored_items_list_type
	
	std::vector<cMonster> summons; // an array of monsters which can be summoned by the party's items yet don't originate from this scenario
	unsigned short scen_won, scen_played; // numbers of scenarios won and played respectively by this party
	std::map<std::string,campaign_flag_type> campaign_flags;
private:
	std::map<unsigned short,std::pair<unsigned short,unsigned char>> pointers;
	using sd_array = decltype(stuff_done);
public:
	static const int sdx_max = std::tuple_size<sd_array>::value;
	static const int sdy_max = std::tuple_size<sd_array::value_type>::value;
	
	void set_ptr(unsigned short p, unsigned short sdfx, unsigned short sdfy);
	void force_ptr(unsigned short p, unsigned short val);
	void clear_ptr(unsigned short p);
	unsigned char get_ptr(unsigned short p) const;
	
	void import_legacy(legacy::party_record_type& old, cUniverse& univ);
	void import_legacy(legacy::big_tr_type& old);
	void import_legacy(legacy::stored_items_list_type& old,short which_list);
	void import_legacy(legacy::setup_save_type& old);
	void import_legacy(legacy::pc_record_type(& old)[6]);
	
	bool is_in_scenario() const;
	bool is_alive() const override;
	bool is_friendly() const override;
	bool is_friendly(const iLiving& other) const override;
	bool is_shielded() const override;
	int get_shared_dmg(int base_dmg) const override;
	location get_loc() const override;
	
	void apply_status(eStatus which, int how_much) override;
	void heal(int how_much) override;
	void poison(int how_much) override;
	void cure(int how_much) override;
	void acid(int how_much) override;
	void curse(int how_much) override;
	void slow(int how_much) override;
	void web(int how_much) override;
	void disease(int how_much) override;
	void dumbfound(int how_much) override;
	void scare(int how_much) override;
	void sleep(eStatus type, int how_much, int adj) override;
	void clear_bad_status() override;
	void avatar() override;
	void drain_sp(int how_much, bool allow_resist) override;
	void restore_sp(int how_much) override;
	
	int get_health() const override;
	int get_magic() const override;
	int get_level() const override;
	int calc_day() const;
	
	std::unique_ptr<cPlayer> remove_pc(size_t spot);
	void new_pc(size_t spot);
	void replace_pc(size_t spot, std::unique_ptr<cPlayer> with);
	size_t free_space() const;
	size_t count(eMainStatus type = eMainStatus::ALIVE) const;
	bool save_talk(const std::string& who, const std::string& where, const std::string& str1, const std::string& str2);
	bool add_to_journal(const std::string& event, short day);
	bool record(eEncNoteType type, const std::string& what, const std::string& where);
	bool start_timer(short time, spec_num_t node, eSpecCtxType type);
	cPlayer& operator[](unsigned short n);
	const cPlayer& operator[](unsigned short n) const;
	void writeTo(cTagFile& file) const;
	void readFrom(const cTagFile& file, bool preview = false);
	
	bool give_item(cItem item,int flags);
	bool forced_give(cItem item,eItemAbil abil,short dat = -1);
	bool all_items_identified();
	bool has_abil(eItemAbil abil, short dat = -1) const;
	bool take_abil(eItemAbil abil, short dat = -1);
	bool has_class(unsigned int item_class, bool require_charges = false);
	bool take_class(unsigned int item_class) const;
	
	bool start_split(short x, short y, snd_num_t noise, short who);
	bool end_split(snd_num_t noise);
	bool is_split() const;
	bool pc_present(short n) const;
	iLiving& pc_present() const; // If only one pc is present, returns that pc. Otherwise returns party.
	void swap_pcs(size_t a, size_t b);
	
	bool sd_legit(short a, short b) const;
	void wipe_sdfs();
	
	auto begin() -> boost::indirect_iterator<decltype(adven)::iterator> {
		return boost::make_indirect_iterator(adven.begin());
	}
	
	auto end() -> boost::indirect_iterator<decltype(adven)::iterator> {
		return boost::make_indirect_iterator(adven.end());
	}
	
	auto begin() const -> boost::indirect_iterator<decltype(adven)::const_iterator> {
		return boost::make_indirect_iterator(adven.begin());
	}
	
	auto end() const -> boost::indirect_iterator<decltype(adven)::const_iterator> {
		return boost::make_indirect_iterator(adven.end());
	}
	
	typedef std::vector<cEncNote>::iterator encIter;
	typedef std::vector<cJournal>::iterator journalIter;
	typedef std::vector<cConvers>::iterator talkIter;
	cParty(ePartyPreset party_preset = PARTY_DEFAULT);
	// Copy-and-swap
	friend void swap(cParty& lhs, cParty& rhs);
	cParty(const cParty& other);
	cParty(cParty&& other);
	cParty& operator=(cParty other);
};

bool operator==(const cParty::cConvers& one, const cParty::cConvers& two);
bool operator==(const cParty::cJournal& one, const cParty::cJournal& two);
bool operator==(const cParty::cEncNote& one, const cParty::cEncNote& two);

std::istream& operator>>(std::istream& in, eEncNoteType& type);
std::ostream& operator<<(std::ostream& out, eEncNoteType type);

#endif
