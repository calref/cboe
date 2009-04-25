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
	strcpy((char*)m_name,(char*)old.m_name);
	health = old.health;
	m_health = old.m_health;
	mp = old.mp;
	max_mp = old.max_mp;
	armor = old.armor;
	skill = old.skill;
	for(i = 0; i < 3; i++) a[i] = old.a[i];
	a1_type = old.a1_type;
	a23_type = old.a23_type;
	m_type = old.m_type;
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
}