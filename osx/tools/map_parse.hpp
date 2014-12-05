//
//  map_parse.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-04.
//
//

#ifndef BoE_map_parse_hpp
#define BoE_map_parse_hpp

#include <vector>
#include <map>
#include <boost/filesystem/path.hpp>
#include "location.h"

namespace fs = boost::filesystem;

enum class eMapFeature {
	NONE, // Special value, won't appear in the map.
	SPECIAL_NODE,
	SIGN,
	WANDERING,
	TOWN,
	VEHICLE, // Another special value that shouldn't appear in the map
	BOAT,
	HORSE,
	ENTRANCE_NORTH,
	ENTRANCE_WEST,
	ENTRANCE_SOUTH,
	ENTRANCE_EAST,
};

struct loc_compare {
	bool operator()(location a, location b) const;
};

struct map_data {
	std::vector<std::vector<int>> grid;
	std::multimap<location,std::pair<eMapFeature,int>,loc_compare> features;
	void set(unsigned int x, unsigned int y, unsigned int val);
	unsigned int get(unsigned int x, unsigned int y);
	void addFeature(unsigned int x, unsigned int y, eMapFeature feature, int val = 0);
};

map_data load_map(fs::path path);

#endif
