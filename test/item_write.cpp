//
//  item_write.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-07-29.
//
//

#include <fstream>
#include "catch.hpp"
#include "tinyprint.h"
#include "scenario.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readItemsFromXml(Document&& data, cScenario& scenario);
extern void writeItemsToXml(Printer&& data, cScenario& scenario);

static void in_and_out(string name, cScenario& scen) {
	string fpath = "junk/item_";
	fpath += name;
	fpath += ".xml";
	ofstream fout;
	fout.exceptions(ios::badbit);
	fout.open(fpath);
	writeItemsToXml(Printer(name, fout), scen);
	fout.close();
	// Reconstruct the scenario, to ensure that it doesn't just pass due to old data still being around
	scen.~cScenario();
	new(&scen) cScenario();
	ifstream fin;
	fin.exceptions(ios::badbit);
	fin.open(fpath);
	readItemsFromXml(xmlDocFromStream(fin, name), scen);
}

TEST_CASE("Saving item types") {
	cScenario scen;
	scen.scen_items.resize(1);
	scen.scen_items[0].variety = eItemType::POTION;
	scen.scen_items[0].item_level = 0;
	scen.scen_items[0].graphic_num = 60;
	scen.scen_items[0].value = 100;
	scen.scen_items[0].weight = 5;
	scen.scen_items[0].name = "Potion";
	scen.scen_items[0].full_name = "Test Potion";
	SECTION("With basic information") {
		in_and_out("basic", scen);
		REQUIRE(scen.scen_items.size() == 1);
		CHECK(scen.scen_items[0].variety == eItemType::POTION);
		CHECK(scen.scen_items[0].item_level == 0);
		CHECK(scen.scen_items[0].graphic_num == 60);
		CHECK(scen.scen_items[0].value == 100);
		CHECK(scen.scen_items[0].weight == 5);
		CHECK(scen.scen_items[0].name == "Potion");
		CHECK(scen.scen_items[0].full_name == "Test Potion");
	}
}
