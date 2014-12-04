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
#include <boost/phoenix/bind.hpp>

#include "special.h"

namespace ph = boost::phoenix;
qi::symbols<char, eSpecNodeType> opcode;

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
			("null", SPEC_NULL)
			("set-sdf", SPEC_SET_SDF)
			("inc-sdf", SPEC_INC_SDF)
			("disp-msg", SPEC_DISPLAY_MSG)
			("secret-pass", SPEC_SECRET_PASSAGE)
			("disp-sm-msg", SPEC_DISPLAY_SM_MSG)
			("flip-sdf", SPEC_FLIP_SDF)
			("filter-context", SPEC_OUT_BLOCK)
			("block-move", SPEC_CANT_ENTER)
			("change-time", SPEC_CHANGE_TIME)
			("start-timer-scen", SPEC_SCEN_TIMER_START)
			("play-sound", SPEC_PLAY_SOUND)
			("change-horse", SPEC_CHANGE_HORSE_OWNER)
			("change-boat", SPEC_CHANGE_BOAT_OWNER)
			("town-visible", SPEC_SET_TOWN_VISIBILITY)
			("set-event", SPEC_MAJOR_EVENT_OCCURRED)
			("force-give", SPEC_FORCED_GIVE)
			("buy-item-class", SPEC_BUY_ITEMS_OF_TYPE)
			("call-global", SPEC_CALL_GLOBAL)
			("set-sdf-row", SPEC_SET_SDF_ROW)
			("copy-sdf", SPEC_COPY_SDF)
			("rest", SPEC_REST)
			("set-wander-fight", SPEC_WANDERING_WILL_FIGHT)
			("end-scen", SPEC_END_SCENARIO)
			("once-give-item", SPEC_ONCE_GIVE_ITEM)
			("once-give-spec-item", SPEC_ONCE_GIVE_SPEC_ITEM)
			("once", SPEC_ONCE_NULL)
			("once-set-sdf", SPEC_ONCE_SET_SDF)
			("once-disp-msg", SPEC_ONCE_DISPLAY_MSG)
			("once-dlog", SPEC_ONCE_DIALOG)
			("once-give-dlog", SPEC_ONCE_GIVE_ITEM_DIALOG)
			("once-encounter", SPEC_ONCE_OUT_ENCOUNTER)
			("once-trap", SPEC_ONCE_TRAP)
			("select-pc", SPEC_SELECT_PC)
			("damage", SPEC_DAMAGE)
			("hp", SPEC_AFFECT_HP)
			("sp", SPEC_AFFECT_SP)
			("xp", SPEC_AFFECT_XP)
			("skill-pts", SPEC_AFFECT_SKILL_PTS)
			("death", SPEC_AFFECT_DEADNESS)
			("status", SPEC_AFFECT_POISON)
			("statistic", SPEC_AFFECT_STAT)
			("spell", SPEC_AFFECT_MAGE_SPELL)
			("gold", SPEC_AFFECT_GOLD)
			("food", SPEC_AFFECT_FOOD)
			("alchemy", SPEC_AFFECT_ALCHEMY)
			("stealth", SPEC_AFFECT_STEALTH)
			("firewalk", SPEC_AFFECT_FIREWALK)
			("flight", SPEC_AFFECT_FLIGHT)
			("if-sdf", SPEC_IF_SDF)
			("if-town", SPEC_IF_TOWN_NUM)
			("if-spec-item", SPEC_IF_HAVE_SPECIAL_ITEM)
			("if-sdf-compare", SPEC_IF_SDF_COMPARE)
			("if-ter", SPEC_IF_TOWN_TER_TYPE)
			("if-gold", SPEC_IF_HAS_GOLD_AND_TAKE)
			("if-food", SPEC_IF_HAS_FOOD_AND_TAKE)
			("if-item-class-on-space", SPEC_IF_ITEM_CLASS_ON_SPACE_AND_TAKE)
			("if-item-class", SPEC_IF_HAVE_ITEM_CLASS_AND_TAKE)
			("if-item-class-equip", SPEC_IF_EQUIP_ITEM_CLASS_AND_TAKE)
			("if-day", SPEC_IF_DAY_REACHED)
			("if-field", SPEC_IF_BARRELS)
			("if-object", SPEC_IF_CRATES)
			("if-event", SPEC_IF_EVENT_OCCURRED)
			("if-cave-lore", SPEC_IF_HAS_CAVE_LORE)
			("if-woodsman", SPEC_IF_HAS_WOODSMAN)
			("if-mage-lore", SPEC_IF_ENOUGH_MAGE_LORE)
			("if-response", SPEC_IF_TEXT_RESPONSE)
			("if-sdf-eq", SPEC_IF_SDF_EQ)
			("town-attitude", SPEC_MAKE_TOWN_HOSTILE)
			("change-ter", SPEC_TOWN_CHANGE_TER)
			("swap-ter", SPEC_TOWN_SWAP_TER)
			("trans-ter", SPEC_TOWN_TRANS_TER)
			("move-party", SPEC_TOWN_MOVE_PARTY)
			("hit-space", SPEC_TOWN_HIT_SPACE)
			("explode-space", SPEC_TOWN_EXPLODE_SPACE)
			("lock-space", SPEC_TOWN_LOCK_SPACE)
			("unlock-space", SPEC_TOWN_UNLOCK_SPACE)
			("anim-explode", SPEC_TOWN_SFX_BURST)
			("make-wandering", SPEC_TOWN_CREATE_WANDERING)
			("place-monst", SPEC_TOWN_PLACE_MONST)
			("destroy-most", SPEC_TOWN_DESTROY_MONST)
			("nuke-monsts", SPEC_TOWN_NUKE_MONSTS)
			("lever-generic", SPEC_TOWN_GENERIC_LEVER)
			("portal-generic", SPEC_TOWN_GENERIC_PORTAL)
			("stair-generic", SPEC_TOWN_GENERIC_STAIR)
			("button-generic", SPEC_TOWN_GENERIC_BUTTON)
			("lever", SPEC_TOWN_LEVER)
			("portal", SPEC_TOWN_PORTAL)
			("stair", SPEC_TOWN_STAIR)
			("set-sector", SPEC_TOWN_RELOCATE)
			("place-item", SPEC_TOWN_PLACE_ITEM)
			("split-party", SPEC_TOWN_SPLIT_PARTY)
			("unite-party", SPEC_TOWN_REUNITE_PARTY)
			("start-timer-town", SPEC_TOWN_TIMER_START)
			("rect-place-field", SPEC_RECT_PLACE_BLADE)
			("rect-cleanse", SPEC_RECT_CLEANSE)
			("rect-place-sfx", SPEC_RECT_PLACE_SFX)
			("rect-place-object", SPEC_RECT_PLACE_OBJECT)
			("rect-move-items", SPEC_RECT_MOVE_ITEMS)
			("rect-destroy-items", SPEC_RECT_DESTROY_ITEMS)
			("rect-change-ter", SPEC_RECT_CHANGE_TER)
			("rect-swap-ter", SPEC_RECT_SWAP_TER)
			("rect-trans-ter", SPEC_RECT_TRANS_TER)
			("rect-lock", SPEC_RECT_LOCK)
			("rect-unlock", SPEC_RECT_UNLOCK)
			("make-out-monst", SPEC_OUT_PLACE_ENCOUNTER)
			("start-shop", SPEC_OUT_STORE)
		;
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
	curSpec.type = SPEC_NULL;
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

void SpecialParser::set_type(eSpecNodeType type) {
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
