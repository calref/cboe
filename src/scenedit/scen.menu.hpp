// Author: xq, Tuesday 2020-01-28

#ifndef SCEN_MENU_HPP
#define SCEN_MENU_HPP

// NOTE: this also includes SFML for us
#include <TGUI/TGUI.hpp>
#include <vector>
#include "tools/undo.hpp"
#include "tools/event_listener.hpp"
#include "tools/drawable.hpp"

class OpenBoESceneditMenu : public iEventListener, public iDrawable  {
public:

	OpenBoESceneditMenu(sf::RenderWindow &);
	
	virtual bool handle_event(const sf::Event&) override;
	virtual void draw() override;
	void update_for_mode(short mode);
	void update_edit_menu(cUndoList const &);
	
private:
	
	using MenuHierarchy = std::vector<tgui::String>;
	
	tgui::Gui tgui;
	const tgui::String internal_menubar_widget_name { "openboe-scenedit-menu" };

	tgui::MenuBar::Ptr build_menubar() const;
	void add_menu_placeholders(tgui::MenuBar::Ptr&) const;
	void add_persistent_menu_items(tgui::MenuBar::Ptr&) const;
	tgui::MenuBar::Ptr get_menubar_ptr() const;
	bool handle_keypressed_event(const sf::Event&) const;
	bool handle_ctrl_keypress(const sf::Event&) const;
	bool handle_ctrl_shift_keypress(const sf::Event&) const;
	bool is_control_key_pressed() const;
	bool is_shift_key_pressed() const;
	void update_for_mode_0();
	void update_for_mode_1();
	void update_for_mode_2();
	void update_for_mode_3();
	void update_for_mode_4();
};

#endif
