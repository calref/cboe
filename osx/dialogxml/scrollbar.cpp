//
//  scrollbar.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-03-28.
//
//

#include "scrollbar.h"

cScrollbar::cScrollbar(cDialog& parent) : cControl(CTRL_SCROLL, parent) {}

cScrollbar::cScrollbar(sf::RenderWindow& parent) : cControl(CTRL_SCROLL, parent) {}

bool cScrollbar::isClickable(){
	return true;
}

void cScrollbar::setPosition(long newPos) {
	if(newPos > max) pos = max;
	else pos = newPos;
}

void cScrollbar::setMaximum(long newMax) {
	max = newMax;
}

long cScrollbar::getPosition() {
	return pos;
}

long cScrollbar::getMaximum() {
	return max;
}

void cScrollbar::attachClickHandler(click_callback_t f) throw(xHandlerNotSupported) {
	onClick = f;
}

void cScrollbar::attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported) {
	throw xHandlerNotSupported(true);
}

bool cScrollbar::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, location where) {
	// TODO: Implement detection of scrolling stuff, maybe even dragging the thumb
	if(onClick != NULL) return onClick(me,id,mods);
	return false;
}

void cScrollbar::setFormat(eFormat prop, short val) throw(xUnsupportedProp) {
	throw xUnsupportedProp(prop);
}

short cScrollbar::getFormat(eFormat prop) throw(xUnsupportedProp) {
	throw xUnsupportedProp(prop);
}

void cScrollbar::setColour(sf::Color clr) throw(xUnsupportedProp) {
	// TODO: Colour is unsupported
}

sf::Color cScrollbar::getColour() throw(xUnsupportedProp) {
	// TODO: Colour is unsupported
}

void cScrollbar::draw() {
	// TODO: Draw the scrollbar
}
