//
//  tagfile.cpp
//  boe_test
//
//  Created by Celtic Minstrel on 2022-07-12.
//

#include "fileio/tagfile.hpp"
#include <sstream>
#include <array>

#include "catch.hpp"

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
	cTagFile content;
	SECTION("output") {
		std::ostringstream file;
		auto& p1 = content.add();
		p1["A"] << 12;
		p1["B"] << 22;
		p1["C"] << 45;
		auto& p2 = content.add();
		p2["X"] << "Hello World";
		p2["Y"] << "foo";
		p2["Z"] << "Blah!";
		content.writeTo(file);
		CHECK(file.str() == file_contents);
	}
	SECTION("input") {
		std::istringstream file(file_contents);
		content.readFrom(file);
		CHECK(content[0]["A"][0] == 12);
		CHECK(content[0]["B"][0] == 22);
		CHECK(content[0]["C"][0] == 45);
		CHECK(content[1]["X"][0] == std::string("Hello World"));
		CHECK(content[1]["Y"][0] == std::string("foo"));
		CHECK(content[1]["Z"][0] == std::string("Blah!"));
	}
}

TEST_CASE("Complex tag file") {
	static const std::string file_contents =
	// Page 1, a simple page with 3 single tags
	"A 12\n"
	"B 22\n"
	"C 45\n"
	"\f"
	// Page 2, a complex page with some array-like values
	"ID 123\n"
	"STRING foo\n"
	"STRING bar\n"
	"LOC 1 5\n"
	"LOC 12 22\n"
	"LOC 143 9\n"
	"COUNT 12\n"
	"YES\n"
	"ENABLE false\n"
	// Page 3 and onward, a series of identical pages
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
	cTagFile content;
	SECTION("output") {
		std::ostringstream file;
		auto& p1 = content.add();
		p1["A"] << 12;
		p1["B"] << 22;
		p1["C"] << 45;
		auto& p2 = content.add();
		p2["ID"] << 123;
		p2["STRING"] << "foo";
		p2["STRING"] << "bar";
		p2["LOC"] << 1 << 5;
		p2["LOC"] << std::array<int,2>{{12,22}};
		p2["LOC"] << std::pair<int,int>{143,9};
		p2["COUNT"] << 12;
		p2.add("YES");
		p2["ENABLE"] << false;
		auto& p4a = content.add();
		p4a["ID"] << '\x0c';
		p4a["VALUE"] << 400;
		p4a["COMMENT"] << "This is a comment!!!";
		auto& p4b = content.add();
		p4b["ID"] << '\x0d';
		p4b["VALUE"] << 128;
		p4b["COMMENT"] << "Nope nope nope";
		auto& p4c = content.add();
		p4c["ID"] << '\x12';
		p4c["VALUE"] << 90;
		p4c["COMMENT"] << "It's great!";
		content.writeTo(file);
		CHECK(file.str() == file_contents);
	}
	SECTION("input") {
		std::istringstream file(file_contents);
		content.readFrom(file);
		bool p1 = false, p2 = false;
		size_t p3 = 0;
		for(const auto& page : content) {
			if(!p1) {
				p1 = true;
				int a = 0, b = 0, c = 0;
				page["A"] >> a;
				page["B"] >> b;
				page["C"] >> c;
				CHECK(a == 12);
				CHECK(b == 22);
				CHECK(c == 45);
			} else if(!p2) {
				p2 = true;
				int id = 0, count = 0;
				page["ID"] >> id;
				page["COUNT"] >> count;
				CHECK(id == 123);
				CHECK(count == 12);
				std::vector<std::string> strings;
				page["STRING"].extract(strings);
				REQUIRE(strings.size() == 2);
				CHECK(strings[0] == "foo");
				CHECK(strings[1] == "bar");
				std::vector<std::pair<int,int>> locations;
				page["LOC"].extract(locations);
				REQUIRE(locations.size() == 3);
				CHECK(locations[0].first == 1);
				CHECK(locations[0].second == 5);
				CHECK(locations[1].first == 12);
				CHECK(locations[1].second == 22);
				CHECK(locations[2].first == 143);
				CHECK(locations[2].second == 9);
				bool yes = page.contains("YES"), no = page.contains("NO"), enable = true;
				page["ENABLE"] >> enable;
				CHECK(yes == true);
				CHECK(no == false);
				CHECK(enable == false);
			} else {
				p3++;
				char id;
				int value;
				std::string comment;
				page["ID"] >> id;
				page["VALUE"] >> value;
				page["COMMENT"] >> comment;
				switch(p3) {
					case 1:
						CHECK(id == '\x0c');
						CHECK(value == 400);
						CHECK(comment == "This is a comment!!!");
						break;
					case 2:
						CHECK(id == '\x0d');
						CHECK(value == 128);
						CHECK(comment == "Nope nope nope");
						break;
					case 3:
						CHECK(id == '\x12');
						CHECK(value == 90);
						CHECK(comment == "It's great!");
						break;
					default:
						FAIL("Too many extra pages!!!");
				}
			}
		}
		CHECK(p1);
		CHECK(p2);
		CHECK(p3);
		CHECK(p3 == 3);
	}
}
