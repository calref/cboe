//
//  pc.menus.h
//  BoE
//
//  Created by Celtic Minstrel on 14-04-15.
//
//

#ifndef BoE_pc_menus_h
#define BoE_pc_menus_h

void init_menubar();
void menu_activate();

namespace sf { class Event; };
bool menuBarProcessEvent(const sf::Event&);
void drawMenuBar();

enum class eMenu {
	NONE, ABOUT, PREFS, QUIT,
	FILE_OPEN, FILE_CLOSE, FILE_SAVE, FILE_SAVE_AS, FILE_REVERT, HELP_TOC,
	// Edit Party menu
	EDIT_GOLD, EDIT_FOOD, EDIT_ALCHEMY,
	HEAL_DAMAGE, RESTORE_MANA, RAISE_DEAD, CURE_CONDITIONS,
	EDIT_MAGE, EDIT_PRIEST, EDIT_TRAITS, EDIT_SKILLS, EDIT_XP,
	REUNITE_PARTY, OWN_VEHICLES,
	EDIT_ITEM,
	// Scenario Edit menu
	EDIT_DAY, LEAVE_TOWN, RESET_TOWNS, ADD_OUT_MAPS, ADD_TOWN_MAPS, LEAVE_SCENARIO, SET_SDF,
};

void handle_menu_choice(eMenu item_hit);

#endif
