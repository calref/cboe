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
#include <map>
#include <set>
#include "vehicle.h"
#include "creatlist.h"
#include "item.h"
#include "pc.h"
#include "outdoors.h"
#include "monster.h"

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

class cUniverse;

class cParty {
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
	class cTimer {
	public:
		short time;
		short global_or_town;
		short node_to_call;
	};
	// formerly party_record_type
	// TODO: Should we make age a long long?
	unsigned long age;
	unsigned short gold;
	unsigned short food;
	unsigned char stuff_done[310][50];
	unsigned char item_taken[200][8];
	short light_level;
	location outdoor_corner;
	location i_w_c;
	// TODO: Does this duplicate cCurTown::p_loc? If not, why not?
	location p_loc;
	location loc_in_sec;
	cVehicle boats[30];
	cVehicle horses[30];
	cPopulation creature_save[4];
	short in_boat;
	short in_horse;
	cOutdoors::cCreature out_c[10];
	cItemRec magic_store_items[5][10];
	short imprisoned_monst[4]; // Soul Crystal?
	char m_noted[256]; // has the monster been scried?
	char m_seen[256]; // has the monster ever been seen? (this used to have the above meaning)
	std::vector<cJournal> journal;
	std::vector<cEncNote> special_notes;
	std::vector<cConvers> talk_save;
	short direction;
	short at_which_save_slot;
	char alchemy[20];
	bool can_find_town[200];
	short key_times[100];
	std::vector<cTimer> party_event_timers;
	//short global_or_town[30];
	//short node_to_call[30];
	char spec_items[50];
	char help_received[120];
	short m_killed[200]; // monsters killed per town, I think
	long long total_m_killed, total_dam_done, total_xp_gained, total_dam_taken;
	std::string scen_name;
private:
	cUniverse& univ;
	cPlayer adven[6];
public:
	unsigned short setup[4][64][64]; // formerly setup_save_type
	cItemRec stored_items[3][115]; // formerly stored_items_list_type
	
	//string graphicsFile; // the name of the png file holding this party's custom item, pc, and summonable monster graphics
	std::vector<cMonster> summons; // an array of monsters which can be summoned by the party's items yet don't originate from this scenario
	bool graphicUsed[250]; // whether each custom graphics slot on the party's sheet is actually used; needed to place new custom graphics on the sheet.
	unsigned short scen_won, scen_played; // numbers of scenarios won and played respectively by this party
private:
	std::map<std::string,campaign_flag_type> campaign_flags;
	std::map<unsigned short,std::pair<unsigned short,unsigned char>> pointers;
public:
	
	void set_ptr(unsigned short p, unsigned short sdfx, unsigned short sdfy);
	void force_ptr(unsigned short p, unsigned short sdfx, unsigned short sdfy);
	void clear_ptr(unsigned short p);
	unsigned char get_ptr(unsigned short p);
	
	unsigned char& cpn_flag(unsigned int x, unsigned int y, std::string id = "");
	
	void append(legacy::party_record_type& old);
	void append(legacy::big_tr_type& old);
	void append(legacy::stored_items_list_type& old,short which_list);
	void append(legacy::setup_save_type& old);
	
	void add_pc(legacy::pc_record_type old);
	void add_pc(cPlayer new_pc);
	void void_pcs();
	bool save_talk(const std::string& who, const std::string& where, const std::string& str1, const std::string& str2);
	bool add_to_journal(const std::string& event, short day);
	bool record(eEncNoteType type, const std::string& what, const std::string& where);
	bool start_timer(short time, short node, short type);
	cPlayer& operator[](unsigned short n);
	const cPlayer& operator[](unsigned short n) const;
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
	
	std::string start_split(short a, short b, snd_num_t noise, short who);
	std::string end_split(snd_num_t noise);
	bool is_split();
	bool pc_present(short n);
	short pc_present(); // If only one pc is present, returns the number of that pc. Otherwise returns 6.
	location left_at(); // The location that the left-behind character in a split were left at.
	size_t left_in(); // The town they were left in.
	
	typedef std::vector<cEncNote>::iterator encIter;
	typedef std::vector<cJournal>::iterator journalIter;
	typedef std::vector<cConvers>::iterator talkIter;
	typedef std::vector<cTimer>::iterator timerIter;
	cParty(cUniverse& univ, long party_preset = 'dflt');
	// TODO: Remove this in favour of cParty constructor
	friend void init_party(short);
};

bool operator==(const cParty::cConvers& one, const cParty::cConvers& two);
bool operator==(const cParty::cJournal& one, const cParty::cJournal& two);
bool operator==(const cParty::cEncNote& one, const cParty::cEncNote& two);

std::istream& operator>>(std::istream& in, eEncNoteType& type);
std::ostream& operator<<(std::ostream& out, eEncNoteType type);

#endif