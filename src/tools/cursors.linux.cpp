
#include "cursors.hpp"
// #include "restypes.hpp" // Include before X11 to avoid macro conflicts in SFML
#include "fileio/resmgr/res_cursor.hpp"
#include <SFML/Graphics.hpp>

namespace x11 {

    #include <X11/Xlib.h>
    #include <X11/cursorfont.h>

}

x11::Cursor ibeam; // = XCreateFontCursor(NULL, XC_xterm);

// extern cursor_type current_cursor;
extern sf::RenderWindow mainPtr;

Cursor::Cursor(fs::path imgPath, float hotSpotX, float hotSpotY)
    : ptr(nullptr) {
    
}

Cursor::~Cursor() {
    if (ptr != nullptr) {
	    x11::Cursor* realPtr = reinterpret_cast<x11::Cursor*>(ptr);
	    x11::XFreeCursor(NULL, *realPtr);
	    delete realPtr;
	}
}

void Cursor::apply() {
	// XDefineCursor(NULL, current_window, *reinterpret_cast<Cursor*>(ptr));
}

void obscureCursor() {
	// TODO: This hides it permanently; it should only hide it until it moves
	// XUndefineCursor(NULL, current_window);
}

void set_cursor(cursor_type which_c) {
	if(which_c != watch_curs)
		// current_cursor = which_c;
        Cursor::current = which_c;
	if(which_c == text_curs) {
		// XDefineCursor(NULL, current_window, ibeam);
	} else {
		Cursor& curs = *ResMgr::cursors.get(cursors[which_c]);
		curs.apply();
	}
}

void restore_cursor() {
	set_cursor(Cursor::current);
}
