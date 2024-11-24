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
	kIArray = 3
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
	@"RepeatRoomDescriptions": @(kBool),
	@"ShowInstantHelp": @(kBool),
	@"ShowStartupFlash": @(kBool),
	@"UIScale": @(kFloat),
	@"UIScaleMap": @(kFloat)
};

bool prefsLoaded = false;

static NSString* convertKey(std::string keypath) {
	NSString* key = [NSString stringWithCString: keypath.c_str() encoding: NSASCIIStringEncoding];
	return key;
}

static NSUserDefaults* getCurrentDefaults() {
	if(replaying){
		return replayUserDefaults;
	}else{
		return [NSUserDefaults standardUserDefaults];
	}
}

void set_pref(std::string keypath, bool value) {
	[getCurrentDefaults() setBool: value forKey: convertKey(keypath)];
}

bool get_bool_pref(std::string keypath, bool fallback) {
	id val = [getCurrentDefaults() objectForKey: convertKey(keypath)];
	if([val isKindOfClass: [NSNumber class]]) return [val boolValue];
	return fallback;
}

void set_pref(std::string keypath, int value) {
	[getCurrentDefaults() setInteger: value forKey: convertKey(keypath)];
}

int get_int_pref(std::string keypath, int fallback) {
	id val = [getCurrentDefaults() objectForKey: convertKey(keypath)];
	if([val isKindOfClass: [NSNumber class]]) return [val intValue];
	return fallback;
}

void set_pref(std::string keypath, double value) {
	[getCurrentDefaults() setDouble: value forKey: convertKey(keypath)];
}

double get_float_pref(std::string keypath, double fallback) {
	id val = [getCurrentDefaults() objectForKey: convertKey(keypath)];
	if([val isKindOfClass: [NSNumber class]]) return [val doubleValue];
	return fallback;
}

void append_iarray_pref(std::string keypath, int value) {
	NSString* key = convertKey(keypath);
	NSArray* list = [getCurrentDefaults() arrayForKey: key];
	NSNumber* num = [NSNumber numberWithInt: value];
	if(list == nil)
		[getCurrentDefaults() setObject: [NSArray arrayWithObject: num] forKey: key] ;
	else [getCurrentDefaults() setObject: [list arrayByAddingObject: num] forKey: key];
}

std::vector<int> get_iarray_pref(std::string keypath) {
	NSArray* list = [getCurrentDefaults() arrayForKey: convertKey(keypath)];
	if(list == nil) return {};
	std::vector<int> result;
	for(size_t i = 0; i < [list count]; i++)
		result.push_back([[list objectAtIndex: i] intValue]);
	return result;
}

void clear_pref(std::string keypath) {
	[getCurrentDefaults() setValue: nil forKey: convertKey(keypath)];
}

// When replaying, load the preferences from the action log into a
// non-synchronized UserDefaults object
static bool load_prefs(std::istream& istream) {
	std::string line;
	while(std::getline(istream, line)) {
		if(!istream) {
			perror("Error reading preferences");
			return false;
		}
		if(line[0] == '#') continue;
		int eq = line.find_first_of('=');
		if(eq == std::string::npos) {
			printf("Error reading preferences: line is not a comment and lacks an = sign:\n\t%s\n", line.c_str());
			return false;
		}
		int key_end = line.find_last_not_of(' ', eq - 1), val_beg = line.find_first_not_of(' ', eq + 1);
		if(val_beg == std::string::npos) {
			printf("Error reading preferences: line is missing value:\n\t%s\n", line.c_str());
			return false;
		}
		if(key_end == std::string::npos) {
			printf("Error reading preferences: line is missing key:\n\t%s\n", line.c_str());
			return false;
		}

		std::string key = line.substr(0, key_end + 1), val = line.substr(val_beg);
		NSInteger type = [prefsToRecord[[NSString stringWithUTF8String: key.c_str()]] integerValue];
		switch((int)type) {
			case kBool:
				if(val == "true") set_pref(key, true);
				else if(val == "false") set_pref(key, false);
				break;
			case kIArray:
				if(val[0] == '[') {
					int arr_end = val.find_first_of(']');
					std::istringstream arr_vals(val.substr(1, arr_end - 1));
					int i = 0;
					clear_pref(key);
					while(arr_vals >> i) append_iarray_pref(key, i);
				}
				break;
			case kFloat:
				set_pref(key, boost::lexical_cast<double>(val));
				break;
			case kInt:
				set_pref(key, boost::lexical_cast<int>(val));
				break;
		}
	}

	prefsLoaded = true;
	return true;
}

bool sync_prefs() {
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
						if(bvalue == true){
							prefs_recording << "true";
						}else{
							prefs_recording << "false";
						}
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
				}
				prefs_recording << std::endl;
			}
		}
		record_action("load_prefs", prefs_recording.str(), true);
		prefsLoaded = true;
	}else if(replaying && !prefsLoaded){
		replayUserDefaults = [[NSUserDefaults alloc] init];
		Element& prefs_action = pop_next_action("load_prefs");
		std::istringstream istream(prefs_action.GetText());
		return load_prefs(istream);
	}
	return [[NSUserDefaults standardUserDefaults] synchronize];
}

