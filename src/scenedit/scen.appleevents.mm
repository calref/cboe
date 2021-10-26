//
//  boe.appleevents.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-03-26.
//
//

#include "scenario.hpp" // Include before Cocoa because the Cocoa header defines things that cause compilation errors in here
#include <Cocoa/Cocoa.h>
#include <memory>
#include <algorithm>
#include <iostream>
#include "fileio.hpp"
#include "scen.actions.hpp"
#include "scen.townout.hpp"

extern bool pending_quit, change_made, All_Done;
extern fs::path pending_file_to_load;

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

// TODO: What if they're already in a scenario? It should ask for confirmation, right?
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

-(NSApplicationTerminateReply)applicationShouldTerminate: (NSApplication*)sender {
	// REMOVEME when we solve the causes of the crash
	//    note: this is actually very bad because we will cancel a shutdown,
	//          and this does not work if a dialog is opened, ...
	//          but at least this does not lead to a crash
	if (!change_made) {
		All_Done = true;
		return NSTerminateNow;
	}
	pending_quit=true;
	return NSTerminateCancel;
}
@end
