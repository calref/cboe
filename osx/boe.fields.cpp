
//#include "item.h"

#include "boe.global.h"

#include "classes.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "mathutil.h"

//extern current_town_type univ.town;
extern short which_combat_type,current_pc,town_type;
extern eGameMode overall_mode;
extern bool web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern bool sleep_field;
//extern big_tr_type t_d;
//extern cOutdoors outdoors[2][2];
extern ter_num_t combat_terrain[64][64];
//extern unsigned char out[96][96], univ.out.out_e[96][96];
//extern unsigned char univ.out.misc_i[64][64],univ.out.sfx[64][64];
//extern short town_size[3];
extern cScenario scenario;
extern cUniverse univ;

bool is_explored(short i,short j)
{
	if (is_out())
		return (univ.out.out_e[i][j] != 0) ? true : false;
	else return univ.town.is_explored(i,j);
//	if (univ.town.explored[i][j] & 1) {
//		return true;
//		}
//		else return false;
}
void make_explored(short i,short j)
{
	if (is_out())
		univ.out.out_e[i][j] = 1;
//	univ.town.explored[i][j] = univ.town.explored[i][j] | 1;
	else univ.town.set_explored(i,j,true);
}

void take_explored(short i,short j)
{
	if (is_out())
		univ.out.out_e[i][j] = 0;
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 254;
	else univ.town.set_explored(i,j,false);
}

bool is_out()
{
	if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS))
		return true;
	else return false;
}
bool is_town()
{
	if (((overall_mode > MODE_OUTDOORS) && (overall_mode < MODE_COMBAT)) || (overall_mode == MODE_LOOK_TOWN))
		return true;
	else return false;
}
bool is_combat()
{
	if (((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING)) || (overall_mode == MODE_LOOK_COMBAT))
		return true;
	else return false;
}



//bool special(short i,short j)
///**/{
//	if (((univ.out.misc_i[i][j]) & 2) != 0)
//		return true;
//		else return false;
//}
//void flip_special(short i,short j)
///**/{
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] ^ (char) (2);
//}
//void make_special(short i,short j)
///**/{
//	univ.out.misc_i[i][j] =  univ.out.misc_i[i][j] | (char) (2);
//}
//void take_special(short i,short j)
///**/{
//	univ.out.misc_i[i][j] =  univ.out.misc_i[i][j] & 253;
//}

//bool is_web(short i,short j)
///**/{
//	return (univ.out.misc_i[i][j] & 4) ? true : false;
//}
void make_web(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
//	if ((univ.out.misc_i[i][j] & 224) || (univ.town.explored[i][j] & 238))
//		return;
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 4;
	web = univ.town.set_web(i,j,true);
//	web = true;
}
//void take_web(short i,short j)
///**/{
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 251;
//}

//bool is_crate(short i,short j)
///**/{
//	return (univ.out.misc_i[i][j] & 8) ? true : false;
//}
//void make_crate(short i,short j)
///**/{
////	if (univ.out.misc_i[i][j] & 240)
////		return;
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 8;
//	crate = true;
//}
//void take_crate(short i,short j)
///**/{
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 247;
//}

//bool is_barrel(short i,short j)
///**/{
//	return (univ.out.misc_i[i][j] & 16) ? true : false;
//}
//void make_barrel(short i,short j)
///**/{
////	if (univ.out.misc_i[i][j] & 234)
////		return;
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 16;
//	barrel = true;
//}
//void take_barrel(short i,short j)
///**/{
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 239;
//}

//bool is_fire_barrier(short i,short j)
///**/{
//	return (univ.out.misc_i[i][j] & 32) ? true : false;
//}
//void make_fire_barrier(short i,short j)
///**/{
//	if ((is_antimagic(i,j)) && (get_ran(1,0,3) < 3))
//		return;
//	if (univ.out.misc_i[i][j] & 248)
//		return;
//	take_web(i,j);
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 1;
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 32;
//	fire_barrier = true;
//}
//void take_fire_barrier(short i,short j)
///**/{
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 223;
//}

//bool is_force_barrier(short i,short j)
///**/{
//	return (univ.out.misc_i[i][j] & 64) ? true : false;
//}
//void make_force_barrier(short i,short j)
///**/{
//	if ((is_antimagic(i,j)) && (get_ran(1,0,2) < 2))
//		return;
//	if (univ.out.misc_i[i][j] & 248)
//		return;
//	take_web(i,j);
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 1;
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 64;
//	force_barrier = true;
//}
//void take_force_barrier(short i,short j)
///**/{
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 191;
//}

//bool is_quickfire(short i,short j)
///**/{
//	return (univ.out.misc_i[i][j] & 128) ? true : false;
//}
void make_quickfire(short i,short j)
/**/{////
	ter_num_t ter;
//
//	if ((is_antimagic(i,j)) && (get_ran(1,0,1) == 0))
//		return;
//	if ((is_force_barrier(i,j)) || (is_fire_barrier(i,j)))
//		return;
	ter = coord_to_ter(i,j);
	if(scenario.ter_types[ter].blockage == eTerObstruct::BLOCK_SIGHT)
		return;
	// TODO: Isn't it a little odd that BLOCK_MOVE_AND_SHOOT isn't included here?
	if(scenario.ter_types[ter].blockage == eTerObstruct::BLOCK_MOVE_AND_SIGHT)
		return;
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 1;
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 3;
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] | 128;
	quickfire = univ.town.set_quickfire(i,j,true);
//	quickfire = true;
}
//void take_quickfire(short i,short j)
///**/{
//	univ.out.misc_i[i][j] = univ.out.misc_i[i][j] & 127;
//}

//bool is_force_wall(short i,short j)
///**/{
//	return (univ.town.explored[i][j] & 2) ? true : false;
//}
void make_force_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
//	if ((univ.town.explored[i][j] & 74) || (univ.out.misc_i[i][j] & 248))
//		return;
//	take_web(i,j);
//	take_fire_wall(i,j);
//	univ.town.explored[i][j] = univ.town.explored[i][j] | 2;
	force_wall = univ.town.set_force_wall(i,j,true);
//	force_wall = true;
}
//void take_force_wall(short i,short j)
///**/{
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 253;
//}


//bool is_fire_wall(short i,short j)
///**/{
//	return (univ.town.explored[i][j] & 4) ? true : false;
//}
void make_fire_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
//	if ((univ.town.explored[i][j] & 248) || (univ.out.misc_i[i][j] & 254))
//		return;
//	take_web(i,j);
//	univ.town.explored[i][j] = univ.town.explored[i][j] | 4;
	fire_wall = univ.town.set_fire_wall(i,j,true);
//	fire_wall = true;
}
//void take_fire_wall(short i,short j)
///**/{
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 251;
//}

//bool is_antimagic(short i,short j)
///**/{
//	return (univ.town.explored[i][j] & 8) ? true : false;
//}
void make_antimagic(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
//	if (univ.out.misc_i[i][j] & 224)
//		return;
//
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 1;
//	univ.town.explored[i][j] = univ.town.explored[i][j] | 8;
	antimagic = univ.town.set_antimagic(i,j,true);
//	antimagic = true;
}
//void take_antimagic(short i,short j)
///**/{
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 247;
//}

//bool is_scloud(short i,short j)
///**/{
//	return (univ.town.explored[i][j] & 16) ? true : false;
//}
void make_scloud(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
//
//	if ((univ.town.explored[i][j] & 238) || (univ.out.misc_i[i][j] & 224))
//		return;
//	univ.town.explored[i][j] = univ.town.explored[i][j] | 16;
	scloud = univ.town.set_scloud(i,j,true);
//	scloud = true;
}
//void take_scloud(short i,short j)
///**/{
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 239;
//}

//bool is_ice_wall(short i,short j)
///**/{
//	return (univ.town.explored[i][j] & 32) ? true : false;
//}
void make_ice_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
//	if ((univ.town.explored[i][j] & 74) || (univ.out.misc_i[i][j] & 252))
//		return;
//	take_fire_wall(i,j);
//	take_scloud(i,j);
//	univ.town.explored[i][j] = univ.town.explored[i][j] | 32;
	ice_wall = univ.town.set_ice_wall(i,j,true);
//	ice_wall = true;
}
//void take_ice_wall(short i,short j)
///**/{
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 223;
//}


//bool is_blade_wall(short i,short j)
///**/{
//	return (univ.town.explored[i][j] & 64) ? true : false;
//}
void make_blade_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
//	if ((univ.town.explored[i][j] & 8) || (univ.out.misc_i[i][j] & 224))
//		return;
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 9;
//	univ.town.explored[i][j] = univ.town.explored[i][j] | 64;
	blade_wall = univ.town.set_blade_wall(i,j,true);
//	blade_wall = true;
}
//void take_blade_wall(short i,short j)
///**/{
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 191;
//}
//bool is_sleep_cloud(short i,short j)
///**/{
//	return (univ.town.explored[i][j] & 128) ? true : false;
//}
void make_sleep_cloud(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
//	if ((univ.town.explored[i][j] & 8) || (univ.out.misc_i[i][j] & 224))
//		return;
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 9;
//	univ.town.explored[i][j] = univ.town.explored[i][j] | 128;
	sleep_field = univ.town.set_sleep_cloud(i,j,true);
//	sleep_field = true;
}
//void take_sleep_cloud(short i,short j)
///**/{
//	univ.town.explored[i][j] = univ.town.explored[i][j] & 127;
//}

// START SFX
//bool is_small_blood(short i,short j)
//{
//	return (univ.out.sfx[i][j] & 1) ? true : false;
//}
//bool is_medium_blood(short i,short j)
//{
//	return (univ.out.sfx[i][j] & 2) ? true : false;
//}
//bool is_large_blood(short i,short j)
//{
//	return (univ.out.sfx[i][j] & 4) ? true : false;
//}
//bool is_small_slime(short i,short j)
//{
//	return (univ.out.sfx[i][j] & 8) ? true : false;
//}
//bool is_big_slime(short i,short j)
//{
//	return (univ.out.sfx[i][j] & 16) ? true : false;
//}
//bool is_ash(short i,short j)
//{
//	return (univ.out.sfx[i][j] & 32) ? true : false;
//}
//bool is_bones(short i,short j)
//{
//	return (univ.out.sfx[i][j] & 64) ? true : false;
//}
//bool is_rubble(short i,short j)
//{
//	return (univ.out.sfx[i][j] & 128) ? true : false;
//}
void make_sfx(short i,short j, short type)
{
	ter_num_t ter;
	
	if(sight_obscurity(i,j) > 0)
		return;
	ter = coord_to_ter(i,j);
	if(scenario.ter_types[ter].blockage != eTerObstruct::CLEAR)
		return;
	switch (type) {
//		case 1: case 2:
//			if (univ.out.sfx[i][j] == 4)
//				return;
//			if (univ.out.sfx[i][j] < 4)
//				type = min(3,type + univ.out.sfx[i][j]);
//			break;
//		case 4:
//			if (univ.out.sfx[i][j] == 8)
//				type = 5;
//			break;
		case 1:
			univ.town.set_sm_blood(i,j,true);
			break;
		case 2:
			univ.town.set_med_blood(i,j,true);
			break;
		case 3:
			univ.town.set_lg_blood(i,j,true);
			break;
		case 4:
			univ.town.set_sm_slime(i,j,true);
			break;
		case 5:
			univ.town.set_lg_slime(i,j,true);
			break;
		case 6:
			univ.town.set_ash(i,j,true);
			break;
		case 7:
			univ.town.set_bones(i,j,true);
			break;
		case 8:
			univ.town.set_rubble(i,j,true);
			break;
	}
//	univ.out.sfx[i][j] = s_pow(2,type - 1);
}

void spread_sfx(short type,short prob)
// prob is 1 - 100
{
	short i,j;
	
	for (i = 0; i < univ.town->max_dim(); i++)
		for (j = 0; j < univ.town->max_dim(); j++)
			if ((univ.town->terrain(i,j) < 5) && (get_ran(1,1,100) <= prob))
				make_sfx(i,j,type);
	
}
