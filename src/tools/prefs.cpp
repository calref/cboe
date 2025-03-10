
#include "prefs.hpp"
#include "game/boe.global.hpp"
#include "tools/replay.hpp"
#include "fileio/fileio.hpp"
#include "ticpp.h"
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

std::map<std::string,boost::any> prefs;
using iarray = std::vector<int>;
static bool prefsLoaded = false, prefsDirty = false;

// These can make the use of platform-specific overloads DRY
#ifdef SFML_SYSTEM_MACOS
#define SET_PREF_MAC(type) void set_pref_mac(std::string keypath, type value); if(!was_replaying){ set_pref_mac(keypath, value); return; }
#define GET_PREF_MAC(type) type get_##type##_pref_mac(std::string keypath, type fallback); if(!was_replaying){ return get_##type##_pref_mac(keypath, fallback); }
#else
#define SET_PREF_MAC(type)
#define GET_PREF_MAC(type)
#endif

void set_pref(std::string keypath, bool value) {
	SET_PREF_MAC(bool);
	prefsDirty = true;
	prefs[keypath] = value;
}

bool get_bool_pref(std::string keypath, bool fallback) {
	GET_PREF_MAC(bool);
	if(prefs.find(keypath) == prefs.end()) return fallback;
	if(prefs[keypath].type() == typeid(bool)) return boost::any_cast<bool>(prefs[keypath]);
	return fallback;
}

void set_pref(std::string keypath, int value) {
	SET_PREF_MAC(int);
	prefsDirty = true;
	prefs[keypath] = value;
}

int get_int_pref(std::string keypath, int fallback) {
	GET_PREF_MAC(int);
	if(prefs.find(keypath) == prefs.end()) return fallback;
	if(prefs[keypath].type() == typeid(int)) return boost::any_cast<int>(prefs[keypath]);
	return fallback;
}

void set_pref(std::string keypath, double value) {
	SET_PREF_MAC(double);
	prefsDirty = true;
	prefs[keypath] = value;
}

double get_float_pref(std::string keypath, double fallback) {
	GET_PREF_MAC(double);
	if(prefs.find(keypath) == prefs.end()) return fallback;
	if(prefs[keypath].type() == typeid(double)) return boost::any_cast<double>(prefs[keypath]);
	return fallback;
}

void append_iarray_pref(std::string keypath, int value) {
	#ifdef SFML_SYSTEM_MACOS
	void append_iarray_pref_mac(std::string keypath, int value);
	if(!was_replaying){
		append_iarray_pref_mac(keypath, value);
	}
	#endif
	prefsDirty = true;
	if(prefs.find(keypath) == prefs.end() || prefs[keypath].type() != typeid(iarray))
		prefs[keypath] = iarray{value};
	else {
		iarray& arr = boost::any_cast<iarray&>(prefs[keypath]);
		arr.push_back(value);
		prefs[keypath] = arr;
	}
}

std::vector<int> get_iarray_pref(std::string keypath) {
	#ifdef SFML_SYSTEM_MACOS
	std::vector<int> get_iarray_pref_mac(std::string keypath);
	if(!was_replaying){
		return get_iarray_pref_mac(keypath);
	}
	#endif
	if(prefs.find(keypath) == prefs.end()) return {};
	if(prefs[keypath].type() == typeid(iarray)) return boost::any_cast<iarray&>(prefs[keypath]);
	return {};
}

void set_pref(std::string keypath, std::string value) {
	SET_PREF_MAC(std::string);
	prefsDirty = true;
	prefs[keypath] = value;
}

std::string get_string_pref(std::string keypath, std::string fallback) {
	using std::string;
	GET_PREF_MAC(string);
	if(prefs.find(keypath) == prefs.end()) return fallback;
	if(prefs[keypath].type() == typeid(std::string)) return boost::any_cast<std::string>(prefs[keypath]);
	return fallback;
}

void clear_pref(std::string keypath) {
	#ifdef SFML_SYSTEM_MACOS
	void clear_pref_mac(std::string keypath);
	if(!was_replaying){
		clear_pref_mac(keypath);
		return;
	}
	#endif
	prefsDirty = true;
	auto iter = prefs.find(keypath);
	if(iter != prefs.end()) prefs.erase(iter);
}

static bool save_prefs(fs::path fpath) {
	// Don't save preferences when running a replay which might change them
	if(replaying) return true;

	if(!prefsDirty) return true;
	fs::create_directories(fpath.parent_path());
	std::ofstream fout(fpath.string().c_str());
	for(auto& kv : prefs) {
		if(kv.second.type() == typeid(iarray)) {
			const iarray& arr = boost::any_cast<iarray&>(kv.second);
			fout << kv.first << " = [";
			for(int i : arr) fout << i << ' ';
			fout.seekp(-1,std::ios::cur); // To overwrite the final space written in the loop
			fout << ']' << std::endl;
		} else if(kv.second.type() == typeid(bool))
			fout << kv.first << " = " << std::boolalpha << boost::any_cast<bool>(kv.second) << std::endl;
		else if(kv.second.type() == typeid(int))
			fout << kv.first << " = " << boost::any_cast<int>(kv.second) << std::endl;
		else if(kv.second.type() == typeid(double))
			fout << kv.first << " = " << std::fixed << boost::any_cast<double>(kv.second) << std::endl;
		else if(kv.second.type() == typeid(std::string)){
			std::string value = boost::any_cast<std::string>(kv.second);
			fout << kv.first << " = " << maybe_quote_string(value, true) << std::endl;
		}else printf("Warning: Unknown preference value type, skipping...\n");
		if(!fout) {
			perror("Error writing preferences");
			return false;
		}
	}
	prefsDirty = false;
	return true;
}

static bool load_prefs(std::istream& in) {
	prefsDirty = false;
	std::map<std::string,boost::any> temp_prefs;
	std::string line;
	std::ostringstream prefs_recording;
	while(std::getline(in, line)) {
		if(!in) {
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
		if (recording) {
			prefs_recording << line << std::endl;
		}

		std::string key = line.substr(0, key_end + 1), val = line.substr(val_beg);
		if(val == "true") temp_prefs[key] = true;
		else if(val == "false") temp_prefs[key] = false;
		else if(val[0] == '"' || val[0] == '\'') {
			std::istringstream str_val(val);
			std::string value = read_maybe_quoted_string(str_val);
			temp_prefs[key] = value;
		} else if(val[0] == '[') {
			int arr_end = val.find_first_of(']');
			std::istringstream arr_vals(val.substr(1, arr_end - 1));
			int i = 0;
			iarray vals;
			while(arr_vals >> i) vals.push_back(i);
			temp_prefs[key] = vals;
		} else if(val.find_first_of('.') != std::string::npos) {
			temp_prefs[key] = boost::lexical_cast<double>(val);
		} else {
			temp_prefs[key] = boost::lexical_cast<int>(val);
		}
	}
	prefs.swap(temp_prefs);

	if (recording) {
		record_action("load_prefs", prefs_recording.str(), true);
	}

	return prefsLoaded = true;
}

static bool load_prefs(fs::path fpath) {
	// When replaying a BoE session, disregard the system settings and use the ones
	// that were recorded
	if (replaying) {
		Element& prefs_action = pop_next_action("load_prefs");
		std::istringstream in(prefs_action.GetText());
		return load_prefs(in);
	} else {
		std::ifstream in(fpath.c_str());
		return load_prefs(in);
	}
}

extern fs::path tempDir;
bool sync_prefs() {
	#ifdef SFML_SYSTEM_MACOS
	extern bool sync_prefs_mac();
	if(!was_replaying){
		return sync_prefs_mac();
	}
	#endif
	fs::path prefsPath = tempDir.parent_path() / "bladesprefs.ini";
	if(prefsLoaded) return save_prefs(prefsPath);
	else return load_prefs(prefsPath);
}
