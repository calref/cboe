
#ifndef BOE_GAME_GRAPHICS_H
#define BOE_GAME_GRAPHICS_H

#include <SFML/Graphics.hpp>
#include "location.hpp"

enum {
	REFRESH_NONE = 0,
	REFRESH_TERRAIN = 1 << 0,
	REFRESH_STATS = 1 << 1,
	REFRESH_INVEN = 1 << 2,
	REFRESH_TRANS = 1 << 3,
	REFRESH_BAR = 1 << 4,
	REFRESH_DLOG = 1 << 5,
	REFRESH_TEXT = REFRESH_BAR | REFRESH_TRANS,
	REFRESH_ALL = 0x3f
};

void adjust_window_mode();
void init_startup();
void reload_startup();
void draw_startup(short but_type);
void draw_anim();
void place_anim(); 
void draw_start_button(short which_position,short which_button);
void main_button_click(int which_button);
void arrow_button_click(rectangle button_rect);
void end_startup();
void load_main_screen();
void redraw_screen(int refresh);
void put_background();
void draw_buttons(short mode);
void draw_text_bar();
void refresh_text_bar();
void put_text_bar(std::string str);
void draw_terrain(short	mode = 0);
void place_trim(short q,short r,location where,ter_num_t ter_type);
void draw_trim(short q,short r,short which_trim,short which_mode);
void place_road(short q,short r,location where,bool here);
void draw_rest_screen();
void boom_space(location where,short mode,short type,short damage,short sound);
void draw_pointing_arrows() ;
void redraw_terrain();
void draw_targets(location center);
void frame_space(location where,short mode,short width,short height);
void erase_spot(short i,short j);
void draw_targeting_line(location where_curs);
void redraw_partial_terrain(rectangle redraw_rect);
bool is_nature(short i, short j, unsigned short ground_t);
void put_dialog_graphic(short graphic_num,short spec_g,rectangle draw_rect);
void draw_startup_stats();
void draw_trim(short q,short r,short which_trim,ter_num_t ground_ter);

void draw_startup_anim(bool advance);

#endif
