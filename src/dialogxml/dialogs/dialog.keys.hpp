//
//  dialog.keys.h
//  BoE
//
//  Created by Celtic Minstrel on 14-03-31.
//
//

#ifndef BoE_dialog_keys_h
#define BoE_dialog_keys_h

/// @file
/// Key-related classes and types.

/// Keyboard modifiers, as a bit-field-like enumeration.
/// Note that mod_ctrl refers to both the control key and the Mac's command key.
/// It also covers the "meta" key supported in certain other systems, and may
/// also be triggered by the Windows key.
///
/// Use mod_contains() to check if a specific modifier is set in the bit field.
enum eKeyMod {
	mod_none = 0, ///< No modifier
	/// @{ A single modifier
	mod_alt = 1, mod_shift = 2, mod_ctrl = 4,
	/// @}
	/// @{ Two modifiers
	mod_altshift = mod_alt + mod_shift,
	mod_altctrl = mod_alt + mod_ctrl,
	mod_shiftctrl = mod_shift + mod_ctrl,
	/// @}
	/// All modifiers
	mod_all = mod_alt + mod_shift + mod_ctrl,
};

/// Representations of special keys.
/// Not all of these represent a literal single keypress.
/// Some refer to common two-keypress keyboard shortcuts.
enum eSpecKey {
	key_left, key_right, key_up, key_down,
	key_esc, key_enter, key_tab, key_help, // key_help should bind to the help key on Mac and the F1 key on Windows
	key_bsp, key_del, key_home, key_end, key_pgup, key_pgdn, key_top, key_bottom, key_insert,
	key_copy, key_cut, key_paste, key_selectall, key_undo, key_redo,
	key_word_left, key_word_right, key_word_bsp, key_word_del,
};

/// Represents a keypress.
struct cKey {
	/// If true, it's a special key. Otherwise, a character has been typed.
	bool spec;
	union {
		/// The character that has been typed.
		wchar_t c;
		/// The special key that was pressed.
		eSpecKey k;
	};
	/// A bit field of held key modifiers.
	eKeyMod mod;
};

/// Combine two key modifiers.
/// @param lhs @param rhs
/// @return lhs + rhs
eKeyMod operator +  (eKeyMod lhs, eKeyMod rhs);
/// Cancel out a key modifier.
/// @param lhs @param rhs
/// @return lhs - rhs
eKeyMod operator -  (eKeyMod lhs, eKeyMod rhs);
/// Combine two key modifiers.
/// @param lhs The key modifier set to modify.
/// @param rhs The key modifier to remove.
/// @return lhs, now modified.
eKeyMod&operator += (eKeyMod&lhs, eKeyMod rhs);
/// Cancel out a key modifier.
/// @param lhs The key modifier set to modify.
/// @param rhs The key modifier to remove.
/// @return lhs, now modified.
eKeyMod&operator -= (eKeyMod&lhs, eKeyMod rhs);

/// Compare two keys.
/// @param a @param b
/// @return Whether they are equal.
bool operator== (cKey a, cKey b);
/// Check if haystack contains the modifier specified by needle.
/// @param haystack The set of modifiers to check.
/// @param needle The modifier to check for; generally one of mod_alt, mod_shift, or mod_ctrl.
/// @return true if the needle is in the haystack
bool mod_contains(eKeyMod haystack, eKeyMod needle);

#endif
