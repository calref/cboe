//
//  enums.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-07-07.
//
//

#include <type_traits>
#include <boost/lexical_cast.hpp>
#include "catch.hpp"
#include "universe/universe.hpp"

using namespace std;
using boost::lexical_cast;

static void test_enum() {};

template<typename T, typename... Rest>
static void test_enum(T val, string sym, Rest... rest) {
	static_assert(is_enum<T>::value, "test_enum must be used on enum types");
	CAPTURE(val);
	CAPTURE(sym);
	CHECK(lexical_cast<string>(val) == sym);
	CAPTURE(val);
	CAPTURE(sym);
	CHECK(lexical_cast<T>(sym) == val);
	CAPTURE(val);
	CHECK(lexical_cast<T>(int(val)) == val);
	test_enum(rest...);
}

TEST_CASE("Enum text conversions") {
	test_enum(eSkill::STRENGTH, "str", eSkill::LUCK, "luck", eSkill::MAX_SP, "sp");
	test_enum(eItemType::NO_ITEM, "none", eItemType::QUEST, "quest");
	test_enum(eItemUse::HARM_ALL, "harm-all", eItemUse::HELP_ALL, "help-all", eItemUse::HARM_ONE, "harm-one", eItemUse::HELP_ONE, "help-one");
	test_enum(eItemAbil::NONE, "none", eItemAbil::SOULSUCKER, "weap-soulsuck", eItemAbil::WEAK_WEAPON, "weap-weak", eItemAbil::WEAPON_CALL_SPECIAL, "spec-weap", eItemAbil::DAMAGE_PROTECTION, "prot-dmg", eItemAbil::DRAIN_MISSILES, "missile-drain", eItemAbil::LOCKPICKS, "lockpick", eItemAbil::DROP_CALL_SPECIAL, "spec-drop", eItemAbil::POISON_WEAPON, "use-poison", eItemAbil::QUICKFIRE, "use-quickfire", eItemAbil::HOLLY, "holly", eItemAbil::RESURRECTION_BALM, "balm");
	test_enum(eStatus::POISONED_WEAPON, "poison-weap", eStatus::ACID, "acid", eStatus::FORCECAGE, "cage", eStatus::CHARM, "charm", eStatus::MAIN, "main");
	test_enum(eRace::HUMAN, "human", eRace::MAGICAL, "magic", eRace::GOBLIN, "goblin");
	test_enum(eMonstTime::ALWAYS, "always", eMonstTime::APPEAR_AFTER_CHOP, "after-death");
	test_enum(DIR_N, "n", DIR_HERE, "?");
	test_enum(SPECIAL_EXPLORED, "explored", BARRIER_CAGE, "barr-cage", FIELD_DISPEL, "dispel", FIELD_SMASH, "smash");
	test_enum(eDamageType::WEAPON, "weap", eDamageType::DEMON, "demon", eDamageType::SPECIAL, "spec");
	test_enum(eMonstAbil::NO_ABIL, "none", eMonstAbil::STUN, "stun", eMonstAbil::SUMMON, "summon");
	test_enum(eMonstGen::RAY, "ray", eMonstGen::SPIT, "spit");
	test_enum(eMonstMelee::SWING, "swing", eMonstMelee::STAB, "stab");
	test_enum(eMonstMissile::DART, "dart", eMonstMissile::RAPID_ARROW, "arrow++");
	test_enum(eMonstSummon::TYPE, "type", eMonstSummon::SPECIES, "race");
	test_enum(NOTE_SCEN, "SCEN", NOTE_OUT, "OUT", NOTE_TOWN, "TOWN");
	test_enum(ePartyStatus::STEALTH, "STEALTH", ePartyStatus::FIREWALK, "FIREWALK");
	test_enum(eQuestStatus::AVAILABLE, "avail", eQuestStatus::FAILED, "fail");
	test_enum(eMainStatus::ABSENT, "empty", eMainStatus::FLED, "fled", eMainStatus::SPLIT, "split", eMainStatus::SPLIT_FLED, "split-fled");
	test_enum(eShopType::NORMAL, "live", eShopType::ALLOW_DEAD, "dead", eShopType::RANDOM, "rand");
	test_enum(eShopPrompt::SHOPPING, "shop", eShopPrompt::TRAINING, "train");
	test_enum(eTerSpec::NONE, "none", eTerSpec::DANGEROUS, "danger", eTerSpec::CRUMBLING, "fragile", eTerSpec::UNLOCKABLE, "unlock", eTerSpec::IS_A_SIGN, "sign", eTerSpec::CALL_SPECIAL, "step-spec", eTerSpec::IS_A_CONTAINER, "box", eTerSpec::CALL_SPECIAL_WHEN_USED, "use-spec");
	test_enum(eTrimType::NONE, "none", eTrimType::CITY, "city");
	test_enum(eTerObstruct::CLEAR, "none", eTerObstruct::BLOCK_MOVE_AND_SIGHT, "move-and-sight");
	test_enum(eStepSnd::STEP, "step", eStepSnd::SPLASH, "splash");
	test_enum(LIGHT_NORMAL, "lit", LIGHT_NONE, "none");
	test_enum(eTalkNode::REGULAR, "reg", eTalkNode::JOB_BANK, "jobs", eTalkNode::SELL_WEAPONS, "sell-weap", eTalkNode::CALL_SCEN_SPEC, "call-global");
	test_enum(eAttitude::DOCILE, "docile", eAttitude::HOSTILE_B, "hostile-b");
	test_enum(eContentRating::G, "G", eContentRating::NC17, "NC-17");
}
