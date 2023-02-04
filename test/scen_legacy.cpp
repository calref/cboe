//
//  scen_legacy.cpp
//  BoE
//
//  Created by Celtic Minstrel on 16-08-05.
//
//

#include "catch.hpp"
#include "scenario/scenario.hpp"
#include "oldstructs.hpp"

TEST_CASE("Converting legacy scenario data") {
	legacy::scenario_data_type old_scen = {
		2, 2, 1, 27, 0,
		{1, 1}, // town sizes
		{false, true}, // town hidden
		0x0B0E, // password flag A
		28, 8,
		0, // padding
		{3,3},
		{4,4}, {1,1},
		3,
		0x0B0E, // password flag B
		{{0}}, // town data size; not relevant for loading just the header
		{1}, {{2,7}},
		0x0B0E, // password flag C
		{0}, // out data size; not relevant for loading just the header
		{{1,2,3,4}, {2,4,6,8}, {1,8,2,9}},
		{0, 1, 2},
		0x0B0E, // password flag E
		{5, 9}, {12, -1}, // special items
		2, false,
		0x0B0E, // password flag F
		{0}, // monsters; tested separately
		{ // boats
			{
				{33,33}, {22,22}, {1,1},
				2, true, true
			}
		},
		{0}, // horses; same as boats, so not tested
		0x0B0E, // password flag G
		{0}, // terrains; tested separately
		{100, 1200}, {3, 88, -1}, // timers
		0x0B0E, // password flag H
		{0}, // specials; tested separately
		{ // item placement shortcuts
			{12, {1,2,3,4,5,6,7,8,9,10}, {10,15,20,25,30,35,40,45,50,55}, true}
		},
		0x0B0E, // password flag D
		{0}, // string lengths; not relevant for loading just the header
		0x0B0E, // password flag I
		{1,1},
		2
	};
	cScenario scen;
	scen.import_legacy(old_scen);
	
	SECTION("Basic header data") {
		CHECK(scen.adjust_diff);
		CHECK(scen.bg_dungeon == 9);
		CHECK(scen.bg_fight == 4);
		CHECK(scen.bg_out == 10);
		CHECK(scen.bg_town == 13);
		CHECK(scen.campaign_id.empty());
		CHECK(scen.custom_graphics.empty());
		CHECK(scen.default_ground == 0);
		CHECK(scen.difficulty == 1);
		CHECK(scen.init_spec == -1);
		CHECK(scen.intro_mess_pic == cPictNum(27,PIC_SCEN));
		CHECK(scen.intro_pic == cPictNum(27,PIC_SCEN));
		CHECK(scen.is_legacy);
		CHECK(scen.journal_strs.empty());
		CHECK(scen.last_out_edited == loc(1,1));
		CHECK(scen.last_town_edited == 2);
		CHECK(scen.out_sec_start == loc(4,4));
		CHECK(scen.out_start == loc(1,1));
		CHECK(scen.rating == eContentRating::R);
		CHECK(scen.shops.empty());
		CHECK_FALSE(scen.uses_custom_graphics);
		CHECK(scen.where_start == loc(3,3));
		CHECK(scen.which_town_start == 3);
	}
	SECTION("With boats") {
		REQUIRE(scen.boats.size() >= 1);
		CHECK(scen.boats[0].exists);
		CHECK(scen.boats[0].loc == loc(33,33));
		// TODO: This field is meaningless in legacy scenario boats but matters in legacy svaed game boats.
//		CHECK(scen.boats[0].loc_in_sec == loc(22,22));
		CHECK(scen.boats[0].property);
		CHECK(scen.boats[0].sector == loc(1,1));
		CHECK(scen.boats[0].which_town == 2);
	}
	SECTION("With special items") {
		REQUIRE(scen.special_items.size() >= 2);
		CHECK(scen.special_items[0].flags == 5);
		CHECK(scen.special_items[0].special == 12);
		CHECK(scen.special_items[1].flags == 9);
		CHECK(scen.special_items[1].special == -1);
	}
	SECTION("With item storage rects") {
		REQUIRE(scen.store_item_rects.size() >= 3);
		REQUIRE(scen.store_item_towns.size() >= 3);
		CHECK(scen.store_item_rects[0] == rect(1,2,3,4));
		CHECK(scen.store_item_towns[0] == 0);
		CHECK(scen.store_item_rects[1] == rect(2,4,6,8));
		CHECK(scen.store_item_towns[1] == 1);
		CHECK(scen.store_item_rects[2] == rect(1,8,2,9));
		CHECK(scen.store_item_towns[2] == 2);
	}
	SECTION("With item storage shortcuts") {
		REQUIRE(scen.storage_shortcuts.size() >= 1);
		CHECK(scen.storage_shortcuts[0].ter_type == 12);
		CHECK(scen.storage_shortcuts[0].property);
		for(int i = 0; i < 10; i++) {
			int item_num = old_scen.storage_shortcuts[0].item_num[i];
			CAPTURE(i);
			CAPTURE(item_num);
			CHECK(scen.storage_shortcuts[0].item_num[i] == item_num);
			int item_odds = old_scen.storage_shortcuts[0].item_odds[i];
			CAPTURE(i);
			CAPTURE(item_odds);
			CHECK(scen.storage_shortcuts[0].item_odds[i] == item_odds);
		}
	}
	SECTION("With town mods") {
		REQUIRE(scen.town_mods.size() >= 1);
		CHECK(scen.town_mods[0].spec == 1);
		CHECK(scen.town_mods[0] == loc(2,7));
	}
}
