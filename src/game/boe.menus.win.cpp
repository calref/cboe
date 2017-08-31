
#include "boe.menus.hpp"
#include <map>
#include <sstream>
#include <SFML/Graphics/RenderWindow.hpp>
#include "boeresource.h"
#include "universe.hpp"
#include "boe.party.hpp"
#include "boe.infodlg.hpp"
#include "boe.consts.hpp"
#include "spell.hpp"
#include "winutil.hpp"
#include "menu_accel.win.hpp"

// Include this last because some #defines in the Windows headers cause compile errors in my headers.
// Fortunately they're on symbols not used in this file, so this should work.
#include <windows.h>
#include <gl/GL.h>
#undef HELP_INDEX // Except this one
#undef FILE_OPEN

// This is the index of each menu on the menubar
enum {
	FILE_MENU_POS = 0,
	OPT_MENU_POS = 1,
	ACT_MENU_POS = 2,
	MONST_MENU_POS = 3,
	MAGE_MENU_POS = 4,
	PRIEST_MENU_POS = 5,
	LIB_MENU_POS = 6,
	HELP_MENU_POS = 7,
};

extern short on_spell_menu[2][62];
extern short on_monst_menu[256];
extern bool party_in_memory;
extern cUniverse univ;
extern eGameMode overall_mode;
extern sf::RenderWindow mainPtr;
LONG_PTR mainProc;
HMENU menuHandle = NULL;
accel_table_t accel;
std::map<int,eMenu> menuChoices;

LRESULT CALLBACK menuProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

void setMenuCommand(HMENU& menu, int i, eMenu cmd) {
	static char title[256];
	MENUITEMINFOA item;
	item.cbSize = sizeof(MENUITEMINFOA);
	item.cch = 255;
	item.dwTypeData = title;
	item.fMask = MIIM_ID | MIIM_FTYPE | MIIM_STRING;
	GetMenuItemInfoA(menu, i++, true, &item);
	if(item.fType == MFT_SEPARATOR) return;
	menuChoices[item.wID] = cmd;
	// Now set up the accelerator, if any
	std::string item_name = item.dwTypeData;
	size_t pos = item_name.find_last_of('\t');
	if(pos == std::string::npos) return;
	pos++;
	if(pos >= item_name.size()) return;
	std::string key_name = item_name.substr(pos);
	accel.add(item.wID, key_name);
}

void init_menubar() {
	HWND winHandle = mainPtr.getSystemHandle();
	if(winHandle == NULL) return;
	if(menuHandle == NULL)
		menuHandle = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_BLADESOFEXILE));
	SetMenu(winHandle, menuHandle);
	// Now we have to do a little hack to handle menu messages.
	// We replace SFML's window procedure with our own, which checks for menu events and then forwards to SFML's procedure.
	mainProc = SetWindowLongPtr(winHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&menuProc));
	mainPtr.setActive();
	// Fix the window's viewport so that everything is drawn correctly
	sf::Vector2u sz = mainPtr.getSize();
	double menubarHeight = getMenubarHeight();
	double usableHeight = sz.y - menubarHeight;
	sf::View view(sf::FloatRect(0, 0, sz.x, usableHeight));
	mainPtr.setView(view);
	
	// And now initialize the mapping from Windows menu commands to eMenu constants
	static bool inited = false;
	if(inited) return;
	inited = true;
	
	static const eMenu file_choices[] = {
		eMenu::FILE_NEW, eMenu::FILE_OPEN, eMenu::NONE, eMenu::FILE_ABORT, eMenu::FILE_SAVE, eMenu::FILE_SAVE_AS, eMenu::NONE, eMenu::PREFS, eMenu::NONE, eMenu::QUIT,
	};
	static const eMenu opt_choices[] = {
		eMenu::OPTIONS_PC_GRAPHIC, eMenu::OPTIONS_RENAME_PC, eMenu::OPTIONS_NEW_PC, eMenu::OPTIONS_DELETE_PC, eMenu::NONE,
		eMenu::OPTIONS_TALK_NOTES, eMenu::OPTIONS_ENCOUNTER_NOTES, eMenu::OPTIONS_STATS, eMenu::OPTIONS_JOURNAL,
	};
	static const eMenu act_choices[] = {
		eMenu::ACTIONS_ALCHEMY, eMenu::ACTIONS_WAIT, eMenu::ACTIONS_AUTOMAP,
	};
	static const eMenu lib_choices[] = {
		eMenu::LIBRARY_MAGE, eMenu::LIBRARY_PRIEST, eMenu::LIBRARY_SKILLS, eMenu::LIBRARY_ALCHEMY, eMenu::LIBRARY_TIPS,
		eMenu::NONE, eMenu::LIBRARY_INTRO,
	};
	static const eMenu help_choices[] = {
		eMenu::HELP_TOC, eMenu::ABOUT, eMenu::NONE, eMenu::HELP_OUT, eMenu::HELP_TOWN, eMenu::HELP_COMBAT, eMenu::HELP_BARRIER,
		eMenu::NONE, eMenu::HELP_HINTS, eMenu::HELP_SPELLS,
	};
	
	HMENU file_menu = GetSubMenu(menuHandle, FILE_MENU_POS);
	HMENU opt_menu = GetSubMenu(menuHandle, OPT_MENU_POS);
	HMENU act_menu = GetSubMenu(menuHandle, ACT_MENU_POS);
	HMENU lib_menu = GetSubMenu(menuHandle, LIB_MENU_POS);
	HMENU help_menu = GetSubMenu(menuHandle, HELP_MENU_POS);
	
	int i = 0;
	for(eMenu opt : file_choices)
		setMenuCommand(file_menu, i++, opt);
	i = 0;
	for(eMenu opt : opt_choices)
		setMenuCommand(opt_menu, i++, opt);
	i = 0;
	for(eMenu opt : act_choices)
		setMenuCommand(act_menu, i++, opt);
	i = 0;
	for(eMenu opt : lib_choices)
		setMenuCommand(lib_menu, i++, opt);
	i = 0;
	for(eMenu opt : help_choices)
		setMenuCommand(help_menu, i++, opt);
	
	menuChoices[IDM_MAGE_ABOUT] = eMenu::ABOUT_MAGE;
	menuChoices[IDM_PRIEST_ABOUT] = eMenu::ABOUT_PRIEST;
	menuChoices[IDM_MONSTERS_ABOUT] = eMenu::ABOUT_MONSTERS;

	accel.build();
}

void adjust_monst_menu() {
	if(menuHandle == NULL) return;
	short i, monst_pos = 0;
	HMENU monst_menu;

	if(overall_mode == MODE_STARTUP) return;

	monst_menu = GetSubMenu(menuHandle, MONST_MENU_POS);
	for(i = 0; i < 256; i++) {
		on_monst_menu[i] = -1;
	}
	for(i = 1; i < 256; i++) {
		if(i == 1 || univ.party.m_noted.count(i) > 0) {
			on_monst_menu[monst_pos] = i;
			monst_pos++;
		}
	}

	while(GetMenuItemCount(monst_menu) > 2) {
		RemoveMenu(monst_menu, 2, MF_BYPOSITION);
	}
	for(i = 0; i < 256; i++) {
		if(on_monst_menu[i] >= 0) {
			std::string monst_name = univ.scenario.scen_monsters[on_monst_menu[i]].m_name;
			AppendMenuA(monst_menu, MF_STRING | MF_ENABLED, 1000 + i, monst_name.c_str());
		}
	}
	DrawMenuBar(mainPtr.getSystemHandle());
}

void init_spell_menus() {
	short i, j;

	for(i = 0; i < 2; i++)
		for(j = 0; j < 62; j++)
			on_spell_menu[i][j] = -1;
}

void adjust_spell_menus() {
	if(menuHandle == NULL) return;
	short i, j, spell_pos = 0;
	HMENU spell_menu;
	short old_on_spell_menu[2][62];
	bool need_menu_change = false;

	if(overall_mode == MODE_STARTUP || univ.cur_pc == 6)
		return;

	for(i = 0; i < 2; i++)
		for(j = 0; j < 62; j++)
			old_on_spell_menu[i][j] = on_spell_menu[i][j];

	spell_menu = GetSubMenu(menuHandle, MAGE_MENU_POS);

	for(i = 0; i < 62; i++) {
		on_spell_menu[0][i] = -1;
	}
	for(i = 0; i < 62; i++)
		if(pc_can_cast_spell(univ.current_pc(), cSpell::fromNum(eSkill::MAGE_SPELLS, i))) {
			on_spell_menu[0][spell_pos] = i;
			spell_pos++;
		}
	for(i = 0; i < 62; i++)
		if(on_spell_menu[0][i] != old_on_spell_menu[0][i])
			need_menu_change = true;
	if(need_menu_change) {
		while(GetMenuItemCount(spell_menu) > 2) {
			RemoveMenu(spell_menu, 2, MF_BYPOSITION);
		}
		for(i = 0; i < 62; i++)
			if(on_spell_menu[0][i] >= 0) {
				eSpell spell = cSpell::fromNum(eSkill::MAGE_SPELLS, on_spell_menu[0][i]);
				std::ostringstream sout;
				sout << " L" << (*spell).level << " - " << (*spell).name() << ", C ";
				if((*spell).cost >= 0) sout << (*spell).cost; else sout << '?';
				AppendMenuA(spell_menu, MF_STRING | MF_ENABLED, 2000 + int(spell), sout.str().c_str());
			}
	}

	need_menu_change = false;
	spell_pos = 0;

	spell_menu = GetSubMenu(menuHandle, PRIEST_MENU_POS);

	for(i = 0; i < 62; i++) {
		on_spell_menu[1][i] = -1;
	}
	for(i = 0; i < 62; i++)
		if(pc_can_cast_spell(univ.current_pc(), cSpell::fromNum(eSkill::PRIEST_SPELLS, i))) {
			on_spell_menu[1][spell_pos] = i;
			spell_pos++;
		}
	for(i = 0; i < 62; i++)
		if(on_spell_menu[1][i] != old_on_spell_menu[1][i])
			need_menu_change = true;
	if(need_menu_change) {
		while(GetMenuItemCount(spell_menu) > 2) {
			RemoveMenu(spell_menu, 2, MF_BYPOSITION);
		}
		for(i = 0; i < 62; i++)
			if(on_spell_menu[1][i] >= 0) {
				eSpell spell = cSpell::fromNum(eSkill::MAGE_SPELLS, on_spell_menu[1][i]);
				std::ostringstream sout;
				sout << " L" << (*spell).level << " - " << (*spell).name() << ", C ";
				if((*spell).cost >= 0) sout << (*spell).cost; else sout << '?';
				AppendMenuA(spell_menu, MF_STRING | MF_ENABLED, 2000 + int(spell), sout.str().c_str());
			}
	}


}

void menu_activate() {
	if(menuHandle == NULL) return;
	HMENU file_menu = GetSubMenu(menuHandle, FILE_MENU_POS);
	if(overall_mode == MODE_STARTUP) {
		EnableMenuItem(menuHandle, OPT_MENU_POS, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, MONST_MENU_POS, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, ACT_MENU_POS, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, MAGE_MENU_POS, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, PRIEST_MENU_POS, MF_GRAYED | MF_BYPOSITION);

		EnableMenuItem(file_menu, IDM_FILE_SAVE, MF_GRAYED | MF_BYCOMMAND);
		if(!party_in_memory) {
			EnableMenuItem(file_menu, IDM_FILE_SAVE_AS, MF_GRAYED | MF_BYCOMMAND);
		} else {
			EnableMenuItem(file_menu, IDM_FILE_SAVE_AS, MF_ENABLED | MF_BYCOMMAND);
		}
	} else {
		EnableMenuItem(menuHandle, OPT_MENU_POS, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, MONST_MENU_POS, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, ACT_MENU_POS, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, MAGE_MENU_POS, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, PRIEST_MENU_POS, MF_ENABLED | MF_BYPOSITION);

		EnableMenuItem(file_menu, IDM_FILE_SAVE, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(file_menu, IDM_FILE_SAVE_AS, MF_ENABLED | MF_BYCOMMAND);
	}
	DrawMenuBar(mainPtr.getSystemHandle());
}

void hideMenuBar() {
	if(menuHandle == NULL) return;
	SetMenu(mainPtr.getSystemHandle(), NULL);
	DrawMenuBar(mainPtr.getSystemHandle());
}

void showMenuBar() {
	if(menuHandle == NULL) return;
	SetMenu(mainPtr.getSystemHandle(), menuHandle);
	DrawMenuBar(mainPtr.getSystemHandle());
}

#include "cursors.hpp"

LRESULT CALLBACK menuProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	MSG msg = {handle, message, wParam, lParam};
	if(HIWORD(wParam) != 1 || message != WM_COMMAND) {
		if(TranslateAccelerator(handle, accel.handle, &msg))
			return 0;
	}
	if(message == WM_COMMAND) {
		int cmd = LOWORD(wParam);
		if(cmd >= 1000 && cmd < 2000) {
			handle_monster_info_menu(cmd - 1000 + 1);
		} else if(cmd >= 2000 && cmd < 3000) {
			handle_menu_spell(cSpell::fromNum(eSkill::MAGE_SPELLS, cmd - 2000));
		} else if(cmd >= 3000 && cmd < 4000) {
			handle_menu_spell(cSpell::fromNum(eSkill::PRIEST_SPELLS, cmd - 3000));
		} else handle_menu_choice(menuChoices[cmd]);
	} else if(message == WM_SETCURSOR) {
		// Windows resets the cursor to an arrow whenever the mouse moves, unless we do this.
		// Note: By handling this message, sf::Window::setMouseCursorVisible() will NOT work.
		restore_cursor();
		return true;
	}
	return CallWindowProc(reinterpret_cast<WNDPROC>(mainProc), handle, message, wParam, lParam);
}

#include "fileio.hpp"
#include "boe.graphics.hpp"
#include "boe.actions.hpp"
#include "boe.fileio.hpp"

extern bool ae_loading, finished_init;
void set_up_apple_events(int argc, char* argv[]) {
	if(argc > 1) {
		if(!load_party(argv[1], univ))
			return;

		if(!finished_init) {
			ae_loading = true;
			overall_mode = MODE_STARTUP;
		} else finish_load_party();
		if(overall_mode != MODE_STARTUP)
			end_startup();
		if(overall_mode != MODE_STARTUP)
			post_load();
	}
}
