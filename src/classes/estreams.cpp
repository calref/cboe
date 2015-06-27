//
//  estreams.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-06-12.
//
//

#include <string>
#include <iostream>
#include <memory>
#include <queue>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_set.hpp>
#include "simpletypes.hpp"

// A simple lookup map based on the concept of a trie
class cEnumLookup {
	struct node {
		char c;
		mutable boost::ptr_set<node> next;
		mutable boost::optional<unsigned long> value;
		node(char c) : c(c) {}
		node(char c, unsigned long v) : c(c), value(v) {}
		bool find(unsigned long val, std::string& result) const {
			if(value && *value == val) {
				if(c >= ' ') result.push_back(c);
				return true;
			}
			for(const node& n : next) {
				if(n.find(val, result)) {
					if(c >= ' ') result.push_back(c);
					return true;
				}
			}
			return false;
		}
		friend bool operator<(const node& a, const node& b) {
			return a.c < b.c;
		}
	};
	node root;
	mutable size_t sz;
	mutable bool size_cached = false;
public:
	cEnumLookup(const std::initializer_list<const char*> strings) : root(0) {
		size_t i = 0;
		for(std::string str : strings) {
			if(!str.empty())
				insert(str, i);
			i++;
		}
	}
	void insert(const std::string& str, unsigned long val) {
		size_t i = 0;
		const node* cur = &root;
		while(i < str.size()) {
			auto check = cur->next.find(str[i]);
			if(check == cur->next.end())
				check = cur->next.insert(new node(str[i])).first;
			cur = &*check;
			i++;
		}
		if(cur->value)
			throw std::string("Duplicate value in enum lookup: " + str);
		cur->value = val;
		size_cached = false;
	}
	unsigned long get(const std::string& str, unsigned long def = 0) const {
		size_t i = 0;
		const node* cur = &root;
		while(i < str.size()) {
			auto check = cur->next.find(str[i]);
			if(check == cur->next.end())
				return def;
			cur = &*check;
			i++;
		}
		if(cur->value)
			return *cur->value;
		return def;
	}
	std::string find(unsigned long val, std::string def = "") const {
		std::string result;
		if(root.find(val,result)) {
			std::reverse(result.begin(), result.end());
			return result;
		}
		return def;
	}
	bool contains(unsigned long val) const {
		if(find(val).empty()) return false;
		return true;
	}
	size_t size() const {
		if(size_cached) return sz;
		sz = 0;
		std::queue<const node*> to_check;
		to_check.push(&root);
		while(!to_check.empty()) {
			const node* cur = to_check.front();
			to_check.pop();
			if(cur->value) sz++;
			for(const node& n : cur->next)
				to_check.push(&n);
		}
		size_cached = true;
		return sz;
	}
};

template<typename E> void writeEnum(std::ostream& out, E val, cEnumLookup& tbl, std::string def = "") {
	out << tbl.find(int(val), def);
}

template<typename E> void readEnum(std::istream& in, E& to, cEnumLookup& tbl, E def = E()) {
	std::string key;
	in >> key;
	bool have_num = true;
	for(char c : key) {
		if(!isdigit(c)) {
			have_num = false;
			break;
		}
	}
	if(have_num) {
		int n = boost::lexical_cast<int>(key);
		to = tbl.contains(n) ? E(n) : def;
	} else to = E(tbl.get(key, int(def)));
}

// These operators have their prototypes declared all over the place, but I'm not including those headers, so silence the warnings
#ifdef __clang__
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

// MARK: eSkill

cEnumLookup skill_names = {
	"str", "dex", "int", "edged", "bashing", "pole", "thrown", "archery", "defense",
	"mage", "priest", "mage-lore", "alchemy", "item-lore", "traps", "lockpick", "assassin", "poison", "luck",
};

std::ostream& operator << (std::ostream& out, eSkill e) {
	writeEnum(out, e, skill_names, "edged");
	return out;
}

std::istream& operator >> (std::istream& in, eSkill& e){
	readEnum(in, e, skill_names, eSkill::EDGED_WEAPONS);
	return in;
}

// MARK: eItemType

cEnumLookup item_types = {
	"none", "weapon-1hand", "weapon-2hand", "gold", "bow", "arrow", "thrown-missile", "potion", "scroll", "wand",
	"tool", "food", "shield", "armor", "helm", "gloves", "shield2", "boots", "ring", "necklace",
	"poison", "object", "pants", "crossbow", "bolts", "missile", "special", "quest",
};

std::ostream& operator << (std::ostream& out, eItemType e) {
	writeEnum(out, e, item_types, "none");
	return out;
}

std::istream& operator >> (std::istream& in, eItemType& e) {
	readEnum(in, e, item_types, eItemType::NO_ITEM);
	return in;
}

// MARK: eItemUse

cEnumLookup item_uses = {"help-one", "harm-one", "help-all", "harm-all"};

std::ostream& operator << (std::ostream& out, eItemUse e) {
	writeEnum(out, e, item_uses, "help-one");
	return out;
}

std::istream& operator >> (std::istream& in, eItemUse& e){
	readEnum(in, e, item_uses, eItemUse::HELP_ONE);
	return in;
}

// MARK: eItemAbil

cEnumLookup item_abils = {
	"none", "weap-dmg", "weap-slay", "weap-heal", "weap-explode",  "weap-return", "weap-dist", "weap-seek", "weap-antimagic", "weap-status",
	"weap-soulsuck", "", "weap-weak", "weap-fear", "spec-weap", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",
	"prot-dmg", "prot-full", "prot-melee", "evade", "martyr", "encumber", "prot-status", "skill", "boost-stat", "boost-war",
	"boost-magic", "accuracy", "thief", "giant", "light", "heavy", "status", "spec-hit", "save-life", "prot-petrify",
	"regen", "poison-aug", "radiant", "will", "freedom", "speed", "slow", "prot-race", "lockpick", "missile-drain",
	"spec-drop", "", "", "", "", "", "", "", "", "",
	"use-poison", "use-status", "use-spell", "bliss-doom", "use-xp", "use-skillpt", "use-hp", "use-sp", "use-light", "use-party-stat",
	"major-heal", "spec-use", "use-summon", "use-summon-mass", "use-quickfire", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",    "", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",    "", "", "", "", "", "", "", "", "", "",
	"", "", "", "", "", "", "", "", "", "",    "", "", "", "", "", "", "", "", "", "",
	"holly", "comfrey", "nettle", "wormgrass", "asptongue", "ember", "graymold", "mandrake", "sapphire", "smoky",
	"balm",
};

std::ostream& operator << (std::ostream& out, eItemAbil e) {
	writeEnum(out, e, item_abils, "none");
	return out;
}

std::istream& operator >> (std::istream& in, eItemAbil& e){
	readEnum(in, e, item_abils, eItemAbil::NONE);
	return in;
}

// MARK: eStatus

cEnumLookup pc_status = {
	"poison-weap", "bless-curse", "poison", "haste-slow", "magic", "web", "disease", "invis", "dumb-smart",
	"martyr", "sleep", "paralysis", "acid", "cage", "charm",
};

std::ostream& operator << (std::ostream& out, eStatus e){
	writeEnum(out, e, pc_status, "main");
	return out;
}

std::istream& operator >> (std::istream& in, eStatus& e){
	readEnum(in, e, pc_status, eStatus::MAIN);
	return in;
}

// MARK: eRace

cEnumLookup race_names = {
	"human", "nephil", "slith", "vahnatai", "reptile", "beast", "important", "mage", "priest", "humanoid",
	"demon", "undead", "giant", "slime", "stone", "bug", "dragon", "magic", "plant", "bird",
	"skeletal", "goblin",
};

std::ostream& operator << (std::ostream& out, eRace e){
	writeEnum(out, e, race_names, "humanoid");
	return out;
}

std::istream& operator >> (std::istream& in, eRace& e){
	readEnum(in, e, race_names, eRace::HUMANOID);
	return in;
}

// MARK: eMonstTime

cEnumLookup monst_times = {
	"always", "after-day", "until-day",
	"travel-a", "travel-b", "travel-c",
	"after-event", "until-event", "after-death",
};

std::ostream& operator << (std::ostream& out, eMonstTime e){
	writeEnum(out, e, monst_times, "always");
	return out;
}

std::istream& operator >> (std::istream& in, eMonstTime& e){
	readEnum(in, e, monst_times, eMonstTime::ALWAYS);
	return in;
}

// MARK: eDirection

cEnumLookup dirs = {"n", "ne", "e", "se", "s", "sw", "w", "nw", "?"};

std::ostream& operator<<(std::ostream& out, eDirection dir) {
	writeEnum(out, dir, dirs, "?");
	return out;
}

std::istream& operator>>(std::istream& in, eDirection& dir) {
	readEnum(in, dir, dirs, DIR_HERE);
	return in;
}

// MARK: eFieldType

cEnumLookup field_names = {
	"explored", "wall-force", "wall-fire", "field-antimagic", "cloud-stink", "wall-ice", "wall-blades", "cloud-sleep",
	"obj-block", "spec-spot", "field-web", "obj-crate", "obj-barrel", "barr-fire", "barr-force", "field-quickfire",
	"sfx-sm-bld", "sfx-med-bld", "sfx-lg-bld", "sfx-sm-slm", "sfx-lg-slm", "sfx-ash", "sfx-bone", "sfx-rock",
	"barr-cage", "", "", "", "", "", "", "",
	"dispel", "smash",
};

std::ostream& operator << (std::ostream& out, eFieldType e) {
	writeEnum(out, e, field_names, "dispel");
	return out;
}

std::istream& operator >> (std::istream& in, eFieldType& e) {
	readEnum(in, e, field_names, FIELD_DISPEL);
	return in;
}

// MARK: eDamageType

cEnumLookup dmg_names = {
	"weap", "fire", "poison", "magic", "unblockable", "cold", "undead", "demon", "spec",
};

std::ostream& operator << (std::ostream& out, eDamageType e) {
	writeEnum(out, e, dmg_names);
	return out;
}

std::istream& operator >> (std::istream& in, eDamageType& e) {
	readEnum(in, e, dmg_names, eDamageType::MARKED);
	return in;
}

// MARK: eMonstAbil

cEnumLookup monst_abils = {
	"none", "missile", "dmg", "status", "field", "petrify", "drain-sp", "drain-xp", "kill", "steal-food",
	"steal-gold", "stun", "dmg2", "status2", "splits", "martyr", "absorb", "old-web", "old-heat", "spec-act",
	"spec-hit", "spec-death", "radiate", "summon",
};

std::ostream& operator << (std::ostream& out, eMonstAbil e) {
	writeEnum(out, e, monst_abils, "none");
	return out;
}

std::istream& operator >> (std::istream& in, eMonstAbil& e) {
	readEnum(in, e, monst_abils, eMonstAbil::NO_ABIL);
	return in;
}

// MARK: eMonstGen

cEnumLookup monst_abil_types = {"ray", "touch", "gaze", "breath", "spit"};

std::ostream& operator << (std::ostream& out, eMonstGen e) {
	writeEnum(out, e, monst_abil_types, "touch");
	return out;
}

std::istream& operator >> (std::istream& in, eMonstGen& e) {
	readEnum(in, e, monst_abil_types, eMonstGen::TOUCH);
	return in;
}

// MARK: eMonstMelee

cEnumLookup monst_melee = {"swing", "claw", "bite", "slime", "punch", "sting", "club", "burn", "harm", "stab"};

std::ostream& operator << (std::ostream& out, eMonstMelee e) {
	writeEnum(out, e, monst_melee, "punch");
	return out;
}

std::istream& operator >> (std::istream& in, eMonstMelee& e) {
	readEnum(in, e, monst_melee, eMonstMelee::PUNCH);
	return in;
}

// MARK: eMonstMissile

cEnumLookup monst_missiles = {"dart", "arrow", "spear", "stone", "star", "spine", "knife", "bolt", "boulder", "arrow++"};

std::ostream& operator << (std::ostream& out, eMonstMissile e) {
	writeEnum(out, e, monst_missiles, "arrow");
	return out;
}

std::istream& operator >> (std::istream& in, eMonstMissile& e) {
	readEnum(in, e, monst_missiles, eMonstMissile::ARROW);
	return in;
}

// MARK: eMonstSummon

cEnumLookup monst_summons = {"type", "lvl", "race"};

std::ostream& operator << (std::ostream& out, eMonstSummon e) {
	writeEnum(out, e, monst_summons, "type");
	return out;
}

std::istream& operator >> (std::istream& in, eMonstSummon& e) {
	readEnum(in, e, monst_summons, eMonstSummon::TYPE);
	return in;
}

// MARK: eEncNoteType

cEnumLookup note_types = {"SCEN", "OUT", "TOWN"};

std::istream& operator>>(std::istream& in, eEncNoteType& type) {
	readEnum(in, type, note_types, NOTE_SCEN);
	return in;
}

std::ostream& operator<<(std::ostream& out, eEncNoteType type) {
	writeEnum(out, type, note_types, "SCEN");
	return out;
}

// MARK: ePartyStatus

cEnumLookup party_status = {"STEALTH", "FLIGHT", "DETECT", "FIREWALK"};

std::istream& operator>>(std::istream& in, ePartyStatus& type) {
	readEnum(in, type, party_status, ePartyStatus::STEALTH);
	return in;
}

std::ostream& operator<<(std::ostream& out, ePartyStatus type) {
	writeEnum(out, type, party_status, "STEALTH");
	return out;
}

// MARK: eQuestStatus

cEnumLookup quest_status = {"avail", "start", "done", "fail"};

std::istream& operator>>(std::istream& in, eQuestStatus& type) {
	readEnum(in, type, quest_status, eQuestStatus::AVAILABLE);
	return in;
}

std::ostream& operator<<(std::ostream& out, eQuestStatus type) {
	writeEnum(out, type, quest_status, "avail");
	return out;
}

// MARK: eMainStatus

cEnumLookup main_status = {
	"empty", "alive", "dead", "dust", "stone", "fled", "surface", "won", "", "",
	"split", "split-alive", "split-dead", "split-dust", "split-stone", "split-fled", "split-surface", "split-won",
};

std::ostream& operator << (std::ostream& out, eMainStatus e){
	writeEnum(out, e, main_status, "empty");
	return out;
}

std::istream& operator >> (std::istream& in, eMainStatus& e){
	readEnum(in, e, main_status, eMainStatus::ABSENT);
	return in;
}

// MARK: eShopType

cEnumLookup shop_types = {"live", "dead", "rand"};

std::istream& operator>>(std::istream& in, eShopType& type) {
	readEnum(in, type, shop_types, eShopType::NORMAL);
	return in;
}

std::ostream& operator<<(std::ostream& out, eShopType type) {
	writeEnum(out, type, shop_types, "live");
	return out;
}

// MARK: eShopPrompt

cEnumLookup shop_prompts = {"shop", "heal", "mage", "priest", "spell", "alch", "train"};

std::istream& operator>>(std::istream& in, eShopPrompt& type) {
	readEnum(in, type, shop_prompts, eShopPrompt::SHOPPING);
	return in;
}

std::ostream& operator<<(std::ostream& out, eShopPrompt type) {
	writeEnum(out, type, shop_prompts, "shop");
	return out;
}

// MARK: eTerSpec

cEnumLookup ter_types = {
	"none", "step-change", "dmg", "bridge", "bed", "danger", "", "fragile", "lock", "unlock",
	"", "sign", "step-spec", "", "box", "wild-cave", "wild-wood", "falls-cave", "falls-mntn", "belt",
	"monst-block", "town", "use-change", "use-spec",
};

std::ostream& operator << (std::ostream& out, eTerSpec e){
	writeEnum(out, e, ter_types, "none");
	return out;
}

std::istream& operator >> (std::istream& in, eTerSpec& e){
	readEnum(in, e, ter_types, eTerSpec::NONE);
	return in;
}

// MARK: eTrimType

cEnumLookup ter_trims = {
	"none", "wall", "s", "se", "e", "ne", "n", "nw", "w", "sw",
	"ne-inner", "se-inner", "sw-inner", "nw-inner", "frills", "road", "walkway", "waterfall", "city",
};

std::ostream& operator << (std::ostream& out, eTrimType e){
	writeEnum(out, e, ter_trims, "none");
	return out;
}

std::istream& operator >> (std::istream& in, eTrimType& e){
	readEnum(in, e, ter_trims, eTrimType::NONE);
	return in;
}

// MARK: eTerObstruct

cEnumLookup ter_blocks = {"none", "sight", "monsters", "move", "move-and-shoot", "move-and-sight"};

std::ostream& operator<< (std::ostream& out, eTerObstruct block) {
	writeEnum(out, block, ter_blocks, "none");
	return out;
}

std::istream& operator >> (std::istream& in, eTerObstruct& e){
	readEnum(in, e, ter_blocks, eTerObstruct::CLEAR);
	return in;
}

// MARK: eStepSnd

cEnumLookup step_snds = {"step", "squish", "crunch", "none", "splash"};

std::ostream& operator<< (std::ostream& out, eStepSnd snd) {
	writeEnum(out, snd, step_snds, "step");
	return out;
}

std::istream& operator >> (std::istream& in, eStepSnd& e){
	readEnum(in, e, step_snds, eStepSnd::STEP);
	return in;
}

// MARK: eLighting

cEnumLookup light_types = {"lit", "dark", "drains", "none"};

std::ostream& operator<< (std::ostream& out, eLighting light) {
	writeEnum(out, light, light_types, "lit");
	return out;
}

std::istream& operator>> (std::istream& in, eLighting& light) {
	readEnum(in, light, light_types, LIGHT_NORMAL);
	return in;
}
