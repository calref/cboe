
#include <stack>
#include <set>
#include <map>
#include "scen.global.h"
#include "classes.h"
#include "graphtool.h"
#include "scen.graphics.h"
#include "scen.keydlgs.h"
#include "scen.core.h"
#include "dlogutil.h"
#include "restypes.hpp"

extern short cen_x, cen_y/*, overall_mode*/;
extern bool mouse_button_held;
extern short cur_viewing_mode;
extern cTown* town;
//extern big_tr_type t_d;
extern short /*max_dim[3],*/mode_count,to_create;
extern ter_num_t template_terrain[64][64];
extern cItemRec item_list[400];
extern cScenario scenario;
extern cSpecial null_spec_node;
extern cSpeech null_talk_node;
//extern piles_of_stuff_dumping_type *data_store;
extern cOutdoors current_terrain;
extern cCustomGraphics spec_scen_g;

std::stack<short> last_node;
cSpecial store_spec_node;
short num_specs[3] = {256,60,100};

std::vector<pic_num_t> field_pics = {2,3,5,6,7,8,9,10,11,12,13,14,15,16,24,25,26,27,28,29,30,31};
std::vector<pic_num_t> boom_pics = {0,1,2,3,4,8,16,24,32};
std::vector<pic_num_t> lgdlog_pics = {0,72};

size_t num_strs(short str_mode) {
	switch(str_mode) {
		case 0: return scenario.spec_strs.size();
		case 1: return current_terrain.spec_strs.size();
		case 2: return town->spec_strs.size();
		case 3: return scenario.journal_strs.size();
		case 4: return current_terrain.sign_strs.size();
		case 5: return town->sign_strs.size();
	}
	return 0;
}

//cre = check range error
bool cre(short val,short min,short max,const char *text1,const char *text2,cDialog* parent) {
	if((val < min) || (val > max)) {
		giveError(text1,text2,parent);
		return true;
	}
	return false;
}

// TODO: I have two functions that do this. (The other one is pick_picture.)
pic_num_t choose_graphic(short cur_choice,ePicType g_type,cDialog* parent) {
	int i = 0;
	std::vector<std::pair<pic_num_t,ePicType>> pics;
	cPictChoice* pic_dlg = nullptr;
	switch(g_type) {
		case PIC_TER: // TODO: Increase upper limit to allow picking of the added graphics
			pic_dlg = new cPictChoice(0, 252, PIC_TER, parent);
			break;
		case PIC_TER_ANIM: // TODO: Increase to allow picking of the added graphics
			pic_dlg = new cPictChoice(0, 13, PIC_TER_ANIM, parent);
			break;
		case PIC_MONST:
		case PIC_MONST_WIDE:
		case PIC_MONST_TALL:
		case PIC_MONST_LG:
			for(m_pic_index_t m_pic : m_pic_index) {
				// TODO: Put the added monster graphics in m_pic_index to allow picking them
				ePicType type = PIC_MONST;
				if(m_pic.x == 2) type += PIC_WIDE;
				if(m_pic.y == 2) type += PIC_TALL;
				pics.push_back({i++, type});
			}
			pic_dlg = new cPictChoice(pics, parent);
			break;
		case PIC_DLOG: // TODO: Increase upper limit to allow picking of the added graphics
			pic_dlg = new cPictChoice(0, 31, PIC_DLOG, parent);
			break;
		case PIC_TALK:
			pic_dlg = new cPictChoice(0, 83, PIC_TALK, parent);
			break;
		case PIC_SCEN:
			pic_dlg = new cPictChoice(0, 29, PIC_SCEN, parent);
			break;
		case PIC_ITEM:
			pic_dlg = new cPictChoice(0, 122, PIC_ITEM, parent);
			break;
		case PIC_PC:
			pic_dlg = new cPictChoice(0, 35, PIC_PC, parent);
			break;
		case PIC_FIELD:
			pic_dlg = new cPictChoice(field_pics, PIC_FIELD, parent);
			break;
		case PIC_BOOM:
			pic_dlg = new cPictChoice(boom_pics, PIC_BOOM, parent);
			break;
		case PIC_DLOG_LG:
			pic_dlg = new cPictChoice(lgdlog_pics, PIC_DLOG_LG, parent);
			break;
		case PIC_FULL:
			// TODO: Should this be handled at all?
			break;
		case PIC_MISSILE:
			pic_dlg = new cPictChoice(0, 15, PIC_MISSILE, parent);
			break;
		case PIC_STATUS:
			pic_dlg = new cPictChoice(0, 17, PIC_STATUS, parent);
			break;
		case PIC_SCEN_LG:
			pic_dlg = new cPictChoice(0, 3, PIC_SCEN_LG, parent);
			break;
		case PIC_TER_MAP:
			pic_dlg = new cPictChoice(0, 418, PIC_TER_MAP, parent);
			break;
		default: // Custom or party; assume custom, since this is the scenario editor and the party sheet isn't available
			if(g_type & PIC_PARTY) break;
			ePicType g_base_type = g_type - PIC_CUSTOM;
			pic_num_t totalPics = spec_scen_g.count();
			pic_num_t last;
			if(g_base_type == PIC_DLOG || g_base_type == PIC_TALK || g_base_type == PIC_SCEN)
				last = totalPics - 2;
			else if(g_base_type == PIC_TER_ANIM || g_base_type == PIC_MONST || g_base_type == PIC_PC || g_base_type == PIC_MISSILE)
				last = totalPics - 4;
			else if(g_base_type==PIC_DLOG_LG || g_base_type==PIC_SCEN_LG || g_base_type==PIC_MONST_WIDE || g_base_type==PIC_MONST_TALL)
				last = totalPics - 8;
			else if(g_base_type == PIC_MONST_LG) last = totalPics - 16;
			else if(g_base_type == PIC_TER_MAP) last = totalPics * 6 - 1; // TODO: Check this formula
			else last = totalPics = 1;
			pic_dlg = new cPictChoice(0, last, g_type, parent);
	}
	if(!pic_dlg) return cur_choice;
	bool made_choice = pic_dlg->show(cur_choice);
	pic_num_t item_hit = pic_dlg->getPicChosen();
	delete pic_dlg;
	return made_choice ? item_hit : NO_PIC;
}

short choose_text_res(std::string res_list,short first_t,short last_t,unsigned short cur_choice,cDialog* parent,const char *title) {
	location view_loc;
	if((cur_choice < first_t) || (cur_choice > last_t))
		cur_choice = first_t;
	
	StringRsrc strings = *ResMgr::get<StringRsrc>(res_list);
	cStringChoice dlog(strings.begin() + first_t, strings.begin() + last_t + 1, title, parent);
	
	return dlog.show(cur_choice);
}

short choose_text(eStrType list, unsigned short cur_choice, cDialog* parent, const char* title) {
	location view_loc;
	
	std::vector<std::string> strings;
	switch(list) {
		case STRT_MONST:
			for(cMonster& monst : scenario.scen_monsters) {
				strings.push_back(monst.m_name);
			}
			break;
		case STRT_ITEM:
			for(cItemRec& item : scenario.scen_items) {
				strings.push_back(item.full_name);
			}
			break;
		case STRT_TER:
			for(cTerrain& ter : scenario.ter_types) {
				strings.push_back(ter.name);
			}
			break;
		case STRT_BUTTON:
			for(int btn : available_btns) {
				strings.push_back(basic_buttons[btn].label);
			}
	}
	if(cur_choice < 0 || cur_choice >= strings.size())
		cur_choice = 0;
	cStringChoice dlog(strings, title, parent);
	
	return dlog.show(cur_choice);
}

static bool edit_text_event_filter(cDialog& me, std::string item_hit, short& which_str, short str_mode) {
	std::string newVal = me["text"].getText();
	if(str_mode == 0)
		scenario.spec_strs[which_str] = newVal;
	if(str_mode == 1)
		current_terrain.spec_strs[which_str] = newVal;
	if(str_mode == 2)
		town->spec_strs[which_str] = newVal;
	if(str_mode == 3)
		scenario.journal_strs[which_str] = newVal;
	if(str_mode == 4)
		current_terrain.sign_strs[which_str] = newVal;
	if(str_mode == 5)
		town->sign_strs[which_str] = newVal;
	if(item_hit == "okay") me.toast(true);
	else if(item_hit == "left" || item_hit == "right") {
		if(item_hit[0] == 'l')
			which_str--;
		else which_str++;
		if(which_str < 0) which_str = num_strs(str_mode) - 1;
		if(which_str >= num_strs(str_mode)) which_str = 0;
	}
	me["num"].setTextToNum(which_str);
	if(str_mode == 0)
		me["text"].setText(scenario.spec_strs[which_str]);
	if(str_mode == 1)
		me["text"].setText(current_terrain.spec_strs[which_str]);
	if(str_mode == 2)
		me["text"].setText(town->spec_strs[which_str]);
	if(str_mode == 3)
		me["text"].setText(scenario.journal_strs[which_str]);
	if(str_mode == 4)
		me["text"].setText(current_terrain.sign_strs[which_str]);
	if(str_mode == 5)
		me["text"].setText(town->sign_strs[which_str]);
	return true;
}

// mode 0 - scen 1 - out 2 - town
void edit_text_str(short which_str,short mode) {
	using namespace std::placeholders;
	
	cDialog dlog("edit-text.xml");
	dlog.attachClickHandlers(std::bind(edit_text_event_filter, _1, _2, std::ref(which_str), mode), {"okay", "left", "right"});
	
	dlog["num"].setTextToNum(which_str);
	if(mode == 0)
		dlog["text"].setText(scenario.spec_strs[which_str]);
	if(mode == 1)
		dlog["text"].setText(current_terrain.spec_strs[which_str]);
	if(mode == 2)
		dlog["text"].setText(town->spec_strs[which_str]);
	if(mode == 3)
		dlog["text"].setText(scenario.journal_strs[which_str]);
	if(mode == 4)
		dlog["text"].setText(current_terrain.sign_strs[which_str]);
	if(mode == 5)
		dlog["text"].setText(town->sign_strs[which_str]);
	
	dlog.run();
}

static bool edit_area_rect_event_filter(cDialog& me, std::string item_hit, short which_str, short str_mode) {
	if(item_hit == "okay") {
		me.setResult(true);
		me.toast(true);
		std::string str = me["area"].getText().substr(0,29);
		if(str_mode == 0)
			current_terrain.rect_names[which_str];
		else town->rect_names[which_str];
	} else if(item_hit == "cancel") {
		me.setResult(false);
		me.toast(false);
	}
	return true;
}

// mode 0 - out 1 - town
bool edit_area_rect_str(short which_str,short mode) {
	using namespace std::placeholders;
	
	cDialog dlog("set-area-desc.xml");
	dlog.attachClickHandlers(std::bind(edit_area_rect_event_filter, _1, _2, which_str, mode), {"okay", "cancel"});
	
	if(mode == 0)
		dlog["area"].setText(current_terrain.rect_names[which_str]);
	else dlog["area"].setText(town->rect_names[which_str]);
	
	dlog.run();
	
	return dlog.getResult<bool>();
}

// MARK: Special node dialog

static bool save_spec_enc(cDialog& me, short which_mode, short which_node) {
	store_spec_node.sd1 = me["sdf1"].getTextAsNum();
	store_spec_node.sd2 = me["sdf2"].getTextAsNum();
	store_spec_node.m1 = me["msg1"].getTextAsNum();
	store_spec_node.m2 = me["msg2"].getTextAsNum();
	store_spec_node.pic = me["pict"].getTextAsNum();
	if(store_spec_node.pic < 0)
		store_spec_node.pic = 0;
	store_spec_node.ex1a = me["x1a"].getTextAsNum();
	store_spec_node.ex1b = me["x1b"].getTextAsNum();
	store_spec_node.ex2a = me["x2a"].getTextAsNum();
	store_spec_node.ex2b = me["x2b"].getTextAsNum();
	store_spec_node.jumpto = me["jump"].getTextAsNum();
	
	if((*store_spec_node.type).sdf_label == 1) {
		if(cre(store_spec_node.sd1,-1,299,"The first part of a Stuff Done flag must be from 0 to 299 (or -1 if the Stuff Done flag is ignored.",
				"",&me) > 0) return false;
		if(cre(store_spec_node.sd2,-1,9,"The second part of a Stuff Done flag must be from 0 to 9 (or -1 if the Stuff Done flag is ignored.",
				"",&me) > 0) return false;
	}
	
	if(which_mode == 0)
		scenario.scen_specials[which_node] = store_spec_node;
	if(which_mode == 1)
		current_terrain.specials[which_node] = store_spec_node;
	if(which_mode == 2)
		town->specials[which_node] = store_spec_node;
	return true;
}

static void put_spec_enc_in_dlog(cDialog& me, short which_node) {
	std::string str;
	
	me["num"].setTextToNum(which_node);
	str = get_str("special-node-names",int(store_spec_node.type) + 1);
	me["type"].setText(str);
	
	if(last_node.empty())
		me["back"].hide();
	else me["back"].show();
	
	me["sdf1"].setTextToNum(store_spec_node.sd1);
	me["sdf2"].setTextToNum(store_spec_node.sd2);
	switch((*store_spec_node.type).sdf_label) {
		case 0:
			me["sdf1-lbl"].setText("Unused.");
			me["sdf2-lbl"].setText("Unused.");
			break;
		case 1:
			me["sdf1-lbl"].setText("Stuff Done Flag Part A");
			me["sdf2-lbl"].setText("Stuff Done Flag Part B");
			break;
		case 2:
			me["sdf1-lbl"].setText("Chance of placing (0 - 100)");
			me["sdf2-lbl"].setText("Unused");
			break;
		case 3:
			me["sdf1-lbl"].setText("Stuff Done Flag Part A");
			me["sdf2-lbl"].setText("Unused");
			break;
		case 4:
			me["sdf1-lbl"].setText("X of space to move to");
			me["sdf2-lbl"].setText("Y of space to move to");
			break;
		case 5:
			me["sdf1-lbl"].setText("Terrain to change to");
			me["sdf2-lbl"].setText("Chance of changing (0 - 100)");
			break;
		case 6:
			me["sdf1-lbl"].setText("Switch this ter. type");
			me["sdf2-lbl"].setText("with this ter. type");
			break;
		case 7:
			me["sdf1-lbl"].setText("Chance of placing (0 - 100)");
			me["sdf2-lbl"].setText("What to place (see docs.)");
			break;
		case 8:
			me["sdf1-lbl"].setText("Chance of placing (0 - 100)");
			me["sdf2-lbl"].setText("0 - web, 1 - barrel, 2 - crate");
			break;
	}
	
	me["msg1"].setTextToNum(store_spec_node.m1);
	me["msg2"].setTextToNum(store_spec_node.m2);
	switch((*store_spec_node.type).msg_label) {
		case 0:
			me["msg1-lbl"].setText("Unused.");
			me["msg2-lbl"].setText("Unused.");
			me["msg1-edit"].hide();
			me["msg2-edit"].hide();
			break;
		case 1:
			me["msg1-lbl"].setText("First part of message");
			me["msg2-lbl"].setText("Second part of message");
			me["msg1-edit"].hide();
			me["msg2-edit"].show();
			break;
		case 2:
			me["msg1-lbl"].setText("Number of first message in dialog");
			me["msg2-lbl"].setText("Unused");
			me["msg1-edit"].hide();
			me["msg2-edit"].show();
			break;
		case 3:
			me["msg1-lbl"].setText("Name of Store");
			me["msg2-lbl"].setText("Unused");
			me["msg1-edit"].hide();
			me["msg2-edit"].show();
			break;
		case 4:
			me["msg1-lbl"].setText("Number of first message in dialog");
			me["msg2-lbl"].setText("1 - add 'Leave'/'OK' button, else no");
			me["msg1-edit"].hide();
			me["msg2-edit"].show();
			break;
		case 5:
			me["msg1-lbl"].setText("Number of first message in dialog");
			me["msg2-lbl"].setText("Num. of spec. item to give (-1 none)");
			me["msg1-edit"].hide();
			me["msg2-edit"].show();
			break;
	}
	
	me["pict"].setTextToNum(store_spec_node.pic);
	switch((*store_spec_node.type).pic_label) {
		case 0:
			me["pict-lbl"].setText("Unused.");
			me["pict-edit"].hide();
			break;
		case 1:
			me["pict-lbl"].setText("Dialog Picture number");
			me["pict-edit"].show();
			break;
		case 2:
			me["pict-lbl"].setText("Terrain Picture number");
			me["pict-edit"].show();
			break;
		case 3:
			me["pict-lbl"].setText("Monster Picture number");
			me["pict-edit"].show();
			break;
		case 4:
			me["pict-lbl"].setText("Chance of changing (0 - 100)");
			me["pict-edit"].hide();
			break;
		case 5:
			me["pict-lbl"].setText("Number of letters to match");
			me["pict-edit"].hide();
			break;
		case 6:
			me["pict-lbl"].setText("Radius of explosion");
			me["pict-edit"].hide();
			break;
	}
	
	me["x1a"].setTextToNum(store_spec_node.ex1a);
	str = get_str("special-x1a",int(store_spec_node.type) + 1);
	me["x1a-lbl"].setText(str);
	if((*store_spec_node.type).ex1a_choose == 1)
		me["x1a-edit"].show();
	else me["x1a-edit"].hide();
	
	me["x1b"].setTextToNum(store_spec_node.ex1b);
	str = get_str("special-x1b",int(store_spec_node.type) + 1);
	me["x1b-lbl"].setText(str);
	if((*store_spec_node.type).ex1b_choose == 1)
		me["x1b-edit"].show();
	else me["x1b-edit"].hide();
	
	me["x2a"].setTextToNum(store_spec_node.ex2a);
	str = get_str("special-x2a",int(store_spec_node.type) + 1);
	me["x2a-lbl"].setText(str);
	if((*store_spec_node.type).ex2a_choose == 1)
		me["x2a-edit"].show();
	else me["x2a-edit"].hide();
	
	me["x2b"].setTextToNum(store_spec_node.ex2b);
	str = get_str("special-x2b",int(store_spec_node.type) + 1);
	me["x2b-lbl"].setText(str);
	if((*store_spec_node.type).ex2b_choose == 1)
		me["x2b-edit"].show();
	else me["x2b-edit"].hide();
	
	me["jump"].setTextToNum(store_spec_node.jumpto);
	switch((*store_spec_node.type).jmp_label) {
		case 0:
			me["jump-lbl"].setText("Special to Jump To");
			break;
		case 1:
			me["jump-lbl"].setText("Special node if not blocked");
			break;
		case 2:
			me["jump-lbl"].setText("Special after trap finished");
			break;
		case 3:
			me["jump-lbl"].setText("Otherwise call this special");
			break;
		case 4:
			me["jump-lbl"].setText("Special if OK/Leave picked");
			break;
	}
}

static bool edit_spec_enc_event_filter(cDialog& me, std::string item_hit, short& which_mode, short& which_node) {
	short i,node_to_change_to = -1,spec;
	
	if(item_hit == "okay") {
		if(save_spec_enc(me, which_mode, which_node))
			me.toast(true);
		me.setResult(true);
	} else if(item_hit == "back") {
		if(!save_spec_enc(me, which_mode, which_node))
			return true;
		if(!last_node.empty()) {
			node_to_change_to = last_node.top();
			last_node.pop();
		}
	} else if(item_hit == "cancel") {
		if(!last_node.empty()) {
			giveError("You can't cancel out of making a special until you're back at the beginning of the special chain.",
					  "Press the Go Back button until it disappears.",&me);
			return true;
		}
		me.toast(false);
		me.setResult(false);
	} else if(me[item_hit].getText() == "Create/Edit") {
		if(!save_spec_enc(me, which_mode, which_node))
			return true;
		if(item_hit == "x1b-edit")
			spec = me["x1b"].getTextAsNum();
		else if(item_hit == "x2b-edit")
			spec = me["x2b"].getTextAsNum();
		else if(item_hit == "jump-edit")
			spec = me["jump"].getTextAsNum();
		if((spec < 0) || (spec >= num_specs[which_mode])) {
			spec = -1;
			//CDSN(822,8,-1)
			// TODO: Generalize this situation of a node jumping to a scenario node
			if(item_hit == "x1b-edit" && store_spec_node.type == eSpecType::SCEN_TIMER_START)
				spec = get_fresh_spec(0);
			else if(item_hit == "jump-edit" && store_spec_node.type == eSpecType::CALL_GLOBAL)
				spec = get_fresh_spec(0);
			else spec = get_fresh_spec(which_mode);
			if(spec < 0) {
				giveError("You can't create a new special encounter because there are no more free special nodes.",
						  "To free a special node, set its type to No Special and set its Jump To special to -1.",&me);
				return true;
			}
			if(item_hit == "x1b-edit")
				me["x1b"].setTextToNum(spec);
			else if(item_hit == "x2b-edit")
				me["x2b"].setTextToNum(spec);
			else if(item_hit == "jump-edit")
				me["jump"].setTextToNum(spec);
			/*
			if(item_hit == 43)
				store_spec_node.ex1b = spec;
			if(item_hit == 44)
				store_spec_node.ex2b = spec;
			if(item_hit == 45)
				store_spec_node.jumpto = spec;
			*/
		}
		if(!save_spec_enc(me, which_mode, which_node))
			return true;
		if(item_hit == "x1b-edit" && store_spec_node.type == eSpecType::SCEN_TIMER_START)
			node_to_change_to = spec;
		else if(item_hit == "jump-edit" && store_spec_node.type == eSpecType::CALL_GLOBAL)
			node_to_change_to = spec;
		else node_to_change_to = which_mode * 1000 + spec;
		last_node.push(which_mode * 1000 + which_node);
	} else if(item_hit == "x1a-edit") {
		switch(store_spec_node.type) {
			case eSpecType::FORCED_GIVE:
			case eSpecType::ONCE_GIVE_ITEM:
			case eSpecType::ONCE_GIVE_ITEM_DIALOG:
			case eSpecType::ONCE_GIVE_ITEM_TERRAIN:
			case eSpecType::ONCE_GIVE_ITEM_MONSTER:
				i = choose_text(STRT_ITEM,store_spec_node.ex1a,&me,"Give which item?");
				break;
			case eSpecType::OUT_STORE:
				i = choose_text(STRT_ITEM,store_spec_node.ex1a,&me,"First item in store?");
				break;
			case eSpecType::ONCE_DIALOG:
			case eSpecType::ONCE_DIALOG_TERRAIN:
			case eSpecType::ONCE_DIALOG_MONSTER:
				i = choose_text(STRT_BUTTON,store_spec_node.ex1a,&me,"Which button label?");
				break;
			case eSpecType::TOWN_DESTROY_MONST:
				i = choose_text(STRT_MONST,store_spec_node.ex1a,&me,"Choose Which Monster:");
				break;
		}
		store_spec_node.ex1a = i;
		me["x1a"].setTextToNum(store_spec_node.ex1a);
	} else if(item_hit == "x2a-edit") {
		switch(store_spec_node.type) {
			case eSpecType::FORCED_GIVE:
			case eSpecType::ONCE_GIVE_ITEM:
			case eSpecType::TOWN_PLACE_ITEM:
			case eSpecType::OUT_STORE: // TODO: This isn't right - it should be "number of items" or some such
				i = choose_text(STRT_ITEM,store_spec_node.ex2a,&me,"Give which item?");
				break;
			case eSpecType::ONCE_DIALOG:
			case eSpecType::ONCE_DIALOG_TERRAIN:
			case eSpecType::ONCE_DIALOG_MONSTER:
			case eSpecType::ONCE_GIVE_ITEM_DIALOG:
			case eSpecType::ONCE_GIVE_ITEM_TERRAIN:
			case eSpecType::ONCE_GIVE_ITEM_MONSTER:
				i = choose_text(STRT_BUTTON,store_spec_node.ex2a,&me,"Which button label?");
				break;
			case eSpecType::TOWN_PLACE_MONST:
				i = choose_text(STRT_MONST,store_spec_node.ex2a,&me,"Choose Which Monster:");
				break;
			case eSpecType::IF_TOWN_TER_TYPE:
			case eSpecType::IF_OUT_TER_TYPE:
			case eSpecType::TOWN_CHANGE_TER:
			case eSpecType::TOWN_SWAP_TER:
			case eSpecType::OUT_CHANGE_TER:
				i = choose_text(STRT_TER,store_spec_node.ex2a,&me,"Which Terrain?");
				break;
		}
		store_spec_node.ex2a = i;
		me["x2a"].setTextToNum(store_spec_node.ex2a);
	} else if(item_hit == "msg2-edit") { // TODO: What about msg1-edit?
		if(save_spec_enc(me, which_mode, which_node))
			return true;
		if(((*store_spec_node.type).msg_label == 2) ||
			((*store_spec_node.type).msg_label == 4) ||
			((*store_spec_node.type).msg_label == 5)) {
			edit_dialog_text(which_mode,&store_spec_node.m1,&me);
			put_spec_enc_in_dlog(me, which_node);
		}
		else if(((*store_spec_node.type).msg_label == 1) ||
				 ((*store_spec_node.type).msg_label == 3)) {
			edit_spec_text(which_mode,&store_spec_node.m1,
						   &store_spec_node.m2,&me);
			put_spec_enc_in_dlog(me, which_node);
		}
	} else if(item_hit == "pict-edit") {
		if(save_spec_enc(me, which_mode, which_node))
			return true;
		i = -1;
		switch((*store_spec_node.type).pic_label) {
			case 1:
				i = choose_graphic(store_spec_node.pic,PIC_DLOG,&me);
				break;
			case 2:
				i = choose_graphic(store_spec_node.pic,PIC_TER,&me);
				break;
			case 3:
				i = choose_graphic(store_spec_node.pic,PIC_MONST,&me);
				break;
		}
		if(i != NO_PIC) {
			store_spec_node.pic = i;
			put_spec_enc_in_dlog(me, which_node);
		}
	} else if(item_hit == "general") {
		if(save_spec_enc(me, which_mode, which_node))
			return true;
		// TODO: I wonder if this can all be achieved without casts... if not, at least check getNodeCategory to ensure validity.
		i = choose_text_res("special-node-names",1,28,int(store_spec_node.type) + 1,&me,"Choose General Use Special:");
		if(i >= 0) {
			store_spec_node.type = eSpecType(i - 1);
		}
		put_spec_enc_in_dlog(me, which_node);
	} else if(item_hit == "oneshot") {
		if(save_spec_enc(me, which_mode, which_node))
			return true;
		i = choose_text_res("special-node-names",51,64,int(store_spec_node.type) + 1,&me,"Choose One-Shot Special:");
		if(i >= 0) {
			store_spec_node.type = eSpecType(i - 1);
			store_spec_node.sd1 = -1;
			store_spec_node.sd2 = -1;
			if(store_spec_node.type == eSpecType::ONCE_DIALOG || store_spec_node.type == eSpecType::ONCE_DIALOG_TERRAIN || store_spec_node.type == eSpecType::ONCE_DIALOG_MONSTER)
				store_spec_node.m2 = 1;
		}
		put_spec_enc_in_dlog(me, which_node);
	} else if(item_hit == "affectpc") {
		if(save_spec_enc(me, which_mode, which_node))
			return true;
		i = choose_text_res("special-node-names",81,107,int(store_spec_node.type) + 1,&me,"Choose Affect Party Special:");
		if(i >= 0) store_spec_node.type = eSpecType(i - 1);
		put_spec_enc_in_dlog(me, which_node);
	} else if(item_hit == "ifthen") {
		if(save_spec_enc(me, which_mode, which_node))
			return true;
		i = choose_text_res("special-node-names",131,156,int(store_spec_node.type) + 1,&me,"Choose If-Then Special:");
		if(i >= 0) {
			store_spec_node.type = eSpecType(i - 1);
		}
		put_spec_enc_in_dlog(me, which_node);
	} else if(item_hit == "town") {
		if(save_spec_enc(me, which_mode, which_node))
			return true;
		i = choose_text_res("special-node-names",171,219,int(store_spec_node.type) + 1,&me,"Choose Town Special:");
		if(i >= 0) store_spec_node.type = eSpecType(i - 1);
		put_spec_enc_in_dlog(me, which_node);
	} else if(item_hit == "out") {
		if(save_spec_enc(me, which_mode, which_node))
			return true;
		i = choose_text_res("special-node-names",226,230,int(store_spec_node.type) + 1,&me,"Choose Outdoor Special:");
		if(i >= 0) store_spec_node.type = eSpecType(i - 1);
		put_spec_enc_in_dlog(me, which_node);
	}
	/*if((item_hit >= 37) && (item_hit <= 42)) {
		if(cd_get_active(822,43) == 0)
			CDSN(822,8,0);
		if(cd_get_active(822,44) == 0)
			CDSN(822,10,0);
	}*/// Might be useful, but I forget what I was thinking when I added it.
	if(node_to_change_to >= 0) {
		which_mode = node_to_change_to / 1000;
		which_node = node_to_change_to % 1000;
		if(which_mode == 0)
			store_spec_node = scenario.scen_specials[which_node];
		if(which_mode == 1)
			store_spec_node = current_terrain.specials[which_node];
		if(which_mode == 2)
			store_spec_node = town->specials[which_node];
		if(store_spec_node.pic < 0)
			store_spec_node.pic = 0;
		put_spec_enc_in_dlog(me, which_node);
	}
	return true;
}

// mode - 0 scen 1 - out 2 - town
bool edit_spec_enc(short which_node,short mode,cDialog* parent) {
	// ignore parent in Mac version
	using namespace std::placeholders;
	
	// Clear the "nodes edited" stack; should already be clear, but just make sure
	while(!last_node.empty()) last_node.pop();
	//last_node[0] = store_which_mode * 1000 + store_which_node;
	if(mode == 0)
		store_spec_node = scenario.scen_specials[which_node];
	if(mode == 1)
		store_spec_node = current_terrain.specials[which_node];
	if(mode == 2)
		store_spec_node = town->specials[which_node];
	if(store_spec_node.pic < 0)
		store_spec_node.pic = 0;
	
	cDialog special("edit-special-node.xml",parent);
	auto callback = std::bind(edit_spec_enc_event_filter, _1, _2, std::ref(mode), std::ref(which_node));
	special.attachClickHandlers(callback, {"okay", "cancel", "back"});
	special.attachClickHandlers(callback, {"general", "oneshot", "affectpc", "ifthen", "town", "out"});
	special.attachClickHandlers(callback, {"x1a-edit", "x1b-edit", "x2a-edit", "x2b-edit"});
	special.attachClickHandlers(callback, {"msg1-edit", "msg2-edit", "pict-edit", "jump-edit"});
	
	special["back"].hide();
	put_spec_enc_in_dlog(special, which_node);
	
	special.run();
	return special.getResult<bool>();
}

short get_fresh_spec(short which_mode) {
	short i;
	cSpecial store_node;
	
	for(i = 0; i < num_specs[which_mode]; i++) {
		if(which_mode == 0)
			store_node = scenario.scen_specials[i];
		if(which_mode == 1)
			store_node = current_terrain.specials[i];
		if(which_mode == 2)
			store_node = town->specials[i];
		if(store_node.type == eSpecType::NONE && store_node.jumpto == -1 && store_node.pic == -1)
			return i;
	}
	
	return -1;
}

static bool edit_spec_text_event_filter(cDialog& me, std::string item_hit, short spec_str_mode, short* str1, short* str2) {
	std::string str;
	short i;
	
	if(item_hit == "okay") {
		str = me["str1"].getText();
		if(!str.empty()) {
			if(*str1 < 0) {
				size_t n = num_strs(spec_str_mode);
				for(i = 0; i < n; i++)
					switch(spec_str_mode) {
						case 0:
							// TODO: This could overwrite a string if it's unlucky enough to start with *
							if(scenario.spec_strs[i][0] == '*') {
								*str1 = i;
								i = 500;
							}
							break;
						case 1:
							if(current_terrain.spec_strs[i][0] == '*') {
								*str1 = i;
								i = 500;
							}
							break;
						case 2:
							if(town->spec_strs[i][0] == '*') {
								*str1 = i;
								i = 500;
							}
							break;
						case 3:
							if(scenario.journal_strs[i][0] == '*') {
								*str1 = i;
								i = 500;
							}
							break;
						case 4:
							if(current_terrain.sign_strs[i][0] == '*') {
								*str1 = i;
								i = 500;
							}
							break;
						case 5:
							if(town->sign_strs[i][0] == '*') {
								*str1 = i;
								i = 500;
							}
							break;
					}
				if(i < 500) {
					giveError("There are no more free message slots.",
							  "To free up some slots, go into Edit Town/Out/Scenario Text to clear some messages.", &me);
					return true;
				}
			}
			if(*str1 >= 0) {
				switch(spec_str_mode) {
					case 0:
						scenario.spec_strs[*str1] = str;
						break;
					case 1:
						current_terrain.spec_strs[*str1] = str;
						break;
					case 2:
						town->spec_strs[*str1] = str;
						break;
					case 3:
						scenario.journal_strs[*str1] = str;
						break;
					case 4:
						current_terrain.sign_strs[*str1] = str;
						break;
					case 5:
						town->sign_strs[*str1] = str;
						break;
				}
			}
		}
		str = me["str2"].getText();
		if(!str.empty()) {
			if(*str2 < 0) {
				size_t n = num_strs(spec_str_mode);
				for(i = 160; i < n; i++)
					switch(spec_str_mode) {
						case 0:
							if(scenario.spec_strs[i][0] == '*') {
								*str2 = i;
								i = 500;
							}
							break;
						case 1:
							if(current_terrain.spec_strs[i][0] == '*') {
								*str2 = i;
								i = 500;
							}
							break;
						case 2:
							if(town->spec_strs[i][0] == '*') {
								*str2 = i;
								i = 500;
							}
							break;
						case 3:
							if(scenario.journal_strs[i][0] == '*') {
								*str2 = i;
								i = 500;
							}
							break;
						case 4:
							if(current_terrain.sign_strs[i][0] == '*') {
								*str2 = i;
								i = 500;
							}
							break;
						case 5:
							if(town->sign_strs[i][0] == '*') {
								*str2 = i;
								i = 500;
							}
							break;
							
					}
				if(i < 500) {
					giveError("There are no more free message slots.",
							  "To free up some slots, go into Edit Town/Out/Scenario Text to clear some messages.", &me);
					return true;
				}
			}
			if(*str2 >= 0) {
				switch(spec_str_mode) {
					case 0:
						scenario.spec_strs[*str2] = str;
						break;
					case 1:
						current_terrain.spec_strs[*str2] = str;
						break;
					case 2:
						town->spec_strs[*str2] = str;
						break;
					case 3:
						scenario.journal_strs[*str2] = str;
						break;
					case 4:
						current_terrain.sign_strs[*str2] = str;
						break;
					case 5:
						town->sign_strs[*str2] = str;
						break;
				}
			}
		}
		me.toast(true);
	} else me.toast(false);
	return true;
}

// mode 0 - scen 1 - out 2 - town
void edit_spec_text(short mode,short *str1,short *str2,cDialog* parent) {
	using namespace std::placeholders;
	short num_s_strs[3] = {100,90,100};
	
	cDialog edit("edit-special-text.xml", parent);
	edit.attachClickHandlers(std::bind(edit_spec_text_event_filter, _1, _2, mode, str1, str1), {"okay", "cancel"});
	
	if(*str1 >= num_s_strs[mode])
		*str1 = -1;
	if(*str1 >= 0){
		if(mode == 0)
			edit["str1"].setText(scenario.spec_strs[*str1]);
		if(mode == 1)
			edit["str1"].setText(current_terrain.spec_strs[*str1]);
		if(mode == 2)
			edit["str1"].setText(town->spec_strs[*str1]);
		if(mode == 3)
			edit["str1"].setText(scenario.journal_strs[*str1]);
		if(mode == 4)
			edit["str1"].setText(current_terrain.sign_strs[*str1]);
		if(mode == 5)
			edit["str1"].setText(town->sign_strs[*str1]);
	}
	if(*str2 >= num_s_strs[mode])
		*str2 = -1;
	if(*str2 >= 0){
		if(mode == 0)
			edit["str2"].setText(scenario.spec_strs[*str2]);
		if(mode == 1)
			edit["str2"].setText(current_terrain.spec_strs[*str2]);
		if(mode == 2)
			edit["str2"].setText(town->spec_strs[*str2]);
		if(mode == 3)
			edit["str2"].setText(scenario.journal_strs[*str2]);
		if(mode == 4)
			edit["str2"].setText(current_terrain.sign_strs[*str2]);
		if(mode == 5)
			edit["str2"].setText(town->sign_strs[*str2]);
	}
	edit.run();
}

static bool edit_dialog_text_event_filter(cDialog& me, std::string item_hit, short spec_str_mode, short* str1){
	std::string str;
	short i;
	
	if(item_hit == "okay") {
		for(i = 0; i < 6; i++) {
			std::string id = "str" + std::to_string(i + 1);
			str = me[id].getText();
			if(i == 0 && str.empty()) break;
			switch(spec_str_mode) {
				case 0:
					scenario.spec_strs[*str1 + i] = str;
					break;
				case 1:
					current_terrain.spec_strs[*str1 + i] = str;
					break;
				case 2:
					town->spec_strs[*str1 + i] = str;
					break;
				case 3:
					scenario.journal_strs[*str1 + i] = str;
					break;
				case 4:
					current_terrain.sign_strs[*str1 + i] = str;
					break;
				case 5:
					town->sign_strs[*str1 + i] = str;
					break;
			}
		}
		me.toast(true);
	} else me.toast(false);
	return true;
}

// mode 0 - scen 1 - out 2 - town
void edit_dialog_text(short mode,short *str1,cDialog* parent) {
	short i,j;
	short num_s_strs[3] = {100,90,100};
	
	if(*str1 >= num_s_strs[mode] - 6)
		*str1 = -1;
	// first, assign the 6 strs for the dialog.
	if(*str1 < 0) {
		size_t n = num_strs(mode);
		for(i = 160; i < n - 6; i++) {
			for(j = i; j < i + 6; j++)
				switch(mode) {
					case 0:
						if(scenario.spec_strs[j][0] != '*')
							j = 500;
						break;
					case 1:
						if(current_terrain.spec_strs[j][0] != '*')
							j = 500;
						break;
					case 2:
						if(town->spec_strs[j][0] != '*')
							j = 500;
						break;
					case 3:
						if(scenario.journal_strs[j][0] != '*')
							j = 500;
						break;
					case 4:
						if(current_terrain.sign_strs[j][0] != '*')
							j = 500;
						break;
					case 5:
						if(town->sign_strs[j][0] != '*')
							j = 500;
						break;
				}
			if(j < 500) {
				*str1 = i;
				i = 500;
			}
		}
		if(*str1 >= 0)
			for(i = *str1; i < *str1 + 6; i++) {
				switch(mode) {
					case 0:
						scenario.spec_strs[i] = "";
						break;
					case 1:
						current_terrain.spec_strs[i] = "";
						break;
					case 2:
						town->spec_strs[i] = "";
						break;
					case 3:
						scenario.journal_strs[i] = "";
						break;
					case 4:
						current_terrain.sign_strs[i] = "";
						break;
					case 5:
						town->sign_strs[i] = "";
						break;
				}
			}
	}
	if(*str1 < 0) {
		giveError("To create a dialog, you need 6 consecutive unused messages. To free up 6 messages, select Edit Out/Town/Scenario Text from the menus.","",parent);
		return;
	}
	
	using namespace std::placeholders;
	cDialog edit("edit-dialog-text.xml",parent);
	edit.attachClickHandlers(std::bind(edit_dialog_text_event_filter, _1, _2, mode, str1), {"okay", "cancel"});
	
	if(*str1 >= 0){
		for(i = 0; i < 6; i++) {
			std::string id = "str" + std::to_string(i + 1);
			if(mode == 0)
				edit[id].setText(scenario.spec_strs[*str1 + i]);
			if(mode == 1)
				edit[id].setText(current_terrain.spec_strs[*str1 + i]);
			if(mode == 2)
				edit[id].setText(town->spec_strs[*str1 + i]);
			if(mode == 0)
				edit[id].setText(scenario.journal_strs[*str1 + i]);
			if(mode == 1)
				edit[id].setText(current_terrain.sign_strs[*str1 + i]);
			if(mode == 2)
				edit[id].setText(town->sign_strs[*str1 + i]);
		}
	}
	
	edit.run();
}

static bool edit_special_num_event_filter(cDialog& me, std::string item_hit, short spec_mode) {
	short i;
	
	if(item_hit == "cancel") me.setResult<short>(-1);
	else if(item_hit == "okay") {
		i = me["num"].getTextAsNum();
		if((i < 0) || (i >= num_specs[spec_mode])) {
			giveError("There is no special node with that number. Legal ranges are 0 to 255 for scenario specials, 0 to 59 for outdoor specials, and 0 to 99 for town specials.","",&me);
		}
		me.setResult(i);
		me.toast(true);
	} me.toast(false);
	return true;
}

short edit_special_num(short mode,short what_start) {
	using namespace std::placeholders;
	
	cDialog edit("edit-special-assign.xml");
	edit.attachClickHandlers(std::bind(edit_special_num_event_filter, _1, _2, mode), {"okay", "cancel"});
	
	edit["num"].setTextToNum(what_start);
	
	edit.run();
	
	return edit.getResult<short>();
}

static bool edit_scen_intro_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	short i;
	
	if(item_hit == "okay") {
		scenario.intro_pic = me["picnum"].getTextAsNum();
		if(scenario.intro_pic > 29) {
			giveError("Intro picture number is out of range.","",&me);
			return true;
		}
		for(i = 0; i < 6; i++) {
			std::string id = "str" + std::to_string(i + 1);
			scenario.intro_strs[i] = me[id].getText();
		}
		me.toast(true);
	} else if(item_hit == "cancel") {
		me.toast(false);
	} else if(item_hit == "choose") {
		pic_num_t pic = me["picnum"].getTextAsNum();
		i = choose_graphic(pic,PIC_SCEN,&me);
		if(i != NO_PIC) {
			me["picnum"].setTextToNum(i);
			dynamic_cast<cPict&>(me["pic"]).setPict(i);
		}
	}
	return true;
}

void edit_scen_intro() {
	short i;
	
	cDialog edit("edit-intro.xml");
	edit.attachClickHandlers(edit_scen_intro_event_filter, {"okay", "cancel", "choose"});
	
	edit["picnum"].setTextToNum(scenario.intro_pic);
	for(i = 0; i < 6; i++) {
		std::string id = "str" + std::to_string(i + 1);
		edit[id].setText(scenario.intro_strs[i]);
	}
	dynamic_cast<cPict&>(edit["pic"]).setPict(scenario.intro_pic);
	
	edit.run();
}

void make_cursor_sword() {
	set_cursor(wand_curs);
}
