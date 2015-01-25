
#include <cstring>
#include "scen.global.h"
#include "scenario.h"
#include <iostream>
#include <fstream>
#include <iomanip>
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
extern fs::path progDir;
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
	for(int i = 0; i < 20; i++) {
		if(scenario.scenario_timer_times[i] > 0) {
			data.OpenElement("timer");
			data.PushAttribute("time", scenario.scenario_timer_times[i]);
			data.PushText(scenario.scenario_timer_specs[i]);
			data.CloseElement("timer");
		}
	}
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
	data.OpenElement("strings");
	for(size_t i = 0; i < scenario.spec_strs.size(); i++)
		data.PushElement("string", scenario.spec_strs[i]);
	data.CloseElement("strings");
	data.OpenElement("journal");
	for(size_t i = 0; i < scenario.journal_strs.size(); i++)
		data.PushElement("string", scenario.journal_strs[i]);
	data.CloseElement("journal");
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
		data.PushElement("flag", ter.flag1.u);
		data.PushElement("flag", ter.flag2.u);
		data.PushElement("flag", ter.flag3.u);
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
	for(auto& sign : sector.sign_strs)
		data.PushElement("string", sign);
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
	for(size_t i = 0; i < town.sign_strs.size(); i++) {
		if(town.sign_strs[i].empty()) continue;
		data.OpenElement("sign");
		data.PushAttribute("id", i);
		data.PushText(town.sign_strs[i]);
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

map_data buildOutMapData(location which) {
	cOutdoors& sector = *scenario.outdoors[which.x][which.y];
	map_data terrain;
	for(size_t x = 0; x < 48; x++) {
		for(size_t y = 0; y < 48; y++) {
			terrain.set(x, y, sector.terrain[x][y]);
			if(sector.special_spot[x][y])
				terrain.addFeature(x, y, eMapFeature::FIELD, SPECIAL_SPOT);
		}
	}
	for(size_t i = 0; i < 18; i++) {
		if(sector.special_id[i] >= 0)
			terrain.addFeature(sector.special_locs[i].x, sector.special_locs[i].y, eMapFeature::SPECIAL_NODE, sector.special_id[i]);
	}
	for(size_t i = 0; i < 8; i++) {
		if(sector.exit_dests[i] >= 0)
			terrain.addFeature(sector.exit_locs[i].x, sector.exit_locs[i].y, eMapFeature::TOWN, sector.exit_dests[i]);
	}
	for(size_t i = 0; i < sector.sign_strs.size(); i++) {
		if(!sector.sign_strs[i].empty())
			terrain.addFeature(sector.sign_locs[i].x, sector.sign_locs[i].y, eMapFeature::SIGN, i);
	}
	for(size_t i = 0; i < 4; i++) {
		terrain.addFeature(sector.wandering_locs[i].x, sector.wandering_locs[i].y, eMapFeature::WANDERING, i);
	}
	for(size_t i = 0; i < 30; i++) {
		if(scenario.boats[i].which_town == 200 && scenario.boats[i].sector == which) {
			int j = i;
			if(scenario.boats[i].property) j *= -1;
			terrain.addFeature(scenario.boats[i].loc.x, scenario.boats[i].loc.y, eMapFeature::HORSE, j);
		}
		if(scenario.horses[i].which_town == 200 && scenario.horses[i].sector == which) {
			int j = i;
			if(scenario.horses[i].property) j *= -1;
			terrain.addFeature(scenario.horses[i].loc.x, scenario.horses[i].loc.y, eMapFeature::HORSE, j);
		}
	}
	return terrain;
}

map_data buildTownMapData(size_t which) {
	cTown& town = *scenario.towns[which];
	map_data terrain;
	for(size_t x = 0; x < town.max_dim(); x++) {
		for(size_t y = 0; y < town.max_dim(); y++) {
			terrain.set(x, y, town.terrain(x,y));
		}
	}
	for(size_t i = 0; i < 50; i++) {
		if(town.spec_id[i] >= 0)
			terrain.addFeature(town.special_locs[i].x, town.special_locs[i].y, eMapFeature::SPECIAL_NODE, town.spec_id[i]);
	}
	for(size_t i = 0; i < town.sign_strs.size(); i++) {
		if(!town.sign_strs[i].empty())
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
	for(size_t i = 0; i < 30; i++) {
		if(scenario.boats[i].which_town == which) {
			int j = i;
			if(scenario.boats[i].property) j *= -1;
			terrain.addFeature(scenario.boats[i].loc.x, scenario.boats[i].loc.y, eMapFeature::HORSE, j);
		}
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

void save_scenario(fs::path toFile) {
	// TODO: I'm not certain 1.0.0 is the correct version here?
	scenario.format.prog_make_ver[0] = 1;
	scenario.format.prog_make_ver[1] = 0;
	scenario.format.prog_make_ver[2] = 0;
	// TODO: This is just a skeletal outline of what needs to be done to save the scenario
	tarball scen_file;
	{
		// First, write out the scenario header data. This is in a binary format identical to older scenarios.
		std::ostream& header = scen_file.newFile("scenario/header.exs");
		
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
		std::string file_basename = 't' + std::to_string(i);
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
		std::ostream& town_talk = scen_file.newFile("scenario/towns/" + file_basename + "talk.xml");
	}
	giveError("Sorry, scenario saving is currently disabled.");
	return;
	
	// Make sure it has the proper file extension
	std::string fname = toFile.filename().string();
	size_t dot = fname.find_last_of('.');
	if(dot == std::string::npos || fname.substr(dot) != ".boes")
		fname += ".boes";
	toFile = toFile.parent_path()/fname;
	
	// Now write to zip file.
	ogzstream zout(toFile.string().c_str());
	scen_file.writeTo(zout);
	zout.close();
}

void create_basic_scenario() {
//	short i,j,k,num_outdoors;
//	FSSpec to_load,new_file,dummy_file;
//	NavReplyRecord reply;
//	short dummy_f;
//	char *buffer = NULL;
//	Size buf_len = 100000;
//	OSErr error;
//	short out_num;
//	long len;
//	char message[256] = "Select saved game:                                     ";
//	legacy::big_tr_type t_d;
//	legacy::ave_tr_type ave_t;
//	legacy::tiny_tr_type tiny_t;
//
//	//FSMakeFSSpec(start_volume,start_dir,"Blades of Exile Base",&new_file);
//
//	//FSpDelete(&new_file);
//	//error = FSpCreate(&new_file,'blx!','BETM',smSystemScript);
//	//OK. FIrst find out what file name we're working with, and make the dummy file
//	// which we'll build the new scenario in
//	//to_load = store_file_reply;
//	FSMakeFSSpec(start_volume,start_dir,"::::Blades of Exile Base",&dummy_file);
//	FSpDelete(&dummy_file);
//	error = FSpCreate(&dummy_file,'blx!','BETM',reply.keyScript);
//	if((error != 0) && (error != dupFNErr)) {
//				//add_string_to_buf("Save: Couldn't create file.         ");
//				SysBeep(2);
//				return;
//				}
//	if((error = FSpOpenDF(&dummy_file,3,&dummy_f)) != 0) {
//		SysBeep(2);
//		return;
//		}
//
//
//	// Now we need to set up a buffer for moving the data over to the dummy
//	buffer = (char *) NewPtr(buf_len);
//	if(buffer == NULL) {
//		SysBeep(2); FSClose(dummy_f);
//		return;
//		}
//
//	cur_town = 0;
//	scenario.num_towns = 1;
//	scenario.town_size[0] = 1;
//	scenario.out_width = 1;
//	scenario.out_height = 1;
//	cur_out.x = 0;
//	cur_out.y = 0;
//	scenario.last_out_edited = cur_out;
//	// We're finally set up. Let's first set up the new scenario field
//	// We need the new info for the current town and outdoors, which may have been changed
//	scenario.town_data_size[cur_town][0] = sizeof(legacy::town_record_type);
//	if(scenario.town_size[cur_town] == 0)
//		scenario.town_data_size[cur_town][0] += sizeof(legacy::big_tr_type);
//		else if(scenario.town_size[cur_town] == 1)
//			scenario.town_data_size[cur_town][0] += sizeof(legacy::ave_tr_type);
//			else scenario.town_data_size[cur_town][0] += sizeof(legacy::tiny_tr_type);
//	scenario.town_data_size[cur_town][1] = 0;
//	for(i = 0; i < 60; i++)
//		scenario.town_data_size[cur_town][1] += strlen(data_store->town_strs[i]);
//	scenario.town_data_size[cur_town][2] = 0;
//	for(i = 60; i < 140; i++)
//		scenario.town_data_size[cur_town][2] += strlen(data_store->town_strs[i]);
//	scenario.town_data_size[cur_town][3] = sizeof(legacy::talking_record_type);
//	for(i = 0; i < 80; i++)
//		scenario.town_data_size[cur_town][3] += strlen(data_store->talk_strs[i]);
//	scenario.town_data_size[cur_town][4] = 0;
//	for(i = 80; i < 170; i++)
//		scenario.town_data_size[cur_town][4] += strlen(data_store->talk_strs[i]);
//
//	out_num = cur_out.y * scenario.out_width + cur_out.x;
//	scenario.out_data_size[out_num][0] = sizeof(legacy::outdoor_record_type);
//	scenario.out_data_size[out_num][1] = 0;
//	for(i = 0; i < 120; i++)
//		scenario.out_data_size[out_num][1] += strlen(data_store->out_strs[i]);
//
//	for(i = 0; i < 300; i++)
//		scenario.scen_str_len[i] = 0;
//	for(i = 0; i < 270; i++)
//		scenario.scen_str_len[i] = strlen(data_store->scen_strs[i]);
//	scenario.last_town_edited = cur_town;
//	scenario.last_out_edited = cur_out;
//
//	// now write scenario data
//	scenario.format.flag1 = 10;
//	scenario.format.flag2 = 20;
//	scenario.format.flag3 = 30;
//	scenario.format.flag4 = 40; /// these mean made on mac
//	len = sizeof(legacy::scenario_data_type); // scenario data
//	if((error = FSWrite(dummy_f, &len, (char *) &scenario)) != 0) {
//		SysBeep(2); FSClose(dummy_f);
//		return;
//		}
//	len = sizeof(scen_item_data_type); // item data
//	if((error = FSWrite(dummy_f, &len, (char *) &(data_store->scen_item_list))) != 0) {
//		SysBeep(2); FSClose(dummy_f);
//		return;
//		}
//	for(i = 0; i < 270; i++) { // scenario strings
//		len = (long) scenario.scen_str_len[i];
//		if((error = FSWrite(dummy_f, &len, (char *) &(data_store->scen_strs[i]))) != 0) {
//			SysBeep(2); FSClose(dummy_f);
//			return;
//			}
//		}
//
//	// OK ... scenario written. Now outdoors.
//	num_outdoors = scenario.out_width * scenario.out_height;
//	for(i = 0; i < num_outdoors; i++)
//		if(i == out_num) {
//			// write outdoors
//			for(j = 0; j < 180; j++)
//				current_terrain.strlens[j] = 0;
//			for(j = 0; j < 120; j++)
//				current_terrain.strlens[j] = strlen(data_store->out_strs[j]);
//			len = sizeof(legacy::outdoor_record_type);
//			FSWrite(dummy_f, &len, (char *) &current_terrain);
//
//			for(j = 0; j < 120; j++) {
//				len = (long) current_terrain.strlens[j];
//				FSWrite(dummy_f, &len, (char *) &(data_store->out_strs[j]));
//				}
//			}
//
//	// now, finally, write towns.
//	for(k = 0; k < scenario.num_towns; k++)
//		if(k == cur_town) {
//			for(i = 0; i < 180; i++)
//				town->strlens[i] = 0;
//			for(i = 0; i < 140; i++)
//				town->strlens[i] = strlen(data_store->town_strs[i]);
//
//			// write towns
//			len = sizeof(legacy::town_record_type);
//			FSWrite(dummy_f, &len, (char *) &town);
//
//			switch(scenario.town_size[cur_town]) {
//				case 0:
//					len = sizeof(legacy::big_tr_type);
//					FSWrite(dummy_f, &len, (char *) &t_d);
//					break;
//
//				case 1:
//					for(i = 0; i < 48; i++)
//						for(j = 0; j < 48; j++) {
//							ave_t.terrain[i][j] = town->terrain(i,j);
//							ave_t.lighting[i / 8][j] = town->lighting(i / 8,j);
//							}
//					for(i = 0; i < 16; i++) {
//						ave_t.room_rect[i] = town->room_rect(i);
//						}
//					for(i = 0; i < 40; i++) {
//						//ave_t.creatures[i] = town->creatures(i);
//						}
//					len = sizeof(legacy::ave_tr_type);
//					FSWrite(dummy_f, &len, (char *) &ave_t);
//				break;
//
//
//				case 2:
//					for(i = 0; i < 32; i++)
//						for(j = 0; j < 32; j++) {
//							tiny_t.terrain[i][j] = town->terrain(i,j);
//							tiny_t.lighting[i / 8][j] = town->lighting(i / 8,j);
//							}
//					for(i = 0; i < 16; i++) {
//						tiny_t.room_rect[i] = town->room_rect(i);
//						}
//					for(i = 0; i < 30; i++) {
//						//tiny_t.creatures[i] = town->creatures(i);
//						}
//					len = sizeof(legacy::tiny_tr_type);
//					FSWrite(dummy_f, &len, (char *) &tiny_t);
//					break;
//				}
//			for(j = 0; j < 140; j++) {
//				len = (long) town->strlens[j];
//				FSWrite(dummy_f, &len, (char *) &(data_store->town_strs[j]));
//				}
//
//			for(i = 0; i < 200; i++)
//				talking.strlens[i] = 0;
//			for(i = 0; i < 170; i++)
//				talking.strlens[i] = strlen(data_store->talk_strs[i]);
//			len = sizeof(legacy::talking_record_type);
//			FSWrite(dummy_f, &len, (char *) &talking);
//			for(j = 0; j < 170; j++) {
//				len = (long) talking.strlens[j];
//				FSWrite(dummy_f, &len, (char *) &(data_store->talk_strs[j]));
//				}
//
//			}
//
//
//	// now, everything is moved over. Delete the original, and rename the dummy
//	 FSClose(dummy_f);
//	DisposePtr(buffer);
	giveError("Scenario not created; creation currently disabled.");
}

// When this is called, the current town is the town to make town 0.
void make_new_scenario(std::string /*file_name*/,short /*out_width*/,short /*out_height*/,bool /*making_warriors_grove*/,
					   bool /*use_grass*/) {
//	short i,j,k,num_outdoors;
//	FSSpec dummy_file;
//	NavReplyRecord reply;
//	short dummy_f,file_id;
//	Size buf_len = 100000;
//	OSErr error;
//	short out_num;
//	long len,scen_ptr_move = 0,save_town_size = 0,save_out_size = 0;
//	location loc;
//	short x,y;
//	legacy::big_tr_type t_d;
//	legacy::ave_tr_type ave_t;
//	legacy::tiny_tr_type tiny_t;
//
//	// Step 1 - load scenario file from scenario base. It contains all the monsters and
//	// items done up properly!
//	error = FSMakeFSSpec(start_volume,start_dir,"::::Blades of Exile Base",&temp_file_to_load);
//	if(error != 0) {oops_error(80);}
//
//	if((error = FSpOpenDF(&temp_file_to_load,1,&file_id)) != 0) {
//		give_error("Could not find the file 'Blades of Exile Base', which needs to be in the Scenario Editor folder. If you no longer have it, you need to redownload Blades of Exile.","",0);
//		oops_error(81);	return;
//		}
//
//	len = (long) sizeof(legacy::scenario_data_type);
//	if((error = FSRead(file_id, &len, (char *) &scenario)) != 0){
//		FSClose(file_id); oops_error(82); return;
//		}
//
//	len = sizeof(scen_item_data_type); // item data
//	if((error = FSRead(file_id, &len, (char *) &(data_store->scen_item_list))) != 0){
//		FSClose(file_id); oops_error(83); return;
//		}
//	for(i = 0; i < 270; i++) {
//		len = (long) (scenario.scen_str_len[i]);
//		FSRead(file_id, &len, (char *) &(data_store->scen_strs[i]));
//		data_store->scen_strs[i][len] = 0;
//		}
//	FSClose(file_id);
//
//	char newname[256] = "::::";
//	strcat((char*)newname,(char*)file_name);
//
//	// now write scenario
//	c2pstr((char*) newname);
//
//	//OK. FIrst find out what file name we're working with, and make the dummy file
//	// which we'll build the new scenario in   Blades of Exile Base
//	FSMakeFSSpec(file_to_load.vRefNum,file_to_load.parID,newname,&dummy_file);
//	FSpDelete(&dummy_file);
//	error = FSpCreate(&dummy_file,'blx!','BETM',smSystemScript);
//	if((error != 0) && (error != dupFNErr)) {
//				oops_error(1);
//				return;
//				}
//	if((error = FSpOpenDF(&dummy_file,3,&dummy_f)) != 0) {
//		oops_error(2);
//		return;
//		}
//
//
//	scenario.format.prog_make_ver[0] = 1;
//	scenario.format.prog_make_ver[1] = 0;
//	scenario.format.prog_make_ver[2] = 0;
//	cur_town = 0;
//	scenario.num_towns = 1;
//	scenario.town_size[0] = 1;
//	scenario.out_width = out_width;
//	scenario.out_height = out_height;
//	cur_out.x = 0;
//	cur_out.y = 0;
//	scenario.last_out_edited = cur_out;
//
//	// We're finally set up. Let's first set up the new scenario field
//	// We need the new info for the current town and outdoors, which may have been changed
//	scenario.town_data_size[cur_town][0] = sizeof(legacy::town_record_type);
//	if(scenario.town_size[cur_town] == 0)
//		scenario.town_data_size[cur_town][0] += sizeof(legacy::big_tr_type);
//		else if(scenario.town_size[cur_town] == 1)
//			scenario.town_data_size[cur_town][0] += sizeof(legacy::ave_tr_type);
//			else scenario.town_data_size[cur_town][0] += sizeof(legacy::tiny_tr_type);
//	scenario.town_data_size[cur_town][1] = 0;
//	for(i = 0; i < 60; i++)
//		scenario.town_data_size[cur_town][1] += strlen(data_store->town_strs[i]);
//	scenario.town_data_size[cur_town][2] = 0;
//	for(i = 60; i < 140; i++)
//		scenario.town_data_size[cur_town][2] += strlen(data_store->town_strs[i]);
//	scenario.town_data_size[cur_town][3] = sizeof(legacy::talking_record_type);
//	for(i = 0; i < 80; i++)
//		scenario.town_data_size[cur_town][3] += strlen(data_store->talk_strs[i]);
//	scenario.town_data_size[cur_town][4] = 0;
//	for(i = 80; i < 170; i++)
//		scenario.town_data_size[cur_town][4] += strlen(data_store->talk_strs[i]);
//
//	num_outdoors = scenario.out_width * scenario.out_height;
//	for(i = 0; i < num_outdoors; i++) {
//		scenario.out_data_size[i][0] = sizeof(legacy::outdoor_record_type);
//		scenario.out_data_size[i][1] = 0;
//		for(j = 0; j < 120; j++)
//			scenario.out_data_size[i][1] += strlen(data_store->out_strs[j]);
//		}
//
//	for(i = 0; i < 300; i++)
//		scenario.scen_str_len[i] = 0;
//	for(i = 0; i < 270; i++)
//		scenario.scen_str_len[i] = strlen(data_store->scen_strs[i]);
//	scenario.last_town_edited = cur_town;
//	scenario.last_out_edited = cur_out;
//
//	// now write scenario data
//	scenario.format.flag1 = 10;
//	scenario.format.flag2 = 20;
//	scenario.format.flag3 = 30;
//	scenario.format.flag4 = 40; /// these mean made on mac
//	// now flags
//	scenario.flag_a = sizeof(legacy::scenario_data_type) + get_ran(1,-1000,1000);
//	scenario.flag_b = town_s(user_given_password);
//	scenario.flag_c = out_s(user_given_password);
//	scenario.flag_e = str_size_1(user_given_password);
//	scenario.flag_f = str_size_2(user_given_password);
//	scenario.flag_h = str_size_3(user_given_password);
//	scenario.flag_g = 10000 + get_ran(1,0,5000);
//	scenario.flag_d = init_data(user_given_password);
//
//	len = sizeof(legacy::scenario_data_type); // scenario data
//	scen_ptr_move += len;
//	if((error = FSWrite(dummy_f, &len, (char *) &scenario)) != 0) {
//		FSClose(dummy_f);
//				oops_error(3);
//		return;
//		}
//	len = sizeof(scen_item_data_type); // item data
//	scen_ptr_move += len;
//	if((error = FSWrite(dummy_f, &len, (char *) &(data_store->scen_item_list))) != 0) {
// 		FSClose(dummy_f);
//		oops_error(4);
//		return;
//		}
//	for(i = 0; i < 270; i++) { // scenario strings
//		len = (long) scenario.scen_str_len[i];
//		scen_ptr_move += len;
//		if((error = FSWrite(dummy_f, &len, (char *) &(scenario.scen_strs(i)))) != 0) {
//			FSClose(dummy_f);oops_error(5);
//			return;
//			}
//		}
//
//
//	// OK ... scenario written. Now outdoors.
//	num_outdoors = scenario.out_width * scenario.out_height;
//	for(i = 0; i < num_outdoors; i++)
//		{
//			loc.x = i % scenario.out_width;
//			loc.y = i / scenario.out_width;
//
//			for(x = 0; x < 48; x++)
//				for(y = 0; y < 48; y++) {
//					current_terrain.terrain[x][y] = (use_grass > 0) ? 2 : 0;
//
//					if((x < 4) && (loc.x == 0))
//						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
//					if((y < 4) && (loc.y == 0))
//						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
//					if((x > 43) && (loc.x == scenario.out_width - 1))
//						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
//					if((y > 43) && (loc.y == scenario.out_height - 1))
//						current_terrain.terrain[x][y] = (use_grass > 0) ? 22 : 5;
//					if((i == 0) && (making_warriors_grove > 0)) {
//						current_terrain.terrain[24][24] = (use_grass > 0) ? 234 : 232;
//						current_terrain.exit_locs[0].x = 24;
//						current_terrain.exit_locs[0].y = 24;
//						current_terrain.exit_dests[0] = 0;
//						}
//						else current_terrain.exit_locs[0].x = 100;
//					}
//
//			// write outdoors
//			for(j = 0; j < 180; j++)
//				current_terrain.strlens[j] = 0;
//			for(j = 0; j < 120; j++)
//				current_terrain.strlens[j] = strlen(current_terrain.out_strs(j));
//			len = sizeof(legacy::outdoor_record_type);
//			error = FSWrite(dummy_f, &len, (char *) &current_terrain);
//			if(error != 0) {FSClose(dummy_f);oops_error(6);}
//
//			for(j = 0; j < 120; j++) {
//				len = (long) current_terrain.strlens[j];
//				error = FSWrite(dummy_f, &len, (char *) &(current_terrain.out_strs(j)));
//				if(error != 0) {
//					FSClose(dummy_f);
//					oops_error(7);
//				}
//			}
//		}
//
//
//	// now, finally, write towns.
//	for(k = 0; k < scenario.num_towns; k++)
//		{
//			for(i = 0; i < 180; i++)
//				town->strlens[i] = 0;
//			for(i = 0; i < 140; i++)
//				town->strlens[i] = strlen(town->town_strs[i]);
//
//			// write towns
//			len = sizeof(legacy::town_record_type);
//			error = FSWrite(dummy_f, &len, (char *) &town);
//			if(error != 0) {FSClose(dummy_f);oops_error(8);}
//
//			switch(scenario.town_size[cur_town]) {
//				case 0:
//					len = sizeof(legacy::big_tr_type);
//					FSWrite(dummy_f, &len, (char *) &t_d);
//					break;
//
//				case 1:
//					for(i = 0; i < 48; i++)
//						for(j = 0; j < 48; j++) {
//							ave_t.terrain[i][j] = town->terrain(i,j);
//							ave_t.lighting[i / 8][j] = town->lighting(i / 8,j);
//							}
//					for(i = 0; i < 16; i++) {
//						ave_t.room_rect[i] = town->room_rect(i);
//						}
//					for(i = 0; i < 40; i++) {
//						//ave_t.creatures[i] = town->creatures(i);
//						}
//					len = sizeof(legacy::ave_tr_type);
//					FSWrite(dummy_f, &len, (char *) &ave_t);
//				break;
//
//
//				case 2:
//					for(i = 0; i < 32; i++)
//						for(j = 0; j < 32; j++) {
//							tiny_t.terrain[i][j] = town->terrain(i,j);
//							tiny_t.lighting[i / 8][j] = town->lighting(i / 8,j);
//							}
//					for(i = 0; i < 16; i++) {
//						tiny_t.room_rect[i] = town->room_rect(i);
//						}
//					for(i = 0; i < 30; i++) {
//						//tiny_t.creatures[i] = town->creatures(i);
//						}
//					len = sizeof(legacy::tiny_tr_type);
//					FSWrite(dummy_f, &len, (char *) &tiny_t);
//					break;
//				}
//			for(j = 0; j < 140; j++) {
//				len = (long) town->strlens[j];
//				FSWrite(dummy_f, &len, (char *) &(town->town_strs[j]));
//				}
//
//			for(i = 0; i < 200; i++)
//				talking.strlens[i] = 0;
//			for(i = 0; i < 170; i++)
//				talking.strlens[i] = strlen(town->talk_strs[i]);
//			len = sizeof(legacy::talking_record_type);
//			error = FSWrite(dummy_f, &len, (char *) &talking);
//			if(error != 0) {FSClose(dummy_f);oops_error(9);}
//			for(j = 0; j < 170; j++) {
//				len = (long) talking.strlens[j];
//				FSWrite(dummy_f, &len, (char *) &(town->talk_strs[j]));
//				}
//
//			}
//
//
//	change_made = true;
//	// now, everything is moved over. Delete the original, and rename the dummy
//	error = FSClose(dummy_f);
//	if(error != 0) {FSClose(dummy_f);oops_error(10);}
//	file_to_load = dummy_file;
	giveError("Scenario not created; creation is currently disabled.");
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
				if(scenario.outdoors[out_sec.x][out_sec.y]->sign_strs[i][0] != '*')
					fout << "    Sign " << i << ": " << scenario.outdoors[out_sec.x][out_sec.y]->sign_strs[i] << endl;
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
			if(scenario.towns[i]->sign_strs[i][0] != '*')
				fout << "    Sign " << i << ": " << scenario.towns[i]->sign_strs[i] << endl;
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

