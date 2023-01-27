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
	"HEX 3ad\n"
	"STRING foo\n"
	"STRING bar\n"
	"LOC 1 5\n"
	"LOC 12 22\n"
	"LOC 143 9\n"
	"START 12 40\n"
	"MODE 1 2 3 4\n"
	"STATUS 10 20 30 40 50 60 70 80 90 100\n"
	"COUNT 12\n"
	"YES\n"
	"ENABLE false\n"
	"ARRAY 1\n"
	"ARRAY 3\n"
	"ARRAY 7\n"
	"ARRAY 12\n"
	"MAP a b c\n"
	"MAP d e f\n"
	"MAP g h i\n"
	"\f"
	// Page 3, a complex page with some sparse array-like values
	"RECORD 2 'Hello World'\n"
	"RECORD 5 \"Isn't it cool?\"\n"
	"SDF 0 0 12\n"
	"SDF 1 3 52\n"
	"SDF 5 9 81\n"
	"DICT 'a b' 'c d'\n"
	"DICT abc def\n"
	"DICT blah blah\n"
	"DICT foo bar\n"
	// Page 4 and onward, a series of identical pages
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
		p2["HEX"] << as_hex<int>(0x3ad);
		p2["STRING"] << "foo";
		p2["STRING"] << "bar";
		p2["LOC"] << 1 << 5;
		p2["LOC"] << 12 << 22;
		p2["LOC"] << 143 << 9;
		p2["START"] << std::make_pair(12, 40);
		p2["MODE"] << std::make_tuple(1, 2, 3, 4);
		p2["STATUS"] << std::array<int,10>{{10,20,30,40,50,60,70,80,90,100}};
		p2["COUNT"] << 12;
		p2.add("YES");
		p2["ENABLE"] << false;
		std::vector<int> array{1,3,7,12};
		p2["ARRAY"].encode(array);
		vector2d<std::string> map;
		map.resize(3, 3);
		map.row(0)[0] = "a";
		map.row(0)[1] = "b";
		map.row(0)[2] = "c";
		map.row(1)[0] = "d";
		map.row(1)[1] = "e";
		map.row(1)[2] = "f";
		map.row(2)[0] = "g";
		map.row(2)[1] = "h";
		map.row(2)[2] = "i";
		p2["MAP"].encode(map);
		auto& p3 = content.add();
		std::vector<std::string> records{
			"", "",
			"Hello World",
			"", "",
			"Isn't it cool?",
		};
		p3["RECORD"].encodeSparse(records);
		vector2d<int> sdf;
		sdf.resize(6, 10);
		sdf[0][0] = 12;
		sdf[1][3] = 52;
		sdf[5][9] = 81;
		p3["SDF"].encodeSparse<int>(sdf);
		std::map<std::string, std::string> dict{
			{"foo", "bar"},
			{"a b", "c d"},
			{"abc", "def"},
			{"blah", "blah"},
		};
		p3["DICT"].encodeSparse(dict);
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
		auto output = file.str();
		CHECK(output == file_contents);
	}
	SECTION("input") {
		std::istringstream file(file_contents);
		content.readFrom(file);
		bool p1 = false, p2 = false, p3 = false;
		size_t p4 = 0;
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
				as_hex<int> hex = 0;
				page["ID"] >> id;
				page["HEX"] >> hex;
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
				std::pair<int, int> startloc;
				page["START"] >> startloc;
				CHECK(startloc.first == 12);
				CHECK(startloc.second == 40);
				std::tuple<int, int, int, int> mode;
				page["MODE"] >> mode;
				CHECK(std::get<0>(mode) == 1);
				CHECK(std::get<1>(mode) == 2);
				CHECK(std::get<2>(mode) == 3);
				CHECK(std::get<3>(mode) == 4);
				std::array<int,10> status;
				page["STATUS"] >> status;
				CHECK(status[0] == 10);
				CHECK(status[1] == 20);
				CHECK(status[2] == 30);
				CHECK(status[3] == 40);
				CHECK(status[4] == 50);
				CHECK(status[5] == 60);
				CHECK(status[6] == 70);
				CHECK(status[7] == 80);
				CHECK(status[8] == 90);
				CHECK(status[9] == 100);
				bool yes = page.contains("YES"), no = page.contains("NO"), enable = true;
				page["ENABLE"] >> enable;
				CHECK(yes == true);
				CHECK(no == false);
				CHECK(enable == false);
				std::vector<int> array;
				page["ARRAY"].extract(array);
				REQUIRE(array.size() == 4);
				CHECK(array[0] == 1);
				CHECK(array[1] == 3);
				CHECK(array[2] == 7);
				CHECK(array[3] == 12);
				vector2d<std::string> map;
				page["MAP"].extract(map);
				REQUIRE(map.width() == 3);
				REQUIRE(map.height() == 3);
				CHECK(map.row(0)[0] == std::string("a"));
				CHECK(map.row(0)[1] == std::string("b"));
				CHECK(map.row(0)[2] == std::string("c"));
				CHECK(map.row(1)[0] == std::string("d"));
				CHECK(map.row(1)[1] == std::string("e"));
				CHECK(map.row(1)[2] == std::string("f"));
				CHECK(map.row(2)[0] == std::string("g"));
				CHECK(map.row(2)[1] == std::string("h"));
				CHECK(map.row(2)[2] == std::string("i"));
			} else if(!p3) {
				p3 = true;
				std::vector<std::string> records;
				page["RECORD"].extractSparse(records);
				REQUIRE(records.size() == 6);
				CHECK(records[0] == std::string(""));
				CHECK(records[1] == std::string(""));
				CHECK(records[2] == std::string("Hello World"));
				CHECK(records[3] == std::string(""));
				CHECK(records[4] == std::string(""));
				CHECK(records[5] == std::string("Isn't it cool?"));
				vector2d<int> sdf;
				page["SDF"].extractSparse<int>(sdf);
				REQUIRE(sdf.width() == 6);
				REQUIRE(sdf.height() == 10);
				CHECK(sdf[0][0] == 12);
				CHECK(sdf[1][3] == 52);
				CHECK(sdf[5][9] == 81);
				for(size_t x = 0; x <= 5; x++) {
					for(size_t y = 0; y <= 9; y++) {
						if(x == 0 && y == 0) continue;
						if(x == 1 && y == 3) continue;
						if(x == 5 && y == 9) continue;
						CAPTURE(x);
						CAPTURE(y);
						CHECK(sdf[x][y] == 0);
					}
				}
				std::map<std::string, std::string> dict;
				page["DICT"].extractSparse(dict);
				REQUIRE(dict.size() == 4);
				CHECK(dict.count("foo") == 1);
				CHECK(dict["foo"] == "bar");
				CHECK(dict.count("a b") == 1);
				CHECK(dict["a b"] == "c d");
				CHECK(dict.count("abc") == 1);
				CHECK(dict["abc"] == "def");
				CHECK(dict.count("blah") == 1);
				CHECK(dict["blah"] == "blah");
			} else {
				p4++;
				char id;
				int value;
				std::string comment;
				page["ID"] >> id;
				page["VALUE"] >> value;
				page["COMMENT"] >> comment;
				switch(p4) {
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
		CHECK(p4 == 3);
	}
}
