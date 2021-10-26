//
//  boe.appleevents.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-03-26.
//
//

#include "universe.hpp" // Include before Cocoa because the Cocoa header defines things that cause compilation errors in here

#include <Cocoa/Cocoa.h>
#include <memory>
#include <algorithm>
#include <iostream>
#include "fileio.hpp"
#include "boe.global.hpp"
#include "choicedlog.hpp"

extern void finish_load_party();
extern void end_startup();
extern void post_load();

extern bool ae_loading, All_Done, party_in_memory, finished_init;
extern eGameMode overall_mode;
extern cUniverse univ;
extern bool pending_quit;
extern fs::path pending_file_to_load;

typedef NSAppleEventDescriptor AEDescr;

@interface AppleEventHandler : NSObject<NSApplicationDelegate>
-(BOOL)application:(NSApplication*) app openFile:(NSString*) file;
-(NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*) sender;
@end

void set_up_apple_events(int argc, char* argv[]); // Suppress "no prototype" warning
void set_up_apple_events(int, char*[]) {
	static AppleEventHandler* aeHandler;
	aeHandler = [[AppleEventHandler alloc] init];
	[[NSApplication sharedApplication] setDelegate: aeHandler];
}

@implementation AppleEventHandler
-(BOOL)application:(NSApplication*) app openFile:(NSString*) file {
	(void) app; // Suppress "unused parameter" warning
	if(file == nil) {
		std::cerr << "Error: filename was nil" << std::endl;
		return NO;
	}
	pending_file_to_load=file.fileSystemRepresentation;
	return YES;
}

// TODO: Something about the cChoiceDlog causes this to crash... AFTER returning.
-(NSApplicationTerminateReply)applicationShouldTerminate: (NSApplication*)sender {
	(void) sender; // Suppress "unused parameter" warning
	if(overall_mode == MODE_STARTUP && !party_in_memory) {
		All_Done = true;
		return NSTerminateNow;
	}
	// REMOVEME when we solve the causes of the crash
	//    note: this is actually very bad because we will cancel a shutdown,
	//          and this does not work if a dialog is opened, ...
	//          but at least this does not lead to a crash
	pending_quit=true;
	return NSTerminateCancel;
#if 0
	if(overall_mode == MODE_TOWN || overall_mode == MODE_OUTDOORS || (overall_mode == MODE_STARTUP && party_in_memory)) {
		std::string choice = cChoiceDlog("quit-confirm-save", {"save", "quit", "cancel"}).show();
		if(choice == "cancel") return NSTerminateCancel;
		if(choice == "save")
			save_party(univ.file, univ);
	} else {
		std::string choice = cChoiceDlog("quit-confirm-nosave", {"quit", "cancel"}).show();
		if(choice == "cancel") return NSTerminateCancel;
	}
	
	All_Done = true;
	return NSTerminateNow;
#endif
}
@end
