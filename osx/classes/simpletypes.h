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
enum eStatus {
	STATUS_POISONED_WEAPON = 0,
	STATUS_BLESS_CURSE = 1,
	STATUS_POISON = 2,
	STATUS_HASTE_SLOW = 3,
	STATUS_INVULNERABLE = 4,
	STATUS_MAGIC_RESISTANCE = 5,
	STATUS_WEBS = 6,
	STATUS_DISEASE = 7,
	STATUS_INVISIBLE = 8, //sanctuary
	STATUS_DUMB = 9,
	STATUS_MARTYRS_SHIELD = 10,
	STATUS_ASLEEP = 11,
	STATUS_PARALYZED = 12,
	STATUS_ACID = 13,
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

enum eTerSpec {
//	TER_SPEC_NONE = 0,
//	TER_SPEC_CHANGE_WHEN_STEP_ON = 1,
//	TER_SPEC_DOES_FIRE_DAMAGE = 2,
//	TER_SPEC_DOES_COLD_DAMAGE = 3,
//	TER_SPEC_DOES_MAGIC_DAMAGE = 4,
//	TER_SPEC_POISON_LAND = 5,
//	TER_SPEC_DISEASED_LAND = 6,
//	TER_SPEC_CRUMBLING_TERRAIN = 7,
//	TER_SPEC_LOCKABLE_TERRAIN = 8,
//	TER_SPEC_UNLOCKABLE_TERRAIN = 9,
//	TER_SPEC_UNLOCKABLE_BASHABLE = 10,
//	TER_SPEC_IS_A_SIGN = 11,
//	TER_SPEC_CALL_LOCAL_SPECIAL = 12,
//	TER_SPEC_CALL_SCENARIO_SPECIAL = 13,
//	TER_SPEC_IS_A_CONTAINER = 14,
//	TER_SPEC_WATERFALL = 15,
//	TER_SPEC_CONVEYOR_NORTH = 16,
//	TER_SPEC_CONVEYOR_EAST = 17,
//	TER_SPEC_CONVEYOR_SOUTH = 18,
//	TER_SPEC_CONVEYOR_WEST = 19,
//	TER_SPEC_BLOCKED_TO_MONSTERS = 20,
//	TER_SPEC_TOWN_ENTRANCE = 21,
//	TER_SPEC_CHANGE_WHEN_USED = 22,
//	TER_SPEC_CALL_SPECIAL_WHEN_USED = 23,
	TER_SPEC_NONE = 0,
	TER_SPEC_CHANGE_WHEN_STEP_ON = 1,
	TER_SPEC_DAMAGING = 2,
	TER_SPEC_BRIDGE = 3, // new
	TER_SPEC_BED = 4, // new
	TER_SPEC_DANGEROUS = 5,
	TER_SPEC_UNUSED1 = 6,
	TER_SPEC_CRUMBLING = 7,
	TER_SPEC_LOCKABLE = 8,
	TER_SPEC_UNLOCKABLE = 9,
	TER_SPEC_UNUSED2 = 10,
	TER_SPEC_IS_A_SIGN = 11,
	TER_SPEC_CALL_SPECIAL = 12,
	TER_SPEC_UNUSED3 = 13,
	TER_SPEC_IS_A_CONTAINER = 14,
	TER_SPEC_WATERFALL = 15,
	TER_SPEC_CONVEYOR = 16,
	TER_SPEC_UNUSED4 = 17,
	TER_SPEC_UNUSED5 = 18,
	TER_SPEC_UNUSED6 = 19,
	TER_SPEC_BLOCKED_TO_MONSTERS = 20,
	TER_SPEC_TOWN_ENTRANCE = 21,
	TER_SPEC_CHANGE_WHEN_USED = 22,
	TER_SPEC_CALL_SPECIAL_WHEN_USED = 23,
	//	1. Change when step on (What to change to, number of sound, Unused)
	//	2. Damaging terrain; can't rest here (Amount of damage done, multiplier, damage type)
	//	3. Reserved
	//	4. Reserved
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
	//	24. Bridge - if the party boats over it, they get the option to land. (Unused, Unused, Unused)
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

enum eSpecContext {
	SPEC_OUT_MOVE = 0,
	SPEC_TOWN_MOVE = 1,
	SPEC_COMBAT_MOVE = 2,
	SPEC_OUT_LOOK = 3,
	SPEC_TOWN_LOOK = 4,
	SPEC_ENTER_TOWN = 5,
	SPEC_LEAVE_TOWN = 6,
	SPEC_TALK = 7,
	SPEC_USE_SPEC_ITEM = 8,
	SPEC_TOWN_TIMER = 9,
	SPEC_SCEN_TIMER = 10,
	SPEC_PARTY_TIMER = 11,
	SPEC_KILL_MONST = 12,
	SPEC_OUTDOOR_ENC = 13,
	SPEC_WIN_ENCOUNTER = 14,
	SPEC_FLEE_ENCOUNTER = 15,
	SPEC_TARGET = 16,
	SPEC_USE_SPACE = 17,
	SPEC_SEE_MONST = 18,
};

enum eSpecNodeType {
	SPEC_NULL = 0,
	SPEC_SET_SDF = 1,
	SPEC_INC_SDF = 2,
	SPEC_DISPLAY_MSG = 3,
	SPEC_SECRET_PASSAGE = 4,
	SPEC_DISPLAY_SM_MSG = 5,
	SPEC_FLIP_SDF = 6,
	SPEC_OUT_BLOCK = 7,
	SPEC_TOWN_BLOCK = 8,
	SPEC_FIGHT_BLOCK = 9,
	SPEC_LOOK_BLOCK = 10,
	SPEC_CANT_ENTER = 11,
	SPEC_CHANGE_TIME = 12,
	SPEC_SCEN_TIMER_START = 13,
	SPEC_PLAY_SOUND = 14,
	SPEC_CHANGE_HORSE_OWNER = 15,
	SPEC_CHANGE_BOAT_OWNER = 16,
	SPEC_SET_TOWN_VISIBILITY = 17,
	SPEC_MAJOR_EVENT_OCCURRED = 18,
	SPEC_FORCED_GIVE = 19,
	SPEC_BUY_ITEMS_OF_TYPE = 20,
	SPEC_CALL_GLOBAL = 21,
	SPEC_SET_SDF_ROW = 22,
	SPEC_COPY_SDF = 23,
	SPEC_SANCTIFY = 24,
	SPEC_REST = 25,
	SPEC_WANDERING_WILL_FIGHT = 26,
	SPEC_END_SCENARIO = 27,
	SPEC_ONCE_GIVE_ITEM = 50,
	SPEC_ONCE_GIVE_SPEC_ITEM = 51,
	SPEC_ONCE_NULL = 52,
	SPEC_ONCE_SET_SDF = 53,
	SPEC_ONCE_DISPLAY_MSG = 54,
	SPEC_ONCE_DIALOG = 55,
	SPEC_ONCE_DIALOG_TERRAIN = 56,
	SPEC_ONCE_DIALOG_MONSTER = 57,
	SPEC_ONCE_GIVE_ITEM_DIALOG = 58,
	SPEC_ONCE_GIVE_ITEM_TERRAIN = 59,
	SPEC_ONCE_GIVE_ITEM_MONSTER = 60,
	SPEC_ONCE_OUT_ENCOUNTER = 61,
	SPEC_ONCE_TOWN_ENCOUNTER = 62,
	SPEC_ONCE_TRAP = 63,
	SPEC_SELECT_PC = 80,
	SPEC_DAMAGE = 81,
	SPEC_AFFECT_HP = 82,
	SPEC_AFFECT_SP = 83,
	SPEC_AFFECT_XP = 84,
	SPEC_AFFECT_SKILL_PTS = 85,
	SPEC_AFFECT_DEADNESS = 86,
	SPEC_AFFECT_POISON = 87,
	SPEC_AFFECT_SPEED = 88,
	SPEC_AFFECT_INVULN = 89,
	SPEC_AFFECT_MAGIC_RES = 90,
	SPEC_AFFECT_WEBS = 91,
	SPEC_AFFECT_DISEASE = 92,
	SPEC_AFFECT_SANCTUARY = 93,
	SPEC_AFFECT_CURSE_BLESS = 94,
	SPEC_AFFECT_DUMBFOUND = 95,
	SPEC_AFFECT_SLEEP = 96,
	SPEC_AFFECT_PARALYSIS = 97,
	SPEC_AFFECT_STAT = 98,
	SPEC_AFFECT_MAGE_SPELL = 99,
	SPEC_AFFECT_PRIEST_SPELL = 100,
	SPEC_AFFECT_GOLD = 101,
	SPEC_AFFECT_FOOD = 102,
	SPEC_AFFECT_ALCHEMY = 103,
	SPEC_AFFECT_STEALTH = 104,
	SPEC_AFFECT_FIREWALK = 105,
	SPEC_AFFECT_FLIGHT = 106,
	SPEC_IF_SDF = 130,
	SPEC_IF_TOWN_NUM = 131,
	SPEC_IF_RANDOM = 132,
	SPEC_IF_HAVE_SPECIAL_ITEM = 133,
	SPEC_IF_SDF_COMPARE = 134,
	SPEC_IF_TOWN_TER_TYPE = 135,
	SPEC_IF_OUT_TER_TYPE = 136,
	SPEC_IF_HAS_GOLD = 137,
	SPEC_IF_HAS_FOOD = 138,
	SPEC_IF_ITEM_CLASS_ON_SPACE = 139,
	SPEC_IF_HAVE_ITEM_CLASS = 140,
	SPEC_IF_EQUIP_ITEM_CLASS = 141,
	SPEC_IF_HAS_GOLD_AND_TAKE = 142,
	SPEC_IF_HAS_FOOD_AND_TAKE = 143,
	SPEC_IF_ITEM_CLASS_ON_SPACE_AND_TAKE = 144,
	SPEC_IF_HAVE_ITEM_CLASS_AND_TAKE = 145,
	SPEC_IF_EQUIP_ITEM_CLASS_AND_TAKE = 146,
	SPEC_IF_DAY_REACHED = 147,
	SPEC_IF_BARRELS = 148,
	SPEC_IF_CRATES = 149,
	SPEC_IF_EVENT_OCCURRED = 150,
	SPEC_IF_HAS_CAVE_LORE = 151,
	SPEC_IF_HAS_WOODSMAN = 152,
	SPEC_IF_ENOUGH_MAGE_LORE = 153,
	SPEC_IF_TEXT_RESPONSE = 154,
	SPEC_IF_SDF_EQ = 155,
	SPEC_MAKE_TOWN_HOSTILE = 170,
	SPEC_TOWN_CHANGE_TER = 171,
	SPEC_TOWN_SWAP_TER = 172,
	SPEC_TOWN_TRANS_TER = 173,
	SPEC_TOWN_MOVE_PARTY = 174,
	SPEC_TOWN_HIT_SPACE = 175,
	SPEC_TOWN_EXPLODE_SPACE = 176,
	SPEC_TOWN_LOCK_SPACE = 177,
	SPEC_TOWN_UNLOCK_SPACE = 178,
	SPEC_TOWN_SFX_BURST = 179,
	SPEC_TOWN_CREATE_WANDERING = 180,
	SPEC_TOWN_PLACE_MONST = 181,
	SPEC_TOWN_DESTROY_MONST = 182,
	SPEC_TOWN_NUKE_MONSTS = 183,
	SPEC_TOWN_GENERIC_LEVER = 184,
	SPEC_TOWN_GENERIC_PORTAL = 185,
	SPEC_TOWN_GENERIC_BUTTON = 186,
	SPEC_TOWN_GENERIC_STAIR = 187,
	SPEC_TOWN_LEVER = 188,
	SPEC_TOWN_PORTAL = 189,
	SPEC_TOWN_STAIR = 190,
	SPEC_TOWN_RELOCATE = 191, // Relocate outdoors
	SPEC_TOWN_PLACE_ITEM = 192,
	SPEC_TOWN_SPLIT_PARTY = 193,
	SPEC_TOWN_REUNITE_PARTY = 194,
	SPEC_TOWN_TIMER_START = 195,
	SPEC_RECT_PLACE_FIRE = 200,
	SPEC_RECT_PLACE_FORCE = 201,
	SPEC_RECT_PLACE_ICE = 202,
	SPEC_RECT_PLACE_BLADE = 203,
	SPEC_RECT_PLACE_SCLOUD = 204,
	SPEC_RECT_PLACE_SLEEP = 205,
	SPEC_RECT_PLACE_QUICKFIRE = 206,
	SPEC_RECT_PLACE_FIRE_BARR = 207,
	SPEC_RECT_PLACE_FORCE_BARR = 208,
	SPEC_RECT_CLEANSE = 209,
	SPEC_RECT_PLACE_SFX = 210,
	SPEC_RECT_PLACE_OBJECT = 211, // place barrels, etc
	SPEC_RECT_MOVE_ITEMS = 212,
	SPEC_RECT_DESTROY_ITEMS = 213,
	SPEC_RECT_CHANGE_TER = 214,
	SPEC_RECT_SWAP_TER = 215,
	SPEC_RECT_TRANS_TER = 216,
	SPEC_RECT_LOCK = 217,
	SPEC_RECT_UNLOCK = 218,
	SPEC_OUT_MAKE_WANDER = 225,
	SPEC_OUT_CHANGE_TER = 226,
	SPEC_OUT_PLACE_ENCOUNTER = 227,
	SPEC_OUT_MOVE_PARTY = 228,
	SPEC_OUT_STORE = 229,
};

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

#endif
