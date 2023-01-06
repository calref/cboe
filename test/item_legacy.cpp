//
//  item_legacy.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-10-01.
//
//

#include "catch.hpp"
#include "scenario/item.hpp"
#include "oldstructs.hpp"
#include "spell.hpp"
#include "damage.hpp"
#include "race.hpp"

static void set_item_name(legacy::item_record_type& item, const std::string& full, const std::string& name) {
	std::copy_n(full.begin(), std::min<size_t>(25, 1 + full.size()), item.full_name);
	std::copy_n(name.begin(), std::min<size_t>(15, 1 + name.size()), item.name);
}

TEST_CASE("Converting items from legacy scenarios") {
	legacy::item_record_type old_item = {
		5, 7,
		2, 1, 3, 10, 0, 0,
		52, 0, 0, 100, 0,
		0, 500, 80, 12,
		{0,0}, "Test Item", "Item",
		3, 0x3F, 0, 0,
	};
	cItem new_item;
	
	SECTION("Basic info") {
		new_item.import_legacy(old_item);
		CHECK(new_item.variety == eItemType::ARROW);
		CHECK(new_item.item_level == 7);
		CHECK(new_item.awkward == 2);
		CHECK(new_item.bonus == 1);
		CHECK(new_item.protection == 3);
		CHECK(new_item.charges == 10);
		CHECK(new_item.graphic_num == 62);
		CHECK(new_item.type_flag == 100);
		CHECK(new_item.value == 500);
		CHECK(new_item.weight == 80);
		CHECK(new_item.special_class == 12);
		CHECK(new_item.full_name == "Test Item");
		CHECK(new_item.name == "Item");
		CHECK(new_item.treas_class == 3);
		CHECK(new_item.ident);
		CHECK(new_item.property);
		CHECK(new_item.magic);
		CHECK(new_item.cursed);
		CHECK(new_item.unsellable);
		CHECK(new_item.concealed);
	}
	
	SECTION("Weapons") {
		SECTION("Edged") {
			set_item_name(old_item, "Test Sword", "Sword");
			old_item.variety = 2;
			old_item.type = 1;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::TWO_HANDED);
			CHECK(new_item.weap_type == eSkill::EDGED_WEAPONS);
			CHECK(new_item.full_name == "Test Sword");
			CHECK(new_item.name == "Sword");
		}
		SECTION("Bashing") {
			set_item_name(old_item, "Test Mace", "Mace");
			old_item.variety = 1;
			old_item.type = 2;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::ONE_HANDED);
			CHECK(new_item.weap_type == eSkill::BASHING_WEAPONS);
			CHECK(new_item.full_name == "Test Mace");
			CHECK(new_item.name == "Mace");
		}
		SECTION("Pole") {
			set_item_name(old_item, "Test Spear", "Spear");
			old_item.variety = 2;
			old_item.type = 3;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::TWO_HANDED);
			CHECK(new_item.weap_type == eSkill::POLE_WEAPONS);
			CHECK(new_item.full_name == "Test Spear");
			CHECK(new_item.name == "Spear");
		}
		SECTION("Bow") {
			set_item_name(old_item, "Test Bow", "Bow");
			old_item.variety = 4;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::BOW);
			CHECK(new_item.weap_type == eSkill::ARCHERY);
			CHECK(new_item.full_name == "Test Bow");
			CHECK(new_item.name == "Bow");
		}
		SECTION("Crossbow") {
			set_item_name(old_item, "Test Crossbow", "Crossbow");
			old_item.variety = 23;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::CROSSBOW);
			CHECK(new_item.weap_type == eSkill::ARCHERY);
			CHECK(new_item.full_name == "Test Crossbow");
			CHECK(new_item.name == "Crossbow");
		}
		SECTION("Sling") {
			set_item_name(old_item, "Test Sling", "Sling");
			old_item.variety = 25;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::MISSILE_NO_AMMO);
			CHECK(new_item.weap_type == eSkill::ARCHERY);
			CHECK(new_item.missile == 12);
			CHECK(new_item.full_name == "Test Sling");
			CHECK(new_item.name == "Sling");
		}
		SECTION("Arrows") {
			set_item_name(old_item, "Test Arrows", "Arrows");
			old_item.variety = 5;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::ARROW);
			CHECK(new_item.missile == 4);
			CHECK(new_item.full_name == "Test Arrows");
			CHECK(new_item.name == "Arrows");
			old_item.item_properties = 0;
			new_item.import_legacy(old_item);
			CHECK(new_item.missile == 3);
		}
		SECTION("Bolts") {
			set_item_name(old_item, "Test Bolts", "Bolts");
			old_item.variety = 24;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::BOLTS);
			CHECK(new_item.missile == 4);
			CHECK(new_item.full_name == "Test Bolts");
			CHECK(new_item.name == "Bolts");
			old_item.item_properties = 0;
			new_item.import_legacy(old_item);
			CHECK(new_item.missile == 3);
		}
		SECTION("Darts") {
			set_item_name(old_item, "Test Darts", "Darts");
			old_item.variety = 6;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::THROWN_MISSILE);
			CHECK(new_item.weap_type == eSkill::THROWN_MISSILES);
			CHECK(new_item.missile == 1);
			CHECK(new_item.full_name == "Test Darts");
			CHECK(new_item.name == "Darts");
		}
		SECTION("Knives") {
			set_item_name(old_item, "Test Knives", "Knives");
			old_item.variety = 6;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::THROWN_MISSILE);
			CHECK(new_item.weap_type == eSkill::THROWN_MISSILES);
			CHECK(new_item.missile == 10);
			CHECK(new_item.full_name == "Test Knives");
			CHECK(new_item.name == "Knives");
		}
		SECTION("Javelins") {
			set_item_name(old_item, "Test Javelins", "Javelins");
			old_item.variety = 6;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::THROWN_MISSILE);
			CHECK(new_item.weap_type == eSkill::THROWN_MISSILES);
			CHECK(new_item.missile == 5);
			CHECK(new_item.full_name == "Test Javelins");
			CHECK(new_item.name == "Javelins");
		}
		SECTION("Razordisks") {
			set_item_name(old_item, "Test Razordisks", "Razordisks");
			old_item.variety = 6;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::THROWN_MISSILE);
			CHECK(new_item.weap_type == eSkill::THROWN_MISSILES);
			CHECK(new_item.missile == 7);
			CHECK(new_item.full_name == "Test Razordisks");
			CHECK(new_item.name == "Razordisks");
		}
		SECTION("Rocks") {
			set_item_name(old_item, "Test Rocks", "Rocks");
			old_item.variety = 6;
			new_item.import_legacy(old_item);
			CHECK(new_item.variety == eItemType::THROWN_MISSILE);
			CHECK(new_item.weap_type == eSkill::THROWN_MISSILES);
			CHECK(new_item.missile == 12);
			CHECK(new_item.full_name == "Test Rocks");
			CHECK(new_item.name == "Rocks");
		}
	}
	
	SECTION("With abilities") {
		old_item.ability_strength = 120;
		
		// Weapon abilities
		SECTION("Flaming Weapon") {
			old_item.ability = 1;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGING_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.damage == eDamageType::UNBLOCKABLE);
			CHECK(new_item.abil_data.value == int(eDamageType::UNBLOCKABLE));
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Demon Slayer") {
			old_item.ability = 2;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.race == eRace::DEMON);
			CHECK(new_item.abil_data.value == int(eRace::DEMON));
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Undead Slayer") {
			old_item.ability = 3;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.race == eRace::UNDEAD);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Lizard Slayer") {
			old_item.ability = 4;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.race == eRace::REPTILE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Giant Slayer") {
			old_item.ability = 5;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.race == eRace::GIANT);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Mage Slayer") {
			old_item.ability = 6;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.race == eRace::MAGE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Priest Slayer") {
			old_item.ability = 7;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.race == eRace::PRIEST);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Bug Slayer") {
			old_item.ability = 8;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.race == eRace::BUG);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Acidic Weapon") {
			old_item.ability = 9;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::ACID);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Soulsucker") {
			old_item.ability = 10;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SOULSUCKER);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Drain Missiles") {
			old_item.ability = 11;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DRAIN_MISSILES);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Weak Weapon") {
			old_item.ability = 12;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::WEAK_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Causes Fear") {
			old_item.ability = 13;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAUSES_FEAR);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Poisoned Weapon") {
			old_item.ability = 14;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::POISON);
			CHECK_FALSE(new_item.can_use());
		}
		
		// Inherent abilities
		SECTION("Protection") {
			old_item.ability = 30;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.damage == eDamageType::WEAPON);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Full Protection") {
			old_item.ability = 31;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::FULL_PROTECTION);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Fire Protection") {
			old_item.ability = 32;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_strength == 60);
			CHECK(new_item.abil_data.damage == eDamageType::FIRE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Cold Protection") {
			old_item.ability = 33;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_strength == 60);
			CHECK(new_item.abil_data.damage == eDamageType::COLD);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Poison Protection") {
			old_item.ability = 34;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_PROTECTION);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::POISON);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Magic Protection") {
			old_item.ability = 35;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_strength == 60);
			CHECK(new_item.abil_data.damage == eDamageType::MAGIC);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Acid Protection") {
			old_item.ability = 36;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_PROTECTION);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::ACID);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Skill") {
			old_item.ability = 37;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SKILL);
			CHECK(new_item.abil_strength == 7);
			CHECK(new_item.abil_data.value == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Strength") {
			old_item.ability = 38;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::BOOST_STAT);
			CHECK(new_item.abil_strength == 1);
			CHECK(new_item.abil_data.skill == eSkill::STRENGTH);
			CHECK(new_item.desc == "Original ability strength was 120");
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Dexterity") {
			old_item.ability = 39;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::BOOST_STAT);
			CHECK(new_item.abil_strength == 1);
			CHECK(new_item.abil_data.skill == eSkill::DEXTERITY);
			CHECK(new_item.desc == "Original ability strength was 120");
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Intelligence") {
			old_item.ability = 40;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::BOOST_STAT);
			CHECK(new_item.abil_strength == 1);
			CHECK(new_item.abil_data.skill == eSkill::INTELLIGENCE);
			CHECK(new_item.desc == "Original ability strength was 120");
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Accuracy") {
			old_item.ability = 41;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::ACCURACY);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Thieving") {
			old_item.ability = 42;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::THIEVING);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Giant Strength") {
			old_item.ability = 43;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::GIANT_STRENGTH);
			CHECK(new_item.abil_strength == 7);
			CHECK(new_item.abil_data.value == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Lighter Object") {
			old_item.ability = 44;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::LIGHTER_OBJECT);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Heavier Object") {
			old_item.ability = 45;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::HEAVIER_OBJECT);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Occasional Bless") {
			old_item.ability = 46;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::OCCASIONAL_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::BLESS_CURSE);
			CHECK(new_item.magic_use_type == eItemUse::HELP_ONE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Occasional Haste") {
			old_item.ability = 47;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::OCCASIONAL_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::HASTE_SLOW);
			CHECK(new_item.magic_use_type == eItemUse::HELP_ONE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Life Saving") {
			old_item.ability = 48;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::LIFE_SAVING);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Protect from Petrify") {
			old_item.ability = 49;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::PROTECT_FROM_PETRIFY);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Regenerate") {
			old_item.ability = 50;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::REGENERATE);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Poison Augment") {
			old_item.ability = 51;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::POISON_AUGMENT);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Disease Party") {
			old_item.ability = 52;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::OCCASIONAL_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::DISEASE);
			CHECK(new_item.magic_use_type == eItemUse::HARM_ALL);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Will") {
			old_item.ability = 53;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::WILL);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Free Action") {
			old_item.ability = 54;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::FREE_ACTION);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Speed") {
			old_item.ability = 55;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SPEED);
			CHECK(new_item.abil_strength == 18);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Slow Wearer") {
			old_item.ability = 56;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLOW_WEARER);
			CHECK(new_item.abil_strength == 24);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Protection from Undead") {
			old_item.ability = 57;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_strength == 60);
			CHECK(new_item.abil_data.damage == eDamageType::UNDEAD);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Protection from Demons") {
			old_item.ability = 58;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_strength == 60);
			CHECK(new_item.abil_data.damage == eDamageType::DEMON);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Protection from Humanoids") {
			old_item.ability = 59;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::PROTECT_FROM_SPECIES);
			CHECK(new_item.abil_strength == 60);
			CHECK(new_item.abil_data.race == eRace::HUMANOID);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Protection from Reptiles") {
			old_item.ability = 60;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::PROTECT_FROM_SPECIES);
			CHECK(new_item.abil_strength == 60);
			CHECK(new_item.abil_data.race == eRace::REPTILE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Protection from Giants") {
			old_item.ability = 61;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::PROTECT_FROM_SPECIES);
			CHECK(new_item.abil_strength == 60);
			CHECK(new_item.abil_data.race == eRace::GIANT);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Protection from Disease") {
			old_item.ability = 62;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_PROTECTION);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::DISEASE);
			CHECK_FALSE(new_item.can_use());
		}
		
		// Non-spell usable abilities
		SECTION("Poison Weapon") {
			old_item.ability = 70;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::POISON_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK_FALSE(new_item.use_magic());
		}
		SECTION("Curse/Bless User") {
			old_item.ability = 71;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::BLESS_CURSE);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Cure/Cause Poison") {
			old_item.ability = 72;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::POISON);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Speed/Slow User") {
			old_item.ability = 73;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::HASTE_SLOW);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Add/Lose Invulnerability") {
			old_item.ability = 74;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::INVULNERABLE);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Add/Lose Magic Resistance") {
			old_item.ability = 75;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::MAGIC_RESISTANCE);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Add/Lose Web") {
			old_item.ability = 76;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::WEBS);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Cure/Cause Disease") {
			old_item.ability = 77;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::DISEASE);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Add/Lose Sanctuary") {
			old_item.ability = 78;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::INVISIBLE);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Cure/Cause Dumbfounding") {
			old_item.ability = 79;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::DUMB);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Add/Lose Martyr's Shield") {
			old_item.ability = 80;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::MARTYRS_SHIELD);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Cure/Cause Sleep") {
			old_item.ability = 81;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::ASLEEP);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Cure/Cause Paralysis") {
			old_item.ability = 82;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::PARALYZED);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Cure/Cause Acid") {
			old_item.ability = 83;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.status == eStatus::ACID);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Bliss") {
			old_item.ability = 84;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::BLISS_DOOM);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.abil_harms());
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Add/Lose Experience") {
			old_item.ability = 85;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_EXPERIENCE);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Add/Lose Skill Points") {
			old_item.ability = 86;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_SKILL_POINTS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Add/Lose Health") {
			old_item.ability = 87;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_HEALTH);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Add/Lose Spell Points") {
			old_item.ability = 88;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_SPELL_POINTS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Doom") {
			old_item.ability = 89;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::BLISS_DOOM);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
			CHECK(new_item.abil_harms());
		}
		SECTION("Light") {
			old_item.ability = 90;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::LIGHT);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK_FALSE(new_item.use_magic());
			CHECK_FALSE(new_item.abil_harms());
		}
		SECTION("Stealth") {
			old_item.ability = 91;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_PARTY_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.party == ePartyStatus::STEALTH);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
			CHECK_FALSE(new_item.abil_harms());
		}
		SECTION("Firewalk") {
			old_item.ability = 92;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_PARTY_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.party == ePartyStatus::FIREWALK);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
			CHECK_FALSE(new_item.abil_harms());
		}
		SECTION("Flying") {
			old_item.ability = 93;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_PARTY_STATUS);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.party == ePartyStatus::FLIGHT);
			CHECK_FALSE(new_item.use_in_town());
			CHECK_FALSE(new_item.use_in_combat());
			CHECK(new_item.use_outdoors());
			CHECK(new_item.use_magic());
			CHECK_FALSE(new_item.abil_harms());
		}
		SECTION("Major Healing") {
			old_item.ability = 94;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::HEALTH_POISON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK(new_item.use_outdoors());
			CHECK(new_item.use_magic());
			CHECK_FALSE(new_item.abil_harms());
		}
		
		// Spell usable abilities
		SECTION("Flame") {
			old_item.ability = 110;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
				CHECK(new_item.abil_data.spell == eSpell::FLAME);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Fireball") {
			old_item.ability = 111;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::FIREBALL);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Firestorm") {
			old_item.ability = 112;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::FIRESTORM);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Kill") {
			old_item.ability = 113;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::KILL);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Ice Bolt") {
			old_item.ability = 114;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::ICE_BOLT);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Slow") {
			old_item.ability = 115;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::SLOW);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Shockwave") {
			old_item.ability = 116;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::SHOCKWAVE);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Dispel Undead") {
			old_item.ability = 117;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::DISPEL_UNDEAD);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Dispel Spirit") {
			old_item.ability = 118;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::RAVAGE_SPIRIT);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Summoning") {
			old_item.ability = 119;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SUMMONING);
			CHECK(new_item.abil_strength == 50);
			CHECK(new_item.abil_data.value == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Mass Summoning") {
			old_item.ability = 120;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::MASS_SUMMONING);
			CHECK(new_item.abil_strength == 6);
			CHECK(new_item.abil_data.value == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Acid Spray") {
			old_item.ability = 121;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::ACID_SPRAY);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Stinking Cloud") {
			old_item.ability = 122;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::FOUL_VAPOR);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Sleep Field") {
			old_item.ability = 123;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::CLOUD_SLEEP);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Venom") {
			old_item.ability = 124;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::POISON);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Shockstorm") {
			old_item.ability = 125;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::SHOCKSTORM);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Paralysis") {
			old_item.ability = 126;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::PARALYZE_BEAM);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Web Spell") {
			old_item.ability = 127;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::GOO_BOMB);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Strengthen Target") {
			old_item.ability = 128;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::STRENGTHEN_TARGET);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Quickfire") {
			old_item.ability = 129;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::QUICKFIRE);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Mass Charm") {
			old_item.ability = 130;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::CHARM_MASS);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Magic Map") {
			old_item.ability = 131;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::MAGIC_MAP);
			CHECK(new_item.use_in_town());
			CHECK_FALSE(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Dispel Barrier") {
			old_item.ability = 132;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::DISPEL_BARRIER);
			CHECK(new_item.use_in_town());
			CHECK_FALSE(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Ice Wall") {
			old_item.ability = 133;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::WALL_ICE_BALL);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Charm") {
			old_item.ability = 134;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.spell == eSpell::CHARM_FOE);
			CHECK_FALSE(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		SECTION("Antimagic Cloud") {
			old_item.ability = 135;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_strength == 241);
			CHECK(new_item.abil_data.spell == eSpell::ANTIMAGIC);
			CHECK(new_item.use_in_town());
			CHECK(new_item.use_in_combat());
			CHECK_FALSE(new_item.use_outdoors());
			CHECK(new_item.use_magic());
		}
		
		// Reagent abilities
		SECTION("Holly/Toadstool") {
			old_item.ability = 150;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::HOLLY);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Comfrey Root") {
			old_item.ability = 151;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::COMFREY);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Glowing Nettle") {
			old_item.ability = 152;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::NETTLE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Crypt Shroom/Wormgrass") {
			old_item.ability = 153;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::WORMGRASS);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Asptongue Mold") {
			old_item.ability = 154;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::ASPTONGUE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Ember Flowers") {
			old_item.ability = 155;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::EMBERF);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Graymold") {
			old_item.ability = 156;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::GRAYMOLD);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Mandrake Root") {
			old_item.ability = 157;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::MANDRAKE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Sapphire") {
			old_item.ability = 158;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SAPPHIRE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Smoky Crystal") {
			old_item.ability = 159;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SMOKY_CRYSTAL);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Resurrection Balm") {
			old_item.ability = 160;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::RESURRECTION_BALM);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Lockpicks") {
			old_item.ability = 161;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::LOCKPICKS);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
		
		// Missile Abilities
		SECTION("Returning Missile") {
			old_item.ability = 170;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::RETURNING_MISSILE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Lightning Missile") {
			old_item.ability = 171;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGING_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.damage == eDamageType::FIRE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Exploding Missile") {
			old_item.ability = 172;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::EXPLODING_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK(new_item.abil_data.damage == eDamageType::FIRE);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Acid Missile") {
			old_item.ability = 173;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_WEAPON);
			CHECK(new_item.abil_strength == 240);
			CHECK(new_item.abil_data.status == eStatus::ACID);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Slay Undead") {
			old_item.ability = 174;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_strength == 123);
			CHECK(new_item.abil_data.race == eRace::UNDEAD);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Slay Demon") {
			old_item.ability = 175;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_strength == 123);
			CHECK(new_item.abil_data.race == eRace::DEMON);
			CHECK_FALSE(new_item.can_use());
		}
		SECTION("Heal Target") {
			old_item.ability = 176;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::HEALING_WEAPON);
			CHECK(new_item.abil_strength == 120);
			CHECK_FALSE(new_item.can_use());
		}
	}
}
