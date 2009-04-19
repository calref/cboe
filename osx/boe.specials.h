Boolean town_specials(short which,short t_num);
Boolean handle_wandering_specials (short which,short mode);
Boolean check_special_terrain(location where_check,short mode,short which_pc,short *spec_num,
	Boolean *forced);
void check_fields(location where_check,short mode,short which_pc);
void use_spec_item(short item);
void use_item(short pc,short item);
Boolean use_space(location where);
Boolean adj_town_look(location where);
void PSOE(short str1a,short str1b,short str2a,short str2b,
	short which_special,unsigned char *stuff_done_val,short where_put);
Boolean damage_monst(short which_m, short who_hit, short how_much, short how_much_spec, short dam_type);
void kill_monst(creature_data_type *which_m,short who_killed);
void special_increase_age();
void out_move_party(char x,char y) ;
void teleport_party(short x,short y,short mode);
Boolean run_stone_circle(short which);
void fade_party();
void change_level(short town_num,short x,short y);
void push_things();
void special_increase_age();
void run_special(short which_mode,short which_type,short start_spec,location spec_loc,short *a,short *b,short *redraw);
special_node_type get_node(short cur_spec,short cur_spec_type);
void general_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void setsd(short a,short b,short val);
void handle_message(short which_mode,short cur_type,short mess1,short mess2,short *a,short *b);
void get_strs(char *str1,char *str2,short cur_type,short which_str1,short which_str2) ;
void ifthen_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void affect_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void oneshot_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void townmode_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void rect_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
void outdoor_spec(short which_mode,special_node_type cur_node,short cur_spec_type,
	short *next_spec,short *next_spec_type,short *a,short *b,short *redraw);
