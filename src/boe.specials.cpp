
#include <cstdio>
#include <cstring>
#include <queue>
#include <map>

#include "boe.global.hpp"

#include "universe.hpp"
#include "boe.party.hpp"
#include "boe.town.hpp"
#include "boe.text.hpp"
#include "boe.infodlg.hpp"
#include "boe.items.hpp"
#include "boe.combat.hpp"
#include "boe.monster.hpp"
#include "boe.locutils.hpp"
#include "boe.actions.hpp"
#include "soundtool.hpp"
#include "boe.townspec.hpp"
#include "boe.graphics.hpp"
#include "boe.fileio.hpp"
#include "boe.specials.hpp"
#include "boe.newgraph.hpp"
#include "boe.dlgutil.hpp"
#include "mathutil.hpp"
#include "boe.main.hpp"
#include "strdlog.hpp"
#include "choicedlog.hpp"
#include "fileio.hpp"
#include <array>
#include "spell.hpp"
#include "boe.menus.hpp"

extern sf::RenderWindow mainPtr;
extern eGameMode overall_mode;
extern short which_combat_type,current_pc,stat_window;
extern location center;
extern bool processing_fields,monsters_going,boom_anim_active;
extern effect_pat_type single,t,square,radius2,radius3,small_square,open_square,field[8];
extern effect_pat_type current_pat;
extern cOutdoors::cWandering store_wandering_special;
extern eSpell spell_being_cast, town_spell;
extern short spell_caster, spec_target_fail, spec_target_type, spec_target_options;
extern sf::RenderWindow mini_map;
extern short fast_bang;
extern bool end_scenario;
extern cUniverse univ;
extern std::queue<pending_special_type> special_queue;
extern short combat_posing_monster;

bool can_draw_pcs = true;
bool fog_lifted = false;
bool cartoon_happening = false;

std::map<eDamageType,int> boom_gr = {
	{eDamageType::WEAPON, 3},
	{eDamageType::FIRE, 0},
	{eDamageType::POISON, 2},
	{eDamageType::MAGIC, 1},
	{eDamageType::UNBLOCKABLE, 5},
	{eDamageType::COLD, 4},
	{eDamageType::UNDEAD, 3},
	{eDamageType::DEMON, 3},
	{eDamageType::SPECIAL, 1},
};
short store_item_spell_level = 10;

// global values for when processing special encounters
iLiving* current_pc_picked_in_spec_enc = nullptr;
extern std::map<eSkill,short> skill_max;
bool special_in_progress = false;

static void start_cartoon() {
	if(!cartoon_happening && !is_combat()) {
		for(int i = 0; i < 6; i++)
			univ.party[i].combat_pos = univ.party.town_loc;
	}
	cartoon_happening = true;
}

// 0 - can't use 1 - combat only 2 - town only 3 - town & combat only  4 - everywhere  5 - outdoor
// + 10 - mag. inept can use
std::map<eItemAbil, short> abil_chart = {
	{eItemAbil::POISON_WEAPON,13}, {eItemAbil::AFFECT_STATUS,3}, {eItemAbil::BLISS_DOOM,3}, {eItemAbil::AFFECT_EXPERIENCE,4},
	{eItemAbil::AFFECT_SKILL_POINTS,4}, {eItemAbil::AFFECT_HEALTH,4}, {eItemAbil::AFFECT_SPELL_POINTS,4},
	{eItemAbil::LIGHT,13}, {eItemAbil::AFFECT_PARTY_STATUS,3}, {eItemAbil::HEALTH_POISON,4},
	{eItemAbil::CALL_SPECIAL,4}, {eItemAbil::CAST_SPELL,4}, {eItemAbil::MESSAGE,14},
};

// which is unused
//short mode; // 0 - pre  1 - end by victory  2 - end by flight
// wanderin spec 99 -> generic spec
bool handle_wandering_specials (short /*which*/,short mode) {
	
	// TODO: Is loc_in_sec the correct location to pass here?
	// (I'm pretty sure it is, but I should verify it somehow.)
	// (It's either that or univ.party.p_loc.)
	short s1 = 0,s2 = 0,s3 = 0;
	// TODO: If s2 > 0, encounter is forced (monsters don't flee even if they're weak)
	
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
bool check_special_terrain(location where_check,eSpecCtx mode,cPlayer& which_pc,bool *forced) {
	ter_num_t ter;
	short r1,door_pc,pic_type = 0,ter_pic = 0;
	eTerSpec ter_special;
	std::string choice;
	int ter_flag1,ter_flag2,ter_flag3;
	eDamageType dam_type = eDamageType::WEAPON;
	bool can_enter = true;
	location out_where,from_loc,to_loc;
	short s1 = 0,s2 = 0,s3 = 0;
	
	short spec_num = -1;
	*forced = false;
	
	switch(mode) {
		case eSpecCtx::OUT_MOVE:
			ter = univ.out[where_check.x][where_check.y];
			from_loc = univ.party.out_loc;
			break;
		case eSpecCtx::TOWN_MOVE:
			ter = univ.town->terrain(where_check.x,where_check.y);
			from_loc = univ.party.town_loc;
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
			((ter_flag3 == DIR_N) && (where_check.y > from_loc.y)) ||
			((ter_flag3 == DIR_E) && (where_check.x < from_loc.x)) ||
			((ter_flag3 == DIR_S) && (where_check.y < from_loc.y)) ||
			((ter_flag3 == DIR_W) && (where_check.x > from_loc.x)) ) {
			ASB("The moving floor prevents you.");
			return false;
		}
	}
	
	if(mode == eSpecCtx::OUT_MOVE) {
		out_where = global_to_local(where_check);
		
		for(short i = 0; i < univ.out->special_locs.size(); i++)
			if(out_where == univ.out->special_locs[i]) {
				spec_num = univ.out->special_locs[i].spec;
				// call special
				run_special(mode,1,spec_num,out_where,&s1,&s2,&s3);
				if(s1 > 0)
					can_enter = false;
				else if(s2 > 0)
					*forced = true;
				erase_out_specials();
				put_pc_screen();
				put_item_screen(stat_window);
			}
	}
	
	if((is_combat()) && (univ.town.is_spot(where_check.x, where_check.y) ||
						  (univ.scenario.ter_types[coord_to_ter(where_check.x, where_check.y)].trim_type == eTrimType::CITY))) {
		ASB("Move: Can't trigger this special in combat.");
		return false; // TODO: Maybe replace eTrimType::CITY check with a blockage == clear/special && is_special() check?
	}
	
	if(mode != eSpecCtx::OUT_MOVE && univ.town.is_force_barr(where_check.x,where_check.y)) {
		add_string_to_buf("  Magic barrier!");
		can_enter = false;
	}
	if(mode != eSpecCtx::OUT_MOVE && univ.town.is_force_cage(where_check.x,where_check.y)) {
		add_string_to_buf("  Force cage!");
		can_enter = false;
	}
	if((mode == eSpecCtx::TOWN_MOVE || (mode == eSpecCtx::COMBAT_MOVE && which_combat_type == 1))
		&& can_enter && univ.town.is_special(where_check.x,where_check.y)) {
		for(short i = 0; i < univ.town->special_locs.size(); i++)
			if(where_check == univ.town->special_locs[i]) {
				spec_num = univ.town->special_locs[i].spec;
				bool runSpecial = false;
				if(!is_blocked(where_check)) runSpecial = true;
				if(ter_special == eTerSpec::CHANGE_WHEN_STEP_ON) runSpecial = true;
				if(ter_special == eTerSpec::CALL_SPECIAL) runSpecial = true;
				if(univ.town->specials[spec_num].type == eSpecType::CANT_ENTER)
					runSpecial = true;
				if(!univ.scenario.is_legacy && univ.party.in_boat >= 0 && univ.scenario.ter_types[ter].boat_over)
					runSpecial = true;
				if(runSpecial) {
					give_help(54,0);
					run_special(mode,2,spec_num,where_check,&s1,&s2,&s3);
					if(s1 > 0)
						can_enter = false;
					else if(s2 > 0)
						*forced = true;
				}
			}
		put_pc_screen();
		put_item_screen(stat_window);
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
		
		if(univ.town.is_web(where_check.x,where_check.y) && univ.party[current_pc].race != eRace::BUG) {
			add_string_to_buf("  Webs!");
			if(mode != eSpecCtx::COMBAT_MOVE) {
				for(short i = 0; i < 6; i++) {
					r1 = get_ran(1,2,3);
					univ.party[i].web(r1);
				}
			}
			else univ.party[current_pc].web(get_ran(1,2,3));
			put_pc_screen();
			univ.town.set_web(where_check.x,where_check.y,false);
		}
		if(univ.town.is_crate(where_check.x,where_check.y)) {
			add_string_to_buf("  You push the crate.");
			to_loc = push_loc(from_loc,where_check);
			univ.town.set_crate(where_check.x,where_check.y,false);
			if(to_loc.x > 0)
				univ.town.set_crate(to_loc.x,to_loc.y,true);
			for(short i = 0; i < univ.town.items.size(); i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where_check
				   && univ.town.items[i].contained && univ.town.items[i].held)
					univ.town.items[i].item_loc = to_loc;
		}
		if(univ.town.is_barrel(where_check.x,where_check.y)) {
			add_string_to_buf("  You push the barrel.");
			to_loc = push_loc(from_loc,where_check);
			univ.town.set_barrel(where_check.x,where_check.y,false);
			if(to_loc.x > 0)
				univ.town.set_barrel(to_loc.x,to_loc.y,true);
			for(short i = 0; i < univ.town.items.size(); i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where_check
				   && univ.town.items[i].contained && univ.town.items[i].held)
					univ.town.items[i].item_loc = to_loc;
		}
		if(univ.town.is_block(where_check.x,where_check.y)) {
			add_string_to_buf("  You push the stone block.");
			to_loc = push_loc(from_loc,where_check);
			univ.town.set_block(where_check.x,where_check.y,false);
			if(to_loc.x > 0)
				univ.town.set_block(to_loc.x,to_loc.y,true);
		}
	}
	
	switch(ter_special) {
			// First, put the specials that aren't activated on moving into the space here.
			// This is to silence compiler warnings while still preserving the warning if a new special is added.
		case eTerSpec::NONE: case eTerSpec::BRIDGE: case eTerSpec::BED:
		case eTerSpec::UNUSED1: case eTerSpec::CRUMBLING: case eTerSpec::LOCKABLE:
		case eTerSpec::UNUSED2: case eTerSpec::IS_A_SIGN: case eTerSpec::UNUSED3:
		case eTerSpec::IS_A_CONTAINER: case eTerSpec::WATERFALL_CAVE: case eTerSpec::WATERFALL_SURFACE:
		case eTerSpec::CONVEYOR: case eTerSpec::BLOCKED_TO_MONSTERS: case eTerSpec::TOWN_ENTRANCE:
		case eTerSpec::CHANGE_WHEN_USED: case eTerSpec::CALL_SPECIAL_WHEN_USED:
			break;
		case eTerSpec::CHANGE_WHEN_STEP_ON:
			alter_space(where_check.x,where_check.y,ter_flag1);
			if(ter_flag2 >= 0) {
				play_sound(-1 * ter_flag2);
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
			if(ter_flag3 > 0 && ter_flag3 < 8)
				dam_type = (eDamageType) ter_flag3;
			else dam_type = eDamageType::WEAPON;
			r1 = get_ran(ter_flag2,1,ter_flag1);
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
				case eDamageType::SPECIAL:
					dam_type = eDamageType::UNBLOCKABLE;
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
				case eDamageType::MARKED:
					break;
			}
			if(r1 < 0) break; // "It doesn't affect you."
			if(mode != eSpecCtx::COMBAT_MOVE)
				hit_party(r1,dam_type);
			fast_bang = 1;
			if(mode == eSpecCtx::COMBAT_MOVE)
				damage_pc(which_pc,r1,dam_type,eRace::UNKNOWN,0);
			else
				boom_space(univ.party.out_loc,overall_mode,pic_type,r1,12);
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
			for(short i = mode == eSpecCtx::COMBAT_MOVE ? univ.get_target_i(which_pc) : 0 ; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE) {
					if(get_ran(1,1,100) <= ter_flag2) {
						switch((eStatus)ter_flag3){
								// TODO: Should we disallow some statuses when outdoors?
							case eStatus::POISONED_WEAPON: // TODO: Do something here
								if(get_ran(1,1,100) > 60) add_string_to_buf("It's eerie here...");
								break;
							case eStatus::BLESS_CURSE: // Should say "You feel awkward." / "You feel blessed."?
								univ.party[i].curse(ter_flag1);
								break;
							case eStatus::POISON:
								univ.party[i].poison(ter_flag1);
								break;
							case eStatus::HASTE_SLOW: // Should say "You feel sluggish." / "You feel speedy."?
								univ.party[i].slow(ter_flag1);
								break;
							case eStatus::INVULNERABLE: // Should say "You feel odd." / "You feel protected."?
								univ.party[i].apply_status(eStatus::INVULNERABLE,ter_flag1);
								break;
							case eStatus::MAGIC_RESISTANCE: // Should say "You feel odd." / "You feel protected."?
								univ.party[i].apply_status(eStatus::MAGIC_RESISTANCE,ter_flag1);
								break;
							case eStatus::WEBS: // Should say "You feel sticky." / "Your skin tingles."?
								univ.party[i].web(ter_flag1);
								break;
							case eStatus::DISEASE: // Should say "You feel healthy." / "You feel sick."?
								univ.party[i].disease(ter_flag1);
								break;
							case eStatus::INVISIBLE:
								if(ter_flag1 < 0) add_string_to_buf("You feel obscure.");
								else add_string_to_buf("You feel exposed.");
								univ.party[i].apply_status(eStatus::INVISIBLE,ter_flag1);
								break;
							case eStatus::DUMB: // Should say "You feel clearheaded." / "You feel confused."?
								univ.party[i].dumbfound(ter_flag1);
								break;
							case eStatus::MARTYRS_SHIELD: // Should say "You feel dull." / "You start to glow slightly."?
								univ.party[i].apply_status(eStatus::MARTYRS_SHIELD,ter_flag1);
								break;
							case eStatus::ASLEEP: // Should say "You feel alert." / "You feel very tired."?
								univ.party[i].sleep(eStatus::ASLEEP,ter_flag1,ter_flag1 / 2);
								break;
							case eStatus::PARALYZED: // Should say "You find it easier to move." / "You feel very stiff."?
								univ.party[i].sleep(eStatus::PARALYZED,ter_flag1,ter_flag1 / 2);
								break;
							case eStatus::ACID: // Should say "Your skin tingles pleasantly." / "Your skin burns!"?
								univ.party[i].acid(ter_flag1);
								break;
							case eStatus::FORCECAGE:
								if(is_out()) break;
								univ.party[i].sleep(eStatus::FORCECAGE,ter_flag1,ter_flag1 / 2);
								break;
							case eStatus::MAIN: case eStatus::CHARM: // These magic values are illegal in this context
								break;
						}
						if(mode == eSpecCtx::COMBAT_MOVE) break; // only damage once in combat!
					}
				}
			put_pc_screen();
			if(ter_flag3 == int(eStatus::DUMB))
				adjust_spell_menus();
			//print_nums(1,which_pc,current_pc);
			break;
		case eTerSpec::CALL_SPECIAL: {
			short spec_type = 0;
			if(ter_flag2 == 1){
				if(mode == eSpecCtx::TOWN_MOVE || (mode == eSpecCtx::COMBAT_MOVE && which_combat_type == 1))
					spec_type = 2;
				else spec_type = 1;
			}
			run_special(mode,spec_type,ter_flag1,where_check,&s1,&s2,&s3);
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
		case eTerSpec::WILDERNESS_CAVE:
		case eTerSpec::WILDERNESS_SURFACE:
			handle_hunting();
			break;
	}
	
	// Action may change terrain, so update what's been seen
	if(is_town())
		update_explored(univ.party.town_loc);
	if(is_combat())
		update_explored(univ.party[current_pc].combat_pos);
	
	return can_enter;
}

// This procedure find the effects of fields that would affect a PC who moves into
// a space or waited in that same space
// In town mode, process_fields() is responsible for actually dealing the damage
// All this does is print a message
void check_fields(location where_check,eSpecCtx mode,cPlayer& which_pc) {
	short r1;
	
	if(mode != eSpecCtx::COMBAT_MOVE && mode != eSpecCtx::TOWN_MOVE && mode != eSpecCtx::OUT_MOVE) {
		std::cout << "Note: Improper mode passed to check_special_terrain: " << int(mode) << std::endl;
		return;
	}
	if(is_out())
		return;
	if(univ.town.is_fire_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Fire wall!");
		r1 = get_ran(1,1,6) + 1;
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::FIRE,eRace::UNKNOWN,0);
	}
	if(univ.town.is_force_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Force wall!");
		r1 = get_ran(2,1,6);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::MAGIC,eRace::UNKNOWN,0);
	}
	if(univ.town.is_ice_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Ice wall!");
		r1 = get_ran(2,1,6);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::COLD,eRace::UNKNOWN,0);
		if(overall_mode < MODE_COMBAT)
			boom_space(univ.party.town_loc,overall_mode,4,r1,7);
	}
	if(univ.town.is_blade_wall(where_check.x,where_check.y)) {
		add_string_to_buf("  Blade wall!");
		r1 = get_ran(4,1,8);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::WEAPON,eRace::UNKNOWN,0);
	}
	if(univ.town.is_quickfire(where_check.x,where_check.y)) {
		add_string_to_buf("  Quickfire!");
		r1 = get_ran(2,1,8);
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::FIRE,eRace::UNKNOWN,0);
	}
	if(univ.town.is_scloud(where_check.x,where_check.y)) {
		add_string_to_buf("  Stinking cloud!");
		which_pc.curse(get_ran(1,2,3));
	}
	if(univ.town.is_sleep_cloud(where_check.x,where_check.y)) {
		add_string_to_buf("  Sleep cloud!");
		which_pc.sleep(eStatus::ASLEEP,3,0);
	}
	if(univ.town.is_fire_barr(where_check.x,where_check.y)) {
		add_string_to_buf("  Magic barrier!");
		r1 = get_ran(2,1,10);
		if(is_town()) fast_bang = 1;
		if(mode == eSpecCtx::COMBAT_MOVE)
			damage_pc(which_pc,r1,eDamageType::MAGIC,eRace::UNKNOWN,0);
		else hit_party(r1,eDamageType::MAGIC,0);
		fast_bang = 0;
	}
	put_pc_screen();
}

void use_spec_item(short item) {
	short i,j,k;
	location null_loc;
	
	run_special(eSpecCtx::USE_SPEC_ITEM,0,univ.scenario.special_items[item].special,univ.party.get_loc(),&i,&j,&k);
}


void use_item(short pc,short item) {
	bool take_charge = true,inept_ok = false;
	short level,item_use_code,str,r1;
	short sp[3] = {}; // Dummy values to pass to run_special; not actually used
	std::string str1, str2; // Used by books
	eStatus status;
	eItemUse type;
	eSpell spell;
	location user_loc;
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
		user_loc = univ.party.out_loc;
	if(is_town())
		user_loc = univ.party.town_loc;
	if(is_combat())
		user_loc = univ.party[current_pc].combat_pos;
	
	if(item_use_code == 0) {
		add_string_to_buf("Use: Can't use this item.");
		take_charge = false;
	}
	if(univ.party[pc].traits[eTrait::MAGICALLY_INEPT] && !inept_ok){
		add_string_to_buf("Use: Can't - magically inept.");
		take_charge = false;
	}
	
	// 0 - can't use 1 - combat only 2 - town only 3 - town & combat only  4 - everywhere  5 - outdoor
	if(take_charge) {
		if(overall_mode == MODE_OUTDOORS && item_use_code < 4) {
			add_string_to_buf("Use: Not while outdoors.");
			take_charge = false;
		}
		// TODO: Almost all of these look wrong!
		if((overall_mode == MODE_TOWN) && (item_use_code == 1)) {
			add_string_to_buf("Use: Not while in town.");
			take_charge = false;
		}
		if((overall_mode == MODE_COMBAT) && (item_use_code == 2)) {
			add_string_to_buf("Use: Not in combat.");
			take_charge = false;
		}
		if((overall_mode != MODE_OUTDOORS) && (item_use_code == 5)){
			add_string_to_buf("Use: Only outdoors.");
			take_charge = false;
		}
	}
	if(take_charge) {
		cItem& the_item = univ.party[pc].items[item];
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
				take_charge = poison_weapon(pc,str,false);
				break;
			case eItemAbil::AFFECT_STATUS:
				switch(status) {
					case eStatus::MAIN: case eStatus::CHARM:
						// These don't make any sense in this context.
						break;
					case eStatus::POISONED_WEAPON:
						if(the_item.abil_harms()) {
							ASB("  Weapon poison lost.");
							if(the_item.abil_group())
								univ.party.apply_status(eStatus::POISONED_WEAPON,-str);
							else univ.party[pc].apply_status(eStatus::POISONED_WEAPON,-str);
						} else if(the_item.abil_group()) {
							for(short i = 0; i < 6; i++)
								take_charge = take_charge || poison_weapon(i,str,true);
						} else take_charge = poison_weapon(pc,str,true);
						break;
					case eStatus::BLESS_CURSE:
						play_sound(4);
						if(!the_item.abil_harms()) {
							ASB("  You feel blessed.");
							str = str * -1;
						}else ASB("  You feel awkward.");
						if(the_item.abil_group())
							univ.party.curse(str);
						else univ.party[pc].curse(str);
						break;
					case eStatus::HASTE_SLOW:
						// TODO: Is this the right sound?
						play_sound(75);
						if(!the_item.abil_harms()) {
							ASB("  You feel speedy.");
							str = str * -1;
						}else ASB("  You feel sluggish.");
						if(the_item.abil_group())
							univ.party.slow(str);
						else univ.party[pc].slow(str);
						break;
					case eStatus::INVULNERABLE:
						// TODO: Is this the right sound?
						play_sound(68);
						if(the_item.abil_harms()) {
							ASB("  You feel odd.");
							str = str * -1;
						}else ASB("  You feel protected.");
						if(the_item.abil_group())
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::MAGIC_RESISTANCE:
						// TODO: Is this the right sound?
						// TODO: This ignores resistances if it's negative
						play_sound(51);
						if(the_item.abil_harms()) {
							ASB("  You feel odd.");
							str = str * -1;
						}else ASB("  You feel protected.");
						if(the_item.abil_group())
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::WEBS:
						if(the_item.abil_harms())
							ASB("  You feel sticky.");
						else {
							ASB("  Your skin tingles.");
							str = str * -1;
						}
						if(the_item.abil_group())
							univ.party.web(str);
						else univ.party[pc].web(str);
						break;
					case eStatus::INVISIBLE:
						// TODO: Is this the right sound?
						play_sound(43);
						if(the_item.abil_harms()) {
							ASB("  You feel exposed.");
							str = str * -1;
						}else ASB("  You feel obscure.");
						if(the_item.abil_group())
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::MARTYRS_SHIELD:
						// TODO: Is this the right sound?
						play_sound(43);
						if(the_item.abil_harms()) {
							ASB("  You feel dull.");
							str = str * -1;
						}else ASB("  You start to glow slightly.");
						if(the_item.abil_group())
							univ.party.apply_status(status,str);
						else univ.party[pc].apply_status(status,str);
						break;
					case eStatus::POISON:
						switch(type) {
							case eItemUse::HELP_ONE:
								ASB("  You feel better.");
								univ.party[pc].cure(str);
								break;
							case eItemUse::HARM_ONE:
								ASB("  You feel ill.");
								univ.party[pc].poison(str);
								break;
							case eItemUse::HELP_ALL:
								ASB("  You all feel better.");
								univ.party.cure(str);
								break;
							case eItemUse::HARM_ALL:
								ASB("  You all feel ill.");
								univ.party.poison(str);
								break;
						}
						break;
					case eStatus::DISEASE:
						switch(type) {
							case eItemUse::HELP_ONE:
								ASB("  You feel healthy.");
								univ.party[pc].apply_status(eStatus::DISEASE,-1 * str);
								break;
							case eItemUse::HARM_ONE:
								ASB("  You feel sick.");
								univ.party[pc].disease(str);
								break;
							case eItemUse::HELP_ALL:
								ASB("  You all feel healthy.");
								univ.party.apply_status(eStatus::DISEASE,-1 * str);
								break;
							case eItemUse::HARM_ALL:
								ASB("  You all feel sick.");
								univ.party.disease(str);
								break;
						}
						break;
					case eStatus::DUMB:
						switch(type) {
							case eItemUse::HELP_ONE:
								ASB("  You feel clear headed.");
								univ.party[pc].apply_status(eStatus::DUMB,-1 * str);
								break;
							case eItemUse::HARM_ONE:
								ASB("  You feel confused.");
								univ.party[pc].dumbfound(str);
								break;
							case eItemUse::HELP_ALL:
								ASB("  You all feel clear headed.");
								univ.party.apply_status(eStatus::DUMB,-1 * str);
								break;
							case eItemUse::HARM_ALL:
								ASB("  You all feel confused.");
								univ.party.dumbfound(str);
								break;
						}
						adjust_spell_menus();
						break;
					case eStatus::ASLEEP:
						switch(type) {
							case eItemUse::HELP_ONE:
								ASB("  You feel alert.");
								univ.party[pc].apply_status(eStatus::ASLEEP,-1 * str);
								break;
							case eItemUse::HARM_ONE:
								ASB("  You feel very tired.");
								univ.party[pc].sleep(eStatus::ASLEEP,str + 1,200);
								break;
							case eItemUse::HELP_ALL:
								ASB("  You all feel alert.");
								univ.party.apply_status(eStatus::ASLEEP,-1 * str);
								break;
							case eItemUse::HARM_ALL:
								ASB("  You all feel very tired.");
								univ.party.sleep(eStatus::ASLEEP,str + 1,200);
								break;
						}
						break;
					case eStatus::PARALYZED:
						switch(type) {
							case eItemUse::HELP_ONE:
								ASB("  You find it easier to move.");
								univ.party[pc].apply_status(eStatus::PARALYZED,-1 * str * 100);
								break;
							case eItemUse::HARM_ONE:
								ASB("  You feel very stiff.");
								univ.party[pc].sleep(eStatus::PARALYZED,str * 20 + 10,200);
								break;
							case eItemUse::HELP_ALL:
								ASB("  You all find it easier to move.");
								univ.party.apply_status(eStatus::PARALYZED,-1 * str * 100);
								break;
							case eItemUse::HARM_ALL:
								ASB("  You all feel very stiff.");
								univ.party.sleep(eStatus::PARALYZED,str * 20 + 10,200);
								break;
						}
						break;
					case eStatus::ACID:
						switch(type) {
							case eItemUse::HELP_ONE:
								ASB("  Your skin tingles pleasantly.");
								univ.party[pc].apply_status(eStatus::ACID,-1 * str);
								break;
							case eItemUse::HARM_ONE:
								ASB("  Your skin burns!");
								univ.party[pc].acid(str);
								break;
							case eItemUse::HELP_ALL:
								ASB("  You all tingle pleasantly.");
								univ.party.apply_status(eStatus::ACID,-1 * str);
								break;
							case eItemUse::HARM_ALL:
								ASB("  Everyone's skin burns!");
								univ.party.acid(str);
								break;
						}
						break;
					case eStatus::FORCECAGE:
						switch(type) {
							case eItemUse::HELP_ONE:
								process_force_cage(univ.party[pc].get_loc(), pc, str);
								break;
							case eItemUse::HARM_ONE:
								univ.party[pc].sleep(eStatus::FORCECAGE, str, str / 2);
								break;
							case eItemUse::HELP_ALL:
								for(short i = 0; i < 6; i++)
									process_force_cage(univ.party[i].get_loc(), i, str);
								break;
							case eItemUse::HARM_ALL:
								univ.party.sleep(eStatus::FORCECAGE, str, str / 2);
								break;
						}
						break;
				}
				break;
			case eItemAbil::BLISS_DOOM:
				switch(type) {
					case eItemUse::HELP_ONE:
						ASB("  You feel wonderful!");
						univ.party[pc].heal(str * 20);
						univ.party[pc].apply_status(eStatus::BLESS_CURSE,str);
						break;
					case eItemUse::HARM_ONE:
						ASB("  You feel terrible.");
						drain_pc(univ.party[pc],str * 5);
						damage_pc(univ.party[pc],20 * str,eDamageType::UNBLOCKABLE,eRace::HUMAN,0);
						univ.party[pc].disease(2 * str);
						univ.party[pc].dumbfound(2 * str);
						break;
					case eItemUse::HELP_ALL:
						ASB("  Everyone feels wonderful!");
						univ.party.heal(str*20);
						univ.party.apply_status(eStatus::BLESS_CURSE,str);
						break;
					case eItemUse::HARM_ALL:
						ASB("  You all feel terrible.");
						for(short i = 0; i < 6; i++) {
							drain_pc(univ.party[i],str * 5);
							damage_pc(univ.party[i],20 * str,eDamageType::UNBLOCKABLE,eRace::HUMAN,0);
							univ.party[i].disease(2 * str);
							univ.party[i].dumbfound(2 * str);
						}
						break;
				}
				if(the_item.abil_harms())
					adjust_spell_menus();
				break;
			case eItemAbil::AFFECT_EXPERIENCE:
				switch(type) {
					case eItemUse::HELP_ONE:
						ASB("  You feel much smarter.");
						award_xp(pc,str * 5);
						break;
					case eItemUse::HARM_ONE:
						ASB("  You feel forgetful.");
						drain_pc(univ.party[pc],str * 5);
						break;
					case eItemUse::HELP_ALL:
						ASB("  You all feel much smarter.");
						award_party_xp(str * 5);
						break;
					case eItemUse::HARM_ALL:
						ASB("  You all feel forgetful.");
						for(short i = 0; i < 6; i++)
							drain_pc(univ.party[i],str * 5);
						break;
				}
				break;
			case eItemAbil::AFFECT_SKILL_POINTS:
				// TODO: Is this the right sound?
				play_sound(68);
				switch(type) {
					case eItemUse::HELP_ONE:
						ASB("  You feel much smarter.");
						univ.party[pc].skill_pts += str;
						break;
					case eItemUse::HARM_ONE:
						ASB("  You feel forgetful.");
						univ.party[pc].skill_pts = max(0,univ.party[pc].skill_pts - str);
						break;
					case eItemUse::HELP_ALL:
						ASB("  You all feel much smarter.");
						for(short i = 0; i < 6; i++)
							univ.party[i].skill_pts += str;
						break;
					case eItemUse::HARM_ALL:
						ASB("  You all feel forgetful.");
						for(short i = 0; i < 6; i++)
							univ.party[i].skill_pts = max(0,univ.party[i].skill_pts - str);
						break;
				}
				break;
			case eItemAbil::AFFECT_HEALTH:
				switch(type) {
					case eItemUse::HELP_ONE:
						ASB("  You feel better.");
						univ.party[pc].heal(str * 20);
						break;
					case eItemUse::HARM_ONE:
						ASB("  You feel sick.");
						damage_pc(univ.party[pc],20 * str,eDamageType::UNBLOCKABLE,eRace::HUMAN,0);
						break;
					case eItemUse::HELP_ALL:
						ASB("  You all feel better.");
						univ.party.heal(str * 20);
						break;
					case eItemUse::HARM_ALL:
						ASB("  You all feel sick.");
						hit_party(20 * str,eDamageType::UNBLOCKABLE);
						break;
				}
				break;
			case eItemAbil::AFFECT_SPELL_POINTS:
				switch(type) {
					case eItemUse::HELP_ONE:
						ASB("  You feel energized.");
						univ.party[pc].restore_sp(str * 5);
						break;
					case eItemUse::HARM_ONE:
						ASB("  You feel drained.");
						univ.party[pc].cur_sp = max(0,univ.party[pc].cur_sp - str * 5);
						break;
					case eItemUse::HELP_ALL:
						ASB("  You all feel energized.");
						univ.party.restore_sp(str * 5);
						break;
					case eItemUse::HARM_ALL:
						ASB("  You all feel drained.");
						for(short i = 0; i < 6; i++)
							univ.party[i].cur_sp = max(0,univ.party[i].cur_sp - str * 5);
						break;
				}
				break;
			case eItemAbil::LIGHT:
				if(!the_item.abil_harms()) {
					ASB("  You have more light.");
					increase_light(50 * str);
				} else {
					ASB("  It gets darker.");
					increase_light(-50 * str);
				}
				break;
			case eItemAbil::AFFECT_PARTY_STATUS:
				if(the_item.abil_harms()) {
					ePartyStatus status = ePartyStatus(the_item.abil_data[1]);
					int i = univ.party.status[status];
					switch(status) {
						case ePartyStatus::STEALTH: ASB("  Your footsteps become louder."); str *= 5; break;
						case ePartyStatus::FIREWALK: ASB("  The chill recedes from your feet."); str *= 2; break;
						case ePartyStatus::DETECT_LIFE: ASB("  Your vision of life becomes blurry."); break;
						case ePartyStatus::FLIGHT:
							if(i <= str) {
								if(blocksMove(univ.scenario.ter_types[univ.out[univ.party.out_loc.x][univ.party.out_loc.y]].blockage)) {
									add_string_to_buf("  You plummet to your deaths.");
									slay_party(eMainStatus::DEAD);
									print_buf();
									pause(150);
								} else if(i > 1) {
									add_string_to_buf("  You plummet to the ground.");
									hit_party(get_ran(i,1,12), eDamageType::SPECIAL);
								} else add_string_to_buf("  You land safely.");
							} else add_string_to_buf("  You start to descend.");
							break;
					}
					if(str > i) str = i;
					str *= -1;
				} else switch(ePartyStatus(the_item.abil_data[1])) {
					case ePartyStatus::STEALTH: ASB("  Your footsteps become quieter."); str *= 5; break;
					case ePartyStatus::FIREWALK: ASB("  You feel chilly."); str *= 2; break;
					case ePartyStatus::DETECT_LIFE: ASB("  You detect life."); break;
					case ePartyStatus::FLIGHT:
						if(univ.party.status[ePartyStatus::FLIGHT] > 0) {
							add_string_to_buf("  Not while already flying.");
							take_charge = false;
						} else if(univ.party.in_boat >= 0) {
							add_string_to_buf("  Leave boat first.");
							take_charge = false;
						} else if(univ.party.in_horse >= 0) {
							add_string_to_buf("  Leave horse first.");
							take_charge = false;
						} else ASB("  You rise into the air!");
						break;
				}
				if(take_charge) univ.party.status[ePartyStatus(univ.party[pc].items[item].abil_data[1])] += str;
				break;
			case eItemAbil::HEALTH_POISON:
				switch(type) {
					case eItemUse::HELP_ONE:
						ASB("  You feel wonderful.");
						univ.party[pc].heal(str*25);
						univ.party[pc].cure(str);
						break;
					case eItemUse::HARM_ONE:
						ASB("  You feel terrible.");
						damage_pc(univ.party[pc], str*25, eDamageType::UNBLOCKABLE, eRace::UNKNOWN, 0);
						univ.party[pc].poison(str);
						break;
					case eItemUse::HELP_ALL:
						ASB("  You all feel wonderful.");
						univ.party.heal(str*25);
						univ.party.cure(str);
						break;
					case eItemUse::HARM_ALL:
						ASB("  You all feel terrible.");
						hit_party(str*25, eDamageType::UNBLOCKABLE);
						univ.party.poison(str);
						break;
				}
				break;
			case eItemAbil::CALL_SPECIAL:
				// TODO: Should this have its own separate eSpecCtx?
				run_special(eSpecCtx::USE_SPEC_ITEM,0,str,user_loc,&sp[0],&sp[1],&sp[2]);
				if(sp[0] > 0)
					take_charge = false;
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
				if(!summon_monster(univ.party[pc].items[item].abil_data[1],user_loc,str,eAttitude::FRIENDLY,true))
					add_string_to_buf("  Summon failed.");
				break;
			case eItemAbil::MASS_SUMMONING:
				r1 = get_ran(str,1,4); // TODO: This value was never used, so why is it here?
				r1 = get_ran(1,3,5);
				for(short i = 0; i < r1; i++)
					if(!summon_monster(univ.party[pc].items[item].abil_data[1],user_loc,r1,eAttitude::FRIENDLY,true))
						add_string_to_buf("  Summon failed.");
				break;
			case eItemAbil::QUICKFIRE:
				add_string_to_buf("Fire pours out!");
				univ.town.set_quickfire(user_loc.x,user_loc.y,true);
				break;
			case eItemAbil::MESSAGE:
				take_charge = false;
				r1 = univ.party[pc].items[item].desc.find("|||");
				str1 = univ.party[pc].items[item].desc.substr(r1 + 3);
				r1 = str1.find("|||");
				if(r1 != std::string::npos) {
					str2 = str1.substr(r1 + 3);
					str1 = str1.substr(0, r1);
				}
				r1 = univ.party[pc].items[item].graphic_num;
				cStrDlog(str1, str2, "Reading " + univ.party[pc].items[item].name, r1, PIC_ITEM).show();
				break;
				// Now for all the non-usable abilities. These are enumerated here so that the compiler can catch if we've missed one.
			case eItemAbil::ACCURACY: case eItemAbil::ANTIMAGIC_WEAPON: case eItemAbil::ASPTONGUE: case eItemAbil::BOOST_MAGIC:
			case eItemAbil::BOOST_STAT: case eItemAbil::BOOST_WAR: case eItemAbil::CAUSES_FEAR: case eItemAbil::COMFREY:
			case eItemAbil::DAMAGE_PROTECTION: case eItemAbil::DAMAGING_WEAPON: case eItemAbil::DISTANCE_MISSILE:
			case eItemAbil::DRAIN_MISSILES: case eItemAbil::EMBERF: case eItemAbil::ENCUMBERING: case eItemAbil::EVASION:
			case eItemAbil::EXPLODING_WEAPON: case eItemAbil::FREE_ACTION: case eItemAbil::FULL_PROTECTION: case eItemAbil::WILL:
			case eItemAbil::GIANT_STRENGTH: case eItemAbil::GRAYMOLD: case eItemAbil::HEALING_WEAPON: case eItemAbil::HEAVIER_OBJECT:
			case eItemAbil::HIT_CALL_SPECIAL: case eItemAbil::HOLLY: case eItemAbil::LIFE_SAVING: case eItemAbil::LIGHTER_OBJECT:
			case eItemAbil::LOCKPICKS: case eItemAbil::MANDRAKE: case eItemAbil::MARTYRS_SHIELD: case eItemAbil::MAGERY:
			case eItemAbil::NETTLE: case eItemAbil::NONE: case eItemAbil::OCCASIONAL_STATUS: case eItemAbil::POISON_AUGMENT:
			case eItemAbil::PROTECT_FROM_PETRIFY: case eItemAbil::PROTECT_FROM_SPECIES: case eItemAbil::RADIANT:
			case eItemAbil::REGENERATE: case eItemAbil::RESURRECTION_BALM: case eItemAbil::RETURNING_MISSILE: case eItemAbil::SAPPHIRE:
			case eItemAbil::SEEKING_MISSILE: case eItemAbil::SKILL: case eItemAbil::SLAYER_WEAPON: case eItemAbil::SLOW_WEARER:
			case eItemAbil::SMOKY_CRYSTAL: case eItemAbil::SOULSUCKER: case eItemAbil::SPEED: case eItemAbil::STATUS_PROTECTION:
			case eItemAbil::STATUS_WEAPON: case eItemAbil::THIEVING: case eItemAbil::WEAK_WEAPON: case eItemAbil::WEAPON_CALL_SPECIAL:
			case eItemAbil::WORMGRASS: case eItemAbil::UNUSED: case eItemAbil::DROP_CALL_SPECIAL:
			case eItemAbil::HP_DAMAGE: case eItemAbil::HP_DAMAGE_REVERSE: case eItemAbil::SP_DAMAGE: case eItemAbil::SP_DAMAGE_REVERSE:
				break;
		}
	}
	
	put_pc_screen();
	if((take_charge) && (univ.party[pc].items[item].charges > 0))
		univ.party[pc].remove_charge(item);
	if(stat_window == pc)
		put_item_screen(stat_window);
	if(!take_charge) {
		draw_terrain(0);
		put_item_screen(stat_window);
	}
}

// Returns true if an action is actually carried out. This can only be reached in town.
bool use_space(location where) {
	ter_num_t ter;
	location from_loc,to_loc;
	
	ter = univ.town->terrain(where.x,where.y);
	from_loc = univ.party.town_loc;
	
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
		for(short i = 0; i < univ.town.items.size(); i++)
			if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where
			   && univ.town.items[i].contained && univ.town.items[i].held)
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
		for(short i = 0; i < univ.town.items.size(); i++)
			if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where
			   && univ.town.items[i].contained && univ.town.items[i].held)
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
		alter_space(where.x,where.y,univ.scenario.ter_types[ter].flag1);
		if(univ.scenario.ter_types[ter].flag2 >= 0)
			play_sound(univ.scenario.ter_types[ter].flag2);
		return true;
	} else if(univ.scenario.ter_types[ter].special == eTerSpec::CALL_SPECIAL_WHEN_USED) {
		short spec_type = 0;
		if(univ.scenario.ter_types[ter].flag2 == 1){
			if(is_town() || (is_combat() && which_combat_type == 1)) spec_type = 2;
			else spec_type = 1;
		}
		short i; // Dummy variable
		run_special(eSpecCtx::USE_SPACE,spec_type,univ.scenario.ter_types[ter].flag1,where,&i,&i,&i);
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
	short s1 = 0, s2 = 0, s3 = 0;
	
	for(short i = 0; i < univ.town.items.size(); i++)
		if(univ.town.items[i].variety != eItemType::NO_ITEM && (univ.town.items[i].contained) &&
		   (where == univ.town.items[i].item_loc))
			item_there = true;
	
	terrain = univ.town->terrain(where.x,where.y);
	if(univ.town.is_special(where.x,where.y)) {// && (get_blockage(terrain) > 0)) {
		if(!adjacent(univ.party.town_loc,where))
			add_string_to_buf("  Not close enough to search.");
		else {
			for(short i = 0; i < univ.town->special_locs.size(); i++)
				if(where == univ.town->special_locs[i]) {
					if(get_blockage(univ.town->terrain(where.x,where.y)) > 0) {
						// tell party you find something, if looking at a space they can't step in
						add_string_to_buf("  Search: You find something!");
					}
					
					run_special(eSpecCtx::TOWN_LOOK,2,univ.town->special_locs[i].spec,where,&s1,&s2,&s3);
					if(s1 > 0)
						can_open = false;
					got_special = true;
				}
			put_item_screen(stat_window);
		}
	}
	if(is_container(where) && item_there && can_open) {
		get_item(where,6,true);
	}else if(univ.scenario.ter_types[terrain].special == eTerSpec::CHANGE_WHEN_USED ||
			 univ.scenario.ter_types[terrain].special == eTerSpec::CALL_SPECIAL_WHEN_USED) {
		add_string_to_buf("  (Use this space to do something with it.)", 2);
	}else{
		if(!got_special)
			add_string_to_buf("  Search: You don't find anything.");
		return false;
	}
	return false;
}

void out_move_party(short x,short y) {
	location l;
	
	l.x = x;l.y = y;
	l = local_to_global(l);
	univ.party.out_loc = l;
	center = l;
	update_explored(l);
}

// mode - 0=full teleport flash 1=no teleport flash 2=only fade flash 3=only arrival flash
void teleport_party(short x,short y,short mode) {
	// TODO: Teleport sound? (Sound 10)
	location l;
	bool fadeIn = false, fadeOut = false;
	if(mode == 0 || mode == 2) fadeOut = true;
	if(mode == 0 || mode == 3) fadeIn = true;
	
	// Clear forcecage status
	for(int i = 0; i < 6; i++)
		univ.party[i].status[eStatus::FORCECAGE] = 0;
	
	if(is_combat())
		mode = 1;
	
	l = univ.party.town_loc;
	update_explored(l);
	
	if(fadeOut) {
		start_missile_anim();
		for(short i = 0; i < 9; i++)
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
	univ.party.town_loc.x = x;
	univ.party.town_loc.y = y;
	update_explored(l);
	draw_terrain(0);
	
	if(fadeIn) {
		start_missile_anim();
		for(short i = 0; i < 14; i++)
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
	location l;
	
	l = univ.party.town_loc;
	start_missile_anim();
	for(short i = 0; i < 14; i++)
		add_explosion(l,-1,1,1,0,0);
	do_explosion_anim(5,1);
	univ.party.town_loc.x = 100;
	univ.party.town_loc.y = 100;
	do_explosion_anim(5,2);
	end_missile_anim();
}

void change_level(short town_num,short x,short y) {
	location l(x,y);
	
	if((town_num < 0) || (town_num >= univ.scenario.towns.size())) {
		showError("The scenario special encounter tried to put you into a town that doesn't exist.");
		return;
	}
	
	// Clear forcecage status
	for(int i = 0; i < 6; i++)
		univ.party[i].status[eStatus::FORCECAGE] = 0;
	
	force_town_enter(town_num,l);
	end_town_mode(1,l);
	start_town_mode(town_num,9);
}


// Damaging and killing monsters needs to be here because several have specials attached to them.
bool damage_monst(cCreature& victim, short who_hit, short how_much, eDamageType dam_type, short sound_type, bool do_print) {
	short r1,which_spot;
	location where_put;
	
	//print_num(which_m,(short)univ.town.monst[which_m].m_loc.x,(short)univ.town.monst[which_m].m_loc.y);
	
	if(victim.active == 0) return false;
	
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
	
	if(dam_type < eDamageType::SPECIAL) {
		how_much *= victim.resist[dam_type];
		how_much /= 100;
	}
	
	// Absorb damage?
	if((dam_type == eDamageType::FIRE || dam_type == eDamageType::MAGIC || dam_type == eDamageType::COLD)
		&& victim.abil[eMonstAbil::ABSORB_SPELLS].active && get_ran(1,1,1000) <= victim.abil[eMonstAbil::ABSORB_SPELLS].special.extra1) {
		if(32767 - victim.health > how_much)
			victim.health = 32767;
		else victim.health += how_much;
		ASB("  Magic absorbed.");
		return false;
	}
	
	// Saving throw
	if((dam_type == eDamageType::FIRE || dam_type == eDamageType::COLD) && get_ran(1,0,20) <= victim.level)
		how_much /= 2;
	if(dam_type == eDamageType::MAGIC && (get_ran(1,0,24) <= victim.level))
		how_much /= 2;
	
	// Invulnerable?
	if(dam_type != eDamageType::SPECIAL && victim.invuln)
		how_much = how_much / 10;
	if(dam_type != eDamageType::SPECIAL && victim.status[eStatus::INVULNERABLE] > 0)
		how_much /= 10;
	
	// Mag. res helps w. fire and cold
	// TODO: Why doesn't this help with magic damage!?
	if(dam_type == eDamageType::FIRE || dam_type == eDamageType::COLD) {
		int magic_res = victim.status[eStatus::MAGIC_RESISTANCE];
		if(magic_res > 0)
			how_much /= 2;
		else if(magic_res < 0)
			how_much *= 2;
	}
	
	// TODO: So, player armour blocks demon/undead damage, but monster armour doesn't?
	if(dam_type == eDamageType::WEAPON) {
		r1 = get_ran(1,0,(victim.armor * 5) / 4);
		r1 += victim.level / 4;
		how_much -= r1;
	}
	
	if(boom_anim_active) {
		if(how_much < 0)
			how_much = 0;
		short boom_type = 2;
		if(dam_type == eDamageType::FIRE)
			boom_type = 0;
		else if(dam_type == eDamageType::UNBLOCKABLE)
			boom_type = 4;
		else if(dam_type == eDamageType::COLD)
			boom_type = 5;
		victim.marked_damage += how_much;
		add_explosion(victim.cur_loc,how_much,0,boom_type,14 * (victim.x_width - 1),18 * (victim.y_width - 1));
		// Note: Windows version printed an "undamaged" message here if applicable, but I don't think that's right.
		if(how_much == 0)
			return false;
		else return true;
	}
	
	if(how_much <= 0) {
		if(is_combat())
			victim.spell_note(7);
		if(how_much <= 0 && (dam_type == eDamageType::WEAPON || dam_type == eDamageType::UNDEAD || dam_type == eDamageType::DEMON)) {
			draw_terrain(2);
			play_sound(2);
		}
//		sprintf ((char *) create_line, "  No damage.              ");
//		add_string_to_buf((char *) create_line);
		return false;
	}
	
	if(do_print)
		victim.damaged_msg(how_much,0);
	victim.health = victim.health - how_much;
	
	if(univ.debug_mode)
		victim.health = -1;
	
	// splitting monsters
	if(victim.abil[eMonstAbil::SPLITS].active && victim.health > 0 && get_ran(1,1,1000) < victim.abil[eMonstAbil::SPLITS].special.extra1){
		where_put = find_clear_spot(victim.cur_loc,1);
		if(where_put.x > 0)
			if((which_spot = place_monster(victim.number,where_put)) < univ.town.monst.size()) {
				static_cast<cTownperson&>(univ.town.monst[which_spot]) = victim;
				univ.town.monst[which_spot].health = victim.health;
				victim.spell_note(27);
			}
	}
	if(who_hit < 7)
		univ.party.total_dam_done += how_much;
	
	// Monster damages. Make it hostile.
	victim.active = 2;
	
	if(dam_type != eDamageType::MARKED) {
		if(party_can_see_monst(univ.get_target_i(victim) - 100)) {
			boom_space(victim.cur_loc,100,boom_gr[dam_type],how_much,sound_type);
		}
		else {
			boom_space(victim.cur_loc,overall_mode, boom_gr[dam_type],how_much,sound_type);
		}
	}
	
	if(victim.health < 0) {
		victim.killed_msg();
		kill_monst(victim,who_hit);
	}
	else {
		if(how_much > 0)
			victim.morale = victim.morale - 1;
		if(how_much > 5)
			victim.morale = victim.morale - 1;
		if(how_much > 10)
			victim.morale = victim.morale - 1;
		if(how_much > 20)
			victim.morale = victim.morale - 2;
	}
	
	if(victim.is_friendly() && who_hit < 7 &&
		((!processing_fields && !monsters_going) || (processing_fields && !univ.party.hostiles_present))) {
		add_string_to_buf("Damaged an innocent.");
		victim.attitude = eAttitude::HOSTILE_A;
		make_town_hostile();
	}
	
	return true;
}

void petrify_monst(cCreature& which_m,int strength) {
	which_m.spell_note(9);
	short r1 = get_ran(1,0,20);
	r1 += which_m.level / 4;
	r1 += which_m.status[eStatus::BLESS_CURSE];
	r1 -= strength;
	
	// TODO: This should probably do something similar to charm_monst with the magic resistance
	if(r1 > 14 || which_m.resist[eDamageType::MAGIC] == 0)
		which_m.spell_note(10);
	else {
		which_m.spell_note(8);
		kill_monst(which_m,7,eMainStatus::STONE);
	}
}

void kill_monst(cCreature& which_m,short who_killed,eMainStatus type) {
	short xp,i,j,s1,s2,s3;
	location l;
	
	if(isHumanoid(which_m.m_type)) {
		if(which_m.m_type == eRace::GOBLIN)
			i = 4;
		else i = get_ran(1,0,1);
		play_sound(29 + i);
	} else switch(which_m.m_type) {
		case eRace::GIANT: play_sound(29); break;
			// TODO: Should birds be considered beasts? If there are any birds in the bladbase, probably; otherwise, better to have new sound
		case eRace::REPTILE: case eRace::BEAST: case eRace::DEMON: case eRace::UNDEAD: case eRace::SKELETAL: case eRace::STONE:
			i = get_ran(1,0,1); play_sound(31 + i); break;
			// TODO: I feel like dragons should have a different sound.
		default: play_sound(33); break;
	}
	
	// Special killing effects
	if(univ.party.sd_legit(which_m.spec1,which_m.spec2))
		PSD[which_m.spec1][which_m.spec2] = 1;
	
	if(which_m.special_on_kill >= 0)
		run_special(eSpecCtx::KILL_MONST,2,which_m.special_on_kill,which_m.cur_loc,&s1,&s2,&s3);
	if(which_m.abil[eMonstAbil::DEATH_TRIGGER].active)
		run_special(eSpecCtx::KILL_MONST,0,which_m.abil[eMonstAbil::DEATH_TRIGGER].special.extra1,which_m.cur_loc,&s1,&s2,&s3);
	
	if(!univ.debug_mode && (which_m.summon_time == 0 || !which_m.party_summoned)) { // no xp for party-summoned monsters
		xp = which_m.level * 2;
		if(who_killed < 6)
			award_xp(who_killed,xp);
		else if(who_killed == 6)
			award_party_xp(xp / 6 + 1);
		if(who_killed < 7) {
			univ.party.total_m_killed++;
			i = max((xp / 6),1);
			award_party_xp(i);
		}
		l = which_m.cur_loc;
		place_glands(l,which_m.number);
		
	}
	if(!univ.debug_mode && which_m.summon_time == 0)
		place_treasure(which_m.cur_loc, which_m.level / 2, which_m.treasure, 0);
	
	i = which_m.cur_loc.x;
	j = which_m.cur_loc.y;
	if(type == eMainStatus::DUST)
		univ.town.set_ash(i,j,true);
	else if(type == eMainStatus::ABSENT || type == eMainStatus::STONE);
	else switch(which_m.m_type) {
		case eRace::DEMON:
			univ.town.set_ash(i,j,true);
			break;
		case eRace::UNDEAD:
			break;
		case eRace::SKELETAL:
			univ.town.set_bones(i,j,true);
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
	
	if((is_town() || which_combat_type == 1) && which_m.summon_time == 0) {
		univ.town->m_killed++;
	}
	
	which_m.spec1 = 0; // make sure, if this is a spec. activated monster, it won't come back
	
	which_m.active = 0;
}

// Pushes party and monsters around by moving walls and conveyor belts.
// This is very fragile, and only hands a few cases.
void push_things() {
	bool redraw = false;
	ter_num_t ter;
	location l;
	
	if(is_out()) // TODO: Make these work outdoors
		return;
	if(!univ.town.belt_present)
		return;
	
	for(short i = 0; i < univ.town.monst.size(); i++)
		if(univ.town.monst[i].active > 0) {
			l = univ.town.monst[i].cur_loc;
			ter = univ.town->terrain(l.x,l.y);
			switch(univ.scenario.ter_types[ter].flag1) { // TODO: Implement the other 4 possible directions
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
	for(short i = 0; i < univ.town.items.size(); i++)
		if(univ.town.items[i].variety != eItemType::NO_ITEM) {
			l = univ.town.items[i].item_loc;
			ter = univ.town->terrain(l.x,l.y);
			switch(univ.scenario.ter_types[ter].flag1) { // TODO: Implement the other 4 possible directions
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
		ter = univ.town->terrain(univ.party.town_loc.x,univ.party.town_loc.y);
		l = univ.party.town_loc;
		switch(univ.scenario.ter_types[ter].flag1) { // TODO: Implement the other 4 possible directions
			case DIR_N: l.y--; break;
			case DIR_E: l.x++; break;
			case DIR_S: l.y++; break;
			case DIR_W: l.x--; break;
		}
		if(l != univ.party.town_loc) {
			// TODO: Will this push you into a placed forcecage or barrier? Should it?
			ASB("You get pushed.");
			if(univ.scenario.ter_types[ter].special == eTerSpec::CONVEYOR)
				draw_terrain(0);
			center = l;
			univ.party.town_loc = l;
			update_explored(l);
			ter = univ.town->terrain(univ.party.town_loc.x,univ.party.town_loc.y);
			draw_map(true);
			if(univ.town.is_barrel(univ.party.town_loc.x,univ.party.town_loc.y)) {
				univ.town.set_barrel(univ.party.town_loc.x,univ.party.town_loc.y,false);
				ASB("You smash the barrel.");
			}
			if(univ.town.is_crate(univ.party.town_loc.x,univ.party.town_loc.y)) {
				univ.town.set_crate(univ.party.town_loc.x,univ.party.town_loc.y,false);
				ASB("You smash the crate.");
			}
			if(univ.town.is_block(univ.party.town_loc.x,univ.party.town_loc.y)) {
				ASB("You crash into the block.");
				hit_party(get_ran(1, 1, 6), eDamageType::WEAPON);
			}
			for(short k = 0; k < univ.town.items.size(); k++)
				if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].held
				   && (univ.town.items[k].item_loc == univ.party.town_loc))
					univ.town.items[k].contained = univ.town.items[k].held = false;
			redraw = true;
		}
	}
	if(is_combat()) {
		for(short i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE) {
				ter = univ.town->terrain(univ.party[i].combat_pos.x,univ.party[i].combat_pos.y);
				l = univ.party[i].combat_pos;
				switch(univ.scenario.ter_types[ter].flag1) { // TODO: Implement the other 4 possible directions
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
					if(univ.town.is_block(univ.party[i].combat_pos.x,univ.party[i].combat_pos.y)) {
						ASB("You crash into the block.");
						damage_pc(univ.party[i],get_ran(1, 1, 6), eDamageType::WEAPON,eRace::UNKNOWN,0);
					}
					for(short k = 0; k < univ.town.items.size(); k++)
						if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].held
						   && (univ.town.items[k].item_loc == univ.party[i].combat_pos))
							univ.town.items[k].contained = univ.town.items[k].held = false;
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
	short s1,s2,s3;
	bool redraw = false,stat_area = false;
	location trigger_loc;
	unsigned long age_before = univ.party.age - length;
	unsigned long current_age = univ.party.age;
	bool failed_job = false;
	
	if(is_combat()) {
		extern short combat_active_pc;
		trigger_loc = univ.party[combat_active_pc].combat_pos;
	} else if(is_town()) {
		trigger_loc = univ.party.town_loc;
	} else if(is_out()) {
		trigger_loc = univ.party.out_loc;
	}
	
	for(auto& p : univ.party.quest_status) {
		if(p.second != eQuestStatus::STARTED)
			continue;
		cQuest& quest = univ.scenario.quests[p.first];
		if(quest.deadline <= 0)
			continue;
		bool is_relative = quest.flags % 10;
		int deadline = quest.deadline + is_relative * univ.party.quest_start[p.first];
		if(day_reached(deadline + 1, quest.event)) {
			p.second = eQuestStatus::FAILED;
			if(univ.party.quest_source[p.first] >= 0) {
				int bank = univ.party.quest_source[p.first];
				// Safety valve in case it was given by a special node
				if(bank >= univ.party.job_banks.size())
					univ.party.job_banks.resize(bank + 1);
				int add_anger = 1;
				if(quest.flags % 10 == 1) {
					if(quest.deadline < 20)
						add_anger++;
					if(quest.deadline < 10)
						add_anger++;
					if(quest.deadline < 5)
						add_anger++;
				} else if(quest.deadline - univ.party.quest_start[p.first] > 20)
					add_anger++;
				univ.party.job_banks[bank].anger += add_anger;
			}
			failed_job = true;
		}
	}
	if(failed_job) {
		add_string_to_buf("The deadline for one of your quests has passed.",2);
		print_buf();
		if(stat_window == ITEM_WIN_QUESTS)
			set_stat_window(stat_window);
	}
	
	// Angered job boards slowly forgive you
	if(univ.party.age % 30 == 0)
		for(short i = 0; i < univ.party.job_banks.size(); i++)
			move_to_zero(univ.party.job_banks[i].anger);
	
	if(is_town() || (is_combat() && which_combat_type == 1)) {
		for(short i = 0; i < univ.town->timers.size(); i++)
			if(univ.town->timers[i].time > 0) {
				short time = univ.town->timers[i].time;
				for(unsigned long j = age_before + (time == 1); j <= current_age; j++)
					if(j % time == 0) {
						if(queue) {
							univ.party.age = j;
							queue_special(eSpecCtx::TOWN_TIMER, 2, univ.town->timers[i].node, trigger_loc);
						} else run_special(eSpecCtx::TOWN_TIMER,2,univ.town->timers[i].node,trigger_loc,&s1,&s2,&s3);
					}
				stat_area = true;
				if(s3 > 0)
					redraw = true;
			}
	}
	univ.party.age = current_age;
	for(short i = 0; i < univ.scenario.scenario_timers.size(); i++)
		if(univ.scenario.scenario_timers[i].time > 0) {
			short time = univ.scenario.scenario_timers[i].time;
			for(unsigned long j = age_before + (time == 1); j <= current_age; j++)
				if(j % time == 0) {
					if(queue) {
						univ.party.age = j;
						queue_special(eSpecCtx::SCEN_TIMER, 0, univ.scenario.scenario_timers[i].node, trigger_loc);
					} else run_special(eSpecCtx::SCEN_TIMER,0,univ.scenario.scenario_timers[i].node,trigger_loc,&s1,&s2,&s3);
				}
			stat_area = true;
			if(s3 > 0)
				redraw = true;
		}
	univ.party.age = current_age;
	auto party_timers = univ.party.party_event_timers;
	for(short i = 0; i < party_timers.size(); i++) {
		if(party_timers[i].time <= length) {
			univ.party.age = age_before + party_timers[i].time;
			short which_type = party_timers[i].node_type;
			if(queue)
				queue_special(eSpecCtx::PARTY_TIMER, which_type, party_timers[i].node, trigger_loc);
			else run_special(eSpecCtx::PARTY_TIMER, which_type, party_timers[i].node, trigger_loc, &s1, &s2, &s3);
			univ.party.party_event_timers[i].time = 0;
			univ.party.party_event_timers[i].node = -1;
			stat_area = true;
			if(s3 > 0)
				redraw = true;
		} else univ.party.party_event_timers[i].time -= length;
	}
	univ.party.age = current_age;
	if(stat_area) {
		put_pc_screen();
		put_item_screen(stat_window);
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
	current_pc_picked_in_spec_enc = nullptr;
	switch(which_mode) {
		case eSpecCtx::OUT_MOVE: case eSpecCtx::TOWN_MOVE: case eSpecCtx::COMBAT_MOVE:
		case eSpecCtx::OUT_LOOK: case eSpecCtx::TOWN_LOOK: case eSpecCtx::ENTER_TOWN: case eSpecCtx::LEAVE_TOWN:
		case eSpecCtx::TALK: case eSpecCtx::USE_SPEC_ITEM: case eSpecCtx::TOWN_HOSTILE:
		case eSpecCtx::TOWN_TIMER: case eSpecCtx::SCEN_TIMER: case eSpecCtx::PARTY_TIMER:
		case eSpecCtx::OUTDOOR_ENC: case eSpecCtx::FLEE_ENCOUNTER: case eSpecCtx::WIN_ENCOUNTER:
		case eSpecCtx::DROP_ITEM: case eSpecCtx::SHOPPING: case eSpecCtx::STARTUP:
			// Default behaviour - select entire party, or active member if split or in combat
			// We also have a legacy flag - originally, it always defaulted to whole party
			if(is_combat() && !univ.scenario.is_legacy)
				current_pc_picked_in_spec_enc = &univ.party[current_pc];
			else {
				if(univ.party.is_split() && cur_node.type != eSpecType::AFFECT_DEADNESS)
					current_pc_picked_in_spec_enc = &univ.party.pc_present();
				else current_pc_picked_in_spec_enc = &univ.party;
			}
			break;
		case eSpecCtx::KILL_MONST: case eSpecCtx::SEE_MONST: case eSpecCtx::MONST_SPEC_ABIL:
		case eSpecCtx::ATTACKED_MELEE: case eSpecCtx::ATTACKING_MELEE:
		case eSpecCtx::ATTACKED_RANGE: case eSpecCtx::ATTACKING_RANGE:
			if(univ.scenario.is_legacy) {
				current_pc_picked_in_spec_enc = &univ.party;
				break;
			}
			// The monster/PC on the trigger space is the target
			current_pc_picked_in_spec_enc = univ.target_there(spec_loc);
			if(!current_pc_picked_in_spec_enc)
				current_pc_picked_in_spec_enc = &univ.party;
			break;
		case eSpecCtx::TARGET: case eSpecCtx::USE_SPACE: case eSpecCtx::HAIL:
			// If there's a monster on the space, select that as the target
			current_pc_picked_in_spec_enc = univ.target_there(spec_loc, TARG_MONST);
			if(!current_pc_picked_in_spec_enc)
				current_pc_picked_in_spec_enc = &univ.party;
			break;
	}
	if(end_scenario) {
		special_in_progress = false;
		return;
	}
	
	// Store the special's location in reserved pointers
	univ.party.force_ptr(10, spec_loc.x);
	univ.party.force_ptr(11, spec_loc.y);
	// Also store the terrain type on that location
	univ.party.force_ptr(12, coord_to_ter(spec_loc.x, spec_loc.y));

	
	while(next_spec >= 0) {
		
		cur_spec = next_spec;
		cur_spec_type = next_spec_type;
		next_spec = -1;
		cur_node = get_node(cur_spec,cur_spec_type);
		
		if(univ.node_step_through) {
			give_help(68,69);
			std::string debug = "Step: ";
			debug += (*cur_node.type).name();
			debug += " - ";
			debug += std::to_string(cur_spec);
			add_string_to_buf(debug);
			redraw_screen(REFRESH_TRANS);
			sf::Event evt;
			while(true) {
				if(mainPtr.pollEvent(evt) && (evt.type == sf::Event::KeyPressed || evt.type == sf::Event::MouseButtonPressed))
					break;
			}
			if(evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Escape)
				univ.node_step_through = false;
		}
		
		// Convert pointer values to reference values
		// TODO: Might need to make a database of which nodes don't allow pointers in which slots.
		// This is because some nodes now use -2 as a meaningful value. If that's all, then
		// just disallowing single-digit pointers should suffice, but what about arithmetic?
		// (Of course, currently all SDFs are positive, so allowing negative arithmetic is useless.)
		if(cur_node.sd1 <= -10) cur_node.sd1 = univ.party.get_ptr(-cur_node.sd1);
		if(cur_node.sd2 <= -10) cur_node.sd2 = univ.party.get_ptr(-cur_node.sd2);
		if(cur_node.m1 <= -10) cur_node.m1 = univ.party.get_ptr(-cur_node.m1);
		if(cur_node.m2 <= -10) cur_node.m2 = univ.party.get_ptr(-cur_node.m2);
		if(cur_node.m3 <= -10) cur_node.m3 = univ.party.get_ptr(-cur_node.m3);
		if(cur_node.pic <= -10) cur_node.pic = univ.party.get_ptr(-cur_node.pic);
		if(cur_node.pictype <= -10) cur_node.pictype = univ.party.get_ptr(-cur_node.pictype);
		if(cur_node.ex1a <= -10) cur_node.ex1a = univ.party.get_ptr(-cur_node.ex1a);
		if(cur_node.ex1b <= -10) cur_node.ex1b = univ.party.get_ptr(-cur_node.ex1b);
		if(cur_node.ex1c <= -10) cur_node.ex1c = univ.party.get_ptr(-cur_node.ex1c);
		if(cur_node.ex2a <= -10) cur_node.ex2a = univ.party.get_ptr(-cur_node.ex2a);
		if(cur_node.ex2b <= -10) cur_node.ex2b = univ.party.get_ptr(-cur_node.ex2b);
		if(cur_node.ex2c <= -10) cur_node.ex2c = univ.party.get_ptr(-cur_node.ex2c);
		if(cur_node.jumpto <= -10) cur_node.jumpto = univ.party.get_ptr(-cur_node.jumpto);
		
		//print_nums(1111,cur_spec_type,cur_node.type);
		
		if(cur_node.type == eSpecType::INVALID) {
			special_in_progress = false;
			return;
		}
		switch(getNodeCategory(cur_node.type)) {
			case eSpecCat::GENERAL:
				if(cur_node.type == eSpecType::NONE && univ.debug_mode) {
					std::string type("???");
					switch(cur_spec_type) {
						case 0: type = "scenario"; break;
						case 1: type = "outdoors" ; break;
						case 2: type = "town"; break;
					}
					add_string_to_buf("Warning: Null " + type + " special called (ID " + std::to_string(cur_spec) + ") - was this intended?", 4);
				}
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
			add_string_to_buf("SPECIAL ENCOUNTER INTERRUPTED.", 3);
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
	dummy_node.type = eSpecType::INVALID;
	if(cur_spec_type == 0) {
		if(cur_spec != minmax(0,univ.scenario.scen_specials.size() - 1,cur_spec)) {
			showError("The scenario called a scenario special node out of range.");
			return dummy_node;
		}
		return univ.scenario.scen_specials[cur_spec];
	}
	if(cur_spec_type == 1) {
		if(cur_spec != minmax(0,univ.out->specials.size() - 1,cur_spec)) {
			showError("The scenario called an outdoor special node out of range.");
			return dummy_node;
		}
		return univ.out->specials[cur_spec];
	}
	if(cur_spec_type == 2) {
		if(cur_spec != minmax(0,univ.town->specials.size() - 1,cur_spec)) {
			showError("The scenario called a town special node out of range.");
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
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	switch(cur_node.type) {
		case eSpecType::NONE: break; // null spec
		case eSpecType::SET_SDF:
			check_mess = true;
			setsd(cur_node.sd1,cur_node.sd2,cur_node.ex1a);
			break;
		case eSpecType::INC_SDF:
			check_mess = true;
			setsd(cur_node.sd1,cur_node.sd2,
				  PSD[cur_node.sd1][cur_node.sd2] + ((cur_node.ex1b == 0) ? 1 : -1) * cur_node.ex1a);
			break;
		case eSpecType::DISPLAY_MSG:
			check_mess = true;
			break;
		case eSpecType::TITLED_MSG:
			get_strs(str1,str2, cur_spec_type, cur_node.m3, -1);
			handle_message(which_mode, cur_spec_type, cur_node.m1, cur_node.m2, a, b, str1, cur_node.pic, ePicType(cur_node.pictype));
			break;
		case eSpecType::DISPLAY_SM_MSG:
			get_strs(str1,str2, cur_spec_type,cur_node.m1,cur_node.m2);
			if(cur_node.m1 >= 0)
				add_string_to_buf(str1, 4);
			if(cur_node.m2 >= 0)
				add_string_to_buf(str2, 4);
			break;
		case eSpecType::FLIP_SDF:
			setsd(cur_node.sd1,cur_node.sd2,
				  ((PSD[cur_node.sd1][cur_node.sd2] == 0) ? 1 : 0) );
			check_mess = true;
			break;
		case eSpecType::CANT_ENTER:
			check_mess = true;
			if(which_mode == eSpecCtx::TALK) {
				extern bool talk_end_forced;
				talk_end_forced = spec.ex1a;
			} else if(spec.ex1a != 0)
				*a = 1;
			else {
				*a = 0;
				if(spec.ex2a != 0) *b = 1;
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
			if(spec.ex1a != minmax(0,univ.party.horses.size() - 1,spec.ex1a))
				showError("Horse out of range.");
			else univ.party.horses[spec.ex1a].property = (spec.ex2a == 0) ? 1 : 0;
			break;
		case eSpecType::CHANGE_BOAT_OWNER:
			check_mess = true;
			if(spec.ex1a != minmax(0,univ.party.boats.size() - 1,spec.ex1a))
				showError("Boat out of range.");
			else univ.party.boats[spec.ex1a].property = (spec.ex2a == 0) ? 1 : 0;
			break;
		case eSpecType::SET_TOWN_VISIBILITY:
			check_mess = true;
			if(spec.ex1a != minmax(0,univ.scenario.towns.size() - 1,spec.ex1a))
				showError("Town out of range.");
			else univ.scenario.towns[spec.ex1a]->can_find = spec.ex2a;
			*redraw = true;
			break;
		case eSpecType::MAJOR_EVENT_OCCURRED:
			check_mess = true;
			if(spec.ex1a != minmax(1,10,spec.ex1a))
				showError("Event code out of range.");
			else if(univ.party.key_times.count(spec.ex1a) == 0)
				univ.party.key_times[spec.ex1a] = univ.party.calc_day();
			break;
		case eSpecType::FORCED_GIVE:
			check_mess = true;
			if(spec.ex1a < 0 || spec.ex1a >= univ.scenario.scen_items.size())
				break;
			if(!univ.party.forced_give(univ.scenario.scen_items[spec.ex1a],eItemAbil::NONE) && spec.ex1b >= 0)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::BUY_ITEMS_OF_TYPE:
			for(short i = 0; i < 144; i++)
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
				showError("Stuff Done flag out of range.");
			else for(short i = 0; i < 50; i++) PSD[spec.sd1][i] = spec.ex1a;
			break;
		case eSpecType::COPY_SDF:
			if(!univ.party.sd_legit(spec.sd1,spec.sd2) || !univ.party.sd_legit(spec.ex1a,spec.ex1b))
				showError("Stuff Done flag out of range.");
			else PSD[spec.sd1][spec.sd2] = PSD[spec.ex1a][spec.ex1b];
			break;
		case eSpecType::REST:
			check_mess = true;
			do_rest(spec.ex1a, spec.ex1b, spec.ex1b);
			break;
		case eSpecType::END_SCENARIO:
			end_scenario = true;
			break;
		case eSpecType::SET_POINTER:
			if(spec.ex1a < 0)
				showError("Attempted to assign a pointer out of range (100..199)");
			else try {
				if(spec.sd1 < 0 && spec.sd2 < 0)
					univ.party.clear_ptr(spec.ex1a);
				else univ.party.set_ptr(spec.ex1a,spec.sd1,spec.sd2);
			} catch(std::range_error x) {
				showError(x.what());
			}
			break;
		case eSpecType::SET_CAMP_FLAG:
			if(!univ.party.sd_legit(spec.sd1,spec.sd2))
				showError("Stuff Done flag out of range (x - 0..299, y - 0..49).");
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
					if(univ.party.sd_legit(spec.sd1, spec.sd2))
						setsd(spec.sd1, spec.sd2, i / j);
					if(univ.party.sd_legit(spec.ex1c, spec.ex2c))
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
			if(!univ.debug_mode) break;
			check_mess = false;
			get_strs(str1,str2, cur_spec_type,cur_node.m1, cur_node.m2);
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
					if(spec.ex1a >= univ.town.monst.size()) break;
					print_nums(spec.ex1a, univ.town.monst[spec.ex1a].health, univ.town.monst[spec.ex1a].mp);
					break;
			}
			break;
		case eSpecType::CHANGE_TER:
			alter_space(spec.ex1a,spec.ex1b,spec.ex2a);
			*redraw = true;
			draw_map(true);
			check_mess = true;
			break;
		case eSpecType::SWAP_TER:
			swap_ter(spec.ex1a,spec.ex1b,spec.ex2a,spec.ex2b);
			*redraw = 1;
			draw_map(true);
			check_mess = true;
			break;
		case eSpecType::TRANS_TER:
			alter_space(spec.ex1a,spec.ex1b,univ.scenario.ter_types[coord_to_ter(spec.ex1a,spec.ex1b)].trans_to_what);
			*redraw = 1;
			draw_map(true);
			check_mess = true;
			break;
		case eSpecType::ENTER_SHOP:
			get_strs(str1,str2,1,spec.m1,-1);
			spec.ex1b = minmax(0,6,spec.ex2b);
			start_shop_mode(spec.ex1a, spec.ex1b, str1);
			*next_spec = -1;
			break;
		case eSpecType::STORY_DIALOG:
			get_strs(str1,str2,cur_spec_type,spec.m1,-1);
			story_dialog(str1, spec.m2, spec.m3, cur_spec_type, spec.pic, ePicType(spec.pictype));
			break;
		case eSpecType::CLEAR_BUF:
			univ.get_buf().clear();
			break;
		case eSpecType::APPEND_STRING:
			get_strs(str1,str1,cur_spec_type,spec.ex1a,-1);
			if(spec.pic) univ.get_buf() += ' ';
			univ.get_buf() += str1;
			break;
		case eSpecType::APPEND_NUM:
			if(spec.pic) univ.get_buf() += ' ';
			univ.get_buf() += std::to_string(spec.ex1a);
			break;
		case eSpecType::APPEND_MONST:
			if(spec.pic) univ.get_buf() += ' ';
			if(spec.ex1a == 0) {
				int pc = univ.get_target_i(*current_pc_picked_in_spec_enc);
				if(pc == 6)
					univ.get_buf() += "Your party";
				else if(pc < 100)
					univ.get_buf() += univ.party[pc].name;
				else if(!is_out())
					univ.get_buf() += univ.town.monst[pc - 100].m_name;
			} else univ.get_buf() += univ.scenario.scen_monsters[spec.ex1a].m_name;
			break;
		case eSpecType::APPEND_ITEM:
			if(spec.pic) univ.get_buf() += ' ';
			if(spec.ex1b == 1)
				univ.get_buf() += univ.scenario.scen_items[spec.ex1a].full_name;
			else if(spec.ex1b == 2)
				univ.get_buf() += get_item_interesting_string(univ.scenario.scen_items[spec.ex1a]);
			else univ.get_buf() += univ.scenario.scen_items[spec.ex1a].name;
			break;
		case eSpecType::APPEND_TER:
			if(spec.pic) univ.get_buf() += ' ';
			univ.get_buf() += univ.scenario.ter_types[spec.ex1a].name;
			break;
		case eSpecType::SWAP_STR_BUF:
			univ.swap_buf(spec.ex1a);
			break;
		case eSpecType::STR_BUF_TO_SIGN:
			if(spec.ex1a < 0) break;
			if(is_out()) {
				if(spec.ex1a >= univ.out->sign_locs.size()) break;
				std::swap(univ.out->sign_locs[spec.ex1a].text, univ.get_buf());
			} else {
				if(spec.ex1a >= univ.town->sign_locs.size()) break;
				std::swap(univ.town->sign_locs[spec.ex1a].text, univ.get_buf());
			}
			break;
		case eSpecType::PAUSE:
			if(spec.ex1a < 0) break;
			redraw_screen(REFRESH_TERRAIN | REFRESH_STATS);
			sf::sleep(sf::milliseconds(spec.ex1a));
			break;
		case eSpecType::START_TALK:
			i = univ.get_target_i(*current_pc_picked_in_spec_enc);
			if(i >= 100) i -= 100;
			else i = -1;
			start_talk_mode(i, spec.ex1a, spec.ex1b, spec.pic);
			*next_spec = -1;
			break;
		case eSpecType::UPDATE_QUEST:
			check_mess = true;
			if(spec.ex1a < 0 || spec.ex1a >= univ.scenario.quests.size()) {
				showError("The scenario tried to update a non-existent quest.");
				break;
			}
			if(spec.ex1b < 0 || spec.ex1b > 3) {
				showError("Invalid quest status (range 0 .. 3).");
				break;
			}
			if(spec.ex1b == int(eQuestStatus::STARTED) && univ.party.quest_status[spec.ex1a] != eQuestStatus::STARTED) {
				univ.party.quest_start[spec.ex1a] = univ.party.calc_day();
				univ.party.quest_source[spec.ex1a] = max(-1,spec.ex2a);
				if(univ.party.quest_source[spec.ex1a] >= univ.party.job_banks.size())
					univ.party.job_banks.resize(univ.party.quest_source[spec.ex1a] + 1);
			}
			univ.party.quest_status[spec.ex1a] = eQuestStatus(spec.ex1b);
			switch(univ.party.quest_status[spec.ex1a]) {
				case eQuestStatus::STARTED: add_string_to_buf("You have received a quest."); break;
				case eQuestStatus::AVAILABLE: break; // TODO: Should this award XP/gold if the quest was previously started?
				case eQuestStatus::FAILED:
					add_string_to_buf("You have failed to complete a quest.");
					if(univ.party.quest_source[spec.ex1a] >= 0 && univ.party.quest_source[spec.ex1a] < univ.party.job_banks.size())
						univ.party.job_banks[univ.party.quest_source[spec.ex1a]].anger += spec.ex2a < 0 ? 1 : spec.ex2a;
					break;
				case eQuestStatus::COMPLETED:
					add_string_to_buf("You have completed a quest!");
					if(univ.scenario.quests[spec.ex1a].gold > 0) {
						int gold = univ.scenario.quests[spec.ex1a].gold;
						add_string_to_buf("  Received " + std::to_string(gold) + " as a reward.");
						give_gold(gold, true);
					}
					if(univ.scenario.quests[spec.ex1a].xp > 0)
						award_party_xp(univ.scenario.quests[spec.ex1a].xp);
					break;
			}
			break;
		default:
			showError("Special node type \"" + (*cur_node.type).name() + "\" is either miscategorized or unimplemented!");
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

void oneshot_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
				  short* next_spec,short* next_spec_type,short* a,short* b,short* redraw) {
	bool check_mess = true,set_sd = true;
	std::array<std::string, 6> strs;
	std::array<short, 3> buttons = {-1,-1,-1};
	cSpecial spec;
	cItem store_i;
	location l;
	int dlg_res;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	if((univ.party.sd_legit(spec.sd1,spec.sd2)) && (PSD[spec.sd1][spec.sd2] == 250)) {
		*next_spec = -1;
		return;
	}
	switch(cur_node.type) {
		case eSpecType::ONCE_GIVE_ITEM:
			if(spec.ex2b >= 0 && spec.ex2b < univ.scenario.scen_items.size() &&
					!univ.party.forced_give(univ.scenario.scen_items[spec.ex1a],eItemAbil::NONE)) {
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
				showError("Special item is out of range.");
				set_sd = false;
			}
			else if(spec.ex1b == 0)
				univ.party.spec_items.insert(spec.ex1a);
			else univ.party.spec_items.erase(spec.ex1a);
			if(stat_window == ITEM_WIN_SPECIAL)
				set_stat_window(ITEM_WIN_SPECIAL);
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
			for(short i = 0; i < 3; i++)
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
				showError("Dialog box ended up with no buttons.");
				break;
			}
			dlg_res = custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons);
			if(spec.m3 > 0) {
				if(dlg_res == 1) {
					if((spec.ex1a >= 0) || (spec.ex2a >= 0)) {
						set_sd = false;
					}
				}
				if(dlg_res == 2) *next_spec = spec.ex1b;
				if(dlg_res == 3) *next_spec = spec.ex2b;
			}
			else {
				if(dlg_res == 1) *next_spec = spec.ex1b;
				if(dlg_res == 2) *next_spec = spec.ex2b;
			}
			break;
		case eSpecType::ONCE_GIVE_ITEM_DIALOG:
			check_mess = false;
			if(spec.m1 < 0)
				break;
			for(short i = 0; i < 3; i++)
				get_strs(strs[i * 2],strs[i * 2 + 1],cur_spec_type, spec.m1 + i * 2,spec.m1 + i * 2 + 1);
			buttons[0] = 20; buttons[1] = 19;
			dlg_res = custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons);
			if(dlg_res == 1) {set_sd = false; *next_spec = -1;}
			else {
				store_i = univ.scenario.get_stored_item(spec.ex1a);
				if((spec.ex1a >= 0) && (!univ.party.give_item(store_i,true))) {
					set_sd = false; *next_spec = -1;
				}
				else {
					give_gold(spec.ex1b,true);
					give_food(spec.ex2a,true);
					if((spec.m3 >= 0) && (spec.m3 < 50)) {
						if(!univ.party.spec_items.count(spec.m3))
							ASB("You get a special item.");
						univ.party.spec_items.insert(spec.m3);
						*redraw = true;
						if(stat_window == ITEM_WIN_SPECIAL)
							set_stat_window(ITEM_WIN_SPECIAL);
					}
					if(spec.ex2b >= 0) *next_spec = spec.ex2b;
				}
			}
			break;
		case eSpecType::ONCE_OUT_ENCOUNTER:
			if(spec.ex1a != minmax(0,3,spec.ex1a)) {
				showError("Special outdoor enc. is out of range. Must be 0-3.");
				set_sd = false;
			}
			else {
				l = global_to_local(univ.party.out_loc);
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
				dlg_res = custom_choice_dialog(strs,spec.pic,ePicType(spec.pictype),buttons);
				// TODO: Make custom_choice_dialog return string?
			}
			else dlg_res = cChoiceDlog("basic-trap",{"yes","no"}).show() == "no";
			if(dlg_res == 1) {
				set_sd = false;
				*next_spec = -1;
				*a = 1;
			} else {
				if(!is_combat()) {
					dlg_res = char_select_pc(0,"Trap! Who will disarm?");
					if(dlg_res == 6){
						*a = 1;
						set_sd = false;
					}
				} else dlg_res = current_pc;
				bool disarmed = run_trap(dlg_res,eTrapType(spec.ex1a),spec.ex1b,spec.ex2a);
				if(!disarmed && spec.ex1a == TRAP_CUSTOM) {
					if(spec.jumpto >= 0)
						queue_special(which_mode, cur_spec_type, spec.jumpto, loc(univ.party.get_ptr(10), univ.party.get_ptr(11)));
					*next_spec = spec.ex2b;
					*next_spec_type = 0;
				}
			}
			break;
		case eSpecType::ONCE_DISPLAY_MSG:
			break; // Nothing to do here, but need to include it to prevent the below error from showing.
		default:
			showError("Special node type \"" + (*cur_node.type).name() + "\" is either miscategorized or unimplemented!");
			break;
	}
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
	if((set_sd) && (univ.party.sd_legit(spec.sd1,spec.sd2)))
		PSD[spec.sd1][spec.sd2] = 250;
	
}

void affect_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
				 short *next_spec,short* /*next_spec_type*/,short *a,short *b,short *redraw) {
	bool check_mess = true;
	short r1;
	iLiving* pc = current_pc_picked_in_spec_enc;
	short pc_num = univ.get_target_i(*current_pc_picked_in_spec_enc);
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
				int i;
				if(spec.ex1a == 2)
					current_pc_picked_in_spec_enc = &univ.party;
				else if(spec.ex1a == 1) {
					i = select_pc(0);
					if(i != 6)
						current_pc_picked_in_spec_enc = &univ.party[i];
				}
				else if(spec.ex1a == 0) {
					i = select_pc(1);
					if(i != 6)
						current_pc_picked_in_spec_enc = &univ.party[i];
				}
				else if(spec.ex1a == 3) {
					i = select_pc(2);
					if(i != 6)
						current_pc_picked_in_spec_enc = &univ.party[i];
				}
				else if(spec.ex1a == 4) {
					i = select_pc(3);
					if(i != 6)
						current_pc_picked_in_spec_enc = &univ.party[i];
				}
				if(i == 6)// && (spec.ex1b >= 0))
					*next_spec = spec.ex1b;
				
			}
			else if(spec.ex2a == 2) {
				// Select a specific PC
				short pc = spec.ex2b;
				bool can_pick = true;
				if(pc >= 0 && pc < 6) {
					// Honour the request for alive PCs only.
					if(univ.party[pc].main_status == eMainStatus::ABSENT)
						can_pick = false;
					else if(spec.ex1a % 4 == 0 && univ.party[pc].main_status != eMainStatus::ALIVE)
						can_pick = false;
					else if(spec.ex1a == 3 && univ.party[pc].main_status == eMainStatus::ALIVE)
						can_pick = false;
					else if(spec.ex1a == 4 && !univ.party[pc].has_space())
						can_pick = false;
				} else if(pc >= 100 && pc < univ.town.monst.size() + 100) {
					short monst = pc - 100;
					// Honour the request for alive only
					if(spec.ex1a == 0 && univ.town.monst[monst].active == 0)
						can_pick = false;
					else if(spec.ex1a == 3 && univ.town.monst[monst].active > 0)
						can_pick = false;
				} else if(pc >= 1000 || pc == -1) { // Select PC by unique ID; might be a stored PC rather than a party member
					if(pc == -1) pc = 1000 + spec.ex2c;
					can_pick = false; // Assume ID is invalid unless proven otherwise
					cPlayer* found = nullptr;
					for(short i = 0; i < 6; i++) {
						if(univ.party[i].unique_id == pc) {
							can_pick = true;
							found = &univ.party[i];
							break;
						}
					}
					if(!can_pick) {
						for(auto& p : univ.stored_pcs) {
							if(p.first == pc && p.second->unique_id == pc) {
								can_pick = true;
								found = p.second;
								break;
							}
						}
					}
					if(can_pick) {
						// Honour the request for alive PCs only.
						if(found->main_status == eMainStatus::ABSENT)
							can_pick = false;
						else if(spec.ex1a % 4 == 0 && found->main_status != eMainStatus::ALIVE)
							can_pick = false;
						else if(spec.ex1a == 3 && found->main_status == eMainStatus::ALIVE)
							can_pick = false;
						else if(spec.ex1a == 4 && !found->has_space())
							can_pick = false;
					}
					if(can_pick)
						current_pc_picked_in_spec_enc = found;
					else *next_spec = spec.ex1b;
					break; // To avoid the call to get_target
				} else can_pick = false; // Because it's an invalid index
				if(can_pick)
					current_pc_picked_in_spec_enc = &univ.get_target(pc);
				else *next_spec = spec.ex1b;
			} else if(spec.ex2a == 1) {
				// Pick random PC (from *i)
				int i;
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
						else if(spec.ex1a == 4 && !univ.party[i].has_space())
							can_pick = false;
						tries++;
					}
					if(can_pick)
						current_pc_picked_in_spec_enc = &univ.party[i];
					else *next_spec = spec.ex1b;
				}
				else {
					i = get_ran(1,0,5);
					current_pc_picked_in_spec_enc = &univ.party[i];
				}
			}
			break;
		case eSpecType::DAMAGE: {
			r1 = get_ran(spec.ex1a,1,spec.ex1b) + spec.ex2a;
			eDamageType dam_type = (eDamageType) spec.ex2b;
			int snd_type = spec.ex2c < 0 ? 0 : -spec.ex2c;
			if(pc_num == 6) hit_party(r1, dam_type, snd_type);
			else damage_target(pc_num, r1, dam_type, snd_type);
			break;
		}
		case eSpecType::AFFECT_HP:
			if(spec.ex1b == 0)
				pc->heal(spec.ex1a);
			else pc->heal(-spec.ex1a);
			if(cCreature* who = dynamic_cast<cCreature*>(pc)) {
				if(spec.ex1b == 0)
					who->spell_note(41);
				else who->spell_note(42);
			}
			break;
		case eSpecType::AFFECT_SP:
			if(spec.ex1b == 0)
				pc->restore_sp(spec.ex1a);
			else pc->restore_sp(-spec.ex1a);
			if(cCreature* who = dynamic_cast<cCreature*>(pc)) {
				if(spec.ex1b == 0)
					who->spell_note(43);
				else who->spell_note(44);
			}
			break;
		case eSpecType::AFFECT_XP:
			if(pc_num >= 100) break;
			for(short i = 0; i < 6; i++)
				if(pc_num == 6 || pc_num == i) {
					if(spec.ex1a < 0)
						univ.party[i].experience = univ.party[i].level * univ.party[i].get_tnl();
					else if(spec.ex1b == 0) award_xp(i,spec.ex1a,true);
					else drain_pc(univ.party[i],spec.ex1a);
				}
			break;
		case eSpecType::AFFECT_SKILL_PTS:
			if(pc_num >= 100) break;
			for(short i = 0; i < 6; i++)
				if(pc_num == 6 || pc_num == i)
					univ.party[i].skill_pts = minmax(0,	100,
						univ.party[i].skill_pts + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case eSpecType::AFFECT_DEADNESS:
			if(pc_num < 100) {
				for(short i = 0; i < 6; i++)
					if(pc_num == 6 || pc_num == i) {
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
								kill_pc(univ.party[i],spec.ex1c > 0 ? eMainStatus::DEAD : eMainStatus::SPLIT_DEAD);
								break;
							case 1:
								kill_pc(univ.party[i],spec.ex1c > 0 ? eMainStatus::DUST : eMainStatus::SPLIT_DUST);
								break;
							case 2:
								if(spec.ex1c > 0)
									petrify_pc(univ.party[i],spec.ex1c);
								else kill_pc(univ.party[i],eMainStatus::SPLIT_STONE);
								break;
							case 3:
								if(!is_combat() || which_combat_type != 0)
									break;
								if(univ.party[i].main_status == eMainStatus::ALIVE)
									univ.party[i].main_status = eMainStatus::FLED;
								break;
							case 4:
								if(party_size(true) > 1)
									univ.party[i].main_status += eMainStatus::SPLIT;
								break;
							case 5:
								kill_pc(univ.party[i],spec.ex1c > 0 ? eMainStatus::ABSENT : eMainStatus::SPLIT_ABSENT);
								break;
						}
					}
				*redraw = 1;
			} else {
				// Kill monster
				cCreature& who = univ.town.monst[pc_num - 100];
				if(who.active > 0 && spec.ex1b > 0) {
					switch(spec.ex1a) {
						case 0:
							who.spell_note(46);
							kill_monst(who,7,eMainStatus::DEAD);
							break;
						case 1:
							who.spell_note(51);
							kill_monst(who,7,eMainStatus::DUST);
							break;
						case 2:
							if(spec.ex1c > 0)
								petrify_monst(who,spec.ex1c);
							else {
								who.spell_note(8);
								kill_monst(who,7,eMainStatus::STONE);
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
					who.spell_note(45);
				}
			}
			break;
		case eSpecType::AFFECT_STATUS:
			switch(eStatus(spec.ex1c)) {
				case eStatus::POISON:
					if(spec.ex1b == 0)
						pc->cure(spec.ex1a);
					else pc->poison(spec.ex1a);
					break;
				case eStatus::HASTE_SLOW:
					if(spec.ex1b == 0)
						pc->slow(-spec.ex1a);
					else pc->slow(spec.ex1a);
					break;
				case eStatus::INVULNERABLE:
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::INVULNERABLE, spec.ex1a);
					else pc->apply_status(eStatus::INVULNERABLE, -spec.ex1a);
					break;
				case eStatus::MAGIC_RESISTANCE:
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::MAGIC_RESISTANCE, spec.ex1a);
					else pc->apply_status(eStatus::MAGIC_RESISTANCE, -spec.ex1a);
					break;
				case eStatus::WEBS:
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::WEBS, -spec.ex1a);
					else pc->web(spec.ex1a);
					break;
				case eStatus::DISEASE:
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::DISEASE, -spec.ex1a);
					else pc->disease(spec.ex1a);
					break;
				case eStatus::INVISIBLE:
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::INVISIBLE, spec.ex1a);
					else pc->apply_status(eStatus::INVISIBLE, -spec.ex1a);
					break;
				case eStatus::BLESS_CURSE:
					if(spec.ex1b == 0)
						pc->curse(-spec.ex1a);
					else pc->curse(spec.ex1a);
					break;
				case eStatus::DUMB:
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::DUMB, -spec.ex1a);
					else pc->dumbfound(spec.ex1a);
					break;
				case eStatus::ASLEEP:
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::ASLEEP, -spec.ex1a);
					else pc->sleep(eStatus::ASLEEP, spec.ex1a, 10);
					break;
				case eStatus::PARALYZED:
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::PARALYZED, -spec.ex1a);
					else pc->sleep(eStatus::PARALYZED, spec.ex1a, 10);
					break;
				case eStatus::POISONED_WEAPON:
					if(pc_num >= 100) break;
					for(short i = 0; i < 6; i++)
						if(pc_num == 6 || pc_num == i) {
							if(spec.ex1b == 0)
								poison_weapon(i, spec.ex1a, true);
							else univ.party[i].apply_status(eStatus::POISONED_WEAPON, -spec.ex1a);
						}
					break;
				case eStatus::MARTYRS_SHIELD:
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::MARTYRS_SHIELD, spec.ex1a);
					else pc->apply_status(eStatus::MARTYRS_SHIELD, -spec.ex1a);
					break;
				case eStatus::ACID:
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::ACID, -spec.ex1a);
					else pc->acid(spec.ex1a);
					break;
				case eStatus::FORCECAGE:
					if(is_out()) break;
					if(spec.ex1b == 0)
						pc->apply_status(eStatus::FORCECAGE, -spec.ex1a);
					else pc->sleep(eStatus::FORCECAGE, spec.ex1a, 10);
					break;
					// Invalid values
				case eStatus::MAIN:
				case eStatus::CHARM:
					break;
			}
			put_pc_screen();
			if(spec.ex2a == int(eStatus::DUMB))
				adjust_spell_menus();
			break;
		case eSpecType::AFFECT_STAT:
			if(pc_num >= 100) break;
			if(spec.ex2a != minmax(0,20,spec.ex2a)) {
				showError("Skill is out of range.");
				break;
			}
			for(short i = 0; i < 6; i++)
				if((pc_num == 6 || pc_num == i) && get_ran(1,1,100) < spec.pic) {
					eSkill skill = eSkill(spec.ex2a);
					int adj = spec.ex1a * (spec.ex1b != 0 ? -1: 1);
					if(skill == eSkill::MAX_HP)
						univ.party[i].max_health = minmax(6, 250, univ.party[i].max_health + adj);
					else if(skill == eSkill::MAX_SP)
						univ.party[i].max_sp = minmax(0, 150, univ.party[i].max_sp + adj);
					else univ.party[i].skills[skill] = minmax(0, skill_max[skill], univ.party[i].skills[skill] + adj);
				}
			break;
		case eSpecType::AFFECT_LEVEL:
			if(pc_num >= 100) {
				int lvl = pc->get_level();
				if(spec.ex1b == 0)
					lvl += spec.ex1a;
				else lvl -= spec.ex1a;
				dynamic_cast<cCreature*>(pc)->level = lvl;
			} else for(short i = 0; i < 6; i++)
				if(pc_num == 6 || pc_num == i) {
					int cur = univ.party[i].get_level();
					int lvl = cur;
					if(spec.ex1b == 0)
						lvl += spec.ex1a;
					else lvl -= spec.ex1a;
					univ.party[i].level = lvl;
				}
			break;
		case eSpecType::AFFECT_MORALE:
			pc->scare(spec.ex1a * (spec.ex1b != 0 ? 1 : -1));
			break;
		case eSpecType::AFFECT_MONST_ATT:
			if(pc_num < 100) break;
			if(spec.ex1a < 0 || spec.ex1a > 2) {
				showError("Invalid monster attack (0-2)");
				break;
			}
			if(spec.ex2a == 0) {
				dynamic_cast<cCreature*>(pc)->a[spec.ex1a].dice += spec.ex1b;
				dynamic_cast<cCreature*>(pc)->a[spec.ex1a].sides += spec.ex1c;
			} else {
				dynamic_cast<cCreature*>(pc)->a[spec.ex1a].dice -= spec.ex1b;
				dynamic_cast<cCreature*>(pc)->a[spec.ex1a].sides -= spec.ex1c;
			}
			break;
		case eSpecType::AFFECT_MONST_STAT:
			if(pc_num < 100) break;
			if(spec.ex2a < 0 || spec.ex2a > 7) {
				showError("Invalid monster stat (0-7)");
				break;
			}
			// Blah, let's hackily reuse pc_num...
			pc_num = spec.ex1a;
			if(spec.ex1b > 0)
				pc_num = -pc_num;
			switch(spec.ex2a) {
				case 0: dynamic_cast<cCreature*>(pc)->m_health += pc_num; break;
				case 1: dynamic_cast<cCreature*>(pc)->max_mp += pc_num; break;
				case 2: dynamic_cast<cCreature*>(pc)->armor += pc_num; break;
				case 3: dynamic_cast<cCreature*>(pc)->skill += pc_num; break;
				case 4: dynamic_cast<cCreature*>(pc)->speed += pc_num; break;
				case 5: dynamic_cast<cCreature*>(pc)->mu += pc_num; break;
				case 6: dynamic_cast<cCreature*>(pc)->cl += pc_num; break;
			}
			break;
		case eSpecType::AFFECT_MAGE_SPELL:
			if(pc_num >= 100) break;
			if(spec.ex1a != minmax(0,61,spec.ex1a)) {
				showError("Mage spell is out of range (0 - 61). See docs.");
				break;
			}
			for(short i = 0; i < 6; i++)
				if(pc_num == 6 || pc_num == i)
					univ.party[i].mage_spells[spec.ex1a] = !spec.ex1b;
			break;
		case eSpecType::AFFECT_PRIEST_SPELL:
			if(pc_num >= 100) break;
			if(spec.ex1a != minmax(0,61,spec.ex1a)) {
				showError("Priest spell is out of range (0 - 61). See docs.");
				break;
			}
			for(short i = 0; i < 6; i++)
				if(pc_num == 6 || pc_num == i)
					univ.party[i].priest_spells[spec.ex1a] = !spec.ex1b;
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
				showError("Alchemy is out of range.");
				break;
			}
			univ.party.alchemy[spec.ex1a] = true;
			break;
		case eSpecType::AFFECT_SOUL_CRYSTAL:
			if(pc_num < 100) break;
			if(spec.ex1a == 0)
				record_monst(dynamic_cast<cCreature*>(pc), spec.ex1b);
			else for(mon_num_t& monst : univ.party.imprisoned_monst) {
				if(monst == dynamic_cast<cCreature*>(pc)->number)
					monst = 0;
			}
			break;
		case eSpecType::AFFECT_PARTY_STATUS:
			if(spec.ex2a < 0 || spec.ex2a > 3) break;
			if(spec.ex1b == 0 && spec.ex2a == 1 && univ.party.in_boat >= 0)
				add_string_to_buf("  Can't fly when on a boat.");
			else if(spec.ex1b == 0 && spec.ex2a == 1 && univ.party.in_horse >= 0)
				add_string_to_buf("  Can't fly when on a horse.");
			r1 = univ.party.status[ePartyStatus(spec.ex2a)];
			if(spec.ex1b == 0)
				r1 = minmax(0,250,r1 + spec.ex1a);
			else r1 = minmax(0,250,r1 - spec.ex1a);
			univ.party.status[ePartyStatus::STEALTH] = r1;
			break;
		case eSpecType::AFFECT_TRAITS:
			if(pc_num >= 100) break;
			if(spec.ex1a < 0 || spec.ex1a > 15) {
				showError("Trait is out of range (0 - 15).");
				break;
			}
			for(short i = 0; i < 6; i++)
				if(pc_num == 6 || pc_num == i)
					univ.party[i].traits[eTrait(spec.ex1a)] = !spec.ex1b;
			break;
		case eSpecType::AFFECT_AP:
			if(!is_combat()) break;
			if(pc_num == 6) {
				for(short i = 0; i < 6; i++) {
					if(spec.ex1b)
						univ.party[i].ap += spec.ex1a;
					else univ.party[i].ap -= spec.ex1a;
					if(univ.party[i].ap < 0)
						univ.party[i].ap = 0;
				}
			} else {
				if(spec.ex1b)
					pc->ap += spec.ex1a;
				else pc->ap -= spec.ex1a;
				if(pc->ap < 0)
					pc->ap = 0;
			}
			break;
		case eSpecType::AFFECT_NAME:
			get_strs(str, str, cur_spec_type, spec.m3, -1);
			if(cPlayer* who = dynamic_cast<cPlayer*>(pc))
				who->name = str;
			else if(cCreature* monst = dynamic_cast<cCreature*>(pc))
				monst->m_name = str;
			else if(dynamic_cast<cParty*>(pc))
				for(short i = 0; i < 6; i++)
					univ.party[i].name = str;
			break;
		case eSpecType::CREATE_NEW_PC:
			if(spec.ex1c < 0 || spec.ex1c > 19) {
				showError("Race out of range (0 - 19).");
				break;
			}
			pc_num = univ.party.free_space();
			if(pc_num == 6) {
				add_string_to_buf("No room for new PC.");
				*next_spec = spec.pictype;
				check_mess = false;
				break;
			}
			get_strs(str, str, cur_spec_type, spec.m3, -1);
			univ.party.new_pc(pc_num);
			univ.party[pc_num].name = str;
			univ.party[pc_num].which_graphic = spec.pic;
			univ.party[pc_num].cur_health = univ.party[pc_num].max_health = spec.ex1a;
			univ.party[pc_num].cur_sp = univ.party[pc_num].max_sp = spec.ex1b;
			univ.party[pc_num].race = eRace(spec.ex1c);
			univ.party[pc_num].skills[eSkill::STRENGTH] = spec.ex2a;
			univ.party[pc_num].skills[eSkill::DEXTERITY] = spec.ex2b;
			univ.party[pc_num].skills[eSkill::INTELLIGENCE] = spec.ex2c;
			current_pc_picked_in_spec_enc = &univ.get_target(pc_num);
			if(univ.party.sd_legit(spec.sd1, spec.sd2))
				univ.party.stuff_done[spec.sd1][spec.sd2] = univ.party[pc_num].unique_id - 1000;
			break;
		case eSpecType::STORE_PC:
			if(cPlayer* who = dynamic_cast<cPlayer*>(pc)) {
				if(univ.party.sd_legit(spec.sd1, spec.sd2))
					univ.party.stuff_done[spec.sd1][spec.sd2] = univ.party[pc_num].unique_id - 1000;
				if(spec.ex1a == 1) break;
				who->main_status += eMainStatus::SPLIT;
				univ.stored_pcs[who->unique_id] = who->leave_party();
				univ.party.new_pc(pc_num);
			}
			break;
		case eSpecType::UNSTORE_PC:
			if(spec.ex1a < 1000) spec.ex1a += 1000;
			if(univ.stored_pcs.find(spec.ex1a) == univ.stored_pcs.end()) {
				showError("Scenario tried to unstore a nonexistent PC!");
				break;
			}
			pc_num = univ.party.free_space();
			if(pc_num == 6) {
				add_string_to_buf("No room for PC.");
				*next_spec = spec.ex1b;
				check_mess = false;
				break;
			}
			univ.party.replace_pc(pc_num, univ.stored_pcs[spec.ex1a]);
			current_pc_picked_in_spec_enc = &univ.get_target(pc_num);
			univ.party[pc_num].main_status -= eMainStatus::SPLIT;
			univ.stored_pcs.erase(spec.ex1a);
			break;
		case eSpecType::AFFECT_MONST_TARG:
			if(pc_num < 100) break;
			// TODO: Verify this actually works! It's possible the monster ignores this and just recalculates its target each turn.
			dynamic_cast<cCreature*>(pc)->target = spec.ex1a;
			break;
		case eSpecType::GIVE_ITEM:
			if(pc_num >= 100) break;
			if(spec.ex1a >= 0 && spec.ex1a < univ.scenario.scen_items.size()) {
				cItem to_give = univ.scenario.scen_items[spec.ex1a];
				if(spec.ex1b >= 0 && spec.ex1b <= 6) {
					// TODO: This array and accompanying calculation is now duplicated here, in start_town_mode(), and in place_buy_button()
					const short aug_cost[10] = {4,7,10,8, 15,15,10, 0,0,0};
					int val = max(aug_cost[spec.ex1b] * 100, to_give.value * (5 + aug_cost[spec.ex1b]));
					to_give.enchant_weapon(eEnchant(spec.ex1b), val);
				}
				if(to_give.charges > 0 && spec.ex1c >= 0)
					to_give.charges = spec.ex1c;
				if(spec.ex2a == 1 || spec.ex2a == 2) to_give.ident = true;
				else if(spec.ex2a == 0) to_give.ident = false;
				if(spec.ex2a == 2) to_give.concealed = false;
				if(spec.ex2b == 1) to_give.cursed = to_give.unsellable = true;
				else if(spec.ex2b == 0) to_give.cursed = to_give.unsellable = false;
				int equip_type = 0;
				if(spec.ex2c == 0) equip_type = GIVE_EQUIP_SOFT;
				else if(spec.ex2c == 1) equip_type = GIVE_EQUIP_TRY;
				else if(spec.ex2c >= 2) equip_type = GIVE_EQUIP_FORCE;
				bool success = true;
				for(short i = 0; i < 6; i++)
					if(pc_num == 6 || pc_num == i)
						success = success && univ.party[i].give_item(to_give, equip_type | GIVE_ALLOW_OVERLOAD);
				if(!success)
					*next_spec = spec.pic;
			}
			break;
		default:
			showError("Special node type \"" + (*cur_node.type).name() + "\" is either miscategorized or unimplemented!");
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

void ifthen_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
				 short *next_spec,short* /*next_spec_type*/,short *a,short *b,short *redraw) {
	bool check_mess = false;
	std::string str1, str2, str3;
	cSpecial spec;
	location l;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	switch(cur_node.type) {
		case eSpecType::IF_SDF:
			if(univ.party.sd_legit(spec.sd1,spec.sd2)) {
				if((spec.ex1a >= 0) && (PSD[spec.sd1][spec.sd2] >= spec.ex1a))
					*next_spec = spec.ex1b;
				else if((spec.ex2a >= 0) && (PSD[spec.sd1][spec.sd2] < spec.ex2a))
					*next_spec = spec.ex2b;
			}
			break;
		case eSpecType::IF_TOWN_NUM:
			if(((is_town()) || (is_combat())) && (univ.party.town_num == spec.ex1a))
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_RANDOM:
			if(get_ran(1,1,100) < spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_HAVE_SPECIAL_ITEM:
			if(spec.ex1a != minmax(0,49,spec.ex1a)) {
				showError("Special item is out of range.");
			}
			else if(univ.party.spec_items.count(spec.ex1a) > 0)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_SDF_COMPARE:
			if((univ.party.sd_legit(spec.sd1,spec.sd2)) && (univ.party.sd_legit(spec.ex1a,spec.ex1b))) {
				if(PSD[spec.ex1a][spec.ex1b] < PSD[spec.sd1][spec.sd2])
					*next_spec = spec.ex2b;
			}
			else showError("A Stuff Done flag is out of range.");
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
				if(spec.ex2a > 0) take_gold(spec.ex1a,true);
				*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_HAS_FOOD:
			if(univ.party.food >= spec.ex1a) {
				if(spec.ex2a > 0) take_food(spec.ex1a,true);
				*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_ITEM_CLASS_ON_SPACE:
			if(is_out())
				break;
			l.x = spec.ex1a; l.y = spec.ex1b;
			for(short i = 0; i < univ.town.items.size(); i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].special_class == (unsigned)spec.ex2a
				   && l == univ.town.items[i].item_loc) {
					*next_spec = spec.ex2b;
					if(spec.ex2c > 0) {
						*redraw = 1;
						univ.town.items[i].variety = eItemType::NO_ITEM;
					}
				}
			break;
		case eSpecType::IF_HAVE_ITEM_CLASS:
			if(univ.party.check_class(spec.ex1a,spec.ex2a > 0))
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_EQUIP_ITEM_CLASS:
			for(cPlayer& pc : univ.party)
				if(pc.main_status == eMainStatus::ALIVE)
					for(short j = 0; j < pc.items.size(); j++)
						if(pc.items[j].variety != eItemType::NO_ITEM && pc.items[j].special_class == (unsigned)spec.ex1a
						   && pc.equip[j]) {
							*next_spec = spec.ex1b;
							if(spec.ex2a > 0) {
								*redraw = 1;
								pc.take_item(j);
								if(&pc == &univ.party[stat_window])
									put_item_screen(stat_window);
							}
						}
			break;
		case eSpecType::IF_DAY_REACHED:
			if(univ.party.calc_day() >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_FIELDS:
			if(is_out()) break;
			if(!isValidField(spec.m1, false)) {
				showError("Scenario tried to check for invalid field type (1...24)");
			} else {
				int i = 0;
				for(short j = spec.ex1b; j < min(spec.ex2b, univ.town->max_dim()); j++)
					for(short k = spec.ex1a; k < min(spec.ex2a, univ.town->max_dim()); k++) {
						switch(eFieldType(spec.m1)) {
							// These values are not allowed
							case SPECIAL_EXPLORED: case SPECIAL_SPOT: case SPECIAL_ROAD:
							case FIELD_DISPEL: case FIELD_SMASH:
								break;
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
			}
			break;
		case eSpecType::IF_PARTY_SIZE:
			if(spec.ex2a < 1) {
				if(party_size(spec.ex1a <= 0) == spec.ex2b)
					*next_spec = spec.ex1b;
			}
			else {
				if(party_size(spec.ex1a <= 0) >= spec.ex2b)
					*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_EVENT_OCCURRED:
			if(day_reached(spec.ex1a,spec.ex1b))
				*next_spec = spec.ex2b;
			break;
		case eSpecType::IF_SPECIES:
			if(spec.ex1a < 0 || spec.ex1a > 21) {
				showError("Species out of range (0-21)");
			} else {
				int i = race_present(eRace(spec.ex1a)), j = minmax(1, party_size(true), spec.ex2a);
				if(spec.ex2b == -2 && i <= j) *next_spec = spec.ex1b;
				if(spec.ex2b == -1 && i < j) *next_spec = spec.ex1b;
				if(spec.ex2b == 0 && i == j) *next_spec = spec.ex1b;
				if(spec.ex2b == 1 && i > j) *next_spec = spec.ex1b;
				if(spec.ex2b == 2 && i >= j) *next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_TRAIT:
			if(spec.ex1a < 0 || spec.ex1a > 15) {
				showError("Invalid trait (0...15)");
			} else {
				int i = trait_present(eTrait(spec.ex1a)), j = minmax(1, party_size(true), spec.ex2a);
				if(spec.ex2b == -2 && i <= j) *next_spec = spec.ex1b;
				if(spec.ex2b == -1 && i < j) *next_spec = spec.ex1b;
				if(spec.ex2b == 0 && i == j) *next_spec = spec.ex1b;
				if(spec.ex2b == 1 && i > j) *next_spec = spec.ex1b;
				if(spec.ex2b == 2 && i >= j) *next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_STATISTIC:
			if((spec.ex2a < 0 || spec.ex2a > 20) && (spec.ex2a < 100 || spec.ex2a > 104)) {
				showError("Attempted to check an invalid statistic (0...20 or 100...104).");
				break;
			}
			if(spec.ex2b < -1 || spec.ex2b > 3) {
				showError("Invalid statistic-checking mode (-1...3); will fall back to cumulative check.");
				spec.ex2b = 0;
			}
			
			if(spec.ex2b == -1) {
				// Check specific PC's stat (uses the active PC from Select PC node)
				short pc = univ.get_target_i(*current_pc_picked_in_spec_enc);
				if(pc == 6 && univ.party.is_split())
					pc = univ.get_target_i(univ.party.pc_present());
				if(pc >= 0 && pc < 6) {
					if(check_party_stat(eSkill(spec.ex2a), 10 + pc) >= spec.ex1a)
						*next_spec = spec.ex1b;
					break;
				}
				spec.ex2b = 0;
			}
			if(check_party_stat(eSkill(spec.ex2a), spec.ex2b) >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_TEXT_RESPONSE:
			check_mess = false;
			get_strs(str1,str1,0,spec.m1,-1);
			str3 = get_text_response(str1);
			spec.pic = minmax(0,50,spec.pic);
			get_strs(str1,str2,0,spec.ex1a,spec.ex2a);
			if(spec.ex1a >= 0 && str3.compare(0, spec.pic, str1, 0, spec.pic) == 0)
				*next_spec = spec.ex1b;
			if(spec.ex2a >= 0 && str3.compare(0, spec.pic, str2, 0, spec.pic) == 0)
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
		case eSpecType::IF_NUM_RESPONSE: {
			check_mess = false;
			if(spec.m2 > spec.m3) std::swap(spec.m2,spec.m3);
			get_strs(str1,str1,0,spec.m1,-1);
			int i = get_num_response(spec.m2,spec.m3,str1);
			setsd(spec.sd1, spec.sd2, abs(i));
			int j = 0;
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
		}
		case eSpecType::IF_SDF_EQ:
			if(univ.party.sd_legit(spec.sd1,spec.sd2)) {
				if(PSD[spec.sd1][spec.sd2] == spec.ex1a)
					*next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_ALIVE:
			if(spec.ex1a == -1 && current_pc_picked_in_spec_enc->is_alive())
				*next_spec = spec.ex1b;
			else if(dynamic_cast<cPlayer*>(current_pc_picked_in_spec_enc)) {
				int pc = univ.get_target_i(*current_pc_picked_in_spec_enc);
				eMainStatus stat;
				switch(spec.ex1a) {
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
				bool pass = false;
				if(stat == eMainStatus::SPLIT && univ.party.is_split())
					pass = true;
				else if(pc < 6 && univ.party[pc].main_status == stat)
					pass = true;
				else {
					int n = std::count_if(univ.party.begin(), univ.party.end(), [stat](const cPlayer& who) {
						return who.main_status == stat;
					});
					pass = n > 0;
				}
				if(pass) *next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_MAGE_SPELL:
			if(cPlayer* who = dynamic_cast<cPlayer*>(current_pc_picked_in_spec_enc)) {
				int pc = univ.get_target_i(*who);
				if(spec.ex1a < 0 || spec.ex1a >= 62)
					break;
				bool pass = false;
				if(pc < 6 && univ.party[pc].mage_spells[spec.ex1a])
					pass = true;
				else {
					int n = std::count_if(univ.party.begin(), univ.party.end(), [&spec](const cPlayer& who) {
						return who.mage_spells[spec.ex1a];
					});
					pass = n + 0;
				}
				if(pass) *next_spec = spec.ex1b;
			} else {
				// TODO: Implement for monsters
				// Currently they have a fixed spell list depending solely on caster level
			}
			break;
		case eSpecType::IF_PRIEST_SPELL:
			if(cPlayer* who = dynamic_cast<cPlayer*>(current_pc_picked_in_spec_enc)) {
				int pc = univ.get_target_i(*who);
				if(spec.ex1a < 0 || spec.ex1a >= 62)
					break;
				bool pass = false;
				if(pc < 6 && univ.party[pc].priest_spells[spec.ex1a])
					pass = true;
				else {
					int n = std::count_if(univ.party.begin(), univ.party.end(), [&spec](const cPlayer& who) {
						return who.priest_spells[spec.ex1a];
					});
					pass = n + 0;
				}
				if(pass) *next_spec = spec.ex1b;
			} else {
				// TODO: Implement for monsters
				// Currently they have a fixed spell list depending solely on caster level
			}
			break;
		case eSpecType::IF_RECIPE:
			if(spec.ex1a < 0 || spec.ex1a >= 20) {
				showError("Alchemy recipe out of range (0 - 19).");
				break;
			}
			if(univ.party.alchemy[spec.ex1a])
				*next_spec = spec.ex1b;
			break;
		case eSpecType::IF_STATUS:
			if(spec.ex1a < 0 || spec.ex1a > 14) {
				showError("Invalid status effect (0...14)");
			} else {
				int k = 0;
				if(dynamic_cast<cParty*>(current_pc_picked_in_spec_enc)) {
					k = spec.ex2b == 2 ? std::numeric_limits<short>::max() : 0;
					int j = 0;
					for(short i = 0; i < 6; i++, j++)
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
				} else {
					if(current_pc_picked_in_spec_enc->is_alive())
					   k = current_pc_picked_in_spec_enc->status[eStatus(spec.ex1a)];
					else k = 0;
				}
				int j = spec.ex2a;
				if(spec.ex2c == -2 && k <= j) *next_spec = spec.ex1b;
				if(spec.ex2c == -1 && k < j) *next_spec = spec.ex1b;
				if(spec.ex2c == 0 && k == j) *next_spec = spec.ex1b;
				if(spec.ex2c == 1 && k > j) *next_spec = spec.ex1b;
				if(spec.ex2c == 2 && k >= j) *next_spec = spec.ex1b;
			}
			break;
		case eSpecType::IF_QUEST:
			if(spec.ex1a < 0 || spec.ex1a >= univ.scenario.quests.size()) {
				showError("The scenario tried to update a non-existent quest.");
				break;
			}
			if(spec.ex1b < 0 || spec.ex1b > 3) {
				showError("Invalid quest status (range 0 .. 3).");
				break;
			}
			if(univ.party.quest_status[spec.ex1a] == eQuestStatus(spec.ex1b))
				*next_spec = spec.ex1c;
			break;
		case eSpecType::IF_CONTEXT:
			// TODO: Test this. In particular, test that the legacy behaviour is correct.
			if(which_mode == eSpecCtx(spec.ex1a)) {
				if(which_mode <= eSpecCtx::COMBAT_MOVE) {
					*a = bool(spec.ex1b); // Should block move? 1 = yes, 0 = no
					if(*a) {
						if(which_mode == eSpecCtx::OUT_MOVE)
							ASB("Can't go here while outdoors.");
						else if(which_mode == eSpecCtx::TOWN_MOVE)
							ASB("Can't go here while in town mode.");
						else if(which_mode == eSpecCtx::COMBAT_MOVE)
							ASB("Can't go here during combat.");
					}
				} else if(which_mode == eSpecCtx::TARGET && spec.ex1b >= 0) {
					// TODO: I'm not quite sure if this covers every way of determining which spell was cast
					if(is_town() && int(town_spell) != spec.ex1b)
						break;
					if(is_combat() && int(spell_being_cast) != spec.ex1b)
						break;
				}
				*next_spec = spec.ex1c;
			}
			break;
		case eSpecType::IF_LOOKING:
			if(which_mode == eSpecCtx::OUT_LOOK || which_mode == eSpecCtx::TOWN_LOOK)
				*next_spec = spec.ex1c;
			break;
		case eSpecType::IF_IN_BOAT:
			if((spec.ex1b == -1 && univ.party.in_boat >= 0) || spec.ex1b == univ.party.in_boat)
				*next_spec = spec.ex1c;
			break;
		case eSpecType::IF_ON_HORSE:
			if((spec.ex1b == -1 && univ.party.in_horse >= 0) || spec.ex1b == univ.party.in_horse)
				*next_spec = spec.ex1c;
			break;
		default:
			showError("Special node type \"" + (*cur_node.type).name() + "\" is either miscategorized or unimplemented!");
			break;
	}
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
}

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
	short r1;
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
			if(spec.ex2a < 0 || spec.ex2a > 3){
				showError("Invalid attitude (0-Friendly Docile, 1-Hostile A, 2-Friendly Will Fight, 3-Hostile B).");
				break;
			}
			set_town_attitude(spec.ex1a,spec.ex1b,eAttitude(spec.ex2a));
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
				alter_space(l.x,l.y,univ.scenario.ter_types[ter].flag1);
			*redraw = 1;
			break;
		case eSpecType::TOWN_UNLOCK_SPACE:
			ter = coord_to_ter(spec.ex1a,spec.ex1b);
			if(univ.scenario.ter_types[ter].special == eTerSpec::UNLOCKABLE)
				alter_space(l.x,l.y,univ.scenario.ter_types[ter].flag1);
			*redraw = 1;
			break;
		case eSpecType::TOWN_SFX_BURST:
			if(which_mode == eSpecCtx::TALK)
				break;
			if(spec.ex2b == 1)
				mondo_boom(l,spec.ex2a,spec.ex2c);
			else run_a_boom(l,spec.ex2a,0,0,spec.ex2c);
			break;
		case eSpecType::TOWN_CREATE_WANDERING:
			create_wand_monst();
			*redraw = 1;
			break;
		case eSpecType::TOWN_PLACE_MONST:
			place_monster(spec.ex2a,l,spec.ex2b);
			*redraw = 1;
			break;
		case eSpecType::TOWN_DESTROY_MONST:
			if(spec.ex1a >= 0 && spec.ex1b >= 0) {
				iLiving* monst = univ.target_there(l, TARG_MONST);
				if(monst != nullptr)
					dynamic_cast<cCreature*>(monst)->active = 0;
			}
			*redraw = 1;
			break;
		case eSpecType::TOWN_NUKE_MONSTS:
			for(short i = 0; i < univ.town.monst.size(); i++)
				if(univ.town.monst[i].active > 0 &&
					(univ.town.monst[i].number == spec.ex1a || spec.ex1a == 0 ||
					(spec.ex1a == -1 && univ.town.monst[i].is_friendly()) ||
					(spec.ex1a == -2 && !univ.town.monst[i].is_friendly()))) {
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
				if(handle_lever(loc(univ.party.get_ptr(10), univ.party.get_ptr(11))))
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
			check_mess = false;
			if(spec.ex2c != 1 && spec.ex2c != 2 && is_combat()) {
				ASB("Can't change level in combat.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
			}
			else if(spec.ex2c != 2 && spec.ex2c != 3 && which_mode != eSpecCtx::TOWN_MOVE) {
				ASB("Can't change level now.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
			}
			else {
				*a = 1;
				if(spec.ex2b < 0) spec.ex2b = 0;
				if((spec.ex2b >= 8) || (cChoiceDlog(stairDlogs[spec.ex2b],{"climb","leave"}).show() == "climb")) {
					bool was_in_combat = false;
					short was_active;
					if(overall_mode == MODE_TALKING)
						end_talk_mode();
					else if(is_combat()) {
						was_in_combat = true;
						if(which_combat_type == 0) { // outdoor combat
							ASB("Can't change level in combat.");
							*next_spec = -1;
							break;
						} else {
							was_active = current_pc;
							eDirection dir = end_town_combat();
							if(dir == DIR_HERE) {
								ASB("Can't change level now.");
								break;
							}
							univ.party.direction = dir;
						}
					}
					*a = 1;
					change_level(spec.ex2a,l.x,l.y);
					if(was_in_combat) {
						start_town_combat(univ.party.direction);
						current_pc = was_active;
					}
				} else *next_spec = -1;
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
				for(short i = 0; i < 3; i++)
					get_strs(strs[i * 2],strs[i * 2 + 1],cur_spec_type, spec.m1 + i * 2 ,spec.m1 + i * 2 + 1);
				buttons[0] = 9; buttons[1] = 35;
				if(custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons) == 1)
					*next_spec = -1;
				else {
					int x = univ.party.get_ptr(10), y = univ.party.get_ptr(11);
					ter = coord_to_ter(x, y);
					alter_space(x,y,univ.scenario.ter_types[ter].trans_to_what);
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
				for(short i = 0; i < 3; i++)
					get_strs(strs[i * 2],strs[i * 2 + 1], cur_spec_type,spec.m1 + i * 2, spec.m1 + i * 2 + 1);
				buttons[0] = 9; buttons[1] = 8;
				if(custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons) == 1) {
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
			}
			else if(spec.ex2c != 2 && spec.ex2c != 3 && which_mode != eSpecCtx::TOWN_MOVE) {
				ASB("Can't change level now.");
				if(which_mode == eSpecCtx::OUT_MOVE || which_mode == eSpecCtx::TOWN_MOVE || which_mode == eSpecCtx::COMBAT_MOVE)
					*a = 1;
				*next_spec = -1;
			}
			else {
				for(short i = 0; i < 3; i++)
					get_strs(strs[i * 2],strs[i * 2 + 1],cur_spec_type, spec.m1 + i * 2, spec.m1 + i * 2 + 1);
				buttons[0] = 20; buttons[1] = 24;
				int i = spec.ex2b == 1 ? 2 : custom_choice_dialog(strs, spec.pic, ePicType(spec.pictype), buttons);
				*a = 1;
				if(i == 1) {
					*next_spec = -1;
				} else {
					bool was_in_combat = false;
					short was_active;
					if(overall_mode == MODE_TALKING)
						end_talk_mode();
					else if(is_combat()) {
						was_in_combat = true;
						if(which_combat_type == 0) { // outdoor combat
							ASB("Can't change level in combat.");
							*next_spec = -1;
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
			store_i = univ.scenario.get_stored_item(spec.ex2a);
			place_item(store_i,l,spec.ex2b);
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
				update_explored(univ.party.town_loc);
				center = univ.party.town_loc;
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
			else if(univ.party.left_in == size_t(-1) || univ.party.town_num == univ.party.left_in) {
				univ.party.town_loc = univ.party.left_at;
				update_explored(univ.party.town_loc);
				center = univ.party.town_loc;
				
				// Clear forcecage status
				for(int i = 0; i < 6; i++)
					univ.party[i].status[eStatus::FORCECAGE] = 0;
			} else change_level(univ.party.left_in, univ.party.left_at.x, univ.party.left_at.y);
			break;
		case eSpecType::TOWN_TIMER_START:
			univ.party.start_timer(spec.ex1a, spec.ex1b, 2);
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
				showError("Tried to change the attitude of a nonexistent monster (should be 0...59).");
				break;
			}
			if(spec.ex1b < 0 || spec.ex1b > 3){
				showError("Invalid attitude (0-Friendly Docile, 1-Hostile A, 2-Friendly Will Fight, 3-Hostile B).");
				break;
			}
			univ.town.monst[spec.ex1a].attitude = eAttitude(spec.ex1b);
			break;
		case eSpecType::TOWN_RUN_MISSILE:
			if(which_mode != eSpecCtx::TALK) {
				int i;
				if(iLiving* targ = univ.target_there(loc(spec.ex2a, spec.ex2b), TARG_MONST)) {
					cCreature* who = dynamic_cast<cCreature*>(targ);
					i = 14 * who->x_width - 1;
					r1 = 18 * who->y_width - 1;
				} else i = r1 = 0;
				run_a_missile(l, loc(spec.ex2a, spec.ex2b), spec.pic, spec.ex1c, spec.ex2c, i, r1, 100);
			}
			break;
		case eSpecType::TOWN_BOOM_SPACE:
			// TODO: This should work, but does it need a bit of extra logic?
			if(which_mode == eSpecCtx::TALK)
				break;
			boom_space(l, 100, spec.ex2a, spec.ex2b, -spec.ex2c);
			break;
		case eSpecType::TOWN_MONST_ATTACK:
			// TODO: I'm not certain if this will work.
			if(which_mode != eSpecCtx::TALK) {
				int i = combat_posing_monster;
				if(l.y >= 0) {
					iLiving* who = univ.target_there(l);
					if(who != nullptr)
						combat_posing_monster = univ.get_target_i(*who);
					if(combat_posing_monster == 6)
						combat_posing_monster = -1;
				} else combat_posing_monster = spec.ex1a;
				if(combat_posing_monster < 0 || combat_posing_monster - 100 >= univ.town.monst.size()) {
					combat_posing_monster = i;
					break;
				}
				redraw_screen(REFRESH_TERRAIN);
				combat_posing_monster = i;
			}
			break;
		case eSpecType::TOWN_SET_CENTER:
			if(l.x >= 0 && l.y >= 0) center = l;
			else center = is_combat() ? univ.party[current_pc].combat_pos : univ.party.town_loc;
			start_cartoon();
			redraw_screen(REFRESH_TERRAIN);
			break;
		case eSpecType::TOWN_LIFT_FOG:
			fog_lifted = spec.ex1a;
			redraw_screen(REFRESH_TERRAIN);
			break;
		case eSpecType::TOWN_START_TARGETING:
			*next_spec = -1;
			if(spec.ex1a < 0 || spec.ex1a > 7) {
				showError("Invalid spell pattern (0 - 7).");
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
			spec_target_type = cur_spec_type;
			spec_target_fail = spec.ex2a;
			spec_target_options = 0;
			if(spec.ex2b > 0)
				spec_target_options += 1;
			if(spec.ex2c > 0)
				spec_target_options += 20;
			else if(spec.ex2c == 0)
				spec_target_options += 10;
			break;
		case eSpecType::TOWN_SPELL_PAT_FIELD:
			if(spec.ex1c < -1 || spec.ex1c > 14) {
				showError("Invalid spell pattern (-1 - 14).");
				break;
			}
			if((spec.ex2a < 1 || spec.ex2a == 9 || spec.ex2a > 24) && spec.ex2a != 32 && spec.ex2a != 33) {
				showError("Invalid field type (see docs).");
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
				showError("Invalid spell pattern (-1 - 14).");
				break;
			}
			if(spec.ex2a < 0 || spec.ex2a > 7) {
				showError("Invalid damage type (0 - 7).");
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
		case eSpecType::TOWN_RELOCATE_CREATURE:
			if(spec.ex2b > 5) {
				showError("Invalid positioning mode (0-5)");
			} else {
				int i = spec.ex2a < 0 ? univ.get_target_i(*current_pc_picked_in_spec_enc) : spec.ex2a;
				if(spec.ex2b == 5) {
					spec.ex2b = 0;
					if(i >= 100) {
						std::set<location, loc_compare> checked;
						std::queue<location> to_check;
						location cur_check = l;
						for(int tries = 0; tries < 100 && !monst_can_be_there(cur_check, i - 100); tries++) {
							for(int x = -1; x <= 1; x++) {
								for(int y = -1; y <= 1; y++) {
									if(x == 0 && y == 0)
										continue;
									location next(l.x+x,l.y+y);
									if(next.x < 0 || next.y < 0 || next.x >= univ.town->max_dim() || next.y >= univ.town->max_dim())
										continue;
									if(!checked.count(next))
										to_check.push(next);
									checked.insert(cur_check);
								}
							}
							cur_check = to_check.front();
							to_check.pop();
						}
						if(monst_can_be_there(cur_check, i - 100))
							l = cur_check;
					}
				}
				if(spec.ex2b > 1) {
					if(spec.ex2b <= 3) l.x *= -1;
					if(spec.ex2b >= 3) l.y *= -1;
				}
				if(i < 6) {
					start_cartoon();
					if(spec.ex2b == 0)
						univ.party[i].combat_pos = l;
					else {
						univ.party[i].combat_pos.x += l.x;
						univ.party[i].combat_pos.y += l.y;
					}
				} else if(i >= 100) {
					i -= 100;
					if(spec.ex2b == 0)
						univ.town.monst[i].cur_loc = l;
					else {
						univ.town.monst[i].cur_loc.x += l.x;
						univ.town.monst[i].cur_loc.y += l.y;
					}
				} else {
					showError("Invalid positioning target!");
					break;
				}
				redraw_screen(REFRESH_TERRAIN);
				if(spec.ex2c > 0)
					sf::sleep(sf::milliseconds(spec.ex2c));
				*redraw = 1;
			}
			break;
		case eSpecType::TOWN_PLACE_LABEL:
			check_mess = false;
			if(l.y < 0) {
				if(l.x < 0)
					l.x = univ.get_target_i(*current_pc_picked_in_spec_enc);
				if(l.x < 6)
					l = (is_combat() || cartoon_happening) ? univ.party[l.x].combat_pos : univ.party.town_loc;
				else if(l.x == 6)
					l = univ.party.town_loc;
				else if(l.x >= 100 && l.x - 100 < univ.town.monst.size())
					l = univ.town.monst[l.x - 100].cur_loc;
				else {
					showError("Invalid label target!");
					break;
				}
			}
			get_strs(strs[0], strs[1], cur_spec_type, spec.m1, spec.m1);
			place_text_label(strs[0], l, spec.ex2a);
			redraw_screen(REFRESH_TERRAIN);
			if(spec.ex2b > 0) // TODO: Add preferences setting to increase this delay, for slow readers
				sf::sleep(sf::seconds(spec.ex2b));
			break;
		default:
			showError("Special node type \"" + (*cur_node.type).name() + "\" is either miscategorized or unimplemented!");
			break;
	}
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
}

void rect_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
			   short *next_spec,short* /*next_spec_type*/,short *a,short *b,short *redraw){
	bool check_mess = true;
	cSpecial spec;
	location l;
	ter_num_t ter;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	*redraw = 1;
	for(short i = spec.ex1b;i <= spec.ex2b;i++)
		for(short j = spec.ex1a; j <= spec.ex2a; j++) {
			l.x = i; l.y = j;
			// If pict non-zero, exclude rectangle interior
			if(spec.pic > 0 && i > spec.ex1b && i < spec.ex2b && j > spec.ex1a && j < spec.ex2a)
				continue;
			switch(cur_node.type) {
				case eSpecType::RECT_PLACE_FIELD:
					if(is_out())
						return;
					if(!isValidField(spec.sd2, true)) {
						showError("Scenario tried to place an invalid field type (1...24)");
						goto END; // Break out of the switch AND both loops, but still handle messages
					}
					if(spec.sd2 == FIELD_DISPEL || get_ran(1,1,100) <= spec.sd1)
						switch(eFieldType(spec.sd2)) {
							// These values are not allowed.
							case SPECIAL_EXPLORED: case SPECIAL_SPOT: case SPECIAL_ROAD: break;
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
					for(short k = 0; k < univ.town.items.size(); k++)
						if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].item_loc == l) {
							univ.town.items[k].item_loc.x = spec.sd1;
							univ.town.items[k].item_loc.y = spec.sd2;
							if(i && spec.m3) {
								univ.town.items[k].contained = is_container(univ.town.items[k].item_loc);
								if(univ.town.is_crate(spec.sd1,spec.sd2) || univ.town.is_barrel(spec.sd1,spec.sd2))
									univ.town.items[k].held = true;
							}
						}
					break;
				case eSpecType::RECT_DESTROY_ITEMS:
					if(is_out())
						return;
					for(short k = 0; k < univ.town.items.size(); k++)
						if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].item_loc == l) {
							univ.town.items[k].variety = eItemType::NO_ITEM;
						}
					break;
				case eSpecType::RECT_CHANGE_TER:
					if(get_ran(1,1,100) <= spec.sd2){
						alter_space(l.x,l.y,spec.sd1);
						*redraw = true;
						draw_map(true);
					}
					break;
				case eSpecType::RECT_SWAP_TER:
					swap_ter(l.x,l.y,spec.sd1,spec.sd2);
					*redraw = true;
					draw_map(true);
					break;
				case eSpecType::RECT_TRANS_TER:
					ter = coord_to_ter(i,j);
					alter_space(l.x,l.y,univ.scenario.ter_types[ter].trans_to_what);
					*redraw = true;
					draw_map(true);
					break;
				case eSpecType::RECT_LOCK:
					ter = coord_to_ter(i,j);
					if(univ.scenario.ter_types[ter].special == eTerSpec::LOCKABLE){
						alter_space(l.x,l.y,univ.scenario.ter_types[ter].flag1);
						*redraw = true;
						draw_map(true);
					}
					break;
				case eSpecType::RECT_UNLOCK:
					ter = coord_to_ter(i,j);
					if(univ.scenario.ter_types[ter].special == eTerSpec::UNLOCKABLE){
						alter_space(l.x,l.y,univ.scenario.ter_types[ter].flag1);
						*redraw = true;
						draw_map(true);
						break;
					}
				case eSpecType::RECT_SET_EXPLORED:
					if(spec.sd1)
						make_explored(l.x, l.y);
					else take_explored(l.x, l.y);
					break;
				default:
					showError("Special node type \"" + (*cur_node.type).name() + "\" is either miscategorized or unimplemented!");
					break;
			}
		}
END:
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
}

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
				showError("Special outdoor enc. is out of range. Must be 0-3.");
				//set_sd = false;
			}
			else {
				l = global_to_local(univ.party.out_loc);
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
		case eSpecType::OUT_FORCE_TOWN: {
			l = {spec.ex2a, spec.ex2b};
			int i = 0;
			if(l.x < 0 || l.y < 0 || l.x >= 64 || l.y >= 64)
				i = 9;
			else if(spec.ex1b == 0) i = 2;
			else if(spec.ex1b == 4) i = 0;
			else if(spec.ex1b < 4) i = 3;
			else i = 1;
			if(i == 9) force_town_enter(spec.ex1a, l);
			start_town_mode(spec.ex1a, i);
		}
			break;
		default:
			showError("Special node type \"" + (*cur_node.type).name() + "\" is either miscategorized or unimplemented!");
			break;
	}
	
	if(check_mess) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
	}
}

void setsd(short a,short b,short val) {
	if(!univ.party.sd_legit(a,b)) {
		showError("The scenario attempted to change an out of range Stuff Done flag.");
		return;
	}
	PSD[a][b] = val;
}

void handle_message(eSpecCtx which_mode,short cur_type,short mess1,short mess2,short *a,short *b,std::string title,pic_num_t pic,ePicType pt) {
	if(pic == -1) {
		pic = univ.scenario.intro_pic;
		pt = PIC_SCEN;
	}
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
	where1 = is_out() ? univ.party.outdoor_corner.x + univ.party.i_w_c.x : univ.party.town_num;
	where2 = is_out() ? univ.party.outdoor_corner.y + univ.party.i_w_c.y : univ.party.town_num;
	std::string placename = is_out() ? univ.out->out_name : univ.town->town_name;
	cStrDlog display_strings(str1.c_str(), str2.c_str(),title,pic,pt,0);
	display_strings.setSound(57);
	display_strings.setRecordHandler(cStringRecorder(note_type).string1(mess1).string2(mess2).from(where1,where2).at(placename));
	display_strings.show();
}

void get_strs(std::string& str1,std::string& str2,short cur_type,short which_str1,short which_str2) {
	size_t num_strs;
	if(cur_type == 0)
		num_strs = univ.scenario.spec_strs.size();
	else if(cur_type == 1)
		num_strs = univ.out->spec_strs.size();
	else if(cur_type == 2)
		num_strs = univ.town->spec_strs.size();
	
	if(((which_str1 >= 0) && (which_str1 != minmax(0,num_strs,which_str1))) ||
		((which_str2 >= 0) && (which_str2 != minmax(0,num_strs,which_str2)))) {
		showError("The scenario attempted to access a message out of range.");
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
	
	if(which_str1 == -8)
		str1 = univ.get_buf();
	if(which_str2 == -8)
		str2 = univ.get_buf();
}

// This function sets/retrieves values to/from campaign flags
void set_campaign_flag(short sdf_a, short sdf_b, short cpf_a, short cpf_b, short str, bool get_send) {
	// get_send = false: Send value in SDF to Campaign Flag
	// get_send = true: Retrieve value from Campaign Flag and put in SDF
	try {
		if(str >= 0 && str < univ.scenario.spec_strs.size()) {
			std::string cp_id = univ.scenario.spec_strs[str];
			if(get_send)
				univ.party.stuff_done[sdf_a][sdf_b] = univ.cpn_flag(cpf_a, cpf_b, cp_id);
			else
				univ.cpn_flag(cpf_a, cpf_b, cp_id) = univ.party.stuff_done[sdf_a][sdf_b];
		} else {
			if(get_send)
				univ.party.stuff_done[sdf_a][sdf_b] = univ.cpn_flag(cpf_a, cpf_b);
			else
				univ.cpn_flag(cpf_a, cpf_b) = univ.party.stuff_done[sdf_a][sdf_b];
		}
	} catch(std::range_error x) {
		showError(x.what());
	}
}
