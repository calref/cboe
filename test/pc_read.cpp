//
//  pc_read.cpp
//  BoE
//
//  Created by Celtic Minstrel on 16-09-15.
//
//

#include <fstream>
#include "catch.hpp"
#include "universe/pc.hpp"
#include "universe/party.hpp"

using namespace std;

TEST_CASE("Loading player character from file") {
	ifstream fin;
	fin.exceptions(ios::badbit);
	cPlayer pc(no_party);
	// Fill in some junk data
	pc.cur_sp = 27;
	pc.max_sp = 38;
	pc.status[eStatus::CHARM] = 17;
	pc.traits[eTrait::ANAMA] = true;
	pc.skills[eSkill::LUCK] = 15;
	pc.mage_spells = 0xfefefe;
	pc.priest_spells = 0xefefef;
	pc.weap_poisoned.slot = 2;
	pc.ap = 22;
	
	SECTION("Basic Info") {
		fin.open("files/player/basic.txt");
		pc.readFrom(fin);
		CHECK(pc.unique_id == 3);
		CHECK(pc.main_status == eMainStatus::ALIVE);
		CHECK(pc.name == "Freddy O'Hara");
		CHECK(pc.skills.empty());
		CHECK(pc.max_health == 20);
		CHECK(pc.max_sp == 0);
		CHECK(pc.cur_health == 18);
		CHECK(pc.cur_sp == 0);
		CHECK(pc.experience == 12);
		CHECK(pc.skill_pts == 3);
		CHECK(pc.level == 2);
		CHECK(pc.status.size() == 1);
		CHECK(pc.status[eStatus::WEBS] == 3);
		CHECK(pc.traits.size() == 3);
		CHECK(pc.traits[eTrait::AMBIDEXTROUS]);
		CHECK(pc.traits[eTrait::NIMBLE]);
		CHECK(pc.traits[eTrait::BAD_BACK]);
		CHECK(pc.which_graphic == 3);
		CHECK(pc.race == eRace::HUMAN);
		CHECK(pc.direction == DIR_E);
		CHECK(pc.mage_spells == 0);
		CHECK(pc.priest_spells == 0);
		CHECK(pc.weap_poisoned.slot > 24);
		CHECK(pc.ap == 0);
	}
	SECTION("Skills") {
		fin.open("files/player/skills.txt");
		pc.readFrom(fin);
		CHECK(pc.skills.size() == 8);
		CHECK(pc.skills[eSkill::STRENGTH] == 5);
		CHECK(pc.skills[eSkill::DEXTERITY] == 6);
		CHECK(pc.skills[eSkill::INTELLIGENCE] == 3);
		CHECK(pc.skills[eSkill::EDGED_WEAPONS] == 3);
		CHECK(pc.skills[eSkill::DISARM_TRAPS] == 2);
		CHECK(pc.skills[eSkill::ITEM_LORE] == 1);
		CHECK(pc.skills[eSkill::LOCKPICKING] == 2);
		CHECK(pc.skills[eSkill::LUCK] == 1);
		CHECK(pc.max_health == 20);
	}
	SECTION("Spells") {
		fin.open("files/player/spells.txt");
		pc.readFrom(fin);
		// This has bits 3, 7, and 20 set
		CHECK(pc.mage_spells == 0x100088);
		// This has bits 2, 8, and 60 set
		CHECK(pc.priest_spells == 0x1000000000000104);
	}
}

