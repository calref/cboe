//
//  town_write.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-09-02.
//
//

#include <fstream>
#include "catch.hpp"
#include "tinyprint.h"
#include "scenario/scenario.hpp"
#include "scenario/town.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readTownFromXml(Document&& data, cTown*& town, cScenario& scen);
extern void writeTownToXml(ticpp::Printer&& data, cTown& town);

static void in_and_out(string name, cTown*& town, cScenario& scen) {
	string fpath = "junk/town_";
	fpath += name;
	fpath += ".xml";
	ofstream fout;
	fout.exceptions(ios::badbit);
	fout.open(fpath);
	writeTownToXml(Printer(name, fout), *town);
	fout.close();
	// Reconstruct the town, to ensure that it doesn't just pass due to old data still being around
	delete town;
	ifstream fin;
	fin.exceptions(ios::badbit);
	fin.open(fpath);
	readTownFromXml(xmlDocFromStream(fin, name), town, scen);
}

TEST_CASE("Saving a town") {
	cScenario scen;
	cTown* town = new cTown(scen, AREA_SMALL);
	town->name = "Test Town";
	town->in_town_rect = {2,3,30,29};
	town->difficulty = 1;
	town->lighting_type = LIGHT_NONE;
	town->max_num_monst = 20;
	SECTION("With the minimal required information") {
		in_and_out("basic", town, scen);
		CHECK(town->max_dim == 32);
		CHECK(town->name == "Test Town");
		CHECK(town->in_town_rect == rect(2,3,30,29));
		CHECK(town->difficulty == 1);
		CHECK(town->lighting_type == LIGHT_NONE);
		CHECK(town->max_num_monst == 20);
		CHECK_FALSE(town->has_tavern);
		CHECK_FALSE(town->defy_scrying);
	}
	SECTION("With lots of optional information") {
		town->comment[2] = "Try a comment!";
		town->spec_on_entry = 42;
		town->spec_on_entry_if_dead = 19;
		town->spec_on_hostile = 47;
		town->exits[0] = {24,2};
		town->exits[0].spec = 16;
		town->town_chop_time = 25;
		town->town_chop_key = 6;
		town->max_num_monst = 100000;
		town->is_hidden = town->has_tavern = true;
		town->defy_scrying = town->defy_mapping = true;
		town->strong_barriers = true;
		REQUIRE(town->timers.size() >= 1);
		town->timers[0].node = 12;
		town->timers[0].time = 2500;
		REQUIRE(town->wandering.size() >= 1);
		town->wandering[0].monst = {7,8,9,10};
		town->sign_locs.emplace_back(0,0,"Sign #4279816");
		town->spec_strs.emplace_back("Something!    With extra spaces!");
		town->area_desc.emplace_back(0,0,1,1,"Unknown Area . . .");
		in_and_out("optional", town, scen);
		CHECK(town->comment[2] == "Try a comment!");
		CHECK(town->spec_on_entry == 42);
		CHECK(town->spec_on_entry_if_dead == 19);
		CHECK(town->spec_on_hostile == 47);
		CHECK(town->exits[0].spec == 16);
		CHECK(town->exits[0] == loc(24,2));
		CHECK(town->town_chop_time == 25);
		CHECK(town->town_chop_key == 6);
		CHECK(town->max_num_monst == 100000);
		CHECK(town->is_hidden);
		CHECK(town->strong_barriers);
		CHECK(town->has_tavern);
		CHECK(town->defy_mapping);
		CHECK(town->defy_scrying);
		REQUIRE(town->timers.size() >= 1);
		CHECK(town->timers[0].node_type == eSpecCtxType::TOWN);
		CHECK(town->timers[0].node == 12);
		CHECK(town->timers[0].time == 2500);
		REQUIRE(town->wandering.size() >= 1);
		CHECK(town->wandering[0].monst[0] == 7);
		CHECK(town->wandering[0].monst[1] == 8);
		CHECK(town->wandering[0].monst[2] == 9);
		CHECK(town->wandering[0].monst[3] == 10);
		REQUIRE(town->sign_locs.size() >= 1);
		CHECK(town->sign_locs[0].text == "Sign #4279816");
		REQUIRE(town->spec_strs.size() >= 1);
		CHECK(town->spec_strs[0] == "Something!    With extra spaces!");
		REQUIRE(town->area_desc.size() >= 1);
		CHECK(town->area_desc[0].descr == "Unknown Area . . .");
	}
	SECTION("With a preset item") {
		town->preset_items.emplace_back();
		town->preset_items.back().code = 52;
		town->preset_items.back().ability = eEnchant::BLESSED;
		town->preset_items.back().charges = 102;
		town->preset_items.back().always_there = true;
		town->preset_items.back().property = true;
		town->preset_items.back().contained = true;
		in_and_out("item", town, scen);
		REQUIRE(town->preset_items.size() >= 1);
		CHECK(town->preset_items[0].code == 52);
		CHECK(town->preset_items[0].ability == eEnchant::BLESSED);
		CHECK(town->preset_items[0].charges == 102);
		CHECK(town->preset_items[0].always_there);
		CHECK(town->preset_items[0].property);
		CHECK(town->preset_items[0].contained);
	}
	SECTION("With a townsperson") {
		town->creatures.emplace_back();
		town->creatures.back().number = 60;
		town->creatures.back().start_attitude = eAttitude::DOCILE;
		town->creatures.back().mobility = 1;
		town->creatures.back().spec1 = 17;
		town->creatures.back().spec2 = 4;
		town->creatures.back().spec_enc_code = 3;
		town->creatures.back().time_flag = eMonstTime::DISAPPEAR_WHEN_EVENT;
		town->creatures.back().time_code = 5;
		town->creatures.back().monster_time = 16;
		town->creatures.back().facial_pic = 56;
		town->creatures.back().personality = 8;
		town->creatures.back().special_on_kill = 9;
		town->creatures.back().special_on_talk = 12;
		in_and_out("townsperson", town, scen);
		REQUIRE(town->creatures.size() >= 1);
		CHECK(town->creatures[0].number == 60);
		CHECK(town->creatures[0].start_attitude == eAttitude::DOCILE);
		CHECK(town->creatures[0].mobility == 1);
		CHECK(town->creatures[0].spec1 == 17);
		CHECK(town->creatures[0].spec2 == 4);
		CHECK(town->creatures[0].spec_enc_code == 3);
		CHECK(town->creatures[0].time_flag == eMonstTime::DISAPPEAR_WHEN_EVENT);
		CHECK(town->creatures[0].monster_time == 16);
		CHECK(town->creatures[0].time_code == 5);
		CHECK(town->creatures[0].facial_pic == 56);
		CHECK(town->creatures[0].personality == 8);
		CHECK(town->creatures[0].special_on_kill == 9);
		CHECK(town->creatures[0].special_on_talk == 12);
	}
}
