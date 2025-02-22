
#include <stack>
#include <set>
#include <map>
#include <numeric>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include "scen.global.hpp"
#include "scenario/scenario.hpp"
#include "gfx/gfxsheets.hpp"
#include "utility.hpp"
#include "scen.graphics.hpp"
#include "scen.keydlgs.hpp"
#include "scen.core.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "dialogxml/widgets/control.hpp"
#include "dialogxml/widgets/button.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/3choice.hpp"
#include "dialogxml/dialogs/strchoice.hpp"
#include "dialogxml/dialogs/pictchoice.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "fileio/resmgr/res_strings.hpp"
#include "spell.hpp"
#include "gfx/render_shapes.hpp" // for colour constants

extern short cen_x, cen_y;
extern bool mouse_button_held;
extern short cur_viewing_mode;
extern cTown* town;
extern short mode_count,to_create;
extern ter_num_t template_terrain[64][64];
extern cScenario scenario;
extern cOutdoors* current_terrain;
extern cCustomGraphics spec_scen_g;

std::vector<pic_num_t> field_pics = {0,3,5,6,7,8,9,10,11,12,13,14,15,24,25,26,27,28,29,30,31,4};
std::vector<pic_num_t> boom_pics = {0,1,2,3,4,5};
std::vector<pic_num_t> lgdlog_pics = {0,32};

size_t num_strs(eStrMode str_mode) {
	switch(str_mode) {
		case 0: return scenario.spec_strs.size();
		case 1: return current_terrain->spec_strs.size();
		case 2: return town->spec_strs.size();
		case 3: return scenario.journal_strs.size();
		case 4: return current_terrain->sign_locs.size();
		case 5: return town->sign_locs.size();
		case 6: return current_terrain->area_desc.size();
		case 7: return town->area_desc.size();
	}
	return 0;
}

static void ensure_str(eStrMode str_mode, size_t which) {
	if(which >= num_strs(str_mode)) {
		switch(str_mode) {
			case 0: scenario.spec_strs.resize(which + 1, "*"); break;
			case 1: current_terrain->spec_strs.resize(which + 1, "*"); break;
			case 2: town->spec_strs.resize(which + 1, "*"); break;
			case 3: scenario.journal_strs.resize(which + 1, "*"); break;
			case 4: current_terrain->sign_locs.resize(which + 1, {-1, -1, "*"}); break;
			case 5: town->sign_locs.resize(which + 1, {-1, -1, "*"}); break;
			case 6: current_terrain->area_desc.resize(which + 1, {-1, -1, -1, -1, "*"}); break;
			case 7: town->area_desc.resize(which + 1, {-1, -1, -1, -1, "*"}); break;
		}
	}
}

static std::string& fetch_str(eStrMode str_mode, size_t which) {
	ensure_str(str_mode, which);
	switch(str_mode) {
		case 0: return scenario.spec_strs[which];
		case 1: return current_terrain->spec_strs[which];
		case 2: return town->spec_strs[which];
		case 3: return scenario.journal_strs[which];
		case 4: return current_terrain->sign_locs[which].text;
		case 5: return town->sign_locs[which].text;
		case 6: return current_terrain->area_desc[which].descr;
		case 7: return town->area_desc[which].descr;
	}
	throw "Invalid string mode " + std::to_string(str_mode) + " (valid are 0-5)";
}

static std::string str_info(eStrMode str_mode, size_t which) {
	ensure_str(str_mode, which);
	std::ostringstream sout;
	switch(str_mode) {
		case 0: case 1: case 2: case 3:
			sout << which;
			break;
		case 4:
			sout << "(" << current_terrain->sign_locs[which].x;
			sout << ", " << current_terrain->sign_locs[which].y << ")";
			break;
		case 5:
			sout << "(" << town->sign_locs[which].x;
			sout << ", " << town->sign_locs[which].y << ")";
			break;
		case 6:
			sout << "(" << current_terrain->area_desc[which].left;
			sout << ", " << current_terrain->area_desc[which].top;
			sout << ")|(" << current_terrain->area_desc[which].right;
			sout << ", " << current_terrain->area_desc[which].bottom << ")";
			break;
		case 7:
			sout << "(" << town->area_desc[which].left;
			sout << ", " << town->area_desc[which].top;
			sout << ")|(" << town->area_desc[which].right;
			sout << ", " << town->area_desc[which].bottom << ")";
			break;
	}
	return sout.str();
}

//cre = check range error
bool cre(short val,short min,short max,std::string text1,std::string text2,cDialog* parent) {
	if((val < min) || (val > max)) {
		showError(text1,text2,parent);
		return true;
	}
	return false;
}

short choose_background(short cur_choice, cDialog* parent) {
	cDialog bg_dlg(*ResMgr::dialogs.get("choose-bg"),parent);
	auto get_selected = [&]() -> short {
		std::string sel = dynamic_cast<cLedGroup&>(bg_dlg["group"]).getSelected();
		if(sel.empty()) return -1;
		return boost::lexical_cast<int>(sel.substr(3)) - 1;
	};
	auto set_colour_for = [&](int which) {
		sf::Color to = sf::Color::Black;
		if(which == 2)
			to = Colours::RED;
		else if(which == 3)
			to = {0x00, 0x00, 0x80};
		else if(which == 11)
			to = {0xC0, 0x40, 0xFF};
		else if(which <= 5 || which == 9 || which == 15)
			to = sf::Color::White;
		bg_dlg["title"].setColour(to);
		for(int i = 0; i < 21; i++)
			bg_dlg["led" + std::to_string(i + 1)].setColour(to);
	};
	bg_dlg["done"].attachClickHandler(std::bind(&cDialog::toast, &bg_dlg, true));
	bg_dlg["cancel"].attachClickHandler(std::bind(&cDialog::toast, &bg_dlg, false));
	bg_dlg["group"].attachFocusHandler([&](cDialog&, std::string, bool) -> bool {
		int which = get_selected();
		bg_dlg.setBg(which);
		set_colour_for(which);
		return true;
	});
	int sel = cur_choice;
	if(sel < 0 || sel >= 21) sel = bg_dlg.getBg();
	dynamic_cast<cLedGroup&>(bg_dlg["group"]).setSelected("led" + std::to_string(sel + 1));
	bg_dlg.setBg(sel);
	set_colour_for(sel);
	bg_dlg.run();
	if(bg_dlg.accepted())
		return get_selected();
	return cur_choice;
}

// TODO: I have two functions that do this. (The other one is pick_picture.)
extern std::string scenario_temp_dir_name;
pic_num_t choose_graphic(short cur_choice,ePicType g_type,cDialog* parent) {
	extern fs::path tempDir;
	int i = 0;
	std::vector<pic_num_t> all_pics;
	size_t total_pics = 0;
	cPictChoice* pic_dlg = nullptr;
	switch(g_type + PIC_PRESET) {
		case PIC_TER: total_pics = 680; break;
		case PIC_TER_ANIM: total_pics = 30; break;
		case PIC_DLOG: total_pics = 44; break;
		case PIC_TALK: total_pics = 84; break;
		case PIC_SCEN: total_pics = 30; break;
		case PIC_ITEM: case PIC_TINY_ITEM: total_pics = 139; break;
		// TODO: Include small monster graphics in the PC pic picker
		case PIC_PC: total_pics = 37; break;
		case PIC_FIELD: all_pics = field_pics; break;
		case PIC_BOOM: all_pics = boom_pics; break;
		case PIC_DLOG_LG: all_pics = lgdlog_pics; break;
		case PIC_MISSILE: total_pics = 16; break;
		case PIC_STATUS: total_pics = 27; break;
		case PIC_SCEN_LG: total_pics = 4; break;
		case PIC_TER_MAP: total_pics = 990; break;
		case PIC_FULL:
			if(!fs::is_directory(tempDir/scenario_temp_dir_name/"graphics")) {
				showError("You have no custom graphics, so it's not possible to select this kind of picture!",parent);
				return NO_PIC;
			}
			for(fs::directory_iterator iter(tempDir/scenario_temp_dir_name/"graphics"); iter != fs::directory_iterator(); iter++) {
				std::string fname = iter->path().filename().string().c_str();
				size_t dot = fname.find_last_of('.');
				if(dot == std::string::npos) continue;
				std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
				if(fname.substr(dot) != ".png") continue;
				size_t digit = fname.find_first_of("0123456789");
				if(digit == std::string::npos) continue;
				if(fname.substr(0, digit) != "sheet") continue;
				all_pics.push_back(boost::lexical_cast<pic_num_t>(fname.substr(digit, dot - digit)));
			}
			std::sort(all_pics.begin(), all_pics.end());
			pic_dlg = new cPictChoice(all_pics, g_type, parent); // To suppress adding custom pics
			break;
		default:
			std::cerr << "Picture type " << (g_type + PIC_PRESET) << " is missing case in choose_graphic." << std::endl;
			return NO_PIC;
		case PIC_MONST: case PIC_MONST_WIDE:
		case PIC_MONST_TALL: case PIC_MONST_LG:
			std::vector<std::pair<pic_num_t,ePicType>> pics;
			for(m_pic_index_t m_pic : m_pic_index) {
				// TODO: Put the added monster graphics in m_pic_index to allow picking them
				ePicType type = PIC_MONST;
				if(m_pic.x == 2) type += PIC_WIDE;
				if(m_pic.y == 2) type += PIC_TALL;
				pics.push_back({i++, type});
			}
			for(size_t i = 0; i < scenario.custom_graphics.size(); i++) {
				if(scenario.custom_graphics[i] == PIC_MONST)
					pics.push_back({1000 + i, PIC_CUSTOM_MONST});
				else if(scenario.custom_graphics[i] == PIC_MONST_WIDE)
					pics.push_back({2000 + i, PIC_CUSTOM_MONST_WIDE});
				else if(scenario.custom_graphics[i] == PIC_MONST_TALL)
					pics.push_back({3000 + i, PIC_CUSTOM_MONST_TALL});
				else if(scenario.custom_graphics[i] == PIC_MONST_LG)
					pics.push_back({4000 + i, PIC_CUSTOM_MONST_LG});
				if(cur_choice == pics.back().first)
					cur_choice = pics.size() - 1;
			}
			pic_dlg = new cPictChoice(pics, parent);
			break;
	}
	if(!all_pics.empty())
		cur_choice = std::find(all_pics.begin(), all_pics.end(), cur_choice) - all_pics.begin();
	if(!pic_dlg) {
		if(all_pics.size());
		else if(total_pics > 0) {
			all_pics.resize(total_pics);
			std::iota(all_pics.begin(), all_pics.end(), 0);
		} else return cur_choice;
		// Now add the custom pics
		for(size_t i = 0; i < scenario.custom_graphics.size(); i++) {
			if(g_type == PIC_BOOM) break;
			if(scenario.custom_graphics[i] == g_type) {
				if(g_type == PIC_TER_MAP) {
					for(int j = 1; j <= 6; j++)
						all_pics.push_back(j * 1000 + i);
				} else if(g_type == PIC_TER_ANIM)
					all_pics.push_back(2000 + i);
				else all_pics.push_back(1000 + i);
			}
		}
		if(cur_choice >= 1000) {
			auto selected = std::find(all_pics.begin(), all_pics.end(), cur_choice);
			if(selected == all_pics.end()) cur_choice = -1;
			else cur_choice = selected - all_pics.begin();
		}
		pic_dlg = new cPictChoice(all_pics, g_type, parent);
	}
	bool made_choice = pic_dlg->show(cur_choice);
	pic_num_t item_hit = pic_dlg->getPicChosen();
	delete pic_dlg;
	return made_choice ? item_hit : NO_PIC;
}

short choose_text_res(std::string res_list,short first_t,short last_t,unsigned short cur_choice,cDialog* parent,const char *title) {
	location view_loc;
	if((cur_choice < first_t) || (cur_choice > last_t))
		cur_choice = -1;
	else cur_choice -= first_t;
	
	StringList strings = *ResMgr::strings.get(res_list);
	cStringChoice dlog(strings.begin() + first_t - 1, strings.begin() + last_t, title, parent);
	
	return dlog.show(cur_choice);
}

short choose_text(eStrType list, unsigned short cur_choice, cDialog* parent, std::string title) {
	location view_loc;
	
	std::vector<std::string> strings;
	switch(list) {
		case STRT_MONST:
			for(cMonster& monst : scenario.scen_monsters) {
				strings.push_back(monst.m_name);
			}
			strings.erase(strings.begin()); // First monster is not a valid monster
			break;
		case STRT_ITEM:
			for(cItem& item : scenario.scen_items) {
				strings.push_back(item.full_name);
			}
			break;
		case STRT_SPEC_ITEM:
			for(cSpecItem& item : scenario.special_items) {
				strings.push_back(item.name);
			}
			break;
		case STRT_QUEST:
			for(cQuest& quest : scenario.quests) {
				strings.push_back(quest.name);
			}
			break;
		case STRT_TER:
			for(cTerrain& ter : scenario.ter_types) {
				strings.push_back(ter.name);
			}
			break;
		case STRT_TOWN:
			for(cTown* town : scenario.towns) {
				strings.push_back(town->name);
			}
			break;
		case STRT_SECTOR:
			for(size_t i = 0; i < scenario.outdoors.width(); i++) {
				for(size_t j = 0; j < scenario.outdoors.height(); j++) {
					std::ostringstream name;
					name << '[' << i << ',' << j << ']';
					name << ' ' << scenario.outdoors[i][j]->name;
					strings.push_back(name.str());
				}
			}
			break;
		case STRT_MAGE:
			for(int i = 0; i < 62; i++) {
				strings.push_back((*cSpell::fromNum(eSkill::MAGE_SPELLS, i)).name());
			}
			break;
		case STRT_PRIEST:
			for(int i = 0; i < 62; i++) {
				strings.push_back((*cSpell::fromNum(eSkill::PRIEST_SPELLS, i)).name());
			}
			break;
		case STRT_ALCHEMY:
			for(int i = 0; i < 20; i++) {
				strings.push_back(get_str("magic-names", i + 200));
			}
			break;
		case STRT_SKILL:
			for(int i = 0; i < 19; i++) {
				strings.push_back(get_str("skills", i * 2 + 1));
			}
			strings.push_back("Maximum Health");
			strings.push_back("Maximum Spell Points");
			break;
		case STRT_TRAIT:
			for(int i = 0; i < 17; i++) {
				strings.push_back(get_str("traits", i * 2 + 1));
			}
			break;
		case STRT_RACE:
			for(int i = 0; i < 22; i++) {
				strings.push_back(get_str("traits", i * 2 + 35));
			}
			break;
		case STRT_PICT:
			strings = *ResMgr::strings.get("picture-types");
			break;
		case STRT_TRAP:
			strings = *ResMgr::strings.get("trap-types");
			break;
		case STRT_HEALING:
			strings = *ResMgr::strings.get("shop-specials");
			break;
		case STRT_BUTTON:
			for(auto btn : basic_buttons) {
				strings.push_back(btn.name.empty() ? btn.label : btn.name);
			}
			break;
		case STRT_CMP:
			strings = {"Less Than or Equal", "Less Than", "Equal", "Greater Than", "Grater Than or Equal"};
			break;
		case STRT_ACCUM:
			strings = {"Current PC Only", "Cumulative", "Average", "Minimum", "Maximum"};
			break;
		case STRT_ATTITUDE:
			strings = {"Docile", "Hostile Type A", "Friendly", "Hostile Type B"};
			break;
		case STRT_STAIR:
			strings = {"Stairs Up", "Stairs Down", "Slope Up", "Slope Down", "Slimy Stairs Up", "Slimy Stairs Down", "Dark Slope Up", "Dark Slope Down"};
			break;
		case STRT_STAIR_MODE:
			strings = {"Normal behaviour", "Allow triggering in combat", "Allow triggering when not moving (eg when looking at the space)", "Both of the above"};
			break;
		case STRT_LIGHT:
			strings = {"Fully Lit", "Dark", "Very Dark", "Totally Dark"};
			break;
		case STRT_TALK_NODE:
			for(int i = 0; i < 31; i++) {
				strings.push_back(get_str("talk-node-descs", i * 7 + 1));
			}
			break;
		case STRT_CONTEXT:
			strings = *ResMgr::strings.get("special-contexts");
			break;
		case STRT_SHOP:
			for(cShop& shop : scenario.shops) {
				strings.push_back(shop.getName());
			}
			break;
		case STRT_COST_ADJ:
			strings = {"Extremely Cheap", "Very Reasonable", "Pretty Average", "Somewhat Pricey", "Expensive", "Exorbitant", "Utterly Ridiculous"};
			break;
		case STRT_STATUS:
			strings = {"Alive", "Dead", "Dust", "Petrified", "Fled Outdoor Combat", "Absent", "Deleted"};
			break;
		case STRT_SPELL_PAT:
			strings = {"Single Space", "3x3 Square", "2x2 Square", "3x3 Open Square", "Radius 2 Circle", "Radius 3 Circle", "Cross", "Rotateable 2x8 Wall"};
			break;
		case STRT_SUMMON:
			strings = {"0 - no summon (weak)", "1 - weak summoning", "2 - summoning", "3 - major summoning", "4 - no summon (unique/powerful"};
			break;
		case STRT_TALK_NODE_PERSON:
			strings.push_back("All personalities use this response");
			strings.push_back("Node is unused");
			BOOST_FALLTHROUGH;
		case STRT_TALK:
			for(cTown* town : scenario.towns) {
				for(cPersonality who : town->talking.people) {
					strings.push_back(who.title + " (in " + town->name + ")");
				}
			}
			break;
		case STRT_ENCHANT:
			strings = {"+1", "+2", "+3", "Shoot Flames", "Flaming", "+5", "Blessed", "+4"};
			break;
		case STRT_DIR:
			strings = {"North", "Northease", "East", "Southeast", "South", "Southwest", "West", "Northwest", "None"};
			break;
		case STRT_QUEST_STATUS:
			strings = {"Available", "Started", "Completed", "Failed"};
			break;
		case STRT_TREASURE:
			strings = {"0 - Junk", "1 - Lousy", "2 - So-so", "3 - Good", "4 - Great"};
			break;
		case STRT_MONST_STAT:
			strings = {
				"Maximum Health", "Maximum Magic Points", "Armor", "Skill", "Speed", "Mage Spells", "Priest Spells",
				"Magic Resistance", "Fire Resistance", "Cold Resistance", "Poison Resistance",
			};
			break;
		case STRT_POS_MODE:
			strings = {"Absolute Position", "Move Southeast", "Move Southwest", "Move Northwest", "Move Northeast", "Nearest"};
			break;
		case STRT_DEBUG_PRINT:
			strings = {"Print SDF", "Print Extra1 Values", "Print Monster HP/MP"};
			break;
		case STRT_TARG_TYPE:
			strings = {"Only living characters", "Any character", "All characters simultaneously", "Dead characters only", "Characters with inventory space"};
			break;
		case STRT_TARG_MODE:
			strings = {"Player can choose", "Select at random", "Specific character specified in Extra 2b"};
			break;
		case STRT_ID_MODE:
			strings = {"Based on Item Lore", "Never Identify", "Always Identify", "Always Identify (and also reveal concealed ability)"};
			break;
		case STRT_CURSE_MODE:
			strings = {"Depends on item", "Force uncursed", "Force cursed"};
			break;
		case STRT_EQUIP_MODE:
			strings = {"Do not equip", "Soft Equip (only if no conflicting items equipped)", "Try to equip (unequipping any conflicting items first if possible)", "Force equip (forcibly unequipping even conflicting cursed items)"};
			break;
		case STRT_CMP_MODE:
			strings = {"Check value in range", "Check value outside range", "Compare to single value"};
			break;
		case STRT_PATH:
			strings = {"Straight Line", "Parabolic Arc"};
			break;
		case STRT_SPELL_PAT_MODE:
			strings = {"Sequential Simple Booms", "Simultaneous Animated Booms"};
			break;
		case STRT_LABEL_ALIGN:
			strings = {"Align Top", "Align Centre"};
			break;
	}
	if(cur_choice < 0 || cur_choice >= strings.size())
		cur_choice = -1;
	cStringChoice dlog(strings, title, parent);
	
	return dlog.show(cur_choice);
}

static bool edit_text_event_filter(cDialog& me, std::string item_hit, short& which_str, eStrMode str_mode) {
	std::string newVal = me["text"].getText();
	fetch_str(str_mode, which_str) = newVal;
	if(item_hit == "okay") me.toast(true);
	else if(item_hit == "left" || item_hit == "right") {
		if(item_hit[0] == 'l')
			which_str--;
		else which_str++;
		if(which_str < 0) which_str = num_strs(str_mode) - 1;
		if(which_str >= num_strs(str_mode)) which_str = 0;
	}
	me["num"].setText(str_info(str_mode, which_str));
	me["text"].setText(fetch_str(str_mode, which_str));
	return true;
}

bool edit_text_str(short which_str,eStrMode mode) {
	using namespace std::placeholders;
	short first = which_str;
	
	cDialog dlog(*ResMgr::dialogs.get("edit-text"));
	dlog.attachClickHandlers(std::bind(edit_text_event_filter, _1, _2, std::ref(which_str), mode), {"okay", "left", "right"});
	dlog["cancel"].attachClickHandler(std::bind(&cDialog::toast, _1, false));
	
	dlog["num"].setText(str_info(mode, which_str));
	dlog["text"].setText(fetch_str(mode, which_str));
	
	dlog.run();
	return dlog.accepted() || which_str != first;
}

static bool edit_area_rect_event_filter(cDialog& me, std::string item_hit, info_rect_t& r) {
	if(item_hit == "okay") {
		me.setResult(true);
		me.toast(true);
		std::string str = me["area"].getText().substr(0,29);
		r.descr = str;
	} else if(item_hit == "cancel") {
		me.setResult(false);
		me.toast(false);
	}
	return true;
}

// mode 0 - out 1 - town
bool edit_area_rect_str(info_rect_t& r) {
	using namespace std::placeholders;
	
	cDialog dlog(*ResMgr::dialogs.get("set-area-desc"));
	dlog.attachClickHandlers(std::bind(edit_area_rect_event_filter, _1, _2, std::ref(r)), {"okay", "cancel"});
	
	dlog["area"].setText(r.descr);
	
	dlog.run();
	
	return dlog.getResult<bool>();
}

// MARK: Special node dialog

struct editing_node_t {
	short which, mode;
	cSpecial node;
};

typedef std::stack<editing_node_t> node_stack_t;

static void setup_node_field(cDialog& me, std::string field, short value, const node_function_t& fcn) {
	me[field + "-lbl"].setText(fcn.label());
	me[field].setTextToNum(value);
	bool is_sdf = field.substr(0,3) == "sdf";
	std::string button = field + "-edit";
	switch(fcn.button) {
		case eSpecPicker::NONE:
			me[button].hide();
			break;
		case eSpecPicker::MSG_PAIR: case eSpecPicker::MSG_SINGLE:
		case eSpecPicker::MSG_SEQUENCE: case eSpecPicker::NODE:
			me[button].show();
			if(is_sdf) break;
			me[button].setText("Create/Edit");
			dynamic_cast<cButton&>(me[button]).setBtnType(BTN_LG);
			break;
		default:
			me[button].show();
			if(is_sdf) break;
			me[button].setText("Choose");
			dynamic_cast<cButton&>(me[button]).setBtnType(BTN_REG);
			break;
	}
}

static void put_spec_enc_in_dlog(cDialog& me, node_stack_t& edit_stack) {
	cSpecial& spec = edit_stack.top().node;
	
	// Show which node is being edited and what type of node it is
	switch(edit_stack.top().mode) {
		case 0: me["title"].setText("Edit Scenario Special Node"); break;
		case 1: me["title"].setText("Edit Outdoors Special Node"); break;
		case 2: me["title"].setText("Edit Town Special Node"); break;
	}
	me["num"].setTextToNum(edit_stack.top().which);
	
	// Show the node opcode information
	me["type"].setText((*spec.type).name());
	
	node_properties_t info = *spec.type;
	// Set up the labels, fields, and buttons
	setup_node_field(me, "sdf1", spec.sd1, info.sdf1(spec));
	setup_node_field(me, "sdf2", spec.sd2, info.sdf2(spec));
	
	setup_node_field(me, "msg1", spec.m1, info.msg1(spec));
	setup_node_field(me, "msg2", spec.m2, info.msg2(spec));
	setup_node_field(me, "msg3", spec.m3, info.msg3(spec));
	
	setup_node_field(me, "pict", spec.pic, info.pic(spec));
	setup_node_field(me, "pictype", spec.pictype, info.pictype(spec));
	
	setup_node_field(me, "x1a", spec.ex1a, info.ex1a(spec));
	setup_node_field(me, "x1b", spec.ex1b, info.ex1b(spec));
	setup_node_field(me, "x1c", spec.ex1c, info.ex1c(spec));
	
	setup_node_field(me, "x2a", spec.ex2a, info.ex2a(spec));
	setup_node_field(me, "x2b", spec.ex2b, info.ex2b(spec));
	setup_node_field(me, "x2c", spec.ex2c, info.ex2c(spec));
	
	setup_node_field(me, "jump", spec.jumpto, info.jump(spec));
}

static void save_spec_enc(cDialog& me, node_stack_t& edit_stack) {
	cSpecial& the_node = edit_stack.top().node;
	the_node.sd1 = me["sdf1"].getTextAsNum();
	the_node.sd2 = me["sdf2"].getTextAsNum();
	the_node.m1 = me["msg1"].getTextAsNum();
	the_node.m2 = me["msg2"].getTextAsNum();
	the_node.m3 = me["msg3"].getTextAsNum();
	the_node.pic = me["pict"].getTextAsNum();
	the_node.pictype = me["pictype"].getTextAsNum();
	the_node.ex1a = me["x1a"].getTextAsNum();
	the_node.ex1b = me["x1b"].getTextAsNum();
	the_node.ex1c = me["x1c"].getTextAsNum();
	the_node.ex2a = me["x2a"].getTextAsNum();
	the_node.ex2b = me["x2b"].getTextAsNum();
	the_node.ex2c = me["x2c"].getTextAsNum();
	the_node.jumpto = me["jump"].getTextAsNum();
}

static bool commit_spec_enc(cDialog& me, std::string item_hit, node_stack_t& edit_stack) {
	if(item_hit != "unwind")
		save_spec_enc(me, edit_stack);
	int mode = edit_stack.top().mode, node = edit_stack.top().which;
	switch(mode) {
		case 0: scenario.scen_specials[node] = edit_stack.top().node; break;
		case 1: current_terrain->specials[node] = edit_stack.top().node; break;
		case 2: town->specials[node] = edit_stack.top().node; break;
	}
	edit_stack.pop();
	if(item_hit == "okay") {
		while(!edit_stack.empty())
			commit_spec_enc(me, "unwind", edit_stack);
		me.toast(true);
		me.setResult(true);
	} else if(item_hit == "back") {
		put_spec_enc_in_dlog(me, edit_stack);
		if(edit_stack.size() == 1)
			me["back"].hide();
	}
	return true;
}

static bool discard_spec_enc(cDialog& me, node_stack_t& edit_stack) {
	std::string action = "revert";
	int stack_size = edit_stack.size();
	if(stack_size > 1) {
		action = cChoiceDlog("discard-special-node", {"save", "cancel", "revert"}).show();
		if(action == "save") return true;
	}
	auto cur = edit_stack.top();
	auto& list = cur.mode == 0 ? scenario.scen_specials : (cur.mode == 1 ? current_terrain->specials : town->specials);
	if(cur.which == list.size() - 1 && list[cur.which].type == eSpecType::NONE && list[cur.which].jumpto == -1)
		list.pop_back();
	edit_stack.pop();
	if(action == "cancel") {
		while(!edit_stack.empty()) {
			auto cur = edit_stack.top();
			auto& list = cur.mode == 0 ? scenario.scen_specials : (cur.mode == 1 ? current_terrain->specials : town->specials);
			if(cur.which == list.size() - 1 && list[cur.which].type == eSpecType::NONE && list[cur.which].jumpto == -1)
				list.pop_back();
			edit_stack.pop();
		}
	} else if(edit_stack.size() == 1)
		me["back"].hide();
	if(edit_stack.empty())
		me.toast(false);
	else put_spec_enc_in_dlog(me, edit_stack);
	return true;
}

static bool edit_spec_enc_type(cDialog& me, std::string item_hit, node_stack_t& edit_stack) {
	save_spec_enc(me, edit_stack);
	eSpecCat category = eSpecCat::INVALID;
	if(item_hit == "general") category = eSpecCat::GENERAL;
	else if(item_hit == "oneshot") category = eSpecCat::ONCE;
	else if(item_hit == "affectpc") category = eSpecCat::AFFECT;
	else if(item_hit == "ifthen") category = eSpecCat::IF_THEN;
	else if(item_hit == "town") category = eSpecCat::TOWN;
	else if(item_hit == "out") category = eSpecCat::OUTDOOR;
	else if(item_hit == "rect") category = eSpecCat::RECT;
	auto bounds = *category;
	int start = int(bounds.first), finish = int(bounds.last), current = int(edit_stack.top().node.type);
	if(start < 0 || finish < 0) return true;
	std::vector<std::string> choices;
	for(int i = start; i <= finish; i++) {
		eSpecType node = eSpecType(i);
		std::string name = (*node).name();
		if(name.empty()) choices.push_back("Unused Node");
		else choices.push_back(name);
	}
	size_t cancelled = -1, result;
	cStringChoice choose(choices, "Select a special node type:",&me);
	result = choose.show((current < start || current > finish) ? cancelled : current - start);
	if(result != cancelled) {
		edit_stack.top().node.type = eSpecType(result + start);
		put_spec_enc_in_dlog(me, edit_stack);
	}
	return true;
}

short choose_field_type(short cur, cDialog* parent, bool includeSpec) {
	static const char*const fieldNames[] = {
		"Wall of Force", "Wall of Fire", "Antimagic Field", "Stinking Cloud", "Wall of Ice", "Wall of Blades", "Sleep Cloud",
		"Stone Block", "Webs", "Crate", "Barrel", "Fire Barrier", "Force Barrier", "Quickfire",
		"Small Blood", "Medium Blood", "Large Blood", "Small Slime", "Large Slime", "Ash", "Bones", "Rubble",
		"Force Cage", "Cleanse Space", "Crumble Walls",
	};
	std::vector<std::pair<pic_num_t,ePicType>> pics = {
		{8, PIC_FIELD}, {9, PIC_FIELD}, {10, PIC_FIELD}, {11, PIC_FIELD},
		{12, PIC_FIELD}, {13, PIC_FIELD}, {14, PIC_FIELD},
		{3, PIC_FIELD}, {5, PIC_FIELD}, {6, PIC_FIELD}, {7, PIC_FIELD},
		{14, PIC_TER_ANIM}, {14, PIC_TER_ANIM}, {15, PIC_FIELD},
		{24, PIC_FIELD}, {25, PIC_FIELD}, {26, PIC_FIELD}, {27, PIC_FIELD},
		{28, PIC_FIELD}, {29, PIC_FIELD}, {30, PIC_FIELD}, {31, PIC_FIELD},
		{0, PIC_FIELD},
	};
	if(includeSpec) {
		pics.push_back({23, PIC_FIELD});
		pics.push_back({95, PIC_TER});
	}
	short prev = cur;
	cur--;
	if(cur >= SPECIAL_SPOT) cur--;
	if(cur >= 25) cur -= 7;
	if(cur < 0 || cur >= pics.size()) cur = 0;
	cPictChoice pic_dlg(pics, parent);
	pic_dlg->getControl("prompt").setText("Select a field type:");
	pic_dlg->getControl("help").setText(fieldNames[cur]);
	pic_dlg.attachSelectHandler([](cPictChoice& me, int n) {
		me->getControl("help").setText(fieldNames[n]);
	});
	bool made_choice = pic_dlg.show(cur);
	size_t item_hit = pic_dlg.getSelected();
	item_hit++;
	if(item_hit >= SPECIAL_SPOT)
		item_hit++;
	if(item_hit >= 25)
		item_hit += 7;
	return made_choice ? item_hit : prev;
}

pic_num_t choose_damage_type(short cur, cDialog* parent, bool allow_spec) {
	static const char*const damageNames[] = {"Weapon", "Fire", "Poison", "Magic", "Weird", "Cold", "Undead", "Demon", "Unblockable"};
	static const std::vector<pic_num_t> pics = {3,0,2,1,5,4,3,3,1};
	short prev = cur;
	if(cur < 0 || cur >= pics.size()) cur = 0;
	cPictChoice pic_dlg(pics.begin(), pics.end() - !allow_spec, PIC_BOOM, parent);
	pic_dlg->getControl("prompt").setText("Select a damage type:");
	pic_dlg->getControl("help").setText(damageNames[cur]);
	pic_dlg.attachSelectHandler([](cPictChoice& me, int n) {
		me->getControl("help").setText(damageNames[n]);
	});
	bool made_choice = pic_dlg.show(cur);
	size_t item_hit = pic_dlg.getSelected();
	return made_choice ? item_hit : prev;
}

static pic_num_t choose_boom_type(short cur, cDialog* parent) {
	static const int preset_booms = 6;
	static const char*const boomNames[preset_booms+1] = {
		"Fire", "Teleport", "Magic/Electricity",
		"Magic/Electricity", "Weird", "Cold",
		"Custom Explosion"
	};
	std::vector<pic_num_t> pics;
	for(int i = 0; i < preset_booms; i++) pics.push_back(i + 8);
	for(int i = 0; i < scenario.custom_graphics.size(); i++) {
		if(scenario.custom_graphics[i] == PIC_BOOM)
			pics.push_back(1000 + i);
	}
	short prev = cur;
	if(cur < 0 || (cur >= preset_booms && cur < 1000) || (cur >= 1000 && cur - 1000 < pics.size() - preset_booms))
	   cur = 0;
	cPictChoice pic_dlg(pics, PIC_BOOM, parent);
	pic_dlg->getControl("prompt").setText("Select a boom type:");
	pic_dlg->getControl("help").setText(boomNames[std::min<short>(cur,preset_booms)]);
	pic_dlg.attachSelectHandler([](cPictChoice& me, int n) {
		me->getControl("help").setText(boomNames[std::min<short>(n,preset_booms)]);
	});
	bool made_choice = pic_dlg.show(cur);
	size_t item_hit = pic_dlg.getPicChosen();
	return made_choice ? item_hit : prev;
}

pic_num_t choose_status_effect(short cur, bool party, cDialog* parent) {
	static const char*const status[] = {
		"Poisoned Weapon", "Bless/Curse", "Poison", "Haste/Slow", "Invulnerable",
		"Magic Resist/Amplify", "Webs", "Disease", "Sanctuary", "Dumbfounding/Enlightening",
		"Martyr's Shield", "Sleep/Hyperactivity", "Paralysis", "Acid", "Forcecage",
	};
	static const char*const pstatus[] = {"Stealth", "Flight", "Detect Life", "Firewalk"};
	static const std::vector<pic_num_t> status_pics = {4,2,0,6,5,9,10,11,12,13,14,15,16,17,20};
	static const std::vector<pic_num_t> pstatus_pics = {26,23,24,25};
	short prev = cur;
	if(cur < 0 || cur >= (party ? pstatus_pics : status_pics).size()) cur = 0;
	cPictChoice pic_dlg(party ? pstatus_pics : status_pics, PIC_STATUS, parent);
	pic_dlg->getControl("prompt").setText("Select a status effect:");
	pic_dlg->getControl("help").setText((party ? pstatus : status)[cur]);
	pic_dlg.attachSelectHandler([party](cPictChoice& me, int n) {
		me->getControl("help").setText((party ? pstatus : status)[n]);
	});
	bool made_choice = pic_dlg.show(cur);
	size_t item_hit = pic_dlg.getSelected();
	return made_choice ? item_hit : prev;
}

snd_num_t choose_sound(short cur, cDialog* parent, std::string title) {
	if(cur < 0) cur = 0;
	StringList snd_names = *ResMgr::strings.get("sound-names");
	std::copy(scenario.snd_names.begin(), scenario.snd_names.end(), std::back_inserter(snd_names));
	cStringChoice snd_dlg(snd_names, title, parent);
	snd_dlg.attachSelectHandler([](cStringChoice&, int n) {
		play_sound(-n);
	});
	return snd_dlg.show(cur);
}

static node_function_t get_field_function(const cSpecial& spec, const std::string& field) {
	if(field == "sdf1") return (*spec.type).sdf1(spec);
	else if(field == "sdf2") return (*spec.type).sdf2(spec);
	else if(field == "msg1") return (*spec.type).msg1(spec);
	else if(field == "msg2") return (*spec.type).msg2(spec);
	else if(field == "msg3") return (*spec.type).msg3(spec);
	else if(field == "pict") return (*spec.type).pic(spec);
	else if(field == "pictype") return (*spec.type).pictype(spec);
	else if(field == "x1a") return (*spec.type).ex1a(spec);
	else if(field == "x1b") return (*spec.type).ex1b(spec);
	else if(field == "x1c") return (*spec.type).ex1c(spec);
	else if(field == "x2a") return (*spec.type).ex2a(spec);
	else if(field == "x2b") return (*spec.type).ex2b(spec);
	else if(field == "x2c") return (*spec.type).ex2c(spec);
	else if(field == "jump") return (*spec.type).jump(spec);
	return node_function_t();
}

static bool edit_spec_enc_value(cDialog& me, std::string item_hit, node_stack_t& edit_stack) {
	std::string field = item_hit.substr(0, item_hit.find_first_of('-'));
	const cSpecial& spec = edit_stack.top().node;
	node_function_t fcn = get_field_function(spec, field);
	short val = me[field].getTextAsNum(), store;
	switch(fcn.button) {
		case eSpecPicker::MSG_PAIR:
			store = me["msg2"].getTextAsNum();
			edit_spec_text(fcn.force_global ? STRS_SCEN : eStrMode(edit_stack.top().mode), &val, &store, &me);
			me["msg2"].setTextToNum(store);
			store = val;
			break;
		case eSpecPicker::MSG_SINGLE:
			edit_spec_text(fcn.force_global ? STRS_SCEN : eStrMode(edit_stack.top().mode), &val, nullptr, &me);
			store = val;
			break;
		case eSpecPicker::MSG_SEQUENCE:
			store = val;
			edit_dialog_text(fcn.force_global ? STRS_SCEN : eStrMode(edit_stack.top().mode), &store, &me);
			break;
		case eSpecPicker::NODE: {
			short mode = fcn.force_global ? 0 : edit_stack.top().mode;
			store = val < 0 ? get_fresh_spec(mode) : val;
			me[field].setTextToNum(store);
			save_spec_enc(me, edit_stack);
			cSpecial* node_to_change_to = nullptr;
			if(mode == 0)
				node_to_change_to = &scenario.scen_specials[store];
			else if(mode == 1)
				node_to_change_to = &current_terrain->specials[store];
			else if(mode == 2)
				node_to_change_to = &town->specials[store];
			if (node_to_change_to) {
				if(node_to_change_to->pic < 0)
					node_to_change_to->pic = 0;
				edit_stack.push({store,mode,*node_to_change_to});
			}
			put_spec_enc_in_dlog(me, edit_stack);
			me["back"].show();
			return true;
		}
		case eSpecPicker::STRING: {
			std::string title;
			switch(fcn.str_type) {
				case STRT_SPELL_PAT: title = "Which spell pattern?"; break;
				case STRT_ITEM: title = "Which item?"; break;
				case STRT_SPEC_ITEM: title = "Which special item?"; break;
				case STRT_TER: title = "Which terrain?"; break;
				case STRT_MONST: title = "Which monster?"; break;
				case STRT_MONST_STAT: title = "Which statistic?"; break;
				case STRT_ALCHEMY: title = "Which recipe?"; break;
				case STRT_MAGE: title = "Which spell?"; break;
				case STRT_PRIEST: title = "Which spell?"; break;
				case STRT_SKILL: title = "Which statistic?"; break;
				case STRT_TRAIT: title = "Which trait?"; break;
				case STRT_RACE: title = "Which species?"; break;
				case STRT_TOWN: title = "Which town?"; break;
				case STRT_BUTTON: title = "Which button?"; break;
				case STRT_PICT: title = "Which picture type?"; break;
				case STRT_TRAP: title = "What trap type?"; break;
				case STRT_CMP: title = "What comparison method?"; break;
				case STRT_ACCUM: title = "What accumulation method?"; break;
				case STRT_ATTITUDE: title = "What attitude?"; break;
				case STRT_STAIR: title = "Which stairway text?"; break;
				case STRT_LIGHT: title = "What lighting type?"; break;
				case STRT_SHOP: title = "Which shop?"; break;
				case STRT_COST_ADJ: title = "What cost adjust?"; break;
				case STRT_CONTEXT: title = "What context?"; break;
				case STRT_ENCHANT: title = "Which enchantment?"; break;
				case STRT_POS_MODE: title = "Select positioning mode:"; break;
				case STRT_STAIR_MODE: title = "Select trigger limitations:"; break;
				case STRT_STATUS: title = "Select status:"; break;
				case STRT_QUEST: title = "Select a quest:"; break;
				case STRT_QUEST_STATUS: title = "Select the quest's status:"; break;
				case STRT_DIR: title = "Select the direction:"; break;
				case STRT_DEBUG_PRINT: title = "Choose what to print:"; break;
				case STRT_TARG_TYPE: title = "Select restrictions on chosen characters:"; break;
				case STRT_TARG_MODE: title = "Choose how a target is selected:"; break;
				case STRT_ID_MODE: title = "Choose how to identify the item:"; break;
				case STRT_CURSE_MODE: title = "Choose whether the item should be cursed:"; break;
				case STRT_EQUIP_MODE: title = "Choose whether and how to attempt to equip the item:"; break;
				case STRT_CMP_MODE: title = "Choose the type of comparison to make:"; break;
				case STRT_PATH: title = "What path shape?"; break;
				case STRT_SPELL_PAT_MODE: title = "What kind of booms?"; break;
				case STRT_LABEL_ALIGN: title = "Choose vertical alignment:"; break;
				default: title = "Title not set for this string type!!!"; break;
			}
			store = choose_text(fcn.str_type, val + fcn.adjust, &me, title) - fcn.adjust;
		} break;
		case eSpecPicker::PICTURE: {
			ePicType type = fcn.pic_type;
			if(type == PIC_NONE) {
				short pictype = me["pictype"].getTextAsNum();
				if(pictype < 0) {
					me["pictype"].setTextToNum(PIC_DLOG);
					type = PIC_DLOG;
				} else type = ePicType(pictype);
			}
			store = choose_graphic(val, type, &me);
			if(store < 0) store = val;
		} break;
		case eSpecPicker::FIELD: store = choose_field_type(val, &me, fcn.augmented); break;
		case eSpecPicker::DAMAGE_TYPE: store = choose_damage_type(val, &me, true); break;
		case eSpecPicker::EXPLOSION: store = choose_boom_type(val, &me); break;
		case eSpecPicker::STATUS: store = choose_status_effect(val, false, &me); break;
		case eSpecPicker::STATUS_PARTY: store = choose_status_effect(val, true, &me); break;
		case eSpecPicker::SOUND: store = choose_sound(val, &me); break;
		case eSpecPicker::NONE: return false;
	}
	me[field].setTextToNum(store);
	return true;
}

// mode - 0 scen 1 - out 2 - town
bool edit_spec_enc(short which_node,short mode,cDialog* parent) {
	using namespace std::placeholders;
	cSpecial the_node;
	node_stack_t edit_stack;
	
	if(mode == 0) {
		if(which_node >= scenario.scen_specials.size()) {
			showError("That special node does not exist. You can create a new node by setting the field to -1 and trying again.", parent);
			return false;
		}
		if(scenario.scen_specials[which_node].pic < 0)
			scenario.scen_specials[which_node].pic = 0;
		the_node = scenario.scen_specials[which_node];
	} else if(mode == 1) {
		if(which_node >= current_terrain->specials.size()) {
			showError("That special node does not exist. You can create a new node by setting the field to -1 and trying again.", parent);
			return false;
		}
		if(current_terrain->specials[which_node].pic < 0)
			current_terrain->specials[which_node].pic = 0;
		the_node = current_terrain->specials[which_node];
	} else if(mode == 2) {
		if(which_node >= town->specials.size()) {
			showError("That special node does not exist. You can create a new node by setting the field to -1 and trying again.", parent);
			return false;
		}
		if(town->specials[which_node].pic < 0)
			town->specials[which_node].pic = 0;
		the_node = town->specials[which_node];
	}

	cDialog special(*ResMgr::dialogs.get("edit-special-node"),parent);
	special.attachClickHandlers(std::bind(commit_spec_enc, _1, _2, std::ref(edit_stack)), {"okay", "back"});
	special.attachClickHandlers(std::bind(edit_spec_enc_type, _1, _2, std::ref(edit_stack)), {
		"general", "oneshot", "affectpc", "ifthen", "town", "out", "rect"
	});
	special.attachClickHandlers(std::bind(edit_spec_enc_value, _1, _2, std::ref(edit_stack)), {
		"msg1-edit", "msg2-edit", "msg3-edit", "pict-edit", "pictype-edit", "jump-edit",
		"x1a-edit", "x1b-edit", "x1c-edit", "x2a-edit", "x2b-edit", "x2c-edit",
		"sdf1-edit", "sdf2-edit",
	});
	special["cancel"].attachClickHandler(std::bind(discard_spec_enc, _1, std::ref(edit_stack)));
	special["node-help"].attachClickHandler([&edit_stack](cDialog& me, std::string item_hit, eKeyMod mods) {
		eSpecType type = edit_stack.top().node.type;
		const std::string& str = (*type).descr();
		// TODO: This is the same dialog as give_help(), the only difference being that we don't have a string number!
		cStrDlog display_help(str,"","Instant Help",24,PIC_DLOG, &me);
		display_help.setSound(57);
		display_help.show();
		return true;
	});
	
	special["back"].hide();
	edit_stack.push({which_node,mode,the_node});
	put_spec_enc_in_dlog(special, edit_stack);
	
	special.setResult(false);
	special.run();
	return special.getResult<bool>();
}

short get_fresh_spec(short which_mode) {
	cSpecial store_node;
	size_t num_specs = 0;
	switch(which_mode) {
		case 0: num_specs = scenario.scen_specials.size(); break;
		case 1: num_specs = current_terrain->specials.size(); break;
		case 2: num_specs = town->specials.size(); break;
	}
	
	for(size_t i = 0; i < num_specs; i++) {
		if(which_mode == 0)
			store_node = scenario.scen_specials[i];
		if(which_mode == 1)
			store_node = current_terrain->specials[i];
		if(which_mode == 2)
			store_node = town->specials[i];
		if(store_node.type == eSpecType::NONE && store_node.jumpto == -1 && store_node.pic < 0)
			return i;
	}
	
	switch(which_mode) {
		case 0: scenario.scen_specials.emplace_back(); break;
		case 1: current_terrain->specials.emplace_back(); break;
		case 2: town->specials.emplace_back(); break;
	}
	
	return num_specs;
}

static bool edit_spec_text_event_filter(cDialog& me, std::string item_hit, eStrMode str_mode, short* str1, short* str2) {
	std::string str;
	
	if(item_hit == "okay") {
		str = me["str1"].getText();
		if(!str.empty()) {
			if(*str1 < 0) {
				size_t n = num_strs(str_mode);
				for(short i = 0; i < n; i++) {
					std::string& str = fetch_str(str_mode, i);
					if(!str.empty() && str[0] == '*') {
						*str1 = i;
						break;
					}
				}
				if(*str1 < 0)
					*str1 = n;
			}
			fetch_str(str_mode, *str1) = str;
		}
		str = "";
		if(me.hasControl("str2")) str = me["str2"].getText();
		if(str2 != nullptr && !str.empty()) {
			if(*str2 < 0) {
				size_t n = num_strs(str_mode);
				for(short i = 0; i < n; i++) {
					std::string& str = fetch_str(str_mode, i);
					if(!str.empty() && str[0] == '*') {
						*str2 = i;
						break;
					}
				}
				if(*str2 < 0)
					*str2 = n;
			}
			fetch_str(str_mode, *str2) = str;
		}
		me.toast(true);
	} else me.toast(false);
	return true;
}

// mode 0 - scen 1 - out 2 - town
void edit_spec_text(eStrMode mode,short *str1,short *str2,cDialog* parent) {
	using namespace std::placeholders;
	
	std::string dlog_id = str2 ? "edit-special-text" : "edit-special-text-sm";
	cDialog edit(*ResMgr::dialogs.get(dlog_id), parent);
	edit.attachClickHandlers(std::bind(edit_spec_text_event_filter, _1, _2, mode, str1, str2), {"okay", "cancel"});
	
	if(*str1 >= num_strs(mode))
		*str1 = -1;
	if(*str1 >= 0){
		edit["str1"].setText(fetch_str(mode, *str1));
	}
	if(str2 != nullptr) {
		if(*str2 >= num_strs(mode))
			*str2 = -1;
		if(*str2 >= 0){
			edit["str2"].setText(fetch_str(mode, *str2));
		}
	}
	edit.run();
}

static bool edit_dialog_text_event_filter(cDialog& me, std::string item_hit, eStrMode str_mode, short* str1){
	std::string str;
	
	if(item_hit == "okay") {
		for(short i = 0; i < 6; i++) {
			std::string id = "str" + std::to_string(i + 1);
			str = me[id].getText();
			if(i == 0 && str.empty()) break;
			fetch_str(str_mode, *str1 + i) = str;
		}
		me.toast(true);
	} else me.toast(false);
	return true;
}

// mode 0 - scen 1 - out 2 - town
void edit_dialog_text(eStrMode mode,short *str1,cDialog* parent) {
	if(*str1 >= num_strs(mode))
		*str1 = -1;
	else if(*str1 + 5 >= num_strs(mode))
		ensure_str(mode, *str1 + 5);
	// first, assign the 6 strs for the dialog.
	if(*str1 < 0) {
		size_t n = num_strs(mode);
		for(short i = 0; i < n; i++) {
			short n = 0;
			for(short j = i; j < i + 6; j++, n++) {
				std::string str = fetch_str(mode, j);
				if(!str.empty() && str[0] != '*')
					break;
			}
			if(n == 6) {
				*str1 = i;
				break;
			}
		}
		if(*str1 >= 0) {
			for(short i = *str1; i < *str1 + 6; i++)
				fetch_str(mode, i).clear();
		}
	}
	if(*str1 < 0) {
		*str1 = num_strs(mode);
		ensure_str(mode, *str1 + 5);
	}
	
	using namespace std::placeholders;
	cDialog edit(*ResMgr::dialogs.get("edit-dialog-text"),parent);
	edit.attachClickHandlers(std::bind(edit_dialog_text_event_filter, _1, _2, mode, str1), {"okay", "cancel"});
	
	if(*str1 >= 0) {
		for(short i = 0; i < 6; i++) {
			std::string id = "str" + std::to_string(i + 1);
			edit[id].setText(fetch_str(mode, *str1 + i));
		}
	}
	
	edit.run();
}

static bool edit_special_num_event_filter(cDialog& me, std::string item_hit, short spec_mode) {
	size_t num_specs = 0;
	switch(spec_mode) {
		case 0: num_specs = scenario.scen_specials.size(); break;
		case 1: num_specs = current_terrain->specials.size(); break;
		case 2: num_specs = town->specials.size(); break;
	}
	
	if(item_hit == "cancel") me.setResult<short>(-1);
	else if(item_hit == "okay") {
		short i = me["num"].getTextAsNum();
		if(i < 0 || i >= num_specs) {
			std::string range = num_specs
				? "The available range is 0 to " + std::to_string(num_specs - 1) + "."
				: "There are currently no nodes defined at all.";
			showWarning("There is no special node with that number. " + range,"The node has been set anyway. To create it, select Edit Special Nodes from the menu, scroll to the bottom, and select Create new Node.",&me);
		}
		me.setResult(i);
		me.toast(true);
	} me.toast(false);
	return true;
}

short edit_special_num(short mode,short what_start) {
	using namespace std::placeholders;
	
	cDialog edit(*ResMgr::dialogs.get("edit-special-assign"));
	edit.attachClickHandlers(std::bind(edit_special_num_event_filter, _1, _2, mode), {"okay", "cancel"});
	
	edit["num"].setTextToNum(what_start);
	
	edit.run();
	
	return edit.getResult<short>();
}

static bool edit_scen_intro_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	if(item_hit == "okay") {
		scenario.intro_pic = me["picnum"].getTextAsNum();
		if(scenario.intro_pic > 29) {
			showError("Intro picture number is out of range.","",&me);
			return true;
		}
		for(short i = 0; i < scenario.intro_strs.size(); i++) {
			std::string id = "str" + std::to_string(i + 1);
			scenario.intro_strs[i] = me[id].getText();
		}
		me.toast(true);
	} else if(item_hit == "cancel") {
		me.toast(false);
	} else if(item_hit == "choose") {
		pic_num_t pic = me["picnum"].getTextAsNum();
		short i = choose_graphic(pic,PIC_SCEN,&me);
		if(i != NO_PIC) {
			me["picnum"].setTextToNum(i);
			dynamic_cast<cPict&>(me["pic"]).setPict(i);
		}
	}
	return true;
}

void edit_scen_intro() {
	cDialog edit(*ResMgr::dialogs.get("edit-intro"));
	edit.attachClickHandlers(edit_scen_intro_event_filter, {"okay", "cancel", "choose"});
	
	edit["picnum"].setTextToNum(scenario.intro_pic);
	for(short i = 0; i < scenario.intro_strs.size(); i++) {
		std::string id = "str" + std::to_string(i + 1);
		edit[id].setText(scenario.intro_strs[i]);
	}
	dynamic_cast<cPict&>(edit["pic"]).setPict(scenario.intro_pic);
	
	edit.run();
}
