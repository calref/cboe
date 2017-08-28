//
//  boe.menus.h
//  BoE
//
//  Created by Celtic Minstrel on 14-03-28.
//
//

#ifndef BoE_boe_menus_h
#define BoE_boe_menus_h

// Needed for MinGW builds for some reason
#ifdef FILE_OPEN
#undef FILE_OPEN
#endif

void init_menubar();
void adjust_monst_menu();
void init_spell_menus();
void adjust_spell_menus();
void menu_activate();
void hideMenuBar();
void showMenuBar();

enum class eMenu {
	NONE, ABOUT, PREFS, QUIT,
	FILE_NEW, FILE_OPEN, FILE_ABORT, FILE_SAVE, FILE_SAVE_AS,
	OPTIONS_PC_GRAPHIC, OPTIONS_RENAME_PC, OPTIONS_NEW_PC, OPTIONS_DELETE_PC,
	OPTIONS_TALK_NOTES, OPTIONS_ENCOUNTER_NOTES, OPTIONS_STATS, OPTIONS_JOURNAL,
	ACTIONS_ALCHEMY, ACTIONS_WAIT, ACTIONS_AUTOMAP,
	LIBRARY_MAGE, LIBRARY_PRIEST, LIBRARY_SKILLS, LIBRARY_ALCHEMY, LIBRARY_TIPS, LIBRARY_INTRO,
	HELP_TOC, HELP_OUT, HELP_TOWN, HELP_COMBAT, HELP_BARRIER, HELP_HINTS, HELP_SPELLS,
	// The "About This Menu" options
	ABOUT_MONSTERS, ABOUT_MAGE, ABOUT_PRIEST,
};

enum class eSpell;

void handle_menu_choice(eMenu item_hit);
void handle_menu_spell(eSpell spell_picked);
void handle_monster_info_menu(int item_hit);

#endif
