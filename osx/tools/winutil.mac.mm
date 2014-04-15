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

bool isFrontWindow(sf::Window& win) {
	sf::WindowHandle handle = win.getSystemHandle();
	id nsHandle = id(handle);
	if([nsHandle isKindOfClass: [NSWindow class]]) {
		BOOL main = [nsHandle isMainWindow];
		BOOL key = [nsHandle isKeyWindow];
		return main;
	}
	return false;
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

ModalSession::ModalSession(sf::Window& win) {
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
	[dlg_get_scen setAllowedFileTypes: [NSArray arrayWithObjects: @"com.spidweb.bladesofexile.scenario", nil]];
	[dlg_get_scen setMessage: @"Select a scenario to edit:"];
	[dlg_get_scen setTitle: @"Load Scenario"];
	[dlg_get_scen retain];
	
	dlg_get_game = [NSOpenPanel openPanel];
	[dlg_get_game setAllowedFileTypes: [NSArray arrayWithObjects: @"com.spidweb.bladesofexile.savegame", @"com.spidweb.bladesofexile.oldmacsave", @"com.spidweb.bladesofexile.oldwinsave", nil]];
	[dlg_get_game setMessage: @"Select a saved game to resume:"];
	[dlg_get_game setTitle: @"Load Game"];
	[dlg_get_game retain];
	
	dlg_put_scen = [NSSavePanel savePanel];
	[dlg_put_scen setAllowedFileTypes: [NSArray arrayWithObjects: @"com.spidweb.bladesofexile.scenario", nil]];
	[dlg_put_scen setMessage: @"Select a location to save the scenario:"];
	[dlg_put_scen setTitle: @"Save Scenario"];
	[dlg_put_scen retain];
	
	dlg_put_game = [NSSavePanel savePanel];
	[dlg_put_game setAllowedFileTypes: [NSArray arrayWithObjects: @"com.spidweb.bladesofexile.savegame", nil]];
	[dlg_put_game setMessage: @"Select a location to save your game:"];
	[dlg_put_game setTitle: @"Save Game"];
	[dlg_put_game retain];
}

fs::path nav_get_scenario() {
	bool gotFile = [dlg_get_scen runModal] != NSFileHandlingPanelCancelButton;
	makeFrontWindow(mainPtr);
	if(gotFile)
		return fs::path([[[dlg_get_scen URL] absoluteString] UTF8String]);
	return "";
}

fs::path nav_put_scenario() {
	bool gotFile = [dlg_put_scen runModal] != NSFileHandlingPanelCancelButton;
	makeFrontWindow(mainPtr);
	if(gotFile)
		return [[[dlg_put_scen URL] absoluteString] UTF8String];
	return "";
}

fs::path nav_get_party() {
	bool gotFile = [dlg_get_game runModal] != NSFileHandlingPanelCancelButton;
	makeFrontWindow(mainPtr);
	if(gotFile)
		return fs::path([[[dlg_get_game URL] absoluteString] UTF8String]);
	return "";
}

fs::path nav_put_party() {
	bool gotFile = [dlg_put_game runModal] != NSFileHandlingPanelCancelButton;
	makeFrontWindow(mainPtr);
	if(gotFile)
		return fs::path([[[dlg_put_game URL] absoluteString] UTF8String]);
	return "";
}
