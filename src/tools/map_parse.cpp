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
#include <cstring>

using namespace std;

map_data load_map(std::istream& fin, bool isTown, std::string name) {
	map_data data;
	data.file = name;
	int row = 0;
	while(fin) {
		std::string line;
		getline(fin, line);
		if(line.empty() && !fin.eof())
			continue;
		int n = 0, col = 0;
		eMapFeature curFeature = eMapFeature::NONE;
		// vehicle_owned = true means the party owns it
		bool vehicle_owned;
		for(char c : line) {
			if(c == '#') break; // Found a comment
			if(c == ' ' || c == '\t') continue;
			if(isdigit(c)) {
				n *= 10;
				n += c - '0';
			} else if(c == '^') {
				if(isTown)
					data.addFeature(col, row, eMapFeature::ENTRANCE_NORTH);
				else throw xMapParseError(map_out_has_town_dir, c, row, col, name);
			} else if(c == '<') {
				if(isTown)
					data.addFeature(col, row, eMapFeature::ENTRANCE_WEST);
				else throw xMapParseError(map_out_has_town_dir, c, row, col, name);
			} else if(c == 'v') {
				if(isTown)
					data.addFeature(col, row, eMapFeature::ENTRANCE_SOUTH);
				else throw xMapParseError(map_out_has_town_dir, c, row, col, name);
			} else if(c == '>') {
				if(isTown)
					data.addFeature(col, row, eMapFeature::ENTRANCE_EAST);
				else throw xMapParseError(map_out_has_town_dir, c, row, col, name);
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
				} else if(c == '&') {
					curFeature = eMapFeature::FIELD;
				} else if(c == '$') {
					curFeature = eMapFeature::CREATURE;
				} else if(c == 'h') {
					vehicle_owned = true;
					curFeature = eMapFeature::HORSE;
				} else if(c == 'H') {
					vehicle_owned = false;
					curFeature = eMapFeature::HORSE;
				} else if(c == 'b') {
					vehicle_owned = true;
					curFeature = eMapFeature::BOAT;
				} else if(c == 'B') {
					vehicle_owned = false;
					curFeature = eMapFeature::BOAT;
				} else if(c == ',') {
					col++;
					curFeature = eMapFeature::NONE;
				} else {
					throw xMapParseError(map_bad_feature, c, row, col, name);
				}
			}
		}
		if(curFeature == eMapFeature::NONE)
			data.set(col, row, n);
		else {
			if((curFeature == eMapFeature::BOAT || curFeature == eMapFeature::HORSE) && !vehicle_owned)
				n *= -1;
			data.addFeature(col, row, curFeature, n);
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
					case eMapFeature::NONE: break;
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
					case eMapFeature::BOAT: out << (feat.second > 0 ? 'b' : 'B') << abs(feat.second); break;
					case eMapFeature::HORSE: out << (feat.second > 0 ? 'h' : 'H') << abs(feat.second); break;
				}
			}
		}
		out << '\n';
	}
}

const char*const xMapParseError::messages[NUM_MAP_ERR] = {
	"Unrecognized map feature character found: ",
	"Outdoors map has illegal town entrance direction feature: ",
	"Outdoors map has illegal field type: ",
};

xMapParseError::xMapParseError(eMapError err, char c, int line, int col, std::string file) :
	err(err),
	c(c),
	line(line),
	col(col),
	file(file),
	msg(nullptr) {}

xMapParseError::~xMapParseError() throw() {
	if(msg != nullptr)
		delete[] msg;
}

const char* xMapParseError::what() const throw() {
	if(msg == nullptr) {
		char* s = new(std::nothrow) char[201];
		std::fill_n(s, 201, 0);
		size_t len = strlen(messages[err]);
		strncpy(s, messages[err], std::min<int>(201, len + 1));
		if(err == map_out_bad_field) {
			if(c < 100 && len < 200)
				s[len++] = c / 100;
			if(c < 10 && len < 200)
				s[len++] = c / 10;
			if(len < 200)
				s[len++] = c % 10;
		} else s[len++] = c;
		s[len] = 0;
		msg = s;
	}
	return msg;
}
