
#include "mathutil.h"
#include "scen.global.h"
#include "scen.locutils.h"
//#include "boe.monster.h"
//#include "boe.fields.h"
#include "scen.graphics.h"
#include "mathutil.h"


char terrain_blocked[256];


short short_can_see();
Boolean combat_pt_in_light();
//extern short town_size[3];
location obs_sec;
location which_party_sec;

//extern party_record_type party;
//extern current_town_type c_town;
extern short overall_mode/*,which_combat_type*/,current_pc,town_type;
extern big_tr_type t_d;
//extern outdoor_record_type outdoors[2][2];
extern unsigned char combat_terrain[64][64];
extern unsigned char out[96][96], out_e[96][96];
extern location pc_pos[6],center;
//extern pc_record_type adven[6];
extern Boolean belt_present,web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern unsigned char map_graphic_placed[8][64]; // keeps track of what's been filled on map
extern scenario_data_type scenario;
extern bool editing_town;
extern outdoor_record_type current_terrain;

location light_locs[40];
short num_lights = 0;
char d_s[60];

////
void set_terrain_blocked()
{
	short i;
	
	for (i = 0; i < 256; i++)
		terrain_blocked[i] = scenario.ter_types[i].blockage;
}

short vdist(location p1,location p2) {
	short i,j;
	i = abs((short) (p1.x - p2.x)); j = abs((short) (p1.y - p2.y));
	return max(i,j);
}

Boolean adjacent(location p1,location p2)
{
	if ((abs((short) (p1.x - p2.x)) <= 1) && (abs((short) (p1.y - p2.y)) <= 1))
		return TRUE;
		else return FALSE;
}

Boolean point_onscreen(location center,location check)
{
	if ((abs((short) (center.x - check.x)) <=4) && (abs((short) (center.y - check.y)) <= 4))
		return TRUE;
		else return FALSE;
}


short set_direction (location old_pt, location new_pt)
{
	if (old_pt.x == new_pt.x)
		if (old_pt.y > new_pt.y)
			return 0;
		else return 4;
	if (old_pt.x > new_pt.x) {
		if (old_pt.y > new_pt.y)
			return 7;
		if (old_pt.y < new_pt.y)
			return 5;
			return 6;
		}
	if (old_pt.y > new_pt.y)
		return 1;
	if (old_pt.y < new_pt.y)
		return 3;
	return 2;	
}
Boolean is_container(location loc)
{
	unsigned char ter;
	
	if ((is_barrel(loc.x,loc.y)) || (is_crate(loc.x,loc.y)))
		return TRUE;
	ter = coord_to_ter(loc.x,loc.y);
	if (scenario.ter_types[ter].special == 14)
			return TRUE;
	return FALSE;
}

Boolean special_which_blocks_monst(location to_check)
{
		if (terrain_blocked[coord_to_ter(to_check.x,to_check.y)] == 2)
			return TRUE;
			else return FALSE;
}

// Checks if space is a special that prevents movement into or placement of a PC on
Boolean is_special(location to_check)
{
	unsigned char which_ter;

	if (special_which_blocks_monst(to_check) == FALSE)
		return FALSE;
	which_ter = coord_to_ter(to_check.x,to_check.y);
	if (terrain_blocked[which_ter] == 2)
			return TRUE;
			else return FALSE;
}

Boolean impassable(unsigned char terrain_to_check)
{
	if (terrain_blocked[terrain_to_check] > 2)
		return TRUE;
		else return FALSE;
}

Boolean spot_impassable(short i,short  j)
{
	unsigned char ter;

	ter = coord_to_ter(i,j);
	if (terrain_blocked[ter] == 5)
		return TRUE;
		else return FALSE;
}