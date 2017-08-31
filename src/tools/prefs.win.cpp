
#include "prefs.hpp"
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

void set_pref(std::string keypath, bool value) {
	prefsDirty = true;
	prefs[keypath] = value;
}

bool get_bool_pref(std::string keypath, bool fallback) {
	if(prefs.find(keypath) == prefs.end()) return fallback;
	if(prefs[keypath].type() == typeid(bool)) return boost::any_cast<bool>(prefs[keypath]);
	return fallback;
}

void set_pref(std::string keypath, int value) {
	prefsDirty = true;
	prefs[keypath] = value;
}

int get_int_pref(std::string keypath, int fallback) {
	if(prefs.find(keypath) == prefs.end()) return fallback;
	if(prefs[keypath].type() == typeid(int)) return boost::any_cast<int>(prefs[keypath]);
	return fallback;
}

void append_iarray_pref(std::string keypath, int value) {
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
	if(prefs.find(keypath) == prefs.end()) return {};
	if(prefs[keypath].type() == typeid(iarray)) return boost::any_cast<iarray&>(prefs[keypath]);
	return {};
}

void clear_pref(std::string keypath) {
	prefsDirty = true;
	auto iter = prefs.find(keypath);
	if(iter != prefs.end()) prefs.erase(iter);
}

static bool save_prefs(fs::path fpath) {
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
		else printf("Warning: Unknown preference value type, skipping...\n");
		if(!fout) {
			perror("Error writing preferences");
			return false;
		}
	}
	prefsDirty = false;
	return true;
}

static bool load_prefs(fs::path fpath) {
	prefsDirty = false;
	std::map<std::string,boost::any> temp_prefs;
	std::ifstream fin(fpath.string().c_str());
	std::string line;
	while(std::getline(fin, line)) {
		if(!fin) {
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
		if(val == "true") temp_prefs[key] = true;
		else if(val == "false") temp_prefs[key] = false;
		else if(val[0] == '[') {
			int arr_end = val.find_first_of(']');
			std::istringstream arr_vals(val.substr(1, arr_end - 1));
			int i = 0;
			iarray vals;
			while(arr_vals >> i) vals.push_back(i);
			temp_prefs[key] = vals;
		} else temp_prefs[key] = boost::lexical_cast<int>(val);
	}
	prefs.swap(temp_prefs);
	return prefsLoaded = true;
}

extern fs::path tempDir;
bool sync_prefs() {
	fs::path prefsPath = tempDir.parent_path() / "bladesprefs.ini";
	if(prefsLoaded) return save_prefs(prefsPath);
	else return load_prefs(prefsPath);
}
