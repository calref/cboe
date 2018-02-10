
#include "boe.menus.hpp"
#include <map>
#include <sstream>
#include <SFML/Graphics/RenderWindow.hpp>
#include "universe.hpp"
#include "boe.party.hpp"
#include "boe.infodlg.hpp"
#include "boe.consts.hpp"
#include "spell.hpp"
#include "winutil.hpp"

// This is the index of each menu on the menubar
enum {
	FILE_MENU_POS = 0,
	OPT_MENU_POS = 1,
	ACT_MENU_POS = 2,
	MONST_MENU_POS = 3,
	MAGE_MENU_POS = 4,
	PRIEST_MENU_POS = 5,
	LIB_MENU_POS = 6,
	HELP_MENU_POS = 7,
};

extern short on_spell_menu[2][62];
extern short on_monst_menu[256];
extern bool party_in_memory;
extern cUniverse univ;
extern eGameMode overall_mode;
extern sf::RenderWindow mainPtr;
std::map<int,eMenu> menuChoices;

void init_menubar() {
}

void adjust_monst_menu() {
}

void init_spell_menus() {
}

void adjust_spell_menus() {
}

void menu_activate() {
}

void hideMenuBar() {
}

void showMenuBar() {
}

#include "cursors.hpp"

#include "fileio.hpp"
#include "boe.graphics.hpp"
#include "boe.actions.hpp"
#include "boe.fileio.hpp"

extern bool ae_loading, finished_init;
void set_up_apple_events(int argc, char* argv[]) {

}
