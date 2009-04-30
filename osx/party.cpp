/*
 *  party.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "classes.h"
#include "oldstructs.h"

__attribute__((deprecated))
cParty& cParty::operator = (legacy::party_record_type& old){
	int i,j;
	age = old.age;
	gold = old.gold;
	food = old.food;
	for(i = 0; i < 310; i++)
		for(j = 0; j < 10; j++)
			stuff_done[i][j] = old.stuff_done[i][j];
	for(i = 0; i < 200; i++)
		for(j = 0; j < 8; j++)
			item_taken[i][j] = old.item_taken[i][j];
	light_level = old.light_level;
	outdoor_corner.x = old.outdoor_corner.x;
	outdoor_corner.y = old.outdoor_corner.y;
	i_w_c.x = old.i_w_c.x;
	i_w_c.y = old.i_w_c.y;
	p_loc.x = old.p_loc.x;
	p_loc.y = old.p_loc.y;
	loc_in_sec.x = old.loc_in_sec.x;
	loc_in_sec.y = old.loc_in_sec.y;
	for(i = 0; i < 30; i++){
		boats[i] = old.boats[i];
		horses[i] = old.horses[i];
		party_event_timers[i].time = old.party_event_timers[i];
		party_event_timers[i].global_or_town = old.global_or_town[i];
		party_event_timers[i].node_to_call = old.node_to_call[i];
	}
	for(i = 0; i < 4; i++){
		creature_save[i] = old.creature_save[i];
		imprisoned_monst[i] = old.imprisoned_monst[i];
	}
	in_boat = old.in_boat;
	in_horse = old.in_horse;
	for(i = 0; i < 10; i++){
		out_c[i] = old.out_c[i];
		for(j = 0; j < 5; j++)
			magic_store_items[j][i] = old.magic_store_items[j][i];
	}
	for(i = 0; i < 256; i++)
		m_seen[i] = old.m_seen[i];
	for(i = 0; i < 50; i++){
		journal[i].str_num = old.journal_str[i];
		journal[i].day = old.journal_day[i];
		spec_items[i] = old.spec_items[i];
	}
	for(i = 0; i < 140; i++){
		special_notes[i].str_num = old.special_notes_str[i][0];
		special_notes[i].where = old.special_notes_str[i][1];
	}
	for(i = 0; i < 120; i++){
		talk_save[i] = old.talk_save[i];
		help_received[i] = old.help_received[i];
	}
	direction = old.direction;
	at_which_save_slot = old.at_which_save_slot;
	for(i = 0; i < 20 ; i++)
		alchemy[i] = old.alchemy[i];
	for(i = 0; i < 200; i++){
		can_find_town[i] = old.can_find_town[i];
		m_killed[i] = old.m_killed[i];
	}
	for(i = 0; i < 100; i++)
		key_times[i] = old.key_times[i];
	total_m_killed = old.total_m_killed;
	total_dam_done = old.total_dam_done;
	total_xp_gained = old.total_xp_gained;
	total_dam_taken = old.total_dam_taken;
	scen_name = old.scen_name;
}

__attribute__((deprecated))
void cParty::append(legacy::stored_items_list_type& old,short which_list){
	for(int i = 0; i < 115; i++)
		stored_items[which_list][i] = old.items[i];
}

__attribute__((deprecated))
void cParty::append(legacy::setup_save_type& old){
	for(int n = 0; n < 4; n++)
		for(int i = 0; i < 64; i++)
			for(int j = 0; j < 64; j++)
				setup[n][i][j] = old.setup[n][i][j];
}

__attribute__((deprecated))
cParty::cConvers& cParty::cConvers::operator = (legacy::talk_save_type old){
	personality = old.personality;
	town_num = old.town_num;
	str_num1 = old.str1;
	str_num2 = old.str2;
}

__attribute__((deprecated))
void cParty::add_pc(legacy::pc_record_type old){
	for(int i = 0; i < 6; i++)
		if(adven[i].main_status == 0){
			adven[i] = old;
			break;
		}
}

void cParty::add_pc(cPlayer new_pc){
	for(int i = 0; i < 6; i++)
		if(adven[i].main_status == 0){
			adven[i] = new_pc;
			break;
		}
}

bool cParty::has_talk_save(short who, short str1, short str2){
	for (int j = 0; j < talk_save.size(); j++)
		if ((talk_save[j].personality == who) && (talk_save[j].str_num1 == str1) &&	(talk_save[j].str_num2 == str2))
			return true;
	return false;
}

bool cParty::save_talk(short who, unsigned char where, short str1, short str2){
	if(talk_save.size() == talk_save.max_size()) return false; // This is extremely unlikely
	cConvers talk;
	talk.personality = who;
	talk.town_num = where;
	talk.str_num1 = str1;
	talk.str_num2 = str2;
	// TODO: locate the strings and store them in the record.
	talk_save.push_back(talk);
	return true;
}

bool cParty::add_to_journal(short event, short day){
	if(journal.size() == journal.max_size()) return false; // Practically impossible
	cJournal entry;
	entry.str_num = event;
	entry.day = day;
	journal.push_back(entry);
	return true;
}

bool cParty::record(short what, short where){
	if(special_notes.size() == special_notes.max_size()) return false; // Never happen
	cEncNote note;
	note.str_num = what;
	note.where = where;
	special_notes.push_back(note);
	return true;
}

bool cParty::start_timer(short time, short node, short type){
	if(party_event_timers.size() == party_event_timers.max_size()) return false; // Shouldn't be reached
	cTimer t;
	t.time = time;
	t.global_or_town = type;
	t.node_to_call = node;
	party_event_timers.push_back(t);
}
