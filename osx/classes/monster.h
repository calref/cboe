/*
 *  monster.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef BOE_DATA_MONSTER_H
#define BOE_DATA_MONSTER_H

#include <string>
#include <map>
#include <iosfwd>

#include "soundtool.h"
#include "simpletypes.h"
#include "graphtool.h"

namespace legacy {
	struct monster_record_type;
	struct creature_data_type;
	struct creature_start_type;
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


#define MONSTER_NO_SPECIAL_ABILITY			0
#define MONSTER_THROWS_DARTS				1 //1-6
#define MONSTER_SHOOTS_ARROWS				2 //2-12
#define MONSTER_THROWS_SPEARS				3 //3-18
#define MONSTER_THROWS_ROCKS1				4 //4-24 damages
#define MONSTER_THROWS_ROCKS2				5 //5-30 damages
#define MONSTER_THROWS_ROCKS3				6 //6-36 damages
#define MONSTER_THROWS_RAZORDISKS			7 //4-24
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
#define MONSTER_GOOD_ARCHER					20 //7-42
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
#define MONSTER_SHOOTS_SPINES				34 //7-42
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
		operator int() const;
		cAttack& operator=(int n);
	};
	struct cAbility{
		eMonstAbil abil;
		unsigned char extra1, extra2;
		operator std::string();
	};
	unsigned char level;
	std::string m_name;
	short m_health;
	unsigned char armor;
	unsigned char skill;
	cAttack a[3];
	eRace m_type;
	unsigned char speed;
	unsigned char mu;
	unsigned char cl;
	unsigned char breath;
	unsigned char breath_type;
	unsigned char treasure;
	unsigned char spec_skill; // TODO: Delete in favour of cAbility
	unsigned char poison;
	item_num_t corpse_item;
	short corpse_item_chance;
	unsigned char immunities;
	unsigned char x_width,y_width;
	unsigned char radiate_1; // TODO: Delete in favour of cAbility
	unsigned short radiate_2; // I THINK this is the extra field for the second ability TODO: Delete in favour of cAbility
	unsigned char default_attitude;
	unsigned char summon_type;
	pic_num_t default_facial_pic;
	pic_num_t picture_num;
	str_num_t see_str1, see_str2;
	snd_num_t see_sound, ambient_sound; // ambient_sound has a chance of being played every move
	spec_num_t see_spec;
private:
	cAbility abil1, abil2;
public:
	
	std::string getAbil1Name();
	std::string getAbil2Name();
	bool hasAbil(eMonstAbil what, unsigned char* x1 = NULL, unsigned char* x2 = NULL);
	cMonster& operator = (legacy::monster_record_type& old);
	cMonster();
	void writeTo(std::ostream& file);
	void readFrom(std::istream& file);
};

class cCreature : public cMonster {
public:
	using cMonster::operator=;
	unsigned long id;
	m_num_t number;
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
	short health;
	short mp;
	short max_mp;
	unsigned char ap;
	short morale,m_morale; // these are calculated in-game based on the level
	std::map<eStatus,short> status;
	unsigned char direction;
	
	cCreature();
	cCreature(int num);
	
	cCreature& operator = (legacy::creature_data_type old);
	cCreature& operator = (legacy::creature_start_type old);
	cCreature& operator = (const cCreature& other);
	//cCreature& operator = (const cMonster& other);
	void writeTo(std::ostream& file);
	void readFrom(std::istream& file);
};

std::ostream& operator << (std::ostream& out, eStatus& e);
std::istream& operator >> (std::istream& in, eStatus& e);
std::ostream& operator << (std::ostream& out, eRace& e);
std::istream& operator >> (std::istream& in, eRace& e);
std::ostream& operator << (std::ostream& out, eMonstAbil& e);
std::istream& operator >> (std::istream& in, eMonstAbil& e);
std::ostream& operator<<(std::ostream& out, const cMonster::cAttack& att);
#endif