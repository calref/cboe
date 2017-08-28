
#include <fstream>
#include <iostream>
#include "ticpp.h"
#include "dialog.hpp"
#include "catch.hpp"
#include "scenario.hpp"
#include "res_strings.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readScenarioFromXml(Document&& data, cScenario& scenario);

TEST_CASE("Loading a new-format scenario record") {
	ifstream fin;
	cScenario scen;
	Document doc;
	fin.exceptions(ios::badbit);
	
	SECTION("When the root tag is wrong") {
		fin.open("files/bad_root.xml");
		doc = xmlDocFromStream(fin, "bad_root.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the version attribute is missing") {
		fin.open("files/scenario/no_version.xml");
		doc = xmlDocFromStream(fin, "no_version.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("When the root tag has a bad attribute") {
		fin.open("files/scenario/bad_root_attr.xml");
		doc = xmlDocFromStream(fin, "bad_root_attr.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("When an essential toplevel element is missing") {
		fin.open("files/scenario/missing_toplevel.xml");
		doc = xmlDocFromStream(fin, "missing_toplevel.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When an invalid toplevel element is present") {
		fin.open("files/scenario/bad_toplevel.xml");
		doc = xmlDocFromStream(fin, "bad_toplevel.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When there are too many intro strings") {
		fin.open("files/scenario/intro_overflow.xml");
		doc = xmlDocFromStream(fin, "intro_overflow.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the scenario pic element is empty") {
		fin.open("files/scenario/missing_pic.xml");
		doc = xmlDocFromStream(fin, "missing_pic.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), Exception);
	}
	SECTION("When the scenario version is invalid") {
		fin.open("files/scenario/bad_version.xml");
		doc = xmlDocFromStream(fin, "bad_version.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), string);
	}
	SECTION("When the author email is missing") {
		fin.open("files/scenario/missing_email.xml");
		doc = xmlDocFromStream(fin, "missing_email.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), Exception);
	}
	SECTION("When a scenario rating is missing") {
		fin.open("files/scenario/missing_rating.xml");
		doc = xmlDocFromStream(fin, "missing_rating.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), Exception);
	}
	SECTION("When the scenario content rating is invalid") {
		fin.open("files/scenario/bad_rating.xml");
		doc = xmlDocFromStream(fin, "bad_rating.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), Exception);
	}
	SECTION("When the scenario difficulty is invalid") {
		fin.open("files/scenario/bad_difficulty.xml");
		doc = xmlDocFromStream(fin, "bad_difficulty.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadVal);
	}
	SECTION("When there are too many teaser strings") {
		fin.open("files/scenario/extra_teaser.xml");
		doc = xmlDocFromStream(fin, "extra_teaser.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When some flags are missing") {
		fin.open("files/scenario/missing_flags.xml");
		doc = xmlDocFromStream(fin, "missing_flags.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When a flag is invalid") {
		fin.open("files/scenario/bad_flag.xml");
		doc = xmlDocFromStream(fin, "bad_flag.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When it's not OBoE format") {
		fin.open("files/scenario/bad_format.xml");
		doc = xmlDocFromStream(fin, "bad_format.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadVal);
	}
	SECTION("When an essential <game> child element is missing") {
		fin.open("files/scenario/missing_game.xml");
		doc = xmlDocFromStream(fin, "missing_game.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When an essential <editor> child element is missing") {
		fin.open("files/scenario/missing_editor.xml");
		doc = xmlDocFromStream(fin, "missing_editor.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("With an erroneous <graphics> section") {
		SECTION("Two of them") {
			fin.open("files/scenario/bad_graphics1.xml");
			doc = xmlDocFromStream(fin, "bad_graphics1.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Invalid child element") {
			fin.open("files/scenario/bad_graphics2.xml");
			doc = xmlDocFromStream(fin, "bad_graphics2.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Invalid attribute") {
			fin.open("files/scenario/bad_graphics4.xml");
			doc = xmlDocFromStream(fin, "bad_graphics4.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Missing index attribute") {
			fin.open("files/scenario/bad_graphics3.xml");
			doc = xmlDocFromStream(fin, "bad_graphics3.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Bad graphic type") {
			fin.open("files/scenario/bad_graphics5.xml");
			doc = xmlDocFromStream(fin, "bad_graphics5.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadVal);
		}
	}
	SECTION("With an invalid game subtag") {
		fin.open("files/scenario/bad_game_node.xml");
		doc = xmlDocFromStream(fin, "bad_game_node.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
	}
	SECTION("With an invalid editor subtag") {
		fin.open("files/scenario/bad_editor_node.xml");
		doc = xmlDocFromStream(fin, "bad_editor_node.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
	}
	SECTION("With a sound name for a preset sound") {
		fin.open("files/scenario/bad_snd_name.xml");
		doc = xmlDocFromStream(fin, "bad_snd_name.xml");
		REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadVal);
	}
	SECTION("With the minimal required data") {
		fin.open("files/scenario/minimal.xml");
		doc = xmlDocFromStream(fin, "minimal.xml");
		REQUIRE_NOTHROW(readScenarioFromXml(move(doc), scen));
		CHECK(scen.scen_name == "Test Scenario");
		CHECK(scen.intro_pic == 7);
		CHECK(scen.campaign_id == "campaign");
		CHECK(scen.format.prog_make_ver[0] == 2);
		CHECK(scen.format.prog_make_ver[1] == 5);
		CHECK(scen.format.prog_make_ver[2] == 3);
		CHECK(scen.format.ver[0] == 2);
		CHECK(scen.format.ver[1] == 6);
		CHECK(scen.format.ver[2] == 7);
		// TODO: Check language
		CHECK(scen.contact_info[0] == "BoE Test Suite");
		CHECK(scen.contact_info[1] == "nowhere@example.com");
		CHECK(scen.who_wrote[0] == "Teaser 1");
		CHECK(scen.who_wrote[1] == "Teaser 2");
		CHECK(scen.intro_mess_pic == 7);
		CHECK(scen.intro_strs[0] == "Welcome!!!    To the test scenario!");
		CHECK(scen.rating == eContentRating::R);
		CHECK(scen.difficulty == 2); // Difficulty is 3, but it's stored as 2 (0-3 instead of 1-4)
		CHECK(scen.adjust_diff == true);
		CHECK(scen.is_legacy == false);
		CHECK(scen.uses_custom_graphics == false);
		CHECK(scen.towns.size() == 3);
		CHECK(scen.outdoors.width() == 2);
		CHECK(scen.outdoors.height() == 1);
		CHECK(scen.which_town_start == 7);
		CHECK(scen.where_start == loc(24,28));
		CHECK(scen.out_sec_start == loc(1,3));
		CHECK(scen.out_start == loc(12,21));
		CHECK(scen.default_ground == 2);
		CHECK(scen.last_out_edited == loc(0,0));
		CHECK(scen.last_town_edited == 0);
		CHECK(scen.init_spec == -1);
	}
	SECTION("With a different icon in the intro dialog") {
		fin.open("files/scenario/intro.xml");
		doc = xmlDocFromStream(fin, "intro.xml");
		REQUIRE_NOTHROW(readScenarioFromXml(move(doc), scen));
		CHECK(scen.intro_pic == 7);
		CHECK(scen.intro_mess_pic == 12);
	}
	SECTION("With some optional game data") {
		fin.open("files/scenario/optional.xml");
		doc = xmlDocFromStream(fin, "optional.xml");
		REQUIRE_NOTHROW(readScenarioFromXml(move(doc), scen));
		CHECK(scen.init_spec == 5);;
		REQUIRE(scen.store_item_rects.size() >= 1);
		REQUIRE(scen.store_item_rects.size() >= 1);
		CHECK(scen.store_item_rects[0] == rectangle(12,13,20,36));
		CHECK(scen.store_item_towns[0] == 0);
		REQUIRE(scen.town_mods.size() >= 1);
		CHECK(scen.town_mods[0] == loc(16,21));
		CHECK(scen.town_mods[0].spec == 1);
		REQUIRE(scen.scenario_timers.size() >= 1);
		CHECK(scen.scenario_timers[0].time == 100);
		CHECK(scen.scenario_timers[0].node == 15);
		CHECK(scen.scenario_timers[0].node_type == 0);
		REQUIRE(scen.snd_names.size() >= 1);
		CHECK(scen.snd_names[0] == "The name of my custom sound");
	}
	SECTION("With an invalid town mod flag") {
		SECTION("Missing X") {
			fin.open("files/scenario/bad_townmod1.xml");
			doc = xmlDocFromStream(fin, "bad_townmod1.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Missing Y") {
			fin.open("files/scenario/bad_townmod2.xml");
			doc = xmlDocFromStream(fin, "bad_townmod2.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Missing town") {
			fin.open("files/scenario/bad_townmod3.xml");
			doc = xmlDocFromStream(fin, "bad_townmod3.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Invalid attribtue") {
			fin.open("files/scenario/bad_townmod4.xml");
			doc = xmlDocFromStream(fin, "bad_townmod4.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Too many") {
			fin.open("files/scenario/too_many_flags.xml");
			doc = xmlDocFromStream(fin, "too_many_flags.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
	}
	SECTION("With an invalid save rectangle") {
		SECTION("With invalid attribute") {
			fin.open("files/scenario/rect_bad_attr.xml");
			doc = xmlDocFromStream(fin, "rect_bad_attr.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Missing top") {
			fin.open("files/scenario/rect_missing_top.xml");
			doc = xmlDocFromStream(fin, "rect_missing_top.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Missing left") {
			fin.open("files/scenario/rect_missing_left.xml");
			doc = xmlDocFromStream(fin, "rect_missing_left.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Missing bottom") {
			fin.open("files/scenario/rect_missing_bottom.xml");
			doc = xmlDocFromStream(fin, "rect_missing_bottom.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Missing right") {
			fin.open("files/scenario/rect_missing_right.xml");
			doc = xmlDocFromStream(fin, "rect_missing_right.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Missing town") {
			fin.open("files/scenario/rect_missing_town.xml");
			doc = xmlDocFromStream(fin, "rect_missing_town.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Too many") {
			fin.open("files/scenario/too_many_rects.xml");
			doc = xmlDocFromStream(fin, "too_many_rects.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
	}
	SECTION("With an invalid timer") {
		SECTION("With invalid attribute") {
			fin.open("files/scenario/timer_bad_attr.xml");
			doc = xmlDocFromStream(fin, "timer_bad_attr.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Missing frequency") {
			fin.open("files/scenario/timer_missing_freq.xml");
			doc = xmlDocFromStream(fin, "timer_missing_freq.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Too many") {
			fin.open("files/scenario/too_many_timers.xml");
			doc = xmlDocFromStream(fin, "too_many_timers.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
	}
	SECTION("With a special item") {
		SECTION("Valid") {
			fin.open("files/scenario/special_item.xml");
			doc = xmlDocFromStream(fin, "special_item.xml");
			REQUIRE_NOTHROW(readScenarioFromXml(move(doc), scen));
			CHECK(scen.special_items.size() == 1);
			CHECK(scen.special_items[0].name == "My Special Item");
			CHECK(scen.special_items[0].descr == "Special Item  --  Description!");
			CHECK(scen.special_items[0].flags == 0);
			CHECK(scen.special_items[0].special == -1);
		}
		SECTION("Invalid attribute") {
			fin.open("files/scenario/special_item-bad_attr.xml");
			doc = xmlDocFromStream(fin, "special_item-bad_attr.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Invalid element") {
			fin.open("files/scenario/special_item-bad_elem.xml");
			doc = xmlDocFromStream(fin, "special_item-bad_elem.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Missing name") {
			fin.open("files/scenario/special_item-missing_elem.xml");
			doc = xmlDocFromStream(fin, "special_item-missing_elem.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
		}
	}
	SECTION("With a quest") {
		SECTION("Valid minimal quest") {
			fin.open("files/scenario/quest.xml");
			doc = xmlDocFromStream(fin, "quest.xml");
			REQUIRE_NOTHROW(readScenarioFromXml(move(doc), scen));
			REQUIRE(scen.quests.size() == 1);
			CHECK(scen.quests[0].name == "My Silly Quest");
			CHECK(scen.quests[0].descr == " It is!   The best quest!   ");
			CHECK(scen.quests[0].flags == 0);
			CHECK(scen.quests[0].gold == 0);
			CHECK(scen.quests[0].xp == 0);
			CHECK(scen.quests[0].bank1 == -1);
			CHECK(scen.quests[0].bank2 == -1);
			CHECK(scen.quests[0].deadline == -1);
			CHECK(scen.quests[0].event == -1);
		}
		SECTION("Valid quest, more complex") {
			fin.open("files/scenario/quest2.xml");
			doc = xmlDocFromStream(fin, "quest2.xml");
			REQUIRE_NOTHROW(readScenarioFromXml(move(doc), scen));
			REQUIRE(scen.quests.size() == 1);
			CHECK(scen.quests[0].gold == 150);
			CHECK(scen.quests[0].xp == 1500);
			CHECK(scen.quests[0].deadline == 50);
			CHECK(scen.quests[0].event == 5);
			CHECK(scen.quests[0].bank1 == 1);
			CHECK(scen.quests[0].bank2 == -1);
		}
		SECTION("Invalid attribute") {
			fin.open("files/scenario/quest-bad_attr.xml");
			doc = xmlDocFromStream(fin, "quest-bad_attr.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Invalid element") {
			fin.open("files/scenario/quest-bad_elem.xml");
			doc = xmlDocFromStream(fin, "quest-bad_elem.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Missing name") {
			fin.open("files/scenario/quest-missing_elem.xml");
			doc = xmlDocFromStream(fin, "quest-missing_elem.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
		}
		SECTION("Invalid deadline") {
			fin.open("files/scenario/quest-bad_deadline.xml");
			doc = xmlDocFromStream(fin, "quest-bad_deadline.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Invalid reward") {
			fin.open("files/scenario/quest-bad_reward.xml");
			doc = xmlDocFromStream(fin, "quest-bad_reward.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Too many banks") {
			fin.open("files/scenario/quest-extra_bank.xml");
			doc = xmlDocFromStream(fin, "quest-extra_bank.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
	}
	SECTION("With a shop") {
		// Loading shops requires strings to be available
		// Here we fetch them from the rsrc dir, rather than the data dir
		ResMgr::pushPath<StringRsrc>("../rsrc/strings");
		SECTION("Valid shop") {
			fin.open("files/scenario/shop.xml");
			doc = xmlDocFromStream(fin, "shop.xml");
			REQUIRE_NOTHROW(readScenarioFromXml(move(doc), scen));
			REQUIRE(scen.shops.size() == 1);
			REQUIRE(scen.shops[0].size() == 14);
			CHECK(scen.shops[0].getFace() == 0);
			CHECK(scen.shops[0].getName() == "The Shop of Everything");
			CHECK(scen.shops[0].getType() == eShopType::NORMAL);
			CHECK(scen.shops[0].getPrompt() == eShopPrompt::SHOPPING);
			CHECK(scen.shops[0].getItem(0).type == eShopItemType::ITEM);
			CHECK(scen.shops[0].getItem(0).quantity == 3);
			CHECK(scen.shops[0].getItem(0).index == 10);
			CHECK(scen.shops[0].getItem(1).type == eShopItemType::ITEM);
			CHECK(scen.shops[0].getItem(1).quantity == 0);
			CHECK(scen.shops[0].getItem(1).index == 11);
			CHECK(scen.shops[0].getItem(2).type == eShopItemType::OPT_ITEM);
			CHECK(scen.shops[0].getItem(2).quantity == 42001);
			CHECK(scen.shops[0].getItem(2).index == 12);
			CHECK(scen.shops[0].getItem(3).type == eShopItemType::MAGE_SPELL);
			CHECK(scen.shops[0].getItem(3).item.item_level == 30);
			CHECK(scen.shops[0].getItem(4).type == eShopItemType::PRIEST_SPELL);
			CHECK(scen.shops[0].getItem(4).item.item_level == 31);
			CHECK(scen.shops[0].getItem(5).type == eShopItemType::ALCHEMY);
			CHECK(scen.shops[0].getItem(5).item.item_level == 14);
			CHECK(scen.shops[0].getItem(6).type == eShopItemType::SKILL);
			CHECK(scen.shops[0].getItem(6).item.item_level == 9);
			CHECK(scen.shops[0].getItem(7).type == eShopItemType::TREASURE);
			CHECK(scen.shops[0].getItem(7).item.item_level == 2);
			CHECK(scen.shops[0].getItem(8).type == eShopItemType::CLASS);
			CHECK(scen.shops[0].getItem(8).item.special_class == 147);
			CHECK(scen.shops[0].getItem(9).type == eShopItemType::CURE_DISEASE);
			CHECK(scen.shops[0].getItem(10).type == eShopItemType::CALL_SPECIAL);
			CHECK(scen.shops[0].getItem(10).quantity == 42);
			CHECK(scen.shops[0].getItem(10).item.graphic_num == 0);
			CHECK(scen.shops[0].getItem(10).item.item_level == 100);
			CHECK(scen.shops[0].getItem(10).item.value == 0);
			CHECK(scen.shops[0].getItem(10).item.full_name == "Special Shop Item the First");
			CHECK(scen.shops[0].getItem(10).item.desc == " Haha! It's a magic purchase!");
			CHECK(scen.shops[0].getItem(11).type == eShopItemType::CALL_SPECIAL);
			CHECK(scen.shops[0].getItem(11).quantity == 0);
			CHECK(scen.shops[0].getItem(11).item.graphic_num == 12);
			CHECK(scen.shops[0].getItem(11).item.item_level == 101);
			CHECK(scen.shops[0].getItem(11).item.value == 0);
			CHECK(scen.shops[0].getItem(11).item.full_name == "Special Shop Item the Second");
			CHECK(scen.shops[0].getItem(11).item.desc == "Another magic purchase!");
			CHECK(scen.shops[0].getItem(12).type == eShopItemType::CALL_SPECIAL);
			CHECK(scen.shops[0].getItem(12).quantity == 1);
			CHECK(scen.shops[0].getItem(12).item.graphic_num == 9);
			CHECK(scen.shops[0].getItem(12).item.item_level == 102);
			CHECK(scen.shops[0].getItem(12).item.value == 12000);
			CHECK(scen.shops[0].getItem(12).item.full_name == "Special Shop Item the Third");
			CHECK(scen.shops[0].getItem(12).item.desc == "Yay for the magic purchase!");
			CHECK(scen.shops[0].getItem(13).type == eShopItemType::ITEM);
			CHECK(scen.shops[0].getItem(13).quantity == 0);
			CHECK(scen.shops[0].getItem(13).index == 89);
		}
		SECTION("Too many items") {
			fin.open("files/scenario/shop-overflow.xml");
			doc = xmlDocFromStream(fin, "shop-overflow.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Invalid attribute on regular item") {
			fin.open("files/scenario/shop-bad_item.xml");
			doc = xmlDocFromStream(fin, "shop-bad_item.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Invalid element in special item") {
			fin.open("files/scenario/shop-bad_spec.xml");
			doc = xmlDocFromStream(fin, "shop-bad_spec.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Missing element in special item") {
			fin.open("files/scenario/shop-incomplete_spec.xml");
			doc = xmlDocFromStream(fin, "shop-incomplete_spec.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
		}
		SECTION("Invalid entry type") {
			fin.open("files/scenario/shop-bad_entry.xml");
			doc = xmlDocFromStream(fin, "shop-bad_entry.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Invalid toplevel tag") {
			fin.open("files/scenario/shop-bad_node.xml");
			doc = xmlDocFromStream(fin, "shop-bad_node.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Missing required tag") {
			fin.open("files/scenario/shop-missing_node.xml");
			doc = xmlDocFromStream(fin, "shop-missing_node.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
		}
		ResMgr::popPath<StringRsrc>();
	}
	SECTION("With item storage shortcuts") {
		SECTION("Valid") {
			fin.open("files/scenario/storage.xml");
			doc = xmlDocFromStream(fin, "storage.xml");
			REQUIRE_NOTHROW(readScenarioFromXml(move(doc), scen));
			REQUIRE(scen.storage_shortcuts.size() >= 1);
			CHECK(scen.storage_shortcuts[0].ter_type == 64);
			CHECK(scen.storage_shortcuts[0].property);
			CHECK(scen.storage_shortcuts[0].item_num[0] == 46);
			CHECK(scen.storage_shortcuts[0].item_odds[0] == 25);
		}
		SECTION("Missing terrain") {
			fin.open("files/scenario/storage-missing_ter.xml");
			doc = xmlDocFromStream(fin, "storage-missing_ter.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingElem);
		}
		SECTION("Missing chance") {
			fin.open("files/scenario/storage-missing_chance.xml");
			doc = xmlDocFromStream(fin, "storage-missing_chance.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xMissingAttr);
		}
		SECTION("Invalid attribute") {
			fin.open("files/scenario/storage-bad_attr.xml");
			doc = xmlDocFromStream(fin, "storage-bad_attr.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadAttr);
		}
		SECTION("Invalid subtag") {
			fin.open("files/scenario/storage-bad_node.xml");
			doc = xmlDocFromStream(fin, "storage-bad_node.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Too many items") {
			fin.open("files/scenario/storage-too_many_items.xml");
			doc = xmlDocFromStream(fin, "storage-too_many_items.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Too many shortcuts") {
			fin.open("files/scenario/storage-too_many.xml");
			doc = xmlDocFromStream(fin, "storage-too_many.xml");
			REQUIRE_THROWS_AS(readScenarioFromXml(move(doc), scen), xBadNode);
		}
	}
}
