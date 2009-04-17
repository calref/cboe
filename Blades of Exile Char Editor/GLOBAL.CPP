#include <Windows.h>
#include "global.h"
#include "math.h"
#include <stdlib.h>

extern short give_delays;

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

POINT MAKEPOINT(LONG lParam)
{
	POINT point;
	
	point.x = LOWORD(lParam);
	point.y = HIWORD(lParam);
	
	return point;
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

/*
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
}*/

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

void Delay(short val,long *)
{
	long then,now,wait_val;

	wait_val = (long) val;
	wait_val = wait_val * 16;
	then = (long)GetCurrentTime();
	now = then;
	while (now - then < wait_val)   {
		now = (long) GetCurrentTime();
		}
}

void pause(short length)
{
	long dummy,len;
	
	len = (long) length;
	
	if (give_delays == 0)
		Delay(len, &dummy);
}

void beep()
{
	long dummy;

	MessageBeep(MB_OK);
   Delay(30,&dummy);
	}

void SysBeep(short)
{
	MessageBeep(MB_OK);
}
