
#include <cstdio>
#include <cstring>
#include <stack>
#include <boost/lexical_cast.hpp>
#include "scen.global.h"
#include "scenario.h"
#include "regtown.h"
#include "graphtool.hpp"
#include "scen.graphics.h"
#include "scen.townout.h"
#include "scen.keydlgs.h"
#include "scen.fileio.h"
#include "scen.core.h"
#include "mathutil.hpp"
#include "button.hpp"
#include "dlogutil.hpp"
#include "winutil.hpp"
#include "stack.hpp"
#include "fileio.hpp"

extern short cen_x, cen_y, overall_mode;
extern bool mouse_button_held,editing_town;
extern short cur_viewing_mode;
extern cTown* town;
extern short mode_count,to_create,cur_town;
extern ter_num_t template_terrain[64][64];
extern cScenario scenario;
extern cOutdoors* current_terrain;
extern location cur_out;

const char *day_str_1[] = {"Unused","Day creature appears","Day creature disappears",
	"Unused","Unused","Unused","Unused","Unused","Unused"};
const char *day_str_2[] = {"Unused","Event code (0 - no event)","Event code (0 - no event)",
	"Unused","Unused","Unused",
	"Event code (0 - no event)","Event code (0 - no event)","Unused"};

static void put_placed_monst_in_dlog(cDialog& me, cTownperson& monst, const short which) {
	me["num"].setTextToNum(which);
	me["type"].setText(scenario.scen_monsters[monst.number].m_name);
	// TODO: Make attitude an enum
	dynamic_cast<cLedGroup&>(me["attitude"]).setSelected("att" + std::to_string(monst.start_attitude + 1));
	dynamic_cast<cLedGroup&>(me["mobility"]).setSelected("mob" + std::to_string(monst.mobility + 1));
	me["talk"].setTextToNum(monst.personality);
	me["picnum"].setTextToNum(monst.facial_pic);
	// TODO: Use -1 instead of 0 for "no pic", since 0 is a valid talking picture
 	if(short(monst.facial_pic) < 0)
		dynamic_cast<cPict&>(me["pic"]).setPict(scenario.scen_monsters[monst.number].picture_num, PIC_MONST);
	else if((monst.facial_pic >= 1000))
		dynamic_cast<cPict&>(me["pic"]).setPict(monst.facial_pic - 1000,PIC_CUSTOM_TALK);
	else dynamic_cast<cPict&>(me["pic"]).setPict(monst.facial_pic,PIC_TALK);
}

static void get_placed_monst_in_dlog(cDialog& me, cTownperson& store_placed_monst) {
	store_placed_monst.start_attitude = dynamic_cast<cLedGroup&>(me["attitude"]).getSelected()[3] - '1';
	store_placed_monst.mobility = dynamic_cast<cLedGroup&>(me["mobility"]).getSelected()[3] - '1';
	store_placed_monst.personality = me["talk"].getTextAsNum();
	store_placed_monst.facial_pic = me["picnum"].getTextAsNum();
}

static bool edit_placed_monst_event_filter(cDialog& me, std::string hit, cTownperson& monst, const short which) {
	short i;
	cTownperson store_m;
	
	if(hit == "okay") {
		if(!me.toast(true)) return true;
		get_placed_monst_in_dlog(me, monst);
		town->creatures[which] = monst;
	} else if(hit == "cancel") {
		me.toast(false);
	} else if(hit == "del") {
		me.toast(false);
		town->creatures[which].number = 0;
	} else if(hit == "type-edit") {
		get_placed_monst_in_dlog(me, monst);
		i = choose_text(STRT_MONST,monst.number,&me,"Choose Which Monster:");
		if(i >= 0) {
			monst.number = i;
			put_placed_monst_in_dlog(me, monst, which);
		}
	} else if(hit == "pict-edit") {
		get_placed_monst_in_dlog(me, monst);
		i = choose_graphic(monst.facial_pic,PIC_TALK,&me);
		monst.facial_pic = i;
		put_placed_monst_in_dlog(me, monst, which);
	} else if(hit == "talk-edit") {
		get_placed_monst_in_dlog(me, monst);
		i = choose_text(STRT_TALK, monst.personality, &me, "Which personality?");
		if(i >= 0)
			monst.personality = i;
		put_placed_monst_in_dlog(me, monst, which);
	} else if(hit == "more") { //advanced
		store_m = edit_placed_monst_adv(monst, which, me);
		if(store_m.number != 0)
			monst = store_m;
	}
	return true;
}

void edit_placed_monst(short which_m) {
	using namespace std::placeholders;
	cTownperson monst = town->creatures[which_m];
	
	cDialog edit("edit-townperson");
	edit.attachClickHandlers(std::bind(edit_placed_monst_event_filter, _1, _2, std::ref(monst), which_m), {"type-edit", "pict-edit", "talk-edit", "okay", "cancel", "more", "del"});
	
	put_placed_monst_in_dlog(edit, monst, which_m);
	
	edit.run();
}

static void put_placed_monst_adv_in_dlog(cDialog& me, cTownperson& monst, const short which) {
	me["num"].setTextToNum(which);
	me["type"].setText(scenario.scen_monsters[monst.number].m_name);
	int iTime = 0;
	switch(monst.time_flag) {
		case eMonstTime::ALWAYS: iTime = 0; break;
		case eMonstTime::APPEAR_ON_DAY: iTime = 1; break;
		case eMonstTime::DISAPPEAR_ON_DAY: iTime = 2; break;
		case eMonstTime::SOMETIMES_A: iTime = 3; break;
		case eMonstTime::SOMETIMES_B: iTime = 4; break;
		case eMonstTime::SOMETIMES_C: iTime = 5; break;
		case eMonstTime::APPEAR_WHEN_EVENT: iTime = 6; break;
		case eMonstTime::DISAPPEAR_WHEN_EVENT: iTime = 7; break;
		case eMonstTime::APPEAR_AFTER_CHOP: iTime = 8; break;
	}
	dynamic_cast<cLedGroup&>(me["time"]).setSelected("time" + std::to_string(iTime + 1));
	me["extra1-lbl"].setText(day_str_1[iTime]);
	me["extra2-lbl"].setText(day_str_2[iTime]);
	me["extra1"].setTextToNum(monst.monster_time);
	me["extra2"].setTextToNum(monst.time_code);
	me["group"].setTextToNum(monst.spec_enc_code);
	me["death"].setTextToNum(monst.special_on_kill);
	me["sdfx"].setTextToNum(monst.spec1);
	me["sdfy"].setTextToNum(monst.spec2);
}

static bool get_placed_monst_adv_in_dlog(cDialog& me, cTownperson& monst) {
	switch(dynamic_cast<cLedGroup&>(me["time"]).getSelected()[4] - '1') {
		case 0: monst.time_flag = eMonstTime::ALWAYS; break;
		case 1: monst.time_flag = eMonstTime::APPEAR_ON_DAY; break;
		case 2: monst.time_flag = eMonstTime::DISAPPEAR_ON_DAY; break;
		case 3: monst.time_flag = eMonstTime::SOMETIMES_A; break;
		case 4: monst.time_flag = eMonstTime::SOMETIMES_B; break;
		case 5: monst.time_flag = eMonstTime::SOMETIMES_C; break;
		case 6: monst.time_flag = eMonstTime::APPEAR_WHEN_EVENT; break;
		case 7: monst.time_flag = eMonstTime::DISAPPEAR_WHEN_EVENT; break;
		case 8: monst.time_flag = eMonstTime::APPEAR_AFTER_CHOP; break;
	}
	monst.monster_time = me["extra1"].getTextAsNum();
  	if(cre(monst.monster_time,0,1000,"Given day must be from 0 to 1000.","",&me)) return false;
	monst.time_code = me["extra2"].getTextAsNum();
  	if(cre(monst.time_code,0,10,"Event code must be 0 (for no event) or from 1 to 10.","",&me)) return false;
	monst.special_on_kill = me["death"].getTextAsNum();
  	if(cre(monst.special_on_kill,-1,99,"Town special node number must be from 0 to 99 (or -1 for no special).","",&me)) return false;
	monst.spec1 = me["sdfx"].getTextAsNum();
  	if(cre(monst.spec1,-1,299,"First part of special flag must be -1 (if this is to be ignored) or from 0 to 299.","",&me)) return false;
	monst.spec2 = me["sdfy"].getTextAsNum();
  	if(cre(monst.spec2,-1,9,"Second part of special flag must be -1 (if this is to be ignored) or from 0 to 9.","",&me)) return false;
	
	monst.spec_enc_code = me["group"].getTextAsNum();
	return true;
}

static bool edit_placed_monst_adv_event_filter(cDialog& me, std::string hit, cTownperson& monst) {
	if(hit == "okay") {
		if(!get_placed_monst_adv_in_dlog(me, monst)) return true;
		me.toast(true);
	} else if(hit == "cancel") {
		monst.number = 0;
		me.toast(false);
	}
	return true;
}

static bool edit_placed_monst_adv_time_flag(cDialog& me, std::string, bool losingFocus) {
	if(losingFocus) return true;
	
	cLedGroup& time = dynamic_cast<cLedGroup&>(me["time"]);
	int item_hit = time.getSelected()[4] - '1';
	me["extra1-lbl"].setText(day_str_1[item_hit]);
	me["extra2-lbl"].setText(day_str_2[item_hit]);
	return true;
}

static bool edit_placed_monst_adv_death(cDialog& me, cTownperson& monst) {
	short spec = monst.special_on_kill;
	if(spec < 0)
		spec = get_fresh_spec(2);
	if(edit_spec_enc(spec,2,&me))
		me["death"].setTextToNum(spec);
	return true;
}

cTownperson edit_placed_monst_adv(cTownperson initial, short which, cDialog& parent) {
	using namespace std::placeholders;
	
	cDialog edit("edit-townperson-advanced", &parent);
	edit["okay"].attachClickHandler(std::bind(edit_placed_monst_adv_event_filter, _1, _2, std::ref(initial)));
	edit["cancel"].attachClickHandler(std::bind(edit_placed_monst_adv_event_filter, _1, _2, std::ref(initial)));
	edit["editdeath"].attachClickHandler(std::bind(edit_placed_monst_adv_death, _1, std::ref(initial)));
	edit["time"].attachFocusHandler(edit_placed_monst_adv_time_flag);
	
	put_placed_monst_adv_in_dlog(edit,initial,which);
	
	edit.run();
	
	return initial;
}

static bool put_placed_item_in_dlog(cDialog& me, const cTown::cItem& item, const short which) {
	std::ostringstream loc;
	cItem base = scenario.scen_items[item.code];
	if(item.ability >= 0 && item.ability <= int(eEnchant::BLESSED) && (base.variety == eItemType::ONE_HANDED || base.variety == eItemType::TWO_HANDED)) {
		base.enchant_weapon(eEnchant(item.ability), 0);
	}
	
	me["num"].setTextToNum(which);
	loc << "X = " << item.loc.x << ", Y = " << item.loc.y;
	me["loc"].setText(loc.str());
	me["name"].setText(base.full_name);
	me["charges"].setTextToNum(item.charges);
	me["abil"].setTextToNum(item.ability);
	if(item.always_there)
		dynamic_cast<cLed&>(me["always"]).setState(led_red);
	if(item.property)
		dynamic_cast<cLed&>(me["owned"]).setState(led_red);
	if(item.contained)
		dynamic_cast<cLed&>(me["contained"]).setState(led_red);
	
	dynamic_cast<cPict&>(me["pic"]).setPict(base.graphic_num, PIC_ITEM);
	
	me["charges"].show();
	me["charges-lbl"].show();
	me["abil"].show();
	me["abil-lbl"].show();
	me["abil-choose"].show();
	
	if(base.variety == eItemType::GOLD || base.variety == eItemType::FOOD) {
		me["charges-lbl"].setText("Amount:");
	} else if(base.charges == 0) {
		me["charges-lbl"].hide();
		me["charges"].hide();
	} else {
		me["charges-lbl"].setText("Charges:||(-1 = default)");
	}
	
	if(base.variety == eItemType::ONE_HANDED || base.variety == eItemType::TWO_HANDED) {
		me["abil-lbl"].setText("Enchantment:");
	} else {
		me["abil-lbl"].hide();
		me["abil"].hide();
		me["abil-choose"].hide();
	}
	return true;
}

static bool get_placed_item_in_dlog(cDialog& me, cTown::cItem& item, const short which) {
	if(!me.toast(true)) return true;
	
	item.charges = me["charges"].getTextAsNum();
	if(item.charges < -1 || item.charges > 2500) {
		giveError("Number of charges/amount of gold or food must be from 0 to 2500.",
				  "If an item with charges (not gold or food) leave this at -1 for the item to have the default number of charges.",&me);
		return true;
	}
	
	eItemType type = scenario.scen_items[item.code].variety;
	if(item.charges == 0 && (type == eItemType::GOLD || type == eItemType::FOOD)) {
		giveError("You must assign gold or food an amount of at least 1.","",&me);
		return false;
	}
	
	item.always_there = dynamic_cast<cLed&>(me["always"]).getState() != led_off;
	item.property = dynamic_cast<cLed&>(me["owned"]).getState() != led_off;
	item.contained = dynamic_cast<cLed&>(me["contained"]).getState() != led_off;
	
	town->preset_items[which] = item;
	return true;
}

static bool edit_placed_item_type(cDialog& me, cTown::cItem& item, const short which) {
	short i = choose_text(STRT_ITEM, item.code, &me, "Place which item?");
	if(i >= 0) {
		item.code = i;
		put_placed_item_in_dlog(me, item, which);
	}
	return true;
}

static bool edit_placed_item_abil(cDialog& me, cTown::cItem& item, const short which) {
	cItem& base = scenario.scen_items[item.code];
	short i = item.ability;
	if(base.variety == eItemType::ONE_HANDED || base.variety == eItemType::TWO_HANDED) {
		i = choose_text(STRT_ENCHANT, item.ability, &me, "Which enchantment?");
	}
	if(i >= 0) {
		item.ability = i;
		put_placed_item_in_dlog(me, item, which);
	}
	return true;
}

static bool edit_placed_item_delete(cDialog& me, const short which) {
	me.toast(false);
	town->preset_items[which].code = -1;
	return true;
}

void edit_placed_item(short which_i) {
	using namespace std::placeholders;
	
	cTown::cItem item = town->preset_items[which_i];
	
	cDialog item_dlg("edit-placed-item");
	item_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &item_dlg, false));
	item_dlg["okay"].attachClickHandler(std::bind(get_placed_item_in_dlog, _1, std::ref(item), which_i));
	item_dlg["choose"].attachClickHandler(std::bind(edit_placed_item_type, _1, std::ref(item), which_i));
	item_dlg["del"].attachClickHandler(std::bind(edit_placed_item_delete, _1, which_i));
	item_dlg["abil-choose"].attachClickHandler(std::bind(edit_placed_item_abil, _1, std::ref(item), which_i));
	item_dlg["abil"].attachFocusHandler(std::bind(put_placed_item_in_dlog, _1, std::ref(item), which_i));
	
	put_placed_item_in_dlog(item_dlg, item, which_i);
	
	item_dlg.run();
}

static bool edit_sign_event_filter(cDialog& me, short which_sign) {
	if(!me.toast(true)) return true;
	if(editing_town)
		town->sign_strs[which_sign] = me["text"].getText();
	else current_terrain->sign_strs[which_sign] = me["text"].getText();
#if 0 // TODO: Apparently there used to be left/right buttons on this dialog.
	if(item_hit == 3)
		which_sign--;
	else which_sign++;
	if(which_sign < 0)
		which_sign = (editing_town) ? 14 : 7;
	if(which_sign > (editing_town) ? 14 : 7)
		which_sign = 0;
#endif
	return true;
}

void edit_sign(short which_sign,short picture) {
	using namespace std::placeholders;
	location view_loc;
	
	cDialog sign_dlg("edit-sign");
	sign_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &sign_dlg, false));
	sign_dlg["okay"].attachClickHandler(std::bind(edit_sign_event_filter, _1, which_sign));
	cPict& icon = dynamic_cast<cPict&>(sign_dlg["pic"]);
	if(picture >= 960 && picture < 1000)
		icon.setPict(picture, PIC_TER_ANIM);
	else if(picture >= 2000)
		icon.setPict(picture - 2000, PIC_CUSTOM_TER_ANIM);
	else icon.setPict(picture, PIC_TER); // automatically adjusts for custom graphics
	
	sign_dlg["num"].setTextToNum(which_sign);
	if(!editing_town)
		sign_dlg["text"].setText(current_terrain->sign_strs[which_sign]);
	else sign_dlg["text"].setText(town->sign_strs[which_sign]);
	
	sign_dlg.run();
}

static bool save_roomdescs(cDialog& me, bool isTown, std::array<bool,16> str_do_delete) {
	if(!me.toast(true)) return true;
	int numDescs = isTown ? 16 : 8;
	for(int i = 0; i < numDescs; i++) {
		std::string id = "desc" + std::to_string(i + 1);
		if(isTown) {
			town->room_rect[i].descr = me[id].getText().substr(0,30);
			if(str_do_delete[i])
				town->room_rect[i].right = 0;
		} else {
			current_terrain->info_rect[i].descr = me[id].getText().substr(0,30);
			if(str_do_delete[i])
				current_terrain->info_rect[i].right = 0;
		}
	}
	return true;
}

static void put_roomdescs_in_dlog(cDialog& me, bool isTown, std::array<bool,16> str_do_delete) {
	int numDescs = isTown ? 16 : 8;
	for(int i = 0; i < numDescs; i++) {
		std::string id = std::to_string(i + 1);
		std::ostringstream str;
		bool active = true;
		if(isTown && town->room_rect[i].right == 0) active = false;
		if(!isTown && current_terrain->info_rect[i].right == 0) active = false;
		if(str_do_delete[i]) active = false;
		if(!active) {
			str << "Not yet placed.";
			me["del" + id].hide();
		} else if(isTown) {
			me["desc" + id].setText(town->room_rect[i].descr);
			str << "X = " << town->room_rect[i].left << ", Y = " << town->room_rect[i].top;
		} else {
			me["desc" + id].setText(current_terrain->info_rect[i].descr);
			str << "X = " << current_terrain->info_rect[i].left << ", Y = " << current_terrain->info_rect[i].top;
		}
		me["rect" + id].setText(str.str());
	}
}

static bool delete_roomdesc(cDialog& me, std::string id, bool isTown, std::array<bool,16> str_do_delete) {
	int item_hit = boost::lexical_cast<int>(id.substr(3));
	me["desc" + id.substr(3)].setText("");
	str_do_delete[item_hit - 1] = true;
	put_roomdescs_in_dlog(me, isTown, str_do_delete);
	return true;
}

void edit_roomdescs(bool town) {
	using namespace std::placeholders;
	std::array<bool,16> str_do_delete = {0};
	int numDescs = town ? 16 : 8;
	
	cDialog room_dlg(town ? "edit-town-roomdescs" : "edit-out-roomdescs");
	room_dlg["okay"].attachClickHandler(std::bind(save_roomdescs, _1, town, str_do_delete));
	room_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &room_dlg, false));
	for(int i = 0; i < numDescs; i++) {
		std::string id = std::to_string(i + 1);
		room_dlg["del" + id].attachClickHandler(std::bind(delete_roomdesc, _1, _2, town, str_do_delete));
//		room_dlg["str" + id].attachFocusHandler(check_roomdesc_len);
	}
	
	put_roomdescs_in_dlog(room_dlg, town, str_do_delete);
	
	room_dlg.run();
}

static bool save_town_num(cDialog& me, std::string, eKeyMod) {
	if(me.toast(true)) me.setResult<short>(me["town"].getTextAsNum());
	return true;
}

short pick_town_num(std::string which_dlog,short def,cScenario& scenario) {
	using namespace std::placeholders;
	
	cDialog town_dlg(which_dlog);
	town_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &town_dlg, false));
	town_dlg["okay"].attachClickHandler(save_town_num);
	town_dlg["town"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, scenario.towns.size() - 1, "Town number"));
	town_dlg["choose"].attachClickHandler([&scenario](cDialog& me, std::string, eKeyMod) -> bool {
		int i = me["town"].getTextAsNum();
		if(&scenario != &::scenario)
			scenario.towns.swap(::scenario.towns);
		i = choose_text(STRT_TOWN, i, &me, "Which town?");
		if(&scenario != &::scenario)
			scenario.towns.swap(::scenario.towns);
		me["town"].setTextToNum(i);
		return true;
	});
	
	town_dlg["town"].setTextToNum(def);
	std::string prompt = town_dlg["prompt"].getText();
	prompt += " (0 - " + std::to_string(scenario.towns.size() - 1) + ')';
	town_dlg["prompt"].setText(prompt);
	
	town_dlg.setResult<short>(-1);
	town_dlg.run();
	
	return town_dlg.getResult<short>();
}

static bool change_ter_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	std::string fld = item_hit.substr(7);
	short i = me[fld].getTextAsNum();
	i = choose_text(STRT_TER, i, &me, "Which Terrain?");
	if(i >= 0) me[fld].setTextToNum(i);
	return true;
}

static bool save_ter_change(cDialog& me, std::string, eKeyMod) {
	me.setResult(me.toast(true));
	return true;
}

bool change_ter(short& change_from,short& change_to,short& chance) {
	using namespace std::placeholders;
	cDialog chg_dlg("change-terrain");
	chg_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &chg_dlg, false));
	chg_dlg["okay"].attachClickHandler(save_ter_change);
	chg_dlg["chance"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 100, "The chance"));
	chg_dlg.attachClickHandlers(change_ter_event_filter, {"choose-from", "choose-to"});
	chg_dlg.attachFocusHandlers(std::bind(check_range, _1, _2, _3, 0, 255, "Both terrain types"), {"from", "to"});
	
	chg_dlg.setResult(false);
	chg_dlg.run();
	
	change_from = chg_dlg["from"].getTextAsNum();
	change_to = chg_dlg["to"].getTextAsNum();
	chance = chg_dlg["chance"].getTextAsNum();
	return chg_dlg.getResult<bool>();
}

static bool outdoor_details_event_filter(cDialog& me, std::string, eKeyMod) {
	if(!me.toast(true)) return true;
	current_terrain->out_name = me["name"].getText();
	current_terrain->comment = me["comment"].getText();
	current_terrain->bg_out = me["bg-out"].getTextAsNum();
	current_terrain->bg_fight = me["bg-fight"].getTextAsNum();
	current_terrain->bg_town = me["bg-town"].getTextAsNum();
	current_terrain->bg_dungeon = me["bg-dungeon"].getTextAsNum();
	return true;
}

void outdoor_details() {
	cDialog out_dlg("edit-outdoor-details");
	out_dlg["okay"].attachClickHandler(outdoor_details_event_filter);
	std::ostringstream str_out;
	str_out << "X = " << cur_out.x << ", Y = " << cur_out.y;
	out_dlg["loc"].setText(str_out.str());
	out_dlg["comment"].setText(current_terrain->comment);
	out_dlg["name"].setText(current_terrain->out_name);
	out_dlg["bg-out"].setTextToNum(current_terrain->bg_out);
	out_dlg["bg-fight"].setTextToNum(current_terrain->bg_fight);
	out_dlg["bg-town"].setTextToNum(current_terrain->bg_town);
	out_dlg["bg-dungeon"].setTextToNum(current_terrain->bg_dungeon);
	dynamic_cast<cLedGroup&>(out_dlg["ambient"]).setSelected("snd" + std::to_string(int(current_terrain->ambient_sound) + 1));
	out_dlg["ambient"].attachFocusHandler([](cDialog& me, std::string, bool) -> bool {
		cLedGroup& lg = dynamic_cast<cLedGroup&>(me["ambient"]);
		std::string hit = lg.getSelected();
		std::string prev = lg.getPrevSelection();
		eAmbientSound choice = eAmbientSound(hit[3] - '1');
		if(choice == AMBIENT_CUSTOM) {
			int i = prev == "snd4" ? current_terrain->out_sound : -1;
			i = choose_text(STRT_SND, i, &me, "Choose ambient sound:");
			if(i < 0) {
				lg.setSelected(prev);
				return false;
			}
			current_terrain->out_sound = i;
		}
		current_terrain->ambient_sound = choice;
		return true;
	});
	out_dlg.attachClickHandlers([](cDialog& me, std::string which, eKeyMod) -> bool {
		std::string fld = which.replace(0, 4, "bg");
		int bg_i = me[fld].getTextAsNum();
		bg_i = choose_background(bg_i, &me);
		me[fld].setTextToNum(bg_i);
		return true;
	}, {"pick-out", "pick-fight", "pick-town", "pick-dungeon"});
	using namespace std::placeholders;
	auto focus_handler = std::bind(check_range_msg, _1, _2, _3, -1, 21, _4, "-1 to use scenario default");
	out_dlg["bg-out"].attachFocusHandler(std::bind(focus_handler, _1, _2, _3, "Outdoors Background"));
	out_dlg["bg-fight"].attachFocusHandler(std::bind(focus_handler, _1, _2, _3, "Combat Background"));
	out_dlg["bg-town"].attachFocusHandler(std::bind(focus_handler, _1, _2, _3, "Default Town Background"));
	out_dlg["bg-dungeon"].attachFocusHandler(std::bind(focus_handler, _1, _2, _3, "Default Dungeon Background"));
	
	out_dlg.run();
}

static void put_out_wand_in_dlog(cDialog& me, short which, const cOutdoors::cWandering& wand) {
	short i;
	
	me["num"].setTextToNum(which);
	for(i = 0; i < 7; i++) {
		std::string id = "foe" + std::to_string(i + 1);
		if(wand.monst[i] == 0)
			me[id].setText("Empty");
		// TODO: Wait a second, if 0 is no monster, does that mean it's impossible to use monster 0? Should 1 be subtracted here?
		else me[id].setText(scenario.scen_monsters[wand.monst[i]].m_name);
	}
	for(i = 0; i < 3; i++) {
		std::string id = "ally" + std::to_string(i + 1);
		if(wand.friendly[i] == 0)
			me[id].setText("Empty");
		// TODO: Wait a second, if 0 is no monster, does that mean it's impossible to use monster 0? Should 1 be subtracted here?
		else me[id].setText(scenario.scen_monsters[wand.friendly[i]].m_name);
	}
	dynamic_cast<cLed&>(me["no-flee"]).setState(wand.cant_flee % 10 == 1 ? led_red : led_off);
	dynamic_cast<cLed&>(me["forced"]).setState(wand.cant_flee >= 10 ? led_red : led_off);
	me["onmeet"].setTextToNum(wand.spec_on_meet);
	me["onwin"].setTextToNum(wand.spec_on_win);
	me["onflee"].setTextToNum(wand.spec_on_flee);
	me["endx"].setTextToNum(wand.end_spec1);
	me["endy"].setTextToNum(wand.end_spec2);
}

static void save_out_wand(cDialog& me, short which, cOutdoors::cWandering& wand, short mode) {
	wand.spec_on_meet = me["onmeet"].getTextAsNum();
	wand.spec_on_win = me["onwin"].getTextAsNum();
	wand.spec_on_flee = me["onflee"].getTextAsNum();
	wand.end_spec1 = me["endx"].getTextAsNum();
	wand.end_spec2 = me["endy"].getTextAsNum();
	
	wand.cant_flee = 0;
	if(dynamic_cast<cLed&>(me["forced"]).getState() != led_off)
		wand.cant_flee += 10;
	if(dynamic_cast<cLed&>(me["no-flee"]).getState() != led_off)
		wand.cant_flee += 1;
	
	switch(mode) {
		case 0:
			current_terrain->wandering[which] = wand;
			break;
		case 1:
			current_terrain->special_enc[which] = wand;
			break;
	}
}

static bool edit_out_wand_event_filter(cDialog& me, std::string hit, short& which, cOutdoors::cWandering& wand, short mode) {
	if(!me.toast(true)) return true;
	save_out_wand(me, which, wand, mode);
	if(hit == "left") {
		me.untoast();
		which--;
		if(which < 0) which = 3;
		wand = (mode == 0) ? current_terrain->wandering[which] : current_terrain->special_enc[which];
		put_out_wand_in_dlog(me, which, wand);
	} else if(hit == "right") {
		me.untoast();
		which++;
		if(which > 3) which = 0;
		wand = (mode == 0) ? current_terrain->wandering[which] : current_terrain->special_enc[which];
		put_out_wand_in_dlog(me, which, wand);
	}
	return true;
}

static bool edit_out_wand_spec(cDialog& me, std::string hit, short which, cOutdoors::cWandering wand) {
	if(!me.toast(true)) return true;
	me.untoast();
	save_out_wand(me, which, wand, 100);
	std::string fld = "on" + hit.substr(5);
	short spec = me[fld].getTextAsNum();
	if(spec < 0)
		spec = get_fresh_spec(1);
	if(edit_spec_enc(spec,1,&me))
		me[fld].setTextToNum(spec);
	return true;
}

static bool edit_out_wand_monst(cDialog& me, std::string hit, short which, cOutdoors::cWandering wand) {
	if(!me.toast(true)) return true;
	save_out_wand(me, which, wand, 100);
	std::string fld = hit.substr(7);
	short i;
	if(fld[0] == 'f') {
		i = choose_text(STRT_MONST,wand.monst[fld[3] - '0'],&me,"Choose Which Monster:");
		if(i >= 0) wand.monst[fld[3] - '0'] = i;
	} else if(fld[0] == 'a') {
		i = choose_text(STRT_MONST,wand.friendly[fld[4] - '0'],&me,"Choose Which Monster:");
		if(i >= 0) wand.friendly[fld[4] - '0'] = i;
	}
	me[fld].setText(scenario.scen_monsters[i].m_name);
	return true;
}

// mode 0 - wandering 1 - special
void edit_out_wand(short mode) {
	using namespace std::placeholders;
	
	short which = 0;
	cOutdoors::cWandering wand = (mode == 0) ? current_terrain->wandering[0] : current_terrain->special_enc[0];
	
	cDialog wand_dlg("edit-outdoor-encounter");
	wand_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &wand_dlg, false));
	
	wand_dlg["endx"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 299, "First part of Stuff Done flag", "-1 if not used"));
	wand_dlg["endy"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 9, "Second part of Stuff Done flag", "-1 if not used"));
	wand_dlg.attachClickHandlers(std::bind(edit_out_wand_event_filter, _1, _2, std::ref(which), std::ref(wand), mode), {"okay", "left", "right"});
	wand_dlg.attachClickHandlers(std::bind(edit_out_wand_spec, _1, _2, which, std::ref(wand)), {"edit-meet", "edit-win", "edit-flee"});
	wand_dlg.attachClickHandlers(std::bind(edit_out_wand_monst, _1, _2, which, std::ref(wand)), {"choose-foe1", "choose-foe2", "choose-foe3", "choose-foe4", "choose-foe5", "choose-foe6", "choose-foe7", "choose-ally1", "choose-ally2", "choose-ally3"});
	wand_dlg.attachFocusHandlers(std::bind(check_range_msg, _1, _2, _3, -1, 59, "Outdoor Special Node", "-1 if not used"), {"onmeet", "onwin", "onflee"});
	
	if(mode == 1)
		wand_dlg["title"].setText("Outdoor Special Encounter:");
	
	put_out_wand_in_dlog(wand_dlg, which, wand);
	
	wand_dlg.run();
}

static bool save_town_details(cDialog& me, std::string, eKeyMod) {
	if(!me.toast(true)) return true;
	town->town_name = me["name"].getText();
	town->town_chop_time = me["chop"].getTextAsNum();
	town->town_chop_key = me["key"].getTextAsNum();
	town->max_num_monst = me["population"].getTextAsNum();
	town->difficulty = me["difficulty"].getTextAsNum();
	std::string lighting = dynamic_cast<cLedGroup&>(me["lighting"]).getSelected();
	if(lighting == "lit") town->lighting_type = LIGHT_NORMAL;
	else if(lighting == "dark") town->lighting_type = LIGHT_DARK;
	else if(lighting == "drains") town->lighting_type = LIGHT_DRAINS;
	else if(lighting == "no-light") town->lighting_type = LIGHT_NONE;
	cStack& comments = dynamic_cast<cStack&>(me["cmt"]);
	for(int i = 0; i < 3; i++) {
		comments.setPage(i);
		town->comment[i] = comments["comment"].getText();
	}
	return true;
}

static void put_town_details_in_dlog(cDialog& me) {
	me["name"].setText(town->town_name);
	me["chop"].setTextToNum(town->town_chop_time);
	me["key"].setTextToNum(town->town_chop_key);
	me["population"].setTextToNum(town->max_num_monst);
	me["difficulty"].setTextToNum(town->difficulty);
	cLedGroup& lighting = dynamic_cast<cLedGroup&>(me["lighting"]);
	switch(town->lighting_type) {
		case LIGHT_NORMAL: lighting.setSelected("lit"); break;
		case LIGHT_DARK: lighting.setSelected("dark"); break;
		case LIGHT_DRAINS: lighting.setSelected("drains"); break;
		case LIGHT_NONE: lighting.setSelected("no-light"); break;
	}
	cStack& comments = dynamic_cast<cStack&>(me["cmt"]);
	for(int i = 2; i >= 0; i--) {
		comments.setPage(i);
		comments["comment"].setText(town->comment[i]);
	}
}

void edit_town_details() {
	using namespace std::placeholders;
	cDialog town_dlg("edit-town-details");
	town_dlg["okay"].attachClickHandler(save_town_details);
	town_dlg["chop"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 10000, "The day the town becomes abandoned", "-1 if it doesn't"));
	town_dlg["key"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 10, "The event which prevents the town from becoming abandoned", "-1 or 0 for none"));
	town_dlg["difficulty"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, 0, 10, "The town difficulty", "0 - easiest, 10 - hardest"));
	town_dlg["pick-cmt"].attachFocusHandler([](cDialog& me, std::string id, bool losing) -> bool {
		if(losing) return true;
		int i = dynamic_cast<cLedGroup&>(me[id]).getSelected()[3] - '0' - 1;
		dynamic_cast<cStack&>(me["cmt"]).setPage(i);
		return true;
	});
	
	put_town_details_in_dlog(town_dlg);
	
	town_dlg.run();
}

static bool save_town_events(cDialog& me, std::string, eKeyMod) {
	if(!me.toast(true)) return true;
	for(int i = 0; i < 8; i++) {
		std::string id = std::to_string(i + 1);
		town->timer_spec_times[i] = me["time" + id].getTextAsNum();
		town->timer_specs[i] = me["spec" + id].getTextAsNum();
	}
	return true;
}

static void put_town_events_in_dlog(cDialog& me) {
	short i;
	for(i = 0; i < 8; i++) {
		std::string id = std::to_string(i + 1);
		me["time" + id].setTextToNum(town->timer_spec_times[i]);
		me["spec" + id].setTextToNum(town->timer_specs[i]);
	}
}

static bool edit_town_events_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	std::string id = item_hit.substr(4);
	short spec = me["spec" + id].getTextAsNum();
	if(spec < 0)
		spec = get_fresh_spec(2);
	if(edit_spec_enc(spec,2,&me))
		me["spec" + id].setTextToNum(spec);
	return true;
}

void edit_town_events() {
	using namespace std::placeholders;
	
	cDialog evt_dlg("edit-town-events");
	evt_dlg["okay"].attachClickHandler(save_town_events);
	evt_dlg.attachClickHandlers(edit_town_events_event_filter, {"edit1", "edit2", "edit3", "edit4", "edit5", "edit6", "edit7", "edit8"});
	evt_dlg.attachFocusHandlers(std::bind(check_range_msg, _1, _2, _3, -1, 99, "The town special node", "-1 for no special"), {"spec1", "spec2", "spec3", "spec4", "spec5", "spec6", "spec7", "spec8"});
	
	put_town_events_in_dlog(evt_dlg);
	
	evt_dlg.run();
}

static bool save_advanced_town(cDialog& me, std::string, eKeyMod) {
	if(!me.toast(true)) return true;
	for(int i = 0; i < 4; i++) {
		std::string id = std::to_string(i + 1);
		town->exit_specs[i] = me["onexit" + id].getTextAsNum();
		town->exit_locs[i].x = me["exit" + id + "-x"].getTextAsNum();
		town->exit_locs[i].y = me["exit" + id + "-y"].getTextAsNum();
	}
	town->spec_on_entry = me["onenter"].getTextAsNum();
	town->spec_on_entry_if_dead = me["onenterdead"].getTextAsNum();
	town->spec_on_hostile = me["onhostile"].getTextAsNum();
	town->bg_town = me["bg-town"].getTextAsNum();
	town->bg_fight = me["bg-fight"].getTextAsNum();
	town->is_hidden = dynamic_cast<cLed&>(me["hidden"]).getState() != led_off;
	town->defy_mapping = dynamic_cast<cLed&>(me["nomap"]).getState() != led_off;
	town->defy_scrying = dynamic_cast<cLed&>(me["noscry"]).getState() != led_off;
	town->strong_barriers = dynamic_cast<cLed&>(me["barrier"]).getState() != led_off;
	town->has_tavern = dynamic_cast<cLed&>(me["tavern"]).getState() != led_off;
	return true;
}

static void put_advanced_town_in_dlog(cDialog& me) {
	short i;
	for(i = 0; i < 4; i++) {
		std::string id = std::to_string(i + 1);
		me["onexit" + id].setTextToNum(town->exit_specs[i]);
		me["exit" + id + "-x"].setTextToNum(town->exit_locs[i].x);
		me["exit" + id + "-y"].setTextToNum(town->exit_locs[i].y);
	}
	me["onenter"].setTextToNum(town->spec_on_entry);
	me["onenterdead"].setTextToNum(town->spec_on_entry_if_dead);
	me["onhostile"].setTextToNum(town->spec_on_hostile);
	me["bg-town"].setTextToNum(town->bg_town);
	me["bg-fight"].setTextToNum(town->bg_fight);
	dynamic_cast<cLed&>(me["hidden"]).setState(town->is_hidden ? led_red : led_off);
	dynamic_cast<cLed&>(me["nomap"]).setState(town->defy_mapping ? led_red : led_off);
	dynamic_cast<cLed&>(me["noscry"]).setState(town->defy_scrying ? led_red : led_off);
	dynamic_cast<cLed&>(me["barrier"]).setState(town->strong_barriers ? led_red : led_off);
	dynamic_cast<cLed&>(me["tavern"]).setState(town->has_tavern ? led_red : led_off);
}

static bool edit_advanced_town_special(cDialog& me, std::string hit, eKeyMod) {
	std::string fld = hit.substr(5);
	short spec = me[fld].getTextAsNum();
	if(spec < 0)
		spec = get_fresh_spec(2);
	if(edit_spec_enc(spec,2,&me)) {
		me[fld].setTextToNum(spec);
	}
	return true;
}

void edit_advanced_town() {
	using namespace std::placeholders;
	
	cDialog town_dlg("edit-town-advanced");
	town_dlg["okay"].attachClickHandler(save_advanced_town);
	town_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &town_dlg, false));
	auto loc_check = std::bind(check_range_msg, _1, _2, _3, -1, 47, "The town exit coordinates", "-1 if you want them ignored");
	auto spec_check = std::bind(check_range_msg, _1, _2, _3, -1, 99, _4, "-1 for no special");
	town_dlg.attachFocusHandlers(std::bind(spec_check, _1, _2, _3, "The town exit special"), {"onexit1", "onexit2", "onexit3", "onexit4"});
	town_dlg.attachFocusHandlers(std::bind(spec_check, _1, _2, _3, "The town entry special"), {"onenter", "onenterdead"});
	town_dlg["onhostile"].attachFocusHandler(std::bind(spec_check, _1, _2, _3, "The town hostile special"));
	town_dlg.attachFocusHandlers(loc_check, {"exit1-x", "exit2-x", "exit3-x", "exit4-x"});
	town_dlg.attachFocusHandlers(loc_check, {"exit1-y", "exit2-y", "exit3-y", "exit4-y"});
	town_dlg.attachClickHandlers(edit_advanced_town_special, {"edit-onexit1", "edit-onexit2", "edit-onexit3", "edit-onexit4", "edit-onenter", "edit-onenterdead", "edit-onhostile"});
	town_dlg.attachClickHandlers([](cDialog& me, std::string which, eKeyMod) -> bool {
		std::string fld = which.replace(0, 4, "bg");
		int bg_i = me[fld].getTextAsNum();
		bg_i = choose_background(bg_i, &me);
		me[fld].setTextToNum(bg_i);
		return true;
	}, {"pick-fight", "pick-town"});
	using namespace std::placeholders;
	auto focus_handler = std::bind(check_range_msg, _1, _2, _3, -1, 21, _4, "-1 to use scenario default");
	town_dlg["bg-fight"].attachFocusHandler(std::bind(focus_handler, _1, _2, _3, "Combat Background"));
	town_dlg["bg-town"].attachFocusHandler(std::bind(focus_handler, _1, _2, _3, "Town Background"));
	
	put_advanced_town_in_dlog(town_dlg);
	
	town_dlg.run();
}

static bool save_town_wand(cDialog& me, std::string, eKeyMod) {
	if(!me.toast(true)) return true;
	for(int i = 0; i < 4; i++) {
		std::string base_id = "group" + std::to_string(i + 1) + "-monst";
		for(int j = 0; j < 4; j++) {
			std::string id = base_id + std::to_string(j + 1);
			town->wandering[i].monst[j] = me[id].getTextAsNum();
		}
	}
	return true;
}

static void put_town_wand_in_dlog(cDialog& me) {
	for(int i = 0; i < 4; i++) {
		std::string base_id = "group" + std::to_string(i + 1) + "-monst";
		for(int j = 0; j < 4; j++) {
			std::string id = base_id + std::to_string(j + 1);
			me[id].setTextToNum(town->wandering[i].monst[j]);
		}
	}
}

static bool edit_town_wand_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	static const char*const titles[4] = {
		"Choose First Monster: (1 appears)",
		"Choose Second Monster: (1 appears)",
		"Choose Third Monster: (1 appears)",
		"Choose Fourth Monster: (1-2 appears)",
	};
	short group = item_hit[6] - '0';
	std::string base_id = "group" + std::to_string(group) + "-monst";
	for(int i = 0; i < 4; i++) {
		std::string id = base_id + std::to_string(i + 1);
		short n = choose_text(STRT_MONST, town->wandering[group].monst[i], &me, titles[i]);
		if(n == town->wandering[group].monst[i]) break;
		me[id].setTextToNum(n);
	}
	return true;
}

void edit_town_wand() {
	using namespace std::placeholders;
	
	cDialog wand_dlg("edit-town-wandering");
	wand_dlg["okay"].attachClickHandler(save_town_wand);
	auto check_monst = std::bind(check_range_msg, _1, _2, _3, 0, 255, "Wandering monsters", "0 means no monster");
	// Just go through and attach the same focus handler to ALL text fields.
	// There's 16 of them, so this is kinda the easiest way to do it.
	wand_dlg.forEach([&check_monst](std::string, cControl& ctrl) {
		if(ctrl.getType() == CTRL_FIELD)
			ctrl.attachFocusHandler(check_monst);
		else if(ctrl.getText() == "Choose")
			ctrl.attachClickHandler(edit_town_wand_event_filter);
	});
	
	put_town_wand_in_dlog(wand_dlg);
	
	wand_dlg.run();
}

static void save_basic_dlog(cDialog& me, short& which) {
	auto& the_node = town->talking.people[which];
	the_node.title = me["title"].getText().substr(0,30);
	the_node.dunno = me["dunno"].getText();
	the_node.look = me["look"].getText();
	the_node.name = me["name"].getText();
	the_node.job = me["job"].getText();
}

static void put_basic_dlog_in_dlog(cDialog& me, const short which) {
	auto& the_node = town->talking.people[which];
	me["title"].setText(the_node.title);
	me["dunno"].setText(the_node.dunno);
	me["look"].setText(the_node.look);
	me["name"].setText(the_node.name);
	me["job"].setText(the_node.job);
	me["num"].setTextToNum(which + cur_town * 10);
}

static bool edit_basic_dlog_event_filter(cDialog& me, std::string hit, short& which) {
	if(hit == "left") {
		save_basic_dlog(me, which);
		which--;
		if(which < 0) which = 9;
		put_basic_dlog_in_dlog(me, which);
	} else { // right
		save_basic_dlog(me, which);
		which++;
		if(which > 9) which = 0;
		put_basic_dlog_in_dlog(me, which);
	}
	return true;
}

void edit_basic_dlog(short personality) {
	using namespace std::placeholders;
	
	cDialog person_dlg("edit-personality");
	person_dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, &person_dlg, true));
	person_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &person_dlg, false));
	person_dlg.attachClickHandlers(std::bind(edit_basic_dlog_event_filter, _1, _2, std::ref(personality)), {"left", "right"});
	
	put_basic_dlog_in_dlog(person_dlg, personality);
	
	person_dlg.run();
}

static bool check_talk_personality(cDialog& me, std::string item_hit, bool losing) {
	if(!losing) return true;
	int i = me[item_hit].getTextAsNum();
	if(i != -1 && i != -2 && i / 10 != cur_town) {
		std::ostringstream message;
		message << "The legal range for personalities in this town is from ";
		message << cur_town * 10 << " to " << cur_town * 10 + 9 << ".";
		giveError("Personalities in talk nodes must be -1 (for unused node), -2 (all personalities use) or in the legal range of personalities in this town.", message.str(), &me);
		return false;
	}
	return true;
}

static bool check_talk_key(cDialog& me, std::string item_hit, bool losing) {
	if(!losing) return true;
	std::string key = me[item_hit].getText();
	bool passes = true;
	if(key.length() != 4) passes = false;
	for(size_t i = 0; i < 4; i++) {
		if(i < key.length() && !islower(key[i]))
			passes = false;
	}
	if(!passes) {
		giveError("The words this node is the response to must be 4 characters long, and all characters must be lower case letters.", &me);
		return false;
	}
	return true;
}

static bool save_talk_node(cDialog& me, std::string item_hit, std::stack<short>& talk_edit_stack) {
	if(!me.toast(true)) return false;
	if(item_hit != "okay") me.untoast();
	
	cSpeech::cNode& talk_node = town->talking.talk_nodes[talk_edit_stack.top()];
	
	talk_node.personality = me["who"].getTextAsNum();
	
	std::string link = me["key1"].getText();
	std::copy(link.begin(), link.begin() + 4, talk_node.link1);
	link = me["key2"].getText();
	std::copy(link.begin(), link.begin() + 4, talk_node.link2);
	
	for(int i = 0; i < 4; i++)
		talk_node.extras[i] = me["extra" + std::to_string(i + 1)].getTextAsNum();
	
	int n;
	switch(talk_node.type) {
		case eTalkNode::DEP_ON_SDF: case eTalkNode::SET_SDF:
			if(cre(talk_node.extras[0],0,299,"First part of Stuff Done flag must be from 0 to 299.","",&me)) return false;
			if(cre(talk_node.extras[1],0,9,"Second part of Stuff Done flag must be from 0 to 9.","",&me)) return false;
			break;
		case eTalkNode::INN:
			if(cre(talk_node.extras[0],0,1000,"Inn cost must be from 0 to 1000.","",&me)) return false;
			if(cre(talk_node.extras[1],0,3,"Inn quality must be from 0 to 3.","",&me)) return false;
			break;
		case eTalkNode::DEP_ON_TIME_AND_EVENT:
			if(cre(talk_node.extras[1],0,9,"Event must be from 0 to 9. (0 means no event)","",&me)) return false;
			break;
		case eTalkNode::DEP_ON_TOWN:
			if(cre(talk_node.extras[0],0,scenario.towns.size(),"Town number must be from 0 to " + std::to_string(scenario.towns.size()) + ".","",&me)) return false;
			break;
		case eTalkNode::BUY_TOWN_LOC:
			if(cre(talk_node.extras[1],0,scenario.towns.size(),"Town number must be from 0 to " + std::to_string(scenario.towns.size()) + ".","",&me)) return false;
			break;
		case eTalkNode::SHOP:
			if(cre(talk_node.extras[0],0,6,"Cost adjustment must be from 0 (cheapest) to 6 (most expensive).","",&me)) return false;
			if(cre(talk_node.extras[3],0,11,"Shop type must be from 0 to 11.", "", &me)) return false;
			switch(talk_node.extras[3]) {
				case int(eShopType::ITEMS): n = scenario.scen_items.size(); break;
				case int(eShopType::MAGE): case int(eShopType::PRIEST): n = 62; break;
				case int(eShopType::ALCHEMY): n = 20; break;
				case int(eShopType::SKILLS): n = 18; break;
				default: n = 0; break;
			}
			if(n > 0) {
				if(cre(talk_node.extras[1],0,n,"First item in shop must be from 0 to " + std::to_string(n), "", &me)) return false;
				if(cre(talk_node.extras[1],1,30,"Shops cannot have more than 30 items.", "", &me)) return false;
				int last = talk_node.extras[1] + talk_node.extras[2] - 1;
				if(cre(last,0,n,"Number of items in shop cannot be such that the last item does not exist.", "", &me)) return false;
			}
			break;
		case eTalkNode::ENCHANT:
			if(cre(talk_node.extras[0],0,6,"Enchantment type must be from 0 to 6. See the documentation for a list of possible abilities.","",&me)) return false;
			break;
		case eTalkNode::BUY_SDF:
			if(cre(talk_node.extras[1],0,299,"First part of Stuff Done flag must be from 0 to 299.","",&me)) return false;
			if(cre(talk_node.extras[2],0,9,"Second part of Stuff Done flag must be from 0 to 9.","",&me)) return false;
			break;
		case eTalkNode::BUY_SHIP: case eTalkNode::BUY_HORSE:
			if(cre(talk_node.extras[1],0,29,"The first boat/horse must be in the legal range (0 - 29).","",&me)) return false;
			break;
		case eTalkNode::BUY_SPEC_ITEM:
			if(cre(talk_node.extras[0],0,49,"The special item must be in the legal range (0 - 49).","",&me)) return false;
			break;
		case eTalkNode::CALL_TOWN_SPEC:
			if(cre(talk_node.extras[0],-1,99,"The town special node called must be in the legal range (0 - 99), or -1 for No Special.","",&me)) return false;
			break;
		case eTalkNode::CALL_SCEN_SPEC:
			if(cre(talk_node.extras[0],-1,255,"The scenario special node called must be in the legal range (0 - 255), or -1 for No Special.","",&me)) return false;
			break;
		case eTalkNode::BUY_INFO: case eTalkNode::DEP_ON_TIME: case eTalkNode::REGULAR:
		case eTalkNode::END_ALARM: case eTalkNode::END_DIE: case eTalkNode::END_FIGHT: case eTalkNode::END_FORCE:
		case eTalkNode::IDENTIFY: case eTalkNode::SELL_ARMOR: case eTalkNode::SELL_ITEMS: case eTalkNode::SELL_WEAPONS:
		case eTalkNode::TRAINING:
			break;
	}
	
	talk_node.str1 = me["str1"].getText();
	talk_node.str2 = me["str2"].getText();
	
	town->talking.talk_nodes[talk_edit_stack.top()] = talk_node;
	return true;
}

static void put_talk_node_in_dlog(cDialog& me, std::stack<short>& talk_edit_stack) {
	cSpeech::cNode& talk_node = town->talking.talk_nodes[talk_edit_stack.top()];
	
	me["who"].setTextToNum(talk_node.personality);
	
	std::string link = "";
	for(int i = 0; i < 4; i++) link += talk_node.link1[i];
	me["key1"].setText(link);
	link = "";
	for(int i = 0; i < 4; i++) link += talk_node.link2[i];
	me["key2"].setText(link);
	
	int iDescBase = int(talk_node.type) * 7;
	static const char*const strIDs[] = {"type", "lblA", "lblB", "lblC", "lblD", "lbl1", "lbl2"};
	for(int i = 0; i < 7; i++)
		me[strIDs[i]].setText(get_str("talk-node-descs", iDescBase + i + 1));
	
	for(int i = 0; i < 4; i++)
		me["extra" + std::to_string(i + 1)].setTextToNum(talk_node.extras[i]);
	
	me["str1"].setText(talk_node.str1);
	me["str2"].setText(talk_node.str2);
	
	if(talk_node.type == eTalkNode::SHOP)
		me["chooseB"].show();
	else me["chooseB"].hide();
	if(talk_node.type != eTalkNode::CALL_TOWN_SPEC && talk_node.type != eTalkNode::CALL_SCEN_SPEC)
		me["chooseA"].hide();
	else me["chooseA"].show();
	
	if(talk_edit_stack.size() > 1)
		me["back"].show();
	else me["back"].hide();
}

static bool talk_node_back(cDialog& me, std::stack<short>& talk_edit_stack) {
	if(!save_talk_node(me, "back", talk_edit_stack)) return true;
	talk_edit_stack.pop();
	put_talk_node_in_dlog(me, talk_edit_stack);
	return true;
}

static bool talk_node_branch(cDialog& me, std::stack<short>& talk_edit_stack) {
	if(!save_talk_node(me, "new", talk_edit_stack)) return true;
	
	int spec = -1;
	for(int j = 0; j < 60; j++)
		if(town->talking.talk_nodes[j].personality == -1 && strnicmp(town->talking.talk_nodes[j].link1, "xxxx", 4) == 0) {
			spec = j;
			break;
		}
	
	if(spec < 0) {
		giveError("You have used all 60 available talk nodes. To create fresh dialogue, go back and reuse an old one.", &me);
		return true;
	}
	
	talk_edit_stack.push(spec);
	put_talk_node_in_dlog(me, talk_edit_stack);
	return true;
}

static bool select_talk_node_type(cDialog& me, std::stack<short>& talk_edit_stack) {
	short i = short(town->talking.talk_nodes[talk_edit_stack.top()].type);
	i = choose_text(STRT_TALK_NODE, i, &me, "What Talking Node type?");
	town->talking.talk_nodes[talk_edit_stack.top()].type = eTalkNode(i);
	put_talk_node_in_dlog(me, talk_edit_stack);
	return true;
}

static bool select_talk_node_value(cDialog& me, std::string item_hit, const std::stack<short>& talk_edit_stack) {
	const auto& talk_node = town->talking.talk_nodes[talk_edit_stack.top()];
	if(item_hit == "chooseB") {
		int i = me["extra2"].getTextAsNum();
		switch(talk_node.extras[3]) {
			case int(eShopType::MAGE):
				i = choose_text(STRT_MAGE,i,&me,"What is the first mage spell in the shop?");
				break;
			case int(eShopType::PRIEST):
				i = choose_text(STRT_PRIEST,i,&me,"What is the first priest spell in the shop?");
				break;
			case int(eShopType::ALCHEMY):
				i = choose_text(STRT_ALCHEMY,i,&me,"What is the first recipe in the shop?");
				break;
			case int(eShopType::ITEMS):
				i = choose_text(STRT_ITEM,i,&me,"What is the first item in the shop?");
				break;
			case int(eShopType::SKILLS):
				i = choose_text(STRT_SKILL,i,&me,"What is the first skill in the shop?");
				break;
			default:
				giveError("The shop type you've chosen doesn't support item customization!", &me);
				break;
		}
		me["extra1"].setTextToNum(i);
	} else if(item_hit == "chooseA") {
		int spec = me["extra1"].getTextAsNum();
		int mode = talk_node.type == eTalkNode::CALL_TOWN_SPEC ? 2 : 0;
		if(spec < 0)
			spec = get_fresh_spec(mode);
		if(edit_spec_enc(spec,mode,&me))
			me["extra1"].setTextToNum(spec);
	}
	return true;
}

void edit_talk_node(short which_node) {
	using namespace std::placeholders;
	
	std::stack<short> talk_edit_stack;
	talk_edit_stack.push(which_node);
	cSpeech::cNode talk_node = town->talking.talk_nodes[which_node];
	
	cDialog talk_dlg("edit-talk-node");
	talk_dlg["okay"].attachClickHandler(std::bind(save_talk_node, _1, _2, std::ref(talk_edit_stack)));
	talk_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &talk_dlg, false));
	talk_dlg["back"].attachClickHandler(std::bind(talk_node_back, _1, std::ref(talk_edit_stack)));
	talk_dlg["new"].attachClickHandler(std::bind(talk_node_branch, _1, std::ref(talk_edit_stack)));
	talk_dlg["choose-type"].attachClickHandler(std::bind(select_talk_node_type, _1, std::ref(talk_edit_stack)));
	talk_dlg.attachClickHandlers(std::bind(select_talk_node_value, _1, _2, std::ref(talk_edit_stack)), {"chooseA", "chooseB"});
	talk_dlg["who"].attachFocusHandler(check_talk_personality);
	talk_dlg.attachFocusHandlers(check_talk_key, {"key1", "key2"});
	
	put_talk_node_in_dlog(talk_dlg, talk_edit_stack);
	
	talk_dlg.run();
}

static void put_out_loc_in_dlog(cDialog& me, location cur_loc) {
	std::ostringstream str;
	str << "X = " << cur_loc.x;
	me["x"].setText(str.str());
	str.str("");
	str << "Y = " << cur_loc.y;
	me["y"].setText(str.str());
	me["title"].setText(scenario.outdoors[cur_loc.x][cur_loc.y]->out_name);
}

static bool pick_out_event_filter(cDialog& me, std::string item_hit, location& cur_loc) {
	if(item_hit == "xminus") {
		if(cur_loc.x == 0) beep();
		else cur_loc.x--;
	} else if(item_hit == "xplus") {
		if(cur_loc.x >= scenario.outdoors.width() - 1) beep();
		else cur_loc.x++;
	} else if(item_hit == "yminus") {
		if(cur_loc.y == 0) beep();
		else cur_loc.y--;
	} else if(item_hit == "yplus") {
		if(cur_loc.y >= scenario.outdoors.height() - 1) beep();
		else cur_loc.y++;
	} else if(item_hit == "choose") {
		int i = cur_loc.x * scenario.outdoors.width() + cur_loc.y;
		i = choose_text(STRT_SECTOR, i, &me, "Which sector?");
		cur_loc.x = i / scenario.outdoors.width();
		cur_loc.y = i % scenario.outdoors.width();
	}
	put_out_loc_in_dlog(me, cur_loc);
	return true;
}

static bool finish_pick_out(cDialog& me, bool okay, location& cur_loc, location orig_loc) {
	if(me.toast(okay)) {
		if(!okay) cur_loc = orig_loc;
	}
	return true;
}

location pick_out(location default_loc) {
	using namespace std::placeholders;
	location prev_loc = default_loc;
	
	cDialog out_dlg("select-sector");
	out_dlg["okay"].attachClickHandler(std::bind(finish_pick_out, _1, true, std::ref(default_loc), prev_loc));
	out_dlg["cancel"].attachClickHandler(std::bind(finish_pick_out, _1, false, std::ref(default_loc), prev_loc));
	out_dlg.attachClickHandlers(std::bind(pick_out_event_filter, _1, _2, std::ref(default_loc)), {"xplus", "xminus", "yplus", "yminus", "choose"});
	
	out_dlg["width"].setTextToNum(scenario.outdoors.width());
	out_dlg["height"].setTextToNum(scenario.outdoors.height());
	put_out_loc_in_dlog(out_dlg, default_loc);
	
	out_dlg.run();
	return default_loc;
}

bool new_town(short which_town) {
	std::cout << "Town creation currently disabled.\n";
	short i,j;
	
	cChoiceDlog new_dlg("new-town", {"okay", "cancel"});
	new_dlg->getControl("num").setTextToNum(which_town);
	if(new_dlg.show() == "cancel") return false;
	
	std::string size = dynamic_cast<cLedGroup&>(new_dlg->getControl("size")).getSelected();
	std::string preset = dynamic_cast<cLedGroup&>(new_dlg->getControl("preset")).getSelected();
	
	if(size == "lg") scenario.towns.push_back(new cBigTown(scenario));
	else if(size == "med") scenario.towns.push_back(new cMedTown(scenario));
	else if(size == "sm") scenario.towns.push_back(new cTinyTown(scenario));
	
	cur_town = which_town;
	town = scenario.towns[cur_town];
	scenario.last_town_edited = cur_town;
	town->town_name = new_dlg->getControl("name").getText().substr(0,30);
	
	for(i = 0; i < town->max_dim(); i++)
		for(j = 0; j < town->max_dim(); j++)
			if(preset == "cave") {
				town->terrain(i,j) = 0;
			} else {
				town->terrain(i,j) = 2;
				if(preset == "flowers") {
					if(get_ran(1,0,8) == 0)
						town->terrain(i,j) = 3;
					else if(get_ran(1,0,10) == 0)
						town->terrain(i,j) = 4;
				}
			}
	
	return true;
}

// before calling this, be sure to do all checks to make sure it's safe.
void delete_last_town() {
	cTown* last_town = scenario.towns.back();
	scenario.towns.pop_back();
	delete last_town;
}

cTown* pick_import_town(short def) {
	cScenario temp_scenario;
	fs::path path = nav_get_scenario();
	load_scenario(path, temp_scenario);
	short town_num = pick_town_num("select-import-town", def, temp_scenario);
	if(town_num < 0) return nullptr;
	cTown* town = temp_scenario.towns[town_num];
	temp_scenario.towns[town_num] = nullptr;
	return town;
}
