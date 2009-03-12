#include "global.h"
#include "loc_utils.h"
#include "fields.h"

extern current_town_type c_town;
extern short overall_mode,which_combat_type,current_pc,town_type;
extern Boolean web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern Boolean sleep_field;
extern big_tr_type t_d;
extern outdoor_record_type outdoors[2][2];
extern unsigned char combat_terrain[64][64];
extern unsigned char out[96][96], out_e[96][96];
extern unsigned char misc_i[64][64],sfx[64][64];
extern char terrain_blocked[256];
extern short town_size[3];
extern scenario_data_type scenario;

Boolean is_explored(short i,short j)
{
	if (is_out())
		return (out_e[i][j] != 0) ? TRUE : FALSE;
	if (c_town.explored[i][j] & 1) {
		return TRUE;
		}
		else return FALSE;
}
void make_explored(short i,short j)
{
	if (is_out())
		out_e[i][j] = 1;
	c_town.explored[i][j] = c_town.explored[i][j] | 1;
}

void take_explored(short i,short j)
{
	if (is_out())
		out_e[i][j] = 0;
	c_town.explored[i][j] = c_town.explored[i][j] & 254;
}

Boolean is_out()
{
	if ((overall_mode == 0) || (overall_mode == 35))
		return TRUE;
		else return FALSE;
}
Boolean is_town()
{
	if (((overall_mode > 0) && (overall_mode < 10)) || (overall_mode == 36))
		return TRUE;
		else return FALSE;
}
Boolean is_combat()
{
	if (((overall_mode >= 10) && (overall_mode < 20)) || (overall_mode == 37))
		return TRUE;
		else return FALSE;
}



Boolean special(short i,short j)
/**/{
	if (((misc_i[i][j]) & 2) != 0)
		return TRUE;
		else return FALSE;
}
void flip_special(short i,short j)
/**/{
	misc_i[i][j] = misc_i[i][j] ^ (char) (2);
}
void make_special(short i,short j)
/**/{
	misc_i[i][j] =  misc_i[i][j] | (char) (2);
}
void take_special(short i,short j)
/**/{
	misc_i[i][j] =  misc_i[i][j] & 253;
}

Boolean is_web(short i,short j)
/**/{
	return (misc_i[i][j] & 4) ? TRUE : FALSE;
}
void make_web(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((misc_i[i][j] & 224) || (c_town.explored[i][j] & 238))
		return;
	misc_i[i][j] = misc_i[i][j] | 4;
	web = TRUE;
}
void take_web(short i,short j)
/**/{
	misc_i[i][j] = misc_i[i][j] & 251;
}

Boolean is_crate(short i,short j)
/**/{
	return (misc_i[i][j] & 8) ? TRUE : FALSE;
}
void make_crate(short i,short j)
/**/{
//	if (misc_i[i][j] & 240)
//		return;
	misc_i[i][j] = misc_i[i][j] | 8;
	crate = TRUE;
}
void take_crate(short i,short j)
/**/{
	misc_i[i][j] = misc_i[i][j] & 247;
}

Boolean is_barrel(short i,short j)
/**/{
	return (misc_i[i][j] & 16) ? TRUE : FALSE;
}
void make_barrel(short i,short j)
/**/{
//	if (misc_i[i][j] & 234)
//		return;
	misc_i[i][j] = misc_i[i][j] | 16;
	barrel = TRUE;
}
void take_barrel(short i,short j)
/**/{
	misc_i[i][j] = misc_i[i][j] & 239;
}

Boolean is_fire_barrier(short i,short j)
/**/{
	return (misc_i[i][j] & 32) ? TRUE : FALSE;
}
void make_fire_barrier(short i,short j)
/**/{
	if ((is_antimagic(i,j)) && (get_ran(1,0,3) < 3))
		return;
	if (misc_i[i][j] & 248)
		return;
	take_web(i,j);
	c_town.explored[i][j] = c_town.explored[i][j] & 1;
	misc_i[i][j] = misc_i[i][j] | 32;
	fire_barrier = TRUE;
}
void take_fire_barrier(short i,short j)
/**/{
	misc_i[i][j] = misc_i[i][j] & 223;
}

Boolean is_force_barrier(short i,short j)
/**/{
	return (misc_i[i][j] & 64) ? TRUE : FALSE;
}
void make_force_barrier(short i,short j)
/**/{
	if ((is_antimagic(i,j)) && (get_ran(1,0,2) < 2))
		return;
	if (misc_i[i][j] & 248)
		return;
	take_web(i,j);
	c_town.explored[i][j] = c_town.explored[i][j] & 1;
	misc_i[i][j] = misc_i[i][j] | 64;
	force_barrier = TRUE;
}
void take_force_barrier(short i,short j)
/**/{
	misc_i[i][j] = misc_i[i][j] & 191;
}

Boolean is_quickfire(short i,short j)
/**/{
	return (misc_i[i][j] & 128) ? TRUE : FALSE;
}
void make_quickfire(short i,short j)
/**/{////
	unsigned char ter;

	if ((is_antimagic(i,j)) && (get_ran(1,0,1) == 0))
		return;
	if ((is_force_barrier(i,j)) || (is_fire_barrier(i,j)))
		return;
	ter = coord_to_ter(i,j);
	if (scenario.ter_types[ter].blockage == 1)
		return;
	if (scenario.ter_types[ter].blockage == 5)
		return;
	c_town.explored[i][j] = c_town.explored[i][j] & 1;
	misc_i[i][j] = misc_i[i][j] & 3;
	misc_i[i][j] = misc_i[i][j] | 128;
	quickfire = TRUE;
}
void take_quickfire(short i,short j)
/**/{
	misc_i[i][j] = misc_i[i][j] & 127;
}

Boolean is_force_wall(short i,short j)
/**/{
	return (c_town.explored[i][j] & 2) ? TRUE : FALSE;
}
void make_force_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((c_town.explored[i][j] & 74) || (misc_i[i][j] & 248))
		return;
	take_web(i,j);
	take_fire_wall(i,j);
	c_town.explored[i][j] = c_town.explored[i][j] | 2;
	force_wall = TRUE;
}
void take_force_wall(short i,short j)
/**/{
	c_town.explored[i][j] = c_town.explored[i][j] & 253;
}


Boolean is_fire_wall(short i,short j)
/**/{
	return (c_town.explored[i][j] & 4) ? TRUE : FALSE;
}
void make_fire_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((c_town.explored[i][j] & 248) || (misc_i[i][j] & 254))
		return;
	take_web(i,j);
	c_town.explored[i][j] = c_town.explored[i][j] | 4;
	fire_wall = TRUE;
}
void take_fire_wall(short i,short j)
/**/{
	c_town.explored[i][j] = c_town.explored[i][j] & 251;
}

Boolean is_antimagic(short i,short j)
/**/{
	return (c_town.explored[i][j] & 8) ? TRUE : FALSE;
}
void make_antimagic(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if (misc_i[i][j] & 224)
		return;
	
	c_town.explored[i][j] = c_town.explored[i][j] & 1;
	c_town.explored[i][j] = c_town.explored[i][j] | 8;
	antimagic = TRUE;
}
void take_antimagic(short i,short j)
/**/{
	c_town.explored[i][j] = c_town.explored[i][j] & 247;
}

Boolean is_scloud(short i,short j)
/**/{
	return (c_town.explored[i][j] & 16) ? TRUE : FALSE;
}
void make_scloud(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
		
	if ((c_town.explored[i][j] & 238) || (misc_i[i][j] & 224))
		return;
	c_town.explored[i][j] = c_town.explored[i][j] | 16;
	scloud = TRUE;
}
void take_scloud(short i,short j)
/**/{
	c_town.explored[i][j] = c_town.explored[i][j] & 239;
}

Boolean is_ice_wall(short i,short j)
/**/{
	return (c_town.explored[i][j] & 32) ? TRUE : FALSE;
}
void make_ice_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((c_town.explored[i][j] & 74) || (misc_i[i][j] & 252))
		return;
	take_fire_wall(i,j);
	take_scloud(i,j);
	c_town.explored[i][j] = c_town.explored[i][j] | 32;
	ice_wall = TRUE;
}
void take_ice_wall(short i,short j)
/**/{
	c_town.explored[i][j] = c_town.explored[i][j] & 223;
}


Boolean is_blade_wall(short i,short j)
/**/{
	return (c_town.explored[i][j] & 64) ? TRUE : FALSE;
}
void make_blade_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((c_town.explored[i][j] & 8) || (misc_i[i][j] & 224))
		return;
	c_town.explored[i][j] = c_town.explored[i][j] & 9;
	c_town.explored[i][j] = c_town.explored[i][j] | 64;
	blade_wall = TRUE;
}
void take_blade_wall(short i,short j)
/**/{
	c_town.explored[i][j] = c_town.explored[i][j] & 191;
}
Boolean is_sleep_cloud(short i,short j)
/**/{
	return (c_town.explored[i][j] & 128) ? TRUE : FALSE;
}
void make_sleep_cloud(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((c_town.explored[i][j] & 8) || (misc_i[i][j] & 224))
		return;
	c_town.explored[i][j] = c_town.explored[i][j] & 9;
	c_town.explored[i][j] = c_town.explored[i][j] | 128;
	sleep_field = TRUE;
}
void take_sleep_cloud(short i,short j)
/**/{
	c_town.explored[i][j] = c_town.explored[i][j] & 127;
}

// START SFX
Boolean is_small_blood(short i,short j)
{
	return (sfx[i][j] & 1) ? TRUE : FALSE;
}
Boolean is_medium_blood(short i,short j)
{
	return (sfx[i][j] & 2) ? TRUE : FALSE;
}
Boolean is_large_blood(short i,short j)
{
	return (sfx[i][j] & 4) ? TRUE : FALSE;
}
Boolean is_small_slime(short i,short j)
{
	return (sfx[i][j] & 8) ? TRUE : FALSE;
}
Boolean is_big_slime(short i,short j)
{
	return (sfx[i][j] & 16) ? TRUE : FALSE;
}
Boolean is_ash(short i,short j)
{
	return (sfx[i][j] & 32) ? TRUE : FALSE;
}
Boolean is_bones(short i,short j)
{
	return (sfx[i][j] & 64) ? TRUE : FALSE;
}
Boolean is_rubble(short i,short j)
{
	return (sfx[i][j] & 128) ? TRUE : FALSE;
}
void make_sfx(short i,short j, short type)
{
	unsigned char ter;

	if (get_obscurity(i,j) > 0)
		return;
	ter = coord_to_ter(i,j);
	if (terrain_blocked[ter] != 0)
		return;
	switch (type) {
		case 1: case 2: 
			if (sfx[i][j] == 4)
				return;
			if (sfx[i][j] < 4)
				type = min(3,type + sfx[i][j]);
			break;
		case 4:
			if (sfx[i][j] == 8)
				type = 5;
			break;
		}
	sfx[i][j] = s_pow(2,type - 1);
}

void spread_sfx(short type,short prob)
// prob is 1 - 100
{
	short i,j;
	
	for (i = 0; i < town_size[town_type]; i++)
		for (j = 0; j < town_size[town_type]; j++)
			if ((t_d.terrain[i][j] < 5) && (get_ran(1,0,100) <= prob))
				make_sfx(i,j,type);
				
}