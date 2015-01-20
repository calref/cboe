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

namespace legacy { struct pc_record_type; };

enum class eBuyStatus {OK, NO_SPACE, NEED_GOLD, TOO_HEAVY, HAVE_LOTS};

class cParty;

class cPlayer {
	cParty& party;
public:
	eMainStatus main_status;
	std::string name;
	std::map<eSkill, short> skills;
	unsigned short max_health;
	short cur_health;
	unsigned short max_sp;
	short cur_sp;
	unsigned short experience;
	short skill_pts;
	short level;
	std::map<eStatus,short> status;
	std::array<cItem,24> items;
	std::array<bool,24> equip;
	bool priest_spells[62];
	bool mage_spells[62];
	pic_num_t which_graphic;
	short weap_poisoned;
	std::map<eTrait,bool> traits;
	eRace race;
	short direction;
	short ap;
	// transient stuff
	std::map<eSkill,eSpell> last_cast;
	location combat_pos;
	short marked_damage = 0, dir, parry, last_attacked;
	
	void finish_create();
	void apply_status(eStatus which, int how_much);
	void avatar();
	
	void combine_things();
	void sort_items();
	bool give_item(cItem item, bool do_print, bool allow_overload = false);
	void take_item(int which_item);
	void remove_charge(int which_item);
	short has_space();
	short max_weight();
	short cur_weight();
	short free_weight();
	short get_prot_level(eItemAbil abil, short dat = -1);
	short has_abil_equip(eItemAbil abil, short dat = -1);
	short has_abil(eItemAbil abil, short dat = -1);
	short skill(eSkill skill);
	eBuyStatus ok_to_buy(short cost,cItem item);
	
	void append(legacy::pc_record_type old);
	cPlayer(cParty& party);
	cPlayer(cParty& party,long key,short slot);
	static void(* print_result)(std::string);
	short get_tnl();
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
};

void operator += (eMainStatus& stat, eMainStatus othr);
void operator -= (eMainStatus& stat, eMainStatus othr);
std::ostream& operator << (std::ostream& out, eMainStatus e);
std::istream& operator >> (std::istream& in, eMainStatus& e);

#endif
