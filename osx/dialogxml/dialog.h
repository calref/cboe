/*
 *  dialog.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <SFML/Graphics.hpp>

#include <string>
#include <map>
#include <vector>
#include <exception>

#include "ticpp.h"
#include "dialog.keys.h"
#include "location.h"
#include <boost/any.hpp>

class cControl;
class cTextField;

enum eLabelPos {
	LABEL_LEFT, LABEL_ABOVE, LABEL_RIGHT, LABEL_BELOW,
};

class cDialog {
	typedef std::map<std::string,cControl*>::iterator ctrlIter;
	std::map<std::string,cControl*> controls;
	short bg;
	sf::Color defTextClr;
	template<class T> std::pair<std::string,T*> parse(ticpp::Element& who);
	sf::Color parseColor(std::string what);
	cKey parseKey(std::string what);
	sf::RenderWindow win;
	std::string currentFocus;
	cDialog* parent;
	void loadFromFile(std::string path);
	template<typename Iter> void handleTabOrder(std::string& itemHit, Iter begin, Iter end);
	std::vector<std::pair<std::string,cTextField*>> tabOrder;
public:
	static void init();
	static const short BG_LIGHT, BG_DARK;
	static short defaultBackground;
	explicit cDialog(cDialog* p = NULL); // dialog with no items
	explicit cDialog(std::string path, cDialog* p = NULL); // cd_create_dialog
	~cDialog(); // cd_kill_dialog
	bool add(cControl* what, RECT ctrl_frame, std::string key); // returns false if the key is used, true if the control was added
	bool remove(std::string key); // returns true if the key existed and was removed, false if the key did not exist
	bool addLabelFor(std::string key, std::string label, eLabelPos where, short offset, bool bold); // returns true if the label was added
	void run(); // cd_run_dialog
	template<typename type> type getResult(){
		return boost::any_cast<type>(result);
	}
	template<typename type> void setResult(const type& val){
		result = val;
	}
	void setBg(short n);
	void setDefTextClr(sf::Color clr);
	void setDefBtn(std::string defBtn);
	sf::Color getDefTextClr();
	bool setFocus(cTextField* newFocus, bool force = false); // Setting force = true skips focus handlers
	bool toast(bool triggerFocus);
	cControl& getControl(std::string id);
	cControl& operator[](std::string id);
	void recalcRect();
	// TODO: It seems like a bad thing for these two to not use the typedefs...
	void attachClickHandlers(std::function<bool(cDialog&,std::string,eKeyMod)> handler, std::vector<std::string> controls);
	void attachFocusHandlers(std::function<bool(cDialog&,std::string,bool)> handler, std::vector<std::string> controls);
	RECT getBounds() {return winRect;}
	cDialog& operator=(cDialog& other) = delete;
	cDialog(cDialog& other) = delete;
private:
	void draw();
	std::string process_keystroke(cKey keyHit);
	std::string process_click(location where);
	bool dialogNotToast;
	RECT winRect;
	std::string defaultButton;
	boost::any result;
	std::string fname;
	friend class cControl;
	friend class cButton;
	friend class cLed;
	friend class cLedGroup;
	friend class cPict;
	friend class cTextField;
	friend class cTextMsg;
	friend class cScrollbar;
	friend class _init;
};

class xBadNode : std::exception {
	std::string type, dlg;
	int row, col;
	const char* msg;
public:
	xBadNode(std::string t, int r, int c, std::string dlg) throw();
	~xBadNode() throw();
	const char* what() throw();
};

class xBadAttr : std::exception {
	std::string type, name, dlg;
	int row, col;
	const char* msg;
public:
	xBadAttr(std::string t,std::string n, int r, int c, std::string dlg) throw();
	~xBadAttr() throw();
	const char* what() throw();
};

class xMissingAttr : std::exception {
	std::string type, name, dlg;
	int row, col;
	const char* msg;
public:
	xMissingAttr(std::string t,std::string n, int r, int c, std::string dlg) throw();
	~xMissingAttr() throw();
	const char* what() throw();
};

class xBadVal : std::exception {
	std::string type, name, val, dlg;
	int row, col;
	const char* msg;
public:
	static constexpr const char*const CONTENT = "<content>";
	xBadVal(std::string t,std::string n,std::string v, int r, int c, std::string dlg) throw();
	~xBadVal() throw();
	const char* what() throw();
};

#endif
