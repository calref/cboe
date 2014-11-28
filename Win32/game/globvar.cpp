#include "globvar.h"

RECT bottom_buttons[7];
RECT town_buttons[10];
RECT combat_buttons[9];
RECT world_screen = {23, 23, 274,346};
RECT item_screen_button_rects[9] =
{
	{11,126,28,140},{40,126,57,140},{69,126,86,140},
	{98,126,115,140},{127,126,144,140},{156,126,173,140},
	{176,126,211,141},{213,126,248,141},{251,127,267,139}
};
RECT border_rect[4] =
{
	{5, 5, 283, 15}, {5, 5, 15, 355},
	{5, 345, 283, 355}, {273, 5, 283, 355}
};
RECT medium_buttons[4] =
{
	{190, 383,225,401}, {190, 402, 225, 420},
	{227, 383, 263, 401}, {227, 402, 263,420}
};
RECT startup_top;
RECT item_buttons[8][6]; 	// name, use, give, drip, info, sell/id
RECT pc_buttons[6][5];		// name, hp, sp, info, trade
pending_special_type special_queue[20];
Boolean end_scenario = false;
// For menu spell casting, some info needs to be stored up here.
short mage_need_select[62] = {0,0,1,1,0,0,0,0,0,0, 0,0,0,0,1,0,0,0,0,0, 0,1,0,0,0,0,0,0,0,1,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,1,0,0,0,0};
short priest_need_select[62] = {1,1,1,0,0,1,1,0,0,0, 1,1,0,0,0,0,0,0,0,1, 1,0,0,0,1,0,0,1,1,0,
	0,0,0,1,0,1,1,0, 0,1,2,0,0,0,0,0, 0,1,0,2,0,0,0,0, 0,0,2,0,0,0,0,0};
// 0 - no select  1 - active only  2 - any existing
Boolean item_area_button_active[8][6];
Boolean pc_area_button_active[6][5];
short item_bottom_button_active[9] = {0,0,0,0,0, 0,1,1,1};
RECT pc_help_button,pc_area_rect,item_area_rect;
short num_out_moves = 0;
short store_drop_item;
short current_switch = 6;
out_wandering_type store_wandering_special;
short store_shop_type;
short debug_ok = 0;
short store_selling_values[8] = {0,0,0,0,0,0,0,0};
char const *dir_string[] = {"North", "NorthEast", "East", "SouthEast", "South", "SouthWest", "West", "NorthWest"};
char get_new_terrain();
item_record_type start_items[6] =
{
	{1,4, 0,1,0,0,1,0, 45,0,0,0,0,0, 2, 7,0, location(),"Bronze Knife","Knife",0,1,0,0},
	{12,1,1,0,0,0,0,0, 65,0,0,0,0,0, 2, 20,0, location(),"Crude Buckler","Buckler",0,1,0,0},
	{4,0,0,0,0,0,0,0, 10,0,0,0,0,0, 15, 20,0, location(),"Cavewood Bow","Bow",0,1,0,0},
	{5,12,0,0,0,12,0,0, 47,0,0,0,0,0, 1, 1,0, location(),"Arrows","Arrows",0,1,0,0},
	{2,9,0,0,0,0,3,0, 4,0,0,0,0,0, 10, 30,0, location(),"Stone Spear","Spear",0,1,0,0},
	{14,1,0,0,0,0,0,0, 66,0,0,0,0,0, 6, 15,0, location(),"Leather Helm","Helm",0,1,0,0}
};
// 0 - whole area, 1 - active area 2 - graphic 3 - item name
// 4 - item cost 5 - item extra str  6 - item help button
RECT shopping_rects[8][7];
unsigned char out[96][96];
unsigned char out_e[96][96];
unsigned char misc_i[64][64],sfx[64][64];
/* Mac stuff globals */
HWND text_sbar = NULL,item_sbar = NULL,shop_sbar = NULL;
RECT sbar_rect = {545,284,562,421};
RECT shop_sbar_rect = {258,67,274,357};
RECT item_sbar_rect = {545,146,562,254};
int dialog_answer;
Boolean All_Done = false, dialog_not_toast = false;
Boolean in_startup_mode = true;
//Boolean play_sounds;
Boolean spell_forced = false,startup_loaded = false;
Boolean save_maps = true,party_in_memory = false,in_scen_debug = false;
Boolean window_in_front = false;
Boolean belt_present = false;
Boolean game_run_before = true;
Boolean give_intro_hint = true;
short on_spell_menu[2][62];
short on_monst_menu[256];
short current_cursor = 120;
HCURSOR arrow_curs[3][3], sword_curs, key_curs, target_curs,talk_curs,look_curs;
/* Adventure globals */
party_record_type party;
outdoor_record_type outdoors[2][2];
current_town_type c_town;
big_tr_type t_d;
stored_items_list t_i;
setup_save_type setup_save;
location monster_targs[T_M];
/* Display globals */
short combat_posing_monster = -1, current_working_monster = -1; // 0-5 PC 100 + x - monster x
Boolean fast_bang = false;
short spec_item_array[60];
short overall_mode = MODE_STARTUP,current_spell_range;
Boolean first_update = true,anim_onscreen = false,frills_on = true,suppress_stat_screen = false;
short stat_window = 0;
Boolean monsters_going = false,boom_anim_active = false;
short give_delays = 0;
Boolean modeless_exists[18] = {false,false,false,false,false,false,
	false,false,false,false,false,false,
	false,false,false,false,false,false};
HWND modeless_dialogs[18] = {NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
	NULL,	NULL,	NULL,	NULL,	NULL,	NULL};
short town_size[3] = {64,48,32};
short ulx = 0, uly = 0;
short display_mode = 0; // 0 - center 1- ul 2 - ur 3 - dl 4 - dr 5 - small win
short current_ground = 0,stat_screen_mode = 0;
long anim_ticks = 0;
// Spell casting globals
short store_mage = 0, store_priest = 0;
short store_mage_lev = 0, store_priest_lev = 0;
short store_spell_target = 6,pc_casting;
short pc_last_cast[2][6] = {{1,1,1,1,1,1},{1,1,1,1,1,1}};
int num_targets_left = 0;
location spell_targets[8];
long store_mouse;
/* Combat globals */
short which_combat_type,town_type;
location center;
unsigned char combat_terrain[64][64];
location pc_pos[6];
short current_pc;
short combat_active_pc;
effect_pat_type current_pat;
short monst_target[T_M]; // 0-5 target that pc   6 - no target  100 + x - target monster x
short spell_caster, missile_firer,current_monst_tactic;
short store_current_pc = 0;
stored_items_list stored_items[3];
stored_outdoor_maps_type o_maps;
// Special stuff booleans
Boolean web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
Boolean sleep_field;
/* Windoze stuff globals */
Boolean cursor_shown = true;
HWND mainPtr;
HFONT font,fantasy_font,small_bold_font,italic_font,underline_font,bold_font,tiny_font;
HDC main_dc,main_dc2,main_dc3;
HINSTANCE store_hInstance;
HACCEL accel;
BOOL event_handled;
scenario_data_type scenario;
talking_record_type talking;
char scen_strs2[110][256];
stored_town_maps_type town_maps;
char szWinName[] = "BoE Dialogs";
char szAppName[] = "BoE for Win32";
char file_path_name[256];
Boolean block_erase = false;
/* dynamically allocated */
piles_of_stuff_dumping_type *data_store;
piles_of_stuff_dumping_type2 *data_store2 = NULL;
piles_of_stuff_dumping_type3 *data_store3;
piles_of_stuff_dumping_type4 *data_store4;
piles_of_stuff_dumping_type5 *data_store5;
scen_item_data_type *scen_item_list;
scen_header_type *scen_headers = NULL;
/* end of dynamically allocated */
char create_line[60];
short spell_being_cast;
short missile_inv_slot, ammo_inv_slot;
short force_wall_position = 10; //  10 -> no force wall
Boolean processing_fields = true;
short futzing;
unsigned char store_sum_monst;
short store_sum_monst_cost;
location out_start_loc = location(20,23);
short hit_chance[51] = {20,30,40,45,50,55,60,65,69,73,
	77,81,84,87,90,92,94,96,97,98,99
	,99,99,99,99,99,99,99,99,99,99
	,99,99,99,99,99,99,99,99,99,99,
	99,99,99,99,99,99,99,99,99,99};
short s_cost[2][62] = {{1,1,1,1,1,2,50,2,1,3, 2,3,2,2,2,2,4,4,2,6, 3,3,5,3,3,5,6,4,6,4,
		4,5,4,8,30,-1,8,6, 5,8,8,6,9,10,6,6, 7,6,8,7,12,10,12,20, 12,8,20,10,14,10,50,10},
	{1,1,1,2,1,1,3,5,50,1, 2,2,2,2,3,5,8,6,4,2, 3,4,3,3,3,10,5,3,4,6,
		5,5,5,15,6,5,5,8, 6,7,25,8,10,12,12,6, 8,7,8,8,14,17,8,7, 10,10,35,10,12,12,30,10}};
short mage_range[80] = {0,6,0,0,7,7,0,14,8,0, 6,8,7,10,0,8,3,8,10,6, 0,0,12,0,10,12,4,10,8,0,
	8,12,12,0,10,4,8,8, 0,0,14,0,2,4,10,12, 8,12,6,8,5,8,4,0, 0,0,8,0,4,2,4,6
	,10,8,8,12,8,10,10,10, 10,10,10,10,10,10,10,10,10,10};
short priest_range[62] = {0,0,0,8,0,0,0,0,0,10, 0,0,10,0,6,4,0,6,6,8, 0,0,8,0,10,0,8,0,0,8,
	0,10,8,0,6,0,0,0, 0,0,0,9,0,4,0,8, 0,0,10,0,4,8,0,8, 0,4,0,12,0,10,0,0};
short monst_mage_cost[27] = {1,1,1,1,2, 2,2,2,2,4, 2,4,4,3,4, 4,4,5,5,5, 5,6,6,6,7, 7,7};
short monst_mage_area_effect[27] = {0,0,0,0,0, 0,0,0,1,0, 1,1,0,1,0, 0,0,0,1,0, 1,0,0,0,0, 0,0};
short monst_priest_cost[26] = {1,1,1,1,2, 2,2,4,2,3, 3,3,4,4,4, 5,5,5,10,6, 6,10,8,8,8, 8};
short monst_priest_area_effect[26] = {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 1};
char const *d_string[] =
{
	"North",
	"NorthEast",
	"East",
	"SouthEast",
	"South",
	"SouthWest",
	"West",
	"NorthWest"
};
short pc_marked_damage[6];
short monst_marked_damage[T_M];
location hor_vert_place[14] =
	{
		location(0,0),location(-1,1),location(1,1),location(-2,2),location(0,2),
		location(2,2),location(0,1),location(-1,2),location(1,2),location(-1,3),
		location(1,3),location(0,3),location(0,4),location(0,5)
	};
location diag_place[14] =
	{
		location(0,0),location(-1,0),location(0,1),location(-1,1),location(-2,0),
		location(0,2),location(-2,1),location(-1,2),location(-2,2),location(-3,2),
		location(-2,3),location(-3,3),location(-4,3),location(-3,4)
	};
effect_pat_type single = {{{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}}};
effect_pat_type t = {{{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0},
		{0,0,0,1,1,1,0,0,0},
		{0,0,0,0,1,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}}};
effect_pat_type small_square = {{{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,1,1,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}}};
effect_pat_type square = {{{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,1,1,1,0,0,0},
		{0,0,0,1,1,1,0,0,0},
		{0,0,0,1,1,1,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}}};
effect_pat_type radius2 =
{{{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,1,1,1,0,0,0},
		{0,0,1,1,1,1,1,0,0},
		{0,0,1,1,1,1,1,0,0},
		{0,0,1,1,1,1,1,0,0},
		{0,0,0,1,1,1,0,0,0},
		{0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0}}};
effect_pat_type radius3 = {{{0,0,0,0,0,0,0,0,0},
		{0,0,0,1,1,1,0,0,0},
		{0,0,1,1,1,1,1,0,0},
		{0,1,1,1,1,1,1,1,0},
		{0,1,1,1,1,1,1,1,0},
		{0,1,1,1,1,1,1,1,0},
		{0,0,1,1,1,1,1,0,0},
		{0,0,0,1,1,1,0,0,0},
		{0,0,0,0,0,0,0,0,0}}};
effect_pat_type field[8] = {{{{0,0,0,0,1,1,0,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,0,1,1,0,0,0}}},
	{{{0,0,0,0,0,0,0,0,1},
			{0,0,0,0,0,0,0,1,1},
			{0,0,0,0,0,0,1,1,0},
			{0,0,0,0,0,1,1,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,1,1,0,0,0,0,0},
			{0,1,1,0,0,0,0,0,0},
			{1,1,0,0,0,0,0,0,0}}},
	{{{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{1,1,1,1,1,1,1,1,1},
			{1,1,1,1,1,1,1,1,1},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0}}},
	{{{1,0,0,0,0,0,0,0,0},
			{1,1,0,0,0,0,0,0,0},
			{0,1,1,0,0,0,0,0,0},
			{0,0,1,1,0,0,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,0,0,1,1,0,0},
			{0,0,0,0,0,0,1,1,0},
			{0,0,0,0,0,0,0,1,1}}},
	{{{0,0,0,1,1,0,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,0,1,1,0,0,0,0}}},
	{{{0,0,0,0,0,0,0,1,1},
			{0,0,0,0,0,0,1,1,0},
			{0,0,0,0,0,1,1,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,1,1,0,0,0,0,0},
			{0,1,1,0,0,0,0,0,0},
			{1,1,0,0,0,0,0,0,0},
			{1,0,0,0,0,0,0,0,0}}},
	{{{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{1,1,1,1,1,1,1,1,1},
			{1,1,1,1,1,1,1,1,1},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0,0}}},
	{{{1,1,0,0,0,0,0,0,0},
			{0,1,1,0,0,0,0,0,0},
			{0,0,1,1,0,0,0,0,0},
			{0,0,0,1,1,0,0,0,0},
			{0,0,0,0,1,1,0,0,0},
			{0,0,0,0,0,1,1,0,0},
			{0,0,0,0,0,0,1,1,0},
			{0,0,0,0,0,0,0,1,1},
			{0,0,0,0,0,0,0,0,1}}}};
short last_attacked[6],pc_dir[6],pc_parry[6],pc_moves[6];
Boolean center_on_monst;
short store_sign_mode;
short store_tip_page_on = 0;
// Talking vars
short store_pre_talk_mode,store_personality,shop_identify_cost;
HBITMAP talk_gworld ;
Boolean talk_end_forced;
char old_str1[256];
char old_str2[256];
char one_back1[256];
char one_back2[256];
RECT talk_area_rect = {5,5,284,420}, word_place_rect = {7,44,257,372},talk_help_rect = {254,5,272,21};
/**/
char title_string[50];
unsigned char store_monst_type;
short store_m_num;
RECT dummy_rect = {0,0,0,0};
short strnum1,strnum2,oldstrnum1,oldstrnum2;
short store_talk_face_pic,cur_town_talk_loaded = -1;
// Shopping vars
// 1 - 499 ... regular items
// 500 alchemy
// 600-620 ... food
// 700+ i - that healing
// 800 + - mage spells
// 900 + - priest spells
// n000 + i - magic store n item i
short store_shop_items[30];
short store_shop_costs[30];
// talk_area_rect and talk_help_rect used for this too
short store_shop_min,store_shop_max,store_pre_shop_mode,store_cost_mult;
char store_store_name[256];
// 0 - whole area, 1 - active area 2 - graphic 3 - item name
// 4 - item cost 5 - item extra str  6 - item help button
RECT bottom_help_rects[4] = {{6,356,250,368},{6,374,270,386},{6,386,250,398},{6,398,250,410}};
RECT shop_frame = {10,62,269,352};
RECT shop_done_rect = {212,388,275,411}; /**/
char const *heal_types[] = {"Heal Damage","Cure Poison","Cure Disease","Cure Paralysis",
	"Uncurse Items","Cure Stoned Character","Raise Dead","Resurrection","Cure Dumbfounding"};
short heal_costs[9] = {50,30,80,100,250,500,1000,3000,100};
long cost_mult[7] = {5,7,10,13,16,20,25};
short cur_display_mode;
short terrain_pic[256];
short store_scen_page_on,store_num_scen;
#define ND	15
#define	NI	500
#define	NL	100
#define	NUM_DLOG_B		53
char text_long_str[10][256];
char text_short_str[140][35];
char labels[NL][25];
Boolean label_taken[NL];
HWND edit_box, talk_edit_box = NULL ;
HWND store_edit_parent; // kludgy
HDC dlg_force_dc ; // save HDCs when dealing with dlogs
short store_free_slot,store_dlog_num;
HWND store_parent;
short available_dlog_buttons[NUM_DLOG_B] = {0,63,64,65,1,4,5,8,
	128,
	9,
	10, // 10
	11,12,13,
	14,15,16,17,29, 51,
	60,61,62, // 20
	66,69,70, 71,72,73,74,79,
	80,83,86,87,88, 91,92,93,99,100,
	101,102,104, 129,130,131,132,133,134,135,136,137};
short button_type[150] = {1,1,4,5,1,1,0,0,1,1,
	1,1,1,1,1,1,1,1,8,8,
	9,9,9,1,1,2,1,6,7,1,
	1,12,1,1,2,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,1,1,1,2,1,1,1,2,2, // 50
	1,1,1,1,1,1,2,3,1,1,
	1,1,1,1,2,2,2,2,2,1,
	1,1,1,1,2,2,1,1,1,2,
	0,1,1,1,14,13,12,12,12,1,
	1,1,1,2,1,2,2,2,2,1, // 100
	2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,1,1,
	1,1,1,1,1,1,1,1,0,0,
	0,0,0,0,0,0,0,0,0,0};
char const *button_strs[150] = {"Done ","Ask"," "," ","Keep", "Cancel","+","-","Buy","Leave",
	"Get","1","2","3","4","5","6","Cast"," "," ",
	" "," "," ","Buy","Sell","Other Spells","Buy x10"," "," ","Save",
	"Race","Train","Items","Spells","Heal Party","1","2","3","4","5",
	"6","7","8","9","10","11","12","13","14","15",
	/*50*/  "16","Take","Create","Delete","Race/Special","Skill","Name","Graphic","Bash Door","Pick Lock",
	"Leave","Steal","Attack","OK","Yes","No","Step In"," ","Record","Climb",
	"Flee","Onward","Answer","Drink","Approach","Mage Spells","Priest Spells","Advantages","New Game","Land",
	"Under","Restore","Restart","Quit","Save First","Just Quit","Rest","Read","Pull","Alchemy",
	"17","Push","Pray","Wait","","","Delete","Graphic","Create","Give",
	/*100*/		"Destroy","Pay","Free","Next Tip","Touch", "Select Icon","Create/Edit","Clear Special","Edit Abilities","Choose",
	"Go Back","Create New","General","One Shots","Affect PCs","If-Thens","Town Specs","Out Specs","Advanced","Weapon Abil",
	"General Abil.","NonSpell Use","Spell Usable","Reagents","Missiles","Abilities","Pick Picture","Animated","Enter","Burn",
	"Insert","Remove","Accept","Refuse","Open","Close","Sit","Stand","","",
	"18","19","20","Invisible!","","","","","",""};
short button_left_adj[150] = {0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,  // 50
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0,
	0,0,0,0,0, 0,0,0,0,0};
char button_def_key[150] = {0,0,20,21,'k', 24,0,0,0,0,
	'g','1','2','3','4', '5','6',0,0,0,
	0,0,0,0,0,' ',0,22,23,0,
	0,0,0,0,0,'1','2','3','4','5',
	'6','7','8','9','a', 'b','c','d','e','f',
	'g',0,0,0,0,0,0,0,0,0,
	0,0,0,0,'y','n',0,'?','r',0,
	0,0,0,0,0,0,0,0,0, 0,
	0,0,0,0,0,0,0,0,0,0,
	'g',0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};
// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
// 25-30  ctrl 1-6  31 - return
short button_ul_x[15] = {0,46,0,126,0, 0,126,126,126,138, 166,0,0,126,172};
short button_ul_y[15] = {0,0,132,23,46, 69,46,69,36,36, 36,23,92,92,0};
short button_width[15] = {23,63,102,16,63, 63,63,63,6,14, 14,63,63,63,30};
short button_height[15] = {23,23,23,13,23, 23,23,23,6,10,10,23,40,40,30};
BOOL CALLBACK dummy_dialog_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK fresh_edit_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
DLGPROC d_proc;
WNDPROC edit_proc;
WNDPROC old_edit_proc;
//HMODULE hModule;
long intro_music_start_time = -1;
HBITMAP spec_scen_g ;
typedef struct {
	char expl[96][96];
}	out_info_type;
Boolean loaded_yet = false;
Boolean cur_scen_is_win = true;
void print_write_position ();
char szFileName [128] = "blades.sav";
char szTitleName [128] = "blades.sav";
OPENFILENAME ofn;
SCROLLINFO lpsi;
MSG msg;
BITMAP bitmap_info;
// Trying this to fix bug. Hope it works.
tiny_tr_type tiny_t;
ave_tr_type ave_t;
outdoor_record_type dummy_out;////
town_record_type dummy_town;
HBITMAP bg_bitmap[14];
HBRUSH checker_brush ;
HBITMAP checker_bitmap ,bw_bitmap;
HBITMAP startup_button_orig,startup_button_g,anim_mess ;
Boolean done_fancy_startup_once = false;
short terrain_there[9][9]; // this is an optimization variabel. Keeps track of what terrain
// is in the terrain spot, so things don't get redrawn.
// 0 - 299 just terrain graphic in place
// 300 - blackness
// -1 - nothign worth saving
// 0 - terrain   1 - buttons   2 - pc stats
// 3 - item stats   4 - text bar   5 - text area (not right)
RECT win_from_rects[6] = {{0,0,279,351},{0,0,258,37},{0,0,288,115},{0,0,288,143},{0,0,279,21},{0,0,288,0}};
RECT win_to_rects[6] = {{5,5,284,356},{5,383,263,420},{0,0,271,116},{0,0,271,144},{5,358,284,379},{0,0,256,138}};
// 0 - title  1 - button  2 - credits  3 - base button
RECT startup_from[4] = {{0,0,602,274},{0,274,301,322},{301,0,579,67},{301,274,341,314}}; ////
RECT trim_rects[8] = {{0,0,28,5},{0,31,28,36},{0,0,5,36},{24,0,28,36},
	{0,0,5,5},{24,0,28,5},{24,31,28,36},{0,31,5,36}};  /**/
short remember_tiny_text = 300; // Remembers what's in the tiny text-bar, to prevent redrawing.
// 50 indicates area name, other number indicates which-rect.
// Add 200 if last mess. was in town
char light_area[13][13];
char unexplored_area[13][13];
// Declare the graphics
HBITMAP mixed_gworld, pc_stats_gworld, item_stats_gworld, text_area_gworld;
HBITMAP terrain_screen_gworld,text_bar_gworld,orig_text_bar_gworld,buttons_gworld;
HBITMAP items_gworld,tiny_obj_gworld,fields_gworld;
HBITMAP dlg_buttons_gworld,missiles_gworld,dlogpics_gworld,small_temp_gworld;
HBITMAP dialog_pattern_gworld,pattern_gworld,status_pattern_gworld;
HBITMAP talking_portraits_gworld, frills_gworld, ter_anim_gworld, monsters_gworld[10], terrains_gworld[6];
HBITMAP displayed_picture = NULL;
// Startup graphics, will die when play starts
HBITMAP startup_gworld;
HRGN clip_region;
HBRUSH gbrush;
HPEN gpen;
// Variables to store trim. Makes game faster, but wastes 15K. We'll see how it works...
char out_trim[96][96];
char town_trim[64][64];
// Array to store which spots have been seen. Time-saver for drawing fields
char spot_seen[9][9];
short startup_anim_pos = 43;
Boolean supressing_some_spaces = false;
location ok_space[4];
char combat_string[100];
short dlog_pat_placed = 0;
short current_pattern = -1;
RECT boat_rects[4] = {{0,0,28,36}, {28,0,56,36},{56,0,84,36},{84,0,112,36}}; /**/
Boolean gave_no_g_error = false;
unsigned char m_pic_sheet[200] = {////
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,2,
	2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,3,3,3,3,3,3,
	3,3,3,3,3,3,3,3,3,3,
	3,3,3,3,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,5,5,5,5, //100
	6,6,6,6,6,6,6,6,6,6,
	6,6,6,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,
	8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,9,9,9,9,9,9,
	9,9,9,9,9,9,9,9,9,9,
	9,9,9,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0};

unsigned char m_pic_index[200] = {////
	1,2,3,4,5,6,7,8,9,10,
	11,12,13,14,15,16,17,18,19,20,
	21,22,23,24,25, 26,27,28,29,30,
	31,32,33,34,35, 36,37,38,39,40,
	41,42,43,44,46,47,48,49,50,51,
	53,55,57,59,60,61,62,63,64,65,
	66,67,68,69,70, 71,72,73,74,75,
	76,77,78,79,81, 82,83,85,86,87,
	88,89,90,91,92, 93,94,95,96,97,
	98,99,100,101,102, 103,104,105,106,107,
	108,109,111,112,113,  114,116,117,118,119, //100
	120,122,123,125,127, 128,129,130,131,135,
	136,137,139,140,141,142,143,144,145,146,
	147,148,149,150,151,152,153,154,155,159,
	160,164,166,168,170,171,172,173,174,175,
	176,177,178,179,180,181,182,183,184,185,
	186,187,188,189,190,191,192,193,194,195,
	196,197,198,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0};
unsigned char m_pic_index_x[200] = {
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,2,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,2,1,1,1,2,1,1,1,1, // 100
	2,1,1,1,1,1,1,1,2,1,
	1,2,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,2,1,
	2,2,2,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1};
unsigned char m_pic_index_y[200] = {
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,2,
	2,2,2,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,2,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,2,2,1,1,1,1,2,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,2,1,
	2,1,1,2,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1};
short mage_spell_pos = 0,priest_spell_pos = 0,skill_pos = 0;
pc_record_type *store_pc;
creature_data_type *store_m;
short store_trait_mode,store_item_pc,store_pc_num;
item_record_type store_i;
Boolean full_roster = false;
// Displaying string vars
short store_which_string_dlog;
short store_page_on,store_num_i;
short store_str_label_1,store_str_label_2,store_str_label_1b,store_str_label_2b;
// Misc dialog vars
short store_display_mode,store_displayed_item,position,cur_entry,num_entries,store_help_mode;
creature_data_type hold_m;
////
Boolean equippable[26] = {false,true,true,false,true, true,true,false,false,false,
	true,false,true,true,true, true,true,true,true,true,
	false,false,true,true,true,true};
short num_hands_to_use[26] = {0,1,2,0,0, 0,0,0,0,0 ,0,0,1,0,0, 0,1,0,0,0, 0,0,0,0,0, 0};
short num_that_can_equip[26] = {0,2,1,0,1, 1,1,0,0,0, 1,0,1,1,1, 1,1,1,2,1, 0,0,1,1,1, 1};
// For following, if an item of type n is equipped, no other items of type n can be equipped,
// if n > 0
short excluding_types[26] = {0,0,0,0,2, 1,1,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,2,1, 2};
short item_max = 0;
short first_item_shown,store_get_mode,current_getting_pc,store_pcnum,total_items_gettable; // these 5 used for get items dialog
short item_array[130]; // NUM_TOWN_ITEMS + a bit
char *store_str;
short store_dnum;
char terrain_blocked[256];/* = {0,0,0,0,0,5,5,1,5,5,
	1,5,5,1,5,5,1,5,5,5,
	5,5,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	3,3,3,3,3,3,3,3,3,3,
	3,3,3,0,3,0,0,0,0,0,
	0,3,0,0,3,2,2,4,2,0,
	0,0,4,0,0,0,3,4,0,0,
	0,5,5,0,0,4,0,0,0,0,
	5,5,1,5,5,5,5,0,4,0,
	5,4,5,5,5,5,4,5,5,1,
	5,5,5,5,0,4,0,5,5,5,
	5,4,5,5,1,5,5,5,5,0,
	4,0,5,5,5,5,5,4,4,4,
	0,0,2,0,4,4,4,4,4,4,
	4,4,4,4,5,4,4,4,0,0,
	0,0,0,0,4,4,4,4,4,4,
	4,4,4,4,0,4,0,2,4,4,
	4,0,4,0,4,0,0,0,4,4,
	0,0,4,4,4,4,4,4,4,0,
	0,4,0,4,4,0,4,2,2,2,
	2,2,2,2,2,2,2,2,5,5,
	2,2,0,0,0,0,0,2,2,2,
	2,2,2,2,2,0,0,0,0,0,
	0,0,4,4,4,3};*/
short short_can_see();
Boolean combat_pt_in_light();
location which_party_sec;
short charm_odds[20] = {90,90,85,80,78, 75,73,60,40,30, 20,10,5,2,1, 0,0,0,0,0};
//creature_start_type null_start_type = {0,0,location(80,80),1,0,0,0,0,0,0,0, 0,-1,-1,-1};
creature_start_type null_start_type = {0,0,location(80,80),1,0,0,0,-1,-1,0,0, 0,-1,-1,-1};
HRGN oval_region ,dark_mask_region,temp_rect_rgn;
short skill_cost[20] = {3,3,3,2,2,2, 1,2,2,6,
	5, 1,2,4,2,1, 4,2,5,0};
short skill_g_cost[20] = {50,50,50,40,40,40,30,50,40,250,
	250,25,100,200,30,20,100,80,0,0};
short skill_bonus[21] = {-3,-3,-2,-1,0,0,1,1,1,2,
	2,2,3,3,3,3,4,4,4,5,5};
short spell_level[62] = {1,1,1,1,1,1,1,1,1,1, 2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,3,3,
	4,4,4,4,4,4,4,4, 5,5,5,5,5,5,5,5, 6,6,6,6,6,6,6,6, 7,7,7,7,7,7,7,7};
short spell_cost[2][62] = {{1,1,1,1,1,2,50,2,1,3, 2,3,2,2,2,2,4,4,2,6, 3,3,5,3,3,5,6,4,6,4,
		4,5,4,8,30,-1,8,6, 5,8,8,6,9,10,6,6, 7,6,8,7,12,10,12,20, 12,8,20,10,14,10,50,10},
	{1,1,1,2,1,1,3,5,50,1, 2,2,2,2,3,5,8,6,4,2, 3,4,3,3,3,10,5,3,4,6,
		5,5,5,15,6,5,5,8, 6,7,25,8,10,12,12,6, 8,7,8,8,14,17,8,7, 10,10,35,10,12,12,30,10}};
const char *mage_s_name[] =
{
	"Light",
	"Spark",
	"Minor Haste",
	"Strength",
	"Scare",
	"Flame Cloud",
	"Identify",
	"Scry Monster",
	"Goo",
	"True Sight",
	"Minor Poison",
	"Flame",
	"Slow",
	"Dumbfound",
	"Envenom",
	"Stinking Cloud",
	"Summon Beast",
	"Conflagration",
	"Dispel Field",
	"Sleep Cloud",
	"Unlock",
	"Haste",
	"Fireball",
	"Long Light",
	"Fear",
	"Wall of Force",
	"Weak Summoning",
	"Flame Arrows",
	"Web",
	"Resist Magic",
	"Poison",
	"Ice Bolt",
	"Slow Group",
	"Magic Map",
	"Capture Soul",
	"Simulacrum",
	"Venom Arrows",
	"Wall of Ice",
	"Stealth",
	"Major Haste",
	"Fire Storm",
	"D. Barrier",
	"Fire Barrier",
	"Summoning",
	"Shockstorm",
	"Spray Fields",
	"Major Poison",
	"Group Fear",
	"Kill",
	"Paralysis",
	"Daemon",
	"Antimagic Cloud",
	"MindDuel",
	"Flight",
	"Shockwave",
	"M. Blessing",
	"Mass Paralysis",
	"Protection",
	"Major Summon",
	"Force Barrier",
	"Quickfire",
	"Death Arrows"};
char const *priest_s_name[] = {"Minor Bless","Minor Heal","Weaken Poison","Turn Undead","Location",
	"Sanctuary","Symbiosis","Minor Manna","Ritual - Sanctify","Stumble",
	"Bless","Cure Poison","Curse","Light","Wound",
	"Summon Spirit","Move Mountains","Charm Foe","Disease","Awaken",
	"Heal","Light Heal All","Holy Scourge","Detect Life","Cure Paralysis",
	"Manna","Forcefield","Cure Disease","Restore Mind","Smite",
	"Cure Party","Curse All","Dispel Undead","Remove Curse",
	"Sticks to Snakes","Martyr's Shield","Cleanse","Firewalk",
	"Bless Party","Major Heal","Raise Dead","Flamestrike",
	"Mass Sanctuary","Summon Host","Shatter","Dispel Fields",
	"Heal All","Revive","Hyperactivity","Destone",
	"Guardian","Mass Charm","Protective Circle","Pestilence",
	"Revive All","Ravage Spirit","Resurrect","Divine Thud",
	"Avatar","Wall of Blades","Word of Recall","Major Cleansing"};
char const *alch_names[] = {"Weak Curing Potion (1)","Weak Healing Potion (1)","Weak Poison (1)",
	"Weak Speed Potion (3)","Medium Poison (3)",
	"Medium Heal Potion (4)","Strong Curing (5)","Medium Speed Potion (5)",
	"Graymold Salve (7)","Weak Energy Potion (9)",
	"Potion of Clarity (9)","Strong Poison (10)","Strong Heal Potion (12)","Killer Poison (12)",
	"Resurrection Balm (9)","Medium Energy Ptn. (14)","Knowledge Brew (19)"	,
	"Strong Strength (10)","Bliss (16)","Strong Energy Ptn. (20)"
};
char const *alch_names_short[] = {"Weak Curing Potion","Weak Healing Potion","Weak Poison",
	"Weak Speed Potion","Medium Poison",
	"Medium Heal Potion","Strong Curing","Medium Speed Potion",
	"Graymold Salve","Weak Energy Potion",
	"Potion of Clarity","Strong Poison","Strong Heal Potion","Killer Poison",
	"Resurrection Bal","Medium Energy Ptn.","Knowledge Brew",
	"Strong Strength","Bliss","Strong Energy Ptn."
};
short spell_w_cast[2][62] = {{0,1,1,1,1,1,3,4,1,2, 1,1,1,1,1,1,4,1,4,1, 2,1,1,0,1,1,4,1,1,0,
		1,1,1,2,4,1,1,1, 2,1,1,2,4,4,1,1, 1,1,1,1,4,4,1,5, 1,4,1,4,4,4,4,1},
	{1,0,0,1,3,1,1,3,2,1, 1,0,1,0,1,4,2,1,1,0, 0,0,1,2,0,3,1,0,0,1,
		0,1,1,3,4,1,0,0, 1,0,3,1,1,4,2,4, 0,0,0,3,4,1,1,1, 0,1,3,1,4,1,5,0}};
// 0 - everywhere 1 - combat only 2 - town only 3 - town & outdoor only 4 - town & combat only  5 - outdoor only
short combat_percent[20] = {150,120,100,90,80,80,80,70,70,70,
	70,70,67,62,57,52,47,42,40,40};
short town_spell,who_cast,which_pc_displayed;
char c_line[60];
// Variables for spell selection
short store_situation,store_last_target_darkened,on_which_spell_page = 0;
short store_last_cast_mage = 6,store_last_cast_priest = 6;
short spell_index[38] = {38,39,40,41,42,43,44,45,90,90,46,47,48,49,50,51,52,53,90,90,
	54,55,56,57,58,59,60,61,90,90, 90,90,90,90,90,90,90,90};
// Says which buttons hit which spells on second spell page, 90 means no button
Boolean can_choose_caster;
// Variables for spending xp
short store_skills[20],store_h,store_sp,i,store_skp,which_skill;
long store_g;
short store_train_mode,store_train_pc;
HBITMAP pcs_gworld;
// Dialog vars
short store_mage_store ;
short store_priest_store ;
short store_store_target;
short store_graphic_pc_num ;
short store_graphic_mode ;
short store_pc_graphic;
pc_array adven;
Boolean can_draw_pcs = true;
short store_item_spell_level = 10;
Boolean special_in_progress = false;
short spec_str_offset[3] = {160,10,20};
short current_pc_picked_in_spec_enc = -1; // pc that's been selected, -1 if none
location store_special_loc;
short boom_gr[8] = {3,0,2,1,1,4,3,3};
short skill_max[20] = {20,20,20,20,20,20,20,20,20,7,
	7,20,20,10,20,20,20,20,20};
// 0 - everywhere 1 - combat only 2 - town only 3 - town & combat only  4 - can't use  5 - outdoor
// + 10 - mag. inept can use
short abil_chart[200] = {4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4, // 50
	4,4,4,4,4,4,4,4,4,4,
	13,0,0,0,3, 3,3,0,3,3,
	3,3,3,3,3, 0,0,0,0,3,
	13,3,3,5,0, 10,0,0,0,0,
	4,4,4,4,4,4,4,4,4,4, // 100
	1,1,1,1,1, 1,1,1,1,3,
	3,1,1,1,1, 1,1,1,1,3,
	1,2,2,1,1, 1,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4, // 150
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4};
RECT startup_button[6];
short buf_pointer = 30;
char store_string[256];
char store_string2[256];
short mark_where_printing_long;
Boolean printing_long = false;
RECT item_buttons_from[7] = {{0,12,14,24},{14,12,28,24},{28,12,42,24},{42,12,56,24},
	{0,24,30,36},{30,24,60,36},{0,36,30,48}}; /**/
Boolean string_added = false;
short store_text_x = 0, store_text_y = 0;
short current_item_button[6] = {-1,-1,-1,-1,-1,-1};
short pc_button_state[6] = {-1,-1,-1,-1,-1,-1};
char const *m_mage_sp[] = {"Spark","Minor Haste","Strength","Flame Cloud","Flame",
	"Minor Poison","Slow","Dumbfound","Stinking Cloud","Summon Beast",
	"Conflagration","Fireball","Weak Summoning","Web","Poison",
	"Ice Bolt","Slow Group","Major Haste","Firestorm","Summoning",
	"Shockstorm","Major Poison","Kill","Daemon","Major Blessing",
	"Major Summoning","Shockwave"};
char const *m_priest_sp[] = {"Minor Bless","Light Heal","Wrack","Stumble","Bless",
	"Curse","Wound","Summon Spirit","Disease","Heal",
	"Holy Scourge","Smite","Curse All","Sticks to Snakes","Martyr's Shield",
	"Bless All","Major Heal","Flamestrike","Summon Host","Revive Self",
	"Unholy Ravaging","Summon Guardian","Pestilence","Revive All","Avatar",
	"Divine Thud"};
// extra devices for maps
HBRUSH hbrush[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
HPEN hpen[6];
RECT store_map_window_rect = {0,0,0,0};
Boolean need_map_full_refresh = true;
HBITMAP map_gworld;
HBRUSH	bg[14];
HBRUSH map_brush[25];
HBITMAP map_bitmap[25];
unsigned char map_pats[256] = {1,1,2,2,2,7,7,7,7,7, ////
	7,7,7,7,7,7,7,7,3,3,
	3,3,3,3,3,3,3,3,3,3,
	3,3,5,5,5,5,5,5,5,5,
	5,5,5,5,5,5,4,4,4,4,
	4,4,4,4,4,4,4,4,4,0, // 50
	0,0,0,0,0,0,0,24,24,24,
	16,16,25,25,0,0,0,0,18,8,
	8,9,2,15,15,10,10,10,6,0,
	0,0,0,0,0,0,0,0,0,0,
	19,0,0,0,0,0,0,0,0,0, // 100
	23,0,0,0,0,0,0,0,0,0,
	0,0,0,11,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,12,0,0, // 150
	0,0,0,13,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,18,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0, // 200
	0,0,0,0,0,17,17,0,17,17,
	17,17,17,17,17,17,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0};// 250
unsigned char anim_map_pats[18] = {14,0,0,0,22, 0,0,21,20,21, 20,0,0,0,0, 0,0,0};
location town_map_adj;
short town_force = INVALID_TOWN;
location town_force_loc;
unsigned char map_graphic_placed[8][64]; // keeps track of what's been filled on map
Boolean kludge_force_full_refresh = false;
word_rect_type preset_words[9] = {{"Look",{4,366,54,389}},{"Name",{70,366,130,389}},{"Job",{136,366,186,389}},
	{"Buy",{4,389,54,412}},{"Sell",{70,389,120,412}},{"Record",{121,389,186,412}},
	{"Done",{210,389,270,412}},{"Go Back",{190,366,270,389}},
	{"Ask About...",{4,343,134,366}}};
word_rect_type store_words[50];
//Timer Special happened ?
BOOL timed_special_happened = false;
BOOL ghost_mode = false;
char talk_edit_string[40] = "";
unsigned char queue_position = 0; //timer queue position
