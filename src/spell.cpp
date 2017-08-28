//
//  spell.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-12.
//
//

#include "spell.hpp"

#include "utility.hpp"

std::map<eSpell,cSpell> cSpell::dictionary;

cSpell::cSpell(eSpell id) : num(id), refer(REFER_YES), cost(0), range(0), level(0), need_select(SELECT_NO), type(eSkill::INVALID), when_cast(0) {
	
}

cSpell& cSpell::withRefer(eSpellRefer r) {
	refer = r;
	return *this;
}

cSpell& cSpell::withCost(int c) {
	cost = c;
	return *this;
}

cSpell& cSpell::withRange(int r) {
	range = r;
	return *this;
}

cSpell& cSpell::asLevel(int lvl) {
	level = lvl;
	return *this;
}

cSpell& cSpell::asType(eSkill t) {
	type = t;
	return *this;
}

cSpell& cSpell::needsSelect(eSpellSelect sel) {
	need_select = sel;
	peaceful = true;
	return *this;
}

cSpell& cSpell::asPeaceful() {
	peaceful = true;
	return *this;
}

cSpell& cSpell::when(eSpellWhen when) {
	when_cast |= when;
	return *this;
}

const cSpell& cSpell::finish() {
	dictionary[num] = *this;
	return *this;
}

std::string cSpell::name() const {
	if(num == eSpell::NONE) return "INVALID SPELL";
	return get_str("magic-names", int(num) + 1);
}

bool cSpell::is_priest() const {
	// This is used to determine where the spell is implemented, not which skill is required.
	return int(num) >= 100;
}

const cSpell& operator*(eSpell spell_num) {
	return cSpell::dictionary[spell_num];
}

eSpell cSpell::fromNum(eSkill type, int num) {
	if(num < 0 || num >= 62) return eSpell::NONE;
	if(type == eSkill::MAGE_SPELLS)
		return eSpell(num);
	else if(type == eSkill::PRIEST_SPELLS)
		return eSpell(num + 100);
	return eSpell::NONE;
}

eSpell cSpell::fromNum(int num) {
	eSpell check = eSpell(num);
	if(dictionary.find(check) == dictionary.end())
		return eSpell::NONE;
	return check;
}

// Mage Spells
cSpell M_LIGHT = cSpell(eSpell::LIGHT).asType(eSkill::MAGE_SPELLS).asLevel(1)
	.withCost(1).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
cSpell M_SPARK = cSpell(eSpell::SPARK).asType(eSkill::MAGE_SPELLS).asLevel(1)
	.withRange(6).withCost(1).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_HASTE_MINOR = cSpell(eSpell::HASTE_MINOR).asType(eSkill::MAGE_SPELLS).when(WHEN_COMBAT).asLevel(1)
	.withCost(1).needsSelect().withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell M_STRENGTH = cSpell(eSpell::STRENGTH).asType(eSkill::MAGE_SPELLS).asLevel(1)
	.withCost(1).needsSelect().withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell M_SCARE = cSpell(eSpell::SCARE).asType(eSkill::MAGE_SPELLS).asLevel(1)
	.withRange(7).withCost(1).withRefer(REFER_TARGET).when(WHEN_COMBAT).asPeaceful().finish();
cSpell M_CLOUD_FLAME = cSpell(eSpell::CLOUD_FLAME).asType(eSkill::MAGE_SPELLS).asLevel(1)
	.withRange(7).withCost(2).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_IDENTIFY = cSpell(eSpell::IDENTIFY).asType(eSkill::MAGE_SPELLS).asLevel(1)
	.withCost(50).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
cSpell M_SCRY_MONSTER = cSpell(eSpell::SCRY_MONSTER).asType(eSkill::MAGE_SPELLS).asLevel(1)
	.withRange(14).withCost(2).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).asPeaceful().finish();
cSpell M_GOO = cSpell(eSpell::GOO).asType(eSkill::MAGE_SPELLS).asLevel(1)
	.withRange(8).withCost(1).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_TRUE_SIGHT = cSpell(eSpell::TRUE_SIGHT).asType(eSkill::MAGE_SPELLS).asLevel(1)
	.withCost(3).when(WHEN_TOWN).asPeaceful().finish();

cSpell M_POISON_MINOR = cSpell(eSpell::POISON_MINOR).asType(eSkill::MAGE_SPELLS).asLevel(2)
	.withRange(6).withCost(2).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_FLAME = cSpell(eSpell::FLAME).asType(eSkill::MAGE_SPELLS).asLevel(2)
	.withRange(8).withCost(3).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_SLOW = cSpell(eSpell::SLOW).asType(eSkill::MAGE_SPELLS).asLevel(2)
	.withRange(7).withCost(2).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_DUMBFOUND = cSpell(eSpell::DUMBFOUND).asType(eSkill::MAGE_SPELLS).asLevel(2)
	.withRange(10).withCost(2).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_ENVENOM = cSpell(eSpell::ENVENOM).asType(eSkill::MAGE_SPELLS).asLevel(2)
	.withCost(2).needsSelect().withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell M_CLOUD_STINK = cSpell(eSpell::CLOUD_STINK).asType(eSkill::MAGE_SPELLS).asLevel(2)
	.withRange(8).withCost(2).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_SUMMON_BEAST = cSpell(eSpell::SUMMON_BEAST).asType(eSkill::MAGE_SPELLS).asLevel(2)
	.withRange(3).withCost(4).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell M_CONFLAGRATION = cSpell(eSpell::CONFLAGRATION).asType(eSkill::MAGE_SPELLS).asLevel(2)
	.withRange(8).withCost(4).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_DISPEL_SQUARE = cSpell(eSpell::DISPEL_SQUARE).asType(eSkill::MAGE_SPELLS).asLevel(2)
	.withRange(10).withCost(2).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).asPeaceful().finish();
cSpell M_CLOUD_SLEEP = cSpell(eSpell::CLOUD_SLEEP).asType(eSkill::MAGE_SPELLS).asLevel(2)
	.withRange(6).withCost(6).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();

cSpell M_UNLOCK = cSpell(eSpell::UNLOCK).asType(eSkill::MAGE_SPELLS).asLevel(3)
	.withCost(3).when(WHEN_TOWN).asPeaceful().finish();
cSpell M_HASTE = cSpell(eSpell::HASTE).asType(eSkill::MAGE_SPELLS).asLevel(3)
	.withCost(3).needsSelect().withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell M_FIREBALL = cSpell(eSpell::FIREBALL).asType(eSkill::MAGE_SPELLS).asLevel(3)
	.withRange(12).withCost(5).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_LIGHT_LONG = cSpell(eSpell::LIGHT_LONG).asType(eSkill::MAGE_SPELLS).asLevel(3)
	.withCost(3).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
// TODO: ^ Is it right for long light to be castable outdoors?
cSpell M_FEAR = cSpell(eSpell::FEAR).asType(eSkill::MAGE_SPELLS).asLevel(3)
	.withRange(10).withCost(3).withRefer(REFER_TARGET).when(WHEN_COMBAT).asPeaceful().finish();
cSpell M_WALL_FORCE = cSpell(eSpell::WALL_FORCE).asType(eSkill::MAGE_SPELLS).asLevel(3)
	.withRange(12).withCost(5).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_SUMMON_WEAK = cSpell(eSpell::SUMMON_WEAK).asType(eSkill::MAGE_SPELLS).asLevel(3)
	.withRange(4).withCost(6).withRefer(REFER_FANCY).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell M_ARROWS_FLAME = cSpell(eSpell::ARROWS_FLAME).asType(eSkill::MAGE_SPELLS).asLevel(3)
	.withRange(10).withCost(4).withRefer(REFER_FANCY).when(WHEN_COMBAT).finish();
cSpell M_WEB = cSpell(eSpell::WEB).asType(eSkill::MAGE_SPELLS).asLevel(3)
	.withRange(8).withCost(6).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_RESIST_MAGIC = cSpell(eSpell::RESIST_MAGIC).asType(eSkill::MAGE_SPELLS).asLevel(3)
	.withCost(4).needsSelect().withRefer(REFER_IMMED).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();

cSpell M_POISON = cSpell(eSpell::POISON).asType(eSkill::MAGE_SPELLS).asLevel(4)
	.withRange(8).withCost(4).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_ICE_BOLT = cSpell(eSpell::ICE_BOLT).asType(eSkill::MAGE_SPELLS).asLevel(4)
	.withRange(12).withCost(5).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_SLOW_GROUP = cSpell(eSpell::SLOW_GROUP).asType(eSkill::MAGE_SPELLS).asLevel(4)
	.withRange(12).withCost(4).withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell M_MAGIC_MAP = cSpell(eSpell::MAGIC_MAP).asType(eSkill::MAGE_SPELLS).asLevel(4)
	.withCost(8).when(WHEN_TOWN).asPeaceful().finish();
cSpell M_CAPTURE_SOUL = cSpell(eSpell::CAPTURE_SOUL).asType(eSkill::MAGE_SPELLS).asLevel(4)
	.withRange(10).withCost(30).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell M_SIMULACRUM = cSpell(eSpell::SIMULACRUM).asType(eSkill::MAGE_SPELLS).asLevel(4)
	.withRange(4).withCost(-1).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_ARROWS_VENOM = cSpell(eSpell::ARROWS_VENOM).asType(eSkill::MAGE_SPELLS).asLevel(4)
	.withRange(8).withCost(8).withRefer(REFER_FANCY).when(WHEN_COMBAT).finish();
cSpell M_WALL_ICE = cSpell(eSpell::WALL_ICE).asType(eSkill::MAGE_SPELLS).asLevel(4)
	.withRange(8).withCost(6).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();

cSpell M_STEALTH = cSpell(eSpell::STEALTH).asType(eSkill::MAGE_SPELLS).asLevel(5)
	.withCost(5).when(WHEN_TOWN).asPeaceful().finish();
cSpell M_HASTE_MAJOR = cSpell(eSpell::HASTE_MAJOR).asType(eSkill::MAGE_SPELLS).asLevel(5)
	.withCost(8).withRefer(REFER_IMMED).when(WHEN_COMBAT).asPeaceful().finish();
cSpell M_FIRESTORM = cSpell(eSpell::FIRESTORM).asType(eSkill::MAGE_SPELLS).asLevel(5)
	.withRange(14).withCost(8).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_DISPEL_BARRIER = cSpell(eSpell::DISPEL_BARRIER).asType(eSkill::MAGE_SPELLS).asLevel(5)
	.withCost(6).when(WHEN_TOWN).asPeaceful().finish();
cSpell M_BARRIER_FIRE = cSpell(eSpell::BARRIER_FIRE).asType(eSkill::MAGE_SPELLS).asLevel(5)
	.withRange(2).withCost(9).withRefer(REFER_TARGET).when(WHEN_TOWN).when(WHEN_COMBAT).finish();
cSpell M_SUMMON = cSpell(eSpell::SUMMON).asType(eSkill::MAGE_SPELLS).asLevel(5)
	.withRange(4).withCost(10).withRefer(REFER_FANCY).when(WHEN_TOWN).when(WHEN_COMBAT).finish();
cSpell M_SHOCKSTORM = cSpell(eSpell::SHOCKSTORM).asType(eSkill::MAGE_SPELLS).asLevel(5)
	.withRange(10).withCost(6).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_SPRAY_FIELDS = cSpell(eSpell::SPRAY_FIELDS).asType(eSkill::MAGE_SPELLS).asLevel(5)
	.withRange(12).withCost(6).withRefer(REFER_FANCY).when(WHEN_COMBAT).finish();

cSpell M_POISON_MAJOR = cSpell(eSpell::POISON_MAJOR).asType(eSkill::MAGE_SPELLS).asLevel(6)
	.withRange(8).withCost(7).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_FEAR_GROUP = cSpell(eSpell::FEAR_GROUP).asType(eSkill::MAGE_SPELLS).asLevel(6)
	.withRange(12).withCost(6).withRefer(REFER_IMMED).when(WHEN_COMBAT).asPeaceful().finish();
cSpell M_KILL = cSpell(eSpell::KILL).asType(eSkill::MAGE_SPELLS).asLevel(6)
	.withRange(6).withCost(8).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_PARALYZE = cSpell(eSpell::PARALYZE).asType(eSkill::MAGE_SPELLS).asLevel(6)
	.withRange(8).withCost(7).withRefer(REFER_FANCY).when(WHEN_COMBAT).finish();
cSpell M_DEMON = cSpell(eSpell::DEMON).asType(eSkill::MAGE_SPELLS).asLevel(6)
	.withRange(5).withCost(12).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell M_ANTIMAGIC = cSpell(eSpell::ANTIMAGIC).asType(eSkill::MAGE_SPELLS).asLevel(6)
	.withRange(8).withCost(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell M_MINDDUEL = cSpell(eSpell::MINDDUEL).asType(eSkill::MAGE_SPELLS).asLevel(6)
	.withRange(40).withCost(12).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell M_FLIGHT = cSpell(eSpell::FLIGHT).asType(eSkill::MAGE_SPELLS).asLevel(6)
	.withCost(20).when(WHEN_OUTDOORS).asPeaceful().finish();

cSpell M_SHOCKWAVE = cSpell(eSpell::SHOCKWAVE).asType(eSkill::MAGE_SPELLS).asLevel(7)
	.withCost(12).withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell M_BLESS_MAJOR = cSpell(eSpell::BLESS_MAJOR).asType(eSkill::MAGE_SPELLS).asLevel(7)
	.withCost(8).withRefer(REFER_IMMED).when(WHEN_COMBAT).when(WHEN_TOWN).asPeaceful().finish();
cSpell M_PARALYSIS_MASS = cSpell(eSpell::PARALYSIS_MASS).asType(eSkill::MAGE_SPELLS).asLevel(7)
	.withRange(8).withCost(20).withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell M_PROTECTION = cSpell(eSpell::PROTECTION).asType(eSkill::MAGE_SPELLS).asLevel(7)
	.withCost(10).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell M_SUMMON_MAJOR = cSpell(eSpell::SUMMON_MAJOR).asType(eSkill::MAGE_SPELLS).asLevel(7)
	.withRange(4).withCost(14).withRefer(REFER_FANCY).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell M_BARRIER_FORCE = cSpell(eSpell::BARRIER_FORCE).asType(eSkill::MAGE_SPELLS).asLevel(7)
	.withRange(2).withCost(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).asPeaceful().finish();
cSpell M_QUICKFIRE = cSpell(eSpell::QUICKFIRE).asType(eSkill::MAGE_SPELLS).asLevel(7)
	.withRange(4).withCost(50).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell M_ARROWS_DEATH = cSpell(eSpell::ARROWS_DEATH).asType(eSkill::MAGE_SPELLS).asLevel(7)
	.withRange(6).withCost(10).withRefer(REFER_FANCY).when(WHEN_COMBAT).finish();

// Priest spells
cSpell P_BLESS_MINOR = cSpell(eSpell::BLESS_MINOR).asType(eSkill::PRIEST_SPELLS).asLevel(1)
	.withCost(1).needsSelect().withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell P_HEAL_MINOR = cSpell(eSpell::HEAL_MINOR).asType(eSkill::PRIEST_SPELLS).asLevel(1)
	.withCost(1).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_POISON_WEAKEN = cSpell(eSpell::POISON_WEAKEN).asType(eSkill::PRIEST_SPELLS).asLevel(1)
	.withCost(1).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_TURN_UNDEAD = cSpell(eSpell::TURN_UNDEAD).asType(eSkill::PRIEST_SPELLS).asLevel(1)
	.withRange(8).withCost(2).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_LOCATION = cSpell(eSpell::LOCATION).asType(eSkill::PRIEST_SPELLS).asLevel(1)
	.withCost(1).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
cSpell P_SANCTUARY = cSpell(eSpell::SANCTUARY).asType(eSkill::PRIEST_SPELLS).asLevel(1)
	.withCost(1).needsSelect().when(WHEN_COMBAT).finish();
cSpell P_SYMBIOSIS = cSpell(eSpell::SYMBIOSIS).asType(eSkill::PRIEST_SPELLS).asLevel(1)
	.withCost(3).needsSelect().when(WHEN_COMBAT).finish();
cSpell P_MANNA_MINOR = cSpell(eSpell::MANNA_MINOR).asType(eSkill::PRIEST_SPELLS).asLevel(1)
	.withCost(5).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
cSpell P_RITUAL_SANCTIFY = cSpell(eSpell::RITUAL_SANCTIFY).asType(eSkill::PRIEST_SPELLS).asLevel(1)
	.withCost(50).when(WHEN_TOWN).asPeaceful().finish();
cSpell P_STUMBLE = cSpell(eSpell::STUMBLE).asType(eSkill::PRIEST_SPELLS).asLevel(1)
	.withRange(10).withCost(1).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();

cSpell P_BLESS = cSpell(eSpell::BLESS).asType(eSkill::PRIEST_SPELLS).asLevel(2)
	.withCost(2).needsSelect().withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell P_POISON_CURE = cSpell(eSpell::POISON_CURE).asType(eSkill::PRIEST_SPELLS).asLevel(2)
	.withCost(2).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_CURSE = cSpell(eSpell::CURSE).asType(eSkill::PRIEST_SPELLS).asLevel(2)
	.withRange(10).withCost(2).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_LIGHT_DIVINE = cSpell(eSpell::LIGHT_DIVINE).asType(eSkill::PRIEST_SPELLS).asLevel(2)
	.withCost(2).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
// TODO: ^ Is it right for light to be castable outdoors?
cSpell P_WOUND = cSpell(eSpell::WOUND).asType(eSkill::PRIEST_SPELLS).asLevel(2)
	.withRange(6).withCost(3).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_SUMMON_SPIRIT = cSpell(eSpell::SUMMON_SPIRIT).asType(eSkill::PRIEST_SPELLS).asLevel(2)
	.withRange(4).withCost(5).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell P_MOVE_MOUNTAINS = cSpell(eSpell::MOVE_MOUNTAINS).asType(eSkill::PRIEST_SPELLS).asLevel(2)
	.withCost(8).when(WHEN_TOWN).asPeaceful().finish();
cSpell P_CHARM_FOE = cSpell(eSpell::CHARM_FOE).asType(eSkill::PRIEST_SPELLS).asLevel(2)
	.withRange(6).withCost(6).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_DISEASE = cSpell(eSpell::DISEASE).asType(eSkill::PRIEST_SPELLS).asLevel(2)
	.withRange(6).withCost(4).withRefer(REFER_FANCY).when(WHEN_COMBAT).finish();
cSpell P_AWAKEN = cSpell(eSpell::AWAKEN).asType(eSkill::PRIEST_SPELLS).asLevel(2)
	.withCost(2).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
// Note: Awaken used to have a range of 8, but I removed it since it doesn't seem to be used (seems to be a relic of something else).

cSpell P_HEAL = cSpell(eSpell::HEAL).asType(eSkill::PRIEST_SPELLS).asLevel(3)
	.withCost(3).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_HEAL_ALL_LIGHT = cSpell(eSpell::HEAL_ALL_LIGHT).asType(eSkill::PRIEST_SPELLS).asLevel(3)
	.withCost(4).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
cSpell P_HOLY_SCOURGE = cSpell(eSpell::HOLY_SCOURGE).asType(eSkill::PRIEST_SPELLS).asLevel(3)
	.withRange(8).withCost(3).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_DETECT_LIFE = cSpell(eSpell::DETECT_LIFE).asType(eSkill::PRIEST_SPELLS).asLevel(3)
	.withCost(3).when(WHEN_TOWN).asPeaceful().finish();
cSpell P_PARALYSIS_CURE = cSpell(eSpell::PARALYSIS_CURE).asType(eSkill::PRIEST_SPELLS).asLevel(3)
	.withCost(3).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
// Note: Cure paralysis used to have a range of 10; I removed it because it doesn't seem to be used and doesn't make sense.
cSpell P_MANNA = cSpell(eSpell::MANNA).asType(eSkill::PRIEST_SPELLS).asLevel(3)
	.withCost(10).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
cSpell P_FORCEFIELD = cSpell(eSpell::FORCEFIELD).asType(eSkill::PRIEST_SPELLS).asLevel(3)
	.withRange(8).withCost(5).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_DISEASE_CURE = cSpell(eSpell::DISEASE_CURE).asType(eSkill::PRIEST_SPELLS).asLevel(3)
	.withCost(3).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_RESTORE_MIND = cSpell(eSpell::RESTORE_MIND).asType(eSkill::PRIEST_SPELLS).asLevel(3)
	.withCost(4).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_SMITE = cSpell(eSpell::SMITE).asType(eSkill::PRIEST_SPELLS).asLevel(3)
	.withRange(8).withCost(6).withRefer(REFER_FANCY).when(WHEN_COMBAT).finish();

cSpell P_POISON_CURE_ALL = cSpell(eSpell::POISON_CURE_ALL).asType(eSkill::PRIEST_SPELLS).asLevel(4)
	.withCost(5).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
cSpell P_CURSE_ALL = cSpell(eSpell::CURSE_ALL).asType(eSkill::PRIEST_SPELLS).asLevel(4)
	.withRange(10).withCost(5).withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell P_DISPEL_UNDEAD = cSpell(eSpell::DISPEL_UNDEAD).asType(eSkill::PRIEST_SPELLS).asLevel(4)
	.withRange(8).withCost(5).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_CURSE_REMOVE = cSpell(eSpell::CURSE_REMOVE).asType(eSkill::PRIEST_SPELLS).asLevel(4)
	.withCost(15).needsSelect().when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_STICKS_TO_SNAKES = cSpell(eSpell::STICKS_TO_SNAKES).asType(eSkill::PRIEST_SPELLS).asLevel(4)
	.withRange(6).withCost(6).withRefer(REFER_FANCY).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell P_MARTYRS_SHIELD = cSpell(eSpell::MARTYRS_SHIELD).asType(eSkill::PRIEST_SPELLS).asLevel(4)
	.withCost(5).needsSelect().when(WHEN_COMBAT).finish();
cSpell P_CLEANSE = cSpell(eSpell::CLEANSE).asType(eSkill::PRIEST_SPELLS).asLevel(4)
	.withCost(5).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_FIREWALK = cSpell(eSpell::FIREWALK).asType(eSkill::PRIEST_SPELLS).asLevel(4)
	.withCost(8).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();

cSpell P_BLESS_PARTY = cSpell(eSpell::BLESS_PARTY).asType(eSkill::PRIEST_SPELLS).asLevel(5)
	.withCost(6).withRefer(REFER_IMMED).when(WHEN_COMBAT).asPeaceful().finish();
cSpell P_HEAL_MAJOR = cSpell(eSpell::HEAL_MAJOR).asType(eSkill::PRIEST_SPELLS).asLevel(5)
	.withCost(7).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_RAISE_DEAD = cSpell(eSpell::RAISE_DEAD).asType(eSkill::PRIEST_SPELLS).asLevel(5)
	.withCost(25).needsSelect(SELECT_ANY).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_FLAMESTRIKE = cSpell(eSpell::FLAMESTRIKE).asType(eSkill::PRIEST_SPELLS).asLevel(5)
	.withRange(9).withCost(8).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_SANCTUARY_MASS = cSpell(eSpell::SANCTUARY_MASS).asType(eSkill::PRIEST_SPELLS).asLevel(5)
	.withCost(10).when(WHEN_COMBAT).asPeaceful().finish();
cSpell P_SUMMON_HOST = cSpell(eSpell::SUMMON_HOST).asType(eSkill::PRIEST_SPELLS).asLevel(5)
	.withRange(4).withCost(12).withRefer(REFER_FANCY).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell P_SHATTER = cSpell(eSpell::SHATTER).asType(eSkill::PRIEST_SPELLS).asLevel(5)
	.withCost(12).when(WHEN_TOWN).asPeaceful().finish();
cSpell P_DISPEL_SPHERE = cSpell(eSpell::DISPEL_SPHERE).asType(eSkill::PRIEST_SPELLS).asLevel(5)
	.withRange(8).withCost(6).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).asPeaceful().finish();

cSpell P_HEAL_ALL = cSpell(eSpell::HEAL_ALL).asType(eSkill::PRIEST_SPELLS).asLevel(6)
	.withCost(8).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
cSpell P_REVIVE = cSpell(eSpell::REVIVE).asType(eSkill::PRIEST_SPELLS).asLevel(6)
	.withCost(7).needsSelect().when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_HYPERACTIVITY = cSpell(eSpell::HYPERACTIVITY).asType(eSkill::PRIEST_SPELLS).asLevel(6)
	.withCost(8).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
// Note: Hyperactivity used to have a range of 10; I removed it because it doesn't seem to be used and doesn't make sense.
cSpell P_DESTONE = cSpell(eSpell::DESTONE).asType(eSkill::PRIEST_SPELLS).asLevel(6)
	.withCost(8).needsSelect(SELECT_ANY).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_SUMMON_GUARDIAN = cSpell(eSpell::SUMMON_GUARDIAN).asType(eSkill::PRIEST_SPELLS).asLevel(6)
	.withRange(4).withCost(14).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell P_CHARM_MASS = cSpell(eSpell::CHARM_MASS).asType(eSkill::PRIEST_SPELLS).asLevel(6)
	.withRange(8).withCost(17).withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell P_PROTECTIVE_CIRCLE = cSpell(eSpell::PROTECTIVE_CIRCLE).asType(eSkill::PRIEST_SPELLS).asLevel(6)
	.withCost(8).withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell P_PESTILENCE = cSpell(eSpell::PESTILENCE).asType(eSkill::PRIEST_SPELLS).asLevel(6)
	.withRange(8).withCost(7).withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();

cSpell P_REVIVE_ALL = cSpell(eSpell::REVIVE_ALL).asType(eSkill::PRIEST_SPELLS).asLevel(7)
	.withCost(10).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();
cSpell P_RAVAGE_SPIRIT = cSpell(eSpell::RAVAGE_SPIRIT).asType(eSkill::PRIEST_SPELLS).asLevel(7)
	.withRange(4).withCost(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_RESURRECT = cSpell(eSpell::RESURRECT).asType(eSkill::PRIEST_SPELLS).asLevel(7)
	.withCost(35).needsSelect(SELECT_ANY).when(WHEN_TOWN).when(WHEN_OUTDOORS).finish();
cSpell P_DIVINE_THUD = cSpell(eSpell::DIVINE_THUD).asType(eSkill::PRIEST_SPELLS).asLevel(7)
	.withRange(12).withCost(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_AVATAR = cSpell(eSpell::AVATAR).asType(eSkill::PRIEST_SPELLS).asLevel(7)
	.withCost(12).withRefer(REFER_IMMED).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell P_WALL_BLADES = cSpell(eSpell::WALL_BLADES).asType(eSkill::PRIEST_SPELLS).asLevel(7)
	.withRange(10).withCost(12).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell P_WORD_RECALL = cSpell(eSpell::WORD_RECALL).asType(eSkill::PRIEST_SPELLS).asLevel(7)
	.withCost(30).when(WHEN_OUTDOORS).asPeaceful().finish();
cSpell P_CLEANSE_MAJOR = cSpell(eSpell::CLEANSE_MAJOR).asType(eSkill::PRIEST_SPELLS).asLevel(7)
	.withCost(10).when(WHEN_COMBAT).when(WHEN_TOWN).when(WHEN_OUTDOORS).asPeaceful().finish();

// Special spells
cSpell S_STRENGTHEN_TARGET = cSpell(eSpell::STRENGTHEN_TARGET)
	.withRange(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).asPeaceful().finish();
cSpell S_SUMMON_RAT = cSpell(eSpell::SUMMON_RAT)
	.withRange(8).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell S_WALL_ICE_BALL = cSpell(eSpell::WALL_ICE_BALL)
	.withRange(8).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_GOO_BOMB = cSpell(eSpell::GOO_BOMB)
	.withRange(12).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_FOUL_VAPOR = cSpell(eSpell::FOUL_VAPOR)
	.withRange(8).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_CLOUD_SLEEP_LARGE = cSpell(eSpell::CLOUD_SLEEP_LARGE)
	.withRange(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_ACID_SPRAY = cSpell(eSpell::ACID_SPRAY)
	.withRange(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_PARALYZE_BEAM = cSpell(eSpell::PARALYZE_BEAM)
	.withRange(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_SLEEP_MASS = cSpell(eSpell::SLEEP_MASS)
	.withRange(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_RAVAGE_ENEMIES = cSpell(eSpell::RAVAGE_ENEMIES)
	.withRange(12).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_BLADE_AURA = cSpell(eSpell::BLADE_AURA)
	.withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell S_DISPEL_FIELD = cSpell(eSpell::DISPEL_FIELD)
	.withRange(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).asPeaceful().finish();
cSpell S_MOVE_MOUNTAINS_MASS = cSpell(eSpell::MOVE_MOUNTAINS_MASS)
	.withRange(8).withRefer(REFER_TARGET).when(WHEN_TOWN).asPeaceful().finish();
// TODO: These two have a range of 10 only because all monster spells do (monster spells ignore official spell range)
// They should perhaps have better ranges assigned at some point.
cSpell S_WRACK = cSpell(eSpell::WRACK).asLevel(1)
	.withRange(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_UNHOLY_RAVAGING = cSpell(eSpell::UNHOLY_RAVAGING).asLevel(6)
	.withRange(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_ICY_RAIN = cSpell(eSpell::ICY_RAIN)
	.withRange(10).withRefer(REFER_TARGET).when(WHEN_COMBAT).finish();
cSpell S_FLAME_AURA = cSpell(eSpell::FLAME_AURA)
	.withRefer(REFER_IMMED).when(WHEN_COMBAT).finish();
cSpell S_SUMMON_AID = cSpell(eSpell::SUMMON_AID)
	.withRange(4).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell S_SUMMON_AID_MAJOR = cSpell(eSpell::SUMMON_AID_MAJOR)
	.withRange(4).withRefer(REFER_TARGET).when(WHEN_COMBAT).when(WHEN_TOWN).finish();
cSpell S_FLASH_STEP = cSpell(eSpell::FLASH_STEP)
	.withRange(8).withRefer(REFER_TARGET).when(WHEN_COMBAT).asPeaceful().finish();
cSpell S_AUGMENTATION = cSpell(eSpell::AUGMENTATION)
	.withRange(10).withRefer(REFER_IMMED).needsSelect().when(WHEN_COMBAT).finish();
cSpell S_NIRVANA = cSpell(eSpell::NIRVANA)
	.withRange(10).withRefer(REFER_IMMED).needsSelect().when(WHEN_COMBAT).finish();
