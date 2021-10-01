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
	loc(0,0),
	sector(0,0),
	which_town(-1),
	property(false)
{}

void cVehicle::import_legacy(legacy::horse_record_type const &old){
	which_town = old.which_town;
	exists = old.exists;
	property = old.property;
	if(which_town < 200) {
		loc.x = old.horse_loc.x;
		loc.y = old.horse_loc.y;
	} else {
		loc.x = old.horse_loc_in_sec.x;
		loc.y = old.horse_loc_in_sec.y;
	}
	sector.x = old.horse_sector.x;
	sector.y = old.horse_sector.y;
}

void cVehicle::import_legacy(legacy::boat_record_type const &old){
	which_town = old.which_town;
	exists = old.exists;
	property = old.property;
	if(which_town < 200) {
		loc.x = old.boat_loc.x;
		loc.y = old.boat_loc.y;
	} else {
		loc.x = old.boat_loc_in_sec.x;
		loc.y = old.boat_loc_in_sec.y;
	}
	sector.x = old.boat_sector.x;
	sector.y = old.boat_sector.y;
}

void cVehicle::writeTo(std::ostream& file) const {
	file << "LOCATION " << loc.x << ' ' << loc.y << '\n';
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
		else if(cur == "SECTOR")
			lineIn >> sector.x >> sector.y;
		else if(cur == "IN")
			lineIn >> which_town;
		else if(cur == "OWNED")
			property = true;
	}
}

bool operator==(const cVehicle& a, const cVehicle& b) {
	if(a.which_town != b.which_town)
		return false;
	if(a.exists != b.exists) return false;
	if(a.which_town == 200)
		return a.loc == b.loc && a.sector == b.sector && a.property == b.property;
	else return a.loc == b.loc && a.property == b.property;
}

bool operator!=(const cVehicle& a, const cVehicle& b) {
	return !(a == b);
}
