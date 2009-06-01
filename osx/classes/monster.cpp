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


#include "classes.h"
#include "oldstructs.h"

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
	picture_num = old.picture_num;
	if(picture_num == 122) picture_num = 119;
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

cCreature& cCreature::operator = (legacy::creature_data_type old){
	active = old.active;
	attitude = old.attitude;
	number = old.number;
	cur_loc.x = old.m_loc.x;
	cur_loc.y = old.m_loc.y;
	*this = old.m_d;
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

cMonster::cAttack::operator int(){
	return dice * 100 + sides;
}

cMonster::cAttack& cMonster::cAttack::operator=(int n){
	dice = n / 100;
	sides = n % 100;
	return *this;
}

std::ostream& operator << (std::ostream& out, eStatus& e){
	return out << (int) e;
}

std::istream& operator >> (std::istream& in, eStatus& e){
	int i;
	in >> i;
	if(i > 0 && i < 14)
		e = (eStatus) i;
	else e = STATUS_POISONED_WEAPON;
	return in;
}

std::ostream& operator << (std::ostream& out, eRace& e){
	return out << (int) e;
}

std::istream& operator >> (std::istream& in, eRace& e){
	int i;
	in >> i;
	if(i > 0 && i < 20)
		e = (eRace) i;
	else e = RACE_HUMAN;
	return in;
}

extern cUniverse univ;
extern cScenario scenario;
cCreature& cCreature::operator = (const cCreature& other){ // replaces return_monster_template() from boe.monsters.cpp
	id = other.id;
	number = other.number;
	start_attitude = other.start_attitude;
	start_loc = other.start_loc;
	mobility = other.mobility;
	time_flag = other.time_flag;
	extra1 = other.extra1;
	extra2 = other.extra2;
	spec1 = other.spec1;
	spec2 = other.spec2;
	spec_enc_code = other.spec_enc_code;
	time_code = other.time_code;
	monster_time = other.monster_time;
	personality = other.personality;
	special_on_kill = other.special_on_kill;
	facial_pic = other.facial_pic;
	*this = scenario.scen_monsters[number];
	active = 1; // TODO: Is this right?
	if(spec_skill == 11) picture_num = 0;
	m_health /= (univ.party.stuff_done[306][7]) ? 2 : 1;
	m_health *= univ.difficulty_adjust();
	health = m_health;
	ap = 0;
	if((mu > 0 || cl > 0))
		max_mp = mp = 12 * level;
	else max_mp = mp = 0;
	m_morale = 10 * level;
	if(level >= 20) m_morale += 10 * (level - 20);
	morale = m_morale;
	direction = 0;
	for(int i = 0; i < 15; i++) status[i] = 0;
	attitude = start_attitude; // TODO: Is this right?
	cur_loc = start_loc;
	target = 6; // No target
	return *this;
}

cMonster::cAbility::operator std::string(){
	std::ostringstream sout;
	short i = 0;
	switch(abil){
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
			switch(extra1){
				case STATUS_BLESS_CURSE:
					sout << "Curse";
					break;
				case STATUS_POISON:
					sout << "Poison";
					i = 1;
					break;
				case STATUS_HASTE_SLOW:
					sout << "Slowing";
					break;
				case STATUS_WEBS:
					sout << "Glue";
					i = 1;
					break;
				case STATUS_DISEASE:
					sout << "Infectious";
					i = 1;
					break;
				case STATUS_DUMB:
					sout << "Dumbfounding";
					break;
				case STATUS_ASLEEP:
					sout << "Sleep";
					break;
				case STATUS_PARALYZED:
					sout << "Paralysis";
					break;
				case STATUS_ACID:
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
			sout << "Summons " << scenario.scen_monsters[extra1].m_name << "s (" << extra2 <<"% chance)";
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
			switch(extra1){
				case RACE_HUMAN:
					sout << "Humans";
					break;
				case RACE_NEPHIL:
					sout << "Nephilim";
					break;
				case RACE_SLITH:
					sout << "Slithzerikai";
					break;
				case RACE_VAHNATAI:
					sout << "Vahnatai";
					break;
				case RACE_REPTILE:
					sout << "reptiles";
					break;
				case RACE_BEAST:
					sout << "beasts";
					break;
				case RACE_HUMANOID:
					sout << "humanoids";
					break;
				case RACE_DEMON:
					sout << "demons";
					break;
				case RACE_UNDEAD:
					sout << "undead";
					break;
				case RACE_GIANT:
					sout << "giants";
					break;
				case RACE_SLIME:
					sout << "slimes";
					break;
				case RACE_STONE:
					sout << "golems";
					break;
				case RACE_BUG:
					sout << "bugs";
					break;
				case RACE_DRAGON:
					sout << "Dragons";
					break;
				case RACE_MAGICAL:
					sout << "magical creatures";
					break;
				case RACE_PLANT:
					sout << "plants";
					break;
				case RACE_BIRD:
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
	return "*ERROR INVALID ABILITY*";
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
