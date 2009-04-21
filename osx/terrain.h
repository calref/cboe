/*
 *  terrain.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

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
	unsigned char shortcut_key;
	unsigned char res1;
	unsigned char res2;
	unsigned char res3;
};