#ifndef _FILEIO_H
	#define _FILEIO_H

BOOL check_for_interrupt();
void Get_Path(char* path);
void file_initialize();
void load_file();
void save_file(short mode);
void form_template_terrain() ;
void load_town(short town_num,short mode,short extra,char *str);
void shift_universe_left();
void shift_universe_right();
void shift_universe_up();
void shift_universe_down();
void position_party(short out_x,short out_y,short pc_pos_x,short pc_pos_y);
void build_outdoors();
void save_outdoor_maps();
void add_outdoor_maps();
void fix_boats();
void load_outdoors(short to_create_x, short to_create_y, short targ_x, short targ_y,
	short mode,short extra,char *str);
void get_reg_data();
long do_waterfall(long flag);
void import_template_terrain();
void import_anim_terrain(short mode);
void start_data_dump();
void end_data_dump();
void set_terrain(location l, unsigned char terrain_type);
void set_up_ter_pics();
Boolean load_scenario();
short onm(char x_sector,char y_sector);
void build_scen_headers();
short get_buf_ptr(short flag);
void build_data_file(short mode);
#endif
