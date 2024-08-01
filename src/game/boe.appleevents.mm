//
//  boe.appleevents.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-03-26.
//
//

#include "universe/universe.hpp" // Include before Cocoa because the Cocoa header defines things that cause compilation errors in here
#include <Cocoa/Cocoa.h>
#include <memory>
#include <algorithm>
#include <iostream>
#include "fileio/fileio.hpp"
#include "boe.global.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "replay.hpp"

extern void finish_load_party();
extern void end_startup();
extern void post_load();
extern void queue_fake_event(const sf::Event&);

extern bool ae_loading, All_Done, party_in_memory, finished_init;
extern eGameMode overall_mode;
extern cUniverse univ;

typedef NSAppleEventDescriptor AEDescr;

@interface AppleEventHandler : NSObject<NSApplicationDelegate>
-(BOOL)application:(NSApplication*) app openFile:(NSString*) file;
-(NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*) sender;
@end

void set_up_apple_events(); // Suppress "no prototype" warning
void set_up_apple_events() {
	AppleEventHandler* aeHandler = [[AppleEventHandler alloc] init];
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
	
	unsigned long len = [file length], sz = len + 1;
	auto msg = std::shared_ptr<unichar>(new unichar[sz], std::default_delete<unichar[]>());
	std::fill(msg.get(), msg.get() + sz, 0);
	[file getCharacters: msg.get() range: (NSRange){0, len}];
	std::string fileName;
	std::copy(msg.get(), msg.get() + len, std::inserter(fileName, fileName.begin()));
	
	if(replaying || recording){
		return FALSE;
	}

	if(!load_party(fileName, univ))
		return FALSE;
	
	if(!finished_init) {
		ae_loading = true;
		overall_mode = MODE_STARTUP;
	} else finish_load_party();
	if(overall_mode != MODE_STARTUP)
		end_startup();
	if(overall_mode != MODE_STARTUP)
		post_load();
	return TRUE;
}

-(NSApplicationTerminateReply)applicationShouldTerminate: (NSApplication*)sender {
	(void) sender; // Suppress "unused parameter" warning

	// To avoid code duplication and fix a weird crash, cancel Apple's
	// termination event, but queue a fake sfml close event for
	// boe.main.cpp to handle on the next frame
	sf::Event event;
	event.type = sf::Event::Closed;
	queue_fake_event(event);

	return NSTerminateCancel;
}
@end
