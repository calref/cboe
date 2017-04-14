//
//  terrain_abilities.hpp
//  BoE
//
//  Created by Celtic Minstrel on 17-04-13.
//
//

#ifndef BoE_TERRAIN_ABILITIES_HPP
#define BoE_TERRAIN_ABILITIES_HPP

enum class eTerSpec {
	NONE = 0,
	CHANGE_WHEN_STEP_ON = 1,
	DAMAGING = 2, // formerly "fire damage"
	BRIDGE = 3, // new; formerly "cold damage"
	BED = 4, // new; formerly "magic damage"
	DANGEROUS = 5, // formerly "poison land"
	UNUSED1 = 6, // formerly "disease land"
	CRUMBLING = 7,
	LOCKABLE = 8,
	UNLOCKABLE = 9,
	UNUSED2 = 10, // formerly "unlockable + bashable"
	IS_A_SIGN = 11,
	CALL_SPECIAL = 12, // formerly "call local special"
	UNUSED3 = 13, // formerly "call scenario special"
	IS_A_CONTAINER = 14,
	WILDERNESS_CAVE = 15,
	WILDERNESS_SURFACE = 16, // formerly "conveyor north"
	WATERFALL_CAVE = 17, // formerly "conveyor east"
	WATERFALL_SURFACE = 18, // formerly "conveyor south"
	CONVEYOR = 19, // formerly "conveyor west"
	BLOCKED_TO_MONSTERS = 20,
	TOWN_ENTRANCE = 21,
	CHANGE_WHEN_USED = 22,
	CALL_SPECIAL_WHEN_USED = 23,
	//	1. Change when step on (What to change to, number of sound, Unused)
	//	2. Damaging terrain; can't rest here (Amount of damage done, multiplier, damage type)
	//	3. Bridge - if the party boats over it, they get the option to land. (Unused, Unused, Unused)
	//	4. Bed - change party graphic when they stand on this space
	//	5. Dangerous land; can't rest here; percentage chance may be 0 (Strength, Percentage chance, status type)
	//	6. Reserved
	//	7. Crumbling terrain (Terrain to change to, strength?, destroyed by what - quickfire, shatter/move mountains, or both)
	//	8. Lockable terrain (Terrain to change to when locked, Unused, Unused)
	//	9. Unlockable terrain (Terrain to change to when locked, Difficulty, can be bashed)
	//	10. Reserved
	//	11. Sign (Unused, Unused, Unused)
	//	12. Call special (Special to call, local or scenario?, Unused)
	//	13. Reserved
	//	14. Container (Unused, Unused, Unused)
	//	15. Waterfall (Direction, Unused, Unused)
	//	16. Conveyor Belt (Direction, Unused, Unused)
	//	17. Reserved
	//	18. Reserved
	//	19. Reserved
	//	20. Blocked to Monsters (Unused, Unused, Unused)
	//	21. Town entrance (Terrain type if hidden, Unused, Unused)
	//	22. Change when Used (Terrain to change to when used, Number of sound, Unused)
	//	23. Call special when used (Special to call, local or scenario?, Unused)
};

enum class eTrimType {
	NONE = 0,
	WALL = 1, // not a trim, but trims will conform to it as if it's the same ground type (eg stone wall)
	S, SE, E, NE, N, NW, W, SW,
	NE_INNER, SE_INNER, SW_INNER, NW_INNER,
	FRILLS = 14, // like on lava and underground water; no trim_ter required
	WALKWAY = 16, // the game will draw walkway corners; trim_ter is base terrain to draw on
	WATERFALL = 17, // special case for waterfalls
	CITY = 18, // the game will join roads up to this space but not draw roads on the space
};

enum class eTerObstruct {
	CLEAR = 0,
	BLOCK_SIGHT = 1,
	BLOCK_MONSTERS = 2,
	BLOCK_MOVE = 3,
	BLOCK_MOVE_AND_SHOOT = 4,
	BLOCK_MOVE_AND_SIGHT = 5,
};

enum class eStepSnd {STEP, SQUISH, CRUNCH, NONE, SPLASH};

std::ostream& operator << (std::ostream& out, eTerSpec e);
std::istream& operator >> (std::istream& in, eTerSpec& e);
std::ostream& operator << (std::ostream& out, eTrimType e);
std::istream& operator >> (std::istream& in, eTrimType& e);
std::ostream& operator << (std::ostream& out, eTerObstruct e);
std::istream& operator >> (std::istream& in, eTerObstruct& e);
std::ostream& operator << (std::ostream& out, eStepSnd e);
std::istream& operator >> (std::istream& in, eStepSnd& e);

#endif
