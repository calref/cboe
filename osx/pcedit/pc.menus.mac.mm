//
//  pc.menus.mac.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-04-15.
//
//

#include "pc.menus.h"
#include <Cocoa/Cocoa.h>
#include "item.h"
#include "universe.h"

#ifndef __APPLE__
#error pc.menus.mm is Mac-specific code; try compiling pc.menus.win.cpp instead
#endif

using MenuHandle = NSMenu*;

extern void handle_apple_menu(int item_hit);
extern void handle_file_menu(int item_hit);
extern void handle_extra_menu(int item_hit);
extern void handle_edit_menu(int item_hit);
extern void handle_item_menu(int item_hit);

extern cUniverse univ;
extern fs::path file_in_mem;
extern bool scen_items_loaded;
MenuHandle menu_bar_handle;
MenuHandle apple_menu, file_menu, reg_menu, extra_menu, items_menu[4];

@interface MenuHandler : NSObject
-(void) fileMenu:(id) sender;
-(void) freeMenu:(id) sender;
-(void) specMenu:(id) sender;
-(void) itemMenu:(id) sender;
-(void) helpMenu:(id) sender;
@end

@interface ItemWrapper : NSObject
+(id) withItem:(int) theItem;
-(cItemRec&) item;
-(void) setItem:(int) theItem;
@end

static void setMenuCallback(NSMenuItem* item, id targ, SEL selector, int num) {
	[item setTarget: targ];
	[item setAction: selector];
	[item setRepresentedObject: [[NSNumber numberWithInt: num] retain]];
}

void init_menubar() {
	NSApplication* app = [NSApplication sharedApplication];
	[NSBundle loadNibNamed: @"pc.menu" owner: app];
	menu_bar_handle = [app mainMenu];
	
	apple_menu = [[menu_bar_handle itemWithTitle: @"BoE Character Editor"] submenu];
	file_menu = [[menu_bar_handle itemWithTitle: @"File"] submenu];
	reg_menu = [[menu_bar_handle itemWithTitle: @"Free Extras"] submenu];
	extra_menu = [[menu_bar_handle itemWithTitle: @"Special Edit"] submenu];
	items_menu[0] = [[menu_bar_handle itemWithTitle: @"Items 1"] submenu];
	items_menu[1] = [[menu_bar_handle itemWithTitle: @"Items 2"] submenu];
	items_menu[2] = [[menu_bar_handle itemWithTitle: @"Items 3"] submenu];
	items_menu[3] = [[menu_bar_handle itemWithTitle: @"Items 4"] submenu];
	
	MenuHandler* handler = [[[MenuHandler alloc] init] retain];
	setMenuCallback([apple_menu itemWithTitle: @"About BoE Character Editor"], handler, @selector(helpMenu:), 1);
	setMenuCallback([apple_menu itemWithTitle: @"Quit BoE Character Editor"], handler, @selector(fileMenu:), 7);
	// TODO: Organize the file menu handling function
	setMenuCallback([file_menu itemWithTitle: @"Save"], handler, @selector(fileMenu:), 1);
	setMenuCallback([file_menu itemWithTitle: @"Save As…"], handler, @selector(fileMenu:), 2);
	setMenuCallback([file_menu itemWithTitle: @"Open…"], handler, @selector(fileMenu:), 3);
	
	for(int i = 0; i < [reg_menu numberOfItems]; i++)
		setMenuCallback([reg_menu itemAtIndex: i], handler, @selector(freeMenu:), i + 1);
	for(int i = 0; i < [extra_menu numberOfItems]; i++)
		setMenuCallback([extra_menu itemAtIndex: i], handler, @selector(specMenu:), i + 1);
	
	update_item_menu();
	menu_activate();
}

void menu_activate() {
	if(file_in_mem.empty())
		for(int i = 3; i < [file_menu numberOfItems]; i++)
			[[file_menu itemAtIndex: i] setEnabled: NO];
	else for(int i = 3; i < [file_menu numberOfItems]; i++)
		[[file_menu itemAtIndex: i] setEnabled: YES];
}

void update_item_menu() {
	id targ = [[file_menu itemAtIndex: 0] target];
	cItemRec(& item_list)[400] = univ.scenario.scen_items;
	for(int j = 0; j < 4; j++){
		[items_menu[j] removeAllItems];
		if(!scen_items_loaded) {
			[[items_menu[j] addItemWithTitle: @"Items Not Loaded" action: @selector(itemMenu:) keyEquivalent: @""] setEnabled: NO];
		} else for(int i = 0; i < 100; i++) {
			ItemWrapper* item = [ItemWrapper withItem: i + 100 * j];
			NSString* item_name = [NSString stringWithCString: item_list[i + j * 100].full_name.c_str() encoding: NSASCIIStringEncoding];
			NSMenuItem* choice = [items_menu[j] addItemWithTitle: item_name action: @selector(itemMenu:) keyEquivalent: @""];
			[choice setTarget: targ];
			// TODO: Also disable gold or food
			[choice setEnabled: [item item].variety != eItemType::NO_ITEM];
			[choice setRepresentedObject: item];
		}
	}
}

@implementation MenuHandler
-(void) fileMenu:(id) sender {
	handle_file_menu([[sender representedObject] shortValue]);
}

-(void) freeMenu:(id) sender {
	handle_extra_menu([[sender representedObject] shortValue]);
}

-(void) specMenu:(id) sender {
	handle_edit_menu([[sender representedObject] shortValue]);
}

-(void) itemMenu:(id) sender {
	ItemWrapper* item = [sender representedObject];
	cItemRec& theItem = [item item];
	(void) theItem; // Suppress "unused parameter" warning
	for(int i = 0; i < 4; i++) {
		int whichItem = [items_menu[i] indexOfItem: sender];
		if(whichItem >= 0)
			handle_item_menu(whichItem + 100 * i);
	}
}

-(void) helpMenu:(id) sender {
	int i = [[sender representedObject] intValue];
	if(i == 0); // TODO: "BoE Character Editor Help"
	else if(i == 1) handle_apple_menu(i);
}

@end

@implementation ItemWrapper {
	int itemID;
}

+(id) withItem:(int) theItem {
	ItemWrapper* item = [[ItemWrapper alloc] init];
	[item setItem: theItem];
	return item;
}

-(void) setItem:(int) theItem {
	self->itemID = theItem;
}

-(cItemRec&) item {
	return univ.scenario.scen_items[self->itemID];
}

@end
