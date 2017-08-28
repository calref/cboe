//
//  living.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#include "living.hpp"

#include <set>
#include <algorithm>
#include "mathutil.hpp"

void iLiving::apply_status(eStatus which, int how_much) {
	if(!is_alive()) return;
	
	static const std::set<eStatus> allow_negative = {
		// The obvious ones:
		eStatus::BLESS_CURSE, eStatus::HASTE_SLOW,
		// The ones that BoE previously allowed:
		eStatus::POISONED_WEAPON, eStatus::POISON, eStatus::ASLEEP,
		// (Note: Negative levels of sleep can be obtained from the Hyperactivity spell. The other two never go negative.)
		// The additional ones that make sense in the negative:
		eStatus::MAGIC_RESISTANCE, eStatus::DUMB,
	};
	
	int lo = 0, hi = 8;
	
	if(which == eStatus::MARTYRS_SHIELD)
		hi = 10;
	else if(which == eStatus::PARALYZED)
		hi = 5000;
	else if(which == eStatus::FORCECAGE)
		hi = 1000;
	
	if(allow_negative.count(which))
		lo = -hi;
	
	if(which == eStatus::ASLEEP || which == eStatus::DUMB) {
		// No "wrapping" allowed for these effects.
		if(status[which] < 0) hi = 0;
		else if(status[which] > 0) lo = 0;
	}
	
	status[which] = minmax(lo,hi,status[which] + how_much);
}

void iLiving::clear_bad_status() {
	std::map<eStatus, short> old;
	status.swap(old);
	std::remove_copy_if(old.begin(), old.end(), std::inserter(status, status.begin()), [](std::pair<const eStatus, short> kv) {
		return isStatusNegative(kv.first) ? kv.second > 0 : kv.second < 0;
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

void(* iLiving::print_result)(std::string) = nullptr;
