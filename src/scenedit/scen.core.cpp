
#include <cstdio>
#include <cstring>
#include <functional>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include "scen.global.hpp"
#include "scenario/scenario.hpp"
#include "scenario/town.hpp"
#include "gfx/gfxsheets.hpp"
#include "utility.hpp"
#include "scen.graphics.hpp"
#include "scen.core.hpp"
#include "scen.keydlgs.hpp"
#include "scen.sdfpicker.hpp"
#include "scen.locpicker.hpp"
#include "scen.townout.hpp"
#include "scen.fileio.hpp"
#include "scen.actions.hpp"
#include "scen.menus.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "dialogxml/widgets/control.hpp"
#include "dialogxml/widgets/button.hpp"
#include "dialogxml/widgets/tilemap.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/3choice.hpp"
#include "dialogxml/dialogs/strchoice.hpp"
#include "dialogxml/dialogs/pictchoice.hpp"
#include "fileio/fileio.hpp"
#include "dialogxml/widgets/field.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "fileio/resmgr/res_strings.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "fileio/resmgr/res_sound.hpp"
#include "tools/cursors.hpp"
#include "dialogxml/widgets/stack.hpp"
#include "spell.hpp"
#include "mathutil.hpp"
#include "tools/winutil.hpp"
#include "view_dialogs.hpp"

extern short cen_x, cen_y,cur_town;
extern bool mouse_button_held;
extern bool editing_town;
extern short cur_viewing_mode;
extern cTown* town;
extern cOutdoors* current_terrain;
extern short mode_count,to_create;
extern ter_num_t template_terrain[64][64];
extern cScenario scenario;
extern cCustomGraphics spec_scen_g;
extern location cur_out;

const std::set<eItemAbil> items_no_strength = {
	eItemAbil::NONE, eItemAbil::HEALING_WEAPON, eItemAbil::RETURNING_MISSILE, eItemAbil::SEEKING_MISSILE, eItemAbil::DRAIN_MISSILES,
	eItemAbil::LIGHTER_OBJECT, eItemAbil::HEAVIER_OBJECT, eItemAbil::LIFE_SAVING, eItemAbil::POISON_AUGMENT,
	eItemAbil::QUICKFIRE,
};

cArea* get_current_area() {
	if(editing_town) return town;
	else return current_terrain;
}

static bool save_ter_info(cDialog& me, cTerrain& ter) {
	eTerSpec prop = eTerSpec(boost::lexical_cast<short>(dynamic_cast<cLedGroup&>(me["prop"]).getSelected().substr(4)));
	int spec_type = me["flag2"].getTextAsNum();
	int num_town = (**std::min_element(scenario.towns.begin(), scenario.towns.end(), [](cTown* a,cTown* b){
		return a->specials.size() < b->specials.size();
	})).specials.size();
	// NOTE: It seems it was planned to add 'Use' to outdoor mode.
	// We're not planning to do that anymore, but if it ever is, this
	// additional constraint will be needed here:

	// int num_out = (**std::min_element(scenario.outdoors.begin(), scenario.outdoors.end(), [](cOutdoors* a,cOutdoors* b){
	// 	return a->specials.size() < b->specials.size();
	// })).specials.size();
	// int num_loc = std::min(num_town, num_out);

	int num_loc = num_town;
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
			if(!check_range(me, "flag1", true, -8, 8, "Strength")) return false;
			if(!check_range(me, "flag2", true, 0, 100, "Percentage chance")) return false;
			if(!check_range(me, "flag3", true, 0, 14, "Status type")) return false;
			break;
		case eTerSpec::CRUMBLING:
			if(!check_range_msg(me, "flag2", true, 0, 2, "Method", "1 - Move Mountains, 2 - quickfire, 0 - either")) return false;
		if(false) // Prevent next line from executing if it's crumbling
		case eTerSpec::UNLOCKABLE:
			if(!check_range(me, "flag2", true, 0, 10, "Difficulty")) return false;
			BOOST_FALLTHROUGH;
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
				showError("Special type must be either 0 or 1.", &me);
				return false;
			}
			// TODO If local, when creating a new town, we should now prompt the designer
			// to implement this special node.
			// NOTE: If 'Use' is ever added to outdoor mode, this message needs to change
			if(!check_range_msg(me, "flag1", true, 0, (spec_type == 0 ? num_glob : num_loc) - 1, "Special to call", "ALL towns must implement this node number"))
				return false;
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
	ter.block_horse = dynamic_cast<cLed&>(me["horse"]).getState() == led_red;
	ter.is_archetype = dynamic_cast<cLed&>(me["arch"]).getState() == led_red;
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
	std::ostringstream sout;

	if (n >= min_val && n <= max_val){
		return true;
	}else if(max_val < min_val){
		// It can be very confusing to be told a number should be from 0 to -1.
		// Explain what this really means, and hope that the extra message
		// provides context on fixing the problem.
		sout << fld_name << " currently has no range of acceptable values";
		if(xtra.empty()){
			xtra = "You can open an issue on GitHub asking for help and a better error message.";
		}
	}else{
		sout << fld_name << " must be from " << min_val << " to " << max_val;
	}
	if(!xtra.empty()) sout << ' ' << '(' << xtra << ')';
	showError(sout.str(), "", &me);
	return false;
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
			pic_num_t show_pic = pic;
			if(type == PIC_TER_ANIM && show_pic < 1000)
				show_pic += 960;
			cTextField& fld_ctrl = dynamic_cast<cTextField&>(parent[result_fld]);
			fld_ctrl.setTextToNum(show_pic);
		}
		if(pic_fld != ""){
			cPict& pic_ctrl = dynamic_cast<cPict&>(parent[pic_fld]);
			if(type == PIC_ITEM && pic_ctrl.getPicType() == PIC_TINY_ITEM)
				type = PIC_TINY_ITEM;
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
	StringList strings = *ResMgr::strings.get(from_file);
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

static bool pick_ter_flag(cDialog& me, std::string id, eKeyMod) {
	if(id == "editspec") {
		int which_type = me["flag2"].getTextAsNum();
		if(which_type == 1) {
			std::string choice = cChoiceDlog("pick-spec-type", {"town", "out", "cancel"}).show();
			if(choice == "cancel") return true;
			else if(choice == "town") which_type = 2;
		}
		short spec = me["flag1"].getTextAsNum();
		if(spec < 0)
			spec = get_fresh_spec(which_type);
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
		case eTerSpec::CALL_SPECIAL: case eTerSpec::CALL_SPECIAL_WHEN_USED:
			if((which == 1) == (prop == eTerSpec::NONE))
				i = choose_text_res("tiny-icons", 1, 137, i + 1, &me, "Which tiny icon?");
			else return true;
			break;
		case eTerSpec::CHANGE_WHEN_STEP_ON: case eTerSpec::CHANGE_WHEN_USED:
			if(which == 1)
				i = choose_text(STRT_TER, i, &me, "Change to what terrain?");
			else if(which == 2)
				i = choose_sound(i, &me, "Select the sound to play when it changes:");
			else return true;
			break;
		case eTerSpec::DAMAGING:
			if(which == 3)
				i = choose_damage_type(i, &me, false);
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
			me["pickflag1"].show();
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
			me["pickflag3"].show();
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
	dynamic_cast<cLed&>(me["flight"]).setState(ter_type.fly_over ? led_red : led_off);
	dynamic_cast<cLed&>(me["boat"]).setState(ter_type.boat_over ? led_red : led_off);
	dynamic_cast<cLed&>(me["horse"]).setState(ter_type.block_horse ? led_red : led_off);
	dynamic_cast<cLed&>(me["arch"]).setState(ter_type.is_archetype ? led_red : led_off);
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
	cDialog obj_dlg(*ResMgr::dialogs.get("edit-ter-obj"), &me);
	obj_dlg.attachFocusHandlers([&pic](cDialog& me, std::string fld, bool losing) -> bool {
		if(!losing) return true;
		int id = me["id"].getTextAsNum();
		if(fld != "id") {
			int i = me[fld].getTextAsNum();
			if(i > 4 || (i == 4 && (fld == "x" || fld == "y"))) {
				showError("Terrain objects cannot be larger than 4x4 tiles.", &me);
				return false;
			} else if(id > 0 && i == 0 && (fld == "w" || fld == "h")) {
				showError("Width and height cannot be zero.", &me);
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
		cTilemap& map = dynamic_cast<cTilemap&>(me["map"]);
		for(int x = 0; x < 4; x++) {
			for(int y = 0; y < 4; y++) {
				dynamic_cast<cPict&>(map.getChild(x,y)).setPict(obj[x][y]);
			}
		}
		return true;
	}, {"w", "h", "x", "y", "id"});
	obj_dlg["okay"].attachClickHandler([](cDialog& me, std::string, eKeyMod) -> bool {
		if(!me.toast(true)) return true;
		int x = me["x"].getTextAsNum(), y = me["y"].getTextAsNum();
		int w = me["w"].getTextAsNum(), h = me["h"].getTextAsNum();
		if(x >= w || y >= h) {
			showError("X and Y must be less than width and height, respectively.", &me);
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

static bool play_ter_step_sound(cDialog& me, std::string id, bool losing) {
	if(losing) return true;
	std::string clicked = dynamic_cast<cLedGroup&>(me[id]).getSelected();
	if(clicked == "step") {
		play_sound(49);
		sf::sleep(sf::milliseconds(200));
		play_sound(50);
	} else if(clicked == "squish") {
		play_sound(55);
	} else if(clicked == "crunch") {
		play_sound(47);
	} else if(clicked == "splash") {
		play_sound(17);
	}
	return true;
}

bool edit_ter_type(ter_num_t which) {
	using namespace std::placeholders;
	ter_num_t first = which;
	cDialog ter_dlg(*ResMgr::dialogs.get("edit-terrain"));
	// Attach handlers
	ter_dlg["pict"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,2999,"terrain graphic"));
	ter_dlg["pickpict"].attachClickHandler(std::bind(pick_picture,PIC_TER,_1,"pict","graphic"));
	ter_dlg["pickanim"].attachClickHandler(std::bind(pick_picture,PIC_TER_ANIM,_1,"pict","graphic"));
	ter_dlg["pickmap"].attachClickHandler(std::bind(pick_picture,PIC_TER_MAP,_1,"map","seemap"));
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
	ter_dlg["help"].attachClickHandler([](cDialog&, std::string, eKeyMod) -> bool {
		launchDocs("editor/Terrain.html");
		return true;
	});
	ter_dlg["sound"].attachFocusHandler(play_ter_step_sound);
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
	return ter_dlg.accepted() || which != first;
}

static void put_monst_pic(cPict& pict, pic_num_t num) {
	if(num < 1000)
		pict.setPict(num, PIC_MONST);
	else {
		ePicType type_g = PIC_CUSTOM_MONST;
		short size_g = num / 1000;
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
		pict.setPict(num,type_g);
	}
}

static void put_monst_info_in_dlog(cDialog& me, cMonster& monst, mon_num_t which) {
	std::ostringstream strb;

	put_monst_pic(dynamic_cast<cPict&>(me["icon"]), monst.picture_num);

	dynamic_cast<cPict&>(me["talkpic"]).setPict(monst.default_facial_pic, PIC_TALK);
	me["num"].setTextToNum(which);
	me["name"].setText(monst.m_name);
	me["pic"].setTextToNum(monst.picture_num);
	strb << "Width = " << int(monst.x_width);
	me["w"].setText(strb.str());
	clear_sstr(strb);
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
		case eAttitude::DOCILE:
			attitude.setSelected("docile");
			break;
		case eAttitude::HOSTILE_A:
			attitude.setSelected("A");
			break;
		case eAttitude::FRIENDLY:
			attitude.setSelected("friendly");
			break;
		case eAttitude::HOSTILE_B:
			attitude.setSelected("B");
			break;
	}
	
	me["type"].setText(get_str("traits",35 + int(monst.m_type) * 2));
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
		case 125: summoned_by.push_back(eSpell::SUMMON_SPIRIT); BOOST_FALLTHROUGH;
		case 126: summoned_by.push_back(eSpell::SUMMON_HOST); break;
		case 99: case 100: summoned_by.push_back(eSpell::STICKS_TO_SNAKES); break;
		case 122: summoned_by.push_back(eSpell::SUMMON_GUARDIAN); break;
	}
	
	clear_sstr(strb);
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
		clear_sstr(strb);
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
	if(def_att == "docile") monst.default_attitude = eAttitude::DOCILE;
	else if(def_att == "friendly") monst.default_attitude = eAttitude::FRIENDLY;
	else if(def_att == "A") monst.default_attitude = eAttitude::HOSTILE_A;
	else if(def_att == "B") monst.default_attitude = eAttitude::HOSTILE_B;
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
		if(which < 1) which = scenario.scen_monsters.size() - 1;
		monst = scenario.scen_monsters[which];
		put_monst_info_in_dlog(me,monst,which);
	} else if(hit == "right") {
		if(!save_monst_info(me,monst)) return false;
		scenario.scen_monsters[which] = monst;
		which++;
		if(which >= scenario.scen_monsters.size()) which = 1;
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
		i = choose_text_res("monster-abilities",130,139,int(monst.a[0].type)+130,&me,"Choose Attack 1 Type:");
		if(i >= 0) {
			monst.a[0].type = eMonstMelee(i);
			put_monst_info_in_dlog(me,monst,which);
		}
	} else if(hit == "picktype2") {
		if(!save_monst_info(me,monst)) return false;
		i = choose_text_res("monster-abilities",130,139,int(monst.a[1].type)+130,&me,"Choose Attack 2 Type:");
		if(i >= 0) {
			monst.a[1].type = eMonstMelee(i);
			put_monst_info_in_dlog(me,monst,which);
		}
	} else if(hit == "picktype3") {
		if(!save_monst_info(me,monst)) return false;
		i = choose_text_res("monster-abilities",130,139,int(monst.a[2].type)+130,&me,"Choose Attack 3 Type:");
		if(i >= 0) {
			monst.a[2].type = eMonstMelee(i);
			put_monst_info_in_dlog(me,monst,which);
		}
	} else if(hit == "preview") {
		// Use dark background that the game uses:
		short defaultBackground = cDialog::defaultBackground;
		cDialog::defaultBackground = cDialog::BG_DARK;
		cDialog monstInfo(*ResMgr::dialogs.get("monster-info"), &me);
		cDialog::defaultBackground = defaultBackground;
		monstInfo["left"].hide();
		monstInfo["right"].hide();
		monstInfo.attachClickHandlers([](cDialog&,std::string,eKeyMod){return false;}, {"guard","mindless","invuln"});
		monstInfo["done"].attachClickHandler(std::bind(&cDialog::toast, &monstInfo, true));
		put_monst_info(monstInfo, monst, scenario);
		monstInfo.run();
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

bool edit_monst_type(short which) {
	using namespace std::placeholders;
	mon_num_t first = which;
	cMonster monst = scenario.scen_monsters[which];
	
	cDialog monst_dlg(*ResMgr::dialogs.get("edit-monster"));
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
	monst_dlg.attachClickHandlers(std::bind(edit_monst_type_event_filter,_1,_2,std::ref(monst),std::ref(which)),{"okay","abils","picktype","picktype1","picktype2","picktype3","preview"});
	
	if(scenario.scen_monsters.size() == 1){
		monst_dlg["left"].hide();
		monst_dlg["right"].hide();
	} else {
		monst_dlg.attachClickHandlers(std::bind(edit_monst_type_event_filter,_1,_2,std::ref(monst),std::ref(which)),{"left","right"});
	}
	
	put_monst_info_in_dlog(monst_dlg, monst, which);
	
	monst_dlg.run();
	return monst_dlg.accepted() || first != which;
}

static void put_monst_abils_in_dlog(cDialog& me, cMonster& monst) {
	
	me["loot-item"].setTextToNum(monst.corpse_item);
	me["loot-chance"].setTextToNum(monst.corpse_item_chance);
	me["magic-res"].setTextToNum(monst.resist[eDamageType::MAGIC]);
	me["fire-res"].setTextToNum(monst.resist[eDamageType::FIRE]);
	me["cold-res"].setTextToNum(monst.resist[eDamageType::COLD]);
	me["poison-res"].setTextToNum(monst.resist[eDamageType::POISON]);
	me["onsee"].setTextToNum(monst.see_spec);
	me["snd"].setTextToNum(monst.ambient_sound);
	
	if(monst.amorphous)
		dynamic_cast<cLed&>(me["amorph"]).setState(led_red);
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
		std::string name = abil.second.to_string(abil.first);
		if(abil.first == eMonstAbil::SUMMON && abil.second.summon.type == eMonstSummon::TYPE)
			name.replace(name.find("%s"), 2, scenario.scen_monsters[abil.second.summon.what].m_name);
		me["abil-name" + id].setText(name);
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
	monst.resist[eDamageType::MAGIC] = me["magic-res"].getTextAsNum();
	monst.resist[eDamageType::FIRE] = me["fire-res"].getTextAsNum();
	monst.resist[eDamageType::COLD] = me["cold-res"].getTextAsNum();
	monst.resist[eDamageType::POISON] = me["poison-res"].getTextAsNum();
	monst.see_spec = me["onsee"].getTextAsNum();
	monst.ambient_sound = me["snd"].getTextAsNum();
	
	monst.amorphous = dynamic_cast<cLed&>(me["amorph"]).getState() != led_off;
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
		if(spec < 0)
			spec = get_fresh_spec(0);
		if(edit_spec_enc(spec,0,&me))
			me["onsee"].setTextToNum(spec);
	} else if(hit == "pick-snd") {
		int i = me["snd"].getTextAsNum();
		i = choose_sound(i, &me, "Select monster vocalization sound:");
		me["snd"].setTextToNum(i);
	}
	return true;
}

static short get_monst_abil_num(std::string prompt, int min, int max, cDialog& parent, int cur = -1) {
	cDialog numPanel(*ResMgr::dialogs.get("get-mabil-num"), &parent);
	numPanel["okay"].attachClickHandler(std::bind(&cDialog::toast, &numPanel, false));
	numPanel["prompt"].setText(prompt + " (" + std::to_string(min) + "-" + std::to_string(max) + ") ");
	numPanel["number"].setTextToNum(minmax(min,max,cur));
	numPanel.run();
	
	int result = numPanel["number"].getTextAsNum();
	if(result < min) return min;
	if(result > max) return max;
	return result;
}

static void fill_monst_abil_detail(cDialog& me, cMonster& monst, eMonstAbil abil, uAbility detail) {
	eMonstAbilCat cat = getMonstAbilCategory(abil);
	me["monst"].setText(monst.m_name);
	std::string name = detail.to_string(abil);
	if(abil == eMonstAbil::SUMMON && detail.summon.type == eMonstSummon::TYPE)
		name.replace(name.find("%s"), 2, scenario.scen_monsters[detail.summon.what].m_name);
	me["name"].setText(detail.to_string(abil));
	// These names start at line 80 in the strings file, but the first valid ability is ID 1, so add 79.
	me["type"].setText(get_str("monster-abilities", 79 + int(abil)));
	// Action points
	if(abil != eMonstAbil::RADIATE && abil != eMonstAbil::SUMMON) {
		int ap = detail.get_ap_cost(abil);
		if(ap == 0)
			me["ap"].setText("0 (passive ability)");
		else if(ap == -1)
			me["ap"].setText("0 (part of standard attack)");
		else me["ap"].setTextToNum(ap);
	}
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
			if(cat != eMonstAbilCat::MISSILE)
				me["missile-touch"].hide();
			me["range"].show();
			if(cat != eMonstAbilCat::MISSILE)
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
		if(cat == eMonstAbilCat::RADIATE)
			me["pat"].setTextToNum(int(detail.radiate.pat));
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
			case eMonstSummon::SPECIES: me["summon"].setText(get_str("traits", detail.summon.what * 2 + 35)); break;
		}
		if(abil == eMonstAbil::SUMMON && detail.summon.type == eMonstSummon::TYPE)
			me["name"].replaceText("%s", scenario.scen_monsters[detail.summon.what].m_name);
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
		detail.radiate.pat = eSpellPat(me["pat"].getTextAsNum());
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
				param = choose_text(STRT_MONST, 1, &me, "Summon which monster?") + 1;
				break;
			case eMonstAbilTemplate::SPECIAL:
			case eMonstAbilTemplate::HIT_TRIGGERS:
			case eMonstAbilTemplate::DEATH_TRIGGERS:
				param = get_fresh_spec(0);
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
			showError("Failed to add the new ability because the ability was not implemented. When reporting this, mention which ability you tried to add.", &me);
			return true;
		}
		// An ability of the same basic type exists and must be overwritten
		if(save_abils.find(iter->first) != save_abils.end() && save_abils[iter->first].active) {
			// Warn first
			cChoiceDlog confirm("edit-mabil-overwrite", {"okay", "cancel"}, &me);
			std::string name = save_abils[iter->first].to_string(iter->first);
			confirm->getControl("warning").replaceText("{{abil}}", name);
			bool overwrite = confirm.show() == "okay";
			if(!overwrite) {
				monst.abil = save_abils;
				return true;
			}
		}
		if(tmpl < eMonstAbilTemplate::CUSTOM_MISSILE && tmpl != eMonstAbilTemplate::SPECIAL) {
			put_monst_abils_in_dlog(me, monst);
			size_t iShow = std::distance(monst.abil.begin(), iter);
			dynamic_cast<cStack&>(me["abils"]).setPage(iShow / 4);
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
	cDialog abil_dlg(*ResMgr::dialogs.get("edit-mabil-" + which_dlg), &me);
	abil_dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, _1, true));
	abil_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, _1, false));
	abil_dlg["delete"].attachClickHandler([&monst,&iter](cDialog& me,std::string,eKeyMod){
		// TODO: Show confirmation first?
		return me.toast(false), iter = monst.abil.erase(iter), true;
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
		int first = 0, last = 0;
		if(cat == eMonstAbilCat::MISSILE) first = 110, last = 119;
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
				// Although SMASH/DISPEL is supported, the AI doesn't understand how to use it.
				// Thus I won't expose it in the UI.
				i = choose_field_type(i, &me, false);
				abil_params.gen.fld = eFieldType(i);
			} else if(abil == eMonstAbil::DAMAGE || abil == eMonstAbil::DAMAGE2) {
				i = int(abil_params.gen.dmg);
				i = choose_damage_type(i, &me, false);
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
				i = choose_pattern(i, &me, false);
				abil_params.gen.strength = i;
				fill_monst_abil_detail(me, monst, abil, abil_params);
				return true;
			});
		else abil_dlg["pick-strength"].hide();
	} else if(cat == eMonstAbilCat::RADIATE) {
		abil_dlg["pick-field"].attachClickHandler([&](cDialog& me,std::string,eKeyMod) -> bool {
			save_monst_abil_detail(me, abil, abil_params);
			int i = abil_params.radiate.type;
			i = choose_field_type(i, &me, true);
			abil_params.radiate.type = eFieldType(i);
			fill_monst_abil_detail(me, monst, abil, abil_params);
			return true;
		});
		abil_dlg["pick-pat"].attachClickHandler([&](cDialog& me,std::string,eKeyMod) -> bool {
			save_monst_abil_detail(me, abil, abil_params);
			int i = abil_params.radiate.pat;
			i = choose_pattern(i, &me, false);
			abil_params.radiate.pat = eSpellPat(i);
			fill_monst_abil_detail(me, monst, abil, abil_params);
			return true;
		});
	} else if(cat == eMonstAbilCat::SUMMON) {
		abil_dlg["pick-summon"].attachClickHandler([&](cDialog& me,std::string,eKeyMod) -> bool {
			save_monst_abil_detail(me, abil, abil_params);
			int i = abil_params.summon.what;
			eStrType type;
			switch(abil_params.summon.type) {
				case eMonstSummon::TYPE: type = STRT_MONST; i--; break;
				case eMonstSummon::LEVEL: type = STRT_SUMMON; break;
				case eMonstSummon::SPECIES: type = STRT_RACE; break;
			}
			i = choose_text(type, i, &me, "Summon what?");
			if(type == STRT_MONST) i++;
			abil_params.summon.what = i;
			fill_monst_abil_detail(me, monst, abil, abil_params);
			return true;
		});
	} else if(cat == eMonstAbilCat::SPECIAL) {
		if(abil == eMonstAbil::SPECIAL || abil == eMonstAbil::HIT_TRIGGER || abil == eMonstAbil::DEATH_TRIGGER)
			abil_dlg["pick-extra1"].attachClickHandler([&](cDialog& me,std::string,eKeyMod) -> bool {
				short spec = me["extra1"].getTextAsNum();
				if(spec < 0)
					spec = get_fresh_spec(0);
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
	if(monst.abil.size() > 0) {
		cStack& stk = dynamic_cast<cStack&>(me["abils"]);
		size_t iShow = iter == monst.abil.end() ? stk.getPageCount() - 1 : std::distance(monst.abil.begin(), iter) / 4;
		stk.setPage(iShow);
	}
	return true;
}

cMonster edit_monst_abil(cMonster initial,short which,cDialog& parent) {
	using namespace std::placeholders;
	
	cDialog monst_dlg(*ResMgr::dialogs.get("edit-monster-abils"),&parent);

	put_monst_pic(dynamic_cast<cPict&>(monst_dlg["icon"]), initial.picture_num);
	monst_dlg["loot-item"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 399, "Item To Drop", "-1 for no item"));

	monst_dlg["pick-item"].attachClickHandler([](cDialog& me, std::string, eKeyMod) -> bool {
		std::vector<std::string> item_names;
		for(cItem& item : scenario.scen_items){
			item_names.push_back(item.full_name);
		}
		short current = me["loot-item"].getTextAsNum();
		short i = cStringChoice(item_names, "Which item?", &me).show(current);
		me["loot-item"].setTextToNum(i);
		return true;
	});
	monst_dlg["loot-chance"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 100, "Dropping Chance", "-1 for no item"));
	monst_dlg.attachClickHandlers(std::bind(edit_monst_abil_detail, _1, _2, std::ref(initial)), {"abil-edit1", "abil-edit2", "abil-edit3", "abil-edit4"});
	monst_dlg.attachClickHandlers(std::bind(edit_monst_abil_event_filter, _1, _2, std::ref(initial)), {"okay", "cancel", "abil-up", "abil-down", "edit-see", "pick-snd"});
	
	monst_dlg["info"].replaceText("{{num}}", std::to_string(which));
	monst_dlg["info"].replaceText("{{name}}", initial.m_name);
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
		case eItemType::QUEST:
			variety.setSelected("quest");
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
	
	std::string abil = item.getAbilName();
	if(item.ability == eItemAbil::SUMMONING || item.ability == eItemAbil::MASS_SUMMONING)
		abil.replace(abil.find("%s"), 2, scenario.scen_monsters[item.abil_data.value].m_name);
	me["abilname"].setText(abil);
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
	else if(variety == "quest") item.variety = eItemType::QUEST;
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
		showError("Due to either the selected special ability or the presence of a type flag, this item's charges have been set to 1.", &me);
}

static bool edit_item_type_event_filter(cDialog& me, std::string hit, cItem& item, short& which) {
	short i;
	cItem temp_item;
	std::string variety = dynamic_cast<cLedGroup&>(me["variety"]).getSelected();
	bool valid = true;
	if(variety.substr(0,6) == "unused") valid = false;
	if(!valid && hit != "cancel" && hit.substr(0,6) != "choose") {
		showError("The Unused item varieties are reserved for later expansions, and can't be used now.","",&me);
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
		if(which < 0) which = scenario.scen_items.size() - 1;
		item = scenario.scen_items[which];
		put_item_info_in_dlog(me, item, which);
	} else if(hit == "next") {
		save_item_info(me, item);
		scenario.scen_items[which] = item;
		which++;
		if(which >= scenario.scen_items.size()) which = 0;
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
		cDialog desc_dlg(*ResMgr::dialogs.get("edit-text"), &me);
		desc_dlg["left"].hide();
		desc_dlg["right"].hide();
		desc_dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, &desc_dlg, true));
		desc_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &desc_dlg, false));
		desc_dlg["text"].setText(item.desc);
		desc_dlg["num-lbl"].setText(item.full_name);
		desc_dlg.run();
		if(desc_dlg.accepted())
			item.desc = desc_dlg["text"].getText();
	} else if(hit == "abils") {
		save_item_info(me, item);
		if(item.variety == eItemType::NO_ITEM) {
			showError("You must give the item a type (weapon, armor, etc.) before you can choose its abilities.","",&me);
			return true;
		}
		if(item.variety == eItemType::GOLD || item.variety == eItemType::FOOD || item.variety == eItemType::SPECIAL || item.variety == eItemType::QUEST) {
			showError("Gold, Food, Quests, and Special Items cannot be given special abilities.","",&me);
			return true;
		}
		temp_item = edit_item_abil(item,which,me);
		if(temp_item.variety != eItemType::NO_ITEM)
			item = temp_item;
		put_item_info_in_dlog(me, item, which);
	} else if(hit == "preview") {
		cItem temp_item = item;
		temp_item.ident = true;

		// Use dark background that the game uses:
		short defaultBackground = cDialog::defaultBackground;
		cDialog::defaultBackground = cDialog::BG_DARK;
		cDialog itemInfo(*ResMgr::dialogs.get("item-info"), &me);
		cDialog::defaultBackground = defaultBackground;

		itemInfo["left"].hide();
		itemInfo["right"].hide();
		itemInfo["done"].attachClickHandler(std::bind(&cDialog::toast, &itemInfo, false));
		itemInfo["magic"].attachClickHandler([](cDialog&, std::string, eKeyMod){return false;});
		itemInfo["id"].attachClickHandler([&temp_item](cDialog& me, std::string, eKeyMod){
			temp_item.ident = !temp_item.ident;
			put_item_info(me, temp_item, scenario);
			return true;
		});
		put_item_info(itemInfo, temp_item, scenario);
		itemInfo.run();
	} else if(hit == "edit-ic") {
		int value = me["class"].getTextAsNum();
		value = choose_text_editable(scenario.ic_names, value, &me, "Select item class:");
		me["class"].setTextToNum(value);
	} else if(hit == "edit-flag") {
		int value = me["flag"].getTextAsNum();
		value = choose_text_editable(scenario.itf_names, value, &me, "Select item type flag:");
		me["flag"].setTextToNum(value);
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

bool edit_item_type(short which) {
	using namespace std::placeholders;
	if(which == scenario.scen_items.size())
		scenario.scen_items.resize(which + 1);
	cItem item = scenario.scen_items[which];
	
	cDialog item_dlg(*ResMgr::dialogs.get("edit-item"));
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
	item_dlg.attachClickHandlers(std::bind(edit_item_type_event_filter, _1, _2, std::ref(item), std::ref(which)), {"okay", "cancel", "abils", "choosepic", "choosetp", "choosemiss", "desc", "preview", "edit-ic", "edit-flag"});
	
	if(scenario.scen_items.size() == 1) {
		item_dlg["prev"].hide();
		item_dlg["next"].hide();
	} else {
		item_dlg.attachClickHandlers(std::bind(edit_item_type_event_filter, _1, _2, std::ref(item), std::ref(which)), {"prev", "next"});
	}
	
	put_item_info_in_dlog(item_dlg, item, which);
	
	item_dlg.run();
	return item_dlg.accepted();
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
	me["str1"].setTextToNum(item.abil_strength);
	me["str2"].setTextToNum(item.abil_data.value);
	
	if(item.ability == eItemAbil::CALL_SPECIAL || item.ability == eItemAbil::WEAPON_CALL_SPECIAL || item.ability == eItemAbil::HIT_CALL_SPECIAL || item.ability == eItemAbil::DROP_CALL_SPECIAL) {
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
	
	if(item.charges <= 0 || item.type_flag != 0) {
		// Only unstackable items with charges can be rechargeable.
		me["recharge"].hide();
	}
	
	dynamic_cast<cLed&>(me["always-id"]).setState(item.ident ? led_red : led_off);
	dynamic_cast<cLed&>(me["magic"]).setState(item.magic ? led_red : led_off);
	dynamic_cast<cLed&>(me["cursed"]).setState(item.cursed ? led_red : led_off);
	dynamic_cast<cLed&>(me["conceal"]).setState(item.concealed ? led_red : led_off);
	dynamic_cast<cLed&>(me["no-sell"]).setState(item.unsellable ? led_red : led_off);
	dynamic_cast<cLed&>(me["recharge"]).setState(item.rechargeable ? led_red : led_off);
}

static void save_item_abils(cDialog& me, cItem& item) {
	item.magic_use_type = boost::lexical_cast<eItemUse>(dynamic_cast<cLedGroup&>(me["use-type"]).getSelected().substr(3));
	item.treas_class = boost::lexical_cast<short>(dynamic_cast<cLedGroup&>(me["treasure"]).getSelected().substr(4));
	item.abil_strength = me["str1"].getTextAsNum();
	item.abil_data.value = me["str2"].getTextAsNum();
	
	item.property = item.enchanted = item.contained = false;
	item.ident = dynamic_cast<cLed&>(me["always-id"]).getState() != led_off;
	item.magic = dynamic_cast<cLed&>(me["magic"]).getState() != led_off;
	item.cursed = dynamic_cast<cLed&>(me["cursed"]).getState() != led_off;
	item.unsellable = dynamic_cast<cLed&>(me["no-sell"]).getState() != led_off;
	item.concealed = dynamic_cast<cLed&>(me["conceal"]).getState() != led_off;
	item.rechargeable = dynamic_cast<cLed&>(me["recharge"]).getState() != led_off;
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
		if(spec < 0)
			spec = get_fresh_spec(0);
		if(edit_spec_enc(spec,0,&me)) {
			item.abil_strength = spec;
			me["str1"].setTextToNum(spec);
		}
	} else if(hit == "str2-choose1") {
		save_item_abils(me, item);
		i = item.abil_data.value;
		switch(item.ability) {
			case eItemAbil::DAMAGING_WEAPON:
			case eItemAbil::EXPLODING_WEAPON:
			case eItemAbil::DAMAGE_PROTECTION:
				i = choose_damage_type(i, &me, false);
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
				i = choose_text_res("magic-names", 1, 79, i + 1, &me, "Which mage spell?");
				if(i < 0) return true;
				break;
			case eItemAbil::SUMMONING:
			case eItemAbil::MASS_SUMMONING:
				i = choose_text(STRT_MONST, i - 1, &me, "Summon which monster type?") + 1;
				break;
			default: return true;
		}
		item.abil_data.value = i;
		me["str2"].setTextToNum(i);
	} else if(hit == "str2-choose2") {
		save_item_abils(me, item);
		i = 100 + choose_text_res("magic-names", 101, 168, item.abil_data.value + 1, &me, "Which priest spell?");
		if(i < 100) return true;
		item.abil_data.value = i;
		me["str2"].setTextToNum(i);
	} else if(hit == "clear") {
		save_item_abils(me, item);
		item.ability = eItemAbil::NONE;
		put_item_abils_in_dlog(me, item, which);
	} else if(hit == "weapon") {
		save_item_abils(me, item);
		if(!(*item.variety).is_weapon) {
			showError("You can only give an ability of this sort to a weapon.","",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 1, 18, int(item.ability), &me, "Choose Weapon Ability (inherent)");
		if(i < 0) return true;
		eItemAbil abil = eItemAbil(i + 1);
		if(abil >= eItemAbil::RETURNING_MISSILE && abil <= eItemAbil::SEEKING_MISSILE) {
			if(item.variety != eItemType::THROWN_MISSILE && item.variety != eItemType::ARROW &&
				item.variety != eItemType::BOLTS && item.variety != eItemType::MISSILE_NO_AMMO) {
				showError("You can only give this ability to a missile.",&me);
				return true;
			}
		}
		item.ability = abil;
		put_item_abils_in_dlog(me, item, which);
	} else if(hit == "general") {
		save_item_abils(me, item);
		if((*item.variety).equip_count == 0 || item.variety == eItemType::ARROW || item.variety == eItemType::THROWN_MISSILE || item.variety == eItemType::BOLTS){
			showError("You can only give an ability of this sort to an non-missile item which can be equipped (like armor, or a ring).",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 30, 60, int(item.ability), &me, "Choose General Ability (inherent)");
		if(i < 0) return true;
		item.ability = eItemAbil(i + 30);
		put_item_abils_in_dlog(me, item, which);
	} else if(hit == "usable") {
		save_item_abils(me, item);
		if((item.variety == eItemType::ARROW) || (item.variety == eItemType::THROWN_MISSILE) || (item.variety == eItemType::BOLTS)){
			showError("You can't give an ability of this sort to a missile.",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 70, 85, int(item.ability), &me, "Choose Usable Ability");
		if(i < 0) return true;
		item.ability = eItemAbil(i + 70);
		put_item_abils_in_dlog(me, item, which);
	} else if(hit == "reagent") {
		save_item_abils(me, item);
		if(item.variety != eItemType::NON_USE_OBJECT){
			showError("You can only give an ability of this sort to an item of type Non-Use Object.",&me);
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
	
	cDialog item_dlg(*ResMgr::dialogs.get("edit-item-abils"),&parent);
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
	item.name = me["name"].getText();
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
		if(which < 0) which = scenario.special_items.size() - 1;
		item = scenario.special_items[which];
		put_spec_item_in_dlog(me, item, which);
	} else if(hit == "right") {
		if(!save_spec_item(me, item, which)) return true;
		which++;
		if(which >= scenario.special_items.size()) which = 0;
		item = scenario.special_items[which];
		put_spec_item_in_dlog(me, item, which);
	} else if(hit == "edit-spec") {
		if(!save_spec_item(me, item, which)) return true;
		short spec = me["spec"].getTextAsNum();
		if(spec < 0)
			spec = get_fresh_spec(0);
		if(edit_spec_enc(spec,0,&me))
			me["spec"].setTextToNum(spec);
		save_spec_item(me, item, which);
		
	}
	return true;
}

bool edit_spec_item(short which_item) {
	short first = which_item;
	using namespace std::placeholders;
	cSpecItem item = scenario.special_items[which_item];
	
	cDialog item_dlg(*ResMgr::dialogs.get("edit-special-item"));
	item_dlg["spec"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, scenario.scen_specials.size(), "Scenario special node called", "-1 for no special"));
	item_dlg.attachClickHandlers(std::bind(edit_spec_item_event_filter, _1, _2, std::ref(item), std::ref(which_item)), {"okay", "cancel", "clear", "edit-spec"});
	
	if(scenario.special_items.size() == 1) {
		item_dlg["left"].hide();
		item_dlg["right"].hide();
	} else {
		item_dlg.attachClickHandlers(std::bind(edit_spec_item_event_filter, _1, _2, std::ref(item), std::ref(which_item)), {"left", "right"});
	}
	
	put_spec_item_in_dlog(item_dlg, item, which_item);
	item_dlg["clear"].hide();
	
	item_dlg.run();
	
	return item_dlg.accepted() || first != which_item;
}

static void put_quest_in_dlog(cDialog& me, const cQuest& quest, size_t which_quest) {
	me["num"].setText(std::to_string(which_quest) + " of " + std::to_string(scenario.quests.size()));
	me["name"].setText(quest.name);
	me["descr"].setText(quest.descr);
	me["chop"].setTextToNum(quest.deadline);
	me["evt"].setTextToNum(quest.event);
	me["xp"].setTextToNum(quest.xp);
	me["gold"].setTextToNum(quest.gold);
	me["bank1"].setTextToNum(quest.bank1);
	me["bank2"].setTextToNum(quest.bank2);
	
	dynamic_cast<cLed&>(me["rel"]).setState(quest.deadline_is_relative ? led_red : led_off);
	dynamic_cast<cLed&>(me["start"]).setState(quest.auto_start ? led_red : led_off);
	dynamic_cast<cLed&>(me["inbank"]).setState(quest.bank1 >= 0 || quest.bank2 >= 0 ? led_red : led_off);
	if(quest.bank1 < 0 && quest.bank2 < 0) {
		me["bank1"].hide();
		me["bank2"].hide();
		me["choose-bank1"].hide();
		me["choose-bank2"].hide();
	} else {
		me["bank1"].show();
		me["bank2"].show();
		me["choose-bank1"].show();
		me["choose-bank2"].show();
	}
}

static bool save_quest_from_dlog(cDialog& me, cQuest& quest, size_t which_quest, bool close) {
	if(!me.toast(true)) return false;
	
	quest.name = me["name"].getText();
	quest.descr = me["descr"].getText();
	quest.deadline = me["chop"].getTextAsNum();
	quest.event = me["evt"].getTextAsNum();
	quest.xp = me["xp"].getTextAsNum();
	quest.gold = me["gold"].getTextAsNum();
	
	quest.deadline_is_relative = dynamic_cast<cLed&>(me["rel"]).getState() == led_red;
	quest.auto_start = dynamic_cast<cLed&>(me["start"]).getState() == led_red;
	if(dynamic_cast<cLed&>(me["inbank"]).getState() == led_red) {
		quest.bank1 = me["bank1"].getTextAsNum();
		quest.bank2 = me["bank2"].getTextAsNum();
	} else quest.bank1 = quest.bank2 = -1;
	
	scenario.quests[which_quest] = quest;
	if(!close) me.untoast();
	return true;
}

static bool change_quest_dlog_page(cDialog& me, std::string dir, cQuest& quest, size_t& which_quest) {
	if(!save_quest_from_dlog(me, quest, which_quest, false))
		return true;
	
	if(dir == "left") {
		if(which_quest == 0)
			which_quest = scenario.quests.size();
		which_quest--;
	} else if(dir == "right") {
		which_quest++;
		if(which_quest == scenario.quests.size())
			which_quest = 0;
	}
	
	quest = scenario.quests[which_quest];
	put_quest_in_dlog(me, quest, which_quest);
	return true;
}

bool edit_quest(size_t which_quest) {
	using namespace std::placeholders;
	if(which_quest == scenario.quests.size()) {
		scenario.quests.resize(which_quest + 1);
		scenario.quests[which_quest].name = "New Quest";
	}
	cQuest quest = scenario.quests[which_quest];
	
	cDialog quest_dlg(*ResMgr::dialogs.get("edit-quest"));
	quest_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, _1, false));
	quest_dlg["okay"].attachClickHandler(std::bind(save_quest_from_dlog, _1, std::ref(quest), std::ref(which_quest), true));
	quest_dlg.attachClickHandlers([](cDialog& me, std::string item_hit, eKeyMod) {
		std::string field_id = item_hit.substr(7);
		std::string title = field_id == "evt" ? "Select an event:" : "Select a job board:";
		auto strings = field_id == "evt" ? scenario.evt_names : scenario.qb_names;
		int value = me[field_id].getTextAsNum();
		value = choose_text_editable(strings, value, &me, title);
		me[field_id].setTextToNum(value);
		return true;
	}, {"choose-evt", "choose-bank1", "choose-bank2"});
	quest_dlg["inbank"].attachFocusHandler([](cDialog& me, std::string, bool losing) -> bool {
		if(losing) {
			me["bank1"].hide();
			me["bank2"].hide();
			me["choose-bank1"].hide();
			me["choose-bank2"].hide();
		} else {
			me["bank1"].show();
			me["bank2"].show();
			me["choose-bank1"].show();
			me["choose-bank2"].show();
		}
		return true;
	});
	// TODO: Some focus handlers
	// Should quests be able to award negative XP or negative gold? I typed the text fields as 'uint' for now.
	
	if(scenario.quests.size() == 1) {
		quest_dlg["left"].hide();
		quest_dlg["right"].hide();
	} else {
		quest_dlg.attachClickHandlers(std::bind(change_quest_dlog_page, _1, _2, std::ref(quest), std::ref(which_quest)), {"left", "right"});
	}
	
	put_quest_in_dlog(quest_dlg, quest, which_quest);
	quest_dlg.run();
	return quest_dlg.accepted();
}

static bool put_shop_item_in_dlog(cPict& pic, cControl& num, cControl& title, const cShop& shop, int which) {
	cShopItem entry = shop.getItem(which);
	num.setTextToNum(which);
	pic.setPict(entry.item.graphic_num);
	if(entry.type == eShopItemType::EMPTY) {
		title.setText("");
		return false;
	}
	std::string name = entry.item.full_name;
	int amount = entry.quantity;
	if(entry.type == eShopItemType::OPT_ITEM) {
		name += " [" + std::to_string(amount / 1000) + "% chance]";
		amount %= 1000;
	}
	if(amount > 0) {
		name = std::to_string(amount) + "x " + name;
	}
	title.setText(name);
	return true;
}

static void put_shop_in_dlog(cDialog& me, const cShop& shop, size_t which_shop) {
	me["num"].setText(std::to_string(which_shop) + " of " + std::to_string(scenario.shops.size()));
	me["name"].setText(shop.getName());
	
	dynamic_cast<cPict&>(me["face"]).setPict(shop.getFace());
	
	dynamic_cast<cLedGroup&>(me["type"]).setSelected("t" + std::to_string(int(shop.getType()) + 1));
	dynamic_cast<cLedGroup&>(me["prompt"]).setSelected("p" + std::to_string(int(shop.getPrompt()) + 1));
	
	cStack& items = dynamic_cast<cStack&>(me["items"]);
	items.setPageCount(std::max<int>(1, ceil(shop.size() / 5.0)));
	for(int i = 0; i < items.getPageCount(); i++) {
		items.setPage(i);
		for(int j = 0; j < 5; j++) {
			std::string id = std::to_string(j + 1);
			if(put_shop_item_in_dlog(dynamic_cast<cPict&>(me["pict" + id]), me["n" + id], me["item" + id], shop, i * 5 + j)) {
				if(i == 0) {
					me["ed" + id].show();
					me["del" + id].show();
				}
			} else if(i == 0) {
				me["ed" + id].hide();
				me["del" + id].hide();
			}
			
		}
	}
	items.setPage(0);

	if(items.getPageCount() <= 1) {
		dynamic_cast<cButton&>(me["up"]).hide();
		dynamic_cast<cButton&>(me["down"]).hide();
	} else {
		dynamic_cast<cButton&>(me["up"]).show();
		dynamic_cast<cButton&>(me["down"]).show();
	}
}

static bool save_shop_from_dlog(cDialog& me, cShop& shop, size_t which_shop, bool close) {
	if(!me.toast(true)) return false;
	
	shop.setName(me["name"].getText());
	shop.setType(eShopType(dynamic_cast<cLedGroup&>(me["type"]).getSelected()[1] - '1'));
	shop.setPrompt(eShopPrompt(dynamic_cast<cLedGroup&>(me["prompt"]).getSelected()[1] - '1'));
	shop.setFace(dynamic_cast<cPict&>(me["face"]).getPicNum());
	// Items are filled in as they're added by the dialog, so that's all we need to do here
	
	scenario.shops[which_shop] = shop;
	if(!close) me.untoast();
	return true;
}

static bool change_shop_dlog_page(cDialog& me, std::string dir, cShop& shop, size_t& which_shop) {
	if(!save_shop_from_dlog(me, shop, which_shop, false))
		return true;
	
	if(dir == "left") {
		if(which_shop == 0)
			which_shop = scenario.shops.size();
		which_shop--;
	} else if(dir == "right") {
		which_shop++;
		if(which_shop == scenario.shops.size())
			which_shop = 0;
	}
	
	shop = scenario.shops[which_shop];
	put_shop_in_dlog(me, shop, which_shop);
	return true;
}

static bool change_shop_dlog_items_page(cDialog& me, std::string dir, const cShop& shop) {
	cStack& items = dynamic_cast<cStack&>(me["items"]);
	int curPage = items.getPage(), maxPage = items.getPageCount();
	if(dir == "up") {
		if(curPage == 0)
			curPage = maxPage - 1;
		else curPage--;
	} else if(dir == "down") {
		if(curPage == maxPage - 1)
			curPage = 0;
		else curPage++;
	}
	for(int i = 0; i < 5; i++) {
		std::string id = std::to_string(i + 1);
		if(shop.getItem(curPage * 5 + i).type == eShopItemType::EMPTY) {
			me["ed" + id].hide();
			me["del" + id].hide();
		} else {
			me["ed" + id].show();
			me["del" + id].show();
		}
	}
	items.setPage(curPage);
	return true;
}

static void edit_shop_item(cDialog& parent, size_t& item, size_t& quantity, bool optional) {
	using namespace std::placeholders;
	int was_item = item;
	cDialog item_dlg(*ResMgr::dialogs.get("edit-shop-item"), &parent);
	item_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, _1, false));
	item_dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, _1, true));
	
	if(optional) {
		item_dlg["amount"].setTextToNum(quantity % 1000);
		item_dlg["chance"].setTextToNum(quantity / 1000);
		item_dlg["amount"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 999, "amount"));
		item_dlg["chance"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 1, 100, "chance"));
	} else {
		item_dlg["amount"].setTextToNum(quantity);
		item_dlg["chance"].hide();
		item_dlg["chance-prompt"].hide();
	}
	
	item_dlg["item"].setText(scenario.scen_items[item].full_name);
	item_dlg["choose"].attachClickHandler([&item](cDialog& me, std::string, eKeyMod) -> bool {
		item = choose_text(STRT_ITEM, item, &me, "Which item?");
		me["item"].setText(scenario.scen_items[item].full_name);
		return true;
	});
	
	item_dlg.run();
	if(item_dlg.accepted()) {
		quantity = item_dlg["amount"].getTextAsNum();
		if(optional)
			quantity += 1000 * item_dlg["chance"].getTextAsNum();
	} else item = was_item;
}

static void edit_shop_special(cDialog& parent, cItem& item, size_t& quantity) {
	using namespace std::placeholders;
	cDialog spec_dlg(*ResMgr::dialogs.get("edit-shop-special"), &parent);
	spec_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, _1, false));
	spec_dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, _1, true));
	spec_dlg["cost"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 10000, "cost"));
	spec_dlg["pickicon"].attachClickHandler(std::bind(pick_picture, PIC_ITEM, _1, "", "icon"));
	
	spec_dlg["edit"].attachClickHandler([](cDialog& me, std::string, eKeyMod) -> bool {
		int spec = me["node"].getTextAsNum();
		if(spec < 0) spec = get_fresh_spec(0);
		if(edit_spec_enc(spec, 0, &me))
			me["node"].setTextToNum(spec);
		return true;
	});
	
	spec_dlg["name"].setText(item.full_name);
	spec_dlg["descr"].setText(item.desc);
	spec_dlg["node"].setTextToNum(item.item_level);
	spec_dlg["cost"].setTextToNum(item.value);
	spec_dlg["amount"].setTextToNum(quantity);
	dynamic_cast<cPict&>(spec_dlg["icon"]).setPict(item.graphic_num);
	
	spec_dlg.run();
	if(spec_dlg.accepted()) {
		item.full_name = spec_dlg["name"].getText();
		item.desc = spec_dlg["descr"].getText();
		item.item_level = spec_dlg["node"].getTextAsNum();
		item.value = spec_dlg["cost"].getTextAsNum();
		quantity = spec_dlg["amount"].getTextAsNum();
		item.graphic_num = dynamic_cast<cPict&>(spec_dlg["icon"]).getPicNum();
	}
}

static bool edit_shop_entry(cDialog& me, std::string which, cShop& shop) {
	int btn_i = which[2] - '0';
	int i = dynamic_cast<cStack&>(me["items"]).getPage() * 5 + btn_i - 1;
	cShopItem entry = shop.getItem(i);
	eStrType list;
	std::string prompt;
	bool need_string = true;
	switch(entry.type) {
		case eShopItemType::EMPTY: return true;
		case eShopItemType::ITEM:
		case eShopItemType::OPT_ITEM:
			edit_shop_item(me, entry.index, entry.quantity, entry.type == eShopItemType::OPT_ITEM);
			entry.item = scenario.scen_items[entry.index];
			shop.replaceItem(i, entry);
			need_string = false;
			break;
		case eShopItemType::CLASS:
			entry.index = get_monst_abil_num("Which special class?", 1, 100, me, entry.index);
			shop.replaceSpecial(i, eShopItemType::CLASS, entry.index);
			need_string = false;
			break;
		case eShopItemType::CALL_SPECIAL:
			edit_shop_special(me, entry.item, entry.quantity);
			shop.replaceItem(i, entry);
			need_string = false;
			break;
		case eShopItemType::MAGE_SPELL: list = STRT_MAGE; prompt = "Which mage spell?"; break;
		case eShopItemType::PRIEST_SPELL: list = STRT_PRIEST; prompt = "Which priest spell?"; break;
		case eShopItemType::ALCHEMY: list = STRT_ALCHEMY; prompt = "Which recipe?"; break;
		case eShopItemType::SKILL: list = STRT_SKILL; prompt = "Which skill?"; break;
		case eShopItemType::TREASURE: list = STRT_TREASURE; prompt = "What class of treasure?"; break;
		case eShopItemType::CURE_ACID: case eShopItemType::CURE_DISEASE: case eShopItemType::CURE_POISON:
		case eShopItemType::CURE_DUMBFOUNDING: case eShopItemType::CURE_PARALYSIS: case eShopItemType::DESTONE:
		case eShopItemType::HEAL_WOUNDS: case eShopItemType::RAISE_DEAD: case eShopItemType::REMOVE_CURSE:
		case eShopItemType::RESURRECT:
			list = STRT_HEALING;
			break;
	}
	if(need_string) {
		entry.index = choose_text(list, entry.index, &me, prompt);
		if(list == STRT_HEALING)
			shop.replaceSpecial(i, eShopItemType(int(eShopItemType::HEAL_WOUNDS) + entry.index));
		else shop.replaceSpecial(i, entry.type, entry.index);
	}
	std::string pic_id = which, num_id = which, title_id = which;
	pic_id.replace(0,2,"pict"); num_id.replace(0,2,"n"); title_id.replace(0,2,"item");
	put_shop_item_in_dlog(dynamic_cast<cPict&>(me[pic_id]), me[num_id], me[title_id], shop, i);
	return true;
}

static bool delete_shop_entry(cDialog& me, std::string which, cShop& shop, size_t which_shop) {
	cStack& items = dynamic_cast<cStack&>(me["items"]);
	int page = items.getPage();
	shop.clearItem((which[3] - '1') + 5 * page);
	put_shop_in_dlog(me, shop, which_shop);
	if(page == items.getPageCount())
		page--;
	items.setPage(page);
	change_shop_dlog_items_page(me, "stay", shop);
	return true;
}

static bool add_shop_entry(cDialog& me, std::string type, cShop& shop, size_t which_shop) {
	if(type == "item" || type == "opt") {
		size_t which_item = 0, amount = 0;
		edit_shop_item(me, which_item, amount, type == "opt");
		if(scenario.scen_items[which_item].variety == eItemType::NO_ITEM)
			return true;
		if(scenario.scen_items[which_item].variety == eItemType::GOLD)
			return true;
		if(type == "item")
			shop.addItem(which_item, scenario.scen_items[which_item], amount);
		else shop.addItem(which_item, scenario.scen_items[which_item], amount % 1000, amount / 1000);
	} else if(type == "spec") {
		cItem item(ITEM_SPECIAL);
		size_t amount = 0;
		edit_shop_special(me, item, amount);
		shop.addSpecial(item.full_name, item.desc, item.graphic_num, item.item_level, item.value, amount);
	} else if(type == "class") {
		int n = get_monst_abil_num("Which special class?", 0, 100, me);
		if(n == 0) return true;
		shop.addSpecial(eShopItemType::CLASS, n);
	} else {
		eStrType list;
		eShopItemType item;
		std::string prompt;
		if(type == "mage") {
			list = STRT_MAGE;
			item = eShopItemType::MAGE_SPELL;
			prompt = "Which mage spell?";
		} else if(type == "priest") {
			list = STRT_PRIEST;
			item = eShopItemType::PRIEST_SPELL;
			prompt = "Which priest spell?";
		} else if(type == "alch") {
			list = STRT_ALCHEMY;
			item = eShopItemType::ALCHEMY;
			prompt = "Which recipe?";
		} else if(type == "skill") {
			list = STRT_SKILL;
			item = eShopItemType::SKILL;
			prompt = "Which skill?";
		} else if(type == "treas") {
			list = STRT_TREASURE;
			item = eShopItemType::TREASURE;
			prompt = "What class of treasure?";
		} else if(type == "heal") {
			list = STRT_HEALING;
			prompt = "What kind of healing?";
		} else {
			return false;
		}
		int i = choose_text(list, -1, &me, prompt);
		if(i == -1) return true;
		if(list == STRT_HEALING) {
			item = eShopItemType(int(eShopItemType::HEAL_WOUNDS) + i);
			i = 0;
		}
		shop.addSpecial(item, i);
	}
	put_shop_in_dlog(me, shop, which_shop);
	int atItem = shop.size() - 1;
	int onPage = atItem / 5;
	dynamic_cast<cStack&>(me["items"]).setPage(onPage);
	change_shop_dlog_items_page(me, "stay", shop);
	return true;
}

bool edit_shop(size_t which_shop, cDialog* parent) {
	using namespace std::placeholders;
	if(which_shop == scenario.shops.size())
		scenario.shops.emplace_back("New Shop");
	cShop shop = scenario.shops[which_shop];
	if(shop.size() == 0 && (shop.getName() == "New Shop" || shop.getName() == "Unused Shop")) {
		cChoiceDlog new_shop_dlg("new-shop", {"magic", "heal", "custom", "cancel"});
		std::string choice = new_shop_dlg.show();
		if(choice == "magic") {
			shop = cShop(SHOP_JUNK);
		} else if(choice == "heal") {
			shop = cShop(SHOP_HEALING);
		} else if(choice == "cancel") {
			return false;
		}
	}
	
	cDialog shop_dlg(*ResMgr::dialogs.get("edit-shop"), parent);
	shop_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, _1, false));
	shop_dlg["okay"].attachClickHandler(std::bind(save_shop_from_dlog, _1, std::ref(shop), std::ref(which_shop), true));
	shop_dlg["pickface"].attachClickHandler(std::bind(pick_picture, PIC_TALK, _1, "", "face"));
	shop_dlg.attachClickHandlers(std::bind(change_shop_dlog_items_page, _1, _2, std::ref(shop)), {"up", "down"});
	shop_dlg.attachClickHandlers(std::bind(delete_shop_entry, _1, _2, std::ref(shop), std::ref(which_shop)), {"del1", "del2", "del3", "del4", "del5"});
	shop_dlg.attachClickHandlers(std::bind(edit_shop_entry, _1, _2, std::ref(shop)), {"ed1", "ed2", "ed3", "ed4", "ed5"});
	shop_dlg.attachClickHandlers(std::bind(add_shop_entry, _1, _2, std::ref(shop), std::ref(which_shop)), {"item", "opt", "spec", "mage", "priest", "alch", "skill", "treas", "heal", "class"});
	
	if(scenario.shops.size() == 1 || parent != nullptr) {
		shop_dlg["left"].hide();
		shop_dlg["right"].hide();
	} else {
		shop_dlg.attachClickHandlers(std::bind(change_shop_dlog_page, _1, _2, std::ref(shop), std::ref(which_shop)), {"left", "right"});
	}
	
	put_shop_in_dlog(shop_dlg, shop, which_shop);
	shop_dlg.run();
	return shop_dlg.accepted();
}

static void put_vehicle_area(cDialog& me, const cVehicle& what) {
	std::ostringstream sout;
	if(what.which_town == 200) {
		sout << "Outdoors @ " << what.sector;
	} else {
		sout << "Town " << what.which_town;
	}
	me["area"].setText(sout.str());
}

bool edit_vehicle(cVehicle& what, int num, bool is_boat) {
	using namespace std::placeholders;
	cDialog dlg(*ResMgr::dialogs.get("edit-vehicle"));
	dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, &dlg, true));
	dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &dlg, false));
	dlg["del"].attachClickHandler([](cDialog& me, std::string, eKeyMod) {
		me.setResult(false);
		return me.toast(false);
	});
	
	put_vehicle_area(dlg, what);
	dlg["loc"].setText(boost::lexical_cast<std::string>(what.loc));
	location new_loc = what.loc;
	dlg["pick-loc"].attachClickHandler([&new_loc, is_boat](cDialog& me, std::string, eKeyMod) -> bool {
		cArea* area = get_current_area();
		cLocationPicker picker(new_loc, *area, is_boat ? "Move Boat" : "Move Horse", &me);
		new_loc = picker.run();
		me["loc"].setText(boost::lexical_cast<std::string>(new_loc));
		return true;
	});

	dlg["num"].setTextToNum(num);
	dlg["title"].setText(is_boat ? "Edit Boat" : "Edit Horse");
	dlg["name"].setText(what.name);
	
	cLed& prop = dynamic_cast<cLed&>(dlg["owned"]);
	prop.setState(what.property ? led_red : led_off);
	
	dlg.setResult(true);
	dlg.run();
	if(!dlg.getResult<bool>())
		return false;
	
	if(dlg.accepted()) {
		what.property = prop.getState() != led_off;
		what.exists = true;
		what.name = dlg["name"].getText();
		if(new_loc != what.loc){
			what.loc = new_loc;
			// Move editor view to keep showing vehicle
			cen_x = new_loc.x;
			cen_y = new_loc.y;
		}
	}
	return what.exists;
}

static bool save_add_town(cDialog& me) {
	cTilemap& grid = dynamic_cast<cTilemap&>(me["varying"]);
	for(short i = 0; i < scenario.town_mods.size(); i++) {
		int town = grid.getChild("town", 0, i).getTextAsNum();
		if(cre(town,
				-1,scenario.towns.size()-1,"Town number must be from 0 to " + std::to_string(scenario.towns.size() - 1) + " (or -1 for no effect).","",&me)) return false;
		int sdf_row = grid.getChild("flag-row", 0, i).getTextAsNum();
		if(cre(sdf_row,0,SDF_ROWS - 1,"First part of flag must be from 0 to " + std::to_string(SDF_ROWS - 1) + ".","",&me))
			return false;
		int sdf_col = grid.getChild("flag-col", 0, i).getTextAsNum();
		if(cre(sdf_col,0,SDF_COLUMNS - 1,"Second part of flag must be from 0 to " + std::to_string(SDF_COLUMNS - 1) + ".","",&me))
			return false;
		scenario.town_mods[i].spec = town;
		scenario.town_mods[i].x = sdf_col;
		scenario.town_mods[i].y = sdf_row;
	}
	return true;
}

static void put_add_town_in_dlog(cDialog& me) {
	cTilemap& grid = dynamic_cast<cTilemap&>(me["varying"]);
	for(short i = 0; i < scenario.town_mods.size(); i++) {
		grid.getChild("town", 0, i).setTextToNum(scenario.town_mods[i].spec);
		grid.getChild("flag-col", 0, i).setTextToNum(scenario.town_mods[i].x);
		grid.getChild("flag-row", 0, i).setTextToNum(scenario.town_mods[i].y);
	}
}

static bool edit_add_town_event_filter(cDialog& me, std::string hit) {
	if(hit == "okay") {
		if(save_add_town(me))
			me.toast(true);
	} else if(hit == "cancel") {
		me.toast(false);
	}
	return true;
}

void edit_add_town() {
	using namespace std::placeholders;
	
	cDialog vary_dlg(*ResMgr::dialogs.get("edit-town-varying"));
	vary_dlg.attachClickHandlers(std::bind(edit_add_town_event_filter, _1, _2), {"okay", "cancel"});
	cTilemap& grid = dynamic_cast<cTilemap&>(vary_dlg["varying"]);
	grid.attachClickHandlers([](cDialog& me, std::string hit, eKeyMod) {
		cTilemap& grid = dynamic_cast<cTilemap&>(me["varying"]);
		auto idx = grid.getCellIdx(me[hit]);
		auto& field = grid.getChild("town", idx.x, idx.y);
		int town = field.getTextAsNum();
		town = choose_text(STRT_TOWN, town, &me, "Which town?");
		field.setTextToNum(town);
		return true;
	}, "pick-town");
	grid.attachClickHandlers([](cDialog& me, std::string hit, eKeyMod) {
		cTilemap& grid = dynamic_cast<cTilemap&>(me["varying"]);
		auto idx = grid.getCellIdx(me[hit]);
		auto& fieldX = grid.getChild("flag-col", idx.x, idx.y);
		auto& fieldY = grid.getChild("flag-row", idx.x, idx.y);
		location sdf(fieldX.getTextAsNum(), fieldY.getTextAsNum());
		cStuffDonePicker picker(sdf);
		sdf = picker.run();
		fieldX.setTextToNum(sdf.x);
		fieldY.setTextToNum(sdf.y);
		return true;
	}, "pick-flag");
	
	put_add_town_in_dlog(vary_dlg);
	
	vary_dlg.run();
}

static bool save_item_placement(cDialog& me, cScenario::cItemStorage& storage, short which) {
	storage.property = dynamic_cast<cLed&>(me["owned"]).getState() != led_off;
	storage.ter_type = me["ter"].getTextAsNum();
	if(cre(storage.ter_type,
			-1,255,"Terrain Type must be from 0 to 255 (or -1 for No Shortcut).","",&me)) return false;
	for(short i = 0; i < 10; i++) {
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
	me["num"].setTextToNum(which);
	dynamic_cast<cLed&>(me["owned"]).setState(storage.property ? led_red : led_off);
	me["ter"].setTextToNum(storage.ter_type);
	for(short i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		me["item" + id].setTextToNum(storage.item_num[i]);
		me["odds" + id].setTextToNum(storage.item_odds[i]);
	}
}

static bool edit_item_placement_event_filter(cDialog& me, std::string hit, cScenario::cItemStorage& storage, short& which) {
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
		short i = me["ter"].getTextAsNum();
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
	
	cDialog shortcut_dlg(*ResMgr::dialogs.get("edit-item-shortcut"));
	shortcut_dlg.attachClickHandlers(std::bind(edit_item_placement_event_filter, _1, _2, std::ref(storage), std::ref(cur_shortcut)), {"okay", "cancel", "left", "right", "choose-ter"});
	for(int i = 0; i < 10; i++) {
		std::string id = "choose-item" + std::to_string(i + 1);
		shortcut_dlg[id].attachClickHandler(std::bind(edit_item_placement_select_item, _1, std::ref(storage), i + 1));
	}
	
	put_item_placement_in_dlog(shortcut_dlg, storage, cur_shortcut);
	
	shortcut_dlg.run();
}

static bool save_scen_details(cDialog& me, std::string, eKeyMod) {
	if(!me.toast(true)) return true;
	
	{
		cLedGroup& difficulty = dynamic_cast<cLedGroup&>(me["difficulty"]);
		scenario.difficulty = difficulty.getSelected()[3] - '1';
	}{
		cLedGroup& rating = dynamic_cast<cLedGroup&>(me["rating"]);
		switch(rating.getSelected()[4]) {
			case '1': scenario.rating = eContentRating::G; break;
			case '2': scenario.rating = eContentRating::PG; break;
			case '3': scenario.rating = eContentRating::R; break;
			case '4': scenario.rating = eContentRating::NC17; break;
		}
	}
	scenario.scen_name = me["title"].getText();
	for(short i = 0; i < 3; i++)
		scenario.format.ver[i] = me["ver" + std::to_string(i + 1)].getTextAsNum();
	scenario.teaser_text[0] = me["teaser1"].getText();
	scenario.teaser_text[1] = me["teaser2"].getText();
	scenario.contact_info[0] = me["author"].getText();
	scenario.contact_info[1] = me["contact"].getText();
	return true;
}

static void put_scen_details_in_dlog(cDialog& me) {
	me["title"].setText(scenario.scen_name);
	dynamic_cast<cLedGroup&>(me["difficulty"]).setSelected("lvl" + std::to_string(scenario.difficulty + 1));
	dynamic_cast<cLedGroup&>(me["rating"]).setSelected("rate" + std::to_string(scenario.rating + 1));
	for(int i = 0; i < 3; i++)
		me["ver" + std::to_string(i + 1)].setTextToNum(scenario.format.ver[i]);
	// Legacy meta text labels:
	if(!scenario.has_feature_flag("scenario-meta-format")){
		me["teaser1-label"].setText("Credits/Teaser Part 1:");
		me["teaser2-label"].setText("Credits/Teaser Part 2:");
		me["author-label"].setText("Author|(not displayed):");
		me["contact-label"].setText("Contact info|(not displayed):");
	}else{
		// Updated meta text labels:
		me["teaser2-label"].hide();
		me["teaser2"].hide();
	}

	me["teaser1"].setText(scenario.teaser_text[0]);
	me["teaser2"].setText(scenario.teaser_text[1]);
	me["author"].setText(scenario.contact_info[0]);
	me["contact"].setText(scenario.contact_info[1]);
}

static bool save_scen_adv_details(cDialog& me, std::string item_hit, eKeyMod) {
	me.toast(item_hit == "okay");
	return true;
}

static void put_scen_adv_details_in_dlog(cDialog& me) {
	dynamic_cast<cLed&>(me["adjust"]).setState(scenario.adjust_diff ? led_red : led_off);
	me["cpnid"].setText(scenario.campaign_id);
	me["bg-out"].setText("Outdoors: " + std::to_string(scenario.bg_out));
	me["bg-town"].setText("In towns: " + std::to_string(scenario.bg_town));
	me["bg-dungeon"].setText("In dungeons: " + std::to_string(scenario.bg_dungeon));
	me["bg-fight"].setText("In combat: " + std::to_string(scenario.bg_fight));
	me["oninit"].setTextToNum(scenario.init_spec);
}

static bool edit_scen_init_spec(cDialog& me, std::string, eKeyMod) {
	int spec = me["oninit"].getTextAsNum();
	if(spec < 0) spec = get_fresh_spec(0);
	if(edit_spec_enc(spec, 0, &me))
		me["oninit"].setTextToNum(spec);
	return true;
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
	cDialog info_dlg(*ResMgr::dialogs.get("edit-scenario-details"));
	info_dlg["okay"].attachClickHandler(save_scen_details);
	info_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &info_dlg, false));
	
	put_scen_details_in_dlog(info_dlg);
	
	info_dlg.run();
}

void edit_scen_adv_details() {
	cDialog info_dlg(*ResMgr::dialogs.get("edit-scenario-advanced"));
	info_dlg.attachClickHandlers(save_scen_adv_details, {"okay", "cancel"});
	info_dlg.attachClickHandlers(edit_scen_default_bgs, {"bg-out", "bg-town", "bg-dungeon", "bg-fight"});
	info_dlg["pickinit"].attachClickHandler(edit_scen_init_spec);

	put_scen_adv_details_in_dlog(info_dlg);
	info_dlg.run();

	if(info_dlg.accepted()){
		scenario.adjust_diff = dynamic_cast<cLed&>(info_dlg["adjust"]).getState() != led_red;

		scenario.campaign_id = info_dlg["cpnid"].getText();
		scenario.bg_out = boost::lexical_cast<int>(info_dlg["bg-out"].getText().substr(10));
		scenario.bg_town = boost::lexical_cast<int>(info_dlg["bg-town"].getText().substr(10));
		scenario.bg_dungeon = boost::lexical_cast<int>(info_dlg["bg-dungeon"].getText().substr(13));
		scenario.bg_fight = boost::lexical_cast<int>(info_dlg["bg-fight"].getText().substr(11));
		scenario.init_spec = info_dlg["oninit"].getTextAsNum();
	}
}

bool edit_make_scen_1(std::string& author,std::string& title,bool& grass) {
	cDialog new_dlog(*ResMgr::dialogs.get("make-scenario1"));
	new_dlog["okay"].attachClickHandler(std::bind(&cDialog::toast, &new_dlog, true));
	new_dlog["cancel"].attachClickHandler(std::bind(&cDialog::toast, &new_dlog, false));
	
	new_dlog.run();
	if(!new_dlog.accepted()) return false;
	
	title = new_dlog["name"].getText();
	author = new_dlog["author"].getText();
	grass = dynamic_cast<cLed&>(new_dlog["surface"]).getState() != led_off;
	return true;
}

static bool make_scen_check_towns(cDialog& me, std::string which, bool losing) {
	if(!losing) return true;
	bool have_wg = dynamic_cast<cLed&>(me["warrior-grove"]).getState() != led_off;
	int sm = me["town-s"].getTextAsNum(), med = me["town-m"].getTextAsNum(), lg = me["town-l"].getTextAsNum();
	if(have_wg) med++;
	if(sm < 0 || sm > 200 || med < 0 || med > 200 || lg < 0 || lg > 200) {
		std::ostringstream error;
		error << "Number of ";
		if(which == "town-s")
			error << "small";
		else if(which == "town-m")
			error << "medium";
		else if(which == "town-l")
			error << "large";
		error << " must be between 0 and 200";
		if(have_wg && which == "town-m")
			error << " (including Warrior's Grove)";
		showError(error.str(), &me);
		return false;
	}
	// TODO: Shouldn't this only be checked when exiting the dialog? At least for the case of no towns.
	int total = sm + med + lg;
	if(total < 1 || total > 200) {
		showError("The total number of towns must be from 1 to 200 (you must have at least 1 town).", &me);
		return false;
	}
	return true;
}

bool edit_make_scen_2(short& out_w, short& out_h, short& town_l, short& town_m, short& town_s, bool& def_town) {
	cDialog new_dlog(*ResMgr::dialogs.get("make-scenario2"));
	new_dlog["okay"].attachClickHandler(std::bind(&cDialog::toast, &new_dlog, true));
	new_dlog["cancel"].attachClickHandler(std::bind(&cDialog::toast, &new_dlog, false));
	new_dlog.attachFocusHandlers(make_scen_check_towns, {"town-s", "town-m", "town-l"});
	new_dlog["warrior-grove"].attachFocusHandler([](cDialog& me, std::string, bool losing) -> bool {
		if(losing) return true;
		int sm = me["town-s"].getTextAsNum(), med = me["town-m"].getTextAsNum(), lg = me["town-l"].getTextAsNum();
		if(sm + med + lg >= 200) {
			showError("Warrior's Grove is in addition to any towns listed above. As such, you need to request less than 200 towns if you want to use Warrior's Grove.", &me);
			return false;
		}
		return true;
	});
	
	new_dlog.run();
	if(!new_dlog.accepted()) return false;
	
	out_w = new_dlog["out-w"].getTextAsNum();
	out_h = new_dlog["out-h"].getTextAsNum();
	town_l = new_dlog["town-l"].getTextAsNum();
	town_m = new_dlog["town-m"].getTextAsNum();
	town_s = new_dlog["town-s"].getTextAsNum();
	def_town = dynamic_cast<cLed&>(new_dlog["warrior-grove"]).getState() != led_off;
	return true;
}

extern fs::path progDir;
extern eScenMode overall_mode;

bool build_scenario() {
	short width, height, lg, med, sm;
	bool default_town, grass;
	std::string author, title;
	cTown* warriors_grove = nullptr;
	std::vector<cShop> warriors_grove_shops;
	
	if(!edit_make_scen_1(author, title, grass))
		return false;
	if(!edit_make_scen_2(width, height, lg, med, sm, default_town))
		return false;
	
	scenario = cScenario();
	scenario.scen_name = title;
	scenario.contact_info[0] = author;
	scenario.default_ground = grass ? 2 : 0;
	
	scenario.feature_flags = {
		{"scenario-meta-format", "V2"},
		{"resurrection-balm", "required"},
	};

	fs::path basePath = progDir/"Blades of Exile Base"/(grass ? "bladbase.boes" : "cavebase.boes");
	if(!fs::exists(basePath)) {
		basePath = basePath.parent_path()/"bladbase.exs";
		if(!fs::exists(basePath)) {
			showError("Blades of Exile Base could not be found.");
			return false;
		}
	}
	cScenario base;
	load_scenario(basePath, base);
	// Load in all terrains, items, and monsters
	std::swap(scenario.ter_types, base.ter_types);
	std::swap(scenario.scen_items, base.scen_items);
	std::swap(scenario.scen_monsters, base.scen_monsters);
	
	if(default_town) {
		warriors_grove = base.towns[0];
		std::swap(warriors_grove_shops, base.shops);
		base.towns[0] = nullptr;
		if(med > 0) med--;
		warriors_grove->reattach(scenario);
	}
	
	scenario.shops.push_back(cShop(SHOP_HEALING));
	
	overall_mode = MODE_MAIN_SCREEN;
	editing_town = false;
	scenario.outdoors.resize(width, height);
	ter_num_t sign_terrain = base.outdoors[0][0]->terrain[23][23];
	ter_num_t town_terrain = base.outdoors[0][0]->terrain[24][24];
	ter_num_t replace_terrain = scenario.ter_types[town_terrain].flag1;
	base.outdoors[0][0]->terrain[23][23] = base.outdoors[0][0]->terrain[24][24] = replace_terrain;
	for(int x = 0; x < width; x++) {
		for(int y = 0; y < height; y++) {
			scenario.outdoors[x][y] = new cOutdoors(scenario);
			// Add borders
			// We divide the base outdoors into 9 16x16 segments
			// Top left segment goes at the top left of the top left sector.
			// Bottom left segment goes at the bottom left of the bottom left sector.
			// Top right segment goes at the top right of the top right sector.
			// Bottom right segment goes at the bottom right of the bottom right sector.
			// Top middle segment is duplicated along the top of each top sector.
			// Bottom middle segment is duplicated along the bottom of each bottom sector.
			// Left middle segment is duplicated along the left of each left sector.
			// Right middle segment is duplicated along the right of each right sector.
			// Central segment is duplicated everywhere else
			// - EXCEPT FOR the tiles at 23,23 and 24,24 - these are both replaced by 24,24's hidden terrain
			// - (The assumption being that 24,24 is a town terrain.)
			for(int i = 0; i < 16; i++) {
				// top left
				for(int j = 0; j < 16; j++) {
					scenario.outdoors[x][y]->terrain(i, j) = base.outdoors[0][0]->terrain(i + (x == 0 ? 0 : 16), j + (y == 0 ? 0 : 16));
				}
				// middle left
				for(int j = 16; j < 32; j++) {
					scenario.outdoors[x][y]->terrain(i, j) = base.outdoors[0][0]->terrain(i + (x == 0 ? 0 : 16), j);
				}
				// bottom left
				for(int j = 32; j < 48; j++) {
					scenario.outdoors[x][y]->terrain(i, j) = base.outdoors[0][0]->terrain(i + (x == 0 ? 0 : 16), j - (y == scenario.outdoors.height() - 1 ? 0 : 16));
				}
			}
			for(int i = 16; i < 32; i++) {
				// top middle
				for(int j = 0; j < 16; j++) {
					scenario.outdoors[x][y]->terrain(i, j) = base.outdoors[0][0]->terrain(i, j + (y == 0 ? 0 : 16));
				}
				// middle middle
				for(int j = 16; j < 32; j++) {
					scenario.outdoors[x][y]->terrain(i, j) = base.outdoors[0][0]->terrain(i, j);
				}
				// bottom middle
				for(int j = 32; j < 48; j++) {
					scenario.outdoors[x][y]->terrain(i, j) = base.outdoors[0][0]->terrain(i, j - (y == scenario.outdoors.height() - 1 ? 0 : 16));
				}
			}
			for(int i = 32; i < 48; i++) {
				// top right
				for(int j = 0; j < 16; j++) {
					scenario.outdoors[x][y]->terrain(i, j) = base.outdoors[0][0]->terrain(i - (x == scenario.outdoors.width() - 1 ? 0 : 16), j + (y == 0 ? 0 : 16));
				}
				// middle right
				for(int j = 16; j < 32; j++) {
					scenario.outdoors[x][y]->terrain(i, j) = base.outdoors[0][0]->terrain(i - (x == scenario.outdoors.width() - 1 ? 0 : 16), j);
				}
				// bottom right
				for(int j = 32; j < 48; j++) {
					scenario.outdoors[x][y]->terrain(i, j) = base.outdoors[0][0]->terrain(i - (x == scenario.outdoors.width() - 1 ? 0 : 16), j - (y == scenario.outdoors.height() - 1 ? 0 : 16));
				}
			}
		}
	}
	cur_out.x = 0;
	cur_out.y = 0;
	current_terrain = scenario.outdoors[0][0];
	// Add starting town and sign
	current_terrain->terrain[24][24] = town_terrain;
	current_terrain->terrain[23][23] = sign_terrain;
	current_terrain->city_locs.push_back({24, 24, 0});
	
	if(default_town && warriors_grove) {
		scenario.towns.push_back(warriors_grove);
		std::swap(scenario.shops, warriors_grove_shops);
	}
	
	for(short i = 0; i < lg; i++) {
		scenario.addTown(AREA_LARGE);
		scenario.towns.back()->name = "Large town " + std::to_string(i + 1);
	}
	for(short i = 0; i < med; i++) {
		scenario.addTown(AREA_MEDIUM);
		scenario.towns.back()->name = "Medium town " + std::to_string(i + 1);
	}
	for(short i = 0; i < sm; i++) {
		scenario.addTown(AREA_SMALL);
		scenario.towns.back()->name = "Small town " + std::to_string(i + 1);
	}
	cur_town = 0;
	town = scenario.towns[0];
	
	scenario.scen_file.clear();
	save_scenario();
	return true;
}

static bool save_scenario_events(cDialog& me, std::string, eKeyMod) {
	if(!me.toast(true)) return true;
	cStack& stk = dynamic_cast<cStack&>(me["list"]);

	for(short i = 0; i < scenario.scenario_timers.size(); i++) {
		stk.setPage(i / 10);
		short fieldId = i % 10;
		std::string id = std::to_string(fieldId + 1);
		scenario.scenario_timers[i].time = stk["time" + id].getTextAsNum();
		scenario.scenario_timers[i].node = stk["node" + id].getTextAsNum();
	}
	return true;
}

static bool check_scenario_timer_time(cDialog& me, std::string id, bool losing) {
	if(!losing) return true;
	int val = me[id].getTextAsNum();
	if(val > 0 && val % 10 != 0) {
		showError("All scenario event times must be multiples of 10 (e.g. 100, 150, 1000, etc.).");
		return false;
	}
	return true;
}

static bool edit_scenario_events_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	// item_hit is of the form editN; we need an ID of the form nodeN
	item_hit.replace(0, 4, "node");
	short spec = me[item_hit].getTextAsNum();
	if(spec < 0)
		spec = get_fresh_spec(0);
	if(edit_spec_enc(spec,0,&me))
		me[item_hit].setTextToNum(spec);
	return true;
}

void edit_scenario_events() {
	using namespace std::placeholders;
	
	cDialog evt_dlg(*ResMgr::dialogs.get("edit-scenario-events"));
	cStack& stk = dynamic_cast<cStack&>(evt_dlg["list"]);
	evt_dlg["prev"].attachClickHandler([&stk] (cDialog&, std::string, eKeyMod) { return stk.setPage(0); });
	evt_dlg["next"].attachClickHandler([&stk] (cDialog&, std::string, eKeyMod) { return stk.setPage(1); });
	evt_dlg["okay"].attachClickHandler(save_scenario_events);
	evt_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &evt_dlg, false));
	for(int i = 0; i < scenario.scenario_timers.size(); i++) {
		stk.setPage(i / 10);
		short fieldId = i % 10;
		std::string id = std::to_string(fieldId + 1);
		stk["time" + id].setTextToNum(scenario.scenario_timers[i].time);
		stk["node" + id].setTextToNum(scenario.scenario_timers[i].node);
	}
	stk.setPage(0);
	for(int i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		evt_dlg["time" + id].attachFocusHandler(check_scenario_timer_time);
		evt_dlg["node" + id].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, scenario.scen_specials.size(), "The scenario special node", "-1 for no special"));
		evt_dlg["edit" + id].attachClickHandler(edit_scenario_events_event_filter);
	}
	evt_dlg.run();
}

static void fill_custom_pics_types(cDialog& me, std::vector<ePicType>& pics, pic_num_t first) {
	pic_num_t last = first + 9;
	if(last >= pics.size()) pics.resize(last + 1, PIC_NONE);
	me["num0"].setTextToNum(first);
	for(pic_num_t i = first; i <= last; i++) {
		std::string id = std::to_string(i - first + 1);
		cLedGroup& grp = dynamic_cast<cLedGroup&>(me["type" + id]);
		cPict& pic = dynamic_cast<cPict&>(me["pic" + id]);
		pic.setPict(i, PIC_CUSTOM_ITEM);
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
			case PIC_STATUS: // Not currently supported, but reserve for later
				break;
			default: // Fix any potential errors
				pics[i] = PIC_NONE;
				BOOST_FALLTHROUGH;
			case PIC_NONE:
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
		pics[pic] = PIC_NONE;
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
		showError("You don't have any custom graphics to classify!");
		return;
	}
	using namespace std::placeholders;
	std::vector<ePicType> pics = scenario.custom_graphics;
	pic_num_t first_pic = 0;
	
	cDialog pic_dlg(*ResMgr::dialogs.get("graphic-types"));
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

static void set_dlg_custom_sheet(cDialog& me, size_t sheet) {
	me["num"].setTextToNum(sheet);
	dynamic_cast<cPict&>(me["sheet"]).setPict(sheet, PIC_FULL);
}

extern fs::path tempDir;
extern std::string scenario_temp_dir_name;

void edit_custom_sheets() {
	int max_pic = -1;
	std::vector<int> all_pics;
	fs::path pic_dir = tempDir/scenario_temp_dir_name/"graphics";
	if(!scenario.scen_file.has_extension()) // It's an unpacked scenario
		pic_dir = scenario.scen_file/"graphics";
	if(!fs::exists(pic_dir)) fs::create_directories(pic_dir);
	for(fs::directory_iterator iter(pic_dir); iter != fs::directory_iterator(); iter++) {
		std::string fname = iter->path().filename().string().c_str();
		int dot = fname.find_last_of('.');
		if(fname.substr(0,5) == "sheet" && fname.substr(dot) == ".png" && std::all_of(fname.begin()+5, fname.begin()+dot, isdigit)) {
			int this_pic = boost::lexical_cast<int>(fname.substr(5,dot-5));
			max_pic = max(max_pic, this_pic);
			all_pics.push_back(this_pic);
		}
	}
	
	// First, make sure we even have custom graphics! Also make sure they're not legacy format.
	bool must_init_spec_g = false;
	if(spec_scen_g.is_old) {
		if(cChoiceDlog("convert-pics-now", {"cancel", "convert"}).show() == "cancel")
			return;
		spec_scen_g.convert_sheets();
		all_pics.resize(spec_scen_g.numSheets);
		std::iota(all_pics.begin(), all_pics.end(), 0);
	} else if(max_pic < 0) {
		if(cChoiceDlog("have-no-pics", {"cancel", "new"}).show() == "cancel")
			return;
		must_init_spec_g = true;
	} else if(max_pic >= 0 && spec_scen_g.numSheets < 1) {
		if(cChoiceDlog("have-only-full-pics", {"cancel", "new"}).show() == "new")
			must_init_spec_g = true;
	}
	
	if(must_init_spec_g) {
		spec_scen_g.clear();
		spec_scen_g.sheets.resize(1);
		spec_scen_g.numSheets = 1;
		spec_scen_g.init_sheet(0);
		spec_scen_g.sheets[0]->copyToImage().saveToFile((pic_dir/"sheet0.png").string().c_str());
		all_pics.insert(all_pics.begin(), 0);
		ResMgr::graphics.pushPath(pic_dir);
	}
	
	set_cursor(watch_curs);
	
	// Get image data from the sheets in memory
	size_t cur = 0;
	std::unordered_map<size_t, sf::Image> sheets;
	for(size_t i = 0; i < spec_scen_g.numSheets; i++) {
		sheets[i] = spec_scen_g.sheets[i]->copyToImage();
	}
	auto sheetsSave = sheets;
	
	using namespace std::placeholders;
	
	cDialog pic_dlg(*ResMgr::dialogs.get("graphic-sheets"));
	pic_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, _1, false));
	pic_dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, _1, true));
	pic_dlg["copy"].attachClickHandler([&sheets,&cur,&all_pics,&pic_dir](cDialog&, std::string, eKeyMod) -> bool {
		if(cur >= spec_scen_g.numSheets) {
			fs::path fromPath = pic_dir/("sheet" + std::to_string(all_pics[cur]) + ".png");
			sf::Image img;
			img.loadFromFile(fromPath.string().c_str());
			set_clipboard_img(img);
			return true;
		}
		set_clipboard_img(sheets[cur]);
		return true;
	});
	pic_dlg["paste"].attachClickHandler([&sheets,&cur,&all_pics,&pic_dir](cDialog&, std::string, eKeyMod) -> bool {
		auto img = get_clipboard_img();
		if(img == nullptr) {
			beep();
			return true;
		}
		if(cur >= spec_scen_g.numSheets) {
			std::string resName = "sheet" + std::to_string(all_pics[cur]);
			fs::path toPath = pic_dir/(resName + ".png");
			img->saveToFile(toPath.string().c_str());
			ResMgr::graphics.free(resName);
			return true;
		}
		sheets[cur] = *img;
		spec_scen_g.replace_sheet(cur, *img);
		return true;
	});
	pic_dlg["open"].attachClickHandler([&sheets,&cur,&all_pics,&pic_dir](cDialog&, std::string, eKeyMod) -> bool {
		fs::path fpath = nav_get_rsrc({"png", "bmp", "jpg", "jpeg", "gif", "psd"});
		if(fpath.empty()) return true;
		sf::Image img;
		if(!img.loadFromFile(fpath.string().c_str())) {
			beep();
			return true;
		}
		if(cur >= spec_scen_g.numSheets) {
			std::string resName = "sheet" + std::to_string(all_pics[cur]);
			fs::path toPath = pic_dir/(resName + ".png");
			img.saveToFile(toPath.string().c_str());
			ResMgr::graphics.free(resName);
			return true;
		}
		sheets[cur] = img;
		spec_scen_g.replace_sheet(cur, img);
		return true;
	});
	pic_dlg["save"].attachClickHandler([&sheets,&cur,&all_pics,&pic_dir](cDialog&, std::string, eKeyMod) -> bool {
		fs::path fpath = nav_put_rsrc({"png", "bmp", "jpg", "jpeg"});
		if(fpath.empty()) return true;
		if(cur >= spec_scen_g.numSheets) {
			fs::path fromPath = pic_dir/("sheet" + std::to_string(all_pics[cur]) + ".png");
			sf::Image img;
			img.loadFromFile(fromPath.string().c_str());
			img.saveToFile(fpath.string().c_str());
			return true;
		}
		sheets[cur].saveToFile(fpath.string().c_str());
		return true;
	});
	pic_dlg["new"].attachClickHandler([&sheets,&cur,&all_pics,&pic_dir](cDialog& me, std::string, eKeyMod) -> bool {
		cChoiceDlog pickNum("add-new-sheet", {"cancel", "new"}, &me);
		pickNum->getControl("num").setTextToNum(spec_scen_g.numSheets);
		if(pickNum.show() == "cancel") return true;
		int newSheet = pickNum->getControl("num").getTextAsNum();
		fs::path sheetPath = pic_dir/("sheet" + std::to_string(newSheet) + ".png");
		if(newSheet == spec_scen_g.numSheets) {
			spec_scen_g.sheets.emplace_back();
			spec_scen_g.init_sheet(newSheet);
			spec_scen_g.sheets[newSheet]->copyToImage().saveToFile(sheetPath.string().c_str());
			spec_scen_g.numSheets++;
			auto iter = all_pics.insert(std::upper_bound(all_pics.begin(), all_pics.end(), newSheet), newSheet);
			cur = iter - all_pics.begin();
		} else {
			auto iter = std::lower_bound(all_pics.begin(), all_pics.end(), newSheet);
			if(*iter == newSheet) {
				showError("Sorry, but that sheet already exists! Try creating a sheet with a different number.", "Sheet number: " + std::to_string(newSheet), &me);
				return true;
			}
			iter = all_pics.insert(iter, newSheet);
			cur = iter - all_pics.begin();
			sf::Image img;
			img.create(280, 360);
			img.saveToFile(sheetPath.string().c_str());
		}
		me["left"].show();
		me["right"].show();
		set_dlg_custom_sheet(me, all_pics[cur]);
		return true;
	});
	pic_dlg["del"].attachClickHandler([&sheets,&cur,&all_pics,&pic_dir](cDialog& me, std::string, eKeyMod) -> bool {
		int which_pic = all_pics[cur];
		if(which_pic < spec_scen_g.numSheets) {
			std::string choice = "del";
			if(which_pic < spec_scen_g.numSheets - 1)
				choice = cChoiceDlog("must-delete-in-order", {"cancel", "del", "move"}, &me).show();
			if(choice == "cancel") return true;
			if(choice == "move") {
				spec_scen_g.sheets.erase(spec_scen_g.sheets.begin() + which_pic);
				spec_scen_g.numSheets--;
				for(; which_pic < spec_scen_g.numSheets; which_pic++) {
					fs::path from = pic_dir/("sheet" + std::to_string(which_pic + 1) + ".png");
					fs::path to = pic_dir/("sheet" + std::to_string(which_pic) + ".png");
					if(!fs::exists(from)) continue; // Just in case
					fs::remove(to);
					fs::rename(from, to);
					ResMgr::graphics.free("sheet" + std::to_string(which_pic));
				}
				auto end = std::find(all_pics.begin() + cur, all_pics.end(), which_pic - 1);
				if(end != all_pics.end())
					all_pics.erase(end);
				else {
					// This shouldn't be reached
					std::cerr << "Whoops! Somehow failed to remove the index of the deleted sheet!" << std::endl;
				}
			} else if(choice == "del") {
				all_pics.erase(all_pics.begin() + cur);
				spec_scen_g.numSheets = which_pic;
				spec_scen_g.sheets.resize(which_pic);
				ResMgr::graphics.free("sheet" + std::to_string(which_pic));
			}
		}
		fs::path fpath = pic_dir/("sheet" + std::to_string(which_pic) + ".png");
		if(fs::exists(fpath)) fs::remove(fpath);
		if(all_pics.size() == 1) {
			me["left"].hide();
			me["right"].hide();
		} else if(all_pics.empty()) {
			cStrDlog("You've just deleted the last custom graphics sheet, so this dialog will now close. If you want to add more sheets, you can of course reopen the dialog.", "", "Last Sheet Deleted", 16, PIC_DLOG).show();
			me.toast(true);
			return true;
		}
		if(cur > 0) cur--;
		set_dlg_custom_sheet(me, all_pics[cur]);
		return true;
	});
	
	if(all_pics.size() == 1) {
		pic_dlg["left"].hide();
		pic_dlg["right"].hide();
	}
	pic_dlg.attachClickHandlers([&sheets,&cur,&all_pics](cDialog& me, std::string dir, eKeyMod) -> bool {
		if(dir == "left") {
			if(cur == 0)
				cur = all_pics.size() - 1;
			else cur--;
		} else if(dir == "right") {
			cur++;
			if(cur >= all_pics.size())
				cur = 0;
		} else return true;
		set_dlg_custom_sheet(me, all_pics[cur]);
		return true;
	}, {"left", "right"});
	
	set_dlg_custom_sheet(pic_dlg, all_pics[cur]);
	shut_down_menus(5); // So that cmd+O, cmd+N, cmd+S can work
	pic_dlg.run();
	
	// Now, we need to restore the sheets if they pressed cancel
	if(!pic_dlg.accepted()) {
		for(auto p : sheetsSave) {
			spec_scen_g.replace_sheet(p.first, p.second);
		}
	}
	
	// Restore menus
	shut_down_menus(4);
	if(overall_mode <= MODE_MAIN_SCREEN)
		shut_down_menus(editing_town ? 2 : 1);
	else shut_down_menus(3);
}

static bool edit_custom_sound_action(cDialog& me, std::string action, std::vector<std::string>& snd_names, int curPage, int& max_snd) {
	size_t a_len = action.length();
	int which_snd = (curPage + 1) * 100 + (action[a_len-1] - '0');
	action.erase(action.end() - 1);
	fs::path sndpath = tempDir/scenario_temp_dir_name/"sounds";
	std::string sndbasenm = "SND" + std::to_string(which_snd);
	fs::path sndfile = sndpath/(sndbasenm + ".wav");
	if(action != "open" && !fs::exists(sndfile)) {
		beep();
		return true;
	}
	if(action == "play") {
		play_sound(-which_snd);
	} else if(action == "del") {
		if(which_snd - 100 < snd_names.size())
			snd_names[which_snd - 100].clear();
		fs::remove(sndfile);
		me["name" + std::to_string(which_snd % 10)].setText("");
	} else if(action == "open") {
		fs::path fpath = nav_get_rsrc({"wav"});
		if(fpath.empty()) return true;
		sf::SoundBuffer snd;
		if(!snd.loadFromFile(fpath.string().c_str())) {
			beep();
			return true;
		}
		fs::copy_file(fpath, sndfile, fs::copy_options::overwrite_existing);
		ResMgr::sounds.free(sound_to_fname(which_snd));
		if(which_snd > max_snd)
			max_snd = which_snd;
		if(max_snd % 10 == 9) {
			me["left"].show();
			me["right"].show();
		}
	} else if(action == "save") {
		fs::path fpath = nav_put_rsrc({"wav"});
		if(fpath.empty()) return true;
		fs::copy_file(sndfile, fpath, fs::copy_options::overwrite_existing);
	}
	return true;
}

static void fill_custom_sounds_page(cDialog& me, std::vector<std::string>& snd_names, int& curPage, int& max_snd, bool firstTime) {
	for(int i = 0; i < 10; i++) {
		int which_snd = (curPage + 1) * 100 + i;
		std::string id = std::to_string(i);
		if(firstTime) {
			using namespace std::placeholders;
			std::vector<std::string> buttons = {
				"play" + id, "del" + id,
				"open" + id, "save" + id,
			};
			me.attachClickHandlers(std::bind(edit_custom_sound_action, _1, _2, std::ref(snd_names), std::ref(curPage), std::ref(max_snd)), buttons);
		}
		me["num" + id].setTextToNum(which_snd);
		if(which_snd - 100 < snd_names.size())
			me["name" + id].setText(snd_names[which_snd - 100]);
	}
}

static void get_sound_names_from_dlg(cDialog& me, std::vector<std::string>& snd_names, int curPage) {
	for(int i = 9; i >= 0; i--) {
		std::string id = "name" + std::to_string(i);
		size_t index = curPage * 100 + i;
		std::string name = me[id].getText();
		if(!name.empty()) {
			if(snd_names.size() <= index)
				snd_names.resize(index + 1);
			snd_names[index] = name;
		} else if(index < snd_names.size())
			snd_names[index].clear();
	}
}

void edit_custom_sounds() {
	cDialog snd_dlg(*ResMgr::dialogs.get("edit-sounds"));
	snd_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &snd_dlg, false));
	snd_dlg["okay"].attachClickHandler(std::bind(&cDialog::toast, &snd_dlg, true));
	
	int max_snd = 99;
	fs::path snd_dir = tempDir/scenario_temp_dir_name/"sounds";
	if(!fs::exists(snd_dir)) fs::create_directories(snd_dir);
	for(fs::directory_iterator iter(snd_dir); iter != fs::directory_iterator(); iter++) {
		std::string fname = iter->path().filename().string().c_str();
		int dot = fname.find_last_of('.');
		if(fname.substr(0,3) == "snd" && fname.substr(dot) == ".wav" && std::all_of(fname.begin()+3, fname.begin()+dot, isdigit))
			max_snd = max(max_snd, boost::lexical_cast<int>(fname.substr(3,dot-3)));
	}
	
	int curPage = 0;
	auto snd_names = scenario.snd_names;
	fill_custom_sounds_page(snd_dlg, snd_names, curPage, max_snd, true);
	
	if(max_snd < 110) {
		snd_dlg["left"].hide();
		snd_dlg["right"].hide();
	}
	snd_dlg.attachClickHandlers([&curPage,&max_snd,&snd_names](cDialog& me, std::string dir, eKeyMod) -> bool {
		get_sound_names_from_dlg(me, snd_names, curPage);
		if(dir == "left") {
			if(curPage == 0)
				curPage = max_snd / 10 - 10;
			else curPage--;
		} else if(dir == "right") {
			curPage++;
			if(curPage > max_snd / 10 - 10)
				curPage = 0;
		} else return true;
		fill_custom_sounds_page(me, snd_names, curPage, max_snd, false);
		return true;
	}, {"left", "right"});
	
	snd_dlg.run();
	if(snd_dlg.accepted()) {
		get_sound_names_from_dlg(snd_dlg, snd_names, curPage);
		snd_names.swap(scenario.snd_names);
	}
}

fs::path run_file_picker(bool saving){
	return os_file_picker(saving);
}
