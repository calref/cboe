
#include "pc.graphics.h"
#include "pc.global.h"
#include "classes.h"
#include "pc.editors.h"
#include "graphtool.h"
#include "dialog.hpp"
#include "control.hpp"
#include "button.hpp"
#include "dlogutil.hpp"
#include "winutil.h"
#include <boost/lexical_cast.hpp>

/*
 * These three are not declared in any included header.
 * Instead they are declared in pc.actions.h, which is not
 * included here because it contains additional functions that
 * should not be available to the game (which also includes
 * this file).
 *
 * For the game's purposes, these are declared in
 * boe.infodlg.h and boe.party.h.
 */
void display_pc(short pc_num,short mode,cDialog* parent);
void display_alchemy(bool allowEdit);
bool spend_xp(short pc_num, short mode, cDialog* parent);
// TODO: There's probably a more logical way of arranging this

/* Adventure globals */
//extern party_record_type party;
//extern outdoor_record_type outdoors[2][2];
//extern current_town_type c_town;
//extern big_tr_type t_d;
//extern town_item_list	t_i;
//extern unsigned char out[96][96],out_e[96][96];
//extern setup_save_type setup_save;
//extern stored_items_list_type stored_items[3];
extern cUniverse univ;

extern short store_flags[3];

extern short current_active_pc;

extern sf::RenderWindow mainPtr;
extern rectangle d_rects[80];
extern short d_rect_index[80];

extern bool diff_depth_ok,current_file_has_maps;
bool choice_active[6];

extern short which_pc_displayed;
cPlayer *store_pc;
sf::Texture button_num_gworld;

std::map<eSkill,short> skill_cost = {
	{eSkill::STRENGTH,3}, {eSkill::DEXTERITY,3}, {eSkill::INTELLIGENCE,3},
	{eSkill::EDGED_WEAPONS,2}, {eSkill::BASHING_WEAPONS,2}, {eSkill::POLE_WEAPONS,2},
	{eSkill::THROWN_MISSILES,1}, {eSkill::ARCHERY,2}, {eSkill::DEFENSE,2},
	{eSkill::MAGE_SPELLS,6}, {eSkill::PRIEST_SPELLS,5}, {eSkill::MAGE_LORE,1},
	{eSkill::ALCHEMY,2}, {eSkill::ITEM_LORE,4}, {eSkill::DISARM_TRAPS,2},
	{eSkill::LOCKPICKING,1}, {eSkill::ASSASSINATION,4}, {eSkill::POISON,2},
	{eSkill::LUCK,5},
};
std::map<eSkill,short> skill_max = {
	{eSkill::STRENGTH,20}, {eSkill::DEXTERITY,20}, {eSkill::INTELLIGENCE,20},
	{eSkill::EDGED_WEAPONS,20}, {eSkill::BASHING_WEAPONS,20}, {eSkill::POLE_WEAPONS,20},
	{eSkill::THROWN_MISSILES,20}, {eSkill::ARCHERY,20}, {eSkill::DEFENSE,20},
	{eSkill::MAGE_SPELLS,7}, {eSkill::PRIEST_SPELLS,7}, {eSkill::MAGE_LORE,20},
	{eSkill::ALCHEMY,20}, {eSkill::ITEM_LORE,10}, {eSkill::DISARM_TRAPS,20},
	{eSkill::LOCKPICKING,20}, {eSkill::ASSASSINATION,20}, {eSkill::POISON,20},
	{eSkill::LUCK,20},
};
std::map<eSkill,short> skill_g_cost = {
	{eSkill::STRENGTH,50}, {eSkill::DEXTERITY,503}, {eSkill::INTELLIGENCE,50},
	{eSkill::EDGED_WEAPONS,40}, {eSkill::BASHING_WEAPONS,40}, {eSkill::POLE_WEAPONS,40},
	{eSkill::THROWN_MISSILES,30}, {eSkill::ARCHERY,50}, {eSkill::DEFENSE,40},
	{eSkill::MAGE_SPELLS,250}, {eSkill::PRIEST_SPELLS,250}, {eSkill::MAGE_LORE,25},
	{eSkill::ALCHEMY,100}, {eSkill::ITEM_LORE,200}, {eSkill::DISARM_TRAPS,30},
	{eSkill::LOCKPICKING,20}, {eSkill::ASSASSINATION,100}, {eSkill::POISON,80},
	{eSkill::LUCK,0},
};
// The index here is the skill's level, not the skill itself; thus 20 is the max index since no spell can go above 20.
short skill_bonus[21] = {
	-3,-3,-2,-1,0,0,1,1,1,2,
	2,2,3,3,3,3,4,4,4,5,5};

// These are the IDs used to refer to the skills in the dialog
const char* skill_ids[19] = {
	"str","dex","int",
	"edge","bash","pole","thrown","bow","def",
	"mage","priest","lore","alch","item",
	"trap","lock","assassin","poison","luck"
};

// Variables for spending xp
bool talk_done = false;
long val_for_text;
bool keep_change = false;
short store_h,store_sp,i,store_skp;
long store_g;
short store_train_mode,store_train_pc;

static bool select_pc_event_filter (cDialog& me, std::string item_hit, eKeyMod) {
	me.toast(true);
	if(item_hit != "cancel") {
		short which_pc = item_hit[item_hit.length() - 1] - '1';
		me.setResult<short>(which_pc);
	} else me.setResult<short>(6);
	return true;
}

//active_only;  // 0 - no  1 - yes   2 - disarm trap
short char_select_pc(short active_only,short free_inv_only,const char *title) {
	short item_hit,i;
	
	make_cursor_sword();
	
	cDialog selectPc("select-pc");
	selectPc.attachClickHandlers(select_pc_event_filter, {"cancel", "pick1", "pick2", "pick3", "pick4", "pick5", "pick6"});
	
	selectPc["title"].setText(title);
	
	for(i = 0; i < 6; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if(univ.party[i].main_status == eMainStatus::ABSENT ||
		   (active_only && univ.party[i].main_status > eMainStatus::ALIVE) ||
		   (free_inv_only == 1 && pc_has_space(i) == 24) || univ.party[i].main_status == eMainStatus::FLED) {
			selectPc["pick" + n].hide();
		}
		// TODO: Wouldn't this lead to blank name fields for non-active characters if those characters are allowed?
		if(univ.party[i].main_status != eMainStatus::ABSENT) {
			selectPc["pc" + n].setText(univ.party[i].name);
		}
		else selectPc["pc" + n].hide();
	}
	
	selectPc.run();
	item_hit = selectPc.getResult<short>();
	
	return item_hit;
}

//active_only;  // 0 - no  1 - yes   2 - disarm trap
short select_pc(short active_only,short free_inv_only) {
	if(active_only == 2)
		return char_select_pc(active_only,free_inv_only,"Trap! Who will disarm?");
	else return char_select_pc(active_only,free_inv_only,"Select a character:");
}

static short party_total_level() {
	short i,j = 0;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			j += univ.party[i].level;
	return j;
}

static void put_pc_spells(cDialog& me, const short store_trait_mode) {
	short i;
	
	for(i = 0; i < 62; i++) {
		std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
		cLed& cur = dynamic_cast<cLed&>(me[id]);
		if(((store_trait_mode == 0) && univ.party[which_pc_displayed].mage_spells[i]) ||
			((store_trait_mode == 1) && univ.party[which_pc_displayed].priest_spells[i]))
			cur.setState(led_red);
		else cur.setState(led_off);
	}
	
	me["who"].setText(univ.party[which_pc_displayed].name.c_str());
}

static bool display_pc_event_filter(cDialog& me, std::string item_hit, const short trait_mode) {
	short pc_num;
	
	pc_num = which_pc_displayed;
	if(item_hit == "done") {
		me.toast(true);
	} else if(item_hit == "left") {
		do {
			pc_num = (pc_num == 0) ? 5 : pc_num - 1;
		} while(univ.party[pc_num].main_status == eMainStatus::ABSENT);
		which_pc_displayed = pc_num;
		put_pc_spells(me, trait_mode);
	} else if(item_hit == "right") {
		do {
			pc_num = (pc_num == 5) ? 0 : pc_num + 1;
		} while(univ.party[pc_num].main_status == eMainStatus::ABSENT);
		which_pc_displayed = pc_num;
		put_pc_spells(me, trait_mode);
	}
	return true;
}

void display_pc(short pc_num,short mode, cDialog* parent) {
	using namespace std::placeholders;
	short i;
	std::string label_str;
	
	if(univ.party[pc_num].main_status == eMainStatus::ABSENT) {
		for(pc_num = 0; pc_num < 6; pc_num++)
			if(univ.party[pc_num].main_status == eMainStatus::ALIVE)
				break;
	}
	which_pc_displayed = pc_num;
	
	make_cursor_sword();
	
	cDialog pcInfo("pc-spell-info", parent);
	pcInfo.attachClickHandlers(std::bind(display_pc_event_filter, _1, _2, mode),{"done","left","right"});
	
	for(i = 0; i < 62; i++) {
		std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
		label_str = get_str("magic-names", i + (mode == 0 ? 1 : 101));
		pcInfo[id].setText(label_str);
	}
	put_pc_spells(pcInfo, mode);
	
	dynamic_cast<cPict&>(pcInfo["pic"]).setPict(14 + mode,PIC_DLOG);
	
	pcInfo.run();
}

static void display_traits_graphics(cDialog& me) {
	short i,store;
	
	std::string race = "race" + boost::lexical_cast<std::string>(int(store_pc->race) + 1);
	dynamic_cast<cLedGroup&>(me["race"]).setSelected(race);
	for(i = 0; i < 10; i++) {
		std::string id = "good" + boost::lexical_cast<std::string>(i + 1);
		eTrait trait = eTrait(i);
		dynamic_cast<cLed&>(me[id]).setState((store_pc->traits[trait] > 0) ? led_red : led_off);
	}
	for(i = 0; i < 5; i++) {
		// TODO: Pacifist
		std::string id = "bad" + boost::lexical_cast<std::string>(i + 1);
		eTrait trait = eTrait(i + 10);
		dynamic_cast<cLed&>(me[id]).setState((store_pc->traits[trait] > 0) ? led_red : led_off);
	}
	
	store = store_pc->get_tnl();
	me["xp"].setTextToNum(store);
}

static bool pick_race_select_led(cDialog& me, std::string item_hit, bool losing, const short store_trait_mode) {
	int abil_str = 0;
	cPlayer *pc;
	
	pc = store_pc;
	if(item_hit == "race") {
		item_hit = dynamic_cast<cLedGroup&>(me["race"]).getSelected();
		eRace race;
		switch(item_hit[4] - '1') {
			case 0: race = eRace::HUMAN; break;
			case 1: race = eRace::NEPHIL; break;
			case 2: race = eRace::SLITH; break;
			case 3: race = eRace::VAHNATAI; break;
		}
		if(store_trait_mode == 0)
			pc->race = race;
		display_traits_graphics(me);
		abil_str = 34 + int(race) * 2;
	} else if(item_hit.substr(0,3) == "bad") {
		int hit = item_hit[3] - '1';
		eTrait trait = eTrait(hit + 10);
		if(store_trait_mode != 1)
			pc->traits[trait] = !pc->traits[trait];
		display_traits_graphics(me);
		abil_str = 22 + hit * 2;
	} else if(item_hit.substr(0,4) == "good") {
		int hit = item_hit[4] - '1';
		eTrait trait = eTrait(hit);
		if(store_trait_mode != 1)
			pc->traits[trait] = !pc->traits[trait];
		display_traits_graphics(me);
		abil_str = 2 + hit * 2;
	}
	if(abil_str > 0)
		me["info"].setText(get_str("traits", abil_str));
	return store_trait_mode == 0;
}

//mode; // 0 - edit  1 - just display  2 - can't change race
void pick_race_abil(cPlayer *pc,short mode) {
	using namespace std::placeholders;
	static const char*const start_str1 = "Click on button by name for description.";
	static const char*const start_str2 = "Click on advantage button to add/remove.";
	
	store_pc = pc;
	make_cursor_sword();
	
	cDialog pickAbil("pick-race-abil");
	pickAbil["done"].attachClickHandler(std::bind(&cDialog::toast, &pickAbil, true));
	auto led_selector = std::bind(pick_race_select_led, _1, _2, _3, mode);
	pickAbil.attachFocusHandlers(led_selector, {"race", "bad1", "bad2", "bad3", "bad4", "bad5"});
	pickAbil.attachFocusHandlers(led_selector, {"good1", "good2", "good3", "good4", "good5"});
	pickAbil.attachFocusHandlers(led_selector, {"good6", "good7", "good8", "good9", "good10"});
	
	display_traits_graphics(pickAbil);
	if(mode == 1)
		pickAbil["info"].setText(start_str1);
	else pickAbil["info"].setText(start_str2);
	
	pickAbil.run();
}

short alch_difficulty[20] = {
	1,1,1,3,3,
	4,5,5,7,9,
	9,10,12,12,9,
	14,19,10,16,20
};

void display_alchemy(bool allowEdit) {
	short i;
	
	make_cursor_sword();
	
	cChoiceDlog showAlch("pc-alchemy-info", {"done"});
	
	for(i = 0; i < 20; i++) {
		std::string id = "potion" + boost::lexical_cast<std::string>(i + 1);
		std::string name = get_str("magic-names", i + 200) + " (";
		name += std::to_string(alch_difficulty[i]);
		name += ')';
		showAlch->addLabelFor(id, name, LABEL_LEFT, 83, true);
		if(!allowEdit)
			showAlch->getControl(id).attachClickHandler(&cLed::noAction);
		cLed& led = dynamic_cast<cLed&>(showAlch->getControl(id));
		if(univ.party.alchemy[i] > 0)
			led.setState(led_red);
		else led.setState(led_off);
	}
	
	showAlch.show();
	if(!allowEdit) return;
	
	for(i = 0; i < 20; i++) {
		std::string id = "potion" + boost::lexical_cast<std::string>(i + 1);
		cLed& led = dynamic_cast<cLed&>(showAlch->getControl(id));
		if(led.getState() == led_red) univ.party.alchemy[i] = true;
		else univ.party.alchemy[i] = false;
	}
}

// TODO: This dialog needs some kind of context system really badly to avoid the rampant globals
// MARK: Start spend XP dialog
static void do_xp_keep(short pc_num,short mode,std::map<eSkill,short>& store_skills) {
	for(i = 0; i < 19; i++) {
		eSkill skill = eSkill(i);
		univ.party[pc_num].skills[skill] = store_skills[skill];
	}
	univ.party[pc_num].cur_health += store_h - univ.party[pc_num].max_health;
	univ.party[pc_num].max_health = store_h;
	univ.party[pc_num].cur_sp += store_sp - univ.party[pc_num].max_sp;
	univ.party[pc_num].max_sp = store_sp;
	if(mode == 1)
		univ.party.gold = store_g;
	univ.party[pc_num].skill_pts = store_skp;
	
}

static void draw_xp_skills(cDialog& me,std::map<eSkill,short>& store_skills) {
	short i;
	// TODO: Wouldn't it make more sense for it to be red when you can't buy the skill rather than red when you can?
	for(i = 0; i < 19; i++) {
		cControl& cur = me[skill_ids[i]];
		eSkill skill = eSkill(i);
		if((store_skp >= skill_cost[skill]) && (store_g >= skill_g_cost[skill]))
			cur.setColour(sf::Color::Red);
		else cur.setColour(me.getDefTextClr());
		cur.setTextToNum(store_skills[skill]);
	}
	
	cControl& sp = me["sp"];
	cControl& hp = me["hp"];
	if((store_skp >= 1) && (store_g >= 10))
		hp.setColour(sf::Color::Red);
	else hp.setColour(me.getDefTextClr());
	hp.setTextToNum(store_h);
	if((store_skp >= 1) && (store_g >= 15))
		sp.setColour(sf::Color::Red);
	else sp.setColour(me.getDefTextClr());
	sp.setTextToNum(store_sp);
}

static void update_gold_skills(cDialog& me) {
	me["gold"].setTextToNum(((store_train_mode == 0) ? 0 : store_g));
	me["skp"].setTextToNum(store_skp);
}


static void do_xp_draw(cDialog& me,std::map<eSkill,short>& store_skills) {
	
	char get_text[256];
	short mode,pc_num;
	
	mode = store_train_mode;
	pc_num = store_train_pc;
	if(mode == 0) {
		if(univ.party[pc_num].main_status == eMainStatus::ALIVE)
			sprintf((char *) get_text, "%s",(char *) univ.party[pc_num].name.c_str());
		else sprintf((char *) get_text, "New PC");
	}
	else sprintf((char *) get_text, "%s",(char *) univ.party[pc_num].name.c_str());
	
	me["recipient"].setText(get_text);
	
	for(i = 0; i < 19; i++) {
		eSkill skill = eSkill(i);
		store_skills[skill] = univ.party[pc_num].skills[skill];
	}
	store_h = univ.party[pc_num].max_health;
	store_sp = univ.party[pc_num].max_sp;
	store_g = (mode == 0) ? 20000 : univ.party.gold;
	store_skp = univ.party[pc_num].skill_pts;
	
	draw_xp_skills(me,store_skills);
	
	
	update_gold_skills(me);
}

static bool spend_xp_navigate_filter(cDialog& me, std::string item_hit,std::map<eSkill,short>& store_skills) {
	short mode,pc_num;
	bool talk_done = false;
	
	mode = store_train_mode;
	pc_num = store_train_pc;
	
	if(item_hit == "cancel") {
		// TODO: Um, I'm pretty sure this can never happen.
		if(mode == 0 && univ.party[pc_num].main_status < eMainStatus::ABSENT)
			univ.party[pc_num].main_status = eMainStatus::ABSENT;
		me.setResult(false);
		talk_done = true;
	} else if(item_hit == "help") {
		univ.party.help_received[10] = 0;
//		give_help(210,11,me);
	} else if(item_hit == "keep") {
		do_xp_keep(pc_num,mode,store_skills);
		me.setResult(true);
		talk_done = true;
	} else if(item_hit == "left") {
		// TODO: Try not forcing a commit when using the arrows?
		if(mode != 0) {
			do_xp_keep(pc_num,mode,store_skills);
			do {
				pc_num = (pc_num == 0) ? 5 : pc_num - 1;
			} while(univ.party[pc_num].main_status != eMainStatus::ALIVE);
			store_train_pc = pc_num;
			do_xp_draw(me,store_skills);
		} else
			beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
	} else if(item_hit == "right") {
		// TODO: If they don't work in mode 0, why are they visible?
		if(mode != 0) {
			do_xp_keep(pc_num,mode,store_skills);
			do {
				pc_num = (pc_num == 5) ? 0 : pc_num + 1;
			} while(univ.party[pc_num].main_status != eMainStatus::ALIVE);
			store_train_pc = pc_num;
			do_xp_draw(me,store_skills);
		} else
			beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
	}
	store_train_pc = pc_num;
	if(talk_done) {
		me.toast(item_hit == "keep");
	}
	return true;
}

static bool spend_xp_event_filter(cDialog& me, std::string item_hit, eKeyMod mods,std::map<eSkill,short>& store_skills) {
	short mode = store_train_mode, pc_num = store_train_pc;
	if(item_hit.substr(0,2) == "hp") {
		if(mod_contains(mods, mod_alt)) {
			cStrDlog aboutHP(get_str("help",63),"","About Health",24,PIC_DLOG,&me);
			aboutHP.setSound(57);
			aboutHP.show();
		} else if(((store_h >= 250) && (item_hit[3] == 'p')) ||
				   ((store_h == univ.party[pc_num].max_health) && (item_hit[3] == 'm') && (mode == 1)) ||
				   ((store_h == 6) && (item_hit[3] == 'm') && (mode == 0)))
			beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
		else if(item_hit == "hp-m") {
			store_g += 10;
			store_h -= 2;
			store_skp += 1;
		}
		else {
			if((store_g < 10) || (store_skp < 1)) {
//				if(store_g < 10)
//					give_help(24,0,me);
//				else give_help(25,0,me);
			}
			else {
				store_g -= 10;
				store_h += 2;
				store_skp -= 1;
			}
		}
		
		update_gold_skills(me);
		me["hp"].setTextToNum(store_h);
		draw_xp_skills(me,store_skills);
		
	} else if(item_hit.substr(0,2) == "sp") {
		if(mod_contains(mods, mod_alt)) {
			cStrDlog aboutSP(get_str("help",64),"","About Spell Points",24,PIC_DLOG,&me);
			aboutSP.setSound(57);
			aboutSP.show();
		} else if(((store_sp >= 150) && (item_hit[3] == 'p')) ||
				   ((store_sp == univ.party[pc_num].max_sp) && (item_hit[3] == 'm') && (mode == 1)) ||
				   ((store_sp == 0) && (item_hit[3] == 'm') && (mode == 0)))
			beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
		else if(item_hit == "sp-m") {
			store_g += 15;
			store_sp -= 1;
			store_skp += 1;
		}
		else {
			if((store_g < 15) || (store_skp < 1)) {
//				if(store_g < 15)
//					give_help(24,0,me);
//				else give_help(25,0,me);
			}
			else {
				store_sp += 1;
				store_g -= 15;
				store_skp -= 1;
			}
		}
		
		update_gold_skills(me);
		me["sp"].setTextToNum(store_sp);
		draw_xp_skills(me,store_skills);
	} else {
		eSkill which_skill = eSkill::INVALID;
		for(int i = 0; i < 19; i++) {
			size_t n = strlen(skill_ids[i]);
			if(item_hit.length() < n + 2) continue;
			if(item_hit.substr(0, item_hit.length() - 2) == skill_ids[i]) {
				which_skill = eSkill(i);
				break;
			}
		}
		if(which_skill == eSkill::INVALID) return true;
/*		if(mod_contains(mods, mod_alt)) display_skills(which_skill,&me);
		else */{
			char dir = item_hit[item_hit.length() - 1];
			
			// TODO: This is a game event, so it should have a game sound, not a system alert.
			if(store_skills[which_skill] >= skill_max[which_skill] && dir == 'p')
				beep();
			else if(store_skills[which_skill] == univ.party[pc_num].skills[which_skill] && dir == 'm' && mode == 1)
				beep();
			else if(store_skills[which_skill] == 0 && dir == 'm' && mode == 0 &&
					which_skill != eSkill::STRENGTH && which_skill != eSkill::DEXTERITY && which_skill != eSkill::INTELLIGENCE)
				beep();
			else if(store_skills[which_skill] == 1 && dir == 'm' && mode == 0 &&
					(which_skill == eSkill::STRENGTH || which_skill == eSkill::DEXTERITY || which_skill == eSkill::INTELLIGENCE))
				beep();
			else {
				if(dir == 'm') {
					store_g += skill_g_cost[which_skill];
					store_skills[which_skill] -= 1;
					store_skp += skill_cost[which_skill];
				}
				else {
					if((store_g < skill_g_cost[which_skill]) || (store_skp < skill_cost[which_skill])) {
//						if(store_g < skill_g_cost[which_skill])
//							give_help(24,0,me);
//						else give_help(25,0,me);
					}
					else {
						store_skills[which_skill] += 1;
						store_g -= skill_g_cost[which_skill];
						store_skp -= skill_cost[which_skill];
					}
				}
				
				update_gold_skills(me);
				me[skill_ids[int(which_skill)]].setTextToNum(store_skills[which_skill]);
				draw_xp_skills(me,store_skills);
			}
		}
	}
	return true;
}

//short mode; // 0 - create  1 - train
// returns 1 if cancelled
bool spend_xp(short pc_num, short mode, cDialog* parent) {
	using namespace std::placeholders;
	store_train_pc = pc_num;
	store_train_mode = mode;
	
	make_cursor_sword();
	
	std::map<eSkill,short> skills = univ.party[pc_num].skills;
	
	cDialog xpDlog("spend-xp",parent);
	xpDlog.addLabelFor("hp","Health (1/10)",LABEL_LEFT,75,true);
	xpDlog.addLabelFor("sp","Spell Pts. (1/15)",LABEL_LEFT,75,true);
	auto spend_xp_filter = std::bind(spend_xp_event_filter,_1,_2,_3,std::ref(skills));
	std::string minus = "-m", plus = "-p";
	for(i = 54; i < 73; i++) {
		std::ostringstream sout;
		eSkill skill = eSkill(i - 54);
		sout << get_str("skills",1 + 2 * (i - 54)) << ' ' << '(';
		sout << skill_cost[skill] << '/' << skill_g_cost[skill] << ')';
		xpDlog.addLabelFor(skill_ids[i - 54],sout.str(),LABEL_LEFT,(i < 63) ? 75 : 69,true);
		xpDlog[skill_ids[i - 54] + minus].attachClickHandler(spend_xp_filter);
		xpDlog[skill_ids[i - 54] + plus].attachClickHandler(spend_xp_filter);
	}
	do_xp_draw(xpDlog,skills);
	
	xpDlog.attachClickHandlers(std::bind(spend_xp_navigate_filter,_1,_2,std::ref(skills)),{"keep","cancel","left","right","help"});
	xpDlog.attachClickHandlers(spend_xp_filter,{"sp-m","sp-p","hp-m","hp-p"});
	
	if(univ.party.help_received[10] == 0) {
		// TODO: Is an initial draw even needed?
//		cd_initial_draw(1010);
//		give_help(10,11,xpDlog);
	}
	
	xpDlog.run();
	
	return xpDlog.getResult<bool>();
}
