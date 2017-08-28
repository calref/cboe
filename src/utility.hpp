//
//  utility.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#ifndef BoE_UTILITY_HPP
#define BoE_UTILITY_HPP

#include <string>
#include <functional>
#include "location.hpp"

short can_see(location p1,location p2,std::function<short(short,short)> get_obscurity);
std::string get_str(std::string list, short j);

// Case-insensitive string comparison seems to be semi-standard, but with different names.
#if defined(__APPLE__)
#define strnicmp strncasecmp
#elif defined(_WIN32)
#define strnicmp _strnicmp
#else
#error Missing strnicmp / strncasecmp
#endif

#endif
