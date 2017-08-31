//
//  view_dialogs.cpp
//  BoE
//
//  Created by Celtic Minstrel on 17-01-21.
//
//

#include "view_dialogs.hpp"

#include "dialog.hpp"
#include "pict.hpp"
#include "button.hpp"

#include "item.hpp"
#include "creature.hpp"
#include "scenario.hpp"
#include "utility.hpp"

void put_item_info(cDialog& me, const cItem& s_i, const cScenario& scen) {
	std::string desc_str;
	
	cPict& pic = dynamic_cast<cPict&>(me["pic"]);
	pic.setPict(s_i.graphic_num, PIC_ITEM);
	
	// id? magic?
	cLed& id = dynamic_cast<cLed&>(me["id"]);
	if(s_i.ident)
		id.setState(led_red);
	else id.setState(led_off);
	cLed& magic = dynamic_cast<cLed&>(me["magic"]);
	if(s_i.magic && s_i.ident)
		magic.setState(led_red);
	else magic.setState(led_off);
	me["type"].setText(get_str("item-types-display", int(s_i.variety) + 1));
	
	// Clear fields
	me["val"].setText("");
	me["dmg"].setText("");
	me["bonus"].setText("");
	me["def"].setText("");
	me["enc"].setText("");
	me["use"].setText("");
	me["lvl"].setText("");
	me["abil"].setText("");
	
	if(!s_i.ident) {
		me["name"].setText(s_i.name);
		return;
	}
	
	me["name"].setText(s_i.full_name);
	me["weight"].setTextToNum(s_i.item_weight());
	me["desc"].setText(s_i.desc.substr(0,s_i.desc.find("|||")));
	
	// TODO: This calculation (value for an item with charges) should be in a member function of cItem
	me["val"].setTextToNum((s_i.charges > 0) ? s_i.value * s_i.charges : s_i.value);
	
	if(s_i.ability != eItemAbil::NONE) {
		if(s_i.concealed) {
			me["abil"].setText("???");
		} else {
			std::string abil = s_i.getAbilName();
			if(s_i.ability == eItemAbil::SUMMONING || s_i.ability == eItemAbil::MASS_SUMMONING)
				abil.replace(abil.find("%s"), 2, scen.scen_monsters[s_i.abil_data[1]].m_name);
			me["abil"].setText(abil);
		}
	}
	if(s_i.charges > 0)
		me["use"].setTextToNum(s_i.charges);
	if(s_i.protection > 0)
		me["def"].setTextToNum(s_i.protection);
	
	std::string store_text;
	switch(s_i.variety) {
		case eItemType::ONE_HANDED:
		case eItemType::TWO_HANDED:
		case eItemType::BOW:
		case eItemType::CROSSBOW:
		case eItemType::THROWN_MISSILE:
		case eItemType::MISSILE_NO_AMMO:
			if(s_i.ability == eItemAbil::NONE)
				me["abil"].setText("Key skill: " + get_str("skills", int(s_i.weap_type) * 2 + 1));
		case eItemType::ARROW:
		case eItemType::BOLTS:
			me["dmg"].setTextToNum(s_i.item_level);
			me["bonus"].setTextToNum(s_i.bonus);
			break;
		case eItemType::POTION:
		case eItemType::RING:
		case eItemType::SCROLL: // TODO: Does this make sense for a scroll, though?
		case eItemType::TOOL: // and what about for a tool?
		case eItemType::WAND: // and a wand? Maybe showing ability strength would be better...
		case eItemType::NECKLACE: // TODO: This doesn't seem right for a necklace...
			me["lvl"].setTextToNum(s_i.item_level);
			break;
		case eItemType::SHIELD:
		case eItemType::ARMOR:
		case eItemType::HELM:
		case eItemType::GLOVES:
		case eItemType::SHIELD_2:
		case eItemType::BOOTS: // TODO: Should this also check eItemType::PANTS?
			// TODO: Why is bonus and protection combined into "bonus"? Why not use the "defense" field?
			me["bonus"].setTextToNum(s_i.bonus + s_i.protection);
			me["def"].setTextToNum(s_i.item_level);
			me["enc"].setTextToNum(s_i.awkward);
			break;
		case eItemType::WEAPON_POISON:
			me["lvl"].setTextToNum(s_i.item_level);
			break;
		default:
			// no item, gold, food, non-use, unused 1 and 2: do nothing
			break;
	}
	
}

void put_monst_info(cDialog& me, const cMonster& store_m, const cScenario& scen) {
	std::string store_text;
	std::string str;
	short i;
	
	cPict& pic = dynamic_cast<cPict&>(me["pic"]);
	if(store_m.invisible)
		pic.setPict(-1,PIC_MONST);
	else if(store_m.picture_num < 1000)
		pic.setPict(store_m.picture_num,PIC_MONST);
	else {
		ePicType type_g = PIC_CUSTOM_MONST;
		short size_g = store_m.picture_num / 1000;
		switch(size_g){
			case 2:
				type_g += PIC_WIDE;
				break;
			case 3:
				type_g += PIC_TALL;
				break;
			case 4:
				type_g += PIC_WIDE;
				type_g += PIC_TALL;
				break;
		}
		pic.setPict(store_m.picture_num % 1000, type_g);
	}
	
	me["name"].setText(store_m.m_name);
	
	i = 1;
	for(auto& abil : store_m.abil) {
		if(i > 4) break; // TODO: Support showing more than just the first four abilities
		if(!abil.second.active) continue;
		std::string id = "abil" + std::to_string(i);
		std::string name = abil.second.to_string(abil.first);
		if(abil.first == eMonstAbil::SUMMON && abil.second.summon.type == eMonstSummon::TYPE)
			name.replace(name.find("%s"), 2, scen.scen_monsters[abil.second.summon.what].m_name);
		me[id].setText(name);
		i++;
	}
	
	for(short i = 0; i < store_m.a.size(); i++) {
		if(store_m.a[i].dice > 0) {
			if(store_m.a[i].sides == 0) continue;
			std::ostringstream sout(std::ios_base::ate);
			sout << store_m.a[i];
			store_text = sout.str();
			sout.str("attack");
			sout << i + 1;
			me[sout.str()].setText(store_text);
		}
	}
	me["lvl"].setTextToNum(store_m.level);
	me["hp"].setTextToNum(store_m.m_health);
	me["sp"].setTextToNum((store_m.mu + store_m.cl) ? store_m.level * 12 : 0);
	me["def"].setTextToNum(store_m.armor);
	me["skill"].setTextToNum(store_m.skill);
	int morale = 10 * store_m.level;
	if(store_m.level > 20)
		morale += 10 * (store_m.level - 20);
	me["morale"].setTextToNum(morale);
	me["ap"].setTextToNum(store_m.speed);
	me["mage"].setTextToNum(store_m.mu);
	me["priest"].setTextToNum(store_m.cl);
	// Immunities
	me["magic-res"].setText(std::to_string(100 - store_m.resist.at(eDamageType::MAGIC)) + '%');
	me["fire-res"].setText(std::to_string(100 - store_m.resist.at(eDamageType::FIRE)) + '%');
	me["cold-res"].setText(std::to_string(100 - store_m.resist.at(eDamageType::COLD)) + '%');
	me["poison-res"].setText(std::to_string(100 - store_m.resist.at(eDamageType::POISON)) + '%');
	dynamic_cast<cLed&>(me["mindless"]).setState(store_m.mindless ? led_red : led_off);
	dynamic_cast<cLed&>(me["invuln"]).setState(store_m.invuln ? led_red : led_off);
	dynamic_cast<cLed&>(me["guard"]).setState(store_m.guard ? led_red : led_off);
}

void put_monst_info(cDialog& me, const cCreature& store_m, const cScenario& scen) {
	put_monst_info(me, static_cast<const cMonster&>(store_m), scen);
	me["hp"].setTextToNum(store_m.health);
	me["sp"].setTextToNum(store_m.mp);
	me["morale"].setTextToNum(store_m.morale);
}
