/*
 *  field.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include <Carbon/Carbon.h>
#include <sstream>
#include "dialog.h"

void cTextField::attachClickHandler(click_callback_t f __attribute__((unused))) throw(xHandlerNotSupported){
	throw xHandlerNotSupported(false);
}

void cTextField::attachFocusHandler(focus_callback_t f __attribute__((unused))) throw(){
	onFocus = f;
}

bool cTextField::triggerFocusHandler(cDialog& me, std::string id, bool losingFocus){
	// TODO: If isNumericField, verify that the contents are in fact a number.
	if(onFocus != NULL) onFocus(me,id,losingFocus);
	return true;
}

void cTextField::setFormat(eFormat prop, short val __attribute__((unused))) throw(xUnsupportedProp){
	throw xUnsupportedProp(prop);
}

short cTextField::getFormat(eFormat prop) throw(xUnsupportedProp){
	throw xUnsupportedProp(prop);
}

void cTextField::setText(std::string what){
	char message[1024];
	strcpy(message,what.c_str());
	c2pstr(message);
	OSErr err = SetControlData(theField,kControlEditTextPart,kControlEditTextTextTag,what.size() + 1,message);
	if(isVisible()) draw();
}

std::string cTextField::getText(){
	unsigned char message[1024];
	OSErr err = GetControlData(theField,kControlEditTextPart,kControlEditTextTextTag,1024,message,NULL);
	p2cstr(message);
	return std::string((char*)message);
}

bool cTextField::isClickable(){
	return false;
}

cTextField::cTextField(cDialog* parent) : cControl(parent,CTRL_FIELD) {
	OSStatus err;
	err = CreateEditTextControl(parent->win,&frame,NULL,false,true/*useInlineInput*/,NULL,&theField);
}

cTextField::~cTextField(){
	DisposeControl(theField);
}

void cTextField::show(){
	ShowControl(theField);
	cControl::show();
}

void cTextField::hide(){
	HideControl(theField);
	cControl::hide();
}

void cTextField::draw(){
	Draw1Control(theField);
}
