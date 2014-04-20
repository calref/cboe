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

#include "classes.h"
#include "oldstructs.h"

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
	party_event_timers.reserve(30);
	for(i = 0; i < 30; i++){
		boats[i] = old.boats[i];
		horses[i] = old.horses[i];
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
		m_noted[i] = old.m_seen[i];
	journal.reserve(50);
	for(i = 0; i < 50; i++){
		cJournal j;
		j.str_num = old.journal_str[i];
		j.day = old.journal_day[i];
		journal.push_back(j);
//		journal[i].str_num = old.journal_str[i];
//		journal[i].day = old.journal_day[i];
		spec_items[i] = old.spec_items[i];
	}
	special_notes.reserve(140);
	for(i = 0; i < 140; i++){
		cEncNote n;
		n.str_num = old.special_notes_str[i][0];
		n.where = old.special_notes_str[i][1];
		special_notes.push_back(n);
//		special_notes[i].str_num = old.special_notes_str[i][0];
//		special_notes[i].where = old.special_notes_str[i][1];
	}
	talk_save.reserve(120);
	for(i = 0; i < 120; i++){
		cConvers t;
		t = old.talk_save[i];
		talk_save.push_back(t);
//		talk_save[i] = old.talk_save[i];
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
	return *this;
}

void cParty::append(legacy::stored_items_list_type& old,short which_list){
	for(int i = 0; i < 115; i++)
		stored_items[which_list][i] = old.items[i];
}

void cParty::append(legacy::setup_save_type& old){
	for(int n = 0; n < 4; n++)
		for(int i = 0; i < 64; i++)
			for(int j = 0; j < 64; j++)
				setup[n][i][j] = old.setup[n][i][j];
}

cParty::cConvers& cParty::cConvers::operator = (legacy::talk_save_type old){
	personality = old.personality;
	town_num = old.town_num;
	str_num1 = old.str1;
	str_num2 = old.str2;
	return *this;
}

void cParty::add_pc(legacy::pc_record_type old){
	for(int i = 0; i < 6; i++)
		if(adven[i].main_status == 0){
			adven[i] = old;
			break;
		}
}

void cParty::void_pcs(){
	for(int i = 0; i < 6; i++)
		adven[i].main_status = MAIN_STATUS_ABSENT;
}

void cParty::add_pc(cPlayer new_pc){
	for(int i = 0; i < 6; i++)
		if(adven[i].main_status == MAIN_STATUS_ABSENT){
			adven[i] = new_pc;
			break;
		}
}

bool cParty::has_talk_save(short who, short where, short str1, short str2){
	cConvers match = {who, where, str1, str2};
	if(std::find(talk_save.begin(), talk_save.end(), match) != talk_save.end())
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
	talk.in_scen = scen_name;
	// TODO: locate the strings and store them in the record.
	if(std::find(talk_save.begin(), talk_save.end(), talk) == talk_save.end()) {
		talk_save.push_back(talk);
	}
	return true;
}

bool cParty::add_to_journal(short event, short day){
	if(journal.size() == journal.max_size()) return false; // Practically impossible
	cJournal entry;
	entry.str_num = event;
	entry.day = day;
	entry.in_scen = scen_name;
	// TODO: locate the strings and store them in the record.
	if(std::find(journal.begin(), journal.end(), entry) == journal.end()) {
		journal.push_back(entry);
	}
	return true;
}

bool cParty::record(eEncNoteType type, short what, short where){
	if(special_notes.size() == special_notes.max_size()) return false; // Never happen
	cEncNote note;
	note.type = type;
	note.str_num = what;
	note.where = where;
	note.in_scen = scen_name;
	// TODO: locate the strings and store them in the record.
	if(std::find(special_notes.begin(), special_notes.end(), note) == special_notes.end()) {
		special_notes.push_back(note);
	}
	return true;
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

void cParty::writeTo(std::ostream& file){
	file << "AGE " << age << '\n';
	file << "GOLD " << gold << '\n';
	file << "FOOD " << food << '\n';
	for(int i = 0; i < 310; i++)
		for(int j = 0; j < 50; j++)
			if(stuff_done[i][j] > 0)
				file << "SDF " << i << ' ' << j << ' ' << unsigned(stuff_done[i][j]) << '\n';
	for(ptrIter iter = pointers.begin(); iter != pointers.end(); iter++)
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
	for(unsigned int i = 0; i < 250; i++)
		if(graphicUsed[i])
			file << "GRAPHIC " << i << '\n';
	file << '\f';
	for(int i = 0; i < 30; i++){
		if(boats[i].exists) {
			file << "BOAT " << i << '\n';
			boats[i].writeTo(file);
		}
	}
	file << '\f';
	for(int i = 0; i < 30; i++){
		if(horses[i].exists) {
			file << "HORSE " << i << '\n';
			horses[i].writeTo(file);
		}
	}
	file << '\f';
	for(int i = 0; i < 5; i++)
		for(int j = 0; j < 50; j++)
			if(magic_store_items[i][j].variety > ITEM_TYPE_NO_ITEM){
				file << "MAGICSTORE " << i << ' ' << j << '\n';
				magic_store_items[i][j].writeTo(file);
			}
	file << '\f';
	for(int i = 0; i < 10; i++)
		if(out_c[i].exists){
			file << "ENCOUNTER " << i << "\n";
			file << "DIRECTION " << out_c[i].direction << '\n';
			file << "SECTOR " << out_c[i].which_sector.x << ' ' << out_c[i].which_sector.y << '\n';
			file << "LOCINSECTOR " << out_c[i].m_loc.x << ' ' << out_c[i].m_loc.y << '\n';
			file << "HOME " << out_c[i].home_sector.x << ' ' << out_c[i].home_sector.y << '\n';
			out_c[i].what_monst.writeTo(file);
		}
	file << '\f';
	for(campIter iter = campaign_flags.begin(); iter != campaign_flags.end(); iter++){
		for(unsigned int i = 0; i < iter->second.size(); i++)
			if(iter->second[i] > 0)
				file << "CAMPAIGN \"" << iter->first << "\" " << i << ' ' << iter->second[i] << '\n';
	}
	file << '\f';
	for(unsigned int i = 0; i < party_event_timers.size(); i++)
		file << "TIMER " << ' ' << party_event_timers[i].time << ' ' << party_event_timers[i].global_or_town
			 << ' ' << party_event_timers[i].node_to_call << '\n';
	file << '\f';
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 64; j++){
			file << setup[i][j][0];
			for(int k = 1; k < 64; k++)
				file << '\t' << setup[i][j][k];
			file << std::endl;
		}
		file << '\f';
	}
	file << '\f';
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 60; j++) {
			if(creature_save[i][j].active > 0) {
				file << "CREATURE " << i << ' ' << j << '\n';
				creature_save[i][j].writeTo(file);
			}
		}
	file << '\f';
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 115; j++)
			if(stored_items[i][j].variety > ITEM_TYPE_NO_ITEM){
				file << "STORED " << i << ' ' << j << '\n';
				stored_items[i][j].writeTo(file);
			}
	if(summons.size() > 0) {
		file << '\f';
		for(cMonster& monst : summons)
			monst.writeTo(file);
	}
	if(journal.size() > 0) {
		file << '\f';
		for(cJournal& entry : journal) {
			file << "JOURNAL " << entry.str_num << ' ' << entry.day << ' ' << entry.in_scen << '\n';
			// TODO: Save the actual string, if the player has asked us to
		}
	}
	if(special_notes.size() > 0) {
		file << '\f';
		for(cEncNote& note : special_notes) {
			file << "ENCNOTE " << note.type << ' ' << note.str_num << ' ' << note.where << '\n';
			// TODO: Save the actual strings, if the player has asked us to
		}
	}
	if(talk_save.size() > 0) {
		file << '\f';
		for(cConvers& note : talk_save) {
			file << "TALKNOTE " << note.str_num1 << ' ' << note.str_num2 << '\n';
			file << "WHO " << note.personality << '\n';
			file << "WHERE " << note.town_num << ' ' << note.in_scen << '\n';
			// TODO: Save the actual strings and names, if the player has asked us to
		}
	}
}

void cParty::readFrom(std::istream& file){
	// TODO: Error-check input
	std::istringstream bin, sin;
	std::string cur;
	getline(file, cur, '\f');
	bin.str(cur);
	while(bin) { // continue as long as no error, such as eof, occurs
		getline(bin, cur);
		sin.str(cur);
		sin >> cur;
		if(cur == "AGE")
			sin >> age;
		else if(cur == "GOLD")
			sin >> gold;
		else if(cur == "FOOD")
			sin >> food;
		else if(cur == "SDF"){
			int i,j;
			sin >> i >> j;
			sin >> stuff_done[i][j];
		}else if(cur == "ITEMTAKEN"){
			int i;
			sin >> i;
			sin >> item_taken[i][0] >> item_taken[i][1] >> item_taken[i][2] >> item_taken[i][3]
				>> item_taken[i][4] >> item_taken[i][5] >> item_taken[i][6] >> item_taken[i][7];
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
		else if(cur == "BOAT"){
			int i;
			sin >> i;
			sin >> boats[i].loc.x >> boats[i].loc.y >> boats[i].loc_in_sec.x >> boats[i].loc_in_sec.y
				>> boats[i].sector.x >> boats[i].sector.y >> boats[i].which_town >> boats[i].property;
			boats[i].exists = true;
		}else if(cur == "HORSE"){
			int i;
			sin >> i;
			sin >> horses[i].loc.x >> horses[i].loc.y >> horses[i].loc_in_sec.x >> horses[i].loc_in_sec.y
				>> horses[i].sector.x >> horses[i].sector.y >> horses[i].which_town >> horses[i].property;
			horses[i].exists = true;
		}else if(cur == "IN")
			sin >> in_boat >> in_horse;
		else if(cur == "MAGICSTORE"){
			int i,j;
			sin >> i >> j >> cur;
			magic_store_items[i][j].readAttrFrom(cur,sin);
		}else if(cur == "ROSTER"){
			int i;
			sin >> i;
			m_noted[i] = true;
		}else if(cur == "SEEN"){
			int i;
			sin >> i;
			m_seen[i] = true;
		}else if(cur == "ENCOUNTER"){
			int i;
			sin >> i >> cur;
			if(cur == "DIRECTION")
				sin >> out_c[i].direction;
			else if(cur == "SECTOR")
				sin >> out_c[i].which_sector.x >> out_c[i].which_sector.y;
			else if(cur == "LOCINSECTOR")
				sin >> out_c[i].m_loc.x >> out_c[i].m_loc.y;
			else if(cur == "HOME")
				sin >> out_c[i].home_sector.x >> out_c[i].home_sector.y;
			else out_c[i].what_monst.readAttrFrom(cur,sin);
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
		}else if(cur == "TOWN"){
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
		else if(cur == "CAMPAIGN"){
			unsigned int i;
			int j;
			if(sin.peek() == '"'){
				sin.get();
				getline(sin, cur, '"');
			}else sin >> cur;
			sin >> i >> j;
			while(campaign_flags[cur].size() < i) campaign_flags[cur].push_back(0);
			campaign_flags[cur][i] = j;
		}else if(cur == "GRAPHIC"){
			int i;
			sin >> i;
			graphicUsed[i] = true;
		}else if(cur == "TIMER"){
			int i;
			sin >> i;
			sin >> party_event_timers[i].time >> party_event_timers[i].global_or_town >> party_event_timers[i].node_to_call;
		}
	}
	getline(file, cur, '\f');
	bin.str(cur);
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 64; j++)
			for(int k = 0; k < 64; k++)
				bin >> setup[i][j][k];
}

cPlayer& cParty::operator[](unsigned short n){
	if(n > 6) throw std::out_of_range("Attempt to access a player that doesn't exist.");
	else if(n == 6) return adven[0]; // TODO: PC #6 should never be accessed, but bounds checking is rarely done, so this is a quick fix.
	return adven[n];
}

void cParty::set_ptr(short p, unsigned short sdfx, unsigned short sdfy){ // This function is not used for setting the reserved pointers
	if(p >= -199 && p <= -100){ // must be a mutable pointer
		if(sdfx >= 300) throw std::range_error("SDF x-coordinate out of range (0..299)");
		if(sdfy >= 50) throw std::range_error("SDF y-coordinate out of range (0..49)");
		pointers[p] = std::make_pair(sdfx,sdfy);
	}
	else throw std::range_error("Pointer out of range (-199 to -100)");
}

void cParty::force_ptr(short p, unsigned short sdfx, unsigned short sdfy){
	pointers[p] = std::make_pair(sdfx,sdfy);
}

unsigned char cParty::get_ptr(short p){
	ptrIter iter = pointers.find(p);
	if(iter == pointers.end()) return 0;
	return stuff_done[iter->second.first][iter->second.second];
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
	for (i = 0; i < 6; i++)
		if (!stuff_done[304][who])
			adven[i].main_status += MAIN_STATUS_SPLIT;
	if (noise > 0)
		play_sound(10);
	return "";
}

std::string cParty::end_split(snd_num_t noise) {
	short i;
	
	if (!is_split())
		return "Party already together!";
	univ.town.p_loc = left_at();
	univ.town.num = left_in();
	for (i = 0; i < 6; i++){
		if (univ.party[i].main_status >= MAIN_STATUS_SPLIT)
			univ.party[i].main_status -= MAIN_STATUS_SPLIT;
		stuff_done[304][i] = true;
	}
	if (noise > 0)
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
	if(one.personality != two.personality) return false;
	if(one.town_num != two.town_num) return false;
	if(one.str_num1 != two.str_num1) return false;
	if(one.str_num2 != two.str_num2) return false;
	if(one.in_scen != two.in_scen) return false;
	return true;
}

bool operator==(const cParty::cJournal& one, const cParty::cJournal& two) {
	if(one.str_num != two.str_num) return false;
	// TODO: Should I compare the day as well?
	if(one.in_scen != two.in_scen) return false;
	return true;
}

bool operator==(const cParty::cEncNote& one, const cParty::cEncNote& two) {
	if(one.type != two.type) return false;
	if(one.str_num != two.str_num) return false;
	if(one.where != two.where) return false;
	if(one.in_scen != two.in_scen) return false;
	return true;
}

