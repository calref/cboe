
/*
 *  led.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 2020-12-21.
 *
 */

#include "led.hpp"

#include "dialogxml/dialogs/dialog.hpp"
#include "gfx/render_image.hpp"

#include "fileio/resmgr/res_image.hpp"

rectangle cLed::ledRects[3][2];

void cLed::init(){
	rectangle baseLed = {0,0,10,14};
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 2; j++){
			ledRects[i][j] = baseLed;
			ledRects[i][j].offset(i * 14, j * 10);
		}
}

cLed::cLed(iComponent& parent)
	: cButton(parent,CTRL_LED)
	, state(led_off)
	, textFont(FONT_BOLD)
{
	textSize = 10;
	type = BTN_LED;
	using namespace std::placeholders;
	attachEventHandler<EVT_CLICK>(std::bind(&cLed::defaultClickHandler, this, _1, _2, _3));
}

void cLed::defaultClickHandler(cDialog&, std::string, eKeyMod) {
	// simple state toggle
	switch(state){
		case led_red:
		case led_green:
			state = led_off;
			break;
		case led_off:
			state = led_red;
	}
}

void cLed::callHandler(event_fcn<EVT_CLICK>::type onClick, cDialog& me, std::string id, eKeyMod mods) {
	eLedState oldState = state;
	if(onClick) onClick(me,id,mods);
	if(!triggerFocusHandler(me,id, oldState != led_off)){
		state = oldState;
	}
}

bool cLed::manageFormat(eFormat prop, bool set, boost::any* val) {
	switch(prop) {
		case TXT_FONT:
			if(val) {
				if(set) textFont = boost::any_cast<eFont>(*val);
				else *val = textFont;
			}
			break;
		case TXT_SIZE:
			if(val) {
				if(set) textSize = boost::any_cast<short>(*val);
				else *val = textSize;
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

const int LED_TEXT_SPACE = 4; // Possibly could be 6

void cLed::draw(){
	rectangle from_rect, to_rect;
	
	getWindow().setActive(false);
	
	if(visible){
		TextStyle style;
		style.pointSize = textSize;
		style.lineHeight = textSize - 1;
		style.font = textFont;
		style.colour = textClr;
		from_rect = ledRects[state][depressed];
		to_rect = frame;
		int text_width = string_length(getText(), style);
		if(textOnRight){
			to_rect.right = to_rect.left + from_rect.width();
			to_rect.bottom = to_rect.top + from_rect.height();
			rect_draw_some_item(*ResMgr::graphics.get(buttons[btnGW[BTN_LED]]),from_rect,getWindow(),to_rect);
		}else{
			to_rect.right = text_width;
			win_draw_string(getWindow(),to_rect,getText(),wrapLabel ? eTextMode::WRAP : eTextMode::LEFT_TOP,style);
		}
		if(textOnRight){
			to_rect.left = frame.left + from_rect.width() + LED_TEXT_SPACE;
			to_rect.right = frame.right;
			win_draw_string(getWindow(),to_rect,getText(),wrapLabel ? eTextMode::WRAP : eTextMode::LEFT_TOP,style);
		}else{
			to_rect.left = frame.left + text_width + LED_TEXT_SPACE;
			to_rect.right = to_rect.left + from_rect.width();
			rect_draw_some_item(*ResMgr::graphics.get(buttons[btnGW[BTN_LED]]),from_rect,getWindow(),to_rect);
		}
	}

	getWindow().setActive();
}

cControl::storage_t cLed::store() const {
	storage_t storage = cButton::store();
	storage["led-state"] = getState();
	return storage;
}

void cLed::restore(storage_t to) {
	cButton::restore(to);
	if(to.find("led-state") != to.end())
		setState(boost::any_cast<eLedState>(to["led-state"]));
	else setState(led_off);
}

cLed::~cLed() {}

void cLed::setState(eLedState to){
	state = to;
}

eLedState cLed::getState() const {
	return state;
}

bool cLed::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	if(attr.Name() == "state"){
		std::string val = attr.Value();
		if(val == "red") setState(led_red);
		else if(val == "green") setState(led_green);
		else if(val == "off") setState(led_off);
		else throw xBadVal(tagName, attr.Name(), val, attr.Row(), attr.Column(), fname);
		return true;
	}else if(attr.Name() == "label-pos"){
		textOnRight = (attr.Value() == "right");
		return true;
	}
	return cButton::parseAttribute(attr, tagName, fname);
}

bool cLed::parseContent(ticpp::Node& content, int n, std::string tagName, std::string fname, std::string& text) {
	if(content.Type() == TiXmlNode::ELEMENT && content.Value() == "key")
		return false;
	return cButton::parseContent(content, n, tagName, fname, text);
}

location cLed::getPreferredSize() const {
	int width = ledRects[0][0].width();
	if(!getText().empty()){
		TextStyle style;
		style.pointSize = textSize;
		width = LED_TEXT_SPACE + ledRects[state][depressed].width() + string_length(getText(), style);
	}
	return {width, ledRects[0][0].height()};
}
