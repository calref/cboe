/*
 *  area.hpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 02/09/16.
 *
 */

#ifndef BOE_DATA_AREA_HPP
#define BOE_DATA_AREA_HPP

#include <vector>
#include <string>
#include <boost/dynamic_bitset.hpp>

#include "tools/vector2d.hpp"
#include "location.hpp"
#include "scenario/special.hpp"
#include "mathutil.hpp"

enum {
	AREA_TINY = 24,
	AREA_SMALL = 32,
	AREA_MEDIUM = 48,
	AREA_LARGE = 64,
	AREA_HUGE = 128,
};

// x of 100 used to be used to indicate an unused location
// or a place to temporarily move monsters out of the tile grid completely.
// I don't know why a negative number wasn't used instead?
// But in case there is a good reason for that, I'm shifting the placeholder
// value to a new, much larger, positive constant, that we *could* change again later.
const int LOC_UNUSED = AREA_HUGE * 2;

class cArea {
public:
	const size_t max_dim;
	vector2d<ter_num_t> terrain;
	std::vector<spec_loc_t> special_locs;
	std::vector<sign_loc_t> sign_locs;
	std::vector<info_rect_t> area_desc;
	std::vector<cSpecial> specials;
	std::string name;
	// Persistent data for saved games
	std::vector<boost::dynamic_bitset<>> maps;
	
	explicit cArea(size_t dim)
		: max_dim(dim)
		, terrain(dim, dim)
		, maps(dim, boost::dynamic_bitset<>(dim))
	{}

	bool is_on_map(location loc) const {
		return loc.x < max_dim && loc.y < max_dim && loc.x >= 0 && loc.y >= 0;
	}

	std::string loc_str(location where) {
		std::string str = name;
		for(info_rect_t rect : area_desc){
			if(!rect.empty() && rect.contains(where)){
				str += ": " + rect.descr;
				break;
			}
		}
		return str;
	}
};

#endif
