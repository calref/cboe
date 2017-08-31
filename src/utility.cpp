//
//  utility.cpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-14.
//
//

#include "utility.hpp"

#include "res_strings.hpp"

std::string get_str(std::string list, short j){
	if(j == 0) return list;
	StringRsrc& strings = *ResMgr::get<StringRsrc>(list);
	return strings[j - 1];
}

short can_see(location p1,location p2,std::function<short(short,short)> get_obscurity) {
	short storage = 0;
	
	if(p1.y == p2.y) {
		if(p1.x > p2.x) {
			for(short count = p2.x + 1; count < p1.x; count++)
				storage += get_obscurity(count, p1.y);
		} else {
			for(short count = p1.x + 1; count < p2.x; count++)
				storage += get_obscurity(count, p1.y);
		}
	} else if(p1.x == p2.x) {
		if(p1.y > p2.y) {
			for(short count = p1.y - 1; count > p2.y; count--)
				storage += get_obscurity(p1.x, count);
		} else {
			for(short count = p1.y + 1; count < p2.y; count++)
				storage += get_obscurity(p1.x, count);
		}
	} else {
		short dx = p2.x - p1.x;
		short dy = p2.y - p1.y;
		
		if(abs(dy) > abs(dx)) {
			if(p2.y > p1.y) {
				for(short count = 1; count < dy; count++)
					storage += get_obscurity(p1.x + (count * dx) / dy, p1.y + count);
			} else {
				for(short count = -1; count > dy; count--)
					storage += get_obscurity(p1.x + (count * dx) / dy, p1.y + count);
			}
		}
		if(abs(dy) <= abs(dx)) {
			if(p2.x > p1.x) {
				for(short count = 1; count < dx; count++)
					storage += get_obscurity(p1.x + count, p1.y + (count * dy) / dx);
			} else {
				for(short count = -1; count > dx; count--)
					storage += get_obscurity(p1.x + count, p1.y + (count * dy) / dx);
			}
		}
	}
	return storage;
}
