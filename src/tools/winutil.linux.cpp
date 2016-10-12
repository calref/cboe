
#include "winutil.hpp"
#include <iostream>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <sstream>

extern sf::RenderWindow mainPtr;

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
	case kb::Numpad0: return '0';
	case kb::Numpad1: return '1';
	case kb::Numpad2: return '2';
	case kb::Numpad3: return '3';
	case kb::Numpad4: return '4';
	case kb::Numpad5: return '5';
	case kb::Numpad6: return '6';
	case kb::Numpad7: return '7';
	case kb::Numpad8: return '8';
	case kb::Numpad9: return '9';
		// TODO: Should have Equal here, but SFML doesn't distinguish between normal and keybad equal :/
		// Ditto for the decimal point.
	case kb::Divide: return '/';
	case kb::Multiply: return '*';
	case kb::Subtract: return '-';
	case kb::Add: return '+';
	default: break;
	}
	return 0;
}

std::string get_os_version() {
	return "Microsoft Windows XP";
}

void makeFrontWindow(sf::Window& win) {
}

void setWindowFloating(sf::Window& win, bool floating) {
}

void init_fileio() {
}

fs::path nav_get_party() {
	return "";
}

fs::path nav_put_party(fs::path def) {
	return "";
}

fs::path nav_get_scenario() {
	return "";
}

fs::path nav_put_scenario(fs::path def) {
	return "";
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
}

void launchURL(std::string url) {
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
	return 1;
}
