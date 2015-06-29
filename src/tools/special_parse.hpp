//
//  special_parse.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-02.
//
//

#ifndef BoE_special_parse_hpp
#define BoE_special_parse_hpp

#include "special.hpp"
#include <boost/spirit/include/classic.hpp>

enum eParseError {
	generic_error,
	double_def,
	expect_op,
	expect_sym,
	expect_dat,
	expect_eq,
	expect_int,
	expect_val,
	expect_nl,
	NUM_PARSE_ERR
};

namespace spirit = boost::spirit::classic;
typedef spirit::rule<> Rule;
typedef spirit::assertion<eParseError> Err;
typedef spirit::guard<eParseError> Guard;

class SpecialParser {
	using Iter = Rule::scanner_t::iterator_t;
	using ErrStatus = spirit::error_status<>;
	static void init_file(Iter, Iter);
	static void init_block(Iter, Iter);
	static void prep_add_symbol(Iter, Iter);
	static void add_symbol(int i);
	static void add_command(Iter, Iter);
	static void set_type(eSpecType type);
	static void skip_to(int i);
	static void for_sdf(Iter, Iter);
	static void for_pic(Iter, Iter);
	static void for_msg(Iter, Iter);
	static void for_ex1(Iter, Iter);
	static void for_ex2(Iter, Iter);
	static void for_goto(Iter, Iter);
	static void set_first(int i);
	static void set_second(int i);
	static void set_third(int i);
	static void next_line(Iter, Iter);
	static void maybe_throw(Iter, Iter);
	static std::string temp_symbol;
	static int cur_node, cur_fld;
	static cSpecial curSpec;
	static int lineno, last_line_start;
	static Iter file_start;
	static std::map<size_t, cSpecial> specials;
	static spirit::symbols<> defn;
	static Rule ws, eq, comment, symbol, symbol_ch, eol, op_assign;
	static Rule datcode, command, init_line, null_line, def_line, cmd_line, op_line, cmd_block, nodes_file;
	static Rule cmd_1st, cmd_2nd, cmd_3rd;
	static Err err, assert_op, assert_sym, assert_dat, assert_eq, assert_int, assert_val;
	static Guard guard;
	static bool grammar_built;
	static ErrStatus check_error(const Rule::scanner_t&, spirit::parser_error<eParseError, Iter>);
	static eParseError last_err;
public:
	SpecialParser();
	std::map<size_t,cSpecial> parse(std::string code, std::string context);
};

class xSpecParseError : public std::exception {
	static const char*const messages[NUM_PARSE_ERR];
	eParseError err;
	int line, col;
	std::string found, file;
	mutable const char* msg;
public:
	xSpecParseError(std::string found, eParseError expect, int line, int col, std::string file);
	~xSpecParseError() throw();
	const char* what() const throw();
};

#endif
