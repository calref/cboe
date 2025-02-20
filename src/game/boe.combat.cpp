
#include <cstdio>

#include "boe.global.hpp"

#include "universe/universe.hpp"
#include "boe.monster.hpp"
#include "boe.graphics.hpp"
#include "boe.locutils.hpp"
#include "boe.newgraph.hpp"
#include "boe.infodlg.hpp"
#include "boe.text.hpp"
#include "boe.items.hpp"
#include "boe.party.hpp"
#include "boe.combat.hpp"
#include "sounds.hpp"
#include "boe.town.hpp"
#include "boe.specials.hpp"
#include "boe.graphutil.hpp"
#include "boe.main.hpp"
#include "boe.ui.hpp"
#include "mathutil.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "boe.menus.hpp"
#include "spell.hpp"
#include "tools/prefs.hpp"
#include "utility.hpp"
#include "replay.hpp"

extern eGameMode overall_mode;
extern short which_combat_type;
extern eItemWinMode stat_window;
extern location center;
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
extern short fast_bang;
extern short store_current_pc;
extern short combat_posing_monster , current_working_monster ; // 0-5 PC 100 + x - monster x
extern short missile_firer,current_monst_tactic;
extern cOutdoors::cWandering store_wandering_special;
eSpell spell_being_cast;
bool spell_freebie;
short missile_inv_slot, ammo_inv_slot;
eSpecCtxType spec_target_type;
short spell_caster, spec_target_fail, spec_target_options;
short force_wall_position = 10; //  10 -> no force wall
bool processing_fields = true;
short futzing;
mon_num_t store_sum_monst;
short store_sum_monst_cost;
extern cUniverse univ;

location out_start_loc(20,23);
std::array<short, 51> hit_chance = {
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

void start_outdoor_combat(cOutdoors::cWandering encounter,location where,short num_walls) {
	short how_many,num_tries = 0;
	short low[10] = {15,7,4,3,2,1,1,7,2,1};
	short high[10] = {30,10,6,5,3,2,1,10,4,1};
	rectangle town_rect(0,0,47,47);
	short nums[10];
	
	for(short i = 0; i < 7; i++)
		nums[i] = get_ran(1,low[i],high[i]);
	for(short i = 0; i < 3; i++)
		nums[i + 7] = get_ran(1,low[i + 7],high[i + 7]);
	notify_out_combat_began(encounter,nums);
	print_buf();
	play_sound(23);
	
	mainPtr.setActive();
	which_combat_type = 0;
	overall_mode = MODE_COMBAT;
	
	// Basically, in outdoor combat, we create kind of a 48x48 town for
	// the combat to take place in
	univ.town.prep_arena();
	univ.town->in_town_rect = town_rect;
	
	ter_num_t in_which_terrain = univ.out[where.x][where.y];
	create_out_combat_terrain(in_which_terrain,num_walls,univ.out.is_road(where.x,where.y));
	
	univ.town.monst.clear();
	
	for(short i = 0; i < 7; i++) {
		how_many = nums[i];
		if(encounter.monst[i] != 0)
			for(short j = 0; j < how_many; j++)
				set_up_monst(eAttitude::HOSTILE_A,encounter.monst[i]);
	}
	for(short i = 0; i < 3; i++) {
		how_many = nums[i + 7];
		if(encounter.friendly[i] != 0)
			for(short j = 0; j < how_many; j++)
				set_up_monst(eAttitude::FRIENDLY,encounter.friendly[i]);
	}
	
	// place PCs
	int pc_num = 0;
	for(cPlayer& pc : univ.party) {
		pc.combat_pos = out_start_loc;
		pc.combat_pos.x = pc.combat_pos.x + hor_vert_place[pc_num].x;
		pc.combat_pos.y = pc.combat_pos.y + hor_vert_place[pc_num].y;
		if(get_blockage(univ.town->terrain(pc.combat_pos.x,pc.combat_pos.y)) > 0)
			univ.town->terrain(pc.combat_pos.x,pc.combat_pos.y) = univ.town->terrain(0,0);
		update_explored(pc.combat_pos);
		
		pc.status[eStatus::POISONED_WEAPON] = 0;
		pc.status[eStatus::BLESS_CURSE] = 0;
		pc.status[eStatus::HASTE_SLOW] = 0;
		pc.status[eStatus::INVULNERABLE] = 0;
		pc.status[eStatus::MAGIC_RESISTANCE] = 0;
		pc_num++;
	}
	
	// place monsters, w. friendly monsts landing near PCs
	for(short i = 0; i < univ.town.monst.size(); i++)
		if(univ.town.monst[i].is_alive()) {
			univ.town.monst[i].target = 6;
			
			univ.town.monst[i].cur_loc.x  = get_ran(1,15,25);
			univ.town.monst[i].cur_loc.y  = get_ran(1,14,18);
			if(univ.town.monst[i].attitude == eAttitude::FRIENDLY)
				univ.town.monst[i].cur_loc.y += 9;
			else if((univ.town.monst[i].mu > 0) || (univ.town.monst[i].cl > 0))
				univ.town.monst[i].cur_loc.y -= 4;//max(12,univ.town.monst[i].m_loc.y - 4);
			num_tries = 0;
			while((!monst_can_be_there(univ.town.monst[i].cur_loc,i) ||
					univ.town->terrain(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y) == 180 ||
					univ.target_there(univ.town.monst[i].cur_loc, TARG_PC)) &&
				   (num_tries++ < 50)) {
				univ.town.monst[i].cur_loc.x = get_ran(1,15,25);
				univ.town.monst[i].cur_loc.y = get_ran(1,14,18);
				if(univ.town.monst[i].attitude == eAttitude::FRIENDLY)
					univ.town.monst[i].cur_loc.y += 9;
				else if((univ.town.monst[i].mu > 0) || (univ.town.monst[i].cl > 0))
					univ.town.monst[i].cur_loc.y -= 4;//max(12,univ.town.monst[i].m_loc.y - 4);
			}
			if(get_blockage(univ.town->terrain(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y)) > 0)
				univ.town->terrain(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y) = univ.town->terrain(0,0);
		}
	
	
	combat_active_pc = 6;
	spell_caster = 6; missile_firer = 6;
	for(short i = 0; i < univ.town.monst.size(); i++)
		univ.town.monst[i].target = 6;
	
	for(cPlayer& pc : univ.party) {
		pc.parry = 0;
		pc.last_attacked = nullptr;
	}
	
	univ.town.items.clear();
	
	store_current_pc = univ.cur_pc;
	univ.cur_pc = 0;
	set_pc_moves();
	pick_next_pc();
	center = univ.current_pc().combat_pos;
	UI::toolbar.draw(mainPtr);
	put_pc_screen();
	set_stat_window_for_pc(univ.cur_pc);
	
	adjust_spell_menus();
	
	//clear_map();
	give_help(48,49);
	
}

void start_outdoor_combat(cOutdoors::cCreature encounter,location where,short num_walls) {
	start_outdoor_combat(encounter.what_monst, where, num_walls);
}

bool pc_combat_move(location destination) {
	std::string create_line;
	short s1;
	bool keep_going = true,forced = false,check_f = false;
	location monst_loc,store_loc;
	eDirection dir;
	
	iLiving* monst_hit = univ.target_there(destination, TARG_MONST);
	
	if(monst_hit == nullptr && univ.current_pc().status[eStatus::FORCECAGE] > 0) {
		add_string_to_buf("Move: Can't escape.");
		add_string_to_buf("  (Try doing something else.)");
		return false;
	}
	
	if(monst_hit == nullptr)
		keep_going = check_special_terrain(destination,eSpecCtx::COMBAT_MOVE,univ.current_pc(),&check_f);
	if(check_f)
		forced = true;
	
	if(univ.debug_mode && univ.ghost_mode) forced = true;
	
	if(keep_going) {
		
		dir = set_direction(univ.current_pc().combat_pos, destination);
		
		if((loc_off_act_area(destination)) && (which_combat_type == 1)) {
			add_string_to_buf("Move: Can't leave town during combat.");
			print_buf();
			return true;
		}
		else if(univ.town->terrain(destination.x,destination.y) == 90 && which_combat_type == 0) {
			if(get_ran(1,1,10) < 3) {
				univ.current_pc().main_status = eMainStatus::FLED;
				if(combat_active_pc == univ.cur_pc)
					combat_active_pc = 6;
				create_line = "Moved: Fled.";
				univ.current_pc().ap = 0;
			}
			else {
				take_ap(1);
				create_line = "Moved: Couldn't flee.";
			}
			add_string_to_buf(create_line);
			return true;
		}
		else if(monst_hit != nullptr) {
			bool do_attack = false;
			if(!monst_hit->is_friendly()) do_attack = true;
			else {
				std::string result = cChoiceDlog("attack-friendly",{"cancel","attack"}).show();
				if(result == "cancel") do_attack = false;
				else if(result == "attack") do_attack = true;
			}
			if(do_attack) {
				if(monst_hit->is_friendly())
					make_town_hostile();
				univ.current_pc().last_attacked = monst_hit;
				pc_attack(univ.cur_pc,monst_hit);
				return true;
			}
		}
		else if((monst_hit = univ.target_there(destination, TARG_PC))) {
			if(monst_hit->ap == 0) {
				add_string_to_buf("Move: Can't switch places.");
				add_string_to_buf("  (other PC has no APs)	");
				return false;
			}
			else monst_hit->ap--;
			cPlayer& switch_pc = *dynamic_cast<cPlayer*>(monst_hit);
			add_string_to_buf("Move: Switch places.");
			store_loc = univ.current_pc().combat_pos;
			univ.current_pc().combat_pos = destination;
			switch_pc.combat_pos = store_loc;
			univ.current_pc().direction = dir;
			take_ap(1);
			check_special_terrain(store_loc,eSpecCtx::COMBAT_MOVE,switch_pc,&check_f);
			move_sound(univ.town->terrain(destination.x,destination.y),univ.current_pc().ap);
			return true;
		}
		else if(forced || (!impassable(univ.town->terrain(destination.x,destination.y)) && monst_hit == nullptr)) {
			// Note: monst_hit == nullptr here means there is no PC on the space
			
			// monsters get back-shots
			for(short i = 0; i < univ.town.monst.size(); i++) {
				monst_loc = univ.town.monst[i].cur_loc;
				bool monst_exist = univ.town.monst[i].is_alive();
				
				s1 = univ.cur_pc;
				if(monst_exist && (monst_adjacent(univ.current_pc().combat_pos,i))
					&& !monst_adjacent(destination,i) &&
					!univ.town.monst[i].is_friendly() &&
					univ.town.monst[i].status[eStatus::ASLEEP] <= 0 &&
					univ.town.monst[i].status[eStatus::PARALYZED] <= 0) {
					combat_posing_monster = current_working_monster = 100 + i;
					monster_attack(i,&univ.current_pc());
					combat_posing_monster = current_working_monster = -1;
					draw_terrain(0);
				}
				if(s1 != univ.cur_pc)
					return true;
			}
			
			// move if still alive
			if(univ.current_pc().main_status == eMainStatus::ALIVE) {
				univ.current_pc().combat_pos = destination;
				univ.current_pc().direction = dir;
				take_ap(1);
				create_line += "Moved: ";
				create_line += d_string[dir];
				add_string_to_buf(create_line);
				move_sound(univ.town->terrain(destination.x,destination.y),univ.current_pc().ap);
				
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
	cPlayer& who = univ.current_pc();
	who.parry = (who.ap / 4) * (2 + who.stat_adj(eSkill::DEXTERITY) + who.skill(eSkill::DEFENSE));
	who.ap = 0;
}

void char_stand_ready() {
	univ.current_pc().parry = 100;
	univ.current_pc().ap = 0;
}

// Melee attack a monster
void pc_attack(short who_att,iLiving* target) {
	short r1,r2;
	short hit_adj, dam_adj;
	cPlayer& attacker = univ.party[who_att];
	
	// slice out bad attacks
	if(!attacker.is_alive()) return;
	if(target == nullptr) return;
	if(attacker.status[eStatus::ASLEEP] > 0 || attacker.status[eStatus::PARALYZED] > 0)
		return;
	if(attacker.traits[eTrait::PACIFIST]) {
		add_string_to_buf("Attack: You're a pacifist!");
		print_buf();
		return;
	}
	
	attacker.last_attacked = target;
	
	auto weapons = attacker.get_weapons();
	auto& weap1 = weapons.first;
	auto& weap2 = weapons.second;
	
	hit_adj = (-5 * minmax(-8,8,attacker.status[eStatus::BLESS_CURSE])) + 5 * minmax(-8,8,target->status[eStatus::BLESS_CURSE])
		- attacker.stat_adj(eSkill::DEXTERITY) * 5 + (attacker.total_encumbrance(hit_chance)) * 5;
	
	dam_adj = minmax(-8,8,attacker.status[eStatus::BLESS_CURSE]) - minmax(-8,8,target->status[eStatus::BLESS_CURSE])
		+ attacker.stat_adj(eSkill::STRENGTH);
	
	if(target->status[eStatus::ASLEEP] > 0 || target->status[eStatus::PARALYZED] > 0) {
		hit_adj -= 80;
		dam_adj += 10;
	}
	
	// TODO: These don't stack?
	if(cInvenSlot skill_item = attacker.has_abil_equip(eItemAbil::SKILL)) {
		hit_adj += 5 * (skill_item->abil_strength / 2 + 1);
		dam_adj += skill_item->abil_strength / 2;
	}
	if(cInvenSlot skill_item = attacker.has_abil_equip(eItemAbil::GIANT_STRENGTH)) {
		dam_adj += skill_item->abil_strength;
		hit_adj += skill_item->abil_strength * 2;
	}
	
	attacker.void_sanctuary();
	
	short store_hp = target->get_health();
	
	combat_posing_monster = current_working_monster = who_att;
	
	if(!weap1) {
		std::string create_line = attacker.name + " punches.";
		add_string_to_buf(create_line);
		
		r1 = get_ran(1,1,100) + hit_adj - 20;
		r1 += 5 * (univ.current_pc().status[eStatus::WEBS] / 3);
		if(univ.current_pc().status[eStatus::FORCECAGE] > 0)
			r1 += 3;
		if(target->status[eStatus::FORCECAGE] > 0)
			r1 += 1;
		r2 = get_ran(1,1,4) + dam_adj;
		
		if(cPlayer* pc_target = dynamic_cast<cPlayer*>(target)) {
			// PCs get some additional defensive perks
			// These calculations were taken straight from the monster-on-PC attack code
			r1 += 5 * pc_target->stat_adj(eSkill::DEXTERITY);
			r1 += pc_target->get_prot_level(eItemAbil::EVASION);
			if(pc_target->parry < 100)
				r1 += 5 * pc_target->parry;
		}
		
		if(r1 <= hit_chance[attacker.skill(eSkill::DEXTERITY)]) {
			eDamageType type = eDamageType::WEAPON;
			if(attacker.race == eRace::UNDEAD || attacker.race == eRace::SKELETAL)
				type = eDamageType::UNDEAD;
			else if(attacker.race == eRace::DEMON)
				type = eDamageType::DEMON;
			damage_target(*target, r2, type, 4, true, who_att, attacker.race);
		}
		else {
			draw_terrain(2);
			
			create_line = attacker.name + " misses.";
			add_string_to_buf(create_line);
			play_sound(2);
		}
	}
	
	if(weap1)
		pc_attack_weapon(who_att, *target, hit_adj, dam_adj, *weap1, 1 + bool(weap2), attacker.weap_poisoned == weap1);
	if(weap2 && target->is_alive())
		pc_attack_weapon(who_att, *target, hit_adj, dam_adj, *weap2, 0, attacker.weap_poisoned == weap2);
	move_to_zero(attacker.status[eStatus::POISONED_WEAPON]);
	take_ap(4);
	
	if(store_hp - target->get_health() > 0) {
		if(target->is_shielded()) {
			int how_much = target->get_shared_dmg(store_hp - target->get_health());
			add_string_to_buf("  Shares damage!");
			damage_pc(attacker, how_much, eDamageType::MAGIC,eRace::UNKNOWN);
		}
	}
	combat_posing_monster = current_working_monster = -1;
}

static void apply_weapon_status(eStatus status, int how_much, int dmg, iLiving& which_m, std::string weap_type) {
	switch(status) {
		case eStatus::MAIN: break; // Not a valid status
		case eStatus::INVISIBLE:
			add_string_to_buf("  " + weap_type + " leaks an odd-coloured aura.");
			which_m.apply_status(eStatus::INVISIBLE, how_much / -2);
			break;
		case eStatus::MAGIC_RESISTANCE:
			add_string_to_buf("  " + weap_type + " leaks an odd-coloured aura.");
			which_m.apply_status(eStatus::MAGIC_RESISTANCE, how_much / -2);
			break;
		case eStatus::INVULNERABLE:
			add_string_to_buf("  " + weap_type + " leaks an odd-coloured aura.");
			which_m.apply_status(eStatus::INVULNERABLE, how_much / -2);
			break;
		case eStatus::POISONED_WEAPON:
			add_string_to_buf("  " + weap_type + " leaks an odd-coloured aura.");
			which_m.apply_status(eStatus::POISONED_WEAPON, how_much / -2);
			break;
		case eStatus::POISON:
			add_string_to_buf("  " + weap_type + " drips venom.");
			which_m.poison(how_much / 2);
			break;
		case eStatus::ACID:
			add_string_to_buf("  " + weap_type + " drips acid.");
			which_m.acid(how_much / 2);
			break;
		case eStatus::BLESS_CURSE:
			add_string_to_buf("  " + weap_type + " leaks a dark aura.");
			which_m.curse(how_much / 2);
			break;
		case eStatus::HASTE_SLOW:
			add_string_to_buf("  " + weap_type + " leaks a smoky aura.");
			which_m.slow(how_much / 2);
			break;
		case eStatus::WEBS:
			add_string_to_buf("  " + weap_type + " drips goo.");
			which_m.web(how_much / 2);
			break;
		case eStatus::DISEASE:
			add_string_to_buf("  " + weap_type + " drips bile.");
			which_m.disease(how_much / 2);
			break;
		case eStatus::DUMB:
			add_string_to_buf("  " + weap_type + " leaks a misty aura.");
			which_m.dumbfound(how_much / 2);
			break;
		case eStatus::ASLEEP:
			add_string_to_buf("  " + weap_type + " emits coruscating lights.");
			which_m.sleep(eStatus::ASLEEP, how_much / 2, 20 + dmg);
			break;
		case eStatus::PARALYZED:
			add_string_to_buf("  " + weap_type + " emits a purple flash.");
			which_m.sleep(eStatus::PARALYZED, how_much / 2, 20 + dmg);
			break;
		case eStatus::CHARM:
			add_string_to_buf("  " + weap_type + " leaks a bright aura.");
			which_m.sleep(eStatus::CHARM, 0, 20 + dmg - how_much / 2);
			break;
		case eStatus::FORCECAGE:
			add_string_to_buf("  " + weap_type + " emits a green flash.");
			which_m.sleep(eStatus::FORCECAGE, how_much, dmg - how_much / 2);
			break;
		case eStatus::MARTYRS_SHIELD:
			add_string_to_buf("  " + weap_type + " leaks an odd-coloured aura.");
			which_m.apply_status(eStatus::MARTYRS_SHIELD, how_much / -2);
			break;
	}
	if(dynamic_cast<cPlayer*>(&which_m)) {
		put_pc_screen();
		if(status == eStatus::DUMB)
			adjust_spell_menus();
	}
}

// primary: 0 - secondary weapon, 1 - primary (and only) weapon, 2 - primary of two weapons
void pc_attack_weapon(short who_att,iLiving& target,short hit_adj,short dam_adj,const cItem& weap,short primary,bool do_poison) {
	short r1, r2;
	cPlayer& attacker = univ.party[who_att];
	
	// safety valve
	int skill;
	if(weap.weap_type == eSkill::INVALID)
		skill = attacker.skill(eSkill::EDGED_WEAPONS);
	else if(weap.weap_type == eSkill::MAX_HP)
		skill = 20 * attacker.cur_health / attacker.max_health;
	else if(weap.weap_type == eSkill::MAX_SP)
		skill = 20 * attacker.cur_sp / attacker.max_sp;
	else skill = attacker.skill(weap.weap_type);
	
	std::string create_line = attacker.name + " swings.";
	add_string_to_buf(create_line);
	r1 = get_ran(1,1,100) + hit_adj - 5 * weap.bonus;
	r1 += 5 * (attacker.status[eStatus::WEBS] / 3);
	if(primary) r1 -= 5;
	
	// Ambidextrous?
	if(primary != 1 && !attacker.traits[eTrait::AMBIDEXTROUS])
		r1 += 25;
	
	// Forcecage penalties (reduced for pole weapons)
	if(attacker.status[eStatus::FORCECAGE] > 0)
		r1 += weap.weap_type == eSkill::POLE_WEAPONS ? 1 : 3;
	if(target.status[eStatus::FORCECAGE] > 0)
		r1 += 1;
	
	if(primary) {
		// race adj.
		if(attacker.race == eRace::SLITH && weap.weap_type == eSkill::POLE_WEAPONS)
			r1 -= 10;
	}
	
	r2 = get_ran(1,1,weap.item_level) + dam_adj + weap.bonus;
	if(primary) r2 += 2; else r2 -= 1;
	if(weap.ability == eItemAbil::WEAK_WEAPON)
		r2 = (r2 * (10 - weap.abil_strength)) / 10;
	if(weap.ability == eItemAbil::HP_DAMAGE)
		r2 = std::max<short>(1, double(r2) * double(attacker.cur_health) / double(attacker.max_health));
	if(weap.ability == eItemAbil::SP_DAMAGE)
		r2 = std::max<short>(1, double(r2) * double(attacker.cur_sp) / double(attacker.max_sp));
	if(weap.ability == eItemAbil::HP_DAMAGE_REVERSE)
		r2 += double(weap.abil_strength) * (1.0 - double(attacker.cur_health) / double(attacker.max_health));
	if(weap.ability == eItemAbil::SP_DAMAGE_REVERSE)
		r2 += double(weap.abil_strength) * (1.0 - double(attacker.cur_sp) / double(attacker.max_sp));
	
	if(cPlayer* pc_target = dynamic_cast<cPlayer*>(&target)) {
		// PCs get some additional defensive perks
		// These calculations were taken straight from the monster-on-PC attack code
		r1 += 5 * pc_target->stat_adj(eSkill::DEXTERITY);
		r1 += pc_target->get_prot_level(eItemAbil::EVASION);
		if(pc_target->parry < 100)
			r1 += 5 * pc_target->parry;
	}
	
	if(weap.ability == eItemAbil::EXPLODING_WEAPON) {
		add_string_to_buf("  The weapon produces an explosion!");
		if(get_int_pref("GameSpeed") == 0)
			pause(1);
		else pause(5);
		play_sound(5);
		start_missile_anim();
		place_spell_pattern(radius2, target.get_loc(), weap.abil_data.damage, weap.abil_strength * 2, who_att);
		do_explosion_anim(5,0);
		end_missile_anim();
		handle_marked_damage();
	} else if(r1 <= hit_chance[skill]) {
		eDamageType dmg_tp = eDamageType::SPECIAL;
		short spec_dam = calc_spec_dam(weap.ability,weap.abil_strength,weap.abil_data.value,target,dmg_tp);
		short bonus_dam = 0;
		if(dmg_tp != eDamageType::SPECIAL)
			std::swap(spec_dam, bonus_dam);
		if(primary) {
			bool splits = false;
			if(cCreature* who = dynamic_cast<cCreature*>(&target))
				splits = who->amorphous;
			// assassinate
			r1 = get_ran(1,1,100);
			int assassin = attacker.skill(eSkill::ASSASSINATION);
			if((attacker.level >= target.get_level() - 1) && assassin >= target.get_level() / 2
				&& !splits) // Can't assassinate splitters
				if(r1 < hit_chance[max(assassin - target.get_level(),0)]) {
					add_string_to_buf("  You assassinate.");
					spec_dam += r2;
				}
		}
		snd_num_t dmg_snd = -1, no_dmg = -1;
		size_t i_monst = univ.get_target_i(target);
		if(weap.ability == eItemAbil::HEALING_WEAPON) {
			ASB("  There is a flash of light.");
			target.heal(r2);
		} else switch(weap.weap_type) {
			case eSkill::EDGED_WEAPONS:
				if(weap.item_level < 8)
					dmg_snd = 1;
				else dmg_snd = 2;
				break;
			case eSkill::BASHING_WEAPONS:
				dmg_snd = 4;
				break;
			case eSkill::POLE_WEAPONS:
				dmg_snd = 3;
				break;
			default: // TODO: Not sure what sound to play for unconventional weapons, but let's just go with the generic "ouch" for now
				dmg_snd = 0;
				break;
		}

		// Existing damage values r2, spec_dam, bonus_dam will get adjusted for armor and
		//   resistances, so we need to store the actual amounts of damage done.
		short inflicted_weapon_damage  = 0;
		short inflicted_special_damage = 0;
		short inflicted_bonus_damage   = 0;

		eRace race = attacker.race;
		if(dmg_snd != no_dmg)
			inflicted_weapon_damage  = damage_target(target, r2, eDamageType::WEAPON, dmg_snd, false, who_att, race);
		if(spec_dam)
			inflicted_special_damage = damage_target(target, spec_dam, eDamageType::SPECIAL, 5, false, who_att, race);
		if(bonus_dam)
			inflicted_bonus_damage   = damage_target(target, bonus_dam, dmg_tp, 0, false, who_att, race);

		if(inflicted_weapon_damage || inflicted_special_damage || inflicted_bonus_damage){
			if(cCreature* monst = dynamic_cast<cCreature*>(&target)) {
				monst->damaged_msg(inflicted_weapon_damage, inflicted_special_damage + inflicted_bonus_damage);
			} else if(cPlayer* who = dynamic_cast<cPlayer*>(&target)) {
				std::string msg = "  " + who->name + " takes " + std::to_string(inflicted_weapon_damage);
				if(inflicted_special_damage + inflicted_bonus_damage)
					msg += '+' + std::to_string(inflicted_special_damage + inflicted_bonus_damage);
				add_string_to_buf(msg + '.');
			}
		}
		if(do_poison) {
			// poison
			if(attacker.status[eStatus::POISONED_WEAPON] > 0) {
				short poison_amt = attacker.status[eStatus::POISONED_WEAPON];
				if(attacker.has_abil_equip(eItemAbil::POISON_AUGMENT))
					poison_amt += 2;
				target.poison(poison_amt);
				if(dynamic_cast<cPlayer*>(&target))
					put_pc_screen();
				move_to_zero(attacker.status[eStatus::POISONED_WEAPON]);
			}
		}
		if((weap.ability == eItemAbil::STATUS_WEAPON) && (get_ran(1,0,1) == 1)) {
			apply_weapon_status(weap.abil_data.status, weap.abil_strength, r2 + spec_dam, target, "Blade");
		} else if(weap.ability == eItemAbil::SOULSUCKER && get_ran(1,0,1) == 1) {
			add_string_to_buf("  Blade drains life.");
			attacker.heal(weap.abil_strength / 2);
		} else if(weap.ability == eItemAbil::ANTIMAGIC_WEAPON) {
			short before = target.get_magic();
			int mage = 0, cleric = 0;
			if(cCreature* check = dynamic_cast<cCreature*>(&target))
				mage = check->mu, cleric = check->cl;
			else if(cPlayer* check = dynamic_cast<cPlayer*>(&target))
				mage = check->skill(eSkill::MAGE_SPELLS), cleric = check->skill(eSkill::PRIEST_SPELLS);
			if(mage + cleric > 0 && get_ran(1,0,1) == 1)
				target.drain_sp(weap.abil_strength, true);
			if(before > target.get_magic()) {
				add_string_to_buf("  Blade drains energy.");
				attacker.restore_sp(before / 3);
			}
		} else if(weap.ability == eItemAbil::WEAPON_CALL_SPECIAL) {
			short s1;
			univ.party.force_ptr(21, target.get_loc().x);
			univ.party.force_ptr(22, target.get_loc().y);
			univ.party.force_ptr(20, i_monst);
			run_special(eSpecCtx::ATTACKING_MELEE, eSpecCtxType::SCEN, weap.abil_strength, attacker.combat_pos, &s1);
			if(s1 > 0)
				attacker.ap += 4;
		}
	}
	else {
		draw_terrain(2);
		create_line = "  " + attacker.name + " misses.";
		add_string_to_buf(create_line);
		if(weap.weap_type == eSkill::POLE_WEAPONS)
			play_sound(19);
		else play_sound(2);
	}
}


short calc_spec_dam(eItemAbil abil,short abil_str,short abil_dat,iLiving& monst,eDamageType& dmg_type) {
	short store = 0;
	
	if(abil == eItemAbil::DAMAGING_WEAPON) {
		store += get_ran(abil_str,1,6);
		dmg_type = eDamageType(abil_dat);
		if(dmg_type >= eDamageType::SPECIAL)
			dmg_type = eDamageType::UNBLOCKABLE;
	} else if(abil == eItemAbil::SLAYER_WEAPON) {
		eRace race = eRace::UNKNOWN;
		if(cCreature* who = dynamic_cast<cCreature*>(&monst))
			race = who->m_type;
		else if(cPlayer* who = dynamic_cast<cPlayer*>(&monst))
			race = who->race;
		if(race == eRace::UNKNOWN) return 0;
		// Slith, nephilim, goblins, and vahnatai are affected by humanoid-bane weapons as well as their individual banes
		if(abil_dat == int(eRace::HUMANOID) && isHumanoid(race) && !isHuman(race));
		// Skeletal undead are also affected by normal undead-bane weapons
		else if(abil_dat == int(eRace::UNDEAD) && race == eRace::SKELETAL);
		else if(race != eRace(abil_dat))
			return 0;
		store += abil_str;
		switch(eRace(abil_dat)) {
			case eRace::UNKNOWN:
				// This one shouldn't happen; it's a negative value, which the editor won't allow.
				// But just in case it does happen, we'll neutralize it.
				store = 0;
				break;
			case eRace::DEMON:
			case eRace::GIANT:
			case eRace::STONE:
			case eRace::DRAGON:
				store *= 8;
				break;
			case eRace::UNDEAD:
			case eRace::SKELETAL:
				store *= 6;
				break;
			case eRace::REPTILE:
			case eRace::BEAST:
			case eRace::BIRD:
				store *= 5;
				break;
			case eRace::MAGE:
			case eRace::PRIEST:
			case eRace::MAGICAL:
				store *= 4;
				break;
			case eRace::BUG:
			case eRace::PLANT:
			case eRace::SLIME:
				store *= 7;
				break;
			case eRace::HUMAN:
			case eRace::NEPHIL:
			case eRace::SLITH:
			case eRace::VAHNATAI:
			case eRace::GOBLIN:
			case eRace::HUMANOID:
				store *= 3;
				break;
			case eRace::IMPORTANT:
				// Part of the point of this "race" is to make them immune to slayer abilities.
				// However, a slayer ability made specifically for VIPs shouldn't be useless, either.
				store /= 2;
				break;
		}
	} else if(abil == eItemAbil::CAUSES_FEAR) {
		monst.scare(abil_str * 10);
	}
	return store;
}

void place_target(location target) {
	bool allow_obstructed = false, allow_antimagic = false;
	if(spell_being_cast == eSpell::DISPEL_BARRIER || (spell_being_cast == eSpell::NONE && spec_target_options % 10 == 1))
		allow_obstructed = true;
	if(spell_being_cast == eSpell::NONE && spec_target_options / 10 == 2)
		allow_antimagic = true;
	
 	if(num_targets_left > 0) {
		if(loc_off_act_area(target)) {
			add_string_to_buf("  Space not in town.");
			return;
		}
		if(can_see_light(univ.current_pc().combat_pos,target,sight_obscurity) > 4) {
			add_string_to_buf("  Can't see target.");
			return;
		}
		if(dist(univ.current_pc().combat_pos,target) > current_spell_range)  {
			add_string_to_buf("  Target out of range.");
			return;
		}
		if(!allow_obstructed && sight_obscurity(target.x,target.y) == 5) {
			add_string_to_buf("  Target space obstructed.");
			return;
		}
		if(univ.town.is_antimagic(target.x,target.y) && !allow_antimagic) {
			add_string_to_buf("  Target in antimagic field.");
			return;
		}
		for(short i = 0; i < 8; i++) {
			if(spell_targets[i] == target) {
				add_string_to_buf("  Target removed.");
				num_targets_left++;
				spell_targets[i].x = -1;
				play_sound(-1);
				return;
			}
		}
		for(short i = 0; i < 8; i++)
			if(spell_targets[i].x == -1) {
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
	short adjust,r1,r2,level,bonus = 1;
	snd_num_t store_sound = 0;
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
	mon_num_t summon = 0;
	iLiving* victim;
	cPlayer& caster = univ.current_pc();
	bool allow_obstructed = false, allow_antimagic = false;
	if(spell_being_cast == eSpell::DISPEL_BARRIER || (spell_being_cast == eSpell::NONE && spec_target_options % 10 == 1))
		allow_obstructed = true;
	if(spell_being_cast == eSpell::NONE && spec_target_options / 10 == 2)
		allow_antimagic = false;
	
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
		level = 1 + caster.level / 2;
		bonus = caster.stat_adj(eSkill::INTELLIGENCE);
		if((*spell_being_cast).level <= caster.get_prot_level(eItemAbil::MAGERY))
			level++;
		if(caster.traits[eTrait::ANAMA] && (*spell_being_cast).is_priest())
			level++;
	}
	force_wall_position = 10;
	
	caster.void_sanctuary();
	if(overall_mode == MODE_SPELL_TARGET) {
		spell_targets[0] = target;
	}
	else {
		num_targets = 8;
	}
	
	if(spell_being_cast != eSpell::NONE)
		spell_caster = univ.cur_pc;
	
	// assign monster summoned, if summoning
	if(spell_being_cast == eSpell::SUMMON_BEAST || spell_being_cast == eSpell::SUMMON_WEAK) {
		summon = get_summon_monster(1);
	} else if(spell_being_cast == eSpell::SUMMON || spell_being_cast == eSpell::SUMMON_AID) {
		summon = get_summon_monster(2);
	} else if(spell_being_cast == eSpell::SUMMON_MAJOR || spell_being_cast == eSpell::SUMMON_AID_MAJOR) {
		summon = get_summon_monster(3);
	}
	combat_posing_monster = current_working_monster = univ.cur_pc;
	
	for(short i = 0; i < num_targets; i++)
		if(spell_targets[i].x != -1) {
			target = spell_targets[i];
			spell_targets[i].x = -1; // nullify target as it is used
			
			if(!cost_taken && !freebie && spell_being_cast != eSpell::MINDDUEL && spell_being_cast != eSpell::SIMULACRUM) {
				caster.cur_sp -= (*spell_being_cast).cost;
				cost_taken = true;
			}
			if(!cost_taken && !freebie && spell_being_cast == eSpell::SIMULACRUM) {
				caster.cur_sp -= store_sum_monst_cost;
				cost_taken = true;
			}

			adjust = can_see_light(caster.combat_pos, target, sight_obscurity);
			// TODO: Should we do this here? Or in the handling of targeting modes?
			// (It really depends whether we want to be able to trigger it for targeting something other than a spell.)
			if(adjust <= 4 && !cast_spell_on_space(target, spell_being_cast)) {
				queue_special(eSpecCtx::TARGET, spec_target_type, spec_target_fail, target);
				continue; // The special node intercepted and cancelled regular spell behaviour.
			}
			
			bool failed = spell_being_cast == eSpell::NONE;

			if(adjust > 4) {
				add_string_to_buf("  Can't see target.");
			}
			else if(loc_off_act_area(target)) {
				add_string_to_buf("  Space not in town.");
			}
			else if(dist(caster.combat_pos,target) > current_spell_range)
				add_string_to_buf("  Target out of range.");
			else if(sight_obscurity(target.x,target.y) == 5 && !allow_obstructed)
				add_string_to_buf("  Target space obstructed.");
			else if(univ.town.is_antimagic(target.x,target.y) && !allow_antimagic)
				add_string_to_buf("  Target in antimagic field.");
			else {
				failed = false;
				if(!ap_taken) {
					if(!freebie)
						take_ap(5);
					ap_taken = true;
					draw_terrain(2);
				}
				boom_targ[i] = target;
				bool need_redraw = false;
				switch(spell_being_cast) {
					case eSpell::NONE: // Not a spell but a special node targeting
						r1 = 0;
						run_special(eSpecCtx::TARGET, spec_target_type, spell_caster, target, &r1, nullptr, &need_redraw);
						failed = r1;
						if(need_redraw) redraw_screen(REFRESH_ALL);
						break;
					case eSpell::GOO: case eSpell::WEB: case eSpell::GOO_BOMB:
						place_spell_pattern(current_pat,target,FIELD_WEB,univ.cur_pc);
						break;
					case eSpell::CLOUD_FLAME: case eSpell::CONFLAGRATION:
						place_spell_pattern(current_pat,target,WALL_FIRE,univ.cur_pc);
						break;
					case eSpell::CLOUD_STINK: case eSpell::FOUL_VAPOR:
						place_spell_pattern(current_pat,target,CLOUD_STINK,univ.cur_pc);
						break;
					case eSpell::WALL_FORCE: case eSpell::SHOCKSTORM: case eSpell::FORCEFIELD:
						place_spell_pattern(current_pat,target,WALL_FORCE,univ.cur_pc);
						break;
					case eSpell::WALL_ICE: case eSpell::WALL_ICE_BALL:
						place_spell_pattern(current_pat,target,WALL_ICE,univ.cur_pc);
						break;
					case eSpell::ANTIMAGIC:
						place_spell_pattern(current_pat,target,FIELD_ANTIMAGIC,univ.cur_pc);
						break;
					case eSpell::CLOUD_SLEEP: case eSpell::CLOUD_SLEEP_LARGE:
						place_spell_pattern(current_pat,target,CLOUD_SLEEP,univ.cur_pc);
						break;
					case eSpell::QUICKFIRE:
						univ.town.set_quickfire(target.x,target.y,true);
						break;
					case eSpell::SPRAY_FIELDS:
						r1 = get_ran(1,0,14);
						place_spell_pattern(current_pat,target,spray_type_array[r1],univ.cur_pc);
						break;
					case eSpell::WALL_BLADES:
						place_spell_pattern(current_pat,target,WALL_BLADES,univ.cur_pc);
						break;
					case eSpell::DISPEL_FIELD: case eSpell::DISPEL_SPHERE: case eSpell::DISPEL_SQUARE:
						place_spell_pattern(current_pat,target,FIELD_DISPEL,univ.cur_pc);
						break;
					case eSpell::BARRIER_FIRE:
						play_sound(68);
						r1 = get_ran(3,2,7);
						hit_space(target,r1,eDamageType::FIRE,true,true);
						univ.town.set_fire_barr(target.x,target.y,true);
						if(univ.town.is_fire_barr(target.x,target.y))
							add_string_to_buf("  You create the barrier.");
						else add_string_to_buf("  Failed.");
						break;
					case eSpell::BARRIER_FORCE:
						play_sound(68);
						r1 = get_ran(7,2,7);
						hit_space(target,r1,eDamageType::FIRE,true,true);
						univ.town.set_force_barr(target.x,target.y,true);
						if(univ.town.is_force_barr(target.x,target.y))
							add_string_to_buf("  You create the barrier.");
						else add_string_to_buf("  Failed.");
						break;
						
					default: // spells which involve animations
						start_missile_anim();
						switch(spell_being_cast) {
								
							case eSpell::DIVINE_THUD:
								add_missile(target,9,1,0,0);
								store_sound = 11;
								r1 = min(18,(level * 7) / 10 + 2 * bonus);
								place_spell_pattern(radius2,target,eDamageType::MAGIC,r1,univ.cur_pc);
								ashes_loc = target;
								break;
								
							case eSpell::SPARK: case eSpell::ICE_BOLT:
								r1 = (spell_being_cast == eSpell::SPARK) ? get_ran(2,1,4) : get_ran(min(20,level + bonus),1,4);
								add_missile(target,6,1,0,0);
								do_missile_anim(100,caster.combat_pos,11);
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
								do_missile_anim(100,caster.combat_pos,24);
								hit_space(target,r1,eDamageType::UNBLOCKABLE,1,0);
								break;
							case eSpell::FLAME:
								r1 = get_ran(min(10,1 + level / 3 + bonus),1,6);
								add_missile(target,2,1,0,0);
								do_missile_anim(100,caster.combat_pos,11);
								hit_space(target,r1,eDamageType::FIRE,1,0);
								break;
							case eSpell::FIREBALL: case eSpell::FLAMESTRIKE:
								r1 = min(9,1 + (level * 2) / 3 + bonus) + 1;
								add_missile(target,2,1,0,0);
								store_sound = 11;
								//do_missile_anim(100,caster.combat_pos,11);
								if(spell_being_cast == eSpell::FLAMESTRIKE)
									r1 = (r1 * 14) / 10;
								else if(r1 > 10) r1 = (r1 * 8) / 10;
								if(r1 <= 0) r1 = 1;
								place_spell_pattern(square,target,eDamageType::FIRE,r1,univ.cur_pc);
								ashes_loc = target;
								break;
							case eSpell::FIRESTORM: case eSpell::ICY_RAIN:
								if(spell_being_cast == eSpell::FIRESTORM)
									add_missile(target,2,1,0,0);
								else add_missile(target,6,1,0,0);
								store_sound = 11;
								//do_missile_anim(100,caster.combat_pos,11);
								r1 = min(12,1 + (level * 2) / 3 + bonus) + 2;
								if(r1 > 20)
									r1 = (r1 * 8) / 10;
								if(spell_being_cast == eSpell::FIRESTORM){
									place_spell_pattern(radius2,target,eDamageType::FIRE,r1,univ.cur_pc);
									ashes_loc = target;
								}else place_spell_pattern(radius2,target,eDamageType::COLD,r1,univ.cur_pc);
								break;
							case eSpell::KILL:
								add_missile(target,9,1,0,0);
								do_missile_anim(100,caster.combat_pos,11);
								r1 = get_ran(3,0,10) + caster.level * 2;
								hit_space(target,40 + r1,eDamageType::MAGIC,1,0);
								break;
							case eSpell::ARROWS_DEATH:
								add_missile(target,9,1,0,0);
								store_sound = 11;
								r1 = get_ran(3,0,10) + caster.level + 3 * bonus;
								boom_type[i] = eDamageType::MAGIC;
								boom_dam[i] = r1;
								//hit_space(target,40 + r1,3,1,0);
								break;
								// summoning spells
							case eSpell::SIMULACRUM: case eSpell::SUMMON_BEAST: case eSpell::SUMMON_RAT:
							case eSpell::SUMMON_WEAK: case eSpell::SUMMON: case eSpell::SUMMON_MAJOR:
							case eSpell::DEMON: case eSpell::SUMMON_SPIRIT: case eSpell::STICKS_TO_SNAKES:
							case eSpell::SUMMON_HOST: case eSpell::SUMMON_GUARDIAN:
							case eSpell::SUMMON_AID: case eSpell::SUMMON_AID_MAJOR:
							case eSpell::FLASH_STEP:
								add_missile(target,8,1,0,0);
								do_missile_anim(50,univ.current_pc().combat_pos,61);
								switch(spell_being_cast) {
									case eSpell::SIMULACRUM:
										r2 = get_ran(3,1,4) + caster.stat_adj(eSkill::INTELLIGENCE);
										if(!summon_monster(store_sum_monst,target,r2,eAttitude::FRIENDLY,true))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_BEAST:
										r2 = get_ran(3,1,4) + caster.stat_adj(eSkill::INTELLIGENCE);
										if((summon == 0) || (!summon_monster(summon,target,r2,eAttitude::FRIENDLY,true)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_WEAK:
										r2 = get_ran(4,1,4) + caster.stat_adj(eSkill::INTELLIGENCE);
										if((summon == 0) || (!summon_monster(summon,target,r2,eAttitude::FRIENDLY,true)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON: case eSpell::SUMMON_AID:
										r2 = get_ran(5,1,4) + caster.stat_adj(eSkill::INTELLIGENCE);
										if((summon == 0) || (!summon_monster(summon,target,r2,eAttitude::FRIENDLY,true)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_MAJOR: case eSpell::SUMMON_AID_MAJOR:
										r2 = get_ran(7,1,4) + caster.stat_adj(eSkill::INTELLIGENCE);
										if((summon == 0) || (!summon_monster(summon,target,r2,eAttitude::FRIENDLY,true)))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::DEMON:
										r2 = get_ran(5,1,4) + caster.stat_adj(eSkill::INTELLIGENCE);
										if(!summon_monster(85,target,r2,eAttitude::FRIENDLY,true))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_RAT:
										r1 = get_ran(3,1,4) + caster.stat_adj(eSkill::INTELLIGENCE);
										if(!summon_monster(80,target,r1,eAttitude::FRIENDLY,true))
											add_string_to_buf("  Summon failed.");
										break;
										
									case eSpell::SUMMON_SPIRIT:
										r2 = get_ran(2,1,5) + caster.stat_adj(eSkill::INTELLIGENCE);
										if(!summon_monster(125,target,r2,eAttitude::FRIENDLY,true))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::STICKS_TO_SNAKES:
										r1 = get_ran(1,0,7);
										r2 = get_ran(2,1,5) + caster.stat_adj(eSkill::INTELLIGENCE);
										if(!summon_monster((r1 == 1) ? 100 : 99,target,r2,eAttitude::FRIENDLY,true))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_HOST: // host
										r2 = get_ran(2,1,4) + caster.stat_adj(eSkill::INTELLIGENCE);
										if(!summon_monster((i == 0) ? 126 : 125,target,r2,eAttitude::FRIENDLY,true))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::SUMMON_GUARDIAN: // guardian
										r2 = get_ran(6,1,4) + caster.stat_adj(eSkill::INTELLIGENCE);
										if(!summon_monster(122,target,r2,eAttitude::FRIENDLY,true))
											add_string_to_buf("  Summon failed.");
										break;
									case eSpell::FLASH_STEP:
										if(is_blocked(target))
											add_string_to_buf("  Teleport failed.");
										else {
											add_string_to_buf("  Flash step!");
											caster.combat_pos = target;
											// This can get you out of a forcecage without breaking it
											caster.status[eStatus::FORCECAGE] = 0;
											// Of course, it can also get you into one. If that happens, force cage syncing will fix it.
										}
										break;
									default:
										add_string_to_buf("  Error: Summoning spell " + (*spell_being_cast).name() + " not implemented for combat mode.", 4);
										break;
								}
								break;
								
								
							default:
								if(!(victim = univ.target_there(target)))
									add_string_to_buf("  Nobody there.");
								else {
									cCreature* cur_monst = dynamic_cast<cCreature*>(victim);
									if(cur_monst && cur_monst->is_friendly() && spell_being_cast != eSpell::SCRY_MONSTER && spell_being_cast != eSpell::CAPTURE_SOUL)
										make_town_hostile();
									switch(spell_being_cast) {
										case eSpell::ACID_SPRAY:
											store_m_type = 0;
											victim->acid(level);
											store_sound = 24;
											break;
										case eSpell::PARALYZE_BEAM:
											store_m_type = 9;
											victim->sleep(eStatus::PARALYZED,500,0);
											store_sound = 24;
											break;
										case eSpell::UNHOLY_RAVAGING:
											store_m_type = 14;
											store_sound = 53;
											r1 = get_ran(4,1,8);
											r2 = get_ran(1,0,2);
											damage_monst(*cur_monst, 7, r1, eDamageType::MAGIC);
											victim->slow(4 + r2);
											victim->poison(5 + r2);
											break;
											
										case eSpell::SCRY_MONSTER:
											if(cur_monst == nullptr) {
												add_string_to_buf("  Nobody there.");
												break;
											}
											store_m_type = -1;
											play_sound(52);
											univ.party.m_noted.insert(cur_monst->number);
											adjust_monst_menu();
											display_monst(0,cur_monst,0);
											store_sound = 25;
											break;
										case eSpell::CAPTURE_SOUL:
											if(cur_monst == nullptr) {
												add_string_to_buf("  Nobody there.");
												break;
											}
											store_m_type = 15;
											record_monst(cur_monst);
											store_sound = 25;
											break;
											
										case eSpell::MINDDUEL:
											if(cur_monst == nullptr) {
												// TODO: Support PC-on-PC mindduels? Might be no point though...
												add_string_to_buf("  Nobody there.");
												break;
											}
											store_m_type = -1;
											if((cur_monst->mu == 0) && (cur_monst->cl == 0))
												add_string_to_buf("  Can't duel: no magic.");
											else {
												if(cInvenSlot item = caster.has_abil(eItemAbil::SMOKY_CRYSTAL)) {
													caster.remove_charge(item.slot);
													if(stat_window == univ.cur_pc)
														put_item_screen(stat_window);
													do_mindduel(univ.cur_pc,cur_monst);
												} else add_string_to_buf("  You need a smoky crystal.   ");
											}
											store_sound = 24;
											break;
											
										case eSpell::CHARM_FOE:
											store_m_type = 14;
											victim->sleep(eStatus::CHARM,0,-1 * (bonus + caster.level / 8));
											store_sound = 24;
											break;
											
										case eSpell::DISEASE:
											store_m_type = 0;
											r1 = get_ran(1,0,1);
											victim->disease(2 + r1 + bonus);
											store_sound = 24;
											break;
											
										case eSpell::STRENGTHEN_TARGET:
											store_m_type = 14;
											victim->heal(20);
											store_sound = 55;
											break;
											
										case eSpell::DUMBFOUND:
											store_m_type = 14;
											victim->dumbfound(1 + bonus / 3);
											store_sound = 53;
											break;
											
										case eSpell::SCARE:
											store_m_type = 11;
											victim->scare(get_ran(2 + bonus,1,6));
											store_sound = 54;
											break;
										case eSpell::FEAR:
											store_m_type = 11;
											victim->scare(get_ran(min(20,caster.level / 2 + bonus),1,8));
											store_sound = 54;
											break;
											
										case eSpell::SLOW:
											store_m_type = 11;
											r1 = get_ran(1,0,1);
											victim->slow(2 + r1 + bonus);
											store_sound = 25;
											break;
											
										case eSpell::POISON_MINOR: case eSpell::ARROWS_VENOM:
											store_m_type = (spell_being_cast == eSpell::ARROWS_VENOM) ? 4 : 11;
											victim->poison(2 + bonus / 2);
											store_sound = 55;
											break;
										case eSpell::PARALYZE:
											store_m_type = 9;
											victim->sleep(eStatus::PARALYZED,1000,-10);
											store_sound = 25;
											break;
										case eSpell::POISON:
											store_m_type = 11;
											victim->poison(4 + bonus / 2);
											store_sound = 55;
											break;
										case eSpell::POISON_MAJOR:
											store_m_type = 11;
											victim->poison(8 + bonus / 2);
											store_sound = 55;
											break;
											
										case eSpell::STUMBLE:
											store_m_type = 8;
											victim->curse(4 + bonus);
											store_sound = 24;
											break;
											
										case eSpell::CURSE:
											store_m_type = 8;
											victim->curse(2 + bonus);
											store_sound = 24;
											break;
											
										case eSpell::HOLY_SCOURGE:
											store_m_type = 8;
											victim->curse(2 + caster.level / 2);
											store_sound = 24;
											break;
											
										case eSpell::TURN_UNDEAD: case eSpell::DISPEL_UNDEAD:
											if(cur_monst != nullptr && cur_monst->m_type != eRace::UNDEAD && cur_monst->m_type != eRace::SKELETAL) {
												add_string_to_buf("  Not undead.");
												store_m_type = -1;
												break;
											} else if(cPlayer* who = dynamic_cast<cPlayer*>(victim)) {
												if(who->race != eRace::UNDEAD && who->race != eRace::SKELETAL) {
													add_string_to_buf("  Not undead.");
													store_m_type = -1;
													break;
												}
											}
											store_m_type = 8;
											r1 = get_ran(1,0,90);
											if(r1 > hit_chance[minmax(0,19,bonus * 2 + level * 4 - (victim->get_level() / 2) + 3)])
												add_string_to_buf("  Monster resisted.");
											else {
												r1 = get_ran((spell_being_cast == eSpell::TURN_UNDEAD) ? 2 : 6, 1, 14);
												if(caster.traits[eTrait::ANAMA])
													r1 += 15;
												if(cur_monst != nullptr)
													damage_monst(*cur_monst, univ.cur_pc, r1, eDamageType::UNBLOCKABLE, 0);
												else if(cPlayer* who = dynamic_cast<cPlayer*>(victim))
													damage_pc(*who, r1, eDamageType::UNBLOCKABLE, eRace::UNKNOWN, 0);
											}
											store_sound = 24;
											break;
											
										case eSpell::RAVAGE_SPIRIT:
											if(cur_monst != nullptr && cur_monst->m_type != eRace::DEMON) {
												add_string_to_buf("  Not a demon.");
												store_m_type = -1;
												break;
											} else if(cPlayer* who = dynamic_cast<cPlayer*>(victim)) {
												if(who->race != eRace::DEMON) {
													add_string_to_buf("  Not a demon.");
													store_m_type = -1;
													break;
												}
											}
											r1 = get_ran(1,1,100);
											if(r1 > hit_chance[minmax(0,19,level * 4 - victim->get_level() + 10)])
												add_string_to_buf("  Demon resisted.");
											else {
												r1 = get_ran(8 + bonus * 2, 1, 11);
												if(caster.status[eStatus::DUMB] < 0)
													r1 += -25 * caster.status[eStatus::DUMB] / 3;
												else if(caster.traits[eTrait::ANAMA])
													r1 += 25;
												if(cur_monst != nullptr)
													damage_monst(*cur_monst, univ.cur_pc, r1, eDamageType::UNBLOCKABLE, 0);
												else if(cPlayer* who = dynamic_cast<cPlayer*>(victim))
													damage_pc(*who, r1, eDamageType::UNBLOCKABLE, eRace::UNKNOWN, 0);
											}
											store_sound = 24;
											break;
										default:
											add_string_to_buf("  Error: Spell " + (*spell_being_cast).name() + " not implemented for combat mode.", 4);
											break;
									}
									if(store_m_type >= 0) {
										int w = cur_monst ? cur_monst->x_width : 1;
										int h = cur_monst ? cur_monst->y_width : 1;
										add_missile(target,store_m_type,1, 14 * (w - 1),18 * (h - 1));
									}
									
									if(dynamic_cast<cPlayer*>(victim))
										put_pc_screen();
								}
								break;
						}
				}
			}
			if(failed)
				queue_special(eSpecCtx::TARGET, spec_target_type, spec_target_fail, target);
		}
	
	do_missile_anim((num_targets > 1) ? 35 : 60,caster.combat_pos,store_sound);
	
	bool hit_ashes_loc = false;
	// process mass damage
	for(short i = 0; i < 8; i++){
		if(boom_dam[i] > 0){
			hit_space(boom_targ[i],boom_dam[i],boom_type[i],1,0);
			if(boom_targ[i] == ashes_loc){
				hit_ashes_loc = true;
			}
		}
	}
	
	if(ashes_loc.x > 0){
		// If ashes are going to appear, there'd better be a visible blast on the spot.
		if(!hit_ashes_loc){
			// the last argument is true so this doesn't break RNG of older replays:
			add_explosion(ashes_loc,0,0,get_boom_type(eDamageType::FIRE),1,0,true);
		}

		univ.town.set_ash(ashes_loc.x,ashes_loc.y,true);
	}
	
	do_explosion_anim(5,0);
	
	end_missile_anim();
	
	handle_marked_damage();
	combat_posing_monster = current_working_monster = -1;
	
	print_buf();
}

void handle_marked_damage() {
	for(cPlayer& pc : univ.party)
		if(pc.marked_damage > 0) {
			damage_pc(pc,pc.marked_damage,eDamageType::MARKED,eRace::UNKNOWN);
			pc.marked_damage = 0;
		}
	for(short i = 0; i < univ.town.monst.size(); i++)
		if(univ.town.monst[i].marked_damage > 0) {
			damage_monst(univ.town.monst[i], univ.cur_pc, univ.town.monst[i].marked_damage, eDamageType::MARKED);
			
			univ.town.monst[i].marked_damage = 0;
		}
}

void load_missile() {
	if(univ.current_pc().traits[eTrait::PACIFIST]) {
		add_string_to_buf("Shoot: You're a pacifist!");
		return;
	}
	
	cInvenSlot
		thrown = univ.current_pc().has_type_equip(eItemType::THROWN_MISSILE),
		bow = univ.current_pc().has_type_equip(eItemType::BOW),
		arrow = univ.current_pc().has_type_equip(eItemType::ARROW),
		crossbow = univ.current_pc().has_type_equip(eItemType::CROSSBOW),
		bolts = univ.current_pc().has_type_equip(eItemType::BOLTS),
		no_ammo = univ.current_pc().has_type_equip(eItemType::MISSILE_NO_AMMO);
	
	if(thrown) {
		missile_inv_slot = thrown.slot;
		ammo_inv_slot = thrown.slot;
		add_string_to_buf("Throw: Select a target.");
		add_string_to_buf("  (Hit 's' to cancel.)");
		overall_mode = MODE_THROWING;
		current_spell_range = 8;
		if(thrown->ability == eItemAbil::DISTANCE_MISSILE)
			current_spell_range += thrown->abil_strength;
		if(thrown->ability == eItemAbil::EXPLODING_WEAPON)
			current_pat = radius2;
		else current_pat = single;
	} else if((bow && bolts) || (crossbow && arrow))  {
		add_string_to_buf("Fire: Wrong ammunition.");
	} else if(bow && !arrow) {
		add_string_to_buf("Fire: Equip some arrows.");
	} else if(crossbow && !bolts) {
		add_string_to_buf("Fire: Equip some bolts.");
	} else if(bow && arrow) {
		missile_inv_slot = bow.slot;
		ammo_inv_slot = arrow.slot;
		overall_mode = MODE_FIRING;
		add_string_to_buf("Fire: Select a target.");
		add_string_to_buf("  (Hit 's' to cancel.)");
		current_spell_range = 12;
		if(arrow->ability == eItemAbil::DISTANCE_MISSILE)
			current_spell_range += arrow->abil_strength;
		if(arrow->ability == eItemAbil::EXPLODING_WEAPON)
			current_pat = radius2;
		else current_pat = single;
	} else if(crossbow && bolts) {
		missile_inv_slot = crossbow.slot;
		ammo_inv_slot = bolts.slot;
		overall_mode = MODE_FIRING;
		add_string_to_buf("Fire: Select a target.");
		add_string_to_buf("  (Hit 's' to cancel.)");
		current_spell_range = 12;
		if(bolts->ability == eItemAbil::DISTANCE_MISSILE)
			current_spell_range += bolts->abil_strength;
		if(bolts->ability == eItemAbil::EXPLODING_WEAPON)
			current_pat = radius2;
		else current_pat = single;
	} else if(no_ammo) {
		missile_inv_slot = no_ammo.slot;
		ammo_inv_slot = no_ammo.slot;
		overall_mode = MODE_FIRING;
		add_string_to_buf("Fire: Select a target.");
		add_string_to_buf("  (Hit 's' to cancel.)");
		current_spell_range = 12;
		if(no_ammo->ability == eItemAbil::DISTANCE_MISSILE)
			current_spell_range += no_ammo->abil_strength;
		if(no_ammo->ability == eItemAbil::EXPLODING_WEAPON)
			current_pat = radius2;
		else current_pat = single;
	}
	else add_string_to_buf("Fire: Equip a missile.");
}

void fire_missile(location target) {
	missile_firer = univ.cur_pc; // This may be used by monsters to help pick a target
	short r1, r2, skill, dam, dam_bonus, hit_bonus = 0, range, spec_dam = 0,poison_amt = 0;
	short skill_item;
	iLiving* victim;
	bool exploding = false;
	cPlayer& missile_firer = univ.current_pc();
	cItem& missile = missile_firer.items[missile_inv_slot];
	cItem& ammo = missile_firer.items[ammo_inv_slot];
	
	if(missile.weap_type == eSkill::INVALID)
		skill = missile_firer.skill(eSkill::ARCHERY);
	else if(missile.weap_type == eSkill::MAX_HP)
		skill = 20 * missile_firer.cur_health / missile_firer.max_health;
	else if(missile.weap_type == eSkill::MAX_SP)
		skill = 20 * missile_firer.cur_sp / missile_firer.max_sp;
	else skill = missile_firer.skill(missile.weap_type);
	range = (overall_mode == MODE_FIRING) ? 12 : 8;
	dam = ammo.item_level;
	dam_bonus = ammo.bonus + minmax(-8,8,missile_firer.status[eStatus::BLESS_CURSE]);
	if(overall_mode == MODE_FIRING)
		hit_bonus = missile.bonus;
	hit_bonus += missile_firer.stat_adj(eSkill::DEXTERITY) - can_see_light(missile_firer.combat_pos,target,sight_obscurity)
		+ minmax(-8,8,missile_firer.status[eStatus::BLESS_CURSE]);
	skill_item = missile_firer.get_prot_level(eItemAbil::ACCURACY) / 2;
	hit_bonus += skill_item;
	dam_bonus += skill_item;
	if(ammo.ability == eItemAbil::SEEKING_MISSILE) {
		victim = univ.target_there(target);
		std::set<iLiving*> targets;
		if(victim == nullptr || victim->is_friendly()) {
			for(int i = -1; i <= 1; i++) {
				for(int j = -1; j <= 1; j++) {
					if(i == 0 && j == 0) continue;
					victim = univ.target_there(loc(target.x + i, target.y + j));
					if(victim) {
						bool invisible = victim->status[eStatus::INVISIBLE] > 0;
						if(cCreature* check = dynamic_cast<cCreature*>(victim))
							invisible = invisible || check->invisible;
						bool friendly = victim->is_friendly();
						int seek_chance = 10;
						if(invisible && friendly)
							seek_chance -= 8;
						else if(invisible)
							seek_chance -= 5;
						else if(friendly)
							seek_chance -= 9;
						if(get_ran(1,1,10) <= seek_chance)
							targets.insert(victim);
					}
				}
			}
			if(!targets.empty()) {
				auto iter = targets.begin();
				std::advance(iter, get_ran(1,1,targets.size()) - 1);
				target = (**iter).get_loc();
			}
		} else hit_bonus += 10;
	}
	
	// race adj.
	if(missile_firer.race == eRace::NEPHIL)
		hit_bonus += 2;
	
	if(ammo.ability == eItemAbil::EXPLODING_WEAPON)
		exploding = true;
	
	if(dist(missile_firer.combat_pos,target) > range)
		add_string_to_buf("  Out of range.");
	else if(can_see_light(missile_firer.combat_pos,target,sight_obscurity) >= 5)
		add_string_to_buf("  Can't see target.");
	else {
		// First, some missiles do special things
		if(exploding) {
			take_ap((overall_mode == MODE_FIRING) ? 3 : 2);
			missile_firer.void_sanctuary(); // TODO: Is this right?
			add_string_to_buf("  The arrow explodes!");
			if(get_int_pref("GameSpeed") == 0)
				pause(dist(missile_firer.combat_pos,target));
			else
				pause(dist(missile_firer.combat_pos,target)*5);
			run_a_missile(missile_firer.combat_pos,target,2,1,5,0,0,100);
			start_missile_anim();
			place_spell_pattern(radius2,target, ammo.abil_data.damage,ammo.abil_strength * 2, univ.cur_pc);
			do_explosion_anim(5,0);
			end_missile_anim();
			handle_marked_damage();
		} else {
			combat_posing_monster = current_working_monster = univ.cur_pc;
			draw_terrain(2);
			missile_firer.void_sanctuary();
			take_ap((overall_mode == MODE_FIRING) ? 3 : 2);
			r1 = get_ran(1,1,100) - 5 * hit_bonus - 10;
			r1 += 5 * (missile_firer.status[eStatus::WEBS] / 3);
			r2 = get_ran(1,1,dam) + dam_bonus;
			if(ammo.ability == eItemAbil::WEAK_WEAPON)
				r2 = (r2 * (10 - ammo.abil_strength)) / 10;
			if(ammo.ability == eItemAbil::HP_DAMAGE)
				r2 = std::max<short>(1, double(r2) * double(missile_firer.cur_health) / double(missile_firer.max_health));
			if(ammo.ability == eItemAbil::SP_DAMAGE)
				r2 = std::max<short>(1, double(r2) * double(missile_firer.cur_sp) / double(missile_firer.max_sp));
			if(ammo.ability == eItemAbil::HP_DAMAGE_REVERSE)
				r2 += double(ammo.abil_strength) * (1.0 - double(missile_firer.cur_health) / double(missile_firer.max_health));
			if(ammo.ability == eItemAbil::SP_DAMAGE_REVERSE)
				r2 += double(ammo.abil_strength) * (1.0 - double(missile_firer.cur_sp) / double(missile_firer.max_sp));
			std::string create_line = missile_firer.name + " fires.";
			add_string_to_buf(create_line);
			
			miss_num_t m_type = ammo.missile;
			run_a_missile(missile_firer.combat_pos,target,m_type,1,(overall_mode == MODE_FIRING) ? 12 : 14,
						  0,0,100);
			
			if(r1 > hit_chance[skill])
				add_string_to_buf("  Missed.");
			else if((victim = univ.target_there(target))) {
				eDamageType dmg_tp = eDamageType::SPECIAL;
				spec_dam = calc_spec_dam(ammo.ability,ammo.abil_strength,ammo.abil_data.value,*victim,dmg_tp);

				// Existing damage values r2, spec_dam will get adjusted for armor and
				//   resistances, so we need to store the actual amounts of damage done.
				short inflicted_weapon_damage  = 0;
				short inflicted_special_damage = 0;

				if(ammo.ability == eItemAbil::HEALING_WEAPON) {
					ASB("  There is a flash of light.");
					victim->heal(r2);
				} else if(cCreature* monst = dynamic_cast<cCreature*>(victim)) {
					inflicted_weapon_damage = damage_monst(*monst, univ.cur_pc, r2, eDamageType::WEAPON,13,false);
					if(spec_dam > 0)
						inflicted_special_damage = damage_monst(*monst, univ.cur_pc, spec_dam, dmg_tp, 0,false);
					if(inflicted_weapon_damage || inflicted_special_damage)
						monst->damaged_msg(inflicted_weapon_damage, inflicted_special_damage);
				} else if(cPlayer* who = dynamic_cast<cPlayer*>(victim)) {
					// TODO: Should the race really be included here? Maybe it's meant for melee attacks only.
					eRace race = missile_firer.race;
					inflicted_weapon_damage = damage_pc(*who, r2, eDamageType::WEAPON, race, 0, false);
					if(spec_dam > 0)
						inflicted_special_damage = damage_pc(*who, spec_dam, dmg_tp, race, 0, false);
					if(inflicted_weapon_damage || inflicted_special_damage) {
						std::string msg = "  " + who->name + " takes " + std::to_string(inflicted_weapon_damage);
						if(spec_dam) msg += '+' + std::to_string(inflicted_special_damage);
						add_string_to_buf(msg + '.');
					}
				}
				// poison
				if(missile_firer.status[eStatus::POISONED_WEAPON] > 0 && missile_firer.weap_poisoned.slot == ammo_inv_slot) {
					poison_amt = missile_firer.status[eStatus::POISONED_WEAPON];
					if(missile_firer.has_abil_equip(eItemAbil::POISON_AUGMENT))
						poison_amt++;
					victim->poison(poison_amt);
					if(dynamic_cast<cPlayer*>(victim))
						put_pc_screen();
				}
				if((ammo.ability == eItemAbil::STATUS_WEAPON) && (get_ran(1,0,1) == 1)) {
					apply_weapon_status(ammo.abil_data.status, ammo.abil_strength, r2 + spec_dam, *victim, "Missile");
				} else if(ammo.ability == eItemAbil::SOULSUCKER && get_ran(1,0,1) == 1) {
					add_string_to_buf("  Missile drains life.");
					missile_firer.heal(ammo.abil_strength / 2);
				} else if(ammo.ability == eItemAbil::ANTIMAGIC_WEAPON) {
					short before = victim->get_magic();
					int mage = 0, cleric = 0;
					if(cCreature* check = dynamic_cast<cCreature*>(victim))
						mage = check->mu, cleric = check->cl;
					else if(cPlayer* check = dynamic_cast<cPlayer*>(victim))
						mage = check->skill(eSkill::MAGE_SPELLS), cleric = check->skill(eSkill::PRIEST_SPELLS);
					if(mage + cleric > 0 && get_ran(1,0,1) == 1)
						victim->drain_sp(ammo.abil_strength, true);
					if(before > victim->get_magic()) {
						add_string_to_buf("  Missile drains energy.");
						missile_firer.restore_sp((before - victim->get_magic()) / 3);
					}
				} else if(ammo.ability == eItemAbil::WEAPON_CALL_SPECIAL) {
					// TODO: Should this be checked on the missile as well as on the ammo? (Provided they're different.)
					short s1;
					univ.party.force_ptr(21, victim->get_loc().x);
					univ.party.force_ptr(22, victim->get_loc().y);
					univ.party.force_ptr(20, univ.get_target_i(*victim));
					run_special(eSpecCtx::ATTACKING_RANGE, eSpecCtxType::SCEN, missile.abil_strength, missile_firer.combat_pos, &s1);
					if(s1 > 0)
						missile_firer.ap += (overall_mode == MODE_FIRING) ? 3 : 2;
				}
				if(cCreature* monst = dynamic_cast<cCreature*>(victim)) {
					if(monst->abil[eMonstAbil::HIT_TRIGGER].active) {
					short s1;
					univ.party.force_ptr(21, monst->cur_loc.x);
					univ.party.force_ptr(22, monst->cur_loc.y);
					univ.party.force_ptr(20, univ.get_target_i(*monst));
						run_special(eSpecCtx::ATTACKED_RANGE, eSpecCtxType::SCEN, monst->abil[eMonstAbil::HIT_TRIGGER].special.extra1, missile_firer.combat_pos, &s1);
					if(s1 > 0)
						missile_firer.ap += (overall_mode == MODE_FIRING) ? 3 : 2;
					}
				} else if(cPlayer* pc = dynamic_cast<cPlayer*>(victim)) {
					if(cInvenSlot spec_item = pc->has_abil_equip(eItemAbil::HIT_CALL_SPECIAL)) {
						short s1;
						univ.party.force_ptr(21, pc->combat_pos.x);
						univ.party.force_ptr(22, pc->combat_pos.y);
						univ.party.force_ptr(20, univ.get_target_i(*pc));
						run_special(eSpecCtx::ATTACKED_RANGE, eSpecCtxType::SCEN, spec_item->abil_strength, missile_firer.combat_pos, &s1);
						if(s1 > 0)
							missile_firer.ap += (overall_mode == MODE_FIRING) ? 3 : 2;
					}
				}
			}
		}
		
		if(ammo.variety != eItemType::MISSILE_NO_AMMO) {
			if(ammo.ability != eItemAbil::RETURNING_MISSILE)
				ammo.charges--;
			else ammo.charges = 1;
			if(missile_firer.has_abil_equip(eItemAbil::DRAIN_MISSILES) && ammo.ability != eItemAbil::RETURNING_MISSILE)
				ammo.charges--;
			if(ammo.charges <= 0)
				missile_firer.take_item(ammo_inv_slot);
			if(univ.cur_pc == stat_window)
				put_item_screen(stat_window);
		}
	}
	
	if(!exploding){
		combat_posing_monster = current_working_monster = -1;
		move_to_zero(missile_firer.status[eStatus::POISONED_WEAPON]);
	}
	print_buf();
}

static bool sync_force_cages() {
	static const int fc_multipliers[10] = {1, 1, 1, 1, 2, 2, 2, 3, 3, 4};
	bool was_change = false;
	// This goes through the list of creatures and places forcecage barriers on any spaces containing someone with forcecage status.
	// If anyone is on a forcecage barrier but doesn't have forcecage status, they are given it.
	for(cPlayer& who : univ.party) {
		location loc = who.get_loc();
		if(who.status[eStatus::FORCECAGE] > 0) {
			was_change = true;
			univ.town.set_force_cage(loc.x, loc.y, true);
		} else if(univ.town.is_force_cage(loc.x, loc.y) && who.status[eStatus::FORCECAGE] == 0) {
			was_change = true;
			who.status[eStatus::FORCECAGE] = get_ran(2, 2, 7) * fc_multipliers[get_ran(1,1,10)];
		}
	}
	for(int i = 0; i < univ.town.monst.size(); i++) {
		cCreature& who = univ.town.monst[i];
		location loc = who.get_loc();
		if(who.status[eStatus::FORCECAGE] > 0) {
			was_change = true;
			univ.town.set_force_cage(loc.x, loc.y, true);
		} else if(univ.town.is_force_cage(loc.x, loc.y) && who.status[eStatus::FORCECAGE] == 0) {
			was_change = true;
			who.status[eStatus::FORCECAGE] = get_ran(2, 2, 7) * fc_multipliers[get_ran(1,1,10)];
		}
	}
	return was_change;
}

// Select next active PC and, if necessary, run monsters
// if monsters go or PC switches (i.e. if need redraw above), return true
bool combat_next_step() {
	bool to_return = false;
	short store_pc; // will print current pc name is active pc changes
	
	to_return = sync_force_cages();
	
	store_pc = univ.cur_pc;
	while(pick_next_pc()) {
		combat_run_monst();
		set_pc_moves();
		if((combat_active_pc < 6) && (univ.party[combat_active_pc].ap == 0)){
			combat_active_pc = 6;
			ASB("The active character is unable to act! The whole party is now active.", 2);
		}
		to_return = true;
		// Safety valve
		if(!univ.party.is_alive())
			return true;
	}
	pick_next_pc();
	if(univ.cur_pc != store_pc)
		to_return = true;
	center = univ.current_pc().combat_pos;
	
	adjust_spell_menus();
	
	// In case running monsters affected active PC...
/*	if(univ.party[current_pc].status[3] < 0)
		this_pc_hasted = false;
	if((univ.party[current_pc].main_status != 1) ||
		((univ.party[current_pc].status[3] < 0) && (univ.party.age % 2 == 0)))
		pick_next_pc();
	center = univ.party[current_pc].combat_pos;		*/
	
	if((combat_active_pc == 6) && (univ.cur_pc != store_pc)) {
		std::ostringstream create_line;
		create_line << "Active:  " << univ.current_pc().name;
		create_line << " (#" << univ.cur_pc + 1 << ", " << univ.current_pc().ap << " ap.)";
		add_string_to_buf(create_line.str());
		print_buf();
	}
	if((univ.cur_pc != store_pc) || (to_return)) {
		put_pc_screen();
		set_stat_window_for_pc(univ.cur_pc);
	}
	return to_return;
}

// Find next active PC, return true if monsters need running
bool pick_next_pc() {
	bool store = false;
	
	// TODO: cur_pc should never be 6
	if(univ.cur_pc == 6)
		univ.cur_pc = 0;
	
	// If current pc isn't active, fry its moves
	if((combat_active_pc < 6) && (combat_active_pc != univ.cur_pc))
		univ.current_pc().ap = 0;
	
	// Find next PC with moves
	while((univ.current_pc().ap <= 0) && (univ.cur_pc < 6)) {
		univ.cur_pc++;
		if((combat_active_pc < 6) && (combat_active_pc != univ.cur_pc))
			univ.current_pc().ap = 0;
	}
	
	// If run out of PC's, return to start and try again
	if(univ.cur_pc == 6) {
		univ.cur_pc = 0;
		while((univ.current_pc().ap <= 0) && (univ.cur_pc < 6)) {
			univ.cur_pc++;
			if((combat_active_pc < 6) && (combat_active_pc != univ.cur_pc))
				univ.current_pc().ap = 0;
		}
		if(univ.cur_pc == 6) {
			store = true;
			univ.cur_pc = 0;
		}
	}
	
	return store;
}


void combat_run_monst() {
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
	
	if(is_town() && univ.town->lighting_type != LIGHT_NORMAL) {
		int radiance = 0;
		for(cPlayer& pc : univ.party)
			radiance += pc.get_prot_level(eItemAbil::RADIANT);
		if(radiance > 0 && univ.party.light_level < radiance && get_ran(1,1,10) < radiance) {
			ASB("One of your items is glowing softly!");
			univ.party.light_level += radiance * 3;
		}
	}
	
	move_to_zero(univ.party.status[ePartyStatus::DETECT_LIFE]);
	move_to_zero(univ.party.status[ePartyStatus::FIREWALK]);
	
	// decrease monster present counter
	move_to_zero(univ.party.hostiles_present);
	
	dump_gold(1);
	
	univ.party.age++;
	if(univ.party.age % 4 == 0)
		for(cPlayer& pc : univ.party) {
			if(pc.status[eStatus::BLESS_CURSE] != 0 || pc.status[eStatus::HASTE_SLOW] != 0)
				update_stat = true;
			move_to_zero(pc.status[eStatus::BLESS_CURSE]);
			move_to_zero(pc.status[eStatus::HASTE_SLOW]);
			move_to_zero(univ.party.status[ePartyStatus::STEALTH]);
			if(cInvenSlot item = pc.has_abil_equip(eItemAbil::REGENERATE)) {
				update_stat = true;
				pc.heal(get_ran(1,0,item->item_level + 1));
			}
		}
	for(cPlayer& pc : univ.party)
		if(pc.main_status == eMainStatus::ALIVE) {
			if(pc.status[eStatus::INVULNERABLE] != 0 || pc.status[eStatus::MAGIC_RESISTANCE] != 0
			   || pc.status[eStatus::INVISIBLE] != 0 || pc.status[eStatus::MARTYRS_SHIELD] != 0
			   || pc.status[eStatus::ASLEEP] != 0 || pc.status[eStatus::PARALYZED] != 0)
				update_stat = true;
			
			move_to_zero(pc.status[eStatus::INVULNERABLE]);
			move_to_zero(pc.status[eStatus::MAGIC_RESISTANCE]);
			move_to_zero(pc.status[eStatus::INVISIBLE]);
			move_to_zero(pc.status[eStatus::MARTYRS_SHIELD]);
			move_to_zero(pc.status[eStatus::ASLEEP]);
			move_to_zero(pc.status[eStatus::PARALYZED]);
			
			// Do special items
			for(const cItem& item : pc.items) {
				if(item.ability != eItemAbil::OCCASIONAL_STATUS) continue;
				if(item.abil_group()) continue; // Affects whole party, which is handled elsewhere
				if(get_ran(1,0,10) != 5) continue;
				int how_much = item.abil_strength;
				if(item.abil_harms()) how_much *= -1;
				eStatus status = item.abil_data.status;
				if((*status).isNegative)
					how_much *= -1;
				switch(status) {
					case eStatus::MAIN: case eStatus::CHARM:
						continue; // Not valid in this context.
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
						else if(pc.status[eStatus::POISON] > 0)
							add_string_to_buf("An item cures you!");
						break;
					case eStatus::INVULNERABLE:
					case eStatus::MAGIC_RESISTANCE:
					case eStatus::INVISIBLE:
					case eStatus::MARTYRS_SHIELD:
						if(how_much > 0) add_string_to_buf("An item protects you!");
						else add_string_to_buf("An item makes you vulnerable!");
						break;
					case eStatus::DISEASE:
						if(how_much > 0) add_string_to_buf("An item diseases you!");
						else if(pc.status[eStatus::DISEASE] > 0)
							add_string_to_buf("An item cures you!");
						break;
					case eStatus::DUMB:
						if(how_much > 0) add_string_to_buf("An item clouds your mind!");
						else if(pc.status[eStatus::DUMB] > 0)
							add_string_to_buf("An item clears your mind!");
						else add_string_to_buf("An item enlightens you!");
						break;
					case eStatus::WEBS:
						if(how_much > 0) add_string_to_buf("An item constricts you!");
						else if(pc.status[eStatus::WEBS] > 0)
							add_string_to_buf("An item cleanses you!");
						break;
					case eStatus::ASLEEP:
						if(how_much > 0) add_string_to_buf("An item knocks you out!");
						else if(pc.status[eStatus::ASLEEP] > 0)
							add_string_to_buf("An item wakes you!");
						else add_string_to_buf("An item makes you restless!");
						break;
					case eStatus::PARALYZED:
						if(how_much > 0) add_string_to_buf("An item paralyzes you!");
						else if(pc.status[eStatus::PARALYZED] > 0)
							add_string_to_buf("An item restores your movement!");
						break;
					case eStatus::ACID:
						if(how_much > 0) add_string_to_buf("An item covers you in acid!");
						else if(pc.status[eStatus::ACID] > 0)
							add_string_to_buf("An item neutralizes the acid!");
						break;
					case eStatus::FORCECAGE:
						if(how_much > 0) add_string_to_buf("An item entraps you!");
						else if(-how_much > pc.status[eStatus::FORCECAGE])
							add_string_to_buf("An item frees you!");
						else add_string_to_buf("An item weakens the barrier!");
						break;
					case eStatus::POISONED_WEAPON:
						if(how_much > 0) {
							if(pc.status[eStatus::POISONED_WEAPON] <= 0) {
								// Need to figure out which weapon to apply to.
								auto weap = std::find_if(pc.items.begin(), pc.items.end(), is_poisonable_weap);
								if(weap != pc.items.end())
									pc.weap_poisoned.slot = weap - pc.items.begin();
								else continue;
								add_string_to_buf("An item poisons your weapon!");
							} else add_string_to_buf("An item augments your weapon poison!");
						} else add_string_to_buf("An item clears the poison from your weapon!");
						break;
				}
				// Note: Since this is an item you're wearing, it bypasses any resistance you might have to the status effect.
				// Thus we just call apply_status instead of all the various status-specific calls.
				pc.apply_status(status, how_much);
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
	short num_monst, target,r1,move_target;
	cCreature *cur_monst;
	bool pc_adj[6];
	
	monsters_going = true; // This affects how graphics are drawn.
	
	num_monst = univ.town.monst.size();
	if(!is_combat())
		which_combat_type = 1;
	
	for(short i = 0; i < num_monst; i++) {  // Give monsters ap's, check activity
		
		cur_monst = &univ.town.monst[i];
		
		// See if hostile monster notices party, during combat
		if(cur_monst->active == eCreatureStatus::IDLE && !cur_monst->is_friendly() && overall_mode == MODE_COMBAT) {
			r1 = get_ran(1,1,100); // Check if see PCs first
			// TODO: Hang on, isn't stealth supposed to get better as you level up?
			r1 += (univ.party.status[ePartyStatus::STEALTH] > 0) ? 45 : 0;
			r1 += can_see_light(cur_monst->cur_loc,closest_pc_loc(cur_monst->cur_loc),sight_obscurity) * 10;
			if(r1 < 50)
				cur_monst->active = eCreatureStatus::ALERTED;
			
			for(short j = 0; j < univ.town.monst.size(); j++)
				if(monst_near(j,cur_monst->cur_loc,5,1)) {
					cur_monst->active = eCreatureStatus::ALERTED;
				}
		}
		if(cur_monst->active == eCreatureStatus::IDLE && !cur_monst->is_friendly()) {
			// Now it looks for PC-friendly monsters
			// dist check is for efficiency
			for(short j = 0; j < univ.town.monst.size(); j++)
				if((univ.town.monst[j].is_alive()) &&
					univ.town.monst[j].is_friendly() &&
					(dist(cur_monst->cur_loc,univ.town.monst[j].cur_loc) <= 6) &&
					(can_see_light(cur_monst->cur_loc,univ.town.monst[j].cur_loc,sight_obscurity) < 5))
					cur_monst->active = eCreatureStatus::ALERTED;
		}
		
		// See if friendly, fighting monster see hostile monster. If so, make mobile
		// dist check is for efficiency
		if(cur_monst->active == eCreatureStatus::IDLE && cur_monst->attitude == eAttitude::FRIENDLY) {
			for(short j = 0; j < univ.town.monst.size(); j++)
				if(univ.town.monst[j].is_alive() && !univ.town.monst[j].is_friendly() &&
					(dist(cur_monst->cur_loc,univ.town.monst[j].cur_loc) <= 6)
					&& (can_see_light(cur_monst->cur_loc,univ.town.monst[j].cur_loc,sight_obscurity) < 5)) {
					cur_monst->active = eCreatureStatus::ALERTED;
					cur_monst->mobility = 1;
				}
		}
		// End of seeing if monsters see others
		
		cur_monst->ap = 0;
		if(cur_monst->active == eCreatureStatus::ALERTED) { // Begin action loop for angry, active monsters
			// First note that hostile monsters are around.
			if(!cur_monst->is_friendly())
				univ.party.hostiles_present = 30;
			
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
		if(univ.debug_mode)
			cur_monst->ap = 0;
		center_on_monst = false;
		
		// Now take care of summoned monsters
		if(cur_monst->is_alive()) {
			if(cur_monst->summon_time == 1) {
				cur_monst->active = eCreatureStatus::DEAD;
				cur_monst->ap = 0;
				cur_monst->spell_note(17);
			}
			move_to_zero(cur_monst->summon_time);
		}
		
	}
	
	for(short i = 0; i < num_monst; i++) {  // Begin main monster loop, do monster actions
		// If party dead, no point
		if(!univ.party.is_alive())
			return;
		
		futzing = 0; // assume monster is fresh
		special_called = false;
		cur_monst = &univ.town.monst[i];
		
		
		for(short j = 0; j < 6; j++)
			if(univ.party[j].main_status == eMainStatus::ALIVE && monst_adjacent(univ.party[j].combat_pos,i))
				pc_adj[j] = true;
			else pc_adj[j] = false;
		
		
		
		while((cur_monst->ap > 0) && (cur_monst->is_alive())) {  // Spend each action point
			
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
					targ_space = univ.party.town_loc;
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
				&& cur_monst->attitude != eAttitude::DOCILE
				&& (target != 6 || !cur_monst->is_friendly())
				&& (party_can_see_monst(i)) ) {
				center_on_monst = true;
				center = cur_monst->cur_loc;
				draw_terrain(0);
				int speed = get_int_pref("GameSpeed");
				pause(speed == 3 ? 9 : speed);
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
					&& !cur_monst->mindless && cur_monst->m_type != eRace::UNDEAD && cur_monst->m_type != eRace::SKELETAL)
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
			if(target != 6 && cur_monst->attitude != eAttitude::DOCILE
				&& (monst_can_see(i,targ_space))
				&& (can_see_monst(targ_space,i))) { // Begin spec. attacks
				
				// Basic breath weapons
				if(cur_monst->abil[eMonstAbil::DAMAGE2].active && cur_monst->abil[eMonstAbil::DAMAGE2].gen.type == eMonstGen::BREATH
					&& !acted_yet && get_ran(1,1,1000) < cur_monst->abil[eMonstAbil::DAMAGE2].gen.odds) {
					if(target != 6  && dist(cur_monst->cur_loc,targ_space) <= cur_monst->abil[eMonstAbil::DAMAGE2].gen.range) {
						acted_yet = monst_breathe(cur_monst,targ_space,cur_monst->abil[eMonstAbil::DAMAGE2]);
						(void) acted_yet; // TODO: Is it correct to discard this and make it always true?
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
						(void) acted_yet; // TODO: Is it correct to discard this and make it always true?
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
						(void) acted_yet; // TODO: Is it correct to discard this and make it always true?
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
							case eMonstAbil::STEAL_FOOD: case eMonstAbil::STEAL_GOLD: case eMonstAbil::STUN: case eMonstAbil::DAMAGE2:
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
								// Non-missile abilities
							case eMonstAbil::ABSORB_SPELLS: case eMonstAbil::DEATH_TRIGGER: case eMonstAbil::HIT_TRIGGER:
							case eMonstAbil::MARTYRS_SHIELD: case eMonstAbil::NO_ABIL: case eMonstAbil::RADIATE:
							case eMonstAbil::SPECIAL: case eMonstAbil::SPLITS: case eMonstAbil::SUMMON:
								continue;
						}
						if(pick_abil.second.active) break;
					}
				}
				if(pick_abil.second.active) {
					print_monst_name(cur_monst->number);
					monst_fire_missile(i/*,cur_monst->skill*/,cur_monst->status[eStatus::BLESS_CURSE],
									   pick_abil,cur_monst->cur_loc,&univ.get_target(target));
					take_m_ap(pick_abil.second.get_ap_cost(pick_abil.first), cur_monst);
					had_monst = true;
					acted_yet = true;
				}
				
				// Unusual ability - don't use multiple times per round
				if(cur_monst->abil[eMonstAbil::SPECIAL].active && !special_called && party_can_see_monst(i) && get_ran(1,1,1000) <= cur_monst->abil[eMonstAbil::SPECIAL].special.extra3) {
					uAbility abil = cur_monst->abil[eMonstAbil::SPECIAL];
					short s1;
					special_called = true;
					univ.party.force_ptr(21, targ_space.x);
					univ.party.force_ptr(22, targ_space.y);
					if(target < 6)
						univ.party.force_ptr(20, 11 + target); // ready to be passed to SELECT_TARGET node
					else univ.party.force_ptr(20, target); // ready to be passed to SELECT_TARGET node
					run_special(eSpecCtx::MONST_SPEC_ABIL, eSpecCtxType::SCEN, abil.special.extra1, cur_monst->cur_loc, &s1);
					if(s1 <= 0)
						take_m_ap(abil.special.extra2,cur_monst);
				}
			} // Special attacks
			
			// Attack melee
			iLiving& who = univ.get_target(target);
			if(!acted_yet && who.is_alive() && monst_adjacent(targ_space,i)) {
				bool do_attack = false;
				if(dynamic_cast<cPlayer*>(&who)) {
					// Attack a PC only if hostile
					if(!cur_monst->is_friendly())
						do_attack = true;
				} else if(dynamic_cast<cCreature*>(&who)) {
					// Attack a monster only if not docile
					if(cur_monst->attitude != eAttitude::DOCILE)
						do_attack = true;
				}
				if(do_attack) {
					monster_attack(i, &who);
					take_m_ap(4,cur_monst);
					acted_yet = true;
					had_monst = true;
				}
			}
			
			if(acted_yet) {
				print_buf();
				pause(8);
				flushingInput = true;
			}
			
			if(overall_mode == MODE_COMBAT) {
				if(!acted_yet && (cur_monst->mobility == 1)) {  // move monst
					move_target = (univ.town.monst[i].target != 6) ? univ.town.monst[i].target : closest_pc(cur_monst->cur_loc);
					if(monst_hate_spot(i,&move_targ)) // First, maybe move out of dangerous space
						seek_party (i,cur_monst->cur_loc,move_targ);
					else { // spot is OK, so go nuts
						if(!cur_monst->is_friendly() && move_target < 6) // Monsters seeking party do so
							if(univ.party[move_target].main_status == eMainStatus::ALIVE) {
								seek_party (i,cur_monst->cur_loc,univ.party[move_target].combat_pos);
								for(short k = 0; k < 6; k++)
									if(univ.party[k].parry > 99 && monst_adjacent(univ.party[k].combat_pos,i)
									   && (cur_monst->is_alive()) && !univ.party[k].traits[eTrait::PACIFIST]) {
										univ.party[k].parry = 0;
										pc_attack(k,cur_monst);
									}
							}
						
						if(move_target >= 100) // Monsters seeking monsters do so
							if(univ.town.monst[move_target - 100].is_alive()) {
								seek_party (i,cur_monst->cur_loc,univ.town.monst[move_target - 100].cur_loc);
								for(short k = 0; k < 6; k++)
									if(univ.party[k].parry > 99 && monst_adjacent(univ.party[k].combat_pos,i)
										&& cur_monst->is_alive() && !cur_monst->is_friendly()
										&& !univ.party[k].traits[eTrait::PACIFIST]) {
										univ.party[k].parry = 0;
										pc_attack(k,cur_monst);
									}
							}
						
						if(cur_monst->attitude == eAttitude::DOCILE) {
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
			if(is_combat())
				for(short k = 0; k < 6; k++)
					if(univ.party[k].main_status == eMainStatus::ALIVE && !monst_adjacent(univ.party[k].combat_pos,i)
						&& pc_adj[k] && !cur_monst->is_friendly() && cur_monst->is_alive() &&
					   univ.party[k].status[eStatus::INVISIBLE] == 0 && !univ.party[k].traits[eTrait::PACIFIST]) {
						combat_posing_monster = current_working_monster = k;
						pc_attack(k,cur_monst);
						combat_posing_monster = current_working_monster = 100 + i;
						pc_adj[k] = false;
					}
			
			// Place fields for monsters that create them. Only done when monst sees foe
			if(target != 6 && can_see_light(cur_monst->cur_loc,targ_space,sight_obscurity) < 5) {
				if(cur_monst->abil[eMonstAbil::RADIATE].active && get_ran(1,1,100) < cur_monst->abil[eMonstAbil::RADIATE].radiate.chance) {
					switch(cur_monst->abil[eMonstAbil::RADIATE].radiate.pat) {
						case PAT_SINGLE:
							place_spell_pattern(single, cur_monst->cur_loc, cur_monst->abil[eMonstAbil::RADIATE].radiate.type, 7);
							break;
						case PAT_SMSQ:
							place_spell_pattern(small_square, cur_monst->cur_loc, cur_monst->abil[eMonstAbil::RADIATE].radiate.type, 7);
							break;
						case PAT_SQ:
							place_spell_pattern(square, cur_monst->cur_loc, cur_monst->abil[eMonstAbil::RADIATE].radiate.type, 7);
							break;
						case PAT_OPENSQ:
							place_spell_pattern(open_square, cur_monst->cur_loc, cur_monst->abil[eMonstAbil::RADIATE].radiate.type, 7);
							break;
						case PAT_RAD2:
							place_spell_pattern(radius2, cur_monst->cur_loc, cur_monst->abil[eMonstAbil::RADIATE].radiate.type, 7);
							break;
						case PAT_RAD3:
							place_spell_pattern(radius3, cur_monst->cur_loc, cur_monst->abil[eMonstAbil::RADIATE].radiate.type, 7);
							break;
						case PAT_PLUS:
							place_spell_pattern(t, cur_monst->cur_loc, cur_monst->abil[eMonstAbil::RADIATE].radiate.type, 7);
							break;
						case PAT_WALL:
							int dir = (cur_monst->direction + 6) % 8;
							place_spell_pattern(field[dir], cur_monst->cur_loc, cur_monst->abil[eMonstAbil::RADIATE].radiate.type, 7);
							break;
					}
				}
				if(cur_monst->abil[eMonstAbil::SUMMON].active && get_ran(1,1,100) < cur_monst->abil[eMonstAbil::SUMMON].summon.chance) {
					uAbility abil = cur_monst->abil[eMonstAbil::SUMMON];
					mon_num_t what_summon = 0;
					switch(abil.summon.type) {
						case eMonstSummon::TYPE: what_summon = abil.summon.what; break;
						case eMonstSummon::LEVEL: what_summon = get_summon_monster(minmax(0, 4, abil.summon.what)); break;
						case eMonstSummon::SPECIES:
							for(short k = 0; k < 200; k++) {
								int j = get_ran(1,0,univ.scenario.scen_monsters.size() - 1);
								if(univ.scenario.scen_monsters[j].m_type == eRace(abil.summon.what)) {
									what_summon = j;
									break;
								}
							}
							if(!what_summon) ASB("  Summon failed.");
							break;
					}
					if(what_summon) r1 = get_ran(1, abil.summon.min, abil.summon.max);
					else r1 = 0;
					if(r1 && summon_monster(what_summon, cur_monst->cur_loc,abil.summon.len,cur_monst->attitude,cur_monst->is_friendly())) {
						cur_monst->spell_note(33);
						play_sound(61);
						bool failed = false;
						while(--r1 && !failed) {
							failed = summon_monster(what_summon,cur_monst->cur_loc,abil.summon.len,cur_monst->attitude,cur_monst->is_friendly());
						}
					}
				}
			}
			
			combat_posing_monster = current_working_monster = -1;
			// Redraw monster after it goes
			if(cur_monst->attitude != eAttitude::DOCILE && cur_monst->is_alive() && cur_monst->ap == 0
				&& (is_combat()) && (cur_monst->picture_num > 0) && (party_can_see_monst(i) )) {
				center = cur_monst->cur_loc;
				draw_terrain(0);
				
			}
			
			// If monster dead, take away actions
			if(cur_monst->active == eCreatureStatus::DEAD)
				cur_monst->ap = 0;
			
			if(futzing > 1) // If monster's just pissing around, give up
				cur_monst->ap = 0;
		}  // End of monster action loop
		
		
	}
	
	for(short i = 0; i < num_monst; i++) {  // Begin monster time stuff loop
		// If party dead, no point
		if(!univ.party.is_alive())
			return;
		
		cur_monst = &univ.town.monst[i];
		
		if(cur_monst->is_alive()) { // Take care of monster effects
			if(cur_monst->status[eStatus::ACID] > 0) {  // Acid
				if(!printed_acid) {
					add_string_to_buf("Acid:");
					printed_acid = true;
				}
				r1 = get_ran(cur_monst->status[eStatus::ACID],1,6);
				damage_monst(*cur_monst, 6,r1, eDamageType::MAGIC);
				cur_monst->status[eStatus::ACID]--;
			}
			
			if(cur_monst->status[eStatus::ASLEEP] == 1)
				cur_monst->spell_note(29);
			move_to_zero(cur_monst->status[eStatus::ASLEEP]);
			move_to_zero(cur_monst->status[eStatus::PARALYZED]);
			move_to_zero(cur_monst->status[eStatus::INVISIBLE]);
			move_to_zero(cur_monst->status[eStatus::INVULNERABLE]);
			move_to_zero(cur_monst->status[eStatus::MAGIC_RESISTANCE]);
			move_to_zero(cur_monst->status[eStatus::MARTYRS_SHIELD]);
			
			if(univ.party.age % 2 == 0) {
				move_to_zero(cur_monst->status[eStatus::BLESS_CURSE]);
				move_to_zero(cur_monst->status[eStatus::HASTE_SLOW]);
				move_to_zero(cur_monst->status[eStatus::WEBS]);
				
				if(cur_monst->status[eStatus::POISON] > 0) {  // Poison
					if(!printed_poison) {
						add_string_to_buf("Poisoned monsters:");
						printed_poison = true;
					}
					r1 = get_ran(cur_monst->status[eStatus::POISON],1,6);
					damage_monst(*cur_monst, 6, r1, eDamageType::POISON);
					cur_monst->status[eStatus::POISON]--;
				}
				if(cur_monst->status[eStatus::DISEASE] > 0) {  // Disease
					if(!printed_disease) {
						add_string_to_buf("Diseased monsters:");
						printed_disease = true;
					}
					switch(get_ran(1,1,5)) {
						case 1: case 2: cur_monst->poison(2); break;
						case 3: cur_monst->slow(2); break;
						case 4: cur_monst->curse(2); break;
						case 5: cur_monst->scare(10); break;
					}
					if(get_ran(1,1,6) < 4)
						cur_monst->status[eStatus::DISEASE]--;
				}
				
			}
			
			if(univ.party.age % 4 == 0) {
				cur_monst->restore_sp(2);
				move_to_zero(cur_monst->status[eStatus::DUMB]);
				// Bonus HP and SP wear off
				if(cur_monst->mp > cur_monst->max_mp)
					cur_monst->mp--;
				if(cur_monst->health > cur_monst->m_health)
					cur_monst->health--;
			}
		} // end take care of monsters
	}
	
	// If in town, need to restore center
	if(!is_combat())
		center = univ.party.town_loc;
	if(had_monst)
		put_pc_screen();
	for(cPlayer& pc : univ.party)
		pc.parry = 0;
	
	monsters_going = false;
}

void monster_attack(short who_att,iLiving* target) {
	short r1,r2,store_hp,sound_type = 0;
	eDamageType dam_type = eDamageType::WEAPON;
	
	cCreature* attacker = &univ.town.monst[who_att];
	
	// A peaceful monster won't attack PCs or friendly monsters
	if(attacker->is_friendly() && target->is_friendly())
		return;
	
	// Draw attacker frames
	if((is_combat())
	   && (center_on_monst || !monsters_going)) {
		if(!attacker->invisible && attacker->status[eStatus::INVISIBLE] <= 0)
			frame_space(attacker->cur_loc,0,attacker->x_width,attacker->y_width);
		frame_space(target->get_loc(),1,1,1);
	}
	
	for(const auto& att : attacker->a)
		if(att.dice != 0) {
			attacker->print_attacks(target);
			break;
		}
			
	// Some things depend on whether it's a player or a monster.
	cCreature* m_target = dynamic_cast<cCreature*>(target);
	cPlayer* pc_target = dynamic_cast<cPlayer*>(target);
	
	// Check sanctuary for melee
	if(target->status[eStatus::INVISIBLE] > 0 || (m_target != nullptr && m_target->invisible)) {
		r1 = get_ran(1,1,100);
		// Usually hit_chance would be indexed by the weapon skill, not monster level.
		// Indexing by monster level is usually a debuff, and half-level moreso.
		if(r1 > hit_chance[attacker->level / 2]) {
			add_string_to_buf("  Can't find target!");
			return;
		}
	}
	
	for(short i = 0; i < attacker->a.size(); i++) {
		if(attacker->a[i].dice > 0 && target->is_alive()) {
//			sprintf ((char *) create_line, "  Attacks %s.",(char *) univ.party[target].name);
//			add_string_to_buf((char *) create_line);
			
			// if target monster friendly to party, make able to attack
			if(m_target != nullptr && m_target->attitude == eAttitude::DOCILE)
				m_target->attitude = eAttitude::FRIENDLY;
			
			// Attack roll
			r1 = get_ran(1,1,100);
			r1 -= 5 * min(8, attacker->status[eStatus::BLESS_CURSE]);
			r1 += 5 * target->status[eStatus::BLESS_CURSE] - 15;
			r1 += 5 * (attacker->status[eStatus::WEBS] / 3);
			if(attacker->status[eStatus::FORCECAGE] > 0)
				r1 += 3; // TODO: PCs have reduced penalty for pole weapons; any way to mirror that here?
			if(target->status[eStatus::FORCECAGE] > 0)
				r1 += 1;
			if(pc_target != nullptr) {
				r1 += 5 * pc_target->stat_adj(eSkill::DEXTERITY);
				r1 += pc_target->get_prot_level(eItemAbil::EVASION);
				if(pc_target->parry < 100)
					r1 += 5 * pc_target->parry;
			}
			
			// Damage roll
			r2 = get_ran(attacker->a[i].dice, 1, attacker->a[i].sides) + 1;
			r2 += min(8, attacker->status[eStatus::BLESS_CURSE]);
			r2 -= target->status[eStatus::BLESS_CURSE];
			if(pc_target != nullptr) {
				if(univ.difficulty_adjust() > 2)
					r2 = r2 * 2;
				if(univ.difficulty_adjust() == 2)
					r2 = (r2 * 3) / 2;
			} else r2 += 1;
			
			if((target->status[eStatus::ASLEEP] > 0) || (target->status[eStatus::PARALYZED] > 0)) {
				r1 -= 80;
				r2 = r2 * 2;
			}
			
			draw_terrain(2);
			// Check if hit, and do effects
			if(r1 <= hit_chance[(attacker->skill + 4) / 2]) {
				if(attacker->m_type == eRace::UNDEAD || attacker->m_type == eRace::SKELETAL)
					dam_type = eDamageType::UNDEAD;
				if(attacker->m_type == eRace::DEMON)
					dam_type = eDamageType::DEMON;
				
				store_hp = target->get_health();
				sound_type = get_monst_sound(attacker,i);
				size_t i_monst = univ.get_target_i(*target);
				short damaged = 0;
				if(m_target != nullptr) {
					// TODO: Maybe this damage should be printed?
					damaged = damage_monst(*m_target,7,r2,dam_type,sound_type,false);
				} else if(pc_target != nullptr) {
					damaged = damage_pc(*pc_target,r2,dam_type,attacker->m_type,sound_type);
					if(store_hp - target->get_health() <= 0)
						damaged = 0;
				}
				if(damaged) {
					damaged_message(store_hp - target->get_health(), attacker->a[i].type);
					
					if(target->is_shielded()) {
						int dmg = attacker->get_shared_dmg(store_hp - target->get_health());
						add_string_to_buf("  Shares damage!");
						int who_hit = pc_target != nullptr ? 6 : 7;
						damage_monst(*attacker, who_hit, dmg, eDamageType::MAGIC);
					}
					
					if(i == 0 && attacker->status[eStatus::POISONED_WEAPON] > 0) {
						short poison_amt = attacker->status[eStatus::POISONED_WEAPON];
						target->poison(poison_amt);
						if(dynamic_cast<cPlayer*>(target))
							put_pc_screen();
						move_to_zero(attacker->status[eStatus::POISONED_WEAPON]);
					}
					
					for(auto& abil : attacker->abil) {
						if(!abil.second.active) continue;
						if(getMonstAbilCategory(abil.first) != eMonstAbilCat::GENERAL)
							continue;
						if(abil.second.gen.type != eMonstGen::TOUCH)
							continue;
						if(abil.second.gen.odds > 0 && get_ran(1,1,1000) <= abil.second.gen.odds)
							continue;
						// Print message and possibly choose sound
						snd_num_t snd = 0;
						switch(abil.first) {
							case eMonstAbil::STUN: add_string_to_buf("  Stuns!"); break;
							case eMonstAbil::PETRIFY: add_string_to_buf("  Petrifying touch!"); break;
							case eMonstAbil::DRAIN_SP: add_string_to_buf("  Drains magic!"); break; // TODO: This has no effect on monsters
							case eMonstAbil::DRAIN_XP: add_string_to_buf("  Drains life!"); break;
							case eMonstAbil::KILL: add_string_to_buf("  Killing touch!"); break;
							case eMonstAbil::STEAL_FOOD:
								if(pc_target == nullptr) continue; // Can't use this against other monsters.
								add_string_to_buf("  Steals food!");
								snd = 26;
								break;
							case eMonstAbil::STEAL_GOLD:
								if(pc_target == nullptr) continue; // Can't use this against other monsters.
								add_string_to_buf("  Steals gold!");
								break; // TODO: Pick a sound
							case eMonstAbil::FIELD: break; // TODO: Invent messages?
							case eMonstAbil::DAMAGE: case eMonstAbil::DAMAGE2:
								switch(abil.second.gen.dmg) {
									case eDamageType::MARKED: break; // Invalid
									case eDamageType::FIRE: add_string_to_buf("  Burning touch!"); break;
									case eDamageType::COLD: add_string_to_buf("  Freezing touch!"); break;
									case eDamageType::MAGIC: add_string_to_buf("  Shocking touch!"); break;
									case eDamageType::SPECIAL:
									case eDamageType::UNBLOCKABLE: add_string_to_buf("  Eerie touch!"); break;
									case eDamageType::POISON: add_string_to_buf("  Slimy touch!"); break;
									case eDamageType::WEAPON: add_string_to_buf("  Drains stamina!"); break;
									case eDamageType::UNDEAD: add_string_to_buf("  Chilling touch!"); break;
									case eDamageType::DEMON: add_string_to_buf("  Unholy touch!"); break;
								}
								break;
							case eMonstAbil::STATUS2:
								// STATUS2 is active only on attack #1; STATUS is active on all attacks
								if(i > 0) continue;
								BOOST_FALLTHROUGH;
							case eMonstAbil::STATUS:
								switch(abil.second.gen.stat) {
									case eStatus::MAIN: continue; // Invalid
									case eStatus::POISON: add_string_to_buf("  Poisonous!"); break;
									case eStatus::DISEASE: add_string_to_buf("  Causes disease!"); break;
									case eStatus::DUMB: add_string_to_buf("  Dumbfounds!"); break;
									case eStatus::WEBS: add_string_to_buf("  Webs!"); break;
									case eStatus::ASLEEP: add_string_to_buf("  Sleeps!"); break;
									case eStatus::PARALYZED: add_string_to_buf("  Paralysis touch!"); break;
									case eStatus::ACID: add_string_to_buf("  Acid touch!"); break;
									case eStatus::HASTE_SLOW: add_string_to_buf("  Slowing touch!"); break;
									case eStatus::BLESS_CURSE: add_string_to_buf("  Cursing touch!"); break;
									case eStatus::CHARM: if(m_target == nullptr) continue; else add_string_to_buf("  Charming touch!"); break;
									case eStatus::FORCECAGE: add_string_to_buf("  Entrapping touch!"); break;
									case eStatus::INVISIBLE: add_string_to_buf("  Revealing touch!"); break;
									case eStatus::INVULNERABLE: add_string_to_buf("  Piercing touch!"); break;
									case eStatus::MAGIC_RESISTANCE: add_string_to_buf("  Overwhelming touch!"); break;
									case eStatus::MARTYRS_SHIELD: add_string_to_buf("  Anti-martyr's touch!"); break;
									case eStatus::POISONED_WEAPON: add_string_to_buf("  Poison-draining touch!"); break;
								}
								break;
								// Non-touch abilities
							case eMonstAbil::MISSILE: case eMonstAbil::MISSILE_WEB: case eMonstAbil::RAY_HEAT:
							case eMonstAbil::ABSORB_SPELLS: case eMonstAbil::DEATH_TRIGGER: case eMonstAbil::HIT_TRIGGER:
							case eMonstAbil::MARTYRS_SHIELD: case eMonstAbil::NO_ABIL: case eMonstAbil::RADIATE:
							case eMonstAbil::SPECIAL: case eMonstAbil::SPLITS: case eMonstAbil::SUMMON:
								continue;
						}
						if(snd > 0) play_sound(snd);
						print_buf();
						monst_basic_abil(who_att, abil, target);
						put_pc_screen();
					}
					
					if(pc_target != nullptr) {
						if(cInvenSlot spec_item = pc_target->has_abil_equip(eItemAbil::HIT_CALL_SPECIAL)) {
							short s1;
							univ.party.force_ptr(21, target->get_loc().x);
							univ.party.force_ptr(22, target->get_loc().y);
							univ.party.force_ptr(20, i_monst);
							run_special(eSpecCtx::ATTACKED_MELEE, eSpecCtxType::SCEN, spec_item->abil_strength, attacker->cur_loc, &s1);
							if(s1 > 0)
								attacker->ap += 4;
						}
					} else if(m_target != nullptr && m_target->abil[eMonstAbil::HIT_TRIGGER].active) {
						short s1;
						univ.party.force_ptr(21, target->get_loc().x);
						univ.party.force_ptr(22, target->get_loc().y);
						univ.party.force_ptr(20, i_monst);
						run_special(eSpecCtx::ATTACKED_MELEE, eSpecCtxType::SCEN, m_target->abil[eMonstAbil::HIT_TRIGGER].special.extra1, attacker->cur_loc, &s1);
						if(s1 > 0)
							attacker->ap += 4;
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
		if(!target->is_alive())
			break;
	}
	
}

void monst_fire_missile(short m_num,short bless,std::pair<eMonstAbil,uAbility> abil,location source,iLiving* target) {
	short r2;
	location targ_space;
	
	if(target == nullptr) return;
	if(!target->is_alive()) return;
	targ_space = target->get_loc();
	
	cPlayer* pc_target = dynamic_cast<cPlayer*>(target);
	cCreature* m_target = dynamic_cast<cCreature*>(target);
	
	if(is_combat() && center_on_monst) {
		frame_space(source,0,univ.town.monst[m_num].x_width,univ.town.monst[m_num].y_width);
		if(m_target != nullptr)
			frame_space(targ_space,1,m_target->x_width,m_target->y_width);
		else frame_space(targ_space,1,1,1);
	}
	
	draw_terrain(2);
	if(abil.first == eMonstAbil::MISSILE) {
		snd_num_t snd = 14;
		switch(abil.second.missile.type) {
			case eMonstMissile::ARROW:
			case eMonstMissile::BOLT:
			case eMonstMissile::RAPID_ARROW:
				snd = 12;
				if(pc_target != nullptr)
					add_string_to_buf("  Shoots at " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(12);
				break;
			case eMonstMissile::SPEAR:
				if(pc_target != nullptr)
					add_string_to_buf("  Throws spear at " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(13);
				break;
			case eMonstMissile::RAZORDISK:
				if(pc_target != nullptr)
					add_string_to_buf("  Throws razordisk at " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(15);
				break;
			case eMonstMissile::SPINE:
				if(pc_target != nullptr)
					add_string_to_buf("  Fires spines at " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(32);
				break;
			case eMonstMissile::DART:
				if(pc_target != nullptr)
					add_string_to_buf("  Throws dart at " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(53);
				break;
			case eMonstMissile::ROCK:
			case eMonstMissile::BOULDER:
				if(pc_target != nullptr)
					add_string_to_buf("  Throws rock at " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(14);
				break;
			case eMonstMissile::KNIFE:
				if(pc_target != nullptr)
					add_string_to_buf("  Throws knife at " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(54);
				break;
		}
		if(abil.second.missile.type == eMonstMissile::ARROW || abil.second.missile.type == eMonstMissile::RAPID_ARROW || abil.second.missile.type == eMonstMissile::BOLT)
			snd = 12;
		if(abil.second.missile.pic < 0) play_sound(snd);
		else run_a_missile(source, targ_space, abil.second.missile.pic, 1, snd, 0, 0, 100);
		// Check sanctuary for missile attack
		if(target->status[eStatus::INVISIBLE] > 0 || (m_target != nullptr && m_target->invisible)) {
			// Usually hit_chance would be indexed by the weapon skill, not monster level.
			// Indexing by monster level is usually a debuff.
			if(get_ran(1,1,100) > hit_chance[univ.town.monst[m_num].level]) {
				add_string_to_buf("  Can't find target!");
				return;
			}
		}
		int target_bless = target->status[eStatus::BLESS_CURSE];
		int r1 = get_ran(1,1,100) - 5 * min(8,bless) + 5 * target_bless - 5 * can_see_light(source, targ_space, sight_obscurity);
		if(pc_target != nullptr) {
			r1 += pc_target->get_prot_level(eItemAbil::EVASION);
			if(pc_target->parry < 100)
				r1 += 5 * pc_target->parry;
		}
		
		size_t i_monst = univ.get_target_i(*target);
		if(r1 <= hit_chance[abil.second.missile.skill]) {
			r2 = get_ran(abil.second.missile.dice,1,abil.second.missile.sides) + min(10,bless);
			if(pc_target != nullptr) {
				add_string_to_buf("  Hits " + pc_target->name + '.');
				// TODO: Should we pass in the monster's actual race here?
				damage_pc(*pc_target,r2,eDamageType::WEAPON,eRace::UNKNOWN,13);
			} else if(m_target != nullptr) {
				m_target->spell_note(16);
				damage_monst(*m_target,7,r2,eDamageType::WEAPON,13);
			}
		} else {
			if(pc_target != nullptr)
				add_string_to_buf("  Misses " + pc_target->name + '.');
			else if(m_target != nullptr)
				m_target->spell_note(18);
		}
		if(pc_target != nullptr) {
			if(cInvenSlot spec_item = pc_target->has_abil_equip(eItemAbil::HIT_CALL_SPECIAL)) {
				short s1;
				// TODO: This force_ptr...run_special code is almost duplicated in several places; maybe make a call_attack_spec subroutine!
				univ.party.force_ptr(21, target->get_loc().x);
				univ.party.force_ptr(22, target->get_loc().y);
				univ.party.force_ptr(20, i_monst);
				run_special(eSpecCtx::ATTACKED_RANGE, eSpecCtxType::SCEN, spec_item->abil_strength, univ.town.monst[m_num].cur_loc, &s1);
				if(s1 > 0)
					univ.town.monst[m_num].ap += abil.second.get_ap_cost(abil.first);
			}
		} else if(m_target != nullptr && m_target->abil[eMonstAbil::HIT_TRIGGER].active) {
			short s1;
			univ.party.force_ptr(21, m_target->cur_loc.x);
			univ.party.force_ptr(22, m_target->cur_loc.y);
			univ.party.force_ptr(20, i_monst);
			run_special(eSpecCtx::ATTACKED_RANGE, eSpecCtxType::SCEN, m_target->abil[eMonstAbil::HIT_TRIGGER].special.extra1, univ.town.monst[m_num].cur_loc, &s1);
			if(s1 > 0)
				univ.town.monst[m_num].ap += abil.second.get_ap_cost(abil.first);
		}
	} else if(abil.first == eMonstAbil::MISSILE_WEB) {
		if(pc_target != nullptr)
			add_string_to_buf("  Throws web at " + pc_target->name + '.');
		else if(m_target != nullptr)
			m_target->spell_note(58);
		run_a_missile(source, targ_space, 8, 0, 14, 0, 0, 100);
		web_space(targ_space.x, targ_space.y);
	} else if(abil.first == eMonstAbil::RAY_HEAT) {
		if(pc_target != nullptr) add_string_to_buf("  Hits " + pc_target->name + " with heat ray!");
		else if(m_target != nullptr)
			m_target->spell_note(55);
		run_a_missile(source, targ_space, 13, 0, 51, 0, 0, 100);
		uAbility proxy = {true};
		proxy.gen.strength = abil.second.special.extra3;
		proxy.gen.type = eMonstGen::RAY;
		proxy.gen.dmg = eDamageType::FIRE;
		monst_basic_abil(m_num, {eMonstAbil::DAMAGE, proxy}, target);
	} else {
		if(abil.first == eMonstAbil::DRAIN_SP && pc_target != nullptr && pc_target->cur_sp < 4) {
			// modify target if target has no sp
			// TODO: What if it's a monster with no sp?
			for(short i = 0; i < 8; i++) {
				int j = get_ran(1,0,5);
				cPlayer& pc = univ.party[j];
				if(pc.main_status == eMainStatus::ALIVE && pc.cur_sp > 4 &&
				   (can_see_light(source,pc.combat_pos,sight_obscurity) < 5) && (dist(source,pc.combat_pos) <= 8)) {
					target = &pc;
					targ_space = pc.combat_pos;
					break;
				}
			}
		}
		snd_num_t snd;
		int path_type = 0;
		switch(abil.second.gen.type) {
			case eMonstGen::TOUCH: return; // never reached
			case eMonstGen::RAY:
				snd = 51;
				if(pc_target != nullptr)
					add_string_to_buf("  Fires ray at " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(55);
				break;
			case eMonstGen::GAZE:
				snd = 43;
				if(pc_target != nullptr)
					add_string_to_buf("  Gazes at " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(56);
				break;
			case eMonstGen::BREATH:
				snd = 44;
				if(pc_target != nullptr)
					add_string_to_buf("  Breathes on " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(57);
				break;
			case eMonstGen::SPIT:
				path_type = 1;
				snd = 64;
				if(pc_target != nullptr)
					add_string_to_buf("  Spits at " + pc_target->name + '.');
				else if(m_target != nullptr)
					m_target->spell_note(59);
				break;
		}
		if(abil.second.gen.pic < 0) play_sound(snd);
		else run_a_missile(source, targ_space, abil.second.gen.pic, path_type, snd, 0, 0, 100);
		monst_basic_abil(m_num, abil, target);
	}
}

void monst_basic_abil(short m_num, std::pair<eMonstAbil,uAbility> abil, iLiving* target) {
	int i, r1;
	if(target == nullptr) return;
	if(!target->is_alive()) return;
	location targ_space = target->get_loc();
	eDamageType dmg_tp;
	
	cPlayer* pc_target = dynamic_cast<cPlayer*>(target);
	cCreature* m_target = dynamic_cast<cCreature*>(target);
	
	switch(abil.first) {
		case eMonstAbil::DAMAGE: case eMonstAbil::DAMAGE2:
			// Determine die size
			i = 6;
			if(abil.second.gen.type == eMonstGen::BREATH)
				i = 8;
			else if(abil.second.gen.type == eMonstGen::SPIT || abil.second.gen.type == eMonstGen::TOUCH)
				i = 10;
			r1 = get_ran(abil.second.gen.strength, 1, i);
			i = univ.get_target_i(*target);
			start_missile_anim();
			dmg_tp = abil.second.gen.dmg;
			if(dmg_tp >= eDamageType::SPECIAL)
				dmg_tp = eDamageType::UNBLOCKABLE;
			damage_target(i, r1, dmg_tp);
			do_explosion_anim(5, 0);
			end_missile_anim();
			handle_marked_damage();
			break;
		case eMonstAbil::STUN:
			if(pc_target != nullptr && pc_target->has_abil_equip(eItemAbil::LIFE_SAVING))
				break;
			BOOST_FALLTHROUGH;
		case eMonstAbil::STATUS: case eMonstAbil::STATUS2:
			switch(abil.second.gen.stat) {
				case eStatus::PARALYZED:
				case eStatus::ASLEEP:
					if(abil.second.gen.type == eMonstGen::TOUCH)
						i = abil.second.gen.stat == eStatus::ASLEEP ? -15 : -5;
					else i = univ.town.monst[m_num].level / 2;
					target->sleep(abil.second.gen.stat, abil.second.gen.strength, i);
					break;
				case eStatus::ACID:
					target->acid(abil.second.gen.strength);
					break;
				case eStatus::POISON:
					target->poison(abil.second.gen.strength);
					break;
				case eStatus::BLESS_CURSE:
					target->curse(abil.second.gen.strength);
					break;
				case eStatus::HASTE_SLOW:
					target->slow(abil.second.gen.strength);
					break;
				case eStatus::WEBS:
					target->web(abil.second.gen.strength);
					break;
				case eStatus::DISEASE:
					target->disease(abil.second.gen.strength);
					break;
				case eStatus::DUMB:
					target->dumbfound(abil.second.gen.strength);
					break;
				case eStatus::INVULNERABLE:
				case eStatus::MAGIC_RESISTANCE:
				case eStatus::INVISIBLE:
				case eStatus::MARTYRS_SHIELD:
					target->apply_status(abil.second.gen.stat, -abil.second.gen.strength);
					break;
				case eStatus::FORCECAGE:
					target->sleep(abil.second.gen.stat, 8, abil.second.gen.strength);
					break;
				// This only works on monsters
				case eStatus::CHARM:
					target->sleep(abil.second.gen.stat, int(univ.town.monst[m_num].attitude), abil.second.gen.strength);
					break;
				// These two don't make sense in this context
				case eStatus::MAIN:
				case eStatus::POISONED_WEAPON:
					return;
			}
			break;
		case eMonstAbil::PETRIFY:
			i = percent(univ.town.monst[m_num].level, abil.second.gen.strength);
			if(pc_target != nullptr)
				petrify_pc(*pc_target, i);
			else if(m_target != nullptr)
				petrify_monst(*m_target, i);
			break;
		case eMonstAbil::DRAIN_SP:
			if(pc_target != nullptr) {
				add_string_to_buf("  Drains " + pc_target->name + '.');
				pc_target->cur_sp = percent(pc_target->cur_sp, abil.second.gen.strength);
			} else {
				m_target->spell_note(11);
				// TODO: If mp < 4 it used to set monster's skill to 1. Should that be restored?
				m_target->mp = percent(m_target->mp, abil.second.gen.strength);
			}
			break;
		case eMonstAbil::DRAIN_XP:
			if(pc_target != nullptr) {
				if(pc_target->has_abil_equip(eItemAbil::LIFE_SAVING)) break;
				drain_pc(*pc_target, percent(univ.town.monst[m_num].level, abil.second.gen.strength));
			}
			break;
		case eMonstAbil::KILL:
			i = univ.get_target_i(*target);
			r1 = get_ran(10 * abil.second.gen.strength, 1, 10);
			damage_target(i, r1, eDamageType::UNBLOCKABLE);
			break;
		case eMonstAbil::STEAL_FOOD:
			if(pc_target == nullptr) break;
			univ.party.food = std::max(0, univ.party.food - get_ran(1,0,abil.second.gen.strength) - abil.second.gen.strength);
			break;
		case eMonstAbil::STEAL_GOLD:
			if(pc_target == nullptr) break;
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
			// Non-basic abilities
		case eMonstAbil::MISSILE: case eMonstAbil::MISSILE_WEB: case eMonstAbil::RAY_HEAT:
		case eMonstAbil::ABSORB_SPELLS: case eMonstAbil::DEATH_TRIGGER: case eMonstAbil::HIT_TRIGGER:
		case eMonstAbil::MARTYRS_SHIELD: case eMonstAbil::NO_ABIL: case eMonstAbil::RADIATE:
		case eMonstAbil::SPECIAL: case eMonstAbil::SPLITS: case eMonstAbil::SUMMON:
			break;
	}
	if(pc_target != nullptr) {
		put_pc_screen();
		if(abil.first == eMonstAbil::STUN || abil.first == eMonstAbil::STATUS || abil.first == eMonstAbil::STATUS2) {
			if(abil.second.gen.stat == eStatus::DUMB)
				adjust_spell_menus();
		}
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
	
	caster->breathe_note();
	short level = get_ran(abil.gen.strength,1,8);
	if(!is_combat())
		level = level / 3;
	start_missile_anim();
	hit_space(targ_space,level,abil.gen.dmg,1,1);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
	
	return true;
}

bool monst_cast_mage(cCreature *caster,short targ) {
	short r1,level,target_levels,friend_levels_near,x;
	bool acted = false;
	location target,ashes_loc,l;
	cCreature *affected;
	static const eSpell caster_array[7][18] = {
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
	static const eSpell emer_spells[7][4] = {
		{eSpell::HASTE_MINOR, eSpell::NONE, eSpell::NONE, eSpell::FLAME},
		{eSpell::HASTE_MINOR, eSpell::SUMMON_BEAST, eSpell::CONFLAGRATION, eSpell::SLOW},
		{eSpell::HASTE_MINOR, eSpell::SUMMON_WEAK, eSpell::FIREBALL, eSpell::SUMMON_WEAK},
		{eSpell::HASTE_MINOR, eSpell::SUMMON_WEAK, eSpell::FIREBALL, eSpell::SUMMON_WEAK},
		{eSpell::HASTE_MAJOR, eSpell::SUMMON, eSpell::FIRESTORM, eSpell::HASTE_MAJOR},
		{eSpell::HASTE_MAJOR, eSpell::DEMON, eSpell::FIRESTORM, eSpell::DEMON},
		{eSpell::HASTE_MAJOR, eSpell::SUMMON_MAJOR, eSpell::FIRESTORM, eSpell::SHOCKWAVE},
	};
	static const std::set<eSpell> area_effects = {
		eSpell::CLOUD_STINK, eSpell::CONFLAGRATION, eSpell::FIREBALL, eSpell::WEB, eSpell::FIRESTORM, eSpell::SHOCKSTORM,
	};
	
	if(univ.town.is_antimagic(caster->cur_loc.x,caster->cur_loc.y)) {
		return false;
	}
	// is target dead?
	if(targ < 6 && univ.party[targ].main_status != eMainStatus::ALIVE)
		return false;
	if((targ >= 100) && (univ.town.monst[targ - 100].active == eCreatureStatus::DEAD))
		return false;
	
	eSpell spell;
	
	level = minmax(1,7,caster->mu - caster->status[eStatus::DUMB]) - 1;
	
	target = find_fireball_loc(caster->cur_loc,1,caster->is_friendly(),&target_levels);
	friend_levels_near = caster->is_friendly() ? count_levels(caster->cur_loc,3) : -1 * count_levels(caster->cur_loc,3);
	
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
	
	iLiving& victim = univ.get_target(targ);
	location vict_loc = victim.get_loc();
	
	// check antimagic
	if(targ == 6 && univ.town.is_antimagic(target.x,target.y))
		return false;
	if(is_combat())
		if(targ < 6 && univ.town.is_antimagic(univ.party[targ].combat_pos.x,univ.party[targ].combat_pos.y))
			return false;
	if(is_town())
		if(targ < 6 && univ.town.is_antimagic(univ.party.town_loc.x,univ.party.town_loc.y))
			return false;
	if(targ >= 100 && univ.town.is_antimagic(univ.town.monst[targ - 100].cur_loc.x, univ.town.monst[targ - 100].cur_loc.y))
		return false;
	
	// How about shockwave? Good idea?
	if(spell == eSpell::SHOCKWAVE && caster->is_friendly())
		spell = eSpell::SUMMON_MAJOR;
	if(spell == eSpell::SHOCKWAVE && !caster->is_friendly() && count_levels(caster->cur_loc,10) < 45)
		spell = eSpell::SUMMON_MAJOR;
	
	l = caster->cur_loc;
	if(caster->direction < 4 && caster->x_width > 1)
		l.x++;
	
	short cost = (*spell).level;
	if(spell == eSpell::SUMMON_BEAST || spell == eSpell::FIREBALL || spell == eSpell::SUMMON_WEAK)
		cost = 4;
	
	if(caster->mp >= cost) {
		caster->cast_spell_note(spell);
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
				caster->slow(-2);
				break;
			case eSpell::STRENGTH:
				play_sound(25);
				caster->curse(-3);
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
				victim.poison(2 + get_ran(1,0,caster->level / 2));
				break;
			case eSpell::SLOW:
				run_a_missile(l,vict_loc,15,0,25,0,0,80);
				victim.slow(2 + caster->level / 2);
				break;
			case eSpell::DUMBFOUND:
				run_a_missile(l,vict_loc,14,0,25,0,0,80);
				victim.dumbfound(2);
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
				summon_monster(r1,caster->cur_loc,x,caster->attitude,caster->is_friendly());
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
			case eSpell::SUMMON_WEAK: case eSpell::SUMMON: case eSpell::SUMMON_MAJOR: {
				int j;
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
				int x = get_ran(4,1,4);
				for(short i = 0; i < j; i++){
					play_sound(-61);
					if(!summon_monster(r1,caster->cur_loc,x,caster->attitude,caster->is_friendly())) {
						add_string_to_buf("  Summon failed."); i = j;}
				}
				break;
			}
			case eSpell::WEB:
				play_sound(25);
				place_spell_pattern(radius2,target,FIELD_WEB,7);
				break;
			case eSpell::POISON:
				run_a_missile(l,vict_loc,11,0,25,0,0,80);
				x = get_ran(1,0,caster->level / 2);
				victim.poison(4 + x);
				break;
			case eSpell::ICE_BOLT:
				run_a_missile(l,vict_loc,6,1,11,0,0,80);
				r1 = get_ran(5 + (caster->level / 5),1,8);
				start_missile_anim();
				damage_target(targ,r1,eDamageType::COLD);
				break;
			case eSpell::SLOW_GROUP:
				play_sound(25);
				if(!caster->is_friendly())
					for(short i = 0; i < 6; i++)
						if(pc_near(i,caster->cur_loc,8))
							univ.party[i].slow(2 + caster->level / 4);
				for(short i = 0; i < univ.town.monst.size(); i++) {
					if(univ.town.monst[i].is_alive() && !caster->is_friendly(univ.town.monst[i])
						&& (dist(caster->cur_loc,univ.town.monst[i].cur_loc) <= 7))
						univ.town.monst[i].slow(2 + caster->level / 4);
				}
				break;
			case eSpell::HASTE_MAJOR:
				play_sound(25);
				for(short i = 0; i < univ.town.monst.size(); i++)
					if((monst_near(i,caster->cur_loc,8,0)) &&
						(caster->attitude == univ.town.monst[i].attitude)) {
						affected = &univ.town.monst[i];
						affected->slow(-3);
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
				victim.poison(6 + x);
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
				summon_monster(85,caster->cur_loc,x,caster->attitude,caster->is_friendly());
				break;
			case eSpell::BLESS_MAJOR:
				play_sound(25);
				for(short i = 0; i < univ.town.monst.size(); i++)
					if((monst_near(i,caster->cur_loc,8,0)) &&
						(caster->attitude == univ.town.monst[i].attitude)) {
						affected = &univ.town.monst[i];
						affected->health += get_ran(2,1,10);
						r1 = get_ran(3,1,4);
						affected->curse(-r1);
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
			default:
				add_string_to_buf("  Error: Mage spell " + (*spell).name() + " not implemented for monsters.", 4);
				break;
		}
	}
	else caster->mp++;
	
	if(ashes_loc.x > 0)
		univ.town.set_ash(ashes_loc.x,ashes_loc.y,true);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
	
	if(dynamic_cast<cPlayer*>(&victim)) {
		put_pc_screen();
		if(spell == eSpell::DUMBFOUND)
			adjust_spell_menus();
	}
	
	return acted;
}

bool monst_cast_priest(cCreature *caster,short targ) {
	short r1,r2,x,level,target_levels,friend_levels_near;
	bool acted = false;
	location target,l;
	cCreature *affected;
	static const eSpell caster_array[7][10] = {
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
	static const eSpell emer_spells[7][4] = {
		{eSpell::NONE, eSpell::BLESS_MINOR, eSpell::NONE, eSpell::HEAL_MINOR},
		{eSpell::NONE, eSpell::SUMMON_SPIRIT, eSpell::NONE, eSpell::HEAL_MINOR},
		{eSpell::NONE, eSpell::SUMMON_SPIRIT, eSpell::NONE, eSpell::HEAL},
		{eSpell::NONE, eSpell::STICKS_TO_SNAKES, eSpell::NONE, eSpell::HEAL},
		{eSpell::NONE, eSpell::SUMMON_HOST, eSpell::FLAMESTRIKE, eSpell::HEAL_MAJOR},
		{eSpell::NONE, eSpell::SUMMON_HOST, eSpell::FLAMESTRIKE, eSpell::HEAL_ALL},
		{eSpell::AVATAR, eSpell::AVATAR, eSpell::DIVINE_THUD, eSpell::REVIVE_ALL},
	};
	static const std::set<eSpell> area_effects = {eSpell::FLAMESTRIKE, eSpell::DIVINE_THUD};
	location ashes_loc;
	
	
	if(targ < 6 && univ.party[targ].main_status != eMainStatus::ALIVE)
		return false;
	if((targ >= 100) && (univ.town.monst[targ - 100].active == eCreatureStatus::DEAD))
		return false;
	if(univ.town.is_antimagic(caster->cur_loc.x,caster->cur_loc.y)) {
		return false;
	}
	level = minmax(1,7,caster->cl - caster->status[eStatus::DUMB]) - 1;
	
	eSpell spell;
	
	target = find_fireball_loc(caster->cur_loc,1,caster->is_friendly(),&target_levels);
	friend_levels_near = caster->is_friendly() ? count_levels(caster->cur_loc,3) : -1 * count_levels(caster->cur_loc,3);
	
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
	
	iLiving& victim = univ.get_target(targ);
	location vict_loc = victim.get_loc();
	if(targ < 6)
		vict_loc = (is_town()) ? univ.party.town_loc : univ.party[targ].combat_pos;
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
		caster->cast_spell_note(spell);
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
				caster->curse(-(spell == eSpell::BLESS ? 5 : 3));
				play_sound(4);
				break;
			case eSpell::CURSE:
				run_a_missile(l,vict_loc,8,0,24,0,0,80);
				x = get_ran(1,0,1);
				victim.curse(2 + x);
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
				summon_monster(spell == eSpell::SUMMON_SPIRIT ? 125 : 122,caster->cur_loc,x,caster->attitude,caster->is_friendly());
				break;
			case eSpell::DISEASE:
				run_a_missile(l,vict_loc,11,0,24,0,0,80);
				x = get_ran(1,0,2);
				victim.disease(2 + x);
				break;
			case eSpell::HOLY_SCOURGE:
				run_a_missile(l,vict_loc,15,0,24,0,0,80);
				// TODO: Why are PCs and monsters affected in different amounts?
				if(targ < 6) {
					r1 = get_ran(1,0,2);
					victim.slow(2 + r1);
					r1 = get_ran(1,0,2);
					victim.curse(3 + r1);
				}
				else {
					r1 = get_ran(1,0,2);
					victim.slow(r1);
					r1 = get_ran(1,0,2);
					victim.curse(r1);
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
				for(short i = 0; i < r1; i++) {
					play_sound(-61);
					r2 = get_ran(1,0,7);
					x = get_ran(3,1,4);
					summon_monster((r2 == 1) ? 100 : 99,caster->cur_loc,x,caster->attitude,caster->is_friendly());
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
				summon_monster(126,caster->cur_loc,x,caster->attitude,caster->is_friendly());
				for(short i = 0; i < 4; i++)	{
					play_sound(-61);
					if(!summon_monster(125,caster->cur_loc,x,caster->attitude,caster->is_friendly()))
						i = 4;
				}
				break;
			case eSpell::CURSE_ALL: case eSpell::PESTILENCE:
				play_sound(24);
				r1 = get_ran(2,0,2);
				r2 = get_ran(1,0,2);
				if(!caster->is_friendly())
					for(short i = 0; i < 6; i++)
						if(pc_near(i,caster->cur_loc,8)) {
							if(spell == eSpell::CURSE_ALL)
								univ.party[i].curse(2 + r1);
							if(spell == eSpell::PESTILENCE)
								univ.party[i].disease(2 + r2);
						}
				for(short i = 0; i < univ.town.monst.size(); i++) {
					if(univ.town.monst[i].is_alive() && !caster->is_friendly(univ.town.monst[i])
						&& (dist(caster->cur_loc,univ.town.monst[i].cur_loc) <= 7)) {
						if(spell == eSpell::CURSE_ALL)
							univ.town.monst[i].curse(2 + r1);
						if(spell == eSpell::PESTILENCE)
							univ.town.monst[i].disease(2 + r2);
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
					default:
						add_string_to_buf("  Error: Healing spell " + (*spell).name() + " not implemented for monsters.", 4);
						break;
				}
				caster->heal(r1);
				break;
			case eSpell::BLESS_PARTY: case eSpell::REVIVE_ALL:
				play_sound(24);
				// TODO: What's r2 for here? Should it be used for Revive All?
				r1 =  get_ran(2,1,4); r2 = get_ran(3,1,6); (void) r2;
				for(short i = 0; i < univ.town.monst.size(); i++)
					if((monst_near(i,caster->cur_loc,8,0)) &&
						(caster->attitude == univ.town.monst[i].attitude)) {
						affected = &univ.town.monst[i];
						if(spell == eSpell::BLESS_PARTY)
							affected->curse(-r1);
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
				victim.slow(6);
				victim.poison(5 + r2);
				break;
			case eSpell::AVATAR:
				play_sound(24);
				caster->spell_note(26);
				caster->avatar();
				break;
			case eSpell::DIVINE_THUD:
				run_a_missile(l,target,9,0,11,0,0,80);
				r1 = (caster->level * 3) / 4 + 5;
				if(r1 > 29) r1 = 29;
				start_missile_anim();
				place_spell_pattern(radius2,target,eDamageType::MAGIC,r1,7 );
				// Note: This used to place ashes on the target square, but the spell description
				// doesn't describe fire or combustion, only force.
				break;
			default:
				add_string_to_buf("  Error: Priest spell " + (*spell).name() + " not implemented for monsters.", 4);
				break;
		}
		
		
	}
	else caster->mp++;
	if(ashes_loc.x > 0)
		univ.town.set_ash(ashes_loc.x,ashes_loc.y,true);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
	
	if(dynamic_cast<cPlayer*>(&victim))
		put_pc_screen();
	
	return acted;
}

short damage_target(short target,short dam,eDamageType type,short sound_type, bool do_print, short who_hit, eRace race) {
	if(target == 6) return 0;
	if(target < 6)
		return damage_pc(univ.party[target],dam,type,race,sound_type, do_print);
	else return damage_monst(univ.town.monst[target - 100], who_hit, dam, type,sound_type, do_print);
}

short damage_target(iLiving& target,short dam,eDamageType type,short sound_type, bool do_print, short who_hit, eRace race) {
	return damage_target(univ.get_target_i(target), dam, type, sound_type, do_print, who_hit, race);
}

//	target = find_fireball_loc(caster->m_loc,1,(caster->attitude == 1) ? 0 : 1,&target_levels);

//short mode; // 0 - hostile casting  1 - friendly casting
location find_fireball_loc(location where,short radius,short mode,short *m) {
	location check_loc,cast_loc(-1,0);
	short cur_lev,level_max = 10;
	
	for(check_loc.x = 1; check_loc.x < univ.town->max_dim - 1; check_loc.x ++)
		for(check_loc.y = 1; check_loc.y < univ.town->max_dim - 1; check_loc.y ++)
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
	location pc_where(-1,-1);
	
	for(cPlayer& pc : univ.party)
		if(pc.main_status == eMainStatus::ALIVE)
			if((dist(where,pc.combat_pos)) < (dist(where,pc_where)))
				pc_where = pc.combat_pos;
	return pc_where;
}

short count_levels(location where,short radius) {
	short store = 0;
	
	for(short i = 0; i < univ.town.monst.size(); i++)
		if(monst_near(i,where,radius,0)) {
			if(!univ.town.monst[i].is_friendly())
				store = store - univ.town.monst[i].level;
			else store = store + univ.town.monst[i].level;
		}
	if(is_combat()) {
		for(short i = 0; i < 6; i++)
			if(pc_near(i,where,radius))
				store = store + 10;
	}
	if(is_town())
		if(vdist(where,univ.party.town_loc) <= radius && can_see(where,univ.party.town_loc,sight_obscurity) < 5)
			store += 20;
	
	return store;
}

bool pc_near(short pc_num,location where,short radius) {
	// Assuming not looking
	cPlayer& pc = univ.party[pc_num];
	if(overall_mode >= MODE_COMBAT) {
		if(pc.main_status == eMainStatus::ALIVE && vdist(pc.combat_pos,where) <= radius)
			return true;
		else return false;
	}
	if(pc.main_status == eMainStatus::ALIVE && vdist(univ.party.town_loc,where) <= radius)
		return true;
	else return false;
}

//short active; // 0 - any monst 1 - monster need be active
bool monst_near(short m_num,location where,short radius,short active) {
	if((univ.town.monst[m_num].is_alive()) && (vdist(univ.town.monst[m_num].cur_loc,where) <= radius)
		&& ((active == 0) || (univ.town.monst[m_num].active == eCreatureStatus::ALERTED)) )
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
	short r1 = 0;
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
	for(short i = minmax(active.left + 1,active.right - 1,center.x - 4);
		 i <= minmax(active.left + 1,active.right - 1,center.x + 4); i++)
		for(short j = minmax(active.top + 1,active.bottom - 1,center.y - 4);
			 j <= minmax(active.top + 1,active.bottom - 1,center.y + 4); j++) {
			s_loc.x = i; s_loc.y = j;
			if(can_see_light(center,s_loc,sight_obscurity) == 5)
				pat.pattern[i - center.x + 4][j - center.y + 4] = 0;
		}
	
	
	// First actually make barriers, then draw them, then inflict damaging effects.
	for(short i = minmax(0,univ.town->max_dim - 1,center.x - 4); i <= minmax(0,univ.town->max_dim - 1,center.x + 4); i++)
		for(short j = minmax(0,univ.town->max_dim - 1,center.y - 4); j <= minmax(0,univ.town->max_dim - 1,center.y + 4); j++) {
			effect = pat.pattern[i - center.x + 4][j - center.y + 4];
			if(effect == FIELD_SMASH || sight_obscurity(i,j) < 5) {
				switch(eFieldType(effect)) {
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
					case SPECIAL_EXPLORED: case SPECIAL_SPOT: case SPECIAL_ROAD:
						break;
				}
			}
		}
	draw_terrain(0);
	if(is_town()) // now make things move faster if in town
		fast_bang = 2;
	
	// Damage to pcs
	for(cPlayer& pc : univ.party)
		for(short i = minmax(0,univ.town->max_dim - 1,center.x - 4); i <= minmax(0,univ.town->max_dim - 1,center.x + 4); i++)
			for(short j = minmax(0,univ.town->max_dim - 1,center.y - 4); j <= minmax(0,univ.town->max_dim - 1,center.y + 4); j++) {
				spot_hit.x = i;
				spot_hit.y = j;
				if(sight_obscurity(i,j) < 5 && pc.main_status == eMainStatus::ALIVE
					&& (((is_combat()) && (pc.combat_pos == spot_hit)) ||
						((is_town()) && (univ.party.town_loc == spot_hit)))) {
					effect = pat.pattern[i - center.x + 4][j - center.y + 4];
					if(effect >= 50) {
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
						} else if(effect > 370 && effect <= 400) {
							type = eDamageType::SPECIAL;
							dice = effect - 370;
						}
						if(type != eDamageType::MARKED) {
							r1 = get_ran(dice,1,6);
							damage_pc(pc,r1,type,eRace::UNKNOWN);
						}
					} else switch(eFieldType(effect)) {
						case WALL_FORCE:
							r1 = get_ran(2,1,6);
							damage_pc(pc,r1,eDamageType::MAGIC,eRace::UNKNOWN);
							break;
						case WALL_FIRE:
							r1 = get_ran(1,1,6) + 1;
							damage_pc(pc,r1,eDamageType::FIRE,eRace::UNKNOWN);
							break;
						case WALL_ICE:
							r1 = get_ran(2,1,6);
							damage_pc(pc,r1,eDamageType::COLD,eRace::UNKNOWN);
							break;
						case WALL_BLADES:
							r1 = get_ran(4,1,8);
							damage_pc(pc,r1,eDamageType::WEAPON,eRace::UNKNOWN);
							break;
						case OBJECT_BLOCK:
							r1 = get_ran(6,1,8);
							damage_pc(pc,r1,eDamageType::WEAPON,eRace::UNKNOWN);
							break;
						case SPECIAL_EXPLORED: case SPECIAL_SPOT: case SPECIAL_ROAD:
						case BARRIER_FIRE: case BARRIER_FORCE: case BARRIER_CAGE:
						case FIELD_ANTIMAGIC: case FIELD_WEB: case FIELD_QUICKFIRE:
						case FIELD_DISPEL: case FIELD_SMASH:
						case CLOUD_STINK: case CLOUD_SLEEP:
						case OBJECT_CRATE: case OBJECT_BARREL:
						case SFX_SMALL_BLOOD: case SFX_MEDIUM_BLOOD: case SFX_LARGE_BLOOD:
						case SFX_SMALL_SLIME: case SFX_LARGE_SLIME:
						case SFX_ASH: case SFX_BONES: case SFX_RUBBLE:
							break;
					}
				}
			}
	put_pc_screen();
	
	fast_bang = 0;
	
	// Damage to monsters
	for(short k = 0; k < univ.town.monst.size(); k++)
		if((univ.town.monst[k].is_alive()) && (dist(center,univ.town.monst[k].cur_loc) <= 5)) {
			monster_hit = false;
			// First actually make barriers, then draw them, then inflict damaging effects.
			for(short i = minmax(0,univ.town->max_dim - 1,center.x - 4); i <= minmax(0,univ.town->max_dim - 1,center.x + 4); i++)
				for(short j = minmax(0,univ.town->max_dim - 1,center.y - 4); j <= minmax(0,univ.town->max_dim - 1,center.y + 4); j++) {
					spot_hit.x = i;
					spot_hit.y = j;
					
					if(!monster_hit && sight_obscurity(i,j) < 5 && univ.town.monst[k].on_space(spot_hit)) {
						
						if(pat.pattern[i - center.x + 4][j - center.y + 4] > 0)
							monster_hit = true;
						effect = pat.pattern[i - center.x + 4][j - center.y + 4];
						which_m = &univ.town.monst[k];
						if(which_m->abil[eMonstAbil::RADIATE].active && effect == which_m->abil[eMonstAbil::RADIATE].radiate.type)
							continue;
						if(effect >= 50) {
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
							} else if(effect > 370 && effect <= 400) {
								type = eDamageType::SPECIAL;
								dice = effect - 370;
							}
							if(type != eDamageType::MARKED) {
								r1 = get_ran(dice,1,6);
								damage_monst(univ.town.monst[k],who_hit,r1,type);
							}
						} else switch(eFieldType(effect)) {
							case FIELD_WEB:
								which_m->web(3);
								break;
							case WALL_FORCE:
								r1 = get_ran(3,1,6);
								damage_monst(univ.town.monst[k], who_hit, r1, eDamageType::MAGIC);
								break;
							case WALL_FIRE:
								r1 = get_ran(2,1,6);
								damage_monst(univ.town.monst[k], who_hit, r1, eDamageType::FIRE);
								break;
							case CLOUD_STINK:
								which_m->curse(get_ran(1,1,2));
								break;
							case WALL_ICE:
								r1 = get_ran(3,1,6);
								damage_monst(univ.town.monst[k], who_hit, r1, eDamageType::COLD);
								break;
							case WALL_BLADES:
								r1 = get_ran(6,1,8);
								damage_monst(univ.town.monst[k], who_hit, r1, eDamageType::WEAPON);
								break;
							case CLOUD_SLEEP:
								which_m->sleep(eStatus::ASLEEP,3,0);
								break;
							case OBJECT_BLOCK:
								r1 = get_ran(6,1,8);
								damage_monst(univ.town.monst[k],who_hit,r1,eDamageType::WEAPON);
								break;
							case BARRIER_CAGE:
								univ.town.monst[k].status[eStatus::FORCECAGE] = max(8, univ.town.monst[k].status[eStatus::FORCECAGE]);
								break;
							case SPECIAL_EXPLORED: case SPECIAL_SPOT: case SPECIAL_ROAD:
							case OBJECT_CRATE: case OBJECT_BARREL:
							case BARRIER_FIRE: case BARRIER_FORCE:
							case FIELD_ANTIMAGIC: case FIELD_QUICKFIRE:
							case FIELD_DISPEL: case FIELD_SMASH:
							case SFX_SMALL_BLOOD: case SFX_MEDIUM_BLOOD: case SFX_LARGE_BLOOD:
							case SFX_SMALL_SLIME: case SFX_LARGE_SLIME:
							case SFX_ASH: case SFX_BONES: case SFX_RUBBLE:
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
		case eDamageType::WEAPON: code = 170; break;
		case eDamageType::POISON: code = 210; break;
		case eDamageType::UNBLOCKABLE: code = 250; break;
		case eDamageType::UNDEAD: code = 290; break;
		case eDamageType::DEMON: code = 330; break;
		case eDamageType::SPECIAL: code = 370; break;
		case eDamageType::MARKED:
			// Not valid; do nothing.
			return;
	}
	place_spell_pattern(pat, center, code + dice, who_hit);
}

void modify_pattern(effect_pat_type *pat,unsigned short type) {
	for(short i = 0; i < 9; i++)
		for(short j = 0; j < 9; j++)
			if(pat->pattern[i][j] > 0)
				pat->pattern[i][j] = type;
}

void do_shockwave(location target) {
	start_missile_anim();
	for(cPlayer& pc : univ.party)
		if((dist(target,pc.combat_pos) > 0) && (dist(target,pc.combat_pos) < 11)
			&& pc.main_status == eMainStatus::ALIVE)
			damage_pc(pc, get_ran(2 + dist(target,pc.combat_pos) / 2, 1, 6), eDamageType::UNBLOCKABLE,eRace::UNKNOWN);
	for(short i = 0; i < univ.town.monst.size(); i++)
		if((univ.town.monst[i].is_alive()) && (dist(target,univ.town.monst[i].cur_loc) > 0)
			&& (dist(target,univ.town.monst[i].cur_loc) < 11)
			&& (can_see_light(target,univ.town.monst[i].cur_loc,sight_obscurity) < 5))
			damage_monst(univ.town.monst[i],univ.cur_pc,get_ran(2 + dist(target,univ.town.monst[i].cur_loc) / 2,1,6),eDamageType::UNBLOCKABLE);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
}

void radius_damage(location target,short radius, short dam, eDamageType type) {
	// TODO: Why no booms in town mode?
	if(is_town()) {
		for(cPlayer& pc : univ.party)
			if((dist(target,univ.party.town_loc) > 0) && (dist(target,univ.party.town_loc) <= radius)
				&& pc.main_status == eMainStatus::ALIVE)
				damage_pc(pc, dam, type,eRace::UNKNOWN);
		for(short i = 0; i < univ.town.monst.size(); i++)
			if((univ.town.monst[i].is_alive()) && (dist(target,univ.town.monst[i].cur_loc) > 0)
				&& (dist(target,univ.town.monst[i].cur_loc) <= radius)
				&& (can_see_light(target,univ.town.monst[i].cur_loc,sight_obscurity) < 5))
				damage_monst(univ.town.monst[i], univ.cur_pc, dam, type);
		return;
	}
	
	start_missile_anim();
	for(cPlayer& pc : univ.party)
		if((dist(target,pc.combat_pos) > 0) && (dist(target,pc.combat_pos) <= radius)
			&& pc.main_status == eMainStatus::ALIVE)
			damage_pc(pc, dam, type,eRace::UNKNOWN);
	for(short i = 0; i < univ.town.monst.size(); i++)
		if((univ.town.monst[i].is_alive()) && (dist(target,univ.town.monst[i].cur_loc) > 0)
			&& (dist(target,univ.town.monst[i].cur_loc) <= radius)
			&& (can_see_light(target,univ.town.monst[i].cur_loc,sight_obscurity) < 5))
			damage_monst(univ.town.monst[i], univ.cur_pc, dam, type);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
}

void hit_pcs_in_space(location target,short dam,eDamageType type,short report,short hit_all) {
	hit_space(target, dam,type, report, 10 + hit_all);
}

//type; // 0 - weapon   1 - fire   2 - poison   3 - general magic   4 - unblockable  5 - cold
//	6 - demon	7 - undead
//short report; // 0 - no  1 - print result
//hit_all; // 0 - nail top thing   1 - hit all in space  + 10 ... no monsters
void hit_space(location target,short dam,eDamageType type,short report,short hit_all) {
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
	
	for(short i = 0; i < univ.town.monst.size(); i++)
		if((hit_monsters) && (univ.town.monst[i].is_alive()) && !stop_hitting)
			if(univ.town.monst[i].on_space(target)) {
				if(processing_fields)
					damage_monst(univ.town.monst[i], 6, dam, type);
				else damage_monst(univ.town.monst[i], (monsters_going) ? 7 : univ.cur_pc, dam, type);
				stop_hitting = (hit_all == 1) ? false : true;
			}
	
	if(is_combat()) {
		for(cPlayer& pc : univ.party)
			if(pc.main_status == eMainStatus::ALIVE && !stop_hitting)
				if(pc.combat_pos == target) {
					damage_pc(pc,dam,type,eRace::UNKNOWN);
					stop_hitting = (hit_all == 1) ? false : true;
				}
	} else if(target == univ.party.town_loc) {
		fast_bang = 1;
		hit_party(dam,type);
		fast_bang = 0;
		stop_hitting = (hit_all == 1) ? false : true;
	}
	
	if((report == 1) && (hit_all == 0) && !stop_hitting)
		add_string_to_buf("  Missed.");
	
}



void do_poison() {
	short r1 = 0;
	bool some_poison = false;
	
	for(cPlayer& pc : univ.party)
		if(pc.main_status == eMainStatus::ALIVE)
			if(pc.status[eStatus::POISON] > 0)
				some_poison = true;
	if(some_poison) {
		add_string_to_buf("Poison:");
		for(cPlayer& pc : univ.party)
			if(pc.main_status == eMainStatus::ALIVE)
				if(pc.status[eStatus::POISON] > 0) {
					r1 = get_ran(pc.status[eStatus::POISON],1,6);
					damage_pc(pc,r1,eDamageType::POISON,eRace::UNKNOWN);
					if(get_ran(1,0,8) < 6)
						move_to_zero(pc.status[eStatus::POISON]);
					if(get_ran(1,0,8) < 6)
						if(pc.traits[eTrait::GOOD_CONST])
							move_to_zero(pc.status[eStatus::POISON]);
					// TODO: Shouldn't the above two conditionals be swapped?
				}
		put_pc_screen();
		//if(overall_mode < 10)
		//	boom_space(univ.univ.party.p_loc,overall_mode,2,r1);
	}
}


void handle_disease() {
	short r1 = 0;
	bool disease = std::any_of(univ.party.begin(), univ.party.end(), [](const cPlayer& pc) {
		return pc.main_status == eMainStatus::ALIVE && pc.status[eStatus::DISEASE] > 0;
	});
	
	if(disease) {
		add_string_to_buf("Disease:");
		for(cPlayer& pc : univ.party)
			if(pc.main_status == eMainStatus::ALIVE)
				if(pc.status[eStatus::DISEASE] > 0) {
					r1 = get_ran(1,1,10);
					switch(r1) {
						case 1: case 2:
							pc.poison(2);
							break;
						case 3: case 4:
							pc.slow(2);
							break;
						case 5:
							drain_pc(pc,5);
							break;
						case 6: case 7:
							pc.curse(3);
							break;
						case 8:
							pc.dumbfound(3);
							adjust_spell_menus();
							break;
						case 9: case 10:
							add_string_to_buf("  " + pc.name + " unaffected.");
							break;
					}
					r1 = get_ran(1,0,7);
					if(pc.traits[eTrait::GOOD_CONST])
						r1 -= 2;
					if(r1 <= 0 || pc.has_abil_equip(eItemAbil::STATUS_PROTECTION,int(eStatus::DISEASE)))
						move_to_zero(pc.status[eStatus::DISEASE]);
				}
		put_pc_screen();
	}
}

void handle_acid() {
	short r1 = 0;
	bool some_acid = false;
	
	for(cPlayer& pc : univ.party)
		if(pc.main_status == eMainStatus::ALIVE)
			if(pc.status[eStatus::ACID] > 0)
				some_acid = true;
	
	if(some_acid) {
		add_string_to_buf("Acid:");
		for(cPlayer& pc : univ.party)
			if(pc.main_status == eMainStatus::ALIVE)
				if(pc.status[eStatus::ACID] > 0) {
					r1 = get_ran(pc.status[eStatus::ACID],1,6);
					damage_pc(pc,r1,eDamageType::MAGIC,eRace::UNKNOWN);
					move_to_zero(pc.status[eStatus::ACID]);
				}
		if(!is_combat())
			boom_space(univ.party.out_loc,overall_mode,3,r1,8);
	}
}

bool hit_end_c_button() {
	bool end_ok = true;
	
	if(which_combat_type == 0) {
		auto out_monst_remaining = out_monst_alive();
		if(!out_monst_remaining.empty()){
			// Print remaining monsters.
			add_string_to_buf("Enemies are still alive!");
			print_encounter_monsters(store_wandering_special, nullptr, out_monst_remaining);

			end_ok = false;
		}
	}
	for(cPlayer& pc : univ.party) {
		if(pc.status[eStatus::FORCECAGE] > 0) {
			add_string_to_buf("  Someone trapped.");
			return false;
		}
	}
	
	if(end_ok)
		end_combat();
	return end_ok;
}

std::map<std::string,short> out_monst_alive() {
	std::map<std::string,short> monst_alive;
	for(short i = 0; i < univ.town.monst.size(); i++){
		cCreature& m = univ.town.monst[i];
		if(m.is_alive() && !m.is_friendly()) {
			monst_alive[m.m_name]++;
		}
	}
	return monst_alive;
}

void end_combat() {
	for(cPlayer& pc : univ.party) {
		if(pc.main_status == eMainStatus::FLED)
			pc.main_status = eMainStatus::ALIVE;
		pc.status[eStatus::POISONED_WEAPON] = 0;
		pc.status[eStatus::BLESS_CURSE] = 0;
		pc.status[eStatus::HASTE_SLOW] = 0;
		pc.combat_pos = {-1,-1};
	}
	if(which_combat_type == 0) {
		overall_mode = MODE_OUTDOORS;
	}
	combat_active_pc = 6;
	univ.cur_pc = store_current_pc;
	if(univ.current_pc().main_status != eMainStatus::ALIVE)
		univ.cur_pc = first_active_pc();
	put_item_screen(stat_window);
	UI::toolbar.draw(mainPtr);
}


bool combat_cast_mage_spell() {
	short store_sp;
	eSpell spell_num;
	cMonster get_monst;
	
	if(univ.current_pc().traits[eTrait::ANAMA]) {
		add_string_to_buf("Cast: You're an Anama!");
		return false;
	}
	
	if(univ.town.is_antimagic(univ.current_pc().combat_pos.x,univ.current_pc().combat_pos.y)) {
		add_string_to_buf("  Not in antimagic field.");
		return false;
	}
	store_sp = univ.current_pc().cur_sp;
	if(univ.current_pc().cur_sp == 0)
		add_string_to_buf("Cast: No spell points.");
	else if(univ.current_pc().skill(eSkill::MAGE_SPELLS) == 0)
		add_string_to_buf("Cast: No mage skill.");
	else if(univ.current_pc().total_encumbrance(hit_chance) > 1) {
		add_string_to_buf("Cast: Too encumbered.");
		take_ap(6);
		give_help(40,0);
		return true;
	}
	else {
		
		
		if(!spell_forced)
			spell_num = pick_spell(univ.cur_pc,eSkill::MAGE_SPELLS);
		else {
			if(!repeat_cast_ok(eSkill::MAGE_SPELLS))
				return false;
			spell_num = univ.current_pc().last_cast[eSkill::MAGE_SPELLS];
		}
		
		if(univ.current_pc().traits[eTrait::PACIFIST] && !(*spell_num).peaceful) {
			add_string_to_buf("Cast: You're a pacifist!");
			return false;
		}
		
		if(spell_num == eSpell::SIMULACRUM) {
			store_sum_monst = pick_trapped_monst();
			if(store_sum_monst == 0)
				return false;
			get_monst = univ.scenario.scen_monsters[store_sum_monst];
			if(store_sp < get_monst.level) {
				add_string_to_buf("Cast: Not enough spell points.");
				return false;
			}
			store_sum_monst_cost = get_monst.level;
		}
		
		combat_posing_monster = current_working_monster = univ.cur_pc;
		if(spell_num == eSpell::NONE) return false;
		print_spell_cast(spell_num,eSkill::MAGE_SPELLS);
		if((*spell_num).refer == REFER_YES) {
			take_ap(6);
			draw_terrain(2);
			do_mage_spell(univ.cur_pc,spell_num);
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
			combat_immed_mage_cast(univ.cur_pc,spell_num);
		}
		put_pc_screen();
	}
	combat_posing_monster = current_working_monster = -1;
	// Did anything actually get cast?
	if(store_sp == univ.current_pc().cur_sp)
		return false;
	else return true;
}

void combat_immed_mage_cast(short current_pc, eSpell spell_num, bool freebie) {
	short target, num_opp = 0, r1;
	snd_num_t store_sound = 0;
	miss_num_t store_m_type = 0;
	cPlayer& caster = univ.party[current_pc];
	short bonus = freebie ? 1 : caster.stat_adj(eSkill::INTELLIGENCE);
	short level = freebie ? store_item_spell_level : caster.level;
	if(!freebie && (*spell_num).level <= caster.get_prot_level(eItemAbil::MAGERY))
		level++;
	cCreature* which_m;
	start_missile_anim();
	switch(spell_num) {
		case eSpell::SHOCKWAVE:
			if(!freebie)
				caster.cur_sp -= (*spell_num).cost;
			add_string_to_buf("  The ground shakes!");
			do_shockwave(caster.combat_pos);
			break;
			
		case eSpell::HASTE_MINOR: case eSpell::HASTE: case eSpell::STRENGTH: case eSpell::ENVENOM: case eSpell::RESIST_MAGIC:
//			target = select_pc(11,0);
			target = store_spell_target;
			if(target < 6) {
				cPlayer& target_pc = univ.party[target];
				if(!freebie)
					caster.cur_sp -= (*spell_num).cost;
				play_sound(4);
				std::string c_line = "  " + target_pc.name;
				switch(spell_num) {
					case eSpell::ENVENOM:
						c_line += " receives venom.";
						poison_weapon(target,3 + bonus,true);
						store_m_type = 11;
						break;
						
					case eSpell::STRENGTH:
						c_line += " stronger.";
						target_pc.curse(-3);
						store_m_type = 8;
						break;
					case eSpell::RESIST_MAGIC:
						c_line += " resistant.";
						target_pc.status[eStatus::MAGIC_RESISTANCE] += 5 + bonus;
						store_m_type = 15;
						break;
						
					default:
						target_pc.slow((spell_num == eSpell::HASTE_MINOR) ? -2 : -max(2,level / 2 + bonus));
						c_line += " hasted.";
						store_m_type = 8;
						break;
				}
				add_string_to_buf(c_line);
				add_missile(target_pc.combat_pos,store_m_type,0,0,0);
			}
			break;
			
		case eSpell::HASTE_MAJOR: case eSpell::BLESS_MAJOR:
			store_sound = 25;
			if(!freebie)
				caster.cur_sp -= (*spell_num).cost;
			
			
			for(short i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE) {
					univ.party[i].slow(-(spell_num == eSpell::HASTE_MAJOR ? 1 + level / 8 + bonus : 3 + bonus));
					if(spell_num == eSpell::BLESS_MAJOR) {
						poison_weapon(i,2,true);
						univ.party[i].curse(-4);
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
			
			
			
		case eSpell::SLOW_GROUP: case eSpell::FEAR_GROUP: case eSpell::PARALYSIS_MASS: case eSpell::SLEEP_MASS:
			if(!freebie)
				caster.cur_sp -= (*spell_num).cost;
			store_sound = 25;
			if(spell_num == eSpell::FEAR_GROUP)
				store_sound = 54;
			switch(spell_num) {
				case eSpell::SLOW_GROUP: add_string_to_buf("  Enemy slowed:"); break;
				case eSpell::RAVAGE_ENEMIES: add_string_to_buf("  Enemy ravaged:"); break;
				case eSpell::FEAR_GROUP: add_string_to_buf("  Enemy scared:"); break;
				case eSpell::PARALYSIS_MASS: add_string_to_buf("  Enemy paralyzed:"); break;
				case eSpell::SLEEP_MASS: add_string_to_buf("  Enemy drowsy:"); break;
				default:
					add_string_to_buf("  Error: Mage group spell " + (*spell_num).name() + " not implemented for combat mode.", 4);
					break;
			}
			for(short i = 0; i < univ.town.monst.size(); i++) {
				if(univ.town.monst[i].is_alive() && !univ.town.monst[i].is_friendly()
					&& (dist(caster.combat_pos,univ.town.monst[i].cur_loc) <= (*spell_num).range)
					&& (can_see_light(caster.combat_pos,univ.town.monst[i].cur_loc,sight_obscurity) < 5)) {
					which_m = &univ.town.monst[i];
					switch(spell_num) {
						case eSpell::FEAR_GROUP:
							r1 = get_ran(level / 3,1,8);
							which_m->scare(r1);
							store_m_type = 10;
							break;
						case eSpell::SLOW_GROUP: case eSpell::RAVAGE_ENEMIES:
							which_m->slow(5 + bonus);
							if(spell_num == eSpell::RAVAGE_ENEMIES)
								which_m->curse(3 + bonus);
							store_m_type = 8;
							break;
						case eSpell::PARALYSIS_MASS:
							which_m->sleep(eStatus::PARALYZED,1000,15);
							store_m_type = 15;
							break;
						case eSpell::SLEEP_MASS:
							which_m->sleep(eStatus::ASLEEP,8,15);
							store_m_type = 15;
							break;
						default: break; // Silence compiler warning
					}
					num_opp++;
					add_missile(univ.town.monst[i].cur_loc,store_m_type,0,
								14 * (which_m->x_width - 1),18 * (which_m->y_width - 1));
				}
				
			}
			break;
			
		case eSpell::BLADE_AURA: // Pyhrrus effect
			place_spell_pattern(radius2,caster.combat_pos,WALL_BLADES,6);
			break;
		case eSpell::FLAME_AURA:
			place_spell_pattern(open_square, caster.combat_pos, eDamageType::FIRE, 6, current_pc);
			break;
		default:
			add_string_to_buf("  Error: Mage spell " + (*spell_num).name() + " not implemented for combat mode.", 4);
			break;
	}
	if(num_opp < 10)
		do_missile_anim((num_opp < 5) ? 50 : 25,caster.combat_pos,store_sound);
	else play_sound(store_sound);
	end_missile_anim();
}

bool combat_cast_priest_spell() {
	short store_sp;
	eSpell spell_num;
	
	if(univ.town.is_antimagic(univ.current_pc().combat_pos.x,univ.current_pc().combat_pos.y)) {
		add_string_to_buf("  Not in antimagic field.");
		return false;
	}
	if(!spell_forced)
		spell_num = pick_spell(univ.cur_pc,eSkill::PRIEST_SPELLS);
	else {
		if(!repeat_cast_ok(eSkill::PRIEST_SPELLS))
			return false;
		spell_num = univ.current_pc().last_cast[eSkill::PRIEST_SPELLS];
	}
	
	store_sp = univ.current_pc().cur_sp;
	if(univ.current_pc().cur_sp == 0) {
		add_string_to_buf("Cast: No spell points.");
		return false;
	} else if(univ.current_pc().skill(eSkill::PRIEST_SPELLS) == 0) {
		add_string_to_buf("Cast: No priest skill.");
		return false;
	}
	
	if(univ.current_pc().traits[eTrait::PACIFIST] && !(*spell_num).peaceful) {
		add_string_to_buf("Cast: You're a pacifist!");
		return false;
	}
	
	if(spell_num == eSpell::NONE) return false;
	
	combat_posing_monster = current_working_monster = univ.cur_pc;
	
	if(spell_num != eSpell::NONE) {
		print_spell_cast(spell_num,eSkill::PRIEST_SPELLS);
		if((*spell_num).refer == REFER_YES) {
			take_ap(5);
			draw_terrain(2);
			do_priest_spell(univ.cur_pc,spell_num);
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
			combat_immed_priest_cast(univ.cur_pc, spell_num);
		}
		put_pc_screen();
	}
	
	combat_posing_monster = current_working_monster = -1;
	// Did anything actually get cast?
	if(store_sp == univ.current_pc().cur_sp)
		return false;
	else return true;
}

void combat_immed_priest_cast(short current_pc, eSpell spell_num, bool freebie) {
	short target = store_spell_target,num_opp = 0;
	snd_num_t store_sound = 0;
	miss_num_t store_m_type = 0;
	cPlayer& caster = univ.party[current_pc];
	short bonus = freebie ? 1 : caster.stat_adj(eSkill::INTELLIGENCE);
	short level = freebie ? store_item_spell_level : caster.level;
	if(!freebie && caster.traits[eTrait::ANAMA])
		level++;
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
			if(target < 6) {
				store_sound = 4;
				if(!freebie)
					caster.cur_sp -= (*spell_num).cost;
				univ.party[target].curse(-(spell_num==eSpell::BLESS_MINOR ? 2 : max(2,(level * 3) / 4 + 1 + bonus)));
				add_missile(univ.party[target].combat_pos,8,0,0,0);
			}
			break;
			
		case eSpell::BLESS_PARTY:
			if(!freebie)
				caster.cur_sp -= (*spell_num).cost;
			for(cPlayer& pc : univ.party)
				if(pc.main_status == eMainStatus::ALIVE) {
					pc.curse(-(level / 3));
					add_missile(pc.combat_pos,8,0,0,0);
				}
			store_sound = 4;
			break;
			
		case eSpell::AVATAR:
			if(!freebie)
				caster.cur_sp -= (*spell_num).cost;
			add_string_to_buf("  " + caster.name + " is an avatar!");
			caster.avatar();
			break;
			
		case eSpell::CURSE_ALL: case eSpell::CHARM_MASS: case eSpell::PESTILENCE:
			if(!freebie)
				caster.cur_sp -= (*spell_num).cost;
			store_sound = 24;
			for(short i = 0; i < univ.town.monst.size(); i++) {
				if(univ.town.monst[i].is_alive() && !univ.town.monst[i].is_friendly() &&
					(dist(caster.combat_pos,univ.town.monst[i].cur_loc) <= (*spell_num).range)) {
					// TODO: Should this ^ also check that you can see each target? ie can_see_light(...) < 5
					// --> can_see_light(caster.combat_pos,univ.town.monst[i].cur_loc,sight_obscurity)
					// (The item version of the spell used to check for this, but no longer does since it now defers to here.)
					which_m = &univ.town.monst[i];
					switch(spell_num) {
						case eSpell::CURSE_ALL:
							which_m->curse(3 + bonus);
							store_m_type = 8;
							break;
						case eSpell::CHARM_MASS:
							// TODO: As an item spell, the penalty was 0, though perhaps it was intended to be 8
							// (since 8 was passed as the amount argument). Now the penalty has increased to 27.
							// It should probably be put back somehow.
							which_m->sleep(eStatus::CHARM,0,28 - bonus);
							store_m_type = 14;
							break;
						case eSpell::PESTILENCE:
							which_m->disease(3 + bonus);
							store_m_type = 0;
							break;
						default:
							add_string_to_buf("  Error: Priest group spell " + (*spell_num).name() + " not implemented for combat mode.", 4);
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
				caster.cur_sp -= (*spell_num).cost;
			play_sound(24);
			add_string_to_buf("  Protective field created.");
			place_spell_pattern(protect_pat,caster.combat_pos,6);
			break;
		case eSpell::AUGMENTATION:
			if(target < 6) {
				add_string_to_buf("  Health augmented!");
				int i = get_ran(3,1,6);
				univ.party[target].cur_health += i;
			}
			break;
		case eSpell::NIRVANA:
			if(target < 6) {
				add_string_to_buf("  Enlightened!");
				int i = get_ran(3,1,6);
				univ.party[target].apply_status(eStatus::DUMB, i / -3);
				univ.party[target].cur_sp += i * 2;
				adjust_spell_menus();
			}
			break;
		default:
			add_string_to_buf("  Error: Priest spell " + (*spell_num).name() + " not implemented for combat mode.", 4);
			break;
	}
	if(num_opp < 10)
		do_missile_anim((num_opp < 5) ? 50 : 25,caster.combat_pos,store_sound);
	else play_sound(store_sound);
	end_missile_anim();
}

void start_spell_targeting(eSpell num, bool freebie, int spell_range, eSpellPat pat) {
	
	// First, remember what spell was cast.
	spell_being_cast = num;
	spell_freebie = freebie;
	
	add_string_to_buf("  Target spell.");
	if(num == eSpell::NONE);
	else if(isMage(num))
		add_string_to_buf("  (Hit 'm' to cancel.)");
	else add_string_to_buf("  (Hit 'p' to cancel.)");
	overall_mode = MODE_SPELL_TARGET;
	current_spell_range = num == eSpell::NONE ? spell_range : (*num).range;
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
		case eSpell::NONE:
			switch(pat) {
				case PAT_SINGLE: current_pat = single; break;
				case PAT_SQ: current_pat = square; break;
				case PAT_SMSQ: current_pat = small_square; break;
				case PAT_OPENSQ: current_pat = open_square; break;
				case PAT_PLUS: current_pat = t; break;
				case PAT_RAD2: current_pat = radius2; break;
				case PAT_RAD3: current_pat = radius3; break;
				case PAT_WALL:
					add_string_to_buf("  (Hit space to rotate.)");
					force_wall_position = 0;
					current_pat = field[0];
					break;
			}
			break;
		default:
			if((*num).refer != REFER_TARGET)
				add_string_to_buf("  Error: Entered targeting for non-targeted spell " + (*num).name(), 4);
			break;
	}
}

void start_fancy_spell_targeting(eSpell num, bool freebie, int spell_range, eSpellPat pat, int targets) {
	location null_loc(-1,0);
	
	// First, remember what spell was cast.
	spell_being_cast = num;
	spell_freebie = freebie;
	
	for(short i = 0; i < 8; i++)
		spell_targets[i] = null_loc;
	add_string_to_buf("  Target spell.");
	if(num == eSpell::NONE);
	else if(isMage(num))
		add_string_to_buf("  (Hit 'm' to cancel.)");
	else add_string_to_buf("  (Hit 'p' to cancel.)");
	add_string_to_buf("  (Hit space to cast.)");
	overall_mode = MODE_FANCY_TARGET;
	current_pat = single;
	current_spell_range = num == eSpell::NONE ? spell_range : (*num).range;
	short bonus = univ.current_pc().stat_adj(eSkill::INTELLIGENCE);
	short level = freebie ? store_item_spell_level : univ.current_pc().level;
	
	switch(num) { // Assign special targeting shapes and number of targets
		case eSpell::SMITE:
			num_targets_left = minmax(1,8,level / 4 + bonus / 2);
			break;
		case eSpell::STICKS_TO_SNAKES:
			num_targets_left = level / 5 + bonus / 2;
			break;
		case eSpell::SUMMON_HOST:
			num_targets_left = 5;
			break;
		case eSpell::ARROWS_FLAME:
			num_targets_left = level / 4 + bonus / 2;
			break;
		case eSpell::ARROWS_VENOM:
			num_targets_left = level / 5 + bonus / 2;
			break;
		case eSpell::ARROWS_DEATH: case eSpell::PARALYZE:
			num_targets_left = level / 8 + bonus / 3;
			break;
		case eSpell::SPRAY_FIELDS:
			num_targets_left = level / 5 + bonus / 2;
			current_pat = t;
			break;
		case eSpell::SUMMON_WEAK:
			num_targets_left = minmax(1,7,level / 4 + bonus / 2);
			break;
		case eSpell::SUMMON:
			num_targets_left = minmax(1,6,level / 6 + bonus / 2);
			break;
		case eSpell::SUMMON_MAJOR:
			num_targets_left = minmax(1,5,level / 8 + bonus / 2);
			break;
		case eSpell::NONE:
			num_targets_left = minmax(1,8,targets);
			switch(pat) {
				case PAT_SINGLE: current_pat = single; break;
				case PAT_SQ: current_pat = square; break;
				case PAT_SMSQ: current_pat = small_square; break;
				case PAT_OPENSQ: current_pat = open_square; break;
				case PAT_PLUS: current_pat = t; break;
				case PAT_RAD2: current_pat = radius2; break;
				case PAT_RAD3: current_pat = radius3; break;
				case PAT_WALL: current_pat = single; break; // Fancy targeting doesn't support the rotateable pattern
			}
			break;
		default:
			if((*num).refer == REFER_FANCY)
				std::cout << "  Warning: Spell " << (*num).name() << " didn't assign target shape and count." << std::endl;
			else add_string_to_buf("  Error: Entered fancy targeting for non-fancy-targeted spell " + (*num).name(), 4);
			break;
	}
	
	num_targets_left = minmax(1,8,num_targets_left);
}

void spell_cast_hit_return() {
	if(recording){
		record_action("spell_cast_hit_return", "");
	}
	
	if(force_wall_position < 10) {
		force_wall_position = (force_wall_position + 1) % 8;
		current_pat = field[force_wall_position];
	}
}

void break_force_cage(location loc) {
	for(cPlayer& pc : univ.party) {
		if(pc.get_loc() == loc)
			pc.status[eStatus::FORCECAGE] = 0;
	}
	for(int j = 0; j < univ.town.monst.size(); j++) {
		if(univ.town.monst[j].get_loc() == loc)
			univ.town.monst[j].status[eStatus::FORCECAGE] = 0;
	}
	univ.town.set_force_cage(loc.x, loc.y, false);
}

void process_force_cage(location loc, short i, short adjust) {
	if(!univ.town.is_force_cage(loc.x,loc.y)) return;
	if(i >= 100) {
		short m = i - 100;
		cCreature& which_m = univ.town.monst[m];
		move_to_zero(which_m.status[eStatus::FORCECAGE]);
		if(which_m.status[eStatus::FORCECAGE] == 0) {
			add_string_to_buf("  Force cage flickers out.");
			break_force_cage(loc);
			return;
		}
		if(!which_m.is_friendly() && get_ran(1,1,100) < which_m.mu * 10 + which_m.cl * 4 + 5 + adjust) {
			// TODO: This sound is not right
			play_sound(60);
			which_m.spell_note(50);
			break_force_cage(loc);
		}
	} else if(i < 0) {
		// Force cages without occupants can last a very long time
		if(get_ran(1,1,1000) == 1)
			break_force_cage(loc);
	} else if(i < 6) {
		cPlayer& who = univ.party[i];
		move_to_zero(who.status[eStatus::FORCECAGE]);
		if(who.status[eStatus::FORCECAGE] == 0) {
			add_string_to_buf("  Force cage flickers out.");
			break_force_cage(loc);
			return;
		}
		// We want to make sure everyone has a chance of eventually breaking a cage, because it never ends on its own,
		// and because being trapped unconditionally prevents you from ending combat mode.
		short bonus = 5 + who.skill(eSkill::MAGE_LORE) + adjust;
		if(get_ran(1,1,100) < who.skill(eSkill::MAGE_SPELLS)*10 + who.skill(eSkill::PRIEST_SPELLS)*4 + bonus) {
			play_sound(60);
			add_string_to_buf("  " + who.name + " breaks force cage.");
			break_force_cage(loc);
		}
	}
}

void process_fields() {
	short r1;
	location loc;
	short qf[64][64];
	rectangle r;
	
	if(is_out())
		return;
	
	if(univ.town.quickfire_present) {
		r = univ.town->in_town_rect;
		for(short i = 0; i < univ.town->max_dim; i++)
			for(short j = 0; j < univ.town->max_dim; j++)
				qf[i][j] = (univ.town.is_quickfire(i,j)) ? 2 : 0;
		for(short k = 0; k < ((is_combat()) ? 4 : 1); k++) {
			for(short i = r.left + 1; i < r.right ; i++)
				for(short j = r.top + 1; j < r.bottom ; j++)
					if(univ.town.is_quickfire(i,j)) {
						r1 = get_ran(1,1,8);
						if(r1 != 1) {
							qf[i - 1][j] = 1;
							qf[i + 1][j] = 1;
							qf[i][j + 1] = 1;
							qf[i][j - 1] = 1;
						}
					}
			for(short i = r.left + 1; i < r.right ; i++)
				for(short j = r.top + 1; j < r.bottom ; j++)
					if(qf[i][j] > 0) {
						ter_num_t ter = univ.town->terrain(i,j);
						if(univ.scenario.ter_types[ter].special == eTerSpec::CRUMBLING && univ.scenario.ter_types[ter].flag2 > 0) {
							// TODO: This seems like the wrong sound
							play_sound(60);
							univ.town->terrain(i,j) = univ.scenario.ter_types[ter].flag1;
							add_string_to_buf("  Quickfire burns through barrier.");
						}
						univ.town.set_quickfire(i,j,true);
					}
		}
	}
	
	for(short i = 0; i < univ.town.monst.size(); i++)
		if(univ.town.monst[i].is_alive())
			monst_inflict_fields(i);
	
	sync_force_cages();
	
	// First fry PCs, then call to handle damage to monsters
	processing_fields = true; // this, in hit_space, makes damage considered to come from whole party
	for(short i = 0; i < univ.town->max_dim; i++)
		for(short j = 0; j < univ.town->max_dim; j++) {
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
				short who = univ.get_target_i(*univ.target_there(loc));
				process_force_cage(loc, who);
				// If we got a PC, check the others too, in case they're on the same space
				while(++who > 0 && who < 6 && univ.town.is_force_cage(i,j)) {
					loc = univ.party[who].get_loc();
					process_force_cage(loc, who);
				}
			}
		}
	
	processing_fields = false;
	monsters_going = true; // this changes who the damage is considered to come from in hit_space
	
	if(univ.town.quickfire_present) {
		for(short i = 0; i < univ.town->max_dim; i++)
			for(short j = 0; j < univ.town->max_dim; j++)
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
	
	target.x = (char) m;
	target.y = (char) n;
	
	univ.town.set_scloud(m,n,true);
	
	if(is_combat()) {
		for(cPlayer& pc : univ.party)
			if(pc.main_status == eMainStatus::ALIVE)
				if(pc.combat_pos == target) {
					pc.curse(get_ran(1,1,2));
				}
	} else if(target == univ.party.town_loc) {
		for(cPlayer& pc : univ.party)
			if(pc.main_status == eMainStatus::ALIVE)
				pc.curse(get_ran(1,1,2));
	}
}

void web_space(short m,short n) {
	location target(m, n);
	
	univ.town.set_web(m,n,true);
	
	if(is_combat()) {
		for(cPlayer& pc : univ.party)
			if(pc.main_status == eMainStatus::ALIVE)
				if(pc.combat_pos == target) {
					pc.web(3);
				}
	} else if(target == univ.party.town_loc) {
		for(cPlayer& pc : univ.party)
			pc.web(3);
	}
}
void sleep_cloud_space(short m,short n) {
	location target(m, n);
	
	univ.town.set_sleep_cloud(m,n,true);
	
	if(is_combat()) {
		for(cPlayer& pc : univ.party)
			if(pc.main_status == eMainStatus::ALIVE)
				if(pc.combat_pos == target) {
					pc.sleep(eStatus::ASLEEP,3,0);
				}
	} else if(target == univ.party.town_loc) {
		univ.party.sleep(eStatus::ASLEEP,3,0);
	}
}


void take_m_ap(short num,cCreature *monst) {
	monst->ap = max(0,monst->ap - num);
}

// TODO: This is used nowhere. Should it be?
void add_new_action(short pc_num) {
	if(pc_num < 6)
		univ.party[pc_num].ap++;
}

short get_monst_sound(cCreature *attacker,short which_att) {
	switch(attacker->a[which_att].type) {
		case eMonstMelee::SLIME:
			return 11;
		case eMonstMelee::PUNCH:
			return 4;
		case eMonstMelee::CLAW:
			return 9;
		case eMonstMelee::BITE:
			return 10;
		case eMonstMelee::STING:
			return 12;
		case eMonstMelee::CLUB:
			return 4;
		case eMonstMelee::BURN:
			return 5;
		case eMonstMelee::HARM:
			return 0;
		case eMonstMelee::STAB:
		case eMonstMelee::SWING:
			// TODO: These sounds don't quite seem right.
			// They're passed to boom_space, so 0 = ouch, 1 = small sword, 2 = loud sword, 3 = pole, 4 = club
			if(attacker->m_type == eRace::HUMAN) {
				if(attacker->a[which_att].sides > 9)
					return 3;
				else return 2;
			}
			if(attacker->m_type == eRace::MAGE)
				return 1;
			if(attacker->m_type == eRace::PRIEST)
				return 4;
			if(isHumanoid(attacker->m_type) || attacker->m_type == eRace::GIANT) {
				return 2;
			}
			return 1;
	}
	return 0;
}
