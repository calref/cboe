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
MenuHandle file_menu, edit_menu;

extern cScenario scenario;

@interface MenuHandler : NSObject

@end

void init_menubar() {
	NSApplication* app = [NSApplication sharedApplication];
	[NSBundle loadNibNamed: @"scen.menu" owner: app];
	menu_bar_handle = [app mainMenu];
	
	item_menu[0] = [[menu_bar_handle itemWithTitle: @"I1"] submenu];
	item_menu[1] = [[menu_bar_handle itemWithTitle: @"I2"] submenu];
	item_menu[2] = [[menu_bar_handle itemWithTitle: @"I3"] submenu];
	item_menu[3] = [[menu_bar_handle itemWithTitle: @"I4"] submenu];
	item_menu[4] = [[menu_bar_handle itemWithTitle: @"I5"] submenu];
	mon_menu[0] = [[menu_bar_handle itemWithTitle: @"M1"] submenu];
	mon_menu[1] = [[menu_bar_handle itemWithTitle: @"M2"] submenu];
	mon_menu[2] = [[menu_bar_handle itemWithTitle: @"M3"] submenu];
	mon_menu[3] = [[menu_bar_handle itemWithTitle: @"M4"] submenu];
}

// mode 0 - initial shut down, 1 - no town, 2 - no out, 3 - no town or out 4 - all menus on
// TODO: Use an enum here
void shut_down_menus(short mode) {
	if (mode == 0) {
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
	if  (mode == 4) {
		[[[[menu_bar_handle itemWithTitle: @"File"] submenu] itemWithTitle: @"Save"] setEnabled: YES];
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
	if ((mode == 1) || (mode == 3)) {
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
	if ((mode == 2) || (mode == 3)) {
		MenuHandle out_menu = [[menu_bar_handle itemWithTitle: @"Outdoors"] submenu];
		for(id item in [out_menu itemArray])
			if([[item title] length] > 0 && [[item title] characterAtIndex: 0] != ' ')
				[item setEnabled: NO];
	}
}

void update_item_menu() {
	MenuHandler* targ = [[file_menu itemAtIndex: 0] target];
	short i,j;
	char item_name[256];
	
	for (j = 0; j < 5; j++) {
		[item_menu[j] removeAllItems];
		for (i = 0; i < 80; i++) {
			NSString* title = [NSString stringWithCString: scenario.scen_items[i + j * 80].full_name.c_str() encoding: NSASCIIStringEncoding];
			NSMenuItem* newItem = [item_menu[j] addItemWithTitle: title action: @selector(itemMenu:) keyEquivalent:@""];
			[newItem setTarget: targ];
		}
	}
	for (j = 0; j < 4; j++) {
		[mon_menu[j] removeAllItems];
		for (i = 0; i < 64; i++) {
			NSString* title = [NSString stringWithCString: scenario.scen_monsters[i + j * 64].m_name.c_str() encoding: NSASCIIStringEncoding];
			NSMenuItem* newItem = [mon_menu[j] addItemWithTitle: title action: @selector(monstMenu:) keyEquivalent: @""];
			[newItem setTarget: targ];
		}
	}
	
}
