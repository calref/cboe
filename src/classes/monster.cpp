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
	breath = old.breath;
	breath_type = old.breath_type;
	treasure = old.treasure;
	spec_skill = old.spec_skill;
	poison = old.poison;
	corpse_item = old.corpse_item;
	corpse_item_chance = old.corpse_item_chance;
	immunities = old.immunities;
	x_width = old.x_width;
	y_width = old.y_width;
	radiate_1 = old.radiate_1;
	radiate_2 = old.radiate_2;
	default_attitude = old.default_attitude;
	summon_type = old.summon_type;
	default_facial_pic = old.default_facial_pic;
	picture_num = old.picture_num;
	if(picture_num == 122) picture_num = 119;
	see_spec = -1;
}

cMonster::cMonster(){
	// TODO: Fill in
	see_str1 = -1;
	see_str2 = -1;
	see_spec = -1;
}

cCreature::cCreature(){
//	short personality;
//	short special_on_kill,facial_pic;
	//{-1,-1,-1}
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
	direction = old.m_d.direction;
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

cMonster::cAbility::operator std::string(){
	std::ostringstream sout;
	short i = 0;
	switch(abil){
		case MONST_NO_ABIL:
			break;
		case MONST_THROWS_DARTS:
			sout << "Throws darts (" << extra1 << 'd' << extra2 << ')';
			break;
		case MONST_SHOOTS_ARROWS:
			sout << "Shoots arrows (" << extra1 << 'd' << extra2 << ')';
			break;
		case MONST_THROWS_SPEARS:
			sout << "Throws spears (" << extra1 << 'd' << extra2 << ')';
			break;
		case MONST_THROWS_ROCKS:
			sout << "Throws rocks (" << extra1 << 'd' << extra2 << ')';
			break;
		case MONST_THROWS_RAZORDISKS:
			sout << "Throws razordisks (" << extra1 << 'd' << extra2 << ')';
			break;
		case MONST_GOOD_ARCHER:
			sout << "Good archer ("  << extra1 << 'd' << extra2 << ')';
			break;
		case MONST_SHOOTS_SPINES:
			sout << "Shoots spines (" << extra1 << 'd' << extra2 << ')';
			break;
		case MONST_THROWS_KNIVES:
			sout << "Throws knives (" << extra1 << 'd' << extra2 << ')';
			break;
		case MONST_DAMAGE_RAY:
		case MONST_DRAIN_XP_DAMAGE_RAY:
		case MONST_DAMAGE_TOUCH:
		case MONST_DRAIN_XP_DAMAGE_TOUCH:
			switch(extra1){
				case DAMAGE_WEAPON:
					sout << "Health drain";
					break;
				case DAMAGE_FIRE:
					sout << "Heat";
					break;
				case DAMAGE_POISON:
					sout << "Pain";
					break;
				case DAMAGE_MAGIC:
					sout << "Shock";
					break;
				case DAMAGE_UNBLOCKABLE:
					sout << "Wounding";
					break;
				case DAMAGE_COLD:
					sout << "Icy";
					break;
				case DAMAGE_UNDEAD:
				case DAMAGE_DEMON:
					sout << "Unholy";
					break;
				default:
					sout << "*ERROR INVALID DAMAGE TYPE*";
			}
			if(abil == MONST_DRAIN_XP_DAMAGE_RAY || abil == MONST_DRAIN_XP_DAMAGE_TOUCH)
				sout << ", draining";
			if(abil == MONST_DAMAGE_RAY || abil == MONST_DRAIN_XP_DAMAGE_RAY)
				sout << " ray";
			else sout << " touch";
			break;
		case MONST_STATUS_RAY:
		case MONST_STATUS_TOUCH:
			switch((eStatus)extra1){
				case eStatus::BLESS_CURSE:
					sout << "Curse";
					break;
				case eStatus::POISON:
					sout << "Poison";
					i = 1;
					break;
				case eStatus::HASTE_SLOW:
					sout << "Slowing";
					break;
				case eStatus::WEBS:
					sout << "Glue";
					i = 1;
					break;
				case eStatus::DISEASE:
					sout << "Infectious";
					i = 1;
					break;
				case eStatus::DUMB:
					sout << "Dumbfounding";
					break;
				case eStatus::ASLEEP:
					sout << "Sleep";
					break;
				case eStatus::PARALYZED:
					sout << "Paralysis";
					break;
				case eStatus::ACID:
					sout << "Acid";
					i = 1;
					break;
				default: // Poisoned weapon, invulnerable, magic resistance, sanctuary, martyr's shield, or invalid
					sout << "*ERROR BAD OR INVALID STATUS TYPE*";
			}
			if(abil == MONST_STATUS_RAY)
				if(i == 1) sout << " spit";
				else sout << " ray";
			else sout << " touch";
			break;
		case MONST_PETRIFY_RAY:
			sout << "Petrification ray";
			break;
		case MONST_DRAIN_SP_RAY:
			sout << "Spell point drain ray";
			break;
		case MONST_DRAIN_XP_RAY:
			sout << "Experience draining ray";
			break;
		case MONST_KILL_RAY:
			sout << "Death ray";
			break;
		case MONST_STEAL_FOOD_RAY:
			sout << "Steals food from afar";
			break;
		case MONST_STEAL_GOLD_RAY:
			sout << "Steals gold from afar";
			break;
			break;
		case MONST_PETRIFY_TOUCH:
			sout << "Petrification touch";
			break;
		case MONST_DRAIN_SP_TOUCH:
			sout << "Spell point draining touch";
			break;
		case MONST_DRAIN_XP_TOUCH:
			sout << "Experience draining touch";
			break;
			break;
		case MONST_KILL_TOUCH:
			sout << "Death touch";
			break;
		case MONST_STEAL_FOOD_TOUCH:
			sout << "Steals food when hits";
			break;
		case MONST_STEAL_GOLD_TOUCH:
			sout << "Steals gold when hits";
			break;
		case MONST_SUMMON_ONE:
			// TODO: Store the name of the summoned monster in the class so it can be used here.
//			sout << "Summons " << univ.scenario.scen_monsters[extra1].m_name << "s (" << extra2 <<"% chance)";
			break;
		case MONST_SUMMON_TYPE:
			sout << "Summons ";
			switch(extra1){
				case 0:
					sout << "wildlife";
					break;
				case 1:
					sout << "weak aid";
					break;
				case 2:
					sout << "strong aid";
					break;
				case 3:
					sout << "powerful aid";
					break;
				case 4:
					sout << "friends";
					break;
			}
			sout << " (" << extra2 << "% chance)";
			break;
		case MONST_SUMMON_SPECIES:
			sout << "Summons ";
			switch((eRace)extra1){
				case eRace::HUMAN:
					sout << "Humans";
					break;
				case eRace::NEPHIL:
					sout << "Nephilim";
					break;
				case eRace::SLITH:
					sout << "Slithzerikai";
					break;
				case eRace::VAHNATAI:
					sout << "Vahnatai";
					break;
				case eRace::REPTILE:
					sout << "reptiles";
					break;
				case eRace::BEAST:
					sout << "beasts";
					break;
				case eRace::HUMANOID:
					sout << "humanoids";
					break;
				case eRace::DEMON:
					sout << "demons";
					break;
				case eRace::UNDEAD:
					sout << "undead";
					break;
				case eRace::GIANT:
					sout << "giants";
					break;
				case eRace::SLIME:
					sout << "slimes";
					break;
				case eRace::STONE:
					sout << "golems";
					break;
				case eRace::BUG:
					sout << "bugs";
					break;
				case eRace::DRAGON:
					sout << "Dragons";
					break;
				case eRace::MAGICAL:
					sout << "magical creatures";
					break;
				case eRace::PLANT:
					sout << "plants";
					break;
				case eRace::BIRD:
					sout << "birds";
					break;
				default: // Important, Mage, Priest, or invalid
					sout << "*ERROR INVALID RACE*";
			}
			sout << " (" << extra2 << "% chance)";
			break;
		case MONST_SUMMON_RANDOM:
			sout << "Summons aid" << " (" << extra2 << "% chance)";
			break;
		case MONST_MASS_SUMMON:
			sout << "Summons aid" << " (" << extra2 << "% chance)";
			break;
		case MONST_SPLITS:
			sout << "Splits when hit" << " (" << extra2 << "% chance)";
			break;
		case MONST_FIELD_MISSILE:
			// TODO: Fill these in
			sout << "MONST_FIELD_MISSILE";
			break;
		case MONST_MARTYRS_SHIELD:
			sout << "Permanent Martyr's shield";
			break;
		case MONST_ABSORB_SPELLS:
			sout << "Absorbs spells";
			break;
		case MONST_INVULNERABLE:
			sout << "Invulnerable";
			break;
		case MONST_RADIATE:
			sout << "Radiates ";
			switch(extra1){ // TODO: Fill these in
			}
			sout << " (" << extra2 << "% chance)";
			break;
		case MONST_CALL_LOCAL_SPECIAL:
		case MONST_CALL_GLOBAL_SPECIAL:
			sout << "Unusual ability";
			break;
	}
	return sout.str();
}

std::string cMonster::getAbil1Name() {
	return (std::string) abil1;
}

std::string cMonster::getAbil2Name() {
	return (std::string) abil2;
}

bool cMonster::hasAbil(eMonstAbil what, unsigned char* x1, unsigned char* x2){
	if(abil1.abil == what){
		if(x1 != NULL) *x1 = abil1.extra1;
		if(x2 != NULL) *x2 = abil1.extra2;
		return true;
	}else if(abil2.abil == what){
		if(x1 != NULL) *x1 = abil2.extra1;
		if(x2 != NULL) *x2 = abil2.extra2;
		return true;
	}
	return false;
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
	file << "BREATH " << int(breath_type) << ' ' << int(breath) << '\n';
	file << "TREASURE" << int(treasure) << '\n';
	file << "ABIL 1 " << int(spec_skill) << " 0 0\n";
	file << "ABIL 2 " << int(radiate_1) << ' ' << int(radiate_2) << " 0\n";
	file << "POISON " << int(poison) << '\n';
	file << "CORPSEITEM " << corpse_item << ' ' << corpse_item_chance << '\n';
	file << "IMMUNE " << int(immunities) << '\n';
	file << "SIZE " << int(x_width) << ' ' << int(y_width) << '\n';
	file << "ATTITUDE " << int(default_attitude) << '\n';
	file << "SUMMON " << int(summon_type) << '\n';
	file << "PORTRAIT " << default_facial_pic << '\n';
	file << "PICTURE " << picture_num << '\n';
	file << "SOUND " << ambient_sound << '\n';
}

void cMonster::readFrom(std::istream& file) {
	// On-see event is not exported, so make sure the fields are not filled with garbage data
	see_sound = 0;
	see_str1 = -1;
	see_str2 = -1;
	see_spec = -1;
	while(file) {
		std::string cur;
		int temp1, temp2, temp3;
		getline(file, cur);
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
		} else if(cur == "BREATH") {
			line >> temp1 >> temp2;
			breath_type = temp1;
			breath = temp2;
		} else if(cur == "ABIL") {
			int which;
			line >> which >> temp1 >> temp2 >> temp3;
			if(which == 1)
				spec_skill = temp1;
			else if(which == 2)
				radiate_1 = temp1, radiate_2 = temp2;
		}  else if(cur == "SIZE") {
			line >> temp1 >> temp2;
			x_width = temp1;
			y_width = temp2;
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
			else if(cur == "POISON")
				poison = temp1;
			else if(cur == "IMMUNITIES")
				immunities = temp1;
			else if(cur == "ATTITUDE")
				default_attitude = temp1;
			else if(cur == "SUMMON")
				summon_type = temp1;
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
