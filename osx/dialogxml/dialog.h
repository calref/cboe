/*
 *  dialog.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <string>
#include <map>
#include <exception>

#include "ticpp.h"

#include "control.h"
#include "button.h"
#include "field.h"
#include "pict.h"
#include "message.h"

class cDialog {
	typedef std::map<std::string,cControl*>::iterator ctrlIter;
	std::map<std::string,cControl*> controls;
	short bg;
	RGBColor defTextClr;
	template<class T> std::pair<std::string,T*> parse(ticpp::Element& who);
	RGBColor parseColor(std::string what);
	cKey parseKey(std::string what);
	WindowRef win;
	cTextField* currentFocus;
	class _init {
		_init();
		~_init();
	};
	static _init init;
public:
	static const short BG_LIGHT, BG_DARK;
	cDialog(std::string path); // cd_create_dialog
	cDialog(std::string path,cDialog parent); // cd_create_dialog_parent_num
	~cDialog(); // cd_kill_dialog
	bool add(cControl* what, std::string key); // returns false if the key is used, true if the control was added
	bool remove(std::string key); // returns true if the key existed and was removed, false if the key did not exist
	void run(); // cd_run_dialog
	//template<class type> type getResult(); // This seems like an unimplementable function
	void setBg(short n);
	void setDefTextClr(RGBColor clr);
	void toast();
	cControl& operator[](std::string id);
private:
	void draw();
	std::string process_keystroke(cKey keyHit);
	std::string process_click(Point where, eKeyMod mods);
	bool dialogNotToast;
	Rect winRect;
	friend class cControl;
	friend class cButton;
	friend class cLed;
	friend class cLedGroup;
	friend class cPict;
	friend class cTextField;
	friend class cTextMsg;
	friend class _init;
};

class xBadNode : std::exception {
	std::string type;
	const char* msg;
public:
	xBadNode(std::string t) throw();
	~xBadNode() throw();
	const char* what() throw();
};

class xBadAttr : std::exception {
	std::string type, name;
	const char* msg;
public:
	xBadAttr(std::string t,std::string n) throw();
	~xBadAttr() throw();
	const char* what() throw();
};

class xBadVal : std::exception {
	std::string type, name, val;
	const char* msg;
public:
	xBadVal(std::string t,std::string n,std::string v) throw();
	~xBadVal() throw();
	const char* what() throw();
};
#endif
