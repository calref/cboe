/*
 *  terrain.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>
#include <iostream>

namespace legacy { struct terrain_type_type; };

/* Terrains Specials Properties : scenario.ter_types[i].special */      //complete

enum eTerSpec {
//	TER_SPEC_NONE = 0,
//	TER_SPEC_CHANGE_WHEN_STEP_ON = 1,
//	TER_SPEC_DOES_FIRE_DAMAGE = 2,
//	TER_SPEC_DOES_COLD_DAMAGE = 3,
//	TER_SPEC_DOES_MAGIC_DAMAGE = 4,
//	TER_SPEC_POISON_LAND = 5,
//	TER_SPEC_DISEASED_LAND = 6,
//	TER_SPEC_CRUMBLING_TERRAIN = 7,
//	TER_SPEC_LOCKABLE_TERRAIN = 8,
//	TER_SPEC_UNLOCKABLE_TERRAIN = 9,
//	TER_SPEC_UNLOCKABLE_BASHABLE = 10,
//	TER_SPEC_IS_A_SIGN = 11,
//	TER_SPEC_CALL_LOCAL_SPECIAL = 12,
//	TER_SPEC_CALL_SCENARIO_SPECIAL = 13,
//	TER_SPEC_IS_A_CONTAINER = 14,
//	TER_SPEC_WATERFALL = 15,
//	TER_SPEC_CONVEYOR_NORTH = 16,
//	TER_SPEC_CONVEYOR_EAST = 17,
//	TER_SPEC_CONVEYOR_SOUTH = 18,
//	TER_SPEC_CONVEYOR_WEST = 19,
//	TER_SPEC_BLOCKED_TO_MONSTERS = 20,
//	TER_SPEC_TOWN_ENTRANCE = 21,
//	TER_SPEC_CHANGE_WHEN_USED = 22,
//	TER_SPEC_CALL_SPECIAL_WHEN_USED = 23,
	TER_SPEC_NONE = 0,
	TER_SPEC_CHANGE_WHEN_STEP_ON = 1,
	TER_SPEC_DAMAGING = 2,
	TER_SPEC_BRIDGE = 3, // new
	TER_SPEC_BED = 4, // new
	TER_SPEC_DANGEROUS = 5,
	TER_SPEC_UNUSED1 = 6,
	TER_SPEC_CRUMBLING = 7,
	TER_SPEC_LOCKABLE = 8,
	TER_SPEC_UNLOCKABLE = 9,
	TER_SPEC_UNUSED2 = 10,
	TER_SPEC_IS_A_SIGN = 11,
	TER_SPEC_CALL_SPECIAL = 12,
	TER_SPEC_UNUSED3 = 13,
	TER_SPEC_IS_A_CONTAINER = 14,
	TER_SPEC_WATERFALL = 15,
	TER_SPEC_CONVEYOR = 16,
	TER_SPEC_UNUSED4 = 17,
	TER_SPEC_UNUSED5 = 18,
	TER_SPEC_UNUSED6 = 19,
	TER_SPEC_BLOCKED_TO_MONSTERS = 20,
	TER_SPEC_TOWN_ENTRANCE = 21,
	TER_SPEC_CHANGE_WHEN_USED = 22,
	TER_SPEC_CALL_SPECIAL_WHEN_USED = 23,
//	1. Change when step on (What to change to, number of sound, Unused)
//	2. Damaging terrain; can't rest here (Amount of damage done, multiplier, damage type)
//	3. Reserved
//	4. Reserved
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
//	24. Bridge - if the party boats over it, they get the option to land. (Unused, Unused, Unused)
};

enum eTrimType {
	TRIM_NONE = 0,
	TRIM_WALL = 1, // not a trim, but trims will conform to it as if it's the same ground type (eg stone wall)
	TRIM_S, TRIM_SE, TRIM_E, TRIM_NE, TRIM_N, TRIM_NW, TRIM_W, TRIM_SW,
	TRIM_NE_INNER, TRIM_SE_INNER, TRIM_SW_INNER, TRIM_NW_INNER,
	TRIM_FRILLS = 14, // like on lava and underground water; no trim_ter required
	TRIM_ROAD = 15, // the game will treat it like a road space and draw roads; no trim_ter required
	TRIM_WALKWAY = 16, // the game will draw walkway corners; trim_ter is base terrain to draw on
	TRIM_WATERFALL = 17, // special case for waterfalls
	TRIM_CITY = 18, // the game will join roads up to this space but not draw roads on the space
};

class cTerrain {
public:
	std::string name;
	short picture;
	unsigned char blockage;
	unsigned short flag1;
	unsigned short flag2;
	unsigned short flag3; // new additional flag for special properties
	eTerSpec special;
	unsigned short trans_to_what;
	unsigned char fly_over;
	unsigned char boat_over;
	unsigned char block_horse;
	unsigned char light_radius;
	unsigned char step_sound;
	unsigned char shortcut_key; // for editor use only
	unsigned char obj_num; // ditto (formerly res1)
	unsigned char ground_type; // ditto (formerly res2)
	eTrimType trim_type; // ditto, mostly (formerly res3)
	unsigned short trim_ter; // ditto
	unsigned short combat_arena;
	location obj_pos; // editor use only
	location obj_size; // editor use only
	unsigned short i; // for temporary use in porting
	
	cTerrain& operator = (legacy::terrain_type_type& old);
	void writeTo(std::ostream& file);
};

#endif