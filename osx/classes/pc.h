/*
 *  pc.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#ifndef PC_H
#define PC_H

#include <string>
#include <iosfwd>

namespace legacy { struct pc_record_type; };

/* adven[i].skills */ //complete
enum eSkill {
	SKILL_STRENGTH = 0,
	SKILL_DEXTERITY = 1,
	SKILL_INTELLIGENCE = 2,
	SKILL_EDGED_WEAPONS = 3,
	SKILL_BASHING_WEAPONS = 4,
	SKILL_POLE_WEAPONS = 5,
	SKILL_THROWN_MISSILES = 6,
	SKILL_ARCHERY = 7,
	SKILL_DEFENSE = 8,
	SKILL_MAGE_SPELLS = 9,
	SKILL_PRIEST_SPELLS = 10,
	SKILL_MAGE_LORE = 11,
	SKILL_ALCHEMY = 12,
	SKILL_ITEM_LORE = 13,
	SKILL_DISARM_TRAPS = 14,
	SKILL_LOCKPICKING = 15,
	SKILL_ASSASSINATION = 16,
	SKILL_POISON = 17,
	SKILL_LUCK = 18,
};

/* adven[i].traits */ //complete
enum eTrait {
	TRAIT_TOUGHNESS = 0,
	TRAIT_MAGICALLY_APT = 1,
	TRAIT_AMBIDEXTROUS = 2,
	TRAIT_NIMBLE = 3,
	TRAIT_CAVE_LORE = 4,
	TRAIT_WOODSMAN = 5,
	TRAIT_GOOD_CONST = 6,
	TRAIT_HIGHLY_ALERT = 7,
	TRAIT_STRENGTH = 8,
	TRAIT_RECUPERATION = 9,
	TRAIT_SLUGGISH = 10,
	TRAIT_MAGICALLY_INEPT = 11,
	TRAIT_FRAIL = 12,
	TRAIT_CHRONIC_DISEASE = 13,
	TRAIT_BAD_BACK = 14,
	TRAIT_PACIFIST = 15,
};

enum eMainStatus {
	MAIN_STATUS_ABSENT = 0, // absent, empty slot
	MAIN_STATUS_ALIVE = 1,
	MAIN_STATUS_DEAD = 2,
	MAIN_STATUS_DUST = 3,
	MAIN_STATUS_STONE = 4,
	MAIN_STATUS_FLED = 5,
	MAIN_STATUS_SURFACE = 6, // fled to surface?
	MAIN_STATUS_WON = 7,
	MAIN_STATUS_SPLIT = 10,
	// The rest are not really necessary, but are here for completeness so that all valid values have a name.
	MAIN_STATUS_SPLIT_ABSENT = MAIN_STATUS_SPLIT + MAIN_STATUS_ABSENT,
	MAIN_STATUS_SPLIT_ALIVE = MAIN_STATUS_SPLIT + MAIN_STATUS_ALIVE,
	MAIN_STATUS_SPLIT_DEAD = MAIN_STATUS_SPLIT + MAIN_STATUS_DEAD,
	MAIN_STATUS_SPLIT_DUST = MAIN_STATUS_SPLIT + MAIN_STATUS_DUST,
	MAIN_STATUS_SPLIT_STONE = MAIN_STATUS_SPLIT + MAIN_STATUS_STONE,
	MAIN_STATUS_SPLIT_FLED = MAIN_STATUS_SPLIT + MAIN_STATUS_FLED,
	MAIN_STATUS_SPLIT_SURFACE = MAIN_STATUS_SPLIT + MAIN_STATUS_SURFACE,
	MAIN_STATUS_SPLIT_WON = MAIN_STATUS_SPLIT + MAIN_STATUS_WON,
};

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
	short which_graphic;
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