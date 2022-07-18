//
//  pc.appleevents.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-04-16.
//
//

#include "universe/universe.hpp" // Include before Cocoa because the Cocoa header defines things that cause compilation errors in here
#include <Cocoa/Cocoa.h>
#include <iostream>
#include <string>
#include "fileio/fileio.hpp"
#include "pc.menus.hpp"
#include "pc.fileio.hpp"

extern bool All_Done, party_in_scen;
extern bool pending_quit;
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
	(void) sender; // Suppress "unused parameter" warning
	// REMOVEME when we solve the causes of the crash
	//    note: this is actually very bad because we will cancel a shutdown,
	//          and this does not work if a dialog is opened, ...
	//          but at least this does not lead to a crash
	if (!party_in_scen) {
		All_Done = true;
		return NSTerminateNow;
	}
	pending_quit=true;
	return NSTerminateCancel;
#if 0
	All_Done = verify_restore_quit("save-quit");
	return All_Done ? NSTerminateNow : NSTerminateCancel;
#endif
}
@end

