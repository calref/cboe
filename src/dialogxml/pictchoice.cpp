/*
 *  dlogutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "pictchoice.hpp"

#include <sstream>
#include <boost/lexical_cast.hpp>
#include "pict.hpp"

cPictChoice::cPictChoice(const std::vector<pic_num_t>& pics,ePicType t,cDialog* parent) : cPictChoice(pics.begin(), pics.end(), t, parent) {}

cPictChoice::cPictChoice(const std::vector<std::pair<pic_num_t,ePicType>>& pics,cDialog* parent) : dlg("choose-pict",parent) {
	picts = pics;
	attachHandlers();
}

cPictChoice::cPictChoice(
		std::vector<pic_num_t>::const_iterator begin,
		std::vector<pic_num_t>::const_iterator end,
		ePicType t,
		cDialog* parent
	) : dlg("choose-pict",parent) {
	for(auto iter = begin; iter != end; iter++) {
		picts.push_back({*iter,t});
	}
	attachHandlers();
}

cPictChoice::cPictChoice(pic_num_t first, pic_num_t last, ePicType t, cDialog* parent) : dlg("choose-pict",parent) {
	for(pic_num_t i = first; i <= last; i++) {
		picts.push_back({i,t});
	}
	attachHandlers();
}

void cPictChoice::attachHandlers() {
	using namespace std::placeholders;
	dlg["left"].attachClickHandler(std::bind(&cPictChoice::onLeft,this));
	dlg["right"].attachClickHandler(std::bind(&cPictChoice::onRight,this));
	dlg["done"].attachClickHandler(std::bind(&cPictChoice::onOkay,this));
	dlg["cancel"].attachClickHandler(std::bind(&cPictChoice::onCancel,this));
	leds = &dynamic_cast<cLedGroup&>(dlg["group"]);
	leds->attachFocusHandler(std::bind(&cPictChoice::onSelect,this,_3));
	if(picts.size() <= per_page) {
		dlg["left"].hide();
		dlg["right"].hide();
	}
}

cDialog* cPictChoice::operator->() {
	return &dlg;
}

bool cPictChoice::show(size_t cur_sel){
	if(cur_sel >= picts.size())
		cur_sel = 0;
	cur = cur_sel;
	dlg.setResult(picts[cur_sel]);
	page = cur / per_page;
	fillPage();
	dlg.run();
	return didAccept;
}

void cPictChoice::attachSelectHandler(std::function<void(cPictChoice&,int)> f) {
	select_handler = f;
}

void cPictChoice::fillPage(){
	cLedGroup& group = dynamic_cast<cLedGroup&>(dlg["group"]);
	group.setSelected(""); // unselect all LEDs, since the currently selected one may be on another page
	for(size_t i = 0; i < per_page; i++){
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
			ePicType tp = picts[per_page * page + i].second;
			pic.setPict(picts[per_page * page + i].first, tp);
			rectangle b = pic.getBounds();
			if(tp == PIC_DLOG_LG || tp == PIC_CUSTOM_DLOG_LG || tp == PIC_SCEN_LG) {
				pic.setFormat(TXT_WRAP, false);
				b.width() /= 2;
				b.height() /= 2;
			} else if(tp == PIC_FULL) {
				pic.setFormat(TXT_WRAP, false);
				b.width() = 40;
				b.height() = 40;
			} else pic.setFormat(TXT_WRAP, true);
			pic.setBounds(b);
		}else pic.hide();
	}
}

bool cPictChoice::onLeft(){
	if(page == 0) page = (picts.size() - 1) / per_page;
	else page--;
	fillPage();
	return true;
}

bool cPictChoice::onRight(){
	if(page == (picts.size() - 1) / per_page) page = 0;
	else page++;
	fillPage();
	return true;
}

bool cPictChoice::onCancel(){
	dlg.toast(false);
	didAccept = false;
	return true;
}

bool cPictChoice::onOkay(){
	dlg.setResult(picts[cur]);
	dlg.toast(true);
	didAccept = true;
	return true;
}

bool cPictChoice::onSelect(bool losing) {
	if(losing) return true;
	int i = boost::lexical_cast<int>(leds->getSelected().substr(3));
	cur = page * per_page + i - 1;
	if(select_handler)
		select_handler(*this, cur);
	return true;
}

pic_num_t cPictChoice::getPicChosen() {
	return dlg.getResult<std::pair<pic_num_t,ePicType>>().first;
}

ePicType cPictChoice::getPicChosenType() {
	return dlg.getResult<std::pair<pic_num_t,ePicType>>().second;
}

size_t cPictChoice::getSelected() {
	return cur;
}
