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

// Startup buttons
enum eStartButton {
	STARTBTN_LOAD = 0,
	STARTBTN_NEW = 1,
	STARTBTN_ORDER = 2,
	STARTBTN_JOIN = 3,
	STARTBTN_CUSTOM = 4,
	STARTBTN_SCROLL = 5,
	MAX_eStartButton // keep last
};

// Shop rects
enum eShopArea {
	SHOPRECT_WHOLE_AREA = 0,
	SHOPRECT_ACTIVE_AREA = 1,
	SHOPRECT_GRAPHIC = 2,
	SHOPRECT_ITEM_NAME = 3,
	SHOPRECT_ITEM_COST = 4,
	SHOPRECT_ITEM_EXTRA = 5,
	SHOPRECT_ITEM_HELP = 6,
	MAX_eShopArea // keep last
};

// Item buttons
enum eItemButton {
	ITEMBTN_ICON = 0,
	ITEMBTN_NAME = 1,
	ITEMBTN_USE = 2,
	ITEMBTN_GIVE = 3,
	ITEMBTN_DROP = 4,
	ITEMBTN_INFO = 5,
	ITEMBTN_SPEC = 6, // Sell, Identify, or Enchant
	MAX_eItemButton // keep last
};

const int ITEMBTN_ALL = 10; // use, give, drop, info
const int ITEMBTN_NORM = 11; // give, drop, info

// PC buttons
enum ePlayerButton {
	PCBTN_NAME = 0,
	PCBTN_HP = 1,
	PCBTN_SP = 2,
	PCBTN_INFO = 3,
	PCBTN_TRADE = 4,
	MAX_ePlayerButton // keep last
};

// Item window modes (non-PC)
enum eItemWinMode {
	ITEM_WIN_PC1 = 0,
	ITEM_WIN_PC2 = 1,
	ITEM_WIN_PC3 = 2,
	ITEM_WIN_PC4 = 3,
	ITEM_WIN_PC5 = 4,
	ITEM_WIN_PC6 = 5,
	ITEM_WIN_SPECIAL = 6,
	ITEM_WIN_QUESTS = 7,
};

// Gobal window rects
enum eGuiArea {
	WINRECT_TERVIEW = 0,
	WINRECT_ACTBTNS = 1,
	WINRECT_PCSTATS = 2,
	WINRECT_INVEN = 3,
	WINRECT_STATUS = 4,
	WINRECT_TRANSCRIPT = 5,
	MAX_eGuiArea // keep last
};

// height of the menubar in pixels
const int MENUBAR_HEIGHT = 20;

// amount of player-castable spells
const int NUM_MAGE_SPELLS = 62;
const int NUM_PRIEST_SPELLS = 62;

// spell levels
const int MAGE_SPELL_LEVEL_MAX = 7;
const int PRIEST_SPELL_LEVEL_MAX = 7;

#endif
