//#include <OSUtils.h>
#include <Carbon/Carbon.h>
#include "ed.global.h"
#include "math.h"

extern short give_delays;

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

Boolean same_point(location p1,location p2)
{
	if ((p1.x == p2.x) & (p1.y == p2.y))
		return TRUE;
		else return FALSE;
}

short move_to_zero(short val)
{
	if (val < 0)
		return val + 1;
	if (val > 0)
		return val - 1;
	return val;
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

short s_pow(short x,short y)
{
	return (short) pow((double) x, (double) y);
}

short a_v(short x)
{
	if (x < 0)
		return (-1 * x);
		else return x;
}
short ex_abs(short x)
{
	if (x < 0)
		return (-1 * x);
		else return x;
}

void pause(short length)
{
	unsigned long dummy,len;
	
	len = (unsigned long)length;
	
	if (give_delays == 0)
		Delay(len, &dummy);
}