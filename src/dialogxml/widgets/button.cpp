
/*
 *  button.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "button.hpp"

#include <boost/algorithm/string.hpp>
#include "dialogxml/dialogs/dialog.hpp"
#include "gfx/render_image.hpp"
#include "gfx/render_text.hpp"

#include "fileio/resmgr/res_image.hpp"

//extern sf::Texture bg_gworld;

cButton::cButton(sf::RenderWindow& parent) :
	cControl(CTRL_BTN,parent),
	wrapLabel(false),
	type(BTN_REG),
	textSize(defaultTextSize(type)),
	textClr(sf::Color::Black),
	fromList("none") {
}

cButton::cButton(cDialog& parent) :
	cControl(CTRL_BTN,parent),
	wrapLabel(false),
	type(BTN_REG),
	textSize(defaultTextSize(type)),
	textClr(parent.getDefTextClr()),
	fromList("none") {
}

/* This constructor is only called for LEDs. */
cButton::cButton(cDialog& parent,eControlType t) :
	cControl(t,parent),
	fromList("none"),
	wrapLabel(false) {
}

short cButton::defaultTextSize(eBtnType type) {
	if(type == BTN_TINY) return 9;
	else if(type == BTN_PUSH) return 10;
	else return 12;
}

bool cButton::isClickable() const {
	return true;
}

bool cButton::isFocusable() const {
	return false;
}

bool cButton::isScrollable() const {
	return false;
}

const int TINY_TEXT_OFFSET = 18;

void cButton::draw(){
	rectangle from_rect, to_rect;
	
	inWindow->setActive(false);
	
	if(visible){
		TextStyle style;
		style.pointSize = textSize;
		from_rect = btnRects[type][depressed];
		to_rect = frame;
		if(type == BTN_TINY) {
			to_rect.right = to_rect.left + 14;
			to_rect.bottom = to_rect.top + 10;
		}
		rect_draw_some_item(*ResMgr::graphics.get(buttons[btnGW[type]]),from_rect,*inWindow,to_rect,sf::BlendAlpha);
		style.colour = sf::Color::Black;
		style.lineHeight = 8;
		eTextMode textMode = eTextMode::CENTRE;
		if(type == BTN_TINY) {
			textMode = wrapLabel ? eTextMode::WRAP : eTextMode::LEFT_TOP;
			to_rect.left += TINY_TEXT_OFFSET;
			style.colour = textClr;
		} else if(type == BTN_PUSH) {
			to_rect.top += 42;
			style.colour = textClr;
			int w = string_length(getText(), style);
			to_rect.inset((w - 30) / -2,0);
		}
		std::string label = getText(), keyDesc = getAttachedKeyDescription();
		for(size_t key_pos = label.find_first_of(KEY_PLACEHOLDER); key_pos < label.size(); key_pos = label.find_first_of(KEY_PLACEHOLDER)) {
			label.replace(key_pos, 1, keyDesc);
		}
		
		std::vector<std::string> forced_lines;
		boost::split(forced_lines, label, boost::is_any_of("|"));
		if(forced_lines.size() > 1){
			to_rect.offset(0, (-style.pointSize/2) * (forced_lines.size() - 1));
		}
		for(std::string line : forced_lines){
			win_draw_string(*inWindow,to_rect,line,textMode,style);
			to_rect.offset(0, style.pointSize);
		}
		// frame default button, to provide a visual cue that it's the default
		if(key.spec && key.k == key_enter) drawFrame(2,frameStyle);
	}
	inWindow->setActive();
}

bool cButton::manageFormat(eFormat prop, bool set, boost::any* val) {
	switch(prop) {
		case TXT_FRAME:
			if(val) {
				if(set) frameStyle = boost::any_cast<eFrameStyle>(*val);
				else *val = frameStyle;
			}
			break;
		case TXT_WRAP:
			if(val) {
				if(set) wrapLabel = boost::any_cast<bool>(*val);
				else *val = wrapLabel;
			}
			break;
		case TXT_COLOUR:
			if(val) {
				if(set) textClr = boost::any_cast<sf::Color>(*val);
				else *val = textClr;
			}
			break;
		default: return false;
	}
	return true;
}

bool cButton::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	std::string name = attr.Name();
	if(name == "type"){
		std::string val = attr.Value();
		if(val == "small") setBtnType(BTN_SM);
		else if(val == "regular") setBtnType(BTN_REG);
		else if(val == "large") setBtnType(BTN_LG);
		else if(val == "help") setBtnType(BTN_HELP);
		else if(val == "left") setBtnType(BTN_LEFT);
		else if(val == "right") setBtnType(BTN_RIGHT);
		else if(val == "up") setBtnType(BTN_UP);
		else if(val == "down") setBtnType(BTN_DOWN);
		else if(val == "tiny") setBtnType(BTN_TINY);
		else if(val == "done") setBtnType(BTN_DONE);
		else if(val == "tall") setBtnType(BTN_TALL);
		else if(val == "trait") setBtnType(BTN_TRAIT);
		else if(val == "push") setBtnType(BTN_PUSH);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}else if(name == "text-size"){
		std::string val = attr.Value();
		short size = std::stoi(val);
		textSize = size;
		return true;
	}else if(name == "def-key"){
		std::string val = attr.Value();
		try{
			attachKey(parseKey(val));
		}catch(int){
			throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		}
		return true;
	}
	return cControl::parseAttribute(attr, tagName, fname);
}

bool cButton::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	if(content.Type() == TiXmlNode::TEXT) {
		text += dlogStringFilter(content.Value());
		return true;
	} else if(content.Value() == "key") {
		text += KEY_PLACEHOLDER;
		return true;
	}
	return cControl::parseContent(content, n, tagName, fname, text);
}

static const std::set<eBtnType> labelledButtons{BTN_TINY, BTN_LED, BTN_PUSH};
	
void cButton::validatePostParse(ticpp::Element& elem, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>& elems) {
	cControl::validatePostParse(elem, fname, attrs, elems);
	if(getType() == CTRL_BTN && !attrs.count("type")) throw xMissingAttr(elem.Value(), "type", elem.Row(), elem.Column(), fname);
	if(type == BTN_PUSH && !getText().empty() && !attrs.count("width"))
		throw xMissingAttr(elem.Value(), "width", elem.Row(), elem.Column(), fname);

	// This needs to happen again after parsing, which sets preset buttons' text to empty
	initPreset();

	if(labelledButtons.count(type)) {
		if(!attrs.count("color") && !attrs.count("colour") && parent->getBg() == cDialog::BG_DARK)
			setColour(sf::Color::White);
	}
}

location cButton::getPreferredSize() const {
	int width = btnRects[type][0].width();
	if(type == BTN_TINY && !getText().empty()){
		TextStyle style;
		style.pointSize = textSize;
		width = TINY_TEXT_OFFSET + string_length(getText(), style);
	}
	return {width, btnRects[type][0].height()};
}

void cButton::recalcRect() {
	location bestSz = getPreferredSize();
	if(labelledButtons.count(type)) {
		if(frame.width() < bestSz.x) frame.width() = bestSz.x;
		if(frame.height() < bestSz.y) frame.height() = bestSz.y;
	} else {
		frame.width() = bestSz.x;
		frame.height() = bestSz.y;
	}
}

// Indices within the buttons array.
size_t cButton::btnGW[14] = {
	0, // BTN_SM
	1, // BTN_REG
	2, // BTN_LG
	4, // BTN_HELP
	1, // BTN_LEFT
	1, // BTN_RIGHT
	1, // BTN_UP
	1, // BTN_DOWN
	5, // BTN_TINY
	1, // BTN_DONE
	3, // BTN_TALL
	3, // BTN_TRAIT
	6, // BTN_PUSH
	5, // BTN_LED
};

rectangle cButton::btnRects[13][2];
std::string cButton::buttons[7] {
	"dlogbtnsm",
	"dlogbtnmed",
	"dlogbtnlg",
	"dlogbtntall",
	"dlogbtnhelp",
	"dlogbtnled",
	"dlgbtnred",
};

void cButton::init(){
	btnRects[BTN_SM][0] = {0,0,23,23};
	btnRects[BTN_REG][0] = {0,0,23,63};
	btnRects[BTN_LEFT][0] = {23,0,46,63};
	btnRects[BTN_RIGHT][0] = {46,0,69,63};
	btnRects[BTN_UP][0] = {69,0,92,63};
	btnRects[BTN_DOWN][0] = {92,0,115,63};
	btnRects[BTN_DONE][0] = {0,0,23,63};
	btnRects[BTN_LG][0] = {0,0,23,102};
	btnRects[BTN_HELP][0] = {0,0,13,16};
	btnRects[BTN_TINY][0] = {0,42,10,56};
	btnRects[BTN_TALL][0] = {0,0,40,63};
	btnRects[BTN_TRAIT][0] = {0,0,40,63};
	btnRects[BTN_PUSH][0] = {0,0,30,30};
	for(int j = 0; j <= 12; j++)
		btnRects[j][1] = btnRects[j][0];
	btnRects[BTN_SM][1].offset(23,0);
	btnRects[BTN_REG][1].offset(63,0);
	btnRects[BTN_LEFT][1].offset(63,0);
	btnRects[BTN_RIGHT][1].offset(63,0);
	btnRects[BTN_UP][1].offset(63,0);
	btnRects[BTN_DOWN][1].offset(63,0);
	btnRects[BTN_DONE][1].offset(63,0);
	btnRects[BTN_LG][1].offset(102,0);
	btnRects[BTN_HELP][1].offset(16,0);
	btnRects[BTN_TINY][1].offset(0,10);
	btnRects[BTN_TALL][1].offset(63,0);
	btnRects[BTN_TRAIT][1].offset(63,0);
	btnRects[BTN_PUSH][1].offset(30,0);
}

cButton::~cButton() {}

void cButton::setBtnType(eBtnType newType){
	if(type == BTN_LED || newType == BTN_LED) return; // can't change type
	type = newType;
	switch(newType) {
		case BTN_SM:
			frame.width() = 23;
			frame.height() = 23;
			break;
		case BTN_REG: case BTN_DONE:
		case BTN_LEFT: case BTN_RIGHT:
		case BTN_UP: case BTN_DOWN:
			frame.width() = 63;
			frame.height() = 23;
			break;
		case BTN_LG:
			frame.width() = 102;
			frame.height() = 23;
			break;
		case BTN_HELP:
			frame.width() = 16;
			frame.height() = 13;
			break;
		case BTN_TALL:
		case BTN_TRAIT:
			frame.width() = 63;
			frame.height() = 40;
			break;
		case BTN_PUSH:
			frame.width() = std::min<int>(frame.width(), 30);
			frame.height() = std::min<int>(frame.height(), 30);
			break;
		case BTN_TINY:
		case BTN_LED:
			frame.width() = std::min<int>(frame.width(), 14);
			frame.height() = std::min<int>(frame.height(), 10);
			break;
	}
	initPreset();
}

eBtnType cButton::getBtnType() const {
	return type;
}

void cButton::initPreset() {
	switch(type){
		case BTN_DONE:
			setText("Done");
			break;
		case BTN_TRAIT:
			setText("Race|& Traits");
			break;
	}
}
