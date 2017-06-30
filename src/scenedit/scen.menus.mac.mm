//
//  scen.menus.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-04-22.
//
//

#include "scen.menus.hpp"
#include "scenario.hpp" // Include before Cocoa because the Cocoa header defines things that cause compilation errors in here
#include <Cocoa/Cocoa.h>
#include "winutil.hpp"
#include "undo.hpp"

using MenuHandle = NSMenu*;
MenuHandle menu_bar_handle;
MenuHandle file_menu, edit_menu, app_menu, scen_menu, town_menu, out_menu, help_menu;

extern cScenario scenario;
extern cUndoList undo_list;

@interface MenuHandler : NSObject
-(IBAction) menuChoice:(id) sender;
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
	[NSBundle loadNibNamed: @"scenedit" owner: app];
	menu_bar_handle = [app mainMenu];
	
	app_menu = [[menu_bar_handle itemWithTitle: @"BoE Scenario Editor"] submenu];
	file_menu = [[menu_bar_handle itemWithTitle: @"File"] submenu];
	edit_menu = [[menu_bar_handle itemWithTitle: @"Edit"] submenu];
	scen_menu = [[menu_bar_handle itemWithTitle: @"Scenario"] submenu];
	town_menu = [[menu_bar_handle itemWithTitle: @"Town"] submenu];
	out_menu = [[menu_bar_handle itemWithTitle: @"Outdoors"] submenu];
	help_menu = [[menu_bar_handle itemWithTitle: @"Help"] submenu];
	
	static const eMenu file_choices[] = {
		eMenu::FILE_NEW, eMenu::FILE_OPEN, eMenu::NONE, eMenu::FILE_CLOSE, eMenu::FILE_SAVE, eMenu::FILE_SAVE_AS, eMenu::FILE_REVERT,
	};
	static const eMenu edit_choices[] = {
		eMenu::EDIT_UNDO, eMenu::EDIT_REDO, eMenu::NONE,
		eMenu::EDIT_CUT, eMenu::EDIT_COPY, eMenu::EDIT_PASTE, eMenu::EDIT_DELETE, eMenu::EDIT_SELECT_ALL,
	};
	static const eMenu scen_choices[] = {
		eMenu::TOWN_CREATE, eMenu::OUT_RESIZE, eMenu::NONE,
		eMenu::SCEN_DETAILS, eMenu::SCEN_INTRO, eMenu::SCEN_SHEETS, eMenu::SCEN_PICS, eMenu::SCEN_SNDS, eMenu::NONE, eMenu::NONE,
		eMenu::SCEN_SPECIALS, eMenu::SCEN_TEXT, eMenu::SCEN_JOURNALS, eMenu::TOWN_IMPORT, eMenu::OUT_IMPORT, eMenu::SCEN_SAVE_ITEM_RECTS,
		eMenu::TOWN_VARYING, eMenu::SCEN_TIMERS, eMenu::SCEN_ITEM_SHORTCUTS,
		eMenu::TOWN_DELETE, eMenu::SCEN_DATA_DUMP, eMenu::SCEN_TEXT_DUMP,
	};
	static const eMenu town_choices[] = {
		eMenu::TOWN_DETAILS, eMenu::TOWN_WANDERING, eMenu::TOWN_BOUNDARIES, eMenu::FRILL, eMenu::UNFRILL, eMenu::TOWN_AREAS,
		eMenu::NONE, eMenu::TOWN_START, eMenu::TOWN_ITEMS_RANDOM, eMenu::TOWN_ITEMS_NOT_PROPERTY, eMenu::TOWN_ITEMS_CLEAR,
		eMenu::NONE, eMenu::NONE,
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
// 5 - disable file/edit menus
// TODO: Use an enum here
void shut_down_menus(short mode) {
	if(mode == 0) {
		[[file_menu itemWithTitle: @"Save"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Scenario"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Town"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Outdoors"] setEnabled: NO];
	}
	if(mode == 4) {
		[[menu_bar_handle itemWithTitle: @"File"] setEnabled: YES];
		[[menu_bar_handle itemWithTitle: @"Edit"] setEnabled: YES];
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
	}
	if((mode == 1) || (mode == 3)) {
		MenuHandle town_menu = [[menu_bar_handle itemWithTitle: @"Town"] submenu];
		for(id item in [town_menu itemArray])
			if([[item title] length] > 0 && [[item title] characterAtIndex: 0] != ' ')
				[item setEnabled: NO];
	}
	if((mode == 2) || (mode == 3)) {
		MenuHandle out_menu = [[menu_bar_handle itemWithTitle: @"Outdoors"] submenu];
		for(id item in [out_menu itemArray])
			if([[item title] length] > 0 && [[item title] characterAtIndex: 0] != ' ')
				[item setEnabled: NO];
	}
	if(mode == 5) {
		[[menu_bar_handle itemWithTitle: @"File"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Edit"] setEnabled: NO];
	}
}

void update_edit_menu() {
	NSMenuItem* mi_undo = [edit_menu itemAtIndex: 0];
	if(undo_list.noUndo()) {
		[mi_undo setTitle: @"Can't Undo"];
		[mi_undo setEnabled: NO];
	} else {
		std::string undo_name = "Undo " + undo_list.undoName();
		[mi_undo setTitle: [NSString stringWithCString: undo_name.c_str() encoding: NSASCIIStringEncoding]];
		[mi_undo setEnabled: YES];
	}
	NSMenuItem* mi_redo = [edit_menu itemAtIndex: 1];
	if(undo_list.noRedo()) {
		[mi_redo setTitle: @"Can't Redo"];
		[mi_redo setEnabled: NO];
	} else {
		std::string redo_name = "Redo " + undo_list.redoName();
		[mi_redo setTitle: [NSString stringWithCString: redo_name.c_str() encoding: NSASCIIStringEncoding]];
		[mi_redo setEnabled: YES];
	}
}

@implementation MenuHandler
-(void) menuChoice:(id) sender {
	handle_menu_choice(eMenu([[sender representedObject] intValue]));
}
@end
