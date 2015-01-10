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

spirit::symbols<eSpecType> opcode;

#define _(fcn) &SpecialParser::fcn

bool SpecialParser::grammar_built = false;
std::string SpecialParser::temp_symbol;
int SpecialParser::cur_node, SpecialParser::cur_fld;
cSpecial SpecialParser::curSpec;
std::map<size_t, cSpecial> SpecialParser::specials;
spirit::symbols<> SpecialParser::defn;
Rule SpecialParser::ws, SpecialParser::comment, SpecialParser::symbol, SpecialParser::symbol_ch;
Rule SpecialParser::datcode, SpecialParser::command, SpecialParser::def_line, SpecialParser::cmd_line;
Rule SpecialParser::init_line, SpecialParser::null_line;
Rule SpecialParser::op_line, SpecialParser::cmd_block, SpecialParser::nodes_file;

SpecialParser::SpecialParser() {
	if(grammar_built) return;
	using namespace spirit;
	ws = blank_p;
	comment = ch_p('#') >> *(range_p('!','~') | ws);
	symbol_ch = chset<>("A-Za-z$_-");
	symbol = symbol_ch >> *symbol_ch;

	datcode = str_p("sdf")[_(for_sdf)] | str_p("pic")[_(for_pic)] | str_p("msg")[_(for_msg)] |
		str_p("ex1")[_(for_ex1)] | str_p("ex2")[_(for_ex2)] | str_p("goto")[_(for_goto)];

	command = datcode >> +ws >> (int_p[_(set_first)] | defn[_(set_first)]) >>
		!(*ws >> ch_p(',') >> *ws >> (int_p[_(set_second)] | defn[_(set_second)]) >>
		!(*ws >> ch_p(',') >> *ws >> (int_p[_(set_third)] | defn[_(set_third)])));

	null_line = !comment >> eol_p;
	init_line = null_line | def_line;
	def_line = str_p("def") >> +ws >> symbol[_(prep_add_symbol)] >> *ws >> ch_p('=') >> *ws >> int_p[_(add_symbol)] >> *ws >> !comment >> eol_p;
	cmd_line = command >> *ws >> !comment >> eol_p;
	op_line = ch_p('@') >> opcode[_(set_type)] >> *ws >> !(ch_p('=') >> *ws >> int_p[_(skip_to)]) >> *ws >> !comment >> eol_p;

	cmd_block = eps_p[_(init_block)] >> op_line >> *(*ws >> (cmd_line | def_line | null_line));

	nodes_file = eps_p[_(init_file)] >> *(*ws >> init_line) >> *cmd_block[_(add_command)] >> end_p;

	grammar_built = true;
}
						   
#undef A

void init_specials_parse() {
	opcode.add((*eSpecType::NONE).opcode().c_str(), eSpecType::NONE);
	// Fill in all the opcodes and check for missing types.
	// There's really no need to check all the way to the max of the underlying type.
	// It's unlikely we'd go above 255, so unsigned char would be fine, but just in case,
	// let's use unsigned short.
	// Could change the actual enum's underlying type instead though?
	using underlying = unsigned short;
	for(underlying i = 1; i < std::numeric_limits<underlying>::max(); i++) {
		eSpecType check = (eSpecType) i;
		eSpecCat category = getNodeCategory(check);
		if(category == eSpecCat::INVALID) continue;
		if((*check).opcode().empty())
			std::cout << "Warning: Missing opcode definition for special node type with ID " << i << std::endl;
		else opcode.add((*check).opcode().c_str(), check);
	}
}

void SpecialParser::init_file(Iter, Iter) {
	specials.clear();
	cur_node = -1;
}

void SpecialParser::init_block(Iter, Iter) {
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

void SpecialParser::prep_add_symbol(Iter start, Iter end) {
	temp_symbol.assign(start, end);
}

void SpecialParser::add_symbol(int i) {
	defn.add(temp_symbol.c_str(), i);
	temp_symbol.clear();
}

void SpecialParser::add_command(Iter, Iter) {
	specials[cur_node] = curSpec;
}

void SpecialParser::set_type(eSpecType type) {
	curSpec.type = type;
}

void SpecialParser::skip_to(int i) {
	cur_node = i;
}

void SpecialParser::for_sdf(Iter, Iter) {
	cur_fld = 0;
}

void SpecialParser::for_pic(Iter, Iter) {
	cur_fld = 1;
}

void SpecialParser::for_msg(Iter, Iter) {
	cur_fld = 2;
}

void SpecialParser::for_ex1(Iter, Iter) {
	cur_fld = 3;
}

void SpecialParser::for_ex2(Iter, Iter) {
	cur_fld = 4;
}

void SpecialParser::for_goto(Iter, Iter) {
	cur_fld = 5;
}

void SpecialParser::set_first(int i) {
	switch(cur_fld) {
		case 0: curSpec.sd1 = i; break;
		case 1: curSpec.pic = i; break;
		case 2: curSpec.m1 = i; break;
		case 3: curSpec.ex1a = i; break;
		case 4: curSpec.ex2a = i; break;
		case 5: curSpec.jumpto = i; break;
		default: bool pass = false; break;
	}
}

void SpecialParser::set_second(int i) {
	switch(cur_fld) {
		case 0: curSpec.sd2 = i; break;
		case 1: curSpec.pictype = i; break;
		case 2: curSpec.m2 = i; break;
		case 3: curSpec.ex1b = i; break;
		case 4: curSpec.ex2b = i; break;
		default: bool pass = false; break;
	}
}

void SpecialParser::set_third(int i) {
	switch(cur_fld) {
		case 2: curSpec.m3 = i; break;
		case 3: curSpec.ex1c = i; break;
		case 4: curSpec.ex2c = i; break;
		default: bool pass = false; break;
	}
}

std::map<size_t,cSpecial> SpecialParser::parse(std::string code) {
	static bool inited = false;
	if(!inited) init_specials_parse();
	auto result = spirit::parse(code.c_str(), nodes_file);
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

