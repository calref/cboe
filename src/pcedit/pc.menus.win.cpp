
#include "pc.menus.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "Resource.h"
#include "universe.h"
#include "winutil.h"

// Include this last because some #defines in the Windows headers cause compile errors in my headers.
// Fortunately they're on symbols not used in this file, so this should work.
#include <Windows.h>

// This is the index of each menu on the menubar
enum {
	FILE_MENU_POS = 0,
	PARTY_MENU_POS = 1,
	SCEN_MENU_POS = 2,
	ITEMS_MENU_POS = 3,
	HELP_MENU_POS = 7,
};

extern sf::RenderWindow mainPtr;
extern cUniverse univ;
extern bool scen_items_loaded;
extern fs::path file_in_mem;
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
	// Fix the window's viewport so that everything is drawn correctly
	sf::Vector2u sz = mainPtr.getSize();
	double menubarHeight = getMenubarHeight();
	double usableHeight = sz.y - menubarHeight;
	sf::View view(sf::FloatRect(0, 0, sz.x, usableHeight));
	mainPtr.setView(view);
}

void update_item_menu() {
	if(menuHandle == NULL) return;
	cItemRec(& item_list)[400] = univ.scenario.scen_items;
	for(int j = 0; j < 4; j++) {
		HMENU items_menu = GetSubMenu(menuHandle, ITEMS_MENU_POS + j);
		while(GetMenuItemCount(items_menu)) RemoveMenu(items_menu, 0, MF_BYPOSITION);
		if(!scen_items_loaded) {
			AppendMenuA(items_menu, MF_STRING | MF_GRAYED, 1000, "Items Not Loaded");
		} else for(int i = 0; i < 100; i++) {
			cItemRec& item = item_list[i + j * 100];
			UINT flags = MF_STRING | MF_ENABLED;
			if(i % 25 == 0) flags |= MF_MENUBARBREAK;
			AppendMenuA(items_menu, flags, 1000 + j * 100 + i, item.full_name.c_str());
			// TODO: Also disable gold or food
			EnableMenuItem(items_menu, i, MF_BYPOSITION | (item.variety != eItemType::NO_ITEM ? MF_ENABLED : MF_GRAYED));
		}
	}
	DrawMenuBar(mainPtr.getSystemHandle());
}

void menu_activate() {
	if(menuHandle == NULL) return;
	HMENU file_menu = GetSubMenu(menuHandle, FILE_MENU_POS);
	if(file_in_mem.empty())
		for(int i = 1; i < GetMenuItemCount(file_menu) - 1; i++)
			EnableMenuItem(file_menu, i, MF_BYPOSITION | MF_GRAYED);
	else for(int i = 1; i < GetMenuItemCount(file_menu) - 1; i++)
		EnableMenuItem(file_menu, i, MF_BYPOSITION | MF_ENABLED);
	DrawMenuBar(mainPtr.getSystemHandle());
}

void handle_help_menu(int item_hit);
void handle_file_menu(int item_hit);
void handle_edit_menus(int item_hit);
void handle_item_menu(int item_hit);

#include "cursors.h"

LRESULT CALLBACK menuProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	if(message == WM_COMMAND) {
		int cmd = LOWORD(wParam);
		if(cmd >= 1000) { // Item menus
			handle_item_menu(cmd - 1000);
		} else switch(cmd) {
			// File menu
			case IDM_FILE_OPEN: handle_file_menu(3); break;
			case IDM_FILE_CLOSE: handle_file_menu(4); break;
			case IDM_FILE_SAVE: handle_file_menu(1); break;
			case IDM_FILE_SAVE_AS: handle_file_menu(2); break;
			case IDM_FILE_REVERT: handle_file_menu(5); break;
			case IDM_FILE_QUIT: handle_file_menu(7); break;
			// Edit Party menu
			case IDM_PARTY_GOLD: handle_edit_menus(1); break;
			case IDM_PARTY_FOOD: handle_edit_menus(2); break;
			case IDM_PARTY_ALCHEMY: handle_edit_menus(3); break;
			case IDM_PARTY_DAMAGE: handle_edit_menus(5); break;
			case IDM_PARTY_MANA: handle_edit_menus(6); break;
			case IDM_PARTY_DEAD: handle_edit_menus(7); break;
			case IDM_PARTY_CONDITIONS: handle_edit_menus(8); break;
			case IDM_PARTY_REUNITE: handle_edit_menus(10); break;
			case IDM_PARTY_VEHICLES: handle_edit_menus(11); break;
			case IDM_PARTY_MAGE: handle_edit_menus(13); break;
			case IDM_PARTY_PRIEST: handle_edit_menus(14); break;
			case IDM_PARTY_TRAITS: handle_edit_menus(15); break;
			case IDM_PARTY_SKILLS: handle_edit_menus(16); break;
			case IDM_PARTY_XP: handle_edit_menus(17); break;
			// Scenario Editing menu
			case IDM_SCEN_DAY: handle_edit_menus(101); break;
			case IDM_SCEN_LEAVE_TOWN: handle_edit_menus(103); break;
			case IDM_SCEN_TOWN_RESET: handle_edit_menus(104); break;
			case IDM_SCEN_TOWN_MAP: handle_edit_menus(105); break;
			case IDM_SCEN_OUT_MAP: handle_edit_menus(106); break;
			case IDM_SCEN_LEAVE: handle_edit_menus(108); break;
			case IDM_SCEN_SDF: handle_edit_menus(109); break;
			// Help menu
			case IDM_HELP: handle_help_menu(1);
		}
	} else if(message == WM_SETCURSOR) {
		// Windows resets the cursor to an arrow whenever the mouse moves, unless we do this.
		// Note: By handling this message, sf::Window::setMouseCursorVisible() will NOT work.
		restore_cursor();
		return true;
	}
	return CallWindowProc(reinterpret_cast<WNDPROC>(mainProc), handle, message, wParam, lParam);
}
