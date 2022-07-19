/*
 *  monster.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef BOE_DATA_MONSTER_H
#define BOE_DATA_MONSTER_H

#include <string>
#include <map>
#include <array>
#include <iosfwd>

#include "monster_abilities.hpp"
#include "race.hpp"
#include "location.hpp"

namespace legacy {
	struct monster_record_type;
	struct creature_data_type;
	struct creature_start_type;
};

class cScenario;
class cUniverse;
class cTagFile_Page;

enum class eAttitude {
	DOCILE, HOSTILE_A, FRIENDLY, HOSTILE_B
};

enum class eMonstTime {
	ALWAYS,
	APPEAR_ON_DAY, DISAPPEAR_ON_DAY,
	SOMETIMES_C, SOMETIMES_A, SOMETIMES_B,
	APPEAR_WHEN_EVENT, DISAPPEAR_WHEN_EVENT,
	APPEAR_AFTER_CHOP,
};

class cMonster {
public:
	struct cAttack{
		unsigned short dice = 0, sides = 0;
		eMonstMelee type = eMonstMelee::SWING;
	};
	unsigned int level;
	std::string m_name;
	short m_health;
	unsigned int armor;
	unsigned int skill;
	std::array<cAttack, 3> a;
	eRace m_type;
	unsigned int speed;
	unsigned int mu;
	unsigned int cl;
	unsigned int treasure;
	// HACK: This is only really marked mutable so that I can use operator[] from const methods
	mutable std::map<eMonstAbil, uAbility> abil;
	item_num_t corpse_item;
	short corpse_item_chance;
	std::map<eDamageType, int> resist;
	bool mindless, invuln, invisible, guard, amorphous;
	unsigned int x_width,y_width;
	eAttitude default_attitude;
	unsigned int summon_type;
	pic_num_t default_facial_pic;
	pic_num_t picture_num;
	snd_num_t ambient_sound; // has a chance of being played every move
	spec_num_t see_spec;
	
	std::map<eMonstAbil,uAbility>::iterator addAbil(eMonstAbilTemplate what, int param = 0);
	int addAttack(unsigned short dice, unsigned short sides, eMonstMelee type = eMonstMelee::SWING);
	
	void import_legacy(legacy::monster_record_type const &old);
	cMonster();
	void writeTo(cTagFile_Page& page) const;
	void readFrom(const cTagFile_Page& page);
	
	static cMonster bad() {
		cMonster monster;
		monster.m_name="BAD Monster";
		return monster;
	}
};

class cTownperson {
public:
	mon_num_t number;
	eAttitude start_attitude;
	location start_loc;
	unsigned short mobility;
	eMonstTime time_flag;
	short spec1, spec2;
	short spec_enc_code, time_code;
	short monster_time, personality;
	short special_on_kill, special_on_talk;
	pic_num_t facial_pic;
	
	void import_legacy(legacy::creature_start_type const &old);
	cTownperson();
	cTownperson(location loc, mon_num_t num, const cMonster& monst);
};

std::ostream& operator << (std::ostream& out, eMonstTime e);
std::istream& operator >> (std::istream& in, eMonstTime& e);
std::ostream& operator<< (std::ostream& out, eAttitude node);
std::istream& operator>> (std::istream& in, eAttitude& node);
std::ostream& operator<<(std::ostream& out, const cMonster::cAttack& att);

#endif
