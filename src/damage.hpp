//
//  damage.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_DATA_DAMAGE_HPP
#define BoE_DATA_DAMAGE_HPP

enum class eDamageType {
	WEAPON = 0,
	FIRE = 1,
	POISON = 2,
	MAGIC = 3,
	UNBLOCKABLE = 4,
	COLD = 5,
	UNDEAD = 6,
	DEMON = 7,
	// Keep these two last
	SPECIAL = 8, // Completely unblockable damage from assassination skill
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

inline bool isStatusNegative(eStatus stat) {
	switch(stat) {
		case eStatus::MAIN: return false;
		case eStatus::POISONED_WEAPON: return false;
		case eStatus::BLESS_CURSE: return false;
		case eStatus::POISON: return true;
		case eStatus::HASTE_SLOW: return false;
		case eStatus::INVULNERABLE: return false;
		case eStatus::MAGIC_RESISTANCE: return false;
		case eStatus::WEBS: return true;
		case eStatus::DISEASE: return true;
		case eStatus::INVISIBLE: return false;
		case eStatus::DUMB: return true;
		case eStatus::MARTYRS_SHIELD: return false;
		case eStatus::ASLEEP: return true;
		case eStatus::PARALYZED: return true;
		case eStatus::ACID: return true;
		case eStatus::FORCECAGE: return true;
		case eStatus::CHARM: return true;
	}
	return false;
}

enum class ePartyStatus {
	STEALTH,
	FLIGHT,
	DETECT_LIFE,
	FIREWALK,
};

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
