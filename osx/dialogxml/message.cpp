/*
 *  message.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include <Carbon/Carbon.h>
#include "graphtool.h"
#include "mathutil.h"
#include "dialog.h"

extern GWorldPtr bg_gworld;

void cTextMsg::attachClickHandler(click_callback_t f) throw(){
	onClick = f;
}

void cTextMsg::attachFocusHandler(focus_callback_t f __attribute__((unused))) throw(xHandlerNotSupported){
	throw xHandlerNotSupported(true);
}

bool cTextMsg::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, Point where){
	if(onClick != NULL) return onClick(me,id,mods);
	return false;
}

void cTextMsg::setFormat(eFormat prop, short val) throw(xUnsupportedProp){
	switch(prop){
		case TXT_COLOR:
			{ // TODO: Test this and see if it works as imagined
				RGBColor clr;
				unsigned char red, green, blue, bright;
				red = (val & 0xF000) >> 12;
				green = (val & 0x0F00) >> 8;
				blue = (val & 0x00F0) >> 4;
				bright = val & 0x000F;
				red *= bright;
				green *= bright;
				blue *= bright;
				clr.red = red * 257;
				clr.green = green * 257;
				clr.blue = blue * 257;
				color = clr;
			}
			break;
		case TXT_FRAME:
			drawFramed = val;
			break;
		case TXT_SIZE:
			textSize = val;
			break;
		case TXT_FONT:
			if(val == DUNGEON) textFont = DUNGEON;
			else if(val == GENEVA) textFont = GENEVA;
			else if(val == MAIDENWORD) textFont = MAIDENWORD;
			else textFont = SILOM; // Defaults to Silom if an invalid value is provided.
			break;
		case TXT_WRAP:
			throw xUnsupportedProp(prop);
	}
}

short cTextMsg::getFormat(eFormat prop) throw(xUnsupportedProp){
	switch(prop){
		case TXT_COLOR:
			{
				unsigned char red, green, blue, bright;
				short val;
				red = color.red / 257;
				green = color.green / 257;
				blue = color.green / 257;
				bright = gcd(red,gcd(green,blue));
				red /= bright;
				green /= bright;
				blue /= bright;
				val |= bright;
				val |= blue << 4;
				val |= green << 8;
				val |= red << 12;
				return val;
			}
		case TXT_FRAME:
			return drawFramed;
		case TXT_SIZE:
			return textSize;
		case TXT_FONT:
			return textFont;
		case TXT_WRAP:
			throw xUnsupportedProp(prop);
	}
	return 0;
}

cTextMsg::cTextMsg(cDialog* parent) : cControl(parent,CTRL_TEXT), drawFramed(true), textFont(SILOM), textSize(10) {}

bool cTextMsg::isClickable(){
	return clickable;
}

void cTextMsg::draw(){
	GrafPtr old_port;
	Rect to_rect = frame;
	
	GetPort(&old_port);
	SetPortWindowPort(parent->win);
	tileImage(frame,bg_gworld,bg[parent->bg]);
	
	if(visible){
		TextFont(font_nums[textFont]);
		if(textFont == SILOM && !foundSilom()) TextFace(bold);
		else TextFace(normal);
		TextSize(textSize);
		if(drawFramed) drawFrame(2,0);
		RGBColor draw_color = color;
		if(clickable && depressed){
			color.red = 65535 - color.red;
			color.green = 65535 - color.green;
			color.blue = 65535 - color.blue;
		}
		RGBForeColor(&draw_color);
		if (to_rect.bottom - to_rect.top < 20) { // essentially, it's a single line
			to_rect.left += 3;
			char_win_draw_string(parent->win,to_rect,lbl.c_str(),3,12);
		}else {
			InsetRect(&to_rect,4,4);
			char_win_draw_string(parent->win,to_rect,lbl.c_str(),0,textSize + 2);
		}
	}
	
	SetPort(old_port);
}

cTextMsg::~cTextMsg() {}
