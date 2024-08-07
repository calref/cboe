/*
 *  mathutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#include <cstdlib>
#include "mathutil.hpp"

std::mt19937 game_rand;
std::mt19937 ui_rand;

short get_ran (short times,short  min,short  max){
	long int store;
	short to_ret = 0;
	
	if(max < min) max = min;
	if(max == min) return times * min;
	
	for(short i = 1; i < times + 1; i++) {
		store = game_rand();
		to_ret += min + (store % (max - min + 1));
	}
	return to_ret;
}

short max(short a,short b){
	if(a > b)
		return a;
	else return b;
}

short min(short a,short b){
	if(a < b)
		return a;
	else return b;
}

short minmax(short min,short max,short k){
	if(k < min)
		return min;
	if(k > max)
		return max;
	return k;
}

int percent(int value, int percentage) {
	// TODO: Any need to protect from overflow?
	return (value * percentage) / 100;
}

short gcd(short a, short b){ // Grabbed from Wikipedia and translated to C code
	short t;
	while(b != 0){
		t = b;
		b = a % b;
		a = t;
	}
	return a;
}

// According to Macintosh Toolbox Essentials, a tick is about 1/60ths of a second.
sf::Time time_in_ticks(int ticks) {
	unsigned long long micro = ticks;
	micro *= 1000;
	micro *= 1000;
	micro /= 60;
	return sf::microseconds(micro);
}


