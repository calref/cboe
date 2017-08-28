//
//  init.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-10-04.
//
//

#include "catch.hpp"
#include "scenario.hpp"
#include "creature.hpp"
#include "population.hpp"
#include "pc.hpp"
#include "party.hpp"

TEST_CASE("Initialization sanity test for terrain") {
	cTerrain ter;
	CHECK(ter.name.empty());
	CHECK(ter.picture == 0);
	CHECK(ter.blockage == eTerObstruct::CLEAR);
	CHECK(ter.flag1 == -1);
	CHECK(ter.flag2 == 0);
	CHECK(ter.flag3 == 0);
	CHECK(ter.special == eTerSpec::NONE);
	CHECK(ter.trans_to_what == 0);
	CHECK_FALSE(ter.fly_over);
	CHECK_FALSE(ter.boat_over);
	CHECK_FALSE(ter.block_horse);
	CHECK_FALSE(ter.is_archetype);
	CHECK(ter.light_radius == 0);
	CHECK(ter.step_sound == eStepSnd::STEP);
	CHECK(ter.shortcut_key == 0);
	CHECK(ter.obj_num == 0);
	CHECK(ter.ground_type == 0);
	CHECK(ter.trim_type == eTrimType::NONE);
	CHECK(ter.trim_ter == 0);
	CHECK(ter.frill_for == -1);
	CHECK(ter.frill_chance == 0);
	CHECK(ter.combat_arena == 0);
	CHECK(ter.obj_pos == loc(0,0));
	CHECK(ter.obj_size == loc(0,0));
	CHECK(ter.map_pic == -1);
}

TEST_CASE("Initialization sanity test for monster") {
	cCreature who;
	SECTION("Monster base class") {
		cMonster& monst = who;
		CHECK(monst.level == 0);
		CHECK(monst.m_name.empty());
		CHECK(monst.m_health == 0);
		CHECK(monst.armor == 0);
		CHECK(monst.skill == 0);
		CHECK(monst.a[0].dice == 0);
		CHECK(monst.a[0].sides == 0);
		CHECK(monst.a[0].type == eMonstMelee::SWING);
		CHECK(monst.a[1].dice == 0);
		CHECK(monst.a[1].sides == 0);
		CHECK(monst.a[1].type == eMonstMelee::SWING);
		CHECK(monst.a[2].dice == 0);
		CHECK(monst.a[2].sides == 0);
		CHECK(monst.a[2].type == eMonstMelee::SWING);
		CHECK(monst.m_type == eRace::HUMAN);
		CHECK(monst.speed == 4);
		CHECK(monst.mu == 0);
		CHECK(monst.cl == 0);
		CHECK(monst.treasure == 0);
		CHECK(monst.abil.empty());
		CHECK(monst.corpse_item == 0);
		CHECK(monst.corpse_item_chance == 0);
		CHECK(monst.resist.size() == 10);
		CHECK(monst.resist[eDamageType::WEAPON] == 100);
		CHECK(monst.resist[eDamageType::FIRE] == 100);
		CHECK(monst.resist[eDamageType::POISON] == 100);
		CHECK(monst.resist[eDamageType::COLD] == 100);
		CHECK(monst.resist[eDamageType::MAGIC] == 100);
		CHECK(monst.resist[eDamageType::UNBLOCKABLE] == 100);
		CHECK(monst.resist[eDamageType::UNDEAD] == 100);
		CHECK(monst.resist[eDamageType::DEMON] == 100);
		CHECK(monst.resist[eDamageType::MARKED] == 100);
		CHECK_FALSE(monst.mindless);
		CHECK_FALSE(monst.invuln);
		CHECK_FALSE(monst.invisible);
		CHECK_FALSE(monst.guard);
		CHECK_FALSE(monst.amorphous);
		CHECK(monst.x_width == 1);
		CHECK(monst.y_width == 1);
		CHECK(monst.default_attitude == eAttitude::DOCILE);
		CHECK(monst.summon_type == 0);
		CHECK(monst.default_facial_pic == 0);
		CHECK(monst.picture_num == 149);
		CHECK(monst.ambient_sound == -1);
		CHECK(monst.see_spec == -1);
	}
	SECTION("Townsperson base class") {
		cTownperson& dude = who;
		CHECK(dude.number == 0);
		CHECK(dude.start_attitude == eAttitude::DOCILE);
		CHECK(dude.start_loc == loc(80,80));
		CHECK(dude.mobility == 1);
		CHECK(dude.time_flag == eMonstTime::ALWAYS);
		CHECK(dude.spec1 == -1);
		CHECK(dude.spec2 == -1);
		CHECK(dude.spec_enc_code == 0);
		CHECK(dude.time_code == 0);
		CHECK(dude.monster_time == 0);
		CHECK(dude.personality == -1);
		CHECK(dude.special_on_kill == -1);
		CHECK(dude.special_on_talk == -1);
		CHECK(dude.facial_pic == -1);
	}
	SECTION("Living base class") {
		iLiving& base = who;
		CHECK(base.status.empty());
		CHECK(base.ap == 0);
		CHECK(base.direction == DIR_HERE);
		CHECK(base.marked_damage == 0);
	}
	SECTION("Main creature class") {
		CHECK(who.active == 0);
		CHECK(who.attitude == eAttitude::DOCILE);
		CHECK(who.cur_loc == loc(80,80));
		CHECK(who.summon_time == 0);
		CHECK(who.target == 6);
		CHECK(who.targ_loc == loc(80,80));
		CHECK(who.health == 0);
		CHECK(who.mp == 0);
		CHECK(who.max_mp == 0);
		CHECK(who.morale == 0);
		CHECK(who.m_morale == 0);
	}
}

TEST_CASE("Construction sanity test for monster") {
	// First, build a base monster.
	cMonster monst;
	monst.default_facial_pic = 12;
	monst.default_attitude = eAttitude::HOSTILE_A;
	
	cTownperson dude({10,10}, 7, monst); // This is here because the population sections also need it
	SECTION("Into townsperson") {
		CHECK(dude.number == 7);
		CHECK(dude.start_loc == loc(10,10));
		CHECK(dude.facial_pic == 12);
		CHECK(dude.start_attitude == eAttitude::HOSTILE_A);
		return; // Don't need to bother adding the extra monster/townperson attributes for this test
	}
	
	cPopulation pop;
	SECTION("Population init sanity test") {
		CHECK(pop.size() == 0);
		CHECK(pop.which_town == 200);
		CHECK_FALSE(pop.hostile);
		return; // Don't need to bother adding the extra monster/townperson attributes for this test
	}
	
	monst.addAbil(eMonstAbilTemplate::TOUCH_STEAL_GOLD);
	monst.addAttack(2, 8, eMonstMelee::STAB);
	monst.resist[eDamageType::FIRE] = 25;
	monst.level = 5;
	monst.m_name = "The Grinch";
	monst.m_health = 32;
	monst.armor = 9;
	monst.skill = 1;
	monst.m_type = eRace::GOBLIN;
	monst.invisible = true;
	monst.mu = 1;
	
	dude.time_flag = eMonstTime::APPEAR_AFTER_CHOP;
	dude.personality = 8;
	dude.start_attitude = eAttitude::HOSTILE_B;
	
	SECTION("Into population (normal mode)") {
		pop.assign(0, dude, monst, false, 1);
		REQUIRE(pop.size() == 1);
		CHECK(pop[0].active == 1);
		CHECK(pop[0].picture_num == 0);
		CHECK(pop[0].m_health == 32);
		CHECK(pop[0].health == 32);
		CHECK(pop[0].max_mp == 60);
		CHECK(pop[0].mp == 60);
		CHECK(pop[0].m_morale == 50);
		CHECK(pop[0].morale == 50);
		CHECK(pop[0].ap == 0);
		CHECK(pop[0].direction == DIR_HERE);
		CHECK(pop[0].status.size() == 0);
		CHECK(pop[0].attitude == eAttitude::HOSTILE_B);
		CHECK(pop[0].cur_loc == loc(10,10));
		// Townsperson stuff
		CHECK(pop[0].time_flag == eMonstTime::APPEAR_AFTER_CHOP);
		CHECK(pop[0].personality == 8);
		// Monster stuff
		CHECK(pop[0].m_name == "The Grinch");
		CHECK(pop[0].level == 5);
		CHECK(pop[0].armor == 9);
		CHECK(pop[0].skill == 1);
		CHECK(pop[0].speed == 4);
		CHECK(pop[0].m_type == eRace::GOBLIN);
		CHECK(pop[0].resist[eDamageType::FIRE] == 25);
		CHECK(pop[0].invisible);
		CHECK(pop[0].mu == 1);
		REQUIRE(pop[0].abil.size() == 1);
		CHECK(pop[0].abil[eMonstAbil::STEAL_GOLD].active);
		// No need to bother checking the attributes of the ability, just make sure it's there
	}
	SECTION("Into population (easy mode)") {
		pop.assign(0, dude, monst, true, 1);
		REQUIRE(pop.size() == 1);
		CHECK(pop[0].m_health == 16);
		CHECK(pop[0].health == 16);
	}
	SECTION("Into population (with difficulty adjust") {
		pop.assign(0, dude, monst, false, 10);
		REQUIRE(pop.size() == 1);
		CHECK(pop[0].m_health == 320);
		CHECK(pop[0].health == 320);
	}
	SECTION("Into population (high-level monster)") {
		monst.level = 50;
		pop.assign(0, dude, monst, false, 1);
		REQUIRE(pop.size() == 1);
		CHECK(pop[0].m_morale == 800);
		CHECK(pop[0].morale == 800);
	}
}

TEST_CASE("Construction sanity test for player character") {
	cParty party;
	cPlayer pc(party);
	SECTION("Living base class") {
		iLiving& base = pc;
		CHECK(base.status.empty());
		CHECK(base.ap == 0);
		CHECK(base.direction == DIR_N);
		CHECK(base.marked_damage == 0);
	}
	SECTION("Main player class") {
		CHECK(pc.main_status == eMainStatus::ABSENT);
		CHECK(pc.name == "\n");
		CHECK(pc.skills.size() == 3);
		CHECK(pc.skills[eSkill::STRENGTH] == 1);
		CHECK(pc.skills[eSkill::DEXTERITY] == 1);
		CHECK(pc.skills[eSkill::INTELLIGENCE] == 1);
		CHECK(pc.max_health == 6);
		CHECK(pc.cur_health == 6);
		CHECK(pc.max_sp == 0);
		CHECK(pc.cur_sp == 0);
		CHECK(pc.experience == 0);
		CHECK(pc.skill_pts == 65);
		CHECK(pc.level == 1);
		CHECK(pc.items[0].variety == eItemType::NO_ITEM);
		CHECK_FALSE(pc.equip.any());
		CHECK(pc.priest_spells == cPlayer::basic_spells);
		CHECK(pc.mage_spells == cPlayer::basic_spells);
		CHECK(pc.which_graphic == 0);
		using weap_slot_t = decltype(pc.weap_poisoned.slot);
		CHECK(pc.weap_poisoned.slot == std::numeric_limits<weap_slot_t>::max());
		CHECK(pc.traits.empty());
		CHECK(pc.race == eRace::HUMAN);
		// Skip unique_id since it's non-deterministic
		CHECK(pc.last_cast.empty());
		CHECK(pc.combat_pos == loc(-1,-1));
		CHECK(pc.parry == 0);
		CHECK(pc.last_attacked == nullptr);
	}
	SECTION("Player spells") {
		// This is more just a bitset sanity test
		for(int i = 0; i < 62; i++) {
			CHECK(pc.priest_spells[i] == (i < 30));
			CHECK(pc.mage_spells[i] == (i < 30));
		}
	}
}
