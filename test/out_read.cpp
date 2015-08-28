//
//  out_read.cpp
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
#include "outdoors.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readOutdoorsFromXml(Document&& data, cOutdoors& out);

TEST_CASE("Loading an outdoor section definition") {
	ifstream fin;
	cScenario scen;
	cOutdoors sector(scen);
	Document doc;
	fin.exceptions(ios::badbit);
	
	SECTION("When the root tag is wrong") {
		fin.open("files/bad_root.xml");
		doc = xmlDocFromStream(fin, "bad_root.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xBadNode);
	}
	SECTION("When the version attribute is missing") {
		fin.open("files/outdoor/no_version.xml");
		doc = xmlDocFromStream(fin, "no_version.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xMissingAttr);
	}
	SECTION("When the root tag has a bad attribute") {
		fin.open("files/outdoor/bad_root_attr.xml");
		doc = xmlDocFromStream(fin, "bad_root_attr.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xBadAttr);
	}
	SECTION("When an essential toplevel element is missing") {
		fin.open("files/outdoor/missing_toplevel.xml");
		doc = xmlDocFromStream(fin, "missing_toplevel.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xMissingElem);
	}
	SECTION("When an invalid toplevel element is present") {
		fin.open("files/outdoor/bad_toplevel.xml");
		doc = xmlDocFromStream(fin, "bad_toplevel.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xBadNode);
	}
	SECTION("When an encounter has an invalid subtag") {
		fin.open("files/outdoor/encounter_bad_node.xml");
		doc = xmlDocFromStream(fin, "encounter_bad_node.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xBadNode);
	}
	SECTION("When an encounter has no monsters") {
		fin.open("files/outdoor/encounter_empty.xml");
		doc = xmlDocFromStream(fin, "encounter_empty.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xMissingElem);
	}
	SECTION("When an encounter has too many monsters") {
		fin.open("files/outdoor/encounter_too_many_monst.xml");
		doc = xmlDocFromStream(fin, "encounter_too_many_monst.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xBadNode);
	}
	SECTION("When an encounter monster has an invalid attribute") {
		fin.open("files/outdoor/encounter_bad_attr.xml");
		doc = xmlDocFromStream(fin, "encounter_bad_attr.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xBadAttr);
	}
	SECTION("When there are too many encounters") {
		fin.open("files/outdoor/encounter_too_many.xml");
		doc = xmlDocFromStream(fin, "encounter_too_many.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xBadNode);
	}
	SECTION("With the minimal required data") {
		fin.open("files/outdoor/minimal.xml");
		doc = xmlDocFromStream(fin, "minimal.xml");
		REQUIRE_NOTHROW(readOutdoorsFromXml(move(doc), sector));
		CHECK(sector.out_name == "Test Sector");
	}
}
