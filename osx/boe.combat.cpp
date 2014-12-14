
#include <cstdio>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"
#include "boe.monster.h"
#include "boe.graphics.h"
#include "boe.locutils.h"
#include "boe.newgraph.h"
#include "boe.infodlg.h"
#include "boe.text.h"
#include "boe.items.h"
#include "boe.party.h"
#include "boe.combat.h"
#include "soundtool.h"
#include "boe.town.h"
#include "boe.specials.h"
#include "boe.graphutil.h"
#include "boe.main.h"
#include "mathutil.h"
#include "dlogutil.h"
#include "boe.menus.h"
#include "spell.hpp"

//extern party_record_type party;
//extern current_town_type univ.town;
//extern big_tr_type t_d;
//extern town_item_list	t_i;
extern eGameMode overall_mode;
extern bool ghost_mode;
extern short which_combat_type;
extern short stat_window;
extern location center;
extern ter_num_t combat_terrain[64][64];
extern short current_pc;
extern short combat_active_pc;
extern bool monsters_going,spell_forced;
extern bool flushingInput;
extern sf::RenderWindow mainPtr;
extern eSpell store_mage, store_priest;
extern short store_mage_lev, store_priest_lev,store_item_spell_level;
extern short store_spell_target,pc_casting,current_spell_range;
extern effect_pat_type current_pat;
//extern short town_size[3];
//extern short monst_target[60]; // 0-5 target that pc   6 - no target  100 + x - target monster x
extern short num_targets_left;
extern location spell_targets[8];
extern bool in_scen_debug;
extern bool fast_bang;
//extern unsigned char misc_i[64][64],sfx[64][64];
extern short store_current_pc;
//extern location monster_targs[60];
extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x
extern cScenario scenario;
extern short spell_caster, missile_firer,current_monst_tactic;
char create_line[60];
eSpell spell_being_cast;
bool spell_freebie;
short missile_inv_slot, ammo_inv_slot;
short force_wall_position = 10; //  10 -> no force wall
bool processing_fields = true;
short futzing;
m_num_t store_sum_monst;
short store_sum_monst_cost;
extern cUniverse univ;

location out_start_loc(20,23);
short hit_chance[51] = {
	20,30,40,45,50,55,60,65,69,73,
	77,81,84,87,90,92,94,96,97,98,99
	,99,99,99,99,99,99,99,99,99,99
	,99,99,99,99,99,99,99,99,99,99,
	99,99,99,99,99,99,99,99,99,99};

short monst_mage_spell[55] = {
	1,1,1,1,1,1,2,2,2,2,
	2,2,3,3,3,3,3,4,4,4,
	4,4,4,5,5,5,5,5,4,4,
	6,6,6,6,7,7,7,7,7,8,
	8,8,8,8,9,9,9,10,10,10,
	11,11,11,12,12};
short monst_cleric_spell[55] = {
	1,1,1,1,1,1,1,1,2,2,
	2,2,2,3,3,3,11,11,11,4,
	4,4,4,5,5,5,11,11,6,6,
	6,6,6,6,7,7,7,7,7,7,
	8,8,8,8,8,7,7,7,7,7,
	7,9,9,10,10};
short monst_mage_cost[27] = {1,1,1,1,2, 2,2,2,2,4, 2,4,4,3,4, 4,4,5,5,5, 5,6,6,6,7, 7,7};
short monst_mage_area_effect[27] = {0,0,0,0,0, 0,0,0,1,0, 1,1,0,1,0, 0,0,0,1,0, 1,0,0,0,0, 0,0};
short monst_priest_cost[26] = {1,1,1,1,2, 2,2,4,2,3, 3,3,4,4,4, 5,5,5,10,6, 6,10,8,8,8, 8};
short monst_priest_area_effect[26] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 1};

extern short boom_gr[8];

const char *d_string[] = {"North", "NorthEast", "East", "SouthEast", "South", "SouthWest", "West", "NorthWest"};

short monst_marked_damage[60];

location hor_vert_place[14] = {
	loc(0,0),loc(-1,1),loc(1,1),loc(-2,2),loc(0,2),
	loc(2,2),loc(0,1),loc(-1,2),loc(1,2),loc(-1,3),
	loc(1,3),loc(0,3),loc(0,4),loc(0,5)};
location diag_place[14] = {
	loc(0,0),loc(-1,0),loc(0,1),loc(-1,1),loc(-2,0),
	loc(0,2),loc(-2,1),loc(-1,2),loc(-2,2),loc(-3,2),
	loc(-2,3),loc(-3,3),loc(-4,3),loc(-3,4)};


effect_pat_type null_pat = {{
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}}};
effect_pat_type single = {{
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}}};
effect_pat_type t = {{
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,1,0,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,0,1,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}}};
effect_pat_type small_square = {{
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,1,1,0,0,0},
	{0,0,0,0,1,1,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}}};
effect_pat_type square = {{
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}}};
effect_pat_type open_square = {{
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,1,0,1,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}}};
effect_pat_type rad2 = {{
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}}};
effect_pat_type rad3 = {{
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,0},
	{0,1,1,1,1,1,1,1,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,0,0,0,0,0,0}}};
effect_pat_type field[8] = {
	{{
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,1,1,0,0,0}}},
	
	{{
		{0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,1,1},
		{0,0,0,0,0,0,1,1,0},
		{0,0,0,0,0,1,1,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,1,1,0,0,0,0,0},
		{0,1,1,0,0,0,0,0,0},
		{1,1,0,0,0,0,0,0,0}}},
	
	{{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}}},
	
	{{
		{1,0,0,0,0,0,0,0,0},
		{1,1,0,0,0,0,0,0,0},
		{0,1,1,0,0,0,0,0,0},
		{0,0,1,1,0,0,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,0,1,1,0,0},
		{0,0,0,0,0,0,1,1,0},
		{0,0,0,0,0,0,0,1,1}}},
	
	{{
		{0,0,0,1,1,0,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,0,1,1,0,0,0,0}}},
	
	{{
		{0,0,0,0,0,0,0,1,1},
		{0,0,0,0,0,0,1,1,0},
		{0,0,0,0,0,1,1,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,1,1,0,0,0,0,0},
		{0,1,1,0,0,0,0,0,0},
		{1,1,0,0,0,0,0,0,0},
		{1,0,0,0,0,0,0,0,0}}},
	
	{{
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}}},
	
	{{
		{1,1,0,0,0,0,0,0,0},
		{0,1,1,0,0,0,0,0,0},
		{0,0,1,1,0,0,0,0,0},
		{0,0,0,1,1,0,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,0,1,1,0,0},
		{0,0,0,0,0,0,1,1,0},
		{0,0,0,0,0,0,0,1,1},
		{0,0,0,0,0,0,0,0,1}}}};

bool center_on_monst;





void start_outdoor_combat(cOutdoors::cCreature encounter,ter_num_t in_which_terrain,short num_walls)
{
	short i,j,how_many,num_tries = 0;
//	short low[10] = {15,7,3,3,1,1,1,7,2,1};
//	short high[10] = {30,10,5,5,3,2,1,10,4,1};
	short low[10] = {15,7,4,3,2,1,1,7,2,1};
	short high[10] = {30,10,6,5,3,2,1,10,4,1};
	rectangle town_rect(0,0,47,47);
	short nums[10];
	
	for (i = 0; i < 7; i++)
		nums[i] = get_ran(1,low[i],high[i]);
	for (i = 0; i < 3; i++)
		nums[i + 7] = get_ran(1,low[i + 7],high[i + 7]);
	notify_out_combat_began(encounter.what_monst,nums);
	print_buf();
	play_sound(23);
	
	mainPtr.setActive();
	which_combat_type = 0;
	overall_mode = MODE_COMBAT;
	
	// Basically, in outdoor combat, we create kind of a 48x48 town for
	// the combat to take place in
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			univ.town.fields[i][j] = 0;
			//univ.out.misc_i[i][j] = 0;
			//univ.out.sfx[i][j] = 0;
		}
	univ.town.prep_arena();
	univ.town->in_town_rect = town_rect;
	
	create_out_combat_terrain((short) in_which_terrain,num_walls,0);////
	
	for (i = 0; i < univ.town->max_monst(); i++) {
		univ.town.monst[i].number = 0;
		univ.town.monst[i].active = 0;
	}
	for (i = 0; i < 7; i++) {
		how_many = nums[i];
		if (encounter.what_monst.monst[i] != 0)
			for (j = 0; j < how_many; j++)
				set_up_monst(0,encounter.what_monst.monst[i]);
	}
	for (i = 0; i < 3; i++) {
		how_many = nums[i + 7];
		if (encounter.what_monst.friendly[i] != 0)
			for (j = 0; j < how_many; j++)
				set_up_monst(1,encounter.what_monst.friendly[i]);
	}
	
	// place PCs
	univ.party[0].combat_pos = out_start_loc;
	update_explored(univ.party[0].combat_pos);
	if (get_blockage(combat_terrain[univ.party[0].combat_pos.x][univ.party[0].combat_pos.y]) > 0)
		combat_terrain[univ.party[0].combat_pos.x][univ.party[0].combat_pos.y] = combat_terrain[0][0];
	for (i = 1; i < 6; i++) {
		univ.party[i].combat_pos = univ.party[0].combat_pos;
		univ.party[i].combat_pos.x = univ.party[i].combat_pos.x + hor_vert_place[i].x;
		univ.party[i].combat_pos.y = univ.party[i].combat_pos.y + hor_vert_place[i].y;
		if (get_blockage(combat_terrain[univ.party[i].combat_pos.x][univ.party[i].combat_pos.y]) > 0)
			combat_terrain[univ.party[i].combat_pos.x][univ.party[i].combat_pos.y] = combat_terrain[0][0];
		update_explored(univ.party[i].combat_pos);
		
		univ.party[i].status[eStatus::POISONED_WEAPON] = 0;
		univ.party[i].status[eStatus::BLESS_CURSE] = 0;
		univ.party[i].status[eStatus::HASTE_SLOW] = 0;
		univ.party[i].status[eStatus::INVULNERABLE] = 0;
		univ.party[i].status[eStatus::MAGIC_RESISTANCE] = 0;
	}
	
	// place monsters, w. friendly monsts landing near PCs
	for (i = 0; i < univ.town->max_monst(); i++)
		if (univ.town.monst[i].active > 0) {
			univ.town.monst[i].target = 6;
			
			univ.town.monst[i].cur_loc.x  = get_ran(1,15,25);
			univ.town.monst[i].cur_loc.y  = get_ran(1,14,18);
			if (univ.town.monst[i].attitude == 2)
				univ.town.monst[i].cur_loc.y += 9;
			else if ((univ.town.monst[i].mu > 0) || (univ.town.monst[i].cl > 0))
				univ.town.monst[i].cur_loc.y -= 4;//max(12,univ.town.monst[i].m_loc.y - 4);
			num_tries = 0;
			while (((monst_can_be_there(univ.town.monst[i].cur_loc,i) == false) ||
					(combat_terrain[univ.town.monst[i].cur_loc.x][univ.town.monst[i].cur_loc.y] == 180) ||
					(pc_there(univ.town.monst[i].cur_loc) < 6)) &&
				   (num_tries++ < 50)) {
				univ.town.monst[i].cur_loc.x = get_ran(1,15,25);
				univ.town.monst[i].cur_loc.y = get_ran(1,14,18);
				if (univ.town.monst[i].attitude == 2)
					univ.town.monst[i].cur_loc.y += 9;
				else if ((univ.town.monst[i].mu > 0) || (univ.town.monst[i].cl > 0))
					univ.town.monst[i].cur_loc.y -= 4;//max(12,univ.town.monst[i].m_loc.y - 4);
			}
			if (get_blockage(combat_terrain[univ.town.monst[i].cur_loc.x][univ.town.monst[i].cur_loc.y]) > 0)
				combat_terrain[univ.town.monst[i].cur_loc.x][univ.town.monst[i].cur_loc.y] = combat_terrain[0][0];
		}
	
	
	combat_active_pc = 6;
	spell_caster = 6; missile_firer = 6;
	for (i = 0; i < univ.town->max_monst(); i++)
		univ.town.monst[i].target = 6;
	
	for (i = 0; i < 6; i++) {
		univ.party[i].parry = 0;
		univ.party[i].last_attacked = univ.town->max_monst() + 10;
	}
	
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		univ.town.items[i].variety = eItemType::NO_ITEM;
	store_current_pc = current_pc;
	current_pc = 0;
	set_pc_moves();
	pick_next_pc();
	center = univ.party[current_pc].combat_pos;
	draw_buttons(0);
	put_pc_screen();
	set_stat_window(current_pc);
	
	adjust_spell_menus();
	
	//clear_map();
	give_help(48,49);
	
}

bool pc_combat_move(location destination) ////
{
	short dir,monst_hit,s1,s2,i,monst_exist,switch_pc;
	bool keep_going = true,forced = false,check_f = false;
	location monst_loc,store_loc;
	short spec_num;
	
	monst_hit = monst_there(destination);
	
	if(monst_hit > univ.town->max_monst() && univ.party[current_pc].status[eStatus::FORCECAGE] > 0) {
		add_string_to_buf("Move: Can't escape.");
		return false;
	}
	
	if(monst_hit > univ.town->max_monst())
		keep_going = check_special_terrain(destination,eSpecCtx::COMBAT_MOVE,current_pc,&spec_num,&check_f);
	if (check_f == true)
		forced = true;
	
	if(in_scen_debug && ghost_mode) forced = true;
	
	if (keep_going == true) {
		
		dir = set_direction(univ.party[current_pc].combat_pos, destination);
		
		if ((loc_off_act_area(destination) == true) && (which_combat_type == 1)) {
			add_string_to_buf("Move: Can't leave town during combat.");
			print_buf();
			return true;
		}
		else if ((combat_terrain[destination.x][destination.y] == 90) && (which_combat_type == 0)) {
			if (get_ran(1,1,10) < 3) {
				univ.party[current_pc].main_status = eMainStatus::FLED;
				if (combat_active_pc == current_pc)
					combat_active_pc = 6;
				sprintf (create_line, "Moved: Fled.                    ");
				univ.party[current_pc].ap = 0;
			}
			else {
				take_ap(1);
				sprintf (create_line, "Moved: Couldn't flee.                  ");
			}
			add_string_to_buf(create_line);
			return true;
		}
		else if(monst_hit <= univ.town->max_monst()) {
			// s2 = 2 here appears to mean "go ahead and attack", while s2 = 1 means "cancel attack".
			// Then s1 % 2 == 1 means the monster is hostile to the party.
			s1 = univ.town.monst[monst_hit].attitude;
			if(s1 % 2 == 1) s2 = 2;
			else {
				std::string result = cChoiceDlog("attack-friendly.xml",{"cancel","attack"}).show();
				if(result == "cancel") s2 = 1;
				else if(result == "attack") s2 = 2;
			}
			if ((s2 == 2) && (s1 % 2 != 1))
				make_town_hostile();
			if (s2 == 2) {
				univ.party[current_pc].last_attacked = monst_hit;
				pc_attack(current_pc,monst_hit);
				return true;
			}
		}
		else if ((switch_pc = pc_there(destination)) < 6) {
			if (univ.party[switch_pc].ap == 0) {
				add_string_to_buf("Move: Can't switch places.");
				add_string_to_buf("  (other PC has no APs)	");
				return false;
			}
			else univ.party[switch_pc].ap--;
			add_string_to_buf("Move: Switch places.");
			store_loc = univ.party[current_pc].combat_pos;
			univ.party[current_pc].combat_pos = destination;
			univ.party[switch_pc].combat_pos = store_loc;
			univ.party[current_pc].direction = dir;
			take_ap(1);
			check_special_terrain(store_loc,eSpecCtx::COMBAT_MOVE,switch_pc,&spec_num,&check_f);
			move_sound(combat_terrain[destination.x][destination.y],univ.party[current_pc].ap);
			return true;
		}
		else if ((forced == true)
				 || ((impassable(combat_terrain[destination.x][destination.y]) == false) && (pc_there(destination) == 6))) {
			
			// monsters get back-shots
			for (i = 0; i < univ.town->max_monst(); i++) {
				monst_loc = univ.town.monst[i].cur_loc;
				monst_exist = univ.town.monst[i].active;
				
				s1 = current_pc;
				if ((monst_exist > 0) && (monst_adjacent(univ.party[current_pc].combat_pos,i) == true)
					&& (monst_adjacent(destination,i) == false) &&
					(univ.town.monst[i].attitude % 2 == 1) &&
					univ.town.monst[i].status[eStatus::ASLEEP] <= 0 &&
					univ.town.monst[i].status[eStatus::PARALYZED] <= 0) {
					combat_posing_monster = current_working_monster = 100 + i;
					monster_attack_pc(i,current_pc);
					combat_posing_monster = current_working_monster = -1;
					draw_terrain(0);
				}
				if (s1 != current_pc)
					return true;
			}
			
			// move if still alive
			if(univ.party[current_pc].main_status == eMainStatus::ALIVE) {
				univ.party[current_pc].dir = set_direction(univ.party[current_pc].combat_pos,destination);
				univ.party[current_pc].combat_pos = destination;
				univ.party[current_pc].direction = dir;
				take_ap(1);
				sprintf ((char *) create_line, "Moved: %s               ",d_string[dir]);
				add_string_to_buf((char *) create_line);
				move_sound(combat_terrain[destination.x][destination.y],univ.party[current_pc].ap);
				
			}
			else return false;
			return true;
		}
		else {
			sprintf ((char *) create_line, "Blocked: %s               ",d_string[dir]);
			add_string_to_buf((char *) create_line);
			return false;
		}
	}
	return false;
}

void char_parry()
{
	univ.party[current_pc].parry = (univ.party[current_pc].ap / 4) *
		(2 + stat_adj(current_pc,eSkill::DEXTERITY) + univ.party[current_pc].skills[eSkill::DEFENSE]);
	univ.party[current_pc].ap = 0;
}

void char_stand_ready()
{
	univ.party[current_pc].parry = 100;
	univ.party[current_pc].ap = 0;
}

void pc_attack(short who_att,short target)////
{
	short r1,r2,weap1 = 24, weap2 = 24,i,store_hp,skill_item;
	eSkill what_skill1 = eSkill::DEXTERITY, what_skill2 = eSkill::DEXTERITY;
	cCreature *which_m;
	short hit_adj, dam_adj, spec_dam = 0,poison_amt;
	
	// slice out bad attacks
	if(univ.party[who_att].main_status != eMainStatus::ALIVE)
		return;
	if(univ.party[who_att].status[eStatus::ASLEEP] > 0 || univ.party[who_att].status[eStatus::PARALYZED] > 0)
		return;
	
	univ.party[who_att].last_attacked = target;
	which_m = &univ.town.monst[target];
	
	for (i = 0; i < 24; i++)
		if (((univ.party[who_att].items[i].variety == eItemType::ONE_HANDED) || (univ.party[who_att].items[i].variety == eItemType::TWO_HANDED)) &&
			univ.party[who_att].equip[i]) {
			if (weap1 == 24)
				weap1 = i;
			else weap2 = i;
		}
	
	hit_adj = (-5 * minmax(-8,8,univ.party[who_att].status[eStatus::BLESS_CURSE])) + 5 * minmax(-8,8,which_m->status[eStatus::BLESS_CURSE])
		- stat_adj(who_att,eSkill::DEXTERITY) * 5 + (get_encumberance(who_att)) * 5;
	
	dam_adj = minmax(-8,8,univ.party[who_att].status[eStatus::BLESS_CURSE]) - minmax(-8,8,which_m->status[eStatus::BLESS_CURSE])
		+ stat_adj(who_att,eSkill::STRENGTH);
	
	if(which_m->status[eStatus::ASLEEP] > 0 || which_m->status[eStatus::PARALYZED] > 0) {
		hit_adj -= 80;
		dam_adj += 10;
	}
	
	
	if ((skill_item = pc_has_abil_equip(who_att,eItemAbil::SKILL)) < 24) {
		hit_adj += 5 * (univ.party[who_att].items[skill_item].item_level / 2 + 1);
		dam_adj += univ.party[who_att].items[skill_item].item_level / 2;
	}
	if ((skill_item = pc_has_abil_equip(who_att,eItemAbil::GIANT_STRENGTH)) < 24) {
		dam_adj += univ.party[who_att].items[skill_item].item_level;
		hit_adj += univ.party[who_att].items[skill_item].item_level * 2;
	}
	
	void_sanctuary(who_att);
	
	store_hp = univ.town.monst[target].health;
	
	combat_posing_monster = current_working_monster = who_att;
	
	if (weap1 == 24) {
		
		sprintf ((char *) create_line, "%s punches.  ",(char *) univ.party[who_att].name.c_str());//,hit_adj, dam_adj);
		add_string_to_buf((char *) create_line);
		
		r1 = get_ran(1,1,100) + hit_adj - 20;
		r1 += 5 * (univ.party[current_pc].status[eStatus::WEBS] / 3);
		r2 = get_ran(1,1,4) + dam_adj;
		
		if (r1 <= hit_chance[univ.party[who_att].skills[what_skill1]]) {
			damage_monst(target, who_att, r2, 0,DAMAGE_WEAPON,4);
		}
		else {
			draw_terrain(2);
			sprintf (create_line, "%s misses. ",(char *) univ.party[who_att].name.c_str());
			add_string_to_buf(create_line);
			play_sound(2);
		}
	}
	
	// Don't forget awkward and stat adj.
	if (weap1 < 24) {
		what_skill1 = univ.party[who_att].items[weap1].type;
		
		// safety valve
		if (what_skill1 == eSkill::INVALID)
			what_skill1 = eSkill::EDGED_WEAPONS;
		
		sprintf (create_line, "%s swings. ",univ.party[who_att].name.c_str());//,hit_adj, dam_adj);
		add_string_to_buf(create_line);
		
		r1 = get_ran(1,1,100) - 5 + hit_adj
			- 5 * univ.party[who_att].items[weap1].bonus;
		r1 += 5 * (univ.party[current_pc].status[eStatus::WEBS] / 3);
		
		if(weap2 < 24 && !univ.party[who_att].traits[eTrait::AMBIDEXTROUS])
			r1 += 25;
		
		// race adj.
		if(univ.party[who_att].race == eRace::SLITH && univ.party[who_att].items[weap1].type == eSkill::POLE_WEAPONS)
			r1 -= 10;
		
		r2 = get_ran(1,1,univ.party[who_att].items[weap1].item_level) + dam_adj + 2 + univ.party[who_att].items[weap1].bonus;
		if (univ.party[who_att].items[weap1].ability == eItemAbil::WEAK_WEAPON)
			r2 = (r2 * (10 - univ.party[who_att].items[weap1].ability_strength)) / 10;
		
		if (r1 <= hit_chance[univ.party[who_att].skills[what_skill1]]) {
			spec_dam = calc_spec_dam(univ.party[who_att].items[weap1].ability,
									 univ.party[who_att].items[weap1].ability_strength,which_m);
			
			// assassinate
			r1 = get_ran(1,1,100);
			if ((univ.party[who_att].level >= which_m->level - 1)
				&& univ.party[who_att].skills[eSkill::ASSASSINATION] >= which_m->level / 2
				&& (which_m->spec_skill != 12)) // Can't assassinate splitters
				if(r1 < hit_chance[max(univ.party[who_att].skills[eSkill::ASSASSINATION] - which_m->level,0)]) {
					add_string_to_buf("  You assassinate.           ");
					spec_dam += r2;
				}
			
			switch (what_skill1) {
			 	case eSkill::EDGED_WEAPONS:
					if (univ.party[who_att].items[weap1].item_level < 8)
						damage_monst(target, who_att, r2, spec_dam, DAMAGE_WEAPON,1);
					else damage_monst(target, who_att, r2, spec_dam, DAMAGE_WEAPON,2);
					break;
			 	case eSkill::BASHING_WEAPONS:
					damage_monst(target, who_att, r2, spec_dam, DAMAGE_WEAPON,4);
					break;
			 	case eSkill::POLE_WEAPONS:
					damage_monst(target, who_att, r2, spec_dam, DAMAGE_WEAPON,3);
					break;
			}
			// poison
			if(univ.party[who_att].status[eStatus::POISONED_WEAPON] > 0 && univ.party[who_att].weap_poisoned == weap1) {
				poison_amt = univ.party[who_att].status[eStatus::POISONED_WEAPON];
				if (pc_has_abil_equip(who_att,eItemAbil::POISON_AUGMENT) < 24)
					poison_amt += 2;
				poison_monst(which_m,poison_amt);
				move_to_zero(univ.party[who_att].status[eStatus::POISONED_WEAPON]);
			}
			if ((univ.party[who_att].items[weap1].ability == eItemAbil::POISONED_WEAPON) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drips venom.             ");
				poison_monst(which_m,univ.party[who_att].items[weap1].ability_strength / 2);
			}
			if ((univ.party[who_att].items[weap1].ability == eItemAbil::ACIDIC_WEAPON) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drips acid.             ");
				acid_monst(which_m,univ.party[who_att].items[weap1].ability_strength / 2);
			}
			if ((univ.party[who_att].items[weap1].ability == eItemAbil::SOULSUCKER) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drains life.             ");
				heal_pc(who_att,univ.party[who_att].items[weap1].ability_strength / 2);
			}
		}
		else {
			draw_terrain(2);
			sprintf ((char *) create_line, "  %s misses.              ",(char *) univ.party[who_att].name.c_str());
			add_string_to_buf((char *) create_line);
			if (what_skill1 == eSkill::POLE_WEAPONS)
				play_sound(19);
			else play_sound(2);
		}
	}
	if ((weap2 < 24) && (which_m->active > 0)) {
		what_skill2 = univ.party[who_att].items[weap2].type;
		
		// safety valve
		if (what_skill1 == eSkill::INVALID)
			what_skill1 = eSkill::EDGED_WEAPONS;
		
		
		sprintf(create_line, "%s swings.                    ",univ.party[who_att].name.c_str());//,hit_adj, dam_adj);
		add_string_to_buf(create_line);
		r1 = get_ran(1,1,100) + hit_adj - 5 * univ.party[who_att].items[weap2].bonus;
		
		// Ambidextrous?
		if(!univ.party[who_att].traits[eTrait::AMBIDEXTROUS])
			r1 += 25;
		
		r1 += 5 * (univ.party[current_pc].status[eStatus::WEBS] / 3);
		r2 = get_ran(1,1,univ.party[who_att].items[weap2].item_level) + dam_adj - 1 + univ.party[who_att].items[weap2].bonus;
		if (univ.party[who_att].items[weap2].ability == eItemAbil::WEAK_WEAPON)
			r2 = (r2 * (10 - univ.party[who_att].items[weap2].ability_strength)) / 10;
		
		if (r1 <= hit_chance[univ.party[who_att].skills[what_skill2]]) {
			spec_dam = calc_spec_dam(univ.party[who_att].items[weap2].ability,
									 univ.party[who_att].items[weap2].ability_strength,which_m);
			switch (what_skill2) {
			 	case eSkill::EDGED_WEAPONS:
					if (univ.party[who_att].items[weap1].item_level < 8)
						damage_monst(target, who_att, r2, spec_dam, DAMAGE_WEAPON,1);
					else damage_monst(target, who_att, r2, spec_dam, DAMAGE_WEAPON,2);
					break;
			 	case eSkill::BASHING_WEAPONS:
					damage_monst(target, who_att, r2, spec_dam, DAMAGE_WEAPON,4);
					break;
			 	case eSkill::POLE_WEAPONS:
					damage_monst(target, who_att, r2, spec_dam, DAMAGE_WEAPON,3);
					break;
			}
			
			if ((univ.party[who_att].items[weap2].ability == eItemAbil::POISONED_WEAPON) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drips venom.             ");
				poison_monst(which_m,univ.party[who_att].items[weap2].ability_strength / 2);
			}
			if ((univ.party[who_att].items[weap2].ability == eItemAbil::ACIDIC_WEAPON) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drips acid.             ");
				acid_monst(which_m,univ.party[who_att].items[weap2].ability_strength / 2);
			}
			if ((univ.party[who_att].items[weap2].ability == eItemAbil::SOULSUCKER) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drains life.             ");
				heal_pc(who_att,univ.party[who_att].items[weap2].ability_strength / 2);
			}
			
		}
		else {
			draw_terrain(2);
			sprintf ((char *) create_line, "%s misses.             ",(char *) univ.party[who_att].name.c_str());
			add_string_to_buf((char *) create_line);
			if (what_skill2 == eSkill::POLE_WEAPONS)
				play_sound(19);
			else play_sound(2);
		}
	}
	move_to_zero(univ.party[who_att].status[eStatus::POISONED_WEAPON]);
	take_ap(4);
	
	if((univ.town.monst[target].status[eStatus::MARTYRS_SHIELD] > 0 || univ.town.monst[target].spec_skill == 22)
		&& (store_hp - univ.town.monst[target].health > 0)) {
		add_string_to_buf("  Shares damage!   ");
		damage_pc(who_att, store_hp - univ.town.monst[target].health, DAMAGE_MAGIC,eRace::UNKNOWN,0);
	}
	combat_posing_monster = current_working_monster = -1;
	
}


short calc_spec_dam(eItemAbil abil,short abil_str,cCreature *monst) {
	short store = 0;
	
	switch (abil) {
		case eItemAbil::FLAMING_WEAPON:
		case eItemAbil::MISSILE_LIGHTNING:
			store += get_ran(abil_str,1,6);
			break;
		case eItemAbil::DEMON_SLAYER:
			if(monst->m_type == eRace::DEMON)
				store += 8 * abil_str;
			break;
		case eItemAbil::MISSILE_SLAY_DEMON:
			if(monst->m_type == eRace::DEMON)
				store += 25 + 8 * abil_str;
			break;
		case eItemAbil::MISSILE_SLAY_UNDEAD:
			if(monst->m_type == eRace::UNDEAD)
				store += 20 + 6 * abil_str;
			break;
		case eItemAbil::UNDEAD_SLAYER:
			if(monst->m_type == eRace::UNDEAD)
				store += 6 * abil_str;
			break;
		case eItemAbil::LIZARD_SLAYER:
			if(monst->m_type == eRace::REPTILE)
				store += 5 * abil_str;
			break;
		case eItemAbil::GIANT_SLAYER:
			if(monst->m_type == eRace::GIANT)
				store += 8 * abil_str;
			break;
		case eItemAbil::MAGE_SLAYER:
			if(monst->m_type == eRace::MAGE)
				store += 4 * abil_str;
			break;
		case eItemAbil::PRIEST_SLAYER:
			if(monst->m_type == eRace::PRIEST)
				store += 4 * abil_str;
			break;
		case eItemAbil::BUG_SLAYER:
			if(monst->m_type == eRace::BUG)
				store += 7 * abil_str;
			break;
		case eItemAbil::CAUSES_FEAR:
			scare_monst(monst,abil_str * 10);
			break;
		case eItemAbil::MISSILE_ACID:
			acid_monst(monst,abil_str);
			break;
	}
	return store;
}

void place_target(location target)
{
	short i;
	
 	if (num_targets_left > 0) {
		if (loc_off_act_area(target) == true) {
			add_string_to_buf("  Space not in town.           ");
			return;
		}
		if (can_see_light(univ.party[current_pc].combat_pos,target,sight_obscurity) > 4) {
			add_string_to_buf("  Can't see target.           ");
			return;
		}
		if (dist(univ.party[current_pc].combat_pos,target) > (*spell_being_cast).range)  {
			add_string_to_buf("  Target out of range.");
			return;
		}
		if(sight_obscurity(target.x,target.y) == 5 && spell_being_cast != eSpell::DISPEL_BARRIER) {
			add_string_to_buf("  Target space obstructed.           ");
			return;
		}
		if (univ.town.is_antimagic(target.x,target.y)) {
			add_string_to_buf("  Target in antimagic field.");
			return;
		}
		for (i = 0; i < 8; i++) {
			if (spell_targets[i] == target) {
				add_string_to_buf("  Target removed.");
				num_targets_left++;
				spell_targets[i].x = 120;
				play_sound(-1);
				return;
			}
		}
		for (i = 0; i < 8; i++)
			if (spell_targets[i].x == 120) {
				add_string_to_buf("  Target added.");
				spell_targets[i] = target;
				num_targets_left--;
				play_sound(0);
				i = 8;
			}
	}
	
	if (num_targets_left == 0) {
		do_combat_cast(spell_targets[0]);
		overall_mode = MODE_COMBAT;
	}
}

void do_combat_cast(location target)////
{
	short adjust,r1,r2,targ_num,level,bonus = 1,i,item,store_sound = 0;
	cCreature *cur_monst;
	bool freebie = false,ap_taken = false,cost_taken = false;
	short num_targets = 1,store_m_type = 2;
	eFieldType spray_type_array[15] = {
		FIELD_WEB,FIELD_WEB,FIELD_WEB,
		WALL_FORCE,WALL_FORCE,
		WALL_FIRE,WALL_FIRE,WALL_FIRE,
		FIELD_ANTIMAGIC,FIELD_ANTIMAGIC,
		CLOUD_STINK,CLOUD_STINK,
		WALL_ICE,WALL_ICE,WALL_BLADES,
	};
	m_num_t summon;
	
	location ashes_loc;
	
	// to wedge in animations, have to kludge like crazy
	short boom_dam[8] = {0,0,0,0,0,0,0,0};
	eDamageType boom_type[8];
	location boom_targ[8];
	
	if(spell_freebie) {
		freebie = true;
		level = store_item_spell_level;
		level = minmax(2,20,level);
	}
	else {
		level = 1 + univ.party[current_pc].level / 2;
		bonus = stat_adj(current_pc,eSkill::INTELLIGENCE);
	}
	force_wall_position = 10;
	
	void_sanctuary(current_pc);
	if (overall_mode == MODE_SPELL_TARGET) {
		spell_targets[0] = target;
	}
	else {
		num_targets = 8;
	}
	
	spell_caster = current_pc;
	
	// assign monster summoned, if summoning
	if(spell_being_cast == eSpell::SUMMON_BEAST) {
		summon = get_summon_monster(1);
		
	} else if (spell_being_cast == eSpell::SUMMON_WEAK) {
		summon = get_summon_monster(1);
	} else if (spell_being_cast == eSpell::SUMMON) {
		summon = get_summon_monster(2);
	} else if (spell_being_cast == eSpell::SUMMON_MAJOR) {
		summon = get_summon_monster(3);
	}
	combat_posing_monster = current_working_monster = current_pc;
	
	for (i = 0; i < num_targets; i++)
		if (spell_targets[i].x != 120) {
			target = spell_targets[i];
			spell_targets[i].x = 120; // nullify target as it is used
			
			if(!cost_taken && !freebie && spell_being_cast != eSpell::MINDDUEL && spell_being_cast != eSpell::SIMULACRUM) {
				univ.party[current_pc].cur_sp -= (*spell_being_cast).cost;
				cost_taken = true;
			}
			if(!cost_taken && !freebie && spell_being_cast == eSpell::SIMULACRUM) {
				univ.party[current_pc].cur_sp -=	store_sum_monst_cost;
				cost_taken = true;
			}
			
			if ((adjust = can_see_light(univ.party[current_pc].combat_pos,target,sight_obscurity)) > 4) {
				add_string_to_buf("  Can't see target.           ");
			}
			else if (loc_off_act_area(target) == true) {
				add_string_to_buf("  Space not in town.           ");
			}
			else if (dist(univ.party[current_pc].combat_pos,target) > (*spell_being_cast).range)
				add_string_to_buf("  Target out of range.");
			else if(sight_obscurity(target.x,target.y) == 5 && spell_being_cast != eSpell::DISPEL_BARRIER)
				add_string_to_buf("  Target space obstructed.           ");
			else if (univ.town.is_antimagic(target.x,target.y))
				add_string_to_buf("  Target in antimagic field.");
			else {
				if (ap_taken == false) {
					if (freebie == false)
						take_ap(5);
					ap_taken = true;
					draw_terrain(2);
				}
				boom_targ[i] = target;
				switch (spell_being_cast) {
						
					case eSpell::GOO: case eSpell::WEB: case eSpell::GOO_BOMB:
						place_spell_pattern(current_pat,target,FIELD_WEB,current_pc);
						break;
					case eSpell::CLOUD_FLAME: case eSpell::CONFLAGRATION:
						place_spell_pattern(current_pat,target,WALL_FIRE,current_pc);
						break;
					case eSpell::CLOUD_STINK: case eSpell::FOUL_VAPOR:
						place_spell_pattern(current_pat,target,CLOUD_STINK,current_pc);
						break;
					case eSpell::WALL_FORCE: case eSpell::SHOCKSTORM: case eSpell::FORCEFIELD:
						place_spell_pattern(current_pat,target,WALL_FORCE,current_pc);
						break;
					case eSpell::WALL_ICE: case eSpell::WALL_ICE_BALL:
						place_spell_pattern(current_pat,target,WALL_ICE,current_pc);
						break;
					case eSpell::ANTIMAGIC:
						place_spell_pattern(current_pat,target,FIELD_ANTIMAGIC,current_pc);
						break;
					case eSpell::CLOUD_SLEEP: case eSpell::CLOUD_SLEEP_LARGE:
						place_spell_pattern(current_pat,target,CLOUD_SLEEP,current_pc);
						break;
					case eSpell::QUICKFIRE:
						univ.town.set_quickfire(target.x,target.y,true);
						break;
					case eSpell::SPRAY_FIELDS:
						r1 = get_ran(1,0,14);
						place_spell_pattern(current_pat,target,spray_type_array[r1],current_pc);
						break;
					case eSpell::WALL_BLADES:
						place_spell_pattern(current_pat,target,WALL_BLADES,current_pc);
						break;
					case eSpell::DISPEL_FIELD: case eSpell::DISPEL_SPHERE: case eSpell::DISPEL_SQUARE:
						place_spell_pattern(current_pat,target,FIELD_DISPEL,current_pc);
						break;
					case eSpell::BARRIER_FIRE:
						play_sound(68);
						r1 = get_ran(3,2,7);
						hit_space(target,r1,DAMAGE_FIRE,true,true);
						univ.town.set_fire_barr(target.x,target.y,true);
						if (univ.town.is_fire_barr(target.x,target.y))
							add_string_to_buf("  You create the barrier.              ");
						else add_string_to_buf("  Failed.");
						break;
					case eSpell::BARRIER_FORCE:
						play_sound(68);
						r1 = get_ran(7,2,7);
						hit_space(target,r1,DAMAGE_FIRE,true,true);
						univ.town.set_force_barr(target.x,target.y,true);
						if (univ.town.is_force_barr(target.x,target.y))
							add_string_to_buf("  You create the barrier.              ");
						else add_string_to_buf("  Failed.");
						break;
						
					default: // spells which involve animations
						start_missile_anim();
						switch (spell_being_cast) {
								
							case eSpell::DIVINE_THUD:
								add_missile(target,9,1,0,0);
								store_sound = 11;
								r1 = min(18,(level * 7) / 10 + 2 * bonus);
								place_spell_pattern(rad2,target,DAMAGE_MAGIC,r1,current_pc);
								ashes_loc = target;
								break;
								
							case eSpell::SPARK: case eSpell::ICE_BOLT:
								r1 = (spell_being_cast == eSpell::SPARK) ? get_ran(2,1,4) : get_ran(min(20,level + bonus),1,4);
								add_missile(target,6,1,0,0);
								do_missile_anim(100,univ.party[current_pc].combat_pos,11);
								hit_space(target,r1,(spell_being_cast == eSpell::SPARK) ? DAMAGE_MAGIC : DAMAGE_COLD,1,0);
								break;
							case eSpell::ARROWS_FLAME:
								add_missile(target,4,1,0,0);
								r1 = get_ran(2,1,4);
								boom_type[i] = DAMAGE_FIRE;
								boom_dam[i] = r1;
								//hit_space(target,r1,1,1,0);
								break;
							case eSpell::SMITE:
								add_missile(target,6,1,0,0);
								r1 = get_ran(2,1,5);
								boom_type[i] = DAMAGE_COLD;
								boom_dam[i] = r1;
								//hit_space(target,r1,5,1,0);
								break;
							case eSpell::WOUND:
								r1 = get_ran(min(7,2 + bonus + level / 2),1,4);
								add_missile(target,14,1,0,0);
								do_missile_anim(100,univ.party[current_pc].combat_pos,24);
								hit_space(target,r1,DAMAGE_UNBLOCKABLE,1,0);
								break;
							case eSpell::FLAME:
								r1 = get_ran(min(10,1 + level / 3 + bonus),1,6);
								add_missile(target,2,1,0,0);
								do_missile_anim(100,univ.party[current_pc].combat_pos,11);
								hit_space(target,r1,DAMAGE_FIRE,1,0);
								break;
							case eSpell::FIREBALL: case eSpell::FLAMESTRIKE:
								r1 = min(9,1 + (level * 2) / 3 + bonus) + 1;
								add_missile(target,2,1,0,0);
								store_sound = 11;
								//do_missile_anim(100,univ.party[current_pc].combat_pos,11);
								if (spell_being_cast == eSpell::FLAMESTRIKE)
									r1 = (r1 * 14) / 10;
								else if (r1 > 10) r1 = (r1 * 8) / 10;
								if (r1 <= 0) r1 = 1;
								place_spell_pattern(square,target,DAMAGE_FIRE,r1,current_pc);
								ashes_loc = target;
								break;
							case eSpell::FIRESTORM:
								add_missile(target,2,1,0,0);
								store_sound = 11;
								//do_missile_anim(100,univ.party[current_pc].combat_pos,11);
								r1 = min(12,1 + (level * 2) / 3 + bonus) + 2;
								if (r1 > 20)
									r1 = (r1 * 8) / 10;
								place_spell_pattern(rad2,target,DAMAGE_FIRE,r1,current_pc);
								ashes_loc = target;
								break;
							case eSpell::KILL:
								add_missile(target,9,1,0,0);
								do_missile_anim(100,univ.party[current_pc].combat_pos,11);
								r1 = get_ran(3,0,10) + univ.party[current_pc].level * 2;
								hit_space(target,40 + r1,DAMAGE_MAGIC,1,0);
								break;
							case eSpell::ARROWS_DEATH:
								add_missile(target,9,1,0,0);
								store_sound = 11;
								r1 = get_ran(3,0,10) + univ.party[current_pc].level + 3 * bonus;
								boom_type[i] = DAMAGE_MAGIC;
								boom_dam[i] = r1;
								//hit_space(target,40 + r1,3,1,0);
								break;
								// summoning spells
							case eSpell::SIMULACRUM: case eSpell::SUMMON_BEAST: case eSpell::SUMMON_RAT:
							case eSpell::SUMMON_WEAK: case eSpell::SUMMON: case eSpell::SUMMON_MAJOR:
							case eSpell::DEMON: case eSpell::SUMMON_SPIRIT: case eSpell::STICKS_TO_SNAKES:
							case eSpell::SUMMON_HOST: case eSpell::SUMMON_GUARDIAN:
								add_missile(target,8,1,0,0);
								do_missile_anim(50,univ.party[current_pc].combat_pos,61);
								switch (spell_being_cast) {
									case eSpell::SIMULACRUM:
										r2 = get_ran(3,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if (summon_monster(store_sum_monst,target,r2,2) == false)
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_BEAST:
										r2 = get_ran(3,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if ((summon == 0) || (!summon_monster(summon,target,r2,2)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_WEAK:
										r2 = get_ran(4,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if ((summon == 0) || (!summon_monster(summon,target,r2,2)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON:
										r2 = get_ran(5,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if ((summon == 0) || (!summon_monster(summon,target,r2,2)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_MAJOR:
										r2 = get_ran(7,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if ((summon == 0) || (!summon_monster(summon,target,r2,2)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::DEMON:
										r2 = get_ran(5,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if (!summon_monster(85,target,r2,2))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_RAT:
										r1 = get_ran(3,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if (!summon_monster(80,target,r1,2))
											add_string_to_buf("  Summon failed.");
										break;
										
									case eSpell::SUMMON_SPIRIT:
										r2 = get_ran(2,1,5) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if (summon_monster(125,target,r2,2) == false)
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::STICKS_TO_SNAKES:
										r1 = get_ran(1,0,7);
										r2 = get_ran(2,1,5) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if (summon_monster((r1 == 1) ? 100 : 99,target,r2,2) == false)
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_HOST: // host
										r2 = get_ran(2,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if (summon_monster((i == 0) ? 126 : 125,target,r2,2) == false)
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_GUARDIAN: // guardian
										r2 = get_ran(6,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if (summon_monster(122,target,r2,2) == false)
											add_string_to_buf("  Summon failed.");
										break;
								}
								break;
								
								
							default:
								targ_num = monst_there(target);
								if (targ_num > univ.town->max_monst())
									add_string_to_buf("  Nobody there                 ");
								else {
									cur_monst = &univ.town.monst[targ_num];
									cCreature* cur_monst = &univ.town.monst[targ_num];
									if(cur_monst->attitude % 2 != 1 && spell_being_cast != eSpell::SCRY_MONSTER && spell_being_cast != eSpell::CAPTURE_SOUL)
										make_town_hostile();
									switch (spell_being_cast) {
										case eSpell::ACID_SPRAY:
											store_m_type = 0;
											acid_monst(cur_monst,level);
											store_sound = 24;
											break;
										case eSpell::PARALYZE_BEAM:
											store_m_type = 9;
											charm_monst(cur_monst,0,eStatus::PARALYZED,500);
											store_sound = 24;
											break;
											
										case eSpell::SCRY_MONSTER:
											store_m_type = -1;
											play_sound(52);
											univ.party.m_noted[cur_monst->number] = true;
											adjust_monst_menu();
											display_monst(0,cur_monst,0);
											store_sound = 25;
											break;
										case eSpell::CAPTURE_SOUL:
											store_m_type = 15;
											record_monst(cur_monst);
											store_sound = 25;
											break;
											
										case eSpell::MINDDUEL:
											store_m_type = -1;
											if ((cur_monst->mu == 0) && (cur_monst->cl == 0))
												add_string_to_buf("  Can't duel: no magic.");
											else {
												item = pc_has_abil(current_pc,eItemAbil::SMOKY_CRYSTAL);
												if (item >= 24)
													add_string_to_buf("  You need a smoky crystal.   ");
												else {
													remove_charge(current_pc,item);
													do_mindduel(current_pc,cur_monst);
												}
											}
											store_sound = 24;
											break;
											
										case eSpell::CHARM_FOE:
											store_m_type = 14;
											charm_monst(cur_monst,-1 * (bonus + univ.party[current_pc].level / 8),eStatus::CHARM,0);
											store_sound = 24;
											break;
											
										case eSpell::DISEASE:
											store_m_type = 0;
											r1 = get_ran(1,0,1);
											disease_monst(cur_monst,2 + r1 + bonus);
											store_sound = 24;
											break;
											
										case eSpell::STRENGTHEN_TARGET:
											store_m_type = 14;
											cur_monst->health += 20;
											store_sound = 55;
											break;
											
										case eSpell::DUMBFOUND:
											store_m_type = 14;
											dumbfound_monst(cur_monst,1 + bonus / 3);
											store_sound = 53;
											break;
											
										case eSpell::SCARE:
											store_m_type = 11;
											scare_monst(cur_monst,get_ran(2 + bonus,1,6));
											store_sound = 54;
											break;
										case eSpell::FEAR:
											store_m_type = 11;
											scare_monst(cur_monst,get_ran(min(20,univ.party[current_pc].level / 2 + bonus),1,8));
											store_sound = 54;
											break;
											
										case eSpell::SLOW:
											store_m_type = 11;
											r1 = get_ran(1,0,1);
											slow_monst(cur_monst,2 + r1 + bonus);
											store_sound = 25;
											break;
											
										case eSpell::POISON_MINOR: case eSpell::ARROWS_VENOM:
											store_m_type = (spell_being_cast == eSpell::ARROWS_VENOM) ? 4 : 11;
											poison_monst(cur_monst,2 + bonus / 2);
											store_sound = 55;
											break;
										case eSpell::PARALYZE:
											store_m_type = 9;
											charm_monst(cur_monst,-10,eStatus::PARALYZED,1000);
											store_sound = 25;
											break;
										case eSpell::POISON:
											store_m_type = 11;
											poison_monst(cur_monst,4 + bonus / 2);
											store_sound = 55;
											break;
										case eSpell::POISON_MAJOR:
											store_m_type = 11;
											poison_monst(cur_monst,8 + bonus / 2);
											store_sound = 55;
											break;
											
										case eSpell::STUMBLE:
											store_m_type = 8;
											curse_monst(cur_monst,4 + bonus);
											store_sound = 24;
											break;
											
										case eSpell::CURSE:
											store_m_type = 8;
											curse_monst(cur_monst,2 + bonus);
											store_sound = 24;
											break;
											
										case eSpell::HOLY_SCOURGE:
											store_m_type = 8;
											curse_monst(cur_monst,2 + univ.party[current_pc].level / 2);
											store_sound = 24;
											break;
											
										case eSpell::TURN_UNDEAD: case eSpell::DISPEL_UNDEAD:
											if (cur_monst->m_type != eRace::UNDEAD) {
												add_string_to_buf("  Not undead.                    ");
												store_m_type = -1;
												break;
											}
											store_m_type = 8;
											r1 = get_ran(1,0,90);
											if (r1 > hit_chance[minmax(0,19,bonus * 2 + level * 4 - (cur_monst->level / 2) + 3)])
												add_string_to_buf("  Monster resisted.                  ");
											else {
												r1 = get_ran((spell_being_cast == eSpell::TURN_UNDEAD) ? 2 : 6, 1, 14);
												
												hit_space(cur_monst->cur_loc,r1,DAMAGE_UNBLOCKABLE,0,current_pc);
											}
											store_sound = 24;
											break;
											
										case eSpell::RAVAGE_SPIRIT:
											if (cur_monst->m_type != eRace::DEMON) {
												add_string_to_buf("  Not a demon.                    ");
												store_m_type = -1;
												break;
											}
											r1 = get_ran(1,1,100);
											if (r1 > hit_chance[minmax(0,19,level * 4 - cur_monst->level + 10)])
												add_string_to_buf("  Demon resisted.                  ");
											else {
												r1 = get_ran(8 + bonus * 2, 1, 11);
												//if (PSD[4][0] == 3) // anama
												//	r1 += 25;
												//play_sound(53);
												hit_space(cur_monst->cur_loc,r1,DAMAGE_UNBLOCKABLE,0,current_pc);
											}
											store_sound = 24;
											break;
									}
									if (store_m_type >= 0)
										add_missile(target,store_m_type,1,
													14 * (cur_monst->x_width - 1),18 * (cur_monst->y_width - 1));
									
								}
								break;
						}
				}
			}
		}
	
	do_missile_anim((num_targets > 1) ? 35 : 60,univ.party[current_pc].combat_pos,store_sound);
	
	// process mass damage
	for (i = 0; i < 8; i++)
		if (boom_dam[i] > 0)
			hit_space(boom_targ[i],boom_dam[i],boom_type[i],1,0);
	
	if (ashes_loc.x > 0)
		univ.town.set_ash(ashes_loc.x,ashes_loc.y,true);
	
	do_explosion_anim(5,0);
	
	end_missile_anim();
	
	handle_marked_damage();
	combat_posing_monster = current_working_monster = -1;
	
	print_buf();
}

void handle_marked_damage()
{
	short i;
	
	for (i = 0; i < 6; i++)
		if (univ.party[i].marked_damage > 0)
		{
			// TODO: Perhaps there should be a way of determining the correct race here?
			damage_pc(i,univ.party[i].marked_damage,DAMAGE_MARKED,eRace::UNKNOWN,0);
			univ.party[i].marked_damage = 0;
		}
	for (i = 0; i < univ.town->max_monst(); i++)
		if (monst_marked_damage[i] > 0)
		{
			damage_monst(i, current_pc, monst_marked_damage[i], 0, DAMAGE_MARKED,0); // was 9 rather than 10; probably a mistake
			
			monst_marked_damage[i] = 0;
		}
}

void load_missile() ////
{
	short i,bow = 24,arrow = 24,thrown = 24,crossbow = 24,bolts = 24,no_ammo = 24;
	
	for (i = 0; i < 24; i++) {
		if ((univ.party[current_pc].equip[i] == true) &&
			(univ.party[current_pc].items[i].variety == eItemType::THROWN_MISSILE))
			thrown = i;
		if ((univ.party[current_pc].equip[i] == true) &&
			(univ.party[current_pc].items[i].variety == eItemType::BOW))
			bow = i;
		if ((univ.party[current_pc].equip[i] == true) &&
			(univ.party[current_pc].items[i].variety == eItemType::ARROW))
			arrow = i;
		if ((univ.party[current_pc].equip[i] == true) &&
			(univ.party[current_pc].items[i].variety == eItemType::CROSSBOW))
			crossbow = i;
		if ((univ.party[current_pc].equip[i] == true) &&
			(univ.party[current_pc].items[i].variety == eItemType::BOLTS))
			bolts = i;
		if ((univ.party[current_pc].equip[i] == true) &&
			(univ.party[current_pc].items[i].variety == eItemType::MISSILE_NO_AMMO))
			no_ammo = i;
	}
	
	if (thrown < 24) {
		ammo_inv_slot = thrown;
		add_string_to_buf("Throw: Select a target.        ");
		add_string_to_buf("  (Hit 's' to cancel.)");
		overall_mode = MODE_THROWING;
		current_spell_range = 8;
		current_pat = single;
	}
	else if (((bolts < 24) && (bow < 24)) || ((arrow < 24) && (crossbow < 24)))  {
		add_string_to_buf("Fire: Wrong ammunition.       ");
	}
	else if ((arrow == 24) && (bow < 24)) {
		add_string_to_buf("Fire: Equip some arrows.       ");
	}
	else if(crossbow == 24 && bolts < 24) {
		add_string_to_buf("Fire: Equip some bolts.        ");
	}
	else if ((arrow < 24) && (bow < 24)) {
		missile_inv_slot = bow;
		ammo_inv_slot = arrow;
		overall_mode = MODE_FIRING;
		add_string_to_buf("Fire: Select a target.        ");
		add_string_to_buf("  (Hit 's' to cancel.)");
		current_spell_range = 12;
		if(univ.party[current_pc].items[arrow].ability == eItemAbil::MISSILE_EXPLODING)
			current_pat = rad2;
		else
			current_pat = single;
	}
	else if ((bolts < 24) && (crossbow < 24)) {
		missile_inv_slot = crossbow;
		ammo_inv_slot = bolts;
		overall_mode = MODE_FIRING;
		add_string_to_buf("Fire: Select a target.        ");
		add_string_to_buf("  (Hit 's' to cancel.)");
		current_spell_range = 12;
		current_pat = single;
	}
	else if (no_ammo < 24) {
		missile_inv_slot = no_ammo;
		ammo_inv_slot = no_ammo;
		overall_mode = MODE_FIRING;
		add_string_to_buf("Fire: Select a target.        ");
		add_string_to_buf("  (Hit 's' to cancel.)");
		current_spell_range = 12;
		current_pat = single;
	}
	else add_string_to_buf("Fire: Equip a missile.       ");
}

void fire_missile(location target) {
	short r1, r2, skill, dam, dam_bonus, hit_bonus, range, targ_monst, spec_dam = 0,poison_amt = 0;
	short skill_item,m_type = 0;
	cCreature *cur_monst;
	bool exploding = false;
	missile_firer = current_pc;
	
	skill = overall_mode == MODE_FIRING ? univ.party[missile_firer].skills[eSkill::ARCHERY] : univ.party[missile_firer].skills[eSkill::THROWN_MISSILES];
	range = (overall_mode == MODE_FIRING) ? 12 : 8;
	dam = univ.party[missile_firer].items[ammo_inv_slot].item_level;
	dam_bonus = univ.party[missile_firer].items[ammo_inv_slot].bonus + minmax(-8,8,univ.party[missile_firer].status[eStatus::BLESS_CURSE]);
	hit_bonus = (overall_mode == MODE_FIRING) ? univ.party[missile_firer].items[missile_inv_slot].bonus : 0;
	hit_bonus += stat_adj(missile_firer,eSkill::DEXTERITY) - can_see_light(univ.party[missile_firer].combat_pos,target,sight_obscurity)
		+ minmax(-8,8,univ.party[missile_firer].status[eStatus::BLESS_CURSE]);
	if ((skill_item = pc_has_abil_equip(missile_firer,eItemAbil::ACCURACY)) < 24) {
		hit_bonus += univ.party[missile_firer].items[skill_item].ability_strength / 2;
		dam_bonus += univ.party[missile_firer].items[skill_item].ability_strength / 2;
	}
	
	// race adj.
	// TODO: Should this apply to sliths as well? The bladbase suggests otherwise, but it has been changed from the original; maybe the sliths were originally considered to be reptiles.
	if(univ.party[missile_firer].race == eRace::REPTILE)
		hit_bonus += 2;
	
	if (univ.party[missile_firer].items[ammo_inv_slot].ability == eItemAbil::MISSILE_EXPLODING)
		exploding = true;
	
	if (dist(univ.party[missile_firer].combat_pos,target) > range)
		add_string_to_buf("  Out of range.");
	else if (can_see_light(univ.party[missile_firer].combat_pos,target,sight_obscurity) >= 5)
		add_string_to_buf("  Can't see target.             ");
	else {
		// First, some missiles do special things
		if (exploding) {
			take_ap((overall_mode == MODE_FIRING) ? 3 : 2);
			void_sanctuary(current_pc); // TODO: Is this right?
			missile_firer = current_pc;
			add_string_to_buf("  The arrow explodes!             ");
			if(PSD[SDF_GAME_SPEED] == 0)
				pause(dist(univ.party[current_pc].combat_pos,target));
			else
				pause(dist(univ.party[current_pc].combat_pos,target)*5);
			run_a_missile(univ.party[missile_firer].combat_pos,target,2,1,5,0,0,100);
			start_missile_anim();
			place_spell_pattern(rad2,target, DAMAGE_FIRE,univ.party[missile_firer].items[ammo_inv_slot].ability_strength * 2,missile_firer);
			do_explosion_anim(5,0);
			end_missile_anim();
			handle_marked_damage();
		} else {
			combat_posing_monster = current_working_monster = missile_firer;
			draw_terrain(2);
			void_sanctuary(missile_firer);
			//play_sound((overall_mode == MODE_FIRING) ? 12 : 14);
			take_ap((overall_mode == MODE_FIRING) ? 3 : 2);
			missile_firer = missile_firer;
			r1 = get_ran(1,1,100) - 5 * hit_bonus - 10;
			r1 += 5 * (univ.party[missile_firer].status[eStatus::WEBS] / 3);
			r2 = get_ran(1,1,dam) + dam_bonus;
			sprintf ((char *) create_line, "%s fires.",(char *) univ.party[missile_firer].name.c_str()); // debug
			add_string_to_buf((char *) create_line);
			
			if (overall_mode == MODE_THROWING) {
				switch (univ.party[missile_firer].items[ammo_inv_slot].item_level) {
					case 7:
						m_type = 10;
						break;
					case 4:
						m_type = 1;
						break;
					case 8:
						m_type = 5;
						break;
					case 9:
						m_type = 7;
						break;
					default:
						m_type = 10;
						break;
				}
			} else if (overall_mode == MODE_FIRING || overall_mode == MODE_FANCY_TARGET)
				m_type = univ.party[missile_firer].items[ammo_inv_slot].magic ? 4 : 3;
			run_a_missile(univ.party[missile_firer].combat_pos,target,m_type,1,(overall_mode == MODE_FIRING) ? 12 : 14,
						  0,0,100);
			
			if (r1 > hit_chance[skill])
				add_string_to_buf("  Missed.");
			else if ((targ_monst = monst_there(target)) < univ.town->max_monst()) {
				cur_monst = &univ.town.monst[targ_monst];
				spec_dam = calc_spec_dam(univ.party[missile_firer].items[ammo_inv_slot].ability,
										 univ.party[missile_firer].items[ammo_inv_slot].ability_strength,cur_monst);
				if(univ.party[missile_firer].items[ammo_inv_slot].ability == eItemAbil::MISSILE_HEAL_TARGET) {
					ASB("  There is a flash of light.");
					cur_monst->health += r2;
				}
				else damage_monst(targ_monst, missile_firer, r2, spec_dam, DAMAGE_WEAPON,13);
				
				//if (univ.party[missile_firer].items[ammo_inv_slot].ability == 33)
				//	hit_space(cur_monst->m_loc,get_ran(3,1,6),1,1,1);
				
				// poison
				if(univ.party[missile_firer].status[eStatus::POISONED_WEAPON] > 0 && univ.party[missile_firer].weap_poisoned == ammo_inv_slot) {
					poison_amt = univ.party[missile_firer].status[eStatus::POISONED_WEAPON];
					if (pc_has_abil_equip(missile_firer,eItemAbil::POISON_AUGMENT) < 24)
						poison_amt++;
					poison_monst(cur_monst,poison_amt);
				}
			}
//			else if((targ_monst = pc_there(target)) < 6 && univ.party[current_pc].items[ammo_inv_slot].ability == 176){
//				ASB("  There is a flash of light.");
//				heal_pc(targ_monst,r2);
//			}
			else hit_space(target,r2,DAMAGE_WEAPON,1,0);
			
		}
		
		if (univ.party[missile_firer].items[ammo_inv_slot].variety != eItemType::MISSILE_NO_AMMO) {
			if(univ.party[missile_firer].items[ammo_inv_slot].ability != eItemAbil::MISSILE_RETURNING)
				univ.party[missile_firer].items[ammo_inv_slot].charges--;
			else univ.party[missile_firer].items[ammo_inv_slot].charges = 1;
			if(pc_has_abil_equip(missile_firer,eItemAbil::DRAIN_MISSILES) < 24
			   && univ.party[missile_firer].items[ammo_inv_slot].ability != eItemAbil::MISSILE_RETURNING)
				univ.party[missile_firer].items[ammo_inv_slot].charges--;
			if (univ.party[missile_firer].items[ammo_inv_slot].charges <= 0)
				take_item(missile_firer,ammo_inv_slot);
		}
	}
	
	if(!exploding){
		combat_posing_monster = current_working_monster = -1;
		move_to_zero(univ.party[missile_firer].status[eStatus::POISONED_WEAPON]);
	}
	print_buf();
}

// Select next active PC and, if necessary, run monsters
// if monsters go or PC switches (i.e. if need redraw above), return true
bool combat_next_step()
{
	bool to_return = false;
	short store_pc; // will print current pc name is active pc changes
	
	store_pc = current_pc;
	while (pick_next_pc() == true) {
		combat_run_monst();
		set_pc_moves();
		if((combat_active_pc < 6) && (univ.party[combat_active_pc].ap == 0)){
			combat_active_pc = 6;
			ASB("The active character is unable to act!");
			ASB("The whole party is now active.");
		}
		to_return = true;
		// Safety valve
		if (party_toast() == true)
			return true;
	}
	pick_next_pc();
	if (current_pc != store_pc)
		to_return = true;
	center = univ.party[current_pc].combat_pos;
	//if (ensure_redraw == true)
	//	draw_terrain(0);
	
	adjust_spell_menus();
	
	// In case running monsters affected active PC...
/*	if (univ.party[current_pc].status[3] < 0)
		this_pc_hasted = false;
	if ((univ.party[current_pc].main_status != 1) ||
		((univ.party[current_pc].status[3] < 0) && (univ.party.age % 2 == 0)))
		pick_next_pc();
	center = univ.party[current_pc].combat_pos;		*/
	
	if ((combat_active_pc == 6) && (current_pc != store_pc)) {
		sprintf(create_line, "Active:  %s (#%d, %d ap.)                     ",
				univ.party[current_pc].name.c_str(),current_pc + 1,univ.party[current_pc].ap);
		add_string_to_buf(create_line);
		print_buf();
	}
	if ((current_pc != store_pc) || (to_return == true)) {
		put_pc_screen();
		set_stat_window(current_pc);
	}
	return to_return;
}

// Find next active PC, return true is monsters need running, and run monsters is slow spells
// active
bool pick_next_pc()
{
	bool store = false;
	
	if (current_pc == 6)
		current_pc = 0;
	
	// If current pc isn't active, fry its moves
	if ((combat_active_pc < 6) && (combat_active_pc != current_pc))
		univ.party[current_pc].ap = 0;
	
	// Find next PC with moves
	while ((univ.party[current_pc].ap <= 0) && (current_pc < 6)) {
		current_pc++;
		if ((combat_active_pc < 6) && (combat_active_pc != current_pc))
			univ.party[current_pc].ap = 0;
	}
	
	// If run out of PC's, return to start and try again
	if (current_pc == 6) {
		current_pc = 0;
		while ((univ.party[current_pc].ap <= 0) && (current_pc < 6)) {
			current_pc++;
			if ((combat_active_pc < 6) && (combat_active_pc != current_pc))
				univ.party[current_pc].ap = 0;
		}
		if (current_pc == 6) {
			store = true;
			current_pc = 0;
		}
	}
	
	return store;
}


void combat_run_monst()
{
	short i,item,item_level;
	bool update_stat = false;
	
	
	monsters_going = true;
	do_monster_turn();
	monsters_going = false;
	
	process_fields();
	move_to_zero(univ.party.light_level);
	if ((which_combat_type == 1) && (univ.town->lighting_type == 2))
		univ.party.light_level = max (0,univ.party.light_level - 9);
	if (univ.town->lighting_type == 3)
		univ.party.light_level = 0;
	
	move_to_zero(PSD[SDF_PARTY_DETECT_LIFE]);
	move_to_zero(PSD[SDF_PARTY_FIREWALK]);
	
	// decrease monster present counter
	move_to_zero(PSD[SDF_HOSTILES_PRESENT]);
	
	dump_gold(1);
	
	univ.party.age++;
	if (univ.party.age % 4 == 0)
		for (i = 0; i < 6; i++) {
			if(univ.party[i].status[eStatus::BLESS_CURSE] != 0 || univ.party[i].status[eStatus::HASTE_SLOW] != 0)
				update_stat = true;
			move_to_zero(univ.party[i].status[eStatus::BLESS_CURSE]);
			move_to_zero(univ.party[i].status[eStatus::HASTE_SLOW]);
			move_to_zero(PSD[SDF_PARTY_STEALTHY]);
			if ((item = pc_has_abil_equip(i,eItemAbil::REGENERATE)) < 24) {
				update_stat = true;
				heal_pc(i,get_ran(1,0,univ.party[i].items[item].item_level + 1));
			}
		}
	for (i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
			if(univ.party[i].status[eStatus::INVULNERABLE] != 0 || univ.party[i].status[eStatus::MAGIC_RESISTANCE] != 0
				|| univ.party[i].status[eStatus::INVISIBLE] != 0 || univ.party[i].status[eStatus::MARTYRS_SHIELD] != 0
				|| univ.party[i].status[eStatus::ASLEEP] != 0 || univ.party[i].status[eStatus::PARALYZED] != 0)
				update_stat = true;
			
			move_to_zero(univ.party[i].status[eStatus::INVULNERABLE]);
			move_to_zero(univ.party[i].status[eStatus::MAGIC_RESISTANCE]);
			move_to_zero(univ.party[i].status[eStatus::INVISIBLE]);
			move_to_zero(univ.party[i].status[eStatus::MARTYRS_SHIELD]);
			move_to_zero(univ.party[i].status[eStatus::ASLEEP]);
			move_to_zero(univ.party[i].status[eStatus::PARALYZED]);
			
			// Do special items
			if (((item_level = get_prot_level(i,eItemAbil::OCCASIONAL_HASTE)) > 0)
				&& (get_ran(1,0,10) == 5)) {
				update_stat = true;
				univ.party[i].status[eStatus::HASTE_SLOW] += item_level / 2;
				add_string_to_buf("An item hastes you!");
			}
			if ((item_level = get_prot_level(i,eItemAbil::OCCASIONAL_BLESS)) > 0) {
				if (get_ran(1,0,10) == 5) {
					update_stat = true;
					univ.party[i].status[eStatus::BLESS_CURSE] += item_level / 2;
					add_string_to_buf("An item blesses you!");
				}
			}
			
			
		}
	
	special_increase_age();
	push_things();
	
	if (univ.party.age % 2 == 0)
		do_poison();
	if (univ.party.age % 3 == 0)
		handle_disease();
	handle_acid();
	
	if (update_stat == true)
		put_pc_screen();
	
}

void do_monster_turn()
{
	bool acted_yet, had_monst = false,printed_poison = false,printed_disease = false,printed_acid = false;
	bool redraw_not_yet_done = true;
	bool special_called = false;
	location targ_space,move_targ,l;
	short i,j,k,num_monst, target,r1,move_target;
	cCreature *cur_monst;
	bool pc_adj[6];
	short abil_range[40] = {
		0,6,8,8,10, 10,10,8,6,8, 6,0,0,0,6, 0,0,0,0,4, 10,0,0,6,0,
		0,0,0,0,0, 0,0,8,6,9, 0,0,0,0,0};
	short abil_odds[40] = {
		0,5,7,6,6, 5,5,6,6,6, 6,0,0,0,4, 0,0,0,0,4, 8,0,0,7,0,
		0,0,0,0,0, 0,0,7,5,6, 0,0,0,0,0};
	
	monsters_going = true; // This affects how graphics are drawn.
	
	num_monst = univ.town->max_monst();
	if (overall_mode < MODE_COMBAT)
		which_combat_type = 1;
	
	for (i = 0; i < num_monst; i++) {  // Give monsters ap's, check activity
		
		cur_monst = &univ.town.monst[i];
		
		// See if hostile monster notices party, during combat
		if ((cur_monst->active == 1) && (cur_monst->attitude % 2 == 1) && (overall_mode == MODE_COMBAT)) {
			r1 = get_ran(1,1,100); // Check if see PCs first
			r1 += (PSD[SDF_PARTY_STEALTHY] > 0) ? 45 : 0;
			r1 += can_see_light(cur_monst->cur_loc,closest_pc_loc(cur_monst->cur_loc),sight_obscurity) * 10;
			if (r1 < 50)
				cur_monst->active = 2;
			
			for (j = 0; j < univ.town->max_monst(); j++)
				if (monst_near(j,cur_monst->cur_loc,5,1) == true) {
					cur_monst->active = 2;
				}
		}
		if ((cur_monst->active == 1) && (cur_monst->attitude % 2 == 1)) {
			// Now it looks for PC-friendly monsters
			// dist check is for efficiency
			for (j = 0; j < univ.town->max_monst(); j++)
				if ((univ.town.monst[j].active > 0) &&
					(univ.town.monst[j].attitude % 2 != 1) &&
					(dist(cur_monst->cur_loc,univ.town.monst[j].cur_loc) <= 6) &&
					(can_see_light(cur_monst->cur_loc,univ.town.monst[j].cur_loc,sight_obscurity) < 5))
					cur_monst->active = 2;
		}
		
		// See if friendly, fighting monster see hostile monster. If so, make mobile
		// dist check is for efficiency
		if ((cur_monst->active == 1) && (cur_monst->attitude == 2)) {
			for (j = 0; j < univ.town->max_monst(); j++)
				if ((univ.town.monst[j].active > 0) && (univ.town.monst[j].attitude % 2 == 1) &&
					(dist(cur_monst->cur_loc,univ.town.monst[j].cur_loc) <= 6)
					&& (can_see_light(cur_monst->cur_loc,univ.town.monst[j].cur_loc,sight_obscurity) < 5)) {
					cur_monst->active = 2;
					cur_monst->mobility = 1;
				}
		}
		// End of seeing if monsters see others
		
		cur_monst->ap = 0;
		if (cur_monst->active == 2) { // Begin action loop for angry, active monsters
			// First note that hostile monsters are around.
			if (cur_monst->attitude % 2 == 1)
				PSD[SDF_HOSTILES_PRESENT] = 30;
			
			// Give monster its action points
			cur_monst->ap = cur_monst->speed;
			if (is_town())
				cur_monst->ap = max(1,cur_monst->ap / 3);
			if (univ.party.age % 2 == 0)
				if(cur_monst->status[eStatus::HASTE_SLOW] < 0)
					cur_monst->ap = 0;
			if (cur_monst->ap > 0) { // adjust for webs
				cur_monst->ap = max(0,cur_monst->ap - cur_monst->status[eStatus::WEBS] / 2);
				if (cur_monst->ap == 0)
					cur_monst->status[eStatus::WEBS] = max(0,cur_monst->status[eStatus::WEBS] - 2);
			}
			if(cur_monst->status[eStatus::HASTE_SLOW] > 0)
				cur_monst->ap *= 2;
		}
		if(cur_monst->status[eStatus::ASLEEP] > 0 || cur_monst->status[eStatus::PARALYZED] > 0)
			cur_monst->ap = 0;
		if (in_scen_debug == true)
			cur_monst->ap = 0;
		center_on_monst = false;
		
		// Now take care of summoned monsters
		if (cur_monst->active > 0) {
			if ((cur_monst->summoned % 100) == 1) {
				cur_monst->active = 0;
				cur_monst->ap = 0;
				monst_spell_note(cur_monst->number,17);
			}
			move_to_zero(cur_monst->summoned);
		}
		
	}
	
	for (i = 0; i < num_monst; i++) {  // Begin main monster loop, do monster actions
		// If party dead, no point
		if (party_toast() == true)
			return;
		
		futzing = 0; // assume monster is fresh
		
		cur_monst = &univ.town.monst[i];
		
		
		for (j = 0; j < 6; j++)
			if(univ.party[j].main_status == eMainStatus::ALIVE && monst_adjacent(univ.party[j].combat_pos,i))
				pc_adj[j] = true;
			else pc_adj[j] = false;
		
		
		
		while ((cur_monst->ap > 0) && (cur_monst->active > 0)) {  // Spend each action point
			
			if (is_combat()) { // Pick target. If in town, target already picked
				// in do_monsters
				target = monst_pick_target(i);
				target = switch_target_to_adjacent(i,target);
				if (target < 6)
					targ_space = univ.party[target].combat_pos;
				else if (target != 6)
					targ_space = univ.town.monst[target - 100].cur_loc;
				univ.town.monst[i].target = target;
			}
			else {
				if (univ.town.monst[i].target < 6)
					targ_space = univ.town.p_loc;
				else if (univ.town.monst[i].target != 6)
					targ_space = univ.town.monst[univ.town.monst[i].target - 100].cur_loc;
			}
			
//			sprintf((char *)create_line,"  %d targets %d.",i,target);
//			add_string_to_buf((char *) create_line);
			
			if ((univ.town.monst[i].target < 0) || ((univ.town.monst[i].target > 5) &&
													(univ.town.monst[i].target < 100)))
				univ.town.monst[i].target = 6;
			target = univ.town.monst[i].target;
			
			// Now if in town and monster about to attack, do a redraw, so we see monster
			// in right place
			if ((target != 6) && (is_town() == true) && (redraw_not_yet_done == true)
				&& (party_can_see_monst(i) == true)) {
				draw_terrain(0);
				redraw_not_yet_done = false;
			}
			
			// Draw w. monster in center, if can see
			if ((cur_monst->ap > 0) && (is_combat() == true)
				// First make sure it has a target and is close, if not, don't bother
				&& (cur_monst->attitude > 0) && (cur_monst->picture_num > 0)
				&& ((target != 6) || (cur_monst->attitude % 2 == 1))
				&& (party_can_see_monst(i) == true) ) {
				center_on_monst = true;
				center = cur_monst->cur_loc;
				draw_terrain(0);
				pause((PSD[SDF_GAME_SPEED] == 3) ? 9 : PSD[SDF_GAME_SPEED]);
			}
			
			combat_posing_monster = current_working_monster = 100 + i;
			
			acted_yet = false;
			
			
			// Now the monster, if evil, looks at the situation and maybe picks a tactic.
			// This only happens when there is >1 a.p. left, and tends to involve
			// running to a nice position.
			current_monst_tactic = 0;
			if ((target != 6) && (cur_monst->ap > 1) && (futzing == 0)) {
				l = closest_pc_loc(cur_monst->cur_loc);
				if (((cur_monst->mu > 0) || (cur_monst->cl > 0)) &&
					(dist(cur_monst->cur_loc,l) < 5) && (monst_adjacent(l,i) == false))
					current_monst_tactic = 1; // this means flee
				
				
				if ( (((cur_monst->spec_skill > 0) && (cur_monst->spec_skill < 4))
					  || (cur_monst->spec_skill == 20)) && // Archer?
					(dist(cur_monst->cur_loc,targ_space) < 6) &&
					(monst_adjacent(targ_space,i) == false))
					current_monst_tactic = 1; // this means flee
			}
			
			
			// flee
			if ((univ.town.monst[i].target != 6) && (((cur_monst->morale <= 0)
					&& cur_monst->spec_skill != 13 && cur_monst->m_type != eRace::UNDEAD)
													 || (current_monst_tactic == 1))) {
				if (cur_monst->morale < 0)
					cur_monst->morale++;
				if (cur_monst->health > 50)
					cur_monst->morale++;
				r1 = get_ran(1,1,6);
				if (r1 == 3)
					cur_monst->morale++;
				short targ = univ.town.monst[i].target;
				if(targ < 6 && univ.party[targ].main_status == eMainStatus::ALIVE && cur_monst->mobility == 1) {
					acted_yet = flee_party (i,cur_monst->cur_loc,targ_space);
					if (acted_yet == true) take_m_ap(1,cur_monst);
				}
			}
			if ((target != 6) && (cur_monst->attitude > 0)
				&& (monst_can_see(i,targ_space) == true)
				&& (can_see_monst(targ_space,i) == true)) { // Begin spec. attacks
				
//				sprintf((char *)create_line,"%d: %d  %d  %d",i,cur_monst->breath,cur_monst->mu,cur_monst->cl);
//				add_string_to_buf((char *)create_line);
				
				// Breathe (fire)
				if ( (cur_monst->breath > 0)
					&& (get_ran(1,1,8) < 4) && (acted_yet == false)) {
					//print_nums(cur_monst->breath,cur_monst->breath_type,dist(cur_monst->m_loc,targ_space) );
					if ((target != 6)
						&& (dist(cur_monst->cur_loc,targ_space) <= 8)) {
						acted_yet = monst_breathe(cur_monst,targ_space,cur_monst->breath_type);
						had_monst = true;
						acted_yet = true;
						take_m_ap(4,cur_monst);
					}
				}
				// Mage spell
				if ((cur_monst->mu > 0) && (get_ran(1,1,10) < ((cur_monst->cl > 0) ? 6 : 9) )
					&& (acted_yet == false)) {
					if (((monst_adjacent(targ_space,i) == false) || (get_ran(1,0,2) < 2) || (cur_monst->number >= 160)
						 || (cur_monst->level > 9))
						&& (dist(cur_monst->cur_loc,targ_space) <= 10)) {
						acted_yet = monst_cast_mage(cur_monst,target);
						had_monst = true;
						acted_yet = true;
						take_m_ap(5,cur_monst);
					}
				}
				// Priest spell
				if ((cur_monst->cl > 0) && (get_ran(1,1,8) < 7) && (acted_yet == false)) {
					if (((monst_adjacent(targ_space,i) == false) || (get_ran(1,0,2) < 2) || (cur_monst->level > 9))
						&& (dist(cur_monst->cur_loc,targ_space) <= 10)) {
						acted_yet = monst_cast_priest(cur_monst,target);
						had_monst = true;
						acted_yet = true;
						take_m_ap(4,cur_monst);
					}
				}
				
				// Missile
				if ((abil_range[cur_monst->spec_skill] > 0) // breathing gas short range
					&& (get_ran(1,1,8) < abil_odds[cur_monst->spec_skill]) && (acted_yet == false)) {
					// Don't fire when adjacent, unless non-gaze magical attack
					if (((monst_adjacent(targ_space,i) == false) ||
						 ((cur_monst->spec_skill > 7) && (cur_monst->spec_skill != 20)
						  && (cur_monst->spec_skill != 33)))
						// missile range
						&& (dist(cur_monst->cur_loc,targ_space) <= abil_range[cur_monst->spec_skill])) {
						print_monst_name(cur_monst->number);
						monst_fire_missile(i/*,cur_monst->skill*/,cur_monst->status[eStatus::BLESS_CURSE],
										   cur_monst->spec_skill,cur_monst->cur_loc,target);
						
						// Vapors don't count as action
						if ((cur_monst->spec_skill == 11) || (cur_monst->spec_skill == 7) ||
							(cur_monst->spec_skill == 20))
							take_m_ap(2,cur_monst);
						else if (cur_monst->spec_skill == 10)
							take_m_ap(1,cur_monst);
						else take_m_ap(3,cur_monst);
						had_monst = true;
						acted_yet = true;
					}
				}
			} // Special attacks
			
			// Attack pc
			if(univ.town.monst[i].target < 6 && univ.party[univ.town.monst[i].target].main_status == eMainStatus::ALIVE
				&& (monst_adjacent(targ_space,i) == true)  && (cur_monst->attitude % 2 == 1)
				&& (acted_yet == false)) {
				monster_attack_pc(i,univ.town.monst[i].target);
				take_m_ap(4,cur_monst);
				acted_yet = true;
				had_monst = true;
			}
			// Attack monst
			if ((univ.town.monst[i].target >= 100) && (univ.town.monst[univ.town.monst[i].target - 100].active > 0)
				&& (monst_adjacent(targ_space,i) == true)  && (cur_monst->attitude > 0)
				&& (acted_yet == false)) {
				monster_attack_monster(i,univ.town.monst[i].target - 100);
				take_m_ap(4,cur_monst);
				acted_yet = true;
				had_monst = true;
			}
			
			if (acted_yet == true) {
				print_buf();
				if (j == 0)
					pause(8);
				flushingInput = true;
			}
			
			if (overall_mode == MODE_COMBAT) {
				if ((acted_yet == false) && (cur_monst->mobility == 1)) {  // move monst
					move_target = (univ.town.monst[i].target != 6) ? univ.town.monst[i].target : closest_pc(cur_monst->cur_loc);
					if (monst_hate_spot(i,&move_targ) == true) // First, maybe move out of dangerous space
						seek_party (i,cur_monst->cur_loc,move_targ);
					else { // spot is OK, so go nuts
						if ((cur_monst->attitude % 2 == 1) && (move_target < 6)) // Monsters seeking party do so
							if(univ.party[move_target].main_status == eMainStatus::ALIVE) {
								seek_party (i,cur_monst->cur_loc,univ.party[move_target].combat_pos);
								for (k = 0; k < 6; k++)
									if(univ.party[k].parry > 99 && monst_adjacent(univ.party[k].combat_pos,i)
										&& (cur_monst->active > 0)) {
										univ.party[k].parry = 0;
										pc_attack(k,i);
									}
							}
						
						if (move_target >= 100) // Monsters seeking monsters do so
							if (univ.town.monst[move_target - 100].active > 0) {
								seek_party (i,cur_monst->cur_loc,univ.town.monst[move_target - 100].cur_loc);
								for (k = 0; k < 6; k++)
									if(univ.party[k].parry > 99 && monst_adjacent(univ.party[k].combat_pos,i)
										&& (cur_monst->active > 0) && (cur_monst->attitude % 2 == 1)) {
										univ.party[k].parry = 0;
										pc_attack(k,i);
									}
							}
						
						if (cur_monst->attitude == 0) {
							acted_yet = rand_move (i);
							futzing++;
						}
					}
					take_m_ap(1,cur_monst);
				}
				if ((acted_yet == false) && (cur_monst->mobility == 0)) { // drain action points
					take_m_ap(1,cur_monst);
					futzing++;
				}
			}
			else if (acted_yet == false) {
				take_m_ap(1,cur_monst);
				futzing++;
			}
			
			// pcs attack any fleeing monsters
			if ((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING))
				for (k = 0; k < 6; k++)
					if(univ.party[k].main_status == eMainStatus::ALIVE && !monst_adjacent(univ.party[k].combat_pos,i)
						&& (pc_adj[k] == true) && (cur_monst->attitude % 2 == 1) && (cur_monst->active > 0) &&
						univ.party[k].status[eStatus::INVISIBLE] == 0) {
						combat_posing_monster = current_working_monster = k;
						pc_attack(k,i);
						combat_posing_monster = current_working_monster = 100 + i;
						pc_adj[k] = false;
					}
			
			// Place fields for monsters that create them. Only done when monst sees foe
			if ((target != 6) && (can_see_light(cur_monst->cur_loc,targ_space,sight_obscurity) < 5)) { ////
				if ((cur_monst->radiate_1 == 1) && (get_ran(1,1,100) < cur_monst->radiate_2))
					place_spell_pattern(square,cur_monst->cur_loc,WALL_FIRE,7);
				if ((cur_monst->radiate_1 == 2) && (get_ran(1,1,100) < cur_monst->radiate_2))
					place_spell_pattern(square,cur_monst->cur_loc,WALL_ICE,7);
				if ((cur_monst->radiate_1 == 3) && (get_ran(1,1,100) < cur_monst->radiate_2))
					place_spell_pattern(square,cur_monst->cur_loc,WALL_FORCE,7);
				if ((cur_monst->radiate_1 == 4) && (get_ran(1,1,100) < cur_monst->radiate_2))
					place_spell_pattern(square,cur_monst->cur_loc,FIELD_ANTIMAGIC,7);
				if ((cur_monst->radiate_1 == 5) && (get_ran(1,1,100) < cur_monst->radiate_2))
					place_spell_pattern(square,cur_monst->cur_loc,CLOUD_SLEEP,7);
				if ((cur_monst->radiate_1 == 6) && (get_ran(1,1,100) < cur_monst->radiate_2))
					place_spell_pattern(square,cur_monst->cur_loc,CLOUD_STINK,7);
				if ((cur_monst->radiate_1 == 10) && (get_ran(1,1,100) < 5)){
					if (summon_monster(cur_monst->radiate_2,
									   cur_monst->cur_loc,130,cur_monst->attitude) == true)
					{monst_spell_note(cur_monst->number,33); play_sound(61);}
				}
				if ((cur_monst->radiate_1 == 11) && (get_ran(1,1,100) < 20)){
					if (summon_monster(cur_monst->radiate_2,
									   cur_monst->cur_loc,130,cur_monst->attitude) == true)
					{monst_spell_note(cur_monst->number,33); play_sound(61);}
				}
				if ((cur_monst->radiate_1 == 12) && (get_ran(1,1,100) < 50)){
					if (summon_monster(cur_monst->radiate_2,
									   cur_monst->cur_loc,130,cur_monst->attitude) == true)
					{monst_spell_note(cur_monst->number,33); play_sound(61);}
				}
				if ((cur_monst->radiate_1 == 14) && !special_called && party_can_see_monst(i))	{
					short s1, s2, s3;
					special_called = true;
					take_m_ap(1,cur_monst);
					run_special(eSpecCtx::MONST_SPEC_ABIL,0,cur_monst->radiate_2,cur_monst->cur_loc,&s1,&s2,&s3);		
				}
			}
			
			combat_posing_monster = current_working_monster = -1;
			// Redraw monster after it goes
			if ((cur_monst->attitude > 0) && (cur_monst->active > 0) && (cur_monst->ap == 0)
				&& (is_combat()) && (cur_monst->picture_num > 0) && (party_can_see_monst(i)  == true)) {
				center = cur_monst->cur_loc;
				draw_terrain(0);
				
			}
			
			// If monster dead, take away actions
			if (cur_monst->active == 0)
				cur_monst->ap = 0;
			
			//if ((futzing == 1) && (get_ran(1,0,1) == 0)) // If monster's just pissing around, give up
			//	cur_monst->ap = 0;
			if (futzing > 1) // If monster's just pissing around, give up
				cur_monst->ap = 0;
		}  // End of monster action loop
		
		
	}
	
	for (i = 0; i < num_monst; i++) {  // Begin monster time stuff loop
		// If party dead, no point
		if (party_toast() == true)
			return;
		
		cur_monst = &univ.town.monst[i];
		
		if ((cur_monst->active < 0) || (cur_monst->active > 2))
			cur_monst->active = 0; // clean up
		if (cur_monst->active != 0) { // Take care of monster effects
			if(cur_monst->status[eStatus::ACID] > 0) {  // Acid
				if (printed_acid == false) {
					add_string_to_buf("Acid:              ");
					printed_acid = true;
				}
				r1 = get_ran(cur_monst->status[eStatus::ACID],1,6);
				damage_monst(i, 6,r1, 0, DAMAGE_MAGIC,0);
				cur_monst->status[eStatus::ACID]--;
			}
			
			if (cur_monst->status[eStatus::ASLEEP] == 1)
				monst_spell_note(cur_monst->number,29);
			move_to_zero(cur_monst->status[eStatus::ASLEEP]);
			move_to_zero(cur_monst->status[eStatus::PARALYZED]);
			
			if (univ.party.age % 2 == 0) {
				move_to_zero(cur_monst->status[eStatus::BLESS_CURSE]);
				move_to_zero(cur_monst->status[eStatus::HASTE_SLOW]);
				move_to_zero(cur_monst->status[eStatus::WEBS]);
				
				if(cur_monst->status[eStatus::POISON] > 0) {  // Poison
					if (printed_poison == false) {
						add_string_to_buf("Poisoned monsters:              ");
						printed_poison = true;
					}
					r1 = get_ran(cur_monst->status[eStatus::POISON],1,6);
					damage_monst(i, 6, r1, 0, DAMAGE_POISON,0);
					cur_monst->status[eStatus::POISON]--;
				}
				if(cur_monst->status[eStatus::DISEASE] > 0) {  // Disease
					if (printed_disease == false) {
						add_string_to_buf("Diseased monsters:              ");
						printed_disease = true;
					}
					k = get_ran(1,1,5);
					switch (k) {
						case 1: case 2: poison_monst(cur_monst, 2);break;
						case 3:	slow_monst(cur_monst,2); break;
						case 4: curse_monst(cur_monst,2); break;
						case 5: scare_monst(cur_monst,10); break;
					}
					if (get_ran(1,1,6) < 4)
						cur_monst->status[eStatus::DISEASE]--;
				}
				
			}
			
			if (univ.party.age % 4 == 0) {
				if (cur_monst->mp < cur_monst->max_mp)
					cur_monst->mp += 2;
				move_to_zero(cur_monst->status[eStatus::DUMB]);
			}
		} // end take care of monsters
	}
	
	// If in town, need to restore center
	if (overall_mode < MODE_COMBAT)
		center = univ.town.p_loc;
	if (had_monst == true)
		put_pc_screen();
	for (i = 0; i < 6; i++)
		univ.party[i].parry = 0;
	
	monsters_going = false;
}

void monster_attack_pc(short who_att,short target)
{
	cCreature *attacker;
	short r1,r2,i,store_hp,sound_type = 0;
	eDamageType dam_type = DAMAGE_WEAPON;
	
	
	attacker = &univ.town.monst[who_att];
	
	// A peaceful monster won't attack
	if (attacker->attitude % 2 != 1)
		return;
	
	// Draw attacker frames
	if ((is_combat())
		&& ((center_on_monst == true) || (monsters_going == false))) {
		if (attacker->spec_skill != 11)
			frame_space(attacker->cur_loc,0,attacker->x_width,attacker->y_width);
		frame_space(univ.party[target].combat_pos,1,1,1);
	}
	
	
	
	if ((attacker->a[0] != 0) || (attacker->a[2] != 0))
		print_monst_attacks(attacker->number,target);
	
	// Check sanctuary
	if(univ.party[target].status[eStatus::INVISIBLE] > 0) {
		r1 = get_ran(1,1,100);
		if (r1 > hit_chance[attacker->level / 2]) {
			add_string_to_buf("  Can't find target!                 ");
		}
		return;
	}
	
	for (i = 0; i < 3; i++) {
		if(attacker->a[i] > 0 && univ.party[target].main_status == eMainStatus::ALIVE) {
//			sprintf ((char *) create_line, "  Attacks %s.",(char *) univ.party[target].name);
//			add_string_to_buf((char *) create_line);
			
			// Attack roll
			r1 = get_ran(1,1,100) - 5 * min(8,attacker->status[eStatus::BLESS_CURSE]) + 5 * univ.party[target].status[eStatus::BLESS_CURSE]
				+ 5 * stat_adj(target,eSkill::DEXTERITY) - 15;
			r1 += 5 * (attacker->status[eStatus::WEBS] / 3);
			if(univ.party[target].parry < 100)
				r1 += 5 * univ.party[target].parry;
			
			// Damage roll
			r2 = get_ran(attacker->a[i] / 100 + 1,1,attacker->a[i] % 100)
				+ min(8,attacker->status[eStatus::BLESS_CURSE]) - univ.party[target].status[eStatus::BLESS_CURSE] + 1;
			if (univ.difficulty_adjust() > 2)
				r2 = r2 * 2;
			if (univ.difficulty_adjust() == 2)
				r2 = (r2 * 3) / 2;
			
			if ((univ.party[target].status[eStatus::ASLEEP] > 0) || (univ.party[target].status[eStatus::PARALYZED] > 0)) {
				r1 -= 80;
				r2 = r2 * 2;
			}
			
			draw_terrain(2);
			// Check if hit, and do effects
			if (r1 <= hit_chance[(attacker->skill + 4) / 2]) {
				if(attacker->m_type == eRace::UNDEAD)
					dam_type = DAMAGE_UNDEAD;
				if(attacker->m_type == eRace::DEMON)
					dam_type = DAMAGE_DEMON;
				
				store_hp = univ.party[target].cur_health;
				sound_type = get_monst_sound(attacker,i);
				dam_type += DAMAGE_MARKED;
				if (damage_pc(target,r2,dam_type,
							  attacker->m_type,sound_type) &&
					(store_hp - univ.party[target].cur_health > 0)) {
					damaged_message(store_hp - univ.party[target].cur_health,
									attacker->a[i].type);
					
					if(univ.party[target].status[eStatus::MARTYRS_SHIELD] > 0) {
						add_string_to_buf("  Shares damage!                 ");
						damage_monst(who_att, 6, store_hp - univ.party[target].cur_health, 0, DAMAGE_MAGIC,0);
					}
					
					if ((attacker->poison > 0) && (i == 0)) {
						poison_pc(target,attacker->poison);
					}
					
					// Gremlin
					if ((attacker->spec_skill == 21) && (get_ran(1,0,2) < 2)) {
						add_string_to_buf("  Steals food!                 ");
						print_buf();
						play_sound(26);
						univ.party.food = (long) max(0, (short) (univ.party.food) - get_ran(1,0,10) - 10);
						put_pc_screen();
					}
					
					// Disease
					if (((attacker->spec_skill == 25))
						&& (get_ran(1,0,2) < 2)) {
						add_string_to_buf("  Causes disease!                 ");
						print_buf();
						disease_pc(target,6);
					}
					
					// Petrification touch
					if ((attacker->spec_skill == 30)
						&& (pc_has_abil_equip(target,eItemAbil::PROTECT_FROM_PETRIFY) == 24)
						&& (get_ran(1,0,20) + univ.party[target].level / 4 + univ.party[target].status[eStatus::BLESS_CURSE]) <= 14)
					{
						add_string_to_buf("  Petrifying touch!");
						print_buf();
						kill_pc(target,eMainStatus::STONE); // petrified, duh!
					}
#if 0				// TODO: This is *i's version of the petrification touch ability.
					// It seems better in some ways, like printing a message when you resist,
					// but its calculation is very different, so I'm not sure what to with it.
					// Note, his version has also been incorporated into monster_attack_monster.
					
					// Petrify target
					if (attacker->spec_skill == 30)	{
						add_string_to_buf("  Petrification touch!                ");
						r1 = max(0,(get_ran(1,0,100) - univ.party[target].level + 0.5*attacker->level));
						// Equip petrify protection?
						if (pc_has_abil_equip(target,eItemAbil:PROTECT_FROM_PETRIFY) < 24)
							r1 = 0;
						// Check if petrified.
						if (r1 > 60) {
							kill_pc(target,eMainStatus::STONE);
							add_string_to_buf("    Turned to stone! ");
							play_sound(43);
						}	
						else {
							add_string_to_buf("    Resists! ");							
						}
					}
#endif
					
					// Undead xp drain
					if (((attacker->spec_skill == 16) || (attacker->spec_skill == 17))
						&& (pc_has_abil_equip(target,eItemAbil::LIFE_SAVING) == 24)) {
						// TODO: Uh, wait a second. These two abilities have an identical effect? That doesn't seem right!
						add_string_to_buf("  Drains life!                 ");
						drain_pc(target,(attacker->level * 3) / 2);
						put_pc_screen();
					}
					
					// Undead slow
					if ((attacker->spec_skill == 18) && (get_ran(1,0,8) < 6) && (pc_has_abil_equip(target,eItemAbil::LIFE_SAVING) == 24)) {
						add_string_to_buf("  Stuns! ");
						slow_pc(target,2);
						put_pc_screen();
					}
					// Dumbfound target
					if (attacker->spec_skill == 24) {
						add_string_to_buf("  Dumbfounds! ");
						dumbfound_pc(target,2);
						put_pc_screen();
					}
					
					// Web target
					if (attacker->spec_skill == 27) {
						add_string_to_buf("  Webs!                    ");
						web_pc(target,5);
						put_pc_screen();
					}
					// Sleep target
					if (attacker->spec_skill == 28) {
						add_string_to_buf("  Sleeps!                    ");
						sleep_pc(target,6,eStatus::ASLEEP,-15);
						put_pc_screen();
					}
					// Paralyze target
					if (attacker->spec_skill == 29) {
						add_string_to_buf("  Paralysis touch!                    ");
						sleep_pc(target,500,eStatus::PARALYZED,-5);
						put_pc_screen();
					}
					// Acid touch
					if (attacker->spec_skill == 31)  {
						add_string_to_buf("  Acid touch!      ");
						acid_pc(target,(attacker->level > 20) ? 4 : 2);
					}
					
					// Freezing touch
					if (((attacker->spec_skill == 15) || (attacker->spec_skill == 17))
						&& (get_ran(1,0,8) < 6) && (pc_has_abil_equip(target,eItemAbil::LIFE_SAVING) == 24)) {
						add_string_to_buf("  Freezing touch!");
						r1 = get_ran(3,1,10);
						damage_pc(target,r1,DAMAGE_COLD,eRace::UNKNOWN,0);
					}
					// Killing touch
					if (attacker->spec_skill == 35)
					{
						add_string_to_buf("  Killing touch!");
						r1 = get_ran(20,1,10);
						damage_pc(target,r1,DAMAGE_UNBLOCKABLE,eRace::UNKNOWN,0);
					}
				}
			}
			else {
				sprintf ((char *) create_line, "  Misses.");
				add_string_to_buf((char *) create_line);
				play_sound(2);
			}
			combat_posing_monster = -1;
			draw_terrain(2);
			combat_posing_monster = 100 + who_att;
			
			
		}
		if(univ.party[target].main_status != eMainStatus::ALIVE)
			i = 3;
	}
	
}

void monster_attack_monster(short who_att,short attackee)
{
	cCreature *attacker,*target;
	short r1,r2,i,store_hp,sound_type = 0;
	eDamageType dam_type = DAMAGE_WEAPON;
	
	attacker = &univ.town.monst[who_att];
	target = &univ.town.monst[attackee];
	
	// Draw attacker frames
	if ((is_combat())
		&& ((center_on_monst == true) || (monsters_going == false))) {
		if (attacker->spec_skill != 11)
			frame_space(attacker->cur_loc,0,attacker->x_width,attacker->y_width);
		frame_space(target->cur_loc,1,1,1);
	}
	
	
	if ((attacker->a[1] != 0) || (attacker->a[0] != 0))
		print_monst_attacks(attacker->number,100 + attackee);
	for (i = 0; i < 3; i++) {
		if ((attacker->a[i] > 0) && (target->active != 0)) {
//			sprintf ((char *) create_line, "  Attacks %s.",(char *) univ.party[target].name);
//			add_string_to_buf((char *) create_line);
			
			// if friendly to party, make able to attack
			if (target->attitude == 0)
				target->attitude = 2;
			
			// Attack roll
			r1 = get_ran(1,1,100) - 5 * min(10,attacker->status[eStatus::BLESS_CURSE])
				+ 5 * target->status[eStatus::BLESS_CURSE] - 15;
			r1 += 5 * (attacker->status[eStatus::WEBS] / 3);
			
			// Damage roll
			r2 = get_ran(attacker->a[i] / 100 + 1,1,attacker->a[i] % 100)
				+ min(10,attacker->status[eStatus::BLESS_CURSE]) - target->status[eStatus::BLESS_CURSE] + 2;
			
			if(target->status[eStatus::ASLEEP] > 0 || target->status[eStatus::PARALYZED] > 0) {
				r1 -= 80;
				r2 = r2 * 2;
			}
			
			draw_terrain(2);
			// Check if hit, and do effects
			if (r1 <= hit_chance[(attacker->skill + 4) / 2]) {
				if(attacker->m_type == eRace::DEMON)
					dam_type = DAMAGE_DEMON;
				if(attacker->m_type == eRace::UNDEAD)
					dam_type = DAMAGE_UNDEAD;
				store_hp = target->health;
				
				sound_type = get_monst_sound(attacker,i);
				dam_type += DAMAGE_MARKED;
				if (damage_monst(attackee,7,r2,0,dam_type,sound_type) == true) {
					damaged_message(store_hp - target->health,
									attacker->a[i].type);
					
					if ((attacker->poison > 0) && (i == 0)) {
						poison_monst(target,attacker->poison);
					}
					
					// Undead slow
					if ((attacker->spec_skill == 18) && (get_ran(1,0,8) < 6)) {
						add_string_to_buf("  Stuns!      ");
						slow_monst(target,2);
					}
					
					// Web target
					if (attacker->spec_skill == 27)  {
						add_string_to_buf("  Webs!      ");
						web_monst(target,4);
					}
					// Sleep target
					if (attacker->spec_skill == 28)  {
						add_string_to_buf("  Sleeps!      ");
						charm_monst(target,-15,eStatus::ASLEEP,6);
					}
					// Dumbfound target
					if (attacker->spec_skill == 24)  {
						add_string_to_buf("  Dumbfounds!      ");
						dumbfound_monst(target,2);
					}
					// Disease target
					if (((attacker->spec_skill == 25))
						&& (get_ran(1,0,2) < 2)) {
						add_string_to_buf("  Causes disease!                 ");
						print_buf();
						disease_monst(target,6);
					}
					// Paralyze target
					if (attacker->spec_skill == 29)  {
						add_string_to_buf("  Paralysis touch!      ");
						charm_monst(target,-5,eStatus::PARALYZED,500);
					}
					// Petrify target
					if (attacker->spec_skill == 30)	{
						add_string_to_buf("  Petrification touch!                ");
						r1 = max(0,(get_ran(1,0,100) - target->level + 0.5*attacker->level));
						// Check if petrified.
						if ((r1 < 60) || (target->immunities & 2)) {								
							add_string_to_buf("    Resists! ");
						}	
						else {
							kill_monst(target,7);
							add_string_to_buf("    Turned to stone! ");				
							play_sound(43);
							
						}
					}							
					
					// Acid touch
					if (attacker->spec_skill == 31)  {
						add_string_to_buf("  Acid touch!      ");
						acid_monst(target,3);
					}
					
					// Freezing touch
					if (((attacker->spec_skill == 15) || (attacker->spec_skill == 17))
						&& (get_ran(1,0,8) < 6)) {
						add_string_to_buf("  Freezing touch!");
						r1 = get_ran(3,1,10);
						damage_monst(attackee,7,r1,0,DAMAGE_COLD,0);
					}
					
					// Death touch
					if ((attacker->spec_skill == 35)
						&& (get_ran(1,0,8) < 6)) {
						add_string_to_buf("  Killing touch!");
						r1 = get_ran(20,1,10);
						damage_monst(attackee,7,r1,0,DAMAGE_UNBLOCKABLE,0);
					}
				}
			}
			else {
				sprintf ((char *) create_line, "  Misses.");
				add_string_to_buf((char *) create_line);
				play_sound(2);
			}
			combat_posing_monster = -1;
			draw_terrain(2);
			combat_posing_monster = 100 + who_att;
		}
		if (target->active == 0)
			i = 3;
	}
	
}


void monst_fire_missile(short m_num,short bless,short level,location source,short target)
//short target; // 100 +  - monster is target
{
	cCreature *m_target;
	short r1,r2,dam[40] = {
		0,1,2,3,4, 6,8,7,0,0, 0,0,0,0,0, 0,0,0,0,0,
		8,0,0,0,0, 0,0,0,0,0, 0,0,0,0,6, 0,0,0,0,0},i,j;
	location targ_space;
	
	if (target == 6)
		return;
	if (target >= 100) {
		targ_space = univ.town.monst[target - 100].cur_loc;
		if (univ.town.monst[target - 100].active == 0)
			return;
	}
	else {
		targ_space = (is_combat()) ? univ.party[target].combat_pos : univ.town.p_loc;
		if(univ.party[target].main_status != eMainStatus::ALIVE)
			return;
	}
	
	if (target >= 100)
		m_target = &univ.town.monst[target - 100];
	if (((overall_mode >= MODE_COMBAT) && (overall_mode <= MODE_TALKING)) && (center_on_monst == true)) {
		frame_space(source,0,univ.town.monst[m_num].x_width,univ.town.monst[m_num].y_width);
		if (target >= 100)
			frame_space(targ_space,1,m_target->x_width,m_target->y_width);
		else frame_space(targ_space,1,1,1);
	}
	
	draw_terrain(2);
	if (level == 32) { // sleep cloud
		ASB("Creature breathes.");
		run_a_missile(source,targ_space,0,0,44,
					  0,0,100);
		place_spell_pattern(rad2,targ_space,CLOUD_SLEEP,7);
	}
	else if (level == 14) { // vapors
		//play_sound(44);
		if (target < 100) { // on PC
			sprintf (create_line, "  Breathes on %s.                  ",univ.party[target].name.c_str());
			add_string_to_buf(create_line);
		}
		else {  // on monst
			add_string_to_buf("  Breathes vapors.");
		}
		run_a_missile(source,targ_space,12,0,44,
					  0,0,100);
		scloud_space(targ_space.x,targ_space.y);
	}
	else if (level == 19) { // webs
		//play_sound(14);
		if (target < 100) { // on PC
			sprintf (create_line, "  Throws web at %s.                  ",univ.party[target].name.c_str());
			add_string_to_buf(create_line);
		}
		else {  // on monst
			add_string_to_buf("  Throws web.");
		}
		run_a_missile(source,targ_space,8,0,14,
					  0,0,100);
		web_space(targ_space.x,targ_space.y);
	}
	else if (level == 23) { // paral
		play_sound(51);
		if (target < 100) { // on PC
			sprintf (create_line, "  Fires ray at %s.                  ",univ.party[target].name.c_str());
			add_string_to_buf((char *) create_line);
			sleep_pc(target,100,eStatus::PARALYZED,0);
		}
		else {  // on monst
			add_string_to_buf("  Shoots a ray.");
			charm_monst(m_target,0,eStatus::PARALYZED,100);
		}
		//run_a_missile(source,targ_space,8,0,14,
		//	0,0,100);
		//web_space(targ_space.x,targ_space.y);
	}
	else if (level == 8) { // petrify
		//play_sound(43);
		run_a_missile(source,targ_space,14,0,43,0,0,100);
		if (target < 100) { // on PC
			sprintf (create_line, "  Gazes at %s.                  ",univ.party[target].name.c_str());
			add_string_to_buf(create_line);
			r1 = get_ran(1,0,20) + univ.party[target].level / 4 + univ.party[target].status[eStatus::BLESS_CURSE];
			if (pc_has_abil_equip(target,eItemAbil::PROTECT_FROM_PETRIFY) < 24)
				r1 = 20;
			if (r1 > 14) {
				sprintf (create_line, "  %s resists.                  ",univ.party[target].name.c_str());
				add_string_to_buf(create_line);
			}
			else {
				sprintf (create_line, "  %s is turned to stone.                  ",univ.party[target].name.c_str());
				add_string_to_buf(create_line);
				kill_pc(target,eMainStatus::STONE);
			}
		}
		else {
			monst_spell_note(m_target->number,9);
			r1 = get_ran(1,0,20) + m_target->level / 4 + m_target->status[eStatus::BLESS_CURSE];
			if ((r1 > 14) || (m_target->immunities & 2))
				monst_spell_note(m_target->number,10);
			else {
				monst_spell_note(m_target->number,8);
				kill_monst(m_target,7);
			}
		}
	}
	else if (level == 9) { /// Drain sp
		if (target < 100) { // pc
			// modify target is target has no sp
			if (univ.party[target].cur_sp < 4) {
				for (i = 0; i < 8; i++) {
					j = get_ran(1,0,5);
					if(univ.party[j].main_status == eMainStatus::ALIVE && univ.party[j].cur_sp > 4 &&
						(can_see_light(source,univ.party[j].combat_pos,sight_obscurity) < 5) && (dist(source,univ.party[j].combat_pos) <= 8)) {
						target = j;
						i = 8;
						targ_space = univ.party[target].combat_pos;
					}
				}
				
			}
			run_a_missile(source,targ_space,8,0,43,0,0,100);
			sprintf (create_line, "  Drains %s.                  ",univ.party[target].name.c_str());
			add_string_to_buf(create_line);
			univ.party[target].cur_sp = univ.party[target].cur_sp / 2;
		}
		else { // on monst
			run_a_missile(source,targ_space,8,0,43,0,0,100);
			monst_spell_note(m_target->number,11);
			if (m_target->mp >= 4)
				m_target->mp = m_target->mp / 2;
			else m_target->skill = 1;
		}
	}
	else if (level == 10) { // heat ray
		run_a_missile(source,targ_space,13,0,51,
					  0,0,100);
		r1 = get_ran(7,1,6);
		start_missile_anim();
		if (target < 100) { // pc
			sprintf (create_line, "  Hits %s with heat ray.",univ.party[target].name.c_str());
			add_string_to_buf(create_line);
			damage_pc(target,r1,DAMAGE_FIRE,eRace::UNKNOWN,0);
		}
		else { // on monst
			add_string_to_buf("  Fires heat ray.");
			damage_monst(target - 100,7,r1,0,DAMAGE_FIRE,0);
		}
		do_explosion_anim(5,0);
		end_missile_anim();
		handle_marked_damage();
	}
	else if (level == 33) { // acid spit
		run_a_missile(source,targ_space,0,1,64,
					  0,0,100);
		//play_sound(64);
		if (target < 100) { // pc
			sprintf (create_line, "  Spits acid on %s.",univ.party[target].name.c_str());
			add_string_to_buf(create_line);
			acid_pc(target,6);
		}
		else { // on monst
			add_string_to_buf("  Spits acid.");
			acid_monst(m_target,6);
		}
	}
	else if (target < 100) {  // missile on PC
		
		switch (level) {
			case 1: case 2: case 20:
				run_a_missile(source,targ_space,3,1,12,0,0,100);
				sprintf (create_line, "  Shoots at %s.",univ.party[target].name.c_str());
				break;
			case 3:
				run_a_missile(source,targ_space,5,1,14,0,0,100);
				sprintf (create_line, "  Throws spear at %s.",univ.party[target].name.c_str());
				break;
			case 7:
				run_a_missile(source,targ_space,7,1,14,0,0,100);
				sprintf (create_line, "  Throws razordisk at %s.",univ.party[target].name.c_str());
				break;
			case 34:
				run_a_missile(source,targ_space,5,1,14,0,0,100);
				sprintf (create_line, "  Fires spines at %s.",univ.party[target].name.c_str());
				break;
			default:
				run_a_missile(source,targ_space,12,1,14,0,0,100);
				sprintf (create_line, "  Throws rock at %s.",univ.party[target].name.c_str());
				break;
		}
		
		add_string_to_buf(create_line);
		
		// Check sanctuary
		if(univ.party[target].status[eStatus::INVISIBLE] > 0) {
			r1 = get_ran(1,1,100);
			if (r1 > hit_chance[level]) {
				add_string_to_buf("  Can't find target!                 ");
			}
			return;
		}
		
		r1 = get_ran(1,1,100) - 5 * min(10,bless) + 5 * univ.party[target].status[eStatus::BLESS_CURSE]
			- 5 * (can_see_light(source, univ.party[target].combat_pos,sight_obscurity));
		if(univ.party[target].parry < 100)
			r1 += 5 * univ.party[target].parry;
		r2 = get_ran(dam[level],1,7) + min(10,bless);
		
		if (r1 <= hit_chance[dam[level] * 2]) {
//			sprintf ((char *) create_line, "  Hits %s.",(char *) univ.party[target].name);
//			add_string_to_buf((char *) create_line);
			
			if(damage_pc(target,r2,DAMAGE_WEAPON,eRace::UNKNOWN,13)) {
				// TODO: Uh, is something supposed to happen here!?
			}
		}
		else {
			sprintf (create_line, "  Misses %s.",univ.party[target].name.c_str());
			add_string_to_buf(create_line);
		}
		
	}
	else { // missile on monst
		switch (level) {
			case 1: case 2: case 20:
				run_a_missile(source,targ_space,3,1,12,0,0,100);
				monst_spell_note(m_target->number,12);
				break;
			case 3:
				run_a_missile(source,targ_space,5,1,14,0,0,100);
				monst_spell_note(m_target->number,13);
				break;
			case 7:
				run_a_missile(source,targ_space,7,1,14,0,0,100);
				monst_spell_note(m_target->number,15);
				break;
			case 34:
				run_a_missile(source,targ_space,5,1,14,0,0,100);
				monst_spell_note(m_target->number,32);
				break;
			default:
				run_a_missile(source,targ_space,12,1,14,0,0,100);
				monst_spell_note(m_target->number,14);
				break;
		}
		r1 = get_ran(1,1,100) - 5 * min(10,bless) + 5 * m_target->status[eStatus::BLESS_CURSE]
			- 5 * (can_see_light(source, m_target->cur_loc,sight_obscurity));
		r2 = get_ran(dam[level],1,7) + min(10,bless);
		
		if (r1 <= hit_chance[dam[level] * 2]) {
//			monst_spell_note(m_target->number,16);
			
			damage_monst(target - 100,7,r2,0,DAMAGE_WEAPON,13);
		}
		else {
			monst_spell_note(m_target->number,18);
		}
	}
}


bool monst_breathe(cCreature *caster,location targ_space,short dam_type)////
//dam_type; // 0 - fire  1 - cold  2 - magic
{
	short level,missile_t[4] = {13,6,8,8};
	eDamageType type[4] = {DAMAGE_FIRE, DAMAGE_COLD, DAMAGE_MAGIC, DAMAGE_UNBLOCKABLE};
	location l;
	
	draw_terrain(2);
	if ((is_combat()) && (center_on_monst == true)) {
		frame_space(caster->cur_loc,0,caster->x_width,caster->y_width);
	}
	//if (dam_type < 2)
	l = caster->cur_loc;
	if ((caster->direction < 4) &&
		(caster->x_width > 1))
		l.x++;
	
	dam_type = caster->breath_type;
	run_a_missile(l,targ_space,missile_t[dam_type],0,44,0,0,100);
	//	play_sound(44);
	//else play_sound(64);
	level = caster->breath;
	//if (level > 10)
	//	play_sound(5);
	
	monst_breathe_note(caster->number);
	level = get_ran(caster->breath,1,8);
	if (overall_mode < MODE_COMBAT)
		level = level / 3;
	start_missile_anim();
	hit_space(targ_space,level,type[dam_type],1,1);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
	
	return true;
}

bool monst_cast_mage(cCreature *caster,short targ)////
{
	short r1,j,spell,i,level,target_levels,friend_levels_near,x;
	bool acted = false;
	location target,vict_loc,ashes_loc,l;
	cCreature *affected;
	short caster_array[7][18] = {
		{1,1,1,2,2, 2,1,3,4,4, 1,1,1,2,2, 2,3,4},
		{5,5,5,6,7, 8,9,10,11,11, 2,2,2,5,7, 10,10,5},
		{5,5,2,9,11, 12,12,12,14,13, 13,12,12,2,2, 2,2,2},
		{15,15,16,17,17, 5,12,12,13,13, 17,17,16,17,16, 2,2,2},
		{15,18,19,19,20, 20,21,21,16,17, 18,18,18,18,19, 19,19,20},
		{23,23,22,22,21, 21,20,24,19,18, 18,18,18,18,18, 23,23,19},
		{23,23,24,25,26, 27,19,22,19,18, 18,18,18,18,26, 24,24,23}};
	short emer_spells[7][4] = {
		{2,0,0,5},
		{2,10,11,7},
		{2,13,12,13},
		{2,13,12,13},
		{18,20,19,18},
		{18,24,19,24},
		{18,26,19,27}};
	
	
	if (univ.town.is_antimagic(caster->cur_loc.x,caster->cur_loc.y)) {
		return false;
	}
	// is target dead?
	if(targ < 6 && univ.party[targ].main_status != eMainStatus::ALIVE)
		return false;
	if ((targ >= 100) && (univ.town.monst[targ - 100].active == 0))
		return false;
	
	level = max(1,caster->mu - caster->status[eStatus::DUMB]) - 1;
	
	target = find_fireball_loc(caster->cur_loc,1,(caster->attitude % 2 == 1) ? 0 : 1,&target_levels);
	friend_levels_near = (caster->attitude % 2 != 1) ? count_levels(caster->cur_loc,3) : -1 * count_levels(caster->cur_loc,3);
	
	if ((caster->health * 4 < caster->m_health) && (get_ran(1,0,10) < 9))
		spell = emer_spells[level][3];
	else if(((caster->status[eStatus::HASTE_SLOW] < 0 && get_ran(1,0,10) < 7) ||
			  (caster->status[eStatus::HASTE_SLOW] == 0 && get_ran(1,0,10) < 5)) && emer_spells[level][0] != 0)
		spell = emer_spells[level][0];
	else if ((friend_levels_near <= -10) && (get_ran(1,0,10) < 7) && (emer_spells[level][1] != 0))
		spell = emer_spells[level][1];
	else if ((target_levels > 50) && (get_ran(1,0,10) < 7) && (emer_spells[level][2] != 0))
		spell = emer_spells[level][2];
	else {
		r1 = get_ran(1,0,17);
		spell = caster_array[level][r1];
	}
	
	// Hastes happen often now, but don't cast them redundantly
	if(caster->status[eStatus::HASTE_SLOW] > 0 && (spell == 2 || spell == 18))
		spell = emer_spells[level][3];
	
	
	// Anything prventing spell?
	if ((target.x > 64) && (monst_mage_area_effect[spell - 1] > 0)) {
		r1 = get_ran(1,0,9);
		spell = caster_array[level][r1];
		if ((target.x > 64) && (monst_mage_area_effect[spell - 1] > 0))
			return false;
	}
	if (monst_mage_area_effect[spell - 1] > 0) {
		targ = 6;
	}
	
	if (targ < 6) {
		vict_loc = (is_combat()) ? univ.party[targ].combat_pos : univ.town.p_loc;
		if (is_town())
			vict_loc = target = univ.town.p_loc;
	}
	if (targ >= 100)
		vict_loc = univ.town.monst[targ - 100].cur_loc;
	if ((targ == 6) && (univ.town.is_antimagic(target.x,target.y)))
		return false;
	
	// check antimagic
	if (is_combat())
		if ((targ < 6) && (univ.town.is_antimagic(univ.party[targ].combat_pos.x,univ.party[targ].combat_pos.y)))
			return false;
	if (is_town())
		if ((targ < 6) && (univ.town.is_antimagic(univ.town.p_loc.x,univ.town.p_loc.y)))
			return false;
	if ((targ >= 100) && (univ.town.is_antimagic(univ.town.monst[targ - 100].cur_loc.x,
												 univ.town.monst[targ - 100].cur_loc.y)))
		return false;
	
	
	// How about shockwave? Good idea?
	if ((spell == 27) && (caster->attitude % 2 != 1))
		spell = 26;
	if ((spell == 27) && (caster->attitude % 2 == 1) && (count_levels(caster->cur_loc,10) < 45))
		spell = 26;
	
//	sprintf((char *)create_line,"m att %d trg %d trg2 x%dy%d spl %d mp %d tl:%d ",caster->attitude,targ,
//		(short)target.x,(short)target.y,spell,caster->mp,target_levels);
//	add_string_to_buf((char *) create_line);
	
	l = caster->cur_loc;
	if ((caster->direction < 4) && (caster->x_width > 1))
		l.x++;
	
	if (caster->mp >= monst_mage_cost[spell - 1]) {
		monst_cast_spell_note(caster->number,spell,0);
		acted = true;
		caster->mp -= monst_mage_cost[spell - 1];
		
		draw_terrain(2);
		switch (spell) {
			case 1: // spark
				run_a_missile(l,vict_loc,6,1,11,0,0,80);
				r1 = get_ran(2,1,4);
				damage_target(targ,r1,DAMAGE_FIRE);
				break;
			case 2: // minor haste
				play_sound(25);
				caster->status[eStatus::HASTE_SLOW] += 2;
				break;
			case 3: // strength
				play_sound(25);
				caster->status[eStatus::BLESS_CURSE] += 3;
				break;
			case 4: // flame cloud
				run_a_missile(l,vict_loc,2,1,11,0,0,80);
				place_spell_pattern(single,vict_loc,WALL_FIRE,7);
				break;
			case 5: // flame
				run_a_missile(l,vict_loc,2,1,11,0,0,80);
				start_missile_anim();
				r1 = get_ran(caster->level,1,4);
				damage_target(targ,r1,DAMAGE_FIRE);
				break;
			case 6: // minor poison
				run_a_missile(l,vict_loc,11,0,25,0,0,80);
				if (targ < 6)
					poison_pc(targ,2 + get_ran(1,0,1));
				else poison_monst(&univ.town.monst[targ - 100],2 + get_ran(1,0,1));
				break;
			case 7: // slow
				run_a_missile(l,vict_loc,15,0,25,0,0,80);
				if (targ < 6)
					slow_pc(targ,2 + caster->level / 2);
				else slow_monst(&univ.town.monst[targ - 100],2 + caster->level / 2);
				break;
			case 8: // dumbfound
				run_a_missile(l,vict_loc,14,0,25,0,0,80);
				if (targ < 6)
					dumbfound_pc(targ,2);
				else dumbfound_monst(&univ.town.monst[targ - 100],2);
				break;
			case 9: // scloud
				run_a_missile(l,target,0,0,25,0,0,80);
				place_spell_pattern(square,target,CLOUD_STINK,7);
				break;
			case 10: // summon beast
				r1 = get_summon_monster(1);
				if (r1 == 0)
					break;
				x = get_ran(3,1,4);
				//Delay(12,&dummy); // gives sound time to end
				play_sound(25);
				play_sound(-61);
				summon_monster(r1,caster->cur_loc,
							   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				break;
			case 11: // conflagration
				run_a_missile(l,target,13,1,25,0,0,80);
				place_spell_pattern(rad2,target,WALL_FIRE,7);
				break;
			case 12: // fireball
				r1 = 1 + (caster->level * 3) / 4;
				if (r1 > 29) r1 = 29;
				run_a_missile(l,target,2,1,11,0,0,80);
				start_missile_anim();
				place_spell_pattern(square,target,DAMAGE_FIRE,r1,7);
				ashes_loc = target;
				break;
			case 13: case 20: case 26:// summon
				play_sound(25);
				if (spell == 13) {
					r1 = get_summon_monster(1);
					if (r1 == 0)
						break;
					j = get_ran(2,1,3) + 1;
				}
				if (spell == 20) {
					r1 = get_summon_monster(2);
					if (r1 == 0)
						break;
					j = get_ran(2,1,2) + 1;
				}
				if (spell == 26) {
					r1 = get_summon_monster(3);
					if (r1 == 0)
						break;
					j = get_ran(1,2,3);
				}
				mainPtr.display(); // TODO: Needed?
				sf::sleep(time_in_ticks(12)); // gives sound time to end
				x = get_ran(4,1,4);
				for (i = 0; i < j; i++){
					play_sound(-61);
					if (summon_monster(r1,caster->cur_loc,
									   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude) == false) {
						add_string_to_buf("  Summon failed."); i = j;}
				}
				break;
			case 14: // web
				play_sound(25);
				place_spell_pattern(rad2,target,FIELD_WEB,7);
				break;
			case 15: // poison
				run_a_missile(l,vict_loc,11,0,25,0,0,80);
				x = get_ran(1,0,3);
				if (targ < 6)
					poison_pc(targ,4 + x);
				else poison_monst(&univ.town.monst[targ - 100],4 + x);
				break;
			case 16: // ice bolt
				run_a_missile(l,vict_loc,6,1,11,0,0,80);
				r1 = get_ran(5 + (caster->level / 5),1,8);
				start_missile_anim();
				damage_target(targ,r1,DAMAGE_COLD);
				break;
			case 17: // slow gp
				play_sound(25);
				if (caster->attitude % 2 == 1)
					for (i = 0; i < 6; i++)
						if (pc_near(i,caster->cur_loc,8))
							slow_pc(i,2 + caster->level / 4);
				for (i = 0; i < univ.town->max_monst(); i++) {
					if ((univ.town.monst[i].active != 0) &&
						(((univ.town.monst[i].attitude % 2 == 1) && (caster->attitude % 2 != 1)) ||
						 ((univ.town.monst[i].attitude % 2 != 1) && (caster->attitude % 2 == 1)) ||
						 ((univ.town.monst[i].attitude % 2 == 1) && (caster->attitude != univ.town.monst[i].attitude)))
						&& (dist(caster->cur_loc,univ.town.monst[i].cur_loc) <= 7))
						slow_monst(&univ.town.monst[i],2 + caster->level / 4);
				}
				break;
			case 18: // major haste
				play_sound(25);
				for (i = 0; i < univ.town->max_monst(); i++)
					if ((monst_near(i,caster->cur_loc,8,0)) &&
						(caster->attitude == univ.town.monst[i].attitude)) {
						affected = &univ.town.monst[i];
						affected->status[eStatus::HASTE_SLOW] += 3;
					}
				play_sound(4);
				break;
			case 19: // firestorm
				run_a_missile(l,target,2,1,11,0,0,80);
				r1 = 1 + (caster->level * 3) / 4 + 3;
				if (r1 > 29) r1 = 29;
				start_missile_anim();
				place_spell_pattern(rad2,target,DAMAGE_FIRE,r1,7);
				ashes_loc = target;
				break;
				
				
				
				
			case 21: // shockstorm
				run_a_missile(l,target,6,1,11,0,0,80);
				place_spell_pattern(rad2,target,WALL_FORCE,7);
				break;
			case 22: // m. poison
				run_a_missile(l,vict_loc,11,1,11,0,0,80);
				x = get_ran(1,1,2);
				if (targ < 6)
					poison_pc(targ,6 + x);
				else poison_monst(&univ.town.monst[targ - 100],6 + x);
				break;
			case 23: // kill!!!
				run_a_missile(l,vict_loc,9,1,11,0,0,80);
				r1 = 35 + get_ran(3,1,10);
				start_missile_anim();
				damage_target(targ,r1,DAMAGE_MAGIC);
				break;
			case 24: // daemon
				x = get_ran(3,1,4);
				play_sound(25);
				play_sound(-61);
				mainPtr.display(); // TODO: Needed?
				sf::sleep(time_in_ticks(12)); // gives sound time to end
				summon_monster(85,caster->cur_loc,
							   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				break;
			case 25: // major bless
				play_sound(25);
				for (i = 0; i < univ.town->max_monst(); i++)
					if ((monst_near(i,caster->cur_loc,8,0)) &&
						(caster->attitude == univ.town.monst[i].attitude)) {
						affected = &univ.town.monst[i];
						affected->health += get_ran(2,1,10);
						r1 = get_ran(3,1,4);
						affected->status[eStatus::BLESS_CURSE] = min(8,affected->status[eStatus::BLESS_CURSE] + r1);
						affected->status[eStatus::WEBS] = 0;
						if (affected->status[eStatus::HASTE_SLOW] < 0)
							affected->status[eStatus::HASTE_SLOW] = 0;
						affected->morale += get_ran(3,1,10);
					}
				play_sound(4);
				break;
			case 27: // shockwave
				do_shockwave(caster->cur_loc);
				break;
		}
	}
	else caster->mp++;
	
	if (ashes_loc.x > 0)
		univ.town.set_ash(ashes_loc.x,ashes_loc.y,true);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
	
	return acted;
}

bool monst_cast_priest(cCreature *caster,short targ)
{
	short r1,r2,spell,i,x,level,target_levels,friend_levels_near;
	bool acted = false;
	location target,vict_loc,l;
	cCreature *affected;
	short caster_array[7][10] = {
		{1,1,1,1,3,3,3,4,4,4},
		{5,5,6,6,7,7,8,8,8,9},
		{9,6,6,8,11,12,12,5,5,12},
		{12,12,13,13,14,9,9,14,14,15},
		{19,18,13,19,15,18,18,19,16,18},
		{22,18,16,19,18,18,21,22,23,23},
		{26,26,25,24,26,22,24,22,26,25}};
	short emer_spells[7][4] = {
		{0,1,0,2},
		{0,8,0,2},
		{0,8,0,10},
		{0,14,0,10},
		{0,19,18,17},
		{0,19,18,20},
		{25,25,26,24}};
	location ashes_loc;
	
	
	if(targ < 6 && univ.party[targ].main_status != eMainStatus::ALIVE)
		return false;
	if ((targ >= 100) && (univ.town.monst[targ - 100].active == 0))
		return false;
	if (univ.town.is_antimagic(caster->cur_loc.x,caster->cur_loc.y)) {
		return false;
	}
	level = max(1,caster->cl - caster->status[eStatus::DUMB]) - 1;
	
	target = find_fireball_loc(caster->cur_loc,1,(caster->attitude % 2 == 1) ? 0 : 1,&target_levels);
	friend_levels_near = (caster->attitude % 2 != 1) ? count_levels(caster->cur_loc,3) : -1 * count_levels(caster->cur_loc,3);
	
	if ((caster->health * 4 < caster->m_health) && (get_ran(1,0,10) < 9))
		spell = emer_spells[level][3];
	else if(caster->status[eStatus::HASTE_SLOW] < 0 && get_ran(1,0,10) < 7 && emer_spells[level][0] != 0)
		spell = emer_spells[level][0];
	else if ((friend_levels_near <= -10) && (get_ran(1,0,10) < 7) && (emer_spells[level][1] != 0))
		spell = emer_spells[level][1];
	else if ((target_levels > 50 < 0) && (get_ran(1,0,10) < 7) && (emer_spells[level][2] != 0))
		spell = emer_spells[level][2];
	else {
		r1 = get_ran(1,0,9);
		spell = caster_array[level][r1];
	}
	
	
	
	// Anything preventing spell?
	if ((target.x > 64) && (monst_priest_area_effect[spell - 1] > 0))  {
		r1 = get_ran(1,0,9);
		spell = caster_array[level][r1];
		if ((target.x > 64) && (monst_priest_area_effect[spell - 1] > 0))
			return false;
	}
	if (monst_priest_area_effect[spell - 1] > 0)
		targ = 6;
	if (targ < 6)
		vict_loc = (is_town()) ? univ.town.p_loc : univ.party[targ].combat_pos;
	if (targ >= 100)
		vict_loc = univ.town.monst[targ - 100].cur_loc;
	if ((targ == 6) && (univ.town.is_antimagic(target.x,target.y)))
		return false;
	if ((targ < 6) && (univ.town.is_antimagic(univ.party[targ].combat_pos.x,univ.party[targ].combat_pos.y)))
		return false;
	if ((targ >= 100) && (univ.town.is_antimagic(univ.town.monst[targ - 100].cur_loc.x,
												 univ.town.monst[targ - 100].cur_loc.y)))
		return false;
	
	
//	sprintf((char *)create_line,"p att %d trg %d trg2 x%dy%d spl %d mp %d",caster->attitude,targ,
//		(short)target.x,(short)target.y,spell,caster->mp);
//	add_string_to_buf((char *) create_line);
	
	// snuff heals if unwounded
	if ((caster->health == caster->m_health) &&
		((spell == 17) || (spell == 20)))
	 	spell--;
	
	l = caster->cur_loc;
	if ((caster->direction < 4) && (caster->x_width > 1))
		l.x++;
	
	if (caster->mp >= monst_priest_cost[spell - 1]) {
		monst_cast_spell_note(caster->number,spell,1);
		acted = true;
		caster->mp -= monst_priest_cost[spell - 1];
		draw_terrain(2);
		switch (spell) {
			case 3: // wrack
				run_a_missile(l,vict_loc,8,0,24,0,0,80);
				r1 = get_ran(2,1,4);
				start_missile_anim();
				damage_target(targ,r1,DAMAGE_UNBLOCKABLE);
				break;
			case 4: // stumble
				play_sound(24);
				place_spell_pattern(single,vict_loc,FIELD_WEB,7);
				break;
			case 1: case 5: // Blesses
				play_sound(24);
				caster->status[eStatus::BLESS_CURSE] = min(8,caster->status[eStatus::BLESS_CURSE] + ((spell == 1) ? 3 : 5));
				play_sound(4);
				break;
			case 6: // curse
				run_a_missile(l,vict_loc,8,0,24,0,0,80);
				x = get_ran(1,0,1);
				if (targ < 6)
					curse_pc(targ,2 + x);
				else curse_monst(&univ.town.monst[targ - 100],2 + x);
				break;
			case 7: // wound
				run_a_missile(l,vict_loc,8,0,24,0,0,80);
				r1 = get_ran(2,1,6) + 2;
				start_missile_anim();
				damage_target(targ,r1,DAMAGE_MAGIC);
				break;
			case 8: case 22: // summon spirit,summon guardian
				play_sound(24);
				play_sound(-61);
				
				x =  get_ran(3,1,4);
				summon_monster(((spell == 8) ? 125 : 122),caster->cur_loc,
							   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				break;
			case 9: // disease
				run_a_missile(l,vict_loc,11,0,24,0,0,80);
				x = get_ran(1,0,2);
				if (targ < 6)
					disease_pc(targ,2 + x);
				else disease_monst(&univ.town.monst[targ - 100],2 + x);
				break;
			case 11: // holy scourge
				run_a_missile(l,vict_loc,15,0,24,0,0,80);
				if (targ < 6) {
					r1 = get_ran(1,0,2);
					slow_pc(targ,2 + r1);
					r1 = get_ran(1,0,2);
					curse_pc(targ,3 + r1);
				}
				else {
					r1 = get_ran(1,0,2);
					slow_monst(&univ.town.monst[targ - 100],r1);
					r1 = get_ran(1,0,2);
					curse_monst(&univ.town.monst[targ - 100],r1);
				}
				break;
			case 12: // smite
				run_a_missile(l,vict_loc,6,0,24,0,0,80);
				r1 = get_ran(4,1,6) + 2;
				start_missile_anim();
				damage_target(targ,r1,DAMAGE_COLD);
				break;
			case 14: // sticks to snakes
				play_sound(24);
				r1 = get_ran(1,1,4) + 2;
				for (i = 0; i < r1; i++) {
					play_sound(-61);
					r2 = get_ran(1,0,7);
					x = get_ran(3,1,4);
					summon_monster((r2 == 1) ? 100 : 99,caster->cur_loc,
								   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				}
				break;
			case 15: // martyr's shield
				play_sound(24);
				caster->status[eStatus::MARTYRS_SHIELD] = min(10,caster->status[eStatus::MARTYRS_SHIELD] + 5);
				break;
			case 19: // summon host
				play_sound(24);
				x = get_ran(3,1,4) + 1;
				play_sound(-61);
				summon_monster(126,caster->cur_loc,
							   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				for (i = 0; i < 4; i++)	{
					play_sound(-61);
					if (summon_monster(125,caster->cur_loc,
									   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude) == false)
						i = 4;
				}
				break;
				
			case 13: case 23: // holy scourge,curse all,pestilence
				play_sound(24);
				r1 = get_ran(2,0,2);
				r2 = get_ran(1,0,2);
				if (caster->attitude % 2 == 1)
					for (i = 0; i < 6; i++)
						if (pc_near(i,caster->cur_loc,8)) {
							if (spell == 13)
								curse_pc(i,2 + r1);
							if (spell == 23)
								disease_pc(i,2 + r2);
						}
				for (i = 0; i < univ.town->max_monst(); i++) {
					if ((univ.town.monst[i].active != 0) &&
						(((univ.town.monst[i].attitude % 2 == 1) && (caster->attitude % 2 != 1)) ||
						 ((univ.town.monst[i].attitude % 2 != 1) && (caster->attitude % 2 == 1)) ||
						 ((univ.town.monst[i].attitude % 2 == 1) && (caster->attitude != univ.town.monst[i].attitude)))
						&& (dist(caster->cur_loc,univ.town.monst[i].cur_loc) <= 7)) {
						if (spell == 13)
							curse_monst(&univ.town.monst[i],2 + r1);
						if (spell == 23)
							disease_monst(&univ.town.monst[i],2 + r2);
					}
				}
				break;
				
			case 2: case 10: case 17: case 20: // heals
				play_sound(24);
				switch(spell) {
					case 2: r1 = get_ran(2,1,4) + 2; break;
					case 10: r1 = get_ran(3,1,6); break;
					case 17: r1 = get_ran(5,1,6) + 3; break;
					case 20: r1 = 50; break;
				}
				caster->health = min(caster->health + r1, caster->m_health);
				break;
			case 16: case 24:// bless all,revive all
				play_sound(24);
				// TODO: What's r2 for here? Should it be used for Revive All?
				r1 =  get_ran(2,1,4); r2 = get_ran(3,1,6);
				for (i = 0; i < univ.town->max_monst(); i++)
					if ((monst_near(i,caster->cur_loc,8,0)) &&
						(caster->attitude == univ.town.monst[i].attitude)) {
						affected = &univ.town.monst[i];
						if (spell == 16)
							affected->status[eStatus::BLESS_CURSE] = min(8,affected->status[eStatus::BLESS_CURSE] + r1);
						if (spell == 24)
							affected->health += r1;
					}
				play_sound(4);
				break;
			case 18: // Flamestrike
				run_a_missile(l,target,2,0,11,0,0,80);
				r1 = 2 + caster->level / 2 + 2;
				start_missile_anim();
				place_spell_pattern(square,target,DAMAGE_FIRE,r1,7);
				ashes_loc = target;
				break;
				
				
			case 21: // holy ravaging
				run_a_missile(l,vict_loc,14,0,53,0,0,80);
				r1 = get_ran(4,1,8);
				r2 = get_ran(1,0,2);
				damage_target(targ,r1,DAMAGE_MAGIC);
				if (targ < 6) {
					slow_pc(targ,6);
					poison_pc(targ,5 + r2);
				}
				else {
					slow_monst(&univ.town.monst[targ - 100],6);
					poison_monst(&univ.town.monst[targ - 100],5 + r2);
				}
				break;
			case 25: // avatar
				play_sound(24);
				monst_spell_note(caster->number,26);
				caster->health = caster->m_health;
				caster->status[eStatus::MARTYRS_SHIELD] = 8;
				caster->status[eStatus::POISON] = 0;
				caster->status[eStatus::HASTE_SLOW] = 8;
				caster->status[eStatus::WEBS] = 0;
				caster->status[eStatus::DISEASE] = 0;
				caster->status[eStatus::DUMB] = 0;
				caster->status[eStatus::MARTYRS_SHIELD] = 8;
				break;
			case 26: // divine thud
				run_a_missile(l,target,9,0,11,0,0,80);
				r1 = (caster->level * 3) / 4 + 5;
				if (r1 > 29) r1 = 29;
				start_missile_anim();
				place_spell_pattern(rad2,target,DAMAGE_MAGIC,r1,7 );
				ashes_loc = target;
				break;
		}
		
		
	}
	else caster->mp++;
	if (ashes_loc.x > 0)
		univ.town.set_ash(ashes_loc.x,ashes_loc.y,true);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
	
	return acted;
}

void damage_target(short target,short dam,eDamageType type)
{
	if (target == 6) return;
	if (target < 6)
		damage_pc(target,dam,type,eRace::UNKNOWN,0);
	else damage_monst(target - 100, 7, dam, 0, type,0);
}


//	target = find_fireball_loc(caster->m_loc,1,(caster->attitude == 1) ? 0 : 1,&target_levels);

location find_fireball_loc(location where,short radius,short mode,short *m)
//short mode; // 0 - hostile casting  1 - friendly casting
{
	location check_loc,cast_loc(120,0);
	short cur_lev,level_max = 10;
	
	for (check_loc.x = 1; check_loc.x < univ.town->max_dim() - 1; check_loc.x ++)
		for (check_loc.y = 1; check_loc.y < univ.town->max_dim() - 1; check_loc.y ++)
			if(dist(where,check_loc) <= 8 && can_see(where,check_loc,sight_obscurity) < 5 && sight_obscurity(check_loc.x,check_loc.y) < 5) {
				{
					cur_lev = count_levels(check_loc,radius);
					if (mode == 1)
						cur_lev = cur_lev * -1;
					if ( ((cur_lev > level_max) || ((cur_lev == level_max) && (get_ran(1,0,1) == 0)))
						&& (dist(where,check_loc) > radius)) {
						level_max = cur_lev;
						cast_loc = check_loc;
					}
				}
			}
	*m = level_max;
	
	return cast_loc;
}

location closest_pc_loc(location where)
{
	short i;
	location pc_where(120,120);
	
	for (i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			if ((dist(where,univ.party[i].combat_pos)) < (dist(where,pc_where)))
				pc_where = univ.party[i].combat_pos;
	return pc_where;
}

short count_levels(location where,short radius)
{
	short i,store = 0;
	
	for (i = 0; i < univ.town->max_monst(); i++)
		if (monst_near(i,where,radius,0) == true) {
			if (univ.town.monst[i].attitude % 2 == 1)
				store = store - univ.town.monst[i].level;
			else store = store + univ.town.monst[i].level;
		}
	if (is_combat()) {
		for (i = 0; i < 6; i++)
			if (pc_near(i,where,radius) == true)
				store = store + 10;
	}
	if (is_town())
		if(vdist(where,univ.town.p_loc) <= radius && can_see(where,univ.town.p_loc,sight_obscurity) < 5)
			store += 20;
	
	return store;
}

bool pc_near(short pc_num,location where,short radius)
{
	// Assuming not looking
	if (overall_mode >= MODE_COMBAT) {
		if(univ.party[pc_num].main_status == eMainStatus::ALIVE && vdist(univ.party[pc_num].combat_pos,where) <= radius)
			return true;
		else return false;
	}
	if(univ.party[pc_num].main_status == eMainStatus::ALIVE && vdist(univ.town.p_loc,where) <= radius)
		return true;
	else return false;
}

/*short pc_there(where)
location where;
{
	short i;
	
	for (i = 0; i < 6; i++)
		if ((univ.party[i].main_status == 1) && (univ.party[i].combat_pos.x == where.x) && (univ.party[i].combat_pos.y == where.y))
			return i;
	return 6;
} */

bool monst_near(short m_num,location where,short radius,short active)
//short active; // 0 - any monst 1 - monster need be active
{
	if ((univ.town.monst[m_num].active != 0) && (vdist(univ.town.monst[m_num].cur_loc,where) <= radius)
		&& ((active == 0) || (univ.town.monst[m_num].active == 2)) )
		return true;
	else return false;
}

void fireball_space(location loc,short dam)
{
	place_spell_pattern(square,loc,DAMAGE_FIRE,dam,7);
}

static void place_spell_pattern(effect_pat_type pat,location center,unsigned short type,short who_hit)
//type;  // 0 - take codes in pattern, OW make all nonzero this type
// Types  0 - Null  1 - web  2 - fire barrier  3 - force barrier  4 - force wall  5 - fire wall
//   6 - anti-magic field  7 - stink cloud  8 - ice wall  9 - blade wall  10 - quickfire
//   11 - dispel  12 - sleep field
//  50 + i - 80 :  id6 fire damage  90 + i - 120 : id6 cold damage 	130 + i - 160 : id6 magic dam.
// if prep for anim is true, supporess look checks and go fast
{
	short i,j,r1,k = 0;
	unsigned short effect;
	location spot_hit;
	location s_loc;
	rectangle active;
	cCreature *which_m;
	bool monster_hit = false;
	
	
	if(type != FIELD_NONE)
		modify_pattern(&pat,type);
	
	
	
	active = univ.town->in_town_rect;
	// eliminate barriers that can't be seen
	for (i = minmax(active.left + 1,active.right - 1,center.x - 4);
		 i <= minmax(active.left + 1,active.right - 1,center.x + 4); i++)
		for (j = minmax(active.top + 1,active.bottom - 1,center.y - 4);
			 j <= minmax(active.top + 1,active.bottom - 1,center.y + 4); j++) {
			s_loc.x = i; s_loc.y = j;
			if (can_see_light(center,s_loc,sight_obscurity) == 5)
				pat.pattern[i - center.x + 4][j - center.y + 4] = 0;
		}
	
	
	// First actually make barriers, then draw them, then inflict damaging effects.
	for (i = minmax(0,univ.town->max_dim() - 1,center.x - 4); i <= minmax(0,univ.town->max_dim() - 1,center.x + 4); i++)
		for (j = minmax(0,univ.town->max_dim() - 1,center.y - 4); j <= minmax(0,univ.town->max_dim() - 1,center.y + 4); j++)
			if(sight_obscurity(i,j) < 5) {
				effect = pat.pattern[i - center.x + 4][j - center.y + 4];
				switch (effect) {
					case FIELD_WEB:
						web_space(i,j);
						break;
					case BARRIER_FIRE:
						univ.town.set_fire_barr(i,j,true);
						break;
					case BARRIER_FORCE:
						univ.town.set_force_barr(i,j,true);
						break;
					case WALL_FORCE:
						univ.town.set_force_wall(i, j, true);
						break;
					case WALL_FIRE:
						univ.town.set_fire_wall(i,j,true);
						break;
					case FIELD_ANTIMAGIC:
						univ.town.set_antimagic(i,j,true);
						break;
					case CLOUD_STINK:
						scloud_space(i,j);
						break;
					case WALL_ICE:
						univ.town.set_ice_wall(i,j,true);
						break;
					case WALL_BLADES:
						univ.town.set_blade_wall(i,j,true);
						break;
					case FIELD_QUICKFIRE:
						univ.town.set_quickfire(i,j,true);
						break;
					case FIELD_DISPEL:
						dispel_fields(i,j,0);
						break;
					case CLOUD_SLEEP:
						sleep_cloud_space(i,j);
						break;
					case FIELD_SMASH:
						crumble_wall(loc(i,j));
						break;
					case OBJECT_CRATE:
						univ.town.set_crate(i,j,true);
						break;
					case OBJECT_BARREL:
						univ.town.set_barrel(i,j,true);
						break;
					case OBJECT_BLOCK:
						univ.town.set_block(i,j,true);
						break;
					case BARRIER_CAGE:
						univ.town.set_force_cage(i, j, true);
						break;
					case SFX_SMALL_BLOOD:
						univ.town.set_sm_blood(i,j,true);
						break;
					case SFX_MEDIUM_BLOOD:
						univ.town.set_med_blood(i,j,true);
						break;
					case SFX_LARGE_BLOOD:
						univ.town.set_lg_blood(i,j,true);
						break;
					case SFX_SMALL_SLIME:
						univ.town.set_sm_slime(i,j,true);
						break;
					case SFX_LARGE_SLIME:
						univ.town.set_lg_slime(i,j,true);
						break;
					case SFX_ASH:
						univ.town.set_ash(i,j,true);
						break;
					case SFX_BONES:
						univ.town.set_bones(i,j,true);
						break;
					case SFX_RUBBLE:
						univ.town.set_rubble(i,j,true);
						break;
				}
			}
	draw_terrain(0);
	if (is_town()) // now make things move faster if in town
		fast_bang = 2;
	
	// Damage to pcs
	for (k = 0; k < 6; k++)
		for (i = minmax(0,univ.town->max_dim() - 1,center.x - 4); i <= minmax(0,univ.town->max_dim() - 1,center.x + 4); i++)
			for (j = minmax(0,univ.town->max_dim() - 1,center.y - 4); j <= minmax(0,univ.town->max_dim() - 1,center.y + 4); j++) {
				spot_hit.x = i;
				spot_hit.y = j;
				if(sight_obscurity(i,j) < 5 && univ.party[k].main_status == eMainStatus::ALIVE
					&& (((is_combat()) && (univ.party[k].combat_pos == spot_hit)) ||
						((is_town()) && (univ.town.p_loc == spot_hit)))) {
						effect = pat.pattern[i - center.x + 4][j - center.y + 4];
						switch (effect) {
							case WALL_FORCE:
								r1 = get_ran(2,1,6);
								damage_pc(k,r1,DAMAGE_MAGIC,eRace::UNKNOWN,0);
								break;
							case WALL_FIRE:
								r1 = get_ran(1,1,6) + 1;
								damage_pc(k,r1,DAMAGE_FIRE,eRace::UNKNOWN,0);
								break;
							case WALL_ICE:
								r1 = get_ran(2,1,6);
								damage_pc(k,r1,DAMAGE_COLD,eRace::UNKNOWN,0);
								break;
							case WALL_BLADES:
								r1 = get_ran(4,1,8);
								damage_pc(k,r1,DAMAGE_WEAPON,eRace::UNKNOWN,0);
								break;
							case OBJECT_BLOCK:
								r1 = get_ran(6,1,8);
								damage_pc(k,r1,DAMAGE_WEAPON,eRace::UNKNOWN,0);
								break;
							case BARRIER_CAGE:
								univ.party[k].status[eStatus::FORCECAGE] = 8;
								break;
							default:
								eDamageType type = DAMAGE_MARKED;
								unsigned short dice;
								if(effect > 50 && effect <= 80) {
									type = DAMAGE_FIRE;
									dice = effect - 50;
								} else if(effect > 90 && effect <= 120) {
									type = DAMAGE_COLD;
									dice = effect - 90;
								} else if(effect > 130 && effect <= 160) {
									type = DAMAGE_MAGIC;
									dice = effect - 130;
									// The rest of these are new, currently unused.
								} else if(effect > 170 && effect <= 200) {
									type = DAMAGE_WEAPON;
									dice = effect - 170;
								} else if(effect > 210 && effect <= 240) {
									type = DAMAGE_POISON;
									dice = effect - 210;
								} else if(effect > 250 && effect <= 280) {
									type = DAMAGE_UNBLOCKABLE;
									dice = effect - 250;
								} else if(effect > 290 && effect <= 320) {
									type = DAMAGE_UNDEAD;
									dice = effect - 290;
								} else if(effect > 330 && effect <= 360) {
									type = DAMAGE_DEMON;
									dice = effect - 330;
								}
								if(type == DAMAGE_MARKED) break;
								r1 = get_ran(dice,1,6);
								damage_pc(k,r1,type,eRace::UNKNOWN,0);
								break;
						}
					}
			}
	
	fast_bang = 0;
	
	// Damage to monsters
	for (k = 0; k < univ.town->max_monst(); k++)
		if ((univ.town.monst[k].active > 0) && (dist(center,univ.town.monst[k].cur_loc) <= 5)) {
			monster_hit = false;
			// First actually make barriers, then draw them, then inflict damaging effects.
			for (i = minmax(0,univ.town->max_dim() - 1,center.x - 4); i <= minmax(0,univ.town->max_dim() - 1,center.x + 4); i++)
				for (j = minmax(0,univ.town->max_dim() - 1,center.y - 4); j <= minmax(0,univ.town->max_dim() - 1,center.y + 4); j++) {
					spot_hit.x = i;
					spot_hit.y = j;
					
					if(!monster_hit && sight_obscurity(i,j) < 5 && monst_on_space(spot_hit,k) > 0) {
						
						if (pat.pattern[i - center.x + 4][j - center.y + 4] > 0)
							monster_hit = true;
						effect = pat.pattern[i - center.x + 4][j - center.y + 4];
						switch (effect) {
							case FIELD_WEB:
								which_m = &univ.town.monst[k];
								web_monst(which_m,3);
								break;
							case WALL_FORCE:
								r1 = get_ran(3,1,6);
								damage_monst(k, who_hit, r1,0, DAMAGE_MAGIC,0);
								break;
							case WALL_FIRE:
								r1 = get_ran(2,1,6);
								which_m = &univ.town.monst[k];
								if (which_m->spec_skill == 22)
									break;
								damage_monst(k, who_hit, r1,0, DAMAGE_FIRE,0);
								break;
							case CLOUD_STINK:
								which_m = &univ.town.monst[k];
								curse_monst(which_m,get_ran(1,1,2));
								break;
							case WALL_ICE:
								which_m = &univ.town.monst[k];
								r1 = get_ran(3,1,6);
								if (which_m->spec_skill == 23)
									break;
								damage_monst(k, who_hit, r1,0, DAMAGE_COLD,0);
								break;
							case WALL_BLADES:
								r1 = get_ran(6,1,8);
								damage_monst(k, who_hit, r1,0, DAMAGE_WEAPON,0);
								break;
							case CLOUD_SLEEP:
								which_m = &univ.town.monst[k];
								charm_monst(which_m,0,eStatus::ASLEEP,3);
								break;
							case OBJECT_BLOCK:
								r1 = get_ran(6,1,8);
								damage_monst(k,who_hit,r1,0,DAMAGE_WEAPON,0);
								break;
							case BARRIER_CAGE:
								univ.town.monst[k].status[eStatus::FORCECAGE] = 8;
								break;
							default:
								eDamageType type = DAMAGE_MARKED;
								unsigned short dice;
								if(effect > 50 && effect <= 80) {
									type = DAMAGE_FIRE;
									dice = effect - 50;
								} else if(effect > 90 && effect <= 120) {
									type = DAMAGE_COLD;
									dice = effect - 90;
								} else if(effect > 130 && effect <= 160) {
									type = DAMAGE_MAGIC;
									dice = effect - 130;
									// The rest of these are new, currently unused.
								} else if(effect > 170 && effect <= 200) {
									type = DAMAGE_WEAPON;
									dice = effect - 170;
								} else if(effect > 210 && effect <= 240) {
									type = DAMAGE_POISON;
									dice = effect - 210;
								} else if(effect > 250 && effect <= 280) {
									type = DAMAGE_UNBLOCKABLE;
									dice = effect - 250;
								} else if(effect > 290 && effect <= 320) {
									type = DAMAGE_UNDEAD;
									dice = effect - 290;
								} else if(effect > 330 && effect <= 360) {
									type = DAMAGE_DEMON;
									dice = effect - 330;
								}
								if(type == DAMAGE_MARKED) break;
								r1 = get_ran(dice,1,6);
								damage_monst(k,who_hit,r1,0,type,0);
								break;
						}
					}
				}
		}
}
void handle_item_spell(location loc,short num)
{
	// TODO: This function is currently unused
	switch (num) {
		case 82: // Pyhrrus
			place_spell_pattern(rad2,loc,WALL_BLADES,6);
			break;
	}
	
}

void place_spell_pattern(effect_pat_type pat,location center,short who_hit) {
	place_spell_pattern(pat, center, FIELD_NONE, who_hit);
}

void place_spell_pattern(effect_pat_type pat,location center,eFieldType type,short who_hit) {
	unsigned short code = type;
	place_spell_pattern(pat, center, code, who_hit);
}

// Copied from place_spell_pattern comment above:
//  50 + i - 80 :  id6 fire damage  90 + i - 120 : id6 cold damage 	130 + i - 160 : id6 magic dam.
void place_spell_pattern(effect_pat_type pat,location center,eDamageType type,short dice,short who_hit) {
	unsigned short code;
	dice = minmax(1, 30, dice);
	switch(type) {
		case DAMAGE_FIRE: code = 50; break;
		case DAMAGE_COLD: code = 90; break;
		case DAMAGE_MAGIC: code = 130; break;
			// TODO: These are new; nothing actually uses them, but maybe eventually!
		case DAMAGE_WEAPON: code = 170; break;
		case DAMAGE_POISON: code = 210; break;
		case DAMAGE_UNBLOCKABLE: code = 250; break;
		case DAMAGE_UNDEAD: code = 290; break;
		case DAMAGE_DEMON: code = 330; break;
	}
	place_spell_pattern(pat, center, code + dice, who_hit);
}

void modify_pattern(effect_pat_type *pat,unsigned short type)
{
	short i,j;
	
	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			if (pat->pattern[i][j] > 0)
				pat->pattern[i][j] = type;
}

void do_shockwave(location target)
{
	short i;
	
	start_missile_anim();
	for (i = 0; i < 6; i++)
		if ((dist(target,univ.party[i].combat_pos) > 0) && (dist(target,univ.party[i].combat_pos) < 11)
			&& univ.party[i].main_status == eMainStatus::ALIVE)
			damage_pc(i, get_ran(2 + dist(target,univ.party[i].combat_pos) / 2, 1, 6), DAMAGE_UNBLOCKABLE,eRace::UNKNOWN,0);
	for (i = 0; i < univ.town->max_monst(); i++)
		if ((univ.town.monst[i].active != 0) && (dist(target,univ.town.monst[i].cur_loc) > 0)
			&& (dist(target,univ.town.monst[i].cur_loc) < 11)
			&& (can_see_light(target,univ.town.monst[i].cur_loc,sight_obscurity) < 5))
			damage_monst(i, current_pc, get_ran(2 + dist(target,univ.town.monst[i].cur_loc) / 2 , 1, 6), 0, DAMAGE_UNBLOCKABLE,0);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
}

void radius_damage(location target,short radius, short dam, eDamageType type)////
{
	short i;
	
	if (is_town()) {
		for (i = 0; i < 6; i++)
			if ((dist(target,univ.town.p_loc) > 0) && (dist(target,univ.town.p_loc) <= radius)
				&& univ.party[i].main_status == eMainStatus::ALIVE)
				damage_pc(i, dam, type,eRace::UNKNOWN,0);
		for (i = 0; i < univ.town->max_monst(); i++)
			if ((univ.town.monst[i].active != 0) && (dist(target,univ.town.monst[i].cur_loc) > 0)
				&& (dist(target,univ.town.monst[i].cur_loc) <= radius)
				&& (can_see_light(target,univ.town.monst[i].cur_loc,sight_obscurity) < 5))
				damage_monst(i, current_pc, dam, 0, type,0);
		return;
	}
	
	start_missile_anim();
	for (i = 0; i < 6; i++)
		if ((dist(target,univ.party[i].combat_pos) > 0) && (dist(target,univ.party[i].combat_pos) <= radius)
			&& univ.party[i].main_status == eMainStatus::ALIVE)
			damage_pc(i, dam, type,eRace::UNKNOWN,0);
	for (i = 0; i < univ.town->max_monst(); i++)
		if ((univ.town.monst[i].active != 0) && (dist(target,univ.town.monst[i].cur_loc) > 0)
			&& (dist(target,univ.town.monst[i].cur_loc) <= radius)
			&& (can_see_light(target,univ.town.monst[i].cur_loc,sight_obscurity) < 5))
			damage_monst(i, current_pc, dam, 0, type,0);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
}
// Slightly kludgy way to only damage PCs in space)
void hit_pcs_in_space(location target,short dam,eDamageType type,short report,short hit_all)
{
	//short store_active[T_M],i;
	
	//for (i = 0; i < T_M; i++) {
	//	store_active[i] = univ.town.monst[i].active;
	//	univ.town.monst[i].active = 0;
	//}
	hit_space(target, dam,type, report, 10 + hit_all);
	//for (i = 0; i < T_M; i++)
	//	univ.town.monst[i].active = store_active[i];
}

void hit_space(location target,short dam,eDamageType type,short report,short hit_all)
//type; // 0 - weapon   1 - fire   2 - poison   3 - general magic   4 - unblockable  5 - cold
//	6 - demon	7 - undead
//short report; // 0 - no  1 - print result
//hit_all; // 0 - nail top thing   1 - hit all in space  + 10 ... no monsters
{
	short i;
	bool stop_hitting = false,hit_monsters = true;
	
//	sprintf ((char *) create_line, "  %d %d.                                  ",target.x,target.y);
//	add_string_to_buf((char *) create_line);
	if ((target.x < 0) || (target.x > 63) || (target.y < 0) || (target.y > 63))
		return;
	
	if (hit_all >= 10) {
		hit_monsters = false;
		hit_all -= 10;
	}
	
	if ((univ.town.is_antimagic(target.x,target.y)) && ((type == 1) || (type == 3) || (type == 5))) {
		return;
	}
	
	if (dam <= 0) {
		add_string_to_buf("  No damage.");
		return;
	}
	
	for (i = 0; i < univ.town->max_monst(); i++)
		if ((hit_monsters == true) && (univ.town.monst[i].active != 0) && (stop_hitting == false))
			if (monst_on_space(target,i)) {
				if (processing_fields == true)
					damage_monst(i, 6, dam, 0, type,0);
				else damage_monst(i, (monsters_going == true) ? 7 : current_pc, dam, 0, type,0);
				stop_hitting = (hit_all == 1) ? false : true;
			}
	
	if (overall_mode >= MODE_COMBAT)
		for (i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE && !stop_hitting)
				if (univ.party[i].combat_pos == target) {
					damage_pc(i,dam,type,eRace::UNKNOWN,0);
					stop_hitting = (hit_all == 1) ? false : true;
				}
	if (overall_mode < MODE_COMBAT)
		if (target == univ.town.p_loc) {
			fast_bang = 1;
			hit_party(dam,type);
			fast_bang = 0;
			stop_hitting = (hit_all == 1) ? false : true;
		}
	
	if ((report == 1) && (hit_all == 0) && (stop_hitting == false))
		add_string_to_buf("  Missed.");
	
}



void do_poison()
{
	short i,r1 = 0;
	bool some_poison = false;
	
	for (i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			if(univ.party[i].status[eStatus::POISON] > 0)
				some_poison = true;
	if (some_poison == true) {
		add_string_to_buf("Poison:                        ");
		for (i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if(univ.party[i].status[eStatus::POISON] > 0) {
					r1 = get_ran(univ.party[i].status[eStatus::POISON],1,6);
					damage_pc(i,r1,DAMAGE_POISON,eRace::UNKNOWN,0);
					if (get_ran(1,0,8) < 6)
						move_to_zero(univ.party[i].status[eStatus::POISON]);
					if (get_ran(1,0,8) < 6)
						if(univ.party[i].traits[eTrait::GOOD_CONST])
							move_to_zero(univ.party[i].status[eStatus::POISON]);
					// TODO: Shouldn't the above two conditionals be swapped?
				}
		put_pc_screen();
		//if (overall_mode < 10)
		//	boom_space(univ.univ.party.p_loc,overall_mode,2,r1);
	}
}


void handle_disease()
{
	short i,r1 = 0;
	bool disease = false;
	
	for (i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			if(univ.party[i].status[eStatus::DISEASE] > 0)
				disease = true;
	
	if (disease == true) {
		add_string_to_buf("Disease:                        ");
		for (i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if(univ.party[i].status[eStatus::DISEASE] > 0) {
					r1 = get_ran(1,1,10);
					switch (r1) {
						case 1: case 2:
							poison_pc(i,2);
							break;
						case 3: case 4:
							slow_pc(i,2);
							break;
						case 5:
							drain_pc(i,5);
							break;
						case 6: case 7:
							curse_pc(i,3);
							break;
						case 8:
							dumbfound_pc(i,3);
							break;
						case 9: case 10:
							sprintf (create_line, "  %s unaffected. ",
									 univ.party[i].name.c_str());
							add_string_to_buf(create_line);
							break;
					}
					r1 = get_ran(1,0,7);
					if(univ.party[i].traits[eTrait::GOOD_CONST])
						r1 -= 2;
					if ((get_ran(1,0,7) <= 0) || (pc_has_abil_equip(i,eItemAbil::PROTECT_FROM_DISEASE) < 24))
						move_to_zero(univ.party[i].status[eStatus::DISEASE]);
				}
		put_pc_screen();
	}
}

void handle_acid()
{
	short i,r1 = 0;
	bool some_acid = false;
	
	for (i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			if(univ.party[i].status[eStatus::ACID] > 0)
				some_acid = true;
	
	if (some_acid == true) {
		add_string_to_buf("Acid:                        ");
		for (i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if(univ.party[i].status[eStatus::ACID] > 0) {
					r1 = get_ran(univ.party[i].status[eStatus::ACID],1,6);
					damage_pc(i,r1,DAMAGE_MAGIC,eRace::UNKNOWN,0);
					move_to_zero(univ.party[i].status[eStatus::ACID]);
				}
		if (overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,3,r1,8);
	}
}

bool no_pcs_left()
{
	short i = 0;
	
	while (i < 6) {
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			return false;
		i++;
	}
	return true;
	
}

bool hit_end_c_button()
{
	bool end_ok = true;
	
	if (which_combat_type == 0) {
		end_ok = out_monst_all_dead();
	}
	for(int i = 0; i < 6; i++) {
		if(univ.party[i].status[eStatus::FORCECAGE] > 0) {
			add_string_to_buf("  Someone trapped.");
			return false;
		}
	}
	
	if (end_ok == true)
		end_combat();
	return end_ok;
}

bool out_monst_all_dead()
{
	short i;
	
	for (i = 0; i < univ.town->max_monst(); i++)
		if ((univ.town.monst[i].active > 0) && (univ.town.monst[i].attitude % 2 == 1)) {
			//print_nums(5555,i,univ.town.monst[i].number);
			//print_nums(5555,univ.town.monst[i].m_loc.x,univ.town.monst[i].m_loc.y);
			return false;
		}
	return true;
}

void end_combat()
{
	short i;
	
	for (i = 0; i < 6; i++) {
		if(univ.party[i].main_status == eMainStatus::FLED)
			univ.party[i].main_status = eMainStatus::ALIVE;
		univ.party[i].status[eStatus::POISONED_WEAPON] = 0;
		univ.party[i].status[eStatus::BLESS_CURSE] = 0;
		univ.party[i].status[eStatus::HASTE_SLOW] = 0;
	}
	if (which_combat_type == 0) {
		overall_mode = MODE_OUTDOORS;
	}
	combat_active_pc = 6;
	current_pc = store_current_pc;
	if(univ.party[current_pc].main_status != eMainStatus::ALIVE)
		current_pc = first_active_pc();
	put_item_screen(stat_window,0);
	draw_buttons(0);
}


bool combat_cast_mage_spell()
{
	short target,i,store_sp,bonus = 1,r1,store_sound = 0,store_m_type = 0,num_opp = 0;
	eSpell spell_num;
	char c_line[60];
	cCreature *which_m;
	cMonster get_monst;
	
	if (univ.town.is_antimagic(univ.party[current_pc].combat_pos.x,univ.party[current_pc].combat_pos.y)) {
		add_string_to_buf("  Not in antimagic field.");
		return false;
	}
	store_sp = univ.party[current_pc].cur_sp;
	if (univ.party[current_pc].cur_sp == 0)
		add_string_to_buf("Cast: No spell points.        ");
	else if (univ.party[current_pc].skills[eSkill::MAGE_SPELLS] == 0)
		add_string_to_buf("Cast: No mage skill.        ");
	else if (get_encumberance(current_pc) > 1) {
		add_string_to_buf("Cast: Too encumbered.        ");
		take_ap(6);
		give_help(40,0);
		return true;
	}
	else {
		
		
		if (spell_forced == false)
			spell_num = pick_spell(current_pc,eSkill::MAGE_SPELLS);
		else {
			if(!repeat_cast_ok(eSkill::MAGE_SPELLS))
				return false;
			spell_num = univ.party[current_pc].last_cast[eSkill::MAGE_SPELLS];
		}
		
		if (spell_num == eSpell::SIMULACRUM) {
			store_sum_monst = pick_trapped_monst();
			if (store_sum_monst == 0)
				return false;
			get_monst = scenario.scen_monsters[store_sum_monst];
			if (store_sp < get_monst.level) {
				add_string_to_buf("Cast: Not enough spell points.        ");
				return false;
			}
			store_sum_monst_cost = get_monst.level;
		}
		
		bonus = stat_adj(current_pc,eSkill::INTELLIGENCE);
		combat_posing_monster = current_working_monster = current_pc;
		if(spell_num == eSpell::NONE) return false;
			print_spell_cast(spell_num,eSkill::MAGE_SPELLS);
			if ((*spell_num).refer == REFER_YES) {
				take_ap(6);
				draw_terrain(2);
				do_mage_spell(current_pc,spell_num);
				combat_posing_monster = current_working_monster = -1;
			}
			else if ((*spell_num).refer == REFER_TARGET) {
				start_spell_targeting(spell_num);
			}
			else if ((*spell_num).refer == REFER_FANCY) {
				start_fancy_spell_targeting(spell_num);
			}
			else {
				start_missile_anim();
				take_ap(6);
				draw_terrain(2);
				switch (spell_num) {
					case eSpell::SHOCKWAVE:
						univ.party[current_pc].cur_sp -= (*spell_num).cost;
						add_string_to_buf("  The ground shakes.          ");
						do_shockwave(univ.party[current_pc].combat_pos);
						break;
						
					case eSpell::HASTE_MINOR: case eSpell::HASTE: case eSpell::STRENGTH: case eSpell::ENVENOM: case eSpell::RESIST_MAGIC:
//						target = select_pc(11,0);
						target = store_spell_target;
						if (target < 6) {
							univ.party[current_pc].cur_sp -= (*spell_num).cost;
							play_sound(4);
							switch (spell_num) {
								case eSpell::ENVENOM:
									sprintf (c_line, "  %s receives venom.               ",
											 univ.party[target].name.c_str());
									poison_weapon(target,3 + bonus,1);
									store_m_type = 11;
									break;
									
								case eSpell::STRENGTH:
									sprintf (c_line, "  %s stronger.                     ",
											 univ.party[target].name.c_str());
									univ.party[target].status[eStatus::BLESS_CURSE] = univ.party[target].status[eStatus::BLESS_CURSE] + 3;
									store_m_type = 8;
									break;
								case eSpell::RESIST_MAGIC:
									sprintf (c_line, "  %s resistant.                     ",
											 univ.party[target].name.c_str());
									univ.party[target].status[eStatus::MAGIC_RESISTANCE] = univ.party[target].status[eStatus::MAGIC_RESISTANCE] + 5 + bonus;
									store_m_type = 15;
									break;
									
								default:
									i = (spell_num == eSpell::HASTE_MINOR) ? 2 : max(2,univ.party[current_pc].level / 2 + bonus);
									univ.party[target].status[eStatus::HASTE_SLOW] = min(8, univ.party[target].status[eStatus::HASTE_SLOW] + i);
									sprintf (c_line, "  %s hasted.                       ",
											 univ.party[target].name.c_str());
									store_m_type = 8;
									break;
							}
							add_string_to_buf(c_line);
							add_missile(univ.party[target].combat_pos,store_m_type,0,0,0);
						}
						break;
						
					case eSpell::HASTE_MAJOR: case eSpell::BLESS_MAJOR:
						store_sound = 25;
						univ.party[current_pc].cur_sp -= (*spell_num).cost;
						
						
						for (i = 0; i < 6; i++)
							if(univ.party[i].main_status == eMainStatus::ALIVE) {
								univ.party[i].status[eStatus::HASTE_SLOW] = min(8, univ.party[i].status[eStatus::HASTE_SLOW] + ((spell_num == eSpell::HASTE_MAJOR) ? 1 + univ.party[current_pc].level / 8 + bonus : 3 + bonus));
								if (spell_num == eSpell::BLESS_MAJOR) {
									poison_weapon(i,2,1);
									univ.party[i].status[eStatus::BLESS_CURSE] += 4;
									add_missile(univ.party[i].combat_pos,14,0,0,0);
								}
								else add_missile(univ.party[i].combat_pos,8,0,0,0);
							}
						//play_sound(4);
						if (spell_num == eSpell::HASTE_MAJOR)
							sprintf (c_line, "  Party hasted.     ");
						else
							sprintf (c_line, "  Party blessed!           ");
						add_string_to_buf(c_line);
						
						break;
						
						
						
					case eSpell::SLOW_GROUP: case eSpell::FEAR_GROUP: case eSpell::PARALYSIS_MASS: // affect monsters in area spells
						univ.party[current_pc].cur_sp -= (*spell_num).cost;
						store_sound = 25;
						if (spell_num == eSpell::FEAR_GROUP)
							store_sound = 54;
						switch (spell_num) {
							case eSpell::SLOW_GROUP: sprintf (c_line, "  Enemy slowed:       "); break;
							case eSpell::RAVAGE_ENEMIES: sprintf (c_line, "  Enemy ravaged:              ");break;
							case eSpell::FEAR_GROUP: sprintf (c_line, "  Enemy scared:   ");break;
							case eSpell::PARALYSIS_MASS: sprintf (c_line, "  Enemy paralyzed:   ");break;
						}
						add_string_to_buf(c_line);
						for (i = 0; i < univ.town->max_monst(); i++) {
							if ((univ.town.monst[i].active != 0) && (univ.town.monst[i].attitude % 2 == 1)
								&& (dist(univ.party[current_pc].combat_pos,univ.town.monst[i].cur_loc) <= (*spell_num).range)
								&& (can_see_light(univ.party[current_pc].combat_pos,univ.town.monst[i].cur_loc,sight_obscurity) < 5)) {
								which_m = &univ.town.monst[i];
								switch (spell_num) {
									case eSpell::FEAR_GROUP:
										r1 = get_ran(univ.party[current_pc].level / 3,1,8);
										scare_monst(which_m,r1);
										store_m_type = 10;
										break;
									case eSpell::SLOW_GROUP: case eSpell::RAVAGE_ENEMIES:
										slow_monst(which_m,5 + bonus);
										if (spell_num == eSpell::RAVAGE_ENEMIES)
											curse_monst(which_m,3 + bonus);
										store_m_type = 8;
										break;
									case eSpell::PARALYSIS_MASS:
										charm_monst(which_m,15,eStatus::PARALYZED,1000);
										store_m_type = 15;
										break;
								}
								num_opp++;
								add_missile(univ.town.monst[i].cur_loc,store_m_type,0,
											14 * (which_m->x_width - 1),18 * (which_m->y_width - 1));
							}
							
						}
						break;
						
				}
				
			}
			if (num_opp < 10)
				do_missile_anim((num_opp < 5) ? 50 : 25,univ.party[current_pc].combat_pos,store_sound);
			else play_sound(store_sound);
			end_missile_anim();
			put_pc_screen();
	}
	combat_posing_monster = current_working_monster = -1;
	// Did anything actually get cast?
	if (store_sp == univ.party[current_pc].cur_sp)
		return false;
	else return true;
}


bool combat_cast_priest_spell()
{
	short target,i,store_sp,bonus,store_sound = 0,store_m_type = 0,num_opp = 0;
	eSpell spell_num;
	char c_line[60];
	cCreature *which_m;
	effect_pat_type protect_pat = {{
		{0,4,4,4,4,4,4,4,0},
		{4,8,8,8,8,8,8,8,4},
		{4,8,9,9,9,9,9,8,4},
		{4,8,9,6,6,6,9,8,4},
		{4,8,9,6,6,6,9,8,4},
		{4,8,9,6,6,6,9,8,4},
		{4,8,9,9,9,9,9,8,4},
		{4,8,8,8,8,8,8,8,4},
		{0,4,4,4,4,4,4,4,0}}};
	
	if (univ.town.is_antimagic(univ.party[current_pc].combat_pos.x,univ.party[current_pc].combat_pos.y)) {
		add_string_to_buf("  Not in antimagic field.");
		return false;
	}
	if (spell_forced == false)
		spell_num = pick_spell(current_pc,eSkill::PRIEST_SPELLS);
	else {
		if(!repeat_cast_ok(eSkill::PRIEST_SPELLS))
			return false;
		spell_num = univ.party[current_pc].last_cast[eSkill::PRIEST_SPELLS];
	}
	
	store_sp = univ.party[current_pc].cur_sp;
	if(univ.party[current_pc].cur_sp == 0) {
		add_string_to_buf("Cast: No spell points.        ");
		return false;
	} else if(univ.party[current_pc].skills[eSkill::PRIEST_SPELLS] == 0) {
		add_string_to_buf("Cast: No priest skill.        ");
		return false;
	}
	
	if(spell_num == eSpell::NONE) return false;
	bonus = stat_adj(current_pc,eSkill::INTELLIGENCE);
	
	combat_posing_monster = current_working_monster = current_pc;
	
	if (univ.party[current_pc].cur_sp == 0)
		add_string_to_buf("Cast: No spell points.        ");
	else if(spell_num != eSpell::NONE) {
		print_spell_cast(spell_num,eSkill::PRIEST_SPELLS);
		if ((*spell_num).refer == REFER_YES) {
			take_ap(5);
			draw_terrain(2);
			do_priest_spell(current_pc,spell_num);
		}
		else if ((*spell_num).refer == REFER_TARGET) {
			start_spell_targeting(spell_num);
		}
		else if ((*spell_num).refer == REFER_FANCY) {
			start_fancy_spell_targeting(spell_num);
		}
		else {
			start_missile_anim();
			take_ap(5);
			draw_terrain(2);
			switch (spell_num) {
				case eSpell::BLESS_MINOR: case eSpell::BLESS:
//					target = select_pc(11,0);
					target = store_spell_target;
					if (target < 6) {
						store_sound = 4;
						univ.party[current_pc].cur_sp -= (*spell_num).cost;
						univ.party[target].status[eStatus::BLESS_CURSE] += (spell_num == eSpell::BLESS_MINOR) ? 2 :
						max(2,(univ.party[current_pc].level * 3) / 4 + 1 + bonus);
						sprintf ((char *) c_line, "  %s blessed.              ",
								 (char *) univ.party[target].name.c_str());
						add_string_to_buf((char *) c_line);
						add_missile(univ.party[target].combat_pos,8,0,0,0);
					}
					break;
					
				case eSpell::BLESS_PARTY:
					univ.party[current_pc].cur_sp -= (*spell_num).cost;
					for (i = 0; i < 6; i++)
						if(univ.party[i].main_status == eMainStatus::ALIVE) {
							univ.party[i].status[eStatus::BLESS_CURSE] += univ.party[current_pc].level / 3;
							add_missile(univ.party[i].combat_pos,8,0,0,0);
						}
					sprintf ((char *) c_line, "  Party blessed.                    ");
					add_string_to_buf((char *) c_line);
					store_sound = 4;
					break;
					
				case eSpell::AVATAR:
					univ.party[current_pc].cur_sp -= (*spell_num).cost;
					sprintf ((char *) c_line, "  %s is an avatar! ",
							 (char *) univ.party[current_pc].name.c_str());
					add_string_to_buf((char *) c_line);
					heal_pc(current_pc,200);
					cure_pc(current_pc,8);
					univ.party[current_pc].status[eStatus::BLESS_CURSE] = 8;
					univ.party[current_pc].status[eStatus::HASTE_SLOW] = 8;
					univ.party[current_pc].status[eStatus::INVULNERABLE] = 3;
					univ.party[current_pc].status[eStatus::MAGIC_RESISTANCE] = 8;
					univ.party[current_pc].status[eStatus::WEBS] = 0;
					univ.party[current_pc].status[eStatus::DISEASE] = 0;
					univ.party[current_pc].status[eStatus::DUMB] = 0;
					univ.party[current_pc].status[eStatus::MARTYRS_SHIELD] = 8;
					break;
					
				case eSpell::CURSE_ALL: case eSpell::CHARM_MASS: case eSpell::PESTILENCE:
					univ.party[current_pc].cur_sp -= (*spell_num).cost;
					store_sound = 24;
					for (i = 0; i < univ.town->max_monst(); i++) {
						if ((univ.town.monst[i].active != 0) &&(univ.town.monst[i].attitude % 2 == 1) &&
							(dist(univ.party[current_pc].combat_pos,univ.town.monst[i].cur_loc) <= (*spell_num).range)) {
							which_m = &univ.town.monst[i];
							switch (spell_num) {
								case eSpell::CURSE_ALL:
									curse_monst(which_m,3 + bonus);
									store_m_type = 8;
									break;
								case eSpell::CHARM_MASS:
									charm_monst(which_m,28 - bonus,eStatus::CHARM,0);
									store_m_type = 14;
									break;
								case eSpell::PESTILENCE:
									disease_monst(which_m,3 + bonus);
									store_m_type = 0;
									break;
							}
							num_opp++;
							add_missile(univ.town.monst[i].cur_loc,store_m_type,0,
										14 * (which_m->x_width - 1),18 * (which_m->y_width - 1));
						}
					}
					
					break;
					
				case eSpell::PROTECTIVE_CIRCLE:
					univ.party[current_pc].cur_sp -= (*spell_num).cost;
					play_sound(24);
					add_string_to_buf("  Protective field created.");
					place_spell_pattern(protect_pat,univ.party[current_pc].combat_pos,6);
					break;
			}
		}
		if (num_opp < 10)
			do_missile_anim((num_opp < 5) ? 50 : 25,univ.party[current_pc].combat_pos,store_sound);
		else play_sound(store_sound);
		end_missile_anim();
		put_pc_screen();
	}
	
	combat_posing_monster = current_working_monster = -1;
	// Did anything actually get cast?
	if (store_sp == univ.party[current_pc].cur_sp)
		return false;
	else return true;
}

void start_spell_targeting(eSpell num, bool freebie)
{
	
	// First, remember what spell was cast.
	spell_being_cast = num;
	spell_freebie = freebie;
	
	sprintf ((char *) create_line, "  Target spell.                ");
	add_string_to_buf((char *) create_line);
	if(isMage(num))
		add_string_to_buf("  (Hit 'm' to cancel.)");
	else add_string_to_buf("  (Hit 'p' to cancel.)");
	overall_mode = MODE_SPELL_TARGET;
	current_spell_range = (*num).range;
	current_pat = single;
	
	switch (num) {  // Different spells have different messages and diff. target shapes
		case eSpell::CLOUD_SLEEP:
			current_pat = small_square;
			break;
		case eSpell::DISPEL_SQUARE: case eSpell::FIREBALL: case eSpell::CLOUD_STINK:
		case eSpell::FLAMESTRIKE: case eSpell::FORCEFIELD:
			current_pat = square;
			break;
		case eSpell::CONFLAGRATION: case eSpell::FIRESTORM: case eSpell::SHOCKSTORM: case eSpell::WEB:
		case eSpell::ANTIMAGIC: case eSpell::WALL_ICE_BALL: case eSpell::CLOUD_SLEEP_LARGE:
		case eSpell::DIVINE_THUD: case eSpell::DISPEL_SPHERE:
			current_pat = rad2;
			break;
		case eSpell::PESTILENCE: case eSpell::GOO_BOMB: case eSpell::FOUL_VAPOR:
			current_pat = rad3;
			break;
		case eSpell::WALL_FORCE: case eSpell::WALL_ICE: case eSpell::WALL_BLADES:
			add_string_to_buf("  (Hit space to rotate.)");
			force_wall_position = 0;
			current_pat = field[0];
			break;
	}
}

void start_fancy_spell_targeting(eSpell num, bool freebie)
{
	short i;
	location null_loc(120,0);
	
	// First, remember what spell was cast.
	spell_being_cast = num;
	spell_freebie = freebie;
	
	for (i = 0; i < 8; i++)
		spell_targets[i] = null_loc;
	sprintf (create_line, "  Target spell.                ");
	if(isMage(num))
		add_string_to_buf("  (Hit 'm' to cancel.)");
	else add_string_to_buf("  (Hit 'p' to cancel.)");
	add_string_to_buf("  (Hit space to cast.)");
	add_string_to_buf(create_line);
	overall_mode = MODE_FANCY_TARGET;
	current_pat = single;
	current_spell_range = (*num).range;
	
	switch (num) { // Assign special targeting shapes and number of targets
		case eSpell::SMITE:
			num_targets_left = minmax(1,8,univ.party[current_pc].level / 4 + stat_adj(current_pc,eSkill::INTELLIGENCE) / 2);
			break;
		case eSpell::STICKS_TO_SNAKES:
			num_targets_left = univ.party[current_pc].level / 5 + stat_adj(current_pc,eSkill::INTELLIGENCE) / 2;
			break;
		case eSpell::SUMMON_HOST:
			num_targets_left = 5;
			break;
		case eSpell::ARROWS_FLAME:
			num_targets_left = univ.party[current_pc].level / 4 + stat_adj(current_pc,eSkill::INTELLIGENCE) / 2;
			break;
		case eSpell::ARROWS_VENOM:
			num_targets_left = univ.party[current_pc].level / 5 + stat_adj(current_pc,eSkill::INTELLIGENCE) / 2;
			break;
		case eSpell::ARROWS_DEATH: case eSpell::PARALYZE:
			num_targets_left = univ.party[current_pc].level / 8 + stat_adj(current_pc,eSkill::INTELLIGENCE) / 3;
			break;
		case eSpell::SPRAY_FIELDS:
			num_targets_left = univ.party[current_pc].level / 5 + stat_adj(current_pc,eSkill::INTELLIGENCE) / 2;
			current_pat = t;
			break;
		case eSpell::SUMMON_WEAK:
			num_targets_left = minmax(1,7,univ.party[current_pc].level / 4 + stat_adj(current_pc,eSkill::INTELLIGENCE) / 2);
			break;
		case eSpell::SUMMON:
			num_targets_left = minmax(1,6,univ.party[current_pc].level / 6 + stat_adj(current_pc,eSkill::INTELLIGENCE) / 2);
			break;
		case eSpell::SUMMON_MAJOR:
			num_targets_left = minmax(1,5,univ.party[current_pc].level / 8 + stat_adj(current_pc,eSkill::INTELLIGENCE) / 2);
			break;
	}
	
	num_targets_left = minmax(1,8,num_targets_left);
}

void spell_cast_hit_return()
{
	
	if (force_wall_position < 10) {
		force_wall_position = (force_wall_position + 1) % 8;
		current_pat = field[force_wall_position];
	}
}

static void process_force_cage(location loc, short i) {
	if(i >= 100) {
		short m = i - 100;
		cCreature& which_m = univ.town.monst[m];
		if(which_m.attitude % 2 == 1 && get_ran(1,1,100) < which_m.mu * 10 + which_m.cl * 4 + 5) {
			play_sound(60);
			monst_spell_note(m, 50);
			univ.town.set_force_cage(loc.x,loc.y,false);
			which_m.status[eStatus::FORCECAGE] = 0;
		} else which_m.status[eStatus::FORCECAGE] = 8;
	} else if(i < 0) {
		if(get_ran(1,1,100) < 35)
			univ.town.set_force_cage(loc.x,loc.y,false);
	} else if(i < 6) {
		cPlayer& who = univ.party[i];
		// We want to make sure everyone has a chance of eventually breaking a cage, because it never ends on its own,
		// and because being trapped unconditionally prevents you from ending combat mode.
		short bonus = 5 + who.skills[eSkill::MAGE_LORE];
		if(get_ran(1,1,100) < who.skills[eSkill::MAGE_SPELLS]*10 + who.skills[eSkill::PRIEST_SPELLS]*4 + bonus) {
			play_sound(60);
			add_string_to_buf("  " + who.name + " breaks force cage.");
			univ.town.set_force_cage(loc.x,loc.y,false);
			who.status[eStatus::FORCECAGE] = 0;
		} else who.status[eStatus::FORCECAGE] = 8;
	}
}

void process_fields()
{
	short i,j,k,r1;
	location loc;
	char qf[64][64];
	rectangle r;
	
	if (is_out())
		return;
	
	if(univ.town.quickfire_present) {
		r = univ.town->in_town_rect;
		for (i = 0; i < univ.town->max_dim(); i++)
			for (j = 0; j < univ.town->max_dim(); j++)
				qf[i][j] = (univ.town.is_quickfire(i,j)) ? 2 : 0;
		for (k = 0; k < ((is_combat()) ? 4 : 1); k++) {
			for (i = r.left + 1; i < r.right ; i++)
				for (j = r.top + 1; j < r.bottom ; j++)
					if (univ.town.is_quickfire(i,j) > 0) {
						r1 = get_ran(1,1,8);
						if (r1 != 1) {
							qf[i - 1][j] = 1;
							qf[i + 1][j] = 1;
							qf[i][j + 1] = 1;
							qf[i][j - 1] = 1;
						}
					}
			for (i = r.left + 1; i < r.right ; i++)
				for (j = r.top + 1; j < r.bottom ; j++)
					if (qf[i][j] > 0) {
						univ.town.set_quickfire(i,j,true);
					}
		}
	}
	
	for (i = 0; i < univ.town->max_monst(); i++)
		if (univ.town.monst[i].active > 0)
			monst_inflict_fields(i);
	
	// First fry PCs, then call to handle damage to monsters
	processing_fields = true; // this, in hit_space, makes damage considered to come from whole party
		for (i = 0; i < univ.town->max_dim(); i++)
			for(j = 0; j < univ.town->max_dim(); j++) {
				if (univ.town.is_force_wall(i,j)) {
					r1 = get_ran(3,1,6);
					loc.x = i; loc.y = j;
					hit_pcs_in_space(loc,r1,DAMAGE_MAGIC,1,1);
					r1 = get_ran(1,1,6);
					if (r1 == 2)
						univ.town.set_force_wall(i,j,false);
				}
				if (univ.town.is_fire_wall(i,j)) {
					loc.x = i; loc.y = j;
					r1 = get_ran(2,1,6) + 1;
					hit_pcs_in_space(loc,r1,DAMAGE_FIRE,1,1);
					r1 = get_ran(1,1,4);
					if (r1 == 2)
						univ.town.set_fire_wall(i,j,false);
				}
				if (univ.town.is_antimagic(i,j)) {
					r1 = get_ran(1,1,8);
					if (r1 == 2)
						univ.town.set_antimagic(i,j,false);
				}
				if (univ.town.is_scloud(i,j)) {
					r1 = get_ran(1,1,4);
					if (r1 == 2)
						univ.town.set_scloud(i,j,false);
					else {
						scloud_space(i,j);
					}
				}
				if (univ.town.is_sleep_cloud(i,j)) {
					r1 = get_ran(1,1,4);
					if (r1 == 2)
						univ.town.set_sleep_cloud(i,j,false);
					else {
						sleep_cloud_space(i,j);
					}
				}
				if (univ.town.is_ice_wall(i,j)) {
					loc.x = i; loc.y = j;
					r1 = get_ran(3,1,6);
					hit_pcs_in_space(loc,r1,DAMAGE_COLD,1,1);
					r1 = get_ran(1,1,6);
					if (r1 == 1)
						univ.town.set_ice_wall(i,j,false);
				}
				if (univ.town.is_blade_wall(i,j)) {
					loc.x = i; loc.y = j;
					r1 = get_ran(6,1,8);
					hit_pcs_in_space(loc,r1,DAMAGE_WEAPON,1,1);
					r1 = get_ran(1,1,5);
					if (r1 == 1)
						univ.town.set_blade_wall(i,j,false);
				}
				if(univ.town.is_force_cage(i,j)) {
					loc.x = i; loc.y = j;
					short m = monst_there(loc);
					if(m == 90) {
						short pc = pc_there(loc);
						if(pc == 6) process_force_cage(loc, -1);
						else process_force_cage(loc, pc);
					} else process_force_cage(loc, 100 + m);
				}
			}
	
	processing_fields = false;
	monsters_going = true; // this changes who the damage is considered to come from in hit_space
	
	if(univ.town.quickfire_present) {
		for (i = 0; i < univ.town->max_dim(); i++)
			for (j = 0; j < univ.town->max_dim(); j++)
				if (univ.town.is_quickfire(i,j)) {
					loc.x = i; loc.y = j;
					r1 = get_ran(2,1,8);
					hit_pcs_in_space(loc,r1,DAMAGE_FIRE,1,1);
				}
	}
	
	monsters_going = false;
}

void scloud_space(short m,short n)
{
	location target;
	//cPopulation::cCreature;
	short i;
	
	target.x = (char) m;
	target.y = (char) n;
	
	univ.town.set_scloud(m,n,true);
	
	if (overall_mode >= MODE_COMBAT)
		for (i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if (univ.party[i].combat_pos == target) {
					curse_pc(i,get_ran(1,1,2));
				}
	if (overall_mode < MODE_COMBAT)
		if (target == univ.town.p_loc) {
			for (i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE)
					curse_pc(i,get_ran(1,1,2));
		}
}

void web_space(short m,short n)
{
	location target;
	short i;
	
	target.x = (char) m;
	target.y = (char) n;
	
	univ.town.set_web(m,n,true);
	
	if (overall_mode >= MODE_COMBAT)
		for (i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if (univ.party[i].combat_pos == target) {
					web_pc(i,3);
				}
	if (overall_mode < MODE_COMBAT)
		if (target == univ.town.p_loc) {
			for (i = 0; i < 6; i++)
				web_pc(i,3);
		}
}
void sleep_cloud_space(short m,short n)
{
	location target;
	short i;
	
	target.x = (char) m;
	target.y = (char) n;
	
	univ.town.set_sleep_cloud(m,n,true);
	
	if (overall_mode >= MODE_COMBAT)
		for (i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if (univ.party[i].combat_pos == target) {
					sleep_pc(i,3,eStatus::ASLEEP,0);
				}
	if (overall_mode < MODE_COMBAT)
		if (target == univ.town.p_loc) {
			for (i = 0; i < 6; i++)
				sleep_pc(i,3,eStatus::ASLEEP,0);
		}
}


void take_m_ap(short num,cCreature *monst)
{
	monst->ap = max(0,monst->ap - num);
}

void add_new_action(short pc_num)
{
	if (pc_num < 6)
		univ.party[pc_num].ap++;
}

short get_monst_sound(cCreature *attacker,short which_att) {
	short type,strength;
	
	type = attacker->a[which_att].type;
	strength = attacker->a[which_att];
	
	switch (type) {
		case 3:
			return 11;
			break;
		case 4:
			return 4;
			break;
		case 1:
			return 9;
			break;
		case 2:
			return 10;
			break;
			
		default:
			if(attacker->m_type == eRace::HUMAN) {
				if (strength > 9)
					return 3;
				else return 2;
			}
			if(attacker->m_type == eRace::HUMAN || attacker->m_type == eRace::IMPORTANT || attacker->m_type == eRace::GIANT) {
				return 2;
			}
			if(attacker->m_type == eRace::MAGE)
				return 1;
			if(attacker->m_type == eRace::PRIEST)
				return 4;
			return 0;
			break;
	}
	return 0;
}
