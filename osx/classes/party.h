/*
 *  party.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef PARTY_H
#define PARTY_H

#include <string>
#include <vector>
#include <map>

namespace legacy {
	struct party_record_type;
	struct big_tr_type;
	struct stored_items_list_type;
	struct talk_save_type;
	struct creature_list_type;
	struct pc_record_type;
	struct setup_save_type;
};

class cParty {
public:
	class cConvers { // conversation; formerly talk_save_type
	public:
		short personality;
		short town_num;
		short str_num1, str_num2;
		std::string who_said, in_town, the_str1, the_str2, in_scen; // the actual strings; not always saved, like maps
		
		cConvers& operator = (legacy::talk_save_type old);
	};
	class cJournal {
	public:
		unsigned short str_num;
		unsigned short day;
		std::string the_str, in_scen; // the actual strings; not always saved, like maps
	};
	class cEncNote {
	public:
		unsigned short str_num, where;
		std::string the_str1, the_str2, in_scen; // the actual strings; not always saved, like maps
	};
	class cTimer {
	public:
		short time;
		short global_or_town;
		short node_to_call;
	};
	// formerly party_record_type
	unsigned long age;
	unsigned short gold;
	unsigned short food;
	unsigned char stuff_done[310][50];
	unsigned char item_taken[200][8];
	short light_level;
	location outdoor_corner;
	location i_w_c;
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
	char m_seen[256];
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
	
	cPlayer adven[6];
	
	unsigned short setup[4][64][64]; // formerly setup_save_type
	cItemRec stored_items[3][115]; // formerly stored_items_list_type
	
	//string graphicsFile; // the name of the png file holding this party's custom item, pc, and summonable monster graphics
	cMonster summons; // an array of monsters which can be summoned by the parties items yet don't originate from this scenario
	bool graphicUsed[250]; // whether each custom graphics slot on the party's sheet is actually used; needed to place new custom graphics on the sheet.
	unsigned short scen_won, scen_played; // numbers of scenarios won and played respectively by this party
	std::map<std::string,std::vector<int> > campaign_flags;
	
	cParty& operator = (legacy::party_record_type& old);
	void append(legacy::big_tr_type& old);
	void append(legacy::stored_items_list_type& old,short which_list);
	void append(legacy::setup_save_type& old);
	
	void add_pc(legacy::pc_record_type old);
	void add_pc(cPlayer new_pc);
	void void_pcs();
	bool has_talk_save(short who, short str1, short str2);
	bool save_talk(short who, unsigned char where, short str1, short str2);
	bool add_to_journal(short event, short day);
	bool record(short what, short where);
	bool start_timer(short time, short node, short type);
	cPlayer& operator[](unsigned short n);
	void writeTo(std::ostream& file);
	void readFrom(std::istream& file);
	
	typedef std::vector<cEncNote>::iterator encIter;
	typedef std::vector<cJournal>::iterator journalIter;
	typedef std::vector<cConvers>::iterator talkIter;
	typedef std::vector<cTimer>::iterator timerIter;
	typedef std::map<std::string,std::vector<int> >::iterator campIter;
};

#endif