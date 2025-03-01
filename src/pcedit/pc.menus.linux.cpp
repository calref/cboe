
#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>
#include "tools/winutil.hpp"
#include "universe/universe.hpp"
#include "pc.menus.hpp"
#include "pc.menu.hpp"

//extern sf::RenderWindow mainPtr;
extern bool party_in_scen;
extern cUniverse univ;

std::shared_ptr<OpenBoEPCEditMenu> menu_ptr;

void init_menubar() {
	sf::RenderWindow& mainPtr = get_main_window();
	menu_ptr.reset(new OpenBoEPCEditMenu(mainPtr));
}

void menu_activate() {
	menu_ptr->update_for_editor_state(!univ.file.empty(), party_in_scen);
}

bool menuBarProcessEvent (sf::Event const & event) {
	return menu_ptr->handle_event(event);
}

void drawMenuBar() {
	menu_ptr->draw();
}

void set_up_apple_events() {
}
