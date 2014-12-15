/*
 *  control.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "control.h"
#include <sstream>
#include "dialog.h"
#include "soundtool.h"
#include "button.h"
#include "graphtool.h"
#include "restypes.hpp"
#include "mathutil.h"

extern bool play_sounds;

void cControl::setText(std::string l){
	lbl = l;
}

std::string cControl::getText(){
	return lbl;
}

RECT cControl::getBounds() {
	return frame;
}

void cControl::setBounds(RECT newFrame) {
	frame = newFrame;
}

void cControl::relocate(location to) {
	frame.offset(to.x - frame.left, to.y - frame.top);
}

const char* xHandlerNotSupported::focusMsg = "This control cannot handle focus events.\n";
const char* xHandlerNotSupported::clickMsg = "This control cannot handle click events.\n";

xHandlerNotSupported::xHandlerNotSupported(bool isFocus){
	this->isFocus = isFocus;
}
const char* xHandlerNotSupported::what(){
	if(isFocus) return focusMsg;
	else return clickMsg;
}

xUnsupportedProp::xUnsupportedProp(eFormat prop) throw(){
	whichProp = prop;
	msg = NULL;
}
xUnsupportedProp::~xUnsupportedProp() throw(){
	if(msg != NULL) delete msg;
}
const char* xUnsupportedProp::what() throw(){
	if(msg == NULL){
		msg = new char[62];
		std::string s;
		switch(whichProp){
			case TXT_FRAME:
				s = "TXT_FRAME";
				break;
			case TXT_FRAMESTYLE:
				s = "TXT_FRAMESTYLE";
				break;
			case TXT_FONT:
				s = "TXT_FONT";
				break;
			case TXT_SIZE:
				s = "TXT_SIZE";
				break;
			case TXT_WRAP:
				s = "TXT_WRAP";
				break;
		}
		// TODO: Support colour, which doesn't use the setFormat function
		sprintf(msg,"Format property %s not valid for this control.\n",s.c_str());
	}
	return msg;
}

eKeyMod operator + (eKeyMod lhs, eKeyMod rhs){
	if(lhs == rhs) return lhs;
	else if(lhs == mod_none) return rhs;
	else if(lhs == mod_alt){
		if(rhs == mod_shift || rhs == mod_altshift) return mod_altshift;
		else if(rhs == mod_ctrl || rhs == mod_altctrl) return mod_altctrl;
		else if(rhs == mod_shiftctrl || rhs == mod_all) return mod_all;
		else return mod_alt;
	}else if(lhs == mod_shift){
		if(rhs == mod_alt || rhs == mod_altshift) return mod_altshift;
		else if(rhs == mod_ctrl || rhs == mod_shiftctrl) return mod_shiftctrl;
		else if(rhs == mod_altctrl || rhs == mod_all) return mod_all;
		else return mod_shift;
	}else if(lhs == mod_ctrl){
		if(rhs == mod_alt || rhs == mod_altctrl) return mod_altctrl;
		else if(rhs == mod_shift || rhs == mod_shiftctrl) return mod_shiftctrl;
		else if(rhs == mod_altshift || rhs == mod_all) return mod_all;
		else return mod_ctrl;
	}else return rhs + lhs;
}

eKeyMod operator - (eKeyMod lhs, eKeyMod rhs){
	if(lhs == rhs || lhs == mod_none || rhs == mod_all) return mod_none;
	else if(rhs == mod_none) return lhs;
	else if(lhs == mod_all){
		if(rhs == mod_alt) return mod_shiftctrl;
		else if(rhs == mod_shift) return mod_altctrl;
		else if(rhs == mod_ctrl) return mod_altshift;
		else if(rhs == mod_altshift) return mod_ctrl;
		else if(rhs == mod_altctrl) return mod_shift;
		else if(rhs == mod_shiftctrl) return mod_alt;
		else return mod_all;
	}else if(lhs == mod_shiftctrl){
		if(rhs == mod_shift || rhs == mod_altshift) return mod_ctrl;
		else if(rhs == mod_ctrl || rhs == mod_altctrl) return mod_shift;
		else return mod_shiftctrl;
	}else if(lhs == mod_altctrl){
		if(rhs == mod_alt || rhs == mod_altshift) return mod_ctrl;
		else if(rhs == mod_ctrl || rhs == mod_shiftctrl) return mod_alt;
		else return mod_altctrl;
	}else if(lhs == mod_altshift){
		if(rhs == mod_alt || rhs == mod_altctrl) return mod_shift;
		else if(rhs == mod_shift || rhs == mod_shiftctrl) return mod_alt;
		else return mod_altshift;
	}else if(lhs == mod_alt && (rhs == mod_altshift || rhs == mod_altctrl))
		return mod_none;
	else if(lhs == mod_shift && (rhs == mod_altshift || rhs == mod_shiftctrl))
		return mod_none;
	else if(lhs == mod_ctrl && (rhs == mod_altctrl || rhs == mod_shiftctrl))
		return mod_none;
	else return lhs;
}

eKeyMod& operator += (eKeyMod&lhs, eKeyMod rhs){
	lhs = lhs + rhs;
	return lhs;
}

eKeyMod& operator -= (eKeyMod&lhs, eKeyMod rhs){
	lhs = lhs - rhs;
	return lhs;
}

bool operator== (cKey a, cKey b){
	if(a.spec != b.spec) return false;
	if(a.mod != b.mod) return false;
	return a.spec ? a.k == b.k : (a.c == 0 ? false : a.c == b.c);
}

bool mod_contains(eKeyMod mods, eKeyMod mod) {
	if((mods & mod) != 0) return true;
	return false;
}

void cControl::show(){
	visible = true;
	if(labelCtrl) labelCtrl->show();
}

void cControl::hide(){
	visible = false;
	if(labelCtrl) labelCtrl->hide();
}

bool cControl::isVisible(){
	if(!parent || parent->dialogNotToast)
		return visible;
	else return false;
}

void cControl::setLabelCtrl(cControl* label) {
	labelCtrl = label;
}

cKey cControl::getAttachedKey() {
	return key;
}

void cControl::setActive(bool active) {
	depressed = active;
}

void cControl::redraw() {
	// If there's no parent dialog, we're not responsible for redrawing
	if(parent) parent->draw();
}

bool cControl::handleClick(location){
	sf::Event e;
	bool done = false, clicked = false;
	inWindow->setActive();
	depressed = true;
	while(!done){
		redraw();
		if(!inWindow->pollEvent(e)) continue;
		if(e.type == sf::Event::MouseButtonReleased){
			done = true;
			clicked = frame.contains(e.mouseButton.x, e.mouseButton.y);
			depressed = false;
		} else if(e.type == sf::Event::MouseMoved){
			depressed = frame.contains(e.mouseMove.x, e.mouseMove.y);
		}
	}
	if(play_sounds) {
		if(typeid(this) == typeid(cLed*))
			play_sound(34);
		else play_sound(37);
		sf::sleep(time_in_ticks(6));
	}
	else sf::sleep(time_in_ticks(14));
	redraw();
	return clicked;
}

static unsigned char applyShift(unsigned char c){
	static const char afterShift[] = {
		' ', '!', '"', '#', '$', '%', '&', '"', '(', ')', '*', '+', '<', '_', '>', '?',
		')', '!', '@', '#', '$', '%', '^', '&', '*', '(', ':', ':', '<', '+', '>', '?',
		'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '^', '_',
		'~', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '~',
	};
	return afterShift[c - ' '];
}

static unsigned char removeShift(unsigned char c){
	static const char afterUnShift[] = {
		' ', '1', '\'','3', '4', '5', '7', '\'','9', '0', '8', '=', ',', '-', '.', '/',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ';', ';', ',', '=', '.', '/',
		'2', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
		'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\',']', '6', '-',
		'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
		'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\',']', '`',
	};
	return afterUnShift[c - ' '];
}

void cControl::setTextToKey(){
	if(key.spec); // TODO: Handle this case
	else{
		unsigned char c = key.c;
		if(key.mod - mod_shift != key.mod) c = applyShift(c);
		else c = removeShift(c);
		if(key.mod - mod_ctrl != key.mod) lbl = "^" + c;
		else if(key.mod - mod_alt != key.mod) lbl = "*" + c;
		else lbl = c;
	}
	if(isVisible()) draw();
}

void cControl::attachKey(cKey key){
	this->key = key;
}

void cControl::detachKey(){
	this->key.spec = false;
	this->key.c = 0;
}

cControl::cControl(eControlType t, cDialog& p) : parent(&p), inWindow(&p.win), type(t), visible(true), key({false, 0, mod_none}), frameStyle(0) {}

cControl::cControl(eControlType t, sf::RenderWindow& p) : parent(NULL), inWindow(&p), type(t), visible(true), key({false, 0, mod_none}), frameStyle(0) {}

bool cControl::triggerClickHandler(cDialog&, std::string, eKeyMod){
	return true;
}

bool cControl::triggerFocusHandler(cDialog&, std::string, bool){
	return true;
}

void cControl::drawFrame(short amt, bool med_or_lt){
	// dk_gray had a 0..65535 component of 12287, and med_gray had a 0..65535 component of 24574
	static sf::Color lt_gray = {224,224,224},dk_gray = {48,48,48},med_gray = {96,96,96};
	RECT rect = frame, ul_rect;
	
	inWindow->setActive();
	
	rect.inset(-amt,-amt);
	ul_rect = rect;
	ul_rect.left -= 1;
	ul_rect.top -= 1;
	
	frame_rect(*inWindow, rect, med_or_lt ? med_gray : lt_gray);
	clip_rect(*inWindow, ul_rect);
	frame_rect(*inWindow, rect, dk_gray);
	undo_clip(*inWindow);
}

cControl::~cControl() {}

eControlType cControl::getType(){
	return type;
}

void cControl::setTextToNum(long long what){
	std::ostringstream sout;
	sout << what;
	setText(sout.str());
}

long long cControl::getTextAsNum(){
	std::istringstream sin(getText());
	long long n;
	sin >> n;
	return n;
}

bool cControl::hasKey(){
	if(key.spec) return true;
	return key.c != 0;
}
