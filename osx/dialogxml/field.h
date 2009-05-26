/*
 *  field.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef FIELD_H
#define FIELD_H

#include <string>

class cTextField : public cControl {
public:
	void attachClickHandler(click_callback_t f) throw(xHandlerNotSupported);
	void attachFocusHandler(focus_callback_t f) throw();
	bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	std::string getText();
	void setText(std::string what);
	short getTextAsNum();
	void setTextToNum(short what);
	cTextField(cDialog* parent);
	bool isClickable();
	virtual ~cTextField();
	void show();
	void hide();
protected:
	void draw();
private:
	friend class cDialog;
	bool isNumericField;
	focus_callback_t onFocus;
	ControlRef theField;
};
#endif
