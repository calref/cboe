//
//  damage.cpp
//  BoE
//
//  Created by Celtic Minstrel on 2023-08-27.
//
//

#include "damage.hpp"

static status_info_t status_info[int(eStatus::CHARM) + 1] = {
	{false, 4}, // POISONED_WEAPON
	{false, 2, 3}, // BLESS_CURSE
	{true, 0, -1, {1, 4}}, // POISON
	{false, 6, 8 // HASTE_SLOW
		//, {7, 0, 0} // uncomment to enable the "normal speed" icon
	},
	{false, 5}, // INVULNERABLE
	{false, 9, 19}, // MAGIC_RESISTANCE
	{true, 10}, // WEBS
	{true, 11}, // DISEASE
	{false, 12}, // INVISIBLE
	{true, 13, 18}, // DUMB
	{false, 14}, // MARTYRS_SHIELD
	{true, 15, 21}, // ASLEEP
	{true, 16}, // PARALYZED
	{true, 17}, // ACIDS
	{true, 20}, // FORCECAGE
	{true, 22}, // CHARM
};

static status_info_t party_status_info[int(ePartyStatus::FIREWALK) + 1] = {
	{false, 26}, // STEALTH
	{false, 23}, // FLIGHT
	{false, 24}, // DETECT_LIFE
	{false, 25}, // FIREWALK
};

status_info_t dummy_main_info{};

const status_info_t& operator* (eStatus status) {
	if(status == eStatus::MAIN) return dummy_main_info;
	return status_info[int(status)];
}

const status_info_t& operator* (ePartyStatus status) {
	return party_status_info[int(status)];
}
