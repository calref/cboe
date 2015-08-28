//
//  town_read.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-08-27.
//
//

#include <fstream>
#include "ticpp.h"
#include "catch.hpp"
#include "dialog.hpp"
#include "scenario.hpp"
#include "town.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readTownFromXml(Document&& data, cTown*& town, cScenario& scen);

TEST_CASE("Loading a town definition") {
	ifstream fin;
	cScenario scen;
	cTown* town = nullptr;
	Document doc;
	fin.exceptions(ios::badbit);
	
	SECTION("When the root tag is wrong") {
		fin.open("files/bad_root.xml");
		doc = xmlDocFromStream(fin, "bad_root.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadNode);
	}
	SECTION("When the version attribute is missing") {
		fin.open("files/town/no_version.xml");
		doc = xmlDocFromStream(fin, "no_version.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xMissingAttr);
	}
	SECTION("When the root tag has a bad attribute") {
		fin.open("files/town/bad_root_attr.xml");
		doc = xmlDocFromStream(fin, "bad_root_attr.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadAttr);
	}
	SECTION("When an essential toplevel element is missing") {
		fin.open("files/town/missing_toplevel.xml");
		doc = xmlDocFromStream(fin, "missing_toplevel.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xMissingElem);
	}
	SECTION("When the size is not first") {
		fin.open("files/town/size_not_first.xml");
		doc = xmlDocFromStream(fin, "size_not_first.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadNode);
	}
	SECTION("When there is an invalid toplevel node") {
		fin.open("files/town/bad_toplevel.xml");
		doc = xmlDocFromStream(fin, "bad_toplevel.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadNode);
	}
	SECTION("When there are too many comments") {
		fin.open("files/town/too_many_comments.xml");
		doc = xmlDocFromStream(fin, "too_many_comments.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadNode);
	}
	SECTION("When the onenter condition is invalid") {
		fin.open("files/town/bad_onenter_condition.xml");
		doc = xmlDocFromStream(fin, "bad_onenter_condition.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadVal);
	}
	SECTION("When the onexit direction is invalid") {
		fin.open("files/town/bad_onexit_dir.xml");
		doc = xmlDocFromStream(fin, "bad_onexit_dir.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadVal);
	}
	SECTION("When the exit direction is invalid") {
		fin.open("files/town/bad_exit_dir.xml");
		doc = xmlDocFromStream(fin, "bad_exit_dir.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadVal);
	}
	SECTION("When there are too many timers") {
		fin.open("files/town/too_many_timers.xml");
		doc = xmlDocFromStream(fin, "too_many_timers.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadNode);
	}
	SECTION("When there is an invalid flag") {
		fin.open("files/town/bad_flag.xml");
		doc = xmlDocFromStream(fin, "bad_flag.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadNode);
	}
	SECTION("When there is a bad wandering definition") {
		fin.open("files/town/bad_wandering.xml");
		doc = xmlDocFromStream(fin, "bad_wandering.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadNode);
	}
	SECTION("When there is an empty wandering definition") {
		fin.open("files/town/empty_wandering.xml");
		doc = xmlDocFromStream(fin, "empty_wandering.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xMissingElem);
	}
	SECTION("When there is a preset item with an invalid tag") {
		fin.open("files/town/bad_item.xml");
		doc = xmlDocFromStream(fin, "bad_item.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadNode);
	}
	SECTION("When there is a preset item without a type") {
		fin.open("files/town/empty_item.xml");
		doc = xmlDocFromStream(fin, "empty_item.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xMissingElem);
	}
	SECTION("When there is a preset creature with an invalid tag") {
		fin.open("files/town/bad_creature.xml");
		doc = xmlDocFromStream(fin, "bad_creature.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadNode);
	}
	SECTION("When there is a preset creature without a type") {
		fin.open("files/town/empty_creature.xml");
		doc = xmlDocFromStream(fin, "empty_creature.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xMissingElem);
	}
	SECTION("With the minimal required data") {
		fin.open("files/town/minimal.xml");
		doc = xmlDocFromStream(fin, "minimal.xml");
		REQUIRE_NOTHROW(readTownFromXml(move(doc), town, scen));
		CHECK(town->max_dim() == 32);
		CHECK(town->town_name == "Test Town");
		CHECK(town->in_town_rect == rect(4, 4, 28, 28));
		CHECK(town->difficulty == 1);
		CHECK(town->lighting_type == LIGHT_NORMAL);
	}
	
	delete town;
}
