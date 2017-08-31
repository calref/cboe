//
//  winutil.h
//  BoE
//
//  Created by Celtic Minstrel on 14-03-28.
//
//

#ifndef BoE_boe_windows_h
#define BoE_boe_windows_h

#include <boost/filesystem/path.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Image.hpp>

char keyToChar(sf::Keyboard::Key key, bool isShift);

void makeFrontWindow(sf::Window& win);
void setWindowFloating(sf::Window& win, bool floating);

void init_fileio();
void launchURL(std::string url);

std::string get_os_version();

fs::path nav_get_party();
fs::path nav_put_party(fs::path def = "");
fs::path nav_get_scenario();
fs::path nav_put_scenario(fs::path def = "");
fs::path nav_get_rsrc(std::initializer_list<std::string> extensions);
fs::path nav_put_rsrc(std::initializer_list<std::string> extensions, fs::path def = "");

// Deal with text snippets in the clipboard.
// If the clipboard contains something other than text,
// get_clipboard should return an empty string.
void set_clipboard(std::string text);
std::string get_clipboard();

// Deal with images in the clipboard, returning null if no image found
void set_clipboard_img(sf::Image& img);
std::unique_ptr<sf::Image> get_clipboard_img();

void beep();

// Calculates how much of the window is occupied by the menubar
int getMenubarHeight();

class ModalSession {
	void* session;
	sf::Window* parent;
public:
	void pumpEvents();
	ModalSession(sf::Window& win, sf::Window& parent);
	~ModalSession();
};

// System key
#ifdef __APPLE__
bool sf::Event::KeyEvent::*const systemKey = &sf::Event::KeyEvent::system;
#else
bool sf::Event::KeyEvent::*const systemKey = &sf::Event::KeyEvent::control;
#endif

#endif
