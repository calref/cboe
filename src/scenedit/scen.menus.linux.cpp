#include "scen.menus.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include "scen.menu.hpp"
#include "undo.hpp"

extern sf::RenderWindow mainPtr;
extern cUndoList undo_list;

std::shared_ptr <OpenBoESceneditMenu> menu_ptr;

void init_menubar() {
	menu_ptr.reset(new OpenBoESceneditMenu(mainPtr));
}

void shut_down_menus(short mode) {
	menu_ptr->update_for_mode(mode);
}

void update_edit_menu() {
	menu_ptr->update_edit_menu(undo_list);
}

bool menuBarProcessEvent(const sf::Event& event) {
	return menu_ptr->handle_event(event);
}

void drawMenuBar() {
	menu_ptr->draw();
}

void set_up_apple_events(int argc, char* argv[]) {
}
