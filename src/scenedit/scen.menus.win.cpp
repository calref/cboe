
#include "scen.menus.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "Resource.h"
#include "scenario.h"

// Include this last because some #defines in the Windows headers cause compile errors in my headers.
// Fortunately they're on symbols not used in this file, so this should work.
#include <Windows.h>

// This is the index of each menu on the menubar
enum {
	FILE_MENU_POS = 0,
	EDIT_MENU_POS = 1,
	SCEN_MENU_POS = 2,
	TOWN_MENU_POS = 3,
	OUT_MENU_POS = 4,
	ITEMS_MENU_POS = 6,
	MONST_MENU_POS = 11,
	HELP_MENU_POS = 15,
};

extern sf::RenderWindow mainPtr;
extern cScenario scenario;
LONG_PTR mainProc;
HMENU menuHandle = NULL;

LRESULT CALLBACK menuProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

void init_menubar() {
	HWND winHandle = mainPtr.getSystemHandle();
	if(winHandle == NULL) return;
	if(menuHandle == NULL)
		menuHandle = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
	SetMenu(winHandle, menuHandle);
	// Now we have to do a little hack to handle menu messages.
	// We replace SFML's window procedure with our own, which checks for menu events and then forwards to SFML's procedure.
	mainProc = SetWindowLongPtr(winHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&menuProc));
}

void update_item_menu() {
	if(menuHandle == NULL) return;
	for(int j = 0; j < 5; j++) {
		HMENU item_menu = GetSubMenu(menuHandle, ITEMS_MENU_POS + j);
		while(GetMenuItemCount(item_menu)) RemoveMenu(item_menu, 0, MF_BYPOSITION);
		for(int i = 0; i < 80; i++) {
			UINT flags = MF_STRING | MF_ENABLED;
			if(i % 40 == 0) flags |= MF_MENUBARBREAK;
			AppendMenuA(item_menu, flags, 10000 + i + j * 80, scenario.scen_items[i + j * 80].full_name.c_str());
		}
	}
	for(int j = 0; j < 4; j++) {
		HMENU mon_menu = GetSubMenu(menuHandle, MONST_MENU_POS + j);
		while(GetMenuItemCount(mon_menu)) RemoveMenu(mon_menu, 0, MF_BYPOSITION);
		for(int i = 0; i < 64; i++) {
			UINT flags = MF_STRING | MF_ENABLED;
			if(i % 32 == 0) flags |= MF_MENUBARBREAK;
			AppendMenuA(mon_menu, flags, 20000 + i + j * 64, scenario.scen_monsters[i + j * 64].m_name.c_str());
		}
	}
	DrawMenuBar(mainPtr.getSystemHandle());
}

void shut_down_menus(short mode) {
	if(menuHandle == NULL) return;
	HMENU file_menu = GetSubMenu(menuHandle, FILE_MENU_POS);
	if(mode == 0) {
		EnableMenuItem(file_menu, IDM_FILE_SAVE, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem(menuHandle, SCEN_MENU_POS, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, TOWN_MENU_POS, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, OUT_MENU_POS, MF_GRAYED | MF_BYPOSITION);
		for(int i = 0; i < 5; i++)
			EnableMenuItem(menuHandle, ITEMS_MENU_POS + i, MF_GRAYED | MF_BYPOSITION);
		for(int i = 0; i < 4; i++)
			EnableMenuItem(menuHandle, MONST_MENU_POS + i, MF_GRAYED | MF_BYPOSITION);
	}
	std::shared_ptr<char> buf(new char[256]);
	MENUITEMINFOA info;
	info.cbSize = sizeof(MENUITEMINFOA);
	info.dwTypeData = buf.get();
	info.fMask = MIIM_STRING | MIIM_FTYPE;
	if(mode == 4) {
		EnableMenuItem(file_menu, IDM_FILE_SAVE, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(menuHandle, SCEN_MENU_POS, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, TOWN_MENU_POS, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, OUT_MENU_POS, MF_ENABLED | MF_BYPOSITION);
		for(int i = 0; i < 5; i++)
			EnableMenuItem(menuHandle, ITEMS_MENU_POS + i, MF_ENABLED | MF_BYPOSITION);
		for(int i = 0; i < 4; i++)
			EnableMenuItem(menuHandle, MONST_MENU_POS + i, MF_ENABLED | MF_BYPOSITION);

		HMENU town_menu = GetSubMenu(menuHandle, TOWN_MENU_POS);
		for(int i = 0; i < GetMenuItemCount(town_menu); i++) {
			info.cch = 256;
			GetMenuItemInfoA(town_menu, i, true, &info);
			if(info.fType == MFT_SEPARATOR || !strcmp("Advanced", info.dwTypeData))
				continue;
			EnableMenuItem(town_menu, i, MF_ENABLED | MF_BYPOSITION);
		}

		HMENU out_menu = GetSubMenu(menuHandle, OUT_MENU_POS);
		for(int i = 0; i < GetMenuItemCount(out_menu); i++) {
			info.cch = 256;
			GetMenuItemInfoA(out_menu, i, true, &info);
			if(!strcmp("Advanced", info.dwTypeData)) continue;
			EnableMenuItem(out_menu, i, MF_ENABLED | MF_BYPOSITION);
		}
	}
	if((mode == 1) || (mode == 3)) {
		for(int i = 0; i < 5; i++)
			EnableMenuItem(menuHandle, ITEMS_MENU_POS + i, MF_GRAYED | MF_BYPOSITION);
		for(int i = 0; i < 4; i++)
			EnableMenuItem(menuHandle, MONST_MENU_POS + i, MF_GRAYED | MF_BYPOSITION);

		HMENU town_menu = GetSubMenu(menuHandle, TOWN_MENU_POS);
		for(int i = 0; i < GetMenuItemCount(town_menu); i++) {
			info.cch = 256;
			GetMenuItemInfoA(town_menu, i, true, &info);
			if(info.fType == MFT_SEPARATOR || info.dwTypeData[0] == ' ')
				continue;
			EnableMenuItem(town_menu, i, MF_GRAYED | MF_BYPOSITION);
		}
	}
	if((mode == 2) || (mode == 3)) {
		HMENU out_menu = GetSubMenu(menuHandle, OUT_MENU_POS);
		for(int i = 0; i < GetMenuItemCount(out_menu); i++) {
			info.cch = 256;
			GetMenuItemInfoA(out_menu, i, true, &info);
			if(info.fType == MFT_SEPARATOR || info.dwTypeData[0] == ' ')
				continue;
			EnableMenuItem(out_menu, i, MF_GRAYED | MF_BYPOSITION);
		}
	}
}

void handle_file_menu(int item_hit);
void handle_edit_menu(int item_hit);
void handle_scenario_menu(int item_hit);
void handle_town_menu(int item_hit);
void handle_outdoor_menu(int item_hit);
void handle_item_menu(int item_hit);
void handle_monst_menu(int item_hit);
void handle_help_menu(int item_hit);

LRESULT CALLBACK menuProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	if(message == WM_COMMAND) {
		int cmd = LOWORD(wParam);
		if(cmd >= 10000 && cmd < 20000) { // Item menus
			handle_item_menu(cmd - 10000);
		} else if(cmd >= 20000 && cmd < 30000) { // Monster menus
			handle_monst_menu(cmd - 20000);
		} else switch(cmd) {
			// File menu
			case IDM_FILE_NEW: handle_file_menu(3); break;
			case IDM_FILE_OPEN: handle_file_menu(1); break;
			case IDM_FILE_CLOSE: break;
			case IDM_FILE_SAVE: handle_file_menu(2); break;
			case IDM_FILE_REVERT: break;
			case IDM_FILE_QUIT: handle_file_menu(5); break;
			// Edit menu
			case IDM_EDIT_UNDO: handle_edit_menu(1); break;
			case IDM_EDIT_REDO: handle_edit_menu(2); break;
			case IDM_EDIT_CUT: handle_edit_menu(4); break;
			case IDM_EDIT_COPY: handle_edit_menu(5); break;
			case IDM_EDIT_PASTE: handle_edit_menu(6); break;
			case IDM_EDIT_DELETE: handle_edit_menu(7); break;
			case IDM_EDIT_SELECT: handle_edit_menu(8); break;
			// Scenario menu
			case IDM_SCEN_NEW_TOWN: handle_scenario_menu(1); break;
			case IDM_SCEN_DETAILS: handle_scenario_menu(3); break;
			case IDM_SCEN_INTRO: handle_scenario_menu(4); break;
			case IDM_SCEN_START: handle_scenario_menu(4); break;
			case IDM_SCEN_PASSWORD: break;
			// Scenario menu (advanced)
			case IDM_SCEN_ADV_SPECIALS: handle_scenario_menu(9); break;
			case IDM_SCEN_ADV_TEXT: handle_scenario_menu(10); break;
			case IDM_SCEN_ADV_JOURNAL: handle_scenario_menu(11); break;
			case IDM_SCEN_ADV_IMPORT_TOWN: handle_scenario_menu(12); break;
			case IDM_SCEN_ADV_SAVE_RECTS: handle_scenario_menu(13); break;
			case IDM_SCEN_ADV_HORSES: handle_scenario_menu(14); break;
			case IDM_SCEN_ADV_BOATS: handle_scenario_menu(15); break;
			case IDM_SCEN_ADV_TOWN_VARY: handle_scenario_menu(16); break;
			case IDM_SCEN_ADV_EVENTS: handle_scenario_menu(17); break;
			case IDM_SCEN_ADV_SHORTCUTS: handle_scenario_menu(18); break;
			case IDM_SCEN_ADV_DELETE_TOWN: handle_scenario_menu(19); break;
			case IDM_SCEN_ADV_DATA_DUMP: handle_scenario_menu(20); break;
			case IDM_SCEN_ADV_TEXT_DUMP: handle_scenario_menu(21); break;
			// Town menu
			case IDM_TOWN_DETAILS: handle_town_menu(1); break;
			case IDM_TOWN_WANDER: handle_town_menu(2); break;
			case IDM_TOWN_BOUNDS: handle_town_menu(3); break;
			case IDM_TOWN_FRILL: handle_town_menu(4); break;
			case IDM_TOWN_UNFRILL: handle_town_menu(5); break;
			case IDM_TOWN_AREAS: handle_town_menu(6); break;
			case IDM_TOWN_RANDOM_ITEMS: handle_town_menu(8); break;
			case IDM_TOWN_NOT_PROPERTY: handle_town_menu(9); break;
			case IDM_TOWN_CLEAR_ITEMS: handle_town_menu(10); break;
			// Town menu (advanced)
			case IDM_TOWN_ADV_SPECIALS: handle_town_menu(13); break;
			case IDM_TOWN_ADV_TEXT: handle_town_menu(14); break;
			case IDM_TOWN_ADV_SIGNS: handle_town_menu(15); break;
			case IDM_TOWN_ADV_DETAILS: handle_town_menu(16); break;
			case IDM_TOWN_ADV_EVENTS: handle_town_menu(17); break;
			case IDM_TOWN_ADV_REPORT: break;
			// Outdoors menu
			case IDM_OUT_DETAILS: handle_outdoor_menu(1); break;
			case IDM_OUT_WANDER: handle_outdoor_menu(2); break;
			case IDM_OUT_ENCOUNTER: handle_outdoor_menu(3); break;
			case IDM_OUT_FRILL: handle_outdoor_menu(4); break;
			case IDM_OUT_UNFRILL: handle_outdoor_menu(5); break;
			case IDM_OUT_AREAS: handle_outdoor_menu(6); break;
			case IDM_OUT_START: handle_outdoor_menu(8); break;
			// Outdoors menu (advanced)
			case IDM_OUT_ADV_SPECIALS: handle_outdoor_menu(11); break;
			case IDM_OUT_ADV_TEXT: handle_outdoor_menu(12); break;
			case IDM_OUT_ADV_SIGNS: handle_outdoor_menu(13); break;
			case IDM_OUT_ADV_REPORT: break;
			// Help menu
			case IDM_HELP_INDEX: ShellExecuteA(NULL, "open", "https://calref.net/~sylae/boe-doc/editor/About.html", NULL, NULL, SW_SHOWNORMAL); break;
			case IDM_HELP_ABOUT: handle_help_menu(0); break;
			case IDM_HELP_START: handle_help_menu(1); break;
			case IDM_HELP_TEST: handle_help_menu(2); break;
			case IDM_HELP_DISTRIBUTE: handle_help_menu(3); break;
		}
	}
	return CallWindowProc(reinterpret_cast<WNDPROC>(mainProc), handle, message, wParam, lParam);
}
