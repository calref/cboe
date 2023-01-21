/*
 *  mathutil.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#pragma once

#include <cmath>
#include <SFML/System/Time.hpp>

// Make sure min and max macros are not defined.
// Some Windows headers may define these.
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using std::abs;
short get_ran(short times, short min, short max);
short max(short a,short b);
short min(short a,short b);
short minmax(short min,short max,short k);
int percent(int value, int percentage);
short gcd(short a, short b);
sf::Time time_in_ticks(int ticks);

template<typename T>
inline void move_to_zero(T& val){
	if(val < 0)
		val++;
	if(val > 0)
		val--;
}

template<typename T>
inline T sgn(T val) {
	return val == 0 ? 0 : std::copysign(1, val);
}
