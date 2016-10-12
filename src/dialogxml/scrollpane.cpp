//
//  scrollpane.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-10-01.
//
//

#include "scrollpane.hpp"
#include "message.hpp"
#include "field.hpp"
#include "button.hpp"
#include "pict.hpp"
#include "stack.hpp"
#include <climits>

cScrollPane::cScrollPane(cDialog& parent) : cContainer(CTRL_PANE, parent), scroll(parent) {
	recalcRect();
}

bool cScrollPane::handleClick(location where) {
	if(scroll.getBounds().contains(where))
		return scroll.handleClick(where);
	where.y += scroll.getPosition();
	return cContainer::handleClick(where);
}

bool cContainer::handleClick(location where) {
	std::string which_clicked;
	bool success = false;
	forEach([&](std::string id, cControl& ctrl) {
		if(ctrl.isClickable() && ctrl.getBounds().contains(where)) {
			success = ctrl.handleClick(where);
			which_clicked = id;
		}
	});
	if(!which_clicked.empty())
		clicking = which_clicked;
	return success;
}

void cScrollPane::recalcRect() {
	auto iter = contents.begin();
	globalFrame = {INT_MAX, INT_MAX, 0, 0};
	while(iter != contents.end()){
		cControl& ctrl = *iter->second;
		rectangle otherFrame = ctrl.getBounds();
		if(otherFrame.right > globalFrame.right)
			globalFrame.right = otherFrame.right;
		if(otherFrame.bottom > globalFrame.bottom)
			globalFrame.bottom = otherFrame.bottom;
		if(otherFrame.left < globalFrame.left)
			globalFrame.left = otherFrame.left;
		if(otherFrame.top < globalFrame.top)
			globalFrame.top = otherFrame.top;
		iter++;
	}
	globalFrame.inset(-4,-4);
	frame.left = globalFrame.left;
	frame.right = globalFrame.right;
	scroll.setMaximum((globalFrame.height() - frame.height()) / 5);
	scroll.setPageSize((frame.height() - 5) / 5);
	rectangle scrollFrame;
	scrollFrame.left = frame.right;
	scrollFrame.top = frame.top;
	scrollFrame.width() = 16;
	scrollFrame.height() = frame.height();
	scroll.setBounds(scrollFrame);
	frame.right += scrollFrame.width();
}

void cScrollPane::setFormat(eFormat prop, short val) throw(xUnsupportedProp) {
	if(prop == TXT_FRAME) framed = val;
	else if(prop == TXT_FRAMESTYLE) frameStyle = eFrameStyle(val);
	else throw xUnsupportedProp(prop);
}

short cScrollPane::getFormat(eFormat prop) throw(xUnsupportedProp) {
	if(prop == TXT_FRAME) return framed;
	else if(prop == TXT_FRAMESTYLE) return frameStyle;
	else throw xUnsupportedProp(prop);
}

void cScrollPane::setColour(sf::Color) throw(xUnsupportedProp) {
	// TODO: Colour is not supported
}

sf::Color cScrollPane::getColour() throw(xUnsupportedProp) {
	// TODO: Colour is not supported
	return sf::Color();
}

cControl::storage_t cScrollPane::store() {
	// We don't call the superclass store() here like other controls do
	storage_t storage;
	storage[""] = scroll.store();
	for(auto& ctrl : contents)
		storage[ctrl.first] = ctrl.second->store();
	return storage;
}

void cScrollPane::restore(storage_t to) {
	// We don't call the superclass restore() here like the other controls do
	if(to.find("") != to.end())
		scroll.restore(boost::any_cast<storage_t>(to[""]));
	for(auto& ctrl : contents) {
		if(to.find(ctrl.first) != to.end())
			ctrl.second->restore(boost::any_cast<storage_t>(to[ctrl.first]));
	}
}

bool cScrollPane::isClickable() {
	return true;
}

bool cScrollPane::isFocusable() {
	return false;
}

bool cScrollPane::isScrollable() {
	return true;
}

long cScrollPane::getPosition() {
	return scroll.getPosition();
}

long cScrollPane::getMaximum() {
	return scroll.getMaximum();
}

void cScrollPane::setPosition(long to) {
	scroll.setPosition(to);
}

eScrollStyle cScrollPane::getStyle() {
	return scroll.getStyle();
}

void cScrollPane::setStyle(eScrollStyle style) {
	scroll.setStyle(style);
}

bool cScrollPane::hasChild(std::string id) {
	return contents.find(id) != contents.end();
}

cControl& cScrollPane::getChild(std::string id) {
	auto iter = contents.find(id);
	if(iter == contents.end()) throw std::invalid_argument(id + " does not exist in the scroll pane.");
	return *(iter->second);
}

void cScrollPane::addChild(cControl* ctrl, std::string key) {
	contents[key] = ctrl;
}

void cScrollPane::draw() {
	if(!visible) return;
	inWindow->setActive();
	clip_rect(*inWindow, getBounds());
	for(auto& ctrl : contents) {
		rectangle localBounds = ctrl.second->getBounds();
		rectangle globalBounds = localBounds;
		globalBounds.offset(0, -5 * scroll.getPosition());
		ctrl.second->setBounds(globalBounds);
		ctrl.second->draw();
		ctrl.second->setBounds(localBounds);
	}
	undo_clip(*inWindow);
	scroll.draw();
	if(framed)
		drawFrame(4, frameStyle);
}

void cScrollPane::forEach(std::function<void(std::string,cControl&)> callback) {
	for(auto ctrl : contents)
		callback(ctrl.first, *ctrl.second);
}

std::string cScrollPane::parse(ticpp::Element& who, std::string fname) {
	using namespace ticpp;
	Iterator<Attribute> attr;
	Iterator<Element> node;
	std::string name, id;
	rectangle frame;
	int width = 0, height = 0;
	bool foundTop = false, foundLeft = false;
	scroll.setStyle(SCROLL_LED);
	for(attr = attr.begin(&who); attr != attr.end(); attr++) {
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&id);
		else if(name == "top")
			attr->GetValue(&frame.top), foundTop = true;
		else if(name == "left")
			attr->GetValue(&frame.left), foundLeft = true;
		else if(name == "width")
			attr->GetValue(&width);
		else if(name == "height")
			attr->GetValue(&height);
		else if(name == "framed") {
			std::string val;
			attr->GetValue(&val);
			if(val == "true") setFormat(TXT_FRAME, true);
			else setFormat(TXT_FRAME, false);
		}else if(name == "outline") {
			std::string val;
			attr->GetValue(&val);
			if(val == "solid") setFormat(TXT_FRAMESTYLE, FRM_SOLID);
			else if(val == "inset") setFormat(TXT_FRAMESTYLE, FRM_INSET);
			else if(val == "outset") setFormat(TXT_FRAMESTYLE, FRM_OUTSET);
			else if(val == "double") setFormat(TXT_FRAMESTYLE, FRM_DOUBLE);
		}else if(name == "style"){
			std::string val;
			attr->GetValue(&val);
			if(val == "white") setStyle(SCROLL_WHITE);
			else if(val == "led") setStyle(SCROLL_LED);
			else throw xBadVal("slider", name, val, attr->Row(), attr->Column(), fname);
		} else throw xBadAttr("pane",name,attr->Row(),attr->Column(),fname);
	}
	if(!foundTop) throw xMissingAttr("pane","top",who.Row(),who.Column(),fname);
	if(!foundLeft) throw xMissingAttr("pane","left",who.Row(),who.Column(),fname);
	if(width > 0) {
		frame.width() = width;
	}else{
		throw xMissingAttr("pane","width",who.Row(),who.Column(),fname);
	}
	if(height > 0) {
		frame.height() = height;
	}else{
		frame.height() = 10;
	}
	setBounds(frame);
	for(node = node.begin(&who); node != node.end(); node++) {
		std::string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::ELEMENT) {
			if(val == "field") {
				auto field = parent->parse<cTextField>(*node);
				contents.insert(field);
				// TODO: Add field to tab order
				//tabOrder.push_back(field);
			} else if(val == "text") {
				auto text = parent->parse<cTextMsg>(*node);
				contents.insert(text);
			} else if(val == "pict") {
				auto pict = parent->parse<cPict>(*node);
				contents.insert(pict);
			} else if(val == "slider") {
				auto slide = parent->parse<cScrollbar>(*node);
				contents.insert(slide);
			} else if(val == "button") {
				auto button = parent->parse<cButton>(*node);
				contents.insert(button);
			} else if(val == "led") {
				auto led = parent->parse<cLed>(*node);
				contents.insert(led);
			} else if(val == "group") {
				auto group = parent->parse<cLedGroup>(*node);
				contents.insert(group);
			} else throw xBadNode(val,node->Row(),node->Column(),fname);
		} else if(type != TiXmlNode::COMMENT)
			throw xBadVal("pane",xBadVal::CONTENT,val,node->Row(),node->Column(),fname);
	}
	recalcRect();
	return id;
}
