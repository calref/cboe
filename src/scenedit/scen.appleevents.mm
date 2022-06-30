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

//extern bool ae_loading, startup_loaded, All_Done, party_in_memory, finished_init;
extern cScenario scenario;
extern cOutdoors* current_terrain;
extern location cur_out;
extern bool change_made, ae_loading;

@interface AppleEventHandler : NSObject<NSApplicationDelegate>
-(BOOL)application:(NSApplication*) app openFile:(NSString*) file;
// TODO: Handle quit event by putting up quit dialog
//-(NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*) sender;
@end

void set_up_apple_events(int argc, char* argv[]); // Suppress "no prototype" warning
void set_up_apple_events(int, char*[]) {
	static AppleEventHandler* aeHandler;
	aeHandler = [[AppleEventHandler alloc] init];
	[[NSApplication sharedApplication] setDelegate: aeHandler];
}

// TODO: What if they're already in a scenario? It should ask for confirmation, right?
// (Need to figure out cChoiceDlog bug first, though, as it would crash here just like it does on the quit event.)
@implementation AppleEventHandler
-(BOOL)application:(NSApplication*) app openFile:(NSString*) file {
	(void) app; // Suppress "unused parameter" warning
	if(file == nil) {
		std::cerr << "Error: filename was nil" << std::endl;
		return FALSE;
	}
	
	std::string fileName=file.fileSystemRepresentation;

	if(load_scenario(fileName, scenario)) {
		set_current_town(scenario.last_town_edited);
		cur_out = scenario.last_out_edited;
		current_terrain = scenario.outdoors[cur_out.x][cur_out.y];
		change_made = false;
		ae_loading = true;
		set_up_main_screen();
	}
	return TRUE;
}
@end
