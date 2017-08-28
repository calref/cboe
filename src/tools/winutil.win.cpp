
#include "winutil.hpp"
#include <iostream>
#include <windows.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Image.hpp>
#include <sstream>
#include <cmath>

extern sf::RenderWindow mainPtr;
OPENFILENAMEA getParty, getScen, getRsrc, putParty, putScen, putRsrc;

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
	// Copied and tweaked from Battle for Wesnoth
	static const std::string base = "Microsoft Windows";

	OSVERSIONINFOEXA v;
	v.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);

	if(!GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&v))) {
		return base;
	}

	const DWORD vnum = v.dwMajorVersion * 100 + v.dwMinorVersion;
	std::ostringstream version;
	version << base << ' ';

	switch(vnum) {
	case 500:
		version << "2000";
		break;
	case 501:
		version << "XP";
		break;
	case 502:
		// This will misidentify XP x64 but who really cares?
		version << "Server 2003";
		break;
	case 600:
		if(v.wProductType == VER_NT_WORKSTATION) {
			version << "Vista";
		} else {
			version << "Server 2008";
		}
		break;
	case 601:
		if(v.wProductType == VER_NT_WORKSTATION) {
			version << "7";
		} else {
			version << "Server 2008 R2";
		}
		break;
	case 602:
		if(v.wProductType == VER_NT_WORKSTATION) {
			version << "8";
		} else {
			version << "Server 2012";
		}
		break;
	case 603:
		if(v.wProductType == VER_NT_WORKSTATION) {
			version << "8.1";
		} else {
			version << "Server 2012 R2";
		}
		break;
	case 1000:
		if(v.wProductType == VER_NT_WORKSTATION) {
			version << "10";
			break;
		} // else fallback to default
	default:
		if(v.wProductType != VER_NT_WORKSTATION) {
			version << "Server";
		}
	}

	if(v.szCSDVersion && *v.szCSDVersion) {
		version << ' ' << v.szCSDVersion;
	}

	return version.str();
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
	getParty = getScen = getRsrc = putParty = putScen = putRsrc = base_dlg;
	// Open saved game dialog
	getParty.lpstrFilter = LPCTSTR("Blades of Exile Saved Games\0*.exg\0Legacy Saved Games\0*.mac;*.boe;*.SAV\0");
	getParty.lpstrTitle = LPCTSTR("Load Game");
	getParty.Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	// Open scenario dialog
	getScen.lpstrFilter = LPCTSTR("Blades of Exile Scenarios\0*.boes\0Legacy or Unpacked Scenarios\0*.exs\0");
	getScen.lpstrTitle = LPCTSTR("Open Scenario");
	getScen.Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	// Import resource dialog
	getRsrc.lpstrTitle = LPCTSTR("Import Resource");
	getRsrc.Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	// Save game dialog
	putParty.lpstrFilter = LPCTSTR("Blades of Exile Saved Games\0*.exg\0");
	putParty.lpstrTitle = LPCTSTR("Save Game");
	putParty.Flags |= OFN_OVERWRITEPROMPT;
	// Save scenario dialog
	putScen.lpstrFilter = LPCTSTR("Blades of Exile Scenario\0*.boes\0");
	putScen.lpstrTitle = LPCTSTR("Save Scenario");
	putScen.Flags |= OFN_OVERWRITEPROMPT;
	// Export resource dialog
	putRsrc.lpstrTitle = LPCTSTR("Export Resource");
	putRsrc.Flags |= OFN_OVERWRITEPROMPT;
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

static std::unique_ptr<CHAR, std::default_delete<CHAR[]>> extListToFilter(std::initializer_list<std::string> extList) {
	std::string title, filter;
	if(extList.size() == 0) {
		title = "All files";
		filter = "*.*";
	} else {
		if(*extList.begin() == "png") title = "Image resources";
		else if(*extList.begin() == "wav") title = "Sound resources";
		else title = "Resources";
		bool first = true;
		for(std::string ext : extList) {
			if(!first) filter += ';';
			first = false;
			filter += "*.";
			filter += ext;
		}
	}
	size_t len = title.size() + filter.size() + 3; // The 3 is for the null terminators
	CHAR* str = new CHAR[len];
	std::fill_n(str, len, 0);
	auto end = std::copy_n(title.begin(), title.size(), str);
	end++;
	std::copy_n(filter.begin(), filter.size(), end);
	return std::unique_ptr<CHAR, std::default_delete<CHAR[]>>(str);
}

fs::path nav_get_rsrc(std::initializer_list<std::string> extensions) {
	auto filter = extListToFilter(extensions);
	getRsrc.lpstrFilter = LPCTSTR(filter.get());
	return runFileDlog(getRsrc, "", false);
}

fs::path nav_put_rsrc(std::initializer_list<std::string> extensions, fs::path def) {
	auto filter = extListToFilter(extensions);
	putRsrc.lpstrFilter = LPCTSTR(filter.get());
	return runFileDlog(putRsrc, def.string(), true);
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

static void printErr(std::string msg) {
	char str[256];
	str[255] = 0;
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, str, 255, NULL);
	std::cerr << msg << ": " << str << std::endl;
}

void set_clipboard_img(sf::Image& img) {
	if(img.getSize().x == 0 && img.getSize().y == 0) return;
	size_t pxSize = img.getSize().x * img.getSize().y * 4;
	HGLOBAL data = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(BITMAPV5HEADER) + pxSize);
	if(data == NULL) {
		printErr("Error allocating bitmap");
		return;
	}
	BITMAPV5HEADER* info = (BITMAPV5HEADER*) GlobalLock(data);
	if(info == NULL) {
		printErr("Error accessing bitmap");
		GlobalFree(data);
		return;
	}
	info->bV5Size = sizeof(BITMAPV5HEADER);
	info->bV5Width = img.getSize().x;
	info->bV5Height = img.getSize().y;
	info->bV5Height *= -1;
	info->bV5Planes = 1;
	info->bV5BitCount = 32;
	info->bV5Compression = BI_BITFIELDS;
	info->bV5SizeImage = pxSize;
	info->bV5RedMask   = 0x000000ff;
	info->bV5GreenMask = 0x0000ff00;
	info->bV5BlueMask  = 0x00ff0000;
	info->bV5AlphaMask = 0xff000000;
	std::copy_n(img.getPixelsPtr(), pxSize, reinterpret_cast<sf::Uint8*>(info + 1));
	if(!OpenClipboard(mainPtr.getSystemHandle())) {
		printErr("Error opening clipboard");
		GlobalFree(data);
		return;
	}
	if(!EmptyClipboard()) {
		printErr("Error emptying clipboard");
		GlobalFree(data);
	}
	auto result = SetClipboardData(CF_DIBV5, data);
	if(result == NULL) {
		printErr("Error setting clipboard");
		GlobalFree(data);
	}
	CloseClipboard();
}

std::unique_ptr<sf::Image> get_clipboard_img() {
	if(!OpenClipboard(NULL)) return nullptr;
	HGLOBAL hData = GetClipboardData(CF_DIBV5);
	// TODO: Probably need to ignore all the below stuff and read the image based on the values in the BITMAPV5HEADER...
	// If the bitmap is compressed as PNG or something, I can probably use img.loadFromMemory, otherwise manually decompress?
	HDC dc = CreateCompatibleDC(GetDC(NULL));
	if(dc == NULL) {
		CloseClipboard();
		return nullptr;
	}
	std::unique_ptr<sf::Image> img;
	//HBITMAP bmp = (HBITMAP)GlobalLock(hData);
	BITMAPV5HEADER* hdr = (BITMAPV5HEADER*) GlobalLock(hData);
	if(hdr->bV5BitCount == 32) {
		if(hdr->bV5Compression == BI_RGB || hdr->bV5Compression == BI_BITFIELDS) {
			DWORD32* data = (DWORD32*) (hdr + 1);
			int rShift = 0, gShift = 0, bShift = 0, aShift = 0;
			if(hdr->bV5RedMask)
				rShift = log2(hdr->bV5RedMask & (-hdr->bV5RedMask));
			if(hdr->bV5GreenMask)
				gShift = log2(hdr->bV5GreenMask & (-hdr->bV5GreenMask));
			if(hdr->bV5BlueMask)
				bShift = log2(hdr->bV5BlueMask & (-hdr->bV5BlueMask));
			if(hdr->bV5AlphaMask)
				aShift = log2(hdr->bV5AlphaMask & (-hdr->bV5AlphaMask));
			img.reset(new sf::Image);
			img->create(hdr->bV5Width, hdr->bV5Height);
			for(int y = 0; y < abs(hdr->bV5Height); y++) {
				for(int x = 0; x < hdr->bV5Width; x++) {
					sf::Color clr;
					clr.r = (*data & hdr->bV5RedMask) >> rShift;
					clr.g = (*data & hdr->bV5GreenMask) >> gShift;
					clr.b = (*data & hdr->bV5BlueMask) >> bShift;
					if(hdr->bV5AlphaMask)
						clr.a = (*data & hdr->bV5AlphaMask) >> aShift;
					if(hdr->bV5Height < 0)
						img->setPixel(x, y, clr);
					else img->setPixel(x, hdr->bV5Height - y - 1, clr);
					data++;
				}
			}
		}
	}
CLEANUP:
	GlobalUnlock(hData);
	CloseClipboard();
	ReleaseDC(NULL, dc);
	return img;
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
	makeFrontWindow(*parent);
	RedrawWindow(parent->getSystemHandle(), NULL, NULL, RDW_NOCHILDREN | RDW_UPDATENOW);
}

int getMenubarHeight() {
	return GetSystemMetrics(SM_CYMENU);
}
