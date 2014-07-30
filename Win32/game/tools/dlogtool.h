#ifndef _DLOGTOOL_H
	#define _DLOGTOOL_H

#include "../globvar.h"
//#include "boe.graphutil.h"

void cd_set_flag(short dlog_num,short item_num,short flag);
short cd_get_led(short dlog_num,short item_num);
short cd_create_custom_dialog(HWND parent,char *strs,short pic_num,short buttons[3]);
void frame_di(HWND hDlg, short item_num, short val);
HDC cd_get_dlog_dc(short which_slot);
short cd_get_active(short dlog_num, short item_num);
short cd_create_dialog_parent_num(short dlog_num,short parent);
void cd_init_dialogs();
short cd_create_dialog(short dlog_num,HWND parent) ;
short cd_kill_dialog(short dlog_num,short parent_message);
short cd_process_click(HWND window,POINT the_point, WPARAM wparam, LPARAM lparam,short *item);
short cd_process_syskeystroke(HWND window,WPARAM wparam, LPARAM lparam,short *item);
short cd_process_keystroke(HWND window,WPARAM wparam, LPARAM lparam,short *item);
void cd_attach_key(short dlog_num,short item_num,char key);
void cd_set_pict(short dlog_num, short item_num, short pict_num);
void cd_activate_item(short dlog_num, short item_num, short status);
void cd_set_item_text(short dlog_num, short item_num, char const *str);
void cd_set_item_num(short dlog_num, short item_num, short num);
void cd_set_led(short dlog_num,short item_num,short state);
void cd_text_frame(short dlog_num,short item_num,short frame);
void cd_add_label(short dlog_num, short item_num, char *label, short label_flag);
void cd_key_label(short dlog_num, short item_num,short loc);
void cd_draw_item(short dlog_num,short item_num);
void cd_initial_draw(short dlog_num);
void cd_draw(short dlog_num);
void cd_redraw(HWND window);
void cd_frame_item(short dlog_num, short item_num, short width);
void cd_erase_item(short dlog_num, short item_num);
void cd_press_button(short dlog_num, short item_num);
short cd_get_indices(short dlg_num, short item_num, short *dlg_index, short *item_index);
short cd_get_dlg_index(short dlog_num);
short cd_find_dlog(HWND window, short *dlg_num, short *dlg_key);
short cd_get_item_id(short dlg_num, short item_num);
void center_window(HWND window);
RECT get_item_rect(HWND hDlg, short item_num);
void frame_dlog_rect(HWND hDlg, RECT rect, short val);
void draw_dialog_graphic(HWND hDlg, RECT rect, short which_g, Boolean do_frame,short win_or_gworld)   ;
void showcursor(Boolean a);
short cd_create_custom_pic_dialog(HWND parent, HBITMAP picture);

void cd_get_text_edit_str(short dlog_num, char *str);
// NOTE!!! Expects a c string
inline void cd_set_text_edit_str(short, char *str)
	{ if (edit_box != NULL) SetWindowText(edit_box,str); }
inline void cdsin(short dlog_num, short item_num, short num)
	{ cd_set_item_num(dlog_num, item_num, num); }
inline void csit(short dlog_num, short item_num, char const *str)
	{ cd_set_item_text( dlog_num,  item_num, str); }
inline void csp(short dlog_num, short item_num, short pict_num)
	{ cd_set_pict( dlog_num, item_num, pict_num); }
inline void cd_set_edit_focus() { if (edit_box) SetFocus(edit_box); }
void ModalDialog();

extern HWND dlgs[ND];

RECT calc_rect(short i, short j);

#endif
