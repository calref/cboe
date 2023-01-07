//
//  tagfile.cpp
//  boe_test
//
//  Created by Celtic Minstrel on 2022-07-12.
//

#include "fileio/tagfile.hpp"
#include <sstream>

#include "catch.hpp"

template<typename T>
std::ostream& operator<<(std::ostream& os, const tBasicTag<T>& tag) {
	return os << T(tag);
}

class pSamplePage1 : public cTagFile_Page {
public:
	DEFINE_TAGFILE_PAGE(pSamplePage1, cTagFile_Page);
	tBasicTag<int> a{*this, "A"}, b{*this, "B"}, c{*this, "C"};
};

class pSamplePage2 : public cTagFile_Page {
public:
	DEFINE_TAGFILE_PAGE(pSamplePage2, cTagFile_Page);
	tBasicTag<std::string> x{*this, "X"}, y{*this, "Y"}, z{*this, "Z"};
};

class fSampleTagFile : public cTagFile {
public:
	pSamplePage1 p1{*this};
	pSamplePage2 p2{*this};
};

class pComplexPage : public cTagFile_Page {
public:
	DEFINE_TAGFILE_PAGE(pComplexPage, cTagFile_Page);
	tBasicTag<int> id{*this, "ID"};
	tArrayTag<std::string> strings{*this, "STRING"};
	tArrayTag<std::pair<int, int>> locations{*this, "LOC"};
	tOptionalTag<int> filter{*this, "FILTER"};
	tOptionalTag<int> count{*this, "COUNT"};
	tOptionalTag<bool> yes{*this, "YES"};
	tOptionalTag<bool> no{*this, "NO"};
	tBasicTag<bool> enable{*this, "ENABLE"};
};

class pSampleMultiPage : public pMultiPage<pSampleMultiPage> {
public:
	DEFINE_TAGFILE_MULTIPAGE(pSampleMultiPage, pMultiPage);
	tBasicTag<char> id{*this, "ID"};
	tBasicTag<int> value{*this, "VALUE"};
	tBasicTag<std::string> comment{*this, "COMMENT"};
};

class fComplexTagFile : public cTagFile {
public:
	pSamplePage1 p1{*this};
	pComplexPage p2{*this};
	pSampleMultiPage p3{*this};
};

TEST_CASE("Simple tag file") {
	static const std::string file_contents =
	"A 12\n"
	"B 22\n"
	"C 45\n"
	"\f"
	"X 'Hello World'\n"
	"Y foo\n"
	"Z Blah!\n"
	;
	fSampleTagFile content;
	SECTION("output") {
		std::ostringstream file;
		content.p1.a = 12;
		content.p1.b = 22;
		content.p1.c = 45;
		content.p2.x = "Hello World";
		content.p2.y = "foo";
		content.p2.z = "Blah!";
		content.writeTo(file);
		CHECK(file.str() == file_contents);
	}
	SECTION("input") {
		std::istringstream file(file_contents);
		content.readFrom(file);
		CHECK(content.p1.a == 12);
		CHECK(content.p1.b == 22);
		CHECK(content.p1.c == 45);
		CHECK(content.p2.x == std::string("Hello World"));
		CHECK(content.p2.y == std::string("foo"));
		CHECK(content.p2.z == std::string("Blah!"));
	}
}

TEST_CASE("Complex tag file") {
	static const std::string file_contents =
	"A 12\n"
	"B 22\n"
	"C 45\n"
	"\f"
	"ID 123\n"
	"STRING foo\n"
	"STRING bar\n"
	"LOC 1 5\n"
	"LOC 12 22\n"
	"LOC 143 9\n"
	"COUNT 12\n"
	"YES\n"
	"ENABLE false\n"
	"\f"
	"ID 12\n"
	"VALUE 400\n"
	"COMMENT 'This is a comment!!!'\n"
	"\f"
	"ID 13\n"
	"VALUE 128\n"
	"COMMENT 'Nope nope nope'\n"
	"\f"
	"ID 18\n"
	"VALUE 90\n"
	"COMMENT \"It's great!\"\n"
	;
	fComplexTagFile content;
	SECTION("output") {
		std::ostringstream file;
		content.p1.a = 12;
		content.p1.b = 22;
		content.p1.c = 45;
		content.p2.id = 123;
		content.p2.strings.add("foo");
		content.p2.strings.add("bar");
		content.p2.locations.add({1,5});
		content.p2.locations.add({12,22});
		content.p2.locations.add({143,9});
		content.p2.count = 12;
		content.p2.yes = true;
		content.p2.no = false;
		content.p2.enable = false;
		content.p3[0].id = 0x0c;
		content.p3[0].value = 400;
		content.p3[0].comment = "This is a comment!!!";
		content.p3[1].id = 0x0d;
		content.p3[1].value = 128;
		content.p3[1].comment = "Nope nope nope";
		auto& p3c = content.p3.add();
		p3c.id = 0x12;
		p3c.value = 90;
		p3c.comment = "It's great!";
		content.writeTo(file);
		CHECK(file.str() == file_contents);
	}
	SECTION("input") {
		std::istringstream file(file_contents);
		content.readFrom(file);
		CHECK(content.p1.a == 12);
		CHECK(content.p1.b == 22);
		CHECK(content.p1.c == 45);
		CHECK(content.p2.id == 123);
		REQUIRE(content.p2.strings.size() == 2);
		CHECK(content.p2.strings[0] == "foo");
		CHECK(content.p2.strings[1] == "bar");
		REQUIRE(content.p2.locations.size() == 3);
		CHECK(content.p2.locations[0].first == 1);
		CHECK(content.p2.locations[0].second == 5);
		CHECK(content.p2.locations[1].first == 12);
		CHECK(content.p2.locations[1].second == 22);
		CHECK(content.p2.locations[2].first == 143);
		CHECK(content.p2.locations[2].second == 9);
		CHECK(content.p2.filter == boost::none);
		CHECK(content.p2.count == 12);
		CHECK(content.p2.yes == true);
		CHECK(content.p2.no == false);
		CHECK(content.p2.enable == false);
		REQUIRE(content.p3.size() == 3);
		CHECK(content.p3[0].id == '\x0c');
		CHECK(content.p3[0].value == 400);
		CHECK(content.p3[0].comment == "This is a comment!!!");
		CHECK(content.p3[1].id == '\x0d');
		CHECK(content.p3[1].value == 128);
		CHECK(content.p3[1].comment == "Nope nope nope");
		CHECK(content.p3[2].id == '\x12');
		CHECK(content.p3[2].value == 90);
		CHECK(content.p3[2].comment == "It's great!");
	}
}
