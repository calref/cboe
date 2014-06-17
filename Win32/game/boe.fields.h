#ifndef _FIELDS_H
	#define _FIELDS_H

#include "global.h"
#include "globvar.h"

/*  explored :

	0000 0001	is the space explored ?
	0000 0010	force wall is present
	0000 0100	fire wall is present
	0000 1000	antimagic field is present
	0001 0000	stink cloud is present
	0010 0000	ice wall is present
	0100 0000	blade wall is present
	1000 0000	sleep cloud is present

    misc_i :

	0000 0001	<not used>
	0000 0010	special is present
	0000 0100	web is present
	0000 1000	crate is present
	0001 0000	barrel is present
	0010 0000	fire barrier is present
	0100 0000	force barrier is present
	1000 0000	quickfire is present

    sfx :

	0000 0001	small blood
	0000 0010	medium blood
	0000 0100	large blood
	0000 1000	small slime
	0001 0000	big slime
	0010 0000	ash
	0100 0000	bones
	1000 0000	rubble
*/

inline Boolean is_web(short i,short j) { return (misc_i[i][j] & 4) ? true : false; }
inline void take_web(short i,short j) { misc_i[i][j] &= 251; }
inline Boolean is_crate(short i,short j) { return (misc_i[i][j] & 8) ? true : false; }
inline void make_crate(short i,short j) { misc_i[i][j] = misc_i[i][j] | 8; crate = true; }
inline void take_crate(short i,short j) { misc_i[i][j] &= 247; }
inline Boolean is_barrel(short i,short j) {	return (misc_i[i][j] & 16) ? true : false; }
inline void make_barrel(short i,short j) { misc_i[i][j] = misc_i[i][j] | 16; barrel = true; }
inline void take_barrel(short i,short j) { misc_i[i][j] &= 239; }
inline Boolean is_fire_barrier(short i,short j) { return (misc_i[i][j] & 32) ? true : false; }
inline void make_special(short i,short j) { misc_i[i][j] =  misc_i[i][j] | (char) (2); }
inline void take_special(short i,short j) { misc_i[i][j] &= 253; }
inline Boolean is_small_blood(short i,short j) { return (sfx[i][j] & 1) ? true : false; }
inline Boolean is_medium_blood(short i,short j) { return (sfx[i][j] & 2) ? true : false; }
inline Boolean is_large_blood(short i,short j) { return (sfx[i][j] & 4) ? true : false; }
inline Boolean is_small_slime(short i,short j) { return (sfx[i][j] & 8) ? true : false; }
inline Boolean is_big_slime(short i,short j) { return (sfx[i][j] & 16) ? true : false; }
inline Boolean is_ash(short i,short j) { return (sfx[i][j] & 32) ? true : false; }
inline Boolean is_bones(short i,short j) { return (sfx[i][j] & 64) ? true : false; }
inline Boolean is_rubble(short i,short j) {	return (sfx[i][j] & 128) ? true : false; }

inline void take_fire_barrier(short i,short j) { misc_i[i][j] &= 223; }
inline Boolean is_force_barrier(short i,short j) { return (misc_i[i][j] & 64) ? true : false; }
inline void take_force_barrier(short i,short j) { misc_i[i][j] &= 191; }
inline Boolean is_quickfire(short i,short j) { return (misc_i[i][j] & 128) ? true : false; }
inline void take_quickfire(short i,short j) { misc_i[i][j] &= 127; }
inline Boolean is_force_wall(short i,short j) { return (c_town.explored[i][j] & 2) ? true : false; }
inline void take_force_wall(short i,short j) { c_town.explored[i][j] &= 253; }
inline Boolean is_fire_wall(short i,short j) { return (c_town.explored[i][j] & 4) ? true : false; }
inline void take_fire_wall(short i,short j) { c_town.explored[i][j] &= 251; }
inline Boolean is_antimagic(short i,short j) { return (c_town.explored[i][j] & 8) ? true : false; }
inline void take_antimagic(short i,short j) { c_town.explored[i][j] &= 247; }
inline Boolean is_scloud(short i,short j) {	return (c_town.explored[i][j] & 16) ? true : false; }//stinking cloud
inline void take_scloud(short i,short j) { c_town.explored[i][j] &= 239; }
inline Boolean is_ice_wall(short i,short j) { return (c_town.explored[i][j] & 32) ? true : false; }
inline void take_ice_wall(short i,short j) { c_town.explored[i][j] &= 223; }
inline Boolean is_blade_wall(short i,short j) { return (c_town.explored[i][j] & 64) ? true : false; }
inline void take_blade_wall(short i,short j) { c_town.explored[i][j] &= 191; }
inline Boolean is_sleep_cloud(short i,short j) { return (c_town.explored[i][j] & 128) ? true : false; }
inline void take_sleep_cloud(short i,short j) { c_town.explored[i][j] &= 127; }

Boolean is_explored(short i,short j);
void make_explored(short i,short j);
Boolean is_out();
Boolean is_town();
Boolean is_combat();
Boolean special(short i,short j);
void make_web(short i,short j);
void make_fire_barrier(short i,short j);
void make_force_barrier(short i,short j);
void make_quickfire(short i,short j);
void make_force_wall(short i,short j);
void make_fire_wall(short i,short j);
void make_antimagic(short i,short j);
void make_scloud(short i,short j);
void make_ice_wall(short i,short j);
void make_blade_wall(short i,short j);
void make_sleep_cloud(short i,short j);
void make_sfx(short i,short j, short type);

#endif
