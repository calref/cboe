//
//  stack.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-22.
//
//

#include "stack.hpp"
#include "field.hpp"
#include "button.hpp"
#include "message.hpp"
#include "pict.hpp"
#include "scrollbar.hpp"
#include <climits>

bool cStack::hasChild(std::string id) {
	return controls.find(id) != controls.end();
}

cControl& cStack::getChild(std::string id) {
	if(!hasChild(id)) throw std::invalid_argument(id + " was not found in the stack");
	return *controls[id];
}

void cContainer::callHandler(event_fcn<EVT_CLICK>::type onClick, cDialog& me, std::string id, eKeyMod mods) {
	std::string which_clicked = clicking;
	clicking = "";
	
	if(onClick) onClick(me, id, mods);
	if(!which_clicked.empty())
		getChild(which_clicked).triggerClickHandler(me, which_clicked, mods);
}

bool cStack::manageFormat(eFormat prop, bool set, boost::any* val) {
	switch(prop) {
		case TXT_FRAME:
			if(val) {
				if(set) frameStyle = boost::any_cast<eFrameStyle>(*val);
				else *val = frameStyle;
			}
			break;
		// TODO: Colour is not supported
		default: return false;
	}
	return true;
}

bool cStack::isClickable() {
	return true;
}

bool cStack::isFocusable() {
	return false;
}

bool cStack::isScrollable() {
	return false;
}

void cStack::draw() {
	if(!isVisible()) return;
	for(auto& p : controls) {
		p.second->draw();
	}
	if(drawFramed) drawFrame(2, frameStyle);
}

bool cStack::setPage(size_t n) {
	if(n >= nPages) return false;
	if(n == curPage) return true;
	cTextField* focus = parent->getFocus();
	bool failed = false;
	for(auto p : controls) {
		const std::string& id = p.first;
		cControl& ctrl = *p.second;
		// Only trigger focus handlers if the current page still exists.
		if(curPage < nPages) {
			storage[curPage][id] = ctrl.store();
			if(!ctrl.triggerFocusHandler(*parent, id, true))
				failed = true;
			if(!failed) {
				ctrl.restore(storage[n][id]);
				if(focus == &ctrl && !ctrl.triggerFocusHandler(*parent, id, false)) {
					failed = true;
					ctrl.restore(storage[curPage][id]);
				}
			}
		}
	}
	if(!failed) curPage = n;
	return !failed;
}

size_t cStack::getPage() {
	return curPage;
}

void cStack::setPageCount(size_t n) {
	if(curPage >= n && n > 0)
		setPage(nPages - 1);
	if(n == 0) curPage = 0;
	nPages = n;
	storage.resize(nPages);
}

void cStack::addPage() {
	setPageCount(getPageCount() + 1);
}

size_t cStack::getPageCount() {
	return nPages;
}

void cStack::recalcRect() {
	auto iter = controls.begin();
	frame = {INT_MAX, INT_MAX, 0, 0};
	while(iter != controls.end()){
		cControl& ctrl = *iter->second;
		rectangle otherFrame = ctrl.getBounds();
		if(otherFrame.right > frame.right)
			frame.right = otherFrame.right;
		if(otherFrame.bottom > frame.bottom)
			frame.bottom = otherFrame.bottom;
		if(otherFrame.left < frame.left)
			frame.left = otherFrame.left;
		if(otherFrame.top < frame.top)
			frame.top = otherFrame.top;
		iter++;
	}
	frame.inset(-6,-6);
}

void cStack::fillTabOrder(std::vector<int>& specificTabs, std::vector<int>& reverseTabs) {
	for(auto p : controls) {
		cControl& ctrl = *p.second;
		if(ctrl.getType() == CTRL_FIELD) {
			cTextField& field = dynamic_cast<cTextField&>(ctrl);
			if(field.tabOrder > 0)
				specificTabs.push_back(field.tabOrder);
			else if(field.tabOrder < 0)
				reverseTabs.push_back(field.tabOrder);
		}
	}
}

cStack::cStack(cDialog& parent) : cContainer(CTRL_STACK, parent), curPage(0), nPages(0) {}

void cStack::forEach(std::function<void(std::string,cControl&)> callback) {
	for(auto ctrl : controls)
		callback(ctrl.first, *ctrl.second);
}


bool cStack::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	if(attr.Name() == "pages") {
		try {
			size_t n;
			attr.GetValue(&n);
			setPageCount(n);
		} catch(ticpp::Exception&) {
			throw xBadVal(tagName, attr.Name(), attr.Value(), attr.Row(), attr.Column(), fname);
		}
		return true;
	}
	return cContainer::parseAttribute(attr, tagName, fname);
}

bool cStack::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	if(content.Type() == TiXmlNode::ELEMENT) {
		std::string id;
		return parseChildControl(dynamic_cast<ticpp::Element&>(content), controls, id);
	}
	return cContainer::parseContent(content, n, tagName, fname, text);
}

void cStack::validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) {
	// Don't defer to the superclass, since this is an abstract widget with no position
	//cControl::validatePostParse(who, fname, attrs, nodes);
}

