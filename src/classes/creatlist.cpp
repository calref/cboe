/*
 *  creatlist.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "classes.h"
#include "oldstructs.h"

void cPopulation::append(legacy::creature_list_type old){
	for(int i = 0; i < 60; i++)
		dudes[i].append(old.dudes[i]);
	which_town = old.which_town;
	friendly = old.friendly;
}

const cCreature& cPopulation::operator[](size_t n) const {
	return dudes[n];
}

cCreature& cPopulation::operator[](size_t n){
	return dudes[n];
}

// This function takes a preset cCreature from a scenario town record and prepares it for use in-game.
// It's needed because in the town record, many fields are ignored, including all the superclass fields.
// replaces return_monster_template() from boe.monsters.cpp
void cPopulation::assign(size_t n, const cCreature& other, const cMonster& base, bool easy, int difficulty_adjust){
 	// First do a basic assign
	dudes[n] = other;
	// And make sure the superclass fields are correctly populated
	static_cast<cMonster&>(dudes[n]) = base;
	// Now set up extra stuff
	dudes[n].active = 1; // TODO: Is this right?
	if(dudes[n].spec_skill == 11) dudes[n].picture_num = 0;
	dudes[n].m_health /= easy ? 2 : 1;
	dudes[n].m_health *= difficulty_adjust;
	dudes[n].health = dudes[n].m_health;
	dudes[n].ap = 0;
	if(dudes[n].mu > 0 || dudes[n].cl > 0)
		dudes[n].max_mp = dudes[n].mp = 12 * dudes[n].level;
	else dudes[n].max_mp = dudes[n].mp = 0;
	dudes[n].m_morale = 10 * dudes[n].level;
	if(dudes[n].level >= 20)
		dudes[n].m_morale += 10 * (dudes[n].level - 20);
	dudes[n].morale = dudes[n].m_morale;
	dudes[n].direction = DIR_HERE;
	dudes[n].status.clear();
	dudes[n].attitude = dudes[n].start_attitude; // TODO: Is this right?
	dudes[n].cur_loc = dudes[n].start_loc;
	dudes[n].target = 6; // No target
	dudes[n].summoned = 0;
}
