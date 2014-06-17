#ifndef _MONSTER_H
	#define _MONSTER_H

monster_record_type return_monster_template(unsigned char store);
short difficulty_adjust();
short out_enc_lev_tot(short which);
void create_wand_monst();
void place_outd_wand_monst(location where,out_wandering_type group,short forced);
short get_monst_picnum(unsigned char monst);
void get_monst_dims(unsigned char monst,short *width, short *height);
void set_up_monst(short mode,unsigned char m_num);
void do_monsters();
Boolean monst_hate_spot(short which_m,location *good_loc);
short monst_pick_target(short which_m);
short monst_pick_target_monst(creature_data_type *which_m);
short monst_pick_target_pc(short m_num,creature_data_type *which_m);
short select_active_pc();
short closest_pc(location where);
short switch_target_to_adjacent(short which_m,short orig_target);
Boolean rand_move(int i);
Boolean seek_party(short i,location l1,location l2);
Boolean flee_party(short i,location l1,location l2);
Boolean try_move(short i,location start,short x,short y);
Boolean combat_move_monster(short which,location destination);
location find_clear_spot(location from_where,short mode);
short pc_there(location where);
Boolean outdoor_move_monster(short num,location dest);
Boolean town_move_monster(short num,location dest);
Boolean monster_placid(short m_num);
void monst_inflict_fields(short which_monst);
Boolean monst_check_special_terrain(location where_check,short mode,short which_monst);
void forced_place_monster(unsigned char which,location where);
Boolean summon_monster(unsigned char which,location where,short duration,short given_attitude);
short get_encumberance(short pc_num);
unsigned short get_summon_monster(short summon_class);

#endif
