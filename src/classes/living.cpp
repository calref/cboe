//
//  living.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#include "living.hpp"

#include <set>
#include "mathutil.hpp"

void iLiving::apply_status(eStatus which, int how_much) {
	static const std::set<eStatus> allow_negative = {
		// The obvious ones:
		eStatus::BLESS_CURSE, eStatus::HASTE_SLOW,
		// The ones that BoE previously allowed:
		eStatus::POISONED_WEAPON, eStatus::POISON, eStatus::ASLEEP,
		// (Note: Negative levels of sleep can be obtained from the Hyperactivity spell. The other two never go negative.)
		// The additional ones that make sense in the negative:
		eStatus::MAGIC_RESISTANCE, eStatus::DUMB,
	};
	
	// TODO: Martyr's Shield range seems to be 0..10; Paralyzed seems to range something like 0..1000
	
	if(!is_alive()) return;
	status[which] = minmax(-8,8,status[which] + how_much);
	if(!allow_negative.count(which))
		status[which] = max(status[which],0);
}

void iLiving::clear_bad_status() {
	status[eStatus::POISON] = 0;
	if(status[eStatus::BLESS_CURSE] < 0)
		status[eStatus::BLESS_CURSE] = 0;
	if(status[eStatus::HASTE_SLOW] < 0)
		status[eStatus::HASTE_SLOW] = 0;
	status[eStatus::WEBS] = 0;
	status[eStatus::DISEASE] = 0;
	if(status[eStatus::DUMB] > 0)
		status[eStatus::DUMB] = 0;
	if(status[eStatus::ASLEEP] > 0)
		status[eStatus::ASLEEP] = 0;
	status[eStatus::PARALYZED] = 0;
	status[eStatus::ACID] = 0;
	if(status[eStatus::MAGIC_RESISTANCE] < 0)
		status[eStatus::MAGIC_RESISTANCE] = 0;
}

void(* iLiving::print_result)(std::string) = nullptr;
