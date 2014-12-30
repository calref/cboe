//
//  winutil.mac.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-03-28.
//
//

#include "winutil.h"
#include <Cocoa/Cocoa.h>
#include <SFML/Graphics/RenderWindow.hpp>

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
	sf::WindowHandle handle = win.getSystemHandle();
	id nsHandle = id(handle);
	if([nsHandle isKindOfClass: [NSWindow class]]) {
		[nsHandle orderFrontRegardless];
		[nsHandle makeKeyWindow];
	}
}

void setWindowFloating(sf::Window& win, bool floating) {
	sf::WindowHandle handle = win.getSystemHandle();
	id nsHandle = id(handle);
	if([nsHandle isKindOfClass: [NSWindow class]]) {
		[nsHandle setLevel: floating ? NSFloatingWindowLevel : NSNormalWindowLevel];
	}
}

ModalSession::ModalSession(sf::Window& win, sf::Window& /*parent*/) {
	sf::WindowHandle handle = win.getSystemHandle();
	id nsHandle = id(handle);
	if([nsHandle isKindOfClass: [NSWindow class]])
		session = [[NSApplication sharedApplication] beginModalSessionForWindow: nsHandle];
}

ModalSession::~ModalSession() {
	NSModalSession nsHandle = (NSModalSession)session;
	[[NSApplication sharedApplication] endModalSession: nsHandle];
}

void ModalSession::pumpEvents() {
	NSModalSession nsHandle = (NSModalSession)session;
	[[NSApplication sharedApplication] runModalSession: nsHandle];
}

void set_clipboard(std::string text) {
	NSString* str = [[NSString stringWithUTF8String: text.c_str()] stringByReplacingOccurrencesOfString: @"|" withString: @"\n"];
	NSArray* contents = [NSArray arrayWithObject: str];
	NSPasteboard* pb = [NSPasteboard generalPasteboard];
	[pb clearContents];
	[pb writeObjects: contents];
}

std::string get_clipboard() {
	NSPasteboard* pb = [NSPasteboard generalPasteboard];
	NSDictionary* options = [NSDictionary dictionary];
	NSArray* types = [NSArray arrayWithObject: [NSString class]];
	if(![pb canReadObjectForClasses: types options: options])
		return "";
	NSArray* contents = [pb readObjectsForClasses: types options: options];
	NSString* str = [contents objectAtIndex: 0];
	return [[str stringByReplacingOccurrencesOfString: @"\n" withString: @"|"] cStringUsingEncoding: NSUTF8StringEncoding];
}

void beep() {
	NSBeep();
}

int getMenubarHeight() {
	// Mac menubar isn't in the window, so we return 0 here.
	return 0;
}

NSOpenPanel* dlg_get_scen;
NSOpenPanel* dlg_get_game;
NSSavePanel* dlg_put_scen;
NSSavePanel* dlg_put_game;
extern sf::RenderWindow mainPtr;

// TODO: I'm not sure if I'll need delegates to enable selection of files with no file extension that have file creator types?
//Boolean scen_file_filter(AEDesc* item, void* info, void * dummy, NavFilterModes filterMode){
//	if(item->descriptorType == typeFSRef){
//		OSErr err;
//		FSRef the_file;
//		FSCatalogInfo cat_info;
//		FSSpec file_spec;
//		AEGetDescData(item,&the_file,AEGetDescDataSize(item));
//		err = FSGetCatalogInfo (&the_file,kFSCatInfoFinderInfo,&cat_info,NULL,&file_spec,NULL);
//		if(err != noErr) return false;
//		FileInfo* file_info = (FileInfo*) &cat_info.finderInfo;
//		if(file_info->fileType == 'BETM') return true;
//		for(int i = 0; i < 64; i++){
//			if(file_spec.name[i] == '.')
//				if(file_spec.name[i + 1] == 'e' || file_spec.name[i + 1] == 'E')
//					if(file_spec.name[i + 1] == 'x' || file_spec.name[i + 1] == 'X')
//						if(file_spec.name[i + 1] == 's' || file_spec.name[i + 1] == 'S')
//							return true;
//		}
//	}
//	return false;
//}
//
//Boolean party_file_filter(AEDesc* item, void* info, void * dummy, NavFilterModes filterMode){
//	if(item->descriptorType == typeFSRef){
//		OSErr err;
//		FSRef the_file;
//		FSCatalogInfo cat_info;
//		FSSpec file_spec;
//		AEGetDescData(item,&the_file,AEGetDescDataSize(item));
//		err = FSGetCatalogInfo (&the_file,kFSCatInfoFinderInfo,&cat_info,NULL,&file_spec,NULL);
//		if(err != noErr) return false;
//		FileInfo* file_info = (FileInfo*) &cat_info.finderInfo;
//		if(file_info->fileType == 'beSV') return true;
//		for(int i = 0; i < 64; i++){
//			if(file_spec.name[i] == '.')
//				if(file_spec.name[i + 1] == 'e' || file_spec.name[i + 1] == 'E')
//					if(file_spec.name[i + 1] == 'x' || file_spec.name[i + 1] == 'X')
//						if(file_spec.name[i + 1] == 'g' || file_spec.name[i + 1] == 'G')
//							return true;
//		}
//	}
//	return false;
//}

void init_fileio(){
	dlg_get_scen = [NSOpenPanel openPanel];
	[dlg_get_scen setAllowedFileTypes: [NSArray arrayWithObjects: @"com.spidweb.bladesofexile.scenario", @"exs", @"boes", nil]];
	[dlg_get_scen setMessage: @"Select a scenario to edit:"];
	[dlg_get_scen setTitle: @"Load Scenario"];
	[dlg_get_scen retain];
	
	dlg_get_game = [NSOpenPanel openPanel];
	[dlg_get_game setAllowedFileTypes: [NSArray arrayWithObjects: @"com.spidweb.bladesofexile.savegame", @"com.spidweb.bladesofexile.oldmacsave", @"com.spidweb.bladesofexile.oldwinsave", @"exg", @"boe", @"SAV", @"mac", nil]];
	[dlg_get_game setMessage: @"Select a saved game to resume:"];
	[dlg_get_game setTitle: @"Load Game"];
	[dlg_get_game retain];
	
	dlg_put_scen = [NSSavePanel savePanel];
	[dlg_put_scen setAllowedFileTypes: [NSArray arrayWithObjects: @"com.spidweb.bladesofexile.scenario", @"boes", nil]];
	[dlg_put_scen setMessage: @"Select a location to save the scenario:"];
	[dlg_put_scen setTitle: @"Save Scenario"];
	[dlg_put_scen retain];
	
	dlg_put_game = [NSSavePanel savePanel];
	[dlg_put_game setAllowedFileTypes: [NSArray arrayWithObjects: @"com.spidweb.bladesofexile.savegame", @"exg", nil]];
	[dlg_put_game setMessage: @"Select a location to save your game:"];
	[dlg_put_game setTitle: @"Save Game"];
	[dlg_put_game retain];
}

fs::path nav_get_scenario() {
	bool gotFile = [dlg_get_scen runModal] != NSFileHandlingPanelCancelButton;
	makeFrontWindow(mainPtr);
	if(gotFile) {
		return fs::path([[[[dlg_get_scen URL] absoluteURL] path] UTF8String]);
	}
	return "";
}

fs::path nav_put_scenario(fs::path def) {
	if(!def.empty()) {
		// TODO: Hopefully requesting UTF-8 doesn't break anything...
		[dlg_put_scen setNameFieldStringValue:[NSString stringWithUTF8String: def.filename().c_str()]];
		[dlg_put_scen setDirectoryURL:[NSURL URLWithString:[NSString stringWithUTF8String: def.parent_path().c_str()]]];
	}
	bool gotFile = [dlg_put_scen runModal] != NSFileHandlingPanelCancelButton;
	makeFrontWindow(mainPtr);
	if(gotFile)
		return fs::path([[[[dlg_put_scen URL] absoluteURL] path] UTF8String]);
	return "";
}

fs::path nav_get_party() {
	bool gotFile = [dlg_get_game runModal] != NSFileHandlingPanelCancelButton;
	makeFrontWindow(mainPtr);
	if(gotFile)
		return fs::path([[[[dlg_get_game URL] absoluteURL] path] UTF8String]);
	return "";
}

fs::path nav_put_party(fs::path def) {
	if(!def.empty()) {
		// TODO: Hopefully requesting UTF-8 doesn't break anything...
		[dlg_put_game setNameFieldStringValue:[NSString stringWithUTF8String: def.filename().c_str()]];
		[dlg_put_game setDirectoryURL:[NSURL URLWithString:[NSString stringWithUTF8String: def.parent_path().c_str()]]];
	}
	bool gotFile = [dlg_put_game runModal] != NSFileHandlingPanelCancelButton;
	makeFrontWindow(mainPtr);
	if(gotFile)
		return fs::path([[[[dlg_put_game URL] absoluteURL] path] UTF8String]);
	return "";
}
