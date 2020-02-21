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
			case TXT_FONT:
				s = "TXT_FONT";
				break;
			case TXT_SIZE:
				s = "TXT_SIZE";
				break;
			case TXT_WRAP:
				s = "TXT_WRAP";
				break;
			case TXT_COLOUR:
				s = "TXT_COLOUR";
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

void cControl::setFormat(eFormat prop, short val) {
	boost::any newVal;
	switch(prop) {
		case TXT_WRAP:
			newVal = bool(val);
			break;
		case TXT_FONT:
			newVal = eFont(val);
			break;
		case TXT_SIZE:
			newVal = val;
			break;
		case TXT_FRAME:
			newVal = eFrameStyle(val);
			break;
		case TXT_COLOUR: // Interpret as a shade of grey
			newVal = sf::Color{val, val, val};
			break;
	}
	if(!manageFormat(prop, true, &newVal))
		throw xUnsupportedProp(prop);
}

short cControl::getFormat(eFormat prop) {
	boost::any curVal;
	if(!manageFormat(prop, false, &curVal))
		throw xUnsupportedProp(prop);
	switch(prop) {
		case TXT_WRAP:
			return boost::any_cast<bool>(curVal);
		case TXT_FONT:
			return boost::any_cast<eFont>(curVal);
		case TXT_SIZE:
			return boost::any_cast<short>(curVal);
		case TXT_FRAME:
			return boost::any_cast<eFrameStyle>(curVal);
		case TXT_COLOUR: // Interpret as a shade of grey
			return boost::any_cast<sf::Color>(curVal).toInteger();
	}
	return 0;
}

bool cControl::canFormat(eFormat prop) {
	return manageFormat(prop, false, nullptr);
}

void cControl::setColour(sf::Color clr) {
	boost::any newVal = clr;
	if(!manageFormat(TXT_COLOUR, true, &newVal))
		throw xUnsupportedProp(TXT_COLOUR);
}

sf::Color cControl::getColour() {
	boost::any curVal;
	if(!manageFormat(TXT_COLOUR, false, &curVal))
		throw xUnsupportedProp(TXT_COLOUR);
	return boost::any_cast<sf::Color>(curVal);
}

bool cControl::manageFormat(eFormat, bool, boost::any*) {
	return false;
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
			location clickPos(e.mouseButton.x, e.mouseButton.y);
			clickPos = inWindow->mapPixelToCoords(clickPos);
			clicked = frame.contains(clickPos);
			depressed = false;
		} else if(e.type == sf::Event::MouseMoved){
			restore_cursor();
			location toPos(e.mouseMove.x, e.mouseMove.y);
			toPos = inWindow->mapPixelToCoords(toPos);
			depressed = frame.contains(toPos);
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


void cControl::attachClickHandler(std::function<bool(cDialog&,std::string,eKeyMod)> f) {
	if(!f) {
		attachEventHandler<EVT_CLICK>(nullptr);
		return;
	}
	attachEventHandler<EVT_CLICK>([f](cDialog& me, std::string id, eKeyMod mods) {
		f(me, id, mods);
	});
}

void cControl::attachFocusHandler(std::function<bool(cDialog&,std::string,bool)> f) {
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
	if(frameStyle == FRM_NONE) return;
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

std::string cControl::parse(ticpp::Element& who, std::string fname) {
	using namespace ticpp;
	std::string tagName, id;
	who.GetValue(&tagName);
	Iterator<Attribute> attr;
	Iterator<Node> node;
	std::set<std::string> foundAttrs;
	std::multiset<std::string> foundNodes;
	int width = 0, height = 0;
	rectangle frame;
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		std::string attrName = attr->Name();
		foundAttrs.insert(attrName);
		if(attrName == "name") attr->GetValue(&id);
		else if(attrName == "top") attr->GetValue(&frame.top);
		else if(attrName == "left") attr->GetValue(&frame.left);
		else if(attrName == "width") attr->GetValue(&width);
		else if(attrName == "height") attr->GetValue(&height);
		else if(!parseAttribute(*attr, tagName, fname))
			throw xBadAttr(tagName, attrName, attr->Row(), attr->Column(), fname);
	}
	std::string text;
	for(node = node.begin(&who); node != node.end(); node++){
		int type = node->Type();
		std::string nodeTag;
		if(type == TiXmlNode::ELEMENT)
			nodeTag = node->Value();
		if(type == TiXmlNode::COMMENT) continue;
		else if(!parseContent(*node, foundNodes.count(nodeTag), tagName, fname, text)) {
			std::string val = nodeTag.empty() ? nodeTag : xBadVal::CONTENT;
			throw xBadVal(tagName, xBadVal::CONTENT, val, node->Row(), node->Column(), fname);
		}
		foundNodes.insert(nodeTag);
	}
	setText(text);
	location bestSz = getPreferredSize();
	frame.width() = width > 0 ? width : bestSz.x;
	frame.height() = height > 0 ? height : bestSz.y;
	setBounds(frame);
	validatePostParse(who, fname, foundAttrs, foundNodes);
	return id;
}

bool cControl::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	std::string name;
	attr.GetName(&name);
	// Colour and formatting, if supported
	if(name == "framed" && canFormat(TXT_FRAME)) {
		std::string val;
		attr.GetValue(&val);
		if(val == "true") setFormat(TXT_FRAME, FRM_SOLID);
		else if(val == "false") setFormat(TXT_FRAME, FRM_NONE);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}
	if(name == "outline" && canFormat(TXT_FRAME)) {
		std::string val;
		attr.GetValue(&val);
		if(val == "none") setFormat(TXT_FRAME, FRM_NONE);
		else if(val == "solid") setFormat(TXT_FRAME, FRM_SOLID);
		else if(val == "inset") setFormat(TXT_FRAME, FRM_INSET);
		else if(val == "outset") setFormat(TXT_FRAME, FRM_OUTSET);
		else if(val == "double") setFormat(TXT_FRAME, FRM_DOUBLE);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}
	if(name == "font" && canFormat(TXT_FONT)) {
		std::string val;
		attr.GetValue(&val);
		if(val == "plain") setFormat(TXT_FONT, FONT_PLAIN);
		else if(val == "bold") setFormat(TXT_FONT, FONT_BOLD);
		else if(val == "dungeon") setFormat(TXT_FONT, FONT_DUNGEON);
		else if(val == "maidenword") setFormat(TXT_FONT, FONT_MAIDWORD);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}
	if(name == "size" && canFormat(TXT_SIZE)) {
		std::string val;
		attr.GetValue(&val);
		if(val == "small") setFormat(TXT_SIZE, 10);
		else if(val == "large") setFormat(TXT_SIZE, 12);
		else if(val == "title") setFormat(TXT_SIZE, 18);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}
	if(name == "wrap" && canFormat(TXT_WRAP)) {
		std::string val;
		attr.GetValue(&val);
		if(val == "true") setFormat(TXT_WRAP, true);
		else if(val == "false") setFormat(TXT_WRAP, false);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}
	if((name == "color" || name == "colour") && canFormat(TXT_COLOUR)) {
		std::string val;
		try{
			sf::Color clr = parseColor(val);
			setColour(clr);
		} catch(int) {
			throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		}
		return true;
	}
	return false;
}


bool cControl::parseContent(ticpp::Node&, int, std::string, std::string, std::string&) {
	return false;
}

void cControl::validatePostParse(ticpp::Element& elem, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>&) {
	if(!attrs.count("left")) throw xMissingAttr(elem.Value(), "left", elem.Row(), elem.Column(), fname);
	if(!attrs.count("top")) throw xMissingAttr(elem.Value(), "top", elem.Row(), elem.Column(), fname);
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
