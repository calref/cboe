
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
		rect_draw_some_item(*ResMgr::get<ImageRsrc>(buttons[btnGW[type]]),from_rect,*inWindow,to_rect,sf::BlendAlpha);
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
		win_draw_string(*inWindow,to_rect,lbl,textMode,style);
		// frame default button, to provide a visual cue that it's the default
		if(key.spec && key.k == key_enter) drawFrame(2,frameStyle);
	}
}

void cButton::setFormat(eFormat prop, short val) throw(xUnsupportedProp){
	if(prop == TXT_WRAP) wrapLabel = val;
	else if(prop == TXT_FRAMESTYLE) frameStyle = eFrameStyle(val);
	else throw xUnsupportedProp(prop);
}

short cButton::getFormat(eFormat prop) throw(xUnsupportedProp){
	if(prop == TXT_WRAP) return wrapLabel;
	else if(prop == TXT_FRAMESTYLE) return frameStyle;
	else throw xUnsupportedProp(prop);
}

void cButton::setColour(sf::Color clr) throw(xUnsupportedProp) {
	textClr = clr;
}

sf::Color cButton::getColour() throw(xUnsupportedProp) {
	return textClr;
}

std::string cButton::parse(ticpp::Element& who, std::string fname) {
	using namespace ticpp;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	std::string name, id;
	int width = 0, height = 0;
	bool foundType = false, foundTop = false, foundLeft = false; // required attributes
	rectangle frame;
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&id);
		else if(name == "wrap"){
			std::string val;
			attr->GetValue(&val);
			if(val == "true") setFormat(TXT_WRAP, true);
		}else if(name == "type"){
			std::string val;
			foundType = true;
			attr->GetValue(&val);
			if(val == "small")
				setBtnType(BTN_SM);
			else if(val == "regular")
				setBtnType(BTN_REG);
			else if(val == "large")
				setBtnType(BTN_LG);
			else if(val == "help")
				setBtnType(BTN_HELP);
			else if(val == "left")
				setBtnType(BTN_LEFT);
			else if(val == "right")
				setBtnType(BTN_RIGHT);
			else if(val == "up")
				setBtnType(BTN_UP);
			else if(val == "down")
				setBtnType(BTN_DOWN);
			else if(val == "tiny")
				setBtnType(BTN_TINY);
			else if(val == "done")
				setBtnType(BTN_DONE);
			else if(val == "tall")
				setBtnType(BTN_TALL);
			else if(val == "trait")
				setBtnType(BTN_TRAIT);
			else if(val == "push")
				setBtnType(BTN_PUSH);
		}else if(name == "color" || name == "colour"){
			std::string val;
			attr->GetValue(&val);
			sf::Color clr;
			try{
				clr = parseColor(val);
			}catch(int){
				throw xBadVal("button",name,val,attr->Row(),attr->Column(),fname);
			}
			setColour(clr);
		}else if(name == "def-key"){
			std::string val;
			attr->GetValue(&val);
			try{
				attachKey(parseKey(val));
			}catch(int){
				throw xBadVal("button",name,val,attr->Row(),attr->Column(),fname);
			}
//		}else if(name == "fromlist"){
//			attr->GetValue(&fromList);
		}else if(name == "top"){
			attr->GetValue(&frame.top), foundTop = true;
		}else if(name == "left"){
			attr->GetValue(&frame.left), foundLeft = true;
		}else if(name == "width"){
			attr->GetValue(&width);
		}else if(name == "height"){
			attr->GetValue(&height);
		}else throw xBadAttr("button",name,attr->Row(),attr->Column(),fname);
	}
	if(parent->getBg() == cDialog::BG_DARK && (getBtnType() == BTN_TINY || getBtnType() == BTN_PUSH))
		setColour(sf::Color::White);
	if(!foundType) throw xMissingAttr("button","type",who.Row(),who.Column(),fname);
	if(!foundTop) throw xMissingAttr("button","top",who.Row(),who.Column(),fname);
	if(!foundLeft) throw xMissingAttr("button","left",who.Row(),who.Column(),fname);
	switch(getBtnType()){
		case BTN_SM:
			frame.right = frame.left + 23;
			frame.bottom = frame.top + 23;
			break;
		case BTN_LG:
			frame.right = frame.left + 102;
			frame.bottom = frame.top + 23;
			break;
		case BTN_HELP:
			frame.right = frame.left + 16;
			frame.bottom = frame.top + 13;
			break;
		case BTN_TINY:
		case BTN_LED: // this should never happen
			frame.right = frame.left + 14;
			frame.bottom = frame.top + 10;
			break;
		case BTN_TALL:
		case BTN_TRAIT:
			frame.right = frame.left + 63;
			frame.bottom = frame.top + 40;
			break;
		case BTN_PUSH:
			frame.right = frame.left + 30;
			frame.bottom = frame.top + 30;
			break;
		default:
			frame.right = frame.left + 63;
			frame.bottom = frame.top + 23;
	}
	if(width > 0)
		frame.right = frame.left + width;
	if(height > 0)
		frame.bottom = frame.top + height;
	setBounds(frame);
	std::string content;
	for(node = node.begin(&who); node != node.end(); node++){
		std::string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::ELEMENT && val == "key"){
			// TODO: There's surely a better way to do this
			if(content.length() > 0) throw xBadVal("button",xBadVal::CONTENT,content + val,node->Row(),node->Column(),fname);
//			labelWithKey = true;
		}else if(type == TiXmlNode::TEXT)
			content += dlogStringFilter(val);
		else if(type != TiXmlNode::COMMENT) {
			val = '<' + val + '>';
			throw xBadVal("button",xBadVal::CONTENT,val,node->Row(),node->Column(),fname);
		}
	}
	setText(content);
	return id;
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

void cLed::setFormat(eFormat prop, short val) throw(xUnsupportedProp){
	if(prop == TXT_FONT) textFont = (eFont) val;
	else if(prop == TXT_SIZE) textSize = val;
	else if(prop == TXT_WRAP) wrapLabel = val;
	else throw xUnsupportedProp(prop);
}

short cLed::getFormat(eFormat prop) throw(xUnsupportedProp){
	if(prop == TXT_FONT) return textFont;
	else if(prop == TXT_SIZE) return textSize;
	else if(prop == TXT_WRAP) return wrapLabel;
	else throw xUnsupportedProp(prop);
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
		rect_draw_some_item(*ResMgr::get<ImageRsrc>(buttons[btnGW[BTN_LED]]),from_rect,*inWindow,to_rect);
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

std::string cLed::parse(ticpp::Element& who, std::string fname) {
	using namespace ticpp;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	std::string name, id;
	int width = 0, height = 0;
	bool foundTop = false, foundLeft = false; // requireds
	rectangle frame;
	if(parent->getBg() == cDialog::BG_DARK)
		setColour(sf::Color::White);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&id);
		else if(name == "state"){
			std::string val;
			attr->GetValue(&val);
			if(val == "red") setState(led_red);
			else if(val == "green") setState(led_green);
			else if(val == "off") setState(led_off);
			else throw xBadVal("led",name,val,attr->Row(),attr->Column(),fname);
//		}else if(name == "fromlist"){
//			attr->GetValue(&fromList);
		}else if(name == "font"){
			std::string val;
			attr->GetValue(&val);
			if(val == "dungeon")
				setFormat(TXT_FONT, FONT_DUNGEON);
			else if(val == "plain")
				setFormat(TXT_FONT, FONT_PLAIN);
			else if(val == "bold")
				setFormat(TXT_FONT, FONT_BOLD);
			else if(val == "maidenword")
				setFormat(TXT_FONT, FONT_MAIDWORD);
			else throw xBadVal("led",name,val,attr->Row(),attr->Column(),fname);
		}else if(name == "size"){
			std::string val;
			attr->GetValue(&val);
			if(val == "large")
				setFormat(TXT_SIZE, 12);
			else if(val == "small")
				setFormat(TXT_SIZE, 10);
			else if(val == "title")
				setFormat(TXT_SIZE, 18);
			else throw xBadVal("led",name,val,attr->Row(),attr->Column(),fname);
		}else if(name == "color" || name == "colour"){
			std::string val;
			attr->GetValue(&val);
			sf::Color clr;
			try{
				clr = parseColor(val);
			}catch(int){
				throw xBadVal("led",name,val,attr->Row(),attr->Column(),fname);
			}
			setColour(clr);
		} else if(name == "wrap") {
			std::string val;
			attr->GetValue(&val);
			if(val == "true")
				setFormat(TXT_WRAP, true);
			else setFormat(TXT_WRAP, false);
		}else if(name == "top"){
			attr->GetValue(&frame.top), foundTop = true;
		}else if(name == "left"){
			attr->GetValue(&frame.left), foundLeft = true;
		}else if(name == "width"){
			attr->GetValue(&width);
		}else if(name == "height"){
			attr->GetValue(&height);
		}else throw xBadAttr("led",name,attr->Row(),attr->Column(),fname);
	}
	if(!foundTop) throw xMissingAttr("led","top",who.Row(),who.Column(),fname);
	if(!foundLeft) throw xMissingAttr("led","left",who.Row(),who.Column(),fname);
	if(width > 0) {
		frame.right = frame.left + width;
	}else{
		frame.right = frame.left + 14;
	}
	if(height > 0) {
		frame.bottom = frame.top + height;
	}else{
		frame.bottom = frame.top + 10;
	}
	setBounds(frame);
	std::string content;
	for(node = node.begin(&who); node != node.end(); node++){
		std::string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::TEXT)
			content += dlogStringFilter(val);
		else if(type != TiXmlNode::COMMENT) {
			val = '<' + val + '>';
			throw xBadVal("led",xBadVal::CONTENT,content + val,node->Row(),node->Column(),fname);
		}
	}
	setText(content);
	return id;
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

void cLedGroup::attachFocusHandler(std::function<bool(cDialog&,std::string,bool)> f) throw(xHandlerNotSupported) {
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

void cLedGroup::setFormat(eFormat prop, short val) throw(xUnsupportedProp) {
	if(prop == TXT_FRAME) drawFramed = val;
	else if(prop == TXT_FRAMESTYLE) frameStyle = eFrameStyle(val);
	else throw xUnsupportedProp(prop);
}

short cLedGroup::getFormat(eFormat prop) throw(xUnsupportedProp) {
	if(prop == TXT_FRAME) return drawFramed;
	else if(prop == TXT_FRAMESTYLE) return frameStyle;
	throw xUnsupportedProp(prop);
}

void cLedGroup::setColour(sf::Color) throw(xUnsupportedProp) {
	// TODO: Colour is not supported
}

sf::Color cLedGroup::getColour() throw(xUnsupportedProp) {
	// TODO: Colour is not supported
	return sf::Color();
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
	if(drawFramed) drawFrame(2, frameStyle);
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
			frame.width() = 30;
			frame.height() = 30;
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

std::string cLedGroup::parse(ticpp::Element& who, std::string fname) {
	using namespace ticpp;
	Iterator<Attribute> attr;
	Iterator<Element> node;
	std::string name, id;
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&id);
//		else if(name == "fromlist")
//			attr->GetValue(&fromList);
		else if(name == "framed"){
			std::string val;
			attr->GetValue(&val);
			if(val == "true") setFormat(TXT_FRAME, true);
		}else if(name == "outline") {
			std::string val;
			attr->GetValue(&val);
			if(val == "solid") setFormat(TXT_FRAMESTYLE, FRM_SOLID);
			else if(val == "inset") setFormat(TXT_FRAMESTYLE, FRM_INSET);
			else if(val == "outset") setFormat(TXT_FRAMESTYLE, FRM_OUTSET);
			else if(val == "double") setFormat(TXT_FRAMESTYLE, FRM_DOUBLE);
		}else throw xBadAttr("group",name,attr->Row(),attr->Column(),fname);
	}
	for(node = node.begin(&who); node != node.end(); node++){
		std::string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::ELEMENT && val == "led"){
			auto led = parent->parse<cLed>(*node);
			addChoice(led.second, led.first);
		}else if(type != TiXmlNode::COMMENT) {
			val = '<' + val + '>';
			throw xBadVal("group",xBadVal::CONTENT,val,node->Row(),node->Column(),fname);
		}
	}
	recalcRect();
	return id;
}
