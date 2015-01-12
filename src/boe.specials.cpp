
#include <cstdio>
#include <cstring>
#include <queue>

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

extern sf::RenderWindow mainPtr;
extern eGameMode overall_mode;
extern short which_combat_type,current_pc,stat_window;
extern location center;
extern bool in_scen_debug,belt_present,processing_fields,monsters_going,suppress_stat_screen,boom_anim_active;
extern effect_pat_type current_pat;
extern cOutdoors::cWandering store_wandering_special;
extern short monst_marked_damage[60];
extern eSpell spell_being_cast, town_spell;
extern sf::RenderWindow mini_map;
extern short fast_bang;
extern bool end_scenario;
extern cUniverse univ;
extern std::queue<pending_special_type> special_queue;

bool can_draw_pcs = true;

short boom_gr[8] = {3,0,2,1,1,4,3,3};
short store_item_spell_level = 10;

// global values for when processing special encounters
short current_pc_picked_in_spec_enc = -1; // pc that's been selected, -1 if none
extern std::map<eSkill,short> skill_max;
location store_special_loc;
bool special_in_progress = false;

// 0 - can't use 1 - combat only 2 - town only 3 - town & combat only  4 - everywhere  5 - outdoor
// + 10 - mag. inept can use
std::map<eItemAbil, short> abil_chart = {
	{eItemAbil::POISON_WEAPON,13}, {eItemAbil::BLESS_CURSE,4}, {eItemAbil::AFFECT_POISON,4}, {eItemAbil::HASTE_SLOW,4},
	{eItemAbil::AFFECT_INVULN,3}, {eItemAbil::AFFECT_MAGIC_RES,3}, {eItemAbil::AFFECT_WEB,3}, {eItemAbil::AFFECT_DISEASE,4},
	{eItemAbil::AFFECT_SANCTUARY,3}, {eItemAbil::AFFECT_DUMBFOUND,3}, {eItemAbil::AFFECT_MARTYRS_SHIELD,3}, {eItemAbil::AFFECT_SLEEP,3},
	{eItemAbil::AFFECT_PARALYSIS,3}, {eItemAbil::AFFECT_ACID,3}, {eItemAbil::BLISS,3}, {eItemAbil::AFFECT_EXPERIENCE,4},
	{eItemAbil::AFFECT_SKILL_POINTS,4}, {eItemAbil::AFFECT_HEALTH,4}, {eItemAbil::AFFECT_SPELL_POINTS,4}, {eItemAbil::DOOM,3},
	{eItemAbil::LIGHT,13}, {eItemAbil::STEALTH,3}, {eItemAbil::FIREWALK,3}, {eItemAbil::FLYING,5}, {eItemAbil::MAJOR_HEALING,4},
	{eItemAbil::CALL_SPECIAL,4}, {eItemAbil::FLAME,1}, {eItemAbil::FIREBALL,1}, {eItemAbil::FIRESTORM,1}, {eItemAbil::KILL,1},
	{eItemAbil::ICE_BOLT,1}, {eItemAbil::SLOW,1}, {eItemAbil::SHOCKWAVE,1}, {eItemAbil::DISPEL_UNDEAD,1}, {eItemAbil::DISPEL_SPIRIT,1},
	{eItemAbil::SUMMONING,3}, {eItemAbil::MASS_SUMMONING,3}, {eItemAbil::ACID_SPRAY,1}, {eItemAbil::STINKING_CLOUD,1},
	{eItemAbil::SLEEP_FIELD,1}, {eItemAbil::VENOM,1}, {eItemAbil::SHOCKSTORM,1}, {eItemAbil::PARALYSIS,1}, {eItemAbil::WEB,1},
	{eItemAbil::STRENGTHEN_TARGET,1}, {eItemAbil::QUICKFIRE,3}, {eItemAbil::MASS_CHARM,1}, {eItemAbil::MAGIC_MAP,2},
	{eItemAbil::DISPEL_BARRIER,2}, {eItemAbil::ICE_WALL,1}, {eItemAbil::CHARM_SPELL,1}, {eItemAbil::ANTIMAGIC_CLOUD,1},
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
	eDamageType dam_type = DAMAGE_WEAPON;
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
				if((*spec_num >= 0) &&
					univ.out->specials[*spec_num].type == eSpecType::SECRET_PASSAGE)
					*forced = true;
				// call special
				run_special(mode,1,univ.out->special_id[i],out_where,&s1,&s2,&s3);
				if(s1 > 0)
					can_enter = false;
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
				if(univ.town->specials[univ.town->spec_id[i]].type == eSpecType::SECRET_PASSAGE) {
					*forced = true;
				}
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
			else dam_type = DAMAGE_WEAPON;
			r1 = get_ran(ter_flag2.u,dam_type,ter_flag1.u);
			switch(dam_type){
				case DAMAGE_FIRE:
					add_string_to_buf("  It's hot!");
					pic_type = 0;
					if(PSD[SDF_PARTY_FIREWALK] > 0) {
						add_string_to_buf("  It doesn't affect you.");
						r1 = -1;
					}
					break;
				case DAMAGE_COLD:
					add_string_to_buf("  You feel cold!");
					pic_type = 4;
					break;
				case DAMAGE_MAGIC:
				case DAMAGE_UNBLOCKABLE:
					add_string_to_buf("  Something shocks you!");
					pic_type = 1;
					break;
				case DAMAGE_WEAPON:
					add_string_to_buf("  You feel pain!");
					pic_type = 3;
					break;
				case DAMAGE_POISON:
					add_string_to_buf("  You suddenly feel very ill for a moment...");
					pic_type = 2;
					break;
				case DAMAGE_UNDEAD:
				case DAMAGE_DEMON:
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
								affect_pc(i,eStatus::INVULNERABLE,ter_flag1.u);
								break;
							case eStatus::MAGIC_RESISTANCE: // Should say "You feel odd." / "You feel protected."?
								affect_pc(i,eStatus::MAGIC_RESISTANCE,ter_flag1.u);
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
								affect_pc(i,eStatus::INVISIBLE,ter_flag1.s);
								break;
							case eStatus::DUMB: // Should say "You feel clearheaded." / "You feel confused."?
								dumbfound_pc(i,ter_flag1.u);
								break;
							case eStatus::MARTYRS_SHIELD: // Should say "You feel dull." / "You start to glow slightly."?
								affect_pc(i,eStatus::MARTYRS_SHIELD,ter_flag1.u);
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
			if((door_pc = select_pc(1,0)) < 6) {
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
			damage_pc(which_pc,r1,DAMAGE_FIRE,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,0,r1,5);
	}
	if(univ.town.is_force_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Force wall!               ");
		r1 = get_ran(2,1,6);
//		if(mode < 2)
//			hit_party(r1,3);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,DAMAGE_MAGIC,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,1,r1,12);
	}
	if(univ.town.is_ice_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Ice wall!               ");
		r1 = get_ran(2,1,6);
//		if(mode < 2)
//			hit_party(r1,5);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,DAMAGE_COLD,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,4,r1,7);
	}
	if(univ.town.is_blade_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Blade wall!               ");
		r1 = get_ran(4,1,8);
//		if(mode < 2)
//			hit_party(r1,0);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,DAMAGE_WEAPON,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.p_loc,overall_mode,3,r1,2);
	}
	if(univ.town.is_quickfire(where_check.x,where_check.y)) {
		add_string_to_buf("  Quickfire!               ");
		r1 = get_ran(2,1,8);
//		if(mode < 2)
//			hit_party(r1,1);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,DAMAGE_FIRE,eRace::UNKNOWN,0);
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
			hit_party(r1,DAMAGE_MAGIC);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,DAMAGE_MAGIC,eRace::UNKNOWN,0);
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
	eStatus which_stat;
	location user_loc;
	cCreature *which_m;
	extern effect_pat_type single;
	eItemAbil abil = univ.party[pc].items[item].ability;
	level = univ.party[pc].items[item].item_level;
	
	item_use_code = abil_chart[abil];
	if(item_use_code >= 10) {
		item_use_code -= 10;
		inept_ok = true;
	}
	
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
		
		str = univ.party[pc].items[item].ability_strength;
		store_item_spell_level = str * 2 + 1;
		type = univ.party[pc].items[item].magic_use_type;
		
		switch(abil) {
			case eItemAbil::POISON_WEAPON: // poison weapon
				take_charge = poison_weapon(pc,str,0);
				break;
			case eItemAbil::BLESS_CURSE:
				play_sound(4);
				which_stat = eStatus::BLESS_CURSE;
				if(type % 2 == 1) {
					ASB("  You feel awkward.");
					str = str * -1;
				}else ASB("  You feel blessed.");
				if(type > 1)
					affect_party(which_stat,str);
				else affect_pc(pc,which_stat,str);
				break;
			case eItemAbil::HASTE_SLOW:
				// TODO: Is this the right sound?
				play_sound(75);
				which_stat = eStatus::HASTE_SLOW;
				if(type % 2 == 1) {
					ASB("  You feel sluggish.");
					str = str * -1;
				}else ASB("  You feel speedy.");
				if(type > 1)
					affect_party(which_stat,str);
				else affect_pc(pc,which_stat,str);
				break;
			case eItemAbil::AFFECT_INVULN:
				// TODO: Is this the right sound?
				play_sound(68);
				which_stat = eStatus::INVULNERABLE;
				if(type % 2 == 1) {
					ASB("  You feel odd.");
					str = str * -1;
				}else ASB("  You feel protected.");
				if(type > 1)
					affect_party(which_stat,str);
				else affect_pc(pc,which_stat,str);
				break;
			case eItemAbil::AFFECT_MAGIC_RES:
				// TODO: Is this the right sound?
				play_sound(51);
				which_stat = eStatus::MAGIC_RESISTANCE;
				if(type % 2 == 1) {
					ASB("  You feel odd.");
					str = str * -1;
				}else ASB("  You feel protected.");
				if(type > 1)
					affect_party(which_stat,str);
				else affect_pc(pc,which_stat,str);
				break;
			case eItemAbil::AFFECT_WEB:
				which_stat = eStatus::WEBS;
				if(type % 2 == 1)
					ASB("  You feel sticky.");
				else {
					ASB("  Your skin tingles.");
					str = str * -1;
				}
				if(type > 1)
					affect_party(which_stat,str);
				else affect_pc(pc,which_stat,str);
				break;
			case eItemAbil::AFFECT_SANCTUARY:
				// TODO: Is this the right sound?
				play_sound(43);
				which_stat = eStatus::INVISIBLE;
				if(type % 2 == 1) {
					ASB("  You feel exposed.");
					str = str * -1;
				}else ASB("  You feel obscure.");
				if(type > 1)
					affect_party(which_stat,str);
				else affect_pc(pc,which_stat,str);
				break;
			case eItemAbil::AFFECT_MARTYRS_SHIELD:
				// TODO: Is this the right sound?
				play_sound(43);
				which_stat = eStatus::MARTYRS_SHIELD;
				if(type % 2 == 1) {
					ASB("  You feel dull.");
					str = str * -1;
				}else ASB("  You start to glow slightly.");
				if(type > 1)
					affect_party(which_stat,str);
				else affect_pc(pc,which_stat,str);
				break;
			case eItemAbil::AFFECT_POISON:
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
			case eItemAbil::AFFECT_DISEASE:
				switch(type) {
					case 0:
						ASB("  You feel healthy.");
						affect_pc(pc,eStatus::DISEASE,-1 * str);
						break;
					case 1:
						ASB("  You feel sick.");
						disease_pc(pc,str);
						break;
					case 2:
						ASB("  You all feel healthy.");
						affect_party(eStatus::DISEASE,-1 * str);
						break;
					case 3:
						ASB("  You all feel sick.");
						for(i = 0; i < 6; i++)
							disease_pc(i,str);
						break;
				}
				break;
			case eItemAbil::AFFECT_DUMBFOUND:
				switch(type) {
					case 0:
						ASB("  You feel clear headed.");
						affect_pc(pc,eStatus::DUMB,-1 * str);
						break;
					case 1:
						ASB("  You feel confused.");
						dumbfound_pc(pc,str);
						break;
					case 2:
						ASB("  You all feel clear headed.");
						affect_party(eStatus::DUMB,-1 * str);
						break;
					case 3:
						ASB("  You all feel confused.");
						for(i = 0; i < 6; i++)
							dumbfound_pc(i,str);
						break;
				}
				break;
			case eItemAbil::AFFECT_SLEEP:
				switch(type) {
					case 0:
						ASB("  You feel alert.");
						affect_pc(pc,eStatus::ASLEEP,-1 * str);
						break;
					case 1:
						ASB("  You feel very tired.");
						sleep_pc(pc,str + 1,eStatus::ASLEEP,200);
						break;
					case 2:
						ASB("  You all feel alert.");
						affect_party(eStatus::ASLEEP,-1 * str);
						break;
					case 3:
						ASB("  You all feel very tired.");
						for(i = 0; i < 6; i++)
							sleep_pc(i,str + 1,eStatus::ASLEEP,200);
						break;
				}
				break;
			case eItemAbil::AFFECT_PARALYSIS:
				switch(type) {
					case 0:
						ASB("  You find it easier to move.");
						affect_pc(pc,eStatus::PARALYZED,-1 * str * 100);
						break;
					case 1:
						ASB("  You feel very stiff.");
						sleep_pc(pc,str * 20 + 10,eStatus::PARALYZED,200);
						break;
					case 2:
						ASB("  You all find it easier to move.");
						affect_party(eStatus::PARALYZED,-1 * str * 100);
						break;
					case 3:
						ASB("  You all feel very stiff.");
						for(i = 0; i < 6; i++)
							sleep_pc(i,str * 20 + 10,eStatus::PARALYZED,200);
						break;
				}
				break;
			case eItemAbil::AFFECT_ACID:
				switch(type) {
					case 0:
						ASB("  Your skin tingles pleasantly.");
						affect_pc(pc,eStatus::ACID,-1 * str);
						break;
					case 1:
						ASB("  Your skin burns!");
						acid_pc(pc,str);
						break;
					case 2:
						ASB("  You all tingle pleasantly.");
						affect_party(eStatus::ACID,-1 * str);
						break;
					case 3:
						ASB("  Everyone's skin burns!");
						for(i = 0; i < 6; i++)
							acid_pc(i,str);
						break;
				}
				break;
			case eItemAbil::BLISS:
				switch(type) {
					case 0: case 1:
						ASB("  You feel wonderful!");
						heal_pc(pc,str * 20);
						affect_pc(pc,eStatus::BLESS_CURSE,str);
						break;
					case 2: case 3:
						ASB("  Everyone feels wonderful!");
						for(i = 0; i < 6; i++) {
							heal_pc(i,str * 20);
							affect_pc(i,eStatus::BLESS_CURSE,str);
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
						damage_pc(pc,20 * str,DAMAGE_UNBLOCKABLE,eRace::HUMAN,0);
						break;
					case 2:
						ASB("  You all feel better.");
						heal_party(str * 20);
						break;
					case 3:
						ASB("  You all feel sick.");
						hit_party(20 * str,DAMAGE_UNBLOCKABLE);
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
			case eItemAbil::DOOM:
				switch(type) {
					case 0: case 1:
						ASB("  You feel terrible.");
						drain_pc(pc,str * 5);
						damage_pc(pc,20 * str,DAMAGE_UNBLOCKABLE,eRace::HUMAN,0);
						disease_pc(pc,2 * str);
						dumbfound_pc(pc,2 * str);
						break;
					case 2: case 3:
						ASB("  You all feel terrible.");
						for(i = 0; i < 6; i++) {
							drain_pc(i,str * 5);
							damage_pc(i,20 * str,DAMAGE_UNBLOCKABLE,eRace::HUMAN,0);
							disease_pc(i,2 * str);
							dumbfound_pc(i,2 * str);
						}
						break;
				}
				break;
			case eItemAbil::LIGHT:
				ASB("  You have more light.");
				increase_light(50 * str);
				break;
			case eItemAbil::STEALTH:
				ASB("  Your footsteps become quieter.");
				PSD[SDF_PARTY_STEALTHY] += 5 * str;
				break;
			case eItemAbil::FIREWALK:
				ASB("  You feel chilly.");
				PSD[SDF_PARTY_FIREWALK] += 2 * str;
				break;
			case eItemAbil::FLYING:
				if(PSD[SDF_PARTY_FLIGHT] > 0) {
					add_string_to_buf("  Not while already flying.          ");
					take_charge = false;
					break;
				}
				if(univ.party.in_boat >= 0) {
					add_string_to_buf("  Leave boat first.             ");
					take_charge = false;
				} else if(univ.party.in_horse >= 0) {
					add_string_to_buf("  Leave horse first.             ");
					take_charge = false;
				} else {
					ASB("  You rise into the air!");
					PSD[SDF_PARTY_FLIGHT] += str;
				}
				break;
			case eItemAbil::MAJOR_HEALING:
				switch(type) {
					case 0: case 1:
						ASB("  You feel wonderful.");
						heal_pc(pc,200);
						cure_pc(pc,8);
						break;
					case 2: case 3:
						ASB("  You all feel wonderful.");
						heal_party(200);
						cure_party(8);
						break;
				}
				break;
			case eItemAbil::CALL_SPECIAL:
				// TODO: Should this have its own separate eSpecCtx?
				run_special(eSpecCtx::USE_SPEC_ITEM,0,str,user_loc,&sp[0],&sp[1],&sp[2]);
				break;
				
				
				// spell effects
			case eItemAbil::FLAME:
				add_string_to_buf("  It fires a bolt of flame.");
				start_spell_targeting(eSpell::FLAME, true);
				break;
			case eItemAbil::FIREBALL:
				add_string_to_buf("  It shoots a fireball.         ");
				start_spell_targeting(eSpell::FIREBALL, true);
				break;
			case eItemAbil::FIRESTORM:
				add_string_to_buf("  It shoots a huge fireball. ");
				start_spell_targeting(eSpell::FIRESTORM, true);
				break;
			case eItemAbil::KILL:
				add_string_to_buf("  It shoots a black ray.  ");
				start_spell_targeting(eSpell::KILL, true);
				break;
			case eItemAbil::ICE_BOLT:
				add_string_to_buf("  It fires a ball of ice.   ");
				start_spell_targeting(eSpell::ICE_BOLT, true);
				break;
			case eItemAbil::SLOW:
				add_string_to_buf("  It fires a purple ray.   ");
				start_spell_targeting(eSpell::SLOW, true);
				break;
			case eItemAbil::SHOCKWAVE:
				add_string_to_buf("  The ground shakes!        ");
				do_shockwave(univ.party[current_pc].combat_pos);
				break;
			case eItemAbil::DISPEL_UNDEAD:
				add_string_to_buf("  It shoots a white ray.   ");
				start_spell_targeting(eSpell::DISPEL_UNDEAD, true);
				break;
			case eItemAbil::DISPEL_SPIRIT:
				add_string_to_buf("  It shoots a golden ray.   ");
				start_spell_targeting(eSpell::RAVAGE_SPIRIT, true);
				break;
			case eItemAbil::SUMMONING:
				if(!summon_monster(str,user_loc,50,2))
					add_string_to_buf("  Summon failed.");
				break;
			case eItemAbil::MASS_SUMMONING:
				r1 = get_ran(6,1,4);
				for(i = 0; i < get_ran(1,3,5); i++) // TODO: Why recalculate the random number for each loop iteration?
					if(!summon_monster(str,user_loc,r1,2))
						add_string_to_buf("  Summon failed.");
				break;
			case eItemAbil::ACID_SPRAY:
				add_string_to_buf("  Acid sprays from the tip!   ");
				start_spell_targeting(eSpell::ACID_SPRAY,  true);
				break;
			case eItemAbil::STINKING_CLOUD:
				add_string_to_buf("  It creates a cloud of gas.   ");
				start_spell_targeting(eSpell::FOUL_VAPOR, true);
				break;
			case eItemAbil::SLEEP_FIELD:
				add_string_to_buf("  It creates a shimmering cloud.   ");
				start_spell_targeting(eSpell::CLOUD_SLEEP, true);
				break;
			case eItemAbil::VENOM:
				add_string_to_buf("  A green ray emerges.        ");
				start_spell_targeting(eSpell::POISON, true);
				break;
			case eItemAbil::SHOCKSTORM:
				add_string_to_buf("  Sparks fly.");
				start_spell_targeting(eSpell::SHOCKSTORM, true);
				break;
			case eItemAbil::PARALYSIS:
				add_string_to_buf("  It shoots a silvery beam.   ");
				start_spell_targeting(eSpell::PARALYZE_BEAM, true);
				break;
			case eItemAbil::WEB:
				add_string_to_buf("  It explodes!");
				start_spell_targeting(eSpell::GOO_BOMB, true);
				break;
			case eItemAbil::STRENGTHEN_TARGET:
				add_string_to_buf("  It shoots a fiery red ray.   ");
				start_spell_targeting(eSpell::STRENGTHEN_TARGET, true);
				break;
			case eItemAbil::QUICKFIRE:
				add_string_to_buf("Fire pours out!");
				univ.town.set_quickfire(user_loc.x,user_loc.y,true);
				break;
			case eItemAbil::MASS_CHARM:
				ASB("It throbs, and emits odd rays.");
				for(i = 0; i < univ.town->max_monst(); i++) {
					if((univ.town.monst[i].active != 0) && (univ.town.monst[i].attitude % 2 == 1)
						&& (dist(univ.party[current_pc].combat_pos,univ.town.monst[i].cur_loc) <= 8)
						&& (can_see_light(univ.party[current_pc].combat_pos,univ.town.monst[i].cur_loc,sight_obscurity) < 5)) {
						which_m = &univ.town.monst[i];
						charm_monst(which_m,0,eStatus::CHARM,8);
					}
				}
				break;
			case eItemAbil::MAGIC_MAP:
				if(univ.town->defy_scrying || univ.town->defy_mapping) {
					add_string_to_buf("  It doesn't work.");
					break;
				}
				add_string_to_buf("  You have a vision.            ");
				for(i = 0; i < univ.town->max_dim(); i++)
					for(j = 0; j < univ.town->max_dim(); j++)
						make_explored(i,j);
				clear_map();
				break;
			case eItemAbil::DISPEL_BARRIER:
				add_string_to_buf("  It fires a blinding ray.");
				add_string_to_buf("  Target spell.    ");
				current_pat = single;
				start_town_targeting(eSpell::DISPEL_BARRIER,current_pc, true);
				break;
			case eItemAbil::ICE_WALL:
				add_string_to_buf("  It shoots a blue sphere.   ");
				start_spell_targeting(eSpell::WALL_ICE_BALL, true);
				break;
			case eItemAbil::CHARM_SPELL:
				add_string_to_buf("  It fires a lovely, sparkling beam.");
				start_spell_targeting(eSpell::CHARM_FOE, true);
				break;
			case eItemAbil::ANTIMAGIC_CLOUD:
				add_string_to_buf("  Your hair stands on end.   ");
				start_spell_targeting(eSpell::ANTIMAGIC, true);
				break;
		}
	}
	
	put_pc_screen();
	if((take_charge) && (univ.party[pc].items[item].charges > 0))
		remove_charge(pc,item);
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
	
	if((town_num < 0) || (town_num >= univ.scenario.num_towns)) {
		giveError("The scenario special encounter tried to put you into a town that doesn't exist.");
		return;
	}
	
	force_town_enter(town_num,l);
	end_town_mode(1,l);
	start_town_mode(town_num,9);
}


// Damaging and killing monsters needs to be here because several have specials attached to them.
//short which_m, who_hit, how_much, how_much_spec;  // 6 for who_hit means dist. xp evenly  7 for no xp
//short dam_type;  // 0 - weapon   1 - fire   2 - poison   3 - general magic   4 - unblockable  5 - cold
// 6 - demon 7 - undead
// 9 - marked damage, from during anim mode
//+10 = no_print
// 100s digit - damage sound for boom space
bool damage_monst(short which_m, short who_hit, short how_much, short how_much_spec, eDamageType dam_type, short sound_type) {
	cCreature *victim;
	short r1,which_spot;
	location where_put;
	
	bool do_print = true;
	char resist;
	
	//print_num(which_m,(short)univ.town.monst[which_m].m_loc.x,(short)univ.town.monst[which_m].m_loc.y);
	
	if(univ.town.monst[which_m].active == 0) return false;
	
	if(dam_type >= DAMAGE_MARKED) { // note: MARKED here actually means NO_PRINT
		do_print = false;
		dam_type -= DAMAGE_MARKED;
	}
	
	if(sound_type == 0) {
		if((dam_type == 1) || (dam_type == 4) )
			sound_type = 5;
		if 	(dam_type == 5)
			sound_type = 7;
		if 	(dam_type == 3)
			sound_type = 12;
		if 	(dam_type == 2)
			sound_type = 11;
	}
	
	
	victim = &univ.town.monst[which_m];
	resist = victim->immunities;
	
	if(dam_type == 3) {
		if(resist & 1)
			how_much = how_much / 2;
		if(resist & 2)
			how_much = 0;
	}
	if(dam_type == 1) {
		if(resist & 4)
			how_much = how_much / 2;
		if(resist & 8)
			how_much = 0;
	}
	if(dam_type == 5) {
		if(resist & 16)
			how_much = how_much / 2;
		if(resist & 32)
			how_much = 0;
	}
	if(dam_type == 2) {
		if(resist & 64)
			how_much = how_much / 2;
		if(resist & 128)
			how_much = 0;
	}
	
	// Absorb damage?
	if(((dam_type == 1) || (dam_type == 3) || (dam_type == 5))
		&& (victim->spec_skill == 26)) {
		if(32767 - victim->health > how_much)
			victim->health = 32767;
		else victim->health += how_much;
		ASB("  Magic absorbed.");
		return false;
	}
	
	// Saving throw
	if(((dam_type == 1) || (dam_type == 5)) && (get_ran(1,0,20) <= victim->level))
		how_much /= 2;
	if((dam_type == 3) && (get_ran(1,0,24) <= victim->level))
		how_much /= 2;
	
	// Invulnerable?
	if(victim->spec_skill == 36)
		how_much = how_much / 10;
	
	
	r1 = get_ran(1,0,(victim->armor * 5) / 4);
	r1 += victim->level / 4;
	if(dam_type == 0)
		how_much -= r1;
	
	if(boom_anim_active) {
		if(how_much < 0)
			how_much = 0;
		monst_marked_damage[which_m] += how_much;
		add_explosion(victim->cur_loc,how_much,0,(dam_type > 2) ? 2 : 0,14 * (victim->x_width - 1),18 * (victim->y_width - 1));
		// Note: Windows version printed an "undamaged" message here if applicable, but I don't think that's right.
		if(how_much == 0)
			return false;
		else return true;
	}
	
	if(how_much <= 0) {
		if(is_combat())
			monst_spell_note(victim->number,7);
		if((how_much <= 0) && ((dam_type == DAMAGE_WEAPON) || (dam_type == DAMAGE_UNDEAD) || (dam_type == DAMAGE_DEMON))) {
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
	if((victim->spec_skill == 12) && (victim->health > 0)){
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
				univ.town.monst[which_spot].extra1 = victim->extra1;
				univ.town.monst[which_spot].extra2 = victim->extra2;
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
	if(dam_type != 9) { // note special damage only gamed in hand-to-hand, not during animation
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
	
	if(r1 > 14 || m_target->immunities & 2)
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
			// TODO: Should sliths be considered reptiles too? Check original bladbase.
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
	if(which_m->radiate_1 == 15)
		run_special(eSpecCtx::KILL_MONST,0,which_m->radiate_2,which_m->cur_loc,&s1,&s2,&s3);
	
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
				hit_party(get_ran(1, 1, 6), DAMAGE_WEAPON);
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
						damage_pc(i,get_ran(1, 1, 6), DAMAGE_WEAPON,eRace::UNKNOWN,0);
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
				else *a = 0;
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
			play_sound(spec.ex1a);
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
			if(spec.ex1a != minmax(0,univ.scenario.num_towns - 1,spec.ex1a))
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
			if(!forced_give(spec.ex1a,eItemAbil::NONE) && spec.ex1b >= 0)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::BUY_ITEMS_OF_TYPE:
			for(i = 0; i < 144; i++)
				if(party_check_class(spec.ex1a,0))
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
				  short *next_spec,short* /*next_spec_type*/,short *a,short *b,short *redraw) {
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
			if(!forced_give(spec.ex1a,eItemAbil::NONE)) {
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
				univ.party.spec_items[spec.ex1a] = (spec.ex1b == 0) ? 1 : 0;
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
				if((spec.ex1a >= 0) && (!give_to_party(store_i,true))) {
					set_sd = false; *next_spec = -1;
				}
				else {
					give_gold(spec.ex1b,true);
					give_food(spec.ex2a,true);
					if((spec.m3 >= 0) && (spec.m3 < 50)) {
						if(univ.party.spec_items[spec.m3] == 0)
							ASB("You get a special item.");
						univ.party.spec_items[spec.m3] = 1;
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
				// TODO: Why not allow a choice of dialog picture?
				i = custom_choice_dialog(strs,27,PIC_DLOG,buttons);
				// TODO: Make custom_choice_dialog return string?
			}
			else i = cChoiceDlog("basic-trap",{"yes","no"}).show() == "no";
			if(i == 1) {set_sd = false; *next_spec = -1; *a = 1;}
			else {
				if(is_combat())
					j = run_trap(current_pc,(eTrapType)spec.ex1a,spec.ex1b,spec.ex2a);
				else j = run_trap(7,(eTrapType)spec.ex1a,spec.ex1b,spec.ex2a);
				if(j == 0) {
					*a = 1; set_sd = false;
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
	short i,pc = 6,r1;
	cSpecial spec;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	if(univ.party.is_split() && cur_node.type != eSpecType::AFFECT_DEADNESS)
		pc = univ.party.pc_present();
	if(pc == 6 && univ.party.pc_present(current_pc_picked_in_spec_enc))
		pc = current_pc_picked_in_spec_enc;
	if(pc == 6) pc = -1;
	
	switch(cur_node.type) {
		case eSpecType::SELECT_PC:
			check_mess = false;
			// If this <= 0, pick PC normally
			// TODO: I think this is for compatibility with old scenarios? If so, remove it and just convert data on load.
			// (Actually, I think the only compatibility thing is that it's <= instead of ==)
			if(spec.ex2a <= 0) {
				
				if(spec.ex1a == 2)
					current_pc_picked_in_spec_enc = -1;
				else if(spec.ex1a == 1) {
					i = select_pc(0,0);
					if(i != 6)
						current_pc_picked_in_spec_enc = i;
				}
				else if(spec.ex1a == 0) {
					i = select_pc(1,0);
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
				if(spec.ex1a == 1 || univ.party[pc].main_status == eMainStatus::ALIVE)
					current_pc_picked_in_spec_enc = pc;
				// TODO: If >= 100, select a specific monster
			} else {
				// Pick random PC (from *i)
				// TODO: What if spec.ex1a == 2?
				
				if(spec.ex1a == 0) {
					short pc_alive = 0;
					while(pc_alive == 0) {
						i = get_ran(1,0,5);
						if(univ.party[i].main_status == eMainStatus::ALIVE)
							pc_alive = 1;
					}
					current_pc_picked_in_spec_enc = i;
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
			if(pc < 0) {
				if(spec.pic == 1 && overall_mode == MODE_COMBAT)
					damage_pc(current_pc,r1,dam_type,eRace::UNKNOWN,0); // was HUMAN
				else hit_party(r1,dam_type);
			}
			else damage_pc(pc,r1,dam_type,eRace::UNKNOWN,0);
			// TODO: Extend to affect monsters too
			break;
		}
		case eSpecType::AFFECT_HP:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i))
						univ.party[i].cur_health = minmax(0,univ.party[i].max_health,
														  univ.party[i].cur_health + spec.ex1a * (spec.ex1b ? -1: 1));
			}
			else {
				univ.town.monst[spec.ex2a].health = minmax(0, univ.town.monst[spec.ex2a].m_health,
					univ.town.monst[spec.ex2a].health + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
				if(spec.ex1b == 0)
					monst_spell_note(univ.town.monst[spec.ex2a].number,41);
				else
					monst_spell_note(univ.town.monst[spec.ex2a].number,42);
			}
			break;
		case eSpecType::AFFECT_SP:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i))
						univ.party[i].cur_sp = minmax(0, univ.party[i].max_sp,
							univ.party[i].cur_sp + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			}
			else {
				univ.town.monst[spec.ex2a].mp = minmax(0, univ.town.monst[spec.ex2a].max_mp,
					univ.town.monst[spec.ex2a].mp + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
				if(spec.ex1b == 0)
					monst_spell_note(univ.town.monst[spec.ex2a].number,43);
				else
					monst_spell_note(univ.town.monst[spec.ex2a].number,44);
			}
			break;
		case eSpecType::AFFECT_XP:
			for(i = 0; i < 6; i++)
				if((pc < 0) || (pc == i)) {
					if(spec.ex1b == 0) award_xp(i,spec.ex1a); else drain_pc(i,spec.ex1a);
				}
			break;
		case eSpecType::AFFECT_SKILL_PTS:
			for(i = 0; i < 6; i++)
				if((pc < 0) || (pc == i))
					univ.party[i].skill_pts = minmax(0,	100,
						univ.party[i].skill_pts + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case eSpecType::AFFECT_DEADNESS:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i)) {
						if(spec.ex1b == 0) {
							if((univ.party[i].main_status > eMainStatus::ABSENT) && (univ.party[i].main_status < eMainStatus::SPLIT))
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
						}
					}
				*redraw = 1;
			} else {
				// Kill monster
				if(univ.town.monst[spec.ex2a].active > 0 && spec.ex1b > 0) {
					switch(spec.ex1a) {
						case 0:
							monst_spell_note(univ.town.monst[spec.ex2a].number,46);
							kill_monst(&univ.town.monst[spec.ex2a],7,eMainStatus::DEAD);
							break;
						case 1:
							monst_spell_note(univ.town.monst[spec.ex2a].number,51);
							kill_monst(&univ.town.monst[spec.ex2a],7,eMainStatus::DUST);
							break;
						case 2:
							if(spec.ex1c > 0)
								petrify_monst(&univ.town.monst[spec.ex2a], spec.ex1c);
							else {
								monst_spell_note(univ.town.monst[spec.ex2a].number,8);
								kill_monst(&univ.town.monst[spec.ex2a],7,eMainStatus::STONE);
							}
							break;
					}
				}
				// Bring back to life
				else if(univ.town.monst[spec.ex2a].active == 0 && spec.ex1b == 0) {
					univ.town.monst[spec.ex2a].active = 1;
					monst_spell_note(univ.town.monst[spec.ex2a].number,45);
				}
			}
			break;
		case eSpecType::AFFECT_POISON:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i)) {
						if(spec.ex1b == 0) {
							cure_pc(i,spec.ex1a);
						}
						else poison_pc(i,spec.ex1a);
					}
			}
			else {
				if(univ.town.monst[spec.ex2a].active > 0) {
					short alvl = spec.ex1a;
					if(spec.ex1b == 0)
						alvl = -1*alvl;
					poison_monst(&univ.town.monst[spec.ex2a],alvl);
				}
			}
			break;
		case eSpecType::AFFECT_SPEED:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i)) {
						if(spec.ex1b == 0) {
							slow_pc(i,-spec.ex1a);
						}
						else slow_pc(i,spec.ex1a);
					}
			}
			else {
				if(univ.town.monst[spec.ex2a].active > 0) {
					short alvl = spec.ex1a;
					if(spec.ex1b == 0)
						alvl = -1*alvl;
					slow_monst(&univ.town.monst[spec.ex2a],alvl);
				}
			}
			break;
		case eSpecType::AFFECT_INVULN:
			for(i = 0; i < 6; i++)
				if((pc < 0) || (pc == i))
					affect_pc(i,eStatus::INVULNERABLE,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case eSpecType::AFFECT_MAGIC_RES:
			for(i = 0; i < 6; i++)
				if((pc < 0) || (pc == i))
					affect_pc(i,eStatus::MAGIC_RESISTANCE,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case eSpecType::AFFECT_WEBS:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i))
						affect_pc(i,eStatus::WEBS,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			}
			else {
				if(univ.town.monst[spec.ex2a].active > 0) {
					short alvl = spec.ex1a;
					if(spec.ex1b == 0)
						alvl = -1*alvl;
					web_monst(&univ.town.monst[spec.ex2a],alvl);
				}
			}
			break;
		case eSpecType::AFFECT_DISEASE:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i))
						affect_pc(i,eStatus::DISEASE,spec.ex1a * ((spec.ex1b != 0) ? 1: -1));
			}
			else {
				if(univ.town.monst[spec.ex2a].active > 0) {
					short alvl = spec.ex1a;
					if(spec.ex1b == 0)
						alvl = -1*alvl;
					disease_monst(&univ.town.monst[spec.ex2a],alvl);
				}
			}
			break;
		case eSpecType::AFFECT_SANCTUARY:
			for(i = 0; i < 6; i++)
				if((pc < 0) || (pc == i))
					affect_pc(i,eStatus::INVISIBLE,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case eSpecType::AFFECT_CURSE_BLESS:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i))
						affect_pc(i,eStatus::BLESS_CURSE,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			}
			else {
				if(univ.town.monst[spec.ex2a].active > 0) {
					short alvl = spec.ex1a;
					if(spec.ex1b == 0)
						alvl = -1*alvl;
					curse_monst(&univ.town.monst[spec.ex2a],alvl);
				}
			}
			break;
		case eSpecType::AFFECT_DUMBFOUND:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i))
						affect_pc(i,eStatus::DUMB,spec.ex1a * ((spec.ex1b == 0) ? -1: 1));
			}
			else {
				if(univ.town.monst[spec.ex2a].active > 0) {
					short alvl = spec.ex1a;
					if(spec.ex1b == 0)
						alvl = -1*alvl;
					dumbfound_monst(&univ.town.monst[spec.ex2a],alvl);
				}
			}
			break;
		case eSpecType::AFFECT_SLEEP:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i)) {
						if(spec.ex1b == 0) {
							affect_pc(i,eStatus::ASLEEP,-1 * spec.ex1a);
						}
						else sleep_pc(i,spec.ex1a,eStatus::ASLEEP,10);
					}
			}
			else {
				if(univ.town.monst[spec.ex2a].active > 0) {
					short alvl = spec.ex1a;
					if(spec.ex1b == 0)
						alvl = -1*alvl;
					charm_monst(&univ.town.monst[spec.ex2a],0,eStatus::ASLEEP,alvl);
				}
			}
			break;
		case eSpecType::AFFECT_PARALYSIS:
			if(spec.ex2a < 0) {
				for(i = 0; i < 6; i++)
					if((pc < 0) || (pc == i)) {
						if(spec.ex1b == 0) {
							affect_pc(i,eStatus::PARALYZED,-1 * spec.ex1a);
						}
						else sleep_pc(i,spec.ex1a,eStatus::PARALYZED,10);
					}
			}
			else {
				if(univ.town.monst[spec.ex2a].active > 0) {
					short alvl = spec.ex1a;
					if(spec.ex1b == 0)
						alvl = -1*alvl;
					charm_monst(&univ.town.monst[spec.ex2a],0,eStatus::PARALYZED,alvl);
				}
			}
			break;
		case eSpecType::AFFECT_STAT:
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
			if(spec.ex1a != minmax(0,61,spec.ex1a)) {
				giveError("Mage spell is out of range (0 - 61). See docs.");
				break;
			}
			for(i = 0; i < 6; i++)
				if((pc < 0) || (pc == i))
					univ.party[i].mage_spells[spec.ex1a] = spec.ex1b;
			break;
		case eSpecType::AFFECT_PRIEST_SPELL:
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
		case eSpecType::AFFECT_STEALTH:
			r1 = (short) PSD[SDF_PARTY_STEALTHY];
			r1 = minmax(0,250,r1 + spec.ex1a);
			PSD[SDF_PARTY_STEALTHY] = r1;
			break;
		case eSpecType::AFFECT_FIREWALK:
			r1 = (short) PSD[SDF_PARTY_FIREWALK];
			r1 = minmax(0,250,r1 + spec.ex1a);
			PSD[SDF_PARTY_FIREWALK] = r1;
			break;
		case eSpecType::AFFECT_FLIGHT:
			if(univ.party.in_boat >= 0)
				add_string_to_buf("  Can't fly when on a boat. ");
			else if(univ.party.in_horse >= 0)////
				add_string_to_buf("  Can't fly when on a horse.  ");
			else {
				r1 = (short) PSD[SDF_PARTY_FLIGHT];
				r1 = minmax(0,250,r1 + spec.ex1a);
				PSD[SDF_PARTY_FLIGHT] = r1;
			}
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
		case eSpecType::IF_TOWN_TER_TYPE:
			if(((is_town()) || (is_combat())) && (univ.town->terrain(spec.ex1a,spec.ex1b) == spec.ex2a))
				*next_spec = spec.ex2b;
			break;
		case eSpecType::IF_OUT_TER_TYPE:
			l.x = spec.ex1a; l.y = spec.ex1b;
			l = local_to_global(l);
			if((is_out()) && (univ.out[l.x][l.y] == spec.ex2a))
				*next_spec = spec.ex2b;
			break;
		case eSpecType::IF_HAS_GOLD:
			if(univ.party.gold >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_HAS_FOOD:
			if(univ.party.food >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_ITEM_CLASS_ON_SPACE:
			if(is_out())
				break;
			l.x = spec.ex1a; l.y = spec.ex1b;
			for(i = 0; i < NUM_TOWN_ITEMS; i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].special_class == (unsigned)spec.ex2a
				   && (l == univ.town.items[i].item_loc))
					*next_spec = spec.ex2b;
			break;
		case eSpecType::IF_HAVE_ITEM_CLASS:
			if(party_check_class(spec.ex1a,1))
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_EQUIP_ITEM_CLASS:
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE)
					for(j = 0; j < 24; j++)
						if(univ.party[i].items[j].variety != eItemType::NO_ITEM && univ.party[i].items[j].special_class == (unsigned)spec.ex1a
						   && (univ.party[i].equip[j]))
							*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_HAS_GOLD_AND_TAKE:
			if(univ.party.gold >= spec.ex1a) {
				take_gold(spec.ex1a,true);
				*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_HAS_FOOD_AND_TAKE:
			if(univ.party.food >= spec.ex1a) {
				take_food(spec.ex1a,true);
				*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_ITEM_CLASS_ON_SPACE_AND_TAKE:
			if(is_out())
				break;
			l.x = spec.ex1a; l.y = spec.ex1b;
			for(i = 0; i < NUM_TOWN_ITEMS; i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].special_class == (unsigned)spec.ex2a
				   && (l == univ.town.items[i].item_loc)) {
					*next_spec = spec.ex2b;
					*redraw = 1;
					univ.town.items[i].variety = eItemType::NO_ITEM;
				}
			break;
		case eSpecType::IF_HAVE_ITEM_CLASS_AND_TAKE:
			if(party_check_class(spec.ex1a,0))
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_EQUIP_ITEM_CLASS_AND_TAKE:
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE)
					for(j = 0; j < 24; j++)
						if(univ.party[i].items[j].variety != eItemType::NO_ITEM && univ.party[i].items[j].special_class == (unsigned)spec.ex1a
						   && (univ.party[i].equip[j])) {
							*next_spec = spec.ex1b;
							*redraw = 1;
							take_item(i,j);
						}
			break;
		case eSpecType::IF_DAY_REACHED:
			if(calc_day() >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_FIELDS:
			if(!isValidField(spec.ex1a, false)) {
				giveError("Scenario tried to check for invalid field type (1...24)");
				break;
			}
			i = 0;
			for(j = 0; j < univ.town->max_dim(); j++)
				for(k = 0; k < univ.town->max_dim(); k++) {
					switch(eFieldType(spec.ex1a)) {
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
			if(i > 0)
				*next_spec = spec.ex1b;
			// TODO: Are there other object types to account for?
			// TODO: Allow restricting to a specific rect
			// TODO: Allow requiring a minimum and maximum number of objects
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
					// Past here are special values that don't have an equivalent in eSpecCtx.
				case 100: // Look (town or out)
					if(which_mode == eSpecCtx::OUT_LOOK || which_mode == eSpecCtx::TOWN_LOOK)
						*next_spec = spec.ex1c;
					break;
				case 101: // In boat
					if(univ.party.in_boat >= 0)
						*next_spec = spec.ex1c;
					break;
				case 102: // On horse
					if(univ.party.in_horse >= 0)
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
	// TODO: Use dynamic size() instead of hard-coded limit
	if(terrain_type >= 256) return;
	ter_num_t former = univ.town->terrain(l.x,l.y);
	univ.town->terrain(l.x,l.y) = terrain_type;
	if(univ.scenario.ter_types[terrain_type].special == eTerSpec::CONVEYOR)
		belt_present = true;
	if(univ.scenario.ter_types[former].light_radius != univ.scenario.ter_types[terrain_type].light_radius)
		univ.town->set_up_lights();
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
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	l.x = spec.ex1a; l.y = spec.ex1b;
	
	if(is_out())
		return;
	switch(cur_node.type) {
		case eSpecType::MAKE_TOWN_HOSTILE:
			set_town_attitude(spec.ex1a,spec.ex1b,spec.ex2a);
			break;
		case eSpecType::TOWN_CHANGE_TER:
			set_terrain(l,spec.ex2a);
			*redraw = true;
			draw_map(true);
			break;
		case eSpecType::TOWN_SWAP_TER:
			if(coord_to_ter(spec.ex1a,spec.ex1b) == spec.ex2a){
				set_terrain(l,spec.ex2b);
			}
			else if(coord_to_ter(spec.ex1a,spec.ex1b) == spec.ex2b){
				set_terrain(l,spec.ex2a);
			}
			*redraw = 1;
			draw_map(true);
			break;
		case eSpecType::TOWN_TRANS_TER:
			ter = coord_to_ter(spec.ex1a,spec.ex1b);
			set_terrain(l,univ.scenario.ter_types[ter].trans_to_what);
			*redraw = 1;
			draw_map(true);
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
			for(i = 0; i < univ.town->max_monst(); i++)
				if(univ.town.monst[i].number == spec.ex1a) {
					univ.town.monst[i].active = 0;
				}
			*redraw = 1;
			break;
		case eSpecType::TOWN_NUKE_MONSTS:
			for(i = 0; i < univ.town->max_monst(); i++)
				if((univ.town.monst[i].active > 0) &&
					(((spec.ex1a == 0) && (1 == 1)) ||
					 ((spec.ex1a == 1) && (univ.town.monst[i].attitude % 2 == 0)) ||
					 ((spec.ex1a == 2) && (univ.town.monst[i].attitude % 2 == 1)))){
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
			place_item(store_i,l,true);
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
			r1 = char_select_pc(1,0,"Which character goes?");
			if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
				*a = 1;
			if(r1 != 6) {
				current_pc = r1;
				*next_spec = -1;
				ASB(univ.party.start_split(spec.ex1a,spec.ex1b,spec.ex2a,r1));
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
			ASB(univ.party.end_split(spec.ex1a));
			update_explored(univ.town.p_loc);
			center = univ.town.p_loc;
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
	
	if(is_out())
		return;
	
	*redraw = 1;
	for(i = spec.ex1b;i <= spec.ex2b;i++)
		for(j = spec.ex1a; j <= spec.ex2a; j++) {
			l.x = i; l.y = j;
			switch(cur_node.type) {
				case eSpecType::RECT_PLACE_FIELD:
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
					for(k = 0; k < NUM_TOWN_ITEMS; k++)
						if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].item_loc == l) {
							univ.town.items[k].item_loc.x = spec.sd1;
							univ.town.items[k].item_loc.y = spec.sd2;
						}
					break;
				case eSpecType::RECT_DESTROY_ITEMS:
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
		case eSpecType::OUT_CHANGE_TER:
			if(spec.ex2a < 0) break;
			univ.out->terrain[spec.ex1a][spec.ex1b] = spec.ex2a;
			l.x = spec.ex1a;
			l.y = spec.ex1b;
			l = local_to_global(l);
			univ.out[l.x][l.y] = spec.ex2a;
			*redraw = 1;
			check_mess = true;
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
		case eSpecType::OUT_STORE:
			get_strs(str1,str2,1,spec.m1,-1);
			if(spec.ex2a >= 40)
				spec.ex2a = 39;
			if(spec.ex2a < 1)
				spec.ex2a = 1;
			spec.ex2b = minmax(0,6,spec.ex2b);
			start_shop_mode(eShopType(spec.ex1b), spec.ex1a, spec.ex1a + spec.ex2a - 1, spec.ex2b, str1);
			*next_spec = -1;
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
