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

typedef void (*record_callback_t)(std::string,std::string);

class cStrDlog {
	cDialog dlg, *parent;
	std::string str1, str2, title;
	short sound;
	record_callback_t rec_f;
	bool onRecord(cDialog& me, std::string id);
	bool onDismiss(cDialog& me, std::string id);
public:
	cStrDlog(std::string str) : dlg("1str.xml") {}
	cStrDlog(std::string str1, std::string str2, bool hasTitle) : dlg(hasTitle ? "1str-title.xml" : "2str.xml") {}
	cStrDlog(std::string str1, std::string str2, std::string title) : dlg("2str-title.xml") {}
	cStrDlog& setPict(short num, ePicType type);
	cStrDlog& setSound(short num);
	cStrDlog& setParent(cDialog* parent);
	void show();
};

class cChoiceDlog {
	cDialog dlg, *parent;
	bool onClick(cDialog& me, std::string id);
public:
	cChoiceDlog(std::string str) : dlg(str) {}
	cChoiceDlog& setParent(cDialog* parent);
	std::string show();
};

class cThreeChoice : public cChoiceDlog {
public:
	cThreeChoice(std::string str) : cChoiceDlog("1str-3-c.xml") {}
	cThreeChoice(std::string str1, std::string str2) : cChoiceDlog("2str-3-c.xml") {}
	cThreeChoice(std::string str1, std::string str2, std::string str3) : cChoiceDlog("3str-3-c.xml") {}
	cThreeChoice(std::string str1, std::string str2, std::string str3, std::string str4) : cChoiceDlog("4str-3-c.xml") {}
	cThreeChoice(std::string str1, std::string str2, std::string str3, std::string str4, std::string str5) : cChoiceDlog("5str-3-c.xml") {}
	cThreeChoice(std::string str1, std::string str2, std::string str3, std::string str4, std::string str5, std::string str6) : cChoiceDlog("6str-3-c.xml") {}
	cThreeChoice& setPict(short num, ePicType type);
	cThreeChoice& setParent(cDialog* parent);
	string show();
};

class cStringChoice {
	cDialog dlg, *parent;
	bool onLeft(cDialog& me, std::string id);
	bool onRight(cDialog& me, std::string id);
	bool onCancel(cDialog& me, std::string id);
	bool onOkay(cDialog& me, std::string id);
public:
	cStringChoice(std::vector<std::string>& strs);
	cStringChoice(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end);
	size_t show(); // returns the _index_ of the chosen string
};

class cPictChoice {
	cDialog dlg, *parent;
	bool onLeft(cDialog& me, std::string id);
	bool onRight(cDialog& me, std::string id);
	bool onCancel(cDialog& me, std::string id);
	bool onOkay(cDialog& me, std::string id);
public:
	cPictChoice(std::vector<short>& pics, ePicType type);
	cPictChoice(std::vector<short>::iterator begin, std::vector<short>::iterator end, ePicType type);
	short show(); // returns the _number_ of the chosen picture, _not_ the index; there's no way to distinguish between duplicates
};
#endif

void giveError(std::string str1, std::string str2, short err, cDialog* parent = NULL);
void oopsError(short error,short code = 0, short mode = 0);
