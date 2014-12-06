//
//  dialog.keys.h
//  BoE
//
//  Created by Celtic Minstrel on 14-03-31.
//
//

#ifndef BoE_dialog_keys_h
#define BoE_dialog_keys_h

enum eKeyMod {
	mod_none = 0,
	mod_alt = 1, mod_shift = 2, mod_ctrl = 4,
	mod_altshift = mod_alt + mod_shift,
	mod_altctrl = mod_alt + mod_ctrl,
	mod_shiftctrl = mod_shift + mod_ctrl,
	mod_all = mod_alt + mod_shift + mod_ctrl,
};

enum eSpecKey {
	key_left, key_right, key_up, key_down,
	key_esc, key_enter, key_tab, key_help, // key_help should bind to the help key on Mac and the F1 key on Windows
	key_bsp, key_del, key_home, key_end, key_pgup, key_pgdn, // TODO: Implement these
	key_copy, key_cut, key_paste, key_selectall
	// TODO: On Mac, command-left should trigger key_home; command-right should trigger key_end;
	// command-up should trigger key_pgup; and command-down should trigger key_pgdn.
	// This is in addition to the home, end, pgup, pgdn keys triggering these.
};

struct cKey {
	bool spec;
	union {
		unsigned char c;
		eSpecKey k;
	};
	eKeyMod mod;
};

eKeyMod operator +  (eKeyMod lhs, eKeyMod rhs);
eKeyMod operator -  (eKeyMod lhs, eKeyMod rhs);
eKeyMod&operator += (eKeyMod&lhs, eKeyMod rhs);
eKeyMod&operator -= (eKeyMod&lhs, eKeyMod rhs);

bool operator== (cKey a, cKey b);
bool mod_contains(eKeyMod haystack, eKeyMod needle);

#endif
