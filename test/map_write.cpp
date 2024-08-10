//
//  map_write.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-07-09.
//
//

#include <fstream>
#include <set>
#include "catch.hpp"
#include "fileio/map_parse.hpp"
#include "scenario/scenario.hpp"
#include "scenario/town.hpp"

using namespace std;

extern void loadOutMapData(map_data&& data, location which, cScenario& scen);
extern void loadTownMapData(map_data&& data, int which, cScenario& scen);
extern map_data buildOutMapData(location which, cScenario& scenario);
extern map_data buildTownMapData(size_t which, cScenario& scenario);

static void in_and_out(string name, map_data& map, bool town) {
	string fpath = "junk/";
	fpath += name;
	fpath += ".map";
	ofstream fout;
	fout.exceptions(ios::badbit);
	fout.open(fpath);
	map.writeTo(fout);
	fout.close();
	// Reconstruct the map, to ensure that it doesn't just pass due to old data still being around
	map.~map_data();
	new(&map) map_data();
	ifstream fin;
	fin.exceptions(ios::badbit);
	fin.open(fpath);
	map = load_map(fin, town, name);
}

static void in_and_out(cScenario& scen, bool town) {
	map_data map = town ? buildTownMapData(0, scen) : buildOutMapData(loc(0,0), scen);
	// Reconstruct the scenario, to ensure that it doesn't just pass due to old data still being around
	scen.~cScenario();
	new(&scen) cScenario();
	scen.ter_types.resize(50);
	if(town) {
		// Reconstruct the town, to ensure that it doesn't just pass due to old data still being around
		scen.addTown(AREA_TINY);
		loadTownMapData(move(map), 0, scen);
	} else {
		// Reconstruct the sector, to ensure that it doesn't just pass due to old data still being around
		scen.outdoors.resize(1,1);
		scen.outdoors[0][0] = new cOutdoors(scen);
		loadOutMapData(move(map), loc(0,0), scen);
	}
}

TEST_CASE("Saving map data to file") {
	map_data map;
	vector<pair<eMapFeature,int>> test;
	
	for(int x = 0; x < 5; x++)
		for(int y = 0; y < 5; y++)
			map.set(x, y, 1 + y * 5 + x);
	
	SECTION("Basic") {
		in_and_out("basic", map, true);
		for(int y = 0; y < 5; y++) {
			for(int x = 0; x < 5; x++) {
				int ter = 1 + y * 5 + x;
				CAPTURE(x);
				CAPTURE(y);
				CAPTURE(ter);
				CHECK(map.get(x, y) == ter);
			}
		}
	}
	SECTION("With vehicles") {
		map.addFeature(0, 0, eMapFeature::BOAT, 1);
		map.addFeature(1, 1, eMapFeature::BOAT, -2);
		map.addFeature(2, 2, eMapFeature::HORSE, 3);
		map.addFeature(3, 3, eMapFeature::HORSE, -4);
		in_and_out("vehicles", map, true);
		test.emplace_back(make_pair(eMapFeature::BOAT, 1));
		CHECK(map.getFeatures(0, 0) == test);
		test[0].second = -2;
		CHECK(map.getFeatures(1, 1) == test);
		test[0].first = eMapFeature::HORSE;
		test[0].second = 3;
		CHECK(map.getFeatures(2, 2) == test);
		test[0].second = -4;
		CHECK(map.getFeatures(3, 3) == test);
	}
	SECTION("With fields") {
		map.addFeature(0, 0, eMapFeature::FIELD, FIELD_WEB);
		map.addFeature(1, 1, eMapFeature::FIELD, SPECIAL_SPOT);
		map.addFeature(2, 2, eMapFeature::FIELD, BARRIER_CAGE);
		map.addFeature(3, 3, eMapFeature::FIELD, OBJECT_BLOCK);
		in_and_out("fields", map, true);
		test.emplace_back(make_pair(eMapFeature::FIELD, FIELD_WEB));
		CHECK(map.getFeatures(0, 0) == test);
		test[0].second = SPECIAL_SPOT;
		CHECK(map.getFeatures(1, 1) == test);
		test[0].second = BARRIER_CAGE;
		CHECK(map.getFeatures(2, 2) == test);
		test[0].second = OBJECT_BLOCK;
		CHECK(map.getFeatures(3, 3) == test);
	}
	SECTION("With fields outdoors") {
		map.addFeature(0, 0, eMapFeature::FIELD, SPECIAL_SPOT);
		map.addFeature(1, 1, eMapFeature::FIELD, SPECIAL_ROAD);
		in_and_out("fields outdoors", map, false);
		test.emplace_back(make_pair(eMapFeature::FIELD, SPECIAL_SPOT));
		CHECK(map.getFeatures(0, 0) == test);
		test[0].second = SPECIAL_ROAD;
		CHECK(map.getFeatures(1, 1) == test);
	}
	SECTION("With town entrance") {
		map.addFeature(0, 0, eMapFeature::TOWN, 4);
		in_and_out("town entry loc", map, false);
		test.emplace_back(make_pair(eMapFeature::TOWN, 4));
		CHECK(map.getFeatures(0, 0) == test);
	}
	SECTION("With town start points") {
		map.addFeature(0, 0, eMapFeature::ENTRANCE_EAST);
		map.addFeature(1, 1, eMapFeature::ENTRANCE_NORTH);
		map.addFeature(2, 2, eMapFeature::ENTRANCE_SOUTH);
		map.addFeature(3, 3, eMapFeature::ENTRANCE_WEST);
		in_and_out("town start loc", map, true);
		test.emplace_back(make_pair(eMapFeature::ENTRANCE_EAST, 0));
		CHECK(map.getFeatures(0, 0) == test);
		test[0].first = eMapFeature::ENTRANCE_NORTH;
		CHECK(map.getFeatures(1, 1) == test);
		test[0].first = eMapFeature::ENTRANCE_SOUTH;
		CHECK(map.getFeatures(2, 2) == test);
		test[0].first = eMapFeature::ENTRANCE_WEST;
		CHECK(map.getFeatures(3, 3) == test);
	}
	SECTION("With misc features") {
		map.addFeature(0, 0, eMapFeature::SIGN);
		map.addFeature(1, 1, eMapFeature::ITEM);
		map.addFeature(2, 2, eMapFeature::CREATURE);
		map.addFeature(3, 3, eMapFeature::SPECIAL_NODE);
		map.addFeature(4, 4, eMapFeature::WANDERING);
		in_and_out("misc", map, true);
		test.emplace_back(make_pair(eMapFeature::SIGN, 0));
		CHECK(map.getFeatures(0, 0) == test);
		test[0].first = eMapFeature::ITEM;
		CHECK(map.getFeatures(1, 1) == test);
		test[0].first = eMapFeature::CREATURE;
		CHECK(map.getFeatures(2, 2) == test);
		test[0].first = eMapFeature::SPECIAL_NODE;
		CHECK(map.getFeatures(3, 3) == test);
		test[0].first = eMapFeature::WANDERING;
		CHECK(map.getFeatures(4, 4) == test);
	}
}

TEST_CASE("Building map data") {
	ifstream fin;
	map_data map;
	fin.exceptions(ios::badbit);
	cScenario scen;
	scen.outdoors.resize(1,1);
	scen.outdoors[0][0] = new cOutdoors(scen);
	scen.addTown(AREA_TINY);
	scen.ter_types.resize(50);
	
	for(int x = 0; x < 5; x++)
		for(int y = 0; y < 5; y++) {
			int n = 1 + y * 5 + x;
			scen.towns[0]->terrain(x, y) = n;
			scen.outdoors[0][0]->terrain[x][y] = n;
		}
	
	SECTION("Basic") {
		in_and_out(scen, true);
		for(int y = 0; y < 5; y++) {
			for(int x = 0; x < 5; x++) {
				int ter = 1 + y * 5 + x;
				CAPTURE(x);
				CAPTURE(y);
				CAPTURE(ter);
				CHECK(scen.towns[0]->terrain(x, y) == ter);
			}
		}
	}
	SECTION("With vehicles") {
		scen.boats.resize(2);
		scen.boats[0].property = true;
		scen.boats[0].loc = {1,1};
		scen.boats[1].property = false;
		scen.boats[1].loc = {2,2};
		scen.horses.resize(2);
		scen.horses[0].property = true;
		scen.horses[0].loc = {3,3};
		scen.horses[1].property = false;
		scen.horses[1].loc = {4,4};
		SECTION("Outdoors") {
			scen.boats[0].which_town = scen.horses[0].which_town = 200;
			scen.boats[1].which_town = scen.horses[1].which_town = 200;
			scen.boats[0].sector = scen.horses[0].sector = {0,0};
			scen.boats[1].sector = scen.horses[1].sector = {0,0};
			in_and_out(scen, false);
			REQUIRE(scen.boats.size() >= 2);
			REQUIRE(scen.horses.size() >= 2);
			CHECK(scen.boats[0].property);
			CHECK(scen.boats[0].loc == loc(1,1));
			CHECK(scen.boats[0].which_town == 200);
			CHECK(scen.boats[0].sector == loc(0,0));
			CHECK_FALSE(scen.boats[1].property);
			CHECK(scen.boats[1].loc == loc(2,2));
			CHECK(scen.boats[1].which_town == 200);
			CHECK(scen.boats[1].sector == loc(0,0));
			CHECK(scen.horses[0].property);
			CHECK(scen.horses[0].loc == loc(3,3));
			CHECK(scen.horses[0].which_town == 200);
			CHECK(scen.horses[0].sector == loc(0,0));
			CHECK_FALSE(scen.horses[1].property);
			CHECK(scen.horses[1].loc == loc(4,4));
			CHECK(scen.horses[1].which_town == 200);
			CHECK(scen.horses[1].sector == loc(0,0));
		}
		SECTION("In town") {
			scen.boats[0].which_town = scen.horses[0].which_town = 0;
			scen.boats[1].which_town = scen.horses[1].which_town = 0;
			in_and_out(scen, true);
			REQUIRE(scen.boats.size() >= 2);
			REQUIRE(scen.horses.size() >= 2);
			CHECK(scen.boats[0].property);
			CHECK(scen.boats[0].loc == loc(1,1));
			CHECK(scen.boats[0].which_town == 0);
			CHECK_FALSE(scen.boats[1].property);
			CHECK(scen.boats[1].loc == loc(2,2));
			CHECK(scen.boats[1].which_town == 0);
			CHECK(scen.horses[0].property);
			CHECK(scen.horses[0].loc == loc(3,3));
			CHECK(scen.horses[0].which_town == 0);
			CHECK_FALSE(scen.horses[1].property);
			CHECK(scen.horses[1].loc == loc(4,4));
			CHECK(scen.horses[1].which_town == 0);
		}
	}
	SECTION("With fields") {
		scen.towns[0]->preset_fields.emplace_back(loc(0,0), FIELD_WEB);
		scen.towns[0]->preset_fields.emplace_back(loc(1,1), OBJECT_BLOCK);
		scen.towns[0]->preset_fields.emplace_back(loc(2,2), BARRIER_CAGE);
		scen.towns[0]->preset_fields.emplace_back(loc(3,3), SPECIAL_SPOT);
		in_and_out(scen, true);
		REQUIRE(scen.towns[0]->preset_fields.size() == 4);
		static const std::map<eFieldType, location> check = {
			{OBJECT_BLOCK, {1,1}}, {SPECIAL_SPOT, {3,3}}, {FIELD_WEB, {0,0}}, {BARRIER_CAGE, {2,2}},
		};
		set<eFieldType> found;
		for(const auto& fld : scen.towns[0]->preset_fields) {
			if(found.count(fld.type))
				FAIL("Error: Two fields of the same type found!");
			found.insert(fld.type);
			CAPTURE(fld.type);
			CAPTURE(fld.loc);
			CAPTURE(check.at(fld.type));
			CHECK(fld.loc == check.at(fld.type));
		}
		if(found.size() != check.size())
			FAIL("Error: A field is missing!");
	}
	SECTION("With fields outdoors") {
		scen.outdoors[0][0]->special_spot[0][0] = true;
		scen.outdoors[0][0]->roads[1][1] = true;
		in_and_out(scen, false);
		CHECK(scen.outdoors[0][0]->special_spot[0][0]);
		CHECK(scen.outdoors[0][0]->roads[1][1]);
	}
	SECTION("With town entrance") {
		scen.outdoors[0][0]->city_locs.emplace_back(5, 6, 7);
		in_and_out(scen, false);
		REQUIRE(scen.outdoors[0][0]->city_locs.size() == 1);
		CHECK(scen.outdoors[0][0]->city_locs[0] == loc(5,6));
		CHECK(scen.outdoors[0][0]->city_locs[0].spec == 7);
	}
	SECTION("With town entry points") {
		int i = 0;
		for(auto& start : scen.towns[0]->start_locs) {
			start.x = i++;
			start.y = i++;
		}
		in_and_out(scen, true);
		i = 0;
		for(const auto& start : scen.towns[0]->start_locs) {
			CAPTURE(i);
			CHECK(start == loc(i, i+1));
			i += 2;
		}
	}
	SECTION("With placed special node") {
		scen.towns[0]->special_locs.emplace_back(12, 13, 14);
		in_and_out(scen, true);
		REQUIRE(scen.towns[0]->special_locs.size() == 1);
		CHECK(scen.towns[0]->special_locs[0] == loc(12,13));
		CHECK(scen.towns[0]->special_locs[0].spec == 14);
	}
	SECTION("With wandering arrival points") {
		int i = 0;
		for(auto& start : scen.towns[0]->wandering_locs) {
			start.x = i++;
			start.y = i++;
		}
		in_and_out(scen, true);
		i = 0;
		for(const auto& start : scen.towns[0]->wandering_locs) {
			CAPTURE(i);
			CHECK(start == loc(i, i+1));
			i += 2;
		}
	}
	SECTION("With placed item") {
		scen.towns[0]->preset_items.emplace_back();
		scen.towns[0]->preset_items[0].loc = {22,12};
		scen.towns[0]->preset_items[0].code = 1;
		map_data map = buildTownMapData(0, scen);
		std::vector<std::pair<eMapFeature,int>> test = {{eMapFeature::ITEM, 0}};
		CHECK(map.getFeatures(22, 12) == test);
	}
	SECTION("With placed creature") {
		scen.towns[0]->creatures.emplace_back();
		scen.towns[0]->creatures[0].start_loc = {9,18};
		scen.towns[0]->creatures[0].number = 1;
		map_data map = buildTownMapData(0, scen);
		std::vector<std::pair<eMapFeature,int>> test = {{eMapFeature::CREATURE, 0}};
		CHECK(map.getFeatures(9, 18) == test);
	}
	SECTION("With a sign") {
		scen.towns[0]->sign_locs.emplace_back(7,11," ");
		map_data map = buildTownMapData(0, scen);
		std::vector<std::pair<eMapFeature,int>> test = {{eMapFeature::SIGN, 0}};
		CHECK(map.getFeatures(7, 11) == test);
	}
}
