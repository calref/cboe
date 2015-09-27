//
//  monst_read.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-07-29.
//
//

#include <fstream>
#include "ticpp.h"
#include "catch.hpp"
#include "dialog.hpp"
#include "scenario.hpp"

using namespace std;
using namespace ticpp;

extern Document xmlDocFromStream(istream& stream, string name);
extern void readMonstersFromXml(Document&& data, cScenario& scenario);

TEST_CASE("Loading a monster type definition") {
	ifstream fin;
	cScenario scen;
	Document doc;
	fin.exceptions(ios::badbit);
	
	SECTION("When the root tag is wrong") {
		fin.open("files/bad_root.xml");
		doc = xmlDocFromStream(fin, "bad_root.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the version attribute is missing") {
		fin.open("files/monsters/no_version.xml");
		doc = xmlDocFromStream(fin, "no_version.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("When the root tag has a bad attribute") {
		fin.open("files/monsters/bad_root_attr.xml");
		doc = xmlDocFromStream(fin, "bad_root_attr.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("When an unknown toplevel tag appears") {
		fin.open("files/monsters/bad_toplevel.xml");
		doc = xmlDocFromStream(fin, "bad_toplevel.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the ID attribute is missing") {
		fin.open("files/monsters/missing_id.xml");
		doc = xmlDocFromStream(fin, "missing_id.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), Exception);
	}
	SECTION("When the ID attribute is zero") {
		fin.open("files/monsters/bad_id.xml");
		doc = xmlDocFromStream(fin, "missing_id.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
	}
	SECTION("When a required subtag is missing") {
		fin.open("files/monsters/missing_req.xml");
		doc = xmlDocFromStream(fin, "missing_req.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("When a bad subtag is found") {
		fin.open("files/monsters/bad_tag.xml");
		doc = xmlDocFromStream(fin, "bad_tag.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When an attack has a bad subtag") {
		fin.open("files/monsters/bad_attack_tag.xml");
		doc = xmlDocFromStream(fin, "bad_attack_tag.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When an attack has a bad attribute") {
		fin.open("files/monsters/bad_attack_attr.xml");
		doc = xmlDocFromStream(fin, "bad_attack_attr.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("When an attack has a bad type") {
		fin.open("files/monsters/bad_attack_type.xml");
		doc = xmlDocFromStream(fin, "bad_attack_type.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), Exception);
	}
	SECTION("When an attack damage is invalid") {
		fin.open("files/monsters/bad_attack.xml");
		doc = xmlDocFromStream(fin, "bad_attack.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
	}
	SECTION("When an attack damage is missing 'd'") {
		fin.open("files/monsters/bad_attack2.xml");
		doc = xmlDocFromStream(fin, "bad_attack2.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
	}
	SECTION("When an attack is missing the type") {
		fin.open("files/monsters/missing_attack_type.xml");
		doc = xmlDocFromStream(fin, "missing_attack_type.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("When the picture is missing the size attributes") {
		fin.open("files/monsters/bad_pic.xml");
		doc = xmlDocFromStream(fin, "bad_pic.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("When the picture has a bad attribute") {
		fin.open("files/monsters/bad_pic2.xml");
		doc = xmlDocFromStream(fin, "bad_pic2.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("When there is a bad immunity") {
		fin.open("files/monsters/bad_immune.xml");
		doc = xmlDocFromStream(fin, "bad_immune.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the loot has a bad subtag") {
		fin.open("files/monsters/bad_loot.xml");
		doc = xmlDocFromStream(fin, "bad_loot.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("When the loot has a missing subtag") {
		fin.open("files/monsters/bad_loot2.xml");
		doc = xmlDocFromStream(fin, "bad_loot2.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
	}
	SECTION("With the minimal required data") {
		fin.open("files/monsters/minimal.xml");
		doc = xmlDocFromStream(fin, "minimal.xml");
		REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
		REQUIRE(scen.scen_monsters.size() >= 2);
		CHECK(scen.scen_monsters[1].m_name == "Test Monster");
		CHECK(scen.scen_monsters[1].level == 1);
		CHECK(scen.scen_monsters[1].armor == 0);
		CHECK(scen.scen_monsters[1].skill == 2);
		CHECK(scen.scen_monsters[1].m_health == 10);
		CHECK(scen.scen_monsters[1].speed == 4);
		CHECK(scen.scen_monsters[1].m_type == eRace::HUMANOID);
		CHECK(scen.scen_monsters[1].picture_num == 5);
		CHECK(scen.scen_monsters[1].x_width == 1);
		CHECK(scen.scen_monsters[1].y_width == 1);
		CHECK(scen.scen_monsters[1].default_attitude == eAttitude::HOSTILE_A);
	}
	SECTION("With some attacks") {
		fin.open("files/monsters/attacks.xml");
		doc = xmlDocFromStream(fin, "attacks.xml");
		REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
		REQUIRE(scen.scen_monsters.size() >= 2);
		CHECK(scen.scen_monsters[1].a[0].type == eMonstMelee::SWING);
		CHECK(scen.scen_monsters[1].a[0].dice == 1);
		CHECK(scen.scen_monsters[1].a[0].sides == 10);
		CHECK(scen.scen_monsters[1].a[1].type == eMonstMelee::PUNCH);
		CHECK(scen.scen_monsters[1].a[1].dice == 2);
		CHECK(scen.scen_monsters[1].a[1].sides == 4);
		CHECK(scen.scen_monsters[1].a[2].type == eMonstMelee::BURN);
		CHECK(scen.scen_monsters[1].a[2].dice == 1);
		CHECK(scen.scen_monsters[1].a[2].sides == 8);
	}
	SECTION("With some immunities") {
		fin.open("files/monsters/immunity.xml");
		doc = xmlDocFromStream(fin, "immunity.xml");
		REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
		REQUIRE(scen.scen_monsters.size() >= 2);
		CHECK(scen.scen_monsters[1].mindless);
		CHECK(scen.scen_monsters[1].invuln);
		CHECK(scen.scen_monsters[1].resist[eDamageType::FIRE] == 10);
		CHECK(scen.scen_monsters[1].resist[eDamageType::COLD] == 20);
		CHECK(scen.scen_monsters[1].resist[eDamageType::MAGIC] == 30);
		CHECK(scen.scen_monsters[1].resist[eDamageType::POISON] == 40);
		CHECK(scen.scen_monsters[1].resist[eDamageType::UNDEAD] == 50);
		CHECK(scen.scen_monsters[1].resist[eDamageType::DEMON] == 60);
		CHECK(scen.scen_monsters[1].resist[eDamageType::UNBLOCKABLE] == 70);
		CHECK(scen.scen_monsters[1].resist[eDamageType::WEAPON] == 80);
		CHECK(scen.scen_monsters[1].resist[eDamageType::SPECIAL] == 90);
		// TODO: Setting special damage resistance shouldn't be allowed though
	}
	SECTION("With some misc optional data") {
		fin.open("files/monsters/optional.xml");
		doc = xmlDocFromStream(fin, "optional.xml");
		REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
		REQUIRE(scen.scen_monsters.size() >= 2);
		CHECK(scen.scen_monsters[1].speed == 3);
		CHECK(scen.scen_monsters[1].mu == 1);
		CHECK(scen.scen_monsters[1].cl == 2);
		CHECK(scen.scen_monsters[1].treasure == 4);
		CHECK(scen.scen_monsters[1].corpse_item == 128);
		CHECK(scen.scen_monsters[1].corpse_item_chance == 35);
		CHECK(scen.scen_monsters[1].summon_type == 3);
		CHECK(scen.scen_monsters[1].default_facial_pic == 12);
		CHECK(scen.scen_monsters[1].ambient_sound == 42);
		CHECK(scen.scen_monsters[1].see_spec == 91);
	}
}

TEST_CASE("Loading monster abilities") {
	ifstream fin;
	cScenario scen;
	Document doc;
	fin.exceptions(ios::badbit);
	
	SECTION("With an invalid category") {
		fin.open("files/monsters/bad_abil_type_tag.xml");
		doc = xmlDocFromStream(fin, "bad_abil_type_tag.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
	}
	SECTION("With missing type attribute") {
		fin.open("files/monsters/bad_abil_no_type.xml");
		doc = xmlDocFromStream(fin, "bad_abil_no_type.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingAttr);
	}
	SECTION("With an invalid attribute") {
		fin.open("files/monsters/bad_abil_bad_attr.xml");
		doc = xmlDocFromStream(fin, "bad_abil_bad_attr.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadAttr);
	}
	SECTION("With an invalid type") {
		fin.open("files/monsters/bad_abil_type_attr.xml");
		doc = xmlDocFromStream(fin, "bad_abil_type_attr.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), Exception);
	}
	SECTION("With a type of none") {
		fin.open("files/monsters/bad_abil_type_none.xml");
		doc = xmlDocFromStream(fin, "bad_abil_type_none.xml");
		REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
	}
	SECTION("General Abilities") {
		SECTION("With a bad ability type") {
			fin.open("files/monsters/abil_gen/bad_type.xml");
			doc = xmlDocFromStream(fin, "bad_type.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
		}
		SECTION("With an invalid subtag") {
			fin.open("files/monsters/abil_gen/bad_node.xml");
			doc = xmlDocFromStream(fin, "bad_node.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Missing a required subtag") {
			fin.open("files/monsters/abil_gen/missing_elem.xml");
			doc = xmlDocFromStream(fin, "missing_elem.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
		}
		SECTION("Minimal touch ability") {
			fin.open("files/monsters/abil_gen/minimal_touch.xml");
			doc = xmlDocFromStream(fin, "minimal_touch.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].gen.type == eMonstGen::TOUCH);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].gen.strength == 8);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].gen.odds == 600);
		}
		SECTION("Minimal ranged ability") {
			fin.open("files/monsters/abil_gen/minimal_range.xml");
			doc = xmlDocFromStream(fin, "minimal_range.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].gen.type == eMonstGen::RAY);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].gen.strength == 8);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].gen.odds == 600);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].gen.pic == 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DRAIN_SP].gen.range == 10);
		}
		SECTION("With an extra value when not needed") {
			fin.open("files/monsters/abil_gen/bad_extra.xml");
			doc = xmlDocFromStream(fin, "bad_extra.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Damage ability without type") {
			fin.open("files/monsters/abil_gen/bad_damage.xml");
			doc = xmlDocFromStream(fin, "bad_damage.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
		}
		SECTION("Field ability without type") {
			fin.open("files/monsters/abil_gen/bad_field.xml");
			doc = xmlDocFromStream(fin, "bad_field.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
		}
		SECTION("Status ability without type") {
			fin.open("files/monsters/abil_gen/bad_status.xml");
			doc = xmlDocFromStream(fin, "bad_status.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
		}
		SECTION("Ranged damage ability") {
			fin.open("files/monsters/abil_gen/minimal_damage.xml");
			doc = xmlDocFromStream(fin, "minimal_damage.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DAMAGE].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DAMAGE].gen.type == eMonstGen::RAY);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DAMAGE].gen.strength == 8);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DAMAGE].gen.odds == 600);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DAMAGE].gen.pic == 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DAMAGE].gen.range == 10);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::DAMAGE].gen.dmg == eDamageType::FIRE);
		}
		SECTION("Ranged field ability") {
			fin.open("files/monsters/abil_gen/minimal_field.xml");
			doc = xmlDocFromStream(fin, "minimal_field.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::FIELD].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::FIELD].gen.type == eMonstGen::RAY);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::FIELD].gen.strength == 8);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::FIELD].gen.odds == 600);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::FIELD].gen.pic == 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::FIELD].gen.range == 10);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::FIELD].gen.fld == CLOUD_SLEEP);
		}
		SECTION("Ranged status ability") {
			fin.open("files/monsters/abil_gen/minimal_status.xml");
			doc = xmlDocFromStream(fin, "minimal_status.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::STATUS].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::STATUS].gen.type == eMonstGen::RAY);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::STATUS].gen.strength == 8);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::STATUS].gen.odds == 600);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::STATUS].gen.pic == 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::STATUS].gen.range == 10);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::STATUS].gen.stat == eStatus::POISON);
		}
	}
	SECTION("Missile Abilities") {
		SECTION("With a bad ability type") {
			fin.open("files/monsters/abil_missile/bad_type.xml");
			doc = xmlDocFromStream(fin, "bad_type.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
		}
		SECTION("With an invalid subtag") {
			fin.open("files/monsters/abil_missile/bad_node.xml");
			doc = xmlDocFromStream(fin, "bad_node.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Missing a required subtag") {
			fin.open("files/monsters/abil_missile/missing_elem.xml");
			doc = xmlDocFromStream(fin, "missing_elem.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
		}
		SECTION("Minimal ability") {
			fin.open("files/monsters/abil_missile/minimal.xml");
			doc = xmlDocFromStream(fin, "minimal.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile.type == eMonstMissile::ARROW);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile.pic == 3);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile.dice == 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile.sides == 6);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile.skill == 8);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile.range == 10);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::MISSILE].missile.odds == 800);
		}
	}
	SECTION("Radiate Abilities") {
		SECTION("With a bad ability type") {
			fin.open("files/monsters/abil_radiate/bad_type.xml");
			doc = xmlDocFromStream(fin, "bad_type.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
		}
		SECTION("With an invalid subtag") {
			fin.open("files/monsters/abil_radiate/bad_node.xml");
			doc = xmlDocFromStream(fin, "bad_node.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Missing a required subtag") {
			fin.open("files/monsters/abil_radiate/missing_elem.xml");
			doc = xmlDocFromStream(fin, "missing_elem.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
		}
		SECTION("Minimal ability") {
			fin.open("files/monsters/abil_radiate/minimal.xml");
			doc = xmlDocFromStream(fin, "minimal.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RADIATE].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RADIATE].radiate.type == CLOUD_SLEEP);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RADIATE].radiate.pat == PAT_SQ);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RADIATE].radiate.chance == 75);
		}
		SECTION("With non-default spell pattern") {
			fin.open("files/monsters/abil_radiate/pattern.xml");
			doc = xmlDocFromStream(fin, "pattern.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RADIATE].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RADIATE].radiate.type == CLOUD_SLEEP);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RADIATE].radiate.pat == PAT_PLUS);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RADIATE].radiate.chance == 75);
		}
	}
	SECTION("Summon Abilities") {
		SECTION("With a bad ability type") {
			fin.open("files/monsters/abil_summon/bad_type.xml");
			doc = xmlDocFromStream(fin, "bad_type.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
		}
		SECTION("With an invalid subtag") {
			fin.open("files/monsters/abil_summon/bad_node.xml");
			doc = xmlDocFromStream(fin, "bad_node.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
		}
		SECTION("Missing a required subtag") {
			fin.open("files/monsters/abil_summon/missing_elem.xml");
			doc = xmlDocFromStream(fin, "missing_elem.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xMissingElem);
		}
		SECTION("Minimal type ability") {
			fin.open("files/monsters/abil_summon/minimal_type.xml");
			doc = xmlDocFromStream(fin, "minimal_type.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.type == eMonstSummon::TYPE);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.what == 32);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.min == 1);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.max == 5);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.len == 15);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.chance == 500);
		}
		SECTION("Minimal level ability") {
			fin.open("files/monsters/abil_summon/minimal_lvl.xml");
			doc = xmlDocFromStream(fin, "minimal_lvl.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.type == eMonstSummon::LEVEL);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.what == 3);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.min == 1);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.max == 5);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.len == 15);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.chance == 500);
		}
		SECTION("Minimal race ability") {
			fin.open("files/monsters/abil_summon/minimal_race.xml");
			doc = xmlDocFromStream(fin, "minimal_race.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.type == eMonstSummon::SPECIES);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.what == int(eRace::REPTILE));
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.min == 1);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.max == 5);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.len == 15);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::SUMMON].summon.chance == 500);
		}
	}
	SECTION("Special Abilities") {
		SECTION("With a bad ability type") {
			fin.open("files/monsters/abil_spec/bad_type.xml");
			doc = xmlDocFromStream(fin, "bad_type.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadVal);
		}
		SECTION("With an invalid subtag") {
			fin.open("files/monsters/abil_spec/bad_node.xml");
			doc = xmlDocFromStream(fin, "bad_node.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
		}
		SECTION("With too many parameters") {
			fin.open("files/monsters/abil_spec/too_many_params.xml");
			doc = xmlDocFromStream(fin, "too_many_params.xml");
			REQUIRE_THROWS_AS(readMonstersFromXml(move(doc), scen), xBadNode);
		}
		SECTION("With all parameters") {
			fin.open("files/monsters/abil_spec/minimal.xml");
			doc = xmlDocFromStream(fin, "minimal.xml");
			REQUIRE_NOTHROW(readMonstersFromXml(move(doc), scen));
			REQUIRE(scen.scen_monsters.size() >= 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RAY_HEAT].active);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RAY_HEAT].special.extra1 == 1);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RAY_HEAT].special.extra2 == 2);
			CHECK(scen.scen_monsters[1].abil[eMonstAbil::RAY_HEAT].special.extra3 == 3);
		}
	}
}
