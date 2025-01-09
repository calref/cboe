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

#include "fileio/resmgr/res_dialog.hpp"

static DialogDefn& loadDefn() {
	return *ResMgr::dialogs.get("choose-string");
}

cStringChoice::cStringChoice(
		std::vector<std::string>& strs,
		std::string title,
		cDialog* parent
	) : dlg(loadDefn(),parent) {
	if(!title.empty()) dlg["title"].setText(title);
	strings = strs;
	attachHandlers();
}

cStringChoice::cStringChoice(
		std::vector<std::string>::iterator begin,
		std::vector<std::string>::iterator end,
		std::string title,
		cDialog* parent
	) : dlg(loadDefn(),parent) {
	if(!title.empty()) dlg["title"].setText(title);
	copy(begin,end,std::inserter(strings, strings.begin()));
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
	if(strings.size() <= per_page) {
		dlg["left"].hide();
		dlg["right"].hide();
	}
}

cDialog* cStringChoice::operator->() {
	return &dlg;
}

size_t cStringChoice::show(size_t selectedIndex) {
	cur = selectedIndex;
	if(cur >= strings.size()) cur = 0;
	page = cur / per_page;
	fillPage();
	dlg.setResult<size_t>(selectedIndex);
	dlg.run();
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
		cLed& led = dynamic_cast<cLed&>(dlg[sout.str()]);
		if(string_idx >= strings.size()){
			led.hide();
			continue;
		}else{
			led.setText(strings[string_idx]);
			led.recalcRect();
			led.show();
		}
		if(string_idx == cur)
			group.setSelected(sout.str());
	}
	group.recalcRect();
}

bool cStringChoice::onLeft(){
	if(page == 0) page = strings.size() / per_page;
	else page--;
	fillPage();
	return true;
}

bool cStringChoice::onRight(){
	if(page == strings.size() / per_page) page = 0;
	else page++;
	fillPage();
	return true;
}

bool cStringChoice::onCancel(cDialog& me){
	me.toast(false);
	return true;
}

bool cStringChoice::onOkay(cDialog& me){
	dlg.setResult(cur);
	me.toast(true);
	return true;
}

bool cStringChoice::onSelect(bool losing) {
	if(losing) return true;
	int i = boost::lexical_cast<int>(leds->getSelected().substr(3));
	cur = page * per_page + i - 1;
	if(select_handler)
		select_handler(*this, cur);
	return true;
}
