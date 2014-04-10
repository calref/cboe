//
//  scrollbar.h
//  BoE
//
//  Created by Celtic Minstrel on 14-03-26.
//
//

#ifndef BoE_scrollbar_h
#define BoE_scrollbar_h

#include "control.h"

class cScrollbar : public cControl {
	int pos, max;
	click_callback_t onClick;
public:
	explicit cScrollbar(sf::RenderWindow& parent);
	explicit cScrollbar(cDialog& parent);
	void attachClickHandler(click_callback_t f) throw(xHandlerNotSupported);
	void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported);
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, location where);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	sf::Color getColour() throw(xUnsupportedProp);
	bool isClickable();
	long getPosition();
	long getMaximum();
	void setPosition(long to);
	void setMaximum(long to);
	void draw();
};

#endif
