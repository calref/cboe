#include <stdlib.h>
#include "global.h"
#include "math.h"
#include "globvar.h"

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

void DebugQuit(char * msg)
{
	MessageBox(0, msg, "DebugQuit", MB_OK);
	PostQuitMessage(EXIT_FAILURE);
	PostQuitMessage(EXIT_FAILURE);
	PostQuitMessage(EXIT_FAILURE);
}

void alter_rect(RECT *r) 
{
	long a;

	a = r->top;
	r->top = r->left;
	r->left = a;
	a = r->bottom;
	r->bottom = r->right;
	r->right = a;
}

void alter_rect(RECT16 *r) 
{
	short a;

	a = r->top;
	r->top = r->left;
	r->left = a;
	a = r->bottom;
	r->bottom = r->right;
	r->right = a;
}

short get_ran (short times, short min, short max)
{
	short store;
	short i, to_ret = 0;

	if ((max - min + 1) == 0)
		return 0;
		
	for (i = 1; i < times + 1; i++)
	{
		store = rand() % (max - min + 1);
		to_ret = to_ret + min + store;
	}
	return to_ret;
}

Boolean same_point(location p1,location p2)
{
	if ((p1.x == p2.x) && (p1.y == p2.y))
		return TRUE;
		else return FALSE;
}

short s_pow(short x,short y)
{
	if (y == 0)
   	return 1;
	return (short) pow((double) x, (double) y);
}

void Delay(short val, long *)
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
	if (give_delays == 0) Delay(length, NULL);
}

// stuff done legit, i.e. flags are within proper ranges for stuff done flag
Boolean sd_legit(short a, short b)
{
	if ((minmax(0,299,(int)a) == a) && (minmax(0,9,(int)b) == b))
		return TRUE;
	return FALSE;
}
