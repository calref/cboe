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
#include "render_shapes.hpp"
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

bool cScrollPane::manageFormat(eFormat prop, bool set, boost::any* val) {
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

bool cScrollPane::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	if(attr.Name() == "style"){
		std::string val = attr.Value();
		if(val == "white") setStyle(SCROLL_WHITE);
		else if(val == "led") setStyle(SCROLL_LED);
		else throw xBadVal(tagName, attr.Name(), val, attr.Row(), attr.Column(), fname);
		return true;
	}
	return cContainer::parseAttribute(attr, tagName, fname);
}

bool cScrollPane::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	if(content.Type() == TiXmlNode::ELEMENT) {
		return parseChildControl(dynamic_cast<ticpp::Element&>(content), contents);
	}
	return cContainer::parseContent(content, n, tagName, fname, text);
}

void cScrollPane::validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) {
	cContainer::validatePostParse(who, fname, attrs, nodes);
	if(!attrs.count("style")) setStyle(SCROLL_LED);
}
