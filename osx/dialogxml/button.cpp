/*
 *  button.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include <vector>
#include <map>
using namespace std;

#include "dialog.h"
#include "graphtool.h"

void cButton::attachFocusHandler(focus_callback_t f __attribute__((unused))) throw(xHandlerNotSupported){
	throw xHandlerNotSupported(true);
}

void cButton::attachClickHandler(click_callback_t f) throw(){
	onClick = f;
}

bool cButton::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods){
	if(onClick != NULL) return onClick(me,id,mods);
	return false;
}

cButton::cButton(cDialog* parent) : cControl(parent) {}

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
		FillCRect(&frame,bg[parent->bg]);
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
	for(int i = 0; i < 7; i++)
		buttons[i] = load_pict(i + 2030);
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

Rect cLed::ledRects[3][2];

void cLed::init(){
	Rect baseLed = {0,0,10,14};
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 2; j++){
			ledRects[i][j] = baseLed;
			OffsetRect(&ledRects[i][j],i * 14, j * 10);
		}
}

cLed::cLed(cDialog* parent) : cButton(parent) {}

void cLed::attachClickHandler(click_callback_t f) throw(){
	onClick = f;
}

void cLed::attachFocusHandler(focus_callback_t __attribute__((unused))) throw(xHandlerNotSupported){
	throw xHandlerNotSupported(true);
}

bool cLed::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods){
	if(onClick != NULL) return onClick(me,id,mods);
	return false;
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
		FillCRect(&frame,bg[parent->bg]);
	}
	
	SetPort(old_port);
}

cLedGroup::cLedGroup(cDialog* parent) : cControl(parent) {}

cButton::~cButton() {}

cLed::~cLed() {}

cLedGroup::~cLedGroup(){
	ledIter iter = choices.begin();
	while(iter != choices.end()){
		delete iter->second;
		iter++;
	}
}
