#include "btnpanel.hpp"

#include <sstream>
#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "dialogxml/widgets/field.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "sounds.hpp"
#include "gfx/render_shapes.hpp"
#include "tools/cursors.hpp"

cButtonPanel::cButtonPanel(cDialog* parent)
	: dlg(*ResMgr::dialogs.get("tiny-button-panel"), parent)
{}

cButtonPanel::cButtonPanel(const std::vector<std::string>& strs, std::vector<std::function<void(cButtonPanel&)>> click_handlers, std::string title, std::string ok_str, cDialog* parent)
	: cButtonPanel(parent)
{
	setTitle(title);
	if(!ok_str.empty()){
		dlg["done"].setText(ok_str);
	}
	strings = strs;
	this->click_handlers = click_handlers;
	attachHandlers();
}

void cButtonPanel::attachHandlers() {
	using namespace std::placeholders;
	dlg["left"].attachClickHandler(std::bind(&cButtonPanel::onLeft,this));
	dlg["right"].attachClickHandler(std::bind(&cButtonPanel::onRight,this));
	dlg["done"].attachClickHandler(std::bind(&cButtonPanel::onOkay,this,_1));
	dlg["cancel"].attachClickHandler(std::bind(&cButtonPanel::onCancel,this,_1));
	if(strings.size() <= per_page) {
		dlg["left"].hide();
		dlg["right"].hide();
	}
	// Attach click handler to the tiny buttons
	for(int i = 0; i < per_page; ++i){
		short string_idx = page * per_page + i;
		std::ostringstream sout;
		sout << "button" << i + 1;
		dlg[sout.str()].attachClickHandler([i, this](cDialog&,std::string,eKeyMod) -> bool {
			click_handlers[i](*this);
			return true;
		});
	}
}

cDialog* cButtonPanel::operator->() {
	return &dlg;
}

bool cButtonPanel::show() {
	page = 0;
	dlg.run(std::bind(&cButtonPanel::fillPage, this));
	return dlg.getResult<bool>();
}

void cButtonPanel::fillPage(){
	for(unsigned int i = 0; i < per_page; i++){
		short string_idx = page * per_page + i;
		std::ostringstream sout;
		sout << "button" << i + 1;
		if(string_idx < strings.size()){
			dlg[sout.str()].setText(strings[string_idx]);
			dlg[sout.str()].recalcRect();
			dlg[sout.str()].show();
		}else{
			dlg[sout.str()].hide();
		}
	}
}

bool cButtonPanel::onLeft(){
	if(page == 0) page = lastPage();
	else page--;
	fillPage();
	return true;
}

bool cButtonPanel::onRight(){
	if(page == lastPage()) page = 0;
	else page++;
	fillPage();
	return true;
}

bool cButtonPanel::onCancel(cDialog& me){
	dlg.setResult(false);
	me.toast(false);
	return true;
}

bool cButtonPanel::onOkay(cDialog& me){
	dlg.setResult(true);
	me.toast(true);
	return true;
}

void cButtonPanel::setTitle(const std::string &title) {
	if(!title.empty()) dlg["title"].setText(title);
}

size_t cButtonPanel::lastPage() const {
	return (strings.size() - 1) / per_page;
}