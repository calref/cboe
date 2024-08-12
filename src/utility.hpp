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
#elif defined(__unix__)
#define strnicmp strncasecmp
#else
#error Missing strnicmp / strncasecmp
#endif

// Various parts of the code, like play_sound(), have used sf::sleep() to pause
// the game for long durations, but this causes the program to hang. event_sleep()
// accomplishes the same purpose by ignoring input events for the given duration,
// while allowing the game loop to continue through normal-length frames.
// Any instance where sf::sleep() could be called for more than a few frames,
// event_sleep() should be used instead.
void event_sleep(sf::Time time);
bool update_event_sleeping();

#endif
