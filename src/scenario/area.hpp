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

	bool is_on_map(location const &loc) const {
		return loc.x < max_dim && loc.y < max_dim && loc.x >= 0 && loc.y >= 0;
	}
	bool is_on_map(short x, short y) const {
		return x < max_dim && y < max_dim && x >= 0 && y >= 0;
	}

	info_rect_t const &get_area_desc(int num) const;
	info_rect_t &get_area_desc(int num);
	sign_loc_t const &get_sign_loc(int num) const;
	sign_loc_t &get_sign_loc(int num);
};

#endif
