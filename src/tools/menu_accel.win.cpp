
#include "menu_accel.win.hpp"
#include <sstream>

void accel_table_t::add(WORD cmd, std::string key) {
	size_t pos = 0;
	bool ctrl = false, alt = false, shift = false;
	while(true) {
		if(key.substr(pos, 5) == "Ctrl+") {
			ctrl = true;
			pos += 5;
		} else if(key.substr(pos, 4) == "Alt+") {
			alt = true;
			pos += 4;
		} else if(key.substr(pos, 6) == "Shift+") {
			shift = true;
			pos += 6;
		} else if(pos >= key.size()) return;
		else break;
	}
	WORD keycode;
	key = key.substr(pos);
	if(key.length() == 0) return;
	else if(key.length() == 1) {
		SHORT virtkey = VkKeyScanA(key[0]);
		keycode = LOBYTE(virtkey);
	} else if(key[0] == 'F') {
		std::istringstream parse;
		parse.str(key.substr(1));
		int fkey = -1;
		parse >> fkey;
		// We allow for 24 F-keys because that's how many VK_Fx constants there are
		if(fkey <= 0 || fkey > 24)
			return;
		keycode = VK_F1 + fkey - 1;
	} else if(key == "Backsp") keycode = VK_BACK;
	else if(key == "Enter") keycode = VK_RETURN;
	else if(key == "Tab") keycode = VK_TAB;
	else if(key == "Esc") keycode = VK_ESCAPE;
	else if(key == "PgUp") keycode = VK_PRIOR;
	else if(key == "PgDn") keycode = VK_NEXT;
	else if(key == "End") keycode = VK_END;
	else if(key == "Home") keycode = VK_HOME;
	else if(key == "Left") keycode = VK_LEFT;
	else if(key == "Right") keycode = VK_RIGHT;
	else if(key == "Up") keycode = VK_UP;
	else if(key == "Down") keycode = VK_DOWN;
	else if(key == "Del") keycode = VK_DELETE;
	else return;
	ACCEL accel;
	accel.cmd = cmd;
	accel.key = keycode;
	accel.fVirt = FVIRTKEY;
	if(ctrl) accel.fVirt |= FCONTROL;
	if(alt) accel.fVirt |= FALT;
	if(shift) accel.fVirt |= FSHIFT;
	table.push_back(accel);
}

void accel_table_t::build() {
	if(handle == NULL)
		handle = CreateAcceleratorTableA(table.data(), table.size());
}

void accel_table_t::destroy() {
	if(handle != NULL)
		DestroyAcceleratorTable(handle);
	handle = NULL;
}

accel_table_t::~accel_table_t() {
	destroy();
}