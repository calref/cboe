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

void cStack::attachClickHandler(click_callback_t f) throw(xHandlerNotSupported) {
	onClick = f;
}

void cStack::attachFocusHandler(focus_callback_t) throw(xHandlerNotSupported) {
	throw xHandlerNotSupported(true);
}

// TODO: The only reason the handleClick and delegation here is needed is because the engine currently has no concept of layering.
// This means a stack hides any of its controls that happen to end up underneath it.
bool cStack::triggerClickHandler(cDialog& me, std::string id, eKeyMod mods) {
	std::string which_clicked = clicking;
	clicking = "";
	
	if(onClick) onClick(me, id, mods);
	return parent->getControl(which_clicked).triggerClickHandler(me, id, mods);
}

bool cStack::handleClick(location where) {
	std::string which_clicked;
	auto iter = controls.begin();
	while(iter != controls.end()){
		if(parent->getControl(*iter).isVisible() && where.in(parent->getControl(*iter).getBounds())){
			if(parent->getControl(*iter).handleClick(where)) {
				which_clicked = *iter;
				break;
			}
		}
		iter++;
	}
	
	if(which_clicked == "") return false;
	
	clicking = which_clicked;
	return true;
}

void cStack::setFormat(eFormat prop, short) throw(xUnsupportedProp) {
	throw xUnsupportedProp(prop);
}

short cStack::getFormat(eFormat prop) throw(xUnsupportedProp) {
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

void cStack::draw() {}

bool cStack::setPage(size_t n) {
	if(n >= nPages) return false;
	if(n == curPage) return true;
	cTextField* focus = parent->getFocus();
	bool failed = false;
	for(auto id : controls) {
		cControl& ctrl = parent->getControl(id);
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
		cControl& ctrl = parent->getControl(*iter);
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

cControl& cStack::operator[](std::string id) {
	auto iter = std::find(controls.begin(), controls.end(), id);
	if(iter == controls.end()) throw std::invalid_argument(id + " does not exist in the stack.");
	return parent->getControl(id);
}

void cStack::fillTabOrder(std::vector<int>& specificTabs, std::vector<int>& reverseTabs) {
	for(auto id : controls) {
		cControl& ctrl = parent->getControl(id);
		if(ctrl.getType() == CTRL_FIELD) {
			cTextField& field = dynamic_cast<cTextField&>(ctrl);
			if(field.tabOrder > 0)
				specificTabs.push_back(field.tabOrder);
			else if(field.tabOrder < 0)
				reverseTabs.push_back(field.tabOrder);
		}
	}
}

cStack::cStack(cDialog& parent) : cControl(CTRL_STACK, parent), curPage(0), nPages(0) {}

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
		} else throw xBadAttr("stack",name,attr->Row(),attr->Column(),fname);
	}
	std::vector<std::string> stack;
	for(node = node.begin(&who); node != node.end(); node++) {
		std::string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::ELEMENT) {
			if(val == "field") {
				auto field = parent->parse<cTextField>(*node);
				parent->add(field.second, field.second->getBounds(), field.first);
				stack.push_back(field.first);
				// TODO: Add field to tab order
				//tabOrder.push_back(field);
			} else if(val == "text") {
				auto text = parent->parse<cTextMsg>(*node);
				parent->add(text.second, text.second->getBounds(), text.first);
				stack.push_back(text.first);
			} else if(val == "pict") {
				auto pict = parent->parse<cPict>(*node);
				parent->add(pict.second, pict.second->getBounds(), pict.first);
				stack.push_back(pict.first);
			} else if(val == "slider") {
				auto slide = parent->parse<cScrollbar>(*node);
				parent->add(slide.second, slide.second->getBounds(), slide.first);
				stack.push_back(slide.first);
			} else if(val == "button") {
				auto button = parent->parse<cButton>(*node);
				parent->add(button.second, button.second->getBounds(), button.first);
				stack.push_back(button.first);
			} else if(val == "led") {
				auto led = parent->parse<cLed>(*node);
				parent->add(led.second, led.second->getBounds(), led.first);
				stack.push_back(led.first);
			} else if(val == "group") {
				auto group = parent->parse<cLedGroup>(*node);
				parent->add(group.second, group.second->getBounds(), group.first);
				stack.push_back(group.first);
			} else throw xBadNode(val,node->Row(),node->Column(),fname);
		} else if(type != TiXmlNode::COMMENT)
			throw xBadVal("stack",xBadVal::CONTENT,val,node->Row(),node->Column(),fname);
	}
	controls = stack;
	recalcRect();
	return id;
}

