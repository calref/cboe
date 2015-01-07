
#include "winutil.h"
#include <iostream>
#include <Windows.h>
#include <SFML/Graphics/RenderWindow.hpp>

extern sf::RenderWindow mainPtr;
OPENFILENAMEA getParty, getScen, putParty, putScen;

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

void makeFrontWindow(sf::Window& win) {
	HWND win_handle = win.getSystemHandle();
	BringWindowToTop(win_handle);
}

void setWindowFloating(sf::Window& win, bool floating) {
	HWND win_handle = win.getSystemHandle();
	UINT flags = SWP_NOMOVE | SWP_NOSIZE;
	HWND newPos = floating ? HWND_TOPMOST : HWND_TOP;
	// The flags param specifies that these 0's are ignored.
	SetWindowPos(win_handle, newPos, 0, 0, 0, 0, flags);
}

void init_fileio() {
	OPENFILENAMEA base_dlg;
	memset(&base_dlg, 0, sizeof(OPENFILENAMEA));
	// Common values
	base_dlg.lStructSize = sizeof(OPENFILENAMEA);
	base_dlg.hwndOwner = mainPtr.getSystemHandle();
	base_dlg.nFilterIndex = 1;
	base_dlg.Flags = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	getParty = getScen = putParty = putScen = base_dlg;
	// Open saved game dialog
	getParty.lpstrFilter = LPCTSTR("Blades of Exile Saved Games\0*.exg\0Legacy Saved Games\0*.mac;*.boe;*.SAV\0");
	getParty.lpstrTitle = LPCTSTR("Load Game");
	getParty.Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	// Open scenario dialog
	getScen.lpstrFilter = LPCTSTR("Blades of Exile Scenarios\0*.boes\0Legacy or Unpacked Scenarios\0*.exs\0");
	getScen.lpstrTitle = LPCTSTR("Open Scenario");
	// Save game dialog
	putParty.lpstrFilter = LPCTSTR("Blades of Exile Saved Games\0*.exg\0");
	putParty.lpstrTitle = LPCTSTR("Save Game");
	putParty.Flags |= OFN_OVERWRITEPROMPT;
	// Save scenario dialog
	putScen.lpstrFilter = LPCTSTR("Blades of Exile Scenario\0*.boes\0");
	putScen.lpstrTitle = LPCTSTR("Save Scenario");
}

static std::string runFileDlog(OPENFILENAMEA& dlg, const std::string& file, bool save) {
	size_t sz = std::max<size_t>(MAX_PATH, file.length()) + 1;
	char* path = new char[sz];
	std::copy(file.begin(), file.end(), path);
	std::fill(path + file.length(), path + sz, 0);

	dlg.lpstrFile = path;
	dlg.nMaxFile = sz - 1;
	dlg.nFileOffset = file.find_last_of('\\');
	dlg.nFileOffset++;
	dlg.nFileExtension = file.find_last_of('.');
	dlg.nFileExtension++;

	int err;
	if(save) err = GetSaveFileNameA(&dlg);
	else err = GetOpenFileNameA(&dlg);
	if(err == 0) {
#define CASE(x) case x: std::cerr << "File dialog failed: " #x << std::endl; break
		switch(CommDlgExtendedError()) {
			CASE(CDERR_DIALOGFAILURE);
			CASE(CDERR_FINDRESFAILURE);
			CASE(CDERR_INITIALIZATION);
			CASE(CDERR_LOADRESFAILURE);
			CASE(CDERR_LOADSTRFAILURE);
			CASE(CDERR_LOCKRESFAILURE);
			CASE(CDERR_MEMALLOCFAILURE);
			CASE(CDERR_MEMLOCKFAILURE);
			CASE(CDERR_NOHINSTANCE);
			CASE(CDERR_NOHOOK);
			CASE(CDERR_NOTEMPLATE);
			CASE(CDERR_STRUCTSIZE);
			CASE(FNERR_BUFFERTOOSMALL);
			CASE(FNERR_INVALIDFILENAME);
			CASE(FNERR_SUBCLASSFAILURE);
		}
#undef CASE
	}

	std::string result = dlg.lpstrFile;
	dlg.lpstrFile = NULL;
	dlg.nMaxFile = 0;
	delete path;
	dlg.Flags &= ~OFN_EXTENSIONDIFFERENT;
	dlg.Flags &= ~OFN_READONLY;
	return result;
}

fs::path nav_get_party() {
	return runFileDlog(getParty, "Blades of Exile Save.exg", false);
}

fs::path nav_put_party(fs::path def) {
	return runFileDlog(putParty, def.string(), true);
}

fs::path nav_get_scenario() {
	return runFileDlog(getScen, "", false);
}

fs::path nav_put_scenario(fs::path def) {
	return runFileDlog(putScen, def.string(), true);
}

void set_clipboard(std::string text) {
	if(text.empty()) return;
	if(!OpenClipboard(mainPtr.getSystemHandle())) return;
	HGLOBAL hData = (char*) GlobalAlloc(GMEM_MOVEABLE, text.length() + 1);
	char* data = (char*)GlobalLock(hData);
	std::copy(text.c_str(), text.c_str() + text.length(), data);
	data[text.length()] = 0;
	GlobalUnlock(hData);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hData);
	CloseClipboard();
}

std::string get_clipboard() {
	if(!OpenClipboard(NULL)) return "";
	HGLOBAL hData = GetClipboardData(CF_TEXT);
	char* data = (char*) GlobalLock(hData);
	std::string contents(data);
	GlobalUnlock(hData);
	CloseClipboard();
	return contents;
}

void beep() {
	MessageBeep(-1);
}

void launchURL(std::string url) {
	ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

// TODO: Implement modal session.
// It seems that Windows doesn't have anything similar to the Mac modal session, so I might have to somehow simulate it myself.
void ModalSession::pumpEvents() {
	HWND win_handle = (HWND)session;
}

ModalSession::ModalSession(sf::Window& win, sf::Window& p) : parent(&p) {
	session = win.getSystemHandle();
	EnableWindow(parent->getSystemHandle(), false);
}

ModalSession::~ModalSession() {
	HWND win_handle = (HWND)session;
	EnableWindow(parent->getSystemHandle(), true);
	makeFrontWindow(parent);
	RedrawWindow(parent->getSystemHandle(), NULL, NULL, RDW_NOCHILDREN | RDW_UPDATENOW);
}

void set_up_apple_events() {
	// TODO: Probably nothing to do here on Windows?
	// Basically what might possibly happen here is to set up stuff to handle dragging a file onto the application icon.
	// The Mac version also handles requests from the operating system for the application to quit.
}

int getMenubarHeight() {
	return GetSystemMetrics(SM_CYMENU);
}
