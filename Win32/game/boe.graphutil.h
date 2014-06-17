#ifndef _GRAPHUTIL_H
	#define _GRAPHUTIL_H

void draw_one_terrain_spot (short i,short j,short terrain_to_draw,short dest);
void draw_monsters();
void play_see_monster_str(unsigned char m);
void draw_pcs(location center,short mode);
void draw_items();
void draw_outd_boats(location center);
void draw_town_boat(location center) ;
void draw_fields();
void draw_spec_items();
void draw_sfx();
void draw_one_field(unsigned char flag,short source_x,short short_y);
void draw_one_spec_item(unsigned char flag,short source_x,short short_y);
void draw_party_symbol(location center);
RECT return_item_rect(short wanted);
RECT get_monster_template_rect (unsigned char type_wanted,short mode,short which_part) ;
RECT get_item_template_rect (short type_wanted);
unsigned char get_t_t(int x, int y);  // returns terrain type at where
Boolean is_fluid(unsigned char ter_type);
Boolean is_shore(unsigned char ter_type);
Boolean is_wall(unsigned char ter_type);
Boolean is_ground(unsigned char ter_type);
void make_town_trim(short mode);
void make_out_trim();
Boolean pt_in_rect(location loc, RECT16 rect);
void adjust_monst_menu();
void frame_space(location where,short mode,short width,short height);

char add_trim_to_array(int x, int y, unsigned char ter_type);
char add_trim_to_array(location where,unsigned char ter_type);


void rect_draw_some_item(HBITMAP src, RECT16 src_rect, HBITMAP dest, RECT16 dest_rect,
	short trans, short main_win);
void fry_dc(HWND hwnd,HDC dc);

void DisposeGWorld(HBITMAP bitmap);
HBITMAP load_pict(short pict_num,HDC);
void paint_pattern(HBITMAP dest,short which_mode,RECT dest_rect,short which_pattern);

/* GK */
extern HBITMAP ReadBMP(char * fileName);
extern HBITMAP ReadScenFile(char* filename);

#endif
