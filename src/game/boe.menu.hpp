#ifndef BOE_MENU_HPP
#define BOE_MENU_HPP

// Author: xq, Saturday 2020-01-25

// NOTE: this also includes SFML for us
#include <TGUI/TGUI.hpp>
#include <vector>
#include "universe.hpp"
#include "boe.consts.hpp"
#include "spell.hpp"
#include "skills_traits.hpp"

class OpenBoEMenu {
public:

	OpenBoEMenu(sf::RenderWindow&, cUniverse&);

	bool handle_event(const sf::Event&);
	void draw();
	void update_for_game_state(eGameMode overall_mode, bool party_in_memory);
	void update_spell_menus();
	void update_monsters_menu();

private:

	using MenuHierarchy = std::vector<sf::String>;

	tgui::Gui tgui;
	sf::RenderWindow& mainPtr;
	cUniverse& univ;
	const sf::String internal_menubar_widget_name { "openboe-menu" };
	std::vector<unsigned int> spell_menus_connection_ids;

	tgui::MenuBar::Ptr build_menubar() const;
	bool handle_keypressed_event(const sf::Event&) const;
	bool handle_ctrl_keypress(const sf::Event&) const;
	bool handle_ctrl_shift_keypress(const sf::Event&) const;
	bool is_control_key_pressed() const;
	bool is_shift_key_pressed() const;
	void add_menu_placeholders(tgui::MenuBar::Ptr&) const;
	void add_persistent_menu_items(tgui::MenuBar::Ptr&) const;
	tgui::MenuBar::Ptr get_menubar_ptr() const;
	void purge_spell_menus(tgui::MenuBar::Ptr&);
	void update_mage_spells_menu(tgui::MenuBar::Ptr&);
	void update_priest_spells_menu(tgui::MenuBar::Ptr&);
	MenuHierarchy menu_hierarchy_from_spell(const cSpell&) const;
};

#endif
