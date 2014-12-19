/*
 *  field.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#include "field.h"
#include <sstream>
#include <map>
#include <boost/lexical_cast.hpp>
#include "dialog.h"
#include "dlogutil.h"
#include "graphtool.h"
#include "winutil.h"

void cTextField::attachClickHandler(click_callback_t) throw(xHandlerNotSupported){
	throw xHandlerNotSupported(false);
}

void cTextField::attachFocusHandler(focus_callback_t f) throw(){
	onFocus = f;
}

bool cTextField::triggerFocusHandler(cDialog& me, std::string id, bool losingFocus){
	if(losingFocus && field_type != FLD_TEXT) {
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
				{FLD_UINT, "a positive integer"},
				{FLD_REAL, "a number"},
			};
			giveError("You need to enter " + typeNames.at(field_type) + "!","",parent);
			return false;
		}
	}
	bool passed = true;
	if(onFocus != NULL) passed = onFocus(me,id,losingFocus);
	if(passed) haveFocus = !losingFocus;
	if(haveFocus && insertionPoint < 0)
		insertionPoint = getText().length();
	return passed;
}

bool cTextField::handleClick(location) {
	// TODO: Set the insertion point, handle selection, etc
	if(haveFocus) return true;
	if(parent && !parent->setFocus(this)) return true;
	haveFocus = true;
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

bool cTextField::hasFocus() {
	return haveFocus;
}

cTextField::cTextField(cDialog* parent) :
		cControl(CTRL_FIELD,*parent),
		color(sf::Color::Black),
		insertionPoint(-1),
		selectionPoint(0),
		haveFocus(false),
		field_type(FLD_TEXT) {}

cTextField::~cTextField(){}

void cTextField::draw(){
	static const sf::Color hiliteClr = {127,127,127}, ipClr = {92, 92, 92};
	inWindow->setActive();
	rectangle outline = frame;
	outline.inset(-2,-2);
	fill_rect(*inWindow, outline, sf::Color::White);
	frame_rect(*inWindow, outline, sf::Color::Black);
	std::string contents = getText();
	rectangle rect = frame;
	rect.inset(2,2);
	TextStyle style;
	style.font = FONT_PLAIN;
	style.pointSize = 12;
	style.colour = sf::Color::Black;
	style.lineHeight = 16;
	size_t ip_offset = 0;
	hilite_t hilite = {insertionPoint, selectionPoint};
	if(selectionPoint < insertionPoint) std::swap(hilite.first,hilite.second);
	if(haveFocus && contents.length() > 1) {
		// Determine which line the insertion and selection points are on
		clip_rect(*inWindow, {0,0,0,0}); // To prevent drawing
		hilite_t tmp_hilite = hilite;
		// Manipulate this to ensure that there is a hilited area
		std::string dummy_str = contents + "  ";
		if(tmp_hilite.first >= tmp_hilite.second)
			tmp_hilite.second = tmp_hilite.first + 1;
		std::vector<rectangle> rects = draw_string_hilite(*inWindow, rect, dummy_str, style, {tmp_hilite}, {0,0,0});
		if(!rects.empty()) {
			// We only care about the first and last rects. Furthermore, we only really need one point
			location ip_pos = rects[0].centre(), sp_pos = rects[rects.size() - 1].centre();
			if(selectionPoint < insertionPoint) std::swap(ip_pos, sp_pos);
			// Prioritize selection point being visible. If possible, also keep insertion point visible.
			// We do this by first ensuring the insertion point is visible, then doing the same
			// for the selection point.
			while(!ip_pos.in(frame)) {
				rect.offset(0,-14);
				ip_pos.y -= 14;
				sp_pos.y -= 14;
			}
			while(!sp_pos.in(frame)) {
				int shift = selectionPoint < insertionPoint ? 14 : -14;
				rect.offset(0,shift);
				ip_pos.y += shift;
				sp_pos.y += shift;
			}
		}
		undo_clip(*inWindow);
	}
	clip_rect(*inWindow, frame);
	if(haveFocus) {
		std::vector<snippet_t> snippets = draw_string_sel(*inWindow, rect, contents, style, {hilite}, hiliteClr);
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
	} else win_draw_string(*inWindow, rect, contents, eTextMode::WRAP, style);
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
			if(key.k == key_up) key.k = key_pgup;
			else if(key.k == key_down) key.k = key_pgdn;
			else if(key.k == key_left) key.k = key_word_left;
			else if(key.k == key_right) key.k = key_word_right;
			else if(key.k == key_del) key.k = key_word_del;
			else if(key.k == key_bsp) key.k = key_word_bsp;
#endif
		} else if(mod_contains(key.mod, mod_shift)) {
			if(key.k == key_insert) key.k = key_paste;
			else if(key.k == key_del) key.k = key_cut;
		}
		if(key.k != former) key.mod -= mod_ctrl;
	}
	return key;
}

void cTextField::handleInput(cKey key) {
	bool select = mod_contains(key.mod, mod_shift);
	bool haveSelection = insertionPoint != selectionPoint;
	key = divineFunction(key);
	size_t new_ip;
	std::string contents = getText();
	if(!key.spec) {
		if(haveSelection) {
			cKey deleteKey = key;
			deleteKey.spec = true;
			deleteKey.k = key_bsp;
			handleInput(deleteKey);
			contents = getText();
		}
		contents.insert(contents.begin() + insertionPoint, char(key.c));
		selectionPoint = ++insertionPoint;
	} else switch(key.k) {
			// TODO: Implement all the other special keys
		case key_left: case key_word_left:
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
			if(haveSelection && !select) {
				selectionPoint = insertionPoint = std::max(selectionPoint,insertionPoint);
				break;
			}
			new_ip = select ? selectionPoint : insertionPoint;
			if(new_ip == contents.length()) break;
			if(key.k == key_word_right) {
				new_ip++;
				while(new_ip < contents.length() && contents[new_ip + 1] != ' ')
					new_ip++;
			} else new_ip++;
			(select ? selectionPoint : insertionPoint) = new_ip;
			if(!select) selectionPoint = insertionPoint;
			break;
		case key_up:
		case key_down:
			break;
		case key_bsp: case key_word_bsp:
			if(haveSelection) {
				if(key.k == key_word_bsp)
					handleInput({true, key_word_right, mod_shift});
				auto begin = contents.begin() + std::min(selectionPoint, insertionPoint);
				auto end = contents.begin() + std::max(selectionPoint, insertionPoint);
				auto result = contents.erase(begin, end);
				selectionPoint = insertionPoint = result - contents.begin();
			} else if(key.k == key_word_bsp) {
				cKey selectKey = key;
				selectKey.k = key_left;
				handleInput(selectKey);
				if(selectionPoint != insertionPoint)
					handleInput(key);
			} else {
				if(insertionPoint == 0) break;
				contents.erase(insertionPoint - 1,1);
				selectionPoint = --insertionPoint;
			}
			break;
		case key_del: case key_word_del:
			if(haveSelection) {
				if(key.k == key_word_del)
					handleInput({true, key_word_left, mod_shift});
				auto begin = contents.begin() + std::min(selectionPoint, insertionPoint);
				auto end = contents.begin() + std::max(selectionPoint, insertionPoint);
				auto result = contents.erase(begin, end);
				selectionPoint = insertionPoint = result - contents.begin();
			} else if(key.k == key_word_del) {
				cKey selectKey = key;
				selectKey.k = key_left;
				handleInput(selectKey);
				if(selectionPoint != insertionPoint)
					handleInput(key);
			} else {
				if(insertionPoint == contents.length()) break;
				contents.erase(insertionPoint,1);
			}
			break;
		case key_top:
			if(select) selectionPoint = 0;
			else insertionPoint = 0;
			break;
		case key_bottom:
			if(select) selectionPoint = contents.length();
			else insertionPoint = contents.length();
			break;
		case key_end:
		case key_home:
		case key_pgup:
		case key_pgdn:
			break;
		case key_copy:
		case key_cut:
			set_clipboard(contents.substr(std::min(insertionPoint,selectionPoint), abs(insertionPoint - selectionPoint)));
			if(key.k == key_cut) {
				cKey deleteKey = key;
				deleteKey.k = key_bsp;
				handleInput(deleteKey);
				contents = getText();
			}
			break;
		case key_paste:
			if(!get_clipboard().empty()) {
				cKey deleteKey = {true, key_bsp, mod_none};
				handleInput(deleteKey);
				contents = getText();
				std::string toInsert = get_clipboard();
				contents.insert(insertionPoint, toInsert);
				insertionPoint += toInsert.length();
				selectionPoint = insertionPoint;
			}
			break;
		case key_undo:
		case key_redo:
			break;
		case key_selectall:
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
	setText(contents);
}
