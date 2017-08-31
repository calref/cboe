//
//  ter_legacy.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-10-04.
//
//

#include "catch.hpp"
#include "terrain.hpp"
#include "oldstructs.hpp"
#include "damage.hpp"

TEST_CASE("Converting terrain types from legacy scenarios") {
	cTerrain new_ter;
	legacy::terrain_type_type old_ter = {
		26,
		2, 0, 0, 0, 12, true, true,
		true, 17, 3, 'c', 0, 0, 0,
	};
	SECTION("Basic information") {
		new_ter.import_legacy(old_ter);
		CHECK(new_ter.picture == 26);
		CHECK(new_ter.blockage == eTerObstruct::BLOCK_MONSTERS);
		CHECK(new_ter.trans_to_what == 12);
		CHECK(new_ter.fly_over);
		CHECK(new_ter.boat_over);
		CHECK(new_ter.block_horse);
		CHECK_FALSE(new_ter.is_archetype);
		CHECK(new_ter.light_radius == 17);
		CHECK(new_ter.step_sound == eStepSnd::NONE);
		CHECK(new_ter.shortcut_key == 'c');
		// Some details from translation
		CHECK(new_ter.combat_arena == 2);
		CHECK(new_ter.trim_type == eTrimType::W);
		CHECK(new_ter.trim_ter == 4);
		CHECK(new_ter.ground_type == 3);
		CHECK(new_ter.special == eTerSpec::NONE);
		CHECK(new_ter.flag1 == -1);
		CHECK(new_ter.flag2 == 0);
		CHECK(new_ter.flag3 == 0);
	}
	SECTION("With special properties") {
		SECTION("Change when walk") {
			old_ter.special = 1;
			old_ter.flag1 = 20;
			old_ter.flag2 = 200;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::CHANGE_WHEN_STEP_ON);
			CHECK(new_ter.flag1 == 20);
			CHECK(new_ter.flag2 == -1);
		}
		SECTION("Fire damage") {
			old_ter.special = 2;
			old_ter.flag1 = 3;
			old_ter.flag2 = 6;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::DAMAGING);
			CHECK(new_ter.flag1 == 3);
			CHECK(new_ter.flag2 == 6);
			CHECK(new_ter.flag3 == int(eDamageType::FIRE));
		}
		SECTION("Cold damage") {
			old_ter.special = 3;
			old_ter.flag1 = 3;
			old_ter.flag2 = 6;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::DAMAGING);
			CHECK(new_ter.flag1 == 3);
			CHECK(new_ter.flag2 == 6);
			CHECK(new_ter.flag3 == int(eDamageType::COLD));
		}
		SECTION("Magic damage") {
			old_ter.special = 4;
			old_ter.flag1 = 3;
			old_ter.flag2 = 6;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::DAMAGING);
			CHECK(new_ter.flag1 == 3);
			CHECK(new_ter.flag2 == 6);
			CHECK(new_ter.flag3 == int(eDamageType::MAGIC));
		}
		SECTION("Poison land") {
			old_ter.special = 5;
			old_ter.flag1 = 8;
			old_ter.flag2 = 75;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::DANGEROUS);
			CHECK(new_ter.flag1 == 8);
			CHECK(new_ter.flag2 == 75);
			CHECK(new_ter.flag3 == int(eStatus::POISON));
		}
		SECTION("Disease land") {
			old_ter.special = 6;
			old_ter.flag1 = 8;
			old_ter.flag2 = 75;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::DANGEROUS);
			CHECK(new_ter.flag1 == 8);
			CHECK(new_ter.flag2 == 75);
			CHECK(new_ter.flag3 == int(eStatus::DISEASE));
		}
		SECTION("Crumbling") {
			old_ter.special = 7;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::CRUMBLING);
			CHECK(new_ter.flag1 == 0);
			CHECK(new_ter.flag2 == 0);
		}
		SECTION("Lockable") {
			old_ter.special = 8;
			old_ter.flag1 = 20;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::LOCKABLE);
			CHECK(new_ter.flag1 == 20);
		}
		SECTION("Unlockable") {
			old_ter.special = 9;
			old_ter.flag1 = 20;
			old_ter.flag2 = 10;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::UNLOCKABLE);
			CHECK(new_ter.flag1 == 20);
			CHECK(new_ter.flag2 == 10);
			CHECK(new_ter.flag3 == 0);
		}
		SECTION("Unlockable/bashable") {
			old_ter.special = 10;
			old_ter.flag1 = 20;
			old_ter.flag2 = 10;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::UNLOCKABLE);
			CHECK(new_ter.flag1 == 20);
			CHECK(new_ter.flag2 == 10);
			CHECK(new_ter.flag3 == 1);
		}
		SECTION("Sign") {
			old_ter.special = 11;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::IS_A_SIGN);
		}
		SECTION("Call local special") {
			old_ter.special = 12;
			old_ter.flag1 = 20;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::CALL_SPECIAL);
			CHECK(new_ter.flag1 == 20);
			CHECK(new_ter.flag2 == 1);
		}
		SECTION("Call global special") {
			old_ter.special = 13;
			old_ter.flag1 = 20;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::CALL_SPECIAL);
			CHECK(new_ter.flag1 == 20);
			CHECK(new_ter.flag2 == 0);
		}
		SECTION("Container") {
			old_ter.special = 14;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::IS_A_CONTAINER);
		}
		SECTION("Waterfall") {
			old_ter.special = 15;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::WATERFALL_CAVE);
			CHECK(new_ter.flag1 == DIR_S);
			CHECK(new_ter.flag2 == 5);
			CHECK(new_ter.flag3 == 90);
		}
		SECTION("Conveyor belt (north)") {
			old_ter.special = 16;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::CONVEYOR);
			CHECK(new_ter.flag1 == DIR_N);
		}
		SECTION("Conveyor belt (east)") {
			old_ter.special = 17;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::CONVEYOR);
			CHECK(new_ter.flag1 == DIR_E);
		}
		SECTION("Conveyor belt (south)") {
			old_ter.special = 18;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::CONVEYOR);
			CHECK(new_ter.flag1 == DIR_S);
		}
		SECTION("Conveyor belt (west)") {
			old_ter.special = 19;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::CONVEYOR);
			CHECK(new_ter.flag1 == DIR_W);
		}
		SECTION("Blocked to monsters") {
			old_ter.special = 20;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::BLOCKED_TO_MONSTERS);
		}
		SECTION("Town entrance") {
			old_ter.special = 21;
			old_ter.flag1 = 20;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::TOWN_ENTRANCE);
			CHECK(new_ter.flag1 == 20);
		}
		SECTION("Change when used") {
			old_ter.special = 22;
			old_ter.flag1 = 20;
			old_ter.flag2 = 200;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::CHANGE_WHEN_USED);
			CHECK(new_ter.flag1 == 20);
			// Unlike change when step, this didn't originally have special allowance for no sound
			CHECK(new_ter.flag2 == 200);
		}
		SECTION("Call scenario special when used") {
			old_ter.special = 23;
			old_ter.flag1 = 20;
			new_ter.import_legacy(old_ter);
			CHECK(new_ter.special == eTerSpec::CALL_SPECIAL_WHEN_USED);
			CHECK(new_ter.flag1 == 20);
			CHECK(new_ter.flag2 == 0);
		}
	}
}
