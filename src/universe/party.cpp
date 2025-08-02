/*
 *  party.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "party.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>

#include "scenario/scenario.hpp"
#include "universe/universe.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "oldstructs.hpp"
#include "fileio/fileio.hpp"
#include "fileio/tagfile.hpp"
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
	wipe_sdfs();
	std::fill(magic_ptrs.begin(), magic_ptrs.end(), 0);
	for(int i = 0; i < 10; i++)
		out_c[i].exists = false;
	for(int i = 0; i < 6; i++)
		adven[i].reset(new cPlayer(*this, party_preset, i));
	for(auto& monst : imprisoned_monst)
		monst = 0;
}

cParty::cParty(const cParty& other)
	: iLiving(other)
	, next_pc_id(other.next_pc_id)
	, age(other.age)
	, gold(other.gold)
	, food(other.food)
	, stuff_done(other.stuff_done)
	, hostiles_present(other.hostiles_present)
	, easy_mode(other.easy_mode)
	, less_wm(other.less_wm)
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
	, setup(other.setup)
	, stored_items(other.stored_items)
	, summons(other.summons)
	, scen_won(other.scen_won)
	, scen_played(other.scen_played)
	, campaign_flags(other.campaign_flags)
	, pointers(other.pointers)
{
	for(int i = 0; i < 6; i++) {
		adven[i].reset(new cPlayer(*this, *other.adven[i]));
	}
}

cParty::cParty(cParty&& other) : cParty() {
	swap(*this, other);
}

cParty& cParty::operator=(cParty other) {
	swap(*this, other);
	return *this;
}

void swap(cParty& lhs, cParty& rhs) {
	using std::swap;
	swap(lhs.next_pc_id, rhs.next_pc_id);
	swap(lhs.age, rhs.age);
	swap(lhs.gold, rhs.gold);
	swap(lhs.food, rhs.food);
	swap(lhs.stuff_done, rhs.stuff_done);
	swap(lhs.hostiles_present, rhs.hostiles_present);
	swap(lhs.easy_mode, rhs.easy_mode);
	swap(lhs.less_wm, rhs.less_wm);
	swap(lhs.magic_ptrs, rhs.magic_ptrs);
	swap(lhs.light_level, rhs.light_level);
	swap(lhs.outdoor_corner, rhs.outdoor_corner);
	swap(lhs.i_w_c, rhs.i_w_c);
	swap(lhs.out_loc, rhs.out_loc);
	swap(lhs.town_loc, rhs.town_loc);
	swap(lhs.loc_in_sec, rhs.loc_in_sec);
	swap(lhs.town_num, rhs.town_num);
	swap(lhs.boats, rhs.boats);
	swap(lhs.horses, rhs.horses);
	swap(lhs.creature_save, rhs.creature_save);
	swap(lhs.in_boat, rhs.in_boat);
	swap(lhs.in_horse, rhs.in_horse);
	swap(lhs.out_c, rhs.out_c);
	swap(lhs.magic_store_items, rhs.magic_store_items);
	swap(lhs.store_limited_stock, rhs.store_limited_stock);
	swap(lhs.job_banks, rhs.job_banks);
	swap(lhs.imprisoned_monst, rhs.imprisoned_monst);
	swap(lhs.m_noted, rhs.m_noted);
	swap(lhs.m_seen, rhs.m_seen);
	swap(lhs.journal, rhs.journal);
	swap(lhs.special_notes, rhs.special_notes);
	swap(lhs.talk_save, rhs.talk_save);
	swap(lhs.status, rhs.status);
	swap(lhs.active_quests, rhs.active_quests);
	swap(lhs.left_at, rhs.left_at);
	swap(lhs.left_in, rhs.left_in);
	swap(lhs.direction, rhs.direction);
	swap(lhs.at_which_save_slot, rhs.at_which_save_slot);
	swap(lhs.alchemy, rhs.alchemy);
	swap(lhs.key_times, rhs.key_times);
	swap(lhs.party_event_timers, rhs.party_event_timers);
	swap(lhs.spec_items, rhs.spec_items);
	swap(lhs.total_m_killed, rhs.total_m_killed);
	swap(lhs.total_dam_done, rhs.total_dam_done);
	swap(lhs.total_xp_gained, rhs.total_xp_gained);
	swap(lhs.total_dam_taken, rhs.total_dam_taken);
	swap(lhs.scen_name, rhs.scen_name);
	swap(lhs.setup, rhs.setup);
	swap(lhs.stored_items, rhs.stored_items);
	swap(lhs.summons, rhs.summons);
	swap(lhs.scen_won, rhs.scen_won);
	swap(lhs.scen_played, rhs.scen_played);
	swap(lhs.campaign_flags, rhs.campaign_flags);
	swap(lhs.pointers, rhs.pointers);
	for(size_t i = 0; i < lhs.adven.size(); i++) {
		swap(*lhs.adven[i], *rhs.adven[i]);
	}
}

void cParty::import_legacy(legacy::party_record_type& old, cUniverse& univ){
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
	for(short i = 0; i < 4; i++){
		creature_save[i].import_legacy(old.creature_save[i]);
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

void cParty::import_legacy(legacy::stored_items_list_type& old,short which_list){
	stored_items[which_list].resize(115);
	for(int i = 0; i < 115; i++)
		stored_items[which_list][i].import_legacy(old.items[i]);
}

void cParty::import_legacy(legacy::setup_save_type& old){
	for(int n = 0; n < 4; n++){
		setup[n].resize(64, 64);
		for(int i = 0; i < 64; i++)
			for(int j = 0; j < 64; j++)
				setup[n][i][j] = old.setup[n][i][j];
	}
}

void cParty::cConvers::import_legacy(legacy::talk_save_type old, const cScenario& scenario){
	size_t town = old.personality / 10;
	size_t npc = old.personality % 10;
	if(town >= scenario.towns.size()) return;
	who_said = scenario.towns[town]->talking.people[npc].title;
	in_town = scenario.towns[old.town_num]->name;
	int strnums[2] = {old.str1, old.str2};
	std::string* strs[2] = {&the_str1, &the_str2};
	for(int i = 0; i < 2; i++) {
		// Okay, so there's a ton of different places where the actual strings might be found.
		// 0 means no string
		// 10 + n is the "look" string for the nth personality in the town (ie, n is personality % 10)
		// 20 + n is the "name" string for the nth personality in the town
		// 30 + n is the "job" string for the nth personality in the town
		// 40 + 2n is the first string from the nth talk node in the town
		// 40 + 2n + 1 is the second string from the nth talk not in the town
		// 2000 + n is the nth town special text
		// 3000 + n is the nth scenario special text
		if(strnums[i] == 0) continue;
		if(strnums[i] >= 3000)
			strs[i]->assign(scenario.spec_strs[strnums[i] - 3000]);
		else if(strnums[i] >= 2000)
			strs[i]->assign(scenario.towns[old.personality / 10]->spec_strs[strnums[i] - 2000]);
		else if(strnums[i] >= 40 && strnums[i] % 2 == 0)
			strs[i]->assign(scenario.towns[old.personality / 10]->talking.talk_nodes[(strnums[i] - 40) / 2].str1);
		else if(strnums[i] >= 40 && strnums[i] % 2 == 1)
			strs[i]->assign(scenario.towns[old.personality / 10]->talking.talk_nodes[(strnums[i] - 40) / 2].str2);
		else if(strnums[i] >= 30)
			strs[i]->assign(scenario.towns[old.personality / 10]->talking.people[old.personality % 10].job);
		else if(strnums[i] >= 20)
			strs[i]->assign(scenario.towns[old.personality / 10]->talking.people[old.personality % 10].name);
		else if(strnums[i] >= 10)
			strs[i]->assign(scenario.towns[old.personality / 10]->talking.people[old.personality % 10].look);
	}
}

void cParty::cEncNote::import_legacy(int16_t(& old)[2], const cScenario& scenario) {
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
			where = scenario.outdoors[old[1] % scenario.outdoors.width()][old[1] / scenario.outdoors.width()]->name;
			type = NOTE_OUT;
			break;
		case 2:
			the_str = scenario.towns[old[1]]->spec_strs[old[0] - 2020];
			where = scenario.towns[old[1]]->name;
			type= NOTE_TOWN;
			break;
	}
}

void cParty::import_legacy(legacy::pc_record_type(& old)[6]) {
	for(int i = 0; i < 6; i++) {
		adven[i].reset(new cPlayer(*this));
		adven[i]->import_legacy(old[i]);
	}
}

std::unique_ptr<cPlayer> cParty::remove_pc(size_t spot) {
	if(spot >= 6) return nullptr;
	adven[spot]->party = nullptr;
	return std::move(adven[spot]);
}

void cParty::new_pc(size_t spot) {
	std::unique_ptr<cPlayer> new_pc{new cPlayer(*this)};
	replace_pc(spot, std::move(new_pc));
	adven[spot]->main_status = eMainStatus::ALIVE;
}

void cParty::replace_pc(size_t spot, std::unique_ptr<cPlayer> with) {
	if(spot < 6 && with != nullptr) {
		with->party = this;
		adven[spot] = std::move(with);
	}
}

size_t cParty::free_space() const {
	for(int i = 0; i < 6; i++)
		if(adven[i]->main_status == eMainStatus::ABSENT)
			return i;
	return 6;
}

size_t cParty::count(eMainStatus type) const {
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

bool cParty::is_in_scenario() const {
	return !scen_name.empty();
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
	for(int i = 0; i < 6; i++)
		if(adven[i]->is_alive())
			health += adven[i]->cur_health;
	return health;
}

int cParty::get_magic() const {
	int magic = 0;
	for(int i = 0; i < 6; i++)
		if(adven[i]->is_alive())
			magic += adven[i]->cur_sp;
	return magic;
}

int cParty::get_level() const {
	short j = 0;
	for(int i = 0; i < 6; i++)
		if(adven[i]->main_status == eMainStatus::ALIVE)
			j += adven[i]->level;
	return j;
}

void cParty::heal(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->heal(how_much);
}

void cParty::poison(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->poison(how_much);
}

void cParty::cure(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->cure(how_much);
}

void cParty::acid(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->acid(how_much);
}

void cParty::curse(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->curse(how_much);
}

void cParty::slow(int how_much) {
	for(int i = 0; i < 6; i++)
		adven[i]->slow(how_much);
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

void cParty::drain_sp(int how_much, bool allow_resist) {
	for(int i = 0; i < 6; i++)
		adven[i]->drain_sp(how_much, allow_resist);
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
	return (age / 3700) + 1;
}

bool cParty::give_item(cItem item,int flags) {
	for(int i = 0; i < 6; i++) {
		if(adven[i]->give_item(item,flags) == eBuyStatus::OK)
			return true;
	}
	return false;
}

// TODO: Utilize the second parameter in special node processing
// if abil > 0, force abil, else ignore
bool cParty::forced_give(cItem item,eItemAbil abil,short dat) {
	if(abil > eItemAbil::NONE) {
		item.ability = abil;
		item.abil_strength = dat / 1000;
		item.abil_data.value = dat % 1000;
	}
	// TODO: It's strange to check main_status in the inner loop here rather than the outer loop
	for(cPlayer& pc : *this)
		for(cItem& slot : pc.items)
			if(pc.main_status == eMainStatus::ALIVE && slot.variety == eItemType::NO_ITEM) {
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
	return false;
}

bool cParty::all_items_identified() {
	bool all_identified = true;
	for(cPlayer& pc : *this)
		for(cItem& item : pc.items)
			if (item.variety != eItemType::NO_ITEM)
				all_identified &= item.ident;
	return all_identified;
}

bool cParty::has_abil(eItemAbil abil, short dat) const {
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
				if(item->charges > 1 || item->rechargeable)
					item->charges--;
				else adven[i]->take_item(item.slot);
				return true;
			}
	return false;
}

bool cParty::has_class(unsigned int item_class, bool require_charges) {
	if(item_class == 0)
		return false;
	for(auto& pc : *this)
		if(pc.main_status == eMainStatus::ALIVE)
			if(cInvenSlot item = pc.has_class(item_class, require_charges)) {
				return true;
			}
	return false;
}

bool cParty::take_class(unsigned int item_class) const {
	if(item_class == 0)
		return false;
	for(auto& pc : *this)
		if(pc.main_status == eMainStatus::ALIVE)
			if(cInvenSlot item = pc.has_class(item_class, true)) {
				if(item->charges > 1 || item->rechargeable)
					item->charges--;
				else pc.take_item(item.slot);
				return true;
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

void cParty::writeTo(cTagFile& file) const {
	auto& main_page = file.add();
	main_page["CREATEVERSION"] << as_hex<unsigned long long>(OBOE_CURRENT_VERSION);
	main_page["AGE"] << age;
	main_page["GOLD"] << gold;
	main_page["FOOD"] << food;
	main_page["NEXTID"] << next_pc_id;
	main_page["HOSTILES"] << hostiles_present;
	main_page["EASY"] << easy_mode;
	main_page["LESSWM"] << less_wm;
	for(int i = 0; i < 310; i++) {
		for(int j = 0; j < 50; j++) {
			if(stuff_done[i][j] > 0) {
				main_page["SDF"] << i << j << stuff_done[i][j];
			}
		}
	}
	for(auto iter = pointers.begin(); iter != pointers.end(); iter++) {
		main_page["POINTER"] << iter->first << iter->second.first << iter->second.second;
	}
	for(int i = 0; i < magic_ptrs.size(); i++) {
		main_page["POINTER"] << i + 10 << magic_ptrs[i];
	}
	main_page["LIGHT"] << light_level;
	main_page["OUTCORNER"] << outdoor_corner.x << outdoor_corner.y;
	main_page["INWHICHCORNER"] << i_w_c.x << i_w_c.y;
	main_page["SECTOR"] << out_loc.x << out_loc.y;
	main_page["LOCINSECTOR"] << loc_in_sec.x << loc_in_sec.y;
	main_page["IN"] << in_boat << in_horse;
	main_page["STATUS"].encodeSparse(status);
	if(is_split()) {
		main_page["SPLIT_LEFT_IN"] << left_in;
		main_page["SPLIT_LEFT_AT"] << left_at.x << left_at.y;
	}
	for(int i : m_noted) {
		main_page["ROSTER"] << i;
	}
	for(int i : m_seen) {
		main_page["SEEN"] << i;
	}
	for(int i = 0; i < 4; i++) {
		if(imprisoned_monst[i] > 0) {
			main_page["SOULCRYSTAL"] << i << imprisoned_monst[i];
		}
	}
	main_page["DIRECTION"] << direction;
	main_page["WHICHSLOT"] << at_which_save_slot;
	for(int i = 0; i < creature_save.size(); i++) {
		auto tmp = main_page["TOWNSAVE"] << i << creature_save[i].which_town;
		if(creature_save[i].hostile) tmp << "HOSTILE";
	}
	for(int i = 0; i < alchemy.size(); i++) {
		if(alchemy[i]) {
			main_page["ALCHEMY"] << i;
		}
	}
	main_page["EVENT"].encodeSparse(key_times);
	for(int i : spec_items) {
		main_page["ITEM"] << i;
	}
	main_page["KILLS"] << total_m_killed;
	main_page["DAMAGE"] << total_dam_done;
	main_page["WOUNDS"] << total_dam_taken;
	main_page["EXPERIENCE"] << total_xp_gained;
	main_page["SCENARIO"] << scen_name;
	main_page["WON"] << scen_won;
	main_page["PLAYED"] << scen_played;
	for(auto p : active_quests) {
		main_page["QUEST"] << p.first << p.second.status << p.second.start << p.second.source;
	}
	for(auto p : store_limited_stock) {
		for(auto p2 : p.second) {
			main_page["SHOPSTOCK"] << p.first << p2.first << p2.second;
		}
	}
	for(auto iter = campaign_flags.begin(); iter != campaign_flags.end(); iter++){
		// Okay, we have the campaign ID in a state such that reading it back in will restore the original ID.
		// Now output any flags that are set for this campaign.
		for(unsigned int i = 0; i < campaign_flag_type::x_max; i++) {
			for(unsigned int j = 0; j < campaign_flag_type::y_max; j++) {
				if(iter->second.idx[i][j] > 0) {
					main_page["CAMPAIGN"] << iter->first << i << j << iter->second.idx[i][j];
				}
			}
		}
	}
	for(int i = 0; i < boats.size(); i++){
		if(boats[i].exists) {
			auto& boat_page = file.add();
			boat_page["BOAT"] << i;
			boats[i].writeTo(boat_page);
		}
	}
	for(int i = 0; i < horses.size(); i++){
		if(horses[i].exists) {
			auto& horse_page = file.add();
			horse_page["HORSE"] << i;
			horses[i].writeTo(horse_page);
		}
	}
	for(auto& p : magic_store_items) {
		for(auto& p2 : p.second) {
			if(p2.second.variety != eItemType::NO_ITEM) {
				auto& store_page = file.add();
				store_page["MAGICSTORE"] << p.first << p2.first;
				p2.second.writeTo(store_page);
			}
		}
	}
	for(int i = 0; i < job_banks.size(); i++) {
		auto& job_page = file.add();
		job_page["JOBBANK"] << i << job_banks[i].anger;
		if(!job_banks[i].inited) continue;
		for(int j = 0; j < 6; j++){
			job_page["JOB"] << j << job_banks[i].jobs[j];
		}
	}
	for(int i = 0; i < out_c.size(); i++) {
		if(out_c[i].exists) {
			auto& enc_page = file.add();
			enc_page["ENCOUNTER"] << i;
			out_c[i].writeTo(enc_page);
		}
	}
	// TODO: Why is each timer on its own page, anyway? It clearly doesn't need to be!
	for(unsigned int i = 0; i < party_event_timers.size(); i++) {
		if(!party_event_timers[i].is_valid()) continue;
		auto& timer_page = file.add();
		timer_page["TIMER"] << i << party_event_timers[i].time << int(party_event_timers[i].node_type) << party_event_timers[i].node;
	}
	for(int i = 0; i < creature_save.size(); i++) {
		for(int j = 0; j < creature_save[i].size(); j++) {
			if(creature_save[i][j].is_alive()) {
				auto& creature_page = file.add();
				creature_page["CREATURE"] << i << j;
				creature_save[i][j].writeTo(creature_page);
			}
		}
	}
	for(const auto& p : stored_items) {
		for(size_t j = 0; j < p.second.size(); j++) {
			if(p.second[j].variety != eItemType::NO_ITEM) {
				auto& item_page = file.add();
				item_page["STORED"] << p.first << j;
				p.second[j].writeTo(item_page);
			}
		}
	}
	if(summons.size() > 0) {
		int i = 0;
		for(const cMonster& monst : summons) {
			auto& monst_page = file.add();
			monst_page["SUMMON"] << i++;
			monst.writeTo(monst_page);
			cTagFile_Page* abil_page = nullptr;
			for(const auto& abil_elem : monst.abil) {
				if(abil_page == nullptr) abil_page = &file.add();
				if(abil_elem.second.writeTo(abil_elem.first, *abil_page)) {
					abil_page = nullptr;
				}
			}
		}
	}
	if(journal.size() > 0) {
		for(const cJournal& entry : journal) {
			auto& journal_page = file.add();
			journal_page["JOURNAL"] << entry.day << entry.in_scen;
			journal_page["STRING"] << entry.the_str;
		}
	}
	if(special_notes.size() > 0) {
		for(const cEncNote& note : special_notes) {
			auto& note_page = file.add();
			note_page["ENCNOTE"] << note.type << note.where << note.in_scen;
			note_page["STRING"] << note.the_str;
		}
	}
	if(talk_save.size() > 0) {
		for(const cConvers& note : talk_save) {
			auto& note_page = file.add();
			note_page.add("TALKNOTE");
			note_page["WHO"] << note.who_said;
			note_page["WHERE"] << note.in_town << note.in_scen;
			note_page["STRING"] << note.the_str1;
			note_page["STRING"] << note.the_str2;
		}
	}
}

void cParty::readFrom(const cTagFile& file, bool preview) {
	size_t monst_i = 0;
	for(const auto& page : file) {
		if(page.index() == 0) {
			as_hex<unsigned long long> version{0};
			page["CREATEVERSION"] >> version;
			// This warning shouldn't interrupt the player when loading previews for the fancy file picker.
			if(version > OBOE_CURRENT_VERSION) {
				if(!preview){
					showWarning("This game appears to have been created with a newer version of Blades of Exile than you are running. Exile will do its best to load the saved game anyway, but there may be loss of information.");
				}else{
					// TODO It could put a silent warning icon on the slot, though.
				}
			}

			page["SCENARIO"] >> scen_name;
			page["OUTCORNER"] >> outdoor_corner.x >> outdoor_corner.y;
			page["INWHICHCORNER"] >> i_w_c.x >> i_w_c.y;
			page["SECTOR"] >> out_loc.x >> out_loc.y;
			page["LOCINSECTOR"] >> loc_in_sec.x >> loc_in_sec.y;
			if(preview) return;

			page["AGE"] >> age;
			page["GOLD"] >> gold;
			page["FOOD"] >> food;
			page["NEXTID"] >> next_pc_id;
			page["HOSTILES"] >> hostiles_present;
			page["EASY"] >> easy_mode;
			page["LESSWM"] >> less_wm;
			page["LIGHT"] >> light_level;
			page["IN"] >> in_boat >> in_horse;
			page["DIRECTION"] >> direction;
			page["WHICHSLOT"] >> at_which_save_slot;
			page["KILLS"] >> total_m_killed;
			page["DAMAGE"] >> total_dam_done;
			page["WOUNDS"] >> total_dam_taken;
			page["EXPERIENCE"] >> total_xp_gained;
			page["WON"] >> scen_won;
			page["PLAYED"] >> scen_played;
			
			page["STATUS"].extractSparse(status); // Does this do anything?
			page["EVENT"].extractSparse(key_times);
			
			if(page.contains("SPLIT_LEFT_IN")) {
				page["SPLIT_LEFT_IN"] >> left_in;
				page["SPLIT_LEFT_AT"] >> left_at.x >> left_at.y;
			} else {
				left_in = -1;
			}
			
			wipe_sdfs();
			for(size_t i = 0; i < page["SDF"].size(); i++) {
				size_t x = sdx_max, y = sdy_max, val = 0;
				page["SDF"] >> x >> y >> val;
				if(x <= sdx_max && y <= sdy_max) {
					stuff_done[x][y] = val;
				}
			}
			
			pointers.clear();
			magic_ptrs.fill(0);
			for(size_t n = 0; n < page["POINTER"].size(); n++) {
				int i = 0, j = 0, k = 0;
				auto tmp = page["POINTER"] >> i >> j;
				if(i >= 10 && i < 100) {
					magic_ptrs[i - 10] = j;
				} else if(i >= 100 && i < 200) {
					tmp >> k;
					pointers[i] = std::make_pair(j, k);
				}
			}
			
			m_noted.clear(); m_seen.clear();
			std::vector<decltype(m_noted)::value_type> roster;
			page["ROSTER"].extract(roster);
			std::copy(roster.begin(), roster.end(), std::inserter(m_noted, m_noted.begin()));
			page["SEEN"].extract(roster);
			std::copy(roster.begin(), roster.end(), std::inserter(m_seen, m_seen.begin()));
			
			imprisoned_monst.fill(0);
			for(size_t n = 0; n < page["SOULCRYSTAL"].size(); n++){
				size_t slot;
				mon_num_t monster;
				page["SOULCRYSTAL"] >> slot >> monster;
				imprisoned_monst[n] = monster;
			}
			
			alchemy.reset();
			size_t alch;
			while(page["ALCHEMY"] >> alch) {
				alchemy[alch] = true;
			}
			
			for(size_t n = 0; n < page["TOWNSAVE"].size(); n++) {
				size_t i;
				auto tmp = page["TOWNSAVE"] >> i;
				if(!tmp || i >= creature_save.size()) continue;
				std::string hostile;
				tmp >> creature_save[i].which_town >> hostile;
				creature_save[i].hostile = hostile == "HOSTILE";
			}
			
			spec_items.clear();
			std::vector<decltype(spec_items)::value_type> items;
			page["ITEM"].extract(items);
			std::copy(items.begin(), items.end(), std::inserter(spec_items, spec_items.begin()));
			
			active_quests.clear();
			std::map<int, std::tuple<eQuestStatus, int, int>> quests;
			page["QUEST"].extractSparse(quests);
			for(const auto& p : quests) {
				cJob job(std::get<1>(p.second), std::get<2>(p.second));
				job.status = std::get<0>(p.second);
				active_quests.emplace(p.first, job);
			}
			
			vector2d<int> shop_stock;
			page["SHOPSTOCK"].extractSparse<int>(shop_stock);
			for(size_t x = 0; x < shop_stock.width(); x++) {
				for(size_t y = 0; y < shop_stock.height(); y++) {
					store_limited_stock[x][y] = shop_stock[x][y];
				}
			}
			
			std::string cur;
			unsigned int i, j;
			int val;
			while(page["CAMPAIGN"] >> cur >> i >> j >> val) {
				if(i <= campaign_flag_type::x_max && j <= campaign_flag_type::y_max) {
					campaign_flags[cur].idx[i][j] = val;
				}
			}
		} else if(page.getFirstKey() == "BOAT") {
			size_t i;
			if(page["BOAT"] >> i) {
				if(i >= boats.size()) boats.resize(i + 1);
				boats[i].exists = true;
				boats[i].readFrom(page);
			}
		} else if(page.getFirstKey() == "HORSE") {
			size_t i;
			if(page["HORSE"] >> i) {
				if(i >= horses.size()) horses.resize(i + 1);
				horses[i].exists = true;
				horses[i].readFrom(page);
			}
		} else if(page.getFirstKey() == "MAGICSTORE") {
			size_t i, j;
			if(page["MAGICSTORE"] >> i >> j) {
				magic_store_items[i][j].readFrom(page);
			}
		} else if(page.getFirstKey() == "ENCOUNTER") {
			int i;
			if(page["ENCOUNTER"] >> i) {
				out_c[i].exists = true;
				out_c[i].readFrom(page);
			}
		} else if(page.getFirstKey() == "TIMER") {
			size_t i, j;
			cTimer timer;
			while(page["TIMER"] >> i >> timer.time >> j >> timer.node) {
				timer.node_type = eSpecCtxType(j);
				if(i >= party_event_timers.size()) party_event_timers.resize(i + 1);
				party_event_timers[i] = timer;
			}
		} else if(page.getFirstKey() == "CREATURE") {
			size_t i, j;
			if(page["CREATURE"] >> i >> j) {
				if(i < 0 || i >= creature_save.size()) continue;
				creature_save[i].init(j);
				creature_save[i][j].readFrom(page);
			}
		} else if(page.getFirstKey() == "STORED") {
			size_t i, j;
			if(page["STORED"] >> i >> j) {
				if(j >= stored_items[i].size()) {
					stored_items[i].resize(j + 1);
				}
				stored_items[i][j].readFrom(page);
			}
		} else if(page.getFirstKey() == "SUMMON") {
			page["SUMMON"] >> monst_i;
			if(monst_i >= summons.size()) {
				summons.resize(monst_i + 1);
			}
			summons[monst_i].readFrom(page);
		} else if(page.getFirstKey() == "ABIL") {
			if(monst_i >= summons.size()) continue;
			uAbility abil;
			eMonstAbil key = abil.readFrom(page);
			if(key != eMonstAbil::NO_ABIL) {
				summons[monst_i].abil[key] = abil;
			}
		} else if(page.getFirstKey() == "JOURNAL") {
			cJournal entry;
			page["JOURNAL"] >> entry.day >> entry.in_scen;
			page["STRING"] >> entry.the_str;
			journal.push_back(entry);
		} else if(page.getFirstKey() == "ENCNOTE") {
			cEncNote note;
			page["ENCNOTE"] >> note.type >> note.where >> note.in_scen;
			page["STRING"] >> note.the_str;
			special_notes.push_back(note);
		} else if(page.getFirstKey() == "TALKNOTE") {
			// TALKNOTE is an empty tag with no value that only serves to identify the page
			cConvers note;
			page["WHO"] >> note.who_said;
			page["WHERE"] >> note.in_town >> note.in_scen;
			page["STRING"] >> note.the_str1;
			page["STRING"] >> note.the_str2;
			talk_save.push_back(note);
		} else if(page.getFirstKey() == "JOBBANK") {
			size_t i;
			job_bank_t bank;
			if(page["JOBBANK"] >> i >> bank.anger) {
				if(i >= job_banks.size()) {
					job_banks.resize(i + 1);
				}
				std::vector<int> jobs;
				page["JOB"].extractSparse(jobs);
				bank.inited = !jobs.empty();
				std::copy_n(jobs.begin(), std::min(jobs.size(), bank.jobs.size()), bank.jobs.begin());
				job_banks[i] = bank;
			}
		}
	}
}

cPlayer& cParty::operator[](unsigned short n){
	if(n > 6) throw std::out_of_range("Attempt to access a player that doesn't exist.");
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

unsigned char cParty::get_ptr(unsigned short p) const {
	if(p < 10 || p >= 200)
		throw std::range_error("Attempted to access a nonexistent pointer (10..199)");
	if(p < 100)
		return magic_ptrs[p-10];
	auto iter = pointers.find(p);
	if(iter == pointers.end()) return 0;
	return stuff_done[iter->second.first][iter->second.second];
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

void cParty::wipe_sdfs() {
	for(auto& col : stuff_done) {
		col.fill(0);
	}
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

