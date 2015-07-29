//
//  item_read.cpp
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
extern void readItemsFromXml(Document&& data, cScenario& scenario);

TEST_CASE("Loading an item type definition") {
	ifstream fin;
	cScenario scen;
	Document doc;
	fin.exceptions(ios::badbit);
	
	SECTION("When the root tag is wrong") {
		fin.open("files/bad_root.xml");
		doc = xmlDocFromStream(fin, "bad_root.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the version attribute is missing") {
		fin.open("files/items/no_version.xml");
		doc = xmlDocFromStream(fin, "no_version.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("When the root tag has a bad attribute") {
		fin.open("files/items/bad_root_attr.xml");
		doc = xmlDocFromStream(fin, "bad_root_attr.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("When an unknown toplevel tag appears") {
		fin.open("files/items/bad_toplevel.xml");
		doc = xmlDocFromStream(fin, "bad_toplevel.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the ID attribute is missing") {
		fin.open("files/items/missing_id.xml");
		doc = xmlDocFromStream(fin, "missing_id.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), Exception);
	}
	SECTION("When a required subtag is missing") {
		fin.open("files/items/missing_req.xml");
		doc = xmlDocFromStream(fin, "missing_req.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When a bad subtag is found") {
		fin.open("files/items/bad_tag.xml");
		doc = xmlDocFromStream(fin, "bad_tag.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When an unknown property is found") {
		fin.open("files/items/bad_prop.xml");
		doc = xmlDocFromStream(fin, "bad_prop.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the variety is invalid") {
		fin.open("files/items/bad_type.xml");
		doc = xmlDocFromStream(fin, "bad_type.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), Exception);
	}
	SECTION("When the weapon key skill is invalid") {
		fin.open("files/items/bad_weapon.xml");
		doc = xmlDocFromStream(fin, "bad_weapon.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), Exception);
	}
	SECTION("When the special ability is missing a required subtag") {
		fin.open("files/items/missing_abil.xml");
		doc = xmlDocFromStream(fin, "missing_abil.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When the special ability is invalid") {
		fin.open("files/items/bad_abil.xml");
		doc = xmlDocFromStream(fin, "bad_abil.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), Exception);
	}
	SECTION("When the special ability has an invalid subtag") {
		fin.open("files/items/bad_abil_tag.xml");
		doc = xmlDocFromStream(fin, "bad_abil_tag.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the special ability has an invalid use flag") {
		fin.open("files/items/bad_use.xml");
		doc = xmlDocFromStream(fin, "bad_use.xml");
		REQUIRE_THROWS_AS(readItemsFromXml(move(doc), scen), Exception);
	}
	SECTION("With the minimal required data") {
		fin.open("files/items/minimal.xml");
		doc = xmlDocFromStream(fin, "minimal.xml");
		REQUIRE_NOTHROW(readItemsFromXml(move(doc), scen));
		REQUIRE(scen.scen_items.size() >= 1);
		CHECK(scen.scen_items[0].full_name == "Test Sword");
		CHECK(scen.scen_items[0].name == "Sword");
		CHECK(scen.scen_items[0].variety == eItemType::ONE_HANDED);
		CHECK(scen.scen_items[0].item_level == 3);
		CHECK(scen.scen_items[0].graphic_num == 0);
		CHECK(scen.scen_items[0].value == 100);
		CHECK(scen.scen_items[0].weight == 10);
	}
}
