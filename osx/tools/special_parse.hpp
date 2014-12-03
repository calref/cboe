//
//  special_parse.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-02.
//
//

#ifndef BoE_special_parse_hpp
#define BoE_special_parse_hpp

#include <boost/spirit/include/qi.hpp>

#include "special.h"

namespace qi = boost::spirit::qi;
typedef boost::spirit::context<boost::fusion::cons<int &, boost::fusion::nil_>, boost::fusion::vector0<void>>& ph_t;
typedef qi::rule<std::string::iterator,int()> Rule;

class SpecialParser {
	static void init_file();
	static void init_block();
	static void prep_add_symbol(char c);
	static void add_symbol(int i);
	static void add_command();
	static void set_type(eSpecNodeType type);
	static void skip_to(int i);
	static void for_sdf();
	static void for_pic();
	static void for_msg();
	static void for_ex1();
	static void for_ex2();
	static void for_goto();
	static void set_first(int i, ph_t, bool& pass);
	static void set_second(int i, ph_t, bool& pass);
	static void set_third(int i, ph_t, bool& pass);
	static std::string temp_symbol;
	static int cur_node, cur_fld;
	static cSpecial curSpec;
	static std::map<size_t, cSpecial> specials;
	static qi::symbols<char, int> defn;
	static Rule ws, comment, symbol, val;
	static Rule datcode, command, def_line, cmd_line, op_line, cmd_block, nodes_file;
	static bool grammar_built;
public:
	SpecialParser();
	std::map<size_t,cSpecial> parse(std::string code);
	void debug();
};

#endif
