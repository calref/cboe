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

bool operator==(const cOutdoors::cWandering& lhs, const cOutdoors::cWandering& rhs);
ostream& operator<<(ostream& out, const cOutdoors::cWandering& enc);

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
	SECTION("When an encounter has an invalid attribute") {
		fin.open("files/outdoor/encounter_bad_attr.xml");
		doc = xmlDocFromStream(fin, "encounter_bad_attr.xml");
		REQUIRE_THROWS_AS(readOutdoorsFromXml(move(doc), sector), xBadAttr);
	}
	SECTION("When an encounter monster has an invalid attribute") {
		fin.open("files/outdoor/encounter_bad_monst_attr.xml");
		doc = xmlDocFromStream(fin, "encounter_bad_monst_attr.xml");
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
		CHECK(sector.name == "Test Sector");
	}
	SECTION("With an arbitrary ambient sound") {
		fin.open("files/outdoor/sound.xml");
		doc = xmlDocFromStream(fin, "sound.xml");
		REQUIRE_NOTHROW(readOutdoorsFromXml(move(doc), sector));
		CHECK(sector.ambient_sound == AMBIENT_CUSTOM);
		CHECK(sector.out_sound == 42);
	}
	SECTION("With all possible data") {
		cOutdoors::cWandering refEncounter;
		refEncounter.monst[0] = 12;
		refEncounter.friendly[0] = 15;
		refEncounter.spec_on_meet = 90;
		refEncounter.spec_on_win = 92;
		refEncounter.spec_on_flee = 84;
		refEncounter.end_spec1 = 202;
		refEncounter.end_spec2 = 19;
		refEncounter.cant_flee = true;
		refEncounter.forced = true;
		fin.open("files/outdoor/full.xml");
		doc = xmlDocFromStream(fin, "full.xml");
		REQUIRE_NOTHROW(readOutdoorsFromXml(move(doc), sector));
		CHECK(sector.comment == "Hello World!");
		CHECK(sector.ambient_sound == AMBIENT_BIRD);
		CHECK(sector.special_enc[0] == refEncounter);
		CHECK(sector.wandering[0] == refEncounter);
		REQUIRE(sector.sign_locs.size() >= 8);
		CHECK(sector.sign_locs[7].text == "The best sign ever!");
		REQUIRE(sector.area_desc.size() >= 1);
		CHECK(sector.area_desc[0].descr == "Some random area       Amazing!");
		REQUIRE(sector.spec_strs.size() >= 10);
		CHECK(sector.spec_strs[9] == "A random special string");
	}
}

bool operator==(const cOutdoors::cWandering& lhs, const cOutdoors::cWandering& rhs) {
	if(lhs.monst != rhs.monst) return false;
	if(lhs.friendly != rhs.friendly) return false;
	if(lhs.spec_on_meet != rhs.spec_on_meet) return false;
	if(lhs.spec_on_win != rhs.spec_on_win) return false;
	if(lhs.spec_on_flee != rhs.spec_on_flee) return false;
	if(lhs.cant_flee != rhs.cant_flee) return false;
	if(lhs.forced != rhs.forced) return false;
	if(lhs.end_spec1 != rhs.end_spec1) return false;
	if(lhs.end_spec2 != rhs.end_spec2) return false;
	return true;
}

ostream& operator<<(ostream& out, const cOutdoors::cWandering& enc) {
	out << "Encounter {";
	out << "hostile = ";
	for(auto i : enc.monst)
		out << i << ' ';
	out << ", friendly = ";
	for(auto i : enc.friendly)
		out << i << ' ';
	out << ", on-meet = " << enc.spec_on_meet << " , ";
	out << "on-win = " << enc.spec_on_win << " , ";
	out << "on-flee = " << enc.spec_on_flee << " , ";
	out << "can-flee = " << !enc.cant_flee << " , ";
	out << "forced = " << enc.forced << " , ";
	out << "sdf = (" << enc.end_spec1 << ',' << enc.end_spec2 << ")}";
	return out;
}
