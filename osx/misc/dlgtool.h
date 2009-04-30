/*
 *  dlogtool.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 15/04/09.
 *
 */

struct dlg_t {
	short key;
	short type;
	WindowPtr win;
	WindowPtr parent;
	short highest_item;
	bool draw_ready;
};

struct dlg_item_t {
	short dlg;
	short number;
	short type;
	Rect rect;
	short flag;
	char active;
	char key;
	short label;
	short label_loc;
};

struct dlg_label_t {
	char str[25];
	bool taken;
};

struct btn_t {
	short type;
	char* str;
	short left_adj;
	char def_key;
};

/*struct dlg_gw_store_t{
	GWorldPtr anim;
	GWorldPtr talkfaces;
	GWorldPtr items;
	GWorldPtr tiny_obj;
	GWorldPtr pc;
	GWorldPtr dlogpics;
	unsigned int num_monst_gw;
	GWorldPtr* monst;//[10];
	unsigned int num_ter_gw;
	GWorldPtr* terrain;//[7];
	GWorldPtr small_ter;
	GWorldPtr fields;
	GWorldPtr pc_stats;
	GWorldPtr item_stats;
	GWorldPtr text_area;
	GWorldPtr storage;
	GWorldPtr terrain_screen;
	GWorldPtr text_bar;
	GWorldPtr orig_text_bar;
	GWorldPtr buttons;
	GWorldPtr party_template;
	GWorldPtr mixed;
	GWorldPtr* custom;
};*/

typedef void (*dlg_filter_t)(short);
struct dlg_filters{
	dlg_filters(int n,dlg_filter_t f) : id(n), callback(f) {}
	short id;
	dlg_filter_t callback;
};

#include <map>
#include <string>
using std::map;
using std::string;

typedef GWorldPtr* gw;
#define INIT_PARAMS gw g1,  gw g2,  gw g3,  gw g4,  gw g5,  gw g6,  gw g7,  gw g8,  gw g9,  gw g10, \
					gw g11, gw g12, /*gw g13, gw g14, gw g15, gw g16, gw g17, gw g18, gw g19,*/ gw g20, gw g21

void cd_init_dialogs(INIT_PARAMS);
short cd_create_dialog_parent_num(short dlog_num,short parent);
short cd_create_dialog(short dlog_num,WindowPtr parent);
short cd_kill_dialog(short dlog_num);
short cd_process_keystroke(WindowPtr window,char char_hit,short *item);
short cd_process_click(WindowPtr window,Point the_point, short mods,short *item);
void cd_attach_key(short dlog_num,short item_num,char key);
void csp(short dlog_num, short item_num, short pict_num, short pict_type);
void cd_set_pict(short dlog_num, short item_num, short pict_num, short pict_type);
void cd_activate_item(short dlog_num, short item_num, short status);
short cd_get_active(short dlog_num, short item_num);
void cd_get_item_text(short dlog_num, short item_num, char *str);
void csit(short dlog_num, short item_num, char *str);
void cd_set_item_text(short dlog_num, short item_num, char *str);
void cd_retrieve_text_edit_str(short dlog_num, short item_num, char *str);
short cd_retrieve_text_edit_num(short dlog_num, short item_num);
void cd_set_text_edit_str(short dlog_num, short item_num, char *str);
void cd_set_text_edit_num(short dlog_num, short item_num, short num);
void cdsin(short dlog_num, short item_num, short num);
void cd_set_item_num(short dlog_num, short item_num, short num);
void cd_set_led(short dlog_num,short item_num,short state);
void cd_flip_led(short dlog_num,short item_num,short item_hit);
void cd_set_led_range(short dlog_num,short first_led,short last_led,short which_to_set);
void cd_hit_led_range(short dlog_num,short first_led,short last_led,short which_to_set);
short cd_get_led_range(short dlog_num,short first_led,short last_led);
void cd_set_flag(short dlog_num,short item_num,short flag);
short cd_get_led(short dlog_num,short item_num);
void cd_text_frame(short dlog_num,short item_num,short frame);
void cd_add_label(short dlog_num, short item_num, char *label, short label_flag);
void cd_take_label(short dlog_num, short item_num);
void cd_key_label(short dlog_num, short item_num,short loc);
void cd_draw_item(short dlog_num,short item_num);
void cd_initial_draw(short dlog_num);
void cd_draw(short dlog_num);
void cd_redraw(WindowPtr window);
void cd_frame_item(short dlog_num, short item_num, short width);
void cd_erase_item(short dlog_num, short item_num, bool just_label = false);
void cd_erase_rect(short dlog_num,Rect to_fry);
void cd_press_button(short dlog_num, short item_num);
bool dialog_not_toast();
void toast_dialog();
void untoast_dialog();
pascal bool cd_event_filter (DialogPtr hDlg, EventRecord *event, short *dummy_item_hit);
short cd_run_dialog();
void cd_register_event_filter(short id, dlg_filter_t filter);
void cd_register_default_event_filter(dlg_filter_t filter);
void cd_set_bg_pat_num(short n);
void cd_set_text_clr(RGBColor clr);
