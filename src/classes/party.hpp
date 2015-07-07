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
#include "vehicle.hpp"
#include "creatlist.hpp"
#include "item.hpp"
#include "pc.hpp"
#include "outdoors.hpp"
#include "monster.hpp"
#include "living.hpp"

namespace legacy {
	struct party_record_type;
	struct big_tr_type;
	struct stored_items_list_type;
	struct talk_save_type;
	struct creature_list_type;
	struct pc_record_type;
	struct setup_save_type;
};

struct campaign_flag_type{
	unsigned char idx[25][25];
};

struct job_bank_t {
	std::array<int,6> jobs;
	int anger = 0;
	bool inited = false;
};

class cUniverse;

class cParty : public iLiving {
public:
	class cConvers { // conversation; formerly talk_save_type
	public:
		bool filled = false;
		std::string who_said, in_town, the_str1, the_str2, in_scen;
		
		void append(legacy::talk_save_type old, const cScenario& scenario);
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
		
		void append(int16_t(& old)[2], const cScenario& scenario);
	};
	// formerly party_record_type
	// TODO: Should we make age a long long?
	long next_pc_id = 1000;
	unsigned long age;
	unsigned short gold;
	unsigned short food;
	unsigned char stuff_done[310][50];
	std::array<unsigned char,90> magic_ptrs;
	unsigned char item_taken[200][8];
	short light_level;
	location outdoor_corner;
	location i_w_c;
	// TODO: Does this duplicate cCurTown::p_loc? If not, why not?
	location p_loc;
	location loc_in_sec;
	std::array<cVehicle,30> boats;
	std::array<cVehicle,30> horses;
	cPopulation creature_save[4];
	short in_boat;
	short in_horse;
	std::array<cOutdoors::cCreature,10> out_c;
	std::map<int,std::array<cItem,30>> magic_store_items;
	std::map<int,std::map<int,int>> store_limited_stock;
	std::vector<job_bank_t> job_banks;
	mon_num_t imprisoned_monst[4]; // Soul Crystal
	std::set<mon_num_t> m_noted; // has the monster been scried?
	std::set<mon_num_t> m_seen; // has the monster ever been seen? (this used to have the above meaning)
	std::vector<cJournal> journal;
	std::vector<cEncNote> special_notes;
	std::vector<cConvers> talk_save;
	std::map<ePartyStatus,short> status;
	// Quest stuff
	std::map<int, eQuestStatus> quest_status;
	std::map<int, int> quest_start; // the day the quest was started; used for quests with relative deadlines
	std::map<int, int> quest_source; // if gotten from a job board, this is the number of the job board; otherwise -1
	location left_at;
	size_t left_in;
	eDirection direction;
	short at_which_save_slot;
	bool alchemy[20];
	bool can_find_town[200];
	std::map<int,int> key_times;
	std::vector<cTimer> party_event_timers;
	std::set<int> spec_items;
	std::set<int> help_received;
	short m_killed[200]; // monsters killed per town, I think
	long long total_m_killed, total_dam_done, total_xp_gained, total_dam_taken;
	std::string scen_name;
private:
	cUniverse& univ;
	std::array<cPlayer*,6> adven;
public:
	unsigned short setup[4][64][64]; // formerly setup_save_type
	std::array<std::vector<cItem>,3> stored_items; // formerly stored_items_list_type
	
	std::vector<cMonster> summons; // an array of monsters which can be summoned by the party's items yet don't originate from this scenario
	unsigned short scen_won, scen_played; // numbers of scenarios won and played respectively by this party
private:
	std::map<std::string,campaign_flag_type> campaign_flags;
	std::map<unsigned short,std::pair<unsigned short,unsigned char>> pointers;
public:
	
	void set_ptr(unsigned short p, unsigned short sdfx, unsigned short sdfy);
	void force_ptr(unsigned short p, unsigned short val);
	void clear_ptr(unsigned short p);
	unsigned char get_ptr(unsigned short p);
	
	unsigned char& cpn_flag(unsigned int x, unsigned int y, std::string id = "");
	
	void append(legacy::party_record_type& old);
	void append(legacy::big_tr_type& old);
	void append(legacy::stored_items_list_type& old,short which_list);
	void append(legacy::setup_save_type& old);
	void append(legacy::pc_record_type(& old)[6]);
	
	bool is_alive() const;
	bool is_friendly() const;
	bool is_friendly(const iLiving& other) const;
	bool is_shielded() const;
	int get_shared_dmg(int base_dmg) const;
	location get_loc() const;
	
	void apply_status(eStatus which, int how_much);
	void heal(int how_much);
	void poison(int how_much);
	void cure(int how_much);
	void acid(int how_much);
	void curse(int how_much);
	void slow(int how_much);
	void web(int how_much);
	void disease(int how_much);
	void dumbfound(int how_much);
	void scare(int how_much);
	void sleep(eStatus type, int how_much, int adj);
	void clear_bad_status();
	void avatar();
	void drain_sp(int how_much);
	void restore_sp(int how_much);
	
	int get_health() const;
	int get_magic() const;
	int get_level() const;
	int calc_day() const;
	
	void new_pc(size_t spot);
	void replace_pc(size_t spot, cPlayer* with);
	size_t free_space();
	size_t count(eMainStatus type = eMainStatus::ALIVE);
	void void_pcs();
	bool save_talk(const std::string& who, const std::string& where, const std::string& str1, const std::string& str2);
	bool add_to_journal(const std::string& event, short day);
	bool record(eEncNoteType type, const std::string& what, const std::string& where);
	bool start_timer(short time, short node, short type);
	cPlayer& operator[](unsigned short n);
	const cPlayer& operator[](unsigned short n) const;
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
	
	bool give_item(cItem item,int flags);
	bool forced_give(item_num_t item_num,eItemAbil abil,short dat = -1);
	bool has_abil(eItemAbil abil, short dat = -1);
	bool take_abil(eItemAbil abil, short dat = -1);
	bool check_class(unsigned int item_class,bool take);
	
	bool start_split(short x, short y, snd_num_t noise, short who);
	bool end_split(snd_num_t noise);
	bool is_split() const;
	bool pc_present(short n) const;
	iLiving& pc_present() const; // If only one pc is present, returns that pc. Otherwise returns party.
	void swap_pcs(size_t a, size_t b);
	
	bool sd_legit(short a, short b);
	
	typedef std::vector<cEncNote>::iterator encIter;
	typedef std::vector<cJournal>::iterator journalIter;
	typedef std::vector<cConvers>::iterator talkIter;
	cParty(cUniverse& univ, long party_preset = 'dflt');
	~cParty();
};

bool operator==(const cParty::cConvers& one, const cParty::cConvers& two);
bool operator==(const cParty::cJournal& one, const cParty::cJournal& two);
bool operator==(const cParty::cEncNote& one, const cParty::cEncNote& two);

std::istream& operator>>(std::istream& in, eEncNoteType& type);
std::ostream& operator<<(std::ostream& out, eEncNoteType type);
std::istream& operator>>(std::istream& in, ePartyStatus& type);
std::ostream& operator<<(std::ostream& out, ePartyStatus type);
std::istream& operator>>(std::istream& in, eQuestStatus& type);
std::ostream& operator<<(std::ostream& out, eQuestStatus type);

#endif
