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
	SECTION("When the size is invalid") {
		fin.open("files/town/bad_size.xml");
		doc = xmlDocFromStream(fin, "bad_size.xml");
		REQUIRE_THROWS_AS(readTownFromXml(move(doc), town, scen), xBadVal);
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
	SECTION("When the onenter condition is duplicated") {
		fin.open("files/town/dup_onenter_condition.xml");
		doc = xmlDocFromStream(fin, "dup_onenter_condition.xml");
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
		CHECK(town->max_dim == 32);
		CHECK(town->name == "Test Town");
		CHECK(town->in_town_rect == rect(4, 4, 28, 28));
		CHECK(town->difficulty == 1);
		CHECK(town->lighting_type == LIGHT_NORMAL);
	}
	SECTION("With all possible data") {
		fin.open("files/town/full.xml");
		doc = xmlDocFromStream(fin, "full.xml");
		REQUIRE_NOTHROW(readTownFromXml(move(doc), town, scen));
		CHECK(town->comment[0] == "This is a silly little comment.");
		CHECK(town->spec_on_entry == 12);
		CHECK(town->spec_on_entry_if_dead == 13);
		CHECK(town->exits[0] == loc(4,16));
		CHECK(town->exits[0].spec == 52);
		CHECK(town->spec_on_hostile == 42);
		CHECK(town->town_chop_time == 18);
		CHECK(town->town_chop_key == 4);
		CHECK(town->max_num_monst == 50000);
		CHECK(town->is_hidden);
		CHECK(town->strong_barriers);
		CHECK(town->has_tavern);
		CHECK(town->defy_mapping);
		CHECK(town->defy_scrying);
		REQUIRE(town->timers.size() >= 1);
		CHECK(town->timers[0].node_type == 2);
		CHECK(town->timers[0].node == 15);
		CHECK(town->timers[0].time == 100);
		REQUIRE(town->wandering.size() >= 1);
		CHECK(town->wandering[0].monst[0] == 40);
		CHECK(town->wandering[0].monst[1] == 41);
		CHECK(town->wandering[0].monst[2] == 42);
		CHECK(town->wandering[0].monst[3] == 43);
		REQUIRE(town->sign_locs.size() >= 2);
		CHECK(town->sign_locs[1].text == "This is a sample sign.");
		REQUIRE(town->spec_strs.size() >= 8);
		CHECK(town->spec_strs[7] == "Here is a town string.");
		REQUIRE(town->preset_items.size() >= 3);
		CHECK(town->preset_items[2].code == 120);
		CHECK(town->preset_items[2].ability == 2);
		CHECK(town->preset_items[2].charges == 17);
		CHECK(town->preset_items[2].always_there);
		CHECK(town->preset_items[2].property);
		CHECK(town->preset_items[2].contained);
		REQUIRE(town->creatures.size() >= 13);
		CHECK(town->creatures[12].number == 140);
		CHECK(town->creatures[12].start_attitude == eAttitude::HOSTILE_B);
		CHECK(town->creatures[12].mobility == 1);
		CHECK(town->creatures[12].spec1 == 12);
		CHECK(town->creatures[12].spec2 == 13);
		CHECK(town->creatures[12].spec_enc_code == 50);
		CHECK(town->creatures[12].time_flag == eMonstTime::APPEAR_WHEN_EVENT);
		CHECK(town->creatures[12].monster_time == 17);
		CHECK(town->creatures[12].time_code == 14);
		CHECK(town->creatures[12].facial_pic == 142);
		CHECK(town->creatures[12].personality == 1);
		CHECK(town->creatures[12].special_on_kill == 80);
		CHECK(town->creatures[12].special_on_talk == 81);
		REQUIRE(town->area_desc.size() >= 1);
		CHECK(town->area_desc[0].descr == "This  is  a  sample  area  description.");
	}
	
	delete town;
}
