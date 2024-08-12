/*
 *  message.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "message.hpp"
#include "mathutil.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include <numeric>

extern sf::Texture bg_gworld;

bool cTextMsg::manageFormat(eFormat prop, bool set, boost::any* val) {
	switch(prop) {
		case TXT_FRAME:
			if(val) {
				if(set) frameStyle = boost::any_cast<eFrameStyle>(*val);
				else *val = frameStyle;
			}
			break;
		case TXT_SIZE:
			if(val) {
				if(set) textSize = boost::any_cast<short>(*val);
				else *val = textSize;
			}
			break;
		case TXT_FONT:
			if(val) {
				if(set) textFont = boost::any_cast<eFont>(*val);
				else *val = textFont;
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

bool cTextMsg::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	if(attr.Name() == "def-key"){
		std::string val = attr.Value();
		try {
			attachKey(parseKey(val));
		} catch(int){ 
			throw xBadVal(tagName, attr.Name(), val, attr.Row(), attr.Column(), fname);
		}
		return true;
	} else if(attr.Name() == "underline") {
		std::string val = attr.Value();
		if(val == "true") underlined = true;
		else if(val == "false") underlined = false;
		else throw xBadVal(tagName, attr.Name(), val, attr.Row(), attr.Column(), fname);
		return true;
	}
	return cControl::parseAttribute(attr, tagName, fname);
}

bool cTextMsg::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	if(content.Type() == TiXmlNode::TEXT) {
		text += dlogStringFilter(content.Value());
		return true;
	} else if(content.Value() == "br") {
		// TODO: De-magic the | character
		text += '|'; // because vertical bar is replaced by a newline when drawing strings
		return true;
	} else if(content.Value() == "key") {
		auto elem = dynamic_cast<ticpp::Element&>(content);
		if(elem.HasAttribute("ref"))
			keyRefs.push_back(elem.GetAttribute("ref"));
		else keyRefs.push_back(boost::none);
		text += KEY_PLACEHOLDER;
		return true;
	}
	return cControl::parseContent(content, n, tagName, fname, text);
}

void cTextMsg::validatePostParse(ticpp::Element& who, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& nodes) {
	cControl::validatePostParse(who, fname, attrs, nodes);
	if(!attrs.count("color") && !attrs.count("colour") && parent->getBg() == cDialog::BG_DARK)
		setColour(sf::Color::White);
	if(attrs.count("width")) fixedWidth = true;
	if(attrs.count("height")) fixedHeight = true;
}

void cTextMsg::setFixed(bool w, bool h) {
	fixedWidth = w;
	fixedHeight = h;
}

void cTextMsg::recalcRect() {
	if(fixedWidth && fixedHeight) return;
	TextStyle style;
	style.font = textFont;
	style.pointSize = textSize;
	style.underline = underlined;
	style.lineHeight = textSize + 2;
	std::string test = lbl;
	size_t lines = 1, cur_line_chars = 0, max_line_chars = 0;
	// Substitute | with newlines for measuring
	for(auto& c : test) {
		if(c == '|') {
			c = '\n';
			lines++;
			max_line_chars = max(max_line_chars, cur_line_chars);
			cur_line_chars = 0;
		} else {
			cur_line_chars++;
		}
	}
	max_line_chars = max(max_line_chars, cur_line_chars);
	std::vector<hilite_t> hilites;
	std::vector<rectangle> rects;
	hilites.emplace_back(0,test.size());
	rectangle calc_rect = frame;
	if(lines == 1) {
		calc_rect.left += 3;
	} else {
		calc_rect.inset(4,4);
	}
	if(!fixedHeight) {
		// Fix the width and calculate the height
		calc_rect.height() = lines * style.lineHeight * 10;
	}
	if(!fixedWidth) {
		// Fix the height and calculate the width
		calc_rect.width() = 100 * max_line_chars;
	}
	sf::RenderTexture temp;
	temp.create(frame.width(), frame.height());
	rectangle test_rect = calc_rect;
	test_rect.offset(-test_rect.left, -test_rect.top);
	rects = draw_string_hilite(temp, test_rect, lbl, style, hilites, sf::Color::Black);
	if(rects.empty()) return;
	// Basically take the the union of the rects, and add 8 to its height or width
	rectangle combo = rects.back();
	if(rects.size() > 1) {
		combo = std::accumulate(rects.begin(), rects.end() - 1, combo, rectunion);
	}
	if(!fixedHeight) {
		calc_rect.height() = combo.height() + 8;
	}
	if(!fixedWidth) {
		calc_rect.width() = combo.width() + 16;
	}
	frame = calc_rect;
}

cTextMsg::cTextMsg(cDialog& parent) :
	cControl(CTRL_TEXT,parent),
	textFont(FONT_BOLD),
	textSize(10),
	color(parent.getDefTextClr()),
	fromList("none") {
	setFormat(TXT_FRAME, FRM_NONE);
}

cTextMsg::cTextMsg(sf::RenderWindow& parent) :
	cControl(CTRL_TEXT,parent),
	textFont(FONT_BOLD),
	textSize(10),
	color(cDialog::defaultBackground == cDialog::BG_DARK ? sf::Color::White : sf::Color::Black),
	fromList("none") {
	setFormat(TXT_FRAME, FRM_NONE);
}

bool cTextMsg::isClickable() const {
	return haveHandler(EVT_CLICK);
}

bool cTextMsg::isFocusable() const {
	return false;
}

bool cTextMsg::isScrollable() const {
	return false;
}

void cTextMsg::draw(){
	rectangle to_rect = frame;
	
	inWindow->setActive();
	
	if(visible){
		TextStyle style;
		style.font = textFont;
		style.pointSize = textSize;
		style.underline = underlined;
		drawFrame(2, frameStyle);
		sf::Color draw_color = color;
		if(depressed){
			draw_color.r = 256 - draw_color.r;
			draw_color.g = 256 - draw_color.g;
			draw_color.b = 256 - draw_color.b;
		}
		std::string msg = lbl;
		for(const auto& key : keyRefs) {
			size_t pos = msg.find_first_of(KEY_PLACEHOLDER);
			if(pos == std::string::npos) break;
			if(key && !parent->hasControl(*key)) continue;
			cControl& ctrl = key ? parent->getControl(*key) : *this;
			msg.replace(pos, 1, ctrl.getAttachedKeyDescription());
		}
		style.colour = draw_color;
		if(to_rect.bottom - to_rect.top < 20) { // essentially, it's a single line
			style.lineHeight = 12;
			to_rect.left += 3;
			win_draw_string(*inWindow,to_rect,msg,eTextMode::LEFT_BOTTOM,style);
		}else {
			style.lineHeight = textSize + 2;
			to_rect.inset(4,4);
			win_draw_string(*inWindow,to_rect,msg,eTextMode::WRAP,style);
		}
	}
}

cTextMsg::~cTextMsg() {}
