//
//  scen.menus.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-04-22.
//
//

#include "scen.menus.h"
#include "scenario.h" // Include before Cocoa because the Cocoa header defines things that cause compilation errors in here
#include <Cocoa/Cocoa.h>
#include "winutil.hpp"

using MenuHandle = NSMenu*;
MenuHandle menu_bar_handle;
MenuHandle item_menu[5], mon_menu[4];
MenuHandle file_menu, edit_menu, app_menu, scen_menu, town_menu, out_menu, help_menu;

extern cScenario scenario;

@interface MenuHandler : NSObject
-(void) menuChoice:(id) sender;
-(void) itemMenu:(id) sender;
-(void) monstMenu:(id) sender;
@end

static void setMenuCallback(NSMenuItem* item, id targ, SEL selector, int num) {
	[item setTarget: targ];
	[item setAction: selector];
	[item setRepresentedObject: [[NSNumber numberWithInt: num] retain]];
}

void init_menubar() {
	static bool inited = false;
	if(inited) return;
	inited = true;
	
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
	
	static const eMenu file_choices[] = {
		eMenu::FILE_NEW, eMenu::FILE_OPEN, eMenu::NONE, eMenu::FILE_CLOSE, eMenu::FILE_SAVE, eMenu::FILE_REVERT,
	};
	static const eMenu edit_choices[] = {
		eMenu::EDIT_UNDO, eMenu::EDIT_REDO, eMenu::NONE,
		eMenu::EDIT_CUT, eMenu::EDIT_COPY, eMenu::EDIT_PASTE, eMenu::EDIT_DELETE, eMenu::EDIT_SELECT_ALL,
	};
	static const eMenu scen_choices[] = {
		eMenu::TOWN_CREATE, eMenu::NONE, eMenu::SCEN_DETAILS, eMenu::SCEN_INTRO, eMenu::TOWN_START, eMenu::SCEN_PICS, eMenu::NONE, eMenu::NONE,
		eMenu::SCEN_SPECIALS, eMenu::SCEN_TEXT, eMenu::SCEN_JOURNALS, eMenu::TOWN_IMPORT, eMenu::SCEN_SAVE_ITEM_RECTS,
		eMenu::SCEN_HORSES, eMenu::SCEN_BOATS, eMenu::TOWN_VARYING, eMenu::SCEN_TIMERS, eMenu::SCEN_ITEM_SHORTCUTS,
		eMenu::TOWN_DELETE, eMenu::SCEN_DATA_DUMP, eMenu::SCEN_TEXT_DUMP,
	};
	static const eMenu town_choices[] = {
		eMenu::TOWN_DETAILS, eMenu::TOWN_WANDERING, eMenu::TOWN_BOUNDARIES, eMenu::FRILL, eMenu::UNFRILL, eMenu::TOWN_AREAS,
		eMenu::NONE, eMenu::TOWN_ITEMS_RANDOM, eMenu::TOWN_ITEMS_NOT_PROPERTY, eMenu::TOWN_ITEMS_CLEAR, eMenu::NONE, eMenu::NONE,
		eMenu::TOWN_SPECIALS, eMenu::TOWN_TEXT, eMenu::TOWN_SIGNS, eMenu::TOWN_ADVANCED, eMenu::TOWN_TIMERS,
	};
	static const eMenu out_choices[] = {
		eMenu::OUT_DETAILS, eMenu::OUT_WANDERING, eMenu::OUT_ENCOUNTERS, eMenu::FRILL, eMenu::UNFRILL, eMenu::OUT_AREAS,
		eMenu::NONE, eMenu::OUT_START, eMenu::NONE, eMenu::NONE,
		eMenu::OUT_SPECIALS, eMenu::OUT_TEXT, eMenu::OUT_SIGNS,
	};
	static const eMenu help_choices[] = {
		eMenu::HELP_TOC, eMenu::NONE, eMenu::HELP_START, eMenu::HELP_TEST, eMenu::HELP_DIST, eMenu::NONE, eMenu::HELP_CONTEST,
	};
	
	MenuHandler* handler = [[[MenuHandler alloc] init] retain];
	setMenuCallback([app_menu itemWithTitle: @"About BoE Scenario Editor"], handler, @selector(menuChoice:), int(eMenu::ABOUT));
	setMenuCallback([app_menu itemWithTitle: @"Quit BoE Scenario Editor"], handler, @selector(menuChoice:), int(eMenu::QUIT));
	
	int i = 0;
	for(eMenu opt : file_choices)
		setMenuCallback([file_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : edit_choices)
		setMenuCallback([edit_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : scen_choices)
		setMenuCallback([scen_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : town_choices)
		setMenuCallback([town_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : out_choices)
		setMenuCallback([out_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : help_choices)
		setMenuCallback([help_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
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

@implementation MenuHandler
-(void) menuChoice:(id) sender {
	handle_menu_choice(eMenu([[sender representedObject] intValue]));
}

// TODO: Monster and item menus
-(void) itemMenu:(id) sender {
	(void) sender; // Suppress "unused parameter" warning
}

-(void) monstMenu:(id) sender {
	(void) sender; // Suppress "unused parameter" warning
}
@end
