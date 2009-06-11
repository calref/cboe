/*
 *  graphtool.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#ifndef GRAPHTOOL_H
#define GRAPHTOOL_H
#include <Carbon/Carbon.h>
#include <string>
#define  LINES_IN_TEXT_WIN	11

enum cursor_type {
	wand_curs = 0,
	eyedropper_curs = 1,
	brush_curs = 2,
	spray_curs = 3,
	eraser_curs = 4,
	topleft_curs = 5,
	bottomright_curs = 6,
	hand_curs = 7,
	NW_curs = 8,
	N_curs = 9,
	NE_curs = 10,
	W_curs = 11,
	wait_curs = 12,
	E_curs = 13,
	SW_curs = 14,
	S_curs = 15,
	SE_curs = 16,
	sword_curs = 17,
	boot_curs = 18,
	drop_curs = 19,
	target_curs = 20,
	talk_curs = 21,
	key_curs = 22,
 	look_curs = 23,
};

struct m_pic_index_t {
	unsigned char i, x, y;
};

typedef unsigned short pic_num_t;
void init_graph_tool(void (*redraw_callback)());
void clean_up_graphtool();
GWorldPtr load_pict(std::string picture_to_get, std::string scen_name = "");
GWorldPtr load_pict(int picture_to_get) __attribute__((deprecated));
GWorldPtr load_bmp(unsigned char *data, unsigned long length);
void set_cursor(cursor_type which_curs);
void restore_cursor();
void rect_draw_some_item (GWorldPtr src_gworld,Rect src_rect,GWorldPtr targ_gworld,Rect targ_rect,char mode = srcCopy);
void rect_draw_some_item (GWorldPtr src_gworld,Rect src_rect,Rect targ_rect,Point offset,char mode = srcCopy);
void char_win_draw_string(WindowPtr dest_window,Rect dest_rect,const char *str,short mode,short line_height,Point offset = (Point){0,0});
void char_port_draw_string(GrafPtr dest_window,Rect dest_rect,const char *str,short mode,short line_height,Point offset = (Point){0,0});
void win_draw_string(GrafPtr dest_window,Rect dest_rect,Str255 str,short mode,short line_height,Point offset = (Point){0,0});
short string_length(const char *str);
//OSStatus flip_pict(OSType domain, OSType type, short id, void *ptr, UInt32 size, bool isNative, void *refcon);
//void draw_terrain();
Rect calc_rect(short i, short j);
Rect get_custom_rect (short which_rect);
short get_custom_rect (short which_rect, Rect& store_rect);
void get_str(Str255 str,short i, short j);
GWorldPtr importPictureFileToGWorld(const FSSpec *fileSpec);
void writeGWorldToPNGFile(GWorldPtr gw, const FSSpec *fileSpec);
void tileImage(Rect area, GWorldPtr img, short mode = srcCopy);
void tileImage(Rect area, GWorldPtr img, Rect srcRect, short mode = srcCopy);
void tileImage(RgnHandle area, GWorldPtr img, short mode = srcCopy);
void tileImage(RgnHandle area, GWorldPtr img, Rect srcRect, short mode = srcCopy);

#ifndef GRAPHTOOL_CPP
extern cursor_type arrow_curs[3][3];
extern cursor_type current_cursor;
extern m_pic_index_t m_pic_index[200];
extern Rect bg[];
extern short geneva_font_num,dungeon_font_num;
#endif

#endif
