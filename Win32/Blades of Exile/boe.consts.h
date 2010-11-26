#ifndef _CONSTS_H
	#define _CONSTS_H

/*
	This file contain numerous constans in form of #defines.
	Almost all of this constants cannot be changed because
	that would make the game work improperly.
*/

#define NUM_OF_PCS		6
#define INVALID_PC		NUM_OF_PCS

#define INVALID_TOWN	200

#define NUM_OF_BOATS	30
#define NUM_OF_HORSES	30

#define SFX_SMALL_BLOOD		1
#define SFX_MEDIUM_BLOOD	2
#define SFX_LARGE_BLOOD		4
#define SFX_SMALL_SLIME 	8
#define SFX_BIG_SLIME 		16
#define SFX_ASH 			32
#define SFX_BONES 			64
#define SFX_RUBBLE 			128

/* overall mode; some seem to be missing */
 #define MODE_OUTDOORS 0
 #define MODE_TOWN 1
 #define MODE_TALK_TOWN 2 // looking for someone to talk
 #define MODE_TOWN_TARGET 3 // spell target, that is
 #define MODE_USE 4
 #define MODE_COMBAT 10
 #define MODE_SPELL_TARGET 11
 #define MODE_FIRING 12 // firing from bow or crossbow
 #define MODE_THROWING 13 // throwing missile
 #define MODE_FANCY_TARGET 14 // spell target, that is
 #define MODE_DROPPING 15
 #define MODE_TALKING 20
 #define MODE_SHOPPING 21
 #define MODE_LOOK_OUTDOORS 35 // looking at something
 #define MODE_LOOK_TOWN 36
 #define MODE_LOOK_COMBAT 37
 #define MODE_STARTUP 45
 #define MODE_REDRAW 50
 #define MODE_CUTSCENE 51

/* adven[i].main_status */ //complete
#define MAIN_STATUS_ABSENT 0		// absent, empty slot
#define MAIN_STATUS_ALIVE 1
#define MAIN_STATUS_DEAD 2
#define MAIN_STATUS_DUST 3
#define MAIN_STATUS_STONE 4
#define MAIN_STATUS_FLED 5
#define MAIN_STATUS_SURFACE 6		// fled to surface?
#define MAIN_STATUS_WON 7
/* main status modifiers */
#define MAIN_STATUS_SPLIT 10		// split from party, added to previous status (e.g 11 is splitted and alive : waiting for the active pc to return).


/* adven[i].skills */     //complete
#define SKILL_STRENGTH 0
#define SKILL_DEXTERITY 1
#define SKILL_INTELLIGENCE 2
#define SKILL_EDGED_WEAPONS 3
#define SKILL_BASHING_WEAPONS 4
#define SKILL_POLE_WEAPONS 5
#define SKILL_THROWN_MISSILES 6
#define SKILL_ARCHERY 7
#define SKILL_DEFENSE 8
#define SKILL_MAGE_SPELLS 9
#define SKILL_PRIEST_SPELLS 10
#define SKILL_MAGE_LORE 11
#define SKILL_ALCHEMY 12
#define SKILL_ITEM_LORE 13
#define SKILL_DISARM_TRAPS 14
#define SKILL_LOCKPICKING 15
#define SKILL_ASSASSINATION 16
#define SKILL_POISON 17
#define SKILL_LUCK 18


/* adven[i].traits */      //complete
#define TRAIT_TOUGHNESS 0
#define TRAIT_MAGICALLY_APT 1
#define TRAIT_AMBIDEXTROUS 2
#define TRAIT_NIMBLE 3
#define TRAIT_CAVE_LORE 4
#define TRAIT_WOODSMAN 5
#define TRAIT_GOOD_CONST 6
#define TRAIT_HIGHLY_ALERT 7
#define TRAIT_EXCEPTIONAL_STRENGTH 8
#define TRAIT_RECUPERATION 9
#define TRAIT_SLUGGISH 10
#define TRAIT_MAGICALLY_INEPT 11
#define TRAIT_FRAIL 12
#define TRAIT_CHRONIC_DISEASE 13
#define TRAIT_BAD_BACK 14

/* adven[i].race */     //complete
#define RACE_HUMAN 0
#define RACE_NEPHIL 1
#define RACE_SLITH 2

/* adven[i].status*/      //complete - assign a positive value for a help pc effect, a negative for harm pc
#define STATUS_POISONED_WEAPON 0
#define STATUS_BLESS 1
#define STATUS_POISON 2
#define STATUS_HASTE 3
#define STATUS_INVULNERABLE 4
#define STATUS_MAGIC_RESISTANCE 5
#define STATUS_WEBS 6
#define STATUS_DISEASE 7
#define STATUS_INVISIBLE 8      //sanctuary
#define STATUS_DUMB	9
#define STATUS_MARTYRS_SHIELD 10
#define STATUS_ASLEEP 11
#define STATUS_PARALYZED 12
#define STATUS_ACID 13

/* damage type*/
/* used as parameter to some functions */
#define DAMAGE_WEAPON 0
#define DAMAGE_FIRE 1
#define DAMAGE_POISON 2
#define DAMAGE_MAGIC 3
#define DAMAGE_UNBLOCKABLE 4 //from the source files - the display is the same as the magic one (damage_monst in SPECIALS.cpp or pc_record_type::runTrap in PARTY.cpp)
#define DAMAGE_COLD 5
#define DAMAGE_UNDEAD 6 //from the source files - the display is the same as the weapon one
#define DAMAGE_DEMON 7 //from the source files - the display is the same as the weapon one
// 8 and 9 aren't defined : doesn't print any damage. According to the source files the 9 is DAMAGE_MARKED though. Wrong ?
#define DAMAGE_MARKED 10	// usage: DAMAGE_MARKED + damage_type
#define DAMAGE_NO_PRINT 30	// usage: DAMAGE_NO_PRINT + damage_type

/* trap type */
/* used in pc_record_type::runTrap(...) */
#define TRAP_RANDOM 0
#define TRAP_BLADE 1
#define TRAP_DART 2
#define TRAP_GAS 3			// poisons all
#define TRAP_EXPLOSION 4	// damages all => uses c_town.difficulty rather than trap_level to calculates damages (and even c_town.difficulty /13).
#define TRAP_SLEEP_RAY 5
#define TRAP_FALSE_ALARM 6
#define TRAP_DRAIN_XP 7
#define TRAP_ALERT 8		// makes town hostile
#define TRAP_FLAMES 9		// damages all => uses trap_level (*5) to calculates damages.
#define TRAP_DUMBFOUND 10   //dumbfound all
#define TRAP_DISEASE 11
#define TRAP_DISEASE_ALL 12

/*      items[i].type    a.k.a type of weapon         */
#define ITEM_EDGED 1
#define ITEM_BASHING 2
#define ITEM_POLE 3

/*      items[i].variety    a.k.a item type (in editor)      */
#define ITEM_TYPE_NO_ITEM 0     //a guess, i can't test it ; but should be accurate      => confirmed by the code
#define ITEM_TYPE_ONE_HANDED 1
#define ITEM_TYPE_TWO_HANDED 2
#define ITEM_TYPE_GOLD 3     //a guess, i can't test it ; but should be accurate     => confirmed by the code
#define ITEM_TYPE_BOW 4
#define ITEM_TYPE_ARROW 5
#define ITEM_TYPE_THROWN_MISSILE 6
#define ITEM_TYPE_POTION 7       // potion/magic item
#define ITEM_TYPE_SCROLL 8       // scroll/magic item
#define ITEM_TYPE_WAND 9
#define ITEM_TYPE_TOOL 10
#define ITEM_TYPE_FOOD 11        //a guess, i can't test it ; but should be accurate      => confirmed by the code
#define ITEM_TYPE_SHIELD 12
#define ITEM_TYPE_ARMOR 13
#define ITEM_TYPE_HELM 14
#define ITEM_TYPE_GLOVES 15
#define ITEM_TYPE_SHIELD_2 16    //don't know why a second type of shield is used ; it is actually checked in the armor code (item >= 12 and <= 17)
#define ITEM_TYPE_BOOTS 17       //(continued) and you can't equip another (12) shield while wearing it ... I didn't find a single item with this property in the bladbase.exs ...
#define ITEM_TYPE_RING 18
#define ITEM_TYPE_NECKLACE 19
#define ITEM_TYPE_WEAPON_POISON 20
#define ITEM_TYPE_NON_USE_OBJECT 21
#define ITEM_TYPE_PANTS 22
#define ITEM_TYPE_CROSSBOW 23
#define ITEM_TYPE_BOLTS 24
#define ITEM_TYPE_MISSILE_NO_AMMO 25       //e.g slings

/*      items[i].ability      */

/* Weapons Ability */
#define ITEM_NO_ABILITY 0
#define ITEM_FLAMING_WEAPON 1
#define ITEM_DEMON_SLAYER 2
#define ITEM_UNDEAD_SLAYER 3
#define ITEM_LIZARD_SLAYER 4
#define ITEM_GIANT_SLAYER 5
#define ITEM_MAGE_SLAYER 6
#define ITEM_PRIEST_SLAYER 7
#define ITEM_BUG_SLAYER 8
#define ITEM_ACIDIC_WEAPON 9
#define ITEM_SOULSUCKER 10
#define ITEM_DRAIN_MISSILES 11
#define ITEM_WEAK_WEAPON 12
#define ITEM_CAUSES_FEAR 13
#define ITEM_POISONED_WEAPON 14

/* General Ability */
#define ITEM_PROTECTION 30
#define ITEM_FULL_PROTECTION 31
#define ITEM_FIRE_PROTECTION 32
#define ITEM_COLD_PROTECTION 33
#define ITEM_POISON_PROTECTION 34
#define ITEM_MAGIC_PROTECTION 35
#define ITEM_ACID_PROTECTION 36
#define ITEM_SKILL 37
#define ITEM_STRENGTH 38
#define ITEM_DEXTERITY 39
#define ITEM_INTELLIGENCE 40
#define ITEM_ACCURACY 41
#define ITEM_THIEVING 42
#define ITEM_GIANT_STRENGTH 43
#define ITEM_LIGHTER_OBJECT 44
#define ITEM_HEAVIER_OBJECT 45
#define ITEM_OCCASIONAL_BLESS 46
#define ITEM_OCCASIONAL_HASTE 47
#define ITEM_LIFE_SAVING 48
#define ITEM_PROTECT_FROM_PETRIFY 49
#define ITEM_REGENERATE 50
#define ITEM_POISON_AUGMENT 51
#define ITEM_DISEASE_PARTY 52
#define ITEM_WILL 53
#define ITEM_FREE_ACTION 54
#define ITEM_SPEED 55
#define ITEM_SLOW_WEARER 56
#define ITEM_PROTECT_FROM_UNDEAD 57
#define ITEM_PROTECT_FROM_DEMONS 58
#define ITEM_PROTECT_FROM_HUMANOIDS 59
#define ITEM_PROTECT_FROM_REPTILES 60
#define ITEM_PROTECT_FROM_GIANTS 61
#define ITEM_PROTECT_FROM_DISEASE 62

/* NonSpell Use ;  the constant refers to both the positive and negative effect (don't mind the name). */
/*#define ITEM_POISON_WEAPON 70        //put poison on weapon
#define ITEM_BLESS_USER 71
#define ITEM_CURE_POISON 72
#define ITEM_HASTE_USER 73
#define ITEM_ADD_INVULNERABILITY 74
#define ITEM_ADD_MAGIC_RESISTANCE 75
#define ITEM_ADD_WEB 76
#define ITEM_CAUSE_DISEASE 77
#define ITEM_ADD_SANCTUARY 78
#define ITEM_CAUSE_DUMBFOUND 79
#define ITEM_ADD_MARTYRS_SHIELD 80
#define ITEM_CURE_SLEEP 81
#define ITEM_CURE_PARALYSIS 82
#define ITEM_CURE_ACID 83
#define ITEM_BLISS 84
#define ITEM_ADD_EXPERIENCE 85
#define ITEM_ADD_SKILL_POINTS 86
#define ITEM_ADD_HEALTH 87
#define ITEM_ADD_SPELL_POINTS 88
#define ITEM_DOOM 89
#define ITEM_LIGHT 90
#define ITEM_STEALTH 91
#define ITEM_FIREWALK 92
#define ITEM_FLYING 93
#define ITEM_MAJOR_HEALING 94*/

#define ITEM_POISON_WEAPON              70 //put poison on weapon
#define ITEM_BLESS_CURSE                71
#define ITEM_AFFECT_POISON              72
#define ITEM_HASTE_SLOW                 73
#define ITEM_AFFECT_INVULN              74
#define ITEM_AFFECT_MAGIC_RES           75
#define ITEM_AFFECT_WEB                 76
#define ITEM_AFFECT_DISEASE             77
#define ITEM_AFFECT_SANCTUARY           78
#define ITEM_AFFECT_DUMBFOUND           79
#define ITEM_AFFECT_MARTYRS_SHIELD      80
#define ITEM_AFFECT_SLEEP               81
#define ITEM_AFFECT_PARALYSIS           82
#define ITEM_AFFECT_ACID                83
#define ITEM_BLISS                      84
#define ITEM_AFFECT_EXPERIENCE          85
#define ITEM_AFFECT_SKILL_POINTS        86
#define ITEM_AFFECT_HEALTH              87
#define ITEM_AFFECT_SPELL_POINTS        88
#define ITEM_DOOM                       89
#define ITEM_LIGHT                      90
#define ITEM_STEALTH                    91
#define ITEM_FIREWALK                   92
#define ITEM_FLYING                     93
#define ITEM_MAJOR_HEALING              94


/* Spell Usable */

#define ITEM_SPELL_FLAME 110
#define ITEM_SPELL_FIREBALL 111
#define ITEM_SPELL_FIRESTORM 112
#define ITEM_SPELL_KILL 113
#define ITEM_SPELL_ICE_BOLT 114
#define ITEM_SPELL_SLOW 115
#define ITEM_SPELL_SHOCKWAVE 116
#define ITEM_SPELL_DISPEL_UNDEAD 117
#define ITEM_SPELL_DISPEL_SPIRIT 118
#define ITEM_SPELL_SUMMONING 119
#define ITEM_SPELL_MASS_SUMMONING 120
#define ITEM_SPELL_ACID_SPRAY 121
#define ITEM_SPELL_STINKING_CLOUD 122
#define ITEM_SPELL_SLEEP_FIELD 123
#define ITEM_SPELL_VENOM 124
#define ITEM_SPELL_SHOCKSTORM 125
#define ITEM_SPELL_PARALYSIS 126
#define ITEM_SPELL_WEB_SPELL 127
#define ITEM_SPELL_STRENGTHEN_TARGET 128 //wand of carrunos effect
#define ITEM_SPELL_QUICKFIRE 129
#define ITEM_SPELL_MASS_CHARM 130
#define ITEM_SPELL_MAGIC_MAP 131
#define ITEM_SPELL_DISPEL_BARRIER 132
#define ITEM_SPELL_MAKE_ICE_WALL 133
#define ITEM_SPELL_CHARM_SPELL 134
#define ITEM_SPELL_ANTIMAGIC_CLOUD 135

/* Reagents */
#define ITEM_HOLLY 150 // Holly/Toadstool
#define ITEM_COMFREY_ROOT 151
#define ITEM_GLOWING_NETTLE 152
#define ITEM_CRYPT_SHROOM 153 // Crypt Shroom/Wormgr.
#define ITEM_ASPTONGUE_MOLD 154
#define ITEM_EMBER_FLOWERS 155
#define ITEM_GRAYMOLD 156
#define ITEM_MANDRAKE 157
#define ITEM_SAPPHIRE 158
#define ITEM_SMOKY_CRYSTAL 159
#define ITEM_RESSURECTION_BALM 160
#define ITEM_LOCKPICKS 161

/* Missiles */

#define ITEM_MISSILE_RETURNING 170
#define ITEM_MISSILE_LIGHTNING 171
#define ITEM_MISSILE_EXPLODING 172
#define ITEM_MISSILE_ACID 173
#define ITEM_MISSILE_SLAY_UNDEAD 174
#define ITEM_MISSILE_SLAY_DEMON 175
#define ITEM_MISSILE_HEAL_TARGET 176

/*      Monsters Stuff      */

/* Skills Same as PC */

/* Monster Type */

#define MONSTER_TYPE_HUMAN 0
#define MONSTER_TYPE_REPTILE 1
#define MONSTER_TYPE_BEAST 2
#define MONSTER_TYPE_IMPORTANT 3
#define MONSTER_TYPE_MAGE 4
#define MONSTER_TYPE_PRIEST 5
#define MONSTER_TYPE_HUMANOID 6
#define MONSTER_TYPE_DEMON 7
#define MONSTER_TYPE_UNDEAD 8
#define MONSTER_TYPE_GIANT 9
#define MONSTER_TYPE_SLIME 10
#define MONSTER_TYPE_STONE 11
#define MONSTER_TYPE_BUG 12
#define MONSTER_TYPE_DRAGON 13
#define MONSTER_TYPE_MAGICAL_CREATURE 14

/* Attack Types */

#define MONSTER_ATTACK_SWINGS 0
#define MONSTER_ATTACK_CLAWS 1
#define MONSTER_ATTACK_BITES 2
#define MONSTER_ATTACK_SLIMES 3
#define MONSTER_ATTACK_PUNCHES 4
#define MONSTER_ATTACK_STINGS 5
#define MONSTER_ATTACK_CLUBS 6
#define MONSTER_ATTACK_BURNS 7
#define MONSTER_ATTACK_HARMS 8
#define MONSTER_ATTACK_STABS 9

/* Special Ability a.k.a spec_skill */

#define MONSTER_NO_SPECIAL_ABILITY 0
#define MONSTER_THROWS_DARTS 1
#define MONSTER_SHOOTS_ARROWS 2
#define MONSTER_THROWS_SPEARS 3
#define MONSTER_THROWS_ROCKS1 4    //4-24 damages
#define MONSTER_THROWS_ROCKS2 5    //5-30 damages
#define MONSTER_THROWS_ROCKS3 6    //6-36 damages
#define MONSTER_THROWS_RAZORDISKS 7
#define MONSTER_PETRIFICATION_RAY 8
#define MONSTER_SP_DRAIN_RAY 9    //spell points drain ray
#define MONSTER_HEAT_RAY 10
#define MONSTER_INVISIBLE 11
#define MONSTER_SPLITS 12
#define MONSTER_MINDLESS 13
#define MONSTER_BREATHES_STINKING_CLOUDS 14
#define MONSTER_ICY_TOUCH 15
#define MONSTER_XP_DRAINING_TOUCH 16
#define MONSTER_ICY_AND_DRAINING_TOUCH 17
#define MONSTER_SLOWING_TOUCH 18
#define MONSTER_SHOOTS_WEB 19
#define MONSTER_GOOD_ARCHER 20
#define MONSTER_STEALS_FOOD 21
#define MONSTER_PERMANENT_MARTYRS_SHIELD 22
#define MONSTER_PARALYSIS_RAY 23
#define MONSTER_DUMBFOUNDING_TOUCH 24
#define MONSTER_DISEASE_TOUCH 25
#define MONSTER_ABSORB_SPELLS 26
#define MONSTER_WEB_TOUCH 27
#define MONSTER_SLEEP_TOUCH 28
#define MONSTER_PARALYSIS_TOUCH 29
#define MONSTER_PETRIFICATION_TOUCH 30
#define MONSTER_ACID_TOUCH 31
#define MONSTER_BREATHES_SLEEP_CLOUDS 32
#define MONSTER_ACID_SPIT 33
#define MONSTER_SHOOTS_SPINES 34
#define MONSTER_DEATH_TOUCH 35
#define MONSTER_INVULNERABILITY 36
#define MONSTER_GUARD 37

/* Create Monsters/Fields */

#define MONSTER_NO_RADIATE 0
#define MONSTER_RADIATE_FIRE_FIELDS 1
#define MONSTER_RADIATE_ICE_FIELDS 2
#define MONSTER_RADIATE_SHOCK_FIELDS 3
#define MONSTER_RADIATE_ANTIMAGIC_FIELDS 4
#define MONSTER_RADIATE_SLEEP_FIELDS 5
#define MONSTER_RADIATE_STINKING_CLOUDS 6
//7,8 and 9 are unused
#define MONSTER_SUMMON1 10    //5 percent chance
#define MONSTER_SUMMON2 11    //20 percent chance
#define MONSTER_SUMMON3 12    //50 percent chance
//13 and 14 are unused
#define MONSTER_DEATH_TRIGGERS 15        //death triggers global special

/* Terrains Specials Properties : scenario.ter_types[i].special */      //complete

#define TER_SPEC_NONE 0
#define TER_SPEC_CHANGE_WHEN_STEP_ON 1
#define TER_SPEC_DOES_FIRE_DAMAGE 2
#define TER_SPEC_DOES_COLD_DAMAGE 3
#define TER_SPEC_DOES_MAGIC_DAMAGE 4
#define TER_SPEC_POISON_LAND 5
#define TER_SPEC_DISEASED_LAND 6
#define TER_SPEC_CRUMBLING_TERRAIN 7
#define TER_SPEC_LOCKABLE_TERRAIN 8
#define TER_SPEC_UNLOCKABLE_TERRAIN 9
#define TER_SPEC_UNLOCKABLE_BASHABLE 10
#define TER_SPEC_IS_A_SIGN 11
#define TER_SPEC_CALL_LOCAL_SPECIAL 12
#define TER_SPEC_CALL_SCENARIO_SPECIAL 13
#define TER_SPEC_IS_A_CONTAINER 14
#define TER_SPEC_WATERFALL 15
#define TER_SPEC_CONVEYOR_NORTH 16
#define TER_SPEC_CONVEYOR_EAST 17
#define TER_SPEC_CONVEYOR_SOUTH 18
#define TER_SPEC_CONVEYOR_WEST 19
#define TER_SPEC_BLOCKED_TO_MONSTERS 20
#define TER_SPEC_TOWN_ENTRANCE 21
#define TER_SPEC_CAN_BE_USED 22
#define TER_SPEC_CALL_SPECIAL_WHEN_USED 23

//stuff used in actions.cpp

//Startup button rects (also used in blades.cpp)
#define STARTBTN_LOAD 0
#define STARTBTN_NEW 1
#define STARTBTN_ORDER 2
#define STARTBTN_JOIN 3
#define STARTBTN_CUSTOM 4

//Shop rects
#define SHOPRECT_WHOLE_AREA 0
#define SHOPRECT_ACTIVE_AREA 1
#define SHOPRECT_GRAPHIC 2
#define SHOPRECT_ITEM_NAME 3
#define SHOPRECT_ITEM_COST 4
#define SHOPRECT_ITEM_EXTRA 5
#define SHOPRECT_ITEM_HELP 6

// Item button rects
#define ITEMBTN_NAME 0
#define ITEMBTN_USE 1
#define ITEMBTN_GIVE 2
#define ITEMBTN_DROP 3
#define ITEMBTN_INFO 4
#define ITEMBTN_SPEC 5 // Sell, Identify, or Enchant

// PC button rects
#define PCBTN_NAME 0
#define PCBTN_HP 1
#define PCBTN_SP 2
#define PCBTN_INFO 3
#define PCBTN_TRADE 4

//Spell select
#define SPELL_SELECT_NONE 0
#define SPELL_SELECT_ACTIVE 1
#define SPELL_SELECT_ANY 2

//Spell refer
#define SPELL_REFER 0
#define SPELL_IMMED 1
#define SPELL_TARGET 2
#define SPELL_FANCY_TARGET 3

/* stuff done flags */
/*#define SDF_IS_PARTY_SPLIT 304][0
#define SDF_PARTY_SPLIT_WHERE_PARTY_X 304][1
#define SDF_PARTY_SPLIT_WHERE_PARTY_Y 304][2
#define SDF_PARTY_SPLIT_PC      304][3
#define SDF_PARTY_SPLIT_TOWN    304][4 //for future use, hopefully
//#define SDF_SKIP_STARTUP        305][4 //is now a boolean : play_startup
#define SDF_LESS_SOUND          305][5
#define SDF_NO_TARGET_LINE      305][6
#define SFD_NO_MAPS             306][0
#define SDF_NO_SOUNDS           306][1
#define SDF_NO_FRILLS           306][2
#define SDF_ROOM_DESCS_AGAIN    306][3 //not used in the code
#define SDF_NO_INSTANT_HELP     306][4 //boolean
#define SDF_NO_SHORE_FRILLS     306][5
#define SDF_GAME_SPEED          306][6
#define SDF_EASY_MODE           306][7
#define SDF_LESS_WANDER_ENC     306][8
#define SDF_NO_TER_ANIM         306][9
#define SDF_HIDDEN_MAP          308][0
#define SDF_LEGACY_SCENARIO     305][8 //0 is new scenario, 1 is legacy
*/

/* Monsters Attitudes :
    0 - Friendly, Docile
    1 - Hostile, Type A
    2 - Friendly, Will fight
    3 - Hostile, Type B
*/

//Specials and Talks

//eSpecContext

#define SPEC_OUT_MOVE 0
#define SPEC_TOWN_MOVE 1
#define SPEC_COMBAT_MOVE 2
#define SPEC_OUT_LOOK 3
#define SPEC_TOWN_LOOK 4
#define SPEC_ENTER_TOWN 5
#define SPEC_LEAVE_TOWN 6
#define SPEC_TALK 7
#define SPEC_USE_SPEC_ITEM 8
#define SPEC_TOWN_TIMER 9
#define SPEC_SCEN_TIMER 10
#define SPEC_PARTY_TIMER 11
#define SPEC_KILL_MONST 12
#define SPEC_OUTDOOR_ENC 13
#define SPEC_WIN_ENCOUNTER 14
#define SPEC_FLEE_ENCOUNTER 15
#define SPEC_TARGET 16
#define SPEC_USE_SPACE 17
#define SPEC_SEE_MONST 18

//eSpecNodeType

#define SPEC_NULL 0
#define SPEC_SET_SDF 1
#define SPEC_INC_SDF 2
#define SPEC_DISPLAY_MSG 3
#define SPEC_SECRET_PASSAGE 4
#define SPEC_DISPLAY_SM_MSG 5
#define SPEC_FLIP_SDF 6
#define SPEC_OUT_BLOCK 7
#define SPEC_TOWN_BLOCK 8
#define SPEC_FIGHT_BLOCK 9
#define SPEC_LOOK_BLOCK 10
#define SPEC_CANT_ENTER 11
#define SPEC_CHANGE_TIME 12
#define SPEC_SCEN_TIMER_START 13
#define SPEC_PLAY_SOUND 14
#define SPEC_CHANGE_HORSE_OWNER 15
#define SPEC_CHANGE_BOAT_OWNER 16
#define SPEC_SET_TOWN_VISIBILITY 17
#define SPEC_MAJOR_EVENT_OCCURRED 18
#define SPEC_FORCED_GIVE 19
#define SPEC_BUY_ITEMS_OF_TYPE 20
#define SPEC_CALL_GLOBAL 21
#define SPEC_SET_SDF_ROW 22
#define SPEC_COPY_SDF 23
#define SPEC_SANCTIFY 24
#define SPEC_REST 25
#define SPEC_WANDERING_WILL_FIGHT 26
#define SPEC_END_SCENARIO 27
#define SPEC_ONCE_GIVE_ITEM = 50
#define SPEC_ONCE_GIVE_SPEC_ITEM 51
#define SPEC_ONCE_NULL 52
#define SPEC_ONCE_SET_SDF 53
#define SPEC_ONCE_DISPLAY_MSG 54
#define SPEC_ONCE_DIALOG 55
#define SPEC_ONCE_DIALOG_TERRAIN 56
#define SPEC_ONCE_DIALOG_MONSTER 57
#define SPEC_ONCE_GIVE_ITEM_DIALOG 58
#define SPEC_ONCE_GIVE_ITEM_TERRAIN 59
#define SPEC_ONCE_GIVE_ITEM_MONSTER 60
#define SPEC_ONCE_OUT_ENCOUNTER 61
#define SPEC_ONCE_TOWN_ENCOUNTER 62
#define SPEC_ONCE_TRAP 63
#define SPEC_SELECT_PC 80
#define SPEC_DAMAGE 81
#define SPEC_AFFECT_HP 82
#define SPEC_AFFECT_SP 83
#define SPEC_AFFECT_XP 84
#define SPEC_AFFECT_SKILL_PTS 85
#define SPEC_AFFECT_DEADNESS 86
#define SPEC_AFFECT_POISON 87
#define SPEC_AFFECT_SPEED 88
#define SPEC_AFFECT_INVULN 89
#define SPEC_AFFECT_MAGIC_RES 90
#define SPEC_AFFECT_WEBS 91
#define SPEC_AFFECT_DISEASE 92
#define SPEC_AFFECT_SANCTUARY 93
#define SPEC_AFFECT_CURSE_BLESS 94
#define SPEC_AFFECT_DUMBFOUND 95
#define SPEC_AFFECT_SLEEP 96
#define SPEC_AFFECT_PARALYSIS 97
#define SPEC_AFFECT_STAT 98
#define SPEC_AFFECT_MAGE_SPELL 99
#define SPEC_AFFECT_PRIEST_SPELL 100
#define SPEC_AFFECT_GOLD 101
#define SPEC_AFFECT_FOOD 102
#define SPEC_AFFECT_ALCHEMY 103
#define SPEC_AFFECT_STEALTH 104
#define SPEC_AFFECT_FIREWALK 105
#define SPEC_AFFECT_FLIGHT 106
#define SPEC_IF_SDF 130
#define SPEC_IF_TOWN_NUM 131
#define SPEC_IF_RANDOM 132
#define SPEC_IF_HAVE_SPECIAL_ITEM 133
#define SPEC_IF_SDF_COMPARE 134
#define SPEC_IF_TOWN_TER_TYPE 135
#define SPEC_IF_OUT_TER_TYPE 136
#define SPEC_IF_HAS_GOLD 137
#define SPEC_IF_HAS_FOOD 138
#define SPEC_IF_ITEM_CLASS_ON_SPACE 139
#define SPEC_IF_HAVE_ITEM_CLASS 140
#define SPEC_IF_EQUIP_ITEM_CLASS 141
#define SPEC_IF_HAS_GOLD_AND_TAKE 142
#define SPEC_IF_HAS_FOOD_AND_TAKE 143
#define SPEC_IF_ITEM_CLASS_ON_SPACE_AND_TAKE 144
#define SPEC_IF_HAVE_ITEM_CLASS_AND_TAKE 145
#define SPEC_IF_EQUIP_ITEM_CLASS_AND_TAKE 146
#define SPEC_IF_DAY_REACHED 147
#define SPEC_IF_BARRELS 148
#define SPEC_IF_CRATES 149
#define SPEC_IF_EVENT_OCCURRED 150
#define SPEC_IF_HAS_CAVE_LORE 151
#define SPEC_IF_HAS_WOODSMAN 152
#define SPEC_IF_ENOUGH_MAGE_LORE 153
#define SPEC_IF_TEXT_RESPONSE 154
#define SPEC_IF_SDF_EQ 155
#define SPEC_MAKE_TOWN_HOSTILE 170
#define SPEC_TOWN_CHANGE_TER 171
#define SPEC_TOWN_SWAP_TER 172
#define SPEC_TOWN_TRANS_TER 173
#define SPEC_TOWN_MOVE_PARTY 174
#define SPEC_TOWN_HIT_SPACE 175
#define SPEC_TOWN_EXPLODE_SPACE 176
#define SPEC_TOWN_LOCK_SPACE 177
#define SPEC_TOWN_UNLOCK_SPACE 178
#define SPEC_TOWN_SFX_BURST 179
#define SPEC_TOWN_CREATE_WANDERING 180
#define SPEC_TOWN_PLACE_MONST 181
#define SPEC_TOWN_DESTROY_MONST 182
#define SPEC_TOWN_NUKE_MONSTS 183
#define SPEC_TOWN_GENERIC_LEVER 184
#define SPEC_TOWN_GENERIC_PORTAL 185
#define SPEC_TOWN_GENERIC_BUTTON 186
#define SPEC_TOWN_GENERIC_STAIR 187
#define SPEC_TOWN_LEVER 188
#define SPEC_TOWN_PORTAL 189
#define SPEC_TOWN_STAIR 190
#define SPEC_TOWN_RELOCATE 191 // Relocate outdoors
#define SPEC_TOWN_PLACE_ITEM 192
#define SPEC_TOWN_SPLIT_PARTY 193
#define SPEC_TOWN_REUNITE_PARTY 194
#define SPEC_TOWN_TIMER_START 195
#define SPEC_RECT_PLACE_FIRE 200
#define SPEC_RECT_PLACE_FORCE 201
#define SPEC_RECT_PLACE_ICE 202
#define SPEC_RECT_PLACE_BLADE 203
#define SPEC_RECT_PLACE_SCLOUD 204
#define SPEC_RECT_PLACE_SLEEP 205
#define SPEC_RECT_PLACE_QUICKFIRE 206
#define SPEC_RECT_PLACE_FIRE_BARR 207
#define SPEC_RECT_PLACE_FORCE_BARR 208
#define SPEC_RECT_CLEANSE 209
#define SPEC_RECT_PLACE_SFX 210
#define SPEC_RECT_PLACE_OBJECT 211 // place barrels, etc
#define SPEC_RECT_MOVE_ITEMS 212
#define SPEC_RECT_DESTROY_ITEMS 213
#define SPEC_RECT_CHANGE_TER 214
#define SPEC_RECT_SWAP_TER 215,
#define SPEC_RECT_TRANS_TER 216
#define SPEC_RECT_LOCK 217
#define SPEC_RECT_UNLOCK 218
#define SPEC_OUT_MAKE_WANDER 225
#define SPEC_OUT_CHANGE_TER 226
#define SPEC_OUT_PLACE_ENCOUNTER 227
#define SPEC_OUT_MOVE_PARTY 228
#define SPEC_OUT_STORE 229


// eTalkNodeType

#define    TALK_REGULAR 0
#define    TALK_DEP_ON_SDF 1
#define    TALK_SET_SDF 2
#define    TALK_INN 3
#define    TALK_DEP_ON_TIME 4
#define    TALK_DEP_ON_TIME_AND_EVENT 5
#define    TALK_DEP_ON_TOWN 6
#define    TALK_BUY_ITEMS 7
#define    TALK_TRAINING 8
#define    TALK_BUY_MAGE 9
#define    TALK_BUY_PRIEST 10
#define    TALK_BUY_ALCHEMY 11
#define    TALK_BUY_HEALING 12
#define    TALK_SELL_WEAPONS 13
#define    TALK_SELL_ARMOR 14
#define    TALK_SELL_ITEMS 15
#define    TALK_IDENTIFY 16
#define    TALK_ENCHANT 17
#define    TALK_BUY_INFO 18
#define    TALK_BUY_SDF 19
#define    TALK_BUY_SHIP 20
#define    TALK_BUY_HORSE 21
#define    TALK_BUY_SPEC_ITEM 22
#define    TALK_BUY_JUNK 23
#define    TALK_BUY_TOWN_LOC 24
#define    TALK_END_FORCE 25
#define    TALK_END_FIGHT 26
#define    TALK_END_ALARM 27 // Town hostile
#define    TALK_END_DIE 28
#define    TALK_CALL_TOWN_SPEC 29
#define    TALK_CALL_SCEN_SPEC 30

/* Spells

x : mage spells (x from 0 to 61)
100 + x : priest spells (x from 0 to 61)
*/

#endif
