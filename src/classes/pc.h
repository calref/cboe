/*
 *  pc.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef BOE_DATA_PC_H
#define BOE_DATA_PC_H

#include <string>
#include <map>
#include <iosfwd>
#include <array>

#include "simpletypes.h"
#include "item.h"
#include "pictypes.hpp"
#include "living.hpp"

namespace legacy { struct pc_record_type; };

enum class eBuyStatus {OK, NO_SPACE, NEED_GOLD, TOO_HEAVY, HAVE_LOTS};

class cParty;

class cPlayer : public iLiving {
	cParty& party;
public:
	eMainStatus main_status;
	std::string name;
	// HACK: This is only really marked mutable so that I can use operator[] from const methods
	mutable std::map<eSkill, short> skills;
	unsigned short max_health;
	short cur_health;
	unsigned short max_sp;
	short cur_sp;
	unsigned short experience;
	short skill_pts;
	short level;
	std::array<cItem,24> items;
	std::array<bool,24> equip;
	bool priest_spells[62];
	bool mage_spells[62];
	pic_num_t which_graphic;
	short weap_poisoned;
	// HACK: This is only really marked mutable so that I can use operator[] from const methods
	mutable std::map<eTrait,bool> traits;
	eRace race;
	// transient stuff
	std::map<eSkill,eSpell> last_cast;
	location combat_pos;
	short parry;
	iLiving* last_attacked = nullptr; // Note: Currently this is assigned but never read
	
	bool is_alive() const;
	bool is_friendly() const;
	bool is_shielded() const;
	int get_shared_dmg(int base_dmg) const;
	
	int get_health() const;
	int get_magic() const;
	int get_level() const;
	location get_loc() const;
	
	void finish_create();
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
	
	void combine_things();
	void sort_items();
	bool give_item(cItem item, bool do_print, bool allow_overload = false);
	void take_item(int which_item);
	void remove_charge(int which_item);
	short has_space() const;
	short max_weight() const;
	short cur_weight() const;
	short free_weight() const;
	short get_prot_level(eItemAbil abil, short dat = -1) const;
	short has_abil_equip(eItemAbil abil, short dat = -1) const;
	short has_abil(eItemAbil abil, short dat = -1) const;
	short skill(eSkill skill) const;
	short stat_adj(eSkill skill) const;
	eBuyStatus ok_to_buy(short cost,cItem item) const;
	
	void append(legacy::pc_record_type old);
	cPlayer(cParty& party);
	cPlayer(cParty& party,long key,short slot);
	short get_tnl();
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
	virtual ~cPlayer() = default;
};

void operator += (eMainStatus& stat, eMainStatus othr);
void operator -= (eMainStatus& stat, eMainStatus othr);
std::ostream& operator << (std::ostream& out, eMainStatus e);
std::istream& operator >> (std::istream& in, eMainStatus& e);

#endif
