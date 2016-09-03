//
//  town_legacy.cpp
//  BoE
//
//  Created by Celtic Minstrel on 16-08-09.
//
//

#include "catch.hpp"
#include "town.hpp"
#include "scenario.hpp"
#include "oldstructs.hpp"

TEST_CASE("Converting legacy town data") {
	cScenario scen;
	scen.ter_types.resize(5);
	legacy::town_record_type old_town = {0};
	
	SECTION("With basic data") {
		old_town.town_chop_time = 5;
		old_town.town_chop_key = 6;
		old_town.wandering_locs[0] = {3,4};
		old_town.special_locs[0] = {1,2};
		old_town.spec_id[0] = 7;
		old_town.sign_locs[0] = {8,9};
		old_town.lighting = 0;
		old_town.start_locs[0] = {15,0};
		old_town.start_locs[1] = {0,15};
		old_town.start_locs[2] = {45,0};
		old_town.start_locs[3] = {0,45};
		old_town.exit_locs[0] = {11,12};
		old_town.exit_specs[0] = 10;
		old_town.in_town_rect = {14, 14, 30, 30};
		old_town.max_num_monst = 12345;
		old_town.spec_on_entry = 13;
		old_town.spec_on_entry_if_dead = 16;
		old_town.timer_spec_times[0] = 17;
		old_town.timer_specs[0] = 19;
		old_town.specials1 = 20;
		old_town.specials2 = 21;
		old_town.difficulty = 1;
		cTown town(scen, AREA_MEDIUM);
		town.import_legacy(old_town);
		CHECK(town.town_chop_time == 5);
		CHECK(town.town_chop_key == 6);
		CHECK(town.wandering_locs[0] == loc(3,4));
		REQUIRE(town.special_locs.size() >= 1);
		CHECK(town.special_locs[0] == loc(1,2));
		CHECK(town.special_locs[0].spec == 7);
		REQUIRE(town.sign_locs.size() >= 1);
		CHECK(town.sign_locs[0] == loc(8,9));
		CHECK(town.lighting_type == LIGHT_NORMAL);
		CHECK(town.start_locs[0] == loc(15,0));
		CHECK(town.start_locs[1] == loc(0,15));
		CHECK(town.start_locs[2] == loc(45,0));
		CHECK(town.start_locs[3] == loc(0,45));
		CHECK(town.exits[0] == loc(11,12));
		CHECK(town.exits[0].spec == 10);
		CHECK(town.in_town_rect == rect(14,14,30,30));
		CHECK(town.max_num_monst == 12345);
		CHECK(town.spec_on_entry == 13);
		CHECK(town.spec_on_entry_if_dead == 16);
		CHECK(town.spec_on_hostile == -1);
		REQUIRE(town.timers.size() >= 1);
		CHECK(town.timers[0].time == 17);
		CHECK(town.timers[0].node == 19);
		CHECK(town.bg_town == -1);
		CHECK(town.bg_fight == -1);
		CHECK_FALSE(town.strong_barriers);
		CHECK_FALSE(town.defy_mapping);
		CHECK_FALSE(town.defy_scrying);
		CHECK_FALSE(town.is_hidden);
		CHECK_FALSE(town.has_tavern);
	}
	SECTION("With preset items") {
		old_town.preset_items[0].item_loc = {7,8};
		old_town.preset_items[0].item_code = 3;
		old_town.preset_items[0].charges = 10;
		old_town.preset_items[0].ability = 9;
		old_town.preset_items[0].always_there = true;
		old_town.preset_items[0].property = true;
		old_town.preset_items[0].contained = true;
		cTown town(scen, AREA_TINY);
		town.import_legacy(old_town);
		REQUIRE(town.preset_items.size() >= 1);
		CHECK(town.preset_items[0].loc == loc(7,8));
		CHECK(town.preset_items[0].code == 3);
		CHECK(town.preset_items[0].charges == 9);
		CHECK(town.preset_items[0].ability == -1);
		CHECK(town.preset_items[0].always_there);
		CHECK(town.preset_items[0].property);
		CHECK(town.preset_items[0].contained);
	}
	SECTION("With preset fields") {
		int j = 0;
		for(int i = 3; i <= 8; i++, j++) {
			old_town.preset_fields[j].field_loc.x = i;
			old_town.preset_fields[j].field_loc.y = i;
			old_town.preset_fields[j].field_type= i;
		}
		for(int i = 14; i <= 21; i++, j++) {
			old_town.preset_fields[j].field_loc.x = i;
			old_town.preset_fields[j].field_loc.y = i;
			old_town.preset_fields[j].field_type = i;
		}
		cTown town(scen, AREA_SMALL);
		town.import_legacy(old_town);
		REQUIRE(town.preset_fields.size() >= 14);
		CHECK(town.preset_fields[0].loc == loc(3,3));
		CHECK(town.preset_fields[0].type == FIELD_WEB);
		CHECK(town.preset_fields[1].loc == loc(4,4));
		CHECK(town.preset_fields[1].type == OBJECT_CRATE);
		CHECK(town.preset_fields[2].loc == loc(5,5));
		CHECK(town.preset_fields[2].type == OBJECT_BARREL);
		CHECK(town.preset_fields[3].loc == loc(6,6));
		CHECK(town.preset_fields[3].type == BARRIER_FIRE);
		CHECK(town.preset_fields[4].loc == loc(7,7));
		CHECK(town.preset_fields[4].type == BARRIER_FORCE);
		CHECK(town.preset_fields[5].loc == loc(8,8));
		CHECK(town.preset_fields[5].type == FIELD_QUICKFIRE);
		CHECK(town.preset_fields[6].loc == loc(14,14));
		CHECK(town.preset_fields[6].type == SFX_SMALL_BLOOD);
		CHECK(town.preset_fields[7].loc == loc(15,15));
		CHECK(town.preset_fields[7].type == SFX_MEDIUM_BLOOD);
		CHECK(town.preset_fields[8].loc == loc(16,16));
		CHECK(town.preset_fields[8].type == SFX_LARGE_BLOOD);
		CHECK(town.preset_fields[9].loc == loc(17,17));
		CHECK(town.preset_fields[9].type == SFX_SMALL_SLIME);
		CHECK(town.preset_fields[10].loc == loc(18,18));
		CHECK(town.preset_fields[10].type == SFX_LARGE_SLIME);
		CHECK(town.preset_fields[11].loc == loc(19,19));
		CHECK(town.preset_fields[11].type == SFX_ASH);
		CHECK(town.preset_fields[12].loc == loc(20,20));
		CHECK(town.preset_fields[12].type == SFX_BONES);
		CHECK(town.preset_fields[13].loc == loc(21,21));
		CHECK(town.preset_fields[13].type == SFX_RUBBLE);
	}
	SECTION("With creatures") {
		legacy::tiny_tr_type pop = {0};
		
		struct creature_start_type {
			uint8_t number;
			uint8_t start_attitude;
			location start_loc;
			uint8_t  mobile;
			uint8_t  time_flag;
			uint8_t  extra1,extra2;
			int16_t spec1, spec2;
			int8_t spec_enc_code,time_code;
			int16_t monster_time,personality;
			int16_t special_on_kill,facial_pic;
		};
		pop.creatures[0] = {
			12, // number
			2, // start attitude
			{3,4}, // start pos
			1, 7, // mobile, time_flag
			6, 5, // extra1, extra2 (unused)
			9, 10, // spec1, spec2
			8, 12, 11, // spec_enc_code, time_code, monster_time
			15, // personality
			22, 19 // spec_on_kill, facial_pic
		};
		cTown town(scen, AREA_SMALL);
		// import_legacy expects special arrays to be filled
		town.specials.resize(100);
		town.special_locs.resize(50);
		town.import_legacy(pop, 0);
		REQUIRE(town.creatures.size() >= 1);
		CHECK(town.creatures[0].number == 12);
		CHECK(town.creatures[0].start_attitude == eAttitude::FRIENDLY);
		CHECK(town.creatures[0].start_loc == loc(3,4));
		CHECK(town.creatures[0].mobility == 1);
		CHECK(town.creatures[0].time_flag == eMonstTime::APPEAR_WHEN_EVENT);
		CHECK(town.creatures[0].spec1 == 9);
		CHECK(town.creatures[0].spec2 == 10);
		CHECK(town.creatures[0].spec_enc_code == 8);
		CHECK(town.creatures[0].time_code == 12);
		CHECK(town.creatures[0].monster_time == 11);
		CHECK(town.creatures[0].personality == 15);
		CHECK(town.creatures[0].special_on_kill == 22);
		CHECK(town.creatures[0].facial_pic == 18);
	}
	SECTION("With wandering monsters") {
		old_town.wandering[0] = {1,2,3,4};
		old_town.wandering[1] = {5,6,7,8};
		cTown town(scen, AREA_TINY);
		// import_legacy expects special arrays to be filled
		town.specials.resize(100);
		town.special_locs.resize(50);
		town.import_legacy(old_town);
		REQUIRE(town.wandering.size() >= 2);
		CHECK(town.wandering[0].monst[0] == 1);
		CHECK(town.wandering[0].monst[1] == 2);
		CHECK(town.wandering[0].monst[2] == 3);
		CHECK(town.wandering[0].monst[3] == 4);
		CHECK(town.wandering[1].monst[0] == 5);
		CHECK(town.wandering[1].monst[1] == 6);
		CHECK(town.wandering[1].monst[2] == 7);
		CHECK(town.wandering[1].monst[3] == 8);
	}
	SECTION("Big town") {
		legacy::big_tr_type big = {0};
		big.terrain[2][3] = 15;
		big.room_rect[0] = {12,13,14,15};
		cTown town(scen, AREA_LARGE);
		// import_legacy expects special arrays to be filled
		town.specials.resize(100);
		town.special_locs.resize(50);
		town.import_legacy(big, 0);
		CHECK(town.max_dim == 64);
		CHECK(town.terrain[2][3] == 15);
		REQUIRE(town.area_desc.size() >= 1);
		CHECK(town.area_desc[0] == rect(12,13,14,15));
		CHECK(town.creatures.size() == 60);
	}
	SECTION("Medium town") {
		legacy::ave_tr_type med = {0};
		med.terrain[2][3] = 15;
		med.room_rect[0] = {12,13,14,15};
		cTown town(scen, AREA_MEDIUM);
		// import_legacy expects special arrays to be filled
		town.specials.resize(100);
		town.special_locs.resize(50);
		town.import_legacy(med, 0);
		CHECK(town.max_dim == 48);
		CHECK(town.terrain[2][3] == 15);
		REQUIRE(town.area_desc.size() >= 1);
		CHECK(town.area_desc[0] == rect(12,13,14,15));
		CHECK(town.creatures.size() == 40);
	}
	SECTION("Small town") {
		legacy::tiny_tr_type sm = {0};
		sm.terrain[2][3] = 15;
		sm.room_rect[0] = {12,13,14,15};
		cTown town(scen, AREA_SMALL);
		// import_legacy expects special arrays to be filled
		town.specials.resize(100);
		town.special_locs.resize(50);
		town.import_legacy(sm, 0);
		CHECK(town.max_dim == 32);
		CHECK(town.terrain[2][3] == 15);
		REQUIRE(town.area_desc.size() >= 1);
		CHECK(town.area_desc[0] == rect(12,13,14,15));
		CHECK(town.creatures.size() == 30);
	}
}
