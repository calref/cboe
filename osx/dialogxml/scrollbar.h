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
	int pos, max, pgsz;
	enum {
		PART_UP,
		PART_PGUP,
		PART_THUMB,
		PART_PGDN,
		PART_DOWN,
	} pressedPart;
	click_callback_t onClick;
	static sf::Texture scroll_gw;
public:
	static void init();
	explicit cScrollbar(sf::RenderWindow& parent);
	explicit cScrollbar(cDialog& parent);
	void attachClickHandler(click_callback_t f) throw(xHandlerNotSupported);
	void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported);
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, location where);
	bool handleClick(location where);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	void setColour(sf::Color clr) throw(xUnsupportedProp);
	sf::Color getColour() throw(xUnsupportedProp);
	bool isClickable();
	long getPosition();
	long getMaximum();
	long getPageSize();
	void setPosition(long to);
	void setMaximum(long to);
	void setPageSize(long to);
	void draw();
};

#endif
