#include <windows.h>
#include <cstdio>
#include <cstring>
#include "global.h"
#include "boe.party.h"
#include "boe.town.h"
#include "boe.text.h"
#include "boe.infodlg.h"
#include "boe.items.h"
#include "boe.itemdata.h"
#include "boe.combat.h"
#include "boe.monster.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "tools/soundtool.h"
#include "boe.graphics.h"
#include "boe.fileio.h"
#include "boe.specials.h"
#include "boe.newgraph.h"
#include "boe.dlgutil.h"
#include "boe.actions.h"
#include "boe.graphutil.h"
#include "tools/dlogtool.h"
#include "tools/mathutil.h"

#include "globvar.h"

void start_split(short a, short b, short noise)
{
	party.stuff_done[SDF_IS_PARTY_SPLIT] = 1;
	party.stuff_done[SDF_PARTY_SPLIT_WHERE_PARTY_X] = c_town.p_loc.x;
	party.stuff_done[SDF_PARTY_SPLIT_WHERE_PARTY_Y] = c_town.p_loc.y;
	c_town.p_loc.x = a;
	c_town.p_loc.y = b;
	for (int i = 0; i < NUM_OF_PCS; i++)
		if (i != party.stuff_done[SDF_PARTY_SPLIT_PC] && adven[i].main_status > 0)
			adven[i].main_status += MAIN_STATUS_SPLIT;
	current_pc = party.stuff_done[SDF_PARTY_SPLIT_PC];
	update_explored(c_town.p_loc);
	center = c_town.p_loc;

	if (noise > 0)
		play_sound(10);
}

void end_split(short noise)
{
	if (party.stuff_done[SDF_IS_PARTY_SPLIT] == 0) {
		ASB("Party already together!");
		return;
		}

	c_town.p_loc.x = party.stuff_done[SDF_PARTY_SPLIT_WHERE_PARTY_X];
	c_town.p_loc.y = party.stuff_done[SDF_PARTY_SPLIT_WHERE_PARTY_Y];
	party.stuff_done[SDF_IS_PARTY_SPLIT] = 0;
	for (int i = 0; i < NUM_OF_PCS; i++)
		if (adven[i].main_status >= MAIN_STATUS_SPLIT)
			adven[i].main_status -= MAIN_STATUS_SPLIT;
	update_explored(c_town.p_loc);
	center = c_town.p_loc;
	if (noise > 0)
		play_sound(10);
	add_string_to_buf("You are reunited.");
}

Boolean handle_wandering_specials (short mode)
//short mode; // 0 - pre  1 - end by victory  2 - end by flight
// wanderin spec 99 -> generic spec
{
	short s1 = 0,s2 = 0,s3 = 0;
	location null_loc;

	if ((mode == 0) && (store_wandering_special.spec_on_meet >= 0)) { // When encountering
	run_special(SPEC_OUTDOOR_ENC,1,store_wandering_special.spec_on_meet,null_loc,&s1,&s2,&s3);
	if (s1 > 0)
		return false;
	}

	if ((mode == 1) && (store_wandering_special.spec_on_win >= 0))  {// After defeating
	run_special(SPEC_WIN_ENCOUNTER,1,store_wandering_special.spec_on_win,null_loc,&s1,&s2,&s3);
		}
	if ((mode == 2) && (store_wandering_special.spec_on_flee >= 0))  {// After fleeing like a buncha girly men
	run_special(SPEC_FLEE_ENCOUNTER,1,store_wandering_special.spec_on_flee,null_loc,&s1,&s2,&s3);
		}
    if(s3 > 0)
        initiate_redraw();
return true;
}


Boolean check_special_terrain(location where_check,short mode,short which_pc,short *spec_num,
	Boolean *forced)
//short mode; // 0 - out 1 - town 2 - combat
{
	unsigned char ter;
	short r1,i,choice,door_pc,ter_special,ter_flag1,ter_flag2,dam_type = 0,pic_type = 0,ter_pic = 0;
	Boolean can_enter = true;
	location out_where,from_loc,to_loc;
	short s1 = 0,s2 = 0,s3 = 0;
	char create_line[128];

	*spec_num = -1;
	*forced = false;

	switch (mode) {
		case 0:
			ter = out[where_check.x][where_check.y];
			from_loc = party.p_loc;
			break;
		case 1:
			ter = t_d.terrain[where_check.x][where_check.y];
			from_loc = c_town.p_loc;
			break;
		case 2:
			ter = combat_terrain[where_check.x][where_check.y];
			from_loc = pc_pos[current_pc];
			break;
		}
	ter_special = scenario.ter_types[ter].special;
	ter_flag1 = scenario.ter_types[ter].flag1;
	ter_flag2 = scenario.ter_types[ter].flag2;
	ter_pic = scenario.ter_types[ter].picture;

		if ((mode > 0) && (ter_special >= TER_SPEC_CONVEYOR_NORTH) &&
			(ter_special <= TER_SPEC_CONVEYOR_WEST)) {
			if (
				((ter_special == TER_SPEC_CONVEYOR_NORTH) && (where_check.y > from_loc.y)) ||
				((ter_special == TER_SPEC_CONVEYOR_EAST) && (where_check.x < from_loc.x)) ||
				((ter_special == TER_SPEC_CONVEYOR_SOUTH) && (where_check.y < from_loc.y)) ||
				((ter_special == TER_SPEC_CONVEYOR_WEST) && (where_check.x > from_loc.x)) ) {
					ASB("The moving floor prevents you.");
					return false;
					}
			}

			if (mode == 0) {
				out_where = where_check.toLocal();

				for (i = 0; i < 18; i++)
					if (same_point(out_where,outdoors[party.i_w_c.x][party.i_w_c.y].special_locs[i]) == true) {
						*spec_num = outdoors[party.i_w_c.x][party.i_w_c.y].special_id[i];
						if ((*spec_num >= 0) &&
							(outdoors[party.i_w_c.x][party.i_w_c.y].specials[*spec_num].type == SPEC_SECRET_PASSAGE))
								*forced = true;
						// call special
						run_special(mode,1,outdoors[party.i_w_c.x][party.i_w_c.y].special_id[i],out_where,&s1,&s2,&s3);
						if (s1 > 0)
							can_enter = false;
						erase_out_specials();
						put_pc_screen();
						put_item_screen(stat_window,0);
					}
				}

	if ((is_combat()) && (((ter_pic <= 207) && (ter_pic >= 212)) || (ter_pic == 406))) {
		ASB("Move: Can't trigger this special in combat.");
		return false;
		}

	if (((mode == 1) || ((mode == 2) && (which_combat_type == 1)))
	&& (special(where_check.x,where_check.y))) {
			if (is_force_barrier(where_check.x,where_check.y)) {
				add_string_to_buf("  Magic barrier!               ");
				return false;
				}
			for (i = 0; i < 50; i++)
				if ((same_point(where_check,c_town.town.special_locs[i]) == true))
					//GK && (c_town.town.spec_id[i] >= 0))
					{
					if (c_town.town.specials[c_town.town.spec_id[i]].type == SPEC_SECRET_PASSAGE) {
						*forced = true;
						}
					*spec_num = c_town.town.spec_id[i];
					if ((is_blocked(where_check) == false) || (ter_special == TER_SPEC_CHANGE_WHEN_STEP_ON) //is_blocked doesn't check if aboard a boat...
	 					|| (ter_special == TER_SPEC_CALL_LOCAL_SPECIAL) || (ter_special == TER_SPEC_CALL_SCENARIO_SPECIAL)
	 					|| ((PSD[SDF_COMPATIBILITY_TRIGGER_SPECIALS_ON_BOAT] == true) && (party.in_boat >= 0) && (scenario.ter_types[ter].boat_over == true))) { //...so, check if in boat and terrain allows to boat over
						give_help(54,0,0);
						run_special(mode,2,c_town.town.spec_id[i],where_check,&s1,&s2,&s3);
						if (s1 > 0)
							can_enter = false;
						}
					}
			put_pc_screen();
			put_item_screen(stat_window,0);
		}

	if((mode == 2) && town_boat_there(where_check) < 30){//can't enter boats while in combat mode
        sprintf (create_line, "Blocked: %s",dir_string[set_direction(pc_pos[current_pc], where_check)]);
		add_string_to_buf(create_line);
        can_enter = false;
        }
	if (can_enter == false)
		return false;

	if ((!is_out()) && (overall_mode < MODE_TALKING)) {
	check_fields(where_check,mode,which_pc);

	if (is_web(where_check.x,where_check.y)) {
		add_string_to_buf("  Webs!               ");
		if (mode < 2) {
			suppress_stat_screen = true;
			for (i = 0; i < 6; i++) {
				r1 = get_ran(1,2,3);
				adven[i].web(r1);
				}
			suppress_stat_screen = true;
			put_pc_screen();
			}
			else adven[current_pc].web(get_ran(1,2,3));
		take_web(where_check.x,where_check.y);
		}
	if (is_force_barrier(where_check.x,where_check.y)) {
		add_string_to_buf("  Magic barrier!               ");
		can_enter = false;
		}
	if (is_crate(where_check.x,where_check.y)) {
		add_string_to_buf("  You push the crate.");
		to_loc = push_loc(from_loc,where_check);
		take_crate((short) where_check.x,(short) where_check.y);
		if (to_loc.x > 0)
			make_crate((short) to_loc.x,(short) to_loc.y);
		for (i = 0; i < NUM_TOWN_ITEMS; i++)
			if ((t_i.items[i].variety > ITEM_TYPE_NO_ITEM) && (same_point(t_i.items[i].item_loc,where_check))
			 && (t_i.items[i].isContained()))
			 	t_i.items[i].item_loc = to_loc;
		}
	if (is_barrel(where_check.x,where_check.y)) {
		add_string_to_buf("  You push the barrel.");
		to_loc = push_loc(from_loc,where_check);
		take_barrel((short) where_check.x,(short) where_check.y);
		if (to_loc.x > 0)
			make_barrel((short) to_loc.x,(short) to_loc.y);
		for (i = 0; i < NUM_TOWN_ITEMS; i++)
			if ((t_i.items[i].variety > ITEM_TYPE_NO_ITEM) && (same_point(t_i.items[i].item_loc,where_check))
			 && (t_i.items[i].isContained()))
			 	t_i.items[i].item_loc = to_loc;
		}
		}


	switch (ter_special) {
		case TER_SPEC_CHANGE_WHEN_STEP_ON:
			alter_space(where_check.x,where_check.y,ter_flag1);
			if (ter_flag2 < 200) {
				play_sound(-1 * ter_flag2);
				}
			give_help(47,65,0);
			if (scenario.ter_types[ter].blockage > 2)
				can_enter = false;
			break;
		case TER_SPEC_DOES_FIRE_DAMAGE: case TER_SPEC_DOES_COLD_DAMAGE: case TER_SPEC_DOES_MAGIC_DAMAGE:
			if (party.isFlying() || party.in_boat >= 0 || party.in_horse >= 0 || (mode ? town_boat_there(where_check) : out_boat_there(where_check)) < 30)
				break;
			if (ter_special == TER_SPEC_DOES_FIRE_DAMAGE) {
				add_string_to_buf("  It's hot!");
				dam_type = DAMAGE_FIRE; pic_type = 0;
				if (party.stuff_done[SDF_LAVAWALK] > 0) {
					add_string_to_buf("  It doesn't affect you.");
					break;
					}
				}
			if (ter_special == TER_SPEC_DOES_COLD_DAMAGE) {
				add_string_to_buf("  You feel cold!");
				dam_type = DAMAGE_COLD; pic_type = 4;
				}
			if (ter_special == TER_SPEC_DOES_MAGIC_DAMAGE) {
				add_string_to_buf("  Something shocks you!");
				dam_type = DAMAGE_MAGIC; pic_type = 1;
				}
			r1 = get_ran(ter_flag2,dam_type,ter_flag1);
			if (mode < 2)
				adven.damage(r1,dam_type);
			fast_bang = 1;
			if (mode == 2)
				adven[which_pc].damage(r1,dam_type,-1);
			fast_bang = 0;
			break;
		case TER_SPEC_POISON_LAND: case TER_SPEC_DISEASED_LAND:
			if (party.isFlying() || party.in_boat >= 0 || (mode ? town_boat_there(where_check) : out_boat_there(where_check)) < 30)
			    break;
			one_sound(17);
			if (mode < 2) {
				for (i = 0; i < 6; i++)
					if (adven[i].isAlive())
						{
						if (get_ran(1,1,100) <= ter_flag2) {
							if (ter_special == TER_SPEC_POISON_LAND)
								adven[i].poison(ter_flag1);
                            else
                                adven[i].disease(ter_flag1);
							}
						}
				}
			//print_nums(1,which_pc,current_pc);
			if (mode == 2) {
						if (get_ran(1,1,100) <= ter_flag2){
							if (ter_special == TER_SPEC_POISON_LAND)
								adven[which_pc].poison(ter_flag1);
                            else
                                adven[which_pc].disease(ter_flag1);
							}
					}
			break;
		case TER_SPEC_CALL_LOCAL_SPECIAL: // local special
		  if(mode == 0){
			run_special(mode,1,ter_flag1,where_check,&s1,&s2,&s3);
					if (s1 > 0)
						can_enter = false;
            }
		  else if(mode == 1){
			run_special(mode,2,ter_flag1,where_check,&s1,&s2,&s3);
					if (s1 > 0)
						can_enter = false;
                    }
          else if(mode == 2){
            if(which_combat_type == 0)//outdoor combat
       			run_special(mode,1,ter_flag1,where_check,&s1,&s2,&s3);
            else
       			run_special(mode,2,ter_flag1,where_check,&s1,&s2,&s3);
            }
			break;
		case TER_SPEC_CALL_SCENARIO_SPECIAL: // global special
			run_special(mode,0,ter_flag1,where_check,&s1,&s2,&s3);
					if (s1 > 0)
						can_enter = false;
			break;


		// Locked doors
		case TER_SPEC_UNLOCKABLE_TERRAIN: case TER_SPEC_UNLOCKABLE_BASHABLE:
			if (is_combat()) {  // No lockpicking in combat
				add_string_to_buf("  Can't enter: It's locked.");
				break;
				}

			// See what party wants to do.
			choice = fancy_choice_dialog(993,0);

			can_enter = false;
			if (choice == 1) break;
			if ((door_pc = select_pc(1,0)) < 6) {
				if (choice == 2)
					where_check.pickLock(door_pc);
					else where_check.bashDoor(door_pc);
				}
			break;


		}

	// Action may change terrain, so update what's been seen
	if (is_town())
		update_explored(c_town.p_loc);
	if (is_combat())
		update_explored(pc_pos[current_pc]);

    if (s3 > 0)
        initiate_redraw();

	return can_enter;
}

// This procedure find the effects of fields that would affect a PC who moves into
// a space or waited in that same space
void check_fields(location where_check,short mode,short which_pc)
//mode; // 0 - out 1 - town 2 - combat
{
	short r1,i;

	if (is_out()) return;
	if (is_town()) fast_bang = 1;
	if (is_fire_wall(where_check.x,where_check.y)) {
			add_string_to_buf("  Fire wall!               ");
			r1 = get_ran(1,1,6) + 1;
			if (mode == 2)
				adven[which_pc].damage(r1,1,-1);
				if (overall_mode < MODE_COMBAT)
					boom_space(party.p_loc,overall_mode,0,r1,5);
		}
	if (is_force_wall(where_check.x,where_check.y)) {
			add_string_to_buf("  Force wall!               ");
			r1 = get_ran(2,1,6);
			if (mode == 2)
				adven[which_pc].damage(r1,3,-1);
				if (overall_mode < MODE_COMBAT)
					boom_space(party.p_loc,overall_mode,1,r1,12);
		}
	if (is_ice_wall(where_check.x,where_check.y)) {
			add_string_to_buf("  Ice wall!               ");
			r1 = get_ran(2,1,6);
			if (mode == 2)
				adven[which_pc].damage(r1,5,-1);
				if (overall_mode < MODE_COMBAT)
					boom_space(party.p_loc,overall_mode,4,r1,7);
		}
	if (is_blade_wall(where_check.x,where_check.y)) {
			add_string_to_buf("  Blade wall!               ");
			r1 = get_ran(4,1,8);
			if (mode == 2)
				adven[which_pc].damage(r1,0,-1);
				if (overall_mode < MODE_COMBAT)
					boom_space(party.p_loc,overall_mode,3,r1,2);
		}
	if (is_quickfire(where_check.x,where_check.y)) {
			add_string_to_buf("  Quickfire!               ");
			r1 = get_ran(2,1,8);
			if (mode == 2)
				adven[which_pc].damage(r1,1,-1);
				if (overall_mode < MODE_COMBAT)
					boom_space(party.p_loc,overall_mode,0,r1,5);
		}
	if (is_scloud(where_check.x,where_check.y)) {
		add_string_to_buf("  Stinking cloud!               ");
		if (mode < 2) {
			suppress_stat_screen = true;
			for (i = 0; i < 6; i++) {
				r1 = get_ran(1,2,3);
				adven[i].curse(r1);
				}
			suppress_stat_screen = false;
			put_pc_screen();
			}
			else adven[current_pc].curse(get_ran(1,2,3));
		}
	if (is_sleep_cloud(where_check.x,where_check.y)) {
		add_string_to_buf("  Sleep cloud!               ");
		if (mode < 2) {
			suppress_stat_screen = true;
			for (i = 0; i < NUM_OF_PCS; i++) {
				adven[i].sleep(3,11,0);
				}
			suppress_stat_screen = false;
			put_pc_screen();
			}
			else adven[current_pc].sleep(3,11,0);
		}
	if (is_fire_barrier(where_check.x,where_check.y)) {
			add_string_to_buf("  Magic barrier!               ");
			r1 = get_ran(2,1,10);
			if (mode < 2)
				adven.damage(r1,3);
			if (mode == 2)
				adven[which_pc].damage(r1,3,-1);
				if (overall_mode < MODE_COMBAT)
					boom_space(party.p_loc,overall_mode,1,r1,12);
		}
	fast_bang = 0;
}
void use_item(short pc,short item)
{
	Boolean take_charge = true,inept_ok = false;
	short abil,level,i,j,item_use_code,str,type,which_stat,r1;
	char to_draw[60];
	location user_loc;
//creature_data_type *which_m;
effect_pat_type s = {{{0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0},
						{0,0,0,0,1,0,0,0,0},
						{0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0},
						{0,0,0,0,0,0,0,0,0}}};
	abil = adven[pc].items[item].ability;
	level = adven[pc].items[item].item_level;

	item_use_code = abil_chart[abil];
	if (item_use_code >= 10) {
		item_use_code -= 10;
		inept_ok = true;
		}

	if (is_out()) user_loc = party.p_loc;
	if (is_town()) user_loc = c_town.p_loc;
	if (is_combat()) user_loc = pc_pos[current_pc];

	if (item_use_code == 4) {
		add_string_to_buf("Use: Can't use this item.       ");
		take_charge = false;
		}
	if ((adven[pc].traits[TRAIT_MAGICALLY_INEPT] == true) && (inept_ok == false)){
		add_string_to_buf("Use: Can't - magically inept.       ");
		take_charge = false;
		}

	if (take_charge == true) {
			if ((overall_mode == MODE_OUTDOORS) && (item_use_code > 0) && (item_use_code != 5)) {
				add_string_to_buf("Use: Not while outdoors.         ");
				take_charge = false;
				}
			if ((overall_mode == MODE_TOWN) && (item_use_code == 1)) {
				add_string_to_buf("Use: Not while in town.         ");
				take_charge = false;
				}
			if ((overall_mode == MODE_COMBAT) && (item_use_code == 2)) {
				add_string_to_buf("Use: Not in combat.         ");
				take_charge = false;
				}
			if ((overall_mode != MODE_OUTDOORS) && (item_use_code == 5)){
				add_string_to_buf("Use: Only outdoors.           ");
				take_charge = false;
				}
		}
	if (take_charge == true) {
		if (adven[pc].items[item].isIdent() == false)
			sprintf(to_draw, "Use: %s",adven[pc].items[item].name);
			else sprintf(to_draw, "Use: %s",adven[pc].items[item].full_name);
		add_string_to_buf(to_draw);

		if ((adven[pc].items[item].variety == ITEM_TYPE_POTION))// &&
//		      (adven[pc].items[item].graphic_num >= 50) && (adven[pc].items[item].graphic_num <= 52))
		      	play_sound(56);

		str = adven[pc].items[item].ability_strength;
		store_item_spell_level = str * 2 + 1;
		type = adven[pc].items[item].magic_use_type;

		switch (abil) {
			case ITEM_POISON_WEAPON: // poison weapon
				take_charge = poison_weapon(pc,str,0);
				break;
			case ITEM_BLESS_CURSE: case ITEM_HASTE_SLOW:  case ITEM_AFFECT_INVULN: case ITEM_AFFECT_MAGIC_RES:
			case ITEM_AFFECT_WEB: case ITEM_AFFECT_SANCTUARY: case ITEM_AFFECT_MARTYRS_SHIELD:
				switch (abil) {
					case ITEM_BLESS_CURSE:
						play_sound(4);
						which_stat = STATUS_BLESS_CURSE;
						if (type % 2 == 1) {
							ASB("  You feel awkward."); str = str * -1;}
							else ASB("  You feel blessed.");
						break;
					case ITEM_HASTE_SLOW:
						play_sound(75);
						which_stat = STATUS_HASTE_SLOW;
						if (type % 2 == 1) {
							ASB("  You feel sluggish."); str = str * -1;}
							else ASB("  You feel speedy.");
						break;
					case ITEM_AFFECT_INVULN:
						play_sound(68);
						which_stat = STATUS_INVULNERABLE;
						if (type % 2 == 1) {
							ASB("  You feel odd."); str = str * -1;}
							else ASB("  You feel protected.");
						break;
					case ITEM_AFFECT_MAGIC_RES:
						play_sound(51);
						which_stat = STATUS_MAGIC_RESISTANCE;
						if (type % 2 == 1) {
							ASB("  You feel odd."); str = str * -1;}
							else ASB("  You feel protected.");
						break;
					case ITEM_AFFECT_WEB:
						which_stat = STATUS_WEBS;
						if (type % 2 == 1)
							ASB("  You feel sticky.");
							else {
								ASB("  Your skin tingles."); str = str * -1;}
						break;
					case ITEM_AFFECT_SANCTUARY:
						play_sound(43);
						which_stat = STATUS_INVISIBLE;
						if (type % 2 == 1) {
							ASB("  You feel exposed."); str = str * -1;}
							else ASB("  You feel obscure.");
						break;
					case ITEM_AFFECT_MARTYRS_SHIELD:
						play_sound(43);
						which_stat = STATUS_MARTYRS_SHIELD;
						if (type % 2 == 1) {
							ASB("  You feel dull."); str = str * -1;}
							else ASB("  You start to glow slightly.");
						break;
					}
				if (type > 1)
					adven.affect(which_stat,str);
					else affect_pc(pc,which_stat,str);
				break;
			case ITEM_AFFECT_POISON:
				switch (type) {
					case 0: ASB("  You feel better."); adven[pc].cure(str); break;
					case 1: ASB("  You feel ill."); adven[pc].poison(str); break;
					case 2: ASB("  You all feel better."); adven.cure(str); break;
					case 3: ASB("  You all feel ill."); adven.poison(str); break;
					}
				break;
			case ITEM_AFFECT_DISEASE:
				switch (type) {
					case 0: ASB("  You feel healthy."); affect_pc(pc,7,-1 * str); break;
					case 1: ASB("  You feel sick."); adven[pc].disease(str); break;
					case 2: ASB("  You all feel healthy."); adven.affect(7,-1 * str); break;
					case 3: ASB("  You all feel sick."); adven.disease(str); break;
					}
				break;
			case ITEM_AFFECT_DUMBFOUND:
				switch (type) {
					case 0: ASB("  You feel clear headed."); affect_pc(pc,9,-1 * str); break;
					case 1: ASB("  You feel confused."); adven[pc].dumbfound(str); break;
					case 2: ASB("  You all feel clear headed."); adven.affect(9,-1 * str); break;
					case 3: ASB("  You all feel confused."); adven.dumbfound(str); break;
					}
				break;
			case ITEM_AFFECT_SLEEP:
				switch (type) {
					case 0: ASB("  You feel alert."); affect_pc(pc,11,-1 * str); break;
					case 1: ASB("  You feel very tired."); adven[pc].sleep(str + 1,11,200); break;
					case 2: ASB("  You all feel alert."); adven.affect(11,-1 * str); break;
					case 3: ASB("  You all feel very tired."); for (i = 0; i < 6; i++) adven[i].sleep(str + 1,11,200); break;
					}
				break;
			case ITEM_AFFECT_PARALYSIS:
				switch (type) {
					case 0: ASB("  You find it easier to move."); affect_pc(pc,12,-1 * str * 100); break;
					case 1: ASB("  You feel very stiff."); adven[pc].sleep(str * 20 + 10,12,200); break;
					case 2: ASB("  You all find it easier to move."); adven.affect(12,-1 * str * 100); break;
					case 3: ASB("  You all feel very stiff."); for (i = 0; i < 6; i++) adven[i].sleep(str * 20 + 10,12,200); break;
					}
				break;
			case ITEM_AFFECT_ACID:
				switch (type) {
					case 0: ASB("  Your skin tingles pleasantly."); affect_pc(pc,13,-1 * str); break;
					case 1: ASB("  Your skin burns!"); adven[pc].acid(str); break;
					case 2: ASB("  You all tingle pleasantly."); adven.affect(13,-1 * str); break;
					case 3: ASB("  Everyone's skin burns!"); for (i = 0; i < 6; i++) adven[i].acid(str); break;
					}
				break;
			case ITEM_BLISS:
				switch (type) {
					case 0:
					case 1:
						ASB("  You feel wonderful!");
						adven[pc].heal(str * 20);
						affect_pc(pc,1,str);
						break;
					case 2:
					case 3:
						ASB("  Everyone feels wonderful!");
						adven.heal(str * 20);
						adven.affect(1, str);
						break;
					}
				break;
			case ITEM_AFFECT_EXPERIENCE:
				switch (type) {
					case 0: ASB("  You feel much smarter."); adven[pc].giveXP(str * 5); break;
					case 1: ASB("  You feel forgetful."); adven[pc].drainXP(str * 5); break;
					case 2: ASB("  You all feel much smarter."); adven.giveXP(str * 5); break;
					case 3: ASB("  You all feel forgetful."); adven.drainXP(str * 5); break;
					}
				break;
			case ITEM_AFFECT_SKILL_POINTS:
				play_sound(68);
				switch (type) {
					case 0: ASB("  You feel much smarter."); adven[pc].skill_pts += str; break;
					case 1: ASB("  You feel forgetful."); adven[pc].skill_pts = max(0,adven[pc].skill_pts - str); break;
					case 2: ASB("  You all feel much smarter."); for (i = 0; i < 6; i++) adven[i].skill_pts += str; break;
					case 3: ASB("  You all feel forgetful."); for (i = 0; i < 6; i++) adven[i].skill_pts = max(0,adven[i].skill_pts - str); break;
					}
				break;
			case ITEM_AFFECT_HEALTH:
				switch (type) {
					case 0: ASB("  You feel better."); adven[pc].heal(str * 20); break;
					case 1: ASB("  You feel sick."); adven[pc].damage(20 * str,4,0); break;
					case 2: ASB("  You all feel better."); adven.heal(str * 20); break;
					case 3: ASB("  You all feel sick."); adven.damage(20 * str,4); break;
					}
				break;
			case ITEM_AFFECT_SPELL_POINTS:
				switch (type) {
					case 0: ASB("  You feel energized."); adven[pc].restoreSP(str * 5); break;
					case 1: ASB("  You feel drained."); adven[pc].cur_sp = max(0,adven[pc].cur_sp - str * 5); break;
					case 2: ASB("  You all feel energized."); adven.restoreSP(str * 5); break;
					case 3: ASB("  You all feel drained."); for (i = 0; i < 6; i++) adven[i].cur_sp = max(0,adven[i].cur_sp - str * 5); break;
					}
				break;
			case ITEM_DOOM:
				switch (type) {
					case 0:
					case 1: ASB("  You feel terrible.");
						adven[pc].drainXP(str * 5);
						adven[pc].damage(20 * str,4,0);
						adven[pc].disease(2 * str);
						adven[pc].dumbfound(2 * str);
						break;
					case 2:
					case 3: ASB("  You all feel terrible."); for (i = 0; i < 6; i++) {
						adven[i].drainXP(str * 5);
						adven[i].damage(20 * str,4,0);
						adven[i].disease(2 * str);
						adven[i].dumbfound(2 * str);}
						break;
					}
				break;
			case ITEM_LIGHT:
				ASB("  You have more light."); increase_light(50 * str);
				break;
			case ITEM_STEALTH:
				ASB("  Your footsteps become quieter."); party.stuff_done[SDF_STEALTH] += 5 * str;
				break;
			case ITEM_FIREWALK:
				ASB("  You feel chilly."); party.stuff_done[SDF_LAVAWALK] += 2 * str;
				break;
			case ITEM_FLYING:
				if (party.stuff_done[SDF_FLYING] > 0) {
					add_string_to_buf("  Not while already flying.          ");
					break;
					}
				if (party.in_boat >= 0)
					add_string_to_buf("  Leave boat first.             ");
				else if (party.in_horse >= 0)////
					add_string_to_buf("  Leave horse first.             ");
				else {
					ASB("  You rise into the air!"); party.stuff_done[SDF_FLYING] += str;
					}
				break;
			case ITEM_MAJOR_HEALING:
				switch (type) {
					case 0:
					case 1:
						ASB("  You feel wonderful.");
						adven[pc].heal(200);
						adven[pc].cure(8);
						break;
					case 2:
					case 3:
						ASB("  You all feel wonderful.");
						adven.heal(200);
						adven.cure(8);
						break;
					}
				break;
			case ITEM_CALL_SPECIAL:
                switch(type){
                    case 0:
                        if((is_town()) || ((is_combat()) && (which_combat_type == 1)))
                            run_special(SPEC_USE_SPEC_ITEM,2,str,location(),&i,&j,&r1);// Call town special
                        else
                            run_special(SPEC_USE_SPEC_ITEM,1,str,location(),&i,&j,&r1);// Call outdoor special
                    break;
                    case 1:
                        if((is_town()) || ((is_combat()) && (which_combat_type == 1)))
                            run_special(SPEC_USE_SPEC_ITEM,2,str,location(),&i,&j,&r1);// Call town special
                        else
                            run_special(SPEC_USE_SPEC_ITEM,0,str,location(),&i,&j,&r1);// Call scenario special
                    break;
                    case 2:
                        if((is_town()) || ((is_combat()) && (which_combat_type == 1)))
                            run_special(SPEC_USE_SPEC_ITEM,0,str,location(),&i,&j,&r1);// Call scenario special
                        else
                            run_special(SPEC_USE_SPEC_ITEM,1,str,location(),&i,&j,&r1);// Call outdoor special
                    break;
                    case 3:
                        run_special(SPEC_USE_SPEC_ITEM,0,str,location(),&i,&j,&r1);// Call scenario special
                    break;
                }
            break;

			// spell effects
			case ITEM_SPELL_FLAME:
				add_string_to_buf("  It fires a bolt of flame.");
				start_spell_targeting(1011);
				break;
			case ITEM_SPELL_FIREBALL:
				add_string_to_buf("  It shoots a fireball.         ");
				start_spell_targeting(1022);
				break;
			case ITEM_SPELL_FIRESTORM:
				add_string_to_buf("  It shoots a huge fireball. ");
				start_spell_targeting(1040);
				break;
			case ITEM_SPELL_KILL:
				add_string_to_buf("  It shoots a black ray.  ");
				start_spell_targeting(1048);
				break;
			case ITEM_SPELL_ICE_BOLT:
				add_string_to_buf("  It fires a ball of ice.   ");
				start_spell_targeting(1031);
				break;
			case ITEM_SPELL_SLOW:
				add_string_to_buf("  It fires a purple ray.   ");
				start_spell_targeting(1012);
				break;
			case ITEM_SPELL_SHOCKWAVE:
				add_string_to_buf("  The ground shakes!        ");
				do_shockwave(pc_pos[current_pc]);
				break;
			case ITEM_SPELL_DISPEL_UNDEAD:
				add_string_to_buf("  It shoots a white ray.   ");
				start_spell_targeting(1132);
				break;
			case ITEM_SPELL_RAVAGE_SPIRIT:
				add_string_to_buf("  It shoots a golden ray.   ");
				start_spell_targeting(1155);
				break;
			case ITEM_SPELL_SUMMONING:
				if (summon_monster(str,user_loc,50,2) == false)
					add_string_to_buf("  Summon failed.");
				break;
			case ITEM_SPELL_MASS_SUMMONING:
				r1 = get_ran(6,1,4);
				j = get_ran(1,3,5);
				for (i = 0; i < j; i++)
					if (summon_monster(str,user_loc,r1,2) == false)
						add_string_to_buf("  Summon failed.");
				break;
			case ITEM_SPELL_ACID_SPRAY:
				add_string_to_buf("  Acid sprays from the tip!   ");
				start_spell_targeting(1068);
				break;
			case ITEM_SPELL_STINKING_CLOUD:
				add_string_to_buf("  It creates a cloud of gas.   ");
				start_spell_targeting(1066);
				break;
			case ITEM_SPELL_SLEEP_FIELD:
				add_string_to_buf("  It creates a shimmering cloud.   ");
				start_spell_targeting(1019);
				break;
			case ITEM_SPELL_VENOM:
				add_string_to_buf("  A green ray emerges.        ");
				start_spell_targeting(1030);
				break;
			case ITEM_SPELL_SHOCKSTORM:
				add_string_to_buf("  Sparks fly.");
				start_spell_targeting(1044);
				break;
			case ITEM_SPELL_PARALYSIS:
				add_string_to_buf("  It shoots a silvery beam.   ");
				start_spell_targeting(1069);
				break;
			case ITEM_SPELL_WEB_SPELL:
				add_string_to_buf("  It explodes!");
				start_spell_targeting(1065);
				break;
			case ITEM_SPELL_STRENGTHEN_TARGET:
				add_string_to_buf("  It shoots a fiery red ray.   ");
				start_spell_targeting(1062);
				break;
			case ITEM_SPELL_QUICKFIRE:
				add_string_to_buf("Fire pours out!");
				make_quickfire(user_loc.x,user_loc.y);
				break;
			case ITEM_SPELL_MASS_CHARM:
				ASB("It throbs, and emits odd rays.");
				for (i = 0; i < T_M; i++) {
						if ((c_town.monst.dudes[i].active != 0) && (c_town.monst.dudes[i].attitude % 2 == 1)
						 && (dist(pc_pos[current_pc],c_town.monst.dudes[i].m_loc) <= 8)
						 && (can_see(pc_pos[current_pc],c_town.monst.dudes[i].m_loc,0) < 5))
						 	{
								c_town.monst.dudes[i].charm(0,0,8);
							}
						}
				break;
			case ITEM_SPELL_MAGIC_MAP:
				if ((c_town.town.defy_scrying == 1) && (c_town.town.defy_mapping == 1)) {
					add_string_to_buf("  It doesn't work.");
					break;
					}
				add_string_to_buf("  You have a vision.            ");
				for (i = 0; i < town_size[town_type]; i++)
					for (j = 0; j < town_size[town_type]; j++)
						make_explored(i,j);
				clear_map();
				break;
			case ITEM_SPELL_DISPEL_BARRIER:
				add_string_to_buf("  It fires a blinding ray.");
				add_string_to_buf("  Target spell.    ");
				overall_mode = MODE_TOWN_TARGET;
				current_pat = s;
				set_town_spell(1041,current_pc);
			break;
			case ITEM_SPELL_MAKE_ICE_WALL:
				add_string_to_buf("  It shoots a blue sphere.   ");
				start_spell_targeting(1064);
				break;
			case ITEM_SPELL_CHARM_SPELL:
				add_string_to_buf("  It fires a lovely, sparkling beam.");
				start_spell_targeting(1117);
				break;
			case ITEM_SPELL_ANTIMAGIC_CLOUD:
				add_string_to_buf("  Your hair stands on end.   ");
				start_spell_targeting(1051);
				break;
			}
// Special spells:
//   62 - Carrunos
//	 63 - Summon Rat <= doesn't exist anymore
//	 64 - Ice Wall Balls
//	 65 - Goo Bomb
//   66 - Foul Vapors
//   67 - Sleep cloud <= doesn't exist anymore
//	 68 - Spray Acid
//	 69 - Paralyze
//   70 - mass sleep <= doesn't exist anymore
		}

	put_pc_screen();
	if ((take_charge == true) && (adven[pc].items[item].charges > 0))
		adven[pc].removeCharge(item);
	if (take_charge == false) {
		draw_terrain(0);
		put_item_screen(stat_window,0);
		}
}

// Returns true if an action is actually carried out. This can only be reached in town.
Boolean use_space(location where)
{
	unsigned char ter;
	short i;
	location from_loc,to_loc;

	ter = t_d.terrain[where.x][where.y];
	from_loc = c_town.p_loc;

	add_string_to_buf("Use...");

	if (is_web(where.x,where.y)) {
		add_string_to_buf("  You clear the webs.");
		take_web(where.x,where.y);
		return true;
		}
	if (is_crate(where.x,where.y)) {
		to_loc = push_loc(from_loc,where);
		if (same_point(from_loc,to_loc) == true) {
			add_string_to_buf("  Can't push crate.");
			return false;
			}
		add_string_to_buf("  You push the crate.");
		take_crate((short) where.x,(short) where.y);
		make_crate((short) to_loc.x,(short) to_loc.y);
		for (i = 0; i < NUM_TOWN_ITEMS; i++)
			if ((t_i.items[i].variety > ITEM_TYPE_NO_ITEM) && (same_point(t_i.items[i].item_loc,where))
			 && (t_i.items[i].isContained()))
			 	t_i.items[i].item_loc = to_loc;
		}
	if (is_barrel(where.x,where.y)) {
		to_loc = push_loc(from_loc,where);
		if (same_point(from_loc,to_loc) == true) {
			add_string_to_buf("  Can't push barrel.");
			return false;
			}
		add_string_to_buf("  You push the barrel.");
		take_barrel((short) where.x,(short) where.y);
		make_barrel((short) to_loc.x,(short) to_loc.y);
		for (i = 0; i < NUM_TOWN_ITEMS; i++)
			if ((t_i.items[i].variety > ITEM_TYPE_NO_ITEM) && (same_point(t_i.items[i].item_loc,where))
			 && (t_i.items[i].isContained()))
			 	t_i.items[i].item_loc = to_loc;
		}

	switch (scenario.ter_types[ter].special) {
		case TER_SPEC_CHANGE_WHEN_USED:
			if (same_point(where,from_loc) == true) {
				add_string_to_buf("  Not while on space.");
				return false;
				}
			add_string_to_buf("  OK.");
			alter_space(where.x,where.y,scenario.ter_types[ter].flag1);
			play_sound(scenario.ter_types[ter].flag2);
			return true;
			break;
		case TER_SPEC_CALL_SPECIAL_WHEN_USED: // call special
			run_special(SPEC_USE_SPACE,0,scenario.ter_types[ter].flag1,where,&i,&i,&i);
			return true;
			break;
		}
	add_string_to_buf("  Nothing to use.");

	return false;
}

// Note ... if this is a container, the code must first process any specials. If
//specials return false, can't get items inside. If true, can get items inside.
// Can't get items out in combat.
Boolean adj_town_look(location where)
{
	unsigned char terrain;
	Boolean can_open = true,item_there = false,got_special = false;
	short i = 0, s1 = 0, s2 = 0, s3 = 0;

	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if ((t_i.items[i].variety > ITEM_TYPE_NO_ITEM) && (t_i.items[i].isContained()) &&
			(same_point(where,t_i.items[i].item_loc) == true))
				item_there = true;

	terrain = t_d.terrain[where.x][where.y];
	if (special(where.x,where.y)) {// && (get_blockage(terrain) > 0)) {
		if (adjacent(c_town.p_loc,where) == false)
			add_string_to_buf("  Not close enough to search.");
			else {
				for (i = 0; i < 50; i++)
					if (same_point(where,c_town.town.special_locs[i]) == true) {
						run_special(SPEC_TOWN_LOOK,2,c_town.town.spec_id[i],where,&s1,&s2,&s3);
						if (s1 > 0)
                            can_open = false;
						got_special = true;
                        if (s3 > 0)
                            initiate_redraw();
						}
				put_item_screen(stat_window,0);
				}
		}
	if ((is_container(where)) && (item_there == true) && (can_open == true))
		get_item(where,6,true);

	else if (scenario.ter_types[terrain].special == TER_SPEC_CHANGE_WHEN_USED || scenario.ter_types[terrain].special == TER_SPEC_CALL_SPECIAL_WHEN_USED)
		{
			add_string_to_buf("  (Use this space to do something");
			add_string_to_buf("  with it.)");

        }

    else if (got_special == false) add_string_to_buf("  Search: You don't find anything.          ");

	return false;
}

void out_move_party(char x,char y)
{
	location l;

	l.x = x;
	l.y = y;
	l = l.toGlobal();
	party.p_loc = l;
	center = l;
	update_explored(l);
}

void teleport_party(short x,short y,short mode)
// mode - 0 full teleport flash 1 no teleport flash 2 only fade flash
{
	short i;
	location l;

	if (is_combat()) mode = 1;

	l = c_town.p_loc;
	update_explored(l);

	if (mode != 1) {
		start_missile_anim();
		for (i = 0; i < 9; i++)
			add_explosion(l,-1,1,1,0,0);
		do_explosion_anim(5,1);
		}
	if (mode != 1)
		can_draw_pcs = false;
	if (mode != 1) {
		do_explosion_anim(5,2);
		end_missile_anim();
		}
	center.x = x; center.y = y;
	if (is_combat()) {
		pc_pos[current_pc].x = x;pc_pos[current_pc].y = y;
		}
	l.x = x; l.y = y;
	c_town.p_loc.x = x;
	c_town.p_loc.y = y;
	update_explored(l);
	draw_terrain(0);

	if (mode == 0) {
		start_missile_anim();
		for (i = 0; i < 14; i++)
			add_explosion(center,-1,1,1,0,0);
		do_explosion_anim(5,1);
		}
	can_draw_pcs = true;
	if (mode == 0) {
		do_explosion_anim(5,2);
		end_missile_anim();
		}
	draw_map(modeless_dialogs[5],5);
}

void change_level(short town_num,short x,short y)
{
	location l;

	if ((town_num < 0) || (town_num >= scenario.num_towns)) {
		give_error("The scenario special encounter tried to put you into a town that doesn't exist.","",0);
		return;
		}

	l.x = x; l.y = y;

	force_town_enter(town_num,l);
	end_town_mode(1,l);
	start_town_mode(town_num,9);
}


// Damaging and killing monsters needs to be here because several have specials attached to them.
Boolean damage_monst(short which_m, short who_hit, short how_much, short how_much_spec, short dam_type)
//short which_m, who_hit, how_much, how_much_spec;  // 6 for who_hit means dist. xp evenly  7 for no xp
//short dam_type;  // 0 - weapon   1 - fire   2 - poison   3 - general magic   4 - unblockable  5 - cold
 				 // 6 - demon 7 - undead
 				 // 9 - marked damage, from during anim mode
 				 //+10 = no_print
 				 // 100s digit - damage sound for boom space
{
	creature_data_type *victim;
	short r1,which_spot,sound_type;
	location where_put;

	Boolean do_print = true;
	char resist;

	if (c_town.monst.dudes[which_m].active == 0) return false;

	sound_type = dam_type / 100;
	dam_type = dam_type % 100;

	if (dam_type >= DAMAGE_WEAPON_MARKED) {
		do_print = false;
		dam_type -= DAMAGE_WEAPON_MARKED;
		}

	if (sound_type == 0) {
		if ((dam_type == DAMAGE_FIRE) || (dam_type == DAMAGE_UNBLOCKABLE) )
			sound_type = 5;
		if 	(dam_type == DAMAGE_COLD)
			sound_type = 7;
		if 	(dam_type == DAMAGE_MAGIC)
			sound_type = 12;
		if 	(dam_type == DAMAGE_POISON)
			sound_type = 11;
		}


	victim = &c_town.monst.dudes[which_m];
	resist = victim->m_d.immunities;

	if (dam_type == DAMAGE_MAGIC) {
		if (resist & 1)
			how_much = how_much / 2;
		if (resist & 2)
			how_much = 0;
		}
	if (dam_type == DAMAGE_FIRE) {
		if (resist & 4)
			how_much = how_much / 2;
		if (resist & 8)
			how_much = 0;
		}
	if (dam_type == DAMAGE_COLD) {
		if (resist & 16)
			how_much = how_much / 2;
		if (resist & 32)
			how_much = 0;
		}
	if (dam_type == DAMAGE_POISON) {
		if (resist & 64)
			how_much = how_much / 2;
		if (resist & 128)
			how_much = 0;
		}

	// Absorb damage?
	if (((dam_type == DAMAGE_FIRE) || (dam_type == DAMAGE_MAGIC) || (dam_type == DAMAGE_COLD))
	 && (victim->m_d.spec_skill == MONSTER_ABSORB_SPELLS)) {
        if((victim->m_d.health + how_much) < 32767)
    		victim->m_d.health += how_much;
		ASB("  Magic absorbed.");
		return false;
		}

	// Saving throw
	if (((dam_type == DAMAGE_FIRE) || (dam_type == DAMAGE_COLD)) && (get_ran(1,0,20) <= victim->m_d.level))
		how_much = how_much / 2;
	if ((dam_type == DAMAGE_MAGIC) && (get_ran(1,0,24) <= victim->m_d.level))
		how_much = how_much / 2;

	// Rentar-Ihrno?
	if (victim->m_d.spec_skill == MONSTER_INVULNERABILITY)
		how_much = how_much / 10;

	r1 = get_ran(1,0,(victim->m_d.armor * 5) / 4);
	r1 += victim->m_d.level / 4;
	if (dam_type == DAMAGE_WEAPON)
		how_much -= r1;

	if (boom_anim_active == true) {
		if (how_much < 0)
			how_much = 0;
		monst_marked_damage[which_m] += how_much;
		if (victim->m_d.spec_skill == MONSTER_INVULNERABILITY)
		add_explosion(victim->m_loc,how_much/10,0,(dam_type > 2) ? 2 : 0,14 * (victim->m_d.x_width - 1),18 * (victim->m_d.y_width - 1));
		else
		add_explosion(victim->m_loc,how_much,0,(dam_type > 2) ? 2 : 0,14 * (victim->m_d.x_width - 1),18 * (victim->m_d.y_width - 1));
		if (how_much == 0){
			monst_spell_note(victim->number,7);
			return false;
            }
			else return true;
		}

	if (how_much <= 0) {
		if (is_combat())
			monst_spell_note(victim->number,7);
		if ((how_much <= 0) && ((dam_type == DAMAGE_WEAPON) || (dam_type == DAMAGE_UNDEAD) || (dam_type == DAMAGE_DEMON))) {
			draw_terrain(2);
			play_sound(2);
			}

		return false;
		}

	if (do_print == true)
		monst_damaged_mes(which_m,how_much,how_much_spec);
	victim->m_d.health = victim->m_d.health - how_much - how_much_spec;

	if (in_scen_debug == true)
		victim->m_d.health = -1;
	// splitting monsters
	if ((victim->m_d.spec_skill == MONSTER_SPLITS) && (victim->m_d.health > 0)){
		where_put = find_clear_spot(victim->m_loc,1);
		if (where_put.x > 0)
			if ((which_spot = c_town.placeMonster(victim->number,where_put)) < 90) {
				c_town.monst.dudes[which_spot].m_d.health = victim->m_d.health;
				c_town.monst.dudes[which_spot].monst_start = victim->monst_start;
				monst_spell_note(victim->number,27);
				}
		}
	if (who_hit < 7)
		party.total_dam_done += how_much + how_much_spec;

	// Monster damages. Make it hostile.
	victim->active = 2;


	if (dam_type != 9) { // note special damage only gamed in hand-to-hand, not during animation
		if (party_can_see_monst(which_m) == true) {
			pre_boom_space(victim->m_loc,100,boom_gr[dam_type],how_much,sound_type);
			if (how_much_spec > 0)
				boom_space(victim->m_loc,100,51,how_much_spec,5);
			}
			else {
				pre_boom_space(victim->m_loc,overall_mode, boom_gr[dam_type],how_much,sound_type);
				if (how_much_spec > 0)
					boom_space(victim->m_loc,overall_mode,51,how_much_spec,5);
				}
		}

	if (victim->m_d.health < 0) {
		monst_killed_mes(which_m);
		kill_monst(victim,who_hit);
		}
		else {
		if (how_much > 0)
			victim->m_d.morale = victim->m_d.morale - 1;
		if (how_much > 5)
			victim->m_d.morale = victim->m_d.morale - 1;
		if (how_much > 10)
			victim->m_d.morale = victim->m_d.morale - 1;
		if (how_much > 20)
			victim->m_d.morale = victim->m_d.morale - 2;
		}

	if ((victim->attitude % 2 != 1) && (who_hit < 7) &&
	 (processing_fields == false) && ((monsters_going == false) || (party.stuff_done[SDF_MONSTERS_ALERTNESS] == 0))) {
		add_string_to_buf("Damaged an innocent.           ");
		victim->attitude = 1;
		set_town_status(0);
		}
	/*if ((victim->attitude % 2 != 1) && (who_hit < 7) &&
	 ((processing_fields == true) && (party.stuff_done[SDF_MONSTERS_ALERTNESS] == 0))) {
		add_string_to_buf("Damaged an innocent.");
		victim->attitude = 1;
		set_town_status(0);
		}*/

	return true;
}

void kill_monst(creature_data_type *which_m,short who_killed)
{
	short xp,i,j,s1,s2,s3;
	location l;
	creature_data_type killed_monster;

	killed_monster = *which_m; //work on a local copy of the monster

	//erase the original monster now, so that if during a special chain the original monster's slot is filled with a new monster, it's not the new monster that is erased.
	which_m->monst_start.spec1 = 0; // make sure, if this is a spec. activated monster, it won't come back

	which_m->active = 0;

	switch (killed_monster.m_d.m_type) {
		case MONSTER_TYPE_HUMAN: case MONSTER_TYPE_IMPORTANT: case MONSTER_TYPE_MAGE: case MONSTER_TYPE_PRIEST: case MONSTER_TYPE_HUMANOID://humanoïd ?
			if (( killed_monster.number == 38) ||//goblin
				( killed_monster.number == 39))//goblin fighter
				i = 4;
            else if ( killed_monster.number == 45)//ogre
				i = 0;
            else
                i = get_ran(1,0,1);
			play_sound(29 + i); break;
		 case MONSTER_TYPE_GIANT: play_sound(29); break;
		 case MONSTER_TYPE_REPTILE: case MONSTER_TYPE_BEAST: case MONSTER_TYPE_DEMON: case MONSTER_TYPE_UNDEAD: case MONSTER_TYPE_STONE:
			i = get_ran(1,0,1); play_sound(31 + i); break;
		default: play_sound(33); break;
		}

	// Special killing effects
	if (sd_legit(killed_monster.monst_start.spec1,killed_monster.monst_start.spec2) == true)
		party.stuff_done[killed_monster.monst_start.spec1][killed_monster.monst_start.spec2] = 1;

	run_special(SPEC_KILL_MONST,2,killed_monster.monst_start.special_on_kill,killed_monster.m_loc,&s1,&s2,&s3);
	if (killed_monster.m_d.radiate_1 == MONSTER_DEATH_TRIGGERS)
		run_special(SPEC_KILL_MONST,0,killed_monster.m_d.radiate_2,killed_monster.m_loc,&s1,&s2,&s3);

	if ((in_scen_debug == false) && ((killed_monster.summoned >= 100) || (killed_monster.summoned == 0))) { // no xp for party-summoned monsters
		xp = killed_monster.m_d.level * 2;
		if (who_killed < NUM_OF_PCS)
			adven[who_killed].giveXP(xp);
			else if (who_killed == INVALID_PC)
				adven.giveXP(xp / 6 + 1);
		if (who_killed < 7) {
			i = max((xp / 6),1);
			adven.giveXP(i);
			}
		l = killed_monster.m_loc;
		place_glands(l,killed_monster.number);

		}
	if ((in_scen_debug == false) && (killed_monster.summoned == 0))
		place_treasure(killed_monster.m_loc, killed_monster.m_d.level / 2, killed_monster.m_d.treasure, 0);

	i = killed_monster.m_loc.x;
	j = killed_monster.m_loc.y;
	switch (killed_monster.m_d.m_type) {
		case MONSTER_TYPE_DEMON:	make_sfx(i,j,6); break;
		case MONSTER_TYPE_UNDEAD:
            if (killed_monster.number <= 59) //skeletons
                make_sfx(i,j,7); //put bones
            break;
		case MONSTER_TYPE_SLIME: case MONSTER_TYPE_BUG: make_sfx(i,j,4); break;
		case MONSTER_TYPE_STONE: make_sfx(i,j,8); break;
		default: make_sfx(i,j,1); break;
		}



	if (((is_town()) || (which_combat_type == 1)) && (killed_monster.summoned == 0)) {
		party.m_killed[c_town.town_num]++;
		}

	party.total_m_killed++;

}

// Pushes party and monsters around by moving walls and conveyor belts.
//This is very fragils, and only hands a few cases.
void push_things()
{
	Boolean redraw = false;
	short i,k;
	unsigned char ter;
	location l;

	if (is_out())
		return;
	if (belt_present == false)
		return;

	for (i = 0; i < T_M; i++)
		if (c_town.monst.dudes[i].active > 0) {
			l = c_town.monst.dudes[i].m_loc;
			ter = t_d.terrain[l.x][l.y];
			switch (scenario.ter_types[ter].special) {
				case TER_SPEC_CONVEYOR_NORTH: l.y--; break;
				case TER_SPEC_CONVEYOR_EAST: l.x++; break;
				case TER_SPEC_CONVEYOR_SOUTH: l.y++; break;
				case TER_SPEC_CONVEYOR_WEST: l.x--; break;
				}
			if (same_point(l,c_town.monst.dudes[i].m_loc) == false) {
				c_town.monst.dudes[i].m_loc = l;
				if ((point_onscreen(center,c_town.monst.dudes[i].m_loc) == true) ||
					(point_onscreen(center,l) == true))
						redraw = true;
				}
			}
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].variety > ITEM_TYPE_NO_ITEM) {
			l = t_i.items[i].item_loc;
			ter = t_d.terrain[l.x][l.y];
			switch (scenario.ter_types[ter].special) {
				case TER_SPEC_CONVEYOR_NORTH: l.y--; break;
				case TER_SPEC_CONVEYOR_EAST: l.x++; break;
				case TER_SPEC_CONVEYOR_SOUTH: l.y++; break;
				case TER_SPEC_CONVEYOR_WEST: l.x--; break;
				}
			if (same_point(l,t_i.items[i].item_loc) == false) {
				t_i.items[i].item_loc = l;
				if ((point_onscreen(center,t_i.items[i].item_loc) == true) ||
					(point_onscreen(center,l) == true))
						redraw = true;
				}
			}

	if (is_town()) {
		ter = t_d.terrain[c_town.p_loc.x][c_town.p_loc.y];
		l = c_town.p_loc;
		switch (scenario.ter_types[ter].special) {
			case TER_SPEC_CONVEYOR_NORTH: l.y--; break;
			case TER_SPEC_CONVEYOR_EAST: l.x++; break;
			case TER_SPEC_CONVEYOR_SOUTH: l.y++; break;
			case TER_SPEC_CONVEYOR_WEST: l.x--; break;
			}
		if (same_point(l,c_town.p_loc) == false) {
			ASB("You get pushed.");
			if (scenario.ter_types[ter].special >= TER_SPEC_CONVEYOR_NORTH)
				draw_terrain(0);
			center = l;
			c_town.p_loc = l;
			update_explored(l);
			ter = t_d.terrain[c_town.p_loc.x][c_town.p_loc.y];
			draw_map(modeless_dialogs[5],5);
			if (is_barrel(c_town.p_loc.x,c_town.p_loc.y)) {
				take_barrel(c_town.p_loc.x,c_town.p_loc.y);
				ASB("You smash the barrel.");
				}
			if (is_crate(c_town.p_loc.x,c_town.p_loc.y)) {
				take_crate(c_town.p_loc.x,c_town.p_loc.y);
				ASB("You smash the crate.");
				}
			for (k = 0; k < NUM_TOWN_ITEMS; k++)
				if ((t_i.items[k].variety > ITEM_TYPE_NO_ITEM) && (t_i.items[k].isContained())
				&& (same_point(t_i.items[k].item_loc,c_town.p_loc) == true))
					t_i.items[k].item_properties = t_i.items[k].item_properties & 247;
			redraw = true;
			}
		}
	if (is_combat()) {
		for (i = 0; i < 6; i++)
			if (adven[i].isAlive()) {
				ter = t_d.terrain[pc_pos[i].x][pc_pos[i].y];
				l = pc_pos[i];
				switch (scenario.ter_types[ter].special) {
					case TER_SPEC_CONVEYOR_NORTH: l.y--; break;
					case TER_SPEC_CONVEYOR_EAST: l.x++; break;
					case TER_SPEC_CONVEYOR_SOUTH: l.y++; break;
					case TER_SPEC_CONVEYOR_WEST: l.x--; break;
					}
				if (same_point(l, pc_pos[i]) == false) {
					ASB("Someone gets pushed.");
					ter = t_d.terrain[l.x][l.y];
					if (scenario.ter_types[ter].special >= TER_SPEC_CONVEYOR_NORTH)
						draw_terrain(0);
					pc_pos[i] = l;
					update_explored(l);
					draw_map(modeless_dialogs[5],5);
					if (is_barrel(pc_pos[i].x,pc_pos[i].y)) {
						take_barrel(pc_pos[i].x,pc_pos[i].y);
						ASB("You smash the barrel.");
						}
					if (is_crate(pc_pos[i].x,pc_pos[i].y)) {
						take_crate(pc_pos[i].x,pc_pos[i].y);
						ASB("You smash the crate.");
						}
					for (k = 0; k < NUM_TOWN_ITEMS; k++)
						if ((t_i.items[k].variety > ITEM_TYPE_NO_ITEM) && (t_i.items[k].isContained())
						&& (same_point(t_i.items[k].item_loc,pc_pos[i]) == true))
							t_i.items[k].item_properties = t_i.items[k].item_properties & 247;
					redraw = true;
					}
				}
		}
	if (redraw == true) {
		print_buf();
		draw_terrain(0);
		}
}

BOOL special_increase_age(char mode)
//mode : 0 triggers, 1 delay
{
	short i,s1,s2,s3;
	Boolean redraw = false,stat_area = false;
	BOOL to_return = false;

	for (i = 0; i < 8; i++)
		if ((c_town.town.timer_spec_times[i] > 0) && (party.age % c_town.town.timer_spec_times[i] == 0)
			&& ((is_town() == true) || ((is_combat() == true) && (which_combat_type == 1)))) {
            if(mode == 0){
    			run_special(SPEC_TOWN_TIMER,2,c_town.town.timer_specs[i],location(),&s1,&s2,&s3);
    			stat_area = true;
    			if (s3 > 0)
    				redraw = true;
    			to_return = true;
                }
            else if (queue_position < 19){ //leave a special pending slot for entering/exiting town
                special_queue[queue_position].queued_special = c_town.town.timer_specs[i];
                special_queue[queue_position].trigger_time = party.age;
                special_queue[queue_position].type = 2;
                special_queue[queue_position].mode = SPEC_TOWN_TIMER;
                special_queue[queue_position].where = location();
                queue_position++;
                }
			}
	for (i = 0; i < 20; i++)
		if ((scenario.scenario_timer_times[i] > 0) && (party.age % scenario.scenario_timer_times[i] == 0)) {
            if(mode == 0){
    			run_special(SPEC_SCEN_TIMER,0,scenario.scenario_timer_specs[i],location(),&s1,&s2,&s3);
    			stat_area = true;
    	   		if (s3 > 0)
    				redraw = true;
    			to_return = true;
                }
            else if (queue_position < 19){ //leave a special pending slot for entering/exiting town
                special_queue[queue_position].queued_special = scenario.scenario_timer_specs[i];
                special_queue[queue_position].trigger_time = party.age;
                special_queue[queue_position].type = 0;
                special_queue[queue_position].mode = SPEC_SCEN_TIMER;
                special_queue[queue_position].where = location();
                queue_position++;
                }
			}
	for (i = 0; i < 30; i++)
		if (party.party_event_timers[i] > 0) {
			if (party.party_event_timers[i] == 1) {
                if(mode == 0){
    				if (party.global_or_town[i] == 0)
	       				run_special(SPEC_PARTY_TIMER,0,party.node_to_call[i],location(),&s1,&s2,&s3);
		      			else run_special(SPEC_PARTY_TIMER,2,party.node_to_call[i],location(),&s1,&s2,&s3);
    				party.party_event_timers[i] = 0;
	       			stat_area = true;
    				if (s3 > 0)
	       				redraw = true;
    	       	  	to_return = true;
			     	}
			    else if (queue_position < 19){ //leave a special pending slot for entering/exiting town
                    if (party.global_or_town[i] == 0){
                        special_queue[queue_position].type = 0;
                        special_queue[queue_position].mode = SPEC_PARTY_TIMER;
                        }
                    else{
                        special_queue[queue_position].type = 2;
                        special_queue[queue_position].mode = SPEC_PARTY_TIMER;
                        }
                    special_queue[queue_position].queued_special = party.node_to_call[i];
                    special_queue[queue_position].trigger_time = party.age;
                    special_queue[queue_position].where = location();
                    queue_position++;
                    }
                }
				else party.party_event_timers[i]--;
			}
	if (stat_area == true) {
		put_pc_screen();
		put_item_screen(stat_window,0);
		}
	if (redraw == true)
		draw_terrain(0);

return to_return;
}

// This is the big painful one, the main special engine
// which_mode - says when it was called
// 0 - out moving (a - 1 if blocked)
// 1 - town moving (a - 1 if blocked)
// 2 - combat moving (a - 1 if blocked)
// 3 - out looking (a - 1 if don't get items inside)  NOT USED!!!
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
// 16 - ritual of sanct
// 17 - using space
// which_type - 0 - scen 1 - out 2 - town
// start spec - the number of the first spec to call
// a,b - 2 values that can be returned
// redraw - 1 if now need redraw
void run_special(short which_mode,short which_type,short start_spec,location spec_loc,short *a,short *b,short *redraw)
{
	short cur_spec,cur_spec_type,next_spec,next_spec_type;
	special_node_type cur_node;
	if (special_in_progress == true) {
		give_error("The scenario called a special node while processing another special encounter. The second special will be ignored.","",0);
		return;
		}

	special_in_progress = true;
	next_spec = start_spec;
	next_spec_type = which_type;
	current_pc_picked_in_spec_enc = -1;
	store_special_loc = spec_loc;
	if (end_scenario == true) {
		special_in_progress = false;
		return;
		}

	while (next_spec >= 0) {

		cur_spec = next_spec;
		cur_spec_type = next_spec_type;
		next_spec = -1;
		cur_node = get_node(cur_spec,cur_spec_type);

		if (cur_node.type == SPEC_ERROR) { /// got an error
			special_in_progress = false;
			return;
			}
		if ((cur_node.type >= SPEC_NULL) && (cur_node.type <= SPEC_DISPLAY_PICTURE)) {
			general_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= SPEC_ONCE_GIVE_ITEM) && (cur_node.type <= SPEC_ONCE_TRAP)) {
			oneshot_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= SPEC_SELECT_PC) && (cur_node.type <= SPEC_AFFECT_FLIGHT)) {
			affect_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= SPEC_IF_SDF) && (cur_node.type <= SPEC_IF_ENOUGH_SPECIES)) {
			ifthen_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= SPEC_SET_TOWN_STATUS) && (cur_node.type <= SPEC_TOWN_CHANGE_ATTITUDE)) {
			townmode_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= SPEC_RECT_PLACE_FIRE) && (cur_node.type <= SPEC_RECT_UNLOCK)) {
			rect_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= SPEC_OUT_MAKE_WANDER) && (cur_node.type <= SPEC_OUT_STORE)) {
			outdoor_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}

    if(check_for_interrupt()){
        give_error("The special encounter was interrupted. The scenario may be in an unexpected state; it is recommended that you reload from a saved game.","",0);
// 	    give_error("A special encounter can be at most 50 nodes long. The 50th node was just processed. The encounter will now end.","",0);
		next_spec = -1;
		}
	}
	if (is_out())
		erase_out_specials();
		else erase_specials();
	special_in_progress = false;

}

special_node_type get_node(short cur_spec,short cur_spec_type)
{
	special_node_type dummy_node;

	dummy_node = scenario.scen_specials[0];
	dummy_node.type = SPEC_ERROR;
	if (cur_spec_type == 0) {
		if (cur_spec != minmax(0,255,(int)cur_spec)) {
			give_error("The scenario called a scenario special node out of range.","",0);
			return dummy_node;
			}
		return scenario.scen_specials[cur_spec];
		}
	if (cur_spec_type == 1) {
		if (cur_spec != minmax(0,59,(int)cur_spec)) {
			give_error("The scenario called an outdoor special node out of range.","",0);
			return dummy_node;
			}
		return outdoors[party.i_w_c.x][party.i_w_c.y].specials[cur_spec];
		}
	if (cur_spec_type == 2) {
		if (cur_spec != minmax(0,99,(int)cur_spec)) {
			give_error("The scenario called a town special node out of range.","",0);
			return dummy_node;
			}
		return c_town.town.specials[cur_spec];
		}
	return dummy_node;
}

void general_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw)
{
	Boolean check_mess = false;
	char str1[256] = "",str2[256] = "";
	short store_val = 0,i;
	special_node_type spec;
	short mess_adj[3] = {160,10,20};

	spec = cur_node;
	*next_spec = cur_node.jumpto;

	switch (cur_node.type) {
		case SPEC_NULL: break; // null spec
		case SPEC_SET_SDF:
			check_mess = true;
			setsd(cur_node.sd1,cur_node.sd2,cur_node.ex1a);
			break;
		case SPEC_INC_SDF:
			check_mess = true;
			setsd(cur_node.sd1,cur_node.sd2,
				PSD[cur_node.sd1][cur_node.sd2] + ((cur_node.ex1b == 0) ? 1 : -1) * cur_node.ex1a);
			break;
		case SPEC_DISPLAY_MSG:
			check_mess = true;
			break;
		case SPEC_DISPLAY_SM_MSG:
			get_strs(str1, str2, cur_spec_type,cur_node.m1 + mess_adj[cur_spec_type],
				cur_node.m2 + mess_adj[cur_spec_type]);
			if (cur_node.m1 >= 0)
				ASB(str1);
			if (cur_node.m2 >= 0)
				ASB(str2);
			break;
		case SPEC_FLIP_SDF:
			setsd(cur_node.sd1,cur_node.sd2,
				((PSD[cur_node.sd1][cur_node.sd2] == 0) ? 1 : 0) );
			check_mess = true;
			break;
		case SPEC_OUT_BLOCK:
			if (is_out()) *next_spec = -1;
			if ((is_out()) && (spec.ex1a != 0) && (which_mode == SPEC_OUT_MOVE)) {
				ASB("Can't go here while outdoors.");
				*a = 1;
				}
			break;
		case SPEC_TOWN_BLOCK:
			if (is_town()) *next_spec = -1;
			if ((is_town()) && (spec.ex1a != 0) && (which_mode == SPEC_TOWN_MOVE)) {
				ASB("Can't go here while in town mode.");
				*a = 1;
				}
			break;
		case SPEC_FIGHT_BLOCK:
			if (is_combat()) *next_spec = -1;
			if ((is_combat()) && (spec.ex1a != 0) && (which_mode == SPEC_COMBAT_MOVE)) {
				ASB("Can't go here during combat.");
				*a = 1;
				}
			break;
		case SPEC_LOOK_BLOCK:
			if ((which_mode == SPEC_OUT_LOOK) || (which_mode == SPEC_TOWN_LOOK)) *next_spec = -1;
			break;
		case SPEC_CANT_ENTER:
			check_mess = true;
			if (which_mode < SPEC_OUT_LOOK) {
				if (spec.ex1a != 0)
					*a = 1;
					else *a = 0;
				}
			break;
		case SPEC_CHANGE_TIME:
			check_mess = true;
			party.age += spec.ex1a;//won't trigger special events !!!
			break;
		case SPEC_SCEN_TIMER_START:
			check_mess = true;
			for (i = 0; i < 30; i++)
	 			if (party.party_event_timers[i] == 0) {
	 				party.party_event_timers[i] = spec.ex1a;
	 				party.node_to_call[i] = spec.ex1b;
	 				party.global_or_town[i] = 0;
	 				i = 30;
	 				}
			break;
		case SPEC_PLAY_SOUND:
            if(spec.ex1a < 0)
                play_sound(spec.ex1a);
            else
			    play_sound(1000 + spec.ex1a);
			break;
		case SPEC_CHANGE_HORSE_OWNER:
			check_mess = true;
			if (spec.ex1a != minmax(0,29,(int)spec.ex1a))
				give_error("Horse out of range.","",0);
            else
                party.horses[spec.ex1a].property = (spec.ex2a == 0) ? 1 : 0;
			break;
		case SPEC_CHANGE_BOAT_OWNER:
			check_mess = true;
			if (spec.ex1a != minmax(0,29,(int)spec.ex1a))
				give_error("Boat out of range.","",0);
            else
                party.boats[spec.ex1a].property = (spec.ex2a == 0) ? 1 : 0;
			break;
		case SPEC_SET_TOWN_VISIBILITY:
			check_mess = true;
			if (spec.ex1a != minmax(0,scenario.num_towns - 1,(int)spec.ex1a))
				give_error("Town out of range.","",0);
            else{
			    if(PSD[SDF_LEGACY_SCENARIO] == 0) //"new" scenario, fixed show/hide town node
                    party.can_find_town[spec.ex1a] = (spec.ex1b == 0) ? 0 : 1;
                else //legacy scenario, old buggy show/hide town node (for compatibility purposes)
                    party.can_find_town[spec.ex1a] = (spec.ex2a == 0) ? 0 : 1;
				}
			*redraw = true;
			break;
		case SPEC_MAJOR_EVENT_OCCURRED:
			check_mess = true;
			if (spec.ex1a != minmax(1,10,(int)spec.ex1a))
				give_error("Event code out of range.","",0);
				else if (party.key_times[spec.ex1a] == 30000)
					party.key_times[spec.ex1a] = calc_day();
			break;
		case SPEC_FORCED_GIVE:
			check_mess = true;
			if ((forced_give(spec.ex1a,0) == false) && ( spec.ex1b >= 0))
				*next_spec = spec.ex1b;
			break;
		case SPEC_BUY_ITEMS_OF_TYPE:
			for (i = 0; i < 144; i++)
				if (adven.checkClass(spec.ex1a,0) == true)
					store_val++;
			if (store_val == 0) {
				if ( spec.ex1b >= 0)
					*next_spec = spec.ex1b;
				}
				else {
					check_mess = true;
					party.giveGold(store_val * spec.ex2a,true);
					}
			break;
		case SPEC_CALL_GLOBAL:
			*next_spec_type = 0;
			break;
		case SPEC_SET_SDF_ROW:
			if (spec.sd1 != minmax(0,299,(int)spec.sd1))
				give_error("Stuff Done flag out of range.","",0);
				else for (i = 0; i < 10; i++) PSD[spec.sd1][i] = spec.ex1a;
			break;
		case SPEC_COPY_SDF:
			if ((sd_legit(spec.sd1,spec.sd2) == false) || (sd_legit(spec.ex1a,spec.ex1b) == false))
				give_error("Stuff Done flag out of range.","",0);
				else PSD[spec.sd1][spec.sd2] = PSD[spec.ex1a][spec.ex1b];
			break;
		case SPEC_SANCTIFY:
			if (which_mode != SPEC_TARGET)
				*next_spec = spec.ex1b;
			break;
		case SPEC_REST:
					check_mess = true;
					if(party.stuff_done[SDF_COMPATIBILITY_CHECK_TIMERS_WHILE_RESTING] == 1){
                        for(i = 0; i < spec.ex1a ; i++){
                        party.age++;
                        special_increase_age(1);//if timed specials happens, put them on the timer queue
                        }
                    }
                    else
       					party.age += spec.ex1a;
					adven.heal(spec.ex1b);
					adven.restoreSP(spec.ex1b);
			break;
		case SPEC_WANDERING_WILL_FIGHT:
			if (which_mode != SPEC_OUTDOOR_ENC)
				break;
			*a = (spec.ex1a == 0) ? 1 : 0;
			break;
		case SPEC_END_SCENARIO:
            for(i=0;i<6;i++)//temporary fix, maybe we'll do a special "you lose" dialog.
                if(adven[i].isAlive() == false)
                    store_val++;

            if(store_val != 6)
			 end_scenario = true;
			break;
	   case SPEC_DISPLAY_PICTURE:
            check_mess = false;
            if(spec.ex1a >= 160 && spec.ex1a < 260)
                strcpy(str1, scen_strs2[spec.ex1a - 160]);
            else{
                give_error("Wrong scenario text number has been called (must be between 160 and 259).","",0);
                break;
                }
            if ((displayed_picture = ReadScenFile(str1)) == NULL){
                give_error("Error opening the image file. Make sure the extension (.bmp) is specified in the scenario message text (the first search directory is the current scenario folder).","",0);
                break;
                }

         cd_create_custom_pic_dialog(mainPtr,displayed_picture);

         while (dialog_not_toast)
             ModalDialog();

         cd_kill_dialog(905,0);
         DeleteObject(displayed_picture);
         displayed_picture = NULL;
         break;
		 }
	if (check_mess == true) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
}


void oneshot_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = true,set_sd = true;
	char strs[6][256] = {"","","","","",""};
	short i,j,buttons[3] = {-1,-1,-1};
	special_node_type spec;
	item_record_type store_i;
	location l;

	spec = cur_node;
	*next_spec = cur_node.jumpto;
	if ((sd_legit(spec.sd1,spec.sd2) == true) && (PSD[spec.sd1][spec.sd2] == 250)) {
		*next_spec = -1;
		return;
		}
	switch (cur_node.type) {
		case SPEC_ONCE_GIVE_ITEM:
			if (forced_give(spec.ex1a,0) == false) {
				set_sd = false;
				if ( spec.ex2b >= 0)
					*next_spec = spec.ex2b;
				}
				else {
					party.giveGold(spec.ex1b,true);
					party.giveFood(spec.ex2a,true);
					}
			break;
		case SPEC_ONCE_GIVE_SPEC_ITEM:
			if (spec.ex1a != minmax(0,49,(int)spec.ex1a)) {
				give_error("Special item is out of range.","",0);
				set_sd = false;
				}
				else {
					party.spec_items[spec.ex1a] = (spec.ex1b == 0) ? 1 : 0;
					}
			if (stat_window == 6)
				set_stat_window(6);
			*redraw = 1;
			break;
		case SPEC_ONCE_NULL:
			set_sd = false;
			check_mess = false;
			break;
		case SPEC_ONCE_SET_SDF:
			check_mess = false;
			break;
		case SPEC_ONCE_DIALOG: case SPEC_ONCE_DIALOG_TERRAIN: case SPEC_ONCE_DIALOG_MONSTER:
			check_mess = false;
			if (spec.m1 < 0)
				break;
			for (i = 0; i < 3; i++)
				get_strs(strs[i * 2], strs[i * 2 + 1],cur_spec_type,
					spec.m1 + i * 2 + spec_str_offset[cur_spec_type],spec.m1 + i * 2 + 1 + spec_str_offset[cur_spec_type]);
			if (spec.m2 > 0)
				{buttons[0] = 1; buttons[1] = spec.ex1a; buttons[2] = spec.ex2a;
				if ((spec.ex1a >= 0) || (spec.ex2a >= 0)) buttons[0] = 20; }
			if (spec.m2 <= 0) {buttons[0] = spec.ex1a;buttons[1] = spec.ex2a;}
			if ((buttons[0] < 0) && (buttons[1] < 0)) {
				give_error("Dialog box ended up with no buttons.","",0);
				break;
				}
			switch (cur_node.type) {
				case SPEC_ONCE_DIALOG: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2400,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic ,buttons) ; break;
				case SPEC_ONCE_DIALOG_TERRAIN: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2000,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic,buttons) ; break;
				case SPEC_ONCE_DIALOG_MONSTER: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2000,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic ,buttons) ; break;
				}
			if (spec.m2 > 0) {
				if (i == 1) {
					if ((spec.ex1a >= 0) || (spec.ex2a >= 0)) {
						set_sd = false;
						}
					}
				if (i == 2) *next_spec = spec.ex1b;
				if (i == 3) *next_spec = spec.ex2b;
				}
				else {
					if (i == 1) *next_spec = spec.ex1b;
					if (i == 2) *next_spec = spec.ex2b;
					}
			break;
		case SPEC_ONCE_GIVE_ITEM_DIALOG: case SPEC_ONCE_GIVE_ITEM_TERRAIN: case SPEC_ONCE_GIVE_ITEM_MONSTER:
			check_mess = false;
			if (spec.m1 < 0)
				break;
			for (i = 0; i < 3; i++)
				get_strs((char *) strs[i * 2],(char *) strs[i * 2 + 1],cur_spec_type,
					spec.m1 + i * 2 + spec_str_offset[cur_spec_type],spec.m1 + i * 2 + 1  + spec_str_offset[cur_spec_type]);
			buttons[0] = 20; buttons[1] = 19;
			switch (cur_node.type) {
				case SPEC_ONCE_GIVE_ITEM_DIALOG: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2400,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic,buttons) ; break;
				case SPEC_ONCE_GIVE_ITEM_TERRAIN: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2000,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic,buttons) ; break;
				case SPEC_ONCE_GIVE_ITEM_MONSTER: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2000,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic,buttons) ; break;
				}
			if (i == 1) {set_sd = false; *next_spec = -1;}
				else {
					store_i = get_stored_item(spec.ex1a);
					if ((spec.ex1a >= 0) && (give_to_party(store_i,true) == false)) {
						set_sd = false; *next_spec = -1;
						}
						else {
							party.giveGold(spec.ex1b,true);
							party.giveFood(spec.ex2a,true);
							if ((spec.m2 >= 0) && (spec.m2 < 50)) {
								if (party.spec_items[spec.m2] == 0)
									ASB("You get a special item.");
								party.spec_items[spec.m2] = 1;
								*redraw = true;
								if (stat_window == 6)
									set_stat_window(6);
								}
							if (spec.ex2b >= 0) *next_spec = spec.ex2b;
							}
					}
			break;
		case SPEC_ONCE_OUT_ENCOUNTER:
			if (spec.ex1a != minmax(0,3,(int)spec.ex1a)) {
				give_error("Special outdoor enc. is out of range. Must be 0-3.","",0);
				set_sd = false;
				}
				else {
					l = party.p_loc.toLocal();
					place_outd_wand_monst(l,
					outdoors[party.i_w_c.x][party.i_w_c.y].special_enc[spec.ex1a],true);
					}
			break;
		case SPEC_ONCE_TOWN_ENCOUNTER:
             c_town.activateMonsters(spec.ex1a);
			break;
		case SPEC_ONCE_TRAP:
			check_mess = false;
			if ((spec.m1 >= 0) || (spec.m2 >= 0)) {
				get_strs((char *) strs[0],(char *) strs[1],
					cur_spec_type,
					spec.m1 + ((spec.m1 >= 0) ? spec_str_offset[cur_spec_type] : 0),
					spec.m2 + ((spec.m2 >= 0) ? spec_str_offset[cur_spec_type] : 0));
				buttons[0] = 3; buttons[1] = 2;
				i = custom_choice_dialog((char *) strs,727,buttons);
				}
				else i = FCD(872,0);
			if (i == 1) {set_sd = false; *next_spec = -1; *a = 1;}
				else {
					if (is_combat() == true)
						j = adven[current_pc].runTrap(spec.ex1a,spec.ex1b,spec.ex2a);
					else
					{
						short tmp = select_pc(1,0);

						if (tmp == INVALID_PC) j = false;
						else j = adven[tmp].runTrap(spec.ex1a,spec.ex1b,spec.ex2a);
					}

					if (j == 0 || (party_toast() == true))//maybe party has been killed by the traps...
					{
						*a = 1;
						set_sd = false;
					}
					}
			break;
		}
	if (check_mess == true) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
	if ((set_sd == true) && (sd_legit(spec.sd1,spec.sd2) == true))
		PSD[spec.sd1][spec.sd2] = 250;

}

void affect_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = true;
	short i,pc,r1;
	special_node_type spec;

	spec = cur_node;
	*next_spec = cur_node.jumpto;
	if((PSD[SDF_IS_PARTY_SPLIT] > 0) && (cur_node.type != SPEC_AFFECT_DEADNESS))
        pc = PSD[SDF_PARTY_SPLIT_PC];
    else pc = current_pc_picked_in_spec_enc;


	if ((check_mess == true) && (cur_node.type != SPEC_SELECT_PC)) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}

	switch (cur_node.type) {
		case SPEC_SELECT_PC://select a pc
			if (spec.ex1a == 2)
				current_pc_picked_in_spec_enc = -1;
				else if (spec.ex1a == 1) {
					i = select_pc(0,0);
					if (i != 6)
						current_pc_picked_in_spec_enc = i;
					}
				else if (spec.ex1a == 0) {
					i = select_pc(1,0);
					if (i != 6)
						current_pc_picked_in_spec_enc = i;
					}
			if (i == 6)// && (spec.ex1b >= 0))
				*next_spec = spec.ex1b;
			break;
		case SPEC_DAMAGE://do damage
			r1 = get_ran(spec.ex1a,1,spec.ex1b) + spec.ex2a;
			if (pc < 0) {
                if(spec.pic == 1 && overall_mode == MODE_COMBAT)
                    adven[current_pc].damage(r1,spec.ex2b,0);
                else
				    adven.damage(r1,spec.ex2b);
				}
				else adven[pc].damage(r1,spec.ex2b,0);
			break;
		case SPEC_AFFECT_HP://affect health
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					adven[i].cur_health = minmax(0,	(int)adven[i].max_health,
						adven[i].cur_health + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case SPEC_AFFECT_SP://affect spell points
			for (i = 0; i < 6; i++){
				if ((pc < 0) || (pc == i))
				   adven[i].cur_sp = minmax(0,	(int)adven[i].max_sp,
                        adven[i].cur_sp + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
                }
			break;
		case SPEC_AFFECT_XP://affect experience
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0) adven[i].giveXP(spec.ex1a);
					else adven[i].drainXP(spec.ex1a);
					}
			break;
		case SPEC_AFFECT_SKILL_PTS://affect skill points
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					adven[i].skill_pts = minmax(0,	100,
						adven[i].skill_pts + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case SPEC_AFFECT_DEADNESS://kill/raise dead
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0) {
						if ((adven[i].main_status > MAIN_STATUS_ABSENT) && (adven[i].main_status < MAIN_STATUS_SPLIT))
							adven[i].main_status=MAIN_STATUS_ALIVE;
						}
						else
                        if(party.stuff_done[SDF_COMPATIBILITY_LEGACY_KILL_NODE] == 0){//legacy behavior
                         adven[i].kill(spec.ex1a + ((spec.ex1b == 2)? 22 : 12));
                            }
                        else{//kill only present pc
                        if ((adven[i].main_status > MAIN_STATUS_ABSENT) && (adven[i].main_status < MAIN_STATUS_SPLIT))
                            adven[i].kill(spec.ex1a + ((spec.ex1b == 2)? 22 : 12));
                        }

					}
			*redraw = 1;
			break;
		case SPEC_AFFECT_POISON://affect poison
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0)
						adven[i].cure(spec.ex1a);
					else
						adven[i].poison(spec.ex1a);
					}
			break;
		case SPEC_AFFECT_SPEED://affect slow/haste
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0) {
						adven[i].slow(-1 * spec.ex1a);
						}
						else adven[i].slow(spec.ex1a);
					}
			break;
		case SPEC_AFFECT_INVULN://affect invulnerability
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,STATUS_INVULNERABLE,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case SPEC_AFFECT_MAGIC_RES://affect magic resistance
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,STATUS_MAGIC_RESISTANCE,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case SPEC_AFFECT_WEBS://affect webs
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,STATUS_WEBS,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case SPEC_AFFECT_DISEASE://affect disease
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,STATUS_DISEASE,spec.ex1a * ((spec.ex1b != 0) ? 1: -1));
			break;
		case SPEC_AFFECT_SANCTUARY://affect sanctuary
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,STATUS_INVISIBLE,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case SPEC_AFFECT_CURSE_BLESS://affect curse/bless
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,STATUS_BLESS_CURSE,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case SPEC_AFFECT_DUMBFOUND://affect dumbfounding
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,STATUS_DUMB,spec.ex1a * ((spec.ex1b == 0) ? -1: 1));
			break;
		case SPEC_AFFECT_SLEEP://affect sleep
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0) {
						affect_pc(i,STATUS_ASLEEP,-1 * spec.ex1a);
						}
						else adven[i].sleep(spec.ex1a,11,10);
					}
			break;
		case SPEC_AFFECT_PARALYSIS://affect paralysis
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0) {
						affect_pc(i,STATUS_PARALYZED,-1 * spec.ex1a);
						}
						else adven[i].sleep(spec.ex1a,12,10);
					}
			break;
		case SPEC_AFFECT_STAT://affect statistic
			if (spec.ex2a != minmax(0,18,(int)spec.ex2a)) {
				give_error("Skill is out of range.","",0);
				break;
				}
			for (i = 0; i < 6; i++)
				if (((pc < 0) || (pc == i)) && (get_ran(1,1,100) <= spec.pic))
					adven[i].skills[spec.ex2a] = minmax(0, (int)skill_max[spec.ex2a],
						adven[i].skills[spec.ex2a] + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case SPEC_AFFECT_MAGE_SPELL://affect mage spell
            if(spec.ex1b == -1)
                spec.ex1b = 1;
            if ((spec.ex1b != 0) && (spec.ex1b != 1)){
                give_error("Error the Extra 1b field should be 0 (take spell) or 1 (give spell). (-1 is legacy compatibility)","",0);
                break;
                }
            if((spec.ex1a < 0) || ((spec.ex1a > 31) && ( spec.ex1a < 100)) || (spec.ex1a > 129)){
				give_error("Mage spell is out of range (0 - 31) or (100-129). See docs.","",0);
				break;
				}
            if(spec.ex1a >= 100)
                r1 = spec.ex1a - 130;
            else
                r1 = spec.ex1a;
			for (i = 0; i < 6; i++)
                if ((pc < 0) || (pc == i))
					adven[i].mage_spells[r1 + 30] = spec.ex1b;
			break;
		case SPEC_AFFECT_PRIEST_SPELL://affect priest spell
            if(spec.ex1b == -1)
                spec.ex1b = 1;
            if ((spec.ex1b != 0) && (spec.ex1b != 1)){
                give_error("Error the Extra 1b field should be 0 (take spell) or 1 (give spell). (-1 is legacy compatibility)","",0);
                break;
                }
            if((spec.ex1a < 0) || ((spec.ex1a > 31) && ( spec.ex1a < 100)) || (spec.ex1a > 129)){
				give_error("Priest spell is out of range (0 - 31) or (100-129). See docs.","",0);
				break;
				}
            if(spec.ex1a >= 100)
                r1 = spec.ex1a - 130;
            else
                r1 = spec.ex1a;
			for (i = 0; i < 6; i++)
                if ((pc < 0) || (pc == i))
					adven[i].priest_spells[r1 + 30] = spec.ex1b;
			break;
		case SPEC_AFFECT_GOLD://affect gold
			if (spec.ex1b == 0)
				party.giveGold(spec.ex1a,true);
				else if (party.gold < spec.ex1a)
					party.gold = 0;
					else party.takeGold(spec.ex1a,false);
			break;
		case SPEC_AFFECT_FOOD://affect food
			if (spec.ex1b == 0)
				party.giveFood(spec.ex1a,true);
				else if (party.food < spec.ex1a)
					party.food = 0;
					else party.takeFood(spec.ex1a,false);
			break;
		case SPEC_AFFECT_ALCHEMY://affect alchemy
			if (spec.ex1a != minmax(0,19,(int)spec.ex1a)) {
				give_error("Alchemy is out of range.","",0);
				break;
				}
			party.alchemy[spec.ex1a] = true;
			break;
		case SPEC_AFFECT_STEALTH://affect stealth
			r1 = (short) party.stuff_done[SDF_STEALTH];
			r1 = minmax(0,250,r1 + spec.ex1a);
			party.stuff_done[SDF_STEALTH] = r1;
			break;
		case SPEC_AFFECT_FIREWALK://affect firewalk
			r1 = (short) party.stuff_done[SDF_LAVAWALK];
			r1 = minmax(0,250,r1 + spec.ex1a);
			party.stuff_done[SDF_LAVAWALK] = r1;
			break;
		case SPEC_AFFECT_FLIGHT://affect flying
			if (party.in_boat >= 0)
				add_string_to_buf("  Can't fly when on a boat. ");
			else if (party.in_horse >= 0)////
				add_string_to_buf("  Can't fly when on a horse.  ");
			else {
				r1 = (short) party.stuff_done[SDF_FLYING];
				r1 = minmax(0,250,r1 + spec.ex1a);
				party.stuff_done[SDF_FLYING] = r1;
				}
			break;
		}
}

void ifthen_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = false;
	char str1[256] = "",str2[256] = "",str3[256] = "";
	short i,j,k;
	special_node_type spec;
	location l;

	spec = cur_node;
	*next_spec = cur_node.jumpto;

	switch (cur_node.type) {
		case SPEC_IF_SDF:
			if (sd_legit(spec.sd1,spec.sd2) == true) {
				if ((spec.ex1a >= 0) && (PSD[spec.sd1][spec.sd2] >= spec.ex1a))
					*next_spec = spec.ex1b;
					else if ((spec.ex2a >= 0) && (PSD[spec.sd1][spec.sd2] < spec.ex2a))
						*next_spec = spec.ex2b;
				}
			break;
		case SPEC_IF_TOWN_NUM:
			if (((is_town()) || (is_combat())) && (c_town.town_num == spec.ex1a))
				*next_spec = spec.ex1b;
			break;
		case SPEC_IF_RANDOM:
			if (get_ran(1,1,100) <= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case SPEC_IF_HAVE_SPECIAL_ITEM:
			if (spec.ex1a != minmax(0,49,(int)spec.ex1a)) {
				give_error("Special item is out of range.","",0);
				}
				else if (party.spec_items[spec.ex1a] > 0)
					*next_spec = spec.ex1b;
			break;
		case SPEC_IF_SDF_COMPARE:
			if ((sd_legit(spec.sd1,spec.sd2) == true) && (sd_legit(spec.ex1a,spec.ex1b) == true)) {
				if (PSD[spec.ex1a][spec.ex1b] < PSD[spec.sd1][spec.sd2])
					*next_spec = spec.ex2b;
				}
				else give_error("A Stuff Done flag is out of range.","",0);
			break;
		case SPEC_IF_TOWN_TER_TYPE:
			if (((is_town()) || (is_combat())) && (t_d.terrain[spec.ex1a][spec.ex1b] == spec.ex2a))
				*next_spec = spec.ex2b;
			break;
		case SPEC_IF_OUT_TER_TYPE:
			l.x = spec.ex1a; l.y = spec.ex1b;
			l = l.toGlobal();
			if ((is_out()) && (out[l.x][l.y] == spec.ex2a))
				*next_spec = spec.ex2b;
 			break;
		case SPEC_IF_HAS_GOLD:
			if (party.gold >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case SPEC_IF_HAS_FOOD:
			if (party.food >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case SPEC_IF_ITEM_CLASS_ON_SPACE:
			if (is_out())
				break;
			l.x = spec.ex1a; l.y = spec.ex1b;
			for (i = 0; i < NUM_TOWN_ITEMS; i++)
				if ((t_i.items[i].variety > ITEM_TYPE_NO_ITEM) && (t_i.items[i].special_class == spec.ex2a)
					&& (same_point(l,t_i.items[i].item_loc) == true))
						*next_spec = spec.ex2b;
			break;
		case SPEC_IF_HAVE_ITEM_CLASS:
			if (adven.checkClass(spec.ex1a,1) == true)
				*next_spec = spec.ex1b;
			break;
		case SPEC_IF_EQUIP_ITEM_CLASS:
			for (i = 0; i < 6; i++)
				if (adven[i].isAlive())
					for (j = 0; j < 24; j++)
						if ((adven[i].items[j].variety > ITEM_TYPE_NO_ITEM) && (adven[i].items[j].special_class == spec.ex1a)
							&& (adven[i].equip[j] == true))
							*next_spec = spec.ex1b;
			break;
		case SPEC_IF_HAS_GOLD_AND_TAKE:
			if (party.gold >= spec.ex1a) {
				party.takeGold(spec.ex1a,true);
				*next_spec = spec.ex1b;
				}
			break;
		case SPEC_IF_HAS_FOOD_AND_TAKE:
			if (party.food >= spec.ex1a) {
				party.takeFood(spec.ex1a,true);
				*next_spec = spec.ex1b;
				}
			break;
		case SPEC_IF_ITEM_CLASS_ON_SPACE_AND_TAKE:
			if (is_out())
				break;
			l.x = spec.ex1a; l.y = spec.ex1b;
			for (i = 0; i < NUM_TOWN_ITEMS; i++)
				if ((t_i.items[i].variety > ITEM_TYPE_NO_ITEM) && (t_i.items[i].special_class == spec.ex2a)
					&& (same_point(l,t_i.items[i].item_loc) == true)) {
						*next_spec = spec.ex2b;
						*redraw = 1;
						t_i.items[i].variety = ITEM_TYPE_NO_ITEM;
						}
			break;
		case SPEC_IF_HAVE_ITEM_CLASS_AND_TAKE:
			if (adven.checkClass(spec.ex1a,0) == true)
				*next_spec = spec.ex1b;
			break;
		case SPEC_IF_EQUIP_ITEM_CLASS_AND_TAKE:
			for (i = 0; i < 6; i++)
				if (adven[i].isAlive())
					for (j = 0; j < 24; j++)
						if ((adven[i].items[j].variety > ITEM_TYPE_NO_ITEM) && (adven[i].items[j].special_class == spec.ex1a)
							&& (adven[i].equip[j] == true)) {
							*next_spec = spec.ex1b;
							*redraw = 1;
							adven[i].takeItem(j);
							}
			break;
		case SPEC_IF_DAY_REACHED:
			if (calc_day() >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case SPEC_IF_BARRELS:
			for (j = 0; j < town_size[town_type]; j++)
				for (k = 0; k < town_size[town_type]; k++)
					if (is_barrel(j,k))
						*next_spec = spec.ex1b;
			break;
		case SPEC_IF_CRATES:
			for (j = 0; j < town_size[town_type]; j++)
				for (k = 0; k < town_size[town_type]; k++)
					if (is_crate(j,k))
						*next_spec = spec.ex1b;
			break;
		case SPEC_IF_EVENT_OCCURRED:
			if (day_reached(spec.ex1a,spec.ex1b) == true)
				*next_spec = spec.ex2b;
			break;
		case SPEC_IF_HAS_CAVE_LORE:
			for (i = 0; i < 6; i++)
				if ((adven[i].isAlive()) && (adven[i].traits[TRAIT_CAVE_LORE] > 0))
					*next_spec = spec.ex1b;
			break;
		case SPEC_IF_HAS_WOODSMAN:
			for (i = 0; i < 6; i++)
				if ((adven[i].isAlive()) && (adven[i].traits[TRAIT_WOODSMAN] > 0))
					*next_spec = spec.ex1b;
			break;
		case SPEC_IF_ENOUGH_STATISTIC: //has enough statistic ?
            char to_check;
            k=0; // number to compare to
            j=0; // number of alive pc

            if((PSD[SDF_LEGACY_SCENARIO] == 1) || (spec.ex2b == -1) || (spec.ex2a == -1)){//old (legacy) compatibility check
            	if (adven.getMageLore() >= spec.ex1a)
				    *next_spec = spec.ex1b;
			         break;
                }

            // 19 is current health, 20 is max health, 21 current spell points, 22 max spell points
            // 23 is experience, 24 is skill points, 25 is level
            if((spec.ex2a < -1) || (spec.ex2a > 25)){
                give_error("The check statistic node tried to check an out-of-range statistic (shoud be between 0 - strengh to 18 - luck and 25. -1 is default compatibility to check mage lore.).","",0);
                break;
                }

            if(current_pc_picked_in_spec_enc >= 0)// is a pc selected ?
               to_check = 10 + current_pc_picked_in_spec_enc;
            else
                to_check = spec.ex2b;

            switch(to_check){
                case 1: // highest
                    for(i = 0;i < 6; i++)
                        if(adven[i].main_status == MAIN_STATUS_ALIVE){
                            switch (spec.ex2a){
                            case 19: //current health
                                if(k < adven[i].cur_health)
                                    k = adven[i].cur_health;
                                break;
                            case 20: //max health
                                if(k < adven[i].max_health)
                                    k = adven[i].max_health;
                                break;
                            case 21: //current spell points
                                if(k < adven[i].cur_sp)
                                    k = adven[i].cur_sp;
                                break;
                            case 22: //max spell points
                                if(k < adven[i].max_sp)
                                    k = adven[i].max_sp;
                                break;
                            case 23 ://experience
                                if(k < adven[i].experience)
                                    k = adven[i].experience;
                                break;
                            case 24 ://skill points
                                if(k < adven[i].skill_pts)
                                    k = adven[i].skill_pts;
                                break;
                            case 25://level
                                if(k < adven[i].level)
                                    k = adven[i].level;
                                break;
                            default :
                                if(k < adven[i].skills[spec.ex2a])
                                    k = adven[i].skills[spec.ex2a];
                                break;
                            }
                        }
                break;
                case 2:// average
                    for(i = 0;i < 6;i++)
                        if(adven[i].main_status == MAIN_STATUS_ALIVE){
                            switch (spec.ex2a){
                                case 19: //current health
                                    k += adven[i].cur_health;
                                    break;
                                case 20: //max health
                                    k += adven[i].max_health;
                                    break;
                                case 21: //current spell points
                                    k += adven[i].cur_sp;
                                    break;
                                case 22: //max spell points
                                    k += adven[i].max_sp;
                                    break;
                                case 23 ://experience
                                    k += adven[i].experience;
                                break;
                                case 24 ://skill points
                                    k += adven[i].skill_pts;
                                    break;
                                case 25://level
                                    k += adven[i].level;
                                    break;
                                default :
                                    k += adven[i].skills[spec.ex2a];
                                    break;
                                }
                            j++;
                        }
                    k = k / j;
                break;
                case 3: // lowest
                    k = 32000;
                    for(i = 0;i < 6 ; i++)
                        if(adven[i].main_status == MAIN_STATUS_ALIVE)
                          switch (spec.ex2a){
                            case 19: //current health
                                if(k > adven[i].cur_health)
                                    k = adven[i].cur_health;
                                break;
                            case 20: //max health
                                if(k > adven[i].max_health)
                                    k = adven[i].max_health;
                                break;
                            case 21: //current spell points
                                if(k > adven[i].cur_sp)
                                    k = adven[i].cur_sp;
                                break;
                            case 22: //max spell points
                                if(k > adven[i].max_sp)
                                    k = adven[i].max_sp;
                                break;
                            case 23 ://experience
                                if(k > adven[i].experience)
                                    k = adven[i].experience;
                                break;
                            case 24 ://skill points
                                if(k > adven[i].skill_pts)
                                    k = adven[i].skill_pts;
                                break;
                            case 25://level
                                if(k > adven[i].level)
                                    k = adven[i].level;
                                break;
                            default :
                                if(k > adven[i].skills[spec.ex2a])
                                    k = adven[i].skills[spec.ex2a];
                                break;
                            }

                break;
                case 10: case 11: case 12: case 13: case 14: case 15: //individual pc
                    if(adven[to_check - 10].main_status == MAIN_STATUS_ALIVE)
                        switch (spec.ex2a){
                            case 19: //current health
                                k = adven[to_check - 10].cur_health;
                                break;
                            case 20: //max health
                                k = adven[to_check - 10].max_health;
                                break;
                            case 21: //current spell points
                                k = adven[to_check - 10].cur_sp;
                                break;
                            case 22: //max spell points
                                k = adven[to_check - 10].max_sp;
                                break;
                            case 23 ://experience
                                k = adven[to_check - 10].experience;
                                break;
                            case 24 ://skill points
                                k = adven[to_check - 10].skill_pts;
                                break;
                            case 25://level
                                k = adven[to_check - 10].level;
                                break;
                            default :
                                k = adven[to_check - 10].skills[spec.ex2a];
                                break;
                            }
                break;
                default : // cumulative
                    for(i = 0;i < 6;i++)
                        if(adven[i].main_status == MAIN_STATUS_ALIVE)
                            switch (spec.ex2a){
                                case 19: //current health
                                    k += adven[i].cur_health;
                                    break;
                                case 20: //max health
                                    k += adven[i].max_health;
                                    break;
                                case 21: //current spell points
                                    k += adven[i].cur_sp;
                                    break;
                                case 22: //max spell points
                                    k += adven[i].max_sp;
                                    break;
                                case 23 ://experience
                                    k += adven[i].experience;
                                break;
                                case 24 ://skill points
                                    k += adven[i].skill_pts;
                                    break;
                                case 25://level
                                    k += adven[i].level;
                                    break;
                                default :
                                    k += adven[i].skills[spec.ex2a];
                                    break;
                                }
                break;
                }
				if (k >= spec.ex1a)
					*next_spec = spec.ex1b;
			break;
		case SPEC_IF_TEXT_RESPONSE: // text response
			check_mess = false;
			get_text_response(873,str3,0);
			j = 1; k = 1;
			spec.pic = minmax(0,8,(int)spec.pic);
			get_strs((char *) str1,(char *) str2,0,spec.ex1a,spec.ex2a);
			for (i = 0; i < spec.pic;i++) {
				if ((spec.ex1a < 0) || (str3[i] != str1[i]))
					j = 0;
				if ((spec.ex2a < 0) || (str3[i] != str2[i]))
					k = 0;
				}
			if (j == 1)
				*next_spec = spec.ex1b;
				else if (k == 1)
					*next_spec = spec.ex2b;
			break;
		case SPEC_IF_SDF_EQ:
			if (sd_legit(spec.sd1,spec.sd2) == true) {
				if (PSD[spec.sd1][spec.sd2] == spec.ex1a)
					*next_spec = spec.ex1b;
				}
			break;
        case SPEC_IF_ENOUGH_SPECIES:
                k = 0; // number to compare to

                if((spec.ex1a < 0) || (spec.ex1a > 2)){
                    give_error("Species out of range. (should be 0 - Human, 1 - Nephilim, 2 - Slith)","",0);
                    break;
                    }
                for(i = 0; i < 6; i++)
                    if((adven[i].main_status == MAIN_STATUS_ALIVE) && (adven[i].race == spec.ex1a))
                        k++;
                if(((spec.ex2b == -1) && (k < spec.ex2a)) || ((spec.ex2b == 0) && (k == spec.ex2a)) || ((spec.ex2b == 1) && (k > spec.ex2a)))
                    *next_spec = spec.ex1b;
            break;
		}
	if (check_mess == true) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
}

void townmode_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = true;
	char strs[6][256] = {"","","","","",""};
	short i,buttons[3] = {-1,-1,-1},r1;
	special_node_type spec;
	location l;
	unsigned char ter;
	item_record_type store_i;

	spec = cur_node;
	*next_spec = cur_node.jumpto;

	l.x = spec.ex1a; l.y = spec.ex1b;

	if (is_out())
		return;
	switch (cur_node.type) {
		case SPEC_SET_TOWN_STATUS:
            if((PSD[SDF_LEGACY_SCENARIO] == 1) || (spec.ex1a == -1))// legacy compatibility check
                spec.ex1a = 0;
            if((spec.ex1a < 0) || (spec.ex1a > 2)){
                give_error("The status to change town to was out of reach (should be 0 - Hostile, 1 - Friendly, 2- Dead).","",0);
                break;
            }
			set_town_status(spec.ex1a);
			break;
		case SPEC_TOWN_CHANGE_TER:
			set_terrain(l,spec.ex2a);
            if(scenario.ter_types[spec.ex2a].special >= TER_SPEC_CONVEYOR_NORTH && scenario.ter_types[spec.ex2a].special <= TER_SPEC_CONVEYOR_WEST)
                belt_present = true;
            if(party.stuff_done[SDF_HIDDEN_MAP] == 0)
                draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			*redraw = true;
			break;
		case SPEC_TOWN_SWAP_TER:
			if (coord_to_ter(spec.ex1a,spec.ex1b) == spec.ex2a){
                set_terrain(l,spec.ex2b);
                if(scenario.ter_types[spec.ex2b].special >= TER_SPEC_CONVEYOR_NORTH && scenario.ter_types[spec.ex2b].special <= TER_SPEC_CONVEYOR_WEST)
                    belt_present = true;
            }
			else if (coord_to_ter(spec.ex1a,spec.ex1b) == spec.ex2b){
    			set_terrain(l,spec.ex2a);
                if(scenario.ter_types[spec.ex2a].special >= TER_SPEC_CONVEYOR_NORTH && scenario.ter_types[spec.ex2a].special <= TER_SPEC_CONVEYOR_WEST)
                    belt_present = true;
            }
			*redraw = 1;
            if(party.stuff_done[SDF_HIDDEN_MAP] == 0)
                draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		case SPEC_TOWN_TRANS_TER:
			ter = coord_to_ter(spec.ex1a,spec.ex1b);
			set_terrain(l,scenario.ter_types[ter].trans_to_what);
			if(scenario.ter_types[scenario.ter_types[ter].trans_to_what].special >= TER_SPEC_CONVEYOR_NORTH && scenario.ter_types[scenario.ter_types[ter].trans_to_what].special <= TER_SPEC_CONVEYOR_WEST)
                belt_present = true;
			*redraw = 1;
            if(party.stuff_done[SDF_HIDDEN_MAP] == 0)
                draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		case SPEC_TOWN_MOVE_PARTY:
			if (is_combat()) {
				ASB("Not while in combat.");
				if (which_mode < SPEC_OUT_LOOK)//if moving
					*a = 1;
				*next_spec = -1;
				check_mess = false;
				}
				else { // 1 no
				if (which_mode < SPEC_OUT_LOOK)//if moving
					*a = 1;
                if ((which_mode == SPEC_TALK) || (spec.ex2a == 0))
                    teleport_party(spec.ex1a,spec.ex1b,1);
                else
                    teleport_party(spec.ex1a,spec.ex1b,0);
					}
			*redraw = 1;
			break;
		case SPEC_TOWN_HIT_SPACE:
			if (which_mode == SPEC_TALK)
				break;
			hit_space(l,spec.ex2a,spec.ex2b,1,1);
			*redraw = 1;
			break;
		case SPEC_TOWN_EXPLODE_SPACE:
			if (which_mode == SPEC_TALK)
				break;
			radius_damage(l,spec.pic, spec.ex2a, spec.ex2b);
			*redraw = 1;
			break;
		case SPEC_TOWN_LOCK_SPACE:
			ter = coord_to_ter(spec.ex1a,spec.ex1b);
			if (scenario.ter_types[ter].special == TER_SPEC_LOCKABLE_TERRAIN)
				set_terrain(l,scenario.ter_types[ter].flag1);
			*redraw = 1;
			break;
		case SPEC_TOWN_UNLOCK_SPACE:
			ter = coord_to_ter(spec.ex1a,spec.ex1b);
			if ((scenario.ter_types[ter].special == TER_SPEC_UNLOCKABLE_TERRAIN) || (scenario.ter_types[ter].special == TER_SPEC_UNLOCKABLE_BASHABLE))
				set_terrain(l,scenario.ter_types[ter].flag1);
			*redraw = 1;
			break;
		case SPEC_TOWN_SFX_BURST:
			if (which_mode == SPEC_TALK)
				break;
			run_a_boom(l,spec.ex2a,0,0);
			break;
		case SPEC_TOWN_CREATE_WANDERING:
			create_wand_monst();
			*redraw = 1;
			break;
		case SPEC_TOWN_PLACE_MONST:
			if (spec.ex2a > 0)
				forced_place_monster(spec.ex2a,l);
				else c_town.placeMonster(spec.ex2a,l);
			*redraw = 1;
			break;
		case SPEC_TOWN_DESTROY_MONST:
			for (i = 0; i < T_M; i++)
				if (c_town.monst.dudes[i].number == spec.ex1a) {
					c_town.monst.dudes[i].active = 0;
					}
			*redraw = 1;
			break;
		case SPEC_TOWN_NUKE_MONSTS:
			for (i = 0; i < T_M; i++)
				if ((c_town.monst.dudes[i].active > 0) &&
					(((spec.ex1a == 0) && (1 == 1)) ||
					((spec.ex1a == 1) && (c_town.monst.dudes[i].attitude % 2 == 0)) ||
					((spec.ex1a == 2) && (c_town.monst.dudes[i].attitude % 2 == 1)))){
					c_town.monst.dudes[i].active = 0;
					}
			*redraw = 1;
			break;
		case SPEC_TOWN_GENERIC_LEVER:
			check_mess = false;
			if (which_mode > SPEC_TOWN_LOOK) {
				ASB("Can't use lever now.");
				check_mess = false;
				*next_spec = -1;
				}
				else if (store_special_loc.handleLever() > 0) *next_spec = spec.ex1b;
			break;
		case SPEC_TOWN_GENERIC_PORTAL:
			check_mess = false;
			if (is_combat()) {
				ASB("Not while in combat.");
				if (which_mode < SPEC_OUT_LOOK) *a = 1;//if moving
				*next_spec = -1;
				}
				else if ((which_mode != SPEC_TOWN_MOVE) && (which_mode != SPEC_TOWN_LOOK)) {
					ASB("Can't teleport now.");
					if (which_mode < SPEC_OUT_LOOK) *a = 1;
					*next_spec = -1;
					}
				else if (FCD(870,0) == 1) { *next_spec = -1; if (which_mode < SPEC_OUT_LOOK) *a = 1;}
                else{
                    if (which_mode < SPEC_OUT_LOOK) //if moving
					   *a = 1;
					if ((which_mode == SPEC_TALK) || (spec.ex2a == 0))
						teleport_party(spec.ex1a,spec.ex1b,1);
                    else
                        teleport_party(spec.ex1a,spec.ex1b,0);
					}
			break;
		case SPEC_TOWN_GENERIC_BUTTON:
			check_mess = false;
			if (FCD(871,0) == 2)
				*next_spec = spec.ex1b;
			break;
		case SPEC_TOWN_GENERIC_STAIR:
			check_mess = false;
/*			if (is_combat()) {
				ASB("Can't change level in combat.");
				if (which_mode < SPEC_OUT_LOOK) //if moving
					*a = 1;
				*next_spec = -1;
				}*/
				if ((which_mode != SPEC_TOWN_MOVE) && (PSD[SDF_COMPATIBILITY_ANYTIME_STAIRWAY_NODES] == 0)) {
					ASB("Can't change level now.");
					if (which_mode < SPEC_OUT_LOOK)//if moving
						*a = 1;
					*next_spec = -1;
					}
				else {
                    if (spec.ex2b < 0) spec.ex2b = 0;
                    if (spec.ex2b > 7) spec.ex2b = 7;
                    if (FCD(880 + spec.ex2b,0) == 1) {
                         *next_spec = -1;
                         if (which_mode < SPEC_OUT_LOOK) *a = 1;
                         }
                else {
                    if(overall_mode == MODE_TALKING)
                        end_talk_mode();
                    if (is_combat()){
						if (which_combat_type == 0){ //outdoor combat
						  end_combat();
   						  end_town_mode(0,c_town.p_loc);
               			  menu_activate(1);
						  put_pc_screen();
						  set_stat_window(current_pc);
                          }
                        else{ //town combat
                            party.direction = end_town_combat();
						    set_stat_window(current_pc);
						    menu_activate(1);
                        }
                    }
					*a = 1;
					change_level(spec.ex2a,l.x,l.y);
					}
                }
			break;
		case SPEC_TOWN_LEVER:
			check_mess = false;
			if (spec.m1 < 0)
				break;
			if (which_mode > SPEC_TOWN_LOOK) {
				ASB("Can't use lever now.");
				check_mess = false;
				*next_spec = -1;
				}
				else {
					for (i = 0; i < 3; i++)
						get_strs(strs[i * 2], strs[i * 2 + 1],cur_spec_type,
						spec.m1 + i * 2 + spec_str_offset[cur_spec_type],spec.m1 + i * 2 + 1 + spec_str_offset[cur_spec_type]);
					buttons[0] = 9; buttons[1] = 35;
					i = custom_choice_dialog((char *) strs,spec.pic,buttons);
					if (i == 1) {*next_spec = -1;}
						else {
							ter = coord_to_ter(store_special_loc.x,store_special_loc.y);
							set_terrain(store_special_loc,scenario.ter_types[ter].trans_to_what);
							*next_spec = spec.ex1b;
							}
					}
			break;
		case SPEC_TOWN_PORTAL:
			check_mess = false;
			if (spec.m1 < 0)
				break;
			if (is_combat()) {
				ASB("Not while in combat.");
				if (which_mode < SPEC_OUT_LOOK) //if moving
					*a = 1;
				*next_spec = -1;
				check_mess = false;
				}
				else if ((which_mode != SPEC_TOWN_MOVE) && (which_mode != SPEC_TOWN_LOOK)) {
					ASB("Can't teleport now.");
					if (which_mode < SPEC_OUT_LOOK) //if moving
						*a = 1;
					*next_spec = -1;
					check_mess = false;
					}
				else {
					for (i = 0; i < 3; i++)
						get_strs((char *) strs[i * 2],(char *) strs[i * 2 + 1]
						,cur_spec_type,spec.m1 + i * 2 + spec_str_offset[cur_spec_type],spec.m1 + i * 2 + 1 + spec_str_offset[cur_spec_type]);
					buttons[0] = 9; buttons[1] = 8;
					i = custom_choice_dialog((char *) strs,722,buttons);
					if (i == 1) { *next_spec = -1; if (which_mode < SPEC_OUT_LOOK) *a = 1;}
						else {
                            if (which_mode < SPEC_OUT_LOOK) //if moving
							    *a = 1;
							if (spec.ex2a == 0)
								teleport_party(spec.ex1a,spec.ex1b,1);
								else teleport_party(spec.ex1a,spec.ex1b,0);
							}
					}
			break;
		case SPEC_TOWN_STAIR:
			check_mess = false;
			if ((spec.m1 < 0) && (spec.ex2b != 1))
				break;
				if (which_mode != SPEC_TOWN_MOVE && PSD[SDF_COMPATIBILITY_ANYTIME_STAIRWAY_NODES] == 0) {
					ASB("Can't change level now.");
					if (which_mode < SPEC_OUT_LOOK) //if moving
						*a = 1;
					*next_spec = -1;
					check_mess = false;
					}
				else {
					if (spec.m1 >= 0) {
						for (i = 0; i < 3; i++)
							get_strs((char *) strs[i * 2],(char *) strs[i * 2 + 1],cur_spec_type,
							spec.m1 + i * 2 + spec_str_offset[cur_spec_type],spec.m1 + i * 2 + 1 + spec_str_offset[cur_spec_type]);
						buttons[0] = 20; buttons[1] = 24;
						}
					if (spec.ex2b == 1)
						i = 2;
						else i = custom_choice_dialog((char *) strs,719,buttons) ;

					if (i == 1) {
                        *next_spec = -1;
                        if(which_mode < SPEC_OUT_LOOK) //if moving
                             *a = 1;
                        }
						else {
                            if(overall_mode == MODE_TALKING)
                                end_talk_mode();
                            if (is_combat()){
                                i = 10; //was in combat ...
					           	if (which_combat_type == 0){ //outdoor combat
        						  end_combat();
   		       		      		  end_town_mode(0,c_town.p_loc);
                    			  menu_activate(1);
						          put_pc_screen();
        						  set_stat_window(current_pc);
                                  }
                                else{ //town combat
                                    party.direction = end_town_combat();
        						    set_stat_window(current_pc);
		      				    menu_activate(1);
                              }
                            }
							*a = 1;
							change_level(spec.ex2a,l.x,l.y);
                            if(i == 10){ // ... so back in combat
                                start_town_combat(party.direction);
                                }
							}
					}
			break;
		case SPEC_TOWN_RELOCATE:
			position_party(spec.ex1a,spec.ex1b,spec.ex2a,spec.ex2b);
			break;
		case SPEC_TOWN_PLACE_ITEM:
			store_i = get_stored_item(spec.ex2a);
			place_item(store_i,l,true);
			break;
		case SPEC_TOWN_SPLIT_PARTY:
			if (which_mode == SPEC_TALK)
				break;
			if (is_combat()) {
				ASB("Not while in combat.");
				if (which_mode < SPEC_OUT_LOOK) //if moving
					*a = 1;
				*next_spec = -1;
				check_mess = false;
				}
			else if (party.stuff_done[SDF_IS_PARTY_SPLIT] > 0) {
				ASB("Party is already split.");
				if (which_mode < SPEC_OUT_LOOK) //if moving
					*a = 1;
				*next_spec = -1;
				check_mess = false;
				}
			else{
			r1 = char_select_pc(1,0,"Which character goes?");
			if (which_mode < SPEC_OUT_LOOK) //if moving
				*a = 1;
			if (r1 != 6) {
				party.stuff_done[SDF_PARTY_SPLIT_PC] = r1;
				*next_spec = -1;
				start_split(spec.ex1a,spec.ex1b,spec.ex2a);
				}
				else check_mess = false;
            }
			break;
		case SPEC_TOWN_REUNITE_PARTY:
			if (is_combat()) {
				ASB("Not while in combat.");
				break;
				}
			if (which_mode < SPEC_OUT_LOOK) //if moving
				*a = 1;
			*next_spec = -1;
			check_mess = false;
			end_split(spec.ex1a);
			break;
		case SPEC_TOWN_TIMER_START:
			for (i = 0; i < 30; i++)
	 			if (party.party_event_timers[i] == 0) {
	 				party.party_event_timers[i] = spec.ex1a;
	 				party.node_to_call[i] = spec.ex1b;
	 				party.global_or_town[i] = 1;
	 				i = 30;
	 				}
			break;
        case SPEC_TOWN_CHANGE_LIGHTNING:
            if(spec.ex1a >= 0 && spec.ex1a < 4)//change town lightning
                c_town.town.lighting = spec.ex1a;
            if(spec.ex2b == 0)//give party light
                party.light_level += spec.ex2a;
            else if(spec.ex2b == 1)//take party light
                party.light_level -= spec.ex2a;
            break;
        case SPEC_TOWN_CHANGE_ATTITUDE: //attitude: 0 - Friendly, Docile, 1 - Hostile, Type A, 2 - Friendly, Will fight, 3 - Hostile, Type B
            if((spec.ex1a < 0) || (spec.ex1a > 59)){
                give_error("The monster number in Extra 1a was out of bound (should be between 0 and 59).","",0);
                break;
                }
            if((spec.ex1b < 0) || (spec.ex1b > 3)){
                give_error("The attitude Extra 1b was out of bound (0 - Friendly Docile, 1 - Hostile Type A, 2 - Friendly Will Fight, 3 -Hostile Type B).","",0);
                break;
                }
            c_town.monst.dudes[spec.ex1a].attitude = spec.ex1b;
            break;
		}
	if (check_mess == true) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
}

void rect_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = true;
	short i,j,k;
	special_node_type spec;
	location l;
	unsigned char ter;

	spec = cur_node;
	*next_spec = cur_node.jumpto;

	if (is_out())
		return;

	*redraw = 1;
	for (i = spec.ex1b;i <= spec.ex2b;i++){
		for (j = spec.ex1a; j <= spec.ex2a; j++) {

	l.x = i; l.y = j;
	switch (cur_node.type) {
		case SPEC_RECT_PLACE_FIRE: if (get_ran(1,1,100) <= spec.sd1 ) make_fire_wall(i,j); break;
		case SPEC_RECT_PLACE_FORCE: if (get_ran(1,1,100) <= spec.sd1 ) make_force_wall(i,j); break;
		case SPEC_RECT_PLACE_ICE: if (get_ran(1,1,100) <= spec.sd1 ) make_ice_wall(i,j); break;
		case SPEC_RECT_PLACE_BLADE: if (get_ran(1,1,100) <= spec.sd1 ) make_blade_wall(i,j); break;
		case SPEC_RECT_PLACE_SCLOUD: if (get_ran(1,1,100) <= spec.sd1 ) make_scloud(i,j); break;
		case SPEC_RECT_PLACE_SLEEP: if (get_ran(1,1,100) <= spec.sd1 ) make_sleep_cloud(i,j); break;
		case SPEC_RECT_PLACE_QUICKFIRE: if (get_ran(1,1,100) <= spec.sd1 ) make_quickfire(i,j); break;
		case SPEC_RECT_PLACE_FIRE_BARR: if (get_ran(1,1,100) <= spec.sd1 ) make_fire_barrier(i,j); break;
		case SPEC_RECT_PLACE_FORCE_BARR: if (get_ran(1,1,100) <= spec.sd1 ) make_force_barrier(i,j); break;
		case SPEC_RECT_CLEANSE: if (spec.sd1 == 0) dispel_fields(i,j,1); else dispel_fields(i,j,2);break;
		case SPEC_RECT_PLACE_SFX: if (get_ran(1,1,100) <= spec.sd1 ) make_sfx(i,j,spec.sd2 + 1); break;
		case SPEC_RECT_PLACE_OBJECT: if (get_ran(1,1,100) <= spec.sd1 ) {
					if (spec.sd2 == 0) make_web(i,j);
					if (spec.sd2 == 1) make_barrel(i,j);
					if (spec.sd2 == 2) make_crate(i,j);
					} break;
		case SPEC_RECT_MOVE_ITEMS:
			for (k = 0; k < NUM_TOWN_ITEMS; k++)
				if ((t_i.items[k].variety > ITEM_TYPE_NO_ITEM) && (same_point(t_i.items[k].item_loc,l) == true)) {
					t_i.items[k].item_loc.x = spec.sd1;
					t_i.items[k].item_loc.y = spec.sd2;
					}
			break;
		case SPEC_RECT_DESTROY_ITEMS:
			for (k = 0; k < NUM_TOWN_ITEMS; k++)
				if ((t_i.items[k].variety > ITEM_TYPE_NO_ITEM) && (same_point(t_i.items[k].item_loc,l) == true)) {
					t_i.items[k].variety = ITEM_TYPE_NO_ITEM;
					}
			break;
		case SPEC_RECT_CHANGE_TER:
			if (get_ran(1,1,100) <= spec.sd2){
                set_terrain(l,spec.sd1);
	            if(scenario.ter_types[spec.sd1].special >= TER_SPEC_CONVEYOR_NORTH && scenario.ter_types[spec.sd1].special <= TER_SPEC_CONVEYOR_WEST)
                    belt_present = true;
            if(party.stuff_done[SDF_HIDDEN_MAP] == 0)
                draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);

            }
			break;
		case SPEC_RECT_SWAP_TER:
			if (coord_to_ter(i,j) == spec.sd1){
                set_terrain(l,spec.sd2);
                if(scenario.ter_types[spec.sd2].special >= TER_SPEC_CONVEYOR_NORTH && scenario.ter_types[spec.sd2].special <= TER_SPEC_CONVEYOR_WEST)
                      belt_present = true;
            }
			else if (coord_to_ter(i,j) == spec.sd2){
                set_terrain(l,spec.sd1);
                if(scenario.ter_types[spec.sd1].special >= TER_SPEC_CONVEYOR_NORTH && scenario.ter_types[spec.sd1].special <= TER_SPEC_CONVEYOR_WEST)
                    belt_present = true;
            }
            if(party.stuff_done[SDF_HIDDEN_MAP] == 0)
                draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		case SPEC_RECT_TRANS_TER:
			ter = coord_to_ter(i,j);
			set_terrain(l,scenario.ter_types[ter].trans_to_what);
			if(scenario.ter_types[scenario.ter_types[ter].trans_to_what].special >= TER_SPEC_CONVEYOR_NORTH && scenario.ter_types[scenario.ter_types[ter].trans_to_what].special <= TER_SPEC_CONVEYOR_WEST)
                belt_present = true;
            if(party.stuff_done[SDF_HIDDEN_MAP] == 0)
                draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		case SPEC_RECT_LOCK:
			ter = coord_to_ter(i,j);
			if (scenario.ter_types[ter].special == TER_SPEC_LOCKABLE_TERRAIN)
				set_terrain(l,scenario.ter_types[ter].flag1);
			if(scenario.ter_types[scenario.ter_types[ter].flag1].special >= TER_SPEC_CONVEYOR_NORTH && scenario.ter_types[scenario.ter_types[ter].flag1].special <= TER_SPEC_CONVEYOR_WEST)
                belt_present = true;
            if(party.stuff_done[SDF_HIDDEN_MAP] == 0)
                draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		case SPEC_RECT_UNLOCK:
			ter = coord_to_ter(i,j);
			if ((scenario.ter_types[ter].special == TER_SPEC_UNLOCKABLE_TERRAIN) || (scenario.ter_types[ter].special == TER_SPEC_UNLOCKABLE_BASHABLE))
				set_terrain(l,scenario.ter_types[ter].flag1);
		  if(scenario.ter_types[scenario.ter_types[ter].flag1].special >= TER_SPEC_CONVEYOR_NORTH && scenario.ter_types[scenario.ter_types[ter].flag1].special <= TER_SPEC_CONVEYOR_WEST)
                belt_present = true;
            if(party.stuff_done[SDF_HIDDEN_MAP] == 0)
                draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		}
	}
}
	if (check_mess == true) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
}

void outdoor_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = false;
	char str1[256] = "",str2[256] = "";
	special_node_type spec;
	location l;

	spec = cur_node;
	*next_spec = cur_node.jumpto;

	if (is_out() == false) return;

	switch (cur_node.type) {
		case SPEC_OUT_MAKE_WANDER: create_wand_monst();
			*redraw = 1;
			break;
		case SPEC_OUT_CHANGE_TER:
			outdoors[party.i_w_c.x][party.i_w_c.y].terrain[spec.ex1a][spec.ex1b] = spec.ex2a;
			l.x = spec.ex1a;
			l.y = spec.ex1b;
			l = l.toGlobal();
			out[l.x][l.y] = spec.ex2a;
			*redraw = 1;
			check_mess = true;
			break;
		case SPEC_OUT_PLACE_ENCOUNTER:
			if (spec.ex1a != minmax(0,3,(int)spec.ex1a)) {
				give_error("Special outdoor enc. is out of range. Must be 0-3.","",0);
				//set_sd = false;
				}
				else {
					l = party.p_loc.toLocal();
					place_outd_wand_monst(l,
					outdoors[party.i_w_c.x][party.i_w_c.y].special_enc[spec.ex1a],true);
					check_mess = true;
					}
			break;
		case SPEC_OUT_MOVE_PARTY:
			check_mess = true;
			out_move_party(spec.ex1a,spec.ex1b);
			*redraw = 1;
			*a = 1;
			break;
		case SPEC_OUT_STORE:
			get_strs(str1, str2,1,spec.m1 + 10,-1);
			if (spec.ex2a >= 40)
				spec.ex2a = 39;
			if (spec.ex2a < 1)
				spec.ex2a = 1;
			spec.ex2b = minmax(0,6,(int)spec.ex2b);
			switch (spec.ex1b) {
				case 0: start_shop_mode(SHOP_WEAPON_SHOP,spec.ex1a,spec.ex1a + spec.ex2a - 1,spec.ex2b, str1); break;
				case 1: start_shop_mode(SHOP_MAGE_SPELLS,spec.ex1a,spec.ex1a + spec.ex2a - 1,spec.ex2b, str1); break;
				case 2: start_shop_mode(SHOP_PRIEST_SPELLS,spec.ex1a,spec.ex1a + spec.ex2a - 1 ,spec.ex2b, str1); break;
				case 3: start_shop_mode(SHOP_ALCHEMY,spec.ex1a,spec.ex1a + spec.ex2a - 1,spec.ex2b, str1); break;
				case 4: start_shop_mode(SHOP_HEALER,spec.ex1a,spec.ex1a + spec.ex2a - 1,spec.ex2b, str1); break;
				}
			*next_spec = -1;
			break;
		}

	if (check_mess == true) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
}

void setsd(short a,short b,short val)
{
	if (sd_legit(a,b) == false) {
		give_error("The scenario attempted to change an out of range Stuff Done flag.","",0);
		return;
		}
	PSD[a][b] = val;
}

void handle_message(short which_mode,short cur_type,short mess1,short mess2,short *a,short *b)
{
	char str1[256] = "",str2[256] = "";
	short label1 = -1,label2 = -1,label1b = -1,label2b = -1;
	short mess_adj[3] = {160,10,20};

	if ((mess1 < 0) && (mess2 < 0)){
    	return;
        }
	if (which_mode == SPEC_TALK) { // talking
		*a = mess1 + ((mess1 >= 0) ? mess_adj[cur_type] : 0);
		*b = mess2 + ((mess2 >= 0) ? mess_adj[cur_type] : 0);
		return;
		}
	get_strs(str1, str2, cur_type, mess1 + ((mess1 >= 0) ? mess_adj[cur_type] : 0),
		mess2 + ((mess2 >= 0) ? mess_adj[cur_type] : 0)) ;
	if (mess1 >= 0) {
		label1 = 1000 * cur_type + mess1 + mess_adj[cur_type];
		label1b = (is_out()) ? (party.outdoor_corner.x + party.i_w_c.x) +
			scenario.out_width * (party.outdoor_corner.y + party.i_w_c.y) : c_town.town_num;
		}
	if (mess2 >= 0) {
		label2 = 1000 * cur_type + mess2 + mess_adj[cur_type];
		label2b = (is_out()) ? (party.outdoor_corner.x + party.i_w_c.x) +
			scenario.out_width * (party.outdoor_corner.y + party.i_w_c.y) : c_town.town_num;
		}
	display_strings(str1, str2,label1,label2, label1b,label2b,
		"",57,1600 + scenario.intro_pic,0);
}

void get_strs(char *str1,char *str2,short cur_type,short which_str1,short which_str2)
{
	short num_strs[3] = {260,108,135};

	if (((which_str1 >= 0) && (which_str1 != minmax((short) 0,num_strs[cur_type],which_str1))) ||
		((which_str2 >= 0) && (which_str2 != minmax((short) 0,num_strs[cur_type],which_str2)))) {
		give_error("The scenario attempted to access a message out of range.","",0);
		return;
		}
	switch (cur_type) {
		case 0:
			if (which_str1 >= 0) {
				if (which_str1 < 160)
					strcpy(str1,data_store5->scen_strs[which_str1]);
					else strcpy(str1,scen_strs2[which_str1 - 160]);

				}
			if (which_str2 >= 0){
				if (which_str2 < 160)
					strcpy(str2,data_store5->scen_strs[which_str2]);
					else strcpy(str2,scen_strs2[which_str2 - 160]);
				}
			break;
		case 1:
			if (which_str1 >= 0)
				load_outdoors(party.outdoor_corner.x + party.i_w_c.x,
					party.outdoor_corner.y + party.i_w_c.y, party.i_w_c.x, party.i_w_c.y,
					1,which_str1, str1);
			if (which_str2 >= 0)
				load_outdoors(party.outdoor_corner.x + party.i_w_c.x,
					party.outdoor_corner.y + party.i_w_c.y, party.i_w_c.x, party.i_w_c.y,
					1,which_str2, str2);
			break;
		case 2:
			if (which_str1 >= 0)
				strcpy(str1,data_store->town_strs[which_str1]);
			if (which_str2 >= 0)
				strcpy(str2,data_store->town_strs[which_str2]);
			break;
		}

}

void use_spec_item(short item)
{
	short i,j,k;

	run_special(SPEC_USE_SPEC_ITEM,0,scenario.special_item_special[item],location(),&i,&j,&k);
}
