
#include "boe.menus.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "Resource.h"
#include "universe.h"
#include "boe.party.h"
#include "boe.infodlg.h"
#include "boe.consts.h"
#include "spell.hpp"
#include "winutil.h"

// Include this last because some #defines in the Windows headers cause compile errors in my headers.
// Fortunately they're on symbols not used in this file, so this should work.
#include <Windows.h>
#include <gl/GL.h>

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
extern short current_pc;
extern cUniverse univ;
extern eGameMode overall_mode;
extern sf::RenderWindow mainPtr;
LONG_PTR mainProc;
HMENU menuHandle = NULL;

LRESULT CALLBACK menuProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

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
		if((i == 1) || (univ.party.m_noted[i] > 0)) {
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
	char spell_name[256];
	short old_on_spell_menu[2][62];
	bool need_menu_change = false;

	if(overall_mode == MODE_STARTUP || current_pc == 6)
		return;

	for(i = 0; i < 2; i++)
		for(j = 0; j < 62; j++)
			old_on_spell_menu[i][j] = on_spell_menu[i][j];

	spell_menu = GetSubMenu(menuHandle, MAGE_MENU_POS);

	for(i = 0; i < 62; i++) {
		on_spell_menu[0][i] = -1;
	}
	for(i = 0; i < 62; i++)
	if(pc_can_cast_spell(current_pc, cSpell::fromNum(eSkill::MAGE_SPELLS, i))) {
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
			std::string name = get_str("magic-names", i + 1);
			if((*spell).cost >= 0)
				sprintf(spell_name, " L%d - %s, C %d", (*spell).level, name.c_str(), (*spell).cost);
			else sprintf(spell_name, " L%d - %s, C ?", (*spell).level, name.c_str());
			AppendMenuA(spell_menu, MF_STRING | MF_ENABLED, 2000 + i, spell_name);
		}
	}

	need_menu_change = false;
	spell_pos = 0;

	spell_menu = GetSubMenu(menuHandle, PRIEST_MENU_POS);

	for(i = 0; i < 62; i++) {
		on_spell_menu[1][i] = -1;
	}
	for(i = 0; i < 62; i++)
	if(pc_can_cast_spell(current_pc, cSpell::fromNum(eSkill::PRIEST_SPELLS, i))) {
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
			std::string name = get_str("magic-names", i + 101);
			if((*spell).cost >= 0)
				sprintf(spell_name, " L%d - %s, C %d", (*spell).level, name.c_str(), (*spell).cost);
			else sprintf(spell_name, " L%d - %s, C ?", (*spell).level, name.c_str());
			AppendMenuA(spell_menu, MF_STRING | MF_ENABLED, 2000 + i, spell_name);
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

void handle_file_menu(int item_hit);
void handle_options_menu(int item_hit);
void handle_help_menu(int item_hit);
void handle_library_menu(int item_hit);
void handle_actions_menu(int item_hit);
void handle_monster_info_menu(int item_hit);
void handle_menu_spell(eSpell spell_picked);

#include "cursors.h"

LRESULT CALLBACK menuProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	if(message == WM_COMMAND) {
		int cmd = LOWORD(wParam);
		if(cmd >= 1000 && cmd < 2000) {
			handle_monster_info_menu(cmd - 1000);
		} else if(cmd >= 2000 && cmd < 3000) {
			handle_menu_spell(cSpell::fromNum(eSkill::MAGE_SPELLS, cmd - 2000));
		} else if(cmd >= 3000 && cmd < 4000) {
			handle_menu_spell(cSpell::fromNum(eSkill::PRIEST_SPELLS, cmd - 3000));
		} else switch(cmd) {
			// File menu
			case IDM_FILE_NEW: handle_file_menu(4); break;
			case IDM_FILE_OPEN: handle_file_menu(1); break;
			case IDM_FILE_ABORT: handle_file_menu(0); break;
			case IDM_FILE_SAVE: handle_file_menu(2); break;
			case IDM_FILE_SAVE_AS: handle_file_menu(3); break;
			case IDM_FILE_PREFS: handle_file_menu(6); break;
			case IDM_FILE_QUIT: handle_file_menu(8); break;
			// Options menu
			case IDM_OPTIONS_GRAPHIC: handle_options_menu(1); break;
			case IDM_OPTIONS_NAME: handle_options_menu(2); break;
			case IDM_OPTIONS_NEW: handle_options_menu(3); break;
			case IDM_OPTIONS_DELETE: handle_options_menu(4); break;
			case IDM_OPTIONS_TALKING: handle_options_menu(6); break;
			case IDM_OPTIONS_ENCOUNTER: handle_options_menu(7); break;
			case IDM_OPTIONS_STATS: handle_options_menu(8); break;
			// Actions menu
			case IDM_ACTIONS_ALCHEMY: handle_actions_menu(1); break;
			case IDM_ACTIONS_WAIT: handle_actions_menu(2); break;
			case IDM_ACTIONS_MAP: handle_actions_menu(3); break;
			// Monster/Spell menu About items
			case IDM_MONSTERS_ABOUT: break;
			case IDM_MAGE_ABOUT: give_help(209, 0); break;
			case IDM_PRIEST_ABOUT: give_help(209, 0); break;
			// Library menu
			case IDM_LIBRARY_MAGE: handle_library_menu(1); break;
			case IDM_LIBRARY_PRIEST: handle_library_menu(2); break;
			case IDM_LIBRARY_ALCHEMY: handle_library_menu(3); break;
			case IDM_LIBRARY_SKILLS: handle_library_menu(4); break;
			case IDM_LIBRARY_TIPS: handle_library_menu(5); break;
			case IDM_LIBRARY_INTRO: handle_library_menu(7); break;
			// Help menu
			case IDM_HELP_INDEX: ShellExecuteA(NULL, "open", "https://calref.net/~sylae/boe-doc/game/Contents.html", NULL, NULL, SW_SHOWNORMAL); break;
			case IDM_HELP_ABOUT: handle_help_menu(10); break;
			case IDM_HELP_OUTDOOR: handle_help_menu(1); break;
			case IDM_HELP_TOWN: handle_help_menu(2); break;
			case IDM_HELP_COMBAT: handle_help_menu(3); break;
			case IDM_HELP_BARRIER: handle_help_menu(4); break;
			case IDM_HELP_HINTS: handle_help_menu(6); break;
			case IDM_HELP_SPELLS: handle_help_menu(7); break;
		}
	} else if(message == WM_SETCURSOR) {
		// Windows resets the cursor to an arrow whenever the mouse moves, unless we do this.
		// Note: By handling this message, sf::Window::setMouseCursorVisible() will NOT work.
		restore_cursor();
		return true;
	}
	return CallWindowProc(reinterpret_cast<WNDPROC>(mainProc), handle, message, wParam, lParam);
}
