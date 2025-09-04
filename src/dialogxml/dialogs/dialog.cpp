/*
 *  dialog.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include <cmath>
#include <stdexcept>
#include <functional>
#include <sstream>
#include <string>
#include <map>
#include "dialog.hpp"
#include "gfx/tiling.hpp" // for bg
#include "fileio/resmgr/res_dialog.hpp"
#include "sounds.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "dialogxml/widgets/pict.hpp"
#include "dialogxml/widgets/button.hpp"
#include "dialogxml/widgets/field.hpp"
#include "dialogxml/widgets/ledgroup.hpp"
#include "dialogxml/widgets/line.hpp"
#include "dialogxml/widgets/message.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include "dialogxml/widgets/scrollpane.hpp"
#include "dialogxml/widgets/stack.hpp"
#include "dialogxml/widgets/tilemap.hpp"
#include "tools/keymods.hpp"
#include "tools/winutil.hpp"
#include "mathutil.hpp"
#include "tools/cursors.hpp"
#include "tools/prefs.hpp"
#include "tools/framerate_limiter.hpp"
#include "replay.hpp"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace ticpp;

const short cDialog::BG_DARK = 5, cDialog::BG_LIGHT = 16;
short cDialog::defaultBackground = cDialog::BG_DARK;
cDialog* cDialog::topWindow = nullptr;
void (*cDialog::redraw_everything)() = nullptr;
std::function<void(const cDialog&)> cDialog::onOpen;
std::function<void(const cDialog&)> cDialog::onClose;
std::function<void(sf::RenderWindow& win)> cDialog::onLostFocus;
std::function<void(sf::RenderWindow& win)> cDialog::onGainedFocus;
std::function<void(sf::RenderWindow& win)> cDialog::onHandleEvents;

extern std::map<std::string,sf::Color> colour_map;

extern bool check_for_interrupt(std::string);
extern void showError(std::string str1, cDialog* parent = nullptr);

std::string cDialog::generateId(const std::string& explicitId) const {
	return explicitId.empty() ? cControl::generateRandomString() : explicitId;
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
	}else if(colour_map.find(what) != colour_map.end()){
		return colour_map[what];
	}else throw -1;
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

cDialog::cDialog(cDialog* p) : parent(p), doAnimations(defaultDoAnimations) {}

cDialog::cDialog(const DialogDefn& file, cDialog* p) : parent(p), doAnimations(defaultDoAnimations) {
	loadFromFile(file);
}

static std::string ctrlTypeName(eControlType type) {
	std::string ctrlType;
	switch(type) {
		case CTRL_UNKNOWN: ctrlType = "???"; break;
		case CTRL_BTN: ctrlType = "button"; break;
		case CTRL_LED: ctrlType = "led"; break;
		case CTRL_LINE: ctrlType = "line"; break;
		case CTRL_PICT: ctrlType = "pict"; break;
		case CTRL_FIELD: ctrlType = "field"; break;
		case CTRL_TEXT: ctrlType = "text"; break;
		case CTRL_GROUP: ctrlType = "group"; break;
		case CTRL_STACK: ctrlType = "stack"; break;
		case CTRL_SCROLL: ctrlType = "slider"; break;
		case CTRL_PANE: ctrlType = "pane"; break;
		case CTRL_MAP: ctrlType = "tilemap"; break;
	}
	return ctrlType;
}

extern fs::path progDir;
void cDialog::loadFromFile(const DialogDefn& file){
	bg = defaultBackground;
	fname = file.id;
	try{
		const Document& xml = file.defn;
		
		Iterator<Attribute> attr;
		Iterator<Element> node;
		string type, name, val, defbtn, escbtn;
		
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
			}else if(name == "defbtn"){
				defbtn = val;
			}else if(name == "escbtn"){
				escbtn = val;
			}else if(name != "debug")
				throw xBadAttr(type,name,attr->Row(),attr->Column(),fname);
		}
		
		vector<int> specificTabs, reverseTabs;
		std::pair<std::string,cControl*> prevCtrl{"", nullptr};
		for(node = node.begin(xml.FirstChildElement()); node != node.end(); node++){
			node->GetValue(&type);
			ctrlIter inserted;
			// Yes, I'm using insert instead of [] to add elements to the map.
			// In this situation, it's actually easier that way; the reason being, the
			// map key is obtained from the name attribute of each element.
			if(type == "field") {
				auto field = parse<cTextField>(*node, *this);
				inserted = controls.insert(field).first;
				tabOrder.push_back(field);
				if(field.second->tabOrder > 0)
					specificTabs.push_back(field.second->tabOrder);
				else if(field.second->tabOrder < 0)
					reverseTabs.push_back(field.second->tabOrder);
			} else if(type == "text")
				inserted = controls.insert(parse<cTextMsg>(*node, *this)).first;
			else if(type == "pict")
				inserted = controls.insert(parse<cPict>(*node, *this)).first;
			else if(type == "slider")
				inserted = controls.insert(parse<cScrollbar>(*node, *this)).first;
			else if(type == "button")
				inserted = controls.insert(parse<cButton>(*node, *this)).first;
			else if(type == "led")
				inserted = controls.insert(parse<cLed>(*node, *this)).first;
			else if(type == "line")
				inserted = controls.insert(parse<cConnector>(*node, *this)).first;
			else if(type == "group")
				inserted = controls.insert(parse<cLedGroup>(*node, *this)).first;
			else if(type == "stack") {
				auto parsed = parse<cStack>(*node, *this);
				inserted = controls.insert(parsed).first;
				// Now, if it contains any fields, their tab order must be accounted for
				parsed.second->fillTabOrder(specificTabs, reverseTabs);
			} else if(type == "pane") {
				auto parsed = parse<cScrollPane>(*node, *this);
				inserted = controls.insert(parsed).first;
				// TODO: Now, if it contains any fields, their tab order must be accounted for
				//parsed.second->fillTabOrder(specificTabs, reverseTabs);
			} else if(type == "tilemap") {
				auto parsed = parse<cTilemap>(*node, *this);
				inserted = controls.insert(parsed).first;
				parsed.second->fillTabOrder(specificTabs, reverseTabs);
			} else throw xBadNode(type,node->Row(),node->Column(),fname);
			if(prevCtrl.second) {
				if(inserted->second->anchor == "$$prev$$" && prevCtrl.second->anchor == "$$next$$") {
					throw xBadVal(type, "anchor", "<circular dependency>", node->Row(), node->Column(), fname);
				} else if(inserted->second->anchor == "$$prev$$") {
					inserted->second->anchor = prevCtrl.first;
				} else if(prevCtrl.second->anchor == "$$next$$") {
					prevCtrl.second->anchor = inserted->first;
				}
			}
			prevCtrl = *inserted;
			// Needed to correctly resolve relative positioning
			inserted->second->recalcRect();
		}
		
		// Resolve relative positioning
		bool all_resolved = true;

		std::function<void(const std::string&, cControl&)> process_ctrl;
		process_ctrl = [this, &all_resolved, &process_ctrl](const std::string&, cControl& ctrl) {
			if(!ctrl.anchor.empty()) {
				cControl* anchor = findControl(ctrl.anchor);
				if(anchor == nullptr) {
					throw xBadVal(ctrlTypeName(ctrl.getType()), "anchor", ctrl.anchor, 0, 0, fname);
				}
				if(!anchor->anchor.empty()) {
					// Make sure it's not a loop!
					std::vector<std::string> refs{ctrl.anchor};
					while(!anchor->anchor.empty()) {
						cControl* next_anchor = findControl(anchor->anchor);
						if(next_anchor == nullptr){
							throw xBadVal(ctrlTypeName(anchor->getType()), "anchor", anchor->anchor, 0, 0, fname);
						}
						refs.push_back(anchor->anchor);
						anchor = next_anchor;
						if(std::find(refs.begin(), refs.end(), anchor->anchor) != refs.end()) {
							throw xBadVal(ctrlTypeName(ctrl.getType()), "anchor", "<circular dependency>", 0, 0, fname);
						}
					}
					all_resolved = false;
					return;
				}
				if(auto pane = dynamic_cast<cContainer*>(anchor)) {
					// If the anchor is a container, make sure to resolve its contents first.
					bool all_children_resolved = true;
					pane->forEach([&all_children_resolved](const std::string&, cControl& ctrl) {
						if(!ctrl.anchor.empty()) {
							all_children_resolved = false;
						}
					});
					if(!all_children_resolved) return;
					pane->postChildrenResolve();
				}
				ctrl.relocateRelative(ctrl.frame.topLeft(), anchor, ctrl.horz, ctrl.vert);
				ctrl.anchor.clear();
				ctrl.horz = ctrl.vert = POS_ABS;
			} else if(auto pane = dynamic_cast<cContainer*>(&ctrl)) {
				bool all_children_resolved = true;
				pane->forEach([&process_ctrl, &all_children_resolved](std::string id, cControl& ctrl) {
					if(!ctrl.anchor.empty()) {
						all_children_resolved = false;
					}
					process_ctrl(id, ctrl);
				});
				if(!all_children_resolved) return;
				pane->postChildrenResolve();
			}
		};

		do {
			all_resolved = true;
			for(auto& p : controls) {
				auto ctrl = p.second;

				process_ctrl(p.first, *ctrl);
			}
		} while(!all_resolved);
		
		// Set the default button.
		setDefaultButton(defbtn);
		// Set the escape button.
		setEscapeButton(escbtn);
		
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
	recalcRect();
	currentFocus = "";
	for(ctrlIter iter = controls.begin(); iter != controls.end(); iter++){
		if(typeid(iter->second) == typeid(cTextField*)){
			currentFocus = iter->first;
			break;
		}
		if(iter->second->isContainer()){
			cContainer* tmp = dynamic_cast<cContainer*>(iter->second);
			tmp->forEach([this, iter](std::string, cControl& child) {
				if(typeid(&child) == typeid(cTextField*)) {
					if(currentFocus.empty()) currentFocus = iter->first;
				}
			});
		}
	}
}

void cDialog::recalcRect(){
	bool haveRel = false;
	winRect = rectangle();
	for(ctrlIter iter = controls.begin(); iter != controls.end(); iter++) {
		using namespace std::placeholders;
		if(auto container = dynamic_cast<cContainer*>(iter->second))
			container->forEach(std::bind(&cControl::recalcRect, _2));
		iter->second->recalcRect();
		rectangle frame = iter->second->getBounds();
		haveRel = haveRel || iter->second->horz != POS_ABS || iter->second->vert != POS_ABS;
		if(iter->second->horz != POS_REL_NEG && frame.right > winRect.right)
			winRect.right = frame.right;
		if(iter->second->vert != POS_REL_NEG && frame.bottom > winRect.bottom)
			winRect.bottom = frame.bottom;
	}
	winRect.right += 6;
	winRect.bottom += 6;
	if(haveRel) {
		// Resolve any remaining relative positions
		// Controls placed relative to the dialog's edges can go off the edge of the dialog
		for(ctrlIter iter = controls.begin(); iter != controls.end(); iter++) {
			location pos = iter->second->getBounds().topLeft();
			if(iter->second->horz == POS_REL_NEG)
				pos.x = winRect.right - pos.x;
			if(iter->second->vert == POS_REL_NEG)
				pos.y = winRect.bottom - pos.y;
			iter->second->horz = iter->second->vert = POS_ABS;
			iter->second->relocate(pos);
		}
	}

	winRect.right *= get_ui_scale();
	winRect.bottom *= get_ui_scale();
	// Uncomment if you need to measure any dialogs.
	/*
	LOG_VALUE(fname);
	LOG_VALUE(winRect.right);
	LOG_VALUE(winRect.bottom);
	//*/
}

bool cDialog::initCalled = false;

void cDialog::init(){
	cButton::init();
	cLed::init();
	cPict::init();
	cScrollbar::init();
	initCalled = true;
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
	what->setName(key);
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
	dynamic_cast<cTextField&>(topWindow->getControl(field)).handleInput(key);
	return true;
}

void cDialog::run(std::function<void(cDialog&)> onopen){
	cPict::resetAnim();
	cDialog* formerTop = topWindow;

	sf::RenderWindow* parentWin = &(parent ? parent->win : mainPtr());
	auto parentPos = parentWin->getPosition();
	auto parentSz = parentWin->getSize();
	cursor_type former_curs = Cursor::current;
	dialogNotToast = true;
	set_cursor(sword_curs);
	sf::Event currentEvent;
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
	// Make sure the requested size isn't insane.
	auto desktop = sf::VideoMode::getDesktopMode();
	if(winRect.width() > desktop.width * 1.5 || winRect.height() > desktop.height * 1.5) {
		throw std::string("Dialog ") + fname + std::string(" requested a crazy window size of ") + std::to_string(winRect.width()) + "x" + std::to_string(winRect.height());
	}
	// Sometimes it seems like the Cocoa menu handling clobbers the active rendering context.
	// For whatever reason, delaying 100 milliseconds appears to fix this.
	sf::sleep(sf::milliseconds(100));
	// So this little section of code is a real-life valley of dying things.
	// Instantiating a window and then closing it seems to fix the update error, because magic.
	win.create(sf::VideoMode(1,1),"");
	win.close();
	win.create(sf::VideoMode(winRect.width(), winRect.height()), "Dialog", sf::Style::Titlebar);
	winLastX = parentPos.x + (int(parentSz.x) - winRect.width()) / 2;
	winLastY = parentPos.y + (int(parentSz.y) - winRect.height()) / 2;
	win.setPosition({winLastX, winLastY});
	draw();
	stackWindowsCorrectly();
	// This is a loose modal session, as it doesn't prevent you from clicking away,
	// but it does prevent editing other dialogs, and it also keeps this window on top
	// even when it loses focus.
	ModalSession dlog(win, *parentWin);
	animTimer.restart();

	has_focus = true;
	topWindow = this;

	// Run the static onOpen event first
	if(cDialog::onOpen) cDialog::onOpen(*this);
	// Run this dialog's onOpen event
	if(onopen) onopen(*this);

	handle_events();

	win.setVisible(false);
	// Flush events on parent window from while this one was running
	while(pollEvent(parentWin, currentEvent));
	set_cursor(former_curs);
	topWindow = formerTop;
	stackWindowsCorrectly();
	if(cDialog::onClose) cDialog::onClose(*this);
}

void cDialog::runWithHelp(short help1, short help2, bool help_forced) {
	using namespace std::placeholders;
	extern void give_help(short help1, short help2, cDialog& parent, bool help_forced);
	run(std::bind(&give_help,help1, help2, _1, help_forced));
}

// This method is a main event event loop of the dialog.
void cDialog::handle_events() {
	sf::Event currentEvent;
	cFramerateLimiter fps_limiter;

	while(dialogNotToast) {
		if(replaying && fname != "confirm-interrupt-replay" && check_for_interrupt("confirm-interrupt-replay")){
			replaying = false;
			return;
		}else if(replaying && has_next_action("click_control")){
			Element& next_action = pop_next_action();
			auto info = info_from_action(next_action);
			if(info["id"].empty()) continue;
			eKeyMod mods = static_cast<eKeyMod>(std::stoi(info["mods"]));
			controls[info["id"]]->triggerClickHandler(*this, info["id"], mods);
		}else if(replaying && has_next_action("field_input")){
			Element& next_action = pop_next_action();
			cKey key = key_from_action(next_action);
			dynamic_cast<cTextField&>(getControl(currentFocus)).handleInput(key, true);
		}else if(replaying && has_next_action("handleTab")){
			Element& next_action = pop_next_action();
			handleTab(str_to_bool(next_action.GetText()));
		}else if(replaying && has_next_action("field_focus")) {
			Element& next_action = pop_next_action();
			setFocus(&(dynamic_cast<cTextField&>(getControl(next_action.GetText()))));
		}else if(replaying && has_next_action("field_selection")) {
			cTextField& text_field = dynamic_cast<cTextField&>(getControl(currentFocus));
			text_field.replay_selection(pop_next_action());
		}else if(replaying && has_next_action("scrollbar_setPosition")){
			Element& next_action = pop_next_action();
			auto info = info_from_action(next_action);
			std::string name = info["name"];
			long newPos = boost::lexical_cast<long>(info["newPos"]);

			cScrollPane& pane = dynamic_cast<cScrollPane&>(getControl(name));
			pane.getScroll().setPosition(newPos);
		}else if(replaying && has_next_action("error")){
			// The error is recorded for debugging only. It should be triggered by replaying the actions.
			pop_next_action();
		}else if(replaying && has_next_action()){
			replaying = false;
			std::string type =  next_action_type();
			std::string line =  std::to_string(next_action_line());
			throw std::string { "Replaying a dialog, have the wrong replay action: " + type + " on line " + line};
		}else{
			if(has_focus && onHandleEvents) onHandleEvents(win);
			while(pollEvent(win, currentEvent)){
				handle_one_event(currentEvent, fps_limiter);
			}
		}

		// Ideally, this should be the only draw call that is done in a cycle.
		draw();

		// Prevent the loop from executing too fast.
		fps_limiter.frame_finished();
	}
}

void cDialog::handleTab(bool reverse) {
	if(recording){
		record_action("handleTab", bool_to_str(reverse));
	}
	if(reverse) {
		handleTabOrder(currentFocus, tabOrder.rbegin(), tabOrder.rend());
	} else {
		handleTabOrder(currentFocus, tabOrder.begin(), tabOrder.end());
	}
}

void cDialog::stackWindowsCorrectly() {
	// Put all dialogs in correct z order:
	std::vector<cDialog*> dialog_stack;
	cDialog* next = this;
	while(next != nullptr){
		dialog_stack.push_back(next);
		next = next->parent;
	}
	makeFrontWindow(mainPtr());
	for(int i = dialog_stack.size() - 1; i >= 0; --i){
		if(dialog_stack[i]->dialogNotToast){
			makeFrontWindow(dialog_stack[i]->win);
		}
	}
}

cKey translate_sfml_key(sf::Event::KeyEvent key_event) {
	using Key = sf::Keyboard::Key;

	cKey key;
	switch(key_event.code){
		case Key::Up:
			key.spec = true;
			key.k = key_up;
			break;
		case Key::Right:
			key.spec = true;
			key.k = key_right;
			break;
		case Key::Left:
			key.spec = true;
			key.k = key_left;
			break;
		case Key::Down:
			key.spec = true;
			key.k = key_down;
			break;
		case Key::Escape:
			key.spec = true;
			key.k = key_esc;
			break;
		case Key::Return: // TODO: Also enter (keypad)
			key.spec = true;
			key.k = key_enter;
			break;
		case Key::BackSpace:
			key.spec = true;
			key.k = key_bsp;
			break;
		case Key::Delete:
			key.spec = true;
			key.k = key_del;
			break;
		case Key::Tab:
			key.spec = true;
			key.k = key_tab;
			break;
		case Key::Insert:
			key.spec = true;
			key.k = key_insert;
			break;
		case Key::F1:
			key.spec = true;
			key.k = key_help;
			break;
		case Key::Home:
			key.spec = true;
			key.k = key_home;
			break;
		case Key::End:
			key.spec = true;
			key.k = key_end;
			break;
		case Key::PageUp:
			key.spec = true;
			key.k = key_pgup;
			break;
		case Key::PageDown:
			key.spec = true;
			key.k = key_pgdn;
			break;
		default:
			key.spec = false;
			key.c = keyToChar(key_event.code, false);
			break;
	}
	key.mod = mod_none;
	if(key_event.*systemKey)
		key.mod += mod_ctrl;
	if(key_event.shift) key.mod += mod_shift;
	if(key_event.alt) key.mod += mod_alt;
	return key;
}

// This method handles one event received by the dialog.
void cDialog::handle_one_event(const sf::Event& currentEvent, cFramerateLimiter& fps_limiter) {

	// HACK: This needs to be stored between consecutive invocations of this function
	static cKey pendingKey = {true};
	std::string itemHit = "";
	location where;
	cKey key;
	switch(currentEvent.type) {
		case sf::Event::KeyPressed:
			key = translate_sfml_key(currentEvent.key);
			
			// Handles button hotkeys. Note that the event still falls through to text fields.
			// It probably shouldn't, because right now we have to be careful not to put a button
			// on the same dialog as a field if its hotkey is a typable character.
			process_keystroke(key);
			// Now check for focused fields.
			if(currentFocus.empty()) break;
			// If it's a tab, handle tab order
			if(key.spec && key.k == key_tab) {
				// Could use key.mod, but this is slightly easier.
				handleTab(currentEvent.key.shift);
			} else {
				// If it's a character key, and the system key (control/command) is not pressed,
				// we have an upcoming TextEntered event which contains more information.
				// Otherwise, handle it right away. But never handle enter or escape.
				if((key.spec && key.k != key_enter && key.k != key_esc) || mod_contains(key.mod, mod_ctrl))
					dynamic_cast<cTextField&>(getControl(currentFocus)).handleInput(key, true);
				pendingKey = key;
				if(key.k != key_enter && key.k != key_esc) itemHit = "";
			}
			break;
		case sf::Event::TextEntered:
			if(!pendingKey.spec && !currentFocus.empty()) {
				pendingKey.c = currentEvent.text.unicode;
				if(pendingKey.c != '\t')
					dynamic_cast<cTextField&>(getControl(currentFocus)).handleInput(pendingKey, true);
			}
			break;
		case sf::Event::MouseButtonPressed:
			key.mod = current_key_mod();
			where = {(int)(currentEvent.mouseButton.x / get_ui_scale()), (int)(currentEvent.mouseButton.y / get_ui_scale())};
			process_click(where, key.mod, fps_limiter);
			break;
		case sf::Event::LostFocus:
			has_focus = false;
			if(onLostFocus){
				onLostFocus(win);
			}
			break;
		case sf::Event::GainedFocus:
			if(!has_focus){
				has_focus = true;
				if(onGainedFocus){
					onGainedFocus(win);
				}
				stackWindowsCorrectly();
			}
			BOOST_FALLTHROUGH;
		case sf::Event::MouseMoved:{
			int x = currentEvent.mouseMove.x / get_ui_scale();
			int y = currentEvent.mouseMove.y / get_ui_scale();
			// Did the window move, potentially dirtying the canvas below it?
			if(check_window_moved(win, winLastX, winLastY))
				if (redraw_everything != NULL)
					redraw_everything();

			bool inField = false;
			for(auto& ctrl : controls) {
				if(ctrl.second->getType() == CTRL_FIELD && ctrl.second->getBounds().contains(x, y)) {
					set_cursor(text_curs);
					inField = true;
					break;
				}
			}
			if(!inField) set_cursor(sword_curs);
		}break;
		default: // To silence warning of unhandled enum values
			break;
	}
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

short cDialog::getBg() const {
	return bg;
}

void cDialog::setDefTextClr(sf::Color clr){
	defTextClr = clr;
}

sf::Color cDialog::getDefTextClr() const {
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

void cDialog::untoast(bool triggerFocusHandler) {
	dialogNotToast = true;
	if(!currentFocus.empty() && triggerFocusHandler)
		this->getControl(currentFocus).triggerFocusHandler(*this, currentFocus, false);
}

bool cDialog::accepted() const {
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
	for(auto iter = controls.begin(); iter != controls.end(); iter++) {
		if(iter->second == newFocus) {
			if (!force && !newFocus->triggerFocusHandler(*this, iter->first, false)) return false;
			currentFocus = iter->first;
			return true;
		}
		if(iter->second->isContainer()) {
			cContainer* tmp = dynamic_cast<cContainer*>(iter->second);
			std::string foundKey = "";
			tmp->forEach([newFocus, &foundKey](std::string key, cControl& child) {
				if (&child == newFocus) {
					foundKey = key;
				}
			});
			if(!foundKey.empty()) {
				if(!force && !newFocus->triggerFocusHandler(*this, foundKey, false)) return false;
				currentFocus = foundKey;
				return true;
			}
		}
	}
	return false;
}

cTextField* cDialog::getFocus() {
	if (currentFocus.empty()) return nullptr;
	return dynamic_cast<cTextField*>(&getControl(currentFocus));
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
	bool enterKeyHit = keyHit.spec && keyHit.k == key_enter;
	bool escapeKeyHit = keyHit.spec && keyHit.k == key_esc;
	while(iter != controls.end()){
		cControl* ctrl = iter->second;
		if(ctrl->isVisible()){
			if(ctrl->isClickable() &&
				(ctrl->getAttachedKey() == keyHit || (ctrl->isDefault() && enterKeyHit) || (ctrl->isEscape() && escapeKeyHit))){

				ctrl->handleKeyTriggered(*this);
				return;
			}
			if(ctrl->isContainer()){
				cContainer* container = dynamic_cast<cContainer*>(ctrl);
				std::string child_hit;
				container->forEach([&keyHit, &child_hit, enterKeyHit](std::string child_id, cControl& child_ctrl) {
					if(child_ctrl.isVisible() && child_ctrl.isClickable() &&
						(child_ctrl.getAttachedKey() == keyHit || (child_ctrl.isDefault() && enterKeyHit))){
						
						if(child_hit.empty()) child_hit = child_id;
					}
				});
				if(!child_hit.empty()) {
					findControl(child_hit)->handleKeyTriggered(*this);
					return;
				}
			}

		}
		iter++;
	}
}

void cDialog::process_click(location where, eKeyMod mods, cFramerateLimiter& fps_limiter){
	// TODO: Return list of all controls whose bounding rect contains the clicked point.
	// Then the return value of the click handler can mean "Don't pass this event on to other things below me".
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		if(iter->second->isVisible() && iter->second->isClickable() && where.in(iter->second->getBounds())){
			if(iter->second->handleClick(where, fps_limiter))
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

bool cDialog::defaultDoAnimations = false;

void cDialog::draw(){
	win.setActive(false);
	tileImage(win,winRect,::bg[bg]);
	if(doAnimations && animTimer.getElapsedTime().asMilliseconds() >= (1000 / anim_pict_fps)) {
		cPict::advanceAnim();
		animTimer.restart();
	}
	
	// Scale dialogs:
	sf::View view = win.getDefaultView();
	view.setViewport(sf::FloatRect(0, 0, get_ui_scale(), get_ui_scale()));
	win.setView(view);
	
	ctrlIter iter = controls.begin();
	while(iter != controls.end()){
		iter->second->draw();
		iter++;
	}
	
	win.setActive();
	win.display();
}

cControl& cDialog::operator[](std::string id){
	return getControl(id);
}

const cControl& cDialog::operator[](std::string id) const {
	return const_cast<cDialog&>(*this).getControl(id);
}

cControl* cDialog::findControl(std::string id, cContainer* parent) {
	cControl* found = nullptr;
	auto fcn = [this, &found, id](std::string check_id, cControl& ctrl) {
		if(found) return;
		if(id == check_id) found = &ctrl;
		else if(ctrl.isContainer()) {
			found = findControl(id, dynamic_cast<cContainer*>(&ctrl));
		}
	};
	if(parent) parent->forEach(fcn);
	else forEach(fcn);
	return found;
}

cControl& cDialog::getControl(std::string id) {
	if(auto ctrl = findControl(id))
		return *ctrl;
	throw std::invalid_argument(id + " does not exist in dialog " + fname);
}

bool cDialog::hasControl(std::string id) const {
	auto iter = controls.find(id);
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

void cDialog::setSpecialButton(std::string& name_ref, std::string name, bool escape) {
	if(!name.empty() && !hasControl(name)){
		// this is likely because the dialogxml is malformed. maybe the linter already checks this,
		// but the engine might as well also.
		throw std::string { "Requested button does not exist: " } + name;
	}
	if(!name_ref.empty()){
		if(escape){
			getControl(name_ref).setEscape(false);
		}else{
			getControl(name_ref).setDefault(false);
		}
		name_ref = "";
	}
	if(!name.empty()){
		name_ref = name;
		if(escape){
			getControl(name).setEscape(true);
		}else{
			getControl(name).setDefault(true);
		}
	}
}

void cDialog::setDefaultButton(std::string defbtn) {
	setSpecialButton(defaultButton, defbtn, false);
}

void cDialog::setEscapeButton(std::string escbtn) {
	setSpecialButton(escapeButton, escbtn, true);
}

const char*const xBadVal::CONTENT = "$content$";

cDialogIterator::cDialogIterator() : parent(nullptr) {}

cDialogIterator::cDialogIterator(cDialog* parent) : parent(parent), current(parent->controls.begin()) {}

std::pair<std::string, cControl*>& cDialogIterator::dereference() const {
	// Ugly fugly... especially the reinterpret_cast...
	using value_type = std::pair<std::string, cControl*>;
	if(children.empty()) return *reinterpret_cast<value_type*>(&*current);
	return const_cast<value_type&>(children.front());
}

bool cDialogIterator::equal(const cDialogIterator& other) const {
	if(parent == nullptr) return other.parent == nullptr;
	return parent == other.parent && current == other.current && std::equal(children.begin(), children.end(), other.children.begin());
}

void cDialogIterator::increment() {
	if(children.empty()) {
		if(current->second->isContainer()) {
			cContainer& box = dynamic_cast<cContainer&>(*current->second);
			std::vector<std::pair<std::string, cControl*>> newChildren;
			box.forEach([&newChildren](std::string id, cControl& ctrl) {
				newChildren.emplace_back(id, &ctrl);
			});
			children.insert(children.begin(), newChildren.begin(), newChildren.end());
		}
		++current;
	} else {
		children.pop_front();
	}
	if(children.empty() && current == parent->controls.end()) {
		parent = nullptr;
	}
}

void preview_dialog_xml(fs::path dialog_xml) {
	try{
		std::unique_ptr<DialogDefn> defn(load_dialog_defn(dialog_xml));
		cDialog dialog(*defn);

		// Make every clickable control's click event close the dialog
		for (auto control : dialog){
			try{
				control.second->attachClickHandler([](cDialog& me, std::string, eKeyMod) -> bool {
					me.toast(false);
					return true;
				});
			}catch(...){}
		}
		dialog.run();
	}catch(std::exception& x) {
		showError(x.what());
	}catch(std::string& x){
		showError(x);
	}
}

sf::Color cParentless::getDefTextClr() const {
	return cDialog::defaultBackground == cDialog::BG_DARK ? sf::Color::White : sf::Color::Black;
}

void setup_dialog_pict_anim(cDialog& dialog, std::string pict_id, short anim_loops, short anim_fps) {
	cPict& pict = dynamic_cast<cPict&>(dialog[pict_id]);
	pict.setAnimLoops(anim_loops);
	dialog.setAnimPictFPS(anim_fps);
	dialog.setDoAnimations(true);
}
