// Author: xq, Sunday 2020-02-02

#ifndef PC_MENU_HPP
#define PC_MENU_HPP

// NOTE: this also includes SFML for us
#include <TGUI/TGUI.hpp>
#include <vector>

class OpenBoEPCEditMenu {
public:

	OpenBoEPCEditMenu(sf::RenderWindow&);
	bool handle_event(const sf::Event&);
	void draw();
	void update_for_editor_state(bool party_in_memory, bool party_in_scenario);
	
private:
	
	using MenuHierarchy = std::vector<tgui::String>;
	
	tgui::Gui tgui;
	sf::RenderWindow& mainPtr;
	const tgui::String internal_menubar_widget_name { "openboe-pcedit-menu" };
	
	tgui::MenuBar::Ptr build_menubar() const;
	void add_menu_placeholders(tgui::MenuBar::Ptr&) const;
	void add_persistent_menu_items(tgui::MenuBar::Ptr&) const;
	tgui::MenuBar::Ptr get_menubar_ptr() const;
	bool handle_keypressed_event(const sf::Event&) const;
	bool handle_ctrl_keypress(const sf::Event&) const;
	bool handle_ctrl_shift_keypress(const sf::Event&) const;
	bool is_control_key_pressed() const;
	bool is_shift_key_pressed() const;
};

#endif
