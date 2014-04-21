/*
 *  field.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "field.h"
#include <sstream>
#include "dialog.h"
#include "graphtool.h"

void cTextField::attachClickHandler(click_callback_t f __attribute__((unused))) throw(xHandlerNotSupported){
	throw xHandlerNotSupported(false);
}

void cTextField::attachFocusHandler(focus_callback_t f __attribute__((unused))) throw(){
	onFocus = f;
}

bool cTextField::triggerFocusHandler(cDialog& me, std::string id, bool losingFocus){
	// TODO: If isNumericField, verify that the contents are in fact a number.
	if(onFocus != NULL) onFocus(me,id,losingFocus);
	haveFocus = !losingFocus;
	if(haveFocus && insertionPoint < 0)
		insertionPoint = getText().length();
	return true;
}

void cTextField::setFormat(eFormat prop, short val __attribute__((unused))) throw(xUnsupportedProp){
	throw xUnsupportedProp(prop);
}

short cTextField::getFormat(eFormat prop) throw(xUnsupportedProp){
	throw xUnsupportedProp(prop);
}

void cTextField::setColour(sf::Color clr) throw(xUnsupportedProp) {
	color = clr;
}

sf::Color cTextField::getColour() throw(xUnsupportedProp) {
	return color;
}

eFldType cTextField::getInputType() {
	if(isNumericField) return FLD_NUM;
	else return FLD_TEXT;
}

void cTextField::setInputType(eFldType type) {
	switch(type) {
		case FLD_NUM:
			isNumericField = true;
			break;
		case FLD_TEXT:
			isNumericField = false;
			break;
	}
}

bool cTextField::isClickable(){
	// TODO: Should be clickable in order to handle text selection
	return false;
}

bool cTextField::hasFocus() {
	return haveFocus;
}

cTextField::cTextField(cDialog* parent) :
		cControl(CTRL_FIELD,*parent),
		color(sf::Color::Black),
		insertionPoint(-1),
		selectionPoint(0),
		isNumericField(false) {}

cTextField::~cTextField(){}

void cTextField::draw(){
	static const sf::Color hiliteClr = {127,127,127}, ipClr = {92, 92, 92};
	inWindow->setActive();
	RECT outline = frame;
	outline.inset(-2,-2);
	fill_rect(*inWindow, outline, sf::Color::White);
	frame_rect(*inWindow, outline, sf::Color::Black);
	std::string contents = getText();
	RECT rect = frame;
	rect.inset(2,6);
	TextStyle style;
	style.font = FONT_PLAIN;
	style.pointSize = 12;
	style.colour = sf::Color::Black;
	style.lineHeight = 14;
	// TODO: Proper support for multiline fields
	int ip_offset, sel_offset;
	if(haveFocus) {
		std::string pre_ip = contents.substr(0, insertionPoint);
		// TODO: Update string_length to take a std::string
		ip_offset = string_length(pre_ip, style);
		if(insertionPoint != selectionPoint) {
			std::string pre_sel = contents.substr(0, selectionPoint);
			sel_offset = string_length(pre_sel, style);
			int sel_start = std::min(ip_offset, sel_offset) + 1;
			int sel_width = abs(ip_offset - sel_offset) + 3;
			RECT selectRect = frame;
			selectRect.left += sel_start;
			selectRect.right = selectRect.left + sel_width;
			fill_rect(*inWindow, selectRect, ipClr);
			// TODO: I forget whether this was supposed to be = or -=
			selectRect.right - 1;
			fill_rect(*inWindow, selectRect, hiliteClr);
		} else if(ip_timer.getElapsedTime().asMilliseconds() < 500) {
//			printf("Blink on (%d); ", ip_timer.getElapsedTime().asMilliseconds());
			RECT ipRect = frame;
			ipRect.left += ip_offset + 2;
			ipRect.right = ipRect.left + 1;
			fill_rect(*inWindow, ipRect, ipClr);
		} else if(ip_timer.getElapsedTime().asMilliseconds() > 1000) {
//			printf("Blink off (%d); ", ip_timer.getElapsedTime().asMilliseconds());
			ip_timer.restart();
		}
	}
	// TODO: Update win_draw_string to take a std::string
	win_draw_string(*inWindow, rect, contents, 0, style);
}

void cTextField::handleInput(cKey key) {
	bool select = mod_contains(key.mod, mod_shift);
	bool word = mod_contains(key.mod, mod_alt) || mod_contains(key.mod, mod_ctrl);
	bool haveSelection = insertionPoint != selectionPoint;
	int new_ip;
	std::string contents = getText();
	if(!key.spec) {
		if(haveSelection) {
			cKey deleteKey = key;
			deleteKey.spec = true;
			deleteKey.k = key_bsp;
			handleInput(deleteKey);
			contents = getText();
		}
		contents.insert(contents.begin() + insertionPoint, key.c);
		selectionPoint = ++insertionPoint;
	} else switch(key.k) {
			// TODO: Implement all the other special keys
		case key_left:
			if(haveSelection && !select) {
				selectionPoint = insertionPoint = std::min(selectionPoint,insertionPoint);
				break;
			}
			new_ip = select ? selectionPoint : insertionPoint;
			if(new_ip == 0) break;
			if(word) {
				new_ip--;
				while(new_ip > 0 && contents[new_ip - 1] != ' ')
					new_ip--;
			} else new_ip--;
			(select ? selectionPoint : insertionPoint) = new_ip;
			if(!select) selectionPoint = insertionPoint;
			break;
		case key_right:
			if(haveSelection && !select) {
				selectionPoint = insertionPoint = std::max(selectionPoint,insertionPoint);
				break;
			}
			new_ip = select ? selectionPoint : insertionPoint;
			if(new_ip == contents.length()) break;
			if(word) {
				new_ip++;
				while(new_ip < contents.length() && contents[new_ip + 1] != ' ')
					new_ip++;
			} else new_ip++;
			(select ? selectionPoint : insertionPoint) = new_ip;
			if(!select) selectionPoint = insertionPoint;
			break;
		case key_up:
		case key_down:
			break;
		case key_enter:
			key.spec = false;
			key.c = '\n';
			handleInput(key);
			break;
		case key_bsp:
			if(haveSelection) {
				auto begin = contents.begin() + std::min(selectionPoint, insertionPoint);
				auto end = contents.begin() + std::max(selectionPoint, insertionPoint);
				auto result = contents.erase(begin, end);
				selectionPoint = insertionPoint = result - contents.begin();
			} else if(word) {
				cKey selectKey = key;
				selectKey.k = key_left;
				handleInput(selectKey);
				if(selectionPoint != insertionPoint)
					handleInput(key);
			} else {
				if(insertionPoint == 0) break;
				contents.erase(insertionPoint - 1,1);
				selectionPoint = --insertionPoint;
			}
			break;
		case key_del:
			if(haveSelection) {
				auto begin = contents.begin() + std::min(selectionPoint, insertionPoint);
				auto end = contents.begin() + std::max(selectionPoint, insertionPoint);
				auto result = contents.erase(begin, end);
				selectionPoint = insertionPoint = result - contents.begin();
			} else if(word) {
				cKey selectKey = key;
				selectKey.k = key_left;
				handleInput(selectKey);
				if(selectionPoint != insertionPoint)
					handleInput(key);
			} else {
				if(insertionPoint == contents.length()) break;
				contents.erase(insertionPoint,1);
			}
			break;
		case key_end:
		case key_home:
		case key_pgup:
		case key_pgdn:
		case key_copy:
		case key_cut:
		case key_paste:
			break;
		case key_selectall:
			selectionPoint = 0;
			insertionPoint = contents.length();
			break;
		case key_esc:
		case key_tab:
		case key_help:
			break;
	}
	setText(contents);
}
