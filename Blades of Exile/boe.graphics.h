#ifndef _GRAPHICS_H
	#define _GRAPHICS_H
	
void draw_startup_anim();
void draw_startup_stats();
void adjust_window_mode();
void plop_fancy_startup();
void fancy_startup_delay();
void init_startup();
void init_animation();
void next_animation_step() ;
void draw_startup(short but_type);
void reload_startup();
void draw_anim();
void place_anim(); 
void draw_start_button(short which_position,short which_button);
void main_button_click(short mode,RECT button_rect);
void arrow_button_click(RECT button_rect);
void end_startup();
void Set_up_win ();
void load_main_screen();
void create_clip_region();
void set_gworld_fonts(short font_num);
void redraw_screen(short mode);
void put_background();
void refresh_screen(short mode);
void draw_buttons(short mode);
void draw_text_area(short mode);
void reset_text_bar();
void draw_text_bar(short mode);
void put_text_bar(char *str);

void draw_terrain(short	mode);
void place_trim(short q,short r,location where,unsigned char ter_type);
void draw_trim(short q,short r,short which_trim,short which_mode);
Boolean extend_road_terrain(unsigned char ter);
void place_road(short q,short r,location where);
void draw_rest_screen();
void boom_space(location where,short mode,short type,short damage,short sound);
void draw_pointing_arrows() ;
void redraw_terrain();
void draw_targets(location center);
void erase_spot(short i,short j);
void draw_targeting_line(POINT where_curs);
Boolean party_toast();
void redraw_partial_terrain(RECT redraw_rect);
void final_process_dialog(short which_dlog);
Boolean is_nature(char i, char j);
void put_dialog_graphic(short graphic_num,short spec_g,RECT draw_rect);
void lose_graphics();
void put_anim_str();
void pre_boom_space(location where,short mode,short type,short damage,short sound);

void swap_platform_graphics();
#endif
