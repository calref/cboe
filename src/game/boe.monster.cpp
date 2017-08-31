
#include <cstdio>

#include "boe.global.hpp"

#include "universe.hpp"
#include "boe.locutils.hpp"
#include "boe.monster.hpp"
#include "boe.combat.hpp"
#include "boe.text.hpp"
#include "boe.specials.hpp"
#include "boe.items.hpp"
#include "sounds.hpp"
#include "boe.graphics.hpp"
#include "boe.newgraph.hpp"
#include "boe.main.hpp"
#include "mathutil.hpp"
#include "gfxsheets.hpp"

extern eGameMode overall_mode;
extern short which_combat_type;
extern short spell_caster, missile_firer,current_monst_tactic;
extern short hit_chance[21];

extern location center;
extern short boom_gr[8],futzing;
extern bool processing_fields,monsters_going;
extern cUniverse univ;

short out_enc_lev_tot(short which) {
	short count = 0;
	short num[7] = {22,8,4,4,3,2,1};
	
	if(univ.party.out_c[which].what_monst.cant_flee)
		return 10000;
	
	for(short i = 0; i < 7; i++)
		if(univ.party.out_c[which].what_monst.monst[i] != 0)
			count += univ.scenario.scen_monsters[univ.party.out_c[which].what_monst.monst[i]].level * num[i];
	return count;
}

void create_wand_monst() {
	short r1,r2,r3,num_tries = 0;
	location p_loc;
	
	r1 = get_ran(1,0,univ.out->wandering.size() - 1);
	if(overall_mode == MODE_OUTDOORS) {
		if(!univ.out->wandering[r1].isNull()) {
			r2 = get_ran(1,0,univ.out->wandering_locs.size() - 1);
			while(point_onscreen(univ.out->wandering_locs[r2], global_to_local(univ.party.out_loc)) && num_tries++ < 100)
				r2 = get_ran(1,0,3);
			if(!is_blocked(univ.out->wandering_locs[r2]))
				place_outd_wand_monst(univ.out->wandering_locs[r2], univ.out->wandering[r1],0);
		}
	} else if(!univ.town->wandering[r1].isNull() && univ.town.countMonsters() <= 50
			  && !univ.town->is_cleaned_out()) {
		// won't place wandering if more than 50 monsters
		r2 = get_ran(1,0,univ.town->wandering.size() - 1);
		while(point_onscreen(univ.town->wandering_locs[r2],univ.party.town_loc) &&
			  !loc_off_act_area(univ.town->wandering_locs[r2]) && num_tries++ < 100)
			r2 = get_ran(1,0,3);
		for(short i = 0; i < 4; i++) {
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
			// TODO: This contradicts the documentation which says only 1-2 are placed of the last monster
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
			if((univ.party.sd_legit(group.end_spec1,group.end_spec2)) && (PSD[group.end_spec1][group.end_spec2] > 0))
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

std::pair<short,short> get_monst_dims(mon_num_t monst) {
	cMonster& the_monst = monst >= 10000 ? univ.party.summons[monst - 10000] : univ.scenario.scen_monsters[monst];
	return std::make_pair(the_monst.x_width, the_monst.y_width);
}

// Used to set up monsters for outdoor wandering encounters.
void set_up_monst(eAttitude mode,mon_num_t m_num) {
	short which = univ.town.monst.size();
	
	cMonster& monst = m_num >= 10000 ? univ.party.summons[m_num - 10000] : univ.scenario.scen_monsters[m_num];
	univ.town.monst.assign(which, cCreature(m_num), monst, univ.party.easy_mode, univ.difficulty_adjust());
	univ.town.monst[which].active = 2;
	univ.town.monst[which].summon_time = 0;
	univ.town.monst[which].attitude = mode;
	univ.town.monst[which].mobility = 1;
}

void do_monsters() {
	short r1,target;
	location l1,l2;
	bool acted_yet = false;
	
	if(overall_mode == MODE_TOWN)
		for(short i = 0; i < univ.town.monst.size(); i++)
			if(univ.town.monst[i].active != 0 && univ.town.monst[i].status[eStatus::ASLEEP] <= 0
			   && univ.town.monst[i].status[eStatus::PARALYZED] <= 0) {
				// have to pick targets
				if(univ.town.monst[i].active == 1)
					target = 6;
				else {
					target = monst_pick_target(i); // will return 0 if target party
					target = switch_target_to_adjacent(i,target);
					if(target == 0) {
						if(dist(univ.town.monst[i].cur_loc,univ.party.town_loc) > 8)
							target = 6;
						else target = select_active_pc();
					}
					if(univ.town.monst[i].is_friendly() && target < 6)
						target = 6;
				}
				univ.town.monst[i].target = target;
//				sprintf((char *)debug,"  t: %d targets %d.",i,monst_target[i]);
//				add_string_to_buf((char *) debug);
				
				if((univ.town.monst[i].active == 2)
					|| (univ.town.monst[i].active != 0 && univ.town.monst[i].is_friendly())) {
					acted_yet = false;
					// TODO: I don't think this univ.town.hostile flag is ever actually set.
					if((univ.town.monst[i].attitude == eAttitude::DOCILE || univ.town.monst[i].target == 6) && !univ.town.monst.hostile) {
						if(univ.town.monst[i].mobility == 1) { // OK, it doesn't see the party or
							// isn't nasty, and the town isn't totally hostile.
							if(univ.town.monst[i].is_friendly() || get_ran(1,0,1) == 0) {
								acted_yet = rand_move(i);
							}
							else acted_yet = seek_party(i,univ.town.monst[i].cur_loc,univ.party.town_loc);
						}
					}
					if(univ.town.monst[i].attitude != eAttitude::DOCILE || univ.town.monst.hostile) {
						if((univ.town.monst[i].mobility == 1) && (univ.town.monst[i].target != 6)) {
							l1 = univ.town.monst[i].cur_loc;
							l2 = (univ.town.monst[i].target <= 6) ? univ.party.town_loc : univ.town.monst[target - 100].cur_loc;
							
							if(univ.town.monst[i].morale < 0 && !univ.town.monst[i].mindless
							   && univ.town.monst[i].m_type != eRace::UNDEAD && univ.town.monst[i].m_type != eRace::SKELETAL)  {
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
				if(univ.town.monst[i].active == 1 && !univ.town.monst[i].is_friendly()
					&& (dist(univ.town.monst[i].cur_loc,univ.party.town_loc) <= 8)) {
					r1 = get_ran(1,1,100);
					r1 += (univ.party.status[ePartyStatus::STEALTH] > 0) ? 46 : 0;
					r1 += can_see_light(univ.town.monst[i].cur_loc,univ.party.town_loc,sight_obscurity) * 10;
					if(r1 < 50) {
						univ.town.monst[i].active = 2;
						add_string_to_buf("Monster saw you!");
						// play go active sound
						if(isHumanoid(univ.town.monst[i].m_type) || univ.town.monst[i].m_type == eRace::GIANT)
							play_sound(18);
						else play_sound(46);
					}
					for(short j = 0; j < univ.town.monst.size(); j++)
						if((univ.town.monst[j].active == 2)
							&& ((dist(univ.town.monst[i].cur_loc,univ.town.monst[j].cur_loc) <= 5)))
							univ.town.monst[i].active = 2;
				}
				
			}
	if(overall_mode == MODE_OUTDOORS) {
		for(short i = 0; i < 10; i++)
			if(univ.party.out_c[i].exists) {
				acted_yet = false;
				l1 = univ.party.out_c[i].m_loc;
				l2 = univ.party.out_loc;
				
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
		else if(univ.town.monst[which_m].resist[eDamageType::COLD] == 0) hate_spot = false;
	}
	// Hate fire wall?
	else if(univ.town.is_fire_wall(loc.x,loc.y)) {
		hate_spot = true;
		if(have_radiate && which_radiate == eFieldType::WALL_FIRE) hate_spot = false;
		else if(univ.town.monst[which_m].resist[eDamageType::FIRE] == 0) hate_spot = false;
	}
	// Note: Monsters used to enter shock walls even if they were merely resistant to magic
	// Hate shock wall?
	else if(univ.town.is_force_wall(loc.x,loc.y)) {
		hate_spot = true;
		if(have_radiate && which_radiate == eFieldType::WALL_FORCE) hate_spot = false;
		else if(univ.town.monst[which_m].resist[eDamageType::MAGIC] == 0) hate_spot = false;
	}
	// Hate stink cloud?
	else if(univ.town.is_scloud(loc.x,loc.y)) {
		hate_spot = true;
		if(have_radiate && which_radiate == eFieldType::CLOUD_STINK) hate_spot = false;
		else if(univ.town.monst[which_m].resist[eDamageType::MAGIC] <= 50) hate_spot = false;
	}
	// Hate sleep cloud?
	else if(univ.town.is_sleep_cloud(loc.x,loc.y)) {
		hate_spot = true;
		if(have_radiate && which_radiate == eFieldType::CLOUD_SLEEP) hate_spot = false;
		else if(univ.town.monst[which_m].resist[eDamageType::MAGIC] <= 50) hate_spot = false;
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
		if((!cur_monst->is_friendly() &&
			(univ.town.monst[univ.town.monst[which_m].target - 100].attitude == cur_monst->attitude)) ||
			(cur_monst->is_friendly() && univ.town.monst[univ.town.monst[which_m].target - 100].is_friendly()))
			univ.town.monst[which_m].target = 6;
		else if(univ.town.monst[univ.town.monst[which_m].target - 100].active == 0)
			univ.town.monst[which_m].target = 6;
	}
	if(univ.town.monst[which_m].target < 6)
		if(univ.party[univ.town.monst[which_m].target].main_status != eMainStatus::ALIVE || get_ran(1,0,3) == 1)
			univ.town.monst[which_m].target = 6;
	
	if(is_combat() && !cur_monst->is_friendly()) {
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
		if(cur_monst->is_friendly()) {
			return targ_m;
		}
		if(targ_m == 6 && !cur_monst->is_friendly())
			return 0;
		if(dist(cur_monst->cur_loc,univ.town.monst[targ_m - 100].cur_loc) <
			dist(cur_monst->cur_loc,univ.party.town_loc))
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
	short min_dist = 1000,cur_targ = 6;
	
	for(short i = 0; i < univ.town.monst.size(); i++) {
		if(univ.town.monst[i].active > 0 && !which_m->is_friendly(univ.town.monst[i]) && // allve + they hate each other
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
	
	if(which_m->is_friendly())
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
	short how_close = 200,store = 6;
	
	for(short i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE && dist(where,univ.party[i].combat_pos) < how_close) {
			store = i;
			how_close = dist(where,univ.party[i].combat_pos);
		}
	return store;
}

//mode;  // 1 - closest hostile to PCs  2 - closest friendly to PCs
short closest_monst(location where,bool friendly) {
	short how_close = 200,store = 6;
	
	for(short i = 0; i < univ.town.monst.size(); i++)
		if(univ.town.monst[i].is_friendly() == friendly
			&& (dist(where,univ.town.monst[i].cur_loc) < how_close)) {
			store = i;
			how_close = dist(where,univ.town.monst[i].cur_loc);
		}
	return store;
}

short switch_target_to_adjacent(short which_m,short orig_target) {
	location monst_loc;
	short num_adj = 0;
	
	monst_loc = univ.town.monst[which_m].cur_loc;
	
	// First, take care of friendly monsters.
	if(univ.town.monst[which_m].is_friendly()) {
		if(orig_target >= 100)
			if((univ.town.monst[orig_target - 100].active > 0) &&
				(monst_adjacent(univ.town.monst[orig_target - 100].cur_loc,which_m)))
				return orig_target;
		for(short i = 0; i < univ.town.monst.size(); i++)
			if((univ.town.monst[i].active > 0) &&
				!univ.town.monst[i].is_friendly() &&
				(monst_adjacent(univ.town.monst[i].cur_loc,which_m)))
				return i + 100;
		return orig_target;
	}
	
	// If we get here while in town, just need to check if switch to pc
	if((is_town()) && (monst_adjacent(univ.party.town_loc,which_m)))
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
		for(short i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE && monst_adjacent(univ.party[i].combat_pos,which_m) &&
			   (get_encumbrance(i) < 2))
		 		return i;
	
	// Check for a nice, adjacent, friendly monster and maybe attack
	for(short i = 0; i < univ.town.monst.size(); i++)
		if((univ.town.monst[i].active > 0) &&
			univ.town.monst[i].is_friendly() &&
			(monst_adjacent(univ.town.monst[i].cur_loc,which_m)) &&
			(get_ran(1,0,2) < 2))
			return i + 100;
	
	// OK. Now if this monster has PCs adjacent, pick one at randomn and hack. Otherwise,
	// stick with orig. target.
	for(short i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE && monst_adjacent(univ.party[i].combat_pos,which_m))
			num_adj++;
	
	if(num_adj == 0)
		return orig_target;
	
	short i = 0;
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
		for(short j = 0; j < 3; j++) {
			store_loc = univ.town.monst[i].cur_loc;
			store_loc.x += get_ran(1,0,24) - 12;
			store_loc.y += get_ran(1,0,24) - 12;
			if(!loc_off_act_area(store_loc) && (can_see_light(univ.town.monst[i].cur_loc,store_loc,sight_obscurity) < 5)) {
				univ.town.monst[i].targ_loc = store_loc; j = 3;
			}
		}
		
		if(univ.town.monst[i].targ_loc.x == 0) {
			// maybe pick a wand loc, else juist pick a loc
			short j = get_ran(1,0,univ.town->wandering_locs.size() - 1);
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
// TODO: NO WAIT IT DOESN'T LOOK AT ALL SPACES!!!
// TODO: THIS MAKES NO ADJUSTMENTS FOR BIG MONSTERS!!!
//mode; // 0 - normal  1 - prefer adjacent space
location find_clear_spot(location from_where,short mode) {
	location loc,store_loc;
	short num_tries = 0,r1;
	// Here 254 indicates the low byte of the town fields, minus explored spaces (which is lowest bit).
	unsigned long blocking_fields = SPECIAL_SPOT | OBJECT_CRATE | OBJECT_BARREL | OBJECT_BLOCK | FIELD_QUICKFIRE | 254;
	
	while(num_tries < 75) {
		num_tries++;
		loc = from_where;
		r1 = get_ran(1,-2,2);
		loc.x = loc.x + r1;
		r1 = get_ran(1,-2,2);
		loc.y = loc.y + r1;
		if(!loc_off_act_area(loc) && !is_blocked(loc)
			&& can_see_light(from_where,loc,combat_obscurity) == 0
			&& (!is_combat() || univ.target_there(loc,TARG_PC) == nullptr)
			&& (!(is_town()) || (loc != univ.party.town_loc))
			&& (!(univ.town.fields[loc.x][loc.y] & blocking_fields))) {
			if((mode == 0) || ((mode == 1) && (adjacent(from_where,loc))))
				return loc;
			else store_loc = loc;
		}
	}
	return store_loc;
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
		(dest != univ.party.out_loc) &&
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
	if(univ.town.monst[m_num].attitude == eAttitude::DOCILE ||
	   (univ.town.monst[m_num].attitude == eAttitude::FRIENDLY && univ.party.hostiles_present == 0)) {
		return true;
	} else {
		return false;
	}
}

// This damages a monster by any fields it's in, and destroys any barrels or crates
// it's stiing on.
void monst_inflict_fields(short which_monst) {
	short r1;
	location where_check;
	cCreature *which_m;
	
	if(univ.town.monst[which_monst].active == 0)
		return;
	
	which_m = &univ.town.monst[which_monst];
	bool have_radiate = which_m->abil[eMonstAbil::RADIATE].active;
	eFieldType which_radiate = which_m->abil[eMonstAbil::RADIATE].radiate.type;
	for(short i = 0; i < univ.town.monst[which_monst].x_width; i++)
		for(short j = 0; j < univ.town.monst[which_monst].y_width; j++)
			if(univ.town.monst[which_monst].active > 0) {
				where_check.x = univ.town.monst[which_monst].cur_loc.x + i;
				where_check.y = univ.town.monst[which_monst].cur_loc.y + j;
				// TODO: If the goal is to damage the monster by any fields it's on, why all the break statements?
				if(univ.town.is_quickfire(where_check.x,where_check.y)) {
					r1 = get_ran(2,1,8);
					damage_monst(*which_m,7,r1,eDamageType::FIRE,0);
					break;
				}
				if(univ.town.is_blade_wall(where_check.x,where_check.y)) {
					r1 = get_ran(6,1,8);
					if(have_radiate && which_radiate != eFieldType::WALL_BLADES)
						damage_monst(*which_m,7,r1,eDamageType::WEAPON,0);
					break;
				}
				if(univ.town.is_force_wall(where_check.x,where_check.y)) {
					r1 = get_ran(3,1,6);
					if(have_radiate && which_radiate != eFieldType::WALL_FORCE)
						damage_monst(*which_m,7,r1,eDamageType::MAGIC,0);
					break;
				}
				if(univ.town.is_sleep_cloud(where_check.x,where_check.y)) {
					if(have_radiate && which_radiate != eFieldType::CLOUD_SLEEP)
						which_m->sleep(eStatus::ASLEEP,3,0);
					break;
				}
				if(univ.town.is_ice_wall(where_check.x,where_check.y)) {
					r1 = get_ran(3,1,6);
					if(have_radiate && which_radiate != eFieldType::WALL_ICE)
						damage_monst(*which_m,7,r1,eDamageType::COLD,0);
					break;
				}
				if(univ.town.is_scloud(where_check.x,where_check.y)) {
					r1 = get_ran(1,2,3);
					if(have_radiate && which_radiate != eFieldType::CLOUD_STINK)
						which_m->curse(r1);
					break;
				}
				if(univ.town.is_web(where_check.x,where_check.y) && which_m->m_type != eRace::BUG) {
					which_m->spell_note(19);
					r1 = get_ran(1,2,3);
					which_m->web(r1);
					univ.town.set_web(where_check.x,where_check.y,false);
					break;
				}
				if(univ.town.is_fire_wall(where_check.x,where_check.y)) {
					r1 = get_ran(2,1,6);
					if(have_radiate && which_radiate != eFieldType::WALL_FIRE)
						damage_monst(*which_m,7,r1,eDamageType::FIRE,0);
					break;
				}
				if(univ.town.is_force_cage(where_check.x,where_check.y))
					process_force_cage(where_check, univ.get_target_i(*which_m));
			}
	if(univ.town.monst[which_monst].active > 0)
		for(short i = 0; i < univ.town.monst[which_monst].x_width; i++)
			for(short j = 0; j < univ.town.monst[which_monst].y_width; j++) {
				where_check.x = univ.town.monst[which_monst].cur_loc.x + i;
				where_check.y = univ.town.monst[which_monst].cur_loc.y + j;
				if((univ.town.is_crate(where_check.x,where_check.y)) ||
					(univ.town.is_barrel(where_check.x,where_check.y)) )
					for(short k = 0; k < univ.town.items.size(); k++)
						if(univ.town.items[k].variety != eItemType::NO_ITEM && univ.town.items[k].contained
						   && (univ.town.items[k].item_loc == where_check))
							univ.town.items[k].contained = univ.town.items[k].held = false;
				univ.town.set_crate(where_check.x,where_check.y,false);
				univ.town.set_barrel(where_check.x,where_check.y,false);
				if(univ.town.is_fire_barr(where_check.x,where_check.y)) {
					r1 = get_ran(2,1,10);
					damage_monst(*which_m,7,r1,eDamageType::FIRE,0);
				}
			}
	
	
	
}

//mode; // 1 - town 2 - combat
bool monst_check_special_terrain(location where_check,short mode,short which_monst) {
	ter_num_t ter = 0;
	short r1,guts = 0;
	bool can_enter = true,mage = false;
	location from_loc,to_loc;
	bool do_look = false; // If becomes true, terrain changed, so need to update what party sees
	cCreature *which_m;
	eTerSpec ter_abil;
	unsigned short ter_dir;
	
	from_loc = univ.town.monst[which_monst].cur_loc;
	ter = univ.town->terrain(where_check.x,where_check.y);
	////
	which_m = &univ.town.monst[which_monst];
	ter_abil = univ.scenario.ter_types[ter].special;
	ter_dir = univ.scenario.ter_types[ter].flag3;
	
	if(mode > 0 && ter_abil == eTerSpec::CONVEYOR) {
		if(
			((ter_dir == DIR_N) && (where_check.y > from_loc.y)) ||
			((ter_dir == DIR_E) && (where_check.x < from_loc.x)) ||
			((ter_dir == DIR_S) && (where_check.y < from_loc.y)) ||
			((ter_dir == DIR_W) && (where_check.x > from_loc.x)) ) {
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
	if(which_m->attitude == eAttitude::DOCILE)
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
		if(!which_m->is_friendly() && get_ran(1,1,100) < which_m->mu * 10 + which_m->cl * 4) {
			// TODO: Are these barrier sounds right?
			play_sound(60);
			which_m->spell_note(49);
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
		if(!which_m->is_friendly() && get_ran(1,1,100) < which_m->mu * 10 + which_m->cl * 4
			&& (!univ.town->strong_barriers)) {
			play_sound(60);
			which_m->spell_note(49);
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
			for(short i = 0; i < univ.town.items.size(); i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where_check
				   && univ.town.items[i].contained && univ.town.items[i].held)
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
			for(short i = 0; i < univ.town.items.size(); i++)
				if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].item_loc == where_check
				   && univ.town.items[i].contained && univ.town.items[i].held)
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
		case eTerSpec::NONE: break;
			// changing ter
		case eTerSpec::CHANGE_WHEN_STEP_ON:
			can_enter = false;
			if(!(monster_placid(which_monst))) {
				univ.town->terrain(where_check.x,where_check.y) = univ.scenario.ter_types[ter].flag1;
				do_look = true;
				if(point_onscreen(center,where_check))
					play_sound(univ.scenario.ter_types[ter].flag2);
			}
			break;
			
		case eTerSpec::BLOCKED_TO_MONSTERS:
		case eTerSpec::TOWN_ENTRANCE:
		case eTerSpec::WATERFALL_CAVE:
		case eTerSpec::WATERFALL_SURFACE:
			can_enter = false;
			break;
			
		case eTerSpec::DAMAGING:
			if(univ.town.monst[which_monst].resist[eDamageType(univ.scenario.ter_types[ter].flag3)] == 0)
				return true;
			else return univ.town.monst[which_monst].invuln;
			// TODO: Should it check any other terrain specials?
		case eTerSpec::BED: case eTerSpec::BRIDGE: case eTerSpec::CALL_SPECIAL_WHEN_USED: case eTerSpec::CHANGE_WHEN_USED:
		case eTerSpec::CONVEYOR: case eTerSpec::CRUMBLING: case eTerSpec::IS_A_CONTAINER: case eTerSpec::IS_A_SIGN:
		case eTerSpec::LOCKABLE: case eTerSpec::UNLOCKABLE:
		case eTerSpec::UNUSED1: case eTerSpec::UNUSED2: case eTerSpec::UNUSED3:
		case eTerSpec::WILDERNESS_CAVE: case eTerSpec::WILDERNESS_SURFACE:
		case eTerSpec::CALL_SPECIAL: case eTerSpec::DANGEROUS: // Maybe these two should do something?
			break;
	}
	
	// Action may change terrain, so update what's been seen
	if(do_look) {
		if(is_town())
			update_explored(univ.party.town_loc);
		if(is_combat())
			for(short i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE)
					update_explored(univ.party[i].combat_pos);
	}
	
	return can_enter;
}

void record_monst(cCreature* which_m, bool forced) {
	short r1;
	
	r1 = get_ran(1,1,100);
	r1 = (r1 * 7) / 10;
	
	if(forced) r1 = 0;
	
	if((which_m->x_width > 1) || (which_m->y_width > 1)) {
		ASB("Capture Soul: Monster is too big.");
	}
	// TODO: Are these two sounds right?
	else if(r1 > cCreature::charm_odds[which_m->level / 2] || which_m->abil[eMonstAbil::SPLITS].active
			 || which_m->m_type == eRace::IMPORTANT) {
		which_m->spell_note(10);
		play_sound(68);
	}
	else {
		which_m->spell_note(24);
		r1 = get_ran(1,0,univ.party.imprisoned_monst.size() - 1);
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

short place_monster(mon_num_t which,location where,bool forced) {
	// TODO: Currently "forced" is only used by special nodes. Should it be used in other places?
	if(!forced && univ.target_there(where, TARG_MONST) != nullptr)
		return univ.town.monst.size();
	short i = 0;
	
	while(i < univ.town.monst.size() && (univ.town.monst[i].active != 0 || univ.town.monst[i].spec_enc_code > 0)) {
		i++;
	}
	
	// 10000 or more means an exported summon saved with the party
	cMonster& monst = which >= 10000 ? univ.party.summons[which - 10000] : univ.scenario.scen_monsters[which];
	univ.town.monst.assign(i, cCreature(which), monst, univ.party.easy_mode, univ.difficulty_adjust());
	// TODO: Should this static_cast assignment be happening?
	// One effect is resetting max health to ignore difficulty_adjust()
	static_cast<cMonster&>(univ.town.monst[i]) = monst;
	univ.town.monst[i].attitude = monst.default_attitude;
	// TODO: Huh? Why does it disallow friendly monster placements?
	if(univ.town.monst[i].is_friendly())
		univ.town.monst[i].attitude = eAttitude::HOSTILE_A;
	univ.town.monst[i].mobility = 1;
	univ.town.monst[i].active = 2;
	univ.town.monst[i].cur_loc = where;
	univ.town.monst[i].summon_time = 0;
	univ.town.monst[i].target = 6;
	
	univ.town.set_crate(where.x,where.y,false);
	univ.town.set_barrel(where.x,where.y,false);
	univ.town.set_block(where.x,where.y,false);
	
	return i;
}

// returns true if placement was successful
//which; // if in town, this is caster loc., if in combat, this is where to try
// to put monster
bool summon_monster(mon_num_t which,location where,short duration,eAttitude given_attitude,bool by_party) {
	location loc;
	short spot;
	
	if(which <= 0) return false;
	
	if((is_town()) || (monsters_going)) {
		loc = find_clear_spot(where,0);
		if(loc.x == 0)
			return false;
	}
	else {
		// pc may be summoning using item, in which case where will be pc's space, so fix
		if(univ.target_there(where, TARG_PC)) {
			where = find_clear_spot(where,0);
			if(where.x == 0)
				return false;
		}
		if(univ.town.is_barrel(where.x,where.y) || univ.town.is_crate(where.x,where.y) || univ.town.is_block(where.x,where.y))
			return false;
		loc = where;
	}
	
	spot = place_monster(which,loc);
	if(spot >= univ.town.monst.size()) {
		if(duration < 100)
			add_string_to_buf("  Too many monsters.");
		return false;
	}
	//play_sound(61);
	
	univ.town.monst[spot].attitude = given_attitude;
	
	univ.town.monst[spot].summon_time = duration;
	univ.town.monst[spot].party_summoned = by_party;
	univ.town.monst[spot].spell_note(21);
	
	return true;
}

void activate_monsters(short code,short /*attitude*/) {
	if(code == 0)
		return;
	for(short i = 0; i < univ.town->creatures.size(); i++)
		if(univ.town->creatures[i].spec_enc_code == code) {
			cTownperson& monst = univ.town->creatures[i];
			univ.town.monst.assign(i, monst, univ.scenario.scen_monsters[monst.number], univ.party.easy_mode, univ.difficulty_adjust());
			univ.town.monst[i].spec_enc_code = 0;
			univ.town.monst[i].active = 2;
			
			univ.town.monst[i].summon_time = 0;
			univ.town.monst[i].target = 6;
			
			univ.town.set_crate(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y,false);
			univ.town.set_barrel(univ.town.monst[i].cur_loc.x,univ.town.monst[i].cur_loc.y,false);
		}
}

short get_encumbrance(short pc_num) {
	short store = 0,what_val;
	
	what_val = univ.party[pc_num].free_weight();
	if(what_val < 0) store += what_val / -10;
	
	for(short i = 0; i < univ.party[pc_num].items.size(); i++)
		if(univ.party[pc_num].equip[i]) {
			what_val = univ.party[pc_num].items[i].awkward;
			if(univ.party[pc_num].items[i].ability == eItemAbil::ENCUMBERING)
				what_val += univ.party[pc_num].items[i].abil_data[0];
			if(what_val == 1 && get_ran(1,0,130) < hit_chance[univ.party[pc_num].skill(eSkill::DEFENSE)])
				what_val--;
			if(what_val > 1 && get_ran(1,0,70) < hit_chance[univ.party[pc_num].skill(eSkill::DEFENSE)])
				what_val--;
			store += what_val;
		}
	return store;
}

mon_num_t get_summon_monster(short summon_class) {
	for(short i = 0; i < 200; i++) {
		short j = get_ran(1,0,255);
		if(univ.scenario.scen_monsters[j].summon_type == summon_class) {
			return j;
		}
	}
	ASB("  Summon failed.");
	return 0;
}
