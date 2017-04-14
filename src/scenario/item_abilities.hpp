//
//  item_abilities.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_ITEM_ABILITIES_HPP
#define BoE_ITEM_ABILITIES_HPP

enum class eItemAbil {
	// Weapon abilities
	NONE = 0,
	DAMAGING_WEAPON = 1,
	SLAYER_WEAPON = 2,
	HEALING_WEAPON = 3,
	EXPLODING_WEAPON = 4,
	RETURNING_MISSILE = 5,
	DISTANCE_MISSILE = 6,
	SEEKING_MISSILE = 7,
	ANTIMAGIC_WEAPON = 8,
	STATUS_WEAPON = 9,
	SOULSUCKER = 10,
	UNUSED = 11,
	WEAK_WEAPON = 12,
	CAUSES_FEAR = 13,
	WEAPON_CALL_SPECIAL = 14,
	HP_DAMAGE = 15,
	HP_DAMAGE_REVERSE = 16,
	SP_DAMAGE = 17,
	SP_DAMAGE_REVERSE = 18,
	// General abilities
	DAMAGE_PROTECTION = 30,
	FULL_PROTECTION = 31,
	MAGERY = 32,
	EVASION = 33,
	MARTYRS_SHIELD = 34,
	ENCUMBERING = 35,
	STATUS_PROTECTION = 36,
	SKILL = 37,
	BOOST_STAT = 38,
	BOOST_WAR = 39,
	BOOST_MAGIC = 40,
	ACCURACY = 41,
	THIEVING = 42,
	GIANT_STRENGTH = 43,
	LIGHTER_OBJECT = 44,
	HEAVIER_OBJECT = 45,
	OCCASIONAL_STATUS = 46,
	HIT_CALL_SPECIAL = 47,
	LIFE_SAVING = 48,
	PROTECT_FROM_PETRIFY = 49,
	REGENERATE = 50,
	POISON_AUGMENT = 51,
	RADIANT = 52,
	WILL = 53,
	FREE_ACTION = 54,
	SPEED = 55,
	SLOW_WEARER = 56,
	PROTECT_FROM_SPECIES = 57,
	LOCKPICKS = 58,
	DRAIN_MISSILES = 59,
	DROP_CALL_SPECIAL = 60,
	// Usable
	POISON_WEAPON = 70,
	AFFECT_STATUS = 71,
	CAST_SPELL = 72,
	BLISS_DOOM = 73,
	AFFECT_EXPERIENCE = 74,
	AFFECT_SKILL_POINTS = 75,
	AFFECT_HEALTH = 76,
	AFFECT_SPELL_POINTS = 77,
	LIGHT = 78,
	AFFECT_PARTY_STATUS = 79,
	HEALTH_POISON = 80,
	CALL_SPECIAL = 81,
	SUMMONING = 82,
	MASS_SUMMONING = 83,
	QUICKFIRE = 84,
	MESSAGE = 85,
	// Reagents
	HOLLY = 150, // Holly/Toadstool
	COMFREY = 151, // Comfrey Root
	NETTLE = 152, // Glowing Nettle
	WORMGRASS = 153, // Crypt Shroom/Wormgrass
	ASPTONGUE = 154, // Asptongue Mold
	EMBERF = 155, // Ember Flower
	GRAYMOLD = 156,
	MANDRAKE = 157,
	SAPPHIRE = 158,
	SMOKY_CRYSTAL = 159,
	RESURRECTION_BALM = 160,
};

enum class eItemAbilCat {
	INVALID = -1,
	WEAPON, GENERAL, USABLE, REAGENT
};

inline eItemAbilCat getItemAbilCategory(eItemAbil abil) {
	int code = (int) abil;
	if(code >= 0 && code <= 13)
		return eItemAbilCat::WEAPON;
	if(code >= 30 && code <= 57)
		return eItemAbilCat::GENERAL;
	if(code >= 70 && code <= 129)
		return eItemAbilCat::USABLE;
	if(code >= 150 && code <= 161)
		return eItemAbilCat::REAGENT;
	return eItemAbilCat::INVALID;
}

std::ostream& operator << (std::ostream& out, eItemAbil e);
std::istream& operator >> (std::istream& in, eItemAbil& e);

#endif
