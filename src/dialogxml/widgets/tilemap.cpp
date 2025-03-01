//
//  tilemap.cpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-02-01.
//

#include "tilemap.hpp"

#include "button.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "field.hpp"
#include "message.hpp"
#include "pict.hpp"
#include "scrollbar.hpp"
#include "stack.hpp"
#include <climits>

std::string cTilemap::generateId(const std::string& baseId) const {
	if(baseId.empty()) {
		if(id_tries++ == 0) return current_cell;
		return cContainer::generateId(baseId) + "-" + current_cell;
	}
	return baseId + "-" + current_cell;
}

location cTilemap::getCell(const std::string& id) const {
	size_t y_pos = id.find_last_of("y");
	size_t x_pos = id.find_last_of("x");
	std::string x_str = id.substr(x_pos + 1, y_pos);
	std::string y_str = id.substr(y_pos + 1);
	return location(std::stoi(x_str) * cellWidth, std::stoi(y_str) * cellHeight);
}

location cTilemap::getCellPos(size_t x, size_t y) const {
	rectangle bounds = getBounds();
	bounds.offset(x * cellWidth, y * cellHeight);
	return bounds.topLeft();
}

location cTilemap::getCellPos(cControl& child) const {
	// If it's not ours, return origin.
	if(!hasChild(child.getName())) return location();
	return getCell(child.getName());
}

std::string cTilemap::buildId(const std::string& base, size_t x, size_t y) {
	std::ostringstream sout;
	if(!base.empty()) {
		sout << base << '-';
	}
	sout << 'x' << x << 'y' << y;
	return sout.str();
}

bool cTilemap::hasChild(std::string id) const {
	return controls.find(id) != controls.end();
}

cControl& cTilemap::getChild(std::string id) {
	if(!hasChild(id)) throw std::invalid_argument(id + " was not found in the tilemap");
	return *controls[id];
}

bool cTilemap::hasChild(std::string id, size_t x, size_t y) const {
	return hasChild(buildId(id, x, y));
}

cControl& cTilemap::getChild(std::string id, size_t x, size_t y) {
	return getChild(buildId(id, x, y));
}

bool cTilemap::hasChild(size_t x, size_t y) const {
	return hasChild("", x, y);
}

cControl& cTilemap::getChild(size_t x, size_t y) {
	return getChild("", x, y);
}

bool cTilemap::manageFormat(eFormat prop, bool set, boost::any* val) {
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

bool cTilemap::isClickable() const {
	return true;
}

bool cTilemap::isFocusable() const {
	return false;
}

bool cTilemap::isScrollable() const {
	return false;
}

bool cTilemap::handleClick(location where, cFramerateLimiter& fps_limiter) {
	bool success = false;
	isHandlingClick = true;
	forEach([&](std::string id, cControl& ctrl) {
		rectangle bounds = ctrl.getBounds();
		bounds.offset(getBounds().topLeft());
		bounds.offset(getCell(id));
		if(!success && ctrl.isClickable() && bounds.contains(where)) {
			rectangle localBounds = ctrl.getBounds();
			ctrl.setBounds(bounds);
			clicking = id;
			if(ctrl.handleClick(where, fps_limiter)){
				success = true;
			} else clicking.clear();
			ctrl.setBounds(localBounds);
		}
	});
	isHandlingClick = false;
	return success;
}

void cTilemap::draw() {
	if(!isVisible()) return;
	for(auto& ctrl : controls) {
		rectangle localBounds = ctrl.second->getBounds();
		rectangle globalBounds = localBounds;
		if(isHandlingClick ? ctrl.first != clicking : true) {
			globalBounds.offset(getBounds().topLeft());
			globalBounds.offset(getCell(ctrl.first));
			ctrl.second->setBounds(globalBounds);
		}
		ctrl.second->draw();
		ctrl.second->setBounds(localBounds);
	}
	drawFrame(2, frameStyle);
}

void cTilemap::recalcRect() {
	auto iter = controls.begin();
	auto location = frame.topLeft();
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
	frame.offset(location);
	frame.right += spacing;
	frame.bottom += spacing;
	cellWidth = frame.width();
	cellHeight = frame.height();
	frame.width() *= cols;
	frame.height() *= rows;
	frame.right -= spacing;
	frame.bottom -= spacing;
}

void cTilemap::attachClickHandlers(std::function<bool(cDialog&,std::string,eKeyMod)> handler, std::string prefix) {
	for(auto& ctrl : controls) {
		bool matches = false;
		if(prefix.empty()) {
			size_t first_x = ctrl.first.find_first_of("x"), last_x = ctrl.first.find_last_of("x"), first_y = ctrl.first.find_first_of("y"), last_y = ctrl.first.find_last_of("y");
			if(first_x == 0 && last_x == 0 && first_y != std::string::npos && first_y == last_y) {
				matches = true;
			}
		} else {
			if(ctrl.first.compare(0, prefix.size(), prefix)) {
				matches = true;
			}
		}
		if(matches) {
			ctrl.second->attachClickHandler(handler);
		}
	}
}

void cTilemap::fillTabOrder(std::vector<int>& specificTabs, std::vector<int>& reverseTabs) {
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

cTilemap::cTilemap(iComponent& parent)
	: cContainer(CTRL_MAP, parent)
{}

void cTilemap::forEach(std::function<void(std::string,cControl&)> callback) {
	for(auto ctrl : controls)
		callback(ctrl.first, *ctrl.second);
}

bool cTilemap::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	if(attr.Name() == "rows") {
		try {
			attr.GetValue(&rows);
		} catch(ticpp::Exception&) {
			throw xBadVal(tagName, attr.Name(), attr.Value(), attr.Row(), attr.Column(), fname);
		}
		return true;
	} else if(attr.Name() == "cols") {
		try {
			attr.GetValue(&cols);
		} catch(ticpp::Exception&) {
			throw xBadVal(tagName, attr.Name(), attr.Value(), attr.Row(), attr.Column(), fname);
		}
		return true;
	} else if(attr.Name() == "cellspacing") {
		try {
			attr.GetValue(&spacing);
		} catch(ticpp::Exception&) {
			throw xBadVal(tagName, attr.Name(), attr.Value(), attr.Row(), attr.Column(), fname);
		}
		return true;
	}
	return cContainer::parseAttribute(attr, tagName, fname);
}

bool cTilemap::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	using namespace ticpp;
	if(content.Type() == TiXmlNode::ELEMENT) {
		std::string id;
		auto& elem = dynamic_cast<Element&>(content);
		id_tries = 0;
		current_cell = "x0y0";
		if(!parseChildControl(elem, controls, id, fname)) return false;
		for(size_t x = 0; x < cols; x++) {
			for(size_t y = 0; y < rows; y++) {
				if(x == 0 && y == 0) continue; // already did this one
				id_tries = 0;
				std::ostringstream sout;
				sout << "x" << x << "y" << y;
				current_cell = sout.str();
				parseChildControl(elem, controls, id, fname);
			}
		}
		return true;
	}
	return cContainer::parseContent(content, n, tagName, fname, text);
}

void cTilemap::validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) {
	cControl::validatePostParse(who, fname, attrs, nodes);
	if(!attrs.count("rows")) throw xMissingAttr(who.Value(), "rows", who.Row(), who.Column(), fname);
	if(!attrs.count("cols")) throw xMissingAttr(who.Value(), "cols", who.Row(), who.Column(), fname);
}
