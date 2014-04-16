/*
 *  dlogutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */
#define BTNS_DEFINED

#include <sstream>
#include <algorithm>

#include <functional>
#include "dialog.h"
#include "dlogutil.h"
#include "mathutil.h"
#include <array>
#include "message.h"

// TODO: This should probably be a source file instead of a header
#include "dlogutil.buttons.h" // must be included here and only here

const size_t cPictChoice::per_page = 36;

cPictChoice::cPictChoice(std::vector<pic_num_t>& pics,ePicType t,cDialog* parent) : dlg("choose-pict.xml",parent), type(t) {
	attachClickHandlers();
	picts = pics;
	sort(picts.begin(),picts.end());
}

cPictChoice::cPictChoice(
		std::vector<pic_num_t>::iterator begin,
		std::vector<pic_num_t>::iterator end,
		ePicType t,
		cDialog* parent
	) : dlg("choose-pict.xml",parent), type(t) {
	attachClickHandlers();
	copy(begin,end,picts.begin());
	sort(picts.begin(),picts.end());
}

cPictChoice::cPictChoice(pic_num_t begin, pic_num_t end, ePicType t, cDialog* parent) : dlg("choose-pict.xml",parent), type(t) {
	attachClickHandlers();
	for(pic_num_t i = begin; i < end; i++) {
		picts.push_back(i);
	}
}

void cPictChoice::attachClickHandlers() {
	using namespace std::placeholders;
	dlg["left"].attachClickHandler(std::bind(&cPictChoice::onLeft,this,_1,_2));
	dlg["right"].attachClickHandler(std::bind(&cPictChoice::onRight,this,_1,_2));
	dlg["done"].attachClickHandler(std::bind(&cPictChoice::onOkay,this,_1,_2));
	dlg["cancel"].attachClickHandler(std::bind(&cPictChoice::onCancel,this,_1,_2));
}

cDialog* cPictChoice::operator->() {
	return &dlg;
}

pic_num_t cPictChoice::show(pic_num_t fallback, pic_num_t cur_sel){
	dlg.setResult(fallback);
	cur = cur_sel;
	page = cur / per_page;
	// TODO: Hide left/right buttons if only one page?
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
	if(page == 0) page = (picts.size() - 1) / per_page;
	else page--;
	fillPage();
	return true;
}

bool cPictChoice::onRight(cDialog& me, std::string id){
	if(page == (picts.size() - 1) / per_page) page = 0;
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
	using namespace std::placeholders;
	dlg["left"].attachClickHandler(std::bind(&cStringChoice::onLeft,this,_1,_2));
	dlg["right"].attachClickHandler(std::bind(&cStringChoice::onRight,this,_1,_2));
	dlg["done"].attachClickHandler(std::bind(&cStringChoice::onOkay,this,_1,_2));
	dlg["cancel"].attachClickHandler(std::bind(&cStringChoice::onCancel,this,_1,_2));
	strings = strs;
}

cStringChoice::cStringChoice(
		std::vector<std::string>::iterator begin,
		std::vector<std::string>::iterator end,
		cDialog* parent
	) : dlg("choose-string.xml",parent) {
	using namespace std::placeholders;
	dlg["left"].attachClickHandler(std::bind(&cStringChoice::onLeft,this,_1,_2));
	dlg["right"].attachClickHandler(std::bind(&cStringChoice::onRight,this,_1,_2));
	dlg["done"].attachClickHandler(std::bind(&cStringChoice::onOkay,this,_1,_2));
	dlg["cancel"].attachClickHandler(std::bind(&cStringChoice::onCancel,this,_1,_2));
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
	using namespace std::placeholders;
	std::vector<std::string>::iterator iter = buttons.begin();
	while(iter != buttons.end()){
		dlg[*iter].attachClickHandler(std::bind(&cChoiceDlog::onClick,this,_1,_2));
		iter++;
	}
}

cChoiceDlog::cChoiceDlog(cDialog* p) : dlg(p) {}

cChoiceDlog::cChoiceDlog(std::string file, cDialog* p)
	: cChoiceDlog(file, {"okay"}, p) {}

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
		else buttonDefs[i++] = basic_buttons[available_btns[j]];
	}
	init_buttons(buttonDefs[0], buttonDefs[1], buttonDefs[2]);
	init_pict(pic);
	parentDlog.recalcRect();
}

void cThreeChoice::init_strings(std::vector<std::string>& strings, unsigned short left){
	RECT cur_text_rect = {2, left, 0, 0};
	size_t total_len, str_width, str_height;
	for (unsigned int i = 0; i < strings.size(); i++) 
		total_len += string_length(strings[i]);
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
		str_height = ((string_length(strings[j]) + 60) / str_width) * 12 + 16;
		cur_text_rect.bottom = cur_text_rect.top + str_height;
		cTextMsg* str = new cTextMsg(*me);
		str->setText(strings[j]);
		me->add(str, cur_text_rect, sout.str());
		cur_text_rect.top = cur_text_rect.bottom + 8;
	}
	buttons_right = cur_text_rect.right + 30;
	buttons_top = cur_text_rect.top;
}

void cThreeChoice::init_buttons(cBasicButtonType btn1, cBasicButtonType btn2, cBasicButtonType btn3){
	using namespace std::placeholders;
	RECT cur_btn_rect = {buttons_top,0,0,buttons_right};
	if(btn1) btns[0] = btn1;
	if(btn2) btns[1] = btn2;
	if(btn3) btns[2] = btn3;
	cDialog* me = operator->();
	// TODO: Is it correct for the first button to always be the default?
	bool haveDefault = false;
	for(int i = 0; i < 3; i++){
		if(!btns[i]) continue;
		std::ostringstream sout;
		sout << "btn" << i + 1;
		cButton* btn = new cButton(me);
		btn->attachKey(btns[i]->defaultKey);
		btn->setText(btns[i]->label);
		btn->setType(btns[i]->type);
		btn->attachClickHandler(std::bind(&cChoiceDlog::onClick,this,_1,_2));
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
		if(!haveDefault) {
			me->setDefBtn(sout.str());
			haveDefault = true;
		} else if(btns[i]->label == "OK") {
			me->setDefBtn(sout.str());
		}
	}
}

void cThreeChoice::init_pict(pic_num_t pic){
	RECT pic_rect;
	switch(type){
		case PIC_DLOG:
		case PIC_CUSTOM_DLOG:
			pic_rect = {0,0,36,36};
			break;
		case PIC_TALK:
		case PIC_CUSTOM_TALK:
		case PIC_SCEN:
		case PIC_CUSTOM_SCEN:
			pic_rect = {0,0,32,32};
			break;
		case PIC_MISSILE:
		case PIC_CUSTOM_MISSILE:
			pic_rect = {0,0,18,18};
			break;
		case PIC_DLOG_LG:
		case PIC_CUSTOM_DLOG_LG:
			pic_rect = {0,0,72,72};
			break;
		case PIC_SCEN_LG:
			pic_rect = {0,0,64,64};
			break;
		case PIC_TER_MAP:
		case PIC_CUSTOM_TER_MAP:
			pic_rect = {0,0,24,24};
			break;
		case PIC_STATUS:
			pic_rect = {0,0,12,12};
		default:
			pic_rect = {0,0,28,36};
	}
	pic_rect.offset(8,8);
	cDialog* me = operator->();
	cPict* pic_ctrl = new cPict(*me);
	pic_ctrl->setPict(pic,type);
	me->add(pic_ctrl, pic_rect, "pict");
}

std::string cThreeChoice::show(){
	std::string result = cChoiceDlog::show();
	if(result == "btn1") return btns[0]->label;
	else if(result == "btn2") return btns[1]->label;
	else if(result == "btn3") return btns[2]->label;
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
	using namespace std::placeholders;
	cPict& pic_ctrl = dynamic_cast<cPict&>(dlg["pict"]);
	pic_ctrl.setPict(pic, type);
	if(str1 != "") {
		dlg["str1"].setText(str1);
		if(str2 != "") dlg["str2"].setText(str2);
	}else if(str2 != "") dlg["str1"].setText(str2);
	if(title != "") dlg["title"].setText(title);
	dlg["record"].hide();
	dlg["record"].attachClickHandler(std::bind(&cStrDlog::onRecord, this, _1, _2));
	dlg["done"].attachClickHandler(std::bind(&cStrDlog::onDismiss, this, _1, _2));
}

bool cStrDlog::onRecord(cDialog& me, std::string id){
	if(hasRecord) rec_f(me);
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
	if(sound > 0) play_sound(sound);
	dlg.run();
}

void giveError(std::string str1, std::string str2, cDialog* parent){
	cStrDlog error(str1,str2,"Error!!!",25,PIC_DLOG,parent);
	error.show();
}

void oopsError(short error, short code, short mode){ // mode is 0 for scened, 1 for game, 2 for pced
	std::ostringstream error_str1, error_str2;
	static const char* progname[3] = {"the scenario editor", "Blades of Exile", "the PC editor"};
	static const char* filetname[3] = {"scenario", "game", "game"};
	
	error_str1 << "The program encountered an error while loading/saving/creating the " << filetname[mode]
		<< ". To prevent future problems, the program will now terminate. Trying again may solve the problem.";
	// TODO: Update this error message - giving more memory is no longer an option in OSX.
	error_str2 << "Giving " << progname[mode] << " more memory might also help. Be sure to back your " << filetname[mode] << " up often. Error number: " << error << ".";
	if(code != 0)
		error_str2 << " Result code: " << code << ".";
	giveError(error_str1.str(),error_str2.str(),NULL);
	exit(1);
}
