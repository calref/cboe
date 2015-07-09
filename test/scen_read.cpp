
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
		REQUIRE_THROWS_AS(readScenarioFromXml(std::move(doc), scen), xBadNode);
	}
	SECTION("When the version attribute is missing") {
		fin.open("files/scenario/no_version.xml");
		doc = xmlDocFromStream(fin, "no_version.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(std::move(doc), scen), xMissingAttr);
	}
	SECTION("When the root tag has a bad attribute") {
		fin.open("files/scenario/bad_root_attr.xml");
		doc = xmlDocFromStream(fin, "bad_root_attr.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(std::move(doc), scen), xBadAttr);
	}
	SECTION("When an essential toplevel element is missing") {
		fin.open("files/scenario/missing_toplevel.xml");
		doc = xmlDocFromStream(fin, "missing_toplevel.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(std::move(doc), scen), xMissingElem);
	}
	SECTION("When there are too many intro strings") {
		fin.open("files/scenario/intro_overflow.xml");
		doc = xmlDocFromStream(fin, "intro_overflow.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(std::move(doc), scen), xBadNode);
	}
}
