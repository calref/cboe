// Author: xq, Sunday 2020-02-02

#include "pc.menu.hpp"
#include "pc.menus.hpp"

OpenBoEPCEditMenu::OpenBoEPCEditMenu(sf::RenderWindow& window)
	: mainPtr { window }
	, tgui    { window } {

	// Build a menubar and store it in tgui with a known name
	this->tgui.add(this->build_menubar(), this->internal_menubar_widget_name);
}

tgui::MenuBar::Ptr OpenBoEPCEditMenu::build_menubar() const {
	auto menubar = tgui::MenuBar::create();

	// XXX TODO FIXME can we get this constant magic number from somewhere?
	menubar->setSize(this->mainPtr.getSize().x, 20);

	this->add_menu_placeholders(menubar);
	this->add_persistent_menu_items(menubar);

	return menubar;
}

// This method ensures that the menus on the menubar are in specific order
void OpenBoEPCEditMenu::add_menu_placeholders(tgui::MenuBar::Ptr& menubar) const {
	menubar->addMenu("File");
	menubar->addMenu("Party");
	menubar->addMenu("Scenario");
	menubar->addMenu("Help");
}

// This method fills the menu with items that never change.
void OpenBoEPCEditMenu::add_persistent_menu_items(tgui::MenuBar::Ptr& menubar) const {

	const std::vector<std::pair <OpenBoEPCEditMenu::MenuHierarchy, eMenu>> persistent_menu_items {
		{ { "File", "Open Game Ctrl-O"               }, eMenu::FILE_OPEN       },
		{ { "File", "Close Ctrl-W"                   }, eMenu::FILE_CLOSE      },
		{ { "File", "Save Game Ctrl-S"               }, eMenu::FILE_SAVE       },
		{ { "File", "Save As... Ctrl-Shift-S"        }, eMenu::FILE_SAVE_AS    },
		{ { "File", "Revert to Saved"                }, eMenu::FILE_REVERT     },
                { { "File", "Preferences"                    }, eMenu::PREFS           },
		{ { "File", "Quit Ctrl-Q"                    }, eMenu::QUIT            },

		{ { "Party", "Edit Gold"                     }, eMenu::EDIT_GOLD       },
		{ { "Party", "Edit Food"                     }, eMenu::EDIT_FOOD       },
		{ { "Party", "Edit Alchemy"                  }, eMenu::EDIT_ALCHEMY    },
		{ { "Party", "Heal Damage"                   }, eMenu::HEAL_DAMAGE     },
		{ { "Party", "Restore Spell Points"          }, eMenu::RESTORE_MANA    },
		{ { "Party", "Raise Dead, Destone, etc."     }, eMenu::RAISE_DEAD      },
		{ { "Party", "Removed Bad Conditions"        }, eMenu::CURE_CONDITIONS },
		{ { "Party", "Edit Mage Spells"              }, eMenu::EDIT_MAGE       },
		{ { "Party", "Edit Priest Spells"            }, eMenu::EDIT_PRIEST     },
		{ { "Party", "Add Item"                      }, eMenu::EDIT_ITEM       },
		{ { "Party", "Edit Traits"                   }, eMenu::EDIT_TRAITS     },
		{ { "Party", "Edit Skills"                   }, eMenu::EDIT_SKILLS     },
		{ { "Party", "Edit XP"                       }, eMenu::EDIT_XP         },
		{ { "Party", "Reunite Party"                 }, eMenu::REUNITE_PARTY   },

		{ { "Scenario", "Edit Day"                   }, eMenu::EDIT_DAY        },
		{ { "Scenario", "Leave Town"                 }, eMenu::LEAVE_TOWN      },
		{ { "Scenario", "Make Towns Forget You"      }, eMenu::RESET_TOWNS     },
		{ { "Scenario", "Add All Town Maps"          }, eMenu::ADD_TOWN_MAPS   },
		{ { "Scenario", "Add All Outdoor Maps"       }, eMenu::ADD_OUT_MAPS    },
		{ { "Scenario", "Own All Boats/Horses"       }, eMenu::OWN_VEHICLES    },
		{ { "Scenario", "Remove Party From Scenario" }, eMenu::LEAVE_SCENARIO  },
		{ { "Scenario", "Set Stuff Done Flag"        }, eMenu::SET_SDF         },
		
		{ { "Help", "About Blades of Exile Editor"   }, eMenu::ABOUT           },
		{ { "Help", "Blades of Exile Editor Help F1" }, eMenu::HELP_TOC        },
	};

	// Note that signal connection ids are discarded.
	for(const auto& item : persistent_menu_items) {
		menubar->addMenuItem(item.first);
		menubar->connectMenuItem(item.first, handle_menu_choice, item.second);
	}
}


bool OpenBoEPCEditMenu::handle_event(const sf::Event& event) {
	
	if(event.type == sf::Event::KeyPressed && this->handle_keypressed_event(event))
		return true;

	return this->tgui.handleEvent(event);
}

// Returns true if event was consumed
bool OpenBoEPCEditMenu::handle_keypressed_event(const sf::Event& event) const {

	// NOTE: since we are manually adding keyboard shortcut descriptions
	// to the menu items, they become parts of menu hierarchies

	bool event_was_consumed { false };
	
	if(this->is_control_key_pressed()) {
		if(this->is_shift_key_pressed()) {
			event_was_consumed = this->handle_ctrl_shift_keypress(event);
		} else {
			event_was_consumed = this->handle_ctrl_keypress(event);
		}
	}

	switch(event.key.code) {
		case sf::Keyboard::F1:
			handle_menu_choice(eMenu::HELP_TOC);
			event_was_consumed = true;
			break;
		default: break;
	}

	return event_was_consumed;
}

bool OpenBoEPCEditMenu::handle_ctrl_keypress(const sf::Event& event) const {
	auto menubar = this->get_menubar_ptr();
	bool event_was_consumed { false };

	switch(event.key.code) {
		case sf::Keyboard::S:
			if(!menubar->getMenuItemEnabled({ "File", "Save Game Ctrl-S" })) break;
			handle_menu_choice(eMenu::FILE_SAVE);
			event_was_consumed = true;
			break;
		case sf::Keyboard::W:
			if(!menubar->getMenuItemEnabled({ "File", "Close Ctrl-W" })) break;
			handle_menu_choice(eMenu::FILE_CLOSE);
			event_was_consumed = true;
			break;
		case sf::Keyboard::O:
			handle_menu_choice(eMenu::FILE_OPEN);
			event_was_consumed = true;
			break;
		case sf::Keyboard::Q:
			handle_menu_choice(eMenu::QUIT);
			event_was_consumed = true;
			break;
		default: break;
	}
	
	return event_was_consumed;
}

bool OpenBoEPCEditMenu::handle_ctrl_shift_keypress(const sf::Event& event) const {
	auto menubar = this->get_menubar_ptr();
	bool event_was_consumed { false };

	switch(event.key.code) {
		case sf::Keyboard::S:
			if(!menubar->getMenuItemEnabled({ "File", "Save As... Ctrl-Shift-S" })) break;
			handle_menu_choice(eMenu::FILE_SAVE_AS);
			event_was_consumed = true;
			break;
		default: break;
	}

	return event_was_consumed;
}

bool OpenBoEPCEditMenu::is_control_key_pressed() const {

	// NOTE: Control is not cross-platform (apple)

	return (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
		 || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl));
}

bool OpenBoEPCEditMenu::is_shift_key_pressed() const {
	return (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
		 || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift));
}

void OpenBoEPCEditMenu::draw() {
	this->tgui.draw();
}

tgui::MenuBar::Ptr OpenBoEPCEditMenu::get_menubar_ptr() const {
	return this->tgui.get<tgui::MenuBar>(this->internal_menubar_widget_name);
}

void OpenBoEPCEditMenu::update_for_editor_state(bool party_in_memory, bool party_in_scenario) {
	auto menubar = this->get_menubar_ptr();
	
	if(party_in_memory) {
		menubar->setMenuEnabled("Party"   , true);
		menubar->setMenuEnabled("Scenario", party_in_scenario);

		menubar->setMenuItemEnabled({ "File", "Close Ctrl-W"     }, true);
		menubar->setMenuItemEnabled({ "File", "Save Game Ctrl-S" }, true);
		menubar->setMenuItemEnabled({ "File", "Save As... Ctrl-Shift-S" }, true);
		menubar->setMenuItemEnabled({ "File", "Revert to Saved"  }, true);
	} else {
		menubar->setMenuEnabled("Party"   , false);
		menubar->setMenuEnabled("Scenario", false);

		menubar->setMenuItemEnabled({ "File", "Close Ctrl-W"     }, false);
		menubar->setMenuItemEnabled({ "File", "Save Game Ctrl-S" }, false);
		menubar->setMenuItemEnabled({ "File", "Save As... Ctrl-Shift-S" }, false);
		menubar->setMenuItemEnabled({ "File", "Revert to Saved"  }, false);
	}
}

