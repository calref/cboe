//
//  spec_legacy.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-07-11.
//
//

#include "catch.hpp"
#include "oldstructs.hpp"
#include "special.hpp"
#include "res_strings.hpp"
#include "pictypes.hpp"
#include "spell.hpp"
#include "damage.hpp"
#include "skills_traits.hpp"
#include "fields.hpp"
#include "race.hpp"
#include "monster.hpp" // for eAttitude
#include "town.hpp" // for lighting constants

using namespace std;

ostream& operator<< (ostream& out, eSpecType spec);

// TODO: Replace these with behavioral tests - ie, actually run the node and verify correct results.

TEST_CASE("When converting legacy special nodes (general)") {
	cSpecial newSpec;
	legacy::special_node_type oldSpec = {0,-1,-1,0,-1,-1,-1,-1,-1,-1,-1};
	// Fetching opcodes requires strings to be available
	// Here we fetch them from the rsrc dir, rather than the data dir
	ResMgr::pushPath<StringRsrc>("../rsrc/strings");
	
	oldSpec.jumpto = 12;
	SECTION("Null Special") {
		oldSpec.type = 0;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::NONE);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Set Flag") {
		oldSpec.type = 1;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		oldSpec.sd1 = 4; oldSpec.sd2 = 5;
		oldSpec.ex1a = 6;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::SET_SDF);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.sd1 == 4); CHECK(newSpec.sd2 == 5);
		CHECK(newSpec.ex1a == 6);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Increment Flag") {
		oldSpec.type = 2;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		oldSpec.sd1 = 4; oldSpec.sd2 = 5;
		oldSpec.ex1a = 6; oldSpec.ex1b = 7;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.sd1 == 4); CHECK(newSpec.sd2 == 5);
		CHECK(newSpec.ex1a == 6); CHECK(newSpec.ex1b == 7);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Display Message") {
		oldSpec.type = 3;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Secret Passage") {
		oldSpec.type = 4;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::CANT_ENTER);
		CHECK(newSpec.ex1a == 0);
		CHECK(newSpec.ex2a == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Display Small Message") {
		oldSpec.type = 5;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Flip Flag") {
		oldSpec.type = 6;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		oldSpec.sd1 = 4; oldSpec.sd2 = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::FLIP_SDF);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.sd1 == 4); CHECK(newSpec.sd2 == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Out Block") {
		oldSpec.type = 7;
		oldSpec.m1 = 4; oldSpec.m2 = 5;
		oldSpec.ex1a = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_CONTEXT);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == int(eSpecCtx::OUT_MOVE));
		CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Town Block") {
		oldSpec.type = 8;
		oldSpec.m1 = 4; oldSpec.m2 = 5;
		oldSpec.ex1a = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_CONTEXT);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == int(eSpecCtx::TOWN_MOVE));
		CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Combat Block") {
		oldSpec.type = 9;
		oldSpec.m1 = 4; oldSpec.m2 = 5;
		oldSpec.ex1a = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_CONTEXT);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == int(eSpecCtx::COMBAT_MOVE));
		CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Looking Block") {
		oldSpec.type = 10;
		oldSpec.m1 = 4; oldSpec.m2 = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_LOOKING);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Can't Enter") {
		oldSpec.type = 11;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::CANT_ENTER);
		CHECK(newSpec.ex1a == 0);
		CHECK(newSpec.ex2a == 0);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Change Time") {
		oldSpec.type = 12;
		oldSpec.ex1a = 4;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::CHANGE_TIME);
		CHECK(newSpec.ex1a == 4);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Start Scenario Timer") {
		oldSpec.type = 13;
		oldSpec.ex1a = 4; oldSpec.ex1b = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::SCEN_TIMER_START);
		CHECK(newSpec.ex1a == 4); CHECK(newSpec.ex1b == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Play Sound") {
		oldSpec.type = 14;
		oldSpec.ex1a = 10;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::PLAY_SOUND);
		CHECK(newSpec.ex1a == 10);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Change Horse Possession") {
		oldSpec.type = 15;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		oldSpec.ex1a = 4;
		oldSpec.ex2a = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::CHANGE_HORSE_OWNER);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.ex1a == 4);
		CHECK(newSpec.ex2a == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Change Boat Owner") {
		oldSpec.type = 16;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		oldSpec.ex1a = 4;
		oldSpec.ex2a = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::CHANGE_BOAT_OWNER);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.ex1a == 4);
		CHECK(newSpec.ex2a == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Set Town Visibility") {
		oldSpec.type = 17;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		oldSpec.ex1a = 4; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::SET_TOWN_VISIBILITY);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.ex1a == 4); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Major Event Occurred") {
		oldSpec.type = 18;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		oldSpec.ex1a = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::MAJOR_EVENT_OCCURRED);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.ex1a == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Forced Give") {
		oldSpec.type = 19;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		oldSpec.ex1a = 4;
		oldSpec.ex2b = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::FORCED_GIVE);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.ex1a == 4);
		CHECK(newSpec.ex2b == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Buy Items of Type") {
		oldSpec.type = 20;
		oldSpec.type = 19;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		oldSpec.ex1a = 4; oldSpec.ex1b = 5;
		oldSpec.ex2a = 6;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::FORCED_GIVE);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.ex1a == 4); CHECK(newSpec.ex1b == 5);
		CHECK(newSpec.ex2a == 6);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Call Scenario Special") {
		oldSpec.type = 21;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::CALL_GLOBAL);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Set Many Flags") {
		oldSpec.type = 22;
		oldSpec.sd1 = 4;
		oldSpec.ex1a = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::SET_SDF_ROW);
		CHECK(newSpec.sd1 == 4);
		CHECK(newSpec.ex1a == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Copy Flag") {
		oldSpec.type = 23;
		oldSpec.sd1 = 2; oldSpec.sd2 = 3;
		oldSpec.ex1a = 4; oldSpec.ex1b = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::COPY_SDF);
		CHECK(newSpec.sd1 == 2); CHECK(newSpec.sd2 == 3);
		CHECK(newSpec.ex1a == 4); CHECK(newSpec.ex1b == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Ritual of Sanctification") {
		oldSpec.type = 24;
		oldSpec.ex1b = 32;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_CONTEXT);
		CHECK(newSpec.ex1a == int(eSpecCtx::TARGET));
		CHECK(newSpec.ex1b == int(eSpell::RITUAL_SANCTIFY));
		CHECK(newSpec.ex1c == 12);
		CHECK(newSpec.jumpto == 32);
	}
	SECTION("Rest") {
		oldSpec.type = 25;
		oldSpec.ex1a = 2; oldSpec.ex1b = 3;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.ex1a == 2); CHECK(newSpec.ex1b == 3);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Wandering Will Fight") {
		oldSpec.type = 26;
		oldSpec.ex1a = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::CANT_ENTER);
		CHECK(newSpec.ex1a == 0);
		CHECK(newSpec.ex2a == 0);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("End Scenario") {
		oldSpec.type = 27;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::END_SCENARIO);
		CHECK(newSpec.jumpto == 12);
	}
	// Clean up after ourselves
	ResMgr::popPath<StringRsrc>();
}

TEST_CASE("When converting legacy special nodes (one-shot)") {
	cSpecial newSpec;
	legacy::special_node_type oldSpec = {0,-1,-1,0,-1,-1,-1,-1,-1,-1,-1};
	// Fetching opcodes requires strings to be available
	// Here we fetch them from the rsrc dir, rather than the data dir
	ResMgr::pushPath<StringRsrc>("../rsrc/strings");
	
	oldSpec.sd1 = 8; oldSpec.sd2 = 7;
	oldSpec.m1 = 4; oldSpec.m2 = 5;
	oldSpec.jumpto = 12;
	SECTION("Give Item") {
		oldSpec.type = 50;
		oldSpec.ex1a = 9;
		oldSpec.ex1b = 2500;
		oldSpec.ex2a = 1500;
		oldSpec.ex2b = 10;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_GIVE_ITEM);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 9);
		CHECK(newSpec.ex1b == 2500);
		CHECK(newSpec.ex2a == 1500);
		CHECK(newSpec.ex2b == 10);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Give Special Item") {
		oldSpec.type = 51;
		oldSpec.ex1a = 15;
		oldSpec.ex1b = 16;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_GIVE_SPEC_ITEM);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 15);
		CHECK(newSpec.ex1b == 16);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Do Nothing") {
		oldSpec.type = 52;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_NULL);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Set SDF") {
		oldSpec.type = 53;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_SET_SDF);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Message") {
		oldSpec.type = 54;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_DISPLAY_MSG);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Dialog") {
		oldSpec.type = 55;
		oldSpec.pic = 42;
		oldSpec.ex1a = 20; oldSpec.ex1b = 18;
		oldSpec.ex2a = 19; oldSpec.ex2b = 17;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_DIALOG);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.pictype == PIC_DLOG); CHECK(newSpec.pic == 42);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == -1); CHECK(newSpec.m3 == 5);
		CHECK(newSpec.ex1a == 9); CHECK(newSpec.ex1b == 18);
		CHECK(newSpec.ex2a == 19); CHECK(newSpec.ex2b == 17);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Dialog (terrain)") {
		oldSpec.type = 56;
		oldSpec.pic = 42;
		oldSpec.ex1a = 20; oldSpec.ex1b = 18;
		oldSpec.ex2a = 19; oldSpec.ex2b = 17;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_DIALOG);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.pictype == PIC_TER); CHECK(newSpec.pic == 42);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == -1); CHECK(newSpec.m3 == 5);
		CHECK(newSpec.ex1a == 9); CHECK(newSpec.ex1b == 18);
		CHECK(newSpec.ex2a == 19); CHECK(newSpec.ex2b == 17);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Dialog (monster)") {
		oldSpec.type = 57;
		oldSpec.pic = 42;
		oldSpec.ex1a = 20; oldSpec.ex1b = 18;
		oldSpec.ex2a = 19; oldSpec.ex2b = 17;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_DIALOG);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.pictype == PIC_MONST); CHECK(newSpec.pic == 42);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == -1); CHECK(newSpec.m3 == 5);
		CHECK(newSpec.ex1a == 9); CHECK(newSpec.ex1b == 18);
		CHECK(newSpec.ex2a == 19); CHECK(newSpec.ex2b == 17);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Give Item") {
		oldSpec.type = 58;
		oldSpec.pic = 42;
		oldSpec.ex1a = 20; oldSpec.ex1b = 18;
		oldSpec.ex2a = 19; oldSpec.ex2b = 17;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_GIVE_ITEM_DIALOG);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.pictype == PIC_DLOG); CHECK(newSpec.pic == 42);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == -1); CHECK(newSpec.m3 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 18);
		CHECK(newSpec.ex2a == 19); CHECK(newSpec.ex2b == 17);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Give Item (terrain)") {
		oldSpec.type = 59;
		oldSpec.pic = 42;
		oldSpec.ex1a = 20; oldSpec.ex1b = 18;
		oldSpec.ex2a = 19; oldSpec.ex2b = 17;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_GIVE_ITEM_DIALOG);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.pictype == PIC_TER); CHECK(newSpec.pic == 42);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == -1); CHECK(newSpec.m3 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 18);
		CHECK(newSpec.ex2a == 19); CHECK(newSpec.ex2b == 17);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Give Item (monster)") {
		oldSpec.type = 60;
		oldSpec.pic = 42;
		oldSpec.ex1a = 20; oldSpec.ex1b = 18;
		oldSpec.ex2a = 19; oldSpec.ex2b = 17;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_GIVE_ITEM_DIALOG);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.pictype == PIC_MONST); CHECK(newSpec.pic == 42);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == -1); CHECK(newSpec.m3 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 18);
		CHECK(newSpec.ex2a == 19); CHECK(newSpec.ex2b == 17);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Outdoor Encounter") {
		oldSpec.type = 61;
		oldSpec.ex1a = 2;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_OUT_ENCOUNTER);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Town Encounter") {
		oldSpec.type = 62;
		oldSpec.ex1a = 7;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_TOWN_ENCOUNTER);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Trap") {
		oldSpec.type = 63;
		oldSpec.pic = 7;
		oldSpec.ex1a = 14;
		oldSpec.ex1b = 15;
		oldSpec.ex2a = 16;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_TRAP);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.pictype == PIC_DLOG); CHECK(newSpec.pic == 27);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 14);
		CHECK(newSpec.ex1b == 15);
		CHECK(newSpec.ex2a == 16);
		CHECK(newSpec.jumpto == 12);
	}
	// Clean up after ourselves
	ResMgr::popPath<StringRsrc>();
}

TEST_CASE("When converting legacy special nodes (affect)") {
	cSpecial newSpec;
	legacy::special_node_type oldSpec = {0,-1,-1,0,-1,-1,-1,-1,-1,-1,-1};
	// Fetching opcodes requires strings to be available
	// Here we fetch them from the rsrc dir, rather than the data dir
	ResMgr::pushPath<StringRsrc>("../rsrc/strings");
	
	oldSpec.m1 = 4; oldSpec.m2 = 5;
	oldSpec.jumpto = 12;
	SECTION("Select a PC") {
		oldSpec.type = 80;
		oldSpec.ex1a = 1;
		oldSpec.ex1b = 10;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::SELECT_TARGET);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 1);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Do Damage") {
		oldSpec.type = 81;
		oldSpec.pic = 1;
		oldSpec.ex1a = 2; oldSpec.ex1b = 6;
		oldSpec.ex2a = 3; oldSpec.ex2b = 6;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::DAMAGE);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 2); CHECK(newSpec.ex1b == 6);
		CHECK(newSpec.ex2a == 3); CHECK(newSpec.ex2b == int(eDamageType::UNDEAD));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Health") {
		oldSpec.type = 82;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_HP);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Spell Points") {
		oldSpec.type = 83;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_SP);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Experience") {
		oldSpec.type = 84;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_XP);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Skill Points") {
		oldSpec.type = 85;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_SKILL_PTS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Deadness") {
		oldSpec.type = 86;
		oldSpec.ex1a = 2; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_DEADNESS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 2); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Poison") {
		oldSpec.type = 87;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::POISON));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Slow/Haste") {
		oldSpec.type = 88;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::HASTE_SLOW));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Invulnerability") {
		oldSpec.type = 89;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::INVULNERABLE));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Magic Resistance") {
		oldSpec.type = 90;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::MAGIC_RESISTANCE));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Webs") {
		oldSpec.type = 91;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::WEBS));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Disease") {
		oldSpec.type = 92;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::DISEASE));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Sanctuary") {
		oldSpec.type = 93;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::INVISIBLE));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Curse/Bless") {
		oldSpec.type = 94;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::BLESS_CURSE));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Dumbfounding") {
		oldSpec.type = 95;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::DUMB));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Sleep") {
		oldSpec.type = 96;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::ASLEEP));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Paralysis") {
		oldSpec.type = 97;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex1c == int(eStatus::PARALYZED));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Statistic") {
		oldSpec.type = 98;
		oldSpec.pic = 50;
		oldSpec.ex1a = 10; oldSpec.ex1b = 1;
		oldSpec.ex2a = 12;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_STAT);
		CHECK(newSpec.pic == 50);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 10); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.ex2a == int(eSkill::ALCHEMY));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Mage Spell") {
		oldSpec.type = 99;
		oldSpec.ex1a = 30;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_MAGE_SPELL);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 60); CHECK(newSpec.ex1b == 0);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Priest Spell") {
		oldSpec.type = 100;
		oldSpec.ex1a = 30;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_PRIEST_SPELL);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 60); CHECK(newSpec.ex1b == 0);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Gold") {
		oldSpec.type = 101;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_GOLD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Food") {
		oldSpec.type = 102;
		oldSpec.ex1a = 20; oldSpec.ex1b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_FOOD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 20); CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Alchemy") {
		oldSpec.type = 103;
		oldSpec.ex1a = 16;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_ALCHEMY);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 16);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Stealth") {
		oldSpec.type = 104;
		oldSpec.ex1a = 30;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_PARTY_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 30); CHECK(newSpec.ex1b == 0);
		CHECK(newSpec.ex2a == int(ePartyStatus::STEALTH));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Firewalk") {
		oldSpec.type = 105;
		oldSpec.ex1a = 30;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_PARTY_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 30); CHECK(newSpec.ex1b == 0);
		CHECK(newSpec.ex2a == int(ePartyStatus::FIREWALK));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Affect Flying") {
		oldSpec.type = 106;
		oldSpec.ex1a = 30;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::AFFECT_PARTY_STATUS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 30); CHECK(newSpec.ex1b == 0);
		CHECK(newSpec.ex2a == int(ePartyStatus::FLIGHT));
		CHECK(newSpec.jumpto == 12);
	}
	// Clean up after ourselves
	ResMgr::popPath<StringRsrc>();
}

TEST_CASE("When converting legacy special nodes (if-then)") {
	cSpecial newSpec;
	legacy::special_node_type oldSpec = {0,-1,-1,0,-1,-1,-1,-1,-1,-1,-1};
	// Fetching opcodes requires strings to be available
	// Here we fetch them from the rsrc dir, rather than the data dir
	ResMgr::pushPath<StringRsrc>("../rsrc/strings");
	
	oldSpec.jumpto = 12;
	SECTION("Stuff Done Flag?") {
		oldSpec.type = 130;
		oldSpec.sd1 = 8; oldSpec.sd2 = 7;
		oldSpec.ex1a = 1;
		oldSpec.ex1b = 10;
		oldSpec.ex2a = 9;
		oldSpec.ex2b = 13;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_SDF);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.ex1a == 1);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 9);
		CHECK(newSpec.ex2b == 13);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Town Number?") {
		oldSpec.type = 131;
		oldSpec.ex1a = 12; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_TOWN_NUM);
		CHECK(newSpec.ex1a == 12); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Random Number?") {
		oldSpec.type = 132;
		oldSpec.ex1a = 100; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_RANDOM);
		CHECK(newSpec.ex1a == 100); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Have Special Item?") {
		oldSpec.type = 133;
		oldSpec.ex1a = 17; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_HAVE_SPECIAL_ITEM);
		CHECK(newSpec.ex1a == 17); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Stuff Done Compare?") {
		oldSpec.type = 134;
		oldSpec.sd1 = 4; oldSpec.sd2 = 5;
		oldSpec.ex1a = 6; oldSpec.ex1b = 7;
		oldSpec.ex2b = 8;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_SDF_COMPARE);
		CHECK(newSpec.sd1 == 4); CHECK(newSpec.sd2 == 5);
		CHECK(newSpec.ex1a == 6); CHECK(newSpec.ex1b == 7);
		CHECK(newSpec.ex2b == 8);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Town Terrain This Type?") {
		oldSpec.type = 135;
		oldSpec.ex1a = 13; oldSpec.ex1b = 14;
		oldSpec.ex2a = 15; oldSpec.ex2b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_TER_TYPE);
		CHECK(newSpec.ex1a == 13); CHECK(newSpec.ex1b == 14);
		CHECK(newSpec.ex2a == 15); CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Outdoor Terrain This Type?") {
		oldSpec.type = 136;
		oldSpec.ex1a = 13; oldSpec.ex1b = 14;
		oldSpec.ex2a = 15; oldSpec.ex2b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_TER_TYPE);
		CHECK(newSpec.ex1a == 13); CHECK(newSpec.ex1b == 14);
		CHECK(newSpec.ex2a == 15); CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Has Gold?") {
		oldSpec.type = 137;
		oldSpec.ex1a = 100; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_HAS_GOLD);
		CHECK(newSpec.ex1a == 100); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == 0);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Has Food?") {
		oldSpec.type = 138;
		oldSpec.ex1a = 100; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_HAS_FOOD);
		CHECK(newSpec.ex1a == 100); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == 0);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Item Class on Space?") {
		oldSpec.type = 139;
		oldSpec.ex1a = 15; oldSpec.ex1b = 16;
		oldSpec.ex2a = 100; oldSpec.ex2b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_ITEM_CLASS_ON_SPACE);
		CHECK(newSpec.ex1a == 15); CHECK(newSpec.ex1b == 16);
		CHECK(newSpec.ex2a == 100); CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.ex2c == 0);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Have Item with Class?") {
		oldSpec.type = 140;
		oldSpec.ex1a = 100; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_HAVE_ITEM_CLASS);
		CHECK(newSpec.ex1a == 100); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == 0);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Equipped Item with Class?") {
		oldSpec.type = 141;
		oldSpec.ex1a = 100; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_EQUIP_ITEM_CLASS);
		CHECK(newSpec.ex1a == 100); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == 0);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Has Gold? (and take)") {
		oldSpec.type = 142;
		oldSpec.ex1a = 100; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_HAS_GOLD);
		CHECK(newSpec.ex1a == 100); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Has Food? (and take)") {
		oldSpec.type = 143;
		oldSpec.ex1a = 100; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_HAS_FOOD);
		CHECK(newSpec.ex1a == 100); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Item Class on Space? (and take)") {
		oldSpec.type = 144;
		oldSpec.ex1a = 15; oldSpec.ex1b = 16;
		oldSpec.ex2a = 100; oldSpec.ex2b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_ITEM_CLASS_ON_SPACE);
		CHECK(newSpec.ex1a == 15); CHECK(newSpec.ex1b == 16);
		CHECK(newSpec.ex2a == 100); CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.ex2c == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Have Item with Class? (and take)") {
		oldSpec.type = 145;
		oldSpec.ex1a = 100; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_HAVE_ITEM_CLASS);
		CHECK(newSpec.ex1a == 100); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Equipped Item with Class? (and take)") {
		oldSpec.type = 146;
		oldSpec.ex1a = 100; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_EQUIP_ITEM_CLASS);
		CHECK(newSpec.ex1a == 100); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Day Reached?") {
		oldSpec.type = 147;
		oldSpec.ex1a = 25; oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_DAY_REACHED);
		CHECK(newSpec.ex1a == 25); CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Any Barrels?") {
		oldSpec.type = 148;
		oldSpec.ex1b = 13;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_FIELDS);
		CHECK(newSpec.sd1 == 1); CHECK(newSpec.sd2 >= 64 * 64);
		CHECK(newSpec.m1 == int(OBJECT_BARREL)); CHECK(newSpec.m2 == 13);
		CHECK(newSpec.ex1a == 0); CHECK(newSpec.ex1b == 0);
		CHECK(newSpec.ex2a == 64); CHECK(newSpec.ex2b == 64);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Any Crates?") {
		oldSpec.type = 149;
		oldSpec.ex1b = 13;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_FIELDS);
		CHECK(newSpec.sd1 == 1); CHECK(newSpec.sd2 >= 64 * 64);
		CHECK(newSpec.m1 == int(OBJECT_CRATE)); CHECK(newSpec.m2 == 13);
		CHECK(newSpec.ex1a == 0); CHECK(newSpec.ex1b == 0);
		CHECK(newSpec.ex2a == 64); CHECK(newSpec.ex2b == 64);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Special Thing Happened?") {
		oldSpec.type = 150;
		oldSpec.ex1a = 15; oldSpec.ex1b = 13;
		oldSpec.ex2b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_EVENT_OCCURRED);
		CHECK(newSpec.ex1a == 15); CHECK(newSpec.ex1b == 13);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Has Cave Lore?") {
		oldSpec.type = 151;
		oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_TRAIT);
		CHECK(newSpec.ex1a == int(eTrait::CAVE_LORE));
		CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == 1); // need one PC ...
		CHECK(newSpec.ex2b == 2); // ... or more
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Has Woodsman?") {
		oldSpec.type = 152;
		oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_TRAIT);
		CHECK(newSpec.ex1a == int(eTrait::WOODSMAN));
		CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == 1); // need one PC ...
		CHECK(newSpec.ex2b == 2); // ... or more
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Has Enough Mage Lore?") {
		oldSpec.type = 153;
		oldSpec.ex1a = 20;
		oldSpec.ex1b = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_STATISTIC);
		CHECK(newSpec.ex1a == 20);
		CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == int(eSkill::MAGE_LORE));
		CHECK(newSpec.ex2b == 0); // cumulative check
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Text Response?") {
		oldSpec.type = 154;
		oldSpec.pic = 10;
		oldSpec.ex1a = 167; oldSpec.ex1b = 12;
		oldSpec.ex2a = 169; oldSpec.ex2b = 15;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_TEXT_RESPONSE);
		CHECK(newSpec.pic == 10);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 12);
		CHECK(newSpec.ex2a == 9); CHECK(newSpec.ex2b == 15);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Stuff Done Equal?") {
		oldSpec.type = 155;
		oldSpec.sd1 = 5; oldSpec.sd2 = 6;
		oldSpec.ex1a = 13; oldSpec.ex1b = 14;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_SDF_EQ);
		CHECK(newSpec.sd1 == 5); CHECK(newSpec.sd2 == 6);
		CHECK(newSpec.ex1a == 13); CHECK(newSpec.ex1b == 14);
		CHECK(newSpec.jumpto == 12);
	}
	// Clean up after ourselves
	ResMgr::popPath<StringRsrc>();
}

TEST_CASE("When converting legacy special nodes (town)") {
	cSpecial newSpec;
	legacy::special_node_type oldSpec = {0,-1,-1,0,-1,-1,-1,-1,-1,-1,-1};
	// Fetching opcodes requires strings to be available
	// Here we fetch them from the rsrc dir, rather than the data dir
	ResMgr::pushPath<StringRsrc>("../rsrc/strings");
	
	oldSpec.m1 = 4; oldSpec.m2 = 5;
	oldSpec.jumpto = 12;
	SECTION("Town Hostile") {
		oldSpec.type = 170;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::MAKE_TOWN_HOSTILE);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Change Terrain") {
		oldSpec.type = 171;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 9;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::CHANGE_TER);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 9);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Swap Terrain") {
		oldSpec.type = 172;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 9; oldSpec.ex2b = 10;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::SWAP_TER);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 9); CHECK(newSpec.ex2b == 10);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Transform Terrain") {
		oldSpec.type = 173;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TRANS_TER);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Move Party") {
		oldSpec.type = 174;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 2;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_MOVE_PARTY);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 2);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Hit Space") {
		oldSpec.type = 175;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 9; oldSpec.ex2b = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_HIT_SPACE);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 9); CHECK(newSpec.ex2b == int(eDamageType::COLD));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Explode Space") {
		oldSpec.type = 176;
		oldSpec.pic = 30;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 9; oldSpec.ex2b = 3;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_EXPLODE_SPACE);
		CHECK(newSpec.pic == 30);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 9); CHECK(newSpec.ex2b == int(eDamageType::MAGIC));
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Lock space") {
		oldSpec.type = 177;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_LOCK_SPACE);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Unlock space") {
		oldSpec.type = 178;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_UNLOCK_SPACE);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("SFX Burst") {
		oldSpec.type = 179;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 2;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_SFX_BURST);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 2);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Make Wandering Monster") {
		oldSpec.type = 180;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_CREATE_WANDERING);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Monster") {
		oldSpec.type = 181;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 40; oldSpec.ex2b = 100;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_PLACE_MONST);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 40); CHECK(newSpec.ex2b == 100);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Destroy Monsters") {
		oldSpec.type = 182;
		oldSpec.ex1a = 50;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_NUKE_MONSTS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 50);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Destroy All Monsters") {
		oldSpec.type = 183;
		oldSpec.ex1a = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_NUKE_MONSTS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == -1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Generic Lever") {
		oldSpec.type = 184;
		oldSpec.ex1b = 12;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_GENERIC_LEVER);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1b == 12);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Generic Portal") {
		oldSpec.type = 185;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_GENERIC_PORTAL);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Generic Button") {
		oldSpec.type = 186;
		oldSpec.ex1b = 12;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_GENERIC_BUTTON);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1b == 12);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Generic Stairway") {
		oldSpec.type = 187;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 12; oldSpec.ex2b = 5;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_GENERIC_STAIR);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 12); CHECK(newSpec.ex2b == 5);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Lever") {
		oldSpec.type = 188;
		oldSpec.pic = 12;
		oldSpec.ex1b = 15;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_LEVER);
		CHECK(newSpec.pictype == PIC_TER); CHECK(newSpec.pic == 12);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == -1); CHECK(newSpec.m3 == 5);
		CHECK(newSpec.ex1b == 15);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Portal") {
		oldSpec.type = 189;
		oldSpec.pic = 12;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_PORTAL);
		CHECK(newSpec.pictype == PIC_DLOG); CHECK(newSpec.pic == 12);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == -1); CHECK(newSpec.m3 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Stairway") {
		oldSpec.type = 190;
		oldSpec.pic = 12;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 15; oldSpec.ex2b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_STAIR);
		CHECK(newSpec.pictype == PIC_DLOG); CHECK(newSpec.pic == 12);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == -1); CHECK(newSpec.m3 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 15); CHECK(newSpec.ex2b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Relocate Outdoors") {
		oldSpec.type = 191;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 9; oldSpec.ex2b = 10;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_RELOCATE);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 9); CHECK(newSpec.ex2b == 10);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Item") {
		oldSpec.type = 192;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 50;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_PLACE_ITEM);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(oldSpec.ex2a == 50);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Split Party") {
		oldSpec.type = 193;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 10;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_SPLIT_PARTY);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 10);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Reunite Party") {
		oldSpec.type = 194;
		oldSpec.ex1a = 10;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_REUNITE_PARTY);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 10);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Start Timer") {
		oldSpec.type = 195;
		oldSpec.ex1a = 7;
		oldSpec.ex2b = 8;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_TIMER_START);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7);
		CHECK(newSpec.ex2b == 8);
		CHECK(newSpec.jumpto == 12);
	}
	// Clean up after ourselves
	ResMgr::popPath<StringRsrc>();
}

TEST_CASE("When converting legacy special nodes (rect)") {
	cSpecial newSpec;
	legacy::special_node_type oldSpec = {0,-1,-1,0,-1,-1,-1,-1,-1,-1,-1};
	// Fetching opcodes requires strings to be available
	// Here we fetch them from the rsrc dir, rather than the data dir
	ResMgr::pushPath<StringRsrc>("../rsrc/strings");
	
	oldSpec.m1 = 4; oldSpec.m2 = 5;
	oldSpec.pic = 1;
	oldSpec.ex1a = 2;
	oldSpec.ex1b = 10;
	oldSpec.ex2a = 4;
	oldSpec.ex2b = 20;
	oldSpec.jumpto = 12;
	SECTION("Place Fire Wall") {
		oldSpec.type = 200;
		oldSpec.sd1 = 75;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == WALL_FIRE);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Force Wall") {
		oldSpec.type = 201;
		oldSpec.sd1 = 75;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == WALL_FORCE);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Ice Wall") {
		oldSpec.type = 202;
		oldSpec.sd1 = 75;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == WALL_ICE);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Blade Wall") {
		oldSpec.type = 203;
		oldSpec.sd1 = 75;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == WALL_BLADES);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Stinking Cloud") {
		oldSpec.type = 204;
		oldSpec.sd1 = 75;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == CLOUD_STINK);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Sleep Cloud") {
		oldSpec.type = 205;
		oldSpec.sd1 = 75;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == CLOUD_SLEEP);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Quickfire") {
		oldSpec.type = 206;
		oldSpec.sd1 = 75;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == FIELD_QUICKFIRE);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Fire Barrier") {
		oldSpec.type = 207;
		oldSpec.sd1 = 75;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == BARRIER_FIRE);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Force Barrier") {
		oldSpec.type = 208;
		oldSpec.sd1 = 75;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == BARRIER_FORCE);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Cleanse") {
		oldSpec.type = 209;
		oldSpec.sd1 = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 1);
		CHECK(newSpec.sd2 == FIELD_DISPEL);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Special Effects") {
		oldSpec.type = 210;
		oldSpec.sd1 = 75;
		oldSpec.sd2 = 4;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == SFX_LARGE_SLIME);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Permanent Effects") {
		oldSpec.type = 211;
		oldSpec.sd1 = 75;
		oldSpec.sd2 = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_PLACE_FIELD);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == OBJECT_BARREL);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Move Items") {
		oldSpec.type = 212;
		oldSpec.sd1 = 25;
		oldSpec.sd2 = 26;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_MOVE_ITEMS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 25);
		CHECK(newSpec.sd2 == 26);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Destroy Items") {
		oldSpec.type = 213;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_DESTROY_ITEMS);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Change Terrain") {
		oldSpec.type = 214;
		oldSpec.sd1 = 75;
		oldSpec.sd2 = 80;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_CHANGE_TER);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == 80);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Swap Terrain") {
		oldSpec.type = 215;
		oldSpec.sd1 = 75;
		oldSpec.sd2 = 80;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_SWAP_TER);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.sd1 == 75);
		CHECK(newSpec.sd2 == 80);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Transform Terrain") {
		oldSpec.type = 216;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_TRANS_TER);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Lock Terrain") {
		oldSpec.type = 217;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_LOCK);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Unlock Terrain") {
		oldSpec.type = 218;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::RECT_UNLOCK);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.pic == 1);
		CHECK(newSpec.ex1a == 2);
		CHECK(newSpec.ex1b == 10);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 20);
		CHECK(newSpec.jumpto == 12);
	}
	// Clean up after ourselves
	ResMgr::popPath<StringRsrc>();
}

TEST_CASE("When converting legacy special nodes (outdoors)") {
	cSpecial newSpec;
	legacy::special_node_type oldSpec = {0,-1,-1,0,-1,-1,-1,-1,-1,-1,-1};
	// Fetching opcodes requires strings to be available
	// Here we fetch them from the rsrc dir, rather than the data dir
	ResMgr::pushPath<StringRsrc>("../rsrc/strings");
	
	oldSpec.jumpto = 12;
	SECTION("Make Outdoor Wandering") {
		oldSpec.type = 225;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::OUT_MAKE_WANDER);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Change Terrain") {
		oldSpec.type = 226;
		oldSpec.m1 = 4; oldSpec.m2 = 5;
		oldSpec.ex1a = 7; oldSpec.ex1b = 8;
		oldSpec.ex2a = 9;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::CHANGE_TER);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 7); CHECK(newSpec.ex1b == 8);
		CHECK(newSpec.ex2a == 9);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Place Outdoor Encounter") {
		oldSpec.type = 227;
		oldSpec.m1 = 4; oldSpec.m2 = 5;
		oldSpec.ex1a = 3;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::OUT_PLACE_ENCOUNTER);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 3);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Move Party") {
		oldSpec.type = 228;
		oldSpec.m1 = 4; oldSpec.m2 = 5;
		oldSpec.ex1a = 25; oldSpec.ex1b = 26;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::OUT_MOVE_PARTY);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 25); CHECK(newSpec.ex1b == 26);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Outdoor Store") {
		oldSpec.type = 229;
		oldSpec.m1 = 8;
		oldSpec.ex1a = 1; oldSpec.ex1b = 2;
		oldSpec.ex2a = 5; oldSpec.ex2b = 4;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::ENTER_SHOP);
		CHECK(newSpec.m1 == 8);
		CHECK(newSpec.ex1a == 31);
		CHECK(newSpec.ex1b == 2);
		CHECK(newSpec.ex2a == 5);
		CHECK(newSpec.ex2b == 4);
	}
	// Clean up after ourselves
	ResMgr::popPath<StringRsrc>();
}

TEST_CASE("When converting classic special nodes") {
	cSpecial newSpec;
	legacy::special_node_type oldSpec = {0,-1,-1,0,-1,-1,-1,-1,-1,-1,-1};
	// Fetching opcodes requires strings to be available
	// Here we fetch them from the rsrc dir, rather than the data dir
	ResMgr::pushPath<StringRsrc>("../rsrc/strings");
	
	oldSpec.jumpto = 12;
	SECTION("If Statistic") {
		oldSpec.type = 153;
		oldSpec.ex1a = 20;
		oldSpec.ex1b = 20;
		oldSpec.ex2a = 20;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_STATISTIC);
		CHECK(newSpec.ex1a == 20);
		CHECK(newSpec.ex1b == 20);
		CHECK(newSpec.ex2a == int(eSkill::MAX_HP));
		CHECK(newSpec.ex2b == 0); // cumulative check
		CHECK(newSpec.jumpto == 12);
	}
	/* Disabled for now since it shows a dialog
	SECTION("Display Picture") {
		oldSpec.type = 28;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::DISPLAY_PICTURE);
		CHECK(newSpec.ex1a == 0);
		CHECK(newSpec.jumpto == 12);
	}
	*/
	SECTION("Random") {
		oldSpec.type = 29;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::SDF_RANDOM);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("If Species") {
		oldSpec.type = 156;
		oldSpec.ex1a = 2; oldSpec.ex1b = 50;
		oldSpec.ex2a = 4; oldSpec.ex2b = 0;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_SPECIES);
		CHECK(newSpec.ex1a == int(eRace::SLITH));
		CHECK(newSpec.ex1b == 50);
		CHECK(newSpec.ex2a == 4);
		CHECK(newSpec.ex2b == 0);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Town Change Lighting") {
		oldSpec.type = 196;
		oldSpec.ex1a = 2;
		oldSpec.ex2a = 15;
		oldSpec.ex2b = 1;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_CHANGE_LIGHTING);
		CHECK(newSpec.ex1a == LIGHT_DRAINS);
		CHECK(newSpec.ex2a == 15);
		CHECK(newSpec.ex2b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Town Set Attitude") {
		oldSpec.type = 197;
		oldSpec.ex1a = 20; oldSpec.ex1b = 2;
		newSpec.import_legacy(oldSpec);
		CHECK(newSpec.type == eSpecType::TOWN_SET_ATTITUDE);
		CHECK(newSpec.ex1a == 20);
		CHECK(newSpec.ex1b == int(eAttitude::FRIENDLY));
		CHECK(newSpec.jumpto == 12);
	}
	// Clean up after ourselves
	ResMgr::popPath<StringRsrc>();
}

ostream& operator<< (ostream& out, eSpecType spec) {
	out << (*spec).opcode();
	return out;
}
