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

const short cCreature::charm_odds[21] = {90,90,85,80,78, 75,73,60,40,30, 20,10,4,1,0, 0,0,0,0,0, 0};

cCreature::cCreature() {
	attitude = eAttitude::DOCILE;
	cur_loc.x = cur_loc.y = targ_loc.x = targ_loc.y = 80;
}

cCreature::cCreature(int num) : cCreature() {
	number = num;
}

void cCreature::import_legacy(legacy::creature_data_type old){
	active = old.active;
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

void cCreature::poison(int how_much) {
	if(how_much != 0) {
		how_much *= resist[eDamageType::POISON];
		how_much /= 100;
	}
	apply_status(eStatus::POISON, how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? 10 : 4);
	else
		spell_note(34);
	
}

void cCreature::acid(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::ACID, how_much);
	if(how_much >= 0)
		spell_note(31);
	else
		spell_note(48);
}

void cCreature::slow(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::HASTE_SLOW, -how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? 10 : 2);
	else
		spell_note(35);
	
}

void cCreature::curse(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::BLESS_CURSE, -how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? 10 : 5);
	else
		spell_note(36);
	
}

void cCreature::web(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::WEBS, how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? 10 : 19);
	else
		spell_note(37);
	
}

void cCreature::scare(int how_much) {
	how_much = magic_adjust(how_much);
	morale -= how_much;
	if(how_much >= 0)
		spell_note((how_much == 0) ? 10 : 1);
	else
		spell_note(47);
	
}

void cCreature::disease(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::DISEASE, how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? 10 : 25);
	else
		spell_note(38);
	
}

void cCreature::dumbfound(int how_much) {
	how_much = magic_adjust(how_much);
	apply_status(eStatus::DUMB, how_much);
	if(how_much >= 0)
		spell_note((how_much == 0) ? 10 : 22);
	else
		spell_note(39);
	
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
		spell_note(10);
	}
	else {
		if(which_status == eStatus::CHARM) {
			if(amount <= 0 || amount > 3) amount = 2;
			attitude = eAttitude(amount);
			spell_note(23);
		} else if(which_status == eStatus::FORCECAGE) {
			status[eStatus::FORCECAGE] = amount;
			spell_note(52);
		} else {
			status[which_status] = amount;
			if(which_status == eStatus::ASLEEP && (amount >= 0))
				spell_note(28);
			if(which_status == eStatus::PARALYZED && (amount >= 0))
				spell_note(30);
			if(amount < 0)
				spell_note(40);
		}
		//one_sound(53);
	}
}

bool cCreature::is_alive() const {
	return active > 0;
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
	how_much *= resist[eDamageType::MAGIC];
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
		std::istringstream line(cur);
		line >> cur;
		if(cur == "MONSTER")
			line >> number;
		else if(cur == "ATTITUDE")
			line >> attitude;
		else if(cur == "STARTATT") {
			line >> start_attitude;
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

void cCreature::print_attacks(iLiving* target) {
	if(!print_result) return;
	std::string msg = m_name;
	msg += " attacks ";
	if(cPlayer* who = dynamic_cast<cPlayer*>(target))
		msg += who->name;
	else if(cCreature* monst = dynamic_cast<cCreature*>(target))
		msg += monst->m_name;
	else msg += "you";
	print_result(msg);
}

void cCreature::spell_note(int which_mess) {
	if(!print_result) return;
	std::string msg = m_name;
	switch(which_mess) {
		case 1:
			msg = "  " + msg + " scared.";
			break;
			
		case 2:
			msg = "  " + msg + " slowed.";
			break;
			
		case 3:
			msg = "  " + msg + " weakened.";
			break;
			
		case 4:
			msg = "  " + msg + " poisoned.";
			break;
			
		case 5:
			msg = "  " + msg + " cursed.";
			break;
			
		case 6:
			msg = "  " + msg + " ravaged.";
			break;
			
		case 7:
			msg = "  " + msg + " undamaged.";
			break;
			
		case 8:
			msg = "  " + msg + " is stoned.";
			break;
		case 9:
			msg = "  Gazes at " + msg + '.';
			break;
		case 10:
			msg = "  " + msg + " resists.";
			break;
		case 11:
			msg = "  Drains " + msg + '.';
			break;
		case 12:
			msg = "  Shoots at " + msg + '.';
			break;
		case 13:
			msg = "  Throws spear at " + msg + '.';
			break;
		case 14:
			msg = "  Throws rock at " + msg + '.';
			break;
		case 15:
			msg = "  Throws razordisk at " + msg + '.';
			break;
		case 16:
			msg = "  Hits " + msg + '.';
			break;
		case 17:
			msg = "  " + msg + " disappears.";
			break;
		case 18:
			msg = "  Misses " + msg + '.';
			break;
		case 19:
			msg = "  " + msg + " is webbed.";
			break;
		case 20:
			msg = "  " + msg + " chokes.";
			break;
		case 21:
			msg = "  " + msg + " summoned.";
			break;
		case 22:
			msg = "  " + msg + " is dumbfounded.";
			break;
		case 23:
			msg = "  " + msg + " is charmed.";
			break;
		case 24:
			msg = "  " + msg + " is recorded.";
			break;
		case 25:
			msg = "  " + msg + " is diseased.";
			break;
		case 26:
			msg = "  " + msg + " is an avatar!";
			break;
		case 27:
			msg = "  " + msg + " splits!";
			break;
		case 28:
			msg = "  " + msg + " falls asleep.";
			break;
		case 29:
			msg = "  " + msg + " wakes up.";
			break;
		case 30:
			msg = "  " + msg + " paralyzed.";
			break;
		case 31:
			msg = "  " + msg + " covered with acid.";
			break;
		case 32:
			msg = "  Fires spines at " + msg + '.';
			break;
		case 33:
			msg = "  " + msg + " summons aid.";
			break;
		case 34:
			msg = "  " + msg + " is cured.";
			break;
		case 35:
			msg = "  " + msg + " is hasted.";
			break;
		case 36:
			msg = "  " + msg + " is blessed.";
			break;
		case 37:
			msg = "  " + msg + " cleans webs.";
			break;
		case 38:
			msg = "  " + msg + " feels better.";
			break;
		case 39:
			msg = "  " + msg + " mind cleared.";
			break;
		case 40:
			msg = "  " + msg + " feels alert.";
			break;
		case 41:
			msg = "  " + msg + " is healed.";
			break;
		case 42:
			msg = "  " + msg + " drained of health.";
			break;
		case 43:
			msg = "  " + msg + " magic recharged.";
			break;
		case 44:
			msg = "  " + msg + " drained of magic.";
			break;
		case 45:
			msg = "  " + msg + " returns to life!";
			break;
		case 46:
			msg = "  " + msg + " dies.";
			break;
		case 47:
			msg = "  " + msg + " rallies its courage.";
			break;
		case 48:
			msg = "  " + msg + " cleans off acid.";
			break;
		case 49:
			msg = "  " + msg + " breaks barrier.";
			break;
		case 50:
			msg = "  " + msg + " breaks force cage.";
			break;
		case 51:
			msg = "  " + msg + " is obliterated!";
			break;
		case 52:
			msg = "  " + msg + " is trapped!";
			break;
		case 53:
			msg = "  Throws dart at " + msg + '.';
			break;
		case 54:
			msg = "  Throws knife at " + msg + '.';
			break;
		case 55:
			msg = "  Fires ray at " + msg + '.';
			break;
		case 56:
			msg = "  Gazes at " + msg + '.';
			break;
		case 57:
			msg = "  Breathes on " + msg + '.';
			break;
		case 58:
			msg = "  Throws web at " + msg + '.';
			break;
		case 59:
			msg = "  Spits at " + msg + '.';
			break;
		default:
			msg += ": Unknown action " + std::to_string(which_mess);
	}
	
	if(which_mess > 0)
		print_result((char *) msg.c_str());
}

void cCreature::cast_spell_note(eSpell spell) {
	if(!print_result) return;
	print_result(m_name + " casts:");
	print_result("  " + (*spell).name());
}

void cCreature::breathe_note() {
	if(!print_result) return;
	print_result(m_name + " breathes.");
}

void cCreature::damaged_msg(int how_much,int how_much_spec) {
	if(!print_result) return;
	std::ostringstream sout;
	sout << "  " << m_name << " takes " << how_much;
	if(how_much_spec > 0)
		sout << '+' << how_much_spec;
	print_result(sout.str());
}

void cCreature::killed_msg() {
	if(!print_result) return;
	print_result("  " + m_name + " dies.");
}
