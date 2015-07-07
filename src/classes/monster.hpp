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
#include <iosfwd>

#include "soundtool.hpp"
#include "simpletypes.hpp"
#include "graphtool.hpp"
#include "living.hpp"
#include "spell.hpp"

namespace legacy {
	struct monster_record_type;
	struct creature_data_type;
	struct creature_start_type;
};

class cScenario;
class cUniverse;

enum class eMonstAbilTemplate {
	// Non-magical missiles
	THROWS_DARTS, SHOOTS_ARROWS, THROWS_SPEARS, THROWS_ROCKS1, THROWS_ROCKS2, THROWS_ROCKS3,
	THROWS_RAZORDISKS, THROWS_KNIVES, GOOD_ARCHER, SHOOTS_SPINES, CROSSBOWMAN, SLINGER,
	// Magical missiles
	RAY_PETRIFY, RAY_SP_DRAIN, RAY_HEAT, RAY_PARALYSIS,
	BREATH_FIRE, BREATH_FROST, BREATH_ELECTRICITY, BREATH_DARKNESS, BREATH_FOUL, BREATH_SLEEP,
	SPIT_ACID, SHOOTS_WEB,
	// Touch abilities
	TOUCH_POISON, TOUCH_ACID, TOUCH_DISEASE, TOUCH_WEB, TOUCH_SLEEP, TOUCH_DUMB, TOUCH_PARALYSIS,
	TOUCH_PETRIFY, TOUCH_DEATH, TOUCH_XP_DRAIN, TOUCH_ICY, TOUCH_ICY_DRAINING, TOUCH_STUN, TOUCH_STEAL_FOOD, TOUCH_STEAL_GOLD,
	// Misc abilities
	SPLITS, MARTYRS_SHIELD, ABSORB_SPELLS, SUMMON_5, SUMMON_20, SUMMON_50, SPECIAL, HIT_TRIGGERS, DEATH_TRIGGERS,
	// Radiate abilities
	RADIATE_FIRE, RADIATE_ICE, RADIATE_SHOCK, RADIATE_ANTIMAGIC, RADIATE_SLEEP, RADIATE_STINK, RADIATE_BLADE, RADIATE_WEB,
	// Advanced abilities
	CUSTOM_MISSILE, CUSTOM_DAMAGE, CUSTOM_STATUS, CUSTOM_FIELD, CUSTOM_PETRIFY, CUSTOM_SP_DRAIN, CUSTOM_XP_DRAIN,
	CUSTOM_KILL, CUSTOM_STEAL_FOOD, CUSTOM_STEAL_GOLD, CUSTOM_STUN, CUSTOM_STATUS2, CUSTOM_RADIATE, CUSTOM_SUMMON,
	CUSTOM_DAMAGE2,
};

union uAbility {
	bool active;
	struct {
		bool active;
		eMonstMissile type;
		miss_num_t pic;
		int dice, sides, skill, range, odds;
	} missile;
	struct {
		bool active;
		eMonstGen type;
		miss_num_t pic;
		int strength, range, odds;
		union {
			eDamageType dmg;
			eStatus stat;
			eFieldType fld;
		};
	} gen;
	struct {
		bool active;
		eMonstSummon type;
		mon_num_t what;
		int min, max, len, chance;
	} summon;
	struct {
		bool active;
		eFieldType type;
		int chance;
		eSpellPat pat;
	} radiate;
	struct {
		bool active;
		int extra1, extra2, extra3;
	} special;
	std::string to_string(eMonstAbil myKey) const;
	int get_ap_cost(eMonstAbil key) const;
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
	cAttack a[3];
	eRace m_type;
	unsigned int speed;
	unsigned int mu;
	unsigned int cl;
	unsigned int treasure;
	// HACK: This is only really marked mutable so that I can use operator[] from const methods
	mutable std::map<eMonstAbil, uAbility> abil;
	item_num_t corpse_item;
	short corpse_item_chance;
	unsigned int magic_res;
	unsigned int fire_res;
	unsigned int cold_res;
	unsigned int poison_res;
	bool mindless, invuln, invisible, guard;
	unsigned int x_width,y_width;
	eAttitude default_attitude;
	unsigned int summon_type;
	pic_num_t default_facial_pic;
	pic_num_t picture_num;
	snd_num_t ambient_sound; // has a chance of being played every move
	spec_num_t see_spec;
	
	std::map<eMonstAbil,uAbility>::iterator addAbil(eMonstAbilTemplate what, int param = 0);
	
	void append(legacy::monster_record_type& old);
	cMonster();
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
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
	
	void append(legacy::creature_start_type old);
	cTownperson();
	cTownperson(location loc, mon_num_t num, const cMonster& monst);
};

std::ostream& operator << (std::ostream& out, eStatus e);
std::istream& operator >> (std::istream& in, eStatus& e);
std::ostream& operator << (std::ostream& out, eRace e);
std::istream& operator >> (std::istream& in, eRace& e);
std::ostream& operator << (std::ostream& out, eMonstAbil e);
std::istream& operator >> (std::istream& in, eMonstAbil& e);
std::ostream& operator << (std::ostream& out, eMonstMissile e);
std::istream& operator >> (std::istream& in, eMonstMissile& e);
std::ostream& operator << (std::ostream& out, eMonstSummon e);
std::istream& operator >> (std::istream& in, eMonstSummon& e);
std::ostream& operator << (std::ostream& out, eMonstMelee e);
std::istream& operator >> (std::istream& in, eMonstMelee& e);
std::ostream& operator << (std::ostream& out, eMonstGen e);
std::istream& operator >> (std::istream& in, eMonstGen& e);
std::ostream& operator << (std::ostream& out, eDirection e);
std::istream& operator >> (std::istream& in, eDirection& e);
std::ostream& operator << (std::ostream& out, eDamageType e);
std::istream& operator >> (std::istream& in, eDamageType& e);
std::ostream& operator << (std::ostream& out, eFieldType e);
std::istream& operator >> (std::istream& in, eFieldType& e);
std::ostream& operator << (std::ostream& out, eMonstTime e);
std::istream& operator >> (std::istream& in, eMonstTime& e);
std::ostream& operator<< (std::ostream& out, eAttitude node);
std::istream& operator>> (std::istream& in, eAttitude& node);
std::ostream& operator<<(std::ostream& out, const cMonster::cAttack& att);
#endif
