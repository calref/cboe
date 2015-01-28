//
//  creature.hpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#ifndef BoE_creature_hpp
#define BoE_creature_hpp

#include <iosfwd>
#include "location.h"
#include "monster.h"
#include "living.hpp"
#include "simpletypes.h"

class cCreature : public cMonster, public cTownperson, public iLiving {
public:
	static const short charm_odds[20];
	short active, attitude;
	location cur_loc;
	short summon_time;
	bool party_summoned;
	short target;
	location targ_loc;
	short health;
	short mp;
	short max_mp;
	short morale,m_morale; // these are calculated in-game based on the level
	
	cCreature();
	cCreature(int num);
	
	void heal(int how_much);
	void poison(int how_much);
	void cure(int how_much);
	void acid(int how_much);
	void curse(int how_much);
	void slow(int how_much);
	void web(int how_much);
	void disease(int how_much);
	void dumbfound(int how_much);
	void scare(int how_much);
	void sleep(eStatus type, int how_much, int adj);
	void avatar();
	void drain_sp(int how_much);
	void restore_sp(int how_much);
	void petrify(int adj);
	void kill(eMainStatus type = eMainStatus::DEAD);
	
	int get_health() const;
	int get_magic() const;
	int get_level() const;
	
	bool is_alive() const;
	bool is_friendly() const;
	bool is_shielded() const;
	int get_shared_dmg(int base_dmg) const;
	location get_loc() const;
	
	int magic_adjust(int base);
	
	void spell_note(int which);
	void cast_spell_note(eSpell spell);
	void print_attacks(iLiving* target);
	void breathe_note();
	void damaged_msg(int how_much, int extra);
	void killed_msg();
	bool on_space(location loc) const;
	
	void append(legacy::creature_data_type old);
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
};

#endif
