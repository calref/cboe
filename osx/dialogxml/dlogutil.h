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
#include <boost/function.hpp>
#include "graphtool.h" // for pic_num_t
#include "soundtool.h" // for snd_num_t

//typedef void (*record_callback_t)(std::string,std::string);
typedef boost::function<void(std::string,std::string)> record_callback_t;

class cStrDlog {
	static std::string getFileName(short n_strs, ePicType type, bool hasTitle);
	cDialog dlg;
	std::string str1, str2;
	short sound;
	record_callback_t rec_f;
	bool hasRecord;
	const ePicType type;
	bool onRecord(cDialog& me, std::string id);
	bool onDismiss(cDialog& me, std::string id);
public:
	cStrDlog(std::string str1,std::string str2,std::string title,pic_num_t pic,ePicType t,cDialog* parent);
	cStrDlog& setSound(snd_num_t num);
	cStrDlog& setRecordHandler(record_callback_t rec);
	void show();
};

class cChoiceDlog {
	cDialog dlg;
protected:
	bool onClick(cDialog& me, std::string id);
public:
	cChoiceDlog(std::string file, std::vector<std::string> buttons, cDialog* p = NULL);
	explicit cChoiceDlog(std::vector<std::string> buttons, cDialog* p = NULL);
	cDialog* operator->();
	std::string show();
};

template<bool b> struct bbtt { // stands for "basic button type template"
	eBtnType type;
	std::string label;
	cKey defaultKey;
	operator bool();
	bbtt<b>& operator=(bbtt<!b>&);
};

template<> struct bbtt<false> {
	operator bool();
	bbtt<false>& operator=(bbtt<true>&);
};
typedef bbtt<true> cBasicButtonType;

namespace {const bbtt<false> null_btn = {};}
#ifndef BTNS_DEFINED
extern cBasicButtonType basic_buttons[];
extern size_t available_btns[];
#endif

class cThreeChoice : public cChoiceDlog {
	//static std::string getFileName(size_t n_strs);
	bool btn_used[3];
	cBasicButtonType btns[3];
	unsigned short buttons_right, buttons_top;
	void init_strings(std::vector<std::string>& strings, unsigned short left);
	template<bool a, bool b, bool c> void init_buttons(bbtt<a> btn1, bbtt<b> btn2, bbtt<c> btn3);
	void init_pict(pic_num_t pic);
	const ePicType type;
public:
	template<bool a, bool b, bool c> cThreeChoice
			(std::vector<std::string> strings, bbtt<a> btn1, bbtt<b> btn2, bbtt<c> btn3, pic_num_t pic, ePicType t, cDialog* parent = NULL)
			: cChoiceDlog(std::vector<std::string>(), parent), type(t){
		if(type == PIC_CUSTOM_DLOG_LG || type == PIC_DLOG_LG || type == PIC_SCEN_LG)
			init_strings(strings,86);
		else
			init_strings(strings,50);
		init_buttons(btn1, btn2, btn3);
		init_pict(pic);
		operator->()->recalcRect();
	}
	cThreeChoice(std::vector<std::string> strings, short btn1, short btn2, short btn3, pic_num_t pic, ePicType t, cDialog* parent = NULL);
	std::string show();
};

class cStringChoice {
	static const size_t per_page;
	cDialog dlg;
	bool onLeft(cDialog& me, std::string id);
	bool onRight(cDialog& me, std::string id);
	bool onCancel(cDialog& me, std::string id);
	bool onOkay(cDialog& me, std::string id);
	void fillPage();
	std::vector<std::string> strings;
	size_t page, cur;
public:
	explicit cStringChoice(std::vector<std::string>& strs, cDialog* parent = NULL);
	cStringChoice(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end, cDialog* parent = NULL);
	size_t show(std::string select); // returns the _index_ of the chosen string, relative to begin
	// returns strs.size() if the user cancels
};

class cPictChoice {
	static const size_t per_page;
	cDialog dlg;
	bool onLeft(cDialog& me, std::string id);
	bool onRight(cDialog& me, std::string id);
	bool onCancel(cDialog& me, std::string id);
	bool onOkay(cDialog& me, std::string id);
	void fillPage();
	std::vector<pic_num_t> picts;
	const ePicType type;
	size_t page, cur;
public:
	cPictChoice(std::vector<pic_num_t>& pics, ePicType t, cDialog* parent = NULL);
	cPictChoice(std::vector<pic_num_t>::iterator begin, std::vector<pic_num_t>::iterator end, ePicType t, cDialog* parent = NULL);
	pic_num_t show(pic_num_t fallback, pic_num_t cur_sel); // returns the _number_ of the chosen picture, _not_ the index; there's no way to distinguish between duplicates
	// returns fallback if the user cancels
};
#endif

void giveError(std::string str1, std::string str2, short err, cDialog* parent = NULL);
void oopsError(short error,short code = 0, short mode = 0);
