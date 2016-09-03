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
#include "town.hpp"

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
		test.emplace_back(make_pair(eMapFeature::HORSE, 2));
		CHECK(map.getFeatures(0, 0) == test);
		test[0].second = -3;
		CHECK(map.getFeatures(1, 0) == test);
		test[0].first = eMapFeature::BOAT;
		test[0].second = 4;
		CHECK(map.getFeatures(2, 0) == test);
		test[0].second = -5;
		CHECK(map.getFeatures(3, 0) == test);
	}
	SECTION("With fields") {
		fin.open("files/maps/fields.map");
		map = load_map(fin, true, "fields.map");
		test.emplace_back(make_pair(eMapFeature::FIELD, WALL_FORCE));
		CHECK(map.getFeatures(0, 0) == test);
		test[0].second = WALL_FIRE;
		CHECK(map.getFeatures(1, 0) == test);
		test[0].second = FIELD_ANTIMAGIC;
		CHECK(map.getFeatures(2, 0) == test);
		test[0].second = CLOUD_STINK;
		CHECK(map.getFeatures(3, 0) == test);
		test[0].second = WALL_ICE;
		CHECK(map.getFeatures(0, 1) == test);
		test[0].second = WALL_BLADES;
		CHECK(map.getFeatures(1, 1) == test);
		test[0].second = CLOUD_SLEEP;
		CHECK(map.getFeatures(2, 1) == test);
		test[0].second = OBJECT_BLOCK;
		CHECK(map.getFeatures(3, 1) == test);
		test[0].second = SPECIAL_SPOT;
		CHECK(map.getFeatures(0, 2) == test);
		test[0].second = FIELD_WEB;
		CHECK(map.getFeatures(1, 2) == test);
		test[0].second = OBJECT_CRATE;
		CHECK(map.getFeatures(2, 2) == test);
		test[0].second = OBJECT_BARREL;
		CHECK(map.getFeatures(3, 2) == test);
		test[0].second = BARRIER_FIRE;
		CHECK(map.getFeatures(0, 3) == test);
		test[0].second = BARRIER_FORCE;
		CHECK(map.getFeatures(1, 3) == test);
		test[0].second = FIELD_QUICKFIRE;
		CHECK(map.getFeatures(2, 3) == test);
		test[0].second = SFX_SMALL_BLOOD;
		CHECK(map.getFeatures(3, 3) == test);
		test[0].second = SFX_MEDIUM_BLOOD;
		CHECK(map.getFeatures(0, 4) == test);
		test[0].second = SFX_LARGE_BLOOD;
		CHECK(map.getFeatures(1, 4) == test);
		test[0].second = SFX_SMALL_SLIME;
		CHECK(map.getFeatures(2, 4) == test);
		test[0].second = SFX_LARGE_SLIME;
		CHECK(map.getFeatures(3, 4) == test);
		test[0].second = SFX_ASH;
		CHECK(map.getFeatures(0, 5) == test);
		test[0].second = SFX_BONES;
		CHECK(map.getFeatures(1, 5) == test);
		test[0].second = SFX_RUBBLE;
		CHECK(map.getFeatures(2, 5) == test);
		test[0].second = BARRIER_CAGE;
		CHECK(map.getFeatures(3, 5) == test);
		test[0].second = SPECIAL_ROAD;
		CHECK(map.getFeatures(0, 6) == test);
	}
	SECTION("With fields outdoors") {
		fin.open("files/maps/fields_out.map");
		map = load_map(fin, false, "fields_out.map");
		test.emplace_back(make_pair(eMapFeature::FIELD, SPECIAL_SPOT));
		CHECK(map.getFeatures(2, 2) == test);
		test[0].second = SPECIAL_ROAD;
		CHECK(map.getFeatures(2, 3) == test);
		CHECK(map.getFeatures(2, 4) == test);
	}
	SECTION("With town entrance") {
		fin.open("files/maps/towns_out.map");
		map = load_map(fin, false, "towns_out.map");
		test.emplace_back(make_pair(eMapFeature::TOWN, 5));
		CHECK(map.getFeatures(1, 1) == test);
	}
	SECTION("With town start locs") {
		fin.open("files/maps/towns_entry.map");
		map = load_map(fin, true, "towns_entry.map");
		test.emplace_back(make_pair(eMapFeature::ENTRANCE_NORTH, 0));
		CHECK(map.getFeatures(4, 0) == test);
		test[0].first = eMapFeature::ENTRANCE_WEST;
		CHECK(map.getFeatures(0, 1) == test);
		test[0].first = eMapFeature::ENTRANCE_EAST;
		CHECK(map.getFeatures(8, 1) == test);
		test[0].first = eMapFeature::ENTRANCE_SOUTH;
		CHECK(map.getFeatures(4, 2) == test);
	}
	SECTION("With miscellaneous features") {
		fin.open("files/maps/misc.map");
		map = load_map(fin, true, "misc.map");
		test.emplace_back(make_pair(eMapFeature::WANDERING, 1));
		CHECK(map.getFeatures(0, 0) == test);
		test[0] = {eMapFeature::SPECIAL_NODE, 2};
		CHECK(map.getFeatures(1, 0) == test);
		test[0] = {eMapFeature::SIGN, 3};
		CHECK(map.getFeatures(2, 0) == test);
		test[0] = {eMapFeature::ITEM, 4};
		CHECK(map.getFeatures(3, 0) == test);
		test[0] = {eMapFeature::CREATURE, 5};
		CHECK(map.getFeatures(4, 0) == test);
	}
	SECTION("With town start locs outfoors") {
		fin.open("files/maps/towns_entry.map");
		REQUIRE_THROWS_AS(load_map(fin, false, "towns_entry.map"), xMapParseError);
	}
	SECTION("With invalid feature type") {
		fin.open("files/maps/bad_feature.map");
		REQUIRE_THROWS_AS(load_map(fin, true, "bad_feature.map"), xMapParseError);
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
	scen.addTown(AREA_TINY);
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
			CHECK(scen.horses[1].loc == loc(0,0));
			CHECK(scen.horses[2].property);
			CHECK(scen.horses[2].sector == loc(0,0));
			CHECK(scen.horses[2].which_town == 200);
			CHECK(scen.horses[2].loc == loc(1,0));
			REQUIRE(scen.boats.size() >= 5);
			CHECK_FALSE(scen.boats[3].property);
			CHECK(scen.boats[3].sector == loc(0,0));
			CHECK(scen.boats[3].which_town == 200);
			CHECK(scen.boats[3].loc == loc(2,0));
			CHECK(scen.boats[4].property);
			CHECK(scen.boats[4].sector == loc(0,0));
			CHECK(scen.boats[4].which_town == 200);
			CHECK(scen.boats[4].loc == loc(3,0));
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
	SECTION("With fields") {
		fin.open("files/maps/fields.map");
		map = load_map(fin, true, "fields.map");
		loadTownMapData(move(map), 0, scen);
		static const std::map<eFieldType, location> check = {
			{WALL_FORCE, {0,0}}, {WALL_FIRE, {1,0}}, {FIELD_ANTIMAGIC, {2,0}}, {CLOUD_STINK, {3,0}},
			{WALL_ICE, {0,1}}, {WALL_BLADES, {1,1}}, {CLOUD_SLEEP, {2,1}}, {OBJECT_BLOCK, {3,1}},
			{SPECIAL_SPOT, {0,2}}, {FIELD_WEB, {1,2}}, {OBJECT_CRATE, {2,2}}, {OBJECT_BARREL, {3,2}},
			{BARRIER_FIRE, {0,3}}, {BARRIER_FORCE, {1,3}}, {FIELD_QUICKFIRE, {2,3}}, {SFX_SMALL_BLOOD, {3,3}},
			{SFX_MEDIUM_BLOOD, {0,4}}, {SFX_LARGE_BLOOD, {1,4}}, {SFX_SMALL_SLIME, {2,4}}, {SFX_LARGE_SLIME, {3,4}},
			{SFX_ASH, {0,5}}, {SFX_BONES, {1,5}}, {SFX_RUBBLE, {2,5}}, {BARRIER_CAGE, {3,5}},
			{SPECIAL_ROAD, {0,6}},
		};
		CAPTURE(check.size());
		REQUIRE(scen.towns[0]->preset_fields.size() == check.size());
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
		fin.open("files/maps/fields_out.map");
		map = load_map(fin, false, "fields_out.map");
		loadOutMapData(move(map), loc(0,0), scen);
		CHECK(scen.outdoors[0][0]->special_spot[2][2]);
		CHECK(scen.outdoors[0][0]->roads[2][3]);
		CHECK(scen.outdoors[0][0]->roads[2][4]);
	}
	SECTION("With town entrance") {
		fin.open("files/maps/towns_out.map");
		map = load_map(fin, false, "towns_out.map");
		loadOutMapData(move(map), loc(0,0), scen);
		REQUIRE(scen.outdoors[0][0]->city_locs.size() == 1);
		CHECK(scen.outdoors[0][0]->city_locs[0] == loc(1,1));
		CHECK(scen.outdoors[0][0]->city_locs[0].spec == 5);
	}
	SECTION("With town start locs") {
		fin.open("files/maps/towns_entry.map");
		map = load_map(fin, true, "towns_entry.map");
		loadTownMapData(move(map), 0, scen);
		CHECK(scen.towns[0]->start_locs[0] == loc(4,2));
		CHECK(scen.towns[0]->start_locs[1] == loc(0,1));
		CHECK(scen.towns[0]->start_locs[2] == loc(4,0));
		CHECK(scen.towns[0]->start_locs[3] == loc(8,1));
	}
	SECTION("With miscellaneous features") {
		fin.open("files/maps/misc.map");
		map = load_map(fin, true, "misc.map");
		scen.towns[0]->sign_locs.resize(4);
		scen.towns[0]->preset_items.resize(5);
		scen.towns[0]->creatures.resize(6);
		loadTownMapData(move(map), 0, scen);
		CHECK(scen.towns[0]->wandering_locs[1] == loc(0,0));
		REQUIRE(scen.towns[0]->special_locs.size() == 1);
		CHECK(scen.towns[0]->special_locs[0] == loc(1,0));
		CHECK(scen.towns[0]->special_locs[0].spec == 2);
		REQUIRE(scen.towns[0]->sign_locs.size() == 4);
		CHECK(scen.towns[0]->sign_locs[3] == loc(2,0));
		REQUIRE(scen.towns[0]->preset_items.size() == 5);
		CHECK(scen.towns[0]->preset_items[4].loc == loc(3,0));
		REQUIRE(scen.towns[0]->creatures.size() == 6);
		CHECK(scen.towns[0]->creatures[5].start_loc == loc(4,0));
	}
	SECTION("With invalid field outdoors") {
		fin.open("files/maps/fields.map");
		map = load_map(fin, false, "fields.map");
		REQUIRE_THROWS_AS(loadOutMapData(move(map), loc(0,0), scen), xMapParseError);
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
