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
#include "simpletypes.h"
#include "graphtool.hpp"

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

enum class eMonstMelee {SWING, CLAW, BITE, SLIME, PUNCH, STING, CLUB, BURN, HARM, STAB};

enum class eMonstMissile {DART, ARROW, SPEAR, ROCK, RAZORDISK, SPINE, KNIFE, BOLT};

enum class eMonstGen {RAY, TOUCH, GAZE, BREATH, SPIT};

enum class eMonstSummon {TYPE, LEVEL, SPECIES};

// Directions!
enum eDirection {
	DIR_N = 0,
	DIR_NE = 1,
	DIR_E = 2,
	DIR_SE = 3,
	DIR_S = 4,
	DIR_SW = 5,
	DIR_W = 6,
	DIR_NW = 7,
	DIR_HERE = 8,
};

inline eDirection& operator++ (eDirection& me, int) {
	if(me == DIR_HERE) return me = DIR_N;
	else return me = (eDirection)(1 + (int)me);
}

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
	} radiate;
	struct {
		bool active;
		int extra1, extra2, extra3;
	} special;
	std::string to_string(eMonstAbil myKey) const;
};

class cMonster {
public:
	struct cAttack{
		unsigned short dice, sides;
		eMonstMelee type;
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
	std::map<eMonstAbil, uAbility> abil;
	item_num_t corpse_item;
	short corpse_item_chance;
	unsigned int magic_res;
	unsigned int fire_res;
	unsigned int cold_res;
	unsigned int poison_res;
	bool mindless : 1;
	bool invuln : 1;
	bool invisible : 1;
	bool guard : 1;
	char : 4;
	unsigned int x_width,y_width;
	unsigned int default_attitude;
	unsigned int summon_type;
	pic_num_t default_facial_pic;
	pic_num_t picture_num;
	snd_num_t ambient_sound; // has a chance of being played every move
	spec_num_t see_spec;
public:
	
	std::map<eMonstAbil,uAbility>::iterator addAbil(eMonstAbilTemplate what, int param = 0);
	void append(legacy::monster_record_type& old);
	cMonster();
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
};

enum class eMonstTime {
	ALWAYS,
	APPEAR_ON_DAY, DISAPPEAR_ON_DAY,
	SOMETIMES_C, SOMETIMES_A, SOMETIMES_B,
	APPEAR_WHEN_EVENT, DISAPPEAR_WHEN_EVENT,
	APPEAR_AFTER_CHOP,
};

class cCreature : public cMonster {
public:
	unsigned long id;
	mon_num_t number;
	short active, attitude;
	unsigned int start_attitude;
	location start_loc, cur_loc;
	unsigned short mobility;
	eMonstTime time_flag;
	short summoned;
	short spec1, spec2;
	char spec_enc_code, time_code;
	short monster_time, personality;
	short special_on_kill, facial_pic;
	short target;
	location targ_loc;
	short health;
	short mp;
	short max_mp;
	short ap;
	short morale,m_morale; // these are calculated in-game based on the level
	std::map<eStatus,short> status;
	eDirection direction;
	short marked_damage = 0; // for use during animations
	
	cCreature();
	cCreature(int num);
	
	void append(legacy::creature_data_type old);
	void append(legacy::creature_start_type old);
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
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
std::ostream& operator<<(std::ostream& out, const cMonster::cAttack& att);
#endif
