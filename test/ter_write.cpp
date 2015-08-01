//
//  ter_write.cpp
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
extern void readTerrainFromXml(Document&& data, cScenario& scenario);
extern void writeTerrainToXml(Printer&& data, cScenario& scenario);

static void in_and_out(string name, cScenario& scen) {
	string fpath = "junk/ter_";
	fpath += name;
	fpath += ".xml";
	ofstream fout;
	fout.exceptions(ios::badbit);
	fout.open(fpath);
	writeTerrainToXml(Printer(name, fout), scen);
	fout.close();
	// Reconstruct the scenario, to ensure that it doesn't just pass due to old data still being around
	scen.~cScenario();
	new(&scen) cScenario();
	ifstream fin;
	fin.exceptions(ios::badbit);
	fin.open(fpath);
	readTerrainFromXml(xmlDocFromStream(fin, name), scen);
}

TEST_CASE("Saving terrain types") {
	cScenario scen;
	scen.ter_types.resize(1);
	scen.ter_types[0].name = "Test Terrain";
	scen.ter_types[0].picture = 0;
	scen.ter_types[0].map_pic = 1;
	scen.ter_types[0].blockage = eTerObstruct::BLOCK_SIGHT;
	scen.ter_types[0].trim_type = eTrimType::CITY;
	scen.ter_types[0].combat_arena = 2;
	scen.ter_types[0].special = eTerSpec::WILDERNESS_SURFACE;
	SECTION("With basic information") {
		in_and_out("basic", scen);
		REQUIRE(scen.ter_types.size() == 1);
		CHECK(scen.ter_types[0].name == "Test Terrain");
		CHECK(scen.ter_types[0].picture == 0);
		CHECK(scen.ter_types[0].map_pic == 1);
		CHECK(scen.ter_types[0].blockage == eTerObstruct::BLOCK_SIGHT);
		CHECK(scen.ter_types[0].trim_type == eTrimType::CITY);
		CHECK(scen.ter_types[0].combat_arena == 2);
		CHECK(scen.ter_types[0].special == eTerSpec::WILDERNESS_SURFACE);
		CHECK(scen.ter_types[0].flag1 == -1);
		CHECK(scen.ter_types[0].step_sound == eStepSnd::STEP);
	}
}
