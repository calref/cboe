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
#include "control.h"

enum eFldType {
	FLD_NUM,
	FLD_TEXT,
};

class cTextField : public cControl {
public:
	void attachClickHandler(click_callback_t f) throw(xHandlerNotSupported);
	void attachFocusHandler(focus_callback_t f) throw();
	bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	bool handleClick(location where);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	eFldType getInputType();
	void setInputType(eFldType newType);
	sf::Color getColour() throw(xUnsupportedProp);
	explicit cTextField(cDialog* parent);
	bool isClickable();
	virtual ~cTextField();
	void draw();
	bool hasFocus();
	void handleInput(cKey key);
	cTextField& operator=(cTextField& other) = delete;
	cTextField(cTextField& other) = delete;
	long tabOrder = 0;
private:
	bool isNumericField;
	focus_callback_t onFocus;
	bool haveFocus;
	int insertionPoint;
	int selectionPoint;
	sf::Color color;
	bool ip_visible;
	sf::Clock ip_timer;
};
#endif
