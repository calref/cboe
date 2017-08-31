/*
 *  message.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "message.hpp"
#include "mathutil.hpp"
#include "dialog.hpp"

extern sf::Texture bg_gworld;

void cTextMsg::setColour(sf::Color clr) throw(xUnsupportedProp) {
	color = clr;
}

void cTextMsg::setFormat(eFormat prop, short val) throw(xUnsupportedProp){
	switch(prop){
		case TXT_FRAME:
			drawFramed = val;
			break;
		case TXT_FRAMESTYLE:
			frameStyle = eFrameStyle(val);
			break;
		case TXT_SIZE:
			textSize = val;
			break;
		case TXT_FONT:
			if(val == FONT_DUNGEON) textFont = FONT_DUNGEON;
			else if(val == FONT_PLAIN) textFont = FONT_PLAIN;
			else if(val == FONT_MAIDWORD) textFont = FONT_MAIDWORD;
			else textFont = FONT_BOLD; // Defaults to bold if an invalid value is provided.
			break;
		case TXT_WRAP:
			throw xUnsupportedProp(prop);
	}
}

sf::Color cTextMsg::getColour() throw(xUnsupportedProp) {
	return color;
}

short cTextMsg::getFormat(eFormat prop) throw(xUnsupportedProp){
	switch(prop){
		case TXT_FRAME:
			return drawFramed;
		case TXT_FRAMESTYLE:
			return frameStyle;
		case TXT_SIZE:
			return textSize;
		case TXT_FONT:
			return textFont;
		case TXT_WRAP:
			throw xUnsupportedProp(prop);
	}
	return 0;
}

std::string cTextMsg::parse(ticpp::Element& who, std::string fname) {
	using namespace ticpp;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	std::string name, id;
	int width = 0, height = 0;
	bool foundTop = false, foundLeft = false; // top and left are required attributes
	rectangle frame;
	if(parent->getBg() == cDialog::BG_DARK)
		setColour(sf::Color::White);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&id);
		else if(name == "framed"){
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
		}else if(name == "font"){
			std::string val;
			attr->GetValue(&val);
			if(val == "dungeon")
				setFormat(TXT_FONT, FONT_DUNGEON);
			else if(val == "plain")
				setFormat(TXT_FONT, FONT_PLAIN);
			else if(val == "bold")
				setFormat(TXT_FONT, FONT_BOLD);
			else if(val == "maidenword")
				setFormat(TXT_FONT, FONT_MAIDWORD);
			else throw xBadVal("text",name,val,attr->Row(),attr->Column(),fname);
		}else if(name == "size"){
			std::string val;
			attr->GetValue(&val);
			if(val == "large")
				setFormat(TXT_SIZE, 12);
			else if(val == "small")
				setFormat(TXT_SIZE, 10);
			else if(val == "title")
				setFormat(TXT_SIZE, 18);
			else throw xBadVal("text",name,val,attr->Row(),attr->Column(),fname);
		}else if(name == "color" || name == "colour"){
			std::string val;
			attr->GetValue(&val);
			sf::Color clr;
			try{
				clr = parseColor(val);
			}catch(int){
				throw xBadVal("text",name,val,attr->Row(),attr->Column(),fname);
			}
			setColour(clr);
		}else if(name == "def-key"){
			std::string val;
			attr->GetValue(&val);
			try{
				attachKey(parseKey(val));
			}catch(int){
				throw xBadVal("text",name,val,attr->Row(),attr->Column(),fname);
			}
		}else if(name == "top"){
			attr->GetValue(&frame.top), foundTop = true;
		}else if(name == "left"){
			attr->GetValue(&frame.left), foundLeft = true;
		}else if(name == "width"){
			attr->GetValue(&width);
		}else if(name == "height"){
			attr->GetValue(&height);
//		}else if(name == "fromlist"){
//			attr->GetValue(&p.second->fromList);
		}else throw xBadAttr("text",name,attr->Row(),attr->Column(),fname);
	}
	if(!foundTop) throw xMissingAttr("text","top",who.Row(),who.Column(),fname);
	if(!foundLeft) throw xMissingAttr("text","left",who.Row(),who.Column(),fname);
	frame.right = frame.left + width;
	frame.bottom = frame.top + height;
	setBounds(frame);
	std::string content;
	for(node = node.begin(&who); node != node.end(); node++){
		std::string val;
		int type = node->Type();
		node->GetValue(&val);
		// TODO: De-magic the | character
		if(type == TiXmlNode::ELEMENT && val == "br") content += '|'; // because vertical bar is replaced by a newline when drawing strings
		else if(type == TiXmlNode::TEXT)
			content += dlogStringFilter(val);
		else if(type != TiXmlNode::COMMENT) {
			val = '<' + val + '>';
			throw xBadVal("text",xBadVal::CONTENT,content + val,node->Row(),node->Column(),fname);
		}
	}
	setText(content);
	return id;
}

cTextMsg::cTextMsg(cDialog& parent) :
	cControl(CTRL_TEXT,parent),
	drawFramed(false),
	textFont(FONT_BOLD),
	textSize(10),
	color(parent.getDefTextClr()),
	fromList("none") {}

cTextMsg::cTextMsg(sf::RenderWindow& parent) :
	cControl(CTRL_TEXT,parent),
	drawFramed(false),
	textFont(FONT_BOLD),
	textSize(10),
	color(cDialog::defaultBackground == cDialog::BG_DARK ? sf::Color::White : sf::Color::Black),
	fromList("none") {}

bool cTextMsg::isClickable(){
	return haveHandler(EVT_CLICK);
}

bool cTextMsg::isFocusable(){
	return false;
}

bool cTextMsg::isScrollable(){
	return false;
}

void cTextMsg::draw(){
	rectangle to_rect = frame;
	
	inWindow->setActive();
	
	if(visible){
		TextStyle style;
		style.font = textFont;
		style.pointSize = textSize;
		if(drawFramed) drawFrame(2,frameStyle);
		sf::Color draw_color = color;
		if(depressed){
			draw_color.r = 256 - draw_color.r;
			draw_color.g = 256 - draw_color.g;
			draw_color.b = 256 - draw_color.b;
		}
		style.colour = draw_color;
		if(to_rect.bottom - to_rect.top < 20) { // essentially, it's a single line
			style.lineHeight = 12;
			to_rect.left += 3;
			win_draw_string(*inWindow,to_rect,lbl,eTextMode::LEFT_BOTTOM,style);
		}else {
			style.lineHeight = textSize + 2;
			to_rect.inset(4,4);
			win_draw_string(*inWindow,to_rect,lbl,eTextMode::WRAP,style);
		}
	}
}

cTextMsg::~cTextMsg() {}
