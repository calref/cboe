//
//  living.hpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#ifndef BoE_living_hpp
#define BoE_living_hpp

#include <map>
#include <string>

#include "location.hpp"
#include "damage.hpp"

class iLiving {
public:
	// HACK: This is only really marked mutable so that I can use operator[] from const methods
	mutable std::map<eStatus,short> status;
	short ap = 0;
	eDirection direction = DIR_HERE;
	short marked_damage = 0; // for use during animations
	
	virtual bool is_alive() const = 0;
	virtual bool is_friendly() const = 0; // Return true if friendly to the party.
	virtual bool is_friendly(const iLiving& other) const = 0; // Return true if friendly to living entity
	virtual bool is_shielded() const = 0; // Checks for martyr's shield in any form - status, monster abil, item abil
	virtual int get_shared_dmg(int base_dmg) const = 0; // And this goes with the above.
	
	virtual void apply_status(eStatus which, int how_much);
	virtual void clear_brief_status();
	virtual void clear_bad_status();
	virtual void void_sanctuary();
	
	virtual void heal(int how_much) = 0;
	virtual void poison(int how_much) = 0;
	virtual void cure(int how_much) = 0;
	virtual void acid(int how_much) = 0;
	virtual void curse(int how_much) = 0;
	virtual void slow(int how_much) = 0;
	virtual void web(int how_much) = 0;
	virtual void disease(int how_much) = 0;
	virtual void dumbfound(int how_much) = 0;
	virtual void scare(int how_much) = 0;
	virtual void sleep(eStatus type, int how_much, int adj) = 0; // Also handles paralysis, charm, and forcecage
	virtual void avatar() = 0;
	virtual void drain_sp(int how_much) = 0;
	virtual void restore_sp(int how_much) = 0;
	
	virtual int get_health() const = 0;
	virtual int get_magic() const = 0;
	virtual int get_level() const = 0;
	virtual location get_loc() const = 0;
	
	virtual ~iLiving() = default;
	
	static void(* print_result)(std::string);
};

#endif
