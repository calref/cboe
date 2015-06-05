//
//  pc.appleevents.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-04-16.
//
//

#include "universe.h" // Include before Cocoa because the Cocoa header defines things that cause compilation errors in here
#include <Cocoa/Cocoa.h>
#include <string>
#include "fileio.hpp"
#include "pc.menus.h"
#include "pc.fileio.h"

extern bool verify_restore_quit(std::string dlog);
extern bool All_Done, party_in_scen, scen_items_loaded;
extern cUniverse univ;
extern fs::path file_in_mem;

@interface AppleEventHandler : NSObject<NSApplicationDelegate>
-(BOOL)application:(NSApplication*) app openFile:(NSString*) file;
-(NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*) sender;
@end

void set_up_apple_events(); // Suppress "no prototype" warning
void set_up_apple_events() {
	AppleEventHandler* aeHandler = [[AppleEventHandler alloc] init];
	[[NSApplication sharedApplication] setDelegate: aeHandler];
}

@implementation AppleEventHandler
-(BOOL)application:(NSApplication*) app openFile:(NSString*) file {
	(void) app; // Suppress "unused parameter" warning
	
	unsigned long len = [file length], sz = len + 1;
	auto msg = std::shared_ptr<unichar>(new unichar[sz], std::default_delete<unichar[]>());
	std::fill(msg.get(), msg.get() + sz, 0);
	[file getCharacters: msg.get() range: (NSRange){0, len}];
	std::string fileName;
	std::copy(msg.get(), msg.get() + len, std::inserter(fileName, fileName.begin()));
	
	if(load_party(fileName, univ)) {
		file_in_mem = fileName;
		party_in_scen = !univ.party.scen_name.empty();
		if(!party_in_scen) load_base_item_defs();
		scen_items_loaded = true;
		update_item_menu();
		menu_activate();
	}
	return TRUE;
}

-(NSApplicationTerminateReply)applicationShouldTerminate: (NSApplication*)sender {
	(void) sender; // Suppress "unused parameter" warning
	All_Done = verify_restore_quit("save-quit");
	return All_Done;
}
@end

