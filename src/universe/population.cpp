/*
 *  creatlist.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "population.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "oldstructs.hpp"

void cPopulation::import_legacy(legacy::creature_list_type old){
	dudes.resize(60);
	for(int i = 0; i < 60; i++)
		dudes[i].import_legacy(old.dudes[i]);
	which_town = old.which_town;
	hostile = old.hostile;
}

const cCreature& cPopulation::operator[](size_t n) const {
	return dudes[n];
}

cCreature& cPopulation::operator[](size_t n){
	return dudes[n];
}

void cPopulation::init(size_t n) {
	if(n >= dudes.size()) dudes.resize(n + 1);
	dudes[n].active = 1;
}

// This function combines a cTownperson from a scenario town record with a cMonster from the scenario record
// into a cCreature, and prepares it for use in-game according to the user's preferences and party strength
// replaces return_monster_template() from boe.monsters.cpp
void cPopulation::assign(size_t n, const cTownperson& other, const cMonster& base, bool easy, int difficulty_adjust){
	// Make sure the space exists
	if(n >= dudes.size()) dudes.resize(n + 1);
	// First copy over the superclass fields
	static_cast<cTownperson&>(dudes[n]) = other;
	static_cast<cMonster&>(dudes[n]) = base;
	// Now set up extra stuff
	dudes[n].active = 1; // TODO: Is this right?
	if(dudes[n].invisible) dudes[n].picture_num = 0;
	dudes[n].m_health /= easy ? 2 : 1;
	dudes[n].m_health *= difficulty_adjust;
	dudes[n].health = dudes[n].m_health;
	dudes[n].ap = 0;
	if(dudes[n].mu > 0 || dudes[n].cl > 0)
		dudes[n].max_mp = dudes[n].mp = 12 * dudes[n].level;
	else dudes[n].max_mp = dudes[n].mp = 0;
	dudes[n].m_morale = 10 * dudes[n].level;
	if(dudes[n].level > 20)
		dudes[n].m_morale += 10 * (dudes[n].level - 20);
	dudes[n].morale = dudes[n].m_morale;
	dudes[n].direction = DIR_HERE;
	dudes[n].status.clear();
	dudes[n].attitude = dudes[n].start_attitude;
	dudes[n].cur_loc = dudes[n].start_loc;
	dudes[n].target = 6; // No target
	dudes[n].summon_time = 0;
}

void cPopulation::swap(cPopulation& other) {
	std::swap(dudes, other.dudes);
	std::swap(which_town, other.which_town);
	std::swap(hostile, other.hostile);
}

void cPopulation::readFrom(std::istream& in, size_t n) {
	if(n >= dudes.size()) dudes.resize(n + 1);
	dudes[n].readFrom(in);
}
