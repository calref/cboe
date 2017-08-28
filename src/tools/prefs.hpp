//
//  prefs.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-04-14.
//
//

#ifndef BoE_prefs_hpp
#define BoE_prefs_hpp

#include <string>
#include <vector>
#include <algorithm>

void set_pref(std::string keypath, bool value);
bool get_bool_pref(std::string keypath, bool fallback = false);

void set_pref(std::string keypath, int value);
int get_int_pref(std::string keypath, int fallback = 0);

void append_iarray_pref(std::string keypath, int value);
std::vector<int> get_iarray_pref(std::string keypath);

inline bool get_iarray_pref_contains(std::string keypath, int value) {
	const std::vector<int>& val = get_iarray_pref(keypath);
	return std::find(val.begin(), val.end(), value) != val.end();
}

void clear_pref(std::string keypath);
bool sync_prefs();

#endif
