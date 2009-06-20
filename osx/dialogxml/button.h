/*
 *  button.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include <map>
#include <vector>

enum eBtnType {	// w x h
	BTN_SM = 0,	// 23x23 (PICT id 2000 / 2001)
	BTN_REG,	// 63x23 (PICT id 2002 / 2003)
	BTN_LG,		// 102x23 (PICT id 2004 / 2005)
	BTN_HELP,	// 16x13 (PICT id 2006 / 2007) white bubble w/ ? mark
	BTN_LEFT,	// 63x23 (PICT id 2008 / 2009) with left arrow
	BTN_RIGHT,	// 63x23 (PICT id 2010 / 2011) with right arrow
	BTN_UP,		// 63x23 (PICT id 2012 / 2013) with up arrow
	BTN_DOWN,	// 63x23 (PICT id 2014 / 2015) with down arrow
	BTN_TINY,	// 14x10 (PICT id 2021)
	BTN_DONE,	// 63x23 (PICT id 2022 / 2023) says "Done"
	BTN_TALL,	// 63x40 (PICT id 2024 / 2025)
	BTN_TRAIT,	// 63x40 (PICT id 2026 / 2027) says "Race Good/Bad Traits"
	BTN_PUSH,	// 30x30 (PICT id 2028 / 2029) red round button
	BTN_LED,	// 14x10 (PICT id 2018 / 2019 / 2020)
};

enum eLedState {led_green = 0, led_red, led_off};

class cButton : public cControl {
public:
	static void init();
	static void finalize();
	void attachClickHandler(click_callback_t f) throw();
	void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported);
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, Point where);
	//virtual void setPict(short pict, short type) = 0;
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	explicit cButton(cDialog* parent);
	bool isClickable();
	void setType(eBtnType newType);
	eBtnType getType();
	virtual ~cButton();
protected:
	//friend class cDialog;
	void draw();
	eBtnType type;
	click_callback_t onClick;
	cButton(cDialog* parent,eControlType t);
private:
	friend class cDialog;
	bool wrapLabel;
	bool labelWithKey;
	bool pressed;
	std::string fromList;
	static Rect btnRects[13][2];
protected:
	static size_t btnGW[14];
	static GWorldPtr buttons[7];
};

class cLed : public cButton {
public:
	static void init();
	void attachClickHandler(click_callback_t f) throw();
	void attachFocusHandler(focus_callback_t f) throw();
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, Point where);
	bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	explicit cLed(cDialog* parent);
	virtual ~cLed();
	void setState(eLedState to);
	eLedState getState();
protected:
	void draw();
private:
	friend class cDialog;
	friend class cLedGroup;
	eLedState state;
	eTextFont textFont;
	RGBColor color;
	short textSize;
	static Rect ledRects[3][2];
	focus_callback_t onFocus;
};

class cLedGroup : public cControl {
	std::vector<cLed> btns;
	click_callback_t onClick;
	focus_callback_t onFocus;
	std::map<std::string,cLed*> choices;
	std::string fromList;
	std::string curSelect, prevSelect;
public:
	void attachClickHandler(click_callback_t f) throw(); // activated whenever a click is received, even on the currently active LED
	void attachFocusHandler(focus_callback_t f) throw(); // activated only when the selection changes
	bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, Point where);
	bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	void disable(std::string id);
	void enable(std::string id);
	using cControl::show;
	using cControl::hide;
	void hide(std::string id);
	void show(std::string id);
	void setFormat(eFormat prop, short val) throw(xUnsupportedProp);
	short getFormat(eFormat prop) throw(xUnsupportedProp);
	explicit cLedGroup(cDialog* parent);
	bool isClickable();
	virtual ~cLedGroup();
	cLed& operator[](std::string id);
	void setSelected(std::string id);
	std::string getSelected();
	std::string getPrevSelection(); // The id of the element that was last selected before the selection changed to the current selection.
	void recalcRect();
	typedef std::map<std::string,cLed*>::iterator ledIter;
protected:
	void draw();
	friend class cDialog;
};
#endif
