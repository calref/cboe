/*
 *  dloglowlevel.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#define IN_FRONT	(WindowPtr)-1L

void process_new_window (short which_dlg);
short cd_get_indices(short dlg_num, short item_num, short *dlg_index, short *item_index);
short cd_get_dlg_index(short dlog_num);
short cd_find_dlog(WindowPtr window, short *dlg_num, short *dlg_key);
short cd_get_item_id(short dlg_num, short item_num);
Rect get_item_rect(WindowPtr hDlg, short item_num);
void frame_dlog_rect(GrafPtr hDlg, Rect rect, short val, short med_or_lt = 0);
void draw_dialog_graphic(GrafPtr hDlg, Rect rect, short which_g, short type_g, bool do_frame,short win_or_gworld);
void convert_pict(short& which_g, short& type_g,Rect& rect);
void draw_preset_ter_pic(short which_g,Rect& rect);
void draw_preset_anim_ter_pic(short which_g,Rect& rect);
void draw_preset_monst_pic_small(short which_g,Rect& rect);
void draw_preset_monst_pic_wide(short which_g,Rect& rect);
void draw_preset_monst_pic_tall(short which_g,Rect& rect);
void draw_preset_monst_pic_large(short which_g,Rect& rect);
void draw_preset_item_pic(short which_g,Rect& rect);
void draw_preset_dlg_pic(short which_g,Rect& rect);
void draw_preset_pc_pic(short which_g,Rect& rect);
void draw_preset_talk_pic(short which_g,Rect& rect);
void draw_item_info_help_pic(Rect& rect);
void draw_pc_info_help_pic(Rect& rect);
void draw_help_pic(short which_g,Rect& rect);
void draw_combat_ap_help_pic(Rect& rect);
void draw_pc_stat_help_pic(Rect& rect);
void draw_preset_scen_pic(short which_g,Rect& rect);
void draw_preset_field_pic(short which_g,Rect& rect);
void draw_custom_space_pic(short which_g,Rect& rect);
void draw_custom_monst_wide_pic(short which_g,Rect& rect);
void draw_custom_monst_tall_pic(short which_g,Rect& rect);
void draw_custom_monst_large_pic(short which_g,Rect& rect);
void draw_custom_dlg_pic_split(short which_g,Rect& rect);
void draw_custom_talk_pic_split(short which_g,Rect& rect);
void draw_preset_scen_pic_large(short which_g,Rect& rect);
void draw_preset_dlg_pic_large(short which_g,Rect& rect);
