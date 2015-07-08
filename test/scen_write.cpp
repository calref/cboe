
#include <fstream>
#include <iostream>
#include "tinyprint.h"
#include "dialog.hpp"
#include "catch.hpp"
#include "scenario.hpp"
#include "regtown.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readScenarioFromXml(Document&& data, cScenario& scenario);
extern void writeScenarioToXml(Printer&& data, cScenario& scenario);

static void in_and_out(std::string name, cScenario& scen) {
	std::string fpath = "junk/";
	fpath += name;
	fpath += ".xml";
	ofstream fout;
	fout.exceptions(ios::badbit);
	fout.open(fpath);
	writeScenarioToXml(Printer(name, fout), scen);
	fout.close();
	// Reconstruct the scenario, to ensure that it doesn't just pass due to old data still being around
	scen.~cScenario();
	new(&scen) cScenario();
	ifstream fin;
	fin.exceptions(ios::badbit);
	fin.open(fpath);
	readScenarioFromXml(xmlDocFromStream(fin, name), scen);
}

// NOTE: The test cases in this file are written with the implicit assumption that the read routines are trustworthy.
// In other words, they depend on the test cases in scen_read.cpp.

TEST_CASE("Saving a scenario record") {
	cScenario scen;
	scen.reset_version();
	scen.format.ver[0] = 2;
	scen.format.ver[1] = 6;
	scen.format.ver[2] = 7;
	scenario_header_flags vers = scen.format;
	scen.scen_name = "Test Scenario";
	scen.intro_pic = 0;
	scen.campaign_id = "campaign";
	scen.who_wrote[0] = "Teaser 1";
	scen.who_wrote[1] = "Teaser 2";
	scen.contact_info[0] = "BoE Test Suite";
	scen.contact_info[1] = "nowhere@example.com";
	scen.intro_strs[0] = "Welcome to the test scenario!";
	scen.rating = eContentRating::R;
	scen.difficulty = 2;
	scen.which_town_start = 7;
	scen.where_start = {24,28};
	scen.out_sec_start = {1,3};
	scen.out_start = {12, 21};
	SECTION("With basic header data") {
		in_and_out("basic", scen);
		CHECK(scen.format.prog_make_ver[0] == vers.prog_make_ver[0]);
		CHECK(scen.format.prog_make_ver[1] == vers.prog_make_ver[1]);
		CHECK(scen.format.prog_make_ver[2] == vers.prog_make_ver[2]);
		CHECK(scen.format.ver[0] == vers.ver[0]);
		CHECK(scen.format.ver[1] == vers.ver[1]);
		CHECK(scen.format.ver[2] == vers.ver[2]);
		CHECK(scen.scen_name == "Test Scenario");
		CHECK(scen.intro_pic == 0);
		CHECK(scen.campaign_id == "campaign");
		CHECK(scen.who_wrote[0] == "Teaser 1");
		CHECK(scen.who_wrote[1] == "Teaser 2");
		CHECK(scen.contact_info[0] == "BoE Test Suite");
		CHECK(scen.contact_info[1] == "nowhere@example.com");
		CHECK(scen.intro_strs[0] == "Welcome to the test scenario!");
		CHECK(scen.rating == eContentRating::R);
		CHECK(scen.difficulty == 2);
		CHECK(scen.which_town_start == 7);
		CHECK(scen.where_start == loc(24,28));
		CHECK(scen.out_sec_start == loc(1,3));
		CHECK(scen.out_start == loc(12,21));
	}
	SECTION("With some towns and sectors") {
		scen.addTown<cTinyTown>();
		scen.addTown<cMedTown>();
		scen.addTown<cBigTown>();
		scen.outdoors.resize(2,5);
		in_and_out("town&sector", scen);
		CHECK(scen.towns.size() == 3);
		CHECK(scen.outdoors.width() == 2);
		CHECK(scen.outdoors.height() == 5);
	}
	SECTION("With some optional header data") {
		REQUIRE(scen.town_mods.size() >= 1); // A safety valve for if I ever make this array dynamic
		scen.town_mods[0] = loc(12,9);
		scen.town_mods[0].spec = 4;
		REQUIRE(scen.store_item_towns.size() >= 1); // A safety valve for if I ever make this array dynamic
		REQUIRE(scen.store_item_rects.size() >= 1); // A safety valve for if I ever make this array dynamic
		scen.store_item_rects[0] = rect(1,2,3,4);
		scen.store_item_towns[0] = 5;
		REQUIRE(scen.scenario_timers.size() >= 1); // A safety valve for if I ever make this array dynamic
		scen.scenario_timers[0].node = 3;
		scen.scenario_timers[0].node_type = 1;
		scen.scenario_timers[0].time = 30000;
		scen.spec_strs.push_back("This is a sample special string!");
		scen.journal_strs.push_back("This is a sample journal string!");
		in_and_out("optional", scen);
		CHECK(scen.town_mods[0] == loc(12,9));
		CHECK(scen.town_mods[0].spec == 4);
		CHECK(scen.store_item_rects[0] == rect(1,2,3,4));
		CHECK(scen.store_item_towns[0] == 5);
		CHECK(scen.scenario_timers[0].node == 3);
		CHECK(scen.scenario_timers[0].node_type == 0); // This is inferred by the fact that it's in the scenario file
		CHECK(scen.scenario_timers[0].time == 30000);
		REQUIRE(scen.spec_strs.size() == 1);
		CHECK(scen.spec_strs[0] == "This is a sample special string!");
		REQUIRE(scen.journal_strs.size() == 1);
		CHECK(scen.journal_strs[0] == "This is a sample journal string!");
	}
	SECTION("With a special item") {
		scen.special_items.emplace_back();
		scen.special_items[0].flags = 11;
		scen.special_items[0].special = 2;
		scen.special_items[0].name = "Test Special Item";
		scen.special_items[0].descr = "This is a special item description!";
		in_and_out("special item", scen);
		REQUIRE(scen.special_items.size() == 1);
		CHECK(scen.special_items[0].flags == 11);
		CHECK(scen.special_items[0].special == 2);
		CHECK(scen.special_items[0].name == "Test Special Item");
		CHECK(scen.special_items[0].descr == "This is a special item description!");
	}
	SECTION("With a quest") {
		scen.quests.resize(3);
		scen.quests[0].flags = 11;
		scen.quests[0].bank1 = 2;
		scen.quests[0].deadline = 12;
		scen.quests[0].event = 3;
		scen.quests[0].xp = 5200;
		scen.quests[0].name = "Test Quest";
		scen.quests[0].descr = "This is a quest description! It has an absolute deadline which is waived by an event, and an XP reward. It's also in a job bank.";
		scen.quests[1].flags = 10;
		scen.quests[1].gold = 220;
		scen.quests[1].name = "Test Quest 2";
		scen.quests[1].descr = "This is another quest description! It has no deadline, and a monetary reward.";
		scen.quests[2].flags = 1;
		scen.quests[2].deadline = 12;
		scen.quests[2].bank2 = 4;
		scen.quests[2].name = "Test Quest 3";
		scen.quests[2].descr = "And now another quest description! This one has a relative deadline and no reward, but it's in a job bank.";
		in_and_out("quest", scen);
		REQUIRE(scen.quests.size() == 3);
		CHECK(scen.quests[0].flags == 11);
		CHECK(scen.quests[0].bank1 == 2);
		CHECK(scen.quests[0].deadline == 12);
		CHECK(scen.quests[0].event == 3);
		CHECK(scen.quests[0].xp == 5200);
		CHECK(scen.quests[0].name == "Test Quest");
		CHECK(scen.quests[1].flags == 10);
		CHECK(scen.quests[1].gold == 220);
		CHECK(scen.quests[1].name == "Test Quest 2");
		CHECK(scen.quests[2].flags == 1);
		CHECK(scen.quests[2].deadline == 12);
		CHECK(scen.quests[2].bank1 == 4); // bank2 moves into bank1
		CHECK(scen.quests[2].bank2 == -1);
		CHECK(scen.quests[2].name == "Test Quest 3");
	}
	SECTION("With some empty strings, only trailing ones are stripped") {
		scen.spec_strs.resize(12);
		scen.spec_strs[3] = "Hello World!";
		scen.spec_strs[9] = "Goodbye World!";
		scen.journal_strs.resize(19);
		scen.journal_strs[7] = "My best journal!";
		scen.intro_strs[4] = "Another intro string!";
		in_and_out("empty strings", scen);
		REQUIRE(scen.spec_strs.size() == 10);
		CHECK(scen.spec_strs[3] == "Hello World!");
		CHECK(scen.spec_strs[9] == "Goodbye World!");
		REQUIRE(scen.journal_strs.size() == 8);
		CHECK(scen.journal_strs[7] == "My best journal!");
		CHECK(scen.intro_strs[4] == "Another intro string!");
	}
	SECTION("Whitespace is collapsed in short strings but not in long strings") {
		scen.scen_name = "Test    Scenario   with extra spaces";
		scen.who_wrote[0] = "Teaser   the      first!";
		scen.who_wrote[1] = "   Teaser the        second  !    ";
		scen.spec_strs.push_back("      ");
		scen.spec_strs.push_back("   What  is this...   ?");
		scen.journal_strs.push_back("  Do not   collapse  this      journal.");
		scen.intro_strs[1] = "An intro string!      With extra spaces!";
		scen.special_items.emplace_back();
		scen.special_items[0].name = "A special    space-filled      item!";
		scen.special_items[0].descr = "A special item...      with extra spaces!";
		scen.quests.emplace_back();
		scen.quests[0].name = "A quest    filled     with      spaces...      ";
		scen.quests[0].descr = "A quest...      with extra spaces!";
		scen.snd_names.push_back("A    sound    full of    spaces!");
		in_and_out("whitespace", scen);
		CHECK(scen.scen_name == "Test Scenario with extra spaces");
		CHECK(scen.who_wrote[0] == "Teaser the first!");
		CHECK(scen.who_wrote[1] == "Teaser the second !");
		REQUIRE(scen.spec_strs.size() == 2);
		CHECK(scen.spec_strs[0] == "      ");
		CHECK(scen.spec_strs[1] == "   What  is this...   ?");
		REQUIRE(scen.journal_strs.size() == 1);
		CHECK(scen.journal_strs[0] == "  Do not   collapse  this      journal.");
		CHECK(scen.intro_strs[1] == "An intro string!      With extra spaces!");
		REQUIRE(scen.special_items.size() == 1);
		CHECK(scen.special_items[0].name == "A special space-filled item!");
		CHECK(scen.special_items[0].descr == "A special item...      with extra spaces!");
		REQUIRE(scen.quests.size() == 1);
		CHECK(scen.quests[0].name == "A quest filled with spaces...");
		CHECK(scen.quests[0].descr == "A quest...      with extra spaces!");
		CHECK(scen.snd_names[0] == "A sound full of spaces!");
	}
}
