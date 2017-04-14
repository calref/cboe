
#include <cstdio>
#include <map>

#include "boe.global.hpp"

#include "universe.hpp"

#include "boe.graphics.hpp"
#include "boe.newgraph.hpp"
#include "boe.items.hpp"
#include <cstring>
#include "boe.monster.hpp"
#include "boe.party.hpp"
#include "boe.locutils.hpp"
#include "boe.text.hpp"
#include "sounds.hpp"
#include "boe.infodlg.hpp"
#include "boe.fileio.hpp"
#include "boe.main.hpp"
#include "mathutil.hpp"
#include "strdlog.hpp"
#include "button.hpp"
#include "fileio.hpp"
#include <boost/lexical_cast.hpp>
#include "prefs.hpp"
#include "spell.hpp"
#include "view_dialogs.hpp"
#include "cursors.hpp"
#include "utility.hpp"

short mage_spell_pos = 0,priest_spell_pos = 0,skill_pos = 0;

extern std::map<eSkill,short> skill_cost;
extern std::map<eSkill,short> skill_max;
extern std::map<eSkill,short> skill_g_cost;
extern short cur_town_talk_loaded;
extern sf::RenderWindow mainPtr;
extern short on_monst_menu[256];

bool full_roster = false;

extern short dest_personalities[40];
extern location dest_locs[40];

extern short dest_personalities[40];
location source_locs[6] = {loc(2,9),loc(0,6),loc(3,6),loc(3,4),loc(6,2),loc(0,0)};
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
short position;
long num_entries;
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
	
	set_cursor(sword_curs);
	
	cDialog spellInfo("spell-info", parent);
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
	eSkill pos;
	
	pos = eSkill(skill_pos);
	
	store_text = get_str("skills",skill_pos * 2 + 1);
	me["name"].setText(store_text.c_str());
	me["skp"].setTextToNum(skill_cost[pos]);
	me["gold"].setTextToNum(skill_g_cost[pos]);
	me["max"].setTextToNum(skill_max[pos]);
	
	store_text = get_str("skills", skill_pos * 2 + 2);
	me["desc"].setText(store_text);
	store_text = get_str("tips", 1 + skill_pos);
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
	
	set_cursor(sword_curs);
	
	cDialog skillDlog("skill-info", parent);
	skillDlog.attachClickHandlers(display_skills_event_filter,{"done", "left", "right"});
	
	put_skill_info(skillDlog);
	
	skillDlog.run();
}

static bool display_pc_item_event_filter(cDialog& me, std::string item_hit, cItem& store_i, short& item, const short pc_num) {
	
	if(item_hit == "done") {
		me.toast(true);
	} else if(item_hit == "left") {
		do {
			item = (item == 0) ? 23 : item - 1;
		} while(univ.party[pc_num].items[item].variety == eItemType::NO_ITEM);
		store_i = univ.party[pc_num].items[item];
		put_item_info(me,store_i,univ.scenario);
	} else if(item_hit == "right") {
		do {
			item = (item == 23) ? 0 : item + 1;
		} while(univ.party[pc_num].items[item].variety == eItemType::NO_ITEM);
		store_i = univ.party[pc_num].items[item];
		put_item_info(me,store_i,univ.scenario);
	}
	return true;
}

void display_pc_item(short pc_num,short item,cItem si,cDialog* parent) {
	using namespace std::placeholders;
	cItem store_i;
	if(pc_num == 6)
		store_i = si;
	else store_i = univ.party[pc_num].items[item];
	set_cursor(sword_curs);
	
	cDialog itemInfo("item-info",parent);
	// By attaching the click handler to "id" and "magic", we suppress normal LED behaviour
	itemInfo.attachClickHandlers(std::bind(display_pc_item_event_filter, _1, _2, std::ref(store_i), std::ref(item), pc_num), {"done","left","right","id","magic"});
	
	if(pc_num >= 6) {
		itemInfo["left"].hide();
		itemInfo["right"].hide();
	}
	
	put_item_info(itemInfo,si,univ.scenario);
	
	itemInfo.run();
}

static bool display_monst_event_filter(cDialog& me, std::string item_hit, cCreature& store_m) {
	// This is a bit hacky; keep a cPopulation here to handle the full roster; it's treated like a rotating buffer.
	static cPopulation roster;
	roster.init(60);
	
	if(item_hit == "left") {
		if(position == 0) {
			for(short i = 255; on_monst_menu[i] < 0 && i > 0; i--)
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
		roster.assign(position % 60, cCreature(on_monst_menu[position]), monst, univ.party.easy_mode, univ.difficulty_adjust());
		store_m = roster[position % 60];
	}
	put_monst_info(me, store_m, univ.scenario);
	return true;
}

//creature_data_type *which_m; // if nullptr, show full roster
//short mode; // if 1, full roster, else use monster from store_m
void display_monst(short array_pos,cCreature *which_m,short mode) {
	using namespace std::placeholders;
	position = array_pos;
	full_roster = false;
	cCreature store_m;
	if(mode == 1) full_roster = true;
	else store_m = *which_m;
	
	set_cursor(sword_curs);
	
	cDialog monstInfo("monster-info");
	auto event_filter = std::bind(display_monst_event_filter, _1, _2,std::ref(store_m));
	monstInfo["done"].attachClickHandler(std::bind(&cDialog::toast, &monstInfo, true));
	monstInfo.attachClickHandlers(event_filter, {"left", "right"});
	// Also add the click handler to the LEDs to suppress normal LED behaviour
	monstInfo.attachClickHandlers(event_filter, {"guard", "mindless", "invuln"});
	
	if(full_roster) {
		// This is a bit hacky - call event handler with dummy ID to ensure the monster details are correctly populated.
		display_monst_event_filter(monstInfo, "none", store_m);
	} else {
		monstInfo["left"].hide();
		monstInfo["right"].hide();
	}
	put_monst_info(monstInfo, store_m, univ.scenario);
	
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
	
	set_cursor(sword_curs);
	
	cDialog alchemy("many-str");
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
	std::ostringstream to_draw;
	
	short hit_adj = 0, dam_adj = 0;
	
	to_draw << univ.party[pc].name << " is carrying " << univ.party[pc].cur_weight() << " stones out of " << univ.party[pc].max_weight() << '.';
	me["weight"].setText(to_draw.str());
	to_draw.str("");
	
	to_draw << univ.party[pc].cur_health << " out of " << univ.party[pc].max_health << '.';
	me["hp"].setText(to_draw.str());
	to_draw.str("");
	to_draw << univ.party[pc].cur_sp << " out of " << univ.party[pc].max_sp << '.';
	me["sp"].setText(to_draw.str());
	to_draw.str("");
	
	for(short i = 0; i < 19; i++) {
		eSkill skill = eSkill(i);
		int bonus = univ.party[pc].get_prot_level(eItemAbil::BOOST_STAT, i);
		to_draw << univ.party[pc].skills[skill];
		if(bonus > 0) to_draw << '+' << bonus;
		me[boost::lexical_cast<std::string>(skill)].setText(to_draw.str());
		to_draw.str("");
	}
	me["encumb"].setTextToNum(total_encumbrance(pc));
	me["name"].setText(univ.party[pc].name);
	me["lvl"].setTextToNum(univ.party[pc].level);
	me["xp"].setTextToNum(univ.party[pc].experience);
	me["skp"].setTextToNum(univ.party[pc].skill_pts);
	me["progress"].setTextToNum(univ.party[pc].level * univ.party[pc].get_tnl());
	pic_num_t pic = univ.party[pc].which_graphic;
	if(pic >= 100 && pic < 1000)
		dynamic_cast<cPict&>(me["pic"]).setPict(pic - 100,PIC_MONST);
	else dynamic_cast<cPict&>(me["pic"]).setPict(pic,PIC_PC);
	
	// Fight bonuses
	auto weapons = univ.party[pc].get_weapons();
	auto& weap1 = weapons.first;
	auto& weap2 = weapons.second;
	
	hit_adj = univ.party[pc].stat_adj(eSkill::DEXTERITY) * 5 - (total_encumbrance(pc)) * 5
		+ 5 * minmax(-8,8,univ.party[pc].status[eStatus::BLESS_CURSE]);
	if(!univ.party[pc].traits[eTrait::AMBIDEXTROUS] && weap2)
		hit_adj -= 25;
	
	// TODO: Perhaps dam_adj and hit_adj calculation should be moved into a function somewhere?
	dam_adj = univ.party[pc].stat_adj(eSkill::STRENGTH) + minmax(-8,8,univ.party[pc].status[eStatus::BLESS_CURSE]);
	if(cInvenSlot skill_item = univ.party[pc].has_abil_equip(eItemAbil::SKILL)) {
		hit_adj += 5 * (skill_item->abil_data[0] / 2 + 1);
		dam_adj += skill_item->abil_data[0] / 2;
	}
	if(cInvenSlot skill_item = univ.party[pc].has_abil_equip(eItemAbil::GIANT_STRENGTH)) {
		dam_adj += skill_item->abil_data[0];
		hit_adj += skill_item->abil_data[0] * 2;
	}
	
	me["weap1a"].setText("No weapon.");
	me["weap1b"].setText("");
	me["weap2a"].setText("No weapon.");
	me["weap2b"].setText("");
	if(weap1) {
		if(!weap1->ident)
			me["weap1a"].setText("Not identified.");
		else {
			// TODO: What's with always putting the percent sign in front?
			if(hit_adj + 5 * weap1->bonus < 0)
				to_draw << "Penalty to hit: %" << hit_adj + 5 * weap1->bonus;
			else to_draw << "Bonus to hit: +%" << hit_adj + 5 * weap1->bonus;
			me["weap1a"].setText(to_draw.str());
			to_draw.str("");
			to_draw << "Damage: (1-" << weap1->item_level << ") + " << dam_adj + weap1->bonus;
			me["weap1b"].setText(to_draw.str());
			to_draw.str("");
		}
	}
	if(weap2) {
		if(!weap2->ident)
			me["weap2a"].setText("Not identified.");
		else {
			if(hit_adj + 5 * weap2->bonus < 0)
				to_draw << "Penalty to hit: %" << hit_adj + 5 * weap2->bonus;
			else to_draw << "Bonus to hit: +%" << hit_adj + 5 * weap2->bonus;
			me["weap2a"].setText(to_draw.str());
			to_draw.str("");
			to_draw << "Damage: (1-" << weap2->item_level << ") + " << dam_adj + weap2->bonus;
			me["weap2b"].setText(to_draw.str());
			to_draw.str("");
		}
	}
}

static bool give_pc_info_event_filter(cDialog& me, std::string item_hit, short& store_pc_num) {
	if(item_hit == "done") me.toast(true);
	else if(item_hit == "left") {
		do {
			store_pc_num = (store_pc_num == 0) ? 5 : store_pc_num - 1;
		} while(univ.party[store_pc_num].main_status != eMainStatus::ALIVE);
		display_pc_info(me, store_pc_num);
	} else if(item_hit == "right") {
		do {
			store_pc_num = (store_pc_num + 1) % 6;
		} while(univ.party[store_pc_num].main_status != eMainStatus::ALIVE);
		display_pc_info(me, store_pc_num);
	}
	return true;
}

static bool give_pc_extra_info(cDialog& me, std::string item_hit, const short pc) {
	if(item_hit == "seemage") display_pc(pc,0,&me);
	else if(item_hit == "seepriest") display_pc(pc,1,&me);
	else if(item_hit == "trait") pick_race_abil(&univ.party[pc],1,&me);
	else if(item_hit == "seealch") display_alchemy(false,&me);
	return true;
}

void give_pc_info(short pc_num) {
	using namespace std::placeholders;
	std::string str;
	
	set_cursor(sword_curs);
	
	cDialog pcInfo("pc-info");
	pcInfo.attachClickHandlers(std::bind(give_pc_info_event_filter, _1, _2, std::ref(pc_num)), {"done", "left", "right"});
	pcInfo.attachClickHandlers(std::bind(give_pc_extra_info, _1, _2, std::ref(pc_num)), {"seemage", "seepriest", "trait", "seealch"});
	
	for(short i = 0; i < 19; i++) {
		std::string lbl= "lbl" + boost::lexical_cast<std::string>(i + 1);
		str = get_str("skills",1 + i * 2);
		pcInfo[lbl].setText(str);
	}
	display_pc_info(pcInfo, pc_num);
	
	pcInfo.run();
}

static bool adventure_notes_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
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
	for(short i = 0; i < 3; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if(univ.party.special_notes.size() > i) {
			me["str" + n].setText(univ.party.special_notes[i].the_str);
			me["del" + n].show();
		}
		else me["del" + n].hide();
	}
	// TODO: What's this second loop for?
	for(short i = store_page_on * 3; i < (store_page_on * 3) + 3; i++) {
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
	store_num_i = 0;
	store_num_i = univ.party.special_notes.size();
	store_page_on = 0;
	if(store_num_i == 0) {
		ASB("Nothing in your journal.");
		print_buf();
		return;
	}
	
	set_cursor(sword_curs);
	
	cDialog encNotes("adventure-notes");
	encNotes.attachClickHandlers(adventure_notes_event_filter, {"done", "left", "right", "del1", "del2", "del3"});
	
	for(short i = 0; i < 3; i++) {
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
	store_num_i = 0;
	for(size_t i = 0; i < univ.party.talk_save.size(); i++)
		if(univ.party.talk_save[i].filled)
			store_num_i = i + 1;
	store_page_on = 0;
	if(store_num_i == 0) {
		ASB("Nothing in your talk journal.");
		print_buf();
		return;
	}
	
	set_cursor(sword_curs);
	
	cDialog talkNotes("talk-notes");
	talkNotes.attachClickHandlers(talk_notes_event_filter, {"done", "left", "right", "del"});
	
	put_talk(talkNotes);
	if(store_num_i == 1) {
		talkNotes["left"].hide();
		talkNotes["right"].hide();
	}
	
	talkNotes.run();
}

static bool journal_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
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
	return true;
}

static void fill_journal(cDialog& me) {
	for(int i = 0; i < 3; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if((long)univ.party.journal.size() > i + (store_page_on * 3)) {
			me["str" + n].setText(univ.party.journal[i].the_str);
			me["day" + n].setText("Day: " + std::to_string(univ.party.journal[i].day));
		}
		else{
			me["str" + n].setText("");
			me["day" + n].setText("");
		}
	}
}

void journal() {
	if(univ.party.journal.empty()) {
		ASB("Nothing in your events journal.");
		print_buf();
		return;
	}
	
	store_num_i = 0;
	store_num_i = univ.party.journal.size();
	store_page_on = 0;
	
	set_cursor(sword_curs);
	
	cDialog journal("event-journal");
	journal.attachClickHandlers(journal_event_filter, {"done", "left", "right"});
	
	fill_journal(journal);
	if(store_num_i <= 3) {
		journal["left"].hide();
		journal["right"].hide();
	}
	
	journal.run();
}

void add_to_journal(short event) {
	if(univ.party.add_to_journal(univ.scenario.journal_strs[event], univ.party.calc_day()))
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
	if(!get_bool_pref("ShowInstantHelp", true) && !help_forced)
		return;
	if(get_iarray_pref_contains("ReceivedHelp", help1))
		return;
	append_iarray_pref("ReceivedHelp", help1);
	append_iarray_pref("ReceivedHelp", help2);
	str1 = get_str("help",help1);
	if(help2 > 0)
		str2 = get_str("help",help2);
	cStrDlog display_strings(str1,str2,"Instant Help",24,PIC_DLOG, parent);
	display_strings.setSound(57);
	display_strings.show();
}

void give_help(short help1, short help2) {
	give_help(help1, help2, nullptr);
}

void give_help(short help1, short help2, cDialog& parent) {
	give_help(help1, help2, &parent);
}

void put_quest_info(short which_i) {
	cQuest& quest = univ.scenario.quests[which_i];
	cDialog quest_dlg("quest-info");
	quest_dlg["name"].setText(quest.name);
	quest_dlg["descr"].setText(quest.descr);
	int start = univ.party.active_quests[which_i].start;
	quest_dlg["start"].setText("Day " + std::to_string(start));
	if(quest.deadline > 0)
		quest_dlg["chop"].setText("Day " + std::to_string(quest.deadline + (quest.flags % 10) * start));
	else quest_dlg["chop"].setText("None");
	if(quest.gold > 0)
		quest_dlg["pay"].setText(std::to_string(quest.gold) + " gold");
	else quest_dlg["pay"].setText("Unknown");
	quest_dlg["done"].attachClickHandler(std::bind(&cDialog::toast, &quest_dlg, false));
	quest_dlg.run();
}

void put_spec_item_info (short which_i) {
	cStrDlog display_strings(univ.scenario.special_items[which_i].descr,"",
							 univ.scenario.special_items[which_i].name,univ.scenario.intro_pic,PIC_SCEN);
	display_strings.setSound(57);
	display_strings.show();
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
	}
	if(univ.party.record(type, str1, location))
		give_help(58,0,&me);
	univ.party.record(type, str2, location);
}

