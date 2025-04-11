//
//  living.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#include "living.hpp"

#include <algorithm>
#include <tuple>
#include "mathutil.hpp"

void iLiving::apply_status(eStatus which, int how_much) {
	if(!is_alive()) return;
	
	int lo, hi;
	std::tie(lo, hi) = status_bounds(which);
	
	if(which == eStatus::ASLEEP || which == eStatus::DUMB) {
		// No "wrapping" allowed for these effects.
		if(status[which] < 0) hi = 0;
		else if(status[which] >= 0) lo = 0;
	}
	
	status[which] = minmax(lo,hi,status[which] + how_much);
}

void iLiving::clear_bad_status() {
	std::map<eStatus, short> old;
	status.swap(old);
	std::remove_copy_if(old.begin(), old.end(), std::inserter(status, status.begin()), [](std::pair<const eStatus, short> kv) {
		return (*kv.first).isNegative ? kv.second > 0 : kv.second < 0;
	});
}

void iLiving::clear_brief_status() {
	std::map<eStatus, short> old;
	status.swap(old);
	std::remove_copy_if(old.begin(), old.end(), std::inserter(status, status.begin()), [](std::pair<const eStatus, short> kv) -> bool {
		if(kv.first == eStatus::POISON) return false;
		if(kv.first == eStatus::DISEASE) return false;
		if(kv.first == eStatus::DUMB) return false;
		if(kv.first == eStatus::ACID && kv.second > 2)
			return false;
		return true;
	});
}

void iLiving::void_sanctuary() {
	if(status[eStatus::INVISIBLE] > 0)
		status[eStatus::INVISIBLE] = 0;
}

iLiving::iLiving() {}

iLiving::iLiving(const iLiving& other)
	: status(other.status)
	, ap(other.ap)
	, direction(other.direction)
	, marked_damage(other.marked_damage)
{}

void(* iLiving::print_result)(std::string) = nullptr;
