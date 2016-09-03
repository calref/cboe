//
//  out_legacy.cpp
//  BoE
//
//  Created by Celtic Minstrel on 16-08-06.
//
//

#include "catch.hpp"
#include "outdoors.hpp"
#include "scenario.hpp"
#include "oldstructs.hpp"

TEST_CASE("Converting legacy outdoor section data") {
	legacy::outdoor_record_type old_sector = {
		{0}, // terrain
		{{2,2}, {5,5}}, // placed specials
		{7, 12},
		{{3,5}, {6,7}, {12,22}, {34,19}}, // towns
		{9,10,11,15},
		{{21,25}}, // signs
		{ // Wandering Monsters
			{{1,2,3,4,5,6,7}, {10,11,12}, 15, 18, 22, true, 121, 220},
		},
		{ // Special Monster Encounters
			{{31,32,33,34,35,36,37}, {50,51,52}, 55, 58, 52, true, 151, 250},
		},
		{{13,13}, {23,23}, {99,99}, {8,40}}, // wandering locs
		{{106, 110, 222, 180}}, // info rects
	};
	cScenario scen;
	scen.ter_types.resize(1);
	cOutdoors sector(scen);
	sector.append(old_sector);
	
	SECTION("General Data") {
		CHECK(sector.ambient_sound == AMBIENT_NONE);
		CHECK(sector.bg_dungeon == -1);
		CHECK(sector.bg_fight == -1);
		CHECK(sector.bg_out == -1);
		CHECK(sector.bg_town == -1);
		CHECK(sector.out_sound == 0);
		REQUIRE(sector.area_desc.size() >= 1);
		CHECK(sector.area_desc[0] == rect(106,110,222,180));
		REQUIRE(sector.sign_locs.size() >= 1);
		CHECK(sector.sign_locs[0] == loc(21,25));
		REQUIRE(sector.wandering_locs.size() >= 4);
		CHECK(sector.wandering_locs[0] == loc(13,13));
		CHECK(sector.wandering_locs[1] == loc(23,23));
		CHECK(sector.wandering_locs[2] == loc(99,99));
		CHECK(sector.wandering_locs[3] == loc(8,40));
	}
	SECTION("Placed Specials") {
		REQUIRE(sector.special_locs.size() >= 2);
		CHECK(sector.special_locs[0] == loc(2,2));
		CHECK(sector.special_locs[0].spec == 7);
		CHECK(sector.special_locs[1] == loc(5,5));
		CHECK(sector.special_locs[1].spec == 12);
	}
	SECTION("Town Entrances") {
		REQUIRE(sector.city_locs.size() >= 4);
		CHECK(sector.city_locs[0] == loc(3,5));
		CHECK(sector.city_locs[0].spec == 9);
		CHECK(sector.city_locs[1] == loc(6,7));
		CHECK(sector.city_locs[1].spec == 10);
		CHECK(sector.city_locs[2] == loc(12,22));
		CHECK(sector.city_locs[2].spec == 11);
		CHECK(sector.city_locs[3] == loc(34,19));
		CHECK(sector.city_locs[3].spec == 15);
	}
	SECTION("Monster Encounters") {
		REQUIRE(sector.wandering.size() >= 1);
		CHECK(sector.wandering[0].monst[0] == 1);
		CHECK(sector.wandering[0].monst[1] == 2);
		CHECK(sector.wandering[0].monst[2] == 3);
		CHECK(sector.wandering[0].monst[3] == 4);
		CHECK(sector.wandering[0].monst[4] == 5);
		CHECK(sector.wandering[0].monst[5] == 6);
		CHECK(sector.wandering[0].monst[6] == 7);
		CHECK(sector.wandering[0].friendly[0] == 10);
		CHECK(sector.wandering[0].friendly[1] == 11);
		CHECK(sector.wandering[0].friendly[2] == 12);
		REQUIRE(sector.special_enc.size() >= 1);
	}
}
