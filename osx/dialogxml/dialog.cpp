/*
 *  dialog.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */
#define	SLEEP_TICKS		2L
#define	MOUSE_REGION	0L
#define IN_FRONT	(WindowPtr)-1L

#include "dialog.h"
#include "graphtool.h"
#include "soundtool.h"
using namespace std;
using namespace ticpp;

extern bool play_sounds;
const short cDialog::BG_DARK = 5, cDialog::BG_LIGHT = 16;

template<> pair<string,cPict*> cDialog::parse(Element& who /*pict*/){
	pair<string,cPict*> p;
	Iterator<Attribute> attr;
	string name, val;
	int width = 0, height = 0;
	bool wide = false, tall = false, custom = false;
	p.second = new cPict(this);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		attr->GetValue(&val);
		if(name == "name")
			p.first = val;
		else if(name == "type"){
			if(val == "blank"){
				p.second->picType = PIC_TER;
				p.second->picNum = -1;
			}else if(val == "ter")
				p.second->picType = PIC_TER;
			else if(val == "teranim")
				p.second->picType = PIC_TER_ANIM;
			else if(val == "monst")
				p.second->picType = PIC_MONST;
			else if(val == "dlog")
				p.second->picType = PIC_DLOG;
			else if(val == "talk")
				p.second->picType = PIC_TALK;
			else if(val == "scen")
				p.second->picType = PIC_SCEN;
			else if(val == "item")
				p.second->picType = PIC_ITEM;
			else if(val == "pc")
				p.second->picType = PIC_PC;
			else if(val == "field")
				p.second->picType = PIC_FIELD;
			else if(val == "boom")
				p.second->picType = PIC_BOOM;
			else if(val == "missile")
				p.second->picType = PIC_MISSILE;
			else if(val == "full")
				p.second->picType = PIC_FULL;
			else throw xBadVal("pict",name,val);
		}else if(name == "custom") if(val == "true")
			custom = true;
		else if(name == "clickable") if(val == "true")
			p.second->clickable = true;
		else if(name == "size"){
			if(val == "wide") wide = true;
			else if(val == "tall") tall = true;
			else if(val == "large") wide = tall = true;
			else throw xBadVal("pict",name,val);
		}else if(name == "num"){
			istringstream sin(val);
			sin >> p.second->picNum;
		}else if(name == "top"){
			istringstream sin(val);
			sin >> p.second->frame.top;
		}else if(name == "left"){
			istringstream sin(val);
			sin >> p.second->frame.left;
		}else if(name == "width"){
			istringstream sin(val);
			sin >> width;
		}else if(name == "height"){
			istringstream sin(val);
			sin >> height;
		}else throw xBadAttr("pict",name);
	}
	if(wide && !tall && p.second->picType == PIC_MONST) p.second->picType = PIC_MONST_WIDE;
	else if(!wide && tall && p.second->picType == PIC_MONST) p.second->picType = PIC_MONST_TALL;
	else if(wide && tall){
		if(p.second->picType == PIC_MONST) p.second->picType = PIC_MONST_LG;
		else if(p.second->picType == PIC_SCEN) p.second->picType = PIC_SCEN_LG;
		else if(p.second->picType == PIC_DLOG) p.second->picType = PIC_DLOG_LG;
	}
	if(width > 0 || height > 0 || p.second->picType == PIC_FULL){
		p.second->frame.right = p.second->frame.left + width;
		p.second->frame.bottom = p.second->frame.top + height;
	}else switch(p.second->picType){
		case PIC_DLOG:
			p.second->frame.right = p.second->frame.left + 36;
			p.second->frame.bottom = p.second->frame.top + 36;
			break;
		case PIC_DLOG_LG:
			p.second->frame.right = p.second->frame.left + 72;
			p.second->frame.bottom = p.second->frame.top + 72;
			break;
		case PIC_SCEN:
		case PIC_TALK:
			p.second->frame.right = p.second->frame.left + 32;
			p.second->frame.bottom = p.second->frame.top + 32;
			break;
		case PIC_SCEN_LG:
			p.second->frame.right = p.second->frame.left + 64;
			p.second->frame.bottom = p.second->frame.top + 64;
			break;
		case PIC_MISSILE:
			p.second->frame.right = p.second->frame.left + 18;
			p.second->frame.bottom = p.second->frame.top + 18;
			break;
		default:
			p.second->frame.right = p.second->frame.left + 28;
			p.second->frame.bottom = p.second->frame.top + 36;
			break;
	}
	if(custom) p.second->picType += PIC_CUSTOM;
	return p;
}

template<> pair<string,cTextMsg*> cDialog::parse(Element& who /*text*/){
	pair<string,cTextMsg*> p;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	string name, val;
	int width = 0, height = 0;
	p.second = new cTextMsg(this);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		attr->GetValue(&val);
		if(name == "name")
			p.first = val;
		else if(name == "framed") if(val == "true")
			p.second->drawFramed = true;
		else if(name == "clickable") if(val == "true")
			p.second->clickable = true;
		else if(name == "font"){
			if(val == "dungeon")
				p.second->textFont = DUNGEON;
			else if(val == "geneva")
				p.second->textFont = GENEVA;
			else if(val == "silom")
				p.second->textFont = SILOM;
			else if(val == "maidenword")
				p.second->textFont = MAIDENWORD;
			else throw xBadVal("text",name,val);
		}else if(name == "size"){
			if(val == "large")
				p.second->textSize = 12;
			else if(val == "small")
				p.second->textSize = 10;
			else throw xBadVal("text",name,val);
		}else if(name == "color" || name == "colour"){
			RGBColor clr;
			try{
				clr = parseColor(val);
			}catch(int){
				throw xBadVal("text",name,val);
			}
			p.second->color = clr;
		}else if(name == "top"){
			istringstream sin(val);
			sin >> p.second->frame.top;
		}else if(name == "left"){
			istringstream sin(val);
			sin >> p.second->frame.left;
		}else if(name == "width"){
			istringstream sin(val);
			sin >> width;
		}else if(name == "height"){
			istringstream sin(val);
			sin >> height;
		}else if(name == "fromlist"){
			p.second->fromList = val;
		}else throw xBadAttr("pict",name);
	}
	p.second->frame.right = p.second->frame.left + width;
	p.second->frame.bottom = p.second->frame.top + height;
	string content;
	for(node = node.begin(&who); node != node.end(); node++){
		string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::ELEMENT && val == "br") content += '|'; // because vertical bar is replaced by a newline when drawing strings
		else if(type == TiXmlNode::TEXT) content += val;
		else{
			val = '<' + val + '>';
			throw xBadVal("text","<content>",content + val);
		}
	}
	p.second->lbl = content;
	return p;
}

/**
 * Parses an HTML-style colour string, recognizing three-digit hex, six-digit hex, and HTML colour names.
 */
RGBColor cDialog::parseColor(string what){
	RGBColor clr;
	if(what[0] == '#'){
		unsigned int r,g,b;
		if(sscanf(what.c_str(),"#%2x%2x%2x",&r,&g,&b) < 3)
			if(sscanf(what.c_str(),"#%1x%1x%1x",&r,&g,&b) < 3)
				throw -1;
		clr.red = r, clr.green = g, clr.blue = b;
	}else if(what == "black")
		clr.red = 0x00, clr.green = 0x00, clr.blue = 0x00;
	else if(what == "red")
		clr.red = 0xFF, clr.green = 0x00, clr.blue = 0x00;
	else if(what == "lime")
		clr.red = 0x00, clr.green = 0xFF, clr.blue = 0x00;
	else if(what == "blue")
		clr.red = 0x00, clr.green = 0x00, clr.blue = 0xFF;
	else if(what == "yellow")
		clr.red = 0xFF, clr.green = 0xFF, clr.blue = 0x00;
	else if(what == "aqua")
		clr.red = 0x00, clr.green = 0xFF, clr.blue = 0xFF;
	else if(what == "fuchsia")
		clr.red = 0xFF, clr.green = 0x00, clr.blue = 0xFF;
	else if(what == "white")
		clr.red = 0xFF, clr.green = 0xFF, clr.blue = 0xFF;
	else if(what == "gray" || what == "grey")
		clr.red = 0x80, clr.green = 0x80, clr.blue = 0x80;
	else if(what == "maroon")
		clr.red = 0x80, clr.green = 0x00, clr.blue = 0x00;
	else if(what == "green")
		clr.red = 0x00, clr.green = 0x80, clr.blue = 0x00;
	else if(what == "navy")
		clr.red = 0x00, clr.green = 0x00, clr.blue = 0x80;
	else if(what == "olive")
		clr.red = 0x80, clr.green = 0x80, clr.blue = 0x00;
	else if(what == "teal")
		clr.red = 0x00, clr.green = 0x80, clr.blue = 0x80;
	else if(what == "purple")
		clr.red = 0x80, clr.green = 0x00, clr.blue = 0x80;
	else if(what == "silver")
		clr.red = 0xC0, clr.green = 0xC0, clr.blue = 0xC0;
	else throw -1;
	clr.red *= clr.red;
	clr.green *= clr.green;
	clr.blue *= clr.blue;
	return clr;
}

template<> pair<string,cButton*> cDialog::parse(Element& who /*button*/){
	pair<string,cButton*> p;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	string name, val;
	int width = 0, height = 0;
	p.second = new cButton(this);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		attr->GetValue(&val);
		if(name == "name")
			p.first = val;
		else if(name == "wrap") if(val == "true")
			p.second->wrapLabel = true;
		else if(name == "type"){
			if(val == "small")
				p.second->type = BTN_SM;
			else if(val == "regular")
				p.second->type = BTN_REG;
			else if(val == "large")
				p.second->type = BTN_LG;
			else if(val == "help")
				p.second->type = BTN_HELP;
			else if(val == "left")
				p.second->type = BTN_LEFT;
			else if(val == "right")
				p.second->type = BTN_RIGHT;
			else if(val == "up")
				p.second->type = BTN_UP;
			else if(val == "down")
				p.second->type = BTN_DOWN;
			else if(val == "tiny")
				p.second->type = BTN_TINY;
			else if(val == "done")
				p.second->type = BTN_DONE;
			else if(val == "tall")
				p.second->type = BTN_TALL;
			else if(val == "trait")
				p.second->type = BTN_TRAIT;
			else if(val == "push")
				p.second->type = BTN_PUSH;
		}else if(name == "def-key"){
			try{
				p.second->key = parseKey(val);
			}catch(int){
				throw xBadVal("button",name,val);
			}
		}else if(name == "fromlist")
			p.second->fromList = val;
		else if(name == "top"){
			istringstream sin(val);
			sin >> p.second->frame.top;
		}else if(name == "left"){
			istringstream sin(val);
			sin >> p.second->frame.left;
		}else if(name == "width"){
			istringstream sin;
			sin >> width;
		}else if(name == "height"){
			istringstream sin;
			sin >> height;
		}else throw xBadAttr("button",name);
	}
	if(width > 0 || height > 0) {
		p.second->frame.right = p.second->frame.left + width;
		p.second->frame.bottom = p.second->frame.top + height;
	}else switch(p.second->type){
		case BTN_SM:
			p.second->frame.right = p.second->frame.left + 23;
			p.second->frame.bottom = p.second->frame.top + 23;
			break;
		case BTN_LG:
			p.second->frame.right = p.second->frame.left + 102;
			p.second->frame.bottom = p.second->frame.top + 23;
			break;
		case BTN_HELP:
			p.second->frame.right = p.second->frame.left + 16;
			p.second->frame.bottom = p.second->frame.top + 13;
			break;
		case BTN_TINY:
		case BTN_LED: // this should never happen
			p.second->frame.right = p.second->frame.left + 14;
			p.second->frame.bottom = p.second->frame.top + 10;
			break;
		case BTN_TALL:
		case BTN_TRAIT:
			p.second->frame.right = p.second->frame.left + 63;
			p.second->frame.bottom = p.second->frame.top + 40;
			break;
		case BTN_PUSH:
			p.second->frame.right = p.second->frame.left + 30;
			p.second->frame.bottom = p.second->frame.top + 30;
			break;
		default:
			p.second->frame.right = p.second->frame.left + 63;
			p.second->frame.bottom = p.second->frame.top + 23;
	}
	string content;
	for(node = node.begin(&who); node != node.end(); node++){
		string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::ELEMENT && val == "key"){
			if(content.length() > 0) throw xBadVal("button","<content>",content + val);
			p.second->labelWithKey = true;
		}else if(type == TiXmlNode::TEXT) content += val;
		else{
			val = '<' + val + '>';
			throw xBadVal("text","<content>",val);
		}
	}
	p.second->lbl = content;
	return p;
}

cKey cDialog::parseKey(string what){
	cKey key;
	key.spec = false;
	key.c = 0;
	key.mod = mod_none;
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
	string name, val;
	int width = 0, height = 0;
	p.second = new cLed(this);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		attr->GetValue(&val);
		if(name == "name")
			p.first = val;
		else if(name == "state"){
			if(val == "red") p.second->state = led_red;
			else if(val == "green") p.second->state = led_green;
			else if(val == "off") p.second->state = led_off;
			else throw xBadVal("led",name,val);
		}else if(name == "fromlist")
			p.second->fromList = val;
		else if(name == "font"){
			if(val == "dungeon")
				p.second->textFont = DUNGEON;
			else if(val == "geneva")
				p.second->textFont = GENEVA;
			else if(val == "silom")
				p.second->textFont = SILOM;
			else if(val == "maidenword")
				p.second->textFont = MAIDENWORD;
			else throw xBadVal("text",name,val);
		}else if(name == "size"){
			if(val == "large")
				p.second->textSize = 12;
			else if(val == "small")
				p.second->textSize = 10;
			else throw xBadVal("text",name,val);
		}else if(name == "color" || name == "colour"){
			RGBColor clr;
			try{
				clr = parseColor(val);
			}catch(int){
				throw xBadVal("text",name,val);
			}
			p.second->color = clr;
		}else if(name == "top"){
			istringstream sin(val);
			sin >> p.second->frame.top;
		}else if(name == "left"){
			istringstream sin(val);
			sin >> p.second->frame.left;
		}else if(name == "width"){
			istringstream sin;
			sin >> width;
		}else if(name == "height"){
			istringstream sin;
			sin >> height;
		}else throw xBadAttr("button",name);
	}
	if(width > 0 || height > 0) {
		p.second->frame.right = p.second->frame.left + width;
		p.second->frame.bottom = p.second->frame.top + height;
	}else{
		p.second->frame.right = p.second->frame.left + 14;
		p.second->frame.bottom = p.second->frame.top + 10;
	}
	string content;
	for(node = node.begin(&who); node != node.end(); node++){
		string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::TEXT) content += val;
		else{
			val = '<' + val + '>';
			throw xBadVal("text","<content>",content + val);
		}
	}
	p.second->lbl = content;
	return p;
}

template<> pair<string,cLedGroup*> cDialog::parse(Element& who /*group*/){
	pair<string,cLedGroup*> p;
	Iterator<Attribute> attr;
	Iterator<Element> node;
	string name, val;
	p.second = new cLedGroup(this);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		attr->GetValue(&val);
		if(name == "name")
			p.first = val;
		else if(name == "fromlist")
			p.second->fromList = val;
		else throw xBadAttr("button",name);
	}
	string content;
	for(node = node.begin(&who); node != node.end(); node++){
		string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::ELEMENT && val == "led"){
			p.second->choices.insert(parse<cLed>(*node));
		}else{
			val = '<' + val + '>';
			throw xBadVal("text","<content>", content + val);
		}
	}
	p.second->lbl = content;
	return p;
}

template<> pair<string,cTextField*> cDialog::parse(Element& who /*field*/){
	pair<string,cTextField*> p;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	string name, val;
	int width = 0, height = 0;
	p.second = new cTextField(this);
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		attr->GetValue(&val);
		if(name == "name")
			p.first = val;
		else if(name == "type"){
			if(val == "num")
				p.second->isNumericField = true;
			else if(val == "text")
				p.second->isNumericField = false;
			else throw xBadVal("field",name,val);
		}else if(name == "top"){
			istringstream sin(val);
			sin >> p.second->frame.top;
		}else if(name == "left"){
			istringstream sin(val);
			sin >> p.second->frame.left;
		}else if(name == "width"){
			istringstream sin;
			sin >> width;
		}else if(name == "height"){
			istringstream sin;
			sin >> height;
		}else throw xBadAttr("button",name);
	}
	p.second->frame.right = p.second->frame.left + width;
	p.second->frame.bottom = p.second->frame.top + height;
	return p;
}

cDialog::cDialog(string path){
	try{
		Document xml(path);
		xml.LoadFile();
		
		Iterator<Attribute> attr;
		Iterator<Element> node;
		string type, name, val;
		
		xml.GetValue(&type);
		if(type != "dialog") throw xBadNode(type);
		for(attr = attr.begin(&xml); attr != attr.end(); attr++){
			attr->GetName(&name);
			attr->GetValue(&val);
			if(name == "skin"){
				if(val == "light") bg = BG_LIGHT;
				else if(val == "dark") bg = BG_DARK;
				else{
					istringstream sin(val);
					sin >> bg;
					if(sin.fail()) throw xBadVal(type,name,val);
				}
			}else if(name != "debug")
				throw xBadAttr(type,name);
		}
		
		for(node = node.begin(xml.FirstChildElement()); node != node.end(); node++){
			node->GetValue(&type);
			// Yes, I'm using insert instead of [] to add elements to the map.
			// In this situation, it's actually easier that way; the reason being, the
			// map key is obtained from the name attribute of each element.
			if(type == "field")
				controls.insert(parse<cTextField>(*node));
			else if(type == "text")
				controls.insert(parse<cTextMsg>(*node));
			else if(type == "pict")
				controls.insert(parse<cPict>(*node));
			else if(type == "button")
				controls.insert(parse<cButton>(*node));
			else if(type == "led")
				controls.insert(parse<cLed>(*node));
			else if(type == "group")
				controls.insert(parse<cLedGroup>(*node));
			else throw xBadNode(type);
		}
	} catch(Exception& x){ // XML processing exception
		printf(x.what());
	} catch(xBadVal& x){ // Invalid value for an attribute
		printf(x.what());
	} catch(xBadAttr& x){ // Invalid attribute for an element
		printf(x.what());
	} catch(xBadNode& x){ // Invalid element
		printf(x.what());
	}
	dialogNotToast = true;
	bg = BG_DARK; // default is dark background
	// now calculate window rect
	SetRect(&winRect,0,0,0,0);
	ctrlIter iter = controls.begin();
	currentFocus = NULL;
	while(iter != controls.end()){
		if(iter->second->frame.right > winRect.right)
			winRect.right = iter->second->frame.right;
		if(iter->second->frame.bottom > winRect.bottom)
			winRect.bottom = iter->second->frame.bottom;
		if(typeid(iter->second) == typeid(cTextField*)){
			cTextField* fld = (cTextField*) iter->second;
			if(currentFocus == NULL) currentFocus = fld;
			// TODO: Should probably create controls and put them in the window?
		}
		iter++;
	}
	winRect.right += 4;
	winRect.bottom += 4;
	win = NewCWindow(NULL, &winRect, (unsigned char*) "", false, dBoxProc, IN_FRONT, false, 0);
}

void cDialog::init(){
	cControl::init();
	cButton::init();
	cLed::init();
	cPict::init();
}

cDialog::~cDialog(){
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		delete iter->second;
		iter++;
	}
	DisposeWindow(win);
}

bool cDialog::add(cControl* what, std::string key){
	// First make sure the key is not already present.
	// If it is, we can't add the control, so return false.
	if(controls.find(key) != controls.end()) return false;
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

void cDialog::run(){
	char c, k;
	cKey key;
	EventRecord currentEvent;
	std::string itemHit = "";
	dialogNotToast = true;
	ShowWindow(win);
	BeginAppModalStateForWindow(win);
	while(dialogNotToast){
		WaitNextEvent(everyEvent, &currentEvent, SLEEP_TICKS, MOUSE_REGION);
		switch(currentEvent.what){
			case updateEvt:
				BeginUpdate(win);
				draw();
				EndUpdate(win);
				break;
			case keyDown:
			case autoKey:
				c = currentEvent.message & charCodeMask;
				k = (currentEvent.message & keyCodeMask) >> 8;
				switch(k){
					case 126:
						key.spec = true;
						key.k = key_up;
						break;
					case 124:
						key.spec = true;
						key.k = key_right;
						break;
					case 123:
						key.spec = true;
						key.k = key_left;
						break;
					case 125:
						key.spec = true;
						key.k = key_down;
						break;
					case 53:
						key.spec = true;
						key.k = key_esc;
						break;
					case 36: case 76:
						key.spec = true;
						key.k = key_enter;
						break;
					// TODO: Add cases for key_tab and key_help
					default:
						key.spec = false;
						key.c = c;
						break;
				}
				key.mod = mod_none;
				if(currentEvent.modifiers & cmdKey){
					if(key.spec){
						if(key.k == key_left) key.k = key_home;
						else if(key.k == key_right) key.k = key_end;
						else if(key.k == key_up) key.k = key_pgup;
						else if(key.k == key_down) key.k = key_pgdn;
						else key.mod += mod_ctrl;
					}else key.mod += mod_ctrl;
				}if(currentEvent.modifiers & shiftKey) key.mod += mod_shift;
				if(currentEvent.modifiers & alphaLock) key.mod += mod_shift;
				if(currentEvent.modifiers & optionKey) key.mod += mod_alt;
				if(currentEvent.modifiers & controlKey) key.mod += mod_ctrl;
				if(currentEvent.modifiers & rightShiftKey) key.mod += mod_shift;
				if(currentEvent.modifiers & rightOptionKey) key.mod += mod_alt;
				if(currentEvent.modifiers & rightControlKey) key.mod += mod_ctrl;
				itemHit = process_keystroke(key); // TODO: This should be a separate check from the fields thing?
				if(typeid(controls[itemHit]) == typeid(cTextField*)){
					if(!key.spec || key.k == key_bsp || key.k == key_del || key.k == key_home ||
						key.k == key_end || key.k == key_pgup || key.k == key_pgdn ||
						key.k == key_left || key.k == key_right || key.k == key_up || key.k == key_down){
						// TODO: If the dialog contains a field, handle it here.
						// Basically, we should end up here if the user is typing into the field.
						itemHit = "";
					}else if(key.spec && key.k == key_tab){
						// TODO: Tabbing through fields, and trigger focus events.
						ctrlIter cur = controls.find(itemHit), iter;
						if(!cur->second->triggerFocusHandler(*this,itemHit,true)) break;
						iter = cur;
						while(iter != cur){
							if(typeid(iter->second) == typeid(cTextField*)){
								if(iter->second->triggerFocusHandler(*this,iter->first,false)){
									currentFocus = (cTextField*) iter->second;
									itemHit = "";
									break;
								}
							}
							iter++;
							if(iter == controls.end()) iter = controls.begin();
						}
						if(iter == cur) // no focus change occured!
							; // TODO: Surely something should happen here?
					}
				}
				break;
			case mouseDown:
				if(!PtInRect(currentEvent.where,&winRect)) break; // may be superfluous?
				key.mod = mod_none;
				if(currentEvent.modifiers & cmdKey) key.mod += mod_ctrl;
				if(currentEvent.modifiers & shiftKey) key.mod += mod_shift;
				if(currentEvent.modifiers & alphaLock) key.mod += mod_shift;
				if(currentEvent.modifiers & optionKey) key.mod += mod_alt;
				if(currentEvent.modifiers & controlKey) key.mod += mod_ctrl;
				if(currentEvent.modifiers & rightShiftKey) key.mod += mod_shift;
				if(currentEvent.modifiers & rightOptionKey) key.mod += mod_alt;
				if(currentEvent.modifiers & rightControlKey) key.mod += mod_ctrl;
				itemHit = process_click(currentEvent.where,key.mod);
				break;
		}
		ctrlIter ctrl = controls.find(itemHit);
		if(ctrl != controls.end()) ctrl->second->triggerClickHandler(*this,itemHit,key.mod);
	}
	EndAppModalStateForWindow(win);
	HideWindow(win);
}

void cDialog::setBg(short n){
	bg = n;
}

void cDialog::setDefTextClr(RGBColor clr){
	defTextClr = clr;
}

void cDialog::toast(){
	dialogNotToast = false;
}

std::string cDialog::process_keystroke(cKey keyHit){
	unsigned long dummy;
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		if(iter->second->visible && iter->second->isClickable() && iter->second->key == keyHit){
			iter->second->depressed = true;
			iter->second->draw();
			if (play_sounds) {
				if(typeid(iter->second) == typeid(cLed*))
					play_sound(34);
				else play_sound(37);
				Delay(6,&dummy);
			}
			else Delay(14,&dummy);
			iter->second->depressed = false;
			iter->second->draw();
			Delay(8,&dummy);
			return iter->first;
		}
		iter++;
	}
	// If you get an escape and it isn't processed, make it an enter.
	if(keyHit.spec && keyHit.k == key_esc){
		keyHit.k = key_enter;
		return process_keystroke(keyHit);
	}
	return "";
}

std::string cDialog::process_click(Point where, eKeyMod mods){
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		if(iter->second->visible && iter->second->isClickable() && PtInRect(where,&iter->second->frame)){
			if(iter->second->handleClick())
				return iter->first;
			else return "";
		}
		iter++;
	}
	return "";
}

xBadNode::xBadNode(std::string t) throw() : type(t), msg(NULL) {}

const char* xBadNode::what() throw() {
	if(msg == NULL){
		char* s = new (nothrow) char[100];
		if(s == NULL){
			printf("Allocation of memory for error message failed, bailing out...");
			abort();
		}
		sprintf(s,"XML Parse Error: Unknown element %s encountered.",type.c_str());
		msg = s;
	}
	return msg;
}

xBadNode::~xBadNode() throw(){
	if(msg != NULL) delete msg;
}

xBadAttr::xBadAttr(std::string t, std::string n) throw() : type(t), name(n), msg(NULL) {}

const char* xBadAttr::what() throw() {
	if(msg == NULL){
		char* s = new (nothrow) char[100];
		if(s == NULL){
			printf("Allocation of memory for error message failed, bailing out...");
			abort();
		}
		sprintf(s,"XML Parse Error: Unknown attribute %s encountered on element %s.",name.c_str(),type.c_str());
		msg = s;
	}
	return msg;
}

xBadAttr::~xBadAttr() throw(){
	if(msg != NULL) delete msg;
}

xBadVal::xBadVal(std::string t, std::string n, std::string v) throw() : type(t), name(n), val(v), msg(NULL) {}

const char* xBadVal::what() throw() {
	if(msg == NULL){
		char* s = new (nothrow) char[100];
		if(s == NULL){
			printf("Allocation of memory for error message failed, bailing out...");
			abort();
		}
		sprintf(s,"XML Parse Error: Invalid value %s for attribute %s encountered on element %s.",val.c_str(),name.c_str(),type.c_str());
		msg = s;
	}
	return msg;
}

xBadVal::~xBadVal() throw(){
	if(msg != NULL) delete msg;
}

void cDialog::draw(){
	GrafPtr old_port;
	GetPort(&old_port);
	SetPortWindowPort(win);
	FillCRect(&winRect,::bg[bg]);
	
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		iter->second->draw();
		iter++;
	}
	
	SetPort(old_port);
}
