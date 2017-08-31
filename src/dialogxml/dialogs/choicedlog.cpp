/*
 *  dlogutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "choicedlog.hpp"

#include <functional>
#include "control.hpp"

cChoiceDlog::cChoiceDlog(std::string file, std::vector<std::string> buttons, cDialog* p) : dlg(file, p) {
	using namespace std::placeholders;
	std::vector<std::string>::iterator iter = buttons.begin();
	while(iter != buttons.end()){
		dlg[*iter].attachClickHandler(std::bind(&cChoiceDlog::onClick,this,_1,_2));
		iter++;
	}
}

cChoiceDlog::cChoiceDlog(cDialog* p) : dlg(p) {}

cChoiceDlog::cChoiceDlog(std::string file, cDialog* p)
	: cChoiceDlog(file, {"okay"}, p) {}

// so that the caller can set up aspects of the dialog if necessary
cDialog* cChoiceDlog::operator->(){
	return &dlg;
}

std::string cChoiceDlog::show(){
	dlg.run();
	return dlg.getResult<std::string>();
}

bool cChoiceDlog::onClick(cDialog& me, std::string id){
	me.setResult(id);
	me.toast(true);
	return true;
}
