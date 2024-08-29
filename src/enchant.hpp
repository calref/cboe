//
//  enchant.hpp
//  BoE
//
//  Created by Celtic Minstrel on 2023-08-28.
//
//

#ifndef BoE_ENCHANT_HPP
#define BoE_ENCHANT_HPP

#include <iosfwd>
#include <map>
#include <string>
#include "damage.hpp"
#include "scenario/item_abilities.hpp"
#include "spell.hpp"

enum class eEnchant {
	NONE = -1,
	PLUS_ONE,
	PLUS_TWO,
	PLUS_THREE,
	SHOOT_FLAME,
	FLAMING,
	PLUS_FIVE,
	BLESSED,
};

class cEnchant {
	static std::map<eEnchant, cEnchant> dictionary;
	friend const cEnchant& operator* (eEnchant ench);
public:
	static const int MAX = int(eEnchant::BLESSED);
	cEnchant(eEnchant id, std::string suf);
	cEnchant& withCost(short cost);
	cEnchant& withBonus(int bonus);
	cEnchant& withAbility(eItemAbil abil, unsigned int stren);
	cEnchant& withAbility(eItemAbil abil, unsigned int stren, eSpell spell);
	cEnchant& withAbility(eItemAbil abil, unsigned int stren, eDamageType dmg);
	cEnchant& withAbility(eItemAbil abil, unsigned int stren, eStatus status);
	cEnchant& withCharges(int charges);
	const cEnchant& finish();
	eEnchant id;
	std::string suffix;
	short aug_cost;
	int add_bonus = 0;
	eItemAbil add_ability = eItemAbil::NONE;
	unsigned int abil_strength = 0;
	uItemAbilData abil_data;
	int charges = 0;
	int adjust_value(int initial_value) const;
};

// Need to declare this a second time in order for it to be in scope where it's needed
const cEnchant& operator* (eEnchant ench);

std::ostream& operator<< (std::ostream& out, eEnchant ench);
std::istream& operator>> (std::istream& in, eEnchant& ench);

#endif
