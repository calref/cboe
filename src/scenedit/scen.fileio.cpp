
#include <cstring>
#include "scen.global.hpp"
#include "scenario.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/filesystem/operations.hpp>
#include "scen.fileio.hpp"
#include "scen.keydlgs.hpp"
#include "gfxsheets.hpp"
#include "scen.core.hpp"
#include "sounds.hpp"
#include "mathutil.hpp"
#include "oldstructs.hpp"
#include "fileio.hpp"
#include "tarball.hpp"
#include "gzstream.h"
#include "tinyprint.h"
#include "map_parse.hpp"
#include "winutil.hpp"
#include "choicedlog.hpp"
#include "undo.hpp"

extern cScenario scenario;

extern cTown* town;
extern short cur_town;
extern location cur_out;
extern cOutdoors* current_terrain;
extern bool change_made;
extern cCustomGraphics spec_scen_g;
extern bool mac_is_intel;

fs::path temp_file_to_load;
std::string last_load_file = "Blades of Exile Scenario";
extern fs::path progDir, tempDir;
extern bool cur_scen_is_mac;

void print_write_position ();
void load_spec_graphics();

// These aren't static solely so that the test cases can access them.
void writeScenarioToXml(ticpp::Printer&& data, cScenario& scenario);
void writeTerrainToXml(ticpp::Printer&& data, cScenario& scenario);
void writeItemsToXml(ticpp::Printer&& data, cScenario& scenario);
void writeMonstersToXml(ticpp::Printer&& data, cScenario& scenario);
void writeOutdoorsToXml(ticpp::Printer&& data, cOutdoors& sector);
void writeTownToXml(ticpp::Printer&& data, cTown& town);
void writeDialogueToXml(ticpp::Printer&& data, cSpeech& talk, int town_num);
map_data buildOutMapData(location which, cScenario& scenario);
map_data buildTownMapData(size_t which, cScenario& scenario);

template<typename Container> static void writeSpecialNodes(std::ostream& fout, Container nodes) {
	static_assert(std::is_same<typename Container::value_type, cSpecial>::value,
		"writeSpecialNodes must be instantiated with a container of special nodes");
	for(size_t i = 0; i < nodes.size(); i++) {
		nodes[i].writeTo(fout, i);
	}
}

static std::string boolstr(bool b) {
	return b ? "true" : "false";
}

template<> void ticpp::Printer::PushElement(std::string tagName, location pos, bool) {
	OpenElement(tagName);
	PushAttribute("x", pos.x);
	PushAttribute("y", pos.y);
	CloseElement(tagName);
}

template<> void ticpp::Printer::PushElement(std::string tagName, rectangle rect, bool) {
	OpenElement(tagName);
	PushAttribute("top", rect.top);
	PushAttribute("left", rect.left);
	PushAttribute("bottom", rect.bottom);
	PushAttribute("right", rect.right);
	CloseElement(tagName);
}

template<> void ticpp::Printer::PushElement(std::string tagName, cMonster::cAttack attack, bool) {
	OpenElement(tagName);
	PushAttribute("type", attack.type);
	std::ostringstream strength;
	strength << attack.dice << 'd' << attack.sides;
	PushText(strength.str());
	CloseElement(tagName);
}

template<> void ticpp::Printer::PushElement(std::string tagName, cOutdoors::cWandering enc, bool) {
	OpenElement(tagName);
	PushAttribute("can-flee", !enc.cant_flee);
	PushAttribute("force", enc.forced);
	for(size_t i = 0; i < enc.monst.size(); i++) {
		PushElement("monster", enc.monst[i]);
	}
	for(size_t i = 0; i < enc.friendly.size(); i++) {
		OpenElement("monster");
		PushAttribute("friendly", true);
		PushText(enc.friendly[i]);
		CloseElement("monster");
	}
	PushElement("onmeet", enc.spec_on_meet);
	PushElement("onwin", enc.spec_on_win);
	PushElement("onflee", enc.spec_on_flee);
	PushElement("sdf", loc(enc.end_spec1, enc.end_spec2));
	CloseElement(tagName);
}

template<> void ticpp::Printer::PushElement(std::string tagName, info_rect_t rect, bool cdata) {
	OpenElement(tagName);
	PushAttribute("top", rect.top);
	PushAttribute("left", rect.left);
	PushAttribute("bottom", rect.bottom);
	PushAttribute("right", rect.right);
	PushText(rect.descr, cdata);
	CloseElement(tagName);
}

void writeScenarioToXml(ticpp::Printer&& data, cScenario& scenario) {
	data.OpenElement("scenario");
	data.PushAttribute("boes", scenario.format_ed_version());
	data.PushElement("title", scenario.scen_name);
	data.PushElement("icon", scenario.intro_pic);
	data.PushElement("id", scenario.campaign_id);
	data.PushElement("version", scenario.format_scen_version());
	data.PushElement("language", "en-US");
	data.OpenElement("author");
	data.PushElement("name", scenario.contact_info[0]);
	data.PushElement("email", scenario.contact_info[1]);
	data.CloseElement("author");
	data.OpenElement("text");
	data.PushElement("teaser", scenario.who_wrote[0]);
	data.PushElement("teaser", scenario.who_wrote[1]);
	if(scenario.intro_pic != scenario.intro_mess_pic)
		data.PushElement("icon", scenario.intro_mess_pic);
	{
		int last = -1;
		for(int i = 0; i < scenario.intro_strs.size(); i++) {
			if(!scenario.intro_strs[i].empty())
				last = i;
		}
		for(int i = 0; i <= last; i++)
			data.PushElement("intro-msg", scenario.intro_strs[i], true);
	}
	data.CloseElement("text");
	data.OpenElement("ratings");
	data.PushElement("content", scenario.rating);
	data.PushElement("difficulty", scenario.difficulty + 1);
	data.CloseElement("ratings");
	data.OpenElement("flags");
	data.PushElement("adjust-difficulty", boolstr(scenario.adjust_diff));
	data.PushElement("legacy", boolstr(scenario.is_legacy));
	data.PushElement("custom-graphics", boolstr(scenario.uses_custom_graphics));
	data.CloseElement("flags");
	data.OpenElement("creator");
	data.PushElement("type", "oboe");
	data.PushElement("version", scenario.format_ed_version());
	data.PushElement("os", get_os_version());
	data.CloseElement("creator");
	data.OpenElement("game");
	data.PushElement("num-towns", scenario.towns.size());
	data.PushElement("out-width", scenario.outdoors.width());
	data.PushElement("out-height", scenario.outdoors.height());
	if(scenario.init_spec >= 0)
		data.PushElement("on-init", scenario.init_spec);
	data.PushElement("start-town", scenario.which_town_start);
	data.PushElement("town-start", scenario.where_start);
	data.PushElement("outdoor-start", scenario.out_sec_start);
	data.PushElement("sector-start", scenario.out_start);
	for(int i = 0; i < 3; i++) {
		if(scenario.store_item_towns[i] >= 0) {
			data.OpenElement("store-items");
			data.PushAttribute("top", scenario.store_item_rects[i].top);
			data.PushAttribute("left", scenario.store_item_rects[i].left);
			data.PushAttribute("bottom", scenario.store_item_rects[i].bottom);
			data.PushAttribute("right", scenario.store_item_rects[i].right);
			data.PushAttribute("town", scenario.store_item_towns[i]);
			data.CloseElement("store-items");
		}
	}
	for(int i = 0; i < scenario.town_mods.size(); i++) {
		if(scenario.town_mods[i].spec >= 0) {
			data.OpenElement("town-flag");
			data.PushAttribute("town", scenario.town_mods[i].spec);
			data.PushAttribute("add-x", scenario.town_mods[i].x);
			data.PushAttribute("add-y", scenario.town_mods[i].y);
			data.CloseElement("town-flag");
		}
	}
	for(int i = 0; i < scenario.special_items.size(); i++) {
		data.OpenElement("special-item");
		data.PushAttribute("start-with", boolstr(scenario.special_items[i].flags / 10));
		data.PushAttribute("useable", boolstr(scenario.special_items[i].flags % 10));
		data.PushAttribute("special", scenario.special_items[i].special);
		data.PushElement("name", scenario.special_items[i].name);
		data.PushElement("description", scenario.special_items[i].descr, true);
		data.CloseElement("special-item");
	}
	for(size_t i = 0; i < scenario.quests.size(); i++) {
		cQuest& quest = scenario.quests[i];
		data.OpenElement("quest");
		data.PushAttribute("start-with", boolstr(quest.flags / 10));
		if(quest.deadline >= 0) {
			data.OpenElement("deadline");
			data.PushAttribute("relative", boolstr(quest.flags % 10));
			if(quest.event >= 0)
				data.PushAttribute("waive-if", quest.event);
			data.PushText(quest.deadline);
			data.CloseElement("deadline");
		}
		if(quest.xp > 0 || quest.gold > 0) {
			data.OpenElement("reward");
			if(quest.xp > 0)
				data.PushAttribute("xp", quest.xp);
			if(quest.gold > 0)
				data.PushAttribute("gold", quest.gold);
			data.CloseElement("reward");
		}
		if(quest.bank1 >= 0)
			data.PushElement("bank", quest.bank1);
		if(quest.bank2 >= 0)
			data.PushElement("bank", quest.bank2);
		data.PushElement("name", quest.name);
		data.PushElement("description", quest.descr, true);
		data.CloseElement("quest");
	}
	for(size_t i = 0; i < scenario.shops.size(); i++) {
		cShop& shop = scenario.shops[i];
		data.OpenElement("shop");
		data.PushElement("name", shop.getName());
		data.PushElement("type", shop.getType());
		data.PushElement("prompt", shop.getPrompt());
		data.PushElement("face", shop.getFace());
		size_t num_entries = shop.size();
		data.OpenElement("entries");
		for(size_t j = 0; j < num_entries; j++) {
			cShopItem entry = shop.getItem(j);
			int quantity = entry.quantity, chance = 100;
			switch(entry.type) {
				case eShopItemType::EMPTY: break;
				case eShopItemType::OPT_ITEM:
					quantity %= 1000;
					chance = entry.quantity / 1000;
				case eShopItemType::ITEM:
					data.OpenElement("item");
					if(quantity == 0)
						data.PushAttribute("quantity", "infinite");
					else data.PushAttribute("quantity", quantity);
					if(chance < 100)
						data.PushAttribute("chance", chance);
					data.PushText(entry.index);
					data.CloseElement("item");
					break;
				case eShopItemType::CALL_SPECIAL:
					data.OpenElement("special");
					data.PushElement("name", entry.item.full_name);
					data.PushElement("description", entry.item.desc, true);
					data.PushElement("node", entry.item.item_level);
					if(entry.quantity == 0)
						data.PushElement("quantity", "infinite");
					else data.PushElement("quantity", entry.quantity);
					data.PushElement("cost", entry.item.value);
					data.PushElement("icon", entry.item.graphic_num);
					data.CloseElement("special");
					break;
				case eShopItemType::MAGE_SPELL:
					data.PushElement("mage-spell", entry.index);
					break;
				case eShopItemType::PRIEST_SPELL:
					data.PushElement("priest-spell", entry.index);
					break;
				case eShopItemType::ALCHEMY:
					data.PushElement("recipe", entry.index);
					break;
				case eShopItemType::SKILL:
					data.PushElement("skill", entry.index);
					break;
				case eShopItemType::TREASURE:
					data.PushElement("treasure", entry.index);
					break;
				case eShopItemType::CLASS:
					data.PushElement("class", entry.index);
					break;
				case eShopItemType::CURE_ACID: case eShopItemType::CURE_DISEASE: case eShopItemType::CURE_DUMBFOUNDING:
				case eShopItemType::CURE_PARALYSIS: case eShopItemType::CURE_POISON: case eShopItemType::DESTONE:
				case eShopItemType::HEAL_WOUNDS: case eShopItemType::RAISE_DEAD: case eShopItemType::REMOVE_CURSE:
				case eShopItemType::RESURRECT:
					data.PushElement("heal", int(entry.type) - int(eShopItemType::HEAL_WOUNDS));
					break;
			}
		}
		data.CloseElement("entries");
		data.CloseElement("shop");
	}
	for(int i = 0; i < scenario.scenario_timers.size(); i++) {
		if(scenario.scenario_timers[i].time > 0 && scenario.scenario_timers[i].node >= 0) {
			data.OpenElement("timer");
			data.PushAttribute("freq", scenario.scenario_timers[i].time);
			data.PushText(scenario.scenario_timers[i].node);
			data.CloseElement("timer");
		}
	}
	for(size_t i = 0; i < scenario.spec_strs.size(); i++) {
		if(scenario.spec_strs[i].empty()) continue;
		data.OpenElement("string");
		data.PushAttribute("id", i);
		data.PushText(scenario.spec_strs[i], true);
		data.CloseElement("string");
	}
	// Preserve trailing empty strings
	if(!scenario.spec_strs.empty() && scenario.spec_strs.back().empty()) {
		data.OpenElement("string");
		data.PushAttribute("id", scenario.spec_strs.size() - 1);
		data.CloseElement("string");
	}
	for(size_t i = 0; i < scenario.journal_strs.size(); i++) {
		if(scenario.journal_strs[i].empty()) continue;
		data.OpenElement("journal");
		data.PushAttribute("id", i);
		data.PushText(scenario.journal_strs[i], true);
		data.CloseElement("journal");
	}
	// Preserve trailing empty journals
	if(!scenario.journal_strs.empty() && scenario.journal_strs.back().empty()) {
		data.OpenElement("journal");
		data.PushAttribute("id", scenario.journal_strs.size() - 1);
		data.CloseElement("journal");
	}
	data.CloseElement("game");
	data.OpenElement("editor");
	data.PushElement("default-ground", scenario.default_ground);
	data.PushElement("last-out-section", cur_out);
	data.PushElement("last-town", cur_town);
	if(!scenario.custom_graphics.empty()) {
		data.OpenElement("graphics");
		for(size_t i = 0; i < scenario.custom_graphics.size(); i++) {
			if(scenario.custom_graphics[i] == PIC_FULL || scenario.custom_graphics[i] == 0) continue;
			data.OpenElement("pic");
			data.PushAttribute("index", i);
			data.PushText(scenario.custom_graphics[i]);
			data.CloseElement("pic");
		}
		data.CloseElement("graphics");
	}
	for(int i = 0; i < 10; i++) {
		if(scenario.storage_shortcuts[i].ter_type >= 0) {
			cScenario::cItemStorage shortcut = scenario.storage_shortcuts[i];
			data.OpenElement("storage");
			data.PushElement("on-terrain", shortcut.ter_type);
			data.PushElement("is-property", boolstr(shortcut.property));
			for(int j = 0; j < 10; j++) {
				if(shortcut.item_num[j] >= 0) {
					data.OpenElement("item");
					data.PushAttribute("chance", shortcut.item_odds[j]);
					data.PushText(shortcut.item_num[j]);
					data.CloseElement("item");
				}
			}
			data.CloseElement("storage");
		}
	}
	for(int i = 0; i < scenario.snd_names.size(); i++) {
		if(scenario.snd_names[i].empty()) continue;
		data.OpenElement("sound");
		data.PushAttribute("id", i + 100);
		data.PushText(scenario.snd_names[i]);
		data.CloseElement("sound");
	}
	data.CloseElement("editor");
	data.CloseElement("scenario");
}

void writeTerrainToXml(ticpp::Printer&& data, cScenario& scenario) {
	data.OpenElement("terrains");
	data.PushAttribute("boes", scenario.format_ed_version());
	for(size_t i = 0; i < scenario.ter_types.size(); i++) {
		data.OpenElement("terrain");
		data.PushAttribute("id", i);
		cTerrain& ter = scenario.ter_types[i];
		data.PushElement("name", ter.name);
		data.PushElement("pic", ter.picture);
		data.PushElement("map", ter.map_pic);
		data.PushElement("blockage", ter.blockage);
		data.PushElement("transform", ter.trans_to_what);
		data.PushElement("fly", ter.fly_over);
		data.PushElement("boat", ter.boat_over);
		data.PushElement("ride", !ter.block_horse);
		data.PushElement("archetype", ter.is_archetype);
		data.PushElement("light", ter.light_radius);
		data.PushElement("step-sound", ter.step_sound);
		data.PushElement("trim", ter.trim_type);
		data.PushElement("ground", ter.ground_type);
		data.PushElement("trim-for", ter.trim_ter);
		data.PushElement("arena", ter.combat_arena);
		
		data.OpenElement("special");
		data.PushElement("type", ter.special);
		data.PushElement("flag", ter.flag1);
		data.PushElement("flag", ter.flag2);
		data.PushElement("flag", ter.flag3);
		data.CloseElement("special");
		
		data.OpenElement("editor");
		if(ter.shortcut_key > 0 && ter.shortcut_key < '\x7f')
			data.PushElement("shortcut", ter.shortcut_key);
		if(ter.frill_for >= 0) {
			data.OpenElement("frill");
			if(ter.frill_chance != 10)
				data.PushAttribute("chance", ter.frill_chance);
			data.PushText(ter.frill_for);
			data.CloseElement("frill");
		}
		if(ter.obj_num > 0) {
			data.OpenElement("object");
			data.PushElement("num", ter.obj_num);
			data.PushElement("pos", ter.obj_pos);
			data.PushElement("size", ter.obj_size);
			data.CloseElement("object");
		}
		data.CloseElement("editor");
		data.CloseElement("terrain");
	}
	data.CloseElement("terrains");
}

void writeItemsToXml(ticpp::Printer&& data, cScenario& scenario) {
	data.OpenElement("items");
	data.PushAttribute("boes", scenario.format_ed_version());
	for(size_t i = 0; i < scenario.scen_items.size(); i++) {
		data.OpenElement("item");
		data.PushAttribute("id", i);
		cItem& item = scenario.scen_items[i];
		data.PushElement("variety", item.variety);
		data.PushElement("level", item.item_level);
		data.PushElement("awkward", item.awkward);
		data.PushElement("bonus", item.bonus);
		data.PushElement("protection", item.protection);
		if(item.charges > 0)
			data.PushElement("charges", item.charges);
		if((*item.variety).is_weapon && item.variety != eItemType::ARROW && item.variety != eItemType::BOLTS)
			data.PushElement("weapon-type", item.weap_type);
		if(item.missile > 0 || (*item.variety).is_missile)
			data.PushElement("missile-type", item.missile);
		data.PushElement("pic", item.graphic_num);
		if(item.type_flag > 0)
			data.PushElement("flag", item.type_flag);
		data.PushElement("name", item.name);
		data.PushElement("full-name", item.full_name);
		data.PushElement("treasure", item.treas_class);
		data.PushElement("value", item.value);
		data.PushElement("weight", item.weight);
		if(item.special_class > 0)
			data.PushElement("class", item.special_class);
		
		data.OpenElement("properties");
		if(item.ident) data.PushElement("identified", "true");
		if(item.magic) data.PushElement("magic", "true");
		if(item.cursed) data.PushElement("cursed", "true");
		if(item.concealed) data.PushElement("concealed", "true");
		if(item.enchanted) data.PushElement("enchanted", "true");
		if(item.unsellable) data.PushElement("unsellable", "true");
		data.CloseElement("properties");
		
		if(item.ability != eItemAbil::NONE) {
			data.OpenElement("ability");
			data.PushElement("type", item.ability);
			data.PushElement("strength", item.abil_data[0]);
			data.PushElement("data", item.abil_data[1]);
			data.PushElement("use-flag", item.magic_use_type);
			data.CloseElement("ability");
		}
		if(!item.desc.empty()) data.PushElement("description", item.desc, true);
		data.CloseElement("item");
	}
	data.CloseElement("items");
}

void writeMonstersToXml(ticpp::Printer&& data, cScenario& scenario) {
	std::ostringstream str;
	data.OpenElement("monsters");
	data.PushAttribute("boes", scenario.format_ed_version());
	for(size_t i = 1; i < scenario.scen_monsters.size(); i++) {
		data.OpenElement("monster");
		data.PushAttribute("id", i);
		cMonster& monst = scenario.scen_monsters[i];
		data.PushElement("name", monst.m_name);
		if(monst.default_facial_pic > 0)
			data.PushElement("default-face", monst.default_facial_pic);
		
		data.OpenElement("pic");
		data.PushAttribute("w", monst.x_width);
		data.PushAttribute("h", monst.y_width);
		data.PushText(monst.picture_num);
		data.CloseElement("pic");
		
		data.PushElement("race", monst.m_type);
		data.PushElement("level", monst.level);
		data.PushElement("armor", monst.armor);
		data.PushElement("skill", monst.skill);
		data.PushElement("hp", monst.m_health);
		data.PushElement("speed", monst.speed);
		data.PushElement("attitude", monst.default_attitude);
		data.PushElement("summon", monst.summon_type);
		data.PushElement("treasure", monst.treasure);
		if(monst.mu > 0)
			data.PushElement("mage", monst.mu);
		if(monst.cl > 0)
			data.PushElement("priest", monst.cl);
		if(monst.ambient_sound > 0)
			data.PushElement("voice", monst.ambient_sound);
		if(monst.see_spec >= 0)
			data.PushElement("onsight", monst.see_spec);
		
		data.OpenElement("attacks");
		data.PushElement("attack", monst.a[0]);
		if(monst.a[1].dice > 0 || monst.a[2].dice > 0)
			data.PushElement("attack", monst.a[1]);
		if(monst.a[2].dice > 0)
			data.PushElement("attack", monst.a[2]);
		data.CloseElement("attacks");
		
		data.OpenElement("immunity");
		for(int i = 0; i < 8; i++) {
			eDamageType dmg = eDamageType(i);
			if(monst.resist[dmg] != 100)
				data.PushElement(boost::lexical_cast<std::string>(dmg), monst.resist[dmg]);
		}
		if(monst.mindless) data.PushElement("fear", true);
		if(monst.amorphous) data.PushElement("assassinate", true);
		if(monst.invuln) data.PushElement("all", true);
		data.CloseElement("immunity");
		
		if(monst.corpse_item_chance > 0) {
			data.OpenElement("loot");
			data.PushElement("type", monst.corpse_item);
			data.PushElement("chance", monst.corpse_item_chance);
			data.CloseElement("loot");
		}
		
		if(monst.invisible || monst.guard || !monst.abil.empty()) {
			data.OpenElement("abilities");
			if(monst.invisible) data.PushElement("invisible");
			if(monst.guard) data.PushElement("guard");
			for(auto& p : monst.abil) {
				if(p.first == eMonstAbil::NO_ABIL || !p.second.active) continue;
				str.str("");
				eMonstAbil abil = p.first;
				uAbility& param = p.second;
				switch(getMonstAbilCategory(abil)) {
					case eMonstAbilCat::GENERAL:
						data.OpenElement("general");
						data.PushAttribute("type", abil);
						data.PushElement("type", param.gen.type);
						if(param.gen.type != eMonstGen::TOUCH) {
							data.PushElement("missile", param.gen.pic);
							data.PushElement("range", param.gen.range);
						}
						data.PushElement("strength", param.gen.strength);
						str << std::fixed << std::setprecision(1) << float(param.gen.odds)/10;
						data.PushElement("chance", str.str());
						if(abil == eMonstAbil::DAMAGE || abil == eMonstAbil::DAMAGE2)
							data.PushElement("extra", param.gen.dmg);
						else if(abil == eMonstAbil::STATUS || abil == eMonstAbil::STATUS2 || abil == eMonstAbil::STUN)
							data.PushElement("extra", param.gen.stat);
						else if(abil == eMonstAbil::FIELD)
							data.PushElement("extra", param.gen.fld);
						data.CloseElement("general");
						break;
					case eMonstAbilCat::MISSILE:
						data.OpenElement("missile");
						data.PushAttribute("type", abil);
						data.PushElement("type", param.missile.type);
						data.PushElement("missile", param.missile.pic);
						data.OpenElement("strength");
						data.PushText(std::to_string(param.missile.dice) + 'd' + std::to_string(param.missile.sides));
						data.CloseElement("strength");
						data.PushElement("skill", param.missile.skill);
						data.PushElement("range", param.missile.range);
						str << std::fixed << std::setprecision(1) << float(param.missile.odds)/10;
						data.PushElement("chance", str.str());
						data.CloseElement("missile");
						break;
					case eMonstAbilCat::SUMMON:
						data.OpenElement("summon");
						data.PushAttribute("type", abil);
						if(param.summon.type == eMonstSummon::SPECIES)
							data.PushElement("race", eRace(param.summon.what));
						else data.PushElement(boost::lexical_cast<std::string>(param.summon.type), param.summon.what);
						data.PushElement("min", param.summon.min);
						data.PushElement("max", param.summon.max);
						data.PushElement("duration", param.summon.len);
						str << std::fixed << std::setprecision(1) << float(param.summon.chance)/10;
						data.PushElement("chance", str.str());
						data.CloseElement("summon");
						break;
					case eMonstAbilCat::RADIATE:
						data.OpenElement("radiate");
						data.PushAttribute("type", abil);
						data.PushElement("type", param.radiate.type);
						data.PushElement("pattern", param.radiate.pat);
						data.PushElement("chance", param.radiate.chance);
						data.CloseElement("radiate");
						break;
					case eMonstAbilCat::SPECIAL:
						data.OpenElement("special");
						data.PushAttribute("type", abil);
						data.PushElement("param", param.special.extra1);
						if(abil != eMonstAbil::SPLITS && abil != eMonstAbil::DEATH_TRIGGER) {
							data.PushElement("param", param.special.extra2);
							if(abil == eMonstAbil::RAY_HEAT)
								data.PushElement("param", param.special.extra3);
						}
						data.CloseElement("special");
						break;
					case eMonstAbilCat::INVALID: break;
				}
			}
			data.CloseElement("abilities");
		}
		data.CloseElement("monster");
	}
	data.CloseElement("monsters");
}

void writeOutdoorsToXml(ticpp::Printer&& data, cOutdoors& sector) {
	data.OpenElement("sector");
	data.PushAttribute("boes", scenario.format_ed_version());
	data.PushElement("name", sector.name);
	if(!sector.comment.empty())
		data.PushElement("comment", sector.comment);
	switch(sector.ambient_sound) {
		case AMBIENT_NONE: break;
		case AMBIENT_BIRD: data.PushElement("sound", "birds"); break;
		case AMBIENT_DRIP: data.PushElement("sound", "drip"); break;
		case AMBIENT_CUSTOM: data.PushElement("sound", sector.out_sound); break;
	}
	for(auto& enc : sector.special_enc) {
		if(!enc.isNull())
			data.PushElement("encounter", enc);
	}
	for(auto& enc : sector.wandering) {
		if(!enc.isNull())
			data.PushElement("wandering", enc);
	}
	for(size_t i = 0; i < sector.sign_locs.size(); i++) {
		if(sector.sign_locs[i].text.empty()) continue;
		data.OpenElement("sign");
		data.PushAttribute("id", i);
		data.PushText(sector.sign_locs[i].text, true);
		data.CloseElement("sign");
	}
	// Preserve trailing empty signs
	if(!sector.sign_locs.empty() && sector.sign_locs.back().text.empty()) {
		data.OpenElement("sign");
		data.PushAttribute("id", sector.sign_locs.size() - 1);
		data.CloseElement("sign");
	}
	for(auto& area : sector.area_desc) {
		if(!area.descr.empty() && area.top < area.bottom && area.left < area.right)
			data.PushElement("area", area);
	}
	for(size_t i = 0; i < sector.spec_strs.size(); i++) {
		if(sector.spec_strs[i].empty()) continue;
		data.OpenElement("string");
		data.PushAttribute("id", i);
		data.PushText(sector.spec_strs[i], true);
		data.CloseElement("string");
	}
	// Preserve trailing empty strings
	if(!sector.spec_strs.empty() && sector.spec_strs.back().empty()) {
		data.OpenElement("string");
		data.PushAttribute("id", sector.spec_strs.size() - 1);
		data.CloseElement("string");
	}
	data.CloseElement("sector");
}

void writeTownToXml(ticpp::Printer&& data, cTown& town) {
	static const char directions[] = {'n', 'w', 's', 'e'};
	data.OpenElement("town");
	data.PushAttribute("boes", scenario.format_ed_version());
	data.PushElement("size", town.max_dim);
	data.PushElement("name", town.name);
	for(size_t i = 0; i < town.comment.size(); i++) {
		if(!town.comment[i].empty())
			data.PushElement("comment", town.comment[i]);
	}
	data.PushElement("bounds", town.in_town_rect);
	data.PushElement("difficulty", town.difficulty);
	data.PushElement("lighting", town.lighting_type);
	if(town.spec_on_entry >= 0) {
		data.OpenElement("onenter");
		data.PushAttribute("condition", "alive");
		data.PushText(town.spec_on_entry);
		data.CloseElement("onenter");
	}
	if(town.spec_on_entry_if_dead >= 0) {
		data.OpenElement("onenter");
		data.PushAttribute("condition", "dead");
		data.PushText(town.spec_on_entry_if_dead);
		data.CloseElement("onenter");
	}
	for(int i = 0; i < town.exits.size(); i++) {
		if(town.exits[i].x >= 0 && town.exits[i].y >= 0) {
			data.OpenElement("exit");
			data.PushAttribute("dir", directions[i]);
			data.PushAttribute("x", town.exits[i].x);
			data.PushAttribute("y", town.exits[i].y);
			data.CloseElement("exit");
		}
	}
	for(int i = 0; i < 4; i++) {
		if(town.exits[i].spec >= 0) {
			data.OpenElement("onexit");
			data.PushAttribute("dir", directions[i]);
			data.PushText(town.exits[i].spec);
			data.CloseElement("onexit");
		}
	}
	if(town.spec_on_hostile >= 0)
		data.PushElement("onoffend", town.spec_on_hostile);
	for(size_t i = 0; i < town.timers.size(); i++) {
		if(town.timers[i].time < 0 || town.timers[i].node < 0) continue;
		data.OpenElement("timer");
		data.PushAttribute("freq", town.timers[i].time);
		data.PushText(town.timers[i].node);
		data.CloseElement("timer");
	}
	data.OpenElement("flags");
	if(town.town_chop_time > 0) {
		data.OpenElement("chop");
		data.PushAttribute("day", town.town_chop_time);
		data.PushAttribute("event", town.town_chop_key);
		data.PushAttribute("kills", town.max_num_monst);
		data.CloseElement("chop");
	}
	if(town.strong_barriers)
		data.PushElement("strong-barriers", true);
	if(town.defy_mapping)
		data.PushElement("defy-mapping", true);
	if(town.defy_scrying)
		data.PushElement("defy-scrying", true);
	if(town.is_hidden)
		data.PushElement("hidden", true);
	if(town.has_tavern)
		data.PushElement("tavern", true);
	data.CloseElement("flags");
	for(int i = 0; i < town.wandering.size(); i++) {
		if(town.wandering[i].isNull()) continue;
		data.OpenElement("wandering");
		for(int j = 0; j < 4; j++) {
			if(j == 0 || town.wandering[i].monst[j] > 0)
				data.PushElement("monster", town.wandering[i].monst[j]);
		}
		data.CloseElement("wandering");
	}
	for(size_t i = 0; i < town.preset_items.size(); i++) {
		data.OpenElement("item");
		data.PushAttribute("id", i);
		data.PushElement("type", town.preset_items[i].code);
		if(town.preset_items[i].charges > 0)
			data.PushElement("charges", town.preset_items[i].charges);
		if(town.preset_items[i].ability >= 0)
			data.PushElement("mod", town.preset_items[i].ability);
		if(town.preset_items[i].always_there)
			data.PushElement("always", true);
		if(town.preset_items[i].property)
			data.PushElement("property", true);
		if(town.preset_items[i].contained)
			data.PushElement("contained", true);
		data.CloseElement("item");
	}
	for(size_t i = 0; i < town.creatures.size(); i++) {
		data.OpenElement("creature");
		data.PushAttribute("id", i);
		cTownperson& preset = town.creatures[i];
		data.PushElement("type", preset.number);
		data.PushElement("attitude", preset.start_attitude);
		data.PushElement("mobility", preset.mobility);
		data.PushElement("personality", preset.personality);
		data.PushElement("face", preset.facial_pic);
		if(preset.spec1 >= 0 && preset.spec2 >= 0)
			data.PushElement("sdf", loc(preset.spec1, preset.spec2));
		if(preset.spec_enc_code > 0)
			data.PushElement("encounter", preset.spec_enc_code);
		if(preset.special_on_kill >= 0)
			data.PushElement("onkill", preset.special_on_kill);
		if(preset.special_on_talk >= 0)
			data.PushElement("ontalk", preset.special_on_talk);
		if(preset.time_flag != eMonstTime::ALWAYS) {
			data.OpenElement("time");
			data.PushAttribute("type", preset.time_flag);
			if(preset.time_flag == eMonstTime::APPEAR_ON_DAY || preset.time_flag == eMonstTime::DISAPPEAR_ON_DAY || preset.time_flag == eMonstTime::APPEAR_WHEN_EVENT || preset.time_flag == eMonstTime::DISAPPEAR_WHEN_EVENT)
				data.PushElement("day", preset.monster_time);
			if(preset.time_flag == eMonstTime::APPEAR_WHEN_EVENT || preset.time_flag == eMonstTime::DISAPPEAR_WHEN_EVENT)
				data.PushElement("event", preset.time_code);
			data.CloseElement("time");
		}
		data.CloseElement("creature");
	}
	for(auto& area : town.area_desc) {
		if(!area.descr.empty() && area.top < area.bottom && area.left < area.right)
			data.PushElement("area", area);
	}
	for(size_t i = 0; i < town.sign_locs.size(); i++) {
		if(town.sign_locs[i].text.empty()) continue;
		data.OpenElement("sign");
		data.PushAttribute("id", i);
		data.PushText(town.sign_locs[i].text, true);
		data.CloseElement("sign");
	}
	// Preserve trailing empty signs
	if(!town.sign_locs.empty() && town.sign_locs.back().text.empty()) {
		data.OpenElement("sign");
		data.PushAttribute("id", town.sign_locs.size() - 1);
		data.CloseElement("sign");
	}
	for(size_t i = 0; i < town.spec_strs.size(); i++) {
		if(town.spec_strs[i].empty()) continue;
		data.OpenElement("string");
		data.PushAttribute("id", i);
		data.PushText(town.spec_strs[i], true);
		data.CloseElement("string");
	}
	// Preserve trailing empty strings
	if(!town.spec_strs.empty() && town.spec_strs.back().empty()) {
		data.OpenElement("string");
		data.PushAttribute("id", town.spec_strs.size() - 1);
		data.CloseElement("string");
	}
	data.CloseElement("town");
}

void writeDialogueToXml(ticpp::Printer&& data, cSpeech& talk, int town_num) {
	data.OpenElement("dialogue");
	data.PushAttribute("boes", scenario.format_ed_version());
	for(size_t i = 0; i < 10; i++) {
		cPersonality& who = talk.people[i];
		data.OpenElement("personality");
		data.PushAttribute("id", i + 10 * town_num);
		data.PushElement("title", who.title);
		data.PushElement("look", who.look, !who.look.empty());
		data.PushElement("name", who.name, !who.look.empty());
		data.PushElement("job", who.job, !who.look.empty());
		if(!who.dunno.empty())
			data.PushElement("unknown", who.dunno, true);
		data.CloseElement("personality");
	}
	for(size_t i = 0; i < talk.talk_nodes.size(); i++) {
		cSpeech::cNode& node = talk.talk_nodes[i];
		// Don't drop unused nodes if they still contain text
		if(node.personality == -1 && node.str1.empty() && node.str2.empty()) continue;
		// TODO: Is it safe to assume the two links run together like this?
		if(std::string(node.link1, 8) == "        " && node.str1.empty() && node.str2.empty())
			continue;
		data.OpenElement("node");
		data.PushAttribute("for", node.personality);
		if(std::string(node.link1, 4) != "    ")
			data.PushElement("keyword", std::string(node.link1, 4));
		if(std::string(node.link2, 4) != "    ")
			data.PushElement("keyword", std::string(node.link2, 4));
		data.PushElement("type", node.type);
		if(node.extras[0] >= 0 || node.extras[1] >= 0 || node.extras[2] >= 0 || node.extras[3] >= 0)
			data.PushElement("param", node.extras[0]);
		if(node.extras[1] >= 0 || node.extras[2] >= 0 || node.extras[3] >= 0)
			data.PushElement("param", node.extras[1]);
		if(node.extras[2] >= 0 || node.extras[3] >= 0)
			data.PushElement("param", node.extras[2]);
		if(node.extras[3] >= 0)
			data.PushElement("param", node.extras[3]);
		if(!node.str1.empty())
			data.PushElement("text", node.str1, true);
		else data.PushElement("text");
		if(!node.str2.empty())
			data.PushElement("text", node.str2, true);
		data.CloseElement("node");
	}
	data.CloseElement("dialogue");
}

map_data buildOutMapData(location which, cScenario& scenario) {
	cOutdoors& sector = *scenario.outdoors[which.x][which.y];
	map_data terrain;
	for(size_t x = 0; x < 48; x++) {
		for(size_t y = 0; y < 48; y++) {
			terrain.set(x, y, sector.terrain[x][y]);
			if(sector.special_spot[x][y])
				terrain.addFeature(x, y, eMapFeature::FIELD, SPECIAL_SPOT);
			if(sector.roads[x][y])
				terrain.addFeature(x, y, eMapFeature::FIELD, SPECIAL_ROAD);
		}
	}
	for(size_t i = 0; i < sector.special_locs.size(); i++) {
		if(sector.special_locs[i].spec >= 0)
			terrain.addFeature(sector.special_locs[i].x, sector.special_locs[i].y, eMapFeature::SPECIAL_NODE, sector.special_locs[i].spec);
	}
	for(size_t i = 0; i < sector.city_locs.size(); i++) {
		if(sector.city_locs[i].spec >= 0)
			terrain.addFeature(sector.city_locs[i].x, sector.city_locs[i].y, eMapFeature::TOWN, sector.city_locs[i].spec);
	}
	for(size_t i = 0; i < sector.sign_locs.size(); i++) {
		if(!sector.sign_locs[i].text.empty())
			terrain.addFeature(sector.sign_locs[i].x, sector.sign_locs[i].y, eMapFeature::SIGN, i);
	}
	for(size_t i = 0; i < sector.wandering_locs.size(); i++) {
		terrain.addFeature(sector.wandering_locs[i].x, sector.wandering_locs[i].y, eMapFeature::WANDERING, i);
	}
	for(size_t i = 0; i < scenario.boats.size(); i++) {
		if(scenario.boats[i].which_town == 200 && scenario.boats[i].sector == which) {
			int j = i + 1;
			if(scenario.boats[i].property) j *= -1;
			terrain.addFeature(scenario.boats[i].loc.x, scenario.boats[i].loc.y, eMapFeature::BOAT, j);
		}
	}
	for(size_t i = 0; i < scenario.horses.size(); i++) {
		if(scenario.horses[i].which_town == 200 && scenario.horses[i].sector == which) {
			int j = i + 1;
			if(scenario.horses[i].property) j *= -1;
			terrain.addFeature(scenario.horses[i].loc.x, scenario.horses[i].loc.y, eMapFeature::HORSE, j);
		}
	}
	return terrain;
}

map_data buildTownMapData(size_t which, cScenario& scenario) {
	cTown& town = *scenario.towns[which];
	map_data terrain;
	for(size_t x = 0; x < town.max_dim; x++) {
		for(size_t y = 0; y < town.max_dim; y++) {
			terrain.set(x, y, town.terrain(x,y));
		}
	}
	for(size_t i = 0; i < town.special_locs.size(); i++) {
		if(town.special_locs[i].spec >= 0)
			terrain.addFeature(town.special_locs[i].x, town.special_locs[i].y, eMapFeature::SPECIAL_NODE, town.special_locs[i].spec);
	}
	for(size_t i = 0; i < town.sign_locs.size(); i++) {
		if(!town.sign_locs[i].text.empty())
			terrain.addFeature(town.sign_locs[i].x, town.sign_locs[i].y, eMapFeature::SIGN, i);
	}
	for(size_t i = 0; i < town.wandering_locs.size(); i++) {
		terrain.addFeature(town.wandering_locs[i].x, town.wandering_locs[i].y, eMapFeature::WANDERING, i);
	}
	for(size_t i = 0; i < town.preset_items.size(); i++) {
		if(town.preset_items[i].code >= 0)
			terrain.addFeature(town.preset_items[i].loc.x, town.preset_items[i].loc.y, eMapFeature::ITEM, i);
	}
	for(size_t i = 0; i < town.preset_fields.size(); i++) {
		if(town.preset_fields[i].type > 0 && town.preset_fields[i].type < FIELD_DISPEL)
			terrain.addFeature(town.preset_fields[i].loc.x,town.preset_fields[i].loc.y,eMapFeature::FIELD,town.preset_fields[i].type);
	}
	for(size_t i = 0; i < town.creatures.size(); i++) {
		if(town.creatures[i].number > 0)
			terrain.addFeature(town.creatures[i].start_loc.x, town.creatures[i].start_loc.y, eMapFeature::CREATURE, i);
	}
	for(size_t i = 0; i < scenario.boats.size(); i++) {
		if(scenario.boats[i].which_town == which) {
			int j = i + 1;
			if(scenario.boats[i].property) j *= -1;
			terrain.addFeature(scenario.boats[i].loc.x, scenario.boats[i].loc.y, eMapFeature::BOAT, j);
		}
	}
	for(size_t i = 0; i < scenario.horses.size(); i++) {
		if(scenario.horses[i].which_town == which) {
			int j = i + 1;
			if(scenario.horses[i].property) j *= -1;
			terrain.addFeature(scenario.horses[i].loc.x, scenario.horses[i].loc.y, eMapFeature::HORSE, j);
		}
	}
	terrain.addFeature(town.start_locs[0].x, town.start_locs[0].y, eMapFeature::ENTRANCE_SOUTH);
	terrain.addFeature(town.start_locs[1].x, town.start_locs[1].y, eMapFeature::ENTRANCE_WEST);
	terrain.addFeature(town.start_locs[2].x, town.start_locs[2].y, eMapFeature::ENTRANCE_NORTH);
	terrain.addFeature(town.start_locs[3].x, town.start_locs[3].y, eMapFeature::ENTRANCE_EAST);
	return terrain;
}

struct overrides_sheet {
	const std::string& fname;
	overrides_sheet(const std::string& fname) : fname(fname) {}
	bool operator()(const std::string check) const {
		return check == fname;
	}
};

extern std::string scenario_temp_dir_name;
extern fs::path scenDir;
void save_scenario(bool rename) {
	fs::path toFile = scenario.scen_file;
	if(rename || toFile.empty()) {
		fs::path def = scenario.scen_file;
		if(def.empty())
			def = scenDir/"myscenario.boes";
		toFile = nav_put_scenario(def);
		if(toFile.empty()) return;
	}
	
	extern cUndoList undo_list;
	undo_list.save();
	
	if(scenario.is_legacy && cChoiceDlog("save-legacy-scen", {"update", "cancel"}).show() == "update")
		scenario.is_legacy = false;
	
	scenario.reset_version();
	tarball scen_file;
	{
		// First, write out the scenario header data. This is in a binary format identical to older scenarios.
		std::ostream& header = scen_file.newFile("scenario/header.exs");
		header.write(reinterpret_cast<char*>(&scenario.format), sizeof(scenario_header_flags));
		
		// Next, the bulk scenario data.
		std::ostream& scen_data = scen_file.newFile("scenario/scenario.xml");
		writeScenarioToXml(ticpp::Printer("scenario.xml", scen_data), scenario);
		
		// Then the terrains...
		std::ostream& terrain = scen_file.newFile("scenario/terrain.xml");
		writeTerrainToXml(ticpp::Printer("terrain.xml", terrain), scenario);
		
		// ...items...
		std::ostream& items = scen_file.newFile("scenario/items.xml");
		writeItemsToXml(ticpp::Printer("items.xml", items), scenario);
		
		// ...and monsters
		std::ostream& monsters = scen_file.newFile("scenario/monsters.xml");
		writeMonstersToXml(ticpp::Printer("monsters.xml", monsters), scenario);
		
		// And the special nodes.
		std::ostream& scen_spec = scen_file.newFile("scenario/scenario.spec");
		writeSpecialNodes(scen_spec, scenario.scen_specials);
	}
	
	// Next, write the outdoors.
	for(size_t x = 0; x < scenario.outdoors.width(); x++) {
		for(size_t y = 0; y < scenario.outdoors.height(); y++) {
			std::string file_basename = "out" + std::to_string(x) + '~' + std::to_string(y);
			// First the main data.
			std::ostream& outdoors = scen_file.newFile("scenario/out/" + file_basename + ".xml");
			writeOutdoorsToXml(ticpp::Printer(file_basename + ".xml", outdoors), *scenario.outdoors[x][y]);
			
			// Then the map.
			std::ostream& out_map = scen_file.newFile("scenario/out/" + file_basename + ".map");
			buildOutMapData(loc(x,y), scenario).writeTo(out_map);
			
			// And the special nodes.
			std::ostream& out_spec = scen_file.newFile("scenario/out/" + file_basename + ".spec");
			writeSpecialNodes(out_spec, scenario.outdoors[x][y]->specials);
		}
	}
	
	// And finally, the towns.
	for(size_t i = 0; i < scenario.towns.size(); i++) {
		std::string file_basename = "town" + std::to_string(i);
		// First the main data.
		std::ostream& town = scen_file.newFile("scenario/towns/" + file_basename + ".xml");
		writeTownToXml(ticpp::Printer(file_basename + ".xml", town), *scenario.towns[i]);
		
		// Then the map.
		std::ostream& town_map = scen_file.newFile("scenario/towns/" + file_basename + ".map");
		buildTownMapData(i, scenario).writeTo(town_map);
		
		// And the special nodes.
		std::ostream& town_spec = scen_file.newFile("scenario/towns/" + file_basename + ".spec");
		writeSpecialNodes(town_spec, scenario.towns[i]->specials);
		
		// Don't forget the dialogue nodes.
		std::ostream& town_talk = scen_file.newFile("scenario/towns/talk" + std::to_string(i) + ".xml");
		writeDialogueToXml(ticpp::Printer("talk.xml", town_talk), scenario.towns[i]->talking, i);
	}

	change_made = false;
	
	// Alright. At this point, check to see if the scenario was unpacked.
	if(fs::is_directory(toFile)) {
		// If it was, we just need to save each file to its respective location
		// There's no need to worry about custom graphics or sounds, either.
		// And if it's unpacked, it can't possibly be legacy, so graphics don't need conversion.
		for(auto& file : scen_file) {
			std::string fname = file.filename.substr(9);
			std::ofstream fout((toFile/fname).string());
			fout << file.contents.rdbuf();
		}
		return;
	}
	
	// Now, custom graphics.
	if(spec_scen_g.is_old) {
		spec_scen_g.convert_sheets();
		for(size_t i = 0; i < spec_scen_g.numSheets; i++) {
			sf::Image sheet = spec_scen_g.sheets[i].copyToImage();
			fs::path tempPath = tempDir/"temp.png";
			sheet.saveToFile(tempPath.string());
			std::ostream& pic_out = scen_file.newFile("scenario/graphics/sheet" + std::to_string(i) + ".png");
			std::ifstream fin(tempPath.string().c_str(), std::ios::binary);
			pic_out << fin.rdbuf();
			fin.close();
		}
	} else {
		fs::path picPath = tempDir/scenario_temp_dir_name/"graphics";
		if(fs::exists(picPath) && fs::is_directory(picPath)) {
			// First build a list of overridable sheets
			std::set<std::string> sheet_names;
			fs::directory_iterator sheet_iter(progDir/"data"/"graphics");
			for(; sheet_iter != fs::directory_iterator(); sheet_iter++) {
				std::string fname = sheet_iter->path().filename().string();
				size_t dot = fname.find_last_of('.');
				if(dot == std::string::npos) continue;
				if(fname.substr(dot) == ".png")
					sheet_names.insert(fname);
			}
			fs::directory_iterator dir_iter(picPath);
			for(; dir_iter != fs::directory_iterator(); dir_iter++) {
				std::string fname = dir_iter->path().filename().string();
				bool is_sheet = false;
				if(fname.substr(0,5) == "sheet") {
					size_t dot = fname.find_last_of('.');
					if(dot == std::string::npos) continue;
					if(fname.substr(dot) == ".png" && std::all_of(fname.begin() + 5, fname.begin() + dot, isdigit)) {
						// Looks like a valid sheet!
						is_sheet = true;
					}
				} else if(std::any_of(sheet_names.begin(), sheet_names.end(), overrides_sheet(fname))) {
					// Override sheets replace default graphic sets - they should be included as well
					is_sheet = true;
				}
				if(is_sheet) {
					std::ostream& pic_out = scen_file.newFile("scenario/graphics/" + fname);
					std::ifstream fin(dir_iter->path().string().c_str(), std::ios::binary);
					pic_out << fin.rdbuf();
					fin.close();
				}
			}
		}
	}
	
	// And also sounds!
	fs::path sndPath = tempDir/scenario_temp_dir_name/"sounds";
	if(fs::exists(sndPath) && fs::is_directory(sndPath)) {
		fs::directory_iterator dir_iter(sndPath);
		while(dir_iter != fs::directory_iterator()) {
			std::string fname = dir_iter->path().filename().string();
			if(fname.substr(0,3) == "SND") {
				size_t dot = fname.find_last_of('.');
				if(fname.substr(dot) == ".wav" && std::all_of(fname.begin() + 3, fname.begin() + dot, isdigit)) {
					// Looks like a valid sound!
					std::ostream& pic_out = scen_file.newFile("scenario/sounds/" + fname);
					std::ifstream fin(dir_iter->path().string().c_str(), std::ios::binary);
					pic_out << fin.rdbuf();
					fin.close();
				}
			}
			dir_iter++;
		}
	}
	
	// Make sure it has the proper file extension
	std::string fname = toFile.filename().string();
	size_t dot = fname.find_last_of('.');
	std::string ext;
	if(dot != std::string::npos) {
		ext = fname.substr(dot);
		std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
	}
	if(ext != ".boes") {
		if(ext == ".exs")
			fname.replace(dot,4,".boes");
		else fname += ".boes";
	}
	toFile = toFile.parent_path()/fname;
	scenario.scen_file = toFile;
	
	// Now write to zip file.
	ogzstream zout(toFile.string().c_str());
	scen_file.writeTo(zout);
	zout.close();
}

void start_data_dump() {
	using std::endl;
	std::string scen_name = scenario.scen_name;
	std::ofstream fout((scenario.scen_file.parent_path()/(scen_name + " Data.txt")).string().c_str());
	fout << "Scenario data for " << scen_name << ':' << endl << endl;
	fout << "Terrain types for " << scen_name << ':' << endl;
	for(short i = 0; i < scenario.ter_types.size(); i++)
		fout << "  Terrain type " << i << ": " << scenario.ter_types[i].name << endl;
	fout << endl << "Monster types for " << scen_name << ':' << endl;
	for(short i = 0; i < scenario.scen_monsters.size(); i++)
		fout << "  Monster type " << i << ": " << scenario.scen_monsters[i].m_name << endl;
	fout << endl << "Item types for " << scen_name << ':' << endl;
	for(short i = 0; i < scenario.scen_items.size(); i++)
		fout << "  Item type " << i << ": " << scenario.scen_items[i].full_name << endl;
	fout.close();
}

void scen_text_dump(){
	using std::endl;
	std::ofstream fout((scenario.scen_file.parent_path()/(scenario.scen_name + " Text.txt")).string().c_str());
	fout << "Scenario text for " << scenario.scen_name << ':' << endl << endl;
	fout << "Scenario Text:" << endl;
	fout << "Who Wrote 1: " << scenario.who_wrote[0] << endl;
	fout << "Who Wrote 2: " << scenario.who_wrote[1] << endl;
	fout << "Contact Info: " << scenario.contact_info[0] << "    " << scenario.contact_info[1] << endl;
	for(short i = 0; i < scenario.intro_strs.size(); i++)
		if(scenario.intro_strs[i][0] != '*')
			fout << "  Intro Message " << i << ": " << scenario.intro_strs[i] << endl;
	for(short i = 0; i < scenario.journal_strs.size(); i++)
		if(scenario.journal_strs[i][0] != '*')
			fout << "  Journal Entry " << i << ": " << scenario.journal_strs[i] << endl;
	for(short i = 0; i < scenario.special_items.size(); i++)
		if(scenario.special_items[i].name[0] != '*') {
			fout << "  Special Item " << i << ':' << endl;
			fout << "    Name: " << scenario.special_items[i].name << endl;
			fout << "    Description: " << scenario.special_items[i].descr << endl;
		}
	for(short i = 0; i < scenario.quests.size(); i++) {
		fout << "  Quest " << i << ':' << endl;
		fout << "    Name: " << scenario.quests[i].name << endl;
		fout << "    Description: " << scenario.quests[i].descr << endl;
	}
	for(short i = 0; i < scenario.shops.size(); i++ ) {
		fout << "  Shop " << i << ':' << endl;
		fout << "    Name: " << scenario.shops[i].getName() << endl;
		for(short j = 0; j < scenario.shops[i].size(); j++) {
			cShopItem it = scenario.shops[i].getItem(j);
			if(it.type == eShopItemType::CALL_SPECIAL) {
				fout << "    Entry " << j << ':' << endl;
				fout << "      Name: " << it.item.full_name << endl;
				fout << "      Description: " << it.item.desc << endl;
			}
		}
	}
	for(short i = 0; i < scenario.spec_strs.size(); i++)
		if(scenario.spec_strs[i][0] != '*')
			fout << "  Message " << i << ": " << scenario.spec_strs[i] << endl;
	for(short i = 0; i < scenario.scen_items.size(); i++) {
		if(!scenario.scen_items[i].desc.empty())
			fout << "  Item description " << i << ": " << scenario.scen_items[i].desc << endl;
	}
	fout << endl << "Outdoor Sections Text:" << endl << endl;
	for(short x = 0; x < scenario.outdoors.width(); x++) {
		for(short y = 0; y < scenario.outdoors.height(); y++) {
			fout << "  Section (x = " << x << ", y = " << y << "):" << endl;
			fout << "    Name: " << scenario.outdoors[x][y]->name << endl;
			fout << "    Comment: " << scenario.outdoors[x][y]->comment << endl;
			for(short i = 0; i < scenario.outdoors[x][y]->area_desc.size(); i++)
				if(scenario.outdoors[x][y]->area_desc[i].descr[0] != '*')
					fout << "    Area Rectangle " << i << ": " << scenario.outdoors[x][y]->area_desc[i].descr << endl;
			for(short i = 0; i < scenario.outdoors[x][y]->spec_strs.size(); i++)
				if(scenario.outdoors[x][y]->spec_strs[i][0] != '*')
					fout << "    Message " << i << ": " << scenario.outdoors[x][y]->spec_strs[i] << endl;
			for(short i = 0; i < scenario.outdoors[x][y]->sign_locs.size(); i++)
				if(scenario.outdoors[x][y]->sign_locs[i].text[0] != '*')
					fout << "    Sign " << i << ": " << scenario.outdoors[x][y]->sign_locs[i].text << endl;
			fout << endl;
		}
	}
	fout << "Town Text:" << endl << endl;
	for(short j = 0; j < scenario.towns.size(); j++) {
		fout << "  Town " << j << ':' << endl;
		fout << "    Name: " << scenario.towns[j]->name << endl;
		for(short i = 0; i < 3; i++)
			if(scenario.towns[j]->comment[i][0] != '*')
				fout << "    Comment: " << scenario.towns[j]->comment[i] << endl;
		fout << "  Town Messages:" << endl;
		for(short i = 0; i < scenario.towns[j]->area_desc.size(); i++)
			if(scenario.towns[j]->area_desc[i].descr[0] != '*')
				fout << "    Area Rectangle " << i << ": " << scenario.towns[j]->area_desc[i].descr << endl;
		for(short i = 0; i < scenario.towns[j]->spec_strs.size(); i++)
			if(scenario.towns[j]->spec_strs[i][0] != '*')
				fout << "    Message " << i << ": " << scenario.towns[j]->spec_strs[i] << endl;
		for(short i = 0; i < scenario.towns[j]->sign_locs.size(); i++)
			if(scenario.towns[j]->sign_locs[i].text[0] != '*')
				fout << "    Sign " << i << ": " << scenario.towns[j]->sign_locs[i].text << endl;
		fout << endl << "  Town Dialogue:" << endl;
		for(short i = 0; i < 10; i++) {
			fout << "    Personality " << j + i << " (" << scenario.towns[j]->talking.people[i].title << "): " << endl;
			fout << "      look: " << scenario.towns[j]->talking.people[i].look << endl;
			fout << "      name: " << scenario.towns[j]->talking.people[i].name << endl;
			fout << "      job: " << scenario.towns[j]->talking.people[i].job << endl;
			fout << "      confused: " << scenario.towns[j]->talking.people[i].dunno << endl;
		}
		for(short i = 0; i < scenario.towns[j]->talking.talk_nodes.size(); i++) {
			if(scenario.towns[j]->talking.talk_nodes[i].str1.length() > 0)
				fout << "    Node " << i << "a: " << scenario.towns[j]->talking.talk_nodes[i].str1 << endl;
			if(scenario.towns[j]->talking.talk_nodes[i].str2.length() > 0)
				fout << "    Node " << i << "b: " << scenario.towns[j]->talking.talk_nodes[i].str2 << endl;
		}
		fout << endl;
	}
	fout.close();
}

