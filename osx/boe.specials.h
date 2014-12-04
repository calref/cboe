bool town_specials(short which,short t_num);
bool handle_wandering_specials (short which,short mode);
bool check_special_terrain(location where_check,short mode,short which_pc,short *spec_num,
	bool *forced);
void check_fields(location where_check,short mode,short which_pc);
void use_spec_item(short item);
void use_item(short pc,short item);
bool use_space(location where);
bool adj_town_look(location where);
void PSOE(short which_special,unsigned char *stuff_done_val,short where_put);
bool damage_monst(short which_m, short who_hit, short how_much, short how_much_spec, eDamageType dam_type, short sound_type);
void kill_monst(cCreature *which_m,short who_killed);
void special_increase_age();
void out_move_party(char x,char y) ;
void teleport_party(short x,short y,short mode);
bool run_stone_circle(short which);
void fade_party();
void change_level(short town_num,short x,short y);
void push_things();
void special_increase_age();
void run_special(short which_mode,short which_type,short start_spec,location spec_loc,short *a,short *b,short *redraw);
cSpecial get_node(short cur_spec,short cur_spec_type);
void general_spec(short which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void setsd(short a,short b,short val);
void handle_message(short which_mode,short cur_type,short mess1,short mess2,short *a,short *b);
void get_strs(std::string& str1, std::string& str2,short cur_type,short which_str1,short which_str2) ;
void ifthen_spec(short which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void affect_spec(short which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void oneshot_spec(short which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void townmode_spec(short which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void rect_spec(short which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void outdoor_spec(short which_mode,cSpecial cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
