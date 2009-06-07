/*
 *  dlgutil.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 22/04/09.
 *
 */

#define FCD fancy_choice_dialog

short cd_create_custom_dialog(WindowPtr parent, Str255 strs[6],short pic_num,short btns[3]);
void oops_error(short error,short code = 0, short mode = 0);
void give_error(char *text1, char *text2,short parent_num);
void display_strings(char *text1, char *text2,short str_label_1,short str_label_2,short str_label_1b, short str_label_2b, char *title,short sound_num,short graphic_num,short graphic_type,short parent_num) __attribute__((deprecated));
void display_strings(short a1,short a2, short b1, short b2, char *title,short sound_num,short graphic_num,short graphic_type,short parent_num);
void display_strings_event_filter (short item_hit);
void fancy_choice_dialog_event_filter (short item_hit);
short fancy_choice_dialog(short which_dlog,short parent);
void display_strings(char *text1, char *text2, char *title,short sound_num,short graphic_num,short graphic_type,short parent_num);
