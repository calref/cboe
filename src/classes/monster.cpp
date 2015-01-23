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
#include <boost/lexical_cast.hpp>

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
	for(int i = 0; i < 3; i++) {
		a[i].dice = old.a[i] / 100;
		a[i].sides = old.a[i] % 100;
	}
	a[0].type = eMonstMelee(old.a1_type);
	a[1].type = a[2].type = eMonstMelee(old.a23_type);
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
		magic_res = 0;
	else if(old.immunities & 1)
		magic_res = 50;
	else magic_res = 100;
	if(old.immunities & 8)
		fire_res = 0;
	else if(old.immunities & 4)
		fire_res = 50;
	else fire_res = 100;
	if(old.immunities & 32)
		cold_res = 0;
	else if(old.immunities & 16)
		cold_res = 50;
	else cold_res = 100;
	if(old.immunities & 128)
		poison_res = 0;
	else if(old.immunities & 64)
		poison_res = 50;
	else poison_res = 100;
	x_width = old.x_width;
	y_width = old.y_width;
	default_attitude = old.default_attitude;
	summon_type = old.summon_type;
	default_facial_pic = old.default_facial_pic;
	if(default_facial_pic == 0)
		default_facial_pic = NO_PIC;
	picture_num = old.picture_num;
	if(picture_num == 122) picture_num = 119;
	see_spec = -1;
	ambient_sound = 0;
}

std::map<eMonstAbil,uAbility>::iterator cMonster::addAbil(eMonstAbilTemplate what, int param) {
	switch(what) {
		// Missiles: {true, type, missile pic, dice, sides, skill, range, odds}
		case eMonstAbilTemplate::THROWS_DARTS:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::DART, 1, 1, 7, 2, 6, 500};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::SHOOTS_ARROWS:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ARROW, 3, 2, 7, 4, 8, 750};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::THROWS_SPEARS:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::SPEAR, 5, 3, 7, 6, 8, 625};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::THROWS_ROCKS1:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ROCK, 12, 4, 7, 8, 10, 625};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::THROWS_ROCKS2:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ROCK, 12, 6, 7, 12, 10, 500};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::THROWS_ROCKS3:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ROCK, 12, 8, 7, 16, 10, 500};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::THROWS_RAZORDISKS:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::RAZORDISK, 7, 7, 7, 14, 8, 625};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::THROWS_KNIVES:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::KNIFE, 10, 2, 7, 2, 6, 500};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::GOOD_ARCHER:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ARROW, 3, 8, 7, 16, 10, 875};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::SHOOTS_SPINES:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::SPINE, 5, 6, 7, 12, 9, 625};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::CROSSBOWMAN:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::BOLT, 3, 10, 7, 16, 10, 875};
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::SLINGER:
			abil[eMonstAbil::MISSILE].missile = {true, eMonstMissile::ROCK, 12, 2, 7, 3, 8, 750};
			return abil.find(eMonstAbil::MISSILE);
		// Magical missiles: {true, type, missile pic, strength, range, odds}
		case eMonstAbilTemplate::RAY_PETRIFY:
			abil[eMonstAbil::PETRIFY].gen = {true, eMonstGen::GAZE, -1, 25, 6, 625};
			return abil.find(eMonstAbil::PETRIFY);
		case eMonstAbilTemplate::RAY_SP_DRAIN:
			abil[eMonstAbil::DRAIN_SP].gen = {true, eMonstGen::GAZE, 8, 50, 8, 625};
			return abil.find(eMonstAbil::DRAIN_SP);
		case eMonstAbilTemplate::RAY_HEAT:
			abil[eMonstAbil::RAY_HEAT].special = {true, 6, 625, 7};
			return abil.find(eMonstAbil::RAY_HEAT);
		case eMonstAbilTemplate::RAY_PARALYSIS:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::RAY, -1, 100, 6, 750};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::PARALYZED;
			return abil.find(eMonstAbil::STATUS);
		case eMonstAbilTemplate::BREATH_FIRE:
			abil[eMonstAbil::DAMAGE2].gen = {true, eMonstGen::BREATH, 13, param, 8, 375};
			abil[eMonstAbil::DAMAGE2].gen.dmg = eDamageType::FIRE;
			return abil.find(eMonstAbil::DAMAGE2);
		case eMonstAbilTemplate::BREATH_FROST:
			abil[eMonstAbil::DAMAGE2].gen = {true, eMonstGen::BREATH, 6, param, 8, 375};
			abil[eMonstAbil::DAMAGE2].gen.dmg = eDamageType::COLD;
			return abil.find(eMonstAbil::DAMAGE2);
		case eMonstAbilTemplate::BREATH_ELECTRICITY:
			abil[eMonstAbil::DAMAGE2].gen = {true, eMonstGen::BREATH, 8, param, 8, 375};
			abil[eMonstAbil::DAMAGE2].gen.dmg = eDamageType::MAGIC;
			return abil.find(eMonstAbil::DAMAGE2);
		case eMonstAbilTemplate::BREATH_DARKNESS:
			abil[eMonstAbil::DAMAGE2].gen = {true, eMonstGen::BREATH, 8, param, 8, 375};
			abil[eMonstAbil::DAMAGE2].gen.dmg = eDamageType::UNBLOCKABLE;
			return abil.find(eMonstAbil::DAMAGE2);
		case eMonstAbilTemplate::BREATH_FOUL:
			abil[eMonstAbil::FIELD].gen = {true, eMonstGen::BREATH, 12, PAT_SINGLE, 6, 375};
			abil[eMonstAbil::FIELD].gen.fld = eFieldType::CLOUD_STINK;
			return abil.find(eMonstAbil::FIELD);
		case eMonstAbilTemplate::BREATH_SLEEP:
			abil[eMonstAbil::FIELD].gen = {true, eMonstGen::BREATH, 0, PAT_RAD2, 8, 750};
			abil[eMonstAbil::FIELD].gen.fld = eFieldType::CLOUD_SLEEP;
			return abil.find(eMonstAbil::FIELD);
		case eMonstAbilTemplate::SPIT_ACID:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::SPIT, 0, 6, 6, 500};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::ACID;
			return abil.find(eMonstAbil::STATUS);
		case eMonstAbilTemplate::SHOOTS_WEB:
			abil[eMonstAbil::MISSILE_WEB].special = {true, 4, 375};
			return abil.find(eMonstAbil::MISSILE_WEB);
			// Touch abilities
		case eMonstAbilTemplate::TOUCH_POISON:
			abil[eMonstAbil::STATUS2].gen = {true, eMonstGen::TOUCH, -1, param, 0, 1000};
			abil[eMonstAbil::STATUS2].gen.stat = eStatus::POISON;
			return abil.find(eMonstAbil::STATUS2);
		case eMonstAbilTemplate::TOUCH_ACID:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, level > 20 ? 4 : 2, 0, 1000};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::ACID;
			return abil.find(eMonstAbil::STATUS);
		case eMonstAbilTemplate::TOUCH_DISEASE:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, 6, 0, 667};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::DISEASE;
			return abil.find(eMonstAbil::STATUS);
		case eMonstAbilTemplate::TOUCH_WEB:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, 5, 0, 1000};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::WEBS;
			return abil.find(eMonstAbil::STATUS);
		case eMonstAbilTemplate::TOUCH_SLEEP:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, 6, 0, 1000};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::ASLEEP;
			return abil.find(eMonstAbil::STATUS);
		case eMonstAbilTemplate::TOUCH_DUMB:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, 2, 0, 1000};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::DUMB;
			return abil.find(eMonstAbil::STATUS);
		case eMonstAbilTemplate::TOUCH_PARALYSIS:
			abil[eMonstAbil::STATUS].gen = {true, eMonstGen::TOUCH, -1, 0, 500};
			abil[eMonstAbil::STATUS].gen.stat = eStatus::PARALYZED;
			return abil.find(eMonstAbil::STATUS);
		case eMonstAbilTemplate::TOUCH_PETRIFY:
			abil[eMonstAbil::PETRIFY].gen = {true, eMonstGen::TOUCH, -1, 0, 25};
			return abil.find(eMonstAbil::PETRIFY);
		case eMonstAbilTemplate::TOUCH_DEATH:
			abil[eMonstAbil::KILL].gen = {true, eMonstGen::TOUCH, -1, 2, 0, 667};
			return abil.find(eMonstAbil::KILL);
		case eMonstAbilTemplate::TOUCH_ICY:
		case eMonstAbilTemplate::TOUCH_ICY_DRAINING:
			abil[eMonstAbil::DAMAGE].gen = {true, eMonstGen::TOUCH, -1, 3, 0, 667};
			abil[eMonstAbil::DAMAGE].gen.dmg = eDamageType::COLD;
			if(what == eMonstAbilTemplate::TOUCH_ICY)
				return abil.find(eMonstAbil::DAMAGE);
		case eMonstAbilTemplate::TOUCH_XP_DRAIN:
			abil[eMonstAbil::DRAIN_XP].gen = {true, eMonstGen::TOUCH, -1, 0, 150};
			return abil.find(eMonstAbil::DRAIN_XP);
		case eMonstAbilTemplate::TOUCH_STUN:
			abil[eMonstAbil::STUN].gen = {true, eMonstGen::TOUCH, -1, 2, 0, 667};
			abil[eMonstAbil::STUN].gen.stat = eStatus::HASTE_SLOW;
			return abil.find(eMonstAbil::STUN);
		case eMonstAbilTemplate::TOUCH_STEAL_FOOD:
			abil[eMonstAbil::STEAL_FOOD].gen = {true, eMonstGen::TOUCH, -1, 10, 0, 667};
			return abil.find(eMonstAbil::STEAL_FOOD);
		case eMonstAbilTemplate::TOUCH_STEAL_GOLD:
			abil[eMonstAbil::STEAL_GOLD].gen = {true, eMonstGen::TOUCH, -1, 10, 0, 667};
			return abil.find(eMonstAbil::STEAL_GOLD);
			// Misc abilities
		case eMonstAbilTemplate::SPLITS:
			abil[eMonstAbil::SPLITS].special = {true, 1000, 0, 0};
			return abil.find(eMonstAbil::SPLITS);
		case eMonstAbilTemplate::MARTYRS_SHIELD:
			abil[eMonstAbil::MARTYRS_SHIELD].special = {true, 1000, 100, 0};
			return abil.find(eMonstAbil::MARTYRS_SHIELD);
		case eMonstAbilTemplate::ABSORB_SPELLS:
			abil[eMonstAbil::ABSORB_SPELLS].special = {true, 1000, 3, 0};
			return abil.find(eMonstAbil::ABSORB_SPELLS);
		case eMonstAbilTemplate::SUMMON_5:
			abil[eMonstAbil::SUMMON].summon = {true, eMonstSummon::TYPE, static_cast<mon_num_t>(param), 1, 1, 130, 5};
			return abil.find(eMonstAbil::SUMMON);
		case eMonstAbilTemplate::SUMMON_20:
			abil[eMonstAbil::SUMMON].summon = {true, eMonstSummon::TYPE, static_cast<mon_num_t>(param), 1, 1, 130, 20};
			return abil.find(eMonstAbil::SUMMON);
		case eMonstAbilTemplate::SUMMON_50:
			abil[eMonstAbil::SUMMON].summon = {true, eMonstSummon::TYPE, static_cast<mon_num_t>(param), 1, 1, 130, 50};
			return abil.find(eMonstAbil::SUMMON);
		case eMonstAbilTemplate::SPECIAL:
			abil[eMonstAbil::SPECIAL].special = {true, param, 1};
			return abil.find(eMonstAbil::SPECIAL);
		case eMonstAbilTemplate::HIT_TRIGGERS:
			abil[eMonstAbil::HIT_TRIGGER].special = {true, param, 0};
			return abil.find(eMonstAbil::HIT_TRIGGER);
		case eMonstAbilTemplate::DEATH_TRIGGERS:
			abil[eMonstAbil::DEATH_TRIGGER].special = {true, param, 0};
			return abil.find(eMonstAbil::DEATH_TRIGGER);
			// Radiate abilities
		case eMonstAbilTemplate::RADIATE_FIRE:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::WALL_FIRE, param};
			return abil.find(eMonstAbil::RADIATE);
		case eMonstAbilTemplate::RADIATE_ICE:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::WALL_ICE, param};
			return abil.find(eMonstAbil::RADIATE);
		case eMonstAbilTemplate::RADIATE_SHOCK:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::WALL_FORCE, param};
			return abil.find(eMonstAbil::RADIATE);
		case eMonstAbilTemplate::RADIATE_ANTIMAGIC:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::FIELD_ANTIMAGIC, param};
			return abil.find(eMonstAbil::RADIATE);
		case eMonstAbilTemplate::RADIATE_SLEEP:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::CLOUD_SLEEP, param};
			return abil.find(eMonstAbil::RADIATE);
		case eMonstAbilTemplate::RADIATE_STINK:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::CLOUD_STINK, param};
			return abil.find(eMonstAbil::RADIATE);
		case eMonstAbilTemplate::RADIATE_BLADE:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::WALL_BLADES, param};
			return abil.find(eMonstAbil::RADIATE);
		case eMonstAbilTemplate::RADIATE_WEB:
			abil[eMonstAbil::RADIATE].radiate = {true, eFieldType::FIELD_WEB, param};
			return abil.find(eMonstAbil::RADIATE);
			// Advanced abilities
		case eMonstAbilTemplate::CUSTOM_MISSILE:
			abil[eMonstAbil::MISSILE].active = true;
			return abil.find(eMonstAbil::MISSILE);
		case eMonstAbilTemplate::CUSTOM_DAMAGE:
			abil[eMonstAbil::DAMAGE].active = true;
			return abil.find(eMonstAbil::DAMAGE);
		case eMonstAbilTemplate::CUSTOM_DAMAGE2:
			abil[eMonstAbil::DAMAGE2].active = true;
			return abil.find(eMonstAbil::DAMAGE2);
		case eMonstAbilTemplate::CUSTOM_STATUS:
			abil[eMonstAbil::STATUS].active = true;
			return abil.find(eMonstAbil::STATUS);
		case eMonstAbilTemplate::CUSTOM_FIELD:
			abil[eMonstAbil::FIELD].active = true;
			return abil.find(eMonstAbil::FIELD);
		case eMonstAbilTemplate::CUSTOM_PETRIFY:
			abil[eMonstAbil::PETRIFY].active = true;
			return abil.find(eMonstAbil::PETRIFY);
		case eMonstAbilTemplate::CUSTOM_SP_DRAIN:
			abil[eMonstAbil::DRAIN_SP].active = true;
			return abil.find(eMonstAbil::DRAIN_SP);
		case eMonstAbilTemplate::CUSTOM_XP_DRAIN:
			abil[eMonstAbil::DRAIN_XP].active = true;
			return abil.find(eMonstAbil::DRAIN_XP);
		case eMonstAbilTemplate::CUSTOM_KILL:
			abil[eMonstAbil::KILL].active = true;
			return abil.find(eMonstAbil::KILL);
		case eMonstAbilTemplate::CUSTOM_STEAL_FOOD:
			abil[eMonstAbil::STEAL_FOOD].active = true;
			return abil.find(eMonstAbil::STEAL_FOOD);
		case eMonstAbilTemplate::CUSTOM_STEAL_GOLD:
			abil[eMonstAbil::STEAL_GOLD].active = true;
			return abil.find(eMonstAbil::STEAL_GOLD);
		case eMonstAbilTemplate::CUSTOM_STUN:
			abil[eMonstAbil::STUN].active = true;
			return abil.find(eMonstAbil::STUN);
		case eMonstAbilTemplate::CUSTOM_STATUS2:
			abil[eMonstAbil::STATUS2].active = true;
			return abil.find(eMonstAbil::STATUS2);
		case eMonstAbilTemplate::CUSTOM_RADIATE:
			abil[eMonstAbil::RADIATE].active = true;
			return abil.find(eMonstAbil::RADIATE);
		case eMonstAbilTemplate::CUSTOM_SUMMON:
			abil[eMonstAbil::SUMMON].active = true;
			return abil.find(eMonstAbil::SUMMON);
	}
	return abil.end();
}

cMonster::cMonster(){
	magic_res = poison_res = fire_res = cold_res = 100;
	// TODO: Fill in
	see_spec = -1;
}

cCreature::cCreature(){
	number = active = attitude = start_attitude = 0;
	start_loc.x = start_loc.y = cur_loc.x = cur_loc.y = targ_loc.x = targ_loc.y = 80;
	mobility = 1;
	summoned  = 0;
	time_flag = eMonstTime::ALWAYS;
	spec1 = spec2 = spec_enc_code = time_code = monster_time = 0;
	personality = special_on_kill = facial_pic = -1;
	target = 6;
}

cCreature::cCreature(int num) : cCreature() {
	number = num;
}

cTownperson::cTownperson() {
	start_loc = {80,80};
	number = 0;
	mobility = 1;
	time_flag = eMonstTime::ALWAYS;
	spec1 = -1;
	spec2 = -1;
	spec_enc_code = 0;
	time_code = 0;
	monster_time = 0;
	personality = -1;
	special_on_kill = -1;
}

cTownperson::cTownperson(location loc, mon_num_t num, const cMonster& monst) : cTownperson() {
	start_loc = loc;
	number = num;
	start_attitude = monst.default_attitude;
	facial_pic = monst.default_facial_pic;
}

void cTownperson::append(legacy::creature_start_type old){
	number = old.number;
	start_attitude = old.start_attitude;
	start_loc.x = old.start_loc.x;
	start_loc.y = old.start_loc.y;
	mobility = old.mobile;
	switch(old.time_flag) {
		case 0: time_flag = eMonstTime::ALWAYS; break;
		case 1: time_flag = eMonstTime::APPEAR_ON_DAY; break;
		case 2: time_flag = eMonstTime::DISAPPEAR_ON_DAY; break;
		case 4: time_flag = eMonstTime::SOMETIMES_A; break;
		case 5: time_flag = eMonstTime::SOMETIMES_B; break;
		case 6: time_flag = eMonstTime::SOMETIMES_C; break;
		case 7: time_flag = eMonstTime::APPEAR_WHEN_EVENT; break;
		case 8: time_flag = eMonstTime::DISAPPEAR_WHEN_EVENT; break;
	}
	spec1 = old.spec1;
	spec2 = old.spec2;
	spec_enc_code = old.spec_enc_code;
	time_code = old.time_code;
	monster_time = old.monster_time;
	personality = old.personality;
	special_on_kill = old.special_on_kill;
	facial_pic = old.facial_pic;
	if(facial_pic == 0)
		facial_pic = NO_PIC;
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
	switch(old.monst_start.time_flag) {
		case 0: time_flag = eMonstTime::ALWAYS; break;
		case 1: time_flag = eMonstTime::APPEAR_ON_DAY; break;
		case 2: time_flag = eMonstTime::DISAPPEAR_ON_DAY; break;
		case 4: time_flag = eMonstTime::SOMETIMES_A; break;
		case 5: time_flag = eMonstTime::SOMETIMES_B; break;
		case 6: time_flag = eMonstTime::SOMETIMES_C; break;
		case 7: time_flag = eMonstTime::APPEAR_WHEN_EVENT; break;
		case 8: time_flag = eMonstTime::DISAPPEAR_WHEN_EVENT; break;
	}
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
	switch(e) {
		case eRace::HUMAN: out << "human"; break;
		case eRace::NEPHIL: out << "nephil"; break;
		case eRace::SLITH: out << "slith"; break;
		case eRace::VAHNATAI: out << "vahnatai"; break;
		case eRace::HUMANOID: out << "humanoid"; break;
		case eRace::BEAST: out << "beast"; break;
		case eRace::BIRD: out << "bird"; break;
		case eRace::BUG: out << "bug"; break;
		case eRace::DEMON: out << "demon"; break;
		case eRace::DRAGON: out << "dragon"; break;
		case eRace::GIANT: out << "giant"; break;
		case eRace::IMPORTANT: out << "important"; break;
		case eRace::MAGE: out << "mage"; break;
		case eRace::PRIEST: out << "priest"; break;
		case eRace::MAGICAL: out << "magic"; break;
		case eRace::PLANT: out << "plant"; break;
		case eRace::REPTILE: out << "reptile"; break;
		case eRace::SLIME: out << "slime"; break;
		case eRace::STONE: out << "stone"; break;
		case eRace::UNDEAD: out << "undead"; break;
		case eRace::UNKNOWN: out << "humanoid"; break;
	}
	return out;
}

std::istream& operator >> (std::istream& in, eRace& e){
	std::string key;
	in >> key;
	e = eRace::HUMANOID;
	try {
		int i = boost::lexical_cast<int>(key);
		if(i > 0 && i < 20)
			e = (eRace) i;
	} catch(boost::bad_lexical_cast) {
		if(key == "human")
			e = eRace::HUMAN;
		else if(key == "nephil")
			e = eRace::NEPHIL;
		else if(key == "slith")
			e = eRace::SLITH;
		else if(key == "vahnatai")
			e = eRace::VAHNATAI;
		else if(key == "humanoid")
			e = eRace::HUMANOID;
		else if(key == "beast")
			e = eRace::BEAST;
		else if(key == "bird")
			e = eRace::BIRD;
		else if(key == "bug")
			e = eRace::BUG;
		else if(key == "demon")
			e = eRace::DEMON;
		else if(key == "dragon")
			e = eRace::DRAGON;
		else if(key == "giant")
			e = eRace::GIANT;
		else if(key == "important")
			e = eRace::IMPORTANT;
		else if(key == "mage")
			e = eRace::MAGE;
		else if(key == "priest")
			e = eRace::PRIEST;
		else if(key == "magic")
			e = eRace::MAGICAL;
		else if(key == "plant")
			e = eRace::PLANT;
		else if(key == "reptile")
			e = eRace::REPTILE;
		else if(key == "slime")
			e = eRace::SLIME;
		else if(key == "stone")
			e = eRace::STONE;
		else if(key == "undead")
			e = eRace::UNDEAD;
	}
	return in;
}

std::ostream& operator << (std::ostream& out, eMonstTime e){
	switch(e) {
		case eMonstTime::ALWAYS: out << "always"; break;
		case eMonstTime::APPEAR_ON_DAY: out << "after-day"; break;
		case eMonstTime::DISAPPEAR_ON_DAY: out << "until-day"; break;
		case eMonstTime::SOMETIMES_A: out << "travel-a"; break;
		case eMonstTime::SOMETIMES_B: out << "travel-b"; break;
		case eMonstTime::SOMETIMES_C: out << "travel-c"; break;
		case eMonstTime::APPEAR_WHEN_EVENT: out << "after-event"; break;
		case eMonstTime::DISAPPEAR_WHEN_EVENT: out << "until-event"; break;
		case eMonstTime::APPEAR_AFTER_CHOP: out << "after-death"; break;
	}
	return out;
}

std::istream& operator >> (std::istream& in, eMonstTime& e){
	std::string key;
	in >> key;
	e = eMonstTime::ALWAYS;
	try {
		int i = boost::lexical_cast<int>(key);
		if(i >= 0 && i != 3 && i < 6)
			e = eMonstTime(i);
		else if(i > 6 && i <= 8)
			e = eMonstTime(i - 1);
	} catch(boost::bad_lexical_cast) {
		if(key == "always")
			e = eMonstTime::ALWAYS;
		else if(key == "after-day")
			e = eMonstTime::APPEAR_ON_DAY;
		else if(key == "until-day")
			e = eMonstTime::DISAPPEAR_ON_DAY;
		else if(key == "travel-a")
			e = eMonstTime::SOMETIMES_A;
		else if(key == "travel-b")
			e = eMonstTime::SOMETIMES_B;
		else if(key == "travel-c")
			e = eMonstTime::SOMETIMES_C;
		else if(key == "after-event")
			e = eMonstTime::APPEAR_WHEN_EVENT;
		else if(key == "until-event")
			e = eMonstTime::DISAPPEAR_WHEN_EVENT;
		else if(key == "after-death")
			e = eMonstTime::APPEAR_AFTER_CHOP;
	}
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

std::ostream& operator << (std::ostream& out, eMonstMelee e) {
	return out << (int)e;
}

std::istream& operator >> (std::istream& in, eMonstMelee& e) {
	int i;
	in >> i;
	if(i >= 0 && i <= int(eMonstMelee::STAB))
		e = (eMonstMelee)i;
	else e = eMonstMelee::PUNCH;
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

std::ostream& operator << (std::ostream& out, eMonstSummon e) {
	return out << (int)e;
}

std::istream& operator >> (std::istream& in, eMonstSummon& e) {
	int i;
	in >> i;
	if(i >= 0 && i <= int(eMonstSummon::SPECIES))
		e = (eMonstSummon)i;
	else e = eMonstSummon::TYPE;
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
			switch(key) {
				case eMonstAbil::STUN: sout << "Stunning"; break;
				case eMonstAbil::PETRIFY: sout << "Petrifying"; break;
				case eMonstAbil::DRAIN_SP: sout << "Spell point drain"; break;
				case eMonstAbil::DRAIN_XP: sout << "Draining"; break;
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
				case eMonstAbil::DAMAGE: case eMonstAbil::DAMAGE2:
					switch(gen.dmg) {
						case eDamageType::FIRE: sout << "Fiery"; break;
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
			if(key == eMonstAbil::DAMAGE || key == eMonstAbil::DAMAGE2) {
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
				case eMonstAbil::MISSILE_WEB:
					sout << "Throws webs";
					break;
				case eMonstAbil::RAY_HEAT:
					sout << "Heat ray (" << special.extra3 << "d6)";
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
		file << "ATTACK " << i + 1 << ' ' << a[i].dice << ' ' << a[i].sides << ' ' << a[i].type << '\n';
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
				if(abil.first == eMonstAbil::DAMAGE || abil.first == eMonstAbil::DAMAGE2)
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
				file << "TYPE " << abil.second.summon.type << '\t' << abil.second.summon.what << '\n';
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
	// On-see event is not exported, so make sure the field ise not filled with garbage data
	see_spec = -1;
	std::istringstream bin;
	std::string cur;
	getline(file, cur, '\f');
	bin.str(cur);
	while(bin) {
		int temp1, temp2;
		getline(bin, cur);
		std::istringstream line(cur);
		line >> cur;
		if(cur == "MONSTER") {
			line >> std::ws;
			m_name = read_maybe_quoted_string(line);
		} else if(cur == "ATTACK") {
			int which;
			line >> which >> a[which].dice >> a[which].sides >> a[which].type;
			which--;
		} else if(cur == "SIZE") {
			line >> temp1 >> temp2;
			x_width = temp1;
			y_width = temp2;
		} else if(cur == "IMMUNE")
			line >> magic_res >> fire_res >> cold_res >> poison_res;
		else if(cur == "RACE")
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
						line >> abil.summon.type >> abil.summon.what;
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
					if(key == eMonstAbil::DAMAGE || key == eMonstAbil::DAMAGE2)
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
	file << "STARTATT " << start_attitude << '\n';
	file << "STARTLOC " << start_loc.x << ' ' << start_loc.y << '\n';
	file << "LOCATION " << cur_loc.x << ' ' << cur_loc.y << '\n';
	file << "MOBILITY " << mobility << '\n';
	file << "TIMEFLAG " << time_flag << '\n';
	file << "SUMMONED " << summoned << '\n';
	file << "SPEC " << spec1 << ' ' << spec2 << '\n';
	file << "SPECCODE " << spec_enc_code << '\n';
	file << "TIMECODE " << time_code << '\n';
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
	// TODO: Should we be saving "max_mp" and/or "m_morale"?
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
			line >> time_flag;
		} else if(cur == "SUMMONED")
			line >> summoned;
		else if(cur == "SPEC")
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
