/*
 *  control.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "control.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "dialogxml/widgets/container.hpp"
#include "sounds.hpp"
#include "button.hpp"
#include "led.hpp"
#include "gfx/render_shapes.hpp"
#include "gfx/render_text.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "mathutil.hpp"
#include "tools/prefs.hpp"
#include "tools/cursors.hpp"
#include "replay.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "winutil.hpp"

// Hyperlink forward declaration
extern void launchURL(std::string url);
std::mt19937 cControl::ui_rand;

std::string cControl::generateRandomString() {
	// Not bothering to seed, because it doesn't actually matter if it's truly random.
	int n_chars = ui_rand() % 100;
	std::string s = "$";
	while(n_chars > 0){
		s += char(ui_rand() % 96) + ' '; // was 223 ...
		n_chars--;
	}
	return s;
}

void cControl::setText(std::string l){
	lbl = l;
	// NOTE: calling recalcRect() here seems like a good idea but it has too many unwanted side-effects.
	// It's better to call it manually when changing clickable text.
}

void cControl::replaceText(std::string find, std::string replace) {
	std::string text = getText();
	boost::replace_first(text, find, replace);
	setText(text);
}

void cControl::appendText(std::string l) {
	std::string text = getText();
	setText(text + l);
}

std::string cControl::getText() const {
	return lbl;
}

rectangle cControl::getBounds() const {
	return frame;
}

void cControl::setBounds(rectangle newFrame) {
	frame = newFrame;
}

void cControl::relocate(location to) {
	frame.offset(to.x - frame.left, to.y - frame.top);
}

void cControl::relocateRelative(location to, cControl* anchor, ePosition h, ePosition v) {
	if(anchor == nullptr) anchor = this;
	// Determine the anchor point of the relocation
	location anchorPoint;
	switch(h) {
		case POS_ABS: anchorPoint.x = 0; break;
		case POS_REL_PLUS: anchorPoint.x = anchor->frame.right; break;
		case POS_REL_NEG: anchorPoint.x = anchor->frame.left; to.x = -to.x; break;
		case POS_CONT_PLUS: anchorPoint.x = anchor->frame.left; break;
		case POS_CONT_NEG: anchorPoint.x = anchor->frame.right; to.x = -to.x; break;
	}
	switch(v) {
		case POS_ABS: anchorPoint.y = 0; break;
		case POS_REL_PLUS: anchorPoint.y = anchor->frame.bottom; break;
		case POS_REL_NEG: anchorPoint.y = anchor->frame.top; to.y = -to.y; break;
		case POS_CONT_PLUS: anchorPoint.y = anchor->frame.top; break;
		case POS_CONT_NEG: anchorPoint.y = anchor->frame.bottom; to.y = -to.y; break;
	}
	to.x += anchorPoint.x;
	to.y += anchorPoint.y;
	relocate(to);
}

const char* xHandlerNotSupported::msg[4] = {
	"This control cannot handle click events.\n",
	"This control cannot handle focus events.\n",
	"This control cannot handle defocus events.\n",
	"This control cannot handle scroll events.\n",
};

xHandlerNotSupported::xHandlerNotSupported(eDlogEvt t){
	this->evt = t;
}
const char* xHandlerNotSupported::what() const throw() {
	assert("A handler not supported message is missing!" && int(evt) < 4);
	return msg[evt];
}

xUnsupportedProp::xUnsupportedProp(eFormat prop) throw(){
	whichProp = prop;
	msg = nullptr;
}
xUnsupportedProp::~xUnsupportedProp() throw(){
	if(msg != nullptr) delete msg;
}
const char* xUnsupportedProp::what() const throw(){
	if(msg == nullptr){
		msg = new char[62];
		std::string s;
		switch(whichProp){
			case TXT_FRAME:
				s = "TXT_FRAME";
				break;
			case TXT_FONT:
				s = "TXT_FONT";
				break;
			case TXT_SIZE:
				s = "TXT_SIZE";
				break;
			case TXT_WRAP:
				s = "TXT_WRAP";
				break;
			case TXT_COLOUR:
				s = "TXT_COLOUR";
				break;
		}
		sprintf(msg,"Format property %s not valid for this control.\n",s.c_str());
	}
	return msg;
}

void cControl::show(){
	visible = true;
	if(labelCtrl) labelCtrl->show();
}

void cControl::hide(){
	visible = false;
	if(labelCtrl) labelCtrl->hide();
}

cDialog* cControl::getDialog() const {
	if(auto container = dynamic_cast<cContainer*>(parent)) {
		return container->getDialog();
	} else if(auto dlog = dynamic_cast<cDialog*>(parent)) {
		return dlog;
	}
	return nullptr;
}

sf::RenderWindow& cControl::getWindow() {
	return parent->getWindow();
}

iComponent& cControl::getParent() const {
	return *parent;
}

sf::Color cControl::getDefTextClr() const {
	return parent ? parent->getDefTextClr() : sf::Color::Black;
}

bool cControl::isVisible() const {
	if(!getDialog() || getDialog()->dialogNotToast)
		return visible;
	else return false;
}

void cControl::setLabelCtrl(cControl* label) {
	labelCtrl = label;
}

cControl* cControl::getLabelCtrl() {
	return labelCtrl;
}

cKey cControl::getAttachedKey() const {
	return key;
}

void cControl::setActive(bool active) {
	depressed = active;
}

void cControl::setFormat(eFormat prop, short val) {
	boost::any newVal;
	switch(prop) {
		case TXT_WRAP:
			newVal = bool(val);
			break;
		case TXT_FONT:
			newVal = eFont(val);
			break;
		case TXT_SIZE:
			newVal = val;
			break;
		case TXT_FRAME:
			newVal = eFrameStyle(val);
			break;
		case TXT_COLOUR: // Interpret as a shade of grey
			newVal = sf::Color(val, val, val);
			break;
	}
	if(!manageFormat(prop, true, &newVal))
		throw xUnsupportedProp(prop);
}

short cControl::getFormat(eFormat prop) const {
	boost::any curVal;
	if(!const_cast<cControl&>(*this).manageFormat(prop, false, &curVal))
		throw xUnsupportedProp(prop);
	switch(prop) {
		case TXT_WRAP:
			return boost::any_cast<bool>(curVal);
		case TXT_FONT:
			return boost::any_cast<eFont>(curVal);
		case TXT_SIZE:
			return boost::any_cast<short>(curVal);
		case TXT_FRAME:
			return boost::any_cast<eFrameStyle>(curVal);
		case TXT_COLOUR: // Interpret as a shade of grey
			return boost::any_cast<sf::Color>(curVal).toInteger();
	}
	return 0;
}

bool cControl::canFormat(eFormat prop) const {
	return const_cast<cControl&>(*this).manageFormat(prop, false, nullptr);
}

void cControl::setColour(sf::Color clr) {
	boost::any newVal = clr;
	if(!manageFormat(TXT_COLOUR, true, &newVal))
		throw xUnsupportedProp(TXT_COLOUR);
}

sf::Color cControl::getColour() const {
	boost::any curVal;
	if(!const_cast<cControl&>(*this).manageFormat(TXT_COLOUR, false, &curVal))
		throw xUnsupportedProp(TXT_COLOUR);
	return boost::any_cast<sf::Color>(curVal);
}

bool cControl::manageFormat(eFormat, bool, boost::any*) {
	return false;
}

void cControl::redraw() {
	// If there's no parent dialog, we're not responsible for redrawing
	if(getDialog()) getDialog()->draw();
}

void cControl::playClickSound(){
	if(get_bool_pref("PlaySounds", true)) {
		if(typeid(this) == typeid(cLed*))
			play_sound(34);
		else play_sound(37);
		sf::sleep(time_in_ticks(6));
	}
	else sf::sleep(time_in_ticks(14));
}

bool cControl::handleClick(location, cFramerateLimiter& fps_limiter){
	if(!visible) return false;
	sf::Event e;
	bool done = false, clicked = false;
	getWindow().setActive();
	depressed = true;
	while(!done){
		redraw();
		while(pollEvent(getWindow(), e)){
			if(e.type == sf::Event::MouseButtonReleased){
				done = true;
				location clickPos(e.mouseButton.x, e.mouseButton.y);
				clickPos = getWindow().mapPixelToCoords(clickPos);
				clicked = frame.contains(clickPos);
				depressed = false;
				break;
			} else if(e.type == sf::Event::MouseMoved){
				restore_cursor();
				location toPos(e.mouseMove.x, e.mouseMove.y);
				toPos = getWindow().mapPixelToCoords(toPos);
				depressed = frame.contains(toPos);
			}
		}
		fps_limiter.frame_finished();
	}
	playClickSound();
	
	redraw();
	return clicked;
}

void cControl::handleKeyTriggered(cDialog& parent) {
	setActive(true);
	parent.draw();
	playClickSound();
	setActive(false);
	parent.draw();
	sf::sleep(sf::milliseconds(8));
	triggerClickHandler(parent,name,mod_none);
}

std::string cControl::getAttachedKeyDescription() const {
	std::string s;
	if(key.spec) {
		auto mod = key.mod;
		std::string keyName;
		switch(key.k) {
			case key_left: keyName = "left"; break;
			case key_right: keyName = "right"; break;
			case key_up: keyName = "up"; break;
			case key_down: keyName = "down"; break;
			case key_esc: keyName = "esc"; break;
			case key_enter: keyName = "enter"; break;
			case key_tab: keyName = "tab"; break;
			case key_help: keyName = "help"; break;
			case key_bsp: keyName = "backspace"; break;
			case key_del: keyName = "delete"; break;
			case key_home: keyName = "home"; break;
			case key_end: keyName = "end"; break;
			case key_pgup: keyName = "pgup"; break;
			case key_pgdn: keyName = "pgdn"; break;
			case key_insert: keyName = "insert"; break;
			case key_copy: keyName = "c"; mod += mod_ctrl; break;
			case key_cut: keyName = "x"; mod += mod_ctrl; break;
			case key_paste: keyName = "v"; mod += mod_ctrl; break;
			case key_selectall: keyName = "a"; mod += mod_ctrl; break;
			case key_undo: keyName = "z"; mod += mod_ctrl; break;
			case key_redo: keyName = "y"; mod += mod_ctrl; break;
			case key_top: keyName = "home"; mod += mod_ctrl; break;
			case key_bottom: keyName = "end"; mod += mod_ctrl; break;
#ifdef __APPLE__
			case key_word_left: keyName = "left"; mod += mod_alt; break;
			case key_word_right: keyName = "right"; mod += mod_alt; break;
			case key_word_bsp: keyName = "backspace"; mod += mod_alt; break;
			case key_word_del: keyName = "delete"; mod += mod_alt; break;
#else
			case key_word_left: keyName = "left"; mod += mod_ctrl; break;
			case key_word_right: keyName = "right"; mod += mod_ctrl; break;
			case key_word_bsp: keyName = "backspace"; mod += mod_ctrl; break;
			case key_word_del: keyName = "delete"; mod += mod_ctrl; break;
#endif
			case key_find: keyName = "f"; mod += mod_ctrl; break;
		}
		if(mod_contains(mod, mod_ctrl)) s += '^';
		if(mod_contains(mod, mod_alt)) s += '#';
		if(mod_contains(mod, mod_shift)) s += '$';
	} else {
		unsigned char c = key.c;
		if(mod_contains(key.mod, mod_shift)) c = applyShift(c);
		else c = removeShift(c);
		if(mod_contains(key.mod, mod_ctrl)) s += '^';
		if(mod_contains(key.mod, mod_alt)) s += '#';
		if(c == ' ') s += "space";
		else s += c;
	}
	return s;
}

void cControl::attachKey(cKey key){
	this->key = key;
}

void cControl::detachKey(){
	this->key.spec = false;
	this->key.c = 0;
}

cControl::cControl(eControlType t, iComponent& p)
	: parent(&p)
	, type(t)
	, visible(true)
	, key({false, 0, mod_none})
	, frameStyle(FRM_INSET)
{}

void cControl::attachClickHandler(std::function<bool(cDialog&,std::string,eKeyMod)> f) {
	if(!f) {
		attachEventHandler<EVT_CLICK>(nullptr);
		return;
	}
	attachEventHandler<EVT_CLICK>([f](cDialog& me, std::string id, eKeyMod mods) {
		f(me, id, mods);
	});
}

void cControl::attachFocusHandler(std::function<bool(cDialog&,std::string,bool)> f) {
	if(!f) {
		attachEventHandler<EVT_FOCUS>(nullptr);
		attachEventHandler<EVT_DEFOCUS>(nullptr);
		return;
	}
	using namespace std::placeholders;
	attachEventHandler<EVT_FOCUS>([f](cDialog& me, std::string id) {
		f(me, id, false);
	});
	attachEventHandler<EVT_DEFOCUS>(std::bind(f, _1, _2, true));
}

bool cControl::haveHandler(eDlogEvt t) const {
	auto iter = event_handlers.find(t);
	if(iter == event_handlers.end()) return false;
	return !iter->second.empty();
}

bool cControl::triggerClickHandler(cDialog& dlg, std::string id, eKeyMod mods){
	if(recording){
		std::map<std::string, std::string> action_info = {{"id", id}, {"mods", boost::lexical_cast<std::string>(mods)}};
		record_action("click_control", action_info);
	}
	triggerEvent<EVT_CLICK>(dlg, id, mods);
	return true;
}

bool cControl::triggerFocusHandler(cDialog& dlg, std::string id, bool losing){
	if(losing) return triggerEvent<EVT_DEFOCUS>(dlg, id);
	triggerEvent<EVT_FOCUS>(dlg, id);
	return true;
}

void cControl::drawFrame(short amt, eFrameStyle frameStyle){
	if(frameStyle == FRM_NONE) return;
	// dk_gray had a 0..65535 component of 12287, and med_gray had a 0..65535 component of 24574
	static sf::Color lt_gray = {224,224,224},dk_gray = {48,48,48};
	rectangle rect = frame, ul_rect;
	
	getWindow().setActive();
	
	rect.inset(-amt,-amt);
	ul_rect = rect;
	
	if(frameStyle == FRM_OUTSET) {
		ul_rect.right -= 1;
		ul_rect.bottom -= 1;
	} else if(frameStyle == FRM_INSET) {
		ul_rect.top += 1;
		ul_rect.left += 1;
	}
	
	frame_rect(getWindow(), rect, dk_gray);
	if(frameStyle == FRM_OUTSET || frameStyle == FRM_INSET) {
		clip_rect(getWindow(), ul_rect);
		frame_rect(getWindow(), rect, lt_gray);
		undo_clip(getWindow());
	} else if(frameStyle == FRM_DOUBLE) {
		rect.inset(-amt, -amt);
		frame_rect(getWindow(), rect, dk_gray);
	}
}

void cControl::parse(ticpp::Element& who, std::string fname) {
	using namespace ticpp;
	std::string tagName;
	who.GetValue(&tagName);
	Iterator<Attribute> attr;
	Iterator<Node> node;
	std::set<std::string> foundAttrs;
	std::multiset<std::string> foundNodes;
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		foundAttrs.insert(attr->Name());
		if(!parseAttribute(*attr, tagName, fname))
			throw xBadAttr(tagName, attr->Name(), attr->Row(), attr->Column(), fname);
	}
	if(auto namer = dynamic_cast<iNameGiver*>(&getParent())) {
		name = namer->generateId(name);
	}
	std::string text;
	for(node = node.begin(&who); node != node.end(); node++){
		int type = node->Type();
		std::string nodeTag;
		if(type == TiXmlNode::ELEMENT)
			nodeTag = node->Value();
		if(type == TiXmlNode::COMMENT) continue;
		else if(!parseContent(*node, foundNodes.count(nodeTag), tagName, fname, text)) {
			std::string val = nodeTag.empty() ? xBadVal::CONTENT : nodeTag;
			throw xBadVal(tagName, xBadVal::CONTENT, val, node->Row(), node->Column(), fname);
		}
		foundNodes.insert(nodeTag);
	}
	setText(text);
	location bestSz = getPreferredSize();
	frame.width() = width > 0 ? width : bestSz.x;
	frame.height() = height > 0 ? height : bestSz.y;
	validatePostParse(who, fname, foundAttrs, foundNodes);

	// Wire links to function:
	// TODO links are identified only by having the color 'link', and can only link to their text value.
	if(is_link){
		attachClickHandler([](cDialog& self, std::string clicked, eKeyMod) {
			launchURL(self[clicked].getText());
			return false;
		});
	}
}

bool cControl::parseAttribute(ticpp::Attribute& attr, std::string tagName, std::string fname) {
	std::string name;
	attr.GetName(&name);
	// Unique identifier
	if(name == "name") {
		attr.GetValue(&this->name);
		return true;
	}
	// Position and size
	if(name == "top") {
		attr.GetValue(&frame.top);
		return true;
	}
	if(name == "left") {
		attr.GetValue(&frame.left);
		return true;
	}
	if(name == "width") {
		attr.GetValue(&width);
		return true;
	}
	if(name == "height") {
		attr.GetValue(&height);
		return true;
	}
	// Relative positioning
	if(name == "relative") {
		static auto space = " \t";
		std::string rel = attr.Value();
		const xBadVal err(tagName, name, rel, attr.Row(), attr.Column(), fname);
		size_t border = rel.find_first_of(space);
		if(border != std::string::npos) {
			size_t border_end = rel.find_last_of(space);
			// Error if any of [border, border_end] are not spaces
			for(size_t i = border + 1; i < border_end; i++) {
				if(rel[i] != ' ' && rel[i] != '\t') throw err;
			}
			std::string h = rel.substr(0, border), v = rel.substr(border_end + 1);
			if(h == "abs") horz = POS_ABS;
			else if(h == "pos") horz = POS_REL_PLUS;
			else if(h == "neg") horz = POS_REL_NEG;
			else if(h == "pos-in") horz = POS_CONT_PLUS;
			else if(h == "neg-in") horz = POS_CONT_NEG;
			else throw err;
			if(v == "abs") vert = POS_ABS;
			else if(v == "pos") vert = POS_REL_PLUS;
			else if(v == "neg") vert = POS_REL_NEG;
			else if(v == "pos-in") vert = POS_CONT_PLUS;
			else if(v == "neg-in") vert = POS_CONT_NEG;
			else throw err;
		}
		else if(rel == "abs") horz = vert = POS_ABS;
		else if(rel == "pos") horz = vert = POS_REL_PLUS;
		else if(rel == "neg") horz = vert = POS_REL_NEG;
		else if(rel == "pos-in") horz = vert = POS_CONT_PLUS;
		else if(rel == "neg-in") horz = vert = POS_CONT_NEG;
		else throw err;
		return true;
	}
	if(name == "anchor") {
		anchor = attr.Value();
		return true;
	}
	if(name == "rel-anchor") {
		std::string val = attr.Value();
		if(val == "next") anchor = "$$next$$";
		else if(val == "prev") anchor = "$$prev$$";
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}
	// Colour and formatting, if supported
	if(name == "framed" && canFormat(TXT_FRAME)) {
		std::string val;
		attr.GetValue(&val);
		if(val == "true") setFormat(TXT_FRAME, FRM_SOLID);
		else if(val == "false") setFormat(TXT_FRAME, FRM_NONE);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}
	if(name == "outline" && canFormat(TXT_FRAME)) {
		std::string val;
		attr.GetValue(&val);
		if(val == "none") setFormat(TXT_FRAME, FRM_NONE);
		else if(val == "solid") setFormat(TXT_FRAME, FRM_SOLID);
		else if(val == "inset") setFormat(TXT_FRAME, FRM_INSET);
		else if(val == "outset") setFormat(TXT_FRAME, FRM_OUTSET);
		else if(val == "double") setFormat(TXT_FRAME, FRM_DOUBLE);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}
	if(name == "font" && canFormat(TXT_FONT)) {
		std::string val;
		attr.GetValue(&val);
		if(val == "plain") setFormat(TXT_FONT, FONT_PLAIN);
		else if(val == "bold") setFormat(TXT_FONT, FONT_BOLD);
		else if(val == "dungeon") setFormat(TXT_FONT, FONT_DUNGEON);
		else if(val == "maidenword") setFormat(TXT_FONT, FONT_MAIDWORD);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}
	if(name == "size" && canFormat(TXT_SIZE)) {
		std::string val;
		attr.GetValue(&val);
		if(val == "small") setFormat(TXT_SIZE, 10);
		else if(val == "large") setFormat(TXT_SIZE, 12);
		else if(val == "title") setFormat(TXT_SIZE, 18);
		else{
			auto err = xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
			try{
				setFormat(TXT_SIZE, std::stoi(val));
			}catch(std::invalid_argument& e){
				throw err;
			}catch(std::out_of_range& e){
				throw err;
			}
		}
		return true;
	}
	if(name == "wrap" && canFormat(TXT_WRAP)) {
		std::string val;
		attr.GetValue(&val);
		if(val == "true") setFormat(TXT_WRAP, true);
		else if(val == "false") setFormat(TXT_WRAP, false);
		else throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		return true;
	}
	if((name == "color" || name == "colour") && canFormat(TXT_COLOUR)) {
		std::string val = attr.Value();
		if(val == "link") {
			// TODO: Would be nice if it could work for other backgrounds too...
			if(getDialog()->getBg() == cDialog::BG_DARK)
				setColour({0x7f, 0xd7, 0xFF});
			if(getDialog()->getBg() == cDialog::BG_LIGHT)
				setColour({0x00, 0x00, 0xFF});

			is_link = true;
		} else try {
			sf::Color clr = parseColor(val);
			setColour(clr);
		} catch(int) {
			throw xBadVal(tagName, name, val, attr.Row(), attr.Column(), fname);
		}
		return true;
	}
	return false;
}


bool cControl::parseContent(ticpp::Node&, int, std::string, std::string, std::string&) {
	return false;
}

void cControl::validatePostParse(ticpp::Element& elem, std::string fname, const std::set<std::string>& attrs, const std::multiset<std::string>&) {
	if(!attrs.count("left")) throw xMissingAttr(elem.Value(), "left", elem.Row(), elem.Column(), fname);
	if(!attrs.count("top")) throw xMissingAttr(elem.Value(), "top", elem.Row(), elem.Column(), fname);
	if(attrs.count("relative") && !attrs.count("anchor") && !attrs.count("rel-anchor")) {
		// If relative is specified, an anchor is required... unless it's abs or neg
		if((horz != POS_ABS && horz != POS_REL_NEG) || (vert != POS_ABS && vert != POS_REL_NEG))
			throw xMissingAttr(elem.Value(), "anchor", elem.Row(), elem.Column(), fname);
	}
	if((attrs.count("anchor") || attrs.count("rel-anchor")) && horz == POS_ABS && vert == POS_ABS) {
		// If an anchor is specified, "relative" must be set to something other than "abs".
		if(attrs.count("relative"))
			throw xBadVal(elem.Value(), "relative", "abs", elem.Row(), elem.Column(), fname);
		else throw xMissingAttr(elem.Value(), "relative", elem.Row(), elem.Column(), fname);
	}
	if(attrs.count("anchor") && attrs.count("rel-anchor"))
	   throw xBadAttr(elem.Value(), "(rel-)anchor", elem.Row(), elem.Column(), fname);
}

cControl::~cControl() {}

eControlType cControl::getType() const {
	return type;
}

void cControl::setTextToNum(long long what, const std::string& label){
	std::ostringstream sout;
	sout << label << what;
	setText(sout.str());
}

long long cControl::getTextAsNum() const {
	std::istringstream sin(getText());
	long long n;
	sin >> n;
	return n;
}

bool cControl::hasKey() const {
	if(key.spec) return true;
	return key.c != 0;
}

cControl::storage_t cControl::store() const {
	storage_t storage;
	storage["text"] = lbl;
	storage["visible"] = visible;
	return storage;
}

void cControl::restore(storage_t to) {
	if(to.find("text") != to.end())
		setText(boost::any_cast<std::string>(to["text"]));
	else setText("");
	if(to.find("visible") != to.end())
		boost::any_cast<bool>(to["visible"]) ? show() : hide();
}

// Translate raw x/y position using the view of the current rendering target
location cControl::translated_location(const sf::Vector2i point) const {
	return location { const_cast<cControl*>(this)->getWindow().mapPixelToCoords(point) };
}