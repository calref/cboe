
#include "scen.menus.hpp"
#include <map>
#include <SFML/Graphics/RenderWindow.hpp>
#include "scenario.hpp"
#include "winutil.hpp"

// This is the index of each menu on the menubar
enum {
	FILE_MENU_POS = 0,
	EDIT_MENU_POS = 1,
	SCEN_MENU_POS = 2,
	TOWN_MENU_POS = 3,
	OUT_MENU_POS = 4,
	HELP_MENU_POS = 6,
};

extern sf::RenderWindow mainPtr;
extern cScenario scenario;
std::map<int,eMenu> menuChoices;


void init_menubar() {
}

void shut_down_menus(short mode) {
}

#include "cursors.hpp"

#include "fileio.hpp"
#include "scen.actions.hpp"

extern short cur_town;
extern location cur_out;
extern cTown* town;
extern cOutdoors* current_terrain;
extern bool change_made, ae_loading;
void set_up_apple_events(int argc, char* argv[]) {
}
