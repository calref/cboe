/*
 *  simpletypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 01/06/09.
 *
 */

#ifndef BOE_DATA_SIMPLETYPES_H
#define BOE_DATA_SIMPLETYPES_H

typedef unsigned short mon_num_t;
typedef signed short miss_num_t;
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
	FORCECAGE = 14,
	// This one is new
	// It's not quite a real status effect since it doesn't expire
	CHARM = 15,
};

inline bool isStatusNegative(eStatus stat) {
	switch(stat) {
		case eStatus::POISONED_WEAPON: return false;
		case eStatus::BLESS_CURSE: return false;
		case eStatus::POISON: return true;
		case eStatus::HASTE_SLOW: return false;
		case eStatus::INVULNERABLE: return false;
		case eStatus::MAGIC_RESISTANCE: return false;
		case eStatus::WEBS: return true;
		case eStatus::DISEASE: return true;
		case eStatus::INVISIBLE: return false;
		case eStatus::DUMB: return true;
		case eStatus::MARTYRS_SHIELD: return false;
		case eStatus::ASLEEP: return true;
		case eStatus::PARALYZED: return true;
		case eStatus::ACID: return true;
		case eStatus::FORCECAGE: return true;
		case eStatus::CHARM: return true;
	}
	return false;
}

enum class ePartyStatus {
	STEALTH,
	FLIGHT,
	DETECT_LIFE,
	FIREWALK,
};

enum class eMonstAbil {
	NO_ABIL,
	MISSILE,
	
	DAMAGE,
	STATUS,
	FIELD,
	PETRIFY,
	DRAIN_SP,
	DRAIN_XP,
	KILL,
	STEAL_FOOD,
	STEAL_GOLD,
	STUN,
	STATUS2,
	
	SPLITS,
	MARTYRS_SHIELD,
	ABSORB_SPELLS,
	MISSILE_WEB,
	RAY_HEAT,
	SPECIAL,
	DEATH_TRIGGER,
	
	RADIATE,
	SUMMON,
};

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
enum class eItemAbil {
	// Weapon abilities
	NONE = 0,
	DAMAGING_WEAPON = 1,
	SLAYER_WEAPON = 2,
	STATUS_WEAPON = 9,
	SOULSUCKER = 10,
	DRAIN_MISSILES = 11,
	WEAK_WEAPON = 12,
	CAUSES_FEAR = 13,
	// General abilities
	DAMAGE_PROTECTION = 30,
	FULL_PROTECTION = 31,
	STATUS_PROTECTION = 36,
	SKILL = 37,
	BOOST_STAT = 38,
	ACCURACY = 41,
	THIEVING = 42,
	GIANT_STRENGTH = 43,
	LIGHTER_OBJECT = 44,
	HEAVIER_OBJECT = 45,
	OCCASIONAL_STATUS = 46,
	LIFE_SAVING = 48,
	PROTECT_FROM_PETRIFY = 49,
	REGENERATE = 50,
	POISON_AUGMENT = 51,
	WILL = 53,
	FREE_ACTION = 54,
	SPEED = 55,
	SLOW_WEARER = 56,
	PROTECT_FROM_SPECIES = 57,
	// Nonspell Usable
	POISON_WEAPON = 70, //put poison on weapon
	AFFECT_STATUS = 71,
	CAST_SPELL = 72,
	BLISS = 84,
	AFFECT_EXPERIENCE = 85,
	AFFECT_SKILL_POINTS = 86,
	AFFECT_HEALTH = 87,
	AFFECT_SPELL_POINTS = 88,
	DOOM = 89,
	LIGHT = 90,
	STEALTH = 91,
	FIREWALK = 92,
	FLYING = 93,
	MAJOR_HEALING = 94,
	CALL_SPECIAL = 95,
	SUMMONING = 119,
	MASS_SUMMONING = 120,
	QUICKFIRE = 129,
	// Reagents
	HOLLY = 150, // Holly/Toadstool
	COMFREY = 151, // Comfrey Root
	NETTLE = 152, // Glowing Nettle
	WORMGRASS = 153, // Crypt Shroom/Wormgrass
	ASPTONGUE = 154, // Asptongue Mold
	EMBERF = 155, // Ember Flower
	GRAYMOLD = 156,
	MANDRAKE = 157,
	SAPPHIRE = 158,
	SMOKY_CRYSTAL = 159,
	RESURRECTION_BALM = 160,
	LOCKPICKS = 161,
	// Missile Abilities
	MISSILE_RETURNING = 170,
	MISSILE_LIGHTNING = 171,
	MISSILE_EXPLODING = 172,
	MISSILE_ACID = 173,
	MISSILE_SLAYER = 174,
	MISSILE_HEAL_TARGET = 176,
};

enum class eItemAbilCat {
	INVALID = -1,
	WEAPON, GENERAL, NONSPELL, SPELL, REAGENT, MISSILE
};

inline eItemAbilCat getItemAbilCategory(eItemAbil abil) {
	int code = (int) abil;
	if(code >= 0 && code <= 14)
		return eItemAbilCat::WEAPON;
	if(code >= 30 && code <= 62)
		return eItemAbilCat::GENERAL;
	if(code >= 70 && code <= 95)
		return eItemAbilCat::NONSPELL;
	if(code >= 110 && code <= 135)
		return eItemAbilCat::SPELL;
	if(code >= 150 && code <= 161)
		return eItemAbilCat::REAGENT;
	if(code >= 170 && code <= 176)
		return eItemAbilCat::MISSILE;
	return eItemAbilCat::INVALID;
}

/* adven[i].skills */ //complete
enum class eSkill {
	INVALID = -1,
	STRENGTH = 0,
	DEXTERITY = 1,
	INTELLIGENCE = 2,
	EDGED_WEAPONS = 3,
	BASHING_WEAPONS = 4,
	POLE_WEAPONS = 5,
	THROWN_MISSILES = 6,
	ARCHERY = 7,
	DEFENSE = 8,
	MAGE_SPELLS = 9,
	PRIEST_SPELLS = 10,
	MAGE_LORE = 11,
	ALCHEMY = 12,
	ITEM_LORE = 13,
	DISARM_TRAPS = 14,
	LOCKPICKING = 15,
	ASSASSINATION = 16,
	POISON = 17,
	LUCK = 18,
	MAX_HP = 19,
	MAX_SP = 20,
	// Magic values; only for check_party_stat()
	CUR_HP = 100,
	CUR_SP = 101,
	CUR_XP = 102,
	CUR_SKILL = 103,
	CUR_LEVEL = 104,
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
enum class eDamageType {
	WEAPON = 0,
	FIRE = 1,
	POISON = 2,
	MAGIC = 3,
	UNBLOCKABLE = 4,
	COLD = 5,
	UNDEAD = 6,
	DEMON = 7,
	MARKED = 10,
};

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
	INVALID = -1, // A magic value used while processing nodes
	NONE = 0,
	SET_SDF = 1,
	INC_SDF = 2,
	DISPLAY_MSG = 3,
	ENTER_SHOP = 4,
	DISPLAY_SM_MSG = 5,
	FLIP_SDF = 6,
	SDF_RANDOM = 7,
	SDF_ADD = 8,
	SDF_DIFF = 9,
	STORY_DIALOG = 10,
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
	DISPLAY_PICTURE = 24,
	REST = 25,
	WANDERING_WILL_FIGHT = 26,
	END_SCENARIO = 27,
	SET_POINTER = 28,
	SET_CAMP_FLAG = 29,
	PRINT_NUMS = 30, // For debugging
	SDF_TIMES = 31,
	SDF_DIVIDE = 32, // Computes both quotient and remainder
	SDF_POWER = 33,
	CHANGE_TER = 34,
	SWAP_TER = 35,
	TRANS_TER = 36,
	CLEAR_BUF = 37,
	APPEND_STRING = 38,
	APPEND_NUM = 39,
	APPEND_MONST = 40,
	APPEND_ITEM = 41,
	APPEND_TER = 42,
	PAUSE = 43,
	ONCE_GIVE_ITEM = 50,
	ONCE_GIVE_SPEC_ITEM = 51,
	ONCE_NULL = 52,
	ONCE_SET_SDF = 53,
	ONCE_DISPLAY_MSG = 54,
	ONCE_DIALOG = 55,
	UNUSED13 = 56,
	UNUSED14 = 57,
	ONCE_GIVE_ITEM_DIALOG = 58,
	UNUSED15 = 59,
	UNUSED16 = 60,
	ONCE_OUT_ENCOUNTER = 61,
	ONCE_TOWN_ENCOUNTER = 62,
	ONCE_TRAP = 63,
	SELECT_TARGET = 80,
	DAMAGE = 81,
	AFFECT_HP = 82,
	AFFECT_SP = 83,
	AFFECT_XP = 84,
	AFFECT_SKILL_PTS = 85,
	AFFECT_DEADNESS = 86,
	AFFECT_STATUS = 87,
	AFFECT_TRAITS = 88,
	AFFECT_AP = 89,
	AFFECT_NAME = 90,
	UNUSED30 = 91,
	UNUSED31 = 92,
	UNUSED32 = 93,
	UNUSED33 = 94,
	UNUSED34 = 95,
	UNUSED35 = 96,
	UNUSED36 = 97,
	AFFECT_STAT = 98,
	AFFECT_MAGE_SPELL = 99,
	AFFECT_PRIEST_SPELL = 100,
	AFFECT_GOLD = 101,
	AFFECT_FOOD = 102,
	AFFECT_ALCHEMY = 103,
	AFFECT_PARTY_STATUS = 104,
	CREATE_NEW_PC = 105,
	IF_SDF = 130,
	IF_TOWN_NUM = 131,
	IF_RANDOM = 132,
	IF_HAVE_SPECIAL_ITEM = 133,
	IF_SDF_COMPARE = 134,
	IF_TER_TYPE = 135,
	IF_ALIVE = 136,
	IF_HAS_GOLD = 137,
	IF_HAS_FOOD = 138,
	IF_ITEM_CLASS_ON_SPACE = 139,
	IF_HAVE_ITEM_CLASS = 140,
	IF_EQUIP_ITEM_CLASS = 141,
	IF_MAGE_SPELL = 142,
	IF_PRIEST_SPELL = 143,
	IF_RECIPE = 144,
	IF_STATUS = 145,
	UNUSED26 = 146,
	IF_DAY_REACHED = 147,
	IF_FIELDS = 148,
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
	TOWN_RUN_MISSILE = 171,
	TOWN_MONST_ATTACK = 172,
	TOWN_BOOM_SPACE = 173,
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
	TOWN_SET_CENTER = 198,
	TOWN_LIFT_FOG = 199,
	TOWN_START_TARGETING = 200,
	TOWN_SPELL_PAT_FIELD = 201,
	TOWN_SPELL_PAT_BOOM = 202,
	RECT_PLACE_FIELD = 210,
	RECT_SET_EXPLORED = 211,
	RECT_MOVE_ITEMS = 212,
	RECT_DESTROY_ITEMS = 213,
	RECT_CHANGE_TER = 214,
	RECT_SWAP_TER = 215,
	RECT_TRANS_TER = 216,
	RECT_LOCK = 217,
	RECT_UNLOCK = 218,
	OUT_MAKE_WANDER = 225,
	UNUSED20 = 226,
	OUT_PLACE_ENCOUNTER = 227,
	OUT_MOVE_PARTY = 228,
};

enum class eSpecCat {
	INVALID = -1,
	GENERAL, ONCE, AFFECT, IF_THEN, TOWN, RECT, OUTDOOR
};

inline eSpecCat getNodeCategory(eSpecType node) {
	int code = (int) node;
	if(code >= 0 && code <= 43)
		return eSpecCat::GENERAL;
	if(code >= 50 && code <= 63)
		return eSpecCat::ONCE;
	if(code >= 80 && code <= 105)
		return eSpecCat::AFFECT;
	if(code >= 130 && code <= 157)
		return eSpecCat::IF_THEN;
	if(code >= 170 && code <= 199)
		return eSpecCat::TOWN;
	if(code >= 210 && code <= 218)
		return eSpecCat::RECT;
	if(code >= 225 && code <= 228)
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
};

// MARK: eFieldType
// This is a slight misnomer, as a couple of these are not true fields.
enum eFieldType {
	SPECIAL_EXPLORED = 0,
	WALL_FORCE = 1,
	WALL_FIRE = 2,
	FIELD_ANTIMAGIC = 3,
	CLOUD_STINK = 4,
	WALL_ICE = 5,
	WALL_BLADES = 6,
	CLOUD_SLEEP = 7,
	// Begin fields saved in town setup
	OBJECT_BLOCK = 8,
	SPECIAL_SPOT = 9, // Space contains a white special spot
	FIELD_WEB = 10,
	OBJECT_CRATE = 11,
	OBJECT_BARREL = 12,
	BARRIER_FIRE = 13,
	BARRIER_FORCE = 14,
	FIELD_QUICKFIRE = 15,
	// End fields saved in town setup
	SFX_SMALL_BLOOD = 16,
	SFX_MEDIUM_BLOOD = 17,
	SFX_LARGE_BLOOD = 18,
	SFX_SMALL_SLIME = 19,
	SFX_LARGE_SLIME = 20,
	SFX_ASH = 21,
	SFX_BONES = 22,
	SFX_RUBBLE = 23,
	BARRIER_CAGE = 24,
	// From here on are special values that don't index anything.
	// Thus, they start at 32.
	FIELD_DISPEL = 32, // Dispel field
	FIELD_SMASH = 33, // Move Mountains
	// Mustn't have anything >= 50
};

// Field types are used to index bit fields.
// In this case, their values should be taken as a bit index that is set.
inline unsigned long operator&(unsigned long a, eFieldType b) {
	return a & (1 << b);
}
inline unsigned long operator&(eFieldType a, unsigned long b) {
	return (1 << a) & b;
}
inline unsigned long operator&(eFieldType a, eFieldType b) {
	return (1 << a) & (1 << b);
}
inline unsigned long& operator &=(unsigned long& a, eFieldType b) {
	a = a & b;
	return a;
}
inline unsigned long operator|(unsigned long a, eFieldType b) {
	return a | (1 << b);
}
inline unsigned long operator|(eFieldType a, unsigned long b) {
	return (1 << a) | b;
}
inline unsigned long operator|(eFieldType a, eFieldType b) {
	return (1 << a) | (1 << b);
}
inline unsigned long& operator |=(unsigned long& a, eFieldType b) {
	a = a | b;
	return a;
}
inline unsigned long operator^(unsigned long a, eFieldType b) {
	return a ^ (1 << b);
}
inline unsigned long operator^(eFieldType a, unsigned long b) {
	return (1 << a) ^ b;
}
inline unsigned long operator^(eFieldType a, eFieldType b) {
	return (1 << a) ^ (1 << b);
}
inline unsigned long& operator ^=(unsigned long& a, eFieldType b) {
	a = a ^ b;
	return a;
}
inline unsigned long operator>>(eFieldType a, unsigned long b) {
	return (1 << a) >> b;
}
inline unsigned long operator<<(eFieldType a, unsigned long b) {
	return (1 << a) << b;
}
inline unsigned long operator~(eFieldType f) {
	return ~(1 << f);
}

enum class eSpell {
	NONE = -1,
	// Mage spells
	LIGHT = 0,
	SPARK = 1,
	HASTE_MINOR = 2,
	STRENGTH = 3,
	SCARE = 4,
	CLOUD_FLAME = 5,
	IDENTIFY = 6,
	SCRY_MONSTER = 7,
	GOO = 8,
	TRUE_SIGHT = 9,
	POISON_MINOR = 10,
	FLAME = 11,
	SLOW = 12,
	DUMBFOUND = 13,
	ENVENOM = 14,
	CLOUD_STINK = 15,
	SUMMON_BEAST = 16,
	CONFLAGRATION = 17,
	DISPEL_SQUARE = 18,
	CLOUD_SLEEP = 19,
	UNLOCK = 20,
	HASTE = 21,
	FIREBALL = 22,
	LIGHT_LONG = 23,
	FEAR = 24,
	WALL_FORCE = 25,
	SUMMON_WEAK = 26,
	ARROWS_FLAME = 27,
	WEB = 28,
	RESIST_MAGIC = 29,
	POISON = 30,
	ICE_BOLT = 31,
	SLOW_GROUP = 32,
	MAGIC_MAP = 33,
	CAPTURE_SOUL = 34,
	SIMULACRUM = 35,
	ARROWS_VENOM = 36,
	WALL_ICE = 37,
	STEALTH = 38,
	HASTE_MAJOR = 39,
	FIRESTORM = 40,
	DISPEL_BARRIER = 41,
	BARRIER_FIRE = 42,
	SUMMON = 43,
	SHOCKSTORM = 44,
	SPRAY_FIELDS = 45,
	POISON_MAJOR = 46,
	FEAR_GROUP = 47,
	KILL = 48,
	PARALYZE = 49,
	DEMON = 50,
	ANTIMAGIC = 51,
	MINDDUEL = 52,
	FLIGHT = 53,
	SHOCKWAVE = 54,
	BLESS_MAJOR = 55,
	PARALYSIS_MASS = 56,
	PROTECTION = 57,
	SUMMON_MAJOR = 58,
	BARRIER_FORCE = 59,
	QUICKFIRE = 60,
	ARROWS_DEATH = 61,
	// Special spells
	STRENGTHEN_TARGET = 62,
	SUMMON_RAT = 63,
	WALL_ICE_BALL = 64,
	GOO_BOMB = 65,
	FOUL_VAPOR = 66,
	CLOUD_SLEEP_LARGE = 67,
	ACID_SPRAY = 68,
	PARALYZE_BEAM = 69,
	SLEEP_MASS = 70,
	RAVAGE_ENEMIES = 71,
	BLADE_AURA = 82,
	// Priest spells
	BLESS_MINOR = 100,
	HEAL_MINOR = 101,
	POISON_WEAKEN = 102,
	TURN_UNDEAD = 103,
	LOCATION = 104,
	SANCTUARY = 105,
	SYMBIOSIS = 106,
	MANNA_MINOR = 107,
	RITUAL_SANCTIFY = 108,
	STUMBLE = 109,
	BLESS = 110,
	POISON_CURE = 111,
	CURSE = 112,
	LIGHT_DIVINE = 113,
	WOUND = 114,
	SUMMON_SPIRIT = 115,
	MOVE_MOUNTAINS = 116,
	CHARM_FOE = 117,
	DISEASE = 118,
	AWAKEN = 119,
	HEAL = 120,
	HEAL_ALL_LIGHT = 121,
	HOLY_SCOURGE = 122,
	DETECT_LIFE = 123,
	PARALYSIS_CURE = 124,
	MANNA = 125,
	FORCEFIELD = 126,
	DISEASE_CURE = 127,
	RESTORE_MIND = 128,
	SMITE = 129,
	POISON_CURE_ALL = 130,
	CURSE_ALL = 131,
	DISPEL_UNDEAD = 132,
	CURSE_REMOVE = 133,
	STICKS_TO_SNAKES = 134,
	MARTYRS_SHIELD = 135,
	CLEANSE = 136,
	FIREWALK = 137,
	BLESS_PARTY = 138,
	HEAL_MAJOR = 139,
	RAISE_DEAD = 140,
	FLAMESTRIKE = 141,
	SANCTUARY_MASS = 142,
	SUMMON_HOST = 143,
	SHATTER = 144,
	DISPEL_SPHERE = 145,
	HEAL_ALL = 146,
	REVIVE = 147,
	HYPERACTIVITY = 148,
	DESTONE = 149,
	SUMMON_GUARDIAN = 150,
	CHARM_MASS = 151,
	PROTECTIVE_CIRCLE = 152,
	PESTILENCE = 153,
	REVIVE_ALL = 154,
	RAVAGE_SPIRIT = 155,
	RESURRECT = 156,
	DIVINE_THUD = 157,
	AVATAR = 158,
	WALL_BLADES = 159,
	WORD_RECALL = 160,
	CLEANSE_MAJOR = 161,
	// Special spells
	DISPEL_FIELD = 162,
	MOVE_MOUNTAINS_MASS = 163,
	WRACK = 164,
	UNHOLY_RAVAGING = 165,
};

inline bool isMage(eSpell spell) {
	int code = (int) spell;
	return code >= 0 && code < 62;
}

inline bool isPriest(eSpell spell) {
	int code = (int) spell;
	return code >= 100 && code < 162;
}

enum class eShopType {
	ITEMS = 0,
	MAGE = 1,
	PRIEST = 2,
	ALCHEMY = 3,
	HEALING = 4,
	MAGIC_JUNK = 5,
	MAGIC_LOUSY = 6,
	MAGIC_SO_SO = 7,
	MAGIC_GOOD = 8,
	MAGIC_GREAT = 9,
	FOOD = 10,
};

#endif
