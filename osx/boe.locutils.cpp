
//#include "item.h"

#include "mathutil.h"
#include "boe.global.h"

#include "classes.h"
#include "boe.locutils.h"
#include "boe.text.h"
#include "boe.monster.h"
#include "boe.fields.h"

bool combat_pt_in_light();
//extern short town_size[3];
location obs_sec;
location which_party_sec;

//extern party_record_type party;
//extern current_town_type univ.town;
extern short which_combat_type,current_pc,town_type;
extern eGameMode overall_mode;
//extern big_tr_type t_d;
//extern cOutdoors outdoors[2][2];
extern ter_num_t combat_terrain[64][64];
//extern unsigned char out[96][96], out_e[96][96];
extern location pc_pos[6],center;
extern bool belt_present,web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern unsigned char map_graphic_placed[8][64]; // keeps track of what's been filled on map
extern cScenario scenario;
extern cUniverse univ;

location light_locs[40];
short num_lights = 0;
char d_s[60];

//
//void set_terrain_blocked()
//{
//	short i;
//
//	for (i = 0; i < 256; i++)
//		terrain_blocked[i] = scenario.ter_types[i].blockage;
//}

//short dist(location p1,location p2)
//{
//	return s_sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
//}
//
//short vdist(location p1,location p2) {
//	short i,j;
//	i = abs((short) (p1.x - p2.x)); j = abs((short) (p1.y - p2.y));
//	return max(i,j);
//}

bool adjacent(location p1,location p2)
{
	if ((abs((short) (p1.x - p2.x)) <= 1) && (abs((short) (p1.y - p2.y)) <= 1))
		return true;
	else return false;
}

bool point_onscreen(location center,location check)
{
	if ((abs((short) (center.x - check.x)) <=4) && (abs((short) (center.y - check.y)) <= 4))
		return true;
	else return false;
}


short set_direction (location old_pt, location new_pt)
{
	if (old_pt.x == new_pt.x)
		if (old_pt.y > new_pt.y)
			return DIR_N;
		else return DIR_S;
	if (old_pt.x > new_pt.x) {
		if (old_pt.y > new_pt.y)
			return DIR_NW;
		if (old_pt.y < new_pt.y)
			return DIR_SW;
		return DIR_W;
	}
	if (old_pt.y > new_pt.y)
		return DIR_NE;
	if (old_pt.y < new_pt.y)
		return DIR_SE;
	return DIR_E;
}

location global_to_local(location global)
{
	location local;
	
	local = global;
	if(global.x >= 48)
		local.x = local.x - 48;
	if(global.y >= 48)
		local.y = local.y - 48;
	return local;
}
// TODO: I fixed the above function, but it's impossible to fix this one without adding a parameter (which would be i_w_c)
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
bool loc_off_world(location p1)
{
	if ((p1.x < 0) || (p1.x > univ.town->max_dim()) || (p1.y < 0) || (p1.y > univ.town->max_dim()))
		return true;
	else return false;
}

bool loc_off_act_area(location p1)
{
	if ((p1.x > univ.town->in_town_rect.left) && (p1.x < univ.town->in_town_rect.right) &&
		(p1.y > univ.town->in_town_rect.top) && (p1.y < univ.town->in_town_rect.bottom))
	 	return false;
	return true;
}

location get_cur_loc()
{
	switch (overall_mode) {
		case MODE_OUTDOORS: case MODE_LOOK_OUTDOORS:
			return univ.party.p_loc;
			break;
			
		case MODE_TOWN: case MODE_TALK_TOWN: case MODE_TOWN_TARGET: case MODE_USE_TOWN: case MODE_LOOK_TOWN:
			return univ.town.p_loc;
			break;
			
		default:
			return pc_pos[current_pc];
			break;
	}
}

bool is_lava(short x,short y)////
{
	ter_num_t ter;
	
	ter = coord_to_ter(x,y);
	if (scenario.ter_types[ter].picture == 404)
		return true;
	else return false;
}

short can_see_light(location p1, location p2, std::function<short(short,short)> get_obscurity) {
	if(is_combat() && !combat_pt_in_light(p2)) return 6;
	else if(is_town() && !pt_in_light(p1,p2)) return 6;
	return can_see(p1, p2, get_obscurity);
}

short sight_obscurity(short x,short y) {
	ter_num_t what_terrain;
	short store;
	
	what_terrain = coord_to_ter(x,y);
	
	// TODO: This should not be hard-coded!
	if ((what_terrain >= 237) && (what_terrain <= 242))
		return 1;
	
	store = get_blockage(what_terrain);
	
	if (is_town()) {
		if (univ.town.is_special(x,y))
			store++;
	}
	
	if ((is_town()) || (is_combat())) {
		if (univ.town.is_web(x,y))
			store += 2;
		if ((univ.town.is_fire_barr(x,y)) || (univ.town.is_force_barr(x,y)))
			return 5;
		if(univ.town.is_crate(x,y) || univ.town.is_barrel(x,y) || univ.town.is_block(x,y))
			store++;
		
	}
	return store;
}

short combat_obscurity(short x, short y) {
	if(blocksMove(scenario.ter_types[coord_to_ter(x,y)].blockage)) return 5;
	if(is_lava(x,y)) return 5;
	return sight_obscurity(x,y);
}

ter_num_t coord_to_ter(short x,short y)
{
	ter_num_t what_terrain;
	
	if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS))
		what_terrain = univ.out[x][y];
	else if (((overall_mode > MODE_OUTDOORS) && (overall_mode < MODE_COMBAT))|| (overall_mode == MODE_LOOK_TOWN))
		what_terrain = univ.town->terrain(x,y);
	else
		what_terrain = combat_terrain[x][y];\
	return what_terrain;
}

////
bool is_container(location loc)
{
	ter_num_t ter;
	
	if ((univ.town.is_barrel(loc.x,loc.y)) || (univ.town.is_crate(loc.x,loc.y)))
		return true;
	ter = coord_to_ter(loc.x,loc.y);
	if (scenario.ter_types[ter].special == TER_SPEC_IS_A_CONTAINER)
		return true;
	return false;
}

void update_explored(location dest)
{
	location shortdest,look;
	
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
					if(can_see_light(shortdest, look, sight_obscurity) < 5)
						univ.out.out_e[look.x][look.y] = 1;
			}
		
		
	}
	
	if (overall_mode > MODE_OUTDOORS) {
		make_explored(dest.x,dest.y);
		for (look2.x = max(0,dest.x - 4); look2.x < min(univ.town->max_dim(),dest.x + 5); look2.x++)
			for (look2.y = max(0,dest.y - 4); look2.y < min(univ.town->max_dim(),dest.y + 5); look2.y++)
				if (is_explored(look2.x,look2.y) == false)
					if ((can_see(dest, look2,0) < 5) && (pt_in_light(dest,look2) == true))
						make_explored(look2.x,look2.y);
	}
}


// All purpose function to check is spot is free for travel into.
bool is_blocked(location to_check)
{
	short i,gr;
	ter_num_t ter;
	
	if (is_out()) {
		if (impassable(univ.out[to_check.x][to_check.y]) == true) {
			return true;
		}
		if (to_check == univ.party.p_loc)
			return true;
		for (i = 0; i < 20; i++)
			if ((univ.party.out_c[i].exists) == true)
				if (univ.party.out_c[i].m_loc == to_check)
					return true;
		return false;
	}
	
	if ((is_town()) || (is_combat())) {
		ter = (is_town()) ? univ.town->terrain(to_check.x,to_check.y) : combat_terrain[to_check.x][to_check.y];
		gr = scenario.ter_types[ter].picture;
		
		////
		// Terrain blocking?
		if (impassable(ter) == true) {
			return true;
		}
		
		// Keep away from marked specials during combat
		if ((is_combat()) && univ.town.is_spot(to_check.x, to_check.y))
			return true;
		if ((is_combat()) && (scenario.ter_types[coord_to_ter(to_check.x,to_check.y)].trim_type == eTrimType::CITY))
			return true; // TODO: Maybe replace eTrimType::CITY with a blockage == clear/special && is_special() check
		
		// Party there?
		if (is_town())
			if (to_check == univ.town.p_loc)
				return true;
		if (is_combat())
			for (i = 0; i < 6; i++)
				if ((univ.party[i].main_status == 1) && (to_check == pc_pos[i]))
					return true;
		
		// Monster there?
		if (monst_there(to_check) < 90)
			return true;
		
		// Magic barrier?
		if (univ.town.is_force_barr(to_check.x,to_check.y))
			return true;
		
		return false;
	}
	return true;
}

bool monst_on_space(location loc,short m_num)
{
	
	if (univ.town.monst[m_num].active == 0)
		return false;
	if ((loc.x - univ.town.monst[m_num].cur_loc.x >= 0) &&
		(loc.x - univ.town.monst[m_num].cur_loc.x <= univ.town.monst[m_num].x_width - 1) &&
		(loc.y - univ.town.monst[m_num].cur_loc.y >= 0) &&
		(loc.y - univ.town.monst[m_num].cur_loc.y <= univ.town.monst[m_num].y_width - 1))
		return true;
	return false;
	
}
short monst_there(location where) // returns 90 if no
{
	short i;
	
	for (i = 0; i < univ.town->max_monst(); i++)
		if ((univ.town.monst[i].active != 0) && (monst_on_space(where,i) == true))
			return i;
	return 90;
}
bool monst_can_be_there(location loc,short m_num)
{
	short i,j;
	location destination;
	
	// First clear monst away so it doesn't block itself
	univ.town.monst[m_num].cur_loc.x += 100;
	
	for (i = 0; i < univ.town.monst[m_num].x_width; i++)
		for (j = 0; j < univ.town.monst[m_num].y_width; j++) {
			destination.x = loc.x + i; destination.y = loc.y + j;
			if ((is_blocked(destination) == true)
				|| (loc_off_act_area(destination) == true)) {
				univ.town.monst[m_num].cur_loc.x -= 100;
				return false;
			}
		}
	univ.town.monst[m_num].cur_loc.x -= 100;
	return true;
}

bool monst_adjacent(location loc,short m_num)
{
	short i,j;
	location destination;
	
	for (i = 0; i < univ.town.monst[m_num].x_width; i++)
		for (j = 0; j < univ.town.monst[m_num].y_width; j++) {
			destination.x = univ.town.monst[m_num].cur_loc.x + i;
			destination.y = univ.town.monst[m_num].cur_loc.y + j;
			if (adjacent(destination,loc) == true)
				return true;
		}
	return false;
}

bool monst_can_see(short m_num,location l)
{
	short i,j;
	location destination;
	
	for (i = 0; i < univ.town.monst[m_num].x_width; i++)
		for (j = 0; j < univ.town.monst[m_num].y_width; j++) {
			destination.x = univ.town.monst[m_num].cur_loc.x + i;
			destination.y = univ.town.monst[m_num].cur_loc.y + j;
			if (can_see(destination,l,0) < 5)
				return true;
		}
	return false;
}

bool party_can_see_monst(short m_num)
{
	short i,j;
	location destination;
	
	for (i = 0; i < univ.town.monst[m_num].x_width; i++)
		for (j = 0; j < univ.town.monst[m_num].y_width; j++) {
			destination.x = univ.town.monst[m_num].cur_loc.x + i;
			destination.y = univ.town.monst[m_num].cur_loc.y + j;
			if (party_can_see(destination) < 6)
				return true;
		}
	return false;
}

bool can_see_monst(location l,short m_num)
{
	short i,j;
	location destination;
	
	for (i = 0; i < univ.town.monst[m_num].x_width; i++)
		for (j = 0; j < univ.town.monst[m_num].y_width; j++) {
			destination.x = univ.town.monst[m_num].cur_loc.x + i;
			destination.y = univ.town.monst[m_num].cur_loc.y + j;
			if (can_see(l,destination,0) < 5)
				return true;
		}
	return false;
}

bool outd_is_blocked(location to_check)
{
	short i;
	
	if (overall_mode == MODE_OUTDOORS) {
		if (impassable(univ.out[to_check.x][to_check.y]) == true) {
			return true;
		}
		for (i = 0; i < 10; i++)
			if ((univ.party.out_c[i].exists) == true)
				if (univ.party.out_c[i].m_loc == to_check)
					return true;
		return false;
	}
	return false;
}

bool special_which_blocks_monst(location to_check)
{
	if(scenario.ter_types[coord_to_ter(to_check.x,to_check.y)].blockage == eTerObstruct::BLOCK_MONSTERS)
		return true;
	else return false;
}

// Checks if space is a special that prevents movement into or placement of a PC on
bool is_special(location to_check)
{
	ter_num_t which_ter;
	
	if (special_which_blocks_monst(to_check) == false)
		return false;
	which_ter = coord_to_ter(to_check.x,to_check.y);
	if(scenario.ter_types[which_ter].blockage == eTerObstruct::BLOCK_MONSTERS)
		return true;
	else return false;
}

bool outd_is_special(location to_check)
{
	if (overall_mode == MODE_OUTDOORS) {
		if(scenario.ter_types[univ.out[to_check.x][to_check.y]].blockage == eTerObstruct::BLOCK_MONSTERS) {
			return true;
		}
		else return false;
	}
	return false;
}

bool impassable(ter_num_t terrain_to_check)
{
	if(blocksMove(scenario.ter_types[terrain_to_check].blockage))
		return true;
	else return false;
}

// TODO: What on earth is this and why does it mangle the blockage?
// NOTE: Seems to return 5 for "blocks sight", 1 for "obstructs missiles", 0 otherwise
// So it should probably be called something like "get_opacity" instead.
short get_blockage(ter_num_t terrain_type)
{
	// little kludgy in here for pits
	if ((terrain_type == 90) && (is_combat()) && (which_combat_type == 0))
		return 5;
	if(scenario.ter_types[terrain_type].blockage == eTerObstruct::BLOCK_MOVE_AND_SIGHT ||
	   scenario.ter_types[terrain_type].blockage == eTerObstruct::BLOCK_SIGHT)
		return 5;
	else if(scenario.ter_types[terrain_type].blockage == eTerObstruct::BLOCK_MOVE_AND_SHOOT)
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
		|| (is_out()) || (univ.town->lighting_type == 0))
		return 200;
	for (i = 0; i < 6; i++)
		if (univ.party.light_level > extra_levels[i])
			store++;
	return store;
}

bool pt_in_light(location from_where,location to_where) // Assumes, of course, in town or combat
{
	
	if (univ.town->lighting_type == 0)
		return true;
	if ((to_where.x < 0) || (to_where.x >= univ.town->max_dim())
		|| (to_where.y < 0) || (to_where.y >= univ.town->max_dim()))
		return true;
	if (univ.town->lighting(to_where.x / 8,to_where.y) & (char) (s_pow(2,to_where.x % 8)))
		return true;
	
	if (dist(from_where,to_where) <= light_radius())
		return true;
	
	return false;
}

bool combat_pt_in_light(location to_where)
{
	short i,rad;
	
	if ((univ.town->lighting_type == 0) || (which_combat_type == 0))
		return true;
	if ((to_where.x < 0) || (to_where.x >= univ.town->max_dim())
		|| (to_where.y < 0) || (to_where.y >= univ.town->max_dim()))
		return true;
	if (univ.town->lighting(to_where.x / 8,to_where.y) & (char) (s_pow(2,to_where.x % 8)))
		return true;
	
	rad = light_radius();
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1) {
			if (dist(pc_pos[i],to_where) <= rad)
				return true;
		}
	
	return false;
}

bool party_sees_a_monst() // Returns true is a hostile monster is in sight.
{
	short i;
	
	for (i = 0; i < univ.town->max_monst(); i++) {
		if (univ.town.monst[i].active > 0)
			if ((univ.town.monst[i].attitude == 1) &&
				(party_can_see_monst(i) == true))
				return true;
	}
	
	return false;
}


// Returns 6 if can't see, O.W. returns the # of a PC that can see
short party_can_see(location where)
{
	short i;
	
	if (is_out()) {
		if ((point_onscreen(univ.party.p_loc,where) == true) && (can_see(univ.party.p_loc,where,0) < 5))
			return 1;
		else return 6;
	}
	if (is_town()) {
		if ( ((point_onscreen(univ.town.p_loc,where) == true) || (overall_mode == MODE_LOOK_TOWN)) && (pt_in_light(univ.town.p_loc,where)  == true)
			&& (can_see(univ.town.p_loc,where,0) < 5))
			return 1;
		else return 6;
	}
	
	// Now for combat checks. Doing separately for efficiency. Check first for light. If
	//   dark, give up.
	if ((which_combat_type != 0) && (combat_pt_in_light(where) == false))
		return 6;
	
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1) {
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
	if ((univ.town->terrain(loc_to_try.x,loc_to_try.y) == 90) ||
		((univ.town->terrain(loc_to_try.x,loc_to_try.y) >= 50)&& (univ.town->terrain(loc_to_try.x,loc_to_try.y) <= 64))
		|| (univ.town->terrain(loc_to_try.x,loc_to_try.y) == 71)
		|| ((univ.town->terrain(loc_to_try.x,loc_to_try.y) >= 74)&& (univ.town->terrain(loc_to_try.x,loc_to_try.y) <= 78))
		) {
		// Destroy crate
		loc_to_try.x = 0;
		return loc_to_try;
	}
	if(sight_obscurity(loc_to_try.x,loc_to_try.y) > 0 ||
	    scenario.ter_types[univ.town->terrain(loc_to_try.x,loc_to_try.y)].blockage != eTerObstruct::CLEAR ||
		(loc_off_act_area(loc_to_try) == true) ||
		(monst_there(loc_to_try) < 90) ||
		(pc_there(loc_to_try) < 6))
		return from_where;
	else return loc_to_try;
}


// TODO: This seems to be wrong; impassable implies "blocks movement", which two other blockages also do
bool spot_impassable(short i,short  j)
{
	ter_num_t ter;
	
	ter = coord_to_ter(i,j);
	if(scenario.ter_types[ter].blockage == eTerObstruct::BLOCK_MOVE_AND_SIGHT)
		return true;
	else return false;
}

void swap_ter(short i,short j,ter_num_t ter1,ter_num_t ter2)
{
	if (coord_to_ter(i,j) == ter1)
		alter_space(i,j,ter2);
	else if (coord_to_ter(i,j) == ter2)
		alter_space(i,j,ter1);
}

void alter_space(short i,short j,ter_num_t ter)
{
	location l;
	
	l.x = i;
	l.y = j;
	map_graphic_placed[i / 8][j] = 0;
//	map_graphic_placed[i / 8][j] =
//		map_graphic_placed[i / 8][j] & ~((unsigned char)(s_pow(2,i % 8)));
	
	if (is_out()) {
		l = local_to_global(l);
		univ.out[l.x][l.y] = ter;
		univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].terrain[i][j] = ter;
	}
	else {
		univ.town->terrain(i,j) = ter;
		combat_terrain[i][j] = ter;
		if (scenario.ter_types[univ.town->terrain(i,j)].special == TER_SPEC_CONVEYOR)
			belt_present = true;
	}
}
