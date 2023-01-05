#include "scen.global.hpp"
#include "scen.menus.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include "scen.menu.hpp"
#include "tools/undo.hpp"
#include "tools/event_listener.hpp"
#include "tools/drawable_manager.hpp"

extern sf::RenderWindow mainPtr;
extern cUndoList undo_list;
extern std::unordered_map <std::string, std::shared_ptr <iEventListener>> event_listeners;
extern cDrawableManager drawable_mgr;

std::shared_ptr <OpenBoESceneditMenu> menu_ptr;

void init_menubar() {
	menu_ptr.reset(new OpenBoESceneditMenu(mainPtr));
	
	event_listeners["menubar"] = std::dynamic_pointer_cast <iEventListener> (menu_ptr); 
	drawable_mgr.add_drawable(UI_LAYER_MENUBAR, "menubar", menu_ptr); 
}

void shut_down_menus(short mode) {
	menu_ptr->update_for_mode(mode);
}

void update_edit_menu() {
	menu_ptr->update_edit_menu(undo_list);
}

void set_up_apple_events(int argc, char* argv[]) {
}
