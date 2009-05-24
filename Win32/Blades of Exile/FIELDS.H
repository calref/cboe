#ifndef _FIELDS_H
	#define _FIELDS_H
	
#include "global.h"
#include "globvar.h"

inline Boolean is_web(short i,short j) { return (misc_i[i][j] & 4) ? TRUE : FALSE; }
inline void take_web(short i,short j) { misc_i[i][j] &= 251; }
inline Boolean is_crate(short i,short j) { return (misc_i[i][j] & 8) ? TRUE : FALSE; }
inline void make_crate(short i,short j) { misc_i[i][j] = misc_i[i][j] | 8; crate = TRUE; }
inline void take_crate(short i,short j) { misc_i[i][j] &= 247; }
inline Boolean is_barrel(short i,short j) {	return (misc_i[i][j] & 16) ? TRUE : FALSE; }
inline void make_barrel(short i,short j) { misc_i[i][j] = misc_i[i][j] | 16; barrel = TRUE; }
inline void take_barrel(short i,short j) { misc_i[i][j] &= 239; }
inline Boolean is_fire_barrier(short i,short j) { return (misc_i[i][j] & 32) ? TRUE : FALSE; }
inline void make_special(short i,short j) { misc_i[i][j] =  misc_i[i][j] | (char) (2); }
inline void take_special(short i,short j) { misc_i[i][j] &= 253; }
inline Boolean is_small_blood(short i,short j) { return (sfx[i][j] & 1) ? TRUE : FALSE; }
inline Boolean is_medium_blood(short i,short j) { return (sfx[i][j] & 2) ? TRUE : FALSE; }
inline Boolean is_large_blood(short i,short j) { return (sfx[i][j] & 4) ? TRUE : FALSE; }
inline Boolean is_small_slime(short i,short j) { return (sfx[i][j] & 8) ? TRUE : FALSE; }
inline Boolean is_big_slime(short i,short j) { return (sfx[i][j] & 16) ? TRUE : FALSE; }
inline Boolean is_ash(short i,short j) { return (sfx[i][j] & 32) ? TRUE : FALSE; }
inline Boolean is_bones(short i,short j) { return (sfx[i][j] & 64) ? TRUE : FALSE; }
inline Boolean is_rubble(short i,short j) {	return (sfx[i][j] & 128) ? TRUE : FALSE; }

inline void take_fire_barrier(short i,short j) { misc_i[i][j] &= 223; }
inline Boolean is_force_barrier(short i,short j) { return (misc_i[i][j] & 64) ? TRUE : FALSE; }
inline void take_force_barrier(short i,short j) { misc_i[i][j] &= 191; }
inline Boolean is_quickfire(short i,short j) { return (misc_i[i][j] & 128) ? TRUE : FALSE; }
inline void take_quickfire(short i,short j) { misc_i[i][j] &= 127; }
inline Boolean is_force_wall(short i,short j) { return (c_town.explored[i][j] & 2) ? TRUE : FALSE; }
inline void take_force_wall(short i,short j) { c_town.explored[i][j] &= 253; }
inline Boolean is_fire_wall(short i,short j) { return (c_town.explored[i][j] & 4) ? TRUE : FALSE; }
inline void take_fire_wall(short i,short j) { c_town.explored[i][j] &= 251; }
inline Boolean is_antimagic(short i,short j) { return (c_town.explored[i][j] & 8) ? TRUE : FALSE; }
inline void take_antimagic(short i,short j) { c_town.explored[i][j] &= 247; }
inline Boolean is_scloud(short i,short j) {	return (c_town.explored[i][j] & 16) ? TRUE : FALSE; }
inline void take_scloud(short i,short j) { c_town.explored[i][j] &= 239; }
inline Boolean is_ice_wall(short i,short j) { return (c_town.explored[i][j] & 32) ? TRUE : FALSE; }
inline void take_ice_wall(short i,short j) { c_town.explored[i][j] &= 223; }
inline Boolean is_blade_wall(short i,short j) { return (c_town.explored[i][j] & 64) ? TRUE : FALSE; }
inline void take_blade_wall(short i,short j) { c_town.explored[i][j] &= 191; }
inline Boolean is_sleep_cloud(short i,short j) { return (c_town.explored[i][j] & 128) ? TRUE : FALSE; }
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
