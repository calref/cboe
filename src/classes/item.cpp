/*
 *  item.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "classes.h"
#include "boe.consts.h" // TODO: If this is needed here, maybe it shouldn't be in the "boe" namespace
#include "oldstructs.h"

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

cItem::cItem(){
	variety = eItemType::NO_ITEM;
	item_level = 0;
	awkward = 0;
	bonus = 0;
	protection = 0;
	charges = 0;
	weap_type = eSkill::INVALID;
	magic_use_type = 0;
	graphic_num = 0;
	ability = eItemAbil::NONE;
	abil_data[0] = 0;
	type_flag = 0;
	is_special = 0;
	value = 0;
	weight = 0;
	special_class = 0;
	item_loc.x = 0;
	item_loc.y = 0;
	full_name = "";
	name = "";
	treas_class = 0;
	ident = property = magic = contained = false;
	cursed = concealed = enchanted = unsellable = false;
}

cItem::cItem(long preset){
	ability = eItemAbil::NONE;
	abil_data[0] = 0;
	type_flag = 0;
	is_special = 0;
	special_class = 0;
	item_loc.x = 0;
	item_loc.y = 0;
	treas_class = 0;
	ident = property = magic = contained = false;
	cursed = concealed = enchanted = unsellable = false;
	switch(preset){
		case 'nife':
			variety = eItemType::ONE_HANDED;
			item_level = 4;
			awkward = 0;
			bonus = 1;
			protection = 0;
			charges = 0;
			weap_type = eSkill::EDGED_WEAPONS;
			magic_use_type = 0;
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
			bonus = 0;
			protection = 0;
			charges = 0;
			weap_type = eSkill::INVALID;
			magic_use_type = 0;
			graphic_num = 75;
			value = 2;
			weight = 20;
			full_name = "Crude Buckler";
			name = "Buckler";
			ident = true;
			break;
		case 'bow ':
			variety = eItemType::BOW;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			weap_type = eSkill::INVALID;
			magic_use_type = 0;
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
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 12;
			weap_type = eSkill::INVALID;
			magic_use_type = 0;
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
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			weap_type = eSkill::POLE_WEAPONS;
			magic_use_type = 0;
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
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			weap_type = eSkill::INVALID;
			magic_use_type = 0;
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
			awkward = 0;
			bonus = 1;
			protection = 0;
			charges = 8;
			weap_type = eSkill::INVALID;
			magic_use_type = 0;
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
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			weap_type = eSkill::INVALID;
			magic_use_type = 0;
			graphic_num = 72;
			value = 0;
			weight = 0;
			full_name = "Food";
			name = "Food";
			ident = false;
			break;
		case 'spel':
			variety = eItemType::NON_USE_OBJECT;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 0;
			weap_type = eSkill::INVALID;
			magic_use_type = 0;
			graphic_num = 63;
			value = 0;
			weight = 0;
			full_name = "";
			name = "";
			ident = false;
			break;
		case 'alch':
			variety = eItemType::POTION;
			item_level = 0;
			awkward = 0;
			bonus = 0;
			protection = 0;
			charges = 1;
			weap_type = eSkill::INVALID;
			magic_use_type = 0;
			graphic_num = 60;
			value = 0;
			weight = 8;
			full_name = "Potion";
			name = "Potion";
			ident = false;
			magic = true;
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
			magic_use_type = 2;
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
	magic_use_type = old.magic_use_type;
	graphic_num = old.graphic_num;
	if(graphic_num >= 150) // custom item graphic
		graphic_num += 850;
	else if(graphic_num == 59) // duplicate mushroom graphic
		graphic_num = 74;
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
			break;
		case 33: // Cold protection
			ability = eItemAbil::DAMAGE_PROTECTION;
			abil_data[1] = int(eDamageType::COLD);
			break;
		case 34: // Poison protection
			ability = eItemAbil::STATUS_PROTECTION;
			abil_data[1] = int(eStatus::POISON);
			break;
		case 35: // Magic protection
			ability = eItemAbil::DAMAGE_PROTECTION;
			abil_data[1] = int(eDamageType::MAGIC);
			break;
		case 36: // Acid protection
			ability = eItemAbil::STATUS_PROTECTION;
			abil_data[1] = int(eStatus::ACID);
			break;
		case 37:
			ability = eItemAbil::SKILL;
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
			magic_use_type = 0;
			break;
		case 47:
			ability = eItemAbil::OCCASIONAL_STATUS;
			abil_data[1] = int(eStatus::HASTE_SLOW);
			magic_use_type = 0;
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
			magic_use_type = 3;
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
			break;
		case 58: // Protect from demons
			ability = eItemAbil::DAMAGE_PROTECTION;
			abil_data[1] = int(eDamageType::DEMON);
			break;
		case 59: // Protect from humanoids
			ability = eItemAbil::PROTECT_FROM_SPECIES;
			abil_data[1] = int(eRace::HUMANOID);
			break;
		case 60: // Protect from reptiles
			ability = eItemAbil::PROTECT_FROM_SPECIES;
			abil_data[1] = int(eRace::REPTILE);
			break;
		case 61: // Protect from giants
			ability = eItemAbil::PROTECT_FROM_SPECIES;
			abil_data[1] = int(eRace::GIANT);
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
			if(magic_use_type == 1)
				magic_use_type = 0;
			else if(magic_use_type == 3)
				magic_use_type = 2;
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
			if(magic_use_type == 0)
				magic_use_type = 1;
			else if(magic_use_type == 2)
				magic_use_type = 3;
			break;
		case 90:
			ability = eItemAbil::LIGHT;
			magic_use_type = 2;
			break;
		case 91:
			ability = eItemAbil::AFFECT_PARTY_STATUS;
			abil_data[1] = int(ePartyStatus::STEALTH);
			break;
		case 92:
			ability = eItemAbil::AFFECT_PARTY_STATUS;
			abil_data[1] = int(ePartyStatus::FIREWALK);
			break;
		case 93:
			ability = eItemAbil::AFFECT_PARTY_STATUS;
			abil_data[1] = int(ePartyStatus::FLIGHT);
			break;
		case 94:
			ability = eItemAbil::HEALTH_POISON;
			if(magic_use_type == 1)
				magic_use_type = 0;
			else if(magic_use_type == 3)
				magic_use_type = 2;
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
	enchanted = false;
	unsellable = old.item_properties & 16;
	// Set missile, if needed
	switch(variety) {
		case eItemType::ARROW:
		case eItemType::BOLTS:
			missile = magic ? 4 : 3;
			break;
		case eItemType::MISSILE_NO_AMMO:
			// Just assume it's a sling and use the rock missile.
			missile = 12;
			break;
		case eItemType::THROWN_MISSILE:
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
			break;
	}
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
	file << prefix << "FULLNAME " << full_name << '\n';
	file << prefix << "NAME " << name << '\n';
	file << prefix << "TREASURE " << treas_class << '\n';
	if(ident) file << prefix << "IDENTIFIED\n";
	if(property) file << prefix << "PROPERTY\n";
	if(magic) file << prefix << "MAGIC\n";
	if(contained) file << prefix << "CONTAINED\n";
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
		else if(cur == "FULLNAME"){
			getline(sin,cur);
			full_name = cur;
		}else if(cur == "NAME"){
			getline(sin,cur);
			name = cur;
		}
		else if(cur == "TREASURE") sin >> treas_class;
		else if(cur == "IDENTIFIED") ident = true;
		else if(cur == "PROPERTY") property = true;
		else if(cur == "MAGIC") magic = true;
		else if(cur == "CONTAINED") contained = true;
		else if(cur == "CURSED") cursed = true;
		else if(cur == "CONCEALED") concealed = true;
		else if(cur == "ENCHANTED") enchanted = true;
		else if(cur == "UNSELLABLE") unsellable = true;
	}
}

std::ostream& operator << (std::ostream& out, eSkill e){
	return out << (int) e;
}

std::ostream& operator << (std::ostream& out, eItemType e){
	return out << (int) e;
}

std::ostream& operator << (std::ostream& out, eItemAbil e){
	return out << (int) e;
}

// TODO: Perhaps this should understand symbolic names as well?
std::istream& operator >> (std::istream& in, eSkill& e){
	int i;
	in >> i;
	if(i >= 0 && i < 19)
		e = (eSkill) i;
	else e = eSkill::INVALID;
	return in;
}

// TODO: Perhaps this should understand symbolic names as well?
std::istream& operator >> (std::istream& in, eItemType& e){
	int i;
	in >> i;
	if(i > 0 && i < 28)
		e = (eItemType) i;
	else e = eItemType::NO_ITEM;
	return in;
}

// TODO: Perhaps this should understand symbolic names as well?
std::istream& operator >> (std::istream& in, eItemAbil& e){
	int i;
	in >> i;
	if((i > 0 && i < 15) || (i > 29 && i < 63) ||
	   (i > 69 && i < 95) || (i > 109 && i < 136) ||
	   (i > 149 && i < 162) || (i > 169 && i < 177))
		e = (eItemAbil) i;
	else e = eItemAbil::NONE;
	return in;
}
