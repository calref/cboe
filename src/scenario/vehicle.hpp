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
	void import_legacy(legacy::horse_record_type const &old);
	void import_legacy(legacy::boat_record_type const &old);
	void writeTo(std::ostream& file) const;
	void readFrom(std::istream& file);
	
	static cVehicle bad() {
		cVehicle bad_vehicle;
		bad_vehicle.loc={-1,-1};
		bad_vehicle.exists=false;
		return bad_vehicle;
	}
};

bool operator==(const cVehicle& a, const cVehicle& b);
bool operator!=(const cVehicle& a, const cVehicle& b);

#endif
