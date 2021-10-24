//
//  area.cpp
//  Common
//
//  Created by alonso on 22/10/2021.
//

#include "area.hpp"

info_rect_t const &cArea::get_area_desc(int num) const
{
	if (num>=0 && num<area_desc.size())
		return area_desc[num];
	static info_rect_t bad_area=info_rect_t::bad();
	return bad_area;
}
info_rect_t &cArea::get_area_desc(int num)
{
	if (num>=0 && num<area_desc.size())
		return area_desc[num];
	static info_rect_t bad_area;
	bad_area=info_rect_t::bad();
	return bad_area;
}

sign_loc_t const &cArea::get_sign_loc(int num) const
{
	if (num>=0 && num<sign_locs.size())
		return sign_locs[num];
	static sign_loc_t bad_sign=sign_loc_t::bad();
	return bad_sign;
}
sign_loc_t &cArea::get_sign_loc(int num)
{
	if (num>=0 && num<sign_locs.size())
		return sign_locs[num];
	static sign_loc_t bad_sign;
	bad_sign=sign_loc_t::bad();
	return bad_sign;
}

cSpecial const &cArea::get_special(int num) const
{
	if (num>=0 && num<specials.size())
		return specials[num];
	static cSpecial bad_special=cSpecial::bad();
	return bad_special;
}
cSpecial &cArea::get_special(int num)
{
	if (num>=0 && num<specials.size())
		return specials[num];
	static cSpecial bad_special;
	bad_special=cSpecial::bad();
	return bad_special;
}
