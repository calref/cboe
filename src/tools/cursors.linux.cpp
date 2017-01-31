
#include "cursors.hpp"
#include "restypes.hpp" // Include before X11 to avoid macro conflicts in SFML
#include <X11/Xlib.h>
#include <X11/cursorfont.h>

Cursor ibeam = XCreateFontCursor(NULL, XC_xterm);

extern cursor_type current_cursor;

cCursor::cCursor(fs::path imgPath, float hotSpotX, float hotSpotY) {
}

cCursor::~cCursor() {
	Cursor* realPtr = reinterpret_cast<Cursor*>(ptr);
	XFreeCursor(NULL, *realPtr);
	delete realPtr;
}

void cCursor::apply() {
	XDefineCursor(NULL, current_window, *reinterpret_cast<Cursor*>(ptr));
}

void obscureCursor() {
	// TODO: This hides it permanently; it should only hide it until it moves
	XUndefineCursor(NULL, current_window);
}

void set_cursor(cursor_type which_c) {
	if(which_c != watch_curs)
		current_cursor = which_c;
	if(which_c == text_curs) {
		XDefineCursor(NULL, current_window, ibeam);
	} else {
		cCursor& curs = *ResMgr::get<CursorRsrc>(cursors[which_c]);
		curs.apply();
	}
}

void restore_cursor() {
	set_cursor(current_cursor);
}
