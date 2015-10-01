//
//  monst_legacy.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-09-30.
//
//

#include <catch.hpp>
#include "monster.hpp"
#include "oldstructs.hpp"

TEST_CASE("Converting monsters from legacy scenarios") {
	// Some of the data in this struct is unused in scenarios.
	// (It's only used in saved games.)
	legacy::monster_record_type old_monst = {
		0, 5, "Test Monster",
		50, 50, 25, 25,
		8, 10,
		{108, 212, 306},
		2, 1, 3, 4, 4, 2, 1, 0, 0, 3, 0, 0,
		50, 50,
		142, 75,
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		0, 0x44, 1, 1, 0, 0,
		2, 1, 52, 0, 0, 0,
		21,
	};
	cMonster new_monst;
	
	SECTION("Basic monster") {
		new_monst.append(old_monst);
		CHECK(new_monst.level == 5);
		CHECK(new_monst.m_name == "Test Monster");
		CHECK(new_monst.m_health == 50);
		CHECK(new_monst.armor == 8);
		CHECK(new_monst.skill == 10);
		CHECK(new_monst.m_type == eRace::IMPORTANT);
		CHECK(new_monst.speed == 4);
		CHECK(new_monst.mu == 2);
		CHECK(new_monst.cl == 1);
		CHECK(new_monst.treasure == 3);
		CHECK(new_monst.corpse_item == 142);
		CHECK(new_monst.corpse_item_chance == 75);
		CHECK(new_monst.x_width == 1);
		CHECK(new_monst.y_width == 1);
		CHECK(new_monst.default_attitude == eAttitude::FRIENDLY);
		CHECK(new_monst.default_facial_pic == 51);
		CHECK(new_monst.picture_num == 21);
		CHECK(new_monst.summon_type == 1);
		CHECK(new_monst.ambient_sound == snd_num_t(-1));
		CHECK(new_monst.see_spec == -1);
		CHECK_FALSE(new_monst.mindless);
		CHECK_FALSE(new_monst.invuln);
		CHECK_FALSE(new_monst.invisible);
		CHECK_FALSE(new_monst.guard);
		// Attacks
		CHECK(new_monst.a[0].dice == 1);
		CHECK(new_monst.a[0].sides == 8);
		CHECK(new_monst.a[0].type == eMonstMelee::BITE);
		CHECK(new_monst.a[1].dice == 2);
		CHECK(new_monst.a[1].sides == 12);
		CHECK(new_monst.a[1].type == eMonstMelee::CLAW);
		CHECK(new_monst.a[2].dice == 3);
		CHECK(new_monst.a[2].sides == 6);
		CHECK(new_monst.a[2].type == eMonstMelee::CLAW);
		// Immunities
		CHECK(new_monst.resist[eDamageType::WEAPON] == 100);
		CHECK(new_monst.resist[eDamageType::MAGIC] == 100);
		CHECK(new_monst.resist[eDamageType::FIRE] == 50);
		CHECK(new_monst.resist[eDamageType::COLD] == 100);
		CHECK(new_monst.resist[eDamageType::POISON] == 50);
		CHECK(new_monst.resist[eDamageType::UNDEAD] == 100);
		CHECK(new_monst.resist[eDamageType::DEMON] == 100);
		CHECK(new_monst.resist[eDamageType::UNBLOCKABLE] == 100);
		CHECK(new_monst.resist[eDamageType::SPECIAL] == 100);
		// Abilities (should have none)
		CHECK(new_monst.abil.size() == 0);
	}
	SECTION("With breath weapons") {
		old_monst.breath = 12;
		SECTION("Fire") {
			old_monst.breath_type = 0;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::DAMAGE2].active);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.type == eMonstGen::BREATH);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.pic == 13);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.strength == 12);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.range == 8);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.odds == 375);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.dmg == eDamageType::FIRE);
		}
		SECTION("Cold") {
			old_monst.breath_type = 1;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::DAMAGE2].active);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.type == eMonstGen::BREATH);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.pic == 6);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.strength == 12);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.range == 8);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.odds == 375);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.dmg == eDamageType::COLD);
		}
		SECTION("Electricity") {
			old_monst.breath_type = 2;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::DAMAGE2].active);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.type == eMonstGen::BREATH);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.pic == 8);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.strength == 12);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.range == 8);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.odds == 375);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.dmg == eDamageType::MAGIC);
		}
		SECTION("Darkness") {
			old_monst.breath_type = 3;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::DAMAGE2].active);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.type == eMonstGen::BREATH);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.pic == 8);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.strength == 12);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.range == 8);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.odds == 375);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].gen.dmg == eDamageType::UNBLOCKABLE);
		}
	}
	SECTION("With poison") {
		old_monst.poison = 8;
		new_monst.append(old_monst);
		REQUIRE(new_monst.abil.size() == 1);
		REQUIRE(new_monst.abil[eMonstAbil::STATUS2].active);
		CHECK(new_monst.abil[eMonstAbil::STATUS2].gen.type == eMonstGen::TOUCH);
		CHECK(new_monst.abil[eMonstAbil::STATUS2].gen.strength == 8);
		CHECK(new_monst.abil[eMonstAbil::STATUS2].gen.odds == 1000);
		CHECK(new_monst.abil[eMonstAbil::STATUS2].gen.stat == eStatus::POISON);
	}
	SECTION("With primary abilities") {
		SECTION("Throws Darts") {
			old_monst.spec_skill = 1;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MISSILE].active);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.type == eMonstMissile::DART);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.pic == 1);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.dice == 1);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.sides == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.skill == 2);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.range == 6);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.odds == 500);
		}
		SECTION("Shoots Arrows") {
			old_monst.spec_skill = 2;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MISSILE].active);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.type == eMonstMissile::ARROW);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.pic == 3);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.dice == 2);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.sides == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.skill == 4);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.range == 8);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.odds == 750);
		}
		SECTION("Throws Spears") {
			old_monst.spec_skill = 3;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MISSILE].active);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.type == eMonstMissile::SPEAR);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.pic == 5);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.dice == 3);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.sides == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.skill == 6);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.range == 8);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.odds == 625);
		}
		SECTION("Throws Rocks 4-24") {
			old_monst.spec_skill = 4;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MISSILE].active);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.type == eMonstMissile::BOULDER);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.pic == 12);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.dice == 4);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.sides == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.skill == 8);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.range == 10);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.odds == 625);
		}
		SECTION("Throws Rocks 5-30") {
			old_monst.spec_skill = 5;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MISSILE].active);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.type == eMonstMissile::BOULDER);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.pic == 12);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.dice == 6);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.sides == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.skill == 12);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.range == 10);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.odds == 500);
		}
		SECTION("Throws Rocks 6-36") {
			old_monst.spec_skill = 6;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MISSILE].active);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.type == eMonstMissile::BOULDER);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.pic == 12);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.dice == 8);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.sides == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.skill == 16);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.range == 10);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.odds == 500);
		}
		SECTION("Throws Razordisks") {
			old_monst.spec_skill = 7;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MISSILE].active);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.type == eMonstMissile::RAZORDISK);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.pic == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.dice == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.sides == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.skill == 14);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.range == 8);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.odds == 625);
		}
		SECTION("Petrification Ray") {
			old_monst.spec_skill = 8;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::PETRIFY].active);
			CHECK(new_monst.abil[eMonstAbil::PETRIFY].gen.type == eMonstGen::GAZE);
			CHECK(new_monst.abil[eMonstAbil::PETRIFY].gen.strength == 25);
			CHECK(new_monst.abil[eMonstAbil::PETRIFY].gen.range == 6);
			CHECK(new_monst.abil[eMonstAbil::PETRIFY].gen.odds == 625);
		}
		SECTION("Spell Point Drain Ray") {
			old_monst.spec_skill = 9;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::DRAIN_SP].active);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_SP].gen.type == eMonstGen::GAZE);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_SP].gen.strength == 50);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_SP].gen.range == 8);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_SP].gen.odds == 625);
		}
		SECTION("Heat Ray") {
			old_monst.spec_skill = 10;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::RAY_HEAT].active);
			CHECK(new_monst.abil[eMonstAbil::RAY_HEAT].special.extra1 == 6);
			CHECK(new_monst.abil[eMonstAbil::RAY_HEAT].special.extra2 == 625);
			CHECK(new_monst.abil[eMonstAbil::RAY_HEAT].special.extra3 == 7);
		}
		SECTION("Invisible") {
			old_monst.spec_skill = 11;
			new_monst.append(old_monst);
			CHECK(new_monst.abil.size() == 0);
			CHECK(new_monst.invisible);
		}
		SECTION("Splits When Hit") {
			old_monst.spec_skill = 12;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::SPLITS].active);
			CHECK(new_monst.abil[eMonstAbil::SPLITS].special.extra1 == 1000);
		}
		SECTION("Mindless") {
			old_monst.spec_skill = 13;
			new_monst.append(old_monst);
			CHECK(new_monst.abil.size() == 0);
			CHECK(new_monst.mindless);
		}
		SECTION("Breathes Stinking Clouds") {
			old_monst.spec_skill = 14;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::FIELD].active);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.type == eMonstGen::BREATH);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.pic == 12);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.strength == PAT_SINGLE);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.range == 6);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.odds == 375);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.fld == CLOUD_STINK);
		}
		SECTION("Icy Touch") {
			old_monst.spec_skill = 15;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::DAMAGE].active);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE].gen.strength == 3);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE].gen.odds == 667);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE].gen.dmg == eDamageType::COLD);
		}
		SECTION("Experience Draining Touch") {
			old_monst.spec_skill = 16;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::DRAIN_XP].active);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_XP].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_XP].gen.strength == 150);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_XP].gen.odds == 0);
		}
		SECTION("Icy and Draining Touch") {
			old_monst.spec_skill = 17;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 2);
			REQUIRE(new_monst.abil[eMonstAbil::DAMAGE].active);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE].gen.strength == 3);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE].gen.odds == 667);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE].gen.dmg == eDamageType::COLD);
			REQUIRE(new_monst.abil[eMonstAbil::DRAIN_XP].active);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_XP].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_XP].gen.strength == 150);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_XP].gen.odds == 0);
		}
		SECTION("Slowing Touch") {
			old_monst.spec_skill = 18;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::STUN].active);
			CHECK(new_monst.abil[eMonstAbil::STUN].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::STUN].gen.strength == 2);
			CHECK(new_monst.abil[eMonstAbil::STUN].gen.odds == 667);
			CHECK(new_monst.abil[eMonstAbil::STUN].gen.stat == eStatus::HASTE_SLOW);
		}
		SECTION("Shoots Web") {
			old_monst.spec_skill = 19;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MISSILE_WEB].active);
			CHECK(new_monst.abil[eMonstAbil::MISSILE_WEB].special.extra1 == 4);
			CHECK(new_monst.abil[eMonstAbil::MISSILE_WEB].special.extra2 == 375);
		}
		SECTION("Good Archer") {
			old_monst.spec_skill = 20;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MISSILE].active);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.type == eMonstMissile::RAPID_ARROW);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.pic == 3);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.dice == 8);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.sides == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.skill == 16);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.range == 10);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.odds == 875);
		}
		SECTION("Steals Food") {
			old_monst.spec_skill = 21;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::STEAL_FOOD].active);
			CHECK(new_monst.abil[eMonstAbil::STEAL_FOOD].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::STEAL_FOOD].gen.strength == 10);
			CHECK(new_monst.abil[eMonstAbil::STEAL_FOOD].gen.odds == 667);
		}
		SECTION("Permanent Martyr's Shield") {
			old_monst.spec_skill = 22;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MARTYRS_SHIELD].active);
			CHECK(new_monst.abil[eMonstAbil::MARTYRS_SHIELD].special.extra1 == 1000);
			CHECK(new_monst.abil[eMonstAbil::MARTYRS_SHIELD].special.extra2 == 100);
		}
		SECTION("Paralysis Ray") {
			old_monst.spec_skill = 23;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::STATUS].active);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.type == eMonstGen::RAY);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.strength == 100);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.range == 6);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.odds == 750);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.stat == eStatus::PARALYZED);
		}
		SECTION("Dumbfounding Touch") {
			old_monst.spec_skill = 24;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::STATUS].active);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.strength == 2);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.odds == 1000);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.stat == eStatus::DUMB);
		}
		SECTION("Disease Touch") {
			old_monst.spec_skill = 25;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::STATUS].active);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.strength == 6);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.odds == 667);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.stat == eStatus::DISEASE);
		}
		SECTION("Absorbs Spells") {
			old_monst.spec_skill = 26;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::ABSORB_SPELLS].active);
			CHECK(new_monst.abil[eMonstAbil::ABSORB_SPELLS].special.extra1 == 1000);
			CHECK(new_monst.abil[eMonstAbil::ABSORB_SPELLS].special.extra2 == 3);
		}
		SECTION("Web Touch") {
			old_monst.spec_skill = 27;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::STATUS].active);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.strength == 5);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.odds == 1000);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.stat == eStatus::WEBS);
		}
		SECTION("Sleep Touch") {
			old_monst.spec_skill = 28;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::STATUS].active);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.strength == 6);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.odds == 1000);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.stat == eStatus::ASLEEP);
		}
		SECTION("Paralysis Touch") {
			old_monst.spec_skill = 29;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::STATUS].active);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.strength == 500);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.odds == 0);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.stat == eStatus::PARALYZED);
		}
		SECTION("Petrification Touch") {
			old_monst.spec_skill = 30;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::PETRIFY].active);
			CHECK(new_monst.abil[eMonstAbil::PETRIFY].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::PETRIFY].gen.strength == 25);
			CHECK(new_monst.abil[eMonstAbil::PETRIFY].gen.odds == 0);
		}
		SECTION("Acid Touch") {
			old_monst.spec_skill = 31;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::STATUS].active);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.strength == 2);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.odds == 1000);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.stat == eStatus::ACID);
		}
		SECTION("Breathes Sleep Clouds") {
			old_monst.spec_skill = 32;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::FIELD].active);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.type == eMonstGen::BREATH);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.pic == 0);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.strength == PAT_RAD2);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.range == 8);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.odds == 750);
			CHECK(new_monst.abil[eMonstAbil::FIELD].gen.fld == CLOUD_SLEEP);
		}
		SECTION("Acid Spit") {
			old_monst.spec_skill = 33;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::STATUS].active);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.type == eMonstGen::SPIT);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.pic == 0);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.strength == 6);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.range == 6);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.odds == 500);
			CHECK(new_monst.abil[eMonstAbil::STATUS].gen.stat == eStatus::ACID);
		}
		SECTION("Shoots Spines") {
			old_monst.spec_skill = 34;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::MISSILE].active);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.type == eMonstMissile::SPINE);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.pic == 5);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.dice == 6);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.sides == 7);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.skill == 12);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.range == 9);
			CHECK(new_monst.abil[eMonstAbil::MISSILE].missile.odds == 625);
		}
		SECTION("Death Touch") {
			old_monst.spec_skill = 35;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::KILL].active);
			CHECK(new_monst.abil[eMonstAbil::KILL].gen.type == eMonstGen::TOUCH);
			CHECK(new_monst.abil[eMonstAbil::KILL].gen.strength == 2);
			CHECK(new_monst.abil[eMonstAbil::KILL].gen.odds == 667);
		}
		SECTION("Invulnerable") {
			old_monst.spec_skill = 36;
			new_monst.append(old_monst);
			CHECK(new_monst.abil.size() == 0);
			CHECK(new_monst.invuln);
		}
		SECTION("Guard") {
			old_monst.spec_skill = 37;
			new_monst.append(old_monst);
			CHECK(new_monst.abil.size() == 0);
			CHECK(new_monst.guard);
		}
	}
	SECTION("With secondary abilities") {
		old_monst.radiate_2 = 50;
		SECTION("Radiate Fire Fields") {
			old_monst.radiate_1 = 1;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::RADIATE].active);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.type == WALL_FIRE);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.chance == 50);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.pat == PAT_SQ);
		}
		SECTION("Radiate Ice Fields") {
			old_monst.radiate_1 = 2;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::RADIATE].active);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.type == WALL_ICE);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.chance == 50);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.pat == PAT_SQ);
		}
		SECTION("Radiate Shock Fields") {
			old_monst.radiate_1 = 3;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::RADIATE].active);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.type == WALL_FORCE);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.chance == 50);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.pat == PAT_SQ);
		}
		SECTION("Radiate Antimagic Fields") {
			old_monst.radiate_1 = 4;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::RADIATE].active);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.type == FIELD_ANTIMAGIC);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.chance == 50);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.pat == PAT_SQ);
		}
		SECTION("Radiate Sleep Fields") {
			old_monst.radiate_1 = 5;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::RADIATE].active);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.type == CLOUD_SLEEP);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.chance == 50);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.pat == PAT_SQ);
		}
		SECTION("Radiate Stink Fields") {
			old_monst.radiate_1 = 6;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::RADIATE].active);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.type == CLOUD_STINK);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.chance == 50);
			CHECK(new_monst.abil[eMonstAbil::RADIATE].radiate.pat == PAT_SQ);
		}
		SECTION("Summon 5% chance") {
			old_monst.radiate_1 = 10;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::SUMMON].active);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.type == eMonstSummon::TYPE);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.what == 50);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.min == 1);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.max == 1);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.len == 130);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.chance == 50);
		}
		SECTION("Summon 20% chance") {
			old_monst.radiate_1 = 11;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::SUMMON].active);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.type == eMonstSummon::TYPE);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.what == 50);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.min == 1);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.max == 1);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.len == 130);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.chance == 200);
		}
		SECTION("Summon 50% chance") {
			old_monst.radiate_1 = 12;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::SUMMON].active);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.type == eMonstSummon::TYPE);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.what == 50);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.min == 1);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.max == 1);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.len == 130);
			CHECK(new_monst.abil[eMonstAbil::SUMMON].summon.chance == 500);
		}
		SECTION("Death Triggers Scenario Special") {
			old_monst.radiate_1 = 15;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 1);
			REQUIRE(new_monst.abil[eMonstAbil::DEATH_TRIGGER].active);
			CHECK(new_monst.abil[eMonstAbil::DEATH_TRIGGER].special.extra1 == 50);
		}
	}
	SECTION("With multiple abilities") {
		SECTION("Poison and acid touch") {
			old_monst.poison = 8;
			old_monst.spec_skill = 31;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 2);
			CHECK(new_monst.abil[eMonstAbil::STATUS].active);
			CHECK(new_monst.abil[eMonstAbil::STATUS2].active);
		}
		SECTION("Poison and stun") {
			old_monst.poison = 8;
			old_monst.spec_skill = 18;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 2);
			CHECK(new_monst.abil[eMonstAbil::STATUS2].active);
			CHECK(new_monst.abil[eMonstAbil::STUN].active);
		}
		SECTION("Fire breath and icy+draining touch") {
			old_monst.breath = 8;
			old_monst.breath_type = 1;
			old_monst.spec_skill = 17;
			new_monst.append(old_monst);
			REQUIRE(new_monst.abil.size() == 3);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE].active);
			CHECK(new_monst.abil[eMonstAbil::DAMAGE2].active);
			CHECK(new_monst.abil[eMonstAbil::DRAIN_XP].active);
		}
	}
}
