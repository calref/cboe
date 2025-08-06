/*
 *  dlogutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "3choice.hpp"

#include <sstream>
#include <functional>

#include "mathutil.hpp"
#include "dialogxml/widgets/message.hpp"
#include "dialogxml/widgets/pict.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "tools/cursors.hpp"

cThreeChoice::cThreeChoice
  (std::vector<std::string>& strings, cBasicButtonType button, pic_num_t pic, ePicType t, cDialog* parent)
  : cChoiceDlog(parent), type(t){
	cDialog& parentDlog = *operator->();
	parentDlog.setBg(cDialog::BG_DARK);
	parentDlog.setDefTextClr(sf::Color::White);
	if(type == PIC_CUSTOM_DLOG_LG || type == PIC_DLOG_LG || type == PIC_SCEN_LG)
		init_strings(strings,86);
	else
		init_strings(strings,50);
	init_buttons(button, null_btn, null_btn);
	init_pict(pic);
	parentDlog.recalcRect();
}

cThreeChoice::cThreeChoice
  (std::vector<std::string>& strings, std::array<cBasicButtonType, 3>& buttons, pic_num_t pic, ePicType t, cDialog* parent)
  : cChoiceDlog(parent), type(t){
	cDialog& parentDlog = *operator->();
	parentDlog.setBg(cDialog::BG_DARK);
	parentDlog.setDefTextClr(sf::Color::White);
	if(type == PIC_CUSTOM_DLOG_LG || type == PIC_DLOG_LG || type == PIC_SCEN_LG)
		init_strings(strings,86);
	else
		init_strings(strings,50);
	init_buttons(buttons[1], buttons[2], buttons[3]);
	init_pict(pic);
	parentDlog.recalcRect();
}

cThreeChoice::cThreeChoice
  (std::vector<std::string>& strings,std::array<short, 3>& buttons,pic_num_t pic,ePicType t,cDialog* parent)
  : cChoiceDlog(parent), type(t){
	cDialog& parentDlog = *operator->();
	parentDlog.setBg(cDialog::BG_DARK);
	parentDlog.setDefTextClr(sf::Color::White);
	if(type == PIC_CUSTOM_DLOG_LG || type == PIC_DLOG_LG || type == PIC_SCEN_LG)
		init_strings(strings,86);
	else
		init_strings(strings,50);
	std::array<cBasicButtonType, 3> buttonDefs;
	int i = 0;
	for(short j : buttons) {
		if(j < 0) buttonDefs[i++] = null_btn;
		else buttonDefs[i++] = basic_buttons[j];
	}
	init_buttons(buttonDefs[0], buttonDefs[1], buttonDefs[2]);
	init_pict(pic);
	parentDlog.recalcRect();
}

void cThreeChoice::init_strings(std::vector<std::string>& strings, unsigned short left){
	TextStyle style;
	rectangle cur_text_rect = {2, left, 0, 0};
	size_t total_len = 0, str_width, str_height;
	for(unsigned int i = 0; i < strings.size(); i++)
		total_len += string_length(strings[i], style);
	total_len = total_len * 12;
	str_width = sqrt(total_len) + 20;
	//print_nums(0,total_len,str_width);
	if(str_width < 340)
		str_width = 340;
	cur_text_rect.right = cur_text_rect.left + str_width;
	cDialog* me = operator->();
	for(unsigned int j = 0; j < strings.size(); j++){
		std::ostringstream sout;
		sout << "str" << j + 1;
		str_height = ((string_length(strings[j], style) + 60) / str_width) * 12 + 16;
		cur_text_rect.bottom = cur_text_rect.top + str_height;
		cTextMsg* str = new cTextMsg(*me);
		str->setText(strings[j]);
		str->setFixed(true, false);
		me->add(str, cur_text_rect, sout.str());
		cur_text_rect.top = cur_text_rect.bottom + 8;
	}
	buttons_right = cur_text_rect.right + 30;
	buttons_top = cur_text_rect.top;
}

void cThreeChoice::init_buttons(cBasicButtonType btn1, cBasicButtonType btn2, cBasicButtonType btn3){
	using namespace std::placeholders;
	rectangle cur_btn_rect = {buttons_top,0,0,buttons_right};
	if(btn1) btns[0] = btn1;
	if(btn2) btns[1] = btn2;
	if(btn3) btns[2] = btn3;
	cDialog* me = operator->();
	// NOTE: Buttons used to be added right-to-left, resulting in the Leave button on the right. I've
	// reversed it.
	for(int i : {1, 2, 0}){
		if(!btns[i]) continue;
		std::ostringstream sout;
		sout << "btn" << i + 1;
		cButton* btn = new cButton(*me);
		btn->attachKey(btns[i]->defaultKey);
		btn->setText(btns[i]->label);
		btn->setBtnType(btns[i]->type);
		btn->attachClickHandler(std::bind(&cThreeChoice::onClick,this,_1,_2));
		switch(btns[i]->type){
			case BTN_HELP:
				cur_btn_rect.bottom = cur_btn_rect.top + 13;
				break;
			case BTN_TINY:
			case BTN_LED: // BTN_LED should never occur though
				cur_btn_rect.bottom = cur_btn_rect.top + 10;
				break;
			case BTN_TALL:
			case BTN_TRAIT:
				cur_btn_rect.bottom = cur_btn_rect.top + 40;
				break;
			case BTN_PUSH:
				cur_btn_rect.bottom = cur_btn_rect.top + 30;
				break;
			default: // in fact, this case should be the only one reachable, ideally
				cur_btn_rect.bottom = cur_btn_rect.top + 23;
		}
		switch(btns[i]->type){
			case BTN_SM:
				cur_btn_rect.left = cur_btn_rect.right - 23;
				break;
			case BTN_HELP:
				cur_btn_rect.left = cur_btn_rect.right - 16;
				break;
			case BTN_TINY:
			case BTN_LED:
				cur_btn_rect.left = cur_btn_rect.right - 14;
				break;
			case BTN_PUSH:
				cur_btn_rect.left = cur_btn_rect.right - 30;
				break;
			case BTN_LG:
				cur_btn_rect.left = cur_btn_rect.right - 102;
				break;
			default:
				cur_btn_rect.left = cur_btn_rect.right - 63;
		}
		me->add(btn, cur_btn_rect, sout.str());
		cur_btn_rect.right = cur_btn_rect.left - 4;
	}
	// Add a record button and hide it
	cButton* record = new cButton(*me);
	record->setText("Record");
	record->attachClickHandler(std::bind(&cThreeChoice::onRecord, this, _2));
	cur_btn_rect = {buttons_top,10,buttons_top + 23,buttons_right};
	me->add(record, cur_btn_rect, "record");
	record->hide();
}

void cThreeChoice::init_pict(pic_num_t pic){
	cDialog* me = operator->();
	cPict* pic_ctrl = new cPict(*me);
	pic_ctrl->setBounds({8,8,8,8});
	pic_ctrl->setPict(pic,type);
	pic_ctrl->recalcRect();
	me->add(pic_ctrl, pic_ctrl->getBounds(), "pict");
}

std::string cThreeChoice::show(){
	std::string result = cChoiceDlog::show();
	if(result == "btn1") return btns[0]->label;
	else if(result == "btn2") return btns[1]->label;
	else if(result == "btn3") return btns[2]->label;
	return "**ERROR**"; // shouldn't be reached
}

short custom_choice_dialog(std::array<std::string, 6>& strs,short pic_num,ePicType pic_type,std::array<short, 3>& buttons,bool anim_pict,short anim_loops, int anim_fps, cUniverse* univ, cDialog* parent) {
	set_cursor(sword_curs);

	std::vector<std::string> vec(strs.begin(), strs.end());
	// Strip off trailing empty strings
	while(!vec.empty() && vec.back().empty())
		vec.pop_back();
	cThreeChoice customDialog(vec, buttons, pic_num, pic_type, parent);
	if(anim_pict)
		setup_dialog_pict_anim(*(customDialog.operator->()), "pict", anim_loops, anim_fps);

	if(univ != nullptr){
		customDialog.setRecordHandler([vec, univ](cDialog& dlg) -> void {
			std::string combined;
			for(std::string msg : vec){
				if(!msg.empty()){
					if(!combined.empty()){
						combined += " ||";
					}
					combined += msg;
				}
			}
			// It's not necessarily a NOTE_SCEN and location shouldn't be empty,
			// but these things aren't actually shown to the party in the encounter notes dialog.
			if(univ->party.record(NOTE_SCEN, combined, "")){
				give_help(58,0,dlg);
				// TODO ASB is only in game source, but this function is in common source so the scenario editor can preview dialogs :(
				/*
				add_string_to_buf("Added to encounter notes.");
				*/
			}
		});
	}
	std::string item_hit = customDialog.show();

	for(int i = 0; i < 3; i++) {
		auto& btn = basic_buttons[buttons[i]];
		if(item_hit == btn.label)
			return i + 1;
	}
	return -1;
}

short once_dialog(cUniverse& univ, cSpecial& spec, eSpecCtxType cur_type, cDialog* parent) {
	std::array<std::string, 6> strs;
	std::array<short, 3> buttons = {-1,-1,-1};

	if(spec.m1 < 0)
		return -1;

	univ.get_strs(strs, cur_type, spec.m1);
	if(spec.m3 > 0) {
		// The first button defaults to OK if toggle is on
		buttons[0] = 1;
		buttons[1] = spec.ex1a;
		buttons[2] = spec.ex2a;
		// If button 2 and/or button 3 are provided, the first button becomes a Leave button!
		if((spec.ex1a >= 0) || (spec.ex2a >= 0))
			buttons[0] = 9;
	}
	if(spec.m3 <= 0) {
		buttons[1] = spec.ex1a;
		buttons[2] = spec.ex2a;
	}
	if((buttons[0] < 0) && (buttons[1] < 0)) {
		showError("Dialog box ended up with no buttons.");
		return -1;
	}
	return custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons, true, spec.ex1c, spec.ex2c, &univ, parent);
}

cThreeChoice& cThreeChoice::setRecordHandler(record_callback_t rec){
	if(rec == nullptr){
		hasRecord = false;
		dlg["record"].hide();
	}else{
		hasRecord = true;
		rec_f = rec;
		dlg["record"].show();
	}
	return *this;
}

bool cThreeChoice::onRecord(std::string id){
	if(hasRecord) rec_f(dlg);
	else dlg[id].hide();
	return hasRecord;
}