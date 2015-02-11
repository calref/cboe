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
#include <iterator>
#include <numeric>

using namespace std;

map_data load_map(std::istream& fin, bool isTown) {
	map_data data;
	int row = 0;
	while(!fin.eof()) {
		std::string line;
		getline(fin, line);
		int n = 0, col = 0;
		eMapFeature curFeature = eMapFeature::NONE;
		// vehicle_owned = true means the party owns it
		bool vehicle_owned, found_something;
		for(char c : line) {
			if(c == '#') break; // Found a comment
			if(isblank(c)) continue;
			found_something = true;
			if(isdigit(c)) {
				n *= 10;
				n += c - '0';
			} else if(c == '^' && isTown) {
				data.addFeature(col, row, eMapFeature::ENTRANCE_NORTH);
			} else if(c == '<' && isTown) {
				data.addFeature(col, row, eMapFeature::ENTRANCE_WEST);
			} else if(c == 'v' && isTown) {
				data.addFeature(col, row, eMapFeature::ENTRANCE_SOUTH);
			} else if(c == '>' && isTown) {
				data.addFeature(col, row, eMapFeature::ENTRANCE_EAST);
			} else {
				if(curFeature == eMapFeature::NONE)
					data.set(col, row, n);
				else {
					if((curFeature == eMapFeature::BOAT || curFeature == eMapFeature::HORSE) && !vehicle_owned)
						n += 10000;
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
				} else if(c == '&') {
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
		row++;
	}
	return data;
}

void map_data::set(unsigned int x, unsigned int y, unsigned int val) {
	// First make sure the location exists
	if(y >= grid.size())
		grid.resize(y + 1);
	if(x >= grid[y].size())
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

auto map_data::getFeatures(unsigned int x, unsigned int y) -> std::vector<feature_t> {
	std::vector<feature_t> ls;
	if(y >= grid.size()) return ls;
	if(x >= grid[y].size()) return ls;
	using iter_t = decltype(features)::const_iterator;
	std::pair<iter_t,iter_t> range = features.equal_range(loc(x,y));
	if(range.first == range.second) return ls;
	for(auto iter = range.first; iter != range.second; iter++)
		ls.push_back(iter->second);
	return ls;
}

void map_data::writeTo(std::ostream& out) {
	unsigned int height = grid.size();
	unsigned int width = std::accumulate(grid.begin(), grid.end(), 0, [](size_t m, std::vector<int>& v){
		return std::max(m, v.size());
	});
	for(unsigned int y = 0; y < height; y++) {
		bool first = true;
		for(unsigned int x = 0; x < width; x++) {
			if(!first) out << ',';
			first = false;
			out << grid[y][x];
			for(auto feat : getFeatures(x,y)) {
				switch(feat.first) {
					case eMapFeature::NONE: case eMapFeature::VEHICLE: break;
					case eMapFeature::SPECIAL_NODE: out << ':' << feat.second; break;
					case eMapFeature::SIGN: out << '!' << feat.second; break;
					case eMapFeature::WANDERING: out << '*' << feat.second; break;
					case eMapFeature::TOWN: case eMapFeature::ITEM: out << '@' << feat.second; break;
					case eMapFeature::CREATURE: out << '$' << feat.second; break;
					case eMapFeature::FIELD: out << '&' << feat.second; break;
					case eMapFeature::ENTRANCE_EAST: out << '>'; break;
					case eMapFeature::ENTRANCE_NORTH: out << '^'; break;
					case eMapFeature::ENTRANCE_SOUTH: out << 'v'; break;
					case eMapFeature::ENTRANCE_WEST: out << '<'; break;
					case eMapFeature::BOAT: out << (feat.second < 0 ? 'b' : 'B') << abs(feat.second); break;
					case eMapFeature::HORSE: out << (feat.second < 0 ? 'h' : 'H') << abs(feat.second); break;
				}
			}
		}
		out << '\n';
	}
}

bool loc_compare::operator()(location a, location b) const {
	// This is just a lexicographical ordering.
	if(a.x != b.x) return a.x < b.x;
	if(a.y != b.y) return a.y < b.y;
	return false;
}
