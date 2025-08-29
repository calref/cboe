
#include "universe/creature.hpp"

bool handle_wandering_specials(short mode);
bool check_special_terrain(location where_check,eSpecCtx mode,cPlayer& which_pc,bool *forced);
void check_fields(location where_check,eSpecCtx mode,cPlayer& which_pc);
void use_spec_item(short item, bool& need_redraw);
void use_item(short pc,short item);
bool use_space(location where);
bool adj_town_look(location where);
short get_sound_type(eDamageType dam_type, short forced_sound_type = -1);
short get_boom_type(eDamageType dam_type);
short damage_monst(cCreature& which_m, short who_hit, short how_much, eDamageType dam_type, short sound_type = -1, bool do_print = true);
void petrify_monst(cCreature& which_m,int strength);
void kill_monst(cCreature& which_m,short who_killed,eMainStatus type = eMainStatus::DEAD);
void special_increase_age(long length = 1, bool queue = false);
void do_rest(long length, int hp_restore, int mp_restore);
void out_move_party(short x,short y) ;
void teleport_party(short x,short y,short mode);
bool run_stone_circle(short which);
void change_level(short town_num,short x,short y);
void push_things();
bool queue_special(eSpecCtx mode, eSpecCtxType which_type, spec_num_t spec, location spec_loc);
void run_special(eSpecCtx which_mode, eSpecCtxType which_type, spec_num_t start_spec, location spec_loc, short* a = nullptr, short* b = nullptr, bool* redraw = nullptr);
void run_special(pending_special_type spec, short* a, short* b, bool* redraw);

void set_campaign_flag(short sdf_a, short sdf_b, short cpf_a, short cpf_b, short str, bool get_send);
