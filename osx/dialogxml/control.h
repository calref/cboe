/*
 *  control.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <SFML/Graphics.hpp>

#include <string>
#include <exception>
#include <functional>
#include "dialog.h"

#include "location.h"

//struct cPict {
//	short pict;
//	short type;
//};

enum eFormat {
	TXT_FRAME,
	TXT_FONT,
	TXT_SIZE,
	TXT_WRAP,
	TXT_FRAMESTYLE,
};

enum eControlType {
	CTRL_UNKNOWN,
	CTRL_BTN,	// An ordinary push button
	CTRL_LED,	// An LED (checkbox/radiobutton)
	CTRL_PICT,	// A picture
	CTRL_FIELD,	// An edit text field
	CTRL_TEXT,	// A static text object
	CTRL_GROUP,	// A LED radiobutton-like group
	CTRL_STACK,	// A group of controls that display pages (not implemented yet)
	CTRL_SCROLL,// A scrollbar (not implemented yet)
};

enum eTextFont {DUNGEON, GENEVA, SILOM, MAIDENWORD};

//typedef bool (*click_callback_t)(cDialog&/*me*/,std::string/*id*/, eKeyMod/*mods*/);
//typedef bool (*focus_callback_t)(cDialog&/*me*/,std::string/*id*/,bool/*losing*/); // losing is true if losing focus, false if gaining focus.
typedef std::function<bool(cDialog&,std::string,eKeyMod)> click_callback_t;
typedef std::function<bool(cDialog&,std::string,bool)> focus_callback_t;

class xHandlerNotSupported : std::exception {
	static const char* focusMsg;
	static const char* clickMsg;
	bool isFocus;
public:
	xHandlerNotSupported(bool isFocus);
	const char* what();
};

class xUnsupportedProp : std::exception {
	eFormat whichProp;
	char* msg;
public:
	xUnsupportedProp(eFormat prop) throw();
	~xUnsupportedProp() throw();
	const char* what() throw();
};

class cControl {
public:
	static void init();
	void attachKey(cKey key);
	void detachKey();
	void setTextToKey();
	bool hasKey();
	cKey getAttachedKey();
	virtual void attachClickHandler(click_callback_t f) throw(xHandlerNotSupported) = 0;
	virtual void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported) = 0;
	virtual bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, location where);
	virtual bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	//virtual void setPict(short pict, short type) = 0;
	virtual void show(); // cd_activate_item true
	virtual void hide(); // cd_activate_item false
	bool isVisible(); // cd_get_active
	void setActive(bool active); // "active" here means "selected", so if it's a button, draw it pressed
	eControlType getType();
	virtual void setText(std::string l);
	virtual std::string getText();
	RECT getBounds();
	void setBounds(RECT newBounds);
	void relocate(location to);
	long long getTextAsNum();
	void setTextToNum(long long what);
	virtual void setFormat(eFormat prop, short val) throw(xUnsupportedProp) = 0;
	virtual short getFormat(eFormat prop) throw(xUnsupportedProp) = 0;
	virtual void setColour(sf::Color clr) throw(xUnsupportedProp) = 0;
	virtual sf::Color getColour() throw(xUnsupportedProp) = 0;
	virtual bool isClickable() = 0;
	virtual bool handleClick();
	cControl(eControlType t, sf::RenderWindow& p);
	cControl(eControlType t, cDialog& p);
	virtual ~cControl();
	virtual void draw() = 0;
protected:
	cDialog* parent;
	sf::RenderWindow* inWindow;
	std::string lbl;
	bool visible, depressed = false; // depressed is only applicable for clickable controls
	RECT frame;
	int frameStyle;
	cKey key;
	static bool foundSilom();
	static std::string font_nums[4];
	void drawFrame(short amt, bool med_or_lt);
private:
	eControlType type;
	static bool found_silom;
};

#endif
