/*
 *  monster.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef MONSTER_H
#define MONSTER_H

#include <string>
#include <iosfwd>

namespace legacy {
	struct monster_record_type;
	struct creature_data_type;
	struct creature_start_type;
};

typedef unsigned short m_num_t;

/* adven[i].race */ //complete
enum eRace {
	RACE_UNKNOWN = -1, // for parameters to some functions; not valid in the class
	RACE_HUMAN = 0,
	RACE_NEPHIL = 1,
	RACE_SLITH = 2,
	RACE_VAHNATAI = 3,
	RACE_REPTILE = 4,
	RACE_BEAST = 5,
	RACE_IMPORTANT = 6,
	RACE_MAGE = 7,
	RACE_PRIEST = 8,
	RACE_HUMANOID = 9,
	RACE_DEMON = 10,
	RACE_UNDEAD = 11,
	RACE_GIANT = 12,
	RACE_SLIME = 13,
	RACE_STONE = 14,
	RACE_BUG = 15,
	RACE_DRAGON = 16,
	RACE_MAGICAL = 17,
	RACE_PLANT = 18,
	RACE_BIRD = 19,
}; // TODO: Expand and merge with eMonsterType

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

/* Monster Type */
enum eMonsterType {
	MONSTER_TYPE_UNKNOWN = -1, // for parameters to some functions; not valid in the class
	MONSTER_TYPE_HUMAN = 0,
	MONSTER_TYPE_REPTILE = 1,
	MONSTER_TYPE_BEAST = 2,
	MONSTER_TYPE_IMPORTANT = 3,
	MONSTER_TYPE_MAGE = 4,
	MONSTER_TYPE_PRIEST = 5,
	MONSTER_TYPE_HUMANOID = 6,
	MONSTER_TYPE_DEMON = 7,
	MONSTER_TYPE_UNDEAD = 8,
	MONSTER_TYPE_GIANT = 9,
	MONSTER_TYPE_SLIME = 10,
	MONSTER_TYPE_STONE = 11,
	MONSTER_TYPE_BUG = 12,
	MONSTER_TYPE_DRAGON = 13,
	MONSTER_TYPE_MAGICAL = 14,
};

/* Attack Types */

#define MONSTER_ATTACK_SWINGS		0
#define MONSTER_ATTACK_CLAWS		1
#define MONSTER_ATTACK_BITES		2
#define MONSTER_ATTACK_SLIMES		3
#define MONSTER_ATTACK_PUNCHES		4
#define MONSTER_ATTACK_STINGS		5
#define MONSTER_ATTACK_CLUBS		6
#define MONSTER_ATTACK_BURNS		7
#define MONSTER_ATTACK_HARMS		8
#define MONSTER_ATTACK_STABS		9

/* Special Ability a.k.a spec_skill */

#define MONSTER_NO_SPECIAL_ABILITY			0
#define MONSTER_THROWS_DARTS				1
#define MONSTER_SHOOTS_ARROWS				2
#define MONSTER_THROWS_SPEARS				3
#define MONSTER_THROWS_ROCKS1				4 //4-24 damages
#define MONSTER_THROWS_ROCKS2				5 //5-30 damages
#define MONSTER_THROWS_ROCKS3				6 //6-36 damages
#define MONSTER_THROWS_RAZORDISKS			7
#define MONSTER_PETRIFICATION_RAY			8
#define MONSTER_SP_DRAIN_RAY				9 //spell points drain ray
#define MONSTER_HEAT_RAY					10
#define MONSTER_INVISIBLE					11
#define MONSTER_SPLITS						12
#define MONSTER_MINDLESS					13
#define MONSTER_BREATHES_STINKING_CLOUDS	14
#define MONSTER_ICY_TOUCH					15
#define MONSTER_XP_DRAINING_TOUCH			16
#define MONSTER_ICY_AND_DRAINING_TOUCH		17
#define MONSTER_SLOWING_TOUCH				18
#define MONSTER_SHOOTS_WEB					19
#define MONSTER_GOOD_ARCHER					20
#define MONSTER_STEALS_FOOD					21
#define MONSTER_PERMANENT_MARTYRS_SHIELD	22
#define MONSTER_PARALYSIS_RAY				23
#define MONSTER_DUMBFOUNDING_TOUCH			24
#define MONSTER_DISEASE_TOUCH				25
#define MONSTER_ABSORB_SPELLS				26
#define MONSTER_WEB_TOUCH					27
#define MONSTER_SLEEP_TOUCH					28
#define MONSTER_PARALYSIS_TOUCH				29
#define MONSTER_PETRIFICATION_TOUCH			30
#define MONSTER_ACID_TOUCH					31
#define MONSTER_BREATHES_SLEEP_CLOUDS		32
#define MONSTER_ACID_SPIT					33
#define MONSTER_SHOOTS_SPINES				34
#define MONSTER_DEATH_TOUCH					35
#define MONSTER_INVULNERABILITY				36
#define MONSTER_GUARD						37

/* Create Monsters/Fields */

#define MONSTER_NO_RADIATE					0
#define MONSTER_RADIATE_FIRE_FIELDS			1
#define MONSTER_RADIATE_ICE_FIELDS			2
#define MONSTER_RADIATE_SHOCK_FIELDS		3
#define MONSTER_RADIATE_ANTIMAGIC_FIELDS	4
#define MONSTER_RADIATE_SLEEP_FIELDS		5
#define MONSTER_RADIATE_STINKING_CLOUDS		6
//as said 7,8 and 9 are unused
#define MONSTER_SUMMON1						10 //5 percent chance
#define MONSTER_SUMMON2						11 //20 percent chance
#define MONSTER_SUMMON3						12 //50 percent chance
//as said 13 and 14 are unused
#define MONSTER_DEATH_TRIGGERS				15 //death triggers global special

class cMonster {
public:
	struct cAttack{
		unsigned char dice, sides, type;
		// TODO: Remove the need for these operators by changing the code that uses them
		operator int();
		cAttack& operator=(int n);
	};
	m_num_t m_num; // TODO: This probably shouldn't be necessary. Consider why it is, and determine if it can be removed
	unsigned char level;
	std::string m_name;
	short health; // TODO: Move health, mp and max_mp to cCreature
	short m_health;
	short mp;
	short max_mp;
	unsigned char armor;
	unsigned char skill;
	cAttack a[3];
	unsigned char a1_type,a23_type; // TODO: Delete in favour of type field of cAttack
	eMonsterType m_type;
	unsigned char speed;
	unsigned char ap; // TODO: Move ap to cCreature
	unsigned char mu;
	unsigned char cl;
	unsigned char breath;
	unsigned char breath_type;
	unsigned char treasure;
	unsigned char spec_skill;
	unsigned char poison;
	short morale,m_morale; // TODO: Move to cCreature (since these are calculated in-game based on the level)
	item_num_t corpse_item;
	short corpse_item_chance;
	short status[15]; // TODO: Move to cCreature
	unsigned char direction; // TODO: Move direction to cCreature
	unsigned char immunities;
	unsigned char x_width,y_width;
	unsigned char radiate_1;
	unsigned short radiate_2; // I THINK this is the extra field for the second ability
	unsigned char default_attitude,summon_type,default_facial_pic,res1,res2,res3;
	short picture_num;
	str_num_t see_str1, see_str2;
	snd_num_t see_sound, ambient_sound; // ambient_sound has a 
	spec_num_t see_spec;
	
	cMonster& operator = (legacy::monster_record_type& old);
	void writeTo(std::ostream& file, std::string prefix);
};

class cCreature : public cMonster {
public:
	using cMonster::operator=;
	unsigned long id;
	m_num_t number; // TODO: This appears to be a duplicate of cMonster::m_num (ie it's used for the same thing)
	short active, attitude;
	unsigned char start_attitude;
	location start_loc, cur_loc;
	unsigned char mobility; // Was a bool, but am making it a char to allow for additional mobility states
	unsigned char time_flag;
	short summoned;
	unsigned char extra1, extra2;
	short spec1, spec2;
	char spec_enc_code, time_code;
	short monster_time, personality;
	short special_on_kill, facial_pic;
	short target;
	location targ_loc;
	
	cCreature();
	
	cCreature& operator = (legacy::creature_data_type old);
	cCreature& operator = (legacy::creature_start_type old);
	cCreature& operator = (const cCreature& other);
	//cCreature& operator = (const cMonster& other);
};

std::ostream& operator << (std::ostream& out, eStatus& e);
std::istream& operator >> (std::istream& in, eStatus& e);
std::ostream& operator << (std::ostream& out, eRace& e);
std::istream& operator >> (std::istream& in, eRace& e);
#endif