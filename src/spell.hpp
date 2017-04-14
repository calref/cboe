//
//  spell.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-12.
//
//

#ifndef BoE_DATA_SPELL_HPP
#define BoE_DATA_SPELL_HPP

#include <map>
#include "skills_traits.hpp"

enum eSpellRefer {REFER_YES, REFER_IMMED, REFER_TARGET, REFER_FANCY};
enum eSpellSelect {SELECT_NO, SELECT_ACTIVE, SELECT_ANY};
// This one is meant for indexing a bit field
enum eSpellWhen {WHEN_COMBAT = 1, WHEN_TOWN = 2, WHEN_OUTDOORS = 4};

enum eSpellPat {PAT_SINGLE, PAT_SQ, PAT_SMSQ, PAT_OPENSQ, PAT_RAD2, PAT_RAD3, PAT_PLUS, PAT_WALL};

enum class eSpell {
	NONE = -1,
	// Mage spells
	LIGHT = 0, SPARK = 1, HASTE_MINOR = 2, STRENGTH = 3, SCARE = 4,
	CLOUD_FLAME = 5, IDENTIFY = 6, SCRY_MONSTER = 7, GOO = 8, TRUE_SIGHT = 9,
	POISON_MINOR = 10, FLAME = 11, SLOW = 12, DUMBFOUND = 13, ENVENOM = 14,
	CLOUD_STINK = 15, SUMMON_BEAST = 16, CONFLAGRATION = 17, DISPEL_SQUARE = 18, CLOUD_SLEEP = 19,
	UNLOCK = 20, HASTE = 21, FIREBALL = 22, LIGHT_LONG = 23, FEAR = 24,
	WALL_FORCE = 25, SUMMON_WEAK = 26, ARROWS_FLAME = 27, WEB = 28, RESIST_MAGIC = 29,
	POISON = 30, ICE_BOLT = 31, SLOW_GROUP = 32, MAGIC_MAP = 33, CAPTURE_SOUL = 34,
	SIMULACRUM = 35, ARROWS_VENOM = 36, WALL_ICE = 37, STEALTH = 38, HASTE_MAJOR = 39,
	FIRESTORM = 40, DISPEL_BARRIER = 41, BARRIER_FIRE = 42, SUMMON = 43, SHOCKSTORM = 44,
	SPRAY_FIELDS = 45, POISON_MAJOR = 46, FEAR_GROUP = 47, KILL = 48, PARALYZE = 49,
	DEMON = 50, ANTIMAGIC = 51, MINDDUEL = 52, FLIGHT = 53, SHOCKWAVE = 54,
	BLESS_MAJOR = 55, PARALYSIS_MASS = 56, PROTECTION = 57, SUMMON_MAJOR = 58, BARRIER_FORCE = 59,
	QUICKFIRE = 60, ARROWS_DEATH = 61, /* Special spells */ STRENGTHEN_TARGET = 62, SUMMON_RAT = 63, WALL_ICE_BALL = 64,
	GOO_BOMB = 65, FOUL_VAPOR = 66, CLOUD_SLEEP_LARGE = 67, ACID_SPRAY = 68, PARALYZE_BEAM = 69,
	SLEEP_MASS = 70, RAVAGE_ENEMIES = 71, BLADE_AURA = 72, ICY_RAIN = 73, FLAME_AURA = 74,
	SUMMON_AID = 75, SUMMON_AID_MAJOR = 76, FLASH_STEP = 77,
	// Priest spells
	BLESS_MINOR = 100, HEAL_MINOR = 101, POISON_WEAKEN = 102, TURN_UNDEAD = 103, LOCATION = 104,
	SANCTUARY = 105, SYMBIOSIS = 106, MANNA_MINOR = 107, RITUAL_SANCTIFY = 108, STUMBLE = 109,
	BLESS = 110, POISON_CURE = 111, CURSE = 112, LIGHT_DIVINE = 113, WOUND = 114,
	SUMMON_SPIRIT = 115, MOVE_MOUNTAINS = 116, CHARM_FOE = 117, DISEASE = 118, AWAKEN = 119,
	HEAL = 120, HEAL_ALL_LIGHT = 121, HOLY_SCOURGE = 122, DETECT_LIFE = 123, PARALYSIS_CURE = 124,
	MANNA = 125, FORCEFIELD = 126, DISEASE_CURE = 127, RESTORE_MIND = 128, SMITE = 129,
	POISON_CURE_ALL = 130, CURSE_ALL = 131, DISPEL_UNDEAD = 132, CURSE_REMOVE = 133, STICKS_TO_SNAKES = 134,
	MARTYRS_SHIELD = 135, CLEANSE = 136, FIREWALK = 137, BLESS_PARTY = 138, HEAL_MAJOR = 139,
	RAISE_DEAD = 140, FLAMESTRIKE = 141, SANCTUARY_MASS = 142, SUMMON_HOST = 143, SHATTER = 144,
	DISPEL_SPHERE = 145, HEAL_ALL = 146, REVIVE = 147, HYPERACTIVITY = 148, DESTONE = 149,
	SUMMON_GUARDIAN = 150, CHARM_MASS = 151, PROTECTIVE_CIRCLE = 152, PESTILENCE = 153, REVIVE_ALL = 154,
	RAVAGE_SPIRIT = 155, RESURRECT = 156, DIVINE_THUD = 157, AVATAR = 158, WALL_BLADES = 159,
	WORD_RECALL = 160, CLEANSE_MAJOR = 161, /* Special spells */ DISPEL_FIELD = 162, MOVE_MOUNTAINS_MASS = 163, WRACK = 164,
	UNHOLY_RAVAGING = 165, AUGMENTATION = 166, NIRVANA = 167,
};

inline bool isMage(eSpell spell) {
	int code = (int) spell;
	return code >= 0 && code < 62;
}

inline bool isPriest(eSpell spell) {
	int code = (int) spell;
	return code >= 100 && code < 162;
}

class cSpell {
	static std::map<eSpell,cSpell> dictionary;
	friend const cSpell& operator*(eSpell spell_num);
public:
	cSpell() {} // This is just here because the map doesn't work without it
	cSpell(eSpell id);
	cSpell& withRefer(eSpellRefer r);
	cSpell& withCost(int c);
	cSpell& withRange(int r);
	cSpell& asLevel(int lvl);
	cSpell& asType(eSkill type);
	cSpell& asPeaceful();
	cSpell& needsSelect(eSpellSelect sel = SELECT_ACTIVE);
	cSpell& when(eSpellWhen when);
	const cSpell& finish();
	eSpell num;
	eSpellRefer refer;
	int cost, range, level;
	eSpellSelect need_select;
	eSkill type;
	int when_cast;
	bool peaceful = false;
	std::string name() const;
	bool is_priest() const;
	static eSpell fromNum(eSkill type, int num);
	static eSpell fromNum(int num);
};

// Need to declare this a second time in order for it to be in scope where it's needed
const cSpell& operator*(eSpell spell_num);

std::ostream& operator<< (std::ostream& out, eSpellPat pat);
std::istream& operator>> (std::istream& in, eSpellPat& pat);

#endif
