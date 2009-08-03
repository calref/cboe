#ifndef _PARTY_H
	#define _PARTY_H

void init_party(short mode);
void init_party_scen_data();
void make_boats() ;
pc_record_type return_dummy_pc();
pc_record_type create_prefab_pc(short num);
Boolean create_pc(short spot,short parent_num);
void increase_light(short amt);
void do_xp_keep(short pc_num,short mode);
Boolean spend_xp_event_filter (short item_hit);
void do_xp_draw();
void draw_xp_skills();
Boolean spend_xp(short pc_num, short mode, short parent);
Boolean poison_weapon( short pc_num, short how_much,short safe);
Boolean is_weapon(short pc_num,short item);
void cast_spell(short type,short situation);
Boolean repeat_cast_ok(short type);
void give_party_spell(short which);
void do_mage_spell(short pc_num,short spell_num);
void do_priest_spell(short pc_num,short spell_num);
void cast_town_spell(location where);
void check_spell_on_space(location where, unsigned char spell);
void do_mindduel(short pc_num,creature_data_type *monst);
void dispel_fields(short i,short j,short mode);
Boolean pc_can_cast_spell(short pc_num,short type,short spell_num);
void pick_spell_event_filter (short item_hit);
short pick_spell(short pc_num,short type,short situation) ;
void set_town_spell(short s_num,short who_c);
void alch_choice_event_filter (short item_hit);
void do_alchemy();
short alch_choice(short pc_num);
void pc_graphic_event_filter (short item_hit);
Boolean pick_pc_graphic(short pc_num,short mode,short parent_num);
void pc_name_event_filter ();
Boolean pick_pc_name(short pc_num,short parent_num)  ;
void pick_trapped_monst_event_filter (short item_hit);
unsigned char pick_trapped_monst()  ;
void affect_pc(short which_pc,short type,short how_much);
void void_sanctuary(short pc_num);
void set_pc_moves();
void take_ap(short num);
short cave_lore_present();
void adjust_spell_menus();
void print_spell_cast(short spell_num,short which);
void update_gold_skills();
void put_party_in_scen();

#endif
