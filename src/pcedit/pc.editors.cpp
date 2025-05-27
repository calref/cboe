
#include "pc.graphics.hpp"
#include "universe/universe.hpp"
#include "pc.editors.hpp"
#include "utility.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "dialogxml/widgets/control.hpp"
#include "dialogxml/widgets/button.hpp"
#include "dialogxml/widgets/ledgroup.hpp"
#include "dialogxml/widgets/pict.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "tools/winutil.hpp"
#include "tools/cursors.hpp"
#include "gfx/render_shapes.hpp" // for colour constants
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

void display_skills(eSkill skill,cDialog* parent, bool record = false);

extern cUniverse univ;
extern short store_flags[3];
extern short current_active_pc;

extern rectangle d_rects[80];
extern short d_rect_index[80];

extern bool current_file_has_maps;
bool choice_active[6];

extern short which_pc_displayed;
cPlayer *store_pc;

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
	
	me["title"].replaceText("{{type}}", store_trait_mode == 0 ? "Mage" : "Priest");
	me["who"].setText(univ.party[which_pc_displayed].name.c_str());
}

static bool check_for_spell_changes(cDialog& pcInfo, short pc_num, short mode) {
	if(mode >= 10) {
		mode %= 10;
		for(short i = 0; i < 62; i++) {
			std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
			bool set = dynamic_cast<cLed&>(pcInfo[id]).getState() != led_off;
			if(mode == 0 && univ.party[pc_num].mage_spells[i] != set) return true;
			else if(mode == 1 && univ.party[pc_num].priest_spells[i] != set) return true;
		}
	}
	return false;
}

static void keep_pc_spells(cDialog& pcInfo, short pc_num, short mode) {
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

static bool confirm_switch_pc_spells(cDialog& me, short pc_num, short mode) {
	cChoiceDlog dlog("confirm-edit-spells", {"keep","revert","cancel"}, &me);
	dlog->getControl("keep-msg").replaceText("{{PC}}", univ.party[pc_num].name);
	std::string choice = dlog.show();
	if(choice == "keep"){
		keep_pc_spells(me, pc_num, mode);
		return true;
	}else if(choice == "revert"){
		return true;
	}else{
		return false;
	}
}

static bool display_pc_event_filter(cDialog& me, std::string item_hit, const short trait_mode) {
	short pc_num;
	
	pc_num = which_pc_displayed;
	if(item_hit == "cancel"){
		me.toast(false);
	}else if(item_hit == "done") {
		me.toast(true);
		keep_pc_spells(me, pc_num, trait_mode);
	}else if(item_hit == "left") {
		// if spells haven't changed, confirmation isn't necessary
		if(!check_for_spell_changes(me, pc_num, trait_mode) || confirm_switch_pc_spells(me, pc_num, trait_mode)){
			do {
				pc_num = (pc_num == 0) ? 5 : pc_num - 1;
			} while(univ.party[pc_num].main_status == eMainStatus::ABSENT);
			which_pc_displayed = pc_num;
			put_pc_spells(me, trait_mode);
		}
	}else if(item_hit == "right") {
		// if spells haven't changed, confirmation isn't necessary
		if(!check_for_spell_changes(me, pc_num, trait_mode) || confirm_switch_pc_spells(me, pc_num, trait_mode)){
			do {
				pc_num = (pc_num == 5) ? 0 : pc_num + 1;
			} while(univ.party[pc_num].main_status == eMainStatus::ABSENT);
			which_pc_displayed = pc_num;
			put_pc_spells(me, trait_mode);
		}
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
	
	cDialog pcInfo(*ResMgr::dialogs.get("pc-spell-info"), parent);
	pcInfo.attachClickHandlers(std::bind(display_pc_event_filter, _1, _2, mode),{"done","left","right","cancel"});
	
	for(short i = 0; i < 62; i++) {
		std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
		label_str = get_str("magic-names", i + (mode % 10 == 0 ? 1 : 101));
		pcInfo[id].setText(label_str);
		pcInfo[id].recalcRect();
		if(mode < 10)
			pcInfo[id].attachClickHandler(&cLed::noAction);
	}
	put_pc_spells(pcInfo, mode);
	
	dynamic_cast<cPict&>(pcInfo["pic"]).setPict(15 + mode,PIC_DLOG); // 15 is explosion, 16 is an ankh
	
	pcInfo.run();
	
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
			default: return false;
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
	
	cDialog pickAbil(*ResMgr::dialogs.get("pick-race-abil"),parent);
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

void display_alchemy(bool allowEdit,cDialog* parent) {
	set_cursor(sword_curs);
	
	cChoiceDlog showAlch("pc-alchemy-info", {"done"}, parent);
	
	for(short i = 0; i < 20; i++) {
		std::string id = "potion" + boost::lexical_cast<std::string>(i + 1);
		std::string name = get_str("magic-names", i + 200) + " (";
		name += std::to_string((*eAlchemy(i)).difficulty);
		name += ')';
		if(!allowEdit)
			showAlch->getControl(id).attachClickHandler(&cLed::noAction);
		cLed& led = dynamic_cast<cLed&>(showAlch->getControl(id));
		led.setText(name);
		led.recalcRect();
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
	int start_skp;
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

static int get_skill_max(eSkill skill) {
	switch(skill){
		case eSkill::MAX_HP:
			return 250;
		case eSkill::MAX_SP:
			return 150;
		default:
			return skill_max[skill];
	}
}

static bool can_change_skill(eSkill skill, xp_dlog_state& save, bool increase) {
	unsigned int min_level, max_level, cur_level, orig_level, cost, g_cost;
	max_level = get_skill_max(skill);
	if(skill == eSkill::MAX_HP) {
		min_level = 6;
		cur_level = save.hp;
		orig_level = univ.party[save.who].max_health;
		cost = 1;
		g_cost = 10;
	} else if(skill == eSkill::MAX_SP) {
		min_level = 0;
		cur_level = save.sp;
		orig_level = univ.party[save.who].max_sp;
		cost = 1;
		g_cost = 15;
	} else {
		if(skill == eSkill::STRENGTH || skill == eSkill::DEXTERITY || skill == eSkill::INTELLIGENCE)
			min_level = 1;
		else min_level = 0;
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
	auto add_cost_to_label = [&me, save](std::string skill, int skp, int gold, bool max) {
		cControl& label = me[skill + "-label"];
		// If cost is already there, start over
		if(label.getText().find('(') != std::string::npos){
			label.setText(label.getText().substr(0, label.getText().find('(') - 1));
		}
		if(max){
			label.appendText (" (MAX)");
		}else if(save.mode < 2){
			label.appendText(" (");
			label.appendText(std::to_string(skp) + " pt");
			if(skp != 1){
				label.appendText("s");
			}
			label.appendText(".");
			if(save.mode == 1){
				label.appendText("/" + std::to_string(gold) + "gp");
			}
			label.appendText(")");
		}
	};

	add_cost_to_label("hp", 1, 10, univ.party[save.who].max_health == get_skill_max(eSkill::MAX_HP));
	add_cost_to_label("sp", 1, 15, univ.party[save.who].max_sp == get_skill_max(eSkill::MAX_SP));
	for(short i = 0; i <= 20; i++) {
		eSkill skill = eSkill(i);
		std::string id = boost::lexical_cast<std::string>(skill);
		cControl& cur = me[boost::lexical_cast<std::string>(skill)];
		// White means it can't change.
		cur.setColour(me.getDefTextClr());
		me[id + "-m"].hide();
		// Red means it can be changed, but only down
		if(can_change_skill(skill, save, false)){
			me[id + "-m"].show();
			cur.setColour(Colours::RED);
		}
		// Green means it can still be bumped up!
		me[id + "-p"].hide();
		if(can_change_skill(skill, save, true)){
			me[id + "-p"].show();
			cur.setColour(Colours::LIGHT_GREEN);
		}
		if(i < 19){
			cur.setTextToNum(save.skills[skill]);
			add_cost_to_label(id, skill_cost[skill], skill_g_cost[skill], save.skills[skill] == get_skill_max(skill));
		}
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

static bool confirm_switch_pc(cDialog& me, xp_dlog_state& save) {
	cChoiceDlog dlog("confirm-spend-xp", {"keep","revert","cancel"}, &me);
	dlog->getControl("keep-msg").replaceText("{{PC}}", univ.party[save.who].name);
	std::string choice = dlog.show();
	if(choice == "keep"){
		do_xp_keep(save);
		return true;
	}else if(choice == "revert"){
		return true;
	}else{
		return false;
	}
}

static bool spend_xp_navigate_filter(cDialog& me, std::string item_hit,xp_dlog_state& save) {
	if(item_hit == "cancel") {
		// TODO: Um, I'm pretty sure this can never happen.
		if(save.mode == 0 && univ.party[save.who].main_status < eMainStatus::ABSENT)
			univ.party[save.who].main_status = eMainStatus::ABSENT;
		me.setResult(false);
		me.toast(false);
	} else if(item_hit == "help") {
		give_help(10,11,me,true);
	} else if(item_hit == "keep") {
		do_xp_keep(save);
		me.setResult(true);
		me.toast(true);
	} else if(item_hit == "left") {
		// If the number of skill points hasn't changed, no confirmation is necessary
		if(save.skp == save.start_skp || confirm_switch_pc(me, save)){
			do {
				save.who = (save.who == 0) ? 5 : save.who - 1;
			} while(univ.party[save.who].main_status != eMainStatus::ALIVE);
			do_xp_draw(me,save);
			save.start_skp = save.skp;
		}
	} else if(item_hit == "right") {
		// If the number of skill points hasn't changed, no confirmation is necessary
		if(save.skp == save.start_skp || confirm_switch_pc(me, save)){
			do {
				save.who = (save.who == 5) ? 0 : save.who + 1;
			} while(univ.party[save.who].main_status != eMainStatus::ALIVE);
			do_xp_draw(me,save);
			save.start_skp = save.skp;
		}
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
			} else play_sound(1);
		} else if(item_hit[3] == 'p') {
			if(can_change_skill(eSkill::MAX_HP, save, true)) {
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
				else play_sound(1);
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
			} else play_sound(1);
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
				else play_sound(1);
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
				} else play_sound(1);
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
					else play_sound(1);
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
	
	cDialog xpDlog(*ResMgr::dialogs.get("spend-xp"),parent);

	// Making new PC, you can't switch to train other PCs
	if(mode == 0){
		xpDlog["left"].hide();
		xpDlog["right"].hide();
	}

	const int LABEL_OFFSET_COL1 = 85;
	const int LABEL_OFFSET_COL2 = 74;
	xpDlog.addLabelFor("hp","Health",LABEL_LEFT,LABEL_OFFSET_COL1,true);
	xpDlog.addLabelFor("sp","Spell Pts.",LABEL_LEFT,LABEL_OFFSET_COL1,true);
	auto spend_xp_filter = std::bind(spend_xp_event_filter,_1,_2,_3,std::ref(save));
	static const std::string minus = "-m", plus = "-p";
	for(int i = 54; i < 73; i++) {
		std::ostringstream sout;
		eSkill skill = eSkill(i - 54);
		std::string id = boost::lexical_cast<std::string>(skill);
		xpDlog.addLabelFor(id,get_str("skills",1 + 2 * (i - 54)),LABEL_LEFT,(i < 63) ? LABEL_OFFSET_COL1 : LABEL_OFFSET_COL2,true);
		xpDlog[id + minus].attachClickHandler(spend_xp_filter);
		xpDlog[id + plus].attachClickHandler(spend_xp_filter);
	}
	do_xp_draw(xpDlog,save);
	save.start_skp = save.skp;
	
	xpDlog.attachClickHandlers(std::bind(spend_xp_navigate_filter,_1,_2,std::ref(save)),{"keep","cancel","left","right","help"});
	xpDlog.attachClickHandlers(spend_xp_filter,{"sp-m","sp-p","hp-m","hp-p"});
	
	if(mode != 1){
		xpDlog["gold-label"].hide();
		xpDlog["gold"].hide();
	}

	xpDlog.runWithHelp(10,11);
	
	return xpDlog.getResult<bool>();
}

void edit_stuff_done() {
	cDialog sdf_dlg(*ResMgr::dialogs.get("set-sdf"));
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
