/*
 *  consts.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 13/04/09.
 *
 */

#ifndef BOE_GAME_CONSTS_H
#define BOE_GAME_CONSTS_H

#include <set>

/* overall mode; some seem to be missing */
enum eGameMode {
	MODE_OUTDOORS = 0,
	MODE_TOWN = 1,
	MODE_TALK_TOWN = 2, // looking for someone to talk
	MODE_TOWN_TARGET = 3, // spell target, that is
	MODE_USE_TOWN = 4,
	MODE_DROP_TOWN = 5,
	MODE_BASH_TOWN = 6, // unused
	MODE_COMBAT = 10,
	MODE_SPELL_TARGET = 11,
	MODE_FIRING = 12, // firing from bow or crossbow
	MODE_THROWING = 13, // throwing missile
	MODE_FANCY_TARGET = 14, // spell target, that is; I think it's for multitarget spells
	MODE_DROP_COMBAT = 15,
	MODE_TALKING = 20,
	MODE_SHOPPING = 21,
	MODE_LOOK_OUTDOORS = 35, // looking at something
	MODE_LOOK_TOWN = 36,
	MODE_LOOK_COMBAT = 37,
	MODE_STARTUP = 45,
	MODE_RESTING = 50,
	MODE_CUTSCENE = 51, // for future use
};

static const std::set<eGameMode> scrollableModes = {
	MODE_SPELL_TARGET,
	MODE_FIRING,
	MODE_THROWING,
	MODE_FANCY_TARGET,
	MODE_LOOK_COMBAT,
	MODE_LOOK_TOWN
};

enum eStatMode {
	MODE_INVEN = 0,
	MODE_SHOP = 1,
	MODE_IDENTIFY = 2,
	MODE_SELL_WEAP = 3,
	MODE_SELL_ARMOR = 4,
	MODE_SELL_ANY = 5,
	MODE_ENCHANT = 6,
};

static const signed char dir_x_dif[9] = {0,1,1,1,0,-1,-1,-1,0};
static const signed char dir_y_dif[9] = {-1,-1,0,1,1,1,0,-1,0};

/* trap type */
/* used in run_trap(...) */
enum eTrapType {
	TRAP_RANDOM = 0,
	TRAP_BLADE = 1,
	TRAP_DART = 2,
	TRAP_GAS = 3, // poisons all
	TRAP_EXPLOSION = 4, // damages all => uses c_town.difficulty rather than trap_level to calculates damages (and even c_town.difficulty /13).
	TRAP_SLEEP_RAY = 5, // TODO: Rename "Paralysis ray"
	TRAP_FALSE_ALARM = 6,
	TRAP_DRAIN_XP = 7,
	TRAP_ALERT = 8, // makes town hostile
	TRAP_FLAMES = 9, // damages all => uses trap_level (*5) to calculates damages.
	TRAP_DUMBFOUND = 10, //dumbfound all
	TRAP_DISEASE = 11,
	TRAP_DISEASE_ALL = 12,
	TRAP_CUSTOM = 13,
};

// Startup button rects
const int STARTBTN_LOAD = 0;
const int STARTBTN_NEW = 1;
const int STARTBTN_ORDER = 2;
const int STARTBTN_JOIN = 3;
const int STARTBTN_CUSTOM = 4;

// Shop rects
const int SHOPRECT_WHOLE_AREA = 0;
const int SHOPRECT_ACTIVE_AREA = 1;
const int SHOPRECT_GRAPHIC = 2;
const int SHOPRECT_ITEM_NAME = 3;
const int SHOPRECT_ITEM_COST = 4;
const int SHOPRECT_ITEM_EXTRA = 5;
const int SHOPRECT_ITEM_HELP = 6;

// Item button rects
const int ITEMBTN_NAME = 0;
const int ITEMBTN_USE = 1;
const int ITEMBTN_GIVE = 2;
const int ITEMBTN_DROP = 3;
const int ITEMBTN_INFO = 4;
const int ITEMBTN_SPEC = 5; // Sell, Identify, or Enchant
const int ITEMBTN_ALL = 10; // use, give, drop, info
const int ITEMBTN_NORM = 11; // give, drop, info

// PC button rects
const int PCBTN_NAME = 0;
const int PCBTN_HP = 1;
const int PCBTN_SP = 2;
const int PCBTN_INFO = 3;
const int PCBTN_TRADE = 4;

// Item window modes (non-PC)
const int ITEM_WIN_SPECIAL = 6;
const int ITEM_WIN_QUESTS = 7;

#endif
