
#include "pc.menus.hpp"
#include <map>
#include <SFML/Graphics/RenderWindow.hpp>
#include "universe.hpp"
#include "winutil.hpp"

// This is the index of each menu on the menubar
enum {
	FILE_MENU_POS = 0,
	PARTY_MENU_POS = 1,
	SCEN_MENU_POS = 2,
	ITEMS_MENU_POS = 3,
	HELP_MENU_POS = 7,
};

extern sf::RenderWindow mainPtr;
extern cUniverse univ;
extern bool scen_items_loaded, party_in_scen;
extern fs::path file_in_mem;
std::map<int,eMenu> menuChoices;


void init_menubar() {
}

void update_item_menu() {
}

void menu_activate() {
}

#include "cursors.hpp"


#include "fileio.hpp"
#include "pc.fileio.hpp"

extern bool party_in_scen, scen_items_loaded;
extern fs::path file_in_mem;
void set_up_apple_events(int argc, char* argv[]) {
}
