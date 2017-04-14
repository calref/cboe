/*
 *  mathutil.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#include <cmath>
#include <SFML/System/Time.hpp>

using std::abs;
short get_ran(short times, short min, short max);
short max(short a,short b);
short min(short a,short b);
short minmax(short min,short max,short k);
short gcd(short a, short b);
sf::Time time_in_ticks(int ticks);

template<typename T>
inline void move_to_zero(T& val){
	if(val < 0)
		val++;
	if(val > 0)
		val--;
}
