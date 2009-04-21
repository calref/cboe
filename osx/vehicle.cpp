/*
 *  vehicle.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "vehicle.h"

cVehicle::cVehicle() :
	//loc(0,0),
	//loc_in_sec(0,0),
	//sector(0,0),
	which_town(-1),
	exists(false),
	property(false) {
	// do nothing
		loc.x = 0; loc.y = 0;
		loc_in_sec.x = 0; loc_in_sec.y = 0;
		sector.x = 0; sector.y = 0;
}