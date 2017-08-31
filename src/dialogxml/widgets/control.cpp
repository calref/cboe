/*
 *  control.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "control.hpp"
#include <sstream>
#include "dialog.hpp"
#include "sounds.hpp"
#include "button.hpp"
#include "render_shapes.hpp"
#include "res_image.hpp"
#include "mathutil.hpp"
#include "prefs.hpp"
#include "cursors.hpp"

void cControl::setText(std::string l){
	lbl = l;
}

std::string cControl::getText(){
	return lbl;
}

rectangle cControl::getBounds() {
	return frame;
}

void cControl::setBounds(rectangle newFrame) {
	frame = newFrame;
}

void cControl::relocate(location to) {
	frame.offset(to.x - frame.left, to.y - frame.top);
}

const char* xHandlerNotSupported::msg[4] = {
	"This control cannot handle click events.\n",
	"This control cannot handle focus events.\n",
	"This control cannot handle defocus events.\n",
	"This control cannot handle scroll events.\n",
};

xHandlerNotSupported::xHandlerNotSupported(eDlogEvt t){
	this->evt = t;
}
const char* xHandlerNotSupported::what() const throw() {
	assert("A handler not supported message is missing!" && evt < 4);
	return msg[evt];
}

xUnsupportedProp::xUnsupportedProp(eFormat prop) throw(){
	whichProp = prop;
	msg = nullptr;
}
xUnsupportedProp::~xUnsupportedProp() throw(){
	if(msg != nullptr) delete msg;
}
const char* xUnsupportedProp::what() const throw(){
	if(msg == nullptr){
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
			restore_cursor();
			depressed = frame.contains(e.mouseMove.x, e.mouseMove.y);
		}
	}
	if(get_bool_pref("PlaySounds", true)) {
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

void cControl::setTextToKey() {
	if(key.spec); // TODO: Handle this case
	else {
		unsigned char c = key.c;
		if(key.mod - mod_shift != key.mod) c = applyShift(c);
		else c = removeShift(c);
		lbl.clear();
		if(key.mod - mod_ctrl != key.mod) lbl += '^';
		else if(key.mod - mod_alt != key.mod) lbl = '*';
		lbl += c;
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

cControl::cControl(eControlType t, cDialog& p) : parent(&p), inWindow(&p.win), type(t), visible(true), key({false, 0, mod_none}), frameStyle(FRM_INSET) {}

cControl::cControl(eControlType t, sf::RenderWindow& p) : parent(nullptr), inWindow(&p), type(t), visible(true), key({false, 0, mod_none}), frameStyle(FRM_INSET) {}


void cControl::attachClickHandler(std::function<bool(cDialog&,std::string,eKeyMod)> f) throw(xHandlerNotSupported) {
	if(!f) {
		attachEventHandler<EVT_CLICK>(nullptr);
		return;
	}
	attachEventHandler<EVT_CLICK>([f](cDialog& me, std::string id, eKeyMod mods) {
		f(me, id, mods);
	});
}

void cControl::attachFocusHandler(std::function<bool(cDialog&,std::string,bool)> f) throw(xHandlerNotSupported) {
	if(!f) {
		attachEventHandler<EVT_FOCUS>(nullptr);
		attachEventHandler<EVT_DEFOCUS>(nullptr);
		return;
	}
	using namespace std::placeholders;
	attachEventHandler<EVT_FOCUS>([f](cDialog& me, std::string id) {
		f(me, id, false);
	});
	attachEventHandler<EVT_DEFOCUS>(std::bind(f, _1, _2, true));
}

bool cControl::triggerClickHandler(cDialog& dlg, std::string id, eKeyMod mods){
	triggerEvent<EVT_CLICK>(dlg, id, mods);
	return true;
}

bool cControl::triggerFocusHandler(cDialog& dlg, std::string id, bool losing){
	if(losing) return triggerEvent<EVT_DEFOCUS>(dlg, id);
	triggerEvent<EVT_FOCUS>(dlg, id);
	return true;
}

void cControl::drawFrame(short amt, eFrameStyle frameStyle){
	// dk_gray had a 0..65535 component of 12287, and med_gray had a 0..65535 component of 24574
	static sf::Color lt_gray = {224,224,224},dk_gray = {48,48,48};
	rectangle rect = frame, ul_rect;
	
	inWindow->setActive();
	
	rect.inset(-amt,-amt);
	ul_rect = rect;
	
	if(frameStyle == FRM_OUTSET) {
		ul_rect.right -= 1;
		ul_rect.bottom -= 1;
	} else if(frameStyle == FRM_INSET) {
		ul_rect.top += 1;
		ul_rect.left += 1;
	}
	
	frame_rect(*inWindow, rect, dk_gray);
	if(frameStyle == FRM_OUTSET || frameStyle == FRM_INSET) {
		clip_rect(*inWindow, ul_rect);
		frame_rect(*inWindow, rect, lt_gray);
		undo_clip(*inWindow);
	} else if(frameStyle == FRM_DOUBLE) {
		rect.inset(-amt, -amt);
		frame_rect(*inWindow, rect, dk_gray);
	}
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

cControl::storage_t cControl::store() {
	storage_t storage;
	storage["text"] = lbl;
	return storage;
}

void cControl::restore(storage_t to) {
	if(to.find("text") != to.end())
		lbl = boost::any_cast<std::string>(to["text"]);
	else lbl = "";
}
