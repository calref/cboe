//
//  race.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_RACE_HPP
#define BoE_RACE_HPP

enum class eRace {
	UNKNOWN = -1, // for parameters to some functions; not valid in the class
	HUMAN = 0,
	NEPHIL = 1,
	SLITH = 2,
	VAHNATAI = 3,	// Former value from eMonsterType
	REPTILE = 4,	// 1
	BEAST = 5,		// 2
	IMPORTANT = 6,	// 3
	MAGE = 7,		// 4
	PRIEST = 8,		// 5
	HUMANOID = 9,	// 6
	DEMON = 10,		// 7
	UNDEAD = 11,	// 8
	GIANT = 12,		// 9
	SLIME = 13,		// 10
	STONE = 14,		// 11
	BUG = 15,		// 12
	DRAGON = 16,	// 13
	MAGICAL = 17,	// 14
	PLANT = 18,
	BIRD = 19,
	SKELETAL = 20,
	GOBLIN = 21,
};

// Types IMPORTANT, MAGE, and PRIEST are implicitly human
inline bool isHuman(eRace race) {
	int code = (int) race;
	return code == 0 || (code >= 6 && code <= 8);
}

// Types NEPHIL, SLITH, and VAHNATAI are implicitly humanoid
inline bool isHumanoid(eRace race) {
	int code = (int) race;
	return (code >= 0 && code <= 3) || (code >= 6 && code <= 9) || code == 21;
}

std::ostream& operator << (std::ostream& out, eRace e);
std::istream& operator >> (std::istream& in, eRace& e);

#endif
