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
short minmax(short min,short max,short k);
short move_to_zero(short val);
short gcd(short a, short b);
