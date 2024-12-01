/*
 *  global.hpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 17-04-13.
 *
 */

#ifndef BOE_PCH
#define BOE_PCH

#include <string>
#include <iostream>

typedef unsigned short mon_num_t;
typedef signed short miss_num_t;
typedef unsigned short ter_num_t;
typedef signed short spec_num_t;
typedef signed short item_num_t;
typedef unsigned short str_num_t;
typedef signed short pic_num_t;
typedef signed int snd_num_t;

// OBoE Current Version
const unsigned long long OBOE_CURRENT_VERSION = 0x020000; // MMmmff; M - major, m - minor, f - bugfix
const char* oboeVersionString();

// Window Resolutions
const short boe_width = 605, boe_height = 430;
const short pc_width = 590, pc_height = 440;
const short scen_width = 584, scen_height = 420;

// A convenient alias
namespace boost { namespace filesystem {}}
namespace fs = boost::filesystem;

inline bool str_to_bool(std::string str) {
	return str == "true";
}

inline std::string bool_to_str(bool b) {
	return b ? "true" : "false";
}

inline void LOG(std::string line) {
	std::cout << line << std::endl;
}

#define LOG_VALUE(x) std::cout << #x << ": " << x << std::endl;

#endif
