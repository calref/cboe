//
//  specials_parse.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-02.
//
//

//#define BOOST_SPIRIT_DEBUG

#include "special_parse.hpp"

#include <fstream>
#include <sstream>
#include <iterator>
#include <set>
#include <boost/phoenix/bind.hpp>

#include "special.h"

namespace ph = boost::phoenix;
qi::symbols<char, eSpecType> opcode;

#define _(fcn) SpecialParser::fcn

bool SpecialParser::grammar_built = false;
std::string SpecialParser::temp_symbol;
int SpecialParser::cur_node, SpecialParser::cur_fld;
cSpecial SpecialParser::curSpec;
std::map<size_t, cSpecial> SpecialParser::specials;
qi::symbols<char, int> SpecialParser::defn;
Rule SpecialParser::ws, SpecialParser::comment, SpecialParser::symbol, SpecialParser::val;
Rule SpecialParser::datcode, SpecialParser::command, SpecialParser::def_line, SpecialParser::cmd_line;
Rule SpecialParser::init_line, SpecialParser::null_line;
Rule SpecialParser::op_line, SpecialParser::cmd_block, SpecialParser::nodes_file;

SpecialParser::SpecialParser() {
	if(grammar_built) return;
	using namespace qi;
	ws = char_(" \t");
	comment = char_('#') >> *(print | ws);
	symbol = lexeme[char_("A-Za-z$_-")[_(prep_add_symbol)] >> *char_("A-Za-z0-9$_-")[_(prep_add_symbol)]];
	val = int_ | defn;

	datcode = lit("sdf")[_(for_sdf)] | lit("pic")[_(for_pic)] | lit("msg")[_(for_msg)] |
		lit("ex1")[_(for_ex1)] | lit("ex2")[_(for_ex2)] | lit("goto")[_(for_goto)];

	command = datcode >> +ws >> val[_(set_first)] >>
		-(*ws >> char_(',') >> *ws >> val[_(set_second)] >>
		-(*ws >> char_(',') >> *ws >> val[_(set_third)]));

	null_line = -comment >> eol;
	init_line = null_line | def_line;
	def_line = lit("def") >> +ws >> symbol >> *ws >> char_('=') >> *ws >> uint_[_(add_symbol)] >> *ws >> -comment >> eol;
	cmd_line = command >> *ws >> -comment >> eol;
	op_line = char_('@') >> opcode[_(set_type)] >> *ws >> -(char_('=') >> *ws >> int_[_(skip_to)]) >> *ws >> -comment >> eol;

	cmd_block = eps[_(init_block)] >> op_line >> *(*ws >> (cmd_line | def_line | null_line));

	nodes_file = eps[_(init_file)] >> *(*ws >> init_line) >> *cmd_block[_(add_command)];
	
	grammar_built = true;
}
						   
#undef A

struct initer {
	initer() {
		opcode.add
			("null", eSpecType::NONE)
			("set-sdf", eSpecType::SET_SDF)
			("inc-sdf", eSpecType::INC_SDF)
			("disp-msg", eSpecType::DISPLAY_MSG)
			("secret-pass", eSpecType::SECRET_PASSAGE)
			("disp-sm-msg", eSpecType::DISPLAY_SM_MSG)
			("flip-sdf", eSpecType::FLIP_SDF)
			("if-context", eSpecType::IF_CONTEXT)
			("block-move", eSpecType::CANT_ENTER)
			("change-time", eSpecType::CHANGE_TIME)
			("start-timer-scen", eSpecType::SCEN_TIMER_START)
			("play-sound", eSpecType::PLAY_SOUND)
			("change-horse", eSpecType::CHANGE_HORSE_OWNER)
			("change-boat", eSpecType::CHANGE_BOAT_OWNER)
			("town-visible", eSpecType::SET_TOWN_VISIBILITY)
			("set-event", eSpecType::MAJOR_EVENT_OCCURRED)
			("force-give", eSpecType::FORCED_GIVE)
			("buy-item-class", eSpecType::BUY_ITEMS_OF_TYPE)
			("call-global", eSpecType::CALL_GLOBAL)
			("set-sdf-row", eSpecType::SET_SDF_ROW)
			("copy-sdf", eSpecType::COPY_SDF)
			("rest", eSpecType::REST)
			("set-wander-fight", eSpecType::WANDERING_WILL_FIGHT)
			("end-scen", eSpecType::END_SCENARIO)
			("once-give-item", eSpecType::ONCE_GIVE_ITEM)
			("once-give-spec-item", eSpecType::ONCE_GIVE_SPEC_ITEM)
			("once", eSpecType::ONCE_NULL)
			("once-set-sdf", eSpecType::ONCE_SET_SDF)
			("once-disp-msg", eSpecType::ONCE_DISPLAY_MSG)
			("once-dlog", eSpecType::ONCE_DIALOG)
			("once-give-dlog", eSpecType::ONCE_GIVE_ITEM_DIALOG)
			("once-encounter", eSpecType::ONCE_OUT_ENCOUNTER)
			("once-trap", eSpecType::ONCE_TRAP)
			("select-pc", eSpecType::SELECT_PC)
			("damage", eSpecType::DAMAGE)
			("hp", eSpecType::AFFECT_HP)
			("sp", eSpecType::AFFECT_SP)
			("xp", eSpecType::AFFECT_XP)
			("skill-pts", eSpecType::AFFECT_SKILL_PTS)
			("death", eSpecType::AFFECT_DEADNESS)
			("status", eSpecType::AFFECT_POISON)
			("statistic", eSpecType::AFFECT_STAT)
			("spell", eSpecType::AFFECT_MAGE_SPELL)
			("gold", eSpecType::AFFECT_GOLD)
			("food", eSpecType::AFFECT_FOOD)
			("alchemy", eSpecType::AFFECT_ALCHEMY)
			("stealth", eSpecType::AFFECT_STEALTH)
			("firewalk", eSpecType::AFFECT_FIREWALK)
			("flight", eSpecType::AFFECT_FLIGHT)
			("if-sdf", eSpecType::IF_SDF)
			("if-town", eSpecType::IF_TOWN_NUM)
			("if-spec-item", eSpecType::IF_HAVE_SPECIAL_ITEM)
			("if-sdf-compare", eSpecType::IF_SDF_COMPARE)
			("if-ter", eSpecType::IF_TOWN_TER_TYPE)
			("if-gold", eSpecType::IF_HAS_GOLD_AND_TAKE)
			("if-food", eSpecType::IF_HAS_FOOD_AND_TAKE)
			("if-item-class-on-space", eSpecType::IF_ITEM_CLASS_ON_SPACE_AND_TAKE)
			("if-item-class", eSpecType::IF_HAVE_ITEM_CLASS_AND_TAKE)
			("if-item-class-equip", eSpecType::IF_EQUIP_ITEM_CLASS_AND_TAKE)
			("if-day", eSpecType::IF_DAY_REACHED)
//			("if-field", eSpecType::IF_BARRELS)
			("if-object", eSpecType::IF_OBJECTS)
			("if-event", eSpecType::IF_EVENT_OCCURRED)
			("if-trait", eSpecType::IF_TRAIT)
			("if-species", eSpecType::IF_SPECIES)
			("if-statistic", eSpecType::IF_STATISTIC)
			("if-response", eSpecType::IF_TEXT_RESPONSE)
			("if-sdf-eq", eSpecType::IF_SDF_EQ)
			("town-attitude", eSpecType::MAKE_TOWN_HOSTILE)
			("change-ter", eSpecType::TOWN_CHANGE_TER)
			("swap-ter", eSpecType::TOWN_SWAP_TER)
			("trans-ter", eSpecType::TOWN_TRANS_TER)
			("move-party", eSpecType::TOWN_MOVE_PARTY)
			("hit-space", eSpecType::TOWN_HIT_SPACE)
			("explode-space", eSpecType::TOWN_EXPLODE_SPACE)
			("lock-space", eSpecType::TOWN_LOCK_SPACE)
			("unlock-space", eSpecType::TOWN_UNLOCK_SPACE)
			("anim-explode", eSpecType::TOWN_SFX_BURST)
			("make-wandering", eSpecType::TOWN_CREATE_WANDERING)
			("place-monst", eSpecType::TOWN_PLACE_MONST)
			("destroy-most", eSpecType::TOWN_DESTROY_MONST)
			("nuke-monsts", eSpecType::TOWN_NUKE_MONSTS)
			("lever-generic", eSpecType::TOWN_GENERIC_LEVER)
			("portal-generic", eSpecType::TOWN_GENERIC_PORTAL)
			("stair-generic", eSpecType::TOWN_GENERIC_STAIR)
			("button-generic", eSpecType::TOWN_GENERIC_BUTTON)
			("lever", eSpecType::TOWN_LEVER)
			("portal", eSpecType::TOWN_PORTAL)
			("stair", eSpecType::TOWN_STAIR)
			("set-sector", eSpecType::TOWN_RELOCATE)
			("place-item", eSpecType::TOWN_PLACE_ITEM)
			("split-party", eSpecType::TOWN_SPLIT_PARTY)
			("unite-party", eSpecType::TOWN_REUNITE_PARTY)
			("start-timer-town", eSpecType::TOWN_TIMER_START)
			("rect-place-field", eSpecType::RECT_PLACE_BLADE)
			("rect-cleanse", eSpecType::RECT_CLEANSE)
			("rect-place-sfx", eSpecType::RECT_PLACE_SFX)
			("rect-place-object", eSpecType::RECT_PLACE_OBJECT)
			("rect-move-items", eSpecType::RECT_MOVE_ITEMS)
			("rect-destroy-items", eSpecType::RECT_DESTROY_ITEMS)
			("rect-change-ter", eSpecType::RECT_CHANGE_TER)
			("rect-swap-ter", eSpecType::RECT_SWAP_TER)
			("rect-trans-ter", eSpecType::RECT_TRANS_TER)
			("rect-lock", eSpecType::RECT_LOCK)
			("rect-unlock", eSpecType::RECT_UNLOCK)
			("make-out-monst", eSpecType::OUT_PLACE_ENCOUNTER)
			("start-shop", eSpecType::OUT_STORE)
		;
		// A check for missing types.
		using underlying = std::underlying_type<eSpecType>::type;
		struct node_less : std::binary_function<eSpecType, eSpecType, bool> {
			bool operator()(const eSpecType& x, const eSpecType& y) const {return underlying(x) < underlying(y);}
		};
		std::set<eSpecType, node_less> allNodes;
		for(underlying i = 0; i < std::numeric_limits<underlying>::max(); i++) {
			eSpecType check = (eSpecType) i;
			eSpecCat category = getNodeCategory(check);
			if(category == eSpecCat::INVALID) continue;
			allNodes.insert(check);
		}
		opcode.for_each([&allNodes](const std::string&, eSpecType node) {
			allNodes.erase(node);
		});
		std::for_each(allNodes.begin(), allNodes.end(), [](eSpecType node){
			printf("Warning: Missing opcode definition for special node type with ID %d\n", (int)node);
		});
	}
};

static struct initer initer;

void SpecialParser::init_file() {
	specials.clear();
	cur_node = -1;
}

void SpecialParser::init_block() {
	cur_node++;
	temp_symbol.clear();
	curSpec.type = eSpecType::NONE;
	curSpec.sd1 = -1;
	curSpec.sd2 = -1;
	curSpec.m1 = -1;
	curSpec.m2 = -1;
	curSpec.m3 = -1;
	curSpec.pic = -1;
	curSpec.pictype = 4; // PIC_DLOG
	curSpec.ex1a = -1;
	curSpec.ex1b = -1;
	curSpec.ex1c = -1;
	curSpec.ex2a = -1;
	curSpec.ex2b = -1;
	curSpec.ex2c = -1;
	curSpec.jumpto = -1;
}

void SpecialParser::prep_add_symbol(char c) {
	temp_symbol += c;
}

void SpecialParser::add_symbol(int i) {
	defn.add(temp_symbol, i);
	temp_symbol.clear();
}

void SpecialParser::add_command() {
	specials[cur_node] = curSpec;
}

void SpecialParser::set_type(eSpecType type) {
	curSpec.type = type;
}

void SpecialParser::skip_to(int i) {
	cur_node = i;
}

void SpecialParser::for_sdf() {
	cur_fld = 0;
}

void SpecialParser::for_pic() {
	cur_fld = 1;
}

void SpecialParser::for_msg() {
	cur_fld = 2;
}

void SpecialParser::for_ex1() {
	cur_fld = 3;
}

void SpecialParser::for_ex2() {
	cur_fld = 4;
}

void SpecialParser::for_goto() {
	cur_fld = 5;
}

void SpecialParser::set_first(int i, ph_t, bool& pass) {
	switch(cur_fld) {
		case 0: curSpec.sd1 = i; break;
		case 1: curSpec.pic = i; break;
		case 2: curSpec.m1 = i; break;
		case 3: curSpec.ex1a = i; break;
		case 4: curSpec.ex2a = i; break;
		case 5: curSpec.jumpto = i; break;
		default: pass = false; break;
	}
}

void SpecialParser::set_second(int i, ph_t, bool& pass) {
	switch(cur_fld) {
		case 0: curSpec.sd2 = i; break;
		case 1: curSpec.pictype = i; break;
		case 2: curSpec.m2 = i; break;
		case 3: curSpec.ex1b = i; break;
		case 4: curSpec.ex2b = i; break;
		default: pass = false; break;
	}
}

void SpecialParser::set_third(int i, ph_t, bool& pass) {
	switch(cur_fld) {
		case 2: curSpec.m3 = i; break;
		case 3: curSpec.ex1c = i; break;
		case 4: curSpec.ex2c = i; break;
		default: pass = false; break;
	}
}

std::map<size_t,cSpecial> SpecialParser::parse(std::string code) {
	bool success = qi::parse(code.begin(), code.end(), nodes_file);
	(void) success; // Suppress "unused parameter" warning
	return specials;
}

void SpecialParser::init_debug() {
//	BOOST_SPIRIT_DEBUG_NODE(comment);
//	BOOST_SPIRIT_DEBUG_NODE(datcode);
//	BOOST_SPIRIT_DEBUG_NODE(command);
//	BOOST_SPIRIT_DEBUG_NODE(def_line);
//	BOOST_SPIRIT_DEBUG_NODE(cmd_line);
//	BOOST_SPIRIT_DEBUG_NODE(op_line);
//	BOOST_SPIRIT_DEBUG_NODE(cmd_block);
//	BOOST_SPIRIT_DEBUG_NODE(nodes_file);
}

void test_special_parse(std::string file); // Suppress "no prototype" warning
void test_special_parse(std::string file) {
	std::ostringstream code;
	std::ifstream fin(file);
	code << fin.rdbuf();
	fin.close();
	SpecialParser parser;
	parser.init_debug();
	auto specials = parser.parse(code.str());
	std::ofstream fout(file + ".out");
	for(auto p : specials) {
		fout << "Special node ID " << p.first << ":\n";
		fout << "  Type: " << p.second.type << '\n';
		fout << "  SDF: (" << p.second.sd1 << ',' << p.second.sd2 << ")\n";
		fout << "  Message: (" << p.second.m1 << ',' << p.second.m2 << ',' << p.second.m3 << ")\n";
		fout << "  Pic: " << p.second.pic << '@' << p.second.pictype << '\n';
		fout << "  Extra 1a: " << p.second.ex1a << '\n';
		fout << "  Extra 1b: " << p.second.ex1b << '\n';
		fout << "  Extra 1c: " << p.second.ex1c << '\n';
		fout << "  Extra 2a: " << p.second.ex2a << '\n';
		fout << "  Extra 2b: " << p.second.ex2b << '\n';
		fout << "  Extra 2c: " << p.second.ex2c << '\n';
		fout << "  Jump To: " << p.second.jumpto << "\n\n";
	}
	fout.close();
}
