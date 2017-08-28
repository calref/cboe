
#include "pc.graphics.hpp"
#include "universe.hpp"
#include "pc.editors.hpp"
#include "utility.hpp"
#include "dialog.hpp"
#include "control.hpp"
#include "button.hpp"
#include "pict.hpp"
#include "strdlog.hpp"
#include "choicedlog.hpp"
#include "winutil.hpp"
#include "cursors.hpp"
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
void display_alchemy(bool allowEdit,cDialog* parent);
bool spend_xp(short pc_num, short mode, cDialog* parent);
// TODO: There's probably a more logical way of arranging this

extern void give_help(short help1, short help2);
extern void give_help(short help1, short help2, cDialog& parent);
void display_skills(eSkill skill,cDialog* parent);

extern cUniverse univ;
extern short store_flags[3];
extern short current_active_pc;

extern sf::RenderWindow mainPtr;
extern rectangle d_rects[80];
extern short d_rect_index[80];

extern bool current_file_has_maps;
bool choice_active[6];

extern short which_pc_displayed;
cPlayer *store_pc;
sf::Texture button_num_gworld;

extern std::map<eSkill,short> skill_cost;
extern std::map<eSkill,short> skill_max;
extern std::map<eSkill,short> skill_g_cost;

static void put_pc_spells(cDialog& me, short store_trait_mode) {
	store_trait_mode %= 10;
	
	for(short i = 0; i < 62; i++) {
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
	std::string label_str;
	
	if(univ.party[pc_num].main_status == eMainStatus::ABSENT) {
		for(pc_num = 0; pc_num < 6; pc_num++)
			if(univ.party[pc_num].main_status == eMainStatus::ALIVE)
				break;
	}
	which_pc_displayed = pc_num;
	
	set_cursor(sword_curs);
	
	cDialog pcInfo("pc-spell-info", parent);
	pcInfo.attachClickHandlers(std::bind(display_pc_event_filter, _1, _2, mode),{"done","left","right"});
	
	for(short i = 0; i < 62; i++) {
		std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
		label_str = get_str("magic-names", i + (mode % 10 == 0 ? 1 : 101));
		pcInfo[id].setText(label_str);
		if(mode < 10)
			pcInfo[id].attachClickHandler(&cLed::noAction);
	}
	put_pc_spells(pcInfo, mode);
	
	dynamic_cast<cPict&>(pcInfo["pic"]).setPict(14 + mode,PIC_DLOG);
	
	pcInfo.run();
	
	if(mode >= 10) {
		mode %= 10;
		for(short i = 0; i < 62; i++) {
			std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
			bool set = dynamic_cast<cLed&>(pcInfo[id]).getState() != led_off;
			if(mode == 0) univ.party[pc_num].mage_spells[i] = set;
			else if(mode == 1) univ.party[pc_num].priest_spells[i] = set;
		}
	}
}

static void display_traits_graphics(cDialog& me) {
	short store;
	
	if(store_pc->race <= eRace::VAHNATAI) {
		std::string race = "race" + boost::lexical_cast<std::string>(int(store_pc->race) + 1);
		dynamic_cast<cLedGroup&>(me["race"]).setSelected(race);
	}
	for(short i = 0; i < 10; i++) {
		std::string id = "good" + boost::lexical_cast<std::string>(i + 1);
		eTrait trait = eTrait(i);
		dynamic_cast<cLed&>(me[id]).setState(store_pc->traits[trait] ? led_red : led_off);
	}
	for(short i = 0; i < 7; i++) {
		std::string id = "bad" + boost::lexical_cast<std::string>(i + 1);
		eTrait trait = eTrait(i + 10);
		dynamic_cast<cLed&>(me[id]).setState(store_pc->traits[trait] ? led_red : led_off);
	}
	
	store = store_pc->get_tnl();
	me["xp"].setTextToNum(store);
}

static bool pick_race_select_led(cDialog& me, std::string item_hit, bool, const short store_trait_mode) {
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
		abil_str = 36 + int(race) * 2;
	} else if(item_hit.substr(0,3) == "bad") {
		int hit = item_hit[3] - '1';
		eTrait trait = eTrait(hit + 10);
		if(store_trait_mode != 1)
			pc->traits[trait] = !pc->traits[trait];
		display_traits_graphics(me);
		abil_str = 22 + hit * 2;
	} else if(item_hit.substr(0,4) == "good") {
		int hit = boost::lexical_cast<int>(item_hit.substr(4)) - 1;
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
void pick_race_abil(cPlayer *pc,short mode,cDialog* parent) {
	using namespace std::placeholders;
	static const char*const start_str1 = "Click on button by name for description.";
	static const char*const start_str2 = "Click on advantage button to add/remove.";
	
	store_pc = pc;
	set_cursor(sword_curs);
	
	cDialog pickAbil("pick-race-abil",parent);
	pickAbil["done"].attachClickHandler(std::bind(&cDialog::toast, &pickAbil, true));
	auto led_selector = std::bind(pick_race_select_led, _1, _2, _3, mode);
	pickAbil.attachFocusHandlers(led_selector, {"race", "bad1", "bad2", "bad3", "bad4", "bad5", "bad6", "bad7"});
	pickAbil.attachFocusHandlers(led_selector, {"good1", "good2", "good3", "good4", "good5"});
	pickAbil.attachFocusHandlers(led_selector, {"good6", "good7", "good8", "good9", "good10"});
	
	display_traits_graphics(pickAbil);
	if(mode == 1)
		pickAbil["info"].setText(start_str1);
	else pickAbil["info"].setText(start_str2);
	
	pickAbil.run();
}

extern const short alch_difficulty[20] = {
	1,1,1,3,3,
	4,5,5,7,9,
	9,10,12,12,9,
	14,19,10,16,20
};
extern const eItemAbil alch_ingred1[20] = {
	eItemAbil::HOLLY,eItemAbil::COMFREY,eItemAbil::HOLLY,eItemAbil::COMFREY,eItemAbil::WORMGRASS,
	eItemAbil::NETTLE,eItemAbil::NETTLE,eItemAbil::WORMGRASS,eItemAbil::GRAYMOLD,eItemAbil::WORMGRASS,
	eItemAbil::GRAYMOLD,eItemAbil::ASPTONGUE,eItemAbil::GRAYMOLD,eItemAbil::MANDRAKE,eItemAbil::EMBERF,
	eItemAbil::MANDRAKE,eItemAbil::MANDRAKE,eItemAbil::NETTLE,eItemAbil::GRAYMOLD,eItemAbil::MANDRAKE,
};
extern const eItemAbil alch_ingred2[20] = {
	eItemAbil::NONE,eItemAbil::NONE,eItemAbil::NONE,eItemAbil::WORMGRASS,eItemAbil::NONE,
	eItemAbil::NONE,eItemAbil::NONE,eItemAbil::NETTLE,eItemAbil::NONE,eItemAbil::ASPTONGUE,
	eItemAbil::HOLLY,eItemAbil::NONE,eItemAbil::COMFREY,eItemAbil::NONE,eItemAbil::NONE,
	eItemAbil::ASPTONGUE,eItemAbil::EMBERF,eItemAbil::EMBERF,eItemAbil::ASPTONGUE,eItemAbil::EMBERF,
};

void display_alchemy(bool allowEdit,cDialog* parent) {
	set_cursor(sword_curs);
	
	cChoiceDlog showAlch("pc-alchemy-info", {"done"}, parent);
	
	for(short i = 0; i < 20; i++) {
		std::string id = "potion" + boost::lexical_cast<std::string>(i + 1);
		std::string name = get_str("magic-names", i + 200) + " (";
		name += std::to_string(alch_difficulty[i]);
		name += ')';
		showAlch->addLabelFor(id, name, LABEL_LEFT, 83, true);
		if(!allowEdit)
			showAlch->getControl(id).attachClickHandler(&cLed::noAction);
		cLed& led = dynamic_cast<cLed&>(showAlch->getControl(id));
		if(univ.party.alchemy[i])
			led.setState(led_red);
		else led.setState(led_off);
	}
	
	showAlch.show();
	if(!allowEdit) return;
	
	for(short i = 0; i < 20; i++) {
		std::string id = "potion" + boost::lexical_cast<std::string>(i + 1);
		cLed& led = dynamic_cast<cLed&>(showAlch->getControl(id));
		if(led.getState() == led_red) univ.party.alchemy[i] = true;
		else univ.party.alchemy[i] = false;
	}
}

// MARK: Start spend XP dialog

struct xp_dlog_state {
	std::map<eSkill,short> skills;
	int who, mode;
	int hp, sp, g, skp;
};

static void do_xp_keep(xp_dlog_state& save) {
	for(int i = 0; i < 19; i++) {
		eSkill skill = eSkill(i);
		univ.party[save.who].skills[skill] = save.skills[skill];
	}
	univ.party[save.who].cur_health += save.hp - univ.party[save.who].max_health;
	univ.party[save.who].max_health = save.hp;
	univ.party[save.who].cur_sp += save.sp - univ.party[save.who].max_sp;
	univ.party[save.who].max_sp = save.sp;
	if(save.mode == 1)
		univ.party.gold = save.g;
	univ.party[save.who].skill_pts = save.skp;
	if(univ.party[save.who].traits[eTrait::ANAMA] && univ.party[save.who].skills[eSkill::MAGE_SPELLS] > 0 && save.mode == 1) {
		univ.party[save.who].skills[eSkill::STRENGTH] -= 2;
		univ.party[save.who].skills[eSkill::DEXTERITY] -= 2;
		univ.party[save.who].skills[eSkill::INTELLIGENCE] -= 4;
		univ.party[save.who].skills[eSkill::LUCK] = 0;
		univ.party[save.who].traits[eTrait::ANAMA] = false;
	}
}

static bool can_change_skill(eSkill skill, xp_dlog_state& save, bool increase) {
	unsigned int min_level, max_level, cur_level, orig_level, cost, g_cost;
	if(skill == eSkill::MAX_HP) {
		min_level = 6;
		max_level = 250;
		cur_level = save.hp;
		orig_level = univ.party[save.who].max_health;
		cost = 1;
		g_cost = 10;
	} else if(skill == eSkill::MAX_SP) {
		min_level = 0;
		max_level = 150;
		cur_level = save.sp;
		orig_level = univ.party[save.who].max_sp;
		cost = 1;
		g_cost = 15;
	} else {
		if(skill == eSkill::STRENGTH || skill == eSkill::DEXTERITY || skill == eSkill::INTELLIGENCE)
			min_level = 1;
		else min_level = 0;
		max_level = skill_max[skill];
		cur_level = save.skills[skill];
		orig_level = univ.party[save.who].skills[skill];
		cost = skill_cost[skill];
		g_cost = skill_g_cost[skill];
	}
	if(increase) {
		if(cur_level == max_level)
			return false;
		if(save.mode < 2 && save.skp < cost)
			return false;
		if(save.mode == 1 && save.g < g_cost)
			return false;
		return true;
	} else {
		if(cur_level == min_level)
			return false;
		if(save.mode == 1 && cur_level == orig_level)
			return false;
		return true;
	}
}

static void draw_xp_skills(cDialog& me,xp_dlog_state& save) {
	// TODO: Wouldn't it make more sense for it to be red when you can't buy the skill rather than red when you can?
	for(short i = 0; i <= 20; i++) {
		eSkill skill = eSkill(i);
		cControl& cur = me[boost::lexical_cast<std::string>(skill)];
		if(can_change_skill(skill, save, true))
			cur.setColour(sf::Color::Red);
		else cur.setColour(me.getDefTextClr());
		if(i < 19)
			cur.setTextToNum(save.skills[skill]);
		else cur.setTextToNum(skill == eSkill::MAX_HP ? save.hp : save.sp);
	}
}

static void update_gold_skills(cDialog& me, xp_dlog_state& save) {
	me["gold"].setTextToNum(((save.mode == 0) ? 0 : save.g));
	me["skp"].setTextToNum(save.skp);
}


static void do_xp_draw(cDialog& me,xp_dlog_state& save) {
	if(save.mode == 0 && univ.party[save.who].main_status != eMainStatus::ALIVE)
		me["recipient"].setText("New PC");
	else me["recipient"].setText(univ.party[save.who].name);
	
	for(int i = 0; i < 19; i++) {
		eSkill skill = eSkill(i);
		save.skills[skill] = univ.party[save.who].skills[skill];
	}
	save.hp = univ.party[save.who].max_health;
	save.sp = univ.party[save.who].max_sp;
	save.g = (save.mode == 0) ? 20000 : univ.party.gold;
	save.skp = univ.party[save.who].skill_pts;
	
	draw_xp_skills(me,save);
	
	
	update_gold_skills(me, save);
}

static bool spend_xp_navigate_filter(cDialog& me, std::string item_hit,xp_dlog_state& save) {
	if(item_hit == "cancel") {
		// TODO: Um, I'm pretty sure this can never happen.
		if(save.mode == 0 && univ.party[save.who].main_status < eMainStatus::ABSENT)
			univ.party[save.who].main_status = eMainStatus::ABSENT;
		me.setResult(false);
		me.toast(false);
	} else if(item_hit == "help") {
		give_help(210,11,me);
	} else if(item_hit == "keep") {
		do_xp_keep(save);
		me.setResult(true);
		me.toast(true);
	} else if(item_hit == "left") {
		// TODO: Try not forcing a commit when using the arrows?
		if(save.mode != 0) {
			do_xp_keep(save);
			do {
				save.who = (save.who == 0) ? 5 : save.who - 1;
			} while(univ.party[save.who].main_status != eMainStatus::ALIVE);
			do_xp_draw(me,save);
		} else
			beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
	} else if(item_hit == "right") {
		// TODO: If they don't work in mode 0, why are they visible?
		if(save.mode != 0) {
			do_xp_keep(save);
			do {
				save.who = (save.who == 5) ? 0 : save.who + 1;
			} while(univ.party[save.who].main_status != eMainStatus::ALIVE);
			do_xp_draw(me,save);
		} else
			beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
	}
	return true;
}

static bool spend_xp_event_filter(cDialog& me, std::string item_hit, eKeyMod mods,xp_dlog_state& save) {
	if(item_hit.substr(0,2) == "hp") {
		if(mod_contains(mods, mod_alt)) {
			cStrDlog aboutHP(get_str("help",63),"","About Health",24,PIC_DLOG,&me);
			aboutHP.setSound(57);
			aboutHP.show();
		} else if(item_hit[3] == 'm') {
			if(can_change_skill(eSkill::MAX_HP, save, false)) {
				save.hp -= 2;
				if(save.mode < 2) {
					save.skp += 1;
					if(save.mode == 1)
						save.g += 10;
				}
			} else beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
		} else if(item_hit[3] == 'p') {
			if(can_change_skill(eSkill::MAX_SP, save, true)) {
				save.hp += 2;
				if(save.mode < 2) {
					save.skp -= 1;
					if(save.mode == 1)
						save.g -= 10;
				}
			} else {
				if(save.mode < 2 && save.skp < 1)
					give_help(25,0,me);
				else if(save.mode == 1 && save.g < 10)
					give_help(24,0,me);
				else beep();
			}
		}
		
		update_gold_skills(me, save);
		me["hp"].setTextToNum(save.hp);
		draw_xp_skills(me,save);
		
	} else if(item_hit.substr(0,2) == "sp") {
		if(mod_contains(mods, mod_alt)) {
			cStrDlog aboutSP(get_str("help",64),"","About Spell Points",24,PIC_DLOG,&me);
			aboutSP.setSound(57);
			aboutSP.show();
		} else if(item_hit[3] == 'm') {
			if(can_change_skill(eSkill::MAX_SP, save, false)) {
				save.sp -= 1;
				if(save.mode < 2) {
					save.skp += 1;
					if(save.mode == 1)
						save.g += 15;
				}
			} else beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
		} else if(item_hit[3] == 'p') {
			if(can_change_skill(eSkill::MAX_SP, save, true)) {
				save.sp += 1;
				if(save.mode < 2) {
					save.skp -= 1;
					if(save.mode == 1)
						save.g -= 15;
				}
			} else {
				if(save.mode < 2 && save.skp < 1)
					give_help(25,0,me);
				else if(save.mode == 1 && save.g < 15)
					give_help(24,0,me);
				else beep();
			}
		}
		
		update_gold_skills(me, save);
		me["sp"].setTextToNum(save.sp);
		draw_xp_skills(me,save);
	} else {
		eSkill which_skill = eSkill::INVALID;
		// TODO: Even though it includes 19 and 20, these will never match since they're handled separately above
		for(int i = 0; i <= 20; i++) {
			std::string id_base = boost::lexical_cast<std::string>(eSkill(i));
			if(id_base.compare(0, id_base.length(), item_hit, 0, item_hit.length() - 2) == 0) {
				which_skill = eSkill(i);
				break;
			}
		}
		if(which_skill == eSkill::INVALID) return true;
		if(which_skill == eSkill::MAGE_SPELLS && univ.party[save.who].traits[eTrait::ANAMA] && save.mode == 1) {
			if(save.skills[eSkill::MAGE_SPELLS] == 0)
				cStrDlog("The oaths of an Anama member include eschewing research into arcane magics. By increasing your mage spells skill, you will be in violation of this oath. If you keep this change, you will be afflicted with a terrible permanent curse.", "", "The Anama Curse", 41, PIC_TALK).show();
 		}
		if(mod_contains(mods, mod_alt)) display_skills(which_skill,&me);
		else {
			char dir = item_hit[item_hit.length() - 1];
			
			if(dir == 'm') {
				if(can_change_skill(which_skill, save, false)) {
					save.skills[which_skill] -= 1;
					if(save.mode < 2) {
						save.skp += skill_cost[which_skill];
						if(save.mode == 1)
							save.g += skill_g_cost[which_skill];
					}
				} else beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
			} else if(dir == 'p') {
				if(can_change_skill(which_skill, save, true)) {
					save.skills[which_skill] += 1;
					if(save.mode < 2) {
						save.skp -= skill_cost[which_skill];
						if(save.mode == 1)
							save.g -= skill_g_cost[which_skill];
					}
				} else {
					if(save.mode < 2 && save.skp < skill_cost[which_skill])
						give_help(25,0,me);
					else if(save.mode == 1 && save.g < skill_g_cost[which_skill])
						give_help(24,0,me);
					else beep(); // TODO: This is a game event, so it should have a game sound, not a system alert.
				}
			}
			
			update_gold_skills(me, save);
			me[boost::lexical_cast<std::string>(which_skill)].setTextToNum(save.skills[which_skill]);
			draw_xp_skills(me,save);
		}
	}
	return true;
}

// Mode is one of the following:
// 0 - Creating a new character (need skill points but not gold)
// 1 - Training a character (need both skill points and gold)
// 2 - Editing a character (need neither skill points nor gold)
bool spend_xp(short pc_num, short mode, cDialog* parent) {
	using namespace std::placeholders;
	
	set_cursor(sword_curs);
	
	xp_dlog_state save;
	save.who = pc_num;
	save.mode = mode;
	save.skills = univ.party[pc_num].skills;
	
	cDialog xpDlog("spend-xp",parent);
	xpDlog.addLabelFor("hp","Health (1/10)",LABEL_LEFT,75,true);
	xpDlog.addLabelFor("sp","Spell Pts. (1/15)",LABEL_LEFT,75,true);
	auto spend_xp_filter = std::bind(spend_xp_event_filter,_1,_2,_3,std::ref(save));
	static const std::string minus = "-m", plus = "-p";
	for(int i = 54; i < 73; i++) {
		std::ostringstream sout;
		eSkill skill = eSkill(i - 54);
		std::string id = boost::lexical_cast<std::string>(skill);
		sout << get_str("skills",1 + 2 * (i - 54)) << ' ' << '(';
		sout << skill_cost[skill] << '/' << skill_g_cost[skill] << ')';
		xpDlog.addLabelFor(id,sout.str(),LABEL_LEFT,(i < 63) ? 75 : 69,true);
		xpDlog[id + minus].attachClickHandler(spend_xp_filter);
		xpDlog[id + plus].attachClickHandler(spend_xp_filter);
	}
	do_xp_draw(xpDlog,save);
	
	xpDlog.attachClickHandlers(std::bind(spend_xp_navigate_filter,_1,_2,std::ref(save)),{"keep","cancel","left","right","help"});
	xpDlog.attachClickHandlers(spend_xp_filter,{"sp-m","sp-p","hp-m","hp-p"});
	
	give_help(10,11,xpDlog);
	
	xpDlog.run();
	
	return xpDlog.getResult<bool>();
}

void edit_stuff_done() {
	cDialog sdf_dlg("set-sdf");
	sdf_dlg.attachFocusHandlers([](cDialog& me, std::string, bool losing) -> bool {
		if(!losing) return true;
		int x = me["x"].getTextAsNum(), y = me["y"].getTextAsNum();
		if(!univ.party.sd_legit(x,y)) {
			std::ostringstream strb;
			strb << "SDF (" << x << ',' << y << ") does not exist!";
			me["feedback"].setText(strb.str());
		} else {
			me["feedback"].setText("");
			me["val"].setTextToNum(univ.party.stuff_done[x][y]);
		}
		return true;
	}, {"x","y"});
	sdf_dlg["set"].attachClickHandler([](cDialog& me, std::string, eKeyMod) -> bool {
		int x = me["x"].getTextAsNum(), y = me["y"].getTextAsNum(), val = me["val"].getTextAsNum();
		std::ostringstream strb;
		if(!univ.party.sd_legit(x,y)) {
			strb << "SDF (" << x << ',' << y << ") does not exist!";
			me["feedback"].setText(strb.str());
		} else {
			strb << "You have set SDF (" << x << ',' << y << ") = " << val;
			me["feedback"].setText(strb.str());
			univ.party.stuff_done[x][y] = val;
		}
		return true;
	});
	sdf_dlg["exit"].attachClickHandler(std::bind(&cDialog::toast, &sdf_dlg, false));
	// Initialize fields with some default values
	sdf_dlg["x"].setText("0");
	sdf_dlg["y"].setText("0");
	sdf_dlg["val"].setTextToNum(univ.party.stuff_done[0][0]);
	sdf_dlg.run();
}
