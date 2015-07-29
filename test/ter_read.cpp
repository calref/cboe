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
#include "dialog.hpp"
#include "scenario.hpp"

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
		CHECK(scen.ter_types[0].trim_type == eTrimType::NONE);
		CHECK(scen.ter_types[0].special == eTerSpec::NONE);
	}
}
