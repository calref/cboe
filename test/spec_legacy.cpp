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
#include "restypes.hpp"
#include "pictypes.hpp"

using namespace std;

ostream& operator<< (ostream& out, eSpecType spec);

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
		CHECK(newSpec.ex1a == 1);
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
	
	oldSpec.ex1a = 2;
	oldSpec.ex1b = 10;
	oldSpec.ex2a = 4;
	oldSpec.ex2b = 20;
	oldSpec.jumpto = 12;
	SECTION("Place Fire Wall") {
		oldSpec.type = 200;
		oldSpec.m1 = 4; oldSpec.m2 = 5;
		oldSpec.pic = 1;
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
	// Clean up after ourselves
	ResMgr::popPath<StringRsrc>();
}

ostream& operator<< (ostream& out, eSpecType spec) {
	out << (*spec).opcode();
	return out;
}
