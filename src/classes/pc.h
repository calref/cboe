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

class cPlayer {
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
	short marked_damage, dir, parry, last_attacked;
	
	void finish_create();
	
	void append(legacy::pc_record_type old);
	cPlayer();
	cPlayer(long key,short slot);
	short get_tnl();
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
};

void operator += (eMainStatus& stat, eMainStatus othr);
void operator -= (eMainStatus& stat, eMainStatus othr);
std::ostream& operator << (std::ostream& out, eMainStatus e);
std::istream& operator >> (std::istream& in, eMainStatus& e);

#endif
