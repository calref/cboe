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
	@"DrawTerrainAnimation": @(kBool),
	@"DrawTerrainFrills": @(kBool),
	@"DrawTerrainShoreFrills": @(kBool),
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

static NSString* convertKey(std::string keypath) {
	NSString* key = [NSString stringWithCString: keypath.c_str() encoding: NSASCIIStringEncoding];
	return key;
}

void set_pref(std::string keypath, bool value) {
	[[NSUserDefaults standardUserDefaults] setBool: value forKey: convertKey(keypath)];
}

bool get_bool_pref(std::string keypath, bool fallback) {
	id val = [[NSUserDefaults standardUserDefaults] objectForKey: convertKey(keypath)];
	if([val isKindOfClass: [NSNumber class]]) return [val boolValue];
	return fallback;
}

void set_pref(std::string keypath, int value) {
	[[NSUserDefaults standardUserDefaults] setInteger: value forKey: convertKey(keypath)];
}

int get_int_pref(std::string keypath, int fallback) {
	id val = [[NSUserDefaults standardUserDefaults] objectForKey: convertKey(keypath)];
	if([val isKindOfClass: [NSNumber class]]) return [val intValue];
	return fallback;
}

void set_pref(std::string keypath, double value) {
	[[NSUserDefaults standardUserDefaults] setDouble: value forKey: convertKey(keypath)];
}

double get_float_pref(std::string keypath, double fallback) {
	id val = [[NSUserDefaults standardUserDefaults] objectForKey: convertKey(keypath)];
	if([val isKindOfClass: [NSNumber class]]) return [val doubleValue];
	return fallback;
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
	[[NSUserDefaults standardUserDefaults] setValue: nil forKey: convertKey(keypath)];
}

bool sync_prefs() {
	if(recording){
		std::ostringstream prefs_recording;
		NSUserDefaults* standard = [NSUserDefaults standardUserDefaults];
		for(id pref in prefsToRecord) {
			id object = [standard objectForKey: pref];
			if(object != nil){
				prefs_recording << [pref UTF8String] << " = ";
				NSInteger type = [prefsToRecord[pref] integerValue];
				bool bvalue;
				int ivalue;
				float fvalue;
				int count;
				int c;
				NSArray* arrayValue;
				switch((int)type) {
					case kBool:
						bvalue = [standard boolForKey: pref];
						if(bvalue == true){
							prefs_recording << "true";
						}else{
							prefs_recording << "false";
						}
						break;
					case kInt:
						ivalue = (int)[standard integerForKey: pref];
						prefs_recording << ivalue;
						break;
					case kIArray:
						arrayValue = [standard arrayForKey: pref];
						prefs_recording << "[";
						count = [arrayValue count];
						c = 0;
						for(id num in arrayValue){
							prefs_recording << [num integerValue];
							if (c < count - 1)
								prefs_recording << " ";
							++c;
						}
						prefs_recording << "]";
						break;
					case kFloat:
						fvalue = (float)[standard floatForKey: pref];
						prefs_recording << fvalue;
						break;
				}
				prefs_recording << std::endl;
			}
		}
		record_action("sync_prefs", prefs_recording.str());
	}else if(replaying){
		replayUserDefaults = [NSUserDefaults init];
	}
	return [[NSUserDefaults standardUserDefaults] synchronize];
}

