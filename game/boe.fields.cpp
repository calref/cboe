#include <windows.h>

#include "global.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "globvar.h"
#include "tools/mathutil.h"

Boolean is_explored(short i,short j)
{
	if (is_out())  {
		if ((i != minmax(0,95,(int)i)) || (j != minmax(0,95,(int)j)))
      		return false;
		return (out_e[i][j] != 0) ? true : false;
		}
	if (c_town.explored[i][j] & 1) return true;
	else return false;
}
void make_explored(short i,short j)
{
	if (is_out()) out_e[i][j] = 1;
	c_town.explored[i][j] |= 1;
}

Boolean is_out()
{
	return ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS))? true : false;
}
Boolean is_town()
{
	return (((overall_mode > MODE_OUTDOORS) && (overall_mode < MODE_COMBAT)) || (overall_mode == MODE_LOOK_TOWN))? true : false;
}
Boolean is_combat()
{
	return (((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING)) || (overall_mode == MODE_LOOK_COMBAT))? true : false;
}

Boolean special(short i,short j)
{
	if (((misc_i[i][j]) & 2) != 0)
		return true;
		else return false;
}
void make_web(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
	if ((misc_i[i][j] & 224) || (c_town.explored[i][j] & 238))
		return;
	misc_i[i][j] |= 4;
	web = true;
}

void make_fire_barrier(short i,short j)
/**/{
	if ((is_antimagic(i,j)) && (get_ran(1,0,3) < 3))
		return;
	if (misc_i[i][j] & 248)
		return;
	take_web(i,j);
	c_town.explored[i][j] = c_town.explored[i][j] & 1;
	misc_i[i][j] |= 32;
	fire_barrier = true;
}

void make_force_barrier(short i,short j)
/**/{
	if ((is_antimagic(i,j)) && (get_ran(1,0,2) < 2))
		return;
	if (misc_i[i][j] & 248)
		return;
	take_web(i,j);
	c_town.explored[i][j] = c_town.explored[i][j] & 1;
	misc_i[i][j] |= 64;
	force_barrier = true;
}

void make_quickfire(short i,short j)
/**/{
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
	misc_i[i][j] &= 3;
	misc_i[i][j] |= 128;
	quickfire = true;
}

void make_force_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
	if ((c_town.explored[i][j] & 74) || (misc_i[i][j] & 248))
		return;
	take_web(i,j);
	take_fire_wall(i,j);
	c_town.explored[i][j] |= 2;
	force_wall = true;
}

void make_fire_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
	if ((c_town.explored[i][j] & 248) || (misc_i[i][j] & 254))
		return;
	take_web(i,j);
	c_town.explored[i][j] |= 4;
	fire_wall = true;
}

void make_antimagic(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
	if (misc_i[i][j] & 224)
		return;

	c_town.explored[i][j] &= 1;
	c_town.explored[i][j] |= 8;
	antimagic = true;
}

void make_scloud(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;

	if ((c_town.explored[i][j] & 238) || (misc_i[i][j] & 224))
		return;
	c_town.explored[i][j] |= 16;
	scloud = true;
}

void make_ice_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
	if ((c_town.explored[i][j] & 74) || (misc_i[i][j] & 252))
		return;
	take_fire_wall(i,j);
	take_scloud(i,j);
	c_town.explored[i][j] |= 32;
	ice_wall = true;
}

void make_blade_wall(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
	if ((c_town.explored[i][j] & 8) || (misc_i[i][j] & 224))
		return;
	c_town.explored[i][j] &= 9;
	c_town.explored[i][j] |= 64;
	blade_wall = true;
}

void make_sleep_cloud(short i,short j)
/**/{
	if (spot_impassable(i,j) == true)
		return;
	if ((c_town.explored[i][j] & 8) || (misc_i[i][j] & 224))
		return;
	c_town.explored[i][j] &= 9;
	c_town.explored[i][j] |= 128;
	sleep_field = true;
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
