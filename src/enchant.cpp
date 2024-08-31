//
//  enchant.cpp
//  BoE
//
//  Created by Celtic Minstrel on 2023-08-28.
//
//

#include "enchant.hpp"
#include "mathutil.hpp"

std::map<eEnchant, cEnchant> cEnchant::dictionary;

cEnchant::cEnchant(eEnchant id, std::string suf) : id(id), suffix(suf) {}

cEnchant& cEnchant::withCost(short cost) {
	aug_cost = cost;
	return *this;
}

cEnchant& cEnchant::withBonus(int bonus) {
	add_bonus = bonus;
	return *this;
}

cEnchant& cEnchant::withAbility(eItemAbil abil, unsigned int stren) {
	add_ability = abil;
	abil_strength = stren;
	return *this;
}

cEnchant& cEnchant::withAbility(eItemAbil abil, unsigned int stren, eSpell spell) {
	withAbility(abil, stren);
	abil_data.spell = spell;
	return *this;
}

cEnchant& cEnchant::withAbility(eItemAbil abil, unsigned int stren, eDamageType dmg) {
	withAbility(abil, stren);
	abil_data.damage = dmg;
	return *this;
}

cEnchant& cEnchant::withAbility(eItemAbil abil, unsigned int stren, eStatus status) {
	withAbility(abil, stren);
	abil_data.status = status;
	return *this;
}

cEnchant& cEnchant::withCharges(int c) {
	charges = c;
	return *this;
}

const cEnchant& cEnchant::finish() {
	dictionary.emplace(id, *this);
	return *this;
}

int cEnchant::adjust_value(int initial_value) const {
	return max(aug_cost * 100, initial_value * (5 + aug_cost));
}

const cEnchant& operator* (eEnchant ench) {
	static cEnchant none{eEnchant::NONE, ""};
	if(ench == eEnchant::NONE) return none;
	return cEnchant::dictionary.at(ench);
}

cEnchant E_PLUS_ONE = cEnchant(eEnchant::PLUS_ONE, "+1").withCost(4).withBonus(1).finish();
cEnchant E_PLUS_TWO = cEnchant(eEnchant::PLUS_TWO, "+2").withCost(7).withBonus(2).finish();
cEnchant E_PLUS_THREE = cEnchant(eEnchant::PLUS_THREE, "+3").withCost(10).withBonus(3).finish();
cEnchant E_SHOOT_FLAME = cEnchant(eEnchant::SHOOT_FLAME, "F").withCost(8).withAbility(eItemAbil::CAST_SPELL, 5, eSpell::FLAME).withCharges(8).finish();
cEnchant E_FLAMING = cEnchant(eEnchant::FLAMING, "F!").withCost(15).withAbility(eItemAbil::DAMAGING_WEAPON, 5, eDamageType::FIRE).finish();
cEnchant E_PLUS_FIVE = cEnchant(eEnchant::PLUS_FIVE, "+5").withCost(15).withBonus(5).finish();
cEnchant E_BLESSED = cEnchant(eEnchant::BLESSED, "B").withCost(10).withBonus(1).withAbility(eItemAbil::AFFECT_STATUS, 5, eStatus::BLESS_CURSE);
cEnchant E_PLUS_FOUR = cEnchant(eEnchant::PLUS_FOUR, "+5").withCost(15).withBonus(4).finish();
