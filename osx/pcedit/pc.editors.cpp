
#include "pc.graphics.h"
#include "pc.global.h"
#include "classes.h"
#include "pc.editors.h"
#include "graphtool.h"
#include "dialog.h"
#include "control.h"
#include "button.h"
#include "dlogutil.h"
#include <boost/lexical_cast.hpp>

extern std::string get_str(std::string, short);

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
extern RECT d_rects[80];
extern short d_rect_index[80];

extern bool diff_depth_ok,current_file_has_maps;
bool choice_active[6];

extern short store_trait_mode;
extern short which_pc_displayed;
cPlayer *store_pc;
sf::Texture button_num_gworld;

short skill_cost[20] = {
	3,3,3,2,2,2, 1,2,2,6,
	5, 1,2,4,2,1, 4,2,5,0};
short skill_max[20] = {
	20,20,20,20,20,20,20,20,20,7,
	7,20,20,10,20,20,20,20,20};
short skill_g_cost[20] = {
	50,50,50,40,40,40,30,50,40,250,
	250,25,100,200,30,20,100,80,0,0};
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
short store_skills[20],store_h,store_sp,i,store_skp,which_skill;
long store_g;
short store_train_mode,store_train_pc;

static bool select_pc_event_filter (cDialog& me, std::string item_hit, eKeyMod mods)
{
	me.toast();
	if(item_hit != "cancel") {
		short which_pc = item_hit[item_hit.length() - 1] - '1';
		me.setResult<short>(which_pc);
	} else me.setResult<short>(6);
	return true;
}

short char_select_pc(short active_only,short free_inv_only,const char *title)
//active_only;  // 0 - no  1 - yes   2 - disarm trap
{
	short item_hit,i;
	
	make_cursor_sword();
	
	cDialog selectPc("select-pc.xml");
	selectPc.attachClickHandlers(select_pc_event_filter, {"cancel", "pick1", "pick2", "pick3", "pick4", "pick5", "pick6"});
	
	selectPc["title"].setText(title);
	
	for (i = 0; i < 6; i++) {
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

short select_pc(short active_only,short free_inv_only)
//active_only;  // 0 - no  1 - yes   2 - disarm trap
{
	if (active_only == 2)
		return char_select_pc(active_only,free_inv_only,"Trap! Who will disarm?");
	else return char_select_pc(active_only,free_inv_only,"Select a character:");
}

static short party_total_level()
{
	short i,j = 0;
	
	for (i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			j += univ.party[i].level;
	return j;
}

static void put_pc_spells(cDialog& me)
{
	short i;
	
	for (i = 0; i < 62; i++) {
		std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
		cLed& cur = dynamic_cast<cLed&>(me[id]);
		if (((store_trait_mode == 0) && univ.party[which_pc_displayed].mage_spells[i]) ||
			((store_trait_mode == 1) && univ.party[which_pc_displayed].priest_spells[i]))
			cur.setState(led_red);
		else cur.setState(led_off);
	}
	
	me["who"].setText(univ.party[which_pc_displayed].name.c_str());
}

static bool display_pc_event_filter(cDialog& me, std::string item_hit, eKeyMod mods)
{
	short pc_num;
	
	pc_num = which_pc_displayed;
	if(item_hit == "done") {
		me.toast();
	} else if(item_hit == "left") {
		do {
			pc_num = (pc_num == 0) ? 5 : pc_num - 1;
		} while(univ.party[pc_num].main_status == eMainStatus::ABSENT);
		which_pc_displayed = pc_num;
		put_pc_spells(me);
	} else if(item_hit == "right") {
		do {
			pc_num = (pc_num == 5) ? 0 : pc_num + 1;
		} while(univ.party[pc_num].main_status == eMainStatus::ABSENT);
		which_pc_displayed = pc_num;
		put_pc_spells(me);
	}
	return true;
}

void display_pc(short pc_num,short mode,cDialog* parent)
{
	short i;
	std::string label_str;
	
	if(univ.party[pc_num].main_status == eMainStatus::ABSENT) {
		for (pc_num = 0; pc_num < 6; pc_num++)
			if(univ.party[pc_num].main_status == eMainStatus::ALIVE)
				break;
	}
	which_pc_displayed = pc_num;
	store_trait_mode = mode;
	
	make_cursor_sword();
	
	cDialog pcInfo("pc-spell-info.xml", parent);
	pcInfo.attachClickHandlers(display_pc_event_filter,{"done","left","right"});
	
	for (i = 0; i < 62; i++) {
		std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
		label_str = get_str((mode == 0) ? "mage-spells" : "priest-spells",i * 2 + 1);
		pcInfo[id].setText(label_str);
	}
	put_pc_spells(pcInfo);
	
	dynamic_cast<cPict&>(pcInfo["pic"]).setPict(14 + mode,PIC_DLOG);
	
	pcInfo.run();
}

static void display_traits_graphics(cDialog& me)
{
	short i,store;
	
	std::string race = "race" + boost::lexical_cast<std::string>(int(store_pc->race) + 1);
	dynamic_cast<cLedGroup&>(me["race"]).setSelected(race);
	for (i = 0; i < 10; i++) {
		std::string id = "good" + boost::lexical_cast<std::string>(i + 1);
		dynamic_cast<cLed&>(me[id]).setState((store_pc->traits[i] > 0) ? led_red : led_off);
	}
	for (i = 0; i < 5; i++) {
		std::string id = "bad" + boost::lexical_cast<std::string>(i + 1);
		dynamic_cast<cLed&>(me[id]).setState((store_pc->traits[10 + i] > 0) ? led_red : led_off);
	}
	
	store = store_pc->get_tnl();
	me["xp"].setTextToNum(store);
}

static bool pick_race_abil_event_filter(cDialog& me, std::string item_hit, eKeyMod mods) {
	me.toast();
	return true;
}

static bool pick_race_select_led(cDialog& me, std::string item_hit, bool losing)
{
	std::string abil_str;
	cPlayer *pc;
	
	pc = store_pc;
	if(item_hit == "race") {
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
		abil_str = get_str("traits",16 + int(race));
		me["info"].setText(abil_str);
	} else if(item_hit.substr(0,3) == "bad") {
		int hit = item_hit[3] - '1';
		if(store_trait_mode != 1)
			pc->traits[hit + 10] = (pc->traits[hit + 10] == true) ? false : true;
		display_traits_graphics(me);
		abil_str = get_str("traits",hit + 11);
		me["info"].setText(abil_str);
	} else if(item_hit.substr(0,4) == "good") {
		int hit = item_hit[4] - '1';
		if(store_trait_mode != 1)
			pc->traits[hit] = (pc->traits[hit] == true) ? false : true;
		display_traits_graphics(me);
		abil_str = get_str("traits",hit + 1);
		me["info"].setText(abil_str);
	}
	return store_trait_mode == 0;
}

void pick_race_abil(cPlayer *pc,short mode,cDialog* parent)
//mode; // 0 - edit  1 - just display  2 - can't change race
{
	static const char*const start_str1 = "Click on button by name for description.";
	static const char*const start_str2 = "Click on advantage button to add/remove.";
	
	store_trait_mode = mode;
	store_pc = pc;
	make_cursor_sword();
	
	cDialog pickAbil("pick-race-abil.xml");
	pickAbil["done"].attachClickHandler(pick_race_abil_event_filter);
	pickAbil.attachFocusHandlers(pick_race_select_led, {"race", "bad1", "bad2", "bad3", "bad4", "bad5"});
	pickAbil.attachFocusHandlers(pick_race_select_led, {"good1", "good2", "good3", "good4", "good5"});
	pickAbil.attachFocusHandlers(pick_race_select_led, {"good6", "good7", "good8", "good9", "good10"});
	
	display_traits_graphics(pickAbil);
	if (mode == 1)
		pickAbil["info"].setText(start_str1);
	else pickAbil["info"].setText(start_str2);
	
	pickAbil.run();
}

const char* alch_names[20] = {
	"Weak Curing Potion (1)","Weak Healing Potion (1)","Weak Poison (1)",
	"Weak Speed Potion (3)","Medium Poison (3)",
	"Medium Heal Potion (4)","Strong Curing (5)","Medium Speed Potion (5)",
	"Graymold Salve (7)","Weak Energy Potion (9)",
	"Potion of Clarity (9)","Strong Poison (10)","Strong Heal Potion (12)","Killer Poison (12)",
	"Resurrection Balm (9)","Medium Energy Ptn. (14)","Knowledge Brew (19)"	,
	"Strong Strength (10)","Bliss (16)","Strong Power (20)"
};

void display_alchemy(bool allowEdit)
{
	short i;
	
	make_cursor_sword();
	
	cChoiceDlog showAlch("pc-alchemy-info.xml", {"done"});
	
	for(i = 0; i < 20; i++) {
		std::string id = "potion" + boost::lexical_cast<std::string>(i + 1);
		showAlch->addLabelFor(id, alch_names[i], LABEL_LEFT, 83, true);
		if(!allowEdit)
			showAlch->getControl(id).attachClickHandler(&cDialog::noAction);
		cLed& led = dynamic_cast<cLed&>(showAlch->getControl(id));
		if (univ.party.alchemy[i] > 0)
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
static void do_xp_keep(short pc_num,short mode)
{
	for (i = 0; i < 20; i++)
		univ.party[pc_num].skills[i] = store_skills[i];
	univ.party[pc_num].cur_health += store_h - univ.party[pc_num].max_health;
	univ.party[pc_num].max_health = store_h;
	univ.party[pc_num].cur_sp += store_sp - univ.party[pc_num].max_sp;
	univ.party[pc_num].max_sp = store_sp;
	if (mode == 1)
		univ.party.gold = store_g;
	univ.party[pc_num].skill_pts = store_skp;
	
}

static void draw_xp_skills(cDialog& me)
{
	short i;
	// TODO: Wouldn't it make more sense for it to be red when you can't buy the skill rather than red when you can?
	for (i = 0; i < 19; i++) {
		cControl& cur = me[skill_ids[i]];
		if ((store_skp >= skill_cost[i]) && (store_g >= skill_g_cost[i]))
			cur.setColour(sf::Color::Red);
		else cur.setColour(me.getDefTextClr());
		cur.setTextToNum(store_skills[i]);
	}
	
	cControl& sp = me["sp"];
	cControl& hp = me["hp"];
	if ((store_skp >= 1) && (store_g >= 10))
		hp.setColour(sf::Color::Red);
	else hp.setColour(me.getDefTextClr());
	hp.setTextToNum(store_h);
	if ((store_skp >= 1) && (store_g >= 15))
		sp.setColour(sf::Color::Red);
	else sp.setColour(me.getDefTextClr());
	sp.setTextToNum(store_sp);
}

static void update_gold_skills(cDialog& me) {
	me["gold"].setTextToNum(((store_train_mode == 0) ? 0 : store_g));
	me["skp"].setTextToNum(store_skp);
}


static void do_xp_draw(cDialog& me)

{
	
	char get_text[256];
	short mode,pc_num;
	
	mode = store_train_mode;
	pc_num = store_train_pc;
	if (mode == 0) {
		if(univ.party[pc_num].main_status == eMainStatus::ALIVE)
			sprintf((char *) get_text, "%s",(char *) univ.party[pc_num].name.c_str());
		else sprintf((char *) get_text, "New PC");
	}
	else sprintf((char *) get_text, "%s",(char *) univ.party[pc_num].name.c_str());
	
	me["recipient"].setText(get_text);
	
	for (i = 0; i < 20; i++)
		store_skills[i] = univ.party[pc_num].skills[i];
	store_h = univ.party[pc_num].max_health;
	store_sp = univ.party[pc_num].max_sp;
	store_g = (mode == 0) ? 20000 : univ.party.gold;
	store_skp = univ.party[pc_num].skill_pts;
	
	draw_xp_skills(me);
	
	
	update_gold_skills(me);
}

static bool spend_xp_navigate_filter(cDialog& me, std::string item_hit, eKeyMod mods)
{
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
		do_xp_keep(pc_num,mode);
		me.setResult(true);
		talk_done = true;
	} else if(item_hit == "left") {
		// TODO: Try not forcing a commit when using the arrows?
		if (mode != 0) {
			do_xp_keep(pc_num,mode);
			do {
				pc_num = (pc_num == 0) ? 5 : pc_num - 1;
			} while (univ.party[pc_num].main_status != eMainStatus::ALIVE);
			store_train_pc = pc_num;
			do_xp_draw(me);
		} // else TODO: Play an error sound here
	} else if(item_hit == "right") {
		// TODO: If they don't work in mode 0, why are they visible?
		if (mode != 0) {
			do_xp_keep(pc_num,mode);
			do {
				pc_num = (pc_num == 5) ? 0 : pc_num + 1;
			} while (univ.party[pc_num].main_status != eMainStatus::ALIVE);
			store_train_pc = pc_num;
			do_xp_draw(me);
		} // else TODO: Play an error sound here
	}
	store_train_pc = pc_num;
	if (talk_done == true) {
		me.toast();
	}
	return true;
}

static bool spend_xp_event_filter(cDialog& me, std::string item_hit, eKeyMod mods) {
	short mode = store_train_mode, pc_num = store_train_pc;
	if(item_hit.substr(0,2) == "hp") {
		if(mod_contains(mods, mod_alt)) {
			cStrDlog aboutHP(get_str("help",63),"","About Health",24,PIC_DLOG,&me);
			aboutHP.setSound(57);
			aboutHP.show();
		} else if (((store_h >= 250) && (item_hit[3] == 'p')) ||
				   ((store_h == univ.party[pc_num].max_health) && (item_hit[3] == 'm') && (mode == 1)) ||
				   ((store_h == 6) && (item_hit[3] == 'm') && (mode == 0)))
			; // TODO: Play an error sound here
		else if(item_hit == "hp-m") {
			store_g += 10;
			store_h -= 2;
			store_skp += 1;
		}
		else {
			if ((store_g < 10) || (store_skp < 1)) {
//				if (store_g < 10)
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
		draw_xp_skills(me);
		
	} else if(item_hit.substr(0,2) == "sp") {
		if(mod_contains(mods, mod_alt)) {
			cStrDlog aboutSP(get_str("help",64),"","About Spell Points",24,PIC_DLOG,&me);
			aboutSP.setSound(57);
			aboutSP.show();
		} else if (((store_sp >= 150) && (item_hit[3] == 'p')) ||
				   ((store_sp == univ.party[pc_num].max_sp) && (item_hit[3] == 'm') && (mode == 1)) ||
				   ((store_sp == 0) && (item_hit[3] == 'm') && (mode == 0)))
			; // TODO: Play an error sound here
		else if(item_hit == "sp-m") {
			store_g += 15;
			store_sp -= 1;
			store_skp += 1;
		}
		else {
			if ((store_g < 15) || (store_skp < 1)) {
//				if (store_g < 15)
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
		draw_xp_skills(me);
	} else {
		for(int i = 0; i < 19; i++) {
			int n = strlen(skill_ids[i]);
			if(item_hit.length() < n + 2) continue;
			if(item_hit.substr(0, item_hit.length() - 2) == skill_ids[i]) {
				which_skill = i;
				break;
			}
		}
/*		if(mod_contains(mods, mod_alt)) display_skills(which_skill,&me);
		else */{
			char dir = item_hit[item_hit.length() - 1];
			
			if (((store_skills[which_skill] >= skill_max[which_skill]) && (dir == 'p')) ||
				((store_skills[which_skill] == univ.party[pc_num].skills[which_skill]) && (dir == 'm') && (mode == 1)) ||
				((store_skills[which_skill] == 0) && (dir == 'm') && (mode == 0) && (which_skill > 2)) ||
				((store_skills[which_skill] == 1) && (dir == 'm') && (mode == 0) && (which_skill <= 2)))
				; // TODO: Play an error sound here
			else {
				if(dir == 'm') {
					store_g += skill_g_cost[which_skill];
					store_skills[which_skill] -= 1;
					store_skp += skill_cost[which_skill];
				}
				else {
					if ((store_g < skill_g_cost[which_skill]) || (store_skp < skill_cost[which_skill])) {
//						if (store_g < skill_g_cost[which_skill])
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
				me[skill_ids[which_skill]].setTextToNum(store_skills[which_skill]);
				draw_xp_skills(me);
			}
		}
	}
	return true;
}

bool spend_xp(short pc_num, short mode, cDialog* parent)
//short mode; // 0 - create  1 - train
// returns 1 if cancelled
{
	store_train_pc = pc_num;
	store_train_mode = mode;
	
	make_cursor_sword();
	
	cDialog xpDlog("spend-xp.xml",parent);
	xpDlog.addLabelFor("hp","Health (1/10)",LABEL_LEFT,75,true);
	xpDlog.addLabelFor("sp","Spell Pts. (1/15)",LABEL_LEFT,75,true);
	std::string minus = "-m", plus = "-p";
	for (i = 54; i < 73; i++) {
		std::ostringstream sout;
		sout << get_str("skills",1 + 2 * (i - 54)) << ' ' << '(';
		sout << skill_cost[i - 54] << '/' << skill_g_cost[i - 54] << ')';
		xpDlog.addLabelFor(skill_ids[i - 54],sout.str(),LABEL_LEFT,(i < 63) ? 75 : 69,true);
		xpDlog[skill_ids[i - 54] + minus].attachClickHandler(spend_xp_event_filter);
		xpDlog[skill_ids[i - 54] + plus].attachClickHandler(spend_xp_event_filter);
	}
	do_xp_draw(xpDlog);
	
	xpDlog.attachClickHandlers(spend_xp_navigate_filter,{"keep","cancel","left","right","help"});
	xpDlog.attachClickHandlers(spend_xp_event_filter,{"sp-m","sp-p","hp-m","hp-p"});
	
	if (univ.party.help_received[10] == 0) {
		// TODO: Is an initial draw even needed?
//		cd_initial_draw(1010);
//		give_help(10,11,xpDlog);
	}
	
	xpDlog.run();
	
	return xpDlog.getResult<bool>();
}
