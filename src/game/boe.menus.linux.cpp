#include "boe.menus.hpp"
#include "boe.consts.hpp"

#include <memory>
#include <SFML/Graphics.hpp>

#include "boe.menu.hpp"

extern sf::RenderWindow mainPtr;
extern cUniverse univ;
extern bool party_in_memory;
extern eGameMode overall_mode;

std::shared_ptr<OpenBoEMenu> menu_ptr;

void init_menubar() {
	menu_ptr.reset(new OpenBoEMenu(mainPtr, univ));
}

bool menuBarProcessEvent(const sf::Event& event) {
	return menu_ptr->handle_event(event);
}

void drawMenuBar() {
	menu_ptr->draw();
}

void adjust_monst_menu() {
	menu_ptr->update_monsters_menu();
}

void init_spell_menus() {
}

void adjust_spell_menus() {
	menu_ptr->update_spell_menus();
}

void menu_activate() {
	menu_ptr->update_for_game_state(overall_mode, party_in_memory);
}

void hideMenuBar() {
}

void showMenuBar() {
}

void set_up_apple_events(int argc, char* argv[]) {

}
