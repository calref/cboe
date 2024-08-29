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
#include "scenario/scenario.hpp"

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
	SECTION("With all information") {
		scen.scen_items[0].awkward = 1;
		scen.scen_items[0].bonus = 5;
		scen.scen_items[0].protection = 4;
		scen.scen_items[0].charges = 20;
		// So that both weap_type and missile are saved \/
		scen.scen_items[0].variety = eItemType::MISSILE_NO_AMMO;
		scen.scen_items[0].weap_type = eSkill::DEFENSE;
		scen.scen_items[0].missile = 3;
		scen.scen_items[0].type_flag = 9;
		scen.scen_items[0].special_class = 400;
		scen.scen_items[0].treas_class = 2;
		scen.scen_items[0].ability = eItemAbil::POISON_AUGMENT;
		scen.scen_items[0].abil_strength = 6;
		scen.scen_items[0].abil_data.value = 42;
		scen.scen_items[0].magic_use_type = eItemUse::HARM_ONE;
		scen.scen_items[0].ident = true;
		scen.scen_items[0].magic = true;
		scen.scen_items[0].cursed = true;
		scen.scen_items[0].concealed = true;
		scen.scen_items[0].enchanted = true;
		scen.scen_items[0].rechargeable = true;
		scen.scen_items[0].unsellable = true;
		scen.scen_items[0].desc = "  This is a silly,  silly description. ";
		in_and_out("full", scen);
		REQUIRE(scen.scen_items.size() == 1);
		CHECK(scen.scen_items[0].awkward == 1);
		CHECK(scen.scen_items[0].bonus == 5);
		CHECK(scen.scen_items[0].protection == 4);
		CHECK(scen.scen_items[0].charges == 20);
		CHECK(scen.scen_items[0].max_charges == 20);
		CHECK(scen.scen_items[0].weap_type == eSkill::DEFENSE);
		CHECK(scen.scen_items[0].missile == 3);
		CHECK(scen.scen_items[0].type_flag == 9);
		CHECK(scen.scen_items[0].special_class == 400);
		CHECK(scen.scen_items[0].treas_class == 2);
		CHECK(scen.scen_items[0].ability == eItemAbil::POISON_AUGMENT);
		CHECK(scen.scen_items[0].abil_strength == 6);
		CHECK(scen.scen_items[0].abil_data.value == 42);
		CHECK(scen.scen_items[0].magic_use_type == eItemUse::HARM_ONE);
		CHECK(scen.scen_items[0].ident);
		CHECK(scen.scen_items[0].magic);
		CHECK(scen.scen_items[0].cursed);
		CHECK(scen.scen_items[0].concealed);
		CHECK(scen.scen_items[0].enchanted);
		CHECK(scen.scen_items[0].rechargeable);
		CHECK(scen.scen_items[0].unsellable);
		CHECK(scen.scen_items[0].desc == "  This is a silly,  silly description. ");
	}
}
