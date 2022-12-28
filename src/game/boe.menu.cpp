// Author: xq, Saturday 2020-01-25

#include "boe.menu.hpp"
#include "boe.menus.hpp"
#include "boe.party.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

OpenBoEMenu::OpenBoEMenu(sf::RenderWindow& window, cUniverse& universe)
	: tgui { window }
	, univ { universe } {

	// Build a menubar and store it in tgui with a known name
	this->tgui.add(this->build_menubar(), this->internal_menubar_widget_name);

	// Space for 2x "About this menu" connections + 1 for every possible spell
	this->spell_menus_connection_ids.reserve(2 + NUM_MAGE_SPELLS + NUM_PRIEST_SPELLS);

	// These menus are dynamic so we need to generate them
	this->update_spell_menus();
}

tgui::MenuBar::Ptr OpenBoEMenu::build_menubar() const {
	auto menubar = tgui::MenuBar::create();

	menubar->setSize("100%", MENUBAR_HEIGHT);

	this->add_menu_placeholders(menubar);
	this->add_persistent_menu_items(menubar);

	return menubar;
}

// This method ensures that the menus on the menubar are in specific order
void OpenBoEMenu::add_menu_placeholders(tgui::MenuBar::Ptr& menubar) const {
	menubar->addMenu("File");
	menubar->addMenu("Options");
	menubar->addMenu("Actions");
	menubar->addMenu("Monsters");
	menubar->addMenu("Cast Mage");
	menubar->addMenu("Cast Priest");
	menubar->addMenu("Library");
	menubar->addMenu("Help");
}

// This method fills the menu with items that never change.
void OpenBoEMenu::add_persistent_menu_items(tgui::MenuBar::Ptr& menubar) const {

	const std::vector<std::pair <OpenBoEMenu::MenuHierarchy, eMenu>> persistent_menu_items {
		{ { "File", "New Game Ctrl-N"             }, eMenu::FILE_NEW                },
		{ { "File", "Open Game Ctrl-O"            }, eMenu::FILE_OPEN               },
		{ { "File", "Abort"                       }, eMenu::FILE_ABORT              },
		{ { "File", "Save Game Ctrl-S"            }, eMenu::FILE_SAVE               },
		{ { "File", "Save As... Ctrl-Shift-S"     }, eMenu::FILE_SAVE_AS            },
		{ { "File", "Preferences"                 }, eMenu::PREFS                   },
		{ { "File", "Quit Ctrl-Q"                 }, eMenu::QUIT                    },

		{ { "Options", "Pick New PC Graphic"      }, eMenu::OPTIONS_PC_GRAPHIC      },
		{ { "Options", "Pick New PC Name"         }, eMenu::OPTIONS_RENAME_PC       },
		{ { "Options", "Create New PC"            }, eMenu::OPTIONS_NEW_PC          },
		{ { "Options", "Delete PC"                }, eMenu::OPTIONS_DELETE_PC       },
		{ { "Options", "See Talking Notes"        }, eMenu::OPTIONS_TALK_NOTES      },
		{ { "Options", "See Encounter Notes"      }, eMenu::OPTIONS_ENCOUNTER_NOTES },
		{ { "Options", "See Overall Party Stats"  }, eMenu::OPTIONS_STATS           },
		{ { "Options", "See Journal"              }, eMenu::OPTIONS_JOURNAL         },

		{ { "Actions", "Do Alchemy Ctrl-A"        }, eMenu::ACTIONS_ALCHEMY         },
		{ { "Actions", "Wait 80 Moves Ctrl-W"     }, eMenu::ACTIONS_WAIT            },
		{ { "Actions", "Display AutoMap A"        }, eMenu::ACTIONS_AUTOMAP         },

		{ { "Library", "Mage Spells"              }, eMenu::LIBRARY_MAGE            },
		{ { "Library", "Priest Spells"            }, eMenu::LIBRARY_PRIEST          },
		{ { "Library", "Skill Info"               }, eMenu::LIBRARY_SKILLS          },
		{ { "Library", "Alchemy/Poison"           }, eMenu::LIBRARY_ALCHEMY         },
		{ { "Library", "Tip of the Day"           }, eMenu::LIBRARY_TIPS            },
		{ { "Library", "Show Introductory Dialog" }, eMenu::LIBRARY_INTRO           },

		{ { "Help", "Index F1"                    }, eMenu::HELP_TOC                },
		{ { "Help", "About Blades of Exile"       }, eMenu::ABOUT                   },
		{ { "Help", "Outdoor Commands"            }, eMenu::HELP_OUT                },
		{ { "Help", "Town Commands"               }, eMenu::HELP_TOWN               },
		{ { "Help", "Combat Commands"             }, eMenu::HELP_COMBAT             },
		{ { "Help", "Magic Barrier Help"          }, eMenu::HELP_BARRIER            },
		{ { "Help", "Hints, Bugs and Comments"    }, eMenu::HELP_HINTS              },
		{ { "Help", "Spell Casting Help"          }, eMenu::HELP_SPELLS             },
	};

	// Note that signal connection ids are discarded.
	for(const auto& item : persistent_menu_items) {
		menubar->addMenuItem(item.first);
		menubar->connectMenuItem(item.first, handle_menu_choice, item.second);
	}
}

bool OpenBoEMenu::handle_event(const sf::Event& event) {

	if(event.type == sf::Event::KeyPressed && this->handle_keypressed_event(event))
		return true;

	return this->tgui.handleEvent(event);
}

// Returns true if event was consumed
bool OpenBoEMenu::handle_keypressed_event(const sf::Event& event) const {

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

bool OpenBoEMenu::handle_ctrl_keypress(const sf::Event& event) const {
	auto menubar = this->get_menubar_ptr();
	bool event_was_consumed { false };

	switch(event.key.code) {
		case sf::Keyboard::S:
			if(!menubar->getMenuItemEnabled({ "File", "Save Game Ctrl-S" })) break;
			handle_menu_choice(eMenu::FILE_SAVE);
			event_was_consumed = true;
			break;
		case sf::Keyboard::A:
			if(!menubar->getMenuItemEnabled({ "Actions", "Do Alchemy Ctrl-A" })) break;
			handle_menu_choice(eMenu::ACTIONS_ALCHEMY);
			event_was_consumed = true;
			break;
		case sf::Keyboard::W:
			if(!menubar->getMenuItemEnabled({ "Actions", "Wait 80 Moves Ctrl-W" })) break;
			handle_menu_choice(eMenu::ACTIONS_WAIT);
			event_was_consumed = true;
			break;
		case sf::Keyboard::N:
			handle_menu_choice(eMenu::FILE_NEW);
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

bool OpenBoEMenu::handle_ctrl_shift_keypress(const sf::Event& event) const {
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

bool OpenBoEMenu::is_control_key_pressed() const {

	// NOTE: Control is not cross-platform (apple)

	return (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
		 || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl));
}

bool OpenBoEMenu::is_shift_key_pressed() const {
	return (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
		 || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift));
}

tgui::MenuBar::Ptr OpenBoEMenu::get_menubar_ptr() const {
	return this->tgui.get<tgui::MenuBar>(this->internal_menubar_widget_name);
}

void OpenBoEMenu::draw() {
	this->tgui.draw();
}

// This method enables/disables menus based on current game state
// (logic follows the example in windows menu implementation).
void OpenBoEMenu::update_for_game_state(eGameMode overall_mode, bool party_in_memory) {
	auto menubar = this->get_menubar_ptr();

	if(overall_mode == MODE_STARTUP) {
		menubar->setMenuEnabled("Options",     false);
		menubar->setMenuEnabled("Actions",     false);
		menubar->setMenuEnabled("Monsters",    false);
		menubar->setMenuEnabled("Cast Mage",   false);
		menubar->setMenuEnabled("Cast Priest", false);

		menubar->setMenuItemEnabled({ "File", "Save Game Ctrl-S" }, false);
		menubar->setMenuItemEnabled({ "File", "Save As... Ctrl-Shift-S" }, party_in_memory);
		menubar->setMenuItemEnabled({ "File", "Abort" }, false);
	} else {
		menubar->setMenuEnabled("Options",     true);
		menubar->setMenuEnabled("Actions",     true);
		menubar->setMenuEnabled("Monsters",    true);
		menubar->setMenuEnabled("Cast Mage",   true);
		menubar->setMenuEnabled("Cast Priest", true);

		menubar->setMenuItemEnabled({ "File", "Save Game Ctrl-S"  }, true);
		menubar->setMenuItemEnabled({ "File", "Save As... Ctrl-Shift-S" }, true);
		menubar->setMenuItemEnabled({ "File", "Abort" }, true);
	}
}

// Disconnect all spell menu items from signals and clear the menus
void OpenBoEMenu::purge_spell_menus(tgui::MenuBar::Ptr& menubar) {
	for(const auto& connection_id : this->spell_menus_connection_ids) {
		if(!menubar->onMenuItemClick.disconnect(connection_id))
			throw std::runtime_error { "BUG: attempted to disconnect menubar signal using invalid connection id" };
	}

	this->spell_menus_connection_ids.clear();

	menubar->removeMenuItems("Cast Mage");
	menubar->removeMenuItems("Cast Priest");
}

void OpenBoEMenu::update_mage_spells_menu(tgui::MenuBar::Ptr& menubar) {

	// Add "About" menu item and store connection id
	const OpenBoEMenu::MenuHierarchy about_hierarchy { "Cast Mage", "About this menu" };
	menubar->addMenuItem(about_hierarchy);
	this->spell_menus_connection_ids.push_back(
		menubar->connectMenuItem(about_hierarchy, handle_menu_choice, eMenu::ABOUT_MAGE)
	);

	// Add every castable mage spell and store connection ids
	for(int spell_id = 0; spell_id < NUM_MAGE_SPELLS; ++spell_id) {
		eSpell spell = cSpell::fromNum(eSkill::MAGE_SPELLS, spell_id);
		if(pc_can_cast_spell(this->univ.current_pc(), spell)) {
			const auto spell_hierarchy = this->menu_hierarchy_from_spell(*spell);
			menubar->addMenuItem(spell_hierarchy);
			// Connect and store connection id
			this->spell_menus_connection_ids.push_back(
				menubar->connectMenuItem(spell_hierarchy, handle_menu_spell, spell)
			);
		}
	}
}

void OpenBoEMenu::update_priest_spells_menu(tgui::MenuBar::Ptr& menubar) {

	// Add "About" menu item and store connection id
	const OpenBoEMenu::MenuHierarchy about_hierarchy { "Cast Priest", "About this menu" };
	menubar->addMenuItem(about_hierarchy);
	this->spell_menus_connection_ids.push_back(
		menubar->connectMenuItem(about_hierarchy, handle_menu_choice, eMenu::ABOUT_PRIEST)
	);

	// Add every castable Priest spell and store connection ids
	for (int spell_id = 0; spell_id < NUM_PRIEST_SPELLS; ++spell_id) {
		eSpell spell = cSpell::fromNum(eSkill::PRIEST_SPELLS, spell_id);
		if(pc_can_cast_spell(this->univ.current_pc(), spell)) {
			const auto spell_hierarchy = this->menu_hierarchy_from_spell(*spell);
			menubar->addMenuItem(spell_hierarchy);
			// Connect and store connection id
			this->spell_menus_connection_ids.push_back(
				menubar->connectMenuItem(spell_hierarchy, handle_menu_spell, spell)
			);
		}
	}
}

// Create a menu hierarchy from cSpell
OpenBoEMenu::MenuHierarchy OpenBoEMenu::menu_hierarchy_from_spell(const cSpell& spell) const {
	OpenBoEMenu::MenuHierarchy hier;

	// Menu name, level, name+cost
	hier.reserve(3);

	// Fill menu name
	if(spell.type == eSkill::MAGE_SPELLS) {
		hier.push_back("Cast Mage");
	} else if(spell.type == eSkill::PRIEST_SPELLS) {
		hier.push_back("Cast Priest");
	} else {
		throw std::runtime_error { "BUG: attempt to build menu hierarchy from unknown spell type" };
	}

	// Fill level
	std::ostringstream level_label;
	level_label << "Level " << spell.level;
	hier.push_back(level_label.str());

	// Fill name+cost
	std::ostringstream name_cost_label;
	name_cost_label << spell.name() << ", C ";

	if(spell.cost >= 0) {
		name_cost_label << spell.cost;
	} else {
		name_cost_label << '?';
	}

	hier.push_back(name_cost_label.str());

	return hier;
}

void OpenBoEMenu::update_spell_menus() {
	auto menubar = this->get_menubar_ptr();

	this->purge_spell_menus(menubar);
	this->update_mage_spells_menu(menubar);
	this->update_priest_spells_menu(menubar);
}

void OpenBoEMenu::update_monsters_menu() {
	// TODO not implemented.
	// How do you fit a dropdown with 256 monster names in a 605*430 window?

	//~ { { "Monsters", "About this menu"}, eMenu::ABOUT_MONSTERS },
}
