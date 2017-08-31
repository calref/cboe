/*
 *  field.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "field.hpp"
#include <sstream>
#include <map>
#include <numeric>
#include <boost/lexical_cast.hpp>
#include "dialog.hpp"
#include "strdlog.hpp"
#include "render_shapes.hpp"
#include "winutil.hpp"
#include "cursors.hpp"

bool cTextField::callHandler(event_fcn<EVT_DEFOCUS>::type onFocus, cDialog& me, std::string id) {
	if(field_type != FLD_TEXT) {
		try {
			std::string contents = getText();
			switch(field_type) {
				case FLD_TEXT: break;
				case FLD_INT:
					boost::lexical_cast<long long>(contents);
					break;
				case FLD_UINT:
					boost::lexical_cast<unsigned long long>(contents);
					break;
				case FLD_REAL:
					boost::lexical_cast<long double>(contents);
					break;
			}
		} catch(boost::bad_lexical_cast) {
			static const std::map<const eFldType, const std::string> typeNames = {
				{FLD_INT, "an integer"},
				{FLD_UINT, "a non-negative integer"},
				{FLD_REAL, "a number"},
			};
			showError("You need to enter " + typeNames.at(field_type) + "!","",parent);
			return false;
		}
	}
	bool passed = true;
	if(onFocus) passed = onFocus(me,id);
	if(passed) haveFocus = false;
	if(haveFocus && insertionPoint < 0)
		insertionPoint = getText().length();
	return passed;
}

void cTextField::callHandler(event_fcn<EVT_FOCUS>::type onFocus, cDialog& me, std::string id) {
	if(onFocus) onFocus(me,id);
	haveFocus = true;
	if(insertionPoint < 0)
		insertionPoint = getText().length();
}

void cTextField::setText(std::string to) {
	cControl::setText(to);
	if(haveFocus)
		insertionPoint = to.length();
	else insertionPoint = -1;
	selectionPoint = 0;
}

void cTextField::set_ip(location clickLoc, int cTextField::* insertionPoint) {
	TextStyle style;
	style.font = FONT_PLAIN;
	style.pointSize = 12;
	style.colour = sf::Color::Black;
	style.lineHeight = 16;
	size_t foundSnippet = snippets.size();
	// Find snippet clicked.
	for(size_t i = 0; i < snippets.size(); i++) {
		short h, w = string_length(snippets[i].text, style, &h);
		rectangle snipRect;
		snipRect.top = snippets[i].at.y;
		snipRect.left = snippets[i].at.x;
		snipRect.width() = w;
		snipRect.height() = h;
		if(snipRect.contains(clickLoc)) {
			foundSnippet = i;
			break;
		}
	}
	if(foundSnippet < snippets.size()) {
		sf::Text snippet;
		style.applyTo(snippet);
		snippet.setString(snippets[foundSnippet].text);
		snippet.setPosition(snippets[foundSnippet].at);
		size_t charClicked = snippets[foundSnippet].text.length();
		// Find character clicked. By now we know the Y position is okay, so just check X.
		if(clickLoc.x <= snippet.findCharacterPos(0).x)
			charClicked = 0;
		else for(size_t i = 0; i < snippets[foundSnippet].text.length(); i++) {
			if(clickLoc.x > snippet.findCharacterPos(i).x) charClicked = i;
			else break;
		}
		if(charClicked < snippets[foundSnippet].text.length()) {
			size_t pre_ip = std::accumulate(snippets.begin(), snippets.begin() + foundSnippet, 0, [](size_t sum, snippet_t& next) -> size_t {
				return sum + next.text.length();
			});
			int left = snippet.findCharacterPos(charClicked).x;
			int right;
			if(charClicked + 1 == snippets[foundSnippet].text.length())
				right = rectangle(snippet.getGlobalBounds()).right;
			else right = snippet.findCharacterPos(charClicked + 1).x;
			left = clickLoc.x - left;
			right -= clickLoc.x;
			if(left < right) this->*insertionPoint = pre_ip + charClicked;
			else this->*insertionPoint = pre_ip + charClicked + 1;
		}
	}
}

bool cTextField::handleClick(location clickLoc) {
	if(!haveFocus && parent && !parent->setFocus(this)) return true;
	haveFocus = true;
	redraw(); // This ensures the snippets array is populated.
	std::string contents = getText();
	bool hadSelection = selectionPoint != insertionPoint;
	bool is_double = click_timer.getElapsedTime().asMilliseconds() < 500;
	click_timer.restart();
	bool is_shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
	set_ip(clickLoc, is_shift ? &cTextField::selectionPoint : &cTextField::insertionPoint);
	if(!is_shift) selectionPoint = insertionPoint;
	if(is_double && !is_shift && !hadSelection) {
		cKey key = {true, key_word_right, mod_none};
		if(insertionPoint < contents.size() && contents[insertionPoint] != ' ')
			handleInput(key);
		key.k = key_word_left;
		key.mod += mod_shift;
		handleInput(key);
	}
	bool done = false;
	sf::Event e;
	int initial_ip = insertionPoint, initial_sp = selectionPoint;
	while(!done) {
		redraw();
		if(!inWindow->pollEvent(e)) continue;
		if(e.type == sf::Event::MouseButtonReleased){
			done = true;
		} else if(e.type == sf::Event::MouseMoved){
			restore_cursor();
			location newLoc(e.mouseMove.x, e.mouseMove.y);
			set_ip(newLoc, &cTextField::selectionPoint);
			if(is_double) {
				if(selectionPoint > initial_ip) {
					insertionPoint = initial_sp;
					while(selectionPoint < contents.length() && contents[selectionPoint] != ' ')
						selectionPoint++;
				} else {
					insertionPoint = initial_ip;
					while(selectionPoint > 0 && contents[selectionPoint - 1] != ' ')
						selectionPoint--;
				}
			}
		}
	}
	redraw();
	return true;
}

void cTextField::setFormat(eFormat prop, short) throw(xUnsupportedProp){
	throw xUnsupportedProp(prop);
}

short cTextField::getFormat(eFormat prop) throw(xUnsupportedProp){
	throw xUnsupportedProp(prop);
}

void cTextField::setColour(sf::Color clr) throw(xUnsupportedProp) {
	color = clr;
}

sf::Color cTextField::getColour() throw(xUnsupportedProp) {
	return color;
}

eFldType cTextField::getInputType() {
	return field_type;
}

void cTextField::setInputType(eFldType type) {
	field_type = type;
}

bool cTextField::isClickable(){
	return true;
}

bool cTextField::isFocusable(){
	return true;
}

bool cTextField::isScrollable(){
	return false;
}

bool cTextField::hasFocus() {
	return haveFocus;
}

cTextField::cTextField(cDialog& parent) :
		cControl(CTRL_FIELD,parent),
		color(sf::Color::Black),
		insertionPoint(-1),
		selectionPoint(0),
		haveFocus(false),
		field_type(FLD_TEXT) {}

cTextField::~cTextField(){}

void cTextField::draw(){
	if(!visible) return;
	static const sf::Color hiliteClr = {127,127,127}, ipClr = {92, 92, 92};
	inWindow->setActive();
	rectangle outline = frame;
	outline.inset(-2,-2);
	fill_rect(*inWindow, outline, sf::Color::White);
	frame_rect(*inWindow, outline, sf::Color::Black);
	std::string contents = getText();
	TextStyle style;
	style.font = FONT_PLAIN;
	style.pointSize = 12;
	style.colour = sf::Color::Black;
	style.lineHeight = 16;
	size_t ip_offset = 0;
	hilite_t hilite = {insertionPoint, selectionPoint};
	if(selectionPoint < insertionPoint) std::swap(hilite.first,hilite.second);
	if(haveFocus && contents.length() >= 1 && changeMade) {
		text_rect = frame;
		text_rect.inset(2,2);
		// Determine which line the insertion and selection points are on
		clip_rect(*inWindow, {0,0,0,0}); // To prevent drawing
		hilite_t tmp_hilite = hilite;
		// Manipulate this to ensure that there is a hilited area
		std::string dummy_str = contents + "  ";
		if(tmp_hilite.first >= tmp_hilite.second)
			tmp_hilite.second = tmp_hilite.first + 1;
		std::vector<rectangle> rects = draw_string_hilite(*inWindow, text_rect, dummy_str, style, {tmp_hilite}, {0,0,0});
		if(!rects.empty()) {
			// We only care about the first and last rects. Furthermore, we only really need one point
			location ip_pos = rects[0].centre(), sp_pos = rects[rects.size() - 1].centre();
			if(selectionPoint < insertionPoint) std::swap(ip_pos, sp_pos);
			// Prioritize selection point being visible. If possible, also keep insertion point visible.
			// We do this by first ensuring the insertion point is visible, then doing the same
			// for the selection point.
			while(!ip_pos.in(frame)) {
				text_rect.offset(0,-14);
				ip_pos.y -= 14;
				sp_pos.y -= 14;
			}
			while(!sp_pos.in(frame)) {
				int shift = selectionPoint < insertionPoint ? 14 : -14;
				text_rect.offset(0,shift);
				ip_pos.y += shift;
				sp_pos.y += shift;
			}
		}
		undo_clip(*inWindow);
		changeMade = false;
	}
	clip_rect(*inWindow, frame);
	ip_col = ip_row = -1;
	if(haveFocus) {
		snippets = draw_string_sel(*inWindow, text_rect, contents, style, {hilite}, hiliteClr);
		int iSnippet = -1, sum = 0;
		ip_offset = insertionPoint;
		for(size_t i = 0; i < snippets.size(); i++) {
			size_t snippet_len = snippets[i].text.length();
			sum += snippet_len;
			if(sum >= insertionPoint) {
				iSnippet = i;
				break;
			}
			ip_offset -= snippet_len;
		}
		std::string pre_ip = iSnippet >= 0 ? snippets[iSnippet].text.substr(0, ip_offset) : "";
		ip_offset = string_length(pre_ip, style);
		if(ip_timer.getElapsedTime().asMilliseconds() < 500) {
//			printf("Blink on (%d); ", ip_timer.getElapsedTime().asMilliseconds());
			rectangle ipRect = {0, 0, 15, 1};
			if(iSnippet >= 0)
				ipRect.offset(snippets[iSnippet].at.x + ip_offset, snippets[iSnippet].at.y + 1);
			else ipRect.offset(frame.topLeft()), ipRect.offset(3,2);
			fill_rect(*inWindow, ipRect, ipClr);
		} else if(ip_timer.getElapsedTime().asMilliseconds() > 1000) {
//			printf("Blink off (%d); ", ip_timer.getElapsedTime().asMilliseconds());
			ip_timer.restart();
		}
		// Record it so that we can calculate up/down arrow key results
		ip_col = ip_offset;
		ip_row = iSnippet;
	} else {
		rectangle text_rect = frame;
		text_rect.inset(2,2);
		win_draw_string(*inWindow, text_rect, contents, eTextMode::WRAP, style);
	}
	undo_clip(*inWindow);
}

static cKey divineFunction(cKey key) {
	/* Summary of platform-dependent navigation/edit keys:
	 Function	|		Mac			|	Windows
	 -----------+-------------------+-------------
	 Home		|	Home, Cmd-Left	|	Home
	 End		|	End, Cmd-Right	|	End
	 PgUp		|	PgUp, Alt-Up	|	PgUp
	 PgDn		|	PgDn, Alt-Down	|	PgDn
	 To Top		|	Cmd-Up, Cmd-Home|	Ctrl-Up, Ctrl-Home
	 To Bottom	| Cmd-Down, Cmd-End	|	Ctrl-Down, Ctrl-End
	 Word Left	|		Alt-Left	|	Ctrl-Left
	 Word Right	|		Alt-Right	|	Ctrl-Right
	 Word Del	|		Alt-Delete	|	Ctrl-Delete
	 Word Back	|	Alt-Backspace	|	Ctrl-Backspace
	 -----------+-------------------+----------------------
	 Cut		| Cmd-X, Shift-Del	|	Ctrl-X, Shift-Del
	 Copy		|	Cmd-C, Cmd-Ins	|	Ctrl-C, Ctrl-Ins
	 Paste		| Cmd-V, Shift-Ins	|	Ctrl-V, Shift-Ins
	 Select All	|		Cmd-A		|	Ctrl-A
	 Undo		|		Cmd-Z		|	Ctrl-Z
	 Redo		| Cmd-Y, Shift-Cmd-Z|	Ctrl-Y, Shift-Ctrl-Z
	 This is done to more closely emulate native Mac behaviour.
	 The Insert and Shift-Delete combos are included to more closely emulate
	 native Windows behaviour.
	 */
	if(!key.spec) {
		if(mod_contains(key.mod, mod_ctrl)) {
			if(key.c == 'c') {
				key.spec = true;
				key.k = key_copy;
			} else if(key.c == 'x') {
				key.spec = true;
				key.k = key_cut;
			} else if(key.c == 'v') {
				key.spec = true;
				key.k = key_paste;
			} else if(key.c == 'a') {
				key.spec = true;
				key.k = key_selectall;
			} else if(key.c == 'z') {
				key.spec = true;
				if(mod_contains(key.mod, mod_shift)) {
					key.k = key_redo;
					key.mod -= mod_shift;
				} else key.k = key_undo;
			} else if(key.c == 'y') {
				key.spec = true;
				key.k = key_redo;
			}
		}
		if(key.spec) key.mod -= mod_ctrl;
	} else {
		eSpecKey former = key.k;
		if(mod_contains(key.mod, mod_ctrl)) {
			key.mod -= mod_ctrl;
#ifdef __APPLE__
			if(key.k == key_left) key.k = key_home;
			else if(key.k == key_right) key.k = key_end;
#else
			if(key.k == key_left) key.k = key_word_left;
			else if(key.k == key_right) key.k = key_word_right;
#endif
			else if(key.k == key_up) key.k = key_top;
			else if(key.k == key_down) key.k = key_bottom;
			else if(key.k == key_home) key.k = key_top;
			else if(key.k == key_end) key.k = key_bottom;
			else if(key.k == key_insert) key.k = key_copy;
#ifndef __APPLE__
			else if(key.k == key_del) key.k = key_word_del;
			else if(key.k == key_bsp) key.k = key_word_bsp;
#else
		} else if(mod_contains(key.mod, mod_alt)) {
			key.mod -= mod_alt;
			if(key.k == key_up) key.k = key_pgup;
			else if(key.k == key_down) key.k = key_pgdn;
			else if(key.k == key_left) key.k = key_word_left;
			else if(key.k == key_right) key.k = key_word_right;
			else if(key.k == key_del) key.k = key_word_del;
			else if(key.k == key_bsp) key.k = key_word_bsp;
#endif
		} else if(mod_contains(key.mod, mod_shift)) {
			key.mod -= mod_shift;
			if(key.k == key_insert) key.k = key_paste;
			else if(key.k == key_del) key.k = key_cut;
		}
		if(key.k != former) key.mod -= mod_ctrl;
	}
	return key;
}

void cTextField::handleInput(cKey key) {
	changeMade = true;
	bool select = mod_contains(key.mod, mod_shift);
	bool haveSelection = insertionPoint != selectionPoint;
	key = divineFunction(key);
	TextStyle style;
	style.font = FONT_PLAIN;
	style.pointSize = 12;
	size_t new_ip;
	std::string contents = getText();
	if(current_action && hist_timer.getElapsedTime().asSeconds() > 5.0f)
		history.add(current_action), current_action.reset();
	hist_timer.restart();
	if(!key.spec) {
		if(haveSelection) {
			cKey deleteKey = key;
			deleteKey.spec = true;
			deleteKey.k = key_bsp;
			handleInput(deleteKey);
			contents = getText();
		}
		if(aTextInsert* ins = dynamic_cast<aTextInsert*>(current_action.get()))
			ins->append(key.c);
		else {
			if(current_action) history.add(current_action);
			aTextInsert* new_ins = new aTextInsert(*this, insertionPoint);
			new_ins->append(key.c);
			current_action.reset(new_ins);
		}
		contents.insert(contents.begin() + insertionPoint, char(key.c));
		selectionPoint = ++insertionPoint;
	} else switch(key.k) {
		case key_enter: break; // Shouldn't be receiving this anyway
			// TODO: Implement all the other special keys
		case key_left: case key_word_left:
			if(current_action) history.add(current_action), current_action.reset();
			if(haveSelection && !select) {
				selectionPoint = insertionPoint = std::min(selectionPoint,insertionPoint);
				break;
			}
			new_ip = select ? selectionPoint : insertionPoint;
			if(new_ip == 0) break;
			if(key.k == key_word_left) {
				new_ip--;
				while(new_ip > 0 && contents[new_ip - 1] != ' ')
					new_ip--;
			} else new_ip--;
			(select ? selectionPoint : insertionPoint) = new_ip;
			if(!select) selectionPoint = insertionPoint;
			break;
		case key_right: case key_word_right:
			if(current_action) history.add(current_action), current_action.reset();
			if(haveSelection && !select) {
				selectionPoint = insertionPoint = std::max(selectionPoint,insertionPoint);
				break;
			}
			new_ip = select ? selectionPoint : insertionPoint;
			if(new_ip == contents.length()) break;
			if(key.k == key_word_right) {
				new_ip++;
				while(new_ip < contents.length() && contents[new_ip] != ' ')
					new_ip++;
			} else new_ip++;
			(select ? selectionPoint : insertionPoint) = new_ip;
			if(!select) selectionPoint = insertionPoint;
			break;
		case key_up:
			if(current_action) history.add(current_action), current_action.reset();
			if(haveSelection && !select)
				selectionPoint = insertionPoint = std::min(selectionPoint,insertionPoint);
			if(snippets[ip_row].at.y == snippets[0].at.y) {
				key.k = key_top;
				if(select) key.mod += mod_shift;
				handleInput(key);
			} else {
				int x = snippets[ip_row].at.x + ip_col, y = snippets[ip_row].at.y - 10;
				set_ip(loc(x,y), select ? &cTextField::selectionPoint : &cTextField::insertionPoint);
				if(!select) selectionPoint = insertionPoint;
			}
			break;
		case key_down:
			if(current_action) history.add(current_action), current_action.reset();
			if(haveSelection && !select)
				selectionPoint = insertionPoint = std::max(selectionPoint,insertionPoint);
			if(snippets[ip_row].at.y == snippets.back().at.y) {
				key.k = key_bottom;
				if(select) key.mod += mod_shift;
				handleInput(key);
			} else {
				int x = snippets[ip_row].at.x + ip_col, y = snippets[ip_row].at.y + 20;
				set_ip(loc(x,y), select ? &cTextField::selectionPoint : &cTextField::insertionPoint);
				if(!select) selectionPoint = insertionPoint;
			}
			break;
		case key_bsp: case key_word_bsp:
		case key_del: case key_word_del:
			if(haveSelection) {
				if(key.k == key_word_bsp)
					handleInput({true, key_word_left, mod_shift});
				else if(key.k == key_word_del)
					handleInput({true, key_word_right, mod_shift});
				auto begin = contents.begin() + std::min(selectionPoint, insertionPoint);
				auto end = contents.begin() + std::max(selectionPoint, insertionPoint);
				std::string removed(begin, end);
				auto result = contents.erase(begin, end);
				bool dir = insertionPoint < selectionPoint;
				selectionPoint = insertionPoint = result - contents.begin();
				if(current_action) history.add(current_action), current_action.reset();
				history.add(action_ptr(new aTextDelete(*this, std::min(selectionPoint, insertionPoint), removed, dir)));
			} else if(key.k == key_word_bsp) {
				cKey selectKey = key;
				selectKey.k = key_word_left;
				selectKey.mod = mod_shift;
				key.k = key_bsp;
				handleInput(selectKey);
				if(selectionPoint != insertionPoint)
					handleInput(key);
				return;
			} else if(key.k == key_bsp) {
				if(insertionPoint == 0) break;
				char c = contents[insertionPoint - 1];
				contents.erase(insertionPoint - 1,1);
				selectionPoint = --insertionPoint;
				if(aTextDelete* del = dynamic_cast<aTextDelete*>(current_action.get()))
					del->append_front(c);
				else {
					if(current_action) history.add(current_action);
					aTextDelete* new_del = new aTextDelete(*this, insertionPoint + 1, insertionPoint + 1);
					new_del->append_front(c);
					current_action.reset(new_del);
				}
			} else if(key.k == key_word_del) {
				cKey selectKey = key;
				selectKey.k = key_word_right;
				selectKey.mod = mod_shift;
				key.k = key_del;
				handleInput(selectKey);
				if(selectionPoint != insertionPoint)
					handleInput(key);
				return;
			} else if(key.k == key_del) {
				if(insertionPoint == contents.length()) break;
				char c = contents[insertionPoint];
				contents.erase(insertionPoint,1);
				if(aTextDelete* del = dynamic_cast<aTextDelete*>(current_action.get()))
					del->append_back(c);
				else {
					if(current_action) history.add(current_action);
					aTextDelete* new_del = new aTextDelete(*this, insertionPoint, insertionPoint);
					new_del->append_back(c);
					current_action.reset(new_del);
				}
			}
			break;
		case key_top:
			if(current_action) history.add(current_action), current_action.reset();
			if(!select) insertionPoint = 0;
			selectionPoint = 0;
			break;
		case key_bottom:
			if(current_action) history.add(current_action), current_action.reset();
			if(!select) insertionPoint = contents.length();
			selectionPoint = contents.length();
			break;
		case key_end:
			if(current_action) history.add(current_action), current_action.reset();
			new_ip = snippets[ip_row].at.x + string_length(snippets[ip_row].text, style);
			set_ip(loc(new_ip, snippets[ip_row].at.y), select ? &cTextField::selectionPoint : &cTextField::insertionPoint);
			if(!select) selectionPoint = insertionPoint;
			break;
		case key_home:
			if(current_action) history.add(current_action), current_action.reset();
			set_ip(snippets[ip_row].at, select ? &cTextField::selectionPoint : &cTextField::insertionPoint);
			if(!select) selectionPoint = insertionPoint;
			break;
		case key_pgup:
			if(current_action) history.add(current_action), current_action.reset();
			if(snippets[ip_row].at.y != snippets[0].at.y) {
				int x = snippets[ip_row].at.x + ip_col, y = frame.top + 2;
				set_ip(loc(x,y), select ? &cTextField::selectionPoint : &cTextField::insertionPoint);
				if(!select) selectionPoint = insertionPoint;
			}
			break;
		case key_pgdn:
			if(current_action) history.add(current_action), current_action.reset();
			if(snippets[ip_row].at.y != snippets.back().at.y) {
				int x = snippets[ip_row].at.x + ip_col, y = frame.bottom - 2;
				set_ip(loc(x,y), select ? &cTextField::selectionPoint : &cTextField::insertionPoint);
				if(!select) selectionPoint = insertionPoint;
			}
			break;
		case key_copy:
		case key_cut:
			if(current_action) history.add(current_action), current_action.reset();
			set_clipboard(contents.substr(std::min(insertionPoint,selectionPoint), abs(insertionPoint - selectionPoint)));
			if(key.k == key_cut) {
				cKey deleteKey = key;
				deleteKey.k = key_bsp;
				handleInput(deleteKey);
				contents = getText();
			}
			break;
		case key_paste:
			if(current_action) history.add(current_action), current_action.reset();
			if(!get_clipboard().empty()) {
				if(haveSelection) {
					cKey deleteKey = {true, key_bsp, mod_none};
					handleInput(deleteKey);
				}
				contents = getText();
				std::string toInsert = get_clipboard();
				contents.insert(insertionPoint, toInsert);
				history.add(action_ptr(new aTextInsert(*this, insertionPoint, toInsert)));
				insertionPoint += toInsert.length();
				selectionPoint = insertionPoint;
			}
			break;
		case key_undo:
			if(current_action) history.add(current_action), current_action.reset();
			history.undo();
			return;
		case key_redo:
			if(current_action) history.add(current_action), current_action.reset();
			history.redo();
			return;
		case key_selectall:
			if(current_action) history.add(current_action), current_action.reset();
			selectionPoint = 0;
			insertionPoint = contents.length();
			break;
			// These keys have no function in this context.
		case key_esc:
		case key_tab:
		case key_help:
		case key_insert:
			break;
	}
	// Setting the text normally resets insertion/selection point, but we don't want that here.
	int ip = insertionPoint, sp = selectionPoint;
	setText(contents);
	insertionPoint = ip;
	selectionPoint = sp;
}

std::string cTextField::parse(ticpp::Element& who, std::string fname) {
	using namespace ticpp;
	Iterator<Attribute> attr;
	Iterator<Node> node;
	std::string name, id;
	int width = 0, height = 0;
	bool foundTop = false, foundLeft = false; // requireds
	rectangle frame;
	for(attr = attr.begin(&who); attr != attr.end(); attr++){
		attr->GetName(&name);
		if(name == "name")
			attr->GetValue(&id);
		else if(name == "type"){
			std::string val;
			attr->GetValue(&val);
			if(val == "int")
				setInputType(FLD_INT);
			else if(val == "uint")
				setInputType(FLD_UINT);
			else if(val == "real")
				setInputType(FLD_REAL);
			else if(val == "text")
				setInputType(FLD_TEXT);
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
			attr->GetValue(&tabOrder);
		}else throw xBadAttr("field",name,attr->Row(),attr->Column(),fname);
	}
	if(!foundTop) throw xMissingAttr("field","top",attr->Row(),attr->Column(),fname);
	if(!foundLeft) throw xMissingAttr("field","left",attr->Row(),attr->Column(),fname);
	frame.right = frame.left + width;
	frame.bottom = frame.top + height;
	setBounds(frame);
	std::string content;
	for(node = node.begin(&who); node != node.end(); node++){
		std::string val;
		int type = node->Type();
		node->GetValue(&val);
		if(type == TiXmlNode::TEXT)
			content += dlogStringFilter(val);
		else if(type != TiXmlNode::COMMENT) {
			val = '<' + val + '>';
			throw xBadVal("field",xBadVal::CONTENT,val,node->Row(),node->Column(),fname);
		}
	}
	setText(content);
	return id;
}

aTextInsert::aTextInsert(cTextField& in, int at, std::string text) : cAction("insert text"), in(in), at(at), text(text) {}

bool aTextInsert::undo_me() {
	std::string contents = in.getText();
	auto del_start = contents.begin() + at;
	auto del_end = del_start + text.length();
	auto result = contents.erase(del_start, del_end);
	in.setText(contents);
	in.selectionPoint = in.insertionPoint = result - contents.begin();
	return true;
}

bool aTextInsert::redo_me() {
	std::string contents = in.getText();
	contents.insert(at, text);
	in.setText(contents);
	in.selectionPoint = in.insertionPoint = at + text.length();
	return true;
}

void aTextInsert::append(char c) {
	text += c;
}

aTextDelete::aTextDelete(cTextField& in, int start, int end) : cAction("delete text"), in(in), start(start), end(end), ip(0) {}

aTextDelete::aTextDelete(cTextField& in, int start, std::string content, bool from_start) : cAction("delete text"), in(in), start(start), end(start + content.size()), text(content), ip(from_start ? 0 : content.size()) {}

bool aTextDelete::undo_me() {
	std::string contents = in.getText();
	contents.insert(start, text);
	in.setText(contents);
	in.selectionPoint = in.insertionPoint = start + ip;
	return true;
}

bool aTextDelete::redo_me() {
	std::string contents = in.getText();
	auto del_start = contents.begin() + start;
	auto del_end = contents.begin() + end;
	auto result = contents.erase(del_start, del_end);
	in.setText(contents);
	in.selectionPoint = in.insertionPoint = result - contents.begin();
	return true;
}

void aTextDelete::append_front(char c) {
	text = c + text;
	start--;
	ip++;
}

void aTextDelete::append_back(char c) {
	text += c;
	end++;
}

cControl::storage_t cTextField::store() {
	storage_t storage = cControl::store();
	storage["fld-ip"] = insertionPoint;
	storage["fld-sp"] = selectionPoint;
	return storage;
}

void cTextField::restore(storage_t to) {
	cControl::restore(to);
	if(to.find("fld-ip") != to.end())
		insertionPoint = boost::any_cast<int>(to["fld-ip"]);
	else insertionPoint = getText().length();
	if(to.find("fld-sp") != to.end())
		selectionPoint = boost::any_cast<int>(to["fld-sp"]);
	else selectionPoint = 0;
}
