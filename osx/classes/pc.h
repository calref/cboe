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
#include <iosfwd>

#include "simpletypes.h"
#include "item.h"

namespace legacy { struct pc_record_type; };

typedef unsigned short pic_num_t; // TODO: This is now defined in multiple places...

class cPlayer {
public:
	eMainStatus main_status;
	std::string name;
	short skills[30];
	unsigned short max_health;
	short cur_health;
	unsigned short max_sp;
	short cur_sp;
	unsigned short experience;
	short skill_pts;
	short level;
	short status[15];
	cItemRec items[24];
	bool equip[24];
	bool priest_spells[62];
	bool mage_spells[62];
	pic_num_t which_graphic;
	short weap_poisoned;
	//bool advan[15];
	bool traits[15];
	eRace race;
	//short exp_adj;
	short direction;
	short ap;
	
	cPlayer& operator = (legacy::pc_record_type old);
	cPlayer();
	cPlayer(long key,short slot);
	short get_tnl();
	void writeTo(std::ostream& file);
	void readFrom(std::istream& file);
};

void operator += (eMainStatus& stat, eMainStatus othr);
void operator -= (eMainStatus& stat, eMainStatus othr);
std::ostream& operator << (std::ostream& out, eMainStatus& e);
std::istream& operator >> (std::istream& in, eMainStatus& e);

#endif