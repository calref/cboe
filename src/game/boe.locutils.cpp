
#include "mathutil.hpp"
#include "universe.hpp"
#include "utility.hpp"

#include "boe.global.hpp"
#include "boe.locutils.hpp"
#include "boe.minimap.hpp"
#include "boe.monster.hpp"
#include "boe.text.hpp"
#include "boe.town.hpp"

bool combat_pt_in_light();
location obs_sec;
location which_party_sec;

extern short which_combat_type;
extern eGameMode overall_mode;
extern eGameMode store_pre_shop_mode, store_pre_talk_mode;
extern location center;
extern cUniverse univ;
extern bool cartoon_happening, fog_lifted;

location light_locs[40];
short num_lights = 0;
char d_s[60];

bool is_explored(short i,short j) {
	if(is_out())
		return (univ.out.out_e[i][j] != 0) ? true : false;
	else return univ.town.is_explored(i,j);
}

void make_explored(short i,short j, short val) {
	if(is_out()) {
		if (univ.out.out_e[i][j] != val) {
			minimap::add_pending_redraw();
			univ.out.out_e[i][j] = val;
		}
	}
	else {
		bool bVal=bool(val);
		if (bool(univ.town.is_explored(i,j)) != bVal) {
			minimap::add_pending_redraw();
			univ.town.set_explored(i,j,bVal);
		}
	}
}

bool is_out() {
	if((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_LOOK_OUTDOORS))
		return true;
	else if(overall_mode == MODE_SHOPPING) {
		std::swap(overall_mode, store_pre_shop_mode);
		bool ret = is_out();
		std::swap(overall_mode, store_pre_shop_mode);
		return ret;
	} else if(overall_mode == MODE_TALKING) {
		std::swap(overall_mode, store_pre_talk_mode);
		bool ret = is_out();
		std::swap(overall_mode, store_pre_talk_mode);
		return ret;
	} else return false;
}

bool is_town() {
	if((overall_mode > MODE_OUTDOORS && overall_mode < MODE_COMBAT) || overall_mode == MODE_LOOK_TOWN || cartoon_happening)
		return true;
	else if(overall_mode == MODE_SHOPPING) {
		std::swap(overall_mode, store_pre_shop_mode);
		bool ret = is_town();
		std::swap(overall_mode, store_pre_shop_mode);
		return ret;
	} else if(overall_mode == MODE_TALKING) {
		std::swap(overall_mode, store_pre_talk_mode);
		bool ret = is_town();
		std::swap(overall_mode, store_pre_talk_mode);
		return ret;
	} else return false;
}

bool is_combat() {
	if(((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING)) || (overall_mode == MODE_LOOK_COMBAT))
		return true;
	else return false;
}

bool adjacent(location p1,location p2) {
	if((abs((short) (p1.x - p2.x)) <= 1) && (abs((short) (p1.y - p2.y)) <= 1))
		return true;
	else return false;
}

bool point_onscreen(location center,location check) {
	if((abs((short) (center.x - check.x)) <=4) && (abs((short) (center.y - check.y)) <= 4))
		return true;
	else return false;
}


eDirection set_direction (location old_pt, location new_pt) {
	if(old_pt.x == new_pt.x) {
		if(old_pt.y > new_pt.y)
			return DIR_N;
		else return DIR_S;
	}
	if(old_pt.x > new_pt.x) {
		if(old_pt.y > new_pt.y)
			return DIR_NW;
		if(old_pt.y < new_pt.y)
			return DIR_SW;
		return DIR_W;
	}
	if(old_pt.y > new_pt.y)
		return DIR_NE;
	if(old_pt.y < new_pt.y)
		return DIR_SE;
	return DIR_E;
}

location global_to_local(location global) {
	location local;
	
	local = global;
	if(global.x >= 48)
		local.x = local.x - 48;
	if(global.y >= 48)
		local.y = local.y - 48;
	return local;
}
// TODO: I fixed the above function, but it's impossible to fix this one without adding a parameter (which would be i_w_c)
location local_to_global(location local) {
	location global;
	
	global = local;
	if(univ.party.i_w_c.x == 1)
		global.x = global.x + 48;
	if(univ.party.i_w_c.y == 1)
		global.y = global.y + 48;
	return global;
}
bool loc_off_world(location p1) {
	if((p1.x < 0) || (p1.x > univ.town->max_dim) || (p1.y < 0) || (p1.y > univ.town->max_dim))
		return true;
	else return false;
}

bool loc_off_act_area(location p1) {
	if (!univ.town->is_on_map(p1)) return true;
	if((p1.x > univ.town->in_town_rect.left) && (p1.x < univ.town->in_town_rect.right) &&
		(p1.y > univ.town->in_town_rect.top) && (p1.y < univ.town->in_town_rect.bottom))
	 	return false;
	return true;
}

location get_cur_loc() {
	switch(overall_mode) {
		case MODE_OUTDOORS: case MODE_LOOK_OUTDOORS:
			return univ.party.out_loc;
			break;
			
		case MODE_TOWN: case MODE_TALK_TOWN: case MODE_TOWN_TARGET: case MODE_USE_TOWN: case MODE_LOOK_TOWN:
			return univ.party.town_loc;
			break;
			
		default:
			return univ.current_pc().combat_pos;
			break;
	}
}

// TODO: Don't hardcode this!
bool is_lava(short x,short y) {
	cPictNum pic=univ.get_terrain(coord_to_ter(x,y)).get_picture_num();
	return pic.num==4 && pic.type==ePicType::PIC_TER_ANIM;
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
	// It appears to refer to mountain cave entrances, surface tower, and surface pit. (WHY?)
	if((what_terrain >= 237) && (what_terrain <= 242))
		return 1;
	
	store = get_blockage(what_terrain);
	
	if(is_town()) {
		if(univ.town.is_special(x,y))
			store++;
	}
	
	if((is_town()) || (is_combat())) {
		if(univ.town.is_web(x,y))
			store += 2;
		if((univ.town.is_fire_barr(x,y)) || (univ.town.is_force_barr(x,y)))
			return 5;
		if(univ.town.is_crate(x,y) || univ.town.is_barrel(x,y) || univ.town.is_block(x,y))
			store++;
		
	}
	return store;
}

short combat_obscurity(short x, short y) {
	if(univ.get_terrain(coord_to_ter(x,y)).blocksMove()) return 5;
	if(is_lava(x,y)) return 5;
	return sight_obscurity(x,y);
}

ter_num_t coord_to_ter(short x,short y) {
	if (is_out())
		return univ.out[x][y];
	// ASAN called by place_road(..., false) with (x,y)=(univ.town->max_dim,univ.town->max_dim)
	if (x<0 || y<0 || x>=univ.town->max_dim || y>=univ.town->max_dim)
		return 0;
	return univ.town->terrain(x,y);
}

////
bool is_container(location loc) {
	if((univ.town.is_barrel(loc.x,loc.y)) || (univ.town.is_crate(loc.x,loc.y)))
		return true;
	return univ.get_terrain(coord_to_ter(loc.x,loc.y)).special == eTerSpec::IS_A_CONTAINER;
}

void update_explored(location dest) {
	if(cartoon_happening) return;
	
	if(is_out()) {
		location shortdest((short) dest.x,(short) dest.y);
		
		which_party_sec.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
		which_party_sec.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
		make_explored(dest.x,dest.y,2);
		
		location look;
		for(look.x = shortdest.x - 4; look.x < shortdest.x + 5; look.x++)
			for(look.y = shortdest.y - 4; look.y < shortdest.y + 5; look.y++) {
				// TODO: Windows had an extra check, is this needed?
				//if((look.x == minmax(0,95,(int)look.x)) && (look.y == minmax(0,95,(int)look.y))) {
				if(look.x>=0 && look.y>=0 && look.x<96 && look.y<96 && univ.out.out_e[look.x][look.y] == 0) {
					if(can_see_light(shortdest, look, sight_obscurity) < 5)
						make_explored(look.x, look.y);
				}
			}
	} else {
		make_explored(dest.x,dest.y);
		location look2;
		for(look2.x = max(0,dest.x - 4); look2.x < min(univ.town->max_dim,dest.x + 5); look2.x++)
			for(look2.y = max(0,dest.y - 4); look2.y < min(univ.town->max_dim,dest.y + 5); look2.y++)
				if(!is_explored(look2.x,look2.y) && can_see_light(dest, look2,sight_obscurity) < 5 && pt_in_light(dest,look2))
					make_explored(look2.x,look2.y);
	}
}


// All purpose function to check is spot is free for travel into.
bool is_blocked(location to_check) {
	ter_num_t ter;
	// ASAN: location can be bad if we are called by place_party
	if (to_check.x<0 || to_check.y<0)
		return true;
	if(is_out()) {
		if (to_check.x>=96 || to_check.y>=96) // checkme: maybe >=48
			return true;
		if(impassable(univ.out[to_check.x][to_check.y])) {
			return true;
		}
		if(to_check == univ.party.out_loc)
			return true;
		for(short i = 0; i < univ.party.out_c.size(); i++)
			if((univ.party.out_c[i].exists))
				if(univ.party.out_c[i].m_loc == to_check)
					return true;
		return false;
	}
	
	if((is_town()) || (is_combat())) {
		if (to_check.x >= univ.town->max_dim || to_check.y >= univ.town->max_dim)
			return true;
		ter = univ.town->terrain(to_check.x,to_check.y);
		
		// Terrain blocking?
		if(impassable(ter)) {
			return true;
		}
		
		if (is_combat()) {
			if (univ.scenario.is_legacy) {
				// checkme in combat.c this is only called when is_combat() && ter_pic==406
				//         (ie. ter_anim+6) due to a logical error
				cPictNum pict=univ.get_terrain(coord_to_ter(to_check.x,to_check.y)).get_picture_num();
				if (pict.num==6 && pict.type==ePicType::PIC_TER_ANIM)
					return true;
			}
			else {
				if(univ.town.is_spot(to_check.x, to_check.y) || univ.get_terrain(coord_to_ter(to_check.x,to_check.y)).trim_type == eTrimType::CITY)
					return true; // TODO: Maybe replace eTrimType::CITY with a blockage == clear/special && is_special() check
					// Note: The purpose of the above check is to avoid portals.
			}
		}
		
		// Party there?
		if(is_town())
			if(to_check == univ.party.town_loc)
				return true;
		if(is_combat())
			for(short i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE && to_check == univ.party[i].combat_pos)
					return true;
		
		// Monster there?
		if(univ.target_there(to_check, TARG_MONST))
			return true;
		
		// Magic barrier?
		if(univ.town.is_force_barr(to_check.x,to_check.y))
			return true;
		
		if(univ.town.is_force_cage(to_check.x,to_check.y))
			return true;
		
		return false;
	}
	return true;
}

bool monst_can_be_there(location loc,short m_num) {
	location destination;
	
	// First clear monst away so it doesn't block itself
	univ.town.monst[m_num].cur_loc.x += 100;
	
	for(short i = 0; i < univ.town.monst[m_num].x_width; i++)
		for(short j = 0; j < univ.town.monst[m_num].y_width; j++) {
			destination.x = loc.x + i; destination.y = loc.y + j;
			if((is_blocked(destination))
				|| (loc_off_act_area(destination))) {
				univ.town.monst[m_num].cur_loc.x -= 100;
				return false;
			}
		}
	univ.town.monst[m_num].cur_loc.x -= 100;
	return true;
}

bool monst_adjacent(location loc,short m_num) {
	location destination;
	
	for(short i = 0; i < univ.town.monst[m_num].x_width; i++)
		for(short j = 0; j < univ.town.monst[m_num].y_width; j++) {
			destination.x = univ.town.monst[m_num].cur_loc.x + i;
			destination.y = univ.town.monst[m_num].cur_loc.y + j;
			if(adjacent(destination,loc))
				return true;
		}
	return false;
}

bool monst_can_see(short m_num,location l) {
	location destination;
	
	for(short i = 0; i < univ.town.monst[m_num].x_width; i++)
		for(short j = 0; j < univ.town.monst[m_num].y_width; j++) {
			destination.x = univ.town.monst[m_num].cur_loc.x + i;
			destination.y = univ.town.monst[m_num].cur_loc.y + j;
			if(can_see_light(destination,l,sight_obscurity) < 5)
				return true;
		}
	return false;
}

bool party_can_see_monst(short m_num) {
	location destination;
	
	for(short i = 0; i < univ.town.monst[m_num].x_width; i++)
		for(short j = 0; j < univ.town.monst[m_num].y_width; j++) {
			destination.x = univ.town.monst[m_num].cur_loc.x + i;
			destination.y = univ.town.monst[m_num].cur_loc.y + j;
			if(party_can_see(destination) < 6)
				return true;
		}
	return false;
}

bool can_see_monst(location l,short m_num) {
	location destination;
	
	for(short i = 0; i < univ.town.monst[m_num].x_width; i++)
		for(short j = 0; j < univ.town.monst[m_num].y_width; j++) {
			destination.x = univ.town.monst[m_num].cur_loc.x + i;
			destination.y = univ.town.monst[m_num].cur_loc.y + j;
			if(can_see_light(l,destination,sight_obscurity) < 5)
				return true;
		}
	return false;
}

bool outd_is_blocked(location to_check) {
	if(overall_mode == MODE_OUTDOORS) {
		// ASAN to_check can be bad if called by outdoor_move_monster (by rand_move)
		if (to_check.x<0 || to_check.y<0 || to_check.x>=96 || to_check.y>=96)
			return true;
		if(impassable(univ.out[to_check.x][to_check.y])) {
			return true;
		}
		for(short i = 0; i < 10; i++)
			if((univ.party.out_c[i].exists))
				if(univ.party.out_c[i].m_loc == to_check)
					return true;
		return false;
	}
	return false;
}

// Checks if space is a special that prevents movement into or placement of a PC on
bool is_special(location to_check) {
	return univ.get_terrain(coord_to_ter(to_check.x,to_check.y)).blockage == eTerObstruct::BLOCK_MONSTERS;
}

bool outd_is_special(location to_check) {
	if(overall_mode == MODE_OUTDOORS)
		return univ.get_terrain(univ.out[to_check.x][to_check.y]).blockage == eTerObstruct::BLOCK_MONSTERS;
	return false;
}

bool impassable(ter_num_t terrain_to_check) {
	return univ.get_terrain(terrain_to_check).blocksMove();
}

// TODO: What on earth is this and why does it mangle the blockage?
// NOTE: Seems to return 5 for "blocks sight", 1 for "obstructs missiles", 0 otherwise
// So it should probably be called something like "get_opacity" instead.
short get_blockage(ter_num_t terrain_type) {
	// little kludgy in here for pits
	if((terrain_type == 90) && (is_combat()) && (which_combat_type == 0))
		return 5;
	cTerrain const &terrain=univ.get_terrain(terrain_type);
	if(terrain.blockage == eTerObstruct::BLOCK_MOVE_AND_SIGHT || terrain.blockage == eTerObstruct::BLOCK_SIGHT)
		return 5;
	else if(terrain.blockage == eTerObstruct::BLOCK_MOVE_AND_SHOOT)
		return 1;
	else {
		return 0;
	}
}



short light_radius() {
	short store = 1;
	short extra_levels[6] = {10,20,50,75,110,140};
	
	if(((which_combat_type == 0) && (is_combat()))
		|| (is_out()) || (univ.town->lighting_type == 0))
		return 200;
	for(short i = 0; i < 6; i++)
		if(univ.party.light_level > extra_levels[i])
			store++;
	return store;
}

bool pt_in_light(location from_where,location to_where) { // Assumes, of course, in town or combat
	
	if(univ.town->lighting_type == 0)
		return true;
	if((to_where.x < 0) || (to_where.x >= univ.town->max_dim)
		|| (to_where.y < 0) || (to_where.y >= univ.town->max_dim))
		return true;
	if(univ.town->lighting[to_where.x][to_where.y])
		return true;
	
	if(dist(from_where,to_where) <= light_radius())
		return true;
	
	return false;
}

bool combat_pt_in_light(location to_where) {
	short rad;
	
	if((univ.town->lighting_type == 0) || (which_combat_type == 0))
		return true;
	if((to_where.x < 0) || (to_where.x >= univ.town->max_dim)
		|| (to_where.y < 0) || (to_where.y >= univ.town->max_dim))
		return true;
	if(univ.town->lighting[to_where.x][to_where.y])
		return true;
	
	rad = light_radius();
	for(short i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
			if(dist(univ.party[i].combat_pos,to_where) <= rad)
				return true;
		}
	
	return false;
}

bool party_sees_a_monst() { // Returns true is a hostile monster is in sight.
	for(short i = 0; i < univ.town.monst.size(); i++) {
		if(univ.town.monst[i].active > 0)
			if(!univ.town.monst[i].is_friendly() &&
				(party_can_see_monst(i)))
				return true;
	}
	
	return false;
}


// Returns 6 if can't see, O.W. returns the # of a PC that can see
short party_can_see(location where) {
	if(is_out()) {
		if((point_onscreen(univ.party.out_loc,where)) && (can_see_light(univ.party.out_loc,where,sight_obscurity) < 5))
			return 1;
		else return 6;
	}
	if(fog_lifted)
		return point_onscreen(univ.party.town_loc,where) ? 1 : 6;
	if(is_town()) {
		if( ((point_onscreen(univ.party.town_loc,where)) || (overall_mode == MODE_LOOK_TOWN)) && (pt_in_light(univ.party.town_loc,where) )
			&& (can_see_light(univ.party.town_loc,where,sight_obscurity) < 5))
			return 1;
		else return 6;
	}
	
	// Now for combat checks. Doing separately for efficiency. Check first for light. If
	//   dark, give up.
	if((which_combat_type != 0) && !combat_pt_in_light(where))
		return 6;
	
	for(short i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
			if(can_see_light(univ.party[i].combat_pos,where,sight_obscurity) < 5)
				return i;
		}
	
	return 6;
}
location push_loc(location from_where,location to_where) {
	location loc_to_try;
	
	loc_to_try = to_where;
	loc_to_try.x = loc_to_try.x + (to_where.x - from_where.x);
	loc_to_try.y = loc_to_try.y + (to_where.y - from_where.y);
	if((univ.town->terrain(loc_to_try.x,loc_to_try.y) == 90) ||
		((univ.town->terrain(loc_to_try.x,loc_to_try.y) >= 50)&& (univ.town->terrain(loc_to_try.x,loc_to_try.y) <= 64))
		|| (univ.town->terrain(loc_to_try.x,loc_to_try.y) == 71)
		|| ((univ.town->terrain(loc_to_try.x,loc_to_try.y) >= 74)&& (univ.town->terrain(loc_to_try.x,loc_to_try.y) <= 78))
		) {
		// Destroy crate
		loc_to_try.x = 0;
		return loc_to_try;
	}
	if(sight_obscurity(loc_to_try.x,loc_to_try.y) > 0 ||
	   univ.get_terrain(univ.town->terrain(loc_to_try.x,loc_to_try.y)).blockage != eTerObstruct::CLEAR ||
	   (loc_off_act_area(loc_to_try)) ||
	   univ.target_there(loc_to_try))
		return from_where;
	else return loc_to_try;
}


// TODO: This seems to be wrong; impassable implies "blocks movement", which two other blockages also do
bool spot_impassable(short i,short  j) {
	return univ.get_terrain(coord_to_ter(i,j)).blockage == eTerObstruct::BLOCK_MOVE_AND_SIGHT;
}

void swap_ter(short i,short j,ter_num_t ter1,ter_num_t ter2) {
	if(coord_to_ter(i,j) == ter1)
		alter_space(i,j,ter2);
	else if(coord_to_ter(i,j) == ter2)
		alter_space(i,j,ter1);
}

void alter_space(short i,short j,ter_num_t ter) {
	if(is_out()) {
		location l(i,j);
		l = local_to_global(l);
		univ.out[l.x][l.y] = ter;
		if (univ.out->terrain[i][j] != ter) {
			univ.out->terrain[i][j] = ter;
			minimap::draw(true);
		}
	} else {
		ter_num_t former = univ.town->terrain(i,j);
		if (former!=ter) {
			univ.town->terrain(i,j) = ter;
			minimap::draw(true);
		}
		if(univ.get_terrain(ter).special == eTerSpec::CONVEYOR)
			univ.town.belt_present = true;
		if(univ.get_terrain(former).light_radius != univ.get_terrain(ter).light_radius)
			univ.town->set_up_lights();
	}
}
