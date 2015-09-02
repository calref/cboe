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
	out.out_name = "The Outdoors Test";
	SECTION("With the minimal required information") {
		in_and_out("basic", out, scen);
		CHECK(out.out_name == "The Outdoors Test");
	}
}
