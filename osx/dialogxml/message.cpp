/*
 *  message.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "message.h"
#include "graphtool.h"
#include "mathutil.h"
#include "dialog.h"

extern sf::Texture bg_gworld;

void cTextMsg::attachClickHandler(click_callback_t f) throw(){
	onClick = f;
	clickable = onClick != NULL;
}

void cTextMsg::attachFocusHandler(focus_callback_t) throw(xHandlerNotSupported){
	throw xHandlerNotSupported(true);
}

bool cTextMsg::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods){
	if(onClick != NULL) return onClick(me,id,mods);
	return false;
}

void cTextMsg::setColour(sf::Color clr) throw(xUnsupportedProp) {
	color = clr;
}

void cTextMsg::setFormat(eFormat prop, short val) throw(xUnsupportedProp){
	switch(prop){
		case TXT_FRAME:
			drawFramed = val;
			break;
		case TXT_FRAMESTYLE:
			frameStyle = val;
			break;
		case TXT_SIZE:
			textSize = val;
			break;
		case TXT_FONT:
			if(val == FONT_DUNGEON) textFont = FONT_DUNGEON;
			else if(val == FONT_PLAIN) textFont = FONT_PLAIN;
			else if(val == FONT_MAIDWORD) textFont = FONT_MAIDWORD;
			else textFont = FONT_BOLD; // Defaults to bold if an invalid value is provided.
			break;
		case TXT_WRAP:
			throw xUnsupportedProp(prop);
	}
}

sf::Color cTextMsg::getColour() throw(xUnsupportedProp) {
	return color;
}

short cTextMsg::getFormat(eFormat prop) throw(xUnsupportedProp){
	switch(prop){
		case TXT_FRAME:
			return drawFramed;
		case TXT_FRAMESTYLE:
			return frameStyle;
		case TXT_SIZE:
			return textSize;
		case TXT_FONT:
			return textFont;
		case TXT_WRAP:
			throw xUnsupportedProp(prop);
	}
	return 0;
}

cTextMsg::cTextMsg(cDialog& parent) :
	cControl(CTRL_TEXT,parent),
	drawFramed(false),
	textFont(FONT_BOLD),
	textSize(10),
	color(parent.getDefTextClr()),
	clickable(false),
	fromList("none") {}

cTextMsg::cTextMsg(sf::RenderWindow& parent) :
	cControl(CTRL_TEXT,parent),
	drawFramed(false),
	textFont(FONT_BOLD),
	textSize(10),
	color(cDialog::defaultBackground == cDialog::BG_DARK ? sf::Color::White : sf::Color::Black),
	clickable(false),
	fromList("none") {}

bool cTextMsg::isClickable(){
	return clickable;
}

void cTextMsg::draw(){
	RECT to_rect = frame;
	
	inWindow->setActive();
	
	if(visible){
		TextStyle style;
		style.font = textFont;
		style.pointSize = textSize;
		if(drawFramed) drawFrame(2,frameStyle);
		sf::Color draw_color = color;
		if(clickable && depressed){
			draw_color.r = 256 - draw_color.r;
			draw_color.g = 256 - draw_color.g;
			draw_color.b = 256 - draw_color.b;
		}
		style.colour = draw_color;
		if (to_rect.bottom - to_rect.top < 20) { // essentially, it's a single line
			style.lineHeight = 12;
			to_rect.left += 3;
			win_draw_string(*inWindow,to_rect,lbl,eTextMode::LEFT_BOTTOM,style);
		}else {
			style.lineHeight = textSize + 2;
			to_rect.inset(4,4);
			win_draw_string(*inWindow,to_rect,lbl,eTextMode::WRAP,style);
		}
	}
}

cTextMsg::~cTextMsg() {}
