#include <cstdio>
#include <cstring>
#include <functional>
#include <boost/lexical_cast.hpp>
#include "scen.global.h"
#include "classes.h"
#include "graphtool.h"
#include "scen.graphics.h"
//#include "itemlist.h"
#include "scen.core.h"
#include "scen.keydlgs.h"
#include "scen.townout.h"
#include "scen.fileio.h"
#include "scen.actions.h"
#include "dialog.h"
#include "dlogutil.h"
#include "fileio.h"
#include "field.h"
#include "restypes.hpp"

extern std::string get_str(std::string list, short j);
extern short cen_x, cen_y,/* overall_mode,*/cur_town;
extern bool mouse_button_held;
extern short cur_viewing_mode;
extern cTown* town;
//extern short town_type;  // 0 - big 1 - ave 2 - small
extern short max_dim[3];
extern short mode_count,to_create;
extern ter_num_t template_terrain[64][64];
extern cScenario scenario;
extern cSpecial null_spec_node;
extern cSpeech null_talk_node;
//extern piles_of_stuff_dumping_type *data_store;
extern location cur_out;
extern short start_volume, start_dir;

short spec_item_spec,store_horse_page,store_boat_page ;
short cur_shortcut;

const char *item_types[] = {"No Item","1-Handed weapon","2-Handed weapon","Gold","Bow","Arrows","Thrown missile",
	"Potion/Magic Item","Scroll/Magic Item","Wand","Tool","Food","Shield","Armor","Helm",
	"Gloves","Shield","Boots","Ring","Necklace",
"Weapon Poison","Non-Use Object","Pants","Crossbow","Bolts","Missile (no ammo)","Unused","Unused"};

void init_scenario() {
	short i;
	rectangle dummy_rect;
	std::string temp_str;
	cVehicle null_boat;// = {{0,0},{0,0},{0,0},-1,false,false};
	cVehicle null_horse;// = {{0,0},{0,0},{0,0},-1,false,false};
	cScenario::cItemStorage null_item_s;// ={-1,{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},{0,0,0,0,0,0,0,0,0,0},0};
	short j;
	
	scenario.format.ver[0] = 1;
	scenario.format.ver[1] = 0;
	scenario.format.ver[2] = 0;
	scenario.format.min_run_ver = 1;
	scenario.format.prog_make_ver[0] = 1;
	scenario.format.prog_make_ver[1] = 0;
	scenario.format.prog_make_ver[2] = 0;
	scenario.num_towns = 1;
	scenario.out_width = 1;
	scenario.out_height = 1;
	scenario.difficulty = 0;
	scenario.intro_pic = 0;
	scenario.default_ground = 1;
	for (i = 0; i < 200; i++) {
		scenario.town_size[i] = 1;
		scenario.town_hidden[i] = 0;
		scenario.town_data_size[i][0] = 0;
		scenario.town_data_size[i][1] = 0;
		scenario.town_data_size[i][2] = 0;
		scenario.town_data_size[i][3] = 0;
		scenario.town_data_size[i][4] = 0;
	}
	scenario.intro_mess_len = 0;
	scenario.where_start.x = 24;
	scenario.where_start.y = 24;
	scenario.out_sec_start.x = 0;
	scenario.out_sec_start.y = 0;
	scenario.out_start = scenario.where_start;
	scenario.which_town_start = 0;
	for (i = 0; i < 10; i++) {
		scenario.town_to_add_to[i] = -1;
		scenario.flag_to_add_to_town[i][0] = 0;
		scenario.flag_to_add_to_town[i][1] = 0;
	}
	for (i = 0; i < 100; i++) {
		scenario.out_data_size[i][0] = 0;
		scenario.out_data_size[i][1] = 0;
	}
	for (i = 0; i < 3; i++) {
		scenario.store_item_rects[i] = dummy_rect;
		scenario.store_item_towns[i] = -1;
	}
	for (i = 0; i < 50; i++) {
		scenario.special_items[i] = 0;
		scenario.special_item_special[i] = -1;
	}
	scenario.rating = 0;
	scenario.uses_custom_graphics = 0;
	for (i = 0; i < 256; i++) {
		scenario.scen_monsters[i] = cMonster();
	}
	for (i = 0; i < 30; i++) {
		scenario.boats[i] = null_boat;
		scenario.horses[i] = null_horse;
	}
	for (i = 0; i < 256; i++) {
		scenario.ter_types[i] = cTerrain();
		
		scenario.scen_specials[i] = null_spec_node;
	}
	for (i = 0; i < 20; i++) {
		scenario.scenario_timer_times[i] = 0;
		scenario.scenario_timer_specs[i] = -1;
	}
	for (i = 0; i < 10; i++) {
		scenario.storage_shortcuts[i] = null_item_s;
	}
	scenario.last_out_edited.x = 0;
	scenario.last_out_edited.y = 0;
	scenario.last_town_edited = 0;
	for (i = 0; i < 400; i++) {
		scenario.scen_items[i] = cItemRec();
	}
	for (i = 0; i < 270; i++) {
		temp_str = get_str("scen-default",i + 1);
		sprintf((char *)scenario.scen_strs(i), "%s", temp_str.c_str());
		scenario.scen_str_len[i] = strlen(scenario.scen_strs(i));
	}
}	

bool save_ter_info(cDialog& me, cTerrain& store_ter) {
	char str[256];
	short i;
	
	store_ter.picture = me["pict"].getTextAsNum();
	// TODO: Should somehow verify the pict number is valid
	
	std::string blockage = dynamic_cast<cLedGroup&>(me["blockage"]).getSelected();
	if(blockage == "clear") store_ter.blockage = 0;
	else if(blockage == "curtain") store_ter.blockage = 1;
	else if(blockage == "special") store_ter.blockage = 2;
	else if(blockage == "window") store_ter.blockage = 3;
	else if(blockage == "obstructed") store_ter.blockage = 4;
	else if(blockage == "opaque") store_ter.blockage = 5;
	store_ter.special = (eTerSpec) boost::lexical_cast<short>(dynamic_cast<cLedGroup&>(me["prop"]).getSelected().substr(4));
	/*
	i = CDGN(813,6);
	if ((store_ter.special < 2) || (store_ter.special > 6)) {
		if (cre(i,0,256,"First special flag must be from 0 to 255.","",813) == true) return false;
	}
	else if (store_ter.special == TER_SPEC_DAMAGING) {
		if (cre(i,0,256,"First special flag must be from 0 to 100.","",813) == true) return false;
	}
	else if (store_ter.special == TER_SPEC_DANGEROUS) {
		if (cre(i,0,256,"First special flag must be from 0 to 8.","",813) == true) return false;
	}
	 */
	if(store_ter.special == TER_SPEC_NONE)
		store_ter.flag1.s = me["flag1"].getTextAsNum();
	else store_ter.flag1.u = me["flag1"].getTextAsNum();
	if(false) // flag2 is never signed, apparently; but that could change?
		store_ter.flag2.s = me["flag2"].getTextAsNum();
	else store_ter.flag2.u = me["flag2"].getTextAsNum();
	if(store_ter.special == TER_SPEC_CALL_SPECIAL || store_ter.special == TER_SPEC_CALL_SPECIAL_WHEN_USED)
		store_ter.flag3.s = me["flag3"].getTextAsNum();
	else store_ter.flag3.u = me["flag3"].getTextAsNum();
	
	/*
	if (store_ter.special == TER_SPEC_TOWN_ENTRANCE) {
		if (cre(i,0,256,"Second special flag must be from 0 to 200.","",813) == true) return false;
	}
	else if ((store_ter.special == TER_SPEC_DAMAGING) || (store_ter.special == TER_SPEC_DANGEROUS)) {
		if (cre(i,0,256,"Second special flag must be from 0 to 100.","",813) == true) return false;
	}
	*/
	
	/*
	if (cre(i,0,255,"Transform To What must be from 0 to 255.","",813) == true) return false;
	 */
	store_ter.trans_to_what = me["trans"].getTextAsNum();
	store_ter.fly_over = dynamic_cast<cLed&>(me["flight"]).getState() == led_red;
	store_ter.boat_over = dynamic_cast<cLed&>(me["boat"]).getState() == led_red;
	store_ter.block_horse = dynamic_cast<cLed&>(me["horse"]).getState();
	store_ter.light_radius = me["light"].getTextAsNum();
	/*
	if (cre(store_ter.light_radius,0,8,"Light radius must be from 0 to 8.","",813) == true) return false;
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

bool pick_picture(ePicType type, cDialog& parent, std::string result_fld, std::string pic_fld, pic_num_t modifier){
	pic_num_t cur_sel = 0;
	if(result_fld != ""){
		cControl& fld_ctrl = parent[result_fld];
		cur_sel = fld_ctrl.getTextAsNum();
		cur_sel -= modifier;
	}else if(pic_fld != ""){
		cPict& pic_ctrl = dynamic_cast<cPict&>(parent[pic_fld]);
		if(pic_ctrl.getPicType() == type)
			cur_sel = pic_ctrl.getPicNum();
	}
	pic_num_t pic = choose_graphic(cur_sel, type, &parent);
	if(pic < NO_PIC){
		if(result_fld != ""){
			cTextField& fld_ctrl = dynamic_cast<cTextField&>(parent[result_fld]);
			fld_ctrl.setTextToNum(pic + modifier);
		}
		if(pic_fld != ""){
			cPict& pic_ctrl = dynamic_cast<cPict&>(parent[pic_fld]);
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
	size_t result = str_dlg.show(strings[cur_sel]);
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

bool show_help(std::string from_file, cDialog& parent, pic_num_t pic){
	StringRsrc strings = *ResMgr::get<StringRsrc>(from_file);
	cThreeChoice help(strings,basic_buttons[63],pic,PIC_DLOG,&parent);
	help.show();
	return true;
}

bool fill_ter_flag_info(cDialog& me, std::string id, bool dummy){
	eTerSpec prop;
	cLedGroup& led_ctrl = dynamic_cast<cLedGroup&>(me[id]);
	std::istringstream sel(led_ctrl.getSelected().substr(4));
	sel >> prop;
	me["flag1text"].setText(get_str("ter-flag1",prop + 1));
	me["flag2text"].setText(get_str("ter-flag2",prop + 1));
	me["flag3text"].setText(get_str("ter-flag3",prop + 1));
	me["pickflag1"].hide();
	me["pickflag2"].hide();
	me["pickflag3"].hide();
	switch(prop) {
		case TER_SPEC_NONE:
			me["pickflag1"].hide(); // TODO: Could have a pick graphic dialog for the editor icon, but that requires adding a new graphic type
			break;
	}
	// TODO: Click handlers for the "choose" buttons as necessary, plus hide/show them as needed
	return true;
}

void fill_ter_info(cDialog& me, short ter){
	cTerrain& ter_type = scenario.ter_types[ter];
	{
		cPict& pic_ctrl = dynamic_cast<cPict&>(me["graphic"]);
		pic_num_t pic = ter_type.picture;
		if(pic < 400)
			pic_ctrl.setPict(pic, PIC_TER);
		else if(pic < 1000)
			pic_ctrl.setPict(pic % 400, PIC_TER_ANIM);
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
	me["trim"].setTextToNum(ter_type.trim_type);
	std::string propid = "prop" + std::to_string(ter_type.special);
	dynamic_cast<cLedGroup&>(me["prop"]).setSelected(propid);
	fill_ter_flag_info(me, "prop", false);
	{
		cLedGroup& led_ctrl = dynamic_cast<cLedGroup&>(me["blockage"]);
		switch(ter_type.blockage){
			case 0:
				led_ctrl.setSelected("clear");
				break;
			case 1:
				led_ctrl.setSelected("curtain");
				break;
			case 2:
				led_ctrl.setSelected("special");
				break;
			case 3:
				led_ctrl.setSelected("window");
				break;
			case 4:
				led_ctrl.setSelected("obstructed");
				break;
			case 5:
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
	if(ter_type.special == TER_SPEC_NONE)
		me["flag1"].setTextToNum(ter_type.flag1.s);
	else me["flag1"].setTextToNum(ter_type.flag1.u);
	if(false) // flag2 is never signed, apparently; but that could change?
		me["flag2"].setTextToNum(ter_type.flag2.s);
	else me["flag2"].setTextToNum(ter_type.flag2.u);
	if(ter_type.special == TER_SPEC_CALL_SPECIAL || ter_type.special == TER_SPEC_CALL_SPECIAL_WHEN_USED)
		me["flag3"].setTextToNum(ter_type.flag3.s);
	else me["flag3"].setTextToNum(ter_type.flag3.u);
	me["arena"].setTextToNum(ter_type.combat_arena);
}

bool finish_editing_ter(cDialog& me, std::string id, ter_num_t& which_ter) {
	if(!save_ter_info(me, scenario.ter_types[which_ter])) return true;
	
	if(id == "done") me.toast();
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
	cDialog ter_dlg("edit-terrain.xml");
	// Attach handlers
	ter_dlg["pict"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,2999,"terrain graphic"));
	ter_dlg["pickpict"].attachClickHandler(std::bind(pick_picture,PIC_TER,_1,"pict","graphic",0));
	ter_dlg["pickanim"].attachClickHandler(std::bind(pick_picture,PIC_TER_ANIM,_1,"pict","graphic",400));
	ter_dlg["light"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,255,"light radius"));
	ter_dlg["trans"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,65535,"\"transform to what?\""));
	ter_dlg["ground"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,255,"ground type"));
	ter_dlg["trimter"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,255,"trim terrain"));
	ter_dlg["trim"].attachFocusHandler(std::bind(check_range,_1,_2,_3,0,18,"trim type"));
	ter_dlg["prop"].attachFocusHandler(fill_ter_flag_info);
	ter_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &ter_dlg));
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

void put_monst_info_in_dlog(cDialog& me, m_num_t which_monst) {
	char str[256];
	cMonster& store_monst = scenario.scen_monsters[which_monst];
	
	if (store_monst.picture_num < 1000)
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
	sprintf((char *) str,"Width = %d",store_monst.x_width);
	me["w"].setText(str);
	sprintf((char *) str,"Height = %d",store_monst.y_width);
	me["h"].setText(str);
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
	
	me["type"].setText(get_str("monster-abilities",150 + store_monst.m_type));
	me["type1"].setText(get_str("monster-abilities",130 + store_monst.a[0].type));
	me["type2"].setText(get_str("monster-abilities",130 + store_monst.a[1].type));
	// TODO: Attack 3 type
//	me["type3"].setText(get_str("monster-abilities",130 + store_monst.a[2].type));
}

bool check_monst_pic(cDialog& me, std::string id, bool losing, cMonster& store_monst) {
	if(!losing) return true;
	if(check_range(me, id, losing, 0, 4999, "Monster pic")) {
		// later check pic num for error, and assign widths if custom
		if (store_monst.picture_num >= 1000) {
			if ((store_monst.picture_num >= 1000) && (store_monst.picture_num < 2000)) {
				store_monst.x_width = 1;
				store_monst.y_width = 1;
			}
			if ((store_monst.picture_num >= 2000) && (store_monst.picture_num < 3000)) {
				store_monst.x_width = 2;
				store_monst.y_width = 1;
			}
			if ((store_monst.picture_num >= 3000) && (store_monst.picture_num < 4000)) {
				store_monst.x_width = 1;
				store_monst.y_width = 2;
			}
			if ((store_monst.picture_num >= 4000) && (store_monst.picture_num < 5000)) {
				store_monst.x_width = 2;
				store_monst.y_width = 2;
			}
		}
		else {
			// TODO: Update this with new value if more monster pictures are added later
			if (cre(store_monst.picture_num,0,200,"Non-customized monster pic must be from 0 to 200.","",&me) > 0) return false;
			store_monst.x_width = m_pic_index[store_monst.picture_num].x;
			store_monst.y_width = m_pic_index[store_monst.picture_num].y;
			
		}
	}
	return false;
}

bool save_monst_info(cDialog& me, cMonster& store_monst) {
	char str[256];
	short i;
	
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

static bool edit_monst_type_event_filter(cDialog& me,std::string item_hit,cMonster& store_monst,short which_monst) {
	short i;
	cMonster temp_monst;
	
	if(item_hit == "okay") {
		if(save_monst_info(me,store_monst)) {
			scenario.scen_monsters[which_monst] = store_monst;
			me.toast();
		}
	} else if(item_hit == "abils") {
			if(!save_monst_info(me,store_monst)) return false;
			temp_monst = edit_monst_abil(store_monst,814);
			if (temp_monst.level < 255)
				store_monst = temp_monst;
			put_monst_info_in_dlog(me,which_monst);
	} else if(item_hit == "left") {
			if(!save_monst_info(me,store_monst)) return false;
			scenario.scen_monsters[which_monst] = store_monst;
			which_monst--;
		// TODO: Use size() once scen_monsters becomes a vector
			if(which_monst < 1) which_monst = 255;
			store_monst = scenario.scen_monsters[which_monst];
			put_monst_info_in_dlog(me,which_monst);
	} else if(item_hit == "right") {
			if(!save_monst_info(me,store_monst)) return false;
			scenario.scen_monsters[which_monst] = store_monst;
			which_monst++;
			if(which_monst > 255) which_monst = 1;
			store_monst = scenario.scen_monsters[which_monst];
			put_monst_info_in_dlog(me,which_monst);
	} else if(item_hit == "picktype") {
			if(!save_monst_info(me,store_monst)) return false;
			i = choose_text_res("monster-abilities",150,164,store_monst.m_type + 150,&me,"Choose Monster Type:");
			if (i >= 0) {
				i -= 150;
				store_monst.m_type = (eMonsterType) i;
				put_monst_info_in_dlog(me,which_monst);
			}
	} else if(item_hit == "picktype1") {
			if(!save_monst_info(me,store_monst)) return false;
			i = choose_text_res("monster-abilities",130,139,store_monst.a[0].type + 130,&me,"Choose Attack 1 Type:");
			if (i >= 0) {
				store_monst.a[0].type = i - 130;
				put_monst_info_in_dlog(me,which_monst);
			}
	} else if(item_hit == "picktype2") {
			if(!save_monst_info(me,store_monst)) return false;
			i = choose_text_res("monster-abilities",130,139,store_monst.a[1].type + 130,&me,"Choose Attack 2 & 3 Type:");
			if (i >= 0) {
				store_monst.a[1].type = store_monst.a[2].type = i - 130;
				put_monst_info_in_dlog(me,which_monst);
			}
	} else if(item_hit == "picktype3") {
		if(!save_monst_info(me,store_monst)) return false;
		i = choose_text_res("monster-abilities",130,139,store_monst.a[2].type + 130,&me,"Choose Attack 3 Type:");
		if (i >= 0) {
			store_monst.a[2].type = i - 130;
			put_monst_info_in_dlog(me,which_monst);
		}
	}
	return true;
}

short edit_monst_type(short which_monst) {
	using namespace std::placeholders;
	cMonster store_monst = scenario.scen_monsters[which_monst];
	
	cDialog monst_dlg("edit-monster.xml");
	monst_dlg["pickicon"].attachClickHandler(std::bind(pick_picture,PIC_MONST,_1,"pic","icon",0));
	monst_dlg["picktalk"].attachClickHandler(std::bind(pick_picture,PIC_TALK,_1,"talk","",0));
	monst_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &monst_dlg));
	monst_dlg["pic"].attachFocusHandler(std::bind(check_monst_pic, _1, _2, _3, std::ref(store_monst)));
	monst_dlg["level"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 40, "level"));
	monst_dlg["health"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 2500, "health"));
	monst_dlg["armor"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 50, "armor"));
	monst_dlg["skill"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 40, "skill"));
	monst_dlg["speed"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 2, 12, "speed"));
	monst_dlg["mage"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 7, "magic spells"));
	monst_dlg["priest"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 7, "priest spells"));
	monst_dlg["treas"].attachFocusHandler(std::bind(check_range, _1, _2, _3, 0, 4, "treasure"));
	monst_dlg.attachFocusHandlers(std::bind(check_range_msg,_1,_2,_3,0,20,"attack number of dice","0 means no attack"),{"dice1","dice2","dice3"});
	monst_dlg.attachFocusHandlers(std::bind(check_range,_1,_2,_3,1,50,"attack damage per die"),{"sides1","sides2","sides3"});
	monst_dlg.attachClickHandlers(std::bind(edit_monst_type_event_filter,_1,_2,std::ref(store_monst),which_monst),{"okay","abils","left","right","picktype","picktype1","picktype2"});
	
	put_monst_info_in_dlog(monst_dlg, which_monst);
	
	monst_dlg.run();
	return 0;
}

void put_monst_abils_in_dlog() {
#if 0
	char str[256];
	short i;
	
	cdsin(815,24,store_which_monst);
	
	
	CDSN(815,2,store_monst2.poison);
	CDSN(815,3,store_monst2.breath);
	
	get_str(str,20,store_monst2.spec_skill + 1);
	csit(815,30,(char *) str);
	get_str(str,20,store_monst2.radiate_1 + 50);
	csit(815,33,(char *) str);
	get_str(str,20,store_monst2.radiate_1 + 80);
	csit(815,36,(char *) str);
	CDSN(815,4,store_monst2.radiate_2);
	CDSN(815,5,store_monst2.corpse_item);
	CDSN(815,6,store_monst2.corpse_item_chance);
 	cd_set_led_range(815,9,12,store_monst2.breath_type);
 	cd_set_led_range(815,41,44,store_monst2.summon_type);
 	for (i = 0; i < 8; i++)
 		cd_set_led(815,13 + i,0);
 	if (store_monst2.immunities & 1 ) cd_set_led(815,13,1);
 	if (store_monst2.immunities & 2 ) cd_set_led(815,14,1);
 	if (store_monst2.immunities & 4 ) cd_set_led(815,15,1);
 	if (store_monst2.immunities & 8 ) cd_set_led(815,16,1);
 	if (store_monst2.immunities & 16 ) cd_set_led(815,17,1);
 	if (store_monst2.immunities & 32 ) cd_set_led(815,18,1);
 	if (store_monst2.immunities & 64 ) cd_set_led(815,19,1);
 	if (store_monst2.immunities & 128 ) cd_set_led(815,20,1);
#endif
}

bool save_monst_abils() {
#if 0
 	store_monst2.poison = CDGN(815,2);
 	if (cre(store_monst2.poison,0,8,"Poison must be from 0 to 8.","",815) > 0) return false;
	store_monst2.breath = CDGN(815,3);
  	if (cre(store_monst2.poison,0,40,"Breath Damage must be from 0 to 4.","",815) > 0) return false;
	store_monst2.breath_type = cd_get_led_range(815,9,12);
	store_monst2.summon_type = cd_get_led_range(815,41,44);
 	store_monst2.radiate_2 = CDGN(815,4);
 	if ((store_monst2.radiate_1 >= 1) && (store_monst2.radiate_1 <= 6))
   		if (cre(store_monst2.radiate_2,0,100,"Radiation Chance must be from 0 to 100.","",815) > 0) return false;
 	if ((store_monst2.radiate_1 >= 10) && (store_monst2.radiate_1 <= 12))
   		if (cre(store_monst2.radiate_2,0,255,"Summoned Monster must be from 0 to 255.","",815) > 0) return false;
	
 	store_monst2.corpse_item = CDGN(815,5);
  	if (cre(store_monst2.corpse_item,-1,399,"Item To Drop must be from 0 to 399 (or -1 for no item).","",815) > 0) return false;
	store_monst2.corpse_item_chance = CDGN(815,6);
  	if (cre(store_monst2.corpse_item_chance,-1,100,"Dropping Chance must be from 0 to 100 (or -1 for no item).","",815) > 0) return false;
	store_monst2.immunities = 0;
	if (cd_get_led(815,13) == 1) store_monst2.immunities = store_monst2.immunities | 1;
	if (cd_get_led(815,14) == 1) store_monst2.immunities = store_monst2.immunities | 2;
	if (cd_get_led(815,15) == 1) store_monst2.immunities = store_monst2.immunities | 4;
	if (cd_get_led(815,16) == 1) store_monst2.immunities = store_monst2.immunities | 8;
	if (cd_get_led(815,17) == 1) store_monst2.immunities = store_monst2.immunities | 16;
	if (cd_get_led(815,18) == 1) store_monst2.immunities = store_monst2.immunities | 32;
	if (cd_get_led(815,19) == 1) store_monst2.immunities = store_monst2.immunities | 64;
	if (cd_get_led(815,20) == 1) store_monst2.immunities = store_monst2.immunities | 128;
#endif
	return true;
}

void edit_monst_abil_event_filter (short item_hit) {
#if 0
	short i;
	
	switch (item_hit) {
		case 8:
			store_monst2.level = 255;
			toast_dialog();
			break;
		case 7:
			if (save_monst_abils() == true)
				toast_dialog();
			break;
		case 31: // abils
			if (save_monst_abils() == false) break;
			i = choose_text_res(20,1,38,store_monst2.spec_skill + 1,815,"Choose Monster Ability:");
			if (i >= 0) {
				store_monst2.spec_skill = i - 1;
				put_monst_abils_in_dlog();
			}	
			break;
		case 34: // radiate
			if (save_monst_abils() == false) break;
			i = choose_text_res(20,50,65,store_monst2.radiate_1 + 50,815,"Choose Radiation Ability:");
			if (i >= 0) {
				store_monst2.radiate_1 = i - 50;
				put_monst_abils_in_dlog();
			}	
			break;
		default:
			cd_hit_led_range(815,9,12,item_hit);
			cd_hit_led_range(815,41,44,item_hit);
			for (i = 13; i < 21; i++)
				cd_flip_led(815,i,item_hit);
			break;
			
	}
#endif
}

cMonster edit_monst_abil(cMonster starting_record,short parent_num) {
	cMonster store_monst2 = starting_record;
#if 0
	// ignore parent in Mac version
	short item_hit;
	
	//store_which_monst = which_monst;
	store_monst2 = starting_record;
	//make_cursor_sword();
	
	cd_create_dialog_parent_num(815,parent_num);
	
	put_monst_abils_in_dlog();
	
	cd_add_label(815,9,"Fire",35);
	cd_add_label(815,10,"Cold",35);
	cd_add_label(815,11,"Electricity",35);
	cd_add_label(815,12,"Darkness",35);
	cd_add_label(815,41,"No Summon",35);
	cd_add_label(815,42,"Type 1",35);
	cd_add_label(815,43,"Type 2",35);
	cd_add_label(815,44,"Type 3",35);
	
	cd_add_label(815,13,"Resist Magic",55);
	cd_add_label(815,15,"Resist Fire",55);
	cd_add_label(815,17,"Resist Cold",55);
	cd_add_label(815,19,"Resist Poison",55);
	cd_add_label(815,14,"Immune To Magic",55);
	cd_add_label(815,16,"Immune To Fire",55);
	cd_add_label(815,18,"Immune To Cold",55);
	cd_add_label(815,20,"Immune To Poison",55);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(815);
#endif
	return store_monst2;
}

void put_item_info_in_dlog() {
#if 0
	cdsin(818,52,store_which_item);
	CDST(818,2,store_item.full_name.c_str());
	CDST(818,3,store_item.name.c_str());
	if (store_item.graphic_num >= 1000) // was 150
		csp(818,49,store_item.graphic_num % 1000,PICT_CUSTOM + PICT_ITEM);
	else csp(818,49,/*1800 + */store_item.graphic_num,PICT_ITEM);
	CDSN(818,4,store_item.graphic_num);
	cd_set_led_range(818,18,45,store_item.variety);
	CDSN(818,5,store_item.item_level);
	CDSN(818,6,store_item.awkward);
	CDSN(818,7,store_item.bonus);
	CDSN(818,8,store_item.protection);
	CDSN(818,9,store_item.charges);
	CDSN(818,10,store_item.type_flag);
	CDSN(818,11,store_item.value);
	CDSN(818,12,store_item.weight);
	CDSN(818,13,store_item.special_class);
	cd_set_led_range(818,46,48,store_item.type - 1);
#endif
}

bool save_item_info() {
#if 0
	char str[256];
	
	CDGT(818,2,(char *) str);
	str[24] = 0;
	store_item.full_name = (char*)str;
	CDGT(818,3,(char *) str);
	str[14] = 0;
	store_item.name = (char*)str;
	store_item.graphic_num = CDGN(818,4);
	
	store_item.variety = (eItemType) cd_get_led_range(818,18,45);
	store_item.type = (eWeapType) (cd_get_led_range(818,46,48) + 1);
	
	store_item.item_level = CDGN(818,5);
	if (cre(store_item.item_level,0,50,"Item Level must be from 0 to 50.","",818) > 0) return false;
	if (((store_item.variety == 3) || (store_item.variety == 11)) && (store_item.item_level == 0))
		store_item.item_level = 1;
	
	store_item.awkward = CDGN(818,6);
	if (cre(store_item.awkward,0,20,"Awkward must be from 0 to 20.","",818) > 0) return false;
	store_item.bonus = CDGN(818,7);
	if (cre(store_item.bonus,0,60,"Bonus must be from 0 to 60.","",818) > 0) return false;
	store_item.protection = CDGN(818,8);
	if (cre(store_item.protection,-10,20,"Protection must be from -10 to 20.","",818) > 0) return false;
	store_item.charges = CDGN(818,9);
	if (cre(store_item.charges,0,100,"Charges must be from 0 to 100.","",818) > 0) return false;
	store_item.type_flag = CDGN(818,10);
	if (cre(store_item.type_flag,0,1000,"Type Flag must be from 0 to 1000.","",818) > 0) return false;
	store_item.value  = CDGN(818,11);
	if (cre(store_item.value,0,10000,"Value must be from 0 to 10000.","",818) > 0) return false;
	store_item.weight = CDGN(818,12);
	if (cre(store_item.weight,0,250,"Weight must be from 0 to 250.","",818) > 0) return false;
	store_item.special_class = CDGN(818,13);
	if (cre(store_item.special_class,0,100,"Special Class must be from 0 to 100.","",818) > 0) return false;
	
	if ((store_item.type_flag > 0) && (store_item.charges == 0)) {
		give_error("If the Type Flag is greater than 0, the Charges must also be greater than 0.","",818);
		return false;
	}
	if (store_item.variety > 25) {
		give_error("The Unused item varieties are reserved for later expansions, and can't be used now.","",818);
		return false;
	}
	if ((store_item.ability >= 70) && (store_item.ability < 170) && (store_item.charges == 0)) {
		give_error("An item with the special ability selected must have at least 1 charge.","",818);
		return false;
	}
	scenario.scen_items[store_which_item] = store_item ;
#endif
	return true;
}

void edit_item_type_event_filter (short item_hit) {
#if 0
	short i;
	cItemRec temp_item;
	
	switch (item_hit) {
		case 15:
			toast_dialog();
			break;
		case 14:
			if (save_item_info() == true)
				toast_dialog();
			break;
			
		case 16:
			if (save_item_info() == false) break;
			store_which_item--;
			if (store_which_item < 0) store_which_item = 399;
			store_item = scenario.scen_items[store_which_item];
			put_item_info_in_dlog();
			break;
		case 17:
			if (save_item_info() == false) break;
			store_which_item++;
			if (store_which_item > 399) store_which_item = 0;
			store_item = scenario.scen_items[store_which_item];
			put_item_info_in_dlog();
			break;
		case 56:
			if (save_item_info() == false) break;
			i = choose_graphic(/*1800,1922*/0,PICT_N_ITEM,store_item.graphic_num/* + 1800*/,PICT_ITEM,818);
			if (i < 0) break;
			store_item.graphic_num = i;
			put_item_info_in_dlog();
			break;
		case 69:
			if (store_item.variety == 0) {
				give_error("You must give the item a type (weapon, armor, etc.) before you can choose its abilities.","",818);
				break;
			}
			if ((store_item.variety == 3) || (store_item.variety == 11)) {
				give_error("Gold and Food cannot be given special abilities.","",818);
				break;
			}
			temp_item = edit_item_abil(store_item,818);
			if (temp_item.variety != 0)
				store_item = temp_item;
			break;
		default:
			cd_hit_led_range(818,18,45,item_hit);
			store_item.variety = (eItemType) cd_get_led_range(818,18,45);
			if ((store_item.variety == ITEM_TYPE_ONE_HANDED) || (store_item.variety == ITEM_TYPE_TWO_HANDED))
				store_item.type = ITEM_EDGED;
			else store_item.type = ITEM_NOT_MELEE;
			cd_hit_led_range(818,46,48,item_hit);
			break;
			
	}
#endif
}

short edit_item_type(short which_item) {
#if 0
	// ignore parent in Mac version
	short item_hit,i;
	
	store_which_item = which_item;
	store_item = scenario.scen_items[store_which_item];
	//make_cursor_sword();
	
	cd_create_dialog_parent_num(818,0);
	
	put_item_info_in_dlog();
	
	for (i = 18; i < 46; i++) 
		cd_add_label(818, i,item_types[i - 18],47);
	cd_add_label(818,46,"Edged",23);
	cd_add_label(818,47,"Bashing",23);
	cd_add_label(818,48,"Pole",23);
	cd_attach_key(818,16,0);
	cd_attach_key(818,17,0);
	
	item_hit = cd_run_dialog();
	
	cd_kill_dialog(818);
#endif
	return 0;
}

void put_item_abils_in_dlog() {
#if 0
	char str[256];
	
	cdsin(824,16,store_which_item);
	csit(824,32,store_item2.full_name.c_str());
	csit(824,34,item_types[store_item2.variety]);
	get_str(str,23,store_item2.ability + 1);
	csit(824,19,(char *) str);
	
	cd_set_led_range(824,5,8,store_item2.magic_use_type);
	cd_set_led_range(824,26,30,store_item2.treas_class);
	CDSN(824,2,store_item2.ability_strength);
	if (store_item2.ident)
		cd_set_led(824,9,1);
	else cd_set_led(824,9,0);
	if (store_item2.magic)
		cd_set_led(824,10,1);
	else cd_set_led(824,10,0);
	if (store_item2.cursed)
		cd_set_led(824,11,1);
	else cd_set_led(824,11,0);
	if (store_item2.concealed)
		cd_set_led(824,12,1);
	else cd_set_led(824,12,0);
#endif
}

bool save_item_abils() {
#if 0
	store_item2.magic_use_type = cd_get_led_range(824,5,8);
	store_item2.treas_class = cd_get_led_range(824,26,30);
	store_item2.ability_strength = CDGN(824,2);
	if ((store_item2.ability != 119) && (store_item2.ability != 120)) {
		if (cre(store_item2.ability_strength,0,10,"Ability Strength must always be a number from 0 to 10.","",824) > 0) return false;
	}
	else if (cre(store_item2.ability_strength,0,255,"Ability Strength must be 0 to 255 - the number of the monster to summon.","",824) > 0) return false;
	store_item2.property = store_item2.enchanted = store_item2.contained = false;
	store_item2.ident = cd_get_led(824,9);
	store_item2.magic = cd_get_led(824,10);
	store_item2.cursed = store_item2.unsellable = cd_get_led(824,11);
	store_item2.concealed = cd_get_led(824,12);
#endif
	return true;
}

void edit_item_abil_event_filter (short item_hit) {
#if 0
	short i;
	
	switch (item_hit) {
		case 4:
			store_item2.ability = ITEM_NO_ABILITY;
			toast_dialog();
			break;
		case 3:
			if (save_item_abils() == true)
				toast_dialog();
			break;
		case 17:
			if (save_item_abils() == false) break;
			if (store_item.variety > 2) {
				give_error("You can only give an ability of this sort to a melee weapon.","",824);
				break;
			}
			i = choose_text_res(23,1,15,store_item2.ability + 1,824,"Choose Weapon Ability (inherent)");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		case 35:
			if (save_item_abils() == false) break;
			if ((store_item.variety == 5) || (store_item.variety == 6)|| (store_item.variety == 7) || (store_item.variety == 8) ||
				(store_item.variety == 9) || (store_item.variety == 10) || (store_item.variety == 20) ||
				(store_item.variety == 21) || (store_item.variety == 24)){
				give_error("You can only give an ability of this sort to an non-missile item which can be equipped (like armor, or a ring).","",824);
				break;
			}
			i = choose_text_res(23,31,63,store_item2.ability + 1,824,"Choose General Ability (inherent)");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		case 36:
			if (save_item_abils() == false) break;
			if ((store_item.variety == 5) || (store_item.variety == 6) || (store_item.variety == 24)){
				give_error("You can only give an ability of this sort to an item which isn't a missile.","",824);
				break;
			}
			i = choose_text_res(23,71,95,store_item2.ability + 1,824,"Choose Usable Ability (Not spell)");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		case 39:
			if (save_item_abils() == false) break;
			if ((store_item.variety == 5) || (store_item.variety == 6) || (store_item.variety == 24)){
				give_error("You can only give an ability of this sort to an item which isn't a missile.","",824);
				break;
			}
			i = choose_text_res(23,111,136,store_item2.ability + 1,824,"Choose Usable Ability (Spell)");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		case 38:
			if (save_item_abils() == false) break;
			if (store_item.variety != 21){
				give_error("You can only give an ability of this sort to an item of type Non-Use Object.","",824);
				break;
			}
			i = choose_text_res(23,151,162,store_item2.ability + 1,824,"Choose Reagent Ability");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		case 37:
			if (save_item_abils() == false) break;
			if ((store_item.variety != 5) && (store_item.variety != 6) && (store_item.variety != 24)){
				give_error("You can only give an ability of this sort to an item which isn't a missile.","",824);
				break;
			}
			i = choose_text_res(23,171,177,store_item2.ability + 1,824,"Choose Missile Ability");
			if (i >= 0) store_item2.ability = (eItemAbil) (i - 1);
			else store_item2.ability = ITEM_NO_ABILITY;
			put_item_abils_in_dlog();
			break;
		default:
			cd_hit_led_range(824,26,30,item_hit);
			cd_hit_led_range(824,5,8,item_hit);
			for (i = 9; i < 13; i++)
				cd_flip_led(824,i,item_hit);
			break;
			
	}
#endif
}

cItemRec edit_item_abil(cItemRec starting_record,short parent_num) {
	cItemRec store_item2 = starting_record;
#if 0
	// ignore parent in Mac version
	short item_hit;
	
	//store_which_item = which_item;
	store_item2 = starting_record;
	//make_cursor_sword();
	
	cd_create_dialog_parent_num(824,parent_num);
	
	put_item_abils_in_dlog();
	
	cd_add_label(824,26,"Type 0: Junk, Not left",68);
	cd_add_label(824,27,"Type 1: Lousy, 1 - 20 gp",68);
	cd_add_label(824,28,"Type 2: So-so, 20-200 gp",68);
	cd_add_label(824,29,"Type 3: Good, 200+ gp",68);
	cd_add_label(824,30,"Type 4: Great, 2500+ gp",68);
	
	cd_add_label(824,5,"Help using PC",50);
	cd_add_label(824,6,"Harm using PC",50);
	cd_add_label(824,7,"Help whole party",50);
	cd_add_label(824,8,"Harm whole party",50);
	
	cd_add_label(824,9,"Always identified",50);
	cd_add_label(824,10,"Magical",50);
	cd_add_label(824,11,"Cursed",50);
	cd_add_label(824,12,"Conceal ability",50);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(824);
#endif
	return store_item2;
}

void put_spec_item_in_dlog() {
#if 0
	cdsin(806,19,store_which_spec_item);
	CDST(806,2,scenario.scen_strs(60 + store_which_spec_item * 2));
	CDST(806,3,scenario.scen_strs(60 + store_which_spec_item * 2 + 1));
	CDSN(806,4,scenario.special_item_special[store_which_spec_item]);
	if (scenario.special_items[store_which_spec_item] >= 10)
		cd_set_led(806,15,1);
	else cd_set_led(806,15,0);
	if (scenario.special_items[store_which_spec_item] % 10 > 0)
		cd_set_led(806,17,1);
	else cd_set_led(806,17,0);
#endif
}

bool save_spec_item() {
#if 0
	char str[256];
	
	CDGT(806,2,(char *) str);
	str[25] = 0;
	sprintf(scenario.scen_strs(60 + store_which_spec_item * 2 + 0),"%s",str);
	CDGT(806,3,scenario.scen_strs(60 + store_which_spec_item * 2 + 1));
	spec_item_spec = CDGN(806,4);
	if (cre(scenario.special_item_special[store_which_spec_item],
			-1,255,"Scenario special node called must be from 0 to 255 (or -1 for no special).","",806) > 0) return false;
	scenario.special_item_special[store_which_spec_item] = spec_item_spec;
	scenario.special_items[store_which_spec_item] = 0;
	if (cd_get_led(806,15) == 1)
		scenario.special_items[store_which_spec_item] += 10;
	if (cd_get_led(806,17) == 1)
		scenario.special_items[store_which_spec_item] += 1;
#endif
	return true;
}

void edit_spec_item_event_filter (short spec_item_hit) {
#if 0
	short spec;
	
	switch (spec_item_hit) {
		case 11:
			toast_dialog();
			break;
		case 5:
			if (save_spec_item() == true)
				toast_dialog();
			break;
			
		case 20:
			if (save_spec_item() == false) break;
			store_which_spec_item--;
			if (store_which_spec_item < 0) store_which_spec_item = 49;
			spec_item_spec = scenario.special_item_special[store_which_spec_item];
			put_spec_item_in_dlog();
			break;
		case 21:
			if (save_spec_item() == false) break;
			store_which_spec_item++;
			if (store_which_spec_item > 49) store_which_spec_item = 0;
			spec_item_spec = scenario.special_item_special[store_which_spec_item];
			put_spec_item_in_dlog();
			break;
		case 13: //choose edit
			if (save_spec_item() == false)
				break;
			spec = CDGN(806,4);
			if ((spec < 0) || (spec >= 256)) {
				spec = get_fresh_spec(0);
				if (spec < 0) {
					give_error("You can't create a new special encounter because there are no more free scenario special nodes.",
							   "To free a special node, set its type to No Special and set its Jump To special to -1.",806);
					break;
				}
				CDSN(806,4,spec);
			}	
			edit_spec_enc(spec,0,806);
			if ((spec >= 0) && (spec < 256) && (scenario.scen_specials[spec].pic < 0))
				CDSN(806,4,-1);
			if (save_spec_item() == false)
				break;
			break;
		default:
			cd_flip_led(806,15,spec_item_hit);
			cd_flip_led(806,17,spec_item_hit);
			break;
			
	}
#endif
}

void edit_spec_item(short which_item) {
#if 0
	// ignore parent in Mac version
	short spec_item_hit;
	
	//store_which_spec_item = which_spec_item;
	store_which_spec_item = which_item;
	spec_item_spec = scenario.special_item_special[store_which_spec_item];
	
	cd_create_dialog_parent_num(806,0);
	
	put_spec_item_in_dlog();
	cd_attach_key(806,20,0);
	cd_attach_key(806,21,0);
	cd_activate_item(806,12,0);
	
	spec_item_hit = cd_run_dialog();
	
	cd_kill_dialog(806);
#endif
}

void put_save_rects_in_dlog() {
#if 0
	short i;
	
	for (i = 0; i < 3; i++) {
		CDSN(807,3 + 5 * i,scenario.store_item_rects[i].top);
		CDSN(807,4 + 5 * i,scenario.store_item_rects[i].left);
		CDSN(807,5 + 5 * i,scenario.store_item_rects[i].bottom);
		CDSN(807,6 + 5 * i,scenario.store_item_rects[i].right);
		CDSN(807,2 + 5 * i,scenario.store_item_towns[i]);
		
	}
#endif
}

bool save_save_rects() {
#if 0
	short i;
	
	for (i = 0; i < 3; i++) {
		scenario.store_item_rects[i].top = CDGN(807,3 + 5 * i);
		scenario.store_item_rects[i].left = CDGN(807,4 + 5 * i);
		scenario.store_item_rects[i].bottom = CDGN(807,5 + 5 * i);
		scenario.store_item_rects[i].right = CDGN(807,6 + 5 * i);
		scenario.store_item_towns[i] = CDGN(807,2 + 5 * i);
		if ((scenario.store_item_towns[i] < -1) || (scenario.store_item_towns[i] >= 200)) {
			give_error("Towns must be in 0 to 200 range (or -1 for no save items rectangle).","",807);
			return false;
		}
	}
	if (((scenario.store_item_towns[0] == scenario.store_item_towns[1]) && 
		 (scenario.store_item_towns[0] >= 0) && (scenario.store_item_towns[1] >= 0))
		||
		((scenario.store_item_towns[2] == scenario.store_item_towns[1]) && 
		 (scenario.store_item_towns[2] >= 0) && (scenario.store_item_towns[1] >= 0))
		||
		((scenario.store_item_towns[2] == scenario.store_item_towns[0]) && 
		 (scenario.store_item_towns[2] >= 0) && (scenario.store_item_towns[0] >= 0))
		) {
		give_error("The three towns towns with saved item rectangles must be different.","",807);
		return false;
	}
#endif
	return true;
}

void edit_save_rects_event_filter (short save_rects_hit) {
#if 0
	switch (save_rects_hit) {
		case 18:
			toast_dialog();
			break;
		case 17:
			if (save_save_rects() == true)
				toast_dialog();
			break;
			
	}
#endif
}

void edit_save_rects() {
#if 0
	// ignore parent in Mac version
	short save_rects_hit;
	
	cd_create_dialog_parent_num(807,0);
	
	put_save_rects_in_dlog();
	
	save_rects_hit = cd_run_dialog();
	cd_kill_dialog(807);
#endif
}

bool save_horses() {
#if 0
	short i;
	
	for (i = 0; i < 6; i++) {
		scenario.horses[6 * store_horse_page + i].which_town = CDGN(808,2 + i);
		if (cre(scenario.horses[6 * store_horse_page + i].which_town,
				-1,199,"Town number must be from 0 to 199 (or -1 for horse to not exist).","",808) == true) return false;
		scenario.horses[6 * store_horse_page + i].loc.x = CDGN(808,8 + i);
		if (cre(scenario.horses[6 * store_horse_page + i].loc.x,
				0,63,"Horse location coordinates must be from 0 to 63.","",808) == true) return false;
		scenario.horses[6 * store_horse_page + i].loc.y = CDGN(808,14 + i);
		if (cre(scenario.horses[6 * store_horse_page + i].loc.y,
				0,63,"Horse location coordinates must be from 0 to 63.","",808) == true) return false;
		scenario.horses[6 * store_horse_page + i].property = cd_get_led(808,43 + i);
	}
#endif
	return true;
}

void put_horses_in_dlog() {
#if 0
	short i;
	
	for (i = 0; i < 6; i++) {
		cdsin(808,23 + i,6 * store_horse_page + i);
		CDSN(808,2 + i,scenario.horses[6 * store_horse_page + i].which_town);
		CDSN(808,8 + i,scenario.horses[6 * store_horse_page + i].loc.x);
		CDSN(808,14 + i,scenario.horses[6 * store_horse_page + i].loc.y);
		cd_set_led(808,43 + i,scenario.horses[6 * store_horse_page + i].property);
	}
#endif
}

void edit_horses_event_filter (short item_hit) {
#if 0
	short i;
	
	switch (item_hit) {
		case 20:
			if (save_horses() == true)
				toast_dialog();
			break;
		case 21:
			if (save_horses() == false) break;
			store_horse_page--;
			if (store_horse_page < 0) store_horse_page = 4;
			put_horses_in_dlog();
			break;
		case 22:
			if (save_horses() == false) break;
			store_horse_page++;
			if (store_horse_page > 4) store_horse_page = 0;
			put_horses_in_dlog();
			break;
		default:
			for (i = 0; i < 6; i++)
				cd_flip_led(808,43 + i,item_hit);
			break;
	}
#endif
}

void edit_horses() {
#if 0
	// ignore parent in Mac version
	short horses_hit;
	
	store_horse_page = 0;
	
	cd_create_dialog_parent_num(808,0);
	
	put_horses_in_dlog();
	
	horses_hit = cd_run_dialog();
	cd_kill_dialog(808);
#endif
}

bool save_boats() {
#if 0
	short i;
	
	for (i = 0; i < 6; i++) {
		scenario.boats[6 * store_boat_page + i].which_town = CDGN(809,2 + i);
		if (cre(scenario.boats[6 * store_boat_page + i].which_town,
				-1,199,"Town number must be from 0 to 199 (or -1 for boat to not exist).","",809) == true) return false;
		scenario.boats[6 * store_boat_page + i].loc.x = CDGN(809,8 + i);
		if (cre(scenario.boats[6 * store_boat_page + i].loc.x,
				0,63,"boat location coordinates must be from 0 to 63.","",809) == true) return false;
		scenario.boats[6 * store_boat_page + i].loc.y = CDGN(809,14 + i);
		if (cre(scenario.boats[6 * store_boat_page + i].loc.y,
				0,63,"boat location coordinates must be from 0 to 63.","",809) == true) return false;
		scenario.boats[6 * store_boat_page + i].property = cd_get_led(809,43 + i);
	}
#endif
	return true;
}

void put_boats_in_dlog() {
#if 0
	short i;
	
	for (i = 0; i < 6; i++) {
		cdsin(809,24 + i,6 * store_boat_page + i);
		CDSN(809,2 + i,scenario.boats[6 * store_boat_page + i].which_town);
		CDSN(809,8 + i,scenario.boats[6 * store_boat_page + i].loc.x);
		CDSN(809,14 + i,scenario.boats[6 * store_boat_page + i].loc.y);
		cd_set_led(809,43 + i,scenario.boats[6 * store_boat_page + i].property);
	}
#endif
}

void edit_boats_event_filter (short item_hit) {
#if 0
	short i;
	
	switch (item_hit) {
		case 20:
			if (save_boats() == true)
				toast_dialog();
			break;
		case 22:
			if (save_boats() == false) break;
			store_boat_page--;
			if (store_boat_page < 0) store_boat_page = 4;
			put_boats_in_dlog();
			break;
		case 23:
			if (save_boats() == false) break;
			store_boat_page++;
			if (store_boat_page > 4) store_boat_page = 0;
			put_boats_in_dlog();
			break;
		default:
			for (i = 0; i < 6; i++)
				cd_flip_led(809,43 + i,item_hit);
			break;
	}
#endif
}

void edit_boats() {
#if 0
	// ignore parent in Mac version
	short boats_hit;
	
	store_boat_page = 0;
	
	cd_create_dialog_parent_num(809,0);
	
	put_boats_in_dlog();
	
	boats_hit = cd_run_dialog();
	cd_kill_dialog(809);
#endif
}

bool save_add_town() {
#if 0
	short i;
	
	for (i = 0; i < 10; i++) {
		scenario.town_to_add_to[i] = CDGN(810,2 + i);
		if (cre(scenario.town_to_add_to[i],
				-1,199,"Town number must be from 0 to 199 (or -1 for no effect).","",810) == true) return false;
		scenario.flag_to_add_to_town[i][0] = CDGN(810,12 + i);
		if (cre(scenario.flag_to_add_to_town[i][0],
				0,299,"First part of flag must be from 0 to 299.","",810) == true) return false;
		scenario.flag_to_add_to_town[i][1] = CDGN(810,22 + i);
		if (cre(scenario.flag_to_add_to_town[i][1],
				0,9,"Second part of flag must be from 0 to 9.","",810) == true) return false;
	}
#endif
	return true;
}

void put_add_town_in_dlog() {
#if 0
	short i;
	
	for (i = 0; i < 10; i++) {
		CDSN(810,2 + i,scenario.town_to_add_to[i]);
		CDSN(810,12 + i,scenario.flag_to_add_to_town[i][0]);
		CDSN(810,22 + i,scenario.flag_to_add_to_town[i][1]);
	}
#endif
}

void edit_add_town_event_filter (short item_hit) {
#if 0
	switch (item_hit) {
		case 32:
			if (save_add_town() == true)
				toast_dialog();
			break;
			
	}
#endif
}

void edit_add_town() {
#if 0
	// ignore parent in Mac version
	short add_town_hit;
	
	cd_create_dialog_parent_num(810,0);
	
	put_add_town_in_dlog();
	
	add_town_hit = cd_run_dialog();
	cd_kill_dialog(810);
#endif
}

bool save_item_placement() {
#if 0
	short i;
	
	store_storage.property = cd_get_led(812,38);
	store_storage.ter_type = CDGN(812,22);
	if (cre(store_storage.ter_type,
			-1,255,"Terrain Type must be from 0 to 255 (or -1 for No Shortcut).","",812) == true) return false;
	for (i = 0; i < 10; i++) {
		store_storage.item_num[i] = CDGN(812,2 + i);
		if (cre(store_storage.item_num[i],
				-1,399,"All item numbers must be from 0 to 399 (or -1 for No Item).","",812) == true) return false;
		store_storage.item_odds[i] = CDGN(812,12 + i);
		if (cre(store_storage.item_odds[i],
				0,100,"All item chances must bve from 0 to 100.","",812) == true) return false;
	}
	scenario.storage_shortcuts[cur_shortcut] = store_storage;
#endif
	return true;
}

void put_item_placement_in_dlog() {
#if 0
	short i;
	
	cdsin(812,27,cur_shortcut);
	cd_set_led(812,38,store_storage.property);
	CDSN(812,22,store_storage.ter_type);
	for (i = 0; i < 10; i++) {
		CDSN(812,2 + i,store_storage.item_num[i]);
		CDSN(812,12 + i,store_storage.item_odds[i]);
	}
#endif
}

void edit_item_placement_event_filter (short item_hit) {
#if 0
	short i;
	
	switch (item_hit) {
		case 23:
			if (save_item_placement() == true)
				toast_dialog();
			break;
		case 24:
			toast_dialog();
			break;
		case 26:
			if (save_item_placement() == false) break;
			cur_shortcut--;
			if (cur_shortcut < 0) cur_shortcut = 9;
			store_storage = scenario.storage_shortcuts[cur_shortcut];
			put_item_placement_in_dlog();
			break;
		case 25:
			if (save_item_placement() == false) break;
			cur_shortcut++;
			if (cur_shortcut > 9) cur_shortcut = 0;
			store_storage = scenario.storage_shortcuts[cur_shortcut];
			put_item_placement_in_dlog();
			break;
		case 41:
			store_storage.ter_type = CDGN(812,22);
			i = choose_text_res(-4,0,255,store_storage.ter_type,812,"Which Terrain?");
			store_storage.ter_type = i;
			CDSN(812,22,i);
			break;
		default:
			if ((item_hit >= 42) && (item_hit <= 51)) {
				i = CDGN(812,2 + item_hit - 42);
				i = choose_text_res(-2,0,399,i,812,"Place which item?");
				if (i >= 0)
					CDSN(812,2 + item_hit - 42,i);
				break;
			}
			cd_flip_led(812,38,item_hit);
			break;
	}
#endif
}

void edit_item_placement() {
#if 0
	// ignore parent in Mac version
	short item_placement_hit;
	
	store_storage = scenario.storage_shortcuts[0];
	cur_shortcut = 0;
	
	cd_create_dialog_parent_num(812,0);
	
	put_item_placement_in_dlog();
	
	item_placement_hit = cd_run_dialog();
	cd_kill_dialog(812);
#endif
}

bool save_scen_details() {
#if 0
	char str[256];
	short i;
	
	scenario.difficulty = cd_get_led_range(803,30,33);
	scenario.rating = cd_get_led_range(803,21,24);
	scenario.format.ver[0] = CDGN(803,2);
	scenario.format.ver[1] = CDGN(803,3);
	scenario.format.ver[2] = CDGN(803,4);
	for (i = 0; i < 3; i++)
		if (cre(scenario.format.ver[i],
				0,9,"The digits in the version number must be in the 0 to 9 range.","",803) == true) return false;
	CDGT(803,5,(char *) str);
	str[59] = 0;
	strcpy(scenario.scen_strs(1),(char *) str);
	CDGT(803,6,(char *) str);
	str[59] = 0;
	strcpy(scenario.scen_strs(2),(char *) str);
	CDGT(803,7,scenario.scen_strs(3));
#endif
	return true;
}

void put_scen_details_in_dlog() {
#if 0
	cd_set_led_range(803,30,33,scenario.difficulty);
	cd_set_led_range(803,21,24,scenario.rating);
	CDSN(803,2,scenario.format.ver[0]);
	CDSN(803,3,scenario.format.ver[1]);
	CDSN(803,4,scenario.format.ver[2]);
	CDST(803,5,scenario.scen_strs(1));
	CDST(803,6,scenario.scen_strs(2));
	CDST(803,7,scenario.scen_strs(3));
#endif
}

void edit_scen_details_event_filter (short item_hit) {
#if 0
	switch (item_hit) {
		case 8:
			if (save_scen_details() == true)
				toast_dialog();
			break;
		default:
			cd_hit_led_range(803,21,24,item_hit);
			cd_hit_led_range(803,30,33,item_hit);
			break;
	}
#endif
}

void edit_scen_details() {
#if 0
	// ignore parent in Mac version
	short scen_details_hit;
	
	cd_create_dialog_parent_num(803,0);
	
	put_scen_details_in_dlog();
	
	scen_details_hit = cd_run_dialog();
	cd_kill_dialog(803);
#endif
}



void put_make_scen_1_in_dlog() {
#if 0
	CDST(800,2,"Scenario name");
	CDST(800,3,"filename");
#endif
}

void edit_make_scen_1_event_filter (short item_hit) {
#if 0
	char str[256];
	short i,j;
	
	switch (item_hit) {
		case 4:
			CDGT(800,3,(char *) str);
			j = strlen((char *) str);
			if (j == 0) {
				give_error("You've left the file name empty.","",800);
				break;
			}
			if (j > 50) {
				give_error("The file name can be at most 50 characters long.","",800);
				break;
			}
			for (i = 0; i < j; i++)
				if ((str[i] < 97) || (str[i] > 122)) {
					give_error("The file name must consist of only lower case letters.","",800);
					return;
				}
			dialog_answer = 1;
			toast_dialog();
			break;
		case 9:
			dialog_answer = 0;
			toast_dialog();
			break;
		default:
			cd_flip_led(800,11,item_hit);
			break;
	}
#endif
}

short edit_make_scen_1(char *filename,char *title,short *grass) {
#if 0
	// ignore parent in Mac version
	short make_scen_1_hit;
	
	cd_create_dialog_parent_num(800,0);
	
	put_make_scen_1_in_dlog();
	
	make_scen_1_hit = cd_run_dialog();
	CDGT(800,2,title);
	title[30] = 0;
	CDGT(800,3,filename);
	*grass = cd_get_led(800,11);
	cd_kill_dialog(800);
#endif
	return 0;//dialog_answer;
}

void put_make_scen_2_in_dlog() {
#if 0
	CDSN(801,2,1);
	CDSN(801,3,1);
	CDSN(801,4,0);
	CDSN(801,5,1);
	CDSN(801,6,0);
#endif
}

void edit_make_scen_2_event_filter (short item_hit) {
#if 0
	short i,j,k;
	
	switch (item_hit) {
		case 11:
			i = CDGN(801,2);
			if (cre(i,
					1,50,"Outdoors width must be between 1 and 50.","",801) == true) return ;
			j = CDGN(801,3);
			if (cre(j,
					1,50,"Outdoors height must be between 1 and 50.","",801) == true) return ;
			if (cre(i * j,
					1,100,"The total number of outdoor sections (width times height) must be between 1 and 100.","",801) == true) return ;
			i = CDGN(801,4);
			j = CDGN(801,5);
			k = CDGN(801,6);
			if (cre(i,
					0,200,"Number of small towns must be between 0 and 200.","",801) == true) return ;
			if (cre(j,
					1,200,"Number of medium towns must be between 0 and 200. The first town (Town 0) must always be of medium size.","",801) == true) return ;
			if (cre(k,
					0,200,"Number of large towns must be between 0 and 200.","",801) == true) return ;
			if (cre(i + j + k,
					1,200,"The total number of towns must be from 1 to 200 (you must have at least 1 town).","",801) == true) return ;
			
			toast_dialog();
			break;
		case 10:
			dialog_answer = 0;
			toast_dialog();
			break;
		default:
			cd_flip_led(801,26,item_hit);
			break;
	}
#endif
}

short edit_make_scen_2(short *val_array) {
#if 0
	// ignore parent in Mac version
	short make_scen_2_hit,i;//array[6];
	
	//array = val_array;
	cd_create_dialog_parent_num(801,0);
	
	put_make_scen_2_in_dlog();
	
	make_scen_2_hit = cd_run_dialog();
	
	for (i = 0; i < 5; i++)
		val_array[i] = CDGN(801,2 + i);
	val_array[5] = cd_get_led(801,26);
	cd_kill_dialog(801);
#endif
	return 0;//dialog_answer;
}

bool build_scenario() {
	printf("Building a scenario currently disabled.\n");
	//	short two_flags[6]; // width, height, large, med, small, default_town
	//	Str255 f_name[256],f_name2[256],title[256];
	//	short grass,password,which_town,error;
	//	FSSpec temp_file_to_load;
	//	short i,j;
	//	long dummy;
	//	
	//	if (edit_make_scen_1((char *) f_name,(char *) title,&grass) == false)
	//		return false;
	//	sprintf((char *) f_name2,"%s.exs",f_name);
	//	if (edit_make_scen_2((short *) two_flags) == false)
	//		return false;
	//	user_given_password = given_password = get_password();
	//	if (fancy_choice_dialog(860,0) == 2)
	//		return false;
	//	town = new cMedTown;
	//	//cMedTown* t_d = (cMedTown*) town;
	//	init_out();
	//	init_scenario();
	//	strcpy((char *) scenario.scen_strs(0),(char *) title);
	//	if (two_flags[5] == 0) {
	//		init_town(1);
	//		if (grass == 0)
	//			for (i = 0; i < 48; i++)
	//				for (j = 0; j < 48; j++)
	//					town->terrain(i,j) = 0;
	//		}
	//	else {
	//		error = FSMakeFSSpec(start_volume,start_dir,"::::Blades of Exile Base",&temp_file_to_load);
	//		if (error != 0) {oops_error(40);}
	//		import_town(0,temp_file_to_load);
	//	}
	//	if (two_flags[3] > 0)
	//		two_flags[3]--;
	//	
	//	make_new_scenario(f_name2,two_flags[0],two_flags[1],two_flags[5],grass);
	//	
	//	// now make sure correct outdoors is in memory, because we're going to be saving scenarios
	//	// for a while
	//	overall_mode = 60;
	//	cur_out.x = 0;
	//	cur_out.y = 0;
	//	load_outdoors(cur_out,0);
	//
	//	for (i = 0; i < two_flags[2]; i++) {
	//		which_town = scenario.num_towns;
	//		scenario.num_towns++;
	//		scenario.town_size[which_town] = 0;
	//		scenario.town_hidden[which_town] = 0;
	//		cur_town = which_town;
	//		init_town(0);
	//		strcpy(data_store->town_strs[0],"Large town");
	//		town_type = 0;
	//		reset_pwd();
	//		save_scenario();
	//		}
	//	for (i = 0; i < two_flags[3]; i++) {
	//		which_town = scenario.num_towns;
	//		scenario.num_towns++;
	//		scenario.town_size[which_town] = 1;
	//		scenario.town_hidden[which_town] = 0;
	//		cur_town = which_town;
	//		init_town(1);
	//		strcpy(data_store->town_strs[0],"Medium town");
	//		town_type = 1;
	//		reset_pwd();
	//		save_scenario();
	//		}
	//	for (i = 0; i < two_flags[4]; i++) {
	//		which_town = scenario.num_towns;
	//		scenario.num_towns++;
	//		scenario.town_size[which_town] = 2;
	//		scenario.town_hidden[which_town] = 0;
	//		cur_town = which_town;
	//		init_town(2);
	//		strcpy(data_store->town_strs[0],"Small town");
	//		town_type = 2;
	//		reset_pwd();
	//		save_scenario();
	//		}
	//	//Delay(200,&dummy);
	//	if(load_town(0)) cur_town = 0;
	//	cur_town = 0;
	//	augment_terrain(cur_out);
	//	update_item_menu();
	return false;
}

void set_starting_loc_filter (short item_hit) {
#if 0
	char str[256];
	short i,j,k;
	
	switch (item_hit) {
		case 5:
			i = CDGN(805,2);
			j = CDGN(805,3);
			k = CDGN(805,4);
			if ((i < 0) || (i >= scenario.num_towns)) {
				sprintf((char *) str,"The starting town must be from 0 to %d.",scenario.num_towns - 1);
				give_error((char *) str,"",805);
				break;
			}
			if ((j < 0) || (j >= max_dim[scenario.town_size[i]] - 1) ||
				(k < 0) || (k >= max_dim[scenario.town_size[i]] - 1)) {
				give_error("This coordinate is not inside the bounds of the town.","",805);
				break;
			}
			scenario.which_town_start = i;
			scenario.where_start.x = j;
			scenario.where_start.y = k;
			toast_dialog();
			break;
		case 12:
			toast_dialog();
			break;
	}
#endif
}

void set_starting_loc() {
#if 0
	// ignore parent in Mac version
	short town_strs_hit;
	
	cd_create_dialog_parent_num(805,0);
	
	CDSN(805,2,scenario.which_town_start);
	CDSN(805,3,scenario.where_start.x);
	CDSN(805,4,scenario.where_start.y);
	
	town_strs_hit = cd_run_dialog();
	
	
	cd_kill_dialog(805);
#endif
}

bool save_scenario_events() {
#if 0
	short i;
	
	for (i = 0; i < 10; i++) {
		scenario.scenario_timer_times[i] = CDGN(811,2 + i);
		if ((scenario.scenario_timer_times[i] > 0) &&
			(scenario.scenario_timer_times[i] % 10 != 0)) {
			give_error("All scenario event times must be multiples of 10 (e.g. 100, 150, 1000, etc.).","",811);
			return false;
		}
		scenario.scenario_timer_specs[i] = CDGN(811,12 + i);
		if (cre(scenario.scenario_timer_specs[i],-1,255,"The scenario special nodes must be between 0 at 255 (or -1 for no special)."
				,"",811) == true) return false;
	}
#endif
	return true;
}

void put_scenario_events_in_dlog() {
#if 0
	short i;
	
	for (i = 0; i < 10; i++) {
		CDSN(811,2 + i,scenario.scenario_timer_times[i]);
		CDSN(811,12 + i,scenario.scenario_timer_specs[i]);
	}
#endif
}

void edit_scenario_events_event_filter (short item_hit) {
#if 0
	short spec;
	
	switch (item_hit) {
		case 22:
			if (save_scenario_events() == true)
				toast_dialog();
			break;
		default:
			if ((item_hit >= 30) && (item_hit <= 39)) {
				if (save_scenario_events() == false)
					break;
				spec = CDGN(811,item_hit - 30 + 12);
				if ((spec < 0) || (spec > 255)) {
					spec = get_fresh_spec(0);
					if (spec < 0) {
						give_error("You can't create a new scenario special encounter because there are no more free special nodes.",
								   "To free a special node, set its type to No Special and set its Jump To special to -1.",811);
						break;
					}
					CDSN(811,item_hit - 30 + 12,spec);
				}	
				edit_spec_enc(spec,0,811);
				if ((spec >= 0) && (spec < 256) && (scenario.scen_specials[spec].pic < 0))
					CDSN(811,item_hit - 30 + 12,-1);
			}
			break;
	}
#endif
}

void edit_scenario_events() {
#if 0
	// ignore parent in Mac version
	short advanced_town_hit;
	
	
	cd_create_dialog_parent_num(811,0);
	
	put_scenario_events_in_dlog();
	
	advanced_town_hit = cd_run_dialog();
	
	cd_kill_dialog(811);
#endif
}
