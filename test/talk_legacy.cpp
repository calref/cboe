//
//  talk_legacy.cpp
//  BoE
//
//  Created by Celtic Minstrel on 16-08-09.
//
//

#include "catch.hpp"
#include "town.hpp"
#include "talking.hpp"
#include "scenario.hpp"
#include "oldstructs.hpp"

TEST_CASE("Converting legacy talk data") {
	cSpeech talk;
	legacy::talking_record_type old_talk;
	std::vector<shop_info_t> shops;
	old_talk.talk_nodes[0].personality = 12;
	old_talk.talk_nodes[0].type = 0;
	std::copy_n("findthem", 8, old_talk.talk_nodes[0].link1);
	std::fill_n(old_talk.talk_nodes[0].extras, 4, 0);
	
	SECTION("Basic data") {
		talk.import_legacy(old_talk, shops);
		REQUIRE(talk.talk_nodes.size() >= 1);
		CHECK(talk.talk_nodes[0].personality == 12);
		CHECK(talk.talk_nodes[0].type == eTalkNode::REGULAR);
		CHECK(talk.talk_nodes[0].link1[0] == 'f');
		CHECK(talk.talk_nodes[0].link1[1] == 'i');
		CHECK(talk.talk_nodes[0].link1[2] == 'n');
		CHECK(talk.talk_nodes[0].link1[3] == 'd');
		CHECK(talk.talk_nodes[0].link2[0] == 't');
		CHECK(talk.talk_nodes[0].link2[1] == 'h');
		CHECK(talk.talk_nodes[0].link2[2] == 'e');
		CHECK(talk.talk_nodes[0].link2[3] == 'm');
		CHECK(talk.talk_nodes[0].extras[0] == 0);
		CHECK(talk.talk_nodes[0].extras[1] == 0);
		CHECK(talk.talk_nodes[0].extras[2] == 0);
		CHECK(talk.talk_nodes[0].extras[3] == 0);
	}
	SECTION("Basic & If-Then talking nodes") {
		SECTION("Depends on Flag") {
			old_talk.talk_nodes[0].type = 1;
			old_talk.talk_nodes[0].extras[0] = 12;
			old_talk.talk_nodes[0].extras[1] = 13;
			old_talk.talk_nodes[0].extras[2] = 14;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::DEP_ON_SDF);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
			CHECK(talk.talk_nodes[0].extras[1] == 13);
			CHECK(talk.talk_nodes[0].extras[2] == 14);
		}
		SECTION("Set to One") {
			old_talk.talk_nodes[0].type = 2;
			old_talk.talk_nodes[0].extras[0] = 12;
			old_talk.talk_nodes[0].extras[1] = 13;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::SET_SDF);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
			CHECK(talk.talk_nodes[0].extras[1] == 13);
			CHECK(talk.talk_nodes[0].extras[2] == 1);
		}
		SECTION("Inn") {
			old_talk.talk_nodes[0].type = 3;
			old_talk.talk_nodes[0].extras[0] = 12;
			old_talk.talk_nodes[0].extras[1] = 13;
			old_talk.talk_nodes[0].extras[2] = 14;
			old_talk.talk_nodes[0].extras[3] = 15;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::INN);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
			CHECK(talk.talk_nodes[0].extras[1] == 13);
			CHECK(talk.talk_nodes[0].extras[2] == 14);
			CHECK(talk.talk_nodes[0].extras[3] == 15);
		}
		SECTION("Depends on time") {
			old_talk.talk_nodes[0].type = 4;
			old_talk.talk_nodes[0].extras[0] = 12;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::DEP_ON_TIME);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
		}
		SECTION("Depends on time with event") {
			old_talk.talk_nodes[0].type = 5;
			old_talk.talk_nodes[0].extras[0] = 12;
			old_talk.talk_nodes[0].extras[1] = 13;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::DEP_ON_TIME_AND_EVENT);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
			CHECK(talk.talk_nodes[0].extras[1] == 13);
		}
		SECTION("Depends on town") {
			old_talk.talk_nodes[0].type = 6;
			old_talk.talk_nodes[0].extras[0] = 12;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::DEP_ON_TOWN);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
		}
	}
	SECTION("Shops") {
		SECTION("Regular shop") {
			old_talk.talk_nodes[0].type = 7;
			old_talk.talk_nodes[0].extras[0] = 6;
			old_talk.talk_nodes[0].extras[1] = 13;
			old_talk.talk_nodes[0].extras[2] = 14;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::SHOP);
			CHECK(talk.talk_nodes[0].extras[0] == 6); // "Utterly Ridiculous"
			CHECK(talk.talk_nodes[0].extras[1] == 6);
			CHECK(talk.talk_nodes[0].extras[2] == 0);
			REQUIRE(shops.size() == 1);
			CHECK(shops[0].type == eShopItemType::ITEM);
			CHECK(shops[0].first == 13);
			CHECK(shops[0].count == 14);
		}
		SECTION("Training") {
			old_talk.talk_nodes[0].type = 8;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::TRAINING);
		}
		SECTION("Mage spell shop") {
			old_talk.talk_nodes[0].type = 9;
			old_talk.talk_nodes[0].extras[0] = 6;
			old_talk.talk_nodes[0].extras[1] = 13;
			old_talk.talk_nodes[0].extras[2] = 14;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::SHOP);
			CHECK(talk.talk_nodes[0].extras[0] == 6);
			CHECK(talk.talk_nodes[0].extras[1] == 6);
			CHECK(talk.talk_nodes[0].extras[2] == 0);
			REQUIRE(shops.size() == 1);
			CHECK(shops[0].type == eShopItemType::MAGE_SPELL);
			CHECK(shops[0].first == 43);
			CHECK(shops[0].count == 14);
		}
		SECTION("Priest spell shop") {
			old_talk.talk_nodes[0].type = 10;
			old_talk.talk_nodes[0].extras[0] = 6;
			old_talk.talk_nodes[0].extras[1] = 13;
			old_talk.talk_nodes[0].extras[2] = 14;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::SHOP);
			CHECK(talk.talk_nodes[0].extras[0] == 6);
			CHECK(talk.talk_nodes[0].extras[1] == 6);
			CHECK(talk.talk_nodes[0].extras[2] == 0);
			REQUIRE(shops.size() == 1);
			CHECK(shops[0].type == eShopItemType::PRIEST_SPELL);
			CHECK(shops[0].first == 43);
			CHECK(shops[0].count == 14);
		}
		SECTION("Alchemy shop") {
			old_talk.talk_nodes[0].type = 11;
			old_talk.talk_nodes[0].extras[0] = 6;
			old_talk.talk_nodes[0].extras[1] = 13;
			old_talk.talk_nodes[0].extras[2] = 14;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::SHOP);
			CHECK(talk.talk_nodes[0].extras[0] == 6);
			CHECK(talk.talk_nodes[0].extras[1] == 6);
			CHECK(talk.talk_nodes[0].extras[2] == 0);
			REQUIRE(shops.size() == 1);
			CHECK(shops[0].type == eShopItemType::ALCHEMY);
			CHECK(shops[0].first == 13);
			CHECK(shops[0].count == 14);
		}
		SECTION("Healer") {
			old_talk.talk_nodes[0].type = 12;
			old_talk.talk_nodes[0].extras[0] = 6;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::SHOP);
			CHECK(talk.talk_nodes[0].extras[0] == 6);
			CHECK(talk.talk_nodes[0].extras[1] == 5);
			CHECK(talk.talk_nodes[0].extras[2] == 0);
		}
		SECTION("Junk Shop") {
			old_talk.talk_nodes[0].type = 23;
			old_talk.talk_nodes[0].extras[0] = 6;
			old_talk.talk_nodes[0].extras[1] = 4;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::SHOP);
			CHECK(talk.talk_nodes[0].extras[0] == 6);
			CHECK(talk.talk_nodes[0].extras[1] == 4);
		}
		SECTION("Sell weapons") {
			old_talk.talk_nodes[0].type = 13;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::SELL_WEAPONS);
		}
		SECTION("Sell armour") {
			old_talk.talk_nodes[0].type = 14;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::SELL_ARMOR);
		}
		SECTION("Sell anything") {
			old_talk.talk_nodes[0].type = 15;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::SELL_ITEMS);
		}
		SECTION("Identify") {
			old_talk.talk_nodes[0].type = 16;
			old_talk.talk_nodes[0].extras[0] = 12;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::IDENTIFY);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
		}
		SECTION("Enchant") {
			old_talk.talk_nodes[0].type = 17;
			old_talk.talk_nodes[0].extras[0] = 12;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::ENCHANT);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
		}
	}
	SECTION("Other nodes") {
		SECTION("Buy info") {
			old_talk.talk_nodes[0].type = 18;
			old_talk.talk_nodes[0].extras[0] = 12;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::BUY_INFO);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
		}
		SECTION("Buy SDF") {
			old_talk.talk_nodes[0].type = 19;
			old_talk.talk_nodes[0].extras[0] = 12;
			old_talk.talk_nodes[0].extras[1] = 13;
			old_talk.talk_nodes[0].extras[2] = 14;
			old_talk.talk_nodes[0].extras[3] = 15;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::BUY_SDF);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
			CHECK(talk.talk_nodes[0].extras[1] == 13);
			CHECK(talk.talk_nodes[0].extras[2] == 14);
			CHECK(talk.talk_nodes[0].extras[3] == 15);
		}
		SECTION("Buy boat") {
			old_talk.talk_nodes[0].type = 20;
			old_talk.talk_nodes[0].extras[0] = 12;
			old_talk.talk_nodes[0].extras[1] = 13;
			old_talk.talk_nodes[0].extras[2] = 14;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::BUY_SHIP);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
			CHECK(talk.talk_nodes[0].extras[1] == 13);
			CHECK(talk.talk_nodes[0].extras[2] == 14);
		}
		SECTION("Buy horse") {
			old_talk.talk_nodes[0].type = 21;
			old_talk.talk_nodes[0].extras[0] = 12;
			old_talk.talk_nodes[0].extras[1] = 13;
			old_talk.talk_nodes[0].extras[2] = 14;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::BUY_HORSE);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
			CHECK(talk.talk_nodes[0].extras[1] == 13);
			CHECK(talk.talk_nodes[0].extras[2] == 14);
		}
		SECTION("Buy special item") {
			old_talk.talk_nodes[0].type = 22;
			old_talk.talk_nodes[0].extras[0] = 12;
			old_talk.talk_nodes[0].extras[1] = 13;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::BUY_SPEC_ITEM);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
			CHECK(talk.talk_nodes[0].extras[1] == 13);
		}
		SECTION("Buy town location") {
			old_talk.talk_nodes[0].type = 24;
			old_talk.talk_nodes[0].extras[0] = 12;
			old_talk.talk_nodes[0].extras[1] = 13;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::BUY_TOWN_LOC);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
			CHECK(talk.talk_nodes[0].extras[1] == 13);
		}
		SECTION("Force end") {
			old_talk.talk_nodes[0].type = 25;
			old_talk.talk_nodes[0].extras[0] = 12;
			old_talk.talk_nodes[0].extras[1] = 13;
			old_talk.talk_nodes[0].extras[2] = 14;
			old_talk.talk_nodes[0].extras[3] = 15;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::END_FORCE);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
			CHECK(talk.talk_nodes[0].extras[1] == 13);
			CHECK(talk.talk_nodes[0].extras[2] == 14);
			CHECK(talk.talk_nodes[0].extras[3] == 15);
		}
		SECTION("Force end + hostile") {
			old_talk.talk_nodes[0].type = 26;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::END_FIGHT);
		}
		SECTION("Force end + alarm") {
			old_talk.talk_nodes[0].type = 27;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::END_ALARM);
		}
		SECTION("Force end + die") {
			old_talk.talk_nodes[0].type = 28;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::END_DIE);
		}
		SECTION("Call town special") {
			old_talk.talk_nodes[0].type = 29;
			old_talk.talk_nodes[0].extras[0] = 12;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::CALL_TOWN_SPEC);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
		}
		SECTION("Call scenario special") {
			old_talk.talk_nodes[0].type = 30;
			old_talk.talk_nodes[0].extras[0] = 12;
			talk.import_legacy(old_talk, shops);
			REQUIRE(talk.talk_nodes.size() >= 1);
			CHECK(talk.talk_nodes[0].type == eTalkNode::CALL_SCEN_SPEC);
			CHECK(talk.talk_nodes[0].extras[0] == 12);
		}
	}
}
#if 0
enum class eTalkNode {
	REGULAR = 0,
	DEP_ON_SDF = 1,
	SET_SDF = 2,
	INN = 3,
	DEP_ON_TIME = 4,
	DEP_ON_TIME_AND_EVENT = 5,
	DEP_ON_TOWN = 6,
	SHOP = 7,
	TRAINING = 8,
	JOB_BANK = 9,
	SELL_WEAPONS = 13,
	SELL_ARMOR = 14,
	SELL_ITEMS = 15,
	IDENTIFY = 16,
	ENCHANT = 17,
	BUY_INFO = 18,
	BUY_SDF = 19,
	BUY_SHIP = 20,
	BUY_HORSE = 21,
	BUY_SPEC_ITEM = 22,
	RECEIVE_QUEST = 23,
	BUY_TOWN_LOC = 24,
	END_FORCE = 25,
	END_FIGHT = 26,
	END_ALARM = 27, // Town hostile
	END_DIE = 28,
	CALL_TOWN_SPEC = 29,
	CALL_SCEN_SPEC = 30,
};
#endif
