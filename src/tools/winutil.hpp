//
//  winutil.h
//  BoE
//
//  Created by Celtic Minstrel on 14-03-28.
//
//

#ifndef BoE_boe_windows_h
#define BoE_boe_windows_h

#include <wtypes.h>
#include <boost/filesystem/path.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <vector>

#include "prefs.hpp"
#include "mathutil.hpp"

char keyToChar(sf::Keyboard::Key key, bool isShift);

void makeFrontWindow(sf::Window& win);
void setWindowFloating(sf::Window& win, bool floating);

// Necessary wrapper for sf::Window.pollEvent()
bool pollEvent(sf::Window& win, sf::Event& event);
bool pollEvent(sf::Window* win, sf::Event& event);

void init_fileio(HWND window);
void launchURL(std::string url);
// Open a documentation page relative to packaged HTML if available,
// or openboe.com if not
void launchDocs(std::string relative_url);

// Optionally do some platform-specific preprocessing on the command-line arguments before parsing them.
// If preprocessing is needed, the expectation is that they will be modified in-place.
void preprocess_args(int& argc, char* argv[]);

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

// This is an additional offset between the "logical" top of the window an the UI.
// On Windows and Mac no offset is needed because the menubar is not a part of the mainPtr's
// coordinate space, but on Linux it is.
inline int os_specific_y_offset() {
	return
#if defined(SFML_SYSTEM_WINDOWS) || defined(SFML_SYSTEM_MACOS)
		0;
#else
		getMenubarHeight();
#endif
}

double fallback_scale();

inline double get_ui_scale() {
	return get_float_pref("UIScale", fallback_scale());
}

inline double get_ui_scale_map() {
	return get_float_pref("UIScaleMap", fallback_scale());
}

void adjust_window_for_menubar(int mode, unsigned int width, unsigned int height);

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

sf::RenderWindow& get_main_window();
sf::RenderWindow& get_mini_map_window();

#endif
