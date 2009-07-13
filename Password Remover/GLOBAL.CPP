#include "global.h"
#include "math.h"
#include <stdlib.h>

RECT RECT16::rect32()
{
	RECT tmp;
	tmp.left = left;
	tmp.top = top;
	tmp.right = right;
	tmp.bottom = bottom;
	return tmp;
}

RECT16::RECT16 (const RECT & tmp)
{
	left = tmp.left;
	top = tmp.top;
	right = tmp.right;
	bottom = tmp.bottom;
}
	
RECT16 & RECT16::operator=(const RECT & tmp)
{
	left = tmp.left;
	top = tmp.top;
	right = tmp.right;
	bottom = tmp.bottom;
	return (*this);
}

short get_ran (short times,short  min,short  max)
{
	short store;
	short i, to_ret = 0;

	if ((max - min + 1) == 0)
		return 0;
	for (i = 1; i < times + 1; i++) {
		store = rand() % (max - min + 1);
		to_ret = to_ret + min + store;
		}
	return to_ret;
}

short s_pow(short x,short y)
{
	return (short) pow((double) x, (double) y);
}
