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
#include "location.hpp"

enum class eMapFeature {
	NONE, // Special value, won't appear in the map.
	SPECIAL_NODE,
	SIGN,
	WANDERING,
	TOWN,
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

class map_data {
	std::vector<std::vector<int>> grid;
	using feature_t = std::pair<eMapFeature,int>;
	std::multimap<location,feature_t,loc_compare> features;
public:
	std::string file;
	void set(unsigned int x, unsigned int y, unsigned int val);
	unsigned int get(unsigned int x, unsigned int y);
	void addFeature(unsigned int x, unsigned int y, eMapFeature feature, int val = 0);
	std::vector<feature_t> getFeatures(unsigned int x, unsigned int y);
	void writeTo(std::ostream& out);
};

map_data load_map(std::istream& stream, bool isTown, std::string name);

enum eMapError {
	map_bad_feature,
	map_out_has_town_dir,
	map_out_bad_field,
	NUM_MAP_ERR
};

class xMapParseError : public std::exception {
	static const char*const messages[NUM_MAP_ERR];
	int line, col;
	char c;
	std::string file;
	eMapError err;
	mutable const char* msg;
public:
	xMapParseError(eMapError err, char c, int line, int col, std::string file);
	~xMapParseError() throw();
	const char* what() const throw();
};

#endif
