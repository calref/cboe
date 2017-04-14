/*
 *  dlogutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "strdlog.hpp"

#include <sstream>
#include "winutil.hpp"
#include "pict.hpp"

std::string cStrDlog::getFileName(short n_strs, ePicType type, bool hasTitle){
	std::ostringstream sout;
	sout << n_strs << "str";
	if(hasTitle) sout << "-title";
	if(type == PIC_DLOG_LG || type == PIC_CUSTOM_DLOG_LG || type == PIC_SCEN_LG)
		sout << "-lg";
	return sout.str();
}

cStrDlog::cStrDlog(std::string str1,std::string str2,std::string title,pic_num_t pic,ePicType t,cDialog* parent)
	  : dlg(cStrDlog::getFileName((str1 != "") + (str2 != ""), t, title != ""), parent), type(t) {
	using namespace std::placeholders;
	cPict& pic_ctrl = dynamic_cast<cPict&>(dlg["pict"]);
	pic_ctrl.setPict(pic, type);
	if(str1 != "") {
		dlg["str1"].setText(str1);
		if(str2 != "") dlg["str2"].setText(str2);
	}else if(str2 != "") dlg["str1"].setText(str2);
	if(title != "") dlg["title"].setText(title);
	dlg["record"].hide();
	dlg["record"].attachClickHandler(std::bind(&cStrDlog::onRecord, this, _2));
	dlg["done"].attachClickHandler(std::bind(&cStrDlog::onDismiss, this));
}

bool cStrDlog::onRecord(std::string id){
	if(hasRecord) rec_f(dlg);
	else dlg[id].hide();
	return hasRecord;
}

bool cStrDlog::onDismiss(){
	dlg.toast(true);
	return true;
}

cStrDlog& cStrDlog::setSound(snd_num_t snd){
	sound = snd;
	return *this;
}

cStrDlog& cStrDlog::setRecordHandler(record_callback_t rec){
	if(rec == nullptr){
		hasRecord = false;
		dlg["record"].hide();
	}else{
		hasRecord = true;
		rec_f = rec;
		dlg["record"].show();
	}
	return *this;
}

cDialog* cStrDlog::operator->() {
	return &dlg;
}

void cStrDlog::show(){
	if(sound > 0) play_sound(sound);
	dlg.run();
}

static void giveError(pic_num_t pic, std::string title, std::string str1, std::string str2, cDialog* parent) {
	cStrDlog error(str1,str2,title,pic,PIC_DLOG,parent);
	error->getControl("record").setText("Copy");
	error.setRecordHandler([](cDialog& me) {
		std::string error = me["str1"].getText();
		try {
			std::string error2 = me["str2"].getText();
			error += "\n\n" + error2;
		} catch(std::invalid_argument) {}
		for(char& c : error) {
			if(c == '|') c = '\n';
		}
		set_clipboard(error);
	});
	error.show();
}

void showError(std::string str1, cDialog* parent) {
	showError(str1, "", parent);
}

void showError(std::string str1, std::string str2, cDialog* parent) {
	giveError(25, "Error", str1, str2, parent);
}

void showWarning(std::string str1, cDialog* parent) {
	showWarning(str1, "", parent);
}

void showWarning(std::string str1, std::string str2, cDialog* parent) {
	giveError(24, "Warning", str1, str2, parent);
}

void showFatalError(std::string str1, cDialog* parent) {
	showFatalError(str1, "", parent);
}

void showFatalError(std::string str1, std::string str2, cDialog* parent) {
	giveError(25, "Error!!!", str1, str2, parent);
}
