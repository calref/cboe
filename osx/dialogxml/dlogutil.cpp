/*
 *  dlogutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */
#define BTNS_DEFINED

#include <Carbon/Carbon.h>
#include <sstream>
#include <algorithm>

#include <boost/bind.hpp>
#include "dialog.h"
#include "dlogutil.h"
#include "mathutil.h"

#include "dlogutil.buttons.h" // must be included here and only here

const size_t cPictChoice::per_page = 36;

cPictChoice::cPictChoice(std::vector<pic_num_t>& pics,ePicType t,cDialog* parent) : dlg("choose-pict.xml",parent), type(t) {
	dlg["left"].attachClickHandler(boost::bind(&cPictChoice::onLeft,this,_1,_2));
	dlg["right"].attachClickHandler(boost::bind(&cPictChoice::onRight,this,_1,_2));
	dlg["done"].attachClickHandler(boost::bind(&cPictChoice::onOkay,this,_1,_2));
	dlg["cancel"].attachClickHandler(boost::bind(&cPictChoice::onCancel,this,_1,_2));
	picts = pics;
	sort(picts.begin(),picts.end());
}

cPictChoice::cPictChoice(
		std::vector<pic_num_t>::iterator begin,
		std::vector<pic_num_t>::iterator end,
		ePicType t,
		cDialog* parent
	) : dlg("choose-pict.xml",parent), type(t) {
	dlg["left"].attachClickHandler(boost::bind(&cPictChoice::onLeft,this,_1,_2));
	dlg["right"].attachClickHandler(boost::bind(&cPictChoice::onRight,this,_1,_2));
	dlg["done"].attachClickHandler(boost::bind(&cPictChoice::onOkay,this,_1,_2));
	dlg["cancel"].attachClickHandler(boost::bind(&cPictChoice::onCancel,this,_1,_2));
	copy(begin,end,picts.begin());
	sort(picts.begin(),picts.end());
}
	
pic_num_t cPictChoice::show(pic_num_t fallback, pic_num_t cur_sel){
	dlg.setResult(fallback);
	cur = cur_sel;
	page = cur / per_page;
	fillPage();
	dlg.run();
	return dlg.getResult<pic_num_t>();
}// returns the _number_ of the chosen picture, _not_ the index; there's no way to distinguish between duplicates
// returns fallback if the user cancels

void cPictChoice::fillPage(){
	cLedGroup& group = dynamic_cast<cLedGroup&>(dlg["group"]);
	group.setSelected(""); // unselect all LEDs, since the currently selected one may be on another page
	for(int i = 0; i < per_page; i++){
		std::ostringstream sout;
		sout << "led" << i + 1;
		if(page * per_page + i >= picts.size())
			dlg[sout.str()].hide();
		else
			dlg[sout.str()].show();
		if(page * per_page + i == cur)
			group.setSelected(sout.str());
		sout.str("");
		sout << "pic" << i + 1;
		cPict& pic = dynamic_cast<cPict&>(dlg[sout.str()]);
		if(page * per_page + i < picts.size()){
			pic.show();
			pic.setPict(picts[per_page * page + i], type);
		}else pic.hide();
	}
}

bool cPictChoice::onLeft(cDialog& m, std::string ide){
	if(page == 0) page = picts.size() / per_page;
	else page--;
	fillPage();
	return true;
}

bool cPictChoice::onRight(cDialog& me, std::string id){
	if(page == picts.size() / per_page) page = 0;
	else page++;
	fillPage();
	return true;
}

bool cPictChoice::onCancel(cDialog& me, std::string id){
	me.toast();
	return true;
}

bool cPictChoice::onOkay(cDialog& me, std::string id){
	dlg.setResult(picts[cur]);
	me.toast();
	return true;
}

const size_t cStringChoice::per_page = 40;

cStringChoice::cStringChoice(
		std::vector<std::string>& strs,
		cDialog* parent
	) : dlg("choose-string.xml",parent) {
	dlg["left"].attachClickHandler(boost::bind(&cStringChoice::onLeft,this,_1,_2));
	dlg["right"].attachClickHandler(boost::bind(&cStringChoice::onRight,this,_1,_2));
	dlg["done"].attachClickHandler(boost::bind(&cStringChoice::onOkay,this,_1,_2));
	dlg["cancel"].attachClickHandler(boost::bind(&cStringChoice::onCancel,this,_1,_2));
	strings = strs;
}

cStringChoice::cStringChoice(
		std::vector<std::string>::iterator begin,
		std::vector<std::string>::iterator end,
		cDialog* parent
	) : dlg("choose-string.xml",parent) {
	dlg["left"].attachClickHandler(boost::bind(&cStringChoice::onLeft,this,_1,_2));
	dlg["right"].attachClickHandler(boost::bind(&cStringChoice::onRight,this,_1,_2));
	dlg["done"].attachClickHandler(boost::bind(&cStringChoice::onOkay,this,_1,_2));
	dlg["cancel"].attachClickHandler(boost::bind(&cStringChoice::onCancel,this,_1,_2));
	copy(begin,end,strings.begin());
}

size_t cStringChoice::show(std::string select){
	dlg.setResult(strings.size());
	std::vector<std::string>::iterator iter = find(strings.begin(),strings.end(),select);
	cur = iter - strings.begin();
	page = cur / per_page;
	fillPage();
	dlg.run();
	return dlg.getResult<pic_num_t>();
}// returns the _index_ of the chosen string, relative to begin if initialized from a range
// returns strs.size() if the user cancels

void cStringChoice::fillPage(){
	cLedGroup& group = dynamic_cast<cLedGroup&>(dlg["group"]);
	group.setSelected(""); // unselect all LEDs, since the currently selected one may be on another page
	for(unsigned int i = 0; i < per_page; i++){
		std::ostringstream sout;
		sout << "led" << i + 1;
		if(page * per_page + i >= strings.size()){
			dlg[sout.str()].hide();
			continue;
		}else
			dlg[sout.str()].show();
		if(page * per_page + i == cur)
			group.setSelected(sout.str());
		if(page * per_page + i < strings.size()){
			dlg[sout.str()].setText(strings[per_page * page + i]);
		}
	}
}

bool cStringChoice::onLeft(cDialog& me __attribute__((unused)), std::string id __attribute__((unused))){
	if(page == 0) page = strings.size() / per_page;
	else page--;
	fillPage();
	return true;
}

bool cStringChoice::onRight(cDialog& me __attribute__((unused)), std::string id __attribute__((unused))){
	if(page == strings.size() / per_page) page = 0;
	else page++;
	fillPage();
	return true;
}

bool cStringChoice::onCancel(cDialog& me, std::string id __attribute__((unused))){
	me.toast();
	return true;
}

bool cStringChoice::onOkay(cDialog& me, std::string id __attribute__((unused))){
	dlg.setResult(cur);
	me.toast();
	return true;
}

cChoiceDlog::cChoiceDlog(std::string file, std::vector<std::string> buttons, cDialog* p) : dlg(file, p) {
	std::vector<std::string>::iterator iter = buttons.begin();
	while(iter != buttons.end()){
		dlg[*iter].attachClickHandler(boost::bind(&cChoiceDlog::onClick,this,_1,_2));
		iter++;
	}
}

cChoiceDlog::cChoiceDlog(std::vector<std::string> buttons, cDialog* p) : dlg(p) {
	std::vector<std::string>::iterator iter = buttons.begin();
	while(iter != buttons.end()){
		dlg[*iter].attachClickHandler(boost::bind(&cChoiceDlog::onClick,this,_1,_2));
		iter++;
	}
}

// so that the caller can set up aspects of the dialog if necessary
cDialog* cChoiceDlog::operator->(){
	return &dlg;
}

std::string cChoiceDlog::show(){
	dlg.run();
	return dlg.getResult<std::string>();
}

bool cChoiceDlog::onClick(cDialog& me, std::string id){
	me.setResult(id);
	me.toast();
	return true;
}

template<bool b> bbtt<b>::operator bool(){
	return b;
}

template<bool b> bbtt<b>& bbtt<b>::operator=(bbtt<!b>&){
	return *this;
}

cThreeChoice::cThreeChoice
  (std::vector<std::string> strings,short btn1,short btn2,short btn3,pic_num_t pic,ePicType t,cDialog* parent)
  : cChoiceDlog(/*cThreeChoice<type>::getFileName(strings.size()),*/ std::vector<std::string>(), parent), type(t){
	if(type == PIC_CUSTOM_DLOG_LG || type == PIC_DLOG_LG || type == PIC_SCEN_LG)
		init_strings(strings,86);
	else
		init_strings(strings,50);
	if(btn1 < 0){
		if(btn2 < 0){
			if(btn3 < 0)
				init_buttons(basic_buttons[available_btns[63]],null_btn,null_btn);
			else init_buttons(null_btn,null_btn,basic_buttons[available_btns[btn3]]);
		}else{
			if(btn3 < 0)
				init_buttons(null_btn,basic_buttons[available_btns[btn2]],null_btn);
			else init_buttons(null_btn,basic_buttons[available_btns[btn2]],basic_buttons[available_btns[btn3]]);
		}
	}else{
		if(btn2 < 0){
			if(btn3 < 0)
				init_buttons(basic_buttons[available_btns[btn1]],null_btn,null_btn);
			else init_buttons(basic_buttons[available_btns[btn1]],null_btn,basic_buttons[available_btns[btn3]]);
		}else{
			if(btn3 < 0)
				init_buttons(basic_buttons[available_btns[btn1]],basic_buttons[available_btns[btn2]],null_btn);
			else init_buttons(basic_buttons[available_btns[btn1]],basic_buttons[available_btns[btn2]],
							  basic_buttons[available_btns[btn3]]);
		}
	}
	init_pict(pic);
	operator->()->recalcRect();
}

void cThreeChoice::init_strings(std::vector<std::string>& strings, unsigned short left){
	Rect cur_text_rect = {2, left, 0, 0};
	size_t total_len, str_width, str_height;
	for (unsigned int i = 0; i < strings.size(); i++) 
		total_len += string_length(strings[i].c_str());
	total_len = total_len * 12;
	str_width = s_sqrt(total_len) + 20;
	//print_nums(0,total_len,str_width);
	if (str_width < 340)
		str_width = 340;
	cur_text_rect.right = cur_text_rect.left + str_width;
	cDialog* me = operator->();
	for(unsigned int j = 0; j < strings.size(); j++){
		std::ostringstream sout;
		sout << "str" << j + 1;
		str_height = ((string_length(strings[j].c_str()) + 60) / str_width) * 12 + 16;
		cur_text_rect.bottom = cur_text_rect.top + str_height;
		cTextMsg* str = new cTextMsg(me);
		str->setText(strings[j]);
		me->add(str, cur_text_rect, sout.str());
		cur_text_rect.top = cur_text_rect.bottom + 8;
	}
	buttons_right = cur_text_rect.right + 30;
	buttons_top = cur_text_rect.top;
}

template<bool a, bool b, bool c> void cThreeChoice::init_buttons(bbtt<a> btn1,bbtt<b> btn2,bbtt<c> btn3){
	Rect cur_btn_rect = {buttons_top,0,0,buttons_right};
	// The assignments in the if statements are intentional here.
	if(btn_used[0] = a) btns[0] = btn1;
	if(btn_used[1] = b) btns[1] = btn2;
	if(btn_used[2] = c) btns[2] = btn3;
	cDialog* me = operator->();
	for(int i = 0; i < 3; i++){
		if(!btn_used[i]) continue;
		std::ostringstream sout;
		sout << "btn" << i + 1;
		cButton* btn = new cButton(me);
		btn->attachKey(btns[i].defaultKey);
		btn->setText(btns[i].label);
		btn->setType(btns[i].type);
		btn->attachClickHandler(boost::bind(&cChoiceDlog::onClick,this,_1,_2));
		switch(type){
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
		switch(type){
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
}

void cThreeChoice::init_pict(pic_num_t pic){
	Rect pic_rect;
	switch(type){
		case PIC_DLOG:
		case PIC_CUSTOM_DLOG:
			SetRect(&pic_rect,0,0,36,36);
			break;
		case PIC_TALK:
		case PIC_CUSTOM_TALK:
		case PIC_SCEN:
		case PIC_CUSTOM_SCEN:
			SetRect(&pic_rect,0,0,32,32);
			break;
		case PIC_MISSILE:
		case PIC_CUSTOM_MISSILE:
			SetRect(&pic_rect,0,0,18,18);
			break;
		case PIC_DLOG_LG:
		case PIC_CUSTOM_DLOG_LG:
			SetRect(&pic_rect,0,0,72,72);
			break;
		case PIC_SCEN_LG:
			SetRect(&pic_rect,0,0,64,64);
			break;
		case PIC_TER_MAP:
		case PIC_CUSTOM_TER_MAP:
			SetRect(&pic_rect,0,0,24,24);
			break;
		case PIC_STATUS:
			SetRect(&pic_rect,0,0,12,12);
		default:
			SetRect(&pic_rect,0,0,28,36);
	}
	OffsetRect(&pic_rect,8,8);
	cDialog* me = operator->();
	cPict* pic_ctrl = new cPict(me);
	pic_ctrl->setPict(pic,type);
	me->add(pic_ctrl, pic_rect, "pict");
}

std::string cThreeChoice::show(){
	std::string result = cChoiceDlog::show();
	if(result == "btn1") return btns[0].label;
	else if(result == "btn2") return btns[1].label;
	else if(result == "btn3") return btns[2].label;
	return "**ERROR**"; // shouldn't be reached
}

std::string cStrDlog::getFileName(short n_strs, ePicType type, bool hasTitle){
	std::ostringstream sout;
	sout << n_strs << "str";
	if(hasTitle) sout << "-title";
	if(type == PIC_DLOG_LG || type == PIC_CUSTOM_DLOG_LG || type == PIC_SCEN_LG)
		sout << "-lg";
	sout << ".xml";
	return sout.str();
}

cStrDlog::cStrDlog(std::string str1,std::string str2,std::string title,pic_num_t pic,ePicType t,cDialog* parent)
	  : dlg(cStrDlog::getFileName((str1 != "") + (str2 != ""), t, title != ""), parent), type(t) {
	cPict& pic_ctrl = dynamic_cast<cPict&>(dlg["pict"]);
	pic_ctrl.setPict(pic, type);
	if(str1 != "") {
		dlg["str1"].setText(str1);
		if(str2 != "") dlg["str2"].setText(str2);
	}else if(str2 != "") dlg["str1"].setText(str2);
	if(title != "") dlg["title"].setText(title);
	dlg["record"].hide();
	dlg["record"].attachClickHandler(boost::bind(&cStrDlog::onRecord, this, _1, _2));
	dlg["okay"].attachClickHandler(boost::bind(&cStrDlog::onDismiss, this, _1, _2));
	this->str1 = str1;
	this->str2 = str2;
}

bool cStrDlog::onRecord(cDialog& me, std::string id){
	if(hasRecord) rec_f(str1, str2);
	else me[id].hide();
	return hasRecord;
}

bool cStrDlog::onDismiss(cDialog& me, std::string id){
	me.toast();
	return true;
}

cStrDlog& cStrDlog::setSound(snd_num_t snd){
	sound = snd;
	return *this;
}

cStrDlog& cStrDlog::setRecordHandler(record_callback_t rec){
	if(rec == NULL){
		hasRecord = false;
		dlg["record"].hide();
	}else{
		hasRecord = true;
		rec_f = rec;
		dlg["record"].show();
	}
	return *this;
}

void cStrDlog::show(){
	play_sound(sound);
	dlg.run();
}

void giveError(std::string str1, std::string str2, short err, cDialog* parent){
	// TODO: ...
}

void oopsError(short error, short code, short mode){
	// TODO: ...
}
