//
//  pc.menus.mac.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-04-15.
//
//

#include "pc.menus.hpp"
#include "universe/universe.hpp" // Include before Cocoa because the Cocoa header defines things that cause compilation errors in here
#include <Cocoa/Cocoa.h>
#include "scenario/item.hpp"

#ifndef __APPLE__
#error pc.menus.mm is Mac-specific code; try compiling pc.menus.win.cpp instead
#endif

extern short menuChoiceId;

using MenuHandle = NSMenu*;

extern cUniverse univ;
extern fs::path file_in_mem;
extern bool scen_items_loaded, party_in_scen;
MenuHandle menu_bar_handle;
MenuHandle apple_menu, file_menu, reg_menu, extra_menu;

@interface MenuHandler : NSObject
-(void) menuChoice:(id) sender;
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
	[NSBundle loadNibNamed: @"pcedit" owner: app];
	menu_bar_handle = [app mainMenu];
	
	apple_menu = [[menu_bar_handle itemWithTitle: @"BoE Character Editor"] submenu];
	file_menu = [[menu_bar_handle itemWithTitle: @"File"] submenu];
	reg_menu = [[menu_bar_handle itemWithTitle: @"Party"] submenu];
	extra_menu = [[menu_bar_handle itemWithTitle: @"Scenario"] submenu];
	
	static const eMenu file_choices[] = {
		eMenu::FILE_OPEN, eMenu::FILE_CLOSE, eMenu::NONE, eMenu::FILE_SAVE, eMenu::FILE_SAVE_AS, eMenu::FILE_REVERT,
	};
	static const eMenu party_choices[] = {
		eMenu::EDIT_GOLD, eMenu::EDIT_FOOD, eMenu::EDIT_ALCHEMY, eMenu::NONE,
		eMenu::HEAL_DAMAGE, eMenu::RESTORE_MANA, eMenu::RAISE_DEAD, eMenu::CURE_CONDITIONS, eMenu::NONE,
		eMenu::EDIT_MAGE, eMenu::EDIT_PRIEST, eMenu::EDIT_ITEM, eMenu::EDIT_TRAITS, eMenu::EDIT_SKILLS, eMenu::EDIT_XP, eMenu::NONE,
		eMenu::REUNITE_PARTY,
	};
	static const eMenu scen_choices[] = {
		eMenu::EDIT_DAY, eMenu::NONE, eMenu::LEAVE_TOWN, eMenu::RESET_TOWNS, eMenu::ADD_OUT_MAPS, eMenu::ADD_TOWN_MAPS, eMenu::OWN_VEHICLES,
		eMenu::NONE, eMenu::LEAVE_SCENARIO, eMenu::SET_SDF,
	};
	
	MenuHandler* handler = [[[MenuHandler alloc] init] retain];
	setMenuCallback([apple_menu itemWithTitle: @"About BoE Character Editor"], handler, @selector(menuChoice:), int(eMenu::ABOUT));
	setMenuCallback([apple_menu itemWithTitle: @"Preferences…"], handler, @selector(menuChoice:), int(eMenu::PREFS));
	setMenuCallback([apple_menu itemWithTitle: @"Quit BoE Character Editor"], handler, @selector(menuChoice:), int(eMenu::QUIT));
	setMenuCallback([[[menu_bar_handle itemWithTitle: @"Help"] submenu] itemAtIndex: 0], handler, @selector(menuChoice:), int(eMenu::HELP_TOC));
	
	int i = 0;
	for(eMenu opt : file_choices)
		setMenuCallback([file_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : party_choices)
		setMenuCallback([reg_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : scen_choices)
		setMenuCallback([extra_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	
	menu_activate();
}

void menu_activate() {
	if(file_in_mem.empty()) {
		for(int i = 3; i < [file_menu numberOfItems]; i++)
			[[file_menu itemAtIndex: i] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Edit Party"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Scenario Edit"] setEnabled: NO];
	} else {
		for(int i = 3; i < [file_menu numberOfItems]; i++)
			[[file_menu itemAtIndex: i] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"Edit Party"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"Scenario Edit"] setEnabled: party_in_scen];
	}
}

bool menuBarProcessEvent(const sf::Event&) {
	return false;
}

void drawMenuBar() {
}

@implementation MenuHandler
-(void) menuChoice:(id) sender {
	menuChoiceId=short([[sender representedObject] intValue]);
	//eMenu opt = eMenu([[sender representedObject] intValue]);
	//handle_menu_choice(opt);
}
@end
