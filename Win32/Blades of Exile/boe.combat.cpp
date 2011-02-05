#include <windows.h>
#include <cstdio>
#include "global.h"
#include "boe.monster.h"
#include "boe.graphics.h"
#include "boe.locutils.h"
#include "boe.newgraph.h"
#include "boe.infodlg.h"
#include "boe.fields.h"
#include "boe.text.h"
#include "boe.items.h"
#include "boe.party.h"
#include "boe.combat.h"
#include "tools/soundtool.h"
#include "tools/mathutil.h"
#include "boe.town.h"
#include "boe.specials.h"
#include "boe.graphutil.h"
#include "classes/consts.h"

#include "globvar.h"

void start_outdoor_combat(outdoor_creature_type encounter,unsigned char in_which_terrain,short num_walls)
{
	short i,j,how_many,num_tries = 0;
	short low[10] = {15,7,4,3,2,1,1,7,2,1};
	short high[10] = {30,10,6,5,3,2,1,10,4,1};
	RECT town_rect = {0,0,47,47};
	short nums[10];

	for (i = 0; i < 7; i++)
		nums[i] = get_ran(1,low[i],high[i]);
	for (i = 0; i < 3; i++)
		nums[i + 7] = get_ran(1,low[i + 7],high[i + 7]);
		notify_out_combat_began(encounter.what_monst,nums);
  	print_buf();
	play_sound(23);

	which_combat_type = 0;
	town_type = 1;
	overall_mode = MODE_COMBAT;

	// Basically, in outdoor combat, we create kind of a 48x48 town for
	// the combat to take place in
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			c_town.explored[i][j] = 0;
			misc_i[i][j] = 0;
			sfx[i][j] = 0;
			}

	c_town.town.in_town_rect = town_rect;

	create_out_combat_terrain((short) in_which_terrain,num_walls);////

	for (i = 0; i < T_M; i++) {
		c_town.monst.dudes[i].number = 0;
		c_town.monst.dudes[i].active = 0;
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
	pc_pos[0] = out_start_loc;
	update_explored(pc_pos[0]);
	if (get_blockage(combat_terrain[pc_pos[0].x][pc_pos[0].y]) > 0)
		combat_terrain[pc_pos[0].x][pc_pos[0].y] = combat_terrain[0][0];
	for (i = 1; i < 6; i++) {
		pc_pos[i] = pc_pos[0];
		pc_pos[i].x = pc_pos[i].x + hor_vert_place[i].x;
		pc_pos[i].y = pc_pos[i].y + hor_vert_place[i].y;
		if (get_blockage(combat_terrain[pc_pos[i].x][pc_pos[i].y]) > 0)
			combat_terrain[pc_pos[i].x][pc_pos[i].y] = combat_terrain[0][0];
		update_explored(pc_pos[i]);

		for (j = 0; j < 6; j++)
			if (j != 2)
				adven[i].status[j] = 0;
		}

	// place monsters, w. friendly monsts landing near PCs
	for (i = 0; i < T_M; i++)
		if (c_town.monst.dudes[i].active > 0) {
			monst_target[i] = 6;

			c_town.monst.dudes[i].m_loc.x  = get_ran(1,15,25);
			c_town.monst.dudes[i].m_loc.y  = get_ran(1,14,18);
			if (c_town.monst.dudes[i].attitude == 2)
				c_town.monst.dudes[i].m_loc.y += 9;
				else if ((c_town.monst.dudes[i].m_d.mu > 0) || (c_town.monst.dudes[i].m_d.cl > 0))
					c_town.monst.dudes[i].m_loc.y = c_town.monst.dudes[i].m_loc.y - 4;//max(12,c_town.monst.dudes[i].m_loc.y - 4);
			num_tries = 0;
			while (((monst_can_be_there(c_town.monst.dudes[i].m_loc,i) == false) ||
			 (combat_terrain[c_town.monst.dudes[i].m_loc.x][c_town.monst.dudes[i].m_loc.y] == 180) ||
			 (pc_there(c_town.monst.dudes[i].m_loc) < 6)) &&
			 (num_tries++ < 50)) {
				c_town.monst.dudes[i].m_loc.x = get_ran(1,15,25);
				c_town.monst.dudes[i].m_loc.y = get_ran(1,14,18);
				if (c_town.monst.dudes[i].attitude == 2)
					c_town.monst.dudes[i].m_loc.y += 9;
					else if ((c_town.monst.dudes[i].m_d.mu > 0) || (c_town.monst.dudes[i].m_d.cl > 0))
						c_town.monst.dudes[i].m_loc.y = c_town.monst.dudes[i].m_loc.y - 4;//max(12,c_town.monst.dudes[i].m_loc.y - 4);
				}
			if (get_blockage(combat_terrain[c_town.monst.dudes[i].m_loc.x][c_town.monst.dudes[i].m_loc.y]) > 0)
				combat_terrain[c_town.monst.dudes[i].m_loc.x][c_town.monst.dudes[i].m_loc.y] = combat_terrain[0][0];
		}


	combat_active_pc = 6;
	spell_caster = 6; missile_firer = 6;
	for (i = 0; i < T_M; i++)
		monst_target[i] = 6;

	for (i = 0; i < 6; i++) {
		pc_parry[i] = 0;
		last_attacked[i] = T_M + 10;
		}

	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		t_i.items[i].variety = ITEM_TYPE_NO_ITEM;
	store_current_pc = current_pc;
	current_pc = 0;
	set_pc_moves();
	pick_next_pc();
	center = pc_pos[current_pc];
	draw_buttons(0);
	put_pc_screen();
	set_stat_window(current_pc);

	adjust_spell_menus();

	give_help(48,49,0);
}

Boolean pc_combat_move(location destination)
{
	short dir,monst_hit,s1,s2,i,monst_exist,switch_pc;
	Boolean keep_going = true,forced = false,check_f;
	location monst_loc,store_loc;
	short spec_num;

	if (monst_there(destination) > T_M)
		keep_going = check_special_terrain(destination,2,current_pc,&spec_num,&check_f);
	if (check_f == true)
		forced = true;

    if (in_scen_debug && ghost_mode)
        forced = true;

	if (spec_num == 50)
	   forced = true;

	if (keep_going == true) {

	dir = set_direction(pc_pos[current_pc], destination);

	if ((loc_off_act_area(destination) == true) && (which_combat_type == 1)) {
		add_string_to_buf("Move: Can't leave town during combat.");
		print_buf();
		return true;
		}
	else if ((combat_terrain[destination.x][destination.y] == 90) && (which_combat_type == 0)) {
			if (get_ran(1,1,10) < 3) {
				adven[current_pc].main_status = MAIN_STATUS_FLED;
				if (combat_active_pc == current_pc)
					combat_active_pc = 6;
				sprintf (create_line, "Moved: Fled.                    ");
				pc_moves[current_pc] = 0;
				}
				else {
					take_ap(1);
					sprintf (create_line, "Moved: Couldn't flee.                  ");
					}
			add_string_to_buf(create_line);
			return true;
		}
		else if ((monst_hit = monst_there(destination)) <= T_M) {
			s1 = c_town.monst.dudes[monst_hit].attitude;
			s2 = (s1 % 2 == 1) ? 2 : fancy_choice_dialog(1045,0);
			if ((s2 == 2) && (s1 % 2 != 1))
				set_town_status(0);
			if (s2 == 2) {
					last_attacked[current_pc] = monst_hit;
					pc_attack(current_pc,monst_hit);
					return true;
				}
			}
			else if ((switch_pc = pc_there(destination)) < 6) {
				if (pc_moves[switch_pc] == 0) {
					add_string_to_buf("Move: Can't switch places.");
					add_string_to_buf("  (other PC has no APs)	");
					return false;
					}
					else pc_moves[switch_pc]--;
				add_string_to_buf("Move: Switch places.");
				store_loc = pc_pos[current_pc];
				pc_pos[current_pc] = destination;
				pc_pos[switch_pc] = store_loc;
				adven[current_pc].direction = dir;
				take_ap(1);
				check_special_terrain(store_loc,2,switch_pc,&spec_num,&check_f);
				move_sound(combat_terrain[destination.x][destination.y],pc_moves[current_pc]);
				return true;
				}
			else if ((forced == true)
				|| ((impassable(combat_terrain[destination.x][destination.y]) == false) && (pc_there(destination) == 6))) {

				// monsters get back-shots
				for (i = 0; i < T_M; i++) {
					monst_loc = c_town.monst.dudes[i].m_loc;
					monst_exist = c_town.monst.dudes[i].active;

					s1 = current_pc;
					if ((monst_exist > 0) && (monst_adjacent(pc_pos[current_pc],i) == true)
						&& (monst_adjacent(destination,i) == false) &&
							(c_town.monst.dudes[i].attitude % 2 == 1) &&
							(c_town.monst.dudes[i].m_d.status[STATUS_ASLEEP] <= 0) &&
							(c_town.monst.dudes[i].m_d.status[STATUS_PARALYZED] <= 0)) {
							combat_posing_monster = current_working_monster = 100 + i;
							monster_attack_pc(i,current_pc);
							combat_posing_monster = current_working_monster = -1;
							draw_terrain(0);
							}
					if (s1 != current_pc)
						return true;
					}

				// move if still alive
				if (adven[current_pc].isAlive()) {
						pc_dir[current_pc] = set_direction(pc_pos[current_pc],destination);
						pc_pos[current_pc] = destination;
						adven[current_pc].direction = dir;
						take_ap(1);
						//sprintf ((char *) create_line, "Moved: %s               ",d_string[dir]);
						//add_string_to_buf((char *) create_line);
						move_sound(combat_terrain[destination.x][destination.y],pc_moves[current_pc]);

					}
					else return false;
				return true;
				}
				else {
					sprintf (create_line, "Blocked: %s               ",d_string[dir]);
					add_string_to_buf(create_line);
					return false;
				}
	}
	return false;
}

void char_parry()
{
	pc_parry[current_pc] = (pc_moves[current_pc] / 4) *
		(2 + adven[current_pc].statAdj(SKILL_DEXTERITY) + adven[current_pc].skills[SKILL_DEFENSE]);
	pc_moves[current_pc] = 0;
}

void char_stand_ready()
{
	pc_parry[current_pc] = 100;
	pc_moves[current_pc] = 0;
}

void pc_attack(short who_att,short target)
{
	short r1,r2,what_skill1 = 1, what_skill2 = 1, weap1 = 24, weap2 = 24,i,store_hp,skill_item;
	creature_data_type *which_m;
	short hit_adj, dam_adj, spec_dam = 0,poison_amt;

	// slice out bad attacks
	if (adven[who_att].isAlive() == false)
		return;
	if ((adven[who_att].status[STATUS_ASLEEP] > 0) || (adven[who_att].status[STATUS_PARALYZED] > 0))
		return;

	last_attacked[who_att] = target;
	which_m = &c_town.monst.dudes[target];

	for (i = 0; i < 24; i++)
		if (((adven[who_att].items[i].variety == ITEM_TYPE_ONE_HANDED) || (adven[who_att].items[i].variety == ITEM_TYPE_TWO_HANDED)) &&
			(adven[who_att].equip[i] == true))
				if (weap1 == 24)
					weap1 = i;
					else weap2 = i;

	hit_adj = (-5 * minmax(-8,8,(int)adven[who_att].status[STATUS_BLESS_CURSE])) + 5 * minmax(-8,8,(int)which_m->m_d.status[STATUS_BLESS_CURSE])
			- adven[who_att].statAdj(SKILL_DEXTERITY) * 5 + (get_encumberance(who_att)) * 5;

	dam_adj = minmax(-8,8,(int)adven[who_att].status[STATUS_BLESS_CURSE]) - minmax(-8,8,(int)which_m->m_d.status[STATUS_BLESS_CURSE])
			+ adven[who_att].statAdj(SKILL_STRENGTH);

	if ((which_m->m_d.status[STATUS_ASLEEP] > 0) || (which_m->m_d.status[STATUS_PARALYZED] > 0)) {
		hit_adj -= 80;
		dam_adj += 10;
		}


	if ((skill_item = text_pc_has_abil_equip(who_att,ITEM_SKILL)) < 24) {
		hit_adj -= 5 * (adven[who_att].items[skill_item].ability_strength / 2 + 1);
		dam_adj += adven[who_att].items[skill_item].ability_strength / 2;
		}
	if ((skill_item = text_pc_has_abil_equip(who_att,ITEM_GIANT_STRENGTH)) < 24) {
		hit_adj -= adven[who_att].items[skill_item].ability_strength * 2;
		dam_adj += adven[who_att].items[skill_item].ability_strength;
		}

	void_sanctuary(who_att);

	store_hp = c_town.monst.dudes[target].m_d.health;

	combat_posing_monster = current_working_monster = who_att;

	if (weap1 == 24) {

		sprintf (create_line, "%s punches.  ", adven[who_att].name);//,hit_adj, dam_adj);
		add_string_to_buf((char *) create_line);

		r1 = get_ran(1,1,100) + hit_adj - 20;
		r1 += 5 * (adven[current_pc].status[STATUS_WEBS] / 3);
		r2 = get_ran(1,1,4) + dam_adj;

		if (r1 <= hit_chance[adven[who_att].skills[what_skill1]]) {
			damage_monst(target, who_att, r2, 0,400);
			}
			else {
				draw_terrain(2);
				sprintf (create_line, "%s misses. ",adven[who_att].name);
				add_string_to_buf(create_line);
				play_sound(2);
			}
		}

	// Don't forget awkward and stat adj.
	if (weap1 < 24) {
		what_skill1 = 2 + adven[who_att].items[weap1].type;

		// safety valve
		if (what_skill1 == 2)
			what_skill1 = 3;

		sprintf (create_line, "%s swings. ",adven[who_att].name);//,hit_adj, dam_adj);
		add_string_to_buf(create_line);

		r1 = get_ran(1,1,100) - 5 + hit_adj
		 - 5 * adven[who_att].items[weap1].bonus;
		r1 += 5 * (adven[current_pc].status[STATUS_WEBS] / 3);

		if ((weap2 < 24) && (adven[who_att].traits[TRAIT_AMBIDEXTROUS] == false))
			r1 += 25;

		// race adj.
		if ((adven[who_att].race == RACE_SLITH) && (adven[who_att].items[weap1].type == ITEM_POLE))
			r1 -= 10;

		r2 = get_ran(1,1,adven[who_att].items[weap1].item_level) + dam_adj + 2 + adven[who_att].items[weap1].bonus;
		if (adven[who_att].items[weap1].ability == ITEM_WEAK_WEAPON)
			r2 = (r2 * (10 - adven[who_att].items[weap1].ability_strength)) / 10;

		if (r1 <= hit_chance[adven[who_att].skills[what_skill1]]) {
			spec_dam = calc_spec_dam(adven[who_att].items[weap1].ability,
				adven[who_att].items[weap1].ability_strength,which_m);

			// assassinate
			r1 = get_ran(1,1,100);
			if ((adven[who_att].level >= which_m->m_d.level - 1)
				&& (adven[who_att].skills[SKILL_ASSASSINATION] >= which_m->m_d.level / 2)
				&& (which_m->m_d.spec_skill != MONSTER_SPLITS)) // Can't assassinate splitters
				if (r1 < hit_chance[max(adven[who_att].skills[SKILL_ASSASSINATION] - which_m->m_d.level,0)]) {
					add_string_to_buf("  You assassinate.           ");
					spec_dam += r2;
					}

			switch (what_skill1) {
			 	case 3:
					if (adven[who_att].items[weap1].item_level < 8)
						damage_monst(target, who_att, r2, spec_dam, 100);
						else damage_monst(target, who_att, r2, spec_dam, 200);
					break;
			 	case 4:
					damage_monst(target, who_att, r2, spec_dam, 400);
					break;
			 	case 5:
					damage_monst(target, who_att, r2, spec_dam, 300);
					break;
				}
			// poison
			if ((adven[who_att].status[STATUS_POISONED_WEAPON] > 0) && (adven[who_att].weap_poisoned == weap1)) {
					poison_amt = adven[who_att].status[STATUS_POISONED_WEAPON];
					if (adven[who_att].hasAbilEquip(ITEM_POISON_AUGMENT) < 24)
						poison_amt += 2;
					which_m->poison(poison_amt);
					move_to_zero(adven[who_att].status[STATUS_POISONED_WEAPON]);
				}
			if ((adven[who_att].items[weap1].ability == ITEM_POISONED_WEAPON) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drips venom.             ");
				which_m->poison(adven[who_att].items[weap1].ability_strength / 2);
				}
			if ((adven[who_att].items[weap1].ability == ITEM_ACIDIC_WEAPON) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drips acid.             ");
				which_m->acid(adven[who_att].items[weap1].ability_strength / 2);
				}
			if ((adven[who_att].items[weap1].ability == ITEM_SOULSUCKER) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drains life.             ");
				adven[who_att].heal(adven[who_att].items[weap1].ability_strength / 2);
				}
			}
			else {
				draw_terrain(2);
				sprintf (create_line, "  %s misses.              ", adven[who_att].name);
				add_string_to_buf(create_line);
				if (what_skill1 == 5)
					play_sound(19);
					else play_sound(2);
			}
		}
	if ((weap2 < 24) && (which_m->active > 0)) {
		what_skill2 = 2 + adven[who_att].items[weap2].type;

		// safety valve
		if (what_skill2 == 2)
			what_skill2 = 3;


		sprintf (create_line, "%s swings.                    ", adven[who_att].name);//,hit_adj, dam_adj);
		add_string_to_buf(create_line);
		r1 = get_ran(1,1,100) + hit_adj - 5 * adven[who_att].items[weap2].bonus;

		// Ambidextrous?
		if (adven[who_att].traits[TRAIT_AMBIDEXTROUS] == false)
			r1 += 25;

		r1 += 5 * (adven[current_pc].status[STATUS_WEBS] / 3);
		r2 = get_ran(1,1,adven[who_att].items[weap2].item_level) + dam_adj - 1 + adven[who_att].items[weap2].bonus;
		if (adven[who_att].items[weap2].ability == ITEM_WEAK_WEAPON)
			r2 = (r2 * (10 - adven[who_att].items[weap2].ability_strength)) / 10;

		if (r1 <= hit_chance[adven[who_att].skills[what_skill2]]) {
			spec_dam = calc_spec_dam(adven[who_att].items[weap2].ability,
				adven[who_att].items[weap2].ability_strength,which_m);
			switch (what_skill2) {
			 	case 3:
					if (adven[who_att].items[weap1].item_level < 8)
						damage_monst(target, who_att, r2, spec_dam, 100);
						else damage_monst(target, who_att, r2, spec_dam, 200);
					break;
			 	case 4:
					damage_monst(target, who_att, r2, spec_dam, 400);
					break;
			 	case 5:
					damage_monst(target, who_att, r2, spec_dam, 300);
					break;
				}

			if ((adven[who_att].items[weap2].ability == ITEM_POISONED_WEAPON) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drips venom.             ");
				which_m->poison(adven[who_att].items[weap2].ability_strength / 2);
				}
			if ((adven[who_att].items[weap2].ability == ITEM_ACIDIC_WEAPON) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drips acid.             ");
				which_m->acid(adven[who_att].items[weap2].ability_strength / 2);
				}
			if ((adven[who_att].items[weap2].ability == ITEM_SOULSUCKER) && (get_ran(1,0,1) == 1)) {
				add_string_to_buf("  Blade drains life.             ");
				adven[who_att].heal(adven[who_att].items[weap2].ability_strength / 2);
				}

			}
			else {
				draw_terrain(2);
				sprintf (create_line, "%s misses.             ", adven[who_att].name);
				add_string_to_buf( create_line);
				if (what_skill2 == 5)
					play_sound(19);
					else play_sound(2);
			}
		}
	move_to_zero(adven[who_att].status[STATUS_POISONED_WEAPON]);
	take_ap(4);

	if (((c_town.monst.dudes[target].m_d.status[STATUS_MARTYRS_SHIELD] > 0) || (c_town.monst.dudes[target].m_d.spec_skill == MONSTER_PERMANENT_MARTYRS_SHIELD))
	 && (store_hp - c_town.monst.dudes[target].m_d.health > 0)) {
		add_string_to_buf("  Shares damage!   ");
		adven[who_att].damage(store_hp - c_town.monst.dudes[target].m_d.health, 3,-1);
		}
	combat_posing_monster = current_working_monster = -1;
 }


short calc_spec_dam(short abil,short abil_str,creature_data_type *monst) ////
{
	short store = 0;

	switch (abil) {
		case ITEM_FLAMING_WEAPON: case ITEM_MISSILE_LIGHTNING:
			store += get_ran(abil_str,1,6);
			break;
		case ITEM_DEMON_SLAYER:
			if (monst->m_d.m_type == MONSTER_TYPE_DEMON)
				store += 8 * abil_str;
			break;
		case ITEM_UNDEAD_SLAYER:
			if (monst->m_d.m_type == MONSTER_TYPE_UNDEAD)
				store += 6 * abil_str;
			break;
		case ITEM_LIZARD_SLAYER:
			if (monst->m_d.m_type == MONSTER_TYPE_REPTILE)
				store += 5 * abil_str;
			break;
		case ITEM_GIANT_SLAYER:
			if (monst->m_d.m_type == MONSTER_TYPE_GIANT)
				store += 8 * abil_str;
			break;
		case ITEM_MAGE_SLAYER :
			if (monst->m_d.m_type == MONSTER_TYPE_MAGE)
				store += 4 * abil_str;
			break;
		case ITEM_PRIEST_SLAYER :
			if (monst->m_d.m_type == MONSTER_TYPE_PRIEST)
				store += 4 * abil_str;
			break;
		case ITEM_BUG_SLAYER:
			if (monst->m_d.m_type == MONSTER_TYPE_BUG)
				store += 7 * abil_str;
			break;
		case ITEM_CAUSES_FEAR:
			monst->scare(abil_str * 10);
			break;
		case ITEM_MISSILE_ACID:
			monst->acid(abil_str);
			break;
    	case ITEM_MISSILE_SLAY_UNDEAD:
			if (monst->m_d.m_type == MONSTER_TYPE_UNDEAD)
				store += 20 + 6 * abil_str;
			break;
        case ITEM_MISSILE_SLAY_DEMON:
			if (monst->m_d.m_type == MONSTER_TYPE_DEMON)
				store += 25 + 8 * abil_str;
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
		if (can_see(pc_pos[current_pc],target,0) > 4) {
				add_string_to_buf("  Can't see target.           ");
				return;
				}
		if (dist(pc_pos[current_pc],target) > ((spell_being_cast >= 100) ? priest_range[spell_being_cast - 100] : mage_range[spell_being_cast]))  {
			add_string_to_buf("  Target out of range.");
			return;
			}
		if ((get_obscurity(target.x,target.y) == 5) && (spell_being_cast != SPELL_MAGE_DISPEL_BARRIER)) {
				 add_string_to_buf("  Target space obstructed.           ");
				 return;
				 }
		if (is_antimagic(target.x,target.y)) {
				 add_string_to_buf("  Target in antimagic field.");
				 return;
				 }
		for (i = 0; i < 8; i++) {
			if (same_point(spell_targets[i],target) == true) {
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
//spell_being_cast : spell number for mage, 100 + spell number for priest
//1000 + spell number for no mana cost (items)
void do_combat_cast(location target)
{
	short adjust,r1,r2,targ_num,s_num,level,bonus = 1,i,item,store_sound = 0;
	creature_data_type *cur_monst;
	Boolean freebie = false,ap_taken = false,cost_taken = false;
	short num_targets = 1,store_m_type = 2;
	short spray_type_array[15] = {1,1,1,4,4,5,5,5,6,6,7,7,8,8,9};
	unsigned short summon;

	location ashes_loc;

	// to wedge in animations, have to kludge like crazy
	short boom_dam[8] = {0,0,0,0,0,0,0,0};
	short boom_type[8] = {0,0,0,0,0,0,0,0};
	location boom_targ[8];

	if (spell_being_cast >= 1000) {
		spell_being_cast -= 1000;
		freebie = true;
		level = minmax(2,20, (int) store_item_spell_level);
		}
		else {
			level = 1 + adven[current_pc].level / 2;
			bonus = adven[current_pc].statAdj(SKILL_INTELLIGENCE);
			}
	force_wall_position = 10;
	s_num = spell_being_cast % 100;

	void_sanctuary(current_pc);
	if (overall_mode == MODE_SPELL_TARGET)	spell_targets[0] = target;
	else num_targets = 8;

	spell_caster = current_pc;

	// assign monster summoned, if summoning
	if (spell_being_cast == SPELL_MAGE_SUMMON_BEAST)	summon = get_summon_monster(1);
	if (spell_being_cast == SPELL_MAGE_WEAK_SUMMONING)	summon = get_summon_monster(1);
	if (spell_being_cast == SPELL_MAGE_SUMMONING) summon = get_summon_monster(2);
	if (spell_being_cast == SPELL_MAGE_MAJOR_SUMMON) summon = get_summon_monster(3);

	combat_posing_monster = current_working_monster = current_pc;

	for (i = 0; i < num_targets; i++)
		if (spell_targets[i].x != 120) {
		target = spell_targets[i];
		spell_targets[i].x = 120; // nullify target as it is used

		//if ((cost_taken == false) && (freebie == false) && (s_num != 52) && (s_num != 35)) {
        if ((cost_taken == false) && (freebie == false) && (spell_being_cast != SPELL_MAGE_MINDDUEL) && (spell_being_cast != SPELL_MAGE_SIMULACRUM)) {
			adven[current_pc].cur_sp -= s_cost[spell_being_cast / 100][s_num];
			cost_taken = true;
			}
		//if ((cost_taken == false) && (freebie == false) && (s_num == 35)) {
		if ((cost_taken == false) && (freebie == false) && (spell_being_cast == SPELL_MAGE_SIMULACRUM)) {
			adven[current_pc].cur_sp -=	store_sum_monst_cost;
			cost_taken = true;
			}

		if ((adjust = can_see(pc_pos[current_pc],target,0)) > 4) {
			add_string_to_buf("  Can't see target.           ");
			}
		else if (loc_off_act_area(target) == true) {
			add_string_to_buf("  Space not in town.           ");
			}
		else if (dist(pc_pos[current_pc],target) > ((spell_being_cast >= 100) ? priest_range[spell_being_cast - 100] : mage_range[spell_being_cast]))
		add_string_to_buf("  Target out of range.");
			else if ((get_obscurity(target.x,target.y) == 5) && (spell_being_cast != SPELL_MAGE_DISPEL_BARRIER))
			 add_string_to_buf("  Target space obstructed.           ");
			else if (is_antimagic(target.x,target.y))
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

				case SPELL_MAGE_GOO: case SPELL_MAGE_WEB: case SPELL_ITEMS_GOO_BOMB: // web spells
					place_spell_pattern(current_pat,target,1,false,current_pc);
					break;
				case SPELL_MAGE_FLAME_CLOUD: case SPELL_MAGE_CONFLAGRATION: // fire wall spells
					place_spell_pattern(current_pat,target,5,false,current_pc);
					break;
				case SPELL_MAGE_STINKING_CLOUD: case SPELL_ITEMS_FOUL_VAPORS: // stink cloud
					place_spell_pattern(current_pat,target,7,false,current_pc);
					break;
				case SPELL_MAGE_WALL_OF_FORCE: case SPELL_MAGE_SHOCKSTORM: case 100 + SPELL_PRIEST_FORCEFIELD: // force walls
					place_spell_pattern(current_pat,target,4,false,current_pc);
					break;
				case SPELL_MAGE_WALL_OF_ICE: case SPELL_ITEMS_ICE_WALL_BALLS: // ice walls
					place_spell_pattern(current_pat,target,8,false,current_pc);
					break;
				case SPELL_MAGE_ANTIMAGIC_CLOUD: // antimagic
					place_spell_pattern(current_pat,target,6,false,current_pc);
					break;
				case SPELL_MAGE_SLEEP_CLOUD: // sleep clouds //case 67: <= doesn't exist anymore (circular sleep clouds)
					place_spell_pattern(current_pat,target,12,false,current_pc);
					break;
				case SPELL_MAGE_QUICKFIRE:
					make_quickfire(target.x,target.y);
					break;
				case SPELL_MAGE_SPRAY_FIELDS: // spray fields
					r1 = get_ran(1,0,14);
					place_spell_pattern(current_pat,target,spray_type_array[r1],false,current_pc);
					break;
				case 100 + SPELL_PRIEST_WALL_OF_BLADES:  // wall of blades
					place_spell_pattern(current_pat,target,9,false,current_pc);
					break;
				case 100 + SPELL_PRIEST_DISPEL_FIELDS: case SPELL_MAGE_DISPEL_FIELDS: // wall dispelling //case 100 + SPELL_PRIEST_AWAKEN: <= old inexistant spell (119)
					place_spell_pattern(current_pat,target,11,false,current_pc);
					break;
			case SPELL_MAGE_FIRE_BARRIER: // Fire barrier
				play_sound(68);
				r1 = get_ran(3,2,7);
				hit_space(target,r1,1,true,true);
				make_fire_barrier(target.x,target.y);
				if (is_fire_barrier(target.x,target.y))
					add_string_to_buf("  You create the barrier.              ");
					else add_string_to_buf("  Failed.");
				break;
			case SPELL_MAGE_FORCE_BARRIER: // Force barrier
				play_sound(68);
				r1 = get_ran(7,2,7);
				hit_space(target,r1,1,true,true);
				make_force_barrier(target.x,target.y);
				if (is_force_barrier(target.x,target.y))
					add_string_to_buf("  You create the barrier.              ");
					else add_string_to_buf("  Failed.");
				break;

			default: // spells which involve animations
				start_missile_anim();
				switch (spell_being_cast) {

				case 100 + SPELL_PRIEST_DIVINE_THUD: //Divine Thud
					add_missile(target,9,1,0,0);
					store_sound = 11;
					r1 = min(18,(level * 7) / 10 + 2 * bonus);
					place_spell_pattern(radius2,target,130 + r1,true,current_pc);
					ashes_loc = target;
					break;

				case SPELL_MAGE_SPARK: case SPELL_MAGE_ICE_BOLT: //Spark - Ice Bolt
					r1 = (spell_being_cast == SPELL_MAGE_SPARK) ? get_ran(2,1,4) : get_ran(min(20,level + bonus),1,4);
					add_missile(target,6,1,0,0);
					do_missile_anim(100,pc_pos[current_pc],11);
					hit_space(target,r1,(spell_being_cast == 1) ? 3 : 5,1,0);
					break;
				case SPELL_MAGE_FLAME_ARROWS: //Flame Arrows
					add_missile(target,4,1,0,0);
					r1 = get_ran(2,1,4);
					boom_type[i] = 1;
					boom_dam[i] = r1;
					//hit_space(target,r1,1,1,0);
					break;
				case 100 + SPELL_PRIEST_SMITE: //Smite
					add_missile(target,6,1,0,0);
					r1 = get_ran(2,1,5);
					boom_type[i] = 5;
					boom_dam[i] = r1;
					//hit_space(target,r1,5,1,0);
					break;
				case 100 + SPELL_PRIEST_WOUND: //Wound
					r1 = get_ran(min(7,2 + bonus + level / 2),1,4);
					add_missile(target,14,1,0,0);
					do_missile_anim(100,pc_pos[current_pc],24);
					hit_space(target,r1,4,1,0);
					break;
				case SPELL_MAGE_FLAME: //Flame
					r1 = get_ran(min(10,1 + level / 3 + bonus),1,6);
					add_missile(target,2,1,0,0);
					do_missile_anim(100,pc_pos[current_pc],11);
					hit_space(target,r1,1,1,0);
					break;
				case SPELL_MAGE_FIREBALL: case 100 + SPELL_PRIEST_FLAMESTRIKE: //Fireball - Flamestrike
					r1 = min(9,1 + (level * 2) / 3 + bonus) + 1;
					add_missile(target,2,1,0,0);
					store_sound = 11;
					//do_missile_anim(100,pc_pos[current_pc],11);
					if (spell_being_cast == 100 + SPELL_PRIEST_FLAMESTRIKE) //Flamestrike
						r1 = (r1 * 14) / 10;
						else if (r1 > 10) r1 = (r1 * 8) / 10;
					if (r1 <= 0) r1 = 1;
					place_spell_pattern(square,target,50 + r1,true,current_pc);
					ashes_loc = target;
					break;
				case SPELL_MAGE_FIRE_STORM: //Fire Storm
					add_missile(target,2,1,0,0);
					store_sound = 11;
					//do_missile_anim(100,pc_pos[current_pc],11);
					r1 = min(12,1 + (level * 2) / 3 + bonus) + 2;
					if (r1 > 20)
						r1 = (r1 * 8) / 10;
					place_spell_pattern(radius2,target,50 + r1,true,current_pc);
					ashes_loc = target;
					break;
				case SPELL_MAGE_KILL:  // Kill
					add_missile(target,9,1,0,0);
					do_missile_anim(100,pc_pos[current_pc],11);
					r1 = get_ran(3,0,10) + adven[current_pc].level * 2;
					hit_space(target,40 + r1,3,1,0);
					break;
				case SPELL_MAGE_DEATH_ARROWS:	//Death Arrows
					add_missile(target,9,1,0,0);
					store_sound = 11;
					r1 = get_ran(3,0,10) + adven[current_pc].level + 3 * bonus;
					boom_type[i] = 3;
					boom_dam[i] = r1;
					//hit_space(target,40 + r1,3,1,0);
					break;
				// summoning spells
		case SPELL_MAGE_SIMULACRUM: case SPELL_MAGE_SUMMON_BEAST: case SPELL_MAGE_WEAK_SUMMONING:
		case SPELL_MAGE_SUMMONING: case SPELL_MAGE_MAJOR_SUMMON: case SPELL_MAGE_DAEMON:
		case 100 + SPELL_PRIEST_SUMMON_SPIRIT: case 100 + SPELL_PRIEST_STICKS_TO_SNAKES:
		case 100 + SPELL_PRIEST_SUMMON_HOST: case 100 + SPELL_PRIEST_GUARDIAN: //case 63: <= summon rat (doesn't exist anymore)
			add_missile(target,8,1,0,0);
			do_missile_anim(50,pc_pos[current_pc],61);
			switch (spell_being_cast) {
			case SPELL_MAGE_SIMULACRUM: // Simulacrum
				r2 = get_ran(3,1,4) + adven[current_pc].statAdj(SKILL_INTELLIGENCE);
				if (summon_monster(store_sum_monst,target,r2,2) == false)
					add_string_to_buf("  Summon failed.");
				break;
			case SPELL_MAGE_SUMMON_BEAST: // summon beast
				r2 = get_ran(3,1,4) + adven[current_pc].statAdj(SKILL_INTELLIGENCE);
				if ((summon == 0) ||
				(summon_monster(summon,target,r2,2) == false))
					add_string_to_buf("  Summon failed.");
				break;
			case SPELL_MAGE_WEAK_SUMMONING: // summon 1
				r2 = get_ran(4,1,4) + adven[current_pc].statAdj(SKILL_INTELLIGENCE);
				if ((summon == 0) || (summon_monster(summon,target,r2,2) == false))
					add_string_to_buf("  Summon failed.");
				break;
			case SPELL_MAGE_SUMMONING: // summon 2
				r2 = get_ran(5,1,4) + adven[current_pc].statAdj(SKILL_INTELLIGENCE);
				if ((summon == 0) || (summon_monster(summon,target,r2,2) == false))
					add_string_to_buf("  Summon failed.");
				break;
			case SPELL_MAGE_MAJOR_SUMMON: // summon 3
				r2 = get_ran(7,1,4) + adven[current_pc].statAdj(SKILL_INTELLIGENCE);
				if ((summon == 0) || (summon_monster(summon,target,r2,2) == false))
					add_string_to_buf("  Summon failed.");
				break;
			case SPELL_MAGE_DAEMON: // Daemon
				r2 = get_ran(5,1,4) + adven[current_pc].statAdj(SKILL_INTELLIGENCE);
				if (summon_monster(85,target,r2,2) == false)
					add_string_to_buf("  Summon failed.");
				break;
			/*case 63: // Rat!
				r1 = get_ran(3,1,4);
				if (summon_monster(80,target,r1,2) == false)
					add_string_to_buf("  Summon failed.");
				break;*/

			case 100 + SPELL_PRIEST_SUMMON_SPIRIT: // summon spirit
				r2 = get_ran(2,1,5) + adven[current_pc].statAdj(SKILL_INTELLIGENCE);
				if (summon_monster(125,target,r2,2) == false)
					add_string_to_buf("  Summon failed.");
				break;
			case 100 + SPELL_PRIEST_STICKS_TO_SNAKES: // s to s
				r1 = get_ran(1,0,7);
				r2 = get_ran(2,1,5) + adven[current_pc].statAdj(SKILL_INTELLIGENCE);
				if (summon_monster((r1 == 1) ? 100 : 99,target,r2,2) == false)
					add_string_to_buf("  Summon failed.");
				break;
			case 100 + SPELL_PRIEST_SUMMON_HOST: // host
				r2 = get_ran(2,1,4) + adven[current_pc].statAdj(SKILL_INTELLIGENCE);
				if (summon_monster((i == 0) ? 126 : 125,target,r2,2) == false)
					add_string_to_buf("  Summon failed.");
				break;
			case 100 + SPELL_PRIEST_GUARDIAN: // guardian
				r2 = get_ran(6,1,4) + adven[current_pc].statAdj(SKILL_INTELLIGENCE);
				if (summon_monster(122,target,r2,2) == false)
					add_string_to_buf("  Summon failed.");
				break;
				}
			break;


		default:
					targ_num = monst_there(target);
					if (targ_num > T_M)
						add_string_to_buf("  Nobody there                 ");
						else {
							cur_monst = &c_town.monst.dudes[targ_num];
							if ((cur_monst->attitude % 2 != 1) && (spell_being_cast != SPELL_MAGE_SCRY_MONSTER)
							 && (spell_being_cast != SPELL_MAGE_CAPTURE_SOUL))
								set_town_status(0);
							store_sound = (spell_being_cast >= 50) ? 24 : 25;
							switch (spell_being_cast) {
								case SPELL_ITEMS_SPRAY_ACID: // spray acid
									store_m_type = 0;
									cur_monst->acid(level);
									break;
								case SPELL_ITEMS_PARALYZE: // paralyze
									store_m_type = 9;
									cur_monst->charm(0,12,500);
									break;

								case SPELL_MAGE_SCRY_MONSTER: // monster info
									store_m_type = -1;
									play_sound(52);
									party.m_seen[cur_monst->number] = true;
									adjust_monst_menu();
									display_monst(0,cur_monst,0);
									break;
								case SPELL_MAGE_CAPTURE_SOUL: // Capture soul
									store_m_type = 15;
									cur_monst->record();
									break;

								case SPELL_MAGE_MINDDUEL: // Mindduel!
									store_m_type = -1;
									if ((cur_monst->m_d.mu == 0) && (cur_monst->m_d.cl == 0))
										add_string_to_buf("  Can't duel: no magic.");
										else {
											item = adven[current_pc].hasAbil(ITEM_SMOKY_CRYSTAL);
											if (item >= 24)
												add_string_to_buf("  You need a smoky crystal.   ");
												else {
													adven[current_pc].removeCharge(item);
													do_mindduel(current_pc,cur_monst);
													}
											}
									break;

								case 100 + SPELL_PRIEST_CHARM_FOE: // charm
									store_m_type = 14;
									cur_monst->charm(-1 * (bonus + adven[current_pc].level / 8),0,0);
									break;

								case 100 + SPELL_PRIEST_DISEASE: // disease
									store_m_type = 0;
									r1 = get_ran(1,0,1);
									cur_monst->disease(2 + r1 + bonus);
									break;

								case SPELL_ITEMS_STRENGHTEN_TARGET: //wand of Carrunos effect
									store_m_type = 14;
									cur_monst->m_d.health += 20;
									store_sound = 55;
									break;

								case SPELL_MAGE_DUMBFOUND: //Dumbfound
									store_m_type = 14;
									cur_monst->dumbfound(1 + bonus / 3);
									store_sound = 53;
									break;

								case SPELL_MAGE_SCARE: //Scare
									store_m_type = 11;
									cur_monst->scare(get_ran(2 + bonus,1,6));
									store_sound = 54;
									break;
								case SPELL_MAGE_FEAR: //Fear
									store_m_type = 11;
									cur_monst->scare(get_ran(min(20,adven[current_pc].level / 2 + bonus),1, 8));
									store_sound = 54;
									break;

								case SPELL_MAGE_SLOW: //Slow
									store_m_type = 11;
									r1 = get_ran(1,0,1);
									cur_monst->slow(2 + r1 + bonus);
									break;

								case SPELL_MAGE_MINOR_POISON: case SPELL_MAGE_VENOM_ARROWS: //Minor Poison - Venom Arrows
									store_m_type = (spell_being_cast == SPELL_MAGE_VENOM_ARROWS) ? 4 : 11;
									cur_monst->poison(2 + bonus / 2);
									store_sound = 55;
									break;
								case SPELL_MAGE_PARALYSIS: // Paralysis
									store_m_type = 9;
									cur_monst->charm(-10,12,1000);
									break;
								case SPELL_MAGE_POISON: //Poison
									store_m_type = 11;
									cur_monst->poison(4 + bonus / 2);
									store_sound = 55;
									break;
								case SPELL_MAGE_MAJOR_POISON: //Major Poison
									store_m_type = 11;
									cur_monst->poison(8 + bonus / 2);
									store_sound = 55;
									break;

								case 100 + SPELL_PRIEST_STUMBLE: // Stumble
									store_m_type = 8;
									cur_monst->curse(4 + bonus);
									break;

								case 100 + SPELL_PRIEST_CURSE: //Curse
									store_m_type = 8;
									cur_monst->curse(2 + bonus);
									break;

								case 100 + SPELL_PRIEST_HOLY_SCOURGE: //Holy Scourge
									store_m_type = 8;
									cur_monst->curse(2 + adven[current_pc].level / 2);
									break;

								case 100 + SPELL_PRIEST_TURN_UNDEAD: case 100 + SPELL_PRIEST_DISPEL_UNDEAD: //Turn Undead - Dispel Undead
									if (cur_monst->m_d.m_type != MONSTER_TYPE_UNDEAD) {
										add_string_to_buf("  Not undead.                    ");
										store_m_type = -1;
										break;
										}
									store_m_type = 8;
									r1 = get_ran(1,0,90);
									if (r1 > hit_chance[minmax(0,19,bonus * 2 + level * 4 - (cur_monst->m_d.level / 2) + 3)])
										add_string_to_buf("  Monster resisted.                  ");
										else {
										r1 = get_ran((spell_being_cast == 100 + SPELL_PRIEST_TURN_UNDEAD) ? 2 : 6, 1, 14);

										hit_space(cur_monst->m_loc,r1,4,0,current_pc);
										}
									break;

								case 100 + SPELL_PRIEST_RAVAGE_SPIRIT: //Ravage Spirit
									if (cur_monst->m_d.m_type != MONSTER_TYPE_DEMON) {
										add_string_to_buf("  Not a demon.                    ");
										store_m_type = -1;
										break;
										}
									r1 = get_ran(1,0,100);
									if (r1 > hit_chance[minmax(0,19,level * 4 - cur_monst->m_d.level + 10)])
										add_string_to_buf("  Demon resisted.                  ");
										else {
										r1 = get_ran(8 + bonus * 2, 1, 11);
										hit_space(cur_monst->m_loc,r1,4,0,current_pc);
										}
									break;
								}
							if (store_m_type >= 0)
								add_missile(target,store_m_type,1,
									14 * (cur_monst->m_d.x_width - 1),18 * (cur_monst->m_d.y_width - 1));

							}
					break;
					}
				}
			}
		}

	do_missile_anim((num_targets > 1) ? 35 : 60,pc_pos[current_pc],store_sound);

	// process mass damage
	for (i = 0; i < 8; i++)
		if (boom_dam[i] > 0)
			hit_space(boom_targ[i],boom_dam[i],boom_type[i],1,0);

	if (ashes_loc.x > 0)
		make_sfx(ashes_loc.x,ashes_loc.y,6);

	do_explosion_anim(5,0);

	end_missile_anim();

	handle_marked_damage();
	combat_posing_monster = current_working_monster = -1;

	print_buf();
}

void handle_marked_damage()
{
	int i;

	for (i = 0; i < NUM_OF_PCS; i++)
		if (pc_marked_damage[i] > 0)
			{
			adven[i].damage(pc_marked_damage[i],10,-1);
			pc_marked_damage[i] = 0;
			}

	for (i = 0; i < T_M; i++)
		if (monst_marked_damage[i] > 0)
			{
			damage_monst(i, current_pc, monst_marked_damage[i], 0, 9 );

			monst_marked_damage[i] = 0;
			}
}

void load_missile()
{
	short i,bow = 24,arrow = 24,thrown = 24,crossbow = 24,bolts = 24,no_ammo = 24;

	for (i = 0; i < 24; i++) {
		if ((adven[current_pc].equip[i] == true) &&
			(adven[current_pc].items[i].variety == ITEM_TYPE_THROWN_MISSILE))
				thrown = i;
		if ((adven[current_pc].equip[i] == true) &&
			(adven[current_pc].items[i].variety == ITEM_TYPE_BOW))
				bow = i;
		if ((adven[current_pc].equip[i] == true) &&
			(adven[current_pc].items[i].variety == ITEM_TYPE_ARROW))
				arrow = i;
		if ((adven[current_pc].equip[i] == true) &&
			(adven[current_pc].items[i].variety == ITEM_TYPE_CROSSBOW))
				crossbow = i;
		if ((adven[current_pc].equip[i] == true) &&
			(adven[current_pc].items[i].variety == ITEM_TYPE_BOLTS))
				bolts = i;
		if ((adven[current_pc].equip[i] == true) &&
			(adven[current_pc].items[i].variety == ITEM_TYPE_MISSILE_NO_AMMO))
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
	else if ((arrow < 24) && (bow < 24)) {
			missile_inv_slot = bow;
			ammo_inv_slot = arrow;
			overall_mode = MODE_FIRING;
			add_string_to_buf("Fire: Select a target.        ");
		add_string_to_buf("  (Hit 's' to cancel.)");
			current_spell_range = 12;
			if(adven[current_pc].items[arrow].ability == ITEM_MISSILE_EXPLODING && adven[current_pc].items[arrow].isIdent() == true)
            current_pat = radius2;
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

void fire_missile(location target)
{
	short r1, r2, skill, dam, dam_bonus, hit_bonus, range, targ_monst, spec_dam = 0,poison_amt = 0;
	short skill_item,m_type = 0;
	creature_data_type *cur_monst;
	Boolean exploding = false;

	skill = (overall_mode == MODE_FIRING) ? adven[current_pc].skills[SKILL_ARCHERY] : adven[current_pc].skills[SKILL_THROWN_MISSILES];
	range = (overall_mode == MODE_FIRING) ? 12 : 8;
	dam = adven[current_pc].items[ammo_inv_slot].item_level;
	dam_bonus = adven[current_pc].items[ammo_inv_slot].bonus + minmax(-8,8,(int)adven[current_pc].status[STATUS_BLESS_CURSE]);
	hit_bonus = (overall_mode == MODE_FIRING) ? adven[current_pc].items[missile_inv_slot].bonus : 0;
	hit_bonus += adven[current_pc].statAdj(SKILL_DEXTERITY) - can_see(pc_pos[current_pc],target,0)
		+ minmax(-8,8,(int) adven[current_pc].status[STATUS_BLESS_CURSE]);
	if ((skill_item = adven[current_pc].hasAbilEquip(ITEM_ACCURACY)) < 24) {
		hit_bonus += adven[current_pc].items[skill_item].ability_strength / 2;
		dam_bonus += adven[current_pc].items[skill_item].ability_strength / 2;
		}

	// race adj.
	if (adven[current_pc].race == RACE_NEPHIL)
		hit_bonus += 2;

   if (adven[current_pc].items[ammo_inv_slot].ability == ITEM_MISSILE_EXPLODING)
		exploding = true;

	if (dist(pc_pos[current_pc],target) > range)
		add_string_to_buf("  Out of range.");
		else if (can_see(pc_pos[current_pc],target,0) > 4)
			add_string_to_buf("  Can't see target.             ");
			else {
                // First, some missiles do special things
	               if (exploding == true) {
		           take_ap((overall_mode == MODE_FIRING) ? 3 : 2);
                   void_sanctuary(current_pc);
                   missile_firer = current_pc;
		           add_string_to_buf("  The arrow explodes!             ");
/*		           run_a_missile(pc_pos[current_pc],target,2,1,5,
			          0,0,100);*/
                   run_a_missile(pc_pos[current_pc],target,2,1,(overall_mode == MODE_FIRING) ? 12 : 14,
			          0,0,100);
                   if(PSD[SDF_GAME_SPEED] == 0)
                   pause(dist(pc_pos[current_pc],target));
                   else
			       pause(dist(pc_pos[current_pc],target)*5);
		           start_missile_anim();
		           add_missile(target,2,1, 0, 0);
//		           do_missile_anim(100,pc_pos[current_pc], 5); <= redundant missile anim
		           place_spell_pattern(radius2,target,
			          50 + adven[current_pc].items[ammo_inv_slot].ability_strength * 2,true,current_pc);
		           do_explosion_anim(5,0);
		           end_missile_anim();
		           handle_marked_damage();

		           }//end of exploding arrow

		           else{

				combat_posing_monster = current_working_monster = current_pc;
				draw_terrain(2);
				void_sanctuary(current_pc);
				//play_sound((overall_mode == MODE_FIRING) ? 12 : 14);
				take_ap((overall_mode == MODE_FIRING) ? 3 : 2);
				missile_firer = current_pc;
				r1 = get_ran(1,0,100) - 5 * hit_bonus - 10;
				r1 += 5 * (adven[current_pc].status[STATUS_WEBS] / 3);
				r2 = get_ran(1,1,dam) + dam_bonus;
				sprintf (create_line, "%s fires.",adven[current_pc].name); // debug
				add_string_to_buf(create_line);

				switch (overall_mode) {
					case MODE_THROWING:
						switch (adven[current_pc].items[ammo_inv_slot].item_level) {
							case 7:m_type = 10;break;
							case 4:m_type = 1;break;
							case 8:m_type = 5;break;
							case 9:m_type = 7;break;
							default:m_type = 10;break;
							}
						break;
					case MODE_FIRING: case MODE_FANCY_TARGET:
						m_type = (adven[current_pc].items[ammo_inv_slot].isMagic()) ? 4 : 3;
						break;
					}
				run_a_missile(pc_pos[current_pc],target,m_type,1,(overall_mode == MODE_FIRING) ? 12 : 14,
					0,0,100);

				if (r1 > hit_chance[skill])
					add_string_to_buf("  Missed.");
					else if ((targ_monst = monst_there(target)) < T_M) {
						cur_monst = &c_town.monst.dudes[targ_monst];
						spec_dam = calc_spec_dam(adven[current_pc].items[ammo_inv_slot].ability,
							adven[current_pc].items[ammo_inv_slot].ability_strength,cur_monst);
						if (adven[current_pc].items[ammo_inv_slot].ability == ITEM_MISSILE_HEAL_TARGET) {
							ASB("  There is a flash of light.");
							cur_monst->m_d.health += r2;
							}
							else damage_monst(targ_monst, current_pc, r2, spec_dam, 1300);

						// poison
						if ((adven[current_pc].status[STATUS_POISONED_WEAPON] > 0) && (adven[current_pc].weap_poisoned == ammo_inv_slot)) {
								poison_amt = adven[current_pc].status[STATUS_POISONED_WEAPON];
								if (adven[current_pc].hasAbilEquip(ITEM_POISON_AUGMENT) < 24)
									poison_amt++;
								cur_monst->poison(poison_amt);
							}
                        }
/*					   else if((targ_monst = pc_there(target)) <  6 && adven[current_pc].items[ammo_inv_slot].ability == ITEM_MISSILE_HEAL_TARGET){   //Heal Target missiles heal PC ?
						  	  ASB("  There is a flash of light.");                                                              //the idea is rejected for now (item is supposed to be cursed)
						      adven[targ_monst].heal(r2);
							}*/
						else hit_space(target,r2,0,1,0);
}

	if (adven[missile_firer].items[ammo_inv_slot].variety != ITEM_TYPE_MISSILE_NO_AMMO) {//in case someone has been killed and current_pc is not the firer anymore
		if (adven[missile_firer].items[ammo_inv_slot].ability != ITEM_MISSILE_RETURNING)
			adven[missile_firer].items[ammo_inv_slot].charges--;
        else
            adven[missile_firer].items[ammo_inv_slot].charges = 1;
		if ((adven[missile_firer].hasAbilEquip(ITEM_DRAIN_MISSILES) < 24) && (adven[missile_firer].items[ammo_inv_slot].ability != ITEM_MISSILE_RETURNING))
			adven[missile_firer].items[ammo_inv_slot].charges--;
		if (adven[missile_firer].items[ammo_inv_slot].charges <= 0)
			adven[missile_firer].takeItem(ammo_inv_slot);
		}

	}

	if(exploding == false){
    combat_posing_monster = current_working_monster = -1;
   	move_to_zero(adven[current_pc].status[STATUS_POISONED_WEAPON]);
    }
	print_buf();////
}

// Select next active PC and, if necessary, run monsters
// if monsters go or PC switches (i.e. if need redraw above), return true
Boolean combat_next_step()
{

	Boolean to_return = false;
	short store_pc; // will print current pc name is active pc changes

	store_pc = current_pc;
	while (pick_next_pc() == true) {
		combat_run_monst();
		set_pc_moves();
		if((combat_active_pc < 6) && (pc_moves[combat_active_pc] == 0)){
            combat_active_pc = 6;
            ASB(">The active character is unable to act!");
            ASB(">The whole party is now active.");
            }
		to_return = true;
		// Safety valve
		if (party_toast() == true)
			return true;
		}
	pick_next_pc();
	if (current_pc != store_pc)
		to_return = true;

    if(overall_mode == MODE_OUTDOORS) // has combat ended ? (stairway node)
        center = party.p_loc;
    else if (overall_mode == MODE_TOWN)
        center = c_town.p_loc;
    else
    	center = pc_pos[current_pc];

	adjust_spell_menus();

	if ((combat_active_pc == 6) && (current_pc != store_pc)) {
			sprintf(create_line, "Active:  %s (#%d, %d ap.)                     ",
				adven[current_pc].name,current_pc + 1,pc_moves[current_pc]);
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
Boolean pick_next_pc()
{
	Boolean store = false;

	if (current_pc == 6)
		current_pc = 0;

	// If current pc isn't active, fry its moves
	if ((combat_active_pc < 6) && (combat_active_pc != current_pc))
		pc_moves[current_pc] = 0;

	// Find next PC with moves
	while ((pc_moves[current_pc] <= 0) && (current_pc < 6)) {
		current_pc++;
		if ((combat_active_pc < 6) && (combat_active_pc != current_pc))
			pc_moves[current_pc] = 0;
		}

	// If run out of PC's, return to start and try again
	if (current_pc == 6) {
		current_pc = 0;
		while ((pc_moves[current_pc] <= 0) && (current_pc < 6)) {
			current_pc++;
			if ((combat_active_pc < 6) && (combat_active_pc != current_pc))
				pc_moves[current_pc] = 0;
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
	Boolean update_stat = false;


		monsters_going = true;
		do_monster_turn();
		monsters_going = false;

		process_fields();
		move_to_zero(party.light_level);
		if ((which_combat_type == 1) && (c_town.town.lighting == 2))
			party.light_level = max (0,party.light_level - 9);
		if (c_town.town.lighting == 3)
			party.light_level = 0;

		move_to_zero(party.stuff_done[SDF_DETECT_MONSTER]);
		move_to_zero(party.stuff_done[SDF_LAVAWALK]);

		// decrease monster present counter
		move_to_zero(party.stuff_done[SDF_MONSTERS_ALERTNESS]);

		dump_gold(1);

		party.age++;
		if (party.age % 4 == 0)
			for (i = 0; i < 6; i++) {
			if ((adven[i].status[STATUS_BLESS_CURSE] != 0) || (adven[i].status[STATUS_HASTE_SLOW] != 0))
				update_stat = true;
				move_to_zero(adven[i].status[STATUS_BLESS_CURSE]);
				move_to_zero(adven[i].status[STATUS_HASTE_SLOW]);
				move_to_zero(party.stuff_done[SDF_STEALTH]);
				if ((item = adven[i].hasAbilEquip(ITEM_REGENERATE)) < 24) {
					update_stat = true;
					adven[i].heal(get_ran(1,0,adven[i].items[item].item_level + 1));
					}
				}
		for (i = 0; i < 6; i++)
			if (adven[i].main_status == MAIN_STATUS_ALIVE) {
			if ((adven[i].status[STATUS_INVULNERABLE] != 0) || (adven[i].status[STATUS_MAGIC_RESISTANCE] != 0)
			 || (adven[i].status[STATUS_INVISIBLE] != 0)|| (adven[i].status[STATUS_MARTYRS_SHIELD] != 0)
			 || (adven[i].status[STATUS_ASLEEP] != 0)|| (adven[i].status[STATUS_PARALYZED] != 0))
				update_stat = true;

				move_to_zero(adven[i].status[STATUS_INVULNERABLE]);
				move_to_zero(adven[i].status[STATUS_MAGIC_RESISTANCE]);
				move_to_zero(adven[i].status[STATUS_INVISIBLE]);
				move_to_zero(adven[i].status[STATUS_MARTYRS_SHIELD]);
				move_to_zero(adven[i].status[STATUS_ASLEEP]);
				move_to_zero(adven[i].status[STATUS_PARALYZED]);

				// Do special items
				if (((item_level = adven[i].getProtLevel(ITEM_OCCASIONAL_HASTE)) > 0)
					&& (get_ran(1,0,10) == 5)) {
						update_stat = true;
						adven[i].status[STATUS_HASTE_SLOW] += item_level / 2;
						add_string_to_buf("An item hastes you!");
						}
				if ((item_level = adven[i].getProtLevel(ITEM_OCCASIONAL_BLESS)) > 0) {
					if (get_ran(1,0,10) == 5) {
						update_stat = true;
						adven[i].status[STATUS_BLESS_CURSE] += item_level / 2;
						add_string_to_buf("An item blesses you!");
						}
					}


			}

		timed_special_happened = special_increase_age(0);//don't delay the trigger of the special, if there's a special
		push_things();

		if (party.age % 2 == 0) do_poison();
		if (party.age % 3 == 0) handle_disease();
		handle_acid();

		if (update_stat == true)
		put_pc_screen();

}


void do_monster_turn()
{
	Boolean acted_yet, had_monst = false,printed_poison = false,printed_disease = false,printed_acid = false;
	Boolean redraw_not_yet_done = true;
	location targ_space,move_targ,l;
	short i,j,k,num_monst,target,r1,move_target;
	creature_data_type *cur_monst;
	Boolean pc_adj[6];
	short abil_range[40] = {0,6,8,8,10, 10,10,8,6,8, 6,0,0,0,6, 0,0,0,0,4, 10,0,0,6,0,
							0,0,0,0,0, 0,0,8,6,9, 0,0,0,0,0};
	short abil_odds[40] = {0,5,7,6,6, 5,5,6,6,6, 6,0,0,0,4, 0,0,0,0,4, 8,0,0,7,0,
							0,0,0,0,0, 0,0,7,5,6, 0,0,0,0,0};

	monsters_going = true; // This affects how graphics are drawn.

	num_monst = T_M;
	if (overall_mode < MODE_COMBAT)
		which_combat_type = 1;

	for (i = 0; i < num_monst; i++) {  // Give monsters ap's, check activity

		cur_monst = &c_town.monst.dudes[i];

		// See if hostile monster notices party, during combat
		if ((cur_monst->active == 1) && (cur_monst->attitude % 2 == 1) && (overall_mode == MODE_COMBAT)) {
			r1 = get_ran(1,1,100); // Check if see PCs first
			r1 += (party.stuff_done[SDF_STEALTH] > 0) ? 45 : 0;
			r1 += can_see(cur_monst->m_loc,closest_pc_loc(cur_monst->m_loc),0) * 10;
			if (r1 < 50)
				cur_monst->active = 2;

			for (j = 0; j < T_M; j++)
				if (monst_near(j,cur_monst->m_loc,5,1) == true) {
					cur_monst->active = 2;
					}
			}
		if ((cur_monst->active == 1) && (cur_monst->attitude % 2 == 1)) {
			// Now it looks for PC-friendly monsters
			// dist check is for efficiency
			for (j = 0; j < T_M; j++)
				if ((c_town.monst.dudes[j].active > 0) &&
					(c_town.monst.dudes[j].attitude % 2 != 1) &&
					(dist(cur_monst->m_loc,c_town.monst.dudes[j].m_loc) <= 6) &&
				  (can_see(cur_monst->m_loc,c_town.monst.dudes[j].m_loc,0) < 5))
					cur_monst->active = 2;
			}

		// See if friendly, fighting monster see hostile monster. If so, make mobile
		// dist check is for efficiency
		if ((cur_monst->active == 1) && (cur_monst->attitude == 2)) {
			for (j = 0; j < T_M; j++)
				if ((c_town.monst.dudes[j].active > 0) && (c_town.monst.dudes[j].attitude % 2 == 1) &&
		 		 (dist(cur_monst->m_loc,c_town.monst.dudes[j].m_loc) <= 6)
		 		 && (can_see(cur_monst->m_loc,c_town.monst.dudes[j].m_loc,0) < 5)) {
					cur_monst->active = 2;
					cur_monst->mobile = true;
					}
			}
		// End of seeing if monsters see others

		cur_monst->m_d.ap = 0;
		if (cur_monst->active == 2) { // Begin action loop for angry, active monsters
			// First note that hostile monsters are around.
			if (cur_monst->attitude % 2 == 1)
				party.stuff_done[SDF_MONSTERS_ALERTNESS] = 30;

			// Give monster its action points
			cur_monst->m_d.ap = cur_monst->m_d.speed;
			if (is_town())
				cur_monst->m_d.ap = max(1,cur_monst->m_d.ap / 3);
			if (party.age % 2 == 0)
				if (cur_monst->m_d.status[STATUS_HASTE_SLOW] < 0)
					cur_monst->m_d.ap = 0;
			if (cur_monst->m_d.ap > 0) { // adjust for webs (currently aslept/paralyzed monsters are cleaning webs as efficiently (i.e badly) as if awoken/free)
				cur_monst->m_d.ap = max(0,cur_monst->m_d.ap - cur_monst->m_d.status[STATUS_WEBS] / 2);
				if (cur_monst->m_d.ap == 0)
					cur_monst->m_d.status[STATUS_WEBS] = max(0,cur_monst->m_d.status[STATUS_WEBS] - 2);
				}
			if (cur_monst->m_d.status[STATUS_HASTE_SLOW] > 0)
				cur_monst->m_d.ap *= 2;
			}
			if ((cur_monst->m_d.status[STATUS_ASLEEP] > 0) || (cur_monst->m_d.status[STATUS_PARALYZED] > 0))
				cur_monst->m_d.ap = 0;
			if (in_scen_debug == true)
				cur_monst->m_d.ap = 0;
			center_on_monst = false;

			// Now take care of summoned monsters
			if (cur_monst->active > 0) {
				if ((cur_monst->summoned % 100) == 1) {
					cur_monst->active = 0;
					cur_monst->m_d.ap = 0;
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

		cur_monst = &c_town.monst.dudes[i];


		for (j = 0; j < 6; j++)
			if ((adven[j].isAlive()) && (monst_adjacent(pc_pos[j],i) == true))
				pc_adj[j] = true;
				else pc_adj[j] = false;



			while ((cur_monst->m_d.ap > 0) && (cur_monst->active > 0)) {  // Spend each action point

					if (is_combat()) { // Pick target. If in town, target already picked
					// in do_monsters
							target = monst_pick_target(i);
							target = switch_target_to_adjacent(i,target);
							if (target < 6)
								targ_space = pc_pos[target];
								else if (target != 6)
									targ_space = c_town.monst.dudes[target - 100].m_loc;
						monst_target[i] = target;
						}
						else {
							if (monst_target[i] < 6)
								targ_space = c_town.p_loc;
								else if (monst_target[i] != 6)
									targ_space = c_town.monst.dudes[monst_target[i] - 100].m_loc;
						}

		//			sprintf((char *)create_line,"  %d targets %d.",i,target);
		//			add_string_to_buf((char *) create_line);

					if ((monst_target[i] < 0) || ((monst_target[i] > 5) && (monst_target[i] < 100)))
						monst_target[i] = 6;
					target = monst_target[i];

				// Now if in town and monster about to attack, do a redraw, so we see monster
				// in right place
				if ((target != 6) && (is_town() == true) && (redraw_not_yet_done == true)
					&& (party_can_see_monst(i) == true)) {
					draw_terrain(0);
					redraw_not_yet_done = false;
					}

				// Draw w. monster in center, if can see
				if ((cur_monst->m_d.ap > 0) && (is_combat() == true)
					// First make sure it has a target and is close, if not, don't bother
					&& (cur_monst->attitude > 0) && (cur_monst->m_d.picture_num > 0)
					&& ((target != 6) || (cur_monst->attitude % 2 == 1))
					&& (party_can_see_monst(i) == true) ) {
					center_on_monst = true;
					center = cur_monst->m_loc;
					draw_terrain(0);
					pause((PSD[SDF_GAME_SPEED] == 3) ? 9 : PSD[SDF_GAME_SPEED]);
					}

					combat_posing_monster = current_working_monster = 100 + i;

					acted_yet = false;


					// Now the monster, if evil, looks at the situation and maybe picks a tactic.
					// This only happens when there is >1 a.p. left, and tends to involve
					// running to a nice position.
					current_monst_tactic = 0;
					if ((target != 6) && (cur_monst->m_d.ap > 1) && (futzing == 0)) {
						l = closest_pc_loc(cur_monst->m_loc);
						if (((cur_monst->m_d.mu > 0) || (cur_monst->m_d.cl > 0)) &&
						(dist(cur_monst->m_loc,l) < 5) && (monst_adjacent(l,i) == false))
							current_monst_tactic = 1; // this means flee


						if ( (((cur_monst->m_d.spec_skill > MONSTER_NO_SPECIAL_ABILITY) && (cur_monst->m_d.spec_skill < MONSTER_THROWS_ROCKS1))
							 || (cur_monst->m_d.spec_skill == MONSTER_GOOD_ARCHER)) && // Archer?
						(dist(cur_monst->m_loc,targ_space) < 6) &&
						 (monst_adjacent(targ_space,i) == false))
							current_monst_tactic = 1; // this means flee
						}


					// flee
					if ((monst_target[i] < 6) && (((cur_monst->m_d.morale <= 0)
						&& (cur_monst->m_d.spec_skill != MONSTER_MINDLESS) && (cur_monst->m_d.m_type != MONSTER_TYPE_UNDEAD))
						|| (current_monst_tactic == 1))) {
						if (cur_monst->m_d.morale < 0)
							cur_monst->m_d.morale++;
						if (cur_monst->m_d.health > 50)
							cur_monst->m_d.morale++;
						r1 = get_ran(1,1,6);
						if (r1 == 3)
							cur_monst->m_d.morale++;
/*crash*/			if ((adven[monst_target[i]].isAlive()) && (cur_monst->mobile == true)) {
							acted_yet = flee_party (i,cur_monst->m_loc,targ_space);
							if (acted_yet == true) take_m_ap(1,cur_monst);
							}
						}
					if ((target != 6) && (cur_monst->attitude > 0)
					 && (monst_can_see(i,targ_space) == true)
					 && (can_see_monst(targ_space,i) == true)) { // Begin spec. attacks


					// Breathe (fire)
						if ( (cur_monst->m_d.breath > 0)
							&& (get_ran(1,1,8) < 4) && (acted_yet == false)) {
								//print_nums(cur_monst->m_d.breath,cur_monst->m_d.breath_type,dist(cur_monst->m_loc,targ_space) );
								if ((target != 6)
									&& (dist(cur_monst->m_loc,targ_space) <= 8)) {
										acted_yet = monst_breathe(cur_monst,targ_space,cur_monst->m_d.breath_type);
									had_monst = true;
									acted_yet = true;
									take_m_ap(4,cur_monst);
									}
							}
					// Mage spell
						if ((cur_monst->m_d.mu > 0) && (get_ran(1,1,10) < ((cur_monst->m_d.cl > 0) ? 6 : 9) )
							&& (acted_yet == false)) {
								if (((monst_adjacent(targ_space,i) == false) || (get_ran(1,0,2) < 2) /*|| (cur_monst->number >= 160)*/
									|| (cur_monst->m_d.level > 9))
										&& (dist(cur_monst->m_loc,targ_space) <= 10)) {
											acted_yet = monst_cast_mage(cur_monst,target);
											had_monst = true;
											acted_yet = true;
											take_m_ap(5,cur_monst);
									}
							}
					// Priest spell
						if ((cur_monst->m_d.cl > 0) && (get_ran(1,1,8) < 7) && (acted_yet == false)) {
								if (((monst_adjacent(targ_space,i) == false) || (get_ran(1,0,2) < 2) || (cur_monst->m_d.level > 9))
										&& (dist(cur_monst->m_loc,targ_space) <= 10)) {
											acted_yet = monst_cast_priest(cur_monst,target);
											had_monst = true;
											acted_yet = true;
											take_m_ap(4,cur_monst);
									}
							}

				// Missile
						if ((abil_range[cur_monst->m_d.spec_skill] > 0) // breathing gas short range
							&& (get_ran(1,1,8) < abil_odds[cur_monst->m_d.spec_skill]) && (acted_yet == false)) {
								// Don't fire when adjacent, unless non-gaze magical attack
								if (((monst_adjacent(targ_space,i) == false) ||
									((cur_monst->m_d.spec_skill > MONSTER_THROWS_RAZORDISKS) && (cur_monst->m_d.spec_skill != MONSTER_GOOD_ARCHER)
									&& (cur_monst->m_d.spec_skill != MONSTER_ACID_SPIT)))
									// missile range
									&& (dist(cur_monst->m_loc,targ_space) <= abil_range[cur_monst->m_d.spec_skill])) {
									print_monst_name(cur_monst->number);
									monst_fire_missile(i,cur_monst->m_d.status[STATUS_BLESS_CURSE],
										cur_monst->m_d.spec_skill,cur_monst->m_loc,target);

									// Vapors don't count as action
									if ((cur_monst->m_d.spec_skill == MONSTER_THROWS_DARTS) || (cur_monst->m_d.spec_skill == MONSTER_THROWS_RAZORDISKS) ||
									  (cur_monst->m_d.spec_skill == MONSTER_GOOD_ARCHER))
										take_m_ap(2,cur_monst);
										else if (cur_monst->m_d.spec_skill == MONSTER_HEAT_RAY)
											take_m_ap(1,cur_monst);
											else take_m_ap(3,cur_monst);
									had_monst = true;
									acted_yet = true;
									}
							}
						} // Special attacks

					// Attack pc
					if ((monst_target[i] < 6) && (adven[monst_target[i]].isAlive())
						&& (monst_adjacent(targ_space,i) == true)  && (cur_monst->attitude % 2 == 1)
						 && (acted_yet == false)) {
							monster_attack_pc(i,monst_target[i]);
							take_m_ap(4,cur_monst);
							acted_yet = true;
							had_monst = true;
						}
					// Attack monst
					if ((monst_target[i] >= 100) && (c_town.monst.dudes[monst_target[i] - 100].active > 0)
						&& (monst_adjacent(targ_space,i) == true)  && (cur_monst->attitude > 0)
						 && (acted_yet == false)) {
							monster_attack_monster(i,monst_target[i] - 100);
							take_m_ap(4,cur_monst);
							acted_yet = true;
							had_monst = true;
						}

					if (acted_yet == true) {
							print_buf();
							if (j == 0)
								pause(8);
							FlushEvents(2);
						}

					if (overall_mode == MODE_COMBAT) {
						if ((acted_yet == false) && (cur_monst->mobile == true)) {  // move monst
							move_target = (monst_target[i] != 6) ? monst_target[i] : closest_pc(cur_monst->m_loc);
							if (monst_hate_spot(i,&move_targ) == true) // First, maybe move out of dangerous space
								seek_party (i,cur_monst->m_loc,move_targ);
								else { // spot is OK, so go nuts
								if ((cur_monst->attitude % 2 == 1) && (move_target < 6)) // Monsters seeking party do so
									if (adven[move_target].isAlive()) {
										seek_party (i,cur_monst->m_loc,pc_pos[move_target]);
										for (k = 0; k < 6; k++)
											if ((pc_parry[k] > 99) && (monst_adjacent(pc_pos[k],i) == true)
											&& (cur_monst->active > 0)) {
												pc_parry[k] = 0;
												pc_attack(k,i);
												}
											}

								if (move_target >= 100) // Monsters seeking monsters do so
									if (c_town.monst.dudes[move_target - 100].active > 0) {
										seek_party (i,cur_monst->m_loc,c_town.monst.dudes[move_target - 100].m_loc);
										for (k = 0; k < 6; k++)
											if ((pc_parry[k] > 99) && (monst_adjacent(pc_pos[k],i) == true)
												&& (cur_monst->active > 0) && (cur_monst->attitude % 2 == 1)) {
												pc_parry[k] = 0;
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
						if ((acted_yet == false) && (cur_monst->mobile == false)) { // drain action points
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
							if ((adven[k].isAlive()) && (monst_adjacent(pc_pos[k],i) == false)
								&& (pc_adj[k] == true) && (cur_monst->attitude % 2 == 1) && (cur_monst->active > 0) &&
								(adven[k].status[STATUS_INVISIBLE] == 0)) {
									combat_posing_monster = current_working_monster = k;
									pc_attack(k,i);
									combat_posing_monster = current_working_monster = 100 + i;
									pc_adj[k] = false;
									}

				// Place fields for monsters that create them. Only done when monst sees foe
				if ((target != 6) && (can_see(cur_monst->m_loc,targ_space,0) < 5)) { ////
					if ((cur_monst->m_d.radiate_1 == MONSTER_RADIATE_FIRE_FIELDS) && (get_ran(1,1,100) <= cur_monst->m_d.radiate_2))
						place_spell_pattern(square,cur_monst->m_loc,5,false,7);
					if ((cur_monst->m_d.radiate_1 == MONSTER_RADIATE_ICE_FIELDS) && (get_ran(1,1,100) <= cur_monst->m_d.radiate_2))
						place_spell_pattern(square,cur_monst->m_loc,8,false,7);
					if ((cur_monst->m_d.radiate_1 == MONSTER_RADIATE_SHOCK_FIELDS) && (get_ran(1,1,100) <= cur_monst->m_d.radiate_2))
						place_spell_pattern(square,cur_monst->m_loc,4,false,7);
					if ((cur_monst->m_d.radiate_1 == MONSTER_RADIATE_ANTIMAGIC_FIELDS) && (get_ran(1,1,100) <= cur_monst->m_d.radiate_2))
						place_spell_pattern(square,cur_monst->m_loc,6,false,7);
					if ((cur_monst->m_d.radiate_1 == MONSTER_RADIATE_SLEEP_FIELDS) && (get_ran(1,1,100) <= cur_monst->m_d.radiate_2))
						place_spell_pattern(square,cur_monst->m_loc,12,false,7);
					if ((cur_monst->m_d.radiate_1 == MONSTER_RADIATE_STINKING_CLOUDS) && (get_ran(1,1,100) <= cur_monst->m_d.radiate_2))
						place_spell_pattern(square,cur_monst->m_loc,7,false,7);
					if ((cur_monst->m_d.radiate_1 == MONSTER_SUMMON1) && (get_ran(1,0,99) < 5)){
						if (summon_monster(cur_monst->m_d.radiate_2,
							cur_monst->m_loc,130,cur_monst->attitude) == true)
							{monst_spell_note(cur_monst->number,33); play_sound(61);}
						}
					if ((cur_monst->m_d.radiate_1 == MONSTER_SUMMON2) && (get_ran(1,0,99) < 20)){
						if (summon_monster(cur_monst->m_d.radiate_2,
							cur_monst->m_loc,130,cur_monst->attitude) == true)
							{monst_spell_note(cur_monst->number,33); play_sound(61);}
						}
					if ((cur_monst->m_d.radiate_1 == MONSTER_SUMMON3) && (get_ran(1,0,99) < 50)){
						if (summon_monster(cur_monst->m_d.radiate_2,
							cur_monst->m_loc,130,cur_monst->attitude) == true)
							{monst_spell_note(cur_monst->number,33); play_sound(61);}
						}
					}

				combat_posing_monster = current_working_monster = -1;
				// Redraw monster after it goes
				if ((cur_monst->attitude > 0) && (cur_monst->active > 0) && (cur_monst->m_d.ap == 0)
				 && (is_combat()) && (cur_monst->m_d.picture_num > 0) && (party_can_see_monst(i)  == true)) {
					center = cur_monst->m_loc;
					draw_terrain(0);

					}

				// If monster dead, take away actions
				if (cur_monst->active == 0)
					cur_monst->m_d.ap = 0;

				//if ((futzing == 1) && (get_ran(1,0,1) == 0)) // If monster's just pissing around, give up
				//	cur_monst->m_d.ap = 0;
				if (futzing > 1) // If monster's just pissing around, give up
					cur_monst->m_d.ap = 0;
				}  // End of monster action loop


		}

	for (i = 0; i < num_monst; i++) {  // Begin monster time stuff loop
		// If party dead, no point
		if (party_toast() == true)
			return;

		cur_monst = &c_town.monst.dudes[i];

		if ((cur_monst->active < 0) || (cur_monst->active > 2))
			cur_monst->active = 0; // clean up
				if (cur_monst->active != 0) { // Take care of monster effects
						if (cur_monst->m_d.status[STATUS_ACID] > 0) {  // Acid
							if (printed_acid == false) {
								add_string_to_buf("Acid:              ");
								printed_acid = true;
								}
							r1 = get_ran(cur_monst->m_d.status[STATUS_ACID],1,6);
							damage_monst(i, 6,r1, 0, 3);
							cur_monst->m_d.status[STATUS_ACID]--;
							}

						if (cur_monst->m_d.status[STATUS_ASLEEP] == 1)
							monst_spell_note(cur_monst->number,29);
						move_to_zero(cur_monst->m_d.status[STATUS_ASLEEP]);
						move_to_zero(cur_monst->m_d.status[STATUS_PARALYZED]);

						if (party.age % 2 == 0) {
							move_to_zero(cur_monst->m_d.status[STATUS_BLESS_CURSE]);
							move_to_zero(cur_monst->m_d.status[STATUS_HASTE_SLOW]);
							move_to_zero(cur_monst->m_d.status[STATUS_WEBS]);

							if (cur_monst->m_d.status[STATUS_POISON] > 0) {  // Poison
								if (printed_poison == false) {
									add_string_to_buf("Poisoned monsters:              ");
									printed_poison = true;
									}
								r1 = get_ran(cur_monst->m_d.status[STATUS_POISON],1,6);
								damage_monst(i, 6, r1, 0, 2);
								cur_monst->m_d.status[STATUS_POISON]--;
								}
							if (cur_monst->m_d.status[STATUS_DISEASE] > 0) {  // Disease
								if (printed_disease == false) {
									add_string_to_buf("Diseased monsters:              ");
									printed_disease = true;
									}
								k = get_ran(1,1,5);
								switch (k) {
									case 1:
									case 2: cur_monst->poison(2);break;
									case 3:	cur_monst->slow(2); break;
									case 4: cur_monst->curse(2); break;
									case 5: cur_monst->scare(10); break;
									}
								if (get_ran(1,1,6) < 4)
									cur_monst->m_d.status[STATUS_DISEASE]--;
								}

							}

						if (party.age % 4 == 0) {
							if (cur_monst->m_d.mp < cur_monst->m_d.max_mp)
								cur_monst->m_d.mp += 2;
							move_to_zero(cur_monst->m_d.status[STATUS_DUMB]);
							}
					} // end take care of monsters
		}

	// If in town, need to restore center
	if (overall_mode < MODE_COMBAT)
		center = c_town.p_loc;
	if (had_monst == true)
		put_pc_screen();
	for (i = 0; i < 6; i++)
		pc_parry[i] = 0;

	monsters_going = false;
}

void monster_attack_pc(short who_att,short target)
{
	creature_data_type *attacker;
	short r1,r2,i,dam_type = 0,store_hp,sound_type = 0;


	attacker = &c_town.monst.dudes[who_att];

	// A peaceful monster won't attack
	if (attacker->attitude % 2 != 1)
		return;

	// Draw attacker frames
	if ((is_combat())
		&& ((center_on_monst == true) || (monsters_going == false))) {
		if (attacker->m_d.spec_skill != MONSTER_INVISIBLE)
		frame_space(attacker->m_loc,0,attacker->m_d.x_width,attacker->m_d.y_width);
		frame_space(pc_pos[target],1,1,1);
		}



	if ((attacker->m_d.a[0] != 0) || (attacker->m_d.a[2] != 0))
			print_monst_attacks(attacker->number,target);

	// Check sanctuary
	if (adven[target].status[STATUS_INVISIBLE] > 0) {
		r1 = get_ran(1,0,100);
		if (r1 > hit_chance[attacker->m_d.level / 2]) {
			add_string_to_buf("  Can't find target!                 ");
			}
		return;
		}

	for (i = 0; i < 3; i++) {
		if ((attacker->m_d.a[i] > 0) && (adven[target].isAlive())) {

			// Attack roll
			r1 = get_ran(1,0,100) - 5 * min(8,attacker->m_d.status[STATUS_BLESS_CURSE]) + 5 * adven[target].status[STATUS_BLESS_CURSE]
					+ 5 * adven[target].statAdj(SKILL_DEXTERITY) - 15;
			r1 += 5 * (attacker->m_d.status[STATUS_WEBS] / 3);
			if (pc_parry[target] < 100)
				r1 += 5 * pc_parry[target];

			// Damage roll
			r2 = get_ran(attacker->m_d.a[i] / 100 + 1,1,attacker->m_d.a[i] % 100)
				+ min(8,attacker->m_d.status[STATUS_BLESS_CURSE]) - adven[target].status[STATUS_BLESS_CURSE] + 1;
			if (difficulty_adjust() > 2)
				r2 = r2 * 2;
			if (difficulty_adjust() == 2)
				r2 = (r2 * 3) / 2;

			if ((adven[target].status[STATUS_ASLEEP] > 0) || (adven[target].status[STATUS_PARALYZED] > 0)) {
				r1 -= 80;
				r2 = r2 * 2;
				}

			draw_terrain(2);
			// Check if hit, and do effects
			if (r1 <= hit_chance[(attacker->m_d.skill + 4) / 2]) {
					if (attacker->m_d.m_type == MONSTER_TYPE_DEMON)
						dam_type = DAMAGE_DEMON;
					if (attacker->m_d.m_type == MONSTER_TYPE_UNDEAD)
						dam_type = DAMAGE_UNDEAD;

					store_hp = adven[target].cur_health;
					sound_type = get_monst_sound(attacker,i);

					if ((adven[target].damage(r2,sound_type * 100 + 30 + dam_type,
						attacker->m_d.m_type) == true) &&
						(store_hp - adven[target].cur_health > 0)) {
						damaged_message(store_hp - adven[target].cur_health,
						 (i > 0) ? attacker->m_d.a23_type : attacker->m_d.a1_type);

						if (adven[target].status[STATUS_MARTYRS_SHIELD] > 0) {
							add_string_to_buf("  Shares damage!                 ");
							damage_monst(who_att, 6, store_hp - adven[target].cur_health, 0, 3);
							}

						if ((attacker->m_d.poison > 0) && (i == 0)) {
								adven[target].poison(attacker->m_d.poison);
							}

						// Gremlin
						if ((attacker->m_d.spec_skill == MONSTER_STEALS_FOOD) && (get_ran(1,0,2) < 2)) {
							add_string_to_buf("  Steals food!                 ");
							print_buf();
							play_sound(26);
							party.food = (long) max(0, (short) (party.food) - get_ran(1,0,10) - 10);
							put_pc_screen();
							}

						// Disease
						if ((attacker->m_d.spec_skill == MONSTER_DISEASE_TOUCH)  && (get_ran(1,0,2) < 2)) {
							add_string_to_buf("  Causes disease!                 ");
							print_buf();
							adven[target].disease((attacker->m_d.spec_skill == 25) ? 6 : 2);
							}

						// Undead xp drain
						if (((attacker->m_d.spec_skill == MONSTER_XP_DRAINING_TOUCH) || (attacker->m_d.spec_skill == MONSTER_ICY_AND_DRAINING_TOUCH))
						  && (adven[target].hasAbilEquip(ITEM_LIFE_SAVING) == 24)) {
							add_string_to_buf("  Drains life!                 ");
							adven[target].drainXP((attacker->m_d.level * 3) / 2);
							put_pc_screen();
							}

						// Undead slow
						if ((attacker->m_d.spec_skill == MONSTER_SLOWING_TOUCH) && (get_ran(1,0,8) < 6)
							&& (adven[target].hasAbilEquip(ITEM_LIFE_SAVING) == 24)) {
							add_string_to_buf("  Stuns! ");
							adven[target].slow(2);
							put_pc_screen();
							}
						// Dumbfound target
						if (attacker->m_d.spec_skill == MONSTER_DUMBFOUNDING_TOUCH) {
							add_string_to_buf("  Dumbfounds! ");
							adven[target].dumbfound(2);
							put_pc_screen();
							}

						// Web target
						if (attacker->m_d.spec_skill == MONSTER_WEB_TOUCH) {
							add_string_to_buf("  Webs!                    ");
							adven[target].web(5);
							put_pc_screen();
							}
						// Sleep target
						if (attacker->m_d.spec_skill == MONSTER_SLEEP_TOUCH) {
							add_string_to_buf("  Sleeps!                    ");
							adven[target].sleep(6,11,-15);
							put_pc_screen();
							}
						// Paralyze target
						if (attacker->m_d.spec_skill == MONSTER_PARALYSIS_TOUCH) {
							add_string_to_buf("  Paralysis touch!                    ");
							adven[target].sleep(500,12,-5);
							put_pc_screen();
							}
						// Acid touch
						if (attacker->m_d.spec_skill == MONSTER_ACID_TOUCH)  {
							add_string_to_buf("  Acid touch!      ");
							adven[target].acid((attacker->m_d.level > 20) ? 4 : 2);
							}

						// Freezing touch
						if (((attacker->m_d.spec_skill == MONSTER_ICY_TOUCH) || (attacker->m_d.spec_skill == MONSTER_ICY_AND_DRAINING_TOUCH))
						 && (get_ran(1,0,8) < 6) && (adven[target].hasAbilEquip(ITEM_LIFE_SAVING) == 24)) {
							add_string_to_buf("  Freezing touch!");
							r1 = get_ran(3,1,10);
							adven[target].damage(r1, DAMAGE_COLD, -1);
							}
						// Killing touch
						if (attacker->m_d.spec_skill == MONSTER_DEATH_TOUCH)
							 {
							add_string_to_buf("  Killing touch!");
							r1 = get_ran(20,1,10);
							adven[target].damage(r1,DAMAGE_UNBLOCKABLE,-1);
							}
						// Petrification touch
                        if ((attacker->m_d.spec_skill == MONSTER_PETRIFYING_TOUCH) && (get_ran(1,0,8) < 3))
                           {
                           add_string_to_buf("  Petrifying touch!");
                           print_buf();
                           adven[target].kill(4);
                           }
                     }
				}
				else {
					sprintf (create_line, "  Misses.");
					add_string_to_buf(create_line);
					play_sound(2);
				}
		combat_posing_monster = -1;
		draw_terrain(2);
		combat_posing_monster = 100 + who_att;


		}
		if (adven[target].isAlive() == false)
			i = 3;
	}

}

void monster_attack_monster(short who_att,short attackee)
{
	creature_data_type *attacker,*target;
	short r1,r2,i,dam_type = 0,store_hp,sound_type = 0;

	attacker = &c_town.monst.dudes[who_att];
	target = &c_town.monst.dudes[attackee];

	// Draw attacker frames
	if ((is_combat())
		&& ((center_on_monst == true) || (monsters_going == false))) {
		if (attacker->m_d.spec_skill != MONSTER_INVISIBLE)
		frame_space(attacker->m_loc,0,attacker->m_d.x_width,attacker->m_d.y_width);
		frame_space(target->m_loc,1,1,1);
		}


	if ((attacker->m_d.a[1] != 0) || (attacker->m_d.a[0] != 0))
			print_monst_attacks(attacker->number,100 + attackee);
	for (i = 0; i < 3; i++) {
		if ((attacker->m_d.a[i] > 0) && (target->active != 0)) {
//			sprintf ((char *) create_line, "  Attacks %s.",(char *) adven[target].name);
//			add_string_to_buf((char *) create_line);

			// if friendly to party, make able to attack
			if (target->attitude == 0)
				target->attitude = 2;

			// Attack roll
			r1 = get_ran(1,0,100) - 5 * min(10,attacker->m_d.status[STATUS_BLESS_CURSE])
				+ 5 * target->m_d.status[STATUS_BLESS_CURSE]	- 15;
			r1 += 5 * (attacker->m_d.status[STATUS_WEBS] / 3);

			// Damage roll
			r2 = get_ran(attacker->m_d.a[i] / 100 + 1,1,attacker->m_d.a[i] % 100)
				+ min(10,attacker->m_d.status[STATUS_BLESS_CURSE]) - target->m_d.status[STATUS_BLESS_CURSE] + 2;

			if ((target->m_d.status[STATUS_ASLEEP] > 0) || (target->m_d.status[STATUS_PARALYZED] > 0)) {
				r1 -= 80;
				r2 = r2 * 2;
				}

			draw_terrain(2);
			// Check if hit, and do effects
			if (r1 <= hit_chance[(attacker->m_d.skill + 4) / 2]) {
					if (attacker->m_d.m_type == MONSTER_TYPE_DEMON)
						dam_type = DAMAGE_DEMON;
					if (attacker->m_d.m_type == MONSTER_TYPE_UNDEAD)
						dam_type = DAMAGE_UNDEAD;
					store_hp = target->m_d.health;

					sound_type = get_monst_sound(attacker,i);

					if (damage_monst(attackee,7,r2,0,sound_type * 100 + 10 + dam_type) == true) {
						damaged_message(store_hp - target->m_d.health,
						 (i > 0) ? attacker->m_d.a23_type : attacker->m_d.a1_type);

						if ((attacker->m_d.poison > 0) && (i == 0)) {
								target->poison(attacker->m_d.poison);
							}

						// Undead slow
						if ((attacker->m_d.spec_skill == MONSTER_SLOWING_TOUCH) && (get_ran(1,0,8) < 6)) {
							add_string_to_buf("  Stuns!      ");
							target->slow(2);
							}

						// Web target
						if (attacker->m_d.spec_skill == MONSTER_WEB_TOUCH)  {
							add_string_to_buf("  Webs!      ");
							target->web(4);
							}
						// Sleep target
						if (attacker->m_d.spec_skill == MONSTER_SLEEP_TOUCH)  {
							add_string_to_buf("  Sleeps!      ");
							target->charm(-15,11,6);
							}
						// Dumbfound target
						if (attacker->m_d.spec_skill == MONSTER_DUMBFOUNDING_TOUCH)  {
							add_string_to_buf("  Dumbfounds!      ");
							target->dumbfound(2);
							}
						// Paralyze target
						if (attacker->m_d.spec_skill == MONSTER_PARALYSIS_TOUCH)  {
							add_string_to_buf("  Paralysis touch!      ");
							target->charm(-5,12,500);
							}
						// Acid touch
						if (attacker->m_d.spec_skill == MONSTER_ACID_TOUCH)  {
							add_string_to_buf("  Acid touch!      ");
							target->acid(3);
							}

						// Freezing touch
						if (((attacker->m_d.spec_skill == MONSTER_ICY_TOUCH) || (attacker->m_d.spec_skill == MONSTER_ICY_AND_DRAINING_TOUCH))
						 && (get_ran(1,0,8) < 6)) {
							add_string_to_buf("  Freezing touch!");
							r1 = get_ran(3,1,10);
							damage_monst(attackee,7,r1,0,5);
							}

						// Death touch
						if ((attacker->m_d.spec_skill == MONSTER_DEATH_TOUCH)
						 && (get_ran(1,0,8) < 6)) {
							add_string_to_buf("  Killing touch!");
							r1 = get_ran(20,1,10);
							damage_monst(attackee,7,r1,0,4);
							}
						}
				}
				else {
					sprintf (create_line, "  Misses.");
					add_string_to_buf(create_line);
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

//level = spec_skill
void monst_fire_missile(short m_num,short bless,short level,location source,short target)
//short target; // 100 +  - monster is target
{
	creature_data_type *m_target;
	short r1,r2,dam[40] = {0,1,2,3,4, 6,8,7,0,0, 0,0,0,0,0, 0,0,0,0,0,
		8,0,0,0,0, 0,0,0,0,0, 0,0,0,0,6, 0,0,0,0,0},i,j;
	location targ_space;

	if (target == INVALID_PC)
		return;
	if (target >= 100) {
		targ_space = c_town.monst.dudes[target - 100].m_loc;
		if (c_town.monst.dudes[target - 100].active == 0)
			return;
		}
		else {
			targ_space = (is_combat()) ? pc_pos[target] : c_town.p_loc;
			if (adven[target].isAlive() == false)
				return;
			}

	if (target >= 100)
		m_target = &c_town.monst.dudes[target - 100];
	if (((overall_mode >= MODE_COMBAT) && (overall_mode <= MODE_TALKING)) && (center_on_monst == true)) {
		frame_space(source,0,c_town.monst.dudes[m_num].m_d.x_width,c_town.monst.dudes[m_num].m_d.y_width);
		if (target >= 100)
			frame_space(targ_space,1,m_target->m_d.x_width,m_target->m_d.y_width);
			else frame_space(targ_space,1,1,1);
		}

	draw_terrain(2);
	if (level == MONSTER_BREATHES_SLEEP_CLOUDS) { // sleep cloud
		ASB("Creature breathes.");
		run_a_missile(source,targ_space,0,0,44,
			0,0,100);
		place_spell_pattern(radius2,targ_space,12,false,7);
		}
	else if (level == MONSTER_BREATHES_STINKING_CLOUDS) { // vapors
			if (target < 100) { // on PC
				sprintf (create_line, "  Breathes on %s.                  ", adven[target].name);
				add_string_to_buf(create_line);
				}
				else {  // on monst
					add_string_to_buf("  Breathes vapors.");
					}
			run_a_missile(source,targ_space,12,0,44,
				0,0,100);
			scloud_space(targ_space.x,targ_space.y);
		}
	else if (level == MONSTER_SHOOTS_WEB) { // webs
			if (target < 100) { // on PC
				sprintf (create_line, "  Throws web at %s.                  ", adven[target].name);
				add_string_to_buf(create_line);
				}
				else {  // on monst
					add_string_to_buf("  Throws web.");
					}
			run_a_missile(source,targ_space,8,0,14,
				0,0,100);
			web_space(targ_space.x,targ_space.y);
		}
	else if (level == MONSTER_PARALYSIS_RAY) { // paral
			play_sound(51);
			if (target < 100) { // on PC
				sprintf (create_line, "  Fires ray at %s.                  ", adven[target].name);
				add_string_to_buf(create_line);
				adven[target].sleep(100,12,0);
				}
				else {  // on monst
					add_string_to_buf("  Shoots a ray.");
					m_target->charm(0,12,100);
					}
		}
		else if (level == MONSTER_PETRIFICATION_RAY) { // petrify
			run_a_missile(source,targ_space,14,0,43,0,0,100);
			if (target < 100) { // on PC
				sprintf (create_line, "  Gazes at %s.                  ", adven[target].name);
				add_string_to_buf(create_line);
				r1 = get_ran(1,0,20) + adven[target].level / 4 + adven[target].status[STATUS_BLESS_CURSE];
			if (r1 > 14) {
						sprintf (create_line, "  %s resists.                  ", adven[target].name);
						add_string_to_buf(create_line);
					}
					else {
						adven[target].kill(4);
						}
				}
				else {
					monst_spell_note(m_target->number,9);
					r1 = get_ran(1,0,20) + m_target->m_d.level / 4 + m_target->m_d.status[STATUS_BLESS_CURSE];
					if ((r1 > 14) || (m_target->m_d.immunities & 2))//check for magic immunity
						monst_spell_note(m_target->number,10);
						else {
							monst_spell_note(m_target->number,8);
							kill_monst(m_target,7);
							}
					}
		}
		else if (level == MONSTER_SP_DRAIN_RAY) { // Drain sp
			if (target < 100) { // pc
				// modify target is target has no sp
				if (adven[target].cur_sp < 4) {
					for (i = 0; i < 8; i++) {
						j = get_ran(1,0,5);
						if ((adven[j].isAlive()) && (adven[j].cur_sp > 4) &&
							(can_see(source,pc_pos[j],0) < 5) && (dist(source,pc_pos[j]) <= 8)) {
								target = j;
								i = 8;
								targ_space = pc_pos[target];
								}
						}

					}
				run_a_missile(source,targ_space,8,0,43,0,0,100);
				sprintf (create_line, "  Drains %s.                  ", adven[target].name);
				add_string_to_buf(create_line);
				adven[target].cur_sp = adven[target].cur_sp / 2;
				}
				else { // on monst
				run_a_missile(source,targ_space,8,0,43,0,0,100);
					monst_spell_note(m_target->number,11);
					if (m_target->m_d.mp >= 4)
						m_target->m_d.mp = m_target->m_d.mp / 2;
						else m_target->m_d.skill = 1;
					}
			}
		else if (level == MONSTER_HEAT_RAY) { // heat ray
			run_a_missile(source,targ_space,13,0,51,0,0,100);
			r1 = get_ran(7,1,6);
			start_missile_anim();
			if (target < 100) { // pc
				sprintf (create_line, "  Hits %s with heat ray.", adven[target].name);
				add_string_to_buf(create_line);
				adven[target].damage(r1,DAMAGE_FIRE,-1);
				}
				else { // on monst
					add_string_to_buf("  Fires heat ray.");
					damage_monst(target - 100,7,r1,0,1);
					}
			do_explosion_anim(5,0);
			end_missile_anim();
			handle_marked_damage();
			}
		else if (level == MONSTER_ACID_SPIT) { // acid spit
			run_a_missile(source,targ_space,0,1,64,0,0,100);
			if (target < 100) { // pc
				sprintf (create_line, "  Spits acid on %s.", adven[target].name);
				add_string_to_buf(create_line);
				adven[target].acid(6);
				}
				else { // on monst
					add_string_to_buf("  Spits acid.");
					m_target->acid(6);
					}
			}
		else if (target < 100) {  // missile on PC

			switch (level) {
				case MONSTER_THROWS_DARTS: case MONSTER_SHOOTS_ARROWS: case MONSTER_GOOD_ARCHER:
					run_a_missile(source,targ_space,3,1,12,0,0,100);
					sprintf (create_line, "  Shoots at %s.", adven[target].name);
				break;
				case MONSTER_THROWS_SPEARS:
					run_a_missile(source,targ_space,5,1,14,0,0,100);
					sprintf (create_line, "  Throws spear at %s.", adven[target].name);
				break;
				case MONSTER_THROWS_RAZORDISKS:
					run_a_missile(source,targ_space,7,1,14,0,0,100);
					sprintf (create_line, "  Throws razordisk at %s.", adven[target].name);
				break;
				case MONSTER_SHOOTS_SPINES:
					run_a_missile(source,targ_space,5,1,14,0,0,100);
					sprintf (create_line, "  Fires spines at %s.", adven[target].name);
				break;
				default://rock throwing
					run_a_missile(source,targ_space,12,1,14,0,0,100);
					sprintf (create_line, "  Throws rock at %s.", adven[target].name);
				break;
				}

			add_string_to_buf(create_line);

			// Check sanctuary
			if (adven[target].status[STATUS_INVISIBLE] > 0) {
				r1 = get_ran(1,0,100);
				if (r1 > hit_chance[level]) {
					add_string_to_buf("  Can't find target!                 ");
					}
				return;
				}

			r1 = get_ran(1,0,100) - 5 * min(10,bless) + 5 * adven[target].status[STATUS_BLESS_CURSE]
				- 5 * (can_see(source, pc_pos[target],0));
			if (pc_parry[target] < 100)
				r1 += 5 * pc_parry[target];
			r2 = get_ran(dam[level],1,7) + min(10,bless);

			if (r1 <= hit_chance[dam[level] * 2]) {
					if (adven[target].damage(r2,1300,-1) == true) {	}
				}
				else {
					sprintf (create_line, "  Misses %s.", adven[target].name);
					add_string_to_buf(create_line);
					}

		}
		else { // missile on monst
			switch (level) {
				case MONSTER_THROWS_DARTS: case MONSTER_SHOOTS_ARROWS: case MONSTER_GOOD_ARCHER:
					run_a_missile(source,targ_space,3,1,12,0,0,100);
					monst_spell_note(m_target->number,12);
				break;
				case MONSTER_THROWS_SPEARS:
					run_a_missile(source,targ_space,5,1,14,0,0,100);
					monst_spell_note(m_target->number,13);
				break;
				case MONSTER_THROWS_RAZORDISKS:
					run_a_missile(source,targ_space,7,1,14,0,0,100);
					monst_spell_note(m_target->number,15);
					break;
				case MONSTER_SHOOTS_SPINES:
					run_a_missile(source,targ_space,5,1,14,0,0,100);
					monst_spell_note(m_target->number,32);
					break;
				default://rock throwing
					run_a_missile(source,targ_space,12,1,14,0,0,100);
					monst_spell_note(m_target->number,14);
				break;
				}
			r1 = get_ran(1,0,100) - 5 * min(10,bless) + 5 * m_target->m_d.status[STATUS_BLESS_CURSE]
				- 5 * (can_see(source, m_target->m_loc,0));
			r2 = get_ran(dam[level],1,7) + min(10,bless);

			if (r1 <= hit_chance[dam[level] * 2]) {
//					monst_spell_note(m_target->number,16); //print "Hits X"

					damage_monst(target - 100,7,r2,0,1300);
				}
				else {
					monst_spell_note(m_target->number,18);
					}
			}
}


Boolean monst_breathe(creature_data_type *caster,location targ_space,short dam_type)
//dam_type; // 0 - fire,  1 - cold,  2 - magic, 3 - darkness (= unblockable type)
{
	short level,type[4] = {1,5,3,4},missile_t[4] = {13,6,8,8};
	location l;

	draw_terrain(2);
	if ((is_combat()) && (center_on_monst == true)) {
		frame_space(caster->m_loc,0,caster->m_d.x_width,caster->m_d.y_width);
		}

	l = caster->m_loc;
	if ((caster->m_d.direction < 4) &&
		(caster->m_d.x_width > 1))
			l.x++;

	dam_type = caster->m_d.breath_type;
	run_a_missile(l,targ_space,missile_t[dam_type],0,44,0,0,100);
	level = caster->m_d.breath;

	monst_breathe_note(caster->number);
	level = get_ran(caster->m_d.breath,1,8);
	if (overall_mode < MODE_COMBAT)
		level = level / 3;
	start_missile_anim();
	hit_space(targ_space,level,type[dam_type],1,1);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();

	return true;
}

Boolean monst_cast_mage(creature_data_type *caster,short targ)
{
	short j,spell,i,level,target_levels,friend_levels_near,x;
	unsigned short r1;
	Boolean acted = false;
	location target,vict_loc,ashes_loc,l;
	creature_data_type *affected;
	long dummy;
	/*short caster_array[7][18] = {{1,1,1,2,2, 2,1,3,4,4, 1,1,1,2,2, 2,3,4},
								{5,5,5,6,7, 8,9,10,11,11, 2,2,2,5,7, 10,10,5},
								{5,5,2,9,11, 12,12,12,14,13, 13,12,12,2,2, 2,2,2},
								{15,15,16,17,17, 5,12,12,13,13, 17,17,16,17,16, 2,2,2},
								{15,18,19,19,20, 20,21,21,16,17, 18,18,18,18,19, 19,19,20},
								{23,23,22,22,21, 21,20,24,19,18, 18,18,18,18,18, 23,23,19},
								{23,23,24,25,26, 27,19,22,19,18, 18,18,18,18,26, 24,24,23}};
	short emer_spells[7][4] = {{2,0,0,5},//emergency spells
								{2,10,11,7},
								{2,13,12,13},
								{2,13,12,13},
								{18,20,19,18},
								{18,24,19,24},
								{18,26,19,27}};*/

    short caster_array[7][18] = {//mage level 1 (spark, minor haste, strength, flame cloud)
                                {SPELL_MONST_MAGE_SPARK ,SPELL_MONST_MAGE_SPARK ,SPELL_MONST_MAGE_SPARK ,SPELL_MONST_MAGE_MINOR_HASTE,
                                SPELL_MONST_MAGE_MINOR_HASTE, SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_SPARK ,
                                SPELL_MONST_MAGE_STRENGH,SPELL_MONST_MAGE_FLAME_CLOUD,SPELL_MONST_MAGE_FLAME_CLOUD,
                                SPELL_MONST_MAGE_SPARK ,SPELL_MONST_MAGE_SPARK ,SPELL_MONST_MAGE_SPARK ,SPELL_MONST_MAGE_MINOR_HASTE,
                                SPELL_MONST_MAGE_MINOR_HASTE, SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_STRENGH,SPELL_MONST_MAGE_FLAME_CLOUD},
                                //mage level 2 (flame, minor poison, slow, dumbfound, stinking cloud, summon beast, conflagration, minor haste)
								{SPELL_MONST_MAGE_FLAME,SPELL_MONST_MAGE_FLAME,SPELL_MONST_MAGE_FLAME,SPELL_MONST_MAGE_MINOR_POISON,SPELL_MONST_MAGE_SLOW,
								 SPELL_MONST_MAGE_DUMBFOUND,SPELL_MONST_MAGE_STINKING_CLOUD,SPELL_MONST_MAGE_SUMMON_BEAST,SPELL_MONST_MAGE_CONFLAGRATION,
								 SPELL_MONST_MAGE_CONFLAGRATION, SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_MINOR_HASTE,
								 SPELL_MONST_MAGE_FLAME,SPELL_MONST_MAGE_SLOW, SPELL_MONST_MAGE_SUMMON_BEAST,SPELL_MONST_MAGE_SUMMON_BEAST,SPELL_MONST_MAGE_FLAME},
								//mage level 3 (flame, minor haste, stinking cloud, conflagration, fireball, web, weak summoning)
								{SPELL_MONST_MAGE_FLAME,SPELL_MONST_MAGE_FLAME,SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_STINKING_CLOUD,
								SPELL_MONST_MAGE_CONFLAGRATION, SPELL_MONST_MAGE_FIREBALL,SPELL_MONST_MAGE_FIREBALL,SPELL_MONST_MAGE_FIREBALL,
								SPELL_MONST_MAGE_WEB,SPELL_MONST_MAGE_WEAK_SUMMONING, SPELL_MONST_MAGE_WEAK_SUMMONING,SPELL_MONST_MAGE_FIREBALL,
								SPELL_MONST_MAGE_FIREBALL,SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_MINOR_HASTE,
								SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_MINOR_HASTE},
								//mage level 4 (poison, ice bolt, slow group, flame, fireball, weak summoning, minor haste)
								{SPELL_MONST_MAGE_POISON,SPELL_MONST_MAGE_POISON,SPELL_MONST_MAGE_ICE_BOLT,SPELL_MONST_MAGE_SLOW_GROUP,SPELL_MONST_MAGE_SLOW_GROUP,
								SPELL_MONST_MAGE_FLAME,SPELL_MONST_MAGE_FIREBALL,SPELL_MONST_MAGE_FIREBALL,SPELL_MONST_MAGE_WEAK_SUMMONING,
								SPELL_MONST_MAGE_WEAK_SUMMONING, SPELL_MONST_MAGE_SLOW_GROUP,SPELL_MONST_MAGE_SLOW_GROUP,SPELL_MONST_MAGE_ICE_BOLT,
								SPELL_MONST_MAGE_SLOW_GROUP,SPELL_MONST_MAGE_ICE_BOLT, SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_MINOR_HASTE},
								//mage level 5 (poison, major haste, firestorm, summoning, shockstorm, ice bolt, slow group)
								{SPELL_MONST_MAGE_POISON,SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_FIRESTORM,SPELL_MONST_MAGE_FIRESTORM,
								SPELL_MONST_MAGE_SUMMONING, SPELL_MONST_MAGE_SUMMONING,SPELL_MONST_MAGE_SHOCKSTORM,SPELL_MONST_MAGE_SHOCKSTORM,SPELL_MONST_MAGE_ICE_BOLT,
								SPELL_MONST_MAGE_SLOW_GROUP, SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_HASTE,
								SPELL_MONST_MAGE_FIRESTORM, SPELL_MONST_MAGE_FIRESTORM,SPELL_MONST_MAGE_FIRESTORM,SPELL_MONST_MAGE_SUMMONING},
								//mage level 6 (kill, major poison, shockstorm, summoning, daemon, firestorm, major haste)
								{SPELL_MONST_MAGE_KILL,SPELL_MONST_MAGE_KILL,SPELL_MONST_MAGE_MAJOR_POISON,SPELL_MONST_MAGE_MAJOR_POISON,SPELL_MONST_MAGE_SHOCKSTORM,
								 SPELL_MONST_MAGE_SHOCKSTORM,SPELL_MONST_MAGE_SUMMONING,SPELL_MONST_MAGE_DAEMON,SPELL_MONST_MAGE_FIRESTORM,SPELL_MONST_MAGE_MAJOR_HASTE,
								 SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_HASTE,
								 SPELL_MONST_MAGE_KILL,SPELL_MONST_MAGE_KILL,SPELL_MONST_MAGE_FIRESTORM},
								 //mage level 7 (kill, daemon, major blessing, major haste, , major summoning, shockwave, major poison, firestorm )
								{SPELL_MONST_MAGE_KILL,SPELL_MONST_MAGE_KILL,SPELL_MONST_MAGE_DAEMON,SPELL_MONST_MAGE_MAJOR_BLESSING,
								SPELL_MONST_MAGE_MAJOR_SUMMONING, SPELL_MONST_MAGE_SHOCKWAVE,SPELL_MONST_MAGE_FIRESTORM,SPELL_MONST_MAGE_MAJOR_POISON,
								SPELL_MONST_MAGE_FIRESTORM,SPELL_MONST_MAGE_MAJOR_HASTE, SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_HASTE,
								SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_SUMMONING,
								SPELL_MONST_MAGE_DAEMON,SPELL_MONST_MAGE_DAEMON,SPELL_MONST_MAGE_KILL}
								};

	short emer_spells[7][4] = { //emergency spells level 1 (minor haste, flame)
                                {SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_NO_SPELL,SPELL_MONST_MAGE_NO_SPELL,SPELL_MONST_MAGE_FLAME},
                                //emergency spells level 2 (minor haste, summon beast, conflagration slow)
								{SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_SUMMON_BEAST,SPELL_MONST_MAGE_CONFLAGRATION,SPELL_MONST_MAGE_SLOW},
								//emergency spells level 3 (minor haste, weak summoning, fireball)
								{SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_WEAK_SUMMONING,SPELL_MONST_MAGE_FIREBALL,SPELL_MONST_MAGE_WEAK_SUMMONING},
								//emergency spells level 4 (minor haste, weak summoning, fireball) (same as level 3)
								{SPELL_MONST_MAGE_MINOR_HASTE,SPELL_MONST_MAGE_WEAK_SUMMONING,SPELL_MONST_MAGE_FIREBALL,SPELL_MONST_MAGE_WEAK_SUMMONING},
								//emergency spells level 5 (major haste, summoning, firestorm)
								{SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_SUMMONING,SPELL_MONST_MAGE_FIRESTORM,SPELL_MONST_MAGE_MAJOR_HASTE},
								//emergency spells level 6 (major haste, daemon, firestorm)
								{SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_DAEMON,SPELL_MONST_MAGE_FIRESTORM,SPELL_MONST_MAGE_DAEMON},
								//emergency spells level 7 (major haste, major summoning, firestorm, shockwave)
								{SPELL_MONST_MAGE_MAJOR_HASTE,SPELL_MONST_MAGE_MAJOR_SUMMONING,SPELL_MONST_MAGE_FIRESTORM,SPELL_MONST_MAGE_SHOCKWAVE}};


	if (is_antimagic(caster->m_loc.x,caster->m_loc.y)) {
		return false;
		}
	// is target dead?
	if ((targ < 6) && (adven[targ].isAlive() == false))
		return false;
	if ((targ >= 100) && (c_town.monst.dudes[targ - 100].active == 0))
		return false;

	level = max(1,caster->m_d.mu - caster->m_d.status[STATUS_DUMB]) - 1;

	target = find_fireball_loc(caster->m_loc,1,(caster->attitude % 2 == 1) ? 0 : 1,&target_levels);
	friend_levels_near = (caster->attitude % 2 != 1) ? count_levels(caster->m_loc,3) : -1 * count_levels(caster->m_loc,3);

	if ((caster->m_d.health * 4 < caster->m_d.m_health) && (get_ran(1,0,10) < 9))
		spell = emer_spells[level][3];
		else if ((((caster->m_d.status[STATUS_HASTE_SLOW] < 0) && (get_ran(1,0,10) < 7)) ||
			((caster->m_d.status[STATUS_HASTE_SLOW] == 0) && (get_ran(1,0,10) < 5))) && (emer_spells[level][0] != SPELL_MONST_MAGE_NO_SPELL))
			spell = emer_spells[level][0];
			else if ((friend_levels_near <= -10) && (get_ran(1,0,10) < 7) && (emer_spells[level][1] != SPELL_MONST_MAGE_NO_SPELL))
				spell = emer_spells[level][1];
				else if ((target_levels > 50) && (get_ran(1,0,10) < 7) && (emer_spells[level][2] != SPELL_MONST_MAGE_NO_SPELL))
					spell = emer_spells[level][2];
					else {
						r1 = get_ran(1,0,17);
						spell = caster_array[level][r1];
						}

	// Hastes happen often now, but don't cast them redundantly
	if ((caster->m_d.status[STATUS_HASTE_SLOW] > 0) && ((spell == SPELL_MONST_MAGE_MINOR_HASTE) || (spell == SPELL_MONST_MAGE_MAJOR_HASTE)))
		spell = emer_spells[level][3];


	// Anything preventing spell?
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
		vict_loc = (is_combat()) ? pc_pos[targ] : c_town.p_loc;
		if (is_town())
			vict_loc = target = c_town.p_loc;
		}
	if (targ >= 100)
		vict_loc = c_town.monst.dudes[targ - 100].m_loc;
	if ((targ == 6) && (is_antimagic(target.x,target.y)))
		return false;

	// check antimagic
	if (is_combat())
		if ((targ < 6) && (is_antimagic(pc_pos[targ].x,pc_pos[targ].y)))
			return false;
	if (is_town())
		if ((targ < 6) && (is_antimagic(c_town.p_loc.x,c_town.p_loc.y)))
			return false;
	if ((targ >= 100) && (is_antimagic(c_town.monst.dudes[targ - 100].m_loc.x,
	 c_town.monst.dudes[targ - 100].m_loc.y)))
		return false;


	// How about shockwave? Good idea?
	if ((spell == SPELL_MONST_MAGE_SHOCKWAVE) && (caster->attitude % 2 != 1))
		spell = SPELL_MONST_MAGE_MAJOR_SUMMONING;
	if ((spell == SPELL_MONST_MAGE_SHOCKWAVE) && (caster->attitude % 2 == 1) && (count_levels(caster->m_loc,10) < 45))
		spell = SPELL_MONST_MAGE_MAJOR_SUMMONING;

	l = caster->m_loc;
	if ((caster->m_d.direction < 4) && (caster->m_d.x_width > 1))
		l.x++;

	if (caster->m_d.mp >= monst_mage_cost[spell - 1]) {
		monst_cast_spell_note(caster->number,spell,0);
		acted = true;
		caster->m_d.mp -= monst_mage_cost[spell - 1];

		draw_terrain(2);
		switch (spell) {
			case SPELL_MONST_MAGE_SPARK: // spark
				run_a_missile(l,vict_loc,6,1,11,0,0,80);
				r1 = get_ran(2,1,4);
				damage_target(targ,r1,1);
				break;
			case SPELL_MONST_MAGE_MINOR_HASTE: // minor haste
				play_sound(25);
				caster->m_d.status[STATUS_HASTE_SLOW] += 2;
				break;
			case SPELL_MONST_MAGE_STRENGH: // strength
				play_sound(25);
				caster->m_d.status[STATUS_BLESS_CURSE] += 3;
				break;
			case SPELL_MONST_MAGE_FLAME_CLOUD: // flame cloud
				run_a_missile(l,vict_loc,2,1,11,0,0,80);
				place_spell_pattern(single,vict_loc,5,false,7);
				break;
			case SPELL_MONST_MAGE_FLAME: // flame
				run_a_missile(l,vict_loc,2,1,11,0,0,80);
				start_missile_anim();
				r1 = get_ran(caster->m_d.level,1,4);
				damage_target(targ,r1,1);
				break;
			case SPELL_MONST_MAGE_MINOR_POISON: // minor poison
				run_a_missile(l,vict_loc,11,0,25,0,0,80);
				if (targ < 6)
					adven[targ].poison(2 + get_ran(1,0,1));
					else c_town.monst.dudes[targ - 100].poison(2 + get_ran(1,0,1));
				break;
			case SPELL_MONST_MAGE_SLOW: // slow
				run_a_missile(l,vict_loc,15,0,25,0,0,80);
				if (targ < 6)
					adven[targ].slow(2 + caster->m_d.level / 2);
					else c_town.monst.dudes[targ - 100].slow(2 + caster->m_d.level / 2);
				break;
			case SPELL_MONST_MAGE_DUMBFOUND: // dumbfound
				run_a_missile(l,vict_loc,14,0,25,0,0,80);
				if (targ < 6)
					adven[targ].dumbfound(2);
					else c_town.monst.dudes[targ - 100].dumbfound(2);
				break;
			case SPELL_MONST_MAGE_STINKING_CLOUD: // scloud
				run_a_missile(l,target,0,0,25,0,0,80);
				place_spell_pattern(square,target,7,false,7);
				break;
			case SPELL_MONST_MAGE_SUMMON_BEAST: // summon beast
				r1 = get_summon_monster(1);
				if (r1 == 0)
					break;
				x = get_ran(3,1,4);
				//Delay(12,&dummy); // gives sound time to end
				play_sound(25);
				play_sound(-61);
				summon_monster(r1,caster->m_loc,
				 ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				break;
			case SPELL_MONST_MAGE_CONFLAGRATION: // conflagration
				run_a_missile(l,target,13,1,25,0,0,80);
				place_spell_pattern(radius2,target,5,false,7);
				break;
			case SPELL_MONST_MAGE_FIREBALL: // fireball
				r1 = 1 + (caster->m_d.level * 3) / 4;
				if (r1 > 29) r1 = 29;
				run_a_missile(l,target,2,1,11,0,0,80);
				start_missile_anim();
				place_spell_pattern(square,target,50 + r1,true,7);
				ashes_loc = target;
				break;
			case SPELL_MONST_MAGE_WEAK_SUMMONING: case SPELL_MONST_MAGE_SUMMONING: case SPELL_MONST_MAGE_MAJOR_SUMMONING:// summon
				play_sound(25);
				if (spell == SPELL_MONST_MAGE_WEAK_SUMMONING) {
					r1 = get_summon_monster(1);
					if (r1 == 0)
						break;
					j = get_ran(2,1,3) + 1;
					}
				if (spell == SPELL_MONST_MAGE_SUMMONING) {
					r1 = get_summon_monster(2);
					if (r1 == 0)
						break;
					j = get_ran(2,1,2) + 1;
					}
				if (spell == SPELL_MONST_MAGE_MAJOR_SUMMONING) {
					r1 = get_summon_monster(3);
					if (r1 == 0)
						break;
					j = get_ran(1,2,3);
					}
				Delay(12,&dummy); // gives sound time to end
				x = get_ran(4,1,4);
				for (i = 0; i < j; i++){
					play_sound(-61);
					if (summon_monster(r1,caster->m_loc,
					 ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude) == false) {
						add_string_to_buf("  Summon failed."); i = j;}
					}
				break;
			case SPELL_MONST_MAGE_WEB: // web
				play_sound(25);
				place_spell_pattern(radius2,target,1,false,7);
				break;
			case SPELL_MONST_MAGE_POISON: // poison
				run_a_missile(l,vict_loc,11,0,25,0,0,80);
				x = get_ran(1,0,3);
				if (targ < 6)
					adven[targ].poison(4 + x);
					else c_town.monst.dudes[targ - 100].poison(4 + x);
				break;
			case SPELL_MONST_MAGE_ICE_BOLT: // ice bolt
				run_a_missile(l,vict_loc,6,1,11,0,0,80);
				r1 = get_ran(5 + (caster->m_d.level / 5),1,8);
				start_missile_anim();
				damage_target(targ,r1,5);
				break;
			case SPELL_MONST_MAGE_SLOW_GROUP: // slow gp
				play_sound(25);
				if (caster->attitude % 2 == 1)
					for (i = 0; i < 6; i++)
						if (pc_near(i,caster->m_loc,8))
							adven[i].slow(2 + caster->m_d.level / 4);
				for (i = 0; i < T_M; i++) {
					if ((c_town.monst.dudes[i].active != 0) &&
					(((c_town.monst.dudes[i].attitude % 2 == 1) && (caster->attitude % 2 != 1)) ||
					((c_town.monst.dudes[i].attitude % 2 != 1) && (caster->attitude % 2 == 1)) ||
					((c_town.monst.dudes[i].attitude % 2 == 1) && (caster->attitude != c_town.monst.dudes[i].attitude)))
						 && (dist(caster->m_loc,c_town.monst.dudes[i].m_loc) <= 7))
							c_town.monst.dudes[i].slow(2 + caster->m_d.level / 4);
					}
				break;
			case SPELL_MONST_MAGE_MAJOR_HASTE: // major haste
				play_sound(25);
				for (i = 0; i < T_M; i++)
					if ((monst_near(i,caster->m_loc,8,0)) &&
						(caster->attitude == c_town.monst.dudes[i].attitude)) {
						affected = &c_town.monst.dudes[i];
						affected->m_d.status[STATUS_HASTE_SLOW] += 3;
						}
				play_sound(4);
				break;
			case SPELL_MONST_MAGE_FIRESTORM: // firestorm
				run_a_missile(l,target,2,1,11,0,0,80);
				r1 = 1 + (caster->m_d.level * 3) / 4 + 3;
				if (r1 > 29) r1 = 29;
				start_missile_anim();
				place_spell_pattern(radius2,target,50 + r1,true,7);
				ashes_loc = target;
				break;
			case SPELL_MONST_MAGE_SHOCKSTORM: // shockstorm
				run_a_missile(l,target,6,1,11,0,0,80);
				place_spell_pattern(radius2,target,4,false,7);
				break;
			case SPELL_MONST_MAGE_MAJOR_POISON: // m. poison
				run_a_missile(l,vict_loc,11,1,11,0,0,80);
				x = get_ran(1,1,2);
				if (targ < 6)
					adven[targ].poison(6 + x);
					else c_town.monst.dudes[targ - 100].poison(6 + x);
				break;
			case SPELL_MONST_MAGE_KILL: // kill!!!
				run_a_missile(l,vict_loc,9,1,11,0,0,80);
				r1 = 35 + get_ran(3,1,10);
				start_missile_anim();
				damage_target(targ,r1,3);
				break;
			case SPELL_MONST_MAGE_DAEMON: // daemon
				x = get_ran(3,1,4);
				play_sound(25);
				play_sound(-61);
				Delay(12,&dummy); // gives sound time to end
				summon_monster(85,caster->m_loc,
				 ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				break;
			case SPELL_MONST_MAGE_MAJOR_BLESSING: // major bless
				play_sound(25);
				for (i = 0; i < T_M; i++)
					if ((monst_near(i,caster->m_loc,8,0)) &&
						(caster->attitude == c_town.monst.dudes[i].attitude)) {
						affected = &c_town.monst.dudes[i];
						affected->m_d.health += get_ran(2,1,10);
						r1 = get_ran(3,1,4);
						affected->m_d.status[STATUS_BLESS_CURSE] = min(8,affected->m_d.status[STATUS_BLESS_CURSE] + r1);
						affected->m_d.status[STATUS_WEBS] = 0;
						if (affected->m_d.status[STATUS_HASTE_SLOW] < 0)
							affected->m_d.status[STATUS_HASTE_SLOW] = 0;
						affected->m_d.morale += get_ran(3,1,10);
						}
				play_sound(4);
				break;
			case SPELL_MONST_MAGE_SHOCKWAVE: // shockwave
				do_shockwave(caster->m_loc);
				break;
			}
		}
		else caster->m_d.mp++;

	if (ashes_loc.x > 0)
		make_sfx(ashes_loc.x,ashes_loc.y,6);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();

	return acted;
}

Boolean monst_cast_priest(creature_data_type *caster,short targ)
{
	short r1,r2,spell,i,x,level,target_levels,friend_levels_near;
	Boolean acted = false;
	location target,vict_loc,l;
	creature_data_type *affected;

    location ashes_loc;

	/*short caster_array[7][10] = {{1,1,1,1,3,3,3,4,4,4},
									{5,5,6,6,7,7,8,8,8,9},
									{9,6,6,8,11,12,12,5,5,12},
									{12,12,13,13,14,9,9,14,14,15},
									{19,18,13,19,15,18,18,19,16,18},
									{22,18,16,19,18,18,21,22,23,23},
									{26,26,25,24,26,22,24,22,26,25}};
	short emer_spells[7][4] = {{0,1,0,2},
								{0,8,0,2},
								{0,8,0,10},
								{0,14,0,10},
								{0,19,18,17},
								{0,19,18,20},
								{25,25,26,24}};*/

    short caster_array[7][10] = {   //priest level 1 (minor bless, wrack, stumble)
                                    {SPELL_MONST_PRIEST_MINOR_BLESS,SPELL_MONST_PRIEST_MINOR_BLESS,SPELL_MONST_PRIEST_MINOR_BLESS,
                                    SPELL_MONST_PRIEST_MINOR_BLESS,SPELL_MONST_PRIEST_WRACK,SPELL_MONST_PRIEST_WRACK,SPELL_MONST_PRIEST_WRACK,
                                    SPELL_MONST_PRIEST_STUMBLE,SPELL_MONST_PRIEST_STUMBLE,SPELL_MONST_PRIEST_STUMBLE},
									//priest level 2 (bless, curse, wound, summon spirit, disease)
									{SPELL_MONST_PRIEST_BLESS,SPELL_MONST_PRIEST_BLESS,SPELL_MONST_PRIEST_CURSE,SPELL_MONST_PRIEST_CURSE,
									SPELL_MONST_PRIEST_WOUND,SPELL_MONST_PRIEST_WOUND,SPELL_MONST_PRIEST_SUMMON_SPIRIT,
									SPELL_MONST_PRIEST_SUMMON_SPIRIT,SPELL_MONST_PRIEST_SUMMON_SPIRIT,SPELL_MONST_PRIEST_DISEASE},
									//priest level 3 (disease, curse, holy scourge, smite, )
									{SPELL_MONST_PRIEST_DISEASE,SPELL_MONST_PRIEST_CURSE,SPELL_MONST_PRIEST_CURSE,
									SPELL_MONST_PRIEST_SUMMON_SPIRIT,SPELL_MONST_PRIEST_HOLY_SCOURGE,SPELL_MONST_PRIEST_SMITE,SPELL_MONST_PRIEST_SMITE,
									SPELL_MONST_PRIEST_BLESS,SPELL_MONST_PRIEST_BLESS,SPELL_MONST_PRIEST_SMITE},
									//priest level 4 (smite, curse all, sticks to snake, disease, martyr's shield)
									{SPELL_MONST_PRIEST_SMITE,SPELL_MONST_PRIEST_SMITE,SPELL_MONST_PRIEST_CURSE_ALL,SPELL_MONST_PRIEST_CURSE_ALL,
									SPELL_MONST_PRIEST_STICKS_TO_SNAKES,SPELL_MONST_PRIEST_DISEASE,SPELL_MONST_PRIEST_DISEASE,
									SPELL_MONST_PRIEST_STICKS_TO_SNAKES,SPELL_MONST_PRIEST_STICKS_TO_SNAKES,SPELL_MONST_PRIEST_MARTYRS_SHIELD},
									//priest level 5 (summon host, flamestrike, curse all, martyr's shield, bless all)
									{SPELL_MONST_PRIEST_SUMMON_HOST,SPELL_MONST_PRIEST_FLAMESTRIKE,SPELL_MONST_PRIEST_CURSE_ALL,SPELL_MONST_PRIEST_SUMMON_HOST,
									SPELL_MONST_PRIEST_MARTYRS_SHIELD,SPELL_MONST_PRIEST_FLAMESTRIKE,SPELL_MONST_PRIEST_FLAMESTRIKE,
									SPELL_MONST_PRIEST_SUMMON_HOST,SPELL_MONST_PRIEST_BLESS_ALL,SPELL_MONST_PRIEST_FLAMESTRIKE},
									//priest level 6 (summon guardian, flamestrike, bless all, summon host, unholy ravaging, pestilence)
									{SPELL_MONST_PRIEST_SUMMON_GUARDIAN,SPELL_MONST_PRIEST_FLAMESTRIKE,SPELL_MONST_PRIEST_BLESS_ALL,
									SPELL_MONST_PRIEST_SUMMON_HOST,SPELL_MONST_PRIEST_FLAMESTRIKE,SPELL_MONST_PRIEST_FLAMESTRIKE,SPELL_MONST_PRIEST_UNHOLY_RAVAGING,
									SPELL_MONST_PRIEST_SUMMON_GUARDIAN,SPELL_MONST_PRIEST_PESTILENCE,SPELL_MONST_PRIEST_PESTILENCE},
									//priest level 7 (divine thud, avatar, revive all, summon guardian)
									{SPELL_MONST_PRIEST_DIVINE_THUD,SPELL_MONST_PRIEST_DIVINE_THUD,SPELL_MONST_PRIEST_AVATAR,SPELL_MONST_PRIEST_REVIVE_ALL,
									SPELL_MONST_PRIEST_DIVINE_THUD,	SPELL_MONST_PRIEST_SUMMON_GUARDIAN,SPELL_MONST_PRIEST_REVIVE_ALL,SPELL_MONST_PRIEST_SUMMON_GUARDIAN,
									SPELL_MONST_PRIEST_DIVINE_THUD,SPELL_MONST_PRIEST_AVATAR}};
	short emer_spells[7][4] = { //emergency spells level 1 (minor bless, light heal)
                                {SPELL_MONST_PRIEST_NO_SPELL,SPELL_MONST_PRIEST_MINOR_BLESS,SPELL_MONST_PRIEST_NO_SPELL,SPELL_MONST_PRIEST_LIGHT_HEAL},
                                //emergency spells level 2 (summon spirit, light heal)
								{SPELL_MONST_PRIEST_NO_SPELL,SPELL_MONST_PRIEST_SUMMON_SPIRIT,SPELL_MONST_PRIEST_NO_SPELL,SPELL_MONST_PRIEST_LIGHT_HEAL},
								//emergency spells level 3 (summon spirit, heal)
								{SPELL_MONST_PRIEST_NO_SPELL,SPELL_MONST_PRIEST_SUMMON_SPIRIT,SPELL_MONST_PRIEST_NO_SPELL,SPELL_MONST_PRIEST_HEAL},
								//emergency spells level 4 (sticks to snakes, heal)
								{SPELL_MONST_PRIEST_NO_SPELL,SPELL_MONST_PRIEST_STICKS_TO_SNAKES,SPELL_MONST_PRIEST_NO_SPELL,SPELL_MONST_PRIEST_HEAL},
								//emergency spells level 5 (summon host, flamestrike, major heal)
								{SPELL_MONST_PRIEST_NO_SPELL,SPELL_MONST_PRIEST_SUMMON_HOST,SPELL_MONST_PRIEST_FLAMESTRIKE,SPELL_MONST_PRIEST_MAJOR_HEAL},
								//emergency spells level 6 (summon host, flamestrike, full heal)
								{SPELL_MONST_PRIEST_NO_SPELL,SPELL_MONST_PRIEST_SUMMON_HOST,SPELL_MONST_PRIEST_FLAMESTRIKE,SPELL_MONST_PRIEST_REVIVE_SELF},
								//emergency spells level 7 (avatar, divine thud, revive all)
								{SPELL_MONST_PRIEST_AVATAR,SPELL_MONST_PRIEST_AVATAR,SPELL_MONST_PRIEST_DIVINE_THUD,SPELL_MONST_PRIEST_REVIVE_ALL}};


	if ((targ < 6) && (adven[targ].isAlive() == false))
		return false;
	if ((targ >= 100) && (c_town.monst.dudes[targ - 100].active == 0))
		return false;
	if (is_antimagic(caster->m_loc.x,caster->m_loc.y)) {
		return false;
		}
	level = max(1,caster->m_d.cl - caster->m_d.status[STATUS_DUMB]) - 1;

	target = find_fireball_loc(caster->m_loc,1,(caster->attitude % 2 == 1) ? 0 : 1,&target_levels);
	friend_levels_near = (caster->attitude % 2 != 1) ? count_levels(caster->m_loc,3) : -1 * count_levels(caster->m_loc,3);

	if ((caster->m_d.health * 4 < caster->m_d.m_health) && (get_ran(1,0,10) < 9))
		spell = emer_spells[level][3];
		else if ((caster->m_d.status[STATUS_HASTE_SLOW] < 0) && (get_ran(1,0,10) < 7) && (emer_spells[level][0] != SPELL_MONST_PRIEST_NO_SPELL))
			spell = emer_spells[level][0];
			else if ((friend_levels_near <= -10) && (get_ran(1,0,10) < 7) && (emer_spells[level][1] != SPELL_MONST_PRIEST_NO_SPELL))
				spell = emer_spells[level][1];
				else if ((target_levels > 50 < 0) && (get_ran(1,0,10) < 7) && (emer_spells[level][2] != SPELL_MONST_PRIEST_NO_SPELL))
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
		vict_loc = (is_town()) ? c_town.p_loc : pc_pos[targ];
	if (targ >= 100)
		vict_loc = c_town.monst.dudes[targ - 100].m_loc;
	if ((targ == 6) && (is_antimagic(target.x,target.y)))
		return false;
	if ((targ < 6) && (is_antimagic(pc_pos[targ].x,pc_pos[targ].y)))
		return false;
	if ((targ >= 100) && (is_antimagic(c_town.monst.dudes[targ - 100].m_loc.x,
	 c_town.monst.dudes[targ - 100].m_loc.y)))
		return false;

	// snuff heals if unwounded
	if ((caster->m_d.health == caster->m_d.m_health) &&
	 ((spell == SPELL_MONST_PRIEST_MAJOR_HEAL) || (spell == SPELL_MONST_PRIEST_REVIVE_SELF) || (spell = SPELL_MONST_PRIEST_LIGHT_HEAL) || (spell = SPELL_MONST_PRIEST_HEAL)))
	 	spell--;

	l = caster->m_loc;
	if ((caster->m_d.direction < 4) && (caster->m_d.x_width > 1))
		l.x++;

	if (caster->m_d.mp >= monst_priest_cost[spell - 1]) {
		monst_cast_spell_note(caster->number,spell,1);
		acted = true;
		caster->m_d.mp -= monst_priest_cost[spell - 1];
		draw_terrain(2);
		switch (spell) {
			case SPELL_MONST_PRIEST_WRACK: // wrack
				run_a_missile(l,vict_loc,8,0,24,0,0,80);
				r1 = get_ran(2,1,4);
				start_missile_anim();
				damage_target(targ,r1,4);
				break;
			case SPELL_MONST_PRIEST_STUMBLE: // stumble
				play_sound(24);
				place_spell_pattern(single,vict_loc,1,false,7);
				break;
			case SPELL_MONST_PRIEST_MINOR_BLESS: case SPELL_MONST_PRIEST_BLESS: // Blesses
				play_sound(24);
				caster->m_d.status[STATUS_BLESS_CURSE] = min(8,caster->m_d.status[STATUS_BLESS_CURSE] + (spell == SPELL_MONST_PRIEST_MINOR_BLESS) ? 3 : 5);
				play_sound(4);
				break;
			case SPELL_MONST_PRIEST_CURSE: // curse
				run_a_missile(l,vict_loc,8,0,24,0,0,80);
				x = get_ran(1,0,1);
				if (targ < 6)
					adven[targ].curse(2 + x);
					else c_town.monst.dudes[targ - 100].curse(2 + x);
				break;
			case SPELL_MONST_PRIEST_WOUND: // wound
				run_a_missile(l,vict_loc,8,0,24,0,0,80);
					r1 = get_ran(2,1,6) + 2;
				start_missile_anim();
					damage_target(targ,r1,3);
				break;
			case SPELL_MONST_PRIEST_SUMMON_SPIRIT: case SPELL_MONST_PRIEST_SUMMON_GUARDIAN: // summon spirit, summon guardian
				play_sound(24);
				play_sound(-61);

				x =  get_ran(3,1,4);
				summon_monster(((spell == SPELL_MONST_PRIEST_SUMMON_SPIRIT) ? 125 : 122),caster->m_loc,
				 ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				break;
			case SPELL_MONST_PRIEST_DISEASE: // disease
				run_a_missile(l,vict_loc,11,0,24,0,0,80);
				x = get_ran(1,0,2);
				if (targ < 6)
					adven[targ].disease(2 + x);
					else c_town.monst.dudes[targ - 100].disease(2 + x);
				break;
			case SPELL_MONST_PRIEST_HOLY_SCOURGE: // holy scourge
				run_a_missile(l,vict_loc,15,0,24,0,0,80);
				if (targ < 6) {
					r1 = get_ran(1,0,2);
					adven[targ].slow(2 + r1);
					r1 = get_ran(1,0,2);
					adven[targ].curse(3 + r1);
					}
					else {
						r1 = get_ran(1,0,2);
						c_town.monst.dudes[targ - 100].slow(r1);
						r1 = get_ran(1,0,2);
						c_town.monst.dudes[targ - 100].curse(r1);
						}
				break;
			case SPELL_MONST_PRIEST_SMITE: // smite
				run_a_missile(l,vict_loc,6,0,24,0,0,80);
				r1 = get_ran(4,1,6) + 2;
				start_missile_anim();
				damage_target(targ,r1,5);
				break;
			case SPELL_MONST_PRIEST_STICKS_TO_SNAKES: // sticks to snakes
				play_sound(24);
				r1 = get_ran(1,1,4) + 2;
				for (i = 0; i < r1; i++) {
					play_sound(-61);
					r2 = get_ran(1,0,7);
					x = get_ran(3,1,4);
					summon_monster((r2 == 1) ? 100 : 99,caster->m_loc,
					 ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
					}
				break;
			case SPELL_MONST_PRIEST_MARTYRS_SHIELD: // martyr's shield
				play_sound(24);
				caster->m_d.status[STATUS_MARTYRS_SHIELD] = min(10,caster->m_d.status[STATUS_MARTYRS_SHIELD] + 5);
				break;
			case SPELL_MONST_PRIEST_SUMMON_HOST: // summon host
				play_sound(24);
				x = get_ran(3,1,4) + 1;
				play_sound(-61);
				summon_monster(126,caster->m_loc,
					((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude);
				for (i = 0; i < 4; i++)	{
					play_sound(-61);
					if (summon_monster(125,caster->m_loc,
						 ((caster->attitude % 2 != 1) ? 0 : 100) + x,caster->attitude) == false)
						 	i = 4;
					}
				break;

			case SPELL_MONST_PRIEST_CURSE_ALL: case SPELL_MONST_PRIEST_PESTILENCE: // curse all,pestilence
				play_sound(24);
				r1 = get_ran(2,0,2);
				r2 = get_ran(1,0,2);
				if (caster->attitude % 2 == 1)
					for (i = 0; i < 6; i++)
						if (pc_near(i,caster->m_loc,8)) {
							if (spell == SPELL_MONST_PRIEST_CURSE_ALL)
								adven[i].curse(2 + r1);
							if (spell == SPELL_MONST_PRIEST_PESTILENCE)
								adven[i].disease(2 + r2);
							}
				for (i = 0; i < T_M; i++) {
					if ((c_town.monst.dudes[i].active != 0) &&
					(((c_town.monst.dudes[i].attitude % 2 == 1) && (caster->attitude % 2 != 1)) ||
					((c_town.monst.dudes[i].attitude % 2 != 1) && (caster->attitude % 2 == 1)) ||
					((c_town.monst.dudes[i].attitude % 2 == 1) && (caster->attitude != c_town.monst.dudes[i].attitude)))
						 && (dist(caster->m_loc,c_town.monst.dudes[i].m_loc) <= 7)) {
							if (spell == SPELL_MONST_PRIEST_CURSE_ALL)
								c_town.monst.dudes[i].curse(2 + r1);
							if (spell == SPELL_MONST_PRIEST_PESTILENCE)
								c_town.monst.dudes[i].disease(2 + r2);
							}
					}
				break;

			case SPELL_MONST_PRIEST_LIGHT_HEAL: case SPELL_MONST_PRIEST_HEAL: case SPELL_MONST_PRIEST_MAJOR_HEAL: case SPELL_MONST_PRIEST_REVIVE_SELF: // heals
				play_sound(24);
				switch(spell) {
					case SPELL_MONST_PRIEST_LIGHT_HEAL: r1 = get_ran(2,1,4) + 2; break;
					case SPELL_MONST_PRIEST_HEAL: r1 = get_ran(3,1,6); break;
					case SPELL_MONST_PRIEST_MAJOR_HEAL: r1 = get_ran(5,1,6) + 3; break;
					case SPELL_MONST_PRIEST_REVIVE_SELF: r1 = 50; break;
					}
				caster->m_d.health = min(caster->m_d.health + r1, caster->m_d.m_health);
				break;
			case SPELL_MONST_PRIEST_BLESS_ALL: case SPELL_MONST_PRIEST_REVIVE_ALL:// bless all,revive all
				play_sound(24);
				r1 =  get_ran(2,1,4);
				r2 = get_ran(3,1,6);// <= shouldn't it be the "revive all" roll ?
				for (i = 0; i < T_M; i++)
					if ((monst_near(i,caster->m_loc,8,0)) &&
						(caster->attitude == c_town.monst.dudes[i].attitude)) {
						affected = &c_town.monst.dudes[i];
						if (spell == SPELL_MONST_PRIEST_BLESS_ALL)
							affected->m_d.status[STATUS_BLESS_CURSE] = min(8,affected->m_d.status[STATUS_BLESS_CURSE] + r1);
						if (spell == SPELL_MONST_PRIEST_REVIVE_ALL)
							affected->m_d.health += r1; //<= here
						}
				play_sound(4);
				break;
			case SPELL_MONST_PRIEST_FLAMESTRIKE: // Flamestrike
				run_a_missile(l,target,2,0,11,0,0,80);
				r1 = 2 + caster->m_d.level / 2 + 2;
				start_missile_anim();
				place_spell_pattern(square,target,50 + r1,true,7);
				ashes_loc = target;
				break;


			case SPELL_MONST_PRIEST_UNHOLY_RAVAGING: // holy ravaging
				run_a_missile(l,vict_loc,14,0,53,0,0,80);
				r1 = get_ran(4,1,8);
				r2 = get_ran(1,0,2);
				damage_target(targ,r1,3);
				if (targ < 6) {
					adven[targ].slow(6);
					adven[targ].poison(5 + r2);
					}
					else {
						c_town.monst.dudes[targ - 100].slow(6);
						c_town.monst.dudes[targ - 100].poison(5 + r2);
						}
				break;
			case SPELL_MONST_PRIEST_AVATAR: // avatar
				play_sound(24);
				monst_spell_note(caster->number,26);
				caster->m_d.health = caster->m_d.m_health;
				caster->m_d.status[STATUS_BLESS_CURSE] = 8;
				caster->m_d.status[STATUS_POISON] = 0;
				caster->m_d.status[STATUS_HASTE_SLOW] = 8;
				caster->m_d.status[STATUS_WEBS] = 0;
				caster->m_d.status[STATUS_DISEASE] = 0;
				caster->m_d.status[STATUS_DUMB] = 0;
				caster->m_d.status[STATUS_MARTYRS_SHIELD] = 8;
				break;
			case SPELL_MONST_PRIEST_DIVINE_THUD: // divine thud
				run_a_missile(l,target,9,0,11,0,0,80);
				r1 = (caster->m_d.level * 3) / 4 + 5;
				if (r1 > 29) r1 = 29;
				start_missile_anim();
				place_spell_pattern(radius2,target,130 + r1,true,7 );
				ashes_loc = target;
				break;
			}


		}
		else caster->m_d.mp++;
	if (ashes_loc.x > 0)
		make_sfx(ashes_loc.x,ashes_loc.y,6);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();

	return acted;
}

void damage_target(short target,short dam,short type)
{
	if (target == INVALID_PC) return;
	if (target < NUM_OF_PCS)
		adven[target].damage(dam,type,-1);
		else damage_monst(target - 100, 7, dam, 0, type);
}

location find_fireball_loc(location where,short radius,short mode,short *m)
//short mode; // 0 - hostile casting  1 - friendly casting
{
	location check_loc,cast_loc = location(120,0);
	short cur_lev,level_max = 10;

	for (check_loc.x = 1; check_loc.x < town_size[town_type] - 1; check_loc.x ++)
		for (check_loc.y = 1; check_loc.y < town_size[town_type] - 1; check_loc.y ++)
			if ((dist(where,check_loc) <= 8) && (can_see(where,check_loc,2) < 5) && (get_obscurity(check_loc.x,check_loc.y) < 5)) {
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
	location pc_where = location(120,120);

	for (i = 0; i < 6; i++)
		if (adven[i].isAlive())
			if ((dist(where,pc_pos[i])) < (dist(where,pc_where)))
				pc_where = pc_pos[i];
	return pc_where;
}

short count_levels(location where,short radius)
{
	short i,store = 0;

	for (i = 0; i < T_M; i++)
		if (monst_near(i,where,radius,0) == true) {
			if (c_town.monst.dudes[i].attitude % 2 == 1)
				store = store - c_town.monst.dudes[i].m_d.level;
				else store = store + c_town.monst.dudes[i].m_d.level;
			}
	if (is_combat()) {
		for (i = 0; i < 6; i++)
			if (pc_near(i,where,radius) == true)
				store = store + 10;
		}
	if (is_town())
		if ((vdist(where,c_town.p_loc) <= radius) && (can_see(where,c_town.p_loc,2) < 5))
			store += 20;

	return store;
}

Boolean pc_near(short pc_num,location where,short radius)
{
	// Assuming not looking
	if (overall_mode >= MODE_COMBAT) {
		if ((adven[pc_num].isAlive()) && (vdist(pc_pos[pc_num],where) <= radius))
			return true;
			else return false;
		}
	if ((adven[pc_num].isAlive()) && (vdist(c_town.p_loc,where) <= radius))
		return true;
		else return false;
}

Boolean monst_near(short m_num,location where,short radius,short active)
//short active; // 0 - any monst 1 - monster need be active
{
	if ((c_town.monst.dudes[m_num].active != 0) && (vdist(c_town.monst.dudes[m_num].m_loc,where) <= radius)
		 && ((active == 0) || (c_town.monst.dudes[m_num].active == 2)) )
		return true;
		else return false;
}

void place_spell_pattern(effect_pat_type pat,location center,short type,Boolean,short who_hit)
//type;  // 0 - take codes in pattern, OW make all nonzero this type
// Types  0 - Null  1 - web  2 - fire barrier  3 - force barrier  4 - force wall  5 - fire wall
//   6 - anti-magic field  7 - stink cloud  8 - ice wall  9 - blade wall  10 - quickfire
//   11 - dispel  12 - sleep field
//  50 + i - 80 :  id6 fire damage  90 + i - 120 : id6 cold damage 	130 + i - 160 : id6 magic dam.
// if prep for anim is true, suppress look checks and go fast
{
	short i,j,r1,k;
	unsigned char effect;
	location spot_hit;
	location s_loc;
	RECT active;
	creature_data_type *which_m;
	Boolean monster_hit = false;

if (type > 0)
		modify_pattern(&pat,type);

	active = c_town.town.in_town_rect.rect32();;
	// eliminate barriers that can't be seen
		for (i = minmax(active.left + 1,active.right - 1,(long)center.x - 4);
		 i <= minmax(active.left + 1,active.right - 1,(long)center.x + 4); i++)
			for (j = minmax(active.top + 1,active.bottom - 1,(long)center.y - 4);
			 j <= minmax(active.top + 1,active.bottom - 1,(long)center.y + 4); j++) {
				s_loc.x = i; s_loc.y = j;
				if (can_see(center,s_loc,0) > 4)
					 pat.pattern[i - center.x + 4][j - center.y + 4] = 0;
			}


	// First actually make barriers, then draw them, then inflict damaging effects.
	for (i = minmax(0,town_size[town_type] - 1,center.x - 4); i <= minmax(0,town_size[town_type] - 1,center.x + 4); i++)
		for (j = minmax(0,town_size[town_type] - 1,center.y - 4); j <= minmax(0,town_size[town_type] - 1,center.y + 4); j++)
			if (get_obscurity(i,j) < 5) {
				effect = pat.pattern[i - center.x + 4][j - center.y + 4];
				switch (effect) {
					case 1: web_space(i,j); break;
					case 2: make_fire_barrier(i,j); break;
					case 3: make_force_barrier(i,j); break;
					case 4: make_force_wall(i,j); break;
					case 5: make_fire_wall(i,j); break;
					case 6: make_antimagic(i,j); break;
					case 7: scloud_space(i,j); break;
					case 8: make_ice_wall(i,j); break;
					case 9: make_blade_wall(i,j); break;
					case 10:
						make_quickfire(i,j);
						break;
					case 11:
						dispel_fields(i,j,0);
						break;
					case 12: sleep_cloud_space(i,j); break;
					}
					}
	draw_terrain(0);
	if (is_town()) // now make things move faster if in town
		fast_bang = 2;

	// Damage to pcs
	for (k = 0; k < 6; k++)
		for (i = minmax(0,town_size[town_type] - 1,center.x - 4); i <= minmax(0,town_size[town_type] - 1,center.x + 4); i++)
			for (j = minmax(0,town_size[town_type] - 1,center.y - 4); j <= minmax(0,town_size[town_type] - 1,center.y + 4); j++) {
				spot_hit.x = i;
				spot_hit.y = j;
				if ((get_obscurity(i,j) < 5) && (adven[k].isAlive())
					&& (((is_combat()) &&(same_point(pc_pos[k],spot_hit) == true)) ||
					((is_town()) && (same_point(c_town.p_loc,spot_hit) == true)))) {
					effect = pat.pattern[i - center.x + 4][j - center.y + 4];
					switch (effect) {
						case 4://force wall
							r1 = get_ran(2,1,6);
							adven[k].damage(r1,3,-1);
							break;
						case 5://fire wall
							r1 = get_ran(1,1,6) + 1;
							adven[k].damage(r1,1,-1);
							break;
						case 8://ice wall
							r1 = get_ran(2,1,6);
							adven[k].damage(r1,5,-1);
							break;
						case 9://blade wall
							r1 = get_ran(4,1,8);
							adven[k].damage(r1,0,-1);
							break;
						default:
							if ((effect >= 50) && (effect < 80)) {
								r1 = get_ran(effect - 50,1,6);
								adven[k].damage(r1,1,-1);
								}
							if ((effect >= 90) && (effect < 120)) {
								r1 = get_ran(effect - 90,1,6);
								adven[k].damage(r1,5,-1);
								}
							if ((effect >= 130) && (effect < 160)) {
								r1 = get_ran(effect - 130,1,6);
								adven[k].damage(r1,3,-1);
								}
						break;
  						}
					}
		}

	fast_bang = 0;

	// Damage to monsters
	for (k = 0; k < T_M; k++)
		if ((c_town.monst.dudes[k].active > 0) && (dist(center,c_town.monst.dudes[k].m_loc) <= 5)) {
		monster_hit = false;
		// First actually make barriers, then draw them, then inflict damaging effects.
		for (i = minmax(0,town_size[town_type] - 1,center.x - 4); i <= minmax(0,town_size[town_type] - 1,center.x + 4); i++)
			for (j = minmax(0,town_size[town_type] - 1,center.y - 4); j <= minmax(0,town_size[town_type] - 1,center.y + 4); j++) {
				spot_hit.x = i;
				spot_hit.y = j;

				if ((monster_hit == false) && (get_obscurity(i,j) < 5) && (monst_on_space(spot_hit,k) > 0)) {

					if (pat.pattern[i - center.x + 4][j - center.y + 4] > 0)
						monster_hit = true;
					effect = pat.pattern[i - center.x + 4][j - center.y + 4];
					switch (effect) {
						case 1://web
							which_m = &c_town.monst.dudes[k];
							which_m->web(3);
							break;
						case 4://force wall
							r1 = get_ran(3,1,6);
							which_m = &c_town.monst.dudes[k];
							if (which_m->m_d.radiate_1 == MONSTER_RADIATE_SHOCK_FIELDS)
								break;
							damage_monst(k, who_hit, r1,0, 3);
							break;
						case 5://fire wall
							r1 = get_ran(2,1,6);
							which_m = &c_town.monst.dudes[k];
							//if (which_m->m_d.spec_skill == MONSTER_PERMANENT_MARTYRS_SHIELD)//old exile 3 leftover
							if (which_m->m_d.radiate_1 == MONSTER_RADIATE_FIRE_FIELDS)
								break;
							damage_monst(k, who_hit, r1,0, 1);
							break;
						case 7://stink cloud
							which_m = &c_town.monst.dudes[k];
							if (which_m->m_d.radiate_1 == MONSTER_RADIATE_STINKING_CLOUDS)
								break;
							which_m->curse(get_ran(1,1,2));
							break;
						case 8://ice wall
							which_m = &c_town.monst.dudes[k];
							r1 = get_ran(3,1,6);
							//if (which_m->m_d.spec_skill == MONSTER_PARALYSIS_RAY)//old exile 3 leftover
							if (which_m->m_d.radiate_1 == MONSTER_RADIATE_ICE_FIELDS)
								break;
							damage_monst(k, who_hit, r1,0, 5);
							break;
						case 9://blade wall
							r1 = get_ran(6,1,8);
							damage_monst(k, who_hit, r1,0, 0);
							break;
						case 12://sleep field
							which_m = &c_town.monst.dudes[k];
							if (which_m->m_d.radiate_1 == MONSTER_RADIATE_SLEEP_FIELDS)
								break;
							which_m->charm(0,11,3);
							break;
						default:
							if ((effect >= 50) && (effect < 80)) {
								r1 = get_ran(effect - 50,1,6);
								damage_monst(k,who_hit,  r1,0,1);
								}
							if ((effect >= 90) && (effect < 120)) {
								r1 = get_ran(effect - 90,1,6);
								damage_monst(k,who_hit,  r1,0, 5);
								}
							if ((effect >= 130) && (effect < 160)) {
								r1 = get_ran(effect - 130,1,6);
								damage_monst(k,who_hit,  r1,0, 3 );
								}
						}
					}
			}
		}
}


void modify_pattern(effect_pat_type *pat,short type)
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
		if ((dist(target,pc_pos[i]) > 0) && (dist(target,pc_pos[i]) < 11)
			&& (adven[i].isAlive()))
				adven[i].damage(get_ran(2 + dist(target,pc_pos[i]) / 2, 1, 6), 4,-1);
  	for (i = 0; i < T_M; i++)
		if ((c_town.monst.dudes[i].active != 0) && (dist(target,c_town.monst.dudes[i].m_loc) > 0)
			 && (dist(target,c_town.monst.dudes[i].m_loc) < 11)
			 && (can_see(target,c_town.monst.dudes[i].m_loc,0) < 5))
				damage_monst(i, current_pc, get_ran(2 + dist(target,c_town.monst.dudes[i].m_loc) / 2 , 1, 6), 0, 4);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
}

void radius_damage(location target,short radius, short dam, short type)////
{
	short i;

	if (is_town()) {
		for (i = 0; i < 6; i++)
			if ((dist(target,c_town.p_loc) > 0) && (dist(target,c_town.p_loc) <= radius)
				&& (adven[i].isAlive()))
					adven[i].damage(dam, type,-1);
		for (i = 0; i < T_M; i++)
			if ((c_town.monst.dudes[i].active != 0) && (dist(target,c_town.monst.dudes[i].m_loc) > 0)
				 && (dist(target,c_town.monst.dudes[i].m_loc) <= radius)
				 && (can_see(target,c_town.monst.dudes[i].m_loc,0) < 5))
					damage_monst(i, current_pc, dam, 0, type);
	   return;
		}

	start_missile_anim();
	for (i = 0; i < 6; i++)
		if ((dist(target,pc_pos[i]) > 0) && (dist(target,pc_pos[i]) <= radius)
			&& (adven[i].isAlive()))
				adven[i].damage(dam, type,-1);
	for (i = 0; i < T_M; i++)
		if ((c_town.monst.dudes[i].active != 0) && (dist(target,c_town.monst.dudes[i].m_loc) > 0)
			 && (dist(target,c_town.monst.dudes[i].m_loc) <= radius)
			 && (can_see(target,c_town.monst.dudes[i].m_loc,0) < 5))
				damage_monst(i, current_pc, dam, 0, type);
	do_explosion_anim(5,0);
	end_missile_anim();
	handle_marked_damage();
}

// Slightly kludgy way to only damage PCs in space)
void hit_pcs_in_space(location target,short dam,short type,short report,short hit_all)
{
	hit_space(target, dam,type, report, 10 + hit_all);
}

void hit_space(location target,short dam,short type,short report,short hit_all)
//type; // 0 - weapon   1 - fire   2 - poison   3 - general magic   4 - unblockable  5 - cold
			//	6 - demon	7 - undead
//short report; // 0 - no  1 - print result
//hit_all; // 0 - nail top thing   1 - hit all in space  + 10 ... no monsters
{
	short i;
	Boolean stop_hitting = false,hit_monsters = true;

	if ((target.x < 0) || (target.x > 63) || (target.y < 0) || (target.y > 63))
		return;

	if (hit_all >= 10) {
		hit_monsters = false;
		hit_all -= 10;
		}

	if ((is_antimagic(target.x,target.y)) && ((type == 1) || (type == 3) || (type == 5))) {
		return;
		}

	if (dam <= 0) {
		add_string_to_buf("  No damage.");
		return;
		}

	for (i = 0; i < T_M; i++)
		if ((hit_monsters == true) && (c_town.monst.dudes[i].active != 0) && (stop_hitting == false))
			if (monst_on_space(target,i)) {
				if (processing_fields == true)
					damage_monst(i, 6, dam, 0, type);
					else damage_monst(i, (monsters_going == true) ? 7 : current_pc, dam, 0, type);
				stop_hitting = (hit_all == 1) ? false : true;
				}

	if (overall_mode >= MODE_COMBAT)
		for (i = 0; i < NUM_OF_PCS; i++)
			if ((adven[i].isAlive()) && (stop_hitting == false))
				if (same_point(pc_pos[i],target) == true) {
						adven[i].damage(dam,type,-1);
						stop_hitting = (hit_all == 1) ? false : true;
					}
	if (overall_mode < MODE_COMBAT)
		if (same_point(target,c_town.p_loc) == true) {
			fast_bang = 1;
			adven.damage(dam,type);
			fast_bang = 0;
			stop_hitting = (hit_all == 1) ? false : true;
			}

	if ((report == 1) && (hit_all == 0) && (stop_hitting == false))
		add_string_to_buf("  Missed.");

}



void do_poison()
{
	short i,r1 = 0;
	Boolean some_poison = false;

	for (i = 0; i < 6; i++)
		if (adven[i].isAlive())
			if (adven[i].status[STATUS_POISON] > 0)
				some_poison = true;
	if (some_poison == true) {
		add_string_to_buf("Poison:                        ");
		for (i = 0; i < 6; i++)
			if (adven[i].isAlive())
				if (adven[i].status[STATUS_POISON] > 0) {
					r1 = get_ran(adven[i].status[STATUS_POISON],1,6);
					adven[i].damage(r1,2,-1);
					if (get_ran(1,0,8) < 6)
						move_to_zero(adven[i].status[STATUS_POISON]);
					if (get_ran(1,0,8) < 6)
						if (adven[i].traits[TRAIT_GOOD_CONST] == true)
							move_to_zero(adven[i].status[STATUS_POISON]);
				}
		put_pc_screen();
		}
}


void handle_disease()
{
	short i,r1 = 0;
	Boolean disease = false;

	for (i = 0; i < 6; i++)
		if (adven[i].isAlive())
			if (adven[i].status[STATUS_DISEASE] > 0)
				disease = true;

	if (disease == true) {
		add_string_to_buf("Disease:                        ");
		for (i = 0; i < NUM_OF_PCS; i++)
			if (adven[i].isAlive())
				if (adven[i].status[STATUS_DISEASE] > 0) {
					r1 = get_ran(1,1,10);
					switch (r1) {
						case 1: case 2:
							adven[i].poison(2);
							break;
						case 3: case 4:
							adven[i].slow(2);
							break;
						case 5:
							adven[i].drainXP(5);
							break;
						case 6: case 7:
							adven[i].curse(3);
							break;
						case 8:
							adven[i].dumbfound(3);
							break;
						case 9: case 10:
							sprintf ( create_line, "  %s unaffected. ", adven[i].name);
							add_string_to_buf(create_line);
							break;
						}
					r1 = get_ran(1,0,7);
					if (adven[i].traits[TRAIT_GOOD_CONST] == true)
						r1 -= 2;
					if ((r1 <= 0) || (adven[i].hasAbilEquip(ITEM_PROTECT_FROM_DISEASE) < 24))
						move_to_zero(adven[i].status[STATUS_DISEASE]);
				}
		put_pc_screen();
		}
}

void handle_acid()
{
	short i,r1 = 0;
	Boolean some_acid = false;

	for (i = 0; i < NUM_OF_PCS; i++)
		if (adven[i].isAlive())
			if (adven[i].status[STATUS_ACID] > 0)
				some_acid = true;

	if (some_acid == true) {
		add_string_to_buf("Acid:                        ");
		for (i = 0; i < NUM_OF_PCS; i++)
			if (adven[i].isAlive())
				if (adven[i].status[STATUS_ACID] > 0) {
					r1 = get_ran(adven[i].status[STATUS_ACID],1,6);
					adven[i].damage(r1,3,-1);
					move_to_zero(adven[i].status[STATUS_ACID]);
				}
		if (overall_mode < MODE_COMBAT)
			boom_space(party.p_loc,overall_mode,3,r1,8);
		}
}

Boolean no_pcs_left()
{
	short i = 0;

	while (i < 6) {
		if (adven[i].isAlive())
			return false;
		i++;
		}
	return true;

}

Boolean hit_end_c_button()
{
	Boolean end_ok = true;

	if (which_combat_type == 0) {
		end_ok = out_monst_all_dead();
		}
	if (end_ok == true)
		end_combat();
	return end_ok;
}

Boolean out_monst_all_dead()
{
	short i;

	for (i = 0; i < T_M; i++)
		if ((c_town.monst.dudes[i].active > 0) && (c_town.monst.dudes[i].attitude % 2 == 1))
			return false;
	return true;
}

void end_combat()
{
	short i;

	for (i = 0; i < 6; i++) {
		if (adven[i].main_status == MAIN_STATUS_FLED)
			adven[i].main_status = MAIN_STATUS_ALIVE;
		adven[i].status[STATUS_POISONED_WEAPON] = 0;
		adven[i].status[STATUS_BLESS_CURSE] = 0;
		adven[i].status[STATUS_HASTE_SLOW] = 0;
		}
	if (which_combat_type == 0) {
		overall_mode = MODE_OUTDOORS;
		}
	combat_active_pc = 6;
	current_pc = store_current_pc;
	if (adven[current_pc].isAlive() == false)
		current_pc = first_active_pc();
	put_item_screen(stat_window,0);
	draw_buttons(0);
}


Boolean combat_cast_mage_spell()
{
	short spell_num,target,i,store_sp,bonus = 1,r1,store_sound = 0,store_m_type = 0,num_opp = 0;
	char c_line[60];
	creature_data_type *which_m;
	monster_record_type get_monst;
	// 0 - refer  1 - do in combat immed.  2 - need targeting  3 - need fancy targeting
	short refer_mage[62] = {0,2,1,1,2,2,0,2,2,0, 2,2,2,2,1,2,2,2,2,2, 0,1,2,0,2,2,3,3,2,1,
	2,2,1,0,2,2,3,2, 0,1,2,0,2,3,2,3, 2,1,2,3,2,2,2,0, 1,1,1,0,3,2,2,3};

	if (is_antimagic(pc_pos[current_pc].x,pc_pos[current_pc].y)) {
		add_string_to_buf("  Not in antimagic field.");
		return false;
		}
	store_sp = adven[current_pc].cur_sp;
	if (adven[current_pc].cur_sp == 0)
		add_string_to_buf("Cast: No spell points.        ");
	else if (adven[current_pc].skills[SKILL_MAGE_SPELLS] == 0)
		add_string_to_buf("Cast: No mage skill.        ");
	else if (get_encumberance(current_pc) > 1) {
		add_string_to_buf("Cast: Too encumbered.        ");
		take_ap(6);
		give_help(40,0,0);
		return true;
		}
		else {


	if (spell_forced == false)
		spell_num = pick_spell(current_pc,0,2);
		else {
			if (repeat_cast_ok(0) == false)
				return false;
			spell_num = pc_last_cast[0][current_pc];
			}

	if (spell_num == SPELL_MAGE_SIMULACRUM) {
		store_sum_monst = pick_trapped_monst();
		if (store_sum_monst == 0)
			return false;
		get_monst = return_monster_template(store_sum_monst);
		if (store_sp < get_monst.level) {
			add_string_to_buf("Cast: Not enough spell points.        ");
			return false;
			}
		store_sum_monst_cost = get_monst.level;
		}

	bonus = adven[current_pc].statAdj(SKILL_INTELLIGENCE);
	combat_posing_monster = current_working_monster = current_pc;
	if (spell_num >= 70)
		return false;
	if (spell_num < 70) {
			print_spell_cast(spell_num,0);
			if (refer_mage[spell_num] == 0) {
				take_ap(6);
				draw_terrain(2);
				do_mage_spell(current_pc,spell_num);
				combat_posing_monster = current_working_monster = -1;
				}
				else if (refer_mage[spell_num] == 2) {
					start_spell_targeting(spell_num);
					}
				else if (refer_mage[spell_num] == 3) {
					start_fancy_spell_targeting(spell_num);
					}
					else {
						start_missile_anim();
						take_ap(6);
						draw_terrain(2);
						switch (spell_num) {
							case SPELL_MAGE_SHOCKWAVE:
								adven[current_pc].cur_sp -= s_cost[0][spell_num];
								add_string_to_buf("  The ground shakes.          ");
								do_shockwave(pc_pos[current_pc]);
								break;

							case SPELL_MAGE_MINOR_HASTE: case SPELL_MAGE_HASTE: case SPELL_MAGE_STRENGTH: case SPELL_MAGE_ENVENOM: case SPELL_MAGE_RESIST_MAGIC:
								target = store_spell_target;
								if (target < 6) {
									adven[current_pc].cur_sp -= s_cost[0][spell_num];
									play_sound(4);
									switch (spell_num) {
										case SPELL_MAGE_ENVENOM:
											sprintf (c_line, "  %s receives venom.               ",
												adven[target].name);
											poison_weapon(target,3 + bonus,1);
											store_m_type = 11;
											break;

										case  SPELL_MAGE_STRENGTH:
											sprintf (c_line, "  %s stronger.                     ",
												adven[target].name);
											adven[target].status[STATUS_BLESS_CURSE] = adven[target].status[STATUS_BLESS_CURSE] + 3; // !!! no cap !!!
											store_m_type = 8;
											break;
										case SPELL_MAGE_RESIST_MAGIC:
											sprintf (c_line, "  %s resistant.                     ",
												adven[target].name);
											adven[target].status[STATUS_MAGIC_RESISTANCE] = adven[target].status[STATUS_MAGIC_RESISTANCE] + 5 + bonus; // !!! no cap !!!
											store_m_type = 15;
											break;

										default:
											i = (spell_num == SPELL_MAGE_MINOR_HASTE) ? 2 : max(2,adven[current_pc].level / 2 + bonus);
											adven[target].status[STATUS_HASTE_SLOW] = min(8,adven[target].status[STATUS_HASTE_SLOW] + i);
											sprintf (c_line, "  %s hasted.                       ",
												adven[target].name);
											store_m_type = 8;
											break;
										}
									add_string_to_buf(c_line);
									add_missile(pc_pos[target],store_m_type,0,0,0);
							}
								break;

							case SPELL_MAGE_MAJOR_HASTE: case SPELL_MAGE_MAJOR_BLESSING:
								store_sound = 25;
								adven[current_pc].cur_sp -= s_cost[0][spell_num];


								for (i = 0; i < 6; i++)
									if (adven[i].isAlive()) {
									adven[i].status[STATUS_HASTE_SLOW] = min(8,
										adven[i].status[STATUS_HASTE_SLOW] + ((spell_num == SPELL_MAGE_MAJOR_HASTE) ? 1 + adven[current_pc].level / 8 + bonus : 3 + bonus));
									if (spell_num == SPELL_MAGE_MAJOR_BLESSING) {
										poison_weapon(i,2,1);
										adven[i].status[STATUS_BLESS_CURSE ] += 4; // !!! no cap !!!
										add_missile(pc_pos[i],14,0,0,0);
										}
										else add_missile(pc_pos[i],8,0,0,0);
									}
								//play_sound(4);
								if (spell_num == SPELL_MAGE_MAJOR_HASTE)
									sprintf (c_line, "  Party hasted.     ");
									else
										sprintf (c_line, "  Party blessed!           ");
								add_string_to_buf(c_line);

								break;



							case SPELL_MAGE_SLOW_GROUP: case SPELL_MAGE_GROUP_FEAR: case SPELL_MAGE_MASS_PARALYSIS: // affect monsters in area spells
								adven[current_pc].cur_sp -= s_cost[0][spell_num];
								store_sound = 25;
								if (spell_num == SPELL_MAGE_GROUP_FEAR)
									store_sound = 54;
								switch (spell_num) {
									case SPELL_MAGE_SLOW_GROUP: sprintf (c_line, "  Enemy slowed:       "); break;
									//case 49: sprintf (c_line, "  Enemy ravaged:              ");break; //apparently 49 is an old "Ravage enemy" spell
									case SPELL_MAGE_GROUP_FEAR: sprintf (c_line, "  Enemy scared:   ");break;
									case SPELL_MAGE_MASS_PARALYSIS: sprintf (c_line, "  Enemy paralyzed:   ");break;
									}
								add_string_to_buf((char *) c_line);
								for (i = 0; i < T_M; i++) {
										if ((c_town.monst.dudes[i].active != 0) && (c_town.monst.dudes[i].attitude % 2 == 1)
										 && (dist(pc_pos[current_pc],c_town.monst.dudes[i].m_loc) <= mage_range[spell_num])
										 && (can_see(pc_pos[current_pc],c_town.monst.dudes[i].m_loc,0) < 5)) {
												which_m = &c_town.monst.dudes[i];
												switch (spell_num) {
													case SPELL_MAGE_GROUP_FEAR:
														r1 = get_ran(adven[current_pc].level / 3,1,8);
														which_m->scare(r1);
														store_m_type = 10;
														break;
													case SPELL_MAGE_SLOW_GROUP: //case 49:
														which_m->slow(5 + bonus);
														/*if (spell_num == 49)
															which_m->curse(3 + bonus);*/
														store_m_type = 8;
														break;
													case SPELL_MAGE_MASS_PARALYSIS:
														which_m->charm(5,12,1000);
														store_m_type = 15;
														break;
													}
											num_opp++;
											add_missile(c_town.monst.dudes[i].m_loc,store_m_type,0,
												14 * (which_m->m_d.x_width - 1),18 * (which_m->m_d.y_width - 1));
											}

										}
								break;

						}

					}
			if (num_opp < 10)
				do_missile_anim((num_opp < 5) ? 50 : 25,pc_pos[current_pc],store_sound);
				else play_sound(store_sound);
			end_missile_anim();
			put_pc_screen();

		}
	}
	combat_posing_monster = current_working_monster = -1;
	// Did anything actually get cast?
	if (store_sp == adven[current_pc].cur_sp)
		return false;
		else return true;
}


Boolean combat_cast_priest_spell()
{
	short spell_num,target,i,store_sp,bonus,store_sound = 0,store_m_type = 0,num_opp = 0;
	char c_line[60];
	creature_data_type *which_m;
	// 0 - refer  1 - do in combat immed.  2 - need targeting  3 - need fancy targeting
	short refer_priest[62] = {1,0,0,2,0,0,0,0,0,2, 1,0,2,0,2,2,0,2,3,0, 0,0,2,0,0,0,2,0,0,3,
	0,1,2,0,3,0,0,0, 1,0,0,2,0,3,0,2, 0,0,0,0,2,1,1,1, 0,2,0,2,1,2,0,0};
	effect_pat_type protect_pat = {{{0,4,4,4,4,4,4,4,0},
						{4,8,8,8,8,8,8,8,4},
						{4,8,9,9,9,9,9,8,4},
						{4,8,9,6,6,6,9,8,4},
						{4,8,9,6,6,6,9,8,4},
						{4,8,9,6,6,6,9,8,4},
						{4,8,9,9,9,9,9,8,4},
						{4,8,8,8,8,8,8,8,4},
						{0,4,4,4,4,4,4,4,0}}};

	if (is_antimagic(pc_pos[current_pc].x,pc_pos[current_pc].y)) {
		add_string_to_buf("  Not in antimagic field.");
		return false;
		}
	if (spell_forced == false)
		spell_num = pick_spell(current_pc,1,2);
		else {
			if (repeat_cast_ok(1) == false)
				return false;
			spell_num = pc_last_cast[1][current_pc];
			}

	store_sp = adven[current_pc].cur_sp;

	if (spell_num >= 70)
		return false;

	bonus = adven[current_pc].statAdj(SKILL_INTELLIGENCE);

	combat_posing_monster = current_working_monster = current_pc;

	if (adven[current_pc].cur_sp == 0)
		add_string_to_buf("Cast: No spell points.        ");
	else if (spell_num < 70) {
			print_spell_cast(spell_num,1);
			if (refer_priest[spell_num] == 0) {
				take_ap(5);
				draw_terrain(2);
				do_priest_spell(current_pc,spell_num);
				}
				else if (refer_priest[spell_num] == 2) {
					start_spell_targeting(100 + spell_num);
					}
				else if (refer_priest[spell_num] == 3) {
					start_fancy_spell_targeting(100 + spell_num);
					}
				else {
					start_missile_anim();
					take_ap(5);
					draw_terrain(2);
					switch (spell_num) {
						case SPELL_PRIEST_MINOR_BLESS: case SPELL_PRIEST_BLESS:
//							target = select_pc(11,0);
								target = store_spell_target;
							if (target < 6) {
								store_sound = 4;
								adven[current_pc].cur_sp -= s_cost[1][spell_num];
								adven[target].status[STATUS_BLESS_CURSE] += (spell_num == SPELL_PRIEST_MINOR_BLESS) ? 2 :
									max(2,(adven[current_pc].level * 3) / 4 + 1 + bonus);
								sprintf (c_line, "  %s blessed.              ",
									adven[target].name);
								add_string_to_buf(c_line);
								add_missile(pc_pos[target],8,0,0,0);
								}
						break;

						case SPELL_PRIEST_BLESS_PARTY:
							adven[current_pc].cur_sp -= s_cost[1][spell_num];
							for (i = 0; i < 6; i++)
								if (adven[i].isAlive()) {
									adven[i].status[STATUS_BLESS_CURSE] += adven[current_pc].level / 3;
								add_missile(pc_pos[i],8,0,0,0);
								}
								sprintf (c_line, "  Party blessed.                    ");
								add_string_to_buf(c_line);
							store_sound = 4;
							break;

						case SPELL_PRIEST_AVATAR:
							adven[current_pc].cur_sp -= s_cost[1][spell_num];
							sprintf (c_line, "  %s is an avatar! ",
								adven[current_pc].name);
							add_string_to_buf(c_line);
							adven[current_pc].heal(200);
							adven[current_pc].cure(8);
							adven[current_pc].status[STATUS_BLESS_CURSE ] = 8;
							adven[current_pc].status[STATUS_HASTE_SLOW] = 8;
							adven[current_pc].status[STATUS_INVULNERABLE] = 3;
							adven[current_pc].status[STATUS_MAGIC_RESISTANCE] = 8;
							adven[current_pc].status[STATUS_WEBS] = 0;
							adven[current_pc].status[STATUS_DISEASE] = 0;
							adven[current_pc].status[STATUS_DUMB] = 0;
							adven[current_pc].status[STATUS_MARTYRS_SHIELD] = 8;
							break;

						case SPELL_PRIEST_CURSE_ALL: case SPELL_PRIEST_MASS_CHARM: case SPELL_PRIEST_PESTILENCE: //mass affecting spells
								adven[current_pc].cur_sp -= s_cost[1][spell_num];
								store_sound = 24;
								for (i = 0; i < T_M; i++) {
									if ((c_town.monst.dudes[i].active != 0) &&(c_town.monst.dudes[i].attitude % 2 == 1) &&
									 (dist(pc_pos[current_pc],c_town.monst.dudes[i].m_loc) <= priest_range[spell_num])) {
											which_m = &c_town.monst.dudes[i];
											switch (spell_num) {
												case SPELL_PRIEST_CURSE_ALL:
													which_m->curse(3 + bonus);
													store_m_type = 8;
													break;
												case SPELL_PRIEST_MASS_CHARM:
													which_m->charm(28 - bonus,0,0);
													store_m_type = 14;
													break;
												case SPELL_PRIEST_PESTILENCE:
													which_m->disease(3 + bonus);
													store_m_type = 0;
													break;
												}
											num_opp++;
											add_missile(c_town.monst.dudes[i].m_loc,store_m_type,0,
												14 * (which_m->m_d.x_width - 1),18 * (which_m->m_d.y_width - 1));
											}
									}

								break;

						case SPELL_PRIEST_PROTECTIVE_CIRCLE:
								adven[current_pc].cur_sp -= s_cost[1][spell_num];
								play_sound(24);
								add_string_to_buf("  Protective field created.");
								place_spell_pattern(protect_pat,pc_pos[current_pc],0,false,6);
							break;
					}
				}
			if (num_opp < 10)
				do_missile_anim((num_opp < 5) ? 50 : 25,pc_pos[current_pc],store_sound);
				else play_sound(store_sound);
			end_missile_anim();
			put_pc_screen();
		}

	combat_posing_monster = current_working_monster = -1;
	// Did anything actually get cast?
	if (store_sp == adven[current_pc].cur_sp)
		return false;
		else return true;
}

void start_spell_targeting(short num)
{
	// First, remember what spell was cast.
	spell_being_cast = num;

	// Then, is num >= 1000, it's a freebie, so remove the 1000
	if (num >= 1000)
		num -= 1000;
	sprintf (create_line, "  Target spell.                ");
	add_string_to_buf(create_line);
	if (num < 100)
		add_string_to_buf("  (Hit 'm' to cancel.)");
		else add_string_to_buf("  (Hit 'p' to cancel.)");
	overall_mode = MODE_SPELL_TARGET;
	current_spell_range = (num >= 100) ? priest_range[num - 100] : mage_range[num];
	current_pat = single;

	switch (num) {  // Different spells have different messages and diff. target shapes
		case SPELL_MAGE_SLEEP_CLOUD:
			current_pat = small_square;
			break;
		case SPELL_MAGE_DISPEL_FIELDS: case SPELL_MAGE_FIREBALL: case 100 + SPELL_PRIEST_FLAMESTRIKE :
		case 100 + SPELL_PRIEST_FORCEFIELD : case SPELL_MAGE_STINKING_CLOUD: //case 100 + SPELL_PRIEST_AWAKEN:
			current_pat = square;
			break;
		case SPELL_MAGE_CONFLAGRATION: case SPELL_MAGE_FIRE_STORM: case SPELL_MAGE_SHOCKSTORM: case SPELL_MAGE_WEB:
		case SPELL_MAGE_ANTIMAGIC_CLOUD: case 100 + SPELL_PRIEST_DIVINE_THUD: case 100 + SPELL_PRIEST_DISPEL_FIELDS:
		case SPELL_ITEMS_ICE_WALL_BALLS: //case 67: <= old ball style sleep bomb
			current_pat = radius2;
			break;
		case 100 + SPELL_PRIEST_PESTILENCE: case SPELL_ITEMS_GOO_BOMB: case SPELL_ITEMS_FOUL_VAPORS:
			current_pat = radius3;
			break;
		case SPELL_MAGE_WALL_OF_FORCE: case SPELL_MAGE_WALL_OF_ICE: case 100 + SPELL_PRIEST_WALL_OF_BLADES:
			add_string_to_buf("  (Hit space to rotate.)");
			force_wall_position = 0;
			current_pat = field[0];
			break;
		}
}

void start_fancy_spell_targeting(short num)
{
	short i;
	location null_loc = location(120,0);

	// First, remember what spell was cast.
	spell_being_cast = num;
		// Then, is num >= 1000, it's a freebie, so remove the 1000
	if (num >= 1000)
		num -= 1000;

	for (i = 0; i < 8; i++)
		spell_targets[i] = null_loc;
	sprintf (create_line, "  Target spell.                ");
	if (num < 100)
		add_string_to_buf("  (Hit 'm' to cancel.)");
		else add_string_to_buf("  (Hit 'p' to cancel.)");
	add_string_to_buf("  (Hit space to cast.)");
	add_string_to_buf(create_line);
	overall_mode = MODE_FANCY_TARGET;
	current_pat = single;
	current_spell_range = (num >= 100) ? priest_range[num - 100] : mage_range[num];

	switch (num) { // Assign special targeting shapes and number of targets
		case 100 + SPELL_PRIEST_SMITE: // smite
			num_targets_left = minmax(1,8,adven[current_pc].level / 4 + adven[current_pc].statAdj(SKILL_INTELLIGENCE) / 2);
			break;
		case 100 + SPELL_PRIEST_STICKS_TO_SNAKES: // sticks to snakes
			num_targets_left = adven[current_pc].level / 5 + adven[current_pc].statAdj(SKILL_INTELLIGENCE) / 2;
			break;
		case 100 + SPELL_PRIEST_SUMMON_HOST: // summon host
			num_targets_left = 5;
			break;
		case SPELL_MAGE_FLAME_ARROWS: // flame arrows
			num_targets_left = adven[current_pc].level / 4 + adven[current_pc].statAdj(SKILL_INTELLIGENCE) / 2;
			break;
		case SPELL_MAGE_VENOM_ARROWS: // venom arrows
			num_targets_left = adven[current_pc].level / 5 + adven[current_pc].statAdj(SKILL_INTELLIGENCE) / 2;
			break;
		case SPELL_MAGE_DEATH_ARROWS: case SPELL_MAGE_PARALYSIS: // death arrows, paralysis
			num_targets_left = adven[current_pc].level / 8 + adven[current_pc].statAdj(SKILL_INTELLIGENCE) / 3;
			break;
		case SPELL_MAGE_SPRAY_FIELDS: // spray fields
			num_targets_left = adven[current_pc].level / 5 + adven[current_pc].statAdj(SKILL_INTELLIGENCE) / 2;
			current_pat = t;
			break;
		case SPELL_MAGE_WEAK_SUMMONING: // summon 1
			num_targets_left = minmax(1,7,adven[current_pc].level / 4 + adven[current_pc].statAdj(SKILL_INTELLIGENCE) / 2);
			break;
		case SPELL_MAGE_SUMMONING: // summon 2
			num_targets_left = minmax(1,6,adven[current_pc].level / 6 + adven[current_pc].statAdj(SKILL_INTELLIGENCE) / 2);
			break;
		case SPELL_MAGE_MAJOR_SUMMON: // summon 3
			num_targets_left = minmax(1,5,adven[current_pc].level / 8 + adven[current_pc].statAdj(SKILL_INTELLIGENCE) / 2);
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

void process_fields()
{
	short i,j,k,r1;
	location loc;
	char qf[64][64];
	RECT r;

	if (is_out())
		return;

	if (quickfire)
	{
		r = c_town.town.in_town_rect.rect32();
		for (i = 0; i < town_size[town_type]; i++)
			for (j = 0; j < town_size[town_type]; j++)
				qf[i][j] = (is_quickfire(i,j)) ? 2 : 0;
		for (k = 0; k < ((is_combat()) ? 4 : 1); k++) {
			for (i = r.left + 1; i < r.right ; i++)
				for (j = r.top + 1; j < r.bottom ; j++)
					if (is_quickfire(i,j) > 0) {
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
					make_quickfire(i,j);
					}
			}
		}

	for (i = 0; i < T_M; i++)
		if (c_town.monst.dudes[i].active > 0)
			monst_inflict_fields(i);

	// First fry PCs, then call to handle damage to monsters
	processing_fields = true; // this, in hit_space, makes damage considered to come from whole party
	if (force_wall) {
		force_wall = false;
		for (i = 0; i < town_size[town_type]; i++)
			for (j = 0; j < town_size[town_type]; j++)
				if (is_force_wall(i,j)) {
							r1 = get_ran(3,1,6);
							loc.x = i; loc.y = j;
							hit_pcs_in_space(loc,r1,3,1,1);
					r1 = get_ran(1,1,6);
					if (r1 == 2)
						take_force_wall(i,j);
						else {
							force_wall = true;
							}
					}
		}
	if (fire_wall) {
		fire_wall = false;
		for (i = 0; i < town_size[town_type]; i++)
			for (j = 0; j < town_size[town_type]; j++)
				if (is_fire_wall(i,j)) {
							loc.x = i; loc.y = j;
							r1 = get_ran(2,1,6) + 1;
							 hit_pcs_in_space(loc,r1,1,1,1);
					r1 = get_ran(1,1,4);
					if (r1 == 2)
						take_fire_wall(i,j);
						else {
							fire_wall = true;
							}
					}
		}
	if (antimagic) {
		antimagic = false;
		for (i = 0; i < town_size[town_type]; i++)
			for (j = 0; j < town_size[town_type]; j++)
				if (is_antimagic(i,j)) {
					r1 = get_ran(1,1,8);
					if (r1 == 2)
						take_antimagic(i,j);
						else antimagic = true;
					}
		}
	if (scloud) {
		scloud = false;
		for (i = 0; i < town_size[town_type]; i++)
			for (j = 0; j < town_size[town_type]; j++)
				if (is_scloud(i,j)) {
					r1 = get_ran(1,1,4);
					if (r1 == 2)
						take_scloud(i,j);
						else {
							scloud_space(i,j);
							scloud = true;
							}
					}
		}
	if (sleep_field) {
		sleep_field = false;
		for (i = 0; i < town_size[town_type]; i++)
			for (j = 0; j < town_size[town_type]; j++)
				if (is_sleep_cloud(i,j)) {
					r1 = get_ran(1,1,4);
					if (r1 == 2)
						take_sleep_cloud(i,j);
						else {
							sleep_cloud_space(i,j);
							sleep_field = true;
							}
					}
		}
	if (ice_wall) {
		ice_wall = false;
		for (i = 0; i < town_size[town_type]; i++)
			for (j = 0; j < town_size[town_type]; j++)
				if (is_ice_wall(i,j)) {
							loc.x = i; loc.y = j;
							r1 = get_ran(3,1,6);
							hit_pcs_in_space(loc,r1,5,1,1);
					r1 = get_ran(1,1,6);
					if (r1 == 1)
						take_ice_wall(i,j);
						else {
							ice_wall = true;
							}
					}
		}
	if (blade_wall) {
		blade_wall = false;
		for (i = 0; i < town_size[town_type]; i++)
			for (j = 0; j < town_size[town_type]; j++)
				if (is_blade_wall(i,j)) {
							loc.x = i; loc.y = j;
							r1 = get_ran(6,1,8);
							hit_pcs_in_space(loc,r1,0,1,1);
					r1 = get_ran(1,1,5);
					if (r1 == 1)
						take_blade_wall(i,j);
						else {
							blade_wall = true;
							}
					}
		}

	processing_fields = false;
	monsters_going = true; // this changes who the damage is considered to come from
							// in hit_space

	if (quickfire) {
		for (i = 0; i < town_size[town_type]; i++)
			for (j = 0; j < town_size[town_type]; j++)
				if (is_quickfire(i,j)) {
					loc.x = i; loc.y = j;
					r1 = get_ran(2,1,8);
					hit_pcs_in_space(loc,r1,1,1,1);
					}
		}

	monsters_going = false;
}

void scloud_space(short m,short n)
{
	location target;
	short i;

	target.x = (char) m;
	target.y = (char) n;

	make_scloud(m,n);

	if (overall_mode >= MODE_COMBAT)
		for (i = 0; i < 6; i++)
			if (adven[i].isAlive())
				if (same_point(pc_pos[i],target) == true) {
					adven[i].curse(get_ran(1,1,2));
					}
	if (overall_mode < MODE_COMBAT)
		if (same_point(target,c_town.p_loc) == true) {
			for (i = 0; i < 6; i++)
				adven[i].curse(get_ran(1,1,2));
			}
}

void web_space(short m,short n)
{
	location target;
	short i;

	target.x = (char) m;
	target.y = (char) n;

	make_web(m,n);

	if (overall_mode >= MODE_COMBAT)
		for (i = 0; i < 6; i++)
			if (adven[i].isAlive())
				if (same_point(pc_pos[i],target) == true) {
						adven[i].web(3);
					}
	if (overall_mode < MODE_COMBAT)
		if (same_point(target,c_town.p_loc) == true) {
			for (i = 0; i < 6; i++)
					adven[i].web(3);
			}
}
void sleep_cloud_space(short m,short n)
{
	location target;
	short i;

	target.x = (char) m;
	target.y = (char) n;

	make_sleep_cloud(m,n);

	if (overall_mode >= MODE_COMBAT)
		for (i = 0; i < 6; i++)
			if (adven[i].isAlive())
				if (same_point(pc_pos[i],target) == true) {
					adven[i].sleep(3,11,0);
					}
	if (overall_mode < MODE_COMBAT)
		if (same_point(target,c_town.p_loc) == true) {
			for (i = 0; i < 6; i++)
				adven[i].sleep(3,11,0);
			}
}

void take_m_ap(short num,creature_data_type *monst)
{
	monst->m_d.ap = max(0,monst->m_d.ap - num);
}

short get_monst_sound(creature_data_type *attacker,short which_att) {
	short type,strength;

	type = (which_att == 0) ? attacker->m_d.a1_type :  attacker->m_d.a23_type;
	strength = attacker->m_d.a[which_att];

	switch (type) {
		case MONSTER_ATTACK_SLIMES:
			return 11;
			break;
		case MONSTER_ATTACK_PUNCHES:
			return 4;
			break;
		case MONSTER_ATTACK_CLAWS:
			return 9;
			break;
		case MONSTER_ATTACK_BITES:
			return 10;
			break;

		default:
			if (attacker->m_d.m_type == MONSTER_TYPE_HUMAN) {
				if (strength > 9)
					return 3;
					else return 2;
				}
			if ((attacker->m_d.m_type == MONSTER_TYPE_HUMAN) ||(attacker->m_d.m_type == MONSTER_TYPE_HUMANOID) ||(attacker->m_d.m_type == MONSTER_TYPE_GIANT) ){
				return 2;
				}
			if (attacker->m_d.m_type == MONSTER_TYPE_MAGE)
				return 1;
			if (attacker->m_d.m_type == MONSTER_TYPE_PRIEST)
				return 4;
			return 0;
			break;
		}
	return 0;
	}
