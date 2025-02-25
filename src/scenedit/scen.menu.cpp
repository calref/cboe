// Author: xq, Tuesday 2020-01-28

#include "scen.menu.hpp"
#include "scen.menus.hpp"

#include <stdexcept>

OpenBoESceneditMenu::OpenBoESceneditMenu(sf::RenderWindow& window)
	: tgui { window } {
	// Build a menubar and store it in tgui with a known name
	this->tgui.add(this->build_menubar(), this->internal_menubar_widget_name);
}

tgui::MenuBar::Ptr OpenBoESceneditMenu::build_menubar() const {
	auto menubar = tgui::MenuBar::create();

	// XXX can we get this constant magic number from somewhere?
	menubar->setSize("100%", 20);

	this->add_menu_placeholders(menubar);
	this->add_persistent_menu_items(menubar);

	return menubar;
}

// This method ensures that the menus on the menubar are in specific order
void OpenBoESceneditMenu::add_menu_placeholders(tgui::MenuBar::Ptr& menubar) const {
	menubar->addMenu("File");
	menubar->addMenu("Edit");
	
	// With Advanced as a topmost item to have more vertical space for sub-items there
	menubar->addMenuItem({"Scenario", "Advanced"});
	menubar->addMenuItem({"Town", "Advanced"});
	menubar->addMenuItem({"Outdoors", "Advanced"});
	
	menubar->addMenu("Help");
}

// This method fills the menu with items that never change.
void OpenBoESceneditMenu::add_persistent_menu_items(tgui::MenuBar::Ptr& menubar) const {
	const std::vector<std::pair <OpenBoESceneditMenu::MenuHierarchy, eMenu>> persistent_menu_items {
		{ { "File", "New Scenario Ctrl-N"     }, eMenu::FILE_NEW     },
		{ { "File", "Open Scenario Ctrl-O"    }, eMenu::FILE_OPEN    },
		{ { "File", "Close Scenario Ctrl-W"   }, eMenu::FILE_CLOSE   },
		{ { "File", "Save Scenario Ctrl-S"    }, eMenu::FILE_SAVE    },
		{ { "File", "Save As... Ctrl-Shift-S" }, eMenu::FILE_SAVE_AS },
		{ { "File", "Revert to Saved"         }, eMenu::FILE_REVERT  },
                { { "File", "Preferences"             }, eMenu::PREFS        },
		{ { "File", "Quit Ctrl-Q"             }, eMenu::QUIT         },

		{ { "Edit", "Undo Ctrl-Z"       }, eMenu::EDIT_UNDO       },
		{ { "Edit", "Redo Ctrl-Y"       }, eMenu::EDIT_REDO       },
		{ { "Edit", "Cut Ctrl-X"        }, eMenu::EDIT_CUT        },
		{ { "Edit", "Copy Ctrl-C"       }, eMenu::EDIT_COPY       },
		{ { "Edit", "Paste Ctrl-V"      }, eMenu::EDIT_PASTE      },
		{ { "Edit", "Delete"            }, eMenu::EDIT_DELETE     },
		{ { "Edit", "Select All Ctrl-A" }, eMenu::EDIT_SELECT_ALL },
	
		{ { "Scenario", "Advanced", "Edit Special Nodes"            }, eMenu::SCEN_SPECIALS        },
		{ { "Scenario", "Advanced", "Edit Scenario Text"            }, eMenu::SCEN_TEXT            },
		{ { "Scenario", "Advanced", "Edit Journal Entries"          }, eMenu::SCEN_JOURNALS        },
		{ { "Scenario", "Advanced", "Advanced Scenario Details"     }, eMenu::SCEN_ADV_DETAILS     },
		{ { "Scenario", "Advanced", "Import Town"                   }, eMenu::TOWN_IMPORT          },
		{ { "Scenario", "Advanced", "Import Outdoor Sector"         }, eMenu::OUT_IMPORT           },
		{ { "Scenario", "Advanced", "Edit Saved Item Rectangles"    }, eMenu::SCEN_SAVE_ITEM_RECTS },
		{ { "Scenario", "Advanced", "Set Variable Town Entry"       }, eMenu::TOWN_VARYING         },
		{ { "Scenario", "Advanced", "Set Scenario Event Timers"     }, eMenu::SCEN_TIMERS          },
		{ { "Scenario", "Advanced", "Edit Item Placement Shortcuts" }, eMenu::SCEN_ITEM_SHORTCUTS  },
		{ { "Scenario", "Advanced", "Delete Last Town"              }, eMenu::TOWN_DELETE          },
		{ { "Scenario", "Advanced", "Write Data To Text File"       }, eMenu::SCEN_DATA_DUMP       },
		{ { "Scenario", "Advanced", "Do Full Text Dump"             }, eMenu::SCEN_TEXT_DUMP       },
		// NOT IMPLEMENTED:
		// { { "Scenario", "Advanced", "Scenario Shopping Text Dump" }, eMenu::NONE                 },
		// { { "Scenario", "Advanced", "Scenario Monster Dump"       }, eMenu::NONE                 },
		// { { "Scenario", "Advanced", "Scenario Specials Dump"      }, eMenu::NONE                 },
		// { { "Scenario", "Advanced", "Scenario Object Data Dump"   }, eMenu::NONE                 },
	
		{ { "Scenario", "Launch From Here"           }, eMenu::LAUNCH_HERE     },
		{ { "Scenario", "Launch From Start"          }, eMenu::LAUNCH_START    },
		{ { "Scenario", "Launch From Town Entrance"  }, eMenu::LAUNCH_ENTRANCE },
		{ { "Scenario", "Create New Town"            }, eMenu::TOWN_CREATE     },
		{ { "Scenario", "Resize Outdoors"            }, eMenu::OUT_RESIZE      },
		{ { "Scenario", "Scenario Details"           }, eMenu::SCEN_DETAILS    },
		{ { "Scenario", "Scenario Intro Text"        }, eMenu::SCEN_INTRO      },
		{ { "Scenario", "Edit Custom Graphic Sheets" }, eMenu::SCEN_SHEETS     },
		{ { "Scenario", "Classify Custom Graphics"   }, eMenu::SCEN_PICS       },
		{ { "Scenario", "Edit Custom Sounds"         }, eMenu::SCEN_SNDS       },

		{ { "Town", "Advanced", "Edit Special Nodes"    }, eMenu::TOWN_SPECIALS },
		{ { "Town", "Advanced", "Edit Town Text"        }, eMenu::TOWN_TEXT     },
		{ { "Town", "Advanced", "Edit Town Signs"       }, eMenu::TOWN_SIGNS    },
		{ { "Town", "Advanced", "Advanced Town Details" }, eMenu::TOWN_ADVANCED },
		{ { "Town", "Advanced", "Set Town Event Timers" }, eMenu::TOWN_TIMERS   },
		// NOT IMPLEMENTED:
		// { { "Town", "Advanced", "Concise Town Report" }, eMenu::NONE          },
		
		{ { "Town", "Town Details"               }, eMenu::TOWN_DETAILS            },
		{ { "Town", "Town Wandering Monsters"    }, eMenu::TOWN_WANDERING          },
		{ { "Town", "Set Town Boundaries"        }, eMenu::TOWN_BOUNDARIES         },
		{ { "Town", "Frill Up Terrain"           }, eMenu::FRILL                   },
		{ { "Town", "Remove Terrain Frills"      }, eMenu::UNFRILL                 },
		{ { "Town", "Edit Area Descriptions"     }, eMenu::TOWN_AREAS              },
		{ { "Town", "Set Starting Location"      }, eMenu::TOWN_START              },
		{ { "Town", "Add Random Items"           }, eMenu::TOWN_ITEMS_RANDOM       },
		{ { "Town", "Set All Items Not Property" }, eMenu::TOWN_ITEMS_NOT_PROPERTY },
		{ { "Town", "Clear All Items"            }, eMenu::TOWN_ITEMS_CLEAR        },
		
		{ { "Outdoors", "Advanced", "Edit Special Nodes" }, eMenu::OUT_SPECIALS },
		{ { "Outdoors", "Advanced", "Edit Outdoor Text"  }, eMenu::OUT_TEXT     },
		{ { "Outdoors", "Advanced", "Edit Outdoor Signs" }, eMenu::OUT_SIGNS    },
		// NOT IMPLEMENTED:
		// { { "Outdoors", "Advanced", "Concise Outdoor Report" }, eMenu::NONE         },
	
		{ { "Outdoors", "Outdoor Details"            }, eMenu::OUT_DETAILS    },
		{ { "Outdoors", "Outdoor Wandering Monsters" }, eMenu::OUT_WANDERING  },
		{ { "Outdoors", "Outdoor Special Encounters" }, eMenu::OUT_ENCOUNTERS },
		{ { "Outdoors", "Frill Up Terrain"           }, eMenu::FRILL          },
		{ { "Outdoors", "Remove Terrain Frills"      }, eMenu::UNFRILL        },
		{ { "Outdoors", "Edit Area Descriptions"     }, eMenu::OUT_AREAS      },
		{ { "Outdoors", "Set Starting Location"      }, eMenu::OUT_START      },
		
		{ { "Help", "Index F1"                       }, eMenu::HELP_TOC       },
		{ { "Help", "About Blades Scenario Editor"   }, eMenu::ABOUT          },
		{ { "Help", "Getting Started"                }, eMenu::HELP_START     },
		{ { "Help", "Testing Your Scenario"          }, eMenu::HELP_TEST      },
		{ { "Help", "Distributing Your Scenario"     }, eMenu::HELP_DIST      },
	};

	// Note that signal connection ids are discarded.
	for(const auto& item : persistent_menu_items) {
		menubar->addMenuItem(item.first);
		menubar->connectMenuItem(item.first, handle_menu_choice, item.second);
	}
}

bool OpenBoESceneditMenu::handle_event(const sf::Event& event) {
	
	if(event.type == sf::Event::KeyPressed && this->handle_keypressed_event(event))
		return true;

	return this->tgui.handleEvent(event);
}

// Returns true if event was consumed
bool OpenBoESceneditMenu::handle_keypressed_event(const sf::Event& event) const {

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

bool OpenBoESceneditMenu::handle_ctrl_keypress(const sf::Event& event) const {
	auto menubar = this->get_menubar_ptr();
	bool event_was_consumed { false };

	switch(event.key.code) {
		case sf::Keyboard::S:
			if(!menubar->getMenuItemEnabled({ "File", "Save Scenario Ctrl-S" })) break;
			handle_menu_choice(eMenu::FILE_SAVE);
			event_was_consumed = true;
			break;
		case sf::Keyboard::W:
			if(!menubar->getMenuItemEnabled({ "File", "Close Scenario Ctrl-W" })) break;
			handle_menu_choice(eMenu::FILE_CLOSE);
			event_was_consumed = true;
			break;
		case sf::Keyboard::Z:
			if(!menubar->getMenuItemEnabled({ "Edit", "Undo Ctrl-Z" })) break;
			handle_menu_choice(eMenu::EDIT_UNDO);
			event_was_consumed = true;
			break;
		case sf::Keyboard::Y:
			if(!menubar->getMenuItemEnabled({ "Edit", "Redo Ctrl-Y" })) break;
			handle_menu_choice(eMenu::EDIT_REDO);
			event_was_consumed = true;
		case sf::Keyboard::X:
			if(!menubar->getMenuItemEnabled({ "Edit", "Cut Ctrl-X" })) break;
			handle_menu_choice(eMenu::EDIT_CUT);
			event_was_consumed = true;
			break;
		case sf::Keyboard::C:
			if(!menubar->getMenuItemEnabled({ "Edit", "Copy Ctrl-C" })) break;
			handle_menu_choice(eMenu::EDIT_COPY);
			event_was_consumed = true;
			break;
		case sf::Keyboard::V:
			if(!menubar->getMenuItemEnabled({ "Edit", "Paste Ctrl-V" })) break;
			handle_menu_choice(eMenu::EDIT_PASTE);
			event_was_consumed = true;
			break;
		case sf::Keyboard::A:
			if(!menubar->getMenuItemEnabled({ "Edit", "Select All Ctrl-A" })) break;
			handle_menu_choice(eMenu::EDIT_SELECT_ALL);
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
		case sf::Keyboard::N:
			handle_menu_choice(eMenu::FILE_NEW);
			event_was_consumed = true;
			break;
		default: break;
	}
	
	return event_was_consumed;
}

bool OpenBoESceneditMenu::handle_ctrl_shift_keypress(const sf::Event& event) const {
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

bool OpenBoESceneditMenu::is_control_key_pressed() const {

	// NOTE: Control is not cross-platform (apple)

	return (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
		 || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl));
}

bool OpenBoESceneditMenu::is_shift_key_pressed() const {
	return (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
		 || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift));
}

void OpenBoESceneditMenu::draw() {
	this->tgui.draw();
}

tgui::MenuBar::Ptr OpenBoESceneditMenu::get_menubar_ptr() const {
	return this->tgui.get<tgui::MenuBar>(this->internal_menubar_widget_name);
}

// mode 0: no scenario loaded
// mode 1: town menu disabled
// mode 2: outdoors menu disabled
// mode 3: both town and outdoors menus disabled
// mode 4: scenario loaded, everything enabled
void OpenBoESceneditMenu::update_for_mode(short mode) {
	switch(mode){
		case 0: this->update_for_mode_0(); break;
		case 1: this->update_for_mode_1(); break;
		case 2: this->update_for_mode_2(); break;
		case 3: this->update_for_mode_3(); break;
		case 4: this->update_for_mode_4(); break;
		default:
			throw std::runtime_error { "BUG: update_for_mode called with unknown mode!" };
	}
}

void OpenBoESceneditMenu::update_for_mode_0() {
	auto menubar = this->get_menubar_ptr();

	menubar->setMenuItemEnabled({ "File", "Save Scenario Ctrl-S" }, false);
	menubar->setMenuItemEnabled({ "File", "Save As... Ctrl-Shift-S" }, false);
	menubar->setMenuItemEnabled({ "File", "Close Scenario Ctrl-W" }, false);
	menubar->setMenuItemEnabled({ "File", "Revert to Saved" }, false);
	
	menubar->setMenuEnabled("Scenario", false);
	menubar->setMenuEnabled("Town",     false);
	menubar->setMenuEnabled("Outdoors", false);
}

void OpenBoESceneditMenu::update_for_mode_1() {
	auto menubar = this->get_menubar_ptr();

	menubar->setMenuEnabled("Town", false);
}

void OpenBoESceneditMenu::update_for_mode_2() {
	auto menubar = this->get_menubar_ptr();
	
	menubar->setMenuEnabled("Outdoors", false);
}

void OpenBoESceneditMenu::update_for_mode_3() {
	auto menubar = this->get_menubar_ptr();

	menubar->setMenuEnabled("Town",     false);
	menubar->setMenuEnabled("Outdoors", false);
}

void OpenBoESceneditMenu::update_for_mode_4() {
	auto menubar = this->get_menubar_ptr();

	menubar->setMenuItemEnabled({ "File", "Save Scenario Ctrl-S" }, true);
	menubar->setMenuItemEnabled({ "File", "Save As... Ctrl-Shift-S" }, true);
	menubar->setMenuItemEnabled({ "File", "Close Scenario Ctrl-W" }, true);
	menubar->setMenuItemEnabled({ "File", "Revert to Saved" }, true);
	
	menubar->setMenuEnabled("Scenario", true);
	menubar->setMenuEnabled("Town",     true);
	menubar->setMenuEnabled("Outdoors", true);
}

void OpenBoESceneditMenu::update_edit_menu(cUndoList const & undo_list) {
	auto menubar = this->get_menubar_ptr();

	menubar->setMenuItemEnabled({ "Edit", "Undo Ctrl-Z" }, !undo_list.noUndo());
	menubar->setMenuItemEnabled({ "Edit", "Redo Ctrl-Y" }, !undo_list.noRedo());
}

