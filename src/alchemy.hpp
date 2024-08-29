//
//  alchemy.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_ALCHEMY_HPP
#define BoE_ALCHEMY_HPP

#include "scenario/item_abilities.hpp"
#include "scenario/item_variety.hpp"

#include <array>
#include <map>

enum class eAlchemy {
	NONE = -1,
	CURE_WEAK = 0,
	HEAL_WEAK = 1,
	POISON_WEAK = 2,
	SPEED_WEAK = 3,
	POISON_MED = 4,
	HEAL_MED = 5,
	CURE_STRONG = 6,
	SPEED_MED = 7,
	GRAYMOLD = 8,
	POWER_WEAK = 9,
	CLARITY = 10,
	POISON_STRONG = 11,
	HEAL_STRONG = 12,
	POISON_KILL = 13,
	RESURRECT = 14,
	POWER_MED = 15,
	KNOWLEDGE = 16,
	STRENGTH = 17,
	BLISS = 18,
	POWER_STRONG = 19,
};

class cAlchemy {
	static std::map<eAlchemy,cAlchemy> dictionary;
	static const std::array<short, 9> fail_chances;
	friend const cAlchemy& operator*(eAlchemy spell_num);
public:
	cAlchemy(eAlchemy id);
	cAlchemy& withValue(short val);
	cAlchemy& withDifficulty(int d);
	cAlchemy& withIngredient(eItemAbil ingred);
	cAlchemy& withIngredients(eItemAbil first, eItemAbil second);
	cAlchemy& withAbility(eItemAbil abil, int stren);
	cAlchemy& withAbility(eItemAbil abil, int stren, eStatus status);
	cAlchemy& withUse(eItemUse use);
	const cAlchemy& finish();
	eAlchemy id;
	short value = 0;
	int difficulty = 0;
	eItemAbil ability = eItemAbil::NONE, ingred1, ingred2;
	unsigned int abil_strength = 0;
	uItemAbilData abil_data;
	eItemUse magic_use_type = eItemUse::HELP_ONE;
	short fail_chance(short skill) const;
	short charges(short skill) const;
	bool can_make(short skill) const;
};

// Need to declare this a second time in order for it to be in scope where it's needed
const cAlchemy& operator* (eAlchemy alch);

#endif
