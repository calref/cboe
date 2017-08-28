//
//  item_legacy.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-10-01.
//
//

#include "catch.hpp"
#include "item.hpp"
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
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eDamageType::UNBLOCKABLE));
		}
		SECTION("Demon Slayer") {
			old_item.ability = 2;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eRace::DEMON));
		}
		SECTION("Undead Slayer") {
			old_item.ability = 3;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eRace::UNDEAD));
		}
		SECTION("Lizard Slayer") {
			old_item.ability = 4;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eRace::REPTILE));
		}
		SECTION("Giant Slayer") {
			old_item.ability = 5;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eRace::GIANT));
		}
		SECTION("Mage Slayer") {
			old_item.ability = 6;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eRace::MAGE));
		}
		SECTION("Priest Slayer") {
			old_item.ability = 7;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eRace::PRIEST));
		}
		SECTION("Bug Slayer") {
			old_item.ability = 8;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eRace::BUG));
		}
		SECTION("Acidic Weapon") {
			old_item.ability = 9;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::ACID));
		}
		SECTION("Soulsucker") {
			old_item.ability = 10;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SOULSUCKER);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Drain Missiles") {
			old_item.ability = 11;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DRAIN_MISSILES);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Weak Weapon") {
			old_item.ability = 12;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::WEAK_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Causes Fear") {
			old_item.ability = 13;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAUSES_FEAR);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Poisoned Weapon") {
			old_item.ability = 14;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::POISON));
		}
		
		// Inherent abilities
		SECTION("Protection") {
			old_item.ability = 30;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eDamageType::WEAPON));
		}
		SECTION("Full Protection") {
			old_item.ability = 31;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::FULL_PROTECTION);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Fire Protection") {
			old_item.ability = 32;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_data[0] == 60);
			CHECK(new_item.abil_data[1] == int(eDamageType::FIRE));
		}
		SECTION("Cold Protection") {
			old_item.ability = 33;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_data[0] == 60);
			CHECK(new_item.abil_data[1] == int(eDamageType::COLD));
		}
		SECTION("Poison Protection") {
			old_item.ability = 34;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_PROTECTION);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::POISON));
		}
		SECTION("Magic Protection") {
			old_item.ability = 35;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_data[0] == 60);
			CHECK(new_item.abil_data[1] == int(eDamageType::MAGIC));
		}
		SECTION("Acid Protection") {
			old_item.ability = 36;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_PROTECTION);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::ACID));
		}
		SECTION("Skill") {
			old_item.ability = 37;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SKILL);
			CHECK(new_item.abil_data[0] == 7);
			CHECK(new_item.abil_data[1] == 120);
		}
		SECTION("Strength") {
			old_item.ability = 38;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::BOOST_STAT);
			CHECK(new_item.abil_data[0] == 1);
			CHECK(new_item.abil_data[1] == int(eSkill::STRENGTH));
			CHECK(new_item.desc == "Original ability strength was 120");
		}
		SECTION("Dexterity") {
			old_item.ability = 39;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::BOOST_STAT);
			CHECK(new_item.abil_data[0] == 1);
			CHECK(new_item.abil_data[1] == int(eSkill::DEXTERITY));
			CHECK(new_item.desc == "Original ability strength was 120");
		}
		SECTION("Intelligence") {
			old_item.ability = 40;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::BOOST_STAT);
			CHECK(new_item.abil_data[0] == 1);
			CHECK(new_item.abil_data[1] == int(eSkill::INTELLIGENCE));
			CHECK(new_item.desc == "Original ability strength was 120");
		}
		SECTION("Accuracy") {
			old_item.ability = 41;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::ACCURACY);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Thieving") {
			old_item.ability = 42;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::THIEVING);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Giant Strength") {
			old_item.ability = 43;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::GIANT_STRENGTH);
			CHECK(new_item.abil_data[0] == 7);
			CHECK(new_item.abil_data[1] == 120);
		}
		SECTION("Lighter Object") {
			old_item.ability = 44;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::LIGHTER_OBJECT);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Heavier Object") {
			old_item.ability = 45;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::HEAVIER_OBJECT);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Occasional Bless") {
			old_item.ability = 46;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::OCCASIONAL_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::BLESS_CURSE));
			CHECK(new_item.magic_use_type == eItemUse::HELP_ONE);
		}
		SECTION("Occasional Haste") {
			old_item.ability = 47;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::OCCASIONAL_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::HASTE_SLOW));
			CHECK(new_item.magic_use_type == eItemUse::HELP_ONE);
		}
		SECTION("Life Saving") {
			old_item.ability = 48;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::LIFE_SAVING);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Protect from Petrify") {
			old_item.ability = 49;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::PROTECT_FROM_PETRIFY);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Regenerate") {
			old_item.ability = 50;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::REGENERATE);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Poison Augment") {
			old_item.ability = 51;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::POISON_AUGMENT);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Disease Party") {
			old_item.ability = 52;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::OCCASIONAL_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::DISEASE));
			CHECK(new_item.magic_use_type == eItemUse::HARM_ALL);
		}
		SECTION("Will") {
			old_item.ability = 53;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::WILL);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Free Action") {
			old_item.ability = 54;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::FREE_ACTION);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Speed") {
			old_item.ability = 55;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SPEED);
			CHECK(new_item.abil_data[0] == 18);
		}
		SECTION("Slow Wearer") {
			old_item.ability = 56;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLOW_WEARER);
			CHECK(new_item.abil_data[0] == 24);
		}
		SECTION("Protection from Undead") {
			old_item.ability = 57;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_data[0] == 60);
			CHECK(new_item.abil_data[1] == int(eDamageType::UNDEAD));
		}
		SECTION("Protection from Demons") {
			old_item.ability = 58;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGE_PROTECTION);
			CHECK(new_item.abil_data[0] == 60);
			CHECK(new_item.abil_data[1] == int(eDamageType::DEMON));
		}
		SECTION("Protection from Humanoids") {
			old_item.ability = 59;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::PROTECT_FROM_SPECIES);
			CHECK(new_item.abil_data[0] == 60);
			CHECK(new_item.abil_data[1] == int(eRace::HUMANOID));
		}
		SECTION("Protection from Reptiles") {
			old_item.ability = 60;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::PROTECT_FROM_SPECIES);
			CHECK(new_item.abil_data[0] == 60);
			CHECK(new_item.abil_data[1] == int(eRace::REPTILE));
		}
		SECTION("Protection from Giants") {
			old_item.ability = 61;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::PROTECT_FROM_SPECIES);
			CHECK(new_item.abil_data[0] == 60);
			CHECK(new_item.abil_data[1] == int(eRace::GIANT));
		}
		SECTION("Protection from Disease") {
			old_item.ability = 62;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_PROTECTION);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::DISEASE));
		}
		
		// Non-spell usable abilities
		SECTION("Poison Weapon") {
			old_item.ability = 70;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::POISON_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Curse/Bless User") {
			old_item.ability = 71;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::BLESS_CURSE));
		}
		SECTION("Cure/Cause Poison") {
			old_item.ability = 72;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::POISON));
		}
		SECTION("Speed/Slow User") {
			old_item.ability = 73;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::HASTE_SLOW));
		}
		SECTION("Add/Lose Invulnerability") {
			old_item.ability = 74;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::INVULNERABLE));
		}
		SECTION("Add/Lose Magic Resistance") {
			old_item.ability = 75;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::MAGIC_RESISTANCE));
		}
		SECTION("Add/Lose Web") {
			old_item.ability = 76;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::WEBS));
		}
		SECTION("Cure/Cause Disease") {
			old_item.ability = 77;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::DISEASE));
		}
		SECTION("Add/Lose Sanctuary") {
			old_item.ability = 78;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::INVISIBLE));
		}
		SECTION("Cure/Cause Dumbfounding") {
			old_item.ability = 79;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::DUMB));
		}
		SECTION("Add/Lose Martyr's Shield") {
			old_item.ability = 80;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::MARTYRS_SHIELD));
		}
		SECTION("Cure/Cause Sleep") {
			old_item.ability = 81;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::ASLEEP));
		}
		SECTION("Cure/Cause Paralysis") {
			old_item.ability = 82;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::PARALYZED));
		}
		SECTION("Cure/Cause Acid") {
			old_item.ability = 83;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eStatus::ACID));
		}
		SECTION("Bliss") {
			old_item.ability = 84;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::BLISS_DOOM);
			CHECK(new_item.abil_data[0] == 120);
			CHECK_FALSE(new_item.abil_harms());
		}
		SECTION("Add/Lose Experience") {
			old_item.ability = 85;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_EXPERIENCE);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Add/Lose Skill Points") {
			old_item.ability = 86;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_SKILL_POINTS);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Add/Lose Health") {
			old_item.ability = 87;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_HEALTH);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Add/Lose Spell Points") {
			old_item.ability = 88;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_SPELL_POINTS);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Doom") {
			old_item.ability = 89;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::BLISS_DOOM);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_harms());
		}
		SECTION("Light") {
			old_item.ability = 90;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::LIGHT);
			CHECK(new_item.abil_data[0] == 120);
			CHECK_FALSE(new_item.abil_harms());
		}
		SECTION("Stealth") {
			old_item.ability = 91;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_PARTY_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(ePartyStatus::STEALTH));
			CHECK_FALSE(new_item.abil_harms());
		}
		SECTION("Firewalk") {
			old_item.ability = 92;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_PARTY_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(ePartyStatus::FIREWALK));
			CHECK_FALSE(new_item.abil_harms());
		}
		SECTION("Flying") {
			old_item.ability = 93;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::AFFECT_PARTY_STATUS);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(ePartyStatus::FLIGHT));
			CHECK_FALSE(new_item.abil_harms());
		}
		SECTION("Major Healing") {
			old_item.ability = 94;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::HEALTH_POISON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK_FALSE(new_item.abil_harms());
		}
		
		// Spell usable abilities
		SECTION("Flame") {
			old_item.ability = 110;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::FLAME));
		}
		SECTION("Fireball") {
			old_item.ability = 111;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::FIREBALL));
		}
		SECTION("Firestorem") {
			old_item.ability = 112;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::FIRESTORM));
		}
		SECTION("Kill") {
			old_item.ability = 113;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::KILL));
		}
		SECTION("Ice Bolt") {
			old_item.ability = 114;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::ICE_BOLT));
		}
		SECTION("Slow") {
			old_item.ability = 115;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::SLOW));
		}
		SECTION("Shockwave") {
			old_item.ability = 116;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::SHOCKWAVE));
		}
		SECTION("Dispel Undead") {
			old_item.ability = 117;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::DISPEL_UNDEAD));
		}
		SECTION("Dispel Spirit") {
			old_item.ability = 118;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::RAVAGE_SPIRIT));
		}
		SECTION("Summoning") {
			old_item.ability = 119;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SUMMONING);
			CHECK(new_item.abil_data[0] == 50);
			CHECK(new_item.abil_data[1] == 120);
		}
		SECTION("Mass Summoning") {
			old_item.ability = 120;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::MASS_SUMMONING);
			CHECK(new_item.abil_data[0] == 6);
			CHECK(new_item.abil_data[1] == 120);
		}
		SECTION("Acid Spray") {
			old_item.ability = 121;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::ACID_SPRAY));
		}
		SECTION("Stinking Cloud") {
			old_item.ability = 122;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::FOUL_VAPOR));
		}
		SECTION("Sleep Field") {
			old_item.ability = 123;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::CLOUD_SLEEP));
		}
		SECTION("Venom") {
			old_item.ability = 124;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::POISON));
		}
		SECTION("Shockstorm") {
			old_item.ability = 125;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::SHOCKSTORM));
		}
		SECTION("Paralysis") {
			old_item.ability = 126;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::PARALYZE_BEAM));
		}
		SECTION("Web Spell") {
			old_item.ability = 127;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::GOO_BOMB));
		}
		SECTION("Strengthen Target") {
			old_item.ability = 128;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::STRENGTHEN_TARGET));
		}
		SECTION("Quickfire") {
			old_item.ability = 129;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::QUICKFIRE);
			CHECK(new_item.abil_data[0] == 120);
		}
		SECTION("Mass Charm") {
			old_item.ability = 130;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::CHARM_MASS));
		}
		SECTION("Magic Map") {
			old_item.ability = 131;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::MAGIC_MAP));
		}
		SECTION("Dispel Barrier") {
			old_item.ability = 132;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::DISPEL_BARRIER));
		}
		SECTION("Ice Wall") {
			old_item.ability = 133;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::WALL_ICE_BALL));
		}
		SECTION("Charm") {
			old_item.ability = 134;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eSpell::CHARM_FOE));
		}
		SECTION("Antimagic Cloud") {
			old_item.ability = 135;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::CAST_SPELL);
			CHECK(new_item.abil_data[0] == 241);
			CHECK(new_item.abil_data[1] == int(eSpell::ANTIMAGIC));
		}
		
		// Reagent abilities
		SECTION("Holly/Toadstool") {
			old_item.ability = 150;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::HOLLY);
		}
		SECTION("Comfrey Root") {
			old_item.ability = 151;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::COMFREY);
		}
		SECTION("Glowing Nettle") {
			old_item.ability = 152;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::NETTLE);
		}
		SECTION("Crypt Shroom/Wormgrass") {
			old_item.ability = 153;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::WORMGRASS);
		}
		SECTION("Asptongue Mold") {
			old_item.ability = 154;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::ASPTONGUE);
		}
		SECTION("Ember Flowers") {
			old_item.ability = 155;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::EMBERF);
		}
		SECTION("Graymold") {
			old_item.ability = 156;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::GRAYMOLD);
		}
		SECTION("Mandrake Root") {
			old_item.ability = 157;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::MANDRAKE);
		}
		SECTION("Sapphire") {
			old_item.ability = 158;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SAPPHIRE);
		}
		SECTION("Smoky Crystal") {
			old_item.ability = 159;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SMOKY_CRYSTAL);
		}
		SECTION("Resurrection Balm") {
			old_item.ability = 160;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::RESURRECTION_BALM);
		}
		SECTION("Lockpicks") {
			old_item.ability = 161;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::LOCKPICKS);
			CHECK(new_item.abil_data[0] == 120);
		}
		
		// Missile Abilities
		SECTION("Returning Missile") {
			old_item.ability = 170;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::RETURNING_MISSILE);
		}
		SECTION("Lightning Missile") {
			old_item.ability = 171;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::DAMAGING_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eDamageType::FIRE));
		}
		SECTION("Exploding Missile") {
			old_item.ability = 172;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::EXPLODING_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
			CHECK(new_item.abil_data[1] == int(eDamageType::FIRE));
		}
		SECTION("Acid Missile") {
			old_item.ability = 173;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::STATUS_WEAPON);
			CHECK(new_item.abil_data[0] == 240);
			CHECK(new_item.abil_data[1] == int(eStatus::ACID));
		}
		SECTION("Slay Undead") {
			old_item.ability = 174;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_data[0] == 123);
			CHECK(new_item.abil_data[1] == int(eRace::UNDEAD));
		}
		SECTION("Slay Demon") {
			old_item.ability = 175;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::SLAYER_WEAPON);
			CHECK(new_item.abil_data[0] == 123);
			CHECK(new_item.abil_data[1] == int(eRace::DEMON));
		}
		SECTION("Heal Target") {
			old_item.ability = 176;
			new_item.import_legacy(old_item);
			CHECK(new_item.ability == eItemAbil::HEALING_WEAPON);
			CHECK(new_item.abil_data[0] == 120);
		}
	}
}
