
/*
 *  button.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "button.hpp"
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>

#include "dialog.hpp"
#include "render_image.hpp"

#include <cmath>
#include <climits>

#include "res_image.hpp"

extern sf::Texture bg_gworld;

cButton::cButton(sf::RenderWindow& parent) :
	cControl(CTRL_BTN,parent),
	wrapLabel(false),
	type(BTN_REG),
	textClr(sf::Color::Black),
	fromList("none") {}

cButton::cButton(cDialog& parent) :
	cControl(CTRL_BTN,parent),
	wrapLabel(false),
	type(BTN_REG),
	textClr(parent.getDefTextClr()),
	fromList("none") {}

cButton::cButton(cDialog& parent,eControlType t) :
	cControl(t,parent),
	fromList("none"),
	wrapLabel(false) {/* This constructor is only called for LEDs. */}

bool cButton::isClickable(){
	return true;
}

bool cButton::isFocusable(){
	return false;
}

bool cButton::isScrollable(){
	return false;
}

void cButton::draw(){
	rectangle from_rect, to_rect;
	
	inWindow->setActive();
	
	if(visible){
		TextStyle style;
		if(type == BTN_TINY) style.pointSize = 9;
		else if(type == BTN_PUSH) style.pointSize = 10;
		else style.pointSize = 12;
		from_rect = btnRects[type][depressed];
		to_rect = frame;
		if(type == BTN_TINY) {
			to_rect.right = to_rect.left + 14;
			to_rect.bottom = to_rect.top + 10;
		}
		rect_draw_some_item(*ResMgr::graphics.get(buttons[btnGW[type]]),from_rect,*inWindow,to_rect,sf::BlendAlpha);
		style.colour = sf::Color::Black;
		style.lineHeight = 8;
		eTextMode textMode = eTextMode::CENTRE;
		if(type == BTN_TINY) {
			textMode = wrapLabel ? eTextMode::WRAP : eTextMode::LEFT_TOP;
			to_rect.left += 18;
			style.colour = textClr;
		} else if(type == BTN_PUSH) {
			to_rect.top += 42;
			style.colour = textClr;
			int w = string_length(lbl, style);
			to_rect.inset((w - 30) / -2,0);
		}
		std::string label = lbl, keyDesc = getAttachedKeyDescription();
		for(size_t key_pos = label.find_first_of('\a'); key_pos < label.size(); key_pos = label.find_first_of('\a')) {
			label.replace(key_pos, 1, keyDesc);
		}
		win_draw_string(*inWindow,to_rect,label,textMode,style);
		// frame default button, to provide a visual cue that it's the default
		if(key.spec && key.k == key_enter) drawFrame(2,frameStyle);
	}
}

bool cButton::manageFormat(eFormat prop, bool set, boost::any* val) {
	switch(prop) {
		case TXT_FRAME:
			if(val) {
				if(set) frameStyle = boost::any_cast<eFrameStyle>(*val);
				else *val = frameStyle;
			}
			break;
		case TXT_WRAP:
			if(val) {
				if(set) wrapLabel = boost::any_cast<bool>(*val);
				else *val = wrapLabel;
			}
			break;
		case TXT_COLOUR:
			if(val) {
				if(set) textClr = boost::any_cast<sf::Color>(*val);
				else *val = textClr;
			}
			break;
		default: return false;
	}
	return true;
}

bool cButton::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	std::string name = attr.Name();
	if(name == "type") {
		std::string val = attr.Value();
		if(val == "small") setBtnType(BTN_SM);
		else if(val == "regular") setBtnType(BTN_REG);
		else if(val == "large") setBtnType(BTN_LG);
		else if(val == "help") setBtnType(BTN_HELP);
		else if(val == "left") setBtnType(BTN_LEFT);
		else if(val == "right") setBtnType(BTN_RIGHT);
		else if(val == "up") setBtnType(BTN_UP);
		else if(val == "down") setBtnType(BTN_DOWN);
		else if(val == "tiny") setBtnType(BTN_DONE);
		else if(val == "done") setBtnType(BTN_DONE);
		else if(val == "tall") setBtnType(BTN_TALL);
		else if(val == "trait") setBtnType(BTN_TRAIT);
		else if(val == "push") setBtnType(BTN_PUSH);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	} else if(name == "def-key") {
		std::string val = attr.Value();
		try {
			attachKey(parseKey(val));
		} catch(int) {
			throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		}
		return true;
	}
	return cControl::parseAttribute(attr, tagName, fname);
}

bool cButton::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	if(content.Type() == TiXmlNode::TEXT) {
		text += dlogStringFilter(content.Value());
		return true;
	} else if(content.Value() == "key") {
		text += '\a';
		return true;
	}
	return cControl::parseContent(content, n, tagName, fname, text);
}

static const std::set<eBtnType> labelledButtons{BTN_TINY, BTN_LED, BTN_PUSH};
	
void cButton::validatePostParse(ticpp::Element& elem, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& elems) {
	cControl::validatePostParse(elem, fname, attrs, elems);
	if(getType() == CTRL_BTN && !attrs.count("type")) throw xMissingAttr(elem.Value(), "type", elem.Row(), elem.Column(), fname);
	if(labelledButtons.count(type)) {
		if(!attrs.count("color") && !attrs.count("colour") && parent->getBg() == cDialog::BG_DARK)
			setColour(sf::Color::White);
		if(!attrs.count("width"))
			throw xMissingAttr(elem.Value(), "width", elem.Row(), elem.Column(), fname);
	}
}

location cButton::getPreferredSize() {
	return {btnRects[type][0].width(), btnRects[type][0].height()};
}

void cButton::recalcRect() {
	location bestSz = getPreferredSize();
	if(labelledButtons.count(type)) {
		if(frame.width() < bestSz.x) frame.width() = bestSz.x;
		if(frame.height() < bestSz.y) frame.height() = bestSz.y;
	} else {
		frame.width() = bestSz.x;
		frame.height() = bestSz.y;
	}
}

// Indices within the buttons array.
size_t cButton::btnGW[14] = {
	0, // BTN_SM
	1, // BTN_REG
	2, // BTN_LG
	4, // BTN_HELP
	1, // BTN_LEFT
	1, // BTN_RIGHT
	1, // BTN_UP
	1, // BTN_DOWN
	5, // BTN_TINY
	1, // BTN_DONE
	3, // BTN_TALL
	3, // BTN_TRAIT
	6, // BTN_PUSH
	5, // BTN_LED
};

rectangle cButton::btnRects[13][2];
std::string cButton::buttons[7] {
	"dlogbtnsm",
	"dlogbtnmed",
	"dlogbtnlg",
	"dlogbtntall",
	"dlogbtnhelp",
	"dlogbtnled",
	"dlgbtnred",
};

void cButton::init(){
	btnRects[BTN_SM][0] = {0,0,23,23};
	btnRects[BTN_REG][0] = {0,0,23,63};
	btnRects[BTN_LEFT][0] = {23,0,46,63};
	btnRects[BTN_RIGHT][0] = {46,0,69,63};
	btnRects[BTN_UP][0] = {69,0,92,63};
	btnRects[BTN_DOWN][0] = {92,0,115,63};
	btnRects[BTN_DONE][0] = {115,0,138,63};
	btnRects[BTN_LG][0] = {0,0,23,102};
	btnRects[BTN_HELP][0] = {0,0,13,16};
	btnRects[BTN_TINY][0] = {0,42,10,56};
	btnRects[BTN_TALL][0] = {0,0,40,63};
	btnRects[BTN_TRAIT][0] = {40,0,80,63};
	btnRects[BTN_PUSH][0] = {0,0,30,30};
	for(int j = 0; j <= 12; j++)
		btnRects[j][1] = btnRects[j][0];
	btnRects[BTN_SM][1].offset(23,0);
	btnRects[BTN_REG][1].offset(63,0);
	btnRects[BTN_LEFT][1].offset(63,0);
	btnRects[BTN_RIGHT][1].offset(63,0);
	btnRects[BTN_UP][1].offset(63,0);
	btnRects[BTN_DOWN][1].offset(63,0);
	btnRects[BTN_DONE][1].offset(63,0);
	btnRects[BTN_LG][1].offset(102,0);
	btnRects[BTN_HELP][1].offset(16,0);
	btnRects[BTN_TINY][1].offset(0,10);
	btnRects[BTN_TALL][1].offset(63,0);
	btnRects[BTN_TRAIT][1].offset(63,0);
	btnRects[BTN_PUSH][1].offset(30,0);
}

rectangle cLed::ledRects[3][2];

void cLed::init(){
	rectangle baseLed = {0,0,10,14};
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 2; j++){
			ledRects[i][j] = baseLed;
			ledRects[i][j].offset(i * 14, j * 10);
		}
}

cLed::cLed(cDialog& parent) :
	cButton(parent,CTRL_LED),
	state(led_off),
	textFont(FONT_BOLD),
	textSize(10) {
	type = BTN_LED;
	using namespace std::placeholders;
	attachEventHandler<EVT_CLICK>(std::bind(&cLed::defaultClickHandler, this, _1, _2, _3));
}

void cLed::defaultClickHandler(cDialog&, std::string, eKeyMod) {
	// simple state toggle
	switch(state){
		case led_red:
		case led_green:
			state = led_off;
			break;
		case led_off:
			state = led_red;
	}
}

void cLed::callHandler(event_fcn<EVT_CLICK>::type onClick, cDialog& me, std::string id, eKeyMod mods) {
	eLedState oldState = state;
	if(onClick) onClick(me,id,mods);
	if(!triggerFocusHandler(me,id, oldState != led_off)){
		state = oldState;
	}
}

bool cLed::manageFormat(eFormat prop, bool set, boost::any* val) {
	switch(prop) {
		case TXT_FONT:
			if(val) {
				if(set) textFont = boost::any_cast<eFont>(*val);
				else *val = textFont;
			}
			break;
		case TXT_SIZE:
			if(val) {
				if(set) textSize = boost::any_cast<short>(*val);
				else *val = textSize;
			}
			break;
		case TXT_WRAP:
			if(val) {
				if(set) wrapLabel = boost::any_cast<bool>(*val);
				else *val = wrapLabel;
			}
			break;
		case TXT_COLOUR:
			if(val) {
				if(set) textClr = boost::any_cast<sf::Color>(*val);
				else *val = textClr;
			}
			break;
		default: return false;
	}
	return true;
}

void cLed::draw(){
	rectangle from_rect, to_rect;
	
	inWindow->setActive();
	
	if(visible){
		TextStyle style;
		style.pointSize = textSize;
		style.lineHeight = textSize - 1;
		style.font = textFont;
		from_rect = ledRects[state][depressed];
		to_rect = frame;
		to_rect.right = to_rect.left + 14;
		to_rect.bottom = to_rect.top + 10;
		rect_draw_some_item(*ResMgr::graphics.get(buttons[btnGW[BTN_LED]]),from_rect,*inWindow,to_rect);
		style.colour = textClr;
		to_rect.right = frame.right;
		to_rect.left = frame.left + 18; // Possibly could be 20
		win_draw_string(*inWindow,to_rect,lbl,wrapLabel ? eTextMode::WRAP : eTextMode::LEFT_TOP,style);
	}
}

cControl::storage_t cLed::store() {
	storage_t storage = cButton::store();
	storage["led-state"] = getState();
	return storage;
}

void cLed::restore(storage_t to) {
	cButton::restore(to);
	if(to.find("led-state") != to.end())
		setState(boost::any_cast<eLedState>(to["led-state"]));
	else setState(led_off);
}

cLedGroup::cLedGroup(cDialog& parent) :
	cContainer(CTRL_GROUP,parent),
	fromList("none") {}

cButton::~cButton() {}

cLed::~cLed() {}

cLedGroup::~cLedGroup(){
	ledIter iter = choices.begin();
	while(iter != choices.end()){
		delete iter->second;
		iter++;
	}
}

void cLedGroup::recalcRect(){
	ledIter iter = choices.begin();
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

void cLed::setState(eLedState to){
	state = to;
}

eLedState cLed::getState(){
	return state;
}

bool cLed::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	if(attr.Name() == "state") {
		std::string val = attr.Value();
		if(val == "red") setState(led_red);
		else if(val == "green") setState(led_green);
		else if(val == "off") setState(led_off);
		else throw xBadVal(tagName, attr.Name(), val, attr.Row(), attr.Column(), fname);
		return true;
	}
	return cButton::parseAttribute(attr, tagName, fname);
}

bool cLed::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	if(content.Type() == TiXmlNode::ELEMENT && content.Value() == "key")
		return false;
	return cButton::parseContent(content, n, tagName, fname, text);
}

location cLed::getPreferredSize() {
	return {ledRects[0][0].width(), ledRects[0][0].height()};
}

void cLedGroup::addChoice(cLed* ctrl, std::string key) {
	choices[key] = ctrl;
	if(ctrl->getState() != led_off)
		setSelected(key);
}

bool cLedGroup::handleClick(location where) {
	std::string which_clicked;
	ledIter iter = choices.begin();
	while(iter != choices.end()){
		if(iter->second->isVisible() && where.in(iter->second->getBounds())){
			if(iter->second->handleClick(where)) {
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
	std::string which_clicked = clicking;
	clicking = "";
	
	if(choices[which_clicked]->triggerClickHandler(me,which_clicked,mods)){
		if(onClick) onClick(me,id,mods);
		if(!curSelect.empty()) {
			choices[curSelect]->setState(led_off);
			if(!choices[curSelect]->triggerFocusHandler(me,curSelect,true)){
				choices[curSelect]->setState(led_red);
				return;
			}
		}
		choices[which_clicked]->setState(led_red);
		if(!choices[which_clicked]->triggerFocusHandler(me,which_clicked,false)){
			if(!curSelect.empty())
				choices[curSelect]->setState(led_red);
			choices[which_clicked]->setState(led_off);
			return;
		}
	}else return;
	
	std::string savePrevSelect = prevSelect;
	prevSelect = curSelect;
	curSelect = which_clicked;
	if(!triggerFocusHandler(me,id,false)) {
		if(!curSelect.empty())
			choices[curSelect]->setState(led_red);
		choices[which_clicked]->setState(led_off);
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

bool cLedGroup::isClickable(){
	return true;
}

bool cLedGroup::isScrollable(){
	return false;
}

bool cLedGroup::isFocusable(){
	return false;
}

bool cLedGroup::hasChild(std::string id) {
	return choices.find(id) != choices.end();
}

cLed& cLedGroup::getChild(std::string id){
	ledIter iter = choices.find(id);
	if(iter == choices.end()) throw std::invalid_argument(id + " does not exist in the ledgroup.");
	return *(iter->second);
}

void cLedGroup::setSelected(std::string id){
	if(id == "") { // deselect all
		if(curSelect == "") return;
		eLedState was = choices[curSelect]->getState();
		choices[curSelect]->setState(led_off);
		if(choices[curSelect]->triggerFocusHandler(*parent,curSelect,true))
			curSelect = "";
		else
			choices[curSelect]->setState(was);
		return;
	}
	
	ledIter iter = choices.find(id);
	if(iter == choices.end()) throw std::invalid_argument(id + " does not exist in the ledgroup.");
	
	if(curSelect == ""){
		if(iter->second->triggerFocusHandler(*parent,curSelect,false)){
			iter->second->setState(led_red);
			curSelect = iter->first;
		}
	}else{
		eLedState a, b;
		a = choices[curSelect]->getState();
		b = iter->second->getState();
		choices[curSelect]->setState(led_off);
		iter->second->setState(led_red);
		if(!choices[curSelect]->triggerFocusHandler(*parent,curSelect,true)){
			choices[curSelect]->setState(a);
			iter->second->setState(b);
			return;
		}
		if(!iter->second->triggerFocusHandler(*parent,curSelect,false)){
			choices[curSelect]->setState(a);
			iter->second->setState(b);
			return;
		}
		curSelect = iter->first;
	}
}

std::string cLedGroup::getSelected(){
	return curSelect;
}

std::string cLedGroup::getPrevSelection(){
	return prevSelect;
}

void cLedGroup::draw(){
	if(!visible) return;
	ledIter iter = choices.begin();
	while(iter != choices.end()){
		iter->second->draw();
		iter++;
	}
	drawFrame(2, frameStyle);
}

void cButton::setBtnType(eBtnType newType){
	if(type == BTN_LED || newType == BTN_LED) return; // can't change type
	type = newType;
	switch(newType) {
		case BTN_SM:
			frame.width() = 23;
			frame.height() = 23;
			break;
		case BTN_REG: case BTN_DONE:
		case BTN_LEFT: case BTN_RIGHT:
		case BTN_UP: case BTN_DOWN:
			frame.width() = 63;
			frame.height() = 23;
			break;
		case BTN_LG:
			frame.width() = 102;
			frame.height() = 23;
			break;
		case BTN_HELP:
			frame.width() = 16;
			frame.height() = 13;
			break;
		case BTN_TALL:
		case BTN_TRAIT:
			frame.width() = 63;
			frame.height() = 40;
			break;
		case BTN_PUSH:
			frame.width() = std::min<int>(frame.width(), 30);
			frame.height() = std::min<int>(frame.height(), 30);
			break;
		case BTN_TINY:
		case BTN_LED:
			frame.width() = std::min<int>(frame.width(), 14);
			frame.height() = std::min<int>(frame.height(), 10);
			break;
	}
}

eBtnType cButton::getBtnType(){
	return type;
}

cControl::storage_t cLedGroup::store() {
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
		auto led = parent->parse<cLed>(dynamic_cast<ticpp::Element&>(content));
		addChoice(led.second, led.first);
		return true;
	} else if(type == TiXmlNode::TEXT) {
		return false;
	}
	return cContainer::parseContent(content, n, tagName, fname, text);
}

void cLedGroup::validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) {
	// Don't defer to super-class; groups are an abstract container that doesn't require a position.
	//cControl::validatePostParse(who, fname, attrs, nodes);
	frameStyle = FRM_NONE;
}
