
#include <Carbon/Carbon.h>
#include "global.h"
#include "math.h"

short get_ran (short times,short  min,short  max)
{
	long int store;
	short i, to_ret = 0;
	
	for (i = 1; i < times + 1; i++) {
		store = Random();
		to_ret = to_ret + min + (((store + 32767) * (max - min + 1)) / 65536);
		}
	return to_ret;
}

short s_pow(short x,short y)
{
	return (short) pow((double) x, (double) y);
}

short dist(location p1,location p2)
{
	return (short) sqrt((double)((p1.x - p2.x) * (p1.x - p2.x) +
							(p1.y - p2.y) * (p1.y - p2.y)));
}

short max(short a,short b)
{
	if (a > b)
		return a;
		else return b;
}

short min(short a,short b)
{
	if (a < b)
		return a;
		else return b;
}

short minmax(short min,short max,short k)
{
	if (k < min)
		return min;
	if (k > max)
		return max;
	return k;
}
/*
short abs(short x)
{
	if (x < 0)
		return x * -1;
	return x;
}
*/