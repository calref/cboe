
#include <cstdio>

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
#include "soundtool.hpp"
#include "boe.town.h"
#include "boe.specials.h"
#include "boe.graphutil.h"
#include "boe.main.h"
#include "mathutil.hpp"
#include "dlogutil.hpp"
#include "boe.menus.h"
#include "spell.hpp"

extern eGameMode overall_mode;
extern bool ghost_mode;
extern short which_combat_type;
extern short stat_window;
extern location center;
extern short current_pc;
extern short combat_active_pc;
extern bool monsters_going,spell_forced;
extern bool flushingInput;
extern sf::RenderWindow mainPtr;
extern eSpell store_mage, store_priest;
extern short store_mage_lev, store_priest_lev,store_item_spell_level;
extern short store_spell_target,pc_casting,current_spell_range;
extern effect_pat_type current_pat;
extern short num_targets_left;
extern location spell_targets[8];
extern bool in_scen_debug;
extern short fast_bang;
extern short store_current_pc;
extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x
extern short spell_caster, missile_firer,current_monst_tactic;
eSpell spell_being_cast;
bool spell_freebie;
short missile_inv_slot, ammo_inv_slot;
short force_wall_position = 10; //  10 -> no force wall
bool processing_fields = true;
short futzing;
mon_num_t store_sum_monst;
short store_sum_monst_cost;
extern cUniverse univ;

location out_start_loc(20,23);
short hit_chance[51] = {
	20,30,40,45,50,55,60,65,69,73,
	77,81,84,87,90,92,94,96,97,98,99
	,99,99,99,99,99,99,99,99,99,99
	,99,99,99,99,99,99,99,99,99,99,
	99,99,99,99,99,99,99,99,99,99};

extern short boom_gr[8];

const char *d_string[] = {"North", "NorthEast", "East", "SouthEast", "South", "SouthWest", "West", "NorthWest"};

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
effect_pat_type radius2 = {{
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,1,1,1,1,1,0,0},
	{0,0,0,1,1,1,0,0,0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}}};
effect_pat_type radius3 = {{
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





void start_outdoor_combat(cOutdoors::cCreature encounter,ter_num_t in_which_terrain,short num_walls) {
	short i,j,how_many,num_tries = 0;
	short low[10] = {15,7,4,3,2,1,1,7,2,1};
	short high[10] = {30,10,6,5,3,2,1,10,4,1};
	rectangle town_rect(0,0,47,47);
	short nums[10];
	
	for(i = 0; i < 7; i++)
		nums[i] = get_ran(1,low[i],high[i]);
	for(i = 0; i < 3; i++)
		nums[i + 7] = get_ran(1,low[i + 7],high[i + 7]);
	notify_out_combat_began(encounter.what_monst,nums);
	print_buf();
	play_sound(23);
	
	mainPtr.setActive();
	which_combat_type = 0;
	overall_mode = MODE_COMBAT;
	
	// Basically, in outdoor combat, we create kind of a 48x48 town for
	// the combat to take place in
	for(i = 0; i < 48; i++)
		for(j = 0; j < 48; j++) {
			univ.town.fields[i][j] = 0;
		}
	univ.town.prep_arena();
	univ.town->in_town_rect = town_rect;
	
	create_out_combat_terrain((short) in_which_terrain,num_walls,0);////
	
	for(i = 0; i < univ.town->max_monst(); i++) {
		univ.town.monst[i].number = 0;
		univ.town.monst[i].active = 0;
	}
	for(i = 0; i < 7; i++) {
		how_many = nums[i];
		if(encounter.what_monst.monst[i] != 0)
			for(j = 0; j < how_many; j++)
				set_up_monst(0,encounter.what_monst.monst[i]);
	}
	for(i = 0; i < 3; i++) {
		how_many = nums[i + 7];
		if(encounter.what_monst.friendly[i] != 0)
			for(j = 0; j < how_many; j++)
				set_up_monst(1,encounter.what_monst.friendly[i]);
	}
	
	// place PCs
	univ.party[0].combat_pos = out_start_loc;
	update_explored(univ.party[0].combat_pos);
	if(get_blockage(univ.town->terrain(univ.party[0].combat_pos.x,univ.party[0].combat_pos.y)) > 0)
		univ.town->terrain(univ.party[0].combat_pos.x,univ.party[0].combat_pos.y) = univ.town->terrain(0,0);
	for(i = 1; i < 6; i++) {
		univ.party[i].combat_pos = univ.party[0].combat_pos;
		univ.party[i].combat_pos.x = univ.party[i].combat_pos.x + hor_vert_place[i].x;
		univ.party[i].combat_pos.y = univ.party[i].combat_pos.y + hor_vert_place[i].y;
		if(get_blockage(univ.town->terrain(univ.party[i].combat_pos.x,univ.party[i].combat_pos.y)) > 0)
			univ.town->terrain(univ.party[i].combat_pos.x,univ.party[i].combat_pos.y) = univ.town->terrain(0,0);
		update_explored(univ.party[i].combat_pos);
		
		univ.party[i].status[eStatus::POISONED_WEAPON] = 0;
		univ.party[i].status[eStatus::BLESS_CURSE] = 0;
		univ.party[i].status[eStatus::HASTE_SLOW] = 0;
		univ.party[i].status[eStatus::INVULNERABLE] = 0;
		univ.party[i].status[eStatus::MAGIC_RESISTANCE] = 0;
	}
	
	// place monsters, w. friendly monsts landing near PCs
	for(i = 0; i < univ.town->max_monst(); i++)
		if(univ.town.monst[i].active > 0) {
			univ.town.monst[i].target = 6;
			
			univ.town.monst[i].cur_loc.x  = get_ran(1,15,25);
			univ.town.monst[i].cur_loc.y  = get_ran(1,14,18);
			if(univ.town.monst[i].attitude == 2)
				univ.town.monst[i].cur_loc.y += 9;
			else if((univ.town.monst[i].mu > 0) || (univ.town.monst[i].cl > 0))
				univ.town.monst[i].cur_loc.y -= 4;//max(12,univ.town.monst[i].m_loc.y - 4);
			num_tries = 0;
			while((!monst_can_be_there(univ.town.monst[i].cur_loc,i) ||
					univ.town->terrain(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y) == 180 ||
					(pc_there(univ.town.monst[i].cur_loc) < 6)) &&
				   (num_tries++ < 50)) {
				univ.town.monst[i].cur_loc.x = get_ran(1,15,25);
				univ.town.monst[i].cur_loc.y = get_ran(1,14,18);
				if(univ.town.monst[i].attitude == 2)
					univ.town.monst[i].cur_loc.y += 9;
				else if((univ.town.monst[i].mu > 0) || (univ.town.monst[i].cl > 0))
					univ.town.monst[i].cur_loc.y -= 4;//max(12,univ.town.monst[i].m_loc.y - 4);
			}
			if(get_blockage(univ.town->terrain(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y)) > 0)
				univ.town->terrain(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y) = univ.town->terrain(0,0);
		}
	
	
	combat_active_pc = 6;
	spell_caster = 6; missile_firer = 6;
	for(i = 0; i < univ.town->max_monst(); i++)
		univ.town.monst[i].target = 6;
	
	for(i = 0; i < 6; i++) {
		univ.party[i].parry = 0;
		univ.party[i].last_attacked = univ.town->max_monst() + 10;
	}
	
	for(i = 0; i < NUM_TOWN_ITEMS; i++)
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

bool pc_combat_move(location destination) {
	std::string create_line;
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
	if(check_f)
		forced = true;
	
	if(in_scen_debug && ghost_mode) forced = true;
	
	if(keep_going) {
		
		dir = set_direction(univ.party[current_pc].combat_pos, destination);
		
		if((loc_off_act_area(destination)) && (which_combat_type == 1)) {
			add_string_to_buf("Move: Can't leave town during combat.");
			print_buf();
			return true;
		}
		else if(univ.town->terrain(destination.x,destination.y) == 90 && which_combat_type == 0) {
			if(get_ran(1,1,10) < 3) {
				univ.party[current_pc].main_status = eMainStatus::FLED;
				if(combat_active_pc == current_pc)
					combat_active_pc = 6;
				create_line = "Moved: Fled.";
				univ.party[current_pc].ap = 0;
			}
			else {
				take_ap(1);
				create_line = "Moved: Couldn't flee.";
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
				std::string result = cChoiceDlog("attack-friendly",{"cancel","attack"}).show();
				if(result == "cancel") s2 = 1;
				else if(result == "attack") s2 = 2;
			}
			if((s2 == 2) && (s1 % 2 != 1))
				make_town_hostile();
			if(s2 == 2) {
				univ.party[current_pc].last_attacked = monst_hit;
				pc_attack(current_pc,monst_hit);
				return true;
			}
		}
		else if((switch_pc = pc_there(destination)) < 6) {
			if(univ.party[switch_pc].ap == 0) {
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
			move_sound(univ.town->terrain(destination.x,destination.y),univ.party[current_pc].ap);
			return true;
		}
		else if(forced || (!impassable(univ.town->terrain(destination.x,destination.y)) && pc_there(destination) == 6)) {
			
			// monsters get back-shots
			for(i = 0; i < univ.town->max_monst(); i++) {
				monst_loc = univ.town.monst[i].cur_loc;
				monst_exist = univ.town.monst[i].active;
				
				s1 = current_pc;
				if((monst_exist > 0) && (monst_adjacent(univ.party[current_pc].combat_pos,i))
					&& !monst_adjacent(destination,i) &&
					(univ.town.monst[i].attitude % 2 == 1) &&
					univ.town.monst[i].status[eStatus::ASLEEP] <= 0 &&
					univ.town.monst[i].status[eStatus::PARALYZED] <= 0) {
					combat_posing_monster = current_working_monster = 100 + i;
					monster_attack_pc(i,current_pc);
					combat_posing_monster = current_working_monster = -1;
					draw_terrain(0);
				}
				if(s1 != current_pc)
					return true;
			}
			
			// move if still alive
			if(univ.party[current_pc].main_status == eMainStatus::ALIVE) {
				univ.party[current_pc].dir = set_direction(univ.party[current_pc].combat_pos,destination);
				univ.party[current_pc].combat_pos = destination;
				univ.party[current_pc].direction = dir;
				take_ap(1);
				create_line += "Moved: ";
				create_line += d_string[dir];
				add_string_to_buf(create_line);
				move_sound(univ.town->terrain(destination.x,destination.y),univ.party[current_pc].ap);
				
			}
			else return false;
			return true;
		}
		else {
			create_line += "Blocked: ";
			create_line += d_string[dir];
			add_string_to_buf(create_line);
			return false;
		}
	}
	return false;
}

void char_parry() {
	univ.party[current_pc].parry = (univ.party[current_pc].ap / 4) *
		(2 + stat_adj(current_pc,eSkill::DEXTERITY) + univ.party[current_pc].skills[eSkill::DEFENSE]);
	univ.party[current_pc].ap = 0;
}

void char_stand_ready() {
	univ.party[current_pc].parry = 100;
	univ.party[current_pc].ap = 0;
}

void pc_attack(short who_att,short target) {
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
	
	for(i = 0; i < 24; i++)
		if(((univ.party[who_att].items[i].variety == eItemType::ONE_HANDED) || (univ.party[who_att].items[i].variety == eItemType::TWO_HANDED)) &&
			univ.party[who_att].equip[i]) {
			if(weap1 == 24)
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
	
	
	if((skill_item = pc_has_abil_equip(who_att,eItemAbil::SKILL)) < 24) {
		hit_adj += 5 * (univ.party[who_att].items[skill_item].item_level / 2 + 1);
		dam_adj += univ.party[who_att].items[skill_item].item_level / 2;
	}
	if((skill_item = pc_has_abil_equip(who_att,eItemAbil::GIANT_STRENGTH)) < 24) {
		dam_adj += univ.party[who_att].items[skill_item].item_level;
		hit_adj += univ.party[who_att].items[skill_item].item_level * 2;
	}
	
	void_sanctuary(who_att);
	
	store_hp = univ.town.monst[target].health;
	
	combat_posing_monster = current_working_monster = who_att;
	
	if(weap1 == 24) {
		std::string create_line = univ.party[who_att].name + " punches.";
		add_string_to_buf(create_line);
		
		r1 = get_ran(1,1,100) + hit_adj - 20;
		r1 += 5 * (univ.party[current_pc].status[eStatus::WEBS] / 3);
		r2 = get_ran(1,1,4) + dam_adj;
		
		if(r1 <= hit_chance[univ.party[who_att].skills[what_skill1]]) {
			damage_monst(target, who_att, r2, 0,eDamageType::WEAPON,4);
		}
		else {
			draw_terrain(2);
			
			create_line = univ.party[who_att].name + " misses.";
			add_string_to_buf(create_line);
			play_sound(2);
		}
	}
	
	short weap_poisoned = univ.party[who_att].weap_poisoned;
	if(weap1 < 24)
		pc_attack_weapon(who_att, target, hit_adj, dam_adj, univ.party[who_att].items[weap1],weap2 < 24 ? 2 : 1,weap_poisoned == weap1);
	if(weap2 < 24 && which_m->active > 0)
		pc_attack_weapon(who_att, target, hit_adj, dam_adj, univ.party[who_att].items[weap2],0,weap_poisoned == weap2);
	move_to_zero(univ.party[who_att].status[eStatus::POISONED_WEAPON]);
	take_ap(4);
	
	if((univ.town.monst[target].status[eStatus::MARTYRS_SHIELD] > 0 || univ.town.monst[target].abil[eMonstAbil::MARTYRS_SHIELD].active)
	   && (store_hp - univ.town.monst[target].health > 0)) {
		add_string_to_buf("  Shares damage!   ");
		damage_pc(who_att, store_hp - univ.town.monst[target].health, eDamageType::MAGIC,eRace::UNKNOWN,0);
	}
	combat_posing_monster = current_working_monster = -1;
}

// primary: 0 - secondary weapon, 1 - primary (and only) weapon, 2 - primary of two weapons
void pc_attack_weapon(short who_att,short target,short hit_adj,short dam_adj,cItem& weap,short primary,bool do_poison) {
	short r1, r2;
	cCreature* which_m = &univ.town.monst[target];
	eSkill what_skill = weap.weap_type;
	
	// safety valve
	if(what_skill == eSkill::INVALID)
		what_skill = eSkill::EDGED_WEAPONS;
	
	std::string create_line = univ.party[who_att].name + " swings.";
	add_string_to_buf(create_line);
	r1 = get_ran(1,1,100) + hit_adj - 5 * weap.bonus;
	r1 += 5 * (univ.party[current_pc].status[eStatus::WEBS] / 3);
	if(primary) r1 -= 5;
	
	// Ambidextrous?
	if(primary != 1 && !univ.party[who_att].traits[eTrait::AMBIDEXTROUS])
		r1 += 25;
	
	if(primary) {
		// race adj.
		if(univ.party[who_att].race == eRace::SLITH && weap.weap_type == eSkill::POLE_WEAPONS)
			r1 -= 10;
	}
	
	r2 = get_ran(1,1,weap.item_level) + dam_adj + weap.bonus;
	if(primary) r2 += 2; else r2 -= 1;
	if(weap.ability == eItemAbil::WEAK_WEAPON)
		r2 = (r2 * (10 - weap.abil_data[0])) / 10;
	
	if(r1 <= hit_chance[univ.party[who_att].skills[what_skill]]) {
		eDamageType dmg_tp = eDamageType::UNBLOCKABLE;
		short spec_dam = calc_spec_dam(weap.ability,weap.abil_data[0],weap.abil_data[1],which_m,dmg_tp);
		short bonus_dam = 0;
		if(dmg_tp != eDamageType::UNBLOCKABLE)
			std::swap(spec_dam, bonus_dam);
		if(primary) {
			// assassinate
			r1 = get_ran(1,1,100);
			if((univ.party[who_att].level >= which_m->level - 1)
				&& univ.party[who_att].skills[eSkill::ASSASSINATION] >= which_m->level / 2
				&& (!which_m->abil[eMonstAbil::SPLITS].active)) // Can't assassinate splitters
				if(r1 < hit_chance[max(univ.party[who_att].skills[eSkill::ASSASSINATION] - which_m->level,0)]) {
					add_string_to_buf("  You assassinate.");
					spec_dam += r2;
				}
		}
		switch(what_skill) {
			case eSkill::EDGED_WEAPONS:
				if(weap.item_level < 8)
					damage_monst(target, who_att, r2, spec_dam, eDamageType::WEAPON,1);
				else damage_monst(target, who_att, r2, spec_dam, eDamageType::WEAPON,2);
				break;
			case eSkill::BASHING_WEAPONS:
				damage_monst(target, who_att, r2, spec_dam, eDamageType::WEAPON,4);
				break;
			case eSkill::POLE_WEAPONS:
				damage_monst(target, who_att, r2, spec_dam, eDamageType::WEAPON,3);
				break;
			default: // TODO: Not sure what sound to play for unconventional weapons, but let's just go with the generic "ouch" for now
				damage_monst(target, who_att, r2, spec_dam, eDamageType::WEAPON, 0);
				break;
		}
		if(bonus_dam)
			damage_monst(target, who_att, bonus_dam, 0, dmg_tp, 0);
		if(do_poison) {
			// poison
			if(univ.party[who_att].status[eStatus::POISONED_WEAPON] > 0) {
				short poison_amt = univ.party[who_att].status[eStatus::POISONED_WEAPON];
				if(pc_has_abil_equip(who_att,eItemAbil::POISON_AUGMENT) < 24)
					poison_amt += 2;
				poison_monst(which_m,poison_amt);
				move_to_zero(univ.party[who_att].status[eStatus::POISONED_WEAPON]);
			}
		}
		if((weap.ability == eItemAbil::STATUS_WEAPON) && (get_ran(1,0,1) == 1)) {
			switch(eStatus(weap.abil_data[1])) {
					// TODO: It should be possible to make monsters support magic resistance and invulnerability, at least.
					// Maybe also poisoned weapon and invisibility.
				case eStatus::MAIN: // Not a valid status
				case eStatus::INVISIBLE: // Not supported by monsters
				case eStatus::MAGIC_RESISTANCE: // Not supported by monsters
				case eStatus::INVULNERABLE: // Not supported by monsters
				case eStatus::POISONED_WEAPON: // Not supported by monsters
					break;
				case eStatus::POISON:
					add_string_to_buf("  Blade drips venom.");
					poison_monst(which_m,weap.abil_data[0] / 2);
					break;
				case eStatus::ACID:
					add_string_to_buf("  Blade drips acid.");
					acid_monst(which_m,weap.abil_data[0] / 2);
					break;
				case eStatus::BLESS_CURSE:
					add_string_to_buf("  Blade leaks a dark aura.");
					curse_monst(which_m, weap.abil_data[0] / 2);
					break;
				case eStatus::HASTE_SLOW:
					add_string_to_buf("  Blade leaks a smoky aura.");
					slow_monst(which_m, weap.abil_data[0] / 2);
					break;
				case eStatus::WEBS:
					add_string_to_buf("  Blade drips goo.");
					web_monst(which_m, weap.abil_data[0] / 2);
					break;
				case eStatus::DISEASE:
					add_string_to_buf("  Blade drips bile.");
					disease_monst(which_m, weap.abil_data[0] / 2);
					break;
				case eStatus::DUMB:
					add_string_to_buf("  Blade leaks a misty aura.");
					dumbfound_monst(which_m, weap.abil_data[0] / 2);
					break;
				case eStatus::ASLEEP:
					add_string_to_buf("  Blade emits coruscating lights.");
					charm_monst(which_m, 20 + r2 + spec_dam, eStatus::ASLEEP, weap.abil_data[0] / 2);
					break;
				case eStatus::PARALYZED:
					add_string_to_buf("  Blade emits a purple flash.");
					charm_monst(which_m, 20 + r2 + spec_dam, eStatus::PARALYZED, weap.abil_data[0] / 2);
					break;
				case eStatus::CHARM:
					add_string_to_buf("  Blade leaks a bright aura.");
					// Higher penalty means more likely to resist.
					charm_monst(which_m, 20 + r2 + spec_dam - weap.abil_data[0] / 2, eStatus::CHARM, 0);
					break;
				case eStatus::FORCECAGE:
					add_string_to_buf("  Blade emits a green flash.");
					charm_monst(which_m, r2 + spec_dam - weap.abil_data[0] / 2, eStatus::FORCECAGE, 0);
					break;
				case eStatus::MARTYRS_SHIELD:
					add_string_to_buf("  Blade leaks an odd-coloured aura.");
					which_m->status[eStatus::MARTYRS_SHIELD] -= weap.abil_data[0] / 2;
					// TODO: Maybe clip it to 0?
					break;
			}
		}
		if((weap.ability == eItemAbil::SOULSUCKER) && (get_ran(1,0,1) == 1)) {
			add_string_to_buf("  Blade drains life.");
			heal_pc(who_att,weap.abil_data[0] / 2);
		}
		
	}
	else {
		draw_terrain(2);
		create_line = "  " + univ.party[who_att].name + " misses.";
		add_string_to_buf(create_line);
		if(what_skill == eSkill::POLE_WEAPONS)
			play_sound(19);
		else play_sound(2);
	}
}


short calc_spec_dam(eItemAbil abil,short abil_str,short abil_dat,cCreature* monst,eDamageType& dmg_type) {
	short store = 0;
	
	switch(abil) {
		case eItemAbil::DAMAGING_WEAPON:
		case eItemAbil::MISSILE_LIGHTNING:
			store += get_ran(abil_str,1,6);
			if(abil == eItemAbil::DAMAGING_WEAPON)
				dmg_type = eDamageType(abil_dat);
			else dmg_type = eDamageType::FIRE;
			break;
		case eItemAbil::SLAYER_WEAPON:
		case eItemAbil::MISSILE_SLAYER:
			if(monst->m_type != eRace(abil_dat))
				break;
			store += abil_str;
			switch(eRace(abil_dat)) {
				case eRace::DEMON:
				case eRace::GIANT:
					store *= 8;
					break;
				case eRace::UNDEAD:
					store *= 6;
					break;
				case eRace::REPTILE:
					store *= 5;
					break;
				case eRace::MAGE:
				case eRace::PRIEST:
					store *= 4;
					break;
				case eRace::BUG:
				case eRace::PLANT:
					store *= 7;
					break;
			}
			if(abil == eItemAbil::MISSILE_SLAYER) {
				if(eRace(abil_dat) == eRace::DEMON)
					store += 25;
				else if(eRace(abil_dat) == eRace::UNDEAD)
					store += 20;
				else store += 10;
			}
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

void place_target(location target) {
	short i;
	
 	if(num_targets_left > 0) {
		if(loc_off_act_area(target)) {
			add_string_to_buf("  Space not in town.           ");
			return;
		}
		if(can_see_light(univ.party[current_pc].combat_pos,target,sight_obscurity) > 4) {
			add_string_to_buf("  Can't see target.           ");
			return;
		}
		if(dist(univ.party[current_pc].combat_pos,target) > (*spell_being_cast).range)  {
			add_string_to_buf("  Target out of range.");
			return;
		}
		if(sight_obscurity(target.x,target.y) == 5 && spell_being_cast != eSpell::DISPEL_BARRIER) {
			add_string_to_buf("  Target space obstructed.           ");
			return;
		}
		if(univ.town.is_antimagic(target.x,target.y)) {
			add_string_to_buf("  Target in antimagic field.");
			return;
		}
		for(i = 0; i < 8; i++) {
			if(spell_targets[i] == target) {
				add_string_to_buf("  Target removed.");
				num_targets_left++;
				spell_targets[i].x = 120;
				play_sound(-1);
				return;
			}
		}
		for(i = 0; i < 8; i++)
			if(spell_targets[i].x == 120) {
				add_string_to_buf("  Target added.");
				spell_targets[i] = target;
				num_targets_left--;
				play_sound(0);
				i = 8;
			}
	}
	
	if(num_targets_left == 0) {
		do_combat_cast(spell_targets[0]);
		overall_mode = MODE_COMBAT;
	}
}

void do_combat_cast(location target) {
	short adjust,r1,r2,targ_num,level,bonus = 1,i,item;
	snd_num_t store_sound = 0;
	cCreature *cur_monst;
	bool freebie = false,ap_taken = false,cost_taken = false;
	short num_targets = 1;
	miss_num_t store_m_type = 2;
	eFieldType spray_type_array[15] = {
		FIELD_WEB,FIELD_WEB,FIELD_WEB,
		WALL_FORCE,WALL_FORCE,
		WALL_FIRE,WALL_FIRE,WALL_FIRE,
		FIELD_ANTIMAGIC,FIELD_ANTIMAGIC,
		CLOUD_STINK,CLOUD_STINK,
		WALL_ICE,WALL_ICE,WALL_BLADES,
	};
	mon_num_t summon;
	
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
	
	// TODO: Should we do this here? Or in the handling of targeting modes?
	// (It really depends whether we want to be able to trigger it for targeting something other than a spell.)
	if(adjust <= 4 && !cast_spell_on_space(target, spell_being_cast))
		return; // The special node intercepted and cancelled regular spell behaviour.
	
	void_sanctuary(current_pc);
	if(overall_mode == MODE_SPELL_TARGET) {
		spell_targets[0] = target;
	}
	else {
		num_targets = 8;
	}
	
	spell_caster = current_pc;
	
	// assign monster summoned, if summoning
	if(spell_being_cast == eSpell::SUMMON_BEAST) {
		summon = get_summon_monster(1);
		
	} else if(spell_being_cast == eSpell::SUMMON_WEAK) {
		summon = get_summon_monster(1);
	} else if(spell_being_cast == eSpell::SUMMON) {
		summon = get_summon_monster(2);
	} else if(spell_being_cast == eSpell::SUMMON_MAJOR) {
		summon = get_summon_monster(3);
	}
	combat_posing_monster = current_working_monster = current_pc;
	
	for(i = 0; i < num_targets; i++)
		if(spell_targets[i].x != 120) {
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
			
			if((adjust = can_see_light(univ.party[current_pc].combat_pos,target,sight_obscurity)) > 4) {
				add_string_to_buf("  Can't see target.           ");
			}
			else if(loc_off_act_area(target)) {
				add_string_to_buf("  Space not in town.           ");
			}
			else if(dist(univ.party[current_pc].combat_pos,target) > (*spell_being_cast).range)
				add_string_to_buf("  Target out of range.");
			else if(sight_obscurity(target.x,target.y) == 5 && spell_being_cast != eSpell::DISPEL_BARRIER)
				add_string_to_buf("  Target space obstructed.           ");
			else if(univ.town.is_antimagic(target.x,target.y))
				add_string_to_buf("  Target in antimagic field.");
			else {
				if(!ap_taken) {
					if(!freebie)
						take_ap(5);
					ap_taken = true;
					draw_terrain(2);
				}
				boom_targ[i] = target;
				switch(spell_being_cast) {
						
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
						hit_space(target,r1,eDamageType::FIRE,true,true);
						univ.town.set_fire_barr(target.x,target.y,true);
						if(univ.town.is_fire_barr(target.x,target.y))
							add_string_to_buf("  You create the barrier.              ");
						else add_string_to_buf("  Failed.");
						break;
					case eSpell::BARRIER_FORCE:
						play_sound(68);
						r1 = get_ran(7,2,7);
						hit_space(target,r1,eDamageType::FIRE,true,true);
						univ.town.set_force_barr(target.x,target.y,true);
						if(univ.town.is_force_barr(target.x,target.y))
							add_string_to_buf("  You create the barrier.              ");
						else add_string_to_buf("  Failed.");
						break;
						
					default: // spells which involve animations
						start_missile_anim();
						switch(spell_being_cast) {
								
							case eSpell::DIVINE_THUD:
								add_missile(target,9,1,0,0);
								store_sound = 11;
								r1 = min(18,(level * 7) / 10 + 2 * bonus);
								place_spell_pattern(radius2,target,eDamageType::MAGIC,r1,current_pc);
								ashes_loc = target;
								break;
								
							case eSpell::SPARK: case eSpell::ICE_BOLT:
								r1 = (spell_being_cast == eSpell::SPARK) ? get_ran(2,1,4) : get_ran(min(20,level + bonus),1,4);
								add_missile(target,6,1,0,0);
								do_missile_anim(100,univ.party[current_pc].combat_pos,11);
								hit_space(target,r1,(spell_being_cast == eSpell::SPARK) ? eDamageType::MAGIC : eDamageType::COLD,1,0);
								break;
							case eSpell::ARROWS_FLAME:
								add_missile(target,4,1,0,0);
								r1 = get_ran(2,1,4);
								boom_type[i] = eDamageType::FIRE;
								boom_dam[i] = r1;
								//hit_space(target,r1,1,1,0);
								break;
							case eSpell::SMITE:
								add_missile(target,6,1,0,0);
								r1 = get_ran(2,1,5);
								boom_type[i] = eDamageType::COLD;
								boom_dam[i] = r1;
								//hit_space(target,r1,5,1,0);
								break;
							case eSpell::WOUND:
							case eSpell::WRACK:
								if(spell_being_cast == eSpell::WRACK)
									r1 = get_ran(2 + bonus / 2,1,4);
								else r1 = get_ran(min(7,2 + bonus + level / 2),1,4);
								add_missile(target,14,1,0,0);
								do_missile_anim(100,univ.party[current_pc].combat_pos,24);
								hit_space(target,r1,eDamageType::UNBLOCKABLE,1,0);
								break;
							case eSpell::FLAME:
								r1 = get_ran(min(10,1 + level / 3 + bonus),1,6);
								add_missile(target,2,1,0,0);
								do_missile_anim(100,univ.party[current_pc].combat_pos,11);
								hit_space(target,r1,eDamageType::FIRE,1,0);
								break;
							case eSpell::FIREBALL: case eSpell::FLAMESTRIKE:
								r1 = min(9,1 + (level * 2) / 3 + bonus) + 1;
								add_missile(target,2,1,0,0);
								store_sound = 11;
								//do_missile_anim(100,univ.party[current_pc].combat_pos,11);
								if(spell_being_cast == eSpell::FLAMESTRIKE)
									r1 = (r1 * 14) / 10;
								else if(r1 > 10) r1 = (r1 * 8) / 10;
								if(r1 <= 0) r1 = 1;
								place_spell_pattern(square,target,eDamageType::FIRE,r1,current_pc);
								ashes_loc = target;
								break;
							case eSpell::FIRESTORM:
								add_missile(target,2,1,0,0);
								store_sound = 11;
								//do_missile_anim(100,univ.party[current_pc].combat_pos,11);
								r1 = min(12,1 + (level * 2) / 3 + bonus) + 2;
								if(r1 > 20)
									r1 = (r1 * 8) / 10;
								place_spell_pattern(radius2,target,eDamageType::FIRE,r1,current_pc);
								ashes_loc = target;
								break;
							case eSpell::KILL:
								add_missile(target,9,1,0,0);
								do_missile_anim(100,univ.party[current_pc].combat_pos,11);
								r1 = get_ran(3,0,10) + univ.party[current_pc].level * 2;
								hit_space(target,40 + r1,eDamageType::MAGIC,1,0);
								break;
							case eSpell::ARROWS_DEATH:
								add_missile(target,9,1,0,0);
								store_sound = 11;
								r1 = get_ran(3,0,10) + univ.party[current_pc].level + 3 * bonus;
								boom_type[i] = eDamageType::MAGIC;
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
								switch(spell_being_cast) {
									case eSpell::SIMULACRUM:
										r2 = get_ran(3,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if(!summon_monster(store_sum_monst,target,r2,2))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_BEAST:
										r2 = get_ran(3,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if((summon == 0) || (!summon_monster(summon,target,r2,2)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_WEAK:
										r2 = get_ran(4,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if((summon == 0) || (!summon_monster(summon,target,r2,2)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON:
										r2 = get_ran(5,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if((summon == 0) || (!summon_monster(summon,target,r2,2)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_MAJOR:
										r2 = get_ran(7,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if((summon == 0) || (!summon_monster(summon,target,r2,2)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::DEMON:
										r2 = get_ran(5,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if(!summon_monster(85,target,r2,2))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_RAT:
										r1 = get_ran(3,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if(!summon_monster(80,target,r1,2))
											add_string_to_buf("  Summon failed.");
										break;
										
									case eSpell::SUMMON_SPIRIT:
										r2 = get_ran(2,1,5) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if(!summon_monster(125,target,r2,2))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::STICKS_TO_SNAKES:
										r1 = get_ran(1,0,7);
										r2 = get_ran(2,1,5) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if(!summon_monster((r1 == 1) ? 100 : 99,target,r2,2))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_HOST: // host
										r2 = get_ran(2,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if(!summon_monster((i == 0) ? 126 : 125,target,r2,2))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_GUARDIAN: // guardian
										r2 = get_ran(6,1,4) + stat_adj(current_pc,eSkill::INTELLIGENCE);
										if(!summon_monster(122,target,r2,2))
											add_string_to_buf("  Summon failed.");
										break;
								}
								break;
								
								
							default:
								targ_num = monst_there(target);
								if(targ_num > univ.town->max_monst())
									add_string_to_buf("  Nobody there                 ");
								else {
									cur_monst = &univ.town.monst[targ_num];
									cCreature* cur_monst = &univ.town.monst[targ_num];
									if(cur_monst->attitude % 2 != 1 && spell_being_cast != eSpell::SCRY_MONSTER && spell_being_cast != eSpell::CAPTURE_SOUL)
										make_town_hostile();
									switch(spell_being_cast) {
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
										case eSpell::UNHOLY_RAVAGING:
											store_m_type = 14;
											store_sound = 53;
											r1 = get_ran(4,1,8);
											r2 = get_ran(1,0,2);
											damage_monst(targ_num, 7, r1, 0, eDamageType::MAGIC,0);
											slow_monst(cur_monst, 4 + r2);
											poison_monst(cur_monst, 5 + r2);
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
											if((cur_monst->mu == 0) && (cur_monst->cl == 0))
												add_string_to_buf("  Can't duel: no magic.");
											else {
												item = pc_has_abil(current_pc,eItemAbil::SMOKY_CRYSTAL);
												if(item >= 24)
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
											if(cur_monst->m_type != eRace::UNDEAD) {
												add_string_to_buf("  Not undead.                    ");
												store_m_type = -1;
												break;
											}
											store_m_type = 8;
											r1 = get_ran(1,0,90);
											if(r1 > hit_chance[minmax(0,19,bonus * 2 + level * 4 - (cur_monst->level / 2) + 3)])
												add_string_to_buf("  Monster resisted.                  ");
											else {
												r1 = get_ran((spell_being_cast == eSpell::TURN_UNDEAD) ? 2 : 6, 1, 14);
												
												hit_space(cur_monst->cur_loc,r1,eDamageType::UNBLOCKABLE,0,current_pc);
											}
											store_sound = 24;
											break;
											
										case eSpell::RAVAGE_SPIRIT:
											if(cur_monst->m_type != eRace::DEMON) {
												add_string_to_buf("  Not a demon.                    ");
												store_m_type = -1;
												break;
											}
											r1 = get_ran(1,1,100);
											if(r1 > hit_chance[minmax(0,19,level * 4 - cur_monst->level + 10)])
												add_string_to_buf("  Demon resisted.                  ");
											else {
												r1 = get_ran(8 + bonus * 2, 1, 11);
												//if(PSD[4][0] == 3) // anama
												//	r1 += 25;
												//play_sound(53);
												hit_space(cur_monst->cur_loc,r1,eDamageType::UNBLOCKABLE,0,current_pc);
											}
											store_sound = 24;
											break;
									}
									if(store_m_type >= 0)
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
	for(i = 0; i < 8; i++)
		if(boom_dam[i] > 0)
			hit_space(boom_targ[i],boom_dam[i],boom_type[i],1,0);
	
	if(ashes_loc.x > 0)
		univ.town.set_ash(ashes_loc.x,ashes_loc.y,true);
	
	do_explosion_anim(5,0);
	
	end_missile_anim();
	
	handle_marked_damage();
	combat_posing_monster = current_working_monster = -1;
	
	print_buf();
}

void handle_marked_damage() {
	short i;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].marked_damage > 0) {
			// TODO: Perhaps there should be a way of determining the correct race here?
			damage_pc(i,univ.party[i].marked_damage,eDamageType::MARKED,eRace::UNKNOWN,0);
			univ.party[i].marked_damage = 0;
		}
	for(i = 0; i < univ.town->max_monst(); i++)
		if(univ.town.monst[i].marked_damage > 0) {
			damage_monst(i, current_pc, univ.town.monst[i].marked_damage, 0, eDamageType::MARKED,0);
			
			univ.town.monst[i].marked_damage = 0;
		}
}

void load_missile() {
	short i,bow = 24,arrow = 24,thrown = 24,crossbow = 24,bolts = 24,no_ammo = 24;
	
	for(i = 0; i < 24; i++) {
		if((univ.party[current_pc].equip[i]) &&
			(univ.party[current_pc].items[i].variety == eItemType::THROWN_MISSILE))
			thrown = i;
		if((univ.party[current_pc].equip[i]) &&
			(univ.party[current_pc].items[i].variety == eItemType::BOW))
			bow = i;
		if((univ.party[current_pc].equip[i]) &&
			(univ.party[current_pc].items[i].variety == eItemType::ARROW))
			arrow = i;
		if((univ.party[current_pc].equip[i]) &&
			(univ.party[current_pc].items[i].variety == eItemType::CROSSBOW))
			crossbow = i;
		if((univ.party[current_pc].equip[i]) &&
			(univ.party[current_pc].items[i].variety == eItemType::BOLTS))
			bolts = i;
		if((univ.party[current_pc].equip[i]) &&
			(univ.party[current_pc].items[i].variety == eItemType::MISSILE_NO_AMMO))
			no_ammo = i;
	}
	
	if(thrown < 24) {
		ammo_inv_slot = thrown;
		add_string_to_buf("Throw: Select a target.        ");
		add_string_to_buf("  (Hit 's' to cancel.)");
		overall_mode = MODE_THROWING;
		current_spell_range = 8;
		current_pat = single;
	}
	else if(((bolts < 24) && (bow < 24)) || ((arrow < 24) && (crossbow < 24)))  {
		add_string_to_buf("Fire: Wrong ammunition.       ");
	}
	else if((arrow == 24) && (bow < 24)) {
		add_string_to_buf("Fire: Equip some arrows.       ");
	}
	else if(crossbow == 24 && bolts < 24) {
		add_string_to_buf("Fire: Equip some bolts.        ");
	}
	else if((arrow < 24) && (bow < 24)) {
		missile_inv_slot = bow;
		ammo_inv_slot = arrow;
		overall_mode = MODE_FIRING;
		add_string_to_buf("Fire: Select a target.        ");
		add_string_to_buf("  (Hit 's' to cancel.)");
		current_spell_range = 12;
		if(univ.party[current_pc].items[arrow].ability == eItemAbil::MISSILE_EXPLODING)
			current_pat = radius2;
		else
			current_pat = single;
	}
	else if((bolts < 24) && (crossbow < 24)) {
		missile_inv_slot = crossbow;
		ammo_inv_slot = bolts;
		overall_mode = MODE_FIRING;
		add_string_to_buf("Fire: Select a target.        ");
		add_string_to_buf("  (Hit 's' to cancel.)");
		current_spell_range = 12;
		current_pat = single;
	}
	else if(no_ammo < 24) {
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
	short skill_item;
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
	if((skill_item = pc_has_abil_equip(missile_firer,eItemAbil::ACCURACY)) < 24) {
		hit_bonus += univ.party[missile_firer].items[skill_item].abil_data[0] / 2;
		dam_bonus += univ.party[missile_firer].items[skill_item].abil_data[0] / 2;
	}
	
	// race adj.
	// TODO: Should this apply to sliths as well? The bladbase suggests otherwise, but it has been changed from the original; maybe the sliths were originally considered to be reptiles.
	if(univ.party[missile_firer].race == eRace::REPTILE)
		hit_bonus += 2;
	
	if(univ.party[missile_firer].items[ammo_inv_slot].ability == eItemAbil::MISSILE_EXPLODING)
		exploding = true;
	
	if(dist(univ.party[missile_firer].combat_pos,target) > range)
		add_string_to_buf("  Out of range.");
	else if(can_see_light(univ.party[missile_firer].combat_pos,target,sight_obscurity) >= 5)
		add_string_to_buf("  Can't see target.             ");
	else {
		// First, some missiles do special things
		if(exploding) {
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
			place_spell_pattern(radius2,target, eDamageType::FIRE,univ.party[missile_firer].items[ammo_inv_slot].abil_data[0] * 2,missile_firer);
			do_explosion_anim(5,0);
			end_missile_anim();
			handle_marked_damage();
		} else {
			combat_posing_monster = current_working_monster = missile_firer;
			draw_terrain(2);
			void_sanctuary(missile_firer);
			// TODO: Why is this sound commented out?
			//play_sound((overall_mode == MODE_FIRING) ? 12 : 14);
			take_ap((overall_mode == MODE_FIRING) ? 3 : 2);
			missile_firer = missile_firer;
			r1 = get_ran(1,1,100) - 5 * hit_bonus - 10;
			r1 += 5 * (univ.party[missile_firer].status[eStatus::WEBS] / 3);
			r2 = get_ran(1,1,dam) + dam_bonus;
			std::string create_line = univ.party[missile_firer].name + " fires.";
			add_string_to_buf(create_line);
			
			miss_num_t m_type = univ.party[missile_firer].items[ammo_inv_slot].missile;
			run_a_missile(univ.party[missile_firer].combat_pos,target,m_type,1,(overall_mode == MODE_FIRING) ? 12 : 14,
						  0,0,100);
			
			if(r1 > hit_chance[skill])
				add_string_to_buf("  Missed.");
			else if((targ_monst = monst_there(target)) < univ.town->max_monst()) {
				cur_monst = &univ.town.monst[targ_monst];
				eDamageType dmg_tp = eDamageType::UNBLOCKABLE;
				short bonus_dam = 0;
				cItem& missile = univ.party[missile_firer].items[ammo_inv_slot];
				spec_dam = calc_spec_dam(missile.ability,missile.abil_data[0],missile.abil_data[1],cur_monst,dmg_tp);
				if(dmg_tp != eDamageType::UNBLOCKABLE) std::swap(bonus_dam, spec_dam);
				if(univ.party[missile_firer].items[ammo_inv_slot].ability == eItemAbil::MISSILE_HEAL_TARGET) {
					ASB("  There is a flash of light.");
					cur_monst->health += r2;
				}
				else damage_monst(targ_monst, missile_firer, r2, spec_dam, eDamageType::WEAPON,13);
				if(bonus_dam) damage_monst(targ_monst, missile_firer, bonus_dam, 0, dmg_tp, 0);
				
				//if(univ.party[missile_firer].items[ammo_inv_slot].ability == 33)
				//	hit_space(cur_monst->m_loc,get_ran(3,1,6),1,1,1);
				
				// poison
				if(univ.party[missile_firer].status[eStatus::POISONED_WEAPON] > 0 && univ.party[missile_firer].weap_poisoned == ammo_inv_slot) {
					poison_amt = univ.party[missile_firer].status[eStatus::POISONED_WEAPON];
					if(pc_has_abil_equip(missile_firer,eItemAbil::POISON_AUGMENT) < 24)
						poison_amt++;
					poison_monst(cur_monst,poison_amt);
				}
			}
			else if((targ_monst = pc_there(target)) < 6 && univ.party[current_pc].items[ammo_inv_slot].ability == eItemAbil::MISSILE_HEAL_TARGET){
				ASB("  There is a flash of light.");
				heal_pc(targ_monst,r2);
			}
			else hit_space(target,r2,eDamageType::WEAPON,1,0);
			
		}
		
		if(univ.party[missile_firer].items[ammo_inv_slot].variety != eItemType::MISSILE_NO_AMMO) {
			if(univ.party[missile_firer].items[ammo_inv_slot].ability != eItemAbil::MISSILE_RETURNING)
				univ.party[missile_firer].items[ammo_inv_slot].charges--;
			else univ.party[missile_firer].items[ammo_inv_slot].charges = 1;
			if(pc_has_abil_equip(missile_firer,eItemAbil::DRAIN_MISSILES) < 24
			   && univ.party[missile_firer].items[ammo_inv_slot].ability != eItemAbil::MISSILE_RETURNING)
				univ.party[missile_firer].items[ammo_inv_slot].charges--;
			if(univ.party[missile_firer].items[ammo_inv_slot].charges <= 0)
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
bool combat_next_step() {
	bool to_return = false;
	short store_pc; // will print current pc name is active pc changes
	
	store_pc = current_pc;
	while(pick_next_pc()) {
		combat_run_monst();
		set_pc_moves();
		if((combat_active_pc < 6) && (univ.party[combat_active_pc].ap == 0)){
			combat_active_pc = 6;
			ASB("The active character is unable to act!");
			ASB("The whole party is now active.");
		}
		to_return = true;
		// Safety valve
		if(party_toast())
			return true;
	}
	pick_next_pc();
	if(current_pc != store_pc)
		to_return = true;
	// TODO: Windows version checks for overall_mode being MODE_OUTDOORS or MODE_TOWN - that's not necessary, right?
	center = univ.party[current_pc].combat_pos;
	
	adjust_spell_menus();
	
	// In case running monsters affected active PC...
/*	if(univ.party[current_pc].status[3] < 0)
		this_pc_hasted = false;
	if((univ.party[current_pc].main_status != 1) ||
		((univ.party[current_pc].status[3] < 0) && (univ.party.age % 2 == 0)))
		pick_next_pc();
	center = univ.party[current_pc].combat_pos;		*/
	
	if((combat_active_pc == 6) && (current_pc != store_pc)) {
		std::ostringstream create_line;
		create_line << "Active:  " << univ.party[current_pc].name;
		create_line << " (#" << current_pc + 1 << ", " << univ.party[current_pc].ap << " ap.)";
		add_string_to_buf(create_line.str());
		print_buf();
	}
	if((current_pc != store_pc) || (to_return)) {
		put_pc_screen();
		set_stat_window(current_pc);
	}
	return to_return;
}

// Find next active PC, return true is monsters need running, and run monsters is slow spells
// active
bool pick_next_pc() {
	bool store = false;
	
	if(current_pc == 6)
		current_pc = 0;
	
	// If current pc isn't active, fry its moves
	if((combat_active_pc < 6) && (combat_active_pc != current_pc))
		univ.party[current_pc].ap = 0;
	
	// Find next PC with moves
	while((univ.party[current_pc].ap <= 0) && (current_pc < 6)) {
		current_pc++;
		if((combat_active_pc < 6) && (combat_active_pc != current_pc))
			univ.party[current_pc].ap = 0;
	}
	
	// If run out of PC's, return to start and try again
	if(current_pc == 6) {
		current_pc = 0;
		while((univ.party[current_pc].ap <= 0) && (current_pc < 6)) {
			current_pc++;
			if((combat_active_pc < 6) && (combat_active_pc != current_pc))
				univ.party[current_pc].ap = 0;
		}
		if(current_pc == 6) {
			store = true;
			current_pc = 0;
		}
	}
	
	return store;
}


void combat_run_monst() {
	short i,item,item_level;
	bool update_stat = false;
	
	
	monsters_going = true;
	do_monster_turn();
	monsters_going = false;
	
	process_fields();
	move_to_zero(univ.party.light_level);
	if((which_combat_type == 1) && (univ.town->lighting_type == 2))
		univ.party.light_level = max (0,univ.party.light_level - 9);
	if(univ.town->lighting_type == 3)
		univ.party.light_level = 0;
	
	move_to_zero(univ.party.status[ePartyStatus::DETECT_LIFE]);
	move_to_zero(univ.party.status[ePartyStatus::FIREWALK]);
	
	// decrease monster present counter
	move_to_zero(PSD[SDF_HOSTILES_PRESENT]);
	
	dump_gold(1);
	
	univ.party.age++;
	if(univ.party.age % 4 == 0)
		for(i = 0; i < 6; i++) {
			if(univ.party[i].status[eStatus::BLESS_CURSE] != 0 || univ.party[i].status[eStatus::HASTE_SLOW] != 0)
				update_stat = true;
			move_to_zero(univ.party[i].status[eStatus::BLESS_CURSE]);
			move_to_zero(univ.party[i].status[eStatus::HASTE_SLOW]);
			move_to_zero(univ.party.status[ePartyStatus::STEALTH]);
			if((item = pc_has_abil_equip(i,eItemAbil::REGENERATE)) < 24) {
				update_stat = true;
				heal_pc(i,get_ran(1,0,univ.party[i].items[item].item_level + 1));
			}
		}
	for(i = 0; i < 6; i++)
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
			for(int j = 0; j < 24; j++) {
				cItem& item = univ.party[i].items[j];
				if(item.ability != eItemAbil::OCCASIONAL_STATUS) continue;
				if(item.magic_use_type > 1) continue; // Affects whole party, which is handled elsewhere
				if(get_ran(1,0,10) != 5) continue;
				int how_much = item.abil_data[0];
				if(item.magic_use_type % 2 == 1) how_much *= -1;
				eStatus status = eStatus(item.abil_data[1]);
				if(isStatusNegative(status))
					how_much *= -1;
				switch(status) {
					case eStatus::MAIN: break;
					case eStatus::HASTE_SLOW:
						if(how_much > 0) add_string_to_buf("An item hastes you!");
						else add_string_to_buf("An item slows you!");
						break;
					case eStatus::BLESS_CURSE:
						if(how_much > 0) add_string_to_buf("An item blesses you!");
						else add_string_to_buf("An item curses you!");
						break;
					case eStatus::POISON:
						if(how_much > 0) add_string_to_buf("An item poisons you!");
						else if(univ.party[i].status[eStatus::POISON] > 0)
							add_string_to_buf("An item cures you!");
						break;
					case eStatus::INVULNERABLE:
					case eStatus::MAGIC_RESISTANCE:
					case eStatus::INVISIBLE:
					case eStatus::MARTYRS_SHIELD:
						if(how_much > 0) add_string_to_buf("An item protects you!");
						// TODO: Message for negative amounts:
						break;
					case eStatus::DISEASE:
						if(how_much > 0) add_string_to_buf("An item diseases you!");
						else if(univ.party[i].status[eStatus::DISEASE] > 0)
							add_string_to_buf("An item cures you!");
						break;
					case eStatus::DUMB:
						if(how_much > 0) add_string_to_buf("An item clouds your mind!");
						else if(univ.party[i].status[eStatus::DUMB] > 0)
							add_string_to_buf("An item clears your mind!");
						break;
						// TODO: Messages for other statuses?
				}
				univ.party[i].apply_status(status, how_much);
				update_stat = true;
			}
		}
	
	special_increase_age();
	push_things();
	
	if(univ.party.age % 2 == 0)
		do_poison();
	if(univ.party.age % 3 == 0)
		handle_disease();
	handle_acid();
	
	if(update_stat)
		put_pc_screen();
	
}

void do_monster_turn() {
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
	if(overall_mode < MODE_COMBAT)
		which_combat_type = 1;
	
	for(i = 0; i < num_monst; i++) {  // Give monsters ap's, check activity
		
		cur_monst = &univ.town.monst[i];
		
		// See if hostile monster notices party, during combat
		if((cur_monst->active == 1) && (cur_monst->attitude % 2 == 1) && (overall_mode == MODE_COMBAT)) {
			r1 = get_ran(1,1,100); // Check if see PCs first
			// TODO: Hang on, isn't stealth supposed to get better as you level up?
			r1 += (univ.party.status[ePartyStatus::STEALTH] > 0) ? 45 : 0;
			r1 += can_see_light(cur_monst->cur_loc,closest_pc_loc(cur_monst->cur_loc),sight_obscurity) * 10;
			if(r1 < 50)
				cur_monst->active = 2;
			
			for(j = 0; j < univ.town->max_monst(); j++)
				if(monst_near(j,cur_monst->cur_loc,5,1)) {
					cur_monst->active = 2;
				}
		}
		if((cur_monst->active == 1) && (cur_monst->attitude % 2 == 1)) {
			// Now it looks for PC-friendly monsters
			// dist check is for efficiency
			for(j = 0; j < univ.town->max_monst(); j++)
				if((univ.town.monst[j].active > 0) &&
					(univ.town.monst[j].attitude % 2 != 1) &&
					(dist(cur_monst->cur_loc,univ.town.monst[j].cur_loc) <= 6) &&
					(can_see_light(cur_monst->cur_loc,univ.town.monst[j].cur_loc,sight_obscurity) < 5))
					cur_monst->active = 2;
		}
		
		// See if friendly, fighting monster see hostile monster. If so, make mobile
		// dist check is for efficiency
		if((cur_monst->active == 1) && (cur_monst->attitude == 2)) {
			for(j = 0; j < univ.town->max_monst(); j++)
				if((univ.town.monst[j].active > 0) && (univ.town.monst[j].attitude % 2 == 1) &&
					(dist(cur_monst->cur_loc,univ.town.monst[j].cur_loc) <= 6)
					&& (can_see_light(cur_monst->cur_loc,univ.town.monst[j].cur_loc,sight_obscurity) < 5)) {
					cur_monst->active = 2;
					cur_monst->mobility = 1;
				}
		}
		// End of seeing if monsters see others
		
		cur_monst->ap = 0;
		if(cur_monst->active == 2) { // Begin action loop for angry, active monsters
			// First note that hostile monsters are around.
			if(cur_monst->attitude % 2 == 1)
				PSD[SDF_HOSTILES_PRESENT] = 30;
			
			// Give monster its action points
			cur_monst->ap = cur_monst->speed;
			if(is_town())
				cur_monst->ap = max(1,cur_monst->ap / 3);
			if(univ.party.age % 2 == 0)
				if(cur_monst->status[eStatus::HASTE_SLOW] < 0)
					cur_monst->ap = 0;
			if(cur_monst->ap > 0) { // adjust for webs
				cur_monst->ap = max(0,cur_monst->ap - cur_monst->status[eStatus::WEBS] / 2);
				if(cur_monst->ap == 0)
					cur_monst->status[eStatus::WEBS] = max(0,cur_monst->status[eStatus::WEBS] - 2);
			}
			if(cur_monst->status[eStatus::HASTE_SLOW] > 0)
				cur_monst->ap *= 2;
		}
		if(cur_monst->status[eStatus::ASLEEP] > 0 || cur_monst->status[eStatus::PARALYZED] > 0)
			cur_monst->ap = 0;
		if(in_scen_debug)
			cur_monst->ap = 0;
		center_on_monst = false;
		
		// Now take care of summoned monsters
		if(cur_monst->active > 0) {
			if((cur_monst->summoned % 100) == 1) {
				cur_monst->active = 0;
				cur_monst->ap = 0;
				monst_spell_note(cur_monst->number,17);
			}
			move_to_zero(cur_monst->summoned);
		}
		
	}
	
	for(i = 0; i < num_monst; i++) {  // Begin main monster loop, do monster actions
		// If party dead, no point
		if(party_toast())
			return;
		
		futzing = 0; // assume monster is fresh
		
		cur_monst = &univ.town.monst[i];
		
		
		for(j = 0; j < 6; j++)
			if(univ.party[j].main_status == eMainStatus::ALIVE && monst_adjacent(univ.party[j].combat_pos,i))
				pc_adj[j] = true;
			else pc_adj[j] = false;
		
		
		
		while((cur_monst->ap > 0) && (cur_monst->active > 0)) {  // Spend each action point
			
			if(is_combat()) { // Pick target. If in town, target already picked
				// in do_monsters
				target = monst_pick_target(i);
				target = switch_target_to_adjacent(i,target);
				if(target < 6)
					targ_space = univ.party[target].combat_pos;
				else if(target != 6)
					targ_space = univ.town.monst[target - 100].cur_loc;
				univ.town.monst[i].target = target;
			}
			else {
				if(univ.town.monst[i].target < 6)
					targ_space = univ.town.p_loc;
				else if(univ.town.monst[i].target != 6)
					targ_space = univ.town.monst[univ.town.monst[i].target - 100].cur_loc;
			}
			
//			sprintf((char *)create_line,"  %d targets %d.",i,target);
//			add_string_to_buf((char *) create_line);
			
			if((univ.town.monst[i].target < 0) || ((univ.town.monst[i].target > 5) &&
													(univ.town.monst[i].target < 100)))
				univ.town.monst[i].target = 6;
			target = univ.town.monst[i].target;
			
			// Now if in town and monster about to attack, do a redraw, so we see monster
			// in right place
			if((target != 6) && (is_town()) && (redraw_not_yet_done)
				&& (party_can_see_monst(i))) {
				draw_terrain(0);
				redraw_not_yet_done = false;
			}
			
			// Draw w. monster in center, if can see
			if((cur_monst->ap > 0) && (is_combat())
				// First make sure it has a target and is close, if not, don't bother
				&& (cur_monst->attitude > 0) && (cur_monst->picture_num > 0)
				&& ((target != 6) || (cur_monst->attitude % 2 == 1))
				&& (party_can_see_monst(i)) ) {
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
			if((target != 6) && (cur_monst->ap > 1) && (futzing == 0)) {
				l = closest_pc_loc(cur_monst->cur_loc);
				if(((cur_monst->mu > 0) || (cur_monst->cl > 0)) &&
					(dist(cur_monst->cur_loc,l) < 5) && !monst_adjacent(l,i))
					current_monst_tactic = 1; // this means flee
				
				
				if(cur_monst->abil[eMonstAbil::MISSILE].active && // Archer?
					(dist(cur_monst->cur_loc,targ_space) < 6) &&
					!monst_adjacent(targ_space,i))
					current_monst_tactic = 1; // this means flee
			}
			
			
			// flee
			if((univ.town.monst[i].target != 6) && (((cur_monst->morale <= 0)
					&& !cur_monst->mindless && cur_monst->m_type != eRace::UNDEAD)
					|| (current_monst_tactic == 1))) {
				if(cur_monst->morale < 0)
					cur_monst->morale++;
				if(cur_monst->health > 50)
					cur_monst->morale++;
				r1 = get_ran(1,1,6);
				if(r1 == 3)
					cur_monst->morale++;
				short targ = univ.town.monst[i].target;
				if(targ < 6 && univ.party[targ].main_status == eMainStatus::ALIVE && cur_monst->mobility == 1) {
					acted_yet = flee_party (i,cur_monst->cur_loc,targ_space);
					if(acted_yet) take_m_ap(1,cur_monst);
				}
			}
			if((target != 6) && (cur_monst->attitude > 0)
				&& (monst_can_see(i,targ_space))
				&& (can_see_monst(targ_space,i))) { // Begin spec. attacks
				
//				sprintf((char *)create_line,"%d: %d  %d  %d",i,cur_monst->breath,cur_monst->mu,cur_monst->cl);
//				add_string_to_buf((char *)create_line);
				
				// Basic breath weapons
				if(cur_monst->abil[eMonstAbil::DAMAGE].active && cur_monst->abil[eMonstAbil::DAMAGE].gen.type == eMonstGen::BREATH
					&& !acted_yet && get_ran(1,1,1000) < cur_monst->abil[eMonstAbil::DAMAGE].gen.odds) {
					if(target != 6  && dist(cur_monst->cur_loc,targ_space) <= cur_monst->abil[eMonstAbil::DAMAGE].gen.range) {
						acted_yet = monst_breathe(cur_monst,targ_space,cur_monst->abil[eMonstAbil::DAMAGE]);
						had_monst = true;
						acted_yet = true;
						take_m_ap(4,cur_monst);
					}
				}
				// Mage spell
				if((cur_monst->mu > 0) && (get_ran(1,1,10) < ((cur_monst->cl > 0) ? 6 : 9) )
					&& !acted_yet) {
					if((!monst_adjacent(targ_space,i) || (get_ran(1,0,2) < 2) || (cur_monst->number >= 160)
						 || (cur_monst->level > 9))
						&& (dist(cur_monst->cur_loc,targ_space) <= 10)) {
						acted_yet = monst_cast_mage(cur_monst,target);
						had_monst = true;
						acted_yet = true;
						take_m_ap(5,cur_monst);
					}
				}
				// Priest spell
				if((cur_monst->cl > 0) && (get_ran(1,1,8) < 7) && !acted_yet) {
					if((!monst_adjacent(targ_space,i) || (get_ran(1,0,2) < 2) || (cur_monst->level > 9))
						&& (dist(cur_monst->cur_loc,targ_space) <= 10)) {
						acted_yet = monst_cast_priest(cur_monst,target);
						had_monst = true;
						acted_yet = true;
						take_m_ap(4,cur_monst);
					}
				}
				
				// Missile (except basic breath weapons)
				std::pair<eMonstAbil, uAbility> pick_abil;
				if(!acted_yet) {
					// Go through all the monster's abilities and try to select one that's useable at this time
					for(auto& abil : cur_monst->abil) {
						switch(abil.first) {
							case eMonstAbil::MISSILE:
								if(dist(cur_monst->cur_loc, targ_space) > abil.second.missile.range)
									break; // Target not in range
								if(abil.second.missile.type != eMonstMissile::SPINE && monst_adjacent(targ_space, i))
								   break; // Target adjacent; prefer melee attacks
								if(get_ran(1,1,1000) >= abil.second.missile.odds)
									break;
								pick_abil = abil;
								break;
							case eMonstAbil::DAMAGE: case eMonstAbil::STATUS: case eMonstAbil::STATUS2: case eMonstAbil::FIELD:
							case eMonstAbil::PETRIFY: case eMonstAbil::DRAIN_SP: case eMonstAbil::DRAIN_XP: case eMonstAbil::KILL:
							case eMonstAbil::STEAL_FOOD: case eMonstAbil::STEAL_GOLD: case eMonstAbil::STUN:
								if(abil.second.gen.type == eMonstGen::TOUCH)
									break; // We're looking for ranged attacks
								if(dist(cur_monst->cur_loc, targ_space) > abil.second.gen.range)
									break; // Target not in range
								if(abil.second.gen.type == eMonstGen::SPIT && monst_adjacent(targ_space, i))
								   break; // Target adjacent; prefer melee attacks
								if(get_ran(1,1,1000) >= abil.second.gen.odds)
									break;
								pick_abil = abil;
								break;
							case eMonstAbil::MISSILE_WEB:
								if(dist(cur_monst->cur_loc, targ_space) > abil.second.special.extra1)
									break; // Target not in range
								if(get_ran(1,1,1000) >= abil.second.special.extra2)
									break;
								pick_abil = abil;
								break;
							case eMonstAbil::RAY_HEAT:
								if(dist(cur_monst->cur_loc, targ_space) > abil.second.special.extra1)
									break; // Target not in range
								if(get_ran(1,1,1000) >= abil.second.special.extra2)
									break;
								pick_abil = abil;
								break;
						}
						if(pick_abil.second.active) break;
					}
				}
				if(pick_abil.second.active) {
					print_monst_name(cur_monst->number);
					monst_fire_missile(i/*,cur_monst->skill*/,cur_monst->status[eStatus::BLESS_CURSE],
									   pick_abil,cur_monst->cur_loc,target);
					if(pick_abil.first == eMonstAbil::MISSILE) {
						if(pick_abil.second.missile.type == eMonstMissile::ARROW || pick_abil.second.missile.type == eMonstMissile::BOLT
						   || pick_abil.second.missile.type == eMonstMissile::SPINE)
							take_m_ap(3,cur_monst);
						else take_m_ap(2,cur_monst);
					} else if(pick_abil.first == eMonstAbil::RAY_HEAT)
						take_m_ap(1,cur_monst);
					else take_m_ap(3,cur_monst);
					had_monst = true;
					acted_yet = true;
				}
			} // Special attacks
			
			// Attack pc
			if(univ.town.monst[i].target < 6 && univ.party[univ.town.monst[i].target].main_status == eMainStatus::ALIVE
			   && (monst_adjacent(targ_space,i))  && (cur_monst->attitude % 2 == 1)
			   && !acted_yet) {
				monster_attack_pc(i,univ.town.monst[i].target);
				take_m_ap(4,cur_monst);
				acted_yet = true;
				had_monst = true;
			}
			// Attack monst
			if((univ.town.monst[i].target >= 100) && (univ.town.monst[univ.town.monst[i].target - 100].active > 0)
				&& (monst_adjacent(targ_space,i))  && (cur_monst->attitude > 0)
				&& !acted_yet) {
				monster_attack_monster(i,univ.town.monst[i].target - 100);
				take_m_ap(4,cur_monst);
				acted_yet = true;
				had_monst = true;
			}
			
			if(acted_yet) {
				print_buf();
				if(j == 0)
					pause(8);
				flushingInput = true;
			}
			
			if(overall_mode == MODE_COMBAT) {
				if(!acted_yet && (cur_monst->mobility == 1)) {  // move monst
					move_target = (univ.town.monst[i].target != 6) ? univ.town.monst[i].target : closest_pc(cur_monst->cur_loc);
					if(monst_hate_spot(i,&move_targ)) // First, maybe move out of dangerous space
						seek_party (i,cur_monst->cur_loc,move_targ);
					else { // spot is OK, so go nuts
						if((cur_monst->attitude % 2 == 1) && (move_target < 6)) // Monsters seeking party do so
							if(univ.party[move_target].main_status == eMainStatus::ALIVE) {
								seek_party (i,cur_monst->cur_loc,univ.party[move_target].combat_pos);
								for(k = 0; k < 6; k++)
									if(univ.party[k].parry > 99 && monst_adjacent(univ.party[k].combat_pos,i)
									   && (cur_monst->active > 0)) {
										univ.party[k].parry = 0;
										pc_attack(k,i);
									}
							}
						
						if(move_target >= 100) // Monsters seeking monsters do so
							if(univ.town.monst[move_target - 100].active > 0) {
								seek_party (i,cur_monst->cur_loc,univ.town.monst[move_target - 100].cur_loc);
								for(k = 0; k < 6; k++)
									if(univ.party[k].parry > 99 && monst_adjacent(univ.party[k].combat_pos,i)
									   && (cur_monst->active > 0) && (cur_monst->attitude % 2 == 1)) {
										univ.party[k].parry = 0;
										pc_attack(k,i);
									}
							}
						
						if(cur_monst->attitude == 0) {
							acted_yet = rand_move(i);
							futzing++;
						}
					}
					take_m_ap(1,cur_monst);
				}
				if(!acted_yet && (cur_monst->mobility == 0)) { // drain action points
					take_m_ap(1,cur_monst);
					futzing++;
				}
			}
			else if(!acted_yet) {
				take_m_ap(1,cur_monst);
				futzing++;
			}
			
			// pcs attack any fleeing monsters
			if((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING))
				for(k = 0; k < 6; k++)
					if(univ.party[k].main_status == eMainStatus::ALIVE && !monst_adjacent(univ.party[k].combat_pos,i)
					   && (pc_adj[k]) && (cur_monst->attitude % 2 == 1) && (cur_monst->active > 0) &&
					   univ.party[k].status[eStatus::INVISIBLE] == 0) {
						combat_posing_monster = current_working_monster = k;
						pc_attack(k,i);
						combat_posing_monster = current_working_monster = 100 + i;
						pc_adj[k] = false;
					}
			
			// Place fields for monsters that create them. Only done when monst sees foe
			if(target != 6 && can_see_light(cur_monst->cur_loc,targ_space,sight_obscurity) < 5) {
				if(cur_monst->abil[eMonstAbil::RADIATE].active && get_ran(1,1,100) < cur_monst->abil[eMonstAbil::RADIATE].radiate.chance)
					place_spell_pattern(square, cur_monst->cur_loc, cur_monst->abil[eMonstAbil::RADIATE].radiate.type, 7);
				if(cur_monst->abil[eMonstAbil::SUMMON].active && get_ran(1,1,100) < cur_monst->abil[eMonstAbil::SUMMON].summon.chance) {
					uAbility abil = cur_monst->abil[eMonstAbil::SUMMON];
					r1 = get_ran(1, abil.summon.min, abil.summon.max);
					if(r1 && summon_monster(abil.summon.type, cur_monst->cur_loc,abil.summon.len,cur_monst->attitude)) {
						monst_spell_note(cur_monst->number,33);
						play_sound(61);
						bool failed = false;
						while(--r1 && !failed) {
							failed = summon_monster(abil.summon.type, cur_monst->cur_loc,abil.summon.len,cur_monst->attitude);
						}
					}
				}
				if(cur_monst->abil[eMonstAbil::SPECIAL].active && !special_called && party_can_see_monst(i)) {
					uAbility abil = cur_monst->abil[eMonstAbil::SPECIAL];
					short s1, s2, s3;
					special_called = true;
					// TODO: Is it good to allow only one monster to use a special ability per combat round?
					run_special(eSpecCtx::MONST_SPEC_ABIL,0,abil.special.extra1,cur_monst->cur_loc,&s1,&s2,&s3);
					take_m_ap(abil.special.extra2,cur_monst);
				}
			}
			
			combat_posing_monster = current_working_monster = -1;
			// Redraw monster after it goes
			if((cur_monst->attitude > 0) && (cur_monst->active > 0) && (cur_monst->ap == 0)
				&& (is_combat()) && (cur_monst->picture_num > 0) && (party_can_see_monst(i) )) {
				center = cur_monst->cur_loc;
				draw_terrain(0);
				
			}
			
			// If monster dead, take away actions
			if(cur_monst->active == 0)
				cur_monst->ap = 0;
			
			if(futzing > 1) // If monster's just pissing around, give up
				cur_monst->ap = 0;
		}  // End of monster action loop
		
		
	}
	
	for(i = 0; i < num_monst; i++) {  // Begin monster time stuff loop
		// If party dead, no point
		if(party_toast())
			return;
		
		cur_monst = &univ.town.monst[i];
		
		if((cur_monst->active < 0) || (cur_monst->active > 2))
			cur_monst->active = 0; // clean up
		if(cur_monst->active != 0) { // Take care of monster effects
			if(cur_monst->status[eStatus::ACID] > 0) {  // Acid
				if(!printed_acid) {
					add_string_to_buf("Acid:              ");
					printed_acid = true;
				}
				r1 = get_ran(cur_monst->status[eStatus::ACID],1,6);
				damage_monst(i, 6,r1, 0, eDamageType::MAGIC,0);
				cur_monst->status[eStatus::ACID]--;
			}
			
			if(cur_monst->status[eStatus::ASLEEP] == 1)
				monst_spell_note(cur_monst->number,29);
			move_to_zero(cur_monst->status[eStatus::ASLEEP]);
			move_to_zero(cur_monst->status[eStatus::PARALYZED]);
			
			if(univ.party.age % 2 == 0) {
				move_to_zero(cur_monst->status[eStatus::BLESS_CURSE]);
				move_to_zero(cur_monst->status[eStatus::HASTE_SLOW]);
				move_to_zero(cur_monst->status[eStatus::WEBS]);
				
				if(cur_monst->status[eStatus::POISON] > 0) {  // Poison
					if(!printed_poison) {
						add_string_to_buf("Poisoned monsters:              ");
						printed_poison = true;
					}
					r1 = get_ran(cur_monst->status[eStatus::POISON],1,6);
					damage_monst(i, 6, r1, 0, eDamageType::POISON,0);
					cur_monst->status[eStatus::POISON]--;
				}
				if(cur_monst->status[eStatus::DISEASE] > 0) {  // Disease
					if(!printed_disease) {
						add_string_to_buf("Diseased monsters:              ");
						printed_disease = true;
					}
					k = get_ran(1,1,5);
					switch(k) {
						case 1: case 2: poison_monst(cur_monst, 2);break;
						case 3:	slow_monst(cur_monst,2); break;
						case 4: curse_monst(cur_monst,2); break;
						case 5: scare_monst(cur_monst,10); break;
					}
					if(get_ran(1,1,6) < 4)
						cur_monst->status[eStatus::DISEASE]--;
				}
				
			}
			
			if(univ.party.age % 4 == 0) {
				if(cur_monst->mp < cur_monst->max_mp)
					cur_monst->mp += 2;
				move_to_zero(cur_monst->status[eStatus::DUMB]);
			}
		} // end take care of monsters
	}
	
	// If in town, need to restore center
	if(overall_mode < MODE_COMBAT)
		center = univ.town.p_loc;
	if(had_monst)
		put_pc_screen();
	for(i = 0; i < 6; i++)
		univ.party[i].parry = 0;
	
	monsters_going = false;
}

void monster_attack_pc(short who_att,short target) {
	cCreature *attacker;
	short r1,r2,i,store_hp,sound_type = 0;
	eDamageType dam_type = eDamageType::WEAPON;
	
	
	attacker = &univ.town.monst[who_att];
	
	// A peaceful monster won't attack
	if(attacker->attitude % 2 != 1)
		return;
	
	// Draw attacker frames
	if((is_combat())
		&& ((center_on_monst) || !monsters_going)) {
		if(!attacker->invisible)
			frame_space(attacker->cur_loc,0,attacker->x_width,attacker->y_width);
		frame_space(univ.party[target].combat_pos,1,1,1);
	}
	
	
	
	if((attacker->a[0].dice != 0) || (attacker->a[2].dice != 0))
		print_monst_attacks(attacker->number,target);
	
	// Check sanctuary
	if(univ.party[target].status[eStatus::INVISIBLE] > 0) {
		r1 = get_ran(1,1,100);
		if(r1 > hit_chance[attacker->level / 2]) {
			add_string_to_buf("  Can't find target!                 ");
		}
		return;
	}
	
	for(i = 0; i < 3; i++) {
		if(attacker->a[i].dice > 0 && univ.party[target].main_status == eMainStatus::ALIVE) {
//			sprintf ((char *) create_line, "  Attacks %s.",(char *) univ.party[target].name);
//			add_string_to_buf((char *) create_line);
			
			// Attack roll
			r1 = get_ran(1,1,100) - 5 * min(8,attacker->status[eStatus::BLESS_CURSE]) + 5 * univ.party[target].status[eStatus::BLESS_CURSE]
				+ 5 * stat_adj(target,eSkill::DEXTERITY) - 15;
			r1 += 5 * (attacker->status[eStatus::WEBS] / 3);
			if(univ.party[target].parry < 100)
				r1 += 5 * univ.party[target].parry;
			
			// Damage roll
			r2 = get_ran(attacker->a[i].dice,1,attacker->a[i].sides)
				+ min(8,attacker->status[eStatus::BLESS_CURSE]) - univ.party[target].status[eStatus::BLESS_CURSE] + 1;
			if(univ.difficulty_adjust() > 2)
				r2 = r2 * 2;
			if(univ.difficulty_adjust() == 2)
				r2 = (r2 * 3) / 2;
			
			if((univ.party[target].status[eStatus::ASLEEP] > 0) || (univ.party[target].status[eStatus::PARALYZED] > 0)) {
				r1 -= 80;
				r2 = r2 * 2;
			}
			
			draw_terrain(2);
			// Check if hit, and do effects
			if(r1 <= hit_chance[(attacker->skill + 4) / 2]) {
				if(attacker->m_type == eRace::UNDEAD)
					dam_type = eDamageType::UNDEAD;
				if(attacker->m_type == eRace::DEMON)
					dam_type = eDamageType::DEMON;
				
				store_hp = univ.party[target].cur_health;
				sound_type = get_monst_sound(attacker,i);
				if(damage_pc(target,r2,dam_type,
							  attacker->m_type,sound_type) &&
					(store_hp - univ.party[target].cur_health > 0)) {
					damaged_message(store_hp - univ.party[target].cur_health,
									attacker->a[i].type);
					
					if(univ.party[target].status[eStatus::MARTYRS_SHIELD] > 0) {
						add_string_to_buf("  Shares damage!                 ");
						damage_monst(who_att, 6, store_hp - univ.party[target].cur_health, 0, eDamageType::MAGIC,0);
					}
					
					for(auto& abil : attacker->abil) {
						if(!abil.second.active) continue;
						if(getMonstAbilCategory(abil.first) != eMonstAbilCat::GENERAL)
							continue;
						if(abil.second.gen.type != eMonstGen::TOUCH)
							continue;
						if(abil.second.gen.range > 0 && get_ran(1,1,1000) <= abil.second.gen.range)
							continue;
						// Print message and possibly choose sound
						snd_num_t snd = 0;
						switch(abil.first) {
							case eMonstAbil::STUN: add_string_to_buf("  Stuns!"); break;
							case eMonstAbil::PETRIFY: add_string_to_buf("  Petrifying touch!"); break;
							case eMonstAbil::DRAIN_SP: add_string_to_buf("  Drains magic!"); break;
							case eMonstAbil::DRAIN_XP: add_string_to_buf("  Drains life!"); break;
							case eMonstAbil::KILL: add_string_to_buf("  Killing touch!"); break;
							case eMonstAbil::STEAL_FOOD: add_string_to_buf("  Steals food!"); snd = 26; break;
							case eMonstAbil::STEAL_GOLD: add_string_to_buf("  Steals gold!"); break; // TODO: Pick a sound
							case eMonstAbil::FIELD: break; // TODO: Invent messages?
							case eMonstAbil::DAMAGE:
								switch(abil.second.gen.dmg) {
									case eDamageType::FIRE: add_string_to_buf("  Burning touch!"); break;
									case eDamageType::COLD: add_string_to_buf("  Freezing touch!"); break;
									case eDamageType::MAGIC: add_string_to_buf("  Shocking touch!"); break;
									case eDamageType::UNBLOCKABLE: add_string_to_buf("  Eerie touch!"); break;
									case eDamageType::POISON: add_string_to_buf("  Slimy touch!"); break;
									case eDamageType::WEAPON: add_string_to_buf("  Drains stamina!"); break;
									case eDamageType::UNDEAD: add_string_to_buf("  Chilling touch!"); break;
									case eDamageType::DEMON: add_string_to_buf("  Unholy touch!"); break;
								}
								break;
							case eMonstAbil::STATUS: case eMonstAbil::STATUS2:
								switch(abil.second.gen.stat) {
									case eStatus::POISON: add_string_to_buf("  Poisonous!"); break;
									case eStatus::DISEASE: add_string_to_buf("  Causes disease!"); break;
									case eStatus::DUMB: add_string_to_buf("  Dumbfounds!"); break;
									case eStatus::WEBS: add_string_to_buf("  Webs!"); break;
									case eStatus::ASLEEP: add_string_to_buf("  Sleeps!"); break;
									case eStatus::PARALYZED: add_string_to_buf("  Paralysis touch!"); break;
									case eStatus::ACID: add_string_to_buf("  Acid touch!"); break;
									case eStatus::HASTE_SLOW: add_string_to_buf("  Slowing touch!"); break;
									case eStatus::BLESS_CURSE: add_string_to_buf("  Cursing touch!"); break;
								}
								break;
						}
						if(snd > 0) play_sound(snd);
						monst_basic_abil(who_att, abil, target);
					}
				}
			}
			else {
				add_string_to_buf("  Misses.");
				play_sound(2);
			}
			redraw_screen(REFRESH_STATS | REFRESH_TERRAIN | REFRESH_TRANS);
			sf::sleep(time_in_ticks(2));
			combat_posing_monster = -1;
			draw_terrain(2);
			combat_posing_monster = 100 + who_att;
			
			
		}
		if(univ.party[target].main_status != eMainStatus::ALIVE)
			i = 3;
	}
	
}

void monster_attack_monster(short who_att,short attackee) {
	cCreature *attacker,*target;
	short r1,r2,i,store_hp,sound_type = 0;
	eDamageType dam_type = eDamageType::WEAPON;
	
	attacker = &univ.town.monst[who_att];
	target = &univ.town.monst[attackee];
	
	// Draw attacker frames
	if((is_combat())
		&& (center_on_monst || !monsters_going)) {
		if(!attacker->invisible)
			frame_space(attacker->cur_loc,0,attacker->x_width,attacker->y_width);
		frame_space(target->cur_loc,1,1,1);
	}
	
	
	if((attacker->a[1].dice != 0) || (attacker->a[0].dice != 0))
		print_monst_attacks(attacker->number,100 + attackee);
	for(i = 0; i < 3; i++) {
		if((attacker->a[i].dice > 0) && (target->active != 0)) {
//			sprintf ((char *) create_line, "  Attacks %s.",(char *) univ.party[target].name);
//			add_string_to_buf((char *) create_line);
			
			// if friendly to party, make able to attack
			if(target->attitude == 0)
				target->attitude = 2;
			
			// Attack roll
			r1 = get_ran(1,1,100) - 5 * min(10,attacker->status[eStatus::BLESS_CURSE])
				+ 5 * target->status[eStatus::BLESS_CURSE] - 15;
			r1 += 5 * (attacker->status[eStatus::WEBS] / 3);
			
			// Damage roll
			r2 = get_ran(attacker->a[i].dice,1,attacker->a[i].sides)
				+ min(10,attacker->status[eStatus::BLESS_CURSE]) - target->status[eStatus::BLESS_CURSE] + 2;
			
			if(target->status[eStatus::ASLEEP] > 0 || target->status[eStatus::PARALYZED] > 0) {
				r1 -= 80;
				r2 = r2 * 2;
			}
			
			draw_terrain(2);
			// Check if hit, and do effects
			if(r1 <= hit_chance[(attacker->skill + 4) / 2]) {
				if(attacker->m_type == eRace::DEMON)
					dam_type = eDamageType::DEMON;
				if(attacker->m_type == eRace::UNDEAD)
					dam_type = eDamageType::UNDEAD;
				store_hp = target->health;
				
				sound_type = get_monst_sound(attacker,i);
				if(damage_monst(attackee,7,r2,0,dam_type,sound_type,false)) {
					damaged_message(store_hp - target->health,
									attacker->a[i].type);
					
					for(auto& abil : attacker->abil) {
						if(!abil.second.active) continue;
						if(getMonstAbilCategory(abil.first) != eMonstAbilCat::GENERAL)
							continue;
						if(abil.second.gen.type != eMonstGen::TOUCH)
							continue;
						if(abil.second.gen.range > 0 && get_ran(1,1,1000) <= abil.second.gen.range)
							continue;
						// Print message and possibly choose sound
						snd_num_t snd = 0;
						switch(abil.first) {
							case eMonstAbil::STUN: add_string_to_buf("  Stuns!"); break;
							case eMonstAbil::PETRIFY: add_string_to_buf("  Petrifying touch!"); break;
							case eMonstAbil::DRAIN_SP: add_string_to_buf("  Drains magic!"); break;
							case eMonstAbil::KILL: add_string_to_buf("  Killing touch!"); break;
							case eMonstAbil::FIELD: break; // TODO: Invent messages?
							case eMonstAbil::DAMAGE:
								switch(abil.second.gen.dmg) {
									case eDamageType::FIRE: add_string_to_buf("  Burning touch!"); break;
									case eDamageType::COLD: add_string_to_buf("  Freezing touch!"); break;
									case eDamageType::MAGIC: add_string_to_buf("  Shocking touch!"); break;
									case eDamageType::UNBLOCKABLE: add_string_to_buf("  Eerie touch!"); break;
									case eDamageType::POISON: add_string_to_buf("  Slimy touch!"); break;
									case eDamageType::WEAPON: add_string_to_buf("  Drains stamina!"); break;
									case eDamageType::UNDEAD: add_string_to_buf("  Chilling touch!"); break;
									case eDamageType::DEMON: add_string_to_buf("  Unholy touch!"); break;
								}
								break;
							case eMonstAbil::STATUS: case eMonstAbil::STATUS2:
								switch(abil.second.gen.stat) {
									case eStatus::POISON: add_string_to_buf("  Poisonous!"); break;
									case eStatus::DISEASE: add_string_to_buf("  Causes disease!"); break;
									case eStatus::DUMB: add_string_to_buf("  Dumbfounds!"); break;
									case eStatus::WEBS: add_string_to_buf("  Webs!"); break;
									case eStatus::ASLEEP: add_string_to_buf("  Sleeps!"); break;
									case eStatus::PARALYZED: add_string_to_buf("  Paralysis touch!"); break;
									case eStatus::ACID: add_string_to_buf("  Acid touch!"); break;
									case eStatus::HASTE_SLOW: add_string_to_buf("  Slowing touch!"); break;
									case eStatus::BLESS_CURSE: add_string_to_buf("  Cursing touch!"); break;
								}
								break;
						}
						if(snd > 0) play_sound(snd);
						print_buf();
						monst_basic_abil(who_att, abil, attackee + 100);
						put_pc_screen();
					}
				}
			}
			else {
				add_string_to_buf("  Misses.");
				play_sound(2);
			}
			combat_posing_monster = -1;
			draw_terrain(2);
			combat_posing_monster = 100 + who_att;
		}
		if(target->active == 0)
			i = 3;
	}
	
}


//short target; // 100 +  - monster is target
void monst_fire_missile(short m_num,short bless,std::pair<eMonstAbil,uAbility> abil,location source,short target) {
	cCreature *m_target;
	short r1,r2,dam[40] = {
		0,1,2,3,4, 6,8,7,0,0, 0,0,0,0,0, 0,0,0,0,0,
		8,0,0,0,0, 0,0,0,0,0, 0,0,0,0,6, 0,0,0,0,0},i,j;
	location targ_space;
	
	if(target == 6)
		return;
	if(target >= 100) {
		targ_space = univ.town.monst[target - 100].cur_loc;
		if(univ.town.monst[target - 100].active == 0)
			return;
	}
	else {
		targ_space = (is_combat()) ? univ.party[target].combat_pos : univ.town.p_loc;
		if(univ.party[target].main_status != eMainStatus::ALIVE)
			return;
	}
	
	if(target >= 100)
		m_target = &univ.town.monst[target - 100];
	if(((overall_mode >= MODE_COMBAT) && (overall_mode <= MODE_TALKING)) && (center_on_monst)) {
		frame_space(source,0,univ.town.monst[m_num].x_width,univ.town.monst[m_num].y_width);
		if(target >= 100)
			frame_space(targ_space,1,m_target->x_width,m_target->y_width);
		else frame_space(targ_space,1,1,1);
	}
	
	draw_terrain(2);
	if(abil.first == eMonstAbil::MISSILE) {
		snd_num_t snd = 14;
		switch(abil.second.missile.type) {
			case eMonstMissile::ARROW:
			case eMonstMissile::BOLT:
				snd = 12;
				if(target < 100) add_string_to_buf("  Shoots at " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 12);
				break;
			case eMonstMissile::SPEAR:
				if(target < 100) add_string_to_buf("  Throws spear at " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 13);
				break;
			case eMonstMissile::RAZORDISK:
				if(target < 100) add_string_to_buf("  Throws razordisk at " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 15);
				break;
			case eMonstMissile::SPINE:
				if(target < 100) add_string_to_buf("  Fires spines at " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 32);
				break;
			case eMonstMissile::DART:
				if(target < 100) add_string_to_buf("  Throws dart at " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 53);
				break;
			case eMonstMissile::ROCK:
				if(target < 100) add_string_to_buf("  Throws rock at " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 14);
				break;
			case eMonstMissile::KNIFE:
				if(target < 100) add_string_to_buf("  Throws knife at " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 54);
				break;
		}
		if(abil.second.missile.type == eMonstMissile::ARROW || abil.second.missile.type == eMonstMissile::BOLT)
			snd = 12;
		if(abil.second.missile.pic < 0) play_sound(snd);
		else run_a_missile(source, targ_space, abil.second.missile.pic, 1, snd, 0, 0, 100);
		if((target < 100 && univ.party[target].status[eStatus::INVISIBLE] > 0) || m_target->invisible) {
			if(get_ran(1,1,100) > hit_chance[univ.town.monst[m_num].level]) {
				add_string_to_buf("  Can't find target!");
				return;
			}
		}
		int target_bless = target < 100 ? univ.party[target].status[eStatus::BLESS_CURSE] : m_target->status[eStatus::BLESS_CURSE];
		location target_pos = target < 100 ? univ.party[target].combat_pos : m_target->cur_loc;
		int r1 = get_ran(1,1,100) - 5 * min(10,bless) + 5 * target_bless - 5 * can_see_light(source, target_pos, sight_obscurity);
		if(univ.party[target].parry < 100)
			r1 += 5 * univ.party[target].parry;
			
		if(r1 <= hit_chance[abil.second.missile.skill]) {
			r2 = get_ran(abil.second.missile.dice,1,abil.second.missile.sides) + min(10,bless);
			if(target < 100) {
				add_string_to_buf("  Hits " + univ.party[target].name + '.');
				// TODO: Should we pass in the monster's actual race here?
				damage_pc(target,r2,eDamageType::WEAPON,eRace::UNKNOWN,13);
			} else {
				monst_spell_note(m_target->number, 16);
				damage_monst(target - 100,7,r2,0,eDamageType::WEAPON,13);
			}
		} else {
			if(target < 100) add_string_to_buf("  Misses " + univ.party[target].name + '.');
			else monst_spell_note(m_target->number,18);
		}
	} else if(abil.first == eMonstAbil::MISSILE_WEB) {
		if(target < 100) add_string_to_buf("  Throws web at " + univ.party[target].name + '.');
		else monst_spell_note(m_target->number, 58);
		run_a_missile(source, targ_space, 8, 0, 14, 0, 0, 100);
		web_space(targ_space.x, targ_space.y);
	} else if(abil.first == eMonstAbil::RAY_HEAT) {
		if(target < 100) add_string_to_buf("  Hits " + univ.party[target].name + " with heat ray!");
		else monst_spell_note(m_target->number, 55);
		run_a_missile(source, targ_space, 13, 0, 51, 0, 0, 100);
		uAbility proxy = {true};
		proxy.gen.strength = abil.second.special.extra3;
		proxy.gen.type = eMonstGen::RAY;
		proxy.gen.dmg = eDamageType::FIRE;
		monst_basic_abil(m_num, {eMonstAbil::DAMAGE, proxy}, target);
	} else {
		if(abil.first == eMonstAbil::DRAIN_SP && target < 100 && univ.party[target].cur_sp < 4) {
			// modify target if target has no sp
			for(i = 0; i < 8; i++) {
				j = get_ran(1,0,5);
				if(univ.party[j].main_status == eMainStatus::ALIVE && univ.party[j].cur_sp > 4 &&
				   (can_see_light(source,univ.party[j].combat_pos,sight_obscurity) < 5) && (dist(source,univ.party[j].combat_pos) <= 8)) {
					target = j;
					i = 8;
					targ_space = univ.party[target].combat_pos;
				}
			}
		}
		snd_num_t snd;
		int path_type = 0;
		switch(abil.second.gen.type) {
			case eMonstGen::TOUCH: return; // never reached
			case eMonstGen::RAY:
				snd = 51;
				if(target < 100) add_string_to_buf("  Fires ray at " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 55);
				break;
			case eMonstGen::GAZE:
				snd = 43;
				if(target < 100) add_string_to_buf("  Gazes at " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 56);
				break;
			case eMonstGen::BREATH:
				snd = 44;
				if(target < 100) add_string_to_buf("  Breathes on " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 57);
				break;
			case eMonstGen::SPIT:
				path_type = 1;
				snd = 64;
				if(target < 100) add_string_to_buf("  Spits at " + univ.party[target].name + '.');
				else monst_spell_note(m_target->number, 59);
				break;
		}
		if(abil.second.gen.pic < 0) play_sound(snd);
		else run_a_missile(source, targ_space, abil.second.gen.pic, path_type, snd, 0, 0, 100);
		monst_basic_abil(m_num, abil, target);
	}
}

void monst_basic_abil(short m_num, std::pair<eMonstAbil,uAbility> abil, short target) {
	int i, r1;
	cCreature* m_target;
	location targ_space;
	if(target == 6)
		return;
	if(target >= 100) {
		m_target = &univ.town.monst[m_num];
		targ_space = univ.town.monst[target - 100].cur_loc;
		if(univ.town.monst[target - 100].active == 0)
			return;
	} else {
		targ_space = (is_combat()) ? univ.party[target].combat_pos : univ.town.p_loc;
		if(univ.party[target].main_status != eMainStatus::ALIVE)
			return;
	}
	switch(abil.first) {
		case eMonstAbil::DAMAGE:
			// Determine die size
			i = 6;
			if(abil.second.gen.type == eMonstGen::BREATH)
				i = 8;
			else if(abil.second.gen.type == eMonstGen::SPIT || abil.second.gen.type == eMonstGen::TOUCH)
				i = 10;
			r1 = get_ran(abil.second.gen.strength, 1, i);
			start_missile_anim();
			if(target < 100) damage_pc(target, r1, abil.second.gen.dmg, eRace::UNKNOWN, 0);
			else damage_monst(target - 100, 7, r1, 0, abil.second.gen.dmg, 0);
			do_explosion_anim(5, 0);
			end_missile_anim();
			handle_marked_damage();
			break;
		case eMonstAbil::STUN:
			if(target < 100 && pc_has_abil_equip(target, eItemAbil::LIFE_SAVING) < 24)
				break;
		case eMonstAbil::STATUS: case eMonstAbil::STATUS2:
			switch(abil.second.gen.stat) {
				case eStatus::PARALYZED:
				case eStatus::ASLEEP:
					if(abil.second.gen.type == eMonstGen::TOUCH)
						i = abil.second.gen.stat == eStatus::ASLEEP ? -15 : -5;
					else i = univ.town.monst[m_num].level / 2;
					if(target < 100) sleep_pc(target, abil.second.gen.strength, abil.second.gen.stat, i);
					else charm_monst(m_target, i, abil.second.gen.stat, abil.second.gen.strength);
					break;
				case eStatus::ACID:
					if(target < 100) acid_pc(target, abil.second.gen.strength);
					else acid_monst(m_target, abil.second.gen.strength);
					break;
				case eStatus::POISON:
					if(target < 100) poison_pc(target, abil.second.gen.strength);
					else poison_monst(m_target, abil.second.gen.strength);
					break;
				case eStatus::BLESS_CURSE:
					if(target < 100) curse_pc(target, abil.second.gen.strength);
					else curse_monst(m_target, abil.second.gen.strength);
					break;
				case eStatus::HASTE_SLOW:
					if(target < 100) slow_pc(target, abil.second.gen.strength);
					else slow_monst(m_target, abil.second.gen.strength);
					break;
				case eStatus::WEBS:
					if(target < 100) web_pc(target, abil.second.gen.strength);
					else web_monst(m_target, abil.second.gen.strength);
					break;
				case eStatus::DISEASE:
					if(target < 100) disease_pc(target, abil.second.gen.strength);
					else disease_monst(m_target, abil.second.gen.strength);
					break;
				case eStatus::DUMB:
					if(target < 100) dumbfound_pc(target, abil.second.gen.strength);
					else dumbfound_monst(m_target, abil.second.gen.strength);
					break;
				// These only work on PCs
				case eStatus::INVULNERABLE:
				case eStatus::MAGIC_RESISTANCE:
				case eStatus::INVISIBLE:
				case eStatus::MARTYRS_SHIELD:
					if(target < 100)
						univ.party[target].apply_status(abil.second.gen.stat, -abil.second.gen.strength);
					break;
				// This only works on monsters
				case eStatus::CHARM:
					if(target >= 100)
						charm_monst(m_target, abil.second.gen.strength, eStatus::CHARM, univ.town.monst[m_num].attitude);
					break;
				// These three don't make sense in this context
				case eStatus::MAIN:
				case eStatus::POISONED_WEAPON:
				case eStatus::FORCECAGE:
					return;
			}
			break;
		case eMonstAbil::PETRIFY:
			i = univ.town.monst[m_num].level * abil.second.gen.strength / 100;
			if(target < 100) petrify_pc(target, i);
			else petrify_monst(m_target, i);
			break;
		case eMonstAbil::DRAIN_SP:
			if(target < 100) {
				add_string_to_buf("  Drains " + univ.party[target].name + '.');
				univ.party[target].cur_sp *= abil.second.gen.strength;
				univ.party[target].cur_sp /= 100;
			} else {
				monst_spell_note(m_target->number,11);
				// TODO: If mp < 4 it used to set monster's skill to 1. Should that be restored?
				m_target->mp *= abil.second.gen.strength;
				m_target->mp /= 100;
			}
			break;
		case eMonstAbil::DRAIN_XP:
			if(target < 100) {
				if(pc_has_abil_equip(target, eItemAbil::LIFE_SAVING) < 24) break;
				drain_pc(target, univ.town.monst[m_num].level * abil.second.gen.strength / 100);
			}
			break;
		case eMonstAbil::KILL:
			r1 = get_ran(10 * abil.second.gen.strength, 1, 10);
			damage_target(target, r1, eDamageType::UNBLOCKABLE);
			break;
		case eMonstAbil::STEAL_FOOD:
			univ.party.food = std::max(0, univ.party.food - get_ran(1,0,abil.second.gen.strength) - abil.second.gen.strength);
			break;
		case eMonstAbil::STEAL_GOLD:
			univ.party.gold = std::max(0, univ.party.gold - get_ran(1,0,abil.second.gen.strength) - abil.second.gen.strength);
			break;
		case eMonstAbil::FIELD:
			effect_pat_type pat;
			switch(eSpellPat(abil.second.gen.strength)) {
				case PAT_SINGLE: pat = single; break;
				case PAT_SQ: pat = small_square; break;
				case PAT_OPENSQ: pat = open_square; break;
				case PAT_SMSQ: pat = square; break;
				case PAT_PLUS: pat = t; break;
				case PAT_RAD2: pat = radius2; break;
				case PAT_RAD3: pat = radius3; break;
				default: // either PAT_WALL or not a valid eSpellPat
					pat = field[abil.second.gen.strength / 10];
					break;
			}
			place_spell_pattern(pat, targ_space, abil.second.gen.fld, 7);
			break;
	}
}

bool monst_breathe(cCreature *caster,location targ_space,uAbility abil) {
	draw_terrain(2);
	if((is_combat()) && (center_on_monst)) {
		frame_space(caster->cur_loc,0,caster->x_width,caster->y_width);
	}
	
	location l = caster->cur_loc;
	if((caster->direction < 4) &&
		(caster->x_width > 1))
		l.x++;
	
	if(abil.gen.pic >= 0)
		run_a_missile(l,targ_space,abil.gen.pic,0,44,0,0,100);
	else play_sound(44);
	
	monst_breathe_note(caster->number);
	short level = get_ran(abil.gen.strength,1,8);
	if(overall_mode < MODE_COMBAT)
		level = level / 3;
	start_missile_anim();
	hit_space(targ_space,level,abil.gen.dmg,1,1);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
	
	return true;
}

bool monst_cast_mage(cCreature *caster,short targ) {
	short r1,j,i,level,target_levels,friend_levels_near,x;
	bool acted = false;
	location target,vict_loc,ashes_loc,l;
	cCreature *affected;
	const eSpell caster_array[7][18] = {
		{
			eSpell::SPARK, eSpell::SPARK, eSpell::SPARK, eSpell::HASTE_MINOR, eSpell::HASTE_MINOR,
			eSpell::HASTE_MINOR, eSpell::SPARK, eSpell::STRENGTH, eSpell::CLOUD_FLAME, eSpell::CLOUD_FLAME,
			eSpell::SPARK, eSpell::SPARK, eSpell::SPARK, eSpell::HASTE_MINOR, eSpell::HASTE_MINOR,
			eSpell::HASTE_MINOR, eSpell::STRENGTH, eSpell::CLOUD_FLAME,
		}, {
			eSpell::FLAME, eSpell::FLAME, eSpell::FLAME, eSpell::POISON_MINOR, eSpell::SLOW,
			eSpell::DUMBFOUND, eSpell::CLOUD_STINK, eSpell::SUMMON_BEAST, eSpell::CONFLAGRATION, eSpell::CONFLAGRATION,
			eSpell::HASTE_MINOR, eSpell::HASTE_MINOR, eSpell::HASTE_MINOR, eSpell::FLAME, eSpell::SLOW,
			eSpell::SUMMON_BEAST, eSpell::SUMMON_BEAST, eSpell::FLAME,
		}, {
			eSpell::FLAME, eSpell::FLAME, eSpell::HASTE_MINOR, eSpell::CLOUD_STINK, eSpell::CONFLAGRATION,
			eSpell::FIREBALL, eSpell::FIREBALL, eSpell::FIREBALL, eSpell::WEB, eSpell::SUMMON_WEAK,
			eSpell::SUMMON_WEAK, eSpell::FIREBALL, eSpell::FIREBALL, eSpell::HASTE_MINOR, eSpell::HASTE_MINOR,
			eSpell::HASTE_MINOR, eSpell::HASTE_MINOR, eSpell::HASTE_MINOR,
		}, {
			eSpell::POISON, eSpell::POISON, eSpell::ICE_BOLT, eSpell::SLOW_GROUP, eSpell::SLOW_GROUP,
			eSpell::FLAME, eSpell::FIREBALL, eSpell::FIREBALL, eSpell::SUMMON_WEAK, eSpell::SUMMON_WEAK,
			eSpell::SLOW_GROUP, eSpell::SLOW_GROUP, eSpell::ICE_BOLT, eSpell::SLOW_GROUP, eSpell::ICE_BOLT,
			eSpell::HASTE_MINOR, eSpell::HASTE_MINOR, eSpell::HASTE_MINOR,
		}, {
			eSpell::POISON, eSpell::HASTE_MAJOR, eSpell::FIRESTORM, eSpell::FIRESTORM, eSpell::SUMMON,
			eSpell::SUMMON, eSpell::SHOCKSTORM, eSpell::SHOCKSTORM, eSpell::ICE_BOLT, eSpell::SLOW_GROUP,
			eSpell::HASTE_MAJOR, eSpell::HASTE_MAJOR, eSpell::HASTE_MAJOR, eSpell::HASTE_MAJOR, eSpell::FIRESTORM,
			eSpell::FIRESTORM, eSpell::FIRESTORM, eSpell::SUMMON,
		}, {
			eSpell::KILL, eSpell::KILL, eSpell::POISON_MAJOR, eSpell::POISON_MAJOR, eSpell::SHOCKSTORM,
			eSpell::SHOCKSTORM, eSpell::SUMMON, eSpell::DEMON, eSpell::FIRESTORM, eSpell::HASTE_MAJOR,
			eSpell::HASTE_MAJOR, eSpell::HASTE_MAJOR, eSpell::HASTE_MAJOR, eSpell::HASTE_MAJOR, eSpell::HASTE_MAJOR,
			eSpell::KILL, eSpell::KILL, eSpell::FIRESTORM,
		}, {
			eSpell::KILL, eSpell::KILL, eSpell::DEMON, eSpell::BLESS_MAJOR, eSpell::SUMMON_MAJOR,
			eSpell::SHOCKWAVE, eSpell::FIRESTORM, eSpell::POISON_MAJOR, eSpell::FIRESTORM, eSpell::HASTE_MAJOR,
			eSpell::HASTE_MAJOR, eSpell::HASTE_MAJOR, eSpell::HASTE_MAJOR, eSpell::HASTE_MAJOR, eSpell::SUMMON_MAJOR,
			eSpell::DEMON, eSpell::DEMON, eSpell::KILL,
		},
	};
	// 0 - cast when slowed, 1 - don't have enough friends, 2 - lots of clustered enemies, 3 - low on health
	// TODO: In column 3, level 1 monsters shouldn't have flame (a level 2 spell), and level 5 monsters shouldn't use haste.
	// (The latter is because column 3 is also used if they're already hasted.)
	eSpell emer_spells[7][4] = {
		{eSpell::HASTE_MINOR, eSpell::NONE, eSpell::NONE, eSpell::FLAME},
		{eSpell::HASTE_MINOR, eSpell::SUMMON_BEAST, eSpell::CONFLAGRATION, eSpell::SLOW},
		{eSpell::HASTE_MINOR, eSpell::SUMMON_WEAK, eSpell::FIREBALL, eSpell::SUMMON_WEAK},
		{eSpell::HASTE_MINOR, eSpell::SUMMON_WEAK, eSpell::FIREBALL, eSpell::SUMMON_WEAK},
		{eSpell::HASTE_MAJOR, eSpell::SUMMON, eSpell::FIRESTORM, eSpell::HASTE_MAJOR},
		{eSpell::HASTE_MAJOR, eSpell::DEMON, eSpell::FIRESTORM, eSpell::DEMON},
		{eSpell::HASTE_MAJOR, eSpell::SUMMON_MAJOR, eSpell::FIRESTORM, eSpell::SHOCKWAVE},
	};
	std::set<eSpell> area_effects = {
		eSpell::CLOUD_STINK, eSpell::CONFLAGRATION, eSpell::FIREBALL, eSpell::WEB, eSpell::FIRESTORM, eSpell::SHOCKSTORM,
	};
	
	if(univ.town.is_antimagic(caster->cur_loc.x,caster->cur_loc.y)) {
		return false;
	}
	// is target dead?
	if(targ < 6 && univ.party[targ].main_status != eMainStatus::ALIVE)
		return false;
	if((targ >= 100) && (univ.town.monst[targ - 100].active == 0))
		return false;
	
	eSpell spell;
	
	level = max(1,caster->mu - caster->status[eStatus::DUMB]) - 1;
	
	target = find_fireball_loc(caster->cur_loc,1,(caster->attitude % 2 == 1) ? 0 : 1,&target_levels);
	friend_levels_near = (caster->attitude % 2 != 1) ? count_levels(caster->cur_loc,3) : -1 * count_levels(caster->cur_loc,3);
	
	if((caster->health * 4 < caster->m_health) && (get_ran(1,0,10) < 9))
		spell = emer_spells[level][3];
	else if(((caster->status[eStatus::HASTE_SLOW] < 0 && get_ran(1,0,10) < 7) ||
			 (caster->status[eStatus::HASTE_SLOW] == 0 && get_ran(1,0,10) < 5)) && emer_spells[level][0] != eSpell::NONE)
		spell = emer_spells[level][0];
	else if(friend_levels_near <= -10 && get_ran(1,0,10) < 7 && emer_spells[level][1] != eSpell::NONE)
		spell = emer_spells[level][1];
	else if(target_levels > 50 && get_ran(1,0,10) < 7 && emer_spells[level][2] != eSpell::NONE)
		spell = emer_spells[level][2];
	else {
		r1 = get_ran(1,0,17);
		spell = caster_array[level][r1];
	}
	
	// Hastes happen often now, but don't cast them redundantly
	if(caster->status[eStatus::HASTE_SLOW] > 0 && (spell == eSpell::HASTE_MINOR || spell == eSpell::HASTE_MAJOR))
		spell = emer_spells[level][3];
	
	
	// Anything preventing spell?
	if(target.x > 64 && area_effects.count(spell) > 0) {
		r1 = get_ran(1,0,9);
		spell = caster_array[level][r1];
		if(target.x > 64 && area_effects.count(spell) > 0)
			return false;
	}
	if(area_effects.count(spell) > 0) {
		targ = 6;
	}
	
	if(targ < 6) {
		vict_loc = (is_combat()) ? univ.party[targ].combat_pos : univ.town.p_loc;
	}
	if(targ >= 100)
		vict_loc = univ.town.monst[targ - 100].cur_loc;
	
	// check antimagic
	if(targ == 6 && univ.town.is_antimagic(target.x,target.y))
		return false;
	if(is_combat())
		if(targ < 6 && univ.town.is_antimagic(univ.party[targ].combat_pos.x,univ.party[targ].combat_pos.y))
			return false;
	if(is_town())
		if(targ < 6 && univ.town.is_antimagic(univ.town.p_loc.x,univ.town.p_loc.y))
			return false;
	if(targ >= 100 && univ.town.is_antimagic(univ.town.monst[targ - 100].cur_loc.x, univ.town.monst[targ - 100].cur_loc.y))
		return false;
	
	// How about shockwave? Good idea?
	if(spell == eSpell::SHOCKWAVE && caster->attitude % 2 != 1)
		spell = eSpell::SUMMON_MAJOR;
	if(spell == eSpell::SHOCKWAVE && caster->attitude % 2 == 1 && count_levels(caster->cur_loc,10) < 45)
		spell = eSpell::SUMMON_MAJOR;
	
//	sprintf((char *)create_line,"m att %d trg %d trg2 x%dy%d spl %d mp %d tl:%d ",caster->attitude,targ,
//		(short)target.x,(short)target.y,spell,caster->mp,target_levels);
//	add_string_to_buf((char *) create_line);
	
	l = caster->cur_loc;
	if(caster->direction < 4 && caster->x_width > 1)
		l.x++;
	
	short cost = (*spell).level;
	if(spell == eSpell::SUMMON_BEAST || spell == eSpell::FIREBALL || spell == eSpell::SUMMON_WEAK)
		cost = 4;
	
	if(caster->mp >= cost) {
		monst_cast_spell_note(caster->number,spell);
		acted = true;
		caster->mp -= cost;
		
		draw_terrain(2);
		switch(spell) {
			case eSpell::SPARK:
				run_a_missile(l,vict_loc,6,1,11,0,0,80);
				r1 = get_ran(2,1,4);
				damage_target(targ,r1,eDamageType::MAGIC);
				break;
			case eSpell::HASTE_MINOR:
				play_sound(25);
				slow_monst(caster, -2);
				break;
			case eSpell::STRENGTH:
				play_sound(25);
				curse_monst(caster, -3);
				break;
			case eSpell::CLOUD_FLAME:
				run_a_missile(l,vict_loc,2,1,11,0,0,80);
				place_spell_pattern(single,vict_loc,WALL_FIRE,7);
				break;
			case eSpell::FLAME:
				run_a_missile(l,vict_loc,2,1,11,0,0,80);
				start_missile_anim();
				r1 = get_ran(min(15,caster->level),1,4);
				damage_target(targ,r1,eDamageType::FIRE);
				break;
			case eSpell::POISON_MINOR:
				run_a_missile(l,vict_loc,11,0,25,0,0,80);
				if(targ < 6)
					poison_pc(targ,2 + get_ran(1,0,caster->level / 2));
				else poison_monst(&univ.town.monst[targ - 100],2 + get_ran(1,0,caster->level / 2));
				break;
			case eSpell::SLOW:
				run_a_missile(l,vict_loc,15,0,25,0,0,80);
				if(targ < 6)
					slow_pc(targ,2 + caster->level / 2);
				else slow_monst(&univ.town.monst[targ - 100],2 + caster->level / 2);
				break;
			case eSpell::DUMBFOUND:
				run_a_missile(l,vict_loc,14,0,25,0,0,80);
				if(targ < 6)
					dumbfound_pc(targ,2);
				else dumbfound_monst(&univ.town.monst[targ - 100],2);
				break;
			case eSpell::CLOUD_STINK:
				run_a_missile(l,target,0,0,25,0,0,80);
				place_spell_pattern(square,target,CLOUD_STINK,7);
				break;
			case eSpell::SUMMON_BEAST:
				r1 = get_summon_monster(1);
				if(r1 == 0)
					break;
				x = get_ran(3,1,4);
				play_sound(25);
				play_sound(-61);
				summon_monster(r1,caster->cur_loc,
							   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				break;
			case eSpell::CONFLAGRATION:
				run_a_missile(l,target,13,1,25,0,0,80);
				place_spell_pattern(radius2,target,WALL_FIRE,7);
				break;
			case eSpell::FIREBALL:
				r1 = 1 + (caster->level * 3) / 4;
				if(r1 > 29) r1 = 29;
				run_a_missile(l,target,2,1,11,0,0,80);
				start_missile_anim();
				place_spell_pattern(square,target,eDamageType::FIRE,r1,7);
				ashes_loc = target;
				break;
			case eSpell::SUMMON_WEAK: case eSpell::SUMMON: case eSpell::SUMMON_MAJOR:
				play_sound(25);
				if(spell == eSpell::SUMMON_WEAK) {
					r1 = get_summon_monster(1);
					if(r1 == 0)
						break;
					j = get_ran(2,1,3) + 1;
				}
				if(spell == eSpell::SUMMON) {
					r1 = get_summon_monster(2);
					if(r1 == 0)
						break;
					j = get_ran(2,1,2) + 1;
				}
				if(spell == eSpell::SUMMON_MAJOR) {
					r1 = get_summon_monster(3);
					if(r1 == 0)
						break;
					j = get_ran(1,2,3);
				}
				sf::sleep(time_in_ticks(12)); // gives sound time to end
				x = get_ran(4,1,4);
				for(i = 0; i < j; i++){
					play_sound(-61);
					if(!summon_monster(r1,caster->cur_loc,
									   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude)) {
						add_string_to_buf("  Summon failed."); i = j;}
				}
				break;
			case eSpell::WEB:
				play_sound(25);
				place_spell_pattern(radius2,target,FIELD_WEB,7);
				break;
			case eSpell::POISON:
				run_a_missile(l,vict_loc,11,0,25,0,0,80);
				x = get_ran(1,0,caster->level / 2);
				if(targ < 6)
					poison_pc(targ,4 + x);
				else poison_monst(&univ.town.monst[targ - 100],4 + x);
				break;
			case eSpell::ICE_BOLT:
				run_a_missile(l,vict_loc,6,1,11,0,0,80);
				r1 = get_ran(5 + (caster->level / 5),1,8);
				start_missile_anim();
				damage_target(targ,r1,eDamageType::COLD);
				break;
			case eSpell::SLOW_GROUP:
				play_sound(25);
				if(caster->attitude % 2 == 1)
					for(i = 0; i < 6; i++)
						if(pc_near(i,caster->cur_loc,8))
							slow_pc(i,2 + caster->level / 4);
				for(i = 0; i < univ.town->max_monst(); i++) {
					if((univ.town.monst[i].active != 0) &&
						(((univ.town.monst[i].attitude % 2 == 1) && (caster->attitude % 2 != 1)) ||
						 ((univ.town.monst[i].attitude % 2 != 1) && (caster->attitude % 2 == 1)) ||
						 ((univ.town.monst[i].attitude % 2 == 1) && (caster->attitude != univ.town.monst[i].attitude)))
						&& (dist(caster->cur_loc,univ.town.monst[i].cur_loc) <= 7))
						slow_monst(&univ.town.monst[i],2 + caster->level / 4);
				}
				break;
			case eSpell::HASTE_MAJOR:
				play_sound(25);
				for(i = 0; i < univ.town->max_monst(); i++)
					if((monst_near(i,caster->cur_loc,8,0)) &&
						(caster->attitude == univ.town.monst[i].attitude)) {
						affected = &univ.town.monst[i];
						slow_monst(affected, -3);
					}
				play_sound(4);
				break;
			case eSpell::FIRESTORM:
				run_a_missile(l,target,2,1,11,0,0,80);
				r1 = 1 + (caster->level * 3) / 4 + 3;
				if(r1 > 29) r1 = 29;
				start_missile_anim();
				place_spell_pattern(radius2,target,eDamageType::FIRE,r1,7);
				ashes_loc = target;
				break;
			case eSpell::SHOCKSTORM:
				run_a_missile(l,target,6,1,11,0,0,80);
				place_spell_pattern(radius2,target,WALL_FORCE,7);
				break;
			case eSpell::POISON_MAJOR:
				run_a_missile(l,vict_loc,11,1,11,0,0,80);
				x = get_ran(1,1,2);
				if(targ < 6)
					poison_pc(targ,6 + x);
				else poison_monst(&univ.town.monst[targ - 100],6 + x);
				break;
			case eSpell::KILL:
				run_a_missile(l,vict_loc,9,1,11,0,0,80);
				r1 = 35 + get_ran(3,1,10);
				start_missile_anim();
				damage_target(targ,r1,eDamageType::MAGIC);
				break;
			case eSpell::DEMON:
				x = get_ran(3,1,4);
				play_sound(25);
				play_sound(-61);
				sf::sleep(time_in_ticks(12)); // gives sound time to end
				summon_monster(85,caster->cur_loc,
							   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				break;
			case eSpell::BLESS_MAJOR:
				play_sound(25);
				for(i = 0; i < univ.town->max_monst(); i++)
					if((monst_near(i,caster->cur_loc,8,0)) &&
						(caster->attitude == univ.town.monst[i].attitude)) {
						affected = &univ.town.monst[i];
						affected->health += get_ran(2,1,10);
						r1 = get_ran(3,1,4);
						curse_monst(affected, -r1);
						affected->status[eStatus::WEBS] = 0;
						if(affected->status[eStatus::HASTE_SLOW] < 0)
							affected->status[eStatus::HASTE_SLOW] = 0;
						affected->morale += get_ran(3,1,10);
					}
				play_sound(4);
				break;
			case eSpell::SHOCKWAVE:
				do_shockwave(caster->cur_loc);
				break;
		}
	}
	else caster->mp++;
	
	if(ashes_loc.x > 0)
		univ.town.set_ash(ashes_loc.x,ashes_loc.y,true);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
	
	return acted;
}

bool monst_cast_priest(cCreature *caster,short targ) {
	short r1,r2,i,x,level,target_levels,friend_levels_near;
	bool acted = false;
	location target,vict_loc,l;
	cCreature *affected;
	eSpell caster_array[7][10] = {
		{
			eSpell::BLESS_MINOR, eSpell::BLESS_MINOR, eSpell::BLESS_MINOR, eSpell::BLESS_MINOR, eSpell::WRACK,
			eSpell::WRACK, eSpell::WRACK, eSpell::GOO, eSpell::GOO, eSpell::GOO,
		}, {
			eSpell::BLESS, eSpell::BLESS, eSpell::CURSE, eSpell::CURSE, eSpell::WOUND,
			eSpell::WOUND, eSpell::SUMMON_SPIRIT, eSpell::SUMMON_SPIRIT, eSpell::SUMMON_SPIRIT, eSpell::DISEASE,
		}, {
			eSpell::DISEASE, eSpell::CURSE, eSpell::CURSE, eSpell::SUMMON_SPIRIT, eSpell::HOLY_SCOURGE,
			eSpell::SMITE, eSpell::SMITE, eSpell::BLESS, eSpell::BLESS, eSpell::SMITE,
		}, {
			eSpell::SMITE, eSpell::SMITE, eSpell::CURSE_ALL, eSpell::CURSE_ALL, eSpell::STICKS_TO_SNAKES,
			eSpell::DISEASE, eSpell::DISEASE, eSpell::STICKS_TO_SNAKES, eSpell::STICKS_TO_SNAKES, eSpell::MARTYRS_SHIELD,
		}, {
			eSpell::SUMMON_HOST, eSpell::FLAMESTRIKE, eSpell::CURSE_ALL, eSpell::SUMMON_HOST, eSpell::MARTYRS_SHIELD,
			eSpell::FLAMESTRIKE, eSpell::FLAMESTRIKE, eSpell::SUMMON_HOST, eSpell::BLESS_PARTY, eSpell::FLAMESTRIKE,
		}, {
			eSpell::SUMMON_GUARDIAN, eSpell::FLAMESTRIKE, eSpell::BLESS_PARTY, eSpell::SUMMON_HOST, eSpell::FLAMESTRIKE,
			eSpell::FLAMESTRIKE, eSpell::UNHOLY_RAVAGING, eSpell::SUMMON_GUARDIAN, eSpell::PESTILENCE, eSpell::PESTILENCE,
		}, {
			eSpell::DIVINE_THUD, eSpell::DIVINE_THUD, eSpell::AVATAR, eSpell::REVIVE_ALL, eSpell::DIVINE_THUD,
			eSpell::SUMMON_GUARDIAN, eSpell::REVIVE_ALL, eSpell::SUMMON_GUARDIAN, eSpell::DIVINE_THUD, eSpell::AVATAR,
		},
	};
	// 0 - cast when slowed, 1 - don't have enough friends, 2 - lots of clustered enemies, 3 - low on health
	eSpell emer_spells[7][4] = {
		{eSpell::NONE, eSpell::BLESS_MINOR, eSpell::NONE, eSpell::HEAL_MINOR},
		{eSpell::NONE, eSpell::SUMMON_SPIRIT, eSpell::NONE, eSpell::HEAL_MINOR},
		{eSpell::NONE, eSpell::SUMMON_SPIRIT, eSpell::NONE, eSpell::HEAL},
		{eSpell::NONE, eSpell::STICKS_TO_SNAKES, eSpell::NONE, eSpell::HEAL},
		{eSpell::NONE, eSpell::SUMMON_HOST, eSpell::FLAMESTRIKE, eSpell::HEAL_MAJOR},
		{eSpell::NONE, eSpell::SUMMON_HOST, eSpell::FLAMESTRIKE, eSpell::HEAL_ALL},
		{eSpell::AVATAR, eSpell::AVATAR, eSpell::DIVINE_THUD, eSpell::REVIVE_ALL},
	};
	std::set<eSpell> area_effects = {eSpell::FLAMESTRIKE, eSpell::DIVINE_THUD};
	location ashes_loc;
	
	
	if(targ < 6 && univ.party[targ].main_status != eMainStatus::ALIVE)
		return false;
	if((targ >= 100) && (univ.town.monst[targ - 100].active == 0))
		return false;
	if(univ.town.is_antimagic(caster->cur_loc.x,caster->cur_loc.y)) {
		return false;
	}
	level = max(1,caster->cl - caster->status[eStatus::DUMB]) - 1;
	
	eSpell spell;
	
	target = find_fireball_loc(caster->cur_loc,1,(caster->attitude % 2 == 1) ? 0 : 1,&target_levels);
	friend_levels_near = (caster->attitude % 2 != 1) ? count_levels(caster->cur_loc,3) : -1 * count_levels(caster->cur_loc,3);
	
	if((caster->health * 4 < caster->m_health) && (get_ran(1,0,10) < 9))
		spell = emer_spells[level][3];
	else if(caster->status[eStatus::HASTE_SLOW] < 0 && get_ran(1,0,10) < 7 && emer_spells[level][0] != eSpell::NONE)
		spell = emer_spells[level][0];
	else if(friend_levels_near <= -10 && get_ran(1,0,10) < 7 && emer_spells[level][1] != eSpell::NONE)
		spell = emer_spells[level][1];
	else if(target_levels > 50 && get_ran(1,0,10) < 7 && emer_spells[level][2] != eSpell::NONE)
		spell = emer_spells[level][2];
	else {
		r1 = get_ran(1,0,9);
		spell = caster_array[level][r1];
	}
	
	// Anything preventing spell?
	if(target.x > 64 && area_effects.count(spell) > 0)  {
		r1 = get_ran(1,0,9);
		spell = caster_array[level][r1];
		if(target.x > 64 && area_effects.count(spell) > 0)
			return false;
	}
	if(area_effects.count(spell) > 0)
		targ = 6;
	if(targ < 6)
		vict_loc = (is_town()) ? univ.town.p_loc : univ.party[targ].combat_pos;
	if(targ >= 100)
		vict_loc = univ.town.monst[targ - 100].cur_loc;
	if(targ == 6 && univ.town.is_antimagic(target.x,target.y))
		return false;
	if(targ < 6 && univ.town.is_antimagic(univ.party[targ].combat_pos.x,univ.party[targ].combat_pos.y))
		return false;
	if(targ >= 100 && univ.town.is_antimagic(univ.town.monst[targ - 100].cur_loc.x,univ.town.monst[targ - 100].cur_loc.y))
		return false;
	
	
//	sprintf((char *)create_line,"p att %d trg %d trg2 x%dy%d spl %d mp %d",caster->attitude,targ,
//		(short)target.x,(short)target.y,spell,caster->mp);
//	add_string_to_buf((char *) create_line);
	
	// snuff heals if unwounded
	if(caster->health == caster->m_health) {
		// TODO: Should probably check minor heal but not heal all.
		if(spell == eSpell::HEAL_MAJOR) spell = eSpell::BLESS_PARTY;
		if(spell == eSpell::HEAL_ALL) spell = eSpell::SUMMON_HOST;
	}
	
	l = caster->cur_loc;
	if((caster->direction < 4) && (caster->x_width > 1))
		l.x++;
	
	short cost = (*spell).level;
	if(spell == eSpell::SUMMON_SPIRIT || spell == eSpell::PESTILENCE || cost == 7)
		cost = 8;
	if(spell == eSpell::SUMMON_GUARDIAN || spell == eSpell::SUMMON_HOST)
		cost = 10;
	
	if(caster->mp >= cost) {
		monst_cast_spell_note(caster->number,spell);
		acted = true;
		caster->mp -= cost;
		draw_terrain(2);
		switch(spell) {
			case eSpell::WRACK:
				run_a_missile(l,vict_loc,8,0,24,0,0,80);
				r1 = get_ran(2,1,4);
				start_missile_anim();
				damage_target(targ,r1,eDamageType::UNBLOCKABLE);
				break;
			case eSpell::GOO:
				play_sound(24);
				place_spell_pattern(single,vict_loc,FIELD_WEB,7);
				break;
			case eSpell::BLESS_MINOR: case eSpell::BLESS:
				play_sound(24);
				curse_monst(caster, -(spell == eSpell::BLESS ? 5 : 3));
				play_sound(4);
				break;
			case eSpell::CURSE:
				run_a_missile(l,vict_loc,8,0,24,0,0,80);
				x = get_ran(1,0,1);
				if(targ < 6)
					curse_pc(targ,2 + x);
				else curse_monst(&univ.town.monst[targ - 100],2 + x);
				break;
			case eSpell::WOUND:
				run_a_missile(l,vict_loc,8,0,24,0,0,80);
				r1 = get_ran(2,1,6) + 2;
				start_missile_anim();
				damage_target(targ,r1,eDamageType::UNBLOCKABLE);
				break;
			case eSpell::SUMMON_SPIRIT: case eSpell::SUMMON_GUARDIAN:
				play_sound(24);
				play_sound(-61);
				
				x =  get_ran(3,1,4);
				summon_monster(spell == eSpell::SUMMON_SPIRIT ? 125 : 122,caster->cur_loc,
							   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				break;
			case eSpell::DISEASE:
				run_a_missile(l,vict_loc,11,0,24,0,0,80);
				x = get_ran(1,0,2);
				if(targ < 6)
					disease_pc(targ,2 + x);
				else disease_monst(&univ.town.monst[targ - 100],2 + x);
				break;
			case eSpell::HOLY_SCOURGE:
				run_a_missile(l,vict_loc,15,0,24,0,0,80);
				if(targ < 6) {
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
			case eSpell::SMITE:
				run_a_missile(l,vict_loc,6,0,24,0,0,80);
				r1 = get_ran(4,1,6) + 2;
				start_missile_anim();
				damage_target(targ,r1,eDamageType::COLD);
				break;
			case eSpell::STICKS_TO_SNAKES: // sticks to snakes
				play_sound(24);
				r1 = get_ran(1,1,4) + 2;
				for(i = 0; i < r1; i++) {
					play_sound(-61);
					r2 = get_ran(1,0,7);
					x = get_ran(3,1,4);
					summon_monster((r2 == 1) ? 100 : 99,caster->cur_loc,
								   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				}
				break;
			case eSpell::MARTYRS_SHIELD: // martyr's shield
				play_sound(24);
				caster->status[eStatus::MARTYRS_SHIELD] = min(10,caster->status[eStatus::MARTYRS_SHIELD] + 5);
				break;
			case eSpell::SUMMON_HOST: // summon host
				play_sound(24);
				x = get_ran(3,1,4) + 1;
				play_sound(-61);
				summon_monster(126,caster->cur_loc,
							   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				for(i = 0; i < 4; i++)	{
					play_sound(-61);
					if(!summon_monster(125,caster->cur_loc,
									   ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude))
						i = 4;
				}
				break;
			case eSpell::CURSE_ALL: case eSpell::PESTILENCE:
				play_sound(24);
				r1 = get_ran(2,0,2);
				r2 = get_ran(1,0,2);
				if(caster->attitude % 2 == 1)
					for(i = 0; i < 6; i++)
						if(pc_near(i,caster->cur_loc,8)) {
							if(spell == eSpell::CURSE_ALL)
								curse_pc(i,2 + r1);
							if(spell == eSpell::PESTILENCE)
								disease_pc(i,2 + r2);
						}
				for(i = 0; i < univ.town->max_monst(); i++) {
					if((univ.town.monst[i].active != 0) &&
						(((univ.town.monst[i].attitude % 2 == 1) && (caster->attitude % 2 != 1)) ||
						 ((univ.town.monst[i].attitude % 2 != 1) && (caster->attitude % 2 == 1)) ||
						 ((univ.town.monst[i].attitude % 2 == 1) && (caster->attitude != univ.town.monst[i].attitude)))
						&& (dist(caster->cur_loc,univ.town.monst[i].cur_loc) <= 7)) {
						if(spell == eSpell::CURSE_ALL)
							curse_monst(&univ.town.monst[i],2 + r1);
						if(spell == eSpell::PESTILENCE)
							disease_monst(&univ.town.monst[i],2 + r2);
					}
				}
				break;
				
			case eSpell::HEAL_MINOR: case eSpell::HEAL: case eSpell::HEAL_MAJOR: case eSpell::HEAL_ALL:
				play_sound(24);
				switch(spell) {
					case eSpell::HEAL_MINOR: r1 = get_ran(2,1,4) + 2; break;
					case eSpell::HEAL: r1 = get_ran(3,1,6); break;
					case eSpell::HEAL_MAJOR: r1 = get_ran(5,1,6) + 3; break;
					case eSpell::HEAL_ALL: r1 = 50; break;
				}
				caster->health = min(caster->health + r1, caster->m_health);
				break;
			case eSpell::BLESS_PARTY: case eSpell::REVIVE_ALL:
				play_sound(24);
				// TODO: What's r2 for here? Should it be used for Revive All?
				r1 =  get_ran(2,1,4); r2 = get_ran(3,1,6);
				for(i = 0; i < univ.town->max_monst(); i++)
					if((monst_near(i,caster->cur_loc,8,0)) &&
						(caster->attitude == univ.town.monst[i].attitude)) {
						affected = &univ.town.monst[i];
						if(spell == eSpell::BLESS_PARTY)
							curse_monst(affected, -r1);
						if(spell == eSpell::REVIVE_ALL)
							affected->health += r1;
					}
				play_sound(4);
				break;
			case eSpell::FLAMESTRIKE:
				run_a_missile(l,target,2,0,11,0,0,80);
				r1 = 2 + caster->level / 2 + 2;
				start_missile_anim();
				place_spell_pattern(square,target,eDamageType::FIRE,r1,7);
				ashes_loc = target;
				break;
			case eSpell::UNHOLY_RAVAGING:
				run_a_missile(l,vict_loc,14,0,53,0,0,80);
				r1 = get_ran(4,1,8);
				r2 = get_ran(1,0,2);
				damage_target(targ,r1,eDamageType::MAGIC);
				if(targ < 6) {
					slow_pc(targ,6);
					poison_pc(targ,5 + r2);
				}
				else {
					slow_monst(&univ.town.monst[targ - 100],6);
					poison_monst(&univ.town.monst[targ - 100],5 + r2);
				}
				break;
			case eSpell::AVATAR:
				play_sound(24);
				monst_spell_note(caster->number,26);
				caster->health = caster->m_health;
				caster->status[eStatus::BLESS_CURSE] = 8;
				caster->status[eStatus::POISON] = 0;
				caster->status[eStatus::HASTE_SLOW] = 8;
				caster->status[eStatus::WEBS] = 0;
				caster->status[eStatus::DISEASE] = 0;
				caster->status[eStatus::DUMB] = 0;
				caster->status[eStatus::MARTYRS_SHIELD] = 8;
				break;
			case eSpell::DIVINE_THUD:
				run_a_missile(l,target,9,0,11,0,0,80);
				r1 = (caster->level * 3) / 4 + 5;
				if(r1 > 29) r1 = 29;
				start_missile_anim();
				place_spell_pattern(radius2,target,eDamageType::MAGIC,r1,7 );
				ashes_loc = target;
				break;
		}
		
		
	}
	else caster->mp++;
	if(ashes_loc.x > 0)
		univ.town.set_ash(ashes_loc.x,ashes_loc.y,true);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
	
	return acted;
}

void damage_target(short target,short dam,eDamageType type) {
	if(target == 6) return;
	if(target < 6)
		damage_pc(target,dam,type,eRace::UNKNOWN,0);
	else damage_monst(target - 100, 7, dam, 0, type,0);
}


//	target = find_fireball_loc(caster->m_loc,1,(caster->attitude == 1) ? 0 : 1,&target_levels);

//short mode; // 0 - hostile casting  1 - friendly casting
location find_fireball_loc(location where,short radius,short mode,short *m) {
	location check_loc,cast_loc(120,0);
	short cur_lev,level_max = 10;
	
	for(check_loc.x = 1; check_loc.x < univ.town->max_dim() - 1; check_loc.x ++)
		for(check_loc.y = 1; check_loc.y < univ.town->max_dim() - 1; check_loc.y ++)
			if(dist(where,check_loc) <= 8 && can_see(where,check_loc,sight_obscurity) < 5 && sight_obscurity(check_loc.x,check_loc.y) < 5) {
				cur_lev = count_levels(check_loc,radius);
				if(mode == 1)
					cur_lev = cur_lev * -1;
				if( ((cur_lev > level_max) || ((cur_lev == level_max) && (get_ran(1,0,1) == 0)))
					&& (dist(where,check_loc) > radius)) {
					level_max = cur_lev;
					cast_loc = check_loc;
				}
			}
	*m = level_max;
	
	return cast_loc;
}

location closest_pc_loc(location where) {
	short i;
	location pc_where(120,120);
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			if((dist(where,univ.party[i].combat_pos)) < (dist(where,pc_where)))
				pc_where = univ.party[i].combat_pos;
	return pc_where;
}

short count_levels(location where,short radius) {
	short i,store = 0;
	
	for(i = 0; i < univ.town->max_monst(); i++)
		if(monst_near(i,where,radius,0)) {
			if(univ.town.monst[i].attitude % 2 == 1)
				store = store - univ.town.monst[i].level;
			else store = store + univ.town.monst[i].level;
		}
	if(is_combat()) {
		for(i = 0; i < 6; i++)
			if(pc_near(i,where,radius))
				store = store + 10;
	}
	if(is_town())
		if(vdist(where,univ.town.p_loc) <= radius && can_see(where,univ.town.p_loc,sight_obscurity) < 5)
			store += 20;
	
	return store;
}

bool pc_near(short pc_num,location where,short radius) {
	// Assuming not looking
	if(overall_mode >= MODE_COMBAT) {
		if(univ.party[pc_num].main_status == eMainStatus::ALIVE && vdist(univ.party[pc_num].combat_pos,where) <= radius)
			return true;
		else return false;
	}
	if(univ.party[pc_num].main_status == eMainStatus::ALIVE && vdist(univ.town.p_loc,where) <= radius)
		return true;
	else return false;
}

//short active; // 0 - any monst 1 - monster need be active
bool monst_near(short m_num,location where,short radius,short active) {
	if((univ.town.monst[m_num].active != 0) && (vdist(univ.town.monst[m_num].cur_loc,where) <= radius)
		&& ((active == 0) || (univ.town.monst[m_num].active == 2)) )
		return true;
	else return false;
}

void fireball_space(location loc,short dam) {
	place_spell_pattern(square,loc,eDamageType::FIRE,dam,7);
}

//type;  // 0 - take codes in pattern, OW make all nonzero this type
// Types  0 - Null  1 - web  2 - fire barrier  3 - force barrier  4 - force wall  5 - fire wall
//   6 - anti-magic field  7 - stink cloud  8 - ice wall  9 - blade wall  10 - quickfire
//   11 - dispel  12 - sleep field
//  50 + i - 80 :  id6 fire damage  90 + i - 120 : id6 cold damage 	130 + i - 160 : id6 magic dam.
// if prep for anim is true, supporess look checks and go fast
static void place_spell_pattern(effect_pat_type pat,location center,unsigned short type,short who_hit) {
	short i,j,r1,k = 0;
	unsigned short effect;
	location spot_hit;
	location s_loc;
	rectangle active;
	cCreature *which_m;
	bool monster_hit = false;
	
	
	if(type != 0)
		modify_pattern(&pat,type);
	
	
	
	active = univ.town->in_town_rect;
	// eliminate barriers that can't be seen
	for(i = minmax(active.left + 1,active.right - 1,center.x - 4);
		 i <= minmax(active.left + 1,active.right - 1,center.x + 4); i++)
		for(j = minmax(active.top + 1,active.bottom - 1,center.y - 4);
			 j <= minmax(active.top + 1,active.bottom - 1,center.y + 4); j++) {
			s_loc.x = i; s_loc.y = j;
			if(can_see_light(center,s_loc,sight_obscurity) == 5)
				pat.pattern[i - center.x + 4][j - center.y + 4] = 0;
		}
	
	
	// First actually make barriers, then draw them, then inflict damaging effects.
	for(i = minmax(0,univ.town->max_dim() - 1,center.x - 4); i <= minmax(0,univ.town->max_dim() - 1,center.x + 4); i++)
		for(j = minmax(0,univ.town->max_dim() - 1,center.y - 4); j <= minmax(0,univ.town->max_dim() - 1,center.y + 4); j++)
			if(sight_obscurity(i,j) < 5) {
				effect = pat.pattern[i - center.x + 4][j - center.y + 4];
				switch(effect) {
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
	if(is_town()) // now make things move faster if in town
		fast_bang = 2;
	
	// Damage to pcs
	for(k = 0; k < 6; k++)
		for(i = minmax(0,univ.town->max_dim() - 1,center.x - 4); i <= minmax(0,univ.town->max_dim() - 1,center.x + 4); i++)
			for(j = minmax(0,univ.town->max_dim() - 1,center.y - 4); j <= minmax(0,univ.town->max_dim() - 1,center.y + 4); j++) {
				spot_hit.x = i;
				spot_hit.y = j;
				if(sight_obscurity(i,j) < 5 && univ.party[k].main_status == eMainStatus::ALIVE
					&& (((is_combat()) && (univ.party[k].combat_pos == spot_hit)) ||
						((is_town()) && (univ.town.p_loc == spot_hit)))) {
					effect = pat.pattern[i - center.x + 4][j - center.y + 4];
					switch(effect) {
						case WALL_FORCE:
							r1 = get_ran(2,1,6);
							damage_pc(k,r1,eDamageType::MAGIC,eRace::UNKNOWN,0);
							break;
						case WALL_FIRE:
							r1 = get_ran(1,1,6) + 1;
							damage_pc(k,r1,eDamageType::FIRE,eRace::UNKNOWN,0);
							break;
						case WALL_ICE:
							r1 = get_ran(2,1,6);
							damage_pc(k,r1,eDamageType::COLD,eRace::UNKNOWN,0);
							break;
						case WALL_BLADES:
							r1 = get_ran(4,1,8);
							damage_pc(k,r1,eDamageType::WEAPON,eRace::UNKNOWN,0);
							break;
						case OBJECT_BLOCK:
							r1 = get_ran(6,1,8);
							damage_pc(k,r1,eDamageType::WEAPON,eRace::UNKNOWN,0);
							break;
						case BARRIER_CAGE:
							univ.party[k].status[eStatus::FORCECAGE] = 8;
							break;
						default:
							eDamageType type = eDamageType::MARKED;
							unsigned short dice;
							if(effect > 50 && effect <= 80) {
								type = eDamageType::FIRE;
								dice = effect - 50;
							} else if(effect > 90 && effect <= 120) {
								type = eDamageType::COLD;
								dice = effect - 90;
							} else if(effect > 130 && effect <= 160) {
								type = eDamageType::MAGIC;
								dice = effect - 130;
								// The rest of these are new, currently unused.
							} else if(effect > 170 && effect <= 200) {
								type = eDamageType::WEAPON;
								dice = effect - 170;
							} else if(effect > 210 && effect <= 240) {
								type = eDamageType::POISON;
								dice = effect - 210;
							} else if(effect > 250 && effect <= 280) {
								type = eDamageType::UNBLOCKABLE;
								dice = effect - 250;
							} else if(effect > 290 && effect <= 320) {
								type = eDamageType::UNDEAD;
								dice = effect - 290;
							} else if(effect > 330 && effect <= 360) {
								type = eDamageType::DEMON;
								dice = effect - 330;
							}
							if(type == eDamageType::MARKED) break;
							r1 = get_ran(dice,1,6);
							damage_pc(k,r1,type,eRace::UNKNOWN,0);
							break;
					}
				}
			}
	
	fast_bang = 0;
	
	// Damage to monsters
	for(k = 0; k < univ.town->max_monst(); k++)
		if((univ.town.monst[k].active > 0) && (dist(center,univ.town.monst[k].cur_loc) <= 5)) {
			monster_hit = false;
			// First actually make barriers, then draw them, then inflict damaging effects.
			for(i = minmax(0,univ.town->max_dim() - 1,center.x - 4); i <= minmax(0,univ.town->max_dim() - 1,center.x + 4); i++)
				for(j = minmax(0,univ.town->max_dim() - 1,center.y - 4); j <= minmax(0,univ.town->max_dim() - 1,center.y + 4); j++) {
					spot_hit.x = i;
					spot_hit.y = j;
					
					if(!monster_hit && sight_obscurity(i,j) < 5 && monst_on_space(spot_hit,k)) {
						
						if(pat.pattern[i - center.x + 4][j - center.y + 4] > 0)
							monster_hit = true;
						effect = pat.pattern[i - center.x + 4][j - center.y + 4];
						which_m = &univ.town.monst[k];
						if(which_m->abil[eMonstAbil::RADIATE].active && effect == which_m->abil[eMonstAbil::RADIATE].radiate.type)
							continue;
						switch(effect) {
							case FIELD_WEB:
								web_monst(which_m,3);
								break;
							case WALL_FORCE:
								r1 = get_ran(3,1,6);
								damage_monst(k, who_hit, r1,0, eDamageType::MAGIC,0);
								break;
							case WALL_FIRE:
								r1 = get_ran(2,1,6);
								damage_monst(k, who_hit, r1,0, eDamageType::FIRE,0);
								break;
							case CLOUD_STINK:
								curse_monst(which_m,get_ran(1,1,2));
								break;
							case WALL_ICE:
								r1 = get_ran(3,1,6);
								damage_monst(k, who_hit, r1,0, eDamageType::COLD,0);
								break;
							case WALL_BLADES:
								r1 = get_ran(6,1,8);
								damage_monst(k, who_hit, r1,0, eDamageType::WEAPON,0);
								break;
							case CLOUD_SLEEP:
								charm_monst(which_m,0,eStatus::ASLEEP,3);
								break;
							case OBJECT_BLOCK:
								r1 = get_ran(6,1,8);
								damage_monst(k,who_hit,r1,0,eDamageType::WEAPON,0);
								break;
							case BARRIER_CAGE:
								univ.town.monst[k].status[eStatus::FORCECAGE] = 8;
								break;
							default:
								eDamageType type = eDamageType::MARKED;
								unsigned short dice;
								if(effect > 50 && effect <= 80) {
									type = eDamageType::FIRE;
									dice = effect - 50;
								} else if(effect > 90 && effect <= 120) {
									type = eDamageType::COLD;
									dice = effect - 90;
								} else if(effect > 130 && effect <= 160) {
									type = eDamageType::MAGIC;
									dice = effect - 130;
									// The rest of these are new, currently unused.
								} else if(effect > 170 && effect <= 200) {
									type = eDamageType::WEAPON;
									dice = effect - 170;
								} else if(effect > 210 && effect <= 240) {
									type = eDamageType::POISON;
									dice = effect - 210;
								} else if(effect > 250 && effect <= 280) {
									type = eDamageType::UNBLOCKABLE;
									dice = effect - 250;
								} else if(effect > 290 && effect <= 320) {
									type = eDamageType::UNDEAD;
									dice = effect - 290;
								} else if(effect > 330 && effect <= 360) {
									type = eDamageType::DEMON;
									dice = effect - 330;
								}
								if(type == eDamageType::MARKED) break;
								r1 = get_ran(dice,1,6);
								damage_monst(k,who_hit,r1,0,type,0);
								break;
						}
					}
				}
		}
}

void place_spell_pattern(effect_pat_type pat,location center,short who_hit) {
	place_spell_pattern(pat, center, 0, who_hit);
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
		case eDamageType::FIRE: code = 50; break;
		case eDamageType::COLD: code = 90; break;
		case eDamageType::MAGIC: code = 130; break;
			// TODO: These are new; nothing actually uses them, but maybe eventually!
		case eDamageType::WEAPON: code = 170; break;
		case eDamageType::POISON: code = 210; break;
		case eDamageType::UNBLOCKABLE: code = 250; break;
		case eDamageType::UNDEAD: code = 290; break;
		case eDamageType::DEMON: code = 330; break;
	}
	place_spell_pattern(pat, center, code + dice, who_hit);
}

void modify_pattern(effect_pat_type *pat,unsigned short type) {
	short i,j;
	
	for(i = 0; i < 9; i++)
		for(j = 0; j < 9; j++)
			if(pat->pattern[i][j] > 0)
				pat->pattern[i][j] = type;
}

void do_shockwave(location target) {
	short i;
	
	start_missile_anim();
	for(i = 0; i < 6; i++)
		if((dist(target,univ.party[i].combat_pos) > 0) && (dist(target,univ.party[i].combat_pos) < 11)
			&& univ.party[i].main_status == eMainStatus::ALIVE)
			damage_pc(i, get_ran(2 + dist(target,univ.party[i].combat_pos) / 2, 1, 6), eDamageType::UNBLOCKABLE,eRace::UNKNOWN,0);
	for(i = 0; i < univ.town->max_monst(); i++)
		if((univ.town.monst[i].active != 0) && (dist(target,univ.town.monst[i].cur_loc) > 0)
			&& (dist(target,univ.town.monst[i].cur_loc) < 11)
			&& (can_see_light(target,univ.town.monst[i].cur_loc,sight_obscurity) < 5))
			damage_monst(i, current_pc, get_ran(2 + dist(target,univ.town.monst[i].cur_loc) / 2 , 1, 6), 0, eDamageType::UNBLOCKABLE,0);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
}

void radius_damage(location target,short radius, short dam, eDamageType type) {
	short i;
	
	if(is_town()) {
		for(i = 0; i < 6; i++)
			if((dist(target,univ.town.p_loc) > 0) && (dist(target,univ.town.p_loc) <= radius)
				&& univ.party[i].main_status == eMainStatus::ALIVE)
				damage_pc(i, dam, type,eRace::UNKNOWN,0);
		for(i = 0; i < univ.town->max_monst(); i++)
			if((univ.town.monst[i].active != 0) && (dist(target,univ.town.monst[i].cur_loc) > 0)
				&& (dist(target,univ.town.monst[i].cur_loc) <= radius)
				&& (can_see_light(target,univ.town.monst[i].cur_loc,sight_obscurity) < 5))
				damage_monst(i, current_pc, dam, 0, type,0);
		return;
	}
	
	start_missile_anim();
	for(i = 0; i < 6; i++)
		if((dist(target,univ.party[i].combat_pos) > 0) && (dist(target,univ.party[i].combat_pos) <= radius)
			&& univ.party[i].main_status == eMainStatus::ALIVE)
			damage_pc(i, dam, type,eRace::UNKNOWN,0);
	for(i = 0; i < univ.town->max_monst(); i++)
		if((univ.town.monst[i].active != 0) && (dist(target,univ.town.monst[i].cur_loc) > 0)
			&& (dist(target,univ.town.monst[i].cur_loc) <= radius)
			&& (can_see_light(target,univ.town.monst[i].cur_loc,sight_obscurity) < 5))
			damage_monst(i, current_pc, dam, 0, type,0);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
}
// Slightly kludgy way to only damage PCs in space)
void hit_pcs_in_space(location target,short dam,eDamageType type,short report,short hit_all) {
	// TODO: Looks like this no longer does what it claims.
	//short store_active[T_M],i;
	
	//for(i = 0; i < T_M; i++) {
	//	store_active[i] = univ.town.monst[i].active;
	//	univ.town.monst[i].active = 0;
	//}
	hit_space(target, dam,type, report, 10 + hit_all);
	//for(i = 0; i < T_M; i++)
	//	univ.town.monst[i].active = store_active[i];
}

//type; // 0 - weapon   1 - fire   2 - poison   3 - general magic   4 - unblockable  5 - cold
//	6 - demon	7 - undead
//short report; // 0 - no  1 - print result
//hit_all; // 0 - nail top thing   1 - hit all in space  + 10 ... no monsters
void hit_space(location target,short dam,eDamageType type,short report,short hit_all) {
	short i;
	bool stop_hitting = false,hit_monsters = true;
	
	if((target.x < 0) || (target.x > 63) || (target.y < 0) || (target.y > 63))
		return;
	
	if(hit_all >= 10) {
		hit_monsters = false;
		hit_all -= 10;
	}
	
	if(univ.town.is_antimagic(target.x,target.y) && (type == eDamageType::FIRE || type == eDamageType::MAGIC || type == eDamageType::COLD)) {
		return;
	}
	
	if(dam <= 0) {
		add_string_to_buf("  No damage.");
		return;
	}
	
	for(i = 0; i < univ.town->max_monst(); i++)
		if((hit_monsters) && (univ.town.monst[i].active != 0) && !stop_hitting)
			if(monst_on_space(target,i)) {
				if(processing_fields)
					damage_monst(i, 6, dam, 0, type,0);
				else damage_monst(i, (monsters_going) ? 7 : current_pc, dam, 0, type,0);
				stop_hitting = (hit_all == 1) ? false : true;
			}
	
	if(overall_mode >= MODE_COMBAT)
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE && !stop_hitting)
				if(univ.party[i].combat_pos == target) {
					damage_pc(i,dam,type,eRace::UNKNOWN,0);
					stop_hitting = (hit_all == 1) ? false : true;
				}
	if(overall_mode < MODE_COMBAT)
		if(target == univ.town.p_loc) {
			fast_bang = 1;
			hit_party(dam,type);
			fast_bang = 0;
			stop_hitting = (hit_all == 1) ? false : true;
		}
	
	if((report == 1) && (hit_all == 0) && !stop_hitting)
		add_string_to_buf("  Missed.");
	
}



void do_poison() {
	short i,r1 = 0;
	bool some_poison = false;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			if(univ.party[i].status[eStatus::POISON] > 0)
				some_poison = true;
	if(some_poison) {
		add_string_to_buf("Poison:                        ");
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if(univ.party[i].status[eStatus::POISON] > 0) {
					r1 = get_ran(univ.party[i].status[eStatus::POISON],1,6);
					damage_pc(i,r1,eDamageType::POISON,eRace::UNKNOWN,0);
					if(get_ran(1,0,8) < 6)
						move_to_zero(univ.party[i].status[eStatus::POISON]);
					if(get_ran(1,0,8) < 6)
						if(univ.party[i].traits[eTrait::GOOD_CONST])
							move_to_zero(univ.party[i].status[eStatus::POISON]);
					// TODO: Shouldn't the above two conditionals be swapped?
				}
		put_pc_screen();
		//if(overall_mode < 10)
		//	boom_space(univ.univ.party.p_loc,overall_mode,2,r1);
	}
}


void handle_disease() {
	short i,r1 = 0;
	bool disease = false;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			if(univ.party[i].status[eStatus::DISEASE] > 0)
				disease = true;
	
	if(disease) {
		add_string_to_buf("Disease:                        ");
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if(univ.party[i].status[eStatus::DISEASE] > 0) {
					r1 = get_ran(1,1,10);
					switch(r1) {
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
							add_string_to_buf("  " + univ.party[i].name + "unaffected.");
							break;
					}
					r1 = get_ran(1,0,7);
					if(univ.party[i].traits[eTrait::GOOD_CONST])
						r1 -= 2;
					if((get_ran(1,0,7) <= 0) || (pc_has_abil_equip(i,eItemAbil::STATUS_PROTECTION,int(eStatus::DISEASE)) < 24))
						move_to_zero(univ.party[i].status[eStatus::DISEASE]);
				}
		put_pc_screen();
	}
}

void handle_acid() {
	short i,r1 = 0;
	bool some_acid = false;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			if(univ.party[i].status[eStatus::ACID] > 0)
				some_acid = true;
	
	if(some_acid) {
		add_string_to_buf("Acid:                        ");
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if(univ.party[i].status[eStatus::ACID] > 0) {
					r1 = get_ran(univ.party[i].status[eStatus::ACID],1,6);
					damage_pc(i,r1,eDamageType::MAGIC,eRace::UNKNOWN,0);
					move_to_zero(univ.party[i].status[eStatus::ACID]);
				}
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,3,r1,8);
	}
}

bool no_pcs_left() {
	short i = 0;
	
	while(i < 6) {
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			return false;
		i++;
	}
	return true;
	
}

bool hit_end_c_button() {
	bool end_ok = true;
	
	if(which_combat_type == 0) {
		end_ok = out_monst_all_dead();
	}
	for(int i = 0; i < 6; i++) {
		if(univ.party[i].status[eStatus::FORCECAGE] > 0) {
			add_string_to_buf("  Someone trapped.");
			return false;
		}
	}
	
	if(end_ok)
		end_combat();
	return end_ok;
}

bool out_monst_all_dead() {
	short i;
	
	for(i = 0; i < univ.town->max_monst(); i++)
		if((univ.town.monst[i].active > 0) && (univ.town.monst[i].attitude % 2 == 1)) {
			//print_nums(5555,i,univ.town.monst[i].number);
			//print_nums(5555,univ.town.monst[i].m_loc.x,univ.town.monst[i].m_loc.y);
			return false;
		}
	return true;
}

void end_combat() {
	short i;
	
	for(i = 0; i < 6; i++) {
		if(univ.party[i].main_status == eMainStatus::FLED)
			univ.party[i].main_status = eMainStatus::ALIVE;
		univ.party[i].status[eStatus::POISONED_WEAPON] = 0;
		univ.party[i].status[eStatus::BLESS_CURSE] = 0;
		univ.party[i].status[eStatus::HASTE_SLOW] = 0;
	}
	if(which_combat_type == 0) {
		overall_mode = MODE_OUTDOORS;
	}
	combat_active_pc = 6;
	current_pc = store_current_pc;
	if(univ.party[current_pc].main_status != eMainStatus::ALIVE)
		current_pc = first_active_pc();
	put_item_screen(stat_window,0);
	draw_buttons(0);
}


bool combat_cast_mage_spell() {
	short store_sp;
	eSpell spell_num;
	cMonster get_monst;
	
	if(univ.town.is_antimagic(univ.party[current_pc].combat_pos.x,univ.party[current_pc].combat_pos.y)) {
		add_string_to_buf("  Not in antimagic field.");
		return false;
	}
	store_sp = univ.party[current_pc].cur_sp;
	if(univ.party[current_pc].cur_sp == 0)
		add_string_to_buf("Cast: No spell points.        ");
	else if(univ.party[current_pc].skills[eSkill::MAGE_SPELLS] == 0)
		add_string_to_buf("Cast: No mage skill.        ");
	else if(get_encumberance(current_pc) > 1) {
		add_string_to_buf("Cast: Too encumbered.        ");
		take_ap(6);
		give_help(40,0);
		return true;
	}
	else {
		
		
		if(!spell_forced)
			spell_num = pick_spell(current_pc,eSkill::MAGE_SPELLS);
		else {
			if(!repeat_cast_ok(eSkill::MAGE_SPELLS))
				return false;
			spell_num = univ.party[current_pc].last_cast[eSkill::MAGE_SPELLS];
		}
		
		if(spell_num == eSpell::SIMULACRUM) {
			store_sum_monst = pick_trapped_monst();
			if(store_sum_monst == 0)
				return false;
			get_monst = univ.scenario.scen_monsters[store_sum_monst];
			if(store_sp < get_monst.level) {
				add_string_to_buf("Cast: Not enough spell points.        ");
				return false;
			}
			store_sum_monst_cost = get_monst.level;
		}
		
		combat_posing_monster = current_working_monster = current_pc;
		if(spell_num == eSpell::NONE) return false;
		print_spell_cast(spell_num,eSkill::MAGE_SPELLS);
		if((*spell_num).refer == REFER_YES) {
			take_ap(6);
			draw_terrain(2);
			do_mage_spell(current_pc,spell_num);
			combat_posing_monster = current_working_monster = -1;
		}
		else if((*spell_num).refer == REFER_TARGET) {
			start_spell_targeting(spell_num);
		}
		else if((*spell_num).refer == REFER_FANCY) {
			start_fancy_spell_targeting(spell_num);
		}
		else {
			take_ap(6);
			draw_terrain(2);
			combat_immed_mage_cast(current_pc,spell_num);
		}
		put_pc_screen();
	}
	combat_posing_monster = current_working_monster = -1;
	// Did anything actually get cast?
	if(store_sp == univ.party[current_pc].cur_sp)
		return false;
	else return true;
}

void combat_immed_mage_cast(short current_pc, eSpell spell_num, bool freebie) {
	short target, i, num_opp = 0, r1;
	snd_num_t store_sound = 0;
	miss_num_t store_m_type = 0;
	short bonus = freebie ? 1 : stat_adj(current_pc,eSkill::INTELLIGENCE);
	cCreature* which_m;
	start_missile_anim();
	switch(spell_num) {
		case eSpell::SHOCKWAVE:
			if(!freebie)
				univ.party[current_pc].cur_sp -= (*spell_num).cost;
			add_string_to_buf("  The ground shakes!");
			do_shockwave(univ.party[current_pc].combat_pos);
			break;
			
		case eSpell::HASTE_MINOR: case eSpell::HASTE: case eSpell::STRENGTH: case eSpell::ENVENOM: case eSpell::RESIST_MAGIC:
//			target = select_pc(11,0);
			target = store_spell_target;
			if(target < 6) {
				if(!freebie)
					univ.party[current_pc].cur_sp -= (*spell_num).cost;
				play_sound(4);
				std::string c_line = "  " + univ.party[target].name;
				switch(spell_num) {
					case eSpell::ENVENOM:
						c_line += " receives venom.";
						poison_weapon(target,3 + bonus,1);
						store_m_type = 11;
						break;
						
					case eSpell::STRENGTH:
						c_line += " stronger.";
						curse_pc(target, -3);
						store_m_type = 8;
						break;
					case eSpell::RESIST_MAGIC:
						c_line += " resistant.";
						univ.party[target].status[eStatus::MAGIC_RESISTANCE] += 5 + bonus;
						store_m_type = 15;
						break;
						
					default:
						i = (spell_num == eSpell::HASTE_MINOR) ? 2 : max(2,univ.party[current_pc].level / 2 + bonus);
						slow_pc(target, -i);
						c_line += " hasted.";
						store_m_type = 8;
						break;
				}
				add_string_to_buf(c_line);
				add_missile(univ.party[target].combat_pos,store_m_type,0,0,0);
			}
			break;
			
		case eSpell::HASTE_MAJOR: case eSpell::BLESS_MAJOR:
			store_sound = 25;
			if(!freebie)
				univ.party[current_pc].cur_sp -= (*spell_num).cost;
			
			
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE) {
					slow_pc(i, -(spell_num == eSpell::HASTE_MAJOR ? 1 + univ.party[current_pc].level / 8 + bonus : 3 + bonus));
					if(spell_num == eSpell::BLESS_MAJOR) {
						poison_weapon(i,2,1);
						curse_pc(i, -4);
						add_missile(univ.party[i].combat_pos,14,0,0,0);
					}
					else add_missile(univ.party[i].combat_pos,8,0,0,0);
				}
			//play_sound(4);
			if(spell_num == eSpell::HASTE_MAJOR)
				add_string_to_buf("  Party hasted.");
			else
				add_string_to_buf("  Party blessed!");
			
			break;
			
			
			
		case eSpell::SLOW_GROUP: case eSpell::FEAR_GROUP: case eSpell::PARALYSIS_MASS: // affect monsters in area spells
			if(!freebie)
				univ.party[current_pc].cur_sp -= (*spell_num).cost;
			store_sound = 25;
			if(spell_num == eSpell::FEAR_GROUP)
				store_sound = 54;
			switch(spell_num) {
				case eSpell::SLOW_GROUP: add_string_to_buf("  Enemy slowed:"); break;
				case eSpell::RAVAGE_ENEMIES: add_string_to_buf("  Enemy ravaged:"); break;
				case eSpell::FEAR_GROUP: add_string_to_buf("  Enemy scared:"); break;
				case eSpell::PARALYSIS_MASS: add_string_to_buf("  Enemy paralyzed:"); break;
			}
			for(i = 0; i < univ.town->max_monst(); i++) {
				if((univ.town.monst[i].active != 0) && (univ.town.monst[i].attitude % 2 == 1)
					&& (dist(univ.party[current_pc].combat_pos,univ.town.monst[i].cur_loc) <= (*spell_num).range)
					&& (can_see_light(univ.party[current_pc].combat_pos,univ.town.monst[i].cur_loc,sight_obscurity) < 5)) {
					which_m = &univ.town.monst[i];
					switch(spell_num) {
						case eSpell::FEAR_GROUP:
							r1 = get_ran(univ.party[current_pc].level / 3,1,8);
							scare_monst(which_m,r1);
							store_m_type = 10;
							break;
						case eSpell::SLOW_GROUP: case eSpell::RAVAGE_ENEMIES:
							slow_monst(which_m,5 + bonus);
							if(spell_num == eSpell::RAVAGE_ENEMIES)
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
			
		case eSpell::BLADE_AURA: // Pyhrrus effect
			place_spell_pattern(radius2,univ.party[current_pc].combat_pos,WALL_BLADES,6);
			break;
	}
	if(num_opp < 10)
		do_missile_anim((num_opp < 5) ? 50 : 25,univ.party[current_pc].combat_pos,store_sound);
	else play_sound(store_sound);
	end_missile_anim();
}

bool combat_cast_priest_spell() {
	short store_sp;
	eSpell spell_num;
	
	if(univ.town.is_antimagic(univ.party[current_pc].combat_pos.x,univ.party[current_pc].combat_pos.y)) {
		add_string_to_buf("  Not in antimagic field.");
		return false;
	}
	if(!spell_forced)
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
	
	combat_posing_monster = current_working_monster = current_pc;
	
	if(univ.party[current_pc].cur_sp == 0)
		add_string_to_buf("Cast: No spell points.        ");
	else if(spell_num != eSpell::NONE) {
		print_spell_cast(spell_num,eSkill::PRIEST_SPELLS);
		if((*spell_num).refer == REFER_YES) {
			take_ap(5);
			draw_terrain(2);
			do_priest_spell(current_pc,spell_num);
		}
		else if((*spell_num).refer == REFER_TARGET) {
			start_spell_targeting(spell_num);
		}
		else if((*spell_num).refer == REFER_FANCY) {
			start_fancy_spell_targeting(spell_num);
		}
		else {
			take_ap(5);
			draw_terrain(2);
			combat_immed_priest_cast(current_pc, spell_num);
		}
		put_pc_screen();
	}
	
	combat_posing_monster = current_working_monster = -1;
	// Did anything actually get cast?
	if(store_sp == univ.party[current_pc].cur_sp)
		return false;
	else return true;
}

void combat_immed_priest_cast(short current_pc, eSpell spell_num, bool freebie) {
	short target,i,num_opp = 0;
	snd_num_t store_sound = 0;
	miss_num_t store_m_type = 0;
	short bonus = freebie ? 1 : stat_adj(current_pc,eSkill::INTELLIGENCE);
	cCreature *which_m;
	effect_pat_type protect_pat = {{
		{0,1,1,1,1,1,1,1,0},
		{1,5,5,5,5,5,5,5,1},
		{1,5,6,6,6,6,6,5,1},
		{1,5,6,3,3,3,6,5,1},
		{1,5,6,3,3,3,6,5,1},
		{1,5,6,3,3,3,6,5,1},
		{1,5,6,6,6,6,6,5,1},
		{1,5,5,5,5,5,5,5,1},
		{0,1,1,1,1,1,1,1,0}
	}};
	start_missile_anim();
	switch(spell_num) {
		case eSpell::BLESS_MINOR: case eSpell::BLESS:
//			target = select_pc(11,0);
			target = store_spell_target;
			if(target < 6) {
				store_sound = 4;
				if(!freebie)
					univ.party[current_pc].cur_sp -= (*spell_num).cost;
				curse_pc(target,-(spell_num==eSpell::BLESS_MINOR ? 2 : max(2,(univ.party[current_pc].level * 3) / 4 + 1 + bonus)));
				add_missile(univ.party[target].combat_pos,8,0,0,0);
			}
			break;
			
		case eSpell::BLESS_PARTY:
			if(!freebie)
				univ.party[current_pc].cur_sp -= (*spell_num).cost;
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE) {
					curse_pc(i, -(univ.party[current_pc].level / 3));
					add_missile(univ.party[i].combat_pos,8,0,0,0);
				}
			store_sound = 4;
			break;
			
		case eSpell::AVATAR:
			if(!freebie)
				univ.party[current_pc].cur_sp -= (*spell_num).cost;
			add_string_to_buf("  " + univ.party[current_pc].name + " is an avatar!");
			heal_pc(current_pc,200);
			cure_pc(current_pc,8);
			// TODO: Move the heal/cure over to this function as well
			univ.party[current_pc].avatar();
			break;
			
		case eSpell::CURSE_ALL: case eSpell::CHARM_MASS: case eSpell::PESTILENCE:
			if(!freebie)
				univ.party[current_pc].cur_sp -= (*spell_num).cost;
			store_sound = 24;
			for(i = 0; i < univ.town->max_monst(); i++) {
				if((univ.town.monst[i].active != 0) &&(univ.town.monst[i].attitude % 2 == 1) &&
					(dist(univ.party[current_pc].combat_pos,univ.town.monst[i].cur_loc) <= (*spell_num).range)) {
					// TODO: Should this ^ also check that you can see each target? ie can_see_light(...) < 5
					// --> can_see_light(univ.party[current_pc].combat_pos,univ.town.monst[i].cur_loc,sight_obscurity)
					// (The item version of the spell used to check for this, but no longer does since it now defers to here.)
					which_m = &univ.town.monst[i];
					switch(spell_num) {
						case eSpell::CURSE_ALL:
							curse_monst(which_m,3 + bonus);
							store_m_type = 8;
							break;
						case eSpell::CHARM_MASS:
							// TODO: As an item spell, the penalty was 0, though perhaps it was intended to be 8
							// (since 8 was passed as the final argument). Now the penalty has increased to 27.
							// It should probably be put back somehow.
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
			if(!freebie)
				univ.party[current_pc].cur_sp -= (*spell_num).cost;
			play_sound(24);
			add_string_to_buf("  Protective field created.");
			place_spell_pattern(protect_pat,univ.party[current_pc].combat_pos,6);
			break;
	}
	if(num_opp < 10)
		do_missile_anim((num_opp < 5) ? 50 : 25,univ.party[current_pc].combat_pos,store_sound);
	else play_sound(store_sound);
	end_missile_anim();
}

void start_spell_targeting(eSpell num, bool freebie) {
	
	// First, remember what spell was cast.
	spell_being_cast = num;
	spell_freebie = freebie;
	
	add_string_to_buf("  Target spell.");
	if(isMage(num))
		add_string_to_buf("  (Hit 'm' to cancel.)");
	else add_string_to_buf("  (Hit 'p' to cancel.)");
	overall_mode = MODE_SPELL_TARGET;
	current_spell_range = (*num).range;
	current_pat = single;
	
	switch(num) {  // Different spells have different messages and diff. target shapes
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
			current_pat = radius2;
			break;
		case eSpell::PESTILENCE: case eSpell::GOO_BOMB: case eSpell::FOUL_VAPOR:
			current_pat = radius3;
			break;
		case eSpell::WALL_FORCE: case eSpell::WALL_ICE: case eSpell::WALL_BLADES:
			add_string_to_buf("  (Hit space to rotate.)");
			force_wall_position = 0;
			current_pat = field[0];
			break;
	}
}

void start_fancy_spell_targeting(eSpell num, bool freebie) {
	short i;
	location null_loc(120,0);
	
	// First, remember what spell was cast.
	spell_being_cast = num;
	spell_freebie = freebie;
	
	for(i = 0; i < 8; i++)
		spell_targets[i] = null_loc;
	add_string_to_buf("  Target spell.");
	if(isMage(num))
		add_string_to_buf("  (Hit 'm' to cancel.)");
	else add_string_to_buf("  (Hit 'p' to cancel.)");
	add_string_to_buf("  (Hit space to cast.)");
	overall_mode = MODE_FANCY_TARGET;
	current_pat = single;
	current_spell_range = (*num).range;
	
	switch(num) { // Assign special targeting shapes and number of targets
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

void spell_cast_hit_return() {
	
	if(force_wall_position < 10) {
		force_wall_position = (force_wall_position + 1) % 8;
		current_pat = field[force_wall_position];
	}
}

static void process_force_cage(location loc, short i) {
	if(i >= 100) {
		short m = i - 100;
		cCreature& which_m = univ.town.monst[m];
		if(which_m.attitude % 2 == 1 && get_ran(1,1,100) < which_m.mu * 10 + which_m.cl * 4 + 5) {
			// TODO: This sound is not right
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

void process_fields() {
	short i,j,k,r1;
	location loc;
	short qf[64][64];
	rectangle r;
	
	if(is_out())
		return;
	
	if(univ.town.quickfire_present) {
		r = univ.town->in_town_rect;
		for(i = 0; i < univ.town->max_dim(); i++)
			for(j = 0; j < univ.town->max_dim(); j++)
				qf[i][j] = (univ.town.is_quickfire(i,j)) ? 2 : 0;
		for(k = 0; k < ((is_combat()) ? 4 : 1); k++) {
			for(i = r.left + 1; i < r.right ; i++)
				for(j = r.top + 1; j < r.bottom ; j++)
					if(univ.town.is_quickfire(i,j)) {
						r1 = get_ran(1,1,8);
						if(r1 != 1) {
							qf[i - 1][j] = 1;
							qf[i + 1][j] = 1;
							qf[i][j + 1] = 1;
							qf[i][j - 1] = 1;
						}
					}
			for(i = r.left + 1; i < r.right ; i++)
				for(j = r.top + 1; j < r.bottom ; j++)
					if(qf[i][j] > 0) {
						univ.town.set_quickfire(i,j,true);
					}
		}
	}
	
	for(i = 0; i < univ.town->max_monst(); i++)
		if(univ.town.monst[i].active > 0)
			monst_inflict_fields(i);
	
	// First fry PCs, then call to handle damage to monsters
	processing_fields = true; // this, in hit_space, makes damage considered to come from whole party
	for(i = 0; i < univ.town->max_dim(); i++)
		for(j = 0; j < univ.town->max_dim(); j++) {
			if(univ.town.is_force_wall(i,j)) {
				r1 = get_ran(3,1,6);
				loc.x = i; loc.y = j;
				hit_pcs_in_space(loc,r1,eDamageType::MAGIC,1,1);
				r1 = get_ran(1,1,6);
				if(r1 == 2)
					univ.town.set_force_wall(i,j,false);
			}
			if(univ.town.is_fire_wall(i,j)) {
				loc.x = i; loc.y = j;
				r1 = get_ran(2,1,6) + 1;
				hit_pcs_in_space(loc,r1,eDamageType::FIRE,1,1);
				r1 = get_ran(1,1,4);
				if(r1 == 2)
					univ.town.set_fire_wall(i,j,false);
			}
			if(univ.town.is_antimagic(i,j)) {
				r1 = get_ran(1,1,8);
				if(r1 == 2)
					univ.town.set_antimagic(i,j,false);
			}
			if(univ.town.is_scloud(i,j)) {
				r1 = get_ran(1,1,4);
				if(r1 == 2)
					univ.town.set_scloud(i,j,false);
				else {
					scloud_space(i,j);
				}
			}
			if(univ.town.is_sleep_cloud(i,j)) {
				r1 = get_ran(1,1,4);
				if(r1 == 2)
					univ.town.set_sleep_cloud(i,j,false);
				else {
					sleep_cloud_space(i,j);
				}
			}
			if(univ.town.is_ice_wall(i,j)) {
				loc.x = i; loc.y = j;
				r1 = get_ran(3,1,6);
				hit_pcs_in_space(loc,r1,eDamageType::COLD,1,1);
				r1 = get_ran(1,1,6);
				if(r1 == 1)
					univ.town.set_ice_wall(i,j,false);
			}
			if(univ.town.is_blade_wall(i,j)) {
				loc.x = i; loc.y = j;
				r1 = get_ran(6,1,8);
				hit_pcs_in_space(loc,r1,eDamageType::WEAPON,1,1);
				r1 = get_ran(1,1,5);
				if(r1 == 1)
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
		for(i = 0; i < univ.town->max_dim(); i++)
			for(j = 0; j < univ.town->max_dim(); j++)
				if(univ.town.is_quickfire(i,j)) {
					loc.x = i; loc.y = j;
					r1 = get_ran(2,1,8);
					hit_pcs_in_space(loc,r1,eDamageType::FIRE,1,1);
				}
	}
	
	monsters_going = false;
}

void scloud_space(short m,short n) {
	// TODO: Is it correct for these to not affect monsters?
	location target;
	short i;
	
	target.x = (char) m;
	target.y = (char) n;
	
	univ.town.set_scloud(m,n,true);
	
	if(overall_mode >= MODE_COMBAT)
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if(univ.party[i].combat_pos == target) {
					curse_pc(i,get_ran(1,1,2));
				}
	if(overall_mode < MODE_COMBAT)
		if(target == univ.town.p_loc) {
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE)
					curse_pc(i,get_ran(1,1,2));
		}
}

void web_space(short m,short n) {
	location target;
	short i;
	
	target.x = (char) m;
	target.y = (char) n;
	
	univ.town.set_web(m,n,true);
	
	if(overall_mode >= MODE_COMBAT)
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if(univ.party[i].combat_pos == target) {
					web_pc(i,3);
				}
	if(overall_mode < MODE_COMBAT)
		if(target == univ.town.p_loc) {
			for(i = 0; i < 6; i++)
				web_pc(i,3);
		}
}
void sleep_cloud_space(short m,short n) {
	location target;
	short i;
	
	target.x = (char) m;
	target.y = (char) n;
	
	univ.town.set_sleep_cloud(m,n,true);
	
	if(overall_mode >= MODE_COMBAT)
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				if(univ.party[i].combat_pos == target) {
					sleep_pc(i,3,eStatus::ASLEEP,0);
				}
	if(overall_mode < MODE_COMBAT)
		if(target == univ.town.p_loc) {
			for(i = 0; i < 6; i++)
				sleep_pc(i,3,eStatus::ASLEEP,0);
		}
}


void take_m_ap(short num,cCreature *monst) {
	monst->ap = max(0,monst->ap - num);
}

void add_new_action(short pc_num) {
	if(pc_num < 6)
		univ.party[pc_num].ap++;
}

short get_monst_sound(cCreature *attacker,short which_att) {
	short strength;
	eMonstMelee type;
	
	type = attacker->a[which_att].type;
	strength = attacker->a[which_att].sides;
	
	switch(type) {
		case eMonstMelee::SLIME:
			return 11;
		case eMonstMelee::PUNCH:
			return 4;
		case eMonstMelee::CLAW:
			return 9;
		case eMonstMelee::BITE:
			return 10;
			
		default:
			// TODO: These sounds don't quite seem right.
			// They're passed to boom_space, so 0 = ouch, 1 = small sword, 2 = loud sword, 3 = pole, 4 = club
			if(attacker->m_type == eRace::HUMAN) {
				if(strength > 9)
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
	}
	return 0;
}
