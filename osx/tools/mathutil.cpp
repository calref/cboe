/*
 *  mathutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#include <cstdlib>
#include "mathutil.h"

short get_ran (short times,short  min,short  max){
	long int store;
	short i, to_ret = 0;
	
	if(max < min) max = min;
	
	for (i = 1; i < times + 1; i++) {
		store = rand();
		to_ret += min + (store % (max - min + 1));//min + (((store + 32767) * (max - min + 1)) / 65536);
	}
	return to_ret;
}

short s_pow(short x,short y){
	return (short) pow((double) x, (double) y);
}

short s_sqrt(short val)
{
	return (short) sqrt((double)(val));
}

short max(short a,short b){
	if (a > b)
		return a;
	else return b;
}

short min(short a,short b){
	if (a < b)
		return a;
	else return b;
}

short minmax(short min,short max,short k){
	if (k < min)
		return min;
	if (k > max)
		return max;
	return k;
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

	
