
/*
 *  button.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "button.h"
#include <vector>
#include <map>
#include <stdexcept>

#include "dialog.h"
#include "graphtool.h"

#include <cmath>

#include "restypes.hpp"

extern sf::Texture bg_gworld;

void cButton::attachFocusHandler(focus_callback_t f __attribute__((unused))) throw(xHandlerNotSupported){
	throw xHandlerNotSupported(true);
}

void cButton::attachClickHandler(click_callback_t f) throw(){
	onClick = f;
}

bool cButton::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, location where){
	if(onClick) return onClick(me,id,mods);
	return false;
}

cButton::cButton(cDialog* parent) :
	cControl(CTRL_BTN,*parent),
	wrapLabel(false),
	type(BTN_REG),
	fromList("none") {}

cButton::cButton(cDialog* parent,eControlType t) :
	cControl(t,*parent),
	fromList("none"),
	wrapLabel("true") {/* This constructor is only called for LEDs. TODO: Should wrapLabel be true for LEDs? */}

bool cButton::isClickable(){
	return true;
}

void cButton::draw(){
	RECT from_rect, to_rect;
	
	inWindow->setActive();
	
	if(visible){
		TEXT.font = "Geneva";
		if(foundSilom()) {
			TEXT.style = sf::Text::Regular;
			TEXT.font = "Silom";
		} else TEXT.style = sf::Text::Bold;
		if(type == BTN_TINY) TEXT.pointSize = 9;
		else if(type == BTN_PUSH) TEXT.pointSize = 10;
		else TEXT.pointSize = 12;
		from_rect = btnRects[type][depressed];
		to_rect = frame;
		rect_draw_some_item(buttons[btnGW[type]],from_rect,*inWindow,to_rect,sf::BlendAlpha);
		TEXT.colour = sf::Color::Black;
		int textMode = 1;
		if(type == BTN_TINY) {
			textMode = 2;
			to_rect.left += 18;
		} else if(type == BTN_PUSH) {
			to_rect.top += 34;
		}
		win_draw_string(*inWindow,to_rect,lbl,textMode,8);
		// TODO: Adjust string location as appropriate
		// Tiny button string location should be shifted right 20 pixels (or possibly 18)
		// Push button string should be centred below the button
		// Others may need adjustments too, not sure
		TEXT.colour = sf::Color::Black;
		// TODO: How is it supposed to know it's a default button when this fact is stored in the dialog, not the button?
		if(key.spec && key.k == key_enter) drawFrame(2,frameStyle); // frame default button, to provide a visual cue that it's the default
	}else{
		tileImage(*inWindow,frame,bg_gworld,bg[parent->bg]);
	}
}

void cButton::setFormat(eFormat prop, short val) throw(xUnsupportedProp){
	if(prop == TXT_WRAP) wrapLabel = val;
	else if(prop == TXT_FRAMESTYLE) frameStyle = val;
	else throw xUnsupportedProp(prop);
}

short cButton::getFormat(eFormat prop) throw(xUnsupportedProp){
	if(prop == TXT_WRAP) return wrapLabel;
	else throw xUnsupportedProp(prop);
}

void cButton::setColour(sf::Color clr) throw(xUnsupportedProp) {
	// TODO: Colour is not supported
}

sf::Color cButton::getColour() throw(xUnsupportedProp) {
	// TODO: Colour is not supported
	return sf::Color();
}

void cButton::setBtnType(eBtnType newType) {
	type = newType;
}

eBtnType cButton::getBtnType() {
	return type;
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

sf::Texture cButton::buttons[7];
RECT cButton::btnRects[13][2];

void cButton::init(){
	static const char*const buttonFiles[7] = {
		"dlogbtnsm",
		"dlogbtnmed",
		"dlogbtnlg",
		"dlogbtntall",
		"dlogbtnhelp",
		"dlogbtnled",
		"dlgbtnred"
	};
	for(int i = 0; i < 7; i++)
		buttons[i].loadFromImage(*ResMgr::get<ImageRsrc>(buttonFiles[i]));
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
	for(int j = 0; j < 12; j++)
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

RECT cLed::ledRects[3][2];

void cLed::init(){
	RECT baseLed = {0,0,10,14};
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 2; j++){
			ledRects[i][j] = baseLed;
			ledRects[i][j].offset(i * 14, j * 10);
		}
}

cLed::cLed(cDialog* parent) :
	cButton(parent,CTRL_LED),
	state(led_off),
	textFont(SILOM),
	textSize(10),
	color(parent->defTextClr) {
	setBtnType(BTN_LED);
}

void cLed::attachClickHandler(click_callback_t f) throw(){
	onClick = f;
}

void cLed::attachFocusHandler(focus_callback_t f) throw(){
	onFocus = f;
}

bool cLed::triggerFocusHandler(cDialog& me, std::string id, bool losing){
	if(onFocus != NULL) return onFocus(me,id,losing);
	return true;
}

bool cLed::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, location where){
	bool result;
	eLedState oldState = state;
	if(onClick != NULL) result = onClick(me,id,mods);
	else{ // simple state toggle
		switch(state){
			case led_red:
			case led_green:
				state = led_off;
				break;
			case led_off:
				state = led_red;
		}
		result = true;
	}
	if(!triggerFocusHandler(me,id, oldState != led_off)){
		result = false;
		state = oldState;
	}
	return result;
}

void cLed::setFormat(eFormat prop __attribute__((unused)), short val __attribute__((unused))) throw(xUnsupportedProp){
	throw xUnsupportedProp(prop);
}

short cLed::getFormat(eFormat prop __attribute__((unused))) throw(xUnsupportedProp){
	throw xUnsupportedProp(prop);
}

void cLed::draw(){
	RECT from_rect, to_rect;
	
	inWindow->setActive();
	
	if(visible){
		TEXT.font = "Geneva";
		if(foundSilom()) TEXT.style = sf::Text::Regular;
		else TEXT.style = sf::Text::Bold;
		TEXT.pointSize = 9;
		from_rect = ledRects[state][depressed];
		to_rect = frame;
		to_rect.right = to_rect.left + 14;
		rect_draw_some_item(buttons[btnGW[BTN_LED]],from_rect,*inWindow,to_rect);
		TEXT.colour = parent->defTextClr;
		to_rect.right = frame.right;
		to_rect.left = frame.left + 18; // Possibly could be 20
		win_draw_string(*inWindow,to_rect,lbl,2,8);
		TEXT.colour = sf::Color::Black;
	}else{
		tileImage(*inWindow,frame,bg_gworld,bg[parent->bg]);
	}
}

cLedGroup::cLedGroup(cDialog* parent) :
	cControl(CTRL_GROUP,*parent),
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
	while(iter != choices.end()){
		RECT otherFrame = iter->second->getBounds();
		if(otherFrame.right > frame.right)
			frame.right = otherFrame.right;
		if(otherFrame.bottom > frame.bottom)
			frame.bottom = otherFrame.bottom;
		iter++;
	}
	frame.right += 6;
	frame.bottom += 6;
}

/** A click handler is called whenever a click is received, even on the currently selected element. */
void cLedGroup::attachClickHandler(click_callback_t f) throw() {
	onClick = f;
}

/** A focus handler is called when the currently selected element changes. */
void cLedGroup::attachFocusHandler(focus_callback_t f) throw() {
	onFocus = f;
}

void cLed::setState(eLedState to){
	state = to;
}

eLedState cLed::getState(){
	return state;
}

void cLedGroup::addChoice(cLed* ctrl, std::string key) {
	choices[key] = ctrl;
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

bool cLedGroup::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, location where){
	std::string which_clicked = clicking;
	clicking = "";
	
	if(choices[which_clicked]->triggerClickHandler(me,which_clicked,mods,where)){
		eLedState a, b;
		a = choices[curSelect]->getState();
		b = choices[which_clicked]->getState();
		choices[curSelect]->setState(led_off);
		choices[which_clicked]->setState(led_red);
		if(!choices[curSelect]->triggerFocusHandler(me,curSelect,true)){
			choices[curSelect]->setState(a);
			choices[which_clicked]->setState(b);
			return false;
		}
		if(!choices[which_clicked]->triggerFocusHandler(me,which_clicked,false)){
			choices[curSelect]->setState(a);
			choices[which_clicked]->setState(b);
			return false;
		}
		curSelect = which_clicked;
	}else return false;
	
	return triggerFocusHandler(me,id,false);
}

bool cLedGroup::triggerFocusHandler(cDialog& me, std::string id, bool losingFocus){
	if(onFocus != NULL) return onFocus(me,id,losingFocus);
	return true;
}

void cLedGroup::disable(std::string id) {
	// TODO: Implement this
}

void cLedGroup::enable(std::string id) {
	// TODO: Implement this
}

void cLedGroup::show(std::string id){
	choices[id]->show();
}

void cLedGroup::hide(std::string id){
	choices[id]->hide();
}

void cLedGroup::setFormat(eFormat prop __attribute__((unused)), short val __attribute__((unused))) throw(xUnsupportedProp) {
	throw xUnsupportedProp(prop);
}

short cLedGroup::getFormat(eFormat prop __attribute__((unused))) throw(xUnsupportedProp) {
	throw xUnsupportedProp(prop);
}

void cLedGroup::setColour(sf::Color clr) throw(xUnsupportedProp) {
	// TODO: Colour is not supported
}

sf::Color cLedGroup::getColour() throw(xUnsupportedProp) {
	// TODO: Colour is not supported
	return sf::Color();
}

bool cLedGroup::isClickable(){
	return true;
}

cLed& cLedGroup::operator[](std::string id){
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
	ledIter iter = choices.begin();
	while(iter != choices.end()){
		iter->second->draw();
		iter++;
	}
}

void cButton::setType(eBtnType newType){
	if(type == BTN_LED) return; // can't change type
	type = newType;
}

eBtnType cButton::getType(){
	return type;
}
