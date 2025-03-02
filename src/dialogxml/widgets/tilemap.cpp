//
//  tilemap.cpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-02-01.
//

#include "tilemap.hpp"

#include "button.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "dialogxml/widgets/ledgroup.hpp"
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

location cTilemap::getCellIdx(const std::string& id) const {
	size_t y_pos = id.find_last_of("y");
	size_t x_pos = id.find_last_of("x");
	if(y_pos == std::string::npos && x_pos == std::string::npos)
		return location();
	std::string x_str = id.substr(x_pos + 1, y_pos);
	std::string y_str = id.substr(y_pos + 1);
	return location(std::stoi(x_str), std::stoi(y_str));
}

location cTilemap::getCell(const std::string &id) const {
	location idx = getCellIdx(id);
	idx.x *= cellWidth;
	idx.y *= cellHeight;
	return idx;
}

location cTilemap::getCellPos(size_t x, size_t y) const {
	rectangle bounds = getBounds();
	bounds.offset(x * cellWidth, y * cellHeight);
	return bounds.topLeft();
}

location cTilemap::getCellPos(cControl& child) const {
	// TODO: If it's not ours, return origin. (Not done right now because hasChild fails for children of children.)
//	if(!hasChild(child.getName())) return location();
	return getCell(child.getName());
}

location cTilemap::getCellIdx(cControl &child) const {
	// TODO: If it's not ours, return origin. (Not done right now because hasChild fails for children of children.)
//	if(!hasChild(child.getName())) return location();
	return getCellIdx(child.getName());
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

void cTilemap::draw() {
	if(!isVisible()) return;
	for(auto& ctrl : controls) {
		ctrl.second->draw();
	}
	drawFrame(2, frameStyle);
}

void cTilemap::postChildrenResolve() {
	if(childrenPositioned) return cContainer::postChildrenResolve();
	rectangle saveFrame = getBounds();
	recalcRect(); // Calculate the rect with all cells on top of each other
	cellWidth = frame.width() + spacing;
	cellHeight = frame.height() + spacing;
	std::vector<cLedGroup*> groups;
	for(auto p : controls) {
		if(auto group = dynamic_cast<cLedGroup*>(p.second)) {
			groups.push_back(group);
			continue;
		}
		rectangle bounds = p.second->getBounds();
		bounds.offset(saveFrame.topLeft());
		bounds.offset(getCell(p.first));
		p.second->setBounds(bounds);
	}
	for(auto group : groups) {
		group->forEach([this, saveFrame](std::string id, cControl& ctrl) {
			rectangle bounds = ctrl.getBounds();
			bounds.offset(saveFrame.topLeft());
			bounds.offset(getCell(id));
			ctrl.setBounds(bounds);
		});
		group->recalcRect();
	}
	childrenPositioned = true;
	recalcRect(); // And calculate it again now that the controls are all moved into place
}

void cTilemap::recalcRect() {
	auto iter = controls.begin();
	auto location = frame.topLeft();
	frame = {INT_MAX, INT_MAX, 0, 0};
	while(iter != controls.end()){
		cControl& ctrl = *iter->second;
		rectangle otherFrame = ctrl.getBounds();
		if(auto group = dynamic_cast<cLedGroup*>(&ctrl)) {
			group->recalcRect();
			// Groups outset the rect by -6. Inset it by 6 to compensate.
			rectangle bounds = group->getBounds();
			bounds.inset(6, 6);
			otherFrame = bounds;
		}
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
	if(!childrenPositioned) {
		frame.offset(location);
	}
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

void cTilemap::attachFocusHandlers(std::function<bool(cDialog&,std::string,bool)> handler, std::string prefix) {
	for(auto& ctrl : controls) {
		bool matches = false;
		if(prefix.empty()) {
			size_t first_x = ctrl.first.find_first_of("x"), last_x = ctrl.first.find_last_of("x"), first_y = ctrl.first.find_first_of("y"), last_y = ctrl.first.find_last_of("y");
			if(first_x == 0 && last_x == 0 && first_y != std::string::npos && first_y == last_y) {
				matches = true;
			}
		} else {
			if(ctrl.first.compare(0, prefix.size(), prefix) == 0) {
				matches = true;
			}
		}
		if(matches) {
			ctrl.second->attachFocusHandler(handler);
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

template<typename Fcn>
bool cTilemap::parseInstances(Fcn parse) {
	for(size_t x = 0; x < cols; x++) {
		for(size_t y = 0; y < rows; y++) {
			id_tries = 0;
			std::ostringstream sout;
			sout << "x" << x << "y" << y;
			current_cell = sout.str();
			std::pair<std::string, cControl*> p = parse();
			if(p.second == nullptr) return false;
			std::string anchor;
			auto pos = p.second->getPositioning(&anchor);
			if(!anchor.empty()) {
				// Relative positioning was requested. Assume it references another control within the same tilemap, and append the cell suffix.
				// TODO: Support relative anchors too?
				// TODO: Should it support anchoring to something outside the tilemap? Or is that too tricky?
				anchor.push_back('-');
				anchor += current_cell;
				p.second->setPositioning(anchor, pos.first, pos.second);
			}
		}
	}
	return true;
}

bool cTilemap::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	using namespace ticpp;
	if(content.Type() == TiXmlNode::ELEMENT) {
		auto& elem = dynamic_cast<Element&>(content);
		// There's a special case for LED groups...
		if(elem.Value() == "mapgroup") {
			cLedGroup* group = new cLedGroup(*this);
			Iterator<Attribute> attr;
			for(attr = attr.begin(&elem); attr != attr.end(); attr++) {
				if(!group->parseAttribute(*attr, tagName, fname))
					throw xBadAttr(elem.Value(), attr->Name(), attr->Row(), attr->Column(), fname);
			}
			auto id = elem.GetAttribute("name");
			if(id.empty()) id = cControl::generateRandomString();
			group->setName(id);
			controls.emplace(id, group);
			Iterator<Element> iter;
			for(iter = iter.begin(&elem); iter != iter.end(); iter++) {
				parseInstances([this, &iter, group]{
					// TODO: The parent should really be group, not this? But passing this is needed to make the tilemap's generateId be called...
					auto p = getDialog()->parse<cLed>(*iter, *this);
					group->addChoice(p.second, p.first);
					return p;
				});
			}
			return true;
		}
		return parseInstances([this, &elem, &fname]{
			std::string id;
			bool result = parseChildControl(elem, controls, id, fname);
			if(result) return *controls.find(id);
			static const std::pair<const std::string, cControl*> nil;
			return nil;
		});
	}
	return cContainer::parseContent(content, n, tagName, fname, text);
}

void cTilemap::validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) {
	cContainer::validatePostParse(who, fname, attrs, nodes);
	if(!attrs.count("rows")) throw xMissingAttr(who.Value(), "rows", who.Row(), who.Column(), fname);
	if(!attrs.count("cols")) throw xMissingAttr(who.Value(), "cols", who.Row(), who.Column(), fname);
}
