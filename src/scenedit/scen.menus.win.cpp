
#include "scen.menus.hpp"
#include <map>
#include <SFML/Graphics/RenderWindow.hpp>
#include "scenresource.h"
#include "scenario.hpp"
#include "winutil.hpp"
#include "menu_accel.win.hpp"
#include "undo.hpp"

// Include this last because some #defines in the Windows headers cause compile errors in my headers.
// Fortunately they're on symbols not used in this file, so this should work.
#include <windows.h>
#undef HELP_INDEX // Except this one.

// This is the index of each menu on the menubar
enum {
	FILE_MENU_POS = 0,
	EDIT_MENU_POS = 1,
	SCEN_MENU_POS = 2,
	TOWN_MENU_POS = 3,
	OUT_MENU_POS = 4,
	HELP_MENU_POS = 6,
};

extern sf::RenderWindow mainPtr;
extern cScenario scenario;
extern cUndoList undo_list;
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
		eMenu::FILE_NEW, eMenu::FILE_OPEN, eMenu::NONE, eMenu::FILE_CLOSE, eMenu::FILE_SAVE, eMenu::FILE_SAVE_AS, eMenu::FILE_REVERT, eMenu::NONE, eMenu::PREFS, eMenu::QUIT,
	};
	static const eMenu edit_choices[] = {
		eMenu::EDIT_UNDO, eMenu::EDIT_REDO, eMenu::NONE,
		eMenu::EDIT_CUT, eMenu::EDIT_COPY, eMenu::EDIT_PASTE, eMenu::EDIT_DELETE, eMenu::EDIT_SELECT_ALL,
	};
	static const eMenu scen_choices[] = {
		eMenu::LAUNCH_HERE, eMenu::LAUNCH_START, eMenu::LAUNCH_ENTRANCE, eMenu::NONE,
		eMenu::TOWN_CREATE, eMenu::OUT_RESIZE, eMenu::NONE,
		eMenu::SCEN_DETAILS, eMenu::SCEN_INTRO, eMenu::SCEN_SHEETS, eMenu::SCEN_PICS, eMenu::SCEN_SNDS, eMenu::NONE, eMenu::NONE,
		eMenu::SCEN_SPECIALS, eMenu::SCEN_TEXT, eMenu::SCEN_JOURNALS, eMenu::SCEN_ADV_DETAILS,
		eMenu::TOWN_IMPORT, eMenu::OUT_IMPORT, eMenu::SCEN_SAVE_ITEM_RECTS,
		eMenu::TOWN_VARYING, eMenu::SCEN_TIMERS, eMenu::SCEN_ITEM_SHORTCUTS,
		eMenu::TOWN_DELETE, eMenu::SCEN_DATA_DUMP, eMenu::SCEN_TEXT_DUMP,
	};
	static const eMenu town_choices[] = {
		eMenu::TOWN_DETAILS, eMenu::TOWN_WANDERING, eMenu::TOWN_BOUNDARIES, eMenu::FRILL, eMenu::UNFRILL, eMenu::TOWN_AREAS,
		eMenu::NONE, eMenu::TOWN_START, eMenu::TOWN_ITEMS_RANDOM, eMenu::TOWN_ITEMS_NOT_PROPERTY, eMenu::TOWN_ITEMS_CLEAR,
		eMenu::NONE, eMenu::NONE,
		eMenu::TOWN_SPECIALS, eMenu::TOWN_TEXT, eMenu::TOWN_SIGNS, eMenu::TOWN_ADVANCED, eMenu::TOWN_TIMERS,
	};
	static const eMenu out_choices[] = {
		eMenu::OUT_DETAILS, eMenu::OUT_WANDERING, eMenu::OUT_ENCOUNTERS, eMenu::FRILL, eMenu::UNFRILL, eMenu::OUT_AREAS,
		eMenu::NONE, eMenu::OUT_START, eMenu::NONE, eMenu::NONE,
		eMenu::OUT_SPECIALS, eMenu::OUT_TEXT, eMenu::OUT_SIGNS,
	};
	static const eMenu help_choices[] = {
		eMenu::HELP_TOC, eMenu::ABOUT, eMenu::NONE, eMenu::HELP_START, eMenu::HELP_TEST, eMenu::HELP_DIST,
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

	accel.build();
}

void shut_down_menus(short mode) {
	if(menuHandle == NULL) return;
	HMENU file_menu = GetSubMenu(menuHandle, FILE_MENU_POS);
	if(mode == 0) {
		EnableMenuItem(file_menu, IDM_FILE_SAVE, MF_GRAYED | MF_BYCOMMAND);
		EnableMenuItem(menuHandle, SCEN_MENU_POS, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, TOWN_MENU_POS, MF_GRAYED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, OUT_MENU_POS, MF_GRAYED | MF_BYPOSITION);
	}
	std::shared_ptr<char> buf(new char[256], std::default_delete<char[]>());
	MENUITEMINFOA info;
	info.cbSize = sizeof(MENUITEMINFOA);
	info.dwTypeData = buf.get();
	info.fMask = MIIM_STRING | MIIM_FTYPE;
	if(mode == 4) {
		EnableMenuItem(file_menu, IDM_FILE_SAVE, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(menuHandle, SCEN_MENU_POS, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, TOWN_MENU_POS, MF_ENABLED | MF_BYPOSITION);
		EnableMenuItem(menuHandle, OUT_MENU_POS, MF_ENABLED | MF_BYPOSITION);

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
	DrawMenuBar(mainPtr.getSystemHandle());
}

void update_edit_menu() {
	if(menuHandle == NULL) return;
	HMENU edit_menu = GetSubMenu(menuHandle, EDIT_MENU_POS);
	if(undo_list.noUndo()) {
		ModifyMenuA(edit_menu, IDM_EDIT_UNDO, MF_BYCOMMAND, IDM_EDIT_UNDO, "Can't Undo\tCtrl+Z");
		EnableMenuItem(edit_menu, IDM_EDIT_UNDO, MF_BYCOMMAND | MF_GRAYED);
	} else {
		std::string title = "Undo " + undo_list.undoName() + "\tCtrl+Z";
		ModifyMenuA(edit_menu, IDM_EDIT_UNDO, MF_BYCOMMAND, IDM_EDIT_UNDO, title.c_str());
		EnableMenuItem(edit_menu, IDM_EDIT_UNDO, MF_BYCOMMAND | MF_ENABLED);
	}
	if(undo_list.noRedo()) {
		ModifyMenuA(edit_menu, IDM_EDIT_REDO, MF_BYCOMMAND, IDM_EDIT_REDO, "Can't Redo\tCtrl+Y");
		EnableMenuItem(edit_menu, IDM_EDIT_REDO, MF_BYCOMMAND | MF_GRAYED);
	} else {
		std::string title = "Redo " + undo_list.redoName() + "\tCtrl+Y";
		ModifyMenuA(edit_menu, IDM_EDIT_REDO, MF_BYCOMMAND, IDM_EDIT_REDO, title.c_str());
		EnableMenuItem(edit_menu, IDM_EDIT_REDO, MF_BYCOMMAND | MF_ENABLED);
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

void set_up_apple_events() {
}
