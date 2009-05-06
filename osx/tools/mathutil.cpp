/*
 *  mathutil.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#include "mathutil.h"
#include <math.h>

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

short move_to_zero(short val){
	if (val < 0)
		return val + 1;
	if (val > 0)
		return val - 1;
	return val;
}
