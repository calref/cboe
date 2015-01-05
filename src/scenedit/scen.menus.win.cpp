
#include "scen.menus.h"
#include <map>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Resource.h"
#include "scenario.h"
#include "winutil.h"

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
std::map<int,eMenu> menuChoices;

LRESULT CALLBACK menuProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

void setMenuCommand(HMENU& menu, int i, eMenu cmd) {
	MENUITEMINFOA item;
	item.cbSize = sizeof(MENUITEMINFOA);
	item.fMask = MIIM_ID | MIIM_FTYPE;
	GetMenuItemInfoA(file_menu, i++, true, &item);
	if(info.fType == MFT_SEPARATOR) return;
	menuChoices[item.wID] = cmd;
}

void init_menubar() {
	HWND winHandle = mainPtr.getSystemHandle();
	if(winHandle == NULL) return;
	if(menuHandle == NULL)
		menuHandle = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
	SetMenu(winHandle, menuHandle);
	// Now we have to do a little hack to handle menu messages.
	// We replace SFML's window procedure with our own, which checks for menu events and then forwards to SFML's procedure.
	mainProc = SetWindowLongPtr(winHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&menuProc));
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
		eMenu::FILE_NEW, eMenu::FILE_OPEN, eMenu::NONE, eMenu::FILE_CLOSE, eMenu::FILE_SAVE, eMenu::FILE_REVERT, eMenu::NONE, eMenu::QUIT,
	};
	static const eMenu edit_choices[] = {
		eMenu::EDIT_UNDO, eMenu::EDIT_REDO, eMenu::NONE,
		eMenu::EDIT_CUT, eMenu::EDIT_COPY, eMenu::EDIT_PASTE, eMenu::EDIT_DELETE, eMenu::EDIT_SELECT_ALL,
	};
	static const eMenu scen_choices[] = {
		eMenu::TOWN_CREATE, eMenu::NONE, eMenu::SCEN_DETAILS, eMenu::SCEN_INTRO, eMenu::TOWN_START, eMenu::NONE, eMenu::NONE,
		eMenu::SCEN_SPECIALS, eMenu::SCEN_TEXT, eMenu::SCEN_JOURNALS, eMenu::TOWN_IMPORT, eMenu::SCEN_SAVE_ITEM_RECTS,
		eMenu::SCEN_HORSES, eMenu::SCEN_BOATS, eMenu::TOWN_VARYING, eMenu::SCEN_TIMERS, eMenu::SCEN_ITEM_SHORTCUTS,
		eMenu::TOWN_DELETE, eMenu::SCEN_DATA_DUMP, eMenu::SCEN_TEXT_DUMP,
	};
	static const eMenu town_choices[] = {
		eMenu::TOWN_DETAILS, eMenu::TOWN_WANDERING, eMenu::TOWN_BOUNDARIES, eMenu::FRILL, eMenu::UNFRILL, eMenu::TOWN_AREAS,
		eMenu::NONE, eMenu::TOWN_ITEMS_RANDOM, eMenu::TOWN_ITEMS_NOT_PROPERTY, eMenu::TOWN_ITEMS_CLEAR, eMenu::NONE, eMenu::NONE,
		eMenu::TOWN_SPECIALS, eMenu::TOWN_TEXT, eMenu::TOWN_SIGNS, eMenu::TOWN_ADVANCED, eMenu::TOWN_TIMERS,
	};
	static const eMenu out_choices[] = {
		eMenu::OUT_DETAILS, eMenu::OUT_WANDERING, eMenu::OUT_ENCOUNTERS, eMenu::FRILL, eMenu::UNFRILL, eMenu::OUT_AREAS,
		eMenu::NONE, eMenu::OUT_START, eMenu::NONE, eMenu::NONE,
		eMenu::OUT_SPECIALS, eMenu::OUT_TEXT, eMenu::OUT_SIGNS,
	};
	static const eMenu help_choices[] = {
		eMenu::HELP_INDEX, eMenu::ABOUT, eMenu::NONE, eMenu::HELP_START, eMenu::HELP_TEST, eMenu::HELP_DIST,
	};
	
	HMENU file_menu = GetSubMenu(menuHandle, FILE_MENU_POS);
	HMENU edit_menu = GetSubMenu(menuHandle, EDIT_MENU_POS);
	HMENU scen_menu = GetSubMenu(menuHandle, SCEN_MENU_POS);
	HMENU town_menu = GetSubMenu(menuHandle, TOWN_MENU_POS);
	HMENU out_menu = GetSubMenu(menuHandle, OUT_MENU_POS);
	HMENU help_menu = GetSubMenu(menuHandle, HELP_MENU_POS);
	
	int i = 0;
	for(eMenu opt : file_choices)
		setMenuCommand(file_menu, i++, opt);
	i = 0;
	for(eMenu opt : edit_choices)
		setMenuCommand(edit_menu, i++, opt);
	i = 0;
	for(eMenu opt : scen_choices)
		setMenuCommand(scen_menu, i++, opt);
	i = 0;
	for(eMenu opt : town_choices)
		setMenuCommand(town_menu, i++, opt);
	i = 0;
	for(eMenu opt : out_choices)
		setMenuCommand(out_menu, i++, opt);
	i = 0;
	for(eMenu opt : help_choices)
		setMenuCommand(help_menu, i++, opt);
}

void update_item_menu() {
	if(menuHandle == NULL) return;
	for(int j = 0; j < 5; j++) {
		HMENU item_menu = GetSubMenu(menuHandle, ITEMS_MENU_POS + j);
		while(GetMenuItemCount(item_menu)) RemoveMenu(item_menu, 0, MF_BYPOSITION);
		for(int i = 0; i < 80; i++) {
			UINT flags = MF_STRING | MF_ENABLED;
			if(i % 20 == 0) flags |= MF_MENUBARBREAK;
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

#include "cursors.h"

LRESULT CALLBACK menuProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	if(message == WM_COMMAND) {
		int cmd = LOWORD(wParam);
		if(cmd >= 10000 && cmd < 20000) { // Item menus
			handle_item_menu(cmd - 10000);
		} else if(cmd >= 20000 && cmd < 30000) { // Monster menus
			handle_monst_menu(cmd - 20000);
		} else handle_menu_choice(menuChoices[message]);
	} else if(message == WM_SETCURSOR) {
		// Windows resets the cursor to an arrow whenever the mouse moves, unless we do this.
		// Note: By handling this message, sf::Window::setMouseCursorVisible() will NOT work.
		restore_cursor();
		return true;
	}
	return CallWindowProc(reinterpret_cast<WNDPROC>(mainProc), handle, message, wParam, lParam);
}
