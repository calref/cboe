/*
 *  creatlist.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef BOE_DATA_CREATLIST_H
#define BOE_DATA_CREATLIST_H

#include <iosfwd>
#include <memory>
#include "creature.hpp"

namespace legacy {
	struct creature_list_type;
	struct creature_data_type;
};

class cPopulation {
	std::vector<std::shared_ptr<cCreature> > dudes;
	static cCreature dummyDude;
public:
	class iterator;
	
	short which_town;
	bool hostile;
	
	void import_legacy(legacy::creature_list_type old);
	void init(size_t n);
	void assign(size_t n, const cTownperson& other, const cMonster& base, bool easy, int difficulty_adjust);
	void readFrom(std::istream& in, size_t n);
	size_t size() const {return dudes.size();}
	void clear() {dudes.clear();}
	cCreature& operator[](size_t n);
	const cCreature& operator[](size_t n) const;
	// remove the n^th creature from the list by swapping it with the last dude and then pop_back it
	void removeCreature(size_t n);
	cPopulation() : which_town(200), hostile(false) {}
	iterator begin() {return iterator(dudes.begin());}
	iterator end() {return iterator(dudes.end());}
	// Apparently Visual Studio needs this to work
	cPopulation& operator=(const cPopulation& other) = default;
	void swap(cPopulation& other);
	
	class iterator {
	public:
		explicit iterator(std::vector<std::shared_ptr<cCreature>>::iterator it_)
			: it(it_) {
		}
		iterator(const iterator&) = default;
		~iterator() = default;
		iterator& operator=(const iterator&) = default;
		iterator& operator++() { //postfix increment
			++it;
			return *this;
		}
		iterator operator++(int) { //prefix increment
			auto rIt=*this;
			it++;
			return rIt;
		}
		cCreature &operator*();
		bool operator==(const iterator& other) const { return it == other.it; }
		bool operator!=(const iterator& other) const { return !(*this == other); }

		std::vector<std::shared_ptr<cCreature>>::iterator it;
	};

};

#endif
