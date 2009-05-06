/*
 *  vehicle.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "location.h"

namespace legacy {
	struct horse_record_type;
	struct boat_record_type;
};

class cVehicle {
public:
	// Both boats and horses use this type.
	// If they demand different member functions, I'll derive from this class.
	location loc;
	location loc_in_sec;
	location sector;
	short which_town;
	bool exists;
	bool property;
	
	cVehicle();
	cVehicle& operator = (legacy::horse_record_type& old);
	cVehicle& operator = (legacy::boat_record_type& old);
	void writeTo(ostream& file);
};

/*
typedef struct {
	location horse_loc,horse_loc_in_sec,horse_sector;
	short which_town;
	bool exists,property;
} horse_record_type;
typedef struct {
	location boat_loc,boat_loc_in_sec,boat_sector;
	short which_town;
	bool exists,property;
} boat_record_type;
*/
