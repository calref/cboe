//
//  monster_abilities.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_DATA_MONSTER_ABILITIES_HPP
#define BoE_DATA_MONSTER_ABILITIES_HPP

#include "damage.hpp"
#include "fields.hpp"
#include "spell.hpp"

enum class eMonstAbil {
	NO_ABIL,
	MISSILE,
	
	DAMAGE, STATUS, FIELD, PETRIFY, DRAIN_SP,
	DRAIN_XP, KILL, STEAL_FOOD, STEAL_GOLD, STUN,
	DAMAGE2, STATUS2,
	
	SPLITS, MARTYRS_SHIELD, ABSORB_SPELLS, MISSILE_WEB,
	RAY_HEAT, SPECIAL, HIT_TRIGGER, DEATH_TRIGGER,
	
	RADIATE, SUMMON,
};

enum class eMonstMelee {SWING, CLAW, BITE, SLIME, PUNCH, STING, CLUB, BURN, HARM, STAB};

enum class eMonstMissile {DART, ARROW, SPEAR, ROCK, RAZORDISK, SPINE, KNIFE, BOLT, BOULDER, RAPID_ARROW};

enum class eMonstGen {RAY, TOUCH, GAZE, BREATH, SPIT};

enum class eMonstSummon {TYPE, LEVEL, SPECIES};

enum class eMonstAbilCat {
	INVALID, MISSILE, GENERAL, SUMMON, RADIATE, SPECIAL
};

inline eMonstAbilCat getMonstAbilCategory(eMonstAbil what) {
	if(what == eMonstAbil::NO_ABIL)
		return eMonstAbilCat::SPECIAL;
	if(what == eMonstAbil::MISSILE)
		return eMonstAbilCat::MISSILE;
	if(what >= eMonstAbil::DAMAGE && what <= eMonstAbil::STATUS2)
		return eMonstAbilCat::GENERAL;
	if(what >= eMonstAbil::SPLITS && what <= eMonstAbil::DEATH_TRIGGER)
		return eMonstAbilCat::SPECIAL;
	if(what == eMonstAbil::RADIATE)
		return eMonstAbilCat::RADIATE;
	if(what == eMonstAbil::SUMMON)
		return eMonstAbilCat::SUMMON;
	return eMonstAbilCat::INVALID;
}

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

#endif
