/*
 *  control.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include <sstream>
#include "dialog.h"
#include "soundtool.h"

extern bool play_sounds;

void cControl::setLabel(std::string l){
	lbl = l;
	if(isVisible()) draw();
}

std::string cControl::getLabel(){
	return lbl;
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

void cControl::setLabelToKey(){
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
		msg = new char[60];
		std::string s;
		switch(whichProp){
			case TXT_COLOR:
				s = "TXT_COLOR";
				break;
			case TXT_FRAME:
				s = "TXT_FRAME";
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

bool operator== (cKey& a, cKey& b){
	if(a.spec != b.spec) return false;
	if(a.mod != b.mod) return false;
	return a.spec ? a.k == b.k : a.c == b.c;
}

void cControl::show(){
	visible = true;
	if(isVisible()) draw();
}

void cControl::hide(){
	visible = false;
	if(isVisible()) draw();
}

bool cControl::isVisible(){
	if(parent->dialogNotToast)
		return visible;
	else return false;
}

bool cControl::handleClick(){
	EventRecord e;
	unsigned long dummy;
	bool done = false, clicked = false;
	GrafPtr old_port;
	GetPort(&old_port);
	SetPortWindowPort(parent->win);
	RgnHandle in_region = NewRgn(), out_region = NewRgn();
	RectRgn(in_region,&frame);
	RectRgn(out_region,&parent->winRect);
	DiffRgn(out_region,in_region,out_region);
	depressed = true;
	draw();
	while(!done){
		WaitNextEvent(mUpMask,&e,0L,depressed ? in_region : out_region);
		if(e.what == mouseUp){
			done = true;
			clicked = PtInRect(e.where, &frame);
			depressed = false;
		}else if(e.message >> 8 == mouseMovedMessage){
			depressed = !depressed;
			draw();
		}
	}
	if (play_sounds) {
		if(typeid(this) == typeid(cLed*))
			play_sound(34);
		else play_sound(37);
		Delay(6,&dummy);
	}
	else Delay(14,&dummy);
	draw();
	SetPort(old_port);
	return clicked;
}

cControl::cControl(cDialog* p){
	parent = p;
}

bool cControl::triggerClickHandler(cDialog& me __attribute__((unused)), std::string id __attribute__((unused)), eKeyMod mods __attribute__((unused))){
	return true;
}

bool cControl::triggerFocusHandler(cDialog& me __attribute__((unused)), std::string id __attribute__((unused)), bool losingFocus __attribute__((unused))){
	return true;
}

short cControl::font_nums[4];

void cControl::init(){
	Str255 fnGeneva = "\pGeneva";
	Str255 fnDungeon = "\pDungeon Bold";
	Str255 fnMaiden = "\pMaidenWord";
	Str255 fnSilom = "\pSilom";
	Str255 fnPalatino = "\pPalatino";
	Str255 fnChancery = "\pApple Chancery";
	
	GetFNum(fnGeneva,&font_nums[GENEVA]);
	if(font_nums[GENEVA] == 0)
		GetFNum(fnPalatino,&font_nums[GENEVA]);
	GetFNum(fnDungeon,&font_nums[DUNGEON]);
	if(font_nums[DUNGEON] == 0)
		GetFNum(fnChancery,&font_nums[DUNGEON]);
	GetFNum(fnSilom,&font_nums[SILOM]);
	if(font_nums[SILOM] == 0){
		GetFNum(fnPalatino,&font_nums[SILOM]);
		found_silom = false;
	}else found_silom = true;
	GetFNum(fnMaiden,&font_nums[MAIDENWORD]);
	if(font_nums[MAIDENWORD] == 0)
		GetFNum(fnChancery,&font_nums[MAIDENWORD]);
}

void cControl::drawFrame(short amt, short med_or_lt){
	static RGBColor lt_gray = {57344,57344,57344},dk_gray = {12287,12287,12287},med_gray = {24574,24574,24574};
	GrafPtr old_port;
	Rect rect = frame;
	
	GetPort(&old_port);
	SetPortWindowPort(parent->win);
	
	InsetRect(&rect,-1 * amt,-1 * amt);
	
	RGBForeColor(&dk_gray);
	MoveTo(rect.left,rect.top);
	LineTo(rect.right,rect.top);
	if (med_or_lt == 1) // hDlg == GetWindowPort(mainPtr) // ie for the pc editor only
		RGBForeColor(&med_gray);
	else RGBForeColor(&lt_gray);
	LineTo(rect.right,rect.bottom);
	LineTo(rect.left,rect.bottom);
	RGBForeColor(&dk_gray);
	LineTo(rect.left,rect.top);
	ForeColor(blackColor);
	SetPort(old_port);
}

bool cControl::found_silom;

bool cControl::foundSilom(){
	return found_silom;
}

cControl::~cControl() {}
