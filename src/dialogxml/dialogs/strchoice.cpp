/*
 *  dlogutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "strchoice.hpp"

#include <sstream>
#include <algorithm>

#include <boost/lexical_cast.hpp>

#include "dialogxml/widgets/field.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "sounds.hpp"

cStringChoice::cStringChoice(cDialog* parent, bool editable)
	: editable(editable)
	, per_page(editable ? 20 : 40)
	, dlg(*ResMgr::dialogs.get(editable ? "choose-edit-string" : "choose-string"), parent)
{}

cStringChoice::cStringChoice(const std::vector<std::string>& strs, std::string title, cDialog* parent, bool editable)
	: cStringChoice(parent, editable)
{
	setTitle(title);
	strings = strs;
	if(editable) {
		if(strings.empty()) strings.resize(per_page);
		else strings.resize(per_page * ceil(strings.size() / double(per_page)));
	}
	attachHandlers();
}

void cStringChoice::attachHandlers() {
	using namespace std::placeholders;
	dlg["left"].attachClickHandler(std::bind(&cStringChoice::onLeft,this));
	dlg["right"].attachClickHandler(std::bind(&cStringChoice::onRight,this));
	dlg["done"].attachClickHandler(std::bind(&cStringChoice::onOkay,this,_1));
	dlg["cancel"].attachClickHandler(std::bind(&cStringChoice::onCancel,this,_1));
	leds = &dynamic_cast<cLedGroup&>(dlg["strings"]);
	leds->attachFocusHandler(std::bind(&cStringChoice::onSelect,this,_3));
	if(editable) {
		for(int i = 1; i <= per_page; i++) {
			std::ostringstream sout;
			sout << "edit" << i;
			dlg[sout.str()].attachFocusHandler(std::bind(&cStringChoice::onFocus,this,_2,_3));
		}
	}
	if(!editable && strings.size() <= per_page) {
		dlg["left"].hide();
		dlg["right"].hide();
	}
}

cDialog* cStringChoice::operator->() {
	return &dlg;
}

size_t cStringChoice::show(size_t selectedIndex) {
	cur = selectedIndex;
	if(cur >= strings.size()) {
		if(editable) {
			strings.resize(per_page * ceil((cur + 1) / double(per_page)));
		} else cur = 0;
	} else if(cur < 0) {
		cur = 0;
	}
	page = cur / per_page;
	dlg.setResult<size_t>(selectedIndex);
	dlg.run(std::bind(&cStringChoice::fillPage, this));
	return dlg.getResult<size_t>();
}

void cStringChoice::attachSelectHandler(std::function<void(cStringChoice&,int)> f) {
	select_handler = f;
}

void cStringChoice::fillPage(){
	cLedGroup& group = dynamic_cast<cLedGroup&>(dlg["strings"]);
	group.setSelected(""); // unselect all LEDs, since the currently selected one may be on another page
	for(unsigned int i = 0; i < per_page; i++){
		short string_idx = page * per_page + i;
		std::ostringstream sout;
		sout << "led" << i + 1;
		std::string led_id = sout.str(), text_id;
		if(editable) {
			sout.str("");
			sout << "edit" << i + 1;
			text_id = sout.str();
		} else text_id = led_id;
		cLed& led = dynamic_cast<cLed&>(dlg[led_id]);
		cControl& text = dlg[text_id];
		if(string_idx >= strings.size()){
			led.hide();
			text.hide();
			continue;
		}else{
			text.setText(strings[string_idx]);
			if(!editable) led.recalcRect();
			led.show();
			text.show();
		}
		if(string_idx == cur) {
			group.setSelected(led_id);
			if(editable) {
				dlg.setFocus(dynamic_cast<cTextField*>(&text));
			}
		}
	}
	group.recalcRect();
}

bool cStringChoice::onLeft(){
	savePage();
	if(editable && page == lastPage()) {
		int blanks = 0;
		for(int i = strings.size() - 1; i >= 0; i--) {
			if(!strings[i].empty()) break;
			blanks++;
		}
		if(blanks >= per_page) {
			strings.resize(strings.size() - per_page);
		}
	}
	if(page == 0) page = lastPage();
	else page--;
	fillPage();
	return true;
}

bool cStringChoice::onRight(){
	savePage();
	if(editable && page == lastPage()) {
		strings.resize(strings.size() + per_page);
	}
	if(page == lastPage()) page = 0;
	else page++;
	fillPage();
	return true;
}

bool cStringChoice::onCancel(cDialog& me){
	savePage();
	me.toast(false);
	return true;
}

bool cStringChoice::onOkay(cDialog& me){
	savePage();
	dlg.setResult(cur);
	me.toast(true);
	return true;
}

bool cStringChoice::onSelect(bool losing) {
	if(losing) return true;
	int i = boost::lexical_cast<int>(leds->getSelected().substr(3));
	cur = page * per_page + i - 1;
	if(editable) {
		std::ostringstream sout;
		sout << "edit" << i;
		dlg.setFocus(dynamic_cast<cTextField*>(&dlg[sout.str()]));
	}
	if(select_handler)
		select_handler(*this, cur);
	return true;
}

bool cStringChoice::onFocus(std::string which, bool losing) {
	if(losing || !editable) return true;
	if(!dlg[which].getText().empty()) return true;
	int i = boost::lexical_cast<int>(which.substr(4));
	if(!strings[page * per_page + i - 1].empty()) return true;
	std::ostringstream sout;
	sout << "led" << i;
	if(leds->getSelected() != sout.str()) {
		play_sound(34);
		leds->setSelected(sout.str());
		cur = page * per_page + i - 1;
		if(select_handler)
			select_handler(*this, cur);
	}
	return true;
}

void cStringChoice::setTitle(const std::string &title) {
	if(!title.empty()) dlg["title"].setText(title);
}

size_t cStringChoice::lastPage() const {
	return (strings.size() - 1) / per_page;
}

void cStringChoice::savePage() {
	if(!editable) return;
	for(unsigned int i = 0; i < per_page; i++){
		short string_idx = page * per_page + i;
		std::ostringstream sout;
		sout << "edit" << i + 1;
		std::string text_id = sout.str();
		strings[string_idx] = dlg[text_id].getText();
	}
}
