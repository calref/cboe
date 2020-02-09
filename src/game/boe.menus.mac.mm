//
//  boe.menus.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-03-25.
//
//

#include "boe.menus.hpp"

#include "universe.hpp" // Include before Cocoa because the Cocoa header defines things that cause compilation errors in here
#include <Cocoa/Cocoa.h>
#include <sstream>
#include "boe.party.hpp"
#include "boe.infodlg.hpp"
#include "boe.consts.hpp"
#include "spell.hpp"

extern short on_spell_menu[2][62];
extern short on_monst_menu[256];
extern bool party_in_memory;
extern cUniverse univ;
extern eGameMode overall_mode;

#ifndef __APPLE__
#error boe.menus.mm is Mac-specific code; try compiling boe.menus.win.cpp instead
#endif

using MenuHandle = NSMenu*;

MenuHandle menu_bar_handle;
MenuHandle apple_menu,file_menu,extra_menu,help_menu,monster_info_menu,library_menu;
MenuHandle actions_menu,music_menu,mage_spells_menu,priest_spells_menu;

@interface MenuHandler : NSObject
-(void) menuChoice:(id) sender;
-(void) monstMenu:(id) sender;
-(void) spellMenu:(id) sender;
@end

@interface MonsterWrapper : NSObject
@property cMonster* monst;
+(id) withMonster: (cMonster&) theMonster NS_RETURNS_RETAINED;
@end

@interface SpellWrapper : NSObject
@property int num;
@property eSkill type;
+(id) withSpell:(int) num ofType:(eSkill) type NS_RETURNS_RETAINED;
@end

void hideMenuBar() {
	[NSMenu setMenuBarVisible:NO];
}

void showMenuBar() {
	[NSMenu setMenuBarVisible:YES];
}

void adjust_monst_menu() {
	short monst_pos = 0;
	NSMenu* monst_menu;
	
	if(overall_mode == MODE_STARTUP) return;
	
	monst_menu = [[menu_bar_handle itemWithTitle: @"Monsters"] submenu];
	id targ = [[monst_menu itemAtIndex: 0] target];
	for(short i = 0; i < 256; i++) {
		on_monst_menu[i] = -1;
	}
	for(short i = 1; i < 256; i++) {
		if(i == 1 || univ.party.m_noted.count(i) > 0) {
			on_monst_menu[monst_pos] = i;
			monst_pos++;
		}
	}
	
	while([monst_menu numberOfItems] > 2) {
		[monst_menu removeItemAtIndex: 2];
	}
	for(short i = 0; i < 256; i++) {
		if(on_monst_menu[i] >= 0) {
			std::string monst_name = univ.scenario.scen_monsters[on_monst_menu[i]].m_name;
			NSString* str = [NSString stringWithUTF8String: monst_name.c_str()];
			NSMenuItem* newItem = [monst_menu addItemWithTitle: str action: @selector(monstMenu:) keyEquivalent: @""];
			[newItem setTarget: targ];
			[newItem setRepresentedObject: [MonsterWrapper withMonster: univ.scenario.scen_monsters[on_monst_menu[i]]]];
		}
	}
}

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
	[NSBundle loadNibNamed: @"game" owner: app];
	menu_bar_handle = [app mainMenu];
	
	apple_menu = [[menu_bar_handle itemWithTitle: @"Blades of Exile"] submenu];
	file_menu = [[menu_bar_handle itemWithTitle: @"File"] submenu];
	extra_menu = [[menu_bar_handle itemWithTitle: @"Options"] submenu];
	help_menu = [[menu_bar_handle itemWithTitle: @"Help"] submenu];
	library_menu = [[menu_bar_handle itemWithTitle: @"Library"] submenu];
	actions_menu = [[menu_bar_handle itemWithTitle: @"Actions"] submenu];
	mage_spells_menu = [[menu_bar_handle itemWithTitle: @"Cast Mage"] submenu];
	priest_spells_menu = [[menu_bar_handle itemWithTitle: @"Cast Priest"] submenu];
	monster_info_menu = [[menu_bar_handle itemWithTitle: @"Monsters"] submenu];
	
	static const eMenu file_choices[] = {
		eMenu::FILE_NEW, eMenu::FILE_OPEN, eMenu::NONE, eMenu::FILE_ABORT, eMenu::FILE_SAVE, eMenu::FILE_SAVE_AS,
	};
	static const eMenu opt_choices[] = {
		eMenu::OPTIONS_PC_GRAPHIC, eMenu::OPTIONS_RENAME_PC, eMenu::OPTIONS_NEW_PC, eMenu::OPTIONS_DELETE_PC, eMenu::NONE,
		eMenu::OPTIONS_TALK_NOTES, eMenu::OPTIONS_ENCOUNTER_NOTES, eMenu::OPTIONS_STATS, eMenu::OPTIONS_JOURNAL,
	};
	static const eMenu act_choices[] = {
		eMenu::ACTIONS_ALCHEMY, eMenu::ACTIONS_WAIT, eMenu::ACTIONS_AUTOMAP,
	};
	static const eMenu lib_choices[] = {
		eMenu::LIBRARY_MAGE, eMenu::LIBRARY_PRIEST, eMenu::LIBRARY_SKILLS, eMenu::LIBRARY_ALCHEMY, eMenu::LIBRARY_TIPS,
		eMenu::NONE, eMenu::LIBRARY_INTRO,
	};
	static const eMenu help_choices[] = {
		eMenu::HELP_TOC, eMenu::NONE, eMenu::HELP_OUT, eMenu::HELP_TOWN, eMenu::HELP_COMBAT, eMenu::HELP_BARRIER,
		eMenu::NONE, eMenu::HELP_HINTS, eMenu::HELP_SPELLS,
	};
	
	MenuHandler* handler = [[[MenuHandler alloc] init] retain];
	setMenuCallback([apple_menu itemWithTitle: @"About Blades of Exile"], handler, @selector(menuChoice:), int(eMenu::ABOUT));
	setMenuCallback([apple_menu itemWithTitle: @"Preferences…"], handler, @selector(menuChoice:), int(eMenu::PREFS));
	setMenuCallback([apple_menu itemWithTitle: @"Quit Blades of Exile"], handler, @selector(menuChoice:), int(eMenu::QUIT));
	
	int i = 0;
	for(eMenu opt : file_choices)
		setMenuCallback([file_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : opt_choices)
		setMenuCallback([extra_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : act_choices)
		setMenuCallback([actions_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : lib_choices)
		setMenuCallback([library_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	for(eMenu opt : help_choices)
		setMenuCallback([help_menu itemAtIndex: i++], handler, @selector(menuChoice:), int(opt));
	i = 0;
	
	setMenuCallback([mage_spells_menu itemAtIndex: 0], handler, @selector(menuChoice:), int(eMenu::ABOUT_MAGE));
	setMenuCallback([priest_spells_menu itemAtIndex: 0], handler, @selector(menuChoice:), int(eMenu::ABOUT_PRIEST));
	setMenuCallback([monster_info_menu itemAtIndex: 0], handler, @selector(menuChoice:), int(eMenu::ABOUT_MONSTERS));
	
	menu_activate();
}

void init_spell_menus() {
	for(short i = 0; i < 2; i++)
		for(short j = 0; j < 62; j++)
			on_spell_menu[i][j] = -1;
	
}



void adjust_spell_menus() {
	short spell_pos = 0;
	NSMenu* spell_menu;
	short old_on_spell_menu[2][62];
	bool need_menu_change = false;
	
	if(overall_mode == MODE_STARTUP || univ.cur_pc == 6)
		return;
	
	for(short i = 0; i < 2; i++)
		for(short j = 0; j < 62; j++)
			old_on_spell_menu[i][j] = on_spell_menu[i][j];
	
	spell_menu = [[menu_bar_handle itemWithTitle: @"Cast Mage"] submenu];
	id targ = [[spell_menu itemAtIndex: 0] target];
	
	for(short i = 0; i < 62; i++) {
		on_spell_menu[0][i] = -1;
	}
	for(short i = 0; i < 62; i++)
		if(pc_can_cast_spell(univ.current_pc(),cSpell::fromNum(eSkill::MAGE_SPELLS,i))) {
			on_spell_menu[0][spell_pos] = i;
			spell_pos++;
		}
	for(short i = 0; i < 62; i++)
		if(on_spell_menu[0][i] != old_on_spell_menu[0][i])
			need_menu_change = true;
	if(need_menu_change) {
		while([spell_menu numberOfItems] > 2) {
			[spell_menu removeItemAtIndex: 2];
		}
		for(short i = 0; i < 62; i++)
			if(on_spell_menu[0][i] >= 0) {
				eSpell spell = cSpell::fromNum(eSkill::MAGE_SPELLS, on_spell_menu[0][i]);
				std::ostringstream sout;
				sout << " L" << (*spell).level << " - " << (*spell).name() << ", C ";
				if((*spell).cost >= 0) sout << (*spell).cost; else sout << '?';
				NSString* str = [NSString stringWithUTF8String: sout.str().c_str()];
				NSMenuItem* newItem = [spell_menu addItemWithTitle: str action: @selector(spellMenu:) keyEquivalent: @""];
				[newItem setTarget: targ];
				[newItem setRepresentedObject: [SpellWrapper withSpell: on_spell_menu[0][i] ofType: eSkill::MAGE_SPELLS]];
			}
	}
	
	need_menu_change = false;
	spell_pos = 0;
	
	spell_menu = [[menu_bar_handle itemWithTitle: @"Cast Priest"] submenu];
	
	for(short i = 0; i < 62; i++) {
		on_spell_menu[1][i] = -1;
	}
	for(short i = 0; i < 62; i++)
		if(pc_can_cast_spell(univ.current_pc(),cSpell::fromNum(eSkill::PRIEST_SPELLS,i))) {
			on_spell_menu[1][spell_pos] = i;
			spell_pos++;
		}
	for(short i = 0; i < 62; i++)
		if(on_spell_menu[1][i] != old_on_spell_menu[1][i])
			need_menu_change = true;
	if(need_menu_change) {
		while([spell_menu numberOfItems] > 2) {
			[spell_menu removeItemAtIndex: 2];
		}
		for(short i = 0; i < 62; i++)
			if(on_spell_menu[1][i] >= 0) {
				eSpell spell = cSpell::fromNum(eSkill::PRIEST_SPELLS, on_spell_menu[1][i]);
				std::ostringstream sout;
				sout << " L" << (*spell).level << " - " << (*spell).name() << ", C ";
				if((*spell).cost >= 0) sout << (*spell).cost; else sout << '?';
				NSString* str = [NSString stringWithUTF8String: sout.str().c_str()];
				NSMenuItem* newItem = [spell_menu addItemWithTitle: str action: @selector(spellMenu:) keyEquivalent: @""];
				[newItem setTarget: targ];
				[newItem setRepresentedObject: [SpellWrapper withSpell: on_spell_menu[1][i] ofType: eSkill::PRIEST_SPELLS]];
			}
	}
	
	
}


void menu_activate() {
	if(overall_mode == MODE_STARTUP) {
		[[menu_bar_handle itemWithTitle: @"Options"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Monsters"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Actions"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Cast Mage"] setEnabled: NO];
		[[menu_bar_handle itemWithTitle: @"Cast Priest"] setEnabled: NO];
		
		[[file_menu itemWithTitle: @"Save"] setEnabled: NO];
		if(!party_in_memory) {
			[[file_menu itemWithTitle: @"Save As…"] setEnabled: NO];
		} else {
			[[file_menu itemWithTitle: @"Save As…"] setEnabled: YES];
		}
		return;
	}
	
	[[menu_bar_handle itemWithTitle: @"Options"] setEnabled: YES];
	[[menu_bar_handle itemWithTitle: @"Monsters"] setEnabled: YES];
	[[menu_bar_handle itemWithTitle: @"Actions"] setEnabled: YES];
	[[menu_bar_handle itemWithTitle: @"Cast Mage"] setEnabled: YES];
	[[menu_bar_handle itemWithTitle: @"Cast Priest"] setEnabled: YES];
	
	[[file_menu itemWithTitle: @"Save"] setEnabled: YES];
	[[file_menu itemWithTitle: @"Save As…"] setEnabled: YES];
}

@implementation MenuHandler
-(void) monstMenu:(id) sender {
	cMonster* monst = [[sender representedObject] monst];
	(void) monst; // Mark variable as unused
	handle_monster_info_menu([monster_info_menu indexOfItem: sender] - 1);
}

-(void) spellMenu:(id) sender {
	SpellWrapper* spell = [sender representedObject];
	handle_menu_spell(cSpell::fromNum([spell type], [spell num]));
}

-(void) menuChoice:(id) sender {
	handle_menu_choice(eMenu([[sender representedObject] intValue]));
}
@end

@implementation MonsterWrapper
@synthesize monst;

+(id) withMonster:(cMonster&) theMonster {
	MonsterWrapper* wrapper = [[MonsterWrapper alloc] init];
	[wrapper setMonst: &theMonster];
	return wrapper;
}
@end

@implementation SpellWrapper
@synthesize type, num;

+(id) withSpell:(int) num ofType:(eSkill) type {
	SpellWrapper* wrapper = [[SpellWrapper alloc] init];
	[wrapper setType: type];
	[wrapper setNum: num];
	return wrapper;
}
@end

