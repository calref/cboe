
#ifndef BOE_GAME_FILEIO_H
#define BOE_GAME_FILEIO_H

#include <boost/filesystem/path.hpp>
#include "location.hpp"

void finish_load_party();
void change_rect_terrain(rectangle r,ter_num_t terrain_type,short probability,bool hollow);
void form_template_terrain() ;
void shift_universe_left();
void shift_universe_right();
void shift_universe_up();
void shift_universe_down();
void position_party(short out_x,short out_y,short pc_pos_x,short pc_pos_y);
void build_outdoors();
void save_outdoor_maps();
void add_outdoor_maps();
long do_waterfall(long flag);
short init_data(short flag);
void import_template_terrain();
void import_anim_terrain(short mode);
void start_data_dump();
void end_data_dump();
short onm(char x_sector,char y_sector);
void build_scen_headers();
bool load_scenario_header(fs::path filename/*,short header_entry*/);
fs::path locate_scenario(std::string scen_name);

void alter_rect(rectangle *r);

#endif
