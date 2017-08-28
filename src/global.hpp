/*
 *  global.hpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 17-04-13.
 *
 */

#ifndef BOE_PCH
#define BOE_PCH

typedef unsigned short mon_num_t;
typedef signed short miss_num_t;
typedef unsigned short ter_num_t;
typedef signed short spec_num_t;
typedef signed short item_num_t;
typedef unsigned short str_num_t;
typedef signed short pic_num_t;

// OBoE Current Version
const unsigned long long OBOE_CURRENT_VERSION = 0x020000; // MMmmff; M - major, m - minor, f - bugfix
const char* oboeVersionString();

// A convenient alias
namespace boost { namespace filesystem {}}
namespace fs = boost::filesystem;

#endif
