/*
 *  consts.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 13/04/09.
 *
 */

#ifndef _CONSTS_H
#define _CONSTS_H

/*
 This file contain numerous constans in form of #defines.
 Almost all of these constants cannot be changed because
 that would make the game work improperly.
 */

#define NUM_OF_PCS			6
#define INVALID_PC			NUM_OF_PCS
#define INVALID_TOWN		200

#define NUM_OF_BOATS		30
#define NUM_OF_HORSES		30

#define SFX_SMALL_BLOOD		1
#define SFX_MEDIUM_BLOOD	2
#define SFX_LARGE_BLOOD		4
#define SFX_SMALL_SLIME		8
#define SFX_BIG_SLIME		16
#define SFX_ASH				32
#define SFX_BONES			64
#define SFX_RUBBLE			128

/* stuff done flags */
#define SDF_IS_PARTY_SPLIT		304][0
#define SDF_PARTY_SPLIT_X		304][1
#define SDF_PARTY_SPLIT_Y		304][2
#define SDF_PARTY_SPLIT_PC		304][3
#define SDF_PARTY_SPLIT_TOWN	304][4 // for future use, hopefully
#define SDF_PARTY_STEALTHY		305][0
#define SDF_PARTY_FLIGHT		305][1
#define SDF_PARTY_DETECT_LIFE	305][2
#define SDF_PARTY_FIREWALK		305][3
#define SDF_SKIP_STARTUP		305][4 // preferably deprecated
#define SDF_LESS_SOUND			305][5
#define SDF_NO_TARGET_LINE		305][6
#define SDF_RESURRECT_NO_BALM	305][8
#define SDF_HOSTILES_PRESENT	305][9
#define SDF_NO_MAPS				306][0
#define SDF_NO_SOUNDS			306][1
#define SDF_NO_FRILLS			306][2
#define SDF_ROOM_DESCS_AGAIN	306][3
#define SDF_NO_INSTANT_HELP		306][4 // boolean
#define SDF_NO_SHORE_FRILLS		306][5
#define SDF_GAME_SPEED			306][6
#define SDF_EASY_MODE			306][7
#define SDF_LESS_WANDER_ENC		306][8
#define SDF_NO_TER_ANIM			306][9
/*
 [305]
 [4] will be Skip startup screen
 [6] is No targeting line (use if getting crashes)
 [5] is Fewer sounds (use if getting crashes)
 */

/* overall mode; some seem to be missing */
enum eGameMode {
	MODE_OUTDOORS = 0,
	MODE_TOWN = 1,
	MODE_TALK_TOWN = 2, // looking for someone to talk
	MODE_TOWN_TARGET = 3, // spell target, that is
	MODE_USE_TOWN = 4,
	MODE_DROP_TOWN = 5,
	MODE_COMBAT = 10,
	MODE_SPELL_TARGET = 11,
	MODE_FIRING = 12, // firing from bow or crossbow
	MODE_THROWING = 13, // throwing missle
	MODE_FANCY_TARGET = 14, // spell target, that is; I think it's for multitarget spells
	MODE_DROP_COMBAT = 15,
	MODE_TALKING = 20,
	MODE_SHOPPING = 21,
	MODE_LOOK_OUTDOORS = 35, // looking at something
	MODE_LOOK_TOWN = 36,
	MODE_LOOK_COMBAT = 37,
	MODE_STARTUP = 45,
	MODE_RESTING = 50,
};

///* adven[i].main_status */ //complete
//#define MAIN_STATUS_ABSENT	0 // absent, empty slot
//#define MAIN_STATUS_ALIVE	1
//#define MAIN_STATUS_DEAD	2
//#define MAIN_STATUS_DUST	3
//#define MAIN_STATUS_STONE	4
//#define MAIN_STATUS_FLED	5
//#define MAIN_STATUS_SURFACE	6 // fled to surface?
//#define MAIN_STATUS_WON		7
///* main status modifiers */
//#define MAIN_STATUS_SPLIT	10 // split from party

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
#ifndef DIR_ARRAY_DEF
extern signed char dir_x_dif[9];
extern signed char dir_y_dif[9];
#endif
inline eDirection& operator++ (eDirection& me,int){
	if(me == DIR_HERE) return me = DIR_N;
	else return me = (eDirection) (1 + (int) me);
}

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
//inline eDamageType operator + (eDamageType lhs, eDamageType rhs){
//	if(lhs == DAMAGE_MARKED || lhs == DAMAGE_NO_PRINT){
//		if(rhs != DAMAGE_MARKED && rhs != DAMAGE_NO_PRINT)
//			return (eDamageType) ((int)lhs + (int)rhs);
//	}else if(rhs == DAMAGE_MARKED || rhs == DAMAGE_NO_PRINT)
//		return (eDamageType) ((int)lhs + (int)rhs);
//	else{
//		int a = lhs, b = rhs, c = 0;
//		if(a > 30){
//			c += 30;
//			a -= 30;
//			if(b > 30) b -= 30;
//			else if(b > 10) b -= 10;
//			if(a == b) c += a;
//			else c += 4;
//		}else if(b > 30){
//			c += 30;
//			b -= 30;
//			if(a > 30) a -= 30;
//			else if(a > 10) a -= 10;
//			if(a == b) c += a;
//			else c += 4;
//		}else if(a > 10){
//			c += 10;
//			a -= 10;
//			if(b > 30){
//				b -= 30;
//				c += 20;
//			}else if(b > 10) b -= 10;
//			if(a == b) c += a;
//			else c += 4;
//		}else if(b > 10){
//			c += 10;
//			b -= 10;
//			if(a > 30){
//				a -= 30;
//				c += 20;
//			}else if(a > 10) a -= 10;
//			if(a == b) c += a;
//			else c += 4;
//		}
//		return (eDamageType) c; // this SHOULD guarantee a valid result...
//	}
//}

/* trap type */
/* used in pc_record_type::runTrap(...) */
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
};

//stuff used in blxactions.c

//Startup button rects (also used in startup.c)
#define STARTBTN_LOAD		0
#define STARTBTN_NEW		1
#define STARTBTN_ORDER		2
#define STARTBTN_JOIN		3
#define STARTBTN_CUSTOM		4

//Shop rects
#define SHOPRECT_WHOLE_AREA		0
#define SHOPRECT_ACTIVE_AREA	1
#define SHOPRECT_GRAPHIC		2
#define SHOPRECT_ITEM_NAME		3
#define SHOPRECT_ITEM_COST		4
#define SHOPRECT_ITEM_EXTRA		5
#define SHOPRECT_ITEM_HELP		6

// Item button rects
#define ITEMBTN_NAME		0
#define ITEMBTN_USE			1
#define ITEMBTN_GIVE		2
#define ITEMBTN_DROP		3
#define ITEMBTN_INFO		4
#define ITEMBTN_SPEC		5 // Sell, Identify, or Enchant

// PC button rects
#define PCBTN_NAME			0
#define PCBTN_HP			1
#define PCBTN_SP			2
#define PCBTN_INFO			3
#define PCBTN_TRADE			4

//Spell select
#define SPELL_SELECT_NONE		0
#define SPELL_SELECT_ACTIVE		1
#define SPELL_SELECT_ANY		2

//Spell refer
#define SPELL_REFER				0
#define SPELL_IMMED				1
#define SPELL_TARGET			2
#define SPELL_FANCY_TARGET		3

#endif
