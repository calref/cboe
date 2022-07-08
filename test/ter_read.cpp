//
//  ter_read.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-07-29.
//
//

#include <fstream>
#include "ticpp.h"
#include "catch.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "scenario/scenario.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readTerrainFromXml(Document&& data, cScenario& scenario);

TEST_CASE("Loading a terrain type definition") {
	ifstream fin;
	cScenario scen;
	Document doc;
	fin.exceptions(ios::badbit);
	
	SECTION("When the root tag is wrong") {
		fin.open("files/bad_root.xml");
		doc = xmlDocFromStream(fin, "bad_root.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the version attribute is missing") {
		fin.open("files/terrain/no_version.xml");
		doc = xmlDocFromStream(fin, "no_version.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("When the root tag has a bad attribute") {
		fin.open("files/terrain/bad_root_attr.xml");
		doc = xmlDocFromStream(fin, "bad_root_attr.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("When an unknown toplevel tag appears") {
		fin.open("files/terrain/bad_toplevel.xml");
		doc = xmlDocFromStream(fin, "bad_toplevel.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the ID attribute is missing") {
		fin.open("files/terrain/missing_id.xml");
		doc = xmlDocFromStream(fin, "missing_id.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), Exception);
	}
	SECTION("When a required subtag is missing") {
		fin.open("files/terrain/missing_req.xml");
		doc = xmlDocFromStream(fin, "missing_req.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When an invalid subtag is found") {
		fin.open("files/terrain/bad_tag.xml");
		doc = xmlDocFromStream(fin, "bad_tag.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the blockage type is invalid") {
		fin.open("files/terrain/bad_block.xml");
		doc = xmlDocFromStream(fin, "bad_block.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), Exception);
	}
	SECTION("When the trim type is invalid") {
		fin.open("files/terrain/bad_trim.xml");
		doc = xmlDocFromStream(fin, "bad_trim.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), Exception);
	}
	SECTION("When the special ability type is missing") {
		fin.open("files/terrain/missing_abil.xml");
		doc = xmlDocFromStream(fin, "missing_abil.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When the special ability type is invalid") {
		fin.open("files/terrain/bad_abil.xml");
		doc = xmlDocFromStream(fin, "bad_abil.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), Exception);
	}
	SECTION("When there are too many special ability flags") {
		fin.open("files/terrain/too_many_flags.xml");
		doc = xmlDocFromStream(fin, "too_many_flags.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the special ability has an invalid subtag") {
		fin.open("files/terrain/bad_abil_tag.xml");
		doc = xmlDocFromStream(fin, "bad_abil_tag.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xBadNode);
	}
	SECTION("With an incomplete object definition") {
		fin.open("files/terrain/object_missing.xml");
		doc = xmlDocFromStream(fin, "object_missing.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("With an invalid object definition") {
		fin.open("files/terrain/object_bad.xml");
		doc = xmlDocFromStream(fin, "object_bad.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xBadNode);
	}
	SECTION("With an invalid editor subtag") {
		fin.open("files/terrain/bad_editor.xml");
		doc = xmlDocFromStream(fin, "bad_editor.xml");
		REQUIRE_THROWS_AS(readTerrainFromXml(move(doc), scen), xBadNode);
	}
	SECTION("With a default-chance frill reference") {
		fin.open("files/terrain/default_frill.xml");
		doc = xmlDocFromStream(fin, "default_frill.xml");
		REQUIRE_NOTHROW(readTerrainFromXml(move(doc), scen));
		CHECK(scen.ter_types[0].frill_for == 0);
		CHECK(scen.ter_types[0].frill_chance == 10);
	}
	SECTION("With the minimal required data") {
		fin.open("files/terrain/minimal.xml");
		doc = xmlDocFromStream(fin, "minimal.xml");
		REQUIRE_NOTHROW(readTerrainFromXml(move(doc), scen));
		REQUIRE(scen.ter_types.size() >= 1);
		CHECK(scen.ter_types[0].name == "Test Terrain");
		CHECK(scen.ter_types[0].picture == 0);
		CHECK(scen.ter_types[0].map_pic == 0);
		CHECK(scen.ter_types[0].combat_arena == 0);
		CHECK(scen.ter_types[0].blockage == eTerObstruct::BLOCK_MOVE);
		CHECK(scen.ter_types[0].ground_type == 0);
		CHECK(scen.ter_types[0].trim_type == eTrimType::NONE);
		CHECK(scen.ter_types[0].special == eTerSpec::NONE);
		CHECK(scen.ter_types[0].step_sound == eStepSnd::STEP);
		CHECK_FALSE(scen.ter_types[0].fly_over);
		CHECK_FALSE(scen.ter_types[0].boat_over);
		CHECK_FALSE(scen.ter_types[0].block_horse);
		CHECK_FALSE(scen.ter_types[0].is_archetype);
	}
	SECTION("With all possible data") {
		fin.open("files/terrain/full.xml");
		doc = xmlDocFromStream(fin, "full.xml");
		REQUIRE_NOTHROW(readTerrainFromXml(move(doc), scen));
		CHECK(scen.ter_types[0].special == eTerSpec::DAMAGING);
		CHECK(scen.ter_types[0].flag1 == 4);
		CHECK(scen.ter_types[0].flag2 == 6);
		CHECK(scen.ter_types[0].flag3 == 3);
		CHECK(scen.ter_types[0].trans_to_what == 10);
		CHECK(scen.ter_types[0].fly_over);
		CHECK(scen.ter_types[0].boat_over);
		CHECK(scen.ter_types[0].block_horse);
		CHECK(scen.ter_types[0].is_archetype);
		CHECK(scen.ter_types[0].light_radius == 3);
		CHECK(scen.ter_types[0].step_sound == eStepSnd::SPLASH);
		CHECK(scen.ter_types[0].ground_type == 2);
		CHECK(scen.ter_types[0].trim_ter == 1);
		CHECK(scen.ter_types[0].shortcut_key == 'u');
		CHECK(scen.ter_types[0].frill_for == 9);
		CHECK(scen.ter_types[0].frill_chance == 30);
		CHECK(scen.ter_types[0].obj_num == 1);
		CHECK(scen.ter_types[0].obj_pos == loc(0,0));
		CHECK(scen.ter_types[0].obj_size == loc(2,1));
	}
}
