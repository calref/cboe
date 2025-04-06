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
#include "replay.hpp"
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "ticpp.h"

//static const CFStringRef prefsID = CFSTR("com.spidweb.bladesofexile");
typedef NS_ENUM(NSInteger) {
	kInt = 0,
	kBool = 1,
	kFloat = 2,
	kIArray = 3,
	kString = 4
} PrefType;

NSUserDefaults* replayUserDefaults = nil;
NSDictionary* prefsToRecord = @{
	@"DisplayMode": @(kInt),
	@"EasyMode": @(kBool),
	@"GameRunBefore": @(kBool),
	@"GameSpeed": @(kInt),
	@"GiveIntroHint": @(kBool),
	@"LessWanderingMonsters": @(kBool),
	@"PlaySounds": @(kBool),
	@"ReceivedHelp": @(kIArray),
	@"ShowInstantHelp": @(kBool),
	@"ShowStartupSplash": @(kBool),
	@"UIScale": @(kFloat),
	@"UIScaleMap": @(kFloat)
};

bool prefsLoaded = false;

static NSString* convertKey(std::string keypath) {
	NSString* key = [NSString stringWithCString: keypath.c_str() encoding: NSASCIIStringEncoding];
	return key;
}

static std::string convertValue(NSString* value) {
	return std::string([value cStringUsingEncoding : NSUTF8StringEncoding]);
}

static NSString* convertValue(std::string value) {
	NSString* key = [NSString stringWithCString: value.c_str() encoding: NSUTF8StringEncoding];
	return key;
}

static NSUserDefaults* getCurrentDefaults() {
	return [NSUserDefaults standardUserDefaults];
}

// No push - ignore until end of file
#pragma clang diagnostic ignored "-Wmissing-prototypes"

void set_pref_mac(std::string keypath, bool value) {
	[getCurrentDefaults() setBool: value forKey: convertKey(keypath)];
}

bool get_bool_pref_mac(std::string keypath, bool fallback) {
	id val = [getCurrentDefaults() objectForKey: convertKey(keypath)];
	if([val isKindOfClass: [NSNumber class]]) return [val boolValue];
	return fallback;
}

void set_pref_mac(std::string keypath, int value) {
	[getCurrentDefaults() setInteger: value forKey: convertKey(keypath)];
}

int get_int_pref_mac(std::string keypath, int fallback) {
	id val = [getCurrentDefaults() objectForKey: convertKey(keypath)];
	if([val isKindOfClass: [NSNumber class]]) return [val intValue];
	return fallback;
}

void set_pref_mac(std::string keypath, double value) {
	[getCurrentDefaults() setDouble: value forKey: convertKey(keypath)];
}

double get_double_pref_mac(std::string keypath, double fallback) {
	id val = [getCurrentDefaults() objectForKey: convertKey(keypath)];
	if([val isKindOfClass: [NSNumber class]]) return [val doubleValue];
	return fallback;
}

void append_iarray_pref_mac(std::string keypath, int value) {
	NSString* key = convertKey(keypath);
	NSArray* list = [getCurrentDefaults() arrayForKey: key];
	NSNumber* num = [NSNumber numberWithInt: value];
	if(list == nil)
		[getCurrentDefaults() setObject: [NSArray arrayWithObject: num] forKey: key] ;
	else [getCurrentDefaults() setObject: [list arrayByAddingObject: num] forKey: key];
}

std::vector<int> get_iarray_pref_mac(std::string keypath) {
	NSArray* list = [getCurrentDefaults() arrayForKey: convertKey(keypath)];
	if(list == nil) return {};
	std::vector<int> result;
	for(size_t i = 0; i < [list count]; i++)
		result.push_back([[list objectAtIndex: i] intValue]);
	return result;
}

void set_pref_mac(std::string keypath, std::string val) {
	NSString* key = convertKey(keypath);
	NSString* value = convertValue(val);
	[getCurrentDefaults() setObject: value forKey: key];
}

std::string get_string_pref_mac(std::string keypath, std::string fallback) {
	NSString* val = [getCurrentDefaults() stringForKey: convertKey(keypath)];
	if(val == nil) return fallback;
	return convertValue(val);
}

void clear_pref_mac(std::string keypath) {
	[getCurrentDefaults() setValue: nil forKey: convertKey(keypath)];
}

bool sync_prefs_mac() {
	if(recording && !prefsLoaded){
		std::ostringstream prefs_recording;
		NSUserDefaults* standard = [NSUserDefaults standardUserDefaults];
		for(id pref in prefsToRecord) {
			id object = [standard objectForKey: pref];
			if(object != nil){
				prefs_recording << [pref UTF8String] << " = ";
				NSInteger type = [prefsToRecord[pref] integerValue];
				switch((int)type) {
					case kBool: {
						bool bvalue = [standard boolForKey: pref];
						prefs_recording << bool_to_str(bvalue);
					} break;
					case kInt: {
						int ivalue = (int)[standard integerForKey: pref];
						prefs_recording << ivalue;
					} break;
					case kIArray: {
						NSArray* arrayValue = [standard arrayForKey: pref];
						prefs_recording << "[";
						int count = [arrayValue count];
						int c = 0;
						for(id num in arrayValue){
							prefs_recording << [num integerValue];
							if (c < count - 1)
								prefs_recording << " ";
							++c;
						}
						prefs_recording << "]";
					} break;
					case kFloat: {
						double fvalue = (double)[standard doubleForKey: pref];
						prefs_recording << fvalue;
					} break;
					// NOTE: The core game currently has no string preferences, so the recording system doesn't need to know
					// about them for now.
					case kString: break;
				}
				prefs_recording << std::endl;
			}
		}
		record_action("load_prefs", prefs_recording.str(), true);
		prefsLoaded = true;
	}
	return [[NSUserDefaults standardUserDefaults] synchronize];
}

