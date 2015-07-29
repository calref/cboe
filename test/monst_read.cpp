//
//  monst_read.cpp
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
extern void readMonstersFromXml(Document&& data, cScenario& scenario);

TEST_CASE("Loading a monster type definition") {
	ifstream fin;
	cScenario scen;
	Document doc;
	fin.exceptions(ios::badbit);
	
	SECTION("When the root tag is wrong") {
		fin.open("files/bad_root.xml");
		doc = xmlDocFromStream(fin, "bad_root.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the version attribute is missing") {
		fin.open("files/monsters/no_version.xml");
		doc = xmlDocFromStream(fin, "no_version.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("When the root tag has a bad attribute") {
		fin.open("files/monsters/bad_root_attr.xml");
		doc = xmlDocFromStream(fin, "bad_root_attr.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("When an unknown toplevel tag appears") {
		fin.open("files/monsters/bad_toplevel.xml");
		doc = xmlDocFromStream(fin, "bad_toplevel.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the ID attribute is missing") {
		fin.open("files/monsters/missing_id.xml");
		doc = xmlDocFromStream(fin, "missing_id.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), Exception);
	}
	SECTION("When the ID attribute is zero") {
		fin.open("files/monsters/bad_id.xml");
		doc = xmlDocFromStream(fin, "missing_id.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
	}
	SECTION("When a required subtag is missing") {
		fin.open("files/monsters/missing_req.xml");
		doc = xmlDocFromStream(fin, "missing_req.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When a bad subtag is found") {
		fin.open("files/monsters/bad_tag.xml");
		doc = xmlDocFromStream(fin, "bad_tag.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When an attack has a bad subtag") {
		fin.open("files/monsters/bad_attack_tag.xml");
		doc = xmlDocFromStream(fin, "bad_attack_tag.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When an attack has a bad attribute") {
		fin.open("files/monsters/bad_attack_attr.xml");
		doc = xmlDocFromStream(fin, "bad_attack_attr.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("When an attack has a bad type") {
		fin.open("files/monsters/bad_attack_type.xml");
		doc = xmlDocFromStream(fin, "bad_attack_type.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), Exception);
	}
	SECTION("When an attack damage is invalid") {
		fin.open("files/monsters/bad_attack.xml");
		doc = xmlDocFromStream(fin, "bad_attack.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
	}
	SECTION("When an attack damage is missing 'd'") {
		fin.open("files/monsters/bad_attack2.xml");
		doc = xmlDocFromStream(fin, "bad_attack2.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
	}
	SECTION("When an attack is missing the type") {
		fin.open("files/monsters/missing_attack_type.xml");
		doc = xmlDocFromStream(fin, "missing_attack_type.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("When the picture is missing the size attributes") {
		fin.open("files/monsters/bad_pic.xml");
		doc = xmlDocFromStream(fin, "bad_pic.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("When the picture has a bad attribute") {
		fin.open("files/monsters/bad_pic2.xml");
		doc = xmlDocFromStream(fin, "bad_pic2.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("When there is a bad immunity") {
		fin.open("files/monsters/bad_immune.xml");
		doc = xmlDocFromStream(fin, "bad_immune.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the loot has a bad subtag") {
		fin.open("files/monsters/bad_loot.xml");
		doc = xmlDocFromStream(fin, "bad_loot.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the loot has a missing subtag") {
		fin.open("files/monsters/bad_loot2.xml");
		doc = xmlDocFromStream(fin, "bad_loot2.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("With the minimal required data") {
		fin.open("files/monsters/minimal.xml");
		doc = xmlDocFromStream(fin, "minimal.xml");
		REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
		REQUIRE(scen.scen_monsters.size() >= 2);
		CHECK(scen.scen_monsters[1].m_name == "Test Monster");
		CHECK(scen.scen_monsters[1].level == 1);
		CHECK(scen.scen_monsters[1].armor == 0);
		CHECK(scen.scen_monsters[1].skill == 2);
		CHECK(scen.scen_monsters[1].m_health == 10);
		CHECK(scen.scen_monsters[1].speed == 4);
		CHECK(scen.scen_monsters[1].m_type == eRace::HUMANOID);
		CHECK(scen.scen_monsters[1].picture_num == 5);
		CHECK(scen.scen_monsters[1].x_width == 1);
		CHECK(scen.scen_monsters[1].y_width == 1);
		CHECK(scen.scen_monsters[1].default_attitude == eAttitude::HOSTILE_A);
	}
	SECTION("With some attacks") {
		fin.open("files/monsters/attacks.xml");
		doc = xmlDocFromStream(fin, "attacks.xml");
		REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
		REQUIRE(scen.scen_monsters.size() >= 2);
		CHECK(scen.scen_monsters[1].a[0].type == eMonstMelee::SWING);
		CHECK(scen.scen_monsters[1].a[0].dice == 1);
		CHECK(scen.scen_monsters[1].a[0].sides == 10);
		CHECK(scen.scen_monsters[1].a[1].type == eMonstMelee::PUNCH);
		CHECK(scen.scen_monsters[1].a[1].dice == 2);
		CHECK(scen.scen_monsters[1].a[1].sides == 4);
		CHECK(scen.scen_monsters[1].a[2].type == eMonstMelee::BURN);
		CHECK(scen.scen_monsters[1].a[2].dice == 1);
		CHECK(scen.scen_monsters[1].a[2].sides == 8);
	}
}
