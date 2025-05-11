/*
 *  control.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */
#include <sstream>
#include <codecvt>
#include <locale>
#include "keycodes.hpp"
//#include <sstream>
//#include "dialog.hpp"
//#include "sounds.hpp"
//#include "button.hpp"
//#include "render_shapes.hpp"
//#include "res_image.hpp"
//#include "mathutil.hpp"
//#include "prefs.hpp"
//#include "cursors.hpp"
#include "keymods.hpp"

// Windows won't compile if this conversion is not explicit.
// (Wildly enough, it claims that char->wchar_t is a NARROWING conversion).
static wchar_t w(char ch) {
#ifdef SFML_SYSTEM_WINDOWS
	std::ostringstream sstr;
	sstr << ch;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::wstring wide_str = converter.from_bytes(sstr.str());
	return wide_str[0];
#else
	return ch;
#endif
}

// NOTE: This only supports a few characters including the ones
// that are currently used for debug key shortcuts!
// It is also very hard-coded to the key layout on my laptop (which may only be standard in the US!)
cKey charToKey(char ch) {
	if(ch >= 'a' && ch <= 'z'){
		return {false, w(ch)};
	}else if(ch >= 'A' && ch <= 'Z'){
		return {false, w(tolower(ch)), mod_shift};
	}
	switch(ch){
		case '=': case '/':
			return {false, w(ch)};
		case '+':
			return {false, w('='), mod_shift};
		case '<':
			return {false, w(','), mod_shift};
		case '>':
			return {false, w('.'), mod_shift};
		case '!':
			return {false, w('1'), mod_shift};
		case '@':
			return {false, w('2'), mod_shift};
		case '#':
			return {false, w('3'), mod_shift};
		case '$':
			return {false, w('4'), mod_shift};
		case '%':
			return {false, w('5'), mod_shift};
		case '^':
			return {false, w('6'), mod_shift};
		case '?':
			return {false, w('/'), mod_shift};
		case '~':
			return {false, w('`'), mod_shift};
		case '\\':
			return {false, w('\\'), mod_none};
		case '-':
			return {false, w('-'), mod_none};
		case '_':
			return {false, w('-'), mod_shift};
	}
	throw std::string {"Tried to convert unsupported char '"} + ch + "' to cKey!";
}

eKeyMod operator + (eKeyMod lhs, eKeyMod rhs){
	if(lhs == rhs) return lhs;
	else if(lhs == mod_none) return rhs;
	else if(lhs == mod_alt){
		if(rhs == mod_shift || rhs == mod_altshift) return mod_altshift;
		else if(rhs == mod_ctrl || rhs == mod_altctrl) return mod_altctrl;
		else if(rhs == mod_shiftctrl || rhs == mod_all) return mod_all;
		else return mod_alt;
	}else if(lhs == mod_shift){
		if(rhs == mod_alt || rhs == mod_altshift) return mod_altshift;
		else if(rhs == mod_ctrl || rhs == mod_shiftctrl) return mod_shiftctrl;
		else if(rhs == mod_altctrl || rhs == mod_all) return mod_all;
		else return mod_shift;
	}else if(lhs == mod_ctrl){
		if(rhs == mod_alt || rhs == mod_altctrl) return mod_altctrl;
		else if(rhs == mod_shift || rhs == mod_shiftctrl) return mod_shiftctrl;
		else if(rhs == mod_altshift || rhs == mod_all) return mod_all;
		else return mod_ctrl;
	}else return rhs + lhs;
}

eKeyMod operator - (eKeyMod lhs, eKeyMod rhs){
	if(lhs == rhs || lhs == mod_none || rhs == mod_all) return mod_none;
	else if(rhs == mod_none) return lhs;
	else if(lhs == mod_all){
		if(rhs == mod_alt) return mod_shiftctrl;
		else if(rhs == mod_shift) return mod_altctrl;
		else if(rhs == mod_ctrl) return mod_altshift;
		else if(rhs == mod_altshift) return mod_ctrl;
		else if(rhs == mod_altctrl) return mod_shift;
		else if(rhs == mod_shiftctrl) return mod_alt;
		else return mod_all;
	}else if(lhs == mod_shiftctrl){
		if(rhs == mod_shift || rhs == mod_altshift) return mod_ctrl;
		else if(rhs == mod_ctrl || rhs == mod_altctrl) return mod_shift;
		else return mod_shiftctrl;
	}else if(lhs == mod_altctrl){
		if(rhs == mod_alt || rhs == mod_altshift) return mod_ctrl;
		else if(rhs == mod_ctrl || rhs == mod_shiftctrl) return mod_alt;
		else return mod_altctrl;
	}else if(lhs == mod_altshift){
		if(rhs == mod_alt || rhs == mod_altctrl) return mod_shift;
		else if(rhs == mod_shift || rhs == mod_shiftctrl) return mod_alt;
		else return mod_altshift;
	}else if(lhs == mod_alt && (rhs == mod_altshift || rhs == mod_altctrl))
		return mod_none;
	else if(lhs == mod_shift && (rhs == mod_altshift || rhs == mod_shiftctrl))
		return mod_none;
	else if(lhs == mod_ctrl && (rhs == mod_altctrl || rhs == mod_shiftctrl))
		return mod_none;
	else return lhs;
}

eKeyMod& operator += (eKeyMod&lhs, eKeyMod rhs){
	lhs = lhs + rhs;
	return lhs;
}

eKeyMod& operator -= (eKeyMod&lhs, eKeyMod rhs){
	lhs = lhs - rhs;
	return lhs;
}

bool operator== (cKey a, cKey b){
	if(a.spec != b.spec) return false;
	if(a.mod != b.mod) return false;
	return a.spec ? a.k == b.k : (a.c == 0 ? false : a.c == b.c);
}

bool mod_contains(eKeyMod mods, eKeyMod mod) {
	if((mods & mod) != 0) return true;
	return false;
}

unsigned char applyShift(unsigned char c){
	if(c < ' ' || c >= 0x7f) return c;
	static const char afterShift[] = {
		' ', '!', '"', '#', '$', '%', '&', '"', '(', ')', '*', '+', '<', '_', '>', '?',
		')', '!', '@', '#', '$', '%', '^', '&', '*', '(', ':', ':', '<', '+', '>', '?',
		'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '^', '_',
		'~', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '{', '|', '}', '~',
	};
	return afterShift[c - ' '];
}

unsigned char removeShift(unsigned char c){
	if(c < ' ' || c >= 0x7f) return c;
	static const char afterUnShift[] = {
		' ', '1', '\'','3', '4', '5', '7', '\'','9', '0', '8', '=', ',', '-', '.', '/',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ';', ';', ',', '=', '.', '/',
		'2', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
		'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\',']', '6', '-',
		'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
		'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\',']', '`',
	};
	return afterUnShift[c - ' '];
}

eKeyMod current_key_mod() {
	eKeyMod mod = mod_none;
	if(kb.isCtrlPressed()) mod += mod_ctrl;
	if(kb.isMetaPressed()) mod += mod_ctrl;
	if(kb.isAltPressed()) mod += mod_alt;
	if(kb.isShiftPressed()) mod += mod_shift;
	return mod;
}
