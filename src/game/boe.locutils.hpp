
#include <functional>
#include "location.hpp"

bool is_explored(short i,short j);
void make_explored(short i,short j, short val=1);
bool is_out();
bool is_town();
bool is_combat();
bool adjacent(location p1,location p2);
bool point_onscreen(location center,location check);
eDirection set_direction (location old_pt, location new_pt);
location global_to_local(location global);
location local_to_global(location local);
bool loc_off_world(location p1);
bool loc_off_act_area(location p1);
location get_cur_loc();
bool is_lava(short x,short y);
short sight_obscurity(short x,short y);
short can_see_light(location p1, location p2, std::function<short(short,short)> get_obscurity);
short combat_obscurity(short x,short y);
ter_num_t coord_to_ter(short x,short y);
bool is_container(location loc);
void update_explored(location dest);
bool is_blocked(location to_check);
bool monst_can_be_there(location loc,short m_num);
bool monst_adjacent(location loc,short m_num);
bool monst_can_see(short m_num,location l);
bool party_can_see_monst(short m_num);
bool can_see_monst(location l,short m_num);
bool outd_is_blocked(location to_check);
bool is_special(location to_check);
bool outd_is_special(location to_check);
bool impassable(ter_num_t terrain_to_check);
short get_blockage(ter_num_t terrain_type);
short light_radius();
bool pt_in_light(location from_where,location to_where) ;// Assumes, of course, in town or combat
bool combat_pt_in_light(location to_where);
bool party_sees_a_monst(); // Returns true is a hostile monster is in sight.
short party_can_see(location where);
location push_loc(location from_where,location to_where);
bool spot_impassable(short i,short  j);
void swap_ter(short i,short j,ter_num_t ter1,ter_num_t ter2);
void alter_space(short i,short j,ter_num_t ter);
