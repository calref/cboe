
#include <cstdio>
#include <cstring>
#include <functional>
#include <numeric>
#include <boost/lexical_cast.hpp>
#include "scen.global.h"
#include "classes.h"
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

extern short cen_x, cen_y,cur_town;
extern bool mouse_button_held;
extern short cur_viewing_mode;
extern cTown* town;
extern cOutdoors* current_terrain;
extern short mode_count,to_create;
extern ter_num_t template_terrain[64][64];
extern cScenario scenario;
extern cCustomGraphics spec_scen_g;
extern cSpecial null_spec_node;
extern cSpeech null_talk_node;
extern location cur_out;
extern short start_volume, start_dir;

static bool save_ter_info(cDialog& me, cTerrain& store_ter) {
	
	store_ter.picture = me["pict"].getTextAsNum();
	// TODO: Should somehow verify the pict number is valid
	
	std::string blockage = dynamic_cast<cLedGroup&>(me["blockage"]).getSelected();
	if(blockage == "clear") store_ter.blockage = eTerObstruct::CLEAR;
	else if(blockage == "curtain") store_ter.blockage = eTerObstruct::BLOCK_SIGHT;
	else if(blockage == "special") store_ter.blockage = eTerObstruct::BLOCK_MONSTERS;
	else if(blockage == "window") store_ter.blockage = eTerObstruct::BLOCK_MOVE;
	else if(blockage == "obstructed") store_ter.blockage = eTerObstruct::BLOCK_MOVE_AND_SHOOT;
	else if(blockage == "opaque") store_ter.blockage = eTerObstruct::BLOCK_MOVE_AND_SIGHT;
	store_ter.special = (eTerSpec) boost::lexical_cast<short>(dynamic_cast<cLedGroup&>(me["prop"]).getSelected().substr(4));
	/*
	i = CDGN(813,6);
	if((store_ter.special < 2) || (store_ter.special > 6)) {
		if(cre(i,0,256,"First special flag must be from 0 to 255.","",813)) return false;
	}
	else if(store_ter.special == eTerSpec::DAMAGING) {
		if(cre(i,0,256,"First special flag must be from 0 to 100.","",813)) return false;
	}
	else if(store_ter.special == eTerSpec::DANGEROUS) {
		if(cre(i,0,256,"First special flag must be from 0 to 8.","",813)) return false;
	}
	*/
	if(store_ter.special == eTerSpec::NONE)
		store_ter.flag1.s = me["flag1"].getTextAsNum();
	else store_ter.flag1.u = me["flag1"].getTextAsNum();
	if(false) // flag2 is never signed, apparently; but that could change?
		store_ter.flag2.s = me["flag2"].getTextAsNum();
	else store_ter.flag2.u = me["flag2"].getTextAsNum();
	if(store_ter.special == eTerSpec::CALL_SPECIAL || store_ter.special == eTerSpec::CALL_SPECIAL_WHEN_USED)
		store_ter.flag3.s = me["flag3"].getTextAsNum();
	else store_ter.flag3.u = me["flag3"].getTextAsNum();
	
	/*
	if(store_ter.special == eTerSpec::TOWN_ENTRANCE) {
		if(cre(i,0,256,"Second special flag must be from 0 to 200.","",813)) return false;
	}
	else if(store_ter.special == eTerSpec::DAMAGING || store_ter.special == eTerSpec::DANGEROUS) {
		if(cre(i,0,256,"Second special flag must be from 0 to 100.","",813)) return false;
	}
	*/
	
	/*
	if(cre(i,0,255,"Transform To What must be from 0 to 255.","",813)) return false;
	*/
	store_ter.trans_to_what = me["trans"].getTextAsNum();
	store_ter.fly_over = dynamic_cast<cLed&>(me["flight"]).getState() == led_red;
	store_ter.boat_over = dynamic_cast<cLed&>(me["boat"]).getState() == led_red;
	store_ter.block_horse = dynamic_cast<cLed&>(me["horse"]).getState();
	store_ter.light_radius = me["light"].getTextAsNum();
	/*
	 if(cre(store_ter.light_radius,0,8,"Light radius must be from 0 to 8.","",813)) return false;
	 */
	
	std::string sound = dynamic_cast<cLedGroup&>(me["sound"]).getSelected();
	// TODO: Uh, why not use an actual sound number instead of 0...4?
	if(sound == "step") store_ter.step_sound = 0;
	else if(sound == "squish") store_ter.step_sound = 1;
	else if(sound == "crunch") store_ter.step_sound = 2;
	else if(sound == "nosound") store_ter.step_sound = 3;
	else if(sound == "splash") store_ter.step_sound = 4;
	
	std::string shortcut = me["key"].getText();
	if(shortcut.length() > 0) store_ter.shortcut_key = shortcut[0];
	else store_ter.shortcut_key = 0;
	
	store_ter.name = me["name"].getText();
	
	store_ter.ground_type = me["ground"].getTextAsNum();
	store_ter.trim_ter = me["trimter"].getTextAsNum();
	store_ter.trim_type = (eTrimType) me["trim"].getTextAsNum();
	store_ter.combat_arena = me["arena"].getTextAsNum();
	store_ter.map_pic = me["map"].getTextAsNum();
	
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

static bool fill_ter_flag_info(cDialog& me, std::string id, bool){
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
	switch(prop) {
		case eTerSpec::NONE:
			me["pickflag1"].hide(); // TODO: Could have a pick graphic dialog for the editor icon, but that requires adding a new graphic type
			break;
	}
	// TODO: Click handlers for the "choose" buttons as necessary, plus hide/show them as needed
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
	me["key"].setTextToNum(ter_type.shortcut_key);
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
			case 0:
				led_ctrl.setSelected("step");
				break;
			case 1:
				led_ctrl.setSelected("squish");
				break;
			case 2:
				led_ctrl.setSelected("crunch");
				break;
			case 3:
				led_ctrl.setSelected("nosound");
				break;
			case 4:
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
	if(ter_type.special == eTerSpec::NONE)
		me["flag1"].setTextToNum(ter_type.flag1.s);
	else me["flag1"].setTextToNum(ter_type.flag1.u);
	if(false) // flag2 is never signed, apparently; but that could change?
		me["flag2"].setTextToNum(ter_type.flag2.s);
	else me["flag2"].setTextToNum(ter_type.flag2.u);
	if(ter_type.special == eTerSpec::CALL_SPECIAL || ter_type.special == eTerSpec::CALL_SPECIAL_WHEN_USED)
		me["flag3"].setTextToNum(ter_type.flag3.s);
	else me["flag3"].setTextToNum(ter_type.flag3.u);
	me["arena"].setTextToNum(ter_type.combat_arena);
}

static bool finish_editing_ter(cDialog& me, std::string id, ter_num_t& which_ter) {
	if(!save_ter_info(me, scenario.ter_types[which_ter])) return true;
	
	if(id == "done") me.toast(true);
	else if(id == "left") {
		// TODO: Use size() once ter_types becomes a vector
		if(which_ter == 0) which_ter = 255;
		else which_ter--;
		fill_ter_info(me, which_ter);
	} else if(id == "right") {
		which_ter++;
		if(which_ter > 255) which_ter = 0;
		fill_ter_info(me, which_ter);
	}
	return true;
}

short edit_ter_type(ter_num_t which_ter) {
	using namespace std::placeholders;
	cDialog ter_dlg("edit-terrain");
	// Attach handlers
	ter_dlg["pict"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,2999,"terrain graphic"));
	ter_dlg["pickpict"].attachClickHandler(std::bind(pick_picture,PIC_TER,_1,"pict","graphic"));
	ter_dlg["pickanim"].attachClickHandler(std::bind(pick_picture,PIC_TER_ANIM,_1,"pict","graphic"));
	ter_dlg["light"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,255,"light radius"));
	ter_dlg["trans"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,65535,"\"transform to what?\""));
	ter_dlg["ground"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,255,"ground type"));
	ter_dlg["trimter"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,255,"trim terrain"));
	ter_dlg["trim"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,18,"trim type"));
	ter_dlg["prop"].attachFocusHandler(fill_ter_flag_info);
	ter_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &ter_dlg, false));
	ter_dlg["arena"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,299,"ground type"));
	// TODO: Add focus handler for key
	// TODO: Add click handler for object
	ter_dlg.attachClickHandlers(std::bind(finish_editing_ter,_1,_2,std::ref(which_ter)), {"left", "right", "done"});
	ter_dlg["picktrim"].attachClickHandler(std::bind(pick_string,"trim-names", _1, "trim", ""));
	ter_dlg["pickarena"].attachClickHandler(std::bind(pick_string,"arena-names", _1, "arena", ""));
	ter_dlg["help"].attachClickHandler(std::bind(show_help, "ter-type-help", _1, 16));
	fill_ter_info(ter_dlg,which_ter);
	ter_dlg.run();
	// TODO: What should be returned?
	return 0;
}

static void put_monst_info_in_dlog(cDialog& me, cMonster& store_monst, mon_num_t which_monst) {
	std::ostringstream strb;
	
	if(store_monst.picture_num < 1000)
		dynamic_cast<cPict&>(me["icon"]).setPict(store_monst.picture_num,PIC_MONST);
	else {
		ePicType type_g = PIC_CUSTOM_MONST;
		short size_g = store_monst.picture_num / 1000;
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
		dynamic_cast<cPict&>(me["icon"]).setPict(store_monst.picture_num,type_g);
	}
	me["num"].setTextToNum(which_monst);
	me["name"].setText(store_monst.m_name);
	me["pic"].setTextToNum(store_monst.picture_num);
	strb << "Width = " << int(store_monst.x_width);
	me["w"].setText(strb.str());
	strb.str("");
	strb << "Height = " << int(store_monst.y_width);
	me["h"].setText(strb.str());
	me["level"].setTextToNum(store_monst.level);
	me["health"].setTextToNum(store_monst.m_health);
	me["armor"].setTextToNum(store_monst.armor);
	me["skill"].setTextToNum(store_monst.skill);
	me["speed"].setTextToNum(store_monst.speed);
	me["mage"].setTextToNum(store_monst.mu);
	me["priest"].setTextToNum(store_monst.cl);
	me["dice1"].setTextToNum(store_monst.a[0].dice);
	me["sides1"].setTextToNum(store_monst.a[0].sides);
	me["dice2"].setTextToNum(store_monst.a[1].dice);
	me["sides2"].setTextToNum(store_monst.a[1].sides);
	me["dice3"].setTextToNum(store_monst.a[2].dice);
	me["sides3"].setTextToNum(store_monst.a[2].sides);
	me["talk"].setTextToNum(store_monst.default_facial_pic);
	me["treas"].setTextToNum(store_monst.treasure);
	
	cLedGroup& attitude = dynamic_cast<cLedGroup&>(me["attitude"]);
	switch(store_monst.default_attitude) {
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
	
	me["type"].setText(get_str("traits",33 + int(store_monst.m_type) * 2));
	me["type1"].setText(get_str("monster-abilities",130 + int(store_monst.a[0].type)));
	me["type2"].setText(get_str("monster-abilities",130 + int(store_monst.a[1].type)));
	me["type3"].setText(get_str("monster-abilities",130 + int(store_monst.a[2].type)));
}

static bool check_monst_pic(cDialog& me, std::string id, bool losing, cMonster& store_monst) {
	if(!losing) return true;
	static size_t max_preset = m_pic_index.size() - 1;
	static const std::string error = "Non-customized monster pic must be from 0 to " + std::to_string(max_preset) + ".";
	if(check_range(me, id, losing, 0, 4999, "Monster pic")) {
		pic_num_t pic = me[id].getTextAsNum();
		store_monst.picture_num = pic;
		cPict& icon = dynamic_cast<cPict&>(me["icon"]);
		switch(pic / 1000) {
			case 0:
				if(cre(pic,0,max_preset,error,"",&me)) return false;
				store_monst.x_width = m_pic_index[store_monst.picture_num].x;
				store_monst.y_width = m_pic_index[store_monst.picture_num].y;
				icon.setPict(pic, PIC_MONST);
				break;
			case 1:
				store_monst.x_width = 1;
				store_monst.y_width = 1;
				icon.setPict(pic, PIC_MONST);
				break;
			case 2:
				store_monst.x_width = 2;
				store_monst.y_width = 1;
				icon.setPict(pic, PIC_MONST_WIDE);
				break;
			case 3:
				store_monst.x_width = 1;
				store_monst.y_width = 2;
				icon.setPict(pic, PIC_MONST_TALL);
				break;
			case 4:
				store_monst.x_width = 2;
				store_monst.y_width = 2;
				icon.setPict(pic, PIC_MONST_LG);
				break;
		}
		std::ostringstream strb;
		strb << "Width = " << int(store_monst.x_width);
		me["w"].setText(strb.str());
		strb.str("");
		strb << "Height = " << int(store_monst.y_width);
		me["h"].setText(strb.str());
	}
	return true;
}

static bool save_monst_info(cDialog& me, cMonster& store_monst) {
	
	store_monst.m_name = me["name"].getText();
	store_monst.picture_num = me["pic"].getTextAsNum();
	store_monst.level = me["level"].getTextAsNum();
	store_monst.m_health = me["health"].getTextAsNum();
	store_monst.armor = me["armor"].getTextAsNum();
	store_monst.skill = me["skill"].getTextAsNum();
	store_monst.speed = me["speed"].getTextAsNum();
	store_monst.mu = me["mage"].getTextAsNum();
	store_monst.cl = me["priest"].getTextAsNum();
	
	store_monst.a[0].dice = me["dice1"].getTextAsNum();
	store_monst.a[1].dice = me["dice2"].getTextAsNum();
	store_monst.a[2].dice = me["dice3"].getTextAsNum();
	store_monst.a[0].sides = me["sides1"].getTextAsNum();
	store_monst.a[1].sides = me["sides2"].getTextAsNum();
	store_monst.a[2].sides = me["sides3"].getTextAsNum();
	
	store_monst.default_facial_pic = me["talk"].getTextAsNum();
	store_monst.treasure = me["treas"].getTextAsNum();
	std::string def_att = dynamic_cast<cLedGroup&>(me["attitude"]).getSelected();
	if(def_att == "docile") store_monst.default_attitude = 0;
	else if(def_att == "friendly") store_monst.default_attitude = 2;
	else if(def_att == "A") store_monst.default_attitude = 1;
	else if(def_att == "B") store_monst.default_attitude = 3;
	return true;
}

static bool edit_monst_type_event_filter(cDialog& me,std::string item_hit,cMonster& store_monst,short& which_monst) {
	short i;
	cMonster temp_monst;
	
	if(item_hit == "okay") {
		if(save_monst_info(me,store_monst)) {
			scenario.scen_monsters[which_monst] = store_monst;
			me.toast(true);
		}
	} else if(item_hit == "abils") {
		if(!save_monst_info(me,store_monst)) return false;
		temp_monst = edit_monst_abil(store_monst,which_monst);
		if(temp_monst.level < 255)
			store_monst = temp_monst;
		put_monst_info_in_dlog(me,store_monst,which_monst);
	} else if(item_hit == "left") {
		if(!save_monst_info(me,store_monst)) return false;
		scenario.scen_monsters[which_monst] = store_monst;
		which_monst--;
		// TODO: Use size() once scen_monsters becomes a vector
		if(which_monst < 1) which_monst = 255;
		store_monst = scenario.scen_monsters[which_monst];
		put_monst_info_in_dlog(me,store_monst,which_monst);
	} else if(item_hit == "right") {
		if(!save_monst_info(me,store_monst)) return false;
		scenario.scen_monsters[which_monst] = store_monst;
		which_monst++;
		if(which_monst > 255) which_monst = 1;
		store_monst = scenario.scen_monsters[which_monst];
		put_monst_info_in_dlog(me,store_monst,which_monst);
	} else if(item_hit == "picktype") {
		if(!save_monst_info(me,store_monst)) return false;
		i = choose_text(STRT_RACE,int(store_monst.m_type),&me,"Choose Monster Type:");
		if(i >= 0) {
			store_monst.m_type = (eRace) i;
			put_monst_info_in_dlog(me,store_monst,which_monst);
		}
	} else if(item_hit == "picktype1") {
		if(!save_monst_info(me,store_monst)) return false;
		i = choose_text_res("monster-abilities",130,139,int(store_monst.a[0].type),&me,"Choose Attack 1 Type:");
		if(i >= 0) {
			store_monst.a[0].type = eMonstMelee(i);
			put_monst_info_in_dlog(me,store_monst,which_monst);
		}
	} else if(item_hit == "picktype2") {
		if(!save_monst_info(me,store_monst)) return false;
		i = choose_text_res("monster-abilities",130,139,int(store_monst.a[1].type),&me,"Choose Attack 2 & 3 Type:");
		if(i >= 0) {
			store_monst.a[1].type = store_monst.a[2].type = eMonstMelee(i);
			put_monst_info_in_dlog(me,store_monst,which_monst);
		}
	} else if(item_hit == "picktype3") {
		if(!save_monst_info(me,store_monst)) return false;
		i = choose_text_res("monster-abilities",130,139,int(store_monst.a[2].type),&me,"Choose Attack 3 Type:");
		if(i >= 0) {
			store_monst.a[2].type = eMonstMelee(i);
			put_monst_info_in_dlog(me,store_monst,which_monst);
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

short edit_monst_type(short which_monst) {
	using namespace std::placeholders;
	cMonster store_monst = scenario.scen_monsters[which_monst];
	
	cDialog monst_dlg("edit-monster");
	monst_dlg["pickicon"].attachClickHandler(std::bind(pick_monst_picture,_1));
	monst_dlg["picktalk"].attachClickHandler(std::bind(pick_picture,PIC_TALK,_1,"talk",""));
	monst_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &monst_dlg, false));
	monst_dlg["pic"].attachFocusHandler(std::bind(check_monst_pic, _1, _2, _3, std::ref(store_monst)));
	monst_dlg["level"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 40, "level"));
	monst_dlg["health"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 2500, "health"));
	monst_dlg["armor"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 50, "armor"));
	monst_dlg["skill"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 40, "skill"));
	monst_dlg["speed"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 2, 12, "speed"));
	monst_dlg["mage"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 7, "magic spells"));
	monst_dlg["priest"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 7, "priest spells"));
	monst_dlg["treas"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 4, "treasure"));
	monst_dlg.attachFocusHandlers(check_monst_dice,{"dice1","dice2","dice3","sides1","sides2","sides3"});
	monst_dlg.attachClickHandlers(std::bind(edit_monst_type_event_filter,_1,_2,std::ref(store_monst),std::ref(which_monst)),{"okay","abils","left","right","picktype","picktype1","picktype2","picktype3"});
	
	put_monst_info_in_dlog(monst_dlg, store_monst, which_monst);
	
	monst_dlg.run();
	return 0;
}

static void put_monst_abils_in_dlog(cDialog& me, cMonster& store_monst, short which_monst) {
	me["num"].setTextToNum(which_monst);
	
	me["loot-item"].setTextToNum(store_monst.corpse_item);
	me["loot-chance"].setTextToNum(store_monst.corpse_item_chance);
	me["magic-res"].setTextToNum(store_monst.magic_res);
	me["fire-res"].setTextToNum(store_monst.fire_res);
	me["cold-res"].setTextToNum(store_monst.cold_res);
	me["poison-res"].setTextToNum(store_monst.poison_res);
	me["onsee"].setTextToNum(store_monst.see_spec);
	me["snd"].setTextToNum(store_monst.ambient_sound);
	
	if(store_monst.mindless)
		dynamic_cast<cLed&>(me["mindless"]).setState(led_red);
	if(store_monst.invuln)
		dynamic_cast<cLed&>(me["invuln"]).setState(led_red);
	if(store_monst.invisible)
		dynamic_cast<cLed&>(me["invis"]).setState(led_red);
	if(store_monst.guard)
		dynamic_cast<cLed&>(me["guard"]).setState(led_red);
	
	dynamic_cast<cLedGroup&>(me["summon"]).setSelected("s" + boost::lexical_cast<std::string,short>(store_monst.summon_type));
	
	cStack& abils = dynamic_cast<cStack&>(me["abils"]);
	abils.setPageCount(0); // This clears out any data still in the stack
	abils.setPageCount(1);
	int i = 0;
	for(auto& abil : store_monst.abil) {
		if(!abil.second.active) continue;
		std::string id = std::to_string((i % 4) + 1);
		if(i % 4 == 0) abils.setPage(i / 4);
		else if(i % 4 == 3) abils.addPage();
		me["abil-name" + id].setText(abil.second.to_string(abil.first));
		me["abil-edit" + id].setText("Edit");
		i++;
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

static bool save_monst_abils(cDialog& me, cMonster& store_monst) {
	store_monst.summon_type = boost::lexical_cast<short>(dynamic_cast<cLedGroup&>(me["summon"]).getSelected().substr(1));
	
 	store_monst.corpse_item = me["loot-item"].getTextAsNum();
	store_monst.corpse_item_chance = me["loot-chance"].getTextAsNum();
	store_monst.magic_res = me["magic-res"].getTextAsNum();
	store_monst.fire_res = me["fire-res"].getTextAsNum();
	store_monst.cold_res = me["cold-res"].getTextAsNum();
	store_monst.poison_res = me["poison-res"].getTextAsNum();
	store_monst.see_spec = me["onsee"].getTextAsNum();
	store_monst.ambient_sound = me["snd"].getTextAsNum();
	
	store_monst.mindless = dynamic_cast<cLed&>(me["mindless"]).getState() != led_off;
	store_monst.invuln = dynamic_cast<cLed&>(me["invuln"]).getState() != led_off;
	store_monst.invisible = dynamic_cast<cLed&>(me["invis"]).getState() != led_off;
	store_monst.guard = dynamic_cast<cLed&>(me["guard"]).getState() != led_off;
	return true;
}

static bool edit_monst_abil_event_filter(cDialog& me,std::string item_hit,cMonster& store_monst) {
	using namespace std::placeholders;
	
	if(item_hit == "cancel") {
		store_monst.level = 255;
		me.toast(false);
	} else if(item_hit == "okay") {
		if(save_monst_abils(me, store_monst))
			me.toast(true);
	} else if(item_hit == "abil-up") {
		cStack& abils = dynamic_cast<cStack&>(me["abils"]);
		if(abils.getPage() == 0) abils.setPage(abils.getPageCount() - 1);
		else abils.setPage(abils.getPage() - 1);
	} else if(item_hit == "abil-down") {
		cStack& abils = dynamic_cast<cStack&>(me["abils"]);
		if(abils.getPage() >= abils.getPageCount() - 1) abils.setPage(0);
		else abils.setPage(abils.getPage() + 1);
	} else if(item_hit == "edit-see") {
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
	} else if(item_hit == "pick-snd") {
		int i = me["snd"].getTextAsNum();
		i = choose_text(STRT_SND, i, &me, "Select monster vocalization sound:");
		me["snd"].setTextToNum(i);
	}
	return true;
}

cMonster edit_monst_abil(cMonster starting_record,short which_monst) {
	using namespace std::placeholders;
	cMonster store_monst = starting_record;
	
	cDialog monst_dlg("edit-monster-abils");
	monst_dlg["loot-item"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 399, "Item To Drop", "-1 for no item"));
	monst_dlg["loot-chance"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 100, "Dropping Chance", "-1 for no item"));
	monst_dlg.attachClickHandlers(std::bind(edit_monst_abil_event_filter, _1, _2, std::ref(store_monst)), {"okay", "cancel", "abil-up", "abil-down", "edit-see", "pick-snd"});
	
	put_monst_abils_in_dlog(monst_dlg, store_monst, which_monst);
	
	monst_dlg.run();
	return store_monst;
}

static void put_item_info_in_dlog(cDialog& me, cItem& store_item, short which_item) {
	me["num"].setTextToNum(which_item);
	me["full"].setText(store_item.full_name);
	me["short"].setText(store_item.name);
	if(store_item.graphic_num >= 1000) // was 150
		dynamic_cast<cPict&>(me["pic"]).setPict(store_item.graphic_num, PIC_CUSTOM_ITEM);
	else dynamic_cast<cPict&>(me["pic"]).setPict(store_item.graphic_num, PIC_ITEM);
	me["picnum"].setTextToNum(store_item.graphic_num);
	
	cLedGroup& variety = dynamic_cast<cLedGroup&>(me["variety"]);
	switch(store_item.variety) {
		case eItemType::NO_ITEM:
			variety.setSelected("none");
			break;
		case eItemType::ONE_HANDED:
			variety.setSelected("weap1");
			break;
		case eItemType::TWO_HANDED:
			variety.setSelected("weap2");
			break;
		case eItemType::GOLD:
			variety.setSelected("gold");
			break;
		case eItemType::BOW:
			variety.setSelected("bow");
			break;
		case eItemType::ARROW:
			variety.setSelected("arrow");
			break;
		case eItemType::THROWN_MISSILE:
			variety.setSelected("thrown");
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
			break;
		case eItemType::BOLTS:
			variety.setSelected("bolt");
			break;
		case eItemType::MISSILE_NO_AMMO:
			variety.setSelected("missile");
			break;
		case eItemType::UNUSED1:
			variety.setSelected("unused1");
			break;
		case eItemType::UNUSED2:
			variety.setSelected("unused2");
			break;
	}
	
	cLedGroup& weapType = dynamic_cast<cLedGroup&>(me["melee-type"]);
	switch(store_item.weap_type) {
		case eSkill::INVALID:
		case eSkill::EDGED_WEAPONS:
			weapType.setSelected("edge");
			break;
		case eSkill::BASHING_WEAPONS:
			weapType.setSelected("bash");
			break;
		case eSkill::POLE_WEAPONS:
			weapType.setSelected("pole");
			break;
	}
	
	me["level"].setTextToNum(store_item.item_level);
	me["awkward"].setTextToNum(store_item.awkward);
	me["bonus"].setTextToNum(store_item.bonus);
	me["prot"].setTextToNum(store_item.protection);
	me["charges"].setTextToNum(store_item.charges);
	me["flag"].setTextToNum(store_item.type_flag);
	me["value"].setTextToNum(store_item.value);
	me["weight"].setTextToNum(store_item.weight);
	me["class"].setTextToNum(store_item.special_class);
}

static bool save_item_info(cDialog& me, cItem& store_item, short which_item) {
	store_item.full_name = me["full"].getText();
	store_item.name = me["short"].getText();
	store_item.graphic_num = me["picnum"].getTextAsNum();
	
	std::string variety = dynamic_cast<cLedGroup&>(me["variety"]).getSelected();
	if(variety == "none") store_item.variety = eItemType::NO_ITEM;
	else if(variety == "weap1") store_item.variety = eItemType::ONE_HANDED;
	else if(variety == "weap2") store_item.variety = eItemType::TWO_HANDED;
	else if(variety == "gold") store_item.variety = eItemType::GOLD;
	else if(variety == "bow") store_item.variety = eItemType::BOW;
	else if(variety == "arrow") store_item.variety = eItemType::ARROW;
	else if(variety == "thrown") store_item.variety = eItemType::THROWN_MISSILE;
	else if(variety == "potion") store_item.variety = eItemType::POTION;
	else if(variety == "scroll") store_item.variety = eItemType::SCROLL;
	else if(variety == "wand") store_item.variety = eItemType::WAND;
	else if(variety == "tool") store_item.variety = eItemType::TOOL;
	else if(variety == "food") store_item.variety = eItemType::FOOD;
	else if(variety == "shield") store_item.variety = eItemType::SHIELD;
	else if(variety == "armor") store_item.variety = eItemType::ARMOR;
	else if(variety == "helm") store_item.variety = eItemType::HELM;
	else if(variety == "gloves") store_item.variety = eItemType::GLOVES;
	else if(variety == "shield2") store_item.variety = eItemType::SHIELD_2;
	else if(variety == "boots") store_item.variety = eItemType::BOOTS;
	else if(variety == "ring") store_item.variety = eItemType::RING;
	else if(variety == "necklace") store_item.variety = eItemType::NECKLACE;
	else if(variety == "poison") store_item.variety = eItemType::WEAPON_POISON;
	else if(variety == "nonuse") store_item.variety = eItemType::NON_USE_OBJECT;
	else if(variety == "pants") store_item.variety = eItemType::PANTS;
	else if(variety == "xbow") store_item.variety = eItemType::CROSSBOW;
	else if(variety == "bolt") store_item.variety = eItemType::BOLTS;
	else if(variety == "missile") store_item.variety = eItemType::MISSILE_NO_AMMO;
	else if(variety == "unused1") store_item.variety = eItemType::UNUSED1;
	else if(variety == "unused2") store_item.variety = eItemType::UNUSED2;
	store_item.weap_type = eSkill::INVALID;
	if(store_item.variety == eItemType::ONE_HANDED || store_item.variety == eItemType::TWO_HANDED) {
		std::string weapType = dynamic_cast<cLedGroup&>(me["melee-type"]).getSelected();
		if(weapType == "edge") store_item.weap_type = eSkill::EDGED_WEAPONS;
		else if(weapType == "bash") store_item.weap_type = eSkill::BASHING_WEAPONS;
		else if(weapType == "pole") store_item.weap_type = eSkill::POLE_WEAPONS;
	}
	
	store_item.item_level = me["level"].getTextAsNum();
	if((store_item.variety == eItemType::GOLD || store_item.variety == eItemType::FOOD) && store_item.item_level == 0)
		store_item.item_level = 1;
	
	store_item.awkward = me["awkward"].getTextAsNum();
	store_item.bonus = me["bonus"].getTextAsNum();;
	store_item.protection = me["prot"].getTextAsNum();
	store_item.charges = me["charges"].getTextAsNum();
	store_item.type_flag = me["flag"].getTextAsNum();
	store_item.value = me["value"].getTextAsNum();
	store_item.weight = me["weight"].getTextAsNum();
	store_item.special_class = me["class"].getTextAsNum();
	
	if((store_item.type_flag > 0) && (store_item.charges == 0)) {
		giveError("If the Type Flag is greater than 0, the Charges must also be greater than 0.","",&me);
		return false;
	}
	if(store_item.variety == eItemType::UNUSED1 || store_item.variety == eItemType::UNUSED2) {
		giveError("The Unused item varieties are reserved for later expansions, and can't be used now.","",&me);
		return false;
	}
	eItemAbilCat cat = getItemAbilCategory(store_item.ability);
	if((cat == eItemAbilCat::SPELL || cat == eItemAbilCat::NONSPELL || cat == eItemAbilCat::REAGENT) && (store_item.charges == 0)) {
		giveError("An item with the special ability selected must have at least 1 charge.","",&me);
		return false;
	}
	scenario.scen_items[which_item] = store_item;
	return true;
}

static bool edit_item_type_event_filter(cDialog& me, std::string item_hit, cItem& store_item, short& store_which_item) {
	short i;
	cItem temp_item;
	
	if(item_hit == "cancel") {
		me.toast(false);
	} else if(item_hit == "okay") {
		if(save_item_info(me, store_item, store_which_item)) me.toast(true);
	} else if(item_hit == "prev") {
		if(!save_item_info(me, store_item, store_which_item)) return true;
		store_which_item--;
		if(store_which_item < 0) store_which_item = 399;
		store_item = scenario.scen_items[store_which_item];
		put_item_info_in_dlog(me, store_item, store_which_item);
	} else if(item_hit == "next") {
		if(!save_item_info(me, store_item, store_which_item)) return true;
		store_which_item++;
		if(store_which_item > 399) store_which_item = 0;
		store_item = scenario.scen_items[store_which_item];
		put_item_info_in_dlog(me, store_item, store_which_item);
	} else if(item_hit == "choosepic") {
		if(!save_item_info(me, store_item, store_which_item)) return true;
		i = pick_picture(PIC_ITEM, me, "picnum", "pic");
		if(i < 0) return true;
		store_item.graphic_num = i;
	} else if(item_hit == "abils") {
		if(store_item.variety == eItemType::NO_ITEM) {
			giveError("You must give the item a type (weapon, armor, etc.) before you can choose its abilities.","",&me);
			return true;
		}
		if(store_item.variety == eItemType::GOLD || store_item.variety == eItemType::FOOD) {
			giveError("Gold and Food cannot be given special abilities.","",&me);
			return true;
		}
		temp_item = edit_item_abil(store_item,store_which_item);
		if(temp_item.variety != eItemType::NO_ITEM)
			store_item = temp_item;
	}
	return true;
}

short edit_item_type(short which_item) {
	using namespace std::placeholders;
	cItem store_item = scenario.scen_items[which_item];
	
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
	item_dlg.attachClickHandlers(std::bind(edit_item_type_event_filter, _1, _2, std::ref(store_item), std::ref(which_item)), {"okay", "cancel", "prev", "next", "abils", "choosepic"});
	
	put_item_info_in_dlog(item_dlg, store_item, which_item);
	
	item_dlg.run();
	return 0;
}

static void put_item_abils_in_dlog(cDialog& me, cItem& store_item, short which_item) {
	
	me["num"].setTextToNum(which_item);
	me["name"].setText(store_item.full_name.c_str());
	me["variety"].setText(get_str("item-types", (int)store_item.variety));
	me["abilname"].setText(get_str("item-abilities", int(store_item.ability) + 1));
	
	dynamic_cast<cLedGroup&>(me["use-type"]).setSelected("use" + std::to_string(store_item.magic_use_type));
	dynamic_cast<cLedGroup&>(me["treasure"]).setSelected("type" + std::to_string(store_item.treas_class));
	me["str"].setTextToNum(store_item.abil_data[0]);
	
	dynamic_cast<cLed&>(me["always-id"]).setState(store_item.ident ? led_red : led_off);
	dynamic_cast<cLed&>(me["magic"]).setState(store_item.magic ? led_red : led_off);
	dynamic_cast<cLed&>(me["cursed"]).setState(store_item.cursed ? led_red : led_off);
	dynamic_cast<cLed&>(me["conceal"]).setState(store_item.concealed ? led_red : led_off);
}

static bool save_item_abils(cDialog& me, cItem& store_item) {
	store_item.magic_use_type = boost::lexical_cast<short>(dynamic_cast<cLedGroup&>(me["use-type"]).getSelected().substr(3));
	store_item.treas_class = boost::lexical_cast<short>(dynamic_cast<cLedGroup&>(me["treasure"]).getSelected().substr(4));
	store_item.abil_data[0] = me["str"].getTextAsNum();
	
	store_item.property = store_item.enchanted = store_item.contained = false;
	store_item.ident = dynamic_cast<cLed&>(me["always-id"]).getState() != led_off;
	store_item.magic = dynamic_cast<cLed&>(me["magic"]).getState() != led_off;
	store_item.cursed = store_item.unsellable = dynamic_cast<cLed&>(me["cursed"]).getState() != led_off;
	store_item.concealed = dynamic_cast<cLed&>(me["conceal"]).getState() != led_off;
	return true;
}

static bool edit_item_abil_event_filter(cDialog& me, std::string item_hit, cItem& store_item, short which_item) {
	short i;
	
	if(item_hit == "cancel") {
		store_item.ability = eItemAbil::NONE;
		me.toast(false);
		return true;
	} else if(item_hit == "okay") {
		if(save_item_abils(me, store_item)) {
			me.toast(true);
			return true;
		}
	} else if(item_hit == "weapon") {
		if(!save_item_abils(me, store_item)) return true;
		if(store_item.variety != eItemType::ONE_HANDED && store_item.variety != eItemType::TWO_HANDED) {
			giveError("You can only give an ability of this sort to a melee weapon.","",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 0, 14, int(store_item.ability), &me, "Choose Weapon Ability (inherent)");
		if(i >= 0) store_item.ability = eItemAbil(i);
		else store_item.ability = eItemAbil::NONE;
		put_item_abils_in_dlog(me, store_item, which_item);
	} else if(item_hit == "general") {
		if(!save_item_abils(me, store_item)) return true;
		if((store_item.variety == eItemType::ARROW) || (store_item.variety == eItemType::THROWN_MISSILE)|| (store_item.variety == eItemType::POTION) || (store_item.variety == eItemType::SCROLL) ||
		   (store_item.variety == eItemType::WAND) || (store_item.variety == eItemType::TOOL) || (store_item.variety == eItemType::WEAPON_POISON) ||
		   (store_item.variety == eItemType::NON_USE_OBJECT) || (store_item.variety == eItemType::BOLTS)){
			giveError("You can only give an ability of this sort to an non-missile item which can be equipped (like armor, or a ring).","",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 30, 62, int(store_item.ability), &me, "Choose General Ability (inherent)");
		if(i >= 0) store_item.ability = eItemAbil(i + 30);
		else store_item.ability = eItemAbil::NONE;
		put_item_abils_in_dlog(me, store_item, which_item);
	} else if(item_hit == "usable") {
		if(!save_item_abils(me, store_item)) return true;
		if((store_item.variety == eItemType::ARROW) || (store_item.variety == eItemType::THROWN_MISSILE) || (store_item.variety == eItemType::BOLTS)){
			giveError("You can only give an ability of this sort to an item which isn't a missile.","",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 70, 94, int(store_item.ability), &me, "Choose Usable Ability (Not spell)");
		if(i >= 0) store_item.ability = eItemAbil(i + 70);
		else store_item.ability = eItemAbil::NONE;
		put_item_abils_in_dlog(me, store_item, which_item);
	} else if(item_hit == "spell") {
		if(!save_item_abils(me,store_item)) return true;
		if((store_item.variety == eItemType::ARROW) || (store_item.variety == eItemType::THROWN_MISSILE) || (store_item.variety == eItemType::BOLTS)){
			giveError("You can only give an ability of this sort to an item which isn't a missile.","",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 110, 135, int(store_item.ability), &me, "Choose Usable Ability (Spell)");
		if(i >= 0) store_item.ability = eItemAbil(i + 110);
		else store_item.ability = eItemAbil::NONE;
		put_item_abils_in_dlog(me, store_item, which_item);
	} else if(item_hit == "reagent") {
		if(!save_item_abils(me, store_item)) return true;
		// TODO: Some of these should also be applicable to tools, as I recall?
		if(store_item.variety != eItemType::NON_USE_OBJECT){
			giveError("You can only give an ability of this sort to an item of type Non-Use Object.","",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 150, 161, int(store_item.ability), &me, "Choose Reagent Ability");
		if(i >= 0) store_item.ability = eItemAbil(i + 150);
		else store_item.ability = eItemAbil::NONE;
		put_item_abils_in_dlog(me, store_item, which_item);
	} else if(item_hit == "missile") {
		if(!save_item_abils(me,store_item)) return true;
		if((store_item.variety == eItemType::ARROW) || (store_item.variety == eItemType::THROWN_MISSILE) || (store_item.variety == eItemType::BOLTS)){
			giveError("You can only give an ability of this sort to an item which isn't a missile.","",&me);
			return true;
		}
		i = choose_text_res("item-abilities", 170, 176, int(store_item.ability), &me, "Choose Missile Ability");
		if(i >= 0) store_item.ability = eItemAbil(i + 170);
		else store_item.ability = eItemAbil::NONE;
		put_item_abils_in_dlog(me, store_item, which_item);
	}
	using namespace std::placeholders;
	if(store_item.ability != eItemAbil::SUMMONING && store_item.ability != eItemAbil::MASS_SUMMONING) {
		me["str"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 10, "Ability Strength"));
	} else {
		me["str"].attachFocusHandler(std::bind(check_range_msg, _1,_2,_3, 0,255, "Ability Strength","the number of the monster to summon"));
	}
	return true;
}

cItem edit_item_abil(cItem starting_record,short which_item) {
	using namespace std::placeholders;
	
	cItem store_item = starting_record;
	
	cDialog item_dlg("edit-item-abils");
	if(store_item.ability != eItemAbil::SUMMONING && store_item.ability != eItemAbil::MASS_SUMMONING) {
		item_dlg["str"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 10, "Ability Strength"));
	} else {
		item_dlg["str"].attachFocusHandler(std::bind(check_range_msg,_1,_2,_3, 0,255, "Ability Strength","the number of the monster to summon"));
	}
	item_dlg.attachClickHandlers(std::bind(edit_item_abil_event_filter, _1, _2, std::ref(store_item), which_item), {"okay", "cancel", "weapon", "general", "usable", "missile", "reagent", "spell"});
	
	put_item_abils_in_dlog(item_dlg, store_item, which_item);
	
	item_dlg.run();
	
	return store_item;
}

static void put_spec_item_in_dlog(cDialog& me, cSpecItem& store_item, short which_item) {
	me["num"].setTextToNum(which_item);
	me["name"].setText(store_item.name);
	me["descr"].setText(store_item.descr);
	me["spec"].setTextToNum(store_item.special);
	dynamic_cast<cLed&>(me["start-with"]).setState(store_item.flags >= 10 ? led_red : led_off);
	dynamic_cast<cLed&>(me["usable"]).setState(store_item.flags % 10 > 0 ? led_red : led_off);
}

static bool save_spec_item(cDialog& me, cSpecItem& store_item, short which_item) {
	store_item.name = me["name"].getText().substr(0,25);
	store_item.descr = me["descr"].getText();
	store_item.special = me["spec"].getTextAsNum();
	store_item.flags = 0;
	if(dynamic_cast<cLed&>(me["start-with"]).getState() != led_off)
		store_item.flags += 10;
	if(dynamic_cast<cLed&>(me["usable"]).getState() != led_off)
		store_item.flags += 1;
	scenario.special_items[which_item] = store_item;
	return true;
}

static bool edit_spec_item_event_filter(cDialog& me, std::string item_hit, cSpecItem& store_item, short which_item) {
	if(item_hit == "cancel") {
		me.toast(false);
	} else if(item_hit == "okay") {
		if(save_spec_item(me, store_item, which_item)) me.toast(true);
	} else if(item_hit == "left") {
		if(!save_spec_item(me, store_item, which_item)) return true;
		which_item--;
		if(which_item < 0) which_item = 49;
		store_item = scenario.special_items[which_item];
		put_spec_item_in_dlog(me, store_item, which_item);
	} else if(item_hit == "right") {
		if(!save_spec_item(me, store_item, which_item)) return true;
		which_item++;
		if(which_item > 49) which_item = 0;
		store_item = scenario.special_items[which_item];
		put_spec_item_in_dlog(me, store_item, which_item);
	} else if(item_hit == "edit-spec") {
		if(!save_spec_item(me, store_item, which_item)) return true;
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
		save_spec_item(me, store_item, which_item);
		
	}
	return true;
}

void edit_spec_item(short which_item) {
	using namespace std::placeholders;
	cSpecItem store_item = scenario.special_items[which_item];
	
	cDialog item_dlg("edit-special-item");
	item_dlg["spec"].attachFocusHandler(std::bind(check_range_msg, _1, _2, _3, -1, 255, "Scenario special node called", "-1 for no special"));
	item_dlg.attachClickHandlers(std::bind(edit_spec_item_event_filter, _1, _2, std::ref(store_item), which_item), {"okay", "cancel", "clear", "edit-spec", "left", "right"});
	
	put_spec_item_in_dlog(item_dlg, store_item, which_item);
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

static bool edit_vehicles_event_filter(cDialog& me, std::string item_hit, cVehicle* vehicles, size_t nVehicles, size_t& page) {
	
	if(item_hit == "okay") {
		if(save_vehicles(me, vehicles, page))
			me.toast(true);
	} else if(item_hit == "left") {
		if(!save_vehicles(me, vehicles, page)) return true;
		if(page == 0) page = (nVehicles - 1) / 6;
		else page--;
		put_vehicles_in_dlog(me, vehicles, page);
	} else if(item_hit == "right") {
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

static bool edit_add_town_event_filter(cDialog& me, std::string item_hit) {
	if(item_hit == "okay") {
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

static bool save_item_placement(cDialog& me, cScenario::cItemStorage& store_storage, short cur_shortcut) {
	short i;
	
	store_storage.property = dynamic_cast<cLed&>(me["owned"]).getState() != led_off;
	store_storage.ter_type = me["ter"].getTextAsNum();
	if(cre(store_storage.ter_type,
			-1,255,"Terrain Type must be from 0 to 255 (or -1 for No Shortcut).","",&me)) return false;
	for(i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		store_storage.item_num[i] = me["item" + id].getTextAsNum();
		if(cre(store_storage.item_num[i],
			   -1,399,"All item numbers must be from 0 to 399 (or -1 for No Item).","",&me)) return false;
		store_storage.item_odds[i] = me["odds" + id].getTextAsNum();
		if(cre(store_storage.item_odds[i],
			   0,100,"All item chances must bve from 0 to 100.","",&me)) return false;
	}
	scenario.storage_shortcuts[cur_shortcut] = store_storage;
	return true;
}

static void put_item_placement_in_dlog(cDialog& me, const cScenario::cItemStorage& store_storage, short cur_shortcut) {
	short i;
	
	me["num"].setTextToNum(cur_shortcut);
	dynamic_cast<cLed&>(me["owned"]).setState(store_storage.property ? led_red : led_off);
	me["ter"].setTextToNum(store_storage.ter_type);
	for(i = 0; i < 10; i++) {
		std::string id = std::to_string(i + 1);
		me["item" + id].setTextToNum(store_storage.item_num[i]);
		me["odds" + id].setTextToNum(store_storage.item_odds[i]);
	}
}

static bool edit_item_placement_event_filter(cDialog& me, std::string item_hit, cScenario::cItemStorage& store_storage, short& cur_shortcut) {
	short i;
	
	if(item_hit == "okay") {
		if(save_item_placement(me, store_storage, cur_shortcut))
			me.toast(true);
	} else if(item_hit == "cancel") {
		me.toast(false);
	} else if(item_hit == "left") {
		if(!save_item_placement(me, store_storage, cur_shortcut)) return true;
		cur_shortcut--;
		if(cur_shortcut < 0) cur_shortcut = 9;
		store_storage = scenario.storage_shortcuts[cur_shortcut];
		put_item_placement_in_dlog(me, store_storage, cur_shortcut);
	} else if(item_hit == "right") {
		if(!save_item_placement(me, store_storage, cur_shortcut)) return true;
		cur_shortcut++;
		if(cur_shortcut > 9) cur_shortcut = 0;
		store_storage = scenario.storage_shortcuts[cur_shortcut];
		put_item_placement_in_dlog(me, store_storage, cur_shortcut);
	} else if(item_hit == "choose-ter") {
		i = me["ter"].getTextAsNum();
		store_storage.ter_type = i;
		i = choose_text(STRT_TER,i,&me,"Which Terrain?");
		if(i >= 0){
			me["ter"].setTextToNum(i);
			store_storage.ter_type = i;
		}
	}
	return true;
}

static bool edit_item_placement_select_item(cDialog& me, cScenario::cItemStorage& store_storage, short item_hit) {
	std::string id = "item" + std::to_string(item_hit);
	short i = me[id].getTextAsNum();
	store_storage.item_num[item_hit - 1] = i;
	i = choose_text(STRT_ITEM,i,&me,"Place which item?");
	if(i >= 0) {
		me[id].setTextToNum(i);
		store_storage.item_num[item_hit - 1] = i;
	}
	return true;
}

void edit_item_placement() {
	using namespace std::placeholders;
	cScenario::cItemStorage store_storage = scenario.storage_shortcuts[0];
	short cur_shortcut = 0;
	
	cDialog shortcut_dlg("edit-item-shortcut");
	shortcut_dlg.attachClickHandlers(std::bind(edit_item_placement_event_filter, _1, _2, std::ref(store_storage), std::ref(cur_shortcut)), {"okay", "cancel", "left", "right", "choose-ter"});
	for(int i = 0; i < 10; i++) {
		std::string id = "choose-item" + std::to_string(i + 1);
		shortcut_dlg[id].attachClickHandler(std::bind(edit_item_placement_select_item, _1, std::ref(store_storage), i + 1));
	}
	
	put_item_placement_in_dlog(shortcut_dlg, store_storage, cur_shortcut);
	
	shortcut_dlg.run();
}

static bool save_scen_details(cDialog& me) {
	short i;
	
	{
		cLedGroup& difficulty = dynamic_cast<cLedGroup&>(me["difficulty"]);
		scenario.difficulty = difficulty.getSelected()[3] - '1';
	}{
		cLedGroup& rating = dynamic_cast<cLedGroup&>(me["rating"]);
		scenario.rating = rating.getSelected()[4] - '1';
	}
	for(i = 0; i < 3; i++)
		scenario.format.ver[i] = me["ver" + std::to_string(i + 1)].getTextAsNum();
	scenario.who_wrote[0] = me["who1"].getText().substr(0, 60);
	scenario.who_wrote[1] = me["who2"].getText().substr(0, 60);
	scenario.contact_info = me["contact"].getText().substr(0, 256);
	return true;
}

static void put_scen_details_in_dlog(cDialog& me) {
	dynamic_cast<cLedGroup&>(me["difficulty"]).setSelected("lvl" + std::to_string(scenario.difficulty + 1));
	dynamic_cast<cLedGroup&>(me["rating"]).setSelected("rate" + std::to_string(scenario.rating + 1));
	for(int i = 0; i < 3; i++)
		me["ver" + std::to_string(i + 1)].setTextToNum(scenario.format.ver[i]);
	me["who1"].setText(scenario.who_wrote[0]);
	me["who2"].setText(scenario.who_wrote[1]);
	me["contact"].setText(scenario.contact_info);
}

static bool edit_scen_details_event_filter(cDialog& me, std::string, eKeyMod) {
	if(save_scen_details(me))
		me.toast(true);
	return true;
}

void edit_scen_details() {
	cDialog info_dlg("edit-scenario-details");
	info_dlg["okay"].attachClickHandler(edit_scen_details_event_filter);
	
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
