/*
 *  party.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "party.hpp"

#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>

#include "scenario.hpp"
#include "universe.hpp"
#include "strdlog.hpp"
#include "oldstructs.hpp"
#include "fileio.hpp"
#include "mathutil.hpp"

cParty::cParty(ePartyPreset party_preset) {
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
	out_loc.x = 84;
	out_loc.y = 84;
	in_boat = -1;
	in_horse = -1;
	std::memset(stuff_done, 0, sizeof(stuff_done));
	std::fill(magic_ptrs.begin(), magic_ptrs.end(), 0);
	for(int i = 0; i < 10; i++)
		out_c[i].exists = false;
	for(int i = 0; i < 6; i++)
		adven[i] = new cPlayer(*this, party_preset, i);
	for(auto& monst : imprisoned_monst)
		monst = 0;
}

cParty::~cParty() {
	for(int i = 0; i < 6; i++) {
		delete adven[i];
		adven[i] = nullptr;
	}
}

cParty::cParty(const cParty& other)
	: iLiving(other)
	, next_pc_id(other.next_pc_id)
	, age(other.age)
	, gold(other.gold)
	, food(other.food)
	, hostiles_present(other.hostiles_present)
	, easy_mode(other.easy_mode)
	, less_wm(other.less_wm)
	, show_junk_bag(other.show_junk_bag)
	, magic_ptrs(other.magic_ptrs)
	, light_level(other.light_level)
	, outdoor_corner(other.outdoor_corner)
	, i_w_c(other.i_w_c)
	, out_loc(other.out_loc)
	, town_loc(other.town_loc)
	, loc_in_sec(other.loc_in_sec)
	, town_num(other.town_num)
	, boats(other.boats)
	, horses(other.horses)
	, creature_save(other.creature_save)
	, in_boat(other.in_boat)
	, in_horse(other.in_horse)
	, out_c(other.out_c)
	, magic_store_items(other.magic_store_items)
	, store_limited_stock(other.store_limited_stock)
	, job_banks(other.job_banks)
	, imprisoned_monst(other.imprisoned_monst)
	, m_noted(other.m_noted)
	, m_seen(other.m_seen)
	, journal(other.journal)
	, special_notes(other.special_notes)
	, talk_save(other.talk_save)
	, status(other.status)
	, active_quests(other.active_quests)
	, left_at(other.left_at)
	, left_in(other.left_in)
	, direction(other.direction)
	, at_which_save_slot(other.at_which_save_slot)
	, alchemy(other.alchemy)
	, key_times(other.key_times)
	, party_event_timers(other.party_event_timers)
	, spec_items(other.spec_items)
	, total_m_killed(other.total_m_killed)
	, total_dam_done(other.total_dam_done)
	, total_xp_gained(other.total_xp_gained)
	, total_dam_taken(other.total_dam_taken)
	, scen_name(other.scen_name)
	, stored_items(other.stored_items)
	, summons(other.summons)
	, scen_won(other.scen_won)
	, scen_played(other.scen_played)
	, campaign_flags(other.campaign_flags)
	, junk_items(other.junk_items)
	, pointers(other.pointers)
{
	memcpy(stuff_done, other.stuff_done, sizeof(stuff_done));
	memcpy(setup, other.setup, sizeof(setup));
	for(int i = 0; i < 6; i++) {
		adven[i] = new cPlayer(*other.adven[i]);
		adven[i]->join_party(*this);
	}
}

cParty::cParty(cParty&& other) : cParty() {
	swap(other);
}

cParty& cParty::operator=(cParty other) {
	swap(other);
	return *this;
}

void cParty::swap(cParty& other) {
	std::swap(next_pc_id, other.next_pc_id);
	std::swap(age, other.age);
	std::swap(gold, other.gold);
	std::swap(food, other.food);
	std::swap(hostiles_present, other.hostiles_present);
	std::swap(easy_mode, other.easy_mode);
	std::swap(less_wm, other.less_wm);
	std::swap(show_junk_bag, other.show_junk_bag);
	std::swap(magic_ptrs, other.magic_ptrs);
	std::swap(light_level, other.light_level);
	std::swap(outdoor_corner, other.outdoor_corner);
	std::swap(i_w_c, other.i_w_c);
	std::swap(out_loc, other.out_loc);
	std::swap(town_loc, other.town_loc);
	std::swap(loc_in_sec, other.loc_in_sec);
	std::swap(town_num, other.town_num);
	std::swap(boats, other.boats);
	std::swap(horses, other.horses);
	std::swap(creature_save, other.creature_save);
	std::swap(in_boat, other.in_boat);
	std::swap(in_horse, other.in_horse);
	std::swap(out_c, other.out_c);
	std::swap(magic_store_items, other.magic_store_items);
	std::swap(store_limited_stock, other.store_limited_stock);
	std::swap(job_banks, other.job_banks);
	std::swap(imprisoned_monst, other.imprisoned_monst);
	std::swap(m_noted, other.m_noted);
	std::swap(m_seen, other.m_seen);
	std::swap(journal, other.journal);
	std::swap(special_notes, other.special_notes);
	std::swap(talk_save, other.talk_save);
	std::swap(status, other.status);
	std::swap(active_quests, other.active_quests);
	std::swap(left_at, other.left_at);
	std::swap(left_in, other.left_in);
	std::swap(direction, other.direction);
	std::swap(at_which_save_slot, other.at_which_save_slot);
	std::swap(alchemy, other.alchemy);
	std::swap(key_times, other.key_times);
	std::swap(party_event_timers, other.party_event_timers);
	std::swap(spec_items, other.spec_items);
	std::swap(total_m_killed, other.total_m_killed);
	std::swap(total_dam_done, other.total_dam_done);
	std::swap(total_xp_gained, other.total_xp_gained);
	std::swap(total_dam_taken, other.total_dam_taken);
	std::swap(scen_name, other.scen_name);
	std::swap(adven, other.adven);
	std::swap(stored_items, other.stored_items);
	std::swap(summons, other.summons);
	std::swap(scen_won, other.scen_won);
	std::swap(scen_played, other.scen_played);
	std::swap(campaign_flags, other.campaign_flags);
	std::swap(junk_items, other.junk_items);
	std::swap(pointers, other.pointers);
	std::swap(stuff_done, other.stuff_done);
	std::swap(setup, other.setup);
	// Fixup the references of PCs to their party
	for(auto pc : adven)
		pc->join_party(*this);
	for(auto pc : other.adven)
		pc->join_party(other);
}

cVehicle &cParty::get_boat(int id) {
	if (id>=0 && id<boats.size())
		return boats[id];
	static cVehicle bad_vehicle;
	bad_vehicle=cVehicle::bad();
	return bad_vehicle;
}
cVehicle const &cParty::get_boat(int id) const {
	if (id>=0 && id<boats.size())
		return boats[id];
	static cVehicle bad_vehicle=cVehicle::bad();
	return bad_vehicle;
}
cVehicle &cParty::get_horse(int id) {
	if (id>=0 && id<horses.size())
		return horses[id];
	static cVehicle bad_vehicle;
	bad_vehicle=cVehicle::bad();
	return bad_vehicle;
}
cVehicle const &cParty::get_horse(int id) const  {
	if (id>=0 && id<horses.size())
		return horses[id];
	static cVehicle bad_vehicle=cVehicle::bad();
	return bad_vehicle;
}

cMonster const &cParty::get_summon(mon_num_t id) const
{
	if (id<summons.size())
		return summons[id];
	static cMonster bad_monster=cMonster::bad();
	return bad_monster;
}

cMonster &cParty::get_summon(mon_num_t id) {
	if (id<summons.size())
		return summons[id];
	static cMonster bad_monster;
	bad_monster=cMonster::bad();
	return bad_monster;
}

cItem const &cParty::get_junk_item(item_num_t id) const
{
	if (id<junk_items.size())
		return junk_items[id].first;
	static cItem bad_item=cItem::bad();
	return bad_item;
}

cItem &cParty::get_junk_item(item_num_t id)
{
	if (id<junk_items.size())
		return junk_items[id].first;
	static cItem bad_item;
	bad_item=cItem::bad();
	return bad_item;
}

bool cParty::is_junk_item_compatible_with_town(item_num_t id, int townId) const
{
	if (id<junk_items.size())
		return junk_items[id].second.count(townId)>0;
	return false;
}

static bool combine_items(cItem &item1, cItem const &item2)
{
	if (item1.variety != eItemType::NO_ITEM && item1.ident && item2.ident &&
		item2.variety != eItemType::NO_ITEM && item1.type_flag == item2.type_flag &&
		item1.name == item2.name && item1.special_class == item2.special_class) {
		short test = item1.charges + item2.charges;
		if(test > 125) {
			item1.charges = 125;
			if(cParty::print_result)
				cParty::print_result("(Can have at most 125 of any item.");
		}
		else item1.charges += item2.charges;
		return true;
	}
	return false;
}

bool cParty::give_junk_item(cItem const &item, int townId) {
	if(item.variety == eItemType::NO_ITEM)
		return true;
	if(item.variety == eItemType::GOLD || item.variety == eItemType::FOOD || item.variety == eItemType::QUEST || item.variety == eItemType::SPECIAL) {
		if(print_result)
			print_result("Trying to add unexpected items in junk bag.");
		return false;
	}
	cItem fItem(item);
	fItem.property = false;
	fItem.contained = false;
	fItem.held = false;
	if (fItem.charges<0) fItem.charges=1;
	if(fItem.type_flag > 0 && fItem.ident) {
		for(auto &jItemSet : junk_items) {
			if(combine_items(jItemSet.first, fItem)) {
				if (townId>=0) jItemSet.second.insert(townId);
				return true;
			}
		}
	}
	std::set<int> listTowns;
	if (townId>=0) listTowns.insert(townId);
	junk_items.push_back(std::make_pair(fItem,listTowns));
	return true;
}

void cParty::combine_junk_items()
{
	// FIXME: probably better to first construct a multimap of name => id, items for identify id
	//        then for each item which shares the same names check if we combine them and keep a set
	//        of item id to remove, then we can remove the item
	for (size_t i=junk_items.size(); i>0; --i) {
		auto const &item=junk_items[i-1].first;
		if (!item.ident) continue;
		for (size_t j=i-1; j>0; --j) {
			if (!combine_items(junk_items[j-1].first, item))
				continue;
			if (i!=j)
				junk_items[j-1].second.insert(junk_items[i-1].second.begin(), junk_items[i-1].second.end());
			std::swap(junk_items[i-1],junk_items.back());
			junk_items.pop_back();
			break;
		}
	}
}

void cParty::take_junk_item(item_num_t id)
{
	if (id>=junk_items.size()) {
		if(print_result)
			print_result("Trying to remove unexistant item in junk bag.");
		return;
	}
	junk_items.erase(junk_items.begin()+id);
}

void cParty::import_legacy(legacy::party_record_type const & old, cUniverse& univ){
	scen_name = old.scen_name;
	age = old.age;
	gold = old.gold;
	food = old.food;
	for(short i = 0; i < 310; i++)
		for(short j = 0; j < 10; j++)
			stuff_done[i][j] = old.stuff_done[i][j];
	light_level = old.light_level;
	if(stuff_done[305][0] > 0)
		status[ePartyStatus::STEALTH] = stuff_done[305][0];
	if(stuff_done[305][1] > 0)
		status[ePartyStatus::FLIGHT] = stuff_done[305][1];
	if(stuff_done[305][2] > 0)
		status[ePartyStatus::DETECT_LIFE] = stuff_done[305][2];
	if(stuff_done[305][3] > 0)
		status[ePartyStatus::FIREWALK] = stuff_done[305][3];
	outdoor_corner.x = old.outdoor_corner.x;
	outdoor_corner.y = old.outdoor_corner.y;
	i_w_c.x = old.i_w_c.x;
	i_w_c.y = old.i_w_c.y;
	out_loc.x = old.p_loc.x;
	out_loc.y = old.p_loc.y;
	loc_in_sec.x = old.loc_in_sec.x;
	loc_in_sec.y = old.loc_in_sec.y;
	if(stuff_done[304][0]) {
		left_at = loc(stuff_done[304][1], stuff_done[304][2]);
		left_in = -1;
	}
	party_event_timers.reserve(30);
	boats.resize(30);
	horses.resize(30);
	for(short i = 0; i < 30; i++){
		boats[i].import_legacy(old.boats[i]);
		horses[i].import_legacy(old.horses[i]);
		// boe: timer with time<=0 are empty timer
		if (old.party_event_timers[i] <= 0)
			continue;
		cTimer t;
		t.time = old.party_event_timers[i];
		t.node_type = old.global_or_town[i] ? eSpecCtxType::TOWN : eSpecCtxType::SCEN;
		t.node = old.node_to_call[i];
		party_event_timers.push_back(t);
	}
	std::set<int> seen_towns;
	for(short i = 0; i < 4; i++){
		creature_save[i].import_legacy(old.creature_save[i]);
		// check if the town has already been saved, if yes, sets it to unknown: 200
		// ie. old save can have many towns 0, we want only to use the first one
		if (seen_towns.find(creature_save[i].which_town)!=seen_towns.end())
			creature_save[i].which_town=200;
		else
			seen_towns.insert(creature_save[i].which_town);
		imprisoned_monst[i] = old.imprisoned_monst[i];
	}
	in_boat = old.in_boat;
	in_horse = old.in_horse;
	for(short i = 0; i < 10; i++){
		out_c[i].import_legacy(old.out_c[i]);
		for(short j = 0; j < 5; j++)
			magic_store_items[j][i].import_legacy(old.magic_store_items[j][i]);
	}
	for(short i = 0; i < 256; i++)
		if(old.m_seen[i])
			m_noted.insert(i);
	if(!scen_name.empty()) {
		special_notes.reserve(140);
		for(short i = 0; i < 140; i++){
			if(old.special_notes_str[i][0] <= 0) continue;
			cEncNote n;
			n.import_legacy(old.special_notes_str[i], univ.scenario);
			special_notes.push_back(n);
		}
		talk_save.reserve(120);
		for(short i = 0; i < 120; i++){
			cConvers t;
			t.import_legacy(old.talk_save[i], univ.scenario);
			talk_save.push_back(t);
		}
	}
	direction = eDirection(old.direction);
	at_which_save_slot = old.at_which_save_slot;
	for(short i = 0; i < 20 ; i++)
		alchemy[i] = old.alchemy[i];
	for (short i=0; i<50; i++) {
		if (old.spec_items[i]>0)
			spec_items.insert(i);
	}
	for(short i = 0; i < univ.scenario.towns.size(); i++){
		univ.scenario.towns[i]->can_find = old.can_find_town[i];
		univ.scenario.towns[i]->m_killed = old.m_killed[i];
		for(short j = 0; j < 64; j++)
			univ.scenario.towns[i]->set_item_taken(j, old.item_taken[i][j / 8] & (1 << j % 8));
	}
	for(short i = 0; i < 100; i++)
		key_times[i] = old.key_times[i];
	total_m_killed = old.total_m_killed;
	total_dam_done = old.total_dam_done;
	total_xp_gained = old.total_xp_gained;
	total_dam_taken = old.total_dam_taken;
}

void cParty::import_legacy(legacy::stored_items_list_type const & old,short which_list){
	stored_items[which_list].resize(115);
	for(int i = 0; i < 115; i++)
		stored_items[which_list][i].import_legacy(old.items[i]);
}

void cParty::import_legacy(legacy::setup_save_type const & old){
	for(int n = 0; n < 4; n++)
		for(int i = 0; i < 64; i++)
			for(int j = 0; j < 64; j++)
				// boe stored here misc_i, ...
				// unsure maybe we want here (old&0xfe)<<8
				setup[n][i][j] = (old.setup[n][i][j]<<8);
}

void cParty::cConvers::import_legacy(legacy::talk_save_type const &old, const cScenario& scenario){
	if (old.personality<0) // fixme: add an error message here
		return;
	cTown const *town=scenario.towns[old.personality/10];
	who_said = town->talking.people[old.personality%10].title;
	in_town = scenario.towns[old.town_num]->name;
	for(int i = 0; i < 2; i++) {
		int num=i==0 ? old.str1 : old.str2;
		std::string &str=i==0 ? the_str1 : the_str2;
		// Okay, so there's a ton of different places where the actual strings might be found.
		// 0 means no string
		// 10 + n is the "look" string for the nth personality in the town (ie, n is personality % 10)
		// 20 + n is the "name" string for the nth personality in the town
		// 30 + n is the "job" string for the nth personality in the town
		// 40 + 2n is the first string from the nth talk node in the town
		// 40 + 2n + 1 is the second string from the nth talk not in the town
		// 2000 + n is the nth town special text
		// 3000 + n is the nth scenario special text
		if(num == 0) continue;
		if(num >= 3000)
			str.assign(scenario.get_special_string(num - 3000));
		else if(num >= 2000)
			str.assign(town->get_special_string(num - 2000));
		else if(num >= 40 && num % 2 == 0)
			str.assign(town->talking.talk_nodes[(num - 40) / 2].str1);
		else if(num >= 40 && num % 2 == 1)
			str.assign(town->talking.talk_nodes[(num - 40) / 2].str2);
		else if(num >= 30)
			str.assign(town->talking.people[num-30].job);
		else if(num >= 20)
			str.assign(town->talking.people[num-20].name);
		else if(num >= 10)
			str.assign(town->talking.people[num-10].look);
	}
}

void cParty::cEncNote::import_legacy(int16_t const (& old)[2], const cScenario& scenario) {
	in_scen = scenario.scen_name;
	// TODO: Need to verify that I have the correct offsets here.
	switch(old[0] / 1000) {
		case 0:
			the_str = scenario.get_special_string(old[0] - 160);
			where = scenario.scen_name; // Best we can do here; the actual location is long forgotten
			type = NOTE_SCEN;
			break;
		case 1:
			the_str = scenario.outdoors[old[1] % scenario.outdoors.width()][old[1] / scenario.outdoors.width()]->get_special_string(old[0] - 1010);
			where = scenario.outdoors[old[1] % scenario.outdoors.width()][old[1] / scenario.outdoors.width()]->name;
			type = NOTE_OUT;
			break;
		case 2:
			the_str = scenario.towns[old[1]]->get_special_string(old[0] - 2020);
			where = scenario.towns[old[1]]->name;
			type= NOTE_TOWN;
			break;
	}
}

void cParty::import_legacy(legacy::pc_record_type const (& old)[6]) {
	for(int i = 0; i < 6; i++) {
		delete adven[i];
		adven[i] = new cPlayer(*this);
		adven[i]->import_legacy(old[i]);
	}
}

void cParty::enter_scenario(cScenario const &scenario)
{
	using namespace std::placeholders;
	
	age = 0;
	memset(stuff_done, 0, sizeof(stuff_done));
	light_level = 0;
	outdoor_corner = scenario.out_sec_start;
	i_w_c = {0, 0};
	loc_in_sec = scenario.out_start;
	out_loc = scenario.out_start;
	boats.clear();
	horses.clear();
	std::copy_if(scenario.boats.begin(), scenario.boats.end(), std::back_inserter(boats), std::bind(&cVehicle::exists, _1));
	std::copy_if(scenario.horses.begin(), scenario.horses.end(), std::back_inserter(horses), std::bind(&cVehicle::exists, _1));
	for(auto pc : adven) {
		if (!pc) continue;
		pc->status.clear();
		if(isSplit(pc->main_status))
			pc->main_status -= eMainStatus::SPLIT;
		pc->cur_health = pc->max_health;
		pc->cur_sp = pc->max_sp;
	}
	in_boat = -1;
	in_horse = -1;
	for(auto& pop : creature_save) {
		pop.clear();
		pop.which_town = 200;
	}
	for(auto &creat : out_c)
		creat.exists = false;
	store_limited_stock.clear();
	magic_store_items.clear();
	
	journal.clear();
	special_notes.clear();
	talk_save.clear();
	// reset the scried monster
	m_noted.clear();
	
	direction = DIR_N;
	at_which_save_slot = 0;
	key_times.clear();
	party_event_timers.clear();
	spec_items.clear();
	for(short i = 0; i < scenario.special_items.size(); i++) {
		if(scenario.special_items[i].flags >= 10)
			spec_items.insert(i);
	}
	for(short i = 0; i < scenario.quests.size(); i++) {
		if(scenario.quests[i].flags >= 10) {
			active_quests[i] = cJob(1);
		}
	}
	for(auto &item : stored_items)
		item.clear();
}

void cParty::new_pc(size_t spot) {
	replace_pc(spot, new cPlayer(*this));
	if (spot < 6)
		adven[spot]->main_status = eMainStatus::ALIVE;
}

void cParty::replace_pc(size_t spot, cPlayer* with) {
	if(spot < 6 && with != nullptr) {
		with->join_party(*this);
		delete adven[spot];
		adven[spot] = with;
	}
}

size_t cParty::free_space() {
	for(int i = 0; i < 6; i++)
		if(adven[i]->main_status == eMainStatus::ABSENT)
			return i;
	return 6;
}

size_t cParty::count(eMainStatus type) {
	size_t sz = 0;
	for(int i = 0; i < 6; i++)
		if(adven[i]->main_status == type)
			sz++;
	return sz;
}

void cParty::swap_pcs(size_t a, size_t b) {
	if(a < 6 && b < 6)
		std::swap(adven[a], adven[b]);
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

void cParty::apply_status(eStatus which, int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->apply_status(which, how_much);
}

void cParty::clear_bad_status() {
	for(int i = 0; i < 6; i++)
		adven[i]->clear_bad_status();
}

bool cParty::is_alive() const {
	for(int i = 0; i < 6; i++)
		if(adven[i]->is_alive())
			return true;
	return false;
}

bool cParty::is_friendly() const {
	return true;
}

bool cParty::is_friendly(const iLiving& other) const {
	return other.is_friendly();
}

bool cParty::is_shielded() const {
	return false;
}

int cParty::get_shared_dmg(int) const {
	return 0;
}

int cParty::get_health() const {
	int health = 0;
	for(auto pc : adven)
		if (pc && pc->is_alive())
			health += pc->cur_health;
	return health;
}

int cParty::get_magic() const {
	int magic = 0;
	for(auto pc : adven)
		if (pc && pc->is_alive())
			magic += pc->cur_sp;
	return magic;
}

int cParty::get_level() const {
	short j = 0;
	for(auto pc : adven)
		if (pc && pc->main_status == eMainStatus::ALIVE)
			j += pc->level;
	return j;
}

void cParty::heal(int how_much) {
	for(auto pc : adven)
		if (pc) pc->heal(how_much);
}

void cParty::poison(int how_much) {
	for(auto pc : adven)
		if (pc) pc->poison(how_much);
}

void cParty::cure(int how_much) {
	for(auto pc : adven)
		if (pc) pc->cure(how_much);
}

void cParty::acid(int how_much) {
	for(auto pc : adven)
		if (pc) pc->acid(how_much);
}

void cParty::curse(int how_much) {
	for(auto pc : adven)
		if (pc) pc->curse(how_much);
}

void cParty::slow(int how_much) {
	for(auto pc : adven)
		if (pc) pc->slow(how_much);
}

void cParty::web(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->web(how_much);
}

void cParty::disease(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->disease(how_much);
}

void cParty::dumbfound(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->dumbfound(how_much);
}

void cParty::sleep(eStatus type, int how_much, int adj) {
	if(type == eStatus::FORCECAGE) {
		int who = 0;
		int best = 0;
		for(int i = 0; i < 6; i++) {
			int cur = adven[i]->skill(eSkill::MAGE_LORE) + adven[i]->skill(eSkill::MAGE_SPELLS) + adven[i]->skill(eSkill::PRIEST_SPELLS);
			if(adven[i]->is_alive() && cur > best) {
				best = cur;
				who = i;
			}
		}
		adven[who]->sleep(type, how_much, adj);
		for(int i = 0; i < 6; i++)
			adven[i]->status[eStatus::FORCECAGE] = adven[who]->status[eStatus::FORCECAGE];
		return;
	}
	for(int i = 0; i < 6; i++)
		adven[i]->sleep(type, how_much, adj);
}

void cParty::scare(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->scare(how_much);
}

void cParty::avatar() {
	for(int i = 0; i < 6; i++)
		adven[i]->avatar();
}

void cParty::drain_sp(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->drain_sp(how_much);
}

void cParty::restore_sp(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->restore_sp(how_much);
}

location cParty::get_loc() const {
	if(town_num == 200)
		return out_loc;
	return town_loc;
}

int cParty::calc_day() const {
	return int(age / 3700) + 1;
}

bool cParty::give_item(cItem item,int flags) {
	for(int i = 0; i < 6; i++) {
		if(adven[i]->give_item(item,flags))
			return true;
	}
	return false;
}

// TODO: Utilize the second parameter in special node processing
// if abil > 0, force abil, else ignore
bool cParty::forced_give(cItem item,eItemAbil abil,short dat) {
	if(abil > eItemAbil::NONE) {
		item.ability = abil;
		item.abil_data[0] = dat / 1000;
		item.abil_data[1] = dat % 1000;
	}
	for(cPlayer& pc : *this) {
		if(pc.main_status != eMainStatus::ALIVE) continue;
		for(cItem& slot : pc.items)
			if(slot.variety == eItemType::NO_ITEM) {
				slot = item;
				
				if(print_result) {
					std::ostringstream announce;
					announce << "  " << pc.name << " gets ";
					if(!item.ident)
						announce << item.name;
					else announce << item.full_name;
					announce << '.';
					print_result(announce.str());
				}
				pc.combine_things();
				pc.sort_items();
				return true;
			}
	}
	return false;
}

bool cParty::has_abil(eItemAbil abil, short dat) {
	for(int i = 0; i < 6; i++)
		if(adven[i]->main_status == eMainStatus::ALIVE)
			if(adven[i]->has_abil(abil,dat))
				return true;
	return false;
}

bool cParty::take_abil(eItemAbil abil, short dat) {
	for(int i = 0; i < 6; i++)
		if(adven[i]->main_status == eMainStatus::ALIVE)
			if(cInvenSlot item = adven[i]->has_abil(abil,dat)) {
				if(item->charges > 1)
					item->charges--;
				else adven[i]->take_item(item.slot);
				return true;
			}
	return false;
}

bool cParty::check_class(unsigned int item_class,bool take) {
	if(item_class == 0)
		return false;
	for(auto& pc : *this)
		if(pc.main_status == eMainStatus::ALIVE)
			if(cInvenSlot item = pc.has_class(item_class)) {
				if(take) {
					if(item->charges > 1)
						item->charges--;
					else pc.take_item(item.slot);
				}
				return true;
			}
	return false;
}

bool cParty::check_junk_class(unsigned int item_class,bool take,int townId) {
	if(!show_junk_bag || item_class == 0)
		return false;
	for (size_t i=0; i<junk_items.size(); ++i) {
		auto &itemSet = junk_items[i];
		if (itemSet.first.special_class!=item_class)
			continue;
		if (townId>=0 && itemSet.second.count(townId)==0)
			continue;
		if (take) {
			if(itemSet.first.charges > 1)
				--itemSet.first.charges;
			else
				take_junk_item(i);
			return true;
		}
	}
	return false;
}

bool cParty::start_timer(short time, spec_num_t node, eSpecCtxType type){
	if(party_event_timers.size() == party_event_timers.max_size()) return false; // Shouldn't be reached
	cTimer t;
	t.time = time;
	t.node_type = type;
	t.node = node;
	party_event_timers.push_back(t);
	return(true);
}

void cParty::writeTo(std::ostream& file) const {
	file << "CREATEVERSION " << std::hex << OBOE_CURRENT_VERSION << std::dec << '\n';
	file << "AGE " << age << '\n';
	file << "GOLD " << gold << '\n';
	file << "FOOD " << food << '\n';
	file << "NEXTID " << next_pc_id << '\n';
	file << "HOSTILES " << int(hostiles_present)  << '\n';
	file << "EASY " << int(easy_mode) << '\n';
	file << "LESSWM " << int(less_wm) << '\n';
	file << "JUNKBAG" << int(show_junk_bag) << "\n";
	for(int i = 0; i < 310; i++)
		for(int j = 0; j < 50; j++)
			if(stuff_done[i][j] > 0)
				file << "SDF " << i << ' ' << j << ' ' << unsigned(stuff_done[i][j]) << '\n';
	for(auto iter = pointers.begin(); iter != pointers.end(); iter++)
		file << "POINTER " << iter->first << ' ' << iter->second.first << ' ' << iter->second.second << '\n';
	for(int i = 0; i < magic_ptrs.size(); i++)
		file << "POINTER " << i+10 << ' ' << int(magic_ptrs[i]) << '\n';
	file << "LIGHT " << light_level << '\n';
	file << "OUTCORNER " << outdoor_corner.x << ' ' << outdoor_corner.y << '\n';
	file << "INWHICHCORNER " << i_w_c.x << ' ' << i_w_c.y << '\n';
	file << "SECTOR " << out_loc.x << ' ' << out_loc.y << '\n';
	file << "LOCINSECTOR " << loc_in_sec.x << ' ' << loc_in_sec.y << '\n';
	file << "IN " << in_boat << ' ' << in_horse << '\n';
	for(auto& kv : status) {
		if(kv.second > 0)
			file << "STATUS " << kv.first << ' ' << kv.second << '\n';
	}
	if(is_split()) {
		file << "SPLIT_LEFT_IN " << left_in << '\n';
		file << "SPLIT_LEFT_AT " << left_at.x << ' ' << left_at.y << '\n';
	}
	for(int i : m_noted)
		file << "ROSTER " << i << '\n';
	for(int i : m_seen)
		file << "SEEN " << i << '\n';
	for(int i = 0; i < 4; i++)
		if(imprisoned_monst[i] > 0)
			file << "SOULCRYSTAL " << i << ' ' << imprisoned_monst[i] << '\n';
	file << "DIRECTION " << direction << '\n';
	file << "WHICHSLOT " << at_which_save_slot << '\n';
	for(int i = 0; i < creature_save.size(); i++) {
		file << "TOWNSAVE " << i << ' ' << creature_save[i].which_town;
		if(creature_save[i].hostile) file << " HOSTILE";
		file << '\n';
	}
	for(int i = 0; i < alchemy.size(); i++)
		if(alchemy[i])
			file << "ALCHEMY " << i << '\n';
	for(auto key : key_times)
		file << "EVENT " << key.first << ' ' << key.second << '\n';
	for(int i : spec_items)
		file << "ITEM " << i << '\n';
	file << "KILLS " << total_m_killed << '\n';
	file << "DAMAGE " << total_dam_done << '\n';
	file << "WOUNDS " << total_dam_taken << '\n';
	file << "EXPERIENCE " << total_xp_gained << '\n';
	file << "SCENARIO " << scen_name << '\n';
	file << "WON " << scen_won << '\n';
	file << "PLAYED " << scen_played << '\n';
	for(auto p : active_quests)
		file << "QUEST " << p.first << ' ' << p.second.status << ' ' << p.second.start << ' ' << p.second.source << '\n';
	for(auto const &p : store_limited_stock) {
		for(auto const &p2 : p.second) {
			file << "SHOPSTOCK " << p.first << ' ' << p2.first << ' ' << p2.second << '\n';
		}
	}
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
	for(int i = 0; i < boats.size(); i++){
		if(boats[i].exists) {
			file << "BOAT " << i << '\n';
			boats[i].writeTo(file);
			file << '\f';
		}
	}
	file << '\f';
	for(int i = 0; i < horses.size(); i++){
		if(horses[i].exists) {
			file << "HORSE " << i << '\n';
			horses[i].writeTo(file);
			file << '\f';
		}
	}
	file << '\f';
	for (size_t i=0; i<junk_items.size(); ++i) {
		if (junk_items[i].first.variety==eItemType::NO_ITEM)
			continue;
		file << "JUNKITEM " << junk_items[i].second.size();
		for (auto town : junk_items[i].second)
			file << " " << town;
		file << '\n';
		junk_items[i].first.writeTo(file);
		file << '\f';
	}
	file << '\f';
	for(auto& p : magic_store_items) {
		for(auto& p2 : p.second)
			if(p2.second.variety != eItemType::NO_ITEM){
				file << "MAGICSTORE " << p.first << ' ' << p2.first << '\n';
				p2.second.writeTo(file);
				file << '\f';
			}
	}
	file << '\f';
	for(int i = 0; i < job_banks.size(); i++) {
		file << "JOBBANK " << i << ' ' << job_banks[i].anger << '\n';
		if(!job_banks[i].inited) continue;
		for(int j = 0; j < 6; j++)
			file << "JOB " << j << ' ' << job_banks[i].jobs[j] << '\n';
	}
	file << '\f';
	for(int i = 0; i < out_c.size(); i++)
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
	for(unsigned int i = 0; i < party_event_timers.size(); i++) {
		if (party_event_timers[i].node<=-200 || (party_event_timers[i].node>=-9 && party_event_timers[i].node<0))
			continue;
		file << "TIMER " << ' ' << party_event_timers[i].time << ' ' << int(party_event_timers[i].node_type)
			 << ' ' << party_event_timers[i].node << '\f';
	}
	file << '\f';
	for(int i = 0; i < creature_save.size(); i++)
		for(int j = 0; j < creature_save[i].size(); j++) {
			if(creature_save[i][j].active > 0) {
				file << "CREATURE " << i << ' ' << j << '\n';
				creature_save[i][j].writeTo(file);
				file << '\f';
			}
		}
	file << '\f';
	for(int i = 0; i < stored_items.size(); i++)
		for(size_t j = 0; j < stored_items[i].size(); j++)
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
			file << "TALKNOTE\n";
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
		std::istringstream sin(cur);
		sin >> cur;
		if(cur == "AGE")
			sin >> age;
		else if(cur == "GOLD")
			sin >> gold;
		else if(cur == "FOOD")
			sin >> food;
		else if(cur == "NEXTID")
			sin >> next_pc_id;
		else if(cur == "SDF"){
			int i,j;
			unsigned int n;
			sin >> i >> j >> n;
			sd_set(i,j,n);
		} else if(cur == "HOSTILES") {
			int n;
			sin >> n;
			hostiles_present = n;
		} else if(cur == "EASY") {
			int n;
			sin >> n;
			easy_mode = n;
		} else if(cur == "LESSWM") {
			int n;
			sin >> n;
			less_wm = n;
		} else if(cur == "JUNKBAG") {
			int n;
			sin >> n;
			show_junk_bag = n;
		} else if(cur == "CREATEVERSION") {
			unsigned long long version;
			sin >> std::hex >> version >> std::dec;
			if(version > OBOE_CURRENT_VERSION) {
				showWarning("This game appears to have been created with a newer version of Blades of Exile than you are running. Exile will do its best to load the saved game anyway, but there may be loss of information.");
			}
		} else if(cur == "POINTER") {
			int i,j,k;
			sin >> i >> j;
			if(i >= 10 && i < 100) {
				magic_ptrs[i-10] = j;
			} else if(i >= 100 && i < 200) {
				sin >> k;
				pointers[i] = std::make_pair(j,k);
			}
		} else if(cur == "STATUS") {
			ePartyStatus stat;
			int n;
			sin >> stat >> n;
			status[stat] = n;
		}else if(cur == "LIGHT")
			sin >> light_level;
		else if(cur == "OUTCORNER")
			sin >> outdoor_corner.x >> outdoor_corner.y;
		else if(cur == "INWHICHCORNER")
			sin >> i_w_c.x >> i_w_c.y;
		else if(cur == "SECTOR")
			sin >> out_loc.x >> out_loc.y;
		else if(cur == "LOCINSECTOR")
			sin >> loc_in_sec.x >> loc_in_sec.y;
		else if(cur == "SPLIT_LEFT_IN")
			sin >> left_in;
		else if(cur == "SPLIT_LEFT_AT")
			sin >> left_at.x >> left_at.y;
		else if(cur == "IN")
			sin >> in_boat >> in_horse;
		else if(cur == "ROSTER"){
			int i;
			sin >> i;
			m_noted.insert(i);
		}else if(cur == "SEEN"){
			int i;
			sin >> i;
			m_seen.insert(i);
		}else if(cur == "SOULCRYSTAL"){
			int i;
			sin >> i;
			if(i < 0 || i >= imprisoned_monst.size()) continue;
			sin >> imprisoned_monst[i];
		}else if(cur == "DIRECTION")
			sin >> direction;
		else if(cur == "WHICHSLOT")
			sin >> at_which_save_slot;
		else if(cur == "ALCHEMY"){
			int i;
			sin >> i;
			if(i >= 0 && i < alchemy.size())
				alchemy[i] = true;
		} else if(cur == "TOWNSAVE") {
			int i;
			std::string str;
			sin >> i;
			if(i < 0 || i >= creature_save.size()) continue;
			sin >> creature_save[i].which_town >> str;
			creature_save[i].hostile = str == "HOSTILE";
		}else if(cur == "EVENT"){
			int i;
			sin >> i;
			sin >> key_times[i];
		}else if(cur == "ITEM"){
			int i;
			sin >> i;
			spec_items.insert(i);
		} else if(cur == "QUEST") {
			int i;
			sin >> i;
			sin >> active_quests[i].status >> active_quests[i].start >> active_quests[i].source;
		} else if(cur == "SHOPSTOCK") {
			int i, j;
			sin >> i >> j >> store_limited_stock[i][j];
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
			if(i >= boats.size() && i<1000)
				boats.resize(i + 1);
			get_boat(i).exists = true;
			get_boat(i).readFrom(bin);
		} else if(cur == "HORSE") {
			int i;
			bin >> i;
			if(i >= horses.size() && i<1000)
				horses.resize(i + 1);
			get_horse(i).exists = true;
			get_horse(i).readFrom(bin);
		} else if(cur == "MAGICSTORE") {
			int i,j;
			bin >> i >> j;
			magic_store_items[i][j].readFrom(bin);
		} else if (cur == "JUNKITEM") {
			junk_items.emplace_back();
			int n;
			bin >> n;
			for (int i=0; i<n; ++i) {
				int town=-1;
				bin >> town;
				if (town<0) break;
				junk_items.back().second.insert(town);
			}
			junk_items.back().first.readFrom(bin);
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
			int j;
			cTimer timer;
			bin >> timer.time >> j >> timer.node;
			timer.node_type = eSpecCtxType(j);
			party_event_timers.push_back(timer);
		} else if(cur == "CREATURE") {
			int i, j;
			bin >> i >> j;
			if(i < 0 || i >= creature_save.size()) continue;
			creature_save[i].init(j);
			creature_save[i][j].readFrom(bin);
		} else if(cur == "STORED") {
			int i, j;
			bin >> i >> j;
			if(i < 0 || i >= 3 || j < 0) continue;
			if(j >= stored_items[i].size())
				stored_items[i].resize(j + 1);
			stored_items[i][j].readFrom(bin);
		} else if(cur == "SUMMON") {
			size_t i;
			bin >> i;
			cMonster monst;
			bin >> std::ws;
			monst.readFrom(bin);
			if(i >= summons.size() && i<1000)
				summons.resize(i + 1);
			get_summon(i) = monst;
		} else if(cur == "JOURNAL") {
			cJournal entry;
			bin >> entry.day;
			entry.in_scen = read_maybe_quoted_string(bin);
			bin >> std::ws;
			getline(bin, entry.the_str);
			journal.push_back(entry);
		} else if(cur == "ENCNOTE") {
			cEncNote note;
			bin >> note.type;
			note.where = read_maybe_quoted_string(bin);
			bin >> std::ws;
			getline(bin, note.the_str);
			special_notes.push_back(note);
		} else if(cur == "TALKNOTE") {
			cConvers note;
			while(bin) {
				getline(bin, cur);
				std::istringstream sin(cur);
				sin >> cur;
				if(cur == "WHO")
					note.who_said = read_maybe_quoted_string(sin);
				else if(cur == "WHERE") {
					note.in_town = read_maybe_quoted_string(sin);
					note.in_scen = read_maybe_quoted_string(sin);
				} else if(cur == "-") break;
			}
			bin >> std::ws;
			getline(bin, note.the_str1);
			getline(bin, note.the_str2);
			talk_save.push_back(note);
		} else if(cur == "JOB_BANK") {
			int i;
			bin >> i;
			if(i < 0) continue;
			if(i >= job_banks.size())
				job_banks.resize(i + 1);
			bin >> job_banks[i].anger;
			job_banks[i].inited = false;
			while(bin) {
				getline(bin, cur);
				std::istringstream sin(cur);
				sin >> cur;
				if(cur == "JOB") {
					job_banks[i].inited = true;
					int j;
					sin >> j;
					if(j < 0 || j >= 6)
						continue;
					sin >> job_banks[i].jobs[j];
				}
			}
		}
		bin.clear();
	}
}

cPlayer& cParty::operator[](unsigned short n){
	if(n > 6)
		throw std::out_of_range("Attempt to access a player that doesn't exist.");
	else if(n == 6)
		return *adven[0]; // TODO: PC #6 should never be accessed, but bounds checking is rarely done, so this is a quick fix.
	return *adven[n];
}

const cPlayer& cParty::operator[](unsigned short n) const{
	if(n > 6) throw std::out_of_range("Attempt to access a player that doesn't exist.");
	else if(n == 6)
		return *adven[0]; // TODO: PC #6 should never be accessed, but bounds checking is rarely done, so this is a quick fix.
	return *adven[n];
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

void cParty::force_ptr(unsigned short p, unsigned short val){
	if(p < 10 || p >= 100)
		throw std::range_error("Magic pointer out of range (10..99)");
	magic_ptrs[p-10] = val;
}

unsigned char cParty::get_ptr(unsigned short p){
	if(p < 10 || p >= 200)
		throw std::range_error("Attempted to access a nonexistent pointer (10..199)");
	if(p < 100)
		return magic_ptrs[p-10];
	auto iter = pointers.find(p);
	if(iter == pointers.end()) return 0;
	return sd(iter->second.first,iter->second.second);
}

bool cParty::is_split() const {
	bool ret = false;
	for(int i = 0; i < 6; i++)
		if(!pc_present(i))
			ret = true;
	return ret;
}

bool cParty::pc_present(short i) const {
	if(i >= 6 || i < 0) return false;
	return !isSplit(adven[i]->main_status);
}

bool cParty::start_split(short x,short y,snd_num_t noise,short who) {
	if(who >= 6 || who < 0) return false;
	if(is_split())
		return false;
	// TODO: Allow splitting an arbitrary subgroup of the party
	left_at = town_loc;
	left_in = town_num;
	town_loc.x = x;
	town_loc.y = y;
	for(short i = 0; i < 6; i++) {
		if(i != who)
			adven[i]->main_status += eMainStatus::SPLIT;
		adven[i]->status[eStatus::FORCECAGE] = 0;
	}
	play_sound(noise);
	return true;
}

bool cParty::end_split(snd_num_t noise) {
	if(!is_split())
		return false;
	for(short i = 0; i < 6; i++){
		if(isSplit(adven[i]->main_status))
			adven[i]->main_status -= eMainStatus::SPLIT;
	}
	play_sound(noise);
	return true;
}

iLiving& cParty::pc_present() const {
	short ret = 7;
	for(int i = 0; i < 6; i++){
		if(pc_present(i) && ret == 7)
			ret = i;
		else if(pc_present(i) && ret < 6)
			ret = 6;
	}
	if(ret >= 6)
		return *const_cast<cParty*>(this);
	return *adven[ret];
}

// stuff done legit, i.e. flags are within proper ranges for stuff done flag
bool cParty::sd_legit(short a, short b) const {
	if((minmax(0,sdx_max,a) == a) && (minmax(0,sdy_max,b) == b))
		return true;
	return false;
}

unsigned char cParty::sd(short a, short b) const
{
	if (!sd_legit(a,b))
		return 0;
	return stuff_done[a][b];
}

void cParty::sd_set(short a, short b, unsigned char val)
{
	if (sd_legit(a,b))
		stuff_done[a][b]=val;
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

