
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
}
