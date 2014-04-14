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

void set_pref(std::string keypath, bool value);
bool get_bool_pref(std::string keypath);

void set_pref(std::string keypath, int value);
int get_int_pref(std::string keypath);

void append_iarray_pref(std::string keypath, int value);
std::vector<int> get_iarray_pref(std::string keypath);

void clear_pref(std::string keypath);
bool sync_prefs();

#endif
