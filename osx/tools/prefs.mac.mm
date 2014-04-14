//
//  prefs.mac.mm
//  BoE
//
//  Created by Celtic Minstrel on 14-04-14.
//
//

#include "prefs.hpp"
#include <Foundation/Foundation.h>
#include <unordered_map>

//static const CFStringRef prefsID = CFSTR("com.spidweb.bladesofexile");

NSString* convertKey(std::string keypath) {
	NSString* key = [NSString stringWithCString: keypath.c_str() encoding: NSASCIIStringEncoding];
	return [key autorelease];
}

void set_pref(std::string keypath, bool value) {
	[[NSUserDefaults standardUserDefaults] setBool: value forKey: convertKey(keypath)];
}

bool get_bool_pref(std::string keypath) {
	return [[NSUserDefaults standardUserDefaults] boolForKey: convertKey(keypath)];
}

void set_pref(std::string keypath, int value) {
	[[NSUserDefaults standardUserDefaults] setInteger: value forKey: convertKey(keypath)];
}

int get_int_pref(std::string keypath) {
	return [[NSUserDefaults standardUserDefaults] integerForKey: convertKey(keypath)];
}

void append_iarray_pref(std::string keypath, int value) {
	NSString* key = convertKey(keypath);
	NSArray* list = [[NSUserDefaults standardUserDefaults] arrayForKey: key];
	NSNumber* num = [NSNumber numberWithInt: value];
	if(list == nil)
		[[NSUserDefaults standardUserDefaults] setObject: [NSArray arrayWithObject: num] forKey: key] ;
	else [[NSUserDefaults standardUserDefaults] setObject: [list arrayByAddingObject: num] forKey: key];
}

std::vector<int> get_iarray_pref(std::string keypath) {
	NSArray* list = [[NSUserDefaults standardUserDefaults] arrayForKey: convertKey(keypath)];
	if(list == nil) return {};
	std::vector<int> result;
	for(size_t i = 0; i < [list count]; i++)
		result.push_back([[list objectAtIndex: i] intValue]);
	return result;
}

void clear_pref(std::string keypath) {
	[[NSUserDefaults standardUserDefaults] setNilValueForKey: convertKey(keypath)];
}

bool sync_prefs() {
	return [[NSUserDefaults standardUserDefaults] synchronize];
}

