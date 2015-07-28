/*
 *  item.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "item.hpp"

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <boost/lexical_cast.hpp>

#include "boe.consts.hpp" // TODO: If this is needed here, maybe it shouldn't be in the "boe" namespace
#include "oldstructs.hpp"
#include "spell.hpp"
#include "graphtool.hpp" // for get_str()
#include "fileio.hpp"

extern const std::multiset<eItemType> equippable = {
	eItemType::ONE_HANDED, eItemType::TWO_HANDED, eItemType::BOW, eItemType::ARROW, eItemType::THROWN_MISSILE,
	eItemType::TOOL, eItemType::SHIELD, eItemType::ARMOR, eItemType::HELM, eItemType::GLOVES,
	eItemType::SHIELD_2, eItemType::BOOTS, eItemType::RING, eItemType::NECKLACE, eItemType::PANTS,
	eItemType::CROSSBOW, eItemType::BOLTS, eItemType::MISSILE_NO_AMMO,
	// And these are the ones that you can equip two of
	eItemType::ONE_HANDED, eItemType::RING,
};

extern const std::multiset<eItemType> num_hands_to_use = {
	eItemType::ONE_HANDED, eItemType::TWO_HANDED, eItemType::TWO_HANDED, eItemType::SHIELD, eItemType::SHIELD_2,
};

// For following, if an item of type n is equipped, no other items of type n can be equipped,
// TODO: Should SHIELD and SHIELD_2 have an entry here?
std::map<const eItemType, const short> excluding_types = {
	{eItemType::BOW, 2}, {eItemType::ARROW, 1}, {eItemType::THROWN_MISSILE, 1},
	{eItemType::CROSSBOW, 2}, {eItemType::BOLTS, 1}, {eItemType::MISSILE_NO_AMMO, 2}
};

unsigned char cItem::rec_treas_class() const {
	short tmp = value;
	if(charges > 0) tmp *= charges;
	if(tmp >=   15) return 1;
	if(tmp >=  100) return 2;
	if(tmp >=  900) return 3;
	if(tmp >= 2400) return 4;
	return 0;
}

short cItem::item_weight() const {
	short n = charges, w = weight;
	if(variety == eItemType::NO_ITEM)
		return 0;
	if(charges > 0){
		if((variety == eItemType::ARROW) || (variety == eItemType::THROWN_MISSILE) || (variety == eItemType::POTION) || (variety == eItemType::BOLTS))
			return n * w;
	}
	return w;
}

bool cItem::abil_harms() const {
	if(magic_use_type == eItemUse::HARM_ONE || magic_use_type == eItemUse::HARM_ALL)
		return true;
	return false;
}

bool cItem::abil_group() const {
	if(magic_use_type == eItemUse::HELP_ALL || magic_use_type == eItemUse::HARM_ALL)
		return true;
	return false;
}

cItem::cItem(){
	variety = eItemType::NO_ITEM;
	item_level = 0;
	awkward = 0;
	bonus = 0;
	protection = 0;
	charges = 0;
	weap_type = eSkill::INVALID;
	magic_use_type = eItemUse::HELP_ONE;
	graphic_num = 0;
	ability = eItemAbil::NONE;
	abil_data[0] = 0;
	abil_data[1] = 0;
	missile = 0;
	type_flag = 0;
	is_special = 0;
	value = 0;
	weight = 0;
	special_class = 0;
	item_loc.x = 0;
	item_loc.y = 0;
	treas_class = 0;
	ident = property = magic = contained = held = false;
	cursed = concealed = enchanted = unsellable = false;
}

cItem::cItem(long preset) : cItem() {
	switch(preset){
		case 'nife':
			variety = eItemType::ONE_HANDED;
			item_level = 4;
			bonus = 1;
			weap_type = eSkill::EDGED_WEAPONS;
			graphic_num = 55;
			value = 2;
			weight = 7;
			full_name = "Bronze Knife";
			name = "Knife";
			ident = true;
			break;
		case 'buck':
			variety = eItemType::SHIELD;
			item_level = 1;
			awkward = 1;
			graphic_num = 75;
			value = 2;
			weight = 20;
			full_name = "Crude Buckler";
			name = "Buckler";
			ident = true;
			break;
		case 'bow ':
			variety = eItemType::BOW;
			weap_type = eSkill::ARCHERY;
			graphic_num = 10;
			value = 15;
			weight = 20;
			full_name = "Cavewood Bow";
			name = "Bow";
			ident = true;
			break;
		case 'arrw':
			variety = eItemType::ARROW;
			item_level = 12;
			charges = 12;
			graphic_num = 57;
			missile = 3;
			value = 1;
			weight = 1;
			full_name = "Arrows";
			name = "Arrows";
			ident = true;
			break;
		case 'pole':
			variety = eItemType::TWO_HANDED;
			item_level = 9;
			weap_type = eSkill::POLE_WEAPONS;
			graphic_num = 4;
			value = 10;
			weight = 20;
			full_name = "Stone Spear";
			name = "Spear";
			ident = true;
			break;
		case 'helm':
			variety = eItemType::HELM;
			item_level = 1;
			weap_type = eSkill::INVALID;
			graphic_num = 76;
			value = 6;
			weight = 15;
			full_name = "Leather Helm";
			name = "Helm";
			ident = true;
			break;
		case 'rdsk':
			variety = eItemType::THROWN_MISSILE;
			item_level = 9;
			bonus = 1;
			charges = 8;
			weap_type = eSkill::THROWN_MISSILES;
			graphic_num = 59;
			missile = 7;
			value = 10;
			weight = 1;
			full_name = "Iron Razordisks";
			name = "Razordisks";
			ident = true;
			break;
		case 'food':
			variety = eItemType::FOOD;
			graphic_num = 72;
			full_name = "Food";
			name = "Food";
			break;
		case 'spel':
			variety = eItemType::NON_USE_OBJECT;
			graphic_num = 63;
			break;
		case 'alch':
			variety = eItemType::POTION;
			charges = 1;
			graphic_num = 60;
			weight = 8;
			full_name = "Potion";
			name = "Potion";
			magic = true;
			break;
		case 'spec':
			item_level = -1;
			full_name = "Call Special Node";
		case 'shop':
			graphic_num = 105; // The blank graphic
			break;
	}
}

cItem::cItem(eAlchemy recipe) : cItem('alch') {
	full_name = get_str("magic-names", int(recipe) + 200);
	switch(recipe) {
		case eAlchemy::NONE: break;
		case eAlchemy::CURE_WEAK:
			value = 40;
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[0] = 2;
			abil_data[1] = int(eStatus::POISON);
			break;
		case eAlchemy::HEAL_WEAK:
			value = 60;
			ability = eItemAbil::AFFECT_HEALTH;
			abil_data[0] = 2;
			break;
		case eAlchemy::POISON_WEAK:
			value = 15;
			ability = eItemAbil::POISON_WEAPON;
			abil_data[0] = 2;
			break;
		case eAlchemy::SPEED_WEAK:
			value = 50;
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[0] = 2;
			abil_data[1] = int(eStatus::HASTE_SLOW);
			break;
		case eAlchemy::POISON_MED:
			value = 50;
			ability = eItemAbil::POISON_WEAPON;
			abil_data[0] = 4;
			break;
		case eAlchemy::HEAL_MED:
			value = 180;
			ability = eItemAbil::AFFECT_HEALTH;
			abil_data[0] = 5;
			break;
		case eAlchemy::CURE_STRONG:
			value = 200;
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[0] = 8;
			abil_data[1] = int(eStatus::POISON);
			break;
		case eAlchemy::SPEED_MED:
			value = 100;
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[0] = 5;
			abil_data[1] = int(eStatus::HASTE_SLOW);
			break;
		case eAlchemy::GRAYMOLD:
			value = 150;
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[0] = 4;
			abil_data[1] = int(eStatus::DISEASE);
			magic_use_type = eItemUse::HELP_ALL;
			break;
		case eAlchemy::POWER_WEAK:
			value = 100;
			ability = eItemAbil::AFFECT_SPELL_POINTS;
			abil_data[0] = 2;
			break;
		case eAlchemy::CLARITY:
			value = 200;
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[0] = 8;
			abil_data[1] = int(eStatus::DUMB);
			break;
		case eAlchemy::POISON_STRONG:
			value = 150;
			ability = eItemAbil::POISON_WEAPON;
			abil_data[0] = 6;
			break;
		case eAlchemy::HEAL_STRONG:
			value = 300;
			ability = eItemAbil::AFFECT_HEALTH;
			abil_data[0] = 8;
			break;
		case eAlchemy::POISON_KILL:
			value = 400;
			ability = eItemAbil::POISON_WEAPON;
			abil_data[0] = 8;
			break;
		case eAlchemy::RESURRECT:
			value = 100;
			ability = eItemAbil::RESURRECTION_BALM;
			break;
		case eAlchemy::POWER_MED:
			value = 300;
			ability = eItemAbil::AFFECT_SPELL_POINTS;
			abil_data[0] = 5;
			break;
		case eAlchemy::KNOWLEDGE:
			value = 500;
			ability = eItemAbil::AFFECT_SKILL_POINTS;
			abil_data[0] = 2;
			break;
		case eAlchemy::STRENGTH:
			value = 175;
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[0] = 8;
			abil_data[1] = int(eStatus::BLESS_CURSE);
			break;
		case eAlchemy::BLISS:
			value = 250;
			ability = eItemAbil::BLISS_DOOM;
			abil_data[0] = 5;
			break;
		case eAlchemy::POWER_STRONG:
			value = 500;
			ability = eItemAbil::AFFECT_SKILL_POINTS;
			abil_data[0] = 8;
			break;
	}
}

void cItem::enchant_weapon(eEnchant enchant_type,short new_val) {
	if(magic || ability != eItemAbil::NONE)
		return;
	if(variety != eItemType::ONE_HANDED && variety != eItemType::TWO_HANDED)
		return;
	magic = true;
	enchanted = true;
	std::string store_name = full_name;
	switch(enchant_type) {
		case eEnchant::PLUS_ONE:
			store_name += " (+1)";
			bonus++;
			value = new_val;
			break;
		case eEnchant::PLUS_TWO:
			store_name += " (+2)";
			bonus += 2;
			value = new_val;
			break;
		case eEnchant::PLUS_THREE:
			store_name += " (+3)";
			bonus += 3;
			value = new_val;
			break;
		case eEnchant::SHOOT_FLAME:
			store_name += " (F)";
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = 5;
			abil_data[1] = int(eSpell::FLAME);
			charges = 8;
			break;
		case eEnchant::FLAMING:
			store_name += " (F!)";
			value = new_val;
			ability = eItemAbil::DAMAGING_WEAPON;
			abil_data[0] = 5;
			abil_data[1] = int(eDamageType::FIRE);
			break;
		case eEnchant::PLUS_FIVE:
			store_name += " (+5)";
			value = new_val;
			bonus += 5;
			break;
		case eEnchant::BLESSED:
			store_name += " (B)";
			bonus++;
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[0] = 5;
			abil_data[1] = int(eStatus::BLESS_CURSE);
			charges = 8;
			break;
	}
	if(value > 15000)
		value = 15000;
	if(value < 0)
		value = 15000;
	full_name = store_name;
}

void cItem::append(legacy::item_record_type& old){
	variety = (eItemType) old.variety;
	item_level = old.item_level;
	awkward = old.awkward;
	bonus = old.bonus;
	protection = old.protection;
	charges = old.charges;
	if(old.type >= 1 && old.type <= 3)
		weap_type = eSkill(old.type + 2);
	else weap_type = eSkill::INVALID;
	if(variety == eItemType::BOW || variety == eItemType::CROSSBOW || variety == eItemType::MISSILE_NO_AMMO)
		weap_type = eSkill::ARCHERY;
	else if(variety == eItemType::THROWN_MISSILE)
		weap_type = eSkill::THROWN_MISSILES;
	magic_use_type = eItemUse(old.magic_use_type);
	graphic_num = old.graphic_num;
	if(graphic_num >= 150) // custom item graphic
		graphic_num += 850;
	else if(graphic_num == 59) // duplicate mushroom graphic
		graphic_num = 74;
	else if(graphic_num == 17) // gauntlets moved to tinyobj
		graphic_num = 133;
	else if(graphic_num >= 45) // small graphics were moved up to make a bit more room for new large graphics
		graphic_num += 10;
	abil_data[0] = old.ability_strength;
	switch(old.ability) {
		// Weapon abilities
		case 0:
			ability = eItemAbil::NONE;
			break;
		case 1: // Flaming weapon
			ability = eItemAbil::DAMAGING_WEAPON;
			abil_data[1] = int(eDamageType::UNBLOCKABLE);
			break;
		case 2: // Demon slayer
			ability = eItemAbil::SLAYER_WEAPON;
			abil_data[1] = int(eRace::DEMON);
			break;
		case 3: // Undead slayer
			ability = eItemAbil::SLAYER_WEAPON;
			abil_data[1] = int(eRace::UNDEAD);
			break;
		case 4: // Lizard slayer
			ability = eItemAbil::SLAYER_WEAPON;
			abil_data[1] = int(eRace::REPTILE);
			break;
		case 5: // Giant slayer
			ability = eItemAbil::SLAYER_WEAPON;
			abil_data[1] = int(eRace::GIANT);
			break;
		case 6: // Mage slayer
			ability = eItemAbil::SLAYER_WEAPON;
			abil_data[1] = int(eRace::MAGE);
			break;
		case 7: // Priest slayer
			ability = eItemAbil::SLAYER_WEAPON;
			abil_data[1] = int(eRace::PRIEST);
			break;
		case 8: // Bug slayer
			ability = eItemAbil::SLAYER_WEAPON;
			abil_data[1] = int(eRace::BUG);
			break;
		case 9: // Acidic weapon
			ability = eItemAbil::STATUS_WEAPON;
			abil_data[1] = int(eStatus::ACID);
			break;
		case 10:
			ability = eItemAbil::SOULSUCKER;
			break;
		case 11:
			ability = eItemAbil::DRAIN_MISSILES;
			break;
		case 12:
			ability = eItemAbil::WEAK_WEAPON;
			break;
		case 13:
			ability = eItemAbil::CAUSES_FEAR;
			break;
		case 14: // Poisoned weapon
			ability = eItemAbil::STATUS_WEAPON;
			abil_data[1] = int(eStatus::POISON);
			break;
		// General abilities
		case 30: // Protection
			ability = eItemAbil::DAMAGE_PROTECTION;
			abil_data[1] = int(eDamageType::WEAPON);
			break;
		case 31:
			ability = eItemAbil::FULL_PROTECTION;
			break;
		case 32: // Fire protection
			ability = eItemAbil::DAMAGE_PROTECTION;
			abil_data[1] = int(eDamageType::FIRE);
			if(abil_data[0] >= 7) abil_data[0] /= 2;
			break;
		case 33: // Cold protection
			ability = eItemAbil::DAMAGE_PROTECTION;
			abil_data[1] = int(eDamageType::COLD);
			if(abil_data[0] >= 7) abil_data[0] /= 2;
			break;
		case 34: // Poison protection
			ability = eItemAbil::STATUS_PROTECTION;
			abil_data[1] = int(eStatus::POISON);
			break;
		case 35: // Magic protection
			ability = eItemAbil::DAMAGE_PROTECTION;
			abil_data[1] = int(eDamageType::MAGIC);
			if(abil_data[0] >= 7) abil_data[0] /= 2;
			break;
		case 36: // Acid protection
			ability = eItemAbil::STATUS_PROTECTION;
			abil_data[1] = int(eStatus::ACID);
			break;
		case 37:
			ability = eItemAbil::SKILL;
			abil_data[1] = abil_data[0]; // archive original ability strength (SKILL doesn't use abil_data[1] for anything)
			abil_data[0] = item_level; // Put level into ability strength to preserve legacy behaviour
			break;
		case 38: // Strength
			ability = eItemAbil::BOOST_STAT;
			abil_data[0] = 1;
			abil_data[1] = int(eSkill::STRENGTH);
			break;
		case 39: // Dexterity
			ability = eItemAbil::BOOST_STAT;
			abil_data[0] = 1;
			abil_data[1] = int(eSkill::DEXTERITY);
			break;
		case 40: // Intelligence
			ability = eItemAbil::BOOST_STAT;
			abil_data[0] = 1;
			abil_data[1] = int(eSkill::INTELLIGENCE);
			break;
		case 41:
			ability = eItemAbil::ACCURACY;
			break;
		case 42:
			ability = eItemAbil::THIEVING;
			break;
		case 43:
			ability = eItemAbil::GIANT_STRENGTH;
			abil_data[1] = abil_data[0]; // archive original ability strength (GIANT_STRENGTH doesn't use abil_data[1] for anything)
			abil_data[0] = item_level; // Put level into ability strength to preserve legacy behaviour
			break;
		case 44:
			ability = eItemAbil::LIGHTER_OBJECT;
			break;
		case 45:
			ability = eItemAbil::HEAVIER_OBJECT;
			break;
		case 46:
			ability = eItemAbil::OCCASIONAL_STATUS;
			abil_data[1] = int(eStatus::BLESS_CURSE);
			magic_use_type = eItemUse::HELP_ONE;
			break;
		case 47:
			ability = eItemAbil::OCCASIONAL_STATUS;
			abil_data[1] = int(eStatus::HASTE_SLOW);
			magic_use_type = eItemUse::HELP_ONE;
			break;
		case 48:
			ability = eItemAbil::LIFE_SAVING;
			break;
		case 49:
			ability = eItemAbil::PROTECT_FROM_PETRIFY;
			break;
		case 50:
			ability = eItemAbil::REGENERATE;
			break;
		case 51:
			ability = eItemAbil::POISON_AUGMENT;
			break;
		case 52:
			ability = eItemAbil::OCCASIONAL_STATUS;
			abil_data[1] = int(eStatus::DISEASE);
			magic_use_type = eItemUse::HARM_ALL;
			break;
		case 53:
			ability = eItemAbil::WILL;
			break;
		case 54:
			ability = eItemAbil::FREE_ACTION;
			break;
		case 55:
			ability = eItemAbil::SPEED;
			break;
		case 56:
			ability = eItemAbil::SLOW_WEARER;
			break;
		case 57: // Protect from undead
			ability = eItemAbil::DAMAGE_PROTECTION;
			abil_data[1] = int(eDamageType::UNDEAD);
			if(abil_data[0] >= 7) abil_data[0] /= 2;
			break;
		case 58: // Protect from demons
			ability = eItemAbil::DAMAGE_PROTECTION;
			abil_data[1] = int(eDamageType::DEMON);
			if(abil_data[0] >= 7) abil_data[0] /= 2;
			break;
		case 59: // Protect from humanoids
			ability = eItemAbil::PROTECT_FROM_SPECIES;
			abil_data[1] = int(eRace::HUMANOID);
			if(abil_data[0] >= 7) abil_data[0] /= 2;
			break;
		case 60: // Protect from reptiles
			ability = eItemAbil::PROTECT_FROM_SPECIES;
			abil_data[1] = int(eRace::REPTILE);
			if(abil_data[0] >= 7) abil_data[0] /= 2;
			break;
		case 61: // Protect from giants
			ability = eItemAbil::PROTECT_FROM_SPECIES;
			abil_data[1] = int(eRace::GIANT);
			if(abil_data[0] >= 7) abil_data[0] /= 2;
			break;
		case 62: // Protect from disease
			ability = eItemAbil::STATUS_PROTECTION;
			abil_data[1] = int(eStatus::DISEASE);
			break;
		// Usable abilities
		case 70:
			ability = eItemAbil::POISON_WEAPON;
			break;
		case 71:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::BLESS_CURSE);
			break;
		case 72:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::POISON);
			break;
		case 73:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::HASTE_SLOW);
			break;
		case 74:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::INVULNERABLE);
			break;
		case 75:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::MAGIC_RESISTANCE);
			break;
		case 76:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::WEBS);
			break;
		case 77:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::DISEASE);
			break;
		case 78:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::INVISIBLE);
			break;
		case 79:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::DUMB);
			break;
		case 80:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::MARTYRS_SHIELD);
			break;
		case 81:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::ASLEEP);
			break;
		case 82:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::PARALYZED);
			break;
		case 83:
			ability = eItemAbil::AFFECT_STATUS;
			abil_data[1] = int(eStatus::ACID);
			break;
		case 84:
			ability = eItemAbil::BLISS_DOOM;
			if(magic_use_type == eItemUse::HARM_ONE)
				magic_use_type = eItemUse::HELP_ONE;
			else if(magic_use_type == eItemUse::HARM_ALL)
				magic_use_type = eItemUse::HELP_ALL;
			break;
		case 85:
			ability = eItemAbil::AFFECT_EXPERIENCE;
			break;
		case 86:
			ability = eItemAbil::AFFECT_SKILL_POINTS;
			break;
		case 87:
			ability = eItemAbil::AFFECT_HEALTH;
			break;
		case 88:
			ability = eItemAbil::AFFECT_SPELL_POINTS;
			break;
		case 89:
			ability = eItemAbil::BLISS_DOOM;
			if(magic_use_type == eItemUse::HELP_ONE)
				magic_use_type = eItemUse::HARM_ONE;
			else if(magic_use_type == eItemUse::HELP_ALL)
				magic_use_type = eItemUse::HARM_ALL;
			break;
		case 90:
			ability = eItemAbil::LIGHT;
			magic_use_type = eItemUse::HELP_ALL;
			break;
		case 91:
			ability = eItemAbil::AFFECT_PARTY_STATUS;
			abil_data[1] = int(ePartyStatus::STEALTH);
			magic_use_type = eItemUse::HELP_ALL;
			break;
		case 92:
			ability = eItemAbil::AFFECT_PARTY_STATUS;
			abil_data[1] = int(ePartyStatus::FIREWALK);
			magic_use_type = eItemUse::HELP_ALL;
			break;
		case 93:
			ability = eItemAbil::AFFECT_PARTY_STATUS;
			abil_data[1] = int(ePartyStatus::FLIGHT);
			magic_use_type = eItemUse::HELP_ALL;
			break;
		case 94:
			ability = eItemAbil::HEALTH_POISON;
			if(magic_use_type == eItemUse::HARM_ONE)
				magic_use_type = eItemUse::HELP_ONE;
			else if(magic_use_type == eItemUse::HARM_ALL)
				magic_use_type = eItemUse::HELP_ALL;
			break;
		case 95:
			ability = eItemAbil::CALL_SPECIAL;
			break;
		// Spells
		case 110:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::FLAME);
			break;
		case 111:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::FIREBALL);
			break;
		case 112:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::FIRESTORM);
			break;
		case 113:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::KILL);
			break;
		case 114:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::ICE_BOLT);
			break;
		case 115:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::SLOW);
			break;
		case 116:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::SHOCKWAVE);
			break;
		case 117:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::DISPEL_UNDEAD);
			break;
		case 118:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::RAVAGE_SPIRIT);
			break;
		case 119:
			ability = eItemAbil::SUMMONING;
			abil_data[1] = abil_data[0];
			abil_data[0] = 50;
			break;
		case 120:
			ability = eItemAbil::MASS_SUMMONING;
			abil_data[1] = abil_data[0];
			abil_data[0] = 6;
			break;
		case 121:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::ACID_SPRAY);
			break;
		case 122:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::FOUL_VAPOR);
			break;
		case 123:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::CLOUD_SLEEP);
			break;
		case 124:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::POISON);
			break;
		case 125:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::SHOCKSTORM);
			break;
		case 126:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::PARALYZE_BEAM);
			break;
		case 127:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::GOO_BOMB);
			break;
		case 128:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::STRENGTHEN_TARGET);
			break;
		case 129:
			ability = eItemAbil::QUICKFIRE;
			break;
		case 130:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::CHARM_MASS);
			break;
		case 131:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::MAGIC_MAP);
			break;
		case 132:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::DISPEL_BARRIER);
			break;
		case 133:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::WALL_ICE_BALL);
			break;
		case 134:
			ability = eItemAbil::CAST_SPELL;
			abil_data[1] = int(eSpell::CHARM_FOE);
			break;
		case 135:
			ability = eItemAbil::CAST_SPELL;
			abil_data[0] = abil_data[0] * 2 + 1;
			abil_data[1] = int(eSpell::ANTIMAGIC);
			break;
		// Reagents
		case 150:
			ability = eItemAbil::HOLLY;
			break;
		case 151:
			ability = eItemAbil::COMFREY;
			break;
		case 152:
			ability = eItemAbil::NETTLE;
			break;
		case 153:
			ability = eItemAbil::WORMGRASS;
			break;
		case 154:
			ability = eItemAbil::ASPTONGUE;
			break;
		case 155:
			ability = eItemAbil::EMBERF;
			break;
		case 156:
			ability = eItemAbil::GRAYMOLD;
			break;
		case 157:
			ability = eItemAbil::MANDRAKE;
			break;
		case 158:
			ability = eItemAbil::SAPPHIRE;
			break;
		case 159:
			ability = eItemAbil::SMOKY_CRYSTAL;
			break;
		case 160:
			ability = eItemAbil::RESURRECTION_BALM;
			break;
		case 161:
			ability = eItemAbil::LOCKPICKS;
			break;
		// Missile abilities
		case 170:
			ability = eItemAbil::RETURNING_MISSILE;
			break;
		case 171:
			ability = eItemAbil::DAMAGING_WEAPON;
			abil_data[1] = int(eDamageType::FIRE);
			break;
		case 172:
			ability = eItemAbil::EXPLODING_WEAPON;
			abil_data[1] = int(eDamageType::FIRE);
			break;
		case 173:
			ability = eItemAbil::STATUS_WEAPON;
			abil_data[0] *= 2;
			abil_data[1] = int(eStatus::ACID);
			break;
		case 174:
			ability = eItemAbil::SLAYER_WEAPON;
			abil_data[0] += 3;
			abil_data[1] = int(eRace::UNDEAD);
			break;
		case 175:
			ability = eItemAbil::SLAYER_WEAPON;
			abil_data[0] += 3;
			abil_data[1] = int(eRace::DEMON);
			break;
		case 176:
			ability = eItemAbil::HEALING_WEAPON;
			break;
	}
	type_flag = old.type_flag;
	is_special = old.is_special;
	value = old.value;
	weight = old.weight;
	special_class = old.special_class;
	item_loc.x = old.item_loc.x;
	item_loc.y = old.item_loc.y;
	full_name = std::string(old.full_name);
	name = old.name;
	treas_class = old.treas_class;
	ident = old.item_properties & 1;
	property = old.item_properties & 2;
	magic = old.item_properties & 4;
	contained = old.item_properties & 8;
	cursed = old.item_properties & 16;
	concealed = old.item_properties & 32;
	enchanted = held = false;
	unsellable = old.item_properties & 16;
	// Set missile, if needed
	if(variety == eItemType::ARROW || variety == eItemType::BOLTS) {
		missile = magic ? 4 : 3;
	} else if(variety == eItemType::MISSILE_NO_AMMO) {
		// Just assume it's a sling and use the rock missile.
		missile = 12;
	} else if(variety == eItemType::THROWN_MISSILE) {
		// This is tricky... basically, all we can really do is guess.
		// And the only way to guess is by examining the item's name
		// We'll use the unidentified name since it's more likely to contain the appropriate generic words
		auto npos = std::string::npos;
		// Okay, that failed. Try examining the item's name.
		if(name.find("Knife") != npos) missile = 10;
		// Unidentified name limit was quite short, so the S might've been cut off
		else if(name.find("Knive") != npos) missile = 10;
		else if(name.find("Spear") != npos) missile = 5;
		else if(name.find("Javelin") != npos) missile = 5;
		else if(name.find("Razordisk") != npos) missile = 7;
		else if(name.find("Star") != npos) missile = 7;
		else if(name.find("Dart") != npos) missile = 1;
		else if(name.find("Rock") != npos) missile = 12;
		// Okay, give up. Fall back to darts since we have no idea what's correct.
		else missile = 1;
	}
}

std::string cItem::getAbilName() const {
	bool harmful = abil_harms();
	bool party = abil_group();
	std::ostringstream sout;
	switch(ability) {
		case eItemAbil::UNUSED: case eItemAbil::UNUSED2: break; // Invalid
		case eItemAbil::NONE: sout << "No ability"; break;
		case eItemAbil::HEALING_WEAPON: sout << "Heals target"; break;
		case eItemAbil::RETURNING_MISSILE: sout << "Returning missile"; break;
		case eItemAbil::DISTANCE_MISSILE: sout << "Farflight missile"; break;
		case eItemAbil::SEEKING_MISSILE: sout << "Seeking missile"; break;
		case eItemAbil::ANTIMAGIC_WEAPON: sout << "Manasucker"; break;
		case eItemAbil::SOULSUCKER: sout << "Soulsucker"; break;
		case eItemAbil::DRAIN_MISSILES: sout << "Drain Missiles"; break;
		case eItemAbil::WEAK_WEAPON: sout << "Weak Weapon"; break;
		case eItemAbil::CAUSES_FEAR: sout << "Causes Fear"; break;
		case eItemAbil::WEAPON_CALL_SPECIAL: sout << "Unusual Attack Effect"; break;
		case eItemAbil::FULL_PROTECTION: sout << "Full Protection"; break;
		case eItemAbil::EVASION: sout << "Evasion"; break;
		case eItemAbil::MARTYRS_SHIELD: sout << "Martyr's Shield"; break;
		case eItemAbil::ENCUMBERING: sout << "Awkward Weapon"; break;
		case eItemAbil::SKILL: sout << "Skill"; break;
		case eItemAbil::BOOST_WAR: sout << "Warrior's Mantle"; break;
		case eItemAbil::BOOST_MAGIC: sout << "Mage's Mantle"; break;
		case eItemAbil::ACCURACY: sout << "Accuracy"; break;
		case eItemAbil::THIEVING: sout << "Thieving"; break;
		case eItemAbil::GIANT_STRENGTH: sout << "Giant Strength"; break;
		case eItemAbil::LIGHTER_OBJECT: sout << "Lighter Object"; break;
		case eItemAbil::HEAVIER_OBJECT: sout << "Heavier Object"; break;
		case eItemAbil::HIT_CALL_SPECIAL: sout << "Unusual Defense Effect"; break;
		case eItemAbil::DROP_CALL_SPECIAL: sout << "Unusual Effect When Dropped"; break;
		case eItemAbil::LIFE_SAVING: sout << "Life Saving"; break;
		case eItemAbil::PROTECT_FROM_PETRIFY: sout << "Protect from Petrify"; break;
		case eItemAbil::REGENERATE: sout << "Regenerate"; break;
		case eItemAbil::POISON_AUGMENT: sout << "Poison Augment"; break;
		case eItemAbil::RADIANT: sout << "Radiance"; break;
		case eItemAbil::WILL: sout << "Will"; break;
		case eItemAbil::FREE_ACTION: sout << "Free Action"; break;
		case eItemAbil::SPEED: sout << "Speed"; break;
		case eItemAbil::SLOW_WEARER: sout << "Slow Wearer"; break;
		case eItemAbil::LOCKPICKS: sout << "Lockpicks"; break;
		case eItemAbil::POISON_WEAPON: sout << "Poison Weapon"; break;
		case eItemAbil::CALL_SPECIAL: sout << "Unusual Ability"; break;
		case eItemAbil::QUICKFIRE: sout << "Quickfire"; break;
		case eItemAbil::HOLLY: sout << "Holly/Toadstool"; break;
		case eItemAbil::COMFREY: sout << "Comfrey Root"; break;
		case eItemAbil::NETTLE: sout << "Glowing Nettle"; break;
		case eItemAbil::WORMGRASS: sout << "Crypt Shroom/Wormgrass"; break;
		case eItemAbil::ASPTONGUE: sout << "Asptongue Mold"; break;
		case eItemAbil::EMBERF: sout << "Ember Flower"; break;
		case eItemAbil::GRAYMOLD: sout << "Graymold"; break;
		case eItemAbil::MANDRAKE: sout << "Mandrake Root"; break;
		case eItemAbil::SAPPHIRE: sout << "Sapphire"; break;
		case eItemAbil::SMOKY_CRYSTAL: sout << "Smoky Crystal"; break;
		case eItemAbil::RESURRECTION_BALM: sout << "Resurrection Balm"; break;
		case eItemAbil::DAMAGING_WEAPON:
			switch(eDamageType(abil_data[1])) {
				case eDamageType::FIRE: sout << "Flaming"; break;
				case eDamageType::MAGIC: sout << "Shocking"; break;
				case eDamageType::COLD: sout << "Frosty"; break;
				case eDamageType::POISON: sout << "Slimy"; break;
				case eDamageType::WEAPON: sout << "Enhanced"; break;
				case eDamageType::UNDEAD: sout << "Necrotic"; break;
				case eDamageType::DEMON: sout << "Unholy"; break;
				case eDamageType::SPECIAL:
				case eDamageType::UNBLOCKABLE: sout << "Dark"; break;
				case eDamageType::MARKED: break; // Invalid
			}
			sout << " Weapon";
			break;
		case eItemAbil::SLAYER_WEAPON:
			switch(eRace(abil_data[1])) {
				case eRace::UNKNOWN: break; // Invalid
				case eRace::DEMON: sout << "Demon"; break;
				case eRace::UNDEAD: sout << "Undead"; break;
				case eRace::REPTILE: sout << "Lizard"; break;
				case eRace::GIANT: sout << "Giant"; break;
				case eRace::MAGE: sout << "Mage"; break;
				case eRace::PRIEST: sout << "Priest"; break;
				case eRace::BUG: sout << "Bug"; break;
				case eRace::HUMAN: sout << "Human"; break;
				case eRace::NEPHIL: sout << "Nephil"; break;
				case eRace::SLITH: sout << "Slith"; break;
				case eRace::VAHNATAI: sout << "Vahnatai"; break;
				case eRace::HUMANOID: sout << "Humanoid"; break;
				case eRace::BEAST: sout << "Beast"; break;
				case eRace::IMPORTANT: sout << "VIP"; break; // TODO: This one should probably not exist
				case eRace::SLIME: sout << "Slime"; break;
				case eRace::STONE: sout << "Golem"; break;
				case eRace::DRAGON: sout << "Dragon"; break;
				case eRace::MAGICAL: sout << "Magical Beast"; break;
				case eRace::PLANT: sout << "Plant"; break;
				case eRace::BIRD: sout << "Bird"; break;
				case eRace::SKELETAL: sout << "Skeleton"; break;
				case eRace::GOBLIN: sout << "Goblin"; break;
			}
			sout << " Slayer";
			break;
		case eItemAbil::EXPLODING_WEAPON:
			sout << "Explodes ";
			switch(eDamageType(abil_data[1])) {
				case eDamageType::FIRE: sout << "in flames"; break;
				case eDamageType::COLD: sout << "into frost"; break;
				case eDamageType::MAGIC: sout << "in sparks"; break;
				case eDamageType::POISON: sout << "into slime"; break;
				case eDamageType::WEAPON: sout << "in shrapnel"; break;
				case eDamageType::SPECIAL:
				case eDamageType::UNBLOCKABLE: sout << "in darkness"; break;
				case eDamageType::UNDEAD: sout.str("Implodes"); break;
				case eDamageType::DEMON: sout << "into corruption"; break;
				case eDamageType::MARKED: break; // Invalid
			}
			break;
		case eItemAbil::STATUS_WEAPON:
			switch(eStatus(abil_data[1])) {
				case eStatus::MAIN: break; // Invalid
				case eStatus::POISONED_WEAPON: sout << "Poison-draining"; break;
				case eStatus::INVULNERABLE: sout << "Piercing"; break;
				case eStatus::MAGIC_RESISTANCE: sout << "Overwhelming"; break;
				case eStatus::INVISIBLE: sout << "Anti-sanctuary"; break;
				case eStatus::ACID: sout << "Acidic"; break;
				case eStatus::POISON: sout << "Poisoned"; break;
				case eStatus::BLESS_CURSE: sout << "Cursing"; break;
				case eStatus::HASTE_SLOW: sout << "Slowing"; break;
				case eStatus::WEBS: sout << "Webbing"; break;
				case eStatus::DISEASE: sout << "Infectious"; break;
				case eStatus::DUMB: sout << "Dumbfounding"; break;
				case eStatus::MARTYRS_SHIELD: sout << "Martyr Draining"; break;
				case eStatus::ASLEEP: sout << "Soporific"; break;
				case eStatus::PARALYZED: sout << "Paralytic"; break;
				case eStatus::FORCECAGE: sout << "Entrapping"; break;
				case eStatus::CHARM: sout << "Charming"; break;
			}
			sout << " Weapon";
			break;
		case eItemAbil::DAMAGE_PROTECTION:
			switch(eDamageType(abil_data[1])) {
				case eDamageType::WEAPON: break;
				case eDamageType::FIRE: sout << "Fire"; break;
				case eDamageType::COLD: sout << "Cold"; break;
				case eDamageType::MAGIC: sout << "Magic"; break;
				case eDamageType::DEMON: sout << "Demon"; break;
				case eDamageType::UNDEAD: sout << "Undead"; break;
				case eDamageType::POISON: sout << "Poison"; break;
				case eDamageType::SPECIAL:
				case eDamageType::UNBLOCKABLE: sout << "Darkness"; break;
				case eDamageType::MARKED: break; // Invalid
			}
			sout << " Protection";
			break;
		case eItemAbil::STATUS_PROTECTION:
			sout << "Protect From ";
			switch(eStatus(abil_data[1])) {
				case eStatus::MAIN: break; // Invalid
				case eStatus::POISONED_WEAPON:
				case eStatus::INVULNERABLE:
				case eStatus::MARTYRS_SHIELD:
				case eStatus::CHARM:
				case eStatus::INVISIBLE:
					break; // These have no negative aspect, so protection from them isn't implemented
				case eStatus::POISON: sout << "Poison"; break;
				case eStatus::ACID: sout << "Acid"; break;
				case eStatus::DISEASE: sout << "Disease"; break;
				case eStatus::BLESS_CURSE: sout << "Curses"; break;
				case eStatus::HASTE_SLOW: sout << "Slowing"; break;
				case eStatus::MAGIC_RESISTANCE: sout << "Magic Vulnerability"; break;
				case eStatus::WEBS: sout << "Webbing"; break;
				case eStatus::DUMB: sout << "Dumbfounding"; break;
				case eStatus::ASLEEP: sout << "Sleep"; break;
				case eStatus::PARALYZED: sout << "Paralysis"; break;
				case eStatus::FORCECAGE: sout << "Forcecage"; break;
			}
			break;
		case eItemAbil::BOOST_STAT:
			sout << get_str("skills", abil_data[1] * 2 + 1);
			break;
		case eItemAbil::OCCASIONAL_STATUS:
			sout << "Occasional ";
			switch(eStatus(abil_data[1])) {
				case eStatus::MAIN: break; // Invalid
				case eStatus::CHARM: // Doesn't affect PCs
				case eStatus::FORCECAGE: sout << (harmful ? "Entrapment" : "Release"); break;
				case eStatus::DISEASE: sout << (harmful ? "Disease" : "Cure Disease"); break;
				case eStatus::HASTE_SLOW: sout << (harmful ? "Slow" : "Haste"); break;
				case eStatus::BLESS_CURSE: sout << (harmful ? "Curse" : "Bless"); break;
				case eStatus::POISON: sout << (harmful ? "Poison" : "Cure"); break;
				case eStatus::WEBS: sout << (harmful ? "Webbing" : "Cleansing"); break;
				case eStatus::DUMB: sout << (harmful ? "Dumbfounding" : "Enlightening"); break;
				case eStatus::MARTYRS_SHIELD: sout << (harmful ? "Lose" : "Gain") << " Martyr's Shield"; break;
				case eStatus::INVULNERABLE: sout << (harmful ? "Lose" : "Gain") << " Invulnerability"; break;
				case eStatus::MAGIC_RESISTANCE: sout << "Magic " << (harmful ? "Vulnerability" : "Resistance"); break;
				case eStatus::INVISIBLE: sout << (harmful ? "Lose" : "Gain") << " Sanctuary"; break;
				case eStatus::POISONED_WEAPON: sout << (harmful ? "Lose" : "Gain") << " Weapon Poison"; break;
				case eStatus::ASLEEP: sout << (harmful ? "Sleep" : "Hyperactivity"); break;
				case eStatus::PARALYZED: sout << (harmful ? "Gain" : "Lose") << " Paralysis"; break;
				case eStatus::ACID: sout << (harmful ? "Gain" : "Neutralize") << " Acid"; break;
			}
			sout << (party ? " Party" : " Wearer");
			break;
		case eItemAbil::PROTECT_FROM_SPECIES:
			sout << "Protection from ";
			switch(eRace(abil_data[1])) {
				case eRace::UNKNOWN: break; // Invalid
				case eRace::UNDEAD: sout << "Undead"; break;
				case eRace::DEMON: sout << "Demons"; break;
				case eRace::HUMANOID: sout << "Humanoids"; break;
				case eRace::REPTILE: sout << "Reptiles"; break;
				case eRace::GIANT: sout << "Giants"; break;
				case eRace::HUMAN: sout << "Humans"; break;
				case eRace::NEPHIL: sout << "Nephilim"; break;
				case eRace::SLITH: sout << "Sliths"; break;
				case eRace::VAHNATAI: sout << "Vahnatai"; break;
				case eRace::BEAST: sout << "Beasts"; break;
				case eRace::IMPORTANT: sout << "VIPs"; break;
				case eRace::MAGE: sout << "Mages"; break;
				case eRace::PRIEST: sout << "Priests"; break;
				case eRace::SLIME: sout << "Slimes"; break;
				case eRace::STONE: sout << "Golems"; break;
				case eRace::BUG: sout << "Bugs"; break;
				case eRace::DRAGON: sout << "Dragons"; break;
				case eRace::MAGICAL: sout << "Magical Beasts"; break;
				case eRace::PLANT: sout << "Plants"; break;
				case eRace::BIRD: sout << "Birds"; break;
				case eRace::SKELETAL: sout << "Skeleton"; break;
				case eRace::GOBLIN: sout << "Goblin"; break;
			}
			break;
		case eItemAbil::AFFECT_STATUS:
			switch(eStatus(abil_data[1])) {
				case eStatus::MAIN: break; // Invalid;
				case eStatus::FORCECAGE: sout << (harmful ? "Entrapping" : "Cage Break"); break;
				case eStatus::CHARM: break; // TODO: Not implemented
				case eStatus::POISONED_WEAPON: sout << (harmful ? "Increase" : "Decrease") << " Weapon Poison"; break;
				case eStatus::BLESS_CURSE: sout << (harmful ? "Curse" : "Bless"); break;
				case eStatus::POISON: sout << (harmful ? "Cause" : "Cure") << " Poison"; break;
				case eStatus::HASTE_SLOW: sout << (harmful ? "Slow" : "Haste"); break;
				case eStatus::INVULNERABLE: sout << (harmful ? "Lose" : "Add") << " Invulnerability"; break;
				case eStatus::MAGIC_RESISTANCE: sout << (harmful ? "Lose" : "Add") << " Magic Resistance"; break;
				case eStatus::WEBS: sout << (harmful ? "Lose" : "Add") << "Webs"; break;
				case eStatus::DISEASE: sout << (harmful ? "Cause" : "Cure") << " Disease"; break;
				case eStatus::INVISIBLE: sout << (harmful ? "Lose" : "Add") << " Sanctuary"; break;
				case eStatus::DUMB: sout << (harmful ? "Add" : "Lose") << " Dumbfounding"; break;
				case eStatus::MARTYRS_SHIELD: sout << (harmful ? "Lose" : "Add") << " Martyr's Shield"; break;
				case eStatus::ASLEEP: sout << (harmful ? "Cause" : "Cure") << " Sleep"; break;
				case eStatus::PARALYZED: sout << (harmful ? "Cause" : "Cure") << " Paralysis"; break;
				case eStatus::ACID: sout << (harmful ? "Cause" : "Cure") << " Acid"; break;
			}
			break;
		case eItemAbil::CAST_SPELL:
			sout << "Spell: " << (*cSpell::fromNum(abil_data[1])).name();
			break;
		case eItemAbil::BLISS_DOOM:
			if(party)
				sout << "Party ";
			sout << (harmful ? "Doom" : "Bliss");
			break;
		case eItemAbil::AFFECT_EXPERIENCE:
			sout << (harmful ? "Drain" : "Gain") << " Experience";
			break;
		case eItemAbil::AFFECT_SKILL_POINTS:
			sout << (harmful ? "Drain" : "Gain") << " Skill Points";
			break;
		case eItemAbil::AFFECT_HEALTH:
			sout << (harmful ? "Drain Health" : "Heal");
			break;
		case eItemAbil::AFFECT_SPELL_POINTS:
			sout << (harmful ? "Drain" : "Restore") << " Spell Points";
			break;
		case eItemAbil::LIGHT:
			sout << (harmful ? "Drain" : "Increase") << " Light";
			break;
		case eItemAbil::AFFECT_PARTY_STATUS:
			sout << (harmful ? "Lose " : "Gain ");
			switch(ePartyStatus(abil_data[1])) {
				case ePartyStatus::STEALTH: sout << "Stealth"; break;
				case ePartyStatus::FLIGHT: sout << "Flight"; break;
				case ePartyStatus::DETECT_LIFE: sout << "Life Detection"; break;
				case ePartyStatus::FIREWALK: sout << "Firewalk"; break;
			}
			break;
		case eItemAbil::HEALTH_POISON:
			sout << "Major " << (harmful ? "Poison" : "Healing");
			if(party) sout << " All";
			break;
		case eItemAbil::SUMMONING:
			sout << "Summons %s";
			break;
		case eItemAbil::MASS_SUMMONING:
			sout << "Mass summon %s";
			break;
	}
	return sout.str();
}

void cItem::writeTo(std::ostream& file, std::string prefix) const {
	file << prefix << "VARIETY " << variety << '\n';
	file << prefix << "LEVEL " << item_level << '\n';
	file << prefix << "AWKWARD " << awkward << '\n';
	file << prefix << "BONUS " << bonus << '\n';
	file << prefix << "PROT " << protection << '\n';
	file << prefix << "CHARGES " << charges << '\n';
	file << prefix << "WEAPON " << weap_type << '\n';
	file << prefix << "USE " << magic_use_type << '\n';
	file << prefix << "ICON " << graphic_num << '\n';
	file << prefix << "ABILITY " << ability << '\n';
	file << prefix << "ABILSTR " << abil_data[0] << '\t' << abil_data[1] << '\n';
	file << prefix << "TYPE " << type_flag << '\n';
	file << prefix << "ISSPEC " << is_special << '\n';
	file << prefix << "VALUE " << value << '\n';
	file << prefix << "WEIGHT " << weight << '\n';
	file << prefix << "SPEC " << special_class << '\n';
	file << prefix << "AT " << item_loc.x << ' ' << item_loc.y << '\n';
	file << prefix << "FULLNAME " << maybe_quote_string(full_name) << '\n';
	file << prefix << "NAME " << maybe_quote_string(name) << '\n';
	file << prefix << "TREASURE " << treas_class << '\n';
	if(ident) file << prefix << "IDENTIFIED\n";
	if(property) file << prefix << "PROPERTY\n";
	if(magic) file << prefix << "MAGIC\n";
	if(contained) file << prefix << "CONTAINED\n";
	if(held) file << prefix << "HELD\n";
	if(cursed) file << prefix << "CURSED\n";
	if(concealed) file << prefix << "CONCEALED\n";
	if(enchanted) file << prefix << "ENCHANTED\n";
	if(unsellable) file << prefix << "UNSELLABLE\n";
}

void cItem::readFrom(std::istream& sin){
	while(sin) {
		std::string cur;
		getline(sin, cur);
		std::istringstream sin(cur);
		sin >> cur;
		if(cur == "VARIETY") sin >> variety;
		else if(cur == "LEVEL") sin >> item_level;
		else if(cur == "AWKWARD") sin >> awkward;
		else if(cur == "BONUS") sin >> bonus;
		else if(cur == "PROT") sin >> protection;
		else if(cur == "CHARGES") sin >> charges;
		else if(cur == "WEAPON") sin >> weap_type;
		else if(cur == "USE") sin >> magic_use_type;
		else if(cur == "ICON") sin >> graphic_num;
		else if(cur == "ABILITY") sin >> ability;
		else if(cur == "ABILSTR") sin >> abil_data[0] >> abil_data[1];
		else if(cur == "TYPE") sin >> type_flag;
		else if(cur == "ISSPEC") sin >> is_special;
		else if(cur == "VALUE") sin >> value;
		else if(cur == "WEIGHT") sin >> weight;
		else if(cur == "SPEC") sin >> special_class;
		else if(cur == "AT") sin >> item_loc.x >> item_loc.y;
		else if(cur == "FULLNAME") full_name = read_maybe_quoted_string(sin);
		else if(cur == "NAME") name = read_maybe_quoted_string(sin);
		else if(cur == "TREASURE") sin >> treas_class;
		else if(cur == "IDENTIFIED") ident = true;
		else if(cur == "PROPERTY") property = true;
		else if(cur == "MAGIC") magic = true;
		else if(cur == "CONTAINED") contained = true;
		else if(cur == "HELD") held = true;
		else if(cur == "CURSED") cursed = true;
		else if(cur == "CONCEALED") concealed = true;
		else if(cur == "ENCHANTED") enchanted = true;
		else if(cur == "UNSELLABLE") unsellable = true;
	}
}
