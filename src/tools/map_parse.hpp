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
#include <iosfwd>
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
	ITEM,
	CREATURE,
	FIELD,
};

struct loc_compare {
	bool operator()(location a, location b) const;
};

class map_data {
	std::vector<std::vector<int>> grid;
	using feature_t = std::pair<eMapFeature,int>;
	std::multimap<location,feature_t,loc_compare> features;
public:
	void set(unsigned int x, unsigned int y, unsigned int val);
	unsigned int get(unsigned int x, unsigned int y);
	void addFeature(unsigned int x, unsigned int y, eMapFeature feature, int val = 0);
	std::vector<feature_t> getFeatures(unsigned int x, unsigned int y);
	void writeTo(std::ostream& out);
};

map_data load_map(fs::path path, bool isTown);

#endif
