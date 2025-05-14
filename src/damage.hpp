//
//  damage.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_DATA_DAMAGE_HPP
#define BoE_DATA_DAMAGE_HPP

#include <limits>
#include <boost/optional.hpp>
#include <set>

enum class eDamageType {
	WEAPON = 0,
	FIRE = 1,
	POISON = 2,
	MAGIC = 3,
	UNBLOCKABLE = 4,
	COLD = 5,
	UNDEAD = 6,
	DEMON = 7,
	ACID = 8,
	// Keep these two last
	SPECIAL = 9, // Completely unblockable damage from assassination skill
	MARKED = 10,
};

enum class eStatus {
	MAIN = -1, // For saved games only
	POISONED_WEAPON = 0,
	BLESS_CURSE = 1,
	POISON = 2,
	HASTE_SLOW = 3,
	INVULNERABLE = 4,
	MAGIC_RESISTANCE = 5,
	WEBS = 6,
	DISEASE = 7,
	INVISIBLE = 8, //sanctuary
	DUMB = 9,
	MARTYRS_SHIELD = 10,
	ASLEEP = 11,
	PARALYZED = 12,
	ACID = 13,
	FORCECAGE = 14,
	// This one is new
	// It's not quite a real status effect since it doesn't expire
	CHARM = 15,
};

enum class ePartyStatus {
	STEALTH,
	FLIGHT,
	DETECT_LIFE,
	FIREWALK,
};

struct status_info_t {
	bool isNegative = false;
	int icon = -1, negIcon = -1;
	struct special_icon_t {
		int icon, lo, hi;
		special_icon_t(int p, int m, int M = std::numeric_limits<int>::max()) : icon(p), lo(m), hi(M) {}
	};
	boost::optional<special_icon_t> special;
	status_info_t() = default;
	status_info_t(bool neg, int p1, int p2 = -1) : isNegative(neg), icon(p1), negIcon(p2) {}
	status_info_t(bool neg, int p1, int p2, special_icon_t spec) : isNegative(neg), icon(p1), negIcon(p2), special(spec) {}
};

const status_info_t& operator* (eStatus status);
const status_info_t& operator* (ePartyStatus status);

enum class eMainStatus {
	ABSENT = 0, // absent, empty slot
	ALIVE = 1,
	DEAD = 2,
	DUST = 3,
	STONE = 4,
	FLED = 5,
	SURFACE = 6, // fled to surface?
	WON = 7,
	SPLIT = 10,
	SPLIT_ABSENT = SPLIT + ABSENT,
	SPLIT_ALIVE = SPLIT + ALIVE,
	SPLIT_DEAD = SPLIT + DEAD,
	SPLIT_DUST = SPLIT + DUST,
	SPLIT_STONE = SPLIT + STONE,
	SPLIT_FLED = SPLIT + FLED,
	SPLIT_SURFACE = SPLIT + SURFACE,
	SPLIT_WON = SPLIT + WON,
};

const std::set<eMainStatus> dead_statuses = { eMainStatus::DEAD, eMainStatus::STONE, eMainStatus::DUST };

inline eMainStatus exceptSplit(eMainStatus stat) {
	if(int(stat) >= 10)
		return (eMainStatus) (-10 + (int)stat);
	return stat;
}

inline bool isSplit(eMainStatus stat) {
	return int(stat) >= 10;
}

inline bool isAbsent(eMainStatus stat) {
	return stat == eMainStatus::ABSENT || int(stat) > 4;
}

inline bool isDead(eMainStatus stat) {
	int code = (int) stat;
	return code > 1 && code < 5;
}

std::pair<int, int> status_bounds(eStatus which);

std::ostream& operator << (std::ostream& out, eStatus e);
std::istream& operator >> (std::istream& in, eStatus& e);
std::istream& operator >> (std::istream& in, ePartyStatus& type);
std::ostream& operator << (std::ostream& out, ePartyStatus type);
std::ostream& operator << (std::ostream& out, eMainStatus e);
std::istream& operator >> (std::istream& in, eMainStatus& e);
std::ostream& operator << (std::ostream& out, eDamageType e);
std::istream& operator >> (std::istream& in, eDamageType& e);

void operator += (eMainStatus& stat, eMainStatus othr);
void operator -= (eMainStatus& stat, eMainStatus othr);

#endif
