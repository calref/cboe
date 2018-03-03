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

void cStack::setFormat(eFormat prop, short val) throw(xUnsupportedProp) {
	if(prop == TXT_FRAME) drawFramed = val;
	else if(prop == TXT_FRAMESTYLE) frameStyle = eFrameStyle(val);
	else throw xUnsupportedProp(prop);
}

short cStack::getFormat(eFormat prop) throw(xUnsupportedProp) {
	if(prop == TXT_FRAME) return drawFramed;
	else if(prop == TXT_FRAMESTYLE) return frameStyle;
	throw xUnsupportedProp(prop);
}

void cStack::setColour(sf::Color) throw(xUnsupportedProp) {
	// TODO: Colour is not supported
}

sf::Color cStack::getColour() throw(xUnsupportedProp) {
	// TODO: Colour is not supported
	return sf::Color();
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

std::string cStack::parse(ticpp::Element& who, std::string fname) {
	using namespace ticpp;
	Iterator<Attribute> attr;
	Iterator<Element> node;
	std::string name, id;
	for(attr = attr.begin(&who); attr != attr.end(); attr++) {
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&id);
		else if(name == "pages") {
			size_t val;
			attr->GetValue(&val);
			setPageCount(val);
		}else if(name == "framed"){
			std::string val;
			attr->GetValue(&val);
			if(val == "true") setFormat(TXT_FRAME, true);
		}else if(name == "outline") {
			std::string val;
			attr->GetValue(&val);
			if(val == "solid") setFormat(TXT_FRAMESTYLE, FRM_SOLID);
			else if(val == "inset") setFormat(TXT_FRAMESTYLE, FRM_INSET);
			else if(val == "outset") setFormat(TXT_FRAMESTYLE, FRM_OUTSET);
			else if(val == "double") setFormat(TXT_FRAMESTYLE, FRM_DOUBLE);
		} else throw xBadAttr("stack",name,attr->Row(),attr->Column(),fname);
	}
	for(node = node.begin(&who); node != node.end(); node++) {
		std::string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::ELEMENT) {
			if(val == "field") {
				auto field = parent->parse<cTextField>(*node);
				controls.insert(field);
				// TODO: Add field to tab order
				//tabOrder.push_back(field);
			} else if(val == "text") {
				auto text = parent->parse<cTextMsg>(*node);
				controls.insert(text);
			} else if(val == "pict") {
				auto pict = parent->parse<cPict>(*node);
				controls.insert(pict);
			} else if(val == "slider") {
				auto slide = parent->parse<cScrollbar>(*node);
				controls.insert(slide);
			} else if(val == "button") {
				auto button = parent->parse<cButton>(*node);
				controls.insert(button);
			} else if(val == "led") {
				auto led = parent->parse<cLed>(*node);
				controls.insert(led);
			} else if(val == "group") {
				auto group = parent->parse<cLedGroup>(*node);
				controls.insert(group);
			} else throw xBadNode(val,node->Row(),node->Column(),fname);
		} else if(type != TiXmlNode::COMMENT)
			throw xBadVal("stack",xBadVal::CONTENT,val,node->Row(),node->Column(),fname);
	}
	recalcRect();
	return id;
}

