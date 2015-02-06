
#include <cstring>
#include "scen.global.h"
#include "scenario.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <boost/filesystem/operations.hpp>
#include "scen.fileio.h"
#include "scen.keydlgs.h"
#include "graphtool.hpp"
#include "scen.core.h"
#include "soundtool.hpp"
#include "mathutil.hpp"
#include "oldstructs.h"
#include "fileio.hpp"
#include "dlogutil.hpp"
#include "tarball.hpp"
#include "gzstream.h"
#include "tinyprint.hpp"
#include "map_parse.hpp"

#define	DONE_BUTTON_ITEM	1

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

template<> void ticpp::Printer::PushElement(std::string tagName, location pos) {
	OpenElement(tagName);
	PushAttribute("x", pos.x);
	PushAttribute("y", pos.y);
	CloseElement(tagName);
}

template<> void ticpp::Printer::PushElement(std::string tagName, rectangle rect) {
	OpenElement(tagName);
	PushAttribute("top", rect.top);
	PushAttribute("left", rect.left);
	PushAttribute("bottom", rect.bottom);
	PushAttribute("right", rect.right);
	CloseElement(tagName);
}

template<> void ticpp::Printer::PushElement(std::string tagName, cMonster::cAttack attack) {
	OpenElement(tagName);
	PushAttribute("type", attack.type);
	std::ostringstream strength;
	strength << attack.dice << 'd' << attack.sides;
	PushText(strength.str());
	CloseElement(tagName);
}

template<> void ticpp::Printer::PushElement(std::string tagName, cOutdoors::cWandering enc) {
	OpenElement(tagName);
	PushAttribute("can-flee", !enc.cant_flee);
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

template<> void ticpp::Printer::PushElement(std::string tagName, info_rect_t rect) {
	OpenElement(tagName);
	PushAttribute("top", rect.top);
	PushAttribute("left", rect.left);
	PushAttribute("bottom", rect.bottom);
	PushAttribute("right", rect.right);
	PushText(rect.descr);
	CloseElement(tagName);
}

static bool is_minmax(int lo, int hi, int val) {
	return minmax(lo, hi, val) == val;
}

static void writeScenarioToXml(ticpp::Printer&& data) {
	data.OpenElement("scenario");
	data.PushAttribute("boes", scenario.format_ed_version());
	data.PushElement("title", scenario.scen_name);
	data.PushElement("icon", scenario.intro_pic);
	data.PushElement("id", scenario.campaign_id);
	data.PushElement("version", scenario.format_scen_version());
	data.PushElement("language", "en-US");
	data.OpenElement("author");
	data.PushElement("name", scenario.contact_info);
	// TODO: Store name and email in separate fields.
	data.PushElement("email", "");
	data.CloseElement("author");
	data.OpenElement("text");
	data.PushElement("teaser", scenario.who_wrote[0]);
	data.PushElement("teaser", scenario.who_wrote[1]);
	if(scenario.intro_pic != scenario.intro_mess_pic)
		data.PushElement("icon", scenario.intro_mess_pic);
	for(int i = 0; i < 6; i++)
		data.PushElement("intro-msg", scenario.intro_strs[i]);
	data.CloseElement("text");
	data.OpenElement("ratings");
	switch(scenario.rating) {
		case 0: data.PushElement("content", "g"); break;
		case 1: data.PushElement("content", "pg"); break;
		case 2: data.PushElement("content", "r"); break;
		case 3: data.PushElement("content", "nc17"); break;
	}
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
	// TODO: fill <os> element
	data.PushElement("os", "");
	data.CloseElement("creator");
	data.OpenElement("game");
	data.PushElement("num-towns", scenario.towns.size());
	data.PushElement("out-width", scenario.outdoors.width());
	data.PushElement("out-height", scenario.outdoors.height());
	data.PushElement("start-town", scenario.which_town_start);
	data.PushElement("town-start", scenario.where_start);
	data.PushElement("outdoor-start", scenario.out_sec_start);
	data.PushElement("sector-start", scenario.out_start);
	for(int i = 0; i < 3; i++) {
		if(is_minmax(0, scenario.towns.size(), scenario.store_item_towns[i])) {
			data.OpenElement("store-items");
			data.PushAttribute("top", scenario.store_item_rects[i].top);
			data.PushAttribute("left", scenario.store_item_rects[i].left);
			data.PushAttribute("bottom", scenario.store_item_rects[i].bottom);
			data.PushAttribute("right", scenario.store_item_rects[i].right);
			data.PushAttribute("town", scenario.store_item_towns[i]);
			data.CloseElement("store-items");
		}
	}
	for(int i = 0; i < 10; i++) {
		if(is_minmax(0, scenario.towns.size(), scenario.town_to_add_to[i])) {
			data.OpenElement("town-flag");
			data.PushAttribute("town", scenario.town_to_add_to[i]);
			data.PushAttribute("add-x", scenario.flag_to_add_to_town[i][0]);
			data.PushAttribute("add-y", scenario.flag_to_add_to_town[i][1]);
			data.CloseElement("town-flag");
		}
	}
	data.OpenElement("specials");
	for(int i = 0; i < 50; i++) {
		data.OpenElement("item");
		data.PushAttribute("start-with", boolstr(scenario.special_items[i].flags / 10));
		data.PushAttribute("useable", boolstr(scenario.special_items[i].flags % 10));
		data.PushAttribute("special", scenario.special_items[i].special);
		data.PushElement("name", scenario.special_items[i].name);
		data.PushElement("description", scenario.special_items[i].descr);
		data.CloseElement("item");
	}
	data.CloseElement("specials");
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
		data.PushElement("description", quest.descr);
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
				case eShopItemType::OPTIONAL:
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
					data.PushElement("description", entry.item.desc);
					data.PushElement("node", entry.item.item_level);
					data.PushElement("quantity", entry.quantity);
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
	for(int i = 0; i < 20; i++) {
		if(scenario.scenario_timer_times[i] > 0) {
			data.OpenElement("timer");
			data.PushAttribute("time", scenario.scenario_timer_times[i]);
			data.PushText(scenario.scenario_timer_specs[i]);
			data.CloseElement("timer");
		}
	}
	for(size_t i = 0; i < scenario.spec_strs.size(); i++)
		data.PushElement("string", scenario.spec_strs[i]);
	for(size_t i = 0; i < scenario.journal_strs.size(); i++)
		data.PushElement("journal", scenario.journal_strs[i]);
	data.CloseElement("game");
	data.OpenElement("editor");
	data.PushElement("default-ground", scenario.default_ground);
	data.PushElement("last-out-section", scenario.last_out_edited);
	data.PushElement("last-town", scenario.last_town_edited);
	if(!scenario.custom_graphics.empty()) {
		data.OpenElement("graphics");
		for(size_t i = 0; i < scenario.custom_graphics.size(); i++) {
			if(scenario.custom_graphics[i] == PIC_FULL) continue;
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
	data.CloseElement("editor");
	data.CloseElement("scenario");
}

static void writeTerrainToXml(ticpp::Printer&& data) {
	data.OpenElement("terrains");
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
		data.PushElement("light", ter.light_radius);
		data.PushElement("step-sound", ter.step_sound);
		data.PushElement("trim", ter.trim_type);
		data.PushElement("arena", ter.combat_arena);
		
		data.OpenElement("special");
		data.PushElement("type", ter.special);
		data.PushElement("flag", ter.flag1);
		data.PushElement("flag", ter.flag2);
		data.PushElement("flag", ter.flag3);
		data.CloseElement("special");
		
		data.OpenElement("editor");
		if(ter.shortcut_key > 0)
			data.PushElement("shortcut", ter.shortcut_key);
		data.PushElement("ground", ter.ground_type);
		data.PushElement("trim-for", ter.trim_ter);
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

static void writeItemsToXml(ticpp::Printer&& data) {
	data.OpenElement("items");
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
		if(isWeaponType(item.variety) && item.variety != eItemType::ARROW && item.variety != eItemType::BOLTS)
			data.PushElement("weapon-type", item.weap_type);
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
		if(!item.desc.empty()) data.PushElement("description", item.desc);
		data.CloseElement("item");
	}
	data.CloseElement("items");
}

static void writeMonstersToXml(ticpp::Printer&& data) {
	std::ostringstream str;
	data.OpenElement("monsters");
	for(size_t i = 1; i < scenario.scen_monsters.size(); i++) {
		data.OpenElement("monster");
		data.PushAttribute("id", i);
		cMonster& monst = scenario.scen_monsters[i];
		data.PushElement("name", monst.m_name);
		if(monst.default_facial_pic >= 0)
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
		if(monst.magic_res != 100)
			data.PushElement("magic", monst.magic_res);
		if(monst.fire_res != 100)
			data.PushElement("fire", monst.magic_res);
		if(monst.cold_res != 100)
			data.PushElement("cold", monst.magic_res);
		if(monst.poison_res != 100)
			data.PushElement("poison", monst.magic_res);
		if(monst.mindless) data.PushElement("fear", true);
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
						data.PushElement("type", param.summon.type);
						data.PushElement("min", param.summon.min);
						data.PushElement("max", param.summon.max);
						data.PushElement("duration", param.summon.len);
						data.PushElement("chance", param.summon.chance);
						data.CloseElement("summon");
						break;
					case eMonstAbilCat::RADIATE:
						data.OpenElement("radiate");
						data.PushAttribute("type", abil);
						data.PushElement("type", param.radiate.type);
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

static void writeOutdoorsToXml(ticpp::Printer&& data, cOutdoors& sector) {
	data.OpenElement("sector");
	data.PushAttribute("boes", scenario.format_ed_version());
	data.PushElement("name", sector.out_name);
	if(!sector.comment.empty())
		data.PushElement("comment", sector.comment);
	switch(sector.ambient_sound) {
		case AMBIENT_NONE: break;
		case AMBIENT_BIRD: data.PushElement("sound", "birds"); break;
		case AMBIENT_DRIP: data.PushElement("sound", "drip"); break;
		case AMBIENT_CUSTOM: data.PushElement("sound", sector.out_sound); break;
	}
	for(auto& enc : sector.special_enc)
		data.PushElement("encounter", enc);
	for(auto& enc : sector.wandering)
		data.PushElement("wandering", enc);
	data.OpenElement("signs");
	for(auto& sign : sector.sign_locs)
		data.PushElement("string", sign.text);
	data.CloseElement("signs");
	data.OpenElement("descriptions");
	for(auto& area : sector.info_rect) {
		if(!area.descr.empty() && area.top < area.bottom && area.left < area.right)
			data.PushElement("area", area);
	}
	data.CloseElement("descriptions");
	data.OpenElement("strings");
	for(auto& str : sector.spec_strs)
		data.PushElement("string", str);
	data.CloseElement("strings");
	data.CloseElement("sector");
}

static void writeTownToXml(ticpp::Printer&& data, cTown& town) {
	static const char directions[] = {'n', 'w', 's', 'e'};
	data.OpenElement("town");
	data.PushAttribute("boes", scenario.format_ed_version());
	data.PushElement("size", town.max_dim());
	data.PushElement("name", town.town_name);
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
	for(int i = 0; i < 4; i++) {
		if(town.exit_locs[i].x >= 0 && town.exit_locs[i].y >= 0) {
			data.OpenElement("exit");
			data.PushAttribute("dir", directions[i]);
			data.PushAttribute("x", town.exit_locs[i].x);
			data.PushAttribute("y", town.exit_locs[i].y);
			data.CloseElement("exit");
		}
	}
	for(int i = 0; i < 4; i++) {
		if(town.exit_specs[i] >= 0) {
			data.OpenElement("onexit");
			data.PushAttribute("dir", directions[i]);
			data.PushText(town.exit_specs[i]);
			data.CloseElement("onexit");
		}
	}
	if(town.spec_on_hostile >= 0)
		data.PushElement("onoffend", town.spec_on_hostile);
	for(size_t i = 0; i < town.timer_spec_times.size() && i < town.timer_specs.size(); i++) {
		data.OpenElement("timer");
		data.PushAttribute("freq", town.timer_spec_times[i]);
		data.PushText(town.timer_specs[i]);
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
	data.CloseElement("flags");
	for(int i = 0; i < 4; i++) {
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
		if(preset.time_flag != eMonstTime::ALWAYS) {
			data.OpenElement("time");
			data.PushAttribute("type", preset.time_flag);
			data.PushElement("param", preset.monster_time);
			data.PushElement("param", preset.time_code);
			data.CloseElement("time");
		}
		data.CloseElement("creature");
	}
	for(auto& area : town.room_rect) {
		if(!area.descr.empty() && area.top < area.bottom && area.left < area.right)
			data.PushElement("description", area);
	}
	for(size_t i = 0; i < town.sign_locs.size(); i++) {
		if(town.sign_locs[i].text.empty()) continue;
		data.OpenElement("sign");
		data.PushAttribute("id", i);
		data.PushText(town.sign_locs[i].text);
		data.CloseElement("sign");
	}
	for(size_t i = 0; i < town.spec_strs.size(); i++) {
		if(town.spec_strs[i].empty()) continue;
		data.OpenElement("string");
		data.PushAttribute("id", i);
		data.PushText(town.spec_strs[i]);
		data.CloseElement("string");
	}
	data.CloseElement("town");
}

static void writeDialogueToXml(ticpp::Printer&& data, cSpeech& talk) {
	data.OpenElement("dialogue");
	data.PushAttribute("boes", scenario.format_ed_version());
	for(size_t i = 0; i < 10; i++) {
		cPersonality& who = talk.people[i];
		data.OpenElement("personality");
		data.PushAttribute("id", i);
		data.PushElement("title", who.title);
		data.PushElement("look", who.look);
		data.PushElement("name", who.name);
		data.PushElement("job", who.job);
		data.CloseElement("personality");
	}
	for(size_t i = 0; i < 60; i++) {
		cSpeech::cNode& node = talk.talk_nodes[i];
		if(node.personality == -1) continue;
		// TODO: Is it safe to assume the two links run together like this?
		if(std::string(node.link1, 8) == "xxxxxxxx")
			continue;
		data.OpenElement("node");
		data.PushAttribute("for", node.personality);
		if(std::string(node.link1, 4) != "xxxx")
			data.PushElement("keyword", std::string(node.link1, 4));
		if(std::string(node.link2, 4) != "xxxx")
			data.PushElement("keyword", std::string(node.link2, 4));
		data.PushElement("type", int(node.type));
		if(node.extras[0] >= 0 || node.extras[1] >= 0 || node.extras[2] >= 0 || node.extras[3] >= 0)
			data.PushElement("param", node.extras[0]);
		if(node.extras[1] >= 0 || node.extras[2] >= 0 || node.extras[3] >= 0)
			data.PushElement("param", node.extras[1]);
		if(node.extras[2] >= 0 || node.extras[3] >= 0)
			data.PushElement("param", node.extras[2]);
		if(node.extras[3] >= 0)
			data.PushElement("param", node.extras[3]);
		if(!node.str1.empty())
			data.PushElement("text", node.str1);
		else data.PushElement("text");
		if(!node.str2.empty())
			data.PushElement("text", node.str2);
		data.CloseElement("node");
	}
	data.CloseElement("dialogue");
}

static map_data buildOutMapData(location which) {
	cOutdoors& sector = *scenario.outdoors[which.x][which.y];
	map_data terrain;
	for(size_t x = 0; x < 48; x++) {
		for(size_t y = 0; y < 48; y++) {
			terrain.set(x, y, sector.terrain[x][y]);
			if(sector.special_spot[x][y])
				terrain.addFeature(x, y, eMapFeature::FIELD, SPECIAL_SPOT);
		}
	}
	for(size_t i = 0; i < sector.special_locs.size(); i++) {
		if(sector.special_locs[i].spec >= 0)
			terrain.addFeature(sector.special_locs[i].x, sector.special_locs[i].y, eMapFeature::SPECIAL_NODE, sector.special_locs[i].spec);
	}
	for(size_t i = 0; i < sector.exit_locs.size(); i++) {
		if(sector.exit_locs[i].spec >= 0)
			terrain.addFeature(sector.exit_locs[i].x, sector.exit_locs[i].y, eMapFeature::TOWN, sector.exit_locs[i].spec);
	}
	for(size_t i = 0; i < sector.sign_locs.size(); i++) {
		if(!sector.sign_locs[i].text.empty())
			terrain.addFeature(sector.sign_locs[i].x, sector.sign_locs[i].y, eMapFeature::SIGN, i);
	}
	for(size_t i = 0; i < 4; i++) {
		terrain.addFeature(sector.wandering_locs[i].x, sector.wandering_locs[i].y, eMapFeature::WANDERING, i);
	}
	for(size_t i = 0; i < scenario.boats.size(); i++) {
		if(scenario.boats[i].which_town == 200 && scenario.boats[i].sector == which) {
			int j = i;
			if(scenario.boats[i].property) j *= -1;
			terrain.addFeature(scenario.boats[i].loc.x, scenario.boats[i].loc.y, eMapFeature::HORSE, j);
		}
	}
	for(size_t i = 0; i < scenario.horses.size(); i++) {
		if(scenario.horses[i].which_town == 200 && scenario.horses[i].sector == which) {
			int j = i;
			if(scenario.horses[i].property) j *= -1;
			terrain.addFeature(scenario.horses[i].loc.x, scenario.horses[i].loc.y, eMapFeature::HORSE, j);
		}
	}
	return terrain;
}

static map_data buildTownMapData(size_t which) {
	cTown& town = *scenario.towns[which];
	map_data terrain;
	for(size_t x = 0; x < town.max_dim(); x++) {
		for(size_t y = 0; y < town.max_dim(); y++) {
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
	for(size_t i = 0; i < 4; i++) {
		terrain.addFeature(town.wandering_locs[i].x, town.wandering_locs[i].y, eMapFeature::WANDERING, i);
	}
	for(size_t i = 0; i < town.preset_items.size(); i++) {
		if(town.preset_items[i].code >= 0)
			terrain.addFeature(town.preset_items[i].loc.x, town.preset_items[i].loc.y, eMapFeature::ITEM, i);
	}
	for(size_t i = 0; i < town.preset_fields.size(); i++) {
		if(town.preset_fields[i].type > 0)
			terrain.addFeature(town.preset_fields[i].loc.x,town.preset_fields[i].loc.y,eMapFeature::FIELD,town.preset_fields[i].type);
	}
	for(size_t i = 0; i < town.creatures.size(); i++) {
		if(town.creatures[i].number > 0)
			terrain.addFeature(town.creatures[i].start_loc.x, town.creatures[i].start_loc.y, eMapFeature::CREATURE, i);
	}
	for(size_t i = 0; i < scenario.boats.size(); i++) {
		if(scenario.boats[i].which_town == which) {
			int j = i;
			if(scenario.boats[i].property) j *= -1;
			terrain.addFeature(scenario.boats[i].loc.x, scenario.boats[i].loc.y, eMapFeature::HORSE, j);
		}
	}
	for(size_t i = 0; i < scenario.horses.size(); i++) {
		if(scenario.horses[i].which_town == which) {
			int j = i;
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

void save_scenario(fs::path toFile) {
	scenario.format.prog_make_ver[0] = 2;
	scenario.format.prog_make_ver[1] = 0;
	scenario.format.prog_make_ver[2] = 0;
	scenario.format.flag1 = 'O'; scenario.format.flag2 = 'B';
	scenario.format.flag3 = 'O'; scenario.format.flag4 = 'E';
	// TODO: This is just a skeletal outline of what needs to be done to save the scenario
	tarball scen_file;
	{
		// First, write out the scenario header data. This is in a binary format identical to older scenarios.
		std::ostream& header = scen_file.newFile("scenario/header.exs");
		header.write(reinterpret_cast<char*>(&scenario.format), sizeof(scenario_header_flags));
		
		// Next, the bulk scenario data.
		std::ostream& scen_data = scen_file.newFile("scenario/scenario.xml");
		writeScenarioToXml(ticpp::Printer("scenario.xml", scen_data));
		
		// Then the terrains...
		std::ostream& terrain = scen_file.newFile("scenario/terrain.xml");
		writeTerrainToXml(ticpp::Printer("terrain.xml", terrain));
		
		// ...items...
		std::ostream& items = scen_file.newFile("scenario/items.xml");
		writeItemsToXml(ticpp::Printer("items.xml", items));
		
		// ...and monsters
		std::ostream& monsters = scen_file.newFile("scenario/monsters.xml");
		writeMonstersToXml(ticpp::Printer("monsters.xml", monsters));
		
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
			buildOutMapData(loc(x,y)).writeTo(out_map);
			
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
		buildTownMapData(i).writeTo(town_map);
		
		// And the special nodes.
		std::ostream& town_spec = scen_file.newFile("scenario/towns/" + file_basename + ".spec");
		writeSpecialNodes(town_spec, scenario.towns[i]->specials);
		
		// Don't forget the dialogue nodes.
		std::ostream& town_talk = scen_file.newFile("scenario/towns/talk" + std::to_string(i) + ".xml");
		writeDialogueToXml(ticpp::Printer("talk.xml", town_talk), scenario.towns[i]->talking);
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
		fs::path picPath = tempDir/"scenario"/"graphics";
		if(fs::exists(picPath) && fs::is_directory(picPath)) {
			// First build a list of overridable sheets
			std::set<std::string> sheet_names;
			fs::directory_iterator sheet_iter(progDir/"graphics.exd"/"mac");
			while(sheet_iter != fs::directory_iterator()) {
				std::string fname = sheet_iter->path().filename().string();
				size_t dot = fname.find_last_of('.');
				if(fname.substr(dot) == ".png")
					sheet_names.insert(fname);
				sheet_iter++;
			}
			fs::directory_iterator dir_iter(picPath);
			while(dir_iter != fs::directory_iterator()) {
				std::string fname = dir_iter->path().filename().string();
				bool is_sheet = false;
				if(fname.substr(0,5) == "sheet") {
					size_t dot = fname.find_last_of('.');
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
				dir_iter++;
			}
		}
	}
	
	// And also sounds!
	fs::path sndPath = tempDir/"scenario"/"sounds";
	if(fs::exists(sndPath) && fs::is_directory(sndPath)) {
		fs::directory_iterator dir_iter(sndPath);
		while(dir_iter != fs::directory_iterator()) {
			std::string fname = dir_iter->path().filename().string();
			if(fname.substr(0,3) == "SND") {
				size_t dot = fname.find_last_of('.');
				if(fname.substr(dot) == ".WAV" && std::all_of(fname.begin() + 3, fname.begin() + dot, isdigit)) {
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
	if(dot == std::string::npos || fname.substr(dot) != ".boes") {
		if(fname.substr(dot) == ".exs")
			fname.replace(dot,4,".boes");
		else fname += ".boes";
	}
	toFile = toFile.parent_path()/fname;
	
	// Now write to zip file.
	ogzstream zout(toFile.string().c_str());
	scen_file.writeTo(zout);
	zout.close();
}

void start_data_dump() {
	short i;
	using std::endl;
	std::string scen_name = scenario.scen_name;
	std::ofstream fout("Scenario Data.txt");
	fout << "Scenario data for " << scen_name << ':' << endl << endl;
	fout << "Terrain types for " << scen_name << ':' << endl;
	for(i = 0; i < 256; i++)
		fout << "  Terrain type " << i << ": " << scenario.ter_types[i].name << endl;
	fout << endl << "Monster types for " << scen_name << ':' << endl;
	for(i = 0; i < 256; i++)
		fout << "  Monster type " << i << ": " << scenario.scen_monsters[i].m_name << endl;
	fout << endl << "Item types for " << scen_name << ':' << endl;
	for(i = 0; i < 400; i++)
		fout << "  Item type " << i << ": " << scenario.scen_items[i].full_name << endl;
	fout.close();
}

void scen_text_dump(){
	short i;
	using std::endl;
	location out_sec;
	std::ofstream fout("Scenario Text.txt");
	fout << "Scenario text for " << scenario.scen_name << ':' << endl << endl;
	fout << "Scenario Text:" << endl;
	fout << "Who Wrote 1: " << scenario.who_wrote[0] << endl;
	fout << "Who Wrote 2: " << scenario.who_wrote[1] << endl;
	fout << "Contact Info: " << scenario.contact_info << endl;
	for(i = 0; i < 6; i++)
		if(scenario.intro_strs[i][0] != '*')
			fout << "  Intro Message " << i << ": " << scenario.intro_strs[i] << endl;
	for(i = 0; i < 50; i++)
		if(scenario.journal_strs[i][0] != '*')
			fout << "  Journal Entry " << i << ": " << scenario.journal_strs[i] << endl;
	for(i = 0; i < 50; i++)
		if(scenario.special_items[i].name[0] != '*') {
			fout << "  Special Item " << i << ':' << endl;
			fout << "    Name: " << scenario.special_items[i].name << endl;
			fout << "    Description: " << scenario.special_items[i].descr << endl;
		}
	for(i = 0; i < 100; i++)
		if(scenario.spec_strs[i][0] != '*')
			fout << "  Message " << i << ": " << scenario.spec_strs[i] << endl;
	fout << endl << "Outdoor Sections Text:" << endl << endl;
	for(out_sec.x = 0; out_sec.x < scenario.outdoors.width(); out_sec.x++) {
		for(out_sec.y = 0; out_sec.y < scenario.outdoors.height(); out_sec.y++) {
			fout << "  Section (x = " << (short)out_sec.x << ", y = " << (short)out_sec.y << "):" << endl;
			fout << "    Name: " << scenario.outdoors[out_sec.x][out_sec.y]->out_name;
			fout << "    Comment: " << scenario.outdoors[out_sec.x][out_sec.y]->comment;
			for(i = 0; i < 8; i++)
				if(scenario.outdoors[out_sec.x][out_sec.y]->info_rect[i].descr[0] != '*')
					fout << "    Area Rectangle " << i << ": " << scenario.outdoors[out_sec.x][out_sec.y]->info_rect[i].descr << endl;
			for(i = 0; i < 90; i++)
				if(scenario.outdoors[out_sec.x][out_sec.y]->spec_strs[i][0] != '*')
					fout << "    Message " << i << ": " << scenario.outdoors[out_sec.x][out_sec.y]->spec_strs[i] << endl;
			for(i = 0; i < 8; i++)
				if(scenario.outdoors[out_sec.x][out_sec.y]->sign_locs[i].text[0] != '*')
					fout << "    Sign " << i << ": " << scenario.outdoors[out_sec.x][out_sec.y]->sign_locs[i].text << endl;
			fout << endl;
		}
	}
	fout << "Town Text:" << endl << endl;
	for(short j = 0; j < scenario.towns.size(); j++) {
		fout << "  Town " << j << ':' << endl;
		fout << "  Town Messages:" << endl;
		fout << "    Name: " << scenario.towns[i]->town_name << endl;
		for(i = 0; i < 16; i++)
			if(scenario.towns[i]->room_rect[i].descr[0] != '*')
				fout << "    Area Rectangle " << i << ": " << scenario.towns[i]->room_rect[i].descr << endl;
		fout << "    Name: " << scenario.towns[i]->town_name << endl;
		for(i = 0; i < 3; i++)
			if(scenario.towns[i]->comment[i][0] != '*')
				fout << "    Comment " << i << ": " << scenario.towns[i]->comment[i] << endl;
		fout << "    Name: " << scenario.towns[i]->town_name << endl;
		for(i = 0; i < 100; i++)
			if(scenario.towns[i]->spec_strs[i][0] != '*')
				fout << "    Message " << i << ": " << scenario.towns[i]->spec_strs[i] << endl;
		fout << "    Name: " << scenario.towns[i]->town_name << endl;
		for(i = 0; i < 20; i++)
			if(scenario.towns[i]->sign_locs[i].text[0] != '*')
				fout << "    Sign " << i << ": " << scenario.towns[i]->sign_locs[i].text << endl;
		fout << endl << "  Town Dialogue:" << endl;
		for(i = 0; i < 10; i++) {
			fout << "    Personality " << j + i << " (" << scenario.towns[i]->talking.people[i].title << "): " << endl;
			fout << "    look: " << scenario.towns[i]->talking.people[i].look << endl;
			fout << "    name: " << scenario.towns[i]->talking.people[i].name << endl;
			fout << "    job: " << scenario.towns[i]->talking.people[i].job << endl;
			fout << "    confused: " << scenario.towns[i]->talking.people[i].dunno << endl;
		}
		for(i = 0; i < 60; i++) {
			if(scenario.towns[i]->talking.talk_nodes[i].str1.length() > 0)
				fout << "    Node " << i << "a: " << scenario.towns[i]->talking.talk_nodes[i].str1 << endl;
			if(scenario.towns[i]->talking.talk_nodes[i].str2.length() > 0)
				fout << "    Node " << i << "b: " << scenario.towns[i]->talking.talk_nodes[i].str2 << endl;
		}
		fout << endl;
	}
	fout.close();
}

