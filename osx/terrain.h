/*
 *  terrain.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

namespace legacy { struct terrain_type_type; };

/* Terrains Specials Properties : scenario.ter_types[i].special */      //complete

enum eTerSpec {
	TER_SPEC_NONE = 0,
	TER_SPEC_CHANGE_WHEN_STEP_ON = 1,
	TER_SPEC_DOES_FIRE_DAMAGE = 2,
	TER_SPEC_DOES_COLD_DAMAGE = 3,
	TER_SPEC_DOES_MAGIC_DAMAGE = 4,
	TER_SPEC_POISON_LAND = 5,
	TER_SPEC_DISEASED_LAND = 6,
	TER_SPEC_CRUMBLING_TERRAIN = 7,
	TER_SPEC_LOCKABLE_TERRAIN = 8,
	TER_SPEC_UNLOCKABLE_TERRAIN = 9,
	TER_SPEC_UNLOCKABLE_BASHABLE = 10,
	TER_SPEC_IS_A_SIGN = 11,
	TER_SPEC_CALL_LOCAL_SPECIAL = 12,
	TER_SPEC_CALL_SCENARIO_SPECIAL = 13,
	TER_SPEC_IS_A_CONTAINER = 14,
	TER_SPEC_WATERFALL = 15,
	TER_SPEC_CONVEYOR_NORTH = 16,
	TER_SPEC_CONVEYOR_EAST = 17,
	TER_SPEC_CONVEYOR_SOUTH = 18,
	TER_SPEC_CONVEYOR_WEST = 19,
	TER_SPEC_BLOCKED_TO_MONSTERS = 20,
	TER_SPEC_TOWN_ENTRANCE = 21,
	TER_SPEC_CHNAGE_WHEN_USED = 22,
	TER_SPEC_CALL_SPECIAL_WHEN_USED = 23,
};

class cTerrain {
public:
	short picture;
	unsigned char blockage;
	unsigned char flag1;
	unsigned char flag2;
	eTerSpec special;
	unsigned char trans_to_what;
	unsigned char fly_over;
	unsigned char boat_over;
	unsigned char block_horse;
	unsigned char light_radius;
	unsigned char step_sound;
	unsigned char shortcut_key; // for editor use only
	unsigned char obj_num; // ditto (formerly res1)
	unsigned char ground_type; // ditto (formerly res2)
	unsigned char trim_type; // ditto, mostly (formerly res3)
	unsigned short trim_ter; // ditto
	unsigned short combat_arena;
	location obj_pos; // editor use only
	
	cTerrain& operator = (legacy::terrain_type_type& old);
};
