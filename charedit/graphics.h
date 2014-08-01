void init_main_buttons();
void Set_up_win ();
void redraw_screen();
void draw_main_screen();
void do_button_action(short which_pc,short which_button);
void draw_items(short clear_first);
void display_party(short mode,short clear_first);


void char_win_draw_string(HDC dest_window,RECT dest_rect,char const *str,short mode,short line_height);
void win_draw_string(HDC dest_hdc,RECT dest_rect,char *str,short mode,short line_height);
void display_strings_event_filter (short item_hit);
void display_strings(short str1a,short str1b,short str2a,short str2b,
	char const *title,short sound_num,short graphic_num,short parent_num);
void c2p(char *str) ;
void p2c(char *str);
void get_str(char *,short i, short j);
short string_length(char *str,HDC hdc);
void make_cursor_sword();

void MeasureText(short str_len,char *str, short *len_array,HDC hdc);
void GetIndString(char *str,short i, short j) ;

void lose_graphics();

