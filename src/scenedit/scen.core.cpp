
#include <cstdio>
#include <cstring>
#include <functional>
#include <numeric>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "scen.global.h"
#include "scenario.h"
#include "regtown.h"
#include "graphtool.hpp"
#include "scen.graphics.h"
#include "scen.core.h"
#include "scen.keydlgs.h"
#include "scen.townout.h"
#include "scen.fileio.h"
#include "scen.actions.h"
#include "scen.menus.h"
#include "dialog.hpp"
#include "dlogutil.hpp"
#include "fileio.hpp"
#include "field.hpp"
#include "restypes.hpp"
#include "stack.hpp"
#include "spell.hpp"

extern short cen_x, cen_y,cur_town;
extern bool mouse_button_held;
extern short cur_viewing_mode;
extern cTown* town;
extern cOutdoors* current_terrain;
extern short mode_count,to_create;
extern ter_num_t template_terrain[64][64];
extern cScenario scenario;
extern cCustomGraphics spec_scen_g;
extern location cur_out;
extern const std::multiset<eItemType> equippable;

const std::set<eItemAbil> items_no_strength = {
	eItemAbil::NONE, eItemAbil::HEALING_WEAPON, eItemAbil::RETURNING_MISSILE, eItemAbil::SEEKING_MISSILE, eItemAbil::DRAIN_MISSILES,
	eItemAbil::LIGHTER_OBJECT, eItemAbil::HEAVIER_OBJECT, eItemAbil::LIFE_SAVING, eItemAbil::POISON_AUGMENT,
	eItemAbil::QUICKFIRE,
};

static bool save_ter_info(cDialog& me, cTerrain& ter) {
	eTerSpec prop = eTerSpec(boost::lexical_cast<short>(dynamic_cast<cLedGroup&>(me["prop"]).getSelected().substr(4)));
	int spec_type = me["flag2"].getTextAsNum();
	int num_town = (**std::min_element(scenario.towns.begin(), scenario.towns.end(), [](cTown* a,cTown* b){
		return a->specials.size() < b->specials.size();
	})).specials.size();
	int num_out = (**std::min_element(scenario.outdoors.begin(), scenario.outdoors.end(), [](cOutdoors* a,cOutdoors* b){
		return a->specials.size() < b->specials.size();
	})).specials.size();
	int num_loc = std::min(num_town, num_out);
	int num_glob = scenario.scen_specials.size();
	switch(prop) {
		case eTerSpec::CHANGE_WHEN_STEP_ON:
		case eTerSpec::CHANGE_WHEN_USED:
			if(!check_range(me, "flag1", true, 0, scenario.ter_types.size() - 1, "Terrain to change to")) return false;
			if(!check_range(me, "flag2", true, 0, 99, "Sound to play")) return false;
			break;
		case eTerSpec::DAMAGING:
			if(!check_range(me, "flag1", true, 0, 100, "Sides per die")) return false;
			if(!check_range(me, "flag2", true, 0, 100, "Number of sides")) return false;
			if(!check_range(me, "flag3", true, 0, 8, "Damage type")) return false;
			break;
		case eTerSpec::DANGEROUS:
			if(!check_range(me, "flag1", true, 0, 8, "Strength")) return false;
			if(!check_range(me, "flag2", true, 0, 100, "Percentage chance")) return false;
			if(!check_range(me, "flag3", true, 0, 14, "Status type")) return false;
			break;
		case eTerSpec::CRUMBLING:
			if(!check_range_msg(me, "flag2", true, 0, 2, "Method", "1 - Move Mountains, 2 - quickfire, 0 - either")) return false;
		if(false) // Prevent next line from executing if it's crumbling
		case eTerSpec::UNLOCKABLE:
			if(!check_range(me, "flag2", true, 0, 10, "Difficulty")) return false;
		case eTerSpec::LOCKABLE:
		case eTerSpec::TOWN_ENTRANCE:
			if(!check_range(me, "flag1", true, 0, scenario.ter_types.size() - 1,
					prop == eTerSpec::TOWN_ENTRANCE ? "Terrain when hidden" :  "Terrain to change to"))
				return false;
			break;
		case eTerSpec::WATERFALL_CAVE:
		case eTerSpec::WATERFALL_SURFACE:
		case eTerSpec::CONVEYOR:
			if(!check_range(me, "flag1", true, 0, 7, "Direction")) return false;
			break;
		case eTerSpec::CALL_SPECIAL:
		case eTerSpec::CALL_SPECIAL_WHEN_USED:
			if(spec_type < 0 || spec_type > 1) {
				giveError("Special type must be either 0 or 1.", &me);
				return false;
			}
			if(!check_range(me, "flag1", true, 0, (spec_type == 0 ? num_glob : num_loc) - 1, "Special to call")) return false;
			break;
		case eTerSpec::BRIDGE: case eTerSpec::BED: case eTerSpec::IS_A_SIGN: case eTerSpec::IS_A_CONTAINER: case eTerSpec::BLOCKED_TO_MONSTERS:
		case eTerSpec::WILDERNESS_CAVE: case eTerSpec::WILDERNESS_SURFACE: case eTerSpec::NONE:
		case eTerSpec::UNUSED1: case eTerSpec::UNUSED2: case eTerSpec::UNUSED3:
			break;
	}
	
	ter.picture = me["pict"].getTextAsNum();
	// TODO: Should somehow verify the pict number is valid
	
	std::string blockage = dynamic_cast<cLedGroup&>(me["blockage"]).getSelected();
	if(blockage == "clear") ter.blockage = eTerObstruct::CLEAR;
	else if(blockage == "curtain") ter.blockage = eTerObstruct::BLOCK_SIGHT;
	else if(blockage == "special") ter.blockage = eTerObstruct::BLOCK_MONSTERS;
	else if(blockage == "window") ter.blockage = eTerObstruct::BLOCK_MOVE;
	else if(blockage == "obstructed") ter.blockage = eTerObstruct::BLOCK_MOVE_AND_SHOOT;
	else if(blockage == "opaque") ter.blockage = eTerObstruct::BLOCK_MOVE_AND_SIGHT;
	ter.special = prop;
	ter.flag1 = me["flag1"].getTextAsNum();
	ter.flag2 = me["flag2"].getTextAsNum();
	ter.flag3 = me["flag3"].getTextAsNum();
	
	ter.trans_to_what = me["trans"].getTextAsNum();
	ter.fly_over = dynamic_cast<cLed&>(me["flight"]).getState() == led_red;
	ter.boat_over = dynamic_cast<cLed&>(me["boat"]).getState() == led_red;
	ter.block_horse = dynamic_cast<cLed&>(me["horse"]).getState();
	ter.light_radius = me["light"].getTextAsNum();
	
	std::string sound = dynamic_cast<cLedGroup&>(me["sound"]).getSelected();
	// TODO: Offer the option of a custom sound?
	if(sound == "step") ter.step_sound = eStepSnd::STEP;
	else if(sound == "squish") ter.step_sound = eStepSnd::SQUISH;
	else if(sound == "crunch") ter.step_sound = eStepSnd::CRUNCH;
	else if(sound == "nosound") ter.step_sound = eStepSnd::NONE;
	else if(sound == "splash") ter.step_sound = eStepSnd::SPLASH;
	
	std::string shortcut = me["key"].getText();
	if(shortcut.length() > 0) ter.shortcut_key = shortcut[0];
	else ter.shortcut_key = 0;
	
	ter.name = me["name"].getText();
	
	ter.ground_type = me["ground"].getTextAsNum();
	ter.trim_ter = me["trimter"].getTextAsNum();
	ter.trim_type = (eTrimType) me["trim"].getTextAsNum();
	ter.combat_arena = me["arena"].getTextAsNum();
	ter.map_pic = me["map"].getTextAsNum();
	
	return true;
}

// TODO: It would probably be good to use these for other dialogs, instead of the monolithic event filters
bool check_range_msg(cDialog& me,std::string id,bool losing,long min_val,long max_val,std::string fld_name,std::string xtra){
	if(!losing) return true;
	cTextField& fld_ctrl = dynamic_cast<cTextField&>(me[id]);
	long n = fld_ctrl.getTextAsNum();
	if(n < min_val || n > max_val){
		std::ostringstream sout;
		sout << fld_name << " must be from " << min_val << " to " << max_val;
		if(!xtra.empty()) sout << ' ' << '(' << xtra << ')';
		giveError(sout.str(), "", &me);
		return false;
	}
	return true;
}

bool check_range(cDialog& me,std::string id,bool losing,long min_val,long max_val,std::string fld_name) {
	return check_range_msg(me, id, losing, min_val, max_val, fld_name, "");
}

// TODO: I have two functions that do this. (The other one is choose_graphic.)
static bool pick_picture(ePicType type, cDialog& parent, std::string result_fld, std::string pic_fld){
	pic_num_t cur_sel = 0;
	if(result_fld != ""){
		cControl& fld_ctrl = parent[result_fld];
		cur_sel = fld_ctrl.getTextAsNum();
	}else if(pic_fld != ""){
		cPict& pic_ctrl = dynamic_cast<cPict&>(parent[pic_fld]);
		if(pic_ctrl.getPicType() == type)
			cur_sel = pic_ctrl.getPicNum();
	}
	pic_num_t pic = choose_graphic(cur_sel, type, &parent);
	if(pic != NO_PIC){
		if(result_fld != ""){
			cTextField& fld_ctrl = dynamic_cast<cTextField&>(parent[result_fld]);
			fld_ctrl.setTextToNum(pic);
		}
		if(pic_fld != ""){
			cPict& pic_ctrl = dynamic_cast<cPict&>(parent[pic_fld]);
			if(type == PIC_TER_ANIM && pic < 1000)
				pic += 960;
			pic_ctrl.setPict(pic,type);
		}
	}
	return true;
}

bool pick_string(std::string from_file, cDialog& parent, std::string result_fld, std::string str_fld){
	size_t cur_sel = 0;
	if(result_fld != ""){
		cTextField& fld_ctrl = dynamic_cast<cTextField&>(parent[result_fld]);
		cur_sel = fld_ctrl.getTextAsNum();
	}
	StringRsrc strings = *ResMgr::get<StringRsrc>(from_file);
	// TODO: Does it need a title?
	cStringChoice str_dlg(strings, "", &parent);
	size_t result = str_dlg.show(cur_sel);
	if(result < strings.size()){
		if(result_fld != ""){
			cTextField& fld_ctrl = dynamic_cast<cTextField&>(parent[result_fld]);
			fld_ctrl.setTextToNum(result);
		}
		if(str_fld != ""){
			parent[str_fld].setText(strings[result]);
		}
	}
	return true;
}

static bool show_help(std::string from_file, cDialog& parent, pic_num_t pic){
	StringRsrc strings = *ResMgr::get<StringRsrc>(from_file);
	cThreeChoice help(strings,basic_buttons[63],pic,PIC_DLOG,&parent);
	help.show();
	return true;
}

static bool pick_ter_flag(cDialog& me, std::string id, eKeyMod) {
	if(id == "editspec") {
		int which_type = me["flag2"].getTextAsNum();
		if(which_type == 1) {
			std::string choice = cChoiceDlog("pick-spec-type", {"town", "out", "cancel"}).show();
			if(choice == "cancel") return true;
			else if(choice == "town") which_type = 2;
		}
		short spec = me["flag1"].getTextAsNum();
		if(spec < 0 || spec > 255) {
			spec = get_fresh_spec(which_type);
			if(spec < 0) {
				giveError("You can't create a new special encounter because there are no more free special nodes.",
						  "To free a special node, set its type to No Special and set its Jump To special to -1.", &me);
				return true;
			}
		}
		if(edit_spec_enc(spec,which_type,&me))
			me["flag1"].setTextToNum(spec);
		return true;
	} else if(id == "picktrans") {
		int i = me["trans"].getTextAsNum();
		i = choose_text(STRT_TER, i, &me, "Transform to what terrain?");
		me["trans"].setTextToNum(i);
		return true;
	}
	int which = id[8] - '0';
	std::string fld = id.substr(4);
	int i = me[fld].getTextAsNum();
	eTerSpec prop;
	cLedGroup& led_ctrl = dynamic_cast<cLedGroup&>(me["prop"]);
	std::istringstream sel(led_ctrl.getSelected().substr(4));
	sel >> prop;
	switch(prop) {
		case eTerSpec::NONE:
			// TODO: Could have a pick graphic dialog for the editor icon, but that requires adding a new graphic type
			return true;
		case eTerSpec::CHANGE_WHEN_STEP_ON: case eTerSpec::CHANGE_WHEN_USED:
			if(which == 1)
				i = choose_text(STRT_TER, i, &me, "Change to what terrain?");
			else if(which == 2)
				i = choose_text(STRT_SND, i, &me, "Select the sound to play when it changes:");
			else return true;
			break;
		case eTerSpec::DAMAGING:
			if(which == 3)
				i = choose_damage_type(i, &me);
			else return true;
			break;
		case eTerSpec::DANGEROUS:
			if(which == 3)
				i = choose_status_effect(i, false, &me);
			else return true;
			break;
		case eTerSpec::BED:
			if(which == 1)
				i = choose_graphic(i, PIC_TER, &me);
			else return true;
			break;
		case eTerSpec::WATERFALL_CAVE: case eTerSpec::WATERFALL_SURFACE: case eTerSpec::CONVEYOR:
			if(which == 1)
				i = choose_text(STRT_DIR, i, &me, "Which direction?");
			else return true;
			break;
		case eTerSpec::CRUMBLING: case eTerSpec::LOCKABLE: case eTerSpec::UNLOCKABLE: case eTerSpec::TOWN_ENTRANCE:
			if(which == 1)
				i = choose_text(STRT_TER, i, &me, prop == eTerSpec::TOWN_ENTRANCE ? "Terrain type when hidden:" : "Terrain to change to:");
			else return true;
			break;
		case eTerSpec::CALL_SPECIAL: case eTerSpec::CALL_SPECIAL_WHEN_USED:
		case eTerSpec::UNUSED1: case eTerSpec::UNUSED2: case eTerSpec::UNUSED3:
		case eTerSpec::WILDERNESS_CAVE: case eTerSpec::WILDERNESS_SURFACE:
		case eTerSpec::BRIDGE: case eTerSpec::IS_A_SIGN: case eTerSpec::IS_A_CONTAINER: case eTerSpec::BLOCKED_TO_MONSTERS:
			return true;
	}
	me[fld].setTextToNum(i);
	return true;
}

static bool fill_ter_flag_info(cDialog& me, std::string id, bool losing){
	if(losing) return true;
	eTerSpec prop;
	cLedGroup& led_ctrl = dynamic_cast<cLedGroup&>(me[id]);
	std::istringstream sel(led_ctrl.getSelected().substr(4));
	sel >> prop;
	me["flag1text"].setText(get_str("ter-flag1",int(prop) + 1));
	me["flag2text"].setText(get_str("ter-flag2",int(prop) + 1));
	me["flag3text"].setText(get_str("ter-flag3",int(prop) + 1));
	me["pickflag1"].hide();
	me["pickflag2"].hide();
	me["pickflag3"].hide();
	me["editspec"].hide();
	switch(prop) {
		case eTerSpec::NONE:
			me["pickflag1"].hide(); // TODO: Could have a pick graphic dialog for the editor icon, but that requires adding a new graphic type
			break;
		case eTerSpec::CHANGE_WHEN_STEP_ON: case eTerSpec::CHANGE_WHEN_USED:
			me["pickflag1"].show();
			me["pickflag2"].show();
			break;
		case eTerSpec::DAMAGING: case eTerSpec::DANGEROUS:
			me["pickflag3"].show();
			break;
		case eTerSpec::BED: case eTerSpec::WATERFALL_CAVE: case eTerSpec::WATERFALL_SURFACE: case eTerSpec::CONVEYOR:
		case eTerSpec::CRUMBLING: case eTerSpec::LOCKABLE: case eTerSpec::UNLOCKABLE: case eTerSpec::TOWN_ENTRANCE:
			me["pickflag1"].show();
			break;
		case eTerSpec::CALL_SPECIAL: case eTerSpec::CALL_SPECIAL_WHEN_USED:
			me["editspec"].show();
			break;
		case eTerSpec::UNUSED1: case eTerSpec::UNUSED2: case eTerSpec::UNUSED3:
		case eTerSpec::WILDERNESS_CAVE: case eTerSpec::WILDERNESS_SURFACE:
		case eTerSpec::BRIDGE: case eTerSpec::IS_A_SIGN: case eTerSpec::IS_A_CONTAINER: case eTerSpec::BLOCKED_TO_MONSTERS:
			break;
	}
	return true;
}

static void fill_ter_info(cDialog& me, short ter){
	cTerrain& ter_type = scenario.ter_types[ter];
	{
		cPict& pic_ctrl = dynamic_cast<cPict&>(me["graphic"]);
		pic_num_t pic = ter_type.picture;
		if(pic < 960)
			pic_ctrl.setPict(pic, PIC_TER);
		else if(pic < 1000)
			pic_ctrl.setPict(pic - 960, PIC_TER_ANIM);
		else if(pic < 2000)
			pic_ctrl.setPict(pic % 1000, PIC_CUSTOM_TER);
		else
			pic_ctrl.setPict(pic % 2000, PIC_CUSTOM_TER_ANIM);
		me["pict"].setTextToNum(pic);
	}{
		cPict& pic_ctrl = dynamic_cast<cPict&>(me["seemap"]);
		pic_num_t pic = ter_type.map_pic;
		if(pic < 1000)
			pic_ctrl.setPict(pic, PIC_TER_MAP);
		else pic_ctrl.setPict(pic, PIC_CUSTOM_TER_MAP);
		me["map"].setTextToNum(pic);
	}
	me["number"].setTextToNum(ter);
	me["name"].setText(ter_type.name);
	if(ter_type.shortcut_key > ' ')
		me["key"].setText(std::string(1, ter_type.shortcut_key));
	else me["key"].setText("");
	me["light"].setTextToNum(ter_type.light_radius);
	me["trans"].setTextToNum(ter_type.trans_to_what);
	me["ground"].setTextToNum(ter_type.ground_type);
	me["trimter"].setTextToNum(ter_type.trim_ter);
	// TODO: Replace edit text box for trim with a framed name and Select button
	me["trim"].setTextToNum((int)ter_type.trim_type);
	std::string propid = "prop" + std::to_string((int)ter_type.special);
	dynamic_cast<cLedGroup&>(me["prop"]).setSelected(propid);
	fill_ter_flag_info(me, "prop", false);
	{
		cLedGroup& led_ctrl = dynamic_cast<cLedGroup&>(me["blockage"]);
		switch(ter_type.blockage){
			case eTerObstruct::CLEAR:
				led_ctrl.setSelected("clear");
				break;
			case eTerObstruct::BLOCK_SIGHT:
				led_ctrl.setSelected("curtain");
				break;
			case eTerObstruct::BLOCK_MONSTERS:
				led_ctrl.setSelected("special");
				break;
			case eTerObstruct::BLOCK_MOVE:
				led_ctrl.setSelected("window");
				break;
			case eTerObstruct::BLOCK_MOVE_AND_SHOOT:
				led_ctrl.setSelected("obstructed");
				break;
			case eTerObstruct::BLOCK_MOVE_AND_SIGHT:
				led_ctrl.setSelected("opaque");
				break;
		}
	}{
		cLedGroup& led_ctrl = dynamic_cast<cLedGroup&>(me["sound"]);
		switch(ter_type.step_sound){
			case eStepSnd::STEP:
				led_ctrl.setSelected("step");
				break;
			case eStepSnd::SQUISH:
				led_ctrl.setSelected("squish");
				break;
			case eStepSnd::CRUNCH:
				led_ctrl.setSelected("crunch");
				break;
			case eStepSnd::NONE:
				led_ctrl.setSelected("nosound");
				break;
			case eStepSnd::SPLASH:
				led_ctrl.setSelected("splash");
				break;
		}
	}
	if(ter_type.fly_over){
		cLed& led_ctrl = dynamic_cast<cLed&>(me["flight"]);
		led_ctrl.setState(led_red);
	}
	if(ter_type.boat_over){
		cLed& led_ctrl = dynamic_cast<cLed&>(me["boat"]);
		led_ctrl.setState(led_red);
	}
	if(ter_type.block_horse){
		cLed& led_ctrl = dynamic_cast<cLed&>(me["horse"]);
		led_ctrl.setState(led_red);
	}
	me["flag1"].setTextToNum(ter_type.flag1);
	me["flag2"].setTextToNum(ter_type.flag2);
	me["flag3"].setTextToNum(ter_type.flag3);
	me["arena"].setTextToNum(ter_type.combat_arena);
}

static bool finish_editing_ter(cDialog& me, std::string id, ter_num_t& which) {
	if(!save_ter_info(me, scenario.ter_types[which])) return true;
	
	if(!me.toast(true)) return true;
	if(id == "left") {
		me.untoast();
		if(which == 0)
			which = scenario.ter_types.size() - 1;
		else which--;
		fill_ter_info(me, which);
	} else if(id == "right") {
		me.untoast();
		which++;
		if(which >= scenario.ter_types.size())
			which = 0;
		fill_ter_info(me, which);
	}
	return true;
}

static bool edit_ter_obj(cDialog& me, ter_num_t which_ter) {
	cTerrain& ter = scenario.ter_types[which_ter];
	const pic_num_t pic = ter.picture;
	cDialog obj_dlg("edit-ter-obj", &me);
	obj_dlg.attachFocusHandlers([&pic](cDialog& me, std::string fld, bool losing) -> bool {
		if(!losing) return true;
		int id = me["id"].getTextAsNum();
		if(fld != "id") {
			int i = me[fld].getTextAsNum();
			if(i > 4 || (i == 4 && (fld == "x" || fld == "y"))) {
				giveError("Terrain objects cannot be larger than 4x4 tiles.", &me);
				return false;
			} else if(id > 0 && i == 0 && (fld == "w" || fld == "h")) {
				giveError("Width and height cannot be zero.", &me);
				return false;
			}
		} else {
			// Check if the object already exists and, if so, import the size data
			for(cTerrain& check : scenario.ter_types) {
				if(check.obj_num == id) {
					me["w"].setTextToNum(check.obj_size.x);
					me["h"].setTextToNum(check.obj_size.y);
					break;
				}
			}
		}
		pic_num_t obj[4][4] = {{74,74,74,74},{74,74,74,74},{74,74,74,74},{74,74,74,74}};
		for(cTerrain& check : scenario.ter_types) {
			if(check.obj_num != id) continue;
			obj[check.obj_pos.x][check.obj_pos.y] = check.picture;
		}
		obj[me["x"].getTextAsNum()][me["y"].getTextAsNum()] = pic;
		for(int x = 0; x < 4; x++) {
			for(int y = 0; y < 4; y++) {
				std::string id = "x" + std::to_string(x) + "y" + std::to_string(y);
				dynamic_cast<cPict&>(me[id]).setPict(obj[x][y]);
			}
		}
		return true;
	}, {"w", "h", "x", "y", "id"});
	obj_dlg["okay"].attachClickHandler([](cDialog& me, std::string, eKeyMod) -> bool {
		if(!me.toast(true)) return true;
		int x = me["x"].getTextAsNum(), y = me["y"].getTextAsNum();
		int w = me["w"].getTextAsNum(), h = me["h"].getTextAsNum();
		if(x >= w || y >= h) {
			giveError("X and Y must be less than width and height, respectively.", &me);
			me.untoast();
			return true;
		}
		return true;
	});
	obj_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &obj_dlg, false));
	obj_dlg["w"].setTextToNum(ter.obj_size.x);
	obj_dlg["h"].setTextToNum(ter.obj_size.y);
	obj_dlg["x"].setTextToNum(ter.obj_pos.x);
	obj_dlg["y"].setTextToNum(ter.obj_pos.y);
	obj_dlg["id"].setTextToNum(ter.obj_num);
	obj_dlg["id"].triggerFocusHandler(obj_dlg, "w", true);
	obj_dlg.run();
	if(obj_dlg.accepted()) {
		ter.obj_num = obj_dlg["id"].getTextAsNum();
		ter.obj_pos.x = obj_dlg["x"].getTextAsNum();
		ter.obj_pos.y = obj_dlg["y"].getTextAsNum();
		ter.obj_size.x = obj_dlg["w"].getTextAsNum();
		ter.obj_size.y = obj_dlg["h"].getTextAsNum();
		// If you changed the size, propagate it to other terrains in the same object.
		// If the position is the same as an existing terrain in that object, remove that terrain from the object.
		for(cTerrain update : scenario.ter_types) {
			if(update.obj_num == ter.obj_num) {
				if(update.obj_pos == ter.obj_pos) {
					update.obj_pos = update.obj_size = loc();
					update.obj_num = 0;
				} else {
					update.obj_size = ter.obj_size;
				}
			}
		}
	}
	return true;
}

void edit_ter_type(ter_num_t which) {
	using namespace std::placeholders;
	cDialog ter_dlg("edit-terrain");
	// Attach handlers
	ter_dlg["pict"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,2999,"terrain graphic"));
	ter_dlg["pickpict"].attachClickHandler(std::bind(pick_picture,PIC_TER,_1,"pict","graphic"));
	ter_dlg["pickanim"].attachClickHandler(std::bind(pick_picture,PIC_TER_ANIM,_1,"pict","graphic"));
	ter_dlg["light"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,10,"light radius"));
	ter_dlg["trans"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,scenario.ter_types.size() - 1,"\"transform to what?\""));
	ter_dlg["ground"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,255,"ground type"));
	ter_dlg["trimter"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,scenario.ter_types.size() - 1,"trim terrain"));
	ter_dlg["trim"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,18,"trim type"));
	ter_dlg["prop"].attachFocusHandler(fill_ter_flag_info);
	ter_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &ter_dlg, false));
	ter_dlg["arena"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,999 + scenario.towns.size(),"combat areana"));
	// TODO: Add focus handler for key
	ter_dlg["object"].attachClickHandler(std::bind(edit_ter_obj, _1, std::ref(which)));
	ter_dlg.attachClickHandlers(std::bind(finish_editing_ter,_1,_2,std::ref(which)), {"left", "right", "done"});
	ter_dlg["picktrim"].attachClickHandler(std::bind(pick_string,"trim-names", _1, "trim", ""));
	ter_dlg["pickarena"].attachClickHandler(std::bind(pick_string,"arena-names", _1, "arena", ""));
	ter_dlg["help"].attachClickHandler(std::bind(show_help, "ter-type-help", _1, 16));
	ter_dlg.attachClickHandlers(pick_ter_flag, {"pickflag1", "pickflag2", "pickflag3", "editspec", "picktrans"});
	ter_dlg["picktown"].attachClickHandler([](cDialog& me, std::string, eKeyMod) -> bool {
		int i = me["arena"].getTextAsNum();
		if(i < 1000) i = -1; else i -= 1000;
		i = choose_text(STRT_TOWN, i, &me, "Select a town to base the combat arena on:");
		if(i >= 0) me["arena"].setTextToNum(i + 1000);
		return true;
	});
	fill_ter_info(ter_dlg,which);
	ter_dlg.run();
}

static void put_monst_info_in_dlog(cDialog& me, cMonster& monst, mon_num_t which) {
	std::ostringstream strb;
	
	if(monst.picture_num < 1000)
		dynamic_cast<cPict&>(me["icon"]).setPict(monst.picture_num,PIC_MONST);
	else {
		ePicType type_g = PIC_CUSTOM_MONST;
		short size_g = monst.picture_num / 1000;
		switch(size_g){
			case 2:
				type_g += PIC_WIDE;
				break;
			case 3:
				type_g += PIC_TALL;
				break;
			case 4:
				type_g += PIC_LARGE;
				break;
		}
		dynamic_cast<cPict&>(me["icon"]).setPict(monst.picture_num,type_g);
	}
	dynamic_cast<cPict&>(me["talkpic"]).setPict(monst.default_facial_pic, PIC_TALK);
	me["num"].setTextToNum(which);
	me["name"].setText(monst.m_name);
	me["pic"].setTextToNum(monst.picture_num);
	strb << "Width = " << int(monst.x_width);
	me["w"].setText(strb.str());
	strb.str("");
	strb << "Height = " << int(monst.y_width);
	me["h"].setText(strb.str());
	me["level"].setTextToNum(monst.level);
	me["health"].setTextToNum(monst.m_health);
	me["armor"].setTextToNum(monst.armor);
	me["skill"].setTextToNum(monst.skill);
	me["speed"].setTextToNum(monst.speed);
	me["mage"].setTextToNum(monst.mu);
	me["priest"].setTextToNum(monst.cl);
	me["dice1"].setTextToNum(monst.a[0].dice);
	me["sides1"].setTextToNum(monst.a[0].sides);
	me["dice2"].setTextToNum(monst.a[1].dice);
	me["sides2"].setTextToNum(monst.a[1].sides);
	me["dice3"].setTextToNum(monst.a[2].dice);
	me["sides3"].setTextToNum(monst.a[2].sides);
	me["talk"].setTextToNum(short(monst.default_facial_pic));
	me["treas"].setTextToNum(monst.treasure);
	
	cLedGroup& attitude = dynamic_cast<cLedGroup&>(me["attitude"]);
	switch(monst.default_attitude) {
		case 0:
			attitude.setSelected("docile");
			break;
		case 1:
			attitude.setSelected("A");
			break;
		case 2:
			attitude.setSelected("friendly");
			break;
		case 3:
			attitude.setSelected("B");
			break;
	}
	
	me["type"].setText(get_str("traits",33 + int(monst.m_type) * 2));
	me["type1"].setText(get_str("monster-abilities",130 + int(monst.a[0].type)));
	me["type2"].setText(get_str("monster-abilities",130 + int(monst.a[1].type)));
	me["type3"].setText(get_str("monster-abilities",130 + int(monst.a[2].type)));
	
	// Summoning!
	std::vector<eSpell> summoned_by;
	// General summon spells
	switch(monst.summon_type) {
		case 1: summoned_by.push_back(eSpell::SUMMON_BEAST); summoned_by.push_back(eSpell::SUMMON_WEAK); break;
		case 2: summoned_by.push_back(eSpell::SUMMON); break;
		case 3: summoned_by.push_back(eSpell::SUMMON_MAJOR); break;
	}
	// Special summon spells
	switch(which) {
		case 85: summoned_by.push_back(eSpell::DEMON); break;
		case 80: summoned_by.push_back(eSpell::SUMMON_RAT); break;
		case 125: summoned_by.push_back(eSpell::SUMMON_SPIRIT); // deliberate fallthrough
		case 126: summoned_by.push_back(eSpell::SUMMON_HOST); break;
		case 99: case 100: summoned_by.push_back(eSpell::STICKS_TO_SNAKES); break;
		case 122: summoned_by.push_back(eSpell::SUMMON_GUARDIAN); break;
	}
	
	strb.str("");
	bool first = true;
	if(summoned_by.empty())
		strb << "None";
	else for(eSpell spell : summoned_by) {
		if(!first) strb << ", ";
		first = false;
		strb << (*spell).name();
	}
	me["summons"].setText(strb.str());
}

static bool check_monst_pic(cDialog& me, std::string id, bool losing, cMonster& monst) {
	if(!losing) return true;
	static size_t max_preset = m_pic_index.size() - 1;
	static const std::string error = "Non-customized monster pic must be from 0 to " + std::to_string(max_preset) + ".";
	if(check_range(me, id, losing, 0, 4999, "Monster pic")) {
		pic_num_t pic = me[id].getTextAsNum();
		monst.picture_num = pic;
		cPict& icon = dynamic_cast<cPict&>(me["icon"]);
		switch(pic / 1000) {
			case 0:
				if(cre(pic,0,max_preset,error,"",&me)) return false;
				monst.x_width = m_pic_index[monst.picture_num].x;
				monst.y_width = m_pic_index[monst.picture_num].y;
				icon.setPict(pic, PIC_MONST);
				break;
			case 1:
				monst.x_width = 1;
				monst.y_width = 1;
				icon.setPict(pic, PIC_MONST);
				break;
			case 2:
				monst.x_width = 2;
				monst.y_width = 1;
				icon.setPict(pic, PIC_MONST_WIDE);
				break;
			case 3:
				monst.x_width = 1;
				monst.y_width = 2;
				icon.setPict(pic, PIC_MONST_TALL);
				break;
			case 4:
				monst.x_width = 2;
				monst.y_width = 2;
				icon.setPict(pic, PIC_MONST_LG);
				break;
		}
		std::ostringstream strb;
		strb << "Width = " << int(monst.x_width);
		me["w"].setText(strb.str());
		strb.str("");
		strb << "Height = " << int(monst.y_width);
		me["h"].setText(strb.str());
	}
	return true;
}

static bool save_monst_info(cDialog& me, cMonster& monst) {
	
	monst.m_name = me["name"].getText();
	monst.picture_num = me["pic"].getTextAsNum();
	monst.level = me["level"].getTextAsNum();
	monst.m_health = me["health"].getTextAsNum();
	monst.armor = me["armor"].getTextAsNum();
	monst.skill = me["skill"].getTextAsNum();
	monst.speed = me["speed"].getTextAsNum();
	monst.mu = me["mage"].getTextAsNum();
	monst.cl = me["priest"].getTextAsNum();
	
	monst.a[0].dice = me["dice1"].getTextAsNum();
	monst.a[1].dice = me["dice2"].getTextAsNum();
	monst.a[2].dice = me["dice3"].getTextAsNum();
	monst.a[0].sides = me["sides1"].getTextAsNum();
	monst.a[1].sides = me["sides2"].getTextAsNum();
	monst.a[2].sides = me["sides3"].getTextAsNum();
	
	monst.default_facial_pic = me["talk"].getTextAsNum();
	monst.treasure = me["treas"].getTextAsNum();
	std::string def_att = dynamic_cast<cLedGroup&>(me["attitude"]).getSelected();
	if(def_att == "docile") monst.default_attitude = 0;
	else if(def_att == "friendly") monst.default_attitude = 2;
	else if(def_att == "A") monst.default_attitude = 1;
	else if(def_att == "B") monst.default_attitude = 3;
	return true;
}

static bool edit_monst_type_event_filter(cDialog& me,std::string hit,cMonster& monst,short& which) {
	short i;
	cMonster temp_monst;
	
	if(hit == "okay") {
		if(save_monst_info(me,monst)) {
			scenario.scen_monsters[which] = monst;
			me.toast(true);
		}
	} else if(hit == "abils") {
		if(!save_monst_info(me,monst)) return false;
		temp_monst = edit_monst_abil(monst,which,me);
		if(temp_monst.level < 255)
			monst = temp_monst;
		put_monst_info_in_dlog(me,monst,which);
	} else if(hit == "left") {
		if(!save_monst_info(me,monst)) return false;
		scenario.scen_monsters[which] = monst;
		which--;
		// TODO: Use size() once scen_monsters becomes a vector
		if(which < 1) which = 255;
		monst = scenario.scen_monsters[which];
		put_monst_info_in_dlog(me,monst,which);
	} else if(hit == "right") {
		if(!save_monst_info(me,monst)) return false;
		scenario.scen_monsters[which] = monst;
		which++;
		if(which > 255) which = 1;
		monst = scenario.scen_monsters[which];
		put_monst_info_in_dlog(me,monst,which);
	} else if(hit == "picktype") {
		if(!save_monst_info(me,monst)) return false;
		i = choose_text(STRT_RACE,int(monst.m_type),&me,"Choose Monster Type:");
		if(i >= 0) {
			monst.m_type = (eRace) i;
			put_monst_info_in_dlog(me,monst,which);
		}
	} else if(hit == "picktype1") {
		if(!save_monst_info(me,monst)) return false;
		i = choose_text_res("monster-abilities",130,139,int(monst.a[0].type),&me,"Choose Attack 1 Type:");
		if(i >= 0) {
			monst.a[0].type = eMonstMelee(i);
			put_monst_info_in_dlog(me,monst,which);
		}
	} else if(hit == "picktype2") {
		if(!save_monst_info(me,monst)) return false;
		i = choose_text_res("monster-abilities",130,139,int(monst.a[1].type),&me,"Choose Attack 2 & 3 Type:");
		if(i >= 0) {
			monst.a[1].type = monst.a[2].type = eMonstMelee(i);
			put_monst_info_in_dlog(me,monst,which);
		}
	} else if(hit == "picktype3") {
		if(!save_monst_info(me,monst)) return false;
		i = choose_text_res("monster-abilities",130,139,int(monst.a[2].type),&me,"Choose Attack 3 Type:");
		if(i >= 0) {
			monst.a[2].type = eMonstMelee(i);
			put_monst_info_in_dlog(me,monst,which);
		}
	}
	return true;
}

static bool check_monst_dice(cDialog& me, std::string fld, bool losing) {
	if(fld[0] == 'd') return check_range_msg(me, fld, losing, 0, 20, "attack number of dice", "0 means no attack");
	char dice[] = {'d', 'i', 'c', 'e', fld.back(), 0};
	if(me[dice].getTextAsNum() == 0) return true;
	return check_range(me, fld, losing, 1, 50, "attack damage per die");
}

static bool pick_monst_picture(cDialog& me) {
	bool result = pick_picture(PIC_MONST, me, "pic", "icon");
	me["pic"].triggerFocusHandler(me, "pic", true);
	return result;
}

short edit_monst_type(short which) {
	using namespace std::placeholders;
	cMonster monst = scenario.scen_monsters[which];
	
	cDialog monst_dlg("edit-monster");
	monst_dlg["pickicon"].attachClickHandler(std::bind(pick_monst_picture,_1));
	monst_dlg["picktalk"].attachClickHandler(std::bind(pick_picture,PIC_TALK,_1,"talk","talkpic"));
	monst_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &monst_dlg, false));
	monst_dlg["pic"].attachFocusHandler(std::bind(check_monst_pic, _1, _2, _3, std::ref(monst)));
	monst_dlg["level"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 40, "level"));
	monst_dlg["health"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 2500, "health"));
	monst_dlg["armor"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 50, "armor"));
	monst_dlg["skill"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 40, "skill"));
	monst_dlg["speed"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 2, 12, "speed"));
	monst_dlg["mage"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 7, "magic spells"));
	monst_dlg["priest"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 7, "priest spells"));
	monst_dlg["treas"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 4, "treasure"));
	monst_dlg.attachFocusHandlers(check_monst_dice,{"dice1","dice2","dice3","sides1","sides2","sides3"});
	monst_dlg.attachClickHandlers(std::bind(edit_monst_type_event_filter,_1,_2,std::ref(monst),std::ref(which)),{"okay","abils","left","right","picktype","picktype1","picktype2","picktype3"});
	
	put_monst_info_in_dlog(monst_dlg, monst, which);
	
	monst_dlg.run();
	return 0;
}

static void put_monst_abils_in_dlog(cDialog& me, cMonster& monst) {
	
	me["loot-item"].setTextToNum(monst.corpse_item);
	me["loot-chance"].setTextToNum(monst.corpse_item_chance);
	me["magic-res"].setTextToNum(monst.magic_res);
	me["fire-res"].setTextToNum(monst.fire_res);
	me["cold-res"].setTextToNum(monst.cold_res);
	me["poison-res"].setTextToNum(monst.poison_res);
	me["onsee"].setTextToNum(monst.see_spec);
	me["snd"].setTextToNum(monst.ambient_sound);
	
	if(monst.mindless)
		dynamic_cast<cLed&>(me["mindless"]).setState(led_red);
	if(monst.invuln)
		dynamic_cast<cLed&>(me["invuln"]).setState(led_red);
	if(monst.invisible)
		dynamic_cast<cLed&>(me["invis"]).setState(led_red);
	if(monst.guard)
		dynamic_cast<cLed&>(me["guard"]).setState(led_red);
	
	dynamic_cast<cLedGroup&>(me["summon"]).setSelected("s" + boost::lexical_cast<std::string,short>(monst.summon_type));
	
	cStack& abils = dynamic_cast<cStack&>(me["abils"]);
	abils.setPageCount(0); // This clears out any data still in the stack
	abils.setPageCount(1);
	int i = 0;
	for(auto& abil : monst.abil) {
		if(!abil.second.active) continue;
		std::string id = std::to_string((i % 4) + 1);
		if(i % 4 == 0) {
			abils.setPage(i / 4);
			for(int j = 0; j < 4; j++) {
				std::string id = std::to_string(j + 1);
				me["abil-name" + id].setText("");
				me["abil-edit" + id].setText("Add");
			}
		} else if(i % 4 == 3) abils.addPage();
		me["abil-name" + id].setText(abil.second.to_string(abil.first));
		me["abil-edit" + id].setText("Edit");
		i++;
	}
	if(i % 4 == 0) {
		abils.setPage(i / 4);
		for(int j = 0; j < 4; j++) {
			std::string id = std::to_string(j + 1);
			me["abil-name" + id].setText("");
			me["abil-edit" + id].setText("Add");
		}
	}
	abils.setPage(0);
	
	if(abils.getPageCount() <= 1) {
		me["abil-up"].hide();
		me["abil-down"].hide();
	} else {
		me["abil-up"].show();
		me["abil-down"].show();
	}
}

static bool save_monst_abils(cDialog& me, cMonster& monst) {
	monst.summon_type = boost::lexical_cast<short>(dynamic_cast<cLedGroup&>(me["summon"]).getSelected().substr(1));
	
 	monst.corpse_item = me["loot-item"].getTextAsNum();
	monst.corpse_item_chance = me["loot-chance"].getTextAsNum();
	monst.magic_res = me["magic-res"].getTextAsNum();
	monst.fire_res = me["fire-res"].getTextAsNum();
	monst.cold_res = me["cold-res"].getTextAsNum();
	monst.poison_res = me["poison-res"].getTextAsNum();
	monst.see_spec = me["onsee"].getTextAsNum();
	monst.ambient_sound = me["snd"].getTextAsNum();
	
	monst.mindless = dynamic_cast<cLed&>(me["mindless"]).getState() != led_off;
	monst.invuln = dynamic_cast<cLed&>(me["invuln"]).getState() != led_off;
	monst.invisible = dynamic_cast<cLed&>(me["invis"]).getState() != led_off;
	monst.guard = dynamic_cast<cLed&>(me["guard"]).getState() != led_off;
	return true;
}

static bool edit_monst_abil_event_filter(cDialog& me,std::string hit,cMonster& monst) {
	using namespace std::placeholders;
	
	if(hit == "cancel") {
		monst.level = 255;
		me.toast(false);
	} else if(hit == "okay") {
		if(save_monst_abils(me, monst))
			me.toast(true);
	} else if(hit == "abil-up") {
		cStack& abils = dynamic_cast<cStack&>(me["abils"]);
		if(abils.getPage() == 0) abils.setPage(abils.getPageCount() - 1);
		else abils.setPage(abils.getPage() - 1);
	} else if(hit == "abil-down") {
		cStack& abils = dynamic_cast<cStack&>(me["abils"]);
		if(abils.getPage() >= abils.getPageCount() - 1) abils.setPage(0);
		else abils.setPage(abils.getPage() + 1);
	} else if(hit == "edit-see") {
		short spec = me["onsee"].getTextAsNum();
		if(spec < 0 || spec > 255) {
			spec = get_fresh_spec(0);
			if(spec < 0) {
				giveError("You can't create a new scenario special encounter because there are no more free special nodes.",
						  "To free a special node, set its type to No Special and set its Jump To special to -1.", &me);
				return true;
			}
		}
		if(edit_spec_enc(spec,0,&me))
			me["onsee"].setTextToNum(spec);
	} else if(hit == "pick-snd") {
		int i = me["snd"].getTextAsNum();
		i = choose_text(STRT_SND, i, &me, "Select monster vocalization sound:");
		me["snd"].setTextToNum(i);
	}
	return true;
}

static short get_monst_abil_num(std::string prompt, int min, int max, cDialog& parent) {
	cDialog numPanel("get-mabil-num", &parent);
	numPanel["okay"].attachClickHandler(std::bind(&cDialog::toast, &numPanel, false));
	numPanel["prompt"].setText(prompt + " (" + std::to_string(min) + "-" + std::to_string(max) + ") ");
	numPanel["number"].setTextToNum(min);
	numPanel.run();
	
	int result = numPanel["number"].getTextAsNum();
	if(result < min) return min;
	if(result > max) return max;
	return result;
}

static void fill_monst_abil_detail(cDialog& me, cMonster& monst, eMonstAbil abil, uAbility detail) {
	eMonstAbilCat cat = getMonstAbilCategory(abil);
	me["monst"].setText(monst.m_name);
	me["name"].setText(detail.to_string(abil));
	// These names start at line 80 in the strings file, but the first valid ability is ID 1, so add 79.
	me["type"].setText(get_str("monster-abilities", 79 + int(abil)));
	// Action points
	if(cat == eMonstAbilCat::MISSILE) {
		if(detail.missile.type == eMonstMissile::ARROW || detail.missile.type == eMonstMissile::BOLT || detail.missile.type == eMonstMissile::SPINE)
			me["ap"].setTextToNum(3);
		else me["ap"].setTextToNum(2);
	} else if(cat == eMonstAbilCat::GENERAL) {
		if(detail.gen.type == eMonstGen::TOUCH)
			me["ap"].setText("0 (part of standard attack)");
		else if(abil == eMonstAbil::DAMAGE2)
			me["ap"].setTextToNum(4);
		else me["ap"].setTextToNum(3);
	} else if(abil == eMonstAbil::RAY_HEAT)
		me["ap"].setTextToNum(1);
	else if(abil == eMonstAbil::MISSILE_WEB)
		me["ap"].setTextToNum(3);
	else if(abil == eMonstAbil::SPECIAL)
		me["ap"].setTextToNum(detail.special.extra2);
	else if(cat == eMonstAbilCat::SPECIAL)
		me["ap"].setText("0 (passive ability)");
	// Subtype
	if(cat == eMonstAbilCat::MISSILE)
		me["subtype"].setText(get_str("monster-abilities", 110 + int(detail.missile.type)));
	else if(cat == eMonstAbilCat::GENERAL)
		me["subtype"].setText(get_str("monster-abilities", 120 + int(detail.gen.type)));
	else if(cat == eMonstAbilCat::SUMMON)
		me["subtype"].setText(get_str("monster-abilities", 150 + int(detail.summon.type)));
	// Missile and range
	if(cat == eMonstAbilCat::MISSILE || cat == eMonstAbilCat::GENERAL) {
		if(cat == eMonstAbilCat::GENERAL && detail.gen.type == eMonstGen::TOUCH) {
			me["missile"].hide();
			me["pick-missile"].hide();
			me["missile-pic"].hide();
			me["missile-touch"].show();
			me["range"].hide();
			me["range-touch"].show();
		} else {
			me["missile"].show();
			me["pick-missile"].show();
			me["missile-pic"].show();
			me["missile-touch"].hide();
			me["range"].show();
			me["range-touch"].hide();
		}
		miss_num_t missile;
		int range;
		if(cat == eMonstAbilCat::GENERAL) {
			missile = detail.gen.pic;
			range = detail.gen.range;
		} else {
			missile = detail.missile.pic;
			range = detail.missile.range;
		}
		me["missile"].setTextToNum(missile);
		dynamic_cast<cPict&>(me["missile-pic"]).setPict(missile, PIC_MISSILE);
		me["range"].setTextToNum(range);
	}
	// Chance
	if(cat != eMonstAbilCat::SPECIAL) {
		int odds = 0;
		if(cat == eMonstAbilCat::MISSILE) odds = detail.missile.odds;
		else if(cat == eMonstAbilCat::GENERAL) odds = detail.gen.odds;
		else if(cat == eMonstAbilCat::SUMMON) odds = detail.summon.chance;
		else if(cat == eMonstAbilCat::RADIATE) odds = detail.radiate.chance;
		me["odds"].setTextToNum(odds);
	}
	// Field type
	if(cat == eMonstAbilCat::RADIATE || abil == eMonstAbil::FIELD) {
		if(abil == eMonstAbil::FIELD)
			me["extra"].setTextToNum(int(detail.gen.fld));
		else me["field"].setTextToNum(int(detail.radiate.type));
	}
	// Other type-specific fields
	if(cat == eMonstAbilCat::MISSILE) {
		me["dice"].setTextToNum(detail.missile.dice);
		me["sides"].setTextToNum(detail.missile.sides);
		me["skill"].setTextToNum(detail.missile.skill);
	} else if(cat == eMonstAbilCat::GENERAL) {
		me["strength"].setTextToNum(detail.gen.strength);
		bool haveExtra = false;
		cControl& title = me["extra-title"];
		cControl& field = me["extra"];
		cControl& pick = me["pick-extra"];
		if(abil == eMonstAbil::FIELD) {
			haveExtra = true;
			title.setText("Field Type:");
			field.setTextToNum(int(detail.gen.fld));
		} else if(abil == eMonstAbil::DAMAGE || abil == eMonstAbil::DAMAGE2) {
			haveExtra = true;
			title.setText("Damage Type:");
			field.setTextToNum(int(detail.gen.dmg));
		} else if(abil == eMonstAbil::STATUS || abil == eMonstAbil::STATUS2 || abil == eMonstAbil::STUN) {
			haveExtra = true;
			title.setText("Status Effect:");
			field.setTextToNum(int(detail.gen.stat));
		}
		if(haveExtra)
			title.show(), field.show(), pick.show();
		else title.hide(), field.hide(), pick.hide();
	} else if(cat == eMonstAbilCat::SUMMON) {
		switch(detail.summon.type) {
			case eMonstSummon::TYPE: me["summon"].setText(scenario.scen_monsters[detail.summon.what].m_name); break;
			case eMonstSummon::LEVEL: me["summon"].setTextToNum(detail.summon.what); break;
			case eMonstSummon::SPECIES: me["summon"].setText(get_str("traits", detail.summon.what * 2 + 33)); break;
		}
		me["max"].setTextToNum(detail.summon.max);
		me["min"].setTextToNum(detail.summon.min);
		me["len"].setTextToNum(detail.summon.len);
	} else if(cat == eMonstAbilCat::SPECIAL) {
		me["extra1"].setTextToNum(detail.special.extra1);
		me["extra2"].setTextToNum(detail.special.extra2);
		me["extra3"].setTextToNum(detail.special.extra3);
	}
}

static void save_monst_abil_detail(cDialog& me, eMonstAbil abil, uAbility& detail) {
	eMonstAbilCat cat = getMonstAbilCategory(abil);
	if(cat == eMonstAbilCat::MISSILE) {
		detail.missile.pic = me["missile"].getTextAsNum();
		detail.missile.dice = me["dice"].getTextAsNum();
		detail.missile.sides = me["sides"].getTextAsNum();
		detail.missile.range = me["range"].getTextAsNum();
		detail.missile.odds = me["odds"].getTextAsNum();
		detail.missile.skill = me["skill"].getTextAsNum();
	} else if(cat == eMonstAbilCat::GENERAL) {
		detail.gen.pic = me["missile"].getTextAsNum();
		detail.gen.strength = me["strength"].getTextAsNum();
		detail.gen.range = me["range"].getTextAsNum();
		detail.gen.odds = me["odds"].getTextAsNum();
	} else if(cat == eMonstAbilCat::SUMMON) {
		detail.summon.max = me["max"].getTextAsNum();
		detail.summon.min = me["min"].getTextAsNum();
		detail.summon.len = me["len"].getTextAsNum();
		detail.summon.chance = me["odds"].getTextAsNum();
	} else if(cat == eMonstAbilCat::RADIATE) {
		detail.radiate.chance = me["odds"].getTextAsNum();
	} else if(cat == eMonstAbilCat::SPECIAL) {
		detail.special.extra1 = me["extra1"].getTextAsNum();
		detail.special.extra2 = me["extra2"].getTextAsNum();
		detail.special.extra3 = me["extra3"].getTextAsNum();
	}
}

static bool edit_monst_abil_detail(cDialog& me, std::string hit, cMonster& monst) {
	eMonstAbil abil;
	uAbility abil_params;
	std::map<eMonstAbil,uAbility>::iterator iter;
	if(me[hit].getText() == "Add") {
		int i = choose_text_res("monster-abilities", 1, 70, 0, &me, "Select an ability to add.");
		if(i < 0) return true;
		eMonstAbilTemplate tmpl = eMonstAbilTemplate(i);
		int param = 0;
		switch(tmpl) {
			case eMonstAbilTemplate::RADIATE_FIRE:
			case eMonstAbilTemplate::RADIATE_ICE:
			case eMonstAbilTemplate::RADIATE_SHOCK:
			case eMonstAbilTemplate::RADIATE_ANTIMAGIC:
			case eMonstAbilTemplate::RADIATE_SLEEP:
			case eMonstAbilTemplate::RADIATE_STINK:
			case eMonstAbilTemplate::RADIATE_BLADE:
			case eMonstAbilTemplate::RADIATE_WEB:
				param = get_monst_abil_num("Percentage chance of radiating:", 0, 100, me);
				break;
			case eMonstAbilTemplate::SUMMON_5:
			case eMonstAbilTemplate::SUMMON_20:
			case eMonstAbilTemplate::SUMMON_50:
				param = choose_text(STRT_MONST, 0, &me, "Summon which monster?");
				break;
			case eMonstAbilTemplate::SPECIAL:
			case eMonstAbilTemplate::HIT_TRIGGERS:
			case eMonstAbilTemplate::DEATH_TRIGGERS:
				param = get_fresh_spec(0);
				if(param < 0) {
					giveError("You can't create a new scenario special encounter because there are no more free special nodes.",
							  "To free a special node, set its type to No Special and set its Jump To special to -1.", &me);
					return true;
				}
				if(!edit_spec_enc(param,0,&me))
					return true;
				break;
			case eMonstAbilTemplate::TOUCH_POISON:
				param = get_monst_abil_num("Poison strength:", 0, 8, me);
				break;
			case eMonstAbilTemplate::BREATH_FIRE:
			case eMonstAbilTemplate::BREATH_FROST:
			case eMonstAbilTemplate::BREATH_ELECTRICITY:
			case eMonstAbilTemplate::BREATH_DARKNESS:
				param = get_monst_abil_num("Breath weapon strength:", 0, 4, me);
				break;
			default: break; // None of the other templates take parameters
		}
		// Protect from overwriting an existing ability.
		auto save_abils = monst.abil;
		iter = monst.addAbil(tmpl, param);
		// Normally it'll never return end(), but if a new template was added and not implemented, it would.
		if(iter == monst.abil.end()) {
			giveError("Failed to add the new ability. When reporting this, mention which ability you tried to add.", &me);
			return true;
		}
		if(save_abils.find(iter->first) != save_abils.end() && save_abils[iter->first].active) {
			// TODO: Warn about overwriting an ability and give a choce between keeping the old or the new
			bool overwrite = true;
			if(!overwrite) {
				monst.abil = save_abils;
				return true;
			}
		}
		size_t iShow = std::distance(monst.abil.begin(), iter);
		dynamic_cast<cStack&>(me["abils"]).setPage(iShow / 4);
		if(tmpl < eMonstAbilTemplate::CUSTOM_MISSILE && tmpl != eMonstAbilTemplate::SPECIAL) {
			put_monst_abils_in_dlog(me, monst);
			return true;
		}
	} else {
		size_t which = 4 * dynamic_cast<cStack&>(me["abils"]).getPage() + (hit[9] - '1');
		iter = monst.abil.begin();
		std::advance(iter, which);
	}
	abil = iter->first;
	abil_params = iter->second;
	std::string which_dlg = "special";
	eMonstAbilCat cat = getMonstAbilCategory(abil);
	switch(cat) {
		case eMonstAbilCat::MISSILE: which_dlg = "missile"; break;
		case eMonstAbilCat::GENERAL: which_dlg = "general"; break;
		case eMonstAbilCat::SUMMON: which_dlg = "summon"; break;
		case eMonstAbilCat::RADIATE: which_dlg = "radiate"; break;
		case eMonstAbilCat::INVALID: return true;
		case eMonstAbilCat::SPECIAL: break;
	}
	using namespace std::placeholders;
	cDialog abil_dlg("edit-mabil-" + which_dlg, &me);
	abil_dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, _1, true));
	abil_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, _1, false));
	abil_dlg["delete"].attachClickHandler([&monst,iter](cDialog& me,std::string,eKeyMod){
		// TODO: Show confirmation first?
		return me.toast(false), monst.abil.erase(iter), true;
	});
	auto onfocus = [&](cDialog& me,std::string,bool losing) -> bool {
		if(!losing) return true;
		save_monst_abil_detail(me, abil, abil_params);
		fill_monst_abil_detail(me, monst, abil, abil_params);
		return true;
	};
	abil_dlg.forEach([&onfocus](std::string, cControl& ctrl){
		if(ctrl.getType() == CTRL_FIELD)
			ctrl.attachFocusHandler(onfocus);
	});
	
	if(cat == eMonstAbilCat::MISSILE || cat == eMonstAbilCat::GENERAL || cat == eMonstAbilCat::SUMMON) {
		int first, last;
		if(cat == eMonstAbilCat::MISSILE) first = 110, last = 117;
		else if(cat == eMonstAbilCat::GENERAL) first = 120, last = 124;
		else if(cat == eMonstAbilCat::SUMMON) first = 150, last = 152;
		abil_dlg["pick-subtype"].attachClickHandler([&,cat,first,last](cDialog& me,std::string,eKeyMod) -> bool {
			save_monst_abil_detail(me, abil, abil_params);
			int i = 0;
			if(cat == eMonstAbilCat::MISSILE) i = int(abil_params.missile.type);
			else if(cat == eMonstAbilCat::GENERAL) i = int(abil_params.gen.type);
			else if(cat == eMonstAbilCat::SUMMON) i = int(abil_params.summon.type);
			i = choose_text_res("monster-abilities", first, last, i + first, &me, "Select ability subtype:");
			if(i < 0) return true;
			if(cat == eMonstAbilCat::MISSILE) abil_params.missile.type = eMonstMissile(i);
			else if(cat == eMonstAbilCat::GENERAL) abil_params.gen.type = eMonstGen(i);
			else if(cat == eMonstAbilCat::SUMMON) abil_params.summon.type = eMonstSummon(i);
			fill_monst_abil_detail(me, monst, abil, abil_params);
			return true;
		});
		if(cat != eMonstAbilCat::SUMMON)
			abil_dlg["pick-missile"].attachClickHandler(std::bind(pick_picture, PIC_MISSILE, _1, "missile", "missile-pic"));
	}
	if(cat == eMonstAbilCat::GENERAL) {
		abil_dlg["pick-extra"].attachClickHandler([&](cDialog& me,std::string,eKeyMod) -> bool {
			save_monst_abil_detail(me, abil, abil_params);
			int i = -1;
			if(abil == eMonstAbil::FIELD) {
				i = int(abil_params.gen.fld);
				i = choose_field_type(i, &me, false);
				abil_params.gen.fld = eFieldType(i);
			} else if(abil == eMonstAbil::DAMAGE || abil == eMonstAbil::DAMAGE2) {
				i = int(abil_params.gen.dmg);
				i = choose_damage_type(i, &me);
				abil_params.gen.dmg = eDamageType(i);
			} else if(abil == eMonstAbil::STATUS || abil == eMonstAbil::STATUS2 || abil == eMonstAbil::STUN) {
				i = int(abil_params.gen.stat);
				i = choose_status_effect(i, false, &me);
				abil_params.gen.stat = eStatus(i);
			}
			fill_monst_abil_detail(me, monst, abil, abil_params);
			return true;
		});
		if(abil == eMonstAbil::FIELD)
			abil_dlg["pick-strength"].attachClickHandler([&](cDialog& me,std::string,eKeyMod) -> bool {
				save_monst_abil_detail(me, abil, abil_params);
				int i = abil_params.gen.strength;
				i = choose_text(STRT_SPELL_PAT, i, &me, "Which spell pattern?");
				abil_params.gen.strength = i;
				fill_monst_abil_detail(me, monst, abil, abil_params);
				return true;
			});
		else abil_dlg["pick-strength"].hide();
	} else if(cat == eMonstAbilCat::RADIATE) {
		abil_dlg["pick-field"].attachClickHandler([&](cDialog& me,std::string,eKeyMod) -> bool {
			save_monst_abil_detail(me, abil, abil_params);
			int i = abil_params.radiate.type;
			i = choose_field_type(i, &me, false);
			abil_params.radiate.type = eFieldType(i);
			fill_monst_abil_detail(me, monst, abil, abil_params);
			return true;
		});
	} else if(cat == eMonstAbilCat::SUMMON) {
		abil_dlg["pick-summon"].attachClickHandler([&](cDialog& me,std::string,eKeyMod) -> bool {
			save_monst_abil_detail(me, abil, abil_params);
			int i = abil_params.summon.what;
			eStrType type;
			switch(abil_params.summon.type) {
				case eMonstSummon::TYPE: type = STRT_MONST; break;
				case eMonstSummon::LEVEL: type = STRT_SUMMON; break;
				case eMonstSummon::SPECIES: type = STRT_RACE; break;
			}
			i = choose_text(type, i, &me, "Summon what?");
			abil_params.summon.what = i;
			fill_monst_abil_detail(me, monst, abil, abil_params);
			return true;
		});
	} else if(cat == eMonstAbilCat::SPECIAL) {
		if(abil == eMonstAbil::SPECIAL || abil == eMonstAbil::HIT_TRIGGER || abil == eMonstAbil::DEATH_TRIGGER)
			abil_dlg["pick-extra1"].attachClickHandler([&](cDialog& me,std::string,eKeyMod) -> bool {
				short spec = me["extra1"].getTextAsNum();
				if(spec < 0 || spec > 255) {
					spec = get_fresh_spec(0);
					if(spec < 0) {
						giveError("You can't create a new scenario special encounter because there are no more free special nodes.",
								  "To free a special node, set its type to No Special and set its Jump To special to -1.", &me);
						return true;
					}
				}
				if(edit_spec_enc(spec,0,&me))
					me["extra1"].setTextToNum(spec);
				return true;
			});
		else abil_dlg["pick-extra1"].hide();
		int iStr = int(abil) - int(eMonstAbil::SPLITS);
		for(int i = 0; i < 3; i++) {
			std::string id = "extra" + std::to_string(i + 1);
			std::string title = get_str("monster-abilities", 160 + iStr * 3 + i);
			if(title.empty()) {
				abil_dlg[id].hide();
				abil_dlg[id + "-title"].hide();
			} else abil_dlg[id + "-title"].setText(title + ":");
		}
	}
	fill_monst_abil_detail(abil_dlg, monst, abil, abil_params);
	abil_dlg.run();
	save_monst_abil_detail(abil_dlg, abil, abil_params);
	if(abil_dlg.accepted())
		iter->second = abil_params;
	put_monst_abils_in_dlog(me, monst);
	return true;
}

cMonster edit_monst_abil(cMonster initial,short which,cDialog& parent) {
	using namespace std::placeholders;
	
	cDialog monst_dlg("edit-monster-abils",&parent);
	monst_dlg["loot-item"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 399, "Item To Drop", "-1 for no item"));
	monst_dlg["loot-chance"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 100, "Dropping Chance", "-1 for no item"));
	monst_dlg.attachClickHandlers(std::bind(edit_monst_abil_detail, _1, _2, std::ref(initial)), {"abil-edit1", "abil-edit2", "abil-edit3", "abil-edit4"});
	monst_dlg.attachClickHandlers(std::bind(edit_monst_abil_event_filter, _1, _2, std::ref(initial)), {"okay", "cancel", "abil-up", "abil-down", "edit-see", "pick-snd"});
	
	monst_dlg["num"].setTextToNum(which);
	put_monst_abils_in_dlog(monst_dlg, initial);
	
	monst_dlg.run();
	return initial;
}

static void put_item_info_in_dlog(cDialog& me, cItem& item, short which) {
	me["num"].setTextToNum(which);
	me["full"].setText(item.full_name);
	me["short"].setText(item.name);
	if(item.graphic_num >= 1000) // was 150
		dynamic_cast<cPict&>(me["pic"]).setPict(item.graphic_num, PIC_CUSTOM_ITEM);
	else dynamic_cast<cPict&>(me["pic"]).setPict(item.graphic_num, PIC_ITEM);
	me["picnum"].setTextToNum(item.graphic_num);
	
	bool missile = false, weapon = false;
	
	cLedGroup& variety = dynamic_cast<cLedGroup&>(me["variety"]);
	switch(item.variety) {
		case eItemType::NO_ITEM:
			variety.setSelected("none");
			break;
		case eItemType::ONE_HANDED:
			variety.setSelected("weap1");
			weapon = true;
			break;
		case eItemType::TWO_HANDED:
			variety.setSelected("weap2");
			weapon = true;
			break;
		case eItemType::GOLD:
			variety.setSelected("gold");
			break;
		case eItemType::BOW:
			variety.setSelected("bow");
			weapon = true;
			break;
		case eItemType::ARROW:
			variety.setSelected("arrow");
			missile = true;
			break;
		case eItemType::THROWN_MISSILE:
			variety.setSelected("thrown");
			missile = true;
			weapon = true;
			break;
		case eItemType::POTION:
			variety.setSelected("potion");
			break;
		case eItemType::SCROLL:
			variety.setSelected("scroll");
			break;
		case eItemType::WAND:
			variety.setSelected("wand");
			break;
		case eItemType::TOOL:
			variety.setSelected("tool");
			break;
		case eItemType::FOOD:
			variety.setSelected("food");
			break;
		case eItemType::SHIELD:
			variety.setSelected("shield");
			break;
		case eItemType::ARMOR:
			variety.setSelected("armor");
			break;
		case eItemType::HELM:
			variety.setSelected("helm");
			break;
		case eItemType::GLOVES:
			variety.setSelected("gloves");
			break;
		case eItemType::SHIELD_2:
			variety.setSelected("shield2");
			break;
		case eItemType::BOOTS:
			variety.setSelected("boots");
			break;
		case eItemType::RING:
			variety.setSelected("ring");
			break;
		case eItemType::NECKLACE:
			variety.setSelected("necklace");
			break;
		case eItemType::WEAPON_POISON:
			variety.setSelected("poison");
			break;
		case eItemType::NON_USE_OBJECT:
			variety.setSelected("nonuse");
			break;
		case eItemType::PANTS:
			variety.setSelected("pants");
			break;
		case eItemType::CROSSBOW:
			variety.setSelected("xbow");
			weapon = true;
			break;
		case eItemType::BOLTS:
			variety.setSelected("bolt");
			missile = true;
			break;
		case eItemType::MISSILE_NO_AMMO:
			variety.setSelected("missile");
			missile = true;
			weapon = true;
			break;
		case eItemType::UNUSED1:
			variety.setSelected("unused1");
			break;
		case eItemType::SPECIAL:
			variety.setSelected("special");
			break;
	}
	
	if(missile) {
		me["missile"].show();
		me["missile-pic"].show();
		me["missile-pic"].show();
		me["choosemiss"].show();
		me["missile"].setTextToNum(item.missile);
		dynamic_cast<cPict&>(me["missile-pic"]).setPict(item.missile);
	} else {
		me["missile"].hide();
		me["missile-title"].hide();
		me["missile-pic"].hide();
		me["choosemiss"].hide();
		me["missile"].setText("0");
	}
	
	if(weapon) {
		me["skill-title"].show();
		me["weap-type"].show();
		me["choosetp"].show();
		me["weap-type"].setTextToNum(int(item.weap_type));
	} else {
		me["skill-title"].hide();
		me["weap-type"].hide();
		me["choosetp"].hide();
		me["weap-type"].setText("0");
	}
	
	me["level"].setTextToNum(item.item_level);
	me["awkward"].setTextToNum(item.awkward);
	me["bonus"].setTextToNum(item.bonus);
	me["prot"].setTextToNum(item.protection);
	me["charges"].setTextToNum(item.charges);
	me["flag"].setTextToNum(item.type_flag);
	me["value"].setTextToNum(item.value);
	me["weight"].setTextToNum(item.weight);
	me["class"].setTextToNum(item.special_class);
	me["abilname"].setText(item.getAbilName());
}

static void save_item_info(cDialog& me, cItem& item) {
	item.full_name = me["full"].getText();
	item.name = me["short"].getText();
	item.graphic_num = me["picnum"].getTextAsNum();
	
	std::string variety = dynamic_cast<cLedGroup&>(me["variety"]).getSelected();
	if(variety == "none") item.variety = eItemType::NO_ITEM;
	else if(variety == "weap1") item.variety = eItemType::ONE_HANDED;
	else if(variety == "weap2") item.variety = eItemType::TWO_HANDED;
	else if(variety == "gold") item.variety = eItemType::GOLD;
	else if(variety == "bow") item.variety = eItemType::BOW;
	else if(variety == "arrow") item.variety = eItemType::ARROW;
	else if(variety == "thrown") item.variety = eItemType::THROWN_MISSILE;
	else if(variety == "potion") item.variety = eItemType::POTION;
	else if(variety == "scroll") item.variety = eItemType::SCROLL;
	else if(variety == "wand") item.variety = eItemType::WAND;
	else if(variety == "tool") item.variety = eItemType::TOOL;
	else if(variety == "food") item.variety = eItemType::FOOD;
	else if(variety == "shield") item.variety = eItemType::SHIELD;
	else if(variety == "armor") item.variety = eItemType::ARMOR;
	else if(variety == "helm") item.variety = eItemType::HELM;
	else if(variety == "gloves") item.variety = eItemType::GLOVES;
	else if(variety == "shield2") item.variety = eItemType::SHIELD_2;
	else if(variety == "boots") item.variety = eItemType::BOOTS;
	else if(variety == "ring") item.variety = eItemType::RING;
	else if(variety == "necklace") item.variety = eItemType::NECKLACE;
	else if(variety == "poison") item.variety = eItemType::WEAPON_POISON;
	else if(variety == "nonuse") item.variety = eItemType::NON_USE_OBJECT;
	else if(variety == "pants") item.variety = eItemType::PANTS;
	else if(variety == "xbow") item.variety = eItemType::CROSSBOW;
	else if(variety == "bolt") item.variety = eItemType::BOLTS;
	else if(variety == "missile") item.variety = eItemType::MISSILE_NO_AMMO;
	else if(variety == "unused1") item.variety = eItemType::UNUSED1;
	else if(variety == "special") item.variety = eItemType::SPECIAL;
	item.weap_type = eSkill(me["weap-type"].getTextAsNum());
	item.missile = me["missile"].getTextAsNum();
	
	item.item_level = me["level"].getTextAsNum();
	if((item.variety == eItemType::GOLD || item.variety == eItemType::FOOD) && item.item_level == 0)
		item.item_level = 1;
	
	item.awkward = me["awkward"].getTextAsNum();
	item.bonus = me["bonus"].getTextAsNum();;
	item.protection = me["prot"].getTextAsNum();
	item.charges = me["charges"].getTextAsNum();
	item.type_flag = me["flag"].getTextAsNum();
	item.value = me["value"].getTextAsNum();
	item.weight = me["weight"].getTextAsNum();
	item.special_class = me["class"].getTextAsNum();
	
	int was_charges = item.charges;
	if(item.type_flag > 0 && item.charges == 0)
		item.charges = 1;
	eItemAbilCat cat = getItemAbilCategory(item.ability);
	if((cat == eItemAbilCat::USABLE || cat == eItemAbilCat::REAGENT) && item.charges == 0)
		item.charges = 1;
	if(was_charges != item.charges)
		giveError("Due to either the selected special ability or the presence of a type flag, this item's charges have been set to 1.", &me);
}

static bool edit_item_type_event_filter(cDialog& me, std::string hit, cItem& item, short& which) {
	short i;
	cItem temp_item;
	std::string variety = dynamic_cast<cLedGroup&>(me["variety"]).getSelected();
	bool valid = true;
	if(variety.substr(0,6) == "unused") valid = false;
	if(!valid && hit != "cancel" && hit.substr(0,6) != "choose") {
		giveError("The Unused item varieties are reserved for later expansions, and can't be used now.","",&me);
		return true;
	}
	
	if(hit == "cancel") {
		me.toast(false);
	} else if(hit == "okay") {
		save_item_info(me, item);
		if(!me.toast(true)) return true;
		scenario.scen_items[which] = item;
	} else if(hit == "prev") {
		save_item_info(me, item);
		scenario.scen_items[which] = item;
		which--;
		if(which < 0) which = 399;
		item = scenario.scen_items[which];
		put_item_info_in_dlog(me, item, which);
	} else if(hit == "next") {
		save_item_info(me, item);
		scenario.scen_items[which] = item;
		which++;
		if(which > 399) which = 0;
		item = scenario.scen_items[which];
		put_item_info_in_dlog(me, item, which);
	} else if(hit == "choosepic") {
		save_item_info(me, item);
		i = pick_picture(PIC_ITEM, me, "picnum", "pic");
		if(i < 0) return true;
		item.graphic_num = i;
	} else if(hit == "choosetp") {
		save_item_info(me, item);
		i = choose_text(STRT_SKILL, int(item.weap_type), &me, "Select the weapon's key skill:");
		item.weap_type = eSkill(i);
		me["weap-type"].setTextToNum(i);
	} else if(hit == "choosemiss") {
		save_item_info(me, item);
		i = pick_picture(PIC_MISSILE, me, "missile", "missile-pic");
		if(i < 0) return true;
		item.missile = i;
	} else if(hit == "desc") {
		cDialog desc_dlg("edit-text", &me);
		desc_dlg["left"].hide();
		desc_dlg["right"].hide();
		desc_dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, &desc_dlg, true));
		desc_dlg["text"].setText(item.desc);
		desc_dlg["num-lbl"].setText(item.full_name);
		desc_dlg.run();
		item.desc = desc_dlg["text"].getText();
	} else if(hit == "abils") {
		save_item_info(me, item);
		if(item.variety == eItemType::NO_ITEM) {
			giveError("You must give the item a type (weapon, armor, etc.) before you can choose its abilities.","",&me);
			return true;
		}
		if(item.variety == eItemType::GOLD || item.variety == eItemType::FOOD || item.variety == eItemType::SPECIAL) {
			giveError("Gold, Food, and Special Items cannot be given special abilities.","",&me);
			return true;
		}
		temp_item = edit_item_abil(item,which,me);
		if(temp_item.variety != eItemType::NO_ITEM)
			item = temp_item;
	}
	return true;
}

static bool change_item_variety(cDialog& me, std::string group, const cItem& item) {
	std::string hit = dynamic_cast<cLedGroup&>(me[group]).getSelected();
	
	if(hit == "arrow" || hit == "thrown" || hit == "bolt" || hit == "missile") {
		me["missile"].show();
		me["missile-pic"].show();
		me["missile-pic"].show();
		me["choosemiss"].show();
		me["missile"].setTextToNum(item.missile);
		dynamic_cast<cPict&>(me["missile-pic"]).setPict(item.missile);
	} else {
		me["missile"].hide();
		me["missile-title"].hide();
		me["missile-pic"].hide();
		me["choosemiss"].hide();
		me["missile"].setText("0");
	}
	
	if(hit.substr(0,4) == "weap" || hit == "bow" || hit == "xbow" || hit == "missile" || hit == "thrown") {
		me["skill-title"].show();
		me["weap-type"].show();
		me["choosetp"].show();
		me["weap-type"].setTextToNum(int(item.weap_type));
	} else {
		me["skill-title"].hide();
		me["weap-type"].hide();
		me["choosetp"].hide();
		me["weap-type"].setText("0");
	}
	return true;
}

short edit_item_type(short which) {
	using namespace std::placeholders;
	cItem item = scenario.scen_items[which];
	
	cDialog item_dlg("edit-item");
	item_dlg["level"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 50, "Item Level"));
	item_dlg["awkward"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 20, "Awkward"));
	item_dlg["bonus"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 60, "Bonus"));
	item_dlg["prot"].attachFocusHandler(std::bind(check_range, _1, _2, _3, -10, 20, "Protection"));
	item_dlg["charges"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 100, "Charges"));
	item_dlg["flag"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 1000, "Type Flag"));
	item_dlg["value"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 10000, "Value"));
	item_dlg["weight"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 250, "Weight"));
	item_dlg["class"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 100, "Special Class"));
	item_dlg["variety"].attachFocusHandler(std::bind(change_item_variety, _1, _2, std::ref(item)));
	item_dlg.attachClickHandlers(std::bind(edit_item_type_event_filter, _1, _2, std::ref(item), std::ref(which)), {"okay", "cancel", "prev", "next", "abils", "choosepic", "choosetp", "choosemiss", "desc"});
	
	put_item_info_in_dlog(item_dlg, item, which);
	
	item_dlg.run();
	return 0;
}

static void put_item_abils_in_dlog(cDialog& me, cItem& item, short which) {
	
	me["num"].setTextToNum(which);
	me["name"].setText(item.full_name.c_str());
	me["variety"].setText(get_str("item-types", int(item.variety) + 1));
	if(item.ability == eItemAbil::NONE)
		me["abilname"].setText("No ability");
	else me["abilname"].setText(get_str("item-abilities", int(item.ability)));
	
	dynamic_cast<cLedGroup&>(me["use-type"]).setSelected("use" + std::to_string(int(item.magic_use_type)));
	dynamic_cast<cLedGroup&>(me["treasure"]).setSelected("type" + std::to_string(item.treas_class));
	me["str1"].setTextToNum(item.abil_data[0]);
	me["str2"].setTextToNum(item.abil_data[1]);
	
	if(item.ability == eItemAbil::CALL_SPECIAL || item.ability == eItemAbil::WEAPON_CALL_SPECIAL || item.ability == eItemAbil::HIT_CALL_SPECIAL) {
		me["str1-choose"].show();
		me["str1-title"].setText("Special to call");
	} else {
		me["str1-choose"].hide();
		if(getItemAbilCategory(item.ability) == eItemAbilCat::REAGENT || items_no_strength.count(item.ability) > 0)
			me["str1-title"].setText("Unused");
		else me["str1-title"].setText("Ability strength");
	}
	
	me["str2-choose1"].show();
	me["str2-choose1"].setText("Choose");
	me["str2-choose2"].hide();
	switch(item.ability) {
		case eItemAbil::DAMAGING_WEAPON:
		case eItemAbil::EXPLODING_WEAPON:
		case eItemAbil::DAMAGE_PROTECTION:
			me["str2-title"].setText("Type of damage");
			break;
		case eItemAbil::STATUS_WEAPON:
		case eItemAbil::STATUS_PROTECTION:
		case eItemAbil::OCCASIONAL_STATUS:
		case eItemAbil::AFFECT_STATUS:
		case eItemAbil::AFFECT_PARTY_STATUS:
			me["str2-title"].setText("Which status effect");
			break;
		case eItemAbil::SLAYER_WEAPON:
		case eItemAbil::PROTECT_FROM_SPECIES:
			me["str2-title"].setText("Which species");
			break;
		case eItemAbil::BOOST_STAT:
			me["str2-title"].setText("Which statistic");
			break;
		case eItemAbil::CAST_SPELL:
			me["str2-title"].setText("Which spell");
			me["str2-choose1"].setText("Mage");
			me["str2-choose2"].show();
			break;
		case eItemAbil::SUMMONING:
		case eItemAbil::MASS_SUMMONING:
			me["str2-title"].setText("Which monster");
			break;
		default:
			me["str2-title"].setText("Unused");
			me["str2-choose1"].hide();
			break;
	}
	
	if((item.ability >= eItemAbil::BLISS_DOOM && item.ability <= eItemAbil::HEALTH_POISON) || item.ability == eItemAbil::AFFECT_STATUS || item.ability == eItemAbil::OCCASIONAL_STATUS) {
		me["use-title"].show();
		me["use-type"].show();
	} else {
		me["use-title"].hide();
		me["use-type"].hide();
	}
	
	dynamic_cast<cLed&>(me["always-id"]).setState(item.ident ? led_red : led_off);
	dynamic_cast<cLed&>(me["magic"]).setState(item.magic ? led_red : led_off);
	dynamic_cast<cLed&>(me["cursed"]).setState(item.cursed ? led_red : led_off);
	dynamic_cast<cLed&>(me["conceal"]).setState(item.concealed ? led_red : led_off);
	dynamic_cast<cLed&>(me["no-sell"]).setState(item.unsellable ? led_red : led_off);
}

static void save_item_abils(cDialog& me, cItem& item) {
	item.magic_use_type = boost::lexical_cast<eItemUse>(dynamic_cast<cLedGroup&>(me["use-type"]).getSelected().substr(3));
	item.treas_class = boost::lexical_cast<short>(dynamic_cast<cLedGroup&>(me["treasure"]).getSelected().substr(4));
	item.abil_data[0] = me["str1"].getTextAsNum();
	item.abil_data[1] = me["str2"].getTextAsNum();
	
	item.property = item.enchanted = item.contained = false;
	item.ident = dynamic_cast<cLed&>(me["always-id"]).getState() != led_off;
	item.magic = dynamic_cast<cLed&>(me["magic"]).getState() != led_off;
	item.cursed = dynamic_cast<cLed&>(me["cursed"]).getState() != led_off;
	item.unsellable = dynamic_cast<cLed&>(me["no-sell"]).getState() != led_off;
	item.concealed = dynamic_cast<cLed&>(me["conceal"]).getState() != led_off;
}

static bool edit_item_abil_event_filter(cDialog& me, std::string hit, cItem& item, short which) {
	short i;
	
	if(hit == "cancel") {
		item.variety = eItemType::NO_ITEM;
		me.toast(false);
		return true;
	} else if(hit == "okay") {
		save_item_abils(me, item);
		me.toast(true);
	} else if(hit == "str1-choose") {
		save_item_abils(me, item);
		short spec = me["str1"].getTextAsNum();
		if(spec < 0 || spec > 255) {
			spec = get_fresh_spec(0);
			if(spec < 0) {
				giveError("You can't create a new scenario special encounter because there are no more free special nodes.",
						  "To free a special node, set its type to No Special and set its Jump To special to -1.", &me);
				return true;
			}
		}
		if(edit_spec_enc(spec,0,&me)) {
			item.abil_data[0] = spec;
			me["str1"].setTextToNum(spec);
		}
	} else if(hit == "str2-choose1") {
		save_item_abils(me, item);
		i = item.abil_data[1];
		switch(item.ability) {
			case eItemAbil::DAMAGING_WEAPON:
			case eItemAbil::EXPLODING_WEAPON:
			case eItemAbil::DAMAGE_PROTECTION:
				i = choose_damage_type(i, &me);
				break;
			case eItemAbil::STATUS_WEAPON:
			case eItemAbil::STATUS_PROTECTION:
			case eItemAbil::OCCASIONAL_STATUS:
			case eItemAbil::AFFECT_STATUS:
			case eItemAbil::AFFECT_PARTY_STATUS:
				i = choose_status_effect(i, item.ability == eItemAbil::AFFECT_PARTY_STATUS, &me);
				break;
			case eItemAbil::SLAYER_WEAPON:
			case eItemAbil::PROTECT_FROM_SPECIES:
				i = choose_text(STRT_RACE, i, &me, "Which species?");
				break;
			case eItemAbil::BOOST_STAT:
				i = choose_text(STRT_SKILL, i, &me, "Boost which skill?");
				break;
			case eItemAbil::CAST_SPELL:
				i = choose_text_res("magic-names", 1, 73, i + 1, &me, "Which mage spell?");
				if(i < 0) return true;
				break;
			case eItemAbil::SUMMONING:
			case eItemAbil::MASS_SUMMONING:
				i = choose_text(STRT_MONST, i, &me, "Summon which monster type?");
				break;
			default: return true;
		}
		item.abil_data[1] = i;
		me["str2"].setTextToNum(i);
	} else if(hit == "str2-choose2") {
		save_item_abils(me, item);
		i = 100 + choose_text_res("magic-names", 101, 166, item.abil_data[1] + 1, &me, "Which priest spell?");
		if(i < 100) return true;
		item.abil_data[1] = i;
		me["str2"].setTextToNum(i);
	} else if(hit == "clear") {
		save_item_abils(me, item);
		item.ability = eItemAbil::NONE;
		put_item_abils_in_dlog(me, item, which);
	} else if(hit == "weapon") {
		save_item_abils(me, item);
		if(!isWeaponType(item.variety)) {
			giveError("You can only give an ability of this sort to a weapon.","",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 1, 14, int(item.ability), &me, "Choose Weapon Ability (inherent)");
		if(i < 0) return true;
		eItemAbil abil = eItemAbil(i + 1);
		if(abil >= eItemAbil::RETURNING_MISSILE && abil <= eItemAbil::SEEKING_MISSILE) {
			if(item.variety != eItemType::THROWN_MISSILE && item.variety != eItemType::ARROW &&
				item.variety != eItemType::BOLTS && item.variety != eItemType::MISSILE_NO_AMMO) {
				giveError("You can only give this ability to a missile.",&me);
				return true;
			}
		}
		item.ability = abil;
		put_item_abils_in_dlog(me, item, which);
	} else if(hit == "general") {
		save_item_abils(me, item);
		if(equippable.count(item.variety) == 0 || item.variety == eItemType::ARROW || item.variety == eItemType::THROWN_MISSILE || item.variety == eItemType::BOLTS){
			giveError("You can only give an ability of this sort to an non-missile item which can be equipped (like armor, or a ring).",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 30, 58, int(item.ability), &me, "Choose General Ability (inherent)");
		if(i < 0) return true;
		item.ability = eItemAbil(i + 30);
		put_item_abils_in_dlog(me, item, which);
	} else if(hit == "usable") {
		save_item_abils(me, item);
		if((item.variety == eItemType::ARROW) || (item.variety == eItemType::THROWN_MISSILE) || (item.variety == eItemType::BOLTS)){
			giveError("You can't give an ability of this sort to a missile.",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 70, 84, int(item.ability), &me, "Choose Usable Ability");
		if(i < 0) return true;
		item.ability = eItemAbil(i + 70);
		put_item_abils_in_dlog(me, item, which);
	} else if(hit == "reagent") {
		save_item_abils(me, item);
		if(item.variety != eItemType::NON_USE_OBJECT){
			giveError("You can only give an ability of this sort to an item of type Non-Use Object.",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 150, 160, int(item.ability), &me, "Choose Reagent Ability");
		if(i < 0) return true;
		item.ability = eItemAbil(i + 150);
		put_item_abils_in_dlog(me, item, which);
	}
	return true;
}

cItem edit_item_abil(cItem initial,short which_item,cDialog& parent) {
	using namespace std::placeholders;
	
	cDialog item_dlg("edit-item-abils",&parent);
	item_dlg.attachClickHandlers(std::bind(edit_item_abil_event_filter, _1, _2, std::ref(initial), which_item), {
		"okay", "cancel",
		"clear", "weapon", "general", "usable", "reagent",
		"str1-choose", "str2-choose1", "str2-choose2",
	});
	
	put_item_abils_in_dlog(item_dlg, initial, which_item);
	
	item_dlg.run();
	
	return initial;
}

static void put_spec_item_in_dlog(cDialog& me, cSpecItem& item, short which) {
	me["num"].setTextToNum(which);
	me["name"].setText(item.name);
	me["descr"].setText(item.descr);
	me["spec"].setTextToNum(item.special);
	dynamic_cast<cLed&>(me["start-with"]).setState(item.flags >= 10 ? led_red : led_off);
	dynamic_cast<cLed&>(me["usable"]).setState(item.flags % 10 > 0 ? led_red : led_off);
}

static bool save_spec_item(cDialog& me, cSpecItem& item, short which) {
	item.name = me["name"].getText().substr(0,25);
	item.descr = me["descr"].getText();
	item.special = me["spec"].getTextAsNum();
	item.flags = 0;
	if(dynamic_cast<cLed&>(me["start-with"]).getState() != led_off)
		item.flags += 10;
	if(dynamic_cast<cLed&>(me["usable"]).getState() != led_off)
		item.flags += 1;
	scenario.special_items[which] = item;
	return true;
}

static bool edit_spec_item_event_filter(cDialog& me, std::string hit, cSpecItem& item, short& which) {
	if(hit == "cancel") {
		me.toast(false);
	} else if(hit == "okay") {
		if(save_spec_item(me, item, which)) me.toast(true);
	} else if(hit == "left") {
		if(!save_spec_item(me, item, which)) return true;
		which--;
		if(which < 0) which = 49;
		item = scenario.special_items[which];
		put_spec_item_in_dlog(me, item, which);
	} else if(hit == "right") {
		if(!save_spec_item(me, item, which)) return true;
		which++;
		if(which > 49) which = 0;
		item = scenario.special_items[which];
		put_spec_item_in_dlog(me, item, which);
	} else if(hit == "edit-spec") {
		if(!save_spec_item(me, item, which)) return true;
		short spec = me["spec"].getTextAsNum();
		if((spec < 0) || (spec >= 256)) {
			spec = get_fresh_spec(0);
			if(spec < 0) {
				giveError("You can't create a new special encounter because there are no more free scenario special nodes.",
						  "To free a special node, set its type to No Special and set its Jump To special to -1.",&me);
				return true;
			}
			me["spec"].setTextToNum(spec);
		}
		edit_spec_enc(spec,0,&me);
		if(spec >= 0 && spec < 256 && scenario.scen_specials[spec].pic < 0)
			me["spec"].setTextToNum(-1);
		save_spec_item(me, item, which);
		
	}
	return true;
}

void edit_spec_item(short which_item) {
	using namespace std::placeholders;
	cSpecItem item = scenario.special_items[which_item];
	
	cDialog item_dlg("edit-special-item");
	item_dlg["spec"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 255, "Scenario special node called", "-1 for no special"));
	item_dlg.attachClickHandlers(std::bind(edit_spec_item_event_filter, _1, _2, std::ref(item), std::ref(which_item)), {"okay", "cancel", "clear", "edit-spec", "left", "right"});
	
	put_spec_item_in_dlog(item_dlg, item, which_item);
	item_dlg["clear"].hide();
	
	item_dlg.run();
}

static void put_save_rects_in_dlog(cDialog& me) {
	short i;
	
	for(i = 0; i < 3; i++) {
		std::string id = std::to_string(i + 1);
		me["top" + id].setTextToNum(scenario.store_item_rects[i].top);
		me["left" + id].setTextToNum(scenario.store_item_rects[i].left);
		me["bottom" + id].setTextToNum(scenario.store_item_rects[i].bottom);
		me["right" + id].setTextToNum(scenario.store_item_rects[i].right);
		me["town" + id].setTextToNum(scenario.store_item_towns[i]);
		
	}
}

static bool save_save_rects(cDialog& me) {
	short i;
	
	for(i = 0; i < 3; i++) {
		std::string id = std::to_string(i + 1);
		scenario.store_item_rects[i].top = me["top" + id].getTextAsNum();
		scenario.store_item_rects[i].left = me["left" + id].getTextAsNum();
		scenario.store_item_rects[i].bottom = me["bottom" + id].getTextAsNum();
		scenario.store_item_rects[i].right = me["right" + id].getTextAsNum();
		scenario.store_item_towns[i] = me["town" + id].getTextAsNum();
		if((scenario.store_item_towns[i] < -1) || (scenario.store_item_towns[i] >= 200)) {
			giveError("Towns must be in 0 to 200 range (or -1 for no save items rectangle).","",&me);
			return false;
		}
	}
	if(((scenario.store_item_towns[0] == scenario.store_item_towns[1]) &&
		 (scenario.store_item_towns[0] >= 0) && (scenario.store_item_towns[1] >= 0))
		||
		((scenario.store_item_towns[2] == scenario.store_item_towns[1]) &&
		 (scenario.store_item_towns[2] >= 0) && (scenario.store_item_towns[1] >= 0))
		||
		((scenario.store_item_towns[2] == scenario.store_item_towns[0]) &&
		 (scenario.store_item_towns[2] >= 0) && (scenario.store_item_towns[0] >= 0))
		) {
		giveError("The three towns towns with saved item rectangles must be different.","",&me);
		return false;
	}
	return true;
}

static bool edit_save_rects_event_filter(cDialog& me, std::string item_hit) {
	if(item_hit == "cancel") {
		me.toast(false);
	} else if(item_hit == "okay") {
		if(save_save_rects(me))
			me.toast(true);
	}
	return true;
}

void edit_save_rects() {
	using namespace std::placeholders;
	
	cDialog save_dlg("edit-save-rects");
	save_dlg.attachClickHandlers(std::bind(edit_save_rects_event_filter, _1, _2), {"okay"});
	
	put_save_rects_in_dlog(save_dlg);
	
	save_dlg.run();
}

static bool save_vehicles(cDialog& me, cVehicle* vehicles, const short page) {
	short i;
	
	for(i = 0; i < 6; i++) {
		std::string id = std::to_string(i + 1);
		vehicles[6 * page + i].which_town = me["town" + id].getTextAsNum();
		if(cre(vehicles[6 * page + i].which_town,
			   -1,199,"Town number must be from 0 to 199 (or -1 for it to not exist).","",&me)) return false;
		vehicles[6 * page + i].loc.x = me["x" + id].getTextAsNum();
		if(cre(vehicles[6 * page + i].loc.x,
			   0,63,"coordinates must be from 0 to 63.","",&me)) return false;
		vehicles[6 * page + i].loc.y = me["y" + id].getTextAsNum();
		if(cre(vehicles[6 * page + i].loc.y,
			   0,63,"coordinates must be from 0 to 63.","",&me)) return false;
		vehicles[6 * page + i].property = dynamic_cast<cLed&>(me["owned" + id]).getState() != led_off;
	}
	return true;
}

static void put_vehicles_in_dlog(cDialog& me, cVehicle* vehicles, const short page) {
	short i;
	
	for(i = 0; i < 6; i++) {
		std::string id = std::to_string(i + 1);
		me["num" + id].setTextToNum(6 * page + i);
		me["town" + id].setTextToNum(vehicles[6 * page + i].which_town);
		me["x" + id].setTextToNum(vehicles[6 * page + i].loc.x);
		me["y" + id].setTextToNum(vehicles[6 * page + i].loc.y);
		dynamic_cast<cLed&>(me["owned" + id]).setState(vehicles[6 * page + i].property ? led_red : led_off);
	}
}

static bool edit_vehicles_event_filter(cDialog& me, std::string hit, cVehicle* vehicles, size_t nVehicles, size_t& page) {
	
	if(hit == "okay") {
		if(save_vehicles(me, vehicles, page))
			me.toast(true);
	} else if(hit == "left") {
		if(!save_vehicles(me, vehicles, page)) return true;
		if(page == 0) page = (nVehicles - 1) / 6;
		else page--;
		put_vehicles_in_dlog(me, vehicles, page);
	} else if(hit == "right") {
		if(!save_vehicles(me, vehicles, page)) return true;
		page++;
		if(page > (nVehicles - 1) / 6) page = 0;
		put_vehicles_in_dlog(me, vehicles, page);
	}
	return true;
}

void edit_horses() {
	using namespace std::placeholders;
	size_t page = 0;
	
	cDialog horse_dlg("edit-horses");
	horse_dlg.attachClickHandlers(std::bind(edit_vehicles_event_filter, _1, _2, scenario.horses, 30, std::ref(page)), {"okay", "left", "right"});
	
	put_vehicles_in_dlog(horse_dlg, scenario.horses, page);
	
	horse_dlg.run();
}

void edit_boats() {
	using namespace std::placeholders;
	size_t page = 0;
	
	cDialog boat_dlg("edit-boats");
	boat_dlg.attachClickHandlers(std::bind(edit_vehicles_event_filter, _1, _2, scenario.boats, 30, std::ref(page)), {"okay", "left", "right"});
	
	put_vehicles_in_dlog(boat_dlg, scenario.boats, page);
	
	boat_dlg.run();
}

static bool save_add_town(cDialog& me) {
	short i;
	
	for(i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		scenario.town_to_add_to[i] = me["town" + id].getTextAsNum();
		if(cre(scenario.town_to_add_to[i],
				-1,199,"Town number must be from 0 to 199 (or -1 for no effect).","",&me)) return false;
		scenario.flag_to_add_to_town[i][0] = me["flag" + id + "-x"].getTextAsNum();
		if(cre(scenario.flag_to_add_to_town[i][0],
				0,299,"First part of flag must be from 0 to 299.","",&me)) return false;
		scenario.flag_to_add_to_town[i][1] = me["flag" + id + "-y"].getTextAsNum();
		if(cre(scenario.flag_to_add_to_town[i][1],
				0,9,"Second part of flag must be from 0 to 9.","",&me)) return false;
	}
	return true;
}

static void put_add_town_in_dlog(cDialog& me) {
	short i;
	
	for(i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		me["town" + id].setTextToNum(scenario.town_to_add_to[i]);
		me["flag" + id + "-x"].setTextToNum(scenario.flag_to_add_to_town[i][0]);
		me["flag" + id + "-y"].setTextToNum(scenario.flag_to_add_to_town[i][1]);
	}
}

static bool edit_add_town_event_filter(cDialog& me, std::string hit) {
	if(hit == "okay") {
		if(save_add_town(me))
			me.toast(true);
	}
	return true;
}

void edit_add_town() {
	using namespace std::placeholders;
	
	cDialog vary_dlg("edit-town-varying");
	vary_dlg.attachClickHandlers(std::bind(edit_add_town_event_filter, _1, _2), {"okay"});
	
	put_add_town_in_dlog(vary_dlg);
	
	vary_dlg.run();
}

static bool save_item_placement(cDialog& me, cScenario::cItemStorage& storage, short which) {
	short i;
	
	storage.property = dynamic_cast<cLed&>(me["owned"]).getState() != led_off;
	storage.ter_type = me["ter"].getTextAsNum();
	if(cre(storage.ter_type,
			-1,255,"Terrain Type must be from 0 to 255 (or -1 for No Shortcut).","",&me)) return false;
	for(i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		storage.item_num[i] = me["item" + id].getTextAsNum();
		if(cre(storage.item_num[i],
			   -1,399,"All item numbers must be from 0 to 399 (or -1 for No Item).","",&me)) return false;
		storage.item_odds[i] = me["odds" + id].getTextAsNum();
		if(cre(storage.item_odds[i],
			   0,100,"All item chances must bve from 0 to 100.","",&me)) return false;
	}
	scenario.storage_shortcuts[which] = storage;
	return true;
}

static void put_item_placement_in_dlog(cDialog& me, const cScenario::cItemStorage& storage, short which) {
	short i;
	
	me["num"].setTextToNum(which);
	dynamic_cast<cLed&>(me["owned"]).setState(storage.property ? led_red : led_off);
	me["ter"].setTextToNum(storage.ter_type);
	for(i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		me["item" + id].setTextToNum(storage.item_num[i]);
		me["odds" + id].setTextToNum(storage.item_odds[i]);
	}
}

static bool edit_item_placement_event_filter(cDialog& me, std::string hit, cScenario::cItemStorage& storage, short& which) {
	short i;
	
	if(hit == "okay") {
		if(save_item_placement(me, storage, which))
			me.toast(true);
	} else if(hit == "cancel") {
		me.toast(false);
	} else if(hit == "left") {
		if(!save_item_placement(me, storage, which)) return true;
		which--;
		if(which < 0) which = 9;
		storage = scenario.storage_shortcuts[which];
		put_item_placement_in_dlog(me, storage, which);
	} else if(hit == "right") {
		if(!save_item_placement(me, storage, which)) return true;
		which++;
		if(which > 9) which = 0;
		storage = scenario.storage_shortcuts[which];
		put_item_placement_in_dlog(me, storage, which);
	} else if(hit == "choose-ter") {
		i = me["ter"].getTextAsNum();
		storage.ter_type = i;
		i = choose_text(STRT_TER,i,&me,"Which Terrain?");
		if(i >= 0){
			me["ter"].setTextToNum(i);
			storage.ter_type = i;
		}
	}
	return true;
}

static bool edit_item_placement_select_item(cDialog& me, cScenario::cItemStorage& storage, short hit) {
	std::string id = "item" + std::to_string(hit);
	short i = me[id].getTextAsNum();
	storage.item_num[hit - 1] = i;
	i = choose_text(STRT_ITEM,i,&me,"Place which item?");
	if(i >= 0) {
		me[id].setTextToNum(i);
		storage.item_num[hit - 1] = i;
	}
	return true;
}

void edit_item_placement() {
	using namespace std::placeholders;
	cScenario::cItemStorage storage = scenario.storage_shortcuts[0];
	short cur_shortcut = 0;
	
	cDialog shortcut_dlg("edit-item-shortcut");
	shortcut_dlg.attachClickHandlers(std::bind(edit_item_placement_event_filter, _1, _2, std::ref(storage), std::ref(cur_shortcut)), {"okay", "cancel", "left", "right", "choose-ter"});
	for(int i = 0; i < 10; i++) {
		std::string id = "choose-item" + std::to_string(i + 1);
		shortcut_dlg[id].attachClickHandler(std::bind(edit_item_placement_select_item, _1, std::ref(storage), i + 1));
	}
	
	put_item_placement_in_dlog(shortcut_dlg, storage, cur_shortcut);
	
	shortcut_dlg.run();
}

static bool save_scen_details(cDialog& me, std::string, eKeyMod) {
	short i;
	if(!me.toast(true)) return true;
	
	{
		cLedGroup& difficulty = dynamic_cast<cLedGroup&>(me["difficulty"]);
		scenario.difficulty = difficulty.getSelected()[3] - '1';
	}{
		cLedGroup& rating = dynamic_cast<cLedGroup&>(me["rating"]);
		scenario.rating = rating.getSelected()[4] - '1';
	}
	scenario.adjust_diff = dynamic_cast<cLed&>(me["adjust"]).getState() != led_red;
	for(i = 0; i < 3; i++)
		scenario.format.ver[i] = me["ver" + std::to_string(i + 1)].getTextAsNum();
	scenario.who_wrote[0] = me["who1"].getText().substr(0, 60);
	scenario.who_wrote[1] = me["who2"].getText().substr(0, 60);
	scenario.contact_info = me["contact"].getText().substr(0, 256);
	scenario.campaign_id = me["cpnid"].getText();
	scenario.bg_out = boost::lexical_cast<int>(me["bg-out"].getText().substr(10));
	scenario.bg_town = boost::lexical_cast<int>(me["bg-town"].getText().substr(10));
	scenario.bg_dungeon = boost::lexical_cast<int>(me["bg-dungeon"].getText().substr(13));
	scenario.bg_fight = boost::lexical_cast<int>(me["bg-fight"].getText().substr(11));
	return true;
}

static void put_scen_details_in_dlog(cDialog& me) {
	dynamic_cast<cLedGroup&>(me["difficulty"]).setSelected("lvl" + std::to_string(scenario.difficulty + 1));
	dynamic_cast<cLedGroup&>(me["rating"]).setSelected("rate" + std::to_string(scenario.rating + 1));
	dynamic_cast<cLed&>(me["adjust"]).setState(scenario.adjust_diff ? led_red : led_off);
	for(int i = 0; i < 3; i++)
		me["ver" + std::to_string(i + 1)].setTextToNum(scenario.format.ver[i]);
	me["who1"].setText(scenario.who_wrote[0]);
	me["who2"].setText(scenario.who_wrote[1]);
	me["contact"].setText(scenario.contact_info);
	me["cpnid"].setText(scenario.campaign_id);
	me["bg-out"].setText("Outdoors: " + std::to_string(scenario.bg_out));
	me["bg-town"].setText("In towns: " + std::to_string(scenario.bg_town));
	me["bg-dungeon"].setText("In dungeons: " + std::to_string(scenario.bg_dungeon));
	me["bg-fight"].setText("In combat: " + std::to_string(scenario.bg_fight));
}

static bool edit_scen_default_bgs(cDialog& me, std::string which, eKeyMod) {
	int bg_i = 0, idx_i = 0;
	if(which == "bg-out" || which == "bg-town")
		idx_i = 10;
	else if(which == "bg-dungeon")
		idx_i = 13;
	else if(which == "bg-fight")
		idx_i = 11;
	else return true;
	bg_i = boost::lexical_cast<int>(me[which].getText().substr(idx_i));
	bg_i = choose_background(bg_i, &me);
	std::string lbl;
	if(which == "bg-out")
		lbl = "Outdoors";
	else if(which == "bg-town")
		lbl = "In towns";
	else if(which == "bg-dungeon")
		lbl = "In dungeons";
	else if(which == "bg-fight")
		lbl = "In combat";
	me[which].setText(lbl + ": " + std::to_string(bg_i));
	return true;
}

void edit_scen_details() {
	cDialog info_dlg("edit-scenario-details");
	info_dlg["okay"].attachClickHandler(save_scen_details);
	info_dlg.attachClickHandlers(edit_scen_default_bgs, {"bg-out", "bg-town", "bg-dungeon", "bg-fight"});
	
	put_scen_details_in_dlog(info_dlg);
	
	info_dlg.run();
}

static bool edit_make_scen_1_event_filter(cDialog& me, std::string, eKeyMod) {
	short i,j;
	
	std::string str = me["file"].getText();
	j = str.length();
	if(j == 0) {
		giveError("You've left the file name empty.","",&me);
		return true;
	}
	if(j > 50) {
		giveError("The file name can be at most 50 characters long.","",&me);
		return true;
	}
	for(i = 0; i < j; i++)
		if((str[i] < 97) || (str[i] > 122)) {
			giveError("The file name must consist of only lower case letters.","",&me);
			return true;
		}
	me.setResult(true);
	me.toast(true);
	return true;
}

bool edit_make_scen_1(std::string& filename,std::string& title,bool& grass) {
	cDialog new_dlog("make-scenario1");
	new_dlog["okay"].attachClickHandler(edit_make_scen_1_event_filter);
	new_dlog["cancel"].attachClickHandler(std::bind(&cDialog::toast, &new_dlog, false));
	new_dlog.setResult(false);
	
	new_dlog.run();
	if(!new_dlog.getResult<bool>()) return false;
	
	title = new_dlog["name"].getText();
	filename = new_dlog["file"].getText();
	grass = dynamic_cast<cLed&>(new_dlog["surface"]).getState() != led_off;
	return true;
}

static bool edit_make_scen_2_event_filter(cDialog& me, std::string, eKeyMod) {
	short i,j,k;
	
	i = me["out-w"].getTextAsNum();
	if(cre(i, 1,50,"Outdoors width must be between 1 and 50.","",&me)) return true;
	j = me["out-h"].getTextAsNum();
	if(cre(j, 1,50,"Outdoors height must be between 1 and 50.","",&me)) return true;
	if(cre(i * j, 1,100,"The total number of outdoor sections (width times height) must be between 1 and 100.","",&me)) return true;
	i = me["town_s"].getTextAsNum();
	j = me["town_m"].getTextAsNum();
	k = me["town_l"].getTextAsNum();
	if(cre(i, 0,200,"Number of small towns must be between 0 and 200.","",&me)) return true;
	if(cre(j, 1,200,"Number of medium towns must be between 1 and 200. The first town (Town 0) must always be of medium size.","",&me)) return true;
	if(cre(k, 0,200,"Number of large towns must be between 0 and 200.","",&me)) return true;
	if(cre(i + j + k, 1,200,"The total number of towns must be from 1 to 200 (you must have at least 1 town).","",&me)) return true;
	
	me.toast(true);
	return true;
}

bool edit_make_scen_2(short& out_w, short& out_h, short& town_l, short& town_m, short& town_s, bool& def_town) {
	cDialog new_dlog("make-scenario2");
	new_dlog["okay"].attachClickHandler(edit_make_scen_2_event_filter);
	new_dlog["cancel"].attachClickHandler(std::bind(&cDialog::toast, &new_dlog, false));
	new_dlog.setResult(false);
	
	new_dlog.run();
	if(!new_dlog.getResult<bool>()) return false;
	
	out_w = new_dlog["out-w"].getTextAsNum();
	out_h = new_dlog["out-h"].getTextAsNum();
	town_l = new_dlog["town-l"].getTextAsNum();
	town_m = new_dlog["town-m"].getTextAsNum();
	town_s = new_dlog["town-s"].getTextAsNum();
	def_town = dynamic_cast<cLed&>(new_dlog["warrior-grove"]).getState();
	return true;
}

extern fs::path progDir;
extern eScenMode overall_mode;
bool build_scenario() {
	short width, height, lg, med, sm, which_town;
	bool default_town, grass;
	std::string filename, title;
	short i,j;
	cTown* warriors_grove = nullptr;
	
	if(!edit_make_scen_1(filename, title, grass))
		return false;
	filename += ".boes";
	if(!edit_make_scen_2(width, height, lg, med, sm, default_town))
		return false;
	scenario = cScenario(true);
	scenario.scen_name = title;
	if(!default_town) {
		scenario.addTown<cMedTown>();
		if(!grass)
			for(i = 0; i < 48; i++)
				for(j = 0; j < 48; j++)
					town->terrain(i,j) = 0;
	} else {
		fs::path basePath = progDir/"Scenario Editor"/"Blades of Exile Base"/"bladbase.exs";
		if(!fs::exists(basePath)) {oopsError(40);}
		cScenario base;
		load_scenario(basePath, base);
		warriors_grove = base.towns[0];
		base.towns[0] = nullptr;
	}
	if(med > 0) med--;
	// TODO: This will probably change drastically once the new scenario format is implemented
	
	make_new_scenario(filename,width,height,default_town,grass);
	
	overall_mode = MODE_MAIN_SCREEN;
	
	cur_out.x = 0;
	cur_out.y = 0;
	current_terrain = scenario.outdoors[0][0];
	
	if(default_town && warriors_grove)
		scenario.towns.push_back(warriors_grove);
	
	// TODO: Append i+1 to each town name
	for(i = 0; i < lg; i++) {
		scenario.addTown<cBigTown>();
		town->town_name = "Large town";
	}
	for(i = 0; i < med; i++) {
		scenario.addTown<cMedTown>();
		town->town_name = "Medium town";
	}
	for(i = 0; i < sm; i++) {
		scenario.addTown<cTinyTown>();
		town->town_name = "Small town";
	}
	cur_town = 0;
	town = scenario.towns[0];
	update_item_menu();
	return false;
}

static bool check_location_bounds(cDialog& me, std::string id, bool losing) {
	if(!losing) return true;
	short town_num = me["town-num"].getTextAsNum();
	short dim = me[id].getTextAsNum();
	if(dim < 0 || dim >= scenario.towns[town_num]->max_dim()) {
		giveError("This coordinate is not inside the bounds of the town.");
		return false;
	}
	return true;
}

static bool set_starting_loc_filter(cDialog& me, std::string, eKeyMod) {
	if(me.toast(true)) {
		scenario.which_town_start = me["town-num"].getTextAsNum();
		scenario.where_start.x = me["town-x"].getTextAsNum();
		scenario.where_start.y = me["town-y"].getTextAsNum();
	}
	return true;
}

void set_starting_loc() {
	using namespace std::placeholders;
	
	cDialog loc_dlg("set-start-loc");
	loc_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &loc_dlg, false));
	loc_dlg["okay"].attachClickHandler(set_starting_loc_filter);
	loc_dlg["town-num"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, scenario.towns.size() - 1, "The starting town"));
	loc_dlg.attachFocusHandlers(check_location_bounds, {"town-x", "town-y"});
	
	loc_dlg["town-num"].setTextToNum(scenario.which_town_start);
	loc_dlg["town-x"].setTextToNum(scenario.where_start.x);
	loc_dlg["town-y"].setTextToNum(scenario.where_start.y);
	
	loc_dlg.run();
}

static bool save_scenario_events(cDialog& me, std::string, eKeyMod) {
	short i;
	
	if(!me.toast(true)) return true;
	
	for(i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		scenario.scenario_timer_times[i] = me["time" + id].getTextAsNum();
		scenario.scenario_timer_specs[i] = me["node" + id].getTextAsNum();
	}
	return true;
}

static bool check_scenario_timer_time(cDialog& me, std::string id, bool losing) {
	if(!losing) return true;
	int val = me[id].getTextAsNum();
	if(val > 0 && val % 10 != 0) {
		giveError("All scenario event times must be multiples of 10 (e.g. 100, 150, 1000, etc.).");
		return false;
	}
	return true;
}

static bool edit_scenario_events_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	// item_hit is of the form editN; we need an ID of the form nodeN
	item_hit.replace(0, 4, "node");
	short spec = me[item_hit].getTextAsNum();
	if(spec < 0 || spec > 255) {
		spec = get_fresh_spec(0);
		if(spec < 0) {
			giveError("You can't create a new scenario special encounter because there are no more free special nodes.",
					   "To free a special node, set its type to No Special and set its Jump To special to -1.");
			return true;
		}
	}
	if(edit_spec_enc(spec,0,&me))
		me[item_hit].setTextToNum(spec);
	return true;
}

void edit_scenario_events() {
	using namespace std::placeholders;
	
	cDialog evt_dlg("edit-scenario-events");
	evt_dlg["okay"].attachClickHandler(save_scenario_events);
	for(int i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		evt_dlg["time" + id].attachFocusHandler(check_scenario_timer_time);
		evt_dlg["node" + id].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 255, "The scenario special node", "-1 for no special"));
		evt_dlg["edit" + id].attachClickHandler(edit_scenario_events_event_filter);
		evt_dlg["time" + id].setTextToNum(scenario.scenario_timer_times[i]);
		evt_dlg["node" + id].setTextToNum(scenario.scenario_timer_specs[i]);
	}
	
	evt_dlg.run();
}

static void fill_custom_pics_types(cDialog& me, std::vector<ePicType>& pics, pic_num_t first) {
	pic_num_t last = first + 9;
	if(last >= pics.size()) pics.resize(last + 1, PIC_FULL);
	me["num0"].setTextToNum(first);
	for(pic_num_t i = first; i <= last; i++) {
		std::string id = std::to_string(i - first + 1);
		cLedGroup& grp = dynamic_cast<cLedGroup&>(me["type" + id]);
		cPict& pic = dynamic_cast<cPict&>(me["pic" + id]);
		pic.setPict(i, PIC_CUSTOM_TER);
		cControl& num = me["num" + id];
		num.setTextToNum(1000 + i);
		switch(pics[i]) {
			case PIC_TER:
				grp.setSelected("ter" + id);
				break;
			case PIC_TER_ANIM:
				grp.setSelected("anim" + id);
				num.setTextToNum(2000 + i);
				break;
			case PIC_TER_MAP:
				grp.setSelected("map" + id);
				break;
			case PIC_MONST:
				grp.setSelected("monst-sm" + id);
				break;
			case PIC_MONST_WIDE:
				grp.setSelected("monst-wide" + id);
				num.setTextToNum(2000 + i);
				break;
			case PIC_MONST_TALL:
				grp.setSelected("monst-tall" + id);
				num.setTextToNum(3000 + i);
				break;
			case PIC_MONST_LG:
				grp.setSelected("monst-lg" + id);
				num.setTextToNum(4000 + i);
				break;
			case PIC_DLOG:
				grp.setSelected("dlog" + id);
				break;
			case PIC_DLOG_LG:
				grp.setSelected("dlog-lg" + id);
				break;
			case PIC_TALK:
				grp.setSelected("talk" + id);
				pic.setPict(i, PIC_CUSTOM_TALK);
				break;
			case PIC_ITEM:
				grp.setSelected("item" + id);
				break;
			case PIC_BOOM:
				grp.setSelected("boom" + id);
				break;
			case PIC_MISSILE:
				grp.setSelected("miss" + id);
				break;
			case PIC_FULL:
				grp.setSelected("none" + id);
				break;
		}
	}
}

static bool set_custom_pic_type(cDialog& me, std::string hit, std::vector<ePicType>& pics, pic_num_t first) {
	hit = dynamic_cast<cLedGroup&>(me[hit]).getSelected();
	size_t iNum = hit.find_last_not_of("0123456789");
	std::string id = hit.substr(iNum + 1);
	hit = hit.substr(0, iNum + 1);
	pic_num_t pic = boost::lexical_cast<int>(id) + first - 1;
	cControl& num = me["num" + id];
	num.setTextToNum(1000 + pic);
	if(hit == "ter") {
		pics[pic] = PIC_TER;
	} else if(hit == "anim") {
		pics[pic] = PIC_TER_ANIM;
		num.setTextToNum(2000 + pic);
	} else if(hit == "map") {
		pics[pic] = PIC_TER_MAP;
	} else if(hit == "monst-sm") {
		pics[pic] = PIC_MONST;
	} else if(hit == "monst-wide") {
		pics[pic] = PIC_MONST_WIDE;
		num.setTextToNum(2000 + pic);
	} else if(hit == "monst-tall") {
		pics[pic] = PIC_MONST_TALL;
		num.setTextToNum(3000 + pic);
	} else if(hit == "monst-lg") {
		pics[pic] = PIC_MONST_LG;
		num.setTextToNum(4000 + pic);
	} else if(hit == "dlog") {
		pics[pic] = PIC_DLOG;
	} else if(hit == "dlog-lg") {
		pics[pic] = PIC_DLOG_LG;
	} else if(hit == "talk") {
		pics[pic] = PIC_TALK;
	} else if(hit == "item") {
		pics[pic] = PIC_ITEM;
	} else if(hit == "boom") {
		pics[pic] = PIC_BOOM;
	} else if(hit == "miss") {
		pics[pic] = PIC_MISSILE;
	} else if(hit == "none") {
		pics[pic] = PIC_FULL;
	}
	return true;
}

static bool save_pics_types(cDialog& me, const std::vector<ePicType>& pics) {
	if(!me.toast(true)) return true;
	scenario.custom_graphics = pics;
	return true;
}

static bool change_pics_page(cDialog& me, std::string hit, std::vector<ePicType>& pics, pic_num_t& first) {
	size_t num_pics = spec_scen_g.count();
	if(hit == "left") {
		if(first == 0) first = ((num_pics - 1) / 10) * 10;
		else first -= 10;
	} else if(hit == "right") {
		if(first + 10 >= num_pics) first = 0;
		else first += 10;
	} else return true;
	fill_custom_pics_types(me, pics, first);
	return true;
}

void edit_custom_pics_types() {
	if(spec_scen_g.count() == 0) {
		giveError("You don't have any custom graphics to classify!");
		return;
	}
	using namespace std::placeholders;
	std::vector<ePicType> pics = scenario.custom_graphics;
	pic_num_t first_pic = 0;
	
	cDialog pic_dlg("graphic-types");
	for(int i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		pic_dlg["type" + id].attachFocusHandler(std::bind(set_custom_pic_type, _1, _2, std::ref(pics), std::ref(first_pic)));
	}
	pic_dlg["okay"].attachClickHandler(std::bind(save_pics_types, _1, std::ref(pics)));
	pic_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, _1, false));
	pic_dlg.attachClickHandlers(std::bind(change_pics_page, _1, _2, std::ref(pics), std::ref(first_pic)), {"left", "right"});
	
	fill_custom_pics_types(pic_dlg, pics, first_pic);
	if(spec_scen_g.count() <= 10) {
		pic_dlg["left"].hide();
		pic_dlg["right"].hide();
	}
	pic_dlg.run();
}
