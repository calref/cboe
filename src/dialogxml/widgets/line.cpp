//
//  line.cpp
//  BoE
//
//  Created by Celtic Minstrel on 2025-03-08.
//

#include "line.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "gfx/render_shapes.hpp"

bool cConnector::manageFormat(eFormat prop, bool set, boost::any* val) {
	switch(prop) {
		case TXT_SIZE:
			if(val) {
				if(set) thickness = boost::any_cast<short>(*val);
				else *val = thickness;
			}
			break;
		case TXT_COLOUR:
			if(val) {
				if(set) color = boost::any_cast<sf::Color>(*val);
				else *val = color;
			}
			break;
		default: return false;
	}
	return true;
}

bool cConnector::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	if(attr.Name() == "slope") {
		if(attr.Value() == "positive") flip = true;
		else if(attr.Value() == "negative") flip = false;
		else throw xBadVal(tagName, attr.Name(), attr.Value(), attr.Row(), attr.Column(), fname);
		return true;
	}
	return cControl::parseAttribute(attr, tagName, fname);
}

void cConnector::validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) {
	cControl::validatePostParse(who, fname, attrs, nodes);
	if(!attrs.count("color") && !attrs.count("colour") && getDialog()->getBg() == cDialog::BG_DARK)
		setColour(sf::Color::White);
	// Width or height can be zero, but not both
	if(frame.width() == 0 && frame.height() == 0)
		throw xBadVal("line", "width", "0", who.Row(), who.Column(), fname);
	// slope attribute forbidden if either width or height is zero
	if(attrs.count("slope") && (frame.width() == 0 || frame.height() == 0))
		throw xBadAttr("line", "slope", who.Row(), who.Column(), fname);
}

cConnector::cConnector(iComponent& parent)
	: cControl(CTRL_TEXT,parent)
	, color(parent.getDefTextClr())
{}

bool cConnector::isClickable() const {
	return false;
}

bool cConnector::isFocusable() const {
	return false;
}

bool cConnector::isScrollable() const {
	return false;
}

void cConnector::draw(){
	getWindow().setActive();
	
	if(visible){
		location first(flip ? frame.right : frame.left, frame.top);
		location end(flip ? frame.left : frame.right, frame.bottom);
		draw_line(getWindow(), first, end, thickness, color);
//		sf::ConvexShape convex;
//		convex.setPointCount(4);
//		convex.setFillColor(color);
//		convex.setOutlineThickness(0);
//		if(x2 - x1 == 0) {
//			convex.setPoint(0, sf::Vector2f(x1 - thickness / 2, y1));
//			convex.setPoint(1, sf::Vector2f(x1 + thickness / 2, y1));
//			convex.setPoint(2, sf::Vector2f(x2 + thickness / 2, y2));
//			convex.setPoint(3, sf::Vector2f(x2 - thickness / 2, y2));
//		} else {
//			float alpha = atan2(y2 - y1, x2 - x1);
//			convex.setPoint(0, sf::Vector2f(x1 - (thickness / 2) * sin(alpha), y1 + (thickness / 2) * cos(alpha)));
//			convex.setPoint(1, sf::Vector2f(x1 + (thickness / 2) * sin(alpha), y1 - (thickness / 2) * cos(alpha)));
//			convex.setPoint(2, sf::Vector2f(x2 + (thickness / 2) * sin(alpha), y2 - (thickness / 2) * cos(alpha)));
//			convex.setPoint(3, sf::Vector2f(x2 - (thickness / 2) * sin(alpha), y2 + (thickness / 2) * cos(alpha)));
//		}
//		getWindow().draw(convex);
	}
}

cConnector::~cConnector() {}
