void init_main_buttons();
void Set_up_win ();
void redraw_screen();
void draw_main_screen();
void do_button_action(short which_pc,short which_button);
GWorldPtr load_pict(short picture_to_get);
void draw_items(short clear_first);
void display_party(short mode,short clear_first);
void undo_clip();
void add_string_to_buf(char *str) ;
void rect_draw_some_item (GWorldPtr	src_gworld, Rect	src_rect, GWorldPtr	targ_gworld,Rect targ_rect, 
char masked,short main_win);
void char_win_draw_string(GrafPtr dest_window,Rect dest_rect,char *str,short mode,short line_height);
void win_draw_string(GrafPtr dest_window,Rect dest_rect,Str255 str,short mode,short line_height);
void display_strings_event_filter (short item_hit);
void display_strings(short str1a,short str1b,short str2a,short str2b,
	char *title,short sound_num,short graphic_num,short parent_num);
void c2p(Str255 str) ;
void p2c(Str255 str);
void get_str(Str255 str,short i, short j);
short string_length(char *str);
void make_cursor_sword();
