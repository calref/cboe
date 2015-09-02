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
#include "scenario.hpp"
#include "regtown.hpp"

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
	cTown* town = new cTinyTown(scen);
	town->town_name = "Test Town";
	town->in_town_rect = {2,3,30,29};
	town->difficulty = 1;
	town->lighting_type = LIGHT_NONE;
	SECTION("With the minimal required information") {
		in_and_out("basic", town, scen);
		CHECK(town->max_dim() == 32);
		CHECK(town->town_name == "Test Town");
		CHECK(town->in_town_rect == rect(2,3,30,29));
		CHECK(town->difficulty == 1);
		CHECK(town->lighting_type == LIGHT_NONE);
	}
}
