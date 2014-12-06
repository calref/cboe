/*
 *  dlogutil.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef DIALOG_UTIL_H
#define DIALOG_UTIL_H

#include <string>
#include <vector>
#include <functional>
#include "graphtool.h" // for pic_num_t
#include "soundtool.h" // for snd_num_t
#include "pict.h"
#include "dialog.h"
#include "button.h"
#include <boost/optional.hpp>

//typedef void (*record_callback_t)(std::string,std::string);
typedef std::function<void(cDialog&)> record_callback_t;

class cStrDlog {
	static std::string getFileName(short n_strs, ePicType type, bool hasTitle);
	cDialog dlg;
	short sound = -1;
	record_callback_t rec_f;
	bool hasRecord;
	const ePicType type;
	bool onRecord(std::string id);
	bool onDismiss();
public:
	cStrDlog(std::string str1,std::string str2,std::string title,pic_num_t pic,ePicType t,cDialog* parent = NULL);
	cStrDlog& setSound(snd_num_t num);
	cStrDlog& setRecordHandler(record_callback_t rec);
	cDialog* operator->();
	void show();
};

class cChoiceDlog {
	cDialog dlg;
protected:
	bool onClick(cDialog& me, std::string id);
	explicit cChoiceDlog(cDialog* p = NULL);
public:
	explicit cChoiceDlog(std::string file, cDialog* p = NULL);
	cChoiceDlog(std::string file, std::vector<std::string> buttons, cDialog* p = NULL);
	cDialog* operator->();
	std::string show();
};

struct bbtt { // stands for "basic button type template"
	eBtnType type;
	std::string label;
	cKey defaultKey;
};

typedef boost::optional<bbtt> cBasicButtonType;

namespace {cBasicButtonType null_btn = boost::none;}
#ifndef BTNS_DEFINED
extern bbtt basic_buttons[];
extern size_t available_btns[53];
#endif

class cThreeChoice : public cChoiceDlog {
	//static std::string getFileName(size_t n_strs);
	cBasicButtonType btns[3];
	unsigned short buttons_right, buttons_top;
	void init_strings(std::vector<std::string>& strings, unsigned short left);
	void init_buttons(cBasicButtonType btn1, cBasicButtonType btn2, cBasicButtonType btn3);
	void init_pict(pic_num_t pic);
	const ePicType type;
public:
	cThreeChoice(std::vector<std::string>& strings, cBasicButtonType button, pic_num_t pic, ePicType t, cDialog* parent = NULL);
	cThreeChoice(std::vector<std::string>& strings, std::array<cBasicButtonType, 3>& buttons, pic_num_t pic, ePicType t, cDialog* parent = NULL);
	cThreeChoice(std::vector<std::string>& strings, std::array<short, 3>& buttons, pic_num_t pic, ePicType t, cDialog* parent = NULL);
	std::string show();
};

class cStringChoice {
	static const size_t per_page;
	cDialog dlg;
	bool onLeft();
	bool onRight();
	bool onCancel(cDialog& me);
	bool onOkay(cDialog& me);
	bool onSelect(bool losing);
	void attachHandlers();
	void fillPage();
	std::vector<std::string> strings;
	size_t page, cur;
	cLedGroup* leds;
public:
	explicit cStringChoice(std::vector<std::string>& strs, std::string title, cDialog* parent = NULL);
	cStringChoice(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end, std::string title, cDialog* parent = NULL);
	cDialog* operator->();
	size_t show(size_t selectedIndex);
};

class cPictChoice {
	static const size_t per_page;
	bool didAccept;
	cDialog dlg;
	void attachHandlers();
	bool onLeft();
	bool onRight();
	bool onCancel();
	bool onOkay();
	bool onSelect(bool losing);
	void fillPage();
	std::vector<std::pair<pic_num_t,ePicType>> picts;
	size_t page, cur;
	cLedGroup* leds;
public:
	cPictChoice(std::vector<pic_num_t>& pics, ePicType t, cDialog* parent = NULL);
	cPictChoice(std::vector<std::pair<pic_num_t,ePicType>>& pics, cDialog* parent = NULL);
	cPictChoice(std::vector<pic_num_t>::iterator begin, std::vector<pic_num_t>::iterator end, ePicType t, cDialog* parent = NULL);
	cPictChoice(pic_num_t first, pic_num_t last, ePicType t, cDialog* parent = NULL);
	cDialog* operator->();
	bool show(size_t cur_sel);
	pic_num_t getPicChosen();
	ePicType getPicChosenType();
	// returns the _number_ of the chosen picture, _not_ the index; there's no way to distinguish between duplicates
};
#endif

void giveError(std::string str1, std::string str2 = "", cDialog* parent = NULL);
void oopsError(short error,short code = 0, short mode = 0);
