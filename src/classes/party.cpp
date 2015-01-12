/*
 *  party.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>

#include "dlogutil.hpp"
#include "classes.h"
#include "oldstructs.h"
#include "fileio.hpp"

cParty::cParty(cUniverse& univ, long party_preset) : univ(univ) {
	gold = 200;
	food = 100;
	// Note: These starting position values were in the original code and so are included here.
	// However, I suspect they're relics of Exile III that are soon overwritten.
	outdoor_corner.x = 7;
	outdoor_corner.y = 8;
	i_w_c.x = 1;
	i_w_c.y = 1;
	loc_in_sec.x = 36;
	loc_in_sec.y = 36;
	p_loc.x = 84;
	p_loc.y = 84;
	in_boat = -1;
	in_horse = -1;
	for(int i = 0; i < 5; i++)
		for(int j = 0; j < 10; j++)
			magic_store_items[i][j].variety = eItemType::NO_ITEM;
	// Original code set all key times 30000. I felt this was more appropriate.
	std::fill(key_times, key_times + 20, std::numeric_limits<short>::max());
	
	for(int i = 0; i < 6; i++)
		adven[i] = cPlayer(party_preset, i);
}

void cParty::append(legacy::party_record_type& old){
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
	party_event_timers.reserve(30);
	for(i = 0; i < 30; i++){
		boats[i].append(old.boats[i]);
		horses[i].append(old.horses[i]);
		cTimer t;
		t.time = old.party_event_timers[i];
		t.global_or_town = old.global_or_town[i];
		t.node_to_call = old.node_to_call[i];
		party_event_timers.push_back(t);
//		party_event_timers[i].time = old.party_event_timers[i];
//		party_event_timers[i].global_or_town = old.global_or_town[i];
//		party_event_timers[i].node_to_call = old.node_to_call[i];
	}
	for(i = 0; i < 4; i++){
		creature_save[i].append(old.creature_save[i]);
		imprisoned_monst[i] = old.imprisoned_monst[i];
	}
	in_boat = old.in_boat;
	in_horse = old.in_horse;
	for(i = 0; i < 10; i++){
		out_c[i].append(old.out_c[i]);
		for(j = 0; j < 5; j++)
			magic_store_items[j][i].append(old.magic_store_items[j][i]);
	}
	for(i = 0; i < 256; i++)
		m_noted[i] = old.m_seen[i];
	journal.reserve(50);
	// The journal wasn't used before, so let's not bother converting it
//	for(i = 0; i < 50; i++){
//		cJournal j;
//		j.day = old.journal_day[i];
//		journal.push_back(j);
//		spec_items[i] = old.spec_items[i];
//	}
	special_notes.reserve(140);
	for(i = 0; i < 140; i++){
		if(old.special_notes_str[i][0] <= 0) continue;
		cEncNote n;
		n.append(old.special_notes_str[i], univ.scenario);
		special_notes.push_back(n);
	}
	talk_save.reserve(120);
	for(i = 0; i < 120; i++){
		cConvers t;
		t.append(old.talk_save[i], univ.scenario);
		talk_save.push_back(t);
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

void cParty::append(legacy::stored_items_list_type& old,short which_list){
	for(int i = 0; i < 115; i++)
		stored_items[which_list][i].append(old.items[i]);
}

void cParty::append(legacy::setup_save_type& old){
	for(int n = 0; n < 4; n++)
		for(int i = 0; i < 64; i++)
			for(int j = 0; j < 64; j++)
				setup[n][i][j] = old.setup[n][i][j];
}

void cParty::cConvers::append(legacy::talk_save_type old, const cScenario& scenario){
	who_said = scenario.towns[old.personality / 10]->talking.people[old.personality % 10].title;
	in_town = scenario.towns[old.town_num]->town_name;
	the_str1 = scenario.towns[old.personality / 10]->spec_strs[old.str1];
	the_str2 = scenario.towns[old.personality / 10]->spec_strs[old.str2];
}

void cParty::cEncNote::append(int16_t(& old)[2], const cScenario& scenario) {
	in_scen = scenario.scen_name;
	// TODO: Need to verify that I have the correct offsets here.
	switch(old[0] / 1000) {
		case 0:
			the_str = scenario.spec_strs[old[0] - 160];
			where = scenario.scen_name; // Best we can do here; the actual location is long forgotten
			type = NOTE_SCEN;
			break;
		case 1:
			the_str = scenario.outdoors[old[1] % scenario.outdoors.width()][old[1] / scenario.outdoors.width()]->spec_strs[old[0] - 1010];
			where = scenario.outdoors[old[1] % scenario.outdoors.width()][old[1] / scenario.outdoors.width()]->out_name;
			type = NOTE_OUT;
			break;
		case 2:
			the_str = scenario.towns[old[1]]->spec_strs[old[0] - 2020];
			where = scenario.towns[old[1]]->town_name;
			type= NOTE_TOWN;
			break;
	}
}

void cParty::add_pc(legacy::pc_record_type old){
	for(int i = 0; i < 6; i++)
		if(adven[i].main_status == eMainStatus::ABSENT){
			adven[i].append(old);
			break;
		}
}

void cParty::void_pcs(){
	for(int i = 0; i < 6; i++)
		adven[i].main_status = eMainStatus::ABSENT;
}

void cParty::add_pc(cPlayer new_pc){
	for(int i = 0; i < 6; i++)
		if(adven[i].main_status == eMainStatus::ABSENT){
			adven[i] = new_pc;
			break;
		}
}

bool cParty::save_talk(const std::string& who, const std::string& where, const std::string& str1, const std::string& str2){
	if(talk_save.size() == talk_save.max_size()) return false; // This is extremely unlikely
	cConvers talk;
	talk.who_said = who;
	talk.in_town = where;
	talk.the_str1 = str1;
	talk.the_str2 = str2;
	talk.in_scen = scen_name;
	if(std::find(talk_save.begin(), talk_save.end(), talk) == talk_save.end()) {
		talk_save.push_back(talk);
		return true;
	}
	return false;
}

bool cParty::add_to_journal(const std::string& event, short day){
	if(journal.size() == journal.max_size()) return false; // Practically impossible
	cJournal entry;
	entry.day = day;
	entry.the_str = event;
	entry.in_scen = scen_name;
	if(std::find(journal.begin(), journal.end(), entry) == journal.end()) {
		journal.push_back(entry);
		return true;
	}
	return false;
}

bool cParty::record(eEncNoteType type, const std::string& what, const std::string& where){
	if(special_notes.size() == special_notes.max_size()) return false; // Never happen
	cEncNote note;
	note.type = type;
	note.the_str = what;
	note.where = where;
	note.in_scen = scen_name;
	if(std::find(special_notes.begin(), special_notes.end(), note) == special_notes.end()) {
		special_notes.push_back(note);
		return true;
	}
	return false;
}

bool cParty::start_timer(short time, short node, short type){
	if(party_event_timers.size() == party_event_timers.max_size()) return false; // Shouldn't be reached
	cTimer t;
	t.time = time;
	t.global_or_town = type;
	t.node_to_call = node;
	party_event_timers.push_back(t);
	return(true);
}

void cParty::writeTo(std::ostream& file) const {
	file << "CREATEVERSION " << std::hex << OBOE_CURRENT_VERSION << std::dec << '\n';
	file << "AGE " << age << '\n';
	file << "GOLD " << gold << '\n';
	file << "FOOD " << food << '\n';
	for(int i = 0; i < 310; i++)
		for(int j = 0; j < 50; j++)
			if(stuff_done[i][j] > 0)
				file << "SDF " << i << ' ' << j << ' ' << unsigned(stuff_done[i][j]) << '\n';
	for(auto iter = pointers.begin(); iter != pointers.end(); iter++)
		file << "POINTER " << iter->first << ' ' << iter->second.first << ' ' << iter->second.second << '\n';
	for(int i = 0; i < 200; i++)
		if(item_taken[i][0] > 0 || item_taken[i][1] > 0 || item_taken[i][2] > 0 || item_taken[i][3] > 0 ||
		   item_taken[i][4] > 0 || item_taken[i][5] > 0 || item_taken[i][6] > 0 || item_taken[i][7] > 0) {
			file << "ITEMTAKEN " << i;
			for(int j = 0; j < 8; j++)
				file << ' ' << unsigned(item_taken[i][j]);
			file << '\n';
		}
	file << "LIGHT " << light_level << '\n';
	file << "OUTCORNER " << outdoor_corner.x << ' ' << outdoor_corner.y << '\n';
	file << "INWHICHCORNER " << i_w_c.x << ' ' << i_w_c.y << '\n';
	file << "SECTOR " << p_loc.x << ' ' << p_loc.y << '\n';
	file << "LOCINSECTOR " << loc_in_sec.x << ' ' << loc_in_sec.y << '\n';
	file << "IN " << in_boat << ' ' << in_horse << '\n';
	for(int i = 0; i < 256; i++)
		if(m_noted[i])
			file << "ROSTER " << i << '\n';
	for(int i = 0; i < 256; i++)
		if(m_seen[i])
			file << "SEEN " << i << '\n';
	for(int i = 0; i < 4; i++)
		if(imprisoned_monst[i] > 0)
			file << "SOULCRYSTAL " << i << ' ' << imprisoned_monst[i] << '\n';
	file << "DIRECTION " << direction << '\n';
	file << "WHICHSLOT " << at_which_save_slot << '\n';
	for(int i = 0; i < 20; i++)
		if(alchemy[i])
			file << "ALCHEMY " << i << '\n';
	for(int i = 0; i < 200; i++)
		if(can_find_town[i])
			file << "TOWNVISIBLE " << i << '\n';
	for(int i = 0; i < 100; i++)
		if(key_times[i])
			file << "EVENT " << i << ' ' << key_times[i] << '\n';
	for(int i = 0; i < 50; i++)
		if(spec_items[i])
			file << "ITEM " << i << '\n';
	for(int i = 0; i < 120; i++)
		if(help_received[i])
			file << "HELP " << i << '\n';
	for(int i = 0; i < 200; i++)
		if(m_killed[i] > 0)
			file << "TOWNSLAUGHTER " << i << ' ' << m_killed[i] << '\n';
	file << "KILLS " << total_m_killed << '\n';
	file << "DAMAGE " << total_dam_done << '\n';
	file << "WOUNDS " << total_dam_taken << '\n';
	file << "EXPERIENCE " << total_xp_gained << '\n';
	file << "SCENARIO " << scen_name << '\n';
	file << "WON " << scen_won << '\n';
	file << "PLAYED " << scen_played << '\n';
	for(auto iter = campaign_flags.begin(); iter != campaign_flags.end(); iter++){
		std::string campaign_id = maybe_quote_string(iter->first);
		// Okay, we have the campaign ID in a state such that reading it back in will restore the original ID.
		// Now output any flags that are set for this campaign.
		for(unsigned int i = 0; i < 25; i++)
			for(unsigned int j = 0; j < 20; j++)
				if(iter->second.idx[i][j] > 0)
					file << "CAMPAIGN " << campaign_id << ' ' << i << ' ' << j << ' ' << unsigned(iter->second.idx[i][j]) << '\n';
	}
	file << '\f';
	for(int i = 0; i < 30; i++){
		if(boats[i].exists) {
			file << "BOAT " << i << '\n';
			boats[i].writeTo(file);
			file << '\f';
		}
	}
	file << '\f';
	for(int i = 0; i < 30; i++){
		if(horses[i].exists) {
			file << "HORSE " << i << '\n';
			horses[i].writeTo(file);
			file << '\f';
		}
	}
	file << '\f';
	for(int i = 0; i < 5; i++)
		for(int j = 0; j < 10; j++)
			if(magic_store_items[i][j].variety != eItemType::NO_ITEM){
				file << "MAGICSTORE " << i << ' ' << j << '\n';
				magic_store_items[i][j].writeTo(file);
				file << '\f';
			}
	file << '\f';
	for(int i = 0; i < 10; i++)
		if(out_c[i].exists){
			file << "ENCOUNTER " << i << "\n";
			file << "DIRECTION " << out_c[i].direction << '\n';
			file << "SECTOR " << out_c[i].which_sector.x << ' ' << out_c[i].which_sector.y << '\n';
			file << "LOCINSECTOR " << out_c[i].m_loc.x << ' ' << out_c[i].m_loc.y << '\n';
			file << "HOME " << out_c[i].home_sector.x << ' ' << out_c[i].home_sector.y << '\n';
			file << "-\n";
			out_c[i].what_monst.writeTo(file);
			file << '\f';
		}
	file << '\f';
	file << '\f';
	for(unsigned int i = 0; i < party_event_timers.size(); i++)
		file << "TIMER " << ' ' << party_event_timers[i].time << ' ' << party_event_timers[i].global_or_town
			 << ' ' << party_event_timers[i].node_to_call << '\f';
	file << '\f';
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 60; j++) {
			if(creature_save[i][j].active > 0) {
				file << "CREATURE " << i << ' ' << j << '\n';
				creature_save[i][j].writeTo(file);
				file << '\f';
			}
		}
	file << '\f';
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 115; j++)
			if(stored_items[i][j].variety != eItemType::NO_ITEM){
				file << "STORED " << i << ' ' << j << '\n';
				stored_items[i][j].writeTo(file);
				file << '\f';
			}
	if(summons.size() > 0) {
		file << '\f';
		int i = 0;
		for(const cMonster& monst : summons) {
			file << "SUMMON " << i++ << '\n';
			monst.writeTo(file);
			file << '\f';
		}
	}
	if(journal.size() > 0) {
		file << '\f';
		for(const cJournal& entry : journal) {
			file << "JOURNAL " << entry.day << ' ' << maybe_quote_string(entry.in_scen) << '\n' << entry.the_str << '\n';
			file << '\f';
		}
	}
	if(special_notes.size() > 0) {
		file << '\f';
		for(const cEncNote& note : special_notes) {
			file << "ENCNOTE " << note.type << ' ' << maybe_quote_string(note.where) << '\n' << note.the_str << '\n';
			file << '\f';
		}
	}
	if(talk_save.size() > 0) {
		file << '\f';
		for(const cConvers& note : talk_save) {
			file << "TALKNOTE";
			file << "WHO " << maybe_quote_string(note.who_said) << '\n';
			file << "WHERE " << maybe_quote_string(note.in_town) << ' ' << maybe_quote_string(note.in_scen) << '\n';
			file << "-\n" << note.the_str1 << '\n' << note.the_str2 << '\n';
			file << '\f';
		}
	}
}

void cParty::readFrom(std::istream& file){
	// TODO: Error-check input
	// TODO: Don't call this sin, it's a trig function
	std::istringstream bin;
	std::string cur;
	getline(file, cur, '\f');
	bin.str(cur);
	while(bin) { // continue as long as no error, such as eof, occurs
		getline(bin, cur);
		std::cout << "Parsing line in party.txt: " << cur << std::endl;
		std::istringstream sin(cur);
		sin >> cur;
		if(cur == "AGE")
			sin >> age;
		else if(cur == "GOLD")
			sin >> gold;
		else if(cur == "FOOD")
			sin >> food;
		else if(cur == "SDF"){
			int i,j;
			unsigned int n;
			sin >> i >> j >> n;
			stuff_done[i][j] = n;
		} else if(cur == "CREATEVERSION") {
			unsigned long long version;
			sin >> std::hex >> version >> std::dec;
			if(version > OBOE_CURRENT_VERSION) {
				giveError("Warning: this game appears to have been created with a newer version of Blades of Exile than you are running. Exile will do its best to load the saved game anyway, but there may be loss of information.");
			}
		} else if(cur == "POINTER") {
			int i,j,k;
			sin >> i >> j >> k;
			pointers[i] = std::make_pair(j,k);
		}else if(cur == "ITEMTAKEN"){
			int i;
			sin >> i;
			for(int j = 0; j < 8; j++) {
				unsigned int n;
				sin >> n;
				item_taken[i][j] = n;
			}
		}else if(cur == "LIGHT")
			sin >> light_level;
		else if(cur == "OUTCORNER")
			sin >> outdoor_corner.x >> outdoor_corner.y;
		else if(cur == "INWHICHCORNER")
			sin >> i_w_c.x >> i_w_c.y;
		else if(cur == "SECTOR")
			sin >> p_loc.x >> p_loc.y;
		else if(cur == "LOCINSECTOR")
			sin >> loc_in_sec.x >> loc_in_sec.y;
		else if(cur == "IN")
			sin >> in_boat >> in_horse;
		else if(cur == "ROSTER"){
			int i;
			sin >> i;
			m_noted[i] = true;
		}else if(cur == "SEEN"){
			int i;
			sin >> i;
			m_seen[i] = true;
		}else if(cur == "SOULCRYSTAL"){
			int i;
			sin >> i;
			sin >> imprisoned_monst[i];
		}else if(cur == "DIRECTION")
			sin >> direction;
		else if(cur == "WHICHSLOT")
			sin >> at_which_save_slot;
		else if(cur == "ALCHEMY"){
			int i;
			sin >> i;
			alchemy[i] = true;
		} else if(cur == "TOWNVISIBLE") {
			int i;
			sin >> i;
			can_find_town[i] = true;
		}else if(cur == "EVENT"){
			int i;
			sin >> i;
			sin >> key_times[i];
		}else if(cur == "ITEM"){
			int i;
			sin >> i;
			spec_items[i] = true;
		}else if(cur == "HELP"){
			int i;
			sin >> i;
			help_received[i] = true;
		}else if(cur == "TOWNSLAUGHTER"){
			int i;
			sin >> i;
			sin >> m_killed[i];
		}else if(cur == "KILLS")
			sin >> total_m_killed;
		else if(cur == "DAMAGE")
			sin >> total_dam_done;
		else if(cur == "WOUNDS")
			sin >> total_dam_taken;
		else if(cur == "EXPERIENCE")
			sin >> total_xp_gained;
		else if(cur == "SCENARIO")
			sin >> scen_name;
		else if(cur == "WON")
			sin >> scen_won;
		else if(cur == "PLAYED")
			sin >> scen_played;
		sin.clear();
	}
	bin.clear();
	while(file) {
		getline(file, cur, '\f');
		bin.str(cur);
		bin >> cur;
		if(cur == "BOAT") {
			int i;
			bin >> i;
			boats[i].exists = true;
			boats[i].readFrom(bin);
		} else if(cur == "HORSE") {
			int i;
			bin >> i;
			horses[i].exists = true;
			horses[i].readFrom(bin);
		} else if(cur == "MAGICSTORE") {
			int i,j;
			bin >> i >> j;
			magic_store_items[i][j].readFrom(bin);
		} else if(cur == "ENCOUNTER") {
			int i;
			bin >> i;
			while(bin) {
				getline(bin, cur);
				std::istringstream sin(cur);
				sin >> cur;
				if(cur == "DIRECTION")
					sin >> out_c[i].direction;
				else if(cur == "SECTOR")
					sin >> out_c[i].which_sector.x >> out_c[i].which_sector.y;
				else if(cur == "LOCINSECTOR")
					sin >> out_c[i].m_loc.x >> out_c[i].m_loc.y;
				else if(cur == "HOME")
					sin >> out_c[i].home_sector.x >> out_c[i].home_sector.y;
				else if(cur == "-") break;
			}
			out_c[i].what_monst.readFrom(bin);
			out_c[i].exists = true;
		}else if(cur == "CAMPAIGN") {
			unsigned int i, j;
			int val;
			cur = read_maybe_quoted_string(bin);
			bin >> i >> j >> val;
			if(i < 25 && j < 25)
				campaign_flags[cur].idx[i][j] = val;
		} else if(cur == "TIMER") {
			int i;
			bin >> i;
			cTimer timer;
			bin >> timer.time >> timer.global_or_town >> timer.node_to_call;
			party_event_timers.push_back(timer);
		} else if(cur == "CREATURE") {
			int i, j;
			bin >> i >> j;
			creature_save[i][j].active = true;
			creature_save[i][j].readFrom(bin);
		} else if(cur == "STORED") {
			int i, j;
			bin >> i >> j;
			stored_items[i][j].readFrom(bin);
		} else if(cur == "SUMMON") {
			size_t i;
			bin >> i;
			cMonster monst;
			bin >> std::ws;
			monst.readFrom(bin);
			if(i >= summons.size())
				summons.resize(i + 1);
			summons[i] = monst;
		} else if(cur == "JOURNAL") {
			cJournal entry;
			bin >> entry.day;
			entry.in_scen = read_maybe_quoted_string(bin);
			bin >> std::ws;
			getline(bin, entry.the_str);
		} else if(cur == "ENCNOTE") {
			cEncNote note;
			bin >> note.type;
			note.where = read_maybe_quoted_string(bin);
			bin >> std::ws;
			getline(bin, note.the_str);
		} else if(cur == "TALKNOTE") {
			cConvers note;
			while(bin) {
				getline(bin, cur);
				std::istringstream sin(cur);
				sin >> cur;
				if(cur == "WHO")
					note.who_said = read_maybe_quoted_string(bin);
				else if(cur == "WHERE") {
					note.in_town = read_maybe_quoted_string(bin);
					note.in_scen = read_maybe_quoted_string(sin);
				} else if(cur == "-") break;
			}
			bin >> std::ws;
			getline(bin, note.the_str1);
			getline(bin, note.the_str2);
		}
		bin.clear();
	}
}

cPlayer& cParty::operator[](unsigned short n){
	if(n > 6) throw std::out_of_range("Attempt to access a player that doesn't exist.");
	else if(n == 6)
		return adven[0]; // TODO: PC #6 should never be accessed, but bounds checking is rarely done, so this is a quick fix.
	return adven[n];
}

const cPlayer& cParty::operator[](unsigned short n) const{
	if(n > 6) throw std::out_of_range("Attempt to access a player that doesn't exist.");
	else if(n == 6)
		return adven[0]; // TODO: PC #6 should never be accessed, but bounds checking is rarely done, so this is a quick fix.
	return adven[n];
}

// Note that the pointer functions take the pointer with its negative sign stripped off!
void cParty::set_ptr(unsigned short p, unsigned short sdfx, unsigned short sdfy){
	// This function is not used for setting the reserved pointers
	if(p >= 100 && p <= 199){ // must be a mutable pointer
		if(sdfx >= 300) throw std::range_error("SDF x-coordinate out of range (0..299)");
		if(sdfy >= 50) throw std::range_error("SDF y-coordinate out of range (0..49)");
		pointers[p] = std::make_pair(sdfx,sdfy);
	}
	else throw std::range_error("Attempted to assign a pointer out of range (100..199)");
}

void cParty::clear_ptr(unsigned short p) {
	if(p >= 100 && p <= 199) {
		pointers[p] = std::make_pair(-1,-1);
	} else throw std::range_error("Attempted to assign a pointer out of range (100 to 199)");
}

void cParty::force_ptr(unsigned short p, unsigned short sdfx, unsigned short sdfy){
	pointers[p] = std::make_pair(sdfx,sdfy);
}

unsigned char cParty::get_ptr(unsigned short p){
	auto iter = pointers.find(p);
	if(iter == pointers.end()) return 0;
	return stuff_done[iter->second.first][iter->second.second];
}

unsigned char& cParty::cpn_flag(unsigned int x, unsigned int y, std::string id) {
	if(id.empty()) id = univ.scenario.campaign_id;
	if(id.empty()) id = univ.scenario.scen_name;
	if(x >= 25 || y >= 25) throw std::range_error("Attempted to access a campaign flag out of range (0..25)");
	return campaign_flags[id].idx[x][y];
}

bool cParty::is_split(){
	bool ret = false;
	for(int i = 0; i < 6; i++)
		if(!stuff_done[304][i])
			ret = true;
	return ret;
}

bool cParty::pc_present(short i){
	if(i >= 6 || i < 0) return false;
	return stuff_done[304][i];
}

location cParty::left_at(){
	return loc(stuff_done[304][6],stuff_done[304][7]);
}

size_t cParty::left_in(){
	return stuff_done[304][8];
}
extern cUniverse univ;
std::string cParty::start_split(short a,short b,snd_num_t noise,short who) {
	short i;
	
	if(who >= 6 || who < 0) return "";
	if(is_split())
		return "Party already split!";
	stuff_done[304][who] = 0;
	stuff_done[304][6] = univ.town.p_loc.x;
	stuff_done[304][7] = univ.town.p_loc.y;
	stuff_done[304][8] = univ.town.num;
	univ.town.p_loc.x = a;
	univ.town.p_loc.y = b;
	// TODO: This looks like it won't work.
	for(i = 0; i < 6; i++)
		if(!stuff_done[304][who])
			adven[i].main_status += eMainStatus::SPLIT;
	// TODO: Uh, why play sound 10 instead of the one passed in?
	if(noise > 0)
		play_sound(10);
	return "";
}

std::string cParty::end_split(snd_num_t noise) {
	short i;
	
	if(!is_split())
		return "Party already together!";
	univ.town.p_loc = left_at();
	univ.town.num = left_in();
	for(i = 0; i < 6; i++){
		if(isSplit(univ.party[i].main_status))
			univ.party[i].main_status -= eMainStatus::SPLIT;
		stuff_done[304][i] = true;
	}
	if(noise > 0)
		play_sound(10);	
	return "You are reunited.";
}

short cParty::pc_present(){
	short ret = 7;
	for(int i = 0; i < 6; i++){
		if(stuff_done[304][i] && ret == 7)
			ret = i;
		else if(stuff_done[304][i] && ret < 6)
			ret = 6;
	}
	if(ret == 7) ret = 6;
	return ret;
}

bool operator==(const cParty::cConvers& one, const cParty::cConvers& two) {
	if(one.who_said != two.who_said) return false;
	if(one.in_town != two.in_town) return false;
	if(one.the_str1 != two.the_str1) return false;
	if(one.the_str2 != two.the_str2) return false;
	if(one.in_scen != two.in_scen) return false;
	return true;
}

bool operator==(const cParty::cJournal& one, const cParty::cJournal& two) {
	if(one.the_str != two.the_str) return false;
	// TODO: Should I compare the day as well?
	if(one.in_scen != two.in_scen) return false;
	return true;
}

bool operator==(const cParty::cEncNote& one, const cParty::cEncNote& two) {
	if(one.type != two.type) return false;
	if(one.the_str != two.the_str) return false;
	if(one.where != two.where) return false;
	if(one.in_scen != two.in_scen) return false;
	return true;
}

std::istream& operator>>(std::istream& in, eEncNoteType& type) {
	std::string name;
	in >> name;
	if(name == "SCEN") type = NOTE_SCEN;
	else if(name == "OUT") type = NOTE_OUT;
	else if(name == "TOWN") type = NOTE_TOWN;
	else if(name == "MONST") type = NOTE_MONST;
	else in.setstate(std::ios_base::failbit);
	return in;
}

std::ostream& operator<<(std::ostream& out, eEncNoteType type) {
	switch(type) {
		case NOTE_SCEN:
			out << "SCEN";
			break;
		case NOTE_OUT:
			out << "OUT";
			break;
		case NOTE_TOWN:
			out << "TOWN";
			break;
		case NOTE_MONST:
			out << "MONST";
			break;
	}
	return out;
}


