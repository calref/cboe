//
//  creature.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#include "creature.hpp"

#include <iostream>
#include <sstream>
#include "oldstructs.h"
#include "mathutil.hpp"

const short cCreature::charm_odds[20] = {90,90,85,80,78, 75,73,60,40,30, 20,10,4,1,0, 0,0,0,0,0};

cCreature::cCreature(){
	number = active = attitude = start_attitude = 0;
	start_loc.x = start_loc.y = cur_loc.x = cur_loc.y = targ_loc.x = targ_loc.y = 80;
	mobility = 1;
	summon_time  = 0;
	time_flag = eMonstTime::ALWAYS;
	spec1 = spec2 = spec_enc_code = time_code = monster_time = 0;
	personality = special_on_kill = facial_pic = -1;
	target = 6;
}

cCreature::cCreature(int num) : cCreature() {
	number = num;
}

void cCreature::append(legacy::creature_data_type old){
	active = old.active;
	attitude = old.attitude;
	number = old.number;
	cur_loc.x = old.m_loc.x;
	cur_loc.y = old.m_loc.y;
	cMonster::append(old.m_d);
	mobility = old.mobile;
	summon_time = old.summoned;
	if(summon_time >= 100) {
		party_summoned = false;
		summon_time -= 100;
	} else party_summoned = true;
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

void cCreature::avatar() {
	health = m_health;
	status[eStatus::POISON] = 0;
	status[eStatus::BLESS_CURSE] = 8;
	status[eStatus::HASTE_SLOW] = 8;
	status[eStatus::WEBS] = 0;
	status[eStatus::DISEASE] = 0;
	if(status[eStatus::DUMB] > 0)
		status[eStatus::DUMB] = 0;
	status[eStatus::MARTYRS_SHIELD] = 8;
}

void cCreature::heal(int amt) {
	if(!is_alive()) return;
	if(health >= m_health) return;
	health += amt;
	if(health > m_health)
		health = m_health;
	if(health < 0)
		health = 0;
}

void cCreature::cure(int amt) {
	if(!is_alive()) return;
	if(status[eStatus::POISON] <= amt)
		status[eStatus::POISON] = 0;
	else status[eStatus::POISON] -= amt;
}

void cCreature::restore_sp(int amt) {
	if(!is_alive()) return;
	if(mp >= max_mp) return;
	mp += amt;
	if(mp > max_mp)
		mp = max_mp;
	if(mp < 0) mp = 0;
}

void cCreature::drain_sp(int drain) {
	drain = magic_adjust(drain);
	if(drain > 0) {
		if(mu > 0 && mp > 4)
			drain = min(mp, drain / 3);
		else if(cl > 0 && mp > 10)
			drain = min(mp, drain / 2);
		mp -= drain;
		if(mp < 0) mp = 0;
	}
}

bool cCreature::is_alive() const {
	return active > 0;
}

bool cCreature::is_friendly() const {
	return attitude % 2 == 0;
}

location cCreature::get_loc() const {
	return cur_loc;
}

bool cCreature::is_shielded() const {
	if(status[eStatus::MARTYRS_SHIELD] > 0)
		return true;
	if(abil[eMonstAbil::MARTYRS_SHIELD].active && get_ran(1,1,1000) <= abil[eMonstAbil::MARTYRS_SHIELD].special.extra1)
		return true;
	return false;
}

int cCreature::get_shared_dmg(int base_dmg) const {
	if(abil[eMonstAbil::MARTYRS_SHIELD].active) {
		base_dmg *= abil[eMonstAbil::MARTYRS_SHIELD].special.extra2;
		base_dmg /= 100;
	}
	return base_dmg;
}

int cCreature::magic_adjust(int how_much) {
	if(how_much <= 0) return how_much;
	if(abil[eMonstAbil::ABSORB_SPELLS].active && get_ran(1,1,1000) <= abil[eMonstAbil::ABSORB_SPELLS].special.extra1) {
		int gain = abil[eMonstAbil::ABSORB_SPELLS].special.extra2;
		if(32767 - health > gain)
			health = 32767;
		else health += gain;
		return 0;
	}
	// TODO: Magic resistance status effect?
	how_much *= magic_res;
	how_much /= 100;
	return how_much;
}

int cCreature::get_health() const {
	return health;
}

int cCreature::get_magic() const {
	return mp;
}

int cCreature::get_level() const {
	return level;
}

bool cCreature::on_space(location loc) const {
	if(loc.x - cur_loc.x >= 0 && loc.x - cur_loc.x <= x_width - 1 &&
	   loc.y - cur_loc.y >= 0 && loc.y - cur_loc.y <= y_width - 1)
		return true;
	return false;
}

void cCreature::writeTo(std::ostream& file) const {
	file << "MONSTER " << number << '\n';
	file << "ATTITUDE " << attitude << '\n';
	file << "STARTATT " << start_attitude << '\n';
	file << "STARTLOC " << start_loc.x << ' ' << start_loc.y << '\n';
	file << "LOCATION " << cur_loc.x << ' ' << cur_loc.y << '\n';
	file << "MOBILITY " << mobility << '\n';
	file << "TIMEFLAG " << time_flag << '\n';
	file << "SUMMONED " << summon_time << ' ' << party_summoned << '\n';
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
	file << "DIRECTION " << direction << '\n';
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
			line >> summon_time >> party_summoned;
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
