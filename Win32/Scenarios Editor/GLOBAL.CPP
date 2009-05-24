
#include <Windows.h>
#include "global.h"
#include "math.h"
#include <stdlib.h>

extern char scen_strs[160][256];
extern char scen_strs2[110][256];
extern char talk_strs[170][256];
extern char town_strs[180][256];
extern Boolean give_delays;
unsigned long anim_ticks = 0;
BOOL do_choose_anim = FALSE;
short custom_pic = 0;
BOOL play_anim=FALSE;

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

short s_pow(short x,short y)
{
	return (short) pow((double) x, (double) y);
}

short dist(location p1,location p2)
{
	return (short) sqrt((double)((p1.x - p2.x) * (p1.x - p2.x) +
							(p1.y - p2.y) * (p1.y - p2.y)));
}

short minmax(short min,short max,short k)
{
	if (k < min)
		return min;
	if (k > max)
		return max;
	return k;
}

short abs(short x)
{
	if (x < 0)
		return x * -1;
	return x;
}
void Delay(short val,long *dummy)
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

void beep()
{
	long dummy;

	MessageBeep(MB_OK);
	Delay(200,&dummy);

	}

void SysBeep(short)
{
	MessageBeep(MB_ICONQUESTION);
}
