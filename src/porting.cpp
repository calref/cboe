/*
 *  porting.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "porting.hpp"

extern bool cur_scen_is_mac, mac_is_intel;

void port_town(legacy::town_record_type* dummy_town_ptr){
	if(cur_scen_is_mac != mac_is_intel)
		return;
	flip_short(&dummy_town_ptr->town_chop_time);
	flip_short(&dummy_town_ptr->town_chop_key);
	flip_short(&dummy_town_ptr->lighting);
	for(short i = 0; i < 4; i++)
		flip_short(&dummy_town_ptr->exit_specs[i]);
	flip_rect(&dummy_town_ptr->in_town_rect);
	for(short i = 0; i < 64; i++) {
		flip_short(&dummy_town_ptr->preset_items[i].item_code);
		flip_short(&dummy_town_ptr->preset_items[i].ability);
	}
	for(short i = 0; i < 50; i++) {
		flip_short(&dummy_town_ptr->preset_fields[i].field_type);
	}
	flip_short(&dummy_town_ptr->max_num_monst);
	flip_short(&dummy_town_ptr->spec_on_entry);
	flip_short(&dummy_town_ptr->spec_on_entry_if_dead);
	for(short i = 0; i < 8; i++)
		flip_short(&dummy_town_ptr->timer_spec_times[i]);
	for(short i = 0; i < 8; i++)
		flip_short(&dummy_town_ptr->timer_specs[i]);
	flip_short(&dummy_town_ptr->difficulty);
	for(short i = 0; i < 100; i++)
		flip_spec_node(&dummy_town_ptr->specials[i]);
	
}

void port_talk_nodes(legacy::talking_record_type* dummy_talk_ptr) {
	if(cur_scen_is_mac != mac_is_intel)
		return;
	for(short i = 0; i < 60; i++) {
		flip_short(&dummy_talk_ptr->talk_nodes[i].personality);
		flip_short(&dummy_talk_ptr->talk_nodes[i].type);
		flip_short(&dummy_talk_ptr->talk_nodes[i].extras[0]);
		flip_short(&dummy_talk_ptr->talk_nodes[i].extras[1]);
		flip_short(&dummy_talk_ptr->talk_nodes[i].extras[2]);
		flip_short(&dummy_talk_ptr->talk_nodes[i].extras[3]);
	}
}

void port_t_d(legacy::big_tr_type* old) {
	if(cur_scen_is_mac != mac_is_intel)
		return;
	
	for(short i = 0; i < 16; i++)
		flip_rect(&old->room_rect[i]);
	for(short i = 0; i < 60; i++) {
		flip_short(&old->creatures[i].spec1);
		flip_short(&old->creatures[i].spec2);
		flip_short(&old->creatures[i].monster_time);
		flip_short(&old->creatures[i].personality);
		flip_short(&old->creatures[i].special_on_kill);
		flip_short(&old->creatures[i].facial_pic);
	}
}

void port_ave_t(legacy::ave_tr_type* old) {
	if(cur_scen_is_mac != mac_is_intel)
		return;
	
	for(short i = 0; i < 16; i++)
		flip_rect(&old->room_rect[i]);
	for(short i = 0; i < 40; i++) {
		flip_short(&old->creatures[i].spec1);
		flip_short(&old->creatures[i].spec2);
		flip_short(&old->creatures[i].monster_time);
		flip_short(&old->creatures[i].personality);
		flip_short(&old->creatures[i].special_on_kill);
		flip_short(&old->creatures[i].facial_pic);
	}
}

void port_tiny_t(legacy::tiny_tr_type* old) {
	if(cur_scen_is_mac != mac_is_intel)
		return;
	
	for(short i = 0; i < 16; i++)
		flip_rect(&old->room_rect[i]);
	for(short i = 0; i < 30; i++) {
		flip_short(&old->creatures[i].spec1);
		flip_short(&old->creatures[i].spec2);
		flip_short(&old->creatures[i].monster_time);
		flip_short(&old->creatures[i].personality);
		flip_short(&old->creatures[i].special_on_kill);
		flip_short(&old->creatures[i].facial_pic);
	}
}

void port_scenario(legacy::scenario_data_type* temp_scenario) {
	if(cur_scen_is_mac != mac_is_intel)
		return;
	flip_short(&temp_scenario->flag_a);
	flip_short(&temp_scenario->flag_b);
	flip_short(&temp_scenario->flag_c);
	flip_short(&temp_scenario->flag_d);
	flip_short(&temp_scenario->flag_e);
	flip_short(&temp_scenario->flag_f);
	flip_short(&temp_scenario->flag_g);
	flip_short(&temp_scenario->flag_h);
	flip_short(&temp_scenario->flag_i);
	flip_short(&temp_scenario->intro_mess_pic);
	flip_short(&temp_scenario->intro_mess_len);
	flip_short(&temp_scenario->which_town_start);
	for(short i = 0; i < 200; i++)
		for(short j = 0; j < 5; j++)
			flip_short(&temp_scenario->town_data_size[i][j]);
	for(short i = 0; i < 10; i++)
		flip_short(&temp_scenario->town_to_add_to[i]);
	for(short i = 0; i < 10; i++)
		for(short j = 0; j < 2; j++)
			flip_short(&temp_scenario->flag_to_add_to_town[i][j]);
	for(short i = 0; i < 100; i++)
		for(short j = 0; j < 2; j++)
			flip_short(&temp_scenario->out_data_size[i][j]);
	for(short i = 0; i < 3; i++)
		flip_rect(&temp_scenario->store_item_rects[i]);
	for(short i = 0; i < 3; i++)
		flip_short(&temp_scenario->store_item_towns[i]);
	for(short i = 0; i < 50; i++)
		flip_short(&temp_scenario->special_items[i]);
	for(short i = 0; i < 50; i++)
		flip_short(&temp_scenario->special_item_special[i]);
	flip_short(&temp_scenario->rating);
	flip_short(&temp_scenario->uses_custom_graphics);
	for(short i = 0; i < 256; i++) {
		flip_short(&temp_scenario->scen_monsters[i].health);
		flip_short(&temp_scenario->scen_monsters[i].m_health);
		flip_short(&temp_scenario->scen_monsters[i].max_mp);
		flip_short(&temp_scenario->scen_monsters[i].mp);
		flip_short(&temp_scenario->scen_monsters[i].a[1]);
		flip_short(&temp_scenario->scen_monsters[i].a[0]);
		flip_short(&temp_scenario->scen_monsters[i].a[2]);
		flip_short(&temp_scenario->scen_monsters[i].morale);
		flip_short(&temp_scenario->scen_monsters[i].m_morale);
		flip_short(&temp_scenario->scen_monsters[i].corpse_item);
		flip_short(&temp_scenario->scen_monsters[i].corpse_item_chance);
		flip_short(&temp_scenario->scen_monsters[i].picture_num);
	}
	
	for(short i = 0; i < 256; i++) {
		flip_short(&temp_scenario->ter_types[i].picture);
	}
	for(short i = 0; i < 30; i++) {
		flip_short(&temp_scenario->scen_boats[i].which_town);
	}
	for(short i = 0; i < 30; i++) {
		flip_short(&temp_scenario->scen_horses[i].which_town);
	}
	for(short i = 0; i < 20; i++)
		flip_short(&temp_scenario->scenario_timer_times[i]);
	for(short i = 0; i < 20; i++)
		flip_short(&temp_scenario->scenario_timer_specs[i]);
	for(short i = 0; i < 256; i++) {
		flip_spec_node(&temp_scenario->scen_specials[i]);
	}
	for(short i = 0; i < 10; i++)  {
		flip_short(&temp_scenario->storage_shortcuts[i].ter_type);
		flip_short(&temp_scenario->storage_shortcuts[i].property);
		for(short j = 0; j < 10; j++)  {
			flip_short(&temp_scenario->storage_shortcuts[i].item_num[j]);
			flip_short(&temp_scenario->storage_shortcuts[i].item_odds[j]);
		}
	}
	flip_short(&temp_scenario->last_town_edited);
}


void port_item_list(legacy::scen_item_data_type* old){
	if(cur_scen_is_mac != mac_is_intel)
		return;
	
	for(short i = 0; i < 400; i++) {
		flip_short(&(old->scen_items[i].variety));
		flip_short(&(old->scen_items[i].item_level));
		flip_short(&(old->scen_items[i].value));
	}
}

void port_out(legacy::outdoor_record_type *out) {
	if(cur_scen_is_mac != mac_is_intel)
		return;
	
	for(short i = 0; i < 4; i++) {
		flip_short(&(out->wandering[i].spec_on_meet));
		flip_short(&(out->wandering[i].spec_on_win));
		flip_short(&(out->wandering[i].spec_on_flee));
		flip_short(&(out->wandering[i].cant_flee));
		flip_short(&(out->wandering[i].end_spec1));
		flip_short(&(out->wandering[i].end_spec2));
		flip_short(&(out->special_enc[i].spec_on_meet));
		flip_short(&(out->special_enc[i].spec_on_win));
		flip_short(&(out->special_enc[i].spec_on_flee));
		flip_short(&(out->special_enc[i].cant_flee));
		flip_short(&(out->special_enc[i].end_spec1));
		flip_short(&(out->special_enc[i].end_spec2));
	}
	for(short i = 0; i < 8; i++)
		flip_rect(&(out->info_rect[i]));
	for(short i = 0; i < 60; i++)
		flip_spec_node(&(out->specials[i]));
}

void port_party(legacy::party_record_type* old){
	flip_long(&old->age);
	flip_short(&old->gold);
	flip_short(&old->food);
	flip_short(&old->light_level);
	for(short i = 0; i < 30; i++){
		flip_short(&old->boats[i].which_town);
		flip_short(&old->horses[i].which_town);
		flip_short(&old->party_event_timers[i]);
		flip_short(&old->global_or_town[i]);
		flip_short(&old->node_to_call[i]);
	}
	for(short i = 0; i < 4; i++){
		flip_short(&old->creature_save[i].which_town);
		flip_short(&old->creature_save[i].hostile);
		for(short j = 0; j < 60; j++){
			flip_short(&old->creature_save[i].dudes[j].active);
			flip_short(&old->creature_save[i].dudes[j].attitude);
			flip_short(&old->creature_save[i].dudes[j].m_d.health);
			flip_short(&old->creature_save[i].dudes[j].m_d.m_health);
			flip_short(&old->creature_save[i].dudes[j].m_d.mp);
			flip_short(&old->creature_save[i].dudes[j].m_d.max_mp);
			for(short k = 0; k < 3; k++)
				flip_short(&old->creature_save[i].dudes[j].m_d.a[k]);
			flip_short(&old->creature_save[i].dudes[j].m_d.morale);
			flip_short(&old->creature_save[i].dudes[j].m_d.m_morale);
			flip_short(&old->creature_save[i].dudes[j].m_d.corpse_item);
			flip_short(&old->creature_save[i].dudes[j].m_d.corpse_item_chance);
			for(short k = 0; k < 15; k++)
				flip_short(&old->creature_save[i].dudes[j].m_d.status[k]);
			flip_short(&old->creature_save[i].dudes[j].m_d.picture_num);
			flip_short(&old->creature_save[i].dudes[j].summoned);
			flip_short(&old->creature_save[i].dudes[j].monst_start.spec1);
			flip_short(&old->creature_save[i].dudes[j].monst_start.spec2);
			flip_short(&old->creature_save[i].dudes[j].monst_start.monster_time);
			flip_short(&old->creature_save[i].dudes[j].monst_start.personality);
			flip_short(&old->creature_save[i].dudes[j].monst_start.special_on_kill);
			flip_short(&old->creature_save[i].dudes[j].monst_start.facial_pic);
		}
		flip_short(&old->imprisoned_monst[i]);
	}
	flip_short(&old->in_boat);
	flip_short(&old->in_horse);
	for(short i = 0; i < 10; i++){
		flip_short(&old->out_c[i].direction);
		flip_short(&old->out_c[i].what_monst.spec_on_meet);
		flip_short(&old->out_c[i].what_monst.spec_on_win);
		flip_short(&old->out_c[i].what_monst.spec_on_flee);
		flip_short(&old->out_c[i].what_monst.cant_flee);
		flip_short(&old->out_c[i].what_monst.end_spec1);
		flip_short(&old->out_c[i].what_monst.end_spec2);
	}
	for(short i = 0; i < 5; i++)
		for(short j = 0; j < 10; j++){
			flip_short(&old->magic_store_items[i][j].variety);
			flip_short(&old->magic_store_items[i][j].item_level);
			flip_short(&old->magic_store_items[i][j].value);
		}
	for(short i = 0; i < 50; i++)
		flip_short(&old->journal_day[i]);
	for(short i = 0; i < 140; i++){
		flip_short(&old->special_notes_str[i][0]);
		flip_short(&old->special_notes_str[i][1]);
	}
	for(short i = 0; i < 120; i++){
		flip_short(&old->talk_save[i].personality);
		flip_short(&old->talk_save[i].town_num);
		flip_short(&old->talk_save[i].str1);
		flip_short(&old->talk_save[i].str2);
	}
	flip_short(&old->direction);
	flip_short(&old->at_which_save_slot);
	for(short i = 0; i < 100; i++)
		flip_short(&old->key_times[i]);
	for(short i = 0; i < 200; i++)
		flip_short(&old->m_killed[i]);
	flip_long(&old->total_m_killed);
	flip_long(&old->total_dam_done);
	flip_long(&old->total_xp_gained);
	flip_long(&old->total_dam_taken);
}

void port_pc(legacy::pc_record_type* old){
	flip_short(&old->main_status);
	for(short i = 0; i < 30; i++)
		flip_short(&old->skills[i]);
	flip_short(&old->max_health);
	flip_short(&old->cur_health);
	flip_short(&old->max_sp);
	flip_short(&old->cur_sp);
	flip_short(&old->experience);
	flip_short(&old->skill_pts);
	flip_short(&old->level);
	for(short i = 0; i < 15; i++)
		flip_short(&old->status[i]);
	for(short i = 0; i < 10; i++){
		flip_short(&old->items[i].variety);
		flip_short(&old->items[i].item_level);
		flip_short(&old->items[i].value);
	}
	flip_short(&old->which_graphic);
	flip_short(&old->weap_poisoned);
	flip_short(&old->race);
	flip_short(&old->exp_adj);
	flip_short(&old->direction);
}

void port_c_town(legacy::current_town_type* old){
	flip_short(&old->town_num);
	flip_short(&old->difficulty);
	port_town(&old->town);
	flip_short(&old->monst.which_town);
	flip_short(&old->monst.hostile);
	for(short j = 0; j < 60; j++){
		flip_short(&old->monst.dudes[j].active);
		flip_short(&old->monst.dudes[j].attitude);
		flip_short(&old->monst.dudes[j].m_d.health);
		flip_short(&old->monst.dudes[j].m_d.m_health);
		flip_short(&old->monst.dudes[j].m_d.mp);
		flip_short(&old->monst.dudes[j].m_d.max_mp);
		for(short i = 0; i < 3; i++)
			flip_short(&old->monst.dudes[j].m_d.a[i]);
		flip_short(&old->monst.dudes[j].m_d.morale);
		flip_short(&old->monst.dudes[j].m_d.m_morale);
		flip_short(&old->monst.dudes[j].m_d.corpse_item);
		flip_short(&old->monst.dudes[j].m_d.corpse_item_chance);
		for(short i = 0; i < 15; i++)
			flip_short(&old->monst.dudes[j].m_d.status[i]);
		flip_short(&old->monst.dudes[j].m_d.picture_num);
		flip_short(&old->monst.dudes[j].summoned);
		flip_short(&old->monst.dudes[j].monst_start.spec1);
		flip_short(&old->monst.dudes[j].monst_start.spec2);
		flip_short(&old->monst.dudes[j].monst_start.monster_time);
		flip_short(&old->monst.dudes[j].monst_start.personality);
		flip_short(&old->monst.dudes[j].monst_start.special_on_kill);
		flip_short(&old->monst.dudes[j].monst_start.facial_pic);
	}
}

void flip_spec_node(legacy::special_node_type *spec) {
	flip_short(&(spec->type));
	flip_short(&(spec->sd1));
	flip_short(&(spec->sd2));
	flip_short(&(spec->pic));
	flip_short(&(spec->m1));
	flip_short(&(spec->m2));
	flip_short(&(spec->ex1a));
	flip_short(&(spec->ex1b));
	flip_short(&(spec->ex2a));
	flip_short(&(spec->ex2b));
	flip_short(&(spec->jumpto));
}

void flip_short(int16_t *s) {
	char store,*s1, *s2;
	
	s1 = (char *) s;
	s2 = s1 + 1;
	store = *s1;
	*s1 = *s2;
	*s2 = store;
	
}

void flip_long(int32_t *s){
	char store,*s1, *s2, *s3, *s4;
	
	s1 = (char *) s;
	s2 = s1 + 1;
	s3 = s1 + 2;
	s4 = s1 + 3;
	store = *s1;
	*s1 = *s4;
	*s4 = store;
	store = *s2;
	*s2 = *s3;
	*s3 = store;
	
}

// This is needed in order to load in old legacy Windows scenarios, which stored their rects in a different order than Mac scenarios
static void alter_rect(legacy::Rect *r) {
	std::swap(r->top, r->left);
	std::swap(r->bottom, r->right);
}

void flip_rect(legacy::Rect* s) {
	flip_short((int16_t *) &(s->top));
	flip_short((int16_t *) &(s->bottom));
	flip_short((int16_t *) &(s->left));
	flip_short((int16_t *) &(s->right));
	if(!cur_scen_is_mac) alter_rect(s);
}
