
#include <cstdio>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"

#include "boe.graphics.h"
#include "boe.newgraph.h"
#include "boe.items.h"
#include <cstring>
#include "boe.monster.h"
#include "boe.party.h"
#include "boe.locutils.h"
#include "boe.text.h"
#include "soundtool.h"
#include "boe.infodlg.h"
#include "boe.fileio.h"
#include "boe.main.h"
#include "mathutil.h"
#include "dlogutil.hpp"
#include "fileio.h"
#include "restypes.hpp"
#include <boost/lexical_cast.hpp>
#include "prefs.hpp"
#include "spell.hpp"

short mage_spell_pos = 0,priest_spell_pos = 0,skill_pos = 0;

extern short skill_cost[20];
extern short skill_max[20];
extern short skill_g_cost[20];
extern const char* skill_ids[19];
//extern party_record_type	party;
extern short cur_town_talk_loaded;
//extern current_town_type univ.town;
extern bool give_intro_hint;
extern sf::RenderWindow mainPtr;
extern short on_monst_menu[256];
//extern big_tr_type t_d;
extern location tinraya_portculli[12];
//extern piles_of_stuff_dumping_type *data_store;

bool full_roster = false;

extern short dest_personalities[40];
extern location dest_locs[40];

extern short dest_personalities[40];
location source_locs[6] = {loc(2,9),loc(0,6),loc(3,6),loc(3,4),loc(6,2),loc(0,0)};
//extern current_town_type univ.town;
extern location dest_locs[40] ;
extern char *alch_names[];
extern cUniverse univ;

// Displaying string vars
short store_str1a;
short store_str1b;
short store_str2a;
short store_str2b;
short store_page_on,store_num_i;

// Misc dialog vars
short position,num_entries;
unsigned short cur_entry;

static void put_spell_info(cDialog& me, eSkill display_mode) {
	std::ostringstream store_text;
	short pos,ran,spell_num;
	std::string res;
	
	pos = display_mode == eSkill::MAGE_SPELLS ? mage_spell_pos : priest_spell_pos;
	res = display_mode == eSkill::MAGE_SPELLS ? "mage-spells" : "priest-spells";
	spell_num = display_mode == eSkill::MAGE_SPELLS ? (pos + 1) : (pos + 101);
	eSpell spell = cSpell::fromNum(display_mode, pos);
	ran = (*spell).range;
	
	me["name"].setText(get_str("magic-names", spell_num));
	
	if((*spell).cost >= 0)
		store_text << (*spell).level << "/" << (*spell).cost;
	else store_text << (*spell).level << "/?";
	me["cost"].setText(store_text.str());
	
	if(ran == 0) {
		me["range"].setText("");
	}
	else me["range"].setTextToNum(ran);
	
	me["desc"].setText(get_str(res, pos + 1));
	me["when"].setText(get_str("spell-times", (*spell).when_cast));
}


static bool display_spells_event_filter(cDialog& me, std::string item_hit, eSkill display_mode) {
	short store;
	if(item_hit == "done") {
		me.toast(true);
	} else {
		store = display_mode == eSkill::MAGE_SPELLS ? mage_spell_pos : priest_spell_pos;
		if(item_hit == "left") {
			store = (store == 0) ? 61 : store - 1;
		}
		else {
			store = (store + 1) % 62;
		}
		if(display_mode == eSkill::MAGE_SPELLS)
			mage_spell_pos = store;
		else priest_spell_pos = store;
		put_spell_info(me, display_mode);
	}
	return true;
}
//short force_spell; // if 100, ignore
void display_spells(eSkill mode,short force_spell,cDialog* parent) {
	using namespace std::placeholders;
	if(force_spell < 100) {
		if(mode == eSkill::MAGE_SPELLS)
			mage_spell_pos = force_spell;
		else priest_spell_pos = force_spell;
	}
	
	make_cursor_sword();
	
	cDialog spellInfo("spell-info.xml", parent);
	spellInfo.attachClickHandlers(std::bind(display_spells_event_filter,_1,_2,mode), {"done","left","right"});
	
	dynamic_cast<cPict&>(spellInfo["icon"]).setPict(mode == eSkill::MAGE_SPELLS ? 14 : 15);
	put_spell_info(spellInfo, mode);
	if(mode == eSkill::MAGE_SPELLS)
		spellInfo["type"].setText("Mage Spells");
	else spellInfo["type"].setText("Priest Spells");
	
	spellInfo.run();
}

static void put_skill_info(cDialog& me) {
	std::string store_text;
	short pos;
	
	pos = skill_pos;
	
	store_text = get_str("skills",pos * 2 + 1);
	me["name"].setText(store_text.c_str());
	me["skp"].setTextToNum(skill_cost[pos]);
	me["gold"].setTextToNum(skill_g_cost[pos]);
	me["max"].setTextToNum(skill_max[pos]);
	
	store_text = get_str("skills", pos * 2 + 2);
	me["desc"].setText(store_text);
	store_text = get_str("tips", 1 + pos);
	me["tips"].setText(store_text);
}


static bool display_skills_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	if(item_hit == "done") {
		me.toast(true);
	} else {
		if(item_hit == "left") {
			skill_pos = (skill_pos == 0) ? 18 : skill_pos - 1;
		}
		else {
			skill_pos = (skill_pos + 1) % 19;
		}
		put_skill_info(me);
	}
	return true;
}

void display_skills(eSkill force_skill,cDialog* parent) {
	if(force_skill != eSkill::INVALID)
		skill_pos = int(force_skill);
	if(skill_pos < 0)
		skill_pos = 0;
	if(skill_pos > 18)
		skill_pos = 18;
	
	make_cursor_sword();
	
	cDialog skillDlog("skill-info.xml", parent);
	skillDlog.attachClickHandlers(display_skills_event_filter,{"done", "left", "right"});
	
	put_skill_info(skillDlog);
	
	skillDlog.run();
}

static void put_item_info(cDialog& me,const cItemRec& s_i) {
	char store_text[256];
	std::string desc_str;
	
	cPict& pic = dynamic_cast<cPict&>(me["pic"]);
	if(s_i.graphic_num >= 1000) // was 150
		pic.setPict(s_i.graphic_num - 1000, PIC_CUSTOM_ITEM);
	else pic.setPict(s_i.graphic_num, PIC_ITEM);
	
	// id? magic?
	cLed& id = dynamic_cast<cLed&>(me["id"]);
	if(s_i.magic && s_i.ident)
		id.setState(led_red);
	else id.setState(led_off);
	cLed& magic = dynamic_cast<cLed&>(me["magic"]);
	if(s_i.ident)
		magic.setState(led_red);
	else magic.setState(led_off);
	me["type"].setText(get_str("item-types-display", (int)s_i.variety));
	
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
		me["name"].setText(s_i.name.c_str());
		return;
	}
	
	me["name"].setText(s_i.full_name.c_str());
	me["weight"].setTextToNum(s_i.item_weight());
	
	// TODO: This calculation (value for an item with charges) should be in a member function of cItem
	me["val"].setTextToNum((s_i.charges > 0) ? s_i.value * s_i.charges : s_i.value);
	
	if(s_i.ability != eItemAbil::NONE) {
		if(s_i.concealed) {
			me["abil"].setText("???");
		} else {
			// TODO: More descriptive ability descriptions, taking into account potential variation
			desc_str = get_str("item-abilities",int(s_i.ability) + 1);
			me["abil"].setText(desc_str.c_str());
		}
	}
	if(s_i.charges > 0)
		me["use"].setTextToNum(s_i.charges);
	if(s_i.protection > 0)
		me["def"].setTextToNum(s_i.protection);
	
	switch(s_i.variety) {
		case eItemType::ONE_HANDED:
		case eItemType::TWO_HANDED:
			switch(s_i.type) {
				case eSkill::EDGED_WEAPONS:
					sprintf((char *) store_text, "Edged weapon");
					break;
				case eSkill::BASHING_WEAPONS:
					sprintf((char *) store_text, "Bashing weapon");
					break;
				case eSkill::POLE_WEAPONS:
					sprintf((char *) store_text, "Pole weapon");
					break;
				case eSkill::INVALID:
				default:
					sprintf((char*)store_text, "Error weapon"); // should never be reached
			}
			// TODO: I wonder if this would fit better in the Item Type box?
			if(s_i.ability == eItemAbil::NONE)
				me["abil"].setText(store_text);
		case eItemType::BOW:
		case eItemType::CROSSBOW:
		case eItemType::ARROW:
		case eItemType::THROWN_MISSILE:
		case eItemType::BOLTS:
		case eItemType::MISSILE_NO_AMMO:
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

static bool display_pc_item_event_filter(cDialog& me, std::string item_hit, cItemRec& store_i, short& item, const short pc_num) {
	
	if(item_hit == "done") {
		me.toast(true);
	} else if(item_hit == "left") {
		do {
			item = (item == 0) ? 23 : item - 1;
		} while(univ.party[pc_num].items[item].variety == eItemType::NO_ITEM);
		store_i = univ.party[pc_num].items[item];
		put_item_info(me,item);
	} else if(item_hit == "right") {
		do {
			item = (item == 23) ? 0 : item + 1;
		} while(univ.party[pc_num].items[item].variety == eItemType::NO_ITEM);
		store_i = univ.party[pc_num].items[item];
		put_item_info(me,item);
	}
	return true;
}

void display_pc_item(short pc_num,short item,cItemRec si,cDialog* parent) {
	using namespace std::placeholders;
	cItemRec store_i;
	if(pc_num == 6)
		store_i = si;
	else store_i = univ.party[pc_num].items[item];
	make_cursor_sword();
	
	cDialog itemInfo("item-info.xml",parent);
	// By attaching the click handler to "id" and "magic", we suppress normal LED behaviour
	itemInfo.attachClickHandlers(std::bind(display_pc_item_event_filter, _1, _2, std::ref(store_i), std::ref(item), pc_num), {"done","left","right","id","magic"});
	
	if(pc_num >= 6) {
		itemInfo["left"].hide();
		itemInfo["right"].hide();
	}
	itemInfo.addLabelFor("name", "Name:", LABEL_LEFT, 26, true);
	itemInfo.addLabelFor("type", "Type:", LABEL_LEFT, 26, true);
	itemInfo.addLabelFor("val", "Value:", LABEL_LEFT, 20, true);
	itemInfo.addLabelFor("dmg", "Damage:", LABEL_LEFT, 26, true);
	itemInfo.addLabelFor("bonus", "Bonus:", LABEL_LEFT, 24, true);
	itemInfo.addLabelFor("def", "Defend:", LABEL_LEFT, 25, true);
	itemInfo.addLabelFor("enc", "Encumb.:", LABEL_LEFT, 29, true);
	itemInfo.addLabelFor("use", "Uses:", LABEL_LEFT, 19, true);
	itemInfo.addLabelFor("lvl", "Item Level:", LABEL_LEFT, 37, true);
	itemInfo.addLabelFor("abil", "Ability", LABEL_LEFT, 26, true);
	itemInfo.addLabelFor("id", "ID?", LABEL_LEFT, 13, true);
	itemInfo.addLabelFor("magic", "Magic?", LABEL_LEFT, 22, true);
	itemInfo.addLabelFor("weight", "Weight", LABEL_LEFT, 27, true);
	
	itemInfo["desc"].hide();
	
	put_item_info(itemInfo,si);
	
	itemInfo.run();
}


static void put_monst_info(cDialog& me, const cCreature& store_m) {
	std::string store_text;
	std::string str;
	short abil,i;
	
	cPict& pic = dynamic_cast<cPict&>(me["pic"]);
	if(store_m.spec_skill == MONSTER_INVISIBLE)
		pic.setPict(400,PIC_MONST);// TODO: should probably be PICT_BLANK?
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
	
	store_text = get_m_name(store_m.number);
	me["name"].setText(store_text);
	
	// TODO: More descriptive ability descriptions, taking into account potential variation
	abil = store_m.spec_skill;
	str = get_str("monster-abilities",abil + 1);
	me["abil1"].setText(str);
	str = get_str("monster-abilities",store_m.radiate_1 + 50);
	me["abil2"].setText(str);
	
	for(i = 0; i < 3; i++) {
		if(store_m.a[i] > 0) {
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
	me["hp"].setTextToNum(store_m.health);
	me["sp"].setTextToNum(store_m.mp);
	me["def"].setTextToNum(store_m.armor);
	me["skill"].setTextToNum(store_m.skill);
	me["morale"].setTextToNum(store_m.morale);
	me["ap"].setTextToNum(store_m.speed);
	me["mage"].setTextToNum(store_m.mu);
	me["priest"].setTextToNum(store_m.cl);
	me["poison"].setTextToNum(store_m.poison);
	// 2140 - lit  2141 - dark
	// immunities
	for(i = 0; i < 8; i++) {
		std::string id = "immune" + boost::lexical_cast<std::string>(i + 1);
		cLed& led = dynamic_cast<cLed&>(me[id]);
		// TODO: What's this s_pow nonsense? Isn't this just 1 << i? Also, why not make it a C++ bitfield?
		if(store_m.immunities & (char)(s_pow(2,i)))
			led.setState(led_red);
		else led.setState(led_off);
	}
	
}


static bool display_monst_event_filter(cDialog& me, std::string item_hit, cCreature& store_m) {
	// This is a bit hacky; keep a cPopulation here to handle the full roster; it's treated like a rotating buffer.
	static cPopulation roster;
	short i;
	
	if(item_hit == "left") {
		if(position == 0) {
			for(i = 255; on_monst_menu[i] < 0 && i > 0; i--);
			position = i;
		}
		else position--;
		
		if(on_monst_menu[position] < 0)
			position = 0;
	} else if(item_hit == "right") {
		position++;
		if(on_monst_menu[position] < 0)
			position = 0;
	} else if(item_hit != "none")
		return true; // Means an immunity LED was hit
	
	if(roster[position % 60].number != on_monst_menu[position]) {
		cMonster& monst = univ.scenario.scen_monsters[on_monst_menu[position]];
		roster.assign(position % 60, cCreature(on_monst_menu[position]), monst, PSD[SDF_EASY_MODE], univ.difficulty_adjust());
	}
	put_monst_info(me, store_m);
	return true;
}

//creature_data_type *which_m; // if NULL, show full roster
//short mode; // if 1, full roster, else use monster from store_m
void display_monst(short array_pos,cCreature *which_m,short mode) {
	using namespace std::placeholders;
	position = array_pos;
	full_roster = false;
	cCreature store_m;
	if(mode == 1) full_roster = true;
	else store_m = *which_m;
	
	make_cursor_sword();
	
	cDialog monstInfo("monster-info.xml");
	auto event_filter = std::bind(display_monst_event_filter, _1, _2,std::ref(store_m));
	monstInfo["done"].attachClickHandler(std::bind(&cDialog::toast, &monstInfo, true));
	monstInfo.attachClickHandlers(event_filter, {"left", "right"});
	// Also add the click handler to the LEDs to suppress normal LED behaviour
	monstInfo.attachClickHandlers(event_filter, {"immune1", "immune2", "immune3", "immune4"});
	monstInfo.attachClickHandlers(event_filter, {"immune5", "immune6", "immune7", "immune8"});
	
	if(full_roster) {
		// This is a bit hacky - call event handler with dummy ID to ensure the monster details are correctly populated.
		display_monst_event_filter(monstInfo, "none", store_m);
	} else {
		monstInfo["left"].hide();
		monstInfo["right"].hide();
	}
	// TODO: Put these labels in the XML definition
	monstInfo.addLabelFor("name", "Name", LABEL_LEFT, 26, true);
	monstInfo.addLabelFor("lvl", "Level", LABEL_LEFT, 21, true);
	monstInfo.addLabelFor("hp", "Health", LABEL_LEFT, 24, true);
	monstInfo.addLabelFor("sp", "Magic Pts.", LABEL_LEFT, 32, true);
	monstInfo.addLabelFor("def", "Armor", LABEL_LEFT, 23, true);
	monstInfo.addLabelFor("skill", "Skill", LABEL_LEFT, 18, true);
	monstInfo.addLabelFor("morale", "Morale", LABEL_LEFT, 23, true);
	monstInfo.addLabelFor("ap", "Speed", LABEL_LEFT, 19, true);
	monstInfo.addLabelFor("attack1", "Att #1", LABEL_LEFT, 26, true);
	monstInfo.addLabelFor("attack2", "Att #2", LABEL_LEFT, 23, true);
	monstInfo.addLabelFor("attack3", "Att #3", LABEL_LEFT, 22, true);
	monstInfo.addLabelFor("mage", "Mage L.", LABEL_LEFT, 34, true);
	monstInfo.addLabelFor("priest", "Priest L.", LABEL_LEFT, 30, true);
	monstInfo.addLabelFor("poison", "Poison", LABEL_LEFT, 23, true);
	monstInfo.addLabelFor("abil1", "Ability 1", LABEL_LEFT, 29, true);
	monstInfo.addLabelFor("abil2", "Ability 2", LABEL_LEFT, 29, true);
	// TODO: These could be the actual content text instead of a label, maybe?
	monstInfo.addLabelFor("immune1", "Magic Resistant", LABEL_LEFT, 45, false);
	monstInfo.addLabelFor("immune2", "Immune To Magic", LABEL_LEFT, 45, false);
	monstInfo.addLabelFor("immune3", "Fire Resistant", LABEL_LEFT, 45, false);
	monstInfo.addLabelFor("immune4", "Immune To Fire", LABEL_LEFT, 45, false);
	monstInfo.addLabelFor("immune5", "Cold Resistant", LABEL_LEFT, 45, false);
	monstInfo.addLabelFor("immune6", "Immune To Cold", LABEL_LEFT, 45, false);
	monstInfo.addLabelFor("immune7", "Poison Resistant", LABEL_LEFT, 45, false);
	monstInfo.addLabelFor("immune8", "Immune To Poison", LABEL_LEFT, 45, false);
	put_monst_info(monstInfo, store_m);
	
	monstInfo.run();
}

// TODO: Create a dedicated dialog for alchemy info
static bool display_alchemy_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	std::string get_text;
	
	if(item_hit == "done") {
		me.toast(true);
		return true;
	} else if(item_hit == "left") {
		cur_entry = (cur_entry == 3) ? num_entries + 2 : cur_entry - 1;
	} else if(item_hit == "right") {
		cur_entry = (cur_entry == num_entries + 2) ? 3 : cur_entry + 1;
	}
	get_text = get_str("alchemy", cur_entry);
	me["str"].setText(get_text);
	return true;
}

void display_alchemy() {
	std::string get_text;
	
	cur_entry = 3;
	
	make_cursor_sword();
	
	cDialog alchemy("many-str.xml");
	alchemy.attachClickHandlers(display_alchemy_event_filter, {"done", "left", "right"});
	
	get_text = get_str("alchemy", 1);
	alchemy["title"].setText(get_text);
	get_text = get_str("alchemy", 2);
	num_entries = boost::lexical_cast<long>(get_text);
	get_text = get_str("alchemy", cur_entry);
	alchemy["str"].setText(get_text);
	
	alchemy.run();
}

static void display_pc_info(cDialog& me, const short pc) {
	short i,store;
	char str[256];
	char to_draw[60];
	
	short weap1 = 24,weap2 = 24,hit_adj = 0, dam_adj = 0,skill_item;
	
	store = pc_carry_weight(pc);
	i = amount_pc_can_carry(pc);
	sprintf ((char *) to_draw, "%s is carrying %d stones out of %d.",univ.party[pc].name.c_str(),store,i);
	me["weight"].setText(to_draw);
	
	sprintf((char *) str,"%d out of %d.",
			univ.party[pc].cur_health,univ.party[pc].max_health);
	me["hp"].setText(str);
	sprintf((char *) str,"%d out of %d.",
			univ.party[pc].cur_sp,univ.party[pc].max_sp);
	me["sp"].setText(str);
	
	for(i = 0; i < 19; i++) {
		eSkill skill = eSkill(i);
		me[skill_ids[i]].setTextToNum(univ.party[pc].skills[skill]);
	}
	store = total_encumberance(pc);
	me["encumb"].setTextToNum(store);
	me["name"].setText(univ.party[pc].name);
	me["lvl"].setTextToNum(univ.party[pc].level);
	me["xp"].setTextToNum(univ.party[pc].experience);
	me["skp"].setTextToNum(univ.party[pc].skill_pts);
	store = univ.party[pc].level * univ.party[pc].get_tnl();
	me["progress"].setTextToNum(store);
	dynamic_cast<cPict&>(me["pic"]).setPict(univ.party[pc].which_graphic,PIC_PC); // TODO: Was adding 800 needed, or a relic?
	
	// Fight bonuses
	for(i = 0; i < 24; i++)
		if((univ.party[pc].items[i].variety == eItemType::ONE_HANDED || univ.party[pc].items[i].variety == eItemType::TWO_HANDED) &&
			(univ.party[pc].equip[i])) {
			if(weap1 == 24)
				weap1 = i;
			else weap2 = i;
		}
	
	hit_adj = stat_adj(pc,eSkill::DEXTERITY) * 5 - (total_encumberance(pc)) * 5
		+ 5 * minmax(-8,8,univ.party[pc].status[eStatus::BLESS_CURSE]);
	if(!univ.party[pc].traits[eTrait::AMBIDEXTROUS] && weap2 < 24)
		hit_adj -= 25;
	
	dam_adj = stat_adj(pc,eSkill::STRENGTH) + minmax(-8,8,univ.party[pc].status[eStatus::BLESS_CURSE]);
	if((skill_item = pc_has_abil_equip(pc,eItemAbil::SKILL)) < 24) {
		hit_adj += 5 * (univ.party[pc].items[skill_item].item_level / 2 + 1);
		dam_adj += univ.party[pc].items[skill_item].item_level / 2;
	}
	if((skill_item = pc_has_abil_equip(pc,eItemAbil::GIANT_STRENGTH)) < 24) {
		dam_adj += univ.party[pc].items[skill_item].item_level;
		hit_adj += univ.party[pc].items[skill_item].item_level * 2;
	}
	
	me["weap1a"].setText("No weapon.");
	me["weap1b"].setText("");
	me["weap2a"].setText("No weapon.");
	me["weap2b"].setText("");
	if(weap1 < 24) {
		if(!univ.party[pc].items[weap1].ident)
			me["weap1a"].setText("Not identified.");
		else {
			// TODO: What's with always putting the percent sign in front?
			if(hit_adj + 5 * univ.party[pc].items[weap1].bonus < 0)
				sprintf(to_draw,"Penalty to hit: %%%d",hit_adj + 5 * univ.party[pc].items[weap1].bonus);
			else sprintf(to_draw,"Bonus to hit: +%%%d",hit_adj + 5 * univ.party[pc].items[weap1].bonus);
			me["weap1a"].setText(to_draw);
			sprintf(to_draw,"Damage: (1-%d) + %d",univ.party[pc].items[weap1].item_level
					,dam_adj + univ.party[pc].items[weap1].bonus);
			me["weap1b"].setText(to_draw);
			
		}
	}
	if(weap2 < 24) {
		if(!univ.party[pc].items[weap2].ident)
			me["weap2a"].setText("Not identified.");
		else {
			if(hit_adj + 5 * univ.party[pc].items[weap2].bonus < 0)
				sprintf(to_draw,"Penalty to hit: %%%d",hit_adj + 5 * univ.party[pc].items[weap2].bonus);
			else sprintf(to_draw,"Bonus to hit: +%%%d",hit_adj + 5 * univ.party[pc].items[weap2].bonus);
			me["weap2a"].setText(to_draw);
			sprintf(to_draw,"Damage: (1-%d) + %d",univ.party[pc].items[weap2].item_level
					,dam_adj + univ.party[pc].items[weap2].bonus);
			me["weap2b"].setText(to_draw);
			
		}
	}
}

static bool give_pc_info_event_filter(cDialog& me, std::string item_hit, short& store_pc_num) {
	if(item_hit == "done") me.toast(true);
	else if(item_hit == "left") {
		// TODO: Put braces on these do-whiles... o.O
		do
			store_pc_num = (store_pc_num == 0) ? 5 : store_pc_num - 1;
		while(univ.party[store_pc_num].main_status != eMainStatus::ALIVE);
		display_pc_info(me, store_pc_num);
	} else if(item_hit == "right") {
		do
			store_pc_num = (store_pc_num + 1) % 6;
		while(univ.party[store_pc_num].main_status != eMainStatus::ALIVE);
		display_pc_info(me, store_pc_num);
	}
	return true;
}

static bool give_pc_extra_info(cDialog& me, std::string item_hit, const short pc) {
	if(item_hit == "seemage") display_pc(pc,0,&me);
	else if(item_hit == "seepriest") display_pc(pc,1,&me);
	else if(item_hit == "trait") pick_race_abil(&univ.party[pc],1);
	else if(item_hit == "alch") display_alchemy(false);
	return true;
}

void give_pc_info(short pc_num) {
	using namespace std::placeholders;
	short i;
	std::string str;
	
	make_cursor_sword();
	
	cDialog pcInfo("pc-info.xml");
	pcInfo.attachClickHandlers(std::bind(give_pc_info_event_filter, _1, _2, std::ref(pc_num)), {"done", "left", "right"});
	pcInfo.attachClickHandlers(give_pc_extra_info, {"seemage", "seepriest", "trait", "alch"});
	
	for(i = 0; i < 19; i++) {
		std::string lbl= "lbl" + boost::lexical_cast<std::string>(i + 1);
		str = get_str("skills",1 + i * 2);
		pcInfo[lbl].setText(str);
	}
	display_pc_info(pcInfo, pc_num);
	
	pcInfo.run();
}

static bool adventure_notes_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	unsigned short i;
	
	if(item_hit == "done") me.toast(true);
	else if(item_hit == "left") {
		if(store_page_on == 0)
			store_page_on = (store_num_i - 1) / 3;
		else store_page_on--;
	} else if(item_hit == "right") {
		if(store_page_on == (store_num_i - 1) / 3)
			store_page_on = 0;
		else store_page_on++;
	} else if(item_hit.substr(0,3) == "del") {
		int n = item_hit[3] - '1';
		short which_to_delete = (store_page_on * 3) + n;
		cParty::encIter iter = univ.party.special_notes.begin();
		iter += which_to_delete;
		univ.party.special_notes.erase(iter);
	}
	for(i = 0; i < 3; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if(univ.party.special_notes.size() > i) {
			me["str" + n].setText(univ.party.special_notes[i].the_str);
			me["del" + n].show();
		}
		else me["del" + n].hide();
	}
	// TODO: What's this second loop for?
	for(i = store_page_on * 3; i < (store_page_on * 3) + 3; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if(univ.party.special_notes.size() > i) {
			me["str" + n].setText(univ.party.special_notes[i].the_str);
			me["del" + n].show();
		}
		else {
			me["str" + n].setText("");
			me["del" + n].hide();
		}
	}
	return true;
}

void adventure_notes() {
	
	unsigned short i;
	
	store_num_i = 0;
	//for(i = 0; i < 140; i++)
	//	if(univ.party.special_notes_str[i][0] > 0)
	store_num_i = univ.party.special_notes.size(); //i + 1;
	store_page_on = 0;
	if(store_num_i == 0) {
		ASB("Nothing in your journal.");
		print_buf();
		return;
	}
	
	make_cursor_sword();
	
	cDialog encNotes("adventure-notes.xml");
	encNotes.attachClickHandlers(adventure_notes_event_filter, {"done", "left", "rigth", "del1", "del2", "del3"});
	
	for(i = 0; i < 3; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if(univ.party.special_notes.size() > i) {
			encNotes["str" + n].setText(univ.party.special_notes[i].the_str);
			encNotes["del" + n].show();
		}
		else encNotes["del" + n].hide();
	}
	if(store_num_i <= 3) {
		encNotes["left"].hide();
		encNotes["right"].hide();
	}
	
	encNotes.run();
}

static void put_talk(cDialog& me) {
	if(univ.party.talk_save[store_page_on].filled) {
		me["loc"].setText(univ.party.talk_save[store_page_on].in_town);
		me["who"].setText(univ.party.talk_save[store_page_on].who_said);
		me["str1"].setText(univ.party.talk_save[store_page_on].the_str1);
		me["str2"].setText(univ.party.talk_save[store_page_on].the_str2);
	}
}

static bool talk_notes_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	if(item_hit == "done") me.toast(true);
	else if(item_hit == "left") {
		if(store_page_on == 0)
			store_page_on = store_num_i - 1;
		else store_page_on--;
	} else if(item_hit == "right") {
		if(store_page_on == store_num_i - 1)
			store_page_on = 0;
		else store_page_on++;
	} else if(item_hit == "del") {
		// TODO: Actually remove it rather than filled to false
		univ.party.talk_save[store_page_on].filled = false;
	}
	put_talk(me);
	return true;
}

void talk_notes() {
	
	short i;
	
	store_num_i = 0;
	for(i = 0; i < 120; i++)
		if(univ.party.talk_save[i].filled)
			store_num_i = i + 1;
	store_page_on = 0;
	if(store_num_i == 0) {
		ASB("Nothing in your talk journal.");
		print_buf();
		return;
	}
	
	make_cursor_sword();
	
	cDialog talkNotes("talk-notes.xml");
	talkNotes.attachClickHandlers(talk_notes_event_filter, {"done", "left", "right", "del"});
	
	put_talk(talkNotes);
	if(store_num_i == 1) {
		talkNotes["left"].hide();
		talkNotes["right"].hide();
	}
	
	talkNotes.run();
}

static bool journal_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	short i;
	char place_str[256];
	
	if(item_hit == "done") me.toast(true);
	else if(item_hit == "left") {
		if(store_page_on == 0)
			store_page_on = (store_num_i - 1) / 3;
		else store_page_on--;
	} else if(item_hit == "right") {
		if(store_page_on == (store_num_i - 1) / 3)
			store_page_on = 0;
		else store_page_on++;
	}
	for(i = 0; i < 3; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if((long)univ.party.journal.size() > i + (store_page_on * 3)) {
			me["str" + n].setText(univ.party.journal[i].the_str);
			sprintf((char *)place_str,"Day: %d",univ.party.journal[i].day);
			me["day" + n].setText(place_str);
		}
		else{
			me["str" + n].setText("");
			me["day" + n].setText("");
		}
	}
	return true;
}

void journal() {
	
	unsigned short i;
	char place_str[256];
	
	store_num_i = 0;
	//for(i = 0; i < 120; i++)
	//if(univ.party.journal[i].str_num > 0)
	store_num_i = univ.party.journal.size();//i + 1;
	store_page_on = 0;
	
	make_cursor_sword();
	
	cDialog journal("event-journal.xml");
	journal.attachClickHandlers(journal_event_filter, {"done", "left", "right"});
	
	for(i = 0; i < 3; i++) {
		if(univ.party.journal.size() > i) {
			std::string n = boost::lexical_cast<std::string>(i + 1);
			journal["str" + n].setText(univ.party.journal[i].the_str);
			sprintf((char *)place_str,"Day: %d",univ.party.journal[i].day);
			journal["day" + n].setText(place_str);
		}
	}
	if(store_num_i <= 3) {
		journal["left"].hide();
		journal["right"].hide();
	}
	
	journal.run();
}

void add_to_journal(short event) {
	if(univ.party.add_to_journal(univ.scenario.journal_strs[event], calc_day()))
		ASB("Something was added to your journal.");
}

// Call call this anywhere, but don't forget parent!!!
static void give_help(short help1,short help2,cDialog* parent) {
	bool help_forced = false;
	std::string str1,str2;
	
	if(help1 >= 200) {
		help_forced = true;
		help1 -= 200;
	}
	if((PSD[SDF_NO_INSTANT_HELP] > 0) && !help_forced)
		return;
	if(univ.party.help_received[help1] > 0)
		return;
	//if(help1 >= 20)
	univ.party.help_received[help1] = 1;
	append_iarray_pref("ReceivedHelp", help1);
	str1 = get_str("help",help1);
	if(help2 > 0)
		str2 = get_str("help",help2);
	cStrDlog display_strings(str1,str2,"Instant Help",24,PIC_DLOG, parent);
	display_strings.setSound(57);
	display_strings.show();
}

void give_help(short help1, short help2) {
	give_help(help1, help2, NULL);
}

void give_help(short help1, short help2, cDialog& parent) {
	give_help(help1, help2, &parent);
}

void put_spec_item_info (short which_i) {
	cStrDlog display_strings(univ.scenario.special_items[which_i].descr,"",
							 univ.scenario.special_items[which_i].name,univ.scenario.intro_pic,PIC_SCEN);
	display_strings.setSound(57);
	display_strings.show();
	//item_name = get_str(6,1 + which_i * 2);
	//display_strings(6,2 + which_i * 2,0,0,
	//(char *)item_name,-1,702,0);
}

// Callback for recording encounter strings
void cStringRecorder::operator()(cDialog& me) {
	play_sound(0);
	std::string str1, str2;
	switch(type) {
		case NOTE_SCEN:
			str1 = univ.scenario.spec_strs[label1];
			str2 = univ.scenario.spec_strs[label2];
			break;
		case NOTE_TOWN:
			str1 = univ.town->spec_strs[label1];
			str2 = univ.town->spec_strs[label2];
			break;
		case NOTE_OUT:
			str1 = univ.scenario.outdoors[label1b][label2b]->spec_strs[label1];
			str2 = univ.scenario.outdoors[label1b][label2b]->spec_strs[label2];
			break;
		case NOTE_MONST:
			str1 = univ.scenario.monst_strs[label1];
			str2 = univ.scenario.monst_strs[label2];
			break;
	}
	if(univ.party.record(type, str1, location))
		give_help(58,0,&me);
	univ.party.record(type, str2, location);
}

