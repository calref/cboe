//
//  monst_write.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-07-29.
//
//

#include <fstream>
#include "catch.hpp"
#include "tinyprint.h"
#include "scenario.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readMonstersFromXml(Document&& data, cScenario& scenario);
extern void writeMonstersToXml(Printer&& data, cScenario& scenario);

// Some setup to make monster abilities printable and comparable
using MissileAbil = decltype(uAbility::missile);
using GeneralAbil = decltype(uAbility::gen);
using SummonAbil = decltype(uAbility::summon);
using RadiateAbil = decltype(uAbility::radiate);
using SpecialAbil = decltype(uAbility::special);

static ostream& operator<< (ostream& out, const MissileAbil& abil);
static ostream& operator<< (ostream& out, const GeneralAbil& abil);
static ostream& operator<< (ostream& out, const SummonAbil& abil);
static ostream& operator<< (ostream& out, const RadiateAbil& abil);
static ostream& operator<< (ostream& out, const SpecialAbil& abil);
static bool operator == (const MissileAbil& lhs, const MissileAbil& rhs);
static bool operator == (const GeneralAbil& lhs, const GeneralAbil& rhs);
static bool operator == (const SummonAbil& lhs, const SummonAbil& rhs);
static bool operator == (const RadiateAbil& lhs, const RadiateAbil& rhs);
static bool operator == (const SpecialAbil& lhs, const SpecialAbil& rhs);

static void in_and_out(string name, cScenario& scen) {
	string fpath = "junk/monst_";
	fpath += name;
	fpath += ".xml";
	ofstream fout;
	fout.exceptions(ios::badbit);
	fout.open(fpath);
	writeMonstersToXml(Printer(name, fout), scen);
	fout.close();
	// Reconstruct the scenario, to ensure that it doesn't just pass due to old data still being around
	scen.~cScenario();
	new(&scen) cScenario();
	ifstream fin;
	fin.exceptions(ios::badbit);
	fin.open(fpath);
	readMonstersFromXml(xmlDocFromStream(fin, name), scen);
}

TEST_CASE("Saving monster types") {
	cScenario scen;
	scen.scen_monsters.resize(2);
	scen.scen_monsters[1].m_name = "Test Monster";
	scen.scen_monsters[1].level = 3;
	scen.scen_monsters[1].armor = 5;
	scen.scen_monsters[1].skill = 6;
	scen.scen_monsters[1].m_health = 10;
	scen.scen_monsters[1].m_type = eRace::MAGICAL;
	scen.scen_monsters[1].x_width = 1;
	scen.scen_monsters[1].y_width = 2;
	scen.scen_monsters[1].picture= cMonster::get_picture_num(17);
	scen.scen_monsters[1].default_attitude = eAttitude::HOSTILE_B;
	SECTION("With the minimal required information") {
		in_and_out("basic", scen);
		CHECK(scen.scen_monsters[1].m_name == "Test Monster");
		CHECK(scen.scen_monsters[1].level == 3);
		CHECK(scen.scen_monsters[1].armor == 5);
		CHECK(scen.scen_monsters[1].skill == 6);
		CHECK(scen.scen_monsters[1].m_health == 10);
		CHECK(scen.scen_monsters[1].speed == 4);
		CHECK(scen.scen_monsters[1].m_type == eRace::MAGICAL);
		CHECK(scen.scen_monsters[1].x_width == 1);
		CHECK(scen.scen_monsters[1].y_width == 2);
		CHECK(scen.scen_monsters[1].get_num_for_picture() == 17);
		CHECK(scen.scen_monsters[1].default_attitude == eAttitude::HOSTILE_B);
		CHECK_FALSE(scen.scen_monsters[1].mindless);
		CHECK_FALSE(scen.scen_monsters[1].invuln);
		CHECK_FALSE(scen.scen_monsters[1].invisible);
		CHECK_FALSE(scen.scen_monsters[1].guard);
		for(int i = 0; i <= 8; i++) {
			eDamageType dmg = eDamageType(i);
			CAPTURE(dmg);
			CHECK(scen.scen_monsters[1].resist[dmg] == 100);
		}
	}
	SECTION("With most optional information") {
		scen.scen_monsters[1].speed = 3;
		scen.scen_monsters[1].mu = 1;
		scen.scen_monsters[1].cl = 2;
		scen.scen_monsters[1].treasure = 4;
		scen.scen_monsters[1].corpse_item = 128;
		scen.scen_monsters[1].corpse_item_chance = 35;
		scen.scen_monsters[1].amorphous = true;
		scen.scen_monsters[1].mindless = true;
		scen.scen_monsters[1].invuln = true;
		scen.scen_monsters[1].invisible = true;
		scen.scen_monsters[1].guard = true;
		scen.scen_monsters[1].summon_type = 3;
		scen.scen_monsters[1].default_facial_pic = 12;
		scen.scen_monsters[1].ambient_sound = 42;
		scen.scen_monsters[1].see_spec = 91;
		in_and_out("full", scen);
		CHECK(scen.scen_monsters[1].speed == 3);
		CHECK(scen.scen_monsters[1].mu == 1);
		CHECK(scen.scen_monsters[1].cl == 2);
		CHECK(scen.scen_monsters[1].treasure == 4);
		CHECK(scen.scen_monsters[1].corpse_item == 128);
		CHECK(scen.scen_monsters[1].corpse_item_chance == 35);
		CHECK(scen.scen_monsters[1].amorphous);
		CHECK(scen.scen_monsters[1].mindless);
		CHECK(scen.scen_monsters[1].invuln);
		CHECK(scen.scen_monsters[1].invisible);
		CHECK(scen.scen_monsters[1].guard);
		CHECK(scen.scen_monsters[1].summon_type == 3);
		CHECK(scen.scen_monsters[1].default_facial_pic == 12);
		CHECK(scen.scen_monsters[1].ambient_sound == 42);
		CHECK(scen.scen_monsters[1].see_spec == 91);
	}
	SECTION("With resistances") {
		scen.scen_monsters[1].resist[eDamageType::WEAPON] = 5;
		scen.scen_monsters[1].resist[eDamageType::FIRE] = 10;
		scen.scen_monsters[1].resist[eDamageType::POISON] = 15;
		scen.scen_monsters[1].resist[eDamageType::MAGIC] = 20;
		scen.scen_monsters[1].resist[eDamageType::UNBLOCKABLE] = 25;
		scen.scen_monsters[1].resist[eDamageType::COLD] = 30;
		scen.scen_monsters[1].resist[eDamageType::UNDEAD] = 35;
		scen.scen_monsters[1].resist[eDamageType::DEMON] = 40;
		scen.scen_monsters[1].resist[eDamageType::SPECIAL] = 45;
		in_and_out("resistance", scen);
		CHECK(scen.scen_monsters[1].resist[eDamageType::WEAPON] == 5);
		CHECK(scen.scen_monsters[1].resist[eDamageType::FIRE] == 10);
		CHECK(scen.scen_monsters[1].resist[eDamageType::POISON] == 15);
		CHECK(scen.scen_monsters[1].resist[eDamageType::MAGIC] == 20);
		CHECK(scen.scen_monsters[1].resist[eDamageType::UNBLOCKABLE] == 25);
		CHECK(scen.scen_monsters[1].resist[eDamageType::COLD] == 30);
		CHECK(scen.scen_monsters[1].resist[eDamageType::UNDEAD] == 35);
		CHECK(scen.scen_monsters[1].resist[eDamageType::DEMON] == 40);
		// This one should not be saved, so we expect it to revert to default
		CHECK(scen.scen_monsters[1].resist[eDamageType::SPECIAL] == 100);
	}
	SECTION("With attacks") {
		scen.scen_monsters[1].addAttack(1, 4);
		scen.scen_monsters[1].addAttack(2, 6, eMonstMelee::STAB);
		scen.scen_monsters[1].addAttack(3, 8, eMonstMelee::SLIME);
		in_and_out("attacks", scen);
		CHECK(scen.scen_monsters[1].a[0].dice == 1);
		CHECK(scen.scen_monsters[1].a[0].sides == 4);
		CHECK(scen.scen_monsters[1].a[0].type == eMonstMelee::SWING);
		CHECK(scen.scen_monsters[1].a[1].dice == 2);
		CHECK(scen.scen_monsters[1].a[1].sides == 6);
		CHECK(scen.scen_monsters[1].a[1].type == eMonstMelee::STAB);
		CHECK(scen.scen_monsters[1].a[2].dice == 3);
		CHECK(scen.scen_monsters[1].a[2].sides == 8);
		CHECK(scen.scen_monsters[1].a[2].type == eMonstMelee::SLIME);
	}
	SECTION("With an ability of every type") {
		MissileAbil missile = {true, eMonstMissile::SPINE, 5, 1, 8, 7, 10, 500};
		RadiateAbil radiate = {true, WALL_FIRE, 450, PAT_RAD3};
		SummonAbil summon = {true, eMonstSummon::TYPE, 128, 1, 8, 50, 580};
		SpecialAbil
			split = {true, 1, 2, 3},
			martyr = {true, 4, 5, 6},
			absorb = {true, 7, 8, 9},
			web = {true, 10, 11, 12},
			heat = {true, 13, 14, 15},
			special = {true, 16, 17, 18},
			hit = {true, 19, 20, 21},
			death = {true, 22, 23, 24};
		GeneralAbil
			damage = {true, eMonstGen::RAY, 3, 6, 12, 450},
			damage2 = {true, eMonstGen::TOUCH, -1, 4, 0, 250},
			status = {true, eMonstGen::GAZE, 2, 9, 20, 400},
			status2 = {true, eMonstGen::TOUCH, -1, 13, 0, 600},
			field = {true, eMonstGen::SPIT, 11, 14, 16, 150},
			petrify = {true, eMonstGen::BREATH, 15, 18, 22, 350},
			sp = {true, eMonstGen::RAY, 17, 24, 26, 100},
			xp = {true, eMonstGen::GAZE, 19, 27, 28, 120},
			kill = {true, eMonstGen::BREATH, 21, 32, 30, 300},
			food = {true, eMonstGen::SPIT, 23, 35, 40, 240},
			gold = {true, eMonstGen::GAZE, 29, 38, 36, 750},
			stun = {true, eMonstGen::RAY, 31, 33, 34, 800};
		damage.dmg = eDamageType::FIRE;
		damage2.dmg = eDamageType::COLD;
		status.stat = eStatus::PARALYZED;
		status2.stat = eStatus::POISON;
		field.fld = FIELD_ANTIMAGIC;
		stun.stat = eStatus::HASTE_SLOW;
		scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile = missile;
		scen.scen_monsters[1].abil[eMonstAbil::DAMAGE].gen = damage;
		scen.scen_monsters[1].abil[eMonstAbil::DAMAGE2].gen = damage2;
		scen.scen_monsters[1].abil[eMonstAbil::STATUS].gen = status;
		scen.scen_monsters[1].abil[eMonstAbil::STATUS2].gen = status2;
		scen.scen_monsters[1].abil[eMonstAbil::FIELD].gen = field;
		scen.scen_monsters[1].abil[eMonstAbil::PETRIFY].gen = petrify;
		scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].gen = sp;
		scen.scen_monsters[1].abil[eMonstAbil::DRAIN_XP].gen = xp;
		scen.scen_monsters[1].abil[eMonstAbil::KILL].gen = kill;
		scen.scen_monsters[1].abil[eMonstAbil::STEAL_FOOD].gen = food;
		scen.scen_monsters[1].abil[eMonstAbil::STEAL_GOLD].gen = gold;
		scen.scen_monsters[1].abil[eMonstAbil::STUN].gen = stun;
		scen.scen_monsters[1].abil[eMonstAbil::SPLITS].special = split;
		scen.scen_monsters[1].abil[eMonstAbil::MARTYRS_SHIELD].special = martyr;
		scen.scen_monsters[1].abil[eMonstAbil::ABSORB_SPELLS].special = absorb;
		scen.scen_monsters[1].abil[eMonstAbil::MISSILE_WEB].special = web;
		scen.scen_monsters[1].abil[eMonstAbil::RAY_HEAT].special = heat;
		scen.scen_monsters[1].abil[eMonstAbil::SPECIAL].special = special;
		scen.scen_monsters[1].abil[eMonstAbil::HIT_TRIGGER].special = hit;
		scen.scen_monsters[1].abil[eMonstAbil::DEATH_TRIGGER].special = death;
		scen.scen_monsters[1].abil[eMonstAbil::RADIATE].radiate = radiate;
		scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon = summon;
		in_and_out("abilities", scen);
		// Data stored in irrelevant slots should not be saved.
		split.extra2 = split.extra3 = 0;
		martyr.extra3 = absorb.extra3 = 0;
		web.extra3 = special.extra3 = 0;
		hit.extra3 = death.extra2 = death.extra3 = 0;
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile == missile);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::DAMAGE].gen == damage);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::DAMAGE2].gen == damage2);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::STATUS].gen == status);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::STATUS2].gen == status2);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::FIELD].gen == field);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::PETRIFY].gen == petrify);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].gen == sp);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_XP].gen == xp);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::KILL].gen == kill);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::STEAL_FOOD].gen == food);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::STEAL_GOLD].gen == gold);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::STUN].gen == stun);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::SPLITS].special == split);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::MARTYRS_SHIELD].special == martyr);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::ABSORB_SPELLS].special == absorb);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE_WEB].special == web);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::RAY_HEAT].special == heat);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::SPECIAL].special == special);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::HIT_TRIGGER].special == hit);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::DEATH_TRIGGER].special == death);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::RADIATE].radiate == radiate);
		CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon == summon);
	}
	SECTION("Test granularity of permille chances") {
		MissileAbil missile = {true, eMonstMissile::SPINE, 5, 1, 8, 7, 10};
		for(int i = 0; i <= 1000; i++) {
			missile.odds = i;
			scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile = missile;
			in_and_out("permille granularity", scen);
			CAPTURE(i);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile.odds == i);
		}
	}
}

ostream& operator<< (ostream& out, const MissileAbil& abil) {
	out << "Missile {";
	if(!abil.active) return out << "inactive}";
	out << "type = " << abil.type << ", ";
	out << "missile = " << abil.pic << ", ";
	out << "strength = " << abil.dice << 'd' << abil.sides << ", ";
	out << "skill = " << abil.skill << ", ";
	out << "range = " << abil.range << ", ";
	out << "chance = " << abil.odds << "}";
	return out;
}

bool operator == (const MissileAbil& lhs, const MissileAbil& rhs) {
	if(lhs.active != rhs.active) return false;
	if(lhs.type != rhs.type) return false;
	if(lhs.pic != rhs.pic) return false;
	if(lhs.dice != rhs.dice) return false;
	if(lhs.sides != rhs.sides) return false;
	if(lhs.skill != rhs.skill) return false;
	if(lhs.range != rhs.range) return false;
	if(lhs.odds != rhs.odds) return false;
	return true;
}

ostream& operator<< (ostream& out, const GeneralAbil& abil) {
	out << "General {";
	if(!abil.active) return out << "inactive}";
	out << "type = " << abil.type << ", ";
	if(abil.type != eMonstGen::TOUCH)
		out << "missile = " << abil.pic << ", ";
	out << "strength = " << abil.strength << ", ";
	if(abil.type != eMonstGen::TOUCH)
		out << "range = " << abil.range << ", ";
	out << "chance = " << abil.odds << ", ";
	out << "extra = " << abil.dmg << " | " << abil.stat << " | " << abil.fld << "}";
	return out;
}

bool operator == (const GeneralAbil& lhs, const GeneralAbil& rhs) {
	if(lhs.active != rhs.active) return false;
	if(lhs.type != rhs.type) return false;
	if(lhs.strength != rhs.strength) return false;
	if(lhs.odds != rhs.odds) return false;
	if(lhs.dmg != rhs.dmg) return false;
	if(lhs.stat != rhs.stat) return false;
	if(lhs.fld != rhs.fld) return false;
	if(lhs.type == eMonstGen::TOUCH) return true;
	if(lhs.pic != rhs.pic) return false;
	if(lhs.range != rhs.range) return false;
	return true;
}

ostream& operator<< (ostream& out, const SummonAbil& abil) {
	out << "Summon {";
	if(!abil.active) return out << "inactive}";
	out << "type = " << abil.type << ", ";
	out << "what = " << abil.what << ", ";
	out << "count = " << abil.min << '-' << abil.max << ", ";
	out << "duration = " << abil.len << ", ";
	out << "chance = " << abil.chance << "}";
	return out;
}

bool operator == (const SummonAbil& lhs, const SummonAbil& rhs) {
	if(lhs.active != rhs.active) return false;
	if(lhs.type != rhs.type) return false;
	if(lhs.what != rhs.what) return false;
	if(lhs.min != rhs.min) return false;
	if(lhs.max != rhs.max) return false;
	if(lhs.len != rhs.len) return false;
	if(lhs.chance != rhs.chance) return false;
	return true;
}

ostream& operator<< (ostream& out, const RadiateAbil& abil) {
	out << "Radiate {";
	if(!abil.active) return out << "inactive}";
	out << "type = " << abil.type << ", ";
	out << "pattern = " << abil.pat << ", ";
	out << "chance = " << abil.chance << "}";
	return out;
}

bool operator == (const RadiateAbil& lhs, const RadiateAbil& rhs) {
	if(lhs.active != rhs.active) return false;
	if(lhs.type != rhs.type) return false;
	if(lhs.pat != rhs.pat) return false;
	if(lhs.chance != rhs.chance) return false;
	return true;
}

ostream& operator<< (ostream& out, const SpecialAbil& abil) {
	if(!abil.active) return out << "Special {inactive}";
	out << "Special {" << abil.extra1 << ", " << abil.extra2 << ", " << abil.extra3 << "}";
	return out;
}

bool operator == (const SpecialAbil& lhs, const SpecialAbil& rhs) {
	if(lhs.active != rhs.active) return false;
	if(lhs.extra1 != rhs.extra1) return false;
	if(lhs.extra2 != rhs.extra2) return false;
	if(lhs.extra3 != rhs.extra3) return false;
	return true;
}
