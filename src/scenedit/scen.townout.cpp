
#include <cstdio>
#include <cstring>
#include <stack>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include "scen.global.hpp"
#include "scenario/scenario.hpp"
#include "scenario/town.hpp"
#include "utility.hpp"
#include "scen.actions.hpp"
#include "scen.graphics.hpp"
#include "scen.townout.hpp"
#include "scen.keydlgs.hpp"
#include "scen.locpicker.hpp"
#include "scen.sdfpicker.hpp"
#include "scen.fileio.hpp"
#include "scen.core.hpp"
#include "scen.undo.hpp"
#include "mathutil.hpp"
#include "dialogxml/widgets/button.hpp"
#include "dialogxml/widgets/field.hpp"
#include "dialogxml/dialogs/strchoice.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "tools/winutil.hpp"
#include "dialogxml/widgets/ledgroup.hpp"
#include "dialogxml/widgets/stack.hpp"
#include "dialogxml/widgets/scrollpane.hpp"
#include "fileio/fileio.hpp"
#include "fileio/resmgr/res_dialog.hpp"

using boost::algorithm::trim;
using boost::algorithm::to_lower;

extern short cen_x, cen_y;
extern eScenMode overall_mode;
extern bool mouse_button_held,change_made;
extern short cur_viewing_mode;
extern cTown* town;
extern short mode_count,to_create,cur_town;
extern ter_num_t template_terrain[64][64];
extern cScenario scenario;
extern cOutdoors* current_terrain;
extern location cur_out;
extern cUndoList undo_list;
extern std::string help_text_rsrc;
extern bool editing_town;
extern bool last_shift_continuous;
extern void update_edit_menu();

const char *day_str_1[] = {"Unused","Day creature appears","Day creature disappears",
	"Unused","Unused","Unused","Unused","Unused","Unused"};
const char *day_str_2[] = {"Unused","Event code (0 - no event)","Event code (0 - no event)",
	"Unused","Unused","Unused",
	"Event code (0 - no event)","Event code (0 - no event)","Unused"};

static void put_placed_monst_in_dlog(cDialog& me, cTownperson& monst, const short which) {
	me["num"].setTextToNum(which);
	me["type"].setText(scenario.scen_monsters[monst.number].m_name);
	dynamic_cast<cLedGroup&>(me["attitude"]).setSelected(boost::lexical_cast<std::string>(monst.start_attitude));
	dynamic_cast<cLedGroup&>(me["mobility"]).setSelected("mob" + std::to_string(monst.mobility + 1));
	me["talk"].setTextToNum(monst.personality);
	me["picnum"].setTextToNum(monst.facial_pic);
 	if(short(monst.facial_pic) < 0)
		dynamic_cast<cPict&>(me["pic"]).setPict(scenario.scen_monsters[monst.number].picture_num, PIC_MONST);
	else if((monst.facial_pic >= 1000))
		dynamic_cast<cPict&>(me["pic"]).setPict(monst.facial_pic - 1000,PIC_CUSTOM_TALK);
	else dynamic_cast<cPict&>(me["pic"]).setPict(monst.facial_pic,PIC_TALK);
	me["loc"].setText(boost::lexical_cast<std::string>(monst.start_loc));
}

static void get_placed_monst_in_dlog(cDialog& me, cTownperson& store_placed_monst) {
	store_placed_monst.start_attitude = boost::lexical_cast<eAttitude>(dynamic_cast<cLedGroup&>(me["attitude"]).getSelected());
	store_placed_monst.mobility = dynamic_cast<cLedGroup&>(me["mobility"]).getSelected()[3] - '1';
	store_placed_monst.personality = me["talk"].getTextAsNum();
	store_placed_monst.facial_pic = me["picnum"].getTextAsNum();
	store_placed_monst.start_loc = boost::lexical_cast<location>(me["loc"].getText());
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
		i = choose_text(STRT_MONST,monst.number-1,&me,"Choose Which Monster:") + 1;
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
	} else if(hit == "pick-loc") {
		cArea* area = get_current_area();
		cLocationPicker picker(monst.start_loc, *area, "Move Creature", &me);
		monst.start_loc = picker.run();
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
	location old_loc = monst.start_loc;
	
	cDialog edit(*ResMgr::dialogs.get("edit-townperson"));
	edit.attachClickHandlers(std::bind(edit_placed_monst_event_filter, _1, _2, std::ref(monst), which_m), {"type-edit", "pict-edit", "talk-edit", "pick-loc", "okay", "cancel", "more", "del"});
	
	put_placed_monst_in_dlog(edit, monst, which_m);
	
	edit.run();
	if(monst.start_loc != old_loc){
		// Move editor view to keep showing monster
		cen_x = monst.start_loc.x;
		cen_y = monst.start_loc.y;
	}
}

static void put_placed_monst_adv_in_dlog(cDialog& me, cTownperson& monst, const short which) {
	me["num"].setTextToNum(which);
	me["type"].setText(scenario.scen_monsters[monst.number].m_name);
	int iTime = 0;
	bool need_event = false;
	switch(monst.time_flag) {
		case eMonstTime::ALWAYS: iTime = 0; break;
		case eMonstTime::APPEAR_ON_DAY: iTime = 1; need_event = true; break;
		case eMonstTime::DISAPPEAR_ON_DAY: iTime = 2; need_event = true; break;
		case eMonstTime::SOMETIMES_A: iTime = 3; break;
		case eMonstTime::SOMETIMES_B: iTime = 4; break;
		case eMonstTime::SOMETIMES_C: iTime = 5; break;
		case eMonstTime::APPEAR_WHEN_EVENT: iTime = 6; need_event = true; break;
		case eMonstTime::DISAPPEAR_WHEN_EVENT: iTime = 7; need_event = true; break;
		case eMonstTime::APPEAR_AFTER_CHOP: iTime = 8; break;
	}
	dynamic_cast<cLedGroup&>(me["time"]).setSelected("time" + std::to_string(iTime + 1));
	me["extra1-lbl"].setText(day_str_1[iTime]);
	me["extra2-lbl"].setText(day_str_2[iTime]);
	me["extra1"].setTextToNum(monst.monster_time);
	me["extra2"].setTextToNum(monst.time_code);
	me["group"].setTextToNum(monst.spec_enc_code);
	me["death"].setTextToNum(monst.special_on_kill);
	me["hail"].setTextToNum(monst.special_on_talk);
	me["sdfy"].setTextToNum(monst.spec1);
	me["sdfx"].setTextToNum(monst.spec2);
	if(need_event) {
		me["choose-event"].show();
	} else {
		me["choose-event"].hide();
	}
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
  	if(cre(monst.special_on_kill,-1,town->specials.size(),"Town special node number must be from 0 to 99 (or -1 for no special).","",&me)) return false;
	monst.special_on_talk = me["hail"].getTextAsNum();
  	if(cre(monst.special_on_talk,-1,town->specials.size(),"Town special node number must be from 0 to 99 (or -1 for no special).","",&me)) return false;
	monst.spec1 = me["sdfy"].getTextAsNum();
  	if(cre(monst.spec1,-1,SDF_ROWS - 1,"First part of special flag must be -1 (if this is to be ignored) or from 0 to 299.","",&me)) return false;
	monst.spec2 = me["sdfx"].getTextAsNum();
  	if(cre(monst.spec2,-1,SDF_ROWS - 1,"Second part of special flag must be -1 (if this is to be ignored) or from 0 to 9.","",&me)) return false;
	
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
	if(item_hit == 1 || item_hit == 2 || item_hit == 6 || item_hit == 7) {
		me["choose-event"].show();
	} else {
		me["choose-event"].hide();
	}
	return true;
}

static bool edit_placed_monst_adv_death(cDialog& me) {
	short spec = me["death"].getTextAsNum();
	if(spec < 0)
		spec = get_fresh_spec(2);
	if(edit_spec_enc(spec,2,&me))
		me["death"].setTextToNum(spec);
	return true;
}

static bool edit_placed_monst_adv_hail(cDialog& me) {
	short spec = me["hail"].getTextAsNum();
	if(spec < 0)
		spec = get_fresh_spec(2);
	if(edit_spec_enc(spec,2,&me))
		me["hail"].setTextToNum(spec);
	return true;
}

cTownperson edit_placed_monst_adv(cTownperson initial, short which, cDialog& parent) {
	using namespace std::placeholders;
	
	cDialog edit(*ResMgr::dialogs.get("edit-townperson-advanced"), &parent);
	edit["okay"].attachClickHandler(std::bind(edit_placed_monst_adv_event_filter, _1, _2, std::ref(initial)));
	edit["cancel"].attachClickHandler(std::bind(edit_placed_monst_adv_event_filter, _1, _2, std::ref(initial)));
	edit["editdeath"].attachClickHandler(std::bind(edit_placed_monst_adv_death, _1));
	edit["edithail"].attachClickHandler(std::bind(edit_placed_monst_adv_hail, _1));
	edit["time"].attachFocusHandler(edit_placed_monst_adv_time_flag);
	edit["choose-event"].attachClickHandler([](cDialog& me, std::string, eKeyMod) {
		int value = me["extra2"].getTextAsNum();
		value = choose_text_editable(scenario.evt_names, value, &me, "Select an event:");
		me["extra2"].setTextToNum(value);
		return true;
	});
	edit["editsdf"].attachClickHandler([](cDialog& me, std::string, eKeyMod) {
		location sdf(me["sdfx"].getTextAsNum(), me["sdfy"].getTextAsNum());
		sdf = cStuffDonePicker(sdf).run();
		me["sdfx"].setTextToNum(sdf.x);
		me["sdfy"].setTextToNum(sdf.y);
		return true;
	});
	
	put_placed_monst_adv_in_dlog(edit,initial,which);
	
	edit.run();
	
	return initial;
}

// Store the state of the special flag LEDs in case put_placed_item_in_dlog() is called again when a picker finishes
// and would wipe them out
static void store_item_special_flags(cDialog& me, cTown::cItem& item) {
	item.always_there = dynamic_cast<cLed&>(me["always"]).getState() != led_off;
	item.property = dynamic_cast<cLed&>(me["owned"]).getState() != led_off;
	item.contained = dynamic_cast<cLed&>(me["contained"]).getState() != led_off;
}

static bool put_placed_item_in_dlog(cDialog& me, const cTown::cItem& item, const short which) {
	cItem base = scenario.scen_items[item.code];
	if(item.ability != eEnchant::NONE && (base.variety == eItemType::ONE_HANDED || base.variety == eItemType::TWO_HANDED)) {
		base.enchant_weapon(item.ability);
	}
	
	me["num"].setTextToNum(which);
	me["loc"].setText(boost::lexical_cast<std::string>(item.loc));

	me["name"].setText(base.full_name);
	me["charges"].setTextToNum(item.charges);
	me["abil"].setTextToNum(int(item.ability));
	dynamic_cast<cLed&>(me["always"]).setState(item.always_there ? led_red : led_off);
	dynamic_cast<cLed&>(me["owned"]).setState(item.property ? led_red : led_off);
	dynamic_cast<cLed&>(me["contained"]).setState(item.contained ? led_red : led_off);
	
	dynamic_cast<cPict&>(me["pic"]).setPict(base.graphic_num, PIC_ITEM);
	
	me["charges"].show();
	me["charges-lbl"].show();
	me["abil"].show();
	me["abil-lbl"].show();
	me["abil-choose"].show();
	
	if(base.variety == eItemType::GOLD || base.variety == eItemType::FOOD) {
		me["charges-lbl"].setText("Amount:");
	} else if(base.charges == 0 && (*item.ability).charges == 0) {
		me["charges-lbl"].hide();
		me["charges"].hide();
	} else {
		me["charges-lbl"].setText("Charges:|(-1 = default)");
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
		showError("Number of charges/amount of gold or food must be from 0 to 2500.",
				  "If an item with charges (not gold or food) leave this at -1 for the item to have the default number of charges.",&me);
		return true;
	}
	
	eItemType type = scenario.scen_items[item.code].variety;
	if(item.charges == 0 && (type == eItemType::GOLD || type == eItemType::FOOD)) {
		showError("You must assign gold or food an amount of at least 1.","",&me);
		return false;
	}
	
	item.loc = boost::lexical_cast<location>(me["loc"].getText());
	store_item_special_flags(me, item);
	int ench = me["abil"].getTextAsNum();
	if(ench >= 0 && ench <= cEnchant::MAX) item.ability = eEnchant(ench);
	
	town->preset_items[which] = item;
	return true;
}

static bool edit_placed_item_type(cDialog& me, cTown::cItem& item, const short which) {
	short i = choose_text(STRT_ITEM, item.code, &me, "Place which item?");
	if(i >= 0) {
		item.code = i;
		store_item_special_flags(me, item);
		put_placed_item_in_dlog(me, item, which);
	}
	return true;
}

static bool edit_placed_item_abil(cDialog& me, std::string item_hit, cTown::cItem& item, const short which) {
	item.charges = me["charges"].getTextAsNum();
	cItem& base = scenario.scen_items[item.code];
	short i = short(item.ability);
	if(item_hit == "abil") { // User entered a number directly
		i = me["abil"].getTextAsNum();
	} else if(base.variety == eItemType::ONE_HANDED || base.variety == eItemType::TWO_HANDED) {
		i = choose_text(STRT_ENCHANT, i, &me, "Which enchantment?");
	}
	if(i >= -1 && i <= cEnchant::MAX) item.ability = eEnchant(i);
	store_item_special_flags(me, item);
	put_placed_item_in_dlog(me, item, which);
	return true;
}

static bool edit_placed_item_loc(cDialog& me, std::string item_hit, cTown::cItem& item, const short which) {
	cArea* area = get_current_area();
	cLocationPicker picker(item.loc, *area, "Move Item", &me);
	item.loc = picker.run();
	store_item_special_flags(me, item);
	item.contained = container_there(item.loc);
	put_placed_item_in_dlog(me, item, which);
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
	location old_loc = item.loc;
	
	cDialog item_dlg(*ResMgr::dialogs.get("edit-placed-item"));
	item_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &item_dlg, false));
	item_dlg["okay"].attachClickHandler(std::bind(get_placed_item_in_dlog, _1, std::ref(item), which_i));
	item_dlg["choose"].attachClickHandler(std::bind(edit_placed_item_type, _1, std::ref(item), which_i));
	item_dlg["del"].attachClickHandler(std::bind(edit_placed_item_delete, _1, which_i));
	item_dlg["abil-choose"].attachClickHandler(std::bind(edit_placed_item_abil, _1, _2, std::ref(item), which_i));
	item_dlg["abil"].attachFocusHandler(std::bind(edit_placed_item_abil, _1, _2, std::ref(item), which_i));
	item_dlg["pick-loc"].attachClickHandler(std::bind(edit_placed_item_loc, _1, _2, std::ref(item), which_i));
	put_placed_item_in_dlog(item_dlg, item, which_i);
	
	item_dlg.run();

	if(item.loc != old_loc){
		// Move editor view to keep showing item
		cen_x = item.loc.x;
		cen_y = item.loc.y;
	}
}

static bool edit_sign_event_filter(cDialog& me, sign_loc_t& which_sign) {
	if(!me.toast(true)) return true;
	which_sign.text = me["text"].getText();
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

void edit_sign(sign_loc_t& which_sign,short num,short picture) {
	using namespace std::placeholders;
	location view_loc;
	
	cDialog sign_dlg(*ResMgr::dialogs.get("edit-sign"));
	sign_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &sign_dlg, false));
	sign_dlg["okay"].attachClickHandler(std::bind(edit_sign_event_filter, _1, std::ref(which_sign)));
	cPict& icon = dynamic_cast<cPict&>(sign_dlg["pic"]);
	if(picture >= 960 && picture < 1000)
		icon.setPict(picture, PIC_TER_ANIM);
	else if(picture >= 2000)
		icon.setPict(picture - 2000, PIC_CUSTOM_TER_ANIM);
	else icon.setPict(picture, PIC_TER); // automatically adjusts for custom graphics
	
	sign_dlg["num"].setTextToNum(num);
	sign_dlg["text"].setText(which_sign.text);
	
	sign_dlg.run();
}

static bool save_town_num(cDialog& me, std::string, eKeyMod) {
	using namespace std::placeholders;
	me["town"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, scenario.towns.size() - 1, "Town number"));
	if(me.toast(true)) me.setResult<short>(me["town"].getTextAsNum());
	return true;
}

static bool create_town_num(cDialog& me, std::string, eKeyMod) {
	using namespace std::placeholders;
	me["town"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, scenario.towns.size(), "Town number"));
	if(me.toast(true)){
		int i = me["town"].getTextAsNum();
		if(i == scenario.towns.size()){
			// create town
			if(scenario.towns.size() >= 200) {
				showError("You have reached the limit of 200 towns you can have in one scenario.");
				return true;
			}
			if(!new_town()){
				// Create town canceled -- don't store the number because it is out of range
				return true;
			}
		}
		cur_town = i;
		town = scenario.towns[cur_town];
		start_town_edit();
		me.setResult<short>(i);
	}
	return true;
}

short pick_town_num(std::string which_dlog,short def,cScenario& scenario) {
	
	cDialog town_dlg(*ResMgr::dialogs.get(which_dlog));
	town_dlg["prompt"].replaceText("{{max-num}}", std::to_string(scenario.towns.size() - 1));
	town_dlg["prompt"].replaceText("{{next-num}}", std::to_string(scenario.towns.size()));
	town_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &town_dlg, false));
	town_dlg["okay"].attachClickHandler(save_town_num);
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
	if(town_dlg.hasControl("edit")){
		town_dlg["edit"].attachClickHandler(create_town_num);
	}
	
	town_dlg["town"].setTextToNum(def);
	
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
	cDialog chg_dlg(*ResMgr::dialogs.get("change-terrain"));
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
	current_terrain->name = me["name"].getText();
	current_terrain->comment = me["comment"].getText();
	current_terrain->bg_out = me["bg-out"].getTextAsNum();
	current_terrain->bg_fight = me["bg-fight"].getTextAsNum();
	current_terrain->bg_town = me["bg-town"].getTextAsNum();
	current_terrain->bg_dungeon = me["bg-dungeon"].getTextAsNum();
	return true;
}

void outdoor_details() {
	cDialog out_dlg(*ResMgr::dialogs.get("edit-outdoor-details"));
	out_dlg["okay"].attachClickHandler(outdoor_details_event_filter);
	out_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &out_dlg, false));
	std::ostringstream str_out;
	str_out << "X = " << cur_out.x << ", Y = " << cur_out.y;
	out_dlg["loc"].setText(str_out.str());
	out_dlg["comment"].setText(current_terrain->comment);
	out_dlg["name"].setText(current_terrain->name);
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
			i = choose_sound(i, &me, "Choose ambient sound:");
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
	me["num"].setTextToNum(which);
	for(short i = 0; i < 7; i++) {
		std::string id = "foe" + std::to_string(i + 1);
		if(wand.monst[i] == 0)
			me[id].setText("Empty");
		// TODO: Wait a second, if 0 is no monster, does that mean it's impossible to use monster 0? Should 1 be subtracted here?
		else me[id].setText(scenario.scen_monsters[wand.monst[i]].m_name);
	}
	for(short i = 0; i < 3; i++) {
		std::string id = "ally" + std::to_string(i + 1);
		if(wand.friendly[i] == 0)
			me[id].setText("Empty");
		// TODO: Wait a second, if 0 is no monster, does that mean it's impossible to use monster 0? Should 1 be subtracted here?
		else me[id].setText(scenario.scen_monsters[wand.friendly[i]].m_name);
	}
	dynamic_cast<cLed&>(me["no-flee"]).setState(wand.cant_flee ? led_red : led_off);
	dynamic_cast<cLed&>(me["forced"]).setState(wand.forced ? led_red : led_off);
	me["onmeet"].setTextToNum(wand.spec_on_meet);
	me["onwin"].setTextToNum(wand.spec_on_win);
	me["onflee"].setTextToNum(wand.spec_on_flee);
	me["endy"].setTextToNum(wand.end_spec1);
	me["endx"].setTextToNum(wand.end_spec2);
}

static void save_out_wand(cDialog& me, short which, cOutdoors::cWandering& wand, short mode) {
	wand.spec_on_meet = me["onmeet"].getTextAsNum();
	wand.spec_on_win = me["onwin"].getTextAsNum();
	wand.spec_on_flee = me["onflee"].getTextAsNum();
	wand.end_spec1 = me["endy"].getTextAsNum();
	wand.end_spec2 = me["endx"].getTextAsNum();
	
	wand.forced = dynamic_cast<cLed&>(me["forced"]).getState() != led_off;
	wand.cant_flee = dynamic_cast<cLed&>(me["no-flee"]).getState() != led_off;
	
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
	size_t num_enc = (mode == 0) ? current_terrain->wandering.size() : current_terrain->special_enc.size();
	if(hit == "left") {
		me.untoast();
		which--;
		if(which < 0) which = num_enc - 1;
		wand = (mode == 0) ? current_terrain->wandering[which] : current_terrain->special_enc[which];
		put_out_wand_in_dlog(me, which, wand);
	} else if(hit == "right") {
		me.untoast();
		which++;
		if(which >= num_enc) which = 0;
		wand = (mode == 0) ? current_terrain->wandering[which] : current_terrain->special_enc[which];
		put_out_wand_in_dlog(me, which, wand);
	}
	return true;
}

static bool edit_out_wand_spec(cDialog& me, std::string hit, short which, cOutdoors::cWandering& wand) {
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

static bool edit_out_wand_monst(cDialog& me, std::string hit, short which, cOutdoors::cWandering& wand) {
	if(!me.toast(true)) return true;
	me.untoast();
	save_out_wand(me, which, wand, 100);
	std::string fld = hit.substr(7);
	short i = 0;
	if(fld[0] == 'f') {
		i = choose_text(STRT_MONST,wand.monst[fld[3] - '1']-1,&me,"Choose Which Monster:") + 1;
		if(i >= 0) wand.monst[fld[3] - '1'] = i;
	} else if(fld[0] == 'a') {
		i = choose_text(STRT_MONST,wand.friendly[fld[4] - '1']-1,&me,"Choose Which Monster:") + 1;
		if(i >= 0) wand.friendly[fld[4] - '1'] = i;
	}
	me[fld].setText(scenario.scen_monsters[i].m_name);
	return true;
}

// mode 0 - wandering 1 - special
void edit_out_wand(short mode) {
	using namespace std::placeholders;
	
	short which = 0;
	cOutdoors::cWandering wand = (mode == 0) ? current_terrain->wandering[0] : current_terrain->special_enc[0];
	
	cDialog wand_dlg(*ResMgr::dialogs.get("edit-outdoor-encounter"));
	wand_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &wand_dlg, false));
	
	wand_dlg["endx"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, SDF_COLUMNS - 1, "First part of Stuff Done flag", "-1 if not used"));
	wand_dlg["endy"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, SDF_ROWS - 1, "Second part of Stuff Done flag", "-1 if not used"));
	wand_dlg.attachClickHandlers(std::bind(edit_out_wand_event_filter, _1, _2, std::ref(which), std::ref(wand), mode), {"okay", "left", "right"});
	wand_dlg.attachClickHandlers(std::bind(edit_out_wand_spec, _1, _2, which, std::ref(wand)), {"edit-meet", "edit-win", "edit-flee"});
	wand_dlg.attachClickHandlers(std::bind(edit_out_wand_monst, _1, _2, which, std::ref(wand)), {"choose-foe1", "choose-foe2", "choose-foe3", "choose-foe4", "choose-foe5", "choose-foe6", "choose-foe7", "choose-ally1", "choose-ally2", "choose-ally3"});
	wand_dlg.attachFocusHandlers(std::bind(check_range_msg, _1, _2, _3, -1, 59, "Outdoor Special Node", "-1 if not used"), {"onmeet", "onwin", "onflee"});
	wand_dlg["edit-end"].attachClickHandler([](cDialog& me, std::string, eKeyMod) {
		location sdf(me["endx"].getTextAsNum(), me["endy"].getTextAsNum());
		sdf = cStuffDonePicker(sdf).run();
		me["endx"].setTextToNum(sdf.x);
		me["endy"].setTextToNum(sdf.y);
		return true;
	});
	
	if(mode == 1)
		wand_dlg["title"].setText("Outdoor Special Encounter:");
	
	put_out_wand_in_dlog(wand_dlg, which, wand);
	
	wand_dlg.run();
}

static bool save_town_details(cDialog& me, std::string, eKeyMod) {
	if(!me.toast(true)) return true;
	town->name = me["name"].getText();
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
	me["name"].setText(town->name);
	me["chop"].setTextToNum(town->town_chop_time);
	me["key"].setTextToNum(town->town_chop_key);
	me["population"].setTextToNum(town->max_num_monst);
	me["population-hint"].replaceText("{{num}}", std::to_string(town->count_hostiles()));
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
	cDialog town_dlg(*ResMgr::dialogs.get("edit-town-details"));
	town_dlg["okay"].attachClickHandler(save_town_details);
	town_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &town_dlg, false));
	town_dlg["chop"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 10000, "The day the town becomes abandoned", "-1 if it doesn't"));
	town_dlg["key"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 10, "The event which prevents the town from becoming abandoned", "-1 or 0 for none"));
	town_dlg["choose-key"].attachClickHandler([](cDialog& me, std::string id, bool losing) -> bool {
		int value = me["key"].getTextAsNum();
		value = choose_text_editable(scenario.evt_names, value, &me, "Select an event:");
		me["key"].setTextToNum(value);
		return true;
	});
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
	for(int i = 0; i < town->timers.size(); i++) {
		std::string id = std::to_string(i + 1);
		town->timers[i].time = me["time" + id].getTextAsNum();
		town->timers[i].node = me["spec" + id].getTextAsNum();
	}
	return true;
}

static void put_town_events_in_dlog(cDialog& me) {
	for(short i = 0; i < town->timers.size(); i++) {
		std::string id = std::to_string(i + 1);
		me["time" + id].setTextToNum(town->timers[i].time);
		me["spec" + id].setTextToNum(town->timers[i].node);
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
	
	cDialog evt_dlg(*ResMgr::dialogs.get("edit-town-events"));
	evt_dlg["okay"].attachClickHandler(save_town_events);
	evt_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &evt_dlg, false));
	evt_dlg.attachClickHandlers(edit_town_events_event_filter, {"edit1", "edit2", "edit3", "edit4", "edit5", "edit6", "edit7", "edit8"});
	evt_dlg.attachFocusHandlers(std::bind(check_range_msg, _1, _2, _3, -1, town->specials.size(), "The town special node", "-1 for no special"), {"spec1", "spec2", "spec3", "spec4", "spec5", "spec6", "spec7", "spec8"});
	
	put_town_events_in_dlog(evt_dlg);
	
	evt_dlg.run();
}

static bool save_advanced_town(cDialog& me, std::string, eKeyMod) {
	if(!me.toast(true)) return true;
	for(int i = 0; i < town->exits.size(); i++) {
		std::string id = std::to_string(i + 1);
		town->exits[i].spec = me["onexit" + id].getTextAsNum();
		town->exits[i].x = me["exit" + id + "-x"].getTextAsNum();
		town->exits[i].y = me["exit" + id + "-y"].getTextAsNum();
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
	for(short i = 0; i < town->exits.size(); i++) {
		std::string id = std::to_string(i + 1);
		me["onexit" + id].setTextToNum(town->exits[i].spec);
		me["exit" + id + "-x"].setTextToNum(town->exits[i].x);
		me["exit" + id + "-y"].setTextToNum(town->exits[i].y);
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
	auto iter = scenario.store_item_rects.find(cur_town);
	if(iter == scenario.store_item_rects.end())
		me["saved-item-delete"].hide();
	else {
		std::ostringstream sout;
		sout << "(" << iter->second.left << "," << iter->second.top << ") to (" << iter->second.right << "," << iter->second.bottom << ")";
		me["saved-item-rect"].setText(sout.str());
	}
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
	help_text_rsrc = "town-advanced-help";
	
	cDialog town_dlg(*ResMgr::dialogs.get("edit-town-advanced"));
	town_dlg["okay"].attachClickHandler(save_advanced_town);
	town_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &town_dlg, false));
	auto loc_check = std::bind(check_range_msg, _1, _2, _3, -1, 47, "The town exit coordinates", "-1 if you want them ignored");
	auto spec_check = std::bind(check_range_msg, _1, _2, _3, -1, town->specials.size(), _4, "-1 for no special");
	town_dlg.attachFocusHandlers(std::bind(spec_check, _1, _2, _3, "The town exit special"), {"onexit1", "onexit2", "onexit3", "onexit4"});
	town_dlg.attachFocusHandlers(std::bind(spec_check, _1, _2, _3, "The town entry special"), {"onenter", "onenterdead"});
	town_dlg["onhostile"].attachFocusHandler(std::bind(spec_check, _1, _2, _3, "The town hostile special"));
	town_dlg.attachFocusHandlers(loc_check, {"exit1-x", "exit2-x", "exit3-x", "exit4-x"});
	town_dlg.attachFocusHandlers(loc_check, {"exit1-y", "exit2-y", "exit3-y", "exit4-y"});
	town_dlg.attachClickHandlers(edit_advanced_town_special, {"edit-onexit1", "edit-onexit2", "edit-onexit3", "edit-onexit4", "edit-onenter", "edit-onenterdead", "edit-onhostile"});
	town_dlg.attachClickHandlers([](cDialog& me, std::string which, eKeyMod) {
		auto entrances = scenario.find_town_entrances(cur_town);
		std::vector<location> sectors;
		for(const auto& entrance : entrances) {
			if(std::find(sectors.begin(), sectors.end(), entrance.out_sec) == sectors.end()) {
				sectors.push_back(entrance.out_sec);
			}
		}
		int which_sector = 0;
		if(sectors.size() == 0) {
			int sec =  -1;
			sec = choose_text(STRT_SECTOR, 0, &me, "Exit to which sector?");
			if(sec == -1) return true; // cancelled
			sectors.emplace_back(sec / scenario.outdoors.height(), sec % scenario.outdoors.height());
		}
		location loc(me[which + "-x"].getTextAsNum(), me[which + "-y"].getTextAsNum());
		if((loc.x < 0 || loc.y < 0 || loc.x >= 48 || loc.y >= 48) && !entrances.empty()) {
			loc = entrances[0].loc;
		}
		static const std::string directions[4]{"top", "left", "bottom", "right"};
		std::string title = "Select outdoor location when exiting from the " + directions[which.back() - '1'] + ":";
		cLocationPicker picker(loc, *scenario.outdoors[sectors[which_sector].x][sectors[which_sector].y], title, &me);
		if(sectors.size() > 1) {
			std::vector<std::string> out_names;
			for(const auto& sec : sectors) {
				out_names.push_back(scenario.outdoors[sec.x][sec.y]->name);
			}
			cControl& switchBtn = picker->getControl("switch");
			switchBtn.show();
			switchBtn.attachClickHandler([out_names, &sectors, &which_sector, &picker](cDialog& me, std::string, eKeyMod) {
				cStringChoice pickstr(out_names, "Exit to which sector?", &me);
				which_sector = pickstr.show(which_sector);
				picker.setArea(*scenario.outdoors[sectors[which_sector].x][sectors[which_sector].y]);
				return true;
			});
		}
		loc = picker.run();
		if(picker->accepted()) {
			me[which + "-x"].setTextToNum(loc.x);
			me[which + "-y"].setTextToNum(loc.y);
		}
		return true;
	}, {"exit1", "exit2", "exit3", "exit4"});
	town_dlg.attachClickHandlers([](cDialog& me, std::string which, eKeyMod) -> bool {
		std::string fld = which.replace(0, 4, "bg");
		int bg_i = me[fld].getTextAsNum();
		bg_i = choose_background(bg_i, &me);
		me[fld].setTextToNum(bg_i);
		return true;
	}, {"pick-fight", "pick-town"});
	static const std::vector<std::string> help_ids = {"exit-special-help", "special-help", "exit-help", "prop-help", "bg-help", "saved-help"};
	town_dlg.attachClickHandlers([](cDialog& me, std::string which, eKeyMod) {
		auto iter = std::find(help_ids.begin(), help_ids.end(), which);
		if(iter == help_ids.end()) return true;
		int n = iter - help_ids.begin();
		give_help(1 + n, 0, me, true);
		return true;
	}, help_ids);
	// TODO: This should probably be a focus handler, but the focus handler doesn't receive the modifiers
	static const std::vector<std::string> prop_ids = {"hidden", "nomap", "noscry", "barrier", "tavern"};
	town_dlg.attachClickHandlers([](cDialog& me, std::string which, eKeyMod mod) {
		if(mod & mod_alt) {
			auto iter = std::find(prop_ids.begin(), prop_ids.end(), which);
			if(iter == prop_ids.end()) return true;
			int n = iter - prop_ids.begin();
			give_help(10 + n, 0, me, true);
			return false;
		}
		dynamic_cast<cLed&>(me[which]).defaultClickHandler(me, which, mod);
		return true;
	}, prop_ids);
	bool delete_rect = false;
	town_dlg["saved-item-delete"].attachClickHandler([&delete_rect](cDialog& me, std::string, eKeyMod) {
		delete_rect = true;
		me["saved-item-delete"].hide();
		me["saved-item-rect"].setText("(none)");
		return true;
	});
	using namespace std::placeholders;
	auto focus_handler = std::bind(check_range_msg, _1, _2, _3, -1, 21, _4, "-1 to use scenario default");
	town_dlg["bg-fight"].attachFocusHandler(std::bind(focus_handler, _1, _2, _3, "Combat Background"));
	town_dlg["bg-town"].attachFocusHandler(std::bind(focus_handler, _1, _2, _3, "Town Background"));
	
	put_advanced_town_in_dlog(town_dlg);
	
	town_dlg.run();
	if(town_dlg.accepted() && delete_rect)
		scenario.store_item_rects.erase(cur_town);
}

static bool save_town_wand(cDialog& me, std::string, eKeyMod) {
	if(!me.toast(true)) return true;
	for(int i = 0; i < town->wandering.size(); i++) {
		std::string base_id = "group" + std::to_string(i + 1) + "-monst";
		for(int j = 0; j < 4; j++) {
			std::string id = base_id + std::to_string(j + 1);
			town->wandering[i].monst[j] = me[id].getTextAsNum();
		}
	}
	return true;
}

static void put_town_wand_in_dlog(cDialog& me) {
	for(int i = 0; i < town->wandering.size(); i++) {
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
		short n = choose_text(STRT_MONST, town->wandering[group].monst[i] - 1, &me, titles[i]) + 1;
		if(n == town->wandering[group].monst[i]) break;
		me[id].setTextToNum(n);
	}
	return true;
}

void edit_town_wand() {
	using namespace std::placeholders;
	
	cDialog wand_dlg(*ResMgr::dialogs.get("edit-town-wandering"));
	wand_dlg["okay"].attachClickHandler(save_town_wand);
	wand_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &wand_dlg, false));
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
	save_basic_dlog(me, which);
	if(hit == "okay")
		me.toast(true);
	else if(hit == "left") {
		which--;
		if(which < 0) which = 9;
		put_basic_dlog_in_dlog(me, which);
	} else if(hit == "right") {
		which++;
		if(which > 9) which = 0;
		put_basic_dlog_in_dlog(me, which);
	}
	return true;
}

void edit_basic_dlog(short personality) {
	using namespace std::placeholders;
	
	cDialog person_dlg(*ResMgr::dialogs.get("edit-personality"));
	person_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &person_dlg, false));
	person_dlg.attachClickHandlers(std::bind(edit_basic_dlog_event_filter, _1, _2, std::ref(personality)), {"left", "right", "okay"});
	
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
		showError("Personalities in talk nodes must be -1 (for unused node), -2 (all personalities use) or in the legal range of personalities in this town.", message.str(), &me);
		return false;
	}
	return true;
}

static bool check_talk_key(cDialog& me, std::string item_hit, bool losing) {
	if(!losing) return true;
	std::string key = me[item_hit].getText();
	bool passes = true;

	// An empty string is fine, it just means there's only one key.
	if(key.empty()){
		// But there needs to be at least one.
		if(me["key1"].getText() == me["key2"].getText()){
			showError("At least one of the words in this node must be filled.");
			return false;
		}
		return true;
	}

	// I'll always have trouble remembering that it should be 'purc'.
	// So why not make it easy?
	if(key == "buy"){
		me[item_hit].setText("purc");
		return true;
	}

	// We can convert upper-case letters
	to_lower(key);
	me[item_hit].setText(key);
	if(key.length() != 4) passes = false;
	for(size_t i = 0; i < 4; i++) {
		if(i < key.length() && !islower(key[i])){
			passes = false;
		}
	}
	if(!passes) {
		showError("The words this node is the response to must be 4 characters long, and all characters must be letters.", &me);
		return false;
	}
	return true;
}

typedef std::pair<short, cSpeech::cNode> node_ref_t;

static bool check_talk_xtra(cDialog& me, std::stack<node_ref_t>& talk_edit_stack, std::string item_hit, bool losing) {
	if(!losing) return true;
	
	eTalkNode type = talk_edit_stack.top().second.type;
	int which = item_hit[5] - '1';
	int lo = 0, hi = 0, msg = 0;
	static const char*const messages[] = {
		"", // No message required
		"First part of Stuff Done flag must be from 0 to {max}.",
		"Second part of Stuff Done flag must be from 0 to {max}.",
		"Inn cost must be from 0 to {max}.",
		"Inn quality must be from 0 to 3.",
		"Town number must be from 0 to {max}.",
		"Cost adjustment must be from 0 (cheapest) to 6 (most expensive).",
		"Which shop must refer to an existing shop (0 - {max}).",
		"Quest must be an existing quest (0 - {max}).",
		"Enchantment type must be from 0 to {max}. See the documentation for a list of possible abilities.",
		// 10
		"The first boat/horse must be in the legal range (0 - {max}).",
		"The special item must be in the legal range (0 - {max}).",
		"The town special node called must be in the legal range (0 - {max}), or -1 for No Special.",
		"The scenario special node called must be in the legal range (0 - {max}), or -1 for No Special.",
	};

	switch(type) {
		case eTalkNode::DEP_ON_SDF: case eTalkNode::SET_SDF:
			if(which == 0) {
				lo = 0; hi = 349;
				msg = 1;
			} else if(which == 1) {
				lo = 0; hi = 49;
				msg = 2;
			}
			break;
		case eTalkNode::BUY_SDF:
			if(which == 1) {
				lo = 0; hi = 299;
				msg = 1;
			} else if(which == 2) {
				lo = 0; hi = 9;
				msg = 2;
			}
			break;
		case eTalkNode::INN:
			if(which == 0) {
				lo = 0; hi = 1000;
				msg = 3;
			} else if(which == 1) {
				lo = 0; hi = 3;
				msg = 4;
			}
			break;
		case eTalkNode::DEP_ON_TOWN:
			if(which == 0) {
				lo = 0; hi = scenario.towns.size() - 1;
				msg = 5;
			}
			break;
		case eTalkNode::BUY_TOWN_LOC:
			if(which == 1) {
				lo = 0; hi = scenario.towns.size() - 1;
				msg = 5;
			}
			break;
		case eTalkNode::SHOP:
			if(which == 0) {
				lo = 0; hi = 6;
				msg = 6;
			} else if(which == 1) {
				lo = 0; hi = scenario.shops.size() - 1;
				msg = 7;
			}
			break;
		case eTalkNode::RECEIVE_QUEST:
			if(which == 0) {
				lo = 0; hi = scenario.quests.size() - 1;
				msg = 8;
			}
			break;
		case eTalkNode::ENCHANT:
			if(which == 0) {
				lo = 0; hi = cEnchant::MAX;
				msg = 9;
			}
			break;
		case eTalkNode::BUY_SHIP:
			if(which == 1) {
				lo = 0; hi = scenario.boats.size() - 1;
				msg = 10;
			}
			break;
		case eTalkNode::BUY_HORSE:
			if(which == 1) {
				lo = 0; hi = scenario.horses.size() - 1;
				msg = 10;
			}
			break;
		case eTalkNode::BUY_SPEC_ITEM:
			if(which == 0) {
				lo = 0; hi = scenario.special_items.size() - 1;
				msg = 11;
			}
			break;
		case eTalkNode::CALL_TOWN_SPEC:
			if(which == 0) {
				lo = -1; hi = town->specials.size() - 1;
				msg = 12;
			}
			break;
		case eTalkNode::CALL_SCEN_SPEC:
			if(which == 0) {
				lo = -1; hi = scenario.scen_specials.size() - 1;
				msg = 13;
			}
			break;
		case eTalkNode::BUY_INFO: case eTalkNode::DEP_ON_TIME: case eTalkNode::DEP_ON_TIME_AND_EVENT: case eTalkNode::REGULAR:
		case eTalkNode::END_ALARM: case eTalkNode::END_DIE: case eTalkNode::END_FIGHT: case eTalkNode::END_FORCE:
		case eTalkNode::IDENTIFY: case eTalkNode::SELL_ARMOR: case eTalkNode::SELL_ITEMS: case eTalkNode::SELL_WEAPONS:
		case eTalkNode::TRAINING: case eTalkNode::JOB_BANK: case eTalkNode::RECHARGE:
			break;
	}
	
	if(msg) {
		// This value needs validation
		std::string text = messages[msg];
		size_t n = text.find("{max}");
		if(n != std::string::npos) text.replace(n, 5, std::to_string(hi));
		return !cre(me[item_hit].getTextAsNum(), lo, hi, text, "", &me);
	}
	
	return true;
}

static bool save_talk_node(cDialog& me, std::stack<node_ref_t>& talk_edit_stack, bool close_dlg, bool commit) {
	if(!me.toast(true)) return false;
	if(!close_dlg) me.untoast();
	
	cSpeech::cNode& talk_node = talk_edit_stack.top().second;
	
	talk_node.personality = me["who"].getTextAsNum();
	
	std::string link = me["key1"].getText();
	if(link.empty()) link = "    ";
	std::copy(link.begin(), link.begin() + 4, talk_node.link1);
	link = me["key2"].getText();
	if(link.empty()) link = "    ";
	std::copy(link.begin(), link.begin() + 4, talk_node.link2);
	
	for(int i = 0; i < 4; i++) {
		std::string id = "extra" + std::to_string(i + 1);
		cTextField& ctrl = dynamic_cast<cTextField&>(me[id]);
		if(commit && !ctrl.hasFocus() && !ctrl.triggerFocusHandler(me, id, true)) {
			me.untoast();
			me.setFocus(&ctrl);
			return false;
		}
		talk_node.extras[i] = ctrl.getTextAsNum();
	}
	
	talk_node.str1 = me["str1"].getText();
	talk_node.str2 = me["str2"].getText();
	
	if(commit)
		town->talking.talk_nodes[talk_edit_stack.top().first] = talk_node;
	return true;
}

// TODO: Maybe I shouldn't reuse node_function_t here? It has some irrelevant data too.
static std::map<eTalkNode, std::array<node_function_t, 4>> node_map = {
	{eTalkNode::DEP_ON_SDF, {eSpecPicker::SDF}},
	{eTalkNode::SET_SDF, {eSpecPicker::SDF}},
	{eTalkNode::INN, {eSpecPicker::NONE, eSpecPicker::NONE, eSpecPicker::LOCATION}},
	{eTalkNode::DEP_ON_TIME_AND_EVENT, {eSpecPicker::NONE, eSpecPicker::EVENT}},
	{eTalkNode::DEP_ON_TOWN, {STRT_TOWN}},
	{eTalkNode::SHOP, {STRT_COST_ADJ, STRT_SHOP}},
	{eTalkNode::JOB_BANK, {eSpecPicker::JOB_BOARD}},
	{eTalkNode::ENCHANT, {STRT_ENCHANT}},
	{eTalkNode::BUY_SDF, {eSpecPicker::NONE, eSpecPicker::SDF}},
	{eTalkNode::BUY_SHIP, {eSpecPicker::NONE, STRT_BOAT}},
	{eTalkNode::BUY_HORSE, {eSpecPicker::NONE, STRT_HORSE}},
	{eTalkNode::BUY_SPEC_ITEM, {STRT_SPEC_ITEM}},
	{eTalkNode::RECEIVE_QUEST, {eSpecPicker::QUEST}},
	{eTalkNode::BUY_TOWN_LOC, {STRT_TOWN}},
	{eTalkNode::CALL_TOWN_SPEC, {eSpecPicker::NODE}},
	{eTalkNode::CALL_SCEN_SPEC, {+eSpecPicker::NODE}},
};

static void put_talk_node_in_dlog(cDialog& me, std::stack<node_ref_t>& talk_edit_stack) {
	cSpeech::cNode& talk_node =talk_edit_stack.top().second;
	
	me["who"].setTextToNum(talk_node.personality);
	
	std::string link = "";
	for(int i = 0; i < 4; i++) link += talk_node.link1[i];
	// I don't want 4 spaces in the text fields
	trim(link);
	me["key1"].setText(link);
	link = "";
	for(int i = 0; i < 4; i++) link += talk_node.link2[i];
	trim(link);
	me["key2"].setText(link);
	
	int iDescBase = int(talk_node.type) * 7;
	static const char*const strIDs[] = {"type", "lblA", "lblB", "lblC", "lblD", "lbl1", "lbl2"};
	for(int i = 0; i < 7; i++)
		me[strIDs[i]].setText(get_str("talk-node-descs", iDescBase + i + 1));
	
	for(int i = 0; i < 4; i++)
		me["extra" + std::to_string(i + 1)].setTextToNum(talk_node.extras[i]);
	
	me["str1"].setText(talk_node.str1);
	me["str2"].setText(talk_node.str2);
	
	const auto fcns = node_map[talk_node.type];
	for(int i = 0; i < fcns.size(); i++) {
		std::ostringstream sout;
		sout << "choose" << char('A' + i);;
		auto& btn = dynamic_cast<cButton&>(me[sout.str()]);
		if(fcns[i].button == eSpecPicker::NONE) {
			btn.hide();
			continue;
		}
		btn.show();
		if(fcns[i].button == eSpecPicker::NODE || fcns[i].button == eSpecPicker::QUEST) {
			btn.setText("Create/Edit");
			btn.setBtnType(BTN_LG);
		} else {
			btn.setText("Choose");
			btn.setBtnType(BTN_REG);
		}
	}
	
	if(talk_edit_stack.size() > 1)
		me["back"].show();
	else me["back"].hide();
}

static bool talk_node_back(cDialog& me, std::stack<node_ref_t>& talk_edit_stack) {
	if(!save_talk_node(me, talk_edit_stack, false, true)) return true;
	talk_edit_stack.pop();
	put_talk_node_in_dlog(me, talk_edit_stack);
	return true;
}

static bool talk_node_branch(cDialog& me, std::stack<node_ref_t>& talk_edit_stack) {
	if(!save_talk_node(me, talk_edit_stack, false, true)) return true;
	
	int spec = -1;
	for(int j = 0; j < town->talking.talk_nodes.size(); j++)
		if(town->talking.talk_nodes[j].personality == -1 && strnicmp(town->talking.talk_nodes[j].link1, "     ", 4) == 0) {
			spec = j;
			break;
		}
	
	if(spec < 0) {
		spec = town->talking.talk_nodes.size();
		town->talking.talk_nodes.emplace_back();
	}
	
	talk_edit_stack.push({spec, town->talking.talk_nodes[spec]});
	put_talk_node_in_dlog(me, talk_edit_stack);
	return true;
}

static bool select_talk_node_type(cDialog& me, std::stack<node_ref_t>& talk_edit_stack) {
	save_talk_node(me, talk_edit_stack, false, false);
	short i = short(talk_edit_stack.top().second.type);
	i = choose_text(STRT_TALK_NODE, i, &me, "What Talking Node type?");
	talk_edit_stack.top().second.type = eTalkNode(i);
	put_talk_node_in_dlog(me, talk_edit_stack);
	return true;
}

static bool select_talk_node_personality(cDialog& me, std::stack<node_ref_t>& talk_edit_stack) {
	save_talk_node(me, talk_edit_stack, false, false);
	short i = short(talk_edit_stack.top().second.personality);
	// 2 negative possible values, so +2 and -2 to convert from selectedIndex
	i = choose_text(STRT_TALK_NODE_PERSON, i + 2, &me, "Which personality(s)?") - 2;
	talk_edit_stack.top().second.personality = i;
	put_talk_node_in_dlog(me, talk_edit_stack);
	return true;
}

static bool select_talk_node_value(cDialog& me, std::string item_hit, const std::stack<node_ref_t>& talk_edit_stack) {
	const auto& talk_node = talk_edit_stack.top().second;
	int which_field = item_hit.back() - 'A';
	std::string field_id = "extra" + std::to_string(which_field + 1);
	std::string second_field_id = "extra" + std::to_string(which_field + 2);
	const auto& fcn = node_map[talk_node.type][which_field];
	if(fcn.button == eSpecPicker::STRING) {
		int i = me[field_id].getTextAsNum();
		std::string title;
		switch(fcn.str_type) {
			case STRT_TOWN: title = "Which town?"; break;
			case STRT_COST_ADJ: title = "What cost adjust?"; break;
			case STRT_SHOP: title = "Which shop?"; break;
			case STRT_ENCHANT: title = "Which enchantment?"; break;
			case STRT_BOAT: title = "Which boat?"; break;
			case STRT_HORSE: title = "Which horse?"; break;
			case STRT_SPEC_ITEM: title = "Which special item?"; break;
			default: title = "Title not set for this string type!!!"; break;
		}
		i = choose_text(fcn.str_type, i, &me, title);
		me[field_id].setTextToNum(i);
	} else if(fcn.button == eSpecPicker::EVENT) {
		int value = me[field_id].getTextAsNum();
		value = choose_text_editable(scenario.evt_names, value, &me, "Select an event:");
		me[field_id].setTextToNum(value);
	} else if(fcn.button == eSpecPicker::JOB_BOARD) {
		int value = me[field_id].getTextAsNum();
		value = choose_text_editable(scenario.qb_names, value, &me, "Select a job board:");
		me[field_id].setTextToNum(value);
	} else if(fcn.button == eSpecPicker::LOCATION) {
		location loc(me[field_id].getTextAsNum(), me[second_field_id].getTextAsNum());
		cLocationPicker picker(loc, *town, "Choose a location:", &me);
		loc = picker.run();
		if(picker->accepted()) {
			me[field_id].setTextToNum(loc.x);
			me[second_field_id].setTextToNum(loc.y);
		}
	} else if(fcn.button == eSpecPicker::SDF) {
		location sdf(me[second_field_id].getTextAsNum(), me[field_id].getTextAsNum());
		cStuffDonePicker picker(sdf);
		sdf = picker.run();
		me[second_field_id].setTextToNum(sdf.x);
		me[field_id].setTextToNum(sdf.y);
	} else if(fcn.button == eSpecPicker::QUEST) {
		int quest = me[field_id].getTextAsNum();
		if(quest < 0 || quest >= scenario.quests.size()){
			quest = scenario.quests.size();
			me[field_id].setTextToNum(quest);
		}
		edit_quest(quest);
	} else if(fcn.button == eSpecPicker::NODE) {
		int spec = me[field_id].getTextAsNum();
		int mode = fcn.force_global ? 0 : 2;
		if(spec < 0)
			spec = get_fresh_spec(mode);
		if(edit_spec_enc(spec,mode,&me))
			me[field_id].setTextToNum(spec);
	}
	return true;
}

// Returns -1 if accepted, otherwise the node that was cancelled
short edit_talk_node(short which_node) {
	using namespace std::placeholders;
	
	std::stack<node_ref_t> talk_edit_stack;
	talk_edit_stack.push({which_node, town->talking.talk_nodes[which_node]});
	
	cDialog talk_dlg(*ResMgr::dialogs.get("edit-talk-node"));
	talk_dlg["okay"].attachClickHandler(std::bind(save_talk_node, _1, std::ref(talk_edit_stack), true, true));
	talk_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &talk_dlg, false));
	talk_dlg["back"].attachClickHandler(std::bind(talk_node_back, _1, std::ref(talk_edit_stack)));
	talk_dlg["new"].attachClickHandler(std::bind(talk_node_branch, _1, std::ref(talk_edit_stack)));
	talk_dlg["choose-type"].attachClickHandler(std::bind(select_talk_node_type, _1, std::ref(talk_edit_stack)));
	talk_dlg["choose-personality"].attachClickHandler(std::bind(select_talk_node_personality, _1, std::ref(talk_edit_stack)));
	talk_dlg.attachClickHandlers(std::bind(select_talk_node_value, _1, _2, std::ref(talk_edit_stack)), {"chooseA","chooseC","chooseB"});
	talk_dlg["who"].attachFocusHandler(check_talk_personality);
	talk_dlg.attachFocusHandlers(check_talk_key, {"key1", "key2"});
	talk_dlg.attachFocusHandlers(std::bind(check_talk_xtra, _1, std::ref(talk_edit_stack), _2, _3), {"extra1", "extra2", "extra3", "extra4"});
	
	put_talk_node_in_dlog(talk_dlg, talk_edit_stack);
	
	talk_dlg.run();
	return talk_dlg.accepted() ? -1 : talk_edit_stack.top().first;
}

static void put_out_loc_in_dlog(cDialog& me, location cur_loc, cScenario& scenario) {
	std::ostringstream str;
	str << "X = " << cur_loc.x;
	me["x"].setText(str.str());
	clear_sstr(str);
	str << "Y = " << cur_loc.y;
	me["y"].setText(str.str());
	me["title"].setText(scenario.outdoors[cur_loc.x][cur_loc.y]->name);
}

static bool pick_out_event_filter(cDialog& me, std::string item_hit, location& cur_loc, cScenario& scenario) {
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
		using std::swap;
		int i = cur_loc.x * scenario.outdoors.height() + cur_loc.y;
		if(&scenario != &::scenario)
			swap(scenario.outdoors, ::scenario.outdoors);
		i = choose_text(STRT_SECTOR, i, &me, "Which sector?");
		if(&scenario != &::scenario)
			swap(scenario.outdoors, ::scenario.outdoors);
		cur_loc.x = i / scenario.outdoors.height();
		cur_loc.y = i % scenario.outdoors.height();
	}
	put_out_loc_in_dlog(me, cur_loc, scenario);
	return true;
}

static bool finish_pick_out(cDialog& me, bool okay, location& cur_loc, location orig_loc) {
	if(me.toast(okay)) {
		if(!okay) cur_loc = orig_loc;
	}
	return true;
}

location pick_out(location default_loc,cScenario& scenario) {
	using namespace std::placeholders;
	location prev_loc = default_loc;
	if(default_loc.x < 0)
		default_loc.x = 0;
	if(default_loc.y < 0)
		default_loc.y = 0;
	
	cDialog out_dlg(*ResMgr::dialogs.get("select-sector"));
	out_dlg["okay"].attachClickHandler(std::bind(finish_pick_out, _1, true, std::ref(default_loc), prev_loc));
	out_dlg["cancel"].attachClickHandler(std::bind(finish_pick_out, _1, false, std::ref(default_loc), prev_loc));
	out_dlg.attachClickHandlers(std::bind(pick_out_event_filter, _1, _2, std::ref(default_loc), std::ref(scenario)), {"xplus", "xminus", "yplus", "yminus", "choose"});
	
	out_dlg["width"].setTextToNum(scenario.outdoors.width());
	out_dlg["height"].setTextToNum(scenario.outdoors.height());
	put_out_loc_in_dlog(out_dlg, default_loc, scenario);
	
	out_dlg.run();
	return default_loc;
}

static void store_current_town_state() {
	scenario.editor_state.town_view_state[cur_town] = {
		{cen_x, cen_y}, cur_viewing_mode
	};
}

static void store_current_out_state() {
	scenario.editor_state.out_view_state[cur_out] = {
		{cen_x, cen_y}, cur_viewing_mode
	};
}

void store_current_terrain_state() {
	if(overall_mode < MODE_MAIN_SCREEN){
		if(editing_town){
			store_current_town_state();
		}else{
			store_current_out_state();
		}
	}
}

void set_current_town(int to, bool first_restore) {
	if(!first_restore){
		store_current_terrain_state();
	}

	if(to < 0 || to >= scenario.towns.size()) return;
	cur_town = to;
	town = scenario.towns[cur_town];
	scenario.editor_state.last_town_edited = cur_town;
}

void set_current_out(location out_sec, bool continuous_shift, bool first_restore) {
	if(!first_restore){
		store_current_terrain_state();
	}
	cur_out = out_sec;
	scenario.editor_state.last_out_edited = cur_out;
	current_terrain = scenario.outdoors[cur_out.x][cur_out.y];
	last_shift_continuous = continuous_shift;
	set_up_main_screen();
}

bool new_town() {
	ter_num_t preset = 0;
	cChoiceDlog new_dlg("new-town", {"okay", "cancel"});
	new_dlg->getControl("num").setTextToNum(scenario.towns.size());
	new_dlg->getControl("choose-preset").attachClickHandler([&preset](cDialog& me, std::string, eKeyMod) {
		preset = choose_text(STRT_TER, preset, &me, "Select a preset terrain:");
		dynamic_cast<cPict&>(me["preset"]).setPict(scenario.ter_types[preset].picture);
		me["preset-name"].setText(scenario.ter_types[preset].name);
		return true;
	});
	new_dlg->getControl("preset-name").setText(scenario.ter_types[preset].name);
	if(new_dlg.show() == "cancel") return false;
	
	std::string size = dynamic_cast<cLedGroup&>(new_dlg->getControl("size")).getSelected();
	
	if(size == "lg") scenario.addTown(AREA_LARGE);
	else if(size == "med") scenario.addTown(AREA_MEDIUM);
	else if(size == "sm") scenario.addTown(AREA_SMALL);
	else if(size == "huge") scenario.addTown(AREA_HUGE);
	else if(size == "tiny") scenario.addTown(AREA_TINY);
	
	set_current_town(scenario.towns.size() - 1);
	town->name = new_dlg->getControl("name").getText().substr(0,30);
	
	for(short i = 0; i < town->max_dim; i++)
		for(short j = 0; j < town->max_dim; j++)
			town->terrain(i,j) = preset;

	undo_list.add(action_ptr(new aCreateDeleteTown(true, scenario.towns.back())));
	update_edit_menu();
	change_made = true;
	
	return true;
}

// before calling this, be sure to do all checks to make sure it's safe.
void delete_last_town() {
	undo_list.add(action_ptr(new aCreateDeleteTown(false, scenario.towns.back())));
	update_edit_menu();

	scenario.towns.pop_back();
	change_made = true;
}

cTown* pick_import_town() {
	cScenario temp_scenario;
	fs::path path = nav_get_scenario();
	if(path.empty()) return nullptr;
	load_scenario(path, temp_scenario);
	short town_num = pick_town_num("select-import-town", 0, temp_scenario);
	if(town_num < 0) return nullptr;
	cTown* town = temp_scenario.towns[town_num];
	temp_scenario.towns[town_num] = nullptr;
	return town;
}

using loc_set = std::set<location, loc_compare>;
static void fill_resize_outdoors(cDialog& me, int top, int left, int right, int bottom, loc_set& del) {
	me["top"].setTextToNum(top);
	me["left"].setTextToNum(left);
	me["right"].setTextToNum(right);
	me["bottom"].setTextToNum(bottom);
	
	me["w-new"].setTextToNum(scenario.outdoors.width() + left + right);
	me["h-new"].setTextToNum(scenario.outdoors.height() + top + bottom);
	
	rectangle txtBounds = me["delete"].getBounds();
	if(left >= 0 && right >= 0 && top >= 0 && bottom >= 0) {
		me["delete"].setText("(none)");
		txtBounds.height() = 10;
	} else {
		del.clear();
		while(left < 0) {
			for(int y = 0; y < scenario.outdoors.height(); y++)
				del.insert(loc(-left - 1, y));
			left++;
		}
		while(top < 0) {
			for(int x = 0; x < scenario.outdoors.width(); x++)
				del.insert(loc(x, -top - 1));
			top++;
		}
		while(right < 0) {
			for(int y = 0; y < scenario.outdoors.height(); y++)
				del.insert(loc(scenario.outdoors.width() + right, y));
			right++;
		}
		while(bottom < 0) {
			for(int x = 0; x < scenario.outdoors.width(); x++)
				del.insert(loc(x, scenario.outdoors.height() + bottom));
			bottom++;
		}
		std::ostringstream list;
		auto txtHeight = txtBounds.height();
		txtHeight = 0;
		for(location l : del) {
			list << '(' << l.x << ", " << l.y << ") " << scenario.outdoors[l.x][l.y]->name << '|';
			txtHeight += 10;
		}
		me["delete"].setText(list.str());
	}
	me["delete"].setBounds(txtBounds);
	dynamic_cast<cScrollPane&>(me["scroll"]).recalcRect();
}

static bool resize_outdoors_filter(cDialog& me, std::string btn, rectangle& mod, loc_set& del) {
	int dir = btn.back() == 'p' ? 1 : -1;
	btn.erase(btn.end() - 2, btn.end());
	
	// Make sure the change won't put the width/height below 1 or over 50
	if(btn == "top" || btn == "bottom") {
		int h = scenario.outdoors.height() + mod.top + mod.bottom;
		if((dir == 1 && h >= 50) || (dir == -1 && h <= 1)) {
			beep();
			return true;
		}
	} else if(btn == "left" || btn == "right") {
		int w = scenario.outdoors.width() + mod.left + mod.right;
		if((dir == 1 && w >= 50) || (dir == -1 && w <= 1)) {
			beep();
			return true;
		}
	} else return true; // Should be unreachable
	
	if(btn == "top")
		mod.top += dir;
	else if(btn == "left")
		mod.left += dir;
	else if(btn == "right")
		mod.right += dir;
	else if(btn == "bottom")
		mod.bottom += dir;
	
	fill_resize_outdoors(me, mod.top, mod.left, mod.right, mod.bottom, del);
	return true;
}

bool resize_outdoors() {
	using namespace std::placeholders;
	loc_set del;
	rectangle mod;
	cDialog size_dlg(*ResMgr::dialogs.get("resize-outdoors"));
	size_dlg["w-old"].setTextToNum(scenario.outdoors.width());
	size_dlg["h-old"].setTextToNum(scenario.outdoors.height());
	fill_resize_outdoors(size_dlg, mod.top, mod.left, mod.right, mod.bottom, del);
	
	size_dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, &size_dlg, true));
	size_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &size_dlg, false));
	size_dlg.attachClickHandlers(std::bind(resize_outdoors_filter, _1, _2, std::ref(mod), std::ref(del)), {
		"top-p", "top-m", "left-p", "left-m", "right-p", "right-m", "bottom-p", "bottom-m"
	});
	
	size_dlg.run();
	if(!size_dlg.accepted()) return false;
	
	for(location l : del) {
		delete scenario.outdoors[l.x][l.y];
		scenario.outdoors[l.x][l.y] = nullptr;
	}
	
	size_t old_w = scenario.outdoors.width(), old_h = scenario.outdoors.height();
	size_t new_w = old_w + mod.left + mod.right, new_h = old_h + mod.top + mod.bottom;
	
	if(new_w > old_w)
		scenario.outdoors.resize(new_w, scenario.outdoors.height());
	if(new_h > old_h)
		scenario.outdoors.resize(scenario.outdoors.width(), new_h);
	
	if(mod.top > 0) {
		int y = new_h - mod.top;
		while(y--) {
			scenario.outdoors.row(y + mod.top) = scenario.outdoors.row(y);
		}
	} else if(mod.top < 0) {
		for(int y = -mod.top; y < old_h; y++) {
			scenario.outdoors.row(y + mod.top) = scenario.outdoors.row(y);
		}
	}
	
	if(mod.left > 0) {
		int x = new_w - mod.left;
		while(x--) {
			scenario.outdoors.col(x + mod.left) = scenario.outdoors.col(x);
		}
	} else if(mod.left < 0) {
		for(int x = -mod.left; x < old_w; x++) {
			scenario.outdoors.col(x + mod.left) = scenario.outdoors.col(x);
		}
	}
	
	scenario.outdoors.resize(new_w, new_h);
	for(int y = 0; y < new_h; y++) {
		for(int x = 0; x < new_w; x++) {
			if(scenario.outdoors[x][y] == nullptr)
				scenario.outdoors[x][y] = new cOutdoors(scenario);
		}
	}
	
	// Update current location - point to same sector if possible
	cur_out.x += mod.left;
	cur_out.y += mod.top;
	if(cur_out.x >= new_w || cur_out.x < 0)
		cur_out.x = 0;
	if(cur_out.y >= new_h || cur_out.y < 0)
		cur_out.y = 0;
	current_terrain = scenario.outdoors[cur_out.x][cur_out.y];
	
	return true;
}

cOutdoors* pick_import_out() {
	cScenario temp_scenario;
	fs::path path = nav_get_scenario();
	if(path.empty()) return nullptr;
	load_scenario(path, temp_scenario);
	location sector = pick_out({-1,-1},temp_scenario);
	if(sector.x < 0 && sector.y < 0)
		return nullptr;
	cOutdoors* out = temp_scenario.outdoors[sector.x][sector.y];
	temp_scenario.outdoors[sector.x][sector.y] = nullptr;
	return out;
}
