/*
 *  button.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include <Carbon/Carbon.h>
#include <vector>
#include <map>
#include <stdexcept>

#include "dialog.h"
#include "graphtool.h"

#include <cmath>

extern GWorldPtr bg_gworld;

void cButton::attachFocusHandler(focus_callback_t f __attribute__((unused))) throw(xHandlerNotSupported){
	throw xHandlerNotSupported(true);
}

void cButton::attachClickHandler(click_callback_t f) throw(){
	onClick = f;
}

bool cButton::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, Point where){
	if(onClick != NULL) return onClick(me,id,mods);
	return false;
}

cButton::cButton(cDialog* parent) : cControl(parent,CTRL_BTN) {}

cButton::cButton(cDialog* parent,eControlType t) : cControl(parent,t) {}

bool cButton::isClickable(){
	return true;
}

void cButton::draw(){
	GrafPtr old_port;
	Rect from_rect, to_rect;
	GWorldPtr from_gw, to_gw;
	
	GetPort(&old_port);
	SetPortWindowPort(parent->win);
	
	if(visible){
		TextFont(font_nums[GENEVA]);
		if(foundSilom())TextFace(normal);
		else TextFace(bold);
		if(type == BTN_TINY) TextSize(9);
		else if(type == BTN_PUSH) TextSize(10);
		else TextSize(12);
		from_gw = buttons[btnGW[type]];
		to_gw = (GWorldPtr) parent->win;
		from_rect = btnRects[btnGW[type]][depressed];
		to_rect = frame;
		rect_draw_some_item(from_gw,from_rect,to_gw,to_rect,0,2);
		RGBForeColor(&parent->defTextClr);
		char_win_draw_string(parent->win,to_rect,lbl.c_str(),1,8,false);
		// TODO: Adjust string location as appropriate
		// Tiny button string location should be shifted right 20 pixels (or possibly 18)
		// Push button string should be centred below the button
		// Others may need adjustments too, not sure
		ForeColor(blackColor);
		if(key.spec && key.k == key_enter) drawFrame(2,0); // frame default button, to provide a visual cue that it's the default
	}else{
		tileImage(frame,bg_gworld,bg[parent->bg]);
	}
	
	SetPort(old_port);
}

void cButton::setFormat(eFormat prop, short val) throw(xUnsupportedProp){
	if(prop == TXT_WRAP) wrapLabel = val;
	else throw xUnsupportedProp(prop);
	if(isVisible()) draw();
}

short cButton::getFormat(eFormat prop) throw(xUnsupportedProp){
	if(prop == TXT_WRAP) return wrapLabel;
	else throw xUnsupportedProp(prop);
}

// Indices within the buttons array.
size_t cButton::btnGW[13] = {
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
};

GWorldPtr cButton::buttons[7] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};
Rect cButton::btnRects[13][2];

void cButton::init(){
	static const char*const buttonFiles[7] = {
		"dlogbtnsm.png",
		"dlogbtnmed.png",
		"dlogbtnlg.png",
		"dlogbtntall.png",
		"dlogbtnhelp.png",
		"dlogbtnled.png",
		"dlogbtnred.png"
	};
	for(int i = 0; i < 7; i++)
		buttons[i] = load_pict(buttonFiles[i]);
	SetRect(&btnRects[BTN_SM][0],0,0,23,23);
	SetRect(&btnRects[BTN_REG][0],0,0,63,23);
	SetRect(&btnRects[BTN_LEFT][0],0,23,63,46);
	SetRect(&btnRects[BTN_RIGHT][0],0,46,63,69);
	SetRect(&btnRects[BTN_UP][0],0,69,63,92);
	SetRect(&btnRects[BTN_DOWN][0],0,92,63,115);
	SetRect(&btnRects[BTN_DONE][0],0,115,63,138);
	SetRect(&btnRects[BTN_LG][0],0,0,104,23);
	SetRect(&btnRects[BTN_HELP][0],0,0,16,13);
	SetRect(&btnRects[BTN_TINY][0],42,0,56,10);
	SetRect(&btnRects[BTN_TALL][0],0,0,63,40);
	SetRect(&btnRects[BTN_TRAIT][0],0,40,63,80);
	SetRect(&btnRects[BTN_PUSH][0],0,0,30,30);
	for(int j = 0; j < 12; j++)
		btnRects[j][1] = btnRects[j][0];
	OffsetRect(&btnRects[BTN_SM][1],23,0);
	OffsetRect(&btnRects[BTN_REG][1],63,0);
	OffsetRect(&btnRects[BTN_LEFT][1],63,0);
	OffsetRect(&btnRects[BTN_RIGHT][1],63,0);
	OffsetRect(&btnRects[BTN_UP][1],63,0);
	OffsetRect(&btnRects[BTN_DOWN][1],63,0);
	OffsetRect(&btnRects[BTN_DONE][1],63,0);
	OffsetRect(&btnRects[BTN_LG][1],104,0);
	OffsetRect(&btnRects[BTN_HELP][1],16,0);
	OffsetRect(&btnRects[BTN_TINY][1],0,10);
	OffsetRect(&btnRects[BTN_TALL][1],63,0);
	OffsetRect(&btnRects[BTN_TRAIT][1],63,0);
	OffsetRect(&btnRects[BTN_PUSH][1],30,0);
}

void cButton::finalize(){
	for(int i = 0; i < 7; i++)
		DisposeGWorld(buttons[i]);
}

Rect cLed::ledRects[3][2];

void cLed::init(){
	Rect baseLed = {0,0,10,14};
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 2; j++){
			ledRects[i][j] = baseLed;
			OffsetRect(&ledRects[i][j],i * 14, j * 10);
		}
}

cLed::cLed(cDialog* parent) : cButton(parent,CTRL_LED) {}

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

bool cLed::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, Point where){
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
	GrafPtr old_port;
	Rect from_rect, to_rect;
	GWorldPtr from_gw, to_gw;
	
	GetPort(&old_port);
	SetPortWindowPort(parent->win);
	
	if(visible){
		TextFont(font_nums[GENEVA]);
		if(foundSilom())TextFace(normal);
		else TextFace(bold);
		TextSize(9);
		from_gw = buttons[BTN_LED];
		to_gw = (GWorldPtr) parent->win;
		from_rect = ledRects[state][depressed];
		to_rect = frame;
		rect_draw_some_item(from_gw,from_rect,to_gw,to_rect,0,2);
		RGBForeColor(&parent->defTextClr);
		char_win_draw_string(parent->win,to_rect,lbl.c_str(),1,8,false);
		// TODO: Adjust string location as appropriate
		// String location should be shifted right 20 pixels (or possibly 18)
		ForeColor(blackColor);
	}else{
		tileImage(frame,bg_gworld,bg[parent->bg]);
	}
	
	SetPort(old_port);
}

cLedGroup::cLedGroup(cDialog* parent) : cControl(parent,CTRL_GROUP) {}

cButton::~cButton() {}

cLed::~cLed() {}

cLedGroup::~cLedGroup(){
	ledIter iter = choices.begin();
	while(iter != choices.end()){
		delete iter->second;
		iter++;
	}
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

bool cLedGroup::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, Point where){
	std::string which_clicked;
	ledIter iter = choices.begin();
	while(iter != choices.end()){
		if(iter->second->visible && PtInRect(where,&iter->second->frame)){
			if(iter->second->handleClick())
				which_clicked = iter->first;
		}
		iter++;
	}
	
	if(which_clicked == "") return false;
	
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
