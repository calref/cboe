/*
 *  message.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <SFML/Graphics.hpp>

#include <string>
#include "control.h"

class cTextMsg : public cControl {
public:
	void attachClickHandler(click_callback_t f) throw();
	void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported);
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, location where);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	sf::Color getColour() throw(xUnsupportedProp);
	explicit cTextMsg(cDialog* parent);
	bool isClickable();
	virtual ~cTextMsg();
	void draw();
private:
	bool drawFramed, clickable;
	short textSize;
	eTextFont textFont;
	sf::Color color;
	std::string fromList;
	click_callback_t onClick;
};
#endif
