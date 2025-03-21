
/*
 *  ledgroup.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 2020-12-21.
 *
 */

#include <climits>

#include "ledgroup.hpp"

#include "dialogxml/dialogs/dialog.hpp"

#include "replay.hpp"

cLedGroup::cLedGroup(iComponent& parent)
	: cContainer(CTRL_GROUP,parent)
	, fromList("none")
{}

cLedGroup::~cLedGroup(){
	ctrlIter iter = choices.begin();
	while(iter != choices.end()){
		delete iter->second;
		iter++;
	}
}

void cLedGroup::recalcRect(){
	ctrlIter iter = choices.begin();
	frame = {INT_MAX, INT_MAX, 0, 0};
	while(iter != choices.end()){
		rectangle otherFrame = iter->second->getBounds();
		if(otherFrame.right > frame.right)
			frame.right = otherFrame.right;
		if(otherFrame.bottom > frame.bottom)
			frame.bottom = otherFrame.bottom;
		if(otherFrame.left < frame.left)
			frame.left = otherFrame.left;
		if(otherFrame.top < frame.top)
			frame.top = otherFrame.top;
		iter++;
	}
	frame.inset(-6,-6);
}

void cLedGroup::addChoice(cLed* ctrl, std::string key) {
	// normal ledgroups add their children through parseChildControl, but mapgroups still
	// need to do it this way
	if(!hasChild(key))
		choices[key] = ctrl;
	if(ctrl->getState() != led_off)
		setSelected(key);
}

bool cLedGroup::handleClick(location where, cFramerateLimiter& fps_limiter) {
	std::string which_clicked;
	ctrlIter iter = choices.begin();
	while(iter != choices.end()){
		if(iter->second->isVisible() && where.in(iter->second->getBounds())){
			if(iter->second->handleClick(where, fps_limiter)) {
				which_clicked = iter->first;
				break;
			}
		}
		iter++;
	}
	
	if(which_clicked == "") return false;
	
	clicking = which_clicked;
	return true;
}

void cLedGroup::callHandler(event_fcn<EVT_CLICK>::type onClick, cDialog& me, std::string id, eKeyMod mods) {
	// When replaying, a click event for the specifically clicked led comes next
	if(replaying){
		auto led_click_action = pop_next_action("click_control");
		auto info = info_from_action(led_click_action);
		clicking = info["id"];
	}
	std::string which_clicked = clicking;
	clicking = "";
	
	if(choices[which_clicked]->triggerClickHandler(me,which_clicked,mods)){
		if(onClick) onClick(me,id,mods);
		if(!curSelect.empty()) {
			cLed& sel = getChild(curSelect);
			sel.setState(led_off);
			if(!sel.triggerFocusHandler(me,curSelect,true)){
				sel.setState(led_red);
				return;
			}
		}
		cLed& led = getChild(which_clicked);
		led.setState(led_red);
		if(!led.triggerFocusHandler(me,which_clicked,false)){
			if(!curSelect.empty())
				getChild(curSelect).setState(led_red);
			led.setState(led_off);
			return;
		}
	}else return;
	
	std::string savePrevSelect = prevSelect;
	prevSelect = curSelect;
	curSelect = which_clicked;
	if(!triggerFocusHandler(me,id,false)) {
		if(!curSelect.empty())
			getChild(curSelect).setState(led_red);
		getChild(which_clicked).setState(led_off);
		curSelect = prevSelect;
		prevSelect = savePrevSelect;
		return;
	}
	return;
}

void cLedGroup::attachFocusHandler(std::function<bool(cDialog&,std::string,bool)> f) {
	if(!f) {
		attachEventHandler<EVT_FOCUS>(nullptr);
		return;
	}
	using namespace std::placeholders;
	attachEventHandler<EVT_FOCUS>([f](cDialog& me, std::string id) {
		f(me, id, false);
	});
}

void cLedGroup::disable(std::string /*id*/) {
	// TODO: Implement this
}

void cLedGroup::enable(std::string /*id*/) {
	// TODO: Implement this
}

void cLedGroup::show(std::string id){
	choices[id]->show();
}

void cLedGroup::hide(std::string id){
	choices[id]->hide();
}

bool cLedGroup::isClickable() const {
	return true;
}

bool cLedGroup::isScrollable() const {
	return false;
}

bool cLedGroup::isFocusable() const {
	return false;
}

bool cLedGroup::hasChild(std::string id) const {
	return choices.find(id) != choices.end();
}

cLed& cLedGroup::getChild(std::string id){
	ctrlIter iter = choices.find(id);
	if(iter == choices.end()) throw std::invalid_argument(id + " does not exist in the ledgroup.");
	return dynamic_cast<cLed&>(*(iter->second));
}

void cLedGroup::setSelected(std::string id){
	if(id == "") { // deselect all
		if(curSelect == "") return;
		cLed& sel = getChild(curSelect);
		eLedState was = sel.getState();
		sel.setState(led_off);
		if(sel.triggerFocusHandler(*getDialog(),curSelect,true))
			curSelect = "";
		else
			sel.setState(was);
		return;
	}
	
	ctrlIter iter = choices.find(id);
	if(iter == choices.end()) throw std::invalid_argument(id + " does not exist in the ledgroup.");

	cLed& led = getChild(id);
	if(curSelect == ""){
		if(led.triggerFocusHandler(*getDialog(),curSelect,false)){
			led.setState(led_red);
			curSelect = id;
		}
	}else{
		cLed& sel = getChild(curSelect);
		eLedState a, b;
		a = sel.getState();
		b = led.getState();
		sel.setState(led_off);
		led.setState(led_red);
		if(!sel.triggerFocusHandler(*getDialog(),curSelect,true)){
			sel.setState(a);
			led.setState(b);
			return;
		}
		if(!led.triggerFocusHandler(*getDialog(),curSelect,false)){
			sel.setState(a);
			led.setState(b);
			return;
		}
		curSelect = id;
	}
}

std::string cLedGroup::getSelected() const {
	return curSelect;
}

std::string cLedGroup::getPrevSelection() const {
	return prevSelect;
}

void cLedGroup::draw(){
	if(!visible) return;
	ctrlIter iter = choices.begin();
	while(iter != choices.end()){
		iter->second->draw();
		iter++;
	}
	drawFrame(2, frameStyle);
}

cControl::storage_t cLedGroup::store() const {
	storage_t storage = cControl::store();
	storage["led-select"] = getSelected();
	return storage;
}

void cLedGroup::restore(storage_t to) {
	cControl::restore(to);
	if(to.find("led-select") != to.end())
		setSelected(boost::any_cast<std::string>(to["led-select"]));
	else setSelected("");
}

void cLedGroup::forEach(std::function<void(std::string,cControl&)> callback) {
	for(auto ctrl : choices)
		callback(ctrl.first, *ctrl.second);
}

bool cLedGroup::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	std::string val = content.Value();
	int type = content.Type();
	if(type == TiXmlNode::ELEMENT && val == "led"){
		std::string id;
		if(!parseChildControl(dynamic_cast<ticpp::Element&>(content), choices, id, fname))
			return false;
		cLed* led = dynamic_cast<cLed*>(choices[id]);
		addChoice(led, id);
		return true;
	} else if(type == TiXmlNode::TEXT) {
		return false;
	}
	return cContainer::parseContent(content, n, tagName, fname, text);
}

void cLedGroup::validatePostParse(ticpp::Element&, std::string, const std::set<std::string>&, const std::multiset<std::string>&) {
	// Don't defer to super-class; groups are an abstract container that doesn't require a position.
	//cControl::validatePostParse(who, fname, attrs, nodes);
	frameStyle = FRM_NONE;
}
