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
#include "oldstructs.hpp"
#include "mathutil.hpp"
#include "pc.hpp"
#include "spell.hpp"
#include "fileio/tagfile.hpp"

const short cCreature::charm_odds[21] = {90,90,85,80,78, 75,73,60,40,30, 20,10,4,1,0, 0,0,0,0,0, 0};

cCreature::cCreature() {
	attitude = eAttitude::DOCILE;
	cur_loc.x = cur_loc.y = targ_loc.x = targ_loc.y = 80;
}

cCreature::cCreature(int num) : cCreature() {
	number = num;
}

void cCreature::import_legacy(legacy::creature_data_type old){
	active = eCreatureStatus(old.active);
	attitude = eAttitude(old.attitude);
	number = old.number;
	cur_loc.x = old.m_loc.x;
	cur_loc.y = old.m_loc.y;
	cMonster::import_legacy(old.m_d);
	mobility = old.mobile;
	summon_time = old.summoned;
	if(summon_time >= 100) {
		party_summoned = false;
		summon_time -= 100;
	} else party_summoned = true;
	number = old.monst_start.number;
	start_attitude = eAttitude(old.monst_start.start_attitude);
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
	if(amt <= 0) return;
	if(mp >= max_mp) return;
	mp += amt;
	if(mp > max_mp)
		mp = max_mp;
	if(mp < 0) mp = 0;
}

void cCreature::drain_sp(int drain, bool allow_resist) {
	drain = magic_adjust(drain);
	if(drain <= 0) return;
	if(allow_resist) {
		if(mu > 0 && mp > 4)
			drain = min(mp, drain / 3);
		else if(cl > 0 && mp > 10)
			drain = min(mp, drain / 2);
	}
	mp -= drain;
	if(mp < 0) mp = 0;
}

void cCreature::poison(int how_much) {
	if(how_much != 0) {
		how_much = percent(how_much, resist[eDamageType::POISON]);
	}
	apply_status(eStatus::POISON, how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? eSpellNote::RESISTS : eSpellNote::POISONED);
	else
		spell_note(eSpellNote::CURED);
	
}

void cCreature::acid(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::ACID, how_much);
	if(how_much >= 0)
		spell_note(eSpellNote::ACID);
	else
		spell_note(eSpellNote::CLEANS_ACID);
}

void cCreature::slow(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::HASTE_SLOW, -how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? eSpellNote::RESISTS : eSpellNote::SLOWED);
	else
		spell_note(eSpellNote::HASTED);
	
}

void cCreature::curse(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::BLESS_CURSE, -how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? eSpellNote::RESISTS : eSpellNote::CURSED);
	else
		spell_note(eSpellNote::BLESSED);
	
}

void cCreature::web(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::WEBS, how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? eSpellNote::RESISTS : eSpellNote::WEBBED);
	else
		spell_note(eSpellNote::CLEANS_WEBS);
	
}

void cCreature::scare(int how_much) {
	how_much = magic_adjust(how_much);
	morale -= how_much;
	if(how_much >= 0)
		spell_note((how_much == 0) ? eSpellNote::RESISTS : eSpellNote::SCARED);
	else
		spell_note(eSpellNote::RALLIES);
	
}

void cCreature::disease(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::DISEASE, how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? eSpellNote::RESISTS : eSpellNote::DISEASED);
	else
		spell_note(eSpellNote::FEEL_BETTER);
	
}

void cCreature::dumbfound(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::DUMB, how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? eSpellNote::RESISTS : eSpellNote::DUMBFOUNDED);
	else
		spell_note(eSpellNote::MIND_CLEAR);
	
}

// For charm, amount is the resulting attitude of the charmed monster; if 0, attitude is 2.
void cCreature::sleep(eStatus which_status,int amount,int penalty) {
	if(which_status != eStatus::CHARM && amount < 0) {
		status[which_status] -= amount;
		if(which_status != eStatus::ASLEEP)
			status[which_status] = max(0, status[which_status]);
		return;
	}
	
	if((which_status == eStatus::ASLEEP) &&
	   (m_type == eRace::UNDEAD || m_type == eRace::SKELETAL || m_type == eRace::SLIME ||
		m_type == eRace::STONE || m_type == eRace::PLANT))
		return;
	short r1 = get_ran(1,1,100);
	if(resist[eDamageType::MAGIC] > 0) {
		r1 *= 100;
		r1 /= resist[eDamageType::MAGIC];
	} else r1 = 200;
	r1 += penalty;
	if(which_status == eStatus::FORCECAGE && (mu > 0 || cl > 0))
		r1 += 5;
	if(which_status == eStatus::ASLEEP)
		r1 -= 25;
	if(which_status == eStatus::PARALYZED)
		r1 -= 15;
	if(which_status == eStatus::ASLEEP && abil[eMonstAbil::FIELD].active && abil[eMonstAbil::FIELD].gen.fld == eFieldType::CLOUD_SLEEP)
		return;
	
	if(r1 > charm_odds[level / 2]) {
		//one_sound(68);
		spell_note(eSpellNote::RESISTS);
	}
	else {
		if(which_status == eStatus::CHARM) {
			if(amount <= 0 || amount > 3) amount = 2;
			attitude = eAttitude(amount);
			spell_note(eSpellNote::CHARMED);
		} else if(which_status == eStatus::FORCECAGE) {
			status[eStatus::FORCECAGE] = amount;
			spell_note(eSpellNote::TRAPPED);
		} else {
			status[which_status] = amount;
			if(which_status == eStatus::ASLEEP && (amount >= 0))
				spell_note(eSpellNote::ASLEEP);
			if(which_status == eStatus::PARALYZED && (amount >= 0))
				spell_note(eSpellNote::PARALYZED);
			if(amount < 0)
				spell_note(eSpellNote::ALERT);
		}
		//one_sound(53);
	}
}

bool cCreature::is_alive() const {
	return active != eCreatureStatus::DEAD;
}

bool cCreature::is_friendly() const {
	return attitude == eAttitude::DOCILE || attitude == eAttitude::FRIENDLY;
}

bool cCreature::is_friendly(const iLiving& other) const {
	if(is_friendly() != other.is_friendly())
		return false;
	if(const cCreature* monst = dynamic_cast<const cCreature*>(&other)) {
		if(!is_friendly()) return attitude == monst->attitude;
	}
	// If we get this far, both monsters are friendly to the player.
	// (Or, maybe the other is a player rather than a monster.)
	return true;
}

location cCreature::get_loc() const {
	return cur_loc;
}

std::string cCreature::get_name() const {
	return m_name;
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
		base_dmg = percent(base_dmg, abil[eMonstAbil::MARTYRS_SHIELD].special.extra2);
	}
	return base_dmg;
}

int cCreature::magic_adjust(int how_much) {
	if(how_much <= 0) return how_much;
	if(abil[eMonstAbil::ABSORB_SPELLS].active && get_ran(1,1,1000) <= abil[eMonstAbil::ABSORB_SPELLS].special.extra1) {
		int gain = abil[eMonstAbil::ABSORB_SPELLS].special.extra2;
		add_check_overflow(health, gain);
		return 0;
	}
	// TODO: Magic resistance status effect?
	how_much = percent(how_much, resist[eDamageType::MAGIC]);
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
	for(int x = cur_loc.x; x < cur_loc.x + x_width; ++x){
		for(int y = cur_loc.y; y < cur_loc.y + y_width; ++y){
			if(loc.x == x && loc.y == y) return true;
		}
	}
	return false;
}

void cCreature::writeTo(cTagFile_Page& page) const {
	page["MONSTER"] << number;
	page["ALERT"] << (active == eCreatureStatus::ALERTED);
	page["ATTITUDE"] << attitude;
	page["STARTATT"] << start_attitude;
	page["STARTLOC"] << start_loc.x << start_loc.y;
	page["LOCATION"] << cur_loc.x << cur_loc.y;
	page["MOBILITY"] << mobility;
	page["TIMEFLAG"] << time_flag;
	page["SUMMONED"] << summon_time << party_summoned;
	page["SPEC"] << spec1 << spec2;
	page["SPECCODE"] << spec_enc_code;
	page["TIMECODE"] << time_code;
	page["TIME"] << monster_time;
	page["TALK"] << personality;
	page["DEATH"] << special_on_kill;
	page["TALK"] << special_on_talk;
	page["FACE"] << facial_pic;
	page["TARGET"] << target;
	page["STATUS"].encodeSparse(status);
	page["HEALTH"] << health << m_health;
	page["MANA"] << mp << max_mp;
	page["MORALE"] << morale << m_morale;
	page["DIRECTION"] << direction;
}

void cCreature::readFrom(const cTagFile_Page& page) {
	targ_loc = location(0,0);
	bool alert;
	page["MONSTER"] >> number;
	page["ALERT"] >> alert;
	page["ATTITUDE"] >> attitude;
	page["STARTATT"] >> start_attitude;
	page["STARTLOC"] >> start_loc.x >> start_loc.y;
	page["LOCATION"] >> cur_loc.x >> cur_loc.y;
	page["MOBILITY"] >> mobility;
	page["TIMEFLAG"] >> time_flag;
	page["SUMMONED"] >> summon_time >> party_summoned;
	page["SPEC"] >> spec1 >> spec2;
	page["SPECCODE"] >> spec_enc_code;
	page["TIMECODE"] >> time_code;
	page["TIME"] >> monster_time;
	page["TALK"] >> personality;
	page["DEATH"] >> special_on_kill;
	page["TALK"] >> special_on_talk;
	page["FACE"] >> facial_pic;
	page["TARGET"] >> target;
	page["STATUS"].extractSparse(status);
	page["HEALTH"] >> health >> m_health;
	page["MANA"] >> mp >> max_mp;
	page["MORALE"] >> morale;
	page["DIRECTION"] >> direction;
	active = alert ? eCreatureStatus::ALERTED : eCreatureStatus::IDLE;
}
