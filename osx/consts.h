/*
 *  consts.h
 *  BoE
 *
 *  Created by Seanachi Clappison Dillon on 13/04/09.
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
#define SDF_IS_PARTY_SPLIT	304][0
#define SDF_PARTY_SPLIT_X	304][1
#define SDF_PARTY_SPLIT_Y	304][2
#define SDF_NO_INSTANT_HELP	306][4 // boolean

/* overall mode; some seem to be missing */
#define MODE_OUTDOORS		0
#define MODE_TOWN			1
#define MODE_TALK_TOWN		2 // looking for someone to talk
#define MODE_TOWN_TARGET	3 // spell target, that is
#define MODE_USE			4
#define MODE_COMBAT			10
#define MODE_SPELL_TARGET	11
#define MODE_FIRING			12 // firing from bow or crossbow
#define MODE_THROWING		13 // throwing missle
#define MODE_FANCY_TARGET	14 // spell target, that is
#define MODE_DROPPING		15
#define MODE_TALKING		20
#define MODE_SHOPPING		21
#define MODE_LOOK_OUTDOORS	35 // looking at something
#define MODE_LOOK_TOWN		36
#define MODE_LOOK_COMBAT	37
#define MODE_STARTUP		45
#define MODE_REDRAW			50

/* adven[i].main_status */ //complete
#define MAIN_STATUS_ABSENT	0 // absent, empty slot
#define MAIN_STATUS_ALIVE	1
#define MAIN_STATUS_DEAD	2
#define MAIN_STATUS_DUST	3
#define MAIN_STATUS_STONE	4
#define MAIN_STATUS_FLED	5
#define MAIN_STATUS_SURFACE	6 // fled to surface?
#define MAIN_STATUS_WON		7
/* main status modifiers */
#define MAIN_STATUS_SPLIT	10 // split from party


/* adven[i].skills */ //complete
#define SKILL_STRENGTH			0
#define SKILL_DEXTERITY			1
#define SKILL_INTELLIGENCE		2
#define SKILL_EDGED_WEAPONS		3
#define SKILL_BASHING_WEAPONS	4
#define SKILL_POLE_WEAPONS		5
#define SKILL_THROWN_MISSILES	6
#define SKILL_ARCHERY			7
#define SKILL_DEFENSE			8
#define SKILL_MAGE_SPELLS		9
#define SKILL_PRIEST_SPELLS		10
#define SKILL_MAGE_LORE			11
#define SKILL_ALCHEMY			12
#define SKILL_ITEM_LORE			13
#define SKILL_DISARM_TRAPS		14
#define SKILL_LOCKPICKING		15
#define SKILL_ASSASSINATION		16
#define SKILL_POISON			17
#define SKILL_LUCK				18


/* adven[i].traits */ //complete
#define TRAIT_TOUGHNESS			0
#define TRAIT_MAGICALLY_APT		1
#define TRAIT_AMBIDEXTROUS		2
#define TRAIT_NIMBLE			3
#define TRAIT_CAVE_LORE			4
#define TRAIT_WOODSMAN			5
#define TRAIT_GOOD_CONST		6
#define TRAIT_HIGHLY_ALERT		7
#define TRAIT_STRENGTH			8
#define TRAIT_RECUPERATION		9
#define TRAIT_SLUGGISH			10
#define TRAIT_MAGICALLY_INEPT	11
#define TRAIT_FRAIL				12
#define TRAIT_CHRONIC_DISEASE	13
#define TRAIT_BAD_BACK			14

/* adven[i].race */ //complete
#define RACE_HUMAN			0
#define RACE_NEPHIL			1
#define RACE_SLITH			2

/* adven[i].status*/ //complete - assign a positive value for a help pc effect, a negative for harm pc
#define STATUS_POISONED_WEAPON	0
#define STATUS_BLESS			1
#define STATUS_POISON			2
#define STATUS_HASTE			3
#define STATUS_INVULNERABLE		4
#define STATUS_MAGIC_RESISTANCE	5
#define STATUS_WEBS				6
#define STATUS_DISEASE			7
#define STATUS_INVISIBLE		8 //sanctuary
#define STATUS_DUMB				9
#define STATUS_MARTYRS_SHIELD	10
#define STATUS_ASLEEP			11
#define STATUS_PARALYZED		12
#define STATUS_ACID				13

/* damage type*/
/* used as parameter to some functions */
#define DAMAGE_WEAPON			0
#define DAMAGE_FIRE				1
#define DAMAGE_POISON			2
#define DAMAGE_MAGIC			3
#define DAMAGE_UNBLOCKABLE		4 //from the source files - the display is the same as the magic one (damage_monst in SPECIALS.cpp)
#define DAMAGE_COLD				5
#define DAMAGE_UNDEAD			6 //from the source files - the display is the same as the weapon one
#define DAMAGE_DEMON			7 //from the source files - the display is the same as the weapon one
// 8 and 9 aren't defined : doesn't print any damage. According to the source files the 9 is DAMAGE_MARKED though. Wrong ?
#define DAMAGE_MARKED			10 // usage: DAMAGE_MARKED + damage_type
#define DAMAGE_NO_PRINT			30 // usage: DAMAGE_NO_PRINT + damage_type

/* trap type */
/* used in pc_record_type::runTrap(...) */
#define TRAP_RANDOM				0
#define TRAP_BLADE				1
#define TRAP_DART				2
#define TRAP_GAS				3 // poisons all
#define TRAP_EXPLOSION			4 // damages all => uses c_town.difficulty rather than trap_level to calculates damages (and even c_town.difficulty /13).
#define TRAP_SLEEP_RAY			5
#define TRAP_FALSE_ALARM		6
#define TRAP_DRAIN_XP			7
#define TRAP_ALERT				8 // makes town hostile
#define TRAP_FLAMES				9 // damages all => uses trap_level (*5) to calculates damages.
#define TRAP_DUMBFOUND			10 //dumbfound all
#define TRAP_DISEASE			11
#define TRAP_DISEASE_ALL		12

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