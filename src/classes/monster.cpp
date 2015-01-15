/*
 *  monster.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>

#include "classes.h"
#include "oldstructs.h"
#include "fileio.hpp"
#include "spell.hpp"

static uAbility test;
static_assert(&test.active == &test.missile.active, "uAbility union has incorrect layout");

void cMonster::append(legacy::monster_record_type& old){
	level = old.level;
	//m_name = old.m_name;
	m_health = old.m_health;
	armor = old.armor;
	skill = old.skill;
	for(int i = 0; i < 3; i++) a[i] = old.a[i];
	a[0].type = old.a1_type;
	a[1].type = a[2].type = old.a23_type;
	// Unless human, add 3 to the monster's type to get its race
	// This is because nephil, slith, and vahnatai were inserted
	if(old.m_type) m_type = eRace(old.m_type + 3);
	else m_type = eRace::HUMAN;
	speed = old.speed;
	mu = old.mu;
	cl = old.cl;
	treasure = old.treasure;
	invisible = mindless = invuln = guard = false;
	abil.clear();
	switch(old.spec_skill) {
		case 1: addAbil(eMonstAbilTemplate::THROWS_DARTS); break;
		case 2: addAbil(eMonstAbilTemplate::SHOOTS_ARROWS); break;
		case 3: addAbil(eMonstAbilTemplate::THROWS_SPEARS); break;
		case 4: addAbil(eMonstAbilTemplate::THROWS_ROCKS1); break;
		case 5: addAbil(eMonstAbilTemplate::THROWS_ROCKS1); break;
		case 6: addAbil(eMonstAbilTemplate::THROWS_ROCKS1); break;
		case 7: addAbil(eMonstAbilTemplate::THROWS_RAZORDISKS); break;
		case 8: addAbil(eMonstAbilTemplate::RAY_PETRIFY); break;
		case 9: addAbil(eMonstAbilTemplate::RAY_SP_DRAIN); break;
		case 10: addAbil(eMonstAbilTemplate::RAY_HEAT); break;
		case 11: invisible = true; break;
		case 12: addAbil(eMonstAbilTemplate::SPLITS); break;
		case 13: mindless = true; break;
		case 14: addAbil(eMonstAbilTemplate::BREATH_FOUL); break;
		case 15: addAbil(eMonstAbilTemplate::TOUCH_ICY); break;
		case 17: addAbil(eMonstAbilTemplate::TOUCH_ICY_DRAINING); break;
		case 16: addAbil(eMonstAbilTemplate::TOUCH_XP_DRAIN); break;
		case 18: addAbil(eMonstAbilTemplate::TOUCH_STUN); break;
		case 19: addAbil(eMonstAbilTemplate::SHOOTS_WEB); break;
		case 20: addAbil(eMonstAbilTemplate::GOOD_ARCHER); break;
		case 21: addAbil(eMonstAbilTemplate::TOUCH_STEAL_FOOD); break;
		case 22: addAbil(eMonstAbilTemplate::MARTYRS_SHIELD); break;
		case 23: addAbil(eMonstAbilTemplate::RAY_PARALYSIS); break;
		case 24: addAbil(eMonstAbilTemplate::TOUCH_DUMB); break;
		case 25: addAbil(eMonstAbilTemplate::TOUCH_DISEASE); break;
		case 26: addAbil(eMonstAbilTemplate::ABSORB_SPELLS); break;
		case 27: addAbil(eMonstAbilTemplate::TOUCH_WEB); break;
		case 28: addAbil(eMonstAbilTemplate::TOUCH_SLEEP); break;
		case 29: addAbil(eMonstAbilTemplate::TOUCH_PARALYSIS); break;
		case 30: addAbil(eMonstAbilTemplate::TOUCH_PETRIFY); break;
		case 31: addAbil(eMonstAbilTemplate::TOUCH_ACID); break;
		case 32: addAbil(eMonstAbilTemplate::BREATH_SLEEP); break;
		case 33: addAbil(eMonstAbilTemplate::SPIT_ACID); break;
		case 34: addAbil(eMonstAbilTemplate::SHOOTS_SPINES); break;
		case 35: addAbil(eMonstAbilTemplate::TOUCH_DEATH); break;
		case 36: invuln = true; break;
		case 37: guard = true; break;
	}
	switch(old.radiate_1) {
		case 1: addAbil(eMonstAbilTemplate::RADIATE_FIRE, old.radiate_2); break;
		case 2: addAbil(eMonstAbilTemplate::RADIATE_ICE, old.radiate_2); break;
		case 3: addAbil(eMonstAbilTemplate::RADIATE_SHOCK, old.radiate_2); break;
		case 4: addAbil(eMonstAbilTemplate::RADIATE_ANTIMAGIC, old.radiate_2); break;
		case 5: addAbil(eMonstAbilTemplate::RADIATE_SLEEP, old.radiate_2); break;
		case 6: addAbil(eMonstAbilTemplate::RADIATE_STINK, old.radiate_2); break;
		case 10: addAbil(eMonstAbilTemplate::SUMMON_5, old.radiate_2); break;
		case 11: addAbil(eMonstAbilTemplate::SUMMON_20, old.radiate_2); break;
		case 12: addAbil(eMonstAbilTemplate::SUMMON_50, old.radiate_2); break;
		case 15: addAbil(eMonstAbilTemplate::DEATH_TRIGGERS, old.radiate_2); break;
	}
	if(old.poison > 0) addAbil(eMonstAbilTemplate::TOUCH_POISON, old.poison);
	if(old.breath > 0) {
		switch(old.breath_type) {
			case 0: addAbil(eMonstAbilTemplate::BREATH_FIRE, old.breath); break;
			case 1: addAbil(eMonstAbilTemplate::BREATH_FROST, old.breath); break;
			case 2: addAbil(eMonstAbilTemplate::BREATH_ELECTRICITY, old.breath); break;
			case 3: addAbil(eMonstAbilTemplate::BREATH_DARKNESS, old.breath); break;
		}
	}
	corpse_item = old.corpse_item;
	corpse_item_chance = old.corpse_item_chance;
	if(old.immunities & 2)
		magic_res = RESIST_ALL;
	else if(old.immunities & 1)
		magic_res = RESIST_HALF;
	if(old.immunities & 8)
		fire_res = RESIST_ALL;
	else if(old.immunities & 4)
		fire_res = RESIST_HALF;
	if(old.immunities & 32)
		cold_res = RESIST_ALL;
	else if(old.immunities & 16)
		cold_res = RESIST_HALF;
	if(old.immunities & 128)
		poison_res = RESIST_ALL;
	else if(old.immunities & 64)
		poison_res = RESIST_HALF;
	x_width = old.x_width;
	y_width = old.y_width;
	default_attitude = old.default_attitude;
	summon_type = old.summon_type;
	default_facial_pic = old.default_facial_pic;
	picture_num = old.picture_num;
	if(picture_num == 122) picture_num = 119;
	see_spec = -1;
	see_str1 = see_str2 = -1;
	see_sound = 0;
	ambient_sound = 0;
}

void cMonster::addAbil(eMonstAbilTemplate what, int param) {
	switch(what) {
		// Missiles: {true, type, missile pic, dice, sides, skill, range, odds}
		case eMonstAbilTemplate::THROWS_DARTS:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::DART, 1, 1, 7, 2, 6, 500};
			break;
		case eMonstAbilTemplate::SHOOTS_ARROWS:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ARROW, 3, 2, 7, 4, 8, 750};
			break;
		case eMonstAbilTemplate::THROWS_SPEARS:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ARROW, 5, 3, 7, 6, 8, 625};
			break;
		case eMonstAbilTemplate::THROWS_ROCKS1:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ROCK, 12, 4, 7, 8, 10, 625};
			break;
		case eMonstAbilTemplate::THROWS_ROCKS2:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ROCK, 12, 6, 7, 12, 10, 500};
			break;
		case eMonstAbilTemplate::THROWS_ROCKS3:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ROCK, 12, 8, 7, 16, 10, 500};
			break;
		case eMonstAbilTemplate::THROWS_RAZORDISKS:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::RAZORDISK, 7, 7, 7, 14, 8, 625};
			break;
		case eMonstAbilTemplate::THROWS_KNIVES:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::KNIFE, 10, 2, 7, 2, 6, 500};
			break;
		case eMonstAbilTemplate::GOOD_ARCHER:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ARROW, 3, 8, 7, 16, 10, 875};
			break;
		case eMonstAbilTemplate::SHOOTS_SPINES:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::SPINE, 5, 6, 7, 12, 9, 625};
			break;
		case eMonstAbilTemplate::CROSSBOWMAN:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::BOLT, 3, 10, 7, 16, 10, 875};
			break;
		case eMonstAbilTemplate::SLINGER:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ROCK, 12, 2, 7, 3, 8, 750};
			break;
		// Magical missiles: {true, type, missile pic, strength, range, odds}
		case eMonstAbilTemplate::RAY_PETRIFY:
			abil[eMonstAbil::PETRIFY].gen = {true, eMonstGen::GAZE, -1, 25, 6, 625};
			break;
		case eMonstAbilTemplate::RAY_SP_DRAIN:
			abil[eMonstAbil::DRAIN_SP].gen = {true, eMonstGen::GAZE, 8, 50, 8, 625};
			break;
		case eMonstAbilTemplate::RAY_HEAT:
			abil[eMonstAbil::DAMAGE].gen = {true, eMonstGen::RAY, 13, 7, 6, 625};
			abil[eMonstAbil::DAMAGE].gen.dmg = eDamageType::FIRE;
			break;
		case eMonstAbilTemplate::RAY_PARALYSIS:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::RAY, -1, 100, 6, 750};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::PARALYZED;
			break;
		case eMonstAbilTemplate::BREATH_FIRE:
			abil[eMonstAbil::DAMAGE].gen = {true, eMonstGen::BREATH, 13, param, 8, 375};
			abil[eMonstAbil::DAMAGE].gen.dmg = eDamageType::UNBLOCKABLE;
			break;
		case eMonstAbilTemplate::BREATH_FROST:
			abil[eMonstAbil::DAMAGE].gen = {true, eMonstGen::BREATH, 6, param, 8, 375};
			abil[eMonstAbil::DAMAGE].gen.dmg = eDamageType::UNBLOCKABLE;
			break;
		case eMonstAbilTemplate::BREATH_ELECTRICITY:
			abil[eMonstAbil::DAMAGE].gen = {true, eMonstGen::BREATH, 8, param, 8, 375};
			abil[eMonstAbil::DAMAGE].gen.dmg = eDamageType::UNBLOCKABLE;
			break;
		case eMonstAbilTemplate::BREATH_DARKNESS:
			abil[eMonstAbil::DAMAGE].gen = {true, eMonstGen::BREATH, 8, param, 8, 375};
			abil[eMonstAbil::DAMAGE].gen.dmg = eDamageType::UNBLOCKABLE;
			break;
		case eMonstAbilTemplate::BREATH_FOUL:
			abil[eMonstAbil::FIELD].gen = {true, eMonstGen::BREATH, 12, PAT_SINGLE, 6, 375};
			abil[eMonstAbil::FIELD].gen.fld = eFieldType::CLOUD_STINK;
			break;
		case eMonstAbilTemplate::BREATH_SLEEP:
			abil[eMonstAbil::FIELD].gen = {true, eMonstGen::BREATH, 0, PAT_RAD2, 8, 750};
			abil[eMonstAbil::FIELD].gen.fld = eFieldType::CLOUD_SLEEP;
			break;
		case eMonstAbilTemplate::SPIT_ACID:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::SPIT, 0, 6, 6, 500};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::ACID;
			break;
		case eMonstAbilTemplate::SHOOTS_WEB:
			abil[eMonstAbil::FIELD].gen = {true, eMonstGen::SPIT, 8, PAT_SINGLE, 4, 375};
			abil[eMonstAbil::FIELD].gen.fld = eFieldType::FIELD_WEB;
			break;
			// Touch abilities
		case eMonstAbilTemplate::TOUCH_POISON:
			abil[eMonstAbil::STATUS2].gen = {true, eMonstGen::TOUCH, -1, param};
			abil[eMonstAbil::STATUS2].gen.stat = eStatus::POISON;
			break;
		case eMonstAbilTemplate::TOUCH_ACID:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, level > 20 ? 4 : 2};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::ACID;
			break;
		case eMonstAbilTemplate::TOUCH_DISEASE:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, 6, 667};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::DISEASE;
			break;
		case eMonstAbilTemplate::TOUCH_WEB:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, 5};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::WEBS;
			break;
		case eMonstAbilTemplate::TOUCH_SLEEP:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, 6};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::ASLEEP;
			break;
		case eMonstAbilTemplate::TOUCH_DUMB:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, 2};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::DUMB;
			break;
		case eMonstAbilTemplate::TOUCH_PARALYSIS:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, 500};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::PARALYZED;
			break;
		case eMonstAbilTemplate::TOUCH_PETRIFY:
			abil[eMonstAbil::PETRIFY].gen = {true, eMonstGen::TOUCH, -1, 25};
			break;
		case eMonstAbilTemplate::TOUCH_DEATH:
			abil[eMonstAbil::KILL].gen = {true, eMonstGen::TOUCH, -1, 2, 667};
			break;
		case eMonstAbilTemplate::TOUCH_ICY:
		case eMonstAbilTemplate::TOUCH_ICY_DRAINING:
			abil[eMonstAbil::DAMAGE].gen = {true, eMonstGen::TOUCH, -1, 3, 667};
			abil[eMonstAbil::DAMAGE].gen.dmg = eDamageType::COLD;
			if(what == eMonstAbilTemplate::TOUCH_ICY) break;
		case eMonstAbilTemplate::TOUCH_XP_DRAIN:
			abil[eMonstAbil::DRAIN_XP].gen = {true, eMonstGen::TOUCH, -1, 150};
			break;
		case eMonstAbilTemplate::TOUCH_STUN:
			abil[eMonstAbil::STUN].gen = {true, eMonstGen::TOUCH, -1, 2, 667};
			abil[eMonstAbil::STUN].gen.stat = eStatus::HASTE_SLOW;
			break;
		case eMonstAbilTemplate::TOUCH_STEAL_FOOD:
			abil[eMonstAbil::STEAL_FOOD].gen = {true, eMonstGen::TOUCH, -1, 10, 667};
			break;
		case eMonstAbilTemplate::TOUCH_STEAL_GOLD:
			abil[eMonstAbil::STEAL_GOLD].gen = {true, eMonstGen::TOUCH, -1, 10, 667};
			break;
			// Misc abilities
		case eMonstAbilTemplate::SPLITS:
			abil[eMonstAbil::SPLITS].special = {true, 100, 0};
			break;
		case eMonstAbilTemplate::MARTYRS_SHIELD:
			abil[eMonstAbil::MARTYRS_SHIELD].special = {true, 0, 0};
			break;
		case eMonstAbilTemplate::ABSORB_SPELLS:
			abil[eMonstAbil::ABSORB_SPELLS].special = {true, 100, 0};
			break;
		case eMonstAbilTemplate::SUMMON_5:
			abil[eMonstAbil::SUMMON].summon = {true, static_cast<mon_num_t>(param), 1, 1, 130, 5};
			break;
		case eMonstAbilTemplate::SUMMON_20:
			abil[eMonstAbil::SUMMON].summon = {true, static_cast<mon_num_t>(param), 1, 1, 130, 20};
			break;
		case eMonstAbilTemplate::SUMMON_50:
			abil[eMonstAbil::SUMMON].summon = {true, static_cast<mon_num_t>(param), 1, 1, 130, 50};
			break;
		case eMonstAbilTemplate::SPECIAL:
			abil[eMonstAbil::SPECIAL].special = {true, param, 1};
			break;
		case eMonstAbilTemplate::DEATH_TRIGGERS:
			abil[eMonstAbil::DEATH_TRIGGER].special = {true, param, 0};
			break;
			// Radiate abilities
		case eMonstAbilTemplate::RADIATE_FIRE:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::WALL_FIRE, param};
			break;
		case eMonstAbilTemplate::RADIATE_ICE:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::WALL_ICE, param};
			break;
		case eMonstAbilTemplate::RADIATE_SHOCK:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::WALL_FORCE, param};
			break;
		case eMonstAbilTemplate::RADIATE_ANTIMAGIC:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::FIELD_ANTIMAGIC, param};
			break;
		case eMonstAbilTemplate::RADIATE_SLEEP:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::CLOUD_SLEEP, param};
			break;
		case eMonstAbilTemplate::RADIATE_STINK:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::CLOUD_STINK, param};
			break;
		case eMonstAbilTemplate::RADIATE_BLADE:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::WALL_BLADES, param};
			break;
		case eMonstAbilTemplate::RADIATE_WEB:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::FIELD_WEB, param};
			break;
			// Advanced abilities
		case eMonstAbilTemplate::CUSTOM_MISSILE:
			abil[eMonstAbil::MISSILE].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_DAMAGE:
			abil[eMonstAbil::DAMAGE].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_STATUS:
			abil[eMonstAbil::STATUS].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_FIELD:
			abil[eMonstAbil::FIELD].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_PETRIFY:
			abil[eMonstAbil::PETRIFY].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_SP_DRAIN:
			abil[eMonstAbil::DRAIN_SP].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_XP_DRAIN:
			abil[eMonstAbil::DRAIN_XP].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_KILL:
			abil[eMonstAbil::KILL].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_STEAL_FOOD:
			abil[eMonstAbil::STEAL_FOOD].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_STEAL_GOLD:
			abil[eMonstAbil::STEAL_GOLD].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_STUN:
			abil[eMonstAbil::STUN].active = true;
			break;
		case eMonstAbilTemplate::CUSTOM_STATUS2:
			abil[eMonstAbil::STATUS2].active = true;
		case eMonstAbilTemplate::CUSTOM_RADIATE:
			abil[eMonstAbil::RADIATE].active = true;
		case eMonstAbilTemplate::CUSTOM_SUMMON:
			abil[eMonstAbil::SUMMON].active = true;
			break;
	}
}

cMonster::cMonster(){
	// TODO: Fill in
	see_str1 = -1;
	see_str2 = -1;
	see_spec = -1;
}

cCreature::cCreature(){
	id = number = active = attitude = start_attitude = 0;
	start_loc.x = start_loc.y = cur_loc.x = cur_loc.y = targ_loc.x = targ_loc.y = 80;
	mobility = 1;
	time_flag = summoned = extra1 = extra2 = 0;
	spec1 = spec2 = spec_enc_code = time_code = monster_time = 0;
	personality = special_on_kill = facial_pic = -1;
	target = 6;
}

cCreature::cCreature(int num) : cCreature() {
	number = num;
}

void cCreature::append(legacy::creature_start_type old){
	number = old.number;
	start_attitude = old.start_attitude;
	start_loc.x = old.start_loc.x;
	start_loc.y = old.start_loc.y;
	mobility = old.mobile;
	time_flag = old.time_flag;
	extra1 = old.extra1;
	extra2 = old.extra2;
	spec1 = old.spec1;
	spec2 = old.spec2;
	spec_enc_code = old.spec_enc_code;
	time_code = old.time_code;
	monster_time = old.monster_time;
	personality = old.personality;
	special_on_kill = old.special_on_kill;
	facial_pic = old.facial_pic;
}

void cCreature::append(legacy::creature_data_type old){
	active = old.active;
	attitude = old.attitude;
	number = old.number;
	cur_loc.x = old.m_loc.x;
	cur_loc.y = old.m_loc.y;
	cMonster::append(old.m_d);
	mobility = old.mobile;
	summoned = old.summoned;
	number = old.monst_start.number;
	start_attitude = old.monst_start.start_attitude;
	start_loc.x = old.monst_start.start_loc.x;
	start_loc.y = old.monst_start.start_loc.y;
	mobility = old.monst_start.mobile;
	time_flag = old.monst_start.time_flag;
	extra1 = old.monst_start.extra1;
	extra2 = old.monst_start.extra2;
	spec1 = old.monst_start.spec1;
	spec2 = old.monst_start.spec2;
	spec_enc_code = old.monst_start.spec_enc_code;
	time_code = old.monst_start.time_code;
	monster_time = old.monst_start.monster_time;
	personality = old.monst_start.personality;
	special_on_kill = old.monst_start.special_on_kill;
	facial_pic = old.monst_start.facial_pic;
	health = old.m_d.health;
	mp = old.m_d.mp;
	max_mp = old.m_d.max_mp;
	ap = old.m_d.ap;
	morale = old.m_d.morale;
	m_morale = old.m_d.m_morale;
	for(int i = 0; i < 15; i++)
		status[(eStatus) i] = old.m_d.status[i];
	direction = eDirection(old.m_d.direction);
}

cMonster::cAttack::operator int() const {
	return dice * 100 + sides;
}

cMonster::cAttack& cMonster::cAttack::operator=(int n){
	dice = n / 100 + 1;
	sides = n % 100;
	return *this;
}

std::ostream& operator<<(std::ostream& out, const cMonster::cAttack& att) {
	out << int(att.dice) << 'd' << int(att.sides);
	return out;
}

std::ostream& operator << (std::ostream& out, eStatus e){
	return out << (int) e;
}

// TODO: This should probably understand symbolic names as well as the numbers?
std::istream& operator >> (std::istream& in, eStatus& e){
	int i;
	in >> i;
	if(i >= 0 && i < 14)
		e = (eStatus) i;
	else e = eStatus::MAIN;
	return in;
}

std::ostream& operator << (std::ostream& out, eRace e){
	return out << (int) e;
}

std::istream& operator >> (std::istream& in, eRace& e){
	int i;
	in >> i;
	if(i > 0 && i < 20)
		e = (eRace) i;
	else e = eRace::HUMAN;
	return in;
}

std::ostream& operator << (std::ostream& out, eDirection e) {
	return out << (int)e;
}

std::istream& operator >> (std::istream& in, eDirection& e) {
	int i;
	in >> i;
	if(i >= 0 && i < 8)
		e = (eDirection)i;
	else e = DIR_HERE;
	return in;
}

std::ostream& operator << (std::ostream& out, eFieldType e) {
	return out << (int)e;
}

std::istream& operator >> (std::istream& in, eFieldType& e) {
	int i;
	in >> i;
	if(i >= 0 && i <= 24)
		e = (eFieldType)i;
	else if(i == 33) e = eFieldType::FIELD_SMASH;
	else e = eFieldType::FIELD_DISPEL;
	return in;
}

std::ostream& operator << (std::ostream& out, eDamageType e) {
	return out << (int)e;
}

std::istream& operator >> (std::istream& in, eDamageType& e) {
	int i;
	in >> i;
	if(i >= 0 && i < 8)
		e = (eDamageType)i;
	else e = eDamageType::MARKED;
	return in;
}

std::ostream& operator << (std::ostream& out, eMonstAbil e) {
	return out << (int)e;
}

std::istream& operator >> (std::istream& in, eMonstAbil& e) {
	int i;
	in >> i;
	if(i > 0 && i <= int(eMonstAbil::SUMMON))
		e = (eMonstAbil)i;
	else e = eMonstAbil::NO_ABIL;
	return in;
}

std::ostream& operator << (std::ostream& out, eMonstGen e) {
	return out << (int)e;
}

std::istream& operator >> (std::istream& in, eMonstGen& e) {
	int i;
	in >> i;
	if(i >= 0 && i <= int(eMonstGen::SPIT))
		e = (eMonstGen)i;
	else e = eMonstGen::TOUCH;
	return in;
}

std::ostream& operator << (std::ostream& out, eMonstMissile e) {
	return out << (int)e;
}

std::istream& operator >> (std::istream& in, eMonstMissile& e) {
	int i;
	in >> i;
	if(i >= 0 && i <= int(eMonstMissile::BOLT))
		e = (eMonstMissile)i;
	else e = eMonstMissile::ARROW;
	return in;
}

std::string uAbility::to_string(eMonstAbil key) const {
	std::ostringstream sout;
	short i = 0;
	switch(getMonstAbilCategory(key)){
		case eMonstAbilCat::INVALID: break;
		case eMonstAbilCat::MISSILE:
			switch(missile.type) {
				case eMonstMissile::DART:
					sout << "Throws darts (" << missile.dice << 'd' << missile.sides << ')';
					break;
				case eMonstMissile::ARROW:
					sout << "Shoots arrows (" << missile.dice << 'd' << missile.sides << ')';
					break;
				case eMonstMissile::BOLT:
					sout << "Shoots bolts (" << missile.dice << 'd' << missile.sides << ')';
					break;
				case eMonstMissile::SPEAR:
					sout << "Throws spears (" << missile.dice << 'd' << missile.sides << ')';
					break;
				case eMonstMissile::ROCK:
					sout << "Throws rocks (" << missile.dice << 'd' << missile.sides << ')';
					break;
				case eMonstMissile::RAZORDISK:
					sout << "Throws razordisks (" << missile.dice << 'd' << missile.sides << ')';
					break;
				case eMonstMissile::SPINE:
					sout << "Shoots spines (" << missile.dice << 'd' << missile.sides << ')';
					break;
				case eMonstMissile::KNIFE:
					sout << "Throws knives (" << missile.dice << 'd' << missile.sides << ')';
					break;
			}
			break;
		case eMonstAbilCat::GENERAL:
			if(key == eMonstAbil::FIELD && gen.type == eMonstGen::SPIT && gen.fld == eFieldType::FIELD_WEB) {
				sout << "Throws webs";
			} else {
				switch(key) {
					case eMonstAbil::STUN: sout << "Stunning"; break;
					case eMonstAbil::PETRIFY: sout << "Petrifying"; break;
					case eMonstAbil::DRAIN_SP: sout << "Spell point drain"; break;
					case eMonstAbil::DRAIN_XP: sout << "Experience drain"; break;
					case eMonstAbil::KILL: sout << "Death"; break;
					case eMonstAbil::STEAL_FOOD: sout << "Steals food"; break;
					case eMonstAbil::STEAL_GOLD: sout << "Steals gold!"; break;
					case eMonstAbil::FIELD:
						switch(gen.fld) {
							case eFieldType::CLOUD_SLEEP: sout << "Sleep"; break;
							case eFieldType::CLOUD_STINK: sout << "Foul"; break;
							case eFieldType::WALL_FIRE: sout << "Fiery"; break;
							case eFieldType::WALL_FORCE: sout << "Charged"; break;
							case eFieldType::WALL_ICE: sout << "Frosted"; break;
							case eFieldType::WALL_BLADES: sout << "Thorny"; break;
							case eFieldType::FIELD_ANTIMAGIC: sout << "Null"; break;
							case eFieldType::FIELD_WEB: sout << "Web"; break;
							case eFieldType::FIELD_QUICKFIRE: sout << "Incendiary"; break;
						}
						break;
					case eMonstAbil::DAMAGE:
						switch(gen.dmg) {
							case eDamageType::FIRE: sout << "Heat"; break;
							case eDamageType::COLD: sout << "Icy"; break;
							case eDamageType::MAGIC: sout << "Shock"; break;
							case eDamageType::UNBLOCKABLE: sout << "Wounding"; break;
							case eDamageType::POISON: sout << "Pain"; break;
							case eDamageType::WEAPON: sout << "Stamina drain"; break;
							case eDamageType::DEMON: sout << "Unholy"; break;
							case eDamageType::UNDEAD: sout << "Necrotic"; break;
						}
						break;
					case eMonstAbil::STATUS: case eMonstAbil::STATUS2:
						switch(gen.stat) {
							case eStatus::POISON: sout << "Poison"; break;
							case eStatus::DISEASE: sout << "Infectious"; break;
							case eStatus::DUMB: sout << "Dumbfounding"; break;
							case eStatus::WEBS: sout << "Glue"; break;
							case eStatus::ASLEEP: sout << "Sleep"; break;
							case eStatus::PARALYZED: sout << "Paralysis"; break;
							case eStatus::ACID: sout << "Acid"; break;
							case eStatus::HASTE_SLOW: sout << "Slowing"; break;
							case eStatus::BLESS_CURSE: sout << "Curse"; break;
						}
						break;
				}
				switch(gen.type) {
					case eMonstGen::RAY: sout << " ray"; break;
					case eMonstGen::TOUCH: sout << " touch"; break;
					case eMonstGen::GAZE: sout << " gaze"; break;
					case eMonstGen::BREATH: sout << " breath"; break;
					case eMonstGen::SPIT: sout << " spit"; break;
				}
			}
			if(key == eMonstAbil::DAMAGE) {
				sout << " (" << gen.strength << ")";
			} else if(key == eMonstAbil::STATUS || key == eMonstAbil::STATUS2 || key == eMonstAbil::STUN) {
				sout << " (" << gen.strength;
				switch(gen.type) {
					case eMonstGen::RAY: sout << "d6"; break;
					case eMonstGen::TOUCH: sout << "d10"; break;
					case eMonstGen::GAZE: sout << "d6"; break;
					case eMonstGen::BREATH: sout << "d8"; break;
					case eMonstGen::SPIT: sout << "d10"; break;
				}
				sout << ")";
			} else if(key == eMonstAbil::FIELD && gen.strength != PAT_SINGLE) {
				sout << " (";
				switch(eSpellPat(gen.strength)) {
					case PAT_SINGLE: break;
					case PAT_SMSQ: sout << "small ";
					case PAT_SQ: sout << "square"; break;
					case PAT_OPENSQ: sout << "open square"; break;
					case PAT_PLUS: sout << "plus"; break;
					case PAT_RAD2: sout << "small circle"; break;
					case PAT_RAD3: sout << "big circle"; break;
					case PAT_WALL: sout << "line"; break;
				}
				sout << ")";
			} else if(key == eMonstAbil::KILL) {
				sout << " (" << gen.strength * 20 << "d10)";
			} else if(key == eMonstAbil::STEAL_FOOD || key == eMonstAbil::STEAL_GOLD) {
				sout << " (" << gen.strength << '-' << gen.strength * 2 << ")";
			}
			break;
		case eMonstAbilCat::SPECIAL:
			switch(key) {
				case eMonstAbil::SPLITS:
					sout << "Splits when hit (" << special.extra1 << "% chance)";
					break;
				case eMonstAbil::MARTYRS_SHIELD:
					sout << "Permanent martyr's shield";
					break;
				case eMonstAbil::ABSORB_SPELLS:
					sout << "Absorbs spells";
					break;
				case eMonstAbil::SPECIAL:
				case eMonstAbil::DEATH_TRIGGER:
					sout << "Unusual ability";
					break;
			}
			break;
		case eMonstAbilCat::SUMMON:
			// TODO: Somehow look up the name of the monster to be summoned
			sout << "Summons " << summon.min << '-' << summon.max << ' ' << "monst-name" << "s (" << summon.chance << "% chance)";
			break;
		case eMonstAbilCat::RADIATE:
			sout << "Radiates ";
			switch(radiate.type) {
				case eFieldType::WALL_BLADES: sout << "blade fields"; break;
				case eFieldType::WALL_FIRE: sout << "fire fields"; break;
				case eFieldType::WALL_FORCE: sout << "shock fields"; break;
				case eFieldType::WALL_ICE: sout << "ice fields"; break;
				case eFieldType::CLOUD_STINK: sout << "stinking clouds"; break;
				case eFieldType::CLOUD_SLEEP: sout << "sleep fields"; break;
				case eFieldType::FIELD_ANTIMAGIC: sout << "antimagic fields"; break;
				case eFieldType::FIELD_WEB: sout << "webs"; break;
			}
			break;
	}
	return sout.str();
}

void cMonster::writeTo(std::ostream& file) const {
	file << "MONSTER " << maybe_quote_string(m_name) << '\n';
	file << "LEVEL " << int(level) << '\n';
	file << "ARMOR " << int(armor) << '\n';
	file << "SKILL " << int(skill) << '\n';
	for(int i = 0; i < 3; i++)
		file << "ATTACK " << i + 1 << ' ' << int(a[i].dice) << ' ' << int(a[i].sides) << ' ' << int(a[i].type) << '\n';
	file << "HEALTH " << int(m_health) << '\n';
	file << "SPEED " << int(speed) << '\n';
	file << "MAGE " << int(mu) << '\n';
	file << "PRIEST " << int(cl) << '\n';
	file << "RACE " << m_type << '\n';
	file << "TREASURE" << int(treasure) << '\n';
	file << "CORPSEITEM " << corpse_item << ' ' << corpse_item_chance << '\n';
	file << "IMMUNE " << magic_res << '\t' << fire_res << '\t' << cold_res << '\t' << poison_res << '\n';
	file << "SIZE " << int(x_width) << ' ' << int(y_width) << '\n';
	file << "ATTITUDE " << int(default_attitude) << '\n';
	file << "SUMMON " << int(summon_type) << '\n';
	file << "PORTRAIT " << default_facial_pic << '\n';
	file << "PICTURE " << picture_num << '\n';
	file << "SOUND " << ambient_sound << '\n';
	file << '\f';
	for(auto& abil : abil) {
		if(!abil.second.active || abil.first == eMonstAbil::NO_ABIL) continue;
		file << "ABIL " << abil.first << '\n';
		switch(getMonstAbilCategory(abil.first)) {
			case eMonstAbilCat::INVALID: continue;
			case eMonstAbilCat::MISSILE:
				file << "TYPE " << abil.second.missile.type << ' ' << abil.second.missile.pic << '\n';
				file << "DAMAGE " << abil.second.missile.dice << ' ' << abil.second.missile.sides << '\n';
				file << "SKILL " << abil.second.missile.skill << '\n';
				file << "RANGE " << abil.second.missile.range << '\n';
				file << "CHANCE " << abil.second.missile.odds << '\n';
				break;
			case eMonstAbilCat::GENERAL:
				file << "TYPE " << abil.second.gen.type << ' ' << abil.second.gen.pic << '\n';
				file << "DAMAGE " << abil.second.gen.strength << '\n';
				file << "RANGE " << abil.second.gen.range << '\n';
				file << "CHANCE " << abil.second.gen.odds << '\n';
				if(abil.first == eMonstAbil::DAMAGE)
					file << "EXTRA " << abil.second.gen.dmg << '\n';
				else if(abil.first == eMonstAbil::FIELD)
					file << "EXTRA " << abil.second.gen.fld << '\n';
				else if(abil.first == eMonstAbil::STATUS || abil.first == eMonstAbil::STATUS2 || abil.first == eMonstAbil::STUN)
					file << "EXTRA " << abil.second.gen.stat;
				break;
			case eMonstAbilCat::RADIATE:
				file << "TYPE " << abil.second.radiate.type << '\n';
				file << "CHANCE " << abil.second.radiate.chance << '\n';
				break;
			case eMonstAbilCat::SUMMON:
				file << "TYPE " << abil.second.summon.type << '\n';
				file << "HOWMANY " << abil.second.summon.min << ' ' << abil.second.summon.max << '\n';
				file << "CHANCE " << abil.second.summon.chance << '\n';
				break;
			case eMonstAbilCat::SPECIAL:
				file << "EXTRA " << abil.second.special.extra1 << ' ' << abil.second.special.extra2 << '\n';
				break;
		}
		file << '\f';
	}
}

void cMonster::readFrom(std::istream& file) {
	// On-see event is not exported, so make sure the fields are not filled with garbage data
	see_sound = 0;
	see_str1 = -1;
	see_str2 = -1;
	see_spec = -1;
	std::istringstream bin;
	std::string cur;
	getline(file, cur, '\f');
	bin.str(cur);
	while(bin) {
		int temp1, temp2, temp3;
		getline(bin, cur);
		std::istringstream line(cur);
		line >> cur;
		if(cur == "MONSTER") {
			line >> std::ws;
			m_name = read_maybe_quoted_string(line);
		} else if(cur == "ATTACK") {
			int which;
			line >> which >> temp1 >> temp2 >> temp3;
			which--;
			a[which].dice = temp1;
			a[which].sides = temp2;
			a[which].type = temp3;
		} else if(cur == "SIZE") {
			line >> temp1 >> temp2;
			x_width = temp1;
			y_width = temp2;
		} else if(cur == "IMMUNE") {
			line >> temp1 >> temp2;
			magic_res = temp1;
			fire_res = temp2;
			line >> temp1 >> temp2;
			cold_res = temp1;
			poison_res = temp2;
		} else if(cur == "RACE")
			line >> m_type;
		else if(cur == "CORPSEITEM")
			line >> corpse_item >> corpse_item_chance;
		else if(cur == "PORTRAIT")
			line >> default_facial_pic;
		else if(cur == "PICTURE")
			line >> picture_num;
		else if(cur == "SOUND")
			line >> ambient_sound;
		else {
			line >> temp1;
			if(cur == "LEVEL")
				level = temp1;
			else if (cur == "ARMOR")
				armor = temp1;
			else if(cur == "SKILL")
				skill = temp1;
			else if(cur == "HEALTH")
				m_health = temp1;
			else if(cur == "SPEED")
				speed = temp1;
			else if(cur == "MAGE")
				mu = temp1;
			else if(cur == "PRIEST")
				cl = temp1;
			else if(cur == "TREASURE")
				treasure = temp1;
			else if(cur == "ATTITUDE")
				default_attitude = temp1;
			else if(cur == "SUMMON")
				summon_type = temp1;
		}
	}
	while(file) {
		getline(file, cur, '\f');
		bin.str(cur);
		getline(bin, cur);
		std::istringstream line(cur);
		line >> cur;
		if(cur == "ABIL") {
			eMonstAbil key;
			uAbility abil;
			line >> key;
			eMonstAbilCat cat = getMonstAbilCategory(key);
			if(cat == eMonstAbilCat::INVALID) continue;
			while(bin) {
				getline(bin, cur);
				line.str(cur);
				line >> cur;
				if(cur == "TYPE") {
					if(cat == eMonstAbilCat::MISSILE)
						line >> abil.missile.type >> abil.missile.pic;
					else if(cat == eMonstAbilCat::GENERAL)
						line >> abil.gen.type >> abil.gen.pic;
					else if(cat == eMonstAbilCat::RADIATE)
						line >> abil.radiate.type;
					else if(cat == eMonstAbilCat::SUMMON)
						line >> abil.summon.type;
				} else if(cur == "DAMAGE") {
					if(cat == eMonstAbilCat::MISSILE)
						line >> abil.missile.dice >> abil.missile.sides;
					else if(cat == eMonstAbilCat::GENERAL)
						line >> abil.gen.strength;
				} else if(cur == "SKILL") {
					if(cat == eMonstAbilCat::MISSILE)
						line >> abil.missile.skill;
				} else if(cur == "RANGE") {
					if(cat == eMonstAbilCat::MISSILE)
						line >> abil.missile.range;
					else if(cat == eMonstAbilCat::GENERAL)
						line >> abil.gen.range;
				} else if(cur == "CHANCE") {
					if(cat == eMonstAbilCat::MISSILE)
						line >> abil.missile.odds;
					else if(cat == eMonstAbilCat::GENERAL)
						line >> abil.gen.odds;
					else if(cat == eMonstAbilCat::RADIATE)
						line >> abil.radiate.chance;
					else if(cat == eMonstAbilCat::SUMMON)
						line >> abil.summon.chance;
				} else if(cur == "EXTRA") {
					if(key == eMonstAbil::DAMAGE)
						line >> abil.gen.dmg;
					else if(key == eMonstAbil::FIELD)
						line >> abil.gen.fld;
					else if(key == eMonstAbil::STATUS || key == eMonstAbil::STATUS2 || key == eMonstAbil::STUN)
						line >> abil.gen.stat;
					else if(cat == eMonstAbilCat::SPECIAL)
						line >> abil.special.extra1 >> abil.special.extra2;
				} else if(cur == "HOWMANY") {
					if(cat == eMonstAbilCat::SUMMON)
						line >> abil.summon.min >> abil.summon.max;
				}
			}
		}
	}
}

void cCreature::writeTo(std::ostream& file) const {
	file << "MONSTER " << number << '\n';
	file << "ATTITUDE " << attitude << '\n';
	file << "STARTATT " << unsigned(start_attitude) << '\n';
	file << "STARTLOC " << start_loc.x << ' ' << start_loc.y << '\n';
	file << "LOCATION " << cur_loc.x << ' ' << cur_loc.y << '\n';
	file << "MOBILITY " << unsigned(mobility) << '\n';
	file << "TIMEFLAG " << unsigned(time_flag) << '\n';
	file << "SUMMONED " << summoned << '\n';
	// TODO: Don't remember what these do
	file << "EXTRA " << unsigned(extra1) << ' ' << unsigned(extra2) << '\n';
	file << "SPEC " << spec1 << ' ' << spec2 << '\n';
	file << "SPECCODE " << int(spec_enc_code) << '\n';
	file << "TIMECODE " << int(time_code) << '\n';
	file << "TIME " << monster_time << '\n';
	file << "TALK " << personality << '\n';
	file << "DEATH " << special_on_kill << '\n';
	file << "FACE " << facial_pic << '\n';
	file << "TARGET " << target << '\n';
	file << "TARGLOC " << targ_loc.x << ' ' << targ_loc.y << '\n';
	for(auto stat : status) {
		if(stat.second != 0)
			file << "STATUS " << stat.first << ' ' << stat.second << '\n';
	}
	file << "CURHP " << health << '\n';
	file << "CURSP " << mp << '\n';
	file << "MORALE " << morale << '\n';
	file << "DIRECTION " << unsigned(direction) << '\n';
}

void cCreature::readFrom(std::istream& file) {
	while(file) {
		std::string cur;
		getline(file, cur);
		std::cout << "Parsing line in town.txt: " << cur << std::endl;
		std::istringstream line(cur);
		line >> cur;
		if(cur == "MONSTER")
			line >> number;
		else if(cur == "ATTITUDE")
			line >> attitude;
		else if(cur == "STARTATT") {
			unsigned int i;
			line >> i;
			start_attitude = i;
		} else if(cur == "STARTLOC")
			line >> start_loc.x >> start_loc.y;
		else if(cur == "LOCATION")
			line >> cur_loc.x >> cur_loc.y;
		else if(cur == "MOBILITY") {
			unsigned int i;
			line >> i;
			mobility = i;
		} else if(cur == "TIMEFLAG") {
			unsigned int i;
			line >> i;
			time_flag = i;
		} else if(cur == "SUMMONED")
			line >> summoned;
		else if(cur == "EXTRA") {
			unsigned int i,j;
			line >> i >> j;
			extra1 = i;
			extra2 = j;
		} else if(cur == "SPEC")
			line >> spec1 >> spec2;
		else if(cur == "SPECCODE") {
			int i;
			line >> i;
			spec_enc_code = i;
		} else if(cur == "TIMECODE") {
			int i;
			line >> i;
			time_code = i;
		} else if(cur == "TIME")
			line >> monster_time;
		else if(cur == "TALK")
			line >> personality;
		else if(cur == "DEATH")
			line >> special_on_kill;
		else if(cur == "FACE")
			line >> facial_pic;
		else if(cur == "TARGET")
			line >> target;
		else if(cur == "TARGLOC")
			line >> targ_loc.x >> targ_loc.y;
		else if(cur == "CURHP")
			line >> health;
		else if(cur == "CURSP")
			line >> mp;
		else if(cur == "MORALE")
			line >> morale;
		else if(cur == "DIRECTION")
			line >> direction;
		else if(cur == "STATUS") {
			eStatus i;
			line >> i >> status[i];
		}
	}
}
