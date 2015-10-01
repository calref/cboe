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

using namespace std;

ostream& operator<< (ostream& out, eSpecType spec);

TEST_CASE("When converting legacy special nodes (general)") {
	cSpecial newSpec;
	legacy::special_node_type oldSpec = {0,-1,-1,0,-1,-1,-1,-1,-1,-1,-1};
	// Fetching opcodes requires strings to be available
	// Here we fetch them from the rsrc dir, rather than the data dir
	ResMgr::pushPath<StringRsrc>("../rsrc/strings");
	
	oldSpec.jumpto = 12;
	SECTION("Set Flag") {
		oldSpec.type = 1;
		oldSpec.m1 = 2; oldSpec.m2 = 3;
		oldSpec.sd1 = 4; oldSpec.sd2 = 5;
		oldSpec.ex1a = 6;
		newSpec.append(oldSpec);
		CHECK(newSpec.type == eSpecType::SET_SDF);
		CHECK(newSpec.m1 == 2); CHECK(newSpec.m2 == 3);
		CHECK(newSpec.sd1 == 4); CHECK(newSpec.sd2 == 5);
		CHECK(newSpec.ex1a == 6);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Secret Passage") {
		oldSpec.type = 4;
		newSpec.append(oldSpec);
		CHECK(newSpec.type == eSpecType::CANT_ENTER);
		CHECK(newSpec.ex1a == 0);
		CHECK(newSpec.ex2a == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Out Block") {
		oldSpec.type = 7;
		oldSpec.m1 = 4; oldSpec.m2 = 5;
		oldSpec.ex1a = 1;
		newSpec.append(oldSpec);
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
		newSpec.append(oldSpec);
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
		newSpec.append(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_CONTEXT);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == int(eSpecCtx::COMBAT_MOVE));
		CHECK(newSpec.ex1b == 1);
		CHECK(newSpec.jumpto == 12);
	}
	SECTION("Looking Block") {
		oldSpec.type = 10;
		oldSpec.m1 = 4; oldSpec.m2 = 5;
		newSpec.append(oldSpec);
		CHECK(newSpec.type == eSpecType::IF_LOOKING);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
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
		newSpec.append(oldSpec);
		CHECK(newSpec.type == eSpecType::ONCE_GIVE_ITEM);
		CHECK(newSpec.sd1 == 8); CHECK(newSpec.sd2 == 7);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
		CHECK(newSpec.ex1a == 9);
		CHECK(newSpec.ex1b == 2500);
		CHECK(newSpec.ex2a == 1500);
		CHECK(newSpec.ex2b == 10);
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
		newSpec.append(oldSpec);
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
		newSpec.append(oldSpec);
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
	SECTION("Town Hostile") {
		oldSpec.type = 170;
		oldSpec.jumpto = 12;
		newSpec.append(oldSpec);
		CHECK(newSpec.type == eSpecType::MAKE_TOWN_HOSTILE);
		CHECK(newSpec.m1 == 4); CHECK(newSpec.m2 == 5);
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
		newSpec.append(oldSpec);
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
		newSpec.append(oldSpec);
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
