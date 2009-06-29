#include <windows.h>
#include "stdio.h"
#include "string.h"
#include "global.h"
#include "party.h"
#include "town.h"
#include "text.h"
#include "infodlgs.h"
#include "items.h"
#include "itemdata.h"
#include "combat.h"
#include "monster.h"
#include "locutils.h"
#include "fields.h"
#include "exlsound.h"
#include "graphics.h"
#include "fileio.h"
#include "specials.h"
#include "newgraph.h"
#include "dlgutils.h"

#include "globvar.h"


void start_split(short a, short b, short noise) 
{
	party.stuff_done[304][0] = 1;
	party.stuff_done[304][1] = c_town.p_loc.x;
	party.stuff_done[304][2] = c_town.p_loc.y;
	c_town.p_loc.x = a;
	c_town.p_loc.y = b;
	for (int i = 0; i < NUM_OF_PCS; i++)
		if (i != party.stuff_done[304][3] && adven[i].main_status > 0)
			adven[i].main_status += MAIN_STATUS_SPLIT;
	current_pc = party.stuff_done[304][3];
	update_explored(c_town.p_loc);
	center = c_town.p_loc;
	
	if (noise > 0)
		play_sound(10);	
}

void end_split(short noise) 
{
	if (party.stuff_done[304][0] == 0) {
		ASB("Party already together!");
		return;
		}
		
	c_town.p_loc.x = party.stuff_done[304][1];
	c_town.p_loc.y = party.stuff_done[304][2];
	party.stuff_done[304][0] = 0;
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
	run_special(13,1,store_wandering_special.spec_on_meet,null_loc,&s1,&s2,&s3);
	if (s1 > 0)
		return FALSE;
	}
		
	if ((mode == 1) && (store_wandering_special.spec_on_win >= 0))  {// After defeating
	run_special(15,1,store_wandering_special.spec_on_win,null_loc,&s1,&s2,&s3);
		}
	if ((mode == 2) && (store_wandering_special.spec_on_flee >= 0))  {// After fleeing like a buncha girly men
	run_special(14,1,store_wandering_special.spec_on_flee,null_loc,&s1,&s2,&s3);
		}		
return TRUE;
}


Boolean check_special_terrain(location where_check,short mode,short which_pc,short *spec_num,
	Boolean *forced)
//short mode; // 0 - out 1 - town 2 - combat
{
	unsigned char ter;
	short r1,i,choice,door_pc,ter_special,ter_flag1,ter_flag2,dam_type = 0,pic_type = 0,ter_pic = 0;
	Boolean can_enter = TRUE;
	location out_where,from_loc,to_loc;
	short s1 = 0,s2 = 0,s3 = 0;
	char create_line[128];
	
	*spec_num = -1;
	*forced = FALSE;
	
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

		if ((mode > 0) && (ter_special >= 16) && 
			(ter_special <= 19)) {
			if (
				((ter_special == 16) && (where_check.y > from_loc.y)) ||
				((ter_special == 17) && (where_check.x < from_loc.x)) ||
				((ter_special == 18) && (where_check.y < from_loc.y)) ||
				((ter_special == 19) && (where_check.x > from_loc.x)) ) {
					ASB("The moving floor prevents you.");
					return FALSE;
					}
			}

			if (mode == 0) {
				out_where = where_check.toLocal();

				for (i = 0; i < 18; i++) 
					if (same_point(out_where,outdoors[party.i_w_c.x][party.i_w_c.y].special_locs[i]) == TRUE) {
						*spec_num = outdoors[party.i_w_c.x][party.i_w_c.y].special_id[i];
						if ((*spec_num >= 0) &&
							(outdoors[party.i_w_c.x][party.i_w_c.y].specials[*spec_num].type == 4))
								*forced = TRUE;
						// call special
						run_special(mode,1,outdoors[party.i_w_c.x][party.i_w_c.y].special_id[i],out_where,&s1,&s2,&s3);
						if (s1 > 0)
							can_enter = FALSE;
						erase_out_specials();
						put_pc_screen();	
						put_item_screen(stat_window,0);	
					}
				}

	if ((is_combat()) && (((ter_pic <= 207) && (ter_pic >= 212)) || (ter_pic == 406))) {
		ASB("Move: Can't trigger this special in combat.");
		return FALSE;
		}

	if (((mode == 1) || ((mode == 2) && (which_combat_type == 1)))
	&& (special(where_check.x,where_check.y))) {
			if (is_force_barrier(where_check.x,where_check.y)) {
				add_string_to_buf("  Magic barrier!               ");	
				return FALSE;
				}
			for (i = 0; i < 50; i++)
				if ((same_point(where_check,c_town.town.special_locs[i]) == TRUE))
					//GK && (c_town.town.spec_id[i] >= 0))
					{
					if (c_town.town.specials[c_town.town.spec_id[i]].type == 4) {
						*forced = TRUE;
						}
					*spec_num = c_town.town.spec_id[i];
					if ((is_blocked(where_check) == FALSE) || (ter_special == 1)
	 					|| (ter_special == 12) || (ter_special == 13)) {
						give_help(54,0,0);
						run_special(mode,2,c_town.town.spec_id[i],where_check,&s1,&s2,&s3);
						if (s1 > 0)
							can_enter = FALSE;
						}
					}
			put_pc_screen();	
			put_item_screen(stat_window,0);	
		}
	
	if((mode == 2) && town_boat_there(where_check) < 30){//can't enter boats while in combat mode
        sprintf ((char *) create_line, "Blocked: %s",dir_string[set_direction(pc_pos[current_pc], where_check)]);
		add_string_to_buf((char *) create_line);
        can_enter = FALSE;
        }
	if (can_enter == FALSE)
		return FALSE;
	
	if ((!is_out()) && (overall_mode < MODE_TALKING)) {
	check_fields(where_check,mode,which_pc);

	if (is_web(where_check.x,where_check.y)) {
		add_string_to_buf("  Webs!               ");
		if (mode < 2) {
			suppress_stat_screen = TRUE;
			for (i = 0; i < 6; i++) {
				r1 = get_ran(1,2,3);
				adven[i].web(r1);
				}
			suppress_stat_screen = TRUE;
			put_pc_screen();	
			}
			else adven[current_pc].web(get_ran(1,2,3));
		take_web(where_check.x,where_check.y);
		}
	if (is_force_barrier(where_check.x,where_check.y)) {
		add_string_to_buf("  Magic barrier!               ");	
		can_enter = FALSE;
		}
	if (is_crate(where_check.x,where_check.y)) {
		add_string_to_buf("  You push the crate.");
		to_loc = push_loc(from_loc,where_check);
		take_crate((short) where_check.x,(short) where_check.y);
		if (to_loc.x > 0)
			make_crate((short) to_loc.x,(short) to_loc.y);
		for (i = 0; i < NUM_TOWN_ITEMS; i++)
			if ((t_i.items[i].variety > 0) && (same_point(t_i.items[i].item_loc,where_check))
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
			if ((t_i.items[i].variety > 0) && (same_point(t_i.items[i].item_loc,where_check))
			 && (t_i.items[i].isContained()))
			 	t_i.items[i].item_loc = to_loc;
		}
		}


	switch (ter_special) {
		case 1: 
			alter_space(where_check.x,where_check.y,ter_flag1);
			if (ter_flag2 < 200) {
				play_sound(-1 * ter_flag2);
				}
			give_help(47,65,0);
			if (scenario.ter_types[ter].blockage > 2)
				can_enter = FALSE;
			break;
		case 2: case 3:case 4:
			if (party.isFlying() || party.in_boat >= 0 || party.in_horse >= 0 || (mode ? town_boat_there(where_check) : out_boat_there(where_check)) < 30)
				break;
			if (ter_special == 2) {
				add_string_to_buf("  It's hot!");
				dam_type = 1; pic_type = 0;
				if (party.stuff_done[305][3] > 0) {
					add_string_to_buf("  It doesn't affect you.");			
					break;
					}
				}
			if (ter_special == 3) {
				add_string_to_buf("  You feel cold!");
				dam_type = 5; pic_type = 4;
				}
			if (ter_special == 4) {
				add_string_to_buf("  Something shocks you!");
				dam_type = 3; pic_type = 1;
				}
			r1 = get_ran(ter_flag2,dam_type,ter_flag1);
			if (mode < 2)
				adven.damage(r1,dam_type);
			fast_bang = 1;
			if (mode == 2)
				adven[which_pc].damage(r1,dam_type,-1);
			fast_bang = 0;
			break;
		case 5: case 6:
			if (party.isFlying() || party.in_boat >= 0 || (mode ? town_boat_there(where_check) : out_boat_there(where_check)) < 30)
			    break;
			one_sound(17);
			if (mode < 2) {
				for (i = 0; i < 6; i++) 
					if (adven[i].isAlive()) 
						{
						if (get_ran(1,1,100) <= ter_flag2) {
							if (ter_special == 5)
								adven[i].poison(ter_flag1);
								else adven[i].disease(ter_flag1);
							}
						}
				}
			//print_nums(1,which_pc,current_pc);
			if (mode == 2) {
						if (get_ran(1,1,100) <= ter_flag2){
							if (ter_special == 5)
								adven[which_pc].poison(ter_flag1);
								else adven[which_pc].disease(ter_flag1);
							}
					}
			break;
		case 12: // local special
		  if(mode == 0){
			run_special(mode,1,ter_flag1,where_check,&s1,&s2,&s3);
					if (s1 > 0)
						can_enter = FALSE;
            }
		  else if(mode == 1){
			run_special(mode,2,ter_flag1,where_check,&s1,&s2,&s3);
					if (s1 > 0)
						can_enter = FALSE;
                    }
          else if(mode == 2){
            if(which_combat_type == 0)//outdoor combat
       			run_special(mode,1,ter_flag1,where_check,&s1,&s2,&s3);
            else
       			run_special(mode,2,ter_flag1,where_check,&s1,&s2,&s3);
            }
			break;
		case 13: // global special
			run_special(mode,0,ter_flag1,where_check,&s1,&s2,&s3);
					if (s1 > 0)
						can_enter = FALSE;
			break;

			
		// Locked doors
		case 9: case 10:
			if (is_combat()) {  // No lockpicking in combat
				add_string_to_buf("  Can't enter: It's locked.");
				break;
				}
				
			// See what party wants to do.
			choice = fancy_choice_dialog(993,0);
					
			can_enter = FALSE;
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
			suppress_stat_screen = TRUE;
			for (i = 0; i < 6; i++) {
				r1 = get_ran(1,2,3);
				adven[i].curse(r1);
				}
			suppress_stat_screen = FALSE;
			put_pc_screen();
			}
			else adven[current_pc].curse(get_ran(1,2,3));
		}
	if (is_sleep_cloud(where_check.x,where_check.y)) {
		add_string_to_buf("  Sleep cloud!               ");
		if (mode < 2) {
			suppress_stat_screen = TRUE;
			for (i = 0; i < NUM_OF_PCS; i++) {
				adven[i].sleep(3,11,0);
				}
			suppress_stat_screen = FALSE;
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
	Boolean take_charge = TRUE,inept_ok = FALSE;
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
		inept_ok = TRUE;
		}

	if (is_out()) user_loc = party.p_loc;
	if (is_town()) user_loc = c_town.p_loc;
	if (is_combat()) user_loc = pc_pos[current_pc];

	if (item_use_code == 4) {
		add_string_to_buf("Use: Can't use this item.       ");
		take_charge = FALSE;
		}
	if ((adven[pc].traits[TRAIT_MAGICALLY_INEPT] == TRUE) && (inept_ok == FALSE)){
		add_string_to_buf("Use: Can't - magically inept.       ");
		take_charge = FALSE;
		}
	
	if (take_charge == TRUE) {
			if ((overall_mode == MODE_OUTDOORS) && (item_use_code > 0) && (item_use_code != 5)) {
				add_string_to_buf("Use: Not while outdoors.         ");
				take_charge = FALSE;
				}
			if ((overall_mode == MODE_TOWN) && (item_use_code == 1)) {
				add_string_to_buf("Use: Not while in town.         ");
				take_charge = FALSE;
				}
			if ((overall_mode == MODE_COMBAT) && (item_use_code == 2)) {
				add_string_to_buf("Use: Not in combat.         ");
				take_charge = FALSE;
				}
			if ((overall_mode != MODE_OUTDOORS) && (item_use_code == 5)){
				add_string_to_buf("Use: Only outdoors.           ");
				take_charge = FALSE;
				}
		}
	if (take_charge == TRUE) {
		if (adven[pc].items[item].isIdent() == false)
			sprintf((char *) to_draw, "Use: %s",adven[pc].items[item].name);
			else sprintf((char *) to_draw, "Use: %s",adven[pc].items[item].full_name);
		add_string_to_buf((char *) to_draw);

		if ((adven[pc].items[item].variety == ITEM_TYPE_POTION))// &&
//		      (adven[pc].items[item].graphic_num >= 50) && (adven[pc].items[item].graphic_num <= 52))
		      	play_sound(56);
		
		str = adven[pc].items[item].ability_strength;
		store_item_spell_level = str * 2 + 1;
		type = adven[pc].items[item].magic_use_type;
				
		switch (abil) {
			case 70: // poison weapon
				take_charge = poison_weapon(pc,str,0);
				break;				
			case 71: case 73:  case 74: case 75: case 76: case 78: case 80:
				switch (abil) {
					case 71: 
						play_sound(4);
						which_stat = 1;
						if (type % 2 == 1) {
							ASB("  You feel awkward."); str = str * -1;}
							else ASB("  You feel blessed.");
						break;
					case 73: 
						play_sound(75);
						which_stat = 3;
						if (type % 2 == 1) {
							ASB("  You feel sluggish."); str = str * -1;}
							else ASB("  You feel speedy.");
						break;
					case 74:
						play_sound(68);
						which_stat = 4;
						if (type % 2 == 1) {
							ASB("  You feel odd."); str = str * -1;}
							else ASB("  You feel protected.");
						break;
					case 75: 
						play_sound(51);
						which_stat = 5;
						if (type % 2 == 1) {
							ASB("  You feel odd."); str = str * -1;}
							else ASB("  You feel protected.");
						break;
					case 76: 
						which_stat = 6;
						if (type % 2 == 1)
							ASB("  You feel sticky.");
							else {
								ASB("  Your skin tingles."); str = str * -1;}
						break;
					case 78: 
						play_sound(43);
						which_stat = 8;
						if (type % 2 == 1) {
							ASB("  You feel exposed."); str = str * -1;}
							else ASB("  You feel obscure.");
						break;
					case 80: 
						play_sound(43);
						which_stat = 10;
						if (type % 2 == 1) {
							ASB("  You feel dull."); str = str * -1;}
							else ASB("  You start to glow slightly.");
						break;
					}
				if (type > 1)
					adven.affect(which_stat,str);
					else affect_pc(pc,which_stat,str);
				break;
			case 72:
				switch (type) {
					case 0: ASB("  You feel better."); adven[pc].cure(str); break;
					case 1: ASB("  You feel ill."); adven[pc].poison(str); break;
					case 2: ASB("  You all feel better."); adven.cure(str); break;
					case 3: ASB("  You all feel ill."); adven.poison(str); break;
					}
				break;
			case 77:
				switch (type) {
					case 0: ASB("  You feel healthy."); affect_pc(pc,7,-1 * str); break;
					case 1: ASB("  You feel sick."); adven[pc].disease(str); break;
					case 2: ASB("  You all feel healthy."); adven.affect(7,-1 * str); break;
					case 3: ASB("  You all feel sick."); adven.disease(str); break;
					}
				break;
			case 79:
				switch (type) {
					case 0: ASB("  You feel clear headed."); affect_pc(pc,9,-1 * str); break;
					case 1: ASB("  You feel confused."); adven[pc].dumbfound(str); break;
					case 2: ASB("  You all feel clear headed."); adven.affect(9,-1 * str); break;
					case 3: ASB("  You all feel confused."); adven.dumbfound(str); break;
					}
				break;
			case 81:
				switch (type) {
					case 0: ASB("  You feel alert."); affect_pc(pc,11,-1 * str); break;
					case 1: ASB("  You feel very tired."); adven[pc].sleep(str + 1,11,200); break;
					case 2: ASB("  You all feel alert."); adven.affect(11,-1 * str); break;
					case 3: ASB("  You all feel very tired."); for (i = 0; i < 6; i++) adven[i].sleep(str + 1,11,200); break;
					}
				break;
			case 82:
				switch (type) {
					case 0: ASB("  You find it easier to move."); affect_pc(pc,12,-1 * str * 100); break;
					case 1: ASB("  You feel very stiff."); adven[pc].sleep(str * 20 + 10,12,200); break;
					case 2: ASB("  You all find it easier to move."); adven.affect(12,-1 * str * 100); break;
					case 3: ASB("  You all feel very stiff."); for (i = 0; i < 6; i++) adven[i].sleep(str * 20 + 10,12,200); break;
					}
				break;
			case 83:
				switch (type) {
					case 0: ASB("  Your skin tingles pleasantly."); affect_pc(pc,13,-1 * str); break;
					case 1: ASB("  Your skin burns!"); adven[pc].acid(str); break;
					case 2: ASB("  You all tingle pleasantly."); adven.affect(13,-1 * str); break;
					case 3: ASB("  Everyone's skin burns!"); for (i = 0; i < 6; i++) adven[i].acid(str); break;
					}
				break;
			case 84:
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
			case 85:
				switch (type) {
					case 0: ASB("  You feel much smarter."); adven[pc].giveXP(str * 5); break;
					case 1: ASB("  You feel forgetful."); adven[pc].drainXP(str * 5); break;
					case 2: ASB("  You all feel much smarter."); adven.giveXP(str * 5); break;
					case 3: ASB("  You all feel forgetful."); adven.drainXP(str * 5); break;
					}
				break;
			case 86:
				play_sound(68);
				switch (type) {
					case 0: ASB("  You feel much smarter."); adven[pc].skill_pts += str; break;
					case 1: ASB("  You feel forgetful."); adven[pc].skill_pts = max(0,adven[pc].skill_pts - str); break;
					case 2: ASB("  You all feel much smarter."); for (i = 0; i < 6; i++) adven[i].skill_pts += str; break;
					case 3: ASB("  You all feel forgetful."); for (i = 0; i < 6; i++) adven[i].skill_pts = max(0,adven[i].skill_pts - str); break;
					}
				break;
			case 87:
				switch (type) {
					case 0: ASB("  You feel better."); adven[pc].heal(str * 20); break;
					case 1: ASB("  You feel sick."); adven[pc].damage(20 * str,4,0); break;
					case 2: ASB("  You all feel better."); adven.heal(str * 20); break;
					case 3: ASB("  You all feel sick."); adven.damage(20 * str,4); break;
					}
				break;
			case 88:
				switch (type) {
					case 0: ASB("  You feel energized."); adven[pc].restoreSP(str * 5); break;
					case 1: ASB("  You feel drained."); adven[pc].cur_sp = max(0,adven[pc].cur_sp - str * 5); break;
					case 2: ASB("  You all feel energized."); adven.restoreSP(str * 5); break;
					case 3: ASB("  You all feel drained."); for (i = 0; i < 6; i++) adven[i].cur_sp = max(0,adven[i].cur_sp - str * 5); break;
					}
				break;
			case 89:
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
			case 90:
				ASB("  You have more light."); increase_light(50 * str);
				break;
			case 91:
				ASB("  Your footsteps become quieter."); party.stuff_done[305][0] += 5 * str;
				break;
			case 92:
				ASB("  You feel chilly."); party.stuff_done[305][3] += 2 * str;
				break;
			case 93:
				if (party.stuff_done[305][1] > 0) {
					add_string_to_buf("  Not while already flying.          ");
					break;
					}
				if (party.in_boat >= 0)
					add_string_to_buf("  Leave boat first.             "); 
				else if (party.in_horse >= 0)////
					add_string_to_buf("  Leave horse first.             "); 
				else {
					ASB("  You rise into the air!"); party.stuff_done[305][1] += str;
					}
				break;
			case 94:
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
				
			// spell effects
			case 110:
				add_string_to_buf("  It fires a bolt of flame.");
				start_spell_targeting(1011);
				break;
			case 111:
				add_string_to_buf("  It shoots a fireball.         ");
				start_spell_targeting(1022);
				break;
			case 112:
				add_string_to_buf("  It shoots a huge fireball. ");
				start_spell_targeting(1040);
				break;
			case 113:
				add_string_to_buf("  It shoots a black ray.  ");
				start_spell_targeting(1048);
				break;
			case 114:
				add_string_to_buf("  It fires a ball of ice.   ");
				start_spell_targeting(1031);
				break;
			case 115:
				add_string_to_buf("  It fires a purple ray.   ");
				start_spell_targeting(1012);
				break;
			case 116:
				add_string_to_buf("  The ground shakes!        ");
				do_shockwave(pc_pos[current_pc]);
				break;
			case 117:
				add_string_to_buf("  It shoots a white ray.   ");
				start_spell_targeting(1132);
				break;		
			case 118:
				add_string_to_buf("  It shoots a golden ray.   ");
				start_spell_targeting(1155);
				break;	
			case 119:
				if (summon_monster(str,user_loc,50,2) == FALSE)
					add_string_to_buf("  Summon failed.");
				break;
			case 120:
				r1 = get_ran(6,1,4);
				j = get_ran(1,3,5);
				for (i = 0; i < j; i++)
					if (summon_monster(str,user_loc,r1,2) == FALSE)
						add_string_to_buf("  Summon failed.");
				break;
			case 121:
				add_string_to_buf("  Acid sprays from the tip!   ");
				start_spell_targeting(1068);
				break;
			case 122:
				add_string_to_buf("  It creates a cloud of gas.   ");
				start_spell_targeting(1066);
				break;
			case 123:
				add_string_to_buf("  It creates a shimmering cloud.   ");
				start_spell_targeting(1019);
				break;
			case 124:
				add_string_to_buf("  A green ray emerges.        ");
				start_spell_targeting(1030);
				break;
			case 125:
				add_string_to_buf("  Sparks fly.");
				start_spell_targeting(1044);
				break;
			case 126:
				add_string_to_buf("  It shoots a silvery beam.   ");
				start_spell_targeting(1069);
				break;
			case 127:
				add_string_to_buf("  It explodes!");
				start_spell_targeting(1065);
				break;
			case 128:
				add_string_to_buf("  It shoots a fiery red ray.   ");
				start_spell_targeting(1062);
				break;
			case 129:
				add_string_to_buf("Fire pours out!");
				make_quickfire(user_loc.x,user_loc.y);
				break;
			case 130: 
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
			case 131:
				if (c_town.town.specials2 % 10 == 1) {
					add_string_to_buf("  It doesn't work.");
					break;
					}
				add_string_to_buf("  You have a vision.            ");
				for (i = 0; i < town_size[town_type]; i++)
					for (j = 0; j < town_size[town_type]; j++)
						make_explored(i,j);
				clear_map();
				break;
			case 132:
				add_string_to_buf("  It fires a blinding ray.");
				add_string_to_buf("  Target spell.    ");
				overall_mode = 3;
				current_pat = s;
				set_town_spell(1041,current_pc);
			break;
			case 133:
				add_string_to_buf("  It shoots a blue sphere.   ");
				start_spell_targeting(1064);
				break;
			case 134:
				add_string_to_buf("  It fires a lovely, sparkling beam.");
				start_spell_targeting(1117);
				break;
			case 135:
				add_string_to_buf("  Your hair stands on end.   ");
				start_spell_targeting(1051);
				break;
			}
// Special spells:
//   62 - Carrunos
//	 63 - Summon Rat
//	 64 - Ice Wall Balls
//	 65 - Goo Bomb
//   66 - Foul Vapors			
//   67 - Sleep cloud
//	 68 - spray acid
//	 69 - paralyze
//   70 - mass sleep
		}
	
	put_pc_screen();
	if ((take_charge == TRUE) && (adven[pc].items[item].charges > 0))
		adven[pc].removeCharge(item);
	if (take_charge == FALSE) {
		draw_terrain(0);
		put_item_screen(stat_window,0);
		}
}

// Returns TRUE if an action is actually carried out. This can only be reached in town.
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
		return TRUE;
		}
	if (is_crate(where.x,where.y)) {
		to_loc = push_loc(from_loc,where);
		if (same_point(from_loc,to_loc) == TRUE) {
			add_string_to_buf("  Can't push crate.");
			return FALSE;
			}
		add_string_to_buf("  You push the crate.");
		take_crate((short) where.x,(short) where.y);
		make_crate((short) to_loc.x,(short) to_loc.y);
		for (i = 0; i < NUM_TOWN_ITEMS; i++)
			if ((t_i.items[i].variety > 0) && (same_point(t_i.items[i].item_loc,where))
			 && (t_i.items[i].isContained()))
			 	t_i.items[i].item_loc = to_loc;
		}
	if (is_barrel(where.x,where.y)) {
		to_loc = push_loc(from_loc,where);
		if (same_point(from_loc,to_loc) == TRUE) {
			add_string_to_buf("  Can't push barrel.");
			return FALSE;
			}
		add_string_to_buf("  You push the barrel.");
		take_barrel((short) where.x,(short) where.y);
		make_barrel((short) to_loc.x,(short) to_loc.y);
		for (i = 0; i < NUM_TOWN_ITEMS; i++)
			if ((t_i.items[i].variety > 0) && (same_point(t_i.items[i].item_loc,where))
			 && (t_i.items[i].isContained()))
			 	t_i.items[i].item_loc = to_loc;
		}
		
	switch (scenario.ter_types[ter].special) {
		case 22:
			if (same_point(where,from_loc) == TRUE) {
				add_string_to_buf("  Not while on space.");
				return FALSE;
				}
			add_string_to_buf("  OK.");
			alter_space(where.x,where.y,scenario.ter_types[ter].flag1);
			play_sound(scenario.ter_types[ter].flag2);
			return TRUE;
			break;
		case 23: // call special
			run_special(17,0,scenario.ter_types[ter].flag1,where,&i,&i,&i);
			return TRUE;
			break;	
		}
	add_string_to_buf("  Nothing to use.");

	return FALSE;
}

// Note ... if this is a container, the code must first process any specials. If
//specials return FALSE, can't get items inside. If TRUE, can get items inside.
// Can't get items out in combat.
Boolean adj_town_look(location where)
{
	unsigned char terrain;
	Boolean can_open = TRUE,item_there = FALSE,got_special = FALSE;
	short i = 0, s1 = 0, s2 = 0, s3 = 0;

	for (i = 0; i < NUM_TOWN_ITEMS; i++) 
		if ((t_i.items[i].variety > 0) && (t_i.items[i].isContained()) &&
			(same_point(where,t_i.items[i].item_loc) == TRUE))
				item_there = TRUE;

	terrain = t_d.terrain[where.x][where.y];
	if (special(where.x,where.y)) {// && (get_blockage(terrain) > 0)) {
		if (adjacent(c_town.p_loc,where) == FALSE)
			add_string_to_buf("  Not close enough to search.");
			else {
				for (i = 0; i < 50; i++)
					if (same_point(where,c_town.town.special_locs[i]) == TRUE) {
						run_special(4,2,c_town.town.spec_id[i],where,&s1,&s2,&s3);
						if (s1 > 0)	can_open = FALSE;
						got_special = TRUE;
						}
				put_item_screen(stat_window,0);	
				}
		}
	if ((is_container(where)) && (item_there == TRUE) && (can_open == TRUE)) 
		get_item(where,6,TRUE);
		
	else if (scenario.ter_types[terrain].special == 22 || scenario.ter_types[terrain].special == 23)
		{
			add_string_to_buf("  (Use this space to do something");
			add_string_to_buf("  with it.)");

        }
        
    else if (got_special == FALSE) add_string_to_buf("  Search: You don't find anything.          ");

	return FALSE;
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
		can_draw_pcs = FALSE;
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
	can_draw_pcs = TRUE;
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
	
	Boolean do_print = TRUE;
	char resist;

	if (c_town.monst.dudes[which_m].active == 0) return FALSE;
	
	sound_type = dam_type / 100;
	dam_type = dam_type % 100;

	if (dam_type >= 10) {
		do_print = FALSE;
		dam_type -= 10;
		}

	if (sound_type == 0) {
		if ((dam_type == 1) || (dam_type == 4) )
			sound_type = 5;		
		if 	(dam_type == 5)
			sound_type = 7;	
		if 	(dam_type == 3)
			sound_type = 12;	
		if 	(dam_type == 2)
			sound_type = 11;	
		}

		
	victim = &c_town.monst.dudes[which_m];	
	resist = victim->m_d.immunities;

	if (dam_type == 3) {
		if (resist & 1)
			how_much = how_much / 2;
		if (resist & 2)
			how_much = 0;		
		}
	if (dam_type == 1) {
		if (resist & 4)
			how_much = how_much / 2;
		if (resist & 8)
			how_much = 0;		
		}
	if (dam_type == 5) {
		if (resist & 16)
			how_much = how_much / 2;
		if (resist & 32)
			how_much = 0;		
		}
	if (dam_type == 2) {
		if (resist & 64)
			how_much = how_much / 2;
		if (resist & 128)
			how_much = 0;		
		}
	
	// Absorb damage?
	if (((dam_type == 1) || (dam_type == 3) || (dam_type == 5))
	 && (victim->m_d.spec_skill == 26)) {
        if((victim->m_d.health + how_much) < 32767)
    		victim->m_d.health += how_much;
		ASB("  Magic absorbed.");
		return FALSE;
		}
		
	// Saving throw
	if (((dam_type == 1) || (dam_type == 5)) && (get_ran(1,0,20) <= victim->m_d.level))
		how_much = how_much / 2;
	if ((dam_type == 3) && (get_ran(1,0,24) <= victim->m_d.level))
		how_much = how_much / 2;

	// Rentar-Ihrno?
	if (victim->m_d.spec_skill == 36)
		how_much = how_much / 10;
	
	r1 = get_ran(1,0,(victim->m_d.armor * 5) / 4);
	r1 += victim->m_d.level / 4;
	if (dam_type == 0)
		how_much -= r1;
		
	if (boom_anim_active == TRUE) {
		if (how_much < 0)
			how_much = 0;
		monst_marked_damage[which_m] += how_much;
		if (victim->m_d.spec_skill == 36)
		add_explosion(victim->m_loc,how_much/10,0,(dam_type > 2) ? 2 : 0,14 * (victim->m_d.x_width - 1),18 * (victim->m_d.y_width - 1));
		else
		add_explosion(victim->m_loc,how_much,0,(dam_type > 2) ? 2 : 0,14 * (victim->m_d.x_width - 1),18 * (victim->m_d.y_width - 1));
		if (how_much == 0){
			monst_spell_note(victim->number,7);
			return FALSE;
            }
			else return TRUE;
		}

	if (how_much <= 0) {
		if (is_combat())
			monst_spell_note(victim->number,7);
		if ((how_much <= 0) && ((dam_type == 0) || (dam_type == 6) || (dam_type == 7))) {
			draw_terrain(2);
			play_sound(2);
			}

		return FALSE;	
		}

	if (do_print == TRUE)
		monst_damaged_mes(which_m,how_much,how_much_spec);
	victim->m_d.health = victim->m_d.health - how_much - how_much_spec;

	if (in_scen_debug == TRUE)
		victim->m_d.health = -1;	
	// splitting monsters
	if ((victim->m_d.spec_skill == 12) && (victim->m_d.health > 0)){
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
		if (party_can_see_monst(which_m) == TRUE) {
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
	 (processing_fields == FALSE) && (monsters_going == FALSE)) {
		add_string_to_buf("Damaged an innocent.           ");
		victim->attitude = 1;
		make_town_hostile();
		}
	if ((victim->attitude % 2 != 1) && (who_hit < 7) && 
	 ((processing_fields == TRUE) && (party.stuff_done[305][9] == 0))) {
		add_string_to_buf("Damaged an innocent.");
		victim->attitude = 1;
		make_town_hostile();
		}
		 
	return TRUE;
}

void kill_monst(creature_data_type *which_m,short who_killed)
{
	short xp,i,j,s1,s2,s3;	
	location l;
	
	switch (which_m->m_d.m_type) {
		case 0: case 3: case 4: case 5: case 6:  
			if (( which_m->number == 38) || 
				( which_m->number == 39))
				i = 4;
				else if ( which_m->number == 45)
					i = 0;
					else i = get_ran(1,0,1); 
			play_sound(29 + i); break;
		 case 9: play_sound(29); break;
		 case 1: case 2: case 7: case 8: case 11:
			i = get_ran(1,0,1); play_sound(31 + i); break;
		default: play_sound(33); break;
		}
	
	// Special killing effects
	if (sd_legit(which_m->monst_start.spec1,which_m->monst_start.spec2) == TRUE)
		party.stuff_done[which_m->monst_start.spec1][which_m->monst_start.spec2] = 1;
		
	run_special(12,2,which_m->monst_start.special_on_kill,which_m->m_loc,&s1,&s2,&s3);
	if (which_m->m_d.radiate_1 == 15)
		run_special(12,0,which_m->m_d.radiate_2,which_m->m_loc,&s1,&s2,&s3);
	
	if ((in_scen_debug == FALSE) && ((which_m->summoned >= 100) || (which_m->summoned == 0))) { // no xp for party-summoned monsters
		xp = which_m->m_d.level * 2;
		if (who_killed < NUM_OF_PCS)
			adven[who_killed].giveXP(xp);
			else if (who_killed == INVALID_PC)
				adven.giveXP(xp / 6 + 1);
		if (who_killed < 7) {
			i = max((xp / 6),1);
			adven.giveXP(i);
			}
		l = which_m->m_loc;
		place_glands(l,which_m->number);
		
		}
	if ((in_scen_debug == FALSE) && (which_m->summoned == 0))
		place_treasure(which_m->m_loc, which_m->m_d.level / 2, which_m->m_d.treasure, 0);
	
	i = which_m->m_loc.x;
	j = which_m->m_loc.y;
	switch (which_m->m_d.m_type) {
		case 7:	make_sfx(i,j,6); break;
		case 8:	if (which_m->number <= 59) make_sfx(i,j,7); break;
		case 10: case 12: make_sfx(i,j,4); break;
		case 11: make_sfx(i,j,8); break;
		default: make_sfx(i,j,1); break;
		}
	


	if (((is_town()) || (which_combat_type == 1)) && (which_m->summoned == 0)) {
		party.m_killed[c_town.town_num]++;
		}

	party.total_m_killed++;
	
	which_m->monst_start.spec1 = 0; // make sure, if this is a spec. activated monster, it won't come back

	which_m->active = 0;
}

// Pushes party and monsters around by moving walls and conveyor belts. 
//This is very fragils, and only hands a few cases. 
void push_things()
{
	Boolean redraw = FALSE;
	short i,k;
	unsigned char ter;
	location l;
	
	if (is_out())
		return;
	if (belt_present == FALSE)
		return;
	
	for (i = 0; i < T_M; i++)
		if (c_town.monst.dudes[i].active > 0) {
			l = c_town.monst.dudes[i].m_loc;
			ter = t_d.terrain[l.x][l.y];
			switch (scenario.ter_types[ter].special) {
				case 16: l.y--; break;
				case 17: l.x++; break;
				case 18: l.y++; break;
				case 19: l.x--; break;
				}
			if (same_point(l,c_town.monst.dudes[i].m_loc) == FALSE) {
				c_town.monst.dudes[i].m_loc = l;
				if ((point_onscreen(center,c_town.monst.dudes[i].m_loc) == TRUE) || 
					(point_onscreen(center,l) == TRUE))
						redraw = TRUE;
				}
			}
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (t_i.items[i].variety > 0) {
			l = t_i.items[i].item_loc;
			ter = t_d.terrain[l.x][l.y];
			switch (scenario.ter_types[ter].special) {
				case 16: l.y--; break;
				case 17: l.x++; break;
				case 18: l.y++; break;
				case 19: l.x--; break;
				}
			if (same_point(l,t_i.items[i].item_loc) == FALSE) {
				t_i.items[i].item_loc = l;
				if ((point_onscreen(center,t_i.items[i].item_loc) == TRUE) || 
					(point_onscreen(center,l) == TRUE))
						redraw = TRUE;
				}
			}
	
	if (is_town()) {
		ter = t_d.terrain[c_town.p_loc.x][c_town.p_loc.y];
		l = c_town.p_loc;
		switch (scenario.ter_types[ter].special) {
			case 16: l.y--; break;
			case 17: l.x++; break;
			case 18: l.y++; break;
			case 19: l.x--; break;
			}
		if (same_point(l,c_town.p_loc) == FALSE) {
			ASB("You get pushed.");
			if (scenario.ter_types[ter].special >= 16)	
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
				if ((t_i.items[k].variety > 0) && (t_i.items[k].isContained())
				&& (same_point(t_i.items[k].item_loc,c_town.p_loc) == TRUE))
					t_i.items[k].item_properties = t_i.items[k].item_properties & 247;				
			redraw = TRUE;
			}	
		}
	if (is_combat()) {
		for (i = 0; i < 6; i++)
			if (adven[i].isAlive()) {
				ter = t_d.terrain[pc_pos[i].x][pc_pos[i].y];
				l = pc_pos[i];
				switch (scenario.ter_types[ter].special) {
					case 16: l.y--; break;
					case 17: l.x++; break;
					case 18: l.y++; break;
					case 19: l.x--; break;
					}
				if (same_point(l, pc_pos[i]) == FALSE) {
					ASB("Someone gets pushed.");	
					ter = t_d.terrain[l.x][l.y];
					if (scenario.ter_types[ter].special >= 16)	
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
						if ((t_i.items[k].variety > 0) && (t_i.items[k].isContained())
						&& (same_point(t_i.items[k].item_loc,pc_pos[i]) == TRUE))
							t_i.items[k].item_properties = t_i.items[k].item_properties & 247;		
					redraw = TRUE;
					}				
				}
		}
	if (redraw == TRUE) {
		print_buf();
		draw_terrain(0);
		}
}

BOOL special_increase_age()
{
	short i,s1,s2,s3;
	Boolean redraw = FALSE,stat_area = FALSE;
	BOOL to_return = FALSE;
	
	for (i = 0; i < 8; i++)
		if ((c_town.town.timer_spec_times[i] > 0) && (party.age % c_town.town.timer_spec_times[i] == 0)
			&& ((is_town() == TRUE) || ((is_combat() == TRUE) && (which_combat_type == 1)))) {
			run_special(9,2,c_town.town.timer_specs[i],location(),&s1,&s2,&s3);
			stat_area = TRUE;
			if (s3 > 0)
				redraw = TRUE;
			to_return = TRUE;
			}
	for (i = 0; i < 20; i++)
		if ((scenario.scenario_timer_times[i] > 0) && (party.age % scenario.scenario_timer_times[i] == 0)) {
			run_special(10,0,scenario.scenario_timer_specs[i],location(),&s1,&s2,&s3);
			stat_area = TRUE;
			if (s3 > 0)
				redraw = TRUE;
			to_return = TRUE;				
			}
	for (i = 0; i < 30; i++)
		if (party.party_event_timers[i] > 0) {
			if (party.party_event_timers[i] == 1) {
				if (party.global_or_town[i] == 0)
					run_special(11,0,party.node_to_call[i],location(),&s1,&s2,&s3);
					else run_special(11,2,party.node_to_call[i],location(),&s1,&s2,&s3);
				party.party_event_timers[i] = 0;
				stat_area = TRUE;
				if (s3 > 0)
					redraw = TRUE;
    			to_return = TRUE;
				}
				else party.party_event_timers[i]--;
			}
	if (stat_area == TRUE) {
		put_pc_screen();
		put_item_screen(stat_window,0);
		}
	if (redraw == TRUE)
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
	
	if (special_in_progress == TRUE) {
		give_error("The scenario called a special node while processing another special encounter. The second special will be ignored.","",0);
		return;
		}
		
	special_in_progress = TRUE;
	next_spec = start_spec;
	next_spec_type = which_type;
	current_pc_picked_in_spec_enc = -1;
	store_special_loc = spec_loc;
	if (end_scenario == TRUE) {
		special_in_progress = FALSE;
		return;
		}
		
	while (next_spec >= 0) {
	
		cur_spec = next_spec;
		cur_spec_type = next_spec_type;
		next_spec = -1;
		cur_node = get_node(cur_spec,cur_spec_type);

		if (cur_node.type == -1) { /// got an error 
			special_in_progress = FALSE;
			return;
			}
		if ((cur_node.type >= 0) && (cur_node.type <= 27)) {
			general_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= 50) && (cur_node.type <= 63)) {
			oneshot_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= 80) && (cur_node.type <= 106)) {
			affect_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= 130) && (cur_node.type <= 155)) {
			ifthen_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= 170) && (cur_node.type <= 195)) {
			townmode_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= 200) && (cur_node.type <= 218)) {
			rect_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
		if ((cur_node.type >= 225) && (cur_node.type <= 229)) {
			outdoor_spec(which_mode,cur_node,cur_spec_type,&next_spec,&next_spec_type,a,b,redraw);
			}
			
//	if(((GetAsyncKeyState(VK_LCONTROL) & 32768) == 32768) && ((GetAsyncKeyState(0x43) & 32768) == 32768))  {//Ctrl-C escape sequence check
    if(check_for_interrupt()){
        give_error("The special encounter was interrupted. The scenario may be in an unexpected state; it is recommended that you reload from a saved game.","",0);
// 	    give_error("A special encounter can be at most 50 nodes long. The 50th node was just processed. The encounter will now end.","",0);
		next_spec = -1;
		}
	}
	if (is_out())
		erase_out_specials();
		else erase_specials();
	special_in_progress = FALSE;
}

special_node_type get_node(short cur_spec,short cur_spec_type)
{
	special_node_type dummy_node;
	
	dummy_node = scenario.scen_specials[0];
	dummy_node.type = -1;
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
	Boolean check_mess = FALSE;
	char str1[256] = "",str2[256] = "";
	short store_val = 0,i;
	special_node_type spec;
	short mess_adj[3] = {160,10,20};
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	switch (cur_node.type) {
		case 0: break; // null spec
		case 1:
			check_mess = TRUE; setsd(cur_node.sd1,cur_node.sd2,cur_node.ex1a);
			break;	
		case 2:
			check_mess = TRUE;
			setsd(cur_node.sd1,cur_node.sd2,
				PSD[cur_node.sd1][cur_node.sd2] + ((cur_node.ex1b == 0) ? 1 : -1) * cur_node.ex1a);
			break;	
		case 3:
			check_mess = TRUE;break;	
		case 5:
			get_strs((char *) str1,(char *) str2, cur_spec_type,cur_node.m1 + mess_adj[cur_spec_type],
				cur_node.m2 + mess_adj[cur_spec_type]);
			if (cur_node.m1 >= 0)
				ASB((char *) str1);
			if (cur_node.m2 >= 0)
				ASB((char *) str2);
			break;	
		case 6:
			setsd(cur_node.sd1,cur_node.sd2,
				((PSD[cur_node.sd1][cur_node.sd2] == 0) ? 1 : 0) );
			check_mess = TRUE;break;	
		case 7:
			if (is_out()) *next_spec = -1;
			if ((is_out()) && (spec.ex1a != 0) && (which_mode == 0)) {
				ASB("Can't go here while outdoors.");
				*a = 1;
				}
			break;	
		case 8:
			if (is_town()) *next_spec = -1;
			if ((is_town()) && (spec.ex1a != 0) && (which_mode == 1)) {
				ASB("Can't go here while in town mode.");
				*a = 1;
				}
			break;	
		case 9:
			if (is_combat()) *next_spec = -1;
			if ((is_combat()) && (spec.ex1a != 0) && (which_mode == 2)) {
				ASB("Can't go here during combat.");
				*a = 1;
				}
			break;	
		case 10:
			if ((which_mode == 3) || (which_mode == 4)) *next_spec = -1;
			break;
		case 11:
			check_mess = TRUE;
			if (which_mode < 3) {
				if (spec.ex1a != 0)
					*a = 1;
					else *a = 0;
				}
			break;
		case 12:
			check_mess = TRUE; party.age += spec.ex1a;
			break;
		case 13:
			check_mess = TRUE;
			for (i = 0; i < 30; i++)
	 			if (party.party_event_timers[i] == 0) {
	 				party.party_event_timers[i] = spec.ex1a;
	 				party.node_to_call[i] = spec.ex1b;
	 				party.global_or_town[i] = 0;
	 				i = 30;
	 				}
			break;
		case 14:
			play_sound(1000 + spec.ex1a);
			break;
		case 15:
			check_mess = TRUE;
			if (spec.ex1a != minmax(0,29,(int)spec.ex1a))
				give_error("Horse out of range.","",0);
				else party.horses[spec.ex1a].property = (spec.ex2a == 0) ? 1 : 0;
			break;
		case 16:
			check_mess = TRUE;
			if (spec.ex1a != minmax(0,29,(int)spec.ex1a))
				give_error("Boat out of range.","",0);
				else party.boats[spec.ex1a].property = (spec.ex2a == 0) ? 1 : 0;
			break;
		case 17:
			check_mess = TRUE;
			if (spec.ex1a != minmax(0,scenario.num_towns - 1,(int)spec.ex1a))
				give_error("Town out of range.","",0);
				else party.can_find_town[spec.ex1a] = (spec.ex1b == 0) ? 0 : 1;
			*redraw = TRUE;
			break;
		case 18:
			check_mess = TRUE;
			if (spec.ex1a != minmax(1,10,(int)spec.ex1a))
				give_error("Event code out of range.","",0);
				else if (party.key_times[spec.ex1a] == 30000)
					party.key_times[spec.ex1a] = calc_day();
			break;
		case 19:
			check_mess = TRUE;
			if ((forced_give(spec.ex1a,0) == FALSE) && ( spec.ex1b >= 0))
				*next_spec = spec.ex1b;
			break;
		case 20:
			for (i = 0; i < 144; i++)
				if (adven.checkClass(spec.ex1a,0) == true)
					store_val++;
			if (store_val == 0) {
				if ( spec.ex1b >= 0)
					*next_spec = spec.ex1b;
				}
				else {
					check_mess = TRUE;
					party.giveGold(store_val * spec.ex2a,true);
					}
			break;
		case 21:
			*next_spec_type = 0;
			break;
		case 22:
			if (spec.sd1 != minmax(0,299,(int)spec.sd1))
				give_error("Stuff Done flag out of range.","",0);
				else for (i = 0; i < 10; i++) PSD[spec.sd1][i] = spec.ex1a;
			break;
		case 23:
			if ((sd_legit(spec.sd1,spec.sd2) == FALSE) || (sd_legit(spec.ex1a,spec.ex1b) == FALSE))
				give_error("Stuff Done flag out of range.","",0);
				else PSD[spec.sd1][spec.sd2] = PSD[spec.ex1a][spec.ex1b];
			break;
		case 24:
			if (which_mode != 16)
				*next_spec = spec.ex1b;
			break;
		case 25:
					check_mess = TRUE;
					party.age += spec.ex1a;
					adven.heal(spec.ex1b);
					adven.restoreSP(spec.ex1b);
			break;
		case 26:
			if (which_mode != 13)
				break;
			*a = (spec.ex1a == 0) ? 1 : 0;
			break;
		case 27:
        char count=0;//temporary fix, maybe we'll do a special "you lose" dialog.
            for(i=0;i<6;i++)
                if(adven[i].isAlive() == FALSE)
                    count++;
                
            if(count != 6)                    
			 end_scenario = TRUE;
			break;
		}
	if (check_mess == TRUE) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
}


void oneshot_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = TRUE,set_sd = TRUE;
	char strs[6][256] = {"","","","","",""};
	short i,j,buttons[3] = {-1,-1,-1};
	special_node_type spec;
	item_record_type store_i;
	location l;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	if ((sd_legit(spec.sd1,spec.sd2) == TRUE) && (PSD[spec.sd1][spec.sd2] == 250)) {
		*next_spec = -1;
		return;
		}
	switch (cur_node.type) {
		case 50:
			if (forced_give(spec.ex1a,0) == FALSE) {
				set_sd = FALSE;
				if ( spec.ex2b >= 0)
					*next_spec = spec.ex2b;
				}
				else {
					party.giveGold(spec.ex1b,true);
					party.giveFood(spec.ex2a,true);
					}
			break;
		case 51:
			if (spec.ex1a != minmax(0,49,(int)spec.ex1a)) {
				give_error("Special item is out of range.","",0);
				set_sd = FALSE;
				}
				else {
					party.spec_items[spec.ex1a] = (spec.ex1b == 0) ? 1 : 0;
					}
			if (stat_window == 6)
				set_stat_window(6);
			*redraw = 1;
			break;
		case 52:
			set_sd = FALSE;
			check_mess = FALSE;
			break;
		case 53:
			check_mess = FALSE;
			break;
		case 55: case 56: case 57:
			check_mess = FALSE;
			if (spec.m1 < 0)
				break;
			for (i = 0; i < 3; i++)
				get_strs((char *) strs[i * 2],(char *) strs[i * 2 + 1],cur_spec_type,
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
				case 55: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2400,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic ,buttons) ; break;
				case 56: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2000,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic,buttons) ; break;
				case 57: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2000,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic ,buttons) ; break;
				}
			if (spec.m2 > 0) {
				if (i == 1) {
					if ((spec.ex1a >= 0) || (spec.ex2a >= 0)) {
						set_sd = FALSE; 
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
		case 58: case 59: case 60:
			check_mess = FALSE;
			if (spec.m1 < 0)
				break;
			for (i = 0; i < 3; i++)
				get_strs((char *) strs[i * 2],(char *) strs[i * 2 + 1],cur_spec_type,
					spec.m1 + i * 2 + spec_str_offset[cur_spec_type],spec.m1 + i * 2 + 1  + spec_str_offset[cur_spec_type]);
			buttons[0] = 20; buttons[1] = 19;
			switch (cur_node.type) {
				case 58: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2400,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic,buttons) ; break;
				case 59: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2000,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic,buttons) ; break;
				case 60: if (spec.pic >= 1000) i = custom_choice_dialog((char *) strs,(spec.pic % 1000) + 2000,buttons) ;
					else i = custom_choice_dialog((char *) strs,spec.pic,buttons) ; break;
				}
			if (i == 1) {set_sd = FALSE; *next_spec = -1;}
				else {
					store_i = get_stored_item(spec.ex1a);
					if ((spec.ex1a >= 0) && (give_to_party(store_i,TRUE) == FALSE)) {
						set_sd = FALSE; *next_spec = -1;
						}
						else {
							party.giveGold(spec.ex1b,true);
							party.giveFood(spec.ex2a,true);
							if ((spec.m2 >= 0) && (spec.m2 < 50)) {
								if (party.spec_items[spec.m2] == 0)
									ASB("You get a special item.");
								party.spec_items[spec.m2] = 1;
								*redraw = TRUE;
								if (stat_window == 6)
									set_stat_window(6);
								}
							if (spec.ex2b >= 0) *next_spec = spec.ex2b;
							}
					}
			break;
		case 61:
			if (spec.ex1a != minmax(0,3,(int)spec.ex1a)) {
				give_error("Special outdoor enc. is out of range. Must be 0-3.","",0);
				set_sd = FALSE;
				}
				else {
					l = party.p_loc.toLocal();
					place_outd_wand_monst(l,
					outdoors[party.i_w_c.x][party.i_w_c.y].special_enc[spec.ex1a],TRUE);			
					}
			break;
		case 62:
             c_town.activateMonsters(spec.ex1a);
			break;
		case 63:
			check_mess = FALSE;
			if ((spec.m1 >= 0) || (spec.m2 >= 0)) {
				get_strs((char *) strs[0],(char *) strs[1],
					cur_spec_type,
					spec.m1 + ((spec.m1 >= 0) ? spec_str_offset[cur_spec_type] : 0),
					spec.m2 + ((spec.m2 >= 0) ? spec_str_offset[cur_spec_type] : 0));
				buttons[0] = 3; buttons[1] = 2;
				i = custom_choice_dialog((char *) strs,727,buttons);
				}
				else i = FCD(872,0); 
			if (i == 1) {set_sd = FALSE; *next_spec = -1; *a = 1;}
				else {
					if (is_combat() == TRUE)
						j = adven[current_pc].runTrap(spec.ex1a,spec.ex1b,spec.ex2a);
					else
					{
						short tmp = select_pc(1,0);
						
						if (tmp == INVALID_PC) j = FALSE;
						else j = adven[tmp].runTrap(spec.ex1a,spec.ex1b,spec.ex2a);
					}
					
					if (j == 0)
					{
						*a = 1;
						set_sd = FALSE;
					}
					}
			break;
		}
	if (check_mess == TRUE) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
	if ((set_sd == TRUE) && (sd_legit(spec.sd1,spec.sd2) == TRUE)) 
		PSD[spec.sd1][spec.sd2] = 250;

}

void affect_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = TRUE;
	short i,pc,r1;
	special_node_type spec;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	if((PSD[SDF_IS_PARTY_SPLIT] > 0) && (cur_node.type != SPEC_AFFECT_DEADNESS))
        pc = PSD[SDF_PARTY_SPLIT_PC];
    else pc = current_pc_picked_in_spec_enc;



	if ((check_mess == TRUE) && (cur_node.type != 80)) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}


	switch (cur_node.type) {
		case 80:
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
		case 81:
			r1 = get_ran(spec.ex1a,1,spec.ex1b) + spec.ex2a;
			if (pc < 0) {
                if(spec.pic == 1 && overall_mode == MODE_COMBAT)
                    adven[current_pc].damage(r1,spec.ex2b,0);
                else
				    adven.damage(r1,spec.ex2b);
				}
				else adven[pc].damage(r1,spec.ex2b,0);
			break;
		case 82:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					adven[i].cur_health = minmax(0,	(int)adven[i].max_health,
						adven[i].cur_health + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case 83:
			for (i = 0; i < 6; i++){
				if ((pc < 0) || (pc == i))
				   adven[i].cur_sp = minmax(0,	(int)adven[i].max_sp,
                        adven[i].cur_sp + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
                }
			break;
		case 84:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0) adven[i].giveXP(spec.ex1a);
					else adven[i].drainXP(spec.ex1a);
					}
			break;
		case 85:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					adven[i].skill_pts = minmax(0,	100,
						adven[i].skill_pts + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case 86:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0) {
						if ((adven[i].main_status > MAIN_STATUS_ABSENT) && (adven[i].main_status < MAIN_STATUS_SPLIT))
							adven[i].main_status=MAIN_STATUS_ALIVE;
						}
						else 
                        if(party.stuff_done[309][1] == 0){//legacy behavior
                         adven[i].kill(spec.ex1a + 2 + 10);
                            }
                        else{//kill only present pc
                        if ((adven[i].main_status > MAIN_STATUS_ABSENT) && (adven[i].main_status < MAIN_STATUS_SPLIT))
                            adven[i].kill(spec.ex1a + 2 + 10);
                        }

					}
			*redraw = 1;
			break;
		case 87:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0)
						adven[i].cure(spec.ex1a);
					else
						adven[i].poison(spec.ex1a);
					}
			break;
		case 88:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0) {
						adven[i].slow(-1 * spec.ex1a);
						}
						else adven[i].slow(spec.ex1a);
					}
			break;
		case 89:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,4,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case 90:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,5,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case 91:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,6,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case 92:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,7,spec.ex1a * ((spec.ex1b != 0) ? 1: -1));
			break;
		case 93:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,8,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case 94:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,1,spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case 95:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i))
					affect_pc(i,9,spec.ex1a * ((spec.ex1b == 0) ? -1: 1));
			break;
		case 96:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0) {
						affect_pc(i,11,-1 * spec.ex1a);
						}
						else adven[i].sleep(spec.ex1a,11,10);
					}
			break;
		case 97:
			for (i = 0; i < 6; i++)
				if ((pc < 0) || (pc == i)) {
					if (spec.ex1b == 0) {
						affect_pc(i,12,-1 * spec.ex1a);
						}
						else adven[i].sleep(spec.ex1a,12,10);
					}
			break;
		case 98:
			if (spec.ex2a != minmax(0,18,(int)spec.ex2a)) {
				give_error("Skill is out of range.","",0);
				break;
				}
			for (i = 0; i < 6; i++)
				if (((pc < 0) || (pc == i)) && (get_ran(1,1,100) <= spec.pic))
					adven[i].skills[spec.ex2a] = minmax(0, (int)skill_max[spec.ex2a],
						adven[i].skills[spec.ex2a] + spec.ex1a * ((spec.ex1b != 0) ? -1: 1));
			break;
		case 99:
			if (spec.ex1a != minmax(0,31,(int)spec.ex1a)) {
				give_error("Mage spell is out of range (0 - 31). See docs.","",0);
				break;
				}
			for (i = 0; i < 6; i++)
                if ((pc < 0) || (pc == i)) 
					adven[i].mage_spells[spec.ex1a + 30] = TRUE;
			break;
		case 100:
			if (spec.ex1a != minmax(0,31,(int)spec.ex1a)) {
				give_error("Priest spell is out of range (0 - 31). See docs.","",0);
				break;
				}
			for (i = 0; i < 6; i++)
                if ((pc < 0) || (pc == i)) 
					adven[i].priest_spells[spec.ex1a + 30] = TRUE;
			break;
		case 101:
			if (spec.ex1b == 0)
				party.giveGold(spec.ex1a,true);
				else if (party.gold < spec.ex1a)
					party.gold = 0;
					else party.takeGold(spec.ex1a,false);
			break;
		case 102:
			if (spec.ex1b == 0)
				party.giveFood(spec.ex1a,true);
				else if (party.food < spec.ex1a)
					party.food = 0;
					else party.takeFood(spec.ex1a,false);
			break;
		case 103:
			if (spec.ex1a != minmax(0,19,(int)spec.ex1a)) {
				give_error("Alchemy is out of range.","",0);
				break;
				}
			party.alchemy[spec.ex1a] = TRUE;
			break;
		case 104:
			r1 = (short) party.stuff_done[305][0];
			r1 = minmax(0,250,r1 + spec.ex1a);
			party.stuff_done[305][0] = r1;
			break;
		case 105:
			r1 = (short) party.stuff_done[305][3];
			r1 = minmax(0,250,r1 + spec.ex1a);
			party.stuff_done[305][3] = r1;
			break;
		case 106:
			if (party.in_boat >= 0)
				add_string_to_buf("  Can't fly when on a boat. "); 
			else if (party.in_horse >= 0)////
				add_string_to_buf("  Can't fly when on a horse.  "); 
			else {
				r1 = (short) party.stuff_done[305][1];
				r1 = minmax(0,250,r1 + spec.ex1a);
				party.stuff_done[305][1] = r1;
				}
			break;
		}
}

void ifthen_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = FALSE;
	char str1[256] = "",str2[256] = "",str3[256] = "";
	short i,j,k;
	special_node_type spec;
	location l;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	switch (cur_node.type) {
		case 130:
			if (sd_legit(spec.sd1,spec.sd2) == TRUE) {
				if ((spec.ex1a >= 0) && (PSD[spec.sd1][spec.sd2] >= spec.ex1a))
					*next_spec = spec.ex1b;
					else if ((spec.ex2a >= 0) && (PSD[spec.sd1][spec.sd2] < spec.ex2a))
						*next_spec = spec.ex2b;
				}
			break;
		case 131:
			if (((is_town()) || (is_combat())) && (c_town.town_num == spec.ex1a))
				*next_spec = spec.ex1b;
			break;
		case 132:
			if (get_ran(1,1,100) <= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case 133:
			if (spec.ex1a != minmax(0,49,(int)spec.ex1a)) {
				give_error("Special item is out of range.","",0);		
				}
				else if (party.spec_items[spec.ex1a] > 0)
					*next_spec = spec.ex1b;
			break;
		case 134:
			if ((sd_legit(spec.sd1,spec.sd2) == TRUE) && (sd_legit(spec.ex1a,spec.ex1b) == TRUE)) {
				if (PSD[spec.ex1a][spec.ex1b] < PSD[spec.sd1][spec.sd2])
					*next_spec = spec.ex2b;
				}
				else give_error("A Stuff Done flag is out of range.","",0);	
			break;
		case 135:
			if (((is_town()) || (is_combat())) && (t_d.terrain[spec.ex1a][spec.ex1b] == spec.ex2a))
				*next_spec = spec.ex2b;
			break;
		case 136:
			l.x = spec.ex1a; l.y = spec.ex1b;
			l = l.toGlobal();
			if ((is_out()) && (out[l.x][l.y] == spec.ex2a))
				*next_spec = spec.ex2b;
 			break;
		case 137:
			if (party.gold >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case 138:
			if (party.food >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case 139:
			if (is_out())
				break;
			l.x = spec.ex1a; l.y = spec.ex1b;
			for (i = 0; i < NUM_TOWN_ITEMS; i++)
				if ((t_i.items[i].variety > 0) && (t_i.items[i].special_class == spec.ex2a)
					&& (same_point(l,t_i.items[i].item_loc) == TRUE))
						*next_spec = spec.ex2b;
			break;
		case 140:
			if (adven.checkClass(spec.ex1a,1) == true)
				*next_spec = spec.ex1b;
			break;
		case 141:
			for (i = 0; i < 6; i++)
				if (adven[i].isAlive())
					for (j = 0; j < 24; j++)
						if ((adven[i].items[j].variety > 0) && (adven[i].items[j].special_class == spec.ex1a)
							&& (adven[i].equip[j] == TRUE)) 
							*next_spec = spec.ex1b;			
			break;
		case 142:
			if (party.gold >= spec.ex1a) {
				party.takeGold(spec.ex1a,true);
				*next_spec = spec.ex1b;
				}
			break;
		case 143:
			if (party.food >= spec.ex1a) {
				party.takeFood(spec.ex1a,true);
				*next_spec = spec.ex1b;
				}
			break;
		case 144:
			if (is_out())
				break;
			l.x = spec.ex1a; l.y = spec.ex1b;
			for (i = 0; i < NUM_TOWN_ITEMS; i++)
				if ((t_i.items[i].variety > 0) && (t_i.items[i].special_class == spec.ex2a)
					&& (same_point(l,t_i.items[i].item_loc) == TRUE)) {
						*next_spec = spec.ex2b;
						*redraw = 1;
						t_i.items[i].variety = 0;
						}
			break;
		case 145:
			if (adven.checkClass(spec.ex1a,0) == true)
				*next_spec = spec.ex1b;
			break;
		case 146:
			for (i = 0; i < 6; i++)
				if (adven[i].isAlive())
					for (j = 0; j < 24; j++)
						if ((adven[i].items[j].variety > 0) && (adven[i].items[j].special_class == spec.ex1a)
							&& (adven[i].equip[j] == TRUE)) {
							*next_spec = spec.ex1b;			
							*redraw = 1;
							adven[i].takeItem(j);
							}
			break;
		case 147:
			if (calc_day() >= spec.ex1a)
				*next_spec = spec.ex1b;
			break;
		case 148:
			for (j = 0; j < town_size[town_type]; j++)
				for (k = 0; k < town_size[town_type]; k++) 
					if (is_barrel(j,k))
						*next_spec = spec.ex1b;								
			break;
		case 149:
			for (j = 0; j < town_size[town_type]; j++)
				for (k = 0; k < town_size[town_type]; k++) 
					if (is_crate(j,k))
						*next_spec = spec.ex1b;								
			break;
		case 150:
			if (day_reached(spec.ex1a,spec.ex1b) == TRUE)
				*next_spec = spec.ex2b;
			break;
		case 151:
			for (i = 0; i < 6; i++)
				if ((adven[i].isAlive()) && (adven[i].traits[TRAIT_CAVE_LORE] > 0))
					*next_spec = spec.ex1b;
			break;
		case 152:
			for (i = 0; i < 6; i++)
				if ((adven[i].isAlive()) && (adven[i].traits[5] > 0))
					*next_spec = spec.ex1b;
			break;
		case 153:
			if (adven.getMageLore() >= spec.ex1a)
					*next_spec = spec.ex1b;
			break;
		case 154: // text response
			check_mess = FALSE;
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
		case 155:
			if (sd_legit(spec.sd1,spec.sd2) == TRUE) {
				if (PSD[spec.sd1][spec.sd2] == spec.ex1a)
					*next_spec = spec.ex1b;
				}
			break;
		}
	if (check_mess == TRUE) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
}

void townmode_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = TRUE;
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
		case 170:
			make_town_hostile();
			break;
		case 171:
			set_terrain(l,spec.ex2a);
            if(scenario.ter_types[spec.ex2a].special >= 16 && scenario.ter_types[spec.ex2a].special <=19)
                belt_present = TRUE;
            if(party.stuff_done[308][0] == 0)
                draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			*redraw = TRUE;
			break;
		case 172:
			if (coord_to_ter(spec.ex1a,spec.ex1b) == spec.ex2a){
                set_terrain(l,spec.ex2b);                
                if(scenario.ter_types[spec.ex2b].special >= 16 && scenario.ter_types[spec.ex2b].special <=19)
                    belt_present = TRUE;
            }
			else if (coord_to_ter(spec.ex1a,spec.ex1b) == spec.ex2b){
    			set_terrain(l,spec.ex2a);
                if(scenario.ter_types[spec.ex2a].special >= 16 && scenario.ter_types[spec.ex2a].special <=19)
                    belt_present = TRUE;
            }
			*redraw = 1;
            if(party.stuff_done[308][0] == 0)
            draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		case 173:
			ter = coord_to_ter(spec.ex1a,spec.ex1b);
			set_terrain(l,scenario.ter_types[ter].trans_to_what);
			if(scenario.ter_types[scenario.ter_types[ter].trans_to_what].special >= 16 && scenario.ter_types[scenario.ter_types[ter].trans_to_what].special <=19)
                belt_present = TRUE;
			*redraw = 1;
            if(party.stuff_done[308][0] == 0)
            draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		case 174:
			if (is_combat()) {
				ASB("Not while in combat.");
				if (which_mode < 3)
					*a = 1;
				*next_spec = -1;
				check_mess = FALSE;
				}
				else { // 1 no
					*a = 1;
					if ((which_mode == 7) || (spec.ex2a == 0))
						teleport_party(spec.ex1a,spec.ex1b,1);
						else teleport_party(spec.ex1a,spec.ex1b,0);
					}
			*redraw = 1;
			break;
		case 175:
			if (which_mode == 7)
				break;
			hit_space(l,spec.ex2a,spec.ex2b,1,1);
			*redraw = 1;
			break;
		case 176:
			if (which_mode == 7)
				break;
			radius_damage(l,spec.pic, spec.ex2a, spec.ex2b);
			*redraw = 1;
			break;
		case 177:
			ter = coord_to_ter(spec.ex1a,spec.ex1b);
			if (scenario.ter_types[ter].special == 8)
				set_terrain(l,scenario.ter_types[ter].flag1);
			*redraw = 1;
			break;
		case 178:
			ter = coord_to_ter(spec.ex1a,spec.ex1b);
			if ((scenario.ter_types[ter].special == 9) || (scenario.ter_types[ter].special == 10))
				set_terrain(l,scenario.ter_types[ter].flag1);
			*redraw = 1;
			break;
		case 179:
			if (which_mode == 7)
				break;
			run_a_boom(l,spec.ex2a,0,0);
			break;
		case 180:
			create_wand_monst();
			*redraw = 1;
			break;
		case 181:
			if (spec.ex2a > 0)
				forced_place_monster(spec.ex2a,l);
				else c_town.placeMonster(spec.ex2a,l);
			*redraw = 1;
			break;
		case 182:
			for (i = 0; i < T_M; i++)
				if (c_town.monst.dudes[i].number == spec.ex1a) {
					c_town.monst.dudes[i].active = 0;
					}			
			*redraw = 1;
			break;
		case 183:
			for (i = 0; i < T_M; i++)
				if ((c_town.monst.dudes[i].active > 0) &&
					(((spec.ex1a == 0) && (1 == 1)) || 
					((spec.ex1a == 1) && (c_town.monst.dudes[i].attitude % 2 == 0)) || 
					((spec.ex1a == 2) && (c_town.monst.dudes[i].attitude % 2 == 1)))){
					c_town.monst.dudes[i].active = 0;
					}
			*redraw = 1;
			break;
		case 184:
			check_mess = FALSE;
			if (which_mode > 4) {
				ASB("Can't use lever now.");
				check_mess = FALSE;
				*next_spec = -1;
				}
				else if (store_special_loc.handleLever() > 0) *next_spec = spec.ex1b;
			break;
		case 185:
			check_mess = FALSE;
			if (is_combat()) {
				ASB("Not while in combat.");
				if (which_mode < 3) *a = 1;
				*next_spec = -1;
				}
				else if ((which_mode != 1) && (which_mode != 4)) {
					ASB("Can't teleport now.");
					if (which_mode < 3) *a = 1;
					*next_spec = -1;
					}
				else if (FCD(870,0) == 1) { *next_spec = -1; if (which_mode < 3) *a = 1;}
                else{		
					*a = 1;
					if ((which_mode == 7) || (spec.ex2a == 0))
						teleport_party(spec.ex1a,spec.ex1b,1);
						else teleport_party(spec.ex1a,spec.ex1b,0);
					}
			break;
		case 186:
			check_mess = FALSE;
			if (FCD(871,0) == 2) 		
				*next_spec = spec.ex1b;
			break;
		case 187:
			check_mess = FALSE;
			if (is_combat()) {
				ASB("Can't change level in combat.");
				if (which_mode < 3)
					*a = 1;
				*next_spec = -1;
				}
				else if (which_mode != 1) {
					ASB("Can't change level now.");
					if (which_mode < 3)
						*a = 1;
					*next_spec = -1;
					}
				else {
                    if (spec.ex2b < 0) spec.ex2b = 0;
                    if (spec.ex2b > 7) spec.ex2b = 7;
                    if (FCD(880 + spec.ex2b,0) == 1) { *next_spec = -1; if (which_mode < 3) *a = 1;}
                else {
					*a = 1;
					change_level(spec.ex2a,l.x,l.y);
					}
                }
			break;
		case 188:
			check_mess = FALSE;
			if (spec.m1 < 0)
				break;
			if (which_mode > 4) {
				ASB("Can't use lever now.");
				check_mess = FALSE;
				*next_spec = -1;
				}
				else {
					for (i = 0; i < 3; i++)
						get_strs((char *) strs[i * 2],(char *) strs[i * 2 + 1],cur_spec_type,
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
		case 189:
			check_mess = FALSE;
			if (spec.m1 < 0)
				break;
			if (is_combat()) {
				ASB("Not while in combat.");
				if (which_mode < 3)
					*a = 1;
				*next_spec = -1;
				check_mess = FALSE;
				}
				else if ((which_mode != 1) && (which_mode != 4)) {
					ASB("Can't teleport now.");
					if (which_mode < 3)
						*a = 1;
					*next_spec = -1;
					check_mess = FALSE;
					}
				else {
					for (i = 0; i < 3; i++)
						get_strs((char *) strs[i * 2],(char *) strs[i * 2 + 1]
						,cur_spec_type,spec.m1 + i * 2 + spec_str_offset[cur_spec_type],spec.m1 + i * 2 + 1 + spec_str_offset[cur_spec_type]);
					buttons[0] = 9; buttons[1] = 8;
					i = custom_choice_dialog((char *) strs,722,buttons);
					if (i == 1) { *next_spec = -1; if (which_mode < 3) *a = 1;}
						else {
							*a = 1;
							if (which_mode == 7 || (spec.ex2a == 0)) 
								teleport_party(spec.ex1a,spec.ex1b,1);
								else teleport_party(spec.ex1a,spec.ex1b,0);							
							}
					}
			break;
		case 190:
			check_mess = FALSE;
			if ((spec.m1 < 0) && (spec.ex2b != 1))
				break;
			if (is_combat()) {
				ASB("Can't change level in combat.");
				if (which_mode < 3)
					*a = 1;
				*next_spec = -1;
				check_mess = FALSE;
				}
				else if (which_mode != 1) {
					ASB("Can't change level now.");
					if (which_mode < 3)
						*a = 1;
					*next_spec = -1;
					check_mess = FALSE;
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
					*a = 1;
					if (i == 1) { *next_spec = -1;}
						else {
							*a = 1;
							change_level(spec.ex2a,l.x,l.y);
							}
					}
			break;
		case 191:
			position_party(spec.ex1a,spec.ex1b,spec.ex2a,spec.ex2b);
			break;
		case 192:
			store_i = get_stored_item(spec.ex2a);
			place_item(store_i,l,TRUE);
			break;
		case 193:
			if (which_mode == 7)
				break;
			if (is_combat()) {
				ASB("Not while in combat.");
				if (which_mode < 3)
					*a = 1;
				*next_spec = -1;
				check_mess = FALSE;
				}
			else if (party.stuff_done[304][0] > 0) {
				ASB("Party is already split.");
				if (which_mode < 3)
					*a = 1;
				*next_spec = -1;
				check_mess = FALSE;
				}
			else{
			r1 = char_select_pc(1,0,"Which character goes?");
			if (which_mode < 3)
				*a = 1;
			if (r1 != 6) {
				party.stuff_done[304][3] = r1;
				*next_spec = -1;
				start_split(spec.ex1a,spec.ex1b,spec.ex2a);
				}
				else check_mess = FALSE;
            }				
			break;
		case 194:
			if (is_combat()) {
				ASB("Not while in combat.");
				break;
				}
			if (which_mode < 3)
				*a = 1;
			*next_spec = -1;
			check_mess = FALSE;		
			end_split(spec.ex1a);
			break;
		case 195:
			for (i = 0; i < 30; i++)
	 			if (party.party_event_timers[i] == 0) {
	 				party.party_event_timers[i] = spec.ex1a;
	 				party.node_to_call[i] = spec.ex1b;
	 				party.global_or_town[i] = 1;
	 				i = 30;
	 				}
			break;
		}
	if (check_mess == TRUE) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
}

void rect_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = TRUE;
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
		case 200: if (get_ran(1,1,100) <= spec.sd1 ) make_fire_wall(i,j); break;
		case 201: if (get_ran(1,1,100) <= spec.sd1 ) make_force_wall(i,j); break;
		case 202: if (get_ran(1,1,100) <= spec.sd1 ) make_ice_wall(i,j); break;
		case 203: if (get_ran(1,1,100) <= spec.sd1 ) make_blade_wall(i,j); break;
		case 204: if (get_ran(1,1,100) <= spec.sd1 ) make_scloud(i,j); break;
		case 205: if (get_ran(1,1,100) <= spec.sd1 ) make_sleep_cloud(i,j); break;
		case 206: if (get_ran(1,1,100) <= spec.sd1 ) make_quickfire(i,j); break;
		case 207: if (get_ran(1,1,100) <= spec.sd1 ) make_fire_barrier(i,j); break;
		case 208: if (get_ran(1,1,100) <= spec.sd1 ) make_force_barrier(i,j); break;
		case 209: if (spec.sd1 == 0) dispel_fields(i,j,1); else dispel_fields(i,j,2);break;
		case 210: if (get_ran(1,1,100) <= spec.sd1 ) make_sfx(i,j,spec.sd2 + 1); break;
		case 211: if (get_ran(1,1,100) <= spec.sd1 ) {
					if (spec.sd2 == 0) make_web(i,j);
					if (spec.sd2 == 1) make_barrel(i,j);
					if (spec.sd2 == 2) make_crate(i,j);
					} break;
		case 212:
			for (k = 0; k < NUM_TOWN_ITEMS; k++)
				if ((t_i.items[k].variety > 0) && (same_point(t_i.items[k].item_loc,l) == TRUE)) {
					t_i.items[k].item_loc.x = spec.sd1;
					t_i.items[k].item_loc.y = spec.sd2;
					}					
			break;
		case 213:
			for (k = 0; k < NUM_TOWN_ITEMS; k++)
				if ((t_i.items[k].variety > 0) && (same_point(t_i.items[k].item_loc,l) == TRUE)) {
					t_i.items[k].variety = 0;
					}					
			break;
		case 214:
			if (get_ran(1,1,100) <= spec.sd2){ 
                set_terrain(l,spec.sd1);
	            if(scenario.ter_types[spec.sd1].special >= 16 && scenario.ter_types[spec.sd1].special <=19)
                    belt_present = TRUE;
            if(party.stuff_done[308][0] == 0)
            draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
                
            }
			break;
		case 215:
			if (coord_to_ter(i,j) == spec.sd1){
                set_terrain(l,spec.sd2);
                if(scenario.ter_types[spec.sd2].special >= 16 && scenario.ter_types[spec.sd2].special <=19)
                      belt_present = TRUE;
            }
			else if (coord_to_ter(i,j) == spec.sd2){
                set_terrain(l,spec.sd1);
                if(scenario.ter_types[spec.sd1].special >= 16 && scenario.ter_types[spec.sd1].special <=19)
                    belt_present = TRUE;
            }
            if(party.stuff_done[308][0] == 0)
            draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		case 216:
			ter = coord_to_ter(i,j);
			set_terrain(l,scenario.ter_types[ter].trans_to_what);
			if(scenario.ter_types[scenario.ter_types[ter].trans_to_what].special >= 16 && scenario.ter_types[scenario.ter_types[ter].trans_to_what].special <=19)
                belt_present = TRUE;
            if(party.stuff_done[308][0] == 0)
            draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		case 217:
			ter = coord_to_ter(i,j);
			if (scenario.ter_types[ter].special == 8)
				set_terrain(l,scenario.ter_types[ter].flag1);
			if(scenario.ter_types[scenario.ter_types[ter].flag1].special >= 16 && scenario.ter_types[scenario.ter_types[ter].flag1].special <=19)
                belt_present = TRUE;
            if(party.stuff_done[308][0] == 0)
            draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
		case 218:
			ter = coord_to_ter(i,j);
			if ((scenario.ter_types[ter].special == 9) || (scenario.ter_types[ter].special == 10))
				set_terrain(l,scenario.ter_types[ter].flag1);
		  if(scenario.ter_types[scenario.ter_types[ter].flag1].special >= 16 && scenario.ter_types[scenario.ter_types[ter].flag1].special <=19)
                belt_present = TRUE;
            if(party.stuff_done[308][0] == 0)
            draw_map_rect(modeless_dialogs[5],l.x,l.y,l.x,l.y);
			break;
			
		}
	}
}
	if (check_mess == TRUE) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
}

void outdoor_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *,short *a,short *b,short *redraw)
{
	Boolean check_mess = FALSE;
	char str1[256] = "",str2[256] = "";
	special_node_type spec;
	location l;
	
	spec = cur_node;
	*next_spec = cur_node.jumpto;
	
	if (is_out() == FALSE) return;
	
	switch (cur_node.type) {
		case 225: create_wand_monst();
			*redraw = 1;
			break;
		case 226:
			outdoors[party.i_w_c.x][party.i_w_c.y].terrain[spec.ex1a][spec.ex1b] = spec.ex2a;
			l.x = spec.ex1a;
			l.y = spec.ex1b;
			l = l.toGlobal();
			out[l.x][l.y] = spec.ex2a;
			*redraw = 1;
			check_mess = TRUE;
			break;		
		case 227:
			if (spec.ex1a != minmax(0,3,(int)spec.ex1a)) {
				give_error("Special outdoor enc. is out of range. Must be 0-3.","",0);
				//set_sd = FALSE;
				}
				else {
					l = party.p_loc.toLocal();
					place_outd_wand_monst(l,
					outdoors[party.i_w_c.x][party.i_w_c.y].special_enc[spec.ex1a],TRUE);			
					check_mess = TRUE;
					}
			break;		
		case 228:
			check_mess = TRUE;
			out_move_party(spec.ex1a,spec.ex1b);
			*redraw = 1;
			*a = 1;
			break;
		case 229:
			get_strs((char *) str1,(char *) str2,1,spec.m1 + 10,-1);
			if (spec.ex2a >= 40)
				spec.ex2a = 39;
			if (spec.ex2a < 1)
				spec.ex2a = 1;
			spec.ex2b = minmax(0,6,(int)spec.ex2b);
			switch (spec.ex1b) {
				case 0: start_shop_mode(0,spec.ex1a,spec.ex1a + spec.ex2a - 1,spec.ex2b,(char *) str1); break;
				case 1: start_shop_mode(10,spec.ex1a,spec.ex1a + spec.ex2a - 1,spec.ex2b,(char *) str1); break;
				case 2: start_shop_mode(11,spec.ex1a,spec.ex1a + spec.ex2a - 1 ,spec.ex2b,(char *) str1); break;
				case 3: start_shop_mode(12,spec.ex1a,spec.ex1a + spec.ex2a - 1,spec.ex2b,(char *) str1); break;
				case 4: start_shop_mode(3,spec.ex1a,spec.ex1a + spec.ex2a - 1,spec.ex2b,(char *) str1); break;
				}
			*next_spec = -1;
			break;		
		}
		
	if (check_mess == TRUE) {
		handle_message(which_mode,cur_spec_type,cur_node.m1,cur_node.m2,a,b);
		}
}

void setsd(short a,short b,short val)
{
	if (sd_legit(a,b) == FALSE) {
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
	
	if ((mess1 < 0) && (mess2 < 0))
		return;
	if (which_mode == 7) { // talking
		*a = mess1 + ((mess1 >= 0) ? mess_adj[cur_type] : 0); 
		*b = mess2 + ((mess2 >= 0) ? mess_adj[cur_type] : 0);
		return;
		}
	get_strs((char *) str1,(char *) str2, cur_type, mess1 + ((mess1 >= 0) ? mess_adj[cur_type] : 0), 
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
	display_strings((char *) str1, (char *) str2,label1,label2, label1b,label2b, 
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
					strcpy((char *) str1,data_store5->scen_strs[which_str1]);
					else strcpy((char *) str1,scen_strs2[which_str1 - 160]);
				
				}
			if (which_str2 >= 0){
				if (which_str2 < 160)
					strcpy((char *) str2,data_store5->scen_strs[which_str2]);
					else strcpy((char *) str2,scen_strs2[which_str2 - 160]);				
				}
			break;
		case 1:
			if (which_str1 >= 0)
				load_outdoors(party.outdoor_corner.x + party.i_w_c.x, 
					party.outdoor_corner.y + party.i_w_c.y, party.i_w_c.x, party.i_w_c.y,
					1,which_str1,(char *) str1);
			if (which_str2 >= 0)
				load_outdoors(party.outdoor_corner.x + party.i_w_c.x, 
					party.outdoor_corner.y + party.i_w_c.y, party.i_w_c.x, party.i_w_c.y,
					1,which_str2,(char *) str2);
			break;
		case 2:
			if (which_str1 >= 0)
				strcpy((char *) str1,data_store->town_strs[which_str1]);
			if (which_str2 >= 0)
				strcpy((char *) str2,data_store->town_strs[which_str2]);
			break;
		}

}

void use_spec_item(short item)
{
	short i,j,k;

	run_special(8,0,scenario.special_item_special[item],location(),&i,&j,&k);
}
