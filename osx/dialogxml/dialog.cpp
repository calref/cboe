/*
 *  dialog.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include <cmath>
#include <stdexcept>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include "dialog.hpp"
#include "graphtool.h"
#include "soundtool.h"
using namespace std;
using namespace ticpp;
#include "pict.hpp"
#include "button.hpp"
#include "field.hpp"
#include "message.hpp"
#include "scrollbar.hpp"
#include "winutil.h"
#include "mathutil.h"
#include "cursors.h"

// TODO: Would be nice if I could avoid depending on mainPtr
extern sf::RenderWindow mainPtr;
extern cursor_type current_cursor;

extern sf::Texture bg_gworld;
extern bool play_sounds;
const short cDialog::BG_DARK = 5, cDialog::BG_LIGHT = 16;
short cDialog::defaultBackground = cDialog::BG_DARK;
cDialog* cDialog::topWindow = nullptr;

static std::string generateRandomString(){
	// Not bothering to seed, because it doesn't actually matter if it's truly random.
	// Though, this will be called after srand() is called in main() anyway.
	int n_chars = rand() % 100;
	std::string s = "$";
	while(n_chars > 0){
		s += char(rand() % 96) + ' '; // was 223 ...
		n_chars--;
	}
	return s;
}

template<> pair<string,cPict*> cDialog::parse(Element& who /*pict*/){
	std::pair<std::string,cPict*> p;
	Iterator<Attribute> attr;
	std::string name;
	ePicType type;
	bool wide = false, tall = false, custom = false;
	bool foundTop = false, foundLeft = false, foundType = false, foundNum = false; // required attributes
	rectangle frame;
	int width = 0, height = 0;
	p.second = new cPict(*this);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&p.first);
		else if(name == "type"){
			std::string val;
			foundType = true;
			attr->GetValue(&val);
			if(val == "blank"){
				p.second->setPict(cPict::BLANK, PIC_TER);
				foundNum = true;
				continue;
			}else if(val == "ter")
				type = PIC_TER;
			else if(val == "teranim")
				type = PIC_TER_ANIM;
			else if(val == "monst")
				type = PIC_MONST;
			else if(val == "dlog")
				type = PIC_DLOG;
			else if(val == "talk")
				type = PIC_TALK;
			else if(val == "scen")
				type = PIC_SCEN;
			else if(val == "item")
				type = PIC_ITEM;
			else if(val == "pc")
				type = PIC_PC;
			else if(val == "field")
				type = PIC_FIELD;
			else if(val == "boom")
				type = PIC_BOOM;
			else if(val == "missile")
				type = PIC_MISSILE;
			else if(val == "full")
				type = PIC_FULL;
			else if(val == "map")
				type = PIC_TER_MAP;
			else if(val == "status")
				type = PIC_STATUS;
			else throw xBadVal("pict",name,val,attr->Row(),attr->Column(),fname);
			if(foundNum) {
				pic_num_t wasPic = p.second->getPicNum();
				p.second->setPict(wasPic, type);
			}
		}else if(name == "custom"){
			std::string val;
			attr->GetValue(&val);
			if(val == "true") custom = true;
		}else if(name == "size"){
			std::string val;
			attr->GetValue(&val);
			if(val == "wide") wide = true;
			else if(val == "tall") tall = true;
			else if(val == "large") wide = tall = true;
			else throw xBadVal("pict",name,val,attr->Row(),attr->Column(),fname);
		}else if(name == "def-key"){
			std::string val;
			attr->GetValue(&val);
			// TODO: The modifiers are now in key-mod, so this needs to be updated
			try{
				p.second->attachKey(parseKey(val));
			}catch(int){
				throw xBadVal("pict",name,val,attr->Row(),attr->Column(),fname);
			}
		}else if(name == "num"){
			pic_num_t newPic;
			attr->GetValue(&newPic), foundNum = true;
			if(foundType) p.second->setPict(newPic, type);
			else p.second->setPict(newPic);
		}else if(name == "top"){
			attr->GetValue(&frame.top), foundTop = true;
		}else if(name == "left"){
			attr->GetValue(&frame.left), foundLeft = true;
		}else if(name == "width"){
			attr->GetValue(&width);
		}else if(name == "height"){
			attr->GetValue(&height);
		}else throw xBadAttr("pict",name,attr->Row(),attr->Column(),fname);
	}
	if(!foundType) throw xMissingAttr("pict","type",who.Row(),who.Column(),fname);
	if(!foundNum) throw xMissingAttr("pict","num",who.Row(),who.Column(),fname);
	if(!foundTop) throw xMissingAttr("pict","top",who.Row(),who.Column(),fname);
	if(!foundLeft) throw xMissingAttr("pict","left",who.Row(),who.Column(),fname);
	if(wide || tall) {
		pic_num_t wasPic = p.second->getPicNum();
		if(wide && !tall && p.second->getPicType() == PIC_MONST) p.second->setPict(wasPic, PIC_MONST_WIDE);
		else if(!wide && tall && p.second->getPicType() == PIC_MONST) p.second->setPict(wasPic, PIC_MONST_TALL);
		else if(wide && tall){
			if(p.second->getPicType() == PIC_MONST) p.second->setPict(wasPic, PIC_MONST_LG);
			else if(p.second->getPicType() == PIC_SCEN) p.second->setPict(wasPic, PIC_SCEN_LG);
			else if(p.second->getPicType() == PIC_DLOG) p.second->setPict(wasPic, PIC_DLOG_LG);
		}
	}
	frame.right = frame.left;
	frame.bottom = frame.top;
	if(width > 0 || height > 0 || p.second->getPicType() == PIC_FULL){
		frame.right = frame.left + width;
		frame.bottom = frame.top + height;
	}
	p.second->setBounds(frame);
	pic_num_t wasPic = p.second->getPicNum();
	if(custom) {
		p.second->setPict(wasPic, p.second->getPicType() + PIC_CUSTOM);
	} else p.second->setPict(wasPic, p.second->getPicType());
	// The above line also sets the graphic's bounding rect, if necessary
	if(p.first == ""){
		do{
			p.first = generateRandomString();
		}while(controls.find(p.first) != controls.end());
	}
	return p;
}

class dlogStringFilter : public boost::iterator_facade<dlogStringFilter, string::value_type, forward_iterator_tag, string::value_type> {
	friend class boost::iterator_core_access;
	using value_type = std::string::value_type;
	using iter_type = std::string::iterator;
	bool found_nl;
	iter_type base;
	bool equal(const dlogStringFilter& other) const {
		return base == other.base;
	}
	value_type dereference() const {
		if(found_nl) return ' ';
		return *base;
	}
	void increment() {
		if(found_nl) {
			found_nl = false;
			return;
		}
		base++;
		while(*base == '\t' || *base == '\n' || *base == '\r') {
			found_nl = found_nl || *base != '\t';
			base++;
		}
	}
public:
	// TODO: This currently assumes that a string's end() iterator can be dereferenced (presumably to produce a null terminator).
	// This is a poor assumption (though it seems to work in practice).
	dlogStringFilter(iter_type base) : found_nl(false), base(base) {
		if(*base == '\t' || *base == '\n' || *base == '\r')
			increment();
	}
};

template<> pair<string,cTextMsg*> cDialog::parse(Element& who /*text*/){
	pair<string,cTextMsg*> p;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	string name;
	int width = 0, height = 0;
	bool foundTop = false, foundLeft = false; // top and left are required attributes
	rectangle frame;
	p.second = new cTextMsg(*this);
	if(bg == BG_DARK) p.second->setColour(sf::Color::White);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&p.first);
		else if(name == "framed"){
			std::string val;
			attr->GetValue(&val);
			if(val == "true") p.second->setFormat(TXT_FRAME, true);
		}else if(name == "font"){
			std::string val;
			attr->GetValue(&val);
			if(val == "dungeon")
				p.second->setFormat(TXT_FONT, FONT_DUNGEON);
			else if(val == "plain")
				p.second->setFormat(TXT_FONT, FONT_PLAIN);
			else if(val == "bold")
				p.second->setFormat(TXT_FONT, FONT_BOLD);
			else if(val == "maidenword")
				p.second->setFormat(TXT_FONT, FONT_MAIDWORD);
			else throw xBadVal("text",name,val,attr->Row(),attr->Column(),fname);
		}else if(name == "size"){
			std::string val;
			attr->GetValue(&val);
			if(val == "large")
				p.second->setFormat(TXT_SIZE, 12);
			else if(val == "small")
				p.second->setFormat(TXT_SIZE, 10);
			else if(val == "title")
				p.second->setFormat(TXT_SIZE, 18);
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
			p.second->setColour(clr);
		}else if(name == "def-key"){
			std::string val;
			attr->GetValue(&val);
			try{
				p.second->attachKey(parseKey(val));
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
		}else throw xBadAttr("pict",name,attr->Row(),attr->Column(),fname);
	}
	if(!foundTop) throw xMissingAttr("text","top",who.Row(),who.Column(),fname);
	if(!foundLeft) throw xMissingAttr("text","left",who.Row(),who.Column(),fname);
	frame.right = frame.left + width;
	frame.bottom = frame.top + height;
	p.second->setBounds(frame);
	string content;
	for(node = node.begin(&who); node != node.end(); node++){
		string val;
		int type = node->Type();
		node->GetValue(&val);
		// TODO: De-magic the | character
		if(type == TiXmlNode::ELEMENT && val == "br") content += '|'; // because vertical bar is replaced by a newline when drawing strings
		else if(type == TiXmlNode::TEXT)
			copy(dlogStringFilter(val.begin()), dlogStringFilter(val.end()), std::inserter(content, content.end()));
		else if(type != TiXmlNode::COMMENT) {
			val = '<' + val + '>';
			throw xBadVal("text",xBadVal::CONTENT,content + val,node->Row(),node->Column(),fname);
		}
	}
	p.second->setText(content);
	if(p.first == ""){
		do{
			p.first = generateRandomString();
		}while(controls.find(p.first) != controls.end());
	}
	return p;
}

/**
 * Parses an HTML-style colour string, recognizing three-digit hex, six-digit hex, and HTML colour names.
 */
sf::Color cDialog::parseColor(string what){
	sf::Color clr;
	if(what[0] == '#'){
		unsigned int r,g,b;
		if(sscanf(what.c_str(),"#%2x%2x%2x",&r,&g,&b) < 3)
			if(sscanf(what.c_str(),"#%1x%1x%1x",&r,&g,&b) < 3)
				throw -1;
		clr.r = r, clr.g = g, clr.b = b;
	}else if(what == "black")
		clr.r = 0x00, clr.g = 0x00, clr.b = 0x00;
	else if(what == "red")
		clr.r = 0xFF, clr.g = 0x00, clr.b = 0x00;
	else if(what == "lime")
		clr.r = 0x00, clr.g = 0xFF, clr.b = 0x00;
	else if(what == "blue")
		clr.r = 0x00, clr.g = 0x00, clr.b = 0xFF;
	else if(what == "yellow")
		clr.r = 0xFF, clr.g = 0xFF, clr.b = 0x00;
	else if(what == "aqua")
		clr.r = 0x00, clr.g = 0xFF, clr.b = 0xFF;
	else if(what == "fuchsia")
		clr.r = 0xFF, clr.g = 0x00, clr.b = 0xFF;
	else if(what == "white")
		clr.r = 0xFF, clr.g = 0xFF, clr.b = 0xFF;
	else if(what == "gray" || what == "grey")
		clr.r = 0x80, clr.g = 0x80, clr.b = 0x80;
	else if(what == "maroon")
		clr.r = 0x80, clr.g = 0x00, clr.b = 0x00;
	else if(what == "green")
		clr.r = 0x00, clr.g = 0x80, clr.b = 0x00;
	else if(what == "navy")
		clr.r = 0x00, clr.g = 0x00, clr.b = 0x80;
	else if(what == "olive")
		clr.r = 0x80, clr.g = 0x80, clr.b = 0x00;
	else if(what == "teal")
		clr.r = 0x00, clr.g = 0x80, clr.b = 0x80;
	else if(what == "purple")
		clr.r = 0x80, clr.g = 0x00, clr.b = 0x80;
	else if(what == "silver")
		clr.r = 0xC0, clr.g = 0xC0, clr.b = 0xC0;
	else throw -1;
	return clr;
}

template<> pair<string,cButton*> cDialog::parse(Element& who /*button*/){
	pair<string,cButton*> p;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	string name;
	int width = 0, height = 0;
	bool foundType = false, foundTop = false, foundLeft = false; // required attributes
	bool foundKey = false;
	std::string keyMod, keyMain;
	int keyModRow, keyModCol, keyMainRow, keyMainCol;
	rectangle frame;
	p.second = new cButton(this);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&p.first);
		else if(name == "wrap"){
			std::string val;
			attr->GetValue(&val);
			if(val == "true") p.second->setFormat(TXT_WRAP, true);
		}else if(name == "type"){
			std::string val;
			foundType = true;
			attr->GetValue(&val);
			if(val == "small")
				p.second->setBtnType(BTN_SM);
			else if(val == "regular")
				p.second->setBtnType(BTN_REG);
			else if(val == "large")
				p.second->setBtnType(BTN_LG);
			else if(val == "help")
				p.second->setBtnType(BTN_HELP);
			else if(val == "left")
				p.second->setBtnType(BTN_LEFT);
			else if(val == "right")
				p.second->setBtnType(BTN_RIGHT);
			else if(val == "up")
				p.second->setBtnType(BTN_UP);
			else if(val == "down")
				p.second->setBtnType(BTN_DOWN);
			else if(val == "tiny")
				p.second->setBtnType(BTN_TINY);
			else if(val == "done")
				p.second->setBtnType(BTN_DONE);
			else if(val == "tall")
				p.second->setBtnType(BTN_TALL);
			else if(val == "trait")
				p.second->setBtnType(BTN_TRAIT);
			else if(val == "push")
				p.second->setBtnType(BTN_PUSH);
		}else if(name == "def-key"){
			attr->GetValue(&keyMain);
			foundKey = true;
			keyMainRow = attr->Row();
			keyMainCol = attr->Column();
		}else if(name == "key-mod"){
			attr->GetValue(&keyMod);
			foundKey = true;
			keyModRow = attr->Row();
			keyModCol = attr->Column();
//		}else if(name == "fromlist"){
//			attr->GetValue(&p.second->fromList);
		}else if(name == "top"){
			attr->GetValue(&frame.top), foundTop = true;
		}else if(name == "left"){
			attr->GetValue(&frame.left), foundLeft = true;
		}else if(name == "width"){
			attr->GetValue(&width);
		}else if(name == "height"){
			attr->GetValue(&height);
		}else throw xBadAttr("button",name,attr->Row(),attr->Column(),fname);
	}
	if(bg == BG_DARK && p.second->getBtnType() == BTN_TINY) p.second->setColour(sf::Color::White);
	if(!foundType) throw xMissingAttr("button","type",who.Row(),who.Column(),fname);
	if(!foundTop) throw xMissingAttr("button","top",who.Row(),who.Column(),fname);
	if(!foundLeft) throw xMissingAttr("button","left",who.Row(),who.Column(),fname);
	if(foundKey) {
		cKey theKey;
		try{
			theKey = parseKey(keyMod + " " + keyMain);
		}catch(int){
			try {
				theKey = parseKey(keyMain);
			}catch(int){
				throw xBadVal("button","def-key",keyMain,keyMainRow,keyMainCol,fname);
			}
			throw xBadVal("button","key-mod",keyMod,keyModRow,keyModCol,fname);
		}
		p.second->attachKey(theKey);
	}
	if(width > 0 || height > 0) {
		// TODO: What if width is set but height isn't?
		frame.right = frame.left + width;
		frame.bottom = frame.top + height;
	}else switch(p.second->getBtnType()){
		case BTN_SM:
			frame.right = frame.left + 23;
			frame.bottom = frame.top + 23;
			break;
		case BTN_LG:
			frame.right = frame.left + 102;
			frame.bottom = frame.top + 23;
			break;
		case BTN_HELP:
			frame.right = frame.left + 16;
			frame.bottom = frame.top + 13;
			break;
		case BTN_TINY:
		case BTN_LED: // this should never happen
			frame.right = frame.left + 14;
			frame.bottom = frame.top + 10;
			break;
		case BTN_TALL:
		case BTN_TRAIT:
			frame.right = frame.left + 63;
			frame.bottom = frame.top + 40;
			break;
		case BTN_PUSH:
			frame.right = frame.left + 30;
			frame.bottom = frame.top + 30;
			break;
		default:
			frame.right = frame.left + 63;
			frame.bottom = frame.top + 23;
	}
	p.second->setBounds(frame);
	string content;
	for(node = node.begin(&who); node != node.end(); node++){
		string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::ELEMENT && val == "key"){
			// TODO: There's surely a better way to do this
			if(content.length() > 0) throw xBadVal("button",xBadVal::CONTENT,content + val,node->Row(),node->Column(),fname);
//			p.second->labelWithKey = true;
		}else if(type == TiXmlNode::TEXT)
			copy(dlogStringFilter(val.begin()), dlogStringFilter(val.end()), std::inserter(content, content.end()));
		else if(type != TiXmlNode::COMMENT) {
			val = '<' + val + '>';
			throw xBadVal("text",xBadVal::CONTENT,val,node->Row(),node->Column(),fname);
		}
	}
	p.second->setText(content);
	if(p.first == ""){
		do{
			p.first = generateRandomString();
		}while(controls.find(p.first) != controls.end());
	}
	return p;
}

cKey cDialog::parseKey(string what){
	cKey key;
	key.spec = false;
	key.c = 0;
	key.mod = mod_none;
	if(what == "none") return key;
	istringstream sin(what);
	string parts[4];
	sin >> parts[0] >> parts[1] >> parts[2] >> parts[3];
	for(int i = 0; i < 4; i++){
		if(parts[i] == "ctrl") key.mod += mod_ctrl;
		else if(parts[i] == "alt") key.mod += mod_alt;
		else if(parts[i] == "shift") key.mod += mod_shift;
		else if(parts[i] == "left") {
			key.spec = true;
			key.k = key_left;
			break;
		}else if(parts[i] == "right") {
			key.spec = true;
			key.k = key_right;
			break;
		}else if(parts[i] == "up") {
			key.spec = true;
			key.k = key_up;
			break;
		}else if(parts[i] == "down") {
			key.spec = true;
			key.k = key_down;
			break;
		}else if(parts[i] == "esc") {
			key.spec = true;
			key.k = key_esc;
			break;
		}else if(parts[i] == "enter" || parts[i] == "return") {
			key.spec = true;
			key.k = key_enter;
			break;
		}else if(parts[i] == "tab") {
			key.spec = true;
			key.k = key_tab;
			break;
		}else if(parts[i] == "help") {
			key.spec = true;
			key.k = key_help;
			break;
		}else if(parts[i] == "space") {
			key.c = ' ';
			break;
		}else if(parts[i].length() == 1) {
			key.c = parts[i][0];
			break;
		}else throw -1;
	}
	return key;
}

template<> pair<string,cLed*> cDialog::parse(Element& who /*LED*/){
	pair<string,cLed*> p;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	string name;
	int width = 0, height = 0;
	bool foundTop = false, foundLeft = false; // requireds
	rectangle frame;
	p.second = new cLed(this);
	if(bg == BG_DARK) p.second->setColour(sf::Color::White);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&p.first);
		else if(name == "state"){
			std::string val;
			attr->GetValue(&val);
			if(val == "red") p.second->setState(led_red);
			else if(val == "green") p.second->setState(led_green);
			else if(val == "off") p.second->setState(led_off);
			else throw xBadVal("led",name,val,attr->Row(),attr->Column(),fname);
//		}else if(name == "fromlist"){
//			attr->GetValue(&p.second->fromList);
		}else if(name == "font"){
			std::string val;
			attr->GetValue(&val);
			if(val == "dungeon")
				p.second->setFormat(TXT_FONT, FONT_DUNGEON);
			else if(val == "plain")
				p.second->setFormat(TXT_FONT, FONT_PLAIN);
			else if(val == "bold")
				p.second->setFormat(TXT_FONT, FONT_BOLD);
			else if(val == "maidenword")
				p.second->setFormat(TXT_FONT, FONT_MAIDWORD);
			else throw xBadVal("text",name,val,attr->Row(),attr->Column(),fname);
		}else if(name == "size"){
			std::string val;
			attr->GetValue(&val);
			if(val == "large")
				p.second->setFormat(TXT_SIZE, 12);
			else if(val == "small")
				p.second->setFormat(TXT_SIZE, 10);
			if(val == "title")
				p.second->setFormat(TXT_SIZE, 18);
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
			p.second->setColour(clr);
		}else if(name == "top"){
			attr->GetValue(&frame.top), foundTop = true;
		}else if(name == "left"){
			attr->GetValue(&frame.left), foundLeft = true;
		}else if(name == "width"){
			attr->GetValue(&width);
		}else if(name == "height"){
			attr->GetValue(&height);
		}else throw xBadAttr("button",name,attr->Row(),attr->Column(),fname);
	}
	if(!foundTop) throw xMissingAttr("led","top",who.Row(),who.Column(),fname);
	if(!foundLeft) throw xMissingAttr("led","left",who.Row(),who.Column(),fname);
	if(width > 0) {
		frame.right = frame.left + width;
	}else{
		frame.right = frame.left + 14;
	}
	if(height > 0) {
		frame.bottom = frame.top + height;
	}else{
		frame.bottom = frame.top + 10;
	}
	p.second->setBounds(frame);
	string content;
	for(node = node.begin(&who); node != node.end(); node++){
		string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::TEXT)
			copy(dlogStringFilter(val.begin()), dlogStringFilter(val.end()), std::inserter(content, content.end()));
		else if(type != TiXmlNode::COMMENT) {
			val = '<' + val + '>';
			throw xBadVal("text",xBadVal::CONTENT,content + val,node->Row(),node->Column(),fname);
		}
	}
	p.second->setText(content);
	if(p.first == ""){
		do{
			p.first = generateRandomString();
		}while(controls.find(p.first) != controls.end());
	}
	return p;
}

template<> pair<string,cLedGroup*> cDialog::parse(Element& who /*group*/){
	pair<string,cLedGroup*> p;
	Iterator<Attribute> attr;
	Iterator<Element> node;
	string name;
	p.second = new cLedGroup(this);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&p.first);
//		else if(name == "fromlist")
//			attr->GetValue(&p.second->fromList);
		else throw xBadAttr("button",name,attr->Row(),attr->Column(),fname);
	}
	string content;
	for(node = node.begin(&who); node != node.end(); node++){
		string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::ELEMENT && val == "led"){
			auto led = parse<cLed>(*node);
			p.second->addChoice(led.second, led.first);
		}else if(type != TiXmlNode::COMMENT) {
			val = '<' + val + '>';
			throw xBadVal("text",xBadVal::CONTENT, content + val,node->Row(),node->Column(),fname);
		}
	}
	p.second->setText(content);
	p.second->recalcRect();
	if(p.first == ""){
		do{
			p.first = generateRandomString();
		}while(controls.find(p.first) != controls.end());
	}
	return p;
}

template<> pair<string,cTextField*> cDialog::parse(Element& who /*field*/){
	pair<string,cTextField*> p;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	string name;
	int width = 0, height = 0;
	bool foundTop = false, foundLeft = false; // requireds
	rectangle frame;
	p.second = new cTextField(this);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&p.first);
		else if(name == "type"){
			std::string val;
			attr->GetValue(&val);
			if(val == "int")
				p.second->setInputType(FLD_INT);
			else if(val == "uint")
				p.second->setInputType(FLD_UINT);
			else if(val == "real")
				p.second->setInputType(FLD_REAL);
			else if(val == "text")
				p.second->setInputType(FLD_TEXT);
			else throw xBadVal("field",name,val,attr->Row(),attr->Column(),fname);
		}else if(name == "top"){
			attr->GetValue(&frame.top), foundTop = true;
		}else if(name == "left"){
			attr->GetValue(&frame.left), foundLeft = true;
		}else if(name == "width"){
			attr->GetValue(&width);
		}else if(name == "height"){
			attr->GetValue(&height);
		}else if(name == "tab-order"){
			attr->GetValue(&p.second->tabOrder);
		}else throw xBadAttr("button",name,attr->Row(),attr->Column(),fname);
	}
	if(!foundTop) throw xMissingAttr("field","top",attr->Row(),attr->Column(),fname);
	if(!foundLeft) throw xMissingAttr("field","left",attr->Row(),attr->Column(),fname);
	frame.right = frame.left + width;
	frame.bottom = frame.top + height;
	p.second->setBounds(frame);
	string content;
	for(node = node.begin(&who); node != node.end(); node++){
		string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::TEXT)
			copy(dlogStringFilter(val.begin()), dlogStringFilter(val.end()), std::inserter(content, content.end()));
		else if(type != TiXmlNode::COMMENT) {
			val = '<' + val + '>';
			throw xBadVal("text",xBadVal::CONTENT,val,node->Row(),node->Column(),fname);
		}
	}
	p.second->setText(content);
	if(p.first == ""){
		do{
			p.first = generateRandomString();
		}while(controls.find(p.first) != controls.end());
	}
	return p;
}

cDialog::cDialog(cDialog* p) : parent(p) {}

cDialog::cDialog(std::string path, cDialog* p) : parent(p) {
	loadFromFile(path);
}

extern fs::path progDir;
void cDialog::loadFromFile(std::string path){
	bg = defaultBackground;
	fname = path;
	fs::path cPath = progDir/"data"/"dialogs"/path;
	try{
		printf("Loading dialog from: %s\n", cPath.c_str());
		TiXmlBase::SetCondenseWhiteSpace(false);
		Document xml(cPath.c_str());
		xml.LoadFile();
		
		Iterator<Attribute> attr;
		Iterator<Element> node;
		string type, name, val;
		
		xml.FirstChildElement()->GetValue(&type);
		if(type != "dialog") throw xBadNode(type,xml.FirstChildElement()->Row(),xml.FirstChildElement()->Column(),fname);
		for(attr = attr.begin(xml.FirstChildElement()); attr != attr.end(); attr++){
			attr->GetName(&name);
			attr->GetValue(&val);
			if(name == "skin"){
				if(val == "light") bg = BG_LIGHT;
				else if(val == "dark") bg = BG_DARK;
				else{
					istringstream sin(val);
					sin >> bg;
					if(sin.fail()) throw xBadVal(type,name,val,attr->Row(),attr->Column(),fname);
				}
			}else if(name == "fore"){
				sf::Color clr;
				try{
					clr = parseColor(val);
				}catch(int){
					throw xBadVal("text",name,val,attr->Row(),attr->Column(),fname);
				}
			defTextClr = clr;
			} else if(name == "defbtn") {
				defaultButton = val;
			}else if(name != "debug")
				throw xBadAttr(type,name,attr->Row(),attr->Column(),fname);
		}
		
		vector<int> specificTabs, reverseTabs;
		for(node = node.begin(xml.FirstChildElement()); node != node.end(); node++){
			node->GetValue(&type);
			// Yes, I'm using insert instead of [] to add elements to the map.
			// In this situation, it's actually easier that way; the reason being, the
			// map key is obtained from the name attribute of each element.
			if(type == "field") {
				auto field = parse<cTextField>(*node);
				controls.insert(field);
				tabOrder.push_back(field);
				if(field.second->tabOrder > 0)
					specificTabs.push_back(field.second->tabOrder);
				else if(field.second->tabOrder < 0)
					reverseTabs.push_back(field.second->tabOrder);
			} else if(type == "text")
				controls.insert(parse<cTextMsg>(*node));
			else if(type == "pict")
				controls.insert(parse<cPict>(*node));
			else if(type == "button")
				controls.insert(parse<cButton>(*node));
			else if(type == "led")
				controls.insert(parse<cLed>(*node));
			else if(type == "group")
				controls.insert(parse<cLedGroup>(*node));
			else throw xBadNode(type,node->Row(),node->Column(),fname);
		}
		// Sort by tab order
		// First, fill any gaps that might have been left, using ones that had no specific tab order
		// Of course, if there are not enough without a specific tab order, there could still be gaps
		using fld_t = decltype(tabOrder)::value_type;
		auto noTabOrder = [](fld_t x) {return x.second->tabOrder == 0;};
		if(!specificTabs.empty()) {
			int max = *max_element(specificTabs.begin(), specificTabs.end());
			for(int i = 1; i < max; i++) {
				auto check = find(specificTabs.begin(), specificTabs.end(), i);
				if(check != specificTabs.end()) continue;
				auto change = find_if(tabOrder.begin(), tabOrder.end(), noTabOrder);
				if(change == tabOrder.end()) break;
				change->second->tabOrder = i;
			}
		}
		if(!reverseTabs.empty()) {
			int max = -*min_element(reverseTabs.begin(), reverseTabs.end());
			for(int i = 1; i < max; i++) {
				auto check = find(reverseTabs.begin(), reverseTabs.end(), -i);
				if(check != reverseTabs.end()) continue;
				auto change = find_if(tabOrder.begin(), tabOrder.end(), noTabOrder);
				if(change == tabOrder.end()) break;
				change->second->tabOrder = -i;
			}
		}
		// Then, sort - first, positive tab order ascending; then zeros; then negative tab order descending.
		stable_sort(tabOrder.begin(), tabOrder.end(), [](fld_t a, fld_t b) -> bool {
			bool a_neg = a.second->tabOrder < 0, b_neg = b.second->tabOrder < 0;
			if(a_neg && !b_neg) return false;
			else if(!a_neg && b_neg) return true;
			bool a_pos = a.second->tabOrder > 0, b_pos = b.second->tabOrder > 0;
			if(a_pos && !b_pos) return true;
			else if(!a_pos && b_pos) return false;
			return a.second->tabOrder < b.second->tabOrder;
		});
	} catch(Exception& x){ // XML processing exception
		printf("%s",x.what());
		exit(1);
	} catch(xBadVal& x){ // Invalid value for an attribute
		printf("%s",x.what());
		exit(1);
	} catch(xBadAttr& x){ // Invalid attribute for an element
		printf("%s",x.what());
		exit(1);
	} catch(xBadNode& x){ // Invalid element
		printf("%s",x.what());
		exit(1);
	} catch(xMissingAttr& x){ // Invalid element
		printf("%s",x.what());
		exit(1);
	}
	dialogNotToast = true;
	if(bg == BG_DARK) defTextClr = sf::Color::White;
	// now calculate window rect
	winRect = rectangle();
	recalcRect();
	ctrlIter iter = controls.begin();
	currentFocus = "";
	while(iter != controls.end()){
		if(typeid(iter->second) == typeid(cTextField*)){
			if(currentFocus.empty()) currentFocus = iter->first;
			break;
		}
		iter++;
	}
}

void cDialog::recalcRect(){
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		printf("%s \"%s\"\n",typeid(*(iter->second)).name(),iter->first.c_str());
		rectangle frame = iter->second->getBounds();
		if(frame.right > winRect.right)
			winRect.right = frame.right;
		if(frame.bottom > winRect.bottom)
			winRect.bottom = frame.bottom;
		iter++;
	}
	winRect.right += 6;
	winRect.bottom += 6;
}

void cDialog::init(){
	cButton::init();
	cLed::init();
	cPict::init();
	cScrollbar::init();
}

cDialog::~cDialog(){
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		delete iter->second;
		iter++;
	}
	win.close();
}

bool cDialog::add(cControl* what, rectangle ctrl_frame, std::string key){
	// First make sure the key is not already present.
	// If it is, we can't add the control, so return false.
	if(controls.find(key) != controls.end()) return false;
	what->setBounds(ctrl_frame);
	controls.insert(std::make_pair(key,what));
	return true;
}

bool cDialog::remove(std::string key){
	ctrlIter ctrl = controls.find(key);
	if(ctrl == controls.end()) return false;
	delete ctrl->second;
	controls.erase(ctrl);
	return true;
}

bool cDialog::sendInput(cKey key) {
	if(topWindow == nullptr) return false;
	std::string field = topWindow->currentFocus;
	if(field.empty()) return true;
	dynamic_cast<cTextField*>(topWindow->controls[field])->handleInput(key);
	return true;
}

void cDialog::run(){
	cDialog* formerTop = topWindow;
	cursor_type former_curs = current_cursor;
	set_cursor(sword_curs);
	using kb = sf::Keyboard;
	kb::Key k;
	cKey key, pendingKey;
	sf::Event currentEvent;
	std::string itemHit = "";
	dialogNotToast = true;
	// Focus the first text field, if there is one
	if(!tabOrder.empty()) {
		tabOrder[0].second->triggerFocusHandler(*this, tabOrder[0].first, false);
		currentFocus = tabOrder[0].first;
	}
	// Sometimes it seems like the Cocoa menu handling clobbers the active rendering context.
	// For whatever reason, delaying 100 milliseconds appears to fix this.
	sf::sleep(sf::milliseconds(100));
	win.create(sf::VideoMode(winRect.width(), winRect.height()), "Dialog", sf::Style::Titlebar);
	draw();
	win.setVisible(true);
	makeFrontWindow(parent ? parent-> win : mainPtr);
	makeFrontWindow(win);
	ModalSession dlog(win);
	animTimer.restart();
	while(dialogNotToast){
		draw();
		dlog.pumpEvents();
		if(!win.pollEvent(currentEvent)) continue;
		location where;
		switch(currentEvent.type){
			case sf::Event::KeyPressed:
				k = currentEvent.key.code;
				switch(k){
					case kb::Up:
						key.spec = true;
						key.k = key_up;
						break;
					case kb::Right:
						key.spec = true;
						key.k = key_right;
						break;
					case kb::Left:
						key.spec = true;
						key.k = key_left;
						break;
					case kb::Down:
						key.spec = true;
						key.k = key_down;
						break;
					case kb::Escape:
						key.spec = true;
						key.k = key_esc;
						break;
					case kb::Return: // TODO: Also enter (keypad)
						key.spec = true;
						key.k = key_enter;
						break;
					case kb::BackSpace:
						key.spec = true;
						key.k = key_bsp;
						break;
					case kb::Delete:
						key.spec = true;
						key.k = key_del;
						break;
					case kb::Tab:
						key.spec = true;
						key.k = key_tab;
						break;
					case kb::Insert:
						key.spec = true;
						key.k = key_insert;
						break;
					case kb::F1:
						key.spec = true;
						key.k = key_help;
						break;
					case kb::Home:
						key.spec = true;
						key.k = key_home;
						break;
					case kb::End:
						key.spec = true;
						key.k = key_end;
						break;
					case kb::PageUp:
						key.spec = true;
						key.k = key_pgup;
						break;
					case kb::PageDown:
						key.spec = true;
						key.k = key_pgdn;
						break;
					case kb::LShift:
					case kb::RShift:
					case kb::LAlt:
					case kb::RAlt:
					case kb::LControl:
					case kb::RControl:
					case kb::LSystem:
					case kb::RSystem:
						continue;
					default:
						key.spec = false;
						key.c = keyToChar(k, false);
						break;
				}
				key.mod = mod_none;
				if(currentEvent.key.*systemKey)
					key.mod += mod_ctrl;
				if(currentEvent.key.shift) key.mod += mod_shift;
				if(currentEvent.key.alt) key.mod += mod_alt;
				itemHit = process_keystroke(key);
				if(!itemHit.empty())
					where = controls[itemHit]->getBounds().centre();
				// Now check for focused fields.
				if(currentFocus.empty()) break;
				// If it's a tab, handle tab order
				if(key.spec && key.k == key_tab){
					// Could use key.mod, but this is slightly easier.
					if(currentEvent.key.shift)
						handleTabOrder(currentFocus, tabOrder.rbegin(), tabOrder.rend());
					else handleTabOrder(currentFocus, tabOrder.begin(), tabOrder.end());
				} else {
					// If it's a character key, and the system key (control/command) is not pressed,
					// we have an upcoming TextEntered event which contains more information.
					// Otherwise, handle it right away. But never handle enter or escape.
					if((key.spec && key.k != key_enter && key.k != key_esc) || mod_contains(key.mod, mod_ctrl))
						dynamic_cast<cTextField*>(controls[currentFocus])->handleInput(key);
					pendingKey = key;
					if(key.k != key_enter && key.k != key_esc) itemHit = "";
				}
				break;
			case sf::Event::TextEntered:
				if(!pendingKey.spec && !currentFocus.empty()) {
					pendingKey.c = currentEvent.text.unicode;
					dynamic_cast<cTextField*>(controls[currentFocus])->handleInput(pendingKey);
				}
				break;
			case sf::Event::MouseButtonPressed:
				key.mod = mod_none;
				if(kb::isKeyPressed(kb::LControl)) key.mod += mod_ctrl;
				if(kb::isKeyPressed(kb::RControl)) key.mod += mod_ctrl;
				if(kb::isKeyPressed(kb::LSystem)) key.mod += mod_ctrl;
				if(kb::isKeyPressed(kb::RSystem)) key.mod += mod_ctrl;
				if(kb::isKeyPressed(kb::LAlt)) key.mod += mod_alt;
				if(kb::isKeyPressed(kb::RAlt)) key.mod += mod_alt;
				if(kb::isKeyPressed(kb::LShift)) key.mod += mod_shift;
				if(kb::isKeyPressed(kb::RShift)) key.mod += mod_shift;
				where = {currentEvent.mouseButton.x, currentEvent.mouseButton.y};
				itemHit = process_click(where);
				break;
			case sf::Event::MouseMoved:
				set_cursor(sword_curs);
				for(auto& ctrl : controls) {
					if(ctrl.second->getType() == CTRL_FIELD && ctrl.second->getBounds().contains(currentEvent.mouseMove.x, currentEvent.mouseMove.y)) {
						set_cursor(text_curs);
						break;
					}
				}
				break;
			default: // To silence warning of unhandled enum values
				break;
		}
		if(itemHit.empty()) continue;;
		ctrlIter ctrl = controls.find(itemHit);
		// TODO: Should it do something with the boolean return value?
		if(ctrl != controls.end()) ctrl->second->triggerClickHandler(*this,itemHit,key.mod);
		itemHit.clear();
	}
	win.setVisible(false);
	// TODO: The introduction of the static topWindow means I may be able to use this instead of parent->win; do I still need parent?
	sf::RenderWindow* parentWin = &(parent ? parent->win : mainPtr);
	while(parentWin->pollEvent(currentEvent));
	set_cursor(former_curs);
	topWindow = formerTop;
}

template<typename Iter> void cDialog::handleTabOrder(string& itemHit, Iter begin, Iter end) {
	auto cur = find_if(begin, end, [&itemHit](pair<string,cTextField*>& a) {
		return a.first == itemHit;
	});
	if(cur == end) return; // Unlikely, but let's be safe
	if(!cur->second->triggerFocusHandler(*this,itemHit,true)) return;
	string wasFocus = currentFocus;
	auto iter = std::next(cur);
	if(iter == end) iter = begin;
	while(iter != cur){
		// If tab order is explicitly specified for all fields, gaps are possible
		if(iter->second == nullptr) continue;
		if(iter->second->getType() == CTRL_FIELD){
			if(iter->second->triggerFocusHandler(*this,iter->first,false)){
				currentFocus = iter->first;
			}
			break;
		}
		iter++;
		if(iter == end) iter = begin;
	}
	if(iter == cur) // no focus change occured!
		currentFocus = wasFocus; // TODO: Surely something should happen here?
}

void cDialog::setBg(short n){
	bg = n;
}

short cDialog::getBg() {
	return bg;
}

void cDialog::setDefBtn(std::string defBtn) {
	defaultButton = defBtn;
}

void cDialog::setDefTextClr(sf::Color clr){
	defTextClr = clr;
}

sf::Color cDialog::getDefTextClr() {
	return defTextClr;
}

bool cDialog::toast(bool triggerFocus){
	if(triggerFocus && !currentFocus.empty()) {
		if(!this->getControl(currentFocus).triggerFocusHandler(*this, currentFocus, true)) return false;
	}
	dialogNotToast = false;
	return true;
}

void cDialog::untoast() {
	dialogNotToast = true;
}

bool cDialog::setFocus(cTextField* newFocus, bool force) {
	// TODO: Should check that there IS a currently focused field (which might not be the case if there are no fields at all).
	if(!force) {
		if(!this->getControl(currentFocus).triggerFocusHandler(*this, currentFocus, true)) return false;
	}
	auto iter = find_if(controls.begin(), controls.end(), [newFocus](std::pair<const std::string, cControl*> p){
		return p.second == newFocus;
	});
	if(iter == controls.end()) return false;
	if(!force && !newFocus->triggerFocusHandler(*this, iter->first, false)) return false;
	currentFocus = iter->first;
	return true;
}

void cDialog::attachClickHandlers(click_callback_t handler, std::vector<std::string> controls) {
	cDialog& me = *this;
	for(std::string control : controls) {
		me[control].attachClickHandler(handler);
	}
}

void cDialog::attachFocusHandlers(focus_callback_t handler, std::vector<std::string> controls) {
	cDialog& me = *this;
	for(std::string control : controls) {
		me[control].attachFocusHandler(handler);
	}
}

bool cDialog::addLabelFor(std::string key, std::string label, eLabelPos where, short offset, bool bold) {
	cControl& ctrl = this->getControl(key);
	key += "-label";
	rectangle labelRect = ctrl.getBounds();
	switch(where) {
		case LABEL_LEFT:
			labelRect.right = labelRect.left;
			labelRect.left -= 2 * offset;
			break;
		case LABEL_ABOVE:
			labelRect.bottom = labelRect.top;
			labelRect.top -= 2 * offset;
			break;
		case LABEL_RIGHT:
			labelRect.left = labelRect.right;
			labelRect.right += 2 * offset;
			break;
		case LABEL_BELOW:
			labelRect.top = labelRect.bottom;
			labelRect.bottom += 2 * offset;
			break;
	}
	if(labelRect.height() < 14){
		int expand = (14 - labelRect.height()) / 2 + 1;
		labelRect.inset(0, -expand);
	} else labelRect.offset(0, labelRect.height() / 6);
	cControl* labelCtrl;
	auto iter = controls.find(key);
	if(iter != controls.end()) {
		labelCtrl = iter->second;
	} else {
		labelCtrl = new cTextMsg(*this);
	}
	labelCtrl->setText(label);
	labelCtrl->setFormat(TXT_FONT, bold ? FONT_BOLD : FONT_PLAIN);
	if(bg == BG_DARK && dynamic_cast<cButton*>(&ctrl) != NULL)
		labelCtrl->setColour(defTextClr);
	else labelCtrl->setColour(ctrl.getColour());
	ctrl.setLabelCtrl(labelCtrl);
	return add(labelCtrl, labelRect, key);
}

std::string cDialog::process_keystroke(cKey keyHit){
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		if(iter->second->isVisible() && iter->second->isClickable() && iter->second->getAttachedKey() == keyHit){
			iter->second->setActive(true);
			draw();
			if(play_sounds) {
				if(typeid(iter->second) == typeid(cLed*))
					play_sound(34);
				else play_sound(37);
				sf::sleep(time_in_ticks(6));
			}
			else sf::sleep(time_in_ticks(14));
			iter->second->setActive(false);
			draw();
			sf::sleep(sf::milliseconds(8));
			return iter->first;
		}
		iter++;
	}
	// If you get an escape and it isn't processed, make it an enter.
	if(keyHit.spec && keyHit.k == key_esc){
		keyHit.k = key_enter;
		return process_keystroke(keyHit);
	}
	// If nothing was hit and the key was enter, return the default button (if any)
	if(keyHit.spec && keyHit.k == key_enter)
		return defaultButton;
	return "";
}

std::string cDialog::process_click(location where){
	// TODO: Return list of all controls whose bounding rect contains the clicked point.
	// Then the return value of the click handler can mean "Don't pass this event on to other things below me".
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		if(iter->second->isVisible() && iter->second->isClickable() && where.in(iter->second->getBounds())){
			if(iter->second->handleClick(where))
				return iter->first;
			else return "";
		}
		iter++;
	}
	return "";
}

xBadNode::xBadNode(std::string t, int r, int c, std::string dlg) throw() :
	type(t),
	row(r),
	col(c),
	msg(NULL),
	dlg(dlg) {}

const char* xBadNode::what() throw() {
	if(msg == NULL){
		char* s = new (nothrow) char[200];
		if(s == NULL){
			printf("Allocation of memory for error message failed, bailing out...");
			abort();
		}
		sprintf(s,"XML Parse Error: Unknown element %s encountered (dialog %s, line %d, column %d).",type.c_str(),dlg.c_str(),row,col);
		msg = s;
	}
	return msg;
}

xBadNode::~xBadNode() throw(){
	if(msg != NULL) delete msg;
}

xBadAttr::xBadAttr(std::string t, std::string n, int r, int c, std::string dlg) throw() :
	type(t),
	name(n),
	row(r),
	col(c),
	msg(NULL),
	dlg(dlg) {}

const char* xBadAttr::what() throw() {
	if(msg == NULL){
		char* s = new (nothrow) char[200];
		if(s == NULL){
			printf("Allocation of memory for error message failed, bailing out...");
			abort();
		}
		sprintf(s,"XML Parse Error: Unknown attribute %s encountered on element %s (dialog %s, line %d, column %d).",name.c_str(),type.c_str(),dlg.c_str(),row,col);
		msg = s;
	}
	return msg;
}

xBadAttr::~xBadAttr() throw(){
	if(msg != NULL) delete msg;
}

xMissingAttr::xMissingAttr(std::string t, std::string n, int r, int c, std::string dlg) throw() :
	type(t),
	name(n),
	row(r),
	col(c),
	msg(NULL),
	dlg(dlg) {}

const char* xMissingAttr::what() throw() {
	if(msg == NULL){
		char* s = new (nothrow) char[200];
		if(s == NULL){
			printf("Allocation of memory for error message failed, bailing out...");
			abort();
		}
		sprintf(s,"XML Parse Error: Required attribute %s missing on element %s (dialog %s, line %d, column %d).",name.c_str(),type.c_str(),dlg.c_str(),row,col);
		msg = s;
	}
	return msg;
}

xMissingAttr::~xMissingAttr() throw(){
	if(msg != NULL) delete msg;
}

xBadVal::xBadVal(std::string t, std::string n, std::string v, int r, int c, std::string dlg) throw() :
	type(t),
	name(n),
	val(v),
	row(r),
	col(c),
	msg(NULL),
	dlg(dlg) {}

const char* xBadVal::what() throw() {
	if(msg == NULL){
		char* s = new (nothrow) char[200];
		if(s == NULL){
			printf("Allocation of memory for error message failed, bailing out...");
			abort();
		}
		sprintf(s,"XML Parse Error: Invalid value %s for attribute %s encountered on element %s (dialog %s, line %d, column %d).",val.c_str(),name.c_str(),type.c_str(),dlg.c_str(),row,col);
		msg = s;
	}
	return msg;
}

xBadVal::~xBadVal() throw(){
	if(msg != NULL) delete msg;
}

bool cDialog::doAnimations = false;

void cDialog::draw(){
	win.setActive();
	tileImage(win,winRect,::bg[bg]);
	if(doAnimations && animTimer.getElapsedTime().asMilliseconds() >= 500) {
		cPict::advanceAnim();
		animTimer.restart();
	}
	
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		iter->second->draw();
		iter++;
	}
	
	win.display();
}

cControl& cDialog::operator[](std::string id){
	return getControl(id);
}

cControl& cDialog::getControl(std::string id) {
	ctrlIter iter = controls.find(id);
	if(iter != controls.end()) return *(iter->second);
	
	iter = controls.begin();
	while(iter != controls.end()){
		if(iter->second->getType() == CTRL_GROUP){
			try{
				cLedGroup* tmp = (cLedGroup*) (iter->second);
				return tmp->operator[](id);
			}catch(std::invalid_argument) {}
		}else if(iter->second->getType() == CTRL_STACK){ // TODO: Implement stacks
//			try{
//			cStack* tmp = (cStack*) (iter->second);
//			return tmp->operator[](id);
//			}catch(std::invalid_argument) {}
		}
		iter++;
	}
	throw std::invalid_argument(id + " does not exist in dialog " + fname);
}
