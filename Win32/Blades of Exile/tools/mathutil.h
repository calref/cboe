/*
 *  mathutil.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#include <cmath>
using std::abs;

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

extern short get_ran (short times,short  min,short  max);
extern short s_pow(short x,short y);
short s_sqrt(short val);
short max(short a,short b);
short min(short a,short b);
short gcd(short a, short b);

template <class T>
void move_to_zero(T & value)
{
	if (value > 0) --value;
	else if (value < 0)	++value;
};

template <class T>
T minmax(T min, T max, T k)
{
	return (k < min)? min : (k > max)? max : k;
};

template <class T>
T ex_abs(T value)
{
	return (value < 0)? (-value) : value;
};
