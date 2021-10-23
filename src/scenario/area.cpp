//
//  area.cpp
//  Common
//
//  Created by alonso on 22/10/2021.
//

#include "area.hpp"

static info_rect_t getBadAreaDesc()
{
	info_rect_t bad_area={-1,-1,-1,-1,"Bad Aread Desc"};
	return bad_area;
}
info_rect_t const &cArea::get_area_desc(int num) const
{
	if (num>=0 && num<area_desc.size())
		return area_desc[num];
	static info_rect_t bad_area=getBadAreaDesc();
	return bad_area;
}
info_rect_t &cArea::get_area_desc(int num)
{
	if (num>=0 && num<area_desc.size())
		return area_desc[num];
	static info_rect_t bad_area;
	bad_area=getBadAreaDesc();
	return bad_area;
}

static sign_loc_t getBadSignLoc()
{
	return sign_loc_t(-1,-1,"Bad Sign");
}
sign_loc_t const &cArea::get_sign_loc(int num) const
{
	if (num>=0 && num<sign_locs.size())
		return sign_locs[num];
	static sign_loc_t bad_sign=getBadSignLoc();
	return bad_sign;
}
sign_loc_t &cArea::get_sign_loc(int num)
{
	if (num>=0 && num<sign_locs.size())
		return sign_locs[num];
	static sign_loc_t bad_sign;
	bad_sign=getBadSignLoc();
	return bad_sign;
}
