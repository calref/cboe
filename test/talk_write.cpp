//
//  talk_write.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-09-02.
//
//

#include <fstream>
#include "catch.hpp"
#include "tinyprint.h"
#include "talking.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readDialogueFromXml(Document&& data, cSpeech& talk, int town_num);
extern void writeDialogueToXml(ticpp::Printer&& data, cSpeech& talk, int town_num);

static void in_and_out(string name, cSpeech& talk, int town_num = 0) {
	string fpath = "junk/talk_";
	fpath += name;
	fpath += ".xml";
	ofstream fout;
	fout.exceptions(ios::badbit);
	fout.open(fpath);
	writeDialogueToXml(Printer(name, fout), talk, town_num);
	fout.close();
	// Reconstruct the dialogue, to ensure that it doesn't just pass due to old data still being around
	talk.~cSpeech();
	new(&talk) cSpeech();
	ifstream fin;
	fin.exceptions(ios::badbit);
	fin.open(fpath);
	readDialogueFromXml(xmlDocFromStream(fin, name), talk, town_num);
}

TEST_CASE("Saving dialogue") {
	cSpeech talk;
	talk.people[0].title = "Test Person";
	talk.people[0].look = "You see a person. What an amazing sight!";
	talk.people[0].name = "\"My name is John Smith. How do you do?\"";
	talk.people[0].job = "\"Isn't it obvious? I test things!\"";
	SECTION("With the minimal required information for a personality") {
		in_and_out("basic", talk);
		CHECK(talk.people[0].title == "Test Person");
		CHECK(talk.people[0].look == "You see a person. What an amazing sight!");
		CHECK(talk.people[0].name == "\"My name is John Smith. How do you do?\"");
		CHECK(talk.people[0].job == "\"Isn't it obvious? I test things!\"");
	}
	SECTION("With some nodes") {
		talk.people[0].dunno = "\"I have no idea.\"";
		talk.talk_nodes.resize(4);
		strncpy(talk.talk_nodes[0].link1, "sell", 4);
		talk.talk_nodes[0].type = eTalkNode::SELL_ITEMS;
		talk.talk_nodes[0].str1 = "\"Ah, you have unwanted items? Never fear, I can take those off your hands!\"";
		strncpy(talk.talk_nodes[1].link1, "iden", 4);
		talk.talk_nodes[1].type = eTalkNode::IDENTIFY;
		talk.talk_nodes[1].extras[0] = 10;
		talk.talk_nodes[1].str1 = "\"Yes, I can identify your items for a mere 10 gold per item!\"";
		strncpy(talk.talk_nodes[2].link1, "test", 4);
		talk.talk_nodes[2].type = eTalkNode::RECEIVE_QUEST;
		talk.talk_nodes[2].extras[0] = 4;
		talk.talk_nodes[2].str1 = "\"Yes! In fact, you can help me to test things!\"";
		talk.talk_nodes[2].str2 = "\"Thanks for the help!\"";
		strncpy(talk.talk_nodes[3].link1, "boat", 4);
		talk.talk_nodes[3].type = eTalkNode::BUY_SHIP;
		talk.talk_nodes[3].extras[0] = 0;
		talk.talk_nodes[3].extras[1] = 5;
		talk.talk_nodes[3].extras[2] = 1;
		talk.talk_nodes[3].str1 = "\"You need a boat? Then you are in luck! I just happen to have a boat!\"";
		in_and_out("full", talk);
		CHECK(talk.people[0].dunno == "\"I have no idea.\"");
		REQUIRE(talk.talk_nodes.size() == 4);
		CHECK(string(talk.talk_nodes[0].link1, 4) ==  "sell");
		CHECK(talk.talk_nodes[0].type == eTalkNode::SELL_ITEMS);
		CHECK(talk.talk_nodes[0].str1 == "\"Ah, you have unwanted items? Never fear, I can take those off your hands!\"");
		CHECK(string(talk.talk_nodes[1].link1, 4) == "iden");
		CHECK(talk.talk_nodes[1].type == eTalkNode::IDENTIFY);
		CHECK(talk.talk_nodes[1].extras[0] == 10);
		CHECK(talk.talk_nodes[1].str1 == "\"Yes, I can identify your items for a mere 10 gold per item!\"");
		CHECK(string(talk.talk_nodes[2].link1, 4) == "test");
		CHECK(talk.talk_nodes[2].type == eTalkNode::RECEIVE_QUEST);
		CHECK(talk.talk_nodes[2].extras[0] == 4);
		CHECK(talk.talk_nodes[2].str1 == "\"Yes! In fact, you can help me to test things!\"");
		CHECK(talk.talk_nodes[2].str2 == "\"Thanks for the help!\"");
		CHECK(string(talk.talk_nodes[3].link1, 4) == "boat");
		CHECK(talk.talk_nodes[3].type == eTalkNode::BUY_SHIP);
		CHECK(talk.talk_nodes[3].extras[0] == 0);
		CHECK(talk.talk_nodes[3].extras[1] == 5);
		CHECK(talk.talk_nodes[3].extras[2] == 1);
		CHECK(talk.talk_nodes[3].str1 == "\"You need a boat? Then you are in luck! I just happen to have a boat!\"");
	}
}

