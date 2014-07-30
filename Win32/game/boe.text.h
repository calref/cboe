#ifndef _TEXT_H
	#define _TEXT_H

#include <windows.h>	// RECT, HDC etc.
#include "classes/location.h"	// location
#include "classes/pc.h"			// pc_record_type

#include "global.h"		// structs

#define	TEXT_BUF_LEN	70

short text_pc_has_abil_equip(short pc_num,short abil);
void put_pc_screen();
void place_buy_button(short position,short pc_num,short item_num,HDC hdc);
void put_item_screen(short screen_num,short suppress_buttons);
void place_item_bottom_buttons();
RECT get_party_template_rect(short pc_num,short mode);
void set_stat_window(short new_stat);
void place_item_button(short which_button_to_put,short which_slot,short which_button_position,short extraVal);
short first_active_pc();
void refresh_stat_areas(short mode);
short total_encumberance(short pc_num);
short get_tnl(pc_record_type *pc);
void draw_pc_effects(short pc,HDC dest_dc);
void print_party_stats() ;
short do_look(location space);
short town_boat_there(location where);
short out_boat_there(location where);
short town_horse_there(location where);
short out_horse_there(location where);
void notify_out_combat_began(out_wandering_type encounter,short *nums);
void get_m_name(char *str,unsigned char num);
void get_ter_name(char *str,unsigned char num);
void print_monst_name(unsigned char m_type);
void print_monst_attacks(unsigned char m_type,short target);
void damaged_message(short damage,short type);
void print_monster_going(char *combat_str,unsigned char m_num,short ap);
void monst_spell_note(unsigned char number,short which_mess);
void monst_cast_spell_note(unsigned char number,short spell,short type);
void monst_breathe_note(unsigned char number);
void monst_damaged_mes(short which_m,short how_much,short how_much_spec);
void monst_killed_mes(short which_m);
void print_nums(short a,short b,short c);
short print_terrain(location space);
void add_string_to_buf(char const *string);
void print_buf () ;
void through_sending();
void Display_String(char *str);
RECT coord_to_rect(short i,short j);
void c2p(char *str) ;
void p2c(char *str);
short string_length(char *str,HDC hdc);
void char_win_draw_string(HDC dest_window,RECT dest_rect,char *str,short mode,short line_height);
void win_draw_string(HDC dest_window,RECT dest_rect,char *str,short mode,short line_height);
short calc_day();
Boolean day_reached(unsigned char which_day, unsigned char which_event);
void display_string(char *str);
void Draw_Some_Item (HBITMAP src_gworld, RECT src_rect, HBITMAP targ_gworld, location target, char masked, short main_win);
void WinBlackDrawString(char *string,short x,short y);
void WinDrawString(char *string,short x,short y);
void DrawString(char *string,short x,short y,HDC hdc);
void FlushEvents(short mode);
void undo_clip();
void ClipRect(RECT *rect);
void beep();
void GetIndString(char *str,short i, short j) ;
void MeasureText(short str_len,char *str, short *len_array,HDC hdc);
RECT get_custom_rect (short which_rect);

#endif
