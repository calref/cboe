
//#include "item.h"

#include "boe.global.h"
#include "classes.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "mathutil.h"

//extern current_town_type univ.town;
extern short overall_mode,which_combat_type,current_pc,town_type;
extern Boolean web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern Boolean sleep_field;
//extern big_tr_type t_d;
//extern cOutdoors outdoors[2][2];
extern unsigned char combat_terrain[64][64];
//extern unsigned char out[96][96], univ.out.out_e[96][96];
//extern unsigned char univ.out.misc_i[64][64],univ.out.sfx[64][64];
extern char terrain_blocked[256];
extern short town_size[3];
extern cScenario scenario;
extern cUniverse univ;

Boolean is_explored(short i,short j)
{
	if (is_out())
		return (univ.out.out_e[i][j] != 0) ? TRUE : FALSE;
	if (univ.town.explored[i][j] & 1) {
		return TRUE;
		}
		else return FALSE;
}
void make_explored(short i,short j)
{
	if (is_out())
		univ.out.out_e[i][j] = 1;
	univ.town.explored[i][j] = univ.town.explored[i][j] | 1;
}

void take_explored(short i,short j)
{
	if (is_out())
		univ.out.out_e[i][j] = 0;
	univ.town.explored[i][j] = univ.town.explored[i][j] & 254;
}

Boolean is_out()
{
	if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS))
		return TRUE;
		else return FALSE;
}
Boolean is_town()
{
	if (((overall_mode > MODE_OUTDOORS) && (overall_mode < MODE_COMBAT)) || (overall_mode == MODE_LOOK_TOWN))
		return TRUE;
		else return FALSE;
}
Boolean is_combat()
{
	if (((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING)) || (overall_mode == MODE_LOOK_COMBAT))
		return TRUE;
		else return FALSE;
}



Boolean special(short i,short j)
/**/{
	if (((univ.out.misc_i[i][j]) & 2) != 0)
		return TRUE;
		else return FALSE;
}
void flip_special(short i,short j)
/**/{
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] ^ (char) (2);
}
void make_special(short i,short j)
/**/{
	univ.out.misc_i[i][j] =  univ.out.misc_i[i][j] | (char) (2);
}
void take_special(short i,short j)
/**/{
	univ.out.misc_i[i][j] =  univ.out.misc_i[i][j] & 253;
}

Boolean is_web(short i,short j)
/**/{
	return (univ.out.misc_i[i][j] & 4) ? TRUE : FALSE;
}
void make_web(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((univ.out.misc_i[i][j] & 224) || (univ.town.explored[i][j] & 238))
		return;
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 4;
	web = TRUE;
}
void take_web(short i,short j)
/**/{
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 251;
}

Boolean is_crate(short i,short j)
/**/{
	return (univ.out.misc_i[i][j] & 8) ? TRUE : FALSE;
}
void make_crate(short i,short j)
/**/{
//	if (univ.out.misc_i[i][j] & 240)
//		return;
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 8;
	crate = TRUE;
}
void take_crate(short i,short j)
/**/{
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 247;
}

Boolean is_barrel(short i,short j)
/**/{
	return (univ.out.misc_i[i][j] & 16) ? TRUE : FALSE;
}
void make_barrel(short i,short j)
/**/{
//	if (univ.out.misc_i[i][j] & 234)
//		return;
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 16;
	barrel = TRUE;
}
void take_barrel(short i,short j)
/**/{
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 239;
}

Boolean is_fire_barrier(short i,short j)
/**/{
	return (univ.out.misc_i[i][j] & 32) ? TRUE : FALSE;
}
void make_fire_barrier(short i,short j)
/**/{
	if ((is_antimagic(i,j)) && (get_ran(1,0,3) < 3))
		return;
	if (univ.out.misc_i[i][j] & 248)
		return;
	take_web(i,j);
	univ.town.explored[i][j] = univ.town.explored[i][j] & 1;
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 32;
	fire_barrier = TRUE;
}
void take_fire_barrier(short i,short j)
/**/{
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 223;
}

Boolean is_force_barrier(short i,short j)
/**/{
	return (univ.out.misc_i[i][j] & 64) ? TRUE : FALSE;
}
void make_force_barrier(short i,short j)
/**/{
	if ((is_antimagic(i,j)) && (get_ran(1,0,2) < 2))
		return;
	if (univ.out.misc_i[i][j] & 248)
		return;
	take_web(i,j);
	univ.town.explored[i][j] = univ.town.explored[i][j] & 1;
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 64;
	force_barrier = TRUE;
}
void take_force_barrier(short i,short j)
/**/{
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 191;
}

Boolean is_quickfire(short i,short j)
/**/{
	return (univ.out.misc_i[i][j] & 128) ? TRUE : FALSE;
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
	univ.town.explored[i][j] = univ.town.explored[i][j] & 1;
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 3;
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 128;
	quickfire = TRUE;
}
void take_quickfire(short i,short j)
/**/{
	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 127;
}

Boolean is_force_wall(short i,short j)
/**/{
	return (univ.town.explored[i][j] & 2) ? TRUE : FALSE;
}
void make_force_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((univ.town.explored[i][j] & 74) || (univ.out.misc_i[i][j] & 248))
		return;
	take_web(i,j);
	take_fire_wall(i,j);
	univ.town.explored[i][j] = univ.town.explored[i][j] | 2;
	force_wall = TRUE;
}
void take_force_wall(short i,short j)
/**/{
	univ.town.explored[i][j] = univ.town.explored[i][j] & 253;
}


Boolean is_fire_wall(short i,short j)
/**/{
	return (univ.town.explored[i][j] & 4) ? TRUE : FALSE;
}
void make_fire_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((univ.town.explored[i][j] & 248) || (univ.out.misc_i[i][j] & 254))
		return;
	take_web(i,j);
	univ.town.explored[i][j] = univ.town.explored[i][j] | 4;
	fire_wall = TRUE;
}
void take_fire_wall(short i,short j)
/**/{
	univ.town.explored[i][j] = univ.town.explored[i][j] & 251;
}

Boolean is_antimagic(short i,short j)
/**/{
	return (univ.town.explored[i][j] & 8) ? TRUE : FALSE;
}
void make_antimagic(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if (univ.out.misc_i[i][j] & 224)
		return;
	
	univ.town.explored[i][j] = univ.town.explored[i][j] & 1;
	univ.town.explored[i][j] = univ.town.explored[i][j] | 8;
	antimagic = TRUE;
}
void take_antimagic(short i,short j)
/**/{
	univ.town.explored[i][j] = univ.town.explored[i][j] & 247;
}

Boolean is_scloud(short i,short j)
/**/{
	return (univ.town.explored[i][j] & 16) ? TRUE : FALSE;
}
void make_scloud(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
		
	if ((univ.town.explored[i][j] & 238) || (univ.out.misc_i[i][j] & 224))
		return;
	univ.town.explored[i][j] = univ.town.explored[i][j] | 16;
	scloud = TRUE;
}
void take_scloud(short i,short j)
/**/{
	univ.town.explored[i][j] = univ.town.explored[i][j] & 239;
}

Boolean is_ice_wall(short i,short j)
/**/{
	return (univ.town.explored[i][j] & 32) ? TRUE : FALSE;
}
void make_ice_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((univ.town.explored[i][j] & 74) || (univ.out.misc_i[i][j] & 252))
		return;
	take_fire_wall(i,j);
	take_scloud(i,j);
	univ.town.explored[i][j] = univ.town.explored[i][j] | 32;
	ice_wall = TRUE;
}
void take_ice_wall(short i,short j)
/**/{
	univ.town.explored[i][j] = univ.town.explored[i][j] & 223;
}


Boolean is_blade_wall(short i,short j)
/**/{
	return (univ.town.explored[i][j] & 64) ? TRUE : FALSE;
}
void make_blade_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((univ.town.explored[i][j] & 8) || (univ.out.misc_i[i][j] & 224))
		return;
	univ.town.explored[i][j] = univ.town.explored[i][j] & 9;
	univ.town.explored[i][j] = univ.town.explored[i][j] | 64;
	blade_wall = TRUE;
}
void take_blade_wall(short i,short j)
/**/{
	univ.town.explored[i][j] = univ.town.explored[i][j] & 191;
}
Boolean is_sleep_cloud(short i,short j)
/**/{
	return (univ.town.explored[i][j] & 128) ? TRUE : FALSE;
}
void make_sleep_cloud(short i,short j)
/**/{
	if (spot_impassable(i,j) == TRUE)
		return;
	if ((univ.town.explored[i][j] & 8) || (univ.out.misc_i[i][j] & 224))
		return;
	univ.town.explored[i][j] = univ.town.explored[i][j] & 9;
	univ.town.explored[i][j] = univ.town.explored[i][j] | 128;
	sleep_field = TRUE;
}
void take_sleep_cloud(short i,short j)
/**/{
	univ.town.explored[i][j] = univ.town.explored[i][j] & 127;
}

// START SFX
Boolean is_small_blood(short i,short j)
{
	return (univ.out.sfx[i][j] & 1) ? TRUE : FALSE;
}
Boolean is_medium_blood(short i,short j)
{
	return (univ.out.sfx[i][j] & 2) ? TRUE : FALSE;
}
Boolean is_large_blood(short i,short j)
{
	return (univ.out.sfx[i][j] & 4) ? TRUE : FALSE;
}
Boolean is_small_slime(short i,short j)
{
	return (univ.out.sfx[i][j] & 8) ? TRUE : FALSE;
}
Boolean is_big_slime(short i,short j)
{
	return (univ.out.sfx[i][j] & 16) ? TRUE : FALSE;
}
Boolean is_ash(short i,short j)
{
	return (univ.out.sfx[i][j] & 32) ? TRUE : FALSE;
}
Boolean is_bones(short i,short j)
{
	return (univ.out.sfx[i][j] & 64) ? TRUE : FALSE;
}
Boolean is_rubble(short i,short j)
{
	return (univ.out.sfx[i][j] & 128) ? TRUE : FALSE;
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
			if (univ.out.sfx[i][j] == 4)
				return;
			if (univ.out.sfx[i][j] < 4)
				type = min(3,type + univ.out.sfx[i][j]);
			break;
		case 4:
			if (univ.out.sfx[i][j] == 8)
				type = 5;
			break;
		}
	univ.out.sfx[i][j] = s_pow(2,type - 1);
}

void spread_sfx(short type,short prob)
// prob is 1 - 100
{
	short i,j;
	
	for (i = 0; i < town_size[town_type]; i++)
		for (j = 0; j < town_size[town_type]; j++)
			if ((univ.town.town->terrain(i,j) < 5) && (get_ran(1,1,100) <= prob))
				make_sfx(i,j,type);
				
}