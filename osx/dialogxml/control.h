/*
 *  control.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef CONTROL_H
#define CONTROL_H

#include <string>
#include <exception>
#include <boost/function.hpp>

//struct cPict {
//	short pict;
//	short type;
//};
enum eKeyMod {
	mod_none = 0,
	mod_alt = 1, mod_shift = 2, mod_ctrl = 4,
	mod_altshift = mod_alt + mod_shift,
	mod_altctrl = mod_alt + mod_ctrl,
	mod_shiftctrl = mod_shift + mod_ctrl,
	mod_all = mod_alt + mod_shift + mod_ctrl,
};

enum eSpecKey {
	key_left, key_right, key_up, key_down,
	key_esc, key_enter, key_tab, key_help, // key_help should bind to the help key on Mac and the F1 key on Windows
	key_bsp, key_del, key_home, key_end, key_pgup, key_pgdn, // TODO: Implement these
	// On Mac, command-left should trigger key_home; command-right should trigger key_end;
	// command-up should trigger key_pgup; and command-down should trigger key_pgdn.
	// This is in addition to the home, end, pgup, pgdn keys triggering these.
};

struct cKey {
	bool spec;
	union {
		unsigned char c;
		eSpecKey k;
	};
	eKeyMod mod;
};

bool operator== (cKey& a, cKey& b);

enum eFormat {
	TXT_COLOR,
	TXT_FRAME,
	TXT_FONT,
	TXT_SIZE,
	TXT_WRAP,
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
};

enum eTextFont {DUNGEON, GENEVA, SILOM, MAIDENWORD};

class cDialog;
//typedef bool (*click_callback_t)(cDialog&/*me*/,std::string/*id*/, eKeyMod/*mods*/);
//typedef bool (*focus_callback_t)(cDialog&/*me*/,std::string/*id*/,bool/*losing*/); // losing is true if losing focus, false if gaining focus.
typedef boost::function<bool(cDialog&,std::string,eKeyMod)> click_callback_t;
typedef boost::function<bool(cDialog&,std::string,bool)> focus_callback_t;

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
	virtual void attachClickHandler(click_callback_t f) throw(xHandlerNotSupported) = 0;
	virtual void attachFocusHandler(focus_callback_t f) throw(xHandlerNotSupported) = 0;
	virtual bool triggerClickHandler(cDialog& me, std::string id, eKeyMod mods, Point where);
	virtual bool triggerFocusHandler(cDialog& me, std::string id, bool losingFocus);
	//virtual void setPict(short pict, short type) = 0;
	virtual void show(); // cd_activate_item true
	virtual void hide(); // cd_activate_item false
	bool isVisible(); // cd_get_active
	eControlType getType();
	virtual void setText(std::string l);
	virtual std::string getText();
	long getTextAsNum();
	void setTextToNum(long what);
	virtual void setFormat(eFormat prop, short val) throw(xUnsupportedProp) = 0;
	virtual short getFormat(eFormat prop) throw(xUnsupportedProp) = 0;
	virtual bool isClickable() = 0;
	bool handleClick();
	cControl(cDialog* p,eControlType t);
	virtual ~cControl();
protected:
	cDialog* parent;
	std::string lbl;
	bool visible, depressed; // depressed is only applicable for clickable controls
	Rect frame;
	cKey key;
	friend class cDialog;
	friend class cLedGroup;
	//friend class cStack;
	virtual void draw() = 0;
	static bool foundSilom();
	static short font_nums[4];
	void drawFrame(short amt, short med_or_lt);
private:
	eControlType type;
	static bool found_silom;
};

eKeyMod operator +  (eKeyMod lhs, eKeyMod rhs);
eKeyMod operator -  (eKeyMod lhs, eKeyMod rhs);
eKeyMod&operator += (eKeyMod&lhs, eKeyMod rhs);
eKeyMod&operator -= (eKeyMod&lhs, eKeyMod rhs);
#endif
