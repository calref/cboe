/*
 *  container.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 2020-12-21.
 *
 */

#include "container.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "dialogxml/widgets/button.hpp"
#include "dialogxml/widgets/field.hpp"
#include "dialogxml/widgets/ledgroup.hpp"
#include "dialogxml/widgets/message.hpp"
#include "dialogxml/widgets/pict.hpp"
#include "dialogxml/widgets/scrollbar.hpp"

bool cContainer::parseChildControl(ticpp::Element& elem, std::map<std::string,cControl*>& controls, std::string& id) {
	std::string tag = elem.Value();
	if(tag == "field") {
		auto field = parent->parse<cTextField>(elem);
		controls.insert(field);
		parent->tabOrder.push_back(field);
		id = field.first;
	} else if(tag == "text") {
		auto text = parent->parse<cTextMsg>(elem);
		controls.insert(text);
		id = text.first;
	} else if(tag == "pict") {
		auto pict = parent->parse<cPict>(elem);
		controls.insert(pict);
		id = pict.first;
	} else if(tag == "slider") {
		auto slide = parent->parse<cScrollbar>(elem);
		controls.insert(slide);
		id = slide.first;
	} else if(tag == "button") {
		auto button = parent->parse<cButton>(elem);
		controls.insert(button);
		id = button.first;
	} else if(tag == "led") {
		auto led = parent->parse<cLed>(elem);
		controls.insert(led);
		id = led.first;
	} else if(tag == "group") {
		auto group = parent->parse<cLedGroup>(elem);
		controls.insert(group);
		id = group.first;
	} else return false;
	return true;
}

bool cContainer::handleClick(location where) {
	std::string which_clicked;
	bool success = false;
	forEach([&](std::string id, cControl& ctrl) {
		if(!success && ctrl.isClickable() && ctrl.getBounds().contains(where)) {
			if(ctrl.handleClick(where)){
				success = true;
				which_clicked = id;
			}
		}
	});
	if(!which_clicked.empty())
		clicking = which_clicked;
	return success;
}

void cContainer::callHandler(event_fcn<EVT_CLICK>::type onClick, cDialog& me, std::string id, eKeyMod mods) {
	std::string which_clicked = clicking;
	clicking = "";
	
	if(onClick) onClick(me, id, mods);
	if(!which_clicked.empty())
		getChild(which_clicked).triggerClickHandler(me, which_clicked, mods);
}
