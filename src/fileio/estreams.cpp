//
//  estreams.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-06-12.
//
//

#include <algorithm>
#include <string>
#include <iostream>
#include <memory>
#include <queue>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_set.hpp>

#include "spell.hpp"
#include "item_abilities.hpp"
#include "item_variety.hpp"
#include "damage.hpp"
#include "race.hpp"
#include "location.hpp"
#include "fields.hpp"
#include "quest.hpp"
#include "shop.hpp"
#include "terrain_abilities.hpp"

#include "monster.hpp"
#include "party.hpp"
#include "town.hpp"
#include "talking.hpp"
#include "scenario.hpp"

const char* oboeVersionString() {
	static std::shared_ptr<char> version;
	if(version) return version.get();
	unsigned short M, m, f;
	M = OBOE_CURRENT_VERSION >> 16;
	m = (OBOE_CURRENT_VERSION & 0xff00) >> 8;
	f = OBOE_CURRENT_VERSION & 0xff;
	std::ostringstream sout;
	sout << M << '.' << m << '.' << f;
	std::string vers = sout.str();
	version.reset(new char[vers.length()], std::default_delete<char[]>());
	std::copy(vers.begin(), vers.end(), version.get());
	return version.get();
}

// A simple lookup map based on the concept of a trie
class cEnumLookup {
	struct node {
		char c;
		mutable boost::ptr_set<node> next;
		mutable boost::optional<long long> value;
		node(char c) : c(c) {}
		node(char c, long long v) : c(c), value(v) {}
		bool find(long long val, std::string& result) const {
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
	void insert(const std::string& str, long long val) {
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
	unsigned long get(const std::string& str, long long def = 0) const {
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
	std::string find(long long val, std::string def = "") const {
		std::string result;
		if(root.find(val,result)) {
			return std::string(result.rbegin(), result.rend());
		}
		return def;
	}
	bool contains(long long val) const {
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

template<typename E> bool readEnum(std::istream& in, E& to, cEnumLookup& tbl, E def = E()) {
	std::string key;
	in >> key;
	if(in.fail() || key.empty()) return false;
	bool have_num = (isdigit(key[0]) || key[0] == '-') && std::all_of(key.begin() + 1, key.end(), isdigit);
	bool valid = true;
	if(have_num) {
		int n = boost::lexical_cast<int>(key);
		bool valid = tbl.contains(n);
		to = valid ? E(n) : def;
	} else {
		to = E(tbl.get(key, int(def)));
		if(to == def && key != tbl.find(int(def)))
			valid = false;
	}
	return valid;
}

// These operators have their prototypes declared all over the place, but I'm not including those headers, so silence the warnings
#ifdef __clang__
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

// MARK: eSkill

cEnumLookup skill_names = {
	"str", "dex", "int", "edged", "bashing", "pole", "thrown", "archery", "defense",
	"mage", "priest", "mage-lore", "alchemy", "item-lore", "traps", "lockpick", "assassin", "poison", "luck",
	"hp", "sp",
};

std::ostream& operator << (std::ostream& out, eSkill e) {
	writeEnum(out, e, skill_names, "edged");
	return out;
}

std::istream& operator >> (std::istream& in, eSkill& e){
	if(!readEnum(in, e, skill_names, eSkill::EDGED_WEAPONS))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eTrait

cEnumLookup trait_names = {
	"tough", "magic-apt", "ambidex", "nimble", "cave-lore", "wood-lore", "const", "alert",
	"strong", "regen", "slow", "magic-inept", "frail", "sickly", "bad-back", "pacifist", "anama"
};

std::ostream& operator << (std::ostream& out, eTrait e) {
	writeEnum(out, e, trait_names, "tough");
	return out;
}

std::istream& operator >> (std::istream& in, eTrait& e){
	if(!readEnum(in, e, trait_names, eTrait::TOUGHNESS))
		in.setstate(std::ios::failbit);
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
	if(!readEnum(in, e, item_types, eItemType::NO_ITEM))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eItemUse

cEnumLookup item_uses = {"help-one", "harm-one", "help-all", "harm-all"};

std::ostream& operator << (std::ostream& out, eItemUse e) {
	writeEnum(out, e, item_uses, "help-one");
	return out;
}

std::istream& operator >> (std::istream& in, eItemUse& e){
	if(!readEnum(in, e, item_uses, eItemUse::HELP_ONE))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eItemAbil

cEnumLookup item_abils = {
	"none", "weap-dmg", "weap-slay", "weap-heal", "weap-explode",  "weap-return", "weap-dist", "weap-seek", "weap-antimagic", "weap-status",
	"weap-soulsuck", "", "weap-weak", "weap-fear", "spec-weap", "hp-dmg", "hp-dmg-rev", "sp-dmg", "sp-dmg-rev", "",
	"", "", "", "", "", "", "", "", "", "",
	"prot-dmg", "prot-full", "magery", "evade", "martyr", "encumber", "prot-status", "skill", "boost-stat", "boost-war",
	"boost-magic", "accuracy", "thief", "giant", "light", "heavy", "status", "spec-hit", "save-life", "prot-petrify",
	"regen", "poison-aug", "radiant", "will", "freedom", "speed", "slow", "prot-race", "lockpick", "missile-drain",
	"spec-drop", "", "", "", "", "", "", "", "", "",
	"use-poison", "use-status", "use-spell", "bliss-doom", "use-xp", "use-skillpt", "use-hp", "use-sp", "use-light", "use-party-stat",
	"major-heal", "spec-use", "use-summon", "use-summon-mass", "use-quickfire", "use-read", "", "", "", "",
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
	if(!readEnum(in, e, item_abils, eItemAbil::NONE))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eStatus

cEnumLookup pc_status = {
	"poison-weap", "bless-curse", "poison", "haste-slow", "invuln", "magic", "web", "disease", "invis", "dumb-smart",
	"martyr", "sleep", "paralysis", "acid", "cage", "charm",
};

struct finish_status_init {
	finish_status_init() {
		pc_status.insert("main", -1);
	}
} finish_status_init;

std::ostream& operator << (std::ostream& out, eStatus e){
	writeEnum(out, e, pc_status, "main");
	return out;
}

std::istream& operator >> (std::istream& in, eStatus& e){
	if(!readEnum(in, e, pc_status, eStatus::MAIN))
		in.setstate(std::ios::failbit);
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
	if(!readEnum(in, e, race_names, eRace::HUMANOID))
		in.setstate(std::ios::failbit);
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
	if(!readEnum(in, e, monst_times, eMonstTime::ALWAYS))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eDirection

cEnumLookup dirs = {"n", "ne", "e", "se", "s", "sw", "w", "nw", "?"};

std::ostream& operator<<(std::ostream& out, eDirection dir) {
	writeEnum(out, dir, dirs, "?");
	return out;
}

std::istream& operator>>(std::istream& in, eDirection& dir) {
	if(!readEnum(in, dir, dirs, DIR_HERE))
		in.setstate(std::ios::failbit);
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
	if(!readEnum(in, e, field_names, FIELD_DISPEL))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eDamageType

cEnumLookup dmg_names = {
	"weap", "fire", "poison", "magic", "weird", "cold", "undead", "demon", "spec",
};

std::ostream& operator << (std::ostream& out, eDamageType e) {
	writeEnum(out, e, dmg_names);
	return out;
}

std::istream& operator >> (std::istream& in, eDamageType& e) {
	if(!readEnum(in, e, dmg_names, eDamageType::MARKED))
		in.setstate(std::ios::failbit);
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
	if(!readEnum(in, e, monst_abils, eMonstAbil::NO_ABIL))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eMonstGen

cEnumLookup monst_abil_types = {"ray", "touch", "gaze", "breath", "spit"};

std::ostream& operator << (std::ostream& out, eMonstGen e) {
	writeEnum(out, e, monst_abil_types, "touch");
	return out;
}

std::istream& operator >> (std::istream& in, eMonstGen& e) {
	if(!readEnum(in, e, monst_abil_types, eMonstGen::TOUCH))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eMonstMelee

cEnumLookup monst_melee = {"swing", "claw", "bite", "slime", "punch", "sting", "club", "burn", "harm", "stab"};

std::ostream& operator << (std::ostream& out, eMonstMelee e) {
	writeEnum(out, e, monst_melee, "punch");
	return out;
}

std::istream& operator >> (std::istream& in, eMonstMelee& e) {
	if(!readEnum(in, e, monst_melee, eMonstMelee::PUNCH))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eMonstMissile

cEnumLookup monst_missiles = {"dart", "arrow", "spear", "stone", "star", "spine", "knife", "bolt", "boulder", "arrow++"};

std::ostream& operator << (std::ostream& out, eMonstMissile e) {
	writeEnum(out, e, monst_missiles, "arrow");
	return out;
}

std::istream& operator >> (std::istream& in, eMonstMissile& e) {
	if(!readEnum(in, e, monst_missiles, eMonstMissile::ARROW))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eMonstSummon

cEnumLookup monst_summons = {"type", "lvl", "race"};

std::ostream& operator << (std::ostream& out, eMonstSummon e) {
	writeEnum(out, e, monst_summons, "type");
	return out;
}

std::istream& operator >> (std::istream& in, eMonstSummon& e) {
	if(!readEnum(in, e, monst_summons, eMonstSummon::TYPE))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eEncNoteType

cEnumLookup note_types = {"SCEN", "OUT", "TOWN"};

std::istream& operator>>(std::istream& in, eEncNoteType& type) {
	if(!readEnum(in, type, note_types, NOTE_SCEN))
		in.setstate(std::ios::failbit);
	return in;
}

std::ostream& operator<<(std::ostream& out, eEncNoteType type) {
	writeEnum(out, type, note_types, "SCEN");
	return out;
}

// MARK: ePartyStatus

cEnumLookup party_status = {"STEALTH", "FLIGHT", "DETECT", "FIREWALK"};

std::istream& operator>>(std::istream& in, ePartyStatus& type) {
	if(!readEnum(in, type, party_status, ePartyStatus::STEALTH))
		in.setstate(std::ios::failbit);
	return in;
}

std::ostream& operator<<(std::ostream& out, ePartyStatus type) {
	writeEnum(out, type, party_status, "STEALTH");
	return out;
}

// MARK: eQuestStatus

cEnumLookup quest_status = {"avail", "start", "done", "fail"};

std::istream& operator>>(std::istream& in, eQuestStatus& type) {
	if(!readEnum(in, type, quest_status, eQuestStatus::AVAILABLE))
		in.setstate(std::ios::failbit);
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
	if(!readEnum(in, e, main_status, eMainStatus::ABSENT))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eShopType

cEnumLookup shop_types = {"live", "dead", "rand"};

std::istream& operator>>(std::istream& in, eShopType& type) {
	if(!readEnum(in, type, shop_types, eShopType::NORMAL))
		in.setstate(std::ios::failbit);
	return in;
}

std::ostream& operator<<(std::ostream& out, eShopType type) {
	writeEnum(out, type, shop_types, "live");
	return out;
}

// MARK: eShopPrompt

cEnumLookup shop_prompts = {"shop", "heal", "mage", "priest", "spell", "alch", "train"};

std::istream& operator>>(std::istream& in, eShopPrompt& type) {
	if(!readEnum(in, type, shop_prompts, eShopPrompt::SHOPPING))
		in.setstate(std::ios::failbit);
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
	if(!readEnum(in, e, ter_types, eTerSpec::NONE))
		in.setstate(std::ios::failbit);
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
	if(!readEnum(in, e, ter_trims, eTrimType::NONE))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eTerObstruct

cEnumLookup ter_blocks = {"none", "sight", "monsters", "move", "move-and-shoot", "move-and-sight"};

std::ostream& operator<< (std::ostream& out, eTerObstruct block) {
	writeEnum(out, block, ter_blocks, "none");
	return out;
}

std::istream& operator >> (std::istream& in, eTerObstruct& e){
	if(!readEnum(in, e, ter_blocks, eTerObstruct::CLEAR))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eStepSnd

cEnumLookup step_snds = {"step", "squish", "crunch", "none", "splash"};

std::ostream& operator<< (std::ostream& out, eStepSnd snd) {
	writeEnum(out, snd, step_snds, "step");
	return out;
}

std::istream& operator >> (std::istream& in, eStepSnd& e){
	if(!readEnum(in, e, step_snds, eStepSnd::STEP))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eLighting

cEnumLookup light_types = {"lit", "dark", "drains", "none"};

std::ostream& operator<< (std::ostream& out, eLighting light) {
	writeEnum(out, light, light_types, "lit");
	return out;
}

std::istream& operator>> (std::istream& in, eLighting& light) {
	if(!readEnum(in, light, light_types, LIGHT_NORMAL))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eTalkNode

cEnumLookup talk_nodes = {
	"reg","if-sdf","set-sdf","inn","if-time","if-event","if-town","shop","train","jobs",
	"","","","sell-weap","sell-prot","sell-any","id","ench","buy-info","buy-sdf",
	"buy-ship","buy-horse","buy-spec-item","quest","buy-town","end-force","end-fight","end-alarm","end-die","call-local",
	"call-global",
};

std::ostream& operator<< (std::ostream& out, eTalkNode node) {
	writeEnum(out, node, talk_nodes, "reg");
	return out;
}

std::istream& operator>> (std::istream& in, eTalkNode& node) {
	if(!readEnum(in, node, talk_nodes, eTalkNode::REGULAR))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eAttitude

cEnumLookup attitude_strs = {"docile", "hostile-a", "friendly", "hostile-b"};

std::ostream& operator<< (std::ostream& out, eAttitude att) {
	writeEnum(out, att, attitude_strs, "docile");
	return out;
}

std::istream& operator>> (std::istream& in, eAttitude& att) {
	if(!readEnum(in, att, attitude_strs, eAttitude::DOCILE))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eContentRating

cEnumLookup rating_strs = {"G", "PG", "R", "NC-17"};

std::ostream& operator<< (std::ostream& out, eContentRating rating) {
	writeEnum(out, rating, rating_strs, "docile");
	return out;
}

std::istream& operator>> (std::istream& in, eContentRating& rating) {
	if(!readEnum(in, rating, rating_strs, eContentRating::G))
		in.setstate(std::ios::failbit);
	return in;
}

// MARK: eSpellPat

cEnumLookup spell_pat_strs = {
	"single", "square", "small-square", "open-square", "radius-2", "radius-3", "plus", "wall",
};

std::ostream& operator<< (std::ostream& out, eSpellPat pat) {
	writeEnum(out, pat, spell_pat_strs, "single");
	return out;
}

std::istream& operator>> (std::istream& in, eSpellPat& pat) {
	if(!readEnum(in, pat, spell_pat_strs, PAT_SINGLE))
		in.setstate(std::ios::failbit);
	return in;
}
