//
//  pc.appleevents.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-04-16.
//
//

#include <Cocoa/Cocoa.h>

extern bool verify_restore_quit(bool mode);
extern bool All_Done;

typedef NSAppleEventDescriptor AEDescr;

@interface AppleEventHandler : NSObject
-(void)handleOpenDoc:(AEDescr*)theAppleEvent withReply: (AEDescr*)reply;
-(void)handleQuit:(AEDescr*)theAppleEvent withReply: (AEDescr*)reply;
@end

void set_up_apple_events(); // Suppress "no prototype" warning
void set_up_apple_events() {
	AppleEventHandler* aeHandler = [[AppleEventHandler alloc] init];
	NSAppleEventManager* AEmgr = [NSAppleEventManager sharedAppleEventManager];
	[AEmgr setEventHandler: aeHandler andSelector: @selector(handleOpenDoc:withReply:)
		forEventClass: kCoreEventClass andEventID: kAEOpenDocuments];
	[AEmgr setEventHandler: aeHandler andSelector: @selector(handleQuit:withReply:)
		forEventClass: kCoreEventClass andEventID: kAEQuitApplication];
}

@implementation AppleEventHandler
-(void)handleOpenDoc:(AEDescr*)theAppleEvent withReply: (AEDescr*)reply {
	(void) theAppleEvent; // Suppress "unused parameter" warning
	(void) reply;
	// TODO: Handle this
}
-(void)handleQuit:(AEDescr*)theAppleEvent withReply: (AEDescr*)reply {
	(void) theAppleEvent; // Suppress "unused parameter" warning
	(void) reply;
	All_Done = verify_restore_quit(0);
}
@end

