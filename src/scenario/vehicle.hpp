/*
 *  vehicle.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef BOE_DATA_VEHICLE_H
#define BOE_DATA_VEHICLE_H

#include <iosfwd>
#include "location.hpp"

namespace legacy {
	struct horse_record_type;
	struct boat_record_type;
};

class cTagFile_Page;

class cVehicle {
public:
	// Both boats and horses use this type.
	// If they demand different member functions, I'll derive from this class.
	location loc;
	location sector;
	short which_town;
	bool exists;
	bool property;
	
	cVehicle();
	void import_legacy(legacy::horse_record_type& old);
	void import_legacy(legacy::boat_record_type& old);
	void writeTo(cTagFile_Page& page) const;
	void readFrom(const cTagFile_Page& page);
};

bool operator==(const cVehicle& a, const cVehicle& b);
bool operator!=(const cVehicle& a, const cVehicle& b);

#endif
