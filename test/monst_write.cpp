//
//  monst_write.cpp
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
extern void readMonstersFromXml(Document&& data, cScenario& scenario);
extern void writeMonstersToXml(Printer&& data, cScenario& scenario);

static void in_and_out(string name, cScenario& scen) {
	string fpath = "junk/monst_";
	fpath += name;
	fpath += ".xml";
	ofstream fout;
	fout.exceptions(ios::badbit);
	fout.open(fpath);
	writeMonstersToXml(Printer(name, fout), scen);
	fout.close();
	// Reconstruct the scenario, to ensure that it doesn't just pass due to old data still being around
	scen.~cScenario();
	new(&scen) cScenario();
	ifstream fin;
	fin.exceptions(ios::badbit);
	fin.open(fpath);
	readMonstersFromXml(xmlDocFromStream(fin, name), scen);
}

TEST_CASE("Saving monster types") {
	cScenario scen;
	scen.scen_monsters.resize(2);
	scen.scen_monsters[1].m_name = "Test Monster";
	scen.scen_monsters[1].level = 3;
	scen.scen_monsters[1].armor = 5;
	scen.scen_monsters[1].skill = 6;
	scen.scen_monsters[1].m_health = 10;
	scen.scen_monsters[1].m_type = eRace::MAGICAL;
	scen.scen_monsters[1].x_width = 1;
	scen.scen_monsters[1].y_width = 2;
	scen.scen_monsters[1].picture_num = 17;
	scen.scen_monsters[1].default_attitude = eAttitude::HOSTILE_B;
	SECTION("With the minimal required information") {
		in_and_out("basic", scen);
		CHECK(scen.scen_monsters[1].m_name == "Test Monster");
		CHECK(scen.scen_monsters[1].level == 3);
		CHECK(scen.scen_monsters[1].armor == 5);
		CHECK(scen.scen_monsters[1].skill == 6);
		CHECK(scen.scen_monsters[1].m_health == 10);
		CHECK(scen.scen_monsters[1].speed == 4);
		CHECK(scen.scen_monsters[1].m_type == eRace::MAGICAL);
		CHECK(scen.scen_monsters[1].x_width == 1);
		CHECK(scen.scen_monsters[1].y_width == 2);
		CHECK(scen.scen_monsters[1].picture_num == 17);
		CHECK(scen.scen_monsters[1].default_attitude == eAttitude::HOSTILE_B);
		for(int i = 0; i <= 8; i++) {
			eDamageType dmg = eDamageType(i);
			CAPTURE(dmg);
			CHECK(scen.scen_monsters[1].resist[dmg] == 100);
		}
	}
}
