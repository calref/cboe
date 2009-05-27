/*
 *  message.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

class cTextMsg : public cControl {
public:
	void attachClickHandler(click_callback_t f) throw();
	void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported);
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, Point where);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	explicit cTextMsg(cDialog* parent);
	bool isClickable();
	virtual ~cTextMsg();
protected:
	void draw();
private:
	friend class cDialog;
	bool drawFramed, clickable;
	short textSize;
	eTextFont textFont;
	RGBColor color;
	std::string fromList;
	click_callback_t onClick;
};
#endif
