/*
 *  porting.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#include "porting.hpp"

#include "dialogxml/widgets/pict.hpp"

namespace porting {
// check endian with cur_file_is_mac and is_computer_small_endian
// check cur_file_is_mac to see if we need to convert RECT in Rect
static void port_rect(legacy::Rect* s);

bool is_computer_small_endian=true;
void check_endian() {
	union {uint16_t x; uint8_t c;} endian;
	endian.x = 1;
	is_computer_small_endian = endian.c;
}
bool is_small_endian()
{
	return is_computer_small_endian;
}

bool cur_file_is_mac=true;
void set_current_file_type(bool isMac)
{
	cur_file_is_mac = isMac;
}

void port_town(legacy::town_record_type* dummy_town_ptr){
	port_rect(&dummy_town_ptr->in_town_rect);
	if(cur_file_is_mac != is_computer_small_endian)
		return;
	flip_short(&dummy_town_ptr->town_chop_time);
	flip_short(&dummy_town_ptr->town_chop_key);
	flip_short(&dummy_town_ptr->lighting);
	for(short i = 0; i < 4; i++)
		flip_short(&dummy_town_ptr->exit_specs[i]);
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
	if(cur_file_is_mac != is_computer_small_endian)
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
	for(short i = 0; i < 16; i++)
		port_rect(&old->room_rect[i]);

	if(cur_file_is_mac != is_computer_small_endian)
		return;
	
	for(short i = 0; i < 60; i++) {
		flip_short(&old->creatures[i].spec1);
		flip_short(&old->creatures[i].spec2);
		flip_short(&old->creatures[i].monster_time);
		flip_short(&old->creatures[i].personality);
		flip_short(&old->creatures[i].special_on_kill);
		flip_short(&old->creatures[i].facial_pic);
	}
}

void port_t_i(legacy::town_item_list* old)
{
	if(cur_file_is_mac != is_computer_small_endian)
		return;
	for(short i = 0; i < 115; i++)
		port_item_record(&(old->items[i]));
}

void port_stored_items_list(legacy::stored_items_list_type *old)
{
	if(cur_file_is_mac != is_computer_small_endian)
		return;
	for(short i = 0; i < 115; i++)
		port_item_record(&(old->items[i]));
}

void port_ave_t(legacy::ave_tr_type* old) {
	for(short i = 0; i < 16; i++)
		port_rect(&old->room_rect[i]);
	
	if(cur_file_is_mac != is_computer_small_endian)
		return;
	
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
	for(short i = 0; i < 16; i++)
		port_rect(&old->room_rect[i]);
	
	if(cur_file_is_mac != is_computer_small_endian)
		return;
	
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
	for(short i = 0; i < 3; i++)
		port_rect(&temp_scenario->store_item_rects[i]);

	if(cur_file_is_mac != is_computer_small_endian)
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
		flip_short(&temp_scenario->store_item_towns[i]);
	for(auto &item : temp_scenario->special_items)
		flip_short(&item);
	for(auto &item : temp_scenario->special_item_special)
		flip_short(&item);
	flip_short(&temp_scenario->rating);
	flip_short(&temp_scenario->uses_custom_graphics);
	for(auto &monster : temp_scenario->scen_monsters) {
		flip_short(&monster.health);
		flip_short(&monster.m_health);
		flip_short(&monster.max_mp);
		flip_short(&monster.mp);
		flip_short(&monster.a[0]);
		flip_short(&monster.a[1]);
		flip_short(&monster.a[2]);
		flip_short(&monster.morale);
		flip_short(&monster.m_morale);
		flip_short(&monster.corpse_item);
		flip_short(&monster.corpse_item_chance);
		flip_short(&monster.picture_num);
	}
	
	for (auto &terrain : temp_scenario->ter_types)
		flip_short(&terrain.picture);
	for(auto &boat : temp_scenario->scen_boats)
		flip_short(&boat.which_town);
	for(auto &horse : temp_scenario->scen_horses)
		flip_short(&horse.which_town);
	for(auto &timer : temp_scenario->scenario_timer_times)
		flip_short(&timer);
	for(auto &timer : temp_scenario->scenario_timer_specs)
		flip_short(&timer);
	for(auto &special : temp_scenario->scen_specials)
		flip_spec_node(&special);
	for(auto &shortcut : temp_scenario->storage_shortcuts) {
		flip_short(&shortcut.ter_type);
		flip_short(&shortcut.property);
		for(short j = 0; j < 10; j++)  {
			flip_short(&shortcut.item_num[j]);
			flip_short(&shortcut.item_odds[j]);
		}
	}
	flip_short(&temp_scenario->last_town_edited);
}

void port_item_record(legacy::item_record_type *old) {
	if(cur_file_is_mac != is_computer_small_endian)
		return;

	flip_short(&(old->variety));
	flip_short(&(old->item_level));
	flip_short(&(old->value));
}

void port_item_list(legacy::scen_item_data_type* old){
	if(cur_file_is_mac != is_computer_small_endian)
		return;
	
	for(auto &item : old->scen_items)
		port_item_record(&item);
}

void port_out(legacy::outdoor_record_type *out) {
	for(short i = 0; i < 8; i++)
		port_rect(&(out->info_rect[i]));

	if(cur_file_is_mac != is_computer_small_endian)
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
	for(short i = 0; i < 60; i++)
		flip_spec_node(&(out->specials[i]));
}

void port_party(legacy::party_record_type* old){
	if(cur_file_is_mac != is_computer_small_endian)
		return;

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
	if(cur_file_is_mac != is_computer_small_endian)
		return;

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
	port_town(&old->town);
	if(cur_file_is_mac != is_computer_small_endian)
		return;
	
	flip_short(&old->town_num);
	flip_short(&old->difficulty);
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
}

void port_rect(legacy::Rect* s) {
	if(cur_file_is_mac == is_computer_small_endian) {
		flip_short((int16_t *) &(s->top));
		flip_short((int16_t *) &(s->bottom));
		flip_short((int16_t *) &(s->left));
		flip_short((int16_t *) &(s->right));
	}
	if(!cur_file_is_mac) alter_rect(s);
}

cPictNum port_graphic_num(int pic) {
	// from dialog-converting.txt
	/*
	-1			solid black
	0 + x		number of terrain graphic
	300 + x		animated terrain graphic (grabs the first frame only)
	400 + x		monster graphic num
	700 + x		dlog graphic (large dlog graphics were done by using four of these arranged in the correct way)
	800 + x		pc graphic
	900 + x		B&W graphic - the PICT resource for this does not exist
	950			null item
	1000 + x	Talking face
	1100		item info help
	1200		pc screen help
	1300		combat ap
	1400-1402	button help
	1410-1412	large scen graphics
	1500		stat symbols help
	1600 + x	scen graphics
	1700 + x	anim graphic -- drawn from fields_gworld, so a boom or barrier icon?
	1800 + x	items
	2000 + x	custom graphics up to 2399
	2400 + x	custom graphics up to 2799, BUT it's a split graphic ...
	   it looks at the size of rect, and places a 32 x 32 or 36 x 36 graphic drawn
	   from the custom gworld, depending on the size of rect. half of graphic is
	   drawn from one custom slot, and half is drawn from next one.
	+3000		suppress drawing a frame around the graphic
	 */
	if (pic<0)
		return cPictNum(pic, PIC_NONE);
	if (pic>3000) pic-=3000; // suppress drawing a frame around the graphic
	if (pic<300) {
		switch(pic) {
			case 247: pic = 210; break;
			case 248: pic = 211; break;
			case 249: pic = 212; break;
			case 250: pic = 213; break;
			case 202: pic = 0; break;
			case 203: pic = 2; break;
			case 204: pic = 32; break;
			case 207: pic = 0; break;
			case 208: pic = 123; break;
			case 209: pic = 210; break;
			case 210: pic = 163; break;
			case 211: pic = 2; break;
			case 212: pic = 32; break;
			case 218: case 219: case 220: case 221:
			case 222: case 223: case 224: case 225:
			case 215: pic = 216; break;
			case 233: pic = 137; break;
			case 213: pic = 214; break;
			case 214: pic = 215; break;
			case 246: pic = 209; break;
			case 251: pic = 207; break;
			case 252: pic = 208; break;
		}
		return cPictNum(pic, PIC_TER);
	}
	if (pic<400)
		return cPictNum(pic-300, PIC_TER_ANIM);
	if (pic<700)
		return cPictNum(pic-400,PIC_MONST);
	if (pic<800)
		return cPictNum(pic-700, PIC_DLOG);
	if (pic<900)
		return cPictNum(pic-800, PIC_PC);
	if (pic<1000)// ARGH: normally bwpats, force an error picture
		return cPictNum(800, PIC_TER);
	if (pic<1100) // can we really get some talk here?
		return cPictNum(pic-1000, PIC_TALK);
	if (pic<1200) // inventory help
		return cPictNum(1100, PIC_FULL);
	if (pic<1300) // stats help
		return cPictNum(1200, PIC_FULL);
	if (pic<1400) // stats help
		return cPictNum(1300, PIC_FULL);
	if (pic<1403) // button help
		return cPictNum(pic, PIC_FULL);
	// TOADD: 1410-1499: load_pict(910-999)
	// TOADD: 15XX: stat symbol help, RSRC PICT 1400

	if (pic<1600) // ARGH: unsure force an error picture
		return cPictNum(800, PIC_TER);
	if (pic<1700)
		return cPictNum(pic-1600, PIC_SCEN);  // checkme
	if (pic<1800) // ARGH: unsure force an error picture
		return cPictNum(800, PIC_TER);
	if (pic<2000)
		return cPictNum(pic-1800, PIC_ITEM);  // checkme
	if (pic<2400)
		return cPictNum(pic-2000, PIC_CUSTOM_TER);
	if (pic<2800)
		return cPictNum(pic-2400, PIC_CUSTOM_DLOG); // or pict custom monster large (in fact this one seems to depend on the dialog final size)
	// ARGH: not expected, force an error picture
	return cPictNum(800, PIC_TER);
}
	
}
