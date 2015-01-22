
#include <cstdio>
#include <cstring>
#include <queue>
#include <map>

#include "boe.global.h"

#include "classes.h"
#include "boe.party.h"
#include "boe.town.h"
#include "boe.text.h"
#include "boe.infodlg.h"
#include "boe.items.h"
#include "boe.itemdata.h"
#include "boe.combat.h"
#include "boe.monster.h"
#include "boe.locutils.h"
#include "boe.actions.h"
#include "soundtool.hpp"
#include "boe.townspec.h"
#include "boe.graphics.h"
#include "boe.fileio.h"
#include "boe.specials.h"
#include "boe.newgraph.h"
#include "boe.dlgutil.h"
#include "mathutil.hpp"
#include "boe.main.h"
#include "dlogutil.hpp"
#include "fileio.hpp"
#include <array>
#include "spell.hpp"

extern sf::RenderWindow mainPtr;
extern eGameMode overall_mode;
extern short which_combat_type,current_pc,stat_window;
extern location center;
extern bool in_scen_debug,belt_present,processing_fields,monsters_going,suppress_stat_screen,boom_anim_active;
extern effect_pat_type single,t,square,radius2,radius3,small_square,open_square,field[8];
extern effect_pat_type current_pat;
extern cOutdoors::cWandering store_wandering_special;
extern eSpell spell_being_cast, town_spell;
extern short spell_caster;
extern sf::RenderWindow mini_map;
extern short fast_bang;
extern bool end_scenario;
extern cUniverse univ;
extern std::queue<pending_special_type> special_queue;
extern short combat_posing_monster;

bool can_draw_pcs = true;
bool fog_lifted = false;

std::map<eDamageType,int> boom_gr = {
	{eDamageType::WEAPON, 3},
	{eDamageType::FIRE, 0},
	{eDamageType::POISON, 2},
	{eDamageType::MAGIC, 1},
	{eDamageType::UNBLOCKABLE, 1},
	{eDamageType::COLD, 4},
	{eDamageType::UNDEAD, 3},
	{eDamageType::DEMON, 3},
};
short store_item_spell_level = 10;

// global values for when processing special encounters
short current_pc_picked_in_spec_enc = -1; // pc that's been selected, -1 if none
extern std::map<eSkill,short> skill_max;
location store_special_loc;
bool special_in_progress = false;

// 0 - can't use 1 - combat only 2 - town only 3 - town & combat only  4 - everywhere  5 - outdoor
// + 10 - mag. inept can use
std::map<eItemAbil, short> abil_chart = {
	{eItemAbil::POISON_WEAPON,13}, {eItemAbil::AFFECT_STATUS,3}, {eItemAbil::BLISS_DOOM,3}, {eItemAbil::AFFECT_EXPERIENCE,4},
	{eItemAbil::AFFECT_SKILL_POINTS,4}, {eItemAbil::AFFECT_HEALTH,4}, {eItemAbil::AFFECT_SPELL_POINTS,4},
	{eItemAbil::LIGHT,13}, {eItemAbil::AFFECT_PARTY_STATUS,3}, {eItemAbil::HEALTH_POISON,4},
	{eItemAbil::CALL_SPECIAL,4}, {eItemAbil::CAST_SPELL,4},
};

// TODO: I bet this is completely unused; it looks like it does nothing.
//short which; // number, 0 - 49, of special
bool town_specials(short which,short /*t_num*/) {
	bool can_enter = true;
	short spec_id;
	location l;
	
	
	l = univ.town->special_locs[which];
	spec_id = univ.town->spec_id[which];
	if(spec_id < 0)
		return true;
	
	// call special
	
	erase_specials();
	
	return can_enter;
}

// which is unused
//short mode; // 0 - pre  1 - end by victory  2 - end by flight
// wanderin spec 99 -> generic spec
bool handle_wandering_specials (short /*which*/,short mode) {
	
	// TODO: Is loc_in_sec the correct location to pass here?
	// (I'm pretty sure it is, but I should verify it somehow.)
	// (It's either that or univ.party.p_loc.)
	short s1 = 0,s2 = 0,s3 = 0;
	
	if((mode == 0) && (store_wandering_special.spec_on_meet >= 0)) { // When encountering
		run_special(eSpecCtx::OUTDOOR_ENC,1,store_wandering_special.spec_on_meet,univ.party.loc_in_sec,&s1,&s2,&s3);
		if(s1 > 0)
			return false;
	}
	
	if((mode == 1) && (store_wandering_special.spec_on_win >= 0))  {// After defeating
		run_special(eSpecCtx::WIN_ENCOUNTER,1,store_wandering_special.spec_on_win,univ.party.loc_in_sec,&s1,&s2,&s3);
	}
	if((mode == 2) && (store_wandering_special.spec_on_flee >= 0))  {// After fleeing like a buncha girly men
		run_special(eSpecCtx::FLEE_ENCOUNTER,1,store_wandering_special.spec_on_flee,univ.party.loc_in_sec,&s1,&s2,&s3);
	}
	return true;
}


// returns true if can enter this space
// sets forced to true if definitely can enter
bool check_special_terrain(location where_check,eSpecCtx mode,short which_pc,short *spec_num,
						   bool *forced) {
	ter_num_t ter;
	short r1,i,door_pc,pic_type = 0,ter_pic = 0;
	eTerSpec ter_special;
	std::string choice;
	ter_flag_t ter_flag1,ter_flag2,ter_flag3;
	eDamageType dam_type = eDamageType::WEAPON;
	bool can_enter = true;
	location out_where,from_loc,to_loc;
	short s1 = 0,s2 = 0,s3 = 0;
	
	*spec_num = -1;
	*forced = false;
	
	switch(mode) {
		case eSpecCtx::OUT_MOVE:
			ter = univ.out[where_check.x][where_check.y];
			from_loc = univ.party.p_loc;
			break;
		case eSpecCtx::TOWN_MOVE:
			ter = univ.town->terrain(where_check.x,where_check.y);
			from_loc = univ.town.p_loc;
			break;
		case eSpecCtx::COMBAT_MOVE:
			ter = univ.town->terrain(where_check.x,where_check.y);
			from_loc = univ.party[current_pc].combat_pos;
			break;
		default:
			// No movement happened, so just return false.
			// TODO: Should there be an error message here?
			std::cout << "Note: Improper mode passed to check_special_terrain: " << int(mode) << std::endl;
			return false;
	}
	ter_special = univ.scenario.ter_types[ter].special;
	ter_flag1 = univ.scenario.ter_types[ter].flag1;
	ter_flag2 = univ.scenario.ter_types[ter].flag2;
	ter_flag3 = univ.scenario.ter_types[ter].flag3;
	ter_pic = univ.scenario.ter_types[ter].picture;
	
	// TODO: Why not support conveyors outdoors, too?
	if(mode != eSpecCtx::OUT_MOVE && ter_special == eTerSpec::CONVEYOR) {
		if(
			((ter_flag3.u == DIR_N) && (where_check.y > from_loc.y)) ||
			((ter_flag3.u == DIR_E) && (where_check.x < from_loc.x)) ||
			((ter_flag3.u == DIR_S) && (where_check.y < from_loc.y)) ||
			((ter_flag3.u == DIR_W) && (where_check.x > from_loc.x)) ) {
			ASB("The moving floor prevents you.");
			return false;
		}
	}
	
	if(mode == eSpecCtx::OUT_MOVE) {
		out_where = global_to_local(where_check);
		
		for(i = 0; i < 18; i++)
			if(out_where == univ.out->special_locs[i]) {
				*spec_num = univ.out->special_id[i];
				// call special
				run_special(mode,1,univ.out->special_id[i],out_where,&s1,&s2,&s3);
				if(s1 > 0)
					can_enter = false;
				else if(s2 > 0)
					*forced = true;
				erase_out_specials();
				put_pc_screen();
				put_item_screen(stat_window,0);
			}
	}
	
	if((is_combat()) && (univ.town.is_spot(where_check.x, where_check.y) ||
						  (univ.scenario.ter_types[coord_to_ter(where_check.x, where_check.y)].trim_type == eTrimType::CITY))) {
		ASB("Move: Can't trigger this special in combat.");
		return false; // TODO: Maybe replace eTrimType::CITY check with a blockage == clear/special && is_special() check?
	}
	
	if((mode == eSpecCtx::TOWN_MOVE || (mode == eSpecCtx::COMBAT_MOVE && which_combat_type == 1))
	   && (univ.town.is_special(where_check.x,where_check.y))) {
		if(univ.town.is_force_barr(where_check.x,where_check.y)) {
			add_string_to_buf("  Magic barrier!               ");
			return false;
		}
		if(univ.town.is_force_cage(where_check.x,where_check.y)) {
			add_string_to_buf("  Force cage!");
			return false;
		}
		for(i = 0; i < 50; i++)
			if(where_check == univ.town->special_locs[i]) {
				*spec_num = univ.town->spec_id[i];
				bool runSpecial = false;
				if(!is_blocked(where_check)) runSpecial = true;
				if(ter_special == eTerSpec::CHANGE_WHEN_STEP_ON) runSpecial = true;
				if(ter_special == eTerSpec::CALL_SPECIAL) runSpecial = true;
				if(!PSD[SDF_NO_BOAT_SPECIALS] && univ.party.in_boat >= 0 && univ.scenario.ter_types[ter].boat_over)
					runSpecial = true;
				if(runSpecial) {
					give_help(54,0);
					run_special(mode,2,univ.town->spec_id[i],where_check,&s1,&s2,&s3);
					if(s1 > 0)
						can_enter = false;
					else if(s2 > 0)
						*forced = true;
				}
			}
		put_pc_screen();
		put_item_screen(stat_window,0);
	}
	
	// TODO: Just verify that yes, it works with this and doesn't work without it.
	if(mode == eSpecCtx::COMBAT_MOVE && town_boat_there(where_check) < 3) {
		add_string_to_buf("Blocked: Can't enter boats in combat");
		can_enter = false;
	}
	
	if(!can_enter)
		return false;
	
	if((!is_out()) && (overall_mode < MODE_TALKING)) {
		check_fields(where_check,mode,which_pc);
		
		if(univ.town.is_web(where_check.x,where_check.y)) {
			add_string_to_buf("  Webs!               ");
			if(mode != eSpecCtx::COMBAT_MOVE) {
				suppress_stat_screen = true;
				for(i = 0; i < 6; i++) {
					r1 = get_ran(1,2,3);
					web_pc(i,r1);
				}
				suppress_stat_screen = true;
				put_pc_screen();
			}
			else web_pc(current_pc,get_ran(1,2,3));
			univ.town.set_web(where_check.x,where_check.y,false);
		}
		if(univ.town.is_force_barr(where_check.x,where_check.y)) {
			add_string_to_buf("  Magic barrier!               ");
			can_enter = false;
		}
		if(univ.town.is_force_cage(where_check.x,where_check.y)) {
			add_string_to_buf("  Force cage!               ");
			can_enter = false;
		}
		if(univ.town.is_crate(where_check.x,where_check.y)) {
			add_string_to_buf("  You push the crate.");
			to_loc = push_loc(from_loc,where_check);
			univ.town.set_crate(where_check.x,where_check.y,false);
			if(to_loc.x > 0)
				univ.town.set_crate(to_loc.x,to_loc.y,true);
			for(i = 0; i < NUM_TOWN_ITEMS; i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where_check
				   && (univ.town.items[i].contained))
					univ.town.items[i].item_loc = to_loc;
		}
		if(univ.town.is_barrel(where_check.x,where_check.y)) {
			add_string_to_buf("  You push the barrel.");
			to_loc = push_loc(from_loc,where_check);
			univ.town.set_barrel(where_check.x,where_check.y,false);
			if(to_loc.x > 0)
				univ.town.set_barrel(to_loc.x,to_loc.y,false);
			for(i = 0; i < NUM_TOWN_ITEMS; i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where_check
				   && (univ.town.items[i].contained))
					univ.town.items[i].item_loc = to_loc;
		}
		if(univ.town.is_block(where_check.x,where_check.y)) {
			add_string_to_buf("  You push the stone block.");
			to_loc = push_loc(from_loc,where_check);
			univ.town.set_block(where_check.x,where_check.y,false);
			if(to_loc.x > 0)
				univ.town.set_block(to_loc.x,to_loc.y,false);
		}
	}
	
	switch(ter_special) {
		case eTerSpec::CHANGE_WHEN_STEP_ON:
			alter_space(where_check.x,where_check.y,ter_flag1.u);
			if(ter_flag2.u < 200) {
				play_sound(-1 * ter_flag2.u);
			}
			give_help(47,65);
			if(blocksMove(univ.scenario.ter_types[ter].blockage))
				can_enter = false;
			break;
		case eTerSpec::DAMAGING:
			// if the party is flying, in a boat, or entering a boat, they cannot be harmed by terrain
			if(flying() || univ.party.in_boat >= 0)
				break;
			if(mode == eSpecCtx::TOWN_MOVE && town_boat_there(where_check) < 30)
				break;
			if(mode == eSpecCtx::OUT_MOVE && out_boat_there(where_check) < 30)
				break;
			if(ter_flag3.u > 0 && ter_flag3.u < 8)
				dam_type = (eDamageType) ter_flag3.u;
			else dam_type = eDamageType::WEAPON;
			r1 = get_ran(ter_flag2.u,1,ter_flag1.u);
			switch(dam_type){
				case eDamageType::FIRE:
					add_string_to_buf("  It's hot!");
					pic_type = 0;
					// TODO: Would be nice to have something similar to this but for other damaging terrains...
					if(univ.party.status[ePartyStatus::FIREWALK] > 0) {
						add_string_to_buf("  It doesn't affect you.");
						r1 = -1;
					}
					break;
				case eDamageType::COLD:
					add_string_to_buf("  You feel cold!");
					pic_type = 4;
					break;
				case eDamageType::MAGIC:
				case eDamageType::UNBLOCKABLE:
					add_string_to_buf("  Something shocks you!");
					pic_type = 1;
					break;
				case eDamageType::WEAPON:
					add_string_to_buf("  You feel pain!");
					pic_type = 3;
					break;
				case eDamageType::POISON:
					add_string_to_buf("  You suddenly feel very ill for a moment...");
					pic_type = 2;
					break;
				case eDamageType::UNDEAD:
				case eDamageType::DEMON:
					add_string_to_buf("  A dark wind blows through you!");
					pic_type = 1; // TODO: Verify that this is correct
					break;
				default:
					break;
			}
			if(r1 < 0) break; // "It doesn't affect you."
			if(mode != eSpecCtx::COMBAT_MOVE)
				hit_party(r1,dam_type);
			fast_bang = 1;
			if(mode == eSpecCtx::COMBAT_MOVE)
				damage_pc(which_pc,r1,dam_type,eRace::UNKNOWN,0);
			else
				boom_space(univ.party.p_loc,overall_mode,pic_type,r1,12);
			fast_bang = 0;
			break;
		case eTerSpec::DANGEROUS:
			// if the party is flying, in a boat, or entering a boat, they cannot be harmed by terrain
			if(flying() || univ.party.in_boat >= 0)
				break;
			if(mode == eSpecCtx::TOWN_MOVE && town_boat_there(where_check) < 30)
				break;
			if(mode == eSpecCtx::OUT_MOVE && out_boat_there(where_check) < 30)
				break;
			//one_sound(17);
			if(mode == eSpecCtx::COMBAT_MOVE) i = which_pc; else i = 0;
			for( ; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE) {
					if(get_ran(1,1,100) <= ter_flag2.u) {
						switch((eStatus)ter_flag3.u){
							case eStatus::POISONED_WEAPON: // TODO: Do something here
								if(get_ran(1,1,100) > 60) add_string_to_buf("It's eerie here...");
								break;
							case eStatus::BLESS_CURSE: // Should say "You feel awkward." / "You feel blessed."?
								curse_pc(i,ter_flag1.s);
								break;
							case eStatus::POISON:
								poison_pc(i,ter_flag1.u);
								break;
							case eStatus::HASTE_SLOW: // Should say "You feel sluggish." / "You feel speedy."?
								slow_pc(i,ter_flag1.s);
								break;
							case eStatus::INVULNERABLE: // Should say "You feel odd." / "You feel protected."?
								univ.party[i].apply_status(eStatus::INVULNERABLE,ter_flag1.u);
								break;
							case eStatus::MAGIC_RESISTANCE: // Should say "You feel odd." / "You feel protected."?
								univ.party[i].apply_status(eStatus::MAGIC_RESISTANCE,ter_flag1.u);
								break;
							case eStatus::WEBS: // Should say "You feel sticky." / "Your skin tingles."?
								web_pc(i,ter_flag1.u);
								break;
							case eStatus::DISEASE: // Should say "You feel healthy." / "You feel sick."?
								disease_pc(i,ter_flag1.u);
								break;
							case eStatus::INVISIBLE:
								if(ter_flag1.s < 0) add_string_to_buf("You feel obscure.");
								else add_string_to_buf("You feel exposed.");
								univ.party[i].apply_status(eStatus::INVISIBLE,ter_flag1.s);
								break;
							case eStatus::DUMB: // Should say "You feel clearheaded." / "You feel confused."?
								dumbfound_pc(i,ter_flag1.u);
								break;
							case eStatus::MARTYRS_SHIELD: // Should say "You feel dull." / "You start to glow slightly."?
								univ.party[i].apply_status(eStatus::MARTYRS_SHIELD,ter_flag1.u);
								break;
							case eStatus::ASLEEP: // Should say "You feel alert." / "You feel very tired."?
								sleep_pc(i,ter_flag1.u,eStatus::ASLEEP,ter_flag1.u / 2);
								break;
							case eStatus::PARALYZED: // Should say "You find it easier to move." / "You feel very stiff."?
								sleep_pc(i,ter_flag1.u,eStatus::PARALYZED,ter_flag1.u / 2);
								break;
							case eStatus::ACID: // Should say "Your skin tingles pleasantly." / "Your skin burns!"?
								acid_pc(i,ter_flag1.u);
								break;
							case eStatus::MAIN: case eStatus::CHARM: // These magic values are illegal in this context
								break;
						}
						if(mode == eSpecCtx::COMBAT_MOVE) break; // only damage once in combat!
					}
				}
			//print_nums(1,which_pc,current_pc);
			break;
		case eTerSpec::CALL_SPECIAL: {
			short spec_type = 0;
			if(ter_flag2.u == 3){
				if(mode == eSpecCtx::TOWN_MOVE || (mode == eSpecCtx::COMBAT_MOVE && which_combat_type == 1))
					spec_type = 2;
				else spec_type = 1;
			}else if(ter_flag2.u == 2 && (mode == eSpecCtx::OUT_MOVE || (mode == eSpecCtx::COMBAT_MOVE && which_combat_type == 0)))
				spec_type = 1;
			else if(ter_flag2.u == 1 && (mode == eSpecCtx::TOWN_MOVE || (mode == eSpecCtx::COMBAT_MOVE && which_combat_type == 1)))
				spec_type = 2;
			run_special(mode,spec_type,ter_flag1.u,where_check,&s1,&s2,&s3);
			if(s1 > 0)
				can_enter = false;
			break;
		}
			// Locked doors
		case eTerSpec::UNLOCKABLE:
			if(is_combat()) {  // No lockpicking in combat
				add_string_to_buf("  Can't enter: It's locked.");
				break;
			}
			
			// See what party wants to do.
			choice = cChoiceDlog("locked-door-action",{"leave","pick","bash"}).show();
			
			can_enter = false;
			if(choice == "leave")
				break;
			if((door_pc = select_pc(0)) < 6) {
				if(choice == "pick")
					pick_lock(where_check,door_pc);
				else bash_door(where_check,door_pc);
			}
			break;
	}
	
	// Action may change terrain, so update what's been seen
	if(is_town())
		update_explored(univ.town.p_loc);
	if(is_combat())
		update_explored(univ.party[current_pc].combat_pos);
	
	return can_enter;
}

// This procedure find the effects of fields that would affect a PC who moves into
// a space or waited in that same space
void check_fields(location where_check,eSpecCtx mode,short which_pc) {
	short r1,i;
	
	if(mode != eSpecCtx::COMBAT_MOVE && mode != eSpecCtx::TOWN_MOVE && mode != eSpecCtx::OUT_MOVE) {
		std::cout << "Note: Improper mode passed to check_special_terrain: " << int(mode) << std::endl;
		return;
	}
	if(is_out())
		return;
	if(is_town())
		fast_bang = 1;
	if(univ.town.is_fire_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Fire wall!               ");
		r1 = get_ran(1,1,6) + 1;
		// TODO: Is this commented code important?
//		if(mode < 2)
//			hit_party(r1,1);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::FIRE,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,0,r1,5);
	}
	if(univ.town.is_force_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Force wall!               ");
		r1 = get_ran(2,1,6);
//		if(mode < 2)
//			hit_party(r1,3);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::MAGIC,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,1,r1,12);
	}
	if(univ.town.is_ice_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Ice wall!               ");
		r1 = get_ran(2,1,6);
//		if(mode < 2)
//			hit_party(r1,5);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::COLD,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,4,r1,7);
	}
	if(univ.town.is_blade_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Blade wall!               ");
		r1 = get_ran(4,1,8);
//		if(mode < 2)
//			hit_party(r1,0);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::WEAPON,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,3,r1,2);
	}
	if(univ.town.is_quickfire(where_check.x,where_check.y)) {
		add_string_to_buf("  Quickfire!               ");
		r1 = get_ran(2,1,8);
//		if(mode < 2)
//			hit_party(r1,1);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::FIRE,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,0,r1,5);
	}
	if(univ.town.is_scloud(where_check.x,where_check.y)) {
		add_string_to_buf("  Stinking cloud!               ");
		if(mode != eSpecCtx::COMBAT_MOVE) {
			suppress_stat_screen = true;
			for(i = 0; i < 6; i++) {
				r1 = get_ran(1,2,3);
				curse_pc(i,r1);
			}
			suppress_stat_screen = false;
			put_pc_screen();
		}
		else curse_pc(current_pc,get_ran(1,2,3));
	}
	if(univ.town.is_sleep_cloud(where_check.x,where_check.y)) {
		add_string_to_buf("  Sleep cloud!               ");
		if(mode != eSpecCtx::COMBAT_MOVE) {
			suppress_stat_screen = true;
			for(i = 0; i < 6; i++) {
				sleep_pc(i,3,eStatus::ASLEEP,0);
			}
			suppress_stat_screen = false;
			put_pc_screen();
		}
		else sleep_pc(current_pc,3,eStatus::ASLEEP,0);
	}
	if(univ.town.is_fire_barr(where_check.x,where_check.y)) {
		add_string_to_buf("  Magic barrier!               ");
		r1 = get_ran(2,1,10);
		if(mode != eSpecCtx::COMBAT_MOVE)
			hit_party(r1,eDamageType::MAGIC);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::MAGIC,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,1,r1,12);
	}
	if(univ.town.is_force_cage(where_check.x,where_check.y)) {
		if(univ.party[which_pc].status[eStatus::FORCECAGE] == 0)
			add_string_to_buf("  Trapped in force cage!");
		univ.party[which_pc].status[eStatus::FORCECAGE] = 8;
	} else univ.party[which_pc].status[eStatus::FORCECAGE] = 0;
	fast_bang = 0;
}

void use_spec_item(short item) {
	short i,j,k;
	location null_loc;
	
	run_special(eSpecCtx::USE_SPEC_ITEM,0,univ.scenario.special_items[item].special,univ.party.p_loc,&i,&j,&k);
	
}


void use_item(short pc,short item) {
	bool take_charge = true,inept_ok = false;
	short level,i,j,item_use_code,str,type,r1;
	short sp[3] = {}; // Dummy values to pass to run_special; not actually used
	eStatus status;
	eSpell spell;
	location user_loc;
	cCreature *which_m;
	extern effect_pat_type single;
	eItemAbil abil = univ.party[pc].items[item].ability;
	level = univ.party[pc].items[item].item_level;
	
	// TODO: Replace abil_chart with a cItem member function
	item_use_code = abil_chart[abil];
	if(item_use_code >= 10) {
		item_use_code -= 10;
		inept_ok = true;
	}
	if(abil == eItemAbil::AFFECT_STATUS) {
		status = eStatus(univ.party[pc].items[item].abil_data[1]);
		if(status == eStatus::POISON || status == eStatus::DISEASE || status == eStatus::HASTE_SLOW || status == eStatus:: BLESS_CURSE)
			item_use_code = 4;
	} else if(abil == eItemAbil::CAST_SPELL) {
		spell = eSpell(univ.party[pc].items[item].abil_data[1]);
		int when = (*spell).when_cast;
		// Rather than trying to translate the spell's "when cast" value to the less expressive item_use_code,
		// simply check if it's useable in the current context.
		if(is_out() && when & WHEN_OUTDOORS)
			item_use_code = 5;
		else if(is_town() && when & WHEN_TOWN)
			item_use_code = 2;
		else if(is_combat() && when & WHEN_COMBAT)
			item_use_code = 1;
		else {
			// It's not useable in the current context, so translate to the item_use_code that gives the best error message
			if(is_town() || is_out())
				item_use_code = 1;
			else item_use_code = 2;
		}
	} else if(abil == eItemAbil::AFFECT_PARTY_STATUS && univ.party[pc].items[item].abil_data[1] == int(ePartyStatus::FLIGHT))
		item_use_code = 5;
	
	if(is_out())
		user_loc = univ.party.p_loc;
	if(is_town())
		user_loc = univ.town.p_loc;
	if(is_combat())
		user_loc = univ.party[current_pc].combat_pos;
	
	if(item_use_code == 0) {
		add_string_to_buf("Use: Can't use this item.       ");
		take_charge = false;
	}
	if(univ.party[pc].traits[eTrait::MAGICALLY_INEPT] && !inept_ok){
		add_string_to_buf("Use: Can't - magically inept.       ");
		take_charge = false;
	}
	
	// 0 - can't use 1 - combat only 2 - town only 3 - town & combat only  4 - everywhere  5 - outdoor
	if(take_charge) {
		if(overall_mode == MODE_OUTDOORS && item_use_code < 4) {
			add_string_to_buf("Use: Not while outdoors.         ");
			take_charge = false;
		}
		// TODO: Almost all of these look wrong!
		if((overall_mode == MODE_TOWN) && (item_use_code == 1)) {
			add_string_to_buf("Use: Not while in town.         ");
			take_charge = false;
		}
		if((overall_mode == MODE_COMBAT) && (item_use_code == 2)) {
			add_string_to_buf("Use: Not in combat.         ");
			take_charge = false;
		}
		if((overall_mode != MODE_OUTDOORS) && (item_use_code == 5)){
			add_string_to_buf("Use: Only outdoors.           ");
			take_charge = false;
		}
	}
	if(take_charge) {
		std::string name;
		if(!univ.party[pc].items[item].ident)
			name = univ.party[pc].items[item].name.c_str();
		else name = univ.party[pc].items[item].full_name.c_str();
		add_string_to_buf("Use: " + name);
		
		if(univ.party[pc].items[item].variety == eItemType::POTION)
			play_sound(56);
		
		str = univ.party[pc].items[item].abil_data[0];
		store_item_spell_level = str;
		type = univ.party[pc].items[item].magic_use_type;
		
		switch(abil) {
			case eItemAbil::POISON_WEAPON: // poison weapon
				take_charge = poison_weapon(pc,str,0);
				break;
			case eItemAbil::AFFECT_STATUS:
				switch(status) {
					case eStatus::BLESS_CURSE:
						play_sound(4);
						if(type % 2 == 1) {
							ASB("  You feel awkward.");
							str = str * -1;
						}else ASB("  You feel blessed.");
						if(type > 1)
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::HASTE_SLOW:
						// TODO: Is this the right sound?
						play_sound(75);
						if(type % 2 == 1) {
							ASB("  You feel sluggish.");
							str = str * -1;
						}else ASB("  You feel speedy.");
						if(type > 1)
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::INVULNERABLE:
						// TODO: Is this the right sound?
						play_sound(68);
						if(type % 2 == 1) {
							ASB("  You feel odd.");
							str = str * -1;
						}else ASB("  You feel protected.");
						if(type > 1)
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::MAGIC_RESISTANCE:
						// TODO: Is this the right sound?
						play_sound(51);
						if(type % 2 == 1) {
							ASB("  You feel odd.");
							str = str * -1;
						}else ASB("  You feel protected.");
						if(type > 1)
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::WEBS:
						if(type % 2 == 1)
							ASB("  You feel sticky.");
						else {
							ASB("  Your skin tingles.");
							str = str * -1;
						}
						if(type > 1)
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::INVISIBLE:
						// TODO: Is this the right sound?
						play_sound(43);
						if(type % 2 == 1) {
							ASB("  You feel exposed.");
							str = str * -1;
						}else ASB("  You feel obscure.");
						if(type > 1)
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::MARTYRS_SHIELD:
						// TODO: Is this the right sound?
						play_sound(43);
						if(type % 2 == 1) {
							ASB("  You feel dull.");
							str = str * -1;
						}else ASB("  You start to glow slightly.");
						if(type > 1)
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::POISON:
						switch(type) {
							case 0:
								ASB("  You feel better.");
								cure_pc(pc,str);
								break;
							case 1:
								ASB("  You feel ill.");
								poison_pc(pc,str);
								break;
							case 2:
								ASB("  You all feel better.");
								cure_party(str);
								break;
							case 3:
								ASB("  You all feel ill.");
								poison_party(str);
								break;
						}
						break;
					case eStatus::DISEASE:
						switch(type) {
							case 0:
								ASB("  You feel healthy.");
								univ.party[pc].apply_status(eStatus::DISEASE,-1 * str);
								break;
							case 1:
								ASB("  You feel sick.");
								disease_pc(pc,str);
								break;
							case 2:
								ASB("  You all feel healthy.");
								univ.party.apply_status(eStatus::DISEASE,-1 * str);
								break;
							case 3:
								ASB("  You all feel sick.");
								for(i = 0; i < 6; i++)
									disease_pc(i,str);
								break;
						}
						break;
					case eStatus::DUMB:
						switch(type) {
							case 0:
								ASB("  You feel clear headed.");
								univ.party[pc].apply_status(eStatus::DUMB,-1 * str);
								break;
							case 1:
								ASB("  You feel confused.");
								dumbfound_pc(pc,str);
								break;
							case 2:
								ASB("  You all feel clear headed.");
								univ.party.apply_status(eStatus::DUMB,-1 * str);
								break;
							case 3:
								ASB("  You all feel confused.");
								for(i = 0; i < 6; i++)
									dumbfound_pc(i,str);
								break;
						}
						break;
					case eStatus::ASLEEP:
						switch(type) {
							case 0:
								ASB("  You feel alert.");
								univ.party[pc].apply_status(eStatus::ASLEEP,-1 * str);
								break;
							case 1:
								ASB("  You feel very tired.");
								sleep_pc(pc,str + 1,eStatus::ASLEEP,200);
								break;
							case 2:
								ASB("  You all feel alert.");
								univ.party.apply_status(eStatus::ASLEEP,-1 * str);
								break;
							case 3:
								ASB("  You all feel very tired.");
								for(i = 0; i < 6; i++)
									sleep_pc(i,str + 1,eStatus::ASLEEP,200);
								break;
						}
						break;
					case eStatus::PARALYZED:
						switch(type) {
							case 0:
								ASB("  You find it easier to move.");
								univ.party[pc].apply_status(eStatus::PARALYZED,-1 * str * 100);
								break;
							case 1:
								ASB("  You feel very stiff.");
								sleep_pc(pc,str * 20 + 10,eStatus::PARALYZED,200);
								break;
							case 2:
								ASB("  You all find it easier to move.");
								univ.party.apply_status(eStatus::PARALYZED,-1 * str * 100);
								break;
							case 3:
								ASB("  You all feel very stiff.");
								for(i = 0; i < 6; i++)
									sleep_pc(i,str * 20 + 10,eStatus::PARALYZED,200);
								break;
						}
						break;
					case eStatus::ACID:
						switch(type) {
							case 0:
								ASB("  Your skin tingles pleasantly.");
								univ.party[pc].apply_status(eStatus::ACID,-1 * str);
								break;
							case 1:
								ASB("  Your skin burns!");
								acid_pc(pc,str);
								break;
							case 2:
								ASB("  You all tingle pleasantly.");
								univ.party.apply_status(eStatus::ACID,-1 * str);
								break;
							case 3:
								ASB("  Everyone's skin burns!");
								for(i = 0; i < 6; i++)
									acid_pc(i,str);
								break;
						}
						break;
				}
			case eItemAbil::BLISS_DOOM:
				switch(type) {
					case 0:
						ASB("  You feel wonderful!");
						heal_pc(pc,str * 20);
						univ.party[pc].apply_status(eStatus::BLESS_CURSE,str);
						break;
					case 1:
						ASB("  You feel terrible.");
						drain_pc(pc,str * 5);
						damage_pc(pc,20 * str,eDamageType::UNBLOCKABLE,eRace::HUMAN,0);
						disease_pc(pc,2 * str);
						dumbfound_pc(pc,2 * str);
						break;
					case 2:
						ASB("  Everyone feels wonderful!");
						heal_party(str*20);
						univ.party.apply_status(eStatus::BLESS_CURSE,str);
						break;
					case 3:
						ASB("  You all feel terrible.");
						for(i = 0; i < 6; i++) {
							drain_pc(i,str * 5);
							damage_pc(i,20 * str,eDamageType::UNBLOCKABLE,eRace::HUMAN,0);
							disease_pc(i,2 * str);
							dumbfound_pc(i,2 * str);
						}
						break;
				}
				break;
			case eItemAbil::AFFECT_EXPERIENCE:
				switch(type) {
					case 0:
						ASB("  You feel much smarter.");
						award_xp(pc,str * 5);
						break;
					case 1:
						ASB("  You feel forgetful.");
						drain_pc(pc,str * 5);
						break;
					case 2:
						ASB("  You all feel much smarter.");
						award_party_xp(str * 5);
						break;
					case 3:
						ASB("  You all feel forgetful.");
						for(i = 0; i < 6; i++)
							drain_pc(i,str * 5);
						break;
				}
				break;
			case eItemAbil::AFFECT_SKILL_POINTS:
				// TODO: Is this the right sound?
				play_sound(68);
				switch(type) {
					case 0:
						ASB("  You feel much smarter.");
						univ.party[pc].skill_pts += str;
						break;
					case 1:
						ASB("  You feel forgetful.");
						univ.party[pc].skill_pts = max(0,univ.party[pc].skill_pts - str);
						break;
					case 2:
						ASB("  You all feel much smarter.");
						for(i = 0; i < 6; i++)
							univ.party[i].skill_pts += str;
						break;
					case 3:
						ASB("  You all feel forgetful.");
						for(i = 0; i < 6; i++)
							univ.party[i].skill_pts = max(0,univ.party[i].skill_pts - str);
						break;
				}
				break;
			case eItemAbil::AFFECT_HEALTH:
				switch(type) {
					case 0:
						ASB("  You feel better.");
						heal_pc(pc,str * 20);
						break;
					case 1:
						ASB("  You feel sick.");
						damage_pc(pc,20 * str,eDamageType::UNBLOCKABLE,eRace::HUMAN,0);
						break;
					case 2:
						ASB("  You all feel better.");
						heal_party(str * 20);
						break;
					case 3:
						ASB("  You all feel sick.");
						hit_party(20 * str,eDamageType::UNBLOCKABLE);
						break;
				}
				break;
			case eItemAbil::AFFECT_SPELL_POINTS:
				switch(type) {
					case 0:
						ASB("  You feel energized.");
						restore_sp_pc(pc,str * 5);
						break;
					case 1:
						ASB("  You feel drained.");
						univ.party[pc].cur_sp = max(0,univ.party[pc].cur_sp - str * 5);
						break;
					case 2:
						ASB("  You all feel energized.");
						restore_sp_party(str * 5);
						break;
					case 3:
						ASB("  You all feel drained.");
						for(i = 0; i < 6; i++)
							univ.party[i].cur_sp = max(0,univ.party[i].cur_sp - str * 5);
						break;
				}
				break;
			case eItemAbil::LIGHT:
				if(type % 2 == 0) {
					ASB("  You have more light.");
					increase_light(50 * str);
				} else {
					ASB("  It gets darker.");
					increase_light(-50 & str);
				}
				break;
			case eItemAbil::AFFECT_PARTY_STATUS:
				switch(ePartyStatus(univ.party[pc].items[item].abil_data[1])) {
					case ePartyStatus::STEALTH: ASB("  Your footsteps become quieter."); str *= 5; break;
					case ePartyStatus::FIREWALK: ASB("  You feel chilly."); str *= 2; break;
					case ePartyStatus::DETECT_LIFE: ASB("  You detect  life."); break;
					case ePartyStatus::FLIGHT:
						if(univ.party.status[ePartyStatus::FLIGHT] > 0) {
							add_string_to_buf("  Not while already flying.          ");
							take_charge = false;
						} else if(univ.party.in_boat >= 0) {
							add_string_to_buf("  Leave boat first.             ");
							take_charge = false;
						} else if(univ.party.in_horse >= 0) {
							add_string_to_buf("  Leave horse first.             ");
							take_charge = false;
						} else ASB("  You rise into the air!");
						break;
				}
				if(take_charge) univ.party.status[ePartyStatus(univ.party[pc].items[item].abil_data[1])] += str;
				break;
			case eItemAbil::HEALTH_POISON:
				switch(type) {
					case 0:
						ASB("  You feel wonderful.");
						heal_pc(pc,str*25);
						cure_pc(pc,str);
						break;
					case 1:
						ASB("  You feel terrible.");
						damage_pc(pc, str*25, eDamageType::UNBLOCKABLE, eRace::UNKNOWN, 0);
						poison_pc(pc,str);
						break;
					case 2:
						ASB("  You all feel wonderful.");
						heal_party(str*25);
						cure_party(str);
						break;
					case 3:
						ASB("  You all feel terrible.");
						hit_party(str*25, eDamageType::UNBLOCKABLE);
						poison_party(str);
						break;
				}
				break;
			case eItemAbil::CALL_SPECIAL:
				// TODO: Should this have its own separate eSpecCtx?
				run_special(eSpecCtx::USE_SPEC_ITEM,0,str,user_loc,&sp[0],&sp[1],&sp[2]);
				break;
				
			case eItemAbil::CAST_SPELL:
				if(univ.town.is_antimagic(user_loc.x, user_loc.y)) {
					add_string_to_buf("  Not in antimagic field.");
					take_charge = false;
					break;
				}
				switch(spell) {
					case eSpell::FLAME: add_string_to_buf("  It fires a bolt of flame."); break;
					case eSpell::FIREBALL: add_string_to_buf("  It shoots a fireball."); break;
					case eSpell::FIRESTORM: add_string_to_buf("  It shoots a huge fireball. "); break;
					case eSpell::KILL: add_string_to_buf("  It shoots a black ray."); break;
					case eSpell::ICE_BOLT: add_string_to_buf("  It fires a ball of ice."); break;
					case eSpell::SLOW: add_string_to_buf("  It fires a purple ray."); break;
					case eSpell::DISPEL_UNDEAD: add_string_to_buf("  It shoots a white ray."); break;
					case eSpell::RAVAGE_SPIRIT: add_string_to_buf("  It shoots a golden ray."); break;
					case eSpell::ACID_SPRAY: add_string_to_buf("  Acid sprays from the tip!"); break;
					case eSpell::FOUL_VAPOR: add_string_to_buf("  It creates a cloud of gas."); break;
					case eSpell::CLOUD_SLEEP: add_string_to_buf("  It creates a shimmering cloud."); break;
					case eSpell::POISON: add_string_to_buf("  A green ray emerges."); break;
					case eSpell::SHOCKSTORM: add_string_to_buf("  Sparks fly."); break;
					case eSpell::PARALYZE_BEAM: add_string_to_buf("  It shoots a silvery beam."); break;
					case eSpell::GOO_BOMB: add_string_to_buf("  It explodes!"); break;
					case eSpell::STRENGTHEN_TARGET: add_string_to_buf("  It shoots a fiery red ray."); break;
					case eSpell::CHARM_MASS: ASB("It throbs, and emits odd rays."); break;
					case eSpell::DISPEL_BARRIER: add_string_to_buf("  It fires a blinding ray."); break;
					case eSpell::WALL_ICE_BALL: add_string_to_buf("  It shoots a blue sphere."); break;
					case eSpell::CHARM_FOE: add_string_to_buf("  It fires a lovely, sparkling beam."); break;
					case eSpell::ANTIMAGIC: add_string_to_buf("  Your hair stands on end."); break;
					default: add_string_to_buf("  It casts a spell: " + (*spell).name()); break;
				}
				if(overall_mode == MODE_COMBAT) {
					bool priest = (*spell).is_priest();
					switch((*spell).refer) {
						case REFER_YES:
							if(priest) do_priest_spell(current_pc, spell, true);
							else do_mage_spell(current_pc, spell, true);
							break;
						case REFER_TARGET:
							start_spell_targeting(spell, true);
							break;
						case REFER_FANCY:
							start_fancy_spell_targeting(spell, true);
							break;
						case REFER_IMMED:
							if(priest) combat_immed_priest_cast(current_pc, spell, true);
							else combat_immed_mage_cast(current_pc, spell, true);
							break;
					}
				} else if((*spell).is_priest())
					do_priest_spell(current_pc, spell, true);
				else do_mage_spell(current_pc, spell, true);
				break;
			case eItemAbil::SUMMONING:
				if(!summon_monster(univ.party[pc].items[item].abil_data[1],user_loc,str,2))
					add_string_to_buf("  Summon failed.");
				break;
			case eItemAbil::MASS_SUMMONING:
				r1 = get_ran(str,1,4);
				j = get_ran(1,3,5);
				for(i = 0; i < j; i++)
					if(!summon_monster(univ.party[pc].items[item].abil_data[1],user_loc,r1,2))
						add_string_to_buf("  Summon failed.");
				break;
			case eItemAbil::QUICKFIRE:
				add_string_to_buf("Fire pours out!");
				univ.town.set_quickfire(user_loc.x,user_loc.y,true);
				break;
		}
	}
	
	put_pc_screen();
	if((take_charge) && (univ.party[pc].items[item].charges > 0))
		univ.party[pc].remove_charge(item);
	if(stat_window == pc)
		put_item_screen(stat_window,1);
	if(!take_charge) {
		draw_terrain(0);
		put_item_screen(stat_window,0);
	}
}

// Returns true if an action is actually carried out. This can only be reached in town.
bool use_space(location where) {
	ter_num_t ter;
	short i;
	location from_loc,to_loc;
	
	ter = univ.town->terrain(where.x,where.y);
	from_loc = univ.town.p_loc;
	
	add_string_to_buf("Use...");
	
	if(univ.town.is_web(where.x,where.y)) {
		add_string_to_buf("  You clear the webs.");
		univ.town.set_web(where.x,where.y,false);
		return true;
	}
	if(univ.town.is_crate(where.x,where.y)) {
		to_loc = push_loc(from_loc,where);
		if(from_loc == to_loc) {
			add_string_to_buf("  Can't push crate.");
			return false;
		}
		add_string_to_buf("  You push the crate.");
		univ.town.set_crate(where.x,where.y,false);
		univ.town.set_crate(to_loc.x,to_loc.y,true);
		for(i = 0; i < NUM_TOWN_ITEMS; i++)
			if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where
			   && (univ.town.items[i].contained))
			 	univ.town.items[i].item_loc = to_loc;
	}
	if(univ.town.is_barrel(where.x,where.y)) {
		to_loc = push_loc(from_loc,where);
		if(from_loc == to_loc) {
			add_string_to_buf("  Can't push barrel.");
			return false;
		}
		add_string_to_buf("  You push the barrel.");
		univ.town.set_barrel(where.x, where.y,false);
		univ.town.set_barrel(to_loc.x,to_loc.y,true);
		for(i = 0; i < NUM_TOWN_ITEMS; i++)
			if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where
			   && (univ.town.items[i].contained))
			 	univ.town.items[i].item_loc = to_loc;
	}
	if(univ.town.is_block(where.x,where.y)) {
		to_loc = push_loc(from_loc,where);
		if(from_loc == to_loc) {
			add_string_to_buf("  Can't push block.");
			return false;
		}
		add_string_to_buf("  You push the block.");
		univ.town.set_block(where.x,where.y,false);
		univ.town.set_block(to_loc.x,to_loc.y,true);
	}
	
	if(univ.scenario.ter_types[ter].special == eTerSpec::CHANGE_WHEN_USED) {
		if(where == from_loc) {
			add_string_to_buf("  Not while on space.");
			return false;
		}
		add_string_to_buf("  OK.");
		alter_space(where.x,where.y,univ.scenario.ter_types[ter].flag1.u);
		play_sound(univ.scenario.ter_types[ter].flag2.u);
		return true;
	} else if(univ.scenario.ter_types[ter].special == eTerSpec::CALL_SPECIAL_WHEN_USED) {
		short spec_type = 0;
		if(univ.scenario.ter_types[ter].flag2.u == 3){
			if((is_town() || (is_combat() && which_combat_type == 1))) spec_type = 2; else spec_type = 1;
		}else if(univ.scenario.ter_types[ter].flag2.u == 1 && (is_town() || (is_combat() && which_combat_type == 1)))
			spec_type = 2;
		else if(univ.scenario.ter_types[ter].flag2.u == 2 && (is_out() || (is_combat() && which_combat_type == 1)))
			spec_type = 1;
		run_special(eSpecCtx::USE_SPACE,spec_type,univ.scenario.ter_types[ter].flag1.u,where,&i,&i,&i);
		return true;
	}
	add_string_to_buf("  Nothing to use.");
	
	return false;
}

// Note ... if this is a container, the code must first process any specials. If
// specials return false, can't get items inside. If true, can get items inside.
// Can't get items out in combat.
bool adj_town_look(location where) {
	ter_num_t terrain;
	bool can_open = true,item_there = false,got_special = false;
	short i = 0,s1 = 0, s2 = 0, s3 = 0;
	
	for(i = 0; i < NUM_TOWN_ITEMS; i++)
		if(univ.town.items[i].variety != eItemType::NO_ITEM && (univ.town.items[i].contained) &&
		   (where == univ.town.items[i].item_loc))
			item_there = true;
	
	terrain = univ.town->terrain(where.x,where.y);
	if(univ.town.is_special(where.x,where.y)) {// && (get_blockage(terrain) > 0)) {
		if(!adjacent(univ.town.p_loc,where))
			add_string_to_buf("  Not close enough to search.");
		else {
			for(i = 0; i < 50; i++)
				if(where == univ.town->special_locs[i]) {
					if(get_blockage(univ.town->terrain(where.x,where.y)) > 0) {
						// tell party you find something, if looking at a space they can't step in
						add_string_to_buf("  Search: You find something!          ");
					}
					//call special can_open = town_specials(i,univ.town.town_num);
					
					run_special(eSpecCtx::TOWN_LOOK,2,univ.town->spec_id[i],where,&s1,&s2,&s3);
					if(s1 > 0)
						can_open = false;
					got_special = true;
				}
			put_item_screen(stat_window,0);
		}
	}
	if(is_container(where) && item_there && can_open) {
		get_item(where,6,true);
	}else if(univ.scenario.ter_types[terrain].special == eTerSpec::CHANGE_WHEN_USED ||
			 univ.scenario.ter_types[terrain].special == eTerSpec::CALL_SPECIAL_WHEN_USED) {
		add_string_to_buf("  (Use this space to do something");
		add_string_to_buf("  with it.)");
	}else{
		if(!got_special)
			add_string_to_buf("  Search: You don't find anything.          ");
		return false;
	}
	return false;
}

// PSOE - place_special_outdoor_encounter
// if always, stuff_done_val is NULL
void PSOE(short which_special,unsigned char *stuff_done_val,short where_put) {
	short i,j,graphic_num = 0;
	
	if(stuff_done_val != NULL) {
		if(*stuff_done_val > 0)
			return;
		else *stuff_done_val = 20;
	}
	for(i = 0; i < 18; i++)
		if(univ.out->special_id[i] == where_put) {
			for(j = 0; j < 7; j++)
				if(univ.out->special_enc[which_special].monst[j] > 0) {
					graphic_num = get_monst_picnum(univ.out->special_enc[which_special].monst[j]);
					j = 7;
				}
			//display_strings( str1a, str1b, str2a, str2b,
			////	"Encounter!",57, graphic_num, 0);
			draw_terrain(0);
			pause(15);
			//if(univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].special_enc[which_special].spec_code == 0)
			//	univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].special_enc[which_special].spec_code = 1;
			//place_outd_wand_monst(univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].special_locs[i],
			//	univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].special_enc[which_special]);
			
			i = 18;
		}
	draw_terrain(0);
	play_sound(61);
	//play_sound(0);
}

void out_move_party(short x,short y) {
	location l;
	
	l.x = x;l.y = y;
	l = local_to_global(l);
	univ.party.p_loc = l;
	center = l;
	update_explored(l);
}

// mode - 0=full teleport flash 1=no teleport flash 2=only fade flash 3=only arrival flash
void teleport_party(short x,short y,short mode) {
	// TODO: Teleport sound? (Sound 10)
	short i;
	location l;
	bool fadeIn = false, fadeOut = false;
	if(mode == 0 || mode == 2) fadeOut = true;
	if(mode == 0 || mode == 3) fadeIn = true;
	
	if(is_combat())
		mode = 1;
	
	l = univ.town.p_loc;
	update_explored(l);
	
	if(fadeOut) {
		start_missile_anim();
		for(i = 0; i < 9; i++)
			add_explosion(l,-1,1,1,0,0);
		do_explosion_anim(5,1);
		can_draw_pcs = false;
		do_explosion_anim(5,2);
		end_missile_anim();
	}
	center.x = x; center.y = y;
	if(is_combat()) {
		univ.party[current_pc].combat_pos.x = x;
		univ.party[current_pc].combat_pos.y = y;
	}
	l.x = x; l.y = y;
	univ.town.p_loc.x = x;
	univ.town.p_loc.y = y;
	update_explored(l);
	draw_terrain(0);
	
	if(fadeIn) {
		start_missile_anim();
		for(i = 0; i < 14; i++)
			add_explosion(center,-1,1,1,0,0);
		do_explosion_anim(5,1);
	}
	can_draw_pcs = true;
	if(fadeIn) {
		do_explosion_anim(5,2);
		end_missile_anim();
	}
	draw_map(true);
}


void fade_party() {
	short i;
	location l;
	
	l = univ.town.p_loc;
	start_missile_anim();
	for(i = 0; i < 14; i++)
		add_explosion(l,-1,1,1,0,0);
	do_explosion_anim(5,1);
	univ.town.p_loc.x = 100;
	univ.town.p_loc.y = 100;
	do_explosion_anim(5,2);
	end_missile_anim();
}

void change_level(short town_num,short x,short y) {
	location l(x,y);
	
	if((town_num < 0) || (town_num >= univ.scenario.towns.size())) {
		giveError("The scenario special encounter tried to put you into a town that doesn't exist.");
		return;
	}
	
	force_town_enter(town_num,l);
	end_town_mode(1,l);
	start_town_mode(town_num,9);
}


// Damaging and killing monsters needs to be here because several have specials attached to them.
bool damage_monst(short which_m, short who_hit, short how_much, short how_much_spec, eDamageType dam_type, short sound_type, bool do_print) {
	cCreature *victim;
	short r1,which_spot;
	location where_put;
	
	//print_num(which_m,(short)univ.town.monst[which_m].m_loc.x,(short)univ.town.monst[which_m].m_loc.y);
	
	if(univ.town.monst[which_m].active == 0) return false;
	
	if(sound_type == 0) {
		if(dam_type == eDamageType::FIRE || dam_type == eDamageType::UNBLOCKABLE)
			sound_type = 5;
		if(dam_type == eDamageType::COLD)
			sound_type = 7;
		if(dam_type == eDamageType::MAGIC)
			sound_type = 12;
		if(dam_type == eDamageType::POISON)
			sound_type = 11;
	}
	
	victim = &univ.town.monst[which_m];
	
	if(dam_type == eDamageType::MAGIC) {
		how_much *= victim->magic_res;
		how_much /= 100;
	}
	if(dam_type == eDamageType::FIRE) {
		how_much *= victim->fire_res;
		how_much /= 100;
	}
	if(dam_type == eDamageType::COLD) {
		how_much *= victim->cold_res;
		how_much /= 100;
	}
	if(dam_type == eDamageType::POISON) {
		how_much *= victim->poison_res;
		how_much /= 100;
	}
	
	// Absorb damage?
	if((dam_type == eDamageType::FIRE || dam_type == eDamageType::MAGIC || dam_type == eDamageType::COLD)
		&& victim->abil[eMonstAbil::ABSORB_SPELLS].active && get_ran(1,1,1000) <= victim->abil[eMonstAbil::ABSORB_SPELLS].special.extra1) {
		if(32767 - victim->health > how_much)
			victim->health = 32767;
		else victim->health += how_much;
		ASB("  Magic absorbed.");
		return false;
	}
	
	// Saving throw
	if((dam_type == eDamageType::FIRE || dam_type == eDamageType::COLD) && get_ran(1,0,20) <= victim->level)
		how_much /= 2;
	if(dam_type == eDamageType::MAGIC && (get_ran(1,0,24) <= victim->level))
		how_much /= 2;
	
	// Invulnerable?
	if(victim->invuln)
		how_much = how_much / 10;
	
	
	r1 = get_ran(1,0,(victim->armor * 5) / 4);
	r1 += victim->level / 4;
	if(dam_type == eDamageType::WEAPON)
		how_much -= r1;
	
	if(boom_anim_active) {
		if(how_much < 0)
			how_much = 0;
		// TODO: Also, if it's magic, use boom type 3 (must implement in the animation engine first)
		// It would also be nice to have a special boom type for cold.
		short boom_type = 2;
		if(dam_type == eDamageType::FIRE)
			boom_type = 0;
		univ.town.monst[which_m].marked_damage += how_much;
		add_explosion(victim->cur_loc,how_much,0,boom_type,14 * (victim->x_width - 1),18 * (victim->y_width - 1));
		// Note: Windows version printed an "undamaged" message here if applicable, but I don't think that's right.
		if(how_much == 0)
			return false;
		else return true;
	}
	
	if(how_much <= 0) {
		if(is_combat())
			monst_spell_note(victim->number,7);
		if(how_much <= 0 && (dam_type == eDamageType::WEAPON || dam_type == eDamageType::UNDEAD || dam_type == eDamageType::DEMON)) {
			draw_terrain(2);
			play_sound(2);
		}
//		sprintf ((char *) create_line, "  No damage.              ");
//		add_string_to_buf((char *) create_line);
		return false;
	}
	
	if(do_print)
		monst_damaged_mes(which_m,how_much,how_much_spec);
	victim->health = victim->health - how_much - how_much_spec;
	
	if(in_scen_debug)
		victim->health = -1;
	
	// splitting monsters
	if(victim->abil[eMonstAbil::SPLITS].active && victim->health > 0 && get_ran(1,1,1000) < victim->abil[eMonstAbil::SPLITS].special.extra1){
		where_put = find_clear_spot(victim->cur_loc,1);
		if(where_put.x > 0)
			if((which_spot = place_monster(victim->number,where_put)) < 90) {
				// TODO: Why so many assignments? Windows only assigns health and monst_start (start_loc I assume)
				univ.town.monst[which_spot].health = victim->health;
				univ.town.monst[which_spot].number = victim->number;
				univ.town.monst[which_spot].start_attitude = victim->start_attitude;
				univ.town.monst[which_spot].start_loc = victim->start_loc;
				univ.town.monst[which_spot].mobility = victim->mobility;
				univ.town.monst[which_spot].time_flag = victim->time_flag;
				univ.town.monst[which_spot].spec1 = victim->spec1;
				univ.town.monst[which_spot].spec2 = victim->spec2;
				univ.town.monst[which_spot].spec_enc_code = victim->spec_enc_code;
				univ.town.monst[which_spot].time_code = victim->time_code;
				univ.town.monst[which_spot].monster_time = victim->monster_time;
				univ.town.monst[which_spot].personality = victim->personality;
				univ.town.monst[which_spot].special_on_kill = victim->special_on_kill;
				univ.town.monst[which_spot].facial_pic = victim->facial_pic;
				monst_spell_note(victim->number,27);
			}
	}
	if(who_hit < 7)
		univ.party.total_dam_done += how_much + how_much_spec;
	
	// Monster damages. Make it hostile.
	victim->active = 2;
	
	// TODO: This looks like the reason Windows split the boom_space function in two.
	// It doesn't exactly make sense though, since in its version, boom_space is only called for how_much_spec.
	if(dam_type != eDamageType::MARKED) { // note special damage only gamed in hand-to-hand, not during animation
		if(party_can_see_monst(which_m)) {
			boom_space(victim->cur_loc,100,boom_gr[dam_type],how_much,sound_type);
			if(how_much_spec > 0)
				boom_space(victim->cur_loc,100,51,how_much_spec,5);
		}
		else {
			boom_space(victim->cur_loc,overall_mode, boom_gr[dam_type],how_much,sound_type);
			if(how_much_spec > 0)
				boom_space(victim->cur_loc,overall_mode,51,how_much_spec,5);
		}
	}
	
	if(victim->health < 0) {
		monst_killed_mes(which_m);
		kill_monst(victim,who_hit);
	}
	else {
		if(how_much > 0)
			victim->morale = victim->morale - 1;
		if(how_much > 5)
			victim->morale = victim->morale - 1;
		if(how_much > 10)
			victim->morale = victim->morale - 1;
		if(how_much > 20)
			victim->morale = victim->morale - 2;
	}
	
	if((victim->attitude % 2 != 1) && (who_hit < 7) &&
		((processing_fields && !monsters_going) || (processing_fields && !PSD[SDF_HOSTILES_PRESENT]))) {
		add_string_to_buf("Damaged an innocent.           ");
		victim->attitude = 1;
		make_town_hostile();
	}
	
	return true;
}

void petrify_monst(cCreature* m_target, short strength) {
	monst_spell_note(m_target->number,9);
	short r1 = get_ran(1,0,20);
	r1 += m_target->level / 4;
	r1 += m_target->status[eStatus::BLESS_CURSE];
	r1 -= strength;
	
	// TODO: This should probably do something similar to charm_monst with the magic resistance
	if(r1 > 14 || m_target->magic_res == 0)
		monst_spell_note(m_target->number,10);
	else {
		monst_spell_note(m_target->number,8);
		kill_monst(m_target,7);
	}
}

void kill_monst(cCreature *which_m,short who_killed,eMainStatus type) {
	short xp,i,j,s1,s2,s3;
	location l;
	
	if(isHumanoid(which_m->m_type)) {
		// TODO: Uh, don't hardcode these!
		if(( which_m->number == 38) ||
			( which_m->number == 39))
			i = 4;
		else if( which_m->number == 45)
			i = 0;
		else i = get_ran(1,0,1);
		play_sound(29 + i);
	} else switch(which_m->m_type) {
		case eRace::GIANT: play_sound(29); break;
			// TODO: Should birds be considered beasts? If there are any birds in the bladbase, probably; otherwise, better to have new sound
		case eRace::REPTILE: case eRace::BEAST: case eRace::DEMON: case eRace::UNDEAD: case eRace::STONE:
			i = get_ran(1,0,1); play_sound(31 + i); break;
		default: play_sound(33); break;
	}
	
	// Special killing effects
	if(sd_legit(which_m->spec1,which_m->spec2))
		PSD[which_m->spec1][which_m->spec2] = 1;
	
	if(which_m->special_on_kill >= 0)
		run_special(eSpecCtx::KILL_MONST,2,which_m->special_on_kill,which_m->cur_loc,&s1,&s2,&s3);
	if(which_m->abil[eMonstAbil::DEATH_TRIGGER].active)
		run_special(eSpecCtx::KILL_MONST,0,which_m->abil[eMonstAbil::DEATH_TRIGGER].special.extra1,which_m->cur_loc,&s1,&s2,&s3);
	
	if((!in_scen_debug) && ((which_m->summoned >= 100) || (which_m->summoned == 0))) { // no xp for party-summoned monsters
		xp = which_m->level * 2;
		if(who_killed < 6)
			award_xp(who_killed,xp);
		else if(who_killed == 6)
			award_party_xp(xp / 6 + 1);
		if(who_killed < 7) {
			univ.party.total_m_killed++;
			i = max((xp / 6),1);
			award_party_xp(i);
		}
		l = which_m->cur_loc;
		place_glands(l,which_m->number);
		
	}
	if((!in_scen_debug) && (which_m->summoned == 0))
		place_treasure(which_m->cur_loc, which_m->level / 2, which_m->treasure, 0);
	
	i = which_m->cur_loc.x;
	j = which_m->cur_loc.y;
	if(type == eMainStatus::DUST)
		univ.town.set_ash(i,j,true);
	else if(type == eMainStatus::ABSENT || type == eMainStatus::STONE);
	else switch(which_m->m_type) {
		case eRace::DEMON:
			univ.town.set_ash(i,j,true);
			break;
			// TODO: Don't check which_m->number here; find another way to indicate it
		case eRace::UNDEAD:
			if(which_m->number <= 59) univ.town.set_bones(i,j,true);
			break;
		case eRace::SLIME: case eRace::PLANT: case eRace::BUG:
			univ.town.set_sm_slime(i,j,true);
			break;
		case eRace::STONE:
			univ.town.set_rubble(i,j,true);
			break;
		default:
			univ.town.set_sm_blood(i,j,true);
			break;
	}
	
	
	// TODO: Check that this function is not called when a monster kills a monster, since that would lead to false incrementing of the stats.
	if(((is_town()) || (which_combat_type == 1)) && (which_m->summoned == 0)) {
		univ.party.m_killed[univ.town.num]++;
	}
	
	univ.party.total_m_killed++;
	
	which_m->spec1 = 0; // make sure, if this is a spec. activated monster, it won't come back
	
	which_m->active = 0;
}

// Pushes party and monsters around by moving walls and conveyor belts.
// This is very fragile, and only hands a few cases.
void push_things() {
	bool redraw = false;
	short i,k;
	ter_num_t ter;
	location l;
	
	if(is_out()) // TODO: Make these work outdoors
		return;
	if(!belt_present)
		return;
	
	for(i = 0; i < univ.town->max_monst(); i++)
		if(univ.town.monst[i].active > 0) {
			l = univ.town.monst[i].cur_loc;
			ter = univ.town->terrain(l.x,l.y);
			switch(univ.scenario.ter_types[ter].flag1.u) { // TODO: Implement the other 4 possible directions
				case DIR_N: l.y--; break;
				case DIR_E: l.x++; break;
				case DIR_S: l.y++; break;
				case DIR_W: l.x--; break;
			}
			if(l != univ.town.monst[i].cur_loc) {
				univ.town.monst[i].cur_loc = l;
				if((point_onscreen(center,univ.town.monst[i].cur_loc)) ||
					(point_onscreen(center,l)))
					redraw = true;
			}
		}
	for(i = 0; i < NUM_TOWN_ITEMS; i++)
		if(univ.town.items[i].variety != eItemType::NO_ITEM) {
			l = univ.town.items[i].item_loc;
			ter = univ.town->terrain(l.x,l.y);
			switch(univ.scenario.ter_types[ter].flag1.u) { // TODO: Implement the other 4 possible directions
				case DIR_N: l.y--; break;
				case DIR_E: l.x++; break;
				case DIR_S: l.y++; break;
				case DIR_W: l.x--; break;
			}
			if(l != univ.town.items[i].item_loc) {
				univ.town.items[i].item_loc = l;
				if((point_onscreen(center,univ.town.items[i].item_loc)) ||
					(point_onscreen(center,l)))
					redraw = true;
			}
		}
	
	if(is_town()) {
		ter = univ.town->terrain(univ.town.p_loc.x,univ.town.p_loc.y);
		l = univ.town.p_loc;
		switch(univ.scenario.ter_types[ter].flag1.u) { // TODO: Implement the other 4 possible directions
			case DIR_N: l.y--; break;
			case DIR_E: l.x++; break;
			case DIR_S: l.y++; break;
			case DIR_W: l.x--; break;
		}
		if(l != univ.town.p_loc) {
			ASB("You get pushed.");
			if(univ.scenario.ter_types[ter].special == eTerSpec::CONVEYOR)
				draw_terrain(0);
			center = l;
			univ.town.p_loc = l;
			update_explored(l);
			ter = univ.town->terrain(univ.town.p_loc.x,univ.town.p_loc.y);
			draw_map(true);
			if(univ.town.is_barrel(univ.town.p_loc.x,univ.town.p_loc.y)) {
				univ.town.set_barrel(univ.town.p_loc.x,univ.town.p_loc.y,false);
				ASB("You smash the barrel.");
			}
			if(univ.town.is_crate(univ.town.p_loc.x,univ.town.p_loc.y)) {
				univ.town.set_crate(univ.town.p_loc.x,univ.town.p_loc.y,false);
				ASB("You smash the crate.");
			}
			if(univ.town.is_block(univ.town.p_loc.x,univ.town.p_loc.y)) {
				ASB("You crash into the block.");
				hit_party(get_ran(1, 1, 6), eDamageType::WEAPON);
			}
			for(k = 0; k < NUM_TOWN_ITEMS; k++)
				if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].contained
				   && (univ.town.items[k].item_loc == univ.town.p_loc))
					univ.town.items[k].contained = false;
			redraw = true;
		}
	}
	if(is_combat()) {
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE) {
				ter = univ.town->terrain(univ.party[i].combat_pos.x,univ.party[i].combat_pos.y);
				l = univ.party[i].combat_pos;
				switch(univ.scenario.ter_types[ter].flag1.u) { // TODO: Implement the other 4 possible directions
					case DIR_N: l.y--; break;
					case DIR_E: l.x++; break;
					case DIR_S: l.y++; break;
					case DIR_W: l.x--; break;
				}
				if(l != univ.party[i].combat_pos) {
					ASB("Someone gets pushed.");
					ter = univ.town->terrain(l.x,l.y);
					if(univ.scenario.ter_types[ter].special == eTerSpec::CONVEYOR)
						draw_terrain(0);
					univ.party[i].combat_pos = l;
					update_explored(l);
					draw_map(true);
					if(univ.town.is_barrel(univ.party[i].combat_pos.x,univ.party[i].combat_pos.y)) {
						univ.town.set_barrel(univ.party[i].combat_pos.x,univ.party[i].combat_pos.y,false);
						ASB("You smash the barrel.");
					}
					if(univ.town.is_crate(univ.party[i].combat_pos.x,univ.party[i].combat_pos.y)) {
						univ.town.set_crate(univ.party[i].combat_pos.x,univ.party[i].combat_pos.y,false);
						ASB("You smash the crate.");
					}
					if(univ.town.is_block(univ.town.p_loc.x,univ.town.p_loc.y)) {
						ASB("You crash into the block.");
						damage_pc(i,get_ran(1, 1, 6), eDamageType::WEAPON,eRace::UNKNOWN,0);
					}
					for(k = 0; k < NUM_TOWN_ITEMS; k++)
						if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].contained
						   && (univ.town.items[k].item_loc == univ.party[i].combat_pos))
							univ.town.items[k].contained = false;
					redraw = true;
				}
			}
	}
	if(redraw) {
		print_buf();
		draw_terrain(0);
	}
}

void special_increase_age(long length, bool queue) {
	unsigned short i;
	short s1,s2,s3;
	bool redraw = false,stat_area = false;
	location null_loc; // TODO: Should we pass the party's location here? It doesn't quite make sense to me though...
	unsigned long age_before = univ.party.age - length;
	unsigned long current_age = univ.party.age;
	
	if(is_town() || (is_combat() && which_combat_type == 1)) {
		for(i = 0; i < 8; i++)
			if(univ.town->timer_spec_times[i] > 0) {
				short time = univ.town->timer_spec_times[i];
				for(unsigned long j = age_before; j <= current_age; j++)
					if(j % time == 0) {
						if(queue) {
							univ.party.age = j;
							queue_special(eSpecCtx::TOWN_TIMER, 2, univ.town->timer_specs[i], null_loc);
						} else run_special(eSpecCtx::TOWN_TIMER,2,univ.town->timer_specs[i],null_loc,&s1,&s2,&s3);
					}
				stat_area = true;
				if(s3 > 0)
					redraw = true;
			}
	}
	univ.party.age = current_age;
	for(i = 0; i < 20; i++)
		if(univ.scenario.scenario_timer_times[i] > 0) {
			short time = univ.scenario.scenario_timer_times[i];
			for(unsigned long j = age_before; j <= current_age; j++)
				if(j % time == 0) {
					if(queue) {
						univ.party.age = j;
						queue_special(eSpecCtx::SCEN_TIMER, 0, univ.scenario.scenario_timer_specs[i], null_loc);
					} else run_special(eSpecCtx::SCEN_TIMER,0,univ.scenario.scenario_timer_specs[i],null_loc,&s1,&s2,&s3);
				}
			stat_area = true;
			if(s3 > 0)
				redraw = true;
		}
	univ.party.age = current_age;
	for(i = 0; i < univ.party.party_event_timers.size(); i++) {
		if(univ.party.party_event_timers[i].time <= length) {
			univ.party.age = age_before + univ.party.party_event_timers[i].time;
			short which_type = univ.party.party_event_timers[i].global_or_town == 0 ? 0 : 2;
			if(queue)
				queue_special(eSpecCtx::PARTY_TIMER, which_type, univ.party.party_event_timers[i].node_to_call, null_loc);
			else run_special(eSpecCtx::PARTY_TIMER,which_type,univ.party.party_event_timers[i].node_to_call,null_loc,&s1,&s2,&s3);
			univ.party.party_event_timers[i].time = 0;
			stat_area = true;
			if(s3 > 0)
				redraw = true;
		} else univ.party.party_event_timers[i].time -= length;
	}
	univ.party.age = current_age;
	if(stat_area) {
		put_pc_screen();
		put_item_screen(stat_window,0);
	}
	if(redraw)
		draw_terrain(0);
}

void queue_special(eSpecCtx mode, unsigned short which_type, short spec, location spec_loc) {
	if(spec < 0) return;
	pending_special_type queued_special;
	queued_special.spec = spec;
	queued_special.where = spec_loc;
	queued_special.type = which_type;
	queued_special.mode = mode;
	queued_special.trigger_time = univ.party.age;
	special_queue.push(queued_special);
}

void run_special(pending_special_type spec, short* a, short* b, short* redraw) {
	unsigned long store_time = univ.party.age;
	univ.party.age = spec.trigger_time;
	run_special(spec.mode, spec.type, spec.spec, spec.where, a, b, redraw);
	univ.party.age = std::max(univ.party.age, store_time);
}

// This is the big painful one, the main special engine
// which_mode - says when it was called
// 0 - out moving (a - 1 if blocked)
// 1 - town moving (a - 1 if blocked)
// 2 - combat moving (a - 1 if blocked)
// 3 - out looking (a - 1 if don't get items inside) TODO: NOT USED!!!
// 4 - town looking (a - 1 if don't get items inside)
// 5 - entering town
// 6 - leaving town
// 7 - talking (a,b - numbers of strings to respond)
// 8 - using a special item
// 9 - town timer
// 10 - scen timer
// 11 - party countdown timer
// 12 - killed a monst
// 13 - encountering outdoor enc (a - 1 if no fight)
// 14 - winning outdoor enc
// 15 - fleeing outdoor enc
// 16 - target spell on space TODO: Maybe this will become just "target space"?
// 17 - using space
// 18 - seeing monster
// which_type - 0 - scen 1 - out 2 - town
// start spec - the number of the first spec to call
// a,b - 2 values that can be returned
// redraw - 1 if now need redraw
void run_special(eSpecCtx which_mode,short which_type,short start_spec,location spec_loc,short *a,short *b,short *redraw) {
	short cur_spec,cur_spec_type,next_spec,next_spec_type;
	cSpecial cur_node;
	int num_nodes = 0;
	
	// Modify this to put a value in the special node queue instead of raising an error
	if(special_in_progress && start_spec >= 0) {
		queue_special(which_mode, which_type, start_spec, spec_loc);
		return;
	}
	special_in_progress = true;
	next_spec = start_spec;
	next_spec_type = which_type;
	current_pc_picked_in_spec_enc = -1;
	switch(which_mode) {
		case eSpecCtx::OUT_MOVE: case eSpecCtx::TOWN_MOVE: case eSpecCtx::COMBAT_MOVE:
		case eSpecCtx::OUT_LOOK: case eSpecCtx::TOWN_LOOK: case eSpecCtx::ENTER_TOWN: case eSpecCtx::LEAVE_TOWN:
		case eSpecCtx::TALK: case eSpecCtx::USE_SPEC_ITEM: case eSpecCtx::TOWN_HOSTILE:
		case eSpecCtx::TOWN_TIMER: case eSpecCtx::SCEN_TIMER: case eSpecCtx::PARTY_TIMER:
		case eSpecCtx::OUTDOOR_ENC: case eSpecCtx::FLEE_ENCOUNTER: case eSpecCtx::WIN_ENCOUNTER:
			// Default behaviour - select entire party, or active member if split or in combat
			if(is_combat()) current_pc_picked_in_spec_enc = current_pc;
			else {
				if(univ.party.is_split() && cur_node.type != eSpecType::AFFECT_DEADNESS)
					current_pc_picked_in_spec_enc = univ.party.pc_present();
				if(current_pc_picked_in_spec_enc == 6 && univ.party.pc_present(current_pc_picked_in_spec_enc))
					current_pc_picked_in_spec_enc = current_pc_picked_in_spec_enc;
				if(current_pc_picked_in_spec_enc == 6)
					current_pc_picked_in_spec_enc = -1;
			}
			break;
		case eSpecCtx::KILL_MONST: case eSpecCtx::SEE_MONST: case eSpecCtx::MONST_SPEC_ABIL:
		case eSpecCtx::ATTACKED_MELEE: case eSpecCtx::ATTACKING_MELEE:
		case eSpecCtx::ATTACKED_RANGE: case eSpecCtx::ATTACKING_RANGE:
			// The monster/PC on the trigger space is the target
			current_pc_picked_in_spec_enc = 100 + monst_there(spec_loc);
			if(current_pc_picked_in_spec_enc > univ.town->max_monst())
				current_pc_picked_in_spec_enc = pc_there(spec_loc);
			if(current_pc_picked_in_spec_enc == 6)
				current_pc_picked_in_spec_enc = -1;
			break;
		case eSpecCtx::TARGET: case eSpecCtx::USE_SPACE:
			// If there's a monster on the space, select that as the target
			mon_num_t who = monst_there(spec_loc);
			if(who < univ.town->max_monst())
				current_pc_picked_in_spec_enc = 100 + who;
			break;
	}
	store_special_loc = spec_loc;
	if(end_scenario) {
		special_in_progress = false;
		return;
	}
	
	// Store the special's location in reserved pointers
	univ.party.force_ptr(10, 301, 0);
	univ.party.force_ptr(11, 301, 1);
	// And put the location there
	PSD[SDF_SPEC_LOC_X] = spec_loc.x;
	PSD[SDF_SPEC_LOC_Y] = spec_loc.y;
	// Also store the terrain type on that location
	univ.party.force_ptr(12, 301, 2);
	PSD[SDF_SPEC_TER] = coord_to_ter(spec_loc.x, spec_loc.y);
	// And a reference to the string buffer
	univ.party.force_ptr(8, 301, 3);
	PSD[SDF_SPEC_STRBUF] = std::find_if(
		univ.scenario.spec_strs.begin(),
		univ.scenario.spec_strs.end(),
		[](std::string& a) {
			return &a == &univ.scenario.get_buf();
		}) - univ.scenario.spec_strs.begin();

	
	while(next_spec >= 0) {
		
		cur_spec = next_spec;
		cur_spec_type = next_spec_type;
		next_spec = -1;
		cur_node = get_node(cur_spec,cur_spec_type);
		
		
		// Convert pointer values to reference values
		// TODO: Might need to make a database of which nodes don't allow pointers in which slots.
		// This is because some nodes now use -2 as a meaningful value. If that's all, then
		// just disallowing single-digit pointers should suffice, but what about arithmetic?
		// (Of course, currently all SDFs are positive, so allowing negative arithmetic is useless.)
		if(cur_node.sd1 <= -10) cur_node.sd1 = univ.party.get_ptr(-cur_node.sd1);
		if(cur_node.sd2 <= -10) cur_node.sd2 = univ.party.get_ptr(-cur_node.sd2);
		if(cur_node.ex1a <= -10) cur_node.ex1a = univ.party.get_ptr(-cur_node.ex1a);
		if(cur_node.ex1b <= -10) cur_node.ex1b = univ.party.get_ptr(-cur_node.ex1b);
		if(cur_node.ex1c <= -10) cur_node.ex1c = univ.party.get_ptr(-cur_node.ex1c);
		if(cur_node.ex2a <= -10) cur_node.ex2a = univ.party.get_ptr(-cur_node.ex2a);
		if(cur_node.ex2b <= -10) cur_node.ex2b = univ.party.get_ptr(-cur_node.ex2b);
		if(cur_node.ex2c <= -10) cur_node.ex2c = univ.party.get_ptr(-cur_node.ex2c);
		// TODO: Should pointers be allowed in message, pict, or jumpto as well?
		
		//print_nums(1111,cur_spec_type,cur_node.type);
		
		if(cur_node.type == eSpecType::INVALID) {
			special_in_progress = false;
			return;
		}
		switch(getNodeCategory(cur_node.type)) {
			case eSpecCat::GENERAL:
				general_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
				break;
			case eSpecCat::ONCE:
				oneshot_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
				break;
			case eSpecCat::AFFECT:
				affect_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
				break;
			case eSpecCat::IF_THEN:
				ifthen_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
				break;
			case eSpecCat::TOWN:
				townmode_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
				break;
			case eSpecCat::RECT:
				rect_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
				break;
			case eSpecCat::OUTDOOR:
				outdoor_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
				break;
			case eSpecCat::INVALID:
				// TODO: Should it print some kind of error message?
				special_in_progress = false;
				return;
		}
		
		num_nodes++;
		
		if(check_for_interrupt()){
			add_string_to_buf("The special encounter was interrupted. The scenario may be in an unexpected state; it is recommended that you reload from a saved game.", 3);
			next_spec = -1;
		}
	}
	if(is_out())
		erase_out_specials();
	else erase_specials();
	special_in_progress = false;
	
	// TODO: Should find a way to do this that doesn't risk stack overflow
	if(next_spec == -1 && !special_queue.empty()) {
		pending_special_type pending = special_queue.front();
		special_queue.pop();
		run_special(pending, a, b, redraw);
	}
}

cSpecial get_node(short cur_spec,short cur_spec_type) {
	cSpecial dummy_node;
	
	dummy_node = univ.scenario.scen_specials[0];
	dummy_node.type = eSpecType::INVALID;
	if(cur_spec_type == 0) {
		if(cur_spec != minmax(0,255,cur_spec)) {
			giveError("The scenario called a scenario special node out of range.");
			return dummy_node;
		}
		return univ.scenario.scen_specials[cur_spec];
	}
	if(cur_spec_type == 1) {
		if(cur_spec != minmax(0,59,cur_spec)) {
			giveError("The scenario called an outdoor special node out of range.");
			return dummy_node;
		}
		return univ.out->specials[cur_spec];
	}
	if(cur_spec_type == 2) {
		if(cur_spec != minmax(0,99,cur_spec)) {
			giveError("The scenario called a town special node out of range.");
			return dummy_node;
		}
		return univ.town->specials[cur_spec];
	}
	return dummy_node;
}

// TODO: Make cur_spec_type an enum
void general_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
				  short *next_spec,short *next_spec_type,short *a,short *b,short *redraw) {
	bool check_mess = false;
	std::string str1,str2;
	short store_val = 0,i,j;
	cSpecial spec;
	short mess_adj[3] = {160,10,0};
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	switch(cur_node.type) {
		case eSpecType::NONE: break; // null spec
		case eSpecType::SET_SDF:
			check_mess = true; setsd(cur_node.sd1,cur_node.sd2,cur_node.ex1a);
			break;
		case eSpecType::INC_SDF:
			check_mess = true;
			setsd(cur_node.sd1,cur_node.sd2,
				  PSD[cur_node.sd1][cur_node.sd2] + ((cur_node.ex1b == 0) ? 1 : -1) * cur_node.ex1a);
			break;
		case eSpecType::DISPLAY_MSG:
			check_mess = true;
			break;
		case eSpecType::DISPLAY_SM_MSG:
			get_strs(str1,str2, cur_spec_type,cur_node.m1 + mess_adj[cur_spec_type],
					 cur_node.m2 + mess_adj[cur_spec_type]);
			if(cur_node.m1 >= 0)
				ASB(str1.c_str(), 4);
			if(cur_node.m2 >= 0)
				ASB(str2.c_str(), 4);
			break;
		case eSpecType::FLIP_SDF:
			setsd(cur_node.sd1,cur_node.sd2,
				  ((PSD[cur_node.sd1][cur_node.sd2] == 0) ? 1 : 0) );
			check_mess = true;break;
		case eSpecType::CANT_ENTER:
			check_mess = true;
			if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE) {
				if(spec.ex1a != 0)
					*a = 1;
				else {
					*a = 0;
					if(spec.ex2a != 0) *b = 1;
				}
			}
			break;
		case eSpecType::CHANGE_TIME:
			check_mess = true;
			univ.party.age += spec.ex1a;
			// TODO: Should this trigger special events, timers, etc?
			break;
		case eSpecType::SCEN_TIMER_START:
			check_mess = true;
			univ.party.start_timer(spec.ex1a, spec.ex1b, 0);
			break;
		case eSpecType::PLAY_SOUND:
			if(spec.ex1b)
				play_sound(spec.ex1a);
			else play_sound(-spec.ex1a);
			break;
		case eSpecType::CHANGE_HORSE_OWNER:
			check_mess = true;
			if(spec.ex1a != minmax(0,29,spec.ex1a))
				giveError("Horse out of range.");
			else univ.party.horses[spec.ex1a].property = (spec.ex2a == 0) ? 1 : 0;
			break;
		case eSpecType::CHANGE_BOAT_OWNER:
			check_mess = true;
			if(spec.ex1a != minmax(0,29,spec.ex1a))
				giveError("Boat out of range.");
			else univ.party.boats[spec.ex1a].property = (spec.ex2a == 0) ? 1 : 0;
			break;
		case eSpecType::SET_TOWN_VISIBILITY:
			check_mess = true;
			if(spec.ex1a != minmax(0,univ.scenario.towns.size() - 1,spec.ex1a))
				giveError("Town out of range.");
			else univ.party.can_find_town[spec.ex1a] = spec.ex2a;
			*redraw = true;
			break;
		case eSpecType::MAJOR_EVENT_OCCURRED:
			check_mess = true;
			if(spec.ex1a != minmax(1,10,spec.ex1a))
				giveError("Event code out of range.");
			else if(univ.party.key_times[spec.ex1a] == 30000)
				univ.party.key_times[spec.ex1a] = calc_day();
			break;
		case eSpecType::FORCED_GIVE:
			check_mess = true;
			if(!univ.party.forced_give(spec.ex1a,eItemAbil::NONE) && spec.ex1b >= 0)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::BUY_ITEMS_OF_TYPE:
			for(i = 0; i < 144; i++)
				if(univ.party.check_class(spec.ex1a,true))
					store_val++;
			if(store_val == 0) {
				if( spec.ex1b >= 0)
					*next_spec = spec.ex1b;
			}
			else {
				check_mess = true;
				give_gold(store_val * spec.ex2a,true);
			}
			break;
		case eSpecType::CALL_GLOBAL:
			*next_spec_type = 0;
			break;
		case eSpecType::SET_SDF_ROW:
			if(spec.sd1 != minmax(0,299,spec.sd1))
				giveError("Stuff Done flag out of range.");
			else for(i = 0; i < 10; i++) PSD[spec.sd1][i] = spec.ex1a;
			break;
		case eSpecType::COPY_SDF:
			if(!sd_legit(spec.sd1,spec.sd2) || !sd_legit(spec.ex1a,spec.ex1b))
				giveError("Stuff Done flag out of range.");
			else PSD[spec.sd1][spec.sd2] = PSD[spec.ex1a][spec.ex1b];
			break;
		case eSpecType::REST:
			check_mess = true;
			do_rest(spec.ex1a, spec.ex1b, spec.ex1b);
			break;
		case eSpecType::WANDERING_WILL_FIGHT:
			if(which_mode != eSpecCtx::OUTDOOR_ENC)
				break;
			*a = (spec.ex1a == 0) ? 1 : 0;
			break;
		case eSpecType::END_SCENARIO:
			// If party died at some point during the special node, they shouldn't get a victory.
			// (Adapted from Windows version)
			store_val = 6;
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status != eMainStatus::ALIVE)
					store_val--;
			if(store_val == 0) break;
			end_scenario = true;
			break;
		case eSpecType::SET_POINTER:
			if(spec.ex1a < 0)
				giveError("Attempted to assign a pointer out of range (100..199)");
			else try {
				if(spec.sd1 < 0 && spec.sd2 < 0)
					univ.party.clear_ptr(spec.ex1a);
				else univ.party.set_ptr(spec.sd1,spec.sd2,spec.ex1a);
			} catch(std::range_error x) {
				giveError(x.what());
			}
			break;
		case eSpecType::SET_CAMP_FLAG:
			if(!sd_legit(spec.sd1,spec.sd2))
				giveError("Stuff Done flag out of range (x - 0..299, y - 0..49).");
			else {
				set_campaign_flag(spec.sd1,spec.sd2,spec.ex1a,spec.ex1b,spec.m1,spec.ex2a);
			}
			break;
		case eSpecType::DISPLAY_PICTURE:
			// TODO: In addition to the large picture, there's a small icon; should that be customizable?
			check_mess = false;
			get_strs(str1, str1, cur_spec_type, spec.m1, -1);
			custom_pic_dialog(str1, spec.ex1a);
			break;
		case eSpecType::SDF_RANDOM:
			check_mess = true;
			
			short rand;
			// Automatically fix the range in case some idiot puts it in backwards, or the same (WHY)
			if(cur_node.ex1a == cur_node.ex1b) {
				rand = cur_node.ex1b;
			} else {
				rand = get_ran(1,
					min(cur_node.ex1a,cur_node.ex1b),
					max(cur_node.ex1a,cur_node.ex1b)
				);
			}
			setsd(cur_node.sd1,cur_node.sd2,rand);
			//print_nums(rand, cur_node.ex1a, cur_node.ex1b);
			break;
		// SDF arithmetic! :D
		/*
		 SDF1, SDF2 - Output SDF (for division, the quotient)
		 Ex1a, Ex1b - Input SDF (left operand) - if ex1b is -1, takes ex1a as a literal value (which must be positive)
		 Ex2a, Ex2b - Input SDF (right operand) - if ex2b is -1, takes ex2a as a literal value (which must be positive)
		 Ex1c, Ex2c - For division only, output SDF to store the remainder.
		 */
		case eSpecType::SDF_ADD: case eSpecType::SDF_DIFF:
		case eSpecType::SDF_TIMES: case eSpecType::SDF_POWER:
		case eSpecType::SDF_DIVIDE:
			check_mess = true;
			i = spec.ex1b == -1 ? spec.ex1a : PSD[spec.ex1a][spec.ex1b];
			j = spec.ex2b == -1 ? spec.ex2a : PSD[spec.ex2a][spec.ex2b];
			switch(spec.type) {
				case eSpecType::SDF_ADD: setsd(spec.sd1, spec.sd2, i + j); break;
				case eSpecType::SDF_DIFF: setsd(spec.sd1, spec.sd2, i - j); break;
				case eSpecType::SDF_TIMES: setsd(spec.sd1, spec.sd2, i * j); break;
				case eSpecType::SDF_DIVIDE:
					setsd(spec.sd1, spec.sd2, i / j);
					setsd(spec.ex1c, spec.ex2c, i % j);
					break;
				case eSpecType::SDF_POWER:
					if(i == 2) setsd(spec.sd1, spec.sd2, 1 << j);
					else setsd(spec.sd1, spec.sd2, pow(i, j));
					break;
				default: // Unreachable case
					break;
			}
			break;
		case eSpecType::PRINT_NUMS:
			if(!in_scen_debug) break;
			check_mess = false;
			get_strs(str1,str2, cur_spec_type,cur_node.m1 + mess_adj[cur_spec_type],
					 cur_node.m2 + mess_adj[cur_spec_type]);
			if(cur_node.m1 >= 0)
				ASB("debug: " + str1, 7);
			if(cur_node.m2 >= 0)
				ASB("debug: " + str2, 7);
			// TODO: Give more options?
			switch(spec.pic) {
				case 0: // Print SDF contents
					print_nums(spec.sd1, spec.sd2, univ.party.stuff_done[spec.sd1][spec.sd2]);
					break;
				case 1: // Print three literal values (which might be pointers!)
					print_nums(spec.ex1a, spec.ex1b, spec.ex1c);
					break;
				case 2: // Print monster health and spell points
					if(spec.ex1a >= univ.town->max_monst()) break;
					print_nums(spec.ex1a, univ.town.monst[spec.ex1a].health, univ.town.monst[spec.ex1a].mp);
					break;
			}
			break;
		case eSpecType::CHANGE_TER:
			set_terrain(loc(spec.ex1a,spec.ex1b),spec.ex2a);
			*redraw = true;
			draw_map(true);
			check_mess = true;
			break;
		case eSpecType::SWAP_TER:
			if(coord_to_ter(spec.ex1a,spec.ex1b) == spec.ex2a){
				set_terrain(loc(spec.ex1a,spec.ex1b),spec.ex2b);
			}
			else if(coord_to_ter(spec.ex1a,spec.ex1b) == spec.ex2b){
				set_terrain(loc(spec.ex1a,spec.ex1b),spec.ex2a);
			}
			*redraw = 1;
			draw_map(true);
			check_mess = true;
			break;
		case eSpecType::TRANS_TER:
			set_terrain(loc(spec.ex1a,spec.ex1b),univ.scenario.ter_types[coord_to_ter(spec.ex1a,spec.ex1b)].trans_to_what);
			*redraw = 1;
			draw_map(true);
			check_mess = true;
			break;
		case eSpecType::ENTER_SHOP:
			get_strs(str1,str2,1,spec.m1,-1);
			if(spec.ex2a >= 40)
				spec.ex2a = 39;
			if(spec.ex2a < 1)
				spec.ex2a = 1;
			spec.ex2b = minmax(0,6,spec.ex2b);
			start_shop_mode(eShopType(spec.ex1b), spec.ex1a, spec.ex1a + spec.ex2a - 1, spec.ex2b, str1);
			*next_spec = -1;
			break;
		case eSpecType::STORY_DIALOG:
			get_strs(str1,str2,cur_spec_type,spec.m1,-1);
			story_dialog(str1, spec.m2, spec.m3, cur_spec_type, spec.pic, ePicType(spec.pictype));
			break;
		case eSpecType::CLEAR_BUF:
			univ.scenario.get_buf().clear();
			break;
		case eSpecType::APPEND_STRING:
			get_strs(str1,str1,cur_spec_type,spec.ex1a,-1);
			univ.scenario.get_buf() += str1;
			break;
		case eSpecType::APPEND_NUM:
			univ.scenario.get_buf() += std::to_string(spec.ex1a);
			break;
		case eSpecType::APPEND_MONST:
			if(spec.ex1a == 0) {
				int pc = current_pc_picked_in_spec_enc;
				if(pc < 0)
					univ.scenario.get_buf() += "Your party";
				else if(pc < 100)
					univ.scenario.get_buf() += univ.party[pc].name;
				else if(!is_out())
					univ.scenario.get_buf() += univ.town.monst[pc - 100].m_name;
			} else univ.scenario.get_buf() += univ.scenario.scen_monsters[spec.ex1a].m_name;
			break;
		case eSpecType::APPEND_ITEM:
			if(spec.ex1b)
				univ.scenario.get_buf() += univ.scenario.scen_items[spec.ex1a].full_name;
			else univ.scenario.get_buf() += univ.scenario.scen_items[spec.ex1a].name;
			break;
		case eSpecType::APPEND_TER:
			univ.scenario.get_buf() += univ.scenario.ter_types[spec.ex1a].name;
			break;
		case eSpecType::PAUSE:
			if(spec.ex1a < 0) break;
			redraw_screen(REFRESH_TERRAIN | REFRESH_STATS);
			sf::sleep(sf::milliseconds(spec.ex1a));
			break;
		case eSpecType::START_TALK:
			i = current_pc_picked_in_spec_enc;
			if(i >= 100) i -= 100;
			else i = -1;
			start_talk_mode(i, spec.ex1a, spec.ex1b, spec.pic);
			*next_spec = -1;
			break;
	}
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
}

/*
 void general_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
 short *next_spec,short *next_spec_type,short *a,short *b,short *redraw)
 {
 bool check_mess = false;
 char str1[256] = "",str2 = "";
 short store_val = 0,i,j;
 special_node_type spec;
 
 spec = cur_node;
 *next_spec = cur_node.jumpto;
 
 switch(cur_node.type) {
 case :
 break;
 }
 if(check_mess) {
 handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
 }
 }
 */

// TODO: What was next_spec_type for? Is it still needed?
void oneshot_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
				  short* next_spec,short* next_spec_type,short* a,short* b,short* redraw) {
	bool check_mess = true,set_sd = true;
	std::array<std::string, 6> strs;
	short i,j;
	std::array<short, 3> buttons = {-1,-1,-1};
	cSpecial spec;
	cItem store_i;
	location l;
	std::string choice;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	if((sd_legit(spec.sd1,spec.sd2)) && (PSD[spec.sd1][spec.sd2] == 250)) {
		*next_spec = -1;
		return;
	}
	switch(cur_node.type) {
		case eSpecType::ONCE_GIVE_ITEM:
			if(!univ.party.forced_give(spec.ex1a,eItemAbil::NONE)) {
				set_sd = false;
				if( spec.ex2b >= 0)
					*next_spec = spec.ex2b;
			}
			else {
				give_gold(spec.ex1b,true);
				give_food(spec.ex2a,true);
			}
			break;
		case eSpecType::ONCE_GIVE_SPEC_ITEM:
			if(spec.ex1a != minmax(0,49,spec.ex1a)) {
				giveError("Special item is out of range.");
				set_sd = false;
			}
			else {
				univ.party.spec_items[spec.ex1a] = spec.ex1b == 0;
			}
			if(stat_window == 6)
				set_stat_window(6);
			*redraw = 1;
			break;
		case eSpecType::ONCE_NULL:
			set_sd = false;
			check_mess = false;
			break;
		case eSpecType::ONCE_SET_SDF:
			check_mess = false;
			break;
		case eSpecType::ONCE_DIALOG:
			check_mess = false;
			if(spec.m1 < 0)
				break;
			for(i = 0; i < 3; i++)
				get_strs(strs[i * 2],strs[i * 2 + 1],cur_spec_type, spec.m1 + i * 2,spec.m1 + i * 2 + 1);
			if(spec.m3 > 0) {
				buttons[0] = 1;
				buttons[1] = spec.ex1a;
				buttons[2] = spec.ex2a;
				if((spec.ex1a >= 0) || (spec.ex2a >= 0))
					buttons[0] = 20;
			}
			if(spec.m3 <= 0) {
				buttons[0] = spec.ex1a;
				buttons[1] = spec.ex2a;
			}
			if((buttons[0] < 0) && (buttons[1] < 0)) {
				giveError("Dialog box ended up with no buttons.");
				break;
			}
			i = custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons);
			if(spec.m3 > 0) {
				if(i == 1) {
					if((spec.ex1a >= 0) || (spec.ex2a >= 0)) {
						set_sd = false;
					}
				}
				if(i == 2) *next_spec = spec.ex1b;
				if(i == 3) *next_spec = spec.ex2b;
			}
			else {
				if(i == 1) *next_spec = spec.ex1b;
				if(i == 2) *next_spec = spec.ex2b;
			}
			break;
		case eSpecType::ONCE_GIVE_ITEM_DIALOG:
			check_mess = false;
			if(spec.m1 < 0)
				break;
			for(i = 0; i < 3; i++)
				get_strs(strs[i * 2],strs[i * 2 + 1],cur_spec_type, spec.m1 + i * 2,spec.m1 + i * 2 + 1);
			buttons[0] = 20; buttons[1] = 19;
			i = custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons);
			if(i == 1) {set_sd = false; *next_spec = -1;}
			else {
				store_i = get_stored_item(spec.ex1a);
				if((spec.ex1a >= 0) && (!univ.party.give_item(store_i,true))) {
					set_sd = false; *next_spec = -1;
				}
				else {
					give_gold(spec.ex1b,true);
					give_food(spec.ex2a,true);
					if((spec.m3 >= 0) && (spec.m3 < 50)) {
						if(!univ.party.spec_items[spec.m3])
							ASB("You get a special item.");
						univ.party.spec_items[spec.m3] = true;
						*redraw = true;
						if(stat_window == 6)
							set_stat_window(6);
					}
					if(spec.ex2b >= 0) *next_spec = spec.ex2b;
				}
			}
			break;
		case eSpecType::ONCE_OUT_ENCOUNTER:
			if(spec.ex1a != minmax(0,3,spec.ex1a)) {
				giveError("Special outdoor enc. is out of range. Must be 0-3.");
				set_sd = false;
			}
			else {
				l = global_to_local(univ.party.p_loc);
				place_outd_wand_monst(l, univ.out->special_enc[spec.ex1a],true);
			}
			break;
		case eSpecType::ONCE_TOWN_ENCOUNTER:
			activate_monsters(spec.ex1a,0);
			break;
		case eSpecType::ONCE_TRAP:
			check_mess = false;
			if((spec.m1 >= 0) || (spec.m2 >= 0)) {
				get_strs(strs[0],strs[1], cur_spec_type, spec.m1, spec.m2);
				buttons[0] = 3; buttons[1] = 2;
				i = custom_choice_dialog(strs,spec.pic,ePicType(spec.pictype),buttons);
				// TODO: Make custom_choice_dialog return string?
			}
			else i = cChoiceDlog("basic-trap",{"yes","no"}).show() == "no";
			if(i == 1) {
				set_sd = false;
				*next_spec = -1;
				*a = 1;
			} else {
				if(!is_combat()) {
					j = char_select_pc(0,"Trap! Who will disarm?");
					if(j == 6){
						*a = 1;
						set_sd = false;
					}
				} else j = current_pc;
				bool disarmed = run_trap(j,eTrapType(spec.ex1a),spec.ex1b,spec.ex2a);
				if(!disarmed && spec.ex1a == TRAP_CUSTOM) {
					if(spec.jumpto >= 0)
						queue_special(which_mode, cur_spec_type, spec.jumpto, loc(PSD[SDF_SPEC_LOC_X], PSD[SDF_SPEC_LOC_Y]));
					*next_spec = spec.ex2b;
					*next_spec_type = 0;
				}
			}
			break;
	}
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
	if((set_sd) && (sd_legit(spec.sd1,spec.sd2)))
		PSD[spec.sd1][spec.sd2] = 250;
	
}

// TODO: What was next_spec_type for? Is it still needed?
void affect_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
				 short *next_spec,short* /*next_spec_type*/,short *a,short *b,short *redraw) {
	bool check_mess = true;
	short i,pc = current_pc_picked_in_spec_enc,r1;
	std::string str;
	cSpecial spec;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	switch(cur_node.type) {
		case eSpecType::SELECT_TARGET:
			check_mess = false;
			// If this <= 0, pick PC normally
			// TODO: I think this is for compatibility with old scenarios? If so, remove it and just convert data on load.
			// (Actually, I think the only compatibility thing is that it's <= instead of ==)
			if(spec.ex2a <= 0) {
				
				if(spec.ex1a == 2)
					current_pc_picked_in_spec_enc = -1;
				else if(spec.ex1a == 1) {
					i = select_pc(0);
					if(i != 6)
						current_pc_picked_in_spec_enc = i;
				}
				else if(spec.ex1a == 0) {
					i = select_pc(1);
					if(i != 6)
						current_pc_picked_in_spec_enc = i;
				}
				else if(spec.ex1a == 3) {
					i = select_pc(2);
					if(i != 6)
						current_pc_picked_in_spec_enc = i;
				}
				else if(spec.ex1a == 4) {
					i = select_pc(3);
					if(i != 6)
						current_pc_picked_in_spec_enc = i;
				}
				if(i == 6)// && (spec.ex1b >= 0))
					*next_spec = spec.ex1b;
				
			}
			else if(spec.ex2a > 10 || spec.ex2a <= 16) {
				// Select a specific PC
				short pc = spec.ex2a - 11;
				// Honour the request for alive PCs only.
				bool can_pick = true;
				if(univ.party[pc].main_status == eMainStatus::ABSENT)
					can_pick = false;
				else if(spec.ex1a % 4 == 0 && univ.party[pc].main_status != eMainStatus::ALIVE)
					can_pick = false;
				else if(spec.ex1a == 3 && univ.party[pc].main_status == eMainStatus::ALIVE)
					can_pick = false;
				else if(spec.ex1a == 4 && univ.party[pc].has_space() == 24)
					can_pick = false;
				if(can_pick)
					current_pc_picked_in_spec_enc = pc;
				else *next_spec = spec.ex1b;
			} else if(spec.ex2a >= 100) {
				short monst = spec.ex2a - 100;
				// Honour the request for alive only
				bool can_pick = true;
				if(spec.ex1a == 0 && univ.town.monst[monst].active == 0)
					can_pick = false;
				else if(spec.ex1a == 3 && univ.town.monst[monst].active > 0)
					can_pick = false;
				if(can_pick)
					current_pc_picked_in_spec_enc = spec.ex2a;
				else *next_spec = spec.ex1b;
			} else if(spec.ex2a == 1) {
				// Pick random PC (from *i)
				
				if(spec.ex1a == 0) {
					bool can_pick = false;
					int tries = 0;
					while(!can_pick && tries < 100) {
						i = get_ran(1,0,5);
						can_pick = true;
						if(univ.party[i].main_status == eMainStatus::ABSENT)
							can_pick = false;
						else if(spec.ex1a % 4 == 0 && univ.party[i].main_status != eMainStatus::ALIVE)
							can_pick = false;
						else if(spec.ex1a == 3 && univ.party[i].main_status == eMainStatus::ALIVE)
							can_pick = false;
						else if(spec.ex1a == 4 && univ.party[i].has_space() == 24)
							can_pick = false;
						tries++;
					}
					if(can_pick)
						current_pc_picked_in_spec_enc = i;
					else *next_spec = spec.ex1b;
				}
				else {
					i = get_ran(1,0,5);
					current_pc_picked_in_spec_enc = i;
				}
			}
			break;
		case eSpecType::DAMAGE: {
			r1 = get_ran(spec.ex1a,1,spec.ex1b) + spec.ex2a;
			eDamageType dam_type = (eDamageType) spec.ex2b;
			int snd_type = spec.ex2c < 0 ? 0 : -spec.ex2c;
			if(pc < 0) hit_party(r1, dam_type, snd_type);
			else if(pc >= 100) damage_monst(pc - 100, 7, r1, 0, dam_type, snd_type);
			else damage_pc(pc,r1,dam_type,eRace::UNKNOWN, snd_type);
			break;
		}
		case eSpecType::AFFECT_HP:
			if(pc < 100) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i))
						univ.party[i].cur_health = minmax(0,univ.party[i].max_health,
														  univ.party[i].cur_health + spec.ex1a * (spec.ex1b ? -1: 1));
			}
			else {
				cCreature& who = univ.town.monst[pc - 100];
				who.health = minmax(0, who.m_health, who.health + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
				if(spec.ex1b == 0)
					monst_spell_note(who.number,41);
				else monst_spell_note(who.number,42);
			}
			break;
		case eSpecType::AFFECT_SP:
			if(pc < 100) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i))
						univ.party[i].cur_sp = minmax(0, univ.party[i].max_sp,
							univ.party[i].cur_sp + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			}
			else {
				cCreature& who = univ.town.monst[pc - 100];
				who.mp = minmax(0, who.max_mp, who.mp + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
				if(spec.ex1b == 0)
					monst_spell_note(who.number,43);
				else monst_spell_note(who.number,44);
			}
			break;
		case eSpecType::AFFECT_XP:
			if(pc >= 100) break;
			for(i = 0; i < 6; i++)
				if((pc < 0) || (pc == i)) {
					if(spec.ex1b == 0) award_xp(i,spec.ex1a); else drain_pc(i,spec.ex1a);
				}
			break;
		case eSpecType::AFFECT_SKILL_PTS:
			if(pc >= 100) break;
			for(i = 0; i < 6; i++)
				if((pc < 0) || (pc == i))
					univ.party[i].skill_pts = minmax(0,	100,
						univ.party[i].skill_pts + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case eSpecType::AFFECT_DEADNESS:
			if(pc < 100) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i)) {
						if(spec.ex1b == 0) {
							if(spec.ex1a == 3 && is_combat() && which_combat_type == 0 && univ.party[i].main_status == eMainStatus::FLED)
								univ.party[i].main_status = eMainStatus::ALIVE;
							else if(spec.ex1a == 4)
								univ.party[i].main_status -= eMainStatus::SPLIT;
							else if(spec.ex1a == 5)
								univ.party[i].main_status = eMainStatus::ALIVE;
							else if((univ.party[i].main_status > eMainStatus::ABSENT) && (univ.party[i].main_status < eMainStatus::SPLIT))
								univ.party[i].main_status = eMainStatus::ALIVE;
						}
						else if(univ.party[i].main_status == eMainStatus::ABSENT);
						else switch(spec.ex1a){
								// When passed to kill_pc, the SPLIT party status actually means "no saving throw".
							case 0:
								kill_pc(i,spec.ex1c > 0 ? eMainStatus::DEAD : eMainStatus::SPLIT_DEAD);
								break;
							case 1:
								kill_pc(i,spec.ex1c > 0 ? eMainStatus::DUST : eMainStatus::SPLIT_DUST);
								break;
							case 2:
								if(spec.ex1c > 0)
									petrify_pc(i,spec.ex1c);
								else kill_pc(i,eMainStatus::SPLIT_STONE);
								break;
							case 3:
								if(!is_combat() || which_combat_type != 0)
									break;
								if(univ.party[i].main_status == eMainStatus::ALIVE)
									univ.party[i].main_status = eMainStatus::FLED;
								break;
							case 4:
								if(i != univ.party.pc_present())
									univ.party[i].main_status += eMainStatus::SPLIT;
								break;
							case 5:
								kill_pc(i,spec.ex1c > 0 ? eMainStatus::ABSENT : eMainStatus::SPLIT_ABSENT);
								break;
						}
					}
				*redraw = 1;
			} else {
				// Kill monster
				cCreature& who = univ.town.monst[pc - 100];
				if(who.active > 0 && spec.ex1b > 0) {
					switch(spec.ex1a) {
						case 0:
							monst_spell_note(who.number,46);
							kill_monst(&who,7,eMainStatus::DEAD);
							break;
						case 1:
							monst_spell_note(who.number,51);
							kill_monst(&who,7,eMainStatus::DUST);
							break;
						case 2:
							if(spec.ex1c > 0)
								petrify_monst(&who, spec.ex1c);
							else {
								monst_spell_note(who.number,8);
								kill_monst(&who,7,eMainStatus::STONE);
							}
							break;
						case 5:
							who.active = 0;
							break;
					}
				}
				// Bring back to life
				else if(who.active == 0 && spec.ex1b == 0) {
					who.active = 1;
					monst_spell_note(who.number,45);
				}
			}
			break;
		case eSpecType::AFFECT_STATUS:
			if(pc < 100) {
				for(i = 0; i < 6; i++)
					if(pc < 0 || pc == i) {
						switch(eStatus(spec.ex2a)) {
							case eStatus::POISON:
								if(spec.ex1b == 0)
									cure_pc(i, spec.ex1a);
								else poison_pc(i, spec.ex1a);
								break;
							case eStatus::HASTE_SLOW:
								if(spec.ex1b == 0)
									slow_pc(i, -spec.ex1a);
								else slow_pc(i, spec.ex1a);
								break;
							case eStatus::INVULNERABLE:
								if(spec.ex1b == 0)
									univ.party[i].apply_status(eStatus::INVULNERABLE, spec.ex1a);
								else univ.party[i].apply_status(eStatus::INVULNERABLE, -spec.ex1a);
								break;
							case eStatus::MAGIC_RESISTANCE:
								if(spec.ex1b == 0)
									univ.party[i].apply_status(eStatus::MAGIC_RESISTANCE, spec.ex1a);
								else univ.party[i].apply_status(eStatus::MAGIC_RESISTANCE, -spec.ex1a);
								break;
							case eStatus::WEBS:
								if(spec.ex1b == 0)
									univ.party[i].apply_status(eStatus::WEBS, -spec.ex1a);
								else web_pc(i, spec.ex1a);
								break;
							case eStatus::DISEASE:
								if(spec.ex1b == 0)
									univ.party[i].apply_status(eStatus::DISEASE, -spec.ex1a);
								else disease_pc(i, spec.ex1a);
								break;
							case eStatus::INVISIBLE:
								if(spec.ex1b == 0)
									univ.party[i].apply_status(eStatus::INVISIBLE, spec.ex1a);
								else univ.party[i].apply_status(eStatus::INVISIBLE, -spec.ex1a);
								break;
							case eStatus::BLESS_CURSE:
								if(spec.ex1b == 0)
									curse_pc(i, -spec.ex1a);
								else curse_pc(i, spec.ex1a);
								break;
							case eStatus::DUMB:
								if(spec.ex1b == 0)
									univ.party[i].apply_status(eStatus::DUMB, -spec.ex1a);
								else dumbfound_pc(i, spec.ex1a);
								break;
							case eStatus::ASLEEP:
								if(spec.ex1b == 0)
									univ.party[i].apply_status(eStatus::ASLEEP, -spec.ex1a);
								else sleep_pc(i, spec.ex1a, eStatus::ASLEEP, 10);
								break;
							case eStatus::PARALYZED:
								if(spec.ex1b == 0)
									univ.party[i].apply_status(eStatus::PARALYZED, -spec.ex1a);
								else sleep_pc(i, spec.ex1a, eStatus::PARALYZED, 10);
								break;
							case eStatus::POISONED_WEAPON:
								if(spec.ex1b == 0)
									poison_weapon(i, spec.ex1a, true);
								else univ.party[i].apply_status(eStatus::POISONED_WEAPON, -spec.ex1a);
								break;
							case eStatus::MARTYRS_SHIELD:
								if(spec.ex1b == 0)
									univ.party[i].apply_status(eStatus::MARTYRS_SHIELD, spec.ex1a);
								else univ.party[i].apply_status(eStatus::MARTYRS_SHIELD, -spec.ex1a);
								break;
							case eStatus::ACID:
								if(spec.ex1b == 0)
									univ.party[i].apply_status(eStatus::ACID, -spec.ex1a);
								else acid_pc(i, spec.ex1a);
								break;
								// Invalid values
							case eStatus::MAIN:
							case eStatus::CHARM:
							case eStatus::FORCECAGE:
								break;
						}
					}
			}
			else {
				cCreature& who = univ.town.monst[pc - 100];
				if(who.active > 0) {
					switch(eStatus(spec.ex2a)) {
						case eStatus::POISON:
							if(spec.ex1b == 0)
								poison_monst(&who, -spec.ex1a);
							else poison_monst(&who, spec.ex1a);
							break;
						case eStatus::HASTE_SLOW:
							if(spec.ex1b == 0)
								slow_monst(&who, -spec.ex1a);
							else slow_monst(&who, spec.ex1a);
							break;
						case eStatus::WEBS:
							if(spec.ex1b == 0)
								web_monst(&who, -spec.ex1a);
							else web_monst(&who, spec.ex1a);
							break;
						case eStatus::DISEASE:
							if(spec.ex1b == 0)
								disease_monst(&who, -spec.ex1a);
							else disease_monst(&who, spec.ex1a);
							break;
						case eStatus::BLESS_CURSE:
							if(spec.ex1b == 0)
								curse_monst(&who, -spec.ex1a);
							else curse_monst(&who, spec.ex1a);
							break;
						case eStatus::DUMB:
							if(spec.ex1b == 0)
								dumbfound_monst(&who, -spec.ex1a);
							else dumbfound_monst(&who, spec.ex1a);
							break;
						case eStatus::ASLEEP:
							if(spec.ex1b == 0)
								charm_monst(&who, 0, eStatus::ASLEEP, -spec.ex1a);
							else charm_monst(&who, 0, eStatus::ASLEEP, spec.ex1a);
							break;
						case eStatus::PARALYZED:
							if(spec.ex1b == 0)
								charm_monst(&who, 0, eStatus::PARALYZED, -spec.ex1a);
							else charm_monst(&who, 0, eStatus::PARALYZED, spec.ex1a);
							break;
						case eStatus::MARTYRS_SHIELD:
							if(spec.ex1b == 0)
								who.status[eStatus::MARTYRS_SHIELD] = min(10, who.status[eStatus::MARTYRS_SHIELD] + spec.ex1a);
							else who.status[eStatus::MARTYRS_SHIELD] = max(0, who.status[eStatus::MARTYRS_SHIELD] - spec.ex1a);
							break;
						case eStatus::ACID:
							if(spec.ex1b == 0)
								acid_monst(&who, -spec.ex1a);
							else acid_monst(&who, spec.ex1a);
							break;
							// Invalid values
						case eStatus::MAIN:
						case eStatus::FORCECAGE:
						case eStatus::POISONED_WEAPON:
						case eStatus::INVULNERABLE:
						case eStatus::MAGIC_RESISTANCE:
						case eStatus::INVISIBLE:
						case eStatus::CHARM:
							break;
					}
				}
			}
			break;
		case eSpecType::AFFECT_STAT:
			if(pc >= 100) break;
			if(spec.ex2a != minmax(0,20,spec.ex2a)) {
				giveError("Skill is out of range.");
				break;
			}
			for(i = 0; i < 6; i++)
				if((pc < 0 || pc == i) && get_ran(1,1,100) < spec.pic) {
					eSkill skill = eSkill(spec.ex2a);
					int adj = spec.ex1a * (spec.ex1b != 0 ? -1: 1);
					if(skill == eSkill::MAX_HP)
						univ.party[i].max_health = minmax(6, 250, univ.party[i].max_health + adj);
					else if(skill == eSkill::MAX_SP)
						univ.party[i].max_sp = minmax(0, 150, univ.party[i].max_sp + adj);
					else univ.party[i].skills[skill] = minmax(0, skill_max[skill], univ.party[i].skills[skill] + adj);
				}
			break;
		case eSpecType::AFFECT_MAGE_SPELL:
			if(pc >= 100) break;
			if(spec.ex1a != minmax(0,61,spec.ex1a)) {
				giveError("Mage spell is out of range (0 - 61). See docs.");
				break;
			}
			for(i = 0; i < 6; i++)
				if((pc < 0) || (pc == i))
					univ.party[i].mage_spells[spec.ex1a] = spec.ex1b;
			break;
		case eSpecType::AFFECT_PRIEST_SPELL:
			if(pc >= 100) break;
			if(spec.ex1a != minmax(0,61,spec.ex1a)) {
				giveError("Priest spell is out of range (0 - 61). See docs.");
				break;
			}
			for(i = 0; i < 6; i++)
				if((pc < 0) || (pc == i))
					univ.party[i].priest_spells[spec.ex1a] = spec.ex1b;
			break;
		case eSpecType::AFFECT_GOLD:
			if(spec.ex1b == 0)
				give_gold(spec.ex1a,true);
			else if(univ.party.gold < spec.ex1a)
				univ.party.gold = 0;
			else take_gold(spec.ex1a,false);
			break;
		case eSpecType::AFFECT_FOOD:
			if(spec.ex1b == 0)
				give_food(spec.ex1a,true);
			else if(univ.party.food < spec.ex1a)
				univ.party.food = 0;
			else take_food(spec.ex1a,false);
			break;
		case eSpecType::AFFECT_ALCHEMY:
			if(spec.ex1a != minmax(0,19,spec.ex1a)) {
				giveError("Alchemy is out of range.");
				break;
			}
			univ.party.alchemy[spec.ex1a] = true;
			break;
		case eSpecType::AFFECT_PARTY_STATUS:
			if(spec.ex2a < 0 || spec.ex2a > 3) break;
			if(spec.ex2a == 1 && univ.party.in_boat >= 0)
				add_string_to_buf("  Can't fly when on a boat. ");
			else if(spec.ex2a == 1 && univ.party.in_horse >= 0)////
				add_string_to_buf("  Can't fly when on a horse.  ");
			r1 = univ.party.status[ePartyStatus(spec.ex2a)];
			r1 = minmax(0,250,r1 + spec.ex1a);
			univ.party.status[ePartyStatus::STEALTH] = r1;
			break;
		case eSpecType::AFFECT_TRAITS:
			if(pc >= 100) break;
			if(spec.ex1a < 0 || spec.ex1a > 15) {
				giveError("Trait is out of range (0 - 15).");
				break;
			}
			for(i = 0; i < 6; i++)
				if(pc < 0 || pc == i)
					univ.party[i].traits[eTrait(spec.ex1a)] = !spec.ex1b;
			break;
		case eSpecType::AFFECT_AP:
			if(!is_combat()) break;
			if(pc < 100) {
				for(i = 0; i < 6; i++)
					if(pc < 0 || pc == i) {
						if(spec.ex1b)
							univ.party[i].ap += spec.ex1a;
						else univ.party[i].ap -= spec.ex1a;
						if(univ.party[i].ap < 0)
							univ.party[i].ap = 0;
					}
			} else {
				cCreature& who = univ.town.monst[pc - 100];
				if(spec.ex1b)
					who.ap += spec.ex1a;
				else who.ap -= spec.ex1a;
				if(who.ap < 0)
					who.ap = 0;
			}
			break;
		case eSpecType::AFFECT_NAME:
			get_strs(str, str, 0, spec.m3, -1);
			if(pc < 100) {
				for(i = 0; i < 6; i++)
					if(pc < 0 || pc == i)
						univ.party[i].name = str;
			} else univ.town.monst[pc - 100].m_name = str;
			break;
		case eSpecType::CREATE_NEW_PC:
			if(spec.ex1c < 0 || spec.ex1c > 19) {
				giveError("Race out of range (0 - 19).");
				break;
			}
			pc = univ.party.free_space();
			if(pc == 6) {
				add_string_to_buf("No room for new PC.");
				*next_spec = spec.pictype;
				check_mess = false;
				break;
			}
			current_pc_picked_in_spec_enc = pc;
			get_strs(str, str, 0, spec.m3, -1);
			univ.party.new_pc(pc);
			univ.party[pc].name = str;
			univ.party[pc].which_graphic = spec.pic;
			univ.party[pc].cur_health = univ.party[pc].max_health = spec.ex1a;
			univ.party[pc].cur_sp = univ.party[pc].max_sp = spec.ex1b;
			univ.party[pc].race = eRace(spec.ex1c);
			univ.party[pc].skills[eSkill::STRENGTH] = spec.ex2a;
			univ.party[pc].skills[eSkill::DEXTERITY] = spec.ex2b;
			univ.party[pc].skills[eSkill::INTELLIGENCE] = spec.ex2c;
			break;
	}
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
}

static bool isValidField(int fld, bool allowSpecial) {
	if(fld <= SPECIAL_EXPLORED)
		return false;
	if(fld == SPECIAL_SPOT)
		return false;
	if(fld >= WALL_FORCE && fld <= BARRIER_CAGE)
		return true;
	if(!allowSpecial)
		return false;
	if(fld == FIELD_DISPEL || fld == FIELD_SMASH)
		return true;
	return false;
}

// TODO: What was next_spec_type for? Is it still needed?
void ifthen_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
				 short *next_spec,short* /*next_spec_type*/,short *a,short *b,short *redraw) {
	bool check_mess = false;
	std::string str1, str2, str3;
	short i,j,k;
	cSpecial spec;
	location l;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	switch(cur_node.type) {
		case eSpecType::IF_SDF:
			if(sd_legit(spec.sd1,spec.sd2)) {
				if((spec.ex1a >= 0) && (PSD[spec.sd1][spec.sd2] >= spec.ex1a))
					*next_spec = spec.ex1b;
				else if((spec.ex2a >= 0) && (PSD[spec.sd1][spec.sd2] < spec.ex2a))
					*next_spec = spec.ex2b;
			}
			break;
		case eSpecType::IF_TOWN_NUM:
			if(((is_town()) || (is_combat())) && (univ.town.num == spec.ex1a))
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_RANDOM:
			if(get_ran(1,1,100) < spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_HAVE_SPECIAL_ITEM:
			if(spec.ex1a != minmax(0,49,spec.ex1a)) {
				giveError("Special item is out of range.");
			}
			else if(univ.party.spec_items[spec.ex1a] > 0)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_SDF_COMPARE:
			if((sd_legit(spec.sd1,spec.sd2)) && (sd_legit(spec.ex1a,spec.ex1b))) {
				if(PSD[spec.ex1a][spec.ex1b] < PSD[spec.sd1][spec.sd2])
					*next_spec = spec.ex2b;
			}
			else giveError("A Stuff Done flag is out of range.");
			break;
		case eSpecType::IF_TER_TYPE:
			l.x = spec.ex1a; l.y = spec.ex1b;
			l = local_to_global(l);
			if((is_town() || is_combat()) && univ.town->terrain(spec.ex1a,spec.ex1b) == spec.ex2a)
				*next_spec = spec.ex2b;
			else if(is_out() && univ.out[l.x][l.y] == spec.ex2a)
				*next_spec = spec.ex2b;
			break;
		case eSpecType::IF_HAS_GOLD:
			if(univ.party.gold >= spec.ex1a) {
				if(spec.ex2a) take_gold(spec.ex1a,true);
				*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_HAS_FOOD:
			if(univ.party.food >= spec.ex1a) {
				if(spec.ex2a) take_food(spec.ex1a,true);
				*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_ITEM_CLASS_ON_SPACE:
			if(is_out())
				break;
			l.x = spec.ex1a; l.y = spec.ex1b;
			for(i = 0; i < NUM_TOWN_ITEMS; i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].special_class == (unsigned)spec.ex2a
				   && l == univ.town.items[i].item_loc) {
					*next_spec = spec.ex2b;
					if(spec.ex2c) {
						*redraw = 1;
						univ.town.items[i].variety = eItemType::NO_ITEM;
					}
				}
			break;
		case eSpecType::IF_HAVE_ITEM_CLASS:
			if(univ.party.check_class(spec.ex1a,spec.ex2a))
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_EQUIP_ITEM_CLASS:
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE)
					for(j = 0; j < 24; j++)
						if(univ.party[i].items[j].variety != eItemType::NO_ITEM && univ.party[i].items[j].special_class == (unsigned)spec.ex1a
						   && univ.party[i].equip[j]) {
							*next_spec = spec.ex1b;
							if(spec.ex2c) {
								*redraw = 1;
								univ.party[i].take_item(j);
								if(i == stat_window)
									put_item_screen(stat_window,1);
							}
						}
			break;
		case eSpecType::IF_DAY_REACHED:
			if(calc_day() >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_FIELDS:
			if(!isValidField(spec.m1, false)) {
				giveError("Scenario tried to check for invalid field type (1...24)");
				break;
			}
			i = 0;
			for(j = spec.ex1b; j < std::min(spec.ex2b, univ.town->max_dim()); j++)
				for(k = spec.ex1a; k < std::min(spec.ex2a, univ.town->max_dim()); k++) {
					switch(eFieldType(spec.m1)) {
						// These values are not allowed
						case SPECIAL_EXPLORED: case SPECIAL_SPOT: case FIELD_DISPEL: case FIELD_SMASH: break;
						// Walls
						case WALL_FIRE: i += univ.town.is_fire_wall(i,j); break;
						case WALL_FORCE: i += univ.town.is_force_wall(i,j); break;
						case WALL_ICE: i += univ.town.is_ice_wall(i,j); break;
						case WALL_BLADES: i += univ.town.is_blade_wall(i,j); break;
						// Clouds
						case CLOUD_STINK: i += univ.town.is_scloud(i,j); break;
						case CLOUD_SLEEP: i += univ.town.is_sleep_cloud(i,j); break;
						// Advanced
						case FIELD_QUICKFIRE: i += univ.town.is_quickfire(i,j); break;
						case FIELD_ANTIMAGIC: i += univ.town.is_antimagic(i,j); break;
						case BARRIER_FIRE: i += univ.town.is_fire_barr(i,j); break;
						case BARRIER_FORCE: i += univ.town.is_force_barr(i,j); break;
						case BARRIER_CAGE: i += univ.town.is_force_cage(i,j); break;
						// Objects
						case FIELD_WEB: i += univ.town.is_web(i,j); break;
						case OBJECT_BARREL: i += univ.town.is_barrel(i,j); break;
						case OBJECT_CRATE: i += univ.town.is_crate(i,j); break;
						case OBJECT_BLOCK: i += univ.town.is_block(i,j); break;
						// Sfx
						case SFX_SMALL_BLOOD: i += univ.town.is_sm_blood(i,j); break;
						case SFX_MEDIUM_BLOOD: i += univ.town.is_med_blood(i,j); break;
						case SFX_LARGE_BLOOD: i += univ.town.is_lg_blood(i,j); break;
						case SFX_SMALL_SLIME: i += univ.town.is_sm_slime(i,j); break;
						case SFX_LARGE_SLIME: i += univ.town.is_lg_slime(i,j); break;
						case SFX_ASH: i += univ.town.is_ash(i,j); break;
						case SFX_BONES: i += univ.town.is_bones(i,j); break;
						case SFX_RUBBLE: i += univ.town.is_rubble(i,j); break;
					}
				}
			if(i >= spec.sd1 && i <= spec.sd2)
				*next_spec = spec.m2;
			break;
		case eSpecType::IF_PARTY_SIZE:
			if(spec.ex2a < 1) {
				if(party_size(spec.ex2b) == spec.ex1a)
					*next_spec = spec.ex1b;
			}
			else {
				if(party_size(spec.ex2b) >= spec.ex1a)
					*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_EVENT_OCCURRED:
			if(day_reached(spec.ex1a,spec.ex1b))
				*next_spec = spec.ex2b;
			break;
		case eSpecType::IF_SPECIES:
			if(spec.ex1a < 0 || spec.ex1a > 2) {
				giveError("Species out of range (0-human, 1-nephilim, 2-slith)");
				break; // TODO: Should we allow monster races too?
			}
			i = 0;
			j = min(spec.ex2a,party_size(true));
			if(j < 1)
				j = 1;
			i = race_present(eRace(spec.ex1a));
			if(spec.ex2b == -2 && i <= j) *next_spec = spec.ex1b;
			if(spec.ex2b == -1 && i < j) *next_spec = spec.ex1b;
			if(spec.ex2b == 0 && i == j) *next_spec = spec.ex1b;
			if(spec.ex2b == 1 && i > j) *next_spec = spec.ex1b;
			if(spec.ex2b == 2 && i >= j) *next_spec = spec.ex1b;
			break;
		case eSpecType::IF_TRAIT:
			if(spec.ex1a < 0 || spec.ex1a > 15) {
				giveError("Invalid trait (0...15)");
				break;
			}
			j = min(spec.ex2a,party_size(true));
			if(j < 1)
				j = 1;
			i = trait_present((eTrait)spec.ex1a);
			if(spec.ex2b == -2 && i <= j) *next_spec = spec.ex1b;
			if(spec.ex2b == -1 && i < j) *next_spec = spec.ex1b;
			if(spec.ex2b == 0 && i == j) *next_spec = spec.ex1b;
			if(spec.ex2b == 1 && i > j) *next_spec = spec.ex1b;
			if(spec.ex2b == 2 && i >= j) *next_spec = spec.ex1b;
			break;
		case eSpecType::IF_STATISTIC:
			if((spec.ex2a < 0 || spec.ex2a > 20) && (spec.ex2a < 100 || spec.ex2a > 104)) {
				giveError("Attempted to check an invalid statistic (0...20 or 100...104).");
				break;
			}
			if(spec.ex2b < -1 || spec.ex2b > 3) {
				giveError("Invalid statistic-checking mode (-1...3); will fall back to cumulative check.");
				spec.ex2b = 0;
			}
			
			if(spec.ex2b == -1) {
				// Check specific PC's stat (uses the active PC from Select PC node)
				short pc = 6;
				if(univ.party.is_split())
					pc = univ.party.pc_present();
				if(pc == 6 && univ.party.pc_present(current_pc_picked_in_spec_enc))
					pc = current_pc_picked_in_spec_enc;
				if(pc != 6) {
					if(check_party_stat(eSkill(spec.ex2a), 10 + pc) >= spec.ex1a)
						*next_spec = spec.ex1b;
					break;
				}
			}
			if(check_party_stat(eSkill(spec.ex2a), spec.ex2b) >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_TEXT_RESPONSE:
			check_mess = false;
			get_strs(str1,str1,0,spec.m1,-1);
			str3 = get_text_response(str1);
			j = 1; k = 1;
			spec.pic = minmax(0,50,spec.pic);
			get_strs(str1,str2,0,spec.ex1a,spec.ex2a);
			for(i = 0; i < spec.pic;i++) {
				if((spec.ex1a < 0) || (str3[i] != str1[i]))
					j = 0;
				if((spec.ex2a < 0) || (str3[i] != str2[i]))
					k = 0;
			}
			if(j == 1)
				*next_spec = spec.ex1b;
			else if(k == 1)
				*next_spec = spec.ex2b;
			break;
		/* This is a little complicated.
			m1 - points to a prompt string
			m2,m3 - If nonequal, specifies a range of allowed responses.
			pic - Comparison mode: 0 - in range, 1 - not in range, 2 - simple compare
			pictype - Special to jump to if both tests pass
			ex1a, ex1b, ex1c - Test 1 values (see below)
			ex2a, ex2b, ex2c - Test 2 values (see below)
			Test values:
				If pic = 0 or 1:
					ex#a - Lower bound
					ex#b - Upper bound
					Enabled if ex#a < ex#b. If ex#a >= ex#b, this test is ignored.
				If pic = 2:
					ex#a - Value to compare to.
					ex#b - Set to 0 to enable. If -1, this test is ignored.
				ex#c - Special to jump to if test # succeeds but the other test fails.
			jumpto - Special to jump to if both tests fail.
		 */
		case eSpecType::IF_NUM_RESPONSE:
			check_mess = false;
			if(spec.m2 > spec.m3) std::swap(spec.m2,spec.m3);
			get_strs(str1,str1,0,spec.m1,-1);
			i = get_num_response(spec.m2,spec.m3,str1);
			setsd(spec.sd1, spec.sd2, abs(i));
			j = 0;
			spec.pic = minmax(0,2,spec.pic);
			switch(spec.pic) { // Comparison mode
				case 0: // Is in range?
					if(spec.ex1a < spec.ex1b && i == minmax(spec.ex1a,spec.ex1b,i)) j += 1;
					if(spec.ex2a < spec.ex2b && i == minmax(spec.ex2a,spec.ex2b,i)) j += 2;
					break;
				case 1: // Not in range?
					if(spec.ex1a < spec.ex1b && i != minmax(spec.ex1a,spec.ex1b,i)) j += 1;
					if(spec.ex2a < spec.ex2b && i != minmax(spec.ex2a,spec.ex2b,i)) j += 2;
					break;
				case 2: // Simple comparison?
					switch(spec.ex1b) {
						case -2: if(i <= spec.ex1a) j += 1; break;
						case -1: if(i < spec.ex1a) j += 1; break;
						case 0: if(i == spec.ex1a) j += 1; break;
						case 1: if(i > spec.ex1a) j += 1; break;
						case 2: if(i >= spec.ex1a) j += 1; break;
					}
					switch(spec.ex2b) {
						case -2: if(i <= spec.ex2a) j += 1; break;
						case -1: if(i < spec.ex2a) j += 1; break;
						case 0: if(i == spec.ex2a) j += 1; break;
						case 1: if(i > spec.ex2a) j += 1; break;
						case 2: if(i >= spec.ex2a) j += 1; break;
					}
					break;
			}
			if(j == 1) *next_spec = spec.ex1c;
			if(j == 2) *next_spec = spec.ex2c;
			if(j == 3) *next_spec = spec.pictype;
			break;
		case eSpecType::IF_SDF_EQ:
			if(sd_legit(spec.sd1,spec.sd2)) {
				if(PSD[spec.sd1][spec.sd2] == spec.ex1a)
					*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_ALIVE:
			i = 0;
			if(current_pc_picked_in_spec_enc < 100) {
				int pc = current_pc_picked_in_spec_enc;
				eMainStatus stat;
				switch(spec.ex1a) {
					case -1:
						stat = eMainStatus::ALIVE;
						break;
					case 0:
						stat = eMainStatus::DEAD;
						break;
					case 1:
						stat = eMainStatus::DUST;
						break;
					case 2:
						stat = eMainStatus::STONE;
						break;
					case 3:
						stat = eMainStatus::FLED;
						break;
					case 4:
						stat = eMainStatus::SPLIT;
						break;
					case 5:
						stat = eMainStatus::ABSENT;
						break;
				}
				if(stat == eMainStatus::SPLIT)
					i = univ.party.is_split();
				else for(j = 0; j < 6; j++)
					if(pc < 0 || pc == j)
						i += univ.party[j].main_status == stat;
			} else i = univ.town.monst[current_pc_picked_in_spec_enc - 100].active;
			if(i > 0) *next_spec = spec.ex1b;
			break;
		case eSpecType::IF_MAGE_SPELL:
			i = 0;
			if(current_pc_picked_in_spec_enc < 100) {
				int pc = current_pc_picked_in_spec_enc;
				if(spec.ex1a < 0 || spec.ex1a >= 62)
					break;
				for(j = 0; j < 6; j++)
					if(pc < 0 || pc == j)
						i += univ.party[j].mage_spells[spec.ex1a];
			} else {
				// TODO: Implement for monsters
				// Currently they have a fixed spell list depending solely on caster level
			}
			if(i > 0) *next_spec = spec.ex1b;
			break;
		case eSpecType::IF_PRIEST_SPELL:
			i = 0;
			if(current_pc_picked_in_spec_enc < 100) {
				int pc = current_pc_picked_in_spec_enc;
				if(spec.ex1a < 0 || spec.ex1a >= 62)
					break;
				for(j = 0; j < 6; j++)
					if(pc < 0 || pc == j)
						i += univ.party[j].priest_spells[spec.ex1a];
			} else {
				// TODO: Implement for monsters
				// Currently they have a fixed spell list depending solely on caster level
			}
			if(i > 0) *next_spec = spec.ex1b;
			break;
		case eSpecType::IF_RECIPE:
			if(i < 0 || i >= 20) {
				giveError("Alchemy recipe out of range (0 - 19).");
				break;
			}
			if(univ.party.alchemy[spec.ex1a])
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_STATUS:
			if(spec.ex1a < 0 || spec.ex1a > 14) {
				giveError("Invalid status effect (0...14)");
				break;
			}
			if(current_pc_picked_in_spec_enc < 0) {
				k = spec.ex2b == 2 ? std::numeric_limits<short>::max() : 0;
				j = 0;
				for(i = 0; i < 6; i++, j++)
					if(univ.party[i].main_status == eMainStatus::ALIVE) {
						eStatus stat = eStatus(spec.ex1a);
						if(spec.ex2b < 2)
							k += univ.party[i].status[stat];
						else if(spec.ex2b == 3)
							k = max(univ.party[i].status[stat], k);
						else if(spec.ex2b == 2)
							k = min(univ.party[i].status[stat], k);
					}
				if(spec.ex2b == 1 && j > 0)
					k /= j;
			} else if(current_pc_picked_in_spec_enc < 100) {
				cPlayer& pc = univ.party[current_pc_picked_in_spec_enc];
				if(pc.main_status == eMainStatus::ALIVE)
					k = pc.status[eStatus(spec.ex1a)];
				else k = 0;
			} else {
				cCreature& monst = univ.town.monst[current_pc_picked_in_spec_enc - 100];
				if(monst.active > 0)
					k = monst.status[eStatus(spec.ex1a)];
				else k = 0;
			}
			j = spec.ex2a;
			if(spec.ex2c == -2 && k <= j) *next_spec = spec.ex1b;
			if(spec.ex2c == -1 && k < j) *next_spec = spec.ex1b;
			if(spec.ex2c == 0 && k == j) *next_spec = spec.ex1b;
			if(spec.ex2c == 1 && k > j) *next_spec = spec.ex1b;
			if(spec.ex2c == 2 && k >= j) *next_spec = spec.ex1b;
			break;
		case eSpecType::IF_CONTEXT:
			// TODO: Test this. In particular, test that the legacy behaviour is correct.
			j = -1;
			switch(spec.ex1a) {
				case 0: // Out move
					if(is_out()) {
						j = bool(spec.ex1b); // Should block move? 1 = yes, 0 = no
						*next_spec = spec.ex1c;
						if(j && which_mode == eSpecCtx::OUT_MOVE)
							ASB("Can't go here while outdoors.");
					}
					break;
				case 1: // Town move
					if(is_town()) {
						j = bool(spec.ex1b); // Should block move? 1 = yes, 0 = no
						*next_spec = spec.ex1c;
						if(j && which_mode == eSpecCtx::TOWN_MOVE)
							ASB("Can't go here while in town mode.");
					}
					break;
				case 2: // Combat move
					if(is_combat()) {
						j = bool(spec.ex1b); // Should block move? 1 = yes, 0 = no
						*next_spec = spec.ex1c;
						if(j && which_mode == eSpecCtx::COMBAT_MOVE)
							ASB("Can't go here during combat.");
					}
					break;
				case 3: // Out look
					if(is_out()) {
						if(which_mode == eSpecCtx::OUT_LOOK)
							*next_spec = spec.ex1c;
					}
					break;
				case 4: // Town look
					if(is_town()) {
						if(which_mode == eSpecCtx::TOWN_LOOK)
							*next_spec = spec.ex1c;
					}
					break;
				case 5: // Enter town
					if(which_mode == eSpecCtx::ENTER_TOWN)
						*next_spec = spec.ex1c;
					break;
				case 6: // Leave town
					if(which_mode == eSpecCtx::LEAVE_TOWN)
						*next_spec = spec.ex1c;
					break;
				case 7: // Talking
					if(which_mode == eSpecCtx::TALK)
						*next_spec = spec.ex1c;
					break;
				case 8: // Use special item
					if(which_mode == eSpecCtx::USE_SPEC_ITEM)
						*next_spec = spec.ex1c;
					break;
				case 9: // Town timer
					if(which_mode == eSpecCtx::TOWN_TIMER)
						*next_spec = -1;
					break;
				case 10: // Scenario timer
					if(which_mode == eSpecCtx::SCEN_TIMER)
						*next_spec = spec.ex1c;
					else j = 0;
					break;
				case 11: // Party timer
					if(which_mode == eSpecCtx::PARTY_TIMER)
						*next_spec = spec.ex1c;
					break;
				case 12: // Kill monster
					if(which_mode == eSpecCtx::KILL_MONST)
						*next_spec = spec.ex1c;
					break;
				case 13: // Start outdoor encounter
					if(which_mode == eSpecCtx::OUTDOOR_ENC)
						*next_spec = spec.ex1c;
					break;
				case 14: // Flee outdoor encounter
					if(which_mode == eSpecCtx::FLEE_ENCOUNTER)
						*next_spec = spec.ex1c;
					break;
				case 15: // Win outdoor encounter
					if(which_mode == eSpecCtx::WIN_ENCOUNTER)
						*next_spec = spec.ex1c;
					break;
				case 16: // Target spell
					if(which_mode == eSpecCtx::TARGET) {
						// TODO: I'm not quite sure if this covers every way of determining which spell was cast
						if(spec.ex1b == -1 || (is_town() && int(town_spell) == spec.ex1b) || (is_combat() && int(spell_being_cast) == spec.ex1b))
							*next_spec = spec.ex1c;
					}
					break;
				case 17: // Use space
					if(which_mode == eSpecCtx::USE_SPACE)
						*next_spec = spec.ex1c;
					break;
				case 18: // See monster
					if(which_mode == eSpecCtx::SEE_MONST)
						*next_spec = spec.ex1c;
					break;
				case 19: // Monster using special ability
					if(which_mode == eSpecCtx::MONST_SPEC_ABIL)
						*next_spec = spec.ex1c;
					break;
				case 20: // Town goes hostile
					if(which_mode == eSpecCtx::TOWN_HOSTILE)
						*next_spec = spec.ex1c;
					break;
				case 21: // Item ability activated on attacking
					if(which_mode == eSpecCtx::ATTACKING_MELEE)
						*next_spec = spec.ex1c;
					break;
				case 22: // Item ability activated on attacking
					if(which_mode == eSpecCtx::ATTACKING_RANGE)
						*next_spec = spec.ex1c;
					break;
				case 23: // Item or monster ability activated on being hit
					if(which_mode == eSpecCtx::ATTACKED_MELEE)
						*next_spec = spec.ex1c;
					break;
				case 24: // Item or monster ability activated on being hit
					if(which_mode == eSpecCtx::ATTACKED_RANGE)
						*next_spec = spec.ex1c;
					break;
					// Past here are special values that don't have an equivalent in eSpecCtx.
				case 100: // Look (town or out)
					if(which_mode == eSpecCtx::OUT_LOOK || which_mode == eSpecCtx::TOWN_LOOK)
						*next_spec = spec.ex1c;
					break;
				case 101: // In boat
					if((spec.ex1b == -1 && univ.party.in_boat >= 0) || spec.ex1b == univ.party.in_boat)
						*next_spec = spec.ex1c;
					break;
				case 102: // On horse
					if((spec.ex1b == -1 && univ.party.in_horse >= 0) || spec.ex1b == univ.party.in_horse)
						*next_spec = spec.ex1c;
					break;
			}
			if(j >= 0) *a = j;
			break;
	}
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
}

void set_terrain(location l, ter_num_t terrain_type) {
	if(terrain_type >= univ.scenario.ter_types.size()) return;
	if(is_out()) {
		univ.out->terrain[l.x][l.y] = terrain_type;
		l = local_to_global(l);
		univ.out[l.x][l.y] = terrain_type;
	} else {
		ter_num_t former = univ.town->terrain(l.x,l.y);
		univ.town->terrain(l.x,l.y) = terrain_type;
		if(univ.scenario.ter_types[terrain_type].special == eTerSpec::CONVEYOR)
			belt_present = true;
		if(univ.scenario.ter_types[former].light_radius != univ.scenario.ter_types[terrain_type].light_radius)
			univ.town->set_up_lights();
	}
}

// TODO: What was next_spec_type for? Is it still needed?
void townmode_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
				   short *next_spec,short* /*next_spec_type*/,short *a,short *b,short *redraw) {
	static const char*const stairDlogs[8] = {
		"basic-stair-up", "basic-stair-down",
		"basic-slope-up", "basic-slope-down",
		"slimy-stair-up", "slimy-stair-down",
		"dark-slope-up", "dark-slope-down"
	};
	bool check_mess = true;
	std::array<std::string, 6> strs;
	short i,r1;
	std::array<short,3> buttons = {-1,-1,-1};
	cSpecial spec;
	location l;
	ter_num_t ter;
	cItem store_i;
	effect_pat_type pat;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	l.x = spec.ex1a; l.y = spec.ex1b;
	
	if(is_out())
		return;
	switch(cur_node.type) {
		case eSpecType::MAKE_TOWN_HOSTILE:
			set_town_attitude(spec.ex1a,spec.ex1b,spec.ex2a);
			break;
		case eSpecType::TOWN_MOVE_PARTY:
			if(is_combat()) {
				ASB("Not while in combat.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
			}
			else { // 1 no
				*a = 1;
				if(which_mode == eSpecCtx::TALK || spec.ex2a == 0)
					teleport_party(spec.ex1a,spec.ex1b,1);
				else teleport_party(spec.ex1a,spec.ex1b,0);
			}
			*redraw = 1;
			break;
		case eSpecType::TOWN_HIT_SPACE:
			if(which_mode == eSpecCtx::TALK)
				break;
			hit_space(l,spec.ex2a,(eDamageType) spec.ex2b,1,1);
			*redraw = 1;
			break;
		case eSpecType::TOWN_EXPLODE_SPACE:
			if(which_mode == eSpecCtx::TALK)
				break;
			radius_damage(l,spec.pic, spec.ex2a, (eDamageType) spec.ex2b);
			*redraw = 1;
			break;
		case eSpecType::TOWN_LOCK_SPACE:
			ter = coord_to_ter(spec.ex1a,spec.ex1b);
			if(univ.scenario.ter_types[ter].special == eTerSpec::LOCKABLE)
				set_terrain(l,univ.scenario.ter_types[ter].flag1.u);
			*redraw = 1;
			break;
		case eSpecType::TOWN_UNLOCK_SPACE:
			ter = coord_to_ter(spec.ex1a,spec.ex1b);
			if(univ.scenario.ter_types[ter].special == eTerSpec::UNLOCKABLE)
				set_terrain(l,univ.scenario.ter_types[ter].flag1.u);
			*redraw = 1;
			break;
		case eSpecType::TOWN_SFX_BURST: // TODO: Add a "random offset" mode
			if(which_mode == eSpecCtx::TALK)
				break;
			run_a_boom(l,spec.ex2a,0,0);
			break;
		case eSpecType::TOWN_CREATE_WANDERING:
			create_wand_monst();
			*redraw = 1;
			break;
		case eSpecType::TOWN_PLACE_MONST:
			if(spec.ex2a > 0)
				forced_place_monster(spec.ex2a,l);
			else place_monster(spec.ex2a,l);
			*redraw = 1;
			break;
		case eSpecType::TOWN_DESTROY_MONST:
			if(spec.ex1a >= 0 && spec.ex1b >= 0 && (i = monst_there(l)))
				univ.town.monst[i].active = 0;
			*redraw = 1;
			break;
		case eSpecType::TOWN_NUKE_MONSTS:
			for(i = 0; i < univ.town->max_monst(); i++)
				if((univ.town.monst[i].active > 0) &&
					(univ.town.monst[i].number == spec.ex1a || spec.ex1a == 0 ||
					 (spec.ex1a == -1 && univ.town.monst[i].attitude % 2 == 0) ||
					 (spec.ex1a == -2 && univ.town.monst[i].attitude % 2 == 1)){
						univ.town.monst[i].active = 0;
					}
			*redraw = 1;
			break;
		case eSpecType::TOWN_GENERIC_LEVER:
			if(which_mode != eSpecCtx::OUT_MOVE && which_mode != eSpecCtx::TOWN_MOVE && which_mode != eSpecCtx::COMBAT_MOVE
			   && which_mode != eSpecCtx::OUT_LOOK && which_mode != eSpecCtx::TOWN_LOOK) {
				ASB("Can't use lever now.");
				check_mess = false;
				*next_spec = -1;
			}
			else {
				if(handle_lever(store_special_loc) > 0)
					*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::TOWN_GENERIC_PORTAL:
			if(is_combat()) {
				ASB("Not while in combat.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
			}
			else if(which_mode != eSpecCtx::TOWN_MOVE && which_mode != eSpecCtx::TOWN_LOOK) {
				ASB("Can't teleport now.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
			}
			else if(cChoiceDlog("basic-portal",{"yes","no"}).show() == "yes") {
				*a = 1;
				if(which_mode == eSpecCtx::TALK)
					teleport_party(spec.ex1a,spec.ex1b,1);
				else teleport_party(spec.ex1a,spec.ex1b,spec.ex2a);
			}
			break;
		case eSpecType::TOWN_GENERIC_BUTTON:
			if(cChoiceDlog("basic-button",{"yes","no"}).show() == "yes")
				*next_spec = spec.ex1b;
			break;
		case eSpecType::TOWN_GENERIC_STAIR:
			if(spec.ex2c != 1 && spec.ex2c != 2 && is_combat()) {
				ASB("Can't change level in combat.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
			}
			else if(spec.ex2c != 2 && spec.ex2c != 3 && which_mode != eSpecCtx::TOWN_MOVE) {
				ASB("Can't change level now.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
			}
			else {
				// TODO: This is missing some additions that the non-generic stair has
				*a = 1;
				if(spec.ex2b < 0) spec.ex2b = 0;
				if((spec.ex2b >= 8) || (cChoiceDlog(stairDlogs[spec.ex2b],{"climb","leave"}).show() == "climb"))
					change_level(spec.ex2a,l.x,l.y);
			}
			break;
		case eSpecType::TOWN_LEVER:
			check_mess = false;
			if(spec.m1 < 0)
				break;
			if(which_mode != eSpecCtx::OUT_MOVE && which_mode != eSpecCtx::TOWN_MOVE && which_mode != eSpecCtx::COMBAT_MOVE
			   && which_mode != eSpecCtx::OUT_LOOK && which_mode != eSpecCtx::TOWN_LOOK) {
				ASB("Can't use lever now.");
				check_mess = false;
				*next_spec = -1;
			}
			else {
				for(i = 0; i < 3; i++)
					get_strs(strs[i * 2],strs[i * 2 + 1],cur_spec_type, spec.m1 + i * 2 ,spec.m1 + i * 2 + 1);
				buttons[0] = 9; buttons[1] = 35;
				i = custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons);
				if(i == 1) {*next_spec = -1;}
				else {
					ter = coord_to_ter(store_special_loc.x,store_special_loc.y);
					set_terrain(store_special_loc,univ.scenario.ter_types[ter].trans_to_what);
					*next_spec = spec.ex1b;
				}
			}
			break;
		case eSpecType::TOWN_PORTAL:
			check_mess = false;
			if(spec.m1 < 0)
				break;
			if(is_combat()) {
				ASB("Not while in combat.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
			}
			else if(which_mode != eSpecCtx::TOWN_MOVE && which_mode != eSpecCtx::TOWN_LOOK) {
				ASB("Can't teleport now.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
			}
			else {
				for(i = 0; i < 3; i++)
					get_strs(strs[i * 2],strs[i * 2 + 1], cur_spec_type,spec.m1 + i * 2, spec.m1 + i * 2 + 1);
				buttons[0] = 9; buttons[1] = 8;
				i = custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons);
				if(i == 1) {
					*next_spec = -1;
					if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
						*a = 1;
				}
				else {
					*a = 1;
					if(which_mode == eSpecCtx::TALK)
						teleport_party(spec.ex1a,spec.ex1b,1);
					else teleport_party(spec.ex1a,spec.ex1b,spec.ex2a);
				}
			}
			break;
		case eSpecType::TOWN_STAIR:
			check_mess = false;
			if((spec.m1 < 0) && (spec.ex2b != 1))
				break;
			if(spec.ex2c != 1 && spec.ex2c != 2 && is_combat()) {
				ASB("Can't change level in combat.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
			}
			else if(spec.ex2c != 2 && spec.ex2c != 3 && which_mode != eSpecCtx::TOWN_MOVE) {
				ASB("Can't change level now.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
			}
			else {
				if(spec.m1 >= 0) {
					for(i = 0; i < 3; i++)
						get_strs(strs[i * 2],strs[i * 2 + 1],cur_spec_type, spec.m1 + i * 2, spec.m1 + i * 2 + 1);
					buttons[0] = 20; buttons[1] = 24;
				}
				if(spec.ex2b == 1)
					i = 2;
				else i = custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons);
				*a = 1;
				if(i == 1) {
					*next_spec = -1;
					if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
						*a = 1;
				} else {
					bool was_in_combat = false;
					short was_active;
					if(overall_mode == MODE_TALKING)
						end_talk_mode();
					else if(is_combat()) {
						was_in_combat = true;
						if(which_combat_type == 0) { // outdoor combat
							ASB("Can't change level in combat.");
							if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
								*a = 1;
							*next_spec = -1;
							check_mess = false;
							break;
						} else {
							was_active = current_pc;
							univ.party.direction = end_town_combat();
						}
					}
					*a = 1;
					change_level(spec.ex2a,l.x,l.y);
					if(was_in_combat) {
						start_town_combat(univ.party.direction);
						current_pc = was_active;
					}
				}
			}
			break;
		case eSpecType::TOWN_RELOCATE:
			position_party(spec.ex1a,spec.ex1b,spec.ex2a,spec.ex2b);
			break;
		case eSpecType::TOWN_PLACE_ITEM:
			store_i = get_stored_item(spec.ex2a);
			place_item(store_i,l,true,spec.ex2b);
			break;
		case eSpecType::TOWN_SPLIT_PARTY:
			if(which_mode == eSpecCtx::TALK)
				break;
			if(is_combat()) {
				ASB("Not while in combat.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
				break;
			}
			if(univ.party.is_split()) {
				ASB("Party is already split.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
				check_mess = false;
				break;
			}
			r1 = char_select_pc(0,"Which character goes?");
			if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
				*a = 1;
			if(r1 != 6) {
				current_pc = r1;
				*next_spec = -1;
				if(!univ.party.start_split(spec.ex1a,spec.ex1b,spec.ex2a,r1))
					ASB("Party already split!");
				update_explored(univ.town.p_loc);
				center = univ.town.p_loc;
			}
			else check_mess = false;
			break;
		case eSpecType::TOWN_REUNITE_PARTY:
			if(is_combat()) {
				ASB("Not while in combat.");
				break;
			}
			if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
				*a = 1;
			*next_spec = -1;
			check_mess = false;
			if(univ.party.end_split(spec.ex1a))
				ASB("Party already together!");
			else ASB("You are reunited.");
			if(spec.ex2a); // This means reunite the party by bringing the others to the current location rather than the reverse.
			else if(univ.party.left_in == size_t(-1) || univ.town.num == univ.party.left_in) {
				univ.town.p_loc = univ.party.left_at;
				update_explored(univ.town.p_loc);
				center = univ.town.p_loc;
			} else change_level(univ.party.left_in, univ.party.left_at.x, univ.party.left_at.y);
			break;
		case eSpecType::TOWN_TIMER_START:
			univ.party.start_timer(spec.ex1a, spec.ex1b, 1);
			break;
			// OBoE: Change town lighting
		case eSpecType::TOWN_CHANGE_LIGHTING:
			// Change global town lighting
			if(spec.ex1a >= 0 && spec.ex1a <= 3) {
				univ.town->lighting_type = (eLighting) spec.ex1a;
				draw_terrain();
			}
			// Change party light level
			if(spec.ex2a > 0) {
				if(spec.ex2b == 0)
					increase_light(spec.ex2a);
				else increase_light(-spec.ex2a);
			}
			break;
		case eSpecType::TOWN_SET_ATTITUDE:
			if((spec.ex1a < 0) || (spec.ex1a > 59)){
				giveError("Tried to change the attitude of a nonexistent monster (should be 0...59).");
				break;
			}
			if((spec.ex1b < 0) || (spec.ex1b > 3)){
				giveError("Invalid attitude (0-Friendly Docile, 1-Hostile A, 2-Friendly Will Fight, 3-Hostile B).");
				break;
			}
			univ.town.monst[spec.ex1a].attitude = spec.ex1b;
			break;
		case eSpecType::TOWN_RUN_MISSILE:
			if(which_mode == eSpecCtx::TALK)
				break;
			if((i = monst_there(loc(spec.ex2a, spec.ex2b))) < 90) {
				cCreature& who = univ.town.monst[i];
				i = 14 * who.x_width - 1;
				r1 = 18 * who.y_width - 1;
			} else i = r1 = 0;
			run_a_missile(l, loc(spec.ex2a, spec.ex2b), spec.pic, spec.ex1c, spec.ex2c, i, r1, 100);
			break;
		case eSpecType::TOWN_BOOM_SPACE:
			// TODO: This should work, but does it need a bit of extra logic?
			if(which_mode == eSpecCtx::TALK)
				break;
			boom_space(l, 100, spec.ex2a, spec.ex2b, -spec.ex2c);
			break;
		case eSpecType::TOWN_MONST_ATTACK:
			// TODO: I'm not certain if this will work.
			if(which_mode == eSpecCtx::TALK)
				break;
			i = combat_posing_monster;
			if(l.y >= 0) {
				int monst = monst_there(l);
				if(monst < 90)
					combat_posing_monster = 100 + monst;
				else combat_posing_monster = pc_there(l);
				if(combat_posing_monster == 6)
					combat_posing_monster = -1;
			} else combat_posing_monster = spec.ex1a;
			if(combat_posing_monster < 0 || combat_posing_monster >= univ.town->max_monst()) {
				combat_posing_monster = i;
				break;
			}
			redraw_screen(REFRESH_TERRAIN);
			combat_posing_monster = i;
			break;
		case eSpecType::TOWN_SET_CENTER:
			if(l.x >= 0 && l.y >= 0) center = l;
			else center = is_combat() ? univ.party[current_pc].combat_pos : univ.town.p_loc;
			redraw_screen(REFRESH_TERRAIN);
			break;
		case eSpecType::TOWN_LIFT_FOG:
			fog_lifted = spec.ex1a;
			redraw_screen(REFRESH_TERRAIN);
			break;
		case eSpecType::TOWN_START_TARGETING:
			if(spec.ex1a < 0 || spec.ex1a > 7) {
				giveError("Invalid spell pattern (0 - 7).");
				break;
			}
			if(spec.ex1c > 1 && !is_combat()) {
				add_string_to_buf("  Target: Only in combat");
				break;
			}
			if(!is_combat())
				start_town_targeting(eSpell::NONE, spec.jumpto, true, eSpellPat(spec.ex1a));
			else if(spec.ex1c > 1)
				start_fancy_spell_targeting(eSpell::NONE, true, spec.ex1b, eSpellPat(spec.ex1a), spec.ex1c);
			else start_spell_targeting(eSpell::NONE, true, spec.ex1b, eSpellPat(spec.ex1a));
			spell_caster = spec.jumpto;
			*next_spec = -1;
			break;
		case eSpecType::TOWN_SPELL_PAT_FIELD:
			if(spec.ex1c < -1 || spec.ex1c > 14) {
				giveError("Invalid spell pattern (-1 - 14).");
				break;
			}
			if((spec.ex2a < 1 || spec.ex2a == 9 || spec.ex2a > 24) && spec.ex2a != 32 && spec.ex2a != 33) {
				giveError("Invalid field type (see docs).");
				break;
			}
			switch(spec.ex1c) {
				case -1: pat = current_pat; break;
				case PAT_SINGLE: pat = single; break;
				case PAT_SQ: pat = square; break;
				case PAT_SMSQ: pat = small_square; break;
				case PAT_OPENSQ: pat = open_square; break;
				case PAT_PLUS: pat = t; break;
				case PAT_RAD2: pat = radius2; break;
				case PAT_RAD3: pat = radius3; break;
				default: pat = field[spec.ex1c - 7];
			}
			place_spell_pattern(pat, l, eFieldType(spec.ex2a), 6);
			break;
		case eSpecType::TOWN_SPELL_PAT_BOOM:
			if(spec.ex1c < -1 || spec.ex1c > 14) {
				giveError("Invalid spell pattern (-1 - 14).");
				break;
			}
			if(spec.ex2a < 0 || spec.ex2a > 7) {
				giveError("Invalid damage type (0 - 7).");
				break;
			}
			switch(spec.ex1c) {
				case -1: pat = current_pat; break;
				case PAT_SINGLE: pat = single; break;
				case PAT_SQ: pat = square; break;
				case PAT_SMSQ: pat = small_square; break;
				case PAT_OPENSQ: pat = open_square; break;
				case PAT_PLUS: pat = t; break;
				case PAT_RAD2: pat = radius2; break;
				case PAT_RAD3: pat = radius3; break;
				default: pat = field[spec.ex1c - 7];
			}
			if(spec.ex2c) start_missile_anim();
			place_spell_pattern(pat, l, eDamageType(spec.ex2a), spec.ex2b, 6);
			if(spec.ex2c) {
				do_explosion_anim(0, 0);
				end_missile_anim();
			}
			break;
	}
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
}

// TODO: What was next_spec_type for? Is it still needed?
void rect_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
			   short *next_spec,short* /*next_spec_type*/,short *a,short *b,short *redraw){
	bool check_mess = true;
	short i,j,k;
	cSpecial spec;
	location l;
	ter_num_t ter;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	*redraw = 1;
	for(i = spec.ex1b;i <= spec.ex2b;i++)
		for(j = spec.ex1a; j <= spec.ex2a; j++) {
			l.x = i; l.y = j;
			// If pict non-zero, exclude rectangle interior
			if(spec.pic > 0 && i > spec.ex1b && i < spec.ex2b && j > spec.ex1a && j < spec.ex2a)
				continue;
			switch(cur_node.type) {
				case eSpecType::RECT_PLACE_FIELD:
					if(is_out())
						return;
					if(!isValidField(spec.sd2, true)) {
						giveError("Scenario tried to place an invalid field type (1...24)");
						goto END; // Break out of the switch AND both loops, but still handle messages
					}
					if(spec.sd2 == FIELD_DISPEL || get_ran(1,1,100) <= spec.sd1)
						switch(eFieldType(spec.sd2)) {
							// These values are not allowed.
							case SPECIAL_EXPLORED: case SPECIAL_SPOT: break;
							// Walls
							case WALL_FIRE: univ.town.set_fire_wall(i,j,true); break;
							case WALL_FORCE: univ.town.set_force_wall(i,j,true); break;
							case WALL_ICE: univ.town.set_ice_wall(i,j,true); break;
							case WALL_BLADES: univ.town.set_blade_wall(i,j,true); break;
							// Clouds
							case CLOUD_STINK: univ.town.set_scloud(i,j,true); break;
							case CLOUD_SLEEP: univ.town.set_sleep_cloud(i,j,true); break;
							// Advanced
							case FIELD_QUICKFIRE: univ.town.set_quickfire(i,j,true); break;
							case FIELD_ANTIMAGIC: univ.town.set_antimagic(i,j,true); break;
							case BARRIER_FIRE: univ.town.set_fire_barr(i,j,true); break;
							case BARRIER_FORCE: univ.town.set_force_barr(i,j,true); break;
							case BARRIER_CAGE: univ.town.set_force_cage(i,j,true); break;
							// Cleanse
							case FIELD_DISPEL:
								if(spec.sd1 == 0)
									dispel_fields(i,j,1);
								else dispel_fields(i,j,2);
								break;
							// Objects
							case FIELD_WEB: univ.town.set_web(i,j,true); break;
							case OBJECT_BARREL: univ.town.set_barrel(i,j,true); break;
							case OBJECT_CRATE: univ.town.set_crate(i,j,true); break;
							case OBJECT_BLOCK: univ.town.set_block(i,j,true); break;
							// Sfx
							case SFX_SMALL_BLOOD: univ.town.set_sm_blood(i,j,true); break;
							case SFX_MEDIUM_BLOOD: univ.town.set_med_blood(i,j,true); break;
							case SFX_LARGE_BLOOD: univ.town.set_lg_blood(i,j,true); break;
							case SFX_SMALL_SLIME: univ.town.set_sm_slime(i,j,true); break;
							case SFX_LARGE_SLIME: univ.town.set_lg_slime(i,j,true); break;
							case SFX_ASH: univ.town.set_ash(i,j,true); break;
							case SFX_BONES: univ.town.set_bones(i,j,true); break;
							case SFX_RUBBLE: univ.town.set_rubble(i,j,true); break;
							// Special value: Move Mountains!
							case FIELD_SMASH: crumble_wall(loc(i,j)); break;
						}
					break;
				case eSpecType::RECT_MOVE_ITEMS:
					if(is_out())
						return;
					i = is_container(loc(spec.sd1,spec.sd2));
					for(k = 0; k < NUM_TOWN_ITEMS; k++)
						if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].item_loc == l) {
							univ.town.items[k].item_loc.x = spec.sd1;
							univ.town.items[k].item_loc.y = spec.sd2;
							if(i && spec.m3)
								univ.town.items[k].contained = true;
						}
					break;
				case eSpecType::RECT_DESTROY_ITEMS:
					if(is_out())
						return;
					for(k = 0; k < NUM_TOWN_ITEMS; k++)
						if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].item_loc == l) {
							univ.town.items[k].variety = eItemType::NO_ITEM;
						}
					break;
				case eSpecType::RECT_CHANGE_TER:
					if(get_ran(1,1,100) <= spec.sd2){
						set_terrain(l,spec.sd1);
						*redraw = true;
						draw_map(true);
					}
					break;
				case eSpecType::RECT_SWAP_TER:
					if(coord_to_ter(i,j) == spec.sd1){
						set_terrain(l,spec.sd2);
						*redraw = true;
						draw_map(true);
					}
					else if(coord_to_ter(i,j) == spec.sd2){
						set_terrain(l,spec.sd1);
						*redraw = true;
						draw_map(true);
					}
					break;
				case eSpecType::RECT_TRANS_TER:
					ter = coord_to_ter(i,j);
					set_terrain(l,univ.scenario.ter_types[ter].trans_to_what);
					*redraw = true;
					draw_map(true);
					break;
				case eSpecType::RECT_LOCK:
					ter = coord_to_ter(i,j);
					if(univ.scenario.ter_types[ter].special == eTerSpec::LOCKABLE){
						set_terrain(l,univ.scenario.ter_types[ter].flag1.u);
						*redraw = true;
						draw_map(true);
					}
					break;
				case eSpecType::RECT_UNLOCK:
					ter = coord_to_ter(i,j);
					if(univ.scenario.ter_types[ter].special == eTerSpec::UNLOCKABLE){
						set_terrain(l,univ.scenario.ter_types[ter].flag1.u);
						*redraw = true;
						draw_map(true);
						break;
					}
				case eSpecType::RECT_SET_EXPLORED:
					if(spec.sd1)
						make_explored(l.x, l.y);
					else take_explored(l.x, l.y);
					break;
			}
		}
END:
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
}

// TODO: What was next_spec_type for? Is it still needed?
void outdoor_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
				  short *next_spec,short* /*next_spec_type*/,short *a,short *b,short *redraw){
	bool check_mess = false;
	std::string str1, str2;
	cSpecial spec;
	location l;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	if(!is_out()) return;
	
	switch(cur_node.type) {
		case eSpecType::OUT_MAKE_WANDER:
			create_wand_monst();
			*redraw = 1;
			break;
		case eSpecType::OUT_PLACE_ENCOUNTER:
			if(spec.ex1a != minmax(0,3,spec.ex1a)) {
				giveError("Special outdoor enc. is out of range. Must be 0-3.");
				//set_sd = false;
			}
			else {
				l = global_to_local(univ.party.p_loc);
				place_outd_wand_monst(l, univ.out->special_enc[spec.ex1a],true);
				check_mess = true;
			}
			break;
		case eSpecType::OUT_MOVE_PARTY:
			check_mess = true;
			out_move_party(spec.ex1a,spec.ex1b);
			*redraw = 1;
			*a = 1;
			break;
	}
	
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
}

void setsd(short a,short b,short val) {
	if(!sd_legit(a,b)) {
		giveError("The scenario attempted to change an out of range Stuff Done flag.");
		return;
	}
	PSD[a][b] = val;
}

void handle_message(eSpecCtx which_mode,short cur_type,short mess1,short mess2,short *a,short *b) {
	eEncNoteType note_type;
	switch(cur_type) {
		case 0:
			note_type = NOTE_SCEN;
			break;
		case 1:
			note_type = NOTE_OUT;
			break;
		case 2:
			note_type = NOTE_TOWN;
			break;
	}
	std::string str1, str2;
	short where1 = -1,where2 = -1;
	
	if((mess1 < 0) && (mess2 < 0))
		return;
	if(which_mode == eSpecCtx::TALK) {
		*a = mess1;
		*b = mess2;
		return;
	}
	get_strs(str1, str2, cur_type, mess1, mess2);
	where1 = is_out() ? univ.party.outdoor_corner.x + univ.party.i_w_c.x : univ.town.num;
	where2 = is_out() ? univ.party.outdoor_corner.y + univ.party.i_w_c.y : univ.town.num;
	std::string placename = is_out() ? univ.out->out_name : univ.town->town_name;
	cStrDlog display_strings(str1.c_str(), str2.c_str(),"",univ.scenario.intro_pic,PIC_SCEN,0);
	display_strings.setSound(57);
	display_strings.setRecordHandler(cStringRecorder(note_type).string1(mess1).string2(mess2).from(where1,where2).at(placename));
	display_strings.show();
}

void get_strs(std::string& str1,std::string& str2,short cur_type,short which_str1,short which_str2) {
	short num_strs[3] = {260,108,135};
	
	if(((which_str1 >= 0) && (which_str1 != minmax(0,num_strs[cur_type],which_str1))) ||
		((which_str2 >= 0) && (which_str2 != minmax(0,num_strs[cur_type],which_str2)))) {
		giveError("The scenario attempted to access a message out of range.");
		return;
	}
	switch(cur_type) {
		case 0:
			if(which_str1 >= 0)
				str1 = univ.scenario.spec_strs[which_str1];
			if(which_str2 >= 0)
				str2 = univ.scenario.spec_strs[which_str2];
			break;
		case 1:
			if(which_str1 >= 0)
				str1 = univ.out->spec_strs[which_str1];
			if(which_str2 >= 0)
				str2 = univ.out->spec_strs[which_str2];
			break;
		case 2:
			if(which_str1 >= 0)
				str1 = univ.town->spec_strs[which_str1];
			if(which_str2 >= 0)
				str2 = univ.town->spec_strs[which_str2];
			break;
	}
	
}

// This function sets/retrieves values to/from campaign flags
void set_campaign_flag(short sdf_a, short sdf_b, short cpf_a, short cpf_b, short str, bool get_send) {
	// get_send = false: Send value in SDF to Campaign Flag
	// get_send = true: Retrieve value from Campaign Flag and put in SDF
	try {
		if(str >= 0) {
			std::string cp_id = univ.scenario.spec_strs[str];
			if(get_send)
				univ.party.stuff_done[sdf_a][sdf_b] = univ.party.cpn_flag(cpf_a, cpf_b, cp_id);
			else
				univ.party.cpn_flag(cpf_a, cpf_b, cp_id) = univ.party.stuff_done[sdf_a][sdf_b];
		} else {
			if(get_send)
				univ.party.stuff_done[sdf_a][sdf_b] = univ.party.cpn_flag(cpf_a, cpf_b);
			else
				univ.party.cpn_flag(cpf_a, cpf_b) = univ.party.stuff_done[sdf_a][sdf_b];
		}
	} catch(std::range_error x) {
		giveError(x.what());
	}
}
