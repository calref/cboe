//
//  alchemy.cpp
//  BoE
//
//  Created by Celtic Minstrel on 2023-08-28.
//
//

#include "alchemy.hpp"

std::map<eAlchemy,cAlchemy> cAlchemy::dictionary;

// Fail chance lookup, based on adjusted skill
const std::array<short, 9> cAlchemy::fail_chances = {
	50,40,30,20,10,
	8,6,4,2,
};

cAlchemy::cAlchemy(eAlchemy id) : id(id) {}

cAlchemy& cAlchemy::withValue(short val) {
	value = val;
	return *this;
}

cAlchemy& cAlchemy::withDifficulty(int d) {
	difficulty = d;
	return *this;
}

cAlchemy& cAlchemy::withIngredient(eItemAbil ingred) {
	ingred1 = ingred;
	ingred2 = eItemAbil::NONE;
	return *this;
}

cAlchemy& cAlchemy::withIngredients(eItemAbil first, eItemAbil second) {
	ingred1 = first;
	ingred2 = second;
	return *this;
}

cAlchemy& cAlchemy::withAbility(eItemAbil abil, int stren) {
	ability = abil;
	abil_strength = stren;
	return *this;
}

cAlchemy& cAlchemy::withAbility(eItemAbil abil, int stren, eStatus status) {
	withAbility(abil, stren);
	abil_data.status = status;
	return *this;
}

cAlchemy& cAlchemy::withUse(eItemUse use) {
	magic_use_type = use;
	return *this;
}

const cAlchemy& cAlchemy::finish() {
	dictionary.emplace(id, *this);
	return *this;
}

short cAlchemy::fail_chance(short skill) const {
	if(skill < difficulty) return 100;
	if(skill - difficulty > fail_chances.size()) return 0;
	return fail_chances[skill - difficulty];
}

short cAlchemy::charges(short skill) const {
	if(skill < difficulty) return 0;
	short diff = skill - difficulty;
	if(diff >= 11) return 3;
	if(diff >= 5) return 2;
	return 1;
}

bool cAlchemy::can_make(short skill) const {
	return fail_chance(skill) < 100;
}

const cAlchemy& operator* (eAlchemy alch) {
	return cAlchemy::dictionary.at(alch);
}

cAlchemy A_CURE_WEAK = cAlchemy(eAlchemy::CURE_WEAK).withDifficulty(1).withValue(40)
	.withIngredient(eItemAbil::HOLLY).withAbility(eItemAbil::AFFECT_STATUS, 2, eStatus::POISON).finish();
cAlchemy A_HEAL_WEAK = cAlchemy(eAlchemy::HEAL_WEAK).withDifficulty(1).withValue(60)
	.withIngredient(eItemAbil::COMFREY).withAbility(eItemAbil::AFFECT_HEALTH, 2).finish();
cAlchemy A_POISON_WEAK = cAlchemy(eAlchemy::POISON_WEAK).withDifficulty(1).withValue(15)
	.withIngredient(eItemAbil::HOLLY).withAbility(eItemAbil::POISON_WEAPON, 2).finish();
cAlchemy A_SPEED_WEAK = cAlchemy(eAlchemy::SPEED_WEAK).withDifficulty(3).withValue(50)
	.withIngredients(eItemAbil::COMFREY, eItemAbil::WORMGRASS).withAbility(eItemAbil::AFFECT_STATUS, 2, eStatus::HASTE_SLOW).finish();
cAlchemy A_POISON_MED = cAlchemy(eAlchemy::POISON_MED).withDifficulty(3).withValue(50)
	.withIngredient(eItemAbil::WORMGRASS).withAbility(eItemAbil::POISON_WEAPON, 4).finish();

cAlchemy A_HEAL_MED = cAlchemy(eAlchemy::HEAL_MED).withDifficulty(4).withValue(180)
	.withIngredient(eItemAbil::NETTLE).withAbility(eItemAbil::AFFECT_HEALTH, 5).finish();
cAlchemy A_CURE_STRONG = cAlchemy(eAlchemy::CURE_STRONG).withDifficulty(5).withValue(200)
	.withIngredient(eItemAbil::NETTLE).withAbility(eItemAbil::AFFECT_STATUS, 8, eStatus::POISON).finish();
cAlchemy A_SPEED_MED = cAlchemy(eAlchemy::SPEED_MED).withDifficulty(5).withValue(100)
	.withIngredients(eItemAbil::WORMGRASS, eItemAbil::NETTLE).withAbility(eItemAbil::AFFECT_STATUS, 5, eStatus::HASTE_SLOW).finish();
cAlchemy A_GRAYMOLD = cAlchemy(eAlchemy::GRAYMOLD).withDifficulty(7).withValue(150)
	.withIngredient(eItemAbil::GRAYMOLD).withAbility(eItemAbil::AFFECT_STATUS, 4, eStatus::DISEASE).withUse(eItemUse::HELP_ALL).finish();
cAlchemy A_POWER_WEAK = cAlchemy(eAlchemy::POWER_WEAK).withDifficulty(9).withValue(100)
	.withIngredients(eItemAbil::WORMGRASS, eItemAbil::ASPTONGUE).withAbility(eItemAbil::AFFECT_SPELL_POINTS, 2).finish();

cAlchemy A_CLARITY = cAlchemy(eAlchemy::CLARITY).withDifficulty(9).withValue(200)
	.withIngredients(eItemAbil::GRAYMOLD, eItemAbil::HOLLY).withAbility(eItemAbil::AFFECT_STATUS, 8, eStatus::DUMB).finish();
cAlchemy A_POISON_STRING = cAlchemy(eAlchemy::POISON_STRONG).withDifficulty(10).withValue(150)
	.withIngredient(eItemAbil::ASPTONGUE).withAbility(eItemAbil::POISON_WEAPON, 6).finish();
cAlchemy A_HEAL_STRONG = cAlchemy(eAlchemy::HEAL_STRONG).withDifficulty(12).withValue(300)
	.withIngredients(eItemAbil::GRAYMOLD, eItemAbil::COMFREY).withAbility(eItemAbil::AFFECT_HEALTH, 8).finish();
cAlchemy A_POISON_KILL = cAlchemy(eAlchemy::POISON_KILL).withDifficulty(12).withValue(400)
	.withIngredient(eItemAbil::MANDRAKE).withAbility(eItemAbil::POISON_WEAPON, 8).finish();
cAlchemy A_RESURRECT = cAlchemy(eAlchemy::RESURRECT).withDifficulty(9).withValue(100)
	.withIngredient(eItemAbil::EMBERF).withAbility(eItemAbil::RESURRECTION_BALM, 0).finish();

cAlchemy A_POWER_MED = cAlchemy(eAlchemy::POWER_MED).withDifficulty(14).withValue(300)
	.withIngredients(eItemAbil::MANDRAKE, eItemAbil::ASPTONGUE).withAbility(eItemAbil::AFFECT_SPELL_POINTS, 5).finish();
cAlchemy A_KNOWLEDGE = cAlchemy(eAlchemy::KNOWLEDGE).withDifficulty(19).withValue(500)
	.withIngredients(eItemAbil::MANDRAKE, eItemAbil::EMBERF).withAbility(eItemAbil::AFFECT_SKILL_POINTS, 2).finish();
cAlchemy A_STRENGTH = cAlchemy(eAlchemy::STRENGTH).withDifficulty(10).withValue(175)
	.withIngredients(eItemAbil::NETTLE, eItemAbil::EMBERF).withAbility(eItemAbil::AFFECT_STATUS, 8, eStatus::BLESS_CURSE).finish();
cAlchemy A_BLISS = cAlchemy(eAlchemy::BLISS).withDifficulty(16).withValue(250)
	.withIngredients(eItemAbil::GRAYMOLD, eItemAbil::ASPTONGUE).withAbility(eItemAbil::BLISS_DOOM, 5).finish();
cAlchemy A_POWER_STRONG = cAlchemy(eAlchemy::POWER_STRONG).withDifficulty(20).withValue(500)
	.withIngredients(eItemAbil::MANDRAKE, eItemAbil::EMBERF).withAbility(eItemAbil::AFFECT_SKILL_POINTS, 8).finish();
