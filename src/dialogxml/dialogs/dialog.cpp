/*
 *  dialog.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include <cmath>
#include <stdexcept>
#include "dialog.hpp"
#include "tiling.hpp" // for bg
#include "sounds.hpp"
#include "pict.hpp"
#include "button.hpp"
#include "field.hpp"
#include "message.hpp"
#include "scrollbar.hpp"
#include "scrollpane.hpp"
#include "stack.hpp"
#include "winutil.hpp"
#include "mathutil.hpp"
#include "cursors.hpp"
#include "prefs.hpp"

using namespace std;
using namespace ticpp;

// TODO: Would be nice if I could avoid depending on mainPtr
extern sf::RenderWindow mainPtr;

extern sf::Texture bg_gworld;
const short cDialog::BG_DARK = 5, cDialog::BG_LIGHT = 16;
short cDialog::defaultBackground = cDialog::BG_DARK;
cDialog* cDialog::topWindow = nullptr;

std::string cDialog::generateRandomString(){
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

string cControl::dlogStringFilter(string toFilter) {
	string filtered;
	bool found_nl = false;
	for(char c : toFilter) {
		if(c == '\n' || c == '\r') found_nl = true;
		else if(c != '\t' && c != 0) {
			if(found_nl && !filtered.empty()) filtered += ' ';
			found_nl = false;
			filtered += c;
		}
	}
	return filtered;
}

sf::Color cControl::parseColor(string what){
	sf::Color clr;
	if(what[0] == '#'){
		unsigned int r,g,b;
		if(sscanf(what.c_str(),"#%2x%2x%2x",&r,&g,&b) < 3) {
			if(sscanf(what.c_str(),"#%1x%1x%1x",&r,&g,&b) < 3)
				throw -1;
			else {
				r *= 0x11;
				g *= 0x11;
				b *= 0x11;
			}
		}
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

cKey cControl::parseKey(string what){
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

cDialog::cDialog(cDialog* p) : parent(p) {}

cDialog::cDialog(std::string path, cDialog* p) : parent(p) {
	loadFromFile(path + ".xml");
}

extern fs::path progDir;
void cDialog::loadFromFile(std::string path){
	static const cKey enterKey = {true, key_enter};
	bg = defaultBackground;
	fname = path;
	fs::path cPath = progDir/"data"/"dialogs"/path;
	try{
		TiXmlBase::SetCondenseWhiteSpace(false);
		Document xml(cPath.string().c_str());
		xml.LoadFile();
		
		Iterator<Attribute> attr;
		Iterator<Element> node;
		string type, name, val, defaultButton;
		
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
					clr = cControl::parseColor(val);
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
			else if(type == "slider")
				controls.insert(parse<cScrollbar>(*node));
			else if(type == "button")
				controls.insert(parse<cButton>(*node));
			else if(type == "led")
				controls.insert(parse<cLed>(*node));
			else if(type == "group")
				controls.insert(parse<cLedGroup>(*node));
			else if(type == "stack") {
				auto parsed = parse<cStack>(*node);
				controls.insert(parsed);
				// Now, if it contains any fields, their tab order must be accounted for
				parsed.second->fillTabOrder(specificTabs, reverseTabs);
			} else if(type == "pane") {
				auto parsed = parse<cScrollPane>(*node);
				controls.insert(parsed);
				// TODO: Now, if it contains any fields, their tab order must be accounted for
				//parsed.second->fillTabOrder(specificTabs, reverseTabs);
			} else throw xBadNode(type,node->Row(),node->Column(),fname);
		}
		
		// Set the default button.
		if(hasControl(defaultButton))
			getControl(defaultButton).attachKey(enterKey);
		
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
		std::cerr << x.what();
		throw;
	} catch(xBadVal& x){ // Invalid value for an attribute
		std::cerr << x.what();
		throw;
	} catch(xBadAttr& x){ // Invalid attribute for an element
		std::cerr << x.what();
		throw;
	} catch(xBadNode& x){ // Invalid element
		std::cerr << x.what();
		throw;
	} catch(xMissingAttr& x){ // Invalid element
		std::cerr << x.what();
		throw;
	} catch(std::exception& x){ // Other uncaught exception
		std::cerr << x.what();
		throw;
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

void cDialog::run(std::function<void(cDialog&)> onopen){
	cDialog* formerTop = topWindow;
	// TODO: The introduction of the static topWindow means I may be able to use this instead of parent->win; do I still need parent?
	sf::RenderWindow* parentWin = &(parent ? parent->win : mainPtr);
	cursor_type former_curs = Cursor::current;
	set_cursor(sword_curs);
	using kb = sf::Keyboard;
	kb::Key k;
	cKey key, pendingKey = {true};
	sf::Event currentEvent;
	std::string itemHit = "";
	dialogNotToast = true;
	// Focus the first text field, if there is one
	if(!tabOrder.empty()) {
		auto iter = std::find_if(tabOrder.begin(), tabOrder.end(), [](std::pair<std::string,cTextField*> ctrl){
			return ctrl.second->isVisible();
		});
		if(iter != tabOrder.end()) {
			iter->second->triggerFocusHandler(*this, iter->first, false);
			currentFocus = iter->first;
		}
	}
	// Sometimes it seems like the Cocoa menu handling clobbers the active rendering context.
	// For whatever reason, delaying 100 milliseconds appears to fix this.
	sf::sleep(sf::milliseconds(100));
	// So this little section of code is a real-life valley of dying things.
	// Instantiating a window and then closing it seems to fix the update error, because magic.
	win.create(sf::VideoMode(1,1),"");
	win.close();
	win.create(sf::VideoMode(winRect.width(), winRect.height()), "Dialog", sf::Style::Titlebar);
	draw();
	makeFrontWindow(parent ? parent-> win : mainPtr);
	makeFrontWindow(win);
	// This is a loose modal session, as it doesn't prevent you from clicking away,
	// but it does prevent editing other dialogs, and it also keeps this window on top
	// even when it loses focus.
	ModalSession dlog(win, *parentWin);
	if(onopen) onopen(*this);
	animTimer.restart();
	while(dialogNotToast){
		draw();
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
				process_keystroke(key);
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
					if(pendingKey.c != '\t')
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
				process_click(where, key.mod);
				break;
			default: // To silence warning of unhandled enum values
				break;
			case sf::Event::GainedFocus:
			case sf::Event::MouseMoved:
				bool inField = false;
				for(auto& ctrl : controls) {
					if(ctrl.second->getType() == CTRL_FIELD && ctrl.second->getBounds().contains(currentEvent.mouseMove.x, currentEvent.mouseMove.y)) {
						set_cursor(text_curs);
						inField = true;
						break;
					}
				}
				if(!inField) set_cursor(sword_curs);
				break;
		}
	}
	win.setVisible(false);
	while(parentWin->pollEvent(currentEvent));
	set_cursor(former_curs);
	topWindow = formerTop;
	makeFrontWindow(*parentWin);
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
		if(iter->second->isFocusable() && iter->second->isVisible()){
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
	didAccept = triggerFocus;
	return true;
}

void cDialog::untoast() {
	dialogNotToast = true;
	if(!currentFocus.empty())
		this->getControl(currentFocus).triggerFocusHandler(*this, currentFocus, false);
}

bool cDialog::accepted() {
	return didAccept;
}

bool cDialog::setFocus(cTextField* newFocus, bool force) {
	if(!force && !currentFocus.empty()) {
		if(!this->getControl(currentFocus).triggerFocusHandler(*this, currentFocus, true)) return false;
	}
	if(newFocus == nullptr) {
		currentFocus = "";
		return true;
	}
	auto iter = find_if(controls.begin(), controls.end(), [newFocus](std::pair<const std::string, cControl*> p){
		return p.second == newFocus;
	});
	if(iter == controls.end()) return false;
	if(!force && !newFocus->triggerFocusHandler(*this, iter->first, false)) return false;
	currentFocus = iter->first;
	return true;
}

cTextField* cDialog::getFocus() {
	auto iter = controls.find(currentFocus);
	if(iter == controls.end()) return nullptr;
	return dynamic_cast<cTextField*>(iter->second);
}

void cDialog::attachClickHandlers(std::function<bool(cDialog&,std::string,eKeyMod)> handler, std::vector<std::string> controls) {
	cDialog& me = *this;
	for(std::string control : controls) {
		me[control].attachClickHandler(handler);
	}
}

void cDialog::attachFocusHandlers(std::function<bool(cDialog&,std::string,bool)> handler, std::vector<std::string> controls) {
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
	if(bg == BG_DARK && dynamic_cast<cButton*>(&ctrl) != nullptr)
		labelCtrl->setColour(defTextClr);
	else labelCtrl->setColour(ctrl.getColour());
	ctrl.setLabelCtrl(labelCtrl);
	return add(labelCtrl, labelRect, key);
}

void cDialog::process_keystroke(cKey keyHit){
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		if(iter->second->isVisible() && iter->second->isClickable() && iter->second->getAttachedKey() == keyHit){
			iter->second->setActive(true);
			draw();
			if(get_bool_pref("PlaySounds", true)) {
				if(typeid(iter->second) == typeid(cLed*))
					play_sound(34);
				else play_sound(37);
				sf::sleep(time_in_ticks(6));
			}
			else sf::sleep(time_in_ticks(14));
			iter->second->setActive(false);
			draw();
			sf::sleep(sf::milliseconds(8));
			iter->second->triggerClickHandler(*this,iter->first,mod_none);
			return;
		}
		iter++;
	}
	// If you get an escape and it isn't processed, make it an enter.
	if(keyHit.spec && keyHit.k == key_esc){
		keyHit.k = key_enter;
		process_keystroke(keyHit);
	}
}

void cDialog::process_click(location where, eKeyMod mods){
	// TODO: Return list of all controls whose bounding rect contains the clicked point.
	// Then the return value of the click handler can mean "Don't pass this event on to other things below me".
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		if(iter->second->isVisible() && iter->second->isClickable() && where.in(iter->second->getBounds())){
			if(iter->second->handleClick(where))
				break;
			else return;
		}
		iter++;
	}
	if(iter != controls.end())
		iter->second->triggerClickHandler(*this,iter->first,mods);
}

xBadNode::xBadNode(std::string t, int r, int c, std::string dlg) throw() :
	type(t),
	row(r),
	col(c),
	msg(nullptr),
	dlg(dlg) {}

const char* xBadNode::what() const throw() {
	if(msg == nullptr){
		char* s = new (nothrow) char[200];
		if(s == nullptr){
			std::cerr << "Allocation of memory for error message failed, bailing out..." << std::endl;
			abort();
		}
		sprintf(s,"XML Parse Error: Unknown element %s encountered (file %s, line %d, column %d).",type.c_str(),dlg.c_str(),row,col);
		msg = s;
	}
	return msg;
}

xBadNode::~xBadNode() throw(){
	if(msg != nullptr) delete[] msg;
}

xBadAttr::xBadAttr(std::string t, std::string n, int r, int c, std::string dlg) throw() :
	type(t),
	name(n),
	row(r),
	col(c),
	msg(nullptr),
	dlg(dlg) {}

const char* xBadAttr::what() const throw() {
	if(msg == nullptr){
		char* s = new (nothrow) char[200];
		if(s == nullptr){
			std::cerr << "Allocation of memory for error message failed, bailing out..." << std::endl;
			abort();
		}
		sprintf(s,"XML Parse Error: Unknown attribute %s encountered on element %s (file %s, line %d, column %d).",name.c_str(),type.c_str(),dlg.c_str(),row,col);
		msg = s;
	}
	return msg;
}

xBadAttr::~xBadAttr() throw(){
	if(msg != nullptr) delete[] msg;
}

xMissingAttr::xMissingAttr(std::string t, std::string n, int r, int c, std::string dlg) throw() :
	type(t),
	name(n),
	row(r),
	col(c),
	msg(nullptr),
	dlg(dlg) {}

const char* xMissingAttr::what() const throw() {
	if(msg == nullptr){
		char* s = new (nothrow) char[200];
		if(s == nullptr){
			std::cerr << "Allocation of memory for error message failed, bailing out..." << std::endl;
			abort();
		}
		sprintf(s,"XML Parse Error: Required attribute %s missing on element %s (file %s, line %d, column %d).",name.c_str(),type.c_str(),dlg.c_str(),row,col);
		msg = s;
	}
	return msg;
}

xMissingAttr::~xMissingAttr() throw(){
	if(msg != nullptr) delete[] msg;
}

xMissingElem::xMissingElem(std::string p, std::string t, int r, int c, std::string dlg) throw() :
	parent(p),
	name(t),
	row(r),
	col(c),
	msg(nullptr),
	dlg(dlg) {}

const char* xMissingElem::what() const throw() {
	if(msg == nullptr){
		char* s = new (nothrow) char[200];
		if(s == nullptr){
			std::cerr << "Allocation of memory for error message failed, bailing out..." << std::endl;
			abort();
		}
		sprintf(s,"XML Parse Error: Required element %s missing in element %s (file %s, line %d, column %d).",name.c_str(),parent.c_str(),dlg.c_str(),row,col);
		msg = s;
	}
	return msg;
}

xMissingElem::~xMissingElem() throw(){
	if(msg != nullptr) delete[] msg;
}

xBadVal::xBadVal(std::string t, std::string n, std::string v, int r, int c, std::string dlg) throw() :
	type(t),
	name(n),
	val(v),
	row(r),
	col(c),
	msg(nullptr),
	dlg(dlg) {}

const char* xBadVal::what() const throw() {
	if(msg == nullptr){
		char* s = new (nothrow) char[200];
		if(s == nullptr){
			std::cerr << "Allocation of memory for error message failed, bailing out..." << std::endl;
			abort();
		}
		sprintf(s,"XML Parse Error: Invalid value %s for attribute %s encountered on element %s (file %s, line %d, column %d).",val.c_str(),name.c_str(),type.c_str(),dlg.c_str(),row,col);
		msg = s;
	}
	return msg;
}

xBadVal::~xBadVal() throw(){
	if(msg != nullptr) delete[] msg;
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
		if(iter->second->isContainer()){
			cContainer* tmp = dynamic_cast<cContainer*>(iter->second);
			if(tmp->hasChild(id))
				return tmp->getChild(id);
		}
		iter++;
	}
	throw std::invalid_argument(id + " does not exist in dialog " + fname);
}

bool cDialog::hasControl(std::string id) {
	ctrlIter iter = controls.find(id);
	if(iter != controls.end()) return true;
	
	iter = controls.begin();
	while(iter != controls.end()){
		if(iter->second->isContainer()){
			cContainer* tmp = dynamic_cast<cContainer*>(iter->second);
			if(tmp->hasChild(id))
				return true;
		}
		iter++;
	}
	return false;
}

const char*const xBadVal::CONTENT = "<content>";
