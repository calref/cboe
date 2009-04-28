/*
 *  terrain.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

namespace legacy { struct terrain_type_type; };

class cTerrain {
public:
	short picture;
	unsigned char blockage;
	unsigned char flag1;
	unsigned char flag2;
	unsigned char special;
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