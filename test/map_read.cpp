//
//  map_read.cpp
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
ostream& operator<< (ostream& out, eMapFeature feat);

TEST_CASE("Loading map data from file") {
	ifstream fin;
	map_data map;
	fin.exceptions(ios::badbit);
	vector<pair<eMapFeature,int>> test;
	
	SECTION("Basic") {
		fin.open("files/maps/basic.map");
		map = load_map(fin, true, "basic.map");
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
		fin.open("files/maps/vehicles.map");
		map = load_map(fin, true, "vehicles.map");
		test.emplace_back(make_pair(eMapFeature::HORSE, 1));
		CHECK(map.getFeatures(0, 0) == test);
		test[0].second = -2;
		CHECK(map.getFeatures(1, 0) == test);
		test[0].first = eMapFeature::BOAT;
		test[0].second = 3;
		CHECK(map.getFeatures(2, 0) == test);
		test[0].second = -4;
		CHECK(map.getFeatures(3, 0) == test);
	}
}

extern void loadOutMapData(map_data&& data, location which, cScenario& scen);
extern void loadTownMapData(map_data&& data, int which, cScenario& scen);

TEST_CASE("Interpreting loaded map data") {
	ifstream fin;
	map_data map;
	fin.exceptions(ios::badbit);
	cScenario scen;
	scen.outdoors.resize(1,1);
	scen.outdoors[0][0] = new cOutdoors(scen);
	scen.addTown<cTinyTown>();
	scen.ter_types.resize(50);
	
	SECTION("Basic") {
		fin.open("files/maps/basic.map");
		map = load_map(fin, true, "basic.map");
		loadTownMapData(move(map), 0, scen);
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
		fin.open("files/maps/vehicles.map");
		SECTION("Outdoors") {
			map = load_map(fin, false, "vehicles.map");
			loadOutMapData(move(map), loc(0,0), scen);
			REQUIRE(scen.horses.size() >= 3);
			CHECK_FALSE(scen.horses[1].property);
			CHECK(scen.horses[1].sector == loc(0,0));
			CHECK(scen.horses[1].which_town == 200);
			CHECK(scen.horses[1].loc_in_sec == loc(0,0));
			CHECK(scen.horses[2].property);
			CHECK(scen.horses[2].sector == loc(0,0));
			CHECK(scen.horses[2].which_town == 200);
			CHECK(scen.horses[2].loc_in_sec == loc(1,0));
			REQUIRE(scen.boats.size() >= 5);
			CHECK_FALSE(scen.boats[3].property);
			CHECK(scen.boats[3].sector == loc(0,0));
			CHECK(scen.boats[3].which_town == 200);
			CHECK(scen.boats[3].loc_in_sec == loc(2,0));
			CHECK(scen.boats[4].property);
			CHECK(scen.boats[4].sector == loc(0,0));
			CHECK(scen.boats[4].which_town == 200);
			CHECK(scen.boats[4].loc_in_sec == loc(3,0));
		}
		SECTION("In town") {
			map = load_map(fin, true, "vehicles.map");
			loadTownMapData(move(map), 0, scen);
			REQUIRE(scen.horses.size() >= 3);
			CHECK_FALSE(scen.horses[1].property);
			CHECK(scen.horses[1].which_town == 0);
			CHECK(scen.horses[1].loc == loc(0,0));
			CHECK(scen.horses[2].property);
			CHECK(scen.horses[2].which_town == 0);
			CHECK(scen.horses[2].loc == loc(1,0));
			REQUIRE(scen.boats.size() >= 5);
			CHECK_FALSE(scen.boats[3].property);
			CHECK(scen.boats[3].which_town == 0);
			CHECK(scen.boats[3].loc == loc(2,0));
			CHECK(scen.boats[4].property);
			CHECK(scen.boats[4].which_town == 0);
			CHECK(scen.boats[4].loc == loc(3,0));
		}
	}
}

#define CASE(x) case eMapFeature::x: out << #x; break

ostream& operator<< (ostream& out, eMapFeature feat) {
	switch(feat) {
		CASE(BOAT);
		CASE(CREATURE);
		CASE(ENTRANCE_EAST);
		CASE(ENTRANCE_NORTH);
		CASE(ENTRANCE_SOUTH);
		CASE(ENTRANCE_WEST);
		CASE(FIELD);
		CASE(HORSE);
		CASE(ITEM);
		CASE(NONE);
		CASE(SIGN);
		CASE(SPECIAL_NODE);
		CASE(TOWN);
		CASE(WANDERING);
	}
	return out;
}

template<typename T1, typename T2>
ostream& operator<< (ostream& out, const pair<T1,T2>& p) {
	out << '(' << p.first << ',' << p.second << ')';
	return out;
}
