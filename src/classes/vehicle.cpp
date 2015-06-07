/*
 *  vehicle.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#include "vehicle.hpp"

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "oldstructs.hpp"

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

void cVehicle::append(legacy::horse_record_type& old){
	which_town = old.which_town;
	exists = old.exists;
	property = old.property;
	loc.x = old.horse_loc.x;
	loc.y = old.horse_loc.y;
	loc_in_sec.x = old.horse_loc_in_sec.x;
	loc_in_sec.y = old.horse_loc_in_sec.y;
	sector.x = old.horse_sector.x;
	sector.y = old.horse_sector.y;
}

void cVehicle::append(legacy::boat_record_type& old){
	which_town = old.which_town;
	exists = old.exists;
	property = old.property;
	loc.x = old.boat_loc.x;
	loc.y = old.boat_loc.y;
	loc_in_sec.x = old.boat_loc_in_sec.x;
	loc_in_sec.y = old.boat_loc_in_sec.y;
	sector.x = old.boat_sector.x;
	sector.y = old.boat_sector.y;
}

void cVehicle::writeTo(std::ostream& file) const {
	file << "LOCATION " << loc.x << ' ' << loc.y << '\n';
	file << "LOCINSECTOR " << loc_in_sec.x << ' ' << loc_in_sec.y << '\n';
	file << "SECTOR " << sector.x << ' ' << sector.y << '\n';
	file << "IN " << which_town << '\n';
	if(property) file << "OWNED\n";
}

void cVehicle::readFrom(std::istream& file) {
	while(file) {
		std::string cur;
		getline(file, cur);
		std::istringstream lineIn(cur);
		lineIn >> cur;
		if(cur == "LOCATION")
			lineIn >> loc.x >> loc.y;
		else if(cur == "LOCINSECTOR")
			lineIn >> loc_in_sec.x >> loc_in_sec.y;
		else if(cur == "SECTOR")
			lineIn >> sector.x >> sector.y;
		else if(cur == "IN")
			lineIn >> which_town;
		else if(cur == "OWNED")
			property = true;
	}
}
