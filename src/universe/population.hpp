/*
 *  creatlist.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef BOE_DATA_CREATLIST_H
#define BOE_DATA_CREATLIST_H

#include "scenario/monster.hpp"
#include <deque>
#include <iosfwd>
#include <memory>
#include "creature.hpp"

namespace legacy {
	struct creature_list_type;
	struct creature_data_type;
};

class cPopulation {
	std::deque<cCreature> dudes;
public:
	class iterator;
	
	short which_town;
	bool hostile;
	
	void import_legacy(legacy::creature_list_type old);
	void init(size_t n);
	void assign(size_t n, const cTownperson& other, const cMonster& base, bool easy, int difficulty_adjust);
	size_t size() const {return dudes.size();}
	void clear() {dudes.clear();}
	cCreature& operator[](size_t n);
	const cCreature& operator[](size_t n) const;
	// remove the n^th creature from the list by swapping it with the last dude and then pop_back it
	void removeCreature(size_t n);
	cPopulation() : which_town(200), hostile(false) {}
	std::deque<cCreature>::iterator begin() {return dudes.begin();}
	std::deque<cCreature>::iterator end() {return dudes.end();}
	// Apparently Visual Studio needs this to work
	cPopulation& operator=(const cPopulation& other) = default;
	friend void swap(cPopulation& lhs, cPopulation& rhs);
};

#endif
