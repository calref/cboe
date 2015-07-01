
#include "creature.hpp"

bool handle_wandering_specials (short which,short mode);
bool check_special_terrain(location where_check,eSpecCtx mode,cPlayer& which_pc,bool *forced);
void check_fields(location where_check,eSpecCtx mode,cPlayer& which_pc);
void use_spec_item(short item);
void use_item(short pc,short item);
bool use_space(location where);
bool adj_town_look(location where);
bool damage_monst(cCreature& which_m, short who_hit, short how_much, eDamageType dam_type, short sound_type, bool do_print = true);
void petrify_monst(cCreature& which_m,int strength);
void kill_monst(cCreature& which_m,short who_killed,eMainStatus type = eMainStatus::DEAD);
void special_increase_age(long length = 1, bool queue = false);
void do_rest(long length, int hp_restore, int mp_restore);
void out_move_party(short x,short y) ;
void teleport_party(short x,short y,short mode);
bool run_stone_circle(short which);
void fade_party();
void change_level(short town_num,short x,short y);
void push_things();
void queue_special(eSpecCtx mode, unsigned short which_type, short spec, location spec_loc);
void run_special(eSpecCtx which_mode,short which_type,short start_spec,location spec_loc,short *a,short *b,short *redraw);
void run_special(pending_special_type spec, short* a, short* b, short* redraw);
cSpecial get_node(short cur_spec,short cur_spec_type);
void general_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void setsd(short a,short b,short val);
void handle_message(eSpecCtx which_mode,short cur_type,short mess1,short mess2,short*a,short*b,std::string title="",pic_num_t pic=-1,ePicType pt=PIC_SCEN);
void get_strs(std::string& str1, std::string& str2,short cur_type,short which_str1,short which_str2) ;
void ifthen_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void affect_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void oneshot_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void townmode_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void rect_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void outdoor_spec(eSpecCtx which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);

void set_campaign_flag(short sdf_a, short sdf_b, short cpf_a, short cpf_b, short str, bool get_send);
