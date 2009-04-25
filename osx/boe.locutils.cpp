
//#include "item.h"

#include "mathutil.h"
#include "boe.global.h"
#include "classes.h"
#include "boe.locutils.h"
#include "boe.text.h"
#include "boe.monster.h"
#include "boe.fields.h"


char terrain_blocked[256];


short short_can_see();
Boolean combat_pt_in_light();
extern short town_size[3];
location obs_sec;
location which_party_sec;

//extern party_record_type party;
//extern current_town_type univ.town;
extern short overall_mode,which_combat_type,current_pc,town_type;
//extern big_tr_type t_d;
//extern cOutdoors outdoors[2][2];
extern unsigned char combat_terrain[64][64];
//extern unsigned char out[96][96], out_e[96][96];
extern location pc_pos[6],center;
//extern pc_record_type ADVEN[6];
extern Boolean belt_present,web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern unsigned char map_graphic_placed[8][64]; // keeps track of what's been filled on map
extern cScenario scenario;
extern cUniverse univ;

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

short dist(location p1,location p2)
{
	return s_sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
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

location global_to_local(location global)
{
	location local;
	
	local = global;
	if (univ.party.i_w_c.x == 1)
		local.x = local.x - 48; 
	if (univ.party.i_w_c.y == 1)
		local.y = local.y - 48;
	return local; 
}
location local_to_global(location local)
{
	location global;
	
	global = local;
	if (univ.party.i_w_c.x == 1)
		global.x = global.x + 48; 
	if (univ.party.i_w_c.y == 1)
		global.y = global.y + 48;
	return global; 
}
Boolean loc_off_world(location p1)
{
	if ((p1.x < 0) || (p1.x > town_size[town_type]) || (p1.y < 0) || (p1.y > town_size[town_type]))
		return TRUE;
		else return FALSE;
}

Boolean loc_off_act_area(location p1)
{
	if ((p1.x > univ.town.town->in_town_rect.left) && (p1.x < univ.town.town->in_town_rect.right) &&
	 (p1.y > univ.town.town->in_town_rect.top) && (p1.y < univ.town.town->in_town_rect.bottom))
	 	return FALSE;
	return TRUE;
}

location get_cur_loc()
{
	switch (overall_mode) {
		case MODE_OUTDOORS: case MODE_LOOK_OUTDOORS:
			return univ.party.p_loc;
			break;
			
		case MODE_TOWN: case MODE_TALK_TOWN: case MODE_TOWN_TARGET: case MODE_USE: case MODE_LOOK_TOWN:
			return univ.town.p_loc;
			break;
			
		default:
			return pc_pos[current_pc];
			break;	
		}
}

short short_can_see(shortloc p1,shortloc p2)
{
	location s1,s2;
	
	s1.x = (char) p1.x;
	s1.y = (char) p1.y;
	s2.x = (char) p2.x;
	s2.y = (char) p2.y;
	return (can_see(s1,s2,0));
}

Boolean is_lava(short x,short y)////
{
	unsigned char ter;
	
	ter = coord_to_ter(x,y);
	if (scenario.ter_types[ter].picture == 404)
		return TRUE;
		else return FALSE;
}

short can_see(location p1,location p2,short mode)
//short mode; // 0 - normal  1 - counts 1 for blocked spaces or lava (used for party placement in
			//				   town combat)
			// 2 - no light check
{
	short dx,dy,count,storage = 0;

	if (is_combat()) { // Light check
		if ((mode != 2) && (combat_pt_in_light(p2) == FALSE)) {
			return 6;
			}
		}
		else {
		if ((mode != 2) && (pt_in_light(p1,p2) == FALSE)) {
			return 6;
			}
		}

	if (p1.y == p2.y) {
		if (p1.x > p2.x) {
			for (count = p2.x + 1; count < p1.x; count++) {
				storage = storage + get_obscurity(count, p1.y);
				if (((terrain_blocked[coord_to_ter(count,p1.y)] > 2) || (is_lava(count,p1.y) == TRUE)) && (mode == 1))
					return 5;
				}
			}
			else {
				for (count = p1.x + 1; count < p2.x; count++) {

				storage = storage + get_obscurity(count, p1.y);
				if (((terrain_blocked[coord_to_ter(count,p1.y)] > 2) || (is_lava(count,p1.y) == TRUE)) && (mode == 1))
					return 5;
				}
			}
			return storage;
		}
	if (p1.x == p2.x) {
		if (p1.y > p2.y) {
			for (count = p1.y - 1; count > p2.y; count--) {
				storage = storage + get_obscurity(p1.x, count);
				if (((terrain_blocked[coord_to_ter(p1.x,count)] > 2) || (is_lava(p1.x,count) == TRUE)) && (mode == 1))
					return 5;
				}
			}
			else {
				for (count = p1.y + 1; count < p2.y; count++) {
					storage = storage + get_obscurity(p1.x, count);
					if (((terrain_blocked[coord_to_ter(p1.x,count)] > 2) || (is_lava(p1.x,count) == TRUE))  && (mode == 1))
						return 5;
					}
			}
		return storage;	
		}
	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	
	if (abs(dy) > abs(dx)) {
		if (p2.y > p1.y) {
			for (count = 1; count < dy; count++) {
				storage = storage + get_obscurity(p1.x + (count * dx) / dy, p1.y + count);
				if ( ((terrain_blocked[coord_to_ter(p1.x + (count * dx) / dy,p1.y + count)] > 2) ||
					(is_lava(p1.x + (count * dx) / dy,p1.y + count) == TRUE))
					 && (mode == 1))
					return 5;
				}			
			}
		else {
			for (count = -1; count > dy; count--) {
				storage = storage + get_obscurity(p1.x + (count * dx) / dy, p1.y + count);
				if ( ((terrain_blocked[coord_to_ter(p1.x + (count * dx) / dy, p1.y + count)] > 2) ||
					(is_lava(p1.x + (count * dx) / dy, p1.y + count) == TRUE))
					&& (mode == 1))
					return 5;				
				}
			}
		return storage;
		} 
	if (abs(dy) <= abs(dx)) {
		if (p2.x > p1.x) {
			for (count = 1; count < dx; count++) {
				storage = storage + get_obscurity(p1.x + count, p1.y + (count * dy) / dx);
				if (((terrain_blocked[coord_to_ter(p1.x + count,p1.y + (count * dy) / dx)] > 2) ||
					(is_lava(p1.x + count,p1.y + (count * dy) / dx) == TRUE))
					&& (mode == 1))
					return 5;
				}
			}
		else {
			for (count = -1; count > dx; count--) {
				storage = storage + get_obscurity(p1.x + count, p1.y + (count * dy) / dx);
				if ( ((terrain_blocked[coord_to_ter(p1.x + count,p1.y + (count * dy) / dx)] > 2) ||
					(is_lava(p1.x + count,p1.y + (count * dy) / dx) == TRUE))
					&& (mode == 1))
					return 5;
				}
			}
		return storage;
		} 
	if (storage > 5)
		return 5;
		else return storage;
}

short get_obscurity(short x,short y)
{
	unsigned char what_terrain;
	short store;
	
	what_terrain = coord_to_ter(x,y);
	
	if ((what_terrain >= 237) && (what_terrain <= 242))
		return 1;
		
	store = get_blockage(what_terrain);
	
	if (is_town()) {
		if (special(x,y))
			store++;
		}

	if ((is_town()) || (is_combat())) {
		if (is_web(x,y))
			store += 2;
		if ((is_fire_barrier(x,y)) || (is_force_barrier(x,y)))
			return 5;
		if ((is_crate(x,y)) || (is_barrel(x,y)))
			store++;
	
		} 
	return store;
}

unsigned char coord_to_ter(short x,short y)
{
	char what_terrain;

	if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS))
		what_terrain = univ.out.out[x][y];
	else if (((overall_mode > MODE_OUTDOORS) && (overall_mode < MODE_COMBAT))|| (overall_mode == MODE_LOOK_TOWN))
		what_terrain = univ.town.town->terrain(x,y);
	else {
		what_terrain = combat_terrain[x][y];
		}
	return what_terrain;
}

////
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

void update_explored(location dest)
{
	shortloc shortdest,look;

	location look2;

	shortdest.x = (short) dest.x;
	shortdest.y = (short) dest.y;
	
	which_party_sec.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
	which_party_sec.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
	
	if (overall_mode == MODE_OUTDOORS) {
		univ.out.out_e[dest.x][dest.y] = 2;
		for (look.x = shortdest.x - 4; look.x < shortdest.x + 5; look.x++)
			for (look.y = shortdest.y - 4; look.y < shortdest.y + 5; look.y++) {

			if (univ.out.out_e[look.x][look.y] == 0)
					if (short_can_see(shortdest, look) < 5)
						univ.out.out_e[look.x][look.y] = 1;
			}	
			

		}
		
	if (overall_mode > MODE_OUTDOORS) {
		make_explored(dest.x,dest.y);
		for (look2.x = max(0,dest.x - 4); look2.x < min(town_size[town_type],dest.x + 5); look2.x++)
			for (look2.y = max(0,dest.y - 4); look2.y < min(town_size[town_type],dest.y + 5); look2.y++)
				if (is_explored(look2.x,look2.y) == FALSE)
					if ((can_see(dest, look2,0) < 5) && (pt_in_light(dest,look2) == TRUE))
						make_explored(look2.x,look2.y);
		}
}


// All purpose function to check is spot is free for travel into.
Boolean is_blocked(location to_check)
{
	short i,gr;
	unsigned char ter;

	if (is_out()) {
		if (impassable(univ.out.out[to_check.x][to_check.y]) == TRUE) {
			return TRUE;
			}
		if (to_check == univ.party.p_loc)
			return TRUE;
		for (i = 0; i < 20; i++)
			if ((univ.party.out_c[i].exists) == TRUE)  
				if (univ.party.out_c[i].m_loc == to_check)
					return TRUE;
		return FALSE;
		}
		
	if ((is_town()) || (is_combat())) {
		ter = (is_town()) ? univ.town.town->terrain(to_check.x,to_check.y) : combat_terrain[to_check.x][to_check.y];
		gr = scenario.ter_types[ter].picture;
	
		////
		// Terrain blocking?
		if (impassable(ter) == TRUE) {
			return TRUE;
			}
			
		// Keep away from marked specials during combat
		if ((is_combat()) && (gr <= 212) && (gr >= 207))
			return TRUE;
		if ((is_combat()) && (gr == 406))
			return TRUE;
			
		// Party there?
		if (is_town())
			if (to_check == univ.town.p_loc)
				return TRUE;
		if (is_combat())
			for (i = 0; i < 6; i++)
				if ((ADVEN[i].main_status == 1) && (to_check == pc_pos[i]))
					return TRUE;
		
		// Monster there?
		if (monst_there(to_check) < 90)
			return TRUE;
		
		// Magic barrier?
		if (is_force_barrier(to_check.x,to_check.y))
			return TRUE;
			
		return FALSE;
		}
	return TRUE;
}

Boolean monst_on_space(location loc,short m_num)
{
	
	if (univ.town.monst.dudes[m_num].active == 0)
		return FALSE;
	if ((loc.x - univ.town.monst.dudes[m_num].m_loc.x >= 0) && 
	(loc.x - univ.town.monst.dudes[m_num].m_loc.x <= univ.town.monst.dudes[m_num].m_d.x_width - 1) &&
	(loc.y - univ.town.monst.dudes[m_num].m_loc.y >= 0) && 
	(loc.y - univ.town.monst.dudes[m_num].m_loc.y <= univ.town.monst.dudes[m_num].m_d.y_width - 1))
		return TRUE;
	return FALSE;	
	
}
short monst_there(location where) // returns 90 if no
{
	short i;

	for (i = 0; i < T_M; i++)
		if ((univ.town.monst.dudes[i].active != 0) && (monst_on_space(where,i) == TRUE))
			return i;
	return 90;	
}
Boolean monst_can_be_there(location loc,short m_num)
{
	short i,j;
	location destination;
	
	// First clear monst away so it doesn't block itself
	univ.town.monst.dudes[m_num].m_loc.x += 100;
	
	for (i = 0; i < univ.town.monst.dudes[m_num].m_d.x_width; i++)
		for (j = 0; j < univ.town.monst.dudes[m_num].m_d.y_width; j++) {
			destination.x = loc.x + i; destination.y = loc.y + j;
			if ((is_blocked(destination) == TRUE)
				|| (loc_off_act_area(destination) == TRUE)) {
					univ.town.monst.dudes[m_num].m_loc.x -= 100;
					return FALSE;
					}	
			}
	univ.town.monst.dudes[m_num].m_loc.x -= 100;			
	return TRUE;
}

Boolean monst_adjacent(location loc,short m_num)
{
	short i,j;
	location destination;
	
	for (i = 0; i < univ.town.monst.dudes[m_num].m_d.x_width; i++)
		for (j = 0; j < univ.town.monst.dudes[m_num].m_d.y_width; j++) {
			destination.x = univ.town.monst.dudes[m_num].m_loc.x + i; 
			destination.y = univ.town.monst.dudes[m_num].m_loc.y + j;
			if (adjacent(destination,loc) == TRUE)
				return TRUE;
			}
	return FALSE;
}

Boolean monst_can_see(short m_num,location l)
{
	short i,j;
	location destination;
		
	for (i = 0; i < univ.town.monst.dudes[m_num].m_d.x_width; i++)
		for (j = 0; j < univ.town.monst.dudes[m_num].m_d.y_width; j++) {
			destination.x = univ.town.monst.dudes[m_num].m_loc.x + i; 
			destination.y = univ.town.monst.dudes[m_num].m_loc.y + j;
			if (can_see(destination,l,0) < 5)
				return TRUE;
			}
	return FALSE;
}

Boolean party_can_see_monst(short m_num)
{
	short i,j;
	location destination;
		
	for (i = 0; i < univ.town.monst.dudes[m_num].m_d.x_width; i++)
		for (j = 0; j < univ.town.monst.dudes[m_num].m_d.y_width; j++) {
			destination.x = univ.town.monst.dudes[m_num].m_loc.x + i; 
			destination.y = univ.town.monst.dudes[m_num].m_loc.y + j;
			if (party_can_see(destination) < 6)
				return TRUE;
			}
	return FALSE;
}

Boolean can_see_monst(location l,short m_num)
{
	short i,j;
	location destination;
		
	for (i = 0; i < univ.town.monst.dudes[m_num].m_d.x_width; i++)
		for (j = 0; j < univ.town.monst.dudes[m_num].m_d.y_width; j++) {
			destination.x = univ.town.monst.dudes[m_num].m_loc.x + i; 
			destination.y = univ.town.monst.dudes[m_num].m_loc.y + j;
			if (can_see(l,destination,0) < 5)
				return TRUE;
			}
	return FALSE;
}

Boolean outd_is_blocked(location to_check)
{
	short i;

	if (overall_mode == MODE_OUTDOORS) {
		if (impassable(univ.out.out[to_check.x][to_check.y]) == TRUE) {
			return TRUE;
			}
		for (i = 0; i < 10; i++)
			if ((univ.party.out_c[i].exists) == TRUE)  				
				if (univ.party.out_c[i].m_loc == to_check)
					return TRUE;
		return FALSE;
		}
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

Boolean outd_is_special(location to_check)
{
	if (overall_mode == MODE_OUTDOORS) {
		if (terrain_blocked[univ.out.out[to_check.x][to_check.y]] == 2) {
			return TRUE;
			}
			else return FALSE;
		}
	return FALSE;
}

Boolean impassable(unsigned char terrain_to_check)
{
	if (terrain_blocked[terrain_to_check] > 2)
		return TRUE;
		else return FALSE;
}

short get_blockage(unsigned char terrain_type)
{
	// little kludgy in here for pits
	if ((terrain_type == 90) && (is_combat()) && (which_combat_type == 0))
		return 5;
	if ((terrain_blocked[terrain_type] == 5) || (terrain_blocked[terrain_type] == 1))
		return 5;
		else if (terrain_blocked[terrain_type] > 3)
			return 1;
			else {
				return 0;
				}
}



short light_radius()
{
	short store = 1,i;
	short extra_levels[6] = {10,20,50,75,110,140};
	
	if (((which_combat_type == 0) && (is_combat()))
		|| (is_out()) || (univ.town.town->lighting_type == 0))
				return 200;
	for (i = 0; i < 6; i++)
		if (univ.party.light_level > extra_levels[i])
			store++;
	return store;
}

Boolean pt_in_light(location from_where,location to_where) // Assumes, of course, in town or combat
{

	if (univ.town.town->lighting_type == 0)
		return TRUE;
	if ((to_where.x < 0) || (to_where.x >= town_size[town_type])
		|| (to_where.y < 0) || (to_where.y >= town_size[town_type]))
			return TRUE;
	if (univ.town.town->lighting(to_where.x / 8,to_where.y) & (char) (s_pow(2,to_where.x % 8)))
		return TRUE;
		
	if (dist(from_where,to_where) <= light_radius())
		return TRUE;

	return FALSE;
}

Boolean combat_pt_in_light(location to_where)
{
	short i,rad;

	if ((univ.town.town->lighting_type == 0) || (which_combat_type == 0))
		return TRUE;
	if ((to_where.x < 0) || (to_where.x >= town_size[town_type])
		|| (to_where.y < 0) || (to_where.y >= town_size[town_type]))
			return TRUE;
	if (univ.town.town->lighting(to_where.x / 8,to_where.y) & (char) (s_pow(2,to_where.x % 8)))
		return TRUE;	

	rad = light_radius();
	for (i = 0; i < 6; i++)
		if (ADVEN[i].main_status == 1) {
			if (dist(pc_pos[i],to_where) <= rad)
				return TRUE;
			}

	return FALSE;
}

Boolean party_sees_a_monst() // Returns true is a hostile monster is in sight.
{
	short i;
	
	for (i = 0; i < T_M; i++) {
		if (univ.town.monst.dudes[i].active > 0)
			if ((univ.town.monst.dudes[i].attitude == 1) &&
				(party_can_see_monst(i) == TRUE))
					return TRUE;
		}
	
	return FALSE;
}


// Returns 6 if can't see, O.W. returns the # of a PC that can see
short party_can_see(location where)
{
	short i;
	
	if (is_out()) {
		if ((point_onscreen(univ.party.p_loc,where) == TRUE) && (can_see(univ.party.p_loc,where,0) < 5))
			return 1;
			else return 6;
			}
	if (is_town()) {
		if ( ((point_onscreen(univ.town.p_loc,where) == TRUE) || (overall_mode == MODE_LOOK_TOWN)) && (pt_in_light(univ.town.p_loc,where)  == TRUE)
			&& (can_see(univ.town.p_loc,where,0) < 5))
				return 1;
				else return 6;
		}
		
	// Now for combat checks. Doing separately for efficiency. Check first for light. If
	//   dark, give up.
	if ((which_combat_type != 0) && (combat_pt_in_light(where) == FALSE))
		return 6;

	for (i = 0; i < 6; i++)
		if (ADVEN[i].main_status == 1) {
			if (can_see(pc_pos[i],where,0) < 5)
					return i;
			}

	return 6;
}
location push_loc(location from_where,location to_where)
{
	location loc_to_try;
	
	loc_to_try = to_where;
	loc_to_try.x = loc_to_try.x + (to_where.x - from_where.x);
	loc_to_try.y = loc_to_try.y + (to_where.y - from_where.y);
	if ((univ.town.town->terrain(loc_to_try.x,loc_to_try.y) == 90) || 
		 ((univ.town.town->terrain(loc_to_try.x,loc_to_try.y) >= 50)&& (univ.town.town->terrain(loc_to_try.x,loc_to_try.y) <= 64))
		 || (univ.town.town->terrain(loc_to_try.x,loc_to_try.y) == 71) 
		 || ((univ.town.town->terrain(loc_to_try.x,loc_to_try.y) >= 74)&& (univ.town.town->terrain(loc_to_try.x,loc_to_try.y) <= 78))		 
		 ) { 
			// Destroy crate
			loc_to_try.x = 0;
			return loc_to_try;
			}
	if ((get_obscurity((short) loc_to_try.x,(short) loc_to_try.y) > 0) ||
	    (terrain_blocked[univ.town.town->terrain(loc_to_try.x,loc_to_try.y)] > 0) ||
		(loc_off_act_area(loc_to_try) == TRUE) ||
		(monst_there(loc_to_try) < 90) ||
		(pc_there(loc_to_try) < 6))
		return from_where;
		else return loc_to_try;
}



Boolean spot_impassable(short i,short  j)
{
	unsigned char ter;

	ter = coord_to_ter(i,j);
	if (terrain_blocked[ter] == 5)
		return TRUE;
		else return FALSE;
}

void swap_ter(short i,short j,unsigned char ter1,unsigned char ter2)
{
	if (coord_to_ter(i,j) == ter1)
		alter_space(i,j,ter2);
		else if (coord_to_ter(i,j) == ter2)
		alter_space(i,j,ter1);
}

void alter_space(short i,short j,unsigned char ter)
{
	location l;
	
	l.x = i;
	l.y = j;
	map_graphic_placed[i / 8][j] = 0;
//	map_graphic_placed[i / 8][j] = 
//		map_graphic_placed[i / 8][j] & ~((unsigned char)(s_pow(2,i % 8)));
	
	if (is_out()) {
		l = local_to_global(l);
		univ.out.out[l.x][l.y] = ter;
		univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].terrain[i][j] = ter;
		}
		else {
			univ.town.town->terrain(i,j) = ter;
			combat_terrain[i][j] = ter;
			if ((scenario.ter_types[univ.town.town->terrain(i,j)].special >= 16) &&
				(scenario.ter_types[univ.town.town->terrain(i,j)].special <= 19))
					belt_present = TRUE;	
			}
}