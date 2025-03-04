
#include "winutil.hpp"
#include <iostream>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <sstream>
#include <cstdlib>
#include <X11/Xlib.h>

#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>

#include "dialogxml/dialogs/dialog.hpp"

extern sf::RenderWindow mainPtr;
extern void showError(std::string str1, std::string str2, cDialog* parent = nullptr);

extern void play_sound(snd_num_t which, sf::Time delay = sf::Time());

// TODO: I'm sure there's a better way to do this (maybe one that's keyboard layout agnostic)
// The proper way would involve use of the TextEntered event
char keyToChar(sf::Keyboard::Key key, bool isShift) {
	using kb = sf::Keyboard;
	switch(key) {
	case kb::A: return isShift ? 'A' : 'a';
	case kb::B: return isShift ? 'B' : 'b';
	case kb::C: return isShift ? 'C' : 'c';
	case kb::D: return isShift ? 'D' : 'd';
	case kb::E: return isShift ? 'E' : 'e';
	case kb::F: return isShift ? 'F' : 'f';
	case kb::G: return isShift ? 'G' : 'g';
	case kb::H: return isShift ? 'H' : 'h';
	case kb::I: return isShift ? 'I' : 'i';
	case kb::J: return isShift ? 'J' : 'j';
	case kb::K: return isShift ? 'K' : 'k';
	case kb::L: return isShift ? 'L' : 'l';
	case kb::M: return isShift ? 'M' : 'm';
	case kb::N: return isShift ? 'N' : 'n';
	case kb::O: return isShift ? 'O' : 'o';
	case kb::P: return isShift ? 'P' : 'p';
	case kb::Q: return isShift ? 'Q' : 'q';
	case kb::R: return isShift ? 'R' : 'r';
	case kb::S: return isShift ? 'S' : 's';
	case kb::T: return isShift ? 'T' : 't';
	case kb::U: return isShift ? 'U' : 'u';
	case kb::V: return isShift ? 'V' : 'v';
	case kb::W: return isShift ? 'W' : 'w';
	case kb::X: return isShift ? 'X' : 'x';
	case kb::Y: return isShift ? 'Y' : 'y';
	case kb::Z: return isShift ? 'Z' : 'z';
	case kb::Num1: return isShift ? '!' : '1';
	case kb::Num2: return isShift ? '@' : '2';
	case kb::Num3: return isShift ? '#' : '3';
	case kb::Num4: return isShift ? '$' : '4';
	case kb::Num5: return isShift ? '%' : '5';
	case kb::Num6: return isShift ? '^' : '6';
	case kb::Num7: return isShift ? '&' : '7';
	case kb::Num8: return isShift ? '*' : '8';
	case kb::Num9: return isShift ? '(' : '9';
	case kb::Num0: return isShift ? ')' : '0';
	case kb::Tilde: return isShift ? '~' : '`';
	case kb::Dash: return isShift ? '_' : '-';
	case kb::Equal: return isShift ? '+' : '=';
	case kb::LBracket: return isShift ? '{' : '[';
	case kb::RBracket: return isShift ? '}' : ']';
	case kb::SemiColon: return isShift ? ':' : ';';
	case kb::Quote: return isShift ? '"' : '\'';
	case kb::Comma: return isShift ? '<' : ',';
	case kb::Period: return isShift ? '>' : '.';
	case kb::Slash: return isShift ? '?' : '/';
	case kb::BackSlash: return isShift ? '|' : '\\';
	case kb::Tab: return '\t';
	case kb::Space: return ' ';
	case kb::Return: return '\n';
	case kb::BackSpace: return '\b';
	case kb::Delete: return '\x7f';
		// Skip numpad keys, those that we handle at all are handled separately anyway
	default: break;
	}
	return 0;
}

std::string get_os_version() {
    utsname details;
    uname(&details);
    
    std::ostringstream version;
    
    version << details.sysname << ' ' << details.nodename << ' ' << details.release << ' ' << details.version;
    
	return version.str();
}

void makeFrontWindow(sf::Window& win) {
	win.requestFocus();
}

void setWindowFloating(sf::Window& win, bool floating) {
	// Code adapted from <http://stackoverflow.com/a/16235920>
	auto display = XOpenDisplay(NULL);
	Atom wmStateAbove = XInternAtom(display, "_NET_WM_STATE_ABOVE", true);
	if(wmStateAbove != None) {
		std::cout << "_NET_WM_STATE_ABOVE has atom of " << long(wmStateAbove) << std::endl;
	} else {
		std::cerr << "ERROR: cannot find atom for _NET_WM_STATE_ABOVE!\n";
	}
		
	Atom wmNetWmState = XInternAtom(display, "_NET_WM_STATE", true);
	if(wmNetWmState != None) {
		std::cout << "_NET_WM_STATE has atom of " << long(wmNetWmState) << std::endl;
	} else {
		std::cerr << "ERROR: cannot find atom for _NET_WM_STATE !\n";
	}
	// set window always on top hint
	if(wmStateAbove != None) {
		XClientMessageEvent xclient;
		memset(&xclient, 0, sizeof(xclient));
		
		xclient.type = ClientMessage;
		xclient.window = win.getSystemHandle(); // GDK_WINDOW_XID(window);
		xclient.message_type = wmNetWmState; //gdk_x11_get_xatom_by_name_for_display( display, "_NET_WM_STATE" );
		xclient.format = 32;
		xclient.data.l[0] = floating ? 1 : 0;
		xclient.data.l[1] = wmStateAbove; //gdk_x11_atom_to_xatom_for_display (display, state1);
		xclient.data.l[2] = 0; //gdk_x11_atom_to_xatom_for_display (display, state2);
		xclient.data.l[3] = 0;
		xclient.data.l[4] = 0;
		XSendEvent(display,
			win.getSystemHandle(), // !! DefaultRootWindow( display ) !!!
			False,
			SubstructureRedirectMask | SubstructureNotifyMask,
			(XEvent *)&xclient
		);
	}
}

void init_fileio(){
	// if init_fileio() is called more than once, only check once
	static bool checked_zenity = false;
	if(checked_zenity) return;

	std::string command = "zenity --help";
	FILE* hfile = popen(command.c_str(), "r");
	// Without reading the output, pclose() returns non-zero
    char buffer[128];
    while (!feof(hfile)) {
        fgets(buffer, 128, hfile);
    }
	int result = pclose(hfile);
	if(result != 0){
		showError("Open Blades of Exile for Linux requires zenity for file management.", "Please install zenity through your distribution's official package manager and try launching Blades of Exile again. Sorry!");
		exit(result);
	}
	checked_zenity = true;
}

static std::string runFileDialog(const std::string& file, bool save) {
    std::string filename;
    filename.reserve(1024);
    std::string command = "zenity --file-selection";
    if (file != "")
        command += " --filename=\"" + file + "\"";
    if (save)
        command += " --save";
    FILE* hfile = popen(command.c_str(), "r");
    char buffer[128];
    while (!feof(hfile)) {
        if (fgets(buffer, 128, hfile) != NULL)
            filename += buffer;
    }
	
	// When the dialogue is canceled, return empty string.
    int result = pclose(hfile);
    if (result != 0) {
        return "";
    }

    // Remove possible unwanted newline included from end of popen() output
    if (filename[filename.length()-1] == '\n')
        filename.erase(filename.length()-1);

    return filename;
}

fs::path nav_get_party() {
	return runFileDialog("Blades of Exile Save.exg", false);
}

fs::path nav_put_party(fs::path def) {
    if (def.empty())
        def = "Blades of Exile Save.exg";
	return runFileDialog(def.string(), true);
}

fs::path nav_get_scenario() {
	return runFileDialog("", false);
}

fs::path nav_put_scenario(fs::path def) {
	return runFileDialog(def.string(), true);
}


fs::path nav_get_rsrc(std::initializer_list<std::string> extensions) {
	return "";
}

fs::path nav_put_rsrc(std::initializer_list<std::string> extensions, fs::path def) {
	return "";
}

void set_clipboard(std::string text) {
}

std::string get_clipboard() {
	return "";
}

static void printErr(std::string msg) {
}

void set_clipboard_img(sf::Image& img) {
}

std::unique_ptr<sf::Image> get_clipboard_img() {
	return nullptr;
}

void beep() {
	play_sound(1);
}

void launchURL(std::string url) {
	system((std::string { "xdg-open " } + url).c_str());
}

void preprocess_args(int& argc, char* argv[]) {
}

// TODO: Implement modal session.
// It seems that Windows doesn't have anything similar to the Mac modal session, so I might have to somehow simulate it myself.
void ModalSession::pumpEvents() {
}

ModalSession::ModalSession(sf::Window& win, sf::Window& p) : parent(&p) {

}

ModalSession::~ModalSession() {
}

int getMenubarHeight() {
	// TODO: somehow get this constant in here from boe.consts.hpp
	// return MENUBAR_HEIGHT;
	return 20;
}

void adjust_window_for_menubar(int mode, unsigned int width, unsigned int height) {
}
