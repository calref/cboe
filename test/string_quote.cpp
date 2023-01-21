//
//  string_quote.cpp
//  boe_test
//
//  Created by Celtic Minstrel on 2023-01-21.
//

#include "catch.hpp"
#include "fileio/fileio.hpp"

TEST_CASE("Quoting Strings") {
	CHECK(maybe_quote_string("") == std::string("''"));
	CHECK(maybe_quote_string(" ") == std::string("' '"));
	CHECK(maybe_quote_string("Don't!") == std::string("Don't!"));
	CHECK(maybe_quote_string("\"") == std::string("'\"'"));
	CHECK(maybe_quote_string("'") == std::string("\"'\""));
	CHECK(maybe_quote_string("-\"-") == std::string("-\"-"));
	CHECK(maybe_quote_string("-'-") == std::string("-'-"));
	CHECK(maybe_quote_string("Hello World") == std::string("'Hello World'"));
	CHECK(maybe_quote_string("It's great!") == std::string("\"It's great!\""));
	CHECK(maybe_quote_string("That is a \"silly\" idea.") == std::string("'That is a \"silly\" idea.'"));
	CHECK(maybe_quote_string("1\n2") == std::string("'1\\n2'"));
	CHECK(maybe_quote_string("1\t2") == std::string("'1\t2'"));
	CHECK(maybe_quote_string("1\f2") == std::string("'1\\f2'"));
	CHECK(maybe_quote_string("foo\"") == std::string("foo\""));
	CHECK(maybe_quote_string("foo'") == std::string("foo'"));
	CHECK(maybe_quote_string("That|is|great") == std::string("That|is|great"));
	CHECK(maybe_quote_string("==!==") == std::string("==!=="));
	CHECK(maybe_quote_string("Hello") == std::string("Hello"));
	CHECK(maybe_quote_string("123") == std::string("123"));
	CHECK(maybe_quote_string(".") == std::string("."));
	CHECK(maybe_quote_string("path\\to\\file") == std::string("path\\to\\file"));
	CHECK(maybe_quote_string("'path\\to\\file'") == std::string("\"'path\\\\to\\\\file'\""));
	CHECK(maybe_quote_string("Can't stumble with \"quotes\" of both types!") == std::string("'Can\\'t stumble with \"quotes\" of both types!'"));
	CHECK(maybe_quote_string("This is a \"complicated\" string\nwith 'many' different things to \\escape\\ in it! Shouldn't be too hard...?") == std::string("\"This is a \\\"complicated\\\" string\\nwith 'many' different things to \\\\escape\\\\ in it! Shouldn't be too hard...?\""));
}

static std::string unquote_string(std::string str) {
	std::istringstream is(str);
	return read_maybe_quoted_string(is);
}

TEST_CASE("Unquoting Strings") {
	CHECK(unquote_string("''") == std::string(""));
	CHECK(unquote_string("' '" ) == std::string(" "));
	CHECK(unquote_string("Don't!") == std::string("Don't!"));
	CHECK(unquote_string("'\"'") == std::string("\""));
	CHECK(unquote_string("\"'\"" ) == std::string("'"));
	CHECK(unquote_string("-\"-") == std::string("-\"-"));
	CHECK(unquote_string("-'-") == std::string("-'-"));
	CHECK(unquote_string("'Hello World'") == std::string("Hello World"));
	CHECK(unquote_string("\"It's great!\"") == std::string("It's great!"));
	CHECK(unquote_string("'That is a \"silly\" idea.'") == std::string("That is a \"silly\" idea."));
	CHECK(unquote_string("'1\\n2'") == std::string("1\n2"));
	CHECK(unquote_string("'1\t2'") == std::string("1\t2"));
	CHECK(unquote_string("'1\\t2'") == std::string("1\t2"));
	CHECK(unquote_string("'1\\f2'") == std::string("1\f2"));
	CHECK(unquote_string("foo\"") == std::string("foo\""));
	CHECK(unquote_string("foo'") == std::string("foo'"));
	CHECK(unquote_string("That|is|great") == std::string("That|is|great"));
	CHECK(unquote_string("==!==") == std::string("==!=="));
	CHECK(unquote_string("Hello") == std::string("Hello"));
	CHECK(unquote_string("123") == std::string("123"));
	CHECK(unquote_string(".") == std::string("."));
	CHECK(unquote_string("path\\to\\file") == std::string("path\\to\\file"));
	CHECK(unquote_string("\"'path\\\\to\\\\file'\"") == std::string("'path\\to\\file'"));
	CHECK(unquote_string("'Can\\'t stumble with \"quotes\" of both types!'") == std::string("Can't stumble with \"quotes\" of both types!"));
	CHECK(unquote_string("\"This is a \\\"complicated\\\" string\\nwith 'many' different things to \\\\escape\\\\ in it! Shouldn't be too hard...?\"") == std::string("This is a \"complicated\" string\nwith 'many' different things to \\escape\\ in it! Shouldn't be too hard...?"));
}
