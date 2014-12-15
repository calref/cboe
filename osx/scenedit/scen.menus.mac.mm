//
//  scen.menus.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-04-22.
//
//

#include "scen.menus.h"
#include <Cocoa/Cocoa.h>
#include "scenario.h"

using MenuHandle = NSMenu*;
MenuHandle menu_bar_handle;
MenuHandle item_menu[5], mon_menu[4];
MenuHandle file_menu, edit_menu, app_menu, scen_menu, town_menu, out_menu, help_menu;

extern cScenario scenario;

@interface MenuHandler : NSObject
-(void) appMenu:(id) sender;
-(void) fileMenu:(id) sender;
-(void) editMenu:(id) sender;
-(void) scenMenu:(id) sender;
-(void) townMenu:(id) sender;
-(void) outMenu:(id) sender;
-(void) itemMenu:(id) sender;
-(void) monstMenu:(id) sender;
-(void) helpMenu:(id) sender;
-(void) onlineHelp:(id) sender;
@end

static void setMenuCallback(NSMenuItem* item, id targ, SEL selector, int num) {
	[item setTarget: targ];
	[item setAction: selector];
	[item setRepresentedObject: [[NSNumber numberWithInt: num] retain]];
}

void init_menubar() {
	NSApplication* app = [NSApplication sharedApplication];
	[NSBundle loadNibNamed: @"scen.menu" owner: app];
	menu_bar_handle = [app mainMenu];
	
	app_menu = [[menu_bar_handle itemWithTitle: @"BoE Scenario Editor"] submenu];
	file_menu = [[menu_bar_handle itemWithTitle: @"File"] submenu];
	edit_menu = [[menu_bar_handle itemWithTitle: @"Edit"] submenu];
	scen_menu = [[menu_bar_handle itemWithTitle: @"Scenario"] submenu];
	town_menu = [[menu_bar_handle itemWithTitle: @"Town"] submenu];
	out_menu = [[menu_bar_handle itemWithTitle: @"Outdoors"] submenu];
	help_menu = [[menu_bar_handle itemWithTitle: @"Help"] submenu];
	item_menu[0] = [[menu_bar_handle itemWithTitle: @"I1"] submenu];
	item_menu[1] = [[menu_bar_handle itemWithTitle: @"I2"] submenu];
	item_menu[2] = [[menu_bar_handle itemWithTitle: @"I3"] submenu];
	item_menu[3] = [[menu_bar_handle itemWithTitle: @"I4"] submenu];
	item_menu[4] = [[menu_bar_handle itemWithTitle: @"I5"] submenu];
	mon_menu[0] = [[menu_bar_handle itemWithTitle: @"M1"] submenu];
	mon_menu[1] = [[menu_bar_handle itemWithTitle: @"M2"] submenu];
	mon_menu[2] = [[menu_bar_handle itemWithTitle: @"M3"] submenu];
	mon_menu[3] = [[menu_bar_handle itemWithTitle: @"M4"] submenu];
	
	MenuHandler* handler = [[[MenuHandler alloc] init] retain];
	setMenuCallback([app_menu itemWithTitle: @"About BoE Scenario Editor"], handler, @selector(appMenu:), 1);
	setMenuCallback([app_menu itemWithTitle: @"Quit BoE Scenario Editor"], handler, @selector(fileMenu:), 5);
	// TODO: Organize file menu function
	setMenuCallback([file_menu itemWithTitle: @"New Scenario…"], handler, @selector(fileMenu:), 3);
	setMenuCallback([file_menu itemWithTitle: @"Open…"], handler, @selector(fileMenu:), 1);
	setMenuCallback([file_menu itemWithTitle: @"Save"], handler, @selector(fileMenu:), 2);
	setMenuCallback([help_menu itemAtIndex: 0], handler, @selector(onlineHelp:), 0);
	
	for(int i = 0; i < [edit_menu numberOfItems]; i++)
		setMenuCallback([edit_menu itemAtIndex: i], handler, @selector(editMenu:), i + 1);
	for(int i = 0; i < [scen_menu numberOfItems]; i++)
		setMenuCallback([scen_menu itemAtIndex: i], handler, @selector(scenMenu:), i + 1);
	for(int i = 0; i < [town_menu numberOfItems]; i++)
		setMenuCallback([town_menu itemAtIndex: i], handler, @selector(townMenu:), i + 1);
	for(int i = 0; i < [out_menu numberOfItems]; i++)
		setMenuCallback([out_menu itemAtIndex: i], handler, @selector(outMenu:), i + 1);
	for(int i = 2; i < [help_menu numberOfItems]; i++)
		setMenuCallback([help_menu itemAtIndex: i], handler, @selector(helpMenu:), i - 1);
	// TODO: Item and monster menus
}

// mode 0 - initial shut down, 1 - no town, 2 - no out, 3 - no town or out 4 - all menus on
// TODO: Use an enum here
void shut_down_menus(short mode) {
	if(mode == 0) {
		[[file_menu itemWithTitle: @"Save"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Scenario"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Town"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Outdoors"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"I1"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"I2"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"I3"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"I4"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"I5"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"M1"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"M2"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"M3"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"M4"] setEnabled: NO];
	}
	if(mode == 4) {
		[[file_menu itemWithTitle: @"Save"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"Scenario"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"Town"] setEnabled: YES];
		MenuHandle town_menu = [[menu_bar_handle itemWithTitle: @"Town"] submenu];
		for(id item in [town_menu itemArray])
			if(![[item title] isEqualToString: @"Advanced:"])
				[item setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"Outdoors"] setEnabled: YES];
		MenuHandle out_menu = [[menu_bar_handle itemWithTitle: @"Outdoors"] submenu];
		for(id item in [out_menu itemArray])
			if(![[item title] isEqualToString: @"Advanced:"])
				[item setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"I1"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"I2"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"I3"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"I4"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"I5"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"M1"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"M2"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"M3"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"M4"] setEnabled: YES];
	}
	if((mode == 1) || (mode == 3)) {
		MenuHandle town_menu = [[menu_bar_handle itemWithTitle: @"Town"] submenu];
		for(id item in [town_menu itemArray])
			if([[item title] length] > 0 && [[item title] characterAtIndex: 0] != ' ')
				[item setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"I1"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"I2"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"I3"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"I4"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"I5"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"M1"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"M2"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"M3"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"M4"] setEnabled: NO];
	}
	if((mode == 2) || (mode == 3)) {
		MenuHandle out_menu = [[menu_bar_handle itemWithTitle: @"Outdoors"] submenu];
		for(id item in [out_menu itemArray])
			if([[item title] length] > 0 && [[item title] characterAtIndex: 0] != ' ')
				[item setEnabled: NO];
	}
}

void update_item_menu() {
	MenuHandler* targ = [[file_menu itemAtIndex: 0] target];
	short i,j;
	
	for(j = 0; j < 5; j++) {
		[item_menu[j] removeAllItems];
		for(i = 0; i < 80; i++) {
			NSString* title = [NSString stringWithCString: scenario.scen_items[i + j * 80].full_name.c_str() encoding: NSASCIIStringEncoding];
			NSMenuItem* newItem = [item_menu[j] addItemWithTitle: title action: @selector(itemMenu:) keyEquivalent:@""];
			[newItem setTarget: targ];
		}
	}
	for(j = 0; j < 4; j++) {
		[mon_menu[j] removeAllItems];
		for(i = 0; i < 64; i++) {
			NSString* title = [NSString stringWithCString: scenario.scen_monsters[i + j * 64].m_name.c_str() encoding: NSASCIIStringEncoding];
			NSMenuItem* newItem = [mon_menu[j] addItemWithTitle: title action: @selector(monstMenu:) keyEquivalent: @""];
			[newItem setTarget: targ];
		}
	}
	
}

void handle_apple_menu(int item_hit);
void handle_file_menu(int item_hit);
void handle_scenario_menu(int item_hit);
void handle_town_menu(int item_hit);
void handle_outdoor_menu(int item_hit);
void handle_help_menu(int item_hit);
void handle_item_menu(int item_hit);
void handle_monst_menu(int item_hit);

@implementation MenuHandler
-(void) appMenu:(id) sender {
	handle_apple_menu([[sender representedObject] intValue]);
}

-(void) fileMenu:(id) sender {
	handle_file_menu([[sender representedObject] intValue]);
}

// TODO: Implement edit menu (much work to be done here!)
-(void) editMenu:(id) sender {
	(void) sender; // Suppress "unused parameter" warning
}

-(void) scenMenu:(id) sender {
	handle_scenario_menu([[sender representedObject] intValue]);
}

-(void) townMenu:(id) sender {
	handle_town_menu([[sender representedObject] intValue]);
}

-(void) outMenu:(id) sender {
	handle_outdoor_menu([[sender representedObject] intValue]);
}

// TODO: Monster and item menus
-(void) itemMenu:(id) sender {
	(void) sender; // Suppress "unused parameter" warning
}

-(void) monstMenu:(id) sender {
	(void) sender; // Suppress "unused parameter" warning
}

-(void) helpMenu:(id) sender {
	handle_help_menu([[sender representedObject] intValue]);
}

-(void) onlineHelp:(id) sender {
	(void) sender;
	[[NSWorkspace sharedWorkspace] openURL: [NSURL URLWithString: @"https://calref.net/~sylae/boe-doc/editor/About.html"]];
}
@end
