//
//  out_write.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-09-02.
//
//

#include <fstream>
#include "catch.hpp"
#include "tinyprint.h"
#include "scenario.hpp"
#include "outdoors.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readOutdoorsFromXml(Document&& data, cOutdoors& out);
extern void writeOutdoorsToXml(ticpp::Printer&& data, cOutdoors& sector);

extern bool operator==(const cOutdoors::cWandering& lhs, const cOutdoors::cWandering& rhs);
extern ostream& operator<<(ostream& out, const cOutdoors::cWandering& enc);

static void in_and_out(string name, cOutdoors& out, cScenario& scen) {
	string fpath = "junk/out_";
	fpath += name;
	fpath += ".xml";
	ofstream fout;
	fout.exceptions(ios::badbit);
	fout.open(fpath);
	writeOutdoorsToXml(Printer(name, fout), out);
	fout.close();
	// Reconstruct the sector, to ensure that it doesn't just pass due to old data still being around
	out.~cOutdoors();
	new(&out) cOutdoors(scen);
	ifstream fin;
	fin.exceptions(ios::badbit);
	fin.open(fpath);
	readOutdoorsFromXml(xmlDocFromStream(fin, name), out);
}

TEST_CASE("Saving an outdoors sector") {
	cScenario scen;
	cOutdoors out(scen);
	out.name = "The Outdoors Test";
	SECTION("With the minimal required information") {
		in_and_out("basic", out, scen);
		CHECK(out.name == "The Outdoors Test");
	}
	SECTION("With some optional information") {
		out.comment = "Let's make a comment about comments.";
		out.ambient_sound = AMBIENT_DRIP;
		out.sign_locs.emplace_back(0,0,"Guantanamo  -  14 mi.");
		out.area_desc.emplace_back(0,0,1,1,"The heart of the wilderness");
		out.spec_strs.emplace_back("Something happened!");
		in_and_out("optional", out, scen);
		CHECK(out.comment == "Let's make a comment about comments.");
		CHECK(out.ambient_sound == AMBIENT_DRIP);
		REQUIRE(out.sign_locs.size() >= 1);
		CHECK(out.sign_locs[0].text == "Guantanamo  -  14 mi.");
		REQUIRE(out.area_desc.size() >= 1);
		CHECK(out.area_desc[0].descr == "The heart of the wilderness");
		REQUIRE(out.spec_strs.size() >= 1);
		CHECK(out.spec_strs[0] == "Something happened!");
	}
	SECTION("With some encounters") {
		cOutdoors::cWandering spec, wand;
		
		spec.monst[3] = 42;
		spec.monst[5] = 12;
		spec.spec_on_meet = 15;
		spec.spec_on_win = 12;
		spec.spec_on_flee = 9;
		spec.cant_flee = true;
		spec.forced = true;
		
		wand.monst[2] = 80;
		wand.monst[6] = 90;
		wand.friendly[1] = 12;
		wand.end_spec1 = 210;
		wand.end_spec2 = 22;
		
		REQUIRE(out.special_enc.size() >= 1);
		out.special_enc[0] = spec;
		REQUIRE(out.wandering.size() >= 1);
		out.wandering[0] = wand;
		in_and_out("encounters", out, scen);
		REQUIRE(out.special_enc.size() >= 1);
		CHECK(out.special_enc[0] == spec);
		REQUIRE(out.wandering.size() >= 1);
		CHECK(out.wandering[0] == wand);
	}
}
