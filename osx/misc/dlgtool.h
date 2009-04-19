/*
 *  dlogtool.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 15/04/09.
 *
 */

struct m_pic_index_t {
	unsigned char i, x, y;
};

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
	char type;
	Rect rect;
	short flag;
	char active;
	char key;
	short label;
	short label_loc;
	short flag2;
};

struct dlg_label_t {
	char str[25];
	bool taken;
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

#define DLG_KEY_LEFT	20
#define DLG_KEY_RIGHT	21
#define DLG_KEY_UP		22
#define DLG_KEY_DOWN	23
#define DLG_KEY_ESC		24
#define DLG_KEY_CTRL1	25
#define DLG_KEY_CTRL2	26
#define DLG_KEY_CTRL3	27
#define DLG_KEY_CTRL4	28
#define DLG_KEY_CTRL5	29
#define DLG_KEY_CTRL6	30
#define DLG_KEY_RETURN	31

#define DLG_BTN_SM		0 // 23x23 (PICT id 2000 / 2001)
#define DLG_BTN_REG		1 // 63x23 (PICT id 2002 / 2003)
#define DLG_BTN_LG		2 // 102x23 (PICT id 2004 / 2005)
#define DLG_BTN_HELP	3 // 16x13 (PICT id 2006 / 2007) white bubble w/ ? mark
#define DLG_BTN_LEFT	4 // 63x23 (PICT id 2008 / 2009) with left arrow
#define DLG_BTN_RIGHT	5 // 63x23 (PICT id 2010 / 2011) with right arrow
#define DLG_BTN_UP		6 // 63x23 (PICT id 2012 / 2013) with up arrow
#define DLG_BTN_DOWN	7 // 63x23 (PICT id 2014 / 2015) with down arrow
#define DLG_BTN_LED1	8 // 6x6 (PICT id 2016 / 2017)
#define DLG_BTN_LED2	9 // 14x10 (PICT id 2018 / 2019)
#define DLG_BTN_LED3	10 // 14x10 (PICT id 2020 / 2021)
#define DLG_BTN_DONE	11 // 63x23 (PICT id 2022 / 2023) says "Done"
#define DLG_BTN_TALL	12 // 63x40 (PICT id 2024 / 2025)
#define DLG_BTN_TRAIT	13 // 63x40 (PICT id 2026 / 2027) says "Race Good/Bad Traits"
#define DLG_BTN_PUSH	14 // 30x30 (PICT id 2028 / 2029) red round button

struct btn_t {
	short type;
	char* str;
	short left_adj;
	char def_key;
};

#include <map>
#include <string>
using std::map;
using std::string;

typedef GWorldPtr* gw;
#define INIT_PARAMS gw g1,  gw g2,  gw g3,  gw g4,  gw g5,  gw g6,  gw g7,  gw g8,  gw g9,  gw g10, \
					gw g11, gw g12, gw g13, gw g14, gw g15, gw g16, gw g17, gw g18, gw g19, gw g20, gw g21

void cd_init_dialogs(INIT_PARAMS);
short cd_create_dialog_parent_num(short dlog_num,short parent);
short cd_create_dialog(short dlog_num,WindowPtr parent);
short cd_kill_dialog(short dlog_num,short parent_message);
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
void cd_erase_item(short dlog_num, short item_num, Boolean just_label = FALSE);
void cd_erase_rect(short dlog_num,Rect to_fry);
void cd_press_button(short dlog_num, short item_num);
bool dialog_not_toast();
void toast_dialog();
void untoast_dialog();
pascal Boolean cd_event_filter (DialogPtr hDlg, EventRecord *event, short *dummy_item_hit);
short cd_run_dialog();
void cd_register_event_filter(short id, dlg_filter_t filter);
void cd_register_default_event_filter(dlg_filter_t filter);