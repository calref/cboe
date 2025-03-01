
#include "pc.menus.hpp"
#include <map>
#include <SFML/Graphics/RenderWindow.hpp>
#include "pcresource.h"
#include "universe.hpp"
#include "winutil.hpp"
#include "menu_accel.win.hpp"

// Include this last because some #defines in the Windows headers cause compile errors in my headers.
// Fortunately they're on symbols not used in this file, so this should work.
#include <windows.h>

// This is the index of each menu on the menubar
enum {
	FILE_MENU_POS = 0,
	PARTY_MENU_POS = 1,
	SCEN_MENU_POS = 2,
	HELP_MENU_POS = 3,
};

//extern sf::RenderWindow mainPtr;
extern cUniverse univ;
extern bool scen_items_loaded, party_in_scen;
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
	sf::RenderWindow& mainPtr = get_main_window();
	HWND winHandle = mainPtr.getSystemHandle();
	if(winHandle == NULL) return;
	if(menuHandle == NULL)
		menuHandle = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
	// Now we have to do a little hack to handle menu messages.
	// We replace SFML's window procedure with our own, which checks for menu events and then forwards to SFML's procedure.
	mainProc = SetWindowLongPtr(winHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&menuProc));
	SetMenu(winHandle, menuHandle);
	
	// And now initialize the mapping from Windows menu commands to eMenu constants
	static bool inited = false;
	if(inited) return;
	inited = true;
	
	static const eMenu file_choices[] = {
		eMenu::FILE_OPEN, eMenu::FILE_CLOSE, eMenu::NONE, eMenu::FILE_SAVE, eMenu::FILE_SAVE_AS, eMenu::FILE_REVERT, eMenu::NONE, eMenu::QUIT,
	};
	static const eMenu party_choices[] = {
		eMenu::EDIT_GOLD, eMenu::EDIT_FOOD, eMenu::EDIT_ALCHEMY, eMenu::NONE,
		eMenu::HEAL_DAMAGE, eMenu::RESTORE_MANA, eMenu::RAISE_DEAD, eMenu::CURE_CONDITIONS, eMenu::NONE,
		eMenu::EDIT_MAGE, eMenu::EDIT_PRIEST, eMenu::EDIT_ITEM, eMenu::EDIT_TRAITS, eMenu::EDIT_SKILLS, eMenu::EDIT_XP, eMenu::NONE,
		eMenu::REUNITE_PARTY,
	};
	static const eMenu scen_choices[] = {
		eMenu::EDIT_DAY, eMenu::NONE, eMenu::LEAVE_TOWN, eMenu::RESET_TOWNS, eMenu::ADD_OUT_MAPS, eMenu::ADD_TOWN_MAPS, eMenu::OWN_VEHICLES,
		eMenu::NONE, eMenu::LEAVE_SCENARIO, eMenu::SET_SDF,
	};
	static const eMenu help_choices[] = {
		eMenu::ABOUT, eMenu::HELP_TOC
	};
	
	HMENU file_menu = GetSubMenu(menuHandle, FILE_MENU_POS);
	HMENU party_menu = GetSubMenu(menuHandle, PARTY_MENU_POS);
	HMENU scen_menu = GetSubMenu(menuHandle, SCEN_MENU_POS);
	HMENU help_menu = GetSubMenu(menuHandle, HELP_MENU_POS);
	
	int i = 0;
	for(eMenu opt : file_choices)
		setMenuCommand(file_menu, i++, opt);
	i = 0;
	for(eMenu opt : party_choices)
		setMenuCommand(party_menu, i++, opt);
	i = 0;
	for(eMenu opt : scen_choices)
		setMenuCommand(scen_menu, i++, opt);
	i = 0;
	for(eMenu opt : help_choices)
		setMenuCommand(help_menu, i++, opt);

	accel.build();
}

void menu_activate() {
	sf::RenderWindow& mainPtr = get_main_window();
	if(menuHandle == NULL) return;
	HMENU file_menu = GetSubMenu(menuHandle, FILE_MENU_POS);
	if(univ.file.empty()) {
		EnableMenuItem(menuHandle, PARTY_MENU_POS, MF_BYPOSITION | MF_GRAYED);
		EnableMenuItem(menuHandle, SCEN_MENU_POS, MF_BYPOSITION | MF_GRAYED);
		for(int i = 1; i < GetMenuItemCount(file_menu) - 1; i++)
			EnableMenuItem(file_menu, i, MF_BYPOSITION | MF_GRAYED);
	} else {
		EnableMenuItem(menuHandle, PARTY_MENU_POS, MF_BYPOSITION | MF_ENABLED);
		EnableMenuItem(menuHandle, SCEN_MENU_POS, MF_BYPOSITION | (party_in_scen ? MF_ENABLED : MF_GRAYED));
		for(int i = 1; i < GetMenuItemCount(file_menu) - 1; i++)
			EnableMenuItem(file_menu, i, MF_BYPOSITION | MF_ENABLED);
	}
	DrawMenuBar(mainPtr.getSystemHandle());
}

#include "cursors.hpp"

LRESULT CALLBACK menuProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	MSG msg = {handle, message, wParam, lParam};
	
	// Adding the menubar to the window for the first time triggers an unwanted
	// resizing of the window, which we skip processing because it skews our
	// viewport:
	static bool menubarTriggeredResize = false;
	if(message == WM_SIZE && !menubarTriggeredResize) {
		menubarTriggeredResize = true;
		return true;
	}

	if(HIWORD(wParam) != 1 || message != WM_COMMAND) {
		if(TranslateAccelerator(handle, accel.handle, &msg))
			return 0;
	}
	if(message == WM_COMMAND) {
		int cmd = LOWORD(wParam);
		handle_menu_choice(menuChoices[cmd]);
	} else if(message == WM_SETCURSOR) {
		// Windows resets the cursor to an arrow whenever the mouse moves, unless we do this.
		// Note: By handling this message, sf::Window::setMouseCursorVisible() will NOT work.
		restore_cursor();
		return true;
	}
	return CallWindowProc(reinterpret_cast<WNDPROC>(mainProc), handle, message, wParam, lParam);
}

bool menuBarProcessEvent(const sf::Event&) {
	return false;
}

void drawMenuBar() {
}

void set_up_apple_events() {
}
