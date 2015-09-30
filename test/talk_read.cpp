//
//  talk_read.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-08-27.
//
//

#include <fstream>
#include "ticpp.h"
#include "catch.hpp"
#include "dialog.hpp"
#include "talking.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readDialogueFromXml(Document&& data, cSpeech& talk, int town_num);

TEST_CASE("Loading a town dialogue definition") {
	ifstream fin;
	cSpeech talk;
	Document doc;
	fin.exceptions(ios::badbit);
	
	SECTION("When the root tag is wrong") {
		fin.open("files/bad_root.xml");
		doc = xmlDocFromStream(fin, "bad_root.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xBadNode);
	}
	SECTION("When the version attribute is missing") {
		fin.open("files/talk/no_version.xml");
		doc = xmlDocFromStream(fin, "no_version.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xMissingAttr);
	}
	SECTION("When the root tag has a bad attribute") {
		fin.open("files/talk/bad_root_attr.xml");
		doc = xmlDocFromStream(fin, "bad_root_attr.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xBadAttr);
	}
	SECTION("When an invalid toplevel node is found") {
		fin.open("files/talk/bad_node.xml");
		doc = xmlDocFromStream(fin, "bad_node.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xBadNode);
	}
	SECTION("When the root tag is empty") {
		fin.open("files/talk/empty.xml");
		doc = xmlDocFromStream(fin, "empty.xml");
		REQUIRE_NOTHROW(readDialogueFromXml(move(doc), talk, 0));
	}
	
	SECTION("When a personality is missing an ID") {
		fin.open("files/talk/who_missing_id.xml");
		doc = xmlDocFromStream(fin, "who_missing_id.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), Exception);
	}
	SECTION("When a personality has an out-of-bounds ID") {
		fin.open("files/talk/who_bad_id.xml");
		doc = xmlDocFromStream(fin, "who_bad_id.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xBadVal);
	}
	SECTION("When a personality is missing a required subtag") {
		fin.open("files/talk/who_missing_req.xml");
		doc = xmlDocFromStream(fin, "who_missing_req.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xMissingElem);
	}
	SECTION("When a personality has an invalid subtag") {
		fin.open("files/talk/who_bad_node.xml");
		doc = xmlDocFromStream(fin, "who_bad_node.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xBadNode);
	}
	
	SECTION("When node has an invalid subtag") {
		fin.open("files/talk/node_bad_node.xml");
		doc = xmlDocFromStream(fin, "node_bad_node.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xBadNode);
	}
	SECTION("When a node is missing the for attribute") {
		fin.open("files/talk/node_missing_for.xml");
		doc = xmlDocFromStream(fin, "node_missing_for.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), Exception);
	}
	SECTION("When a node is missing the <key> subtag") {
		fin.open("files/talk/node_missing_key.xml");
		doc = xmlDocFromStream(fin, "node_missing_key.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xMissingElem);
	}
	SECTION("When a node is missing the <type> subtag") {
		fin.open("files/talk/node_missing_type.xml");
		doc = xmlDocFromStream(fin, "node_missing_type.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xMissingElem);
	}
	SECTION("When a node is missing the <text> subtag") {
		fin.open("files/talk/node_missing_string.xml");
		doc = xmlDocFromStream(fin, "node_missing_string.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xMissingElem);
	}
	SECTION("When a node has too many keys") {
		fin.open("files/talk/node_too_many_keys.xml");
		doc = xmlDocFromStream(fin, "node_too_many_keys.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xBadNode);
	}
	SECTION("When a node has too many params") {
		fin.open("files/talk/node_too_many_params.xml");
		doc = xmlDocFromStream(fin, "node_too_many_params.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xBadNode);
	}
	SECTION("When a node has too many strings") {
		fin.open("files/talk/node_too_many_strings.xml");
		doc = xmlDocFromStream(fin, "node_too_many_strings.xml");
		REQUIRE_THROWS_AS(readDialogueFromXml(move(doc), talk, 0), xBadNode);
	}
	
	SECTION("With the minimal required information for a personality") {
		fin.open("files/talk/minimal.xml");
		doc = xmlDocFromStream(fin, "minimal.xml");
		REQUIRE_NOTHROW(readDialogueFromXml(move(doc), talk, 0));
		CHECK(talk.people[0].title == "Billy Kumquat");
		CHECK(talk.people[0].look == "You see a small boy juggling fruit.");
		CHECK(talk.people[0].name == "\"I'm Billy Kumquat!\"");
		CHECK(talk.people[0].job == "\"I juggle!\"");
	}
	SECTION("A full personality with several nodes") {
		fin.open("files/talk/full.xml");
		doc = xmlDocFromStream(fin, "full.xml");
		REQUIRE_NOTHROW(readDialogueFromXml(move(doc), talk, 0));
		CHECK(talk.people[0].dunno == "\"No clue!\"");
		REQUIRE(talk.talk_nodes.size() == 4);
		CHECK(talk.talk_nodes[0].personality == 0);
		CHECK(string(talk.talk_nodes[0].link1, 4) == "jugg");
		CHECK(talk.talk_nodes[0].type == eTalkNode::REGULAR);
		CHECK(talk.talk_nodes[0].str1 == "\"It's fun!\"");
		CHECK(talk.talk_nodes[1].personality == 0);
		CHECK(string(talk.talk_nodes[1].link1, 4) == "info");
		CHECK(talk.talk_nodes[1].type == eTalkNode::BUY_SDF);
		CHECK(talk.talk_nodes[1].extras[0] == 1);
		CHECK(talk.talk_nodes[1].extras[1] == 2);
		CHECK(talk.talk_nodes[1].extras[2] == 3);
		CHECK(talk.talk_nodes[1].str1 == "\"There's a treasure hidden in the old tree!\"");
		CHECK(talk.talk_nodes[1].str2 == "\"For a gold piece I'll tell you something interesting!\"");
		CHECK(talk.talk_nodes[2].personality == 0);
		CHECK(string(talk.talk_nodes[2].link1, 4) == "purc");
		CHECK(talk.talk_nodes[2].type == eTalkNode::SHOP);
		CHECK(talk.talk_nodes[2].extras[0] == 0);
		CHECK(talk.talk_nodes[2].extras[1] == 4);
		CHECK(talk.talk_nodes[2].str1 == "Billy Kumquat's Oddest Fruit");
		CHECK(talk.talk_nodes[3].personality == 0);
		CHECK(string(talk.talk_nodes[3].link1, 4) == "kumq");
		CHECK(talk.talk_nodes[3].type == eTalkNode::RECEIVE_QUEST);
		CHECK(talk.talk_nodes[3].extras[0] == 5);
		CHECK(talk.talk_nodes[3].str1 == "\"I'll reward you if you find me some bigger kumquats!\"");
		CHECK(talk.talk_nodes[3].str2 == "\"Thanks for finding the bigger kumquats!\"");
	}
}
