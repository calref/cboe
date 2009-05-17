/*
 *  monster.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "classes.h"
#include "oldstructs.h"

__attribute__((deprecated))
cMonster& cMonster::operator = (legacy::monster_record_type& old){
	int i;
	m_num = old.m_num;
	level = old.level;
	//m_name = old.m_name;
	health = old.health;
	m_health = old.m_health;
	mp = old.mp;
	max_mp = old.max_mp;
	armor = old.armor;
	skill = old.skill;
	for(i = 0; i < 3; i++) a[i] = old.a[i];
	a1_type = old.a1_type;
	a23_type = old.a23_type;
	m_type = (eMonsterType) old.m_type;
	speed = old.speed;
	ap = old.ap;
	mu = old.mu;
	cl = old.cl;
	breath = old.breath;
	breath_type = old.breath_type;
	treasure = old.treasure;
	spec_skill = old.spec_skill;
	poison = old.poison;
	morale = old.morale;
	m_morale = old.m_morale;
	corpse_item = old.corpse_item;
	corpse_item_chance = old.corpse_item_chance;
	for(i = 0; i < 15; i++) status[i] = old.status[i];
	direction = old.direction;
	immunities = old.immunities;
	x_width = old.x_width;
	y_width = old.y_width;
	radiate_1 = old.radiate_1;
	radiate_2 = old.radiate_2;
	default_attitude = old.default_attitude;
	summon_type = old.summon_type;
	default_facial_pic = old.default_facial_pic;
	res1 = old.res1;
	res2 = old.res2;
	res3 = old.res3;
	picture_num = old.picture_num;
	if(picture_num = 122) picture_num = 119;
	return *this;
}

cCreature::cCreature(){
	//		short personality;
	//		short special_on_kill,facial_pic;
	//{-1,-1,-1}
	id = number = active = attitude = start_attitude = 0;
	start_loc.x = start_loc.y = cur_loc.x = cur_loc.y = targ_loc.x = targ_loc.y = 80;
	mobility = 1;
	time_flag = summoned = extra1 = extra2 = 0;
	spec1 = spec2 = spec_enc_code = time_code = monster_time = 0;
	personality = special_on_kill = facial_pic = -1;
	target = 6;
}

cCreature& cCreature::operator = (legacy::creature_start_type old){
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
	return *this;
}

__attribute__((deprecated))
cCreature& cCreature::operator = (legacy::creature_data_type old){
	active = old.active;
	attitude = old.attitude;
	number = old.number;
	cur_loc.x = old.m_loc.x;
	cur_loc.y = old.m_loc.y;
	m_d = old.m_d;
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
	return *this;
}
