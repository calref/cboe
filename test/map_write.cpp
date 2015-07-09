//
//  map_write.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-07-09.
//
//

#include <fstream>
#include "catch.hpp"
#include "map_parse.hpp"
#include "scenario.hpp"
#include "regtown.hpp"

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
	if(town) {
		map_data map = buildTownMapData(0, scen);
		// Reconstruct the town, to ensure that it doesn't just pass due to old data still being around
		delete scen.towns[0];
		scen.towns[0] = new cTinyTown(scen);
		loadTownMapData(move(map), 0, scen);
	} else {
		map_data map = buildOutMapData(loc(0,0), scen);
		// Reconstruct the sector, to ensure that it doesn't just pass due to old data still being around
		delete scen.outdoors[0][0];
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
}

TEST_CASE("Building map data") {
	ifstream fin;
	map_data map;
	fin.exceptions(ios::badbit);
	cScenario scen;
	scen.outdoors.resize(1,1);
	scen.outdoors[0][0] = new cOutdoors(scen);
	scen.addTown<cTinyTown>();
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
}
