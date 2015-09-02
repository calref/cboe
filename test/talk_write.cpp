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
}

