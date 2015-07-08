
#include <fstream>
#include <iostream>
#include "ticpp.h"
#include "dialog.hpp"
#include "catch.hpp"
#include "scenario.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readScenarioFromXml(Document&& data, cScenario& scenario);

TEST_CASE("Loading a new-format scenario record") {
	ifstream fin;
	cScenario scen;
	Document doc;
	fin.exceptions(ios::badbit);
	
	SECTION("When the version attribute is missing") {
		fin.open("files/scenario/bad_root.xml");
		doc = xmlDocFromStream(fin, "bad_root.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the version attribute is missing") {
		fin.open("files/scenario/no_version.xml");
		doc = xmlDocFromStream(fin, "no_version.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("When the root tag has a bad attribute") {
		fin.open("files/scenario/bad_root_attr.xml");
		doc = xmlDocFromStream(fin, "bad_root_attr.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("When an essential toplevel element is missing") {
		fin.open("files/scenario/missing_toplevel.xml");
		doc = xmlDocFromStream(fin, "missing_toplevel.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When there are too many intro strings") {
		fin.open("files/scenario/intro_overflow.xml");
		doc = xmlDocFromStream(fin, "intro_overflow.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the scenario pic element is empty") {
		fin.open("files/scenario/missing_pic.xml");
		doc = xmlDocFromStream(fin, "missing_pic.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), Exception);
	}
	SECTION("When the scenario version is invalid") {
		fin.open("files/scenario/bad_version.xml");
		doc = xmlDocFromStream(fin, "bad_version.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), string);
	}
	SECTION("When the author email is missing") {
		fin.open("files/scenario/missing_email.xml");
		doc = xmlDocFromStream(fin, "missing_email.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), Exception);
	}
	SECTION("When a scenario rating is missing") {
		fin.open("files/scenario/missing_rating.xml");
		doc = xmlDocFromStream(fin, "missing_rating.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), Exception);
	}
	SECTION("When the scenario content rating is invalid") {
		fin.open("files/scenario/bad_rating.xml");
		doc = xmlDocFromStream(fin, "bad_rating.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), Exception);
	}
	SECTION("When there are too many teaser strings") {
		fin.open("files/scenario/extra_teaser.xml");
		doc = xmlDocFromStream(fin, "extra_teaser.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When some flags are missing") {
		fin.open("files/scenario/missing_flags.xml");
		doc = xmlDocFromStream(fin, "missing_flags.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When it's not OBoE format") {
		fin.open("files/scenario/bad_format.xml");
		doc = xmlDocFromStream(fin, "bad_format.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadVal);
	}
	SECTION("When an essential <game> child element is missing") {
		fin.open("files/scenario/missing_game.xml");
		doc = xmlDocFromStream(fin, "missing_game.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When an essential <editor> child element is missing") {
		fin.open("files/scenario/missing_editor.xml");
		doc = xmlDocFromStream(fin, "missing_editor.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("With an erroneous <graphics> section") {
		SECTION("Two of them") {
			fin.open("files/scenario/bad_graphics1.xml");
			doc = xmlDocFromStream(fin, "bad_graphics1.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Invalid child element") {
			fin.open("files/scenario/bad_graphics2.xml");
			doc = xmlDocFromStream(fin, "bad_graphics2.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Invalid attribute") {
			fin.open("files/scenario/bad_graphics4.xml");
			doc = xmlDocFromStream(fin, "bad_graphics4.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Missing index attribute") {
			fin.open("files/scenario/bad_graphics3.xml");
			doc = xmlDocFromStream(fin, "bad_graphics3.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Bad graphic type") {
			fin.open("files/scenario/bad_graphics5.xml");
			doc = xmlDocFromStream(fin, "bad_graphics5.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadVal);
		}
	}
	SECTION("With the minimal required data") {
		fin.open("files/scenario/minimal.xml");
		doc = xmlDocFromStream(fin, "minimal.xml");
		REQUIRE_NOTHROW(readScenarioFromXml(move(doc), scen));
		CHECK(scen.scen_name == "Test Scenario");
		CHECK(scen.intro_pic == 7);
		CHECK(scen.campaign_id == "campaign");
		CHECK(scen.format.prog_make_ver[0] == 2);
		CHECK(scen.format.prog_make_ver[1] == 5);
		CHECK(scen.format.prog_make_ver[2] == 3);
		CHECK(scen.format.ver[0] == 2);
		CHECK(scen.format.ver[1] == 6);
		CHECK(scen.format.ver[2] == 7);
		// TODO: Check language
		CHECK(scen.contact_info[0] == "BoE Test Suite");
		CHECK(scen.contact_info[1] == "nowhere@example.com");
		CHECK(scen.who_wrote[0] == "Teaser 1");
		CHECK(scen.who_wrote[1] == "Teaser 2");
		CHECK(scen.intro_mess_pic == 7);
		CHECK(scen.intro_strs[0] == "Welcome!!!    To the test scenario!");
		CHECK(scen.rating == eContentRating::R);
		CHECK(scen.difficulty == 2); // Difficulty is 3, but it's stored as 2 (0-3 instead of 1-4)
		CHECK(scen.adjust_diff == true);
		CHECK(scen.is_legacy == false);
		CHECK(scen.uses_custom_graphics == false);
		CHECK(scen.towns.size() == 3);
		CHECK(scen.outdoors.width() == 2);
		CHECK(scen.outdoors.height() == 1);
		CHECK(scen.which_town_start == 7);
		CHECK(scen.where_start == loc(24,28));
		CHECK(scen.out_sec_start == loc(1,3));
		CHECK(scen.out_start == loc(12,21));
		CHECK(scen.default_ground == 2);
		CHECK(scen.last_out_edited == loc(0,0));
		CHECK(scen.last_town_edited == 0);
	}
	SECTION("With a special item") {
		SECTION("Valid") {
			fin.open("files/scenario/special_item.xml");
			doc = xmlDocFromStream(fin, "special_item.xml");
			REQUIRE_NOTHROW(readScenarioFromXml(move(doc), scen));
			CHECK(scen.special_items.size() == 1);
			CHECK(scen.special_items[0].name == "My Special Item");
			CHECK(scen.special_items[0].descr == "Special Item  --  Description!");
			CHECK(scen.special_items[0].flags == 0);
			CHECK(scen.special_items[0].special == -1);
		}
		SECTION("Invalid attribute") {
			fin.open("files/scenario/special_item-bad_attr.xml");
			doc = xmlDocFromStream(fin, "special_item-bad_attr.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Invalid element") {
			fin.open("files/scenario/special_item-bad_elem.xml");
			doc = xmlDocFromStream(fin, "special_item-bad_elem.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Missing name") {
			fin.open("files/scenario/special_item-missing_elem.xml");
			doc = xmlDocFromStream(fin, "special_item-missing_elem.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
		}
	}
}
