//
//  map_parse.cpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-04.
//
//

#include "map_parse.hpp"

#include <fstream>
#include <cctype>

using namespace std;

map_data load_map(fs::path path, bool isTown) {
	map_data data;
	ifstream fin(path.string());
	int row = 0;
	while(!fin.eof()) {
		std::string line;
		getline(fin, line);
		int n = 0, col = 0;
		eMapFeature curFeature = eMapFeature::NONE;
		bool vehicle_owned;
		for(char c : line) {
			if(c == '#') break; // Found a comment
			if(isdigit(c)) {
				n *= 10;
				n += c - '0';
			} else if(isblank(c)) {
				continue;
			} else if(c == '^') {
				data.addFeature(col, row, eMapFeature::ENTRANCE_NORTH);
			} else if(c == '<') {
				data.addFeature(col, row, eMapFeature::ENTRANCE_WEST);
			} else if(c == 'v') {
				data.addFeature(col, row, eMapFeature::ENTRANCE_SOUTH);
			} else if(c == '>') {
				data.addFeature(col, row, eMapFeature::ENTRANCE_EAST);
			} else {
				if(curFeature == eMapFeature::NONE)
					data.set(col, row, n);
				else {
					if((curFeature == eMapFeature::BOAT || curFeature == eMapFeature::HORSE) && !vehicle_owned)
						n *= -1;
					data.addFeature(col, row, curFeature, n);
				}
				n = 0;
				if(c == '*') {
					curFeature = eMapFeature::WANDERING;
				} else if(c == ':') {
					curFeature = eMapFeature::SPECIAL_NODE;
				} else if(c == '!') {
					curFeature = eMapFeature::SIGN;
				} else if(c == '@' && !isTown) {
					curFeature = eMapFeature::TOWN;
				} else if(c == '@' && isTown) {
					curFeature = eMapFeature::ITEM;
				} else if(c == '&' && isTown) {
					curFeature = eMapFeature::FIELD;
				} else if(c == '$') {
					curFeature = eMapFeature::CREATURE;
				} else if(c == '~') {
					curFeature = eMapFeature::VEHICLE;
				} else if(c == 'h' && curFeature == eMapFeature::VEHICLE) {
					vehicle_owned = true;
					curFeature = eMapFeature::HORSE;
				} else if(c == 'H' && curFeature == eMapFeature::VEHICLE) {
					vehicle_owned = false;
					curFeature = eMapFeature::HORSE;
				} else if(c == 'b' && curFeature == eMapFeature::VEHICLE) {
					vehicle_owned = true;
					curFeature = eMapFeature::BOAT;
				} else if(c == 'B' && curFeature == eMapFeature::VEHICLE) {
					vehicle_owned = false;
					curFeature = eMapFeature::BOAT;
				} else if(c == ',') {
					col++;
					curFeature = eMapFeature::NONE;
				} else {
					// TODO: This is an error!
				}
			}
		}
	}
	return data;
}

void map_data::set(unsigned int x, unsigned int y, unsigned int val) {
	// First make sure the location exists
	if(y > grid.size())
		grid.resize(y + 1);
	if(x > grid[y].size())
		grid[y].resize(x + 1);
	grid[y][x] = val;
}

unsigned int map_data::get(unsigned int x, unsigned int y) {
	if(y >= grid.size()) return 0;
	if(x >= grid[y].size()) return 0;
	return grid[y][x];
}

void map_data::addFeature(unsigned int x, unsigned int y, eMapFeature feature, int val) {
	location loc(x,y);
	features.insert({loc,{feature,val}});
}

bool loc_compare::operator()(location a, location b) const {
	// This is just a lexicographical ordering.
	if(a.x != b.x) return a.x < b.x;
	if(a.y != b.y) return a.y < b.y;
	return false;
}
