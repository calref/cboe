/*
 *  classes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 * This file simply includes all the classes in an order that satisfies all dependencies.
 *
 */

typedef unsigned short str_num_t;

//class sbyte {
//	signed char c;
//public:
//	operator int() {return c;}
//	sbyte(signed char k) : c(k) {}
//}

#include "location.h"
#include "terrain.h"
#include "vehicle.h"
#include "item.h"
#include "special.h"
#include "monster.h"
#include "talking.h"
#include "town.h"
#include "regtown.h"
#include "tmpltown.h"
#include "outdoors.h"
#include "scenario.h"
#include "pc.h"
#include "creatlist.h"
#include "party.h"
#include "universe.h"
