#ifndef _GLOBVAR_H
	#define _GLOBVAR_H
	
#include <windows.h>
#include "global.h"
#include "boe.newgraph.h"

extern RECT bottom_buttons[7];
extern RECT town_buttons[10];
extern RECT combat_buttons[9];
extern RECT world_screen;
extern RECT item_screen_button_rects[9];
extern RECT border_rect[4];
extern  RECT medium_buttons[4];
extern RECT startup_top;
extern RECT item_buttons[8][6]; 	// name, use, give, drip, info, sell/id
extern RECT pc_buttons[6][5];		// name, hp, sp, info, trade
extern pending_special_type special_queue[20];
extern Boolean end_scenario ;
extern short refer_priest[62];
extern short mage_need_select[62];
extern short priest_need_select[62];

extern Boolean item_area_button_active[8][6];
extern Boolean pc_area_button_active[6][5];
extern short item_bottom_button_active[9];

extern RECT pc_help_button,pc_area_rect,item_area_rect;

extern short current_terrain_type, num_out_moves;
extern short door_pc,store_drop_item;
extern short current_switch;
extern out_wandering_type store_wandering_special;
extern long dummy;
extern short store_shop_type;

extern short debug_ok;
extern short store_selling_values[8];

extern char *dir_string[8];
extern char get_new_terrain();
extern creature_start_type	save_monster_type;

extern short wand_loc_count;
extern short monst_place_count; // 1 - standard place	2 - place last

extern item_record_type start_items[6];

extern RECT shopping_rects[8][7];

extern unsigned char out[96][96];
extern unsigned char out_e[96][96];
extern unsigned char misc_i[64][64],sfx[64][64];
/* Mac stuff globals */

extern HWND text_sbar, item_sbar, shop_sbar;
extern RECT sbar_rect;
extern RECT shop_sbar_rect;
extern RECT item_sbar_rect;
extern int dialog_answer;
extern Boolean All_Done, dialog_not_toast;
extern Boolean in_startup_mode ;
//extern Boolean play_sounds;
extern Boolean diff_depth_ok ,spell_forced ,startup_loaded ;
extern Boolean save_maps ,party_in_memory ,in_scen_debug ;
extern Boolean window_in_front ;
extern Boolean belt_present ;
extern Boolean game_run_before ;
extern Boolean give_intro_hint ;
extern short on_spell_menu[2][62];
extern short on_monst_menu[256];
extern short current_cursor;
extern HCURSOR arrow_curs[3][3], sword_curs, key_curs, target_curs,talk_curs,look_curs;

extern party_record_type party;
extern outdoor_record_type outdoors[2][2];
extern current_town_type c_town;
extern big_tr_type t_d;
extern stored_items_list t_i;
extern setup_save_type setup_save;
extern location monster_targs[T_M];

extern short combat_posing_monster, current_working_monster; // 0-5 PC 100 + x - monster x
extern Boolean fast_bang ;
extern short spec_item_array[60];
extern short overall_mode,current_spell_range;
extern Boolean first_update ,anim_onscreen ,frills_on ,sys_7_avail,suppress_stat_screen ;
extern short stat_window,store_modifier;
extern Boolean monsters_going, boom_anim_active;
extern short give_delays;
extern Boolean modeless_exists[18];
extern short modeless_key[18];
extern HWND modeless_dialogs[18];

extern short town_size[3];
extern short which_item_page[6]; // Remembers which of the 2 item pages pc looked at
extern short ulx, uly;
extern short display_mode; // 0 - center 1- ul 2 - ur 3 - dl 4 - dr 5 - small win
extern short current_ground,stat_screen_mode;
extern long anim_ticks;

// Spell casting globals
extern short store_mage, store_priest;
extern short store_mage_lev, store_priest_lev;
extern short store_spell_target,pc_casting;
extern short pc_last_cast[2][6];
extern int num_targets_left;
extern location spell_targets[8];

extern long store_mouse;

/* Combat globals */
extern short which_combat_type,town_type;
extern location center;
extern unsigned char combat_terrain[64][64];
extern location pc_pos[6];
extern short current_pc;
extern short combat_active_pc;
extern effect_pat_type current_pat;
extern short monst_target[T_M]; // 0-5 target that pc   6 - no target  100 + x - target monster x
extern short spell_caster, missile_firer,current_monst_tactic;
extern short store_current_pc;
extern stored_items_list stored_items[3];
extern stored_outdoor_maps_type o_maps;

// Special stuff booleans
extern Boolean web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
extern Boolean sleep_field;

extern long last_anim_time;

/* Windoze stuff globals */
extern Boolean cursor_shown ;
extern short store_pc_being_created;

extern HWND mainPtr;
extern HWND force_dlog;
extern HFONT font,fantasy_font,small_bold_font,italic_font,underline_font,bold_font,tiny_font;
extern HBITMAP bmap;
extern HDC main_dc,main_dc2,main_dc3;
extern HINSTANCE store_hInstance;
extern HACCEL accel;
extern BOOL event_handled;
extern scenario_data_type scenario;
extern talking_record_type talking;
extern char scen_strs2[110][256];
extern stored_town_maps_type town_maps;

extern char szWinName[12];
extern char szAppName[14];

extern char file_path_name[256];

extern Boolean block_erase ;

/* dynamically allocated */
extern piles_of_stuff_dumping_type *data_store;
extern piles_of_stuff_dumping_type2 *data_store2;
extern piles_of_stuff_dumping_type3 *data_store3;
extern piles_of_stuff_dumping_type4 *data_store4;
extern piles_of_stuff_dumping_type5 *data_store5;
extern scen_item_data_type *scen_item_list;
extern scen_header_type *scen_headers;
/* end of dynamically allocated */

extern char create_line[60];
extern short spell_being_cast;
extern short missile_inv_slot, ammo_inv_slot;
extern short force_wall_position; //  10 -> no force wall
extern Boolean processing_fields ;
extern short futzing;
extern unsigned char store_sum_monst;
extern short store_sum_monst_cost;

extern location out_start_loc;
extern short hit_chance[51];
extern short abil_range[40];
extern short abil_odds[40];
extern short s_cost[2][62] ;
							 
extern short mage_range[80];
extern short priest_range[62];
extern short monst_mage_spell[55];
extern short monst_cleric_spell[55];
extern short monst_mage_cost[27];
extern short monst_mage_area_effect[27];
extern short monst_priest_cost[26];
extern short monst_priest_area_effect[26];

extern char *d_string[8];

extern short pc_marked_damage[6];
extern short monst_marked_damage[T_M];

extern location hor_vert_place[14];
extern location diag_place[14];

extern unsigned char beasts[5];
extern 	unsigned char m1[20];
extern 	unsigned char m2[16];
extern 	unsigned char m3[16];

extern 	short mage_caster_array[7][18];
extern 	short mage_emer_spells[7][4];
extern 	short priest_caster_array[7][10];
extern 	short priest_emer_spells[7][4];
extern effect_pat_type null_pat;
extern effect_pat_type single;
extern effect_pat_type t;
extern effect_pat_type small_square;
extern effect_pat_type square;
						
extern effect_pat_type open_square;
extern effect_pat_type radius2;
extern effect_pat_type radius3;
extern effect_pat_type field[8];

extern short last_attacked[6],pc_dir[6],pc_parry[6],pc_moves[6];
extern Boolean center_on_monst;

extern short sign_mode,person_graphic,store_person_graphic,store_sign_mode;
extern long num_talk_entries;
extern short store_tip_page_on;

extern short store_pre_talk_mode,store_personality,store_personality_graphic,shop_identify_cost;
extern HBITMAP talk_gworld;
extern Boolean talk_end_forced;
extern char old_str1[256];
extern char old_str2[256];
extern char one_back1[256];
extern char one_back2[256]; 
extern RECT talk_area_rect, word_place_rect,talk_help_rect;
/**/
extern char title_string[50];
extern unsigned char store_monst_type;
extern short store_m_num;
extern RECT dummy_rect;
extern short strnum1,strnum2,oldstrnum1,oldstrnum2;
extern short store_talk_face_pic,cur_town_talk_loaded;

extern short store_shop_items[30];
extern short store_shop_costs[30];
extern short store_shop_min,store_shop_max,store_pre_shop_mode,store_cost_mult;
extern char store_store_name[256];
extern RECT bottom_help_rects[4];
extern RECT shop_frame;
extern RECT shop_done_rect; /**/

extern char *heal_types[9];
extern short heal_costs[9];
extern long cost_mult[7];
extern short cur_display_mode;

extern short terrain_pic[256]; 

extern short store_scen_page_on,store_num_scen;

#define ND	15
#define	NI	500
#define	NL	100
#define	NUM_DLOG_B		53

extern char text_long_str[10][256];
extern char text_short_str[140][35];
extern char labels[NL][25];
extern Boolean label_taken[NL];

extern HWND edit_box, talk_edit_box;
extern HWND store_edit_parent; // kludgy

extern HDC dlg_force_dc; // save HDCs when dealing with dlogs

extern short store_free_slot,store_dlog_num;
extern HWND store_parent;

extern short available_dlog_buttons[NUM_DLOG_B];
extern short button_type[150];
extern char *button_strs[150];

extern short button_left_adj[150];
extern char button_def_key[150];
extern short button_ul_x[15];
extern short button_ul_y[15];
extern short button_width[15];
extern short button_height[15];

extern DLGPROC d_proc;
extern WNDPROC edit_proc;
extern WNDPROC old_edit_proc;

//#define	NUM_SOUNDS	100

extern HGLOBAL sound_handles[NUM_SOUNDS];
extern char * snds[NUM_SOUNDS];

extern short last_played;
extern short store_last_sound_played;

//extern bool always_asynch[100];
//extern Boolean load_when_play[100];

extern short can_ignore[100];

extern short num_devs;
extern Boolean sounds_fucked ;
extern long intro_music_start_time;

extern HWND	the_dialog;
extern HBITMAP spec_scen_g;

extern Boolean loaded_yet , got_nagged ,ae_loading ;
extern Boolean cur_scen_is_win ;

extern void print_write_position ();

extern char last_load_file[63];
extern char szFileName [128];
extern char szTitleName [128];
extern OPENFILENAME ofn;
extern OFSTRUCT save_dir,save_dir2;
extern SCROLLINFO lpsi;
extern MSG msg;
extern BITMAP bitmap_info;

// Trying this to fix bug. Hope it works.
extern 	tiny_tr_type tiny_t;
extern 	ave_tr_type ave_t;

extern outdoor_record_type dummy_out;////
extern town_record_type dummy_town;
extern short jl;

extern HBITMAP bg_bitmap[14];
extern HBRUSH checker_brush;
extern HBITMAP checker_bitmap,bw_bitmap;
extern HBITMAP startup_button_orig,startup_button_g,anim_mess ;

extern Boolean done_fancy_startup_once ;

extern short terrain_there[9][9]; 
extern RECT win_from_rects[6];
extern RECT win_to_rects[6];

// 0 - title  1 - button  2 - credits  3 - base button
extern RECT startup_from[4];
extern RECT trim_rects[8];
extern RECT	top_left_rec;

extern char combat_graphics[5];
extern short debug_nums[6];
extern short remember_tiny_text; // Remembers what's in the tiny text-bar, to prevent redrawing.

extern char light_area[13][13];
extern char unexplored_area[13][13];

// Declare the graphics
extern HBITMAP mixed_gworld, pc_stats_gworld, item_stats_gworld, text_area_gworld;
extern HBITMAP terrain_screen_gworld,text_bar_gworld,orig_text_bar_gworld,buttons_gworld;
extern HBITMAP items_gworld,tiny_obj_gworld,fields_gworld;
extern HBITMAP dlg_buttons_gworld,missiles_gworld,dlogpics_gworld,small_temp_gworld;
extern HBITMAP talking_portraits_gworld, frills_gworld, ter_anim_gworld,monsters_gworld[10], terrains_gworld[6];
extern HBITMAP displayed_picture;

extern HBITMAP dialog_pattern_gworld,pattern_gworld,status_pattern_gworld;

// Startup graphics, will die when play starts
extern HBITMAP startup_gworld;

extern HRGN clip_region;
extern HBRUSH gbrush;
extern HPEN gpen;

extern Boolean has_run_anim ;

extern RECT main_win_rect;
extern RECT main_win2_source_rect;
extern RECT main_win2_rect ;

extern RECT tiny_world_1_source_rect,tiny_world_1_rect;

extern RECT share_mess_source_rect,
	share_mess_rect;
extern RECT start_buttons_source_rect,
	start_buttons_rect; /**/

// Variables to store trim. Makes game faster, but wastes 15K. We'll see how it works...
extern char out_trim[96][96];
extern char town_trim[64][64];

// Array to store which spots have been seen. Time-saver for drawing fields
extern char spot_seen[9][9];


extern short startup_anim_pos;

extern Boolean supressing_some_spaces ;
extern location ok_space[4];

extern char combat_string[100];

extern BOOL pal_ok ;
extern Boolean syscolors_stored ;
extern int elements[5];
extern COLORREF store_element_colors[5];
extern short dlog_pat_placed;
extern short current_pattern;

extern RECT boat_rects[4]; /**/
extern Boolean gave_no_g_error ;

extern unsigned char m_pic_sheet[200];
extern unsigned char m_pic_index[200];
extern unsigned char m_pic_index_x[200];
extern unsigned char m_pic_index_y[200];

extern short mage_spell_pos,priest_spell_pos,skill_pos;
extern pc_record_type *store_pc;
extern creature_data_type *store_m;
extern short store_trait_mode,store_item_pc,store_pc_num;
extern item_record_type store_i;

extern Boolean full_roster ;

extern location source_locs[6];

// Displaying string vars
extern short store_str1a;
extern short store_str1b;
extern short store_str2a;
extern short store_str2b;
extern short store_which_string_dlog;
extern short store_page_on,store_num_i;
extern short store_str_label_1,store_str_label_2,store_str_label_1b,store_str_label_2b;

// Misc dialog vars
extern short store_display_mode,store_displayed_item,position,cur_entry,num_entries,store_help_mode;
extern creature_data_type hold_m;

////
extern Boolean equippable[26];
extern short num_hands_to_use[26];
extern short num_that_can_equip[26];

// For following, if an item of type n is equipped, no other items of type n can be equipped,
// if n > 0
extern short excluding_types[26];

extern short selected,item_max;

extern short first_item_shown,store_get_mode,current_getting_pc,store_pcnum,total_items_gettable; // these 5 used for get items dialog
extern short item_array[130]; // NUM_TOWN_ITEMS + a bit

extern short answer_given;

extern char *store_str;
extern short store_dnum;

extern HWND test_dlog3;
extern HWND store_focus;

extern short being_created;
extern short procinst_exists[18];

extern char terrain_blocked[256];

extern short short_can_see();
extern Boolean combat_pt_in_light();
extern location obs_sec;
extern location which_party_sec;

extern location light_locs[40];
extern short num_lights;
extern char d_s[60];

extern short charm_odds[20];	

extern creature_start_type null_start_type;

extern short monsters_faces[190];

extern HRGN oval_region,dark_mask_region,temp_rect_rgn;

extern short skill_cost[20];
extern short skill_g_cost[20];
extern short skill_bonus[21];

extern short spell_level[62];
extern short spell_cost[2][62];
extern const char *mage_s_name[];
		
extern char *priest_s_name[];
extern 	char *alch_names[];
extern 	char *alch_names_short[];
extern short spell_w_cast[2][62];
// 0 - everywhere 1 - combat only 2 - town only 3 - town & outdoor only 4 - town & combat only  5 - outdoor only
extern Boolean get_mage[30];
extern Boolean get_priest[30];
extern short combat_percent[20];

extern short town_spell,who_cast,which_pc_displayed;
extern Boolean spell_button_active[90];

extern char talk_edit_string[40];

extern char c_line[60];

// Variables for spell selection
extern 	short store_situation,store_last_target_darkened,on_which_spell_page;
extern 	short store_last_cast_mage ,store_last_cast_priest ;
extern 	short buttons_on[38];
		// buttons_on determines which buttons can be hit when on the second spell page
extern 	short spell_index[38];
		// Says which buttons hit which spells on second spell page, 90 means no button
extern 	Boolean can_choose_caster;

// Variables for spending xp
extern 	Boolean talk_done ;
extern 	long val_for_text;
extern 	Boolean keep_change ;
extern 	short store_skills[20],store_h,store_sp,i,store_skp,which_skill;
extern 	long store_g;
extern 	short store_train_mode,store_train_pc;
	
extern HBITMAP pcs_gworld;

// Dialog vars
extern short store_mage_store ;
extern short store_priest_store ;
extern short store_store_target;
extern short store_graphic_pc_num ;
extern short store_graphic_mode ;
extern short store_pc_graphic;

extern pc_array adven;

extern Boolean can_draw_pcs ;
extern short store_item_spell_level;
extern Boolean special_in_progress ;

extern item_record_type	null_item;
extern short spec_str_offset[3];
extern short current_pc_picked_in_spec_enc ; // pc that's been selected, -1 if none
extern location store_special_loc;
extern 	short boom_gr[8] ;
extern  short skill_max[20];

extern 	short abil_chart[200];
							
extern RECT startup_button[6];

extern short buf_pointer , lines_to_print, num_added_since_stop ;
extern char store_string[256];
extern char store_string2[256];
extern short start_print_point;
extern short mark_where_printing_long;
extern Boolean printing_long ;
extern char c_str[256];
extern Boolean save_mess_given ;

extern RECT status_panel_clip_rect,item_panel_clip_rect;

extern RECT item_buttons_from[7]; /**/

extern short store_mode;
extern Boolean string_added ;
extern short store_text_x , store_text_y ;

extern short current_item_button[6];
extern short pc_button_state[6];

extern char *m_mage_sp[];
extern char *m_priest_sp[];

// extra devices for maps
extern 	HBRUSH hbrush[6];
extern 	HPEN hpen[6];

extern RECT store_map_window_rect;

extern Boolean need_map_full_refresh ,forcing_map_button_redraw ;
extern HBITMAP map_gworld;
extern HBRUSH	bg[14];
extern HBRUSH map_brush[25];
extern HBITMAP map_bitmap[25];

extern unsigned char map_pats[256];// 250
extern unsigned char anim_map_pats[18];
						
extern location town_map_adj ;
extern short town_force,store_min,store_max,store_shop;
extern location town_force_loc;
extern Boolean shop_button_active[12];
extern RECT map_title_rect; /**/
extern RECT map_bar_rect;
extern unsigned char map_graphic_placed[8][64]; // keeps track of what's been filled on map
extern Boolean kludge_force_full_refresh ;

extern word_rect_type preset_words[9];
extern word_rect_type store_words[50];

//Timer Special happened ?
extern BOOL timed_special_happened;
extern BOOL ghost_mode;
extern unsigned char queue_position;

#endif
