
#include "fields.hpp"
#include "location.hpp"

void Set_up_win ();
void run_startup_g();
void load_graphics();
void draw_main_screen();
void redraw_screen();
void draw_lb();
void draw_lb_slot (short which,short mode) ;
void draw_rb();
void draw_rb_slot (short which,short mode) ;
void set_up_terrain_buttons(bool reset);
void draw_terrain();
void draw_monsts();
void draw_items();
void force_tiny_redraw();
void draw_one_terrain_spot (short i,short j,ter_num_t terrain_to_draw);
void draw_one_tiny_terrain_spot (short i,short j,ter_num_t terrain_to_draw,short size,bool road);
rectangle get_template_rect (unsigned short type_wanted);
void draw_frames();
void place_location();
void set_string(std::string string,std::string string2);
bool is_special(short i,short j);
void take_special(short i,short j);
void make_special(short i,short j);
void sort_specials();
bool is_field_type(short i,short j,eFieldType field_type);
void make_field_type(short i,short j,eFieldType field_type);
void take_field_type(short i,short j,eFieldType field_type);
bool container_there(location l);
bool is_spot(short i,short j);
bool is_road(short i,short j);
short string_length(char *str);
rectangle get_custom_rect (short which_rect);
void init_dialogs();
void record_display_strings();
