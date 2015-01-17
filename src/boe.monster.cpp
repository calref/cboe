
#include <cstdio>

#include "boe.global.h"

#include "classes.h"
#include "boe.locutils.h"
#include "boe.monster.h"
#include "boe.text.h"
#include "boe.specials.h"
#include "boe.items.h"
#include "soundtool.hpp"
#include "boe.graphics.h"
#include "boe.newgraph.h"
#include "boe.main.h"
#include "mathutil.hpp"
#include "graphtool.hpp"

extern eGameMode overall_mode;
extern short which_combat_type;
extern short spell_caster, missile_firer,current_monst_tactic;
extern short hit_chance[21];

extern location center;
extern short boom_gr[8],futzing;
extern bool processing_fields,monsters_going;
extern cUniverse univ;


short charm_odds[20] = {90,90,85,80,78, 75,73,60,40,30, 20,10,4,1,0, 0,0,0,0,0};

short out_enc_lev_tot(short which) {
	short count = 0,i;
	short num[7] = {22,8,4,4,3,2,1};
	
	if(univ.party.out_c[which].what_monst.cant_flee)
		return 10000;
	
	for(i = 0; i < 7; i++)
		if(univ.party.out_c[which].what_monst.monst[i] != 0)
			count += univ.scenario.scen_monsters[univ.party.out_c[which].what_monst.monst[i]].level * num[i];
	return count;
}

void create_wand_monst() {
	short r1,r2,r3,i = 0,num_tries = 0;
	location p_loc;
	
	r1 = get_ran(1,0,3);
	if(overall_mode == MODE_OUTDOORS) {
		if(!univ.out->wandering[r1].isNull()) {
			r2 = get_ran(1,0,3);
			while(point_onscreen(univ.out->wandering_locs[r2], global_to_local(univ.party.p_loc)) && num_tries++ < 100)
				r2 = get_ran(1,0,3);
			if(!is_blocked(univ.out->wandering_locs[r2]))
				place_outd_wand_monst(univ.out->wandering_locs[r2], univ.out->wandering[r1],0);
		}
	} else if(!univ.town->wandering[r1].isNull() && univ.town.countMonsters() <= 50
			  && univ.party.m_killed[univ.town.num] < univ.town->max_num_monst) {
		// won't place wandering if more than 50 monsters
		r2 = get_ran(1,0,3);
		while(point_onscreen(univ.town->wandering_locs[r2],univ.town.p_loc) &&
			  !loc_off_act_area(univ.town->wandering_locs[r2]) && num_tries++ < 100)
			r2 = get_ran(1,0,3);
		for(i = 0; i < 4; i++) {
			if(univ.town->wandering[r1].monst[i] != 0) { // place a monster
				p_loc = univ.town->wandering_locs[r2];
				p_loc.x += get_ran(1,0,4) - 2;
				p_loc.y += get_ran(1,0,4) - 2;
				if(!is_blocked(p_loc))
					place_monster(univ.town->wandering[r1].monst[i],p_loc);
			}
			p_loc = univ.town->wandering_locs[r2];
			p_loc.x += get_ran(1,0,4) - 2;
			p_loc.y += get_ran(1,0,4) - 2;
			r3 = get_ran(1,0,3);
			if(r3 >= 2 && !is_blocked(p_loc)) // place extra monsters?
				place_monster(univ.town->wandering[r1].monst[3],p_loc);
		}
	}
}

void place_outd_wand_monst(location where,cOutdoors::cWandering group,short forced) {
	short i = 0,j = 0;
	location l;
	
	
	while(i < 10) {
		if(!univ.party.out_c[i].exists || ((i == 9) && (forced > 0))) {
			if((sd_legit(group.end_spec1,group.end_spec2)) && (PSD[group.end_spec1][group.end_spec2] > 0))
				return;
			univ.party.out_c[i].exists = true;
			univ.party.out_c[i].direction = 0;
			univ.party.out_c[i].what_monst = group;
			univ.party.out_c[i].which_sector = univ.party.i_w_c;
			univ.party.out_c[i].m_loc = where;
			if(univ.party.out_c[i].which_sector.x == 1)
				univ.party.out_c[i].m_loc.x += 48;
			if(univ.party.out_c[i].which_sector.y == 1)
				univ.party.out_c[i].m_loc.y += 48;
			l = univ.party.out_c[i].m_loc;
			while((forced) && (is_blocked(l)) && (j < 50)) {
				l = univ.party.out_c[i].m_loc;
				l.x += get_ran(1,0,2) - 1;
				l.y += get_ran(1,0,2) - 1;
				j++;
			}
			univ.party.out_c[i].m_loc = l;
			
			i = 50;
		}
		i++;
	}
	
}

location get_monst_head(short m_num) {
	location l;
	
	l = univ.town.monst[m_num].cur_loc;
	if((univ.town.monst[m_num].direction < 4) &&
		(univ.town.monst[m_num].x_width > 1))
		l.x++;
	
	return l;
}

short get_monst_picnum(mon_num_t monst) {
	if(monst >= 10000) return univ.party.summons[monst - 10000].picture_num;
	return univ.scenario.scen_monsters[monst].picture_num;
}

ePicType get_monst_pictype(mon_num_t monst) {
	ePicType type = PIC_MONST;
	short n;
	if(monst >= 10000)
		n = univ.party.summons[monst - 10000].picture_num;
	else n = univ.scenario.scen_monsters[monst].picture_num;
	if(n >= 1000){
		type += PIC_CUSTOM;
		switch(n / 1000){
			case 2:
				type += PIC_WIDE;
				break;
			case 3:
				type += PIC_TALL;
				break;
			case 4:
				type += PIC_WIDE;
				type += PIC_TALL;
				break;
		}
	}else{
		if(m_pic_index[n].x == 2) type += PIC_WIDE;
		if(m_pic_index[n].y == 2) type += PIC_TALL;
	}
	return type;
}

void get_monst_dims(mon_num_t monst,short *width, short *height) {
	cMonster& the_monst = monst >= 10000 ? univ.party.summons[monst - 10000] : univ.scenario.scen_monsters[monst];
	*width = the_monst.x_width;
	*height = the_monst.y_width;
}

// Used to set up monsters for outdoor wandering encounters.
//mode; // 0 - unfriendly  1 - friendly & fightin'
void set_up_monst(short mode,mon_num_t m_num) {
	short which;
	
	for(which = 0; which < univ.town->max_monst(); which++)
		if(univ.town.monst[which].active == 0) {
			cMonster& monst = m_num >= 10000 ? univ.party.summons[m_num - 10000] : univ.scenario.scen_monsters[m_num];
			univ.town.monst.assign(which, cCreature(m_num), monst, PSD[SDF_EASY_MODE], univ.difficulty_adjust());
			univ.town.monst[which].active = 2;
			univ.town.monst[which].summoned = 0;
			univ.town.monst[which].attitude = mode + 1;
			univ.town.monst[which].mobility = 1;
			break;
		}
}

void do_monsters() {
	short i,j,r1,target;
	location l1,l2;
	bool acted_yet = false;
	
	if(overall_mode == MODE_TOWN)
		for(i = 0; i < univ.town->max_monst(); i++)
			if(univ.town.monst[i].active != 0 && univ.town.monst[i].status[eStatus::ASLEEP] <= 0
			   && univ.town.monst[i].status[eStatus::PARALYZED] <= 0) {
				// have to pick targets
				if(univ.town.monst[i].active == 1)
					target = 6;
				else {
					target = monst_pick_target(i); // will return 0 if target party
					target = switch_target_to_adjacent(i,target);
					if(target == 0) {
						if(dist(univ.town.monst[i].cur_loc,univ.town.p_loc) > 8)
							target = 6;
						else target = select_active_pc();
					}
					if((univ.town.monst[i].attitude % 2 != 1) && (target < 6))
						target = 6;
				}
				univ.town.monst[i].target = target;
//				sprintf((char *)debug,"  t: %d targets %d.",i,monst_target[i]);
//				add_string_to_buf((char *) debug);
				
				if((univ.town.monst[i].active == 2)
					|| ((univ.town.monst[i].active != 0) && (univ.town.monst[i].attitude % 2 != 1))) {
					acted_yet = false;
					if(((univ.town.monst[i].attitude == 0) || (univ.town.monst[i].target == 6)) && (univ.town.hostile == 0)) {
						if(univ.town.monst[i].mobility == 1) { // OK, it doesn't see the party or
							// isn't nasty, and the town isn't totally hostile.
							if((univ.town.monst[i].attitude % 2 != 1) || (get_ran(1,0,1) == 0)) {
								acted_yet = rand_move(i);
							}
							else acted_yet = seek_party(i,univ.town.monst[i].cur_loc,univ.town.p_loc);
						}
					}
					if((univ.town.monst[i].attitude > 0) || (univ.town.hostile == 1)) {
						if((univ.town.monst[i].mobility == 1) && (univ.town.monst[i].target != 6)) {
							l1 = univ.town.monst[i].cur_loc;
							l2 = (univ.town.monst[i].target <= 6) ? univ.town.p_loc : univ.town.monst[target - 100].cur_loc;
							
							if(univ.town.monst[i].morale < 0 && !univ.town.monst[i].mindless
								&& univ.town.monst[i].m_type != eRace::UNDEAD)  {
								acted_yet = flee_party(i,l1,l2);
								if(get_ran(1,0,10) < 6)
									univ.town.monst[i].morale++;
							}
							else if(monst_hate_spot(i,&l2))
								acted_yet = seek_party(i,l1,l2);
							else if(((univ.town.monst[i].mu == 0) && (univ.town.monst[i].mu == 0))
									 || (can_see_light(l1,l2,sight_obscurity) > 3))
								acted_yet = seek_party(i,l1,l2);
						}
					}
				}
				
				
				// Make hostile monsters active
				if((univ.town.monst[i].active == 1) && (univ.town.monst[i].attitude % 2 == 1)
					&& (dist(univ.town.monst[i].cur_loc,univ.town.p_loc) <= 8)) {
					r1 = get_ran(1,1,100);
					r1 += (univ.party.status[ePartyStatus::STEALTH] > 0) ? 46 : 0;
					r1 += can_see_light(univ.town.monst[i].cur_loc,univ.town.p_loc,sight_obscurity) * 10;
					if(r1 < 50) {
						univ.town.monst[i].active = 2;
						add_string_to_buf("Monster saw you!");
						// play go active sound
						if(isHumanoid(univ.town.monst[i].m_type) || univ.town.monst[i].m_type == eRace::GIANT)
							play_sound(18);
						else play_sound(46);
					}
					for(j = 0; j < univ.town->max_monst(); j++)
						if((univ.town.monst[j].active == 2)
							&& ((dist(univ.town.monst[i].cur_loc,univ.town.monst[j].cur_loc) <= 5)))
							univ.town.monst[i].active = 2;
				}
				
			}
	if(overall_mode == MODE_OUTDOORS) {
		for(i = 0; i < 10; i++)
			if(univ.party.out_c[i].exists) {
				acted_yet = false;
				l1 = univ.party.out_c[i].m_loc;
				l2 = univ.party.p_loc;
				
				r1 = get_ran(1,1,6);
				if(r1 == 3)
					acted_yet = rand_move(i);
				else acted_yet = seek_party(i,l1,l2);
			}
	}
}

bool monst_hate_spot(short which_m,location *good_loc) {
	location prospect,loc;
	
	loc = univ.town.monst[which_m].cur_loc;
	bool have_radiate = univ.town.monst[which_m].abil[eMonstAbil::RADIATE].active;
	eFieldType which_radiate = univ.town.monst[which_m].abil[eMonstAbil::RADIATE].radiate.type;
	bool hate_spot = false;
	// Hate barriers
	if(univ.town.is_fire_barr(loc.x,loc.y) || univ.town.is_force_barr(loc.x,loc.y)) hate_spot = true;
	// Hate quickfire
	else if(univ.town.is_quickfire(loc.x,loc.y)) hate_spot = true;
	// Hate blade wall?
	else if(univ.town.is_blade_wall(loc.x,loc.y)) {
		hate_spot = true;
		if(have_radiate && which_radiate == eFieldType::WALL_BLADES) hate_spot = false;
		else if(univ.town.monst[which_m].invuln) hate_spot = false;
	}
	// Hate ice wall?
	else if(univ.town.is_ice_wall(loc.x,loc.y)) {
		hate_spot = true;
		if(have_radiate && which_radiate == eFieldType::WALL_ICE) hate_spot = false;
		else if(univ.town.monst[which_m].cold_res == 0) hate_spot = false;
	}
	// Hate fire wall?
	else if(univ.town.is_fire_wall(loc.x,loc.y)) {
		hate_spot = true;
		if(have_radiate && which_radiate == eFieldType::WALL_FIRE) hate_spot = false;
		else if(univ.town.monst[which_m].fire_res == 0) hate_spot = false;
	}
	// Note: Monsters used to enter shock walls even if they were merely resistant to magic
	// Hate shock wall?
	else if(univ.town.is_force_wall(loc.x,loc.y)) {
		hate_spot = true;
		if(have_radiate && which_radiate == eFieldType::WALL_FORCE) hate_spot = false;
		else if(univ.town.monst[which_m].magic_res == 0) hate_spot = false;
	}
	// Hate stink cloud?
	else if(univ.town.is_scloud(loc.x,loc.y)) {
		hate_spot = true;
		if(have_radiate && which_radiate == eFieldType::CLOUD_STINK) hate_spot = false;
		else if(univ.town.monst[which_m].magic_res <= 50) hate_spot = false;
	}
	// Hate sleep cloud?
	else if(univ.town.is_sleep_cloud(loc.x,loc.y)) {
		hate_spot = true;
		if(have_radiate && which_radiate == eFieldType::CLOUD_SLEEP) hate_spot = false;
		else if(univ.town.monst[which_m].magic_res <= 50) hate_spot = false;
	}
	// Hate antimagic?
	else if(univ.town.is_antimagic(loc.x,loc.y)) {
		if(univ.town.monst[which_m].mu > 0 || univ.town.monst[which_m].cl > 0)
			hate_spot = true;
	}
	if(hate_spot) {
		prospect = find_clear_spot(loc,1);
		if(prospect.x > 0) {
			*good_loc = prospect;
			return true;
		}
		return false;
	}
	else return false;
}

short monst_pick_target(short which_m) {
	cCreature *cur_monst;
	short targ_pc,targ_m;
	
	cur_monst = &univ.town.monst[which_m];
	
	// First, any chance target is screwed?
	if(univ.town.monst[which_m].target >= 100) {
		if(((cur_monst->attitude % 2 == 1) &&
			 (univ.town.monst[univ.town.monst[which_m].target - 100].attitude == cur_monst->attitude)) ||
			((cur_monst->attitude % 2 == 0) && (univ.town.monst[univ.town.monst[which_m].target - 100].attitude % 2 == 0)))
			univ.town.monst[which_m].target = 6;
		else if(univ.town.monst[univ.town.monst[which_m].target - 100].active == 0)
			univ.town.monst[which_m].target = 6;
	}
	if(univ.town.monst[which_m].target < 6)
		if(univ.party[univ.town.monst[which_m].target].main_status != eMainStatus::ALIVE || get_ran(1,0,3) == 1)
			univ.town.monst[which_m].target = 6;
	
	if((is_combat()) && (cur_monst->attitude % 2 == 1)) {
		if(spell_caster < 6)
			if((get_ran(1,1,5) < 5) && (monst_can_see(which_m,univ.party[spell_caster].combat_pos))
				&& univ.party[spell_caster].main_status == eMainStatus::ALIVE)
				return spell_caster;
		if(missile_firer < 6)
			if((get_ran(1,1,5) < 3) && (monst_can_see(which_m,univ.party[missile_firer].combat_pos))
				&& univ.party[missile_firer].main_status == eMainStatus::ALIVE)
				return missile_firer;
		if(univ.town.monst[which_m].target < 6)
			if((monst_can_see(which_m,univ.party[univ.town.monst[which_m].target].combat_pos))
				&& univ.party[univ.town.monst[which_m].target].main_status == eMainStatus::ALIVE)
				return univ.town.monst[which_m].target;
	}
	
//	if(monst_target[which_m] >= 100) {
//		if((can_see_light(cur_monst->m_loc,univ.town.monst[monst_target[which_m] - 100].m_loc,sight_obscurity) < 4)
//			&& (univ.town.monst[monst_target[which_m] - 100].active > 0))
//				return monst_target[which_m];
//		}
	
	// Now pick a target pc and a target monst and see which is more attractive
	targ_pc = monst_pick_target_pc(which_m,cur_monst);
	targ_m = monst_pick_target_monst(cur_monst);
	
	if((targ_pc != 6) && (targ_m == 6))
		return targ_pc;
	if((targ_pc == 6) && (targ_m != 6))
		return targ_m;
	if((targ_pc == 6) && (targ_m == 6))
		return 6;
	
	if(is_town()) {
		if(cur_monst->attitude % 2 == 0) {
			return targ_m;
		}
		if((targ_m == 6) && (cur_monst->attitude % 2 == 1))
			return 0;
		if(dist(cur_monst->cur_loc,univ.town.monst[targ_m - 100].cur_loc) <
			dist(cur_monst->cur_loc,univ.town.p_loc))
			return targ_m;
		else return 0;
	}
	// Otherwise we're in combat
	if((dist(cur_monst->cur_loc,univ.town.monst[targ_m - 100].cur_loc) ==
		 dist(cur_monst->cur_loc,univ.party[targ_pc].combat_pos)) && (get_ran(1,0,6) < 3))
		return targ_m;
	else return targ_pc;
	if(dist(cur_monst->cur_loc,univ.town.monst[targ_m - 100].cur_loc) <
		dist(cur_monst->cur_loc,univ.party[targ_pc].combat_pos))
		return targ_m;
	else return targ_pc;
	
}

short monst_pick_target_monst(cCreature *which_m) {
	short min_dist = 1000,i,cur_targ = 6;
	
	for(i = 0; i < univ.town->max_monst(); i++) {
		if((univ.town.monst[i].active > 0) && // alive
			(((which_m->attitude % 2 == 1) && (univ.town.monst[i].attitude % 2 == 0)) ||
			 ((which_m->attitude % 2 == 0) && (univ.town.monst[i].attitude % 2 == 1)) ||
			 ((which_m->attitude % 2 == 1) && (univ.town.monst[i].attitude != which_m->attitude))) && // they hate each other
			((dist(which_m->cur_loc,univ.town.monst[i].cur_loc) < min_dist) ||
			 ((dist(which_m->cur_loc,univ.town.monst[i].cur_loc) == min_dist) && (get_ran(1,0,7) < 4))) &&
			(monst_can_see(i,univ.town.monst[i].cur_loc)) ) {
			min_dist = dist(which_m->cur_loc,univ.town.monst[i].cur_loc);
			cur_targ = i + 100;
			
		}
	}
	return cur_targ;
}

short monst_pick_target_pc(short m_num,cCreature *which_m) {
	short num_tries = 0,r1,store_targ = 6;
	
	if(which_m->attitude % 2 == 0)
		return 6;
	if(is_town())
		return 0;
	
	// First pick any visible, nearby PC
	r1 = get_ran(1,0,5);
	while(num_tries < 6 && (univ.party[r1].main_status != eMainStatus::ALIVE ||
			!monst_can_see(m_num,univ.party[r1].combat_pos))) {
		r1 = get_ran(1,0,5);
		num_tries++;
	}
	if(num_tries < 6)
		store_targ = r1;
	
	// Then, see if target can be replaced with someone nice and close
	r1 = get_ran(1,0,5);
	while(num_tries < 6 && (univ.party[r1].main_status != eMainStatus::ALIVE ||
			(dist(which_m->cur_loc,univ.party[r1].combat_pos) > 4) ||
			!monst_can_see(m_num,univ.party[r1].combat_pos))) {
		r1 = get_ran(1,0,5);
		num_tries++;
	}
	
	if(num_tries < 6)
		return r1;
	else return store_targ;
}

// returns 6 if no
short select_active_pc() {
	short r1, num_tries = 0;
	
	r1 = get_ran(1,0,5);
	while(univ.party[r1].main_status != eMainStatus::ALIVE && num_tries++ < 50)
		r1 = get_ran(1,0,5);
	
	return r1;
}

short closest_pc(location where) {
	short how_close = 200,i,store = 6;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE && dist(where,univ.party[i].combat_pos) < how_close) {
			store = i;
			how_close = dist(where,univ.party[i].combat_pos);
		}
	return store;
}

//mode;  // 1 - closest hostile to PCs  2 - closest friendly to PCs
short closest_monst(location where,short mode) {
	short how_close = 200,i,store = 6;
	
	for(i = 0; i < univ.town->max_monst(); i++)
		if((((univ.town.monst[i].attitude % 2 == 1) && (mode == 1)) ||
			 ((univ.town.monst[i].attitude % 2 == 0) && (mode == 2)))
			&& (dist(where,univ.town.monst[i].cur_loc) < how_close)) {
			store = i;
			how_close = dist(where,univ.town.monst[i].cur_loc);
		}
	return store;
}

short switch_target_to_adjacent(short which_m,short orig_target) {
	location monst_loc;
	short i,num_adj = 0;
	
	monst_loc = univ.town.monst[which_m].cur_loc;
	
	// First, take care of friendly monsters.
	if(univ.town.monst[which_m].attitude % 2 == 0) {
		if(orig_target >= 100)
			if((univ.town.monst[orig_target - 100].active > 0) &&
				(monst_adjacent(univ.town.monst[orig_target - 100].cur_loc,which_m)))
				return orig_target;
		for(i = 0; i < univ.town->max_monst(); i++)
			if((univ.town.monst[i].active > 0) &&
				(univ.town.monst[i].attitude % 2 == 1) &&
				(monst_adjacent(univ.town.monst[i].cur_loc,which_m)))
				return i + 100;
		return orig_target;
	}
	
	// If we get here while in town, just need to check if switch to pc
	if((is_town()) && (monst_adjacent(univ.town.p_loc,which_m)))
		return 0;
	if(is_town())
		return orig_target;
	
	// If target is already adjacent, we're done here.
	if((is_combat()) && (orig_target < 6))
		if(univ.party[orig_target].main_status == eMainStatus::ALIVE && monst_adjacent(univ.party[orig_target].combat_pos,which_m))
			return orig_target;
	if(orig_target >= 100)
		if((univ.town.monst[orig_target - 100].active > 0) &&
			(monst_adjacent(univ.town.monst[orig_target - 100].cur_loc,which_m)))
			return orig_target;
	
	// Anyone unarmored? Heh heh heh...
	if(is_combat())
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE && monst_adjacent(univ.party[i].combat_pos,which_m) &&
			   (get_encumberance(i) < 2))
		 		return i;
	
	// Check for a nice, adjacent, friendly monster and maybe attack
	for(i = 0; i < univ.town->max_monst(); i++)
		if((univ.town.monst[i].active > 0) &&
			(univ.town.monst[i].attitude % 2 == 0) &&
			(monst_adjacent(univ.town.monst[i].cur_loc,which_m)) &&
			(get_ran(1,0,2) < 2))
			return i + 100;
	
	// OK. Now if this monster has PCs adjacent, pick one at randomn and hack. Otherwise,
	// stick with orig. target.
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE && monst_adjacent(univ.party[i].combat_pos,which_m))
			num_adj++;
	
	if(num_adj == 0)
		return orig_target;
	
	i = 0;
	num_adj = get_ran(1,1,num_adj);
	while(num_adj > 1 || univ.party[i].main_status != eMainStatus::ALIVE || !monst_adjacent(univ.party[i].combat_pos,which_m)) {
		if(univ.party[i].main_status == eMainStatus::ALIVE && monst_adjacent(univ.party[i].combat_pos,which_m))
			num_adj--;
		i++;
	}
	return i;
}


bool rand_move(mon_num_t i) {
	bool acted_yet = false;
	short j;
	location store_loc;
	
	// first, if outdoor, just roam.
	if(is_out()) {
		store_loc = random_shift(univ.party.out_c[i].m_loc);
		return outdoor_move_monster(i,store_loc);
	}
	
	if(univ.town.monst[i].targ_loc == univ.town.monst[i].cur_loc)
		univ.town.monst[i].targ_loc.x = 0;
	
	// FIrst, try to move to monst_targs. If it don't work, then we'll shift.
	if(univ.town.monst[i].targ_loc.x > 0)
		acted_yet = seek_party(i,univ.town.monst[i].cur_loc,univ.town.monst[i].targ_loc);
	
	if(!acted_yet) {
		univ.town.monst[i].targ_loc.x = 0;
		for(j = 0; j < 3; j++) {
			store_loc = univ.town.monst[i].cur_loc;
			store_loc.x += get_ran(1,0,24) - 12;
			store_loc.y += get_ran(1,0,24) - 12;
			if(!loc_off_act_area(store_loc) && (can_see_light(univ.town.monst[i].cur_loc,store_loc,sight_obscurity) < 5)) {
				univ.town.monst[i].targ_loc = store_loc; j = 3;
			}
		}
		
		if(univ.town.monst[i].targ_loc.x == 0) {
			// maybe pick a wand loc, else juist pick a loc
			j = get_ran(1,0,3);
			store_loc = univ.town->wandering_locs[j];
			
			if(!loc_off_act_area(store_loc) && (get_ran(1,0,1) == 1))
				univ.town.monst[i].targ_loc = store_loc;
			else {
				store_loc = univ.town.monst[i].cur_loc;
				store_loc.x += get_ran(1,0,20) - 10;
				store_loc.y += get_ran(1,0,20) - 10;
				if(!loc_off_act_area(store_loc))
					univ.town.monst[i].targ_loc = store_loc;
			}
		}
		if(univ.town.monst[i].targ_loc.x > 0)
			acted_yet = seek_party(i,univ.town.monst[i].cur_loc,univ.town.monst[i].targ_loc);
	}
	
	return acted_yet;
}





bool seek_party(short i,location l1,location l2) {
	bool acted_yet = false;
	short m,n;
	if((l1.x > l2.x) && (l1.y > l2.y))
		acted_yet = try_move(i,l1,-1,-1);
	if((l1.x < l2.x) & (l1.y < l2.y) & !acted_yet)
		acted_yet = try_move(i,l1,1,1);
	if((l1.x > l2.x) & (l1.y < l2.y) & !acted_yet)
		acted_yet = try_move(i,l1,-1,1);
	if((l1.x < l2.x) & (l1.y > l2.y) & !acted_yet)
		acted_yet = try_move(i,l1,1,-1);
	if((l1.x > l2.x) & !acted_yet)
		acted_yet = try_move(i,l1,-1,0);
	if((l1.x < l2.x) & !acted_yet)
		acted_yet = try_move(i,l1,1,0);
	if( (l1.y < l2.y) & !acted_yet)
		acted_yet = try_move(i,l1,0,1);
	if( (l1.y > l2.y) & !acted_yet)
		acted_yet = try_move(i,l1,0,-1);
	if(!acted_yet) {
		futzing++;
		m = get_ran(1,0,2) - 1;
		n = get_ran(1,0,2) - 1;
		acted_yet = try_move(i,l1,m,n);
	}
	return acted_yet;
}

bool flee_party(short i,location l1,location l2) {
	bool acted_yet = false;
	
	if((l1.x > l2.x) & (l1.y > l2.y))
		acted_yet = try_move(i,l1,1,1);
	if((l1.x < l2.x) & (l1.y < l2.y) & !acted_yet)
		acted_yet = try_move(i,l1,-1,-1);
	if((l1.x > l2.x) & (l1.y < l2.y) & !acted_yet)
		acted_yet = try_move(i,l1,1,-1);
	if((l1.x < l2.x) & (l1.y > l2.y) & !acted_yet)
		acted_yet = try_move(i,l1,-1,+1);
	if((l1.x > l2.x) & !acted_yet)
		acted_yet = try_move(i,l1,1,0);
	if((l1.x < l2.x) & !acted_yet)
		acted_yet = try_move(i,l1,-1,0);
	if( (l1.y < l2.y) & !acted_yet)
		acted_yet = try_move(i,l1,0,-1);
	if( (l1.y > l2.y) & !acted_yet)
		acted_yet = try_move(i,l1,0,1);
	if(!acted_yet) {
		futzing++;
		acted_yet = rand_move(i);
	}
	return acted_yet;
}

bool try_move(short i,location start,short x,short y) {
	location dest;
	
	dest = start;
	dest.x = dest.x + x;
	dest.y = dest.y + y;
	
	if((overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) && univ.town.is_force_cage(start.x,start.y))
		return false;
	
	if(overall_mode == MODE_TOWN)
		return town_move_monster(i,dest);
	if(overall_mode == MODE_OUTDOORS)
		return outdoor_move_monster(i,dest);
	if(overall_mode == MODE_COMBAT)
		return combat_move_monster((short) i,dest);
	return 0;
}

bool combat_move_monster(short which,location destination) {
	
	
	if(!monst_can_be_there(destination,which))
		return false;
	else if(!monst_check_special_terrain(destination,2,which))
		return false;
	else {
		univ.town.monst[which].direction = set_direction(univ.town.monst[which].cur_loc, destination);
		univ.town.monst[which].cur_loc = destination;
		monst_inflict_fields(which);
		
		if(point_onscreen(destination,center))
			move_sound(univ.town->terrain(destination.x,destination.y), univ.town.monst[which].ap);
		
		return true;
	}
	return false;
}

// Looks at all spaces within 2, looking for a spot which is clear of nastiness and beings
// returns {0,0} if none found
// TODO: THIS MAKES NO ADJUSTMENTS FOR BIG MONSTERS!!!
//mode; // 0 - normal  1 - prefer adjacent space
location find_clear_spot(location from_where,short mode) {
	location loc,store_loc;
	short num_tries = 0,r1;
	// Here 254 indicates the low byte of the town fields, minus explored spaces (which is lowest bit).
	unsigned long blocking_fields = OBJECT_CRATE | OBJECT_BARREL | OBJECT_BLOCK | FIELD_QUICKFIRE | 254;
	
	while(num_tries < 75) {
		num_tries++;
		loc = from_where;
		r1 = get_ran(1,-2,2);
		loc.x = loc.x + r1;
		r1 = get_ran(1,-2,2);
		loc.y = loc.y + r1;
		if(!loc_off_act_area(loc) && !is_blocked(loc)
			&& can_see_light(from_where,loc,combat_obscurity) == 0
			&& (!(is_combat()) || (pc_there(loc) == 6))
			&& (!(is_town()) || (loc != univ.town.p_loc))
			&& (!(univ.town.fields[loc.x][loc.y] & blocking_fields))) {
			if((mode == 0) || ((mode == 1) && (adjacent(from_where,loc))))
				return loc;
			else store_loc = loc;
		}
	}
	return store_loc;
}

short pc_there(location where) {
	short i;
	
	for(i = 0; i < 6; i++)
		if(where == univ.party[i].combat_pos && univ.party[i].main_status == eMainStatus::ALIVE)
			return i;
	return 6;
}

location random_shift(location start) {
	location store;
	
	store = start;
	store.x = store.x + get_ran(1,0,2) - 1;
	store.y = store.y + get_ran(1,0,2) - 1;
	
	return store;
}

bool outdoor_move_monster(short num,location dest) {
	
	if(!outd_is_blocked(dest) && !outd_is_special(dest) &&
		(dest != univ.party.p_loc) &&
	   // TODO: Don't hard-code terrain types!
		((univ.out[dest.x][dest.y] > 21) || (univ.out[dest.x][dest.y] < 5))) {
		univ.party.out_c[num].direction =
		set_direction(univ.party.out_c[num].m_loc, dest);
		univ.party.out_c[num].m_loc = dest;
		return true;
	}
	else return false;
}

bool town_move_monster(short num,location dest) {
	if(!monst_check_special_terrain(dest,1,num))
		return false;
	
	if(monst_can_be_there(dest,num)) {
		univ.town.monst[num].direction = set_direction(univ.town.monst[num].cur_loc, dest);
		univ.town.monst[num].cur_loc = dest;
		monst_inflict_fields(num);
		return true;
	}
	else return false;
}

bool monster_placid(short m_num) {
	if((univ.town.monst[m_num].attitude == 0) ||
		((univ.town.monst[m_num].attitude == 2) && (PSD[SDF_HOSTILES_PRESENT] == 0))) {
		return true;
	} else {
		return false;
	}
}

// This damages a monster by any fields it's in, and destroys any barrels or crates
// it's stiing on.
void monst_inflict_fields(short which_monst) {
	short i,j,r1,k;
	location where_check;
	cCreature *which_m;
	
	if(univ.town.monst[which_monst].active == 0)
		return;
	
	which_m = &univ.town.monst[which_monst];
	bool have_radiate = which_m->abil[eMonstAbil::RADIATE].active;
	eFieldType which_radiate = which_m->abil[eMonstAbil::RADIATE].radiate.type;
	for(i = 0; i < univ.town.monst[which_monst].x_width; i++)
		for(j = 0; j < univ.town.monst[which_monst].y_width; j++)
			if(univ.town.monst[which_monst].active > 0) {
				where_check.x = univ.town.monst[which_monst].cur_loc.x + i;
				where_check.y = univ.town.monst[which_monst].cur_loc.y + j;
				// TODO: If the goal is to damage the monster by any fields it's on, why all the break statements?
				if(univ.town.is_quickfire(where_check.x,where_check.y)) {
					r1 = get_ran(2,1,8);
					damage_monst(which_monst,7,r1,0,eDamageType::FIRE,0);
					break;
				}
				if(univ.town.is_blade_wall(where_check.x,where_check.y)) {
					r1 = get_ran(6,1,8);
					if(have_radiate && which_radiate != eFieldType::WALL_BLADES)
						damage_monst(which_monst,7,r1,0,eDamageType::WEAPON,0);
					break;
				}
				if(univ.town.is_force_wall(where_check.x,where_check.y)) {
					r1 = get_ran(3,1,6);
					if(have_radiate && which_radiate != eFieldType::WALL_FORCE)
						damage_monst(which_monst,7,r1,0,eDamageType::MAGIC,0);
					break;
				}
				if(univ.town.is_sleep_cloud(where_check.x,where_check.y)) {
					if(have_radiate && which_radiate != eFieldType::CLOUD_SLEEP)
						charm_monst(which_m,0,eStatus::ASLEEP,3);
					break;
				}
				if(univ.town.is_ice_wall(where_check.x,where_check.y)) {
					r1 = get_ran(3,1,6);
					if(have_radiate && which_radiate != eFieldType::WALL_ICE)
						damage_monst(which_monst,7,r1,0,eDamageType::COLD,0);
					break;
				}
				if(univ.town.is_scloud(where_check.x,where_check.y)) {
					r1 = get_ran(1,2,3);
					if(have_radiate && which_radiate != eFieldType::CLOUD_STINK)
						curse_monst(which_m,r1);
					break;
				}
				if(univ.town.is_web(where_check.x,where_check.y) && which_m->m_type != eRace::BUG) {
					monst_spell_note(which_m->number,19);
					r1 = get_ran(1,2,3);
					web_monst(which_m,r1);
					univ.town.set_web(where_check.x,where_check.y,false);
					break;
				}
				if(univ.town.is_fire_wall(where_check.x,where_check.y)) {
					r1 = get_ran(2,1,6);
					if(have_radiate && which_radiate != eFieldType::WALL_FIRE)
						damage_monst(which_monst,7,r1,0,eDamageType::FIRE,0);
					break;
				}
				if(univ.town.is_force_cage(where_check.x,where_check.y))
					univ.town.monst[which_monst].status[eStatus::FORCECAGE] = 8;
				else univ.town.monst[which_monst].status[eStatus::FORCECAGE] = 0;
			}
	if(univ.town.monst[which_monst].active > 0)
		for(i = 0; i < univ.town.monst[which_monst].x_width; i++)
			for(j = 0; j < univ.town.monst[which_monst].y_width; j++) {
				where_check.x = univ.town.monst[which_monst].cur_loc.x + i;
				where_check.y = univ.town.monst[which_monst].cur_loc.y + j;
				if((univ.town.is_crate(where_check.x,where_check.y)) ||
					(univ.town.is_barrel(where_check.x,where_check.y)) )
					for(k = 0; k < NUM_TOWN_ITEMS; k++)
						if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].contained
						   && (univ.town.items[k].item_loc == where_check))
							univ.town.items[k].contained = false;
				univ.town.set_crate(where_check.x,where_check.y,false);
				univ.town.set_barrel(where_check.x,where_check.y,false);
				if(univ.town.is_fire_barr(where_check.x,where_check.y)) {
					r1 = get_ran(2,1,10);
					damage_monst(which_monst,7,r1,0,eDamageType::FIRE,0);
				}
			}
	
	
	
}

//mode; // 1 - town 2 - combat
bool monst_check_special_terrain(location where_check,short mode,short which_monst) {
	ter_num_t ter = 0;
	short r1,i,guts = 0;
	bool can_enter = true,mage = false;
	location from_loc,to_loc;
	bool do_look = false; // If becomes true, terrain changed, so need to update what party sees
	cCreature *which_m;
	eTerSpec ter_abil;
	unsigned short ter_flag;
	
	from_loc = univ.town.monst[which_monst].cur_loc;
	ter = univ.town->terrain(where_check.x,where_check.y);
	////
	which_m = &univ.town.monst[which_monst];
	ter_abil = univ.scenario.ter_types[ter].special;
	ter_flag = univ.scenario.ter_types[ter].flag3.u;
	
	if(mode > 0 && ter_abil == eTerSpec::CONVEYOR) {
		if(
			((ter_flag == DIR_N) && (where_check.y > from_loc.y)) ||
			((ter_flag == DIR_E) && (where_check.x < from_loc.x)) ||
			((ter_flag == DIR_S) && (where_check.y < from_loc.y)) ||
			((ter_flag == DIR_W) && (where_check.x > from_loc.x)) ) {
			return false;
		}
	}
	
	// begin determining guts, which determines how enthused the monst is about entering
	// nasty barriers
	if((which_m->mu > 0) || (which_m->cl > 0))
		mage = true;
	if(which_m->mindless)
		guts = 20;
	else guts = get_ran(1,1,(which_m->level / 2));
	guts += which_m->health / 20;
	if(mage)
		guts = guts / 2;
	if(which_m->attitude == 0)
		guts = guts / 2;
	
	if((univ.town.is_antimagic(where_check.x,where_check.y)) && (mage))
		return false;
	bool have_radiate = which_m->abil[eMonstAbil::RADIATE].active;
	eFieldType which_radiate = which_m->abil[eMonstAbil::RADIATE].radiate.type;
	if(univ.town.is_fire_wall(where_check.x,where_check.y) && !(have_radiate && which_radiate == eFieldType::WALL_FIRE)) {
		if(guts < 3) return false;
	}
	if(univ.town.is_force_wall(where_check.x,where_check.y) && !(have_radiate && which_radiate == eFieldType::WALL_FORCE)) {
		if(guts < 4) return false;
	}
	if(univ.town.is_ice_wall(where_check.x,where_check.y) && !(have_radiate && which_radiate == eFieldType::WALL_ICE)) {
		if(guts < 5) return false;
	}
	if(univ.town.is_sleep_cloud(where_check.x,where_check.y) && !(have_radiate && which_radiate == eFieldType::CLOUD_SLEEP)) {
		if(guts < 8) return false;
	}
	if(univ.town.is_blade_wall(where_check.x,where_check.y) && !(have_radiate && which_radiate == eFieldType::WALL_BLADES)) {
		if(guts < 8) return false;
	}
	if(univ.town.is_quickfire(where_check.x,where_check.y)) {
		if(guts < 8) return false;
	}
	if(univ.town.is_scloud(where_check.x,where_check.y) && !(have_radiate && which_radiate == eFieldType::CLOUD_STINK)) {
		if(guts < 4) return false;
	}
	if(univ.town.is_web(where_check.x,where_check.y) && which_m->m_type != eRace::BUG
		&& !(have_radiate && which_radiate == eFieldType::FIELD_WEB)) {
		if(guts < 3) return false;
	}
	if(univ.town.is_fire_barr(where_check.x,where_check.y)) {
		if((which_m->attitude % 2 == 1) && (get_ran(1,1,100) < (which_m->mu * 10 + which_m->cl * 4))) {
			// TODO: Are these barrier sounds right?
			play_sound(60);
			monst_spell_note(which_monst, 49);
			univ.town.set_fire_barr(where_check.x,where_check.y,false);
		}
		else {
			if(guts < 6) return false;
			r1 = get_ran(1,0,10);
			if((r1 < 8) || (monster_placid(which_monst)))
				can_enter = false;
		}
	}
	if(univ.town.is_force_barr(where_check.x,where_check.y)) { /// Not in big towns
		if((which_m->attitude % 2 == 1) && (get_ran(1,1,100) < (which_m->mu * 10 + which_m->cl * 4))
			&& (!univ.town->strong_barriers)) {
			play_sound(60);
			monst_spell_note(which_monst, 49);
			univ.town.set_force_barr(where_check.x,where_check.y,false);
		}
		else can_enter = false;
	}
	if(univ.town.is_force_cage(where_check.x,where_check.y)) can_enter = false;
	if(univ.town.is_crate(where_check.x,where_check.y)) {
		if(monster_placid(which_monst))
			can_enter = false;
		else {
			to_loc = push_loc(from_loc,where_check);
			univ.town.set_crate(where_check.x,where_check.y,false);
			if(to_loc.x > 0)
				univ.town.set_crate(to_loc.x,to_loc.y, true);
			for(i = 0; i < NUM_TOWN_ITEMS; i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where_check
				   && (univ.town.items[i].contained))
					univ.town.items[i].item_loc = to_loc;
		}
	}
	if(univ.town.is_barrel(where_check.x,where_check.y)) {
		if(monster_placid(which_monst))
			can_enter = false;
		else {
			to_loc = push_loc(from_loc,where_check);
			univ.town.set_barrel(where_check.x,where_check.y,false);
			if(to_loc.x > 0)
				univ.town.set_barrel(to_loc.x,to_loc.y,true);
			for(i = 0; i < NUM_TOWN_ITEMS; i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where_check
				   && (univ.town.items[i].contained))
					univ.town.items[i].item_loc = to_loc;
			
		}
	}
	if(monster_placid(which_monst) && // monsters don't hop into bed when things are calm
		univ.scenario.ter_types[ter].special == eTerSpec::BED)
		can_enter = false;
	if(mode == 1 && univ.town.is_spot(where_check.x, where_check.y))
		can_enter = false;
	if(ter == 90) {
		if((is_combat()) && (which_combat_type == 0)) {
			univ.town.monst[which_monst].active = 0;
			add_string_to_buf("Monster escaped! ");
		}
		return false;
	}
	
	switch(ter_abil) {
			// changing ter
		case eTerSpec::CHANGE_WHEN_STEP_ON:
			can_enter = false;
			if(!(monster_placid(which_monst))) {
				univ.town->terrain(where_check.x,where_check.y) = univ.scenario.ter_types[ter].flag1.u;
				do_look = true;
				if(point_onscreen(center,where_check))
					play_sound(univ.scenario.ter_types[ter].flag2.u);
			}
			break;
			
		case eTerSpec::BLOCKED_TO_MONSTERS:
		case eTerSpec::TOWN_ENTRANCE:
		case eTerSpec::WATERFALL:
			can_enter = false;
			break;
			
		case eTerSpec::DAMAGING: // TODO: Update this to check other cases
			switch(eDamageType(ter_flag)) {
				case eDamageType::FIRE:
					return univ.town.monst[which_monst].fire_res == 0;
				case eDamageType::COLD:
					return univ.town.monst[which_monst].cold_res == 0;
				case eDamageType::MAGIC:
					return univ.town.monst[which_monst].magic_res == 0;
				case eDamageType::POISON:
					return univ.town.monst[which_monst].poison_res == 0;
				default:
					return univ.town.monst[which_monst].invuln;
			}
			// TODO: Should it check any other terrain specials?
	}
	
	// Action may change terrain, so update what's been seen
	if(do_look) {
		if(is_town())
			update_explored(univ.town.p_loc);
		if(is_combat())
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE)
					update_explored(univ.party[i].combat_pos);
	}
	
	return can_enter;
}

void forced_place_monster(mon_num_t which,location where) {
	// TODO: Windows version has logic to destroy unimportant monsters to make room, but the real issue here is the non-dynamic nature of the monster array, so a better fix would be to use an std::vector in cPopulation... and then this function wouldn't even be needed.
	bool free_spot = false;
	short i = 0,r1;
	
	while(!free_spot && (i < univ.town->max_monst())) {
		if(univ.town.monst[i].active == 0)
			free_spot = true;
		i++;
	}
	do
		r1 = get_ran(1,0,59);
	while((univ.town.monst[r1].spec1 != 0) || (univ.town.monst[r1].spec2 != 0));
	if(!free_spot)
		univ.town.monst[r1].active = 0;
	place_monster(which,where);
}

void magic_adjust(cCreature *which_m,short *how_much) {
	if(*how_much <= 0) return;
	if(which_m->abil[eMonstAbil::ABSORB_SPELLS].active) {
		*how_much = 0;
		if(32767 - which_m->health > 3)
			which_m->health = 32767;
		else which_m->health += 3;
	}
	*how_much *= which_m->magic_res;
	*how_much /= 100;
}

void poison_monst(cCreature *which_m,short how_much) {
	if(how_much > 0) {
		how_much *= which_m->poison_res;
		how_much /= 100;
	}
	which_m->status[eStatus::POISON] = min(8, which_m->status[eStatus::POISON] + how_much);
	if(how_much >= 0)
		monst_spell_note(which_m->number,(how_much == 0) ? 10 : 4);
	else
		monst_spell_note(which_m->number,34);
	
}
void acid_monst(cCreature *which_m,short how_much) {
	magic_adjust(which_m,&how_much);
	which_m->status[eStatus::ACID] = minmax(-8,8, which_m->status[eStatus::ACID] + how_much);
	if(how_much >= 0)
		monst_spell_note(which_m->number,31);
	else
		monst_spell_note(which_m->number,48);
}

void slow_monst(cCreature *which_m,short how_much) {
	magic_adjust(which_m,&how_much);
	which_m->status[eStatus::HASTE_SLOW] = minmax(-8,8, which_m->status[eStatus::HASTE_SLOW] - how_much);
	if(how_much >= 0)
		monst_spell_note(which_m->number,(how_much == 0) ? 10 : 2);
	else
		monst_spell_note(which_m->number,35);
	
}
void curse_monst(cCreature *which_m,short how_much) {
	magic_adjust(which_m,&how_much);
	which_m->status[eStatus::BLESS_CURSE] = minmax(-8,8, which_m->status[eStatus::BLESS_CURSE] - how_much);
	if(how_much >= 0)
		monst_spell_note(which_m->number,(how_much == 0) ? 10 : 5);
	else
		monst_spell_note(which_m->number,36);
	
}
void web_monst(cCreature *which_m,short how_much) {
	magic_adjust(which_m,&how_much);
	which_m->status[eStatus::WEBS] = minmax(-8,8, which_m->status[eStatus::WEBS] + how_much);
	if(how_much >= 0)
		monst_spell_note(which_m->number,(how_much == 0) ? 10 : 19);
	else
		monst_spell_note(which_m->number,37);
	
}
void scare_monst(cCreature *which_m,short how_much) {
	magic_adjust(which_m,&how_much);
	which_m->morale = which_m->morale - how_much;
	// TODO: I don't think there's currently any way to increase monster morale at the moment - add one!
	if(how_much >= 0)
		monst_spell_note(which_m->number,(how_much == 0) ? 10 : 1);
	else
		monst_spell_note(which_m->number,47);
	
}
void disease_monst(cCreature *which_m,short how_much) {
	magic_adjust(which_m,&how_much);
	which_m->status[eStatus::DISEASE] = minmax(-8,8, which_m->status[eStatus::DISEASE] + how_much);
	if(how_much >= 0)
		monst_spell_note(which_m->number,(how_much == 0) ? 10 : 25);
	else
		monst_spell_note(which_m->number,38);
	
}

void dumbfound_monst(cCreature *which_m,short how_much) {
	magic_adjust(which_m,&how_much);
	which_m->status[eStatus::DUMB] = minmax(-8,8, which_m->status[eStatus::DUMB] + how_much);
	if(how_much >= 0)
		monst_spell_note(which_m->number,(how_much == 0) ? 10 : 22);
	else
		monst_spell_note(which_m->number,39);
	
}

// Also used for sleep and paralyze.
// For charm, amount is the resulting attitude of the charmed monster; if 0, attitude is 2.
void charm_monst(cCreature *which_m,short penalty,eStatus which_status,short amount) {
	short r1;
	
	if(which_status != eStatus::CHARM && which_status != eStatus::FORCECAGE && amount < 0) {
		which_m->status[which_status] -= amount;
		if(which_status == eStatus::PARALYZED)
			which_m->status[which_status] = max(0, which_m->status[which_status]);
		return;
	}
	
	if((which_status == eStatus::ASLEEP) &&
		(which_m->m_type == eRace::UNDEAD || which_m->m_type == eRace::SLIME ||
		 which_m->m_type == eRace::STONE || which_m->m_type == eRace::PLANT))
		return;
	r1 = get_ran(1,1,100);
	if(which_m->magic_res > 0) {
		r1 *= 100;
		r1 /= which_m->magic_res;
	} else r1 = 200;
	r1 += penalty;
	if(which_status == eStatus::ASLEEP)
		r1 -= 25;
	if(which_status == eStatus::PARALYZED)
		r1 -= 15;
	if(which_status == eStatus::ASLEEP && which_m->abil[eMonstAbil::FIELD].active && which_m->abil[eMonstAbil::FIELD].gen.fld == eFieldType::CLOUD_SLEEP)
		return;
	
	if(r1 > charm_odds[which_m->level / 2]) {
		//one_sound(68);
		monst_spell_note(which_m->number,10);
	}
	else {
		if(which_status == eStatus::CHARM) {
			if(amount == 0 || amount > 3) amount = 2;
			which_m->attitude = amount;
			monst_spell_note(which_m->number,23);
		} else if(which_status == eStatus::FORCECAGE) {
			which_m->status[eStatus::FORCECAGE] = 8;
			univ.town.set_force_cage(which_m->cur_loc.x, which_m->cur_loc.y, true);
			monst_spell_note(which_m->number,52);
		} else {
			which_m->status[which_status] = amount;
			if(which_status == eStatus::ASLEEP && (amount >= 0))
				monst_spell_note(which_m->number,28);
			if(which_status == eStatus::PARALYZED && (amount >= 0))
				monst_spell_note(which_m->number,30);
			if(amount < 0)
				monst_spell_note(which_m->number,40);
		}
		//one_sound(53);
	}
}
void record_monst(cCreature *which_m) {
	short r1;
	
	r1 = get_ran(1,1,100);
	r1 = (r1 * 7) / 10;
	
	if((which_m->x_width > 1) || (which_m->y_width > 1)) {
		ASB("Capture Soul: Monster is too big.");
	}
	// TODO: Are these two sounds right?
	else if(r1 > charm_odds[which_m->level / 2] || which_m->abil[eMonstAbil::SPLITS].active
			 || which_m->m_type == eRace::IMPORTANT) {
		monst_spell_note(which_m->number,10);
		play_sound(68);
	}
	else {
		monst_spell_note(which_m->number,24);
		r1 = get_ran(1,0,3);
		if(univ.party.imprisoned_monst[r1] == 0)
			univ.party.imprisoned_monst[r1] = which_m->number;
		else {
			r1 = get_ran(1,0,3);
			univ.party.imprisoned_monst[r1] = which_m->number;
		}
		ASB("Capture Soul: Success!");
		add_string_to_buf("  Caught in slot " + std::to_string(r1 + 1) + ".");
		play_sound(53);
	}
}

// returns 90 is no placement, OW returns # of spot
short place_monster(mon_num_t which,location where) {
	short i = 0;
	
	while((i < univ.town->max_monst()) && ((univ.town.monst[i].active != 0) ||
											(univ.town.monst[i].spec_enc_code > 0))) {
		i++;
	}
	
	if(i < univ.town->max_monst()) {
		// 10000 or more means an exported summon saved with the party
		cMonster& monst = which >= 10000 ? univ.party.summons[which - 10000] : univ.scenario.scen_monsters[which];
		univ.town.monst.assign(i, cCreature(which), monst, PSD[SDF_EASY_MODE], univ.difficulty_adjust());
		static_cast<cMonster&>(univ.town.monst[i]) = monst;
		univ.town.monst[i].attitude = monst.default_attitude;
		if(univ.town.monst[i].attitude % 2 == 0)
			univ.town.monst[i].attitude = 1;
		univ.town.monst[i].mobility = 1;
		univ.town.monst[i].active = 2;
		univ.town.monst[i].cur_loc = where;
		univ.town.monst[i].summoned = 0;
		univ.town.monst[i].target = 6;
		
		univ.town.set_crate(where.x,where.y,false);
		univ.town.set_barrel(where.x,where.y,false);
		univ.town.set_block(where.x,where.y,false);
		
		return i;
	}
	return 90;
}

// returns true if placement was successful
//which; // if in town, this is caster loc., if in combat, this is where to try
// to put monster
bool summon_monster(mon_num_t which,location where,short duration,short given_attitude) {
	location loc;
	short which_m,spot;
	
	if((is_town()) || (monsters_going)) {
		which_m = monst_there(where);
		loc = find_clear_spot(where,0);
		if(loc.x == 0)
			return false;
	}
	else {
		// pc may be summoning using item, in which case where will be pc's space, so fix
		if(pc_there(where) < 6) {
			where = find_clear_spot(where,0);
			if(where.x == 0)
				return false;
		}
		if(univ.town.is_barrel(where.x,where.y) || univ.town.is_crate(where.x,where.y) || univ.town.is_block(where.x,where.y))
			return false;
		loc = where;
	}
	
	spot = place_monster(which,loc);
	if(spot >= univ.town->max_monst()) {
		if(duration < 100)
			add_string_to_buf("  Too many monsters.");
		//ASB("  Monster fails to summon monster.");
		return false;
	}
	//play_sound(61);
	
//	if(duration < 100)
	univ.town.monst[spot].attitude = given_attitude;
//		else univ.town.monst[spot].attitude = which_att;
	
	if(which > 0) {//monster here for good
		univ.town.monst[spot].summoned = duration;
		monst_spell_note(which,21);
	}
	else univ.town.monst[spot].summoned = 0;
	
	return true;
}

void activate_monsters(short code,short /*attitude*/) {
	short i;
	
	if(code == 0)
		return;
	for(i = 0; i < univ.town->max_monst(); i++)
		if(univ.town.monst[i].spec_enc_code == code) {
			cCreature& monst = univ.town->creatures(i);
			univ.town.monst.assign(i, monst, univ.scenario.scen_monsters[monst.number], PSD[SDF_EASY_MODE], univ.difficulty_adjust());
			univ.town.monst[i].spec_enc_code = 0;
			univ.town.monst[i].active = 2;
			
			univ.town.monst[i].summoned = 0;
			univ.town.monst[i].target = 6;
			
			univ.town.set_crate(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y,false);
			univ.town.set_barrel(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y,false);
		}
}

short get_encumberance(short pc_num) {
	short store = 0,i,what_val;
	
	for(i = 0; i < 24; i++)
		if(univ.party[pc_num].equip[i]) {
			what_val = univ.party[pc_num].items[i].awkward;
			if(what_val == 1 && get_ran(1,0,130) < hit_chance[univ.party[pc_num].skills[eSkill::DEFENSE]])
				what_val--;
			if(what_val > 1 && get_ran(1,0,70) < hit_chance[univ.party[pc_num].skills[eSkill::DEFENSE]])
				what_val--;
			store += what_val;
		}
	return store;
}

mon_num_t get_summon_monster(short summon_class) {
	short i,j;
	
	for(i = 0; i < 200; i++) {
		j = get_ran(1,0,255);
		if(univ.scenario.scen_monsters[j].summon_type == summon_class) {
			return j;
		}
	}
	ASB("  Summon failed.");
	return 0;
}
