/*
 *  simpletypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 01/06/09.
 *
 */

#ifndef BOE_DATA_SIMPLETYPES_H
#define BOE_DATA_SIMPLETYPES_H

typedef unsigned short m_num_t;
typedef unsigned short ter_num_t;
typedef signed short spec_num_t;
typedef signed short item_num_t;
typedef unsigned short str_num_t;

enum class eMainStatus {
	ABSENT = 0, // absent, empty slot
	ALIVE = 1,
	DEAD = 2,
	DUST = 3,
	STONE = 4,
	FLED = 5,
	SURFACE = 6, // fled to surface?
	WON = 7,
	SPLIT = 10,
	SPLIT_ABSENT = SPLIT + ABSENT,
	SPLIT_ALIVE = SPLIT + ALIVE,
	SPLIT_DEAD = SPLIT + DEAD,
	SPLIT_DUST = SPLIT + DUST,
	SPLIT_STONE = SPLIT + STONE,
	SPLIT_FLED = SPLIT + FLED,
	SPLIT_SURFACE = SPLIT + SURFACE,
	SPLIT_WON = SPLIT + WON,
};

inline eMainStatus exceptSplit(eMainStatus stat) {
	if(int(stat) >= 10)
		return (eMainStatus) (-10 + (int)stat);
	return stat;
}

inline bool isSplit(eMainStatus stat) {
	return int(stat) >= 10;
}

inline bool isAbsent(eMainStatus stat) {
	return stat == eMainStatus::ABSENT || int(stat) > 4;
}

inline bool isDead(eMainStatus stat) {
	int code = (int) stat;
	return code > 1 && code < 5;
}

/* adven[i].race */ //complete
enum class eRace {
	UNKNOWN = -1, // for parameters to some functions; not valid in the class
	HUMAN = 0,
	NEPHIL = 1,
	SLITH = 2,
	VAHNATAI = 3,	// Former value from eMonsterType
	REPTILE = 4,	// 1
	BEAST = 5,		// 2
	IMPORTANT = 6,	// 3
	MAGE = 7,		// 4
	PRIEST = 8,		// 5
	HUMANOID = 9,	// 6
	DEMON = 10,		// 7
	UNDEAD = 11,	// 8
	GIANT = 12,		// 9
	SLIME = 13,		// 10
	STONE = 14,		// 11
	BUG = 15,		// 12
	DRAGON = 16,	// 13
	MAGICAL = 17,	// 14
	PLANT = 18,
	BIRD = 19,
};

// Types IMPORTANT, MAGE, and PRIEST are implicitly human
// Types NEPHIL, SLITH, and VAHNATAI are implicitly humanoid
inline bool isHumanoid(eRace race) {
	int code = (int) race;
	return (code >= 0 && code <= 3) || (code >= 6 && code <= 9);
}

/* adven[i].status*/ //complete - assign a positive value for a help pc effect, a negative for harm pc
enum class eStatus {
	MAIN = -1, // For saved games only
	POISONED_WEAPON = 0,
	BLESS_CURSE = 1,
	POISON = 2,
	HASTE_SLOW = 3,
	INVULNERABLE = 4,
	MAGIC_RESISTANCE = 5,
	WEBS = 6,
	DISEASE = 7,
	INVISIBLE = 8, //sanctuary
	DUMB = 9,
	MARTYRS_SHIELD = 10,
	ASLEEP = 11,
	PARALYZED = 12,
	ACID = 13,
	FORCECAGE = 14, // This is new and currently unused, since forcecages aren't even really implemented yet.
	// This one is new
	// It's not quite a real status effect since it doesn't expire
	// We use 15 because 14 was technically a "reserved/unused" status, though it was never used for anything
	CHARM = 15,
};

/* Special Ability a.k.a spec_skill */

enum eMonstAbil {
	MONST_NO_ABIL = 0,
	// Missile abilities (extra1 = number of sided dice; extra2 = number of sides)
	MONST_THROWS_DARTS = 10,
	MONST_SHOOTS_ARROWS,
	MONST_THROWS_SPEARS,
	MONST_THROWS_ROCKS,
	MONST_THROWS_RAZORDISKS,
	MONST_GOOD_ARCHER,
	MONST_SHOOTS_SPINES,
	MONST_THROWS_KNIVES,
	// Ray abilities (extra1 = type of damage / status where applicable)
	MONST_DAMAGE_RAY = 20,
	MONST_STATUS_RAY,
	MONST_PETRIFY_RAY,
	MONST_DRAIN_SP_RAY,
	MONST_DRAIN_XP_RAY,
	MONST_DRAIN_XP_DAMAGE_RAY,
	MONST_KILL_RAY,
	MONST_STEAL_FOOD_RAY,
	MONST_STEAL_GOLD_RAY,
	// Touch abilities (extra1 = type of damage / status where applicable)
	MONST_DAMAGE_TOUCH = 30,
	MONST_STATUS_TOUCH,
	MONST_PETRIFY_TOUCH,
	MONST_DRAIN_SP_TOUCH,
	MONST_DRAIN_XP_TOUCH,
	MONST_DRAIN_XP_DAMAGE_TOUCH,
	MONST_KILL_TOUCH,
	MONST_STEAL_FOOD_TOUCH,
	MONST_STEAL_GOLD_TOUCH,
	// Summon abilities (extra1 = which monster / type / species; extra2 = % chance)
	MONST_SUMMON_ONE = 40,
	MONST_SUMMON_TYPE,
	MONST_SUMMON_SPECIES,
	MONST_SUMMON_RANDOM,
	MONST_MASS_SUMMON,
	// Misc abilities (extra1 = field / special #; extra2 = % chance for radiate only)
	MONST_SPLITS = 50,
	MONST_FIELD_MISSILE,
	MONST_MARTYRS_SHIELD,
	MONST_ABSORB_SPELLS,
	MONST_INVULNERABLE,
	MONST_RADIATE,
	MONST_CALL_LOCAL_SPECIAL,
	MONST_CALL_GLOBAL_SPECIAL,
};


/* Terrains Special Properties : scenario.ter_types[i].special */      //complete

enum class eTerSpec {
	NONE = 0,
	CHANGE_WHEN_STEP_ON = 1,
	DAMAGING = 2, // formerly "fire damage"
	BRIDGE = 3, // new; formerly "cold damage"
	BED = 4, // new; formerly "magic damage"
	DANGEROUS = 5, // formerly "poison land"
	UNUSED1 = 6, // formerly "disease land"
	CRUMBLING = 7,
	LOCKABLE = 8,
	UNLOCKABLE = 9,
	UNUSED2 = 10, // formerly "unlockable + bashable"
	IS_A_SIGN = 11,
	CALL_SPECIAL = 12, // formerly "call local special"
	UNUSED3 = 13, // formerly "call scenario special"
	IS_A_CONTAINER = 14,
	WATERFALL = 15,
	CONVEYOR = 16, // formerly "conveyor north"
	UNUSED4 = 17, // formerly "conveyor east"
	UNUSED5 = 18, // formerly "conveyor south"
	UNUSED6 = 19, // formerly "conveyor west"
	BLOCKED_TO_MONSTERS = 20,
	TOWN_ENTRANCE = 21,
	CHANGE_WHEN_USED = 22,
	CALL_SPECIAL_WHEN_USED = 23,
	//	1. Change when step on (What to change to, number of sound, Unused)
	//	2. Damaging terrain; can't rest here (Amount of damage done, multiplier, damage type)
	//	3. Bridge - if the party boats over it, they get the option to land. (Unused, Unused, Unused)
	//	4. Bed - change party graphic when they stand on this space
	//	5. Dangerous land; can't rest here; percentage chance may be 0 (Strength, Percentage chance, status type)
	//	6. Reserved
	//	7. Crumbling terrain (Terrain to change to, strength?, destroyed by what - quickfire, shatter/move mountains, or both)
	//	8. Lockable terrain (Terrain to change to when locked, Unused, Unused)
	//	9. Unlockable terrain (Terrain to change to when locked, Difficulty, can be bashed)
	//	10. Reserved
	//	11. Sign (Unused, Unused, Unused)
	//	12. Call special (Special to call, local or scenario?, Unused)
	//	13. Reserved
	//	14. Container (Unused, Unused, Unused)
	//	15. Waterfall (Direction, Unused, Unused)
	//	16. Conveyor Belt (Direction, Unused, Unused)
	//	17. Reserved
	//	18. Reserved
	//	19. Reserved
	//	20. Blocked to Monsters (Unused, Unused, Unused)
	//	21. Town entrance (Terrain type if hidden, Unused, Unused)
	//	22. Change when Used (Terrain to change to when used, Number of sound, Unused)
	//	23. Call special when used (Special to call, local or scenario?, Unused)
};

enum class eTrimType {
	NONE = 0,
	WALL = 1, // not a trim, but trims will conform to it as if it's the same ground type (eg stone wall)
	S, SE, E, NE, N, NW, W, SW,
	NE_INNER, SE_INNER, SW_INNER, NW_INNER,
	FRILLS = 14, // like on lava and underground water; no trim_ter required
	ROAD = 15, // the game will treat it like a road space and draw roads; no trim_ter required
	WALKWAY = 16, // the game will draw walkway corners; trim_ter is base terrain to draw on
	WATERFALL = 17, // special case for waterfalls
	CITY = 18, // the game will join roads up to this space but not draw roads on the space
};

/*      terrain type blockage          */
enum class eTerObstruct {
	CLEAR = 0,
	BLOCK_SIGHT = 1,
	BLOCK_MONSTERS = 2,
	BLOCK_MOVE = 3,
	BLOCK_MOVE_AND_SHOOT = 4,
	BLOCK_MOVE_AND_SIGHT = 5,
};

inline bool blocksMove(eTerObstruct block) {
	int code = (int) block;
	return code > 2;
}

/*      items[i].type    a.k.a type of weapon         */
enum class eWeapType {
	NOT_MELEE = 0,
	EDGED = 1,
	BASHING = 2,
	POLE = 3,
};

/*      items[i].variety    a.k.a item type (in editor)      */
enum class eItemType {
	NO_ITEM = 0,
	ONE_HANDED = 1,
	TWO_HANDED = 2,
	GOLD = 3,
	BOW = 4,
	ARROW = 5,
	THROWN_MISSILE = 6,
	POTION = 7, // potion/magic item
	SCROLL = 8, // scroll/magic item
	WAND = 9,
	TOOL = 10,
	FOOD = 11,
	SHIELD = 12,
	ARMOR = 13,
	HELM = 14,
	GLOVES = 15,
	SHIELD_2 = 16,
	// don't know why a second type of shield is used ; it is actually checked
	// in the armor code -- see below
	// and you can't equip another (12) shield while wearing it ... I didn't
	// find a single item with this property in the bladbase.exs ...
	BOOTS = 17,
	RING = 18,
	NECKLACE = 19,
	WEAPON_POISON = 20,
	NON_USE_OBJECT = 21,
	PANTS = 22,
	CROSSBOW = 23,
	BOLTS = 24,
	MISSILE_NO_AMMO = 25, //e.g slings
	UNUSED1 = 26, // these are here solely because they are options in the editor
	UNUSED2 = 27,
};

inline bool isArmourType(eItemType type) {
	int code = (int) type;
	return code >= 12 && code <= 17;
}

inline bool isWeaponType(eItemType type) {
	if(type == eItemType::CROSSBOW || type == eItemType::BOLTS)
		return true;
	int code = (int) type;
	return code >= 1 && code <= 6 && code != 3;
}

/*      items[i].ability      */
enum eItemAbil {
	// Weapon abilities
	ITEM_NO_ABILITY = 0,
	ITEM_FLAMING_WEAPON = 1,
	ITEM_DEMON_SLAYER = 2,
	ITEM_UNDEAD_SLAYER = 3,
	ITEM_LIZARD_SLAYER = 4,
	ITEM_GIANT_SLAYER = 5,
	ITEM_MAGE_SLAYER = 6,
	ITEM_PRIEST_SLAYER = 7,
	ITEM_BUG_SLAYER = 8,
	ITEM_ACIDIC_WEAPON = 9,
	ITEM_SOULSUCKER = 10,
	ITEM_DRAIN_MISSILES = 11,
	ITEM_WEAK_WEAPON = 12,
	ITEM_CAUSES_FEAR = 13,
	ITEM_POISONED_WEAPON = 14,
	// General abilities
	ITEM_PROTECTION = 30,
	ITEM_FULL_PROTECTION = 31,
	ITEM_FIRE_PROTECTION = 32,
	ITEM_COLD_PROTECTION = 33,
	ITEM_POISON_PROTECTION = 34,
	ITEM_MAGIC_PROTECTION = 35,
	ITEM_ACID_PROTECTION = 36,
	ITEM_SKILL = 37,
	ITEM_STRENGTH = 38,
	ITEM_DEXTERITY = 39,
	ITEM_INTELLIGENCE = 40,
	ITEM_ACCURACY = 41,
	ITEM_THIEVING = 42,
	ITEM_GIANT_STRENGTH = 43,
	ITEM_LIGHTER_OBJECT = 44,
	ITEM_HEAVIER_OBJECT = 45,
	ITEM_OCCASIONAL_BLESS = 46,
	ITEM_OCCASIONAL_HASTE = 47,
	ITEM_LIFE_SAVING = 48,
	ITEM_PROTECT_FROM_PETRIFY = 49,
	ITEM_REGENERATE = 50,
	ITEM_POISON_AUGMENT = 51,
	ITEM_DISEASE_PARTY = 52,
	ITEM_WILL = 53,
	ITEM_FREE_ACTION = 54,
	ITEM_SPEED = 55,
	ITEM_SLOW_WEARER = 56,
	ITEM_PROTECT_FROM_UNDEAD = 57,
	ITEM_PROTECT_FROM_DEMONS = 58,
	ITEM_PROTECT_FROM_HUMANOIDS = 59,
	ITEM_PROTECT_FROM_REPTILES = 60,
	ITEM_PROTECT_FROM_GIANTS = 61,
	ITEM_PROTECT_FROM_DISEASE = 62,
	// Nonspell Usable
	ITEM_POISON_WEAPON = 70, //put poison on weapon
	ITEM_BLESS_CURSE = 71,
	ITEM_AFFECT_POISON = 72,
	ITEM_HASTE_SLOW = 73,
	ITEM_AFFECT_INVULN = 74,
	ITEM_AFFECT_MAGIC_RES = 75,
	ITEM_AFFECT_WEB = 76,
	ITEM_AFFECT_DISEASE = 77,
	ITEM_AFFECT_SANCTUARY = 78,
	ITEM_AFFECT_DUMBFOUND = 79,
	ITEM_AFFECT_MARTYRS_SHIELD = 80,
	ITEM_AFFECT_SLEEP = 81,
	ITEM_AFFECT_PARALYSIS = 82,
	ITEM_AFFECT_ACID = 83,
	ITEM_BLISS = 84,
	ITEM_AFFECT_EXPERIENCE = 85,
	ITEM_AFFECT_SKILL_POINTS = 86,
	ITEM_AFFECT_HEALTH = 87,
	ITEM_AFFECT_SPELL_POINTS = 88,
	ITEM_DOOM = 89,
	ITEM_LIGHT = 90,
	ITEM_STEALTH = 91,
	ITEM_FIREWALK = 92,
	ITEM_FLYING = 93,
	ITEM_MAJOR_HEALING = 94,
	ITEM_CALL_SPECIAL = 95,
	// Spell Usable
	ITEM_SPELL_FLAME = 110,
	ITEM_SPELL_FIREBALL = 111,
	ITEM_SPELL_FIRESTORM = 112,
	ITEM_SPELL_KILL = 113,
	ITEM_SPELL_ICE_BOLT = 114,
	ITEM_SPELL_SLOW = 115,
	ITEM_SPELL_SHOCKWAVE = 116,
	ITEM_SPELL_DISPEL_UNDEAD = 117,
	ITEM_SPELL_DISPEL_SPIRIT = 118,
	ITEM_SPELL_SUMMONING = 119,
	ITEM_SPELL_MASS_SUMMONING = 120,
	ITEM_SPELL_ACID_SPRAY = 121,
	ITEM_SPELL_STINKING_CLOUD = 122,
	ITEM_SPELL_SLEEP_FIELD = 123,
	ITEM_SPELL_VENOM = 124,
	ITEM_SPELL_SHOCKSTORM = 125,
	ITEM_SPELL_PARALYSIS = 126,
	ITEM_SPELL_WEB_SPELL = 127,
	ITEM_SPELL_STRENGTHEN_TARGET = 128, //wand of carrunos effect
	ITEM_SPELL_QUICKFIRE = 129,
	ITEM_SPELL_MASS_CHARM = 130,
	ITEM_SPELL_MAGIC_MAP = 131,
	ITEM_SPELL_DISPEL_BARRIER = 132,
	ITEM_SPELL_MAKE_ICE_WALL = 133,
	ITEM_SPELL_CHARM_SPELL = 134,
	ITEM_SPELL_ANTIMAGIC_CLOUD = 135,
	// Reagents
	ITEM_HOLLY = 150, // Holly/Toadstool
	ITEM_COMFREY_ROOT = 151,
	ITEM_GLOWING_NETTLE = 152,
	ITEM_WORMGRASS = 153, // Crypt Shroom/Wormgr.
	ITEM_ASPTONGUE_MOLD = 154,
	ITEM_EMBER_FLOWERS = 155,
	ITEM_GRAYMOLD = 156,
	ITEM_MANDRAKE = 157,
	ITEM_SAPPHIRE = 158,
	ITEM_SMOKY_CRYSTAL = 159,
	ITEM_RESSURECTION_BALM = 160,
	ITEM_LOCKPICKS = 161,
	// Missile Abilities
	ITEM_MISSILE_RETURNING = 170,
	ITEM_MISSILE_LIGHTNING = 171,
	ITEM_MISSILE_EXPLODING = 172,
	ITEM_MISSILE_ACID = 173,
	ITEM_MISSILE_SLAY_UNDEAD = 174,
	ITEM_MISSILE_SLAY_DEMON = 175,
	ITEM_MISSILE_HEAL_TARGET = 176,
};


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
	SKILL_MAX_HP = 19,
	SKILL_MAX_SP = 20,
};

/* adven[i].traits */ //complete
enum class eTrait {
	TOUGHNESS = 0,
	MAGICALLY_APT = 1,
	AMBIDEXTROUS = 2,
	NIMBLE = 3,
	CAVE_LORE = 4,
	WOODSMAN = 5,
	GOOD_CONST = 6,
	HIGHLY_ALERT = 7,
	STRENGTH = 8,
	RECUPERATION = 9,
	SLUGGISH = 10,
	MAGICALLY_INEPT = 11,
	FRAIL = 12,
	CHRONIC_DISEASE = 13,
	BAD_BACK = 14,
	PACIFIST = 15,
};


/* damage type*/
/* used as parameter to some functions */
enum eDamageType {
	DAMAGE_WEAPON = 0,
	DAMAGE_FIRE = 1,
	DAMAGE_POISON = 2,
	DAMAGE_MAGIC = 3,
	DAMAGE_UNBLOCKABLE = 4, //from the source files - the display is the same as the magic one (damage_monst in SPECIALS.cpp)
	DAMAGE_COLD = 5,
	DAMAGE_UNDEAD = 6, //from the source files - the display is the same as the weapon one
	DAMAGE_DEMON = 7, //from the source files - the display is the same as the weapon one
	// 8 and 9 aren't defined : doesn't print any damage. According to the source files the 9 is DAMAGE_MARKED though. Wrong ?
	DAMAGE_MARKED = 10, // usage: DAMAGE_MARKED + damage_type
	DAMAGE_WEAPON_MARKED = 10,
	DAMAGE_FIRE_MARKED = 11,
	DAMAGE_POISON_MARKED = 12,
	DAMAGE_MAGIC_MARKED = 13,
	DAMAGE_UNBLOCKABLE_MARKED = 14,
	DAMAGE_COLD_MARKED = 15,
	DAMAGE_UNDEAD_MARKED = 16,
	DAMAGE_DEMON_MARKED = 17,
	DAMAGE_NO_PRINT = 30, // usage: DAMAGE_NO_PRINT + damage_type
	DAMAGE_WEAPON_NO_PRINT = 30,
	DAMAGE_FIRE_NO_PRINT = 31,
	DAMAGE_POISON_NO_PRINT = 32,
	DAMAGE_MAGIC_NO_PRINT = 33,
	DAMAGE_UNBLOCKABLE_NO_PRINT = 34,
	DAMAGE_COLD_NO_PRINT = 35,
	DAMAGE_UNDEAD_NO_PRINT = 36,
	DAMAGE_DEMON_NO_PRINT = 37,
	// What about both NO_PRINT and MARKED?
};

inline void operator -= (eDamageType& cur, eDamageType othr){
	if((othr == DAMAGE_MARKED && cur >= DAMAGE_MARKED && cur < DAMAGE_NO_PRINT) ||
	   (othr == DAMAGE_NO_PRINT && cur >= DAMAGE_NO_PRINT))
		cur = (eDamageType) ((int)cur - (int)othr);
}

inline void operator += (eDamageType& cur, eDamageType othr){
	if((othr == DAMAGE_MARKED || othr == DAMAGE_NO_PRINT) && cur < DAMAGE_MARKED)
		cur = (eDamageType) ((int)cur + (int)othr);
}

//class sbyte {
//	signed char c;
//public:
//	operator int() {return c;}
//	sbyte(signed char k) : c(k) {}
//}

enum class eSpecCtx {
	OUT_MOVE = 0,
	TOWN_MOVE = 1,
	COMBAT_MOVE = 2,
	OUT_LOOK = 3,
	TOWN_LOOK = 4,
	ENTER_TOWN = 5,
	LEAVE_TOWN = 6,
	TALK = 7,
	USE_SPEC_ITEM = 8,
	TOWN_TIMER = 9,
	SCEN_TIMER = 10,
	PARTY_TIMER = 11,
	KILL_MONST = 12,
	OUTDOOR_ENC = 13,
	FLEE_ENCOUNTER = 14,
	WIN_ENCOUNTER = 15,
	TARGET = 16,
	USE_SPACE = 17,
	SEE_MONST = 18,
	MONST_SPEC_ABIL = 19,
	TOWN_HOSTILE = 20,
};

enum class eSpecType {
	ERROR = -1, // A magic value used while processing nodes
	NONE = 0,
	SET_SDF = 1,
	INC_SDF = 2,
	DISPLAY_MSG = 3,
	SECRET_PASSAGE = 4,
	DISPLAY_SM_MSG = 5,
	FLIP_SDF = 6,
	SDF_RANDOM = 7, // formerly OUT_BLOCK
	SDF_ADD = 8, // formerly TOWN_BLOCK
	SDF_DIFF = 9, // formerly FIGHT_BLOCK
	UNUSED1 = 10, // formerly LOOK_BLOCK
	CANT_ENTER = 11,
	CHANGE_TIME = 12,
	SCEN_TIMER_START = 13,
	PLAY_SOUND = 14,
	CHANGE_HORSE_OWNER = 15,
	CHANGE_BOAT_OWNER = 16,
	SET_TOWN_VISIBILITY = 17,
	MAJOR_EVENT_OCCURRED = 18,
	FORCED_GIVE = 19,
	BUY_ITEMS_OF_TYPE = 20,
	CALL_GLOBAL = 21,
	SET_SDF_ROW = 22,
	COPY_SDF = 23,
	DISPLAY_PICTURE = 24, // formerly SANCTIFY
	REST = 25,
	WANDERING_WILL_FIGHT = 26,
	END_SCENARIO = 27,
	SET_POINTER = 28,
	SET_CAMP_FLAG = 29,
	PRINT_NUMS = 30, // For debugging
	SDF_TIMES = 31,
	SDF_DIVIDE = 32, // Computes both quotient and remainder
	SDF_POWER = 33,
	ONCE_GIVE_ITEM = 50,
	ONCE_GIVE_SPEC_ITEM = 51,
	ONCE_NULL = 52,
	ONCE_SET_SDF = 53,
	ONCE_DISPLAY_MSG = 54,
	ONCE_DIALOG = 55,
	ONCE_DIALOG_TERRAIN = 56,
	ONCE_DIALOG_MONSTER = 57,
	ONCE_GIVE_ITEM_DIALOG = 58,
	ONCE_GIVE_ITEM_TERRAIN = 59,
	ONCE_GIVE_ITEM_MONSTER = 60,
	ONCE_OUT_ENCOUNTER = 61,
	ONCE_TOWN_ENCOUNTER = 62,
	ONCE_TRAP = 63,
	SELECT_PC = 80,
	DAMAGE = 81,
	AFFECT_HP = 82,
	AFFECT_SP = 83,
	AFFECT_XP = 84,
	AFFECT_SKILL_PTS = 85,
	AFFECT_DEADNESS = 86,
	AFFECT_POISON = 87,
	AFFECT_SPEED = 88,
	AFFECT_INVULN = 89,
	AFFECT_MAGIC_RES = 90,
	AFFECT_WEBS = 91,
	AFFECT_DISEASE = 92,
	AFFECT_SANCTUARY = 93,
	AFFECT_CURSE_BLESS = 94,
	AFFECT_DUMBFOUND = 95,
	AFFECT_SLEEP = 96,
	AFFECT_PARALYSIS = 97,
	AFFECT_STAT = 98,
	AFFECT_MAGE_SPELL = 99,
	AFFECT_PRIEST_SPELL = 100,
	AFFECT_GOLD = 101,
	AFFECT_FOOD = 102,
	AFFECT_ALCHEMY = 103,
	AFFECT_STEALTH = 104,
	AFFECT_FIREWALK = 105,
	AFFECT_FLIGHT = 106,
	IF_SDF = 130,
	IF_TOWN_NUM = 131,
	IF_RANDOM = 132,
	IF_HAVE_SPECIAL_ITEM = 133,
	IF_SDF_COMPARE = 134,
	IF_TOWN_TER_TYPE = 135,
	IF_OUT_TER_TYPE = 136,
	IF_HAS_GOLD = 137,
	IF_HAS_FOOD = 138,
	IF_ITEM_CLASS_ON_SPACE = 139,
	IF_HAVE_ITEM_CLASS = 140,
	IF_EQUIP_ITEM_CLASS = 141,
	IF_HAS_GOLD_AND_TAKE = 142,
	IF_HAS_FOOD_AND_TAKE = 143,
	IF_ITEM_CLASS_ON_SPACE_AND_TAKE = 144,
	IF_HAVE_ITEM_CLASS_AND_TAKE = 145,
	IF_EQUIP_ITEM_CLASS_AND_TAKE = 146,
	IF_DAY_REACHED = 147,
	IF_OBJECTS = 148,
	IF_PARTY_SIZE = 149,
	IF_EVENT_OCCURRED = 150,
	IF_SPECIES = 151,
	IF_TRAIT = 152,
	IF_STATISTIC = 153,
	IF_TEXT_RESPONSE = 154,
	IF_SDF_EQ = 155,
	IF_CONTEXT = 156,
	IF_NUM_RESPONSE = 157,
	MAKE_TOWN_HOSTILE = 170,
	TOWN_CHANGE_TER = 171,
	TOWN_SWAP_TER = 172,
	TOWN_TRANS_TER = 173,
	TOWN_MOVE_PARTY = 174,
	TOWN_HIT_SPACE = 175,
	TOWN_EXPLODE_SPACE = 176,
	TOWN_LOCK_SPACE = 177,
	TOWN_UNLOCK_SPACE = 178,
	TOWN_SFX_BURST = 179,
	TOWN_CREATE_WANDERING = 180,
	TOWN_PLACE_MONST = 181,
	TOWN_DESTROY_MONST = 182,
	TOWN_NUKE_MONSTS = 183,
	TOWN_GENERIC_LEVER = 184,
	TOWN_GENERIC_PORTAL = 185,
	TOWN_GENERIC_BUTTON = 186,
	TOWN_GENERIC_STAIR = 187,
	TOWN_LEVER = 188,
	TOWN_PORTAL = 189,
	TOWN_STAIR = 190,
	TOWN_RELOCATE = 191, // Relocate outdoors
	TOWN_PLACE_ITEM = 192,
	TOWN_SPLIT_PARTY = 193,
	TOWN_REUNITE_PARTY = 194,
	TOWN_TIMER_START = 195,
	TOWN_CHANGE_LIGHTING = 196,
	TOWN_SET_ATTITUDE = 197,
	RECT_PLACE_FIRE = 200,
	RECT_PLACE_FORCE = 201,
	RECT_PLACE_ICE = 202,
	RECT_PLACE_BLADE = 203,
	RECT_PLACE_SCLOUD = 204,
	RECT_PLACE_SLEEP = 205,
	RECT_PLACE_QUICKFIRE = 206,
	RECT_PLACE_FIRE_BARR = 207,
	RECT_PLACE_FORCE_BARR = 208,
	RECT_CLEANSE = 209,
	RECT_PLACE_SFX = 210,
	RECT_PLACE_OBJECT = 211, // place barrels, etc
	RECT_MOVE_ITEMS = 212,
	RECT_DESTROY_ITEMS = 213,
	RECT_CHANGE_TER = 214,
	RECT_SWAP_TER = 215,
	RECT_TRANS_TER = 216,
	RECT_LOCK = 217,
	RECT_UNLOCK = 218,
	OUT_MAKE_WANDER = 225,
	OUT_CHANGE_TER = 226,
	OUT_PLACE_ENCOUNTER = 227,
	OUT_MOVE_PARTY = 228,
	OUT_STORE = 229,
};

enum class eSpecCat {
	INVALID = -1,
	GENERAL, ONCE, AFFECT, IF_THEN, TOWN, RECT, OUTDOOR
};

inline eSpecCat getNodeCategory(eSpecType node) {
	int code = (int) node;
	if(code >= 0 && code <= 33)
		return eSpecCat::GENERAL;
	if(code >= 50 && code <= 63)
		return eSpecCat::ONCE;
	if(code >= 80 && code <= 106)
		return eSpecCat::AFFECT;
	if(code >= 130 && code <= 157)
		return eSpecCat::IF_THEN;
	if(code >= 170 && code <= 197)
		return eSpecCat::TOWN;
	if(code >= 200 && code <= 218)
		return eSpecCat::RECT;
	if(code >= 225 && code <= 229)
		return eSpecCat::OUTDOOR;
	return eSpecCat::INVALID;
}

enum eTalkNodeType {
	TALK_REGULAR = 0,
	TALK_DEP_ON_SDF = 1,
	TALK_SET_SDF = 2,
	TALK_INN = 3,
	TALK_DEP_ON_TIME = 4,
	TALK_DEP_ON_TIME_AND_EVENT = 5,
	TALK_DEP_ON_TOWN = 6,
	TALK_BUY_ITEMS = 7,
	TALK_TRAINING = 8,
	TALK_BUY_MAGE = 9,
	TALK_BUY_PRIEST = 10,
	TALK_BUY_ALCHEMY = 11,
	TALK_BUY_HEALING = 12,
	TALK_SELL_WEAPONS = 13,
	TALK_SELL_ARMOR = 14,
	TALK_SELL_ITEMS = 15,
	TALK_IDENTIFY = 16,
	TALK_ENCHANT = 17,
	TALK_BUY_INFO = 18,
	TALK_BUY_SDF = 19,
	TALK_BUY_SHIP = 20,
	TALK_BUY_HORSE = 21,
	TALK_BUY_SPEC_ITEM = 22,
	TALK_BUY_JUNK = 23,
	TALK_BUY_TOWN_LOC = 24,
	TALK_END_FORCE = 25,
	TALK_END_FIGHT = 26,
	TALK_END_ALARM = 27, // Town hostile
	TALK_END_DIE = 28,
	TALK_CALL_TOWN_SPEC = 29,
	TALK_CALL_SCEN_SPEC = 30,
};

enum eEncNoteType {
	NOTE_SCEN,
	NOTE_OUT,
	NOTE_TOWN,
	NOTE_MONST,
};

enum eFieldType {
	FIELD_NONE = 0,
	FIELD_WEB = 1,
	BARRIER_FIRE = 2,
	BARRIER_FORCE = 3,
	WALL_FORCE = 4,
	WALL_FIRE = 5,
	FIELD_ANTIMAGIC = 6,
	CLOUD_STINK = 7,
	WALL_ICE = 8,
	WALL_BLADES = 9,
	FIELD_QUICKFIRE = 10,
	FIELD_DISPEL = 11,
	CLOUD_SLEEP = 12,
	// TODO: Sfx fields, objects
};

#endif
